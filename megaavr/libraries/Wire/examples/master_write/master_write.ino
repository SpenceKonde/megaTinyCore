/* Wire Master Write
 * by MX682X
 *
 * Demonstrates use of the New Wire library
 * Writes data to an I2C/TWI slave device
 * Refer to the "Wire Slave Read" example for use with this
 *
 * Enter any data using serial monitor or other console followed by either or
 * both of the line ending characters, and it will be sent to the slave, which
 * should print it out on it's serial port.
 *
 * To use this, you need to connect the SCL and SDA pins of this device to the
 * SCL and SDA pins of a second device running the Wire Slave Read example.
 *
 * Pullup resistors must be connected between both data lines and Vcc.
 * See the Wire library README.md for more information.
 */

#include <Wire.h>

char input[32];
int8_t len = 0;

#define MySerial Serial               // The serial port connected to the to the computer.

void setup() {
  Wire.begin();                       // initialize master
  // MySerial.swap(1);                 // Remember to swap serial pins if you need to do that with your connections.
  MySerial.begin(115200);             // Use 115200 baud - this is the 2020's, and these are modern AVRs.
}

void loop() {
  if (MySerial.available() > 0) {      // as soon as the first byte is received on Serial
    readFromSerial();                 // read the data from the Serial interface
    if (len > 0) {                    // after the while-loop, if there was useful data,
      sendDataWire();                 // send the data over I2C
    }
    len = 0;                          // since the data was sent, the position is 0 again
  }
}

void readFromSerial() {
  while (true) {                      // in an endless while-loop
    while (MySerial.available() == 0);// means we've taken all the bytes in, and are still waiting for a cr/lf.
    char c = MySerial.read();         // read the next char, now that there's one available.
    if (c == '\n' || c == '\r') {     // until a new line or carriage return is found
      break;                          // if so, break the endless while-loop
    }                                 // otherwise
    input[len] = c;                   // save the char
    len++;                            // increment the  position
    if (len > 30) {                   // if there was too much data
      break;                          // break the while-loop to avoid buffer overflow
    }
  }
}

void sendDataWire() {
  Wire.beginTransmission(0x54);     // prepare transmission to slave with address 0x54
  for (uint8_t i = 0; i < len; i++) {
    Wire.write(input[i]);           // Write the received data to the bus buffer
  }
  Wire.write("\r\n");               // add new line and carriage return for the Serial monitor
  Wire.endTransmission();           // finish transmission
}
