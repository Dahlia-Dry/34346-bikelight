#include "LoraConfig.h"
#include "Battery.h"
#include "Accelerometer.h"
#include "RFIDReader.h"
#include "LightArray.h"

//pin definitions
const int interruptPin = 9;
const int buzzer = 2;
const int ldr = 2;
const int indicator_led = 21;
const int SS_PIN = 20;    
const int RST_PIN = 5;  
const int SCK_PIN = 10; 
const int MISO_PIN = 6; 
const int MOSI_PIN = 7;
const int SDA_PIN = 19;
const int SCL_PIN = 18;

//status definitions
char percentage = 0;
bool isMoving = false;
int light_val = 1000; //set light level for turn on/off (outside lvl: ~3300)
unsigned long lastMotionTime = 0;
bool isLocked = false;
volatile bool motionDetected = false;
volatile bool buttonPressed = false;
bool keyMatch = false;
bool detectCard = false;
bool interruptTriggered = false;
RTC_DATA_ATTR char status = 0;

//object definitions
Accelerometer accel(SDA_PIN,SCL_PIN);
Battery battery;
LightArray lights(15);
RFIDReader rfid(SCK_PIN, MISO_PIN, MOSI_PIN, SS_PIN, RST_PIN);

//additional parameters
String RFID_UID_Code = "31 95 FF 4C";     //set RFID_UID_Code as "3195ff4c" - use your card number here to gain access
String RFID_UID_Code1 = "46 41 B2 04";    //set RFID_UID_Code1 as "5641b204" - use your card number here to gain access
int light_threshold = 1000;

// --- Operation Modes ---
#define MODE_PARKED  1
#define MODE_ACTIVE  2
#define MODE_ALARM   3

// Interrupts

void IRAM_ATTR onInterrupt(){
  interruptTriggered = true;
}

void setup() {
  Serial.begin(115200);
  Mcu.begin(HELTEC_BOARD,SLOW_CLK_TPYE);
  
  //battery setup
  //battery.setup();

  //accelerometer setup
  accel.setup();

  //buzzer setup
  pinMode(buzzer, OUTPUT);

  //photoresistor setup
  pinMode(ldr, INPUT);

  attachInterrupt(digitalPinToInterrupt(interruptPin), onInterrupt, FALLING);
}

void loop() {
  //handle interrupt
  if (interruptTriggered){
    interruptTriggered=false;
    isMoving = accel.isMoving();
    detectCard = rfid.detectCard();
    if(isMoving){
      lastMotionTime = millis();
      if (isLocked) {
        status = MODE_ALARM;
        Serial.println("Locked bike was moved! Alarm ON.");
      }
      else if (!isLocked && status == MODE_PARKED && light_val < light_threshold) {
        lights.on();
        status = MODE_ACTIVE;
        Serial.println("Motion detected at night - ACTIVE mode.");
      }
    }
    if(detectCard){
      keyMatch = rfid.checkAccess(RFID_UID_Code,RFID_UID_Code1);
      if(isLocked){
        if(keyMatch){
          lights.showUnlockLight();
        }
        else{
          lights.showDeniedLight();
        }
      }
      else{
        if(keyMatch){
          lights.showLockLight();
        }
        else{
          lights.showDeniedLight();
        }
      }
    }
    if (!isMoving && !detectCard){ //is button
      lights.toggle();
    }
  }
    // --- Sleep in Storage Mode ---
  if (status == MODE_PARKED && !isMoving) {
    Serial.println("Entering deep sleep mode");
    //delay(100);
    //esp_deep_sleep_start();
  }

  else{
    percentage = battery.getBatteryPercent();
    light_val = analogRead(ldr);

    // --- Active Mode ---
    if (status == MODE_ACTIVE) {
      if (millis() - lastMotionTime > 30000) {
        status = MODE_PARKED;
        Serial.println("No motion, switching to PARKED mode.");
      }
      if (light_val < light_threshold){
        lights.on();
      }
    }

    // --- Alarm Mode ---
    if (status == MODE_ALARM) {
      tone(buzzer, 1000);
    } 
    else {
      noTone(buzzer);
    }

    // --- Battery Warning ---
    if (percentage < 20) {
      tone(buzzer, 2000, 200);
    }

    // --- Lora Send ---
    lora_send(status, percentage);

    //delay(100);

  }
}