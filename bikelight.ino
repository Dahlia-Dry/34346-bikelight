#include "LoraConfig.h"
//#include "battery.h"

//pin definitions
//const int ;

//status definitions
char status = 0;
char percentage = 0;

void setup() {
  Serial.begin(115200);
  Mcu.begin(HELTEC_BOARD,SLOW_CLK_TPYE);
  
  //battery setup
  //setup_battery();
}

void loop()
{ 
  if(status == 0){ //sleep

  }
  else if(status == 1){ //parked

  }
  else if (status == 2){ //moving

  }
  lora_send(status, percentage);
}