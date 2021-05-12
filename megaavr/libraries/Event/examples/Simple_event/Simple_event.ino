/***********************************************************************|
| megaAVR event system library                                          |
|                                                                       |
| Simple_Event.ino                                                      |
|                                                                       |
| A library for interfacing with the megaAVR event system.              |
| Developed in 2021 by MCUdude                                          |
| https://github.com/MCUdude/                                           |
|                                                                       |
| In this example we use pin PE0 as an event generator, and pin PE2 and |
| PF2 as event users. In practive, pin PE2 and PF2 will follow          |
| PE0's state.                                                          |
|                                                                       |
| See Microchip's application note AN2451 for more information.         |
|***********************************************************************/

#include <Event.h>

void setup() {
  // Since pin PE0 is only available on event generator channel 4 and 5, we use Event4 as our object
  // Note that we use gen4:: to refer to functionality unique to event channel 4
  Event4.set_generator(gen4::pin_pe0); // Set pin PE0 as event generator

  // For more information about EVOUT, see the PORTMUX section in the datasheet
  Event4.set_user(user::evoute_pin_pe2); // Set EVOUTE as event user
  Event4.set_user(user::evoutf_pin_pf2); // Set EVOUTF as event user

  // Start the event channel
  Event4.start();
}

void loop() {

}
