
#include <Arduino.h>
#include "config.h"
#include "ota_portal.h"

#define PIN_DS 25
#define PIN_SHCP 26
#define PIN_STCP 27

void shiftWrite(uint8_t data)
{
    digitalWrite(PIN_STCP, LOW);
    shiftOut(PIN_DS, PIN_SHCP, MSBFIRST, data);
    digitalWrite(PIN_STCP, HIGH);
}

void setup()
{
    Serial.begin(115200);
    delay(1000);
    pinMode(OTA_TRIGGER_PIN, INPUT_PULLUP);
    delay(100);

    if (digitalRead(OTA_TRIGGER_PIN) == LOW)
    {
        startOTAPortal(); // never returns
    }

    Serial.println("Normal boot");

    pinMode(PIN_DS, OUTPUT);
    pinMode(PIN_SHCP, OUTPUT);
    pinMode(PIN_STCP, OUTPUT);
    shiftWrite(0x00); // all relays off
}

void loop()
{
    for (uint8_t ch = 0; ch < 8; ch++)
    {
        shiftWrite(1 << ch);
        Serial.printf("CH%d ON\n", ch + 1);
        delay(3000);
        shiftWrite(0x00);
        Serial.printf("CH%d OFF\n", ch + 1);
        delay(2000);
    }
}
