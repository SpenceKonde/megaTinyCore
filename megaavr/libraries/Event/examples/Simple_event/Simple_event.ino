/***********************************************************************|
| megaAVR event system library                                          |
|                                                                       |
| Simple_Event.ino                                                      |
|                                                                       |
| A library for interfacing with the megaAVR event system.              |
| Developed in 2021 by MCUdude and adapted to tinyAVR by Spence Konde   |
| https://github.com/MCUdude/                                           |
|                                                                       |
| In this example we use pin PA1 as an event generator, and pin PA2 and |
| as event users. In practive, pin PA2 will follow PA1's state          |
| This trivial example is works without modification on both the 0/1-   |
| series and the 2=series. with no #ifdefs. That is rare.               |
|                                                                       |
| See Microchip's application note AN2451 for more information.         |
|***********************************************************************/

#include <Event.h>

void setup() {

  // For
  // Note that we use gen2:: to refer to functionality unique to event channel 2
  Event2.set_generator(gen2::pin_pd1); // Set pin PA1 as event generator

  // For more information about EVOUT, see the PORTMUX section in the datasheet
  Event2.set_user(user::evouta_pin_pa2); // Set EVOUTA as event user

  // Start the event channel
  Event2.start();
}

void loop() {

}
