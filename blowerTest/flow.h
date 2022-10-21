/*
 * @author: andrewzhang100@gmail.com
 * reference: https://raw.githubusercontent.com/PubInv/SFM3X00/master/examples/SFM3400Test/SFM3400Test.ino
 */

#ifndef Flow_h
#define Flow_h

#include "Arduino.h"
#include "util.h"
#include "SFM3X00.h"
#include <Wire.h>

#define FLOW_SENSOR_ADDRESS 0x40 // address of sensor, usually 64 or 0x40 by default

SFM3X00 flowSensor(FLOW_SENSOR_ADDRESS); // create insance of sensor with address 

void setupFlow(){
  PRINT1("setting up flow");
  Wire.begin();
  flowSensor.begin(); // initialize sensor values and start measuring flow

  // print various sensor values
  DBSERIAL.println();
  DBSERIAL.print("sensor serial number: ");
  DBSERIAL.println(flowSensor.serialNumber, HEX);
  DBSERIAL.print("sensor article number: ");
  DBSERIAL.println(flowSensor.articleNumber, HEX);
  DBSERIAL.println();
  DBSERIAL.print("read scale factor: ");
  DBSERIAL.println(flowSensor.flowScale);
  DBSERIAL.print("read flow offset: ");
  DBSERIAL.println(flowSensor.flowOffset);
  PRINT1("done");
}

void loopFlow(){
  float flow = flowSensor.readFlow(); // read flow from sensor and print

  if(flowSensor.checkRange(flow)){
    DBSERIAL.print("flow exceeded sensor limits:  ");
    DBSERIAL.print(flow);
    DBSERIAL.println(" slm");
  } else {
    DBSERIAL.print("flow : ");
    DBSERIAL.print(flow);
    DBSERIAL.println(" slm");
  }
}

#endif
