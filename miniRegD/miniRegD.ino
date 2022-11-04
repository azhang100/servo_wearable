/*

   @author andrewzhang100@gmail.com

   Library: http://dan.drown.org/stm32duino/package_STM32duino_index.json
   Board: Generic STM32F103C Series
   Variant: STM32F103C8 (64K Flash)
   Upload Method: STLink

   Instructions: https://circuitdigest.com/microcontroller-projects/
       getting-started-with-stm32-blue-pill-development-board-stm32f103c8-using-arduino-ide
   (But used STLink)
*/

#include "Arduino.h"
#include "util.h"
#include "analog.h"
//#include "blower.h"
//#include "flow.h"
#include "gas.h"
#include "humidity.h"
#include "input.h"
// #include "comms.h"
// #include "eeprom.h"

void setup() {
  setupUtil();
  setupComms();
  //  setupBlower();
  setupAnalog();
  //  setupFlow();
  setupGas();
  setupHumidity();
  // setupEEPROM();
}

void loop() {

  loopUtil();
  measureGas();
  measuretegco2();
  measureChamberPressure();
  measureHumidity();
  measureTemp();
  checkInput();
  checkInput2();
  measuretTemp();
  measuretPressure();
  checkHeaterPWM();
  checkPumpPWM();
  measureHeaterPWM();
  measurePumpPWM();
  suggestSweep();
  Serial.println();
  Serial2.println();
  // loopComms();
  //  loopBlower();
  //  loopAnalog();
  //  loopFlow();
  //  loopGas();
  //  loopHumidity();
  // loopEEPROM();
  delay(1000); // FOR TESTING PURPOSES
}
