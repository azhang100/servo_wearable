/*
 * @author: andrewzhang100@gmail.com
 * reference: http://librarymanager/All#SparkFun_SHTC3
 */

#ifndef Humidity_h
#define Humidity_h

#include "comms.h"

#include "SparkFun_SHTC3.h"
SHTC3 mySHTC3; // Declare an instance of the SHTC3 class

float shtc3Humidity; // TODO: should be static
float shtc3Temp; // TODO: should be static
int health = 10000000;

String errorDecoder(SHTC3_Status_TypeDef message) { // The errorDecoder function prints "SHTC3_Status_TypeDef" resultsin a human-friendly way
  switch(message) {
    case SHTC3_Status_Nominal : return "Nominal"; break;
    case SHTC3_Status_Error : return "Error"; break;
    case SHTC3_Status_CRC_Fail : return "CRC Fail"; break;
    default : return "Unknown return code"; break;
  }
}

void printInfo() {
  if(mySHTC3.lastStatus == SHTC3_Status_Nominal) { // You can also assess the status of the last command by checking the ".lastStatus" member of the object
    shtc3Humidity = mySHTC3.toPercent(); // "toPercent" returns the percent humidity as a floating point number
    shtc3Temp = mySHTC3.toDegC(); // "toDegF" and "toDegC" return the temperature as a flaoting point number in deg F and deg C respectively 
    PRINT4NL("RH(%) = ", shtc3Humidity, ", T(*C) = ", shtc3Temp);
  } else {
    PRINT2("Update failed, error: ", errorDecoder(mySHTC3.lastStatus)); 
  }
}

void setupHumidity() {
  PRINT1("setting up SHTC3");
  
  String statusCode = errorDecoder(mySHTC3.begin()); 
  // Most SHTC3 functions return a variable of the type "SHTC3_Status_TypeDef" to indicate the status of their execution
  // To start the sensor you must call "begin()", the default settings use Wire (default Arduino I2C port)
  PRINT2("Beginning sensor. Result = ", statusCode);

  PRINT1("done");
}

void loopHumidity() {
  SHTC3_Status_TypeDef result = mySHTC3.update();             // Call "update()" to command a measurement, wait for measurement to complete, and update the RH and T members of the object
  printInfo();                                                // This function is used to print a nice little line of info to the serial port
}

void measureHumidity(){
  SHTC3_Status_TypeDef result = mySHTC3.update();
  if(mySHTC3.lastStatus == SHTC3_Status_Nominal) { // You can also assess the status of the last command by checking the ".lastStatus" member of the object
    shtc3Humidity = mySHTC3.toPercent(); // "toPercent" returns the percent humidity as a floating point number
    shtc3Temp = mySHTC3.toDegC(); // "toDegF" and "toDegC" return the temperature as a flaoting point number in deg F and deg C respectively 
    sendCommand("humidity", String(shtc3Humidity));
  } else {
    PRINT2("Update failed, error: ", errorDecoder(mySHTC3.lastStatus)); 
  }
}
void checkHealth(){
  if(shtc3Humidity > 95){
    Serial.println("HEALLO");
    health = 00100000;
    sendCommand("sensorHealth", "00100000");
  }
  else{
    health = 10000000;
    sendCommand("sensorHealth", "10000000");
  }
}
#endif
