#ifndef Sweep_h
#define Sweep_h

#include "Arduino.h"
#include "util.h"

String info = "";
int numEnds = 0;
String egco2 = "";
float setPoint = 30;
float Kp = 1;
float Ki = 0.01;
float currentTime;
float previousTime;
float cumError = 0;

void setupSweep(){
  Serial2.begin(9600);
}

void getSweep(){
  char c = info.charAt(9);
  int i = 9;
  egco2 = "";
  while(c != ']'){
    egco2 += c;
    i++;
    c = info.charAt(i);
  }
}

float piSweep(){
  previousTime = currentTime;
  currentTime = millis();
  float elapsedTime = currentTime - previousTime;
  float error = setPoint - egco2;
  cumError += error * elapsedTime;
  float output = Kp * error + Ki * cumError;
  return output;
}
float loopSweep(){
  String newEGCO2;
  while(Serial2.available()){
    char c = Serial2.read();
    info += c;
    //Serial.write(c);
    if(c == ']'){
      numEnds++;
    }
    if(numEnds == 10){
      getSweep();
      numEnds = 0;
      info = "";
      float sweep = piSweep()
    }
  }
  return sweep;
}
#endif
