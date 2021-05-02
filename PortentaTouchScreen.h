// (c) 2021 Lennart Popkes
// Distributed under MIT License

#ifndef _PORTENTA_TOUCH_SCREEN_H
#define _PORTENTA_TOUCH_SCREEN_H

// Includes
#include <Arduino.h>
#include <stdint.h>
#include "hal/analogin_api.h"
#include "hal/gpio_api.h"

// needed for getPoint-method.
#define NUMSAMPLES      2 // should not be changed!!!!
#define ALLOWED_NOISE   2
#define AVERAGE         true
#define RXPLATE         300
#define AVERAGE_SAMPLES 4

/**
 * Point class for the 4-wire resistive touch screen.
 * 
 * Contains variables x-, y-, and z-position (pressure) and the according getter methods as well as overwriting equal-to
 * and unequal operators. 
 */ 
class Point {
    
public:
    /**
     * Construct an empty point variable with all values at 0.
     */
    Point(void);
    /**
     * Construct a point with the given parameters.
     * @param x the x value of the point
     * @param y the y value of the point
     * @param z the z value or pressure of the point
     */
    Point(int x, int y, int z);

    /**
     * Checks if this point is equal to another point (all coordinates match)
     */
    bool operator==(Point);
    /**
     * Checks if this point is not equal to another point (at least one coordinate does not match)
     */ 
    bool operator!=(Point);

    /** @return the x value of the point. */
    int x() const { return mx; };
    /** @return the y value of the point. */
    int y() const { return my; };
    /** @return the z value or pressure of the point. */
    int z() const { return mz; };

private:
    int mx, my, mz;

};


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
     * Read the touch event's X value as a 10 bit integer
     * 
     * @return the touch position on the x-plate or 0, if the touch event was invalid
     */
    uint16_t readTouchX();
    /**
     * Read the touch event's Y value as a 10 bit integer
     * 
     * @return the touch position on the y-plate or 0, if the touch event was invalid
     */
    uint16_t readTouchY();
    /**
     * Read the touch event's Z value/pressure value. A touch event occurs when the return value is above a pressure 
     * threshold.
     * 
     * @see #pressureThreshold
     * @return 16 bit precision integer value of the touch pressure
     */
    uint16_t pressure();

    /**
     * Get the current touch point of the touchscreen. Reads x-, y- and pressure (z) values. If the touch event is
     * invalid, the z value will be 0 and the x and y values are meaningless.
     * 
     * @return the point of the current touch with 11 bit precision integer values (10 bit 2x oversampling)
     */
    Point getPoint();

    /// default pressure threshold. Any pressure above this value means a touch event 
    uint16_t pressureThreshold = 10;

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

    // for getPoint-method
    int analogin_read_average(analogin_t *pInput);

};

#endif // _PORTENTA_TOUCH_SCREEN_H