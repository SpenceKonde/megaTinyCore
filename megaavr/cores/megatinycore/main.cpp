/* main.cpp - Main loop for Arduino sketches
   Part of megaTinyCore - github.com/SpenceKonde/megaTinyCore
   Copyright (c) 2018~2021 Spence Konde, (c) 2005-2013 Arduino
   Free Software - LGPL 2.1, please see LICENCE.md for details */

#include <Arduino.h>

/* Required by some libraries to compile successfully. Even though it's nonsense in Arduino. */
int atexit(void ( * /*func*/)()) { return 0; }

void initVariant() __attribute__((weak));
/* Weak empty variant initialization function. The purpose is unclear. It sounds like it was intended
 * initialize the variant, and specific variants would have their own implementation. But in practice
 * it seems to be instead used as an initialization callback that libraries can use to run code before
 * setup, like FreeRTOS - it would have been nice if the official API included such a callback. */
void initVariant() { }

int main()  __attribute__((weak));
/* The main function - call initialization functions (in wiring.c) then setup, and finally loop repeatedly.
 * If serial event is enabled (which should be unusual, as it is no longer a menu option even, that gets
 * checked for after each call to loop.
 * In the past there was a USB-related function here, that is removed, as work will be needed in any event
 * at the core level if VUSB-based "stuff" arrives that needs support at this level */

int main() {
  init(); //Interrupts are turned on just prior to init() returning.
  initVariant();
  setup();
  for (;;) {
    loop();
    #ifdef ENABLE_SERIAL_EVENT
      if (serialEventRun) serialEventRun();
    #endif
  }
}
