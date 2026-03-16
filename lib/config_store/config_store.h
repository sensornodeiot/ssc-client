#pragma once

#include <cstdint>
#include <cstring>

struct Config {
    // WiFi
    char wifi_ssid[33];
    char wifi_pass[65];

    // MQTT broker
    char mqtt_host[65];
    uint16_t mqtt_port;

    // Device credentials
    char mqtt_client_id[65];
    char mqtt_username[65];
    char mqtt_password[65];

    // SSC identifiers
    char tenant_id[65];
    char application_id[65];

    // Timing
    uint32_t publish_interval_ms;

    void clear() {
        memset(this, 0, sizeof(Config));
        mqtt_port = 1883;
        publish_interval_ms = 10000;
    }

    bool hasWiFi() const {
        return wifi_ssid[0] != '\0';
    }

    bool hasMqtt() const {
        return mqtt_host[0] != '\0' &&
               mqtt_client_id[0] != '\0' &&
               mqtt_username[0] != '\0' &&
               mqtt_password[0] != '\0' &&
               tenant_id[0] != '\0' &&
               application_id[0] != '\0';
    }

    bool isValid() const {
        return hasWiFi() && hasMqtt();
    }
};

class ConfigStore {
public:
    bool begin();
    bool load(Config& config);
    bool save(const Config& config);
    bool clear();

private:
    static constexpr const char* NAMESPACE = "ssc";
};
