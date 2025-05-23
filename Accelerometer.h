/*
* Beam Buddy : IoT-Enabled Smart Bike Light
* 34346 - Networking Technologies and Application Development for IoT, Spring 2025
* Group 1
* Header file for MPU6050 Accelerometer functions
*/

#ifndef ACCELEROMETER_H
#define ACCELEROMETER_H

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

class Accelerometer {
public:
    Accelerometer(int SDA, int SCL);
    ~Accelerometer();
    void setup();
    bool isMoving();

private:
    Adafruit_MPU6050 mpu;
};

#endif