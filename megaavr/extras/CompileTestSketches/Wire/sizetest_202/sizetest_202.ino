#include "Wire.h"

//A small sketch to test the minimal size requirements of the Wire library

void setup() {
   Wire.begin();
}

void loop() {
   Wire.beginTransmission(0x10);
   Wire.write("Hello", 5);
   Wire.endTransmission(true);
}
