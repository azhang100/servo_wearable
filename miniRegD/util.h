#ifndef Util_h
#define Util_h

#include "Arduino.h"
#include <Wire.h>


// ============== PRINTING ==========

#define START_DELIM '['
#define END_DELIM ']'
#define SPLIT_DELIM '='

#define DBSERIAL Serial // debugging

#define PRINT1(a)         DBSERIAL.println(F(a))
#define PRINT2(a,b)       DBSERIAL.print  (F(a)); DBSERIAL.println(b)
#define PRINT3(a,b,c)     DBSERIAL.print  (F(a)); DBSERIAL.print  (b); DBSERIAL.println(F(c))
#define PRINT3NL(a,b,c)   DBSERIAL.print  (F(a)); DBSERIAL.print  (b); DBSERIAL.print(F(c))
#define PRINT4(a,b,c,d)   DBSERIAL.print  (F(a)); DBSERIAL.print  (b); DBSERIAL.print  (F(c)); DBSERIAL.println(d)
#define PRINT4NL(a,b,c,d) DBSERIAL.print  (F(a)); DBSERIAL.print  (b); DBSERIAL.print  (F(c)); DBSERIAL.print  (d)

#define SLOWPRINT1(a)         if (isSlowPrint){ PRINT1(a); }
#define SLOWPRINT2(a,b)       if (isSlowPrint){ PRINT2(a,b); }
#define SLOWPRINT3(a,b,c)     if (isSlowPrint){ PRINT3(a,b,c); }
#define SLOWPRINT4(a,b,c,d)   if (isSlowPrint){ PRINT4(a,b,c,d); }
#define SLOWPRINT4NL(a,b,c,d) if (isSlowPrint){ PRINT4NL(a,b,c,d); }

#define DBSERIAL2 Serial1 // other serial

#define PRINT21(a)         DBSERIAL2.println(F(a))
#define PRINT22(a,b)       DBSERIAL2.print  (F(a)); DBSERIAL2.println(b)
#define PRINT23(a,b,c)     DBSERIAL2.print  (F(a)); DBSERIAL2.print  (b); DBSERIAL2.println(F(c))
#define PRINT23NL(a,b,c)   DBSERIAL2.print  (F(a)); DBSERIAL2.print  (b); DBSERIAL2.print(F(c))
#define PRINT24(a,b,c,d)   DBSERIAL2.print  (F(a)); DBSERIAL2.print  (b); DBSERIAL2.print  (F(c)); DBSERIAL2.println(d)
#define PRINT24NL(a,b,c,d) DBSERIAL2.print  (F(a)); DBSERIAL2.print  (b); DBSERIAL2.print  (F(c)); DBSERIAL2.print  (d)

#define SLOWPRINT1(a)         if (isSlowPrint){ PRINT1(a); }
#define SLOWPRINT2(a,b)       if (isSlowPrint){ PRINT2(a,b); }
#define SLOWPRINT3(a,b,c)     if (isSlowPrint){ PRINT3(a,b,c); }
#define SLOWPRINT4(a,b,c,d)   if (isSlowPrint){ PRINT4(a,b,c,d); }
#define SLOWPRINT4NL(a,b,c,d) if (isSlowPrint){ PRINT4NL(a,b,c,d); }

bool isSlowPrint = true;

void resetSlowPrint() {
  static long lastTime = millis();
  if ((millis() - lastTime) > 1000) {
    lastTime = millis();
    isSlowPrint = true;
  } else {
    isSlowPrint = false;
  }
}

// =============== LED ================

#define PIN_LED PC13
#define LED_DURATION 200

void setupUtil() {
  pinMode(PIN_LED, OUTPUT);
  DBSERIAL.begin(9600); // debugging
  PRINT1("Debug Serial start");
  Wire.begin();

}

void loopUtil() {
  digitalWrite(PIN_LED, millis() % (LED_DURATION * 2) > LED_DURATION);
  // PRINT1("loop");
}

// =============== analogread to millivolt ================

static const long V_DIV = 11000; // 1M / 100K

// overflow 2,147,483,647
// TODO: use long instead of float
int analogReadToMillivolt(int n) {
  // performs calculation in float to prevent overflow
  float res = float(n);
  res = res * 5120 * V_DIV / 1024 / 1024;
  return int(res);
}


#endif
