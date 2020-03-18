#ifndef __TIMERS_H__
#define __TIMERS_H__
#if (defined(MILLIS_USE_TIMERD0_A0) && defined(TCD0)) || defined(MILLIS_USE_TIMERD0)
#define TIME_TRACKING_TIMER_PERIOD		0x1FF
#elif (defined(MILLIS_USE_TIMERB0)  || defined(MILLIS_USE_TIMERB1) )
//conflgure we will configure it to count to (256*64)-1 instead of prescale by 64
#define TIME_TRACKING_TIMER_PERIOD		0x3FFF
#else
#define TIME_TRACKING_TIMER_PERIOD	    0xFE
#endif
#define TIME_TRACKING_TICKS_PER_OVF		(TIME_TRACKING_TIMER_PERIOD + 1)	/* Timer ticks per overflow of TCB3 */

#if (defined(MILLIS_USE_TIMERD0_A0) && defined(TCD0) || defined(MILLIS_USE_TIMERD0))
#define TIME_TRACKING_TIMER_DIVIDER		32		/* Clock divider for TCB3 */
#elif (defined(MILLIS_USE_TIMERB0)||defined(MILLIS_USE_TIMERB1)||defined(MILLIS_USE_TIMERB2)||defined(MILLIS_USE_TIMERB3))
#define TIME_TRACKING_TIMER_DIVIDER		 1
#else
#define TIME_TRACKING_TIMER_DIVIDER		64		/* Clock divider for TCA0 */
#endif

#define TIME_TRACKING_CYCLES_PER_OVF	(TIME_TRACKING_TICKS_PER_OVF * TIME_TRACKING_TIMER_DIVIDER)

#define PWM_TIMER_PERIOD	0xFE	/* For frequency */
#define PWM_TIMER_COMPARE	0x80	/* For duty cycle */



#endif
