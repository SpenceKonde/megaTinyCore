/* Wire Slave Write/Read
 * by MX682X
 *
 * Demonstrates use of the Wire library
 * Writes/Reads data as an I2C/TWI slave device
 * Refer to the "Wire master_write_read" example for use with this
 *
 * If the #define ENABLE_RECEIVE is set, every byte in a Master Write is
 * printed to the Serial Monitor.
 * 
 * If the #define ENABLE_REQUEST is set, the slave sends its millis()
 * (or placeholder) value to the master.
 *
 * To use this, you need to connect the SCL and SDA pins of this device to the
 * SCL and SDA pins of a second device running the Master Write example.
 *
 * Pullup resistors must be connected between both data lines and Vcc.
 * See the Wire library README.md for more information.
 */

#include <Wire.h>

#define MySerial Serial

#define ENABLE_RECEIVE
#define ENABLE_REQUEST


void setup() {
#ifdef ENABLE_RECEIVE
  Wire.onReceive(receiveDataWire);  // give the Wire library the name of the function
  //                                   that will be called on a master write event
#endif
#ifdef ENABLE_REQUEST
  Wire.onRequest(transmitDataWire); // same as above, but master read event
#endif

  Wire.begin(0x54);                 // join i2c bus with address 0x54
  MySerial.begin(115200);
}

void loop() {
  delay(100);
}

#ifdef ENABLE_RECEIVE
void receiveDataWire(int16_t numBytes) {      // the Wire API tells us how many bytes
  for (uint8_t i = 0; i < numBytes; i++) {    // were received so we can for loop for that
    char c = Wire.read();                     // amount and read the received data
    MySerial.write(c);                         // to print it to the Serial Monitor
  }
}
#endif

#ifdef ENABLE_REQUEST
void transmitDataWire() {
  #if !defined(MILLIS_USE_TIMERNONE)
  uint32_t ms = millis();
  #else
  uint32_t ms = 123456789UL; // placeholder - there's no millis to send because it's disabled.
  #endif
  Wire.write((uint8_t) ms);
  Wire.write((uint8_t)(ms >> 8));
  Wire.write((uint8_t)(ms >> 16));
  Wire.write((uint8_t)(ms >> 24));
}
#endif
