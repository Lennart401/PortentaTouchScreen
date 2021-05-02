#include <PortentaTouchScreen.h>

PortentaTouchScreen ts(5, A6, A4, 4);

void setup() {
    Serial.begin(115200);
}

void loop() {
    Point p = ts.getPoint();
    
    if (p.z() > ts.pressureThreshold) {
        Serial.println("Raw touch: " + String(p.x()) + " | " + String(p.y()) + " with pressure of " + p.z() + " ");

        int touchX = map(p.x(), 100, 1950, 0, 240);
        int touchY = map(p.y(), 100, 1950, 0, 320);
        Serial.println("mapped to: " + String(touchX) + " | " + String(touchY));
    }

    delay(1);
}