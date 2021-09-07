// CAUTION!!
//
// This example is untested at the moment. I have not gotten my hands on a
// portenta breakout board, which means I have had no chance to test it.

#include <PortentaTouchScreen.h>

#define YM D5
#define XM A6
#define YP A7 // CAUTION!!! this is a high-density-connector pin!
#define XP D4

// How to wire up the touchscreen, using the SeeedStudio TFT v2:
// TFT pin A0 (=YD/YM/Y-) --> portenta pin D5
// TFT pin A1 (=XL/XM/X-) --> portenta pin A6
// TFT pin A2 (=YU/YP/Y+) --> portenta pin A7 (high density connectors)
// TFT pin A3 (=XR/XP/X+) --> portenta pin D4

// PortentaTouchScreen(Y-, X-, Y+, X+);
PortentaTouchScreen ts(YM, XM, YP, XP);

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

    // add a delay to read the serial output
    delay(100);
}