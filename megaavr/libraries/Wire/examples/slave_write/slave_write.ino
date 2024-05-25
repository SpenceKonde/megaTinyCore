/* Wire Slave Write
 *
 * This sketch responds to a request for data directed at address 0x54
 * with up to 4 bytes of data - namely, the current value of millis.
 *
 * If millis is disabled, it will instead always return the sequence
 * 1234567890 (1,234,567,890, or 0x499602D2 represented in hexadecimal)
 *
 * To use this, you need to connect the SCL and SDA pins of this device to the
 * SCL and SDA pins of a second device running the Master Read example.
 *
 * Pullup resistors must be connected between both data lines and Vcc.
 * See the Wire library README.md for more information.
 */

#include <Wire.h>

void setup() {
  Wire.begin(0x54);                 // initialize slave
  Wire.onRequest(transmitDataWire); // register transmitDataWire as the handler
}

void loop() {

}

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
