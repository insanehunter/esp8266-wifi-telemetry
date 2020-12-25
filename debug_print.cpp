#include <Arduino.h>
#include "debug_print.h"

void debug_print(const Printable &msg) {
  Serial.print(msg);
}

void debug_print(const String &msg) {
  Serial.print(msg);
}

void debug_print(const char *msg) {
  Serial.print(msg);
}
