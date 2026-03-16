#include "mock_sensor.h"
#include <Arduino.h>

bool MockSensor::begin() {
    randomSeed(analogRead(0));
    Serial.println("[Sensor] Mock sensor initialized");
    return true;
}

SensorReading MockSensor::read() {
    SensorReading reading;
    reading.temperature = randomFloat(TEMP_MIN, TEMP_MAX);
    reading.humidity = randomFloat(HUMID_MIN, HUMID_MAX);
    reading.valid = true;
    return reading;
}

float MockSensor::randomFloat(float min, float max) {
    return min + (random(1000) / 1000.0f) * (max - min);
}
