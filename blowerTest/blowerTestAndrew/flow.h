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
int timer = 0;
void setupFlow(){
  Wire.begin();
  flowSensor.begin(); // initialize sensor values and start measuring flow

  
//  BTSERIAL.print("sensor serial number: ");
//  BTSERIAL.println(flowSensor.serialNumber, HEX);
//  BTSERIAL.print("sensor article number: ");
//  BTSERIAL.println(flowSensor.articleNumber, HEX);
//  BTSERIAL.println();
//  BTSERIAL.print("read scale factor: ");
//  BTSERIAL.println(flowSensor.flowScale);
//  BTSERIAL.print("read flow offset: ");
//  BTSERIAL.println(flowSensor.flowOffset);
  PRINT1("done");
}

float loopFlow(){
  timer++;
  float flow = flowSensor.readFlow(); // read flow from sensor and print

  if(flowSensor.checkRange(flow)){
//    DBSERIAL.print("flow exceeded sensor limits:  ");
//    DBSERIAL.print(flow);
//    DBSERIAL.println(" slm");
//    BTSERIAL.print("flow exceeded sensor limits:  ");
//    BTSERIAL.print(flow);
//    BTSERIAL.println(" slm");
  } else {
//    DBSERIAL.print("flow : ");
      if(timer >= 100){
        DBSERIAL.print(" ");
        DBSERIAL.println(flow * -1);
        timer = 0;
      }
//    DBSERIAL.println(" slm");
//    BTSERIAL.print("flow : ");
//    BTSERIAL.print(flow);
//    BTSERIAL.println(" slm");
delay(2);
  }
  return flow;
}

#endif
