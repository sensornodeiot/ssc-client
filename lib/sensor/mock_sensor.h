#pragma once

#include "sensor.h"

class MockSensor : public Sensor {
public:
    bool begin() override;
    SensorReading read() override;

private:
    static constexpr float TEMP_MIN = 20.0f;
    static constexpr float TEMP_MAX = 30.0f;
    static constexpr float HUMID_MIN = 40.0f;
    static constexpr float HUMID_MAX = 70.0f;

    float randomFloat(float min, float max);
};
