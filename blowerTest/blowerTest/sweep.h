#ifndef Sweep_h
#define Sweep_h

#include "Arduino.h"
//#include "util.h"

String info = "";
int numEnds = 0;
String egco2 = "";
float setPointSweep = 30;
float KpSweep = 1;
float KiSweep = 0;
float KdSweep = 0;
float currentTimeSweep;
float previousTimeSweep;
float cumErrorSweep;
float rateErrorSweep;
float firstTimeSweep;
float errorSweep = 0;
float sweep = 1;

void setupSweep(){
  Serial2.begin(9600);
  firstTimeSweep = millis();
  float currentTimeSweep = millis() - firstTimeSweep;
  float previousTimeSweep = currentTimeSweep;
  float cumError = 0;
}

void setsweepKp(float newKpSweep){
  KpSweep = newKpSweep;
}

void setsweepKi(float newKiSweep){
  KiSweep = newKiSweep;
}

void setsweepKd(float newKdSweep){
  KdSweep = newKdSweep;
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
  Serial.print(info);
}

float piSweep(){
  previousTimeSweep = currentTimeSweep;
  currentTimeSweep = millis();
  float elapsedTimeSweep = currentTimeSweep - previousTimeSweep;
  float lastErrorSweep = errorSweep;
  float errorSweep = setPointSweep - egco2.toFloat();
  cumErrorSweep += errorSweep * elapsedTimeSweep;
  rateErrorSweep = (errorSweep - lastErrorSweep)/elapsedTimeSweep;
  float outputSweep = KpSweep * errorSweep + KiSweep * cumErrorSweep + KdSweep * rateErrorSweep;
  return outputSweep;
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
      sweep = piSweep();
    }
  }
  return sweep;
}
#endif
