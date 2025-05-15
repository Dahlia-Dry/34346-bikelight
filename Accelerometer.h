#ifndef ACCELEROMETER_H
#define ACCELEROMETER_H

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

class Accelerometer {
public:
    void setup();
    bool isMoving();

private:
    Adafruit_MPU6050 mpu;
};

#endif
