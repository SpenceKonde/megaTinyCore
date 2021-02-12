/***********************************************************************|
  | megaAVR Configurable Custom Logic library                             |
  |                                                                       |
  | Oscillate.ino                                                         |
  |                                                                       |
  | A library for interfacing with the megaAVR Configurable Custom Logic. |
  | Developed in 2019 by MCUdude. https://github.com/MCUdude/             |
  | Example by Spence Konde, 2020 https://github.com/SpenceKonde/         |
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
  | part running at 20 MHz! Different configurations will result in       |
  | different frequencies. The simplest one can oscillate at a whopping   |
  | 92 MHz!                                                               |
  |                                                                       |
  | It is temperature dependenr - point a can of freeze spray (computer   |
  | duster held upside-down, so the liquid comes out) and the frequency   |
  | goes up. Aim a heat-gun or torch at it (use care - freeze spray DOES  |
  | burn, and you do NOT want to inhale the combustion products or cause  |
  | an inferno in your lab.) and the frequency goes down. Warnings:       |
  | Hitting hot parts with freeze spray may crack the package, ruining the|
  | part, and freeze spray will cause frost to form on the parts. Best to |
  | disconnect them from power while they thaw and dry off before applying|
  | power to them again. PCBs also burn if you are not careful with that  |
  | torch) This is maximum-speed oscillation is of essentially zero       |
  | practical use, but it sure is cool isn't it?                          |
  |                                                                       |
  | Maybe it could be used to examine a new silicon revision to see if    |
  | there were any significant process changes? *shrug*                   |
  |                                                                       |
  | And yes, if you happen to want your part to run at an indeterminate   |
  | and highly temperature dependent speed, you could then set the system |
  | clock prescaler to 2, connect a jumper between the Logic0 output pin  |
  | and EXTCLK pin, and switch to the external clock, leading to it       |
  | running at around 18.5 MHz with wide variation depending on conditions|
  | This is not recommended except as a silly joke, and it is thoroughly  |
  | useless. Maybe if you miss the +/- 10% tolerance on the classic AVR   |
  | internal oscillator?                                                  |
  |                                                                       |
  | In combination with the synchronizer/filter, though, it is has the    |
  | potential to be far more useful - as it will allow generation of a    |
  | prescaled system clock. On parts where a TCB can be clocked from an   |
  | event, this allows one to work around the limited prescale options    |
  | available for TCBs, without having to change the TCA prescaler. For   |
  | more information on this, see: https://github.com/SpenceKonde/        |
  | AVR-Guidance/blob/master/CCL_EVSYS_hacks/CCL_prescaling.md            |
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
  Logic1.input0 = in::feedback;       // use output of block 0 - link works on 0/1-series parts, but not on 2-series ones.
  Logic1.input1 = in::masked;         // masked
  Logic1.input2 = in::masked;         // masked
  Logic1.output = out::enable;        // enable logic block 1 output pin - required on some parts for link to work, see errata.
  Logic1.filter = filter::disable;    // No output filter enabled
  Logic1.truth = 0x02;                // Set truth table - HIGH only if input HIGH
  // Initialize logic block 1
  Logic1.init();

  // Start the AVR logic hardware
  Logic::start();
}

void loop() {

}
