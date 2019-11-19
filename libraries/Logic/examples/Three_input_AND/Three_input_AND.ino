/***********************************************************************|
| megaAVR Configurable Custom Logic library                             |
|                                                                       |
| Three_input_AND.ino                                                   |
|                                                                       |
| A library for interfacing with the megaAVR Configurable Custom Logic. |
| Developed in 2019 by MCUdude.                                         |
| https://github.com/MCUdude/                                           |
|                                                                       |
| In this example we use the configurable logic peripherals the the     |
| megaAVR to create a 3-input AND gate using logic block 0 on PORT A.   |
| The example is pretty straight forward, but the truth table value may |
| be a little difficult to understand at first glance.                  |
| Here's how 0x80 turns out to be the correct value to create a 3-input |
| AND gate:                                                             |
|                                           3-input AND truth table:    |
| If we look at the truth table             |PA2|PA1|PA0| Y |           |
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
|***********************************************************************/

#include <Logic.h>

void setup()
{
  // Initialize logic block 0
  // Logic block 0 has three inputs, PA0, PA1 and PA2.
  // It has one output, PA3, but can be swapped to PA6 if needed
  Logic0.enable = true;               // Enable logic block 0
  Logic0.input0 = in::input_pullup;   // Set PA0 as input with pullup
  Logic0.input1 = in::input_pullup;   // Set PA1 as input with pullup
  Logic0.input2 = in::input_pullup;   // Set PA2 as input with pullup
//Logic0.output_swap = out::pin_swap; // Uncomment this line to route the output to PA6 instead of PA3
  Logic0.output = out::enable;        // Enable logic block 0 output pin (PA3)
  Logic0.filter = filter::disable;    // No output filter enabled
  Logic0.truth = 0x80;                // Set truth table

  // Initialize logic block 0
  Logic0.init();

  // Start the AVR logic hardware
  Logic::start();
}

void loop()
{
  // When using configurable custom logic the CPU isn't doing anything!
}
