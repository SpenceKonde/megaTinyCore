/* Wire Slave Secondary Address
 * by MX682X
 *
 * Demonstrates use of the New Wire library
 * Receives data as an I2C/TWI slave device
 * Refer to the "Wire Two Masters Write" example (DA/DB 32+ pin)
 * or "Wire Master Multi Address Write" example (all other parts)
 *
 * This example prints the address which triggered the receive function
 * and the data that was sent to the slave on the Serial Monitor
 *
 * Note that, for brevity and simplicity, this example writes to serial
 * in the ISR. You should NEVER DO THAT in real life, as the impact when
 * the serial buffer fills up is unpredictable, but universally very bad.
 * Set a flag that you check for in loop and print it then.
 *
 * See the "Slave Address Mask" example for an example that handles this
 * more "properly" (and also is a bit more complex generally).
 *
 * To use this with the "Wire Two Master Write" example, you need
 * it connected to both sets of host pins.
 * e.g. for two AVR128DA48:
 *    SDA connected to PA2 and PF2 of master and PA2 of slave.
 *    SCL connected to PA3 and PF3 of master and PA2 of slave.
 *
 * The "Wire Master Multi-Address Write" only uses a single pair of I2C pins.
 *
 * Pullup resistors must be connected between both data lines and Vcc.
 * See the Wire library README.md for more information.
 */
#include <Wire.h>

char input[32];
int8_t len = 0;

#define MySerial Serial       // The serial port connected to the to the computer.


void setup() {
  // Initializing slave with secondary address
  // 1st argument: 1st address to listen to
  // 2nd argument: listen to general broadcast or "general call" (address 0x00)
  // 3rd argument: bits 7-1: second address if bit 0 is set true
  //               or bit mask of an address if bit 0 is set false
  Wire.begin(0x54, false, WIRE_ALT_ADDRESS(0x64));
  Wire.onReceive(receiveDataWire);

  // Initialize serial port - if you need to swap pins, remember to do so.
  // MySerial.swap(1);
  MySerial.begin(115200);       // Uses 115200 baud - this is the 2020's, and these are modern AVRs.
}

void loop() {
  delay(100);
}



// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveDataWire(int16_t numBytes) {
  uint8_t addr = Wire.getIncomingAddress();   // get the address that triggered this function
  //                                          // the incoming address is leftshifted though
  if (addr == (0x54 << 1)) {                  // if the address was 0x54, do this
    MySerial.print("Addr 0x54: ");
    for (uint8_t i = 0; i < numBytes; i++) {
      char c = Wire.read();
      MySerial.write(c);
    }
  } else if (addr == (0x64 << 1)) {           // if the address was 0x64, do that
    MySerial.print("Addr 0x64: ");
    for (uint8_t i = 0; i < numBytes; i++) {
      char c = Wire.read();
      MySerial.write(c);
    }
  }
}
