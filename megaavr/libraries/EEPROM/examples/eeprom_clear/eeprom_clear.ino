/*
 * EEPROM Clear
 *
 * Sets all of the bytes of the EEPROM to 0xFF (blank).
 * Please see eeprom_iteration for a more in depth
 * look at how to traverse the EEPROM.
 *
 * This example code is in the public domain.
 */

#include <EEPROM.h>

void setup() {
  // initialize the LED pin as an output - skip if LED_BUILTIN is PIN_PA3 and using external clock source. We test for this to ensure that the sketch will compile successfully and can be used for CI testing
  #if CLOCK_SOURCE != 2 || LED_BUILTIN != PIN_PA3
  pinMode(LED_BUILTIN, OUTPUT);
  #endif
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

  for (int i = 0 ; i < EEPROM.length() ; i++) {
    EEPROM.write(i, 0xFF);
  }

  // turn the LED on when we're done
  #if CLOCK_SOURCE != 2 || LED_BUILTIN != PIN_PA3
  digitalWrite(LED_BUILTIN, HIGH);
  #endif
}

void loop() {
  /* Empty loop.  */
}
