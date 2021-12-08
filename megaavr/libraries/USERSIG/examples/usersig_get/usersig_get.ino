/***
    eeprom_get example.

    This shows how to use the USERSIG.get() method.

    To pre-set the USERSIG data, run the example sketch eeprom_put.
    This sketch will run without it, however, the values shown
    will be shown from what ever is already on the USERSIG.

    This may cause the serial object to print out a large string
    of garbage if there is no null character inside one of the strings
    loaded.

    Written by Christopher Andrews 2015
    Adapted to USERSIG Spence Konde 2020
    Released under MIT licence.
***/

#include <USERSIG.h>

void setup() {

  float f = 0.00f;   // Variable to store data read from USERSIG.
  int eeAddress = 0; // USERSIG address to start reading from

  Serial.begin(115200);

  Serial.print("Read float from USERSIG: ");

  // Get the float data from the USERSIG at position 'eeAddress'
  USERSIG.get(eeAddress, f);
  Serial.println(f, 3);    // This may print 'ovf, nan' if the data inside the USERSIG is not a valid float.

  /***
    As get also returns a reference to 'f', you can use it inline.
    E.g: Serial.print(USERSIG.get(eeAddress, f));
  ***/

  /***
    Get can be used with custom structures too.
    I have separated this into an extra function.
  ***/

  secondTest(); // Run the next test.
}

struct MyObject {
  float field1;
  byte field2;
  char name[10];
};

void secondTest() {
  int eeAddress = sizeof(float); // Move address to the next byte after float 'f'.

  MyObject customVar; // Variable to store custom object read from USERSIG.
  USERSIG.get(eeAddress, customVar);

  Serial.println("Read custom object from USERSIG: ");
  Serial.println(customVar.field1);
  Serial.println(customVar.field2);
  Serial.println(customVar.name);
}

void loop() {
  /* Empty loop */
}
