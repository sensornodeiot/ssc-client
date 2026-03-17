#include "ota_handler.h"
#include "config.h"
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <Update.h>
#include <esp_ota_ops.h>
#include <mbedtls/sha256.h>

void OtaHandler::begin(PubSubClient* mqtt, const char* ackTopic, ConfigStore* store) {
    mqtt_ = mqtt;
    ackTopic_ = ackTopic;
    store_ = store;
    state_ = OtaHandlerState::Idle;

    memset(commandId_, 0, sizeof(commandId_));
    memset(targetVersion_, 0, sizeof(targetVersion_));
    memset(downloadUrl_, 0, sizeof(downloadUrl_));
    memset(checksum_, 0, sizeof(checksum_));
    fileSize_ = 0;

    Serial.println("[OTA] Handler initialized");
}

void OtaHandler::handleMessage(const char* topic, uint8_t* payload, unsigned int length) {
    Serial.printf("[OTA] Message received (%u bytes)\n", length);

    JsonDocument doc;
    DeserializationError err = deserializeJson(doc, payload, length);
    if (err) {
        Serial.printf("[OTA] JSON parse error: %s\n", err.c_str());
        return;
    }

    const char* command = doc["command"];
    if (!command) {
        Serial.println("[OTA] No 'command' field in message");
        return;
    }

    if (strcmp(command, "ota:update") == 0) {
        handleOtaUpdate(doc);
    } else if (strcmp(command, "ota:rollback") == 0) {
        handleOtaRollback(doc);
    } else {
        Serial.printf("[OTA] Unknown command: %s\n", command);
    }
}

void OtaHandler::update() {
    // Currently the OTA process is synchronous (blocking during download).
    // This method is reserved for future non-blocking state machine expansion.
}

void OtaHandler::handleOtaUpdate(JsonDocument& doc) {
    if (state_ != OtaHandlerState::Idle) {
        const char* cmdId = doc["commandId"] | "unknown";
        sendAck(cmdId, "ota:update", "failed", -1, nullptr, "Update already in progress");
        return;
    }

    // Extract fields — parameters are nested under "parameters" object
    const char* cmdId = doc["commandId"];
    JsonObject params = doc["parameters"];
    const char* version = params["version"];
    const char* url = params["downloadUrl"];
    const char* chk = params["checksum"];
    size_t fSize = params["fileSize"] | 0;

    if (!cmdId || !version || !url || !chk || fSize == 0) {
        sendAck(cmdId ? cmdId : "unknown", "ota:update", "failed",
                -1, nullptr, "Missing required fields");
        return;
    }

    // Store context
    strncpy(commandId_, cmdId, sizeof(commandId_) - 1);
    strncpy(targetVersion_, version, sizeof(targetVersion_) - 1);
    strncpy(downloadUrl_, url, sizeof(downloadUrl_) - 1);
    strncpy(checksum_, chk, sizeof(checksum_) - 1);
    fileSize_ = fSize;

    Serial.printf("[OTA] Update command: v%s, %u bytes\n", targetVersion_, fileSize_);

    // Send downloading ACK
    state_ = OtaHandlerState::Downloading;
    sendAck(commandId_, "ota:update", "downloading", 0);

    // Execute download and install (blocking)
    bool success = downloadAndInstall(downloadUrl_, checksum_, fileSize_);

    if (success) {
        state_ = OtaHandlerState::Installing;
        sendAck(commandId_, "ota:update", "installing", 100);

        // Persist state for post-reboot ACK
        persistBeforeReboot(commandId_, targetVersion_);

        Serial.println("[OTA] Rebooting to apply update...");
        delay(500);
        ESP.restart();
    } else {
        state_ = OtaHandlerState::Idle;
    }
}

void OtaHandler::handleOtaRollback(JsonDocument& doc) {
    const char* cmdId = doc["commandId"] | "unknown";

    Serial.println("[OTA] Rollback command received");

    const esp_partition_t* previous = esp_ota_get_next_update_partition(nullptr);
    if (!previous) {
        sendAck(cmdId, "ota:rollback", "failed", -1, nullptr, "No previous partition available");
        return;
    }

    esp_err_t err = esp_ota_set_boot_partition(previous);
    if (err != ESP_OK) {
        char errMsg[64];
        snprintf(errMsg, sizeof(errMsg), "Failed to set boot partition: %s", esp_err_to_name(err));
        sendAck(cmdId, "ota:rollback", "failed", -1, nullptr, errMsg);
        return;
    }

    sendAck(cmdId, "ota:rollback", "ok");
    Serial.println("[OTA] Rollback set, rebooting...");
    delay(500);
    ESP.restart();
}

bool OtaHandler::downloadAndInstall(const char* url, const char* checksum, size_t fileSize) {
    WiFiClientSecure secureClient;
    secureClient.setInsecure();  // TODO: Add proper CA cert for production

    HTTPClient http;
    Serial.printf("[OTA] Downloading: %s\n", url);

    if (!http.begin(secureClient, url)) {
        sendAck(commandId_, "ota:update", "failed", -1, nullptr, "HTTP connection failed");
        return false;
    }

    int httpCode = http.GET();
    if (httpCode != HTTP_CODE_OK) {
        char errMsg[64];
        snprintf(errMsg, sizeof(errMsg), "HTTP error: %d", httpCode);
        sendAck(commandId_, "ota:update", "failed", -1, nullptr, errMsg);
        http.end();
        return false;
    }

    int contentLength = http.getSize();
    if (contentLength > 0 && (size_t)contentLength != fileSize) {
        Serial.printf("[OTA] Warning: Content-Length %d != expected %u\n", contentLength, fileSize);
    }

    // Initialize SHA-256 context
    mbedtls_sha256_context sha256Ctx;
    mbedtls_sha256_init(&sha256Ctx);
    mbedtls_sha256_starts(&sha256Ctx, 0);

    // Begin OTA update
    if (!Update.begin(fileSize)) {
        sendAck(commandId_, "ota:update", "failed", -1, nullptr, "Update.begin() failed");
        mbedtls_sha256_free(&sha256Ctx);
        http.end();
        return false;
    }

    // Stream download in chunks
    WiFiClient* stream = http.getStreamPtr();
    uint8_t buf[2048];
    size_t totalRead = 0;
    int lastProgress = 0;

    while (totalRead < fileSize) {
        // Keep MQTT alive during download
        if (mqtt_) mqtt_->loop();

        size_t available = stream->available();
        if (available == 0) {
            if (!stream->connected()) break;
            delay(10);
            continue;
        }

        size_t toRead = min(available, sizeof(buf));
        size_t bytesRead = stream->readBytes(buf, toRead);
        if (bytesRead == 0) break;

        // Write to flash
        size_t written = Update.write(buf, bytesRead);
        if (written != bytesRead) {
            sendAck(commandId_, "ota:update", "failed", -1, nullptr, "Flash write error");
            Update.abort();
            mbedtls_sha256_free(&sha256Ctx);
            http.end();
            return false;
        }

        // Update hash
        mbedtls_sha256_update(&sha256Ctx, buf, bytesRead);
        totalRead += bytesRead;

        // Send progress ACKs at 25% intervals
        int progress = (int)((totalRead * 100) / fileSize);
        if (progress / 25 > lastProgress / 25) {
            sendAck(commandId_, "ota:update", "downloading", progress);
            lastProgress = progress;
        }
    }

    http.end();

    if (totalRead != fileSize) {
        char errMsg[80];
        snprintf(errMsg, sizeof(errMsg), "Download incomplete: %u/%u bytes", totalRead, fileSize);
        sendAck(commandId_, "ota:update", "failed", -1, nullptr, errMsg);
        Update.abort();
        mbedtls_sha256_free(&sha256Ctx);
        return false;
    }

    // Verify checksum before finalizing
    state_ = OtaHandlerState::Verifying;
    uint8_t hash[32];
    mbedtls_sha256_finish(&sha256Ctx, hash);
    mbedtls_sha256_free(&sha256Ctx);

    if (!verifyChecksum(hash, checksum)) {
        sendAck(commandId_, "ota:update", "failed", -1, nullptr, "Checksum verification failed");
        Update.abort();
        return false;
    }

    // Finalize update
    if (!Update.end(true)) {
        sendAck(commandId_, "ota:update", "failed", -1, nullptr, "Update.end() failed");
        return false;
    }

    // Explicitly set boot partition to the newly written OTA slot
    // After Update.end(), the running partition is still the old one,
    // so get_next_update_partition returns the one we just wrote to
    const esp_partition_t* updatePartition = esp_ota_get_next_update_partition(nullptr);
    if (updatePartition) {
        esp_err_t err = esp_ota_set_boot_partition(updatePartition);
        if (err != ESP_OK) {
            char errMsg[80];
            snprintf(errMsg, sizeof(errMsg), "Failed to set boot partition: %s", esp_err_to_name(err));
            sendAck(commandId_, "ota:update", "failed", -1, nullptr, errMsg);
            return false;
        }
        Serial.printf("[OTA] Boot partition set to: %s\n", updatePartition->label);
    }

    Serial.println("[OTA] Download and verification complete");
    return true;
}

bool OtaHandler::verifyChecksum(const uint8_t* computed, const char* expected) {
    char computedHex[65];
    for (int i = 0; i < 32; i++) {
        snprintf(&computedHex[i * 2], 3, "%02x", computed[i]);
    }
    computedHex[64] = '\0';

    Serial.printf("[OTA] Checksum computed: %s\n", computedHex);
    Serial.printf("[OTA] Checksum expected: %s\n", expected);

    return strcasecmp(computedHex, expected) == 0;
}

void OtaHandler::sendAck(const char* commandId, const char* command,
                          const char* status, int progress,
                          const char* version, const char* error) {
    JsonDocument doc;
    doc["commandId"] = commandId;
    doc["command"] = command;
    doc["status"] = status;

    if (progress >= 0) doc["progress"] = progress;
    if (version)       doc["firmwareVersion"] = version;
    if (error)         doc["error"] = error;

    char buffer[512];
    serializeJson(doc, buffer, sizeof(buffer));

    if (mqtt_ && mqtt_->connected()) {
        mqtt_->publish(ackTopic_, buffer);
    }

    Serial.printf("[OTA] ACK: %s\n", buffer);
}

void OtaHandler::persistBeforeReboot(const char* commandId, const char* version) {
    OtaState otaState;
    otaState.clear();
    strncpy(otaState.commandId, commandId, sizeof(otaState.commandId) - 1);
    strncpy(otaState.targetVersion, version, sizeof(otaState.targetVersion) - 1);
    otaState.state = 2;  // PendingAck

    if (store_->saveOtaState(otaState)) {
        Serial.println("[OTA] State persisted to NVS");
    } else {
        Serial.println("[OTA] WARNING: Failed to persist state to NVS");
    }
}

void OtaHandler::checkPendingAck() {
    if (!store_) return;

    OtaState otaState;
    if (!store_->loadOtaState(otaState)) return;
    if (otaState.state != 2) return;  // Not pending ACK

    Serial.printf("[OTA] Post-reboot: pending ACK for cmd %s, target v%s\n",
                  otaState.commandId, otaState.targetVersion);

    // Mark the current app as valid to prevent automatic rollback
    esp_ota_mark_app_valid_cancel_rollback();

    if (strcmp(FIRMWARE_VERSION, otaState.targetVersion) == 0) {
        sendAck(otaState.commandId, "ota:update", "ok", -1, FIRMWARE_VERSION);
        Serial.println("[OTA] Update confirmed successful");
    } else {
        char errMsg[80];
        snprintf(errMsg, sizeof(errMsg), "Version mismatch: running %s, expected %s",
                 FIRMWARE_VERSION, otaState.targetVersion);
        sendAck(otaState.commandId, "ota:update", "failed", -1, nullptr, errMsg);
        Serial.println("[OTA] Update verification failed - version mismatch");
    }

    store_->clearOtaState();
}
