/*
* Beam Buddy : IoT-Enabled Smart Bike Light
* 34346 - Networking Technologies and Application Development for IoT, Spring 2025
* Group 1
* Header file for bike light LED array functions
*/

#ifndef LIGHTARRAY_H
#define LIGHTARRAY_H

#include <Arduino.h>

class LightArray {
public:
    explicit LightArray(uint8_t indicatorPin);
    void setup();
    void on();
    void off();
    void toggle();
    void showUnlockLight();
    void showLockLight();
    void showDeniedLight();

private:
    uint8_t _indicatorPin;
    bool _indicatorStatus;
    void flash(int times, int onDuration, int offDuration);
};

#endif
