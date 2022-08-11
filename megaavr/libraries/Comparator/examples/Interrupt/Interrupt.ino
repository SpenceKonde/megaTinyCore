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
| Instead of using a physical output pin we're instead triggering an    |
| interrupt that will run a user defined function.                      |
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
/* This flag will be set true by the interrupt. Since it's modified in
 * an interrupt but read outside of that interrupt, it must be volatile.
 */
volatile bool int_fired = 0;

void setup() {
  // Configure serial port
  Serial.begin(115200);

  // Configure relevant comparator parameters
  Comparator.input_p = comparator::in_p::in0;       // Use positive input 0 (PA7)
  #if MEGATINYCORE_SERIES == 0
  Comparator.input_n = comparator::in_n::vref;      // 0-series has no DACREF, so use vref directly.
  #else
  Comparator.input_n = comparator::in_n::dacref;    // Connect the negative pin to the DACREF voltage
  Comparator.dacref = 255;              // Gives us 2.5V -> (255 / 256) * 2.5V = 2.5V
  #endif
  Comparator.reference = comparator::ref::vref_2v5; // Set the DACREF voltage to 2.5V
  Comparator.hysteresis = comparator::hyst::large;  // Use a 50mV hysteresis
  Comparator.output = comparator::out::disable;     // Use interrupt trigger instead of output pin

  // Initialize comparator
  Comparator.init();

  // Set interrupt (supports RISING, FALLING and CHANGE)
  Comparator.attachInterrupt(interruptFunction, RISING);

  // Start comparator
  Comparator.start();
}

void loop() {
  if (int_fired) { // Check our flag here
    Serial.println("Output of analog comparator went high!");
    int_fired = 0; // clear our flag so we don't sit there spamming the serial port
  }
}

// This function runs when an interrupt occurs
void interruptFunction() {
  /* You might want to do this, but no! Do not do this!
  Serial.println("Output of analog comparator went high!");
  * Avoid printing to serial within an ISR. The print functions can potentially block for,
  * in the worst case (tx buffer full when print is started), as long as the entire string
  * takes to print (calculated as characters divided by baud/10 - each byte is sent as
  * 10 bits, because there's a start and stop bit. At best, this would cause millis to lose
  * time, and at worst block other critical functions. If you really need to, printing a
  * single character is much less of a hazard than a longer message, and is acceptable for
  * debugging purposes (though still undesirable), as that both reduces the worst case
  * execution time and reduces the chance of there being any blocking delay- only a completely
  * full TX buffer would cause it. This also makes it harder for the interrupt to fire so
  * frequently that the messages it tries to print will outrun the serial port - which is the
  * nightmare scenario.
  * The correct approach is what we demonstrate - set a flag that you can check on the
  * next pass through loop.
  */
  int_fired = 1; // This can be kept short and fast,
}

/* Note that we now support manually defininng interrupts - if neither attach or detach
 * are referenced, efficient interrupts will execute around 50 clocks faster
 */
