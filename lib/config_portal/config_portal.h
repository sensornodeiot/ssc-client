#pragma once

#include <Arduino.h>
#include "config_store.h"

class ConfigPortal {
public:
    explicit ConfigPortal(ConfigStore& store);

    // Starts AP + web server; blocks forever
    void start(const char* apSsid, const char* apPassword);

private:
    void setupRoutes();
    void handleRoot();
    void handleApiConfig();
    void handleWiFiSave();
    void handleMqttSave();
    void handleOtaUpload();
    void handleOtaResult();
    void handleReboot();

    String escapeJson(const char* str);

    ConfigStore& store_;
    Config config_;
};

// Check if BOOT button is held to trigger config portal
bool shouldEnterPortal(uint8_t pin, uint32_t holdDurationMs = 2000);

// Convenience function
void startConfigPortal(ConfigStore& store, const char* apSsid, const char* apPassword);
