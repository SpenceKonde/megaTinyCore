/*
   USERSIG Clear

   Sets all of the bytes of the USERSIG to 0xFF (blank).
   Please see eeprom_iteration for a more in depth
   look at how to traverse the USERSIG.

   This example code is in the public domain.
*/

#include <USERSIG.h>

void setup() {
  // initialize the LED pin as an output - skip if LED_BUILTIN is PIN_PA3 and using external clock source. We test for this to ensure that the sketch will compile successfully and can be used for CI testing
  #if CLOCK_SOURCE != 2 || LED_BUILTIN != PIN_PA3
  pinMode(LED_BUILTIN, OUTPUT);
  #endif

  /***
     Advance to the next address, when at the end restart at the beginning.

     All (post 2016) parts have 32 bytes of USERSIG/USERROW space.

     Rather than hard-coding the length, you should use the pre-provided length function.
     This will make your code portable to future AVR processors which may have more.
   ***/

  for (int i = 0 ; i < USERSIG.length() ; i++) {
    USERSIG.write(i, 0xFF);
  }

  // turn the LED on when we're done
  #if CLOCK_SOURCE != 2 || LED_BUILTIN != PIN_PA3
  digitalWrite(LED_BUILTIN, HIGH);
  #endif
}

void loop() {
  /** Empty loop. **/
}
