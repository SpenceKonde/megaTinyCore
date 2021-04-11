/* Core Parts - a part of Arduino.h for megaTinyCore 2.3.0 and later
 * This is directly included by Arduino.h and nothing else; it just moves
 * a bunch of macros that had started to make Arduino.h hard to read out
 * of that file - most significantly the part-identification and version
 * macro handling stuff
 *
 * Spence Konde 2021 - megaTinyCore is free software (LGPL 2.1)
 * See LICENSE.txt for full legal boilerplate if you must */

//#defines to identify part families
#if defined(__AVR_ATtiny3227__)
#define MEGATINYCORE_MCU 3227
#define MEGATINYCORE_SERIES 2
#define __AVR_ATtinyx27__
#define __AVR_ATtinyxy7__
#elif defined(__AVR_ATtiny1627__)
#define MEGATINYCORE_MCU 1627
#define MEGATINYCORE_SERIES 2
#define __AVR_ATtinyx27__
#define __AVR_ATtinyxy7__
#elif defined(__AVR_ATtiny827__)
#define MEGATINYCORE_MCU 827
#define MEGATINYCORE_SERIES 2
#define __AVR_ATtinyx27__
#define __AVR_ATtinyxy7__
#elif defined(__AVR_ATtiny427__)
#define MEGATINYCORE_MCU 427
#define MEGATINYCORE_SERIES 2
#define __AVR_ATtinyx27__
#define __AVR_ATtinyxy7__
#elif defined(__AVR_ATtiny3226__)
#define MEGATINYCORE_MCU 3226
#define MEGATINYCORE_SERIES 2
#define __AVR_ATtinyx26__
#define __AVR_ATtinyxy6__
#elif defined(__AVR_ATtiny1626__)
#define MEGATINYCORE_MCU 1626
#define MEGATINYCORE_SERIES 2
#define __AVR_ATtinyx26__
#define __AVR_ATtinyxy6__
#elif defined(__AVR_ATtiny826__)
#define MEGATINYCORE_MCU 826
#define MEGATINYCORE_SERIES 2
#define __AVR_ATtinyx26__
#define __AVR_ATtinyxy6__
#elif defined(__AVR_ATtiny426__)
#define MEGATINYCORE_MCU 426
#define MEGATINYCORE_SERIES 2
#define __AVR_ATtinyx26__
#define __AVR_ATtinyxy6__
#elif defined(__AVR_ATtiny3224__)
#define MEGATINYCORE_MCU 3224
#define MEGATINYCORE_SERIES 2
#define __AVR_ATtinyx24__
#define __AVR_ATtinyxy4__
#elif defined(__AVR_ATtiny1624__)
#define MEGATINYCORE_MCU 1624
#define MEGATINYCORE_SERIES 2
#define __AVR_ATtinyx24__
#define __AVR_ATtinyxy4__
#elif defined(__AVR_ATtiny824__)
#define MEGATINYCORE_MCU 824
#define MEGATINYCORE_SERIES 2
#define __AVR_ATtinyx24__
#define __AVR_ATtinyxy4__
#elif defined(__AVR_ATtiny424__)
#define MEGATINYCORE_MCU 424
#define MEGATINYCORE_SERIES 2
#define __AVR_ATtinyx24__
#define __AVR_ATtinyxy4__
#elif defined(__AVR_ATtiny3217__)
#define MEGATINYCORE_MCU 3217
#define MEGATINYCORE_SERIES 1
#define __AVR_ATtinyx17__
#define __AVR_ATtinyxy7__
#elif defined(__AVR_ATtiny1617__)
#define MEGATINYCORE_MCU 1617
#define MEGATINYCORE_SERIES 1
#define __AVR_ATtinyx17__
#define __AVR_ATtinyxy7__
#elif defined(__AVR_ATtiny817__)
#define MEGATINYCORE_MCU 817
#define MEGATINYCORE_SERIES 1
#define __AVR_ATtinyx17__
#define __AVR_ATtinyxy7__
#elif defined(__AVR_ATtiny417__)
#define MEGATINYCORE_MCU 417
#define MEGATINYCORE_SERIES 1
#define __AVR_ATtinyx17__
#define __AVR_ATtinyxy7__
#elif defined(__AVR_ATtiny3207__)
#define MEGATINYCORE_MCU 3207
#define MEGATINYCORE_SERIES 0
#define __AVR_ATtinyx07__
#define __AVR_ATtinyxy7__
#elif defined(__AVR_ATtiny1607__)
#define MEGATINYCORE_MCU 1607
#define MEGATINYCORE_SERIES 0
#define __AVR_ATtinyx07__
#define __AVR_ATtinyxy7__
#elif defined(__AVR_ATtiny807__)
#define MEGATINYCORE_MCU 807
#define MEGATINYCORE_SERIES 0
#define __AVR_ATtinyx07__
#define __AVR_ATtinyxy7__
#elif defined(__AVR_ATtiny407__)
#define MEGATINYCORE_MCU 407
#define MEGATINYCORE_SERIES 0
#define __AVR_ATtinyx07__
#define __AVR_ATtinyxy7__
#elif defined(__AVR_ATtiny3216__)
#define MEGATINYCORE_MCU 3216
#define MEGATINYCORE_SERIES 1
#define __AVR_ATtinyx06__
#define __AVR_ATtinyxy6__
#elif defined(__AVR_ATtiny1616__)
#define MEGATINYCORE_MCU 1616
#define MEGATINYCORE_SERIES 1
#define __AVR_ATtinyx06__
#define __AVR_ATtinyxy6__
#elif defined(__AVR_ATtiny816__)
#define MEGATINYCORE_MCU 816
#define MEGATINYCORE_SERIES 1
#define __AVR_ATtinyx06__
#define __AVR_ATtinyxy6__
#elif defined(__AVR_ATtiny416__)
#define MEGATINYCORE_MCU 416
#define MEGATINYCORE_SERIES 1
#define __AVR_ATtinyx16__
#define __AVR_ATtinyxy6__
#elif defined(__AVR_ATtiny1606__)
#define MEGATINYCORE_MCU 1606
#define MEGATINYCORE_SERIES 0
#define __AVR_ATtinyx06__
#define __AVR_ATtinyxy6__
#elif defined(__AVR_ATtiny806__)
#define MEGATINYCORE_MCU 806
#define MEGATINYCORE_SERIES 0
#define __AVR_ATtinyx06__
#define __AVR_ATtinyxy6__
#elif defined(__AVR_ATtiny406__)
#define MEGATINYCORE_MCU 406
#define MEGATINYCORE_SERIES 0
#define __AVR_ATtinyx06__
#define __AVR_ATtinyxy6__
#elif defined(__AVR_ATtiny214__)
#define MEGATINYCORE_MCU 214
#define MEGATINYCORE_SERIES 1
#define __AVR_ATtinyx14__
#define __AVR_ATtinyxy4__
#elif defined(__AVR_ATtiny1614__)
#define MEGATINYCORE_MCU 1614
#define MEGATINYCORE_SERIES 1
#define __AVR_ATtinyx14__
#define __AVR_ATtinyxy4__
#elif defined(__AVR_ATtiny814__)
#define MEGATINYCORE_MCU 814
#define MEGATINYCORE_SERIES 1
#define __AVR_ATtinyx14__
#define __AVR_ATtinyxy4__
#elif defined(__AVR_ATtiny414__)
#define MEGATINYCORE_MCU 414
#define MEGATINYCORE_SERIES 1
#define __AVR_ATtinyx14__
#define __AVR_ATtinyxy4__
#elif defined(__AVR_ATtiny204__)
#define MEGATINYCORE_MCU 204
#define MEGATINYCORE_SERIES 0
#define __AVR_ATtinyx04__
#define __AVR_ATtinyxy4__
#elif defined(__AVR_ATtiny804__)
#define MEGATINYCORE_MCU 804
#define MEGATINYCORE_SERIES 0
#define __AVR_ATtinyx04__
#define __AVR_ATtinyxy4__
#elif defined(__AVR_ATtiny404__)
#define MEGATINYCORE_MCU 404
#define MEGATINYCORE_SERIES 0
#define __AVR_ATtinyx04__
#define __AVR_ATtinyxy4__
#elif defined(__AVR_ATtiny1604__)
#define MEGATINYCORE_MCU 1604
#define MEGATINYCORE_SERIES 0
#define __AVR_ATtinyx04__
#define __AVR_ATtinyxy4__
#elif defined(__AVR_ATtiny212__)
#define MEGATINYCORE_MCU 212
#define MEGATINYCORE_SERIES 1
#define __AVR_ATtinyx12__
#define __AVR_ATtinyxy2__
#elif defined(__AVR_ATtiny412__)
#define MEGATINYCORE_MCU 412
#define MEGATINYCORE_SERIES 1
#define __AVR_ATtinyx12__
#define __AVR_ATtinyxy2__
#elif defined(__AVR_ATtiny202__)
#define MEGATINYCORE_MCU 202
#define MEGATINYCORE_SERIES 0
#define __AVR_ATtinyx02__
#define __AVR_ATtinyxy2__
#elif defined(__AVR_ATtiny402__)
#define MEGATINYCORE_MCU 402
#define MEGATINYCORE_SERIES 0
#define __AVR_ATtinyx02__
#define __AVR_ATtinyxy2__
#else
  #error "Can't-happen: unknown chip somehow being used!"
#endif

/* PORT names and the NOT_A_* definitions - used EVERYWHERE! */

#define NOT_A_PIN 255
#define NOT_A_PORT 255
#define NOT_AN_INTERRUPT 255

#define PA 0
#define PB 1
#define PC 2
#define PD 3
#define PE 4
#define PF 5
#define PG 6
#define NUM_TOTAL_PORTS 7

// These are used for two things: identifying the timer on a pin
// and for the MILLIS_TIMER define that the uses can test which timer
// actually being used for millis is actually being used
// Reasoning these constants are what they are:
// Low 3 bits are the number of that peripheral
// other bits specify the type of timer
// TCA=0x10, TCB=0x20, TCD=0x40, DAC=0x80, RTC=0x90
// Things that aren't hardware timers with output compare are after that
// DAC output isn't a timer, but has to be treated as such by PINMODE

#define NOT_ON_TIMER 0x00
#define TIMERA0 0x10
#define TIMERB0 0x20
#define TIMERB1 0x21
#define TIMERB2 0x22        // Not present on any tinyAVR 0/1/2-series
#define TIMERB3 0x23        // Not present on any tinyAVR 0/1/2-series
#define TIMERB4 0x24        // Not present on any tinyAVR 0/1/2-series
#define TIMERD0 0x40        // 1-series only
#define DACOUT 0x80         // 1-series only. "PWM" output source only
#define TIMERRTC 0x90       // millis timing source only
#define TIMERRTC_XTAL 0x91  // 1/2-series only, millis timing source only


#if MEGATINYCORE_SERIES == 2
/* Initial version of the IO headers omits these definitions!
 * Either that, or the datasheet includes them in error. The calibration
 * numbers are... maybe not the most useful values though, as the oscillator
 * error is very small. These are in the 0 to 4 range. Which is to be expected
 * since the granularityy of the calbyte is half what it was on the 0/1-series.
 */
  #if !defined(SIGROW_OSC16ERR3V)
    #define SIGROW_OSC16ERR3V (SIGROW.reserved_3[0])
  #endif
  #if !defined(SIGROW_OSC16ERR5V)
    #define SIGROW_OSC16ERR5V (SIGROW.reserved_3[1])
  #endif
  #if !defined(SIGROW_OSC20ERR3V)
    #define SIGROW_OSC20ERR3V (SIGROW.reserved_3[2])
  #endif
  #if !defined(SIGROW_OSC20ERR5V)
    #define SIGROW_OSC20ERR5V (SIGROW.reserved_3[3])
  #endif
#endif



// This define can get black-hole'ed somehow (reported on platformio) likely the ugly syntax to pass a string define from platform.txt via a -D
// directive passed to the compiler is getting mangled somehow, though I'm amazed it doesn't cause a  compile error. But checking for defined(MEGATINYCORE)
// is the documented method to detect that megaTinyCore is in use, and without it things that tried to do conditional compilation based on that were not
// recognizing it as megaTinyCore and hence would fail to compile when that conditional compilation was required to make it build.
// From: https://github.com/adafruit/Adafruit_BusIO/issues/43
#ifndef MEGATINYCORE
  #define MEGATINYCORE "Unknown 2.3.0+"
#endif

// Version related defines now handled in platform.txt
#define MEGATINYCORE_NUM ((MEGATINYCORE_MAJOR<<24)+(MEGATINYCORE_MINOR<<16)+(MEGATINYCORE_PATCH<<8)+MEGATINYCORE_RELEASED)



 /* HARDWARE FEATURES - Used by #ifdefs and as constants in calculations in
  * the core and in libraries; it is hoped that these are at least somewhat
  * useful to users, as well. These are described in more detail in the
  * README. */

#if (MEGATINYCORE_SERIES==2)
  #define ADC_NATIVE_RESOLUTION 12
  #define ADC_NATIVE_RESOLUTION_LOW 8
  // 1 if hardware has a differential ADC - separate from core support
  #define ADC_DIFFERENTIAL 1
  // 0 if hardware has no PGA, otherwise maximum PGA gain
  #define ADC_MAXIMUM_GAIN 16
  // Maximum burst accumulation
  #define ADC_MAXIMUM_ACCUMULATE 1024
  // Maximum SAMPLEN or SAMPDUR
  #define ADC_MAXIMUM_SAMPDUR 0xFF
  // ADC Result Size (bits)
  #define ADC_RESULT_SIZE 32
#else
  #define ADC_NATIVE_RESOLUTION 10
  #define ADC_NATIVE_RESOLUTION_LOW 8
  // 1 if hardware has a differential ADC - separate from core support
  #define ADC_DIFFERENTIAL 0
  // 0 if hardware has no PGA, otherwise maximum PGA gain
  #define ADC_MAXIMUM_GAIN 0
  // Maximum burst accumulation
  #define ADC_MAXIMUM_ACCUMULATE 64
  // Maximum SAMPLEN or SAMPDUR
  #define ADC_MAXIMUM_SAMPDUR 0x1F
  // ADC Result Size (bits)
  #define ADC_RESULT_SIZE 16
  // if (ADC_NATIVE_RESOLUTION + Log2(ADC_MAXIMUM_ACCUMULATE)) > ADC_RESULT_SIZE, long accumulations are truncated.
  // with maximum accumulation of 0/1, for example, 10 + 6 = 16, so it will just fit.
  // On 2-series, 12 + 10 = 22 out of 32 bits; they could have gotten away with a 24-bit register.
#endif
// tinyAVR thus far doesn't have the TTL input level option. Probably MVIO only.
#define PORT_HAS_INLVL 0
