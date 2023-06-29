#ifndef Input_h
#define Input_h

#include "Arduino.h"
#include <Wire.h>

void runCommand(String inputType, String inputVal);

void checkInput() { // serChannel is either Serial or Serial2
  static String inputType = ""; // change to char[] to increase speed
  static String inputVal = "";
  static String inputBuf = "";
  while (Serial.available()) {
    char c = Serial.read();
    Serial2.write(c);
    if (c == '[') { // start delim
      inputBuf = "";
    } else if (c == '=') { 
      inputType = inputBuf;
      inputBuf = "";
    } else if (c == ']') { 
      inputVal = inputBuf;
      inputBuf = "";
      runCommand(inputType, inputVal);
    } else              {
      inputBuf += c;
    }
  }
  while (Serial2.available()) {
    char c = Serial2.read();
    Serial.write(c);
    if (c == '[') { // start delim
      inputBuf = "";
    } else if (c == '=') { 
      inputType = inputBuf;
      inputBuf = "";
    } else if (c == ']') { 
      inputVal = inputBuf;
      inputBuf = "";
      runCommand(inputType, inputVal);
    } else              {
      inputBuf += c;
    }
  }
}

void runCommand(String inputType, String inputVal){
  int type = 0;
  if(inputType == "tTemp"){
    type = 1;
  }
  else if(inputType == "tPressure"){
    type = 2;
  }
  else if(inputType == "tegco2"){
    type = 3;
  }
  else if(inputType == "sweepP"){
    type = 4;
  }
  else if(inputType == "sweepI"){
    type = 5;
  }
  else if(inputType == "sweepD"){
    type = 6;
  }
  switch (type){
    case 1:
      settTemp(inputVal.toInt());
      break;
    case 2:
      settPressure(inputVal.toInt());
      break;
    case 3:
      settegco2(inputVal.toInt());
      break;
    case 4:
      setsweepP(inputVal.toFloat());
      break;
    case 5:
      setsweepI(inputVal.toFloat());
      break;
    case 6:
      setsweepD(inputVal.toFloat());
      break;
    case 0:
      PRINT1("ERROR: BLANK INPUT");
      break;
    default:
      PRINT2("ERROR: INVALID INPUT ", inputType);
  }
}

#endif
