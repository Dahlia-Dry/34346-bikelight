/*
* Beam Buddy : IoT-Enabled Smart Bike Light
* 34346 - Networking Technologies and Application Development for IoT, Spring 2025
* Group 1
* Code for MPU6050 Accelerometer functions
*/

#include "Accelerometer.h"

#if DEBUG
#define D_SerialBegin(...) Serial.begin(__VA_ARGS__);
#define D_print(...)    Serial.print(__VA_ARGS__)
#define D_write(...)    Serial.write(__VA_ARGS__)
#define D_println(...)  Serial.println(__VA_ARGS__)
#else
#define D_SerialBegin(...)
#define D_print(...)
#define D_write(...)
#define D_println(...)
#endif

Accelerometer::Accelerometer(int SDA, int SCL) {
    Wire.setPins(SDA, SCL);
    Wire.begin();
}

Accelerometer::~Accelerometer() {

}

void Accelerometer::setup() {
    D_SerialBegin(115200);
    #ifdef DEBUG
    while (!Serial) delay(10);
    #endif

    D_println("Adafruit MPU6050 test!");

    if (!mpu.begin(0x68)) {
        D_println("Failed to find MPU6050 chip");
        while (1) delay(10);
    }

    D_println("MPU6050 Found!");

    mpu.setHighPassFilter(MPU6050_HIGHPASS_0_63_HZ); 
    mpu.setMotionDetectionThreshold(20);    // LSB = 2mg of force
    mpu.setMotionDetectionDuration(80);    // LSB = 1ms of continous force
    Wire.beginTransmission(0x68);
    Wire.write(0x37);                       // Interrupt register
    Wire.write(0b11001000);                 // Sets interrupt active low, opendrain, no latch and no fsync
    Wire.endTransmission();
    //mpu.setInterruptPinPolarity(true);      // True for active-low
    //mpu.setInterruptPinLatch(false);        // Keep low until cleared
    mpu.setMotionInterrupt(true);           // Set motion interrupt

    delay(100);                             // Let I2C send commands
}

bool Accelerometer::isMoving() {
    return mpu.getMotionInterruptStatus();  // Get status of interrupt
}