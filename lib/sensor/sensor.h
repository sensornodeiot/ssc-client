#pragma once

#include <stdint.h>
#include <string.h>

#define SENSOR_MAX_PARAMS     16
#define SENSOR_PARAM_NAME_LEN 16
#define SENSOR_HEX_BUF_LEN   133  // 4 (nonce) + 16*8 (params) + 1 (null)

struct SensorParam {
    char  name[SENSOR_PARAM_NAME_LEN];
    float value;
};

struct SensorReading {
    SensorParam params[SENSOR_MAX_PARAMS];
    uint8_t     count;
    bool        valid;

    SensorReading() : count(0), valid(false) {}

    void add(const char* paramName, float paramValue) {
        if (count >= SENSOR_MAX_PARAMS) return;
        strncpy(params[count].name, paramName, SENSOR_PARAM_NAME_LEN - 1);
        params[count].name[SENSOR_PARAM_NAME_LEN - 1] = '\0';
        params[count].value = paramValue;
        count++;
    }
};

// Implement these two functions in sensor.cpp
bool sensorBegin();
bool sensorRead(char* hexOut, size_t hexLen);
