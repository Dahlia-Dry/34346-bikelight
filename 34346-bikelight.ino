/*
* Beam Buddy : IoT-Enabled Smart Bike Light
* 34346 - Networking Technologies and Application Development for IoT, Spring 2025
* Group 2
* Main code for Heltec HT-CT62 development board
*/

//debug statements
#define DEBUG 0    // SET TO 0 OUT TO REMOVE TRACES

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
bool overwrite = false;
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

// interrupt can be triggered by accelerometer, RFID scanner, or button
void IRAM_ATTR onInterrupt(){
  D_println("INTERRUPT TRIGGERED");
  interruptTriggered = true;
}

void setup() {
  
  D_SerialBegin(115200); //print to serial monitor if in debug mode

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
  #if !DEBUG
  Serial.end();
  lights.setup();
  lights.off();
  #endif

  status = MODE_ACTIVE; //set initial status to PARKED

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
      pinMode(ldr, INPUT);
      light_val = analogRead(ldr); //read ambient light value from ldr
      
      D_println("Movement");
      D_println(light_val);
      
      if (isLocked) { //trigger alarm if bike is locked and moved
        status = MODE_ALARM;
        
        D_println("Locked bike was moved! Alarm ON.");
        
      }
      else{ //if bike is not locked, mode is ACTIVE
        status = MODE_ACTIVE;
        
        D_println("Motion detected - ACTIVE mode.");
        
      }
    }
    if(detectCard){ //RFID scanner triggered interrupt
      
      D_println("Card");
      
      keyMatch = rfid.checkAccess(RFID_UID_Code,RFID_UID_Code1);
      if(isLocked){
        if(keyMatch){
          pinMode(indicator_led, OUTPUT);
          lights.showUnlockLight();
        }
        else{
          pinMode(indicator_led, OUTPUT);
          lights.showDeniedLight();
        }
      }
      else{
        if(keyMatch){
          pinMode(indicator_led, OUTPUT);
          lights.showLockLight();
        }
        else{
          pinMode(indicator_led, OUTPUT);
          lights.showDeniedLight();
        }
      }
    }
    delay(10); // Let IRQ line settle before checking if it is button
    if (!isMoving && !detectCard && !(digitalRead(interruptPin))) { //button triggered interrupt
      
      D_println("Button");
      status = MODE_ACTIVE;
      overwrite = true;
      lastMotionTime = millis();
      pinMode(indicator_led, OUTPUT);
      lights.toggle(); //turn lights on/off with button press
    }
  }

  //next, check if device should go to sleep 
  if ((status == MODE_PARKED && !isMoving) || (status == MODE_STORAGE)) {
    
    //D_println("Sleep");
    
    delay(10);
    //esp_deep_sleep_start();
  }

  //if not sleep mode, collect+send updated status data via LoRa
  if (status == MODE_ACTIVE) { //if bike is in active mode, check if it is still moving
    //delay(50);
    D_println(millis() - lastMotionTime);
    if (millis() - lastMotionTime > 30000) { //auto turn off if not moved in 30 seconds
      status = MODE_PARKED;
      isMoving = false;
      overwrite = false;
      pinMode(indicator_led, OUTPUT);
      lights.off();
      
      D_println("No motion, switching to PARKED mode.");
      
    }
    else if (overwrite == true) {
      // Bypass checking lights and changing it.
    }

    else{ //if bike is still moving, turn on/off lights on if it is dark
      pinMode(ldr, INPUT);  // Change to input in case buzzer is output
      if (analogRead(ldr) < light_threshold){
        pinMode(indicator_led, OUTPUT);
        lights.on();
      }
      else{ //turn lights off if daylight to save power
        pinMode(indicator_led, OUTPUT);
        lights.off();
      }
    }
  }
  //if alarm activated, activate buzzer
  if (status == MODE_ALARM) {
    pinMode(buzzer, OUTPUT);
    tone(buzzer, 1000);
  } 
  else {
    pinMode(buzzer, OUTPUT);
    noTone(buzzer);
  }

  // if battery percentage is low, activate buzzer
  if (percentage < 20) {
    pinMode(buzzer, OUTPUT);
    tone(buzzer, 2000, 200);
  }

  // Keep the indiciator led as output, HT-CT62 might be fiddling with this
  pinMode(indicator_led, OUTPUT);

  percentage = battery.getBatteryPercent(); //get battery percentage
  // --- Lora Send ---
  delay(10);
  lora_send(status, percentage);
}