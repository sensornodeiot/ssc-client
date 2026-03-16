#include <Arduino.h>
#include <unity.h>
#include "mock_sensor.h"

MockSensor sensor;

void setUp(void) {}
void tearDown(void) {}

void test_sensor_begin_returns_true(void) {
    TEST_ASSERT_TRUE(sensor.begin());
}

void test_sensor_read_returns_valid(void) {
    sensor.begin();
    SensorReading reading = sensor.read();

    TEST_ASSERT_TRUE(reading.valid);
}

void test_sensor_temperature_in_range(void) {
    sensor.begin();

    for (int i = 0; i < 100; i++) {
        SensorReading reading = sensor.read();
        TEST_ASSERT_GREATER_OR_EQUAL(20.0f, reading.temperature);
        TEST_ASSERT_LESS_OR_EQUAL(30.0f, reading.temperature);
    }
}

void test_sensor_humidity_in_range(void) {
    sensor.begin();

    for (int i = 0; i < 100; i++) {
        SensorReading reading = sensor.read();
        TEST_ASSERT_GREATER_OR_EQUAL(40.0f, reading.humidity);
        TEST_ASSERT_LESS_OR_EQUAL(70.0f, reading.humidity);
    }
}

void test_sensor_readings_vary(void) {
    sensor.begin();

    SensorReading r1 = sensor.read();
    SensorReading r2 = sensor.read();
    SensorReading r3 = sensor.read();

    // At least one pair should differ (statistically almost certain)
    bool varies = (r1.temperature != r2.temperature) ||
                  (r2.temperature != r3.temperature) ||
                  (r1.humidity != r2.humidity) ||
                  (r2.humidity != r3.humidity);

    TEST_ASSERT_TRUE(varies);
}

void setup() {
    delay(2000);
    UNITY_BEGIN();

    RUN_TEST(test_sensor_begin_returns_true);
    RUN_TEST(test_sensor_read_returns_valid);
    RUN_TEST(test_sensor_temperature_in_range);
    RUN_TEST(test_sensor_humidity_in_range);
    RUN_TEST(test_sensor_readings_vary);

    UNITY_END();
}

void loop() {}
