#include "battery.h"

byte bytemsg = 0x00;

void setup_battery(){
  while (!maxlipo.begin()) {
    Serial.println(F("Couldnt find Adafruit MAX17048?\nMake sure a battery is plugged in!"));
  }
  maxlipo.setAlertVoltages(3.3, 4.2); //Figure out what the drop out voltage should
  maxlipo.setResetVoltage(3.3);
}