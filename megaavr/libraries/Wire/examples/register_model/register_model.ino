/* Wire Register Model
 * by Spence Konde
 *
 * Demonstrates use of the Wire library
 * Implements an example of the "register machine" type of interface provided by most I2C
 * devices, where there are "registers" much like the ones that control on-chip peripherals
 * with the "autoincrement" functionality it is typically paired with.
 * Just like on those, you write a register by writing more than one byte, with the first
 * byte being the first address to write, and the second the value to write there, and if
 * more bytes are written, those are written to successive addresses after that.
 * For a read, you write a single byte, the address of the register you want to start reading
 * from, which sets the "pointer", and then read as many bytes as you want.
 * As with the registers of a microcontroller, some are read-only, containing outputs.
 *
 * Here we allocate an array of bytes, DeviceRegisters[32]
 * it is, very creatively, filled with the numbers 0 through 32.
 * We also define a variable WriteMask[32] which contains a series of constants, one for each "register"
 * only bits that are a 1 can be written.
 * The others will remain unchanged.
 * 0-4 are fully writeable
 * the 5 low bits of 5 are writeable
 * 6 and 7 are fully writeable,
 * 8-11 only allow the 2 low bits in each nybble to be written
 * 12-15 are read-only
 * 16 and 17 allow only the low nybble to be written
 * 18 and 19 allow only the high nybble to be written.
 *
 * The address pointer autoincrements and wraps around.
 *
 * There are two special registers, 4 and 5.
 * (chosen mostly at random, just to demonstrate making things in loop depend on registers)
 * 4 and 5 are the low and high bytes of the time delay used for the blinking... So by default it's 0x0504 ms
 * or 1284 ms.
 */
#include <Wire.h>
volatile uint8_t DeviceRegisters[32] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                                        0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
                                        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                                        0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F
                                       };
const uint8_t WriteMask[32]          = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x1F, 0xFF, 0xFF,
                                        0x33, 0x33, 0x33, 0x33, 0x00, 0x00, 0x00, 0x00,
                                        0x0F, 0x0F, 0xF0, 0xF0, 0x00, 0x00, 0x00, 0x00,
                                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
                                       };
volatile uint8_t WirePointer = 0;

void setup() {
  Wire.begin(0x69); // let's start off with a nice transgressive number, always a solid start right?
  Wire.onReceive(receiveHandler);
  Wire.onRequest(requestHandler);
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(230400);

}

void loop() {
  static uint32_t lastBlinkAt = 0;
  uint16_t delaytime = DeviceRegisters[4] + ((uint16_t)DeviceRegisters[5] << 8);
  if (millis() - lastBlinkAt > delaytime) {
    lastBlinkAt = millis();
    digitalWrite(LED_BUILTIN, CHANGE);
  }
}
/* Now - the receive handler is pretty straightforward. Since we will account for the bytes read in the request handler, the receive
 * handler calls it, and doesn't keep the returned value. This resets that count of bytes.
 * the line that sets the byte in the array if a write of more than just the address pointer is performed is ugly, but it's a bog
 * standard application of masks. The bits that are 1 in the mask are set to the new value, so AND the new value with that.
 * The bits that are 0 in the mask remain the same, so invert the mask, and AND that with the old value. OR the two together and set.
 *
 * The bitwise AND of the pointer with 0x1F is to make it wrap around. It's the same as the Modulo (%) operator in that way, except
 * it only works for powers of 2 and is orders of magnitude more efficient - modulo means division, and division is slow and bulky,
 * implemented in software (specifically a routine written in assembly as part of libgcc); point is modulo = division = bad. Never do
 * division on an AVR unless you have to. I have put considerable effort into keeping it out of the micros() calculations.
 */
void receiveHandler(int numbytes) {
  Wire.getBytesRead(); // reset count of bytes read. We don't do anything with it here, but a write is going to reset it to a new value.
  WirePointer = Wire.read() & 0x1F; // make sure they can't write off the end of the array!
  numbytes--; // we just read a byte, so we should decrement this.
  while (numbytes > 0) { // If numbytes was more than 1, we'll execute the below loop to write to the "registers".

    uint8_t unchangedbits = (DeviceRegisters[WirePointer] & ~WriteMask[WirePointer]);
    DeviceRegisters[WirePointer] = (Wire.read() & WriteMask[WirePointer]) | unchangedbits;
    WirePointer++;          // increment the pointer.
    WirePointer &= 0x1F;    // Wrap around if it's gone over 32;
    numbytes--;             // decrement remaining bytes.
  }
}
/* This one is weirder. The way the onRequest behaves and what it needs to do is very counter-intuitive (and this model is probably part
 * of WHY all Arduino I2C slave devices use wire like "serial with a clock" instead of like a civilised device....)
 *
 * the handler registered with onRequest is called when the slave gets a packet that matches it's address, set to read; This is called
 * once, and prints out all of the data that the master *might* request. It then does not fire again until the next start condition
 * followed by a matching address. There is a chance that the master might NACK a transmission before it was completed. The Arduino API does
 * not allow to track how many bytes were actually written by the Slave. This is fixed with getBytesRead().
 *
 * Without the getBytesRead() extension, there is no way for a slave written through the Arduino API to react to whether the master has
 * read something something - and that's a very common behaviour in commercial I2C devices.
 *
 * Another thing to consider is that, if the there is a buffer underflow, the TWI will keep the SDA lines released, making the master think
 * a 0xFF was transmitted.
 */
void requestHandler() {
  // We will start reading from the pointer.
  // But if there was a previous read, and the master then started a
  // second read, we want the pointer to pick up where they left off.
  uint8_t bytes_read = Wire.getBytesRead();
  WirePointer       += bytes_read;
  WirePointer       &= 0x1F;
  // You could also do :
  // WirePointer = (WirePointer + Wire.getBytesRead()) & 0x1F;
  // It is no more or less efficient.

  // Making something to react would be done above between getting bytes read
  // and adjusting the pointer. If you were implementing a sensor of some sort
  // you might have the result register or the status register do that.
  // The slave might even check during it's loop function to see if
  // Wire.slaveTransactionOpen() ceased to be true, then check getBytesRead() to
  // see if the key register was read __HOWEVER__
  // While this is executing, you must bear in mind that the slave is "clock stretching"
  // Hence avoiding an unduly long execution time should be one of your priorities.
  // Not only is the slave device wasting time, it's wasting the masters time, and that
  // of anything else waiting to use the bus. So like, if you're a sensor manager, you
  // should be storing the values in the "register" array as you take them; you shouldn't go
  // and take a dozen readings during this handler. Dont forget that this is also an ISR
  // with all that that entails

  for (byte i = 0; i < 32; i++) {
    Wire.write(DeviceRegisters[(WirePointer + i) & 0x1F]);
    // "write" the whole array - but the master might only want one byte.
    // The slave doesn't know how much data the master will want yet.
    // and won't know until it's gotten all that it wants and has generated a stop condition.
  }

}
