#ifndef Sweep_h
#define Sweep_h

#include "Arduino.h"
//#include "util.h"

String info = "";
int numEnds = 0;
String egco2 = "";
float inEGCO2;
float setPointSweep = 40;
float KpSweep = .03;
float KiSweep = 0;
float KdSweep = 0;
float currentTimeSweep;
float previousTimeSweep;
float cumErrorSweep;
float sweep = 1;
float rawLowEGCO2 = 19.6;
float rawHighEGCO2 = 35;
float referenceLowEGCO2 = 25.9;
float referenceHighEGCO2 = 44.2;
float rateErrorSweep;
float firstTimeSweep;
float errorSweep = 0;

void setupSweep(){
  Serial2.begin(9600);
  firstTimeSweep = millis();
  float currentTimeSweep = millis() - firstTimeSweep;
  float previousTimeSweep = currentTimeSweep;
  float cumErrorSweep = 0;
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
  for(int i = 0; i < info.length() - 1; i++){
    if(info.charAt(i) == '[' && info.charAt(i+1) == 'e'){
      egco2 = "";
      i += 7;
      while(info.charAt(i) != ']'){
        egco2 += info.charAt(i);
        i++;
      }
    }
  }
  DBSERIAL.print("egco2= ");
  //DBSERIAL.println(egco2);
  inEGCO2 = egco2.toFloat() * 7.6;
  inEGCO2 = ((inEGCO2-rawLowEGCO2)*(referenceHighEGCO2-referenceLowEGCO2))/(rawHighEGCO2-rawLowEGCO2)+referenceLowEGCO2;
  DBSERIAL.println(inEGCO2);
  
  //DBSERIAL.print(info);
  //BTSERIAL.print(info);
}

float piSweep(){
  previousTimeSweep = currentTimeSweep;
  currentTimeSweep = millis();
  float elapsedTimeSweep = currentTimeSweep - previousTimeSweep;
  float lastErrorSweep = errorSweep;
  float errorSweep = -setPointSweep + (inEGCO2);
  rateErrorSweep = (errorSweep - lastErrorSweep)/elapsedTimeSweep;
  cumErrorSweep += errorSweep * elapsedTimeSweep;
  if(elapsedTimeSweep > 10){
    KiSweep = 0.000006;
  }
  float outputSweep = KpSweep * errorSweep + KiSweep * cumErrorSweep + KdSweep * rateErrorSweep;
  //DBSERIAL.print("outputSweep2= ");
  //DBSERIAL.println(outputSweep);
  if(outputSweep <  inEGCO2 * 0.013){
    outputSweep =  inEGCO2 * 0.013;
    if(outputSweep < 0.1){
      outputSweep = 0.1;
    }
  }
//  else if (outputSweep > 2){ //ADDED THIS IN DK IF OK
//    outputSweep = 2;
//  }
  //DBSERIAL.print("outputSweep= ");
  //DBSERIAL.println(outputSweep);
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
