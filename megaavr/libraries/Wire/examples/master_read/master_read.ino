/* Wire Master Read
 * by MX682X
 *
 * Demonstrates use of the New Wire library
 * Reads data from an I2C/TWI slave device
 * Refer to the "Wire Slave Write" example for use with this
 *
 * This example takes the input from Serial. If the serial input is 'm' or 'M',
 * this code requests 4 bytes from the slave with the address 0x54.
 * When using together with the complementary example, the slave sends it's millis() value.
 * This value is then sent to the serial monitor
 *
 * To use this, you need to connect the SCL and SDA pins of this device to the
 * SCL and SDA pins of a second device running the Wire Slave Write example.
 *
 * Pullup resistors must be connected between both data lines and Vcc.
 * See the Wire library README.md for more information.
 */

#define MySerial Serial

#include <Wire.h>

int8_t rxLen = 0;
int8_t len = 0;

void setup() {
  Wire.begin();                                 // initialize master
  MySerial.begin(115200);
}

void loop() {
  if (MySerial.available() > 0) {    // as soon as the first byte is received on Serial
    char c = MySerial.read();       // read the data from serial.
    if (c == 'm' || c == 'M') {
      sendDataWire();               // send the data over I2C
    }
    len = 0;                        // since the data was sent, the position is 0 again
  }
}

void sendDataWire() {
  uint32_t ms;
  if (4 == Wire.requestFrom(0x54, 4, 0x01)) {    // request from slave
    while (Wire.available()) {
      ms  = (uint32_t)Wire.read();               // read out 32-bit wide data
      ms |= (uint32_t)Wire.read() <<  8;
      ms |= (uint32_t)Wire.read() << 16;
      ms |= (uint32_t)Wire.read() << 24;
      MySerial.println(ms);              // print the milliseconds from Slave
    }
  } else {
    MySerial.println("Wire.requestFrom() timed out!");
  }
}
