// (c) 2021 Lennart Popkes
// Distributed under MIT License

#include <limits.h>
#include "PortentaTouchScreen.h"

#define ALLOWED_NOISE 4

PortentaTouchScreen::PortentaTouchScreen(pin_size_t ym, pin_size_t xm, pin_size_t yp, pin_size_t xp) {
    ymPin = digitalPinToPinName(ym);
    xpPin = digitalPinToPinName(xp);
    xmPin = digitalPinToPinName(xm);
    ypPin = digitalPinToPinName(yp);
}

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