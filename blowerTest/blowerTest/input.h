#ifndef Input_h
#define Input_h

#include "Arduino.h"
#include <Wire.h>


void checkInput() {
  static String inputType = ""; // change to char[] to increase speed
  static String functionInput = "";
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '[') { // start delim
      inputType = "";
    } else if (c == '=') { // end delim
      break;
    } else              {
      inputType += c;
    }
  }
  while (Serial.available()) {
    char c = Serial.read();
    if (c != ']') { // end delim
      functionInput += c;
    }
  }
  if (inputType == "blowerKp") {
    setblowerKp(functionInput.toFloat());
    functionInput = "";
    inputType = "";
  }
  else if (inputType == "blowerKi") {
    setblowerKi(functionInput.toFloat());
    functionInput = "";
    inputType = "";
  }
  else if (inputType == "blowerKd") {
    setblowerKd(functionInput.toFloat());
    functionInput = "";
    inputType = "";
  }
  else if (inputType == "sweepP") {
    setsweepKp(functionInput.toFloat());
    functionInput = "";
    inputType = "";
  }
  else if (inputType == "sweepI") {
    setsweepKi(functionInput.toFloat());
    functionInput = "";
    inputType = "";
  }
  else if (inputType == "sweepD") {
    setsweepKd(functionInput.toFloat());
    functionInput = "";
    inputType = "";
  }
  else if (inputType == "") {

  }
  else {
    PRINT2("ERROR: INVALID INPUT ", inputType);
    functionInput = "";
    inputType = "";
  }
}


#endif
