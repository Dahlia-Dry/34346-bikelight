#include <Arduino.h>
#include "button.h"
#include "accelerometer.h"
#include "light.h"
#include "photoresistor.h"

// variable declarations
int lastButtonState;
int light_state;
bool button_pressed;
bool is_moving;
bool is_daytime;
String mode = "sleep"; //modes: "sleep","active","park"
String mode_previous = "sleep";
String location;
unsigned long previousMillis = 0;

// timer intervals in milliseconds 
const unsigned long park_geolocate_interval = 5000;


void setup() {
  //initialize serial
  Serial.begin(115200);
  //button setup
  pinMode(BUTTON_PIN, INPUT_PULLUP);  // enable the internal pull-up resistor
  lastButtonState = digitalRead(BUTTON_PIN);
  button_pressed = false;
  
}

void loop() {
  // variables to update regardless of operation mode
  //check if button is pressed
  button_pressed = check_button_press(lastButtonState);
  //check if moving
  is_moving = check_moving();
  // initiate actions based on mode
  if (mode == "active"){
    if(mode_previous != mode){
        
    }

  }
  else if(mode =="park"){
    if(mode_previous != mode){
      light_off();
    }
  }
  else{ //sleep
    if(button_pressed | is_moving){
      mode="active";
      mode_previous = mode;
    }
    
  }
}