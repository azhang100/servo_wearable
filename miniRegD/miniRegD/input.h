#ifndef Input_h
#define Input_h

#include "Arduino.h"
#include <Wire.h>

void checkInput(){
  static String serialBuf = ""; // change to char[] to increase speed
  while (Serial.available()){
    char c = Serial.read();
      if (c == '['){ // start delim
      serialBuf = ""; 
    } else if (c == '='){ // end delim
        if(serialBuf == "tTemp"){
          settTemp();
        }
        else if(serialBuf == "tPressure"){
          settPressure();
        }
        else if(serialBuf == "tegco2"){
          settegco2();
        }
        else {
          PRINT2("ERROR: INVALID INPUT", serialBuf);
        }
    } else              { 
      serialBuf += c; 
    }
  }
}
void checkInput2(){
  static String serialBuf = ""; // change to char[] to increase speed
  while (Serial1.available()){
    char c = Serial1.read();
      if (c == '['){ // start delim
      serialBuf = ""; 
    } else if (c == '='){ // end delim
        if(serialBuf == "tTemp"){
          settTemp();
        }
        else if(serialBuf == "tPressure"){
          settPressure();
        }
        else if(serialBuf == "tegco2"){
          settegco2();
        }
        else {
          PRINT2("ERROR: INVALID INPUT", serialBuf);
        }
    } else              { 
      serialBuf += c; 
    }
  }
}


#endif
