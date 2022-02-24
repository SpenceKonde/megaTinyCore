#ifndef __TIMERS_H__
#define __TIMERS_H__
#include <core_parameters.h>

/*****************************************
|     #   # ### #    #    ###  ###       |
|     ## ##  #  #    #     #  #          |
|     # # #  #  #    #     #   ###       |
|     #   #  #  #    #     #      #      |
|     #   # ### #### #### ### ####       |
|_______________________________________*/

#ifdef MILLIS_USE_TIMERNONE
  #define DISABLE_MILLIS
#endif
#if (defined(MILLIS_USE_TIMERRTC_XTAL) || defined(MILLIS_USE_TIMERRTC_XOSC))
  #define MILLIS_USE_TIMERRTC
#endif


#if (defined(MILLIS_USE_TIMERB0) || defined(MILLIS_USE_TIMERB1) || defined(MILLIS_USE_TIMERB2) || defined(MILLIS_USE_TIMERB3) || defined(MILLIS_USE_TIMERB4))
  #if (F_CPU == 1000000UL)
    #define TIME_TRACKING_TIMER_DIVIDER   (1)
    #define TIME_TRACKING_TIMER_PERIOD    ((F_CPU/500)-1)
  #elif (F_CPU == 2000000UL)
    #define TIME_TRACKING_TIMER_DIVIDER   (2)
    #define TIME_TRACKING_TIMER_PERIOD    ((F_CPU/1000)-1)
  #else
    #define TIME_TRACKING_TIMER_DIVIDER   (2)
    #define TIME_TRACKING_TIMER_PERIOD    ((F_CPU/2000)-1)
  #endif
#elif defined(MILLIS_USE_TIMERD0)
  #define TIME_TRACKING_TIMER_PERIOD      (0x1FD)
  #if (F_CPU == 1000000UL)
    #define TIME_TRACKING_TIMER_DIVIDER   (64)    /* Clock divider for TCD0 */
  #else
    #define TIME_TRACKING_TIMER_DIVIDER   (32)    /* Clock divider for TCD0 */
  #endif
#else // Otherwise it must be a TCA
  #define   TIME_TRACKING_TIMER_PERIOD    (0xFE)
  #if     (F_CPU > 30000000UL)
    #define TIME_TRACKING_TIMER_DIVIDER   (256)
  #elif   (F_CPU <= 1000000UL)
    #define TIME_TRACKING_TIMER_DIVIDER   (8)
  #elif   (F_CPU <= 5000000UL)
    #define TIME_TRACKING_TIMER_DIVIDER   (16)
  #else // 30000000UL >= F_CPU > 5000000
    #define TIME_TRACKING_TIMER_DIVIDER   (64)
  #endif
#endif

#define TIME_TRACKING_TICKS_PER_OVF   (TIME_TRACKING_TIMER_PERIOD   + 1UL)
#define TIME_TRACKING_CYCLES_PER_OVF  (TIME_TRACKING_TICKS_PER_OVF  * TIME_TRACKING_TIMER_DIVIDER)

// For a type B timer as millis, these #defines aren't needed, but they're defined accurately anyway,


#if   defined(MILLIS_USE_TIMERA0)
  #ifndef TCA0
    #error "TCA0, selected for millis, does not exist on this part"
  #endif
  #define MILLIS_TIMER TIMERA0
#elif defined(MILLIS_USE_TIMERA1)
  #ifndef TCA1
    #error "TCA1, selected for millis, does not exist on this part"
  #endif
  #define MILLIS_TIMER TIMERA1
#elif defined(MILLIS_USE_TIMERB0)
  #ifndef TCB0
    #error "TCB0, selected for millis, does not exist on this part"
  #endif
  #define MILLIS_TIMER TIMERB0
#elif defined(MILLIS_USE_TIMERB1)
  #ifndef TCB1
    #error "TCB1, selected for millis, does not exist on this part"
  #endif
  #define MILLIS_TIMER TIMERB1
#elif defined(MILLIS_USE_TIMERB2)
  #ifndef TCB2
    #error "TCB2, selected for millis, does not exist on this part"
  #endif
  #define MILLIS_TIMER TIMERB2
#elif defined(MILLIS_USE_TIMERB3)
  #ifndef TCB3
    #error "TCB3, selected for millis, does not exist on this part"
  #endif
  #define MILLIS_TIMER TIMERB3
#elif defined(MILLIS_USE_TIMERB4)
  #ifndef TCB4
    #error "TCB4, selected for millis, does not exist on this part"
  #endif
  #define MILLIS_TIMER TIMERB4
#elif defined(MILLIS_USE_TIMERD0)
  #ifndef TCD0
    #error "TCD0, selected for millis, does not exist on this part"
  #endif
  #define MILLIS_TIMER TIMERD0
#elif defined(MILLIS_USE_TIMERRTC)
  #ifndef RTC
    #error "RTC, selected for millis, does not exist on this part"
  #endif
  #define MILLIS_TIMER TIMERRTC
#elif defined(MILLIS_USE_TIMERNONE)
  #define MILLIS_TIMER NOT_ON_TIMER
#else
  #error "Millis timer not specified, nor is millis disabled - can't happen!"
#endif
#endif

/* These allow other libraries to detect whether timing functions are available with #ifdef */
#define delay delay
#define delayMicroseconds delayMicroseconds

#if !defined(MILLIS_USE_TIMERNONE)
  #define millis millis
  #if !defined(MILLIS_USE_TIMERRTC)
    #define micros micros
  #endif
#endif

/****************************************
|           ####  #     # #   #         |
|           #   # #     # ## ##         |
|           ####  #  #  # # # #         |
|           #      # # #  #   #         |
|           #       # #   #   #         |
|______________________________________*/


/* TYPE-A TIMERS */

#define PWM_TIMER_PERIOD  (0xFE)  // For frequency
#define PWM_TIMER_COMPARE (0x00) // For duty cycle - this is never used.
/* The original implementation set the compare registers (all 6 of them, with an STS instruction),
 * and also set a number of other TCA registers to their POR values. That was dumb, and is no longer done.
 * TCA0 is present on all parts and always used for PWM.
 * TCA1 is used for PWM on Dx-series parts that have it.
 */

/* TYPE-B TIMERS */
#if defined(TCB2) && !defined(MEGATINYCORE)
  // On tinyAVR, and the low pincount DD's TCB PWM isn't helpful and the timer is too valuable anyway.
  // These use the same values as above for TCA, only they're only 1 channel each.
  #define USE_TCB_PWM
  #if defined(SUPPORT_TCB_MUX)
    #undef SUPPORT_TCB_MUX
    #warning "TCB autoMUXing is not yet supported"
  #endif
  /* If this feature is ever implemented,  calling analogWrite() on a pin that has PWM controlled by a TCB
   * it will automatically also set or clear the PORTMUX bit to point the TCB at the right place.
   * These pins will be included in the pin timer table.
   */
#endif



/* TYPE-D TIMERS */
#if defined(MEGATINYCORE)
  /* TCD0 on megaTinyCore is run from the unprescaled clock, at either 20 or 16 MHz depending on the fuse settign.
   * It will always use a count prescale of 32. If the system clock is cranked all the way back to 1 MHz, though
   * we knock it down by another factor of 2 because otherwise the interrupts were taking a large fraction of
   * available CPU time. */
  #if (defined(MILLIS_USE_TIMERD0) && F_CPU == 1000000)
    #define TIMERD0_PRESCALER (TCD_CLKSEL_20MHZ_gc | TCD_CNTPRES_DIV32_gc | TCD_SYNCPRES_DIV2_gc)
    // TODO: change to TIMERD0_CLOCK_SETTING here and elsewhere
    // along with TIMERD0_TOP_SETTING and TIMERD0_WGMODE_SETTING
  #else
    #define TIMERD0_PRESCALER (TCD_CLKSEL_20MHZ_gc | TCD_CNTPRES_DIV32_gc)
  #endif
  // Pretty simple here!
#elif defined(DXCORE)
  #if defined(TIMERD0_TOP_SETTING)
   #if (TIMERD0_TOP_SETTING == 254 || TIMERD0_TOP_SETTING == 509 ||TIMERD0_TOP_SETTING == 1019 ||TIMERD0_TOP_SETTING == 2039 ||TIMERD0_TOP_SETTING == 4079 )
      #if !defined(USE_TIMERD0_PWM)
        #define USE_TIMERD0_PWM 1
      #endif
      #define TIMERD0_TOP_FIXED
    #else // TOP setting defined, but not a supported value
      #if defined(USE_TIMERD0_PWM) && USE_TIMERD0_PWM == 1
        // If they are also asking for PWM from TCD-, that's not a valid configuration.
        #error "Unsupported TIMERD0_TOP_SETTING define, but USE_TIMERD0_PWM is also defined as 1"
      #endif
    #endif
  #else
    #if !defined(USE_TIMERD0_PWM)
      #define USE_TIMERD0_PWM 1
    #endif
  #endif

  /* On the DX-series parts, we can have a lot more fun with the Type D timer!
   * This flexibility comes at the price of losing the use of TCD0 as a millis timer, but that was being done
   * as a desperation move on the highly timer-constrained non-golden tinyAVR 1-series parts. The TCD0 on the
   * tinyAVRs is also far less interesting - it's virtually identical as far as the timer itself, but here
   * we have a much richer selection timing clock sources.
              |   CLK_PER | Prescale A | Prescale D  | TOP D |
              |-----------|------------|-------------|-------|
              | ** 48 MHz |        256 |             |       |
              | ** 44 MHz |        256 |             |       |
              | ** 40 MHz |        256 |             |       |
              | ** 36 MHz |        256 |             |       |
              |  External |            | OSCHF@8  32 |   254 |
              |  * 32 MHz |        256 |          32 |  1019 |
              |  * 30 MHz |         64 | OSCHF@8  32 |   254 |
              |  * 28 MHz |         64 |          32 |  1019 |
              |  * 27 MHz |         64 | OSCHF@8  32 |   254 |
              |    25 MHz |         64 |          32 |  1019 |
              |    24 MHz |         64 |          32 |  1019 |
              |    20 MHz |         64 |          32 |   509 |
              |    16 MHz |         64 |          32 |   509 |
              |    14 MHz |         64 | OSCHF@28 32 |  1019 |
              |    12 MHz |         64 |          32 |   509 |
              |    10 MHz |         64 | OSCHF@20 32 |   509 |
              |     8 MHz |         64 |          32 |   254 |
              |     7 MHz |         16 | OSCHF@28 32 |  1019 |
              |     6 MHz |         16 | OSCHF@12 32 |   509 |
              |     5 MHz |         16 | OSCHF@20 32 |   509 |
              |     4 MHz |         16 |          32 |   254 |
              |     3 MHz |          8 |           4 |   509 |
              |     2 MHz |          8 |           4 |   509 |
              |     1 MHz |          8 |           4 |   254 |
   */
  #if USE_TIMERD0_PWM == 1
    #if !defined(TIMERD0_WGMODE_SETTING)
      #define TIMERD0_WGMODE_SETTING (TCD_WGMODE_ONERAMP_gc)
    #endif
    #if !defined(TIMERD0_CLOCK_SETTING)
      #if (CLOCK_SOURCE != 0)
        /*
        This is ALSO almost indistinguishable! Same F_PWM, but lower internal frequency.
        Sync is slower. but the bugs with TCD async events won't happen, and it's easier to do wacky stuff with the PROGEV.
        #define TIMERD0_CLOCK_SETTING (TCD_CNTPRES_DIV4_gc | TCD_SYNCPRES_DIV1_gc | TCD_CLKSEL_OSCHF_gc)
        #define TIMERD0_SET_CLOCK     (CLKCTRL_FREQSEL_1M_gc)
        */
        #define TIMERD0_CLOCK_SETTING (TCD_CNTPRES_DIV32_gc | TCD_SYNCPRES_DIV1_gc | TCD_CLKSEL_OSCHF_gc)
        #define TIMERD0_SET_CLOCK     (CLKCTRL_FREQSEL_8M_gc)
        // And we can set TOP to the default 254.
        #if !defined(TIMERD0_TOP_SETTING)
          #define TIMERD0_TOP_SETTING   (0xFE)
        // that gives the target 980 kHz PWM freqwuency....
        #endif
      // if it's internal HF osc as system clock, it's more complicated.....
      #elif (F_CPU == 5000000UL || F_CPU == 10000000UL || F_CPU == 6000000UL || F_CPU == 7000000UL || F_CPU == 14000000UL)
        // These speeds are prescaled so we can run from unprescaled clock, and keep the same settings we use at higher clock.
        #define TIMERD0_CLOCK_SETTING (TCD_CNTPRES_DIV32_gc | TCD_SYNCPRES_DIV1_gc | TCD_CLKSEL_OSCHF_gc)
        #if !defined(TIMERD0_TOP_SETTING)
          #if (F_CPU == 7000000 || F_CPU == 14000000)
            #define TIMERD0_TOP_SETTING   (1019)
          #else
            #define TIMERD0_TOP_SETTING   (509)
          #endif
        #endif
      #elif F_CPU < 4000000UL
        #define TIMERD0_CLOCK_SETTING (TCD_CNTPRES_DIV4_gc | TCD_SYNCPRES_DIV1_gc | TCD_CLKSEL_CLKPER_gc)
        #if !defined(TIMERD0_TOP_SETTING)
          #if (F_CPU == 1000000)
            #define TIMERD0_TOP_SETTING   (254)
          #else
            #define TIMERD0_TOP_SETTING   (509)
          #endif
        #endif
      #else //catchall F_CPU speeds
        #define TIMERD0_CLOCK_SETTING  (TCD_CNTPRES_DIV32_gc | TCD_SYNCPRES_DIV1_gc | TCD_CLKSEL_CLKPER_gc)
        #if !defined(TIMERD0_TOP_SETTING)
          #if (F_CPU <= 8000000)
            #define TIMERD0_TOP_SETTING   (254)
          #elif (F_CPU <= 20000000)
            #define TIMERD0_TOP_SETTING   (509)
          #else
            #define TIMERD0_TOP_SETTING   (1019)
          #endif
        #endif
      #endif // end of F_CPU tests
    #else //CLOCK setting IS defined!!
      #if !defined(TIMERD0_TOP_SETTING)
        #define TIMERD0_TOP_SETTING (254)
      #endif
    #endif
    // Okay, now that we've got the speed of the timer all sorted out, what's next?
    //
    // Right, does the PORTMUX work?
    #if (defined(__AVR_DA__) || defined(__AVR_DB__))
      #define TCD0_PWM_NO_MUX
    #elif (defined(__AVR_DD__))
      #define TCD0_PWM_WITH_MUX
      #warning "TCD PWM is currently only available on PA4-7. Support for all working mux options is planned in a near future release."
    #endif
    // NO_MUX expects the TCD pins to be listed in the timer table as TIMERD0
    // WITH_MUX expects TIMERD0_nWOx where N is 0-7 and x is A-D
    // tinyAVR by contrast ONLY ever uses WOC and WOD.
  #endif // End of USE_TIMERD0_PWM
#endif // end of DxCore TCD stuff - see why we don't support it for millis here?
