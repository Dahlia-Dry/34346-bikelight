#include "Adafruit_MAX1704X.h"

Adafruit_MAX17048 maxlipo;

byte bytemsg = 0x00;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  while (!maxlipo.begin()) {
    Serial.println(F("Couldnt find Adafruit MAX17048?\nMake sure a battery is plugged in!"));
  }
  maxlipo.setAlertVoltages(3.3, 4.2); //Figure out what the drop out voltage should
  maxlipo.setResetVoltage(3.3);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println(getBatteryPercentByte());
  delay(2000);
}
// returns battery% in 7lsb and a flag in bit 8 that indicates low voltage==turn off power
byte getBatteryPercentByte() {
  float percent = maxlipo.cellPercent();
  if (percent < 0) percent = 0;
  if (percent > 100) percent = 100;
  bytemsg = ((byte)round(percent));

  if (maxlipo.isActiveAlert()) {
    uint8_t status_flags = maxlipo.getAlertStatus();
    if (status_flags & MAX1704X_ALERTFLAG_VOLTAGE_LOW) {
      Serial.print(", Voltage low");
      bytemsg |= 0x80;
      maxlipo.clearAlertFlag(MAX1704X_ALERTFLAG_VOLTAGE_LOW); // clear the alert
    }
  }
  Serial.println(bytemsg);
  return bytemsg;
}