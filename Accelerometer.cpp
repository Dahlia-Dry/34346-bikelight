#include "Accelerometer.h"

Accelerometer::Accelerometer(int SDA, int SCL) {
    Wire.setPins(SDA, SCL);
    Wire.begin();
}

Accelerometer::~Accelerometer() {

}

void Accelerometer::setup() {
    Serial.begin(115200);
    while (!Serial) delay(10);

    Serial.println("Adafruit MPU6050 test!");

    if (!mpu.begin(0x68)) {
        Serial.println("Failed to find MPU6050 chip");
        while (1) delay(10);
    }

    Serial.println("MPU6050 Found!");

    mpu.setHighPassFilter(MPU6050_HIGHPASS_0_63_HZ);
    mpu.setMotionDetectionThreshold(3);
    mpu.setMotionDetectionDuration(20);
    mpu.setInterruptPinLatch(true);
    mpu.setInterruptPinPolarity(true);
    mpu.setMotionInterrupt(true);

    delay(100);
}

bool Accelerometer::isMoving() {
    return mpu.getMotionInterruptStatus();
}