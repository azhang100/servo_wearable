#ifndef Input_h
#define Input_h

#include "Arduino.h"
#include <Wire.h>

void runCommand(String inputType, String inputVal, int serial);

void checkInput() {
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
      runCommand(inputType, inputVal, 1);
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
      runCommand(inputType, inputVal, 2);
    } else              {
      inputBuf += c;
    }
  }
}

void runCommand(String inputType, String inputVal, int serial){
  int type = 0;
  if(inputType == "sensorHealth"){
    type = 1;
  }
  else if(inputType == "manualSweep"){
    type = 2;
  }
  else if(inputType == "suggestSweep"){
    type = 3;
  }
  else if(inputType == "blowerP"){
    type = 4;
  }
  else if(inputType == "blowerI"){
    type = 5;
  }
  else if(inputType == "blowerD"){
    type = 6;
  }
  switch (type){
    case 1:
      setSensorHealth(inputVal.toInt());
      break;
    case 2:
      setmanualSweep(inputVal.toFloat());
      break;
    case 3:
      settSweep(inputVal.toFloat());
      break;
    case 4:
      setblowerKp(inputVal.toFloat());
      break;
    case 5:
      setblowerKi(inputVal.toFloat());
      break;
    case 6:
      setblowerKd(inputVal.toFloat());
      break;
    case 0:
//      PRINT1("ERROR: BLANK INPUT");
        break;
    default:
      PRINT2("ERROR: INVALID INPUT ", inputType);
  }
}


#endif
