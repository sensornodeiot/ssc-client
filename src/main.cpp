#include <Arduino.h>
#include <WiFi.h>
#include "config.h"
#include "config_store.h"
#include "config_portal.h"
#include "wifi_manager.h"
#include "ssc_client.h"
#include "ota_handler.h"

ConfigStore configStore;
Config config;

WiFiClient wifiClient;
WiFiManager wifiMgr;
SSCClient sscClient(wifiClient);
OtaHandler otaHandler;
bool otaAckChecked = false;

void setup()
{
    Serial.begin(115200);
    Serial.println("\n[Main] ESP32 SSC Client Starting");
    delay(2000);

    configStore.begin();
    configStore.load(config);

    // Enter config portal if:
    // 1. BOOT button held during reset, OR
    // 2. No valid configuration
    if (shouldEnterPortal(OTA_TRIGGER_PIN))
    {
        Serial.println("[Main] Config portal triggered by button");
        startConfigPortal(configStore, OTA_AP_SSID, OTA_AP_PASSWORD);
    }

    if (!config.isValid())
    {
        Serial.println("[Main] Config invalid, entering portal");
        startConfigPortal(configStore, OTA_AP_SSID, OTA_AP_PASSWORD);
    }

    Serial.println("[Main] Configuration loaded");
    Serial.printf("[Main] Firmware version: %s\n", FIRMWARE_VERSION);

    // Set MQTT command callback to route to OtaHandler
    sscClient.setCommandCallback([](const char* topic, uint8_t* payload, unsigned int len) {
        otaHandler.handleMessage(topic, payload, len);
    });

    // Start WiFi
    wifiMgr.begin(config.wifi_ssid, config.wifi_pass);

    // Configure SSC client
    SSCConfig sscConfig = {
        .host = config.mqtt_host,
        .port = config.mqtt_port,
        .clientId = config.mqtt_client_id,
        .username = config.mqtt_username,
        .password = config.mqtt_password,
        .tenantId = config.tenant_id,
        .applicationId = config.application_id};
    sscClient.begin(sscConfig);

    // Initialize OTA handler
    otaHandler.begin(&sscClient.getMqttClient(), sscClient.getAckTopic(), &configStore);
}

void loop()
{
    wifiMgr.update();

    // Only update SSC client when WiFi is connected
    if (wifiMgr.isConnected())
    {
        sscClient.update();

        // After first MQTT connect, check for pending OTA ACK
        if (sscClient.isConnected() && !otaAckChecked)
        {
            otaHandler.checkPendingAck();
            otaAckChecked = true;
        }

        // Drive OTA state machine
        otaHandler.update();
    }

    delay(1000);
}
