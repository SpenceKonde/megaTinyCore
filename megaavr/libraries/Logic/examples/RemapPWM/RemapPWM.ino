/***********************************************************************|
| tinyAVR Configurable Custom Logic library                             |
| RemapPWM.ino                                                          |
|                                                                       |
| A library for interfacing with the megaAVR Configurable Custom Logic. |
| Developed in 2019 by MCUdude.       Example by Spence Konde 2020-2021 |
| https://github.com/MCUdude/            https://github.com/SpenceKonde |
|                                                                       |
| In this example we use the configurable logic peripheral to direct a  |
| PWM Waveform Output (WO) to an arbitrary, different pin.              |
|
| Consider an ATtiny 0, 1, or 2 series, and maximizing the number of    |
| 8-bit PWM channels. On the 14-pin parts, we can get at most 6 from    |
| TCA0, and that's it - the other timer PWM pins are the same as those! |
| TCB0, and (on parts that have it) TCB1 are both sghared with TCA0 pins|
| and so are TCD0 WOA and WOB pins. If we are desperate for another PWM |
| channel (such that we would resort to using a TCB for PW< - they're   |
| lousy for PWM generation - or figure out how to work with TCD0 which  |
| is pretty ugly (actually, since you don't need to enable pin output   |
| TCD is significantly easier to handle here, but still beyond the scope|
| of the example) - you can use the CCL to reroute a PWM signal that    |
| wouldn't have a pin to call it's own onto the logic output pin        |
************************************************************************/

// Make sure this compiles on 8-pin parts for the automated tests...
#if !defined(__AVR_ATtinyxy2__)
  #define PIN_TCA_WO1 PIN_PB1
#else
  #define PIN_TCA_WO1 PIN_PA1
#endif


#include <Logic.h>

void setup() {


  Logic0.enable = true;               // Enable logic block 0
  Logic0.input0 = logic::in::tcb;            // TCB channel - TCB0. On everything except 0/1-series tinyAVR, that's because this is input 0.
  //                                     On those, it's because there's a logic::in::tcb1 option too...
  Logic0.input1 = logic::in::masked;         // mask input 1
  Logic0.input2 = logic::in::masked;         // mask input 2
  Logic0.output = logic::out::enable;        // Enable logic block 0 output pin or PA4 (ATtiny))
  Logic0.filter = logic::filter::disable;    // No output filter enabled
  Logic0.truth = 0x02;                // Set truth table - HIGH if input is HIGH.

  // Initialize logic block 0
  Logic0.init();

  // Start the AVR logic hardware
  Logic::start();

  TCB0.CTRLA = 0x01; // enabled with CLKPER as clock source
  TCB0.CTRLB = 0x07; // PWM8 mode, but output pin not enabled
  TCB0.CCMPL = 255; // 255 counts
  TCB0.CCMPH = 128; // 50% duty cycle
}

void loop() {
  // When using configurable custom logic the CPU isn't doing anything!
}
