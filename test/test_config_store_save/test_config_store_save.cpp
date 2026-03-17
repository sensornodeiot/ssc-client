#include <unity.h>

// Include the stub Preferences.h (found in this directory) before config_store
#include "Preferences.h"
#include "config_store.h"

// Pull in the real implementation
#include "../../lib/config_store/config_store.cpp"

static ConfigStore store;
static Config cfg;

void setUp(void) {
    Preferences::reset();
    cfg.clear();
}

void tearDown(void) {}

// --- Helper: populate all config fields ---
static void fill_config(Config& c) {
    strcpy(c.wifi_ssid, "MyNetwork");
    strcpy(c.wifi_pass, "password123");
    strcpy(c.mqtt_host, "mqtt.example.com");
    c.mqtt_port = 1883;
    strcpy(c.mqtt_client_id, "device-001");
    strcpy(c.mqtt_username, "user");
    strcpy(c.mqtt_password, "pass");
    strcpy(c.tenant_id, "tenant-abc");
    strcpy(c.application_id, "app-xyz");
    c.publish_interval_ms = 10000;
}

// 1. NVS begin failure → save returns false
void test_save_fails_when_nvs_begin_fails(void) {
    fill_config(cfg);
    Preferences::begin_result = false;

    TEST_ASSERT_FALSE(store.save(cfg));
}

// 2. Happy path with all fields populated
void test_save_succeeds_with_valid_full_config(void) {
    fill_config(cfg);

    TEST_ASSERT_TRUE(store.save(cfg));
}

// 3. Empty strings + default numerics still succeed
void test_save_succeeds_with_empty_strings_and_default_numerics(void) {
    // cfg.clear() already done in setUp — all strings empty, port=1883, interval=10000

    TEST_ASSERT_TRUE(store.save(cfg));
}

// 4. putUShort failure → save returns false
void test_save_fails_when_putUShort_fails(void) {
    fill_config(cfg);
    Preferences::putUShort_result = 0;

    TEST_ASSERT_FALSE(store.save(cfg));
}

// 5. putULong failure → save returns false
void test_save_fails_when_putULong_fails(void) {
    fill_config(cfg);
    Preferences::putULong_result = 0;

    TEST_ASSERT_FALSE(store.save(cfg));
}

// 6. Single putString failure → save returns false
void test_save_fails_when_one_putString_fails(void) {
    fill_config(cfg);
    Preferences::putString_fail_on_call = 3;  // fail on 3rd putString call

    TEST_ASSERT_FALSE(store.save(cfg));
}

// 7. Portal WiFi-save scenario: only ssid+pass set, rest defaults
void test_save_wifi_only_portal_scenario(void) {
    strcpy(cfg.wifi_ssid, "HomeWiFi");
    strcpy(cfg.wifi_pass, "secret");
    // All other strings empty, numerics at defaults from clear()

    TEST_ASSERT_TRUE(store.save(cfg));
}

// 8. Empty-string escape hatch: even when putString returns 0,
//    empty strings pass via the || field[0]=='\0' check
void test_save_with_all_empty_strings_ignores_putString_failure(void) {
    // cfg is cleared (all strings empty)
    Preferences::putString_result = 0;  // putString always returns 0

    // Should still succeed because all string fields are empty
    TEST_ASSERT_TRUE(store.save(cfg));
}

// 9. &= doesn't short-circuit — all 10 put calls execute even after failure
void test_save_writes_all_fields_even_after_failure(void) {
    fill_config(cfg);
    Preferences::putString_fail_on_call = 1;  // fail on very first putString

    store.save(cfg);  // result doesn't matter here

    // 8 putString + 1 putUShort + 1 putULong = 10 total
    TEST_ASSERT_EQUAL_INT(8, Preferences::putString_calls);
    TEST_ASSERT_EQUAL_INT(1, Preferences::putUShort_calls);
    TEST_ASSERT_EQUAL_INT(1, Preferences::putULong_calls);
}

int main(int argc, char** argv) {
    UNITY_BEGIN();

    RUN_TEST(test_save_fails_when_nvs_begin_fails);
    RUN_TEST(test_save_succeeds_with_valid_full_config);
    RUN_TEST(test_save_succeeds_with_empty_strings_and_default_numerics);
    RUN_TEST(test_save_fails_when_putUShort_fails);
    RUN_TEST(test_save_fails_when_putULong_fails);
    RUN_TEST(test_save_fails_when_one_putString_fails);
    RUN_TEST(test_save_wifi_only_portal_scenario);
    RUN_TEST(test_save_with_all_empty_strings_ignores_putString_failure);
    RUN_TEST(test_save_writes_all_fields_even_after_failure);

    return UNITY_END();
}
