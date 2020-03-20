#ifndef __TIMERS_H__
#define __TIMERS_H__

#if !(defined(MILLIS_USE_TIMERB0) || defined(MILLIS_USE_TIMERB1) )

#if (defined(MILLIS_USE_TIMERD0_A0) && defined(TCD0)) || defined(MILLIS_USE_TIMERD0)
#define TIME_TRACKING_TIMER_PERIOD		0x1FF
#define TIME_TRACKING_TIMER_DIVIDER		32		/* Clock divider for TCD0 */
#else //Otherwise TCA0
#define TIME_TRACKING_TIMER_PERIOD		0xFE
#define TIME_TRACKING_TIMER_DIVIDER		64		/* Clock divider for TCA0 */
#endif
#define TIME_TRACKING_TICKS_PER_OVF		(TIME_TRACKING_TIMER_PERIOD + 1)
#define TIME_TRACKING_CYCLES_PER_OVF	(TIME_TRACKING_TICKS_PER_OVF * TIME_TRACKING_TIMER_DIVIDER)

#endif
//For a type B timer as millis, these #defines aren't needed!

#define PWM_TIMER_PERIOD	0xFE	/* For frequency */
#define PWM_TIMER_COMPARE	0x80	/* For duty cycle */



#endif
