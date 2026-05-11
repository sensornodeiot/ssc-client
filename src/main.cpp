#include <Arduino.h>
#include "ssc_app.h"

SSCApp app;

void setup() {
    Serial.begin(115200);
    app.begin();
}

void loop() {
    app.update();
    delay(10);
}
