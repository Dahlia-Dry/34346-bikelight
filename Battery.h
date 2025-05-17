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
