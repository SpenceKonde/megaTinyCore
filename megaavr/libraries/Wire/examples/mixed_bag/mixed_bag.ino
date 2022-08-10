/* Wire Mixed Bag Example
 * by MX682X
 *
 * Demonstrates use of the New Wire library
 * Writes data to an I2C/TWI slave and
 * Reads data from an I2C/TWI slave device
 *
 * With formatting and portability changes by Spence to support
 * disabled millis, different primary serial ports.
 *
 * This example takes the input from Serial. This code can run on both MCUs
 * If the first element is a 'r' or 'R', it will read the data on Wire
 * from the slave address 0x54, in this example the millis(), and print
 * it on the serial monitor.
 * If the first element is anything else, it writes the data on Wire to
 * the slave address 0x54. It is them copied in a buffer and sent back
 * to the sender, but with the address 0x64, to avoid an endless loop
 *
 * To use this, you need to connect the slave pins from one device to the
 * master pins of a second device.
 * E.g. for two  AVR DA or DB (the only parts thus far that have two I2C
 * interfaces):
 *
 *       SDA(M) PA2 ----\    /---- PA2 SDA(M)
 *       SCL(M) PA3 ----\\  //---- PA3 SCL(M)
 *                       \\//
 *                        \\
 *                       //\\
 *       SDA(S) PC2 ----//  \\---- PC2 SDA(S)
 *       SCL(S) PC3 ----/    \---- PC3 SCL(S)
 *
 * This sketch, in general, tries to exercise as many functions of the
 * Wire library as possible in as short an simple a system as possible.
 * and in so doing also acts as a test of Wire.
 */

#include <Wire.h>

char input[32];
char rxWire[32];
int8_t rxLen = 0;
int8_t len = 0;

#define MySerial Serial       // The serial port connected to the to the computer.


void setup() {
  // Wire.swapModule(&TWI1);    // Wire can work with TWI0 and TWI1, but the Wire has to be disabled
  // when calling it. Will return true if successful. Used to offer TWI1 pins
  // to Arduino libraries that support only one Wire
  // Wire.swap(2);              // Route TWI pins to an alternative pinout.

  Wire.enableDualMode(true);    // set argument true if FastMode+ speeds are expected on slave
  Wire.begin();                                   // initialize master
  Wire.begin(0x54, false, WIRE_ALT_ADDRESS(0x64));// initialize slave - equiv. ((64 << 1) | 1)
  Wire.onReceive(receiveDataWire);                // Set the slave receive ISR
  Wire.onRequest(transmitDataWire);               // Set the slave request ISR
  // MySerial.swap(1);           // Remember to swap serial pins if you need to do that with your connections.
  MySerial.begin(115200);       // Use 115200 baud - this is the 2020's, and these are modern AVRs.
}

void loop() {
  if (MySerial.available() > 0) {    // as soon as the first byte is received on Serial
    readFromSerial();               // read the data from the Serial interface
    if (len > 0) {                  // after the while-loop, if there was useful data,
      sendDataWire();               // send the data over I2C
    }
    len = 0;                        // since the data was sent, the position is 0 again
  }
  if (rxLen > 0) {
    sendDataBack();
    rxLen = 0;
  }
}

void readFromSerial() {
  #ifndef MILLIS_USE_TIMERNONE      // This example will compile and work without millis
  uint32_t timeout = millis();      // but if we have millis(), we grab the current time now
  #endif
  while (true) {
    char c = MySerial.read();       // read the next char
    while (c == -1) {               // when the buffer is empty, Serial.read() returns -1
      c = MySerial.read();          // this avoids filling the input buffer with gibberish
    }
    if (c == '\n' || c == '\r') {   // until a new line or carriage return is found
      break;                        // if so, break the endless while-loop
    }                               // otherwise
    input[len] = c;                 // save the char
    len++;                          // increment the position
    if (len > 30) {                 // if there was too much data
      break;                        // break the while-loop to avoid buffer overflow
    }
    #ifndef MILLIS_USE_TIMERNONE    // If we have millis haven't gotten newline
    if (millis() - timeout > 30000) {
      // we will give up after 30 seconds, and send what we have.
      MySerial.println("Serial timeout getting command");
      if (len > 0) {
        MySerial.println("Sending what we have");
      }
      break;
    }
    #endif
  }
}
void sendDataWire() {
  uint8_t firstElement = input[0];
  if (firstElement == 'r' || firstElement == 'R') {   // check if the first element is an 'r' or 'R'
    if (4 == Wire.requestFrom(0x54, 4, 0x01)) {       // request from slave
      while (Wire.available()) {
        uint32_t ms;
        ms  = (uint32_t)Wire.read();                  // read out 32-bit wide data
        ms |= (uint32_t)Wire.read() <<  8;
        ms |= (uint32_t)Wire.read() << 16;
        ms |= (uint32_t)Wire.read() << 24;
        MySerial.println(ms);
      }
    } else {
      MySerial.println("Wire.requestFrom() timed out!");
    }
  } else {
    Wire.setClock(250000);              // change TWI clock to 250kHz
    Wire.beginTransmission(0x54);       // prepare transmission to slave with address 0x54

    for (uint8_t i = 0; i < len; i++) {
      Wire.write(input[i]);             // Write the received data to the bus buffer
    }

    Wire.write("\r\n");                 // add new line and carriage return for the Serial monitor
    Wire.endTransmission();             // finish transmission
  }
}

void sendDataBack() {
  Wire.setClock(1000000);               // set clock to 1MHz
  Wire.beginTransmission(0x64);         // prepare transmission to slave with address 0x64

  for (uint8_t i = 0; i < rxLen; i++) {
    Wire.write(rxWire[i]);              // Write the received data to the bus buffer
  }
  Wire.endTransmission();               // finish transmission
}


void receiveDataWire(int16_t numBytes) {
  uint8_t i = 0;
  uint8_t addr = Wire.getIncomingAddress();   // get the address that triggered this function
  //                                             the incoming address is left-shifted by 1
  if (addr == (0x54 << 1)) {                  // if the address was 0x54, do this
    for (; i < numBytes; i++) {
      rxWire[i] = Wire.read();
    }
    rxLen = i;
  } else if (addr == (0x64 << 1)) {           // if the address was 0x64, do that
    /* Warning: the onReceive function is called within an ISR!
     * Writing to serial from an ISR is not advised, as the impacts on the
     * rest of the sketch if the buffer ends up full can be surprising and bad.
     * Thus, this is not a design pattern to emulate in your code.
     */
    MySerial.print("Addr 0x64: ");
    for (; i < numBytes; i++) {
      char c = Wire.read();
      MySerial.write(c);
    }
  }
}

void transmitDataWire() {
  #ifdef MILLIS_USE_TIMERNONE
  uint32_t ms = 1234567890UL; /* We have no milliseconds value to send, so use a super-distinctive placeholder */
  #else
  uint32_t ms = millis();
  #endif
  Wire.write((uint8_t) ms);
  Wire.write((uint8_t)(ms >> 8));
  Wire.write((uint8_t)(ms >> 16));
  Wire.write((uint8_t)(ms >> 24));
}
