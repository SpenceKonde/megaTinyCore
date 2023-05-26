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
#if !defined(TCA_PORTMUX)
  #define TCA_PORTMUX (0x00)
#endif
#define _TCA_USE_WO0
#define _TCA_USE_WO1
#define _TCA_USE_WO2
#if TCA_PORTMUX & 0x01
  #define _TCA_ALT_WO0
#endif
#if TCA_PORTMUX & 0x02
  #define _TCA_ALT_WO1
#endif
#if TCA_PORTMUX & 0x04
  #define _TCA_ALT_WO2
#endif
#if !defined(TCA_BUFFERED_3PIN)
  #if TCA_PORTMUX & 0x08
    #define _TCA_ALT_WO3
  #else
    #define _TCA_USE_WO3
  #endif
  #if TCA_PORTMUX & 0x10
    #define _TCA_ALT_WO4
  #else
    #define _TCA_USE_WO4
  #endif
  #if TCA_PORTMUX & 0x20
    #define _TCA_ALT_WO5
  #else
    #define _TCA_USE_WO5
  #endif
#endif
#if defined(USE_TIMERD0_PWM)
  #if !defined(TCD0)
    #pragma message("Note: This part does not have a TCD, hence there is no PWM from TCD available.")
    #undef USE_TIMERD0_PWM
  #endif
#endif

#if (defined(MILLIS_USE_TIMERB0) || defined(MILLIS_USE_TIMERB1) || defined(MILLIS_USE_TIMERB2) || defined(MILLIS_USE_TIMERB3) || defined(MILLIS_USE_TIMERB4) || defined(MILLIS_USE_TIMERB5) || defined(MILLIS_USE_TIMERB6) || defined(MILLIS_USE_TIMERB7))
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
  #define MILLIS_VECTOR TCA0_HUNF_vect
#elif defined(MILLIS_USE_TIMERA1)
  #ifndef TCA1
    #error "TCA1, selected for millis, does not exist on this part"
  #endif
  #define MILLIS_TIMER TIMERA1
  #define MILLIS_VECTOR TCA1_HUNF_vect
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
#elif defined(MILLIS_USE_TIMERB5)
  #ifndef TCB5
    #error "TCB5, selected for millis, does not exist on this part"
  #endif
  #define MILLIS_TIMER TIMERB5
#elif defined(MILLIS_USE_TIMERB6)
  #ifndef TCB6
    #error "TCB6, selected for millis, does not exist on this part"
  #endif
  #define MILLIS_TIMER TIMERB6
#elif defined(MILLIS_USE_TIMERB7)
  #ifndef TCB7
    #error "TCB7, selected for millis, does not exist on this part"
  #endif
  #define MILLIS_TIMER TIMERB7
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
    #pragma message("TCB autoMUXing is not yet supported")
  #endif
  /* If this feature is ever implemented,  calling analogWrite() on a pin that has PWM controlled by a TCB
   * it will automatically also set or clear the PORTMUX bit to point the TCB at the right place.
   * These pins will be included in the pin timer table.
   */
#endif



/* TYPE-D TIMERS */
#if defined(MEGATINYCORE)
  /* TCD0 on megaTinyCore is run from the unprescaled clock, at either 20 or 16 MHz depending on the fuse setting.
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
      #if defined(TCD0)
        #define USE_TIMERD0_PWM
      #endif
    #endif
  #else

  #endif

  /* On the DX-series parts, we can have a lot more fun with the Type D timer!
   * This flexibility comes at the price of losing the use of TCD0 as a millis timer, but that was being done
   * as a desperation move on the highly timer-constrained non-golden tinyAVR 1-series parts. The TCD0 on the
   * tinyAVRs is also far less interesting - it's virtually identical as far as the timer itself, but here
   * we have a much richer selection timing clock sources.
   */
  #if defined(USE_TIMERD0_PWM)
    #if !defined(TIMERD0_WGMODE_SETTING)
      #define TIMERD0_WGMODE_SETTING (TCD_WGMODE_ONERAMP_gc) // only WGMODE_ONERAMP and WGMODE_DUALSLOPE are supported by the core, as the other options
      // cannot reproduce the behavior of analogWrite as they enforce PWM being non-overlapping. If you want those modes (I think 4 ramp mode has some
      // relevant applications in combination with the TCD.CTRLD options, particularly in custom single wire protocols, particularly if you need to generate events
      // at certain points in the cycle.
    #endif
    #if !defined(TIMERD0_CLOCK_SETTING)
      #if (CLOCK_SOURCE != 0)
        /*
        Hey, we're not using the internal oscillator for the clock! We can totally run it at an ideal frequency from internal and use external for system clock!

        There are at least five ways to slice this that look nearly identical. These are virtually indistinguishable from each other giving PWM at 980 Hz.
        One runs the clock at 8 MHz with /32 division on the count prescaler and /1 on the synchronizer.
        Another method drops the count prescaler back to /4, and compensates by increasing the sync prescaler to /8
        A third way is to drop the count prescaler back to /4, sync prescale to /1, and set the FREQSEL to 1 MHz.
        Sync prescale options of /2 and /4 can be paired with setting the osc to 2 or 4 MHz as well, with the same result
        Lowering count prescale dodges some bugs, and there isn't exactly a wealth of options. (it goes /4 to /32, nothing in the middle,
        and no undocumented fourth option (I checked :-P).
        But /32 will get you right on target (that being ~1 kHz) if CLK_OSCHF / Sync prescale = 8 MHz
        And /4 will get you right on target if CLK_OSCHF / Sync prescale = 1 MHz
        There are still more equivalent settings if you consider that there are more than one way to get to 1 MHz from the internal HF oscillator.
        Those settings are strictly worse if you aren't using the internal osc  for anything else.
        On the one hand, running with clock set to 1 MHz is probably more power efficient, though nothing using a crystal is going to set any records
        for power consumption - and using an external clock, well, you have the power consumption of the clock to worry about, which is often larger
        than the chip itself...
        On the other hand, using the /32 count prescaling exposes us to some silicon errata on the DA/DB - but in weirdo features nobody uses.
        On the third hand, if the crystal or clock is defective, and you were relying on overriding clock failure callbacks with your own functions
        that enable the internal oscillator at a known speed to communicate that to you (because you got a killer deal on crystals that have
        astonishingly high DOA rate or something?) If clock failure is a normal occurrence, you're doing something wrong...

        Anyway, regarding how we get the same frequency, well... very few Arduino folks care!


        #define TIMERD0_CLOCK_SETTING (TCD_CNTPRES_DIV4_gc | TCD_SYNCPRES_DIV8_gc | TCD_CLKSEL_OSCHF_gc)
        #define TIMERD0_SET_CLOCK     (CLKCTRL_FREQSEL_8M_gc)

        #define TIMERD0_CLOCK_SETTING (TCD_CNTPRES_DIV4_gc | TCD_SYNCPRES_DIV4_gc | TCD_CLKSEL_OSCHF_gc)
        #define TIMERD0_SET_CLOCK     (CLKCTRL_FREQSEL_4M_gc)

        #define TIMERD0_CLOCK_SETTING (TCD_CNTPRES_DIV4_gc | TCD_SYNCPRES_DIV2_gc | TCD_CLKSEL_OSCHF_gc)
        #define TIMERD0_SET_CLOCK     (CLKCTRL_FREQSEL_2M_gc)

        #define TIMERD0_CLOCK_SETTING (TCD_CNTPRES_DIV4_gc | TCD_SYNCPRES_DIV1_gc | TCD_CLKSEL_OSCHF_gc)
        #define TIMERD0_SET_CLOCK     (CLKCTRL_FREQSEL_1M_gc)

        Hell... we could even set TOP to 2039 and do
        #define TIMERD0_CLOCK_SETTING (TCD_CNTPRES_DIV4_gc | TCD_SYNCPRES_DIV1_gc | TCD_CLKSEL_OSCHF_gc)
        #define TIMERD0_SET_CLOCK     (CLKCTRL_FREQSEL_1M_gc)
        #define TIMERD0_TOP_SETTING   (2039)



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
    #else //CLOCK setting IS defined!! That is unusual indeed!
      #if !defined(TIMERD0_TOP_SETTING)
        #define TIMERD0_TOP_SETTING (254)
      #endif
    #endif
  #endif // End of USE_TIMERD0_PWM
#endif // end of DxCore TCD stuff - see why we don't support it for millis here?
