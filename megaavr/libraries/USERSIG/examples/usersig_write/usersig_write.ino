/*
   USERSIG Write

   Stores values read from analog input 0 into the USERSIG.
   These values will stay in the USERSIG when the board is
   turned off and may be retrieved later by another sketch.
*/

#include <USERSIG.h>

/** the current address in the USERSIG (i.e. which byte we're going to write to next) **/
int addr = 0;

void setup() {
  /** Empty setup. **/
}

void loop() {
  /***
    Need to divide by 4 because analog inputs range from
    0 to 1023 and each byte of the USERSIG can only hold a
    value from 0 to 255.
  ***/

  int val = analogRead(0) / 4;

  /***
    Write the value to the appropriate byte of the USERSIG.
    these values will remain there when the board is
    turned off.
  ***/

  USERSIG.write(addr, val);

  /***
     Advance to the next address, when at the end restart at the beginning.

     All (post 2016) parts have 32 bytes of USERSIG/USERROW space.

     Rather than hard-coding the length, you should use the pre-provided length function.
     This will make your code portable to future AVR processors which may have more.
   ***/

  addr = addr + 1;
  if (addr == USERSIG.length()) {
    addr = 0;
  }

  delay(100);
}
