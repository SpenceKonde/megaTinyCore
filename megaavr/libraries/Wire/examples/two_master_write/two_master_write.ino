/* Wire Two Masters Write
 * by MX682X
 *
 * Demonstrates use of the new Wire library using a device with two
 * TWI ports to simulate two master devices without needing a third AVR.
 * Writes data to an I2C/TWI slave device.
 *
 * Refer to the "Wire Slave Secondary Address" or "Wire Slave Address Mask"
 * examples for the slave sketch to use with this.
 *
 * This example takes the input from Serial. Input ends on CR or LF character
 *
 * To use this example both interfaces of the master have to be on an I2C bus
 * with a device running one of the above slave sketches. Since this is a demo
 * it was written with the intent of using a single I2C bus, connected to both
 * sets of pins and to the slave device.
 *
 * Of course, in real life you wouldn't bother using both TWI interfaces in
 * master mode like that. However, while I2C is fairly good at mixed logic levels
 * (see the Wire readme), this is sometimes an issue; on an AVR DB or DD with MVIO,
 * one interface could talk to the fragile 1.8v ultra-modern sensor, and the other to
 * a 5v device that doesn't recognize 1.8v as a HIGH. Or one bus could run at FM+ speeds
 * and the other at standard speed, each with devices appropriate for that speed.
 *
 * The ability to have two separate I2C buses in use simultaneously also allows you
 * to have two devices with the same address to be usedwithout conflict. Many devices
 * for example include no address select pin, so if you wanted to use two at once this
 * would allow you to do that. Even without multiple identical parts on the bus, with
 * only 128 addresses, you'd expect to occasionally encounter devices that both want
 * the same address.
 *
 * e.g. for two AVR DA or DB-series parts (32+ pin only, 28 pin ones don't have the
 * second TWI interface)
 *    SDA connected to PA2 and PF2 of master and PA2 of slave.
 *    SCL connected to PA3 and PF3 of master and PA3 of slave.
 *
 * Pullup resistors must be connected between both data lines and Vcc.
 * See the Wire library README.md for more information.
 */
#include <Wire.h>

char input[32];
int8_t len = 0;

#define MySerial Serial             // The serial port connected to the to the computer.
#if !defined(TWI1) || !defined(TWI_USING_WIRE1)
  #error "This sketch requires that the board have a second TWI interface, and be set to enable Wire1 from the tools submenu"
#endif

void setup() {
  Wire.begin();                       // initialize master
  Wire1.begin();                      // initialize the other master
  MySerial.swap(1);                   // Remember to swap serial pins if you need to do that with your connections.
  MySerial.begin(115200);             // Use 115200 baud - this is the 2020's, and these are modern AVRs.
}

void loop() {
  if (MySerial.available() > 0) {     // as soon as the first byte is received on Serial
    readFromSerial();                 // read the data from the Serial interface
    if (len > 0) {                    // after the while-loop, if there was useful data,
      sendDataWire();                 // send the data over I2C
    }
    len = 0;                          // since the data was sent, the position is 0 again
  }
}

void readFromSerial() {
  while (true) {                      // in an endless while-loop until we get a line ending
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
  uint8_t firstElement = input[0];
  if (firstElement >= '0' && firstElement <= '9') {   // check if the first element is a number
    Wire.beginTransmission(0x54);    // prepare transmission to slave with address 0x54

    for (uint8_t i = 0; i < len; i++) {
      Wire.write(input[i]);          // Write the received data to the bus buffer
    }

    Wire.write("\r\n");              // add new line and carriage return for the Serial monitor
    Wire.endTransmission();          // finish transmission
  } else {
    Wire1.beginTransmission(0x64);   // prepare transmission to slave with address 0x64

    for (uint8_t i = 0; i < len; i++) {
      Wire1.write(input[i]);          // Write the received data to the bus buffer
    }

    Wire1.write("\r\n");              // add new line and carriage return for the Serial monitor
    Wire1.endTransmission();          // finish transmission
  }
}
