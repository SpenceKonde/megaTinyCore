/* Wire Register Model Master
 * by Spence Konde
 *
 * Demonstrates a master interacting with a wire slave implementing the register model
 * We knpw from the documented interface for the slave what write protection on
 * the bytes is as follows:
 *
 * 0-4 are fully writable
 * the 5 low bits of 5 are writable
 * 6 and 7 are fully writable,
 * 8-11 only allow the 2 low bits in each nybble to be written
 * 12-15 are read-only
 * 16 and 17 allow only the low nybble to be written
 * 18 and 19 allow only the high nybble to be written.
 *
 * And the default state is for them to hodl values equal to the address, until
 * told otherwise.
 *
 * The address pointer autoincrements and wraps around.
 *
 * Registers 4 and 5 control the blink period of the LED, as an example of
 * how a slave might be configured over I2C. In a real device, all the
 * registers would be either controlled by or
 */
#include <Wire.h>

#define MySerial Serial

void setup() {
  MySerial.begin(115200);
  MySerial.println("Hi, now to use a register model slave");
  Wire.begin();
}

void setAddressPointer(uint8_t address) {
  Wire.beginTransmission(0x69);    // prepare transmission to slave with address 0x69
  Wire.write(address);            // Write just the address
  Wire.endTransmission();
}

void loop() {
  MySerial.println("going to write to 0, then make a series of reads");
  setAddressPointer(0);
  Wire.requestFrom(0x69, 8);
  while (Wire.available()) {
    MySerial.printHex((uint8_t)Wire.read());
    MySerial.print(' ');
  }
  MySerial.println("that was 8 bytes");
  Wire.requestFrom(0x69, 12);
  while (Wire.available()) {
    MySerial.printHex((uint8_t)Wire.read());
    MySerial.print(' ');
  }
  MySerial.println("that was 12 bytes more");
  MySerial.println("Now, let's request a whopping 32, the whole shebang");

  Wire.requestFrom(0x69, 32);
  while (Wire.available()) {
    MySerial.printHex((uint8_t) Wire.read());
    MySerial.print(' ');
  }
  MySerial.println("Now that was cool, no?");
  MySerial.println("Let's demo write protect in action");
  Wire.beginTransmission(0x69);     // prepare transmission to slave with address 0x69
  Wire.write(0x16);                 // Write just the address
  Wire.write(0xEE);                 // Write a value
  Wire.write(0xDD);                 // Write a value
  Wire.write(0xCC);                 // Write a value
  Wire.write(0xBB);                 // Write a value
  Wire.write(0xFF);                 // Write a value
  Wire.endTransmission();           // Send, slave ISR will fire.
  MySerial.println("Read-em-back:");// now read them back by resetting the pointer
  setAddressPointer(16);            // Set pointer to 16
  Wire.requestFrom(0x69, 5);         // Read 5 bytes.
  while (Wire.available()) {        // Hopefully we got 5 bytes. without this library versions new features, slave doesn't know how many were read!
    MySerial.printHex((uint8_t)Wire.read());   // Print it out for user.
    MySerial.print(' ');                       // spaces between each byte
  }
  MySerial.println("Change speed at which the LED blinks");
  Wire.beginTransmission(0x69);   // prepare transmission to slave with address 0x69
  Wire.write(0x4);                // Write just the address
  Wire.write(0x80);               // Write a value
  Wire.write(0x01);               // Write a value: ~3/8ths of a second!
  Wire.endTransmission();
  delay(10000);
}
