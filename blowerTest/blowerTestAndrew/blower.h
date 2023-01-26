#ifndef Blower_h
#define Blower_h

#include "Arduino.h"
#include "util.h"
#include "flow.h"


//ARDUINO CODE
//#define PIN_PWM1 9
//#define PIN_PWM2 10
#define PIN_PWM1 PB9
#define PIN_PWM2 PB8
int timerBlow = 0;
float setPoint = 0.5;
//PI VALUES WITH PLUGGED HOLE and setpoint 1
//float Kp = 700;
//float Ki = 0.05;
// 100, .005, 20000
//700, 0, 10000
float Kp = 800;
float Ki = 0.005;
float Kd = 1000;
float currentTime;
float previousTime;
float cumError;
float rateError;
float firstTime;
float error = 0;
void spinBlower(int b1, int b2){
  analogWrite(PIN_PWM1, b1);
  analogWrite(PIN_PWM2, b2);
}

void setupBlower(){
  firstTime = millis();
  float currentTime = millis() - firstTime;
  float previousTime = currentTime;
  float cumError = 0;
}

void setblowerKp(float newKp){
  Kp = newKp;
}

void setblowerKi(float newKi){
  Ki = newKi;
}

void setblowerKd(float newKd){
  Kd = newKd;
}

void updateSetPoint(float newSetPoint){
  setPoint = newSetPoint;
}

void loopBlower(float flow){
  timerBlow++;
  flow *= -1;
  previousTime = currentTime;
  currentTime = millis() - firstTime;
  float elapsedTime = currentTime - previousTime;
  float lastError = error;
  error = setPoint - flow;
  cumError += error * elapsedTime;
  rateError = (error - lastError)/elapsedTime;
  float output = (Kp * error) + (Ki * cumError) + (Kd * rateError);
  
  if(output < 25){
    output = 25;
  }
  else if(output > 255){
    output = 255;
  }
  //PRINT4("output ", output, "prop. ", Kp*error);
  //PRINT4("int. ", Ki * cumError, "der. ", Kd * rateError);
  spinBlower(output,0);
  if(timerBlow >= 100){
        DBSERIAL.print(setPoint);
        timerBlow = 0;
      }
}



void checkInput() {
  static String input = ""; // change to char[] to increase speed
  if (Serial.available()) {
    input = Serial.readString();
  }
  if(input != ""){
    setPoint = input.toFloat();
  }
  input = "";
}

#endif
