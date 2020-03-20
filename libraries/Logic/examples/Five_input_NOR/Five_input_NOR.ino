/***********************************************************************|
| megaAVR Configurable Custom Logic library                             |
|                                                                       |
| Five_input_NOR.ino                                                    |
|                                                                       |
| A library for interfacing with the megaAVR Configurable Custom Logic. |
| Developed in 2019 by MCUdude.                                         |
| https://github.com/MCUdude/                                           |
|                                                                       |
| In this example we use two logic blocks to get five inputs.           |
| The output of block 1 is connected to one of the inputs of block 0.   |
| With the correct truth tables values we can make the output of        |
| block 0 go high when all inputs are low.                              |
|                                                                       |
| See Microchip's application note TB3218 for more information.         |
|***********************************************************************/

#include <Logic.h>

void setup()
{
  // Initialize logic block 1
  // Logic block 1 has three inputs, PC0, PC1 and PC2 on ATmega parts
  // Logic block 1 inputs are PC3, PC4, and PC5 on ATtiny parts, but
  // only 24-pin parts have all three. 20-pin parts have PC3 only.
  // Because PA0 is not available on ATtiny under most situations
  // on megaTinyCore, we use input0 of logic block 0 as link from
  // other logic block. On the 20-pin parts, we also use the event system to
  // get the other two inputs from other pins.


  // Here, input2 is taken from the other
  // This example shows how this can be worked around for 20-pin parts
  // It's output is disabled because we connect the output signal to block 0 internally
  Logic1.enable = true;               // Enable logic block 1

  #if defined(MEGATINYCORE) && defined(__AVR_ATtinyxy6__)
  // only use this workaround if it's a 20-pin part.
  // Here, PA3 is used as input 1 and PB0 is used as input 2

  PORTA.DIRCLR=(1<<3);                // set PA3 as input
  PORTA.PIN3CTRL|=PORT_PULLUPEN_bm;   // enable pullup
  EVSYS.ASYNC0=0x0D;                  // PA3 per table 14-2 in datasheet
  EVSYS.ASYNCUSER3= 0x03;             // Use Async0 as LUT1 event 0 per Table 14-4 in datasheet
  Logic0.input1 = in::event_0;        // Use LUT event 0 as input 1, which will take input from PA3

  PORTB.DIRCLR=(1<<0);                // set PB0 as input
  PORTB.PIN0CTRL|=PORT_PULLUPEN_bm;   // enable pullup
  EVSYS.ASYNC1=0x0A;                  // PB0 per table 14-2 in datasheet
  EVSYS.ASYNCUSER5= 0x04;             // Use Async1 as LUT1 event 1 per Table 14-4 in datasheet
  Logic1.input2 = in::event_1;        // Use LUT event 1 as input 2, which will take input from PB0

  #elif defined(MEGATINYCORE) && !defined(__AVR_ATtinyxy7__)
  // If it's neither 20-pin nor 24-pin part, cannot be used.

  #error "The 8-pin and 14-pin parts do not support this"

  #else
  // Otherwise it is an ATmega or ATtiny with 24 pins, and no workaround needed.

  Logic1.input1 = in::input_pullup;   // Set PC1 or PC4 as input with pullup
  Logic1.input2 = in::input_pullup;   // Set PC2 or PC5 input with pullup

  #endif
  //End of workaround code

  Logic1.input0 = in::input_pullup;   // Set PC0 or PC3 as input with pullup
  Logic1.output = out::disable;       // Enable output pin
  Logic1.filter = filter::disable;    // No output filter enabled
  Logic1.truth = 0x01;                // Set truth table

  // Initialize logic block 0
  // Logic block 0 has three inputs, PA0, PA1 and PA2.
  // Block 0 output on PA3 on ATmega, PA5 on ATtiny.

  Logic0.enable = true;               // Enable logic block 0
  Logic0.input0 = in::link;           // Route output from block 1 to this input internally
  Logic0.input1 = in::input_pullup;   // Set PA1 as input with pullup
  Logic0.input2 = in::input_pullup;   // Set PA2 as input with pullup
  Logic0.output = out::enable;        // Enable logic block 0 output pin (PA3 (ATmega) or PA5 (ATtiny))
  Logic0.filter = filter::disable;    // No output filter enabled
  Logic0.truth = 0xFE;                // Set truth table

  // Initialize logic block 0 and 1
  Logic0.init();
  Logic1.init();

  // Start the AVR logic hardware
  Logic::start();
}

void loop()
{
  // When using configurable custom logic the CPU isn't doing anything!
}
