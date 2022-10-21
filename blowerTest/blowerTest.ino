#include "Arduino.h"
#include "flow.h"
#include "util.h"

void setup() {
  delay(1000);
  setupUtil();
  delay(1000);
  delay(1000);
  setupFlow();
  delay(1000);
  delay(1000);
}

void loop() {
  loopFlow();
  delay(1000);
}
