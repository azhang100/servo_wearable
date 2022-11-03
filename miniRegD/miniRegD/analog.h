#ifndef Analog_h
#define Analog_h

#include "Arduino.h"
#include "util.h"
#include "comms.h"


#define PIN_PUMP PA12
#define PIN_HEATER PA11
#define PIN_PRESSURE1 PA4
#define GAIN_PRESSURE1 100
#define PIN_PRESSURE2 PA5
#define PIN_PRESSURE3 PA6
#define PIN_PRESSURE4 PA7
#define senseVoltage PA0

float heaterVal = 0;
float pumpVal = 0;
const float pressureProp = 1;
const float heaterProp = 1;
float pressure = 0;
float tPressure = 760;

int maxHeater = 255;
int minHeater = 0;
int maxPump = 255;
int minPump = 0;

void setupAnalog() {
  PRINT1("setting up analog");
  pinMode(PIN_PUMP, OUTPUT);
  pinMode(PIN_HEATER, OUTPUT);
  pinMode(PIN_PRESSURE1, INPUT);
  pinMode(PIN_PRESSURE2, INPUT);
  pinMode(PIN_PRESSURE3, INPUT);
  pinMode(PIN_PRESSURE4, INPUT);
  PRINT1("turning on pump and heater");
  digitalWrite(PIN_PUMP, 255); // approx 30% power // TODO: it freezes here
  digitalWrite(PIN_HEATER, 255); // approx 30% power
  delay(1000);
  digitalWrite(PIN_PUMP, 0);
  digitalWrite(PIN_HEATER, 0);
  PRINT1("done");
}

void loopAnalog() {
  int p1 = analogRead(PIN_PRESSURE1);
  int p2 = analogRead(PIN_PRESSURE2);
  int p3 = analogRead(PIN_PRESSURE3);
  int p4 = analogRead(PIN_PRESSURE4);

  SLOWPRINT4NL("p1:", p1, ",p2:", p2); SLOWPRINT4(", p3:", p3, ",p4:", p4);
}

float measureVoltage() {
  float voltage = analogRead(senseVoltage);
  voltage = (2 * 3.3 * voltage)/4096;
  return voltage;
}

void measureChamberPressure() {
  float pressureMilliVolts = analogRead(PIN_PRESSURE4)*3300/4096;
  float voltage = 5; //measureVoltage();
  float maxVol = 0.9 * voltage*1000;
  float minVol = 0.1 * voltage*1000;
  pressure = map(pressureMilliVolts, minVol, maxVol, 0, 7500);
  sendCommand("pressureChamber", String(pressure));
}



void measuretPressure() {
  sendCommand("tPressure", String(tPressure));
}

void settPressure() {
  static String serialBuf = "";
  while (Serial.available()) {
    char c = Serial.read();
    if (c == ']') { // end delim
      tPressure = serialBuf.toInt();
      serialBuf = "";
    } else              {
      serialBuf += c;
    }
  }
}

void measureHeaterPWM() {
  sendCommand("heaterPWM", String(heaterVal));
}


void measurePumpPWM() {
  sendCommand("pumpPWM", String(pumpVal));
}

void setHeaterPWM(float temp, float tTemp) { //seems to max out temp at ~23.85 with blower off and running full power
  if (tTemp - temp > 1) {                   //seems to max out at ~2 with blower on and running full power
    heaterVal = maxHeater;
    digitalWrite(PIN_HEATER, heaterVal);
  }
  else if (temp < tTemp) {
    float tempHeaterVal = heaterProp * (tTemp - temp);
    if (tempHeaterVal > maxHeater) {
      heaterVal = maxHeater;
    }
    else if (tempHeaterVal < minHeater) {
      heaterVal = minHeater;
    }
    else {
      heaterVal = tempHeaterVal;
    }
    digitalWrite(PIN_HEATER, heaterVal);
  }
  else {
    heaterVal = minHeater;
    digitalWrite(PIN_HEATER, 0);
  }
}

void setPumpPWM(bool pump) { //doesn't seem to change pressure -- sensor in differeent compartment???
  if (tPressure - pressure > 1) {
    pumpVal = maxPump;
    digitalWrite(PIN_PUMP, pumpVal);
  }
  else if (pressure < tPressure) {
    float tempPumpVal = pressureProp * (tPressure - pressure);
    if (tempPumpVal > maxPump) {
      pumpVal = maxPump;
    }
    else if (tempPumpVal < minPump) {
      pumpVal = minPump;
    }
    else {
      pumpVal = tempPumpVal;
    }
    digitalWrite(PIN_PUMP, pumpVal);
  }
  else {
    pumpVal = minPump;
    digitalWrite(PIN_PUMP, 0);
  }
}

void checkPumpPWM() {
  setPumpPWM(pressure < tPressure);
}




#endif
