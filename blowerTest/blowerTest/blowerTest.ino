#include "Arduino.h"
#include "blower.h"
#include "util.h"
#include "flow.h"


void setup() {
  delay(1000);
  setupUtil();
  //Wire.begin();
  delay(1000);
  delay(1000);
  setupBlower();
  delay(1000);
  delay(1000);
  setupFlow();
}

void loop() {
  float flow = loopFlow();
  loopBlower(flow);
}
