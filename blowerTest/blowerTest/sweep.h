#ifndef Sweep_h
#define Sweep_h

#include "Arduino.h"
#include "util.h"

String info = "";
int numEnds = 0;
String egco2 = "";

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
      
    }
  }
  return egco2.toFloat();
}
#endif
