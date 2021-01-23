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
  | We set one input to TCA0 WO1, the other to TCB1 WO, and mask the third|
  | and set the truth table so that the output is only HIGH when both are |
  | As can be immediately seen, this is vastly more powerful than what the|
  | classic AVRs were capable of, where this could be done only with two  |
  | timers on a single pin.                                               |
  | We could even use the CCL as an event generator to move the output to |
  | one of the EVOUT pins!                                                |
  |***********************************************************************/

#include <Logic.h>

void setup() {
  // Initialize logic block 0
  // Logic block 0 has three inputs, PA0, PA1 and PA2.
  // These are the pins directly above the UPDI pin
  // Because PA0 is shared with the UPDI pin and is not usually an option
  // we use PA3 via the event system in this example on ATtiny parts
  // It has one output, PA5 on ATtiny, or alternate PB6 on 20 and 24-pin ATtiny.

  #if !defined(__ATtinyxy2__)
  #define PIN_TCA_WO0 PIN_PB0
  #else
  #define PIN_TCA_WO0 PIN_PA7
  #endif


  Logic0.enable = true;               // Enable logic block 0

  Logic0.input0 = in::tca0;                      // Use TCA WO0 as input0
  Logic0.input1 = in::tcb1;   // Use TCB1 WO as input 1
  Logic0.input2 = in::masked;   // mask input 2
  //Logic0.output_swap = out::pin_swap; // Uncomment this line to route the output to alternate location, if available.
  Logic0.output = out::enable;        // Enable logic block 0 output pin or PA4 (ATtiny))
  Logic0.filter = filter::disable;    // No output filter enabled
  Logic0.truth = 0x08;                // Set truth table - HIGH only if both high

  // Initialize logic block 0
  Logic0.init();

  // Start the AVR logic hardware
  Logic::start();

  analogWrite(PIN_TCA_WO0, 128); //start TCA0 WO0 running
  TCB0.CTRLA = 0x01; //enabled with CLKPER as clock source
  TCB0.CTRLB = 0x07; //PWM8 mode, but output pin not enabled
  TCB0.CCMPL = 255; //255 counts
  TCB0.CCMPH = 128; //50% duty cycle
}

void loop() {
  // When using configurable custom logic the CPU isn't doing anything!
}
