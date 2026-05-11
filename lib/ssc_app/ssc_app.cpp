#include "ssc_app.h"
#include "config.h"
#include <Arduino.h>
#include <ArduinoJson.h>

SSCApp* SSCApp::instance_ = nullptr;

void SSCApp::begin() {
    instance_ = this;

    Serial.println("\n[App] ESP32 SSC Client Starting");
    delay(2000);

    configStore_.begin();
    configStore_.load(config_);

    if (shouldEnterPortal(OTA_TRIGGER_PIN)) {
        Serial.println("[App] Config portal triggered by button");
        startConfigPortal(configStore_, OTA_AP_SSID, OTA_AP_PASSWORD);
    }
    if (!config_.isValid()) {
        Serial.println("[App] Config invalid, entering portal");
        startConfigPortal(configStore_, OTA_AP_SSID, OTA_AP_PASSWORD);
    }

    Serial.println("[App] Configuration loaded");
    Serial.printf("[App] Firmware version: %s\n", FIRMWARE_VERSION);

    sscClient_.setCommandCallback(&SSCApp::onMqttCommand);

    wifiMgr_.begin(config_.wifi_ssid, config_.wifi_pass);

    SSCConfig sscConfig = {
        .host = config_.mqtt_host,
        .port = config_.mqtt_port,
        .clientId = config_.mqtt_client_id,
        .username = config_.mqtt_username,
        .password = config_.mqtt_password,
        .tenantId = config_.tenant_id,
        .applicationId = config_.application_id};
    sscClient_.begin(sscConfig);

    otaHandler_.begin(&sscClient_.getMqttClient(), sscClient_.getAckTopic(), &configStore_);
    cfgCmdHandler_.begin(&configStore_, &config_, &sscClient_);

    sensorBegin();
}

void SSCApp::update() {
    wifiMgr_.update();
    if (!wifiMgr_.isConnected()) return;

    sscClient_.update();

    if (sscClient_.isConnected() && !otaAckChecked_) {
        otaHandler_.checkPendingAck();
        otaAckChecked_ = true;
    }
    otaHandler_.update();

    // Periodic telemetry
    if (sscClient_.isConnected() &&
        (millis() - lastPublishMs_ >= config_.publish_interval_ms)) {
        char hex[SENSOR_HEX_BUF_LEN];
        if (sensorRead(hex, sizeof(hex))) {
            sscClient_.publishTelemetry(hex);
        }
        lastPublishMs_ = millis();
    }
}

void SSCApp::onMqttCommand(const char* topic, uint8_t* payload, unsigned int len) {
    if (instance_) instance_->dispatchCommand(topic, payload, len);
}

void SSCApp::dispatchCommand(const char* topic, uint8_t* payload, unsigned int len) {
    JsonDocument doc;
    if (deserializeJson(doc, payload, len)) {
        Serial.println("[App] Cmd JSON parse error");
        return;
    }
    const char* command = doc["command"];
    if (!command) return;

    if (strncmp(command, "ota:", 4) == 0) {
        otaHandler_.handleMessage(topic, payload, len);
    } else if (strncmp(command, "config:", 7) == 0) {
        cfgCmdHandler_.handleMessage(topic, payload, len);
    } else {
        Serial.printf("[App] Unknown command: %s\n", command);
    }
}
