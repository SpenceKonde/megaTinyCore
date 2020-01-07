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

#ifndef DISABLEMILLIS

#define FRACT_INC
#define MILLIS_INC

#ifdef MILLIS_USE_TIMERD0_A0
#ifdef TCD0
#define MILLIS_USE_TIMERD0
#else
#define MILLIS_USE_TIMERA0
#endif
#endif

volatile uint16_t microseconds_per_timer_overflow;
volatile uint16_t microseconds_per_timer_tick;


// the whole number of milliseconds per timer overflow
uint16_t millis_inc;

// the fractional number of milliseconds per timer overflow

#if !defined(MILLIS_USE_TIMERRTC)
uint16_t fract_inc;
#define FRACT_MAX (1000)
#endif

// whole number of microseconds per timer tick

volatile uint32_t timer_overflow_count = 0;
volatile uint32_t timer_millis = 0;

#if !defined(MILLIS_USE_TIMERRTC)
static uint16_t timer_fract = 0;
#endif

inline uint16_t clockCyclesPerMicrosecondComp(uint32_t clk){
#ifdef MILLIS_USE_TIMERD0
	#if (F_CPU==20000000UL || F_CPU==10000000UL ||F_CPU==5000000UL)
	return ( 20 ); //this always runs off the 20MHz oscillator
	#else
	return ( 16 );
	#endif
#else
	return ( (clk) / 1000000L );
#endif
}

inline uint16_t clockCyclesPerMicrosecond(){
	return clockCyclesPerMicrosecondComp(F_CPU);
}

inline unsigned long clockCyclesToMicroseconds(unsigned long cycles){
	return ( cycles / clockCyclesPerMicrosecond() );
}

inline unsigned long microsecondsToClockCycles(unsigned long microseconds){
	return ( microseconds * clockCyclesPerMicrosecond() );
}



#ifdef MILLIS_USE_TIMERD0
#ifndef TCD0
#error "Selected millis timer, TCD0, only exists on 1-series parts"
#endif
#endif

#if !(defined(MILLIS_USE_TIMERA0)||defined(MILLIS_USE_TIMERD0)||defined(MILLIS_USE_TIMERRTC))
static volatile TCB_t* _timer =
#if defined(MILLIS_USE_TIMERB0)
&TCB0;
#elif defined(MILLIS_USE_TIMERB1)
#ifndef TCB1
#error "Selected millis timer, TCB1 does not exist on this part."
#endif
&TCB1;
#elif defined(MILLIS_USE_TIMERB2) //not sure if any parts that support this exist in the tiny line, but it here anyway
#ifndef TCB2
#error "Selected millis timer, TCB2 does not exist on this part."
#endif
&TCB2;
#elif defined(MILLIS_USE_TIMERB3)//not sure if any parts that support this exist in the tiny line, but it here anyway
#ifndef TCB3
#error "Selected millis timer, TCB3 does not exist on this parts."
#endif
&TCB3;
#else
#error "No millis timer selected".
#endif
#endif

#if defined(MILLIS_USE_TIMERA0)
ISR(TCA0_LUNF_vect)
#elif defined(MILLIS_USE_TIMERD0)
ISR(TCD0_OVF_vect)
#elif defined(MILLIS_USE_TIMERRTC)
ISR(RTC_OVF_vect)
#elif defined(MILLIS_USE_TIMERB0)
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

    #if !defined(MILLIS_USE_TIMERRTC)
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
    #else
	timer_millis+=millis_inc;
	#endif
	/* Clear flag */
	#if defined(MILLIS_USE_TIMERA0)
	TCA0.SPLIT.INTFLAGS = TCA_SPLIT_LUNF_bm;
	#elif defined(MILLIS_USE_TIMERD0)
	TCD0.INTFLAGS=TCD_OVF_bm;
	#elif defined(MILLIS_USE_TIMERRTC)
	RTC.INTFLAGS=RTC_OVF_bm;
	#else //timerb
	_timer->INTFLAGS = TCB_CAPT_bm;
	#endif
}

unsigned long millis()
{
	//return timer_overflow_count; //for debugging timekeeping issues where these variables are out of scope from the sketch
	unsigned long m;

	// disable interrupts while we read timer0_millis or we might get an
	// inconsistent value (e.g. in the middle of a write to timer0_millis)
	uint8_t status = SREG;
	cli();
	m = timer_millis;
	#ifdef MILLIS_USE_TIMERRTC
	//to do: implement millis for RTC timer, as the value of the RTC is very important here.
	#endif
	SREG = status;

	return m;
}
#ifndef MILLIS_USE_TIMERRTC
unsigned long micros() {
	unsigned long overflows, microseconds;

	#if defined(MILLIS_USE_TIMERD0)
	uint16_t ticks;
	#else
	uint8_t ticks;
	#endif

	/* Save current state and disable interrupts */
	uint8_t status = SREG;
	cli();

	/* Get current number of overflows and timer count */
	overflows = timer_overflow_count;
	#if defined(MILLIS_USE_TIMERA0)
	ticks = 0xFF-TCA0.SPLIT.LCNT;
	#elif defined(MILLIS_USE_TIMERD0)
	TCD0.CTRLE=TCD_SCAPTUREA_bm;
	while(!(TCD0.STATUS&TCD_CMDRDY_bm)); //wait for sync - should be only one iteration of this loop
	ticks=TCD0.CAPTUREA;
	#else
	ticks = _timer->CNTL;
	#endif
	/* If the timer overflow flag is raised, we just missed it,
	increment to account for it, & read new ticks */
	#if defined(MILLIS_USE_TIMERD0)
	if(TCD0.INTFLAGS & TCD_OVF_bm){
		overflows++;
		TCD0.CTRLE=TCD_SCAPTUREA_bm;
		while(!(TCD0.STATUS & TCD_CMDRDY_bm)); //wait for sync - should be only one iteration of this loop
		ticks=TCD0.CAPTUREA;
	}
	#elif defined(MILLIS_USE_TIMERA0)
	if(TCA0.SPLIT.INTFLAGS & TCA_SPLIT_LUNF_bm){
		overflows++;
		ticks = 0xFF-TCA0.SPLIT.LCNT;

	}
	#else
	if(_timer->INTFLAGS & TCB_CAPT_bm){
		overflows++;
		ticks = _timer->CNTL;
	}
	#endif

	/* Restore state */
	SREG = status;

	/* Return microseconds of up time  (resets every ~70mins) */
	microseconds = ((overflows * microseconds_per_timer_overflow)
				+ (ticks * microseconds_per_timer_tick));
	return microseconds;
}
#endif //end of non-RTC micros code
#endif //end of non-DISABLE_MILLIS code

#if !(defined(DISABLE_MILLIS) || defined(MILLIS_USE_TIMERRTC)) //delay implementation when we do have micros()
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
#else //delay implementation when we do not
void delay(unsigned long ms)
{
  while(ms--){
    delayMicroseconds(1000);
  }
}
#endif


/* Delay for the given number of microseconds.  Assumes a 1, 4, 5, 8, 10, 16, or 20 MHz clock. */
void delayMicroseconds(unsigned int us)
{
  // call = 4 cycles + 2 to 4 cycles to init us(2 for constant delay, 4 for variable)

  // calling avrlib's delay_us() function with low values (e.g. 1 or
  // 2 microseconds) gives delays longer than desired.
  //delay_us(us);
#if F_CPU >= 20000000L
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

  // account for the time taken in the preceding commands.
  // we just burned 26 (28) cycles above, remove 7, (7*4=28)
  // us is at least 10 so we can subtract 7
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

  // account for the time taken in the preceding commands.
  // we just burned 19 (21) cycles above, remove 5, (5*4=20)
  // us is at least 8 so we can subtract 5
  us -= 5; // = 2 cycles,

#elif F_CPU >= 10000000L
  // for 10MHz (20MHz/2)

  // for a 1 microsecond delay, simply return.  the overhead
  // of the function call takes 14 (16) cycles, which is 1.5us
  if (us <= 1) return; //  = 3 cycles, (4 when true)

  // the following loop takes 2/5 of a microsecond (4 cycles)
  // per iteration, so execute it 2.5 times for each microsecond of
  // delay requested.
  us = (us << 1) + (us>>1); // x2.5 us, = 5 cycles

  // account for the time taken in the preceding commands.
  // we just burned 20 (22) cycles above, remove 5, (5*4=20)
  // us is at least 6 so we can subtract 5
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

  // account for the time taken in the preceding commands.
  // we just burned 17 (19) cycles above, remove 4, (4*4=16)
  // us is at least 6 so we can subtract 4
  us -= 4; // = 2 cycles
#elif F_CPU >= 5000000L
  // for 5MHz (20MHz / 4)

  // for a 1 ~ 3 microsecond delay, simply return.  the overhead
  // of the function call takes 14 (16) cycles, which is 3us
  if (us <= 3) return; //  = 3 cycles, (4 when true)

  // the following loop takes 4/5th microsecond (4 cycles)
  // per iteration, so we want to add it to 1/4th of itself
  us +=us>>2;
  us -= 2; // = 2 cycles

#elif F_CPU >= 4000000L
  // for that unusual 4mhz clock...

  // for a 1 ~ 4 microsecond delay, simply return.  the overhead
  // of the function call takes 14 (16) cycles, which is 4us
  if (us <= 4) return; //  = 3 cycles, (4 when true)

  // the following loop takes 1 microsecond (4 cycles)
  // per iteration, so nothing to do here! \o/

  us -= 2; // = 2 cycles


#else
  // for the 1 MHz internal clock (default settings for common AVR microcontrollers)
  // the overhead of the function calls is 14 (16) cycles
  if (us <= 16) return; //= 3 cycles, (4 when true)
  if (us <= 25) return; //= 3 cycles, (4 when true), (must be at least 25 if we want to subtract 22)

  // compensate for the time taken by the preceding and next commands (about 22 cycles)
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


	#if (F_CPU == 20000000)
		/* No division on clock */
		_PROTECTED_WRITE(CLKCTRL_MCLKCTRLB, 0x00);

	#elif (F_CPU == 16000000)
		/* No division on clock */
		_PROTECTED_WRITE(CLKCTRL_MCLKCTRLB, 0x00);

	#elif (F_CPU == 10000000) //20MHz prescaled by 2
		/* Clock DIV2 */
		_PROTECTED_WRITE(CLKCTRL_MCLKCTRLB, (CLKCTRL_PEN_bm | CLKCTRL_PDIV_2X_gc));

	#elif (F_CPU == 8000000) //16MHz prescaled by 2
		/* Clock DIV2 */
		_PROTECTED_WRITE(CLKCTRL_MCLKCTRLB, (CLKCTRL_PEN_bm | CLKCTRL_PDIV_2X_gc));

	#elif (F_CPU == 5000000) //20MHz prescaled by 4
		/* Clock DIV4 */
		_PROTECTED_WRITE(CLKCTRL_MCLKCTRLB, (CLKCTRL_PEN_bm | CLKCTRL_PDIV_4X_gc));

	#elif (F_CPU == 4000000) //16MHz prescaled by 4
		/* Clock DIV4 */
		_PROTECTED_WRITE(CLKCTRL_MCLKCTRLB, (CLKCTRL_PEN_bm | CLKCTRL_PDIV_4X_gc));

	#elif (F_CPU == 1000000) //16MHz prescaled by 16
		/* Clock DIV8 */
		_PROTECTED_WRITE(CLKCTRL_MCLKCTRLB, (CLKCTRL_PEN_bm | CLKCTRL_PDIV_16X_gc));
	#else

		#ifndef F_CPU
			#error "F_CPU not defined"
		#else
	 		#error "F_CPU defined as an unsupported value"
		#endif
	#endif


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

	#if (HWSERIAL0_MUX!=0 || SPI_MUX!=0 ||TWI_MUX!=0)
		PORTMUX.CTRLB=HWSERIAL0_MUX|SPI_MUX|TWI_MUX; //Set PORTMUX
	#endif
	#ifdef __AVR_ATtinyxy2__
		PORTMUX.CTRLC=1;
	#endif

	setup_timers();

#ifndef DISABLEMILLIS

	/********************* TIMER for system time tracking **************************/

	/* Calculate relevant time tracking values */
	microseconds_per_timer_overflow = clockCyclesToMicroseconds(TIME_TRACKING_CYCLES_PER_OVF);
	microseconds_per_timer_tick = microseconds_per_timer_overflow/TIME_TRACKING_TIMER_PERIOD;

	millis_inc = microseconds_per_timer_overflow / 1000;
	fract_inc = ((microseconds_per_timer_overflow % 1000));

    #if defined(MILLIS_USE_TIMERA0)
    TCA0.SPLIT.INTCTRL = TCA_SPLIT_LUNF_bm;
    #elif defined(MILLIS_USE_TIMERD0)
    TCD0.CMPBCLR=TIME_TRACKING_TIMER_PERIOD; //essentially, this is TOP
    TCD0.INTCTRL=0x01;//enable interrupt
    TCD0.CTRLB=0x00; //oneramp mode
    TCD0.CTRLA=0x11; //set clock source and enable!
    #elif defined(MILLIS_USE_TIMERRTC)
    // to do: add support for RTC timer initialization
    #else //It's a type b timer
	/* Default Periodic Interrupt Mode */
	/* TOP value for overflow every 256 clock cycles */
	_timer->CCMP = TIME_TRACKING_TIMER_PERIOD;

	/* Enable timer interrupt */
	_timer->INTCTRL |= TCB_CAPT_bm;

	/* Clock selection -> same as TCA (F_CPU/64 -- 250kHz) */
	_timer->CTRLA = TCB_CLKSEL_CLKTCA_gc;

	/* Enable & start */
	_timer->CTRLA |= TCB_ENABLE_bm;	/* Keep this last before enabling interrupts to ensure tracking as accurate as possible */
	#endif
#endif
/*************************** DAC VREF *****************************************/
	#if defined(DAC0) && defined(DACVREF)
	VREF.CTRLA |= DACVREF;
	#endif
/*************************** ENABLE GLOBAL INTERRUPTS *************************/

	sei();
}

void setup_timers() {

    /*  TYPE A TIMER   */

    /* PORTMUX setting for TCA - don't need to set because using default */
    //PORTMUX.CTRLA = PORTMUX_TCA00_DEFAULT_gc;

    /* Enable Split Mode */
    TCA0.SPLIT.CTRLD = TCA_SPLIT_SPLITM_bm;

    //Only 1 WGM so no need to specifically set up.

    /* Period setting, 8-bit register in SPLIT mode */
    TCA0.SPLIT.LPER    = PWM_TIMER_PERIOD;
    TCA0.SPLIT.HPER    = PWM_TIMER_PERIOD;

    /* Default duty 50%, will re-assign in analogWrite() */
    //TODO: replace with for loop to make this smaller;
    TCA0.SPLIT.LCMP0 = PWM_TIMER_COMPARE;
    TCA0.SPLIT.LCMP1 = PWM_TIMER_COMPARE;
    TCA0.SPLIT.LCMP2 = PWM_TIMER_COMPARE;
    TCA0.SPLIT.HCMP0 = PWM_TIMER_COMPARE;
    TCA0.SPLIT.HCMP1 = PWM_TIMER_COMPARE;
    TCA0.SPLIT.HCMP2 = PWM_TIMER_COMPARE;

    /* Use DIV64 prescaler (giving 250kHz clock), enable TCA timer */
    TCA0.SPLIT.CTRLA = (TCA_SINGLE_CLKSEL_DIV64_gc) | (TCA_SINGLE_ENABLE_bm);


    /*    TYPE B TIMERS  */

    // No megaTinyCore parts need to change the port mapping.

    /* Start with TCB0 */
    #ifdef TCB0
    TCB_t *timer_B = (TCB_t *)&TCB0;

    /* Timer B Setup loop for TCB[0:2] */
    #ifdef TCB1
    do{
    #endif
        /* 8 bit PWM mode, but do not enable output yet, will do in analogWrite() */
        timer_B->CTRLB = (TCB_CNTMODE_PWM8_gc);

        /* Assign 8-bit period */
        timer_B->CCMPL = PWM_TIMER_PERIOD;

        /* default duty 50%, set when output enabled */
        timer_B->CCMPH = PWM_TIMER_COMPARE;

        /* Use TCA clock (250kHz) and enable */
        /* (sync update commented out, might try to synchronize later */
        timer_B->CTRLA = (TCB_CLKSEL_CLKTCA_gc)
                        //|(TCB_SYNCUPD_bm)
                        |(TCB_ENABLE_bm);

        /* Increment pointer to next TCB instance */
        timer_B++;

    /* Stop when pointing to TCB3 */
    // } while (timer_B < (TCB_t *)&TCB3);

    // ATtiny only uses up to TCB1
    /* Stop when pointing to TCB1 */
    #ifdef TCB1
    } while (timer_B <= (TCB_t *)&TCB1);
    #endif
    #endif
    #ifdef TCD0
    #if defined(USE_TIMERD0_PWM) && (!(defined(MILLIS_USE_TIMERD0_A0) || defined(MILLIS_USE_TIMERD0)))
    TCD0.CMPBCLR=255; //Count to 255 (8-bit resolution)
    TCD0.CTRLC=0x80; //WOD outputs PWM B, WOC outputs PWM A
    TCD0.CTRLB=0x03; //dual slope
    TCD0.CTRLA=0x10; //OSC20M prescaled by 32, gives ~1.2 khz PWM at 20MHz.
    #endif
    #endif

}
