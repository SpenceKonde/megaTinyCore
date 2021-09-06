#include "Wire.h"

void setup() {
   Wire.begin();

}

void loop() {
   Wire.beginTransmission(0x10);
   Wire.write("Hello", 5);
   Wire.endTransmission(true);
}
