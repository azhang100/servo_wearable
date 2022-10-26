#include "Arduino.h"

#define PIN_PUMP PA15
#define PIN_HEATER PB3

void setup() {

 // pinMode(PIN_HEATER, OUTPUT);
  pinMode(PIN_PUMP, OUTPUT);
  //pinMode(PC13, OUTPUT);
}

void loop() {
  
  //digitalWrite(PIN_HEATER, HIGH);
  digitalWrite(PIN_PUMP, HIGH);
  //digitalWrite(PC13, HIGH);

  delay(5000);

  //digitalWrite(PIN_HEATER, LOW);
  digitalWrite(PIN_PUMP, LOW);
  //digitalWrite(PC13, LOW);

  delay(5000);
}
