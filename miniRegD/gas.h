/*
   @author: andrewzhang100@gmail.com
   reference: https://raw.githubusercontent.com/sparkfun/SparkFun_STC3x_Arduino_Library/
                      main/examples/Example1_BasicReadings/Example1_BasicReadings.ino
   Click here to get the library: http://librarymanager/All#SparkFun_STC3x
*/

#ifndef Gas_h
#define Gas_h

float stc3Gas; // TODO: should be static
float stc3Temp; // TODO: should be static

float tTemp = 39;
float temp = 22;

float tegco2 = 30;

float sweep = 0;
const float sweepProp = 1;

#include "SparkFun_STC3x_Arduino_Library.h"
#include "analog.h"
#include "util.h"
#include "comms.h"

STC3x mySensor;

void setupGas() {
  PRINT1("setting up STC3x");
  //mySensor.enableDebugging(); // Uncomment this line to get helpful debug messages on Serial

  if (mySensor.begin() == false) {
    PRINT1("ERROR: STC3x not detected. Please check wiring.");
  }

  //We need to tell the sensor what binary gas and full range we are using
  //Possible values are:
  //  STC3X_BINARY_GAS_CO2_N2_100   : Set binary gas to CO2 in N2.  Range: 0 to 100 vol%
  //  STC3X_BINARY_GAS_CO2_AIR_100  : Set binary gas to CO2 in Air. Range: 0 to 100 vol%
  //  STC3X_BINARY_GAS_CO2_N2_25    : Set binary gas to CO2 in N2.  Range: 0 to 25 vol%
  //  STC3X_BINARY_GAS_CO2_AIR_25   : Set binary gas to CO2 in Air. Range: 0 to 25 vol%

  if (mySensor.setBinaryGas(STC3X_BINARY_GAS_CO2_AIR_100) == false) {
    PRINT1("ERROR: STC3x set gas");
  }
  PRINT1("done");
}

void loopGas() {
  if (mySensor.measureGasConcentration()) { // measureGasConcentration will return true when fresh data is available
    stc3Gas = mySensor.getCO2();
    stc3Temp = mySensor.getTemperature();
    PRINT4("CO2(%):", stc3Gas, "\tTemperature(C):", stc3Temp);
  } else {
    PRINT1("Error: no gas measurement");
  }
}

void measureGas() {
  if (mySensor.measureGasConcentration()) { // measureGasConcentration will return true when fresh data is available
    stc3Gas = mySensor.getCO2();
    //if(stc3Gas < 0){ stc3Gas = 0; }
    //stc3Gas *= 760;
    sendCommand("egco2", String(stc3Gas));
  } else {
    PRINT1("Error: no gas measurement");
  }
}

void measureTemp() {
  //if (mySensor.measureGasConcentration()) { // measureGasConcentration will return true when fresh data is available
  temp = mySensor.getTemperature();
  sendCommand("temp", String(temp));
  //} else { PRINT1("Error: no temp measurement"); }
}

void settTemp(int input) {
  tTemp = input;
}
void measuretTemp() {
  sendCommand("tTemp", String(tTemp));
}

void checkHeaterPWM() {
  setHeaterPWM(temp, tTemp);
}

void settegco2(int input) {
  tegco2 = input;
}

void measuretegco2() {
  sendCommand("tegco2", String(tegco2));
}

void setSweep(bool sweepNeeded) {
  if (sweepNeeded) {
    sweep = (stc3Gas - tegco2) * sweepProp;
    if (sweep < 0) {
      sweep = 0;
    }
  }
  else {
    sweep = 0;
  }
}

void suggestSweep() {
  setSweep(stc3Gas < tegco2);
  sendCommand("suggestSweep", String(sweep));
}


#endif
