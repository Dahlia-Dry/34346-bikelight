/*
* Beam Buddy : IoT-Enabled Smart Bike Light
* 34346 - Networking Technologies and Application Development for IoT, Spring 2025
* Group 1
* Header file for Adafruit MAX17048 Battery gauge functions
*/

#ifndef BATTERY_H
#define BATTERY_H

#include <Arduino.h>
#include <Adafruit_MAX1704X.h>

class Battery {
public:
    void setup();
    char getBatteryPercent();  // returns value between 0–100

private:
    Adafruit_MAX17048 maxlipo;
};

#endif
