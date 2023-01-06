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
   * CS - Any pin. Here we use PA3 as described below
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

// inslude the SPI library:
#include <SPI.h>

// Set PIN_PA7 as the slave select for the digital pot; any pin can be used, but we want it to compile even on 8-pin devices as it is used in automated testing.
const int slaveSelectPin = PIN_PA7;

void setup() {
  // set the slaveSelectPin as an output:
  pinMode(slaveSelectPin, OUTPUT);
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
  digitalWrite(slaveSelectPin, LOW);
  //  send in the address and value via SPI:
  SPI.transfer(address);
  SPI.transfer(value);
  // take the SS pin high to de-select the chip:
  digitalWrite(slaveSelectPin, HIGH);
  /* Best Practices note: If the pin is constant and known at compile
   * time, use digitalWriteFast and pinModeFast
   * These have the same syntax, but the pin (and ideally the value)
   * must be constant and known at compile time but if you can get rid of
   * all digitalWrite and pinMode() calls, you're rewarded with several hundred bytes
   * of flash. And the it executes in 1 clock instead of potentially several hundred
   */

}
