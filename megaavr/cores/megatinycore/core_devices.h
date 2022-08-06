/* Core Parts - a part of Arduino.h for megaTinyCore 2.3.0 and later
 * This is directly included by Arduino.h and nothing else; it just moves
 * clutter out of that file.
 *
 * Spence Konde 2021 - megaTinyCore is free software (LGPL 2.1)
 * See LICENSE.txt for full legal boilerplate if you must */

#include <avr/io.h>
#include <core_parameters.h>
//#defines to identify part families
#if defined(__AVR_ATtiny3227__)
  #define MEGATINYCORE_MCU 3227
  #define MEGATINYCORE_SERIES 2
  #define __AVR_ATtinyx27__
  #define __AVR_ATtinyxy7__
  #define _AVR_FLASH__ 32
#elif defined(__AVR_ATtiny1627__)
  #define MEGATINYCORE_MCU 1627
  #define MEGATINYCORE_SERIES 2
  #define __AVR_ATtinyx27__
  #define __AVR_ATtinyxy7__
  #define _AVR_FLASH__ 16
#elif defined(__AVR_ATtiny827__)
  #define MEGATINYCORE_MCU 827
  #define MEGATINYCORE_SERIES 2
  #define __AVR_ATtinyx27__
  #define __AVR_ATtinyxy7__
  #define _AVR_FLASH__ 8
#elif defined(__AVR_ATtiny427__)
  #define MEGATINYCORE_MCU 427
  #define MEGATINYCORE_SERIES 2
  #define __AVR_ATtinyx27__
  #define __AVR_ATtinyxy7__
  #define _AVR_FLASH__ 4
#elif defined(__AVR_ATtiny3226__)
  #define MEGATINYCORE_MCU 3226
  #define MEGATINYCORE_SERIES 2
  #define __AVR_ATtinyx26__
  #define __AVR_ATtinyxy6__
  #define _AVR_FLASH__ 32
#elif defined(__AVR_ATtiny1626__)
  #define MEGATINYCORE_MCU 1626
  #define MEGATINYCORE_SERIES 2
  #define __AVR_ATtinyx26__
  #define __AVR_ATtinyxy6__
  #define _AVR_FLASH__ 16
#elif defined(__AVR_ATtiny826__)
  #define MEGATINYCORE_MCU 826
  #define MEGATINYCORE_SERIES 2
  #define __AVR_ATtinyx26__
  #define __AVR_ATtinyxy6__
  #define _AVR_FLASH__ 8
#elif defined(__AVR_ATtiny426__)
  #define MEGATINYCORE_MCU 426
  #define MEGATINYCORE_SERIES 2
  #define __AVR_ATtinyx26__
  #define __AVR_ATtinyxy6__
  #define _AVR_FLASH__ 4
#elif defined(__AVR_ATtiny3224__)
  #define MEGATINYCORE_MCU 3224
  #define MEGATINYCORE_SERIES 2
  #define __AVR_ATtinyx24__
  #define __AVR_ATtinyxy4__
  #define _AVR_FLASH__ 32
#elif defined(__AVR_ATtiny1624__)
  #define MEGATINYCORE_MCU 1624
  #define MEGATINYCORE_SERIES 2
  #define __AVR_ATtinyx24__
  #define __AVR_ATtinyxy4__
  #define _AVR_FLASH__ 16
#elif defined(__AVR_ATtiny824__)
  #define MEGATINYCORE_MCU 824
  #define MEGATINYCORE_SERIES 2
  #define __AVR_ATtinyx24__
  #define __AVR_ATtinyxy4__
  #define _AVR_FLASH__ 8
#elif defined(__AVR_ATtiny424__)
  #define MEGATINYCORE_MCU 424
  #define MEGATINYCORE_SERIES 2
  #define __AVR_ATtinyx24__
  #define __AVR_ATtinyxy4__
  #define _AVR_FLASH__ 4
#elif defined(__AVR_ATti4ny3217__)
  #define MEGATINYCORE_MCU 3217
  #define MEGATINYCORE_SERIES 1
  #define __AVR_ATtinyx17__
  #define __AVR_ATtinyxy7__
  #define _AVR_FLASH__ 32
#elif defined(__AVR_ATtiny1617__)
  #define MEGATINYCORE_MCU 1617
  #define MEGATINYCORE_SERIES 1
  #define __AVR_ATtinyx17__
  #define __AVR_ATtinyxy7__
  #define _AVR_FLASH__ 2
#elif defined(__AVR_ATtiny817__)
  #define MEGATINYCORE_MCU 817
  #define MEGATINYCORE_SERIES 1
  #define __AVR_ATtinyx17__
  #define __AVR_ATtinyxy7__
  #define _AVR_FLASH__ 2
#elif defined(__AVR_ATtiny417__)
  #define MEGATINYCORE_MCU 417
  #define MEGATINYCORE_SERIES 1
  #define __AVR_ATtinyx17__
  #define __AVR_ATtinyxy7__
  #define _AVR_FLASH__ 2
#elif defined(__AVR_ATtiny3207__)
  #define MEGATINYCORE_MCU 3207
  #define MEGATINYCORE_SERIES 0
  #define __AVR_ATtinyx07__
  #define __AVR_ATtinyxy7__
  #define _AVR_FLASH__ 2
#elif defined(__AVR_ATtiny1607__)
  #define MEGATINYCORE_MCU 1607
  #define MEGATINYCORE_SERIES 0
  #define __AVR_ATtinyx07__
  #define __AVR_ATtinyxy7__
  #define _AVR_FLASH__ 2
#elif defined(__AVR_ATtiny807__)
  #define MEGATINYCORE_MCU 807
  #define MEGATINYCORE_SERIES 0
  #define __AVR_ATtinyx07__
  #define __AVR_ATtinyxy7__
  #define _AVR_FLASH__ 2
#elif defined(__AVR_ATtiny407__)
  #define MEGATINYCORE_MCU 407
  #define MEGATINYCORE_SERIES 0
  #define __AVR_ATtinyx07__
  #define __AVR_ATtinyxy7__
  #define _AVR_FLASH__ 2
#elif defined(__AVR_ATtiny3216__)
  #define MEGATINYCORE_MCU 3216
  #define MEGATINYCORE_SERIES 1
  #define __AVR_ATtinyx06__
  #define __AVR_ATtinyxy6__
  #define _AVR_FLASH__ 2
#elif defined(__AVR_ATtiny1616__)
  #define MEGATINYCORE_MCU 1616
  #define MEGATINYCORE_SERIES 1
  #define __AVR_ATtinyx06__
  #define __AVR_ATtinyxy6__
  #define _AVR_FLASH__ 2
#elif defined(__AVR_ATtiny816__)
  #define MEGATINYCORE_MCU 816
  #define MEGATINYCORE_SERIES 1
  #define __AVR_ATtinyx06__
  #define __AVR_ATtinyxy6__
  #define _AVR_FLASH__ 2
#elif defined(__AVR_ATtiny416__)
  #define MEGATINYCORE_MCU 416
  #define MEGATINYCORE_SERIES 1
  #define __AVR_ATtinyx16__
  #define __AVR_ATtinyxy6__
  #define _AVR_FLASH__ 2
#elif defined(__AVR_ATtiny1606__)
  #define MEGATINYCORE_MCU 1606
  #define MEGATINYCORE_SERIES 0
  #define __AVR_ATtinyx06__
  #define __AVR_ATtinyxy6__
  #define _AVR_FLASH__ 2
#elif defined(__AVR_ATtiny806__)
  #define MEGATINYCORE_MCU 806
  #define MEGATINYCORE_SERIES 0
  #define __AVR_ATtinyx06__
  #define __AVR_ATtinyxy6__
  #define _AVR_FLASH__ 2
#elif defined(__AVR_ATtiny406__)
  #define MEGATINYCORE_MCU 406
  #define MEGATINYCORE_SERIES 0
  #define __AVR_ATtinyx06__
  #define __AVR_ATtinyxy6__
  #define _AVR_FLASH__ 2
#elif defined(__AVR_ATtiny214__)
  #define MEGATINYCORE_MCU 214
  #define MEGATINYCORE_SERIES 1
  #define __AVR_ATtinyx14__
  #define __AVR_ATtinyxy4__
  #define _AVR_FLASH__ 2
#elif defined(__AVR_ATtiny1614__)
  #define MEGATINYCORE_MCU 1614
  #define MEGATINYCORE_SERIES 1
  #define __AVR_ATtinyx14__
  #define __AVR_ATtinyxy4__
  #define _AVR_FLASH__ 2
#elif defined(__AVR_ATtiny814__)
  #define MEGATINYCORE_MCU 814
  #define MEGATINYCORE_SERIES 1
  #define __AVR_ATtinyx14__
  #define __AVR_ATtinyxy4__
  #define _AVR_FLASH__ 2
#elif defined(__AVR_ATtiny414__)
  #define MEGATINYCORE_MCU 414
  #define MEGATINYCORE_SERIES 1
  #define __AVR_ATtinyx14__
  #define __AVR_ATtinyxy4__
  #define _AVR_FLASH__ 2
#elif defined(__AVR_ATtiny204__)
  #define MEGATINYCORE_MCU 204
  #define MEGATINYCORE_SERIES 0
  #define __AVR_ATtinyx04__
  #define __AVR_ATtinyxy4__
  #define _AVR_FLASH__ 2
#elif defined(__AVR_ATtiny804__)
  #define MEGATINYCORE_MCU 804
  #define MEGATINYCORE_SERIES 0
  #define __AVR_ATtinyx04__
  #define __AVR_ATtinyxy4__
  #define _AVR_FLASH__ 2
#elif defined(__AVR_ATtiny404__)
  #define MEGATINYCORE_MCU 404
  #define MEGATINYCORE_SERIES 0
  #define __AVR_ATtinyx04__
  #define __AVR_ATtinyxy4__
  #define _AVR_FLASH__ 2
#elif defined(__AVR_ATtiny1604__)
  #define MEGATINYCORE_MCU 1604
  #define MEGATINYCORE_SERIES 0
  #define __AVR_ATtinyx04__
  #define __AVR_ATtinyxy4__
  #define _AVR_FLASH__ 2
#elif defined(__AVR_ATtiny212__)
  #define MEGATINYCORE_MCU 212
  #define MEGATINYCORE_SERIES 1
  #define __AVR_ATtinyx12__
  #define __AVR_ATtinyxy2__
  #define _AVR_FLASH__ 2
#elif defined(__AVR_ATtiny412__)
  #define MEGATINYCORE_MCU 412
  #define MEGATINYCORE_SERIES 1
  #define __AVR_ATtinyx12__
  #define __AVR_ATtinyxy2__
  #define _AVR_FLASH__ 2
#elif defined(__AVR_ATtiny202__)
  #define MEGATINYCORE_MCU 202
  #define MEGATINYCORE_SERIES 0
  #define __AVR_ATtinyx02__
  #define __AVR_ATtinyxy2__
  #define _AVR_FLASH__ 2
#elif defined(__AVR_ATtiny402__)
  #define MEGATINYCORE_MCU 402
  #define MEGATINYCORE_SERIES 0
  #define __AVR_ATtinyx02__
  #define __AVR_ATtinyxy2__
  #define _AVR_FLASH__ 2
#else
  #error "Can't-happen: unknown chip somehow being used!"
#endif

#if MEGATINYCORE_SERIES == 0
  #define _AVR_FAMILY "T0"
  #define __AVR_TINY_0__
#elif MEGATINYCORE_SERIES == 1
  #define _AVR_FAMILY "T1"
  #define __AVR_TINY_1__
#elif MEGATINYCORE_SERIES == 2
  #define _AVR_FAMILY "T2"
  #define __AVR_TINY_2__
#endif

#if   defined(__AVR_ATtinyxy7__)
  #define HAS_24_PINS
  #define _AVR_PINCOUUNT (24)
#elif defined(__AVR_ATtinyxy6__)
  #define HAS_20_PINS
  #define _AVR_PINCOUUNT (20)
#elif defined(__AVR_ATtinyxy4__)
  #define HAS_14_PINS
  #define _AVR_PINCOUUNT (14)
#elif defined(__AVR_ATtinyxy2__)
  #define HAS_8_PINS
  #define _AVR_PINCOUUNT (8)
#else
  #error "Unrecognized number of pins"
#endif

#if defined(AC2)
  #define _AVR_AC_COUNT      (3)
#elif defined(AC1)
  #define _AVR_AC_COUNT      (2)
#elif defined(AC0)
  #define _AVR_AC_COUNT      (1)
#else
  #define _AVR_AC_COUNT      (0)
#endif

#if defined(ADC1)
  #define _AVR_ADC_COUNT     (2)
#elif defined(ADC0)
  #define _AVR_ADC_COUNT     (1)
#else
  #define _AVR_ADC_COUNT     (0)
  #warning "No ADC? No supported parts exist, something is wrong"
#endif


#if defined(OPAMP2)
  #define _AVR_OPAMP_COUNT   (3)
#elif defined(OPAMP1)
  #define _AVR_OPAMP_COUNT   (2)
#elif defined(OPAMP0)
  #define _AVR_OPAMP_COUNT   (1)
#else
  #define _AVR_OPAMP_COUNT   (0)
#endif


#if defined(CCL_TRUTH4)
  #define _AVR_LUT_COUNT     (6)
#elif defined(CCL_TRUTH2)
  #define _AVR_LUT_COUNT     (4)
#elif defined(CCL_TRUTH0)
  #define _AVR_LUT_COUNT     (2)
#else
  #warning "No AC? No supported parts exist, something is wrong"
#endif


#if defined(TCA1)
  #define _AVR_TCA_COUNT     (2)
#elif defined(TCA0)
  #define _AVR_TCA_COUNT     (1)
#else
  #warning "No TCA? No supported parts exist, something is wrong"
#endif

#if defined(TCB5)
  #define _AVR_TCB_COUNT     (6)
#elif defined(TCB4)
  #define _AVR_TCB_COUNT     (5)
#elif defined(TCB3)
  #define _AVR_TCB_COUNT     (4)
#elif defined(TCB2)
  #define _AVR_TCB_COUNT     (3)
#elif defined(TCB1)
  #define _AVR_TCB_COUNT     (2)
#elif defined(TCB0)
  #define _AVR_TCB_COUNT     (1)
#else
  #warning "No TCBs? No such parts exist, something is wrong"
#endif


#if defined(TCD1)
  #define _AVR_TCD_COUNT     (1)
#else
  #define _AVR_TCD_COUNT     (0)
#endif

#if defined(TWI1)
  #define _AVR_TWI_COUNT     (2)
#elif defined(TWI0)
  #define _AVR_TWI_COUNT     (1)
#else
  #define _AVR_TWI_COUNT     (0)
  #warning "No TWI? No supported parts like that exist..."
#endif

#if defined(USART5)
  #define _AVR_USART_COUNT     (6)
#elif defined(USART4)
  #define _AVR_USART_COUNT     (5)
#elif defined(USART3)
  #define _AVR_USART_COUNT     (4)
#elif defined(USART2)
  #define _AVR_USART_COUNT     (3)
#elif defined(USART1)
  #define _AVR_USART_COUNT     (2)
#elif defined(USART0)
  #define _AVR_USART_COUNT     (1)
#else
  #warning "No USARTs? No such parts exist, something is wrong";
#endif




/* Used for AttachInterrupt to reduce RAM use. */
#ifdef __AVR_ATtinyxy4__
  #define PORTB_PINS 4
  #define _AVR_PINCOUNT 14
#endif
#ifdef __AVR_ATtinyxy6__
  #define PORTB_PINS 6
  #define PORTC_PINS 4
  #define _AVR_PINCOUNT 20
#endif
#ifdef __AVR_ATtinyxy7__
  #define PORTB_PINS 8
  #define PORTC_PINS 6
  #define _AVR_PINCOUNT 24
#endif
#ifdef __AVR_ATtinyxy2__
  #define _AVR_PINCOUNT 8
#endif

// #define OUTPUT_PULLUP 3  // not implemented

/* PORT names and the NOT_A_* definitions - used EVERYWHERE! */

#define NOT_A_PIN 255
#define NOT_A_PORT 255
#define NOT_AN_INTERRUPT 255
#define NOT_A_MUX 255
#define MUX_NONE 128

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
#define TIMERA1 0x08        // Not present on any tinyAVR devices
#define TIMERB0 0x20
#define TIMERB1 0x21
#define TIMERB2 0x22        // Not present on any tinyAVR 0/1/2-series
#define TIMERB3 0x23        // Not present on any tinyAVR 0/1/2-series
#define TIMERB4 0x24        // Not present on any tinyAVR 0/1/2-series
#define TIMERD0 0x40        // 1-series only
#define DACOUT 0x80         // 1-series only. "PWM" output source only
#define TIMERRTC 0x90       // millis timing source only
#define TIMERRTC_XTAL 0x91  // 1/2-series only, millis timing source only
#define TIMERRTC_XOSC 0x92  // 1/2-series only, millis timing source only

#if MEGATINYCORE_SERIES <= 2
  #define RTC_CLKSEL_OSC32K_gc  RTC_CLKSEL_INT32K_gc
  #define RTC_CLKSEL_OSC1K_gc   RTC_CLKSEL_INT1K_gc
  #define RTC_CLKSEL_XTAL32K_gc RTC_CLKSEL_TOSC32K_gc
#endif

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
#else
  /* 0/1-series, on the other hand, doesm't have these.... */
  #if !defined(SIGROW_OSCCAL16M0)
    #define SIGROW_OSCCAL16M0 _SFR_MEM8(0x1118)
  #endif
  #if !defined(SIGROW_OSCCAL16M1)
    #define SIGROW_OSCCAL16M1 _SFR_MEM8(0x1119)
  #endif
  #if !defined(SIGROW_OSCCAL20M0)
    #define SIGROW_OSCCAL20M0 _SFR_MEM8(0x111A)
  #endif
  #if !defined(SIGROW_OSCCAL20M1)
    #define SIGROW_OSCCAL20M1 _SFR_MEM8(0x111B)
  #endif
  // Also, the 0/1-series have useless names for the event channels. But we can define the 2-series names for them!
  // This makes it easier to write portable code without using the event library and incurring that overhead.

  #if defined(EVSYS_SYNCUSER0)
    #define EVSYS_USERTCA0CNTA EVSYS_SYNCUSER0
  #endif
  #if defined(EVSYS_SYNCUSER1)
    #define EVSYS_USERUSART0IRDA EVSYS_SYNCUSER1
  #endif
  #if defined(EVSYS_ASYNCUSER0)
    #define EVSYS_USERTCB0CAPT EVSYS_ASYNCUSER0
  #endif
  #if defined(EVSYS_ASYNCUSER1)
    #define EVSYS_USERADC0START EVSYS_ASYNCUSER1
  #endif
  #if defined(EVSYS_ASYNCUSER2)
    #define EVSYS_USERCCLLUT0A EVSYS_ASYNCUSER2
  #endif
  #if defined(EVSYS_ASYNCUSER3)
    #define EVSYS_USERCCLLUT1A EVSYS_ASYNCUSER3
  #endif
  #if defined(EVSYS_ASYNCUSER4)
    #define EVSYS_USERCCLLUT0B EVSYS_ASYNCUSER4
  #endif
  #if defined(EVSYS_ASYNCUSER5)
    #define EVSYS_USERCCLLUT1B EVSYS_ASYNCUSER5
  #endif
  #if defined(EVSYS_ASYNCUSER6)
    #define EVSYS_USERTCD0INPUTA EVSYS_ASYNCUSER6
  #endif
  #if defined(EVSYS_ASYNCUSER7)
    #define EVSYS_USERTCD0INPUTB EVSYS_ASYNCUSER7
  #endif
  #if defined(EVSYS_ASYNCUSER8)
    #define EVSYS_USEREVSYSEVOUTA EVSYS_ASYNCUSER8
  #endif
  #if defined(EVSYS_ASYNCUSER9)
    #define EVSYS_USEREVSYSEVOUTB EVSYS_ASYNCUSER9
  #endif
  #if defined(EVSYS_ASYNCUSER10)
    #define EVSYS_USEREVSYSEVOUTC EVSYS_ASYNCUSER10
  #endif
  #if defined(EVSYS_ASYNCUSER11)
    #define EVSYS_USERTCB1CAPT EVSYS_ASYNCUSER11
  #endif
  #if defined(EVSYS_ASYNCUSER12)
    #define EVSYS_USERADC1START EVSYS_ASYNCUSER12
  #endif

#endif

#define CLOCK_TUNE_START (USER_SIGNATURES_SIZE - 12)

/* Add a feature - yay!
 * Rename registers so people can't carry code back and forth - booo!
 */
#ifndef TCA_SINGLE_CNTEI_bm
  #define TCA_SINGLE_CNTEI_bm TCA_SINGLE_CNTAEI_bm
  #define TCA_SINGLE_EVACT_POSEDGE_gc TCA_SINGLE_EVACTA_CNT_POSEDGE_gc
  #define TCA_SINGLE_EVACT_CNT_ANYEDGE_gc TCA_SINGLE_EVACTA_CNT_ANYEDGE_gc
  #define TCA_SINGLE_EVACT_CNT_HIGHLVL_gc TCA_SINGLE_EVACTA_CNT_HIGHLVL_gc
  #define TCA_SINGLE_EVACT_UPDOWN_gc TCA_SINGLE_EVACTA_UPDOWN_gc
  #define TCB_CLKSEL_CLKDIV1_gc TCB_CLKSEL_DIV1_gc
  #define TCB_CLKSEL_CLKDIV2_gc TCB_CLKSEL_DIV2_gc
  #define TCB_CLKSEL_CLKTCA_gc TCB_CLKSEL_TCA0_gc
#else
  #define TCB_CLKSEL_DIV1_gc  TCB_CLKSEL_CLKDIV1_gc
  #define TCB_CLKSEL_DIV2_gc  TCB_CLKSEL_CLKDIV2_gc
  #define TCB_CLKSEL_TCA0_gc  TCB_CLKSEL_CLKTCA_gc
  #define TCB_CLKSEL_EVENT_gc  ({badArg("This processor does not support TCB count on event mode. Only Dx, Ex, and 2-series tiny support that");})
#endif

/* Make sure we error out quickly if told to use an RTC timing option that isn't available. */
#if (defined(MILLIS_USE_TIMERRTC_XTAL) || defined(MILLIS_USE_TIMERRTC_XOSC))
  #if (MEGATINYCORE_SERIES == 0 || defined(__AVR_ATtinyxy2__))
    #error "Only the tinyAVR 1-series and 2-series parts with at least 8 pins support external RTC timebase"
  #endif
#endif

 /* HARDWARE FEATURES - Used by #ifdefs and as constants in calculations in
  * the core and in libraries; it is hoped that these are at least somewhat
  * useful to users, as well. These are described in more detail in the
  * README. */

#if (MEGATINYCORE_SERIES == 2)
  #define ADC_NATIVE_RESOLUTION 12
  #define ADC_NATIVE_RESOLUTION_LOW 8
  // 1 if hardware has a differential ADC - separate from core support
  #define ADC_DIFFERENTIAL 1
  // 0 if hardware has no PGA, otherwise maximum PGA gain
  #define ADC_MAXIMUM_GAIN 16
  // Maximum burst accumulation
  #define ADC_MAXIMUM_ACCUMULATE 1024
  // Maximum resolution obtainable by using maximum accumulate option and decimating that.
  #define ADC_MAX_OVERSAMPLED_RESOLUTION 17
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
  // Maximum resolution obtainable by using maximum accumulate option and decimating that.
  #define ADC_MAX_OVERSAMPLED_RESOLUTION 13
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
