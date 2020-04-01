#ifndef __TIMERS_H__
#define __TIMERS_H__

#if (defined(MILLIS_USE_TIMERB0) || defined(MILLIS_USE_TIMERB1))
  #if (F_CPU==1000000UL)
    #define TIME_TRACKING_TIMER_DIVIDER   1
    #define TIME_TRACKING_TIMER_PERIOD ((F_CPU/500)-1)
  #else
    #define TIME_TRACKING_TIMER_DIVIDER   2
    #define TIME_TRACKING_TIMER_PERIOD ((F_CPU/2000)-1)
  #endif
#elif defined(MILLIS_USE_TIMERD0)
  #define TIME_TRACKING_TIMER_PERIOD		0x1FD
  #if (F_CPU==1000000UL)
    #define TIME_TRACKING_TIMER_DIVIDER		64		/* Clock divider for TCD0 */
  #else
    #define TIME_TRACKING_TIMER_DIVIDER   32    /* Clock divider for TCD0 */
  #endif
#else //Otherwise TCA0
  #define TIME_TRACKING_TIMER_PERIOD		0xFE

  #if (F_CPU==5000000UL)||(F_CPU==4000000UL)
    #define TIME_TRACKING_TIMER_DIVIDER   16
  #elif (F_CPU==1000000UL)
    #define TIME_TRACKING_TIMER_DIVIDER   8
  #else
    #define TIME_TRACKING_TIMER_DIVIDER		64		/* Clock divider for TCA0 */
  #endif
#endif


#if (defined(MILLIS_USE_TIMERD0) && F_CPU==1000000 && defined(TCD0))
  #define TIMERD0_PRESCALER 0x12
#else
  #define TIMERD0_PRESCALER 0x10
#endif

#define TIME_TRACKING_TICKS_PER_OVF		(TIME_TRACKING_TIMER_PERIOD + 1)
#define TIME_TRACKING_CYCLES_PER_OVF	(TIME_TRACKING_TICKS_PER_OVF * TIME_TRACKING_TIMER_DIVIDER)

//For a type B timer as millis, these #defines aren't needed!

#define PWM_TIMER_PERIOD	0xFE	/* For frequency */
#define PWM_TIMER_COMPARE	0x80	/* For duty cycle */

#if defined(MILLIS_USE_TIMERA0)
  #define MILLIS_TIMER TIMERA0
#elif defined(MILLIS_USE_TIMERB0)
  #define MILLIS_TIMER TIMERB0
#elif defined(MILLIS_USE_TIMERB1)
  #define MILLIS_TIMER TIMERB1
#elif defined(MILLIS_USE_TIMERD0)
  #define MILLIS_TIMER TIMERD0
#elif defined(MILLIS_USE_TIMERRTC)
  #define MILLIS_TIMER TIMERRTC
#else
  #define MILLIS_TIMER NOT_ON_TIMER
#endif

#endif
