#include <unity.h>
#include <cstring>
#include <cstdint>

// Inline Config struct for native testing (mirrors config_store.h)
struct Config {
    char wifi_ssid[33];
    char wifi_pass[65];
    char mqtt_host[65];
    uint16_t mqtt_port;
    char mqtt_client_id[65];
    char mqtt_username[65];
    char mqtt_password[65];
    char tenant_id[65];
    char application_id[65];
    uint32_t publish_interval_ms;

    void clear() {
        memset(this, 0, sizeof(Config));
        mqtt_port = 1883;
        publish_interval_ms = 10000;
    }

    bool hasWiFi() const { return wifi_ssid[0] != '\0'; }

    bool hasMqtt() const {
        return mqtt_host[0] != '\0' &&
               mqtt_client_id[0] != '\0' &&
               mqtt_username[0] != '\0' &&
               mqtt_password[0] != '\0' &&
               tenant_id[0] != '\0' &&
               application_id[0] != '\0';
    }

    bool isValid() const { return hasWiFi() && hasMqtt(); }
};

void setUp(void) {}
void tearDown(void) {}

void test_config_clear_sets_defaults(void) {
    Config cfg;
    cfg.clear();

    TEST_ASSERT_EQUAL(1883, cfg.mqtt_port);
    TEST_ASSERT_EQUAL(10000, cfg.publish_interval_ms);
    TEST_ASSERT_EQUAL_STRING("", cfg.wifi_ssid);
    TEST_ASSERT_EQUAL_STRING("", cfg.mqtt_host);
}

void test_config_hasWiFi_false_when_empty(void) {
    Config cfg;
    cfg.clear();

    TEST_ASSERT_FALSE(cfg.hasWiFi());
}

void test_config_hasWiFi_true_when_set(void) {
    Config cfg;
    cfg.clear();
    strcpy(cfg.wifi_ssid, "TestNetwork");

    TEST_ASSERT_TRUE(cfg.hasWiFi());
}

void test_config_hasMqtt_false_when_incomplete(void) {
    Config cfg;
    cfg.clear();
    strcpy(cfg.mqtt_host, "mqtt.example.com");

    TEST_ASSERT_FALSE(cfg.hasMqtt());
}

void test_config_hasMqtt_true_when_complete(void) {
    Config cfg;
    cfg.clear();
    strcpy(cfg.mqtt_host, "mqtt.sensornodeiot.com");
    strcpy(cfg.mqtt_client_id, "device-123");
    strcpy(cfg.mqtt_username, "dev_token");
    strcpy(cfg.mqtt_password, "secret");
    strcpy(cfg.tenant_id, "dc1b368b-fde3-4559-8ef1-37543283233f");
    strcpy(cfg.application_id, "sensornode-uat");

    TEST_ASSERT_TRUE(cfg.hasMqtt());
}

void test_config_isValid_requires_wifi_and_mqtt(void) {
    Config cfg;
    cfg.clear();

    TEST_ASSERT_FALSE(cfg.isValid());

    strcpy(cfg.wifi_ssid, "TestNetwork");
    TEST_ASSERT_FALSE(cfg.isValid());

    strcpy(cfg.mqtt_host, "mqtt.sensornodeiot.com");
    strcpy(cfg.mqtt_client_id, "device-123");
    strcpy(cfg.mqtt_username, "dev_token");
    strcpy(cfg.mqtt_password, "secret");
    strcpy(cfg.tenant_id, "dc1b368b-fde3-4559-8ef1-37543283233f");
    strcpy(cfg.application_id, "sensornode-uat");

    TEST_ASSERT_TRUE(cfg.isValid());
}

int main(int argc, char **argv) {
    UNITY_BEGIN();

    RUN_TEST(test_config_clear_sets_defaults);
    RUN_TEST(test_config_hasWiFi_false_when_empty);
    RUN_TEST(test_config_hasWiFi_true_when_set);
    RUN_TEST(test_config_hasMqtt_false_when_incomplete);
    RUN_TEST(test_config_hasMqtt_true_when_complete);
    RUN_TEST(test_config_isValid_requires_wifi_and_mqtt);

    return UNITY_END();
}
