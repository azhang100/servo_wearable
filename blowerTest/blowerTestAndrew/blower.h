#ifndef Blower_h
#define Blower_h

#include "Arduino.h"
#include "util.h"


//ARDUINO CODE
//#define PIN_PWM1 9
//#define PIN_PWM2 10
#define PIN_PWM1 PB9
#define PIN_PWM2 PB8

float setPoint = 1;
//PI VALUES WITH PLUGGED HOLE
//float Kp = 700;
//float Ki = 0.05;
float Kp = 15;
float Ki = 0;
float currentTime;
float previousTime;
float cumError;
float firstTime;

void spinBlower(int b1, int b2){
  analogWrite(PIN_PWM1, b1);
  analogWrite(PIN_PWM2, b2);
}

void setupBlower(){
  PRINT1("Setting up blower");
  pinMode(PIN_PWM1, OUTPUT);
  pinMode(PIN_PWM2, OUTPUT);
  pinMode(PB15, OUTPUT);
  pinMode(PB4, OUTPUT);
  pinMode(PA8, INPUT);
  pinMode(PB5, INPUT);

  digitalWrite(PB15, HIGH);
  digitalWrite(PB4, HIGH);
  
  PRINT1("blower 1");
  spinBlower(255,0);
  delay(3000);
  PRINT1("blower 2");
  spinBlower(0,255);
  delay(3000);
  PRINT1("stop");
  spinBlower(0,0);
  PRINT1("done");
  firstTime = millis();
  float currentTime = millis() - firstTime;
  float previousTime = currentTime;
  float cumError = 0;
}

void loopBlower(float flow){
  flow *= -1;
  previousTime = currentTime;
  currentTime = millis() - firstTime;
  float elapsedTime = currentTime - previousTime;
  float error = setPoint - flow;
  cumError += error * elapsedTime;
  float output = (Kp * error) + (Ki * cumError);
  
  if(output < 0){
    output = 0;
  }
  else if(output > 255){
    output = 255;
  }
  spinBlower(output,0);
}

#endif
