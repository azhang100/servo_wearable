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
  if (inputType == "tTemp") {
    settTemp(functionInput.toInt());
    functionInput = "";
    inputType = "";
  }
  else if (inputType == "tPressure") {
    settPressure(functionInput.toInt());
    functionInput = "";
    inputType = "";
  }
  else if (inputType == "tegco2") {
    settegco2(functionInput.toInt());
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

void checkInput2() {
  static String inputType = ""; // change to char[] to increase speed
  static String functionInput = "";
  while (Serial2.available()) {
    char c = Serial2.read();
    if (c == '[') { // start delim
      inputType = "";
    } else if (c == '=') { // end delim
      break;
    } else              {
      inputType += c;
    }
  }
  while (Serial2.available()) {
    char c = Serial2.read();
    if (c != ']') { // end delim
      functionInput += c;
    }
  }
  if (inputType == "tTemp") {
    settTemp(functionInput.toInt());
    functionInput = "";
    inputType = "";
  }
  else if (inputType == "tPressure") {
    settPressure(functionInput.toInt());
    functionInput = "";
    inputType = "";
  }
  else if (inputType == "tegco2") {
    settegco2(functionInput.toInt());
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
