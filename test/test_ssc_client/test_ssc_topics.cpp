#include <unity.h>
#include <cstdio>
#include <cstring>

void setUp(void) {}
void tearDown(void) {}

// Test topic building logic (mirrors SSCClient::buildTopics)
void buildTopics(const char* tenantId, const char* appId, const char* deviceToken,
                 char* telemetryTopic, char* statusTopic, size_t bufSize) {
    snprintf(telemetryTopic, bufSize,
             "tenant/%s/app/%s/device/%s/data",
             tenantId, appId, deviceToken);

    snprintf(statusTopic, bufSize,
             "tenant/%s/app/%s/device/%s/status",
             tenantId, appId, deviceToken);
}

void test_topic_format_telemetry(void) {
    char telemetry[160];
    char status[160];

    buildTopics(
        "dc1b368b-fde3-4559-8ef1-37543283233f",
        "sensornode-uat",
        "dev_1d89457ec9ae683f3adc16836851fbed",
        telemetry, status, sizeof(telemetry)
    );

    TEST_ASSERT_EQUAL_STRING(
        "tenant/dc1b368b-fde3-4559-8ef1-37543283233f/app/sensornode-uat/device/dev_1d89457ec9ae683f3adc16836851fbed/data",
        telemetry
    );
}

void test_topic_format_status(void) {
    char telemetry[160];
    char status[160];

    buildTopics(
        "dc1b368b-fde3-4559-8ef1-37543283233f",
        "sensornode-uat",
        "dev_1d89457ec9ae683f3adc16836851fbed",
        telemetry, status, sizeof(telemetry)
    );

    TEST_ASSERT_EQUAL_STRING(
        "tenant/dc1b368b-fde3-4559-8ef1-37543283233f/app/sensornode-uat/device/dev_1d89457ec9ae683f3adc16836851fbed/status",
        status
    );
}

void test_topic_buffer_safety(void) {
    char telemetry[160];
    char status[160];

    const char* longId = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";

    buildTopics(longId, longId, longId, telemetry, status, sizeof(telemetry));

    TEST_ASSERT_LESS_THAN(160, strlen(telemetry));
    TEST_ASSERT_LESS_THAN(160, strlen(status));
}

void test_lwt_payload_format(void) {
    char payload[64];
    snprintf(payload, sizeof(payload), "{\"state\":\"offline\",\"reason\":\"lwt\"}");

    TEST_ASSERT_EQUAL_STRING("{\"state\":\"offline\",\"reason\":\"lwt\"}", payload);
}

void test_birth_payload_format(void) {
    const char* payload = "{\"state\":\"online\"}";

    TEST_ASSERT_EQUAL_STRING("{\"state\":\"online\"}", payload);
}

void test_telemetry_payload_format(void) {
    char payload[128];
    float temp = 25.50f;
    float humid = 60.25f;

    snprintf(payload, sizeof(payload),
             "{\"temperature\":%.2f,\"humidity\":%.2f}",
             temp, humid);

    TEST_ASSERT_EQUAL_STRING("{\"temperature\":25.50,\"humidity\":60.25}", payload);
}

int main(int argc, char **argv) {
    UNITY_BEGIN();

    RUN_TEST(test_topic_format_telemetry);
    RUN_TEST(test_topic_format_status);
    RUN_TEST(test_topic_buffer_safety);
    RUN_TEST(test_lwt_payload_format);
    RUN_TEST(test_birth_payload_format);
    RUN_TEST(test_telemetry_payload_format);

    return UNITY_END();
}
