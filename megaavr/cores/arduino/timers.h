#ifndef __TIMERS_H__
#define __TIMERS_H__

#define TIME_TRACKING_TIMER_PERIOD		0xFF
#define TIME_TRACKING_TICKS_PER_OVF		(TIME_TRACKING_TIMER_PERIOD + 1)	/* Timer ticks per overflow of TCB3 */
#define TIME_TRACKING_TIMER_DIVIDER		64		/* Clock divider for TCB3 */
#define TIME_TRACKING_CYCLES_PER_OVF	(TIME_TRACKING_TICKS_PER_OVF * TIME_TRACKING_TIMER_DIVIDER)

#define PWM_TIMER_PERIOD	0xFF	/* For frequency */
#define PWM_TIMER_COMPARE	0x80	/* For duty cycle */

#endif