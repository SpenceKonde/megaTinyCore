/***********************************************************************|
| tinyAVR Configurable Custom Logic library                             |
|                                                                       |
| Three_input_AND.ino                                                   |
|                                                                       |
| A library for interfacing with the megaAVR Configurable Custom Logic. |
| Developed in 2019 by MCUdude.                                         |
| https://github.com/MCUdude/                                           |
|                                                                       |
| In this example we use the configurable logic peripherals of the      |
| tinyAVR to create a 3-input AND gate using logic block 0 on PORT A.   |
| The example is pretty straight forward, but the truth table value may |
| be a little difficult to understand at first glance.                  |
| Here's how 0x80 turns out to be the correct value to create a 3-input |
| This also demonstrates how to work around that IN0 to LUT0 is UPDI pin|
| AND gate:                                                             |
|                                           3-input AND truth table:    |
| If we look at the truth table             |PA2|PA1|PA6| Y |           |
| to the right, we can see that             |---|---|---|---|           |
| all binary values for Y can               | 0 | 0 | 0 | 0 |           |
| be represented as 10000000.               | 0 | 0 | 1 | 0 |           |
| If we convert this 8-bit                  | 0 | 1 | 0 | 0 |           |
| binary number into hex, we                | 0 | 1 | 1 | 0 |           |
| get 0x80.                                 | 1 | 0 | 0 | 0 |           |
|                                           | 1 | 0 | 1 | 0 |           |
| In this example the output pin            | 1 | 1 | 0 | 0 |           |
| will go high if all three                 | 1 | 1 | 1 | 1 |           |
| inputs are high.                                                      |
|                                                                       |
| The other 3-input logical operations can be achieved with just a      |
| different truth table.                                                |
|***********************************************************************/

#include <Logic.h>

void setup() {
  // Initialize logic block 0
  // Logic block 0 has three inputs, PA0, PA1 and PA2.
  // These are the pins directly above the UPDI pin
  // Because PA0 is shared with the UPDI pin and is not usually an option
  // we use PA6 via the event system in this example on ATtiny parts
  // It has one output, PA5 on ATtiny, or alternate PB6 on 20 and 24-pin ATtiny.

  Logic0.enable = true;               // Enable logic block 0

  // For ATtiny parts, we have to work around PA0 being UPDI
  pinMode(PIN_PA6, INPUT_PULLUP);                   // Could be done faster with direct port writes
  #ifdef EVSYS_CHANNEL0 // means it's a 2-series, where the event system works like it does on everything other than the tinyAVR 0/1-series
  EVSYS.CHANNEL0 = EVSYS_CHANNEL0_PORTA_PIN3_gc;
  EVSYS.USERCCLLUT0A = EVSYS_USER_CHANNEL0_gc;
  #else // it's a tinyAVR 0/1
  EVSYS.ASYNCCH0 = EVSYS_ASYNCCH0_PORTA_PIN3_gc;    //
  EVSYS.ASYNCUSER2 = EVSYS_ASYNCUSER2_ASYNCCH0_gc;  // ASYNCUSER2 is LUT0 event 0
  #endif
  Logic0.input0 = in::event_a;                      // Use LUT event 0/a as input 0

  Logic0.input1 = in::input_pullup;   // Set PA1 as input with pullup
  Logic0.input2 = in::input_pullup;   // Set PA2 as input with pullup
  // Logic0.output_swap = out::pin_swap; // Uncomment this line to route the output to alternate location, if available.
  Logic0.output = out::enable;        // Enable logic block 0 output pin or PA4
  Logic0.filter = filter::disable;    // No output filter enabled
  Logic0.truth = 0x80;                // Set truth table

  // Initialize logic block 0
  Logic0.init();

  // Start the AVR logic hardware
  Logic::start();

}

void loop() {
  // When using configurable custom logic the CPU isn't doing anything!
}
