#include "Arduino.h"

#define PIN_PUMP PA12
#define PIN_HEATER PA11

void setup() {

  pinMode(PIN_HEATER, OUTPUT);
  pinMode(PIN_PUMP, OUTPUT);
  pinMode(PC13, OUTPUT);
}

void loop() {
  
  digitalWrite(PIN_HEATER, HIGH);
  digitalWrite(PIN_PUMP, HIGH);
  digitalWrite(PC13, HIGH);

  delay(100);

  digitalWrite(PIN_HEATER, LOW);
  digitalWrite(PIN_PUMP, LOW);
  digitalWrite(PC13, LOW);

  delay(100);
}
