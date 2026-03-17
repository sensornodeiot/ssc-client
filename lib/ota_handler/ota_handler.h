#pragma once

#include <Arduino.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "config_store.h"

enum class OtaHandlerState : uint8_t {
    Idle = 0,
    Downloading = 1,
    Verifying = 2,
    Installing = 3,
    PendingReboot = 4,
    PendingAck = 5
};

class OtaHandler {
public:
    void begin(PubSubClient* mqtt, const char* ackTopic, ConfigStore* store);

    // Called from MQTT callback when message arrives on /cmd
    void handleMessage(const char* topic, uint8_t* payload, unsigned int length);

    // Called from loop() — drives non-blocking state machine
    void update();

    // Called on boot — checks NVS for pending post-reboot ACK
    void checkPendingAck();

    bool isUpdating() const { return state_ != OtaHandlerState::Idle; }
    OtaHandlerState state() const { return state_; }

private:
    void handleOtaUpdate(JsonDocument& doc);
    void handleOtaRollback(JsonDocument& doc);

    bool downloadAndInstall(const char* url, const char* checksum, size_t fileSize);
    bool verifyChecksum(const uint8_t* computed, const char* expected);

    void sendAck(const char* commandId, const char* command,
                 const char* status, int progress = -1,
                 const char* version = nullptr, const char* error = nullptr);

    void persistBeforeReboot(const char* commandId, const char* version);

    PubSubClient* mqtt_ = nullptr;
    const char* ackTopic_ = nullptr;
    ConfigStore* store_ = nullptr;
    OtaHandlerState state_ = OtaHandlerState::Idle;

    // Current OTA context (valid during active update)
    char commandId_[65];
    char targetVersion_[20];
    char downloadUrl_[256];
    char checksum_[65];
    size_t fileSize_ = 0;
};
