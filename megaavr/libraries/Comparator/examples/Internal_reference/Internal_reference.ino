 /***********************************************************************|
| Modern AVR Comparator library for tinyAVR 0/1/2, megaAVR0, Dx, and  Ex|
|                                                                       |
| Developed in 2019 by MCUdude    https://github.com/MCUdude/           |
| Ported to tinyAVR 2021 by Spence Konde for megaTinyCore               |
| https://github.com/SpenceKonde/megaTinyCore                           |
| Ported to tinyAVR 2022 by Spence Konde for DxCore                     |
| https://github.com/SpenceKonde/DxCore                                 |
|                                                                       |
| In this example we use an internal reference voltage instead of an    |
| external one on the negative pin. This eliminates the need for an     |
| external voltage divider to generate a reference. Note that the       |
| internal reference requires a stable voltage to function properly.    |
|                                                                       |
| This is the formula for the generated voltage:                        |
| Vdacref = (DACREF / 256) * Vref                                       |
|                                                                       |
| Warning: The 0-series parts do not have a DAC. On those parts, you    |
|   can only use the value of the reference itself (as shown below)     |
|                                                                       |
| Warning: The 0/1-series have references of 0.55, 1.1, 1.5, 2.5, and   |
|   4.3 Volts. The 2-series has 1.024, 2.048, 4.096, and 2.5 Volts.     |
|                                                                       |
| Warning: On the 1-series, the DACREF for AC0 is the same DAC that can |
|   output on PIN_PA6 - if both are in use, both will have the same     |
|   reference and setting! This is not an issue with AC1 or AC2 on the  |
|   parts that have these (only 1-series parts with 16k or 32k of flash |
|   have the second and third comparator)                               |
|                                                                       |
|***********************************************************************/

#include <Comparator.h>

void setup() {
  // Configure relevant comparator parameters
  #if defined(MEGATINYCORE) && MEGATINYCORE_SERIES > 0
  Comparator.input_p = comparator::in_p::in0;       // Use positive input 0 (PD2)
  Comparator.input_n = comparator::in_n::dacref;    // Connect the negative pin to the DACREF voltage
  Comparator.reference = comparator::ref::vref_2v5; // Set the DACREF voltage to 2.5V
  Comparator.dacref = 127;              // Gives us 1.24V -> (127 / 256) * 2.5V = 1.24V
  Comparator.hysteresis = comparator::hyst::large;  // Use a 50mV hysteresis
  Comparator.output = comparator::out::enable;      // Enable output on PIN_PA5 (digital pin 1)
  //                                    // or PIN_PA3 (digital pinm 4) on ATtiny412/212)
  #else
  /* for 0-series, there's no DACREF; instead, let's use the 1.1v reference */
  Comparator.input_p = comparator::in_p::in0;       // Use positive input 0 (PD2)
  Comparator.input_n = comparator::in_n::vref;      // Connect the negative pin to the DACREF voltage
  Comparator.reference = comparator::ref::vref_1v1; // Set the DACREF voltage to 2.5V
  Comparator.dacref = 127;              // Gives us 1.24V -> (127 / 256) * 2.5V = 1.24V
  Comparator.hysteresis = comparator::hyst::large;  // Use a 50mV hysteresis
  Comparator.output = comparator::out::enable;      // Enable output on PIN_PA5 (digital pin 1)
  //                                    // or PIN_PA3 (digital pin 4) on ATtiny402/202)
  #endif

  // Initialize comparator
  Comparator.init();

  // Start comparator
  Comparator.start();
}

void loop() {

}
