#include "Battery.h"

void Battery::setup() {
    Serial.begin(115200);

    while (!maxlipo.begin()) {
        Serial.println(F("Couldn't find Adafruit MAX17048? Make sure a battery is plugged in!"));
        delay(1000);
    }

    maxlipo.setAlertVoltages(3.3, 4.2);
    maxlipo.setResetVoltage(3.3);
}

char Battery::getBatteryPercent() {
    float percent = maxlipo.cellPercent();

    if (percent < 0) percent = 0;
    if (percent > 100) percent = 100;

    return static_cast<char>(round(percent));  // char in 0–100 range
}
