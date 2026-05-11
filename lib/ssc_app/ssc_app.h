#pragma once

#include <WiFi.h>
#include "config.h"
#include "config_store.h"
#include "config_portal.h"
#include "wifi_manager.h"
#include "ssc_client.h"
#include "ota_handler.h"
#include "config_cmd_handler.h"
#include "sensor.h"

class SSCApp {
public:
    SSCApp() : sscClient_(wifiClient_) {}

    // Optional. Wire a sensor to enable periodic telemetry.
    // If unset, the device still connects, handles OTA, and accepts
    // remote config commands; it just doesn't publish telemetry.
    void setSensor(Sensor* sensor) { userSensor_ = sensor; }

    // Loads NVS config, may launch portal, brings WiFi + MQTT up,
    // initializes OTA and config-command handlers.
    void begin();

    // Drive the state machines and the telemetry scheduler.
    // Call from Arduino loop().
    void update();

private:
    static void onMqttCommand(const char* topic, uint8_t* payload, unsigned int len);
    static SSCApp* instance_;

    void dispatchCommand(const char* topic, uint8_t* payload, unsigned int len);

    ConfigStore configStore_;
    Config config_;
    WiFiClient wifiClient_;
    WiFiManager wifiMgr_;
    SSCClient sscClient_;
    OtaHandler otaHandler_;
    ConfigCmdHandler cfgCmdHandler_;

    Sensor* userSensor_ = nullptr;
    bool otaAckChecked_ = false;
    uint32_t lastPublishMs_ = 0;
};
