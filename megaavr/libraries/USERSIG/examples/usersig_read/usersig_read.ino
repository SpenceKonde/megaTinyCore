/*
 * USERSIG Read
 *
 * Reads the value of each byte of the USERSIG and prints it
 * to the computer.
 * This example code is in the public domain.
 */

#include <USERSIG.h>

// start reading from the first byte (address 0) of the USERSIG
int address = 0;
byte value;

void setup() {
  Serial.begin(115200);
}

void loop() {
  // read a byte from the current address of the USERSIG
  value = USERSIG.read(address);

  Serial.print(address);
  Serial.print("\t");
  Serial.print(value, DEC);
  Serial.println();

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

  delay(500);
}
