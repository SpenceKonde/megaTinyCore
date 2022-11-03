/* Wire Slave Address Mask
 * by Spence Konde based on work by MX682X
 *
 * Demonstrates use of the new Wire library
 * Receives data as an I2C/TWI slave device
 * Refer to the "Wire Master Multi-Address Write" example for a
 * master to use with this, though "Two Master Write" also works.
 *
 * This example prints the address which triggered the receive function
 * and the data that was sent to the slave on the Serial Monitor, in
 * both text and as hexadecimal (since things other than the test
 * master will rarely be sending ascii text over I2C).
 *
 * It will react to all general call messages, as well as the following
 * addresses: 0x04*, 0x14, 0x24, 0x34, 0x44, 0x54, 0x64, 0x74*.
 * * 0x04 and 0x74 are not valid I2C addresses
 *     0x04-0x07 are used for "high speed" mode (not the same as "fast"
 *       mode or "fast mode+").
 *     0x74 is an address "reserved for future use"
 *
 * Instead of printing from within the ISR, this sets a flag that
 * we check in the main loop (don't print to serial in an ISR. If you
 * have to for debugging, print the fewest characters possible)
 *
 * Since the whole body of the command is stored in the `input` array
 * we disable interrupts while we print it - this is to ensure that we
 * don't get partway though printing it and have it changed out from
 * under us. That isn't catastrophic here, but if we were doing something
 * else with the data it could be.
 *
 * This (more so than the secondary address one) is meant to be a more
 * comprehensive (if not comprehensible) starting point. It's also an
 * example of why I probably shouldn't be writing examples...
 *
 * To use this, you need to connect the SCL and SDA pins of this device to the
 * SCL and SDA pins of a second device running the Two Master Write or
 * Multi Address Write example.

 * To use this with the "Two Master Write" example, you need it connected to
 * both sets of pins.
 * e.g. if master is an AVR DA or DB (the only parts thus far that have two I2C interfaces):
 *    SDA connected to PA2 and PF2 of master and SDA of slave.
 *    SCL connected to PA3 and PF3 of master and SCL of slave.
 *
 * Pullup resistors must be connected between both data lines and Vcc.
 * See the Wire library README.md for more information.
 */
#include <Wire.h>

volatile uint8_t input[32];       // We store the data received here in the interrupt handler see receiveDataWire()
volatile uint8_t gotMessage = 0;  // and we put the address here. It's leftshifted one place, so we can use that
//                                   bit to indicate a message, even if it's a general call, see receiveDataWire()
volatile uint8_t lenMessage = 0;  // And the length gets stored here.
#define MySerial Serial           // The serial port connected to the to the computer.


void setup() {
  // Initializing slave to match general call and ignore the 3 high bits of the address
  // 1st argument: 1st address to listen to
  // 2nd argument: listen to general broadcast or "general call" (address 0x00)
  // 3rd argument: bits 7-1: second address if bit 0 is set true
  //               or bit mask of an address if bit 0 is set false
  //               or use WIRE_ALT_ADDRESS(7-bit address)
  //               or use WIRE_ADDRESS_MASK(7-bit address mask)
  // Those macros shift the address and put the appropriate LSBit in, but make the authors intent clearer.
  // 7-bit addresses and masks are between 0x01 and 0x7F. Higher values will produce a warning about implicit truncation
  // (generally happens because you shifted the value you passed to the above macros first - either pass the number manually shifted
  // or use the macro on the unshifted value.
  Wire.begin(0x54, true, WIRE_ADDRESS_MASK(0x78));
  // This will start TWI slave with an "address" of 0x54, but because it will ignore the 4 most significant bits,
  // only the 3 low bits of the address will be used, so it could just as well have been 0x74 or 0x04 or 0x4C. It will
  // match addresses 0bxxxx100 where x is a don't-care.
  Wire.onReceive(receiveDataWire);

  // Initialize serial port - if you need to swap pins, remember to do so.
  // MySerial.swap(1);
  MySerial.begin(115200);
}

void loop() {
  delay(100);
  if (gotMessage & 1) {
    printMessage();
  }
}

void printMessage() {
  uint8_t addr = gotMessage >> 1;
  cli(); // disable interrupts, we don't want another receive to write over these
  //        while we are reading them. This is the same as NoInterrupts();
  uint8_t len = lenMessage; // lenMessage is volatile, but it won't change
  // during one call to printMessage() since interrupts are disabled.
  // Saving to a temp variable like this saves 2-4 bytes of flash for every place
  // it is referenced, and 2-3 clocks every time it is accessed (those aren't quite
  // the same, because of loops). You probably don't care on a AVR128DB, but you
  // probably do on a ATtiny with 2-4k of flash.
  if (addr == 0) {
    MySerial.print("General Call");
    if (len > 1) {
      MySerial.print(" (malformed? Should only be a 1-byte command)");
    } else {
      MySerial.print(": ");
      MySerial.printHex(input[0]);
    }
  } else {
    MySerial.print("Addressed to ");
    MySerial.printHex(addr);
    if ((addr & 0x78) == 0 || addr > 0x6F) {
      MySerial.print(" (not a legal I2C address)");
    }
  }
  MySerial.println();
  if (len > 32) {
    MySerial.print("Oversized message showing 32/");
    MySerial.println(len);
    len = 32;
  }
  if (len > 1 || addr != 0) {
    MySerial.println();
    MySerial.print("Text: ");
    /* This is the danger point mentioned in the intro if data was changed:
     * Imagine if we expected text. If instead of looping over it the known
     * number of times for the length, we had put it into a char array and
     * tried to pad it with a null terminator and pass it to print(), if we
     * let an interrupt fire, a very ill-timed interrupt could result in
     * printing part of the first message, part of the second, and the contents
     * of every variable stored after them in the RAM until it hit one
     * containing zero.
     */
    for (uint8_t i = 0; i < len; i++) {
      MySerial.write(input[i]);
    }
    MySerial.println();
    MySerial.print("Hex: ");
    for (uint8_t i = 0; i < len; i++) {
      MySerial.printHex(input[i]);
    }
    MySerial.println();
  }
  gotMessage = 0;
  sei(); // enable interrupts again. This is the same as Interrupts();
}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveDataWire(int16_t numBytes) {
  uint8_t addr = Wire.getIncomingAddress();
  // get the address that triggered this function
  // the incoming address is leftshifted
  // We take advantage of this to store a flag in the low bit
  // indicating a message was received.
  // All the ISR should do is read the message, do anything that is
  // "timing critical" for your application, and record what we got.
  for (uint8_t i = 0; i < numBytes; i++) {
    if (i < 32) {               // make sure we don't overflow input
      input[i] = Wire.read();   // in the event of a strangely large
    }                           // payload.
  }
  gotMessage = addr | 1; // store address, with a 1 in the low bit.
  lenMessage = numBytes;
}
