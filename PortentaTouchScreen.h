#ifndef _PORTENTA_TOUCH_SCREEN_H
#define _PORTENTA_TOUCH_SCREEN_H

#include <Arduino.h>
#include <stdint.h>
#include "hal/analogin_api.h"
#include "hal/gpio_api.h"

/**
 * 4-wire resistive touch screen driver class for the Arduino Portenta H7.
 * 
 * This class holds all informations neccessary to communicate with a 4-wire 
 * resistive touchscreen (Y-, X-, Y+, X+) and contains methods to read the
 * x- and y-positions of a touch event and get the corresponding pressure. 
 */
class PortentaTouchScreen {

public:
    /**
     * Construct a new touch screen object. All pins must be I/O pins, not
     * just input pins.
     * 
     * @param ym Y- pin. Can be a digital pin.
     * @param xm X- pin. Must be an analog pin.
     * @param yp Y+ pin. Must be an analog pin.
     * @param xp X+ pin. Can be a digital pin.
     */
    PortentaTouchScreen(pin_size_t ym, pin_size_t xm, pin_size_t yp, pin_size_t xp);

    /**
     * Read the touch event's X value.
     * 
     * @return 16 bit precision integer value of the touch position on the x-plate
     */
    uint16_t readTouchX();
    /**
     * Read the touch event's Y value.
     * 
     * @return 16 bit precision integer value of the touch position on the y-plate
     */
    uint16_t readTouchY();
    /**
     * Read the touch event's Z value/pressure value. A touch event occurs when
     * the return value is above a pressure threshold.
     * 
     * @see #pressureThreshold
     * @return 16 bit precision integer value of the touch pressure
     */
    uint16_t pressure();

    /// default pressure threshold. Any pressure above this value means a touch event 
    uint16_t pressureThreshold = 1000;

private:
    // ym/xp only need digital (gpio) i/o ports
    gpio_t ymInOut;
    gpio_t xpInOut;

    // xm/yp need both analog input and digital (gpio) out ports
    gpio_t xmOut;
    gpio_t ypOut;
    analogin_t xmIn;
    analogin_t ypIn;

    // the pinnames for the provided arduino pins
    PinName ymPin, xpPin, xmPin, ypPin;

};

#endif // _PORTENTA_TOUCH_SCREEN_H