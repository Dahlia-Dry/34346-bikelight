#ifndef ACCELEROMETER_H
#define ACCELEROMETER_H
#include <Arduino.h>

class accelerometer
{
private:
    MPU6050 mpu;

public:
    accelerometer(/* args */);
    ~accelerometer();
    bool initialize();
    bool check_moving();
};

#endif // ACCELEROMETER_H