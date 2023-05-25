#ifndef Sweep_h
#define Sweep_h

#include "Arduino.h"
//#include "util.h"

String info = "";
int numEnds = 0;
String egco2 = "";
float inEGCO2;
float setPointSweep = 10;
//CURRENT
float KpSweep = .02;
float KiSweep = 0.000002;
float KdSweep = 0;
//OLD
//float KpSweep = .015;
//float KiSweep = 0.000003;
//float KdSweep = 0;
float currentTimeSweep;
float previousTimeSweep;
float cumErrorSweep;
float sweep = 1;
float rawLowEGCO2 = 13.4;
float rawHighEGCO2 = 20;
float referenceLowEGCO2 = 26;
float referenceHighEGCO2 = 38.2;
float rateErrorSweep;
float firstTimeSweep;
float errorSweep = 0;
float testTime;
float outputSweep;
//bool buildup = true;
//bool ten = true;
//bool twenty = true;
//bool forty = true;


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
  DBSERIAL.print("[egco2=");
  //DBSERIAL.println(egco2);
  inEGCO2 = egco2.toFloat() * 7.6;
  if(inEGCO2 < 2.4){
    inEGCO2 = ((5-0)/(2.4+16.42))*inEGCO2 + (((5-0)/(2.4+16.42))*16.42);
  }
  else{
    inEGCO2 = ((inEGCO2-rawLowEGCO2)*(referenceHighEGCO2-referenceLowEGCO2))/(rawHighEGCO2-rawLowEGCO2)+referenceLowEGCO2;
  }
  if(inEGCO2 < 0){
    inEGCO2 = 0;
  }
  DBSERIAL.print(inEGCO2);
  DBSERIAL.println("]");
}
//void changeSet(){
//  if(millis() - firstTimeSweep > 120000 && ten == true){
//      setPointSweep = 10;
//      cumErrorSweep = 0;
//      ten = false;
//      Serial.println();
//      Serial.println();
//      Serial.println();
//    }
//    if(millis() - firstTimeSweep > 240000 && millis() - firstTimeSweep < 360000){
//      setPointSweep = 1000;
//    }
//    if(millis() - firstTimeSweep > 360000 && twenty == true){
//      cumErrorSweep = 0;
//      twenty = false;
//      setPointSweep = 20;
//      Serial.println();
//      Serial.println();
//      Serial.println();
//    }
//    if(millis() - firstTimeSweep > 480000&& millis() - firstTimeSweep < 600000){
//      setPointSweep = 1000;
//    }
//    if(millis() - firstTimeSweep > 600000 && forty == true){
//      setPointSweep = 40;
//      cumErrorSweep = 0;
//      forty = false;
//      Serial.println();
//      Serial.println();
//      Serial.println();
//    }
//}
float piSweep(){
    //changeSet();
    previousTimeSweep = currentTimeSweep;
    currentTimeSweep = millis();
    float elapsedTimeSweep = currentTimeSweep - previousTimeSweep;
    float lastErrorSweep = errorSweep;
    float errorSweep = -setPointSweep + (inEGCO2);
//    if(abs(errorSweep) > 0.3){
      rateErrorSweep = (errorSweep - lastErrorSweep)/elapsedTimeSweep;
      cumErrorSweep += errorSweep * elapsedTimeSweep;
      outputSweep = KpSweep * errorSweep + KiSweep * cumErrorSweep + KdSweep * rateErrorSweep;
      if(outputSweep <  inEGCO2 * 0.015){
        outputSweep =  inEGCO2 * 0.015;
        if(outputSweep < 0.1){
          outputSweep = 0.1;
        }
      }
//    }
    if(cumErrorSweep < -1 & errorSweep < 0){
      cumErrorSweep = 0;
    }
    //outputSweep = 1;
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
