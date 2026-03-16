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
    void handleWiFiPage();
    void handleWiFiSave();
    void handleMqttPage();
    void handleMqttSave();
    void handleOtaPage();
    void handleOtaUpload();
    void handleOtaResult();
    void handleReboot();

    void sendPage(const char* title, const String& body);
    String makeInput(const char* label, const char* name, const char* value,
                     const char* type = "text", int maxlen = 64);

    ConfigStore& store_;
    Config config_;
};

// Convenience function matching old API
void startConfigPortal(ConfigStore& store, const char* apSsid, const char* apPassword);
