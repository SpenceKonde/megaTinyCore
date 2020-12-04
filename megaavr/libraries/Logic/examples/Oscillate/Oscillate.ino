/***********************************************************************|
  | megaAVR Configurable Custom Logic library                             |
  |                                                                       |
  | Oscillate.ino                                                         |
  |                                                                       |
  | A library for interfacing with the megaAVR Configurable Custom Logic. |
  | Developed in 2019 by MCUdude.                                         |
  | https://github.com/MCUdude/                                           |
  |                                                                       |
  | In this example, we demonstrate the fact that these are ASYNCHRONOUS  |
  | by (ab)using the CCL in a configuration which will oscillate - this   |
  | example uses two logic blocks - though there are plenty of ways to    |
  | make it oscillate with a single one (see if you can come up with some |
  | - assuming you find this fun). If you set values for the filter, you  |
  | get sane output frequencies - just what one would expect from the     |
  | datasheet. But what fun is that?! If you turn them both off, it       |
  | oscillates much faster - faster than the clock speed! with a sawtooth |
  | waveform which (measured by my 'scope) clocks in at 37 MHz - from a   |
  | part running at 20 MHz! Sadly, the amplitude I measure is only 1.8~2.8|
  | volts (the poor output drivers - or my scope - can't keep up)... at   |
  | room temperature! Point a can of freeze spray (computer duster held   |
  | upside-down, so the liquid comes out) and the frequency goes up.      |
  | Aim a heat-gun or torch at it (use care - freeze spray DOES burn, and |
  | you do NOT want to inhale the combustion products. So do PCBs if you  |
  | aren't careful with that torch) and the frequency goes down. This is  |
  | of essentially zero practical use, but it sure is cool isn't it?      |
  |                                                                       |
  |***********************************************************************/

#include <Logic.h>

void setup() {
  // Initialize logic block 0 and 1. Very simple configuration!


  Logic0.enable = true;               // Enable logic block 0
  Logic0.input0 = in::link;           // Use output of block 1
  Logic0.input1 = in::masked;         // masked
  Logic0.input2 = in::masked;         // masked
  Logic0.output = out::enable;        // Enable logic block 0 output pin or PA4 (ATtiny))
  Logic0.filter = filter::disable;    // No output filter enabled
  Logic0.truth = 0x01;                // Set truth table - HIGH only if input low
  // Initialize logic block 0
  Logic0.init();

  Logic1.enable = true;               // Enable logic block 1
  Logic1.input0 = in::link;           // use output of block 0
  Logic1.input1 = in::masked;         // masked
  Logic1.input2 = in::masked;         // masked
  Logic1.output = out::enable;        // enable logic block 1 output pin
  Logic1.filter = filter::disable;    // No output filter enabled
  Logic1.truth = 0x02;                // Set truth table - HIGH only if input HIGH
  // Initialize logic block 0
  Logic1.init();

  // Start the AVR logic hardware
  Logic::start();
}

void loop() {

}
