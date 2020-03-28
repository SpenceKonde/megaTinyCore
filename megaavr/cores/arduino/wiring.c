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

#ifdef MILLIS_USE_TIMERD0_A0
#ifdef TCD0
#define MILLIS_USE_TIMERD0
#else
#define MILLIS_USE_TIMERA0
#endif
#endif
#ifdef MILLIS_USE_TIMERRTC_XTAL
#define MILLIS_USE_TIMERRTC
#endif

//volatile uint16_t microseconds_per_timer_overflow;
//volatile uint16_t microseconds_per_timer_tick;

#if (defined(MILLIS_USE_TIMERB0)  || defined(MILLIS_USE_TIMERB1) ) //Now TCB as millis source does not need fraction
volatile uint32_t timer_millis = 0; //That's all we need to track here

#elif !defined(MILLIS_USE_TIMERRTC) //all of this stuff is not used when the RTC is used as the timekeeping timer
static uint16_t timer_fract = 0;
uint16_t fract_inc;
volatile uint32_t timer_millis = 0;
#define FRACT_MAX (1000)
#define FRACT_INC (clockCyclesToMicroseconds(TIME_TRACKING_CYCLES_PER_OVF)%1000);
#define MILLIS_INC (clockCyclesToMicroseconds(TIME_TRACKING_CYCLES_PER_OVF)/1000);
volatile uint32_t timer_overflow_count = 0;
#else
volatile uint16_t timer_overflow_count = 0;
#endif

//overflow count is tracked for all timer options, even the RTC


#if !defined(MILLIS_USE_TIMERRTC)




inline uint16_t clockCyclesPerMicrosecond(){
#ifdef MILLIS_USE_TIMERD0
#if (F_CPU==20000000UL || F_CPU==10000000UL ||F_CPU==5000000UL)
	return ( 20 ); //this always runs off the 20MHz oscillator
#else
	return ( 16 );
#endif
#else
	return ( (F_CPU) / 1000000L );
#endif
}


inline unsigned long clockCyclesToMicroseconds(unsigned long cycles){
	return ( cycles / clockCyclesPerMicrosecond() );
}

inline unsigned long microsecondsToClockCycles(unsigned long microseconds){
	return ( microseconds * clockCyclesPerMicrosecond() );
}



#if defined(MILLIS_USE_TIMERD0)
#ifndef TCD0
#error "Selected millis timer, TCD0, only exists on 1-series parts"
#endif
#elif !defined(MILLIS_USE_TIMERA0)
static volatile TCB_t* _timer =
#if defined(MILLIS_USE_TIMERB0)
&TCB0;
#elif defined(MILLIS_USE_TIMERB1)
#ifndef TCB1
#error "Selected millis timer, TCB1 does not exist on this part."
#endif
&TCB1;
#else  //it's not TCB0, TCB1, TCD0, TCA0, or RTC
#error "No millis timer selected, but not disabled - can't happen!".
#endif
#endif

#endif //end #if !defined(MILLIS_USE_TIMERRTC)


#if defined(MILLIS_USE_TIMERA0)
ISR(TCA0_LUNF_vect)
#elif defined(MILLIS_USE_TIMERD0)
ISR(TCD0_OVF_vect)
#elif defined(MILLIS_USE_TIMERRTC)
ISR(RTC_CNT_vect)
#elif defined(MILLIS_USE_TIMERB0)
ISR(TCB0_INT_vect)
#elif defined(MILLIS_USE_TIMERB1)
ISR(TCB1_INT_vect)
#else
#error "no millis timer selected"
#endif
{
  VPORTA.IN|=0x80;
	// copy these to local variables so they can be stored in registers
	// (volatile variables must be read from memory on every access)

    #if (defined(MILLIS_USE_TIMERB0)|defined(MILLIS_USE_TIMERB1))
	timer_millis++; //that's all we need to do!
    #else
    #if !defined(MILLIS_USE_TIMERRTC) //TCA0 or TCD0
	uint32_t m = timer_millis;
	uint16_t f = timer_fract;
	m += MILLIS_INC;
	f += FRACT_INC;
	if (f >= FRACT_MAX) {

		f -= FRACT_MAX;
		m += 1;
	}
	timer_fract = f;
	timer_millis = m;
	#endif
	//if RTC is used as timer, we only increment the overflow count
	timer_overflow_count++;
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
  VPORTA.IN|=0x80;
}

unsigned long millis()
{
	//return timer_overflow_count; //for debugging timekeeping issues where these variables are out of scope from the sketch
	unsigned long m;

	// disable interrupts while we read timer0_millis or we might get an
	// inconsistent value (e.g. in the middle of a write to timer0_millis)
	uint8_t status = SREG;
	cli();
	#if defined(MILLIS_USE_TIMERRTC)
	m=timer_overflow_count;
	if (RTC.INTFLAGS & RTC_OVF_bm) { //there has just been an overflow that hasn't been accounted for by the interrupt
		m++;
	}
	SREG = status;
	m=(m<<16);
	m+=RTC_CNT;
	//now correct for there being 1000ms to the second instead of 1024
	m=m-(m>>5)-(m>>6);
	#else
	m = timer_millis;

	SREG = status;
	#endif
	return m;
}

#ifndef MILLIS_USE_TIMERRTC
unsigned long micros() {

	unsigned long overflows, microseconds;

	#if (defined(MILLIS_USE_TIMERD0)||defined(MILLIS_USE_TIMERB0)||defined(MILLIS_USE_TIMERB1))
	uint16_t ticks;
	#else
	uint8_t ticks;
	#endif

	/* Save current state and disable interrupts */
	uint8_t status = SREG;
	cli();

	/* Get current number of overflows and timer count */
	#if !(defined(MILLIS_USE_TIMERB0)  || defined(MILLIS_USE_TIMERB1) )
	overflows = timer_overflow_count;
	#else
	overflows=timer_millis;
	#endif
	#if defined(MILLIS_USE_TIMERA0)
	ticks = 0xFF-TCA0.SPLIT.LCNT;
	#elif defined(MILLIS_USE_TIMERD0)
	TCD0.CTRLE=TCD_SCAPTUREA_bm;
	while(!(TCD0.STATUS&TCD_CMDRDY_bm)); //wait for sync - should be only one iteration of this loop
	ticks=TCD0.CAPTUREA;
	#else
	ticks = _timer->CNT;
	#endif //end getting ticks

	/* If the timer overflow flag is raised, we just missed it,
	increment to account for it, & read new ticks */
	#if defined(MILLIS_USE_TIMERD0)
	if ((TCD0.INTFLAGS & TCD_OVF_bm) && (ticks < TIME_TRACKING_TIMER_PERIOD)){
		TCD0.CTRLE=TCD_SCAPTUREA_bm;
		while(!(TCD0.STATUS & TCD_CMDRDY_bm)); //wait for sync - should be only one iteration of this loop
		overflows++;
	}
	#elif defined(MILLIS_USE_TIMERA0)
	if ((TCA0.SPLIT.INTFLAGS & TCA_SPLIT_LUNF_bm ) && (ticks < TIME_TRACKING_TIMER_PERIOD)){
		overflows++;

	}
	#else //timerb
	if (((_timer->INTFLAGS & TCB_CAPT_bm) && (ticks < ((F_CPU/1000)-1)))) {
		ticks = _timer->CNTL; //if it just overflowed, then high byte should be 0 anyway!
		overflows++;
	}
	#endif //end getting ticks

	/* Restore state */
	SREG = status;

	#if defined(MILLIS_USE_TIMERD0)
	#if (F_CPU==20000000UL || F_CPU==10000000UL || F_CPU==5000000UL)
	microseconds = ((overflows * (TIME_TRACKING_CYCLES_PER_OVF/(20)))
				+ (ticks * ((TIME_TRACKING_CYCLES_PER_OVF)/(20)/TIME_TRACKING_TIMER_PERIOD)));
	#else
	microseconds = ((overflows * (TIME_TRACKING_CYCLES_PER_OVF/(16)))
				+ (ticks * ((TIME_TRACKING_CYCLES_PER_OVF)/(16)/TIME_TRACKING_TIMER_PERIOD)));
	#endif
    #elif (defined(MILLIS_USE_TIMERB0)||defined(MILLIS_USE_TIMERB1))
    //20MHz-derived clocks, microsecond part is 0.4% slow. 16MHz derived ones dead-on.
	#if (F_CPU==20000000UL)
	ticks=ticks>>3;
	microseconds = overflows*1000+ticks-(ticks>>2)+(ticks>>4)-(ticks>>6);
	#elif (F_CPU==10000000UL)
	ticks=ticks>>2;
	microseconds = overflows*1000+ticks-(ticks>>2)+(ticks>>4)-(ticks>>6);
	#elif (F_CPU==5000000UL)
	ticks=ticks>>1;
	microseconds = overflows*1000+ticks-(ticks>>2)+(ticks>>4)-(ticks>>6);
	#elif (F_CPU==16000000UL)
	microseconds = overflows*1000+(ticks>>3);
	#elif (F_CPU==8000000UL)
	microseconds = overflows*1000+(ticks>>2);
	#elif (F_CPU==4000000UL)
	microseconds = overflows*1000+(ticks>>1);
	#else //(F_CPU==1000000UL - here clock is running at system clock instead of half system clock.
	microseconds = overflows*1000+ticks;
	#endif //end of speed-specific implementations

	#else //TCA0
	/*
	#if (F_CPU==20000001UL && TIME_TRACKING_TICKS_PER_OVF==256 && TIME_TRACKING_TIMER_DIVIDER==64)
	microseconds=(overflows<<8)+ticks;
	microseconds=microseconds*3-(microseconds>>4)+(microseconds>>2);
	#elif (F_CPU==10000000UL && TIME_TRACKING_TICKS_PER_OVF==256 && TIME_TRACKING_TIMER_DIVIDER==64)
	microseconds=(overflows<<8)+ticks;
	microseconds=microseconds*6-(microseconds>>3)+(microseconds>>1);
	#elif (F_CPU==5000000UL && TIME_TRACKING_TICKS_PER_OVF==256 && TIME_TRACKING_TIMER_DIVIDER==64)
	microseconds=(overflows<<8)+ticks;
	microseconds=(microseconds*12-(microseconds>>2))+microseconds;
	#else //fallback
	#warning "fallback"
	*/
	//microseconds = ((overflows * clockCyclesToMicroseconds(TIME_TRACKING_CYCLES_PER_OVF))
	//			+ (ticks * (clockCyclesToMicroseconds(TIME_TRACKING_CYCLES_PER_OVF)/TIME_TRACKING_TIMER_PERIOD)));
	microseconds = ((overflows * (TIME_TRACKING_CYCLES_PER_OVF/(F_CPU/1000000UL)))
				+ (ticks * ((TIME_TRACKING_CYCLES_PER_OVF)/(F_CPU/1000000UL)/TIME_TRACKING_TIMER_PERIOD))
				);
	//#endif //end of other timer speed specific micros calculations
	#endif //end of timer-specific part of micros calculations
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

    #if defined(MILLIS_USE_TIMERA0)
    TCA0.SPLIT.INTCTRL = TCA_SPLIT_LUNF_bm;
    #elif defined(MILLIS_USE_TIMERD0)
    TCD0.CMPBCLR=TIME_TRACKING_TIMER_PERIOD; //essentially, this is TOP
    TCD0.INTCTRL=0x01;//enable interrupt
    TCD0.CTRLB=0x00; //oneramp mode
    TCD0.CTRLA=0x11; //set clock source and enable!
    #elif defined(MILLIS_USE_TIMERRTC)
    // to do: add support for RTC timer initialization
    RTC.PER=0xFFFF;
    #ifdef MILLIS_USE_TIMERRTC_XTAL
    _PROTECTED_WRITE(CLKCTRL.XOSC32KCTRLA,0x03);
    RTC.CLKSEL=2; //external crystal
    #else
    _PROTECTED_WRITE(CLKCTRL.OSC32KCTRLA,0x02);
    //RTC.CLKSEL=0; this is the power on value
    #endif
    RTC.INTCTRL=0x01; //enable overflow interupt
    RTC.CTRLA=0xA9;  //fire it up, prescale by 32.

    #else //It's a type b timer
	//Periodic interrupt mode - count to 500 per 1MHz
    #if (F_CPU>1000000)
	_timer->CCMP = (F_CPU/2000)-1;

	/* Enable timer interrupt */
	_timer->INTCTRL |= TCB_CAPT_bm;

	/* Clock selection: CLK_PER/2, Count mode: Periodic Interrupt Mode*/
	_timer->CTRLA = TCB_CLKSEL_CLKDIV2_gc|TCB_ENABLE_bm;	/* Keep this last before enabling interrupts to ensure tracking as accurate as possible */
	#else //1MHz
	//1000 system clocks per OVF...
	_timer->CCMP = (F_CPU/1000)-1;

	/* Enable timer interrupt */
	_timer->INTCTRL |= TCB_CAPT_bm;

	/* Clock selection: CLK_PER/2, Count mode: Periodic Interrupt Mode*/
	_timer->CTRLA = TCB_CLKSEL_CLKDIV1_gc|TCB_ENABLE_bm;	/* Keep this last before enabling interrupts to ensure tracking as accurate as possible */
	#endif
	#endif
#endif //end #ifndef DISABLEMILLIS
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
    #if (F_CPU > 5000000) //use 64 divider
    TCA0.SPLIT.CTRLA = (TCA_SPLIT_CLKSEL_DIV64_gc) | (TCA_SINGLE_ENABLE_bm);
    #elif (F_CPU > 1000000)
    TCA0.SPLIT.CTRLA = (TCA_SPLIT_CLKSEL_DIV16_gc) | (TCA_SINGLE_ENABLE_bm);
    #else //TIME_TRACKING_TIMER_DIVIDER==8
    TCA0.SPLIT.CTRLA = (TCA_SPLIT_CLKSEL_DIV8_gc) | (TCA_SINGLE_ENABLE_bm);
    PORTA.DIRSET=0x80;
    #endif


    /*    TYPE B TIMERS  */

    // No megaTinyCore parts need to configure this unless used for millis
    #ifdef TCD0
    #if (defined(USE_TIMERD0_PWM) && (!(defined(MILLIS_USE_TIMERD0_A0) || defined(MILLIS_USE_TIMERD0))))
    TCD0.CMPBCLR=255; //Count to 255 (8-bit resolution)
    TCD0.CTRLC=0x80; //WOD outputs PWM B, WOC outputs PWM A
    TCD0.CTRLB=0x03; //dual slope
    TCD0.CTRLA=0x10; //OSC20M prescaled by 32, gives ~1.2 khz PWM at 20MHz.
    #endif
    #endif

}
