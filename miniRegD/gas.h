 * @author: andrewzhang100@gmail.com
 * reference: https://raw.githubusercontent.com/sparkfun/SparkFun_STC3x_Arduino_Library/
 *                    main/examples/Example1_BasicReadings/Example1_BasicReadings.ino
 * Click here to get the library: http://librarymanager/All#SparkFun_STC3x
 */

#ifndef Gas_h
#define Gas_h

float stc3Gas; // TODO: should be static
float stc3Temp; // TODO: should be static

float tTemp = 39;
float temp = 22;

float tegco2 = 20;
const float sweepProp = 1;
//CURRENT
float KpSweep = .02;
float KiSweep = 0.000002;
float KdSweep = 0;
//OLD
//float KpSweep = .015;
//float KiSweep = 0.000003;
//float KdSweep = 0;
float currentTimeSweep;
float previousTimeSweep;
float cumErrorSweep;
float sweep = 1;
float rawLowEGCO2 = 6.9;
float rawHighEGCO2 = 18.27;
float referenceLowEGCO2 = 21.5;
float referenceHighEGCO2 = 31;
float rateErrorSweep;
float firstTimeSweep;
float errorSweep = 0;
float testTime;
float outputSweep;
float egco2;
float pressureCorrectedegco2;

#include "SparkFun_STC3x_Arduino_Library.h"
#include "analog.h"
#include "util.h"
#include "comms.h"

STC3x mySensor;

void setupSweep(){
  Serial2.begin(9600);
  firstTimeSweep = millis();
  float currentTimeSweep = millis() - firstTimeSweep;
  float previousTimeSweep = currentTimeSweep;
  float cumErrorSweep = 0;
}

void setupGas(){
  PRINT1("setting up STC3x");
  //mySensor.enableDebugging(); // Uncomment this line to get helpful debug messages on Serial

  if (mySensor.begin() == false) { PRINT1("ERROR: STC3x not detected. Please check wiring."); }

  //We need to tell the sensor what binary gas and full range we are using
  //Possible values are:
  //  STC3X_BINARY_GAS_CO2_N2_100   : Set binary gas to CO2 in N2.  Range: 0 to 100 vol%
  //  STC3X_BINARY_GAS_CO2_AIR_100  : Set binary gas to CO2 in Air. Range: 0 to 100 vol%
  //  STC3X_BINARY_GAS_CO2_N2_25    : Set binary gas to CO2 in N2.  Range: 0 to 25 vol%
  //  STC3X_BINARY_GAS_CO2_AIR_25   : Set binary gas to CO2 in Air. Range: 0 to 25 vol%

  if (mySensor.setBinaryGas(STC3X_BINARY_GAS_CO2_AIR_100) == false) { PRINT1("ERROR: STC3x set gas"); }
  PRINT1("done");
}

void loopGas() {
  if (mySensor.measureGasConcentration()) { // measureGasConcentration will return true when fresh data is available
    stc3Gas = mySensor.getCO2();
    stc3Temp = mySensor.getTemperature();
    PRINT4("CO2(%):", stc3Gas, "\tTemperature(C):", stc3Temp);
  } else { PRINT1("Error: no gas measurement"); }
}

void setsweepP(float newsweepP){
  KpSweep = newsweepP;
}

void setsweepI(float newsweepI){
  KiSweep = newsweepI;
}

void setsweepD(float newsweepD){
  KdSweep = newsweepD;
}

void printPID(){
  sendCommand("sweepP", String(KpSweep));
  sendCommand("sweepD", String(KiSweep));
  sendCommand("sweepI", String(KdSweep));
}
void measureGas() {
  if (mySensor.measureGasConcentration()) { // measureGasConcentration will return true when fresh data is available
    stc3Gas = mySensor.getCO2();
    //if(stc3Gas < 0){ stc3Gas = 0; }
    pressureCorrectedegco2 = stc3Gas * (pressure/100);
    egco2 = stc3Gas * 7.6;
    if(egco2 < -12.85){
    egco2 = ((5-0)/(-12.85+34.5))*egco2 + (((5-0)/(-12.85+34.5))*34.5);
  }
    else{
      egco2 = ((egco2-rawLowEGCO2)*(referenceHighEGCO2-referenceLowEGCO2))/(rawHighEGCO2-rawLowEGCO2)+referenceLowEGCO2;
    }
    if(egco2 < 0){
      egco2 = 0;
    }
    sendCommand("egco2", String(egco2));
    sendCommand("pressureCorrectedEGCO2", String(pressureCorrectedegco2));
  } else { PRINT1("Error: no gas measurement"); }
}

void measureTemp(){
  //if (mySensor.measureGasConcentration()) { // measureGasConcentration will return true when fresh data is available
    temp = mySensor.getTemperature();
    sendCommand("temp", String(temp));
  //} else { PRINT1("Error: no temp measurement"); }
}

void settTemp(int newtTemp){
  tTemp = newtTemp;
}
void measuretTemp(){
  sendCommand("tTemp", String(tTemp));
}

void checkHeaterPWM(){
  setHeaterPWM(temp, tTemp);
}

void settegco2(int newtegco2){
  tegco2 = newtegco2;
}

void measuretegco2(){
  sendCommand("tegco2", String(tegco2));
}


void setSweep(){
  //changeSet();
    previousTimeSweep = currentTimeSweep;
    currentTimeSweep = millis();
    float elapsedTimeSweep = currentTimeSweep - previousTimeSweep;
    float lastErrorSweep = errorSweep;
    float errorSweep = -tegco2 + (egco2);
//    if(abs(errorSweep) > 0.3){
      rateErrorSweep = (errorSweep - lastErrorSweep)/elapsedTimeSweep;
      cumErrorSweep += errorSweep * elapsedTimeSweep;
      outputSweep = KpSweep * errorSweep + KiSweep * cumErrorSweep + KdSweep * rateErrorSweep;
      if(outputSweep <  egco2 * 0.015){
        outputSweep =  egco2 * 0.015;
        if(outputSweep < 0.1){
          outputSweep = 0.1;
        }
      }
//    }
//    if(cumErrorSweep < -1 & errorSweep < 0){
//      cumErrorSweep = 0;
//    }
    //outputSweep = 1;
}

void suggestSweep(){
  setSweep();
  sendCommand("suggestSweep", String(outputSweep));
}


#endif
