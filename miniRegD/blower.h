#ifndef Blower_h
#define Blower_h

#include "Arduino.h"
#include "util.h"

#define PIN_PWM1 PB8
#define PIN_PWM2 PB9

void spinBlower(int b1, int b2){
  analogWrite(PIN_PWM1, b1);
  analogWrite(PIN_PWM2, b2);
}

void setupBlower(){
  PRINT1("Setting up blower");
  pinMode(PIN_PWM1, OUTPUT);
  pinMode(PIN_PWM2, OUTPUT);

  PRINT1("blower 1");
  spinBlower(255,0);
  delay(1000);
  PRINT1("blower 2");
  spinBlower(0,255);
  delay(1000);
  PRINT1("stop");
  spinBlower(0,0);
  PRINT1("done");
}

void loopBlower(){
  spinBlower(100,0);
}

#endif
