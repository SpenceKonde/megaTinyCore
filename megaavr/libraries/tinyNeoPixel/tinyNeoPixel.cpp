/*-------------------------------------------------------------------------
  Arduino library to control a wide variety of WS2811- and WS2812-based RGB
  LED devices such as Adafruit FLORA RGB Smart Pixels and NeoPixel strips.

  Currently handles 800 KHz bitstreams on 8, 10, 12, 16, and 20 MHz ATtiny
  MCUs with ATTinyCore 1.30+, 8, 10, 12, 16, 20, 24, 28, and 32 MHz AVRxt
  tinyAVR 0/1/2-series parts with megaTinyCore 1.0.3+ and those speeds
  plus the ridiculously overclocked 36, 40, 44, and 48 MHz speeds with
  AVR Dx-series parts. Note that the highest speeds have not been tested
  and it would be surprising if the parts could be pushed that far.

  Like the Adafruit original version, it supports LEDs wired
  for various color orders. 400 kHz support was never included.
  Nobody has ever asked about it, nor have I seen any 400 kHz LEDs for sale.
  Ever.

  Written by Phil Burgess / Paint Your Dragon for Adafruit Industries,
  contributions by PJRC, Michael Miller and other members of the open
  source community.

  Extensive porting to additional parts and different clock speeds by
  Spence Konde.

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing products
  from Adafruit!

  Same goes for Spence too!

  -------------------------------------------------------------------------
  This file is part of the tinyNeoPixel library derived from
  Adafruit_NeoPixel.

  NeoPixel is free software: you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as
  published by the Free Software Foundation, either version 3 of
  the License, or (at your option) any later version.

  NeoPixel is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with NeoPixel.  If not, see
  <http://www.gnu.org/licenses/>.
  -------------------------------------------------------------------------*/

#include "tinyNeoPixel.h"

// Constructor when length, pin and type are known at compile-time:
tinyNeoPixel::tinyNeoPixel(uint16_t n, uint8_t p, neoPixelType t) :
  begun(false), brightness(0), pixels(NULL), endTime(0) {
  updateType(t);
  updateLength(n);
  setPin(p);
}

// via Michael Vogt/neophob: empty constructor is used when strand length
// isn't known at compile-time; situations where program config might be
// read from internal flash memory or an SD card, or arrive via serial
// command.  If using this constructor, MUST follow up with updateType(),
// updateLength(), etc. to establish the strand type, length and pin number!
tinyNeoPixel::tinyNeoPixel() :
  begun(false), numLEDs(0), numBytes(0), pin(NOT_A_PIN), brightness(0), pixels(NULL),
  rOffset(1), gOffset(0), bOffset(2), wOffset(1), endTime(0) {
}

tinyNeoPixel::~tinyNeoPixel() {
  if (pixels) {
    free(pixels);
  }
  pinMode(pin, INPUT);
}

void tinyNeoPixel::begin(void) {
  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW);
  begun = true;
}

void tinyNeoPixel::updateLength(uint16_t n) {
  if (pixels) {
    free(pixels);  // Free existing data (if any)
  }

  // Allocate new data -- note: ALL PIXELS ARE CLEARED
  numBytes = n * ((wOffset == rOffset) ? 3 : 4);
  if ((pixels = (uint8_t *)malloc(numBytes))) {
    memset(pixels, 0, numBytes);
    numLEDs = n;
  } else {
    numLEDs = numBytes = 0;
  }
}

void tinyNeoPixel::updateType(neoPixelType t) {
  boolean oldThreeBytesPerPixel = (wOffset == rOffset); // false if RGBW

  wOffset = (t >> 6) & 0b11; // See notes in header file
  rOffset = (t >> 4) & 0b11; // regarding R/G/B/W offsets
  gOffset = (t >> 2) & 0b11;
  bOffset =  t       & 0b11;

  // If bytes-per-pixel has changed (and pixel data was previously
  // allocated), re-allocate to new size.  Will clear any data.
  if (pixels) {
    boolean newThreeBytesPerPixel = (wOffset == rOffset);
    if (newThreeBytesPerPixel != oldThreeBytesPerPixel) {
      updateLength(numLEDs);
    }
  }
}

// *INDENT-OFF*   astyle don't like assembly
void tinyNeoPixel::show(void) {

  if ((!pixels) || pin >= NUM_DIGITAL_PINS)  {
    return;
  }

  // Data latch = 50+ microsecond pause in the output stream.  Rather than
  // put a delay at the end of the function, the ending time is noted and
  // the function will simply hold off (if needed) on issuing the
  // subsequent round of data until the latch time has elapsed.  This
  // allows the mainline code to start generating the next frame of data
  // rather than stalling for the latch.
  while (!canShow());
  // endTime is a private member (rather than global var) so that multiple
  // instances on different pins can be quickly issued in succession (each
  // instance doesn't delay the next).

  // In order to make this code runtime-configurable to work with any pin,
  // SBI/CBI instructions are eschewed in favor of full PORT writes via the
  // OUT or ST instructions.  It relies on two facts: that peripheral
  // functions (such as PWM) take precedence on output pins, so our PORT-
  // wide writes won't interfere, and that interrupts are globally disabled
  // while data is being issued to the LEDs, so no other code will be
  // accessing the PORT.  The code takes an initial 'snapshot' of the PORT
  // state, computes 'pin high' and 'pin low' values, and writes these back
  // to the PORT register as needed.
  // Notes (Spence 2/2022):
  //   OUTs suck, because they force duplication of the routine to handle
  //   different ports. Luckily we don't need them here - modern AVs are
  //   based on the AVRxt core, and ST takes only 1 clock.
  //   Actually, it is possible to do that on the classic AVR version of
  //   these routines too... I'm not sure why it wasn't done, other than
  //   a strict adherence to the official datasheet, unhindered by any
  //   actual testing of how the parts behave (a LOW of only 6 us is
  //   enough to trigger the latch, that is the only limit on the length
  //   of a LOW, (hence if any bit takes longer than 5 bits are supposed
  //   to, it won't work :-P)), and the length of a 1 is limited by the
  //   fact that it will get "signal reshaped" an "ideal length". If this
  //   results in the following LOW exceeding the latch limit, it breaks.
  //
  //   So, having timing that's not strictly speaking correct located in
  //   anywhere that won't make a 0 look like a 1 or the other way around,
  //   you can kind of get away with a lot.

  //   This code manages to avoid having having to do that all the way down
  //   to 4-5 MHz one. which takes 1400 us to run for 6 bits and 1600 for the
  //   last two.
  //
  //   Over the years a bunch of faster speeds have been added, so it could
  //   be used even on highly overclocked modern AVFs.
  //
  //   Finally, some of the constraints were incorrect, and this only ever
  //   worked because classes are kryptonite to the optimizer.
  //   * [ptr] was decleared read only. No, it is not. The register is
  //     register contaains the address being pointed to. We read with
  //     postincrement, so this is read-write.
  //   * Comversely, [port] is never written. The thing that port writes
  //     *too* is changd, but [port] is not.
  //   * b (bit number for speeds that don't have to unroll the loop)
  //     is given constraint "+r", which can assign it to any register.
  //     But the code uses LDI on it. LDI doesn't work on every register,
  //     it must have the "+d" constraint to guarantee an upper register.

  noInterrupts(); // Need 100% focus on instruction timing


  // AVRxt MCUs --  tinyAVR 0/1/2, megaAVR 0, AVR Dx ----------------------
  // with extended maximum speeds to support vigorously overclocked
  // Dx-series parts. This is by no means intended to imply that they will
  // run at those speeds, only that - if they do - you can control WS2812s
  // with them.

  volatile uint16_t
    i   = numBytes; // Loop counter
  volatile uint8_t
   *ptr = pixels,   // Pointer to next byte
    b   = *ptr++,   // Current byte value
    hi,             // PORT w/output bit set high
    lo;             // PORT w/output bit set low

  // Hand-tuned assembly code issues data to the LED drivers at a specific
  // rate.  There's separate code for different CPU speeds (8, 12, 16 MHz)
  // for both the WS2811 (400 KHz) and WS2812 (800 KHz) drivers.  The
  // datastream timing for the LED drivers allows a little wiggle room each
  // way (listed in the datasheets), so the conditions for compiling each
  // case are set up for a range of frequencies rather than just the exact
  // 8, 12 or 16 MHz values, permitting use with some close-but-not-spot-on
  // devices (e.g. 16.5 MHz DigiSpark).  The ranges were arrived at based
  // on the datasheet figures and have not been extensively tested outside
  // the canonical 8/12/16 MHz speeds; there's no guarantee these will work
  // close to the extremes (or possibly they could be pushed further).
  // Keep in mind only one CPU speed case actually gets compiled; the
  // resulting program isn't as massive as it might look from source here.


  // 5ish MHz(ish) AVRxt
  #if (F_CPU >= 400000UL) && (F_CPU <= 5600000UL)

    volatile uint8_t n1, n2 = 0;  // First, next bits out

    // At this dreadfully slow clock speed, we struggle to meet the constraints.
    // The best I can come up with is this. At 5 MHz, the bits are 1.4 us lonng.
    // The last two bits are 1.6 us long.
    // and if you were to crank it back to 4 MHz, well... it might actually still just barely work.
    // The ones would be way longser

    // 7 instruction clocks per bit: HxxxxLL
    // OUT instructions:             ^^   ^  (T=0,2,5)
    // 200 ns zero bit - barely legal.
    // 800 ns  highs    - legal.
    // 400 ns lows minimum
    //

    hi   = *port |  pinMask;
    lo   = *port & ~pinMask;
    n1 = lo;
    if (b & 0x80) n1 = hi;

    // Dirty trick: RJMPs proceeding to the next instruction are used
    // to delay two clock cycles in one instruction word (rather than
    // using two NOPs).  This was necessary in order to squeeze the
    // loop down to exactly 64 words -- the maximum possible for a
    // relative branch.

    asm volatile(
     "headD:"                   "\n\t" // Clk  Pseudocode
      // Bit 7:
      "st   %a[port], %[hi]"    "\n\t" // 1    PORT = hi
      "st   %a[port], %[n1]"    "\n\t" // 1    PORT = n1
      "mov  %[n2]   , %[lo]"    "\n\t" // 1    n2   = lo
      "sbrc %[byte] , 6"        "\n\t" // 1-2  if (b & 0x40)
       "mov %[n2]   , %[hi]"    "\n\t" // 0-1   n2 = hi
      "st   %a[port], %[lo]"    "\n\t" // 1    PORT = lo
      "nop"                     "\n\t" // 1    nop total 7 clocks
      // Bit 6:
      "st   %a[port], %[hi]"    "\n\t" // 1    PORT = hi
      "st   %a[port], %[n2]"    "\n\t" // 1    PORT = n2
      "mov  %[n1]   , %[lo]"    "\n\t" // 1    n1   = lo
      "sbrc %[byte] , 5"        "\n\t" // 1-2  if (b & 0x20)
       "mov %[n1]   , %[hi]"    "\n\t" // 0-1   n1 = hi
      "st   %a[port], %[lo]"    "\n\t" // 1    PORT = lo
      "nop"                     "\n\t" // 1    nop total 7 clocks
      // Bit 5:
      "st   %a[port], %[hi]"    "\n\t" // 1    PORT = hi
      "st   %a[port], %[n1]"    "\n\t" // 1    PORT = n1
      "mov  %[n2]   , %[lo]"    "\n\t" // 1    n2   = lo
      "sbrc %[byte] , 4"        "\n\t" // 1-2  if (b & 0x10)
       "mov %[n2]   , %[hi]"    "\n\t" // 0-1   n2 = hi
      "st   %a[port], %[lo]"    "\n\t" // 1    PORT = lo
      "nop"                     "\n\t" // 1    nop total 7 clocks
      // Bit 4:
      "st   %a[port], %[hi]"    "\n\t" // 1    PORT = hi
      "st   %a[port], %[n2]"    "\n\t" // 1    PORT = n2
      "mov  %[n1]   , %[lo]"    "\n\t" // 1    n1   = lo
      "sbrc %[byte] , 3"        "\n\t" // 1-2  if (b & 0x08)
       "mov %[n1]   , %[hi]"    "\n\t" // 0-1   n1 = hi
      "st   %a[port], %[lo]"    "\n\t" // 1    PORT = lo
      "nop"                     "\n\t" // 1    nop total 7 clocks
      // Bit 3:
      "st   %a[port], %[hi]"    "\n\t" // 1    PORT = hi
      "st   %a[port], %[n1]"    "\n\t" // 1    PORT = n1
      "mov  %[n2]   , %[lo]"    "\n\t" // 1    n2   = lo
      "sbrc %[byte] , 2"        "\n\t" // 1-2  if (b & 0x04)
       "mov %[n2]   , %[hi]"    "\n\t" // 0-1   n2 = hi
      "st   %a[port], %[lo]"    "\n\t" // 1    PORT = lo
      "subi %A[count], 1"       "\n\t" // 1    Replacing sbiw pt 1 total 7 clocks
      // Bit 2, 1 clock over:
      "st   %a[port], %[hi]"    "\n\t" // 1    PORT = hi
      "st   %a[port], %[n2]"    "\n\t" // 1    PORT = n2
      "mov  %[n1]   , %[lo]"    "\n\t" // 1    n1   = lo
      "sbrc %[byte] , 1"        "\n\t" // 1-2  if (b & 0x02)
       "mov %[n1]   , %[hi]"    "\n\t" // 0-1   n1 = hi
      "st   %a[port], %[lo]"    "\n\t" // 1    PORT = lo
      "sbc  %B[count], r1"      "\n\t" // 2    Replacing sbiw pt 2 - total 7 clocks  clocks
      // Bit 1, 1 clock over:
      "st   %a[port], %[hi]"    "\n\t" // 1    PORT = hi
      "st   %a[port], %[n1]"    "\n\t" // 1    PORT = n1
      "mov  %[n2]   , %[lo]"    "\n\t" // 1    n2   = lo
      "sbrc %[byte] , 0"        "\n\t" // 1-2  if (b & 0x01)
       "mov %[n2]   , %[hi]"    "\n\t" // 0-1   n2 = hi
      "st   %a[port], %[lo]"    "\n\t" // 1    PORT = lo
      "ld   %[byte] , %a[ptr]+" "\n\t" // 2    b = *ptr++  total 8 clocks
      // Bit 0, 1 ckicj over:
      "st   %a[port], %[hi]"    "\n\t" // 1    PORT = hi
      "st   %a[port], %[n2]"    "\n\t" // 1    PORT = n2
      "mov  %[n1]   , %[lo]"    "\n\t" // 1    n1   = lo
      "sbrc %[byte] , 7"        "\n\t" // 1-2  if (b & 0x80)
       "mov %[n1]   , %[hi]"    "\n\t" // 0-1   n1 = hi
      "st   %a[port], %[lo]"    "\n\t" // 1    PORT = lo
      "brne headD"              "\n"   // 2    while(i) (Z flag set above) total 8 clocks.
    : [ptr]   "+e" (ptr),
      [byte]  "+r" (b),
      [n1]    "+r" (n1),
      [n2]    "+r" (n2),
      [count] "+d" (i)     /* Relax requirements - it only needs to be an upper register, because we can't use SBIW, because we need to split the subtraction between two bits to meet timing constraints) */
    : [port]   "e" (port),
      [hi]     "r" (hi),
      [lo]     "r" (lo));



  // 6.4ish MHz(ish) AVRxt
  #elif (F_CPU >= 5600000UL) && (F_CPU <= 7400000UL)

    volatile uint8_t n1, n2 = 0;  // First, next bits out

    // We need to be able to write to the port register in one clock
    // to meet timing constraints here.

    // 8 instruction clocks per bit: HHxxxLLL
    // OUT instructions:             ^ ^  ^  (T=0,2,5)
    // 333 ns zero bit - definitely legal.
    // 833 ns highs    - legal.
    // 500 ns lows  - legal
    // 8 clock per bit target met with NO DEVIANT BITS!

    hi   = *port |  pinMask;
    lo   = *port & ~pinMask;
    n1 = lo;
    if (b & 0x80) n1 = hi;

    // Dirty trick: RJMPs proceeding to the next instruction are used
    // to delay two clock cycles in one instruction word (rather than
    // using two NOPs).  This was necessary in order to squeeze the
    // loop down to exactly 64 words -- the maximum possible for a
    // relative branch.

    asm volatile(
     "headD:"                   "\n\t" // Clk  Pseudocode
      // Bit 7:
      "st   %a[port], %[hi]"    "\n\t" // 1    PORT = hi
      "mov  %[n2]   , %[lo]"    "\n\t" // 1    n2   = lo
      "st   %a[port], %[n1]"    "\n\t" // 1    PORT = n1
      "sbrc %[byte] , 6"        "\n\t" // 1-2  if (b & 0x40)
       "mov %[n2]   , %[hi]"    "\n\t" // 0-1   n2 = hi
      "st   %a[port], %[lo]"    "\n\t" // 1    PORT = lo
      "rjmp .+0"                "\n\t" // 2    nop nop total 8 clocks
      // Bit 6:
      "st   %a[port], %[hi]"    "\n\t" // 1    PORT = hi
      "mov  %[n1]   , %[lo]"    "\n\t" // 1    n1   = lo
      "st   %a[port], %[n2]"    "\n\t" // 1    PORT = n2
      "sbrc %[byte] , 5"        "\n\t" // 1-2  if (b & 0x20)
       "mov %[n1]   , %[hi]"    "\n\t" // 0-1   n1 = hi
      "st   %a[port], %[lo]"    "\n\t" // 1    PORT = lo
      "rjmp .+0"                "\n\t" // 2    nop nop total 8 clocks
      // Bit 5:
      "st   %a[port], %[hi]"    "\n\t" // 1    PORT = hi
      "mov  %[n2]   , %[lo]"    "\n\t" // 1    n2   = lo
      "st   %a[port], %[n1]"    "\n\t" // 1    PORT = n1
      "sbrc %[byte] , 4"        "\n\t" // 1-2  if (b & 0x10)
       "mov %[n2]   , %[hi]"    "\n\t" // 0-1   n2 = hi
      "st   %a[port], %[lo]"    "\n\t" // 1    PORT = lo
      "rjmp .+0"                "\n\t" // 2    nop nop total 8 clocks
      // Bit 4:
      "st   %a[port], %[hi]"    "\n\t" // 1    PORT = hi
      "mov  %[n1]   , %[lo]"    "\n\t" // 1    n1   = lo
      "st   %a[port], %[n2]"    "\n\t" // 1    PORT = n2
      "sbrc %[byte] , 3"        "\n\t" // 1-2  if (b & 0x08)
       "mov %[n1]   , %[hi]"    "\n\t" // 0-1   n1 = hi
      "st   %a[port], %[lo]"    "\n\t" // 1    PORT = lo
      "rjmp .+0"                "\n\t" // 2    nop nop total 8 clocks
      // Bit 3:
      "st   %a[port], %[hi]"    "\n\t" // 1    PORT = hi
      "mov  %[n2]   , %[lo]"    "\n\t" // 1    n2   = lo
      "st   %a[port], %[n1]"    "\n\t" // 1    PORT = n1
      "sbrc %[byte] , 2"        "\n\t" // 1-2  if (b & 0x04)
       "mov %[n2]   , %[hi]"    "\n\t" // 0-1   n2 = hi
      "st   %a[port], %[lo]"    "\n\t" // 1    PORT = lo
      "rjmp .+0"                "\n\t" // 2    nop nop total 8 clocks
      // Bit 2:
      "st   %a[port], %[hi]"    "\n\t" // 1    PORT = hi
      "mov  %[n1]   , %[lo]"    "\n\t" // 1    n1   = lo
      "st   %a[port], %[n2]"    "\n\t" // 1    PORT = n2
      "sbrc %[byte] , 1"        "\n\t" // 1-2  if (b & 0x02)
       "mov %[n1]   , %[hi]"    "\n\t" // 0-1   n1 = hi
      "st   %a[port], %[lo]"    "\n\t" // 1    PORT = lo
      "sbiw %[count], 1"        "\n\t" // 2    i-- (don't act on Z flag yet) total 8 clocks
      // Bit 1:
      "st   %a[port], %[hi]"    "\n\t" // 1    PORT = hi
      "mov  %[n2]   , %[lo]"    "\n\t" // 1    n2   = lo
      "st   %a[port], %[n1]"    "\n\t" // 1    PORT = n1
      "sbrc %[byte] , 0"        "\n\t" // 1-2  if (b & 0x01)
       "mov %[n2]   , %[hi]"    "\n\t" // 0-1   n2 = hi
      "st   %a[port], %[lo]"    "\n\t" // 1    PORT = lo
      "ld   %[byte] , %a[ptr]+" "\n\t" // 2    b = *ptr++  total 8 clocks
      // Bit 0:
      "st   %a[port], %[hi]"    "\n\t" // 1    PORT = hi
      "mov  %[n1]   , %[lo]"    "\n\t" // 1    n1   = lo
      "st   %a[port], %[n2]"    "\n\t" // 1    PORT = n2
      "sbrc %[byte] , 7"        "\n\t" // 1-2  if (b & 0x80)
       "mov %[n1]   , %[hi]"    "\n\t" // 0-1   n1 = hi
      "st   %a[port], %[lo]"    "\n\t" // 1    PORT = lo
      "brne headD"              "\n"   // 2    while(i) (Z flag set above)
    : [ptr]   "+e" (ptr),
      [byte]  "+r" (b),
      [n1]    "+r" (n1),
      [n2]    "+r" (n2),
      [count] "+w" (i)
    : [port]   "e" (port),
      [hi]     "r" (hi),
      [lo]     "r" (lo));



  // 8 MHz(ish) AVRxt ---------------------------------------------------------
  #elif (F_CPU >= 7400000UL) && (F_CPU <= 9500000UL)

    volatile uint8_t n1, n2 = 0;  // First, next bits out

    // We need to be able to write to the port register in one clock
    // to meet timing constraints here.

    // 10 instruction clocks per bit: HHxxxxxLLL
    // OUT instructions:              ^ ^    ^   (T=0,2,7)

    hi   = *port |  pinMask;
    lo   = *port & ~pinMask;
    n1 = lo;
    if (b & 0x80) n1 = hi;

    // Dirty trick: RJMPs proceeding to the next instruction are used
    // to delay two clock cycles in one instruction word (rather than
    // using two NOPs).  This was necessary in order to squeeze the
    // loop down to exactly 64 words -- the maximum possible for a
    // relative branch.

    asm volatile(
     "headD:"                   "\n\t" // Clk  Pseudocode
      // Bit 7:
      "st   %a[port], %[hi]"    "\n\t" // 1    PORT = hi
      "mov  %[n2]   , %[lo]"    "\n\t" // 1    n2   = lo
      "st   %a[port], %[n1]"    "\n\t" // 1    PORT = n1
      "rjmp .+0"                "\n\t" // 2    nop nop
      "sbrc %[byte] , 6"        "\n\t" // 1-2  if (b & 0x40)
       "mov %[n2]   , %[hi]"    "\n\t" // 0-1   n2 = hi
      "st   %a[port], %[lo]"    "\n\t" // 1    PORT = lo
      "rjmp .+0"                "\n\t" // 2    nop nop
      // Bit 6:
      "st   %a[port], %[hi]"    "\n\t" // 1    PORT = hi
      "mov  %[n1]   , %[lo]"    "\n\t" // 1    n1   = lo
      "st   %a[port], %[n2]"    "\n\t" // 1    PORT = n2
      "rjmp .+0"                "\n\t" // 2    nop nop
      "sbrc %[byte] , 5"        "\n\t" // 1-2  if (b & 0x20)
       "mov %[n1]   , %[hi]"    "\n\t" // 0-1   n1 = hi
      "st   %a[port], %[lo]"    "\n\t" // 1    PORT = lo
      "rjmp .+0"                "\n\t" // 2    nop nop
      // Bit 5:
      "st   %a[port], %[hi]"    "\n\t" // 1    PORT = hi
      "mov  %[n2]   , %[lo]"    "\n\t" // 1    n2   = lo
      "st   %a[port], %[n1]"    "\n\t" // 1    PORT = n1
      "rjmp .+0"                "\n\t" // 2    nop nop
      "sbrc %[byte] , 4"        "\n\t" // 1-2  if (b & 0x10)
       "mov %[n2]   , %[hi]"    "\n\t" // 0-1   n2 = hi
      "st   %a[port], %[lo]"    "\n\t" // 1    PORT = lo
      "rjmp .+0"                "\n\t" // 2    nop nop
      // Bit 4:
      "st   %a[port], %[hi]"    "\n\t" // 1    PORT = hi
      "mov  %[n1]   , %[lo]"    "\n\t" // 1    n1   = lo
      "st   %a[port], %[n2]"    "\n\t" // 1    PORT = n2
      "rjmp .+0"                "\n\t" // 2    nop nop
      "sbrc %[byte] , 3"        "\n\t" // 1-2  if (b & 0x08)
       "mov %[n1]   , %[hi]"    "\n\t" // 0-1   n1 = hi
      "st   %a[port], %[lo]"    "\n\t" // 1    PORT = lo
      "rjmp .+0"                "\n\t" // 2    nop nop
      // Bit 3:
      "st   %a[port], %[hi]"    "\n\t" // 1    PORT = hi
      "mov  %[n2]   , %[lo]"    "\n\t" // 1    n2   = lo
      "st   %a[port], %[n1]"    "\n\t" // 1    PORT = n1
      "rjmp .+0"                "\n\t" // 2    nop nop
      "sbrc %[byte] , 2"        "\n\t" // 1-2  if (b & 0x04)
       "mov %[n2]   , %[hi]"    "\n\t" // 0-1   n2 = hi
      "st   %a[port], %[lo]"    "\n\t" // 1    PORT = lo
      "rjmp .+0"                "\n\t" // 2    nop nop
      // Bit 2:
      "st   %a[port], %[hi]"    "\n\t" // 1    PORT = hi
      "mov  %[n1]   , %[lo]"    "\n\t" // 1    n1   = lo
      "st   %a[port], %[n2]"    "\n\t" // 1    PORT = n2
      "rjmp .+0"                "\n\t" // 2    nop nop
      "sbrc %[byte] , 1"        "\n\t" // 1-2  if (b & 0x02)
       "mov %[n1]   , %[hi]"    "\n\t" // 0-1   n1 = hi
      "st   %a[port], %[lo]"    "\n\t" // 1    PORT = lo
      "rjmp .+0"                "\n\t" // 2    nop nop
      // Bit 1:
      "st   %a[port], %[hi]"    "\n\t" // 1    PORT = hi
      "mov  %[n2]   , %[lo]"    "\n\t" // 1    n2   = lo
      "st   %a[port], %[n1]"    "\n\t" // 1    PORT = n1
      "rjmp .+0"                "\n\t" // 2    nop nop
      "sbrc %[byte] , 0"        "\n\t" // 1-2  if (b & 0x01)
       "mov %[n2]   , %[hi]"    "\n\t" // 0-1   n2 = hi
      "st   %a[port], %[lo]"    "\n\t" // 1    PORT = lo
      "sbiw %[count], 1"        "\n\t" // 2    i-- (don't act on Z flag yet)
      // Bit 0:
      "st   %a[port], %[hi]"    "\n\t" // 1    PORT = hi
      "mov  %[n1]   , %[lo]"    "\n\t" // 1    n1   = lo
      "st   %a[port], %[n2]"    "\n\t" // 1    PORT = n2
      "ld   %[byte] , %a[ptr]+" "\n\t" // 2    b = *ptr++
      "sbrc %[byte] , 7"        "\n\t" // 1-2  if (b & 0x80)
       "mov %[n1]   , %[hi]"    "\n\t" // 0-1   n1 = hi
      "st   %a[port], %[lo]"    "\n\t" // 1    PORT = lo
      "brne headD"              "\n"   // 2    while(i) (Z flag set above)
    : [ptr]   "+e" (ptr),
      [byte]  "+r" (b),
      [n1]    "+r" (n1),
      [n2]    "+r" (n2),
      [count] "+w" (i)
    : [port]   "e" (port),
      [hi]     "r" (hi),
      [lo]     "r" (lo));

  #elif (F_CPU >= 9500000UL) && (F_CPU <= 11100000UL)
    /*
    volatile uint8_t n1, n2 = 0;  // First, next bits out

    */
    // 14 instruction clocks per bit: HHHHxxxxLLLLL
    // ST instructions:               ^   ^   ^   (T=0,4,7)
    volatile uint8_t next;

    hi   = *port |  pinMask;
    lo   = *port & ~pinMask;
    next = lo;
    if (b & 0x80) {
      next = hi;
    }

    // Don't "optimize" the OUT calls into the bitTime subroutine;
    // we're exploiting the RCALL and RET as 3- and 4-cycle NOPs!
    asm volatile(
     "headD:"                   "\n\t" //        (T =  0)
      "st   %a[port], %[hi]"    "\n\t" //        (T =  1)
      "rcall bitTimeD"          "\n\t" // Bit 7  (T = 14)
      "st   %a[port], %[hi]"    "\n\t"
      "rcall bitTimeD"          "\n\t" // Bit 6
      "st   %a[port], %[hi]"    "\n\t"
      "rcall bitTimeD"          "\n\t" // Bit 5
      "st   %a[port], %[hi]"    "\n\t"
      "rcall bitTimeD"          "\n\t" // Bit 4
      "st   %a[port], %[hi]"    "\n\t"
      "rcall bitTimeD"          "\n\t" // Bit 3
      "st   %a[port], %[hi]"    "\n\t"
      "rcall bitTimeD"          "\n\t" // Bit 2
      "st   %a[port], %[hi]"    "\n\t"
      "rcall bitTimeD"          "\n\t" // Bit 1
      // Bit 0:
      "st   %a[port], %[hi]"    "\n\t" // 1    PORT = hi    (T =  1)
      "rjmp .+0"                "\n\t" // 2    nop nop      (T =  3)
      "ld   %[byte] , %a[ptr]+" "\n\t" // 2    b = *ptr++   (T =  5)
      "st   %a[port], %[next]"  "\n\t" // 1    PORT = next  (T =  6)
      "mov  %[next] , %[lo]"    "\n\t" // 1    next = lo    (T =  7)
      "sbrc %[byte] , 7"        "\n\t" // 1-2  if (b & 0x80) (T =  8)
       "mov %[next] , %[hi]"    "\n\t" // 0-1    next = hi  (T =  9)
      "st   %a[port], %[lo]"    "\n\t" // 1    PORT = lo    (T = 10)
      "sbiw %[count], 1"        "\n\t" // 2    i--          (T = 12)
      "brne headD"              "\n\t" // 2    if (i != 0) -> (next byte) (T = 14)
       "rjmp doneD"             "\n\t"
      "bitTimeD:"               "\n\t" //      nop nop nop     (T =  4)
       "st   %a[port], %[next]" "\n\t" // 1    PORT = next     (T =  5)
       "mov  %[next], %[lo]"    "\n\t" // 1    next = lo       (T =  6)
       "rol  %[byte]"           "\n\t" // 1    b <<= 1         (T =  7)
       "sbrc %[byte], 7"        "\n\t" // 1-2  if (b & 0x80)    (T =  8)
        "mov %[next], %[hi]"    "\n\t" // 0-1   next = hi      (T =  9)
       "st   %a[port], %[lo]"   "\n\t" // 1    PORT = lo       (T = 10)
       "ret"                    "\n\t" // 4    nop nop nop nop (T = 14)
       "doneD:"                 "\n"
    : [ptr]   "+e" (ptr),
      [byte]  "+r" (b),
      [next]  "+r" (next),
      [count] "+w" (i)
    : [port]   "e" (port),
      [hi]     "r" (hi),
      [lo]     "r" (lo));



// 12 MHz(ish) AVRxt --------------------------------------------------------
#elif (F_CPU >= 11100000UL) && (F_CPU <= 14300000UL)

    // In the 12 MHz case, an optimized 800 KHz datastream (no dead time
    // between bytes) requires a PORT-specific loop similar to the 8 MHz
    // code (but a little more relaxed in this case).

    // 15 instruction clocks per bit: HHHHxxxxxxLLLLL
    // OUT instructions:              ^   ^     ^     (T=0,4,10)

    volatile uint8_t next;

    hi   = *port |  pinMask;
    lo   = *port & ~pinMask;
    next = lo;
    if (b & 0x80) {
      next = hi;
    }

      // Don't "optimize" the OUT calls into the bitTime subroutine;
      // we're exploiting the RCALL and RET as 3- and 4-cycle NOPs!
    asm volatile(
     "headD:"                   "\n\t" //        (T =  0)
      "st   %a[port], %[hi]"    "\n\t" //        (T =  1)
      "rcall bitTimeD"          "\n\t" // Bit 7  (T = 15)
      "st   %a[port], %[hi]"    "\n\t"
      "rcall bitTimeD"          "\n\t" // Bit 6
      "st   %a[port], %[hi]"    "\n\t"
      "rcall bitTimeD"          "\n\t" // Bit 5
      "st   %a[port], %[hi]"    "\n\t"
      "rcall bitTimeD"          "\n\t" // Bit 4
      "st   %a[port], %[hi]"    "\n\t"
      "rcall bitTimeD"          "\n\t" // Bit 3
      "st   %a[port], %[hi]"    "\n\t"
      "rcall bitTimeD"          "\n\t" // Bit 2
      "st   %a[port], %[hi]"    "\n\t"
      "rcall bitTimeD"          "\n\t" // Bit 1
      // Bit 0:
      "st   %a[port], %[hi]"    "\n\t" // 1    PORT = hi    (T =  1)
      "rjmp .+0"                "\n\t" // 2    nop nop      (T =  3)
      "ld   %[byte] , %a[ptr]+" "\n\t" // 2    b = *ptr++   (T =  5)
      "st   %a[port], %[next]"  "\n\t" // 1    PORT = next  (T =  6)
      "mov  %[next] , %[lo]"    "\n\t" // 1    next = lo    (T =  7)
      "sbrc %[byte] , 7"        "\n\t" // 1-2  if (b & 0x80) (T =  8)
       "mov %[next] , %[hi]"    "\n\t" // 0-1    next = hi  (T =  9)
      "nop"                     "\n\t" // 1                 (T = 10)
      "st   %a[port], %[lo]"    "\n\t" // 1    PORT = lo    (T = 11)
      "sbiw %[count], 1"        "\n\t" // 2    i--          (T = 13)
      "brne headD"              "\n\t" // 2    if (i != 0) -> (next byte)
       "rjmp doneD"             "\n\t"
      "bitTimeD:"               "\n\t" //      nop nop nop     (T =  4)
       "st   %a[port], %[next]" "\n\t" // 1    PORT = next     (T =  5)
       "mov  %[next], %[lo]"    "\n\t" // 1    next = lo       (T =  6)
       "rol  %[byte]"           "\n\t" // 1    b <<= 1         (T =  7)
       "sbrc %[byte], 7"        "\n\t" // 1-2  if (b & 0x80)    (T =  8)
        "mov %[next], %[hi]"    "\n\t" // 0-1   next = hi      (T =  9)
       "nop"                    "\n\t" // 1                    (T = 10)
       "st   %a[port], %[lo]"   "\n\t" // 1    PORT = lo       (T = 11)
       "ret"                    "\n\t" // 4    nop nop nop nop (T = 15)
       "doneD:"                 "\n"
    : [ptr]   "+e" (ptr),
      [byte]  "+r" (b),
      [next]  "+r" (next),
      [count] "+w" (i)
    : [port]   "e" (port),
      [hi]     "r" (hi),
      [lo]     "r" (lo));


// 16 MHz(ish) AVRxt ------------------------------------------------------
#elif (F_CPU >= 15400000UL) && (F_CPU <= 19000000L)

    // WS2811 and WS2812 have different hi/lo duty cycles; this is
    // similar but NOT an exact copy of the prior 400-on-8 code.

    // 20 inst. clocks per bit: HHHHHxxxxxxxxLLLLLLL
    // ST instructions:         ^    ^       ^       (T=0,5,13)

    volatile uint8_t next, bit;

    hi   = *port |  pinMask;
    lo   = *port & ~pinMask;
    next = lo;
    bit  = 8;

    asm volatile(
     "head20:"                   "\n\t" // Clk  Pseudocode    (T =  0)
      "st   %a[port],  %[hi]"    "\n\t" // 1    PORT = hi     (T =  1)
      "nop"                      "\n\t" // 1    nop           (T =  2)
      "sbrc %[byte],  7"         "\n\t" // 1-2  if (b & 128)
       "mov  %[next], %[hi]"     "\n\t" // 0-1   next = hi    (T =  4)
      "dec  %[bit]"              "\n\t" // 1    bit--         (T =  5)
      "st   %a[port],  %[next]"  "\n\t" // 1    PORT = next   (T =  6)
      "nop"                      "\n\t" // 1    nop           (T =  7)
      "mov  %[next] ,  %[lo]"    "\n\t" // 1    next = lo     (T =  8)
      "breq nextbyte20"          "\n\t" // 1-2  if (bit == 0) (from dec above)
      "rol  %[byte]"             "\n\t" // 1    b <<= 1       (T = 10)
      "rjmp .+0"                 "\n\t" // 2    nop nop       (T = 12)
      "nop"                      "\n\t" // 1    nop           (T = 13)
      "st   %a[port],  %[lo]"    "\n\t" // 1    PORT = lo     (T = 14)
      "rjmp .+0"                 "\n\t" // 2    nop nop       (T = 16)
      "rjmp .+0"                 "\n\t" // 2    nop nop       (T = 18)
      "rjmp head20"              "\n\t" // 2    -> head20 (next bit out) (T=20)
     "nextbyte20:"               "\n\t" //                    (T = 10)
      "ldi  %[bit]  ,  8"        "\n\t" // 1    bit = 8       (T = 11)
      "ld   %[byte] ,  %a[ptr]+" "\n\t" // 2    b = *ptr++    (T = 13)
      "st   %a[port], %[lo]"     "\n\t" // 1    PORT = lo     (T = 14)
      "rjmp .+0"                 "\n\t" // 2    nop nop       (T = 16)
      "sbiw %[count], 1"         "\n\t" // 2    i--           (T = 18)
       "brne head20"             "\n"   // 2    if (i != 0) -> (next byte) (T=20)
    : [ptr]   "+e" (ptr),
      [byte]  "+r" (b),
      [bit]   "+d" (bit),
      [next]  "+r" (next),
      [count] "+w" (i)
    : [port]   "e" (port),
      [hi]     "r" (hi),
      [lo]     "r" (lo));

// 20 MHz(ish) AVRxt ------------------------------------------------------
#elif (F_CPU >= 19000000UL) && (F_CPU <= 22000000L)


    // 25 inst. clocks per bit: HHHHHHHxxxxxxxxLLLLLLLLLL
    // ST instructions:         ^      ^       ^       (T=0,7,15)

    volatile uint8_t next, bit;

    hi   = *port |  pinMask;
    lo   = *port & ~pinMask;
    next = lo;
    bit  = 8;

    asm volatile(
     "head20:"                   "\n\t" // Clk  Pseudocode    (T =  0)
      "st   %a[port],  %[hi]"    "\n\t" // 1    PORT = hi     (T =  1)
      "sbrc %[byte],  7"         "\n\t" // 1-2  if (b & 128)
       "mov  %[next], %[hi]"     "\n\t" // 0-1   next = hi    (T =  3)
      "dec  %[bit]"              "\n\t" // 1    bit--         (T =  4)
      "nop"                      "\n\t" // 1    nop           (T =  5)
      "rjmp .+0"                 "\n\t" // 2    nop nop       (T =  7)
      "st   %a[port],  %[next]"  "\n\t" // 1    PORT = next   (T =  8)
      "mov  %[next] ,  %[lo]"    "\n\t" // 1    next = lo     (T =  9)
      "breq nextbyte20"          "\n\t" // 1-2  if (bit == 0) (from dec above)
      "rol  %[byte]"             "\n\t" // 1    b <<= 1       (T = 11)
      "rjmp .+0"                 "\n\t" // 2    nop nop       (T = 13)
      "rjmp .+0"                 "\n\t" // 2    nop nop       (T = 15)
      "st   %a[port],  %[lo]"    "\n\t" // 1    PORT = lo     (T = 16)
      "nop"                      "\n\t" // 1    nop           (T = 17)
      "rjmp .+0"                 "\n\t" // 2    nop nop       (T = 19)
      "rjmp .+0"                 "\n\t" // 2    nop nop       (T = 21)
      "rjmp .+0"                 "\n\t" // 2    nop nop       (T = 23)
      "rjmp head20"              "\n\t" // 2    -> head20 (next bit out)
     "nextbyte20:"               "\n\t" //                    (T = 11)
      "ldi  %[bit]  ,  8"        "\n\t" // 1    bit = 8       (T = 12)
      "ld   %[byte] ,  %a[ptr]+" "\n\t" // 2    b = *ptr++    (T = 14)
      "nop"                      "\n\t" // 1    nop           (T = 15)
      "st   %a[port], %[lo]"     "\n\t" // 1    PORT = lo     (T = 16)
      "nop"                      "\n\t" // 1    nop           (T = 17)
      "rjmp .+0"                 "\n\t" // 2    nop nop       (T = 19)
      "rjmp .+0"                 "\n\t" // 2    nop nop       (T = 21)
      "sbiw %[count], 1"         "\n\t" // 2    i--           (T = 23)
       "brne head20"             "\n"   // 2    if (i != 0) -> (next byte)  ()
    : [ptr]   "+e" (ptr),
      [byte]  "+r" (b),
      [bit]   "+d" (bit),
      [next]  "+r" (next),
      [count] "+w" (i)
    : [port]   "e" (port),
      [hi]     "r" (hi),
      [lo]     "r" (lo));

// 24 (22~26) MHz AVRxt  ------------------------------------------------------
#elif (F_CPU >= 22000000UL) && (F_CPU <= 26000000L)


    // 30 inst. clocks per bit: HHHHHHHxxxxxxxxLLLLLLLLLL
    // ST instructions:         ^      ^       ^       (T=0,9,18)

    volatile uint8_t next, bit;

    hi   = *port |  pinMask;
    lo   = *port & ~pinMask;
    next = lo;
    bit  = 8;


    asm volatile(
     "head24:"                   "\n\t" // Clk  Pseudocode    (T =  0)
      "st   %a[port],  %[hi]"    "\n\t" // 1    PORT = hi     (T =  1)
      "sbrc %[byte],  7"         "\n\t" // 1-2  if (b & 128)
      "mov  %[next], %[hi]"      "\n\t" // 0-1   next = hi    (T =  3)
      "dec  %[bit]"              "\n\t" // 1    bit--         (T =  4)
      "nop"                      "\n\t" // 1    nop           (T =  5)
      "rjmp .+0"                 "\n\t" // 2    nop nop       (T =  7)
      "rjmp .+0"                 "\n\t" // 2    nop nop       (T =  9)
      "st   %a[port],  %[next]"  "\n\t" // 1    PORT = next   (T = 10)
      "mov  %[next] ,  %[lo]"    "\n\t" // 1    next = lo     (T = 11)
      "nop"                      "\n\t" // 1    nop           (T = 12)
      "rjmp .+0"                 "\n\t" // 2    nop nop       (T = 14)
      "rjmp .+0"                 "\n\t" // 2    nop nop       (T = 16)
      "breq nextbyte24"          "\n\t" // 1-2  if (bit == 0) (from dec above)
      "rol  %[byte]"             "\n\t" // 1    b <<= 1       (T = 18)
      "st   %a[port],  %[lo]"    "\n\t" // 1    PORT = lo     (T = 19)
      "rcall seconddelay24"      "\n\t" // 2+4+3=9            (T = 28)
      "rjmp head24"              "\n\t" // 2    -> head20 (next bit out)
     "seconddelay24:"            "\n\t" //
      "nop"                      "\n\t" // 1
      "rjmp .+0"                 "\n\t" // 2
      "ret"                      "\n\t" // 4
     "nextbyte24:"               "\n\t" // last bit of a byte (T = 18)
      "st   %a[port], %[lo]"     "\n\t" // 1    PORT = lo     (T = 19)
      "ldi  %[bit]  ,  8"        "\n\t" // 1    bit = 8       (T = 20)
      "ld   %[byte] ,  %a[ptr]+" "\n\t" // 2    b = *ptr++    (T = 22)
      "rjmp .+0"                 "\n\t" // 2    nop nop       (T = 24)
      "rjmp .+0"                 "\n\t" // 2    nop nop       (T = 26)
      "sbiw %[count], 1"         "\n\t" // 2    i--           (T = 28)
      "brne head24"              "\n"   // 2    if (i != 0) -> (next byte)  ()
    : [ptr]   "+e" (ptr),
      [byte]  "+r" (b),
      [bit]   "+d" (bit),
      [next]  "+r" (next),
      [count] "+w" (i)
    : [port]   "e" (port),
      [hi]     "r" (hi),
      [lo]     "r" (lo));




// 28 (26~30) MHz AVRxt  ------------------------------------------------------
#elif (F_CPU >= 26000000UL) && (F_CPU <= 30000000L)


    // 35 inst. clocks per bit: HHHHHHHxxxxxxxxLLLLLLLLLL
    // ST instructions:         ^      ^       ^       (T=0,10,21)

    volatile uint8_t next, bit;

    hi   = *port |  pinMask;
    lo   = *port & ~pinMask;
    next = lo;
    bit  = 8;

    asm volatile(
     "head28:"                   "\n\t" // Clk  Pseudocode    (T =  0)
      "st   %a[port],  %[hi]"    "\n\t" // 1    PORT = hi     (T =  1)
      "sbrc %[byte],  7"         "\n\t" // 1-2  if (b & 128)
       "mov  %[next], %[hi]"     "\n\t" // 0-1   next = hi    (T =  3)
      "dec  %[bit]"              "\n\t" // 1    bit--         (T =  4)
      "rcall zerothdelay32"      "\n\t" // 2+4=6
      "st   %a[port],  %[next]"  "\n\t" // 1    PORT = next   (T = 11)
      "mov  %[next] ,  %[lo]"    "\n\t" // 1    next = lo     (T = 12)
      "rcall firstdelay28"       "\n\t" // 2+4 = 7            (T = 19)
      "breq nextbyte28"          "\n\t" // 1-2  if (bit == 0) (from dec above)
      "rol  %[byte]"             "\n\t" // 1    b <<= 1       (T = 21)
      "st   %a[port],  %[lo]"    "\n\t" // 1    PORT = lo     (T = 22)
      "rcall seconddelay28"      "\n\t" // 2+4+1+4=11         (T = 33)
      "rjmp head28"              "\n\t" // 2    -> head20 (next bit out)
     "seconddelay28:"            "\n\t" //
      "rjmp .+0"                 "\n\t" // 2
      "rjmp .+0"                 "\n\t" // 2
     "firstdelay28:"             "\n\t" // first delay
      "nop"                      "\n\t" // 1    nop
     "thirddelay28:"             "\n\t" // third delay
     "zerothdelay28:"            "\n\t"
      "ret"                      "\n\t" // 4
     "nextbyte28:"               "\n\t" // last bit of a byte (T = 21)
      "st   %a[port], %[lo]"     "\n\t" // 1    PORT = lo     (T = 22)
      "ldi  %[bit]  ,  8"        "\n\t" // 1    bit = 8       (T = 23)
      "ld   %[byte] ,  %a[ptr]+" "\n\t" // 2    b = *ptr++    (T = 25)
      "rcall thirddelay28"       "\n\t" // 2+4 = 6            (T = 31)
      "sbiw %[count], 1"         "\n\t" // 2    i--           (T = 33)
      "brne head28"              "\n"   // 2    if (i != 0) -> (next byte)  ()
    : [ptr]   "+e" (ptr),
      [byte]  "+r" (b),
      [bit]   "+d" (bit),
      [next]  "+r" (next),
      [count] "+w" (i)
    : [port]   "e" (port),
      [hi]     "r" (hi),
      [lo]     "r" (lo));


// 32 (30~34) MHz AVRxt  ------------------------------------------------------
#elif (F_CPU > 30000000UL) && (F_CPU <= 34000000L)


    // 40 inst. clocks per bit: HHHHHHHxxxxxxxxLLLLLLLLLL
    // ST instructions:         ^      ^       ^       (T=0,11,24)

    volatile uint8_t next, bit;

    hi   = *port |  pinMask;
    lo   = *port & ~pinMask;
    next = lo;
    bit  = 8;

    asm volatile(
     "head32:"                   "\n\t" // Clk  Pseudocode    (T =  0)
      "st   %a[port],  %[hi]"    "\n\t" // 1    PORT = hi     (T =  1)
      "sbrc %[byte],  7"         "\n\t" // 1-2  if (b & 128)
       "mov  %[next], %[hi]"     "\n\t" // 0-1   next = hi    (T =  3)
      "dec  %[bit]"              "\n\t" // 1    bit--         (T =  4)
      "rcall zerothdelay32"      "\n\t" // 2+4+1=7
      "st   %a[port],  %[next]"  "\n\t" // 1    PORT = next   (T = 12)
      "mov  %[next] ,  %[lo]"    "\n\t" // 1    next = lo     (T = 13)
      "rcall firstdelay32"       "\n\t" // 2+4+1+2 = 9        (T = 22)
      "breq nextbyte32"          "\n\t" // 1-2  if (bit == 0) (from dec above)
      "rol  %[byte]"             "\n\t" // 1    b <<= 1       (T = 24)
      "st   %a[port],  %[lo]"    "\n\t" // 1    PORT = lo     (T = 25)
      "rcall seconddelay32"      "\n\t" // 2+4+3+2+3=13       (T = 38)
      "rjmp head32"              "\n\t" // 2    -> head20 (next bit out)
     "seconddelay32:"            "\n\t" // second delay 13 cycles
      "rjmp .+0"                 "\n\t" // 2
      "rjmp .+0"                 "\n\t" // 2
     "firstdelay32:"             "\n\t" // first delay 9 cycles
      "nop"                      "\n\t" // 1    nop
     "thirddelay32:"             "\n\t" // third delay 8 cycles
      "nop"                      "\n\t" // 1    nop
     "zerothdelay32:"            "\n\t" // zeroth delay 7 cycles
      "nop"                      "\n\t" // 1    nop
      "ret"                      "\n\t" // 4
     "nextbyte32:"               "\n\t" // last bit of a byte (T = 24)
      "st   %a[port], %[lo]"     "\n\t" // 1    PORT = lo     (T = 25)
      "ldi  %[bit]  ,  8"        "\n\t" // 1    bit = 8       (T = 26)
      "ld   %[byte] ,  %a[ptr]+" "\n\t" // 2    b = *ptr++    (T = 28)
      "rcall thirddelay32"       "\n\t" // 2+4+1+1 = 8        (T = 36)
      "sbiw %[count], 1"         "\n\t" // 2    i--           (T = 38)
      "brne head32"              "\n"   // 2    if (i != 0) -> (next byte)  ()
    : [ptr]   "+e" (ptr),
      [byte]  "+r" (b),
      [bit]   "+d" (bit),
      [next]  "+r" (next),
      [count] "+w" (i)
    : [port]   "e" (port),
      [hi]     "r" (hi),
      [lo]     "r" (lo));

// 36 (34~38) MHz AVRxt  ------------------------------------------------------
#elif (F_CPU > 3400000UL) && (F_CPU <= 38000000L)


    // 45 inst. clocks per bit: HHHHHHHxxxxxxxxLLLLLLLLLL
    // ST instructions:         ^      ^       ^       (T=0,12,27)

    volatile uint8_t next, bit;

    hi   = *port |  pinMask;
    lo   = *port & ~pinMask;
    next = lo;
    bit  = 8;

    asm volatile(
     "head36:"                   "\n\t" // Clk  Pseudocode    (T =  0)
      "st   %a[port],  %[hi]"    "\n\t" // 1    PORT = hi     (T =  1)
      "sbrc %[byte],  7"         "\n\t" // 1-2  if (b & 128)
       "mov  %[next], %[hi]"     "\n\t" // 0-1   next = hi    (T =  3)
      "dec  %[bit]"              "\n\t" // 1    bit--         (T =  4)
      "rcall zerothdelay36"      "\n\t" // 2+4+2=8
      "st   %a[port],  %[next]"  "\n\t" // 1    PORT = next   (T = 13)
      "mov  %[next] ,  %[lo]"    "\n\t" // 1    next = lo     (T = 14)
      "rcall firstdelay36"       "\n\t" // 2+4+3 = 11         (T = 25)
      "breq nextbyte36"          "\n\t" // 1-2  if (bit == 0) (from dec above)
      "rol  %[byte]"             "\n\t" // 1    b <<= 1       (T = 27)
      "st   %a[port],  %[lo]"    "\n\t" // 1    PORT = lo     (T = 28)
      "rcall seconddelay36"      "\n\t" // 2+4+3+2+2=15       (T = 43)
      "rjmp head36"              "\n\t" // 2    -> head20 (next bit out)
     "seconddelay36:"            "\n\t" // second delay 15 cycles
      "rjmp .+0"                 "\n\t" // 2
      "rjmp .+0"                 "\n\t" // 2
     "firstdelay36:"             "\n\t" // first delay 11 cycles
      "nop"                      "\n\t" // 1    nop
     "thirddelay36:"             "\n\t" // third delay 10 cycles
      "rjmp .+0"                 "\n\t" // 2    nop nop
     "zerothdelay36:"            "\n\t" // zeroth delay 8 cycles
      "rjmp .+0"                 "\n\t" // 2    nop nop
      "ret"                      "\n\t" // 4
     "nextbyte36:"               "\n\t" // last bit of a byte (T = 27)
      "st   %a[port], %[lo]"     "\n\t" // 1    PORT = lo     (T = 28)
      "ldi  %[bit]  ,  8"        "\n\t" // 1    bit = 8       (T = 29)
      "ld   %[byte] ,  %a[ptr]+" "\n\t" // 2    b = *ptr++    (T = 31)
      "rcall thirddelay36"       "\n\t" // 2+4 = 10           (T = 41)
      "sbiw %[count], 1"         "\n\t" // 2    i--           (T = 43)
      "brne head36"              "\n"   // 2    if (i != 0) -> (next byte)  ()
    : [ptr]   "+e" (ptr),
      [byte]  "+r" (b),
      [bit]   "+d" (bit),
      [next]  "+r" (next),
      [count] "+w" (i)
    : [port]   "e" (port),
      [hi]     "r" (hi),
      [lo]     "r" (lo));


// 40 (38-44) MHz AVRxt  ------------------------------------------------------
#elif (F_CPU > 3800000UL) && (F_CPU <= 44000000L)


    // 50 inst. clocks per bit: HHHHHHHxxxxxxxxLLLLLLLLLL
    // ST instructions:         ^      ^       ^       (T=0,14,30)

    volatile uint8_t next, bit;

    hi   = *port |  pinMask;
    lo   = *port & ~pinMask;
    next = lo;
    bit  = 8;

    asm volatile(
     "head40:"                   "\n\t" // Clk  Pseudocode    (T =  0)
      "st   %a[port],  %[hi]"    "\n\t" // 1    PORT = hi     (T =  1)
      "sbrc %[byte],  7"         "\n\t" // 1-2  if (b & 128)
       "mov  %[next], %[hi]"     "\n\t" // 0-1   next = hi    (T =  3)
      "dec  %[bit]"              "\n\t" // 1    bit--         (T =  4)
      "rcall zerothdelay40"      "\n\t" // 2+4+4=10
      "st   %a[port],  %[next]"  "\n\t" // 1    PORT = next   (T = 15)
      "mov  %[next] ,  %[lo]"    "\n\t" // 1    next = lo     (T = 16)
      "rcall firstdelay40"       "\n\t" // 2+4+4+2 = 12         (T = 28)
      "breq nextbyte40"          "\n\t" // 1-2  if (bit == 0) (from dec above)
      "rol  %[byte]"             "\n\t" // 1    b <<= 1       (T = 30)
      "st   %a[port],  %[lo]"    "\n\t" // 1    PORT = lo     (T = 31)
      "rcall seconddelay40"      "\n\t" // 2+4+3+2+3=17       (T = 48)
      "rjmp head40"              "\n\t" // 2    -> head20 (next bit out)
     "seconddelay40:"            "\n\t" // second delay 17 cycles
      "nop"                      "\n\t" // 1    nop
      "rjmp .+0"                 "\n\t" // 2
      "rjmp .+0"                 "\n\t" // 2
     "thirddelay40:"             "\n\t" // third delay 12 cycles
     "firstdelay40:"             "\n\t" // first delay 12 cycles
      "rjmp .+0"                 "\n\t" // 2    nop nop
     "zerothdelay40:"            "\n\t" // zeroth delay 10 cycles
      "rjmp .+0"                 "\n\t" // 2    nop nop
      "rjmp .+0"                 "\n\t" // 2    nop nop
      "ret"                      "\n\t" // 4
     "nextbyte40:"               "\n\t" // last bit of a byte (T = 30)
      "st   %a[port], %[lo]"     "\n\t" // 1    PORT = lo     (T = 31)
      "ldi  %[bit]  ,  8"        "\n\t" // 1    bit = 8       (T = 32)
      "ld   %[byte] ,  %a[ptr]+" "\n\t" // 2    b = *ptr++    (T = 34)
      "rcall thirddelay40"       "\n\t" // 2+4+4+2 = 12       (T = 46)
      "sbiw %[count], 1"         "\n\t" // 2    i--           (T = 48)
      "brne head40"              "\n"   // 2    if (i != 0) -> (next byte)  ()
    : [ptr]   "+e" (ptr),
      [byte]  "+r" (b),
      [bit]   "+d" (bit),
      [next]  "+r" (next),
      [count] "+w" (i)
    : [port]   "e" (port),
      [hi]     "r" (hi),
      [lo]     "r" (lo));

// 48 (44-50) MHz AVRxt  ------------------------------------------------------
#elif (F_CPU > 4400000UL) && (F_CPU <= 50000000L)


    // 60 inst. clocks per bit: HHHHHHHxxxxxxxxLLLLLLLLLL
    // ST instructions:         ^      ^       ^       (T=0,16,35)

    volatile uint8_t next, bit;

    hi   = *port |  pinMask;
    lo   = *port & ~pinMask;
    next = lo;
    bit  = 8;
    asm volatile(
     "head48:"                   "\n\t" // Clk  Pseudocode    (T =  0)
      "st   %a[port],  %[hi]"    "\n\t" // 1    PORT = hi     (T =  1)
      "sbrc %[byte],  7"         "\n\t" // 1-2  if (b & 128)
      "mov  %[next], %[hi]"      "\n\t" // 0-1   next = hi    (T =  3)
      "dec  %[bit]"              "\n\t" // 1    bit--         (T =  4)
      "rcall zerothdelay48"      "\n\t" // 2+4=13
      "st   %a[port],  %[next]"  "\n\t" // 1    PORT = next   (T = 17)
      "mov  %[next] ,  %[lo]"    "\n\t" // 1    next = lo     (T = 18)
      "rcall firstdelay48"       "\n\t" // 2+4+3 = 15         (T = 33)
      "breq nextbyte48"          "\n\t" // 1-2  if (bit == 0) (from dec above)
      "rol  %[byte]"             "\n\t" // 1    b <<= 1       (T = 35)
      "st   %a[port],  %[lo]"    "\n\t" // 1    PORT = lo     (T = 36)
      "rcall seconddelay48"      "\n\t" // 2+4+3+2+3=22       (T = 58)
      "rjmp head48"              "\n\t" // 2    -> head20 (next bit out)
     "seconddelay48:"            "\n\t" // second delay 22 cycles
      "rjmp .+0"                 "\n\t" // 2
      "rjmp .+0"                 "\n\t" // 2
      "nop"                      "\n\t" // 1    nop
     "thirddelay48:"             "\n\t" // third delay 17 cycles
      "rjmp .+0"                 "\n\t" // 2
     "firstdelay48:"             "\n\t" // first delay 15 cycles
      "rjmp .+0"                 "\n\t" // 2    nop nop
     "zerothdelay48:"            "\n\t" // zeroth delay 13 cycles
      "nop"                      "\n\t" // 1    nop
      "rcall emptydelay48"       "\n\t" // 2+4
      "ret"                      "\n\t" // 4
     "emptydelay48:"             "\n\t" // immediately returns: 2+4 = 6 cycles, for 2 words!
      "ret"                      "\n\t" // 4
     "nextbyte48:"               "\n\t" // last bit of a byte (T = 35)
      "st   %a[port], %[lo]"     "\n\t" // 1    PORT = lo     (T = 36)
      "ldi  %[bit]  ,  8"        "\n\t" // 1    bit = 8       (T = 37)
      "ld   %[byte] ,  %a[ptr]+" "\n\t" // 2    b = *ptr++    (T = 39)
      "rcall thirddelay48"       "\n\t" // 2+4 = 17           (T = 56)
      "sbiw %[count], 1"         "\n\t" // 2    i--           (T = 58)
      "brne head48"              "\n"   // 2    if (i != 0) -> (next byte)  ()
    : [ptr]   "+e" (ptr),
      [byte]  "+r" (b),
      [bit]   "+d" (bit),
      [next]  "+r" (next),
      [count] "+w" (i)
    : [port]   "e" (port),
      [hi]     "r" (hi),
      [lo]     "r" (lo));

  #else
    #error "CPU SPEED NOT SUPPORTED"
  #endif


  interrupts();
  #if (!defined(MILLIS_USE_TIMERNONE) && !defined(MILLIS_USE_TIMERRTC) && !defined(MILLIS_USE_TIMERRTC_XTAL) && !defined(MILLIS_USE_TIMERRTC_XOSC))
    endTime = micros();
    // Save EOD time for latch on next call
  #else
    #warning "micros() is not available because millis is disabled from the tools subemnu. It is your responsibility to ensure a sufficient time has passed between calls to show(). See documentation."
  #endif
}

// Set the output pin number
void tinyNeoPixel::setPin(uint8_t p) {
  if (begun && (pin < NUM_DIGITAL_PINS)) {
    pinMode(pin, INPUT);
  }
  pin = p;
  if (begun) {
    pinMode(p, OUTPUT);
    digitalWrite(p, LOW);
  }
  port    = portOutputRegister(digitalPinToPort(p));
  pinMask = digitalPinToBitMask(p);
}

// Set pixel color from separate R,G,B components:
void tinyNeoPixel::setPixelColor(uint16_t n, uint8_t r, uint8_t g, uint8_t b) {
  if (n < numLEDs) {
    if (brightness) { // See notes in setBrightness()
      r = (r * brightness) >> 8;
      g = (g * brightness) >> 8;
      b = (b * brightness) >> 8;
    }
    uint8_t *p;
    if (wOffset == rOffset) { // Is an RGB-type strip
      p = &pixels[n * 3];    // 3 bytes per pixel
    } else {                 // Is a WRGB-type strip
      p = &pixels[n * 4];    // 4 bytes per pixel
      p[wOffset] = 0;        // But only R,G,B passed -- set W to 0
    }
    p[rOffset] = r;          // R,G,B always stored
    p[gOffset] = g;
    p[bOffset] = b;
  }
}

void tinyNeoPixel::setPixelColor(uint16_t n, uint8_t r, uint8_t g, uint8_t b, uint8_t w) {
  if (n < numLEDs) {
    if (brightness) { // See notes in setBrightness()
      r = (r * brightness) >> 8;
      g = (g * brightness) >> 8;
      b = (b * brightness) >> 8;
      w = (w * brightness) >> 8;
    }
    uint8_t *p;
    if (wOffset == rOffset) { // Is an RGB-type strip
      p = &pixels[n * 3];    // 3 bytes per pixel (ignore W)
    } else {                 // Is a WRGB-type strip
      p = &pixels[n * 4];    // 4 bytes per pixel
      p[wOffset] = w;        // Store W
    }
    p[rOffset] = r;          // Store R,G,B
    p[gOffset] = g;
    p[bOffset] = b;
  }
}

// Set pixel color from 'packed' 32-bit RGB color:
void tinyNeoPixel::setPixelColor(uint16_t n, uint32_t c) {
  if (n < numLEDs) {
    uint8_t *p,
      r = (uint8_t)(c >> 16),
      g = (uint8_t)(c >>  8),
      b = (uint8_t)c;
    if (brightness) { // See notes in setBrightness()
      r = (r * brightness) >> 8;
      g = (g * brightness) >> 8;
      b = (b * brightness) >> 8;
    }
    if (wOffset == rOffset) {
      p = &pixels[n * 3];
    } else {
      p = &pixels[n * 4];
      uint8_t w = (uint8_t)(c >> 24);
      p[wOffset] = brightness ? ((w * brightness) >> 8) : w;
    }
    p[rOffset] = r;
    p[gOffset] = g;
    p[bOffset] = b;
  }
}

/*!
  @brief   Fill all or part of the NeoPixel strip with a color.
  @param   c      32-bit color value. Most significant byte is white (for
                  RGBW pixels) or ignored (for RGB pixels), next is red,
                  then green, and least significant byte is blue. If all
                  arguments are unspecified, this will be 0 (off).
  @param   first  Index of first pixel to fill, starting from 0. Must be
                  in-bounds, no clipping is performed. 0 if unspecified.
  @param   count  Number of pixels to fill, as a positive value. Passing
                  0 or leaving unspecified will fill to end of strip.
*/
void tinyNeoPixel::fill(uint32_t c, uint16_t first, uint16_t count) {
  uint16_t i, end;

  if (first >= numLEDs) {
    return; // If first LED is past end of strip, nothing to do
  }

  // Calculate the index ONE AFTER the last pixel to fill
  if (count == 0) {
    // Fill to end of strip
    end = numLEDs;
  } else {
    // Ensure that the loop won't go past the last pixel
    end = first + count;
    if (end > numLEDs) end = numLEDs;
  }

  for(i = first; i < end; i++) {
    this->setPixelColor(i, c);
  }
}


/*!
  @brief   Convert hue, saturation and value into a packed 32-bit RGB color
           that can be passed to setPixelColor() or other RGB-compatible
           functions.
  @param   hue  An unsigned 16-bit value, 0 to 65535, representing one full
                loop of the color wheel, which allows 16-bit hues to "roll
                over" while still doing the expected thing (and allowing
                more precision than the wheel() function that was common to
                prior NeoPixel examples).
  @param   sat  Saturation, 8-bit value, 0 (min or pure grayscale) to 255
                (max or pure hue). Default of 255 if unspecified.
  @param   val  Value (brightness), 8-bit value, 0 (min / black / off) to
                255 (max or full brightness). Default of 255 if unspecified.
  @return  Packed 32-bit RGB with the most significant byte set to 0 -- the
           white element of WRGB pixels is NOT utilized. Result is linearly
           but not perceptually correct, so you may want to pass the result
           through the gamma32() function (or your own gamma-correction
           operation) else colors may appear washed out. This is not done
           automatically by this function because coders may desire a more
           refined gamma-correction function than the simplified
           one-size-fits-all operation of gamma32(). Diffusing the LEDs also
           really seems to help when using low-saturation colors.
*/
uint32_t tinyNeoPixel::ColorHSV(uint16_t hue, uint8_t sat, uint8_t val) {

  uint8_t r, g, b;

  // Remap 0-65535 to 0-1529. Pure red is CENTERED on the 64K rollover;
  // 0 is not the start of pure red, but the midpoint...a few values above
  // zero and a few below 65536 all yield pure red (similarly, 32768 is the
  // midpoint, not start, of pure cyan). The 8-bit RGB hexcone (256 values
  // each for red, green, blue) really only allows for 1530 distinct hues
  // (not 1536, more on that below), but the full unsigned 16-bit type was
  // chosen for hue so that one's code can easily handle a contiguous color
  // wheel by allowing hue to roll over in either direction.
  hue = (hue * 1530L + 32768) / 65536;
  // Because red is centered on the rollover point (the +32768 above,
  // essentially a fixed-point +0.5), the above actually yields 0 to 1530,
  // where 0 and 1530 would yield the same thing. Rather than apply a
  // costly modulo operator, 1530 is handled as a special case below.

  // So you'd think that the color "hexcone" (the thing that ramps from
  // pure red, to pure yellow, to pure green and so forth back to red,
  // yielding six slices), and with each color component having 256
  // possible values (0-255), might have 1536 possible items (6*256),
  // but in reality there's 1530. This is because the last element in
  // each 256-element slice is equal to the first element of the next
  // slice, and keeping those in there this would create small
  // discontinuities in the color wheel. So the last element of each
  // slice is dropped...we regard only elements 0-254, with item 255
  // being picked up as element 0 of the next slice. Like this:
  // Red to not-quite-pure-yellow is:        255,   0, 0 to 255, 254,   0
  // Pure yellow to not-quite-pure-green is: 255, 255, 0 to   1, 255,   0
  // Pure green to not-quite-pure-cyan is:     0, 255, 0 to   0, 255, 254
  // and so forth. Hence, 1530 distinct hues (0 to 1529), and hence why
  // the constants below are not the multiples of 256 you might expect.

  // Convert hue to R,G,B (nested ifs faster than divide+mod+switch):
  if (hue < 510) {         // Red to Green-1
    b = 0;
    if (hue < 255) {       //   Red to Yellow-1
      r = 255;
      g = hue;            //     g = 0 to 254
    } else {              //   Yellow to Green-1
      r = 510 - hue;      //     r = 255 to 1
      g = 255;
    }
  } else if (hue < 1020) { // Green to Blue-1
    r = 0;
    if (hue <  765) {      //   Green to Cyan-1
      g = 255;
      b = hue - 510;      //     b = 0 to 254
    } else {              //   Cyan to Blue-1
      g = 1020 - hue;     //     g = 255 to 1
      b = 255;
    }
  } else if (hue < 1530) { // Blue to Red-1
    g = 0;
    if (hue < 1275) {      //   Blue to Magenta-1
      r = hue - 1020;     //     r = 0 to 254
      b = 255;
    } else {              //   Magenta to Red-1
      r = 255;
      b = 1530 - hue;     //     b = 255 to 1
    }
  } else {                // Last 0.5 Red (quicker than % operator)
    r = 255;
    g = b = 0;
  }

  // Apply saturation and value to R,G,B, pack into 32-bit result:
  uint32_t v1 =   1 + val; // 1 to 256; allows >>8 instead of /255
  uint16_t s1 =   1 + sat; // 1 to 256; same reason
  uint8_t  s2 = 255 - sat; // 255 to 0
  return ((((((r * s1) >> 8) + s2) * v1) & 0xff00) << 8) |
          (((((g * s1) >> 8) + s2) * v1) & 0xff00)       |
          (((((b * s1) >> 8) + s2) * v1)           >> 8);
}


// Query color from previously-set pixel (returns packed 32-bit RGB value)
uint32_t tinyNeoPixel::getPixelColor(uint16_t n) const {
  if (n >= numLEDs) {
    return 0;  // Out of bounds, return no color.
  }

  uint8_t *p;

  if (wOffset == rOffset) { // Is RGB-type device
    p = &pixels[n * 3];
    if (brightness) {
      // Stored color was decimated by setBrightness().  Returned value
      // attempts to scale back to an approximation of the original 24-bit
      // value used when setting the pixel color, but there will always be
      // some error -- those bits are simply gone.  Issue is most
      // pronounced at low brightness levels.
      return (((uint32_t)(p[rOffset] << 8) / brightness) << 16) |
             (((uint32_t)(p[gOffset] << 8) / brightness) <<  8) |
             ( (uint32_t)(p[bOffset] << 8) / brightness       );
    } else {
      // No brightness adjustment has been made -- return 'raw' color
      return ((uint32_t)p[rOffset] << 16) |
             ((uint32_t)p[gOffset] <<  8) |
              (uint32_t)p[bOffset];
    }
  } else {                 // Is RGBW-type device
    p = &pixels[n * 4];
    if (brightness) { // Return scaled color
      return (((uint32_t)(p[wOffset] << 8) / brightness) << 24) |
             (((uint32_t)(p[rOffset] << 8) / brightness) << 16) |
             (((uint32_t)(p[gOffset] << 8) / brightness) <<  8) |
             ( (uint32_t)(p[bOffset] << 8) / brightness       );
    } else { // Return raw color
      return ((uint32_t)p[wOffset] << 24) |
             ((uint32_t)p[rOffset] << 16) |
             ((uint32_t)p[gOffset] <<  8) |
              (uint32_t)p[bOffset];
    }
  }
}

// Returns pointer to pixels[] array.  Pixel data is stored in device-
// native format and is not translated here.  Application will need to be
// aware of specific pixel data format and handle colors appropriately.
uint8_t *tinyNeoPixel::getPixels(void) const {
  return pixels;
}

uint16_t tinyNeoPixel::numPixels(void) const {
  return numLEDs;
}

// Adjust output brightness; 0=darkest (off), 255=brightest.  This does
// NOT immediately affect what's currently displayed on the LEDs.  The
// next call to show() will refresh the LEDs at this level.  However,
// this process is potentially "lossy," especially when increasing
// brightness.  The tight timing in the WS2811/WS2812 code means there
// aren't enough free cycles to perform this scaling on the fly as data
// is issued.  So we make a pass through the existing color data in RAM
// and scale it (subsequent graphics commands also work at this
// brightness level).  If there's a significant step up in brightness,
// the limited number of steps (quantization) in the old data will be
// quite visible in the re-scaled version.  For a non-destructive
// change, you'll need to re-render the full strip data.
void tinyNeoPixel::setBrightness(uint8_t b) {
  // Stored brightness value is different than what's passed.
  // This simplifies the actual scaling math later, allowing a fast
  // 8x8-bit multiply and taking the MSB.  'brightness' is a uint8_t,
  // adding 1 here may (intentionally) roll over...so 0 = max brightness
  // (color values are interpreted literally; no scaling), 1 = min
  // brightness (off), 255 = just below max brightness.
  uint8_t newBrightness = b + 1;
  if (newBrightness != brightness) { // Compare against prior value
    // Brightness has changed -- re-scale existing data in RAM
    uint8_t  c,
            *ptr           = pixels,
             oldBrightness = brightness - 1; // De-wrap old brightness value
    uint16_t scale;
    if (oldBrightness == 0) {
      scale = 0;  // Avoid /0
    } else if (b == 255) {
      scale = 65535 / oldBrightness;
    } else {
      scale = (((uint16_t)newBrightness << 8) - 1) / oldBrightness;
    }
    for (uint16_t i = 0; i < numBytes; i++) {
      c      = *ptr;
      *ptr++ = (c * scale) >> 8;
    }
    brightness = newBrightness;
  }
}

// Return the brightness value
uint8_t tinyNeoPixel::getBrightness(void) const {
  return brightness - 1;
}

void tinyNeoPixel::clear() {
  memset(pixels, 0, numBytes);
}

// A 32-bit variant of gamma8() that applies the same function
// to all components of a packed RGB or WRGB value.
uint32_t tinyNeoPixel::gamma32(uint32_t x) {
  uint8_t *y = (uint8_t *)&x;
  // All four bytes of a 32-bit value are filtered even if RGB (not WRGB),
  // to avoid a bunch of shifting and masking that would be necessary for
  // properly handling different endianisms (and each byte is a fairly
  // trivial operation, so it might not even be wasting cycles vs a check
  // and branch for the RGB case). In theory this might cause trouble *if*
  // someone's storing information in the unused most significant byte
  // of an RGB value, but this seems exceedingly rare and if it's
  // encountered in reality they can mask values going in or coming out.
  for(uint8_t i = 0; i<4; i++) y[i] = gamma8(y[i]);
  return x; // Packed 32-bit return
}
