/* Digital Pot Control
 * An example of use of the SPI library. There are far better digipots available now with simpler interfaces.
 *
 * This example controls an Analog Devices AD5206 digital potentiometer.
 * The AD5206 has 6 potentiometer channels. Each channel's pins are labeled
 * A - connect this to voltage
 * W - this is the pot's wiper, which changes when you set it
 * B - connect this to ground.
 *
 * The AD5206 is SPI-compatible,and to command it, you send two bytes,
 * one with the channel number (0 - 5) and one with the resistance value for the
 * channel (0 - 255).
 * The circuit:
   * All A pins  of AD5206 connected to +5V
   * All B pins of AD5206 connected to ground
   * An LED and a 220-ohm resisor in series connected from each W pin to ground
   * CS - Any pin. Here we use use #ifdef's to pick the pin - you should replace that with the pin you actually plan to use.
         We just do that because this is used for CI testing, so it needs to compile on **everything**.
   * SDI - MOSI (this can be used as the name of a pin)
   * CLK - SCK (This can be used as the name of a pin)
   * See https://github.com/SpenceKonde/DxCore/blob/master/megaavr/libraries/SPI/README.md for more information
 * Note that this is a bad use of a digipot: they are meant to be used as references, not to power LEDS
 * but it serves as a useful demonstration.
 * created 10 Aug 2010
 * by Tom Igoe
 * Thanks to Heather Dewey-Hagborg for the original tutorial, 2005
 * modified 8/11/2022 to conform with Azduino conventions.
 */
/* Replace this with a simple #define of SSPIN as whatever pin YOU have connected it to - you can use any pin that can be used as a digital output
 * All the pin needs to be is capable of digital output, not used for something else, and present on the part. Since this sketch is used for compile-testing,
 * we pick them so they're always there so that we don't get compile errors for writing to pins not present, not because they're what we'd necessarily use.
 * the only real consideration in pin selection for SS is that you aren't using the pin somewhere else and won't need to use it.
 * And then it's just wiring convienience - so you probably are going to want to use a pin adjacent to the SPI pins or nearby at least.
 */
#if defined(MEGATINYCORE) && (_AVR_PINCOUNT >= 14 || CLOCK_SOURCE == 0)
  #define SSPIN  PIN_PA3
#elif defined(MEGATINYCORE) //with external clock or an 8-pin part we have no choice but to uise PIN_PA6 instead
  #define SSPIN  PIN_PA6
#else // DxCore - Dx or Ex-series
  #define SSPIN   PIN_PC3 //omnipresent on Dx and Ex parts - even the unknown-if-happening DU had that as it's only PORTC IO pin, So this seems a safe choice.
#endif

// inslude the SPI library:
#include <SPI.h>

void setup() {
  // set the slaveSelectPin as an output:
  // Previously this sketch declared a const int instead of using sspin directly. This also illustrates the common practice of using ALLCAPS for #defines
  // (cammelCase for functions and methods, ProperCaps for global variables that are not constant, and lowercase_and_underscore for local variables is the
  // convention used here). Finally, storing pins in an int  is not recommended. The official Arduino core uses ints for pin arguments. uint8_t (byte) is
  // more than sufficient for representing pins on all current and plausible future AVR devices; The most they ever did was like 100 pins and 80-some I/O
  // pins on the ATmega2560 and those of similar pincount and layout. There were a number of complications because of the number of pins, and some of the
  // ports were "second class ports" and had no atomic bit-level operations.
  // And that was still only 80 pins.
  // A uint8_t is what megaTinyCore uses internally, so using that or a macro will maximize efficienct.

  pinMode(SSPIN, OUTPUT);
  // initialize SPI:
  // SPI.swap(...) uncomment and fill in a number if you need to use alternate pins.
  SPI.begin();
}

void loop() {
  // go through the six channels of the digital pot:
  for (int channel = 0; channel < 6; channel++) {
    // change the resistance on this channel from min to max:
    for (int level = 0; level < 255; level++) {
      digitalPotWrite(channel, level);
      delay(10);
    }
    // wait a second at the top:
    delay(1000);
    // change the resistance on this channel from max to min:
    for (int level = 0; level < 255; level++) {
      digitalPotWrite(channel, 255 - level);
      delay(10);
    }
  }

}

void digitalPotWrite(int address, int value) {
  // take the SS pin low to select the chip:
  digitalWrite(SSPIN, LOW);
  //  send in the address and value via SPI:
  SPI.transfer(address);
  SPI.transfer(value);
  // take the SS pin high to de-select the chip:
  digitalWrite(SSPIN, HIGH);
  /* Best Practices note: If the pin is constant and known at compile
   * time, use digitalWriteFast and pinModeFast
   * These have the same syntax, but the pin (and ideally the value)
   * must be constant and known at compile time but if you can get rid of
   * all digitalWrite and pinMode() calls, you're rewarded with several hundred bytes
   * of flash. And the it executes in 1 clock instead of potentially several hundred
   */

}
