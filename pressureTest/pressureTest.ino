#ifndef Analog_h
#define Analog_h

#include "Arduino.h"

#define PIN_PRESSURE4 PA7

void setup() {
  Serial.begin(9600);

}

void loop() {
  // put your main code here, to run repeatedly:
  float p4 = analogRead(PIN_PRESSURE4);
  float pressure = map(p4, 620.6, 5585.5, 0, 7500);
  Serial.println(pressure);
  delay(1000);
}


#endif
