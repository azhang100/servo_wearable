#ifndef Bluetooth_h
#define Bluetooth_h // disabled (swap order of define and ifndef)


#include "Arduino.h"
#include "util.h"

#define START_DELIM '['
#define END_DELIM ']'
#define SPLIT_ DELIM '='

//=======================Serial choose==================

#define BTSERIAL Serial1// long-range comms
#define IS_SERIAL_DIRECT BTSERIAL == SERIAL
#define BTSERIAL2 Serial// internal comms


//=======================================================

void setupComms();
void acknowledgeCommand(String command, String arg);
void sendCommand(const String& command, const String& arg);
void loopComms(){};

//=======================================================

void setupComms(){
  PRINT1("setting up comms");
  if (IS_SERIAL_DIRECT == false){
    DBSERIAL.println("set up BT");
    BTSERIAL.begin(9600);
    BTSERIAL.println("btSerial online");
    DBSERIAL2.print("set up BT2");
    BTSERIAL2.begin(9600);
    BTSERIAL2.println("btSerial2 online");
  }
  PRINT1("done");
}

void sendCommand(const String& command, const String& arg){
  BTSERIAL.print(START_DELIM);
  BTSERIAL.print(command);
  BTSERIAL.print(SPLIT_DELIM);
  BTSERIAL.print(arg);
  BTSERIAL.print(END_DELIM);
  BTSERIAL2.print(START_DELIM);
  BTSERIAL2.print(command);
  BTSERIAL2.print(SPLIT_DELIM);
  BTSERIAL2.print(arg);
  BTSERIAL2.print(END_DELIM);
}

void acknowledgeCommand(String command, String arg){
  BTSERIAL.print("got cmd:"); BTSERIAL.print(command); BTSERIAL.print("="); BTSERIAL.println(arg);
  if (IS_SERIAL_DIRECT == false){DBSERIAL.print("got cmd:");DBSERIAL.print(command);DBSERIAL.print("=");DBSERIAL.println(arg);}
}


#endif
