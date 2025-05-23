/*
* Beam Buddy : IoT-Enabled Smart Bike Light
* 34346 - Networking Technologies and Application Development for IoT, Spring 2025
* Group 1
* Code file for bike light LED array functions
*/

#include "esp32-hal-gpio.h"
#include "LightArray.h"

LightArray::LightArray(uint8_t indicatorPin) : _indicatorPin(indicatorPin) {}

void LightArray::setup() { //initialize the light array
    pinMode(_indicatorPin, OUTPUT);
    digitalWrite(_indicatorPin, LOW);
    _indicatorStatus = 0;
}

void LightArray::on(){ //turn lights on
  digitalWrite(_indicatorPin, HIGH);
}

void LightArray::off(){ //turn lights off
  digitalWrite(_indicatorPin, LOW);
}

void LightArray::toggle(){ //change light state (on-> off) or (off-> on)
  digitalWrite(_indicatorPin, !digitalRead(_indicatorPin));
}

void LightArray::flash(int times, int onDuration, int offDuration) { //flash the lights a certain number of times
    for (int i = 0; i < times; i++) {
        digitalWrite(_indicatorPin, HIGH);
        delay(onDuration);
        digitalWrite(_indicatorPin, LOW);
        delay(offDuration);
    }
}

void LightArray::showUnlockLight() { //flash the lights to show RFID unlock
    flash(1, 200, 100);
}

void LightArray::showLockLight() { //flash the lights to show RFID lock
    flash(2, 200, 100);
}

void LightArray::showDeniedLight() { //flash the lights to show RFID card denied
    flash(5, 100, 100);
}
