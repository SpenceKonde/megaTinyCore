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

// Function to print information about the passed event
void print_event_info(Event &my_event) {
  Serial.printf("This is event channel no. %d\n", my_event.get_channel_number());
  Serial.printf("This channel uses generator no. 0x%02x, which you can find in Event.h\n", my_event.get_generator());
}

void print_user_info(user::user_t my_user) {
  // Event::get_user_channel() returns -1 if the user isn't connected to any event generator
  Serial.printf("User 0x%02x is connected to event channel no. %d\n\n", my_user, Event::get_user_channel(my_user));
}

void setup() {
  Serial.begin(115200); // Initialize hardware serial port

  Event2.set_generator(gen2::pin_pa1); // Set pin PE0 as event generator
  #if MEGATINYCORE_SERIES == 2
  Event3.set_generator(gen3::pin_pa6); // Set pin PE1 as event generator
  #else
  Event3.set_generator(gen3::pin_pb1); // Set pin PE1 as event generator
  #endif
  // For more information about EVOUT, see the PORTMUX section in the datasheet
  Event2.set_user(user::evouta_pin_pa2); // Set EVOUTE as event user
  Event3.set_user(user::evoutb_pin_pb2); // Set EVOUTF as event user

  // Start event channels
  Event2.start();
  Event3.start();
}

void loop() {
  // Print info about Event4 and its event user
  print_event_info(Event2);
  print_user_info(user::evouta_pin_pa2);

  // Print info about Event3 and its event user
  print_event_info(Event3);
  print_user_info(user::evoutb_pin_pb2);

  delay(5000);
}
