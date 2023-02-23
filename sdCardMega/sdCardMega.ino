// Source https://github.com/adafruit/Light-and-Temp-logger/blob/d615540bfa0b30aa721960312a46d62161b6c2e8/lighttemplogger.ino
// RX0 = D0, RX1 = D19, RX2 = D17, RX3 = D15

#include <SPI.h>
#include <SD.h>

File logfileA; // the logging file
File logfileB; // the logging file
File logfileC; // the logging file
File logfileD; // the logging file

// for the data logging shield, we use digital pin 10 for the SD cs line
const int chipSelect = 10;

void setup() {
  Serial.begin(9600); Serial1.begin(9600); Serial2.begin(9600); Serial3.begin(9600);
  Serial.println("sdCardTest start");

  if (!SD.begin(chipSelect)) { // see if the card is present and can be initialized:
    Serial.println("Card failed, or not present");
  } Serial.println("card initialized.");

  char filenameA[] = "LOG_A_00.CSV"; // create a new file
  char filenameB[] = "LOG_B_00.CSV";
  char filenameC[] = "LOG_C_00.CSV";
  char filenameD[] = "LOG_D_00.CSV";
  for (uint8_t i = 0; i < 100; i++) {
    filenameA[6] = filenameB[6] = filenameC[6] = filenameD[6] = i / 10 + '0'; 
    filenameA[7] = filenameB[7] = filenameC[7] = filenameD[7] = i % 10 + '0';
    if ((!SD.exists(filenameA)) && (!SD.exists(filenameB)) &&
        (!SD.exists(filenameC)) && (!SD.exists(filenameD))) {
      // only open a new file if it doesn't exist
      logfileA = SD.open(filenameA, FILE_WRITE);
      logfileB = SD.open(filenameB, FILE_WRITE);
      logfileC = SD.open(filenameC, FILE_WRITE);
      logfileD = SD.open(filenameD, FILE_WRITE);
      Serial.print("Logging to: "); Serial.println(filenameA);
      Serial.print("Logging to: "); Serial.println(filenameB);
      Serial.print("Logging to: "); Serial.println(filenameC);
      Serial.print("Logging to: "); Serial.println(filenameD);
      break;  // leave the loop!
    } else {
      Serial.print("Exists:"); Serial.println(filenameA);
    }
  }
  if (! logfileA) {
    Serial.println("couldnt create logfileA");
  }
  if (! logfileB) {
    Serial.println("couldnt create logfileB");
  }
  if (! logfileC) {
    Serial.println("couldnt create logfileC");
  }
  if (! logfileD) {
    Serial.println("couldnt create logfileD");
  }
}

void loop() {
  while (Serial.available()) {
    logfileA.write(Serial.read());
    logfileA.flush();
  } 
  while (Serial1.available()) {
    logfileB.write(Serial1.read());
    logfileB.flush();
  } 
  while (Serial2.available()) {
    logfileC.write(Serial2.read());
    logfileC.flush();
  } 
  while (Serial3.available()) {
    logfileD.write(Serial3.read());
    logfileD.flush();
  }
}
