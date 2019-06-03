/*
  wiring.c - Partial implementation of the Wiring API for the ATmega8.
  Part of Arduino - http://www.arduino.cc/

  Copyright (c) 2005-2006 David A. Mellis

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General
  Public License along with this library; if not, write to the
  Free Software Foundation, Inc., 59 Temple Place, Suite 330,
  Boston, MA  02111-1307  USA
*/

#include "wiring_private.h"

// the prescaler is set so that timer ticks every 64 clock cycles, and the
// the overflow handler is called every 256 ticks.
volatile uint16_t microseconds_per_timer_overflow;
volatile uint16_t microseconds_per_timer_tick;

uint32_t F_CPU_CORRECTED = F_CPU;

// the whole number of milliseconds per timer overflow
uint16_t millis_inc;

// the fractional number of milliseconds per timer overflow
uint16_t fract_inc;
#define FRACT_MAX (1000)

// whole number of microseconds per timer tick

volatile uint32_t timer_overflow_count = 0;
volatile uint32_t timer_millis = 0;
static uint16_t timer_fract = 0;

inline uint16_t clockCyclesPerMicrosecondComp(uint32_t clk){
	return ( (clk) / 1000000L );
}

inline uint16_t clockCyclesPerMicrosecond(){
	return clockCyclesPerMicrosecondComp(F_CPU_CORRECTED);
}

inline unsigned long clockCyclesToMicroseconds(unsigned long cycles){
	return ( cycles / clockCyclesPerMicrosecond() );
}

inline unsigned long microsecondsToClockCycles(unsigned long microseconds){
	return ( microseconds * clockCyclesPerMicrosecond() );
}

static volatile TCB_t* _timer =
#if defined(MILLIS_USE_TIMERB0)
&TCB0;
#elif defined(MILLIS_USE_TIMERB1)
&TCB1;
#elif defined(MILLIS_USE_TIMERB2)
&TCB2;
#elif defined(MILLIS_USE_TIMERB3)
&TCB3;
#else
// fallback to TCB0 (every platform has it)
&TCB0;
#endif

#if defined(MILLIS_USE_TIMERB0)
ISR(TCB0_INT_vect)
#elif defined(MILLIS_USE_TIMERB1)
ISR(TCB1_INT_vect)
#elif defined(MILLIS_USE_TIMERB2)
ISR(TCB2_INT_vect)
#elif defined(MILLIS_USE_TIMERB3)
ISR(TCB3_INT_vect)
#else
// fallback to TCB0 (every platform has it)
ISR(TCB0_INT_vect)
#endif
{
	// copy these to local variables so they can be stored in registers
	// (volatile variables must be read from memory on every access)
	uint32_t m = timer_millis;
	uint16_t f = timer_fract;

	m += millis_inc;
	f += fract_inc;
	if (f >= FRACT_MAX) {

		f -= FRACT_MAX;
		m += 1;
	}

	timer_fract = f;
	timer_millis = m;
	timer_overflow_count++;

	/* Clear flag */
	_timer->INTFLAGS = TCB_CAPT_bm;
}

unsigned long millis()
{
	unsigned long m;

	// disable interrupts while we read timer0_millis or we might get an
	// inconsistent value (e.g. in the middle of a write to timer0_millis)
	uint8_t status = SREG;
	cli();
	m = timer_millis;

	SREG = status;

	return m;
}

unsigned long micros() {
	unsigned long overflows, microseconds;
	uint8_t ticks;

	/* Save current state and disable interrupts */
	uint8_t status = SREG;
	cli();

	/* Get current number of overflows and timer count */
	overflows = timer_overflow_count;
	ticks = _timer->CNTL;

	/* If the timer overflow flag is raised, we just missed it,
	increment to account for it, & read new ticks */
	if(_timer->INTFLAGS & TCB_CAPT_bm){
		overflows++;
		ticks = _timer->CNTL;
	}

	/* Restore state */
	SREG = status;

	/* Return microseconds of up time  (resets every ~70mins) */
	microseconds = ((overflows * microseconds_per_timer_overflow)
				+ (ticks * microseconds_per_timer_tick));
	return microseconds;
}

void delay(unsigned long ms)
{
	uint32_t start_time = micros(), delay_time = 1000*ms;

	/* Calculate future time to return */
	uint32_t return_time = start_time + delay_time;

	/* If return time overflows */
	if(return_time < delay_time){
		/* Wait until micros overflows */
		while(micros() > return_time);
	}

	/* Wait until return time */
	while(micros() < return_time);
}

/* Delay for the given number of microseconds.  Assumes a 1, 8, 12, 16, 20 or 24 MHz clock. */
void delayMicroseconds(unsigned int us)
{
	// call = 4 cycles + 2 to 4 cycles to init us(2 for constant delay, 4 for variable)

	// calling avrlib's delay_us() function with low values (e.g. 1 or
	// 2 microseconds) gives delays longer than desired.
	//delay_us(us);
#if F_CPU >= 24000000L
	// for the 24 MHz clock for the aventurous ones, trying to overclock

	// zero delay fix
	if (!us) return; //  = 3 cycles, (4 when true)

	// the following loop takes a 1/6 of a microsecond (4 cycles)
	// per iteration, so execute it six times for each microsecond of
	// delay requested.
	us *= 6; // x6 us, = 7 cycles

	// account for the time taken in the preceeding commands.
	// we just burned 22 (24) cycles above, remove 5, (5*4=20)
	// us is at least 6 so we can substract 5
	us -= 5; //=2 cycles

#elif F_CPU >= 20000000L
	// for the 20 MHz clock on rare Arduino boards

	// for a one-microsecond delay, simply return.  the overhead
	// of the function call takes 18 (20) cycles, which is 1us
	__asm__ __volatile__ (
		"nop" "\n\t"
		"nop" "\n\t"
		"nop" "\n\t"
		"nop"); //just waiting 4 cycles
	if (us <= 1) return; //  = 3 cycles, (4 when true)

	// the following loop takes a 1/5 of a microsecond (4 cycles)
	// per iteration, so execute it five times for each microsecond of
	// delay requested.
	us = (us << 2) + us; // x5 us, = 7 cycles

	// account for the time taken in the preceeding commands.
	// we just burned 26 (28) cycles above, remove 7, (7*4=28)
	// us is at least 10 so we can substract 7
	us -= 7; // 2 cycles

#elif F_CPU >= 16000000L
	// for the 16 MHz clock on most Arduino boards

	// for a one-microsecond delay, simply return.  the overhead
	// of the function call takes 14 (16) cycles, which is 1us
	if (us <= 1) return; //  = 3 cycles, (4 when true)

	// the following loop takes 1/4 of a microsecond (4 cycles)
	// per iteration, so execute it four times for each microsecond of
	// delay requested.
	us <<= 2; // x4 us, = 4 cycles

	// account for the time taken in the preceeding commands.
	// we just burned 19 (21) cycles above, remove 5, (5*4=20)
	// us is at least 8 so we can substract 5
	us -= 5; // = 2 cycles,

#elif F_CPU >= 12000000L
	// for the 12 MHz clock if somebody is working with USB

	// for a 1 microsecond delay, simply return.  the overhead
	// of the function call takes 14 (16) cycles, which is 1.5us
	if (us <= 1) return; //  = 3 cycles, (4 when true)

	// the following loop takes 1/3 of a microsecond (4 cycles)
	// per iteration, so execute it three times for each microsecond of
	// delay requested.
	us = (us << 1) + us; // x3 us, = 5 cycles

	// account for the time taken in the preceeding commands.
	// we just burned 20 (22) cycles above, remove 5, (5*4=20)
	// us is at least 6 so we can substract 5
	us -= 5; //2 cycles

#elif F_CPU >= 8000000L
	// for the 8 MHz internal clock

	// for a 1 and 2 microsecond delay, simply return.  the overhead
	// of the function call takes 14 (16) cycles, which is 2us
	if (us <= 2) return; //  = 3 cycles, (4 when true)

	// the following loop takes 1/2 of a microsecond (4 cycles)
	// per iteration, so execute it twice for each microsecond of
	// delay requested.
	us <<= 1; //x2 us, = 2 cycles

	// account for the time taken in the preceeding commands.
	// we just burned 17 (19) cycles above, remove 4, (4*4=16)
	// us is at least 6 so we can substract 4
	us -= 4; // = 2 cycles

#else
	// for the 1 MHz internal clock (default settings for common Atmega microcontrollers)

	// the overhead of the function calls is 14 (16) cycles
	if (us <= 16) return; //= 3 cycles, (4 when true)
	if (us <= 25) return; //= 3 cycles, (4 when true), (must be at least 25 if we want to substract 22)

	// compensate for the time taken by the preceeding and next commands (about 22 cycles)
	us -= 22; // = 2 cycles
	// the following loop takes 4 microseconds (4 cycles)
	// per iteration, so execute it us/4 times
	// us is at least 4, divided by 4 gives us 1 (no zero delay bug)
	us >>= 2; // us div 4, = 4 cycles


#endif

	// busy wait
	__asm__ __volatile__ (
		"1: sbiw %0,1" "\n\t" // 2 cycles
		"brne 1b" : "=w" (us) : "0" (us) // 2 cycles
	);
	// return = 4 cycles
}

void init()
{
	// this needs to be called before setup() or some functions won't
	// work there
	
/******************************** CLOCK STUFF *********************************/

	/* We assume 5V operating frequency and FUSE.OSCCFG -> 16MHz */

 	int64_t cpu_freq;
 	
	#if (F_CPU == 20000000)
		cpu_freq = 20000000;
		
		/* No division on clock */
		_PROTECTED_WRITE(CLKCTRL_MCLKCTRLB, 0x00);
	
	#elif (F_CPU == 16000000)
		cpu_freq = 16000000;
		
		/* No division on clock */
		_PROTECTED_WRITE(CLKCTRL_MCLKCTRLB, 0x00);
		
	#elif (F_CPU == 8000000)
		cpu_freq = 8000000;
		
		/* Clock DIV2 */
		_PROTECTED_WRITE(CLKCTRL_MCLKCTRLB, (CLKCTRL_PEN_bm | CLKCTRL_PDIV_2X_gc));
		
	#elif (F_CPU == 4000000)
		cpu_freq = 4000000;
		
		/* Clock DIV4 */
		_PROTECTED_WRITE(CLKCTRL_MCLKCTRLB, (CLKCTRL_PEN_bm | CLKCTRL_PDIV_4X_gc));
		
	#elif (F_CPU == 2000000)
		cpu_freq = 2000000;
		
		/* Clock DIV8 */
		_PROTECTED_WRITE(CLKCTRL_MCLKCTRLB, (CLKCTRL_PEN_bm | CLKCTRL_PDIV_8X_gc));
	#else
		
		#ifndef F_CPU
			# warning "F_CPU not defined"
			#define F_CPU 16000000
		#endif
		
		# warning "F_CPU defined as an invalid value - may cause undefined behavior"
		
		/* Default value is 16MHz */
		cpu_freq = 16000000;
				
		/* No division on clock */
		_PROTECTED_WRITE(CLKCTRL_MCLKCTRLB, 0x00);
	#endif

	/* Apply calculated value to F_CPU_CORRECTED */
	F_CPU_CORRECTED = (uint32_t)cpu_freq;


/********************************* ADC ****************************************/

#if defined(ADC0)

	/* ADC clock between 50-200 kHz */

	#if F_CPU >= 20000000 // 20 MHz / 128 = 156.250 kHz
		ADC0.CTRLC |= ADC_PRESC_DIV128_gc;
	#elif F_CPU >= 16000000 // 16 MHz / 128 = 125 kHz
		ADC0.CTRLC |= ADC_PRESC_DIV128_gc;
	#elif F_CPU >= 8000000 // 8 MHz / 64 = 125 kHz
		ADC0.CTRLC |= ADC_PRESC_DIV64_gc;
	#elif F_CPU >= 4000000 // 4 MHz / 32 = 125 kHz
		ADC0.CTRLC |= ADC_PRESC_DIV32_gc;
	#elif F_CPU >= 2000000 // 2 MHz / 16 = 125 kHz
		ADC0.CTRLC |= ADC_PRESC_DIV16_gc;
	#elif F_CPU >= 1000000 // 1 MHz / 8 = 125 kHz
		ADC0.CTRLC |= ADC_PRESC_DIV8_gc;
	#else // 128 kHz / 2 = 64 kHz -> This is the closest you can get, the prescaler is 2
		ADC0.CTRLC |= ADC_PRESC_DIV2_gc;
	#endif

	/* Enable ADC */
	ADC0.CTRLA |= ADC_ENABLE_bm;
	analogReference(VDD);

#endif

	PORTMUX.USARTROUTEA = 0;

	setup_timers();

	/********************* TIMER for system time tracking **************************/

	/* Calculate relevant time tracking values */
	microseconds_per_timer_overflow = clockCyclesToMicroseconds(TIME_TRACKING_CYCLES_PER_OVF);
	microseconds_per_timer_tick = microseconds_per_timer_overflow/TIME_TRACKING_TIMER_PERIOD;

	millis_inc = microseconds_per_timer_overflow / 1000;
	fract_inc = ((microseconds_per_timer_overflow % 1000));

	/* Default Periodic Interrupt Mode */
	/* TOP value for overflow every 1024 clock cycles */
	_timer->CCMP = TIME_TRACKING_TIMER_PERIOD;

	/* Enable timer interrupt */
	_timer->INTCTRL |= TCB_CAPT_bm;

	/* Clock selection -> same as TCA (F_CPU/64 -- 250kHz) */
	_timer->CTRLA = TCB_CLKSEL_CLKTCA_gc;

	/* Enable & start */
	_timer->CTRLA |= TCB_ENABLE_bm;	/* Keep this last before enabling interrupts to ensure tracking as accurate as possible */

/*************************** ENABLE GLOBAL INTERRUPTS *************************/

	sei();
}

void setup_timers(void)  __attribute__((weak));