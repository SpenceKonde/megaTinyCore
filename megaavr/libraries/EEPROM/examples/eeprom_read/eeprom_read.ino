/* EEPROM Read
 *
 * Reads the value of each byte of the EEPROM and prints it
 * to the computer.
 * This example code is in the public domain.
 */

#include <EEPROM.h>

// start reading from the first byte (address 0) of the EEPROM
int address = 0;
byte value;

void setup() {
  Serial.begin(115200);
}

void loop() {
  // read a byte from the current address of the EEPROM
  value = EEPROM.read(address);

  Serial.print(address);
  Serial.print("\t");
  Serial.print(value, DEC);
  Serial.println();

  /*
   * Iterate through each byte of the EEPROM storage.

   * Larger AVR processors have larger EEPROM sizes, E.g:
   * tinyAVR 0/1/2-series 2k flash:      64b
   * tinyAVR 0/1/2-series 4-8k flash:    128b
   * tinyAVR 0/1/2-series 16-32k flash:  256b
   * megaAVR 0-series:                   256b (all flash sizes)
   * DA, DB, EA-series:                  512b (all flash sizes)
   * DD-series:                          256b (all flash sizes)

   * Rather than hard-coding the length, you should use the pre-provided length function.
   * This will make your code portable to all AVR processors.
   */

  address = address + 1;
  if (address == EEPROM.length()) {
    address = 0;
  }

  /*
   * As the EEPROM sizes are powers of two, wrapping (preventing overflow) of an
   * EEPROM address is also doable by a bitwise and of the length - 1.

   * ++address &= EEPROM.length() - 1;
   */

  delay(500);
}
