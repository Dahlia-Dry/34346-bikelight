#include "accelerometer.h"
#include <MPU6050.h>
#include "Wire.h" // This library allows you to communicate with I2C devices.
#ifndef DEBUG
// #define DEBUG 0  // Uncomment to enable debug output
#endif

accelerometer::accelerometer(/* args */)
{
}

accelerometer::~accelerometer()
{
}

bool accelerometer::initialize()
{
    // Start I2C if its not already started
    #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    Wire.begin(); // MPU6050 library handles most I2C communication, but Wire will need to begin first
    #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
    Fastwire::setup(400, true);
    #endif

    // Init the module
    #ifdef DEBUG
    Serial.println("Initializing MPU...");
    #endif
    mpu.initialize();
    #ifdef DEBUG
    Serial.println("MPU initializated");
    #endif
    // Check module connection
    #ifdef DEBUG
    Serial.println("Testing device connections...");
    #endif
    if (mpu.testConnection() == false)
    {
        #ifdef DEBUG
        Serial.println("MPU6050 connection failed");
        #endif
        return false;
    }
    else
    {
        #ifdef DEBUG
        Serial.println("MPU6050 connection successful");
        #endif
    }
    // possible condes into fewer i2c messages.
    mpu.setAccelerometerPowerOnDelay(3);
    mpu.setIntZeroMotionEnabled(DISABLED);
    mpu.setIntMotionEnabled(true); // Set interrupt motion to enabled
    mpu.setDHPFMode(0);
    mpu.setMotionDetectionThreshold(uint8_t(4 / 2)); // Threshold in 2mg per LSB, so divide by 2 to show 500mg
    mpu.setZeroMotionDetectionDuration(uint8_t(4 / 2));
    mpu.setMotionDetectionDuration(5); // LSB is 1 ms. Duration of movement before interrupt is called
    mpu.setZeroMotionDetectionDuration(20);
    mpu.setInterruptDrive(MPU6050_INTDRV_PUSHPULL);       // 0 Push-pull, 1 Open-drain
    mpu.setInterruptLatch(MPU6050_INTLATCH_WAITCLEAR);    // 0 50us-pulse, 1 latch until cleared
    mpu.setInterruptLatchClear(MPU6050_INTCLEAR_ANYREAD); // 0 status read only, 1 any register read
    mpu.setInterruptMode(MPU6050_INTMODE_ACTIVEHIGH);     // 0 active-high, 1 active-low
    return true;
}

bool accelerometer::check_moving()
{
    return mpu.getMotionStatus();
}
