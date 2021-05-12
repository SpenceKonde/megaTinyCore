/***********************************************************************|
| megaAVR event system library                                          |
|                                                                       |
| Route_logic_pins.ino                                                  |
|                                                                       |
| A library for interfacing with the megaAVR event system.              |
| Developed in 2021 by MCUdude and adapted to tinyAVR by Spence Konde   |
| https://github.com/MCUdude/                                           |
|                                                                       |
| In this example we use the configurable logic peripherals on the      |
| megaAVR to create a 3-input AND gate using logic block 0, but we      |
| utilize the event system to route logic input 0 and 1 to pins PB0 and |
| PB1 instead of the default pins.                                      |
| Here's how 0x80 turns out to be the correct value to create a 3-input |
| AND gate:                                                             |
|                                           3-input AND truth table:    |
| If we look at the truth table             |PA2|PB1|PB0| Y |           |
| to the right, we can see that             |---|---|---|---|           |
| all binary values for Y can               | 0 | 0 | 0 | 0 |           |
| be represented as 10000000.               | 0 | 0 | 1 | 0 |           |
| If we convert this 8-bit                  | 0 | 1 | 0 | 0 |           |
| binary number into hex, we                | 0 | 1 | 1 | 0 |           |
| get 0x80.                                 | 1 | 0 | 0 | 0 |           |
|                                           | 1 | 0 | 1 | 0 |           |
| In this example the output pin,           | 1 | 1 | 0 | 0 |           |
| PA3 will go high if all three             | 1 | 1 | 1 | 1 |           |
| inputs are high.                                                      |
|                                                                       |
| Notice that we have to use different channels on  1-series and        |
| 2-series parts. This would require even more modification for 0-series|
| as it has only three channels and channel 1 is not one of them        |
|                                                                       |
| See Microchip's application note AN2451 for more information.         |
|***********************************************************************/

#include <Event.h>
#include <Logic.h>

void setup() {

  #if MEGATINYCORE_SERIES == 1
  // Initialize Event channel 1 and 3  if on 1-series
  Event1.set_generator(gen1::pin_pb0); // Set pin PB0 as event generator
  Event3.set_generator(gen3::pin_pb1); // Set pin PB1 as event generator
  Event1.set_user(user::ccl0_event_a); // Set CCL0 (Logic0) event A as user
  Event3.set_user(user::ccl0_event_b); // Set CCL0 (Logic0) event B as user
  #elif MEGATINYCORE_SERIES == 2 // or channel 2 and 3  if on 2-series
  Event2.set_generator(gen2::pin_pb0); // Set pin PB0 as event generator
  Event3.set_generator(gen3::pin_pb1); // Set pin PB1 as event generator
  Event2.set_user(user::ccl0_event_a); // Set CCL0 (Logic0) event A as user
  Event3.set_user(user::ccl0_event_b); // Set CCL0 (Logic0) event B as user
  #endif

  // Initialize logic block 0
  Logic0.enable = true;                // Enable logic block 0
  Logic0.input0 = in::event_a;         // Connect input 0 to ccl0_event_a (PC0 through Event2)
  Logic0.input1 = in::event_b;         // Connect input 0 to ccl0_event_b (PC1 through Event3)
  Logic0.input2 = in::input;           // Set PA2 as input
  Logic0.output = out::enable;         // Enable logic block 0 output pin (PA3)
  Logic0.truth = 0x80;                 // Set truth table
  Logic0.init();

  // Start event channels and the logic hardware

  #if MEGATINYCORE_SERIES == 1
  Event1.start();
  Event3.start();
  #elif MEGATINYCORE_SERIES == 2
  Event2.start();
  Event3.start();
  #endif
  Logic::start();
}

void loop() {

}
