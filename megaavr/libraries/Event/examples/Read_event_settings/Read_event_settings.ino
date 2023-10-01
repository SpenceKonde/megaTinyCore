/***********************************************************************|
| megaAVR event system library                                          |
|                                                                       |
| Read_event_settings.ino                                               |
|                                                                       |
| A library for interfacing with the megaAVR event system.              |
| Developed in 2021 by MCUdude and adapted to tinyAVR by Spence Konde   |
| https://github.com/MCUdude/                                           |
|                                                                       |
| In this example, we demonstrate the possibilities of reading out what |
| event channel we're working with, what generator is used, and which   |
| channel an event user has been connected to.                          |
| Notice the #if to use different code depending on what kind of part it|
| is - we wanted to use 2 channels, and channel 2 is one of the few     |
| channels that have some of the same pins on them.                     |
|                                                                       |
| See Microchip's application note AN2451 for more information.         |
|***********************************************************************/

#include <Event.h>
#define MYSERIAL Serial
// Function to print information about the passed event
void print_event_info(Event &my_event) {
  MYSERIAL.printf("This is event channel no. %d\n", my_event.get_channel_number());
  MYSERIAL.printf("This channel uses generator no. 0x%02x, which you can find in Event.h\n", my_event.get_generator());
}

void print_user_info(user::user_t my_user) {
  // Event::get_user_channel() returns -1 if the user isn't connected to any event generator
  MYSERIAL.printf("User 0x%02x is connected to event channel no. %d\n\n", my_user, Event::get_user_channel_number(my_user));
}

void setup() {
  MYSERIAL.begin(115200); // Initialize hardware serial port

  Event0.set_generator(gen0::pin_pa6); // Set pin PA6 as event generator

  // For more information about EVOUT, see the PORTMUX section in the datasheet
  Event0.set_user(user::evouta_pin_pa2); // Set EVOUTA as event user

  // Start event channels
  Event0.start();
  #if !defined(MEGATINYCORE) || _AVR_PINCOUNT > 8
  // again for the second channel, if we have
  Event2.set_generator(gen1::pin_pa4); // Set pin PA4 as event generator
  Event2.set_user(user::evoutb_pin_pb2); // Set EVOUTB as event user
  Event2.start();
  #endif
}

void loop() {
  // Print info about Event0 and its event user
  print_event_info(Event0);
  print_user_info(user::evoutb_pin_pa2);

  #if !defined(MEGATINYCORE) || MEGATINYCORE_SERIES == 2
  // Print info about Event2 and its event user
  print_event_info(Event2);
  print_user_info(user::evoutb_pin_pb2);
  #endif
  delay(5000);
}
