#ifndef __TIMERS_H__
#define __TIMERS_H__

#ifdef MILLIS_USE_TIMERNONE
  #define DISABLE_MILLIS
#endif
#ifdef MILLIS_USE_TIMERRTC_XTAL
  #define MILLIS_USE_TIMERRTC
#endif

#if (defined(MILLIS_USE_TIMERB0) || defined(MILLIS_USE_TIMERB1))
  #if (F_CPU == 1000000UL)
    #define TIME_TRACKING_TIMER_DIVIDER   1
    #define TIME_TRACKING_TIMER_PERIOD ((F_CPU/500)-1)
  #else
    #define TIME_TRACKING_TIMER_DIVIDER   2
    #define TIME_TRACKING_TIMER_PERIOD ((F_CPU/2000)-1)
  #endif
#elif defined(MILLIS_USE_TIMERD0)
  #define TIME_TRACKING_TIMER_PERIOD    0x1FD
  #if (F_CPU == 1000000UL)
    #define TIME_TRACKING_TIMER_DIVIDER    64    /* Clock divider for TCD0 */
  #else
    #define TIME_TRACKING_TIMER_DIVIDER   32    /* Clock divider for TCD0 */
  #endif
#else // Otherwise TCA0
  #define TIME_TRACKING_TIMER_PERIOD    0xFE

  #if (F_CPU == 5000000UL)||(F_CPU == 4000000UL)
    #define TIME_TRACKING_TIMER_DIVIDER   16
  #elif (F_CPU == 1000000UL)
    #define TIME_TRACKING_TIMER_DIVIDER   8
  #else
    #define TIME_TRACKING_TIMER_DIVIDER    64    /* Clock divider for TCA0 */
  #endif
#endif


#if (defined(MILLIS_USE_TIMERD0) && F_CPU == 1000000)
  #define TIMERD0_PRESCALER (TCD_CLKSEL_20MHZ_gc | TCD_CNTPRES_DIV32_gc | TCD_SYNCPRES_DIV2_gc)
#else
  //#define TIMERD0_PRESCALER (TCD_CLKSEL_20MHZ_gc | TCD_CNTPRES_DIV32_gc)

  #define TIMERD0_PRESCALER (TCD_CLKSEL_20MHZ_gc | TCD_CNTPRES_DIV32_gc)
#endif

#define TIME_TRACKING_TICKS_PER_OVF   (TIME_TRACKING_TIMER_PERIOD + 1UL)
#define TIME_TRACKING_CYCLES_PER_OVF  (TIME_TRACKING_TICKS_PER_OVF * TIME_TRACKING_TIMER_DIVIDER)
// For a type B timer as millis, these #defines aren't needed, but they're defined accurately anyway,

// However PWM_TIMER_PERIOD _does_ apply to a type B timer as PWM, or a type A timer not doing millis.
// It happens to be the same as the period of a type A timer that *is* doing millis.

#define PWM_TIMER_PERIOD  0xFE  // For frequency
#define PWM_TIMER_COMPARE 0x00  // For duty cycle - this is never used.
/* The original implementation set the compare registers (all 6 of them, with an STS instruction),
 * and also set a number of other TCA registers to their POR values. That was dumb, and is no longer done. */


#if   defined(MILLIS_USE_TIMERA0)
  #define MILLIS_TIMER TIMERA0
#elif defined(MILLIS_USE_TIMERA1)
  #define MILLIS_TIMER TIMERA1
#elif defined(MILLIS_USE_TIMERB0)
  #define MILLIS_TIMER TIMERB0
#elif defined(MILLIS_USE_TIMERB1)
  #define MILLIS_TIMER TIMERB1
#elif defined(MILLIS_USE_TIMERB2)
  #define MILLIS_TIMER TIMERB2
#elif defined(MILLIS_USE_TIMERB3)
  #define MILLIS_TIMER TIMERB3
#elif defined(MILLIS_USE_TIMERB4)
  #define MILLIS_TIMER TIMERB4
#elif defined(MILLIS_USE_TIMERD0)
  #define MILLIS_TIMER TIMERD0
#elif defined(MILLIS_USE_TIMERRTC)
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
