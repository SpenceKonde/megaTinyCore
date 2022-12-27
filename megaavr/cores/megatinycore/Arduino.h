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
 * of stuff nobody should have to read.=
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

/* we call badArg() when we know at compile time that one or more arguments passed to
 * the function is nonsensical or doomed to generate useless output. For example,
 * analogWrite() on a constant pin that will never support PWM, or digitalWrite() on
 * a pin number that is neither a pin nor NOT_A_PIN (which silently does nothing for
 * compatibility).
 * badCall() on the other hand is called if we know that regardless of what arguments
 * are passed, that function is nonsensical with current settings, for example, millis()
 * when millis timekeeping has been disabled */

#if !defined(LTODISABLED)
  void badArg(const char*)    __attribute__((error("")));
  void badCall(const char*)   __attribute__((error("")));
  // The fast digital I/O functions only work when the pin is known at compile time.
  inline __attribute__((always_inline)) void check_constant_pin(pin_size_t pin)
  {
    if(!__builtin_constant_p(pin))
      badArg("Fast digital pin must be a constant");
  }
#else
  void badArg(const char*);
  void badCall(const char*);
  void check_constant_pin(pin_size_t pin);
  #if defined(ARDUINO_MAIN) // We need to make sure these substitutes for the badArg and badCall functions are generated once and only once.
    // They must not attempt to actully detect any error if LTO is disabled
    void badArg(__attribute__((unused))const char* c) {
      return;
    }
    void badCall(__attribute__((unused))const char* c) {
      return;
    }
    void check_constant_pin(__attribute__((unused))pin_size_t pin) {
      return;
    }
  #endif // Intentionally outside of the above #if so that your console gets fucking spammed with this warning.
  // The linker errors you turned off LTO to better understand will still be at the bottom.
  #warning "LTO is disabled. digitalWriteFast(), digitalReadFast(), pinModeFast() and openDrainFast() are unavailable, delayMicroseconds() for short delays and delay() with millis timing disabled is less accuratetest. Unsupported forms of 'new' compile without errors (but always return a NULL pointer). Additionally, functions which normally generate a compile error when passed a value that is known to be invalid at compile time will not do so. The same is true of functions which are not valid with the currently selected tools submenu options."
  #warning "This mode is ONLY for debugging LINK-TIME ERRORS that are reported by the linker as being located at .text+0, and you can't figure out where the bug is from other information it provides. As noted above, while this may make compilation succeed, it will only turn compile-time errors into incorrect runtime behavior, which is much harder to debug. As soon as the bug that forced this to be used is fixed, switch back to the standard platform.txt!"
  #warning "UART implementation is forcibly downgraded, Flash.h writes are replaced with NOPs, and pin interrupts are downgraded to the old (less efficient) implementation. All uploading is disabled, and behavior of exported binaries may vary from normal behavior arbitrarily."
#endif

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
#if !defined(LTODISABLED)
  int8_t   digitalReadFast(uint8_t pinNumber               );
  void    digitalWriteFast(uint8_t pinNumber,   uint8_t val);
  void         pinModeFast(uint8_t pinNumber,  uint8_t mode);
  void       openDrainFast(uint8_t pinNumber,   uint8_t val);
#elif defined(FAKE_FAST_IO)
  // Should be enabled if, and only if, you are debugging something that you can't debug with LTO enabled, AND
  // your code makes use of the fast functions. Note that this drastically alters behavior of code that calls them, taking, in some cases, two orders of magnitude longer
  #warning "FAKE_FAST_IO code should **never** be expected to work correctly running on hardware. It is just here to provide a way to get past missing definition errors when you are forced to disable LTO for debugging."
  #define      digitalRead(uint8_t pinNumber               );
  #define     digitalWrite(uint8_t pinNumber,   uint8_t val);
  #define          pinMode(uint8_t pinNumber,  uint8_t mode);
  #define        openDrain(uint8_t pinNumber,   uint8_t val);
  #endif
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
  #define _SWAP(n) __asm__ __volatile__ ("swap %0"  "\n\t" :"+r"((uint8_t)(n)))
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

//
#define _ADDLOW(a,b) __asm__ __volatile__ ("add %0A, %1"  "\n\t" :"+r"((uint16_t)(a)):"r"((uint8_t) b))
#define _SUBLOW(a,b) __asm__ __volatile__ ("sub %0A, %1"  "\n\t" :"+r"((uint16_t)(a)):"r"((uint8_t) b))
/* This is dirty trick that has benefit when a  16-bit value is a, and an 8-bit value b is added to it
 * and it is known that under all circumstances, this will absolutely never cause a carry
 * (that is, will never change the high byte), butthe compiler does not realize that this is the case,
 * this dirty macro directly adds (or subtracts) an 8 bit value to (or from) a 16-bit value, with no
 * provision for carrying. That is it will only impact the low byte. You are probably wondering why this
 * is such a common situation that we care about saving one clock cycle. The reason is that is is incredibly
 * common when working with SFRs for onchip peripherals. The code may not know which instance of a peripheral
 * it will be passed, but the author does know that the value is 32 or 64 or 128-byte aligned.
 * Now if the offset is constant and less than 64 and you don't change the pointer, it's usually better to
 * just do this:
 *
 * uint8_t foo = *(base_pointer + 17);
 *
 * at least in the absence of enough register pressure that it resorts to using the X register, which imposes a 4 clock 2 byte penalty.
 * but if the value is not constant,
 *
 * base_pointer += bar; // where bar is a uint8_t such that (base_poimter & 0cFF) + bar < 0x100.
 *
 * takes 2 clocks for the math. But if for example, you know that base_pointer is 32-aligned, and bar will always be between oh,
 * 0x10 and 0x17, you would know that you could always safely add bar, whatever value in that restricted range it is, to base_pointer
 * and it would only take 1 clock.
 * This can be roughly translated to "whenever you are working with any peripheral by getting a pointer to it, but the offset you want
 * bounded but variable, For example, you get the port base register, amd now you need the PIMnCTRL register. n is 0-7, and the
 * PINnCTRL registers are located at +0x10 to +0x17, and ports are 32-bit aligned, so adding that to the register will never change the high
 * byte of the address. These are sometimes used when a somewhat time-sensitive function which is called very frequently has to performs
 * such manipulations. Don't use unless you ae damned certain that you meet the conditions, and the function is called constantly
 * Mostly intended for internal use
 */
#define _MAKEPTR_DISPLACE(newptt, highbyte, lowbyte) ((uint8_t*) (__asm__ __volitile("ldi %0B, %1 " "\n\t" "mov %0A, %2 " "\n\t":"+b"((uint16_t) newptr):"M" ((uint8_t) highbyte), "r" ((uint8_t ), lowbyte))
#define          _MAKEPTR(newptt, highbyte, lowbyte) ((uint8_t*) (__asm__ __volitile("ldi %0B, %1 " "\n\t" "mov %0A, %2 " "\n\t":"+e"((uint16_t) newptr):"M" ((uint8_t) highbyte), "r" ((uint8_t ), lowbyte))

/* Very similar to the above. Passed a poimter as two bytes, the high byte constant and the low byte not, this finds use in the same sort
 * of situatios/ See the I/O headers: each class of peripherals often has the same high byte for all addresses. Ports (0x0400, 0x0420, 0x0440
 * and so on. The most freqently used functions get speciakl attention paid to this as a small gain adds up for the most commonly called functions
 * usage is typically sdmething like
 *    lowbyte = (_SWAP(usartnbr)); // passed from,elsewhre io the code, which must be free of bugs!
 *    lowbyte <<= 1;(
 *    volatile uint8_t *myptr l
 *    _MAKEPTR_DISPLACE(myptr. 0x08, lowbyte)
 * Now, myptr points to the start of the specified USART, in just 2 clock cycles, instead of the typical 4 the compiler creates.
 * most helful under the most constrained conditions (in an ISR, or a core function that gets called right and left.
 *
 * _MAKEPTR should be used when you are only reading from that one address, or consecutive addresses starting there, so it can use the X, Y or Z register
 * _MAKEPTR_DISPLACE uses only X or Y register, and should be used if you expect to be accessing registers with an address up to 63 higher and in
 * consecutive order, This will usually be enough to convince the compiler to not then move it into a different pointer.
 *
 */
#define      _SETHIGH(a,b) __asm__ __volatile__ ("mov %0B,  %1"  "\n\t" :"+r"((uint16_t)(a)):"r"((uint8_t) b))
#define _SETHIGHCONST(a,b) __asm__ __volatile__ ("ldi %0B,  %1"  "\n\t" :"+d"((uint16_t)(a)):"M"((uint8_t) b))
#define        _CLRHIGH(a) __asm__ __volatile__ ("eor %0B, %0B"  "\n\t" :"+r"((uint16_t)(a)))
/* More dumb macros to allow surgery on half of a16-bit (likely poimter) value in minimum time.
 * Clear (set to 0), setting to, setting to an existing (preferably local and already in a register) variable value
 * and
 * _SETHIGH(a,b) sets the high byte of 16-bit local variable to the variable b. This makes no guarantees about it being able to be used as a pointer
 * _SETHIGHCONST(a,b) As above, but b is a compile time known constant, rather than a previously calculated result
 */

#define      _SETLOW(a,b) __asm__ __volatile__ ("mov %0A,  %1"  "\n\t" :"+r"((uint16_t)(a)):"r"((uint8_t) b))
#define _SETLOWCONST(a,b) __asm__ __volatile__ ("ldi %0A,  %1"  "\n\t" :"+d"((uint16_t)(a)):"M"((uint8_t) b))
#define        _CLRLOW(a) __asm__ __volatile__ ("eor %0A, %0A"  "\n\t" :"+r"((uint16_t)(a)))
/* As above, except for low byte */

//
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
/* Timers and Timer-like-things
 * These are used for two things: Identifying the timer on a pin in
 * digitalPinToTimer(), and for the MILLIS_TIMER define that users can test to
 * verify which timer is being used for millis.
 *
 * Prior to 1.3.x TCAs were all 0x1_, TCBs 0x2_. But in order to make the
 * take-over tracking work efficiently I needed a dedicated bit for each TCA.
 * so that we can just do (__PeripheralControl | TIMERA0) to test if user has
 * taken over the timer. Hence, all the "big" timers (those which have a
 * takeOverTCxn() function and which PORTMUX moves en masse instead of one at
 * a time) have their own bit within these macros.
 * Note also that the digital_pin_to_timer table doesn't list these anymore.\
 * There are two functions within the core that need to know this:
 * AnalogWrite, and turnOffPWM. These each carry their own implementation of
 * logic to identify the timer and channel; the only other cases in which these
 * pins need to be identified are within user code, where the pin mapping can
 * be chosen freely (subject to user code needing access to other pins), so
 * it does not present the complexity of the core library which must work with
 * the pins in ANY of 7, 2, or 5 mappings (for TCA0, TCA1 and TCD0 respectively)
 *
 * Prior to this change, only the arbitrarily chosen core default timer pin-
 * mapping was supported, so this was a dramatic leap forward in capabilitt.
 *
 * The DAC is listed here because analogWrite() uses it almost exactly like
 * a PWM timer.
 * RTC can be used as a millis timekeeping source (well, not currently on
 * DxCore, but it can on megaTinyCore, and might one day be possible on DxCore
 * though I'm not sure I want to implement it that way).
 *****************************************************************************/
/* More may be implemented here in the future */

#define NOT_ON_TIMER    (0x00)
#define TIMERA0         (0x10) // A "simple" type A timer mapping doesn't get constants for the WO channels.
#define TIMERA1         (0x08) // Formerly 0x11 - giving it a dedicated bit makes the takeover tracking easy and efficient instead of being a morass of tests and bitmath.
#define TIMERB0         (0x20) // TCB0
#define TIMERB1         (0x21) // TCB1
#define TIMERB2         (0x22) // TCB2
#define TIMERB3         (0x23) // TCB3
#define TIMERB4         (0x24) // TCB4
#define TIMERD0         (0x40) // If any of these bits match it's potentially on TCD0
#define DACOUT          (0x80) /// If the high bit is set, it;s either the DAC oone of the new timers.
#define TIMERE0         (0x90) //  Full dettails not known, the below constants are guesses
#define TIMERF0         (0xC0)  // as abpve
#define TIMERE1         (0xA0)
#define TIMERF1         (0xD0)
/* The above are all used in the digitalPinToTimer() macro and appear in the timer table, in addition to being how we identify millis timer.
 * For the millis timer, there's nothing weird here.
 * But the timer table constants contain more information than that for these. When user code interprets the timer table entries it is critical to do it right:
 *  1. If 0x40 is set, TCD0 can output here. bits 4 and 5 contain information on what channel, and bits 0-2 specify what the PORTMUX must be set to.
 *  2. If 0x20 us set, there is a TCB can output PWM there.
 *    2a. If 0x20 is set, check 0x10 - if that's set, it's the alt pin mapping. This is currently not returned by the table, and I assess it to be unlikely to be of use
 *  4. If 0x10 is set, it's a TCA0 pin. This is never used in the timer table, but digitalPinToTimerNow() can return it.
 *  5. If 0x08 is set, it's a TCA1 pin. This is never used in the timer table, but digitalPinToTimerNow() can return it.
 * Ergo, use bitwise ands
 */

#define TIMERRTC        (0x90) // RTC with internal osc
#define TIMERRTC_XTAL   (0x91) // RTC with crystal
#define TIMERRTC_CLK    (0x92) // RTC with ext clock

/* Not used in table */
#define TIMERA0_MUX0    (0x10) // Mapping0 (PORTA 0-5)
#define TIMERA0_MUX1    (0x11) // Mapping1 (PORTB 0-5)
#define TIMERA0_MUX2    (0x12) // Mapping2 (PORTC 0-5)
#define TIMERA0_MUX3    (0x13) // Mapping3 (PORTD 0-5)
#define TIMERA0_MUX4    (0x14) // Mapping4 (PORTE 0-5)
#define TIMERA0_MUX5    (0x15) // Mapping5 (PORTF 0-5)
#define TIMERA0_MUX6    (0x16) // Mapping6 (PORTG 0-5)
#define TIMERA0_MUX7    (0x17) // Mapping7 (PORTA 0-5)
#define TIMERA1_MUX0    (0x08) // Mapping0 (PORTB 0-5) - 48+ pin only.
#define TIMERA1_MUX1    (0x09) // Mapping1 (PORTC 4-6) - only three channels available. 48+ pin only.
#define TIMERA1_MUX2    (0x0A) // Mapping2 (PORTE 4-6) - only three channels available. DB-series only due to errata. 64-pin parts only
#define TIMERA1_MUX3    (0x0B) // Mapping3 (PORTG 0-5) - DB-series only due to errata. 64-pin parts only.
#define TIMERA1_MUX4    (0x0C) // Mapping4 (PORTA 4-6) - only three channels available. New on EA-series.
#define TIMERA1_MUX5    (0x0D) // Mapping5 (PORTD 4-6) - only three channels available. New on EA-series.

/* Not used in table or at all, yet */
#define TIMERB0_ALT     (0x30) // TCB0 with alternate pin mapping - DANGER: NOT YET USED BY CORE.
#define TIMERB1_ALT     (0x31) // TCB1 with alternate pin mapping - DANGER: NOT YET USED BY CORE.
#define TIMERB2_ALT     (0x32) // TCB2 with alternate pin mapping - DANGER: NOT YET USED BY CORE.
#define TIMERB3_ALT     (0x33) // TCB3 with alternate pin mapping - DANGER: NOT YET USED BY CORE.
#define TIMERB4_ALT     (0x34) // TCB4 with alternate pin mapping - DANGER: NOT YET USED BY CORE.
#define TIMERB0_ALT     (0x30) // TCB0 with alternate pin mapping - DANGER: NOT YET USED BY CORE.
#define TIMERB1_ALT     (0x31) // TCB1 with alternate pin mapping - DANGER: NOT YET USED BY CORE.
#define TIMERB2_ALT     (0x32) // TCB2 with alternate pin mapping - DANGER: NOT YET USED BY CORE.
#define TIMERB3_ALT     (0x33) // TCB3 with alternate pin mapping - DANGER: NOT YET USED BY CORE.
#define TIMERB4_ALT     (0x34) // TCB4 with alternate pin mapping - DANGER: NOT YET USED BY CORE.


// 0b01MC 0mmm - the 3 lowest bits refer to the PORTMUX.
//                            bit C specifies whether it's channel A (0) or B (1). If M is 1 it is WOC outputting chan A or WOB outputting D.
//                            WOD outputting A or WOC outputting B is not supported by the core. WOB outputting A or WOA outputting B is not supported by the hardware.
//                            Hence, PORTMUX.TCDROUTEA == (timer table entry) & (0x07)
//                            and any table entry > 0x40 but less than 0x80 could be a TCD
/*
#define TIMERD0_0WOA      (0x40) // PORTA
#define TIMERD0_0WOB      (0x50)
#define TIMERD0_0WOC      (0x60)
#define TIMERD0_0WOD      (0x70)
#define TIMERD0_1WOA      (0x41) // PORTB
#define TIMERD0_1WOB      (0x51)
#define TIMERD0_1WOC      (0x61)
#define TIMERD0_1WOD      (0x71)
#define TIMERD0_2WOA      (0x42) // PORTF
#define TIMERD0_2WOB      (0x52)
#define TIMERD0_2WOC      (0x62)
#define TIMERD0_2WOD      (0x72)
#define TIMERD0_3WOA      (0x43) // PORTG
#define TIMERD0_3WOB      (0x53)
#define TIMERD0_3WOC      (0x63)
#define TIMERD0_3WOD      (0x73)
#define TIMERD0_4WOA      (0x44) // this is PA4, duplicates mux 0.
#define TIMERD0_4WOB      (0x54) // this is PA5, duplicates mux 0.
#define TIMERD0_4WOC      (0x64) // second half is PORTD
#define TIMERD0_4WOD      (0x74)

// For future use
// If timer D ever gets all 8 options, we'd need these

#define TIMERD0_5WOA      (0x45) // hypothetical TCD0 WOA ALT5
#define TIMERD0_5WOB      (0x55) // hypothetical TCD0 WOB ALT5
#define TIMERD0_5WOC      (0x65) // hypothetical TCD0 WOC ALT5
#define TIMERD0_5WOD      (0x75) // hypothetical TCD0 WOD ALT5
#define TIMERD0_6WOA      (0x46) // hypothetical TCD0 WOA ALT6
#define TIMERD0_6WOB      (0x56) // hypothetical TCD0 WOB ALT6
#define TIMERD0_6WOC      (0x66) // hypothetical TCD0 WOC ALT6
#define TIMERD0_6WOD      (0x76) // hypothetical TCD0 WOD ALT6
#define TIMERD0_7WOA      (0x47) // hypothetical TCD0 WOA ALT7
#define TIMERD0_7WOB      (0x57) // hypothetical TCD0 WOB ALT7
#define TIMERD0_7WOC      (0x67) // hypothetical TCD0 WOC ALT7
#define TIMERD0_7WOD      (0x77) // hypothetical TCD0 WOD ALT7

// Uhoh, EB has a new kind of timer, a TCE which looks a lot like a TCA-PWM-Powerhouse timer, only better.
// We predict that PORTMUX.TCEROUTEA will not give individual pin control, but that it will be much like TCA.
// and we will thus be able to quickly detect if the port it's poimted at is ours.

#define TIMERE0_MUX0      (0x90) // HypotheticalTCE/WEX mux
#define TIMERE0_MUX1      (0x91) // HypotheticalTCE/WEX mux
#define TIMERE0_MUX2      (0x92) // HypotheticalTCE/WEX mux
#define TIMERE0_MUX3      (0x93) // HypotheticalTCE/WEX mux
#define TIMERE0_MUX4      (0x94) // HypotheticalTCE/WEX mux
#define TIMERE0_MUX5      (0x95) // HypotheticalTCE/WEX mux
#define TIMERE0_MUX6      (0x96) // HypotheticalTCE/WEX mux
#define TIMERE0_MUX7      (0x97) // HypotheticalTCE/WEX mux

// They might make a chip with 2 of them!
#define TIMERE1_MUX0      (0xA0) // HypotheticalTCE/WEX mux
#define TIMERE1_MUX1      (0xA1) // HypotheticalTCE/WEX mux
#define TIMERE1_MUX2      (0xA2) // HypotheticalTCE/WEX mux
#define TIMERE1_MUX3      (0xA3) // HypotheticalTCE/WEX mux
#define TIMERE1_MUX4      (0xA4) // HypotheticalTCE/WEX mux
#define TIMERE1_MUX5      (0xA5) // HypotheticalTCE/WEX mux
#define TIMERE1_MUX6      (0xA6) // HypotheticalTCE/WEX mux
#define TIMERE1_MUX7      (0xA7) // HypotheticalTCE/WEX mux


// They might make a chip with 3 of them!
#define TIMERE2_MUX0      (0xB0) // HypotheticalTCE/WEX mux
#define TIMERE2_MUX1      (0xB1) // HypotheticalTCE/WEX mux
#define TIMERE2_MUX2      (0xB2) // HypotheticalTCE/WEX mux
#define TIMERE2_MUX3      (0xB3) // HypotheticalTCE/WEX mux
#define TIMERE2_MUX4      (0xB4) // HypotheticalTCE/WEX mux
#define TIMERE2_MUX5      (0xB5) // HypotheticalTCE/WEX mux
#define TIMERE2_MUX6      (0xB6) // HypotheticalTCE/WEX mux
#define TIMERE2_MUX7      (0xB7) // HypotheticalTCE/WEX mux

// Plus this wacky TCF thing.
// Premering on low pincount parts, it's hard to say what the full lineup of pin options will be likek
// I predict... 3 bits for the mux position, and that a larger chip might have 2....

#define TIMERF0_MUX0A      (0xC0) // Hypothetical TCF MUX
#define TIMERF0_MUX0B      (0xC8) // Hypothetical TCF MUX
#define TIMERF0_MUX1A      (0xC1) // Hypothetical TCF MUX
#define TIMERF0_MUX1B      (0xC9) // Hypothetical TCF MUX
#define TIMERF0_MUX2A      (0xC2) // Hypothetical TCF MUX
#define TIMERF0_MUX2B      (0xCA) // Hypothetical TCF MUX
#define TIMERF0_MUX3A      (0xC3) // Hypothetical TCF MUX
#define TIMERF0_MUX3B      (0xCB) // Hypothetical TCF MUX
#define TIMERF0_MUX4A      (0xC4) // Hypothetical TCF MUX
#define TIMERF0_MUX4B      (0xCC) // Hypothetical TCF MUX
#define TIMERF0_MUX5A      (0xC5) // Hypothetical TCF MUX
#define TIMERF0_MUX5B      (0xCD) // Hypothetical TCF MUX
#define TIMERF0_MUX6A      (0xC6) // Hypothetical TCF MUX
#define TIMERF0_MUX6B      (0xCE) // Hypothetical TCF MUX
#define TIMERF0_MUX7A      (0xC7) // Hypothetical TCF MUX
#define TIMERF0_MUX7B      (0xCF) // Hypothetical TCF MUX

// What if a chip has two of them? No problem!
#define TIMERF1_MUX0A      (0xD0) // Hypothetical TCF MUX
#define TIMERF1_MUX0B      (0xD8) // Hypothetical TCF MUX
#define TIMERF1_MUX1A      (0xD1) // Hypothetical TCF MUX
#define TIMERF1_MUX1B      (0xD9) // Hypothetical TCF MUX
#define TIMERF1_MUX2A      (0xD2) // Hypothetical TCF MUX
#define TIMERF1_MUX2B      (0xDA) // Hypothetical TCF MUX
#define TIMERF1_MUX3A      (0xD3) // Hypothetical TCF MUX
#define TIMERF1_MUX3B      (0xDB) // Hypothetical TCF MUX
#define TIMERF1_MUX4A      (0xD4) // Hypothetical TCF MUX
#define TIMERF1_MUX4B      (0xDC) // Hypothetical TCF MUX
#define TIMERF1_MUX5A      (0xD5) // Hypothetical TCF MUX
#define TIMERF1_MUX5B      (0xDD) // Hypothetical TCF MUX
#define TIMERF1_MUX6A      (0xD6) // Hypothetical TCF MUX
#define TIMERF1_MUX6B      (0xDE) // Hypothetical TCF MUX
#define TIMERF1_MUX7A      (0xD7) // Hypothetical TCF MUX
#define TIMERF1_MUX7B      (0xDF) // Hypothetical TCF MUX
*/


__attribute__ ((noinline)) void _delayMicroseconds(unsigned int us);


// Get the bit location within the hardware port of the given virtual pin.
// This comes from the arrays in the  pins_arduino.c file for the active
// board configuration.
// These perform slightly better as macros compared to inline functions

extern const uint8_t digital_pin_to_port[];
extern const uint8_t digital_pin_to_bit_mask[];
extern const uint8_t digital_pin_to_bit_position[];
extern const uint8_t digital_pin_to_timer[];



/*
Supplied by Variant file:
#define digitalPinToAnalogInput(p)      // Given digital pin (p), returns the analog channel number, or NOT_A_PIN if the pin does not suipport analog input.
#define analogChannelToDigitalPin(p)    // Inverse of above. Given analog chanbel number (p) in raw form not ADC_CH() form, returns the digital pin number corresponding to it.
#define analogInputToDigitalPin(p)      // Similar to previous. Given analog input number (p) with the high bit set, returns the digital pin number corresponding to it)
#define digitalOrAnalogPinToDigital(p)  // Given either an analog input number (with high bit set) or a digital pin number (without it set), returns the digital pin number.
Yes, these are poorky named and do not use analog input, analog pin, and analog channel consistently. Unfortunately the names of some of these were set in stone by virtue of their being preexisting macros used in code in the wild.
See Ref_Analog.md for more information of the representations of "analog pins". I blame Arduino for the original sin of "analog pins" as a concept in the first place.
*/

#include "pins_arduino.h"
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
  uint8_t _setRTCEventChan(uint8_t val, uint8_t chan);
  uint8_t _setEventPin(uint8_t pin, uint8_t number); // preliminary thought - pass a pin number, it looks up port, and from there the event control register and sets it.
  //Number being 0 or 1 or 255 to pick the lowest numbered one not set. Returns event channel number TBD if that should be the EVSYS valus or 0 or 1. If "Pick unused ome" is requested but both already assigned, will return 255
  uint8_t _getPortEventConfig(uint8_t port); // just shorthand for looking up the port and returning it's EVGENCTRL value
  uint8_t _setRTCEventChan(uint8_t vail, uint8_t chan); // number is 0, 1 or 255 like above, div is log(2) of the divisor (ie, for 2^5, his would be 5).
  uint8_t _getRTCEventConfig(); //simply returns the RTC channel configuration. Will likely return 255 if called on non Ex
  uint8_t _RTCPrescaleToVal(uint16_t prescale)
#endif
#ifdef __cplusplus
} // extern "C"
#endif
/********************************************************************************
 * CORE AND HARDWARE FEATURE SUPPORT  *
 * CORE_HAS_FASTIO is 1 when digitalReadFast and digitalWriteFast are supplied, and 2 when openDrainFast and pinModeFast are as well.
 * CORE_HAS_OPENDRAIN
 * CORE_HAS_PINCONFIG is 1 if pinConfig is supplied. The allows full configuration of any pin. It is 2 if it accepts commas instead of bitwise or between configuration parameters (NYI)
 * CORE_HAS_FASTPINMODE is 1 if pinModeFast is supplied
 * CORE_HAS_ANALOG_ENH is 0 if no analogReadEnh is supplied, 1 if it is, and 2 if it is supplied and both core and hardware support a PGA.
 * CORE_HAS_ANALOG_DIFF is 0 if no analogReadDiff is supplied, 1 if it's DX-like (Vin < VREF), and 2 if it's a proper
 * differential ADC, supported in both hardware and software. The value -1 is also valid and indicates it's a classic AVR with a  * differential ADC, see the documentation for the core.
 * CORE_HAS_TIMER_TAKEOVER is 1 if takeOverTCxn functions are provided to tell the core not to automatically use all
 * type A and D timers.
 * CORE_HAS_TIMER_RESUME is 1 if resumeTCAn functions are provided to hand control back to the core and reinitialize them.
 * CORE_DETECTS_TCD_PORTMUX is 1 if the TCD portmux works correctly on the hardware and is used by the core, 0 if it would be if
 * the hardware worked, and not defined at all if the hardware doesn't have such a feature even in theory
 * CORE_SUPPORT_LONG_TONES is 1 if the core supports the three argument tone for unreasonably long tones.
 ********************************************************************************/
#define CORE_HAS_FASTIO                 (2)
#define CORE_HAS_OPENDRAIN              (1)
#define CORE_HAS_PINCONFIG              (1)
#define CORE_HAS_FASTPINMODE            (1)

#define CORE_HAS_TIMER_TAKEOVER         (1)
#define CORE_HAS_TIMER_RESUME           (0)
#if (MEGATINYCORE_SERIES == 2)
  #define CORE_HAS_ANALOG_ENH           (2)
  #define CORE_HAS_ANALOG_DIFF          (2)
#else
  #define CORE_HAS_ANALOG_ENH           (1)
  #define CORE_HAS_ANALOG_DIFF          (0)
#endif

#ifndef CORE_SUPPORT_LONG_TONES
  #if (PROGMEM_SIZE > 8192)
    #define CORE_SUPPORT_LONG_TONES 1
  #endif
#endif

/* Hardware capabilities (ADC)
 * ADC_DIFFERENTIAL is 1 for a half-way differential ADC like DX-serie has, 2 for a real one like EA-series will    *
 * ADC_MAX_OVERSAMPLED_RESOLUTION is the maximum resolution attainable by oversampling and decimation               *
 * ADC_NATIVE_RESOLUTION is the higher of the two native ADC resolutions. Either 10 or 12                           *
 * ADC_NATIVE_RESOLUTION_LOW is the lower of the two native ADC resolutions. Either 8 or 10. Can't be deduced from  *
 * above. All permutations where the "native resolution" is higher are extant somewhere                             *
 * ADC_MAXIMUM_ACCUMULATE is the maximum number of sameples that can be accumulated by the burst accumulation mode  *
 * ADC_MAXIMUM_SAMPDUR is the maximum sample duration value. Refer to the core documentation or datasheet for detail*
 * ADC_RESULT_SIZE is the size (in bits) of the registers that hold the ADC result. V2.0 ADC has 32, others have 16 *
 * ADC_MAXIMUM_GAIN is defined if there is a way to amplify the input to the ADC. If you have to use onchip opamps  *
 * and the chip has them, it's -1, otherwise it is the maximum multiplier
 * ADC_REFERENCE_SET is 1 if the references are the weird ones that tinyAVR 0 and 1 use, and 2 if they are 1.024,   *
 * 2.048, 4.096 and 2.5V like civilized parts */

#if MEGATINYCORE_SERIES != 2
  #define ADC_DIFFERENTIAL                (0)
  #define ADC_MAX_OVERSAMPLED_RESOLUTION (13)
  #define ADC_NATIVE_RESOLUTION          (10)
  #define ADC_NATIVE_RESOLUTION_LOW       (8)
  #define ADC_MAXIMUM_ACCUMULATE         (64)
  #define ADC_MAXIMUM_SAMPDUR          (0x1F)
  #define ADC_RESULT_SIZE                (16)
  #define ADC_REFERENCE_SET               (1)
#else
  #define ADC_DIFFERENTIAL                (2)
  #define ADC_MAX_OVERSAMPLED_RESOLUTION (17)
  #define ADC_NATIVE_RESOLUTION          (12)
  #define ADC_NATIVE_RESOLUTION_LOW       (8)
  #define ADC_MAXIMUM_ACCUMULATE       (1024)
  #define ADC_MAXIMUM_SAMPDUR          (0xFF)
  #define ADC_RESULT_SIZE                (32)
  #define ADC_REFERENCE_SET               (2)
  #define ADC_MAXIMUM_GAIN             (16)
#endif

/* Oh and DB/DD can use inlvl to adjust trigger points. */
#define PORT_HAS_INLVL 0

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
#if !defined(NUM_I2C_PINS)
  #define NUM_I2C_PINS                  (2) // per I2C port in use - this number is nonsensical without qualification is is only for compatibility.
#endif
#if !defined(NUM_SPI_PINS)
  #define NUM_SPI_PINS                  (3) // per SPI port in use - this number is nonsensical without qualification is is only for compatibility.
#endif
#if !defined(NUM_TOTAL_FREE_PINS)
  #define NUM_TOTAL_FREE_PINS           (PINS_COUNT - NUM_INTERNALLY_USED_PINS)
#endif
#if !defined(NUM_TOTAL_PINS)
  #define NUM_TOTAL_PINS                (NUM_DIGITAL_PINS) /* Used the same way as NUM_DIGITAL_PINS. so it doesn't mean what it's named  - I didn't make the convention*/
#endif

inline __attribute__((always_inline)) void check_valid_digital_pin(pin_size_t pin) {
  if (__builtin_constant_p(pin)) {
    if (pin >= NUM_TOTAL_PINS && pin != NOT_A_PIN) {
    // Exception made for NOT_A_PIN - code exists which relies on being able to pass this and have nothing happen.
    // While IMO very poor coding practice, these checks aren't here to prevent lazy programmers from intentionally
    // taking shortcuts we disapprove of, but to call out things that are virtually guaranteed to be a bug.
    // Passing -1/255/NOT_A_PIN to the digital I/O functions is most likely intentional.
      badArg("Digital pin is constant, but not a valid pin");
    }
  #if (CLOCK_SOURCE == 2)
    #if defined(MEGATINYCORE)
      if (pin == PIN_PA3) {
        badArg("Constant digital pin PIN_PA3 is used for the external osc, and is not available for other uses.");
      }
    #else
      if (pin == PIN_PA0) {
        badArg("Constant digital pin PIN_PA0 is used for the external osc, and is not available for other uses.");
      }
    #endif
  #elif CLOCK_SOURCE == 1
    if (pin < 2) {
      badArg("Pin PA0 and PA1 cannot be used for digital I/O because those are used for external crystal clock.");
    }
  #elif defined(XTAL_PINS_HARDWIRED)
    if (pin < 2) {
      badArg("On the selected board, PA0 and PA1 are hardwired to the crystal. They may not be used for other purposes.");
    }
  #endif
  }
}
/*******************************************************************
 * PIN CONFIGURE Set any or all pin settings, easily invert, etc   *
 *******************************************************************
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
// Microchip can add one more binary option >.>                    */


/* External defintitions */
/* Actual implementation is in wiring_extra.c (or .cpp, if I find that I'm not able tomake it work with .c)
 * Because of the incrutable rules of C++ scoping, you can define an inline function or a template function in a header....
 * and not in the body of a separate file, while the opposite is true for ANY OTHER KIND OF FUNCTION. */

void __pinconfigure(const uint8_t digital_pin, uint16_t pin_config);
void _pinconfigure(uint8_t pin, uint16_t pin_config);
void pinConfigure(uint8_t digital_pin, uint16_t pin_config);

#ifdef __cplusplus
typedef enum : uint16_t
{
 // OUTPUT
  PIN_DIR_SET        = 0x0001,
  PIN_DIRSET         = 0x0001,
  PIN_DIR_OUTPUT     = 0x0001,
  PIN_DIR_OUT        = 0x0001,
 // INPUT
  PIN_DIR_CLR        = 0x0002,
  PIN_DIRCLR         = 0x0002,
  PIN_DIR_INPUT      = 0x0002,
  PIN_DIR_IN         = 0x0002,
 // TOGGLE INPUT/OUTPUT
  PIN_DIR_TGL        = 0x0003,
  PIN_DIRTGL         = 0x0003,
  PIN_DIR_TOGGLE     = 0x0003,
 // HIGH
  PIN_OUT_SET        = 0x0004,
  PIN_OUTSET         = 0x0004,
  PIN_OUT_HIGH       = 0x0004,
 // LOW
  PIN_OUT_CLR        = 0x0008,
  PIN_OUTCLR         = 0x0008,
  PIN_OUT_LOW        = 0x0008,
// CHANGE/TOGGLE
  PIN_OUT_TGL        = 0x000C,
  PIN_OUTTGL         = 0x000C,
  PIN_OUT_TOGGLE     = 0x000C,
//Interrupt disabled but input buffer enabled
  PIN_ISC_ENABLE     = 0x0080,
  PIN_INPUT_ENABLE   = 0x0080,
 // Interrupt on change
  PIN_ISC_CHANGE     = 0x0090,
  PIN_INT_CHANGE     = 0x0090,
// Interrupt on rising edge
  PIN_ISC_RISE       = 0x00A0,
  PIN_INT_RISE       = 0x00A0,
// Interrupt on falling edge
  PIN_ISC_FALL       = 0x00B0,
  PIN_INT_FALL       = 0x00B0,
// Interrupt and input buffer disabled
  PIN_ISC_DISABLE    = 0x00C0,
  PIN_INPUT_DISABLE  = 0x00C0,
// Interrupt enabled with sense on low level
  PIN_ISC_LEVEL      = 0x00D0,
  PIN_INT_LEVEL      = 0x00D0,
// PULLUP ON
  PIN_PULLUP_ON      = 0x0100,
  PIN_PULLUP         = 0x0100,
  PIN_PULLUP_SET     = 0x0100,
// PULLUP OFF
  PIN_PULLUP_OFF     = 0x0200,
  PIN_PULLUP_CLR     = 0x0200,
// PULLUP TOGGLE
  PIN_PULLUP_TGL     = 0x0300,
  PIN_PULLUP_TOGGLE  = 0x0300,
  PIN_NOPULLUP       = 0x0200,
// Pin Input Level Control
  PIN_INLVL_TTL      = 0x1000,
  PIN_INLVL_ON       = 0x1000,
  PIN_INLVL_SET      = 0x1000,
  PIN_INLVL_SCHMITT  = 0x2000,
  PIN_INLVL_OFF      = 0x2000,
  PIN_INLVL_CLR      = 0x2000, // alias
// PIN INVERT ON
  PIN_INVERT_ON      = 0x4000,
  PIN_INVERT_SET     = 0x4000,
// PIN INVERT OFF
  PIN_INVERT_OFF     = 0x8000,
  PIN_INVERT_CLR     = 0x8000,
// PIN_INVERT_TOGGLE
  PIN_INVERT_TGL     = 0xC000,
  PIN_INVERT_TOGGLE  = 0xC000
} pin_configure_t;

/**
 * @brief Helper functions to catch the last argument in the pincfg recursion loop
 *
 * @param mode Mode parameter
 * @return pin_configure_t
 */


inline pin_configure_t _pincfg(const pin_configure_t mode) {
  return mode;
}

/**
 * @brief Helper functions to catch the nth in the pincfg recursion loop
 *
 * @param digital_pin Arduino pin
 * @param mode First "mode" parameter
 * @param modes Nth "mode" parameter
 * @return uint16_t pin configuration or'ed together
 */
template <typename... MODES>
uint16_t _pincfg(const pin_configure_t mode, const MODES&... modes) {
  return mode | _pincfg(modes...);
}



//void        pinConfigure(const uint8_t pinNumber, const uint16_t mode, const MODES&... modes);

/**
 * @brief Variadic template function for configuring a pin
 *
 * @param digital_pin Arduino pin number
 * @param mode First "mode" parameter
 * @param modes Nth "mode" parameter
 */
template <typename... MODES>
void pinConfigure(const uint8_t digital_pin, const pin_configure_t mode, const MODES&... modes) {
  // Or-ing together the arguments using recursion
  uint16_t pin_config = _pincfg(mode, modes...);
  _pinconfigure(digital_pin, pin_config);
}
#endif // end


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



// A little bit of trickery - this allows Serial to be defined as something other than USART0
// Use case is for boards where the main serial port is not USART0 to be used without the user
// having to find/replace Serial with Serial2 or whatever on their existing code if that's where
// the monitor is. It requires that the board be defined by a variant file
#ifndef Serial
  #define Serial Serial0 // Error here? Check for missing ; previous line in sketch.
#endif

/* Moved from pins_arduino.h to reduce code duplication - also made better decisions */
// These serial port names are intended to allow libraries and architecture-neutral
// sketches to automatically default to the correct port name for a particular type
// of use.  For example, a GPS module would normally connect to SERIAL_PORT_HARDWARE_OPEN,
// the first hardware serial port whose RX/TX pins are not dedicated to another use.
//
// SERIAL_PORT_MONITOR        Port which normally prints to the Arduino Serial Monitor
//
// SERIAL_PORT_USBVIRTUAL     Port which is USB virtual serial
//
// SERIAL_PORT_LINUXBRIDGE    Port which connects to a Linux system via Bridge library
//
// SERIAL_PORT_HARDWARE       Hardware serial port, physical RX & TX pins.
//
// SERIAL_PORT_HARDWARE_OPEN  Hardware serial ports which are open for use.  Their RX & TX
//                            pins are NOT connected to anything by default.

// First, we allow a define to be passed (lkely from boards.txt - though it could
// come from pins_arduino, I suppose) to force a certain port here. The plan is
// that I will pass defines from board definitions specifying this for the Curiosity
// Nano boards, in order to improve the user experience there - though this may be
// obliviated by the Serial/Serial0 thing just above, which has the advantage of
// transparently adapting user code as well.

#if !defined(SERIAL_PORT_MONITOR)
  #if defined(SERIAL_PORT_BOOT)
    #define SERIAL_PORT_MONITOR       SERIAL_PORT_BOOT
  #else
    #define SERIAL_PORT_MONITOR       Serial
  #endif
#endif

// Following example in the Arduino Mega, where despite having 4 serial ports, they
// use Serial for both SERIAL_PORT_MONITOR and SERIAL_PORT_HARDWARE, I will not
// try to spread around the ports we use for these defines - if we're
// going to declare some other port to be the "main" serial port, with the monitor
// on it and all, we should be consistent about that, right? *shrug*
#if !defined(SERIAL_PORT_HARDWARE)
  #define SERIAL_PORT_HARDWARE      SERIAL_PORT_MONITOR
#endif

// If we have USART2 (ie, we are not a DD-series) we will declare that to be
// SERIAL_PORT_HARDWARE_OPEN, so that on a DB-series, libraries are less likely to
// squabble over the more powerful USART1 - USART1 being more powerful because it
// is on PORTC, the MVIO port.
/*
#if !defined(SERIAL_PORT_MONITOR)
  #if defined(USART2) && (SERIAL_PORT_MONITOR != Serial2) && (!defined(SERIAL_PORT_BOOT) || SERIAL_PORT_BOOT != Serial2)
    #define SERIAL_PORT_HARDWARE_OPEN Serial2
  #else
    #if (SERIAL_PORT_MONITOR != Serial0 ) && (!defined(SERIAL_PORT_BOOT) || SERIAL_PORT_BOOT != Serial0)
      #define SERIAL_PORT_HARDWARE_OPEN Serial0
    #else
      #define SERIAL_PORT_HARDWARE_OPEN Serial1
    #endif
  #endif
#endif
#if !defined(SERIAL_PORT_MVIO) && defined(MVIO) // defined on DD snd DB.
// DD-series parts with 20-or-fewer pins will not have a PC0 for the TX line of
// Serial1. that makes it difficult to , so it can't be their MVIO serial port (without involving the event
// system, of course) - but they can get a serial port on MVIO pins with USART0
// and an alternate mapping. So for those parts only, Serial is their MVIO port.
// For everyone else it's Serial1, and for non-DD parts, that is the only serial port connected to the mvio pins.
// Note that if MVIO is disabled, we cannot detect that.
  #if defined(DD_14_PINS) || defined(DD_20_PINS)
    #define SERIAL_PORT_MVIO Serial0
    #define SERIAL_PORT_MVIO_MUX 0x04 // TX PC1 RX PC2
  #else
    #define SERIAL_PORT_MVIO Serial1
  #endif
#endif
*/

// Spence Konde: This is a bit silly - but it does have some utility. I am of the
// opinion that anything that needs to use a serial port or other peripheral of
// which a chip may have several, and it needs to be sure to pick the "right" one
// (that is, most of the time), it should be accepting an argument somewhere to set which serial port rather than assuming.



#endif
