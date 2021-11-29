// Wire pinswapping via Wire.pins()
// Copy of Wire Master Reader
// except for the Wire.pins() call.
// This is mostly here to make sure it gets tested by CI.


#include <Wire.h>

void setup() {
  Wire.swap(2);        // Choose pin swapping level 2 (PC2/PC3 master/slave)
  Wire.begin();        // join i2c bus (address optional for master)
  Serial.begin(9600);  // start serial for output
}

void loop() {
  Wire.requestFrom(8, 6);    // request 6 bytes from slave device #8

  while (Wire.available()) { // slave may send less than requested
    char c = Wire.read(); // receive a byte as character
    Serial.print(c);         // print the character
  }

  delay(500);
}
