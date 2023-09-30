/***********************************************************************|
| Configurable Custom Logic library                                     |
| Developed in 2019 by MCUdude. https://github.com/MCUdude/             |
| Ported to DxC and maintained for mTC and DxC by Sprence Konde         |
| Example by Spence Konde                                               |
|                                                                       |
| LatchNoSeq.ino - Getting "RS Latch" like behavior with a single even  |
| numbered LUT - or with an odd LUT and an event channel                |
|                                                                       |
| In this example we use the configurable logic peripherals in AVR      |
| to act as a "latch" WITHOUT using both LUTs and the sequencer         |
| For the even-numbered logic block(s) we can simply use the feedback   |
| input. Otherwise we need to use the event system.                     |
| Because we can use the output of the LUT, what we want is a truth     |
| table that will maintain a HIGH or LOW if no buttons are prssed. We   |
| also need to make sure that it handles the "both buttons pressed"     |
| case gracefully; three of the 4 options work:
| * Maintain current state if both buttons are depressed. This means    |
|   while both buttons are pressed, the output will be as dictated by
|   the first button pressed, and when one button is released, if the   |
|   other button would flip it, it will. This was used here, partly     |
|   because the truth table ends up the same whether the inputs are     |
|   active high or low                                                  |
| * Set the output LOW whenever both inputs are asserted.               |
|   Assuming active low inputs, this corresponds to truth = 0x8C        |
| * Set the output HIGH whenever both inputs are asserted               |
|   Assuming active low inputs, this corresponds to truth = 0x8F        |
| * ~Set the output to the opposite state when both inputs asserted~     |
|   This does not produce something that acts like a latch. It will     |
|   oscillate at a very high frequency when both inputs are on, as it    |
|   will be switching between two state, each of which tell it to       |
|   switch again. This would correspond to 0x8D in the truth table; so  |
|   avoid that one.
|                                           3-input     truth table:    |
| We use CCL LUT event as our "feedback",   |PA2|PA1|CCL| Y |           |
| PA1 is RESET and PA2 is SET, both         |---|---|---|---|           |
| active low                                | 0 | 0 | 0 | 0 |           |
| Connect a button between those and Gnd    | 0 | 0 | 1 | 1 |           |
| Pressing button on PA2 will set output    | 0 | 1 | 0 | 1 |           |
| HIGH and pressing button on PA1 will set  | 0 | 1 | 1 | 1 |           |
| output LOW, and pressing neither will do  | 1 | 0 | 0 | 0 |           |
| nothing.                                  | 1 | 0 | 1 | 0 |           |
| We could even then fire an interrupt from | 1 | 1 | 0 | 0 |           |
| that pin                                  | 1 | 1 | 1 | 1 |           |
|                                                                       |
| The sky (well, and the number of LUTs) is the limit!!                 |
|                                                                       |
| Warning:                                                              |
| The same library is used for mTC and DxC; this example is used on both|
| As it is used in our compile testing, it needs to compile on both the |
| tinyAVRs and the Dx and Ex series. As a consequence, because this     |
| involves EVSYS, which is very different on tiny0/1 vs everything else |
| a number of #ifdef statements are used to select the right            |
| implementation.                                                       |
|***********************************************************************/

#include <Logic.h>

void setup() {
  // Initialize logic block 0
  // Logic block 0 has three inputs, PA0, PA1 and PA2.
  // Because PA0 is shared with the UPDI pin on tinyAVR parts
  // we use the other two as our button inputs.
  // It outputs on the LUT0 OUT pin - PA4 (alt. PB4) on ATtiny
  // or PA2 (alt PA6) everywhere else.

  Logic0.enable = true;               // Enable logic block 0

  #if defined(MEGATINYCORE) || defined(MEGATINYCORE_SERIES) != 2
  EVSYS.CHANNEL0 = EVSYS_CHANNEL0_CCL_LUT0_gc;
  EVSYS.USERCCLLUT1A = EVSYS_USER_CHANNEL0_gc;
  #else // it's a tinyAVR 0/1
  EVSYS.ASYNCCH0 = EVSYS_ASYNCCH0_CCL_LUT0_gc;      // Use CCL LUT0 as event generator
  EVSYS.ASYNCUSER2 = EVSYS_ASYNCUSER2_ASYNCCH0_gc;  // ASYNCUSER2 is LUT0 event 0
  #endif
  Logic0.input0 = logic::in::event_0;                      // Use event 0 as input0
  Logic0.input1 = logic::in::input_pullup;                 // PA1 as input1 (RESET)
  Logic0.input2 = logic::in::input_pullup;                 // PA2 as input2 (SET)
  // Logic0.output_swap = logic::out::pin_swap; // Uncomment this line to route the output to alternate location, if available.
  Logic0.output = logic::out::enable;        // Enable logic block 0 output pin (see pinout chart)
  Logic0.filter = logic::filter::disable;    // No output filter enabled
  Logic0.truth = 0x8E;                // Set truth table - HIGH only if both high

  // Initialize logic block 0
  Logic0.init();

  // Example for odd-number block where we can't use feedback to get it's own output

  #ifdef EVSYS_CHANNEL0 // means it's not a 0/1-series
  EVSYS.CHANNEL0 = EVSYS_CHANNEL0_CCL_LUT1_gc;
  EVSYS.USERCCLLUT1A = EVSYS_USER_CHANNEL0_gc;
  #else                                             // it's a tinyAVR 0/1 - EVSYS is weird there
  EVSYS.ASYNCCH0 = EVSYS_ASYNCCH0_CCL_LUT1_gc;      // Use CCL LUT1 as event generator
  EVSYS.ASYNCUSER4 = EVSYS_ASYNCUSER2_ASYNCCH0_gc;  // ASYNCUSER4 is LUT1 event 0
  #endif

  Logic1.input0 = in::event_a;          // same distribution of inputs (though there isn't the tinyAVR PA0 issue forcing it here)
  Logic1.input1 = in::input_pullup;     // | get it from LUT 1 event a or event 0 (tinyAVR 0/1 documentation calls them 0 and 1
  Logic1.input2 = in::input_pullup;     // | Logic.h accepts both for all parts, but we recommend the new nomenclature)
  //Logic1.output_swap = out::pin_swap; // Uncomment this line and comment out the next one to route the output to alternate location,
  Logic1.output = out::enable;          // | if available. Output pins on Dx are pins 3 and 6 on the LUT's "home port".
  Logic1.filter = filter::disable;      // No output filter enabled
  Logic1.truth = 0x8E;                  // Set truth table - 0 = LOW, 1-3 = HIGH, 4-6 = LOW, 7 = HIGH
  Logic1.init();                        // Initialize logic block 0
  Logic::start();                       // Start the AVR logic hardware
}

void loop() {
  // When using configurable custom logic the CPU isn't doing anything!
}
