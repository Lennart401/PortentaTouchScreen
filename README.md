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
4. Instantiate a ```PortentaTouchScreen``` passing all 4 pins to the constructor
5. When neccessary: 
    - read the x- and y-Positions using the corresponding methods ```readTouchX``` and ```readTouchY```. Use can use the Arduino-method ```map``` to scale the values to your screen size
    - The touch pressure can be read using ```pressure```. A touch is sensed if the value returned is greater than ```pressureThreshold```
    - All values are returned as a ```uint16_t``` with 16 bit accurarcy (zero to 2^16)

## PortentaTouchScreen and [LVGL](https://lvgl.io)
The touchscreen can be used as an input device for Little VGL. Write your own input device callback using the input type of ```LV_INDEV_TYPE_POINTER```. 

## The issues with the Portenta H7
- Pins A0-A3 are analogue input only (see chapter ```Pin Descriptions``` in the [STM32H747xi datasheet](https://www.st.com/resource/en/datasheet/stm32h747xi.pdf), pins ```PA0_C```, ```PA1_C```, ```PC2_C```, ```PC3_C```)
- Once using ```pinMode()``` on any of the other analogue pins (A4-A6), no ```analogRead()``` will be possible anymore or at least return no sensible value
- Pin A5 (```PC3```) is connected to Pin D8, which is also the default ```SPI1 MOSI```. This is means that as long as you have any SPI devices (like an ILI9341 TFT/LCD) connected, pin A5 is basically useless
