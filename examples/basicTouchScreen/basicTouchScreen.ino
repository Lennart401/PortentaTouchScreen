#include <PortentaTouchScreen.h>

PortentaTouchScreen ts(5, A6, A4, 4);

void setup() {
    Serial.begin(115200);
}

void loop() {
    uint16_t touchX = ts.readTouchX();
    uint16_t touchY = ts.readTouchY();
    uint16_t pressure = ts.pressure();
    
    if (pressure > ts.pressureThreshold && touchX != 0 && touchY != 0) {
        Serial.println("Raw    touch: " + String(touchX) + " | " + String(touchY) + " with pressure of " + pressure + " ");

        touchX = map(touchX, 1000, 64000, 0, 240);
        touchY = map(touchY, 1000, 64000, 0, 320);
        Serial.println("mapped to: " + String(touchX) + " | " + String(touchY));
    }

    delay(200);
}