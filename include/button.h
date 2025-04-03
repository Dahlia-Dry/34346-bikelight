#ifndef BUTTON_H
#define BUTTON_H
#include <Arduino.h>

#define BUTTON_PIN 1

bool check_button_press(int &lastButtonState);

#endif // BUTTON_H