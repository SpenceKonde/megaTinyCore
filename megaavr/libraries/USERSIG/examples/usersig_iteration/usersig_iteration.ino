/***
    eeprom_iteration example.

    A set of example snippets highlighting the
    simplest methods for traversing the USERSIG.

    Running this sketch is not necessary, this is
    simply highlighting certain programming methods.

    Written by Christopher Andrews 2015
    Adapted to USERSIG Spence Konde 2020
    Released under MIT licence.
***/

#include <USERSIG.h>

void setup() {

  /***
    Iterate the USERSIG using a for loop.
  ***/

  for (int index = 0 ; index < USERSIG.length() ; index++) {

    //Add one to each cell in the USERSIG
    USERSIG[ index ] += 1;
  }

  /***
    Iterate the USERSIG using a while loop.
  ***/

  int index = 0;

  while (index < USERSIG.length()) {

    //Add one to each cell in the USERSIG
    USERSIG[ index ] += 1;
    index++;
  }

  /***
    Iterate the USERSIG using a do-while loop.
  ***/

  int idx = 0;  //Used 'idx' to avoid name conflict with 'index' above.

  do {

    //Add one to each cell in the USERSIG
    USERSIG[ idx ] += 1;
    idx++;
  } while (idx < USERSIG.length());


} //End of setup function.

void loop() {}
