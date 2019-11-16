/***********************************************************************|
| megaAVR Configurable Custom Logic library                             |
|                                                                       |
| Interrupt.ino                                                         |
|                                                                       |
| A library for interfacing with the megaAVR Configurable Custom Logic. |
| Developed in 2019 by MCUdude.                                         |
| https://github.com/MCUdude/                                           |
|                                                                       |
| In this example we use the configurable logic peripherals the the     |
| megaAVR to create a 3-input NOR gate using logic block 2 on PORT D.   |
| We will use input on PD0, PD1 and PD2. Instead of having an output    |
| pin the logic block will instead trigger an interrupt that runs a     |
| user defined function.                                                |
|                                                                       |
|                                     3-input NOR truth table:          |
| If we look at the truth table       |PD2|PD1|PD0| Y |                 |
| to the right, we can see that       |---|---|---|---|                 |
| all binary values for Y can         | 0 | 0 | 0 | 1 |                 |
| be represented as 00000001.         | 0 | 0 | 1 | 0 |                 |
| If we convert this 8-bit            | 0 | 1 | 0 | 0 |                 |
| binary number into hex, we          | 0 | 1 | 1 | 0 |                 |
| get 0x01.                           | 1 | 0 | 0 | 0 |                 |
|                                     | 1 | 0 | 1 | 0 |                 |
| In this example the output is       | 1 | 1 | 0 | 0 |                 |
| true if all inputs are low.         | 1 | 1 | 1 | 0 |                 |
|                                                                       |
|***********************************************************************/

#include <Logic.h>

void setup()
{
  // Modify the serial port to match your hardware
  Serial.begin(9600);
  
  // Initialize logic block 2
  // Logic block 2 has three inputs, PA0, PA1 and PA2.
  // It has one output, but this is disabled because we're using an interrupt instead.
  Logic2.enable = true;               // Enable logic block 2
  Logic2.input0 = in::input_pullup;   // Set PD0 as input with pullup
  Logic2.input1 = in::input_pullup;   // Set PD1 as input with pullup
  Logic2.input2 = in::input_pullup;   // Set PD2 as input with pullup
  Logic2.output = out::disable;       // Disable output on PD3 (we don't have to though)
  Logic2.filter = filter::disable;    // No output filter enabled
  Logic2.truth = 0x01;                // Set truth table

  // Initialize logic block 2
  Logic2.init();

  // Set interrupt (supports RISING, FALLING and CHANGE)
  Logic2.attachInterrupt(interruptFunction, RISING);

  // Start the AVR logic hardware
  Logic::start();
}

void loop()
{
  // When using configurable custom logic the CPU isn't doing anything!
}

void interruptFunction()
{
  Serial.println("Output of logic block 2 went high!");
}
