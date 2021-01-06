/*
   USERSIG Clear

   Sets all of the bytes of the USERSIG to 0xFF (blank).
   Please see eeprom_iteration for a more in depth
   look at how to traverse the USERSIG.

   This example code is in the public domain.
*/

#include <USERSIG.h>

void setup() {
  // initialize the LED pin as an output.
  pinMode(LED_BUILTIN, OUTPUT);

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
  digitalWrite(LED_BUILTIN, HIGH);
}

void loop() {
  /** Empty loop. **/
}
