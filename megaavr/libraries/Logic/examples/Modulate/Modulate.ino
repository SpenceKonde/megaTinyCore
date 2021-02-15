/***********************************************************************|
  | tinyAVR Configurable Custom Logic library                             |
  |                                                                       |
  | Modulate.ino                                                   |
  |                                                                       |
  | A library for interfacing with the megaAVR Configurable Custom Logic. |
  | Developed in 2019 by MCUdude.                                         |
  | https://github.com/MCUdude/                                           |
  |                                                                       |
  | In this example we use the configurable logic peripherals of the      |
  | tinyAVR to achieve the "modulate one timer's wave output with another"|
  | behavior like some of the more full-featured classic megaAVR parts    |
  | We set one input to TCA0 WO1, the other to TCB0 WO, and mask the third|
  | and set the truth table so that the output is only HIGH when both are |
  | As can be immediately seen, this is vastly more powerful than what the|
  | classic AVRs were capable of, where this could be done only with two  |
  | timers on a single pin.                                               |
  | We could even use the CCL as an event generator to move the output to |
  | one of the EVOUT pins!                                                |
  |***********************************************************************/

// Make sure this compiles on 8-pin parts for the automated tests...
#if !defined(__AVR_ATtinyxy2__)
  #define PIN_TCA_WO1 PIN_PB1
#else
  #define PIN_TCA_WO1 PIN_PA1
#endif


#include <Logic.h>

void setup() {


  Logic0.enable = true;               // Enable logic block 0
#if MEGATINYCORE_SERIES!=2            // tinyAVR 1-series parts with 16k or 32k flash have a second TCB.
  Logic0.input0 = in::tcb0;           // and either of them can be used by any of the CCL inputs.
#else                                 // Every other AVR with a CCL didn't do it like that - they have one
  Logic0.input0 = in::tcb0;           // TCB channel, and if that's input0, it's TCB0, if it's input1, TCB1, and if input2, it's
#endif                                // TCB2 (on larger chips) On 0/1-series, in::tcb refers to TCB0, same as in::tcb0.
  // All tinies define in::tcb0, only the cadilac ones with a TCB1 define in::tcb1. Non-tinies do not define in::tcb0 or in::tcb1.
  Logic0.input1 = in::tca0;           // Use TCA0 WO1 as input0
  Logic0.input2 = in::masked;         // mask input 2
  Logic0.output = out::enable;        // Enable logic block 0 output pin or PA4 (ATtiny))
  Logic0.filter = filter::disable;    // No output filter enabled
  Logic0.truth = 0x08;                // Set truth table - HIGH only if both high

  // Initialize logic block 0
  Logic0.init();

  // Start the AVR logic hardware
  Logic::start();

  analogWrite(PIN_TCA_WO1, 128); //start TCA0 WO0 running
  TCB0.CTRLA = 0x01; //enabled with CLKPER as clock source
  TCB0.CTRLB = 0x07; //PWM8 mode, but output pin not enabled
  TCB0.CCMPL = 255; //255 counts
  TCB0.CCMPH = 128; //50% duty cycle
}

void loop() {
  // When using configurable custom logic the CPU isn't doing anything!
}
