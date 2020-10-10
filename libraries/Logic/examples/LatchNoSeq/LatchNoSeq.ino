  /***********************************************************************|
  | megaAVR Configurable Custom Logic library                             |
  |                                                                       |
  | LatchNoSeq.ino                                                        |
  |                                                                       |
  | A library for interfacing with the megaAVR Configurable Custom Logic. |
  | Developed in 2019 by MCUdude.                                         |
  | https://github.com/MCUdude/                                           |
  |                                                                       |
  | In this example we use the configurable logic peripherals in the      |
  | tinyAVR to act as a "latch" WITHOUT using both LUTs and the sequencer |
  | We just use the event system to route the output to one of the inputs |
  | Here we use input0 because that's the UPDI pin and is less usable     |
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
  | that pin (interrupt from CCL is normally  | 1 | 1 | 1 | 1 |           |
  | not supported until the 2-series!)                                    |
  | The sky (well, and the number of LUTs) is the limit!!                 |
  |***********************************************************************/

#include <Logic.h>

void setup() {
  // Initialize logic block 0
  // Logic block 0 has three inputs, PA0, PA1 and PA2.
  // These are the pins directly above the UPDI pin
  // Because PA0 is shared with the UPDI pin and is not usually an option
  // we use PA3 via the event system in this example on ATtiny parts
  // It has one output, PA5 on ATtiny, or alternate PB6 on 20 and 24-pin ATtiny.

  Logic0.enable = true;               // Enable logic block 0

  #ifdef EVSYS_CHANNEL0 //means it's a 2-series, where the event system works like it does on everything other than the tinyAVR 0/1-series
    EVSYS.CHANNEL0=EVSYS_CHANNEL0_CCL_LUT0_gc;
    EVSYS.USERCCLLUT1A=EVSYS_USER_CHANNEL0_gc;
  #else //it's a tinyAVR 0/1
    EVSYS.ASYNCCH0 = EVSYS_ASYNCCH0_CCL_LUT0_gc;      // Use CCL LUT0 as event generator
    EVSYS.ASYNCUSER2 = EVSYS_ASYNCUSER2_ASYNCCH0_gc;  // ASYNCUSER2 is LUT0 event 0
  #endif
  Logic0.input0 = in::event_0;                      // Use event 0 as input0
  Logic0.input1 = in::input_pullup;                 // PA1 as input1 (RESET)
  Logic0.input2 = in::input_pullup;                 // PA2 as input2 (SET)
  //Logic0.output_swap = out::pin_swap; // Uncomment this line to route the output to alternate location, if available.
  Logic0.output = out::enable;        // Enable logic block 0 output pin or PA4 (ATtiny))
  Logic0.filter = filter::disable;    // No output filter enabled
  Logic0.truth = 0x8E;                // Set truth table - HIGH only if both high

  // Initialize logic block 0
  Logic0.init();

  // Start the AVR logic hardware
  Logic::start();
}

void loop() {
  // When using configurable custom logic the CPU isn't doing anything!
}
