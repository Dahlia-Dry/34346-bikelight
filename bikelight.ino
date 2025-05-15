#include "LoraConfig.h"
#include "Battery.h"
#include "Accelerometer.h"
#include "RFIDReader.h"
#include "LightArray.h"

//pin definitions
const int buzzer = 0;
const int ldr = 0;
const int indicator_led = 15;
const int SS_PIN = 5;    //set pin D5 for SDA
const int RST_PIN = 22;  //set pin D22 for Reset
const int SCK_PIN = 18;  //set pin D18 for SCK
const int MISO_PIN = 19;  //set pin D19 for MISO
const int MOSI_PIN = 23;  //set pin D23 for MOSI

//status definitions
char status = 0;
char percentage = 0;
bool isMoving = false;
bool wasMoving = false;
int light_val = 1000; //set light level for turn on/off (outside lvl: ~3300)


//object definitions
Accelerometer accel;
Battery battery;
LightArray lights(15);
RFIDReader rfid(SCK_PIN, MISO_PIN, MOSI_PIN, SS_PIN, RST_PIN);

//additional parameters
String RFID_UID_Code = "31 95 FF 4C";     //set RFID_UID_Code as "3195ff4c" - use your card number here to gain access
String RFID_UID_Code1 = "46 41 B2 04";    //set RFID_UID_Code1 as "5641b204" - use your card number here to gain access
int light_threshold = 10;


void setup() {
  Serial.begin(115200);
  Mcu.begin(HELTEC_BOARD,SLOW_CLK_TPYE);
  
  //battery setup
  battery.setup();

  //accelerometer setup
  accel.setup();

  //buzzer setup
  pinMode(buzzer, OUTPUT);

  //photoresistor setup
  pinMode(ldr, INPUT);
}

void loop()
{ 
  //update variables
  percentage = battery.getBatteryPercent();
  wasMoving = isMoving;
  isMoving = accel.isMoving();
  light_val=analogRead(ldr);
  
  //act accordingly
  if(status == 0){ //storage

  }
  else{
    if(status == 1){ //parked
      if(accel.isMoving()){
        status = 3;
      }
    }

    else if (status == 2){ //active
      
    }

    else if (status == 3){ //alarm
      tone(buzzer, 1000); // Send 1KHz sound signal...
    }
    lora_send(status, percentage);
  }
}