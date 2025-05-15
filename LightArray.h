#ifndef LIGHTARRAY_H
#define LIGHTARRAY_H

#include <Arduino.h>

class LightArray {
public:
    explicit LightArray(uint8_t indicatorPin);
    void setup();
    void on();
    void off();
    void toggle();
    void showUnlockLight();
    void showLockLight();
    void showDeniedLight();

private:
    uint8_t _indicatorPin;
    void flash(int times, int onDuration, int offDuration);
};

#endif
