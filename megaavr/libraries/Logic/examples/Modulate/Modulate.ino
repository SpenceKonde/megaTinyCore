/***********************************************************************|
| tinyAVR Configurable Custom Logic library                             |
| Modulate.ino                                                          |
|                                                                       |
| A library for interfacing with the megaAVR Configurable Custom Logic. |
| Developed in 2019 by MCUdude.       Example by Spence Konde 2020-2021 |
| https://github.com/MCUdude/            https://github.com/SpenceKonde |
|                                                                       |
| In this example we use the configurable logic peripherals of the of a |
| modern AVR to achieve the "modulate one timer's PWN with another's"   |
| behavior like some of the more full-featured classic megaAVR parts    |
| We set one input to TCA0 WO1, the other to TCB1 WO, and mask the third|
| and set the truth table so that the output is only HIGH when both are |
| HIGH. You can add some third condition as well, if that was somehow   |
| useful too!                                                           |
| As can be immediately seen, this is vastly more powerful than what the|
| classic AVRs were capable of, where this could be done only with two  |
| timers on a single pin. Now any CCL output pin can be used be made to |
| output any timer channel, modulated by any other. We could even use   |
| the CCL as an event generator to move the output to one of the EVOUT  |
| pins!                                                                 |
| On the Dx-series and 1-series parts, one of the TCD channels might be |
| used, which may be the most useful thing to modulate with another     |
| timer, with it's higher maximum speed and dithering features.         |
|                                                                       |
| It is important to note that it is **not possible to directly port**  |
| code from tinyAVR parts to other product lines in cases where a TCB is|
| used!                                                                 |
| On Dx-series and other non-tiny parts, the CCL logic inpts can come   |
| from TCB0, TCB1, or TCB2 **but this is set by input number** and there|
| is no in::tcb1 or in::tcb2 option!                                    |
| However, on tinyAVR parts thee is either 1 or 2 tcb, and the ones with|
| a TCB1 have a separate in_tcb1 option.                                |
|                                                                       |
| In the crude example below where the timer is configured with the     |
| analogWrite() function both PORTMUX must be pointed at the port that  |
| analgoWrite() is used with AND that the pin used is valid; It turns   |
| out that there is only viable pin on all Dx-series parts and will not |
| be ANY once the DU-series arrives in the 14-pin pincount, so this     |
| example will have to be skipped or adapted there. Otherwise we can    |
| pick PC2. PA0 or PA1 will not compile if a crystal clock is used; we  |
| do not permit use of invalid pins, as (except for crude tricks like   |
| this that are never used in "real life") there is never a right time  |
| to refer to them.                                                     |
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
  Logic0.input1 = logic::in::tca0;           // Use TCA0 WO1 as input0
  Logic0.input2 = logic::in::masked;         // mask input 2
  Logic0.output = logic::out::enable;        // Enable logic block 0 output pin or PA4 (ATtiny))
  Logic0.filter = logic::filter::disable;    // No output filter enabled
  Logic0.truth = 0x08;                // Set truth table - HIGH only if both high

  // Initialize logic block 0
  Logic0.init();

  // Start the AVR logic hardware
  Logic::start();

  analogWrite(PIN_TCA_WO1, 128); // start TCA0 WO0 running
  TCB0.CTRLA = 0x01; // enabled with CLKPER as clock source
  TCB0.CTRLB = 0x07; // PWM8 mode, but output pin not enabled
  TCB0.CCMPL = 255; // 255 counts
  TCB0.CCMPH = 128; // 50% duty cycle
}

void loop() {
  // When using configurable custom logic the CPU isn't doing anything!
}
