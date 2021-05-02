// (c) 2021 Lennart Popkes
// Distributed under MIT License

#include <limits.h>
#include <Arduino.h>
#include <stdint.h>
#include "hal/analogin_api.h"
#include "hal/gpio_api.h"
#include "PortentaTouchScreen.h"
#include "pinDefinitions.h"

// *********************************************************************************************************************
// Point
// *********************************************************************************************************************
Point::Point(void) {
    mx = my = 0;
}

// *********************************************************************************************************************
Point::Point(int x0, int y0, int z0) {
    mx = x0;
    my = y0;
    mz = z0;
}

// *********************************************************************************************************************
bool Point::operator==(Point p1) {
    return ((p1.x() == mx) && (p1.y() == my) && (p1.z() == mz));
}

// *********************************************************************************************************************
bool Point::operator!=(Point p1) {
    return ((p1.x() != mx) || (p1.y() != my) || (p1.z() != mz));
}

// *********************************************************************************************************************
// Portenta Touch Screen 
// *********************************************************************************************************************
PortentaTouchScreen::PortentaTouchScreen(pin_size_t ym, pin_size_t xm, pin_size_t yp, pin_size_t xp) {
    ymPin = digitalPinToPinName(ym);
    xpPin = digitalPinToPinName(xp);
    xmPin = digitalPinToPinName(xm);
    ypPin = digitalPinToPinName(yp);
}

// *********************************************************************************************************************
uint16_t PortentaTouchScreen::readTouchX() {
    gpio_init_in(&ymInOut, ymPin);
    analogin_init(&ypIn, ypPin);

    // apply potential across x-plate
    gpio_init_out_ex(&xpInOut, xpPin, 1);
    gpio_init_out_ex(&xmOut, xmPin, 0);

    // read yp-in twice
    uint16_t ypSample0 = analogin_read_u16(&ypIn) >> 6;
    uint16_t ypSample1 = analogin_read_u16(&ypIn) >> 6;

    // this will be the return value, keep it at 0 (= touch event invalid) for now
    uint16_t result = 0;
    // allow a small amount of noise (2^6 * 2), otherwise keep the result at 0
    if (ypSample0 - ypSample1 > -ALLOWED_NOISE && ypSample0 - ypSample1 < ALLOWED_NOISE) {
        result = (ypSample0 + ypSample1) >> 1; // average the two samples
    }

    // clean up
    gpio_free(&ymInOut);
    analogin_free(&ypIn);

    gpio_write(&xpInOut, 0); gpio_free(&xpInOut);
    gpio_write(&xmOut, 0); gpio_free(&xmOut);

    return result;
}

// *********************************************************************************************************************
uint16_t PortentaTouchScreen::readTouchY() {
    // x-lines are low, xm is input
    gpio_init_in(&xpInOut, xpPin);
    analogin_init(&xmIn, xmPin);

    // apply potential across y-plate
    gpio_init_out_ex(&ypOut, ypPin, 1);
    gpio_init_out_ex(&ymInOut, ymPin, 0);

    // read xm-in twice
    uint16_t xmSample0 = analogin_read_u16(&xmIn) >> 6;
    uint16_t xmSample1 = analogin_read_u16(&xmIn) >> 6;

    // this will be the return value, keep it at 0 (= touch event invalid) for now
    uint16_t result = 0;
    // allow a small amount of noise (2^6 * 2), otherwise keep the result at 0
    if (xmSample0 - xmSample1 > -ALLOWED_NOISE && xmSample0 - xmSample1 < ALLOWED_NOISE) {
        result = (xmSample0 + xmSample1) >> 1; // average the two samples
    }

    // clean up
    gpio_free(&xpInOut);
    analogin_free(&xmIn);

    gpio_write(&ymInOut, 0); gpio_free(&ymInOut);
    gpio_write(&ypOut, 0); gpio_free(&ypOut);

    return result;
}

// *********************************************************************************************************************
uint16_t PortentaTouchScreen::pressure() {
    // set x+ to ground
    gpio_init_out_ex(&xpInOut, xpPin, 0);
    // set y- to Vcc
    gpio_init_out_ex(&ymInOut, ymPin, 1);

    // read the x- and y+ values
    analogin_init(&xmIn, xmPin);
    analogin_init(&ypIn, ypPin);

    uint16_t z1 = analogin_read_u16(&xmIn);
    uint16_t z2 = analogin_read_u16(&ypIn);

    // cleanup
    analogin_free(&xmIn);
    analogin_free(&ypIn);
    
    gpio_write(&xpInOut, 0); gpio_free(&xpInOut);
    gpio_write(&ymInOut, 0); gpio_free(&ymInOut);

    return USHRT_MAX - (z2 - z1);
}

// *********************************************************************************************************************
Point PortentaTouchScreen::getPoint() {
    int x, y, z = 1;
    int samples[NUMSAMPLES];
    bool valid = true;

    // ---------- read x ----------
    // pinMode INPUT for y- (inactive), y+ (read)
    gpio_init_in(&ymInOut, ymPin);
    analogin_init(&ypIn, ypPin);
    
    // pinMode OUTPUT for x-, x+ AND write x+ = HIGH, x- = LOW
    gpio_init_out_ex(&xpInOut, xpPin, 1);
    gpio_init_out_ex(&xmOut, xmPin, 0);

    // read 2 samples
    for (int i = 0; i < NUMSAMPLES; i++) {
        #if AVERAGE
        samples[i] = analogin_read_average(&ypIn);
        #else
        samples[i] = analogin_read_u16(&ypIn) >> 6;
        #endif
    }

    // check for noise on the two samples
    #if !ALLOWED_NOISE
    if (samples[0] != samples[1]) {
        valid = false;
    }
    #else
    int diff = samples[0] > samples[1] ? samples[0] - samples[1] : samples[1] - samples[0];
    if (diff > ALLOWED_NOISE) {
        valid = false;
    }
    #endif
    // set x
    x = (samples[0] + samples[1]);

    // clean up all pins for reading x
    gpio_free(&ymInOut);
    analogin_free(&ypIn);
    gpio_write(&xpInOut, 0); gpio_free(&xpInOut);
    gpio_write(&xmOut, 0); gpio_free(&xmOut);

    // ---------- read y ----------
    // pinMode INPUT for x- (read), x+ (inactive)
    gpio_init_in(&xpInOut, xpPin);
    analogin_init(&xmIn, xmPin);

    // pinMode OUTPUT for y+, y- pins AND write y+ = HIGH, y- = LOW
    gpio_init_out_ex(&ypOut, ypPin, 1);
    gpio_init_out_ex(&ymInOut, ymPin, 0);

    // read 2 samples
    for (int i = 0; i < NUMSAMPLES; i++) {
        #if AVERAGE
        samples[i] = analogin_read_average(&xmIn);
        #else
        samples[i] = analogin_read_u16(&xmIn) >> 6;
        #endif
    }

    // check for noise on the two samples
    #if !ALLOWED_NOISE
    if (samples[0] != samples[1]) {
        valid = false;
    }
    #else
    diff = samples[0] > samples[1] ? samples[0] - samples[1] : samples[1] - samples[0];
    if (diff > ALLOWED_NOISE) {
        valid = false;
    }
    #endif
    // set y
    y = (samples[0] + samples[1]);

    // only check for pressure if the input is even valid, otherwise save the computing power
    if (valid) {
        // only partial cleanup here since we can reuse two already initialized pins
        gpio_free(&xpInOut);
        gpio_write(&ypOut, 0); gpio_free(&ypOut);

        // re-assign x+ and y+
        gpio_init_out_ex(&xpInOut, xpPin, 0);   // set x+ to gnd
        analogin_init(&ypIn, ypPin);            // set y+ to input

        gpio_write(&ymInOut, 1);                // set y- to Vcc (3.3V)
        //analogin_init(&xmIn, xmPin); has already been done earlier

        int z1 = analogin_read_u16(&xmIn) >> 6;
        int z2 = analogin_read_u16(&ypIn) >> 6;
        float rtouch = 0;

        rtouch  = z2;
        rtouch /= z1;
        rtouch -= 1;
        rtouch *= (2046 - x) / 2;
        rtouch *= RXPLATE;
        rtouch /= 1024;
        z = rtouch;

        // something you get some issues, ignore those
        if (z > 0xFFFF) z = 0;

        // final cleanup
        analogin_free(&xmIn);
        analogin_free(&ypIn);
        gpio_write(&ymInOut, 0); gpio_free(&ymInOut);
        gpio_write(&xpInOut, 0); gpio_free(&xpInOut);
    } 
    // if the input was not valid, set z to 0 and clean up.
    else {
        z = 0;

        // clean up
        gpio_free(&xpInOut);
        analogin_free(&xmIn);
        gpio_write(&ymInOut, 0); gpio_free(&ymInOut);
        gpio_write(&ypOut, 0); gpio_free(&ypOut);
    }

    return Point(x, y, z);
}

// *********************************************************************************************************************
int PortentaTouchScreen::analogin_read_average(analogin_t *pInput) {
    int sum = 0;
    int min = 1023;
    int max = 0;

    for (int i = 0; i < AVERAGE_SAMPLES; i++) {
        int tmp = analogin_read_u16(pInput) >> 6;
        if (tmp > max) max = tmp;
        if (tmp < min) min = tmp;
        sum += tmp;
    }

    return (sum - min - max) / (AVERAGE_SAMPLES - 2);
}