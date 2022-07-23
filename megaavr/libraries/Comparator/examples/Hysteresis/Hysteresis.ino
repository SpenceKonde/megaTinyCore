/***********************************************************************|
| tinyAVR 0/1/2-series analog comparator library                        |
|                                                                       |
| Hysteresis.ino                                                        |
|                                                                       |
| A library for interfacing with the tinyAVR analog comparator(s).      |
| Developed in 2019 by MCUdude    https://github.com/MCUdude/           |
| Ported to tinyAVR 2021 by Spence Konde for megaTinyCore               |
| https://github.com/SpenceKonde/megaTinyCore                           |
|                                                                       |
| In this example we use the negative and positive input 0 of the       |
| comparator. The output goes high if the positive input is higher than |
| the negative input, and low otherwise. We'll also use the built-in    |
| hysteresis functionality to prevent spurious signals while the        |
| voltage on the two pins is very close                                 |
|***********************************************************************/

#include <Comparator.h>

void setup() {
  // Configure relevant comparator parameters
  Comparator.input_p = comparator::in_p::in0;      // Use positive input 0 (PA7)
  Comparator.input_n = comparator::in_n::in0;      // Use negative input 0 (PA6)
  Comparator.hysteresis = comparator::hyst::large; // Use a 50mV hysteresis
  Comparator.output = comparator::out::enable;     // Enable output on PIN_PA5 (digital pin 1)
  //                                   // or PIN_PA3 (digital pin 4) on ATtiny402/202)

  // Initialize comparator
  Comparator.init();

  // Start comparator
  Comparator.start();
}

void loop() {

}
