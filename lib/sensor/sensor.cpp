#include "sensor.h"
#include <esp_random.h>
#include <inttypes.h>
#include <stdio.h>

// ═══════════════════════════════════════════════════════════════════
//  YOUR SENSOR — modify only this section
//  Steps:
//    1. Include your sensor library
//    2. Declare your sensor object / serial port
//    3. Init it in sensorBegin()
//    4. Read registers/values in doRead(), call r.add("name", value)
//       and set r.valid = true on success
//
//  Example (Modbus RTU):
//    #include <ModbusMaster.h>
//    ModbusMaster node;
//
//    bool sensorBegin() {
//        Serial2.begin(9600, SERIAL_8N1, 16, 17);
//        node.begin(1, Serial2);
//        return true;
//    }
//
//    static SensorReading doRead() {
//        SensorReading r;
//        if (node.readHoldingRegisters(0x00, 2) != node.ku8MBSuccess) return r;
//        r.add("temperature", node.getResponseBuffer(0) / 10.0f);
//        r.add("humidity",    node.getResponseBuffer(1) / 10.0f);
//        r.valid = true;
//        return r;
//    }
// ───────────────────────────────────────────────────────────────────

bool sensorBegin() {
    return true;
}

static SensorReading doRead() {
    SensorReading r;
    // Mock: temperature 20.0–35.0 °C, humidity 40.0–80.0 %
    float temp  = 20.0f + (esp_random() % 1500) / 100.0f;
    float humid = 40.0f + (esp_random() % 4000) / 100.0f;
    r.add("temperature", temp);
    r.add("humidity", humid);
    r.valid = true;
    return r;
}

// ═══════════════════════════════════════════════════════════════════
//  HEX ENCODING — do not modify below this line
//
//  Payload format:
//    [4-char random nonce][8-char IEEE 754 float per parameter]
//
//  Example — 2 params:
//    "a3f2" + "41cc0000" + "42708ccd"  →  "a3f241cc000042708ccd"
// ───────────────────────────────────────────────────────────────────

bool sensorRead(char* hexOut, size_t hexLen) {
    SensorReading r = doRead();
    if (!r.valid) return false;

    uint8_t offset = 0;

    uint16_t nonce = (uint16_t)(esp_random() & 0xFFFF);
    offset += snprintf(hexOut + offset, hexLen - offset, "%04x", nonce);

    for (uint8_t i = 0; i < r.count; i++) {
        uint32_t raw;
        memcpy(&raw, &r.params[i].value, sizeof(raw));
        offset += snprintf(hexOut + offset, hexLen - offset, "%08" PRIx32, raw);
    }

    return true;
}
