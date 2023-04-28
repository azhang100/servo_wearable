#ifndef Input_h
#define Input_h

#include "Arduino.h"
#include <Wire.h>

void checkInput(){ // these two functions should ideally be deleted, and call checkInput
  checkInput(Serial);
}

void checkInput2(){
  checkInput(Serial2);
}

void checkInputRaw(Serial serChannel) { // serChannel is either Serial or Serial2
  static String inputType = ""; // change to char[] to increase speed
  static String inputVal = "";
  static String inputBuf = "";
  while (serChannel.available()) {
    char c = serChannel.read();
    if (c == '[') { // start delim
      inputBuf = "";
    } else if (c == '=') { 
      inputType = inputBuf
      inputBuf = "";
    } else if (c == ']') { 
      inputVal = inputBuf
      inputBuf = "";
      runCommand(inputType, inputVal)
    } else              {
      inputBuf += c;
    }
  }
}

void runCommand(String inputType, String inputVal){
  switch (inputType){
    case "tTemp":
      settTemp(functionInput.toInt());
      break;
    case "tPressure":
      settPressure(functionInput.toInt());
      break;
    case "tTemp":
      settTemp(functionInput.toInt());
      break;
    case "tegco2":
      settegco2(functionInput.toInt());
      break;
    case "":
      PRINT2("ERROR: BLANK INPUT");
      break;
    default:
      PRINT2("ERROR: INVALID INPUT ", inputType);
}

#endif
