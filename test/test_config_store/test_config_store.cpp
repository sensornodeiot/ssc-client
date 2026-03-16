#include <Arduino.h>
#include <unity.h>
#include "config_store.h"

ConfigStore store;

void setUp(void) {
    store.clear();
}

void tearDown(void) {}

void test_store_begin_succeeds(void) {
    TEST_ASSERT_TRUE(store.begin());
}

void test_store_load_empty_returns_defaults(void) {
    Config cfg;

    TEST_ASSERT_TRUE(store.load(cfg));
    TEST_ASSERT_EQUAL(1883, cfg.mqtt_port);
    TEST_ASSERT_EQUAL(10000, cfg.publish_interval_ms);
    TEST_ASSERT_EQUAL_STRING("", cfg.wifi_ssid);
}

void test_store_save_and_load_wifi(void) {
    Config cfg;
    cfg.clear();
    strcpy(cfg.wifi_ssid, "TestNetwork");
    strcpy(cfg.wifi_pass, "TestPassword123");

    TEST_ASSERT_TRUE(store.save(cfg));

    Config loaded;
    TEST_ASSERT_TRUE(store.load(loaded));

    TEST_ASSERT_EQUAL_STRING("TestNetwork", loaded.wifi_ssid);
    TEST_ASSERT_EQUAL_STRING("TestPassword123", loaded.wifi_pass);
}

void test_store_save_and_load_mqtt(void) {
    Config cfg;
    cfg.clear();
    strcpy(cfg.mqtt_host, "mqtt.sensornodeiot.com");
    cfg.mqtt_port = 1883;
    strcpy(cfg.mqtt_client_id, "d0ed1dac-8a2e-4744-a823-5ea54b37ba16");
    strcpy(cfg.mqtt_username, "dev_1d89457ec9ae683f3adc16836851fbed");
    strcpy(cfg.mqtt_password, "o_BA4JtWk2kPXxfd7dxr53c7WhrX1uTe");
    strcpy(cfg.tenant_id, "dc1b368b-fde3-4559-8ef1-37543283233f");
    strcpy(cfg.application_id, "sensornode-uat");

    TEST_ASSERT_TRUE(store.save(cfg));

    Config loaded;
    TEST_ASSERT_TRUE(store.load(loaded));

    TEST_ASSERT_EQUAL_STRING("mqtt.sensornodeiot.com", loaded.mqtt_host);
    TEST_ASSERT_EQUAL(1883, loaded.mqtt_port);
    TEST_ASSERT_EQUAL_STRING("d0ed1dac-8a2e-4744-a823-5ea54b37ba16", loaded.mqtt_client_id);
    TEST_ASSERT_EQUAL_STRING("dev_1d89457ec9ae683f3adc16836851fbed", loaded.mqtt_username);
    TEST_ASSERT_EQUAL_STRING("o_BA4JtWk2kPXxfd7dxr53c7WhrX1uTe", loaded.mqtt_password);
    TEST_ASSERT_EQUAL_STRING("dc1b368b-fde3-4559-8ef1-37543283233f", loaded.tenant_id);
    TEST_ASSERT_EQUAL_STRING("sensornode-uat", loaded.application_id);
}

void test_store_clear_removes_data(void) {
    Config cfg;
    cfg.clear();
    strcpy(cfg.wifi_ssid, "ToBeCleared");

    store.save(cfg);
    store.clear();

    Config loaded;
    store.load(loaded);

    TEST_ASSERT_EQUAL_STRING("", loaded.wifi_ssid);
}

void test_store_full_config_roundtrip(void) {
    Config cfg;
    cfg.clear();

    // WiFi
    strcpy(cfg.wifi_ssid, "Sensornode");
    strcpy(cfg.wifi_pass, "SSN@IOT2025");

    // MQTT
    strcpy(cfg.mqtt_host, "mqtt.sensornodeiot.com");
    cfg.mqtt_port = 1883;
    strcpy(cfg.mqtt_client_id, "d0ed1dac-8a2e-4744-a823-5ea54b37ba16");
    strcpy(cfg.mqtt_username, "dev_1d89457ec9ae683f3adc16836851fbed");
    strcpy(cfg.mqtt_password, "o_BA4JtWk2kPXxfd7dxr53c7WhrX1uTe");
    strcpy(cfg.tenant_id, "dc1b368b-fde3-4559-8ef1-37543283233f");
    strcpy(cfg.application_id, "sensornode-uat");
    cfg.publish_interval_ms = 5000;

    TEST_ASSERT_TRUE(store.save(cfg));

    Config loaded;
    TEST_ASSERT_TRUE(store.load(loaded));

    TEST_ASSERT_TRUE(loaded.isValid());
    TEST_ASSERT_EQUAL(5000, loaded.publish_interval_ms);
}

void setup() {
    delay(2000);
    UNITY_BEGIN();

    RUN_TEST(test_store_begin_succeeds);
    RUN_TEST(test_store_load_empty_returns_defaults);
    RUN_TEST(test_store_save_and_load_wifi);
    RUN_TEST(test_store_save_and_load_mqtt);
    RUN_TEST(test_store_clear_removes_data);
    RUN_TEST(test_store_full_config_roundtrip);

    UNITY_END();
}

void loop() {}
