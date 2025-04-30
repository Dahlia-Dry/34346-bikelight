#include <HardwareSerial.h>
#include "LoraConfig.h"
#include "

void setup() {
  Serial.begin(115200);
  Mcu.begin(HELTEC_BOARD,SLOW_CLK_TPYE);
  
}

void loop() {
  // put your main code here, to run repeatedly:
  lora_send_once();
}
