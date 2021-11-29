// Wire pinswapping via Wire.pins()
// Copy of Wire Master Reader
// except for the Wire.pins() call.
// This is mostly here to make sure it gets tested by CI.


#include <Wire.h>

void setup() {
  Wire.pins(GPIOR0, GPIOR1); // Choose the PC2/PC3 pinset for the TWI interface
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
