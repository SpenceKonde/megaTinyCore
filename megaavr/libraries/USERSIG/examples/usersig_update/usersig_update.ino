/***
   USERSIG Update method

   Stores values read from analog input 0 into the USERSIG.
   These values will stay in the USERSIG when the board is
   turned off and may be retrieved later by another sketch.

   If a value has not changed in the USERSIG, it is not overwritten
   which would reduce the life span of the USERSIG unnecessarily.

   Released using MIT licence.
 ***/

#include <USERSIG.h>

/** the current address in the USERSIG (i.e. which byte we're going to write to next) **/
int address = 0;

void setup() {
  /** EMpty setup **/
}

void loop() {
  /***
    need to divide by 4 because analog inputs range from
    0 to 1023 and each byte of the USERSIG can only hold a
    value from 0 to 255.
  ***/
  int val = analogRead(0) / 4;

  /***
    Update the particular USERSIG cell.
    these values will remain there when the board is
    turned off.
  ***/
  USERSIG.update(address, val);

  /***
    The function USERSIG.update(address, val) is equivalent to the following:

    if (USERSIG.read(address) != val) {
      USERSIG.write(address, val);
    }
  ***/


  /***
    Advance to the next address, when at the end restart at the beginning.

    All (post 2016) parts have 32 bytes of USERSIG/USERROW space.

    Rather than hard-coding the length, you should use the pre-provided length function.
    This will make your code portable to future AVR processors which may have more.
  ***/

  address = address + 1;
  if (address == USERSIG.length()) {
    address = 0;
  }

  /***
    As the USERSIG sizes are powers of two, wrapping (preventing overflow) of an
    USERSIG address is also doable by a bitwise and of the length - 1.

    ++address &= USERSIG.length() - 1;
  ***/

  delay(100);
}
