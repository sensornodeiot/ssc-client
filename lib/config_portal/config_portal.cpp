#include "config_portal.h"
#include <WiFi.h>
#include <WebServer.h>
#include <Update.h>
#include "portal_html.h"

static WebServer server(80);
static ConfigPortal* instance = nullptr;

ConfigPortal::ConfigPortal(ConfigStore& store) : store_(store) {
    instance = this;
}

void ConfigPortal::start(const char* apSsid, const char* apPassword) {
    Serial.println("[Portal] Starting configuration portal");

    store_.load(config_);

    WiFi.mode(WIFI_AP);
    WiFi.softAP(apSsid, apPassword);
    Serial.printf("[Portal] AP: %s  IP: %s\n", apSsid, WiFi.softAPIP().toString().c_str());

    setupRoutes();
    server.begin();
    Serial.println("[Portal] Ready at http://192.168.4.1");

    while (true) {
        server.handleClient();
        delay(1);
    }
}

void ConfigPortal::setupRoutes() {
    // Serve SPA
    server.on("/", HTTP_GET, []() { instance->handleRoot(); });

    // API endpoint for config
    server.on("/api/config", HTTP_GET, []() { instance->handleApiConfig(); });

    // Form submissions
    server.on("/wifi", HTTP_POST, []() { instance->handleWiFiSave(); });
    server.on("/mqtt", HTTP_POST, []() { instance->handleMqttSave(); });

    // OTA
    server.on("/update", HTTP_POST,
        []() { instance->handleOtaResult(); },
        []() { instance->handleOtaUpload(); });

    // Reboot
    server.on("/reboot", HTTP_POST, []() { instance->handleReboot(); });
}

void ConfigPortal::handleRoot() {
    server.send_P(200, "text/html", PORTAL_HTML);
}

void ConfigPortal::handleApiConfig() {
    String json = "{";
    json += "\"wifi_ssid\":\"" + escapeJson(config_.wifi_ssid) + "\",";
    json += "\"wifi_pass\":\"" + escapeJson(config_.wifi_pass) + "\",";
    json += "\"mqtt_host\":\"" + escapeJson(config_.mqtt_host) + "\",";
    json += "\"mqtt_port\":" + String(config_.mqtt_port) + ",";
    json += "\"tenant_id\":\"" + escapeJson(config_.tenant_id) + "\",";
    json += "\"application_id\":\"" + escapeJson(config_.application_id) + "\",";
    json += "\"mqtt_client_id\":\"" + escapeJson(config_.mqtt_client_id) + "\",";
    json += "\"mqtt_username\":\"" + escapeJson(config_.mqtt_username) + "\",";
    json += "\"mqtt_password\":\"" + escapeJson(config_.mqtt_password) + "\",";
    json += "\"publish_interval_ms\":" + String(config_.publish_interval_ms);
    json += "}";

    server.send(200, "application/json", json);
}

String ConfigPortal::escapeJson(const char* str) {
    String result;
    while (*str) {
        char c = *str++;
        switch (c) {
            case '"':  result += "\\\""; break;
            case '\\': result += "\\\\"; break;
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            case '\t': result += "\\t"; break;
            default:   result += c; break;
        }
    }
    return result;
}

void ConfigPortal::handleWiFiSave() {
    String ssid = server.arg("ssid");
    String pass = server.arg("pass");

    strncpy(config_.wifi_ssid, ssid.c_str(), sizeof(config_.wifi_ssid) - 1);
    strncpy(config_.wifi_pass, pass.c_str(), sizeof(config_.wifi_pass) - 1);

    if (store_.save(config_)) {
        Serial.printf("[Portal] WiFi saved: %s\n", config_.wifi_ssid);
        server.send(200, "text/plain", "OK");
    } else {
        server.send(500, "text/plain", "Failed to save");
    }
}

void ConfigPortal::handleMqttSave() {
    strncpy(config_.mqtt_host, server.arg("host").c_str(), sizeof(config_.mqtt_host) - 1);
    config_.mqtt_port = server.arg("port").toInt();
    strncpy(config_.tenant_id, server.arg("tenant").c_str(), sizeof(config_.tenant_id) - 1);
    strncpy(config_.application_id, server.arg("app").c_str(), sizeof(config_.application_id) - 1);
    strncpy(config_.mqtt_client_id, server.arg("client").c_str(), sizeof(config_.mqtt_client_id) - 1);
    strncpy(config_.mqtt_username, server.arg("user").c_str(), sizeof(config_.mqtt_username) - 1);
    strncpy(config_.mqtt_password, server.arg("pass").c_str(), sizeof(config_.mqtt_password) - 1);

    if (config_.mqtt_port == 0) {
        config_.mqtt_port = 1883;
    }

    if (store_.save(config_)) {
        Serial.printf("[Portal] MQTT saved: %s:%d\n", config_.mqtt_host, config_.mqtt_port);
        server.send(200, "text/plain", "OK");
    } else {
        server.send(500, "text/plain", "Failed to save");
    }
}

void ConfigPortal::handleOtaUpload() {
    HTTPUpload& upload = server.upload();

    if (upload.status == UPLOAD_FILE_START) {
        Serial.printf("[OTA] Start: %s\n", upload.filename.c_str());
        if (!Update.begin(UPDATE_SIZE_UNKNOWN)) {
            Update.printError(Serial);
        }
    } else if (upload.status == UPLOAD_FILE_WRITE) {
        if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
            Update.printError(Serial);
        }
    } else if (upload.status == UPLOAD_FILE_END) {
        if (Update.end(true)) {
            Serial.printf("[OTA] Done: %u bytes\n", upload.totalSize);
        } else {
            Update.printError(Serial);
        }
    }
}

void ConfigPortal::handleOtaResult() {
    bool ok = !Update.hasError();
    if (ok) {
        server.send(200, "text/plain", "OK");
        delay(500);
        ESP.restart();
    } else {
        server.send(500, "text/plain", "Update failed");
    }
}

void ConfigPortal::handleReboot() {
    server.send(200, "text/plain", "Rebooting...");
    delay(500);
    ESP.restart();
}

void startConfigPortal(ConfigStore& store, const char* apSsid, const char* apPassword) {
    ConfigPortal portal(store);
    portal.start(apSsid, apPassword);
}
