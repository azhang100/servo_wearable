#ifndef Analog_h
#define Analog_h

#include "Arduino.h"

#define PIN_PRESSURE4 PA7

void setup() {
  Serial.begin(9600);

}

void loop() {
  // put your main code here, to run repeatedly:
  int p4 = analogRead(PIN_PRESSURE4);
  Serial.println(p4);
  delay(1000);
}


#endif
