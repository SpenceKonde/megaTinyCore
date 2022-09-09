/* Wire Master Write/Read
 * by MX682X
 *
 * Demonstrates use of the New Wire library
 * Writes or reads data to/from an I2C/TWI slave device
 * Refer to the "Wire slave_write_read" example for use with this
 *
 * Enter any data using serial monitor or other console followed by either or
 * both of the line ending characters. Depending on the first char, one of the
 * following happens:
 *    - if it's a 'm' or 'M', a master write is performed (if the #define is set)
 *    - otherwise a master read of 4 bytes is performed (if  the #define is set)
 *
 * To use this, you need to connect the SCL and SDA pins of this device to the
 * SCL and SDA pins of a second device running the Wire Slave Read example.
 *
 * Pullup resistors must be connected between both data lines and Vcc.
 * See the Wire library README.md for more information.
 */

#include <Wire.h>

#define MySerial Serial               // The serial port connected to the to the computer.

// The complete example might not fit on some parts. In that case, you can easily
// comment out some functionality. DECODE_ERROR bloats the code the most.
#define ENABLE_WRITE_TO     // Enables the master write functionality
#define ENABLE_READ_FROM    // Enables the master read functionality
#define DECODE_ERROR        // Prints status messages.

char input[32];
int8_t len = 0;


void setup() {
  Wire.begin();                       // initialize master
  MySerial.begin(115200);             // Use 115200 baud - this is the 2020's, and these are modern AVRs.
}

void loop() {
  if (MySerial.available() > 0) {      // as soon as the first byte is received on Serial
    readFromSerial();                 // read the data from the Serial interface
    if (len > 0) {                    // after the while-loop, if there was useful data,
      char c = input[0];
      if (c == 'm' || c == 'M') {     // If the first char is that
        sendDataWire();               // send the data over I2C to the slave
      } else {                        // otherwise
        requestDataWire();            // request data from I2C slave
      }
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
#ifdef ENABLE_WRITE_TO

  uint8_t err = 0;
  Wire.beginTransmission(0x54);     // prepare transmission to slave with address 0x54
  Wire.write(input, len);
  Wire.write("\r\n");               // add new line and carriage return for the Serial monitor
  err = Wire.endTransmission();     // finish transmission

#ifdef DECODE_ERROR

  switch (err) {
    case 0x00:  MySerial.println("Wire transmit was successful"); break;
    case 0x02:  MySerial.println("Address was NACK'd"); break;
    case 0x03:  MySerial.println("Data was NACK'd"); break;
    case 0x04:  MySerial.println("Unknown error occurred"); break;
    case 0x05:  MySerial.println("Transmission time-outed"); break;
    // The library also supports some extended errors that might give a hint on what is failing.
    case 0x10:  MySerial.println("Wire is uninitialized"); break;
    case 0x11:  MySerial.println("Pullups are missing"); break;
    case 0x12:  MySerial.println("Arbitration lost"); break;
  }

#endif /* DECODE_ERROR */
#endif /* ENABLE_WRITE_TO */
}

void requestDataWire() {
#ifdef ENABLE_READ_FROM
  uint8_t len = 4;
  uint32_t ms = 0;
  if (len == Wire.requestFrom(0x54, len, 0x01)) {    // request from slave
      ms  = (uint32_t)Wire.read();               // read out 32-bit wide data
      ms |= (uint32_t)Wire.read() <<  8;
      ms |= (uint32_t)Wire.read() << 16;
      ms |= (uint32_t)Wire.read() << 24;
      MySerial.println(ms);              // print the milliseconds from Slave
  } else {
#ifdef DECODE_ERROR
    MySerial.println("Wire.requestFrom() failed");
#endif
  }
#endif
}
