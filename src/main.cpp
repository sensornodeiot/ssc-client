#include <Arduino.h>
#include "ssc_app.h"
// #include "my_sensor.h"   // Your Sensor subclass header

SSCApp app;
// MySensor sensor;          // Instantiate your sensor

void setup() {
    Serial.begin(115200);
    // app.setSensor(&sensor); // Uncomment to enable periodic telemetry
    app.begin();
}

void loop() {
    app.update();
    delay(10);
}
