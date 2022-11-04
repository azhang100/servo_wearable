#include "Arduino.h"

void setup() {
  Serial.begin(9600);
  Serial2.begin(9600);
}

void loop() {
  while(Serial.available()){
    Serial2.write(Serial.read());
  }
  while(Serial2.available()){
    Serial.write(Serial2.read());
  }
  
  delay(2000);
}
