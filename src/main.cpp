#include <Arduino.h>
#include <WiFi.h>
#include "config.h"
#include "config_store.h"
#include "config_portal.h"
#include "wifi_manager.h"
#include "ssc_client.h"
#include "mock_sensor.h"

ConfigStore configStore;
Config config;

WiFiClient wifiClient;
WiFiManager wifiMgr;
SSCClient sscClient(wifiClient);
MockSensor sensor;

uint32_t lastPublish = 0;

bool shouldEnterPortal()
{
    pinMode(OTA_TRIGGER_PIN, INPUT_PULLUP);
    delay(50);
    return digitalRead(OTA_TRIGGER_PIN) == LOW;
}

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
    if (shouldEnterPortal())
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

    // Initialize sensor
    sensor.begin();

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
}

void loop()
{
    wifiMgr.update();

    // Only update SSC client when WiFi is connected
    if (wifiMgr.isConnected())
    {
        sscClient.update();
    }

    // Publish telemetry at interval
    if (sscClient.isConnected())
    {
        uint32_t now = millis();
        if (now - lastPublish >= config.publish_interval_ms)
        {
            lastPublish = now;

            SensorReading reading = sensor.read();
            if (reading.valid)
            {
                sscClient.publishTelemetry(reading.temperature, reading.humidity);
            }
        }
    }
}
