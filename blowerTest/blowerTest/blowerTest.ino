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
  setupBlower();
  delay(1000);
  delay(1000);
  setupFlow();
  setupSweep();
}

void loop() {
  //float flow = loopFlow();
  //loopBlower(flow);
  float sweep = loopSweep();  
}
