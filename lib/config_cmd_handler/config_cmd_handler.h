#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>
#include "config_store.h"

class SSCClient;

class ConfigCmdHandler {
public:
    static constexpr uint32_t MIN_INTERVAL_MS = 1000;
    static constexpr uint32_t MAX_INTERVAL_MS = 3600000;

    void begin(ConfigStore* store, Config* config, SSCClient* client);

    // Called from MQTT callback when message arrives on /cmd
    void handleMessage(const char* topic, uint8_t* payload, unsigned int length);

private:
    void handleSetInterval(JsonDocument& doc);
    void sendAck(const char* commandId, const char* command,
                 const char* status, const char* error = nullptr);

    ConfigStore* store_ = nullptr;
    Config* config_ = nullptr;
    SSCClient* client_ = nullptr;
};
