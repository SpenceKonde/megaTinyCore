/* Wire Master and Slave example
 * by MX682X
 *
 * Demonstrates use of the New Wire library
 * This example requires the setting Master And Slave
 * If anything is sent through the Serial Monitor and terminated with NL and/or CR
 * The current millis value is sent to the slave. If enableDualMode is NOT called,
 * the slave pins are the same as the master pins. This creates a loopback mode,
 * in which the sent millis value is received in the same device and is printed back to
 * the Serial Monitor.
 *
 */

#include <Wire.h>

int8_t rxLen = 0;
int8_t len = 0;

#define MySerial Serial       // The serial port connected to the to the computer.


void setup() {
  Wire.onReceive(receiveDataWire);  // Set the slave receive ISR
  //Wire.enableDualMode(false)      // used to use separate pins for master and slave. If enabled,
  //                                   it would disable the loopback feature this example relies upon
  //Wire.swap()                     // used to select the pins of the master and the slave

  Wire.begin();                     // Enables the master functionality
  Wire.begin(0x54);                 // Enables the slave functionality

  MySerial.begin(115200);           // Use 115200 baud - this is the 2020's, and these are modern AVRs.
}

void loop() {
  if (MySerial.available() > 0) {   // as soon as the first byte is received on Serial
    readFromSerial();               // read the data from the Serial interface
    if (len > 0) {                  // after the while-loop, if there was useful data,
      sendDataWire();               // send the data over I2C
    }
    len = 0;                        // since the data was sent, the position is 0 again
  }
}

void readFromSerial() {
  while (true) {                      // in an endless while-loop
    while (MySerial.available() == 0);// means we've taken all the bytes in, and are still waiting for a cr/lf.
    char c = MySerial.read();         // read the next char, now that there's one available.
    if (c == '\n' || c == '\r') {     // until a new line or carriage return is found
      break;                          // if so, break the endless while-loop
    }                                 // otherwise
    len++;                            // increment the  position. data is not saved as it is not needed in this case
  }
}
void sendDataWire() {
  uint32_t ms = 0x22334455;
  #ifndef MILLIS_USE_TIMERNONE
  ms = millis();    // overwrite the default value
  #endif

  Wire.beginTransmission(0x54);     // prepare transmission to slave with address 0x54
  Wire.write((uint8_t) ms);
  Wire.write((uint8_t)(ms >> 8));
  Wire.write((uint8_t)(ms >> 16));
  Wire.write((uint8_t)(ms >> 24));

  Wire.endTransmission();     // finish transmission
}


void receiveDataWire(int16_t numBytes) {
  // Warning: This is terrible practice! Never print from within an ISR!
  // It only works in very simple cases.
  if (numBytes != 4) {
    MySerial.print("Slave received only ");
    MySerial.println(numBytes);
  }
  MySerial.print("Slave Received: ");
  uint32_t temp = 0;
  temp |= (uint32_t)Wire.read();
  temp |= (uint32_t)Wire.read() << 8;
  temp |= (uint32_t)Wire.read() << 16;
  temp |= (uint32_t)Wire.read() << 24;
  MySerial.println(temp);
}
