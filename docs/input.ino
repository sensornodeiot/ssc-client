#include <Arduino.h>
#include "config.h"
#include "ota_portal.h"

// All inputs have external 10K pull-down; switch to +3V3
// Pins 36, 39, 34, 35 are input-only (no internal pull-up/down)
const uint8_t INPUT_PINS[] = {4, 15, 36, 39, 34, 35, 32, 18};
const uint8_t INPUT_COUNT = sizeof(INPUT_PINS) / sizeof(INPUT_PINS[0]);

void setup()
{
    Serial.begin(115200);
    delay(1000);
    pinMode(OTA_TRIGGER_PIN, INPUT_PULLUP);
    delay(100); // debounce

    if (digitalRead(OTA_TRIGGER_PIN) == LOW)
    {
        startOTAPortal(); // never returns
    }

    Serial.println("Normal boot");

    for (uint8_t i = 0; i < INPUT_COUNT; i++)
    {
        pinMode(INPUT_PINS[i], INPUT);
    }
}

void loop()
{
    for (uint8_t i = 0; i < INPUT_COUNT; i++)
    {
        Serial.printf("IO%02d: %d  ", INPUT_PINS[i], digitalRead(INPUT_PINS[i]));
    }
    Serial.println();
    delay(500);
}
