| :warning: **From my testing, this code does currently not seem to work reliably for my hardware configuration (SeeedStudio TFT Touch Shield 2.0). It might work for you, but at this moment I can't provide support for this library.**<br/><br/>EDIT: I might have found the error is was having. Pins A4 and D10 (SPI1 MISO) are also connected (like A5 and D8) and therefore, as soon as I powered an SPI device, the touchscreen read would only give a garbeled mess. This also explains why the touchscreen stopped working as soon as I connected power to the touchscreen (two events that should not interfere which one another). |
|---|

# Arduino Portenta H7 touchscreen library
(c) 2021 Lennart Popkes.

Distributed under the MIT License.

## Introduction
This is a very basic library for interfacing a 4-wire resistive touchscreen with the Arduino Portenta H7.

Due to some pin restrictions from the STM32H7 and a few Arduino API bugs, it is currently (Mar 2021) not possible to use a standard touchscreen driver like the [Adafruit TouchScreen Library](https://github.com/adafruit/Adafruit_TouchScreen). You can read about the issue at the bottom of the document.

This solution uses the mbed HAL layer and does not suffer from the software problems listed below.

## How to use
1. Connect your 4-wire touchscreen to 4 pins of the arduino. 
    - The ```X-``` and ```Y+```-wires need to be connected to analogue I/O pins. If using the break pins on top of the board, the only two available are ```A4``` and ```A6```
    - The ```Y-``` and ```X+```-wires can be connected to any I/O pin
2. Caution! Do not use any of A0-A3 pins, since these are analogue <b>input</b> only! (See [below](#the-issues-with-the-portenta-h7))
3. In your code: include ```PortentaTouchScreen.h```
4. Instantiate a ```PortentaTouchScreen``` passing all 4 pins to the constructor. The library assumes a default resistance of 360 ohms across the x-Plate, which is around what most touchscreen will probably have. If needed, it can be passed as the 5th parameter.
5. When neccessary: 
    - Get the current touch point using ```getPoint```. It contains the x- and y-coordinates and the pressure z. You can use the Arduino-method ```map``` to scale the values to your screen size (like in the *basicTouchScreen* example). You might be able to use ```readTouchX```, ```readTouchY``` and ```pressure```, but they can be a little buggy and return unwanted results.
    - A touch is sensed if the value returned is greater than ```pressureThreshold```.
    - The values from ```getPoint``` have 11 bit accuracy (0 to 2047).

## PortentaTouchScreen and [LVGL](https://lvgl.io)
The touchscreen can be used as an input device for Little VGL. Write your own input device callback using the input type of ```LV_INDEV_TYPE_POINTER```. 

## The issues with the Portenta H7
- Pins A0-A3 are analogue input only (see chapter ```Pin Descriptions``` in the [STM32H747xi datasheet](https://www.st.com/resource/en/datasheet/stm32h747xi.pdf), pins ```PA0_C```, ```PA1_C```, ```PC2_C```, ```PC3_C```)
- Once using ```pinMode()``` on any of the other analogue pins (A4-A6), no ```analogRead()``` will be possible anymore or at least return no sensible value
- Pin A5 (```PC3```) is connected to Pin D8, which is also the default ```SPI1 MOSI```. This means that as long as you have any SPI devices (like an ILI9341 TFT/LCD) connected, pin A5 is basically useless
- Pin A4 (`PC2`) is connected to Pin D10, which is also the default `SPI MISO`. This means that as long as you have any SPI devices (like an ILI9341 TFT/LCD) connected, pin A4 is basically useless.
- With Pins A0-A5 being practically useless for this scenario, it is essentially impossible to connect a TFT/LCD with a resistive 4-wire touchscreen to the portenta without some hacking.
