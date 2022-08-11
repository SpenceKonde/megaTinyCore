/* EEPROM Write
 *
 * Stores values read from analog input 0 into the EEPROM.
 * These values will stay in the EEPROM when the board is
 * turned off and may be retrieved later by another sketch.
 */

#include <EEPROM.h>

/* the current address in the EEPROM (i.e. which byte we're going to write to next) */
int addr = 0;

void setup() {
  // initialize the LED pin as an output - skip if LED_BUILTIN is defined as PIN_PA3 and using external clock source (an invalid configuration in practice!). We test for this to ensure that the sketch will compile successfully and can be used for CI testing
  #if CLOCK_SOURCE != 2 ||  LED_BUILTIN != PIN_PA3
  pinMode(LED_BUILTIN, OUTPUT);
  #endif
}

void loop() {
  /* Need to divide by 4 because analog inputs range from
   * 0 to 1023 and each byte of the EEPROM can only hold a
   * value from 0 to 255.
   */

  int val = analogRead(A7) / 4; // Use A7 for example because all supported parts have it: tinyAVR, Dx, and Ex.

  /* Write the value to the appropriate byte of the EEPROM.
    these values will remain there when the board is
    turned off.
   */

  EEPROM.write(addr, val);

  /* Iterate through each byte of the EEPROM storage.
   *
   * Larger AVR processors have larger EEPROM sizes, E.g:
   * tinyAVR 0/1/2-series 2k flash:      64b
   * tinyAVR 0/1/2-series 4-8k flash:    128b
   * tinyAVR 0/1/2-series 16-32k flash:  256b
   * megaAVR 0-series:                   256b (all flash sizes)
   * DA, DB, EA-series:                  512b (all flash sizes)
   * DD-series:                          256b (all flash sizes)
   *
   * Rather than hard-coding the length, you should use the pre-provided length function.
   * This will make your code portable to all AVR processors.
   */

  addr = addr + 1;
  if (addr == EEPROM.length()) { // Okay, we've written gibberish over the entire EEPROM
    while (1); // Wait forever - no need to sit there wasting rewrite longevity.
  }

  /* As the EEPROM sizes are powers of two, wrapping (preventing overflow) of an
   * EEPROM address is also doable by a bitwise and of the length - 1.
   *
   * ++addr &= EEPROM.length() - 1;
   */
  #if CLOCK_SOURCE != 2 ||  LED_BUILTIN != PIN_PA3
  digitalWrite(LED_BUILTIN, HIGH); // briefly flash LED as activity indication.
  #endif
  delay(2000);
}
