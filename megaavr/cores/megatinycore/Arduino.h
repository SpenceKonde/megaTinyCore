/* Arduino.h - Main include file for the Arduino SDK
 * Copyright (c) 2005-2013 Arduino Team.  All right reserved.
 * And presumably from then until 2018 when this was forked
 * for megaTinyCore. Copyright 2018-2022 Spence Konde
 * Part of megaTinyCore, which adds Arduino support for the
 * ATtiny 0/1/2-series microcontrollers from Microchip.
 * megaTinyCore is free software (LGPL 2.1)
 * See LICENSE.txt for full legal boilerplate if you must */
/*************************************************************
 * This file contains the stuff I think people are most likely
 * to need to refer to. The minutia has all been pushed into
 * core_devices.h if it's independent of pins_arduino.h or into
 * pinswap.h if it relates to PORTMUX, which is a great volume
 * of stuff nobody should have to read.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */
 /*
 * That means functions and macros that may be used by user code
 * (except for part-feature ones - those are clearly documented
 * in the readme if they are ready for users).
 * I also try to put detailed comments in where appropriate.
 *************************************************************/

#ifndef Arduino_h
#define Arduino_h

#include "api/ArduinoAPI.h"
#include "UART_constants.h"
#include "core_devices.h"
/* Gives names to all the timer pins - relies on core_devices.h being included first.*/
/* These names look like:
 * PIN_TCD0_WOC_DEFAULT
 * PIN_TCA0_WO5_ALT3
 * and so on.
 * They are #defines. Pins that don't exist are #defined as NOT_A_PIN.
 * TCA and TCD only currently */
#include <avr/pgmspace.h>
#include <avr/interrupt.h>

#ifdef __cplusplus
extern "C"{
#endif
  // Constant checks error handler
  void badArg(const char*) __attribute__((error("")));
  void badCall(const char*) __attribute__((error("")));
  inline __attribute__((always_inline)) void check_constant_pin(pin_size_t pin)
  {
    if (!__builtin_constant_p(pin))
      badArg("Fast digital pin must be a constant");
  }
/* ADC-related stuff */
/* With 2.3.0, we do the same thing as ATTinyCore and DxCore to specify
   that something is a channel number: set the high bit 1, since it is
   incredibly unlikely that a part with more than 127 digital pins will
   ever be supported by this core */

#define ADC_CH(ch)      (0x80 | (ch))

#if MEGATINYCORE_SERIES < 2
  /* ADC constants for 0/1-series */
  #define INTERNAL0V55    (VREF_ADC0REFSEL_0V55_gc >> VREF_ADC0REFSEL_gp)
  #define INTERNAL1V1     (VREF_ADC0REFSEL_1V1_gc >> VREF_ADC0REFSEL_gp)
  #define INTERNAL2V5     (VREF_ADC0REFSEL_2V5_gc >> VREF_ADC0REFSEL_gp)
  #define INTERNAL4V3     INTERNAL4V34
  #define INTERNAL4V34    (VREF_ADC0REFSEL_4V34_gc >> VREF_ADC0REFSEL_gp)
  #define INTERNAL1V5     (VREF_ADC0REFSEL_1V5_gc >> VREF_ADC0REFSEL_gp)

  #define DEFAULT         ADC_REFSEL_VDDREF_gc
  #define INTERNAL        ADC_REFSEL_INTREF_gc
  #define VDD             ADC_REFSEL_VDDREF_gc

  #if (defined(__AVR_ATtiny1614__) || defined(__AVR_ATtiny1616__) || defined(__AVR_ATtiny1617__) || defined(__AVR_ATtiny3216__) || defined(__AVR_ATtiny3217__))
    #define EXTERNAL      ADC_REFSEL_VREFA_gc
  #endif

  #define ADC_TEMPERATURE ADC_CH(ADC_MUXPOS_TEMPSENSE_gc)
  #define ADC_INTREF      ADC_CH(ADC_MUXPOS_INTREF_gc)
  #define ADC_GROUND      ADC_CH(ADC_MUXPOS_GND_gc)
  #ifdef DAC0
    #define ADC_DAC0      ADC_CH(ADC_MUXPOS_DAC0_gc)
    #define ADC_DACREF0   ADC_DAC0
  #endif

  #if defined(ADC1)
    #define ADC1_CH(ch)     (0xC0 | (ch))
    // All parts with the second adc have both extra dacrefs.
    #define ADC1_DAC0     ADC1_CH(ADC_MUXPOS_DAC0_gc)
    #define ADC1_DACREF0  ADC1_DAC0
    #define ADC1_DAC1     ADC1_CH(ADC_MUXPOS_TEMPSENSE_gc) // see section 30.5.7 MUXPOS register.
    #define ADC1_DACREF1  ADC1_DAC1
    #define ADC1_DAC2     ADC1_CH(0x1B) // see section 30.5.7 MUXPOS register.
    #define ADC1_DACREF2  ADC1_DAC2
  #endif
  #define ADC_DEFAULT_SAMPLE_LENGTH 14
  #define ADC_ACC2        0x81
  #define ADC_ACC4        0x82
  #define ADC_ACC8        0x83
  #define ADC_ACC16       0x84
  #define ADC_ACC32       0x85
  #define ADC_ACC64       0x86
#else
  /* ADC constants for 2-series */
  #define VDD             (0) /* ADC_REFSEL_VDD_gc    */
  #define DEFAULT         VDD /* Gee, I really wish these were named differently - both names are horrendously generic and could mean lots of different things that should be distinguished. */
  #define EXTERNAL        (2) /* ADC_REFSEL_VREFA_gc  */
  #define INTERNAL1V024   (4) /* ADC_REFSEL_1024MV_gc */
  #define INTERNAL2V048   (5) /* ADC_REFSEL_2048MV_gc */
  #define INTERNAL2V5     (6) /* ADC_REFSEL_2500MV_gc */
  #define INTERNAL4V096   (7) /* ADC_REFSEL_4096MV_gc */
  #define INTERNAL4V1     INTERNAL4V096 /* Alias */

  #define AC_REF_1V024    (VREF_AC0REFSEL_1V024_gc)
  #define AC_REF_2V048    (VREF_AC0REFSEL_2V048_gc)
  #define AC_REF_2V5      (VREF_AC0REFSEL_2V5_gc)
  #define AC_REF_4V096    (VREF_AC0REFSEL_4V096_gc)
  #define AC_REF_VDD      (VREF_AC0REFSEL_AVDD_gc)
  #define AC_REF_4V1      AC_REF_4V096/* Alias */

  #define ADC_TEMPERATURE ADC_CH(ADC_MUXPOS_TEMPSENSE_gc)
  #define ADC_GROUND      ADC_CH(ADC_MUXPOS_GND_gc)
  #define ADC_DACREF0     ADC_CH(ADC_MUXPOS_DACREF0_gc)
  #define ADC_DAC0        ADC_DACREF0 /* for compatibility, since on tinyAVR 0/1-seies, the DAC0 voltage is also AC0 DACREF if used */
  #define ADC_VDDDIV10    ADC_CH(ADC_MUXPOS_VDDDIV10_gc)

  /* >= 1us - can't use clockcycles per microsecond from timers.h because
  this needs to always round up */
  #if !(F_CPU >= 32000000)
    #define TIMEBASE_1US (((F_CPU + 999999UL)/1000000UL) << ADC_TIMEBASE_gp)
  #else
    #define TIMEBASE_1US (31 << ADC_TIMEBASE_gp)
  #endif

  #define ADC_DEFAULT_SAMPLE_LENGTH 15
  #define ADC_ACC2        0x81
  #define ADC_ACC4        0x82
  #define ADC_ACC8        0x83
  #define ADC_ACC16       0x84
  #define ADC_ACC32       0x85
  #define ADC_ACC64       0x86
  #define ADC_ACC128      0x87
  #define ADC_ACC256      0x88
  #define ADC_ACC512      0x89
  #define ADC_ACC1024     0x8A

  #define LOW_LAT_ON      0x03
  #define LOW_LAT_OFF     0x02
  #define PGA_KEEP_ON     0x08
  #define PGA_AUTO_OFF    0x0C
  #define PGA_OFF_ONCE    0x04
  #define ADC_ENABLE      0x20
  #define ADC_DISABLE     0x30
  #define ADC_STANDBY_ON  0xC0
  #define ADC_STANDBY_OFF 0x80


#endif

/* Errors in analogReadEnh and analogReadDiff are large negative numbers,
 * since it's a signed long  returned, and  a raw maximally accumulated
 * differential reading could be a huge negative number. Largest negative
 * possible is -2 147 483 648; It would be hard to tell the difference between
 * that and -2147483647, or -2147483646 and remember which is which,
 * so they start at -2100000000.
 * Errors for normal analogRead are small negative numbers because
 * analogRead should never return a negative. Neither should analogReadEnh
 * but I decided to have only 2 sets of ADC errors, not three.  */

#define ADC_ERROR_BAD_PIN_OR_CHANNEL                (-32001)
#define ADC_ERROR_BUSY                              (-32002)
#define ADC_ERROR_DISABLED                          (-32007)
#define ADC_ENH_ERROR_BAD_PIN_OR_CHANNEL       (-2100000001)
// positive channel is not (0x80 | valid_channel) nor a digital pin number
// referring to a pin with analog input.
#define ADC_ENH_ERROR_BUSY                     (-2100000002)
// The ADC is currently performing another conversion in the background (either
// in free-running mode or a long-running burst conversion; taking a burst
// accumulated reading and then calling a specified function when the result
// was finally ready may be supported in a future version.
#define ADC_ENH_ERROR_RES_TOO_LOW              (-2100000003)
// analogReadEnh() must not be called with a resolution lower than 8-bits.
// you can right-shift as well as the library can, without wasting flash for everyone who doesn't need to.
#define ADC_ENH_ERROR_RES_TOO_HIGH             (-2100000004)
// Only resonlutions that can be generated through accumulator oversample
// + decimation are supported, maximum is 13, 15, or 17 bits. This will
// also be returned if a larger raw accumulated result is requested.
#define ADC_DIFF_ERROR_BAD_NEG_PIN             (-2100000005)
// Analog pin given as negative pin is not a valid negative mux pin
#define ADC_ENH_ERROR_NOT_DIFF_ADC             (-2100000006)
// analogReadDiff() called from a part without a differential ADC;
// Never actually returned, because we give compile error here
#define ADC_ENH_ERROR_DISABLED                 (-2100000007)
// The ADC is not currently enabled. This error is disabled currently - if analogReadEnh encounters a disabled ADC, it will enable it, take the reading, and disable it again.
#define ADC_ERROR_INVALID_CLOCK                     (-32255)
// Returned by analogClockSpeed if the value in the register is currently unknown, or if an invalid frequency is requested.


// only returned by analogCheckError()
#define ADC_IMPOSSIBLE_VALUE                        (-127)


#if (!defined(TCB_CLKSEL2_bm))
  // This means it's a tinyAVR 0/1-series, or a megaAVR 0-series.
  // Their TCB_CLKSEL enums use different names for the clock settings, for reasons unclear.
  // To align with the future, we use the Dx-series names for these.
  #define TCB_CLKSEL_DIV2_gc TCB_CLKSEL_CLKDIV2_gc
  #define TCB_CLKSEL_DIV1_gc TCB_CLKSEL_CLKDIV1_gc
#endif

#define VCC_5V0 2
#define VCC_3V3 1
#define VCC_1V8 0

#define interrupts() sei()
#define noInterrupts() cli()


// NON-STANDARD API


void init_ADC0(void); /* Called by init() after clock is set */
#if defined(ADC1)
  void init_ADC1(void); /* Never called automatically, but must be called manuaklkly in order to use the ADC1 functions. */
#endif
void init_clock(void);/* called by init() first  */
void init_millis();   /* called by init() last   */
void init_timers();   /* called by init()        */
void init_TCA0();     /* called by init_timers() */
void init_TCD0();     /* called by init_timers() */

// callbacks normally empty and optimized away.
void onPreMain();
void onBeforeInit();
uint8_t onAfterInit();
void initVariant();


// Peripheral takeover
// These will remove things controlled by
// these timers from analogWrite()/turnOffPWM()
// 0x40 - TCD0, 0x10 - TCA0
void takeOverTCA0();
void takeOverTCD0();

// millis() timer control
void stop_millis();                   // Disable the interrupt and stop counting millis.
void restart_millis();                // Reinitialize the timer and start counting millis again
void set_millis(uint32_t newmillis);  // set current millis time.
/* Expected usage:
 * uint32_t oldmillis=millis();
 * stop_millis();
 * user_code_that_messes with timer
 * set_millis(oldmillis+estimated_time_spent_above)
 * restart millis();
 *
 * Also, this might at times be appropriate
 * set_millis(millis() + known_offset);
 * after doing something that we know will block too long for millis to keep time
 * see also:
 */


/* inlining of a call to delayMicroseconds() would throw it off */
void _delayMicroseconds(unsigned int us) __attribute__((noinline));


/*******************************************************
 * Extended API - Enhanced DxCore features             *
 * These are functions users might call from the       *
 * sketch, or in some cases, override.                 *
 * 1. Timer + millis() control.                        *
 * 2. initialization routines to override              *
 * 3. Advanced Analog functionality                    *
 * 4. Advanced Digital functionality                   *
 ******************************************************/
// stop, restart and set millis intended for switching to RTC for millis timekeeping while sleeping.
void stop_millis();                          // stop the timer being used for millis, and disable the interrupt.
void restart_millis();                       // After having stopped millis either for sleep or to use timer for something else and optionally have set it to correct for passage of time, call this to restart it.
void set_millis(uint32_t newmillis);         // Sets the millisecond timer to the specified number of milliseconds. DO NOT CALL with a number lower than the current millis count if you have any timeouts ongoing.
                                             // they may expire instantly.
// void nudge_millis(uint16_t nudgemillis);  // Sets the millisecond timer forward by the specified number of milliseconds. Currently only implemented for TCB, TCA implementation will be added. This allows a clean
// Not yet implemented, debating if          // way to advance the timer without needing to read the current millis yourself, and without a few other risks. (added becauise *I* needed it, but simple enough).
// this is the right thing to implement      // The intended use case is when you know you're disabling millis for a long time, and know exactly how long that is (ex, to update neopixels), and want to nudge the timer
                                             // forward by a given amount; I added this when in a pinch because *I* needed that functionality.

// Allows for user to mark a timer "do not touch" for purposes of analogWrite and the like, so you can take over a timer and reconfigure it, and not worry about digitalWrite() flipping a CMPEN bit.
// On megaTinyCore this also prevents the situation where PWM is remapped, but then when the user is digitalWrite'ing pins that default to having PWM, it would turn off the PWM now coming from another pin
// This is not an issue because we fully support portmux (can't spare the flash overhead on the tinies, people already complain that the core uses too much flash)
void takeOverTCA0();                         // Can be used to tell core not to use TCA0 for any API calls - user has taken it over.
void takeOverTCA1();                         // Can be used to tell core not to use TCA1 for any API calls - user has taken it over.
void takeOverTCD0();                         // Can be used to tell core not to use TCD0 for any API calls - user has taken it over.
void resumeTCA0();                           // Restores core-mediated functionality that uses TCA0 and restores default TCA0 configuration.
void resumeTCA1();                           // Restores core-mediated functionality that uses TCA1 and restores default TCA1 configuration.
// bool digitalPinHasPWM(uint8_t p);         // Macro. Returns true if the pin can currently output PWM using analogWrite(), regardless of which timer is used and considering current PORTMUX setting
uint8_t digitalPinToTimerNow(uint8_t p);     // Returns the timer that is associated with the pin now (considering PORTMUX)

// These are in here so that - should it be necessary - library functions or user code could override these.
void init_clock()     __attribute__((weak)); // this is called first, to initialize the system clock.
void init_ADC0()      __attribute__((weak)); // this is called to initialize ADC0
//   init_DAC0()                             // no _init_DAC0() - all that the core does is call DACReference().
void init_TCA0()      __attribute__((weak)); // called by init_timers() - without this, pins that give PWM from TCA0 will not function.
void init_TCA1()      __attribute__((weak)); // called by init_timers() - without this, pins that give PWM from TCA1 will not function, nor will the TCBs unless the clock source is changed.
void init_TCBs()      __attribute__((weak)); // called by init_timers()
void init_TCD0()      __attribute__((weak)); // called by init_timers()
void init_millis()    __attribute__((weak)); // called by init() after everything else and just before enabling interrupts and calling setup() - sets up and enables millis timekeeping.

void onClockFailure() __attribute__((weak)); // called by the clock failure detection ISR. Default action is a blink code with 4 blinks.
void onClockTimeout() __attribute__((weak)); // called if we try to switch to external clock, but it doesn't work. Default action is a blink code with 3 blinks.

#ifndef CORE_ATTACH_OLD
// The old attachInterrupt did not require any calls to be made to enable a port.
// It would just grab every port, and take over every port's pin interrupt ISR, so nobody could define one themselves.
// which wouldn't be so bad... except that attachInterrupt interrupts are slow as hell
  void attachPortAEnable();
  void attachPortBEnable();
  void attachPortCEnable();
  void attachPortDEnable();
  void attachPortEEnable();
  void attachPortFEnable();
  void attachPortGEnable();
#endif

// ANALOG EXTENDED FUNCTIONS
// Covered in documentation.
int32_t     analogReadEnh(        uint8_t pin, /* no neg */ uint8_t res, uint8_t gain);
int32_t     analogReadDiff(       uint8_t pos, uint8_t neg, uint8_t res, uint8_t gain);
int16_t     analogClockSpeed(     int16_t frequency,        uint8_t options);
bool        analogSampleDuration( uint8_t dur);
void        ADCPowerOptions(      uint8_t options); // fewer options on 0/1=seroes
#if defined(ADC1)
  int32_t     analogReadEnh1(       uint8_t pin, /* no neg */ uint8_t res, uint8_t gain);
  int32_t     analogReadDiff1(      uint8_t pos, uint8_t neg, uint8_t res, uint8_t gain); /* This just errors because these parts don't have a differential ADC */
  int16_t     analogClockSpeed1(    int16_t frequency,        uint8_t options);
  bool        analogSampleDuration1(uint8_t dur);
  int16_t     analogRead1(          uint8_t pin);
  void        analogReference1(     uint8_t mode);
  void        ADCPowerOptions1(      uint8_t options);  // Fewer optiopms
#endif
void        DACReference(         uint8_t mode);

int32_t           analogReadEnh(uint8_t pin,                uint8_t res,  uint8_t gain);
int32_t          analogReadDiff(uint8_t pos,  uint8_t neg,  uint8_t res,  uint8_t gain);
int16_t        analogClockSpeed(int16_t frequency, uint8_t options);
bool       analogReadResolution(uint8_t res);
bool       analogSampleDuration(uint8_t dur);
void               DACReference(uint8_t mode);

uint8_t      getAnalogReference();
uint8_t         getDACReference();
int8_t  getAnalogReadResolution();

//
// DIGITAL I/O EXTENDED FUNCTIONS
// Covered in documentation.
void           openDrain(uint8_t pinNumber,   uint8_t val);
int8_t   digitalReadFast(uint8_t pinNumber               );
void    digitalWriteFast(uint8_t pinNumber,   uint8_t val);
void         pinModeFast(uint8_t pinNumber,  uint8_t mode);
void       openDrainFast(uint8_t pinNumber,   uint8_t val);
void        pinConfigure(uint8_t pinNumber, uint16_t mode);
void          turnOffPWM(uint8_t pinNumber               );

// Not a function, still important
#define digitalPinHasPWMNow(p)            (digitalPinToTimerNow(p) != NOT_ON_TIMER)

uint8_t PWMoutputTopin(uint8_t timer, uint8_t channel);
// Realized we're not going to be able to make generic code without this.

// Again as above, but this time with the unwieldy 8-byte integer datatype as the base
// avr-libc defines _NOP() since 1.6.2
// Really? Better tell avr-gcc that, it seems to disagree...
#ifndef _NOP
  #define _NOP()    __asm__ __volatile__ ("nop")
#endif
#ifndef _NOP2
  #define _NOP2()   __asm__ __volatile__ ("rjmp .+0")
#endif
#ifndef _NOPNOP
  #define _NOPNOP() __asm__ __volatile__ ("rjmp .+0")
#endif
#ifndef _NOP8
  #define _NOP8()   __asm__ __volatile__ ("rjmp .+2"  "\n\t"   /* 2 clk jump over next instruction */ \
                                          "ret"       "\n\t"   /* 4 clk return "wha? why here?" */    \
                                          "rcall .-4" "\n\t" ) /* 2 clk "Oh, I see. We jump over a return (2 clock) call it, and then immediately return." */
#endif

/*
Not enabled. Ugly ways to get delays at very small flash cost.
#ifndef _NOP6
  #define _NOP6()   __asm__ __volatile__ ("rcall lonereturn") // 2 bytes of flash.  2+4=6 clk only works if you've got _LONE_RETURN() somewhere. Only guaranteed to work on 8k and smaller parts.
  #define _NOP7()   __asm__ __volatile__ ("call lonereturn")  // 4 bytes of flash.  3+4=7 clk and see above, except that this will only work w/>8k flash.
  #define _LONE_RETURN() __asm__ __volatile__ ("rjmp .+2"    "\n\t"  \  // 4 bytes of flash overhead, but must exist once and only once for NOP6/7 (but not any others) . Don't trip over thr ret. Note that if you're writing inline assembly with ret elsewhere, just proceed
                                               "lonereturn:" "\n\t"  \  // it with a a label and jump to it to save 2 bytes vs this methodMust exist somwehere for
                                                 "ret"         "\n\t" )
  // It could be put into it's own function and marked with the "used" attribute. This allows elimination of the initial rjmp, at the cost of making an ugly hack even uglier.
  // Or even worse, you have other inline assembly, and you just stick the label right before the return!
  // Really, these are things you shoudnt do unless you have your back against the flash/RAM limits and a gun to your head.
  #endif
*/
#ifndef _NOP14
  #define _NOP14()  __asm__ __volatile__ ("rjmp .+2"  "\n\t"   /* same idea as above. */ \
                                          "ret"       "\n\t"   /* Except now it's no longer able to beat the loop if it has a free register. */ \
                                          "rcall .-4" "\n\t"   /* so this is unlikely to be better, ever. */ \
                                          "rcall .-6" "\n\t" )
#endif
/* Beyond this, just use a loop.
 * If you don't need submicrosecond accuracy, just use delayMicroseconds(), which uses very similar methods. See Ref_Timers
 * (eg, ldi (any upper register), n; dec r0; brne .-4)
 * and pad with rjmp or nop if needed to get target delay.
 * Simplest form takes a uint8_t and runs for 3n cycles in 3 words. Padded with `nop` or `rjmp .+0`for 3n + 1 or 3n + 2 if outside the loop, 4n or 5n if padded inside the loop
 * And so on. You will likely end up doing something like
 *
                    #define CLOCKS_PER_US   (F_CPU / 1000000);    // preprocessed away
                    #define DELAYCLOCKS     (0.8 * CLOCKS_PER_US) // say we wanted a 0.8 us delay.
                    uint8_t x = DELAYCLOCKS / 3;                  // preprocessed into a constant
                    __asm__ __volatile__ ("dec %0"      "\n\t"    // before this, an ldi is used to load x into the input operand %0
                                          "brne .-4"    "\n\t"
                      #if (DELAYCLOCKS % 3 == 2)                  // 2 clocks extra needed at end
                                          "rjmp .+0"    "\n\t"
                      #elif (DELAYCLOCKS % 3 == 1)                // 1 clock extra needed at end
                                          "nop"         "\n\t"
                      #endif
                                          : "+d"((uint8_t)(x));
 *
 * The above will take very close to 0.8us under most any conditions.  Notice how all the calculation was moved to the preprocessor.
 *
 *
 * You can extend the length of the iterations by adding nop between the dec and brne, and branching 2 bytes further. that makes it 4 clocks per iteration.
 * You can go for much longer by using 16-bits:
 *                  uint16_t x = 2000;    * overall takes 8 bytrsd
 *                  __asm__ __volatile__ ("sbiw %0,1"    "\n\t"  // Line is preceded by 2 implied LDI's to fill that upper register pair, Much higher chance of having to push and pop.
 *                                        \"brne .-4      \"\n\t\"  // SBIW takes 2 clocks. branch takes 2 clocks unless it doesn't branch, when it only takes one
 *                                        : +w"((uint16_t)(x))  // hence this takes 4N+1 clocks (4 per iteration, except for last one which is only 3, plus 2 for the pair of LDI's)
 *
 */


// The fastest way to swap nybbles
#ifndef _SWAP
  #define _SWAP(n) __asm__ __volatile__ ("swap %0"  "\n\t" :"+r"((uint8_t)(n)));
#endif
// internally used - fast multiply by 0x20 that assumes x < 8, so you can add it to a uint8_t* to PORTA or member of PORTA,
// and get the corresponding value for the other port, or equivalently it can be added to 0x0400 which is the address of PORTA.
// Valid only with a valid port number, which must be verified first
// This exists to sidestep inefficiency of compiler generated code when you only know the port at runtime, for the very common task of
// addressing a port register by port number and offset. Trashes the variable you pass it
/*
#define _WRITE_VALUE_TO_PORT_OFFSET(p,o,v) ({
          __asm__ __volatile__ (
            "swap %0A"        "\n\t" // start with a 16-bit pointer register
            "add %0A, %0A "   "\n\t" // low byte for port register is port * 0x20 - so swap nybbles and leftshift to do in 2 cycles.
            "ldi %0B, 0x04"   "\n\t" // high byte for all port registers is 0x0400
            "add %0A, %1"     "\n\t" // add the offset within the port
            "st %a0, %2"      "\n\t" // write the value
           :"+e"(uint16_t)(p)
           :"r"  (uint8_t)(o),
            "r"  (uint8_t)(v)
          );
})
*/
uint16_t clockCyclesPerMicrosecond();
uint32_t clockCyclesToMicroseconds(uint32_t cycles);
uint32_t microsecondsToClockCycles(uint32_t microseconds);

// Currently DxCore has no cases where the millis timer isn't derived from system clock, but that will change
/* This becomes important when we support other timers for timekeeping. The Type D timer can be faster, requiring:
 * These talk about the timebase from which millis is derived, not the actual timer counting frequency.
 * That is, it's used to calculqte the values that are we multipliy by the prescaler to get the timekeeping stuff.
 * These can be different from the above only when the millis timer isn't running from CLK_PER.
     For example, if we run it from a TCD clocked from internal HF but we are running on a crystal.
     That's a strange way to use the TCD, but
 * megaTinyCore has these, and we will have wsomething analogous.

uint16_t millisClockCyclesPerMicrosecond();
uint32_t millisClockCyclesToMicroseconds(uint32_t cycles);
uint32_t microsecondsToMillisClockCycles(uint32_t microseconds);

 * the time and sleep library will require some things like this.
 */


// Copies of above for internal use, and for the really exotic use cases that want this instead of system clocks (basically never in user-land)
uint16_t millisClockCyclesPerMicrosecond();
unsigned long millisClockCyclesToMicroseconds(unsigned long cycles);
unsigned long microsecondsToMillisClockCycles(unsigned long microseconds);


__attribute__ ((noinline)) void _delayMicroseconds(unsigned int us);


// Get the bit location within the hardware port of the given virtual pin.
// This comes from the arrays in the  pins_arduino.c file for the active
// board configuration.
// These perform slightly better as macros compared to inline functions

extern const uint8_t digital_pin_to_port[];
extern const uint8_t digital_pin_to_bit_mask[];
extern const uint8_t digital_pin_to_bit_position[];
extern const uint8_t digital_pin_to_timer[];




// These are used as the second argument to pinConfigure(pin, configuration)
// You can bitwise OR as many of these as you want, or just do one. Very
// flexible function; not the world's fastest though. Directives are handled
// in the order they show up on this list, by pin function:
// PIN_DIR      Direction
// PIN_OUT      Output value
// PIN_ISC      Enable and interrupt mode. If interrupts are turned on w/out the ISR, it will trigger dirty reset.
// PIN_PULLUP   Pullups
// PIN_INLVL    Input levels (MVIO parts only - everything else is schmitt trigger only, except on I2C pins acting as I2C with SMBus levels enabled. )
// PIN_INVERT   Invert pin
//
// Systematically named constants can be made by combining those names with the postfixes here
// except for PIN_ISC which is not a non-binary option. Valid values are listed below.
// _SET, _CLR, and _TGL can be used as a postfix on all binary options.
// _TOGGLE and _TGL are interchangeable as well.
// Additional names are defined where they might be easier to remember.
// It's not an accident that the PORT options have PIN_(name of register in PORTx)
// as an alias.
// Microchip can add one more binary option >.>

/* normal PORT binary options */
#define PIN_DIR_SET          (0x0001) // OUTPUT
#define PIN_DIRSET           (0x0001) // alias
#define PIN_DIR_OUTPUT       (0x0001) // alias
#define PIN_DIR_OUT          (0x0001) // alias
#define PIN_DIR_CLR          (0x0002) // INPUT
#define PIN_DIRCLR           (0x0002) // alias
#define PIN_DIR_INPUT        (0x0002) // alias
#define PIN_DIR_IN           (0x0002) // alias
#define PIN_DIR_TGL          (0x0003) // TOGGLE INPUT/OUTPUT
#define PIN_DIRTGL           (0x0003) // alias
#define PIN_DIR_TOGGLE       (0x0003) // alias
#define PIN_OUT_SET          (0x0004) // HIGH
#define PIN_OUTSET           (0x0004) // alias
#define PIN_OUT_HIGH         (0x0004) // alias
#define PIN_OUT_CLR          (0x0008) // LOW
#define PIN_OUTCLR           (0x0008) // alias
#define PIN_OUT_LOW          (0x0008) // alias
#define PIN_OUT_TGL          (0x000C) // CHANGE/TOGGLE
#define PIN_OUTTGL           (0x000C) // alias
#define PIN_OUT_TOGGLE       (0x000C) // alias
// reserved                  (0x0010) // reserved - couldn't be combined with the ISC options
// reserved                  (0x0020) // reserved - couldn't be combined with the ISC options
// reserved                  (0x0030) // reserved - couldn't be combined with the ISC options
// reserved                  (0x0040) // reserved - couldn't be combined with the ISC options
// reserved                  (0x0050) // reserved - couldn't be combined with the ISC options
// reserved                  (0x0060) // reserved - couldn't be combined with the ISC options
// reserved                  (0x0070) // reserved - couldn't be combined with the ISC options
/* Interrupt and input enable nybble is: 0b1nnn to set to option n, or 0b0xxx to not, and ignore those bits. */
#define PIN_ISC_ENABLE       (0x0080) // No interrupts and enabled.
#define PIN_INPUT_ENABLE     (0x0080) // alias
#define PIN_ISC_CHANGE       (0x0090) // CHANGE
#define PIN_INT_CHANGE       (0x0090) // alias
#define PIN_ISC_RISE         (0x00A0) // RISING
#define PIN_INT_RISE         (0x00A0) // alias
#define PIN_ISC_FALL         (0x00B0) // FALLING
#define PIN_INT_FALL         (0x00B0) // alias
#define PIN_ISC_DISABLE      (0x00C0) // DISABLED
#define PIN_INPUT_DISABLE    (0x00C0) // alias
#define PIN_ISC_LEVEL        (0x00D0) // LEVEL
#define PIN_INT_LEVEL        (0x00D0) // alias
/* PINnCONFIG binary options */
#define PIN_PULLUP_ON        (0x0100) // PULLUP ON
#define PIN_PULLUP           (0x0100) // alias
#define PIN_PULLUP_SET       (0x0100) // alias
#define PIN_PULLUP_OFF       (0x0200) // PULLUP OFF
#define PIN_PULLUP_CLR       (0x0200) // alias
#define PIN_NOPULLUP         (0x0200) // alias
#define PIN_PULLUP_TGL       (0x0300) // PULLUP TOGGLE
#define PIN_PULLUP_TOGGLE    (0x0300) // alias
// reserved                  (0x0400) // reserved
// reserved                  (0x0800) // reserved
// reserved                  (0x0C00) // reserved
#define PIN_INLVL_TTL        (0x1000) // TTL INPUT LEVELS - MVIO parts only
#define PIN_INLVL_ON         (0x1000) // alias MVIO parts only
#define PIN_INLVL_SET        (0x1000) // alias MVIO parts only
#define PIN_INLVL_SCHMITT    (0x2000) // SCHMITT INPUT LEVELS - MVIO parts only
#define PIN_INLVL_OFF        (0x2000) // alias MVIO parts only
#define PIN_INLVL_CLR        (0x2000) // alias MVIO parts only
// reserved                  (0x3000) // INLVL TOGGLE - not supported. If you tell me a reasonable use case
// I'll do it.each possible value is handled separately, slowing it down, and I don't think this would get used.
#define PIN_INVERT_ON        (0x4000) // PIN INVERT ON
#define PIN_INVERT_SET       (0x4000) // alias
#define PIN_INVERT_OFF       (0x8000) // PIN INVERT OFF
#define PIN_INVERT_CLR       (0x8000) // alias
#define PIN_INVERT_TGL       (0xC000) // PIN_INVERT_TOGGLE
#define PIN_INVERT_TOGGLE    (0xC000) // alias

/*
Supplied by Variant file:
#define digitalPinToAnalogInput(p)      // Given digital pin (p), returns the analog channel number, or NOT_A_PIN if the pin does not suipport analog input.
#define analogChannelToDigitalPin(p)    // Inverse of above. Given analog chanbel number (p) in raw form not ADC_CH() form, returns the digital pin number corresponding to it.
#define analogInputToDigitalPin(p)      // Similar to previous. Given analog input number (p) with the high bit set, returns the digital pin number corresponding to it)
#define digitalOrAnalogPinToDigital(p)  // Given either an analog input number (with high bit set) or a digital pin number (without it set), returns the digital pin number.
Yes, these are poorky named and do not use analog input, analog pin, and analog channel consistently. Unfortunately the names of some of these were set in stone by virtue of their being preexisting macros used in code in the wild.
See Ref_Analog.md for more information of the representations of "analog pins". I blame Arduino for the original sin of "analog pins" as a concept in the first place.
*/

#define digitalPinToPort(pin)               (((pin)     < NUM_TOTAL_PINS ) ?                          digital_pin_to_port[pin]                 : NOT_A_PIN)
#define digitalPinToBitPosition(pin)        (((pin)     < NUM_TOTAL_PINS ) ?                  digital_pin_to_bit_position[pin]                 : NOT_A_PIN)
#define digitalPinToBitMask(pin)            (((pin)     < NUM_TOTAL_PINS ) ?                      digital_pin_to_bit_mask[pin]                 : NOT_A_PIN)
#define analogPinToBitPosition(pin)         ((digitalPinToAnalogInput(pin) !=  NOT_A_PIN) ?   digital_pin_to_bit_position[pin]                 : NOT_A_PIN)
#define analogPinToBitMask(pin)             ((digitalPinToAnalogInput(pin) !=  NOT_A_PIN) ?       digital_pin_to_bit_mask[pin]                 : NOT_A_PIN)
#define digitalPinToTimer(pin)              (((pin)     < NUM_TOTAL_PINS ) ?                         digital_pin_to_timer[pin]                 : NOT_ON_TIMER)
#define portToPortStruct(port)              (((port)    < NUM_TOTAL_PORTS) ?                   (((PORT_t *)  &PORTA) + (port))                 : NULL)
#define digitalPinToPortStruct(pin)         (((pin)     < NUM_TOTAL_PINS ) ?    (((PORT_t *) &PORTA) + digitalPinToPort( pin))                 : NULL)
#define getPINnCTRLregister(port, bit_pos)  ((((port) != NULL) && (bit_pos < 8)) ? (((volatile uint8_t *) &(port->PIN0CTRL)) + bit_pos)        : NULL)
#define digitalPinToInterrupt(P)            (P)
// Remember to test for NOT_A_PORT before using thiese.
#define portOutputRegister(P) ((volatile uint8_t *)(&portToPortStruct(P)->OUT))
#define portInputRegister(P)  ((volatile uint8_t *)(&portToPortStruct(P)->IN ))
#define portModeRegister(P)   ((volatile uint8_t *)(&portToPortStruct(P)->DIR))
#if defined(PORTA_EVGENCTRL) //Ex-series only - this all may belong in the Event library anyway, but since the conditional is never met, this code is never used.
  #define portEventRegister(p)  ((volatile uint8_t *)(&portToPortStruct(P)->EVGENCTRL))
  uint8_t setEventPin(uint8_t pin, uint8_t number); // preliminary thought - pass a pin number, it looks up port, and from there the event control register and sets it.
  //Number being 0 or 1 or 255 to pick the lowest numbered one not set. Returns event channel number TBD if that should be the EVSYS valus or 0 or 1. If "Pick unused ome" is requested but both already assigned, will return 255
  uint8_t getPortEventConfig(uint8_t port); // just shorthand for looking up the port and returning it's EVGENCTRL value
  uint8_t setRTCEventChan(uint8_t div, uint8_t number); // number is 0, 1 or 255 like above, div is log(2) of the divisor (ie, for 2^5, his would be 5).
  uint8_t getRTCEventConfig(); //simply returns the RTC channel configuration. Will likely return 255 if called on non Ex
#endif
#ifdef __cplusplus
} // extern "C"
#endif

#ifdef __cplusplus
  #include "UART.h"
  int32_t analogReadEnh( uint8_t pin,              uint8_t res = ADC_NATIVE_RESOLUTION, uint8_t gain = 0);
  int32_t analogReadDiff(uint8_t pos, uint8_t neg, uint8_t res = ADC_NATIVE_RESOLUTION, uint8_t gain = 0);
  int16_t analogClockSpeed(int16_t frequency = 0, uint8_t options = 0);
  #if defined(ADC1)
    int32_t analogReadEnh1(uint8_t pin,              uint8_t res = ADC_NATIVE_RESOLUTION, uint8_t gain = 0);
    int32_t analogReadDiff1(uint8_t pos, uint8_t neg, uint8_t res = ADC_NATIVE_RESOLUTION, uint8_t gain = 0);
    int16_t analogClockSpeed1(int16_t frequency = 0, uint8_t options = 0);
  #endif
#endif

#include "pins_arduino.h"

// Based on those, some ugly formulae for "smart-pin" defines that follow the mux regs around:


#ifdef PIN_WIRE_SCL_PINSWAP_1
  #define SDA_NOW ((uint8_t) (PORTMUX.CTRLB & PORTMUX_TWI0_bm ? PIN_WIRE_SDA_PINSWAP_1 : PIN_WIRE_SDA))
  #define SCL_NOW ((uint8_t) (PORTMUX.CTRLB & PORTMUX_TWI0_bm ? PIN_WIRE_SCL_PINSWAP_1 : PIN_WIRE_SCL))
  static const uint8_t SDA_ALT __attribute__ ((deprecated("Use SDA_ALT1 to match the conventions used in DxCore"))) = PIN_WIRE_SDA_PINSWAP_1; /* deprecated */
  static const uint8_t SCL_ALT __attribute__ ((deprecated("Use SCL_ALT1 to match the conventions used in DxCore"))) = PIN_WIRE_SCL_PINSWAP_1; /* deprecated */
  static const uint8_t SDA_ALT1 = PIN_WIRE_SCL_PINSWAP_1;
  static const uint8_t SCL_ALT1 = PIN_WIRE_SCL_PINSWAP_1;
#endif
static const uint8_t SDA = PIN_WIRE_SDA;
static const uint8_t SCL = PIN_WIRE_SCL;


#ifdef PIN_SPI_SCK_PINSWAP_1
  #define SS_NOW    ((uint8_t) (PORTMUX.CTRLB & PORTMUX_SPI0_bm ? PIN_SPI_SS_PINSWAP_1    : PIN_SPI_SS))
  #define MOSI_NOW  ((uint8_t) (PORTMUX.CTRLB & PORTMUX_SPI0_bm ? PIN_SPI_MOSI_PINSWAP_1  : PIN_SPI_MOSI))
  #define MISO_NOW  ((uint8_t) (PORTMUX.CTRLB & PORTMUX_SPI0_bm ? PIN_SPI_MISO_PINSWAP_1  : PIN_SPI_MISO))
  #define SCK_NOW   ((uint8_t) (PORTMUX.CTRLB & PORTMUX_SPI0_bm ? PIN_SPI_SCK_PINSWAP_1   : PIN_SPI_SCK))
  static const uint8_t SS_ALT    __attribute__ ((deprecated("Use SS_ALT1 to match the conventions used in DxCore"  ))) = PIN_SPI_SS_PINSWAP_1; /* deprecated */
  static const uint8_t MOSI_ALT  __attribute__ ((deprecated("Use MOSI_ALT1 to match the conventions used in DxCore"))) = PIN_SPI_MOSI_PINSWAP_1; /* deprecated */
  static const uint8_t MISO_ALT  __attribute__ ((deprecated("Use MISO_ALT1 to match the conventions used in DxCore"))) = PIN_SPI_MISO_PINSWAP_1; /* deprecated */
  static const uint8_t SCK_ALT   __attribute__ ((deprecated("Use SCK_ALT1 to match the conventions used in DxCore" ))) = PIN_SPI_SCK_PINSWAP_1; /* deprecated */
  static const uint8_t SS_ALT1   = PIN_SPI_SS_PINSWAP_1;
  static const uint8_t MOSI_ALT1 = PIN_SPI_MOSI_PINSWAP_1;
  static const uint8_t MISO_ALT1 = PIN_SPI_MISO_PINSWAP_1;
  static const uint8_t SCK_ALT1  = PIN_SPI_SCK_PINSWAP_1;
#endif
static const uint8_t SS   = PIN_SPI_SS;
static const uint8_t MOSI = PIN_SPI_MOSI;
static const uint8_t MISO = PIN_SPI_MISO;
static const uint8_t SCK  = PIN_SPI_SCK;



#define CORE_HAS_FASTIO 2
#define CORE_HAS_OPENDRAIN 1
#define CORE_HAS_PINCONFIG 1
#define CORE_HAS_FASTPINMODE 1


#if (MEGATINYCORE_SERIES == 2)
  // if analogReadEnh() supplied, this is defined as 1
  #define CORE_HAS_ANALOG_ENH 1
  // if analogReadDiff() supplied, this is defined as 1
  #define CORE_HAS_ANALOG_DIFF 1
#else
  #define CORE_HAS_ANALOG_ENH 1
  // if analogReadDiff() supplied, this is defined as 1
  #define CORE_HAS_ANALOG_DIFF 0
#endif


#ifndef SUPPORT_LONG_TONES
  #if (PROGMEM_SIZE > 8192)
    #define SUPPORT_LONG_TONES 1
  #endif
#endif


#endif
