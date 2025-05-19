/*
* Beam Buddy : IoT-Enabled Smart Bike Light
* 34346 - Networking Technologies and Application Development for IoT, Spring 2025
* Group 2
* Main code for Heltec HT-CT62 development board
*/

//include libraries
#include "LoraConfig.h" //functions for LoRa communication - change OTAA/ABP parameters in LoraConfig.cpp
#include "Battery.h" //functions for MAX17048 battery gauge
#include "Accelerometer.h" //functions for MPU6050 accelerometer
#include "RFIDReader.h" //functions for RC522 RFID reader
#include "LightArray.h" //functions for LED array

//editable parameters
String RFID_UID_Code = "31 95 FF 4C";     //set RFID_UID_Code as "3195ff4c" - use your card number here to gain access
String RFID_UID_Code1 = "46 41 B2 04";    //set RFID_UID_Code1 as "5641b204" - use your card number here to gain access
int light_threshold = 1000; //edit daylight threshold for light turn on/off

//pin definitions
const int interruptPin = 9; //accelerometer, RFID scanner, and button share same interrupt pin
const int buzzer = 2;
const int ldr = 2;
const int indicator_led = 21; //indicator LED is same as LED array
const int SS_PIN = 20;    
const int RST_PIN = 5;  
const int SCK_PIN = 10; 
const int MISO_PIN = 6; 
const int MOSI_PIN = 7;
const int SDA_PIN = 19;
const int SCL_PIN = 18;

//variable definitions
char percentage = 0; //battery percentage
int light_val = 1000; //set light level for turn on/off (outside lvl: ~3300)
unsigned long lastMotionTime = 0;
bool isLocked = false; //true if device is in "locked" state
bool isMoving = false; //true if accelerometer triggers interrupt due to movement detected
bool keyMatch = false; //true if RFID card matches RFID_UID_Code or RFID_UID_Code1
bool detectCard = false; //true if RFID module triggers interrupt due to card being scanned
bool interruptTriggered = false;
RTC_DATA_ATTR char status = 0; //set status as RTC_DATA_ATTR so it is saved after reset

//object definitions
Accelerometer accel(SDA_PIN,SCL_PIN);
Battery battery;
LightArray lights(indicator_led);
RFIDReader rfid(SCK_PIN, MISO_PIN, MOSI_PIN, SS_PIN, RST_PIN);

//operation modes
#define MODE_STORAGE 0
#define MODE_PARKED  1
#define MODE_ACTIVE  2
#define MODE_ALARM   3

#define DEBUG // Uncomment to enable debug messages

// interrupt can be triggered by accelerometer, RFID scanner, or button
void IRAM_ATTR onInterrupt(){
  #ifdef DEBUG
  Serial.println("INTERRUPT TRIGGERED");
  #endif
  interruptTriggered = true;
}

void setup() {
  #ifdef DEBUG
  Serial.begin(115200); //print to serial monitor if in debug mode
  #endif

  Mcu.begin(HELTEC_BOARD,SLOW_CLK_TPYE); //initialized heltec board

  // Wait 5 seconds to allow Accel to be inserted (workaround)
  delay(5000);
  
  //battery setup
  battery.setup();

  //accelerometer setup
  accel.setup();

  //buzzer setup
  //pinMode(buzzer, OUTPUT);

  //photoresistor setup
  pinMode(ldr, INPUT);

  //lights
  //Serial.end();
  //lights.setup();
  //lights.on();

  status = MODE_PARKED; //set initial status to PARKED

  attachInterrupt(digitalPinToInterrupt(interruptPin), onInterrupt, FALLING); //attach interrupt to pin 9
}

void loop() {
  //first, handle interrupt if triggered
  if (interruptTriggered){
    interruptTriggered=false;
    isMoving = accel.isMoving(); //check if accelerometer triggered interrupt (motion detected)
    detectCard = rfid.detectCard(); //check if RFID scanner triggered interrupt (card detected)
    if(isMoving){ //accelerometer triggered interrupt
      lastMotionTime = millis();
      light_val = analogRead(ldr); //read ambient light value from ldr
      #ifdef DEBUG
      Serial.println("Movement");
      Serial.println(light_val);
      #endif
      if (isLocked) { //trigger alarm if bike is locked and moved
        status = MODE_ALARM;
        #ifdef DEBUG
        Serial.println("Locked bike was moved! Alarm ON.");
        #endif
      }
      else{ //if bike is not locked, mode is ACTIVE
        status = MODE_ACTIVE;
        #ifdef DEBUG
        Serial.println("Motion detected - ACTIVE mode.");
        #endif
      }
    }
    if(detectCard){ //RFID scanner triggered interrupt
      #ifdef DEBUG
      Serial.println("Card");
      #endif
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
    delay(10); // Let IRQ line settle before checking if it is button
    if (!isMoving && !detectCard && !(digitalRead(interruptPin))) { //button triggered interrupt
      #ifdef DEBUG
      Serial.println("Button");
      #endif
      lights.toggle(); //turn lights on/off with button press
    }
  }

  //next, check if device should go to sleep 
  if ((status == MODE_PARKED && !isMoving) || (status == MODE_STORAGE)) {
    #ifdef DEBUG
    Serial.println("Entering deep sleep mode");
    #endif
    delay(1000);
    //esp_deep_sleep_start();
  }

  //if not sleep mode, collect+send updated status data via LoRa
  else{
    if (status == MODE_ACTIVE) { //if bike is in active mode, check if it is still moving
      delay(50);
      Serial.println(millis() - lastMotionTime);
      if (millis() - lastMotionTime > 30000) { //auto turn off if not moved in 30 seconds
        status = MODE_PARKED;
        isMoving = false;
        lights.off();
        #ifdef DEBUG
        Serial.println("No motion, switching to PARKED mode.");
        #endif
      }
      else{ //if bike is still moving, turn on/off lights on if it is dark
        if (analogRead(ldr) < light_threshold){
          lights.on();
        }
        else{ //turn lights off if daylight to save power
          lights.off();
        }
      }
    }
    //if alarm activated, activate buzzer
    if (status == MODE_ALARM) {
      tone(buzzer, 1000);
    } 
    else {
      noTone(buzzer);
    }

    // if battery percentage is low, activate buzzer
    if (percentage < 20) {
      tone(buzzer, 2000, 200);
    }

    percentage = battery.getBatteryPercent(); //get battery percentage
    // --- Lora Send ---
    //lora_send(status, percentage);

    //delay(100);

  }
}