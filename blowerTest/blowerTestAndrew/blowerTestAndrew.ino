#include "Arduino.h"
#include "blower.h"
#include "util.h"
#include "flow.h"
#include "sweep.h"


void setup() {
  delay(1000);
  setupUtil();
  //Wire.begin();
  delay(1000);
  delay(1000);
  setupFlow();
  delay(1000);
  delay(1000);
  setupSweep();
  delay(1000);
  delay(1000);
  setupBlower();
}

void loop() {
  float flow = loopFlow();
  loopBlower(flow);
  float sweep = loopSweep();  
}
