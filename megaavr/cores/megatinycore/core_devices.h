/* core_devices  - a part of Arduino.h for megaTinyCore 2.3.0 and later and DxCore 1.4.0, but
 * which takes ion much greater importance (and girth) with the toolchain update in 2.6 and 1.6)
 * This is directly included by Arduino.h and nothing else; it just moves
 * clutter out of that file. You should not directly include this file ever.
 *
 * Spence Konde 2021 -2022- megaTinyCore and DxCore are free software (LGPL 2.1)
 * See LICENSE.txt for full legal boilerplate if you must */

#ifndef Core_Devices_h
#define Core_Devices_h
#include <avr/io.h>
#include <core_parameters.h>
#define PROGMEM_MAPPED // All modern tinyAVRs have mapped flash.
//#defines to identify part families
#if   defined(__AVR_ATtiny3227__)
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
#elif defined(__AVR_ATtiny3217__)
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
#define MEGATINYCORE_NUM (((MEGATINYCORE_MAJOR << 24)+(MEGATINYCORE_MINOR << 16)+(MEGATINYCORE_PATCH << 8)+MEGATINYCORE_RELEASED))

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

#if defined(AC2)
  #define _AVR_AC_COUNT      (3)
#elif defined(AC1)
  #define _AVR_AC_COUNT      (2)
#elif defined(AC0)
  #define _AVR_AC_COUNT      (1)
#else
  #define _AVR_AC_COUNT      (0)
  #error "No AC? No supported parts exist without one, something is wrong"
#endif

#if defined(ADC1)
  #define _AVR_ADC_COUNT     (2)
#elif defined(ADC0)
  #define _AVR_ADC_COUNT     (1)
#else
  #define _AVR_ADC_COUNT     (0)
  #error "No ADC? No supported parts exist without one, something is wrong"
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
  #warning "No CCL? No supported parts exist without one, something is wrong"
#endif


#if defined(TCA1)
  #define _AVR_TCA_COUNT     (2)
#elif defined(TCA0)
  #define _AVR_TCA_COUNT     (1)
#else
  #error "No TCA? No supported parts exist without one, something is wrong"
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
  #error "No TCBs? No supported parts exist without one, something is wrong"
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
  #error "No TWI? No supported parts exist without one, something is wrong"
#endif

#if defined(SPI1)
  #define _AVR_SPI_COUNT     (2)
#elif defined(SPI0)
  #define _AVR_SPI_COUNT     (1)
#else
  #error "No SPI? No supported parts exist without one, something is wrong"
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
  #error "No USARTs? No supported parts exist without one, something is wrong"
#endif

#if defined(ZCD3)
  #define _AVR_ZCD_COUNT     (1) /* Only the DD's have ZCD3, which is their ZCD0 by a different name, since it uses different pins */
#elif defined(ZCD2)
  #define _AVR_ZCD_COUNT     (3)
#elif defined(ZCD1)
  #define _AVR_ZCD_COUNT     (2)
#elif defined(ZCD0)
  #define _AVR_ZCD_COUNT     (1)
#else
  #define _AVR_ZCD_COUNT     (0)
#endif

#if defined(DAC2)
  #define _AVR_DAC_COUNT     (3)
#elif defined(DAC1)
  #define _AVR_DAC_COUNT     (2)
#elif defined(DAC0)
  #define _AVR_DAC_COUNT     (1) /* Note that thus far, no DAC other than DAC0 has ever been able to output data. DAC1 and DAC2 are just the DACREFs for AC1 and AC2 on tinyAVR 1-series parts.*/
#else
  #define _AVR_DAC_COUNT     (0)
#endif


/* PORT names and the NOT_A_* definitions - used EVERYWHERE! */

#define NOT_A_PIN 255
#define NOT_A_PORT 255
#define NOT_AN_INTERRUPT 255
#define NOT_A_MUX 255
#define MUX_NONE 128

#if defined(MEGATINYCORE)
  #define PA 0
  #if _AVR_PINCOUNT > 8
    #define PB 1
    #if _AVR_PINCOUNT > 14
      #define PC 2
      #define NUM_TOTAL_PORTS 3
    #else
      #define PC NOT_A_PORT
      #define NUM_TOTAL_PORTS 2
    #endif
  #else
    #define PB NOT_A_PORT
    #define PC NOT_A_PORT
    #define NUM_TOTAL_PORTS 1
  #endif
  #define PD NOT_A_PORT
  #define PE NOT_A_PORT
  #define PF NOT_A_PORT
  #define PG NOT_A_PORT
#else
  #define PA 0
  #if defined(PORTB)
    #define PB 1
  #else
    #define PB NOT_A_PORT
  #endif
  #define PC 2
  #define PD 3
  #if defined(PORTE)
    #define PE 4
  #else
    #define PE NOT_A_PORT
  #endif
  #define PF 5
  #if defined(PORTG)
    #define PG 6
    #define NUM_TOTAL_PORTS 7
  #else
    #define PG NOT_A_PORT
    #define NUM_TOTAL_PORTS 6
  #endif
#endif



/* In case you need to greatly change the operating frequency of the chip on the fly, and accept that it will trash millis and so on.
 * That's why it's not documented or in Arduino.h, but this subfile - it's about as blunt an instrument as it gets, and resorting to
 * methods like this can result in bizarre and unexpected behavior that can be difficult to diagnose if you don't suspect clock issues
 *
 * Yes, the nomenclature is strange (I'm refering to the "x"; commonly used for multiplication yet the mathematical opperation in question
 * is divison. The symbol for that is not valid in names in C). I didn't start it, but there's no obviously better option, so X it stays.
 */
#define  _setPrescale2x()         (_PROTECTED_WRITE(CLKCTRL_MCLKCTRLB, ( CLKCTRL_PDIV_2X_gc | CLKCTRL_PEN_bm)))
#define  _setPrescale4x()         (_PROTECTED_WRITE(CLKCTRL_MCLKCTRLB, ( CLKCTRL_PDIV_4X_gc | CLKCTRL_PEN_bm)))
#define  _setPrescale8x()         (_PROTECTED_WRITE(CLKCTRL_MCLKCTRLB, ( CLKCTRL_PDIV_8X_gc | CLKCTRL_PEN_bm)))
#define _setPrescale16x()         (_PROTECTED_WRITE(CLKCTRL_MCLKCTRLB, (CLKCTRL_PDIV_16X_gc | CLKCTRL_PEN_bm)))
#define _setPrescale32x()         (_PROTECTED_WRITE(CLKCTRL_MCLKCTRLB, (CLKCTRL_PDIV_32X_gc | CLKCTRL_PEN_bm)))
#define _setPrescale64x()         (_PROTECTED_WRITE(CLKCTRL_MCLKCTRLB, (CLKCTRL_PDIV_64X_gc | CLKCTRL_PEN_bm)))
#define  _setPrescale6x()         (_PROTECTED_WRITE(CLKCTRL_MCLKCTRLB, ( CLKCTRL_PDIV_6X_gc | CLKCTRL_PEN_bm)))
#define _setPrescale10x()         (_PROTECTED_WRITE(CLKCTRL_MCLKCTRLB, (CLKCTRL_PDIV_10X_gc | CLKCTRL_PEN_bm)))
#define _setPrescale12x()         (_PROTECTED_WRITE(CLKCTRL_MCLKCTRLB, (CLKCTRL_PDIV_12X_gc | CLKCTRL_PEN_bm)))
#define _setPrescale24x()         (_PROTECTED_WRITE(CLKCTRL_MCLKCTRLB, (CLKCTRL_PDIV_24X_gc | CLKCTRL_PEN_bm)))
#define _setPrescale48x()         (_PROTECTED_WRITE(CLKCTRL_MCLKCTRLB, (CLKCTRL_PDIV_48X_gc | CLKCTRL_PEN_bm)))



#if MEGATINYCORE_SERIES <= 2
  #define RTC_CLKSEL_OSC32K_gc  RTC_CLKSEL_INT32K_gc
  #define RTC_CLKSEL_OSC1K_gc   RTC_CLKSEL_INT1K_gc
  #define RTC_CLKSEL_XTAL32K_gc RTC_CLKSEL_TOSC32K_gc
#endif

#if MEGATINYCORE_SERIES == 2
/* Initial version of the IO headers omits these definitions!
 * then some versions had them, then the removed them all and stopped
 * performing the cal at all (which was not as usuful as on 0/1-series)
 */
 #if !defined(SIGROW_OSC16ERR3V)
    #define SIGROW_OSC16ERR3V (SIGROW.reserved_3[badCall("the SIGROW does not contain error information about the main oscillator except on very early versions")]
  #endif
  #if !defined(SIGROW_OSC16ERR5V)
    #define SIGROW_OSC16ERR5V (SIGROW.reserved_3[badCall("the SIGROW does not contain error information about the main oscillator except on very early versions")]
  #endif
  #if !defined(SIGROW_OSC20ERR3V)
    #define SIGROW_OSC20ERR3V (SIGROW.reserved_3[badCall("the SIGROW does not contain error information about the main oscillator except on very early versions")]
  #endif
  #if !defined(SIGROW_OSC20ERR5V)
    #define SIGROW_OSC20ERR5V (SIGROW.reserved_3[badCall("the SIGROW does not contain error information about the main oscillator except on very early versions")]
  #endif
#else
  /* 0/1-series, on the other hand, doesm't have these defined even though they are present */
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

/* Microchip has shown a tendency to rename registers bitfields and similar between product lines, even when the behavior is identical.
 * This is a major hindrance to writing highly portable code which I assume is what most people wish to do. It certainly beats having
 * to run code through find replace making trivial changes, forcing a fork where you would rather not have one.
 * Since having to adjust code to match the arbitrary and capricious whims of the header generation scheme kinda sucks, we try to catch
 * all the places they do this and provide a macro for backwards compatibility. For some bizarre reason you may wish to turn this off
 * maybe in preparation for jumping to another development environment like Microchip Studio that does not use Arduino cores.
 * Instead of backwards compatibilily, you want the opposite, which some wags have called "Backwards combatibility"
 * Defining BACKWARD_COMBATIBILITY_MODE turns off all of these definitions that paper over name changes.
 */

// #define BACKWARD_COMBATIBILITY_MODE

#if !defined(BACKWARD_COMBATIBILITY_MODE)
  // We default to seeking compatibility. for COMBATability you would uncomment that #define, and that turns all these off.
  /* Man they just *HAD* to change the names of these values that get assigned to the same register and do the same thing didn't they?
   * Worse still we can't even verify that they are present... just blindly define and pray. Enums can't be seen by macros
   */
   // tinyAVR has TOSC32K (tinyOscillator?)
  #if defined(MEGATINYCORE)
    #define RTC_CLKSEL_OSC32K_gc            RTC_CLKSEL_INT32K_gc
    #define RTC_CLKSEL_OSC1K_gc             RTC_CLKSEL_INT1K_gc
    #define RTC_CLKSEL_XTAL32K_gc           RTC_CLKSEL_TOSC32K_gc
    #define RTC_CLKSEL_XOSC32K_gc           RTC_CLKSEL_TOSC32K_gc
  #else
// Dx has an XOSC32K
    #define RTC_CLKSEL_INT32K_gc            RTC_CLKSEL_OSC32K_gc
    #define RTC_CLKSEL_INT1K_gc             RTC_CLKSEL_OSC1K_gc
    #define RTC_CLKSEL_TOSC32K_gc           RTC_CLKSEL_XOSC32K_gc
    #define RTC_CLKSEL_XTAL32K_gc           RTC_CLKSEL_XOSC32K_gc
  #endif
  /* General Purpose Register names, GPR.GPRn, vs GPIORn vs GPIOn
   * They now appear to have decided they don't like either of the previous conventions, one just a few years old. Now they are grouping
   * them under a "General Purpose Register". "peripheral". I cannot argue that GPR doesn't make more sense, as there's not really any
   * I/O occurring here (ofc they were referring to the IN and OUT instructions, which can be used on these), but I certainly wouldn't
   * have changed a convention like this, at least not when I had just done so a few years prior. */

  // All non-xmega pre-Dx-series modern AVR parts call them GPIORn instead of GPR.GPRn/GPR_GPRn
  // Some classics called them GPIOn.

  /* In one xMega AVR, they were GPIOn, rather than GPIORn
   * One? Yup: The ATxmega32d4. Not the 32d3, nor the 32e5, nor anything else. All the xmega's have GPIORs
   * and their headers list the GPIOn names too. But.... there is only a single header
   * file with them not marked as "Deprecated": ATxmega32D4
   * 24 of the 46 xmega parts with headers in the compiler packages (including the 32d3 and 32e5) had the
   * 4 GPIOR's that we have, and had GPIOn and GPIO_GPIOn present but marked as deprecated.
   * On those parts, these are at addresses 0x0000-0x003, and 0x0004-0x000F do not appear to be used.
   * The other 22.... had THE ENTIRE FIRST HALF OF THE LOW I/O SPACE as GPIOR0-GPIORF!
   * Which ones got all of them and which ones only got 4 seems to have been chosen in typical
   * Atmel fashion (in other words, randomly). No apparent pattern in time or other parameters.
   * Either way, that left them with space for only 4 VPORT register sets (like the ones we got)
   * These had to be configured to point to the desired port.
   * I'm sure everyone is grateful for the fact that the folks designing the Dx-series have their
   * heads screwed on properly and realized that 4 GPIOR-- excuse me, GPRs, 4 awkward VPORTs and
   * 12 unused addresses in the low I/O space was maybe not the best design decision made in the
   * xmega line, and decided that wasn't a winning formula */
  #if !defined(GPIOR0)
    #define GPIOR0                            (_SFR_MEM8(0x001C))
    #define GPIOR1                            (_SFR_MEM8(0x001D))
    #define GPIOR2                            (_SFR_MEM8(0x001E))
    #define GPIOR3                            (_SFR_MEM8(0x001F))
  #endif
  #if !defined(GPIO0)
    #define GPIO0                             (_SFR_MEM8(0x001C))
    #define GPIO1                             (_SFR_MEM8(0x001D))
    #define GPIO2                             (_SFR_MEM8(0x001E))
    #define GPIO3                             (_SFR_MEM8(0x001F))
  #endif
  #if !defined(GPIO_GPIOR0)
    #define GPIO_GPIO0                        (_SFR_MEM8(0x001C))
    #define GPIO_GPIO1                        (_SFR_MEM8(0x001D))
    #define GPIO_GPIO2                        (_SFR_MEM8(0x001E))
    #define GPIO_GPIO3                        (_SFR_MEM8(0x001F))
  #endif
  #if !defined(GPR_GPR0)
    #define GPR_GPR0                        (_SFR_MEM8(0x001C))
    #define GPR_GPR1                        (_SFR_MEM8(0x001D))
    #define GPR_GPR2                        (_SFR_MEM8(0x001E))
    #define GPR_GPR3                        (_SFR_MEM8(0x001F))
  #endif
 // The naming of this has gotten so confusing. I give up, we all know where the registers are.
  #if defined (CLKCTRL_SELHF_bm)
    /* They changed the damned name after selling the part for 6 months!
     * annoyingly you can't even test if it's using the new version of the headers because it's an enum! */
    #define CLKCTRL_SELHF_CRYSTAL_gc CLKCTRL_SELHF_XTAL_gc
  #endif
  /* And one version later they did it again... */
  #if !defined(CLKCTRL_FREQSEL_gm) && defined(CLKCTRL_FRQSEL_gm)
    #define CLKCTRL_FREQSEL_gm     (CLKCTRL_FRQSEL_gm)      /*    Group Mask                  */
    #define CLKCTRL_FREQSEL_gp     (CLKCTRL_FRQSEL_gp)      /*    Group Position              */
    //                                                      /*    Group Codes                 */
    #define CLKCTRL_FREQSEL_1M_gc  (CLKCTRL_FRQSEL_1M_gc)   /*  1 MHz system clock            */
    #define CLKCTRL_FREQSEL_2M_gc  (CLKCTRL_FRQSEL_2M_gc)   /*  2 MHz system clock            */
    #define CLKCTRL_FREQSEL_3M_gc  (CLKCTRL_FRQSEL_3M_gc)   /*  3 MHz system clock            */
    #define CLKCTRL_FREQSEL_4M_gc  (CLKCTRL_FRQSEL_4M_gc)   /*  4 MHz system clock default    */
    #define CLKCTRL_FREQSEL_8M_gc  (CLKCTRL_FRQSEL_8M_gc)   /*  8 MHz system clock            */
    #define CLKCTRL_FREQSEL_12M_gc (CLKCTRL_FRQSEL_12M_gc)  /* 12 MHz system clock            */
    #define CLKCTRL_FREQSEL_16M_gc (CLKCTRL_FRQSEL_16M_gc)  /* 16 MHz system clock            */
    #define CLKCTRL_FREQSEL_20M_gc (CLKCTRL_FRQSEL_20M_gc)  /* 20 MHz system clock            */
    #define CLKCTRL_FREQSEL_24M_gc (CLKCTRL_FRQSEL_24M_gc)  /* 24 MHz system clock            */
    #define CLKCTRL_FREQSEL_28M_gc (CLKCTRL_FRQSEL_28M_gc)  /* 28 MHz system clock unofficial - this will just error out if used since it will replace one undefined symbol with another */
    #define CLKCTRL_FREQSEL_32M_gc (CLKCTRL_FRQSEL_32M_gc)  /* 32 MHz system clock unofficial - this will just error out if used since it will replace one undefined symbol with another */
  #elif defined(CLKCTRL_FRQSEL_gm) && !defined(CLKCTRL_FREQSEL_gm)
    #define CLKCTRL_FRQSEL_gm     (CLKCTRL_FREQSEL_gm)      /*    Group Mask                  */
    #define CLKCTRL_FRQSEL_gp     (CLKCTRL_FREQSEL_gp)      /*    Group Position              */
    //                                                      /*    Group Codes                 */
    #define CLKCTRL_FRQSEL_1M_gc  (CLKCTRL_FREQSEL_1M_gc)   /*  1 MHz system clock            */
    #define CLKCTRL_FRQSEL_2M_gc  (CLKCTRL_FREQSEL_2M_gc)   /*  2 MHz system clock            */
    #define CLKCTRL_FRQSEL_3M_gc  (CLKCTRL_FREQSEL_3M_gc)   /*  3 MHz system clock            */
    #define CLKCTRL_FRQSEL_4M_gc  (CLKCTRL_FREQSEL_4M_gc)   /*  4 MHz system clock default    */
    #define CLKCTRL_FRQSEL_8M_gc  (CLKCTRL_FREQSEL_8M_gc)   /*  8 MHz system clock            */
    #define CLKCTRL_FRQSEL_12M_gc (CLKCTRL_FREQSEL_12M_gc)  /* 12 MHz system clock            */
    #define CLKCTRL_FRQSEL_16M_gc (CLKCTRL_FREQSEL_16M_gc)  /* 16 MHz system clock            */
    #define CLKCTRL_FRQSEL_20M_gc (CLKCTRL_FREQSEL_20M_gc)  /* 20 MHz system clock            */
    #define CLKCTRL_FRQSEL_24M_gc (CLKCTRL_FREQSEL_24M_gc)  /* 24 MHz system clock            */
    #define CLKCTRL_FRQSEL_28M_gc (CLKCTRL_FREQSEL_28M_gc)  /* 28 MHz system clock unofficial - this will just error out if used since it will replace one undefined symbol with another */
    #define CLKCTRL_FRQSEL_32M_gc (CLKCTRL_FREQSEL_32M_gc)  /* 32 MHz system clock unofficial - this will just error out if used since it will replace one undefined symbol with another */
  #endif

  /* Add a feature - yay!
   * Rename registers so people can't carry code back and forth - booo!
   */
  // TCA V1.0 - tinyAVR 0/1, megaAVR 0
  // this only has one event input, but code needs to be able to flow smoothly
  // so we define macros named after he the new version pointing to the old version of event input A.
  // Obviously, we can't do anything about the unfortunate soul who tries to use input B.
  #if !defined(TCA_SINGLE_CNTAEI_bm)
    #define TCA_SINGLE_CNTAEI_bm TCA_SINGLE_CNTEI_bm
    #define TCA_SINGLE_EVACTA_POSEDGE_gc TCA_SINGLE_EVACTA_CNT_POSEDGE_gc
    #define TCA_SINGLE_EVACTA_CNT_ANYEDGE_gc TCA_SINGLE_EVACTA_CNT_ANYEDGE_gc
    #define TCA_SINGLE_EVACTA_CNT_HIGHLVL_gc TCA_SINGLE_EVACTA_CNT_HIGHLVL_gc
    #define TCA_SINGLE_EVACTA_UPDOWN_gc TCA_SINGLE_EVACTA_UPDOWN_gc
  #endif
  // TCA V1.1 - DA, DB, tinyAVR 2?
  //  with two inputs changes the names the existing ones to specify channel A
  // We add in the non-postfixed ana
  #if !defined(TCA_SINGLE_CNTEI_bm)
    #define _TCA_
    #define TCA_SINGLE_CNTEI_bm TCA_SINGLE_CNTAEI_bm
    #define TCA_SINGLE_EVACT_POSEDGE_gc TCA_SINGLE_EVACTA_CNT_POSEDGE_gc
    #define TCA_SINGLE_EVACT_CNT_ANYEDGE_gc TCA_SINGLE_EVACTA_CNT_ANYEDGE_gc
    #define TCA_SINGLE_EVACT_CNT_HIGHLVL_gc TCA_SINGLE_EVACTA_CNT_HIGHLVL_gc
    #define TCA_SINGLE_EVACT_UPDOWN_gc TCA_SINGLE_EVACTA_UPDOWN_gc
  #endif

  #if (!defined(MEGATINYCORE) || MEGATINYCORE_SERIES >= 2)
    #define TCB_CLKSEL_CLKDIV1_gc TCB_CLKSEL_DIV1_gc
    #define TCB_CLKSEL_CLKDIV2_gc TCB_CLKSEL_DIV2_gc
    #define TCB_CLKSEL_CLKTCA_gc TCB_CLKSEL_TCA0_gc
  #else
    #define TCB_CLKSEL_DIV1_gc  TCB_CLKSEL_CLKDIV1_gc
    #define TCB_CLKSEL_DIV2_gc  TCB_CLKSEL_CLKDIV2_gc
    #define TCB_CLKSEL_TCA0_gc  TCB_CLKSEL_CLKTCA_gc
    #define TCB_CLKSEL_EVENT_gc  (badCall("This processor does not support TCB count on event mode. Only Dx, Ex, and 2-series tiny support that"))
  #endif
  /* Make sure we error out quickly if told to use an RTC timing option that isn't available. */
  #if (defined(MILLIS_USE_TIMERRTC_XTAL) || defined(MILLIS_USE_TIMERRTC_XOSC))
    #if (MEGATINYCORE_SERIES == 0 || defined(__AVR_ATtinyxy2__))
      #error "Only the tinyAVR 1-series and 2-series parts with at least 14 pins support external RTC timebase"
    #endif
  #endif
  // And now, it it appears that they realized that they should have had some sort of delimiter between the bit number within a bitfield, and the name of the bitfield, since the names of many bitfields end in numbers,
  // So they went ahead and made that change. Without any compatibility layer.

  // Well, I'd wanted to make deprecation warnings come up only if they were used. I was unuccessful.

  // typedef const uint8_t __attribute__ ((deprecated("\nMicrochip changed the spelling of bits within a bitfiels (macros that end in the bitnumber followed by _bm or _bp), you are using the old name, ex PERIPH_BITFIRLD1_bm.\nYou should use PERIPH_BITFIELD_1_bm; we do not guarantee that this 4000-line bandaid will not be removed in the future.\r\nWhy did they do this? Beats me. Ask their support folks - if enough of us do it, they might hesitate next time they have the urge to mass rename things in their headers")))  deprecated_constant_name;

  // Okay, well that fix didn't work so well. back to plan A.
  /* ======= ACs ======= */
  #if !defined(AC_HYSMODE_0_bm) && defined(AC_HYSMODE0_bm)
    #define AC_HYSMODE_0_bm AC_HYSMODE0_bm
  #elif defined(AC_HYSMODE_0_bm)
    //deprecated_constant_name AC_HYSMODE0_bm = AC_HYSMODE_0_bm;
    #define AC_HYSMODE0_bm AC_HYSMODE_0_bm //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(AC_HYSMODE_0_bp) && defined(AC_HYSMODE0_bp)
    #define AC_HYSMODE_0_bp AC_HYSMODE0_bp
  #elif defined(AC_HYSMODE_0_bp)
    //deprecated_constant_name AC_HYSMODE0_bp = AC_HYSMODE_0_bp;
    #define AC_HYSMODE0_bp AC_HYSMODE_0_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(AC_HYSMODE_1_bm) && defined(AC_HYSMODE1_bm)
    #define AC_HYSMODE_1_bm AC_HYSMODE1_bm
  #elif defined(AC_HYSMODE_1_bm)
    //deprecated_constant_name AC_HYSMODE1_bm = AC_HYSMODE_1_bm;
    #define AC_HYSMODE1_bm AC_HYSMODE_1_bm //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(AC_HYSMODE_1_bp) && defined(AC_HYSMODE1_bp)
    #define AC_HYSMODE_1_bp AC_HYSMODE1_bp
  #elif defined(AC_HYSMODE_1_bp)
    //deprecated_constant_name AC_HYSMODE1_bp = AC_HYSMODE_1_bp;
    #define AC_HYSMODE1_bp AC_HYSMODE_1_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(AC_POWER_0_bm) && defined(AC_POWER0_bm)
    #define AC_POWER_0_bm AC_POWER0_bm
  #elif defined(AC_POWER_0_bm)
    //deprecated_constant_name AC_POWER0_bm = AC_POWER_0_bm;
    #define AC_POWER0_bm AC_POWER_0_bm //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(AC_POWER_0_bp) && defined(AC_POWER0_bp)
    #define AC_POWER_0_bp AC_POWER0_bp
  #elif defined(AC_POWER_0_bp)
    //deprecated_constant_name AC_POWER0_bp = AC_POWER_0_bp;
    #define AC_POWER0_bp AC_POWER_0_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(AC_POWER_1_bm) && defined(AC_POWER1_bm)
    #define AC_POWER_1_bm AC_POWER1_bm
  #elif defined(AC_POWER_1_bm)
    //deprecated_constant_name AC_POWER1_bm = AC_POWER_1_bm;
    #define AC_POWER1_bm AC_POWER_1_bm //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(AC_POWER_1_bp) && defined(AC_POWER1_bp)
    #define AC_POWER_1_bp AC_POWER1_bp
  #elif defined(AC_POWER_1_bp)
    //deprecated_constant_name AC_POWER1_bp = AC_POWER_1_bp;
    #define AC_POWER1_bp AC_POWER_1_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(AC_WINSEL_0_bm) && defined(AC_WINSEL0_bm)
    #define AC_WINSEL_0_bm AC_WINSEL0_bm
  #elif defined(AC_WINSEL_0_bm)
    //deprecated_constant_name AC_WINSEL0_bm = AC_WINSEL_0_bm;
    #define AC_WINSEL0_bm AC_WINSEL_0_bm //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(AC_WINSEL_0_bp) && defined(AC_WINSEL0_bp)
    #define AC_WINSEL_0_bp AC_WINSEL0_bp
  #elif defined(AC_WINSEL_0_bp)
    //deprecated_constant_name AC_WINSEL0_bp = AC_WINSEL_0_bp;
    #define AC_WINSEL0_bp AC_WINSEL_0_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(AC_WINSEL_1_bm) && defined(AC_WINSEL1_bm)
    #define AC_WINSEL_1_bm AC_WINSEL1_bm
  #elif defined(AC_WINSEL_1_bm)
    //deprecated_constant_name AC_WINSEL1_bm = AC_WINSEL_1_bm;
    #define AC_WINSEL1_bm AC_WINSEL_1_bm //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(AC_WINSEL_1_bp) && defined(AC_WINSEL1_bp)
    #define AC_WINSEL_1_bp AC_WINSEL1_bp
  #elif defined(AC_WINSEL_1_bp)
    //deprecated_constant_name AC_WINSEL1_bp = AC_WINSEL_1_bp;
    #define AC_WINSEL1_bp AC_WINSEL_1_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(AC_MUXNEG_0_bm) && defined(AC_MUXNEG0_bm)
    #define AC_MUXNEG_0_bm AC_MUXNEG0_bm
  #elif defined(AC_MUXNEG_0_bm)
    //deprecated_constant_name AC_MUXNEG0_bm = AC_MUXNEG_0_bm;
    #define AC_MUXNEG0_bm AC_MUXNEG_0_bm //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(AC_MUXNEG_0_bp) && defined(AC_MUXNEG0_bp)
    #define AC_MUXNEG_0_bp AC_MUXNEG0_bp
  #elif defined(AC_MUXNEG_0_bp)
    //deprecated_constant_name AC_MUXNEG0_bp = AC_MUXNEG_0_bp;
    #define AC_MUXNEG0_bp AC_MUXNEG_0_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(AC_MUXNEG_1_bm) && defined(AC_MUXNEG1_bm)
    #define AC_MUXNEG_1_bm AC_MUXNEG1_bm
  #elif defined(AC_MUXNEG_1_bm)
    //deprecated_constant_name AC_MUXNEG1_bm = AC_MUXNEG_1_bm;
    #define AC_MUXNEG1_bm AC_MUXNEG_1_bm //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(AC_MUXNEG_1_bp) && defined(AC_MUXNEG1_bp)
    #define AC_MUXNEG_1_bp AC_MUXNEG1_bp
  #elif defined(AC_MUXNEG_1_bp)
    //deprecated_constant_name AC_MUXNEG1_bp = AC_MUXNEG_1_bp;
    #define AC_MUXNEG1_bp AC_MUXNEG_1_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(AC_MUXNEG_2_bm) && defined(AC_MUXNEG2_bm)
    #define AC_MUXNEG_2_bm AC_MUXNEG2_bm
  #elif defined(AC_MUXNEG_2_bm)
    //deprecated_constant_name AC_MUXNEG2_bm = AC_MUXNEG_2_bm;
    #define AC_MUXNEG2_bm AC_MUXNEG_2_bm //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(AC_MUXNEG_2_bp) && defined(AC_MUXNEG2_bp)
    #define AC_MUXNEG_2_bp AC_MUXNEG2_bp
  #elif defined(AC_MUXNEG_2_bp)
    //deprecated_constant_name AC_MUXNEG2_bp = AC_MUXNEG_2_bp;
    #define AC_MUXNEG2_bp AC_MUXNEG_2_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(AC_MUXPOS_0_bm) && defined(AC_MUXPOS0_bm)
    #define AC_MUXPOS_0_bm AC_MUXPOS0_bm
  #elif defined(AC_MUXPOS_0_bm)
    //deprecated_constant_name AC_MUXPOS0_bm = AC_MUXPOS_0_bm;
    #define AC_MUXPOS0_bm AC_MUXPOS_0_bm //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(AC_MUXPOS_0_bp) && defined(AC_MUXPOS0_bp)
    #define AC_MUXPOS_0_bp AC_MUXPOS0_bp
  #elif defined(AC_MUXPOS_0_bp)
    //deprecated_constant_name AC_MUXPOS0_bp = AC_MUXPOS_0_bp;
    #define AC_MUXPOS0_bp AC_MUXPOS_0_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(AC_MUXPOS_1_bm) && defined(AC_MUXPOS1_bm)
    #define AC_MUXPOS_1_bm AC_MUXPOS1_bm
  #elif defined(AC_MUXPOS_1_bm)
    //deprecated_constant_name AC_MUXPOS1_bm = AC_MUXPOS_1_bm;
    #define AC_MUXPOS1_bm AC_MUXPOS_1_bm //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(AC_MUXPOS_1_bp) && defined(AC_MUXPOS1_bp)
    #define AC_MUXPOS_1_bp AC_MUXPOS1_bp
  #elif defined(AC_MUXPOS_1_bp)
    //deprecated_constant_name AC_MUXPOS1_bp = AC_MUXPOS_1_bp;
    #define AC_MUXPOS1_bp AC_MUXPOS_1_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(AC_MUXPOS_2_bm) && defined(AC_MUXPOS2_bm)
    #define AC_MUXPOS_2_bm AC_MUXPOS2_bm
  #elif defined(AC_MUXPOS_2_bm)
    //deprecated_constant_name AC_MUXPOS2_bm = AC_MUXPOS_2_bm;
    #define AC_MUXPOS2_bm AC_MUXPOS_2_bm //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(AC_MUXPOS_2_bp) && defined(AC_MUXPOS2_bp)
    #define AC_MUXPOS_2_bp AC_MUXPOS2_bp
  #elif defined(AC_MUXPOS_2_bp)
    //deprecated_constant_name AC_MUXPOS2_bp = AC_MUXPOS_2_bp;
    #define AC_MUXPOS2_bp AC_MUXPOS_2_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(AC_INTMODE_NORMAL_0_bm) && defined(AC_INTMODE_NORMAL0_bm)
    #define AC_INTMODE_NORMAL_0_bm AC_INTMODE_NORMAL0_bm
  #elif defined(AC_INTMODE_NORMAL_0_bm)
    //deprecated_constant_name AC_INTMODE_NORMAL0_bm = AC_INTMODE_NORMAL_0_bm;
    #define AC_INTMODE_NORMAL0_bm AC_INTMODE_NORMAL_0_bm //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(AC_INTMODE_NORMAL_0_bp) && defined(AC_INTMODE_NORMAL0_bp)
    #define AC_INTMODE_NORMAL_0_bp AC_INTMODE_NORMAL0_bp
  #elif defined(AC_INTMODE_NORMAL_0_bp)
    //deprecated_constant_name AC_INTMODE_NORMAL0_bp = AC_INTMODE_NORMAL_0_bp;
    #define AC_INTMODE_NORMAL0_bp AC_INTMODE_NORMAL_0_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(AC_INTMODE_NORMAL_1_bm) && defined(AC_INTMODE_NORMAL1_bm)
    #define AC_INTMODE_NORMAL_1_bm AC_INTMODE_NORMAL1_bm
  #elif defined(AC_INTMODE_NORMAL_1_bm)
    //deprecated_constant_name AC_INTMODE_NORMAL1_bm = AC_INTMODE_NORMAL_1_bm;
    #define AC_INTMODE_NORMAL1_bm AC_INTMODE_NORMAL_1_bm //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(AC_INTMODE_NORMAL_1_bp) && defined(AC_INTMODE_NORMAL1_bp)
    #define AC_INTMODE_NORMAL_1_bp AC_INTMODE_NORMAL1_bp
  #elif defined(AC_INTMODE_NORMAL_1_bp)
    //deprecated_constant_name AC_INTMODE_NORMAL1_bp = AC_INTMODE_NORMAL_1_bp;
    #define AC_INTMODE_NORMAL1_bp AC_INTMODE_NORMAL_1_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(AC_INTMODE_WINDOW_0_bm) && defined(AC_INTMODE_WINDOW0_bm)
    #define AC_INTMODE_WINDOW_0_bm AC_INTMODE_WINDOW0_bm
  #elif defined(AC_INTMODE_WINDOW_0_bm)
    //deprecated_constant_name AC_INTMODE_WINDOW0_bm = AC_INTMODE_WINDOW_0_bm;
    #define AC_INTMODE_WINDOW0_bm AC_INTMODE_WINDOW_0_bm //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(AC_INTMODE_WINDOW_0_bp) && defined(AC_INTMODE_WINDOW0_bp)
    #define AC_INTMODE_WINDOW_0_bp AC_INTMODE_WINDOW0_bp
  #elif defined(AC_INTMODE_WINDOW_0_bp)
    //deprecated_constant_name AC_INTMODE_WINDOW0_bp = AC_INTMODE_WINDOW_0_bp;
    #define AC_INTMODE_WINDOW0_bp AC_INTMODE_WINDOW_0_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(AC_INTMODE_WINDOW_1_bm) && defined(AC_INTMODE_WINDOW1_bm)
    #define AC_INTMODE_WINDOW_1_bm AC_INTMODE_WINDOW1_bm
  #elif defined(AC_INTMODE_WINDOW_1_bm)
    //deprecated_constant_name AC_INTMODE_WINDOW1_bm = AC_INTMODE_WINDOW_1_bm;
    #define AC_INTMODE_WINDOW1_bm AC_INTMODE_WINDOW_1_bm //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(AC_INTMODE_WINDOW_1_bp) && defined(AC_INTMODE_WINDOW1_bp)
    #define AC_INTMODE_WINDOW_1_bp AC_INTMODE_WINDOW1_bp
  #elif defined(AC_INTMODE_WINDOW_1_bp)
    //deprecated_constant_name AC_INTMODE_WINDOW1_bp = AC_INTMODE_WINDOW_1_bp;
    #define AC_INTMODE_WINDOW1_bp AC_INTMODE_WINDOW_1_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(AC_WINSTATE_0_bm) && defined(AC_WINSTATE0_bm)
    #define AC_WINSTATE_0_bm AC_WINSTATE0_bm
  #elif defined(AC_WINSTATE_0_bm)
    //deprecated_constant_name AC_WINSTATE0_bm = AC_WINSTATE_0_bm;
    #define AC_WINSTATE0_bm AC_WINSTATE_0_bm //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(AC_WINSTATE_0_bp) && defined(AC_WINSTATE0_bp)
    #define AC_WINSTATE_0_bp AC_WINSTATE0_bp
  #elif defined(AC_WINSTATE_0_bp)
    //deprecated_constant_name AC_WINSTATE0_bp = AC_WINSTATE_0_bp;
    #define AC_WINSTATE0_bp AC_WINSTATE_0_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(AC_WINSTATE_1_bm) && defined(AC_WINSTATE1_bm)
    #define AC_WINSTATE_1_bm AC_WINSTATE1_bm
  #elif defined(AC_WINSTATE_1_bm)
    //deprecated_constant_name AC_WINSTATE1_bm = AC_WINSTATE_1_bm;
    #define AC_WINSTATE1_bm AC_WINSTATE_1_bm //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(AC_WINSTATE_1_bp) && defined(AC_WINSTATE1_bp)
    #define AC_WINSTATE_1_bp AC_WINSTATE1_bp
  #elif defined(AC_WINSTATE_1_bp)
    //deprecated_constant_name AC_WINSTATE1_bp = AC_WINSTATE_1_bp;
    #define AC_WINSTATE1_bp AC_WINSTATE_1_bp //Deprecated as of Q2 2022 header change.
  #endif

  /* ======= ADC ======= */
  // alter the way the header's compatibility layer works to actually *show* the deprecation warning.
  #if !defined(ADC_RESSEL_0_bm) && defined(ADC_RESSEL0_bm)
    #define ADC_RESSEL_0_bm ADC_RESSEL0_bm
  #elif defined(ADC_RESSEL_0_bm)
    #undef ADC_RESSEL0_bm /* this special case does have a compatibility layer, but the deprecation is only provided by a comement so users will not be warned that the compatibility layer may be removed in the future. So we undefine in and handle as above */
    //deprecated_constant_name ADC_RESSEL0_bm = ADC_RESSEL_0_bm;
    #define ADC_RESSEL0_bm ADC_RESSEL_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(ADC_RESSEL_0_bp) && defined(ADC_RESSEL0_bp)
    #define ADC_RESSEL_0_bp ADC_RESSEL0_bp
  #elif defined(ADC_RESSEL_0_bp)
    #undef ADC_RESSEL0_bp /* this special case does have a compatibility layer, but the deprecation is only provided by a comement so users will not be warned that the compatibility layer may be removed in the future. So we undefine in and handle as above */
    //deprecated_constant_name ADC_RESSEL0_bp = ADC_RESSEL_0_bp;
    #define ADC_RESSEL0_bp ADC_RESSEL_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(ADC_RESSEL_1_bm) && defined(ADC_RESSEL1_bm)
    #define ADC_RESSEL_1_bm ADC_RESSEL1_bm
  #elif defined(ADC_RESSEL_1_bm)
    #undef ADC_RESSEL1_bm /* this special case does have a compatibility layer, but the deprecation is only provided by a comement so users will not be warned that the compatibility layer may be removed in the future. So we undefine in and handle as above */
    //deprecated_constant_name ADC_RESSEL1_bm = ADC_RESSEL_1_bm;
    #define ADC_RESSEL1_bm ADC_RESSEL_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(ADC_RESSEL_1_bp) && defined(ADC_RESSEL1_bp)
    #define ADC_RESSEL_1_bp ADC_RESSEL1_bp
  #elif defined(ADC_RESSEL_1_bp)
    #undef ADC_RESSEL1_bp /* this special case does have a compatibility layer, but the deprecation is only provided by a comement so users will not be warned that the compatibility layer may be removed in the future. So we undefine in and handle as above */
    //deprecated_constant_name ADC_RESSEL1_bp = ADC_RESSEL_1_bp;
    #define ADC_RESSEL1_bp ADC_RESSEL_1_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(ADC_SAMPNUM_0_bm) && defined(ADC_SAMPNUM0_bm)
    #define ADC_SAMPNUM_0_bm ADC_SAMPNUM0_bm
  #elif defined(ADC_SAMPNUM_0_bm)
    //deprecated_constant_name ADC_SAMPNUM0_bm = ADC_SAMPNUM_0_bm;
    #define ADC_SAMPNUM0_bm ADC_SAMPNUM_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(ADC_SAMPNUM_0_bp) && defined(ADC_SAMPNUM0_bp)
    #define ADC_SAMPNUM_0_bp ADC_SAMPNUM0_bp
  #elif defined(ADC_SAMPNUM_0_bp)
    //deprecated_constant_name ADC_SAMPNUM0_bp = ADC_SAMPNUM_0_bp;
    #define ADC_SAMPNUM0_bp ADC_SAMPNUM_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(ADC_SAMPNUM_1_bm) && defined(ADC_SAMPNUM1_bm)
    #define ADC_SAMPNUM_1_bm ADC_SAMPNUM1_bm
  #elif defined(ADC_SAMPNUM_1_bm)
    //deprecated_constant_name ADC_SAMPNUM1_bm = ADC_SAMPNUM_1_bm;
    #define ADC_SAMPNUM1_bm ADC_SAMPNUM_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(ADC_SAMPNUM_1_bp) && defined(ADC_SAMPNUM1_bp)
    #define ADC_SAMPNUM_1_bp ADC_SAMPNUM1_bp
  #elif defined(ADC_SAMPNUM_1_bp)
    //deprecated_constant_name ADC_SAMPNUM1_bp = ADC_SAMPNUM_1_bp;
    #define ADC_SAMPNUM1_bp ADC_SAMPNUM_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(ADC_SAMPNUM_2_bm) && defined(ADC_SAMPNUM2_bm)
    #define ADC_SAMPNUM_2_bm ADC_SAMPNUM2_bm
  #elif defined(ADC_SAMPNUM_2_bm)
    //deprecated_constant_name ADC_SAMPNUM2_bm = ADC_SAMPNUM_2_bm;
    #define ADC_SAMPNUM2_bm ADC_SAMPNUM_2_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(ADC_SAMPNUM_2_bp) && defined(ADC_SAMPNUM2_bp)
    #define ADC_SAMPNUM_2_bp ADC_SAMPNUM2_bp
  #elif defined(ADC_SAMPNUM_2_bp)
    //deprecated_constant_name ADC_SAMPNUM2_bp = ADC_SAMPNUM_2_bp;
    #define ADC_SAMPNUM2_bp ADC_SAMPNUM_2_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(ADC_PRESC_0_bm) && defined(ADC_PRESC0_bm)
    #define ADC_PRESC_0_bm ADC_PRESC0_bm
  #elif defined(ADC_PRESC_0_bm)
    //deprecated_constant_name ADC_PRESC0_bm = ADC_PRESC_0_bm;
    #define ADC_PRESC0_bm ADC_PRESC_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(ADC_PRESC_0_bp) && defined(ADC_PRESC0_bp)
    #define ADC_PRESC_0_bp ADC_PRESC0_bp
  #elif defined(ADC_PRESC_0_bp)
    //deprecated_constant_name ADC_PRESC0_bp = ADC_PRESC_0_bp;
    #define ADC_PRESC0_bp ADC_PRESC_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(ADC_PRESC_1_bm) && defined(ADC_PRESC1_bm)
    #define ADC_PRESC_1_bm ADC_PRESC1_bm
  #elif defined(ADC_PRESC_1_bm)
    //deprecated_constant_name ADC_PRESC1_bm = ADC_PRESC_1_bm;
    #define ADC_PRESC1_bm ADC_PRESC_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(ADC_PRESC_1_bp) && defined(ADC_PRESC1_bp)
    #define ADC_PRESC_1_bp ADC_PRESC1_bp
  #elif defined(ADC_PRESC_1_bp)
    //deprecated_constant_name ADC_PRESC1_bp = ADC_PRESC_1_bp;
    #define ADC_PRESC1_bp ADC_PRESC_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(ADC_PRESC_2_bm) && defined(ADC_PRESC2_bm)
    #define ADC_PRESC_2_bm ADC_PRESC2_bm
  #elif defined(ADC_PRESC_2_bm)
    //deprecated_constant_name ADC_PRESC2_bm = ADC_PRESC_2_bm;
    #define ADC_PRESC2_bm ADC_PRESC_2_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(ADC_PRESC_2_bp) && defined(ADC_PRESC2_bp)
    #define ADC_PRESC_2_bp ADC_PRESC2_bp
  #elif defined(ADC_PRESC_2_bp)
    //deprecated_constant_name ADC_PRESC2_bp = ADC_PRESC_2_bp;
    #define ADC_PRESC2_bp ADC_PRESC_2_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(ADC_PRESC_3_bm) && defined(ADC_PRESC3_bm)
    #define ADC_PRESC_3_bm ADC_PRESC3_bm
  #elif defined(ADC_PRESC_3_bm)
    //deprecated_constant_name ADC_PRESC3_bm = ADC_PRESC_3_bm;
    #define ADC_PRESC3_bm ADC_PRESC_3_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(ADC_PRESC_3_bp) && defined(ADC_PRESC3_bp)
    #define ADC_PRESC_3_bp ADC_PRESC3_bp
  #elif defined(ADC_PRESC_3_bp)
    //deprecated_constant_name ADC_PRESC3_bp = ADC_PRESC_3_bp;
    #define ADC_PRESC3_bp ADC_PRESC_3_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(ADC_SAMPDLY_0_bm) && defined(ADC_SAMPDLY0_bm)
    #define ADC_SAMPDLY_0_bm ADC_SAMPDLY0_bm
  #elif defined(ADC_SAMPDLY_0_bm)
    //deprecated_constant_name ADC_SAMPDLY0_bm = ADC_SAMPDLY_0_bm;
    #define ADC_SAMPDLY0_bm ADC_SAMPDLY_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(ADC_SAMPDLY_0_bp) && defined(ADC_SAMPDLY0_bp)
    #define ADC_SAMPDLY_0_bp ADC_SAMPDLY0_bp
  #elif defined(ADC_SAMPDLY_0_bp)
    //deprecated_constant_name ADC_SAMPDLY0_bp = ADC_SAMPDLY_0_bp;
    #define ADC_SAMPDLY0_bp ADC_SAMPDLY_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(ADC_SAMPDLY_1_bm) && defined(ADC_SAMPDLY1_bm)
    #define ADC_SAMPDLY_1_bm ADC_SAMPDLY1_bm
  #elif defined(ADC_SAMPDLY_1_bm)
    //deprecated_constant_name ADC_SAMPDLY1_bm = ADC_SAMPDLY_1_bm;
    #define ADC_SAMPDLY1_bm ADC_SAMPDLY_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(ADC_SAMPDLY_1_bp) && defined(ADC_SAMPDLY1_bp)
    #define ADC_SAMPDLY_1_bp ADC_SAMPDLY1_bp
  #elif defined(ADC_SAMPDLY_1_bp)
    //deprecated_constant_name ADC_SAMPDLY1_bp = ADC_SAMPDLY_1_bp;
    #define ADC_SAMPDLY1_bp ADC_SAMPDLY_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(ADC_SAMPDLY_2_bm) && defined(ADC_SAMPDLY2_bm)
    #define ADC_SAMPDLY_2_bm ADC_SAMPDLY2_bm
  #elif defined(ADC_SAMPDLY_2_bm)
    //deprecated_constant_name ADC_SAMPDLY2_bm = ADC_SAMPDLY_2_bm;
    #define ADC_SAMPDLY2_bm ADC_SAMPDLY_2_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(ADC_SAMPDLY_2_bp) && defined(ADC_SAMPDLY2_bp)
    #define ADC_SAMPDLY_2_bp ADC_SAMPDLY2_bp
  #elif defined(ADC_SAMPDLY_2_bp)
    //deprecated_constant_name ADC_SAMPDLY2_bp = ADC_SAMPDLY_2_bp;
    #define ADC_SAMPDLY2_bp ADC_SAMPDLY_2_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(ADC_SAMPDLY_3_bm) && defined(ADC_SAMPDLY3_bm)
    #define ADC_SAMPDLY_3_bm ADC_SAMPDLY3_bm
  #elif defined(ADC_SAMPDLY_3_bm)
    //deprecated_constant_name ADC_SAMPDLY3_bm = ADC_SAMPDLY_3_bm;
    #define ADC_SAMPDLY3_bm ADC_SAMPDLY_3_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(ADC_SAMPDLY_3_bp) && defined(ADC_SAMPDLY3_bp)
    #define ADC_SAMPDLY_3_bp ADC_SAMPDLY3_bp
  #elif defined(ADC_SAMPDLY_3_bp)
    //deprecated_constant_name ADC_SAMPDLY3_bp = ADC_SAMPDLY_3_bp;
    #define ADC_SAMPDLY3_bp ADC_SAMPDLY_3_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(ADC_INITDLY_0_bm) && defined(ADC_INITDLY0_bm)
    #define ADC_INITDLY_0_bm ADC_INITDLY0_bm
  #elif defined(ADC_INITDLY_0_bm)
    //deprecated_constant_name ADC_INITDLY0_bm = ADC_INITDLY_0_bm;
    #define ADC_INITDLY0_bm ADC_INITDLY_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(ADC_INITDLY_0_bp) && defined(ADC_INITDLY0_bp)
    #define ADC_INITDLY_0_bp ADC_INITDLY0_bp
  #elif defined(ADC_INITDLY_0_bp)
    //deprecated_constant_name ADC_INITDLY0_bp = ADC_INITDLY_0_bp;
    #define ADC_INITDLY0_bp ADC_INITDLY_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(ADC_INITDLY_1_bm) && defined(ADC_INITDLY1_bm)
    #define ADC_INITDLY_1_bm ADC_INITDLY1_bm
  #elif defined(ADC_INITDLY_1_bm)
    //deprecated_constant_name ADC_INITDLY1_bm = ADC_INITDLY_1_bm;
    #define ADC_INITDLY1_bm ADC_INITDLY_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(ADC_INITDLY_1_bp) && defined(ADC_INITDLY1_bp)
    #define ADC_INITDLY_1_bp ADC_INITDLY1_bp
  #elif defined(ADC_INITDLY_1_bp)
    //deprecated_constant_name ADC_INITDLY1_bp = ADC_INITDLY_1_bp;
    #define ADC_INITDLY1_bp ADC_INITDLY_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(ADC_INITDLY_2_bm) && defined(ADC_INITDLY2_bm)
    #define ADC_INITDLY_2_bm ADC_INITDLY2_bm
  #elif defined(ADC_INITDLY_2_bm)
    //deprecated_constant_name ADC_INITDLY2_bm = ADC_INITDLY_2_bm;
    #define ADC_INITDLY2_bm ADC_INITDLY_2_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(ADC_INITDLY_2_bp) && defined(ADC_INITDLY2_bp)
    #define ADC_INITDLY_2_bp ADC_INITDLY2_bp
  #elif defined(ADC_INITDLY_2_bp)
    //deprecated_constant_name ADC_INITDLY2_bp = ADC_INITDLY_2_bp;
    #define ADC_INITDLY2_bp ADC_INITDLY_2_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(ADC_WINCM_0_bm) && defined(ADC_WINCM0_bm)
    #define ADC_WINCM_0_bm ADC_WINCM0_bm
  #elif defined(ADC_WINCM_0_bm)
    //deprecated_constant_name ADC_WINCM0_bm = ADC_WINCM_0_bm;
    #define ADC_WINCM0_bm ADC_WINCM_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(ADC_WINCM_0_bp) && defined(ADC_WINCM0_bp)
    #define ADC_WINCM_0_bp ADC_WINCM0_bp
  #elif defined(ADC_WINCM_0_bp)
    //deprecated_constant_name ADC_WINCM0_bp = ADC_WINCM_0_bp;
    #define ADC_WINCM0_bp ADC_WINCM_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(ADC_WINCM_1_bm) && defined(ADC_WINCM1_bm)
    #define ADC_WINCM_1_bm ADC_WINCM1_bm
  #elif defined(ADC_WINCM_1_bm)
    //deprecated_constant_name ADC_WINCM1_bm = ADC_WINCM_1_bm;
    #define ADC_WINCM1_bm ADC_WINCM_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(ADC_WINCM_1_bp) && defined(ADC_WINCM1_bp)
    #define ADC_WINCM_1_bp ADC_WINCM1_bp
  #elif defined(ADC_WINCM_1_bp)
    //deprecated_constant_name ADC_WINCM1_bp = ADC_WINCM_1_bp;
    #define ADC_WINCM1_bp ADC_WINCM_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(ADC_WINCM_2_bm) && defined(ADC_WINCM2_bm)
    #define ADC_WINCM_2_bm ADC_WINCM2_bm
  #elif defined(ADC_WINCM_2_bm)
    //deprecated_constant_name ADC_WINCM2_bm = ADC_WINCM_2_bm;
    #define ADC_WINCM2_bm ADC_WINCM_2_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(ADC_WINCM_2_bp) && defined(ADC_WINCM2_bp)
    #define ADC_WINCM_2_bp ADC_WINCM2_bp
  #elif defined(ADC_WINCM_2_bp)
    //deprecated_constant_name ADC_WINCM2_bp = ADC_WINCM_2_bp;
    #define ADC_WINCM2_bp ADC_WINCM_2_bp //Deprecated as of Q2 2022 header change.
  #endif

  /* ======= BOD ======= */
  #if !defined(BOD_SLEEP_0_bm) && defined(BOD_SLEEP0_bm)
    #define BOD_SLEEP_0_bm BOD_SLEEP0_bm
  #elif defined(BOD_SLEEP_0_bm)
    //deprecated_constant_name BOD_SLEEP0_bm = BOD_SLEEP_0_bm;
    #define BOD_SLEEP0_bm BOD_SLEEP_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(BOD_SLEEP_0_bp) && defined(BOD_SLEEP0_bp)
    #define BOD_SLEEP_0_bp BOD_SLEEP0_bp
  #elif defined(BOD_SLEEP_0_bp)
    //deprecated_constant_name BOD_SLEEP0_bp = BOD_SLEEP_0_bp;
    #define BOD_SLEEP0_bp BOD_SLEEP_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(BOD_SLEEP_1_bm) && defined(BOD_SLEEP1_bm)
    #define BOD_SLEEP_1_bm BOD_SLEEP1_bm
  #elif defined(BOD_SLEEP_1_bm)
    //deprecated_constant_name BOD_SLEEP1_bm = BOD_SLEEP_1_bm;
    #define BOD_SLEEP1_bm BOD_SLEEP_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(BOD_SLEEP_1_bp) && defined(BOD_SLEEP1_bp)
    #define BOD_SLEEP_1_bp BOD_SLEEP1_bp
  #elif defined(BOD_SLEEP_1_bp)
    //deprecated_constant_name BOD_SLEEP1_bp = BOD_SLEEP_1_bp;
    #define BOD_SLEEP1_bp BOD_SLEEP_1_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(BOD_ACTIVE_0_bm) && defined(BOD_ACTIVE0_bm)
    #define BOD_ACTIVE_0_bm BOD_ACTIVE0_bm
  #elif defined(BOD_ACTIVE_0_bm)
    //deprecated_constant_name BOD_ACTIVE0_bm = BOD_ACTIVE_0_bm;
    #define BOD_ACTIVE0_bm BOD_ACTIVE_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(BOD_ACTIVE_0_bp) && defined(BOD_ACTIVE0_bp)
    #define BOD_ACTIVE_0_bp BOD_ACTIVE0_bp
  #elif defined(BOD_ACTIVE_0_bp)
    //deprecated_constant_name BOD_ACTIVE0_bp = BOD_ACTIVE_0_bp;
    #define BOD_ACTIVE0_bp BOD_ACTIVE_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(BOD_ACTIVE_1_bm) && defined(BOD_ACTIVE1_bm)
    #define BOD_ACTIVE_1_bm BOD_ACTIVE1_bm
  #elif defined(BOD_ACTIVE_1_bm)
    //deprecated_constant_name BOD_ACTIVE1_bm = BOD_ACTIVE_1_bm;
    #define BOD_ACTIVE1_bm BOD_ACTIVE_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(BOD_ACTIVE_1_bp) && defined(BOD_ACTIVE1_bp)
    #define BOD_ACTIVE_1_bp BOD_ACTIVE1_bp
  #elif defined(BOD_ACTIVE_1_bp)
    //deprecated_constant_name BOD_ACTIVE1_bp = BOD_ACTIVE_1_bp;
    #define BOD_ACTIVE1_bp BOD_ACTIVE_1_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(BOD_LVL_0_bm) && defined(BOD_LVL0_bm)
    #define BOD_LVL_0_bm BOD_LVL0_bm
  #elif defined(BOD_LVL_0_bm)
    //deprecated_constant_name BOD_LVL0_bm = BOD_LVL_0_bm;
    #define BOD_LVL0_bm BOD_LVL_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(BOD_LVL_0_bp) && defined(BOD_LVL0_bp)
    #define BOD_LVL_0_bp BOD_LVL0_bp
  #elif defined(BOD_LVL_0_bp)
    //deprecated_constant_name BOD_LVL0_bp = BOD_LVL_0_bp;
    #define BOD_LVL0_bp BOD_LVL_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(BOD_LVL_1_bm) && defined(BOD_LVL1_bm)
    #define BOD_LVL_1_bm BOD_LVL1_bm
  #elif defined(BOD_LVL_1_bm)
    //deprecated_constant_name BOD_LVL1_bm = BOD_LVL_1_bm;
    #define BOD_LVL1_bm BOD_LVL_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(BOD_LVL_1_bp) && defined(BOD_LVL1_bp)
    #define BOD_LVL_1_bp BOD_LVL1_bp
  #elif defined(BOD_LVL_1_bp)
    //deprecated_constant_name BOD_LVL1_bp = BOD_LVL_1_bp;
    #define BOD_LVL1_bp BOD_LVL_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(BOD_LVL_2_bm) && defined(BOD_LVL2_bm)
    #define BOD_LVL_2_bm BOD_LVL2_bm
  #elif defined(BOD_LVL_2_bm)
    //deprecated_constant_name BOD_LVL2_bm = BOD_LVL_2_bm;
    #define BOD_LVL2_bm BOD_LVL_2_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(BOD_LVL_2_bp) && defined(BOD_LVL2_bp)
    #define BOD_LVL_2_bp BOD_LVL2_bp
  #elif defined(BOD_LVL_2_bp)
    //deprecated_constant_name BOD_LVL2_bp = BOD_LVL_2_bp;
    #define BOD_LVL2_bp BOD_LVL_2_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(BOD_VLMLVL_0_bm) && defined(BOD_VLMLVL0_bm)
    #define BOD_VLMLVL_0_bm BOD_VLMLVL0_bm
  #elif defined(BOD_VLMLVL_0_bm)
    //deprecated_constant_name BOD_VLMLVL0_bm = BOD_VLMLVL_0_bm;
    #define BOD_VLMLVL0_bm BOD_VLMLVL_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(BOD_VLMLVL_0_bp) && defined(BOD_VLMLVL0_bp)
    #define BOD_VLMLVL_0_bp BOD_VLMLVL0_bp
  #elif defined(BOD_VLMLVL_0_bp)
    //deprecated_constant_name BOD_VLMLVL0_bp = BOD_VLMLVL_0_bp;
    #define BOD_VLMLVL0_bp BOD_VLMLVL_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(BOD_VLMLVL_1_bm) && defined(BOD_VLMLVL1_bm)
    #define BOD_VLMLVL_1_bm BOD_VLMLVL1_bm
  #elif defined(BOD_VLMLVL_1_bm)
    //deprecated_constant_name BOD_VLMLVL1_bm = BOD_VLMLVL_1_bm;
    #define BOD_VLMLVL1_bm BOD_VLMLVL_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(BOD_VLMLVL_1_bp) && defined(BOD_VLMLVL1_bp)
    #define BOD_VLMLVL_1_bp BOD_VLMLVL1_bp
  #elif defined(BOD_VLMLVL_1_bp)
    //deprecated_constant_name BOD_VLMLVL1_bp = BOD_VLMLVL_1_bp;
    #define BOD_VLMLVL1_bp BOD_VLMLVL_1_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(BOD_VLMCFG_0_bm) && defined(BOD_VLMCFG0_bm)
    #define BOD_VLMCFG_0_bm BOD_VLMCFG0_bm
  #elif defined(BOD_VLMCFG_0_bm)
    //deprecated_constant_name BOD_VLMCFG0_bm = BOD_VLMCFG_0_bm;
    #define BOD_VLMCFG0_bm BOD_VLMCFG_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(BOD_VLMCFG_0_bp) && defined(BOD_VLMCFG0_bp)
    #define BOD_VLMCFG_0_bp BOD_VLMCFG0_bp
  #elif defined(BOD_VLMCFG_0_bp)
    //deprecated_constant_name BOD_VLMCFG0_bp = BOD_VLMCFG_0_bp;
    #define BOD_VLMCFG0_bp BOD_VLMCFG_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(BOD_VLMCFG_1_bm) && defined(BOD_VLMCFG1_bm)
    #define BOD_VLMCFG_1_bm BOD_VLMCFG1_bm
  #elif defined(BOD_VLMCFG_1_bm)
    //deprecated_constant_name BOD_VLMCFG1_bm = BOD_VLMCFG_1_bm;
    #define BOD_VLMCFG1_bm BOD_VLMCFG_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(BOD_VLMCFG_1_bp) && defined(BOD_VLMCFG1_bp)
    #define BOD_VLMCFG_1_bp BOD_VLMCFG1_bp
  #elif defined(BOD_VLMCFG_1_bp)
    //deprecated_constant_name BOD_VLMCFG1_bp = BOD_VLMCFG_1_bp;
    #define BOD_VLMCFG1_bp BOD_VLMCFG_1_bp //Deprecated as of Q2 2022 header change.
  #endif

  /* ======= CCL ======= */
  #if !defined(CCL_SEQSEL_0_bm) && defined(CCL_SEQSEL0_bm)
    #define CCL_SEQSEL_0_bm CCL_SEQSEL0_bm
  #elif defined(CCL_SEQSEL_0_bm)
    //deprecated_constant_name CCL_SEQSEL0_bm = CCL_SEQSEL_0_bm;
    #define CCL_SEQSEL0_bm CCL_SEQSEL_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_SEQSEL_0_bp) && defined(CCL_SEQSEL0_bp)
    #define CCL_SEQSEL_0_bp CCL_SEQSEL0_bp
  #elif defined(CCL_SEQSEL_0_bp)
    //deprecated_constant_name CCL_SEQSEL0_bp = CCL_SEQSEL_0_bp;
    #define CCL_SEQSEL0_bp CCL_SEQSEL_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_SEQSEL_1_bm) && defined(CCL_SEQSEL1_bm)
    #define CCL_SEQSEL_1_bm CCL_SEQSEL1_bm
  #elif defined(CCL_SEQSEL_1_bm)
    //deprecated_constant_name CCL_SEQSEL1_bm = CCL_SEQSEL_1_bm;
    #define CCL_SEQSEL1_bm CCL_SEQSEL_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_SEQSEL_1_bp) && defined(CCL_SEQSEL1_bp)
    #define CCL_SEQSEL_1_bp CCL_SEQSEL1_bp
  #elif defined(CCL_SEQSEL_1_bp)
    //deprecated_constant_name CCL_SEQSEL1_bp = CCL_SEQSEL_1_bp;
    #define CCL_SEQSEL1_bp CCL_SEQSEL_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_SEQSEL_2_bm) && defined(CCL_SEQSEL2_bm)
    #define CCL_SEQSEL_2_bm CCL_SEQSEL2_bm
  #elif defined(CCL_SEQSEL_2_bm)
    //deprecated_constant_name CCL_SEQSEL2_bm = CCL_SEQSEL_2_bm;
    #define CCL_SEQSEL2_bm CCL_SEQSEL_2_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_SEQSEL_2_bp) && defined(CCL_SEQSEL2_bp)
    #define CCL_SEQSEL_2_bp CCL_SEQSEL2_bp
  #elif defined(CCL_SEQSEL_2_bp)
    //deprecated_constant_name CCL_SEQSEL2_bp = CCL_SEQSEL_2_bp;
    #define CCL_SEQSEL2_bp CCL_SEQSEL_2_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(CCL_INTMODE0_0_bm) && defined(CCL_INTMODE00_bm)
    #define CCL_INTMODE0_0_bm CCL_INTMODE00_bm
  #elif defined(CCL_INTMODE0_0_bm)
    //deprecated_constant_name CCL_INTMODE00_bm = CCL_INTMODE0_0_bm;
    #define CCL_INTMODE00_bm CCL_INTMODE0_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_INTMODE0_0_bp) && defined(CCL_INTMODE00_bp)
    #define CCL_INTMODE0_0_bp CCL_INTMODE00_bp
  #elif defined(CCL_INTMODE0_0_bp)
    //deprecated_constant_name CCL_INTMODE00_bp = CCL_INTMODE0_0_bp;
    #define CCL_INTMODE00_bp CCL_INTMODE0_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_INTMODE0_1_bm) && defined(CCL_INTMODE01_bm)
    #define CCL_INTMODE0_1_bm CCL_INTMODE01_bm
  #elif defined(CCL_INTMODE0_1_bm)
    //deprecated_constant_name CCL_INTMODE01_bm = CCL_INTMODE0_1_bm;
    #define CCL_INTMODE01_bm CCL_INTMODE0_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_INTMODE0_1_bp) && defined(CCL_INTMODE01_bp)
    #define CCL_INTMODE0_1_bp CCL_INTMODE01_bp
  #elif defined(CCL_INTMODE0_1_bp)
    //deprecated_constant_name CCL_INTMODE01_bp = CCL_INTMODE0_1_bp;
    #define CCL_INTMODE01_bp CCL_INTMODE0_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_INTMODE1_0_bm) && defined(CCL_INTMODE10_bm)
    #define CCL_INTMODE1_0_bm CCL_INTMODE10_bm
  #elif defined(CCL_INTMODE1_0_bm)
    //deprecated_constant_name CCL_INTMODE10_bm = CCL_INTMODE1_0_bm;
    #define CCL_INTMODE10_bm CCL_INTMODE1_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_INTMODE1_0_bp) && defined(CCL_INTMODE10_bp)
    #define CCL_INTMODE1_0_bp CCL_INTMODE10_bp
  #elif defined(CCL_INTMODE1_0_bp)
    //deprecated_constant_name CCL_INTMODE10_bp = CCL_INTMODE1_0_bp;
    #define CCL_INTMODE10_bp CCL_INTMODE1_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_INTMODE1_1_bm) && defined(CCL_INTMODE11_bm)
    #define CCL_INTMODE1_1_bm CCL_INTMODE11_bm
  #elif defined(CCL_INTMODE1_1_bm)
    //deprecated_constant_name CCL_INTMODE11_bm = CCL_INTMODE1_1_bm;
    #define CCL_INTMODE11_bm CCL_INTMODE1_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_INTMODE1_1_bp) && defined(CCL_INTMODE11_bp)
    #define CCL_INTMODE1_1_bp CCL_INTMODE11_bp
  #elif defined(CCL_INTMODE1_1_bp)
    //deprecated_constant_name CCL_INTMODE11_bp = CCL_INTMODE1_1_bp;
    #define CCL_INTMODE11_bp CCL_INTMODE1_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_INTMODE2_0_bm) && defined(CCL_INTMODE20_bm)
    #define CCL_INTMODE2_0_bm CCL_INTMODE20_bm
  #elif defined(CCL_INTMODE2_0_bm)
    //deprecated_constant_name CCL_INTMODE20_bm = CCL_INTMODE2_0_bm;
    #define CCL_INTMODE20_bm CCL_INTMODE2_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_INTMODE2_0_bp) && defined(CCL_INTMODE20_bp)
    #define CCL_INTMODE2_0_bp CCL_INTMODE20_bp
  #elif defined(CCL_INTMODE2_0_bp)
    //deprecated_constant_name CCL_INTMODE20_bp = CCL_INTMODE2_0_bp;
    #define CCL_INTMODE20_bp CCL_INTMODE2_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_INTMODE2_1_bm) && defined(CCL_INTMODE21_bm)
    #define CCL_INTMODE2_1_bm CCL_INTMODE21_bm
  #elif defined(CCL_INTMODE2_1_bm)
    //deprecated_constant_name CCL_INTMODE21_bm = CCL_INTMODE2_1_bm;
    #define CCL_INTMODE21_bm CCL_INTMODE2_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_INTMODE2_1_bp) && defined(CCL_INTMODE21_bp)
    #define CCL_INTMODE2_1_bp CCL_INTMODE21_bp
  #elif defined(CCL_INTMODE2_1_bp)
    //deprecated_constant_name CCL_INTMODE21_bp = CCL_INTMODE2_1_bp;
    #define CCL_INTMODE21_bp CCL_INTMODE2_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_INTMODE3_0_bm) && defined(CCL_INTMODE30_bm)
    #define CCL_INTMODE3_0_bm CCL_INTMODE30_bm
  #elif defined(CCL_INTMODE3_0_bm)
    //deprecated_constant_name CCL_INTMODE30_bm = CCL_INTMODE3_0_bm;
    #define CCL_INTMODE30_bm CCL_INTMODE3_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_INTMODE3_0_bp) && defined(CCL_INTMODE30_bp)
    #define CCL_INTMODE3_0_bp CCL_INTMODE30_bp
  #elif defined(CCL_INTMODE3_0_bp)
    //deprecated_constant_name CCL_INTMODE30_bp = CCL_INTMODE3_0_bp;
    #define CCL_INTMODE30_bp CCL_INTMODE3_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_INTMODE3_1_bm) && defined(CCL_INTMODE31_bm)
    #define CCL_INTMODE3_1_bm CCL_INTMODE31_bm
  #elif defined(CCL_INTMODE3_1_bm)
    //deprecated_constant_name CCL_INTMODE31_bm = CCL_INTMODE3_1_bm;
    #define CCL_INTMODE31_bm CCL_INTMODE3_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_INTMODE3_1_bp) && defined(CCL_INTMODE31_bp)
    #define CCL_INTMODE3_1_bp CCL_INTMODE31_bp
  #elif defined(CCL_INTMODE3_1_bp)
    //deprecated_constant_name CCL_INTMODE31_bp = CCL_INTMODE3_1_bp;
    #define CCL_INTMODE31_bp CCL_INTMODE3_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_INTMODE4_0_bm) && defined(CCL_INTMODE40_bm)
    #define CCL_INTMODE4_0_bm CCL_INTMODE40_bm
  #elif defined(CCL_INTMODE4_0_bm)
    //deprecated_constant_name CCL_INTMODE40_bm = CCL_INTMODE4_0_bm;
    #define CCL_INTMODE40_bm CCL_INTMODE4_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_INTMODE4_0_bp) && defined(CCL_INTMODE40_bp)
    #define CCL_INTMODE4_0_bp CCL_INTMODE40_bp
  #elif defined(CCL_INTMODE4_0_bp)
    //deprecated_constant_name CCL_INTMODE40_bp = CCL_INTMODE4_0_bp;
    #define CCL_INTMODE40_bp CCL_INTMODE4_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_INTMODE4_1_bm) && defined(CCL_INTMODE41_bm)
    #define CCL_INTMODE4_1_bm CCL_INTMODE41_bm
  #elif defined(CCL_INTMODE4_1_bm)
    //deprecated_constant_name CCL_INTMODE41_bm = CCL_INTMODE4_1_bm;
    #define CCL_INTMODE41_bm CCL_INTMODE4_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_INTMODE4_1_bp) && defined(CCL_INTMODE41_bp)
    #define CCL_INTMODE4_1_bp CCL_INTMODE41_bp
  #elif defined(CCL_INTMODE4_1_bp)
    //deprecated_constant_name CCL_INTMODE41_bp = CCL_INTMODE4_1_bp;
    #define CCL_INTMODE41_bp CCL_INTMODE4_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_INTMODE5_0_bm) && defined(CCL_INTMODE50_bm)
    #define CCL_INTMODE5_0_bm CCL_INTMODE50_bm
  #elif defined(CCL_INTMODE5_0_bm)
    //deprecated_constant_name CCL_INTMODE50_bm = CCL_INTMODE5_0_bm;
    #define CCL_INTMODE50_bm CCL_INTMODE5_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_INTMODE5_0_bp) && defined(CCL_INTMODE50_bp)
    #define CCL_INTMODE5_0_bp CCL_INTMODE50_bp
  #elif defined(CCL_INTMODE5_0_bp)
    //deprecated_constant_name CCL_INTMODE50_bp = CCL_INTMODE5_0_bp;
    #define CCL_INTMODE50_bp CCL_INTMODE5_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_INTMODE5_1_bm) && defined(CCL_INTMODE51_bm)
    #define CCL_INTMODE5_1_bm CCL_INTMODE51_bm
  #elif defined(CCL_INTMODE5_1_bm)
    //deprecated_constant_name CCL_INTMODE51_bm = CCL_INTMODE5_1_bm;
    #define CCL_INTMODE51_bm CCL_INTMODE5_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_INTMODE5_1_bp) && defined(CCL_INTMODE51_bp)
    #define CCL_INTMODE5_1_bp CCL_INTMODE51_bp
  #elif defined(CCL_INTMODE5_1_bp)
    //deprecated_constant_name CCL_INTMODE51_bp = CCL_INTMODE5_1_bp;
    #define CCL_INTMODE51_bp CCL_INTMODE5_1_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(CCL_INT_0_bm) && defined(CCL_INT0_bm)
    #define CCL_INT_0_bm CCL_INT0_bm
  #elif defined(CCL_INT_0_bm)
    #undef CCL_INT0_bm /* Another round of placews where they decided to put a compatibility layer the wrong way*/
    //deprecated_constant_name CCL_INT0_bm = CCL_INT_0_bm;
    #define CCL_INT0_bm CCL_INT_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_INT_0_bp) && defined(CCL_INT0_bp)
    #define CCL_INT_0_bp CCL_INT0_bp
  #elif defined(CCL_INT_0_bp)
    #undef CCL_INT0_bp /* Another round of placews where they decided to put a compatibility layer the wrong way*/
    //deprecated_constant_name CCL_INT0_bp = CCL_INT_0_bp;
    #define CCL_INT0_bp CCL_INT_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_INT_1_bm) && defined(CCL_INT1_bm)
    #define CCL_INT_1_bm CCL_INT1_bm
  #elif defined(CCL_INT_1_bm)
    #undef CCL_INT1_bm /* Another round of placews where they decided to put a compatibility layer the wrong way*/
    //deprecated_constant_name CCL_INT1_bm = CCL_INT_1_bm;
    #define CCL_INT1_bm CCL_INT_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_INT_1_bp) && defined(CCL_INT1_bp)
    #define CCL_INT_1_bp CCL_INT1_bp
  #elif defined(CCL_INT_1_bp)
    #undef CCL_INT1_bp /* Another round of placews where they decided to put a compatibility layer the wrong way*/
    //deprecated_constant_name CCL_INT1_bp = CCL_INT_1_bp;
    #define CCL_INT1_bp CCL_INT_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_INT_2_bm) && defined(CCL_INT2_bm)
    #define CCL_INT_2_bm CCL_INT2_bm
  #elif defined(CCL_INT_2_bm)
    #undef CCL_INT2_bm /* Another round of placews where they decided to put a compatibility layer the wrong way*/
    //deprecated_constant_name CCL_INT2_bm = CCL_INT_2_bm;
    #define CCL_INT2_bm CCL_INT_2_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_INT_2_bp) && defined(CCL_INT2_bp)
    #define CCL_INT_2_bp CCL_INT2_bp
  #elif defined(CCL_INT_2_bp)
    #undef CCL_INT2_bp /* Another round of placews where they decided to put a compatibility layer the wrong way*/
    //deprecated_constant_name CCL_INT2_bp = CCL_INT_2_bp;
    #define CCL_INT2_bp CCL_INT_2_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_INT_3_bm) && defined(CCL_INT3_bm)
    #define CCL_INT_3_bm CCL_INT3_bm
  #elif defined(CCL_INT_3_bm)
    #undef CCL_INT3_bm /* Another round of placews where they decided to put a compatibility layer the wrong way*/
    //deprecated_constant_name CCL_INT3_bm = CCL_INT_3_bm;
    #define CCL_INT3_bm CCL_INT_3_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_INT_3_bp) && defined(CCL_INT3_bp)
    #define CCL_INT_3_bp CCL_INT3_bp
  #elif defined(CCL_INT_3_bp)
    #undef CCL_INT3_bp /* Another round of placews where they decided to put a compatibility layer the wrong way*/
    //deprecated_constant_name CCL_INT3_bp = CCL_INT_3_bp;
    #define CCL_INT3_bp CCL_INT_3_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_INT_4_bm) && defined(CCL_INT4_bm)
    #define CCL_INT_4_bm CCL_INT4_bm
  #elif defined(CCL_INT_4_bm)
    #undef CCL_INT4_bm /* Another round of placews where they decided to put a compatibility layer the wrong way*/
    //deprecated_constant_name CCL_INT4_bm = CCL_INT_4_bm;
    #define CCL_INT4_bm CCL_INT_4_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_INT_4_bp) && defined(CCL_INT4_bp)
    #define CCL_INT_4_bp CCL_INT4_bp
  #elif defined(CCL_INT_4_bp)
    #undef CCL_INT4_bp /* Another round of placews where they decided to put a compatibility layer the wrong way*/
    //deprecated_constant_name CCL_INT4_bp = CCL_INT_4_bp;
    #define CCL_INT4_bp CCL_INT_4_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_INT_5_bm) && defined(CCL_INT5_bm)
    #define CCL_INT_5_bm CCL_INT5_bm
  #elif defined(CCL_INT_5_bm)
    #undef CCL_INT5_bm /* Another round of placews where they decided to put a compatibility layer the wrong way*/
    //deprecated_constant_name CCL_INT5_bm = CCL_INT_5_bm;
    #define CCL_INT5_bm CCL_INT_5_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_INT_5_bp) && defined(CCL_INT5_bp)
    #define CCL_INT_5_bp CCL_INT5_bp
  #elif defined(CCL_INT_5_bp)
    #undef CCL_INT5_bp /* Another round of placews where they decided to put a compatibility layer the wrong way*/
    //deprecated_constant_name CCL_INT5_bp = CCL_INT_5_bp;
    #define CCL_INT5_bp CCL_INT_5_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_CLKSRC_0_bm) && defined(CCL_CLKSRC0_bm)
    #define CCL_CLKSRC_0_bm CCL_CLKSRC0_bm
  #elif defined(CCL_CLKSRC_0_bm)
    //deprecated_constant_name CCL_CLKSRC0_bm = CCL_CLKSRC_0_bm;
    #define CCL_CLKSRC0_bm CCL_CLKSRC_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_CLKSRC_0_bp) && defined(CCL_CLKSRC0_bp)
    #define CCL_CLKSRC_0_bp CCL_CLKSRC0_bp
  #elif defined(CCL_CLKSRC_0_bp)
    //deprecated_constant_name CCL_CLKSRC0_bp = CCL_CLKSRC_0_bp;
    #define CCL_CLKSRC0_bp CCL_CLKSRC_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_CLKSRC_1_bm) && defined(CCL_CLKSRC1_bm)
    #define CCL_CLKSRC_1_bm CCL_CLKSRC1_bm
  #elif defined(CCL_CLKSRC_1_bm)
    //deprecated_constant_name CCL_CLKSRC1_bm = CCL_CLKSRC_1_bm;
    #define CCL_CLKSRC1_bm CCL_CLKSRC_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_CLKSRC_1_bp) && defined(CCL_CLKSRC1_bp)
    #define CCL_CLKSRC_1_bp CCL_CLKSRC1_bp
  #elif defined(CCL_CLKSRC_1_bp)
    //deprecated_constant_name CCL_CLKSRC1_bp = CCL_CLKSRC_1_bp;
    #define CCL_CLKSRC1_bp CCL_CLKSRC_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_CLKSRC_2_bm) && defined(CCL_CLKSRC2_bm)
    #define CCL_CLKSRC_2_bm CCL_CLKSRC2_bm
  #elif defined(CCL_CLKSRC_2_bm)
    //deprecated_constant_name CCL_CLKSRC2_bm = CCL_CLKSRC_2_bm;
    #define CCL_CLKSRC2_bm CCL_CLKSRC_2_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_CLKSRC_2_bp) && defined(CCL_CLKSRC2_bp)
    #define CCL_CLKSRC_2_bp CCL_CLKSRC2_bp
  #elif defined(CCL_CLKSRC_2_bp)
    //deprecated_constant_name CCL_CLKSRC2_bp = CCL_CLKSRC_2_bp;
    #define CCL_CLKSRC2_bp CCL_CLKSRC_2_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_FILTSEL_0_bm) && defined(CCL_FILTSEL0_bm)
    #define CCL_FILTSEL_0_bm CCL_FILTSEL0_bm
  #elif defined(CCL_FILTSEL_0_bm)
    //deprecated_constant_name CCL_FILTSEL0_bm = CCL_FILTSEL_0_bm;
    #define CCL_FILTSEL0_bm CCL_FILTSEL_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_FILTSEL_0_bp) && defined(CCL_FILTSEL0_bp)
    #define CCL_FILTSEL_0_bp CCL_FILTSEL0_bp
  #elif defined(CCL_FILTSEL_0_bp)
    //deprecated_constant_name CCL_FILTSEL0_bp = CCL_FILTSEL_0_bp;
    #define CCL_FILTSEL0_bp CCL_FILTSEL_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_FILTSEL_1_bm) && defined(CCL_FILTSEL1_bm)
    #define CCL_FILTSEL_1_bm CCL_FILTSEL1_bm
  #elif defined(CCL_FILTSEL_1_bm)
    //deprecated_constant_name CCL_FILTSEL1_bm = CCL_FILTSEL_1_bm;
    #define CCL_FILTSEL1_bm CCL_FILTSEL_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_FILTSEL_1_bp) && defined(CCL_FILTSEL1_bp)
    #define CCL_FILTSEL_1_bp CCL_FILTSEL1_bp
  #elif defined(CCL_FILTSEL_1_bp)
    //deprecated_constant_name CCL_FILTSEL1_bp = CCL_FILTSEL_1_bp;
    #define CCL_FILTSEL1_bp CCL_FILTSEL_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_INSEL0_0_bm) && defined(CCL_INSEL00_bm)
    #define CCL_INSEL0_0_bm CCL_INSEL00_bm
  #elif defined(CCL_INSEL0_0_bm)
    //deprecated_constant_name CCL_INSEL00_bm = CCL_INSEL0_0_bm;
    #define CCL_INSEL00_bm CCL_INSEL0_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_INSEL0_0_bp) && defined(CCL_INSEL00_bp)
    #define CCL_INSEL0_0_bp CCL_INSEL00_bp
  #elif defined(CCL_INSEL0_0_bp)
    //deprecated_constant_name CCL_INSEL00_bp = CCL_INSEL0_0_bp;
    #define CCL_INSEL00_bp CCL_INSEL0_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_INSEL0_1_bm) && defined(CCL_INSEL01_bm)
    #define CCL_INSEL0_1_bm CCL_INSEL01_bm
  #elif defined(CCL_INSEL0_1_bm)
    //deprecated_constant_name CCL_INSEL01_bm = CCL_INSEL0_1_bm;
    #define CCL_INSEL01_bm CCL_INSEL0_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_INSEL0_1_bp) && defined(CCL_INSEL01_bp)
    #define CCL_INSEL0_1_bp CCL_INSEL01_bp
  #elif defined(CCL_INSEL0_1_bp)
    //deprecated_constant_name CCL_INSEL01_bp = CCL_INSEL0_1_bp;
    #define CCL_INSEL01_bp CCL_INSEL0_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_INSEL0_2_bm) && defined(CCL_INSEL02_bm)
    #define CCL_INSEL0_2_bm CCL_INSEL02_bm
  #elif defined(CCL_INSEL0_2_bm)
    //deprecated_constant_name CCL_INSEL02_bm = CCL_INSEL0_2_bm;
    #define CCL_INSEL02_bm CCL_INSEL0_2_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_INSEL0_2_bp) && defined(CCL_INSEL02_bp)
    #define CCL_INSEL0_2_bp CCL_INSEL02_bp
  #elif defined(CCL_INSEL0_2_bp)
    //deprecated_constant_name CCL_INSEL02_bp = CCL_INSEL0_2_bp;
    #define CCL_INSEL02_bp CCL_INSEL0_2_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_INSEL0_3_bm) && defined(CCL_INSEL03_bm)
    #define CCL_INSEL0_3_bm CCL_INSEL03_bm
  #elif defined(CCL_INSEL0_3_bm)
    //deprecated_constant_name CCL_INSEL03_bm = CCL_INSEL0_3_bm;
    #define CCL_INSEL03_bm CCL_INSEL0_3_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_INSEL0_3_bp) && defined(CCL_INSEL03_bp)
    #define CCL_INSEL0_3_bp CCL_INSEL03_bp
  #elif defined(CCL_INSEL0_3_bp)
    //deprecated_constant_name CCL_INSEL03_bp = CCL_INSEL0_3_bp;
    #define CCL_INSEL03_bp CCL_INSEL0_3_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_INSEL1_0_bm) && defined(CCL_INSEL10_bm)
    #define CCL_INSEL1_0_bm CCL_INSEL10_bm
  #elif defined(CCL_INSEL1_0_bm)
    //deprecated_constant_name CCL_INSEL10_bm = CCL_INSEL1_0_bm;
    #define CCL_INSEL10_bm CCL_INSEL1_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_INSEL1_0_bp) && defined(CCL_INSEL10_bp)
    #define CCL_INSEL1_0_bp CCL_INSEL10_bp
  #elif defined(CCL_INSEL1_0_bp)
    //deprecated_constant_name CCL_INSEL10_bp = CCL_INSEL1_0_bp;
    #define CCL_INSEL10_bp CCL_INSEL1_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_INSEL1_1_bm) && defined(CCL_INSEL11_bm)
    #define CCL_INSEL1_1_bm CCL_INSEL11_bm
  #elif defined(CCL_INSEL1_1_bm)
    //deprecated_constant_name CCL_INSEL11_bm = CCL_INSEL1_1_bm;
    #define CCL_INSEL11_bm CCL_INSEL1_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_INSEL1_1_bp) && defined(CCL_INSEL11_bp)
    #define CCL_INSEL1_1_bp CCL_INSEL11_bp
  #elif defined(CCL_INSEL1_1_bp)
    //deprecated_constant_name CCL_INSEL11_bp = CCL_INSEL1_1_bp;
    #define CCL_INSEL11_bp CCL_INSEL1_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_INSEL1_2_bm) && defined(CCL_INSEL12_bm)
    #define CCL_INSEL1_2_bm CCL_INSEL12_bm
  #elif defined(CCL_INSEL1_2_bm)
    //deprecated_constant_name CCL_INSEL12_bm = CCL_INSEL1_2_bm;
    #define CCL_INSEL12_bm CCL_INSEL1_2_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_INSEL1_2_bp) && defined(CCL_INSEL12_bp)
    #define CCL_INSEL1_2_bp CCL_INSEL12_bp
  #elif defined(CCL_INSEL1_2_bp)
    //deprecated_constant_name CCL_INSEL12_bp = CCL_INSEL1_2_bp;
    #define CCL_INSEL12_bp CCL_INSEL1_2_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_INSEL1_3_bm) && defined(CCL_INSEL13_bm)
    #define CCL_INSEL1_3_bm CCL_INSEL13_bm
  #elif defined(CCL_INSEL1_3_bm)
    //deprecated_constant_name CCL_INSEL13_bm = CCL_INSEL1_3_bm;
    #define CCL_INSEL13_bm CCL_INSEL1_3_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_INSEL1_3_bp) && defined(CCL_INSEL13_bp)
    #define CCL_INSEL1_3_bp CCL_INSEL13_bp
  #elif defined(CCL_INSEL1_3_bp)
    //deprecated_constant_name CCL_INSEL13_bp = CCL_INSEL1_3_bp;
    #define CCL_INSEL13_bp CCL_INSEL1_3_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_INSEL2_0_bm) && defined(CCL_INSEL20_bm)
    #define CCL_INSEL2_0_bm CCL_INSEL20_bm
  #elif defined(CCL_INSEL2_0_bm)
    //deprecated_constant_name CCL_INSEL20_bm = CCL_INSEL2_0_bm;
    #define CCL_INSEL20_bm CCL_INSEL2_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_INSEL2_0_bp) && defined(CCL_INSEL20_bp)
    #define CCL_INSEL2_0_bp CCL_INSEL20_bp
  #elif defined(CCL_INSEL2_0_bp)
    //deprecated_constant_name CCL_INSEL20_bp = CCL_INSEL2_0_bp;
    #define CCL_INSEL20_bp CCL_INSEL2_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_INSEL2_1_bm) && defined(CCL_INSEL21_bm)
    #define CCL_INSEL2_1_bm CCL_INSEL21_bm
  #elif defined(CCL_INSEL2_1_bm)
    //deprecated_constant_name CCL_INSEL21_bm = CCL_INSEL2_1_bm;
    #define CCL_INSEL21_bm CCL_INSEL2_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_INSEL2_1_bp) && defined(CCL_INSEL21_bp)
    #define CCL_INSEL2_1_bp CCL_INSEL21_bp
  #elif defined(CCL_INSEL2_1_bp)
    //deprecated_constant_name CCL_INSEL21_bp = CCL_INSEL2_1_bp;
    #define CCL_INSEL21_bp CCL_INSEL2_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_INSEL2_2_bm) && defined(CCL_INSEL22_bm)
    #define CCL_INSEL2_2_bm CCL_INSEL22_bm
  #elif defined(CCL_INSEL2_2_bm)
    //deprecated_constant_name CCL_INSEL22_bm = CCL_INSEL2_2_bm;
    #define CCL_INSEL22_bm CCL_INSEL2_2_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_INSEL2_2_bp) && defined(CCL_INSEL22_bp)
    #define CCL_INSEL2_2_bp CCL_INSEL22_bp
  #elif defined(CCL_INSEL2_2_bp)
    //deprecated_constant_name CCL_INSEL22_bp = CCL_INSEL2_2_bp;
    #define CCL_INSEL22_bp CCL_INSEL2_2_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_INSEL2_3_bm) && defined(CCL_INSEL23_bm)
    #define CCL_INSEL2_3_bm CCL_INSEL23_bm
  #elif defined(CCL_INSEL2_3_bm)
    //deprecated_constant_name CCL_INSEL23_bm = CCL_INSEL2_3_bm;
    #define CCL_INSEL23_bm CCL_INSEL2_3_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_INSEL2_3_bp) && defined(CCL_INSEL23_bp)
    #define CCL_INSEL2_3_bp CCL_INSEL23_bp
  #elif defined(CCL_INSEL2_3_bp)
    //deprecated_constant_name CCL_INSEL23_bp = CCL_INSEL2_3_bp;
    #define CCL_INSEL23_bp CCL_INSEL2_3_bp //Deprecated as of Q2 2022 header change.
  #endif

  /* ======= CLKCTRL ======= */
  #if !defined(CLKCTRL_FRQSEL_0_bm) && defined(CLKCTRL_FRQSEL0_bm)
    #define CLKCTRL_FRQSEL_0_bm CLKCTRL_FRQSEL0_bm
  #elif defined(CLKCTRL_FRQSEL_0_bm)
    //deprecated_constant_name CLKCTRL_FRQSEL0_bm = CLKCTRL_FRQSEL_0_bm;
    #define CLKCTRL_FRQSEL0_bm CLKCTRL_FRQSEL_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CLKCTRL_FRQSEL_0_bp) && defined(CLKCTRL_FRQSEL0_bp)
    #define CLKCTRL_FRQSEL_0_bp CLKCTRL_FRQSEL0_bp
  #elif defined(CLKCTRL_FRQSEL_0_bp)
    //deprecated_constant_name CLKCTRL_FRQSEL0_bp = CLKCTRL_FRQSEL_0_bp;
    #define CLKCTRL_FRQSEL0_bp CLKCTRL_FRQSEL_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CLKCTRL_FRQSEL_1_bm) && defined(CLKCTRL_FRQSEL1_bm)
    #define CLKCTRL_FRQSEL_1_bm CLKCTRL_FRQSEL1_bm
  #elif defined(CLKCTRL_FRQSEL_1_bm)
    //deprecated_constant_name CLKCTRL_FRQSEL1_bm = CLKCTRL_FRQSEL_1_bm;
    #define CLKCTRL_FRQSEL1_bm CLKCTRL_FRQSEL_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CLKCTRL_FRQSEL_1_bp) && defined(CLKCTRL_FRQSEL1_bp)
    #define CLKCTRL_FRQSEL_1_bp CLKCTRL_FRQSEL1_bp
  #elif defined(CLKCTRL_FRQSEL_1_bp)
    //deprecated_constant_name CLKCTRL_FRQSEL1_bp = CLKCTRL_FRQSEL_1_bp;
    #define CLKCTRL_FRQSEL1_bp CLKCTRL_FRQSEL_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CLKCTRL_FRQSEL_2_bm) && defined(CLKCTRL_FRQSEL2_bm)
    #define CLKCTRL_FRQSEL_2_bm CLKCTRL_FRQSEL2_bm
  #elif defined(CLKCTRL_FRQSEL_2_bm)
    //deprecated_constant_name CLKCTRL_FRQSEL2_bm = CLKCTRL_FRQSEL_2_bm;
    #define CLKCTRL_FRQSEL2_bm CLKCTRL_FRQSEL_2_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CLKCTRL_FRQSEL_2_bp) && defined(CLKCTRL_FRQSEL2_bp)
    #define CLKCTRL_FRQSEL_2_bp CLKCTRL_FRQSEL2_bp
  #elif defined(CLKCTRL_FRQSEL_2_bp)
    //deprecated_constant_name CLKCTRL_FRQSEL2_bp = CLKCTRL_FRQSEL_2_bp;
    #define CLKCTRL_FRQSEL2_bp CLKCTRL_FRQSEL_2_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CLKCTRL_FRQSEL_3_bm) && defined(CLKCTRL_FRQSEL3_bm)
    #define CLKCTRL_FRQSEL_3_bm CLKCTRL_FRQSEL3_bm
  #elif defined(CLKCTRL_FRQSEL_3_bm)
    //deprecated_constant_name CLKCTRL_FRQSEL3_bm = CLKCTRL_FRQSEL_3_bm;
    #define CLKCTRL_FRQSEL3_bm CLKCTRL_FRQSEL_3_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CLKCTRL_FRQSEL_3_bp) && defined(CLKCTRL_FRQSEL3_bp)
    #define CLKCTRL_FRQSEL_3_bp CLKCTRL_FRQSEL3_bp
  #elif defined(CLKCTRL_FRQSEL_3_bp)
    //deprecated_constant_name CLKCTRL_FRQSEL3_bp = CLKCTRL_FRQSEL_3_bp;
    #define CLKCTRL_FRQSEL3_bp CLKCTRL_FRQSEL_3_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CLKCTRL_MULFAC_0_bm) && defined(CLKCTRL_MULFAC0_bm)
    #define CLKCTRL_MULFAC_0_bm CLKCTRL_MULFAC0_bm
  #elif defined(CLKCTRL_MULFAC_0_bm)
    //deprecated_constant_name CLKCTRL_MULFAC0_bm = CLKCTRL_MULFAC_0_bm;
    #define CLKCTRL_MULFAC0_bm CLKCTRL_MULFAC_0_bm //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(CLKCTRL_MULFAC_0_bp) && defined(CLKCTRL_MULFAC0_bp)
    #define CLKCTRL_MULFAC_0_bp CLKCTRL_MULFAC0_bp
  #elif defined(CLKCTRL_MULFAC_0_bp)
    //deprecated_constant_name CLKCTRL_MULFAC0_bp = CLKCTRL_MULFAC_0_bp;
    #define CLKCTRL_MULFAC0_bp CLKCTRL_MULFAC_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CLKCTRL_MULFAC_1_bm) && defined(CLKCTRL_MULFAC1_bm)
    #define CLKCTRL_MULFAC_1_bm CLKCTRL_MULFAC1_bm
  #elif defined(CLKCTRL_MULFAC_1_bm)
    //deprecated_constant_name CLKCTRL_MULFAC1_bm = CLKCTRL_MULFAC_1_bm;
    #define CLKCTRL_MULFAC1_bm CLKCTRL_MULFAC_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CLKCTRL_MULFAC_1_bp) && defined(CLKCTRL_MULFAC1_bp)
    #define CLKCTRL_MULFAC_1_bp CLKCTRL_MULFAC1_bp
  #elif defined(CLKCTRL_MULFAC_1_bp)
    //deprecated_constant_name CLKCTRL_MULFAC1_bp = CLKCTRL_MULFAC_1_bp;
    #define CLKCTRL_MULFAC1_bp CLKCTRL_MULFAC_1_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(CLKCTRL_CSUT_0_bm) && defined(CLKCTRL_CSUT0_bm)
    #define CLKCTRL_CSUT_0_bm CLKCTRL_CSUT0_bm
  #elif defined(CLKCTRL_CSUT_0_bm)
    //deprecated_constant_name CLKCTRL_CSUT0_bm = CLKCTRL_CSUT_0_bm;
    #define CLKCTRL_CSUT0_bm CLKCTRL_CSUT_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CLKCTRL_CSUT_0_bp) && defined(CLKCTRL_CSUT0_bp)
    #define CLKCTRL_CSUT_0_bp CLKCTRL_CSUT0_bp
  #elif defined(CLKCTRL_CSUT_0_bp)
    //deprecated_constant_name CLKCTRL_CSUT0_bp = CLKCTRL_CSUT_0_bp;
    #define CLKCTRL_CSUT0_bp CLKCTRL_CSUT_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CLKCTRL_CSUT_1_bm) && defined(CLKCTRL_CSUT1_bm)
    #define CLKCTRL_CSUT_1_bm CLKCTRL_CSUT1_bm
  #elif defined(CLKCTRL_CSUT_1_bm)
    //deprecated_constant_name CLKCTRL_CSUT1_bm = CLKCTRL_CSUT_1_bm;
    #define CLKCTRL_CSUT1_bm CLKCTRL_CSUT_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CLKCTRL_CSUT_1_bp) && defined(CLKCTRL_CSUT1_bp)
    #define CLKCTRL_CSUT_1_bp CLKCTRL_CSUT1_bp
  #elif defined(CLKCTRL_CSUT_1_bp)
    //deprecated_constant_name CLKCTRL_CSUT1_bp = CLKCTRL_CSUT_1_bp;
    #define CLKCTRL_CSUT1_bp CLKCTRL_CSUT_1_bp //Deprecated as of Q2 2022 header change.
  #endif

  /* ======= Fuses ======= */
  #if !defined(FUSE_PERIOD_0_bm) && defined(FUSE_PERIOD0_bm)
    #define FUSE_PERIOD_0_bm FUSE_PERIOD0_bm
  #elif defined(FUSE_PERIOD_0_bm)
    //deprecated_constant_name FUSE_PERIOD0_bm = FUSE_PERIOD_0_bm;
    #define FUSE_PERIOD0_bm FUSE_PERIOD_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(FUSE_PERIOD_0_bp) && defined(FUSE_PERIOD0_bp)
    #define FUSE_PERIOD_0_bp FUSE_PERIOD0_bp
  #elif defined(FUSE_PERIOD_0_bp)
    //deprecated_constant_name FUSE_PERIOD0_bp = FUSE_PERIOD_0_bp;
    #define FUSE_PERIOD0_bp FUSE_PERIOD_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(FUSE_PERIOD_1_bm) && defined(FUSE_PERIOD1_bm)
    #define FUSE_PERIOD_1_bm FUSE_PERIOD1_bm
  #elif defined(FUSE_PERIOD_1_bm)
    //deprecated_constant_name FUSE_PERIOD1_bm = FUSE_PERIOD_1_bm;
    #define FUSE_PERIOD1_bm FUSE_PERIOD_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(FUSE_PERIOD_1_bp) && defined(FUSE_PERIOD1_bp)
    #define FUSE_PERIOD_1_bp FUSE_PERIOD1_bp
  #elif defined(FUSE_PERIOD_1_bp)
    //deprecated_constant_name FUSE_PERIOD1_bp = FUSE_PERIOD_1_bp;
    #define FUSE_PERIOD1_bp FUSE_PERIOD_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(FUSE_PERIOD_2_bm) && defined(FUSE_PERIOD2_bm)
    #define FUSE_PERIOD_2_bm FUSE_PERIOD2_bm
  #elif defined(FUSE_PERIOD_2_bm)
    //deprecated_constant_name FUSE_PERIOD2_bm = FUSE_PERIOD_2_bm;
    #define FUSE_PERIOD2_bm FUSE_PERIOD_2_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(FUSE_PERIOD_2_bp) && defined(FUSE_PERIOD2_bp)
    #define FUSE_PERIOD_2_bp FUSE_PERIOD2_bp
  #elif defined(FUSE_PERIOD_2_bp)
    //deprecated_constant_name FUSE_PERIOD2_bp = FUSE_PERIOD_2_bp;
    #define FUSE_PERIOD2_bp FUSE_PERIOD_2_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(FUSE_PERIOD_3_bm) && defined(FUSE_PERIOD3_bm)
    #define FUSE_PERIOD_3_bm FUSE_PERIOD3_bm
  #elif defined(FUSE_PERIOD_3_bm)
    //deprecated_constant_name FUSE_PERIOD3_bm = FUSE_PERIOD_3_bm;
    #define FUSE_PERIOD3_bm FUSE_PERIOD_3_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(FUSE_PERIOD_3_bp) && defined(FUSE_PERIOD3_bp)
    #define FUSE_PERIOD_3_bp FUSE_PERIOD3_bp
  #elif defined(FUSE_PERIOD_3_bp)
    //deprecated_constant_name FUSE_PERIOD3_bp = FUSE_PERIOD_3_bp;
    #define FUSE_PERIOD3_bp FUSE_PERIOD_3_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(FUSE_WINDOW_0_bm) && defined(FUSE_WINDOW0_bm)
    #define FUSE_WINDOW_0_bm FUSE_WINDOW0_bm
  #elif defined(FUSE_WINDOW_0_bm)
    //deprecated_constant_name FUSE_WINDOW0_bm = FUSE_WINDOW_0_bm;
    #define FUSE_WINDOW0_bm FUSE_WINDOW_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(FUSE_WINDOW_0_bp) && defined(FUSE_WINDOW0_bp)
    #define FUSE_WINDOW_0_bp FUSE_WINDOW0_bp
  #elif defined(FUSE_WINDOW_0_bp)
    //deprecated_constant_name FUSE_WINDOW0_bp = FUSE_WINDOW_0_bp;
    #define FUSE_WINDOW0_bp FUSE_WINDOW_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(FUSE_WINDOW_1_bm) && defined(FUSE_WINDOW1_bm)
    #define FUSE_WINDOW_1_bm FUSE_WINDOW1_bm
  #elif defined(FUSE_WINDOW_1_bm)
    //deprecated_constant_name FUSE_WINDOW1_bm = FUSE_WINDOW_1_bm;
    #define FUSE_WINDOW1_bm FUSE_WINDOW_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(FUSE_WINDOW_1_bp) && defined(FUSE_WINDOW1_bp)
    #define FUSE_WINDOW_1_bp FUSE_WINDOW1_bp
  #elif defined(FUSE_WINDOW_1_bp)
    //deprecated_constant_name FUSE_WINDOW1_bp = FUSE_WINDOW_1_bp;
    #define FUSE_WINDOW1_bp FUSE_WINDOW_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(FUSE_WINDOW_2_bm) && defined(FUSE_WINDOW2_bm)
    #define FUSE_WINDOW_2_bm FUSE_WINDOW2_bm
  #elif defined(FUSE_WINDOW_2_bm)
    //deprecated_constant_name FUSE_WINDOW2_bm = FUSE_WINDOW_2_bm;
    #define FUSE_WINDOW2_bm FUSE_WINDOW_2_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(FUSE_WINDOW_2_bp) && defined(FUSE_WINDOW2_bp)
    #define FUSE_WINDOW_2_bp FUSE_WINDOW2_bp
  #elif defined(FUSE_WINDOW_2_bp)
    //deprecated_constant_name FUSE_WINDOW2_bp = FUSE_WINDOW_2_bp;
    #define FUSE_WINDOW2_bp FUSE_WINDOW_2_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(FUSE_WINDOW_3_bm) && defined(FUSE_WINDOW3_bm)
    #define FUSE_WINDOW_3_bm FUSE_WINDOW3_bm
  #elif defined(FUSE_WINDOW_3_bm)
    //deprecated_constant_name FUSE_WINDOW3_bm = FUSE_WINDOW_3_bm;
    #define FUSE_WINDOW3_bm FUSE_WINDOW_3_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(FUSE_WINDOW_3_bp) && defined(FUSE_WINDOW3_bp)
    #define FUSE_WINDOW_3_bp FUSE_WINDOW3_bp
  #elif defined(FUSE_WINDOW_3_bp)
    //deprecated_constant_name FUSE_WINDOW3_bp = FUSE_WINDOW_3_bp;
    #define FUSE_WINDOW3_bp FUSE_WINDOW_3_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(FUSE_SLEEP_0_bm) && defined(FUSE_SLEEP0_bm)
    #define FUSE_SLEEP_0_bm FUSE_SLEEP0_bm
  #elif defined(FUSE_SLEEP_0_bm)
    //deprecated_constant_name FUSE_SLEEP0_bm = FUSE_SLEEP_0_bm;
    #define FUSE_SLEEP0_bm FUSE_SLEEP_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(FUSE_SLEEP_0_bp) && defined(FUSE_SLEEP0_bp)
    #define FUSE_SLEEP_0_bp FUSE_SLEEP0_bp
  #elif defined(FUSE_SLEEP_0_bp)
    //deprecated_constant_name FUSE_SLEEP0_bp = FUSE_SLEEP_0_bp;
    #define FUSE_SLEEP0_bp FUSE_SLEEP_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(FUSE_SLEEP_1_bm) && defined(FUSE_SLEEP1_bm)
    #define FUSE_SLEEP_1_bm FUSE_SLEEP1_bm
  #elif defined(FUSE_SLEEP_1_bm)
    //deprecated_constant_name FUSE_SLEEP1_bm = FUSE_SLEEP_1_bm;
    #define FUSE_SLEEP1_bm FUSE_SLEEP_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(FUSE_SLEEP_1_bp) && defined(FUSE_SLEEP1_bp)
    #define FUSE_SLEEP_1_bp FUSE_SLEEP1_bp
  #elif defined(FUSE_SLEEP_1_bp)
    //deprecated_constant_name FUSE_SLEEP1_bp = FUSE_SLEEP_1_bp;
    #define FUSE_SLEEP1_bp FUSE_SLEEP_1_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(FUSE_ACTIVE_0_bm) && defined(FUSE_ACTIVE0_bm)
    #define FUSE_ACTIVE_0_bm FUSE_ACTIVE0_bm
  #elif defined(FUSE_ACTIVE_0_bm)
    //deprecated_constant_name FUSE_ACTIVE0_bm = FUSE_ACTIVE_0_bm;
    #define FUSE_ACTIVE0_bm FUSE_ACTIVE_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(FUSE_ACTIVE_0_bp) && defined(FUSE_ACTIVE0_bp)
    #define FUSE_ACTIVE_0_bp FUSE_ACTIVE0_bp
  #elif defined(FUSE_ACTIVE_0_bp)
    //deprecated_constant_name FUSE_ACTIVE0_bp = FUSE_ACTIVE_0_bp;
    #define FUSE_ACTIVE0_bp FUSE_ACTIVE_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(FUSE_ACTIVE_1_bm) && defined(FUSE_ACTIVE1_bm)
    #define FUSE_ACTIVE_1_bm FUSE_ACTIVE1_bm
  #elif defined(FUSE_ACTIVE_1_bm)
    //deprecated_constant_name FUSE_ACTIVE1_bm = FUSE_ACTIVE_1_bm;
    #define FUSE_ACTIVE1_bm FUSE_ACTIVE_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(FUSE_ACTIVE_1_bp) && defined(FUSE_ACTIVE1_bp)
    #define FUSE_ACTIVE_1_bp FUSE_ACTIVE1_bp
  #elif defined(FUSE_ACTIVE_1_bp)
    //deprecated_constant_name FUSE_ACTIVE1_bp = FUSE_ACTIVE_1_bp;
    #define FUSE_ACTIVE1_bp FUSE_ACTIVE_1_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(FUSE_LVL_0_bm) && defined(FUSE_LVL0_bm)
    #define FUSE_LVL_0_bm FUSE_LVL0_bm
  #elif defined(FUSE_LVL_0_bm)
    //deprecated_constant_name FUSE_LVL0_bm = FUSE_LVL_0_bm;
    #define FUSE_LVL0_bm FUSE_LVL_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(FUSE_LVL_0_bp) && defined(FUSE_LVL0_bp)
    #define FUSE_LVL_0_bp FUSE_LVL0_bp
  #elif defined(FUSE_LVL_0_bp)
    //deprecated_constant_name FUSE_LVL0_bp = FUSE_LVL_0_bp;
    #define FUSE_LVL0_bp FUSE_LVL_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(FUSE_LVL_1_bm) && defined(FUSE_LVL1_bm)
    #define FUSE_LVL_1_bm FUSE_LVL1_bm
  #elif defined(FUSE_LVL_1_bm)
    //deprecated_constant_name FUSE_LVL1_bm = FUSE_LVL_1_bm;
    #define FUSE_LVL1_bm FUSE_LVL_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(FUSE_LVL_1_bp) && defined(FUSE_LVL1_bp)
    #define FUSE_LVL_1_bp FUSE_LVL1_bp
  #elif defined(FUSE_LVL_1_bp)
    //deprecated_constant_name FUSE_LVL1_bp = FUSE_LVL_1_bp;
    #define FUSE_LVL1_bp FUSE_LVL_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(FUSE_LVL_2_bm) && defined(FUSE_LVL2_bm)
    #define FUSE_LVL_2_bm FUSE_LVL2_bm
  #elif defined(FUSE_LVL_2_bm)
    //deprecated_constant_name FUSE_LVL2_bm = FUSE_LVL_2_bm;
    #define FUSE_LVL2_bm FUSE_LVL_2_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(FUSE_LVL_2_bp) && defined(FUSE_LVL2_bp)
    #define FUSE_LVL_2_bp FUSE_LVL2_bp
  #elif defined(FUSE_LVL_2_bp)
    //deprecated_constant_name FUSE_LVL2_bp = FUSE_LVL_2_bp;
    #define FUSE_LVL2_bp FUSE_LVL_2_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(FUSE_CLKSEL_0_bm) && defined(FUSE_CLKSEL0_bm)
    #define FUSE_CLKSEL_0_bm FUSE_CLKSEL0_bm
  #elif defined(FUSE_CLKSEL_0_bm)
    //deprecated_constant_name FUSE_CLKSEL0_bm = FUSE_CLKSEL_0_bm;
    #define FUSE_CLKSEL0_bm FUSE_CLKSEL_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(FUSE_CLKSEL_0_bp) && defined(FUSE_CLKSEL0_bp)
    #define FUSE_CLKSEL_0_bp FUSE_CLKSEL0_bp
  #elif defined(FUSE_CLKSEL_0_bp)
    //deprecated_constant_name FUSE_CLKSEL0_bp = FUSE_CLKSEL_0_bp;
    #define FUSE_CLKSEL0_bp FUSE_CLKSEL_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(FUSE_CLKSEL_1_bm) && defined(FUSE_CLKSEL1_bm)
    #define FUSE_CLKSEL_1_bm FUSE_CLKSEL1_bm
  #elif defined(FUSE_CLKSEL_1_bm)
    //deprecated_constant_name FUSE_CLKSEL1_bm = FUSE_CLKSEL_1_bm;
    #define FUSE_CLKSEL1_bm FUSE_CLKSEL_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(FUSE_CLKSEL_1_bp) && defined(FUSE_CLKSEL1_bp)
    #define FUSE_CLKSEL_1_bp FUSE_CLKSEL1_bp
  #elif defined(FUSE_CLKSEL_1_bp)
    //deprecated_constant_name FUSE_CLKSEL1_bp = FUSE_CLKSEL_1_bp;
    #define FUSE_CLKSEL1_bp FUSE_CLKSEL_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(FUSE_CLKSEL_2_bm) && defined(FUSE_CLKSEL2_bm)
    #define FUSE_CLKSEL_2_bm FUSE_CLKSEL2_bm
  #elif defined(FUSE_CLKSEL_2_bm)
    //deprecated_constant_name FUSE_CLKSEL2_bm = FUSE_CLKSEL_2_bm;
    #define FUSE_CLKSEL2_bm FUSE_CLKSEL_2_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(FUSE_CLKSEL_2_bp) && defined(FUSE_CLKSEL2_bp)
    #define FUSE_CLKSEL_2_bp FUSE_CLKSEL2_bp
  #elif defined(FUSE_CLKSEL_2_bp)
    //deprecated_constant_name FUSE_CLKSEL2_bp = FUSE_CLKSEL_2_bp;
    #define FUSE_CLKSEL2_bp FUSE_CLKSEL_2_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(FUSE_RSTPINCFG_0_bm) && defined(FUSE_RSTPINCFG0_bm)
    #define FUSE_RSTPINCFG_0_bm FUSE_RSTPINCFG0_bm
  #elif defined(FUSE_RSTPINCFG_0_bm)
    //deprecated_constant_name FUSE_RSTPINCFG0_bm = FUSE_RSTPINCFG_0_bm;
    #define FUSE_RSTPINCFG0_bm FUSE_RSTPINCFG_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(FUSE_RSTPINCFG_0_bp) && defined(FUSE_RSTPINCFG0_bp)
    #define FUSE_RSTPINCFG_0_bp FUSE_RSTPINCFG0_bp
  #elif defined(FUSE_RSTPINCFG_0_bp)
    //deprecated_constant_name FUSE_RSTPINCFG0_bp = FUSE_RSTPINCFG_0_bp;
    #define FUSE_RSTPINCFG0_bp FUSE_RSTPINCFG_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(FUSE_RSTPINCFG_1_bm) && defined(FUSE_RSTPINCFG1_bm)
    #define FUSE_RSTPINCFG_1_bm FUSE_RSTPINCFG1_bm
  #elif defined(FUSE_RSTPINCFG_1_bm)
    //deprecated_constant_name FUSE_RSTPINCFG1_bm = FUSE_RSTPINCFG_1_bm;
    #define FUSE_RSTPINCFG1_bm FUSE_RSTPINCFG_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(FUSE_RSTPINCFG_1_bp) && defined(FUSE_RSTPINCFG1_bp)
    #define FUSE_RSTPINCFG_1_bp FUSE_RSTPINCFG1_bp
  #elif defined(FUSE_RSTPINCFG_1_bp)
    //deprecated_constant_name FUSE_RSTPINCFG1_bp = FUSE_RSTPINCFG_1_bp;
    #define FUSE_RSTPINCFG1_bp FUSE_RSTPINCFG_1_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(FUSE_CRCSRC_0_bm) && defined(FUSE_CRCSRC0_bm)
    #define FUSE_CRCSRC_0_bm FUSE_CRCSRC0_bm
  #elif defined(FUSE_CRCSRC_0_bm)
    //deprecated_constant_name FUSE_CRCSRC0_bm = FUSE_CRCSRC_0_bm;
    #define FUSE_CRCSRC0_bm FUSE_CRCSRC_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(FUSE_CRCSRC_0_bp) && defined(FUSE_CRCSRC0_bp)
    #define FUSE_CRCSRC_0_bp FUSE_CRCSRC0_bp
  #elif defined(FUSE_CRCSRC_0_bp)
    //deprecated_constant_name FUSE_CRCSRC0_bp = FUSE_CRCSRC_0_bp;
    #define FUSE_CRCSRC0_bp FUSE_CRCSRC_0_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(FUSE_CRCSRC_1_bm) && defined(FUSE_CRCSRC1_bm)
    #define FUSE_CRCSRC_1_bm FUSE_CRCSRC1_bm
  #elif defined(FUSE_CRCSRC_1_bm)
    //deprecated_constant_name FUSE_CRCSRC1_bm = FUSE_CRCSRC_1_bm;
    #define FUSE_CRCSRC1_bm FUSE_CRCSRC_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(FUSE_CRCSRC_1_bp) && defined(FUSE_CRCSRC1_bp)
    #define FUSE_CRCSRC_1_bp FUSE_CRCSRC1_bp
  #elif defined(FUSE_CRCSRC_1_bp)
    //deprecated_constant_name FUSE_CRCSRC1_bp = FUSE_CRCSRC_1_bp;
    #define FUSE_CRCSRC1_bp FUSE_CRCSRC_1_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(FUSE_SUT_0_bm) && defined(FUSE_SUT0_bm)
    #define FUSE_SUT_0_bm FUSE_SUT0_bm
  #elif defined(FUSE_SUT_0_bm)
    //deprecated_constant_name FUSE_SUT0_bm = FUSE_SUT_0_bm;
    #define FUSE_SUT0_bm FUSE_SUT_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(FUSE_SUT_0_bp) && defined(FUSE_SUT0_bp)
    #define FUSE_SUT_0_bp FUSE_SUT0_bp
  #elif defined(FUSE_SUT_0_bp)
    //deprecated_constant_name FUSE_SUT0_bp = FUSE_SUT_0_bp;
    #define FUSE_SUT0_bp FUSE_SUT_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(FUSE_SUT_1_bm) && defined(FUSE_SUT1_bm)
    #define FUSE_SUT_1_bm FUSE_SUT1_bm
  #elif defined(FUSE_SUT_1_bm)
    //deprecated_constant_name FUSE_SUT1_bm = FUSE_SUT_1_bm;
    #define FUSE_SUT1_bm FUSE_SUT_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(FUSE_SUT_1_bp) && defined(FUSE_SUT1_bp)
    #define FUSE_SUT_1_bp FUSE_SUT1_bp
  #elif defined(FUSE_SUT_1_bp)
    //deprecated_constant_name FUSE_SUT1_bp = FUSE_SUT_1_bp;
    #define FUSE_SUT1_bp FUSE_SUT_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(FUSE_SUT_2_bm) && defined(FUSE_SUT2_bm)
    #define FUSE_SUT_2_bm FUSE_SUT2_bm
  #elif defined(FUSE_SUT_2_bm)
    //deprecated_constant_name FUSE_SUT2_bm = FUSE_SUT_2_bm;
    #define FUSE_SUT2_bm FUSE_SUT_2_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(FUSE_SUT_2_bp) && defined(FUSE_SUT2_bp)
    #define FUSE_SUT_2_bp FUSE_SUT2_bp
  #elif defined(FUSE_SUT_2_bp)
    //deprecated_constant_name FUSE_SUT2_bp = FUSE_SUT_2_bp;
    #define FUSE_SUT2_bp FUSE_SUT_2_bp //Deprecated as of Q2 2022 header change.
  #endif

  /* ======= NVMCTRL ======= */
  #if !defined(NVMCTRL_CMD_0_bm) && defined(NVMCTRL_CMD0_bm)
    #define NVMCTRL_CMD_0_bm NVMCTRL_CMD0_bm
  #elif defined(NVMCTRL_CMD_0_bm)
    //deprecated_constant_name NVMCTRL_CMD0_bm = NVMCTRL_CMD_0_bm;
    #define NVMCTRL_CMD0_bm NVMCTRL_CMD_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(NVMCTRL_CMD_0_bp) && defined(NVMCTRL_CMD0_bp)
    #define NVMCTRL_CMD_0_bp NVMCTRL_CMD0_bp
  #elif defined(NVMCTRL_CMD_0_bp)
    //deprecated_constant_name NVMCTRL_CMD0_bp = NVMCTRL_CMD_0_bp;
    #define NVMCTRL_CMD0_bp NVMCTRL_CMD_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(NVMCTRL_CMD_1_bm) && defined(NVMCTRL_CMD1_bm)
    #define NVMCTRL_CMD_1_bm NVMCTRL_CMD1_bm
  #elif defined(NVMCTRL_CMD_1_bm)
    //deprecated_constant_name NVMCTRL_CMD1_bm = NVMCTRL_CMD_1_bm;
    #define NVMCTRL_CMD1_bm NVMCTRL_CMD_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(NVMCTRL_CMD_1_bp) && defined(NVMCTRL_CMD1_bp)
    #define NVMCTRL_CMD_1_bp NVMCTRL_CMD1_bp
  #elif defined(NVMCTRL_CMD_1_bp)
    //deprecated_constant_name NVMCTRL_CMD1_bp = NVMCTRL_CMD_1_bp;
    #define NVMCTRL_CMD1_bp NVMCTRL_CMD_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(NVMCTRL_CMD_2_bm) && defined(NVMCTRL_CMD2_bm)
    #define NVMCTRL_CMD_2_bm NVMCTRL_CMD2_bm
  #elif defined(NVMCTRL_CMD_2_bm)
    //deprecated_constant_name NVMCTRL_CMD2_bm = NVMCTRL_CMD_2_bm;
    #define NVMCTRL_CMD2_bm NVMCTRL_CMD_2_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(NVMCTRL_CMD_2_bp) && defined(NVMCTRL_CMD2_bp)
    #define NVMCTRL_CMD_2_bp NVMCTRL_CMD2_bp
  #elif defined(NVMCTRL_CMD_2_bp)
    //deprecated_constant_name NVMCTRL_CMD2_bp = NVMCTRL_CMD_2_bp;
    #define NVMCTRL_CMD2_bp NVMCTRL_CMD_2_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(NVMCTRL_CMD_3_bm) && defined(NVMCTRL_CMD3_bm)
    #define NVMCTRL_CMD_3_bm NVMCTRL_CMD3_bm
  #elif defined(NVMCTRL_CMD_3_bm)
    //deprecated_constant_name NVMCTRL_CMD3_bm = NVMCTRL_CMD_3_bm;
    #define NVMCTRL_CMD3_bm NVMCTRL_CMD_3_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(NVMCTRL_CMD_3_bp) && defined(NVMCTRL_CMD3_bp)
    #define NVMCTRL_CMD_3_bp NVMCTRL_CMD3_bp
  #elif defined(NVMCTRL_CMD_3_bp)
    //deprecated_constant_name NVMCTRL_CMD3_bp = NVMCTRL_CMD_3_bp;
    #define NVMCTRL_CMD3_bp NVMCTRL_CMD_3_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(NVMCTRL_CMD_4_bm) && defined(NVMCTRL_CMD4_bm)
    #define NVMCTRL_CMD_4_bm NVMCTRL_CMD4_bm
  #elif defined(NVMCTRL_CMD_4_bm)
    //deprecated_constant_name NVMCTRL_CMD4_bm = NVMCTRL_CMD_4_bm;
    #define NVMCTRL_CMD4_bm NVMCTRL_CMD_4_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(NVMCTRL_CMD_4_bp) && defined(NVMCTRL_CMD4_bp)
    #define NVMCTRL_CMD_4_bp NVMCTRL_CMD4_bp
  #elif defined(NVMCTRL_CMD_4_bp)
    //deprecated_constant_name NVMCTRL_CMD4_bp = NVMCTRL_CMD_4_bp;
    #define NVMCTRL_CMD4_bp NVMCTRL_CMD_4_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(NVMCTRL_CMD_5_bm) && defined(NVMCTRL_CMD5_bm)
    #define NVMCTRL_CMD_5_bm NVMCTRL_CMD5_bm
  #elif defined(NVMCTRL_CMD_5_bm)
    //deprecated_constant_name NVMCTRL_CMD5_bm = NVMCTRL_CMD_5_bm;
    #define NVMCTRL_CMD5_bm NVMCTRL_CMD_5_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(NVMCTRL_CMD_5_bp) && defined(NVMCTRL_CMD5_bp)
    #define NVMCTRL_CMD_5_bp NVMCTRL_CMD5_bp
  #elif defined(NVMCTRL_CMD_5_bp)
    //deprecated_constant_name NVMCTRL_CMD5_bp = NVMCTRL_CMD_5_bp;
    #define NVMCTRL_CMD5_bp NVMCTRL_CMD_5_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(NVMCTRL_CMD_6_bm) && defined(NVMCTRL_CMD6_bm)
    #define NVMCTRL_CMD_6_bm NVMCTRL_CMD6_bm
  #elif defined(NVMCTRL_CMD_6_bm)
    //deprecated_constant_name NVMCTRL_CMD6_bm = NVMCTRL_CMD_6_bm;
    #define NVMCTRL_CMD6_bm NVMCTRL_CMD_6_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(NVMCTRL_CMD_6_bp) && defined(NVMCTRL_CMD6_bp)
    #define NVMCTRL_CMD_6_bp NVMCTRL_CMD6_bp
  #elif defined(NVMCTRL_CMD_6_bp)
    //deprecated_constant_name NVMCTRL_CMD6_bp = NVMCTRL_CMD_6_bp;
    #define NVMCTRL_CMD6_bp NVMCTRL_CMD_6_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(NVMCTRL_FLMAP_0_bm) && defined(NVMCTRL_FLMAP0_bm)
    #define NVMCTRL_FLMAP_0_bm NVMCTRL_FLMAP0_bm
  #elif defined(NVMCTRL_FLMAP_0_bm)
    //deprecated_constant_name NVMCTRL_FLMAP0_bm = NVMCTRL_FLMAP_0_bm;
    #define NVMCTRL_FLMAP0_bm NVMCTRL_FLMAP_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(NVMCTRL_FLMAP_0_bp) && defined(NVMCTRL_FLMAP0_bp)
    #define NVMCTRL_FLMAP_0_bp NVMCTRL_FLMAP0_bp
  #elif defined(NVMCTRL_FLMAP_0_bp)
    //deprecated_constant_name NVMCTRL_FLMAP0_bp = NVMCTRL_FLMAP_0_bp;
    #define NVMCTRL_FLMAP0_bp NVMCTRL_FLMAP_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(NVMCTRL_FLMAP_1_bm) && defined(NVMCTRL_FLMAP1_bm)
    #define NVMCTRL_FLMAP_1_bm NVMCTRL_FLMAP1_bm
  #elif defined(NVMCTRL_FLMAP_1_bm)
    //deprecated_constant_name NVMCTRL_FLMAP1_bm = NVMCTRL_FLMAP_1_bm;
    #define NVMCTRL_FLMAP1_bm NVMCTRL_FLMAP_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(NVMCTRL_FLMAP_1_bp) && defined(NVMCTRL_FLMAP1_bp)
    #define NVMCTRL_FLMAP_1_bp NVMCTRL_FLMAP1_bp
  #elif defined(NVMCTRL_FLMAP_1_bp)
    //deprecated_constant_name NVMCTRL_FLMAP1_bp = NVMCTRL_FLMAP_1_bp;
    #define NVMCTRL_FLMAP1_bp NVMCTRL_FLMAP_1_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(NVMCTRL_ERROR_0_bm) && defined(NVMCTRL_ERROR0_bm)
    #define NVMCTRL_ERROR_0_bm NVMCTRL_ERROR0_bm
  #elif defined(NVMCTRL_ERROR_0_bm)
    //deprecated_constant_name NVMCTRL_ERROR0_bm = NVMCTRL_ERROR_0_bm;
    #define NVMCTRL_ERROR0_bm NVMCTRL_ERROR_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(NVMCTRL_ERROR_0_bp) && defined(NVMCTRL_ERROR0_bp)
    #define NVMCTRL_ERROR_0_bp NVMCTRL_ERROR0_bp
  #elif defined(NVMCTRL_ERROR_0_bp)
    //deprecated_constant_name NVMCTRL_ERROR0_bp = NVMCTRL_ERROR_0_bp;
    #define NVMCTRL_ERROR0_bp NVMCTRL_ERROR_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(NVMCTRL_ERROR_1_bm) && defined(NVMCTRL_ERROR1_bm)
    #define NVMCTRL_ERROR_1_bm NVMCTRL_ERROR1_bm
  #elif defined(NVMCTRL_ERROR_1_bm)
    //deprecated_constant_name NVMCTRL_ERROR1_bm = NVMCTRL_ERROR_1_bm;
    #define NVMCTRL_ERROR1_bm NVMCTRL_ERROR_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(NVMCTRL_ERROR_1_bp) && defined(NVMCTRL_ERROR1_bp)
    #define NVMCTRL_ERROR_1_bp NVMCTRL_ERROR1_bp
  #elif defined(NVMCTRL_ERROR_1_bp)
    //deprecated_constant_name NVMCTRL_ERROR1_bp = NVMCTRL_ERROR_1_bp;
    #define NVMCTRL_ERROR1_bp NVMCTRL_ERROR_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(NVMCTRL_ERROR_2_bm) && defined(NVMCTRL_ERROR2_bm)
    #define NVMCTRL_ERROR_2_bm NVMCTRL_ERROR2_bm
  #elif defined(NVMCTRL_ERROR_2_bm)
    //deprecated_constant_name NVMCTRL_ERROR2_bm = NVMCTRL_ERROR_2_bm;
    #define NVMCTRL_ERROR2_bm NVMCTRL_ERROR_2_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(NVMCTRL_ERROR_2_bp) && defined(NVMCTRL_ERROR2_bp)
    #define NVMCTRL_ERROR_2_bp NVMCTRL_ERROR2_bp
  #elif defined(NVMCTRL_ERROR_2_bp)
    //deprecated_constant_name NVMCTRL_ERROR2_bp = NVMCTRL_ERROR_2_bp;
    #define NVMCTRL_ERROR2_bp NVMCTRL_ERROR_2_bp //Deprecated as of Q2 2022 header change.
  #endif

  /* ======= PORT ======= */
  #if !defined(PORT_INT_0_bm) && defined(PORT_INT0_bm)
    #define PORT_INT_0_bm PORT_INT0_bm
  #elif defined(PORT_INT_0_bm)
    //deprecated_constant_name PORT_INT0_bm = PORT_INT_0_bm;
    #define PORT_INT0_bm PORT_INT_0_bm //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(PORT_INT_0_bp) && defined(PORT_INT0_bp)
    #define PORT_INT_0_bp PORT_INT0_bp
  #elif defined(PORT_INT_0_bp)
    //deprecated_constant_name PORT_INT0_bp = PORT_INT_0_bp;
    #define PORT_INT0_bp PORT_INT_0_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(PORT_INT_1_bm) && defined(PORT_INT1_bm)
    #define PORT_INT_1_bm PORT_INT1_bm
  #elif defined(PORT_INT_1_bm)
    //deprecated_constant_name PORT_INT1_bm = PORT_INT_1_bm;
    #define PORT_INT1_bm PORT_INT_1_bm //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(PORT_INT_1_bp) && defined(PORT_INT1_bp)
    #define PORT_INT_1_bp PORT_INT1_bp
  #elif defined(PORT_INT_1_bp)
    //deprecated_constant_name PORT_INT1_bp = PORT_INT_1_bp;
    #define PORT_INT1_bp PORT_INT_1_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(PORT_INT_2_bm) && defined(PORT_INT2_bm)
    #define PORT_INT_2_bm PORT_INT2_bm
  #elif defined(PORT_INT_2_bm)
    //deprecated_constant_name PORT_INT2_bm = PORT_INT_2_bm;
    #define PORT_INT2_bm PORT_INT_2_bm //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(PORT_INT_2_bp) && defined(PORT_INT2_bp)
    #define PORT_INT_2_bp PORT_INT2_bp
  #elif defined(PORT_INT_2_bp)
    //deprecated_constant_name PORT_INT2_bp = PORT_INT_2_bp;
    #define PORT_INT2_bp PORT_INT_2_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(PORT_INT_3_bm) && defined(PORT_INT3_bm)
    #define PORT_INT_3_bm PORT_INT3_bm
  #elif defined(PORT_INT_3_bm)
    //deprecated_constant_name PORT_INT3_bm = PORT_INT_3_bm;
    #define PORT_INT3_bm PORT_INT_3_bm //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(PORT_INT_3_bp) && defined(PORT_INT3_bp)
    #define PORT_INT_3_bp PORT_INT3_bp
  #elif defined(PORT_INT_3_bp)
    //deprecated_constant_name PORT_INT3_bp = PORT_INT_3_bp;
    #define PORT_INT3_bp PORT_INT_3_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(PORT_INT_4_bm) && defined(PORT_INT4_bm)
    #define PORT_INT_4_bm PORT_INT4_bm
  #elif defined(PORT_INT_4_bm)
    //deprecated_constant_name PORT_INT4_bm = PORT_INT_4_bm;
    #define PORT_INT4_bm PORT_INT_4_bm //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(PORT_INT_4_bp) && defined(PORT_INT4_bp)
    #define PORT_INT_4_bp PORT_INT4_bp
  #elif defined(PORT_INT_4_bp)
    //deprecated_constant_name PORT_INT4_bp = PORT_INT_4_bp;
    #define PORT_INT4_bp PORT_INT_4_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(PORT_INT_5_bm) && defined(PORT_INT5_bm)
    #define PORT_INT_5_bm PORT_INT5_bm
  #elif defined(PORT_INT_5_bm)
    //deprecated_constant_name PORT_INT5_bm = PORT_INT_5_bm;
    #define PORT_INT5_bm PORT_INT_5_bm //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(PORT_INT_5_bp) && defined(PORT_INT5_bp)
    #define PORT_INT_5_bp PORT_INT5_bp
  #elif defined(PORT_INT_5_bp)
    //deprecated_constant_name PORT_INT5_bp = PORT_INT_5_bp;
    #define PORT_INT5_bp PORT_INT_5_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(PORT_INT_6_bm) && defined(PORT_INT6_bm)
    #define PORT_INT_6_bm PORT_INT6_bm
  #elif defined(PORT_INT_6_bm)
    //deprecated_constant_name PORT_INT6_bm = PORT_INT_6_bm;
    #define PORT_INT6_bm PORT_INT_6_bm //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(PORT_INT_6_bp) && defined(PORT_INT6_bp)
    #define PORT_INT_6_bp PORT_INT6_bp
  #elif defined(PORT_INT_6_bp)
    //deprecated_constant_name PORT_INT6_bp = PORT_INT_6_bp;
    #define PORT_INT6_bp PORT_INT_6_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(PORT_INT_7_bm) && defined(PORT_INT7_bm)
    #define PORT_INT_7_bm PORT_INT7_bm
  #elif defined(PORT_INT_7_bm)
    //deprecated_constant_name PORT_INT7_bm = PORT_INT_7_bm;
    #define PORT_INT7_bm PORT_INT_7_bm //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(PORT_INT_7_bp) && defined(PORT_INT7_bp)
    #define PORT_INT_7_bp PORT_INT7_bp
  #elif defined(PORT_INT_7_bp)
    //deprecated_constant_name PORT_INT7_bp = PORT_INT_7_bp;
    #define PORT_INT7_bp PORT_INT_7_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(PORT_ISC_0_bm) && defined(PORT_ISC0_bm)
    #define PORT_ISC_0_bm PORT_ISC0_bm
  #elif defined(PORT_ISC_0_bm)
    //deprecated_constant_name PORT_ISC0_bm = PORT_ISC_0_bm;
    #define PORT_ISC0_bm PORT_ISC_0_bm //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(PORT_ISC_0_bp) && defined(PORT_ISC0_bp)
    #define PORT_ISC_0_bp PORT_ISC0_bp
  #elif defined(PORT_ISC_0_bp)
    //deprecated_constant_name PORT_ISC0_bp = PORT_ISC_0_bp;
    #define PORT_ISC0_bp PORT_ISC_0_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(PORT_ISC_1_bm) && defined(PORT_ISC1_bm)
    #define PORT_ISC_1_bm PORT_ISC1_bm
  #elif defined(PORT_ISC_1_bm)
    //deprecated_constant_name PORT_ISC1_bm = PORT_ISC_1_bm;
    #define PORT_ISC1_bm PORT_ISC_1_bm //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(PORT_ISC_1_bp) && defined(PORT_ISC1_bp)
    #define PORT_ISC_1_bp PORT_ISC1_bp
  #elif defined(PORT_ISC_1_bp)
    //deprecated_constant_name PORT_ISC1_bp = PORT_ISC_1_bp;
    #define PORT_ISC1_bp PORT_ISC_1_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(PORT_ISC_2_bm) && defined(PORT_ISC2_bm)
    #define PORT_ISC_2_bm PORT_ISC2_bm
  #elif defined(PORT_ISC_2_bm)
    //deprecated_constant_name PORT_ISC2_bm = PORT_ISC_2_bm;
    #define PORT_ISC2_bm PORT_ISC_2_bm //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(PORT_ISC_2_bp) && defined(PORT_ISC2_bp)
    #define PORT_ISC_2_bp PORT_ISC2_bp
  #elif defined(PORT_ISC_2_bp)
    //deprecated_constant_name PORT_ISC2_bp = PORT_ISC_2_bp;
    #define PORT_ISC2_bp PORT_ISC_2_bp //Deprecated as of Q2 2022 header change.
  #endif

  /* ======= PORTMUX ======= */
  #if !defined(PORTMUX_USART0_0_bm) && defined(PORTMUX_USART00_bm)
    #define PORTMUX_USART0_0_bm PORTMUX_USART00_bm
  #elif defined(PORTMUX_USART0_0_bm)
    //deprecated_constant_name PORTMUX_USART00_bm = PORTMUX_USART0_0_bm;
    #define PORTMUX_USART00_bm PORTMUX_USART0_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(PORTMUX_USART0_0_bp) && defined(PORTMUX_USART00_bp)
    #define PORTMUX_USART0_0_bp PORTMUX_USART00_bp
  #elif defined(PORTMUX_USART0_0_bp)
    //deprecated_constant_name PORTMUX_USART00_bp = PORTMUX_USART0_0_bp;
    #define PORTMUX_USART00_bp PORTMUX_USART0_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(PORTMUX_USART0_1_bm) && defined(PORTMUX_USART01_bm)
    #define PORTMUX_USART0_1_bm PORTMUX_USART01_bm
  #elif defined(PORTMUX_USART0_1_bm)
    //deprecated_constant_name PORTMUX_USART01_bm = PORTMUX_USART0_1_bm;
    #define PORTMUX_USART01_bm PORTMUX_USART0_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(PORTMUX_USART0_1_bp) && defined(PORTMUX_USART01_bp)
    #define PORTMUX_USART0_1_bp PORTMUX_USART01_bp
  #elif defined(PORTMUX_USART0_1_bp)
    //deprecated_constant_name PORTMUX_USART01_bp = PORTMUX_USART0_1_bp;
    #define PORTMUX_USART01_bp PORTMUX_USART0_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(PORTMUX_USART1_0_bm) && defined(PORTMUX_USART10_bm)
    #define PORTMUX_USART1_0_bm PORTMUX_USART10_bm
  #elif defined(PORTMUX_USART1_0_bm)
    //deprecated_constant_name PORTMUX_USART10_bm = PORTMUX_USART1_0_bm;
    #define PORTMUX_USART10_bm PORTMUX_USART1_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(PORTMUX_USART1_0_bp) && defined(PORTMUX_USART10_bp)
    #define PORTMUX_USART1_0_bp PORTMUX_USART10_bp
  #elif defined(PORTMUX_USART1_0_bp)
    //deprecated_constant_name PORTMUX_USART10_bp = PORTMUX_USART1_0_bp;
    #define PORTMUX_USART10_bp PORTMUX_USART1_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(PORTMUX_USART1_1_bm) && defined(PORTMUX_USART11_bm)
    #define PORTMUX_USART1_1_bm PORTMUX_USART11_bm
  #elif defined(PORTMUX_USART1_1_bm)
    //deprecated_constant_name PORTMUX_USART11_bm = PORTMUX_USART1_1_bm;
    #define PORTMUX_USART11_bm PORTMUX_USART1_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(PORTMUX_USART1_1_bp) && defined(PORTMUX_USART11_bp)
    #define PORTMUX_USART1_1_bp PORTMUX_USART11_bp
  #elif defined(PORTMUX_USART1_1_bp)
    //deprecated_constant_name PORTMUX_USART11_bp = PORTMUX_USART1_1_bp;
    #define PORTMUX_USART11_bp PORTMUX_USART1_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(PORTMUX_USART2_0_bm) && defined(PORTMUX_USART20_bm)
    #define PORTMUX_USART2_0_bm PORTMUX_USART20_bm
  #elif defined(PORTMUX_USART2_0_bm)
    //deprecated_constant_name PORTMUX_USART20_bm = PORTMUX_USART2_0_bm;
    #define PORTMUX_USART20_bm PORTMUX_USART2_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(PORTMUX_USART2_0_bp) && defined(PORTMUX_USART20_bp)
    #define PORTMUX_USART2_0_bp PORTMUX_USART20_bp
  #elif defined(PORTMUX_USART2_0_bp)
    //deprecated_constant_name PORTMUX_USART20_bp = PORTMUX_USART2_0_bp;
    #define PORTMUX_USART20_bp PORTMUX_USART2_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(PORTMUX_USART2_1_bm) && defined(PORTMUX_USART21_bm)
    #define PORTMUX_USART2_1_bm PORTMUX_USART21_bm
  #elif defined(PORTMUX_USART2_1_bm)
    //deprecated_constant_name PORTMUX_USART21_bm = PORTMUX_USART2_1_bm;
    #define PORTMUX_USART21_bm PORTMUX_USART2_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(PORTMUX_USART2_1_bp) && defined(PORTMUX_USART21_bp)
    #define PORTMUX_USART2_1_bp PORTMUX_USART21_bp
  #elif defined(PORTMUX_USART2_1_bp)
    //deprecated_constant_name PORTMUX_USART21_bp = PORTMUX_USART2_1_bp;
    #define PORTMUX_USART21_bp PORTMUX_USART2_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(PORTMUX_USART3_0_bm) && defined(PORTMUX_USART30_bm)
    #define PORTMUX_USART3_0_bm PORTMUX_USART30_bm
  #elif defined(PORTMUX_USART3_0_bm)
    //deprecated_constant_name PORTMUX_USART30_bm = PORTMUX_USART3_0_bm;
    #define PORTMUX_USART30_bm PORTMUX_USART3_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(PORTMUX_USART3_0_bp) && defined(PORTMUX_USART30_bp)
    #define PORTMUX_USART3_0_bp PORTMUX_USART30_bp
  #elif defined(PORTMUX_USART3_0_bp)
    //deprecated_constant_name PORTMUX_USART30_bp = PORTMUX_USART3_0_bp;
    #define PORTMUX_USART30_bp PORTMUX_USART3_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(PORTMUX_USART3_1_bm) && defined(PORTMUX_USART31_bm)
    #define PORTMUX_USART3_1_bm PORTMUX_USART31_bm
  #elif defined(PORTMUX_USART3_1_bm)
    //deprecated_constant_name PORTMUX_USART31_bm = PORTMUX_USART3_1_bm;
    #define PORTMUX_USART31_bm PORTMUX_USART3_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(PORTMUX_USART3_1_bp) && defined(PORTMUX_USART31_bp)
    #define PORTMUX_USART3_1_bp PORTMUX_USART31_bp
  #elif defined(PORTMUX_USART3_1_bp)
    //deprecated_constant_name PORTMUX_USART31_bp = PORTMUX_USART3_1_bp;
    #define PORTMUX_USART31_bp PORTMUX_USART3_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(PORTMUX_USART4_0_bm) && defined(PORTMUX_USART40_bm)
    #define PORTMUX_USART4_0_bm PORTMUX_USART40_bm
  #elif defined(PORTMUX_USART4_0_bm)
    //deprecated_constant_name PORTMUX_USART40_bm = PORTMUX_USART4_0_bm;
    #define PORTMUX_USART40_bm PORTMUX_USART4_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(PORTMUX_USART4_0_bp) && defined(PORTMUX_USART40_bp)
    #define PORTMUX_USART4_0_bp PORTMUX_USART40_bp
  #elif defined(PORTMUX_USART4_0_bp)
    //deprecated_constant_name PORTMUX_USART40_bp = PORTMUX_USART4_0_bp;
    #define PORTMUX_USART40_bp PORTMUX_USART4_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(PORTMUX_USART4_1_bm) && defined(PORTMUX_USART41_bm)
    #define PORTMUX_USART4_1_bm PORTMUX_USART41_bm
  #elif defined(PORTMUX_USART4_1_bm)
    //deprecated_constant_name PORTMUX_USART41_bm = PORTMUX_USART4_1_bm;
    #define PORTMUX_USART41_bm PORTMUX_USART4_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(PORTMUX_USART4_1_bp) && defined(PORTMUX_USART41_bp)
    #define PORTMUX_USART4_1_bp PORTMUX_USART41_bp
  #elif defined(PORTMUX_USART4_1_bp)
    //deprecated_constant_name PORTMUX_USART41_bp = PORTMUX_USART4_1_bp;
    #define PORTMUX_USART41_bp PORTMUX_USART4_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(PORTMUX_USART5_0_bm) && defined(PORTMUX_USART50_bm)
    #define PORTMUX_USART5_0_bm PORTMUX_USART50_bm
  #elif defined(PORTMUX_USART5_0_bm)
    //deprecated_constant_name PORTMUX_USART50_bm = PORTMUX_USART5_0_bm;
    #define PORTMUX_USART50_bm PORTMUX_USART5_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(PORTMUX_USART5_0_bp) && defined(PORTMUX_USART50_bp)
    #define PORTMUX_USART5_0_bp PORTMUX_USART50_bp
  #elif defined(PORTMUX_USART5_0_bp)
    //deprecated_constant_name PORTMUX_USART50_bp = PORTMUX_USART5_0_bp;
    #define PORTMUX_USART50_bp PORTMUX_USART5_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(PORTMUX_USART5_1_bm) && defined(PORTMUX_USART51_bm)
    #define PORTMUX_USART5_1_bm PORTMUX_USART51_bm
  #elif defined(PORTMUX_USART5_1_bm)
    //deprecated_constant_name PORTMUX_USART51_bm = PORTMUX_USART5_1_bm;
    #define PORTMUX_USART51_bm PORTMUX_USART5_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(PORTMUX_USART5_1_bp) && defined(PORTMUX_USART51_bp)
    #define PORTMUX_USART5_1_bp PORTMUX_USART51_bp
  #elif defined(PORTMUX_USART5_1_bp)
    //deprecated_constant_name PORTMUX_USART51_bp = PORTMUX_USART5_1_bp;
    #define PORTMUX_USART51_bp PORTMUX_USART5_1_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(PORTMUX_SPI0_0_bm) && defined(PORTMUX_SPI00_bm)
    #define PORTMUX_SPI0_0_bm PORTMUX_SPI00_bm
  #elif defined(PORTMUX_SPI0_0_bm)
    //deprecated_constant_name PORTMUX_SPI00_bm = PORTMUX_SPI0_0_bm;
    #define PORTMUX_SPI00_bm PORTMUX_SPI0_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(PORTMUX_SPI0_0_bp) && defined(PORTMUX_SPI00_bp)
    #define PORTMUX_SPI0_0_bp PORTMUX_SPI00_bp
  #elif defined(PORTMUX_SPI0_0_bp)
    //deprecated_constant_name PORTMUX_SPI00_bp = PORTMUX_SPI0_0_bp;
    #define PORTMUX_SPI00_bp PORTMUX_SPI0_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(PORTMUX_SPI0_1_bm) && defined(PORTMUX_SPI01_bm)
    #define PORTMUX_SPI0_1_bm PORTMUX_SPI01_bm
  #elif defined(PORTMUX_SPI0_1_bm)
    //deprecated_constant_name PORTMUX_SPI01_bm = PORTMUX_SPI0_1_bm;
    #define PORTMUX_SPI01_bm PORTMUX_SPI0_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(PORTMUX_SPI0_1_bp) && defined(PORTMUX_SPI01_bp)
    #define PORTMUX_SPI0_1_bp PORTMUX_SPI01_bp
  #elif defined(PORTMUX_SPI0_1_bp)
    //deprecated_constant_name PORTMUX_SPI01_bp = PORTMUX_SPI0_1_bp;
    #define PORTMUX_SPI01_bp PORTMUX_SPI0_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(PORTMUX_SPI1_0_bm) && defined(PORTMUX_SPI10_bm)
    #define PORTMUX_SPI1_0_bm PORTMUX_SPI10_bm
  #elif defined(PORTMUX_SPI1_0_bm)
    //deprecated_constant_name PORTMUX_SPI10_bm = PORTMUX_SPI1_0_bm;
    #define PORTMUX_SPI10_bm PORTMUX_SPI1_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(PORTMUX_SPI1_0_bp) && defined(PORTMUX_SPI10_bp)
    #define PORTMUX_SPI1_0_bp PORTMUX_SPI10_bp
  #elif defined(PORTMUX_SPI1_0_bp)
    //deprecated_constant_name PORTMUX_SPI10_bp = PORTMUX_SPI1_0_bp;
    #define PORTMUX_SPI10_bp PORTMUX_SPI1_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(PORTMUX_SPI1_1_bm) && defined(PORTMUX_SPI11_bm)
    #define PORTMUX_SPI1_1_bm PORTMUX_SPI11_bm
  #elif defined(PORTMUX_SPI1_1_bm)
    //deprecated_constant_name PORTMUX_SPI11_bm = PORTMUX_SPI1_1_bm;
    #define PORTMUX_SPI11_bm PORTMUX_SPI1_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(PORTMUX_SPI1_1_bp) && defined(PORTMUX_SPI11_bp)
    #define PORTMUX_SPI1_1_bp PORTMUX_SPI11_bp
  #elif defined(PORTMUX_SPI1_1_bp)
    //deprecated_constant_name PORTMUX_SPI11_bp = PORTMUX_SPI1_1_bp;
    #define PORTMUX_SPI11_bp PORTMUX_SPI1_1_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(PORTMUX_TWI0_0_bm) && defined(PORTMUX_TWI00_bm)
    #define PORTMUX_TWI0_0_bm PORTMUX_TWI00_bm
  #elif defined(PORTMUX_TWI0_0_bm)
    //deprecated_constant_name PORTMUX_TWI00_bm = PORTMUX_TWI0_0_bm;
    #define PORTMUX_TWI00_bm PORTMUX_TWI0_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(PORTMUX_TWI0_0_bp) && defined(PORTMUX_TWI00_bp)
    #define PORTMUX_TWI0_0_bp PORTMUX_TWI00_bp
  #elif defined(PORTMUX_TWI0_0_bp)
    //deprecated_constant_name PORTMUX_TWI00_bp = PORTMUX_TWI0_0_bp;
    #define PORTMUX_TWI00_bp PORTMUX_TWI0_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(PORTMUX_TWI0_1_bm) && defined(PORTMUX_TWI01_bm)
    #define PORTMUX_TWI0_1_bm PORTMUX_TWI01_bm
  #elif defined(PORTMUX_TWI0_1_bm)
    //deprecated_constant_name PORTMUX_TWI01_bm = PORTMUX_TWI0_1_bm;
    #define PORTMUX_TWI01_bm PORTMUX_TWI0_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(PORTMUX_TWI0_1_bp) && defined(PORTMUX_TWI01_bp)
    #define PORTMUX_TWI0_1_bp PORTMUX_TWI01_bp
  #elif defined(PORTMUX_TWI0_1_bp)
    //deprecated_constant_name PORTMUX_TWI01_bp = PORTMUX_TWI0_1_bp;
    #define PORTMUX_TWI01_bp PORTMUX_TWI0_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(PORTMUX_TWI1_0_bm) && defined(PORTMUX_TWI10_bm)
    #define PORTMUX_TWI1_0_bm PORTMUX_TWI10_bm
  #elif defined(PORTMUX_TWI1_0_bm)
    //deprecated_constant_name PORTMUX_TWI10_bm = PORTMUX_TWI1_0_bm;
    #define PORTMUX_TWI10_bm PORTMUX_TWI1_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(PORTMUX_TWI1_0_bp) && defined(PORTMUX_TWI10_bp)
    #define PORTMUX_TWI1_0_bp PORTMUX_TWI10_bp
  #elif defined(PORTMUX_TWI1_0_bp)
    //deprecated_constant_name PORTMUX_TWI10_bp = PORTMUX_TWI1_0_bp;
    #define PORTMUX_TWI10_bp PORTMUX_TWI1_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(PORTMUX_TWI1_1_bm) && defined(PORTMUX_TWI11_bm)
    #define PORTMUX_TWI1_1_bm PORTMUX_TWI11_bm
  #elif defined(PORTMUX_TWI1_1_bm)
    //deprecated_constant_name PORTMUX_TWI11_bm = PORTMUX_TWI1_1_bm;
    #define PORTMUX_TWI11_bm PORTMUX_TWI1_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(PORTMUX_TWI1_1_bp) && defined(PORTMUX_TWI11_bp)
    #define PORTMUX_TWI1_1_bp PORTMUX_TWI11_bp
  #elif defined(PORTMUX_TWI1_1_bp)
    //deprecated_constant_name PORTMUX_TWI11_bp = PORTMUX_TWI1_1_bp;
    #define PORTMUX_TWI11_bp PORTMUX_TWI1_1_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(PORTMUX_TCA0_0_bm) && defined(PORTMUX_TCA00_bm)
    #define PORTMUX_TCA0_0_bm PORTMUX_TCA00_bm
  #elif defined(PORTMUX_TCA0_0_bm)
    //deprecated_constant_name PORTMUX_TCA00_bm = PORTMUX_TCA0_0_bm;
    #define PORTMUX_TCA00_bm PORTMUX_TCA0_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(PORTMUX_TCA0_0_bp) && defined(PORTMUX_TCA00_bp)
    #define PORTMUX_TCA0_0_bp PORTMUX_TCA00_bp
  #elif defined(PORTMUX_TCA0_0_bp)
    //deprecated_constant_name PORTMUX_TCA00_bp = PORTMUX_TCA0_0_bp;
    #define PORTMUX_TCA00_bp PORTMUX_TCA0_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(PORTMUX_TCA0_1_bm) && defined(PORTMUX_TCA01_bm)
    #define PORTMUX_TCA0_1_bm PORTMUX_TCA01_bm
  #elif defined(PORTMUX_TCA0_1_bm)
    //deprecated_constant_name PORTMUX_TCA01_bm = PORTMUX_TCA0_1_bm;
    #define PORTMUX_TCA01_bm PORTMUX_TCA0_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(PORTMUX_TCA0_1_bp) && defined(PORTMUX_TCA01_bp)
    #define PORTMUX_TCA0_1_bp PORTMUX_TCA01_bp
  #elif defined(PORTMUX_TCA0_1_bp)
    //deprecated_constant_name PORTMUX_TCA01_bp = PORTMUX_TCA0_1_bp;
    #define PORTMUX_TCA01_bp PORTMUX_TCA0_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(PORTMUX_TCA0_2_bm) && defined(PORTMUX_TCA02_bm)
    #define PORTMUX_TCA0_2_bm PORTMUX_TCA02_bm
  #elif defined(PORTMUX_TCA0_2_bm)
    //deprecated_constant_name PORTMUX_TCA02_bm = PORTMUX_TCA0_2_bm;
    #define PORTMUX_TCA02_bm PORTMUX_TCA0_2_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(PORTMUX_TCA0_2_bp) && defined(PORTMUX_TCA02_bp)
    #define PORTMUX_TCA0_2_bp PORTMUX_TCA02_bp
  #elif defined(PORTMUX_TCA0_2_bp)
    //deprecated_constant_name PORTMUX_TCA02_bp = PORTMUX_TCA0_2_bp;
    #define PORTMUX_TCA02_bp PORTMUX_TCA0_2_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(PORTMUX_TCA1_0_bm) && defined(PORTMUX_TCA10_bm)
    #define PORTMUX_TCA1_0_bm PORTMUX_TCA10_bm
  #elif defined(PORTMUX_TCA1_0_bm)
    //deprecated_constant_name PORTMUX_TCA10_bm = PORTMUX_TCA1_0_bm;
    #define PORTMUX_TCA10_bm PORTMUX_TCA1_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(PORTMUX_TCA1_0_bp) && defined(PORTMUX_TCA10_bp)
    #define PORTMUX_TCA1_0_bp PORTMUX_TCA10_bp
  #elif defined(PORTMUX_TCA1_0_bp)
    //deprecated_constant_name PORTMUX_TCA10_bp = PORTMUX_TCA1_0_bp;
    #define PORTMUX_TCA10_bp PORTMUX_TCA1_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(PORTMUX_TCA1_1_bm) && defined(PORTMUX_TCA11_bm)
    #define PORTMUX_TCA1_1_bm PORTMUX_TCA11_bm
  #elif defined(PORTMUX_TCA1_1_bm)
    //deprecated_constant_name PORTMUX_TCA11_bm = PORTMUX_TCA1_1_bm;
    #define PORTMUX_TCA11_bm PORTMUX_TCA1_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(PORTMUX_TCA1_1_bp) && defined(PORTMUX_TCA11_bp)
    #define PORTMUX_TCA1_1_bp PORTMUX_TCA11_bp
  #elif defined(PORTMUX_TCA1_1_bp)
    //deprecated_constant_name PORTMUX_TCA11_bp = PORTMUX_TCA1_1_bp;
    #define PORTMUX_TCA11_bp PORTMUX_TCA1_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(PORTMUX_TCA1_2_bm) && defined(PORTMUX_TCA12_bm)
    #define PORTMUX_TCA1_2_bm PORTMUX_TCA12_bm
  #elif defined(PORTMUX_TCA1_2_bm)
    //deprecated_constant_name PORTMUX_TCA12_bm = PORTMUX_TCA1_2_bm;
    #define PORTMUX_TCA12_bm PORTMUX_TCA1_2_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(PORTMUX_TCA1_2_bp) && defined(PORTMUX_TCA12_bp)
    #define PORTMUX_TCA1_2_bp PORTMUX_TCA12_bp
  #elif defined(PORTMUX_TCA1_2_bp)
    //deprecated_constant_name PORTMUX_TCA12_bp = PORTMUX_TCA1_2_bp;
    #define PORTMUX_TCA12_bp PORTMUX_TCA1_2_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(PORTMUX_TCD0_0_bm) && defined(PORTMUX_TCD00_bm)
    #define PORTMUX_TCD0_0_bm PORTMUX_TCD00_bm
  #elif defined(PORTMUX_TCD0_0_bm)
    //deprecated_constant_name PORTMUX_TCD00_bm = PORTMUX_TCD0_0_bm;
    #define PORTMUX_TCD00_bm PORTMUX_TCD0_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(PORTMUX_TCD0_0_bp) && defined(PORTMUX_TCD00_bp)
    #define PORTMUX_TCD0_0_bp PORTMUX_TCD00_bp
  #elif defined(PORTMUX_TCD0_0_bp)
    //deprecated_constant_name PORTMUX_TCD00_bp = PORTMUX_TCD0_0_bp;
    #define PORTMUX_TCD00_bp PORTMUX_TCD0_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(PORTMUX_TCD0_1_bm) && defined(PORTMUX_TCD01_bm)
    #define PORTMUX_TCD0_1_bm PORTMUX_TCD01_bm
  #elif defined(PORTMUX_TCD0_1_bm)
    //deprecated_constant_name PORTMUX_TCD01_bm = PORTMUX_TCD0_1_bm;
    #define PORTMUX_TCD01_bm PORTMUX_TCD0_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(PORTMUX_TCD0_1_bp) && defined(PORTMUX_TCD01_bp)
    #define PORTMUX_TCD0_1_bp PORTMUX_TCD01_bp
  #elif defined(PORTMUX_TCD0_1_bp)
    //deprecated_constant_name PORTMUX_TCD01_bp = PORTMUX_TCD0_1_bp;
    #define PORTMUX_TCD01_bp PORTMUX_TCD0_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(PORTMUX_TCD0_2_bm) && defined(PORTMUX_TCD02_bm)
    #define PORTMUX_TCD0_2_bm PORTMUX_TCD02_bm
  #elif defined(PORTMUX_TCD0_2_bm)
    //deprecated_constant_name PORTMUX_TCD02_bm = PORTMUX_TCD0_2_bm;
    #define PORTMUX_TCD02_bm PORTMUX_TCD0_2_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(PORTMUX_TCD0_2_bp) && defined(PORTMUX_TCD02_bp)
    #define PORTMUX_TCD0_2_bp PORTMUX_TCD02_bp
  #elif defined(PORTMUX_TCD0_2_bp)
    //deprecated_constant_name PORTMUX_TCD02_bp = PORTMUX_TCD0_2_bp;
    #define PORTMUX_TCD02_bp PORTMUX_TCD0_2_bp //Deprecated as of Q2 2022 header change.
  #endif

  /* ======= RTC ======= */
  #if !defined(RTC_PRESCALER_0_bm) && defined(RTC_PRESCALER0_bm)
    #define RTC_PRESCALER_0_bm RTC_PRESCALER0_bm
  #elif defined(RTC_PRESCALER_0_bm)
    //deprecated_constant_name RTC_PRESCALER0_bm = RTC_PRESCALER_0_bm;
    #define RTC_PRESCALER0_bm RTC_PRESCALER_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(RTC_PRESCALER_0_bp) && defined(RTC_PRESCALER0_bp)
    #define RTC_PRESCALER_0_bp RTC_PRESCALER0_bp
  #elif defined(RTC_PRESCALER_0_bp)
    //deprecated_constant_name RTC_PRESCALER0_bp = RTC_PRESCALER_0_bp;
    #define RTC_PRESCALER0_bp RTC_PRESCALER_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(RTC_PRESCALER_1_bm) && defined(RTC_PRESCALER1_bm)
    #define RTC_PRESCALER_1_bm RTC_PRESCALER1_bm
  #elif defined(RTC_PRESCALER_1_bm)
    //deprecated_constant_name RTC_PRESCALER1_bm = RTC_PRESCALER_1_bm;
    #define RTC_PRESCALER1_bm RTC_PRESCALER_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(RTC_PRESCALER_1_bp) && defined(RTC_PRESCALER1_bp)
    #define RTC_PRESCALER_1_bp RTC_PRESCALER1_bp
  #elif defined(RTC_PRESCALER_1_bp)
    //deprecated_constant_name RTC_PRESCALER1_bp = RTC_PRESCALER_1_bp;
    #define RTC_PRESCALER1_bp RTC_PRESCALER_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(RTC_PRESCALER_2_bm) && defined(RTC_PRESCALER2_bm)
    #define RTC_PRESCALER_2_bm RTC_PRESCALER2_bm
  #elif defined(RTC_PRESCALER_2_bm)
    //deprecated_constant_name RTC_PRESCALER2_bm = RTC_PRESCALER_2_bm;
    #define RTC_PRESCALER2_bm RTC_PRESCALER_2_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(RTC_PRESCALER_2_bp) && defined(RTC_PRESCALER2_bp)
    #define RTC_PRESCALER_2_bp RTC_PRESCALER2_bp
  #elif defined(RTC_PRESCALER_2_bp)
    //deprecated_constant_name RTC_PRESCALER2_bp = RTC_PRESCALER_2_bp;
    #define RTC_PRESCALER2_bp RTC_PRESCALER_2_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(RTC_PRESCALER_3_bm) && defined(RTC_PRESCALER3_bm)
    #define RTC_PRESCALER_3_bm RTC_PRESCALER3_bm
  #elif defined(RTC_PRESCALER_3_bm)
    //deprecated_constant_name RTC_PRESCALER3_bm = RTC_PRESCALER_3_bm;
    #define RTC_PRESCALER3_bm RTC_PRESCALER_3_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(RTC_PRESCALER_3_bp) && defined(RTC_PRESCALER3_bp)
    #define RTC_PRESCALER_3_bp RTC_PRESCALER3_bp
  #elif defined(RTC_PRESCALER_3_bp)
    //deprecated_constant_name RTC_PRESCALER3_bp = RTC_PRESCALER_3_bp;
    #define RTC_PRESCALER3_bp RTC_PRESCALER_3_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(RTC_ERROR_0_bm) && defined(RTC_ERROR0_bm)
    #define RTC_ERROR_0_bm RTC_ERROR0_bm
  #elif defined(RTC_ERROR_0_bm)
    //deprecated_constant_name RTC_ERROR0_bm = RTC_ERROR_0_bm;
    #define RTC_ERROR0_bm RTC_ERROR_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(RTC_ERROR_0_bp) && defined(RTC_ERROR0_bp)
    #define RTC_ERROR_0_bp RTC_ERROR0_bp
  #elif defined(RTC_ERROR_0_bp)
    //deprecated_constant_name RTC_ERROR0_bp = RTC_ERROR_0_bp;
    #define RTC_ERROR0_bp RTC_ERROR_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(RTC_ERROR_1_bm) && defined(RTC_ERROR1_bm)
    #define RTC_ERROR_1_bm RTC_ERROR1_bm
  #elif defined(RTC_ERROR_1_bm)
    //deprecated_constant_name RTC_ERROR1_bm = RTC_ERROR_1_bm;
    #define RTC_ERROR1_bm RTC_ERROR_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(RTC_ERROR_1_bp) && defined(RTC_ERROR1_bp)
    #define RTC_ERROR_1_bp RTC_ERROR1_bp
  #elif defined(RTC_ERROR_1_bp)
    //deprecated_constant_name RTC_ERROR1_bp = RTC_ERROR_1_bp;
    #define RTC_ERROR1_bp RTC_ERROR_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(RTC_ERROR_2_bm) && defined(RTC_ERROR2_bm)
    #define RTC_ERROR_2_bm RTC_ERROR2_bm
  #elif defined(RTC_ERROR_2_bm)
    //deprecated_constant_name RTC_ERROR2_bm = RTC_ERROR_2_bm;
    #define RTC_ERROR2_bm RTC_ERROR_2_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(RTC_ERROR_2_bp) && defined(RTC_ERROR2_bp)
    #define RTC_ERROR_2_bp RTC_ERROR2_bp
  #elif defined(RTC_ERROR_2_bp)
    //deprecated_constant_name RTC_ERROR2_bp = RTC_ERROR_2_bp;
    #define RTC_ERROR2_bp RTC_ERROR_2_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(RTC_ERROR_3_bm) && defined(RTC_ERROR3_bm)
    #define RTC_ERROR_3_bm RTC_ERROR3_bm
  #elif defined(RTC_ERROR_3_bm)
    //deprecated_constant_name RTC_ERROR3_bm = RTC_ERROR_3_bm;
    #define RTC_ERROR3_bm RTC_ERROR_3_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(RTC_ERROR_3_bp) && defined(RTC_ERROR3_bp)
    #define RTC_ERROR_3_bp RTC_ERROR3_bp
  #elif defined(RTC_ERROR_3_bp)
    //deprecated_constant_name RTC_ERROR3_bp = RTC_ERROR_3_bp;
    #define RTC_ERROR3_bp RTC_ERROR_3_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(RTC_ERROR_4_bm) && defined(RTC_ERROR4_bm)
    #define RTC_ERROR_4_bm RTC_ERROR4_bm
  #elif defined(RTC_ERROR_4_bm)
    //deprecated_constant_name RTC_ERROR4_bm = RTC_ERROR_4_bm;
    #define RTC_ERROR4_bm RTC_ERROR_4_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(RTC_ERROR_4_bp) && defined(RTC_ERROR4_bp)
    #define RTC_ERROR_4_bp RTC_ERROR4_bp
  #elif defined(RTC_ERROR_4_bp)
    //deprecated_constant_name RTC_ERROR4_bp = RTC_ERROR_4_bp;
    #define RTC_ERROR4_bp RTC_ERROR_4_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(RTC_ERROR_5_bm) && defined(RTC_ERROR5_bm)
    #define RTC_ERROR_5_bm RTC_ERROR5_bm
  #elif defined(RTC_ERROR_5_bm)
    //deprecated_constant_name RTC_ERROR5_bm = RTC_ERROR_5_bm;
    #define RTC_ERROR5_bm RTC_ERROR_5_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(RTC_ERROR_5_bp) && defined(RTC_ERROR5_bp)
    #define RTC_ERROR_5_bp RTC_ERROR5_bp
  #elif defined(RTC_ERROR_5_bp)
    //deprecated_constant_name RTC_ERROR5_bp = RTC_ERROR_5_bp;
    #define RTC_ERROR5_bp RTC_ERROR_5_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(RTC_ERROR_6_bm) && defined(RTC_ERROR6_bm)
    #define RTC_ERROR_6_bm RTC_ERROR6_bm
  #elif defined(RTC_ERROR_6_bm)
    //deprecated_constant_name RTC_ERROR6_bm = RTC_ERROR_6_bm;
    #define RTC_ERROR6_bm RTC_ERROR_6_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(RTC_ERROR_6_bp) && defined(RTC_ERROR6_bp)
    #define RTC_ERROR_6_bp RTC_ERROR6_bp
  #elif defined(RTC_ERROR_6_bp)
    //deprecated_constant_name RTC_ERROR6_bp = RTC_ERROR_6_bp;
    #define RTC_ERROR6_bp RTC_ERROR_6_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(RTC_CLKSEL_0_bm) && defined(RTC_CLKSEL0_bm)
    #define RTC_CLKSEL_0_bm RTC_CLKSEL0_bm
  #elif defined(RTC_CLKSEL_0_bm)
    //deprecated_constant_name RTC_CLKSEL0_bm = RTC_CLKSEL_0_bm;
    #define RTC_CLKSEL0_bm RTC_CLKSEL_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(RTC_CLKSEL_0_bp) && defined(RTC_CLKSEL0_bp)
    #define RTC_CLKSEL_0_bp RTC_CLKSEL0_bp
  #elif defined(RTC_CLKSEL_0_bp)
    //deprecated_constant_name RTC_CLKSEL0_bp = RTC_CLKSEL_0_bp;
    #define RTC_CLKSEL0_bp RTC_CLKSEL_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(RTC_CLKSEL_1_bm) && defined(RTC_CLKSEL1_bm)
    #define RTC_CLKSEL_1_bm RTC_CLKSEL1_bm
  #elif defined(RTC_CLKSEL_1_bm)
    //deprecated_constant_name RTC_CLKSEL1_bm = RTC_CLKSEL_1_bm;
    #define RTC_CLKSEL1_bm RTC_CLKSEL_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(RTC_CLKSEL_1_bp) && defined(RTC_CLKSEL1_bp)
    #define RTC_CLKSEL_1_bp RTC_CLKSEL1_bp
  #elif defined(RTC_CLKSEL_1_bp)
    //deprecated_constant_name RTC_CLKSEL1_bp = RTC_CLKSEL_1_bp;
    #define RTC_CLKSEL1_bp RTC_CLKSEL_1_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(RTC_PERIOD_0_bm) && defined(RTC_PERIOD0_bm)
    #define RTC_PERIOD_0_bm RTC_PERIOD0_bm
  #elif defined(RTC_PERIOD_0_bm)
    //deprecated_constant_name RTC_PERIOD0_bm = RTC_PERIOD_0_bm;
    #define RTC_PERIOD0_bm RTC_PERIOD_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(RTC_PERIOD_0_bp) && defined(RTC_PERIOD0_bp)
    #define RTC_PERIOD_0_bp RTC_PERIOD0_bp
  #elif defined(RTC_PERIOD_0_bp)
    //deprecated_constant_name RTC_PERIOD0_bp = RTC_PERIOD_0_bp;
    #define RTC_PERIOD0_bp RTC_PERIOD_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(RTC_PERIOD_1_bm) && defined(RTC_PERIOD1_bm)
    #define RTC_PERIOD_1_bm RTC_PERIOD1_bm
  #elif defined(RTC_PERIOD_1_bm)
    //deprecated_constant_name RTC_PERIOD1_bm = RTC_PERIOD_1_bm;
    #define RTC_PERIOD1_bm RTC_PERIOD_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(RTC_PERIOD_1_bp) && defined(RTC_PERIOD1_bp)
    #define RTC_PERIOD_1_bp RTC_PERIOD1_bp
  #elif defined(RTC_PERIOD_1_bp)
    //deprecated_constant_name RTC_PERIOD1_bp = RTC_PERIOD_1_bp;
    #define RTC_PERIOD1_bp RTC_PERIOD_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(RTC_PERIOD_2_bm) && defined(RTC_PERIOD2_bm)
    #define RTC_PERIOD_2_bm RTC_PERIOD2_bm
  #elif defined(RTC_PERIOD_2_bm)
    //deprecated_constant_name RTC_PERIOD2_bm = RTC_PERIOD_2_bm;
    #define RTC_PERIOD2_bm RTC_PERIOD_2_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(RTC_PERIOD_2_bp) && defined(RTC_PERIOD2_bp)
    #define RTC_PERIOD_2_bp RTC_PERIOD2_bp
  #elif defined(RTC_PERIOD_2_bp)
    //deprecated_constant_name RTC_PERIOD2_bp = RTC_PERIOD_2_bp;
    #define RTC_PERIOD2_bp RTC_PERIOD_2_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(RTC_PERIOD_3_bm) && defined(RTC_PERIOD3_bm)
    #define RTC_PERIOD_3_bm RTC_PERIOD3_bm
  #elif defined(RTC_PERIOD_3_bm)
    //deprecated_constant_name RTC_PERIOD3_bm = RTC_PERIOD_3_bm;
    #define RTC_PERIOD3_bm RTC_PERIOD_3_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(RTC_PERIOD_3_bp) && defined(RTC_PERIOD3_bp)
    #define RTC_PERIOD_3_bp RTC_PERIOD3_bp
  #elif defined(RTC_PERIOD_3_bp)
    //deprecated_constant_name RTC_PERIOD3_bp = RTC_PERIOD_3_bp;
    #define RTC_PERIOD3_bp RTC_PERIOD_3_bp //Deprecated as of Q2 2022 header change.
  #endif

  /* ======= SLPCTRL ======= */
  #if !defined(SLPCTRL_SMODE_0_bm) && defined(SLPCTRL_SMODE0_bm)
    #define SLPCTRL_SMODE_0_bm SLPCTRL_SMODE0_bm
  #elif defined(SLPCTRL_SMODE_0_bm)
    //deprecated_constant_name SLPCTRL_SMODE0_bm = SLPCTRL_SMODE_0_bm;
    #define SLPCTRL_SMODE0_bm SLPCTRL_SMODE_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(SLPCTRL_SMODE_0_bp) && defined(SLPCTRL_SMODE0_bp)
    #define SLPCTRL_SMODE_0_bp SLPCTRL_SMODE0_bp
  #elif defined(SLPCTRL_SMODE_0_bp)
    //deprecated_constant_name SLPCTRL_SMODE0_bp = SLPCTRL_SMODE_0_bp;
    #define SLPCTRL_SMODE0_bp SLPCTRL_SMODE_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(SLPCTRL_SMODE_1_bm) && defined(SLPCTRL_SMODE1_bm)
    #define SLPCTRL_SMODE_1_bm SLPCTRL_SMODE1_bm
  #elif defined(SLPCTRL_SMODE_1_bm)
    //deprecated_constant_name SLPCTRL_SMODE1_bm = SLPCTRL_SMODE_1_bm;
    #define SLPCTRL_SMODE1_bm SLPCTRL_SMODE_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(SLPCTRL_SMODE_1_bp) && defined(SLPCTRL_SMODE1_bp)
    #define SLPCTRL_SMODE_1_bp SLPCTRL_SMODE1_bp
  #elif defined(SLPCTRL_SMODE_1_bp)
    //deprecated_constant_name SLPCTRL_SMODE1_bp = SLPCTRL_SMODE_1_bp;
    #define SLPCTRL_SMODE1_bp SLPCTRL_SMODE_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(SLPCTRL_PMODE_0_bm) && defined(SLPCTRL_PMODE0_bm)
    #define SLPCTRL_PMODE_0_bm SLPCTRL_PMODE0_bm
  #elif defined(SLPCTRL_PMODE_0_bm)
    //deprecated_constant_name SLPCTRL_PMODE0_bm = SLPCTRL_PMODE_0_bm;
    #define SLPCTRL_PMODE0_bm SLPCTRL_PMODE_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(SLPCTRL_PMODE_0_bp) && defined(SLPCTRL_PMODE0_bp)
    #define SLPCTRL_PMODE_0_bp SLPCTRL_PMODE0_bp
  #elif defined(SLPCTRL_PMODE_0_bp)
    //deprecated_constant_name SLPCTRL_PMODE0_bp = SLPCTRL_PMODE_0_bp;
    #define SLPCTRL_PMODE0_bp SLPCTRL_PMODE_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(SLPCTRL_PMODE_1_bm) && defined(SLPCTRL_PMODE1_bm)
    #define SLPCTRL_PMODE_1_bm SLPCTRL_PMODE1_bm
  #elif defined(SLPCTRL_PMODE_1_bm)
    //deprecated_constant_name SLPCTRL_PMODE1_bm = SLPCTRL_PMODE_1_bm;
    #define SLPCTRL_PMODE1_bm SLPCTRL_PMODE_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(SLPCTRL_PMODE_1_bp) && defined(SLPCTRL_PMODE1_bp)
    #define SLPCTRL_PMODE_1_bp SLPCTRL_PMODE1_bp
  #elif defined(SLPCTRL_PMODE_1_bp)
    //deprecated_constant_name SLPCTRL_PMODE1_bp = SLPCTRL_PMODE_1_bp;
    #define SLPCTRL_PMODE1_bp SLPCTRL_PMODE_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(SLPCTRL_PMODE_2_bm) && defined(SLPCTRL_PMODE2_bm)
    #define SLPCTRL_PMODE_2_bm SLPCTRL_PMODE2_bm
  #elif defined(SLPCTRL_PMODE_2_bm)
    //deprecated_constant_name SLPCTRL_PMODE2_bm = SLPCTRL_PMODE_2_bm;
    #define SLPCTRL_PMODE2_bm SLPCTRL_PMODE_2_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(SLPCTRL_PMODE_2_bp) && defined(SLPCTRL_PMODE2_bp)
    #define SLPCTRL_PMODE_2_bp SLPCTRL_PMODE2_bp
  #elif defined(SLPCTRL_PMODE_2_bp)
    //deprecated_constant_name SLPCTRL_PMODE2_bp = SLPCTRL_PMODE_2_bp;
    #define SLPCTRL_PMODE2_bp SLPCTRL_PMODE_2_bp //Deprecated as of Q2 2022 header change.
  #endif

  /* ======= SPI ======= */
  #if !defined(SPI_PRESC_0_bm) && defined(SPI_PRESC0_bm)
    #define SPI_PRESC_0_bm SPI_PRESC0_bm
  #elif defined(SPI_PRESC_0_bm)
    //deprecated_constant_name SPI_PRESC0_bm = SPI_PRESC_0_bm;
    #define SPI_PRESC0_bm SPI_PRESC_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(SPI_PRESC_0_bp) && defined(SPI_PRESC0_bp)
    #define SPI_PRESC_0_bp SPI_PRESC0_bp
  #elif defined(SPI_PRESC_0_bp)
    //deprecated_constant_name SPI_PRESC0_bp = SPI_PRESC_0_bp;
    #define SPI_PRESC0_bp SPI_PRESC_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(SPI_PRESC_1_bm) && defined(SPI_PRESC1_bm)
    #define SPI_PRESC_1_bm SPI_PRESC1_bm
  #elif defined(SPI_PRESC_1_bm)
    //deprecated_constant_name SPI_PRESC1_bm = SPI_PRESC_1_bm;
    #define SPI_PRESC1_bm SPI_PRESC_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(SPI_PRESC_1_bp) && defined(SPI_PRESC1_bp)
    #define SPI_PRESC_1_bp SPI_PRESC1_bp
  #elif defined(SPI_PRESC_1_bp)
    //deprecated_constant_name SPI_PRESC1_bp = SPI_PRESC_1_bp;
    #define SPI_PRESC1_bp SPI_PRESC_1_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(SPI_MODE_0_bm) && defined(SPI_MODE0_bm)
    #define SPI_MODE_0_bm SPI_MODE0_bm
  #elif defined(SPI_MODE_0_bm)
    //deprecated_constant_name SPI_MODE0_bm = SPI_MODE_0_bm;
    #define SPI_MODE0_bm SPI_MODE_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(SPI_MODE_0_bp) && defined(SPI_MODE0_bp)
    #define SPI_MODE_0_bp SPI_MODE0_bp
  #elif defined(SPI_MODE_0_bp)
    //deprecated_constant_name SPI_MODE0_bp = SPI_MODE_0_bp;
    #define SPI_MODE0_bp SPI_MODE_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(SPI_MODE_1_bm) && defined(SPI_MODE1_bm)
    #define SPI_MODE_1_bm SPI_MODE1_bm
  #elif defined(SPI_MODE_1_bm)
    //deprecated_constant_name SPI_MODE1_bm = SPI_MODE_1_bm;
    #define SPI_MODE1_bm SPI_MODE_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(SPI_MODE_1_bp) && defined(SPI_MODE1_bp)
    #define SPI_MODE_1_bp SPI_MODE1_bp
  #elif defined(SPI_MODE_1_bp)
    //deprecated_constant_name SPI_MODE1_bp = SPI_MODE_1_bp;
    #define SPI_MODE1_bp SPI_MODE_1_bp //Deprecated as of Q2 2022 header change.
  #endif

  /* ======= TCA ======= */
  #if !defined(TCA_SINGLE_CLKSEL_0_bm) && defined(TCA_SINGLE_CLKSEL0_bm)
    #define TCA_SINGLE_CLKSEL_0_bm TCA_SINGLE_CLKSEL0_bm
  #elif defined(TCA_SINGLE_CLKSEL_0_bm)
    //deprecated_constant_name TCA_SINGLE_CLKSEL0_bm = TCA_SINGLE_CLKSEL_0_bm;
    #define TCA_SINGLE_CLKSEL0_bm TCA_SINGLE_CLKSEL_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCA_SINGLE_CLKSEL_0_bp) && defined(TCA_SINGLE_CLKSEL0_bp)
    #define TCA_SINGLE_CLKSEL_0_bp TCA_SINGLE_CLKSEL0_bp
  #elif defined(TCA_SINGLE_CLKSEL_0_bp)
    //deprecated_constant_name TCA_SINGLE_CLKSEL0_bp = TCA_SINGLE_CLKSEL_0_bp;
    #define TCA_SINGLE_CLKSEL0_bp TCA_SINGLE_CLKSEL_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCA_SINGLE_CLKSEL_1_bm) && defined(TCA_SINGLE_CLKSEL1_bm)
    #define TCA_SINGLE_CLKSEL_1_bm TCA_SINGLE_CLKSEL1_bm
  #elif defined(TCA_SINGLE_CLKSEL_1_bm)
    //deprecated_constant_name TCA_SINGLE_CLKSEL1_bm = TCA_SINGLE_CLKSEL_1_bm;
    #define TCA_SINGLE_CLKSEL1_bm TCA_SINGLE_CLKSEL_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCA_SINGLE_CLKSEL_1_bp) && defined(TCA_SINGLE_CLKSEL1_bp)
    #define TCA_SINGLE_CLKSEL_1_bp TCA_SINGLE_CLKSEL1_bp
  #elif defined(TCA_SINGLE_CLKSEL_1_bp)
    //deprecated_constant_name TCA_SINGLE_CLKSEL1_bp = TCA_SINGLE_CLKSEL_1_bp;
    #define TCA_SINGLE_CLKSEL1_bp TCA_SINGLE_CLKSEL_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCA_SINGLE_CLKSEL_2_bm) && defined(TCA_SINGLE_CLKSEL2_bm)
    #define TCA_SINGLE_CLKSEL_2_bm TCA_SINGLE_CLKSEL2_bm
  #elif defined(TCA_SINGLE_CLKSEL_2_bm)
    //deprecated_constant_name TCA_SINGLE_CLKSEL2_bm = TCA_SINGLE_CLKSEL_2_bm;
    #define TCA_SINGLE_CLKSEL2_bm TCA_SINGLE_CLKSEL_2_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCA_SINGLE_CLKSEL_2_bp) && defined(TCA_SINGLE_CLKSEL2_bp)
    #define TCA_SINGLE_CLKSEL_2_bp TCA_SINGLE_CLKSEL2_bp
  #elif defined(TCA_SINGLE_CLKSEL_2_bp)
    //deprecated_constant_name TCA_SINGLE_CLKSEL2_bp = TCA_SINGLE_CLKSEL_2_bp;
    #define TCA_SINGLE_CLKSEL2_bp TCA_SINGLE_CLKSEL_2_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(TCA_SINGLE_WGMODE_0_bm) && defined(TCA_SINGLE_WGMODE0_bm)
    #define TCA_SINGLE_WGMODE_0_bm TCA_SINGLE_WGMODE0_bm
  #elif defined(TCA_SINGLE_WGMODE_0_bm)
    //deprecated_constant_name TCA_SINGLE_WGMODE0_bm = TCA_SINGLE_WGMODE_0_bm;
    #define TCA_SINGLE_WGMODE0_bm TCA_SINGLE_WGMODE_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCA_SINGLE_WGMODE_0_bp) && defined(TCA_SINGLE_WGMODE0_bp)
    #define TCA_SINGLE_WGMODE_0_bp TCA_SINGLE_WGMODE0_bp
  #elif defined(TCA_SINGLE_WGMODE_0_bp)
    //deprecated_constant_name TCA_SINGLE_WGMODE0_bp = TCA_SINGLE_WGMODE_0_bp;
    #define TCA_SINGLE_WGMODE0_bp TCA_SINGLE_WGMODE_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCA_SINGLE_WGMODE_1_bm) && defined(TCA_SINGLE_WGMODE1_bm)
    #define TCA_SINGLE_WGMODE_1_bm TCA_SINGLE_WGMODE1_bm
  #elif defined(TCA_SINGLE_WGMODE_1_bm)
    //deprecated_constant_name TCA_SINGLE_WGMODE1_bm = TCA_SINGLE_WGMODE_1_bm;
    #define TCA_SINGLE_WGMODE1_bm TCA_SINGLE_WGMODE_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCA_SINGLE_WGMODE_1_bp) && defined(TCA_SINGLE_WGMODE1_bp)
    #define TCA_SINGLE_WGMODE_1_bp TCA_SINGLE_WGMODE1_bp
  #elif defined(TCA_SINGLE_WGMODE_1_bp)
    //deprecated_constant_name TCA_SINGLE_WGMODE1_bp = TCA_SINGLE_WGMODE_1_bp;
    #define TCA_SINGLE_WGMODE1_bp TCA_SINGLE_WGMODE_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCA_SINGLE_WGMODE_2_bm) && defined(TCA_SINGLE_WGMODE2_bm)
    #define TCA_SINGLE_WGMODE_2_bm TCA_SINGLE_WGMODE2_bm
  #elif defined(TCA_SINGLE_WGMODE_2_bm)
    //deprecated_constant_name TCA_SINGLE_WGMODE2_bm = TCA_SINGLE_WGMODE_2_bm;
    #define TCA_SINGLE_WGMODE2_bm TCA_SINGLE_WGMODE_2_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCA_SINGLE_WGMODE_2_bp) && defined(TCA_SINGLE_WGMODE2_bp)
    #define TCA_SINGLE_WGMODE_2_bp TCA_SINGLE_WGMODE2_bp
  #elif defined(TCA_SINGLE_WGMODE_2_bp)
    //deprecated_constant_name TCA_SINGLE_WGMODE2_bp = TCA_SINGLE_WGMODE_2_bp;
    #define TCA_SINGLE_WGMODE2_bp TCA_SINGLE_WGMODE_2_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(TCA_SINGLE_CMD_0_bm) && defined(TCA_SINGLE_CMD0_bm)
    #define TCA_SINGLE_CMD_0_bm TCA_SINGLE_CMD0_bm
  #elif defined(TCA_SINGLE_CMD_0_bm)
    //deprecated_constant_name TCA_SINGLE_CMD0_bm = TCA_SINGLE_CMD_0_bm;
    #define TCA_SINGLE_CMD0_bm TCA_SINGLE_CMD_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCA_SINGLE_CMD_0_bp) && defined(TCA_SINGLE_CMD0_bp)
    #define TCA_SINGLE_CMD_0_bp TCA_SINGLE_CMD0_bp
  #elif defined(TCA_SINGLE_CMD_0_bp)
    //deprecated_constant_name TCA_SINGLE_CMD0_bp = TCA_SINGLE_CMD_0_bp;
    #define TCA_SINGLE_CMD0_bp TCA_SINGLE_CMD_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCA_SINGLE_CMD_1_bm) && defined(TCA_SINGLE_CMD1_bm)
    #define TCA_SINGLE_CMD_1_bm TCA_SINGLE_CMD1_bm
  #elif defined(TCA_SINGLE_CMD_1_bm)
    //deprecated_constant_name TCA_SINGLE_CMD1_bm = TCA_SINGLE_CMD_1_bm;
    #define TCA_SINGLE_CMD1_bm TCA_SINGLE_CMD_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCA_SINGLE_CMD_1_bp) && defined(TCA_SINGLE_CMD1_bp)
    #define TCA_SINGLE_CMD_1_bp TCA_SINGLE_CMD1_bp
  #elif defined(TCA_SINGLE_CMD_1_bp)
    //deprecated_constant_name TCA_SINGLE_CMD1_bp = TCA_SINGLE_CMD_1_bp;
    #define TCA_SINGLE_CMD1_bp TCA_SINGLE_CMD_1_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(TCA_SINGLE_EVACTA_0_bm) && defined(TCA_SINGLE_EVACTA0_bm)
    #define TCA_SINGLE_EVACTA_0_bm TCA_SINGLE_EVACTA0_bm
  #elif defined(TCA_SINGLE_EVACTA_0_bm)
    //deprecated_constant_name TCA_SINGLE_EVACTA0_bm = TCA_SINGLE_EVACTA_0_bm;
    #define TCA_SINGLE_EVACTA0_bm TCA_SINGLE_EVACTA_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCA_SINGLE_EVACTA_0_bp) && defined(TCA_SINGLE_EVACTA0_bp)
    #define TCA_SINGLE_EVACTA_0_bp TCA_SINGLE_EVACTA0_bp
  #elif defined(TCA_SINGLE_EVACTA_0_bp)
    //deprecated_constant_name TCA_SINGLE_EVACTA0_bp = TCA_SINGLE_EVACTA_0_bp;
    #define TCA_SINGLE_EVACTA0_bp TCA_SINGLE_EVACTA_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCA_SINGLE_EVACTA_1_bm) && defined(TCA_SINGLE_EVACTA1_bm)
    #define TCA_SINGLE_EVACTA_1_bm TCA_SINGLE_EVACTA1_bm
  #elif defined(TCA_SINGLE_EVACTA_1_bm)
    //deprecated_constant_name TCA_SINGLE_EVACTA1_bm = TCA_SINGLE_EVACTA_1_bm;
    #define TCA_SINGLE_EVACTA1_bm TCA_SINGLE_EVACTA_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCA_SINGLE_EVACTA_1_bp) && defined(TCA_SINGLE_EVACTA1_bp)
    #define TCA_SINGLE_EVACTA_1_bp TCA_SINGLE_EVACTA1_bp
  #elif defined(TCA_SINGLE_EVACTA_1_bp)
    //deprecated_constant_name TCA_SINGLE_EVACTA1_bp = TCA_SINGLE_EVACTA_1_bp;
    #define TCA_SINGLE_EVACTA1_bp TCA_SINGLE_EVACTA_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCA_SINGLE_EVACTA_2_bm) && defined(TCA_SINGLE_EVACTA2_bm)
    #define TCA_SINGLE_EVACTA_2_bm TCA_SINGLE_EVACTA2_bm
  #elif defined(TCA_SINGLE_EVACTA_2_bm)
    //deprecated_constant_name TCA_SINGLE_EVACTA2_bm = TCA_SINGLE_EVACTA_2_bm;
    #define TCA_SINGLE_EVACTA2_bm TCA_SINGLE_EVACTA_2_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCA_SINGLE_EVACTA_2_bp) && defined(TCA_SINGLE_EVACTA2_bp)
    #define TCA_SINGLE_EVACTA_2_bp TCA_SINGLE_EVACTA2_bp
  #elif defined(TCA_SINGLE_EVACTA_2_bp)
    //deprecated_constant_name TCA_SINGLE_EVACTA2_bp = TCA_SINGLE_EVACTA_2_bp;
    #define TCA_SINGLE_EVACTA2_bp TCA_SINGLE_EVACTA_2_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCA_SINGLE_EVACTB_0_bm) && defined(TCA_SINGLE_EVACTB0_bm)
    #define TCA_SINGLE_EVACTB_0_bm TCA_SINGLE_EVACTB0_bm
  #elif defined(TCA_SINGLE_EVACTB_0_bm)
    //deprecated_constant_name TCA_SINGLE_EVACTB0_bm = TCA_SINGLE_EVACTB_0_bm;
    #define TCA_SINGLE_EVACTB0_bm TCA_SINGLE_EVACTB_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCA_SINGLE_EVACTB_0_bp) && defined(TCA_SINGLE_EVACTB0_bp)
    #define TCA_SINGLE_EVACTB_0_bp TCA_SINGLE_EVACTB0_bp
  #elif defined(TCA_SINGLE_EVACTB_0_bp)
    //deprecated_constant_name TCA_SINGLE_EVACTB0_bp = TCA_SINGLE_EVACTB_0_bp;
    #define TCA_SINGLE_EVACTB0_bp TCA_SINGLE_EVACTB_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCA_SINGLE_EVACTB_1_bm) && defined(TCA_SINGLE_EVACTB1_bm)
    #define TCA_SINGLE_EVACTB_1_bm TCA_SINGLE_EVACTB1_bm
  #elif defined(TCA_SINGLE_EVACTB_1_bm)
    //deprecated_constant_name TCA_SINGLE_EVACTB1_bm = TCA_SINGLE_EVACTB_1_bm;
    #define TCA_SINGLE_EVACTB1_bm TCA_SINGLE_EVACTB_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCA_SINGLE_EVACTB_1_bp) && defined(TCA_SINGLE_EVACTB1_bp)
    #define TCA_SINGLE_EVACTB_1_bp TCA_SINGLE_EVACTB1_bp
  #elif defined(TCA_SINGLE_EVACTB_1_bp)
    //deprecated_constant_name TCA_SINGLE_EVACTB1_bp = TCA_SINGLE_EVACTB_1_bp;
    #define TCA_SINGLE_EVACTB1_bp TCA_SINGLE_EVACTB_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCA_SINGLE_EVACTB_2_bm) && defined(TCA_SINGLE_EVACTB2_bm)
    #define TCA_SINGLE_EVACTB_2_bm TCA_SINGLE_EVACTB2_bm
  #elif defined(TCA_SINGLE_EVACTB_2_bm)
    //deprecated_constant_name TCA_SINGLE_EVACTB2_bm = TCA_SINGLE_EVACTB_2_bm;
    #define TCA_SINGLE_EVACTB2_bm TCA_SINGLE_EVACTB_2_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCA_SINGLE_EVACTB_2_bp) && defined(TCA_SINGLE_EVACTB2_bp)
    #define TCA_SINGLE_EVACTB_2_bp TCA_SINGLE_EVACTB2_bp
  #elif defined(TCA_SINGLE_EVACTB_2_bp)
    //deprecated_constant_name TCA_SINGLE_EVACTB2_bp = TCA_SINGLE_EVACTB_2_bp;
    #define TCA_SINGLE_EVACTB2_bp TCA_SINGLE_EVACTB_2_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(TCA_SPLIT_CLKSEL_0_bm) && defined(TCA_SPLIT_CLKSEL0_bm)
    #define TCA_SPLIT_CLKSEL_0_bm TCA_SPLIT_CLKSEL0_bm
  #elif defined(TCA_SPLIT_CLKSEL_0_bm)
    //deprecated_constant_name TCA_SPLIT_CLKSEL0_bm = TCA_SPLIT_CLKSEL_0_bm;
    #define TCA_SPLIT_CLKSEL0_bm TCA_SPLIT_CLKSEL_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCA_SPLIT_CLKSEL_0_bp) && defined(TCA_SPLIT_CLKSEL0_bp)
    #define TCA_SPLIT_CLKSEL_0_bp TCA_SPLIT_CLKSEL0_bp
  #elif defined(TCA_SPLIT_CLKSEL_0_bp)
    //deprecated_constant_name TCA_SPLIT_CLKSEL0_bp = TCA_SPLIT_CLKSEL_0_bp;
    #define TCA_SPLIT_CLKSEL0_bp TCA_SPLIT_CLKSEL_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCA_SPLIT_CLKSEL_1_bm) && defined(TCA_SPLIT_CLKSEL1_bm)
    #define TCA_SPLIT_CLKSEL_1_bm TCA_SPLIT_CLKSEL1_bm
  #elif defined(TCA_SPLIT_CLKSEL_1_bm)
    //deprecated_constant_name TCA_SPLIT_CLKSEL1_bm = TCA_SPLIT_CLKSEL_1_bm;
    #define TCA_SPLIT_CLKSEL1_bm TCA_SPLIT_CLKSEL_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCA_SPLIT_CLKSEL_1_bp) && defined(TCA_SPLIT_CLKSEL1_bp)
    #define TCA_SPLIT_CLKSEL_1_bp TCA_SPLIT_CLKSEL1_bp
  #elif defined(TCA_SPLIT_CLKSEL_1_bp)
    //deprecated_constant_name TCA_SPLIT_CLKSEL1_bp = TCA_SPLIT_CLKSEL_1_bp;
    #define TCA_SPLIT_CLKSEL1_bp TCA_SPLIT_CLKSEL_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCA_SPLIT_CLKSEL_2_bm) && defined(TCA_SPLIT_CLKSEL2_bm)
    #define TCA_SPLIT_CLKSEL_2_bm TCA_SPLIT_CLKSEL2_bm
  #elif defined(TCA_SPLIT_CLKSEL_2_bm)
    //deprecated_constant_name TCA_SPLIT_CLKSEL2_bm = TCA_SPLIT_CLKSEL_2_bm;
    #define TCA_SPLIT_CLKSEL2_bm TCA_SPLIT_CLKSEL_2_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCA_SPLIT_CLKSEL_2_bp) && defined(TCA_SPLIT_CLKSEL2_bp)
    #define TCA_SPLIT_CLKSEL_2_bp TCA_SPLIT_CLKSEL2_bp
  #elif defined(TCA_SPLIT_CLKSEL_2_bp)
    //deprecated_constant_name TCA_SPLIT_CLKSEL2_bp = TCA_SPLIT_CLKSEL_2_bp;
    #define TCA_SPLIT_CLKSEL2_bp TCA_SPLIT_CLKSEL_2_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(TCA_SPLIT_CMDEN_0_bm) && defined(TCA_SPLIT_CMDEN0_bm)
    #define TCA_SPLIT_CMDEN_0_bm TCA_SPLIT_CMDEN0_bm
  #elif defined(TCA_SPLIT_CMDEN_0_bm)
    //deprecated_constant_name TCA_SPLIT_CMDEN0_bm = TCA_SPLIT_CMDEN_0_bm;
    #define TCA_SPLIT_CMDEN0_bm TCA_SPLIT_CMDEN_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCA_SPLIT_CMDEN_0_bp) && defined(TCA_SPLIT_CMDEN0_bp)
    #define TCA_SPLIT_CMDEN_0_bp TCA_SPLIT_CMDEN0_bp
  #elif defined(TCA_SPLIT_CMDEN_0_bp)
    //deprecated_constant_name TCA_SPLIT_CMDEN0_bp = TCA_SPLIT_CMDEN_0_bp;
    #define TCA_SPLIT_CMDEN0_bp TCA_SPLIT_CMDEN_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCA_SPLIT_CMDEN_1_bm) && defined(TCA_SPLIT_CMDEN1_bm)
    #define TCA_SPLIT_CMDEN_1_bm TCA_SPLIT_CMDEN1_bm
  #elif defined(TCA_SPLIT_CMDEN_1_bm)
    //deprecated_constant_name TCA_SPLIT_CMDEN1_bm = TCA_SPLIT_CMDEN_1_bm;
    #define TCA_SPLIT_CMDEN1_bm TCA_SPLIT_CMDEN_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCA_SPLIT_CMDEN_1_bp) && defined(TCA_SPLIT_CMDEN1_bp)
    #define TCA_SPLIT_CMDEN_1_bp TCA_SPLIT_CMDEN1_bp
  #elif defined(TCA_SPLIT_CMDEN_1_bp)
    //deprecated_constant_name TCA_SPLIT_CMDEN1_bp = TCA_SPLIT_CMDEN_1_bp;
    #define TCA_SPLIT_CMDEN1_bp TCA_SPLIT_CMDEN_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCA_SPLIT_CMD_0_bm) && defined(TCA_SPLIT_CMD0_bm)
    #define TCA_SPLIT_CMD_0_bm TCA_SPLIT_CMD0_bm
  #elif defined(TCA_SPLIT_CMD_0_bm)
    //deprecated_constant_name TCA_SPLIT_CMD0_bm = TCA_SPLIT_CMD_0_bm;
    #define TCA_SPLIT_CMD0_bm TCA_SPLIT_CMD_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCA_SPLIT_CMD_0_bp) && defined(TCA_SPLIT_CMD0_bp)
    #define TCA_SPLIT_CMD_0_bp TCA_SPLIT_CMD0_bp
  #elif defined(TCA_SPLIT_CMD_0_bp)
    //deprecated_constant_name TCA_SPLIT_CMD0_bp = TCA_SPLIT_CMD_0_bp;
    #define TCA_SPLIT_CMD0_bp TCA_SPLIT_CMD_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCA_SPLIT_CMD_1_bm) && defined(TCA_SPLIT_CMD1_bm)
    #define TCA_SPLIT_CMD_1_bm TCA_SPLIT_CMD1_bm
  #elif defined(TCA_SPLIT_CMD_1_bm)
    //deprecated_constant_name TCA_SPLIT_CMD1_bm = TCA_SPLIT_CMD_1_bm;
    #define TCA_SPLIT_CMD1_bm TCA_SPLIT_CMD_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCA_SPLIT_CMD_1_bp) && defined(TCA_SPLIT_CMD1_bp)
    #define TCA_SPLIT_CMD_1_bp TCA_SPLIT_CMD1_bp
  #elif defined(TCA_SPLIT_CMD_1_bp)
    //deprecated_constant_name TCA_SPLIT_CMD1_bp = TCA_SPLIT_CMD_1_bp;
    #define TCA_SPLIT_CMD1_bp TCA_SPLIT_CMD_1_bp //Deprecated as of Q2 2022 header change.
  #endif

  /* ======= TCBs ======= */
  #if !defined(TCB_CLKSEL_0_bm) && defined(TCB_CLKSEL0_bm)
    #define TCB_CLKSEL_0_bm TCB_CLKSEL0_bm
  #elif defined(TCB_CLKSEL_0_bm)
    //deprecated_constant_name TCB_CLKSEL0_bm = TCB_CLKSEL_0_bm;
    #define TCB_CLKSEL0_bm TCB_CLKSEL_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCB_CLKSEL_0_bp) && defined(TCB_CLKSEL0_bp)
    #define TCB_CLKSEL_0_bp TCB_CLKSEL0_bp
  #elif defined(TCB_CLKSEL_0_bp)
    //deprecated_constant_name TCB_CLKSEL0_bp = TCB_CLKSEL_0_bp;
    #define TCB_CLKSEL0_bp TCB_CLKSEL_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCB_CLKSEL_1_bm) && defined(TCB_CLKSEL1_bm)
    #define TCB_CLKSEL_1_bm TCB_CLKSEL1_bm
  #elif defined(TCB_CLKSEL_1_bm)
    //deprecated_constant_name TCB_CLKSEL1_bm = TCB_CLKSEL_1_bm;
    #define TCB_CLKSEL1_bm TCB_CLKSEL_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCB_CLKSEL_1_bp) && defined(TCB_CLKSEL1_bp)
    #define TCB_CLKSEL_1_bp TCB_CLKSEL1_bp
  #elif defined(TCB_CLKSEL_1_bp)
    //deprecated_constant_name TCB_CLKSEL1_bp = TCB_CLKSEL_1_bp;
    #define TCB_CLKSEL1_bp TCB_CLKSEL_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCB_CLKSEL_2_bm) && defined(TCB_CLKSEL2_bm)
    #define TCB_CLKSEL_2_bm TCB_CLKSEL2_bm
  #elif defined(TCB_CLKSEL_2_bm)
    //deprecated_constant_name TCB_CLKSEL2_bm = TCB_CLKSEL_2_bm;
    #define TCB_CLKSEL2_bm TCB_CLKSEL_2_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCB_CLKSEL_2_bp) && defined(TCB_CLKSEL2_bp)
    #define TCB_CLKSEL_2_bp TCB_CLKSEL2_bp
  #elif defined(TCB_CLKSEL_2_bp)
    //deprecated_constant_name TCB_CLKSEL2_bp = TCB_CLKSEL_2_bp;
    #define TCB_CLKSEL2_bp TCB_CLKSEL_2_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(TCB_CNTMODE_0_bm) && defined(TCB_CNTMODE0_bm)
    #define TCB_CNTMODE_0_bm TCB_CNTMODE0_bm
  #elif defined(TCB_CNTMODE_0_bm)
    //deprecated_constant_name TCB_CNTMODE0_bm = TCB_CNTMODE_0_bm;
    #define TCB_CNTMODE0_bm TCB_CNTMODE_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCB_CNTMODE_0_bp) && defined(TCB_CNTMODE0_bp)
    #define TCB_CNTMODE_0_bp TCB_CNTMODE0_bp
  #elif defined(TCB_CNTMODE_0_bp)
    //deprecated_constant_name TCB_CNTMODE0_bp = TCB_CNTMODE_0_bp;
    #define TCB_CNTMODE0_bp TCB_CNTMODE_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCB_CNTMODE_1_bm) && defined(TCB_CNTMODE1_bm)
    #define TCB_CNTMODE_1_bm TCB_CNTMODE1_bm
  #elif defined(TCB_CNTMODE_1_bm)
    //deprecated_constant_name TCB_CNTMODE1_bm = TCB_CNTMODE_1_bm;
    #define TCB_CNTMODE1_bm TCB_CNTMODE_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCB_CNTMODE_1_bp) && defined(TCB_CNTMODE1_bp)
    #define TCB_CNTMODE_1_bp TCB_CNTMODE1_bp
  #elif defined(TCB_CNTMODE_1_bp)
    //deprecated_constant_name TCB_CNTMODE1_bp = TCB_CNTMODE_1_bp;
    #define TCB_CNTMODE1_bp TCB_CNTMODE_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCB_CNTMODE_2_bm) && defined(TCB_CNTMODE2_bm)
    #define TCB_CNTMODE_2_bm TCB_CNTMODE2_bm
  #elif defined(TCB_CNTMODE_2_bm)
    //deprecated_constant_name TCB_CNTMODE2_bm = TCB_CNTMODE_2_bm;
    #define TCB_CNTMODE2_bm TCB_CNTMODE_2_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCB_CNTMODE_2_bp) && defined(TCB_CNTMODE2_bp)
    #define TCB_CNTMODE_2_bp TCB_CNTMODE2_bp
  #elif defined(TCB_CNTMODE_2_bp)
    //deprecated_constant_name TCB_CNTMODE2_bp = TCB_CNTMODE_2_bp;
    #define TCB_CNTMODE2_bp TCB_CNTMODE_2_bp //Deprecated as of Q2 2022 header change.
  #endif

  /* ======= TCD ======= */
  #if !defined(TCD_SYNCPRES_0_bm) && defined(TCD_SYNCPRES0_bm)
    #define TCD_SYNCPRES_0_bm TCD_SYNCPRES0_bm
  #elif defined(TCD_SYNCPRES_0_bm)
    //deprecated_constant_name TCD_SYNCPRES0_bm = TCD_SYNCPRES_0_bm;
    #define TCD_SYNCPRES0_bm TCD_SYNCPRES_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_SYNCPRES_0_bp) && defined(TCD_SYNCPRES0_bp)
    #define TCD_SYNCPRES_0_bp TCD_SYNCPRES0_bp
  #elif defined(TCD_SYNCPRES_0_bp)
    //deprecated_constant_name TCD_SYNCPRES0_bp = TCD_SYNCPRES_0_bp;
    #define TCD_SYNCPRES0_bp TCD_SYNCPRES_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_SYNCPRES_1_bm) && defined(TCD_SYNCPRES1_bm)
    #define TCD_SYNCPRES_1_bm TCD_SYNCPRES1_bm
  #elif defined(TCD_SYNCPRES_1_bm)
    //deprecated_constant_name TCD_SYNCPRES1_bm = TCD_SYNCPRES_1_bm;
    #define TCD_SYNCPRES1_bm TCD_SYNCPRES_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_SYNCPRES_1_bp) && defined(TCD_SYNCPRES1_bp)
    #define TCD_SYNCPRES_1_bp TCD_SYNCPRES1_bp
  #elif defined(TCD_SYNCPRES_1_bp)
    //deprecated_constant_name TCD_SYNCPRES1_bp = TCD_SYNCPRES_1_bp;
    #define TCD_SYNCPRES1_bp TCD_SYNCPRES_1_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(TCD_CNTPRES_0_bm) && defined(TCD_CNTPRES0_bm)
    #define TCD_CNTPRES_0_bm TCD_CNTPRES0_bm
  #elif defined(TCD_CNTPRES_0_bm)
    //deprecated_constant_name TCD_CNTPRES0_bm = TCD_CNTPRES_0_bm;
    #define TCD_CNTPRES0_bm TCD_CNTPRES_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_CNTPRES_0_bp) && defined(TCD_CNTPRES0_bp)
    #define TCD_CNTPRES_0_bp TCD_CNTPRES0_bp
  #elif defined(TCD_CNTPRES_0_bp)
    //deprecated_constant_name TCD_CNTPRES0_bp = TCD_CNTPRES_0_bp;
    #define TCD_CNTPRES0_bp TCD_CNTPRES_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_CNTPRES_1_bm) && defined(TCD_CNTPRES1_bm)
    #define TCD_CNTPRES_1_bm TCD_CNTPRES1_bm
  #elif defined(TCD_CNTPRES_1_bm)
    //deprecated_constant_name TCD_CNTPRES1_bm = TCD_CNTPRES_1_bm;
    #define TCD_CNTPRES1_bm TCD_CNTPRES_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_CNTPRES_1_bp) && defined(TCD_CNTPRES1_bp)
    #define TCD_CNTPRES_1_bp TCD_CNTPRES1_bp
  #elif defined(TCD_CNTPRES_1_bp)
    //deprecated_constant_name TCD_CNTPRES1_bp = TCD_CNTPRES_1_bp;
    #define TCD_CNTPRES1_bp TCD_CNTPRES_1_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(TCD_CLKSEL_0_bm) && defined(TCD_CLKSEL0_bm)
    #define TCD_CLKSEL_0_bm TCD_CLKSEL0_bm
  #elif defined(TCD_CLKSEL_0_bm)
    //deprecated_constant_name TCD_CLKSEL0_bm = TCD_CLKSEL_0_bm;
    #define TCD_CLKSEL0_bm TCD_CLKSEL_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_CLKSEL_0_bp) && defined(TCD_CLKSEL0_bp)
    #define TCD_CLKSEL_0_bp TCD_CLKSEL0_bp
  #elif defined(TCD_CLKSEL_0_bp)
    //deprecated_constant_name TCD_CLKSEL0_bp = TCD_CLKSEL_0_bp;
    #define TCD_CLKSEL0_bp TCD_CLKSEL_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_CLKSEL_1_bm) && defined(TCD_CLKSEL1_bm)
    #define TCD_CLKSEL_1_bm TCD_CLKSEL1_bm
  #elif defined(TCD_CLKSEL_1_bm)
    //deprecated_constant_name TCD_CLKSEL1_bm = TCD_CLKSEL_1_bm;
    #define TCD_CLKSEL1_bm TCD_CLKSEL_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_CLKSEL_1_bp) && defined(TCD_CLKSEL1_bp)
    #define TCD_CLKSEL_1_bp TCD_CLKSEL1_bp
  #elif defined(TCD_CLKSEL_1_bp)
    //deprecated_constant_name TCD_CLKSEL1_bp = TCD_CLKSEL_1_bp;
    #define TCD_CLKSEL1_bp TCD_CLKSEL_1_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(TCD_WGMODE_0_bm) && defined(TCD_WGMODE0_bm)
    #define TCD_WGMODE_0_bm TCD_WGMODE0_bm
  #elif defined(TCD_WGMODE_0_bm)
    //deprecated_constant_name TCD_WGMODE0_bm = TCD_WGMODE_0_bm;
    #define TCD_WGMODE0_bm TCD_WGMODE_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_WGMODE_0_bp) && defined(TCD_WGMODE0_bp)
    #define TCD_WGMODE_0_bp TCD_WGMODE0_bp
  #elif defined(TCD_WGMODE_0_bp)
    //deprecated_constant_name TCD_WGMODE0_bp = TCD_WGMODE_0_bp;
    #define TCD_WGMODE0_bp TCD_WGMODE_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_WGMODE_1_bm) && defined(TCD_WGMODE1_bm)
    #define TCD_WGMODE_1_bm TCD_WGMODE1_bm
  #elif defined(TCD_WGMODE_1_bm)
    //deprecated_constant_name TCD_WGMODE1_bm = TCD_WGMODE_1_bm;
    #define TCD_WGMODE1_bm TCD_WGMODE_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_WGMODE_1_bp) && defined(TCD_WGMODE1_bp)
    #define TCD_WGMODE_1_bp TCD_WGMODE1_bp
  #elif defined(TCD_WGMODE_1_bp)
    //deprecated_constant_name TCD_WGMODE1_bp = TCD_WGMODE_1_bp;
    #define TCD_WGMODE1_bp TCD_WGMODE_1_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(TCD_CMPAVAL_0_bm) && defined(TCD_CMPAVAL0_bm)
    #define TCD_CMPAVAL_0_bm TCD_CMPAVAL0_bm
  #elif defined(TCD_CMPAVAL_0_bm)
    //deprecated_constant_name TCD_CMPAVAL0_bm = TCD_CMPAVAL_0_bm;
    #define TCD_CMPAVAL0_bm TCD_CMPAVAL_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_CMPAVAL_0_bp) && defined(TCD_CMPAVAL0_bp)
    #define TCD_CMPAVAL_0_bp TCD_CMPAVAL0_bp
  #elif defined(TCD_CMPAVAL_0_bp)
    //deprecated_constant_name TCD_CMPAVAL0_bp = TCD_CMPAVAL_0_bp;
    #define TCD_CMPAVAL0_bp TCD_CMPAVAL_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_CMPAVAL_1_bm) && defined(TCD_CMPAVAL1_bm)
    #define TCD_CMPAVAL_1_bm TCD_CMPAVAL1_bm
  #elif defined(TCD_CMPAVAL_1_bm)
    //deprecated_constant_name TCD_CMPAVAL1_bm = TCD_CMPAVAL_1_bm;
    #define TCD_CMPAVAL1_bm TCD_CMPAVAL_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_CMPAVAL_1_bp) && defined(TCD_CMPAVAL1_bp)
    #define TCD_CMPAVAL_1_bp TCD_CMPAVAL1_bp
  #elif defined(TCD_CMPAVAL_1_bp)
    //deprecated_constant_name TCD_CMPAVAL1_bp = TCD_CMPAVAL_1_bp;
    #define TCD_CMPAVAL1_bp TCD_CMPAVAL_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_CMPAVAL_2_bm) && defined(TCD_CMPAVAL2_bm)
    #define TCD_CMPAVAL_2_bm TCD_CMPAVAL2_bm
  #elif defined(TCD_CMPAVAL_2_bm)
    //deprecated_constant_name TCD_CMPAVAL2_bm = TCD_CMPAVAL_2_bm;
    #define TCD_CMPAVAL2_bm TCD_CMPAVAL_2_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_CMPAVAL_2_bp) && defined(TCD_CMPAVAL2_bp)
    #define TCD_CMPAVAL_2_bp TCD_CMPAVAL2_bp
  #elif defined(TCD_CMPAVAL_2_bp)
    //deprecated_constant_name TCD_CMPAVAL2_bp = TCD_CMPAVAL_2_bp;
    #define TCD_CMPAVAL2_bp TCD_CMPAVAL_2_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_CMPAVAL_3_bm) && defined(TCD_CMPAVAL3_bm)
    #define TCD_CMPAVAL_3_bm TCD_CMPAVAL3_bm
  #elif defined(TCD_CMPAVAL_3_bm)
    //deprecated_constant_name TCD_CMPAVAL3_bm = TCD_CMPAVAL_3_bm;
    #define TCD_CMPAVAL3_bm TCD_CMPAVAL_3_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_CMPAVAL_3_bp) && defined(TCD_CMPAVAL3_bp)
    #define TCD_CMPAVAL_3_bp TCD_CMPAVAL3_bp
  #elif defined(TCD_CMPAVAL_3_bp)
    //deprecated_constant_name TCD_CMPAVAL3_bp = TCD_CMPAVAL_3_bp;
    #define TCD_CMPAVAL3_bp TCD_CMPAVAL_3_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_CMPBVAL_0_bm) && defined(TCD_CMPBVAL0_bm)
    #define TCD_CMPBVAL_0_bm TCD_CMPBVAL0_bm
  #elif defined(TCD_CMPBVAL_0_bm)
    //deprecated_constant_name TCD_CMPBVAL0_bm = TCD_CMPBVAL_0_bm;
    #define TCD_CMPBVAL0_bm TCD_CMPBVAL_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_CMPBVAL_0_bp) && defined(TCD_CMPBVAL0_bp)
    #define TCD_CMPBVAL_0_bp TCD_CMPBVAL0_bp
  #elif defined(TCD_CMPBVAL_0_bp)
    //deprecated_constant_name TCD_CMPBVAL0_bp = TCD_CMPBVAL_0_bp;
    #define TCD_CMPBVAL0_bp TCD_CMPBVAL_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_CMPBVAL_1_bm) && defined(TCD_CMPBVAL1_bm)
    #define TCD_CMPBVAL_1_bm TCD_CMPBVAL1_bm
  #elif defined(TCD_CMPBVAL_1_bm)
    //deprecated_constant_name TCD_CMPBVAL1_bm = TCD_CMPBVAL_1_bm;
    #define TCD_CMPBVAL1_bm TCD_CMPBVAL_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_CMPBVAL_1_bp) && defined(TCD_CMPBVAL1_bp)
    #define TCD_CMPBVAL_1_bp TCD_CMPBVAL1_bp
  #elif defined(TCD_CMPBVAL_1_bp)
    //deprecated_constant_name TCD_CMPBVAL1_bp = TCD_CMPBVAL_1_bp;
    #define TCD_CMPBVAL1_bp TCD_CMPBVAL_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_CMPBVAL_2_bm) && defined(TCD_CMPBVAL2_bm)
    #define TCD_CMPBVAL_2_bm TCD_CMPBVAL2_bm
  #elif defined(TCD_CMPBVAL_2_bm)
    //deprecated_constant_name TCD_CMPBVAL2_bm = TCD_CMPBVAL_2_bm;
    #define TCD_CMPBVAL2_bm TCD_CMPBVAL_2_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_CMPBVAL_2_bp) && defined(TCD_CMPBVAL2_bp)
    #define TCD_CMPBVAL_2_bp TCD_CMPBVAL2_bp
  #elif defined(TCD_CMPBVAL_2_bp)
    //deprecated_constant_name TCD_CMPBVAL2_bp = TCD_CMPBVAL_2_bp;
    #define TCD_CMPBVAL2_bp TCD_CMPBVAL_2_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_CMPBVAL_3_bm) && defined(TCD_CMPBVAL3_bm)
    #define TCD_CMPBVAL_3_bm TCD_CMPBVAL3_bm
  #elif defined(TCD_CMPBVAL_3_bm)
    //deprecated_constant_name TCD_CMPBVAL3_bm = TCD_CMPBVAL_3_bm;
    #define TCD_CMPBVAL3_bm TCD_CMPBVAL_3_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_CMPBVAL_3_bp) && defined(TCD_CMPBVAL3_bp)
    #define TCD_CMPBVAL_3_bp TCD_CMPBVAL3_bp
  #elif defined(TCD_CMPBVAL_3_bp)
    //deprecated_constant_name TCD_CMPBVAL3_bp = TCD_CMPBVAL_3_bp;
    #define TCD_CMPBVAL3_bp TCD_CMPBVAL_3_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(TCD_CFG_0_bm) && defined(TCD_CFG0_bm)
    #define TCD_CFG_0_bm TCD_CFG0_bm
  #elif defined(TCD_CFG_0_bm)
    //deprecated_constant_name TCD_CFG0_bm = TCD_CFG_0_bm;
    #define TCD_CFG0_bm TCD_CFG_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_CFG_0_bp) && defined(TCD_CFG0_bp)
    #define TCD_CFG_0_bp TCD_CFG0_bp
  #elif defined(TCD_CFG_0_bp)
    //deprecated_constant_name TCD_CFG0_bp = TCD_CFG_0_bp;
    #define TCD_CFG0_bp TCD_CFG_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_CFG_1_bm) && defined(TCD_CFG1_bm)
    #define TCD_CFG_1_bm TCD_CFG1_bm
  #elif defined(TCD_CFG_1_bm)
    //deprecated_constant_name TCD_CFG1_bm = TCD_CFG_1_bm;
    #define TCD_CFG1_bm TCD_CFG_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_CFG_1_bp) && defined(TCD_CFG1_bp)
    #define TCD_CFG_1_bp TCD_CFG1_bp
  #elif defined(TCD_CFG_1_bp)
    //deprecated_constant_name TCD_CFG1_bp = TCD_CFG_1_bp;
    #define TCD_CFG1_bp TCD_CFG_1_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(TCD_INPUTMODE_0_bm) && defined(TCD_INPUTMODE0_bm)
    #define TCD_INPUTMODE_0_bm TCD_INPUTMODE0_bm
  #elif defined(TCD_INPUTMODE_0_bm)
    //deprecated_constant_name TCD_INPUTMODE0_bm = TCD_INPUTMODE_0_bm;
    #define TCD_INPUTMODE0_bm TCD_INPUTMODE_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_INPUTMODE_0_bp) && defined(TCD_INPUTMODE0_bp)
    #define TCD_INPUTMODE_0_bp TCD_INPUTMODE0_bp
  #elif defined(TCD_INPUTMODE_0_bp)
    //deprecated_constant_name TCD_INPUTMODE0_bp = TCD_INPUTMODE_0_bp;
    #define TCD_INPUTMODE0_bp TCD_INPUTMODE_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_INPUTMODE_1_bm) && defined(TCD_INPUTMODE1_bm)
    #define TCD_INPUTMODE_1_bm TCD_INPUTMODE1_bm
  #elif defined(TCD_INPUTMODE_1_bm)
    //deprecated_constant_name TCD_INPUTMODE1_bm = TCD_INPUTMODE_1_bm;
    #define TCD_INPUTMODE1_bm TCD_INPUTMODE_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_INPUTMODE_1_bp) && defined(TCD_INPUTMODE1_bp)
    #define TCD_INPUTMODE_1_bp TCD_INPUTMODE1_bp
  #elif defined(TCD_INPUTMODE_1_bp)
    //deprecated_constant_name TCD_INPUTMODE1_bp = TCD_INPUTMODE_1_bp;
    #define TCD_INPUTMODE1_bp TCD_INPUTMODE_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_INPUTMODE_2_bm) && defined(TCD_INPUTMODE2_bm)
    #define TCD_INPUTMODE_2_bm TCD_INPUTMODE2_bm
  #elif defined(TCD_INPUTMODE_2_bm)
    //deprecated_constant_name TCD_INPUTMODE2_bm = TCD_INPUTMODE_2_bm;
    #define TCD_INPUTMODE2_bm TCD_INPUTMODE_2_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_INPUTMODE_2_bp) && defined(TCD_INPUTMODE2_bp)
    #define TCD_INPUTMODE_2_bp TCD_INPUTMODE2_bp
  #elif defined(TCD_INPUTMODE_2_bp)
    //deprecated_constant_name TCD_INPUTMODE2_bp = TCD_INPUTMODE_2_bp;
    #define TCD_INPUTMODE2_bp TCD_INPUTMODE_2_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_INPUTMODE_3_bm) && defined(TCD_INPUTMODE3_bm)
    #define TCD_INPUTMODE_3_bm TCD_INPUTMODE3_bm
  #elif defined(TCD_INPUTMODE_3_bm)
    //deprecated_constant_name TCD_INPUTMODE3_bm = TCD_INPUTMODE_3_bm;
    #define TCD_INPUTMODE3_bm TCD_INPUTMODE_3_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_INPUTMODE_3_bp) && defined(TCD_INPUTMODE3_bp)
    #define TCD_INPUTMODE_3_bp TCD_INPUTMODE3_bp
  #elif defined(TCD_INPUTMODE_3_bp)
    //deprecated_constant_name TCD_INPUTMODE3_bp = TCD_INPUTMODE_3_bp;
    #define TCD_INPUTMODE3_bp TCD_INPUTMODE_3_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(TCD_DLYSEL_0_bm) && defined(TCD_DLYSEL0_bm)
    #define TCD_DLYSEL_0_bm TCD_DLYSEL0_bm
  #elif defined(TCD_DLYSEL_0_bm)
    //deprecated_constant_name TCD_DLYSEL0_bm = TCD_DLYSEL_0_bm;
    #define TCD_DLYSEL0_bm TCD_DLYSEL_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_DLYSEL_0_bp) && defined(TCD_DLYSEL0_bp)
    #define TCD_DLYSEL_0_bp TCD_DLYSEL0_bp
  #elif defined(TCD_DLYSEL_0_bp)
    //deprecated_constant_name TCD_DLYSEL0_bp = TCD_DLYSEL_0_bp;
    #define TCD_DLYSEL0_bp TCD_DLYSEL_0_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(TCD_DLYSEL_1_bm) && defined(TCD_DLYSEL1_bm)
    #define TCD_DLYSEL_1_bm TCD_DLYSEL1_bm
  #elif defined(TCD_DLYSEL_1_bm)
    //deprecated_constant_name TCD_DLYSEL1_bm = TCD_DLYSEL_1_bm;
    #define TCD_DLYSEL1_bm TCD_DLYSEL_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_DLYSEL_1_bp) && defined(TCD_DLYSEL1_bp)
    #define TCD_DLYSEL_1_bp TCD_DLYSEL1_bp
  #elif defined(TCD_DLYSEL_1_bp)
    //deprecated_constant_name TCD_DLYSEL1_bp = TCD_DLYSEL_1_bp;
    #define TCD_DLYSEL1_bp TCD_DLYSEL_1_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(TCD_DLYTRIG_0_bm) && defined(TCD_DLYTRIG0_bm)
    #define TCD_DLYTRIG_0_bm TCD_DLYTRIG0_bm
  #elif defined(TCD_DLYTRIG_0_bm)
    //deprecated_constant_name TCD_DLYTRIG0_bm = TCD_DLYTRIG_0_bm;
    #define TCD_DLYTRIG0_bm TCD_DLYTRIG_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_DLYTRIG_0_bp) && defined(TCD_DLYTRIG0_bp)
    #define TCD_DLYTRIG_0_bp TCD_DLYTRIG0_bp
  #elif defined(TCD_DLYTRIG_0_bp)
    //deprecated_constant_name TCD_DLYTRIG0_bp = TCD_DLYTRIG_0_bp;
    #define TCD_DLYTRIG0_bp TCD_DLYTRIG_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_DLYTRIG_1_bm) && defined(TCD_DLYTRIG1_bm)
    #define TCD_DLYTRIG_1_bm TCD_DLYTRIG1_bm
  #elif defined(TCD_DLYTRIG_1_bm)
    //deprecated_constant_name TCD_DLYTRIG1_bm = TCD_DLYTRIG_1_bm;
    #define TCD_DLYTRIG1_bm TCD_DLYTRIG_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_DLYTRIG_1_bp) && defined(TCD_DLYTRIG1_bp)
    #define TCD_DLYTRIG_1_bp TCD_DLYTRIG1_bp
  #elif defined(TCD_DLYTRIG_1_bp)
    //deprecated_constant_name TCD_DLYTRIG1_bp = TCD_DLYTRIG_1_bp;
    #define TCD_DLYTRIG1_bp TCD_DLYTRIG_1_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(TCD_DLYPRESC_0_bm) && defined(TCD_DLYPRESC0_bm)
    #define TCD_DLYPRESC_0_bm TCD_DLYPRESC0_bm
  #elif defined(TCD_DLYPRESC_0_bm)
    //deprecated_constant_name TCD_DLYPRESC0_bm = TCD_DLYPRESC_0_bm;
    #define TCD_DLYPRESC0_bm TCD_DLYPRESC_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_DLYPRESC_0_bp) && defined(TCD_DLYPRESC0_bp)
    #define TCD_DLYPRESC_0_bp TCD_DLYPRESC0_bp
  #elif defined(TCD_DLYPRESC_0_bp)
    //deprecated_constant_name TCD_DLYPRESC0_bp = TCD_DLYPRESC_0_bp;
    #define TCD_DLYPRESC0_bp TCD_DLYPRESC_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_DLYPRESC_1_bm) && defined(TCD_DLYPRESC1_bm)
    #define TCD_DLYPRESC_1_bm TCD_DLYPRESC1_bm
  #elif defined(TCD_DLYPRESC_1_bm)
    //deprecated_constant_name TCD_DLYPRESC1_bm = TCD_DLYPRESC_1_bm;
    #define TCD_DLYPRESC1_bm TCD_DLYPRESC_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_DLYPRESC_1_bp) && defined(TCD_DLYPRESC1_bp)
    #define TCD_DLYPRESC_1_bp TCD_DLYPRESC1_bp
  #elif defined(TCD_DLYPRESC_1_bp)
    //deprecated_constant_name TCD_DLYPRESC1_bp = TCD_DLYPRESC_1_bp;
    #define TCD_DLYPRESC1_bp TCD_DLYPRESC_1_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(TCD_DLYVAL_0_bm) && defined(TCD_DLYVAL0_bm)
    #define TCD_DLYVAL_0_bm TCD_DLYVAL0_bm
  #elif defined(TCD_DLYVAL_0_bm)
    //deprecated_constant_name TCD_DLYVAL0_bm = TCD_DLYVAL_0_bm;
    #define TCD_DLYVAL0_bm TCD_DLYVAL_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_DLYVAL_0_bp) && defined(TCD_DLYVAL0_bp)
    #define TCD_DLYVAL_0_bp TCD_DLYVAL0_bp
  #elif defined(TCD_DLYVAL_0_bp)
    //deprecated_constant_name TCD_DLYVAL0_bp = TCD_DLYVAL_0_bp;
    #define TCD_DLYVAL0_bp TCD_DLYVAL_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_DLYVAL_1_bm) && defined(TCD_DLYVAL1_bm)
    #define TCD_DLYVAL_1_bm TCD_DLYVAL1_bm
  #elif defined(TCD_DLYVAL_1_bm)
    //deprecated_constant_name TCD_DLYVAL1_bm = TCD_DLYVAL_1_bm;
    #define TCD_DLYVAL1_bm TCD_DLYVAL_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_DLYVAL_1_bp) && defined(TCD_DLYVAL1_bp)
    #define TCD_DLYVAL_1_bp TCD_DLYVAL1_bp
  #elif defined(TCD_DLYVAL_1_bp)
    //deprecated_constant_name TCD_DLYVAL1_bp = TCD_DLYVAL_1_bp;
    #define TCD_DLYVAL1_bp TCD_DLYVAL_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_DLYVAL_2_bm) && defined(TCD_DLYVAL2_bm)
    #define TCD_DLYVAL_2_bm TCD_DLYVAL2_bm
  #elif defined(TCD_DLYVAL_2_bm)
    //deprecated_constant_name TCD_DLYVAL2_bm = TCD_DLYVAL_2_bm;
    #define TCD_DLYVAL2_bm TCD_DLYVAL_2_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_DLYVAL_2_bp) && defined(TCD_DLYVAL2_bp)
    #define TCD_DLYVAL_2_bp TCD_DLYVAL2_bp
  #elif defined(TCD_DLYVAL_2_bp)
    //deprecated_constant_name TCD_DLYVAL2_bp = TCD_DLYVAL_2_bp;
    #define TCD_DLYVAL2_bp TCD_DLYVAL_2_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_DLYVAL_3_bm) && defined(TCD_DLYVAL3_bm)
    #define TCD_DLYVAL_3_bm TCD_DLYVAL3_bm
  #elif defined(TCD_DLYVAL_3_bm)
    //deprecated_constant_name TCD_DLYVAL3_bm = TCD_DLYVAL_3_bm;
    #define TCD_DLYVAL3_bm TCD_DLYVAL_3_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_DLYVAL_3_bp) && defined(TCD_DLYVAL3_bp)
    #define TCD_DLYVAL_3_bp TCD_DLYVAL3_bp
  #elif defined(TCD_DLYVAL_3_bp)
    //deprecated_constant_name TCD_DLYVAL3_bp = TCD_DLYVAL_3_bp;
    #define TCD_DLYVAL3_bp TCD_DLYVAL_3_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_DLYVAL_4_bm) && defined(TCD_DLYVAL4_bm)
    #define TCD_DLYVAL_4_bm TCD_DLYVAL4_bm
  #elif defined(TCD_DLYVAL_4_bm)
    //deprecated_constant_name TCD_DLYVAL4_bm = TCD_DLYVAL_4_bm;
    #define TCD_DLYVAL4_bm TCD_DLYVAL_4_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_DLYVAL_4_bp) && defined(TCD_DLYVAL4_bp)
    #define TCD_DLYVAL_4_bp TCD_DLYVAL4_bp
  #elif defined(TCD_DLYVAL_4_bp)
    //deprecated_constant_name TCD_DLYVAL4_bp = TCD_DLYVAL_4_bp;
    #define TCD_DLYVAL4_bp TCD_DLYVAL_4_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_DLYVAL_5_bm) && defined(TCD_DLYVAL5_bm)
    #define TCD_DLYVAL_5_bm TCD_DLYVAL5_bm
  #elif defined(TCD_DLYVAL_5_bm)
    //deprecated_constant_name TCD_DLYVAL5_bm = TCD_DLYVAL_5_bm;
    #define TCD_DLYVAL5_bm TCD_DLYVAL_5_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_DLYVAL_5_bp) && defined(TCD_DLYVAL5_bp)
    #define TCD_DLYVAL_5_bp TCD_DLYVAL5_bp
  #elif defined(TCD_DLYVAL_5_bp)
    //deprecated_constant_name TCD_DLYVAL5_bp = TCD_DLYVAL_5_bp;
    #define TCD_DLYVAL5_bp TCD_DLYVAL_5_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_DLYVAL_6_bm) && defined(TCD_DLYVAL6_bm)
    #define TCD_DLYVAL_6_bm TCD_DLYVAL6_bm
  #elif defined(TCD_DLYVAL_6_bm)
    //deprecated_constant_name TCD_DLYVAL6_bm = TCD_DLYVAL_6_bm;
    #define TCD_DLYVAL6_bm TCD_DLYVAL_6_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_DLYVAL_6_bp) && defined(TCD_DLYVAL6_bp)
    #define TCD_DLYVAL_6_bp TCD_DLYVAL6_bp
  #elif defined(TCD_DLYVAL_6_bp)
    //deprecated_constant_name TCD_DLYVAL6_bp = TCD_DLYVAL_6_bp;
    #define TCD_DLYVAL6_bp TCD_DLYVAL_6_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_DLYVAL_7_bm) && defined(TCD_DLYVAL7_bm)
    #define TCD_DLYVAL_7_bm TCD_DLYVAL7_bm
  #elif defined(TCD_DLYVAL_7_bm)
    //deprecated_constant_name TCD_DLYVAL7_bm = TCD_DLYVAL_7_bm;
    #define TCD_DLYVAL7_bm TCD_DLYVAL_7_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_DLYVAL_7_bp) && defined(TCD_DLYVAL7_bp)
    #define TCD_DLYVAL_7_bp TCD_DLYVAL7_bp
  #elif defined(TCD_DLYVAL_7_bp)
    //deprecated_constant_name TCD_DLYVAL7_bp = TCD_DLYVAL_7_bp;
    #define TCD_DLYVAL7_bp TCD_DLYVAL_7_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(TCD_DITHERSEL_0_bm) && defined(TCD_DITHERSEL0_bm)
    #define TCD_DITHERSEL_0_bm TCD_DITHERSEL0_bm
  #elif defined(TCD_DITHERSEL_0_bm)
    //deprecated_constant_name TCD_DITHERSEL0_bm = TCD_DITHERSEL_0_bm;
    #define TCD_DITHERSEL0_bm TCD_DITHERSEL_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_DITHERSEL_0_bp) && defined(TCD_DITHERSEL0_bp)
    #define TCD_DITHERSEL_0_bp TCD_DITHERSEL0_bp
  #elif defined(TCD_DITHERSEL_0_bp)
    //deprecated_constant_name TCD_DITHERSEL0_bp = TCD_DITHERSEL_0_bp;
    #define TCD_DITHERSEL0_bp TCD_DITHERSEL_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_DITHERSEL_1_bm) && defined(TCD_DITHERSEL1_bm)
    #define TCD_DITHERSEL_1_bm TCD_DITHERSEL1_bm
  #elif defined(TCD_DITHERSEL_1_bm)
    //deprecated_constant_name TCD_DITHERSEL1_bm = TCD_DITHERSEL_1_bm;
    #define TCD_DITHERSEL1_bm TCD_DITHERSEL_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_DITHERSEL_1_bp) && defined(TCD_DITHERSEL1_bp)
    #define TCD_DITHERSEL_1_bp TCD_DITHERSEL1_bp
  #elif defined(TCD_DITHERSEL_1_bp)
    //deprecated_constant_name TCD_DITHERSEL1_bp = TCD_DITHERSEL_1_bp;
    #define TCD_DITHERSEL1_bp TCD_DITHERSEL_1_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(TCD_DITHER_0_bm) && defined(TCD_DITHER0_bm)
    #define TCD_DITHER_0_bm TCD_DITHER0_bm
  #elif defined(TCD_DITHER_0_bm)
    //deprecated_constant_name TCD_DITHER0_bm = TCD_DITHER_0_bm;
    #define TCD_DITHER0_bm TCD_DITHER_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_DITHER_0_bp) && defined(TCD_DITHER0_bp)
    #define TCD_DITHER_0_bp TCD_DITHER0_bp
  #elif defined(TCD_DITHER_0_bp)
    //deprecated_constant_name TCD_DITHER0_bp = TCD_DITHER_0_bp;
    #define TCD_DITHER0_bp TCD_DITHER_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_DITHER_1_bm) && defined(TCD_DITHER1_bm)
    #define TCD_DITHER_1_bm TCD_DITHER1_bm
  #elif defined(TCD_DITHER_1_bm)
    //deprecated_constant_name TCD_DITHER1_bm = TCD_DITHER_1_bm;
    #define TCD_DITHER1_bm TCD_DITHER_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_DITHER_1_bp) && defined(TCD_DITHER1_bp)
    #define TCD_DITHER_1_bp TCD_DITHER1_bp
  #elif defined(TCD_DITHER_1_bp)
    //deprecated_constant_name TCD_DITHER1_bp = TCD_DITHER_1_bp;
    #define TCD_DITHER1_bp TCD_DITHER_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_DITHER_2_bm) && defined(TCD_DITHER2_bm)
    #define TCD_DITHER_2_bm TCD_DITHER2_bm
  #elif defined(TCD_DITHER_2_bm)
    //deprecated_constant_name TCD_DITHER2_bm = TCD_DITHER_2_bm;
    #define TCD_DITHER2_bm TCD_DITHER_2_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_DITHER_2_bp) && defined(TCD_DITHER2_bp)
    #define TCD_DITHER_2_bp TCD_DITHER2_bp
  #elif defined(TCD_DITHER_2_bp)
    //deprecated_constant_name TCD_DITHER2_bp = TCD_DITHER_2_bp;
    #define TCD_DITHER2_bp TCD_DITHER_2_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_DITHER_3_bm) && defined(TCD_DITHER3_bm)
    #define TCD_DITHER_3_bm TCD_DITHER3_bm
  #elif defined(TCD_DITHER_3_bm)
    //deprecated_constant_name TCD_DITHER3_bm = TCD_DITHER_3_bm;
    #define TCD_DITHER3_bm TCD_DITHER_3_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_DITHER_3_bp) && defined(TCD_DITHER3_bp)
    #define TCD_DITHER_3_bp TCD_DITHER3_bp
  #elif defined(TCD_DITHER_3_bp)
    //deprecated_constant_name TCD_DITHER3_bp = TCD_DITHER_3_bp;
    #define TCD_DITHER3_bp TCD_DITHER_3_bp //Deprecated as of Q2 2022 header change.
  #endif

  /* ======= TWI ======= */
  #if !defined(TWI_SDAHOLD_0_bm) && defined(TWI_SDAHOLD0_bm)
    #define TWI_SDAHOLD_0_bm TWI_SDAHOLD0_bm
  #elif defined(TWI_SDAHOLD_0_bm)
    //deprecated_constant_name TWI_SDAHOLD0_bm = TWI_SDAHOLD_0_bm;
    #define TWI_SDAHOLD0_bm TWI_SDAHOLD_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TWI_SDAHOLD_0_bp) && defined(TWI_SDAHOLD0_bp)
    #define TWI_SDAHOLD_0_bp TWI_SDAHOLD0_bp
  #elif defined(TWI_SDAHOLD_0_bp)
    //deprecated_constant_name TWI_SDAHOLD0_bp = TWI_SDAHOLD_0_bp;
    #define TWI_SDAHOLD0_bp TWI_SDAHOLD_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TWI_SDAHOLD_1_bm) && defined(TWI_SDAHOLD1_bm)
    #define TWI_SDAHOLD_1_bm TWI_SDAHOLD1_bm
  #elif defined(TWI_SDAHOLD_1_bm)
    //deprecated_constant_name TWI_SDAHOLD1_bm = TWI_SDAHOLD_1_bm;
    #define TWI_SDAHOLD1_bm TWI_SDAHOLD_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TWI_SDAHOLD_1_bp) && defined(TWI_SDAHOLD1_bp)
    #define TWI_SDAHOLD_1_bp TWI_SDAHOLD1_bp
  #elif defined(TWI_SDAHOLD_1_bp)
    //deprecated_constant_name TWI_SDAHOLD1_bp = TWI_SDAHOLD_1_bp;
    #define TWI_SDAHOLD1_bp TWI_SDAHOLD_1_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(TWI_TIMEOUT_0_bm) && defined(TWI_TIMEOUT0_bm)
    #define TWI_TIMEOUT_0_bm TWI_TIMEOUT0_bm
  #elif defined(TWI_TIMEOUT_0_bm)
    //deprecated_constant_name TWI_TIMEOUT0_bm = TWI_TIMEOUT_0_bm;
    #define TWI_TIMEOUT0_bm TWI_TIMEOUT_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TWI_TIMEOUT_0_bp) && defined(TWI_TIMEOUT0_bp)
    #define TWI_TIMEOUT_0_bp TWI_TIMEOUT0_bp
  #elif defined(TWI_TIMEOUT_0_bp)
    //deprecated_constant_name TWI_TIMEOUT0_bp = TWI_TIMEOUT_0_bp;
    #define TWI_TIMEOUT0_bp TWI_TIMEOUT_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TWI_TIMEOUT_1_bm) && defined(TWI_TIMEOUT1_bm)
    #define TWI_TIMEOUT_1_bm TWI_TIMEOUT1_bm
  #elif defined(TWI_TIMEOUT_1_bm)
    //deprecated_constant_name TWI_TIMEOUT1_bm = TWI_TIMEOUT_1_bm;
    #define TWI_TIMEOUT1_bm TWI_TIMEOUT_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TWI_TIMEOUT_1_bp) && defined(TWI_TIMEOUT1_bp)
    #define TWI_TIMEOUT_1_bp TWI_TIMEOUT1_bp
  #elif defined(TWI_TIMEOUT_1_bp)
    //deprecated_constant_name TWI_TIMEOUT1_bp = TWI_TIMEOUT_1_bp;
    #define TWI_TIMEOUT1_bp TWI_TIMEOUT_1_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(TWI_MCMD_0_bm) && defined(TWI_MCMD0_bm)
    #define TWI_MCMD_0_bm TWI_MCMD0_bm
  #elif defined(TWI_MCMD_0_bm)
    //deprecated_constant_name TWI_MCMD0_bm = TWI_MCMD_0_bm;
    #define TWI_MCMD0_bm TWI_MCMD_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TWI_MCMD_0_bp) && defined(TWI_MCMD0_bp)
    #define TWI_MCMD_0_bp TWI_MCMD0_bp
  #elif defined(TWI_MCMD_0_bp)
    //deprecated_constant_name TWI_MCMD0_bp = TWI_MCMD_0_bp;
    #define TWI_MCMD0_bp TWI_MCMD_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TWI_MCMD_1_bm) && defined(TWI_MCMD1_bm)
    #define TWI_MCMD_1_bm TWI_MCMD1_bm
  #elif defined(TWI_MCMD_1_bm)
    //deprecated_constant_name TWI_MCMD1_bm = TWI_MCMD_1_bm;
    #define TWI_MCMD1_bm TWI_MCMD_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TWI_MCMD_1_bp) && defined(TWI_MCMD1_bp)
    #define TWI_MCMD_1_bp TWI_MCMD1_bp
  #elif defined(TWI_MCMD_1_bp)
    //deprecated_constant_name TWI_MCMD1_bp = TWI_MCMD_1_bp;
    #define TWI_MCMD1_bp TWI_MCMD_1_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(TWI_BUSSTATE_0_bm) && defined(TWI_BUSSTATE0_bm)
    #define TWI_BUSSTATE_0_bm TWI_BUSSTATE0_bm
  #elif defined(TWI_BUSSTATE_0_bm)
    //deprecated_constant_name TWI_BUSSTATE0_bm = TWI_BUSSTATE_0_bm;
    #define TWI_BUSSTATE0_bm TWI_BUSSTATE_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TWI_BUSSTATE_0_bp) && defined(TWI_BUSSTATE0_bp)
    #define TWI_BUSSTATE_0_bp TWI_BUSSTATE0_bp
  #elif defined(TWI_BUSSTATE_0_bp)
    //deprecated_constant_name TWI_BUSSTATE0_bp = TWI_BUSSTATE_0_bp;
    #define TWI_BUSSTATE0_bp TWI_BUSSTATE_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TWI_BUSSTATE_1_bm) && defined(TWI_BUSSTATE1_bm)
    #define TWI_BUSSTATE_1_bm TWI_BUSSTATE1_bm
  #elif defined(TWI_BUSSTATE_1_bm)
    //deprecated_constant_name TWI_BUSSTATE1_bm = TWI_BUSSTATE_1_bm;
    #define TWI_BUSSTATE1_bm TWI_BUSSTATE_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TWI_BUSSTATE_1_bp) && defined(TWI_BUSSTATE1_bp)
    #define TWI_BUSSTATE_1_bp TWI_BUSSTATE1_bp
  #elif defined(TWI_BUSSTATE_1_bp)
    //deprecated_constant_name TWI_BUSSTATE1_bp = TWI_BUSSTATE_1_bp;
    #define TWI_BUSSTATE1_bp TWI_BUSSTATE_1_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(TWI_SCMD_0_bm) && defined(TWI_SCMD0_bm)
    #define TWI_SCMD_0_bm TWI_SCMD0_bm
  #elif defined(TWI_SCMD_0_bm)
    //deprecated_constant_name TWI_SCMD0_bm = TWI_SCMD_0_bm;
    #define TWI_SCMD0_bm TWI_SCMD_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TWI_SCMD_0_bp) && defined(TWI_SCMD0_bp)
    #define TWI_SCMD_0_bp TWI_SCMD0_bp
  #elif defined(TWI_SCMD_0_bp)
    //deprecated_constant_name TWI_SCMD0_bp = TWI_SCMD_0_bp;
    #define TWI_SCMD0_bp TWI_SCMD_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TWI_SCMD_1_bm) && defined(TWI_SCMD1_bm)
    #define TWI_SCMD_1_bm TWI_SCMD1_bm
  #elif defined(TWI_SCMD_1_bm)
    //deprecated_constant_name TWI_SCMD1_bm = TWI_SCMD_1_bm;
    #define TWI_SCMD1_bm TWI_SCMD_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TWI_SCMD_1_bp) && defined(TWI_SCMD1_bp)
    #define TWI_SCMD_1_bp TWI_SCMD1_bp
  #elif defined(TWI_SCMD_1_bp)
    //deprecated_constant_name TWI_SCMD1_bp = TWI_SCMD_1_bp;
    #define TWI_SCMD1_bp TWI_SCMD_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TWI_ADDRMASK_0_bm) && defined(TWI_ADDRMASK0_bm)
    #define TWI_ADDRMASK_0_bm TWI_ADDRMASK0_bm
  #elif defined(TWI_ADDRMASK_0_bm)
    //deprecated_constant_name TWI_ADDRMASK0_bm = TWI_ADDRMASK_0_bm;
    #define TWI_ADDRMASK0_bm TWI_ADDRMASK_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TWI_ADDRMASK_0_bp) && defined(TWI_ADDRMASK0_bp)
    #define TWI_ADDRMASK_0_bp TWI_ADDRMASK0_bp
  #elif defined(TWI_ADDRMASK_0_bp)
    //deprecated_constant_name TWI_ADDRMASK0_bp = TWI_ADDRMASK_0_bp;
    #define TWI_ADDRMASK0_bp TWI_ADDRMASK_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TWI_ADDRMASK_1_bm) && defined(TWI_ADDRMASK1_bm)
    #define TWI_ADDRMASK_1_bm TWI_ADDRMASK1_bm
  #elif defined(TWI_ADDRMASK_1_bm)
    //deprecated_constant_name TWI_ADDRMASK1_bm = TWI_ADDRMASK_1_bm;
    #define TWI_ADDRMASK1_bm TWI_ADDRMASK_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TWI_ADDRMASK_1_bp) && defined(TWI_ADDRMASK1_bp)
    #define TWI_ADDRMASK_1_bp TWI_ADDRMASK1_bp
  #elif defined(TWI_ADDRMASK_1_bp)
    //deprecated_constant_name TWI_ADDRMASK1_bp = TWI_ADDRMASK_1_bp;
    #define TWI_ADDRMASK1_bp TWI_ADDRMASK_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TWI_ADDRMASK_2_bm) && defined(TWI_ADDRMASK2_bm)
    #define TWI_ADDRMASK_2_bm TWI_ADDRMASK2_bm
  #elif defined(TWI_ADDRMASK_2_bm)
    //deprecated_constant_name TWI_ADDRMASK2_bm = TWI_ADDRMASK_2_bm;
    #define TWI_ADDRMASK2_bm TWI_ADDRMASK_2_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TWI_ADDRMASK_2_bp) && defined(TWI_ADDRMASK2_bp)
    #define TWI_ADDRMASK_2_bp TWI_ADDRMASK2_bp
  #elif defined(TWI_ADDRMASK_2_bp)
    //deprecated_constant_name TWI_ADDRMASK2_bp = TWI_ADDRMASK_2_bp;
    #define TWI_ADDRMASK2_bp TWI_ADDRMASK_2_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TWI_ADDRMASK_3_bm) && defined(TWI_ADDRMASK3_bm)
    #define TWI_ADDRMASK_3_bm TWI_ADDRMASK3_bm
  #elif defined(TWI_ADDRMASK_3_bm)
    //deprecated_constant_name TWI_ADDRMASK3_bm = TWI_ADDRMASK_3_bm;
    #define TWI_ADDRMASK3_bm TWI_ADDRMASK_3_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TWI_ADDRMASK_3_bp) && defined(TWI_ADDRMASK3_bp)
    #define TWI_ADDRMASK_3_bp TWI_ADDRMASK3_bp
  #elif defined(TWI_ADDRMASK_3_bp)
    //deprecated_constant_name TWI_ADDRMASK3_bp = TWI_ADDRMASK_3_bp;
    #define TWI_ADDRMASK3_bp TWI_ADDRMASK_3_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TWI_ADDRMASK_4_bm) && defined(TWI_ADDRMASK4_bm)
    #define TWI_ADDRMASK_4_bm TWI_ADDRMASK4_bm
  #elif defined(TWI_ADDRMASK_4_bm)
    //deprecated_constant_name TWI_ADDRMASK4_bm = TWI_ADDRMASK_4_bm;
    #define TWI_ADDRMASK4_bm TWI_ADDRMASK_4_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TWI_ADDRMASK_4_bp) && defined(TWI_ADDRMASK4_bp)
    #define TWI_ADDRMASK_4_bp TWI_ADDRMASK4_bp
  #elif defined(TWI_ADDRMASK_4_bp)
    //deprecated_constant_name TWI_ADDRMASK4_bp = TWI_ADDRMASK_4_bp;
    #define TWI_ADDRMASK4_bp TWI_ADDRMASK_4_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TWI_ADDRMASK_5_bm) && defined(TWI_ADDRMASK5_bm)
    #define TWI_ADDRMASK_5_bm TWI_ADDRMASK5_bm
  #elif defined(TWI_ADDRMASK_5_bm)
    //deprecated_constant_name TWI_ADDRMASK5_bm = TWI_ADDRMASK_5_bm;
    #define TWI_ADDRMASK5_bm TWI_ADDRMASK_5_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TWI_ADDRMASK_5_bp) && defined(TWI_ADDRMASK5_bp)
    #define TWI_ADDRMASK_5_bp TWI_ADDRMASK5_bp
  #elif defined(TWI_ADDRMASK_5_bp)
    //deprecated_constant_name TWI_ADDRMASK5_bp = TWI_ADDRMASK_5_bp;
    #define TWI_ADDRMASK5_bp TWI_ADDRMASK_5_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TWI_ADDRMASK_6_bm) && defined(TWI_ADDRMASK6_bm)
    #define TWI_ADDRMASK_6_bm TWI_ADDRMASK6_bm
  #elif defined(TWI_ADDRMASK_6_bm)
    //deprecated_constant_name TWI_ADDRMASK6_bm = TWI_ADDRMASK_6_bm;
    #define TWI_ADDRMASK6_bm TWI_ADDRMASK_6_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TWI_ADDRMASK_6_bp) && defined(TWI_ADDRMASK6_bp)
    #define TWI_ADDRMASK_6_bp TWI_ADDRMASK6_bp
  #elif defined(TWI_ADDRMASK_6_bp)
    //deprecated_constant_name TWI_ADDRMASK6_bp = TWI_ADDRMASK_6_bp;
    #define TWI_ADDRMASK6_bp TWI_ADDRMASK_6_bp //Deprecated as of Q2 2022 header change.
  #endif

  /* ======= USART ======= */
  #if !defined(USART_RXMODE_0_bm) && defined(USART_RXMODE0_bm)
    #define USART_RXMODE_0_bm USART_RXMODE0_bm
  #elif defined(USART_RXMODE_0_bm)
    //deprecated_constant_name USART_RXMODE0_bm = USART_RXMODE_0_bm;
    #define USART_RXMODE0_bm USART_RXMODE_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(USART_RXMODE_0_bp) && defined(USART_RXMODE0_bp)
    #define USART_RXMODE_0_bp USART_RXMODE0_bp
  #elif defined(USART_RXMODE_0_bp)
    //deprecated_constant_name USART_RXMODE0_bp = USART_RXMODE_0_bp;
    #define USART_RXMODE0_bp USART_RXMODE_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(USART_RXMODE_1_bm) && defined(USART_RXMODE1_bm)
    #define USART_RXMODE_1_bm USART_RXMODE1_bm
  #elif defined(USART_RXMODE_1_bm)
    //deprecated_constant_name USART_RXMODE1_bm = USART_RXMODE_1_bm;
    #define USART_RXMODE1_bm USART_RXMODE_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(USART_RXMODE_1_bp) && defined(USART_RXMODE1_bp)
    #define USART_RXMODE_1_bp USART_RXMODE1_bp
  #elif defined(USART_RXMODE_1_bp)
    //deprecated_constant_name USART_RXMODE1_bp = USART_RXMODE_1_bp;
    #define USART_RXMODE1_bp USART_RXMODE_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(USART_CHSIZE_0_bm) && defined(USART_CHSIZE0_bm)
    #define USART_CHSIZE_0_bm USART_CHSIZE0_bm
  #elif defined(USART_CHSIZE_0_bm)
    //deprecated_constant_name USART_CHSIZE0_bm = USART_CHSIZE_0_bm;
    #define USART_CHSIZE0_bm USART_CHSIZE_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(USART_CHSIZE_0_bp) && defined(USART_CHSIZE0_bp)
    #define USART_CHSIZE_0_bp USART_CHSIZE0_bp
  #elif defined(USART_CHSIZE_0_bp)
    //deprecated_constant_name USART_CHSIZE0_bp = USART_CHSIZE_0_bp;
    #define USART_CHSIZE0_bp USART_CHSIZE_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(USART_CHSIZE_1_bm) && defined(USART_CHSIZE1_bm)
    #define USART_CHSIZE_1_bm USART_CHSIZE1_bm
  #elif defined(USART_CHSIZE_1_bm)
    //deprecated_constant_name USART_CHSIZE1_bm = USART_CHSIZE_1_bm;
    #define USART_CHSIZE1_bm USART_CHSIZE_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(USART_CHSIZE_1_bp) && defined(USART_CHSIZE1_bp)
    #define USART_CHSIZE_1_bp USART_CHSIZE1_bp
  #elif defined(USART_CHSIZE_1_bp)
    //deprecated_constant_name USART_CHSIZE1_bp = USART_CHSIZE_1_bp;
    #define USART_CHSIZE1_bp USART_CHSIZE_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(USART_CHSIZE_2_bm) && defined(USART_CHSIZE2_bm)
    #define USART_CHSIZE_2_bm USART_CHSIZE2_bm
  #elif defined(USART_CHSIZE_2_bm)
    //deprecated_constant_name USART_CHSIZE2_bm = USART_CHSIZE_2_bm;
    #define USART_CHSIZE2_bm USART_CHSIZE_2_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(USART_CHSIZE_2_bp) && defined(USART_CHSIZE2_bp)
    #define USART_CHSIZE_2_bp USART_CHSIZE2_bp
  #elif defined(USART_CHSIZE_2_bp)
    //deprecated_constant_name USART_CHSIZE2_bp = USART_CHSIZE_2_bp;
    #define USART_CHSIZE2_bp USART_CHSIZE_2_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(USART_PMODE_0_bm) && defined(USART_PMODE0_bm)
    #define USART_PMODE_0_bm USART_PMODE0_bm
  #elif defined(USART_PMODE_0_bm)
    //deprecated_constant_name USART_PMODE0_bm = USART_PMODE_0_bm;
    #define USART_PMODE0_bm USART_PMODE_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(USART_PMODE_0_bp) && defined(USART_PMODE0_bp)
    #define USART_PMODE_0_bp USART_PMODE0_bp
  #elif defined(USART_PMODE_0_bp)
    //deprecated_constant_name USART_PMODE0_bp = USART_PMODE_0_bp;
    #define USART_PMODE0_bp USART_PMODE_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(USART_PMODE_1_bm) && defined(USART_PMODE1_bm)
    #define USART_PMODE_1_bm USART_PMODE1_bm
  #elif defined(USART_PMODE_1_bm)
    //deprecated_constant_name USART_PMODE1_bm = USART_PMODE_1_bm;
    #define USART_PMODE1_bm USART_PMODE_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(USART_PMODE_1_bp) && defined(USART_PMODE1_bp)
    #define USART_PMODE_1_bp USART_PMODE1_bp
  #elif defined(USART_PMODE_1_bp)
    //deprecated_constant_name USART_PMODE1_bp = USART_PMODE_1_bp;
    #define USART_PMODE1_bp USART_PMODE_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(USART_CMODE_0_bm) && defined(USART_CMODE0_bm)
    #define USART_CMODE_0_bm USART_CMODE0_bm
  #elif defined(USART_CMODE_0_bm)
    //deprecated_constant_name USART_CMODE0_bm = USART_CMODE_0_bm;
    #define USART_CMODE0_bm USART_CMODE_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(USART_CMODE_0_bp) && defined(USART_CMODE0_bp)
    #define USART_CMODE_0_bp USART_CMODE0_bp
  #elif defined(USART_CMODE_0_bp)
    //deprecated_constant_name USART_CMODE0_bp = USART_CMODE_0_bp;
    #define USART_CMODE0_bp USART_CMODE_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(USART_CMODE_1_bm) && defined(USART_CMODE1_bm)
    #define USART_CMODE_1_bm USART_CMODE1_bm
  #elif defined(USART_CMODE_1_bm)
    //deprecated_constant_name USART_CMODE1_bm = USART_CMODE_1_bm;
    #define USART_CMODE1_bm USART_CMODE_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(USART_CMODE_1_bp) && defined(USART_CMODE1_bp)
    #define USART_CMODE_1_bp USART_CMODE1_bp
  #elif defined(USART_CMODE_1_bp)
    //deprecated_constant_name USART_CMODE1_bp = USART_CMODE_1_bp;
    #define USART_CMODE1_bp USART_CMODE_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(USART_ABW_0_bm) && defined(USART_ABW0_bm)
    #define USART_ABW_0_bm USART_ABW0_bm
  #elif defined(USART_ABW_0_bm)
    //deprecated_constant_name USART_ABW0_bm = USART_ABW_0_bm;
    #define USART_ABW0_bm USART_ABW_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(USART_ABW_0_bp) && defined(USART_ABW0_bp)
    #define USART_ABW_0_bp USART_ABW0_bp
  #elif defined(USART_ABW_0_bp)
    //deprecated_constant_name USART_ABW0_bp = USART_ABW_0_bp;
    #define USART_ABW0_bp USART_ABW_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(USART_ABW_1_bm) && defined(USART_ABW1_bm)
    #define USART_ABW_1_bm USART_ABW1_bm
  #elif defined(USART_ABW_1_bm)
    //deprecated_constant_name USART_ABW1_bm = USART_ABW_1_bm;
    #define USART_ABW1_bm USART_ABW_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(USART_ABW_1_bp) && defined(USART_ABW1_bp)
    #define USART_ABW_1_bp USART_ABW1_bp
  #elif defined(USART_ABW_1_bp)
    //deprecated_constant_name USART_ABW1_bp = USART_ABW_1_bp;
    #define USART_ABW1_bp USART_ABW_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(USART_TXPL_0_bm) && defined(USART_TXPL0_bm)
    #define USART_TXPL_0_bm USART_TXPL0_bm
  #elif defined(USART_TXPL_0_bm)
    //deprecated_constant_name USART_TXPL0_bm = USART_TXPL_0_bm;
    #define USART_TXPL0_bm USART_TXPL_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(USART_TXPL_0_bp) && defined(USART_TXPL0_bp)
    #define USART_TXPL_0_bp USART_TXPL0_bp
  #elif defined(USART_TXPL_0_bp)
    //deprecated_constant_name USART_TXPL0_bp = USART_TXPL_0_bp;
    #define USART_TXPL0_bp USART_TXPL_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(USART_TXPL_1_bm) && defined(USART_TXPL1_bm)
    #define USART_TXPL_1_bm USART_TXPL1_bm
  #elif defined(USART_TXPL_1_bm)
    //deprecated_constant_name USART_TXPL1_bm = USART_TXPL_1_bm;
    #define USART_TXPL1_bm USART_TXPL_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(USART_TXPL_1_bp) && defined(USART_TXPL1_bp)
    #define USART_TXPL_1_bp USART_TXPL1_bp
  #elif defined(USART_TXPL_1_bp)
    //deprecated_constant_name USART_TXPL1_bp = USART_TXPL_1_bp;
    #define USART_TXPL1_bp USART_TXPL_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(USART_TXPL_2_bm) && defined(USART_TXPL2_bm)
    #define USART_TXPL_2_bm USART_TXPL2_bm
  #elif defined(USART_TXPL_2_bm)
    //deprecated_constant_name USART_TXPL2_bm = USART_TXPL_2_bm;
    #define USART_TXPL2_bm USART_TXPL_2_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(USART_TXPL_2_bp) && defined(USART_TXPL2_bp)
    #define USART_TXPL_2_bp USART_TXPL2_bp
  #elif defined(USART_TXPL_2_bp)
    //deprecated_constant_name USART_TXPL2_bp = USART_TXPL_2_bp;
    #define USART_TXPL2_bp USART_TXPL_2_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(USART_TXPL_3_bm) && defined(USART_TXPL3_bm)
    #define USART_TXPL_3_bm USART_TXPL3_bm
  #elif defined(USART_TXPL_3_bm)
    //deprecated_constant_name USART_TXPL3_bm = USART_TXPL_3_bm;
    #define USART_TXPL3_bm USART_TXPL_3_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(USART_TXPL_3_bp) && defined(USART_TXPL3_bp)
    #define USART_TXPL_3_bp USART_TXPL3_bp
  #elif defined(USART_TXPL_3_bp)
    //deprecated_constant_name USART_TXPL3_bp = USART_TXPL_3_bp;
    #define USART_TXPL3_bp USART_TXPL_3_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(USART_TXPL_4_bm) && defined(USART_TXPL4_bm)
    #define USART_TXPL_4_bm USART_TXPL4_bm
  #elif defined(USART_TXPL_4_bm)
    //deprecated_constant_name USART_TXPL4_bm = USART_TXPL_4_bm;
    #define USART_TXPL4_bm USART_TXPL_4_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(USART_TXPL_4_bp) && defined(USART_TXPL4_bp)
    #define USART_TXPL_4_bp USART_TXPL4_bp
  #elif defined(USART_TXPL_4_bp)
    //deprecated_constant_name USART_TXPL4_bp = USART_TXPL_4_bp;
    #define USART_TXPL4_bp USART_TXPL_4_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(USART_TXPL_5_bm) && defined(USART_TXPL5_bm)
    #define USART_TXPL_5_bm USART_TXPL5_bm
  #elif defined(USART_TXPL_5_bm)
    //deprecated_constant_name USART_TXPL5_bm = USART_TXPL_5_bm;
    #define USART_TXPL5_bm USART_TXPL_5_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(USART_TXPL_5_bp) && defined(USART_TXPL5_bp)
    #define USART_TXPL_5_bp USART_TXPL5_bp
  #elif defined(USART_TXPL_5_bp)
    //deprecated_constant_name USART_TXPL5_bp = USART_TXPL_5_bp;
    #define USART_TXPL5_bp USART_TXPL_5_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(USART_TXPL_6_bm) && defined(USART_TXPL6_bm)
    #define USART_TXPL_6_bm USART_TXPL6_bm
  #elif defined(USART_TXPL_6_bm)
    //deprecated_constant_name USART_TXPL6_bm = USART_TXPL_6_bm;
    #define USART_TXPL6_bm USART_TXPL_6_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(USART_TXPL_6_bp) && defined(USART_TXPL6_bp)
    #define USART_TXPL_6_bp USART_TXPL6_bp
  #elif defined(USART_TXPL_6_bp)
    //deprecated_constant_name USART_TXPL6_bp = USART_TXPL_6_bp;
    #define USART_TXPL6_bp USART_TXPL_6_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(USART_TXPL_7_bm) && defined(USART_TXPL7_bm)
    #define USART_TXPL_7_bm USART_TXPL7_bm
  #elif defined(USART_TXPL_7_bm)
    //deprecated_constant_name USART_TXPL7_bm = USART_TXPL_7_bm;
    #define USART_TXPL7_bm USART_TXPL_7_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(USART_TXPL_7_bp) && defined(USART_TXPL7_bp)
    #define USART_TXPL_7_bp USART_TXPL7_bp
  #elif defined(USART_TXPL_7_bp)
    //deprecated_constant_name USART_TXPL7_bp = USART_TXPL_7_bp;
    #define USART_TXPL7_bp USART_TXPL_7_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(USART_RXPL_0_bm) && defined(USART_RXPL0_bm)
    #define USART_RXPL_0_bm USART_RXPL0_bm
  #elif defined(USART_RXPL_0_bm)
    //deprecated_constant_name USART_RXPL0_bm = USART_RXPL_0_bm;
    #define USART_RXPL0_bm USART_RXPL_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(USART_RXPL_0_bp) && defined(USART_RXPL0_bp)
    #define USART_RXPL_0_bp USART_RXPL0_bp
  #elif defined(USART_RXPL_0_bp)
    //deprecated_constant_name USART_RXPL0_bp = USART_RXPL_0_bp;
    #define USART_RXPL0_bp USART_RXPL_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(USART_RXPL_1_bm) && defined(USART_RXPL1_bm)
    #define USART_RXPL_1_bm USART_RXPL1_bm
  #elif defined(USART_RXPL_1_bm)
    //deprecated_constant_name USART_RXPL1_bm = USART_RXPL_1_bm;
    #define USART_RXPL1_bm USART_RXPL_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(USART_RXPL_1_bp) && defined(USART_RXPL1_bp)
    #define USART_RXPL_1_bp USART_RXPL1_bp
  #elif defined(USART_RXPL_1_bp)
    //deprecated_constant_name USART_RXPL1_bp = USART_RXPL_1_bp;
    #define USART_RXPL1_bp USART_RXPL_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(USART_RXPL_2_bm) && defined(USART_RXPL2_bm)
    #define USART_RXPL_2_bm USART_RXPL2_bm
  #elif defined(USART_RXPL_2_bm)
    //deprecated_constant_name USART_RXPL2_bm = USART_RXPL_2_bm;
    #define USART_RXPL2_bm USART_RXPL_2_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(USART_RXPL_2_bp) && defined(USART_RXPL2_bp)
    #define USART_RXPL_2_bp USART_RXPL2_bp
  #elif defined(USART_RXPL_2_bp)
    //deprecated_constant_name USART_RXPL2_bp = USART_RXPL_2_bp;
    #define USART_RXPL2_bp USART_RXPL_2_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(USART_RXPL_3_bm) && defined(USART_RXPL3_bm)
    #define USART_RXPL_3_bm USART_RXPL3_bm
  #elif defined(USART_RXPL_3_bm)
    //deprecated_constant_name USART_RXPL3_bm = USART_RXPL_3_bm;
    #define USART_RXPL3_bm USART_RXPL_3_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(USART_RXPL_3_bp) && defined(USART_RXPL3_bp)
    #define USART_RXPL_3_bp USART_RXPL3_bp
  #elif defined(USART_RXPL_3_bp)
    //deprecated_constant_name USART_RXPL3_bp = USART_RXPL_3_bp;
    #define USART_RXPL3_bp USART_RXPL_3_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(USART_RXPL_4_bm) && defined(USART_RXPL4_bm)
    #define USART_RXPL_4_bm USART_RXPL4_bm
  #elif defined(USART_RXPL_4_bm)
    //deprecated_constant_name USART_RXPL4_bm = USART_RXPL_4_bm;
    #define USART_RXPL4_bm USART_RXPL_4_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(USART_RXPL_4_bp) && defined(USART_RXPL4_bp)
    #define USART_RXPL_4_bp USART_RXPL4_bp
  #elif defined(USART_RXPL_4_bp)
    //deprecated_constant_name USART_RXPL4_bp = USART_RXPL_4_bp;
    #define USART_RXPL4_bp USART_RXPL_4_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(USART_RXPL_5_bm) && defined(USART_RXPL5_bm)
    #define USART_RXPL_5_bm USART_RXPL5_bm
  #elif defined(USART_RXPL_5_bm)
    //deprecated_constant_name USART_RXPL5_bm = USART_RXPL_5_bm;
    #define USART_RXPL5_bm USART_RXPL_5_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(USART_RXPL_5_bp) && defined(USART_RXPL5_bp)
    #define USART_RXPL_5_bp USART_RXPL5_bp
  #elif defined(USART_RXPL_5_bp)
    //deprecated_constant_name USART_RXPL5_bp = USART_RXPL_5_bp;
    #define USART_RXPL5_bp USART_RXPL_5_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(USART_RXPL_6_bm) && defined(USART_RXPL6_bm)
    #define USART_RXPL_6_bm USART_RXPL6_bm
  #elif defined(USART_RXPL_6_bm)
    //deprecated_constant_name USART_RXPL6_bm = USART_RXPL_6_bm;
    #define USART_RXPL6_bm USART_RXPL_6_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(USART_RXPL_6_bp) && defined(USART_RXPL6_bp)
    #define USART_RXPL_6_bp USART_RXPL6_bp
  #elif defined(USART_RXPL_6_bp)
    //deprecated_constant_name USART_RXPL6_bp = USART_RXPL_6_bp;
    #define USART_RXPL6_bp USART_RXPL_6_bp //Deprecated as of Q2 2022 header change.
  #endif
/* VPORT.INTFLAGS */
  #if !defined(VPORT_INT_0_bm) && defined(VPORT_INT0_bm)
    #define VPORT_INT_0_bm VPORT_INT0_bm
  #elif defined(VPORT_INT_0_bm)
    //deprecated_constant_name VPORT_INT0_bm = VPORT_INT_0_bm;
    #define VPORT_INT0_bm VPORT_INT_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(VPORT_INT_0_bp) && defined(VPORT_INT0_bp)
    #define VPORT_INT_0_bp VPORT_INT0_bp
  #elif defined(VPORT_INT_0_bp)
    //deprecated_constant_name VPORT_INT0_bp = VPORT_INT_0_bp;
    #define VPORT_INT0_bp VPORT_INT_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(VPORT_INT_1_bm) && defined(VPORT_INT1_bm)
    #define VPORT_INT_1_bm VPORT_INT1_bm
  #elif defined(VPORT_INT_1_bm)
    //deprecated_constant_name VPORT_INT1_bm = VPORT_INT_1_bm;
    #define VPORT_INT1_bm VPORT_INT_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(VPORT_INT_1_bp) && defined(VPORT_INT1_bp)
    #define VPORT_INT_1_bp VPORT_INT1_bp
  #elif defined(VPORT_INT_1_bp)
    //deprecated_constant_name VPORT_INT1_bp = VPORT_INT_1_bp;
    #define VPORT_INT1_bp VPORT_INT_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(VPORT_INT_2_bm) && defined(VPORT_INT2_bm)
    #define VPORT_INT_2_bm VPORT_INT2_bm
  #elif defined(VPORT_INT_2_bm)
    //deprecated_constant_name VPORT_INT2_bm = VPORT_INT_2_bm;
    #define VPORT_INT2_bm VPORT_INT_2_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(VPORT_INT_2_bp) && defined(VPORT_INT2_bp)
    #define VPORT_INT_2_bp VPORT_INT2_bp
  #elif defined(VPORT_INT_2_bp)
    //deprecated_constant_name VPORT_INT2_bp = VPORT_INT_2_bp;
    #define VPORT_INT2_bp VPORT_INT_2_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(VPORT_INT_3_bm) && defined(VPORT_INT3_bm)
    #define VPORT_INT_3_bm VPORT_INT3_bm
  #elif defined(VPORT_INT_3_bm)
    //deprecated_constant_name VPORT_INT3_bm = VPORT_INT_3_bm;
    #define VPORT_INT3_bm VPORT_INT_3_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(VPORT_INT_3_bp) && defined(VPORT_INT3_bp)
    #define VPORT_INT_3_bp VPORT_INT3_bp
  #elif defined(VPORT_INT_3_bp)
    //deprecated_constant_name VPORT_INT3_bp = VPORT_INT_3_bp;
    #define VPORT_INT3_bp VPORT_INT_3_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(VPORT_INT_4_bm) && defined(VPORT_INT4_bm)
    #define VPORT_INT_4_bm VPORT_INT4_bm
  #elif defined(VPORT_INT_4_bm)
    //deprecated_constant_name VPORT_INT4_bm = VPORT_INT_4_bm;
    #define VPORT_INT4_bm VPORT_INT_4_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(VPORT_INT_4_bp) && defined(VPORT_INT4_bp)
    #define VPORT_INT_4_bp VPORT_INT4_bp
  #elif defined(VPORT_INT_4_bp)
    //deprecated_constant_name VPORT_INT4_bp = VPORT_INT_4_bp;
    #define VPORT_INT4_bp VPORT_INT_4_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(VPORT_INT_5_bm) && defined(VPORT_INT5_bm)
    #define VPORT_INT_5_bm VPORT_INT5_bm
  #elif defined(VPORT_INT_5_bm)
    //deprecated_constant_name VPORT_INT5_bm = VPORT_INT_5_bm;
    #define VPORT_INT5_bm VPORT_INT_5_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(VPORT_INT_5_bp) && defined(VPORT_INT5_bp)
    #define VPORT_INT_5_bp VPORT_INT5_bp
  #elif defined(VPORT_INT_5_bp)
    //deprecated_constant_name VPORT_INT5_bp = VPORT_INT_5_bp;
    #define VPORT_INT5_bp VPORT_INT_5_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(VPORT_INT_6_bm) && defined(VPORT_INT6_bm)
    #define VPORT_INT_6_bm VPORT_INT6_bm
  #elif defined(VPORT_INT_6_bm)
    //deprecated_constant_name VPORT_INT6_bm = VPORT_INT_6_bm;
    #define VPORT_INT6_bm VPORT_INT_6_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(VPORT_INT_6_bp) && defined(VPORT_INT6_bp)
    #define VPORT_INT_6_bp VPORT_INT6_bp
  #elif defined(VPORT_INT_6_bp)
    //deprecated_constant_name VPORT_INT6_bp = VPORT_INT_6_bp;
    #define VPORT_INT6_bp VPORT_INT_6_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(VPORT_INT_7_bm) && defined(VPORT_INT7_bm)
    #define VPORT_INT_7_bm VPORT_INT7_bm
  #elif defined(VPORT_INT_7_bm)
    //deprecated_constant_name VPORT_INT7_bm = VPORT_INT_7_bm;
    #define VPORT_INT7_bm VPORT_INT_7_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(VPORT_INT_7_bp) && defined(VPORT_INT7_bp)
    #define VPORT_INT_7_bp VPORT_INT7_bp
  #elif defined(VPORT_INT_7_bp)
    //deprecated_constant_name VPORT_INT7_bp = VPORT_INT_7_bp;
    #define VPORT_INT7_bp VPORT_INT_7_bp //Deprecated as of Q2 2022 header change.
  #endif
  /* ======= VREF ======= */
  #if !defined(VREF_REFSEL_0_bm) && defined(VREF_REFSEL0_bm)
    #define VREF_REFSEL_0_bm VREF_REFSEL0_bm
  #elif defined(VREF_REFSEL_0_bm)
    //deprecated_constant_name VREF_REFSEL0_bm = VREF_REFSEL_0_bm;
    #define VREF_REFSEL0_bm VREF_REFSEL_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(VREF_REFSEL_0_bp) && defined(VREF_REFSEL0_bp)
    #define VREF_REFSEL_0_bp VREF_REFSEL0_bp
  #elif defined(VREF_REFSEL_0_bp)
    //deprecated_constant_name VREF_REFSEL0_bp = VREF_REFSEL_0_bp;
    #define VREF_REFSEL0_bp VREF_REFSEL_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(VREF_REFSEL_1_bm) && defined(VREF_REFSEL1_bm)
    #define VREF_REFSEL_1_bm VREF_REFSEL1_bm
  #elif defined(VREF_REFSEL_1_bm)
    //deprecated_constant_name VREF_REFSEL1_bm = VREF_REFSEL_1_bm;
    #define VREF_REFSEL1_bm VREF_REFSEL_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(VREF_REFSEL_1_bp) && defined(VREF_REFSEL1_bp)
    #define VREF_REFSEL_1_bp VREF_REFSEL1_bp
  #elif defined(VREF_REFSEL_1_bp)
    //deprecated_constant_name VREF_REFSEL1_bp = VREF_REFSEL_1_bp;
    #define VREF_REFSEL1_bp VREF_REFSEL_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(VREF_REFSEL_2_bm) && defined(VREF_REFSEL2_bm)
    #define VREF_REFSEL_2_bm VREF_REFSEL2_bm
  #elif defined(VREF_REFSEL_2_bm)
    //deprecated_constant_name VREF_REFSEL2_bm = VREF_REFSEL_2_bm;
    #define VREF_REFSEL2_bm VREF_REFSEL_2_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(VREF_REFSEL_2_bp) && defined(VREF_REFSEL2_bp)
    #define VREF_REFSEL_2_bp VREF_REFSEL2_bp
  #elif defined(VREF_REFSEL_2_bp)
    //deprecated_constant_name VREF_REFSEL2_bp = VREF_REFSEL_2_bp;
    #define VREF_REFSEL2_bp VREF_REFSEL_2_bp //Deprecated as of Q2 2022 header change.
  #endif
  /* ======= WDT ======= */
  #if !defined(WDT_PERIOD_0_bm) && defined(WDT_PERIOD0_bm)
    #define WDT_PERIOD_0_bm WDT_PERIOD0_bm
  #elif defined(WDT_PERIOD_0_bm)
    //deprecated_constant_name WDT_PERIOD0_bm = WDT_PERIOD_0_bm;
    #define WDT_PERIOD0_bm WDT_PERIOD_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(WDT_PERIOD_0_bp) && defined(WDT_PERIOD0_bp)
    #define WDT_PERIOD_0_bp WDT_PERIOD0_bp
  #elif defined(WDT_PERIOD_0_bp)
    //deprecated_constant_name WDT_PERIOD0_bp = WDT_PERIOD_0_bp;
    #define WDT_PERIOD0_bp WDT_PERIOD_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(WDT_PERIOD_1_bm) && defined(WDT_PERIOD1_bm)
    #define WDT_PERIOD_1_bm WDT_PERIOD1_bm
  #elif defined(WDT_PERIOD_1_bm)
    //deprecated_constant_name WDT_PERIOD1_bm = WDT_PERIOD_1_bm;
    #define WDT_PERIOD1_bm WDT_PERIOD_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(WDT_PERIOD_1_bp) && defined(WDT_PERIOD1_bp)
    #define WDT_PERIOD_1_bp WDT_PERIOD1_bp
  #elif defined(WDT_PERIOD_1_bp)
    //deprecated_constant_name WDT_PERIOD1_bp = WDT_PERIOD_1_bp;
    #define WDT_PERIOD1_bp WDT_PERIOD_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(WDT_PERIOD_2_bm) && defined(WDT_PERIOD2_bm)
    #define WDT_PERIOD_2_bm WDT_PERIOD2_bm
  #elif defined(WDT_PERIOD_2_bm)
    //deprecated_constant_name WDT_PERIOD2_bm = WDT_PERIOD_2_bm;
    #define WDT_PERIOD2_bm WDT_PERIOD_2_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(WDT_PERIOD_2_bp) && defined(WDT_PERIOD2_bp)
    #define WDT_PERIOD_2_bp WDT_PERIOD2_bp
  #elif defined(WDT_PERIOD_2_bp)
    //deprecated_constant_name WDT_PERIOD2_bp = WDT_PERIOD_2_bp;
    #define WDT_PERIOD2_bp WDT_PERIOD_2_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(WDT_PERIOD_3_bm) && defined(WDT_PERIOD3_bm)
    #define WDT_PERIOD_3_bm WDT_PERIOD3_bm
  #elif defined(WDT_PERIOD_3_bm)
    //deprecated_constant_name WDT_PERIOD3_bm = WDT_PERIOD_3_bm;
    #define WDT_PERIOD3_bm WDT_PERIOD_3_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(WDT_PERIOD_3_bp) && defined(WDT_PERIOD3_bp)
    #define WDT_PERIOD_3_bp WDT_PERIOD3_bp
  #elif defined(WDT_PERIOD_3_bp)
    //deprecated_constant_name WDT_PERIOD3_bp = WDT_PERIOD_3_bp;
    #define WDT_PERIOD3_bp WDT_PERIOD_3_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(WDT_WINDOW_0_bm) && defined(WDT_WINDOW0_bm)
    #define WDT_WINDOW_0_bm WDT_WINDOW0_bm
  #elif defined(WDT_WINDOW_0_bm)
    //deprecated_constant_name WDT_WINDOW0_bm = WDT_WINDOW_0_bm;
    #define WDT_WINDOW0_bm WDT_WINDOW_0_bm //Deprecated as of Q2 2022 header change.
  #endif
#endif
#endif
