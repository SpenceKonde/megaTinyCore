/***********************************************************************|
| tinyAVR 0/1/2-series analog comparator library                        |
|                                                                       |
| Interrupt.ino                                                         |
|                                                                       |
| A library for interfacing with the tinyAVR analog comparator(s).      |
| Developed in 2019 by MCUdude  https://github.com/MCUdude/             |
| Ported to tinyAVR 2021 by Spence Konde for megaTinyCore               |
| https://github.com/SpenceKonde/megaTinyCore                           |
|                                                                       |
| In this example we use an internal reference voltage instead of an    |
| external one on the negative pin. This eliminates the need for an     |
| external voltage divider to generate a reference. Note that the       |
| internal reference requires a stable voltage to function properly.    |
| Instead of using a physical output pin we're instead triggering an    |
| interrupt that will run a user defined function.                      |
|                                                                       |
| This is the formula for the generated voltage:                        |
| Vdacref = (DACREF / 256) * Vref                                       |
|                                                                       |
| On the 0-series there is no DACREF option - but we're using the full  |
| reference voltage here anyway, so we instead specify in_n::vref       |
|                                                                       |
|***********************************************************************/

#include <Comparator.h>

void setup() {
  // Configure serial port
  Serial.begin(57600);

  // Configure relevant comparator parameters
  Comparator.input_p = in_p::in0;       // Use positive input 0 (PA7)
  #if MEGATINYCORESERIES == 0
  Comparator.input_n = in_n::vref;      // 0-series has no DACREF, so use vref directly.
  #else
  Comparator.input_n = in_n::dacref;    // Connect the negative pin to the DACREF voltage
  Comparator.dacref = 255;              // Gives us 2.5V -> (255 / 256) * 2.5V = 2.5V
  #endif
  Comparator.reference = ref::vref_2v5; // Set the DACREF voltage to 2.5V
  Comparator.hysteresis = hyst::large;  // Use a 50mV hysteresis
  Comparator.output = out::disable;     // Use interrupt trigger instead of output pin

  // Initialize comparator
  Comparator.init();

  // Set interrupt (supports RISING, FALLING and CHANGE)
  Comparator.attachInterrupt(interruptFunction, RISING);

  // Start comparator
  Comparator.start();
}

void loop() {

}

// This function runs when an interrupt occurs
void interruptFunction() {
  Serial.println("Output of analog comparator went high!");
}
