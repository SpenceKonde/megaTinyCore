/***********************************************************************|
| Developed in 2019 by MCUdude    https://github.com/MCUdude/           |
| https://github.com/SpenceKonde/megaTinyCore                           |
| Ported to tinyAVR 2021 by Spence Konde for megaTinyCore               |
| Ported to tinyAVR 2022 by Spence Konde for DxCore                     |
| https://github.com/SpenceKonde/DxCore                                 |
|                                                                       |
| In this example we use the negative and positive input 0 of the       |
| comparator. The output goes high if the positive input is higher than |
| the negative input, and low otherwise.                                |
|***********************************************************************/

#include <Comparator.h>

void setup() {
  // Configure relevant comparator parameters
  Comparator.input_p = comparator::in_p::in0;      // Use positive input 0 (PA7)
  Comparator.input_n = comparator::in_n::in0;      // Use negative input 0 (PA6)
  Comparator.output = comparator::out::enable;     // Enable output on PIN_PA5 (digital pin 1)
  //                                   // or PIN_PA3 (digital pin 4) on ATtiny402/202)

  // Initialize comparator
  Comparator.init();

  // Start comparator
  Comparator.start();
}

void loop() {

}
