#pragma once

struct SensorReading {
    float temperature;
    float humidity;
    bool valid;
};

class Sensor {
public:
    virtual ~Sensor() = default;
    virtual bool begin() = 0;
    virtual SensorReading read() = 0;
};
