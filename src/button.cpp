#include "button.h"

bool check_button_press(int &lastButtonState) {
  int buttonState = digitalRead(BUTTON_PIN);
  bool buttonPressed = false;
  if (lastButtonState != buttonState) { // state changed
    delay(50); // debounce time

    // Read the button state again after debounce delay
    buttonState = digitalRead(BUTTON_PIN);
    if (lastButtonState != buttonState) {
      if (buttonState == LOW) { // Button is pressed
        Serial.println("The button pressed event");
        buttonPressed = true;
      } else { // Button is released
        Serial.println("The button released event");
      }
      lastButtonState = buttonState; // Update the last button state
    }
  }
  return buttonPressed; // Return true if the button was pressed
}
