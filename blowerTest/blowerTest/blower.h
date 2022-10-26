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
float Kp = 1;
float Ki = 0.01;
float currentTime;
float previousTime;
float cumError = 0;

void spinBlower(int b1, int b2){
  digitalWrite(PIN_PWM1, b1);
  digitalWrite(PIN_PWM2, b2);
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
  float currentTime = millis();
  float previousTime = currentTime;
}

void loopBlower(float flow){
  flow = abs(flow);
  previousTime = currentTime;
  currentTime = millis();
  float elapsedTime = currentTime - previousTime;
  float error = setPoint - flow;
  cumError += error * elapsedTime;
  float output = Kp * error + Ki * cumError;
  if(output < 0){
    output = 0;
  }
  else if(output > 255){
    output = 255;
  }
  spinBlower(255,0);
  //delay(3000);
  //int blow1 = digitalRead(PA8);
  //int blow2 = digitalRead(PB5);
  //PRINT3NL(blow1, "   &  ", blow2);
  //spinBlower(0,255);
  //blow1 = digitalRead(PA8);
  //blow2 = digitalRead(PB5);
  //PRINT3NL(blow1, "   &  ", blow2);
  //delay(3000);
}

#endif
