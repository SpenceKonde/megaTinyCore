/* Wire Multi Address Write
 * by Spence Konde based on work by MX682X
 *
 * Demonstrates use of the new Wire library for use with the secondary address
 * and masked address example sketches.
 *
 * Refer to the "Slave Secondary Address" or "Wire Address Mask"
 * examples for the slave sketch to use with this.
 *
 * This example takes the input from Serial. Input ends on CR or LF character
 * or at 30 characters in length.
 * The addressing depends on if you uncomment the USE_ANY_ADDRESS define.
 * Leave it commented out for the Secondary Address example:
 *   If the first element is a number, it will write the data on Wire to
 *     the slave address 0x54.
 *   Otherwise it writes the data on Wire to the slave address 0x64.
 * Uncomment it for the Address Mask slave example:
 *   If the first element is a 0-7, that will be the first digit (in hexadecimal)
 *     of the address, ex, '5test' would go to 0x54, and '2test' would go to 0x24
 *   Otherwise, it will send to address 0 (general call). According to the I2C Rev 7.0
 *     Specification, it requires a payload of at least 1 byte, the first being one of the
 *     two following codes:
 *     0x06 - Reset and use the new address programmed beforehand
 *     0x04 - just use the new address programmed beforehand.
 *     All other codes are usually ignored.
 *
 *
 * To use this, you need to connect the SCL and SDA pins of this device to the
 * SCL and SDA pins of a second device running the Slave Secondary Address example or
 * Slave Address Mask example.
 *
 * Pullup resistors must be connected between both data lines and Vcc.
 * See the Wire library README.md for more information.
 */
#include <Wire.h>

char input[32];
int8_t len = 0;

#define SEC_ADDRESS_ONLY
// Leave uncommented for the dual address slave
// Comment that out to use with multi-address sketch for best results

#define MySerial Serial             // The serial port connected to the to the computer.

void setup() {
  Wire.begin();                     // initialize master
  // MySerial.swap(1);               // Remember to swap serial pins if you need to do that with your connections.
  MySerial.begin(115200);           // Use 115200 baud - this is the 2020's, and these are modern AVRs.
}

void loop() {
  if (MySerial.available() > 0) {    // as soon as the first byte is received on Serial
    readFromSerial();               // read the data from the Serial interface
    if (len > 0) {                  // after the while-loop, if there was useful data,
      sendDataWire();               // send the data over I2C
    }
    len = 0;                        // since the data was sent, the position is 0 again
  }
}

void readFromSerial() {
  while (true) {                    // in an endless while-loop
    char c = MySerial.read();        // read the next char
    while (c == -1) {               // when the buffer is empty, Serial.read() returns -1
      c = MySerial.read();           // this avoids that
    }
    if (c == '\n' || c == '\r') {    // until a new line or carriage return is found
      break;                        // if so, break the endless while-loop
    }                               // otherwise
    input[len] = c;                 // save the char
    len++;                          // increment the  position
    if (len > 30) {                 // if there was too much data
      break;                        // break the while-loop to avoid buffer overflow
    }
  }
}

void sendDataWire() {
  uint8_t firstElement = input[0];
  uint8_t address = 0;
  #if defined(DUAL_ADDRESS_ONLY)
  if (firstElement >= '0' && firstElement <= '9') {   // check if the first element is a number
    address = 0x54;
  } else {
    address = 0x64;
  }
  #else
  if (firstElement >= '0' && firstElement <= '7') {   // check if the first element 0~7
    address = firstElement - '0';  // a char is an int8_t, a signed 8-bit value, so you can subtract them just fine.
    address <<= 4; // so we got a 0-7 from previous line, leftshift it 4 places to put it in the high bits.
    address |= 4;  // and we promised to use 0x_4 for the addresses, so add bitwise or that 4 into the low nybble.
  } else {
    address = 0;
  }
  #endif
  MySerial.print("Sending to: ");
  MySerial.printHex(address);
  MySerial.println();
  Wire.beginTransmission(address);
  Wire.write(input, len);

  #if defined(DUAL_ADDRESS_ONLY)
  Wire.write("\r\n");              // add new line and carriage return for the Serial monitor
  #endif
  Wire.endTransmission();          // finish transmission
}
