#include <Arduino.h>
#include <MPU6050.h>
#include "Wire.h" // This library allows you to communicate with I2C devices.
#include "driver/rtc_io.h"

#define PUSH_PULL 0;

MPU6050 mpu;

const int MPU_ADDR = 0x68; // I2C address of the MPU-6050. If AD0 pin is set to HIGH, the I2C address will be 0x69.
const int UNO_ADDR = 8;

int16_t accelerometer_x, accelerometer_y, accelerometer_z; // variables for accelerometer raw data
int16_t gyro_x, gyro_y, gyro_z; // variables for gyro raw data
int16_t temperature; // variables for temperature data

char tmp_str[7]; // temporary variable used in convert function

// Any data to be saved should have a RTC_DATA_ATTR before it, and not in a function (Maybe static works for functions)
RTC_DATA_ATTR int bootCount = 0;

char* convert_int16_to_str(int16_t i) { // converts int16 to string. Moreover, resulting strings will have the same length in the debug monitor.
  sprintf(tmp_str, "%6d", i);
  return tmp_str;
}

void go_to_sleep(void)
{
  // Attach interrupt
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_2, HIGH);

  // Disable pullup and enable pulldown
  rtc_gpio_pullup_dis(GPIO_NUM_2);
  rtc_gpio_pulldown_en(GPIO_NUM_2);
  // Clear interrupt just before sleep
  mpu.getIntMotionStatus(); // Clear interrupt bit
  // Sleep
  delay(100);
  esp_deep_sleep_start();
}

void print_wakeup_reason() {
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch (wakeup_reason) {
    case ESP_SLEEP_WAKEUP_EXT0:     Serial.println("Wakeup caused by external signal using RTC_IO"); break;
    case ESP_SLEEP_WAKEUP_EXT1:     Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
    case ESP_SLEEP_WAKEUP_TIMER:    Serial.println("Wakeup caused by timer"); break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD: Serial.println("Wakeup caused by touchpad"); break;
    case ESP_SLEEP_WAKEUP_ULP:      Serial.println("Wakeup caused by ULP program"); break;
    default:                        Serial.printf("Wakeup was not caused by deep sleep: %d\n", wakeup_reason); break;
  }
}

// This function needs Wire.begin() and Serial.begin() to be ran before use
void InitializeMPU6050() {
  // Init the module
  Serial.println("Initializing MPU...");
  mpu.initialize();
  Serial.println("MPU initializated");
  // Check module connection
  Serial.println("Testing device connections...");
  if(mpu.testConnection() ==  false){
    Serial.println("MPU6050 connection failed"); return;
  }
  else{
    Serial.println("MPU6050 connection successful");
  }
  // possible condes into fewer i2c messages.
  mpu.setAccelerometerPowerOnDelay(3);
  mpu.setIntZeroMotionEnabled(DISABLED);
  mpu.setIntMotionEnabled(true);                        // Set interrupt motion to enabled
  mpu.setDHPFMode(0);
  mpu.setMotionDetectionThreshold(uint8_t(4 / 2));      // Threshold in 2mg per LSB, so divide by 2 to show 500mg
  mpu.setZeroMotionDetectionDuration(uint8_t(4 / 2));
  mpu.setMotionDetectionDuration(5);                   // LSB is 1 ms. Duration of movement before interrupt is called
  mpu.setZeroMotionDetectionDuration(20);
  mpu.setInterruptDrive(MPU6050_INTDRV_PUSHPULL);       // 0 Push-pull, 1 Open-drain
  mpu.setInterruptLatch(MPU6050_INTLATCH_WAITCLEAR);    // 0 50us-pulse, 1 latch until cleared
  mpu.setInterruptLatchClear(MPU6050_INTCLEAR_ANYREAD); // 0 status read only, 1 any register read
  mpu.setInterruptMode(MPU6050_INTMODE_ACTIVEHIGH);     // 0 active-high, 1 active-low
  
}

void setup() {
  Serial.begin(115200);
  Serial.println("Boot number: " + String(bootCount));
  print_wakeup_reason();
  
  #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
  Wire.begin(); // MPU6050 library handles most I2C communication, but Wire will need to begin first
  #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
  Fastwire::setup(400, true);
  #endif

  if (bootCount == 0)
  {
    InitializeMPU6050();
  }
  ++bootCount;
  

  // Setup sleep and wakeup
  //attachInterrupt(digitalPinToInterrupt(GPIO_NUM_2), ISR_Accelerometer, RISING);

}
void loop() {

  if (Serial.available())
  {
    Serial.println("Going to sleep");
    go_to_sleep();
  }

  Serial.println("Motion status: " + String(mpu.getMotionStatus()));
  delay(160);
  
  // Call sleep. After waking up, run global variables, setup and then loop
  // go_to_sleep();

}