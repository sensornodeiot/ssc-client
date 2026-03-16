#include "wifi_manager.h"
#include <WiFi.h>

void WiFiManager::begin(const char* ssid, const char* password) {
    ssid_ = ssid;
    password_ = password;
    backoffMs_ = MIN_BACKOFF_MS;

    WiFi.mode(WIFI_STA);
    WiFi.setAutoReconnect(false);

    startConnect();
}

void WiFiManager::update() {
    switch (state_) {
        case WiFiState::Disconnected:
            if (ssid_ && millis() >= retryTime_) {
                startConnect();
            }
            break;

        case WiFiState::Connecting:
            checkConnection();
            break;

        case WiFiState::Connected:
            if (WiFi.status() != WL_CONNECTED) {
                Serial.println("[WiFi] Connection lost");
                state_ = WiFiState::Disconnected;
                scheduleRetry();
            }
            break;
    }
}

void WiFiManager::disconnect() {
    WiFi.disconnect();
    state_ = WiFiState::Disconnected;
    ssid_ = nullptr;
    password_ = nullptr;
}

void WiFiManager::startConnect() {
    if (!ssid_) return;

    Serial.printf("[WiFi] Connecting to %s\n", ssid_);
    WiFi.begin(ssid_, password_);
    state_ = WiFiState::Connecting;
    connectStartTime_ = millis();
}

void WiFiManager::checkConnection() {
    wl_status_t status = WiFi.status();

    if (status == WL_CONNECTED) {
        Serial.printf("[WiFi] Connected, IP: %s\n", WiFi.localIP().toString().c_str());
        state_ = WiFiState::Connected;
        backoffMs_ = MIN_BACKOFF_MS;
        return;
    }

    if (millis() - connectStartTime_ > CONNECT_TIMEOUT_MS) {
        Serial.printf("[WiFi] Connection timeout (status=%d)\n", status);
        WiFi.disconnect();
        state_ = WiFiState::Disconnected;
        scheduleRetry();
    }
}

void WiFiManager::scheduleRetry() {
    retryTime_ = millis() + backoffMs_;
    Serial.printf("[WiFi] Retry in %lu ms\n", backoffMs_);

    backoffMs_ = backoffMs_ * 2;
    if (backoffMs_ > MAX_BACKOFF_MS) {
        backoffMs_ = MAX_BACKOFF_MS;
    }
}
