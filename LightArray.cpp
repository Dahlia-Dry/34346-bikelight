#include "LightArray.h"

LightArray::LightArray(uint8_t indicatorPin) : _indicatorPin(indicatorPin) {}

void LightArray::setupLights() {
    pinMode(_indicatorPin, OUTPUT);
    digitalWrite(_indicatorPin, LOW);
}

void LightArray::flash(int times, int onDuration, int offDuration) {
    for (int i = 0; i < times; i++) {
        digitalWrite(_indicatorPin, HIGH);
        delay(onDuration);
        digitalWrite(_indicatorPin, LOW);
        delay(offDuration);
    }
}

void LightArray::showUnlockLight() {
    flash(1, 200, 100);
}

void LightArray::showLockLight() {
    flash(2, 200, 100);
}

void LightArray::showDeniedLight() {
    flash(5, 100, 100);
}
