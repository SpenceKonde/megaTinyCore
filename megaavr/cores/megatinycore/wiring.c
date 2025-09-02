/*
  wiring.c - Partial implementation of the Wiring API
  Originally part of Arduino - http://www.arduino.cc/
  Copyright (c) 2005-2006 David A. Mellis


  Copyright (c) 2018-2021 Spence Konde
  This has been ported to modern AVRs (Arduino team did that)
  Almost every part of it has since been rewritten for
  megaTinyCore and DxCore. This is the megaTinyCore version, and is
  part of megaTinyCore.

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
#include "util/delay.h"

void init_timers();

#ifndef F_CPU
  #error "F_CPU not defined. F_CPU must always be defined as the clock frequency in Hz"
#endif
#ifndef CLOCK_SOURCE
  #error "CLOCK_SOURCE not defined. Must be 0 for internal, 1 for crystal, or 2 for external clock"
#endif

/*           __PeripheralControl is used to mark peripherals as being "taken over" by the user
 * 0x40 = TIMERD0
 * 0x10 = TIMERA0
 * 0x08 = TIMERA1
 * Implementation and use is not portable between cores - tradeoffs are made which
 * trade generalizability for low resource use
 */
uint8_t __PeripheralControl = 0xFF;

uint8_t __MillisState = 0xFF;




#ifndef MILLIS_USE_TIMERNONE
  struct sTimeMillis {
    #if (MILLIS_TYPE == MILLIS_RTC) || (MILLIS_TYPE == MILLIS_TCB)    // TCB as millis source does not need fraction
      volatile uint32_t timer_millis;   // That's all we need to track here
    #else                               // TCAx or TCD0
      //volatile uint16_t timer_fract;
      //volatile uint32_t timer_millis;
      //volatile uint32_t timer_overflow_count;
      volatile uint32_t timer_overflow_count;
      volatile uint32_t timer_millis;
      volatile uint16_t timer_fract;
    #endif
  } timingStruct;

  struct sTimer {
    uint8_t            intClear;
    volatile uint8_t  *intStatusReg;
  };
  /* volatile uint16_t microseconds_per_timer_overflow;
   * volatile uint16_t microseconds_per_timer_tick;
   * overflow count is tracked for all timer options, even the RTC
   * need to be declared here not in timers.h */
  #if defined(MILLIS_USE_TIMERRTC)
    const struct sTimer _timerS = {RTC_OVF_bm, &RTC.INTCTRL};
  #elif defined (MILLIS_USE_TIMERA0)
    #if defined(TCA_BUFFERED_3PIN)
      const struct sTimer _timerS = {TCA_SINGLE_OVF_bm, &TCA0.SINGLE.INTFLAGS};
    #else
      const struct sTimer _timerS = {TCA_SPLIT_HUNF_bm, &TCA0.SPLIT.INTFLAGS};
    #endif
  #elif defined(MILLIS_USE_TIMERB0)
    static volatile TCB_t *_timer = &TCB0;
    const struct sTimer _timerS =   {TCB_CAPT_bm,       &TCB0.INTFLAGS};
  #elif defined(MILLIS_USE_TIMERB1)
    static volatile TCB_t *_timer = &TCB1;
    const struct sTimer _timerS =   {TCB_CAPT_bm,       &TCB1.INTFLAGS};
  #elif defined(MILLIS_USE_TIMERB2)
    static volatile TCB_t *_timer = &TCB2;
    const struct sTimer _timerS =   {TCB_CAPT_bm,       &TCB2.INTFLAGS};
  #elif defined(MILLIS_USE_TIMERB3)
    static volatile TCB_t *_timer = &TCB3;
    const struct sTimer _timerS =   {TCB_CAPT_bm,       &TCB3.INTFLAGS};
  #elif defined(MILLIS_USE_TIMERB4)
    static volatile TCB_t *_timer = &TCB4;
    const struct sTimer _timerS =   {TCB_CAPT_bm,       &TCB4.INTFLAGS};
  #elif defined(MILLIS_USE_TIMERB5)
    static volatile TCB_t *_timer = &TCB5;
    const struct sTimer _timerS =   {TCB_CAPT_bm,       &TCB5.INTFLAGS};
  #elif defined(MILLIS_USE_TIMERB6)
    static volatile TCB_t *_timer = &TCB6;
    const struct sTimer _timerS =   {TCB_CAPT_bm,       &TCB6.INTFLAGS};
  #elif defined(MILLIS_USE_TIMERB7)
    static volatile TCB_t *_timer = &TCB7;
    const struct sTimer _timerS =   {TCB_CAPT_bm,       &TCB7.INTFLAGS};
  #elif defined(MILLIS_USE_TIMERD0)
    const struct sTimer _timerS =   {TCD_OVF_bm,        &TCD0.INTFLAGS};
  #else
    #error "No millis timer selected, but not disabled - can't happen!".
  #endif  /* defined(MILLIS_USE_TIMER__) */
#endif
#include "wiring_micros_ISR.h"
/* ISRs moved to wiring_micros_ISR.h due to it's length (~200 lines) and ugliness (lots of asm) */



  /*  Both millis and micros must take great care to prevent any kind of backward time travel.
   *
   * These values are unsigned, and should not decrease, except when they overflow. Hence when
   * we compare a value with what we recorded previously and find the new value to be lower, it
   * looks the same as it would 2^32 (4.2 billion) intervals in the future. Timeouts end prematurely
   * and similar undesired behaviors occur.
   *
   * There are three hazardous things we read here:
   * timer_millis, timer_overflow_count, and the timer count itself (TCxn.CNT).
   * The normal variables need only be read with interrupts disabled, in case of an
   * interrupt writing to it while we were reading it. AVRs are little-endian, so this would result
   * in the low byte being read before the overflow and the high byte after, and hence a value
   * higher than it should be for that call. Subsequent calls would return the right value.
   *
   * In the case of the timer value, it is more complicated.
   * Here, the hardware at first glance seems to protect us (see "reading 16-bit registers" in the
   * datasheet). But the register gets read in the interrupt, so we still need those disabled.
   * There is an additional risk though that we get a value from after the timer has overflowed
   * and since we disabled interrupts, the interrupt hasn't updated the overflow. We check the
   * interrupt flag, and if it's set, we check whether the timer value we read was near overflow
   * (the specifics vary by the timer - they're very different timers). If it isn't close to overflow
   * but the flag is set, we must have read it after the overflow, so we compensate for the missed
   * interrupt. If interrupts are disabled for long enough, this heuristic will be wrong, but in
   * that case it is the user's fault, as this limitation is widely known and documentedm, as well
   * as unavoidable. Failure to compensate looks like the inverse of the above case.
   *
   * (note that only micros reads the timer, and hence, only micros can experience backwards time
   * travel due to interrupts being left disabled for too long, millis will just stop increasing.
   *
   * Both of these cause severe breakage everywhere. The first type is simple to avoid, but if
   * missed can be more subtle, since it makes a big difference only if the byte where the read
   * was interrupted rolled over. The second type is more obvious, potentially happening on every timer
   * overflow, instead of just every 256th timer overflow, and when it does happen, anything waiting
   * for a specific number of microseconds to pass that gets that value will do so.
   * Though (see delay below) each incidence only short-circuits one ms of delay(), not the whole
   * thing.
   *
   * All time time travel except for glitchs from disabling millis for too long should no longer
   * be possible. If they are, that is a critical bug.
   */

#if !defined(MILLIS_USE_TIMERNONE)
  unsigned long millis() {
    // return timer_overflow_count; // for debugging timekeeping issues where these variables are out of scope from the sketch
    unsigned long m;
    // disable interrupts while we read timer_millis or we might get an
    // inconsistent value (e.g. in the middle of a write to timer_millis)
    uint8_t oldSREG = SREG;
    cli();
    #if defined(MILLIS_USE_TIMERRTC)
      uint16_t rtccount = RTC.CNT;
      m = timingStruct.timer_millis;
      if (RTC.INTFLAGS & RTC_OVF_bm) {
        /* There has just been an overflow that hasn't been accounted for by the interrupt. Check if the high bit of counter is set.
         * We just basically need to make sure that it didn't JUST roll over at the last couple of clocks. But this merthod is
         * implemented very efficiently (just an sbrs) so it is more efficient than other approaches. If user code is leaving
         * interrupts off nearly 30 seconds, they shouldn't be surprised. */
        if (!(rtccount & 0x8000)) m += 64000;
      }
      SREG = oldSREG;
      m += rtccount - (rtccount >> 5) + (rtccount >> 7);
    #else
      m = timingStruct.timer_millis;
      SREG = oldSREG;
    #endif
    return m;
  }
#else
  unsigned long millis() {
    badCall("millis() is not available because it has been disabled through the tools -> millis()/micros() menu");
    return -1;
  }
#endif

/* micros() moved to wiring_micros_ISR.h due to it's length (~400 lines) and ugliness (lots of asm)*/

/* delay()
 * So what do you WANT in a good delay function?
 * First, obviously you want it to delay things. You do not want it to block interrupts (then a long one would throw off
 * timekeeping, miss inputs, and so on). And you want the compiled size to not be prohibitive for the part.
 * The reason it's so important wrt. interrupts is that in Arduino standard delay(), if an interrupt fires in the middle,
 * will still end at the same time - it is "interrupt insensitive". Whenever a delay is using the builtin _delay_ms()
 * if that is interrupted it has no way of knowing time has passed. Now hopefully you're not spending so much time in
 * an ISR that this is significant, but it is still undesirable.
 *
 * For the unfortunate souls using small-flash parts, the flash usage becomes a major problem - why is it such a space-hog?
 * Because it has to pull in micros(), which is bulky even with the division turned into bitshifts... RTC has same problem
 * with millis(), the conversion of 1024ths of a second to 1000ths is a killer, even with the bitshift tricks,
 * and the compiler seems really stupid about how it handles it; I can't keep it from making an extra copy of the 32-bit
 * value, which ALSO requires 4 more push and pop operations to get registers it can use.
 *
 * Now we will use one of three delay() implementations:
 * If you have 16k+ your delay is the standard one, it pulls in micros(), yes, but you may well already have grabbed
 *  that for your sketch already, and the delay is more accurate and fully interrupt insensitive, and you can afford
 *  the memory. For RTC users they will get the analogous implementation that is based on millis.
 * Users with millis disabled, or with less than 16k flash and using RTC will get the implementation based on _delay_ms().
 * Everyone else (flash under 16k but millis enabled via non-RTC timer) will get the light version which calls _delay_ms()
 *  if the delay is under 16 ms to get less flash usage, and calculates the delay using **millis** not micros otherwise,
 *  saving over 100b of flash. The reason for the split is that the limited granularity of millis introduces an error in
 *  the delay duration of up to 1ms. That doesn't matter much when you call delay(1000) on an internal clock that's within
 *  1% on a good day. It matters greatly when you call delay(1);    */

#if defined(MILLIS_USE_TIMERNONE) || (PROGMEM_SIZE < 16384 && defined(MILLIS_USE_TIMERRTC))
  void delay(uint32_t ms) { /* Interrupts will prolong this delay */
    if (__builtin_constant_p(ms)) {
      _delay_ms(ms);
    } else {
      while (ms--) {
        _delay_ms(1);
      }
    }
  }
#elif (PROGMEM_SIZE >= 16384 && !defined(MILLIS_USE_TIMERRTC))
  void delay(uint32_t ms) { /* Interrupts will not prolong this less flash-efficient delay */
    uint16_t start = (uint16_t) micros();
    while (ms > 0) {
      while (((uint16_t) micros() - start) >= 1000 && ms) {
        ms-- ;
        start += 1000;
      }
    }
  }
#else
  void delay(uint32_t ms) {
    if (__builtin_constant_p(ms) && ms < 16) {
      _delay_ms(ms);
    } else if (ms < 16) {
      while(ms--) {
        _delay_ms(1);
      }
    } else {
      uint32_t start = millis();
      while (millis() - start < ms);
    }
  }
#endif


/* delayMicroseconds() moved to wiring_micros_ISR.h due to it's length (~400 lines) and ugliness (lots of asm) */


/***********************************************************************
 * START simple millis manipulations, init
 **********************************************************************/

void stop_millis() { // Disable the interrupt:
  #if defined(MILLIS_USE_TIMERNONE)
    badCall("stop_millis() is only valid with millis time keeping enabled.");
  #else
    #if defined(MILLIS_USE_TIMERA0)
      TCA0.SPLIT.INTCTRL &= (~TCA_SPLIT_HUNF_bm);
    #elif defined(MILLIS_USE_TIMERD0)
      TCD0.INTCTRL &= 0xFE;
    #elif MILLIS_TYPE == MILLIS_RTC
      RTC.INTCTRL &= 0xFE;
      RTC.CTRLA &= 0xFE;
    #else
      _timer->INTCTRL &= ~TCB_CAPT_bm;
    #endif
      __MillisState = 0x01;
  #endif
}

void restart_millis() {
  // Call this to restart millis after it has been stopped and/or millis timer has been molested by other routines.
  // This resets key registers to their expected states.
  #if defined(MILLIS_USE_TIMERNONE)
    badCall("restart_millis() is only valid with millis time keeping enabled.");
  #else
    #if defined(MILLIS_USE_TIMERA0)
        /* The type A timers need to be restored to the state they were in at the start of restore  */
      TCA0.SPLIT.CTRLA    = 0;                    // timer off (might need that for next steps)
      TCA0.SPLIT.CTRLD    = TCA_SPLIT_SPLITM_bm;  // because this will not work if it's enabled.
      TCA0.SPLIT.HPER     = PWM_TIMER_PERIOD;     // What was left behind
      #if (F_CPU > 25000000)  // use 256 divider when clocked over 25 MHz
        TCA0.SPLIT.CTRLA   = (TCA_SPLIT_CLKSEL_DIV256_gc) | (TCA_SPLIT_ENABLE_bm);
      #elif (F_CPU > 5000000) //  use 64 divider for everything in the middle
        TCA0.SPLIT.CTRLA   =  (TCA_SPLIT_CLKSEL_DIV64_gc) | (TCA_SPLIT_ENABLE_bm);
      #elif (F_CPU > 1000000) // and use 16...
        TCA0.SPLIT.CTRLA   =  (TCA_SPLIT_CLKSEL_DIV16_gc) | (TCA_SPLIT_ENABLE_bm);
      #else                   // or even 8 otherwise for really slow system clocks.
        TCA0.SPLIT.CTRLA   =   (TCA_SPLIT_CLKSEL_DIV8_gc) | (TCA_SPLIT_ENABLE_bm);
      #endif
      /* No TCA1 on tinyAVRs */
    #elif defined(MILLIS_USE_TIMERD0)
      TCD0.CTRLA          = 0x00;
      while (TCD0.STATUS & 0x01);
    #elif (MILLIS_TYPE == MILLIS_TCB) // It's a type b timer
      _timer->CTRLB       = 0;
      #if defined(TCB_CTRLC)
        // What the bloody hell was this added for?
        _timer->CTRLC       = 0;
      #endif
    #endif
    init_millis();
  #endif
}


uint8_t _getCurrentMillisTimer() {
  #if defined(MILLIS_USE_TIMERNONE)
    return NOT_ON_TIMER;
  #elif !defined(MILLIS_TIMER)
    badCall("Can't happen! Millis defines busted");
  #else
    if (!__MillisState) {
      return MILLIS_TIMER;
    } else {
      return __MillisState;
    }
  #endif
}

void __attribute__((weak)) init_millis() {
  #if defined(MILLIS_USE_TIMERNONE)
    badCall("init_millis() is only valid with millis time keeping enabled.");
  #else
    #if defined(MILLIS_USE_TIMERA0)
      #if !defined(TCA_BUFFERED_3PIN)
        TCA0.SPLIT.INTCTRL = TCA_SPLIT_HUNF_bm;
      #else
        TCA0.SINGLE.INTCTRL = TCA_SINGLE_OVF_bm;
      #endif
    #elif defined(MILLIS_USE_TIMERD0)
      TCD_t* pTCD;
      _fastPtr_d(pTCD, &TCD0);
      pTCD->CMPBCLR        = TIME_TRACKING_TIMER_PERIOD; // essentially, this is TOP
      pTCD->CTRLB          = 0x00; // oneramp mode
      pTCD->CTRLC          = 0x80;
      pTCD->INTCTRL        = 0x01; // enable interrupt
      pTCD->CTRLA          = _TIMERD0_PRESCALER | 0x01; // set clock source and enable!
    #elif MILLIS_TYPE == MILLIS_RTC
      while(RTC.STATUS);
      RTC.PER             = TIME_TRACKING_TIMER_PERIOD;
      #if defined(MILLIS_USE_TIMERRTC_XTAL)
        _PROTECTED_WRITE(CLKCTRL.XOSC32KCTRLA,0x03);
        RTC.CLKSEL        = 2; // external crystal
      #elif defined(MILLIS_USE_TIMERRTC_OSC)
        _PROTECTED_WRITE(CLKCTRL.XOSC32KCTRLA,0x07);
        RTC.CLKSEL        = 2; // external crystal
      #elif defined(MILLIS_USE_TIMERRTC_CLK)
        RTC.CLKSEL        = 3; // external CLOCK on CLKIN
      #else
        _PROTECTED_WRITE(CLKCTRL.OSC32KCTRLA,0x02);
        // RTC.CLKSEL=0; this is the power on value
      #endif
      RTC.INTCTRL         = 0x01; // enable overflow interrupt
      RTC.CTRLA           = (RTC_RUNSTDBY_bm|RTC_RTCEN_bm|_RTC_PRESCALE_VALUE);//fire it up, prescale by 32.
      #if MEGATINYCORE_SERIES != 2 && _AVR_FLASH < 32
        // There's an erratum here. I don't think it's worth checking whrther the erratum is present, as essentially all 321x's don't have it, and everything else that's not 2-series has the bug.
        RTC.PITCTRLA      = 0x01; // turn on the PIT doing nothing (needs to be on due to errata)
      #endif
    #else // It's a type b timer - we have already errored out if that wasn't defined
      _timer->CCMP = TIME_TRACKING_TIMER_PERIOD;
      // Enable timer interrupt, but clear the rest of register
      _timer->INTCTRL = TCB_CAPT_bm;
      // Clear timer mode (since it will have been set as PWM by init())
      _timer->CTRLB = 0;
      // CLK_PER/1 is 0b00, . CLK_PER/2 is 0b01, so bitwise OR of valid divider with enable works
      _timer->CTRLA = TIME_TRACKING_TIMER_DIVIDER|TCB_ENABLE_bm;  // Keep this last before enabling interrupts to ensure tracking as accurate as possible
    #endif
    __MillisState = 0x00;
  #endif
}

void set_millis(__attribute__((unused))uint32_t newmillis) {
  #if defined(MILLIS_USE_TIMERNONE)
    badCall("set_millis() is only valid with millis timekeeping enabled.");
    (void)newmillis; // unused parameter
  #else
    #if MILLIS_TYPE == MILLIS_RTC
      uint8_t oldSREG = SREG; // save SREG
      cli();                // interrupts off
      timingStruct.timer_millis = newmillis;
      while(RTC.STATUS & RTC_CNTBUSY_bm); // wait if RTC busy
      RTC.CNT = 0;
      SREG = oldSREG; // re-enable interrupts if we killed them,
    #else
      /* farting around with micros via overflow count was ugly and buggy.
       * may implement again, better, in the future - but millis and micros
       * will get out of sync when you use set_millis
       * I think the way to do it is to make this implementation (but not big one)
       * inline, so if newmillis is constant, we can calculate the (compile-time known)
       * number of overflows using all the floating point math we want, and otherwise,
       * document that it will zero out micros.*/
      timingStruct.timer_millis = newmillis;
    #endif
  #endif
}

void nudge_millis(__attribute__((unused)) uint16_t nudgesize) {
  #if !defined(MILLIS_USE_TIMERNONE)
    uint8_t oldSREG=SREG;
    cli();
    timingStruct.timer_millis += nudgesize;
    SREG=oldSREG;
  #else
    badCall("nudge_millis() is only valid with millis timekeeping enabled.");
    (void)nudgesize; // unused parameter
  #endif
}
/***********************************************************************
 * END simple millis manipulations, init
 *******
 * START non-millis init
 **********************************************************************/

void init() {
  // Initializes hardware: First we configure the main clock, then fire up the other peripherals
  init_clock();
  init_ADC0();
  init_timers();
  #ifndef MILLIS_USE_TIMERNONE
    init_millis();
  #endif
  /*************************** ENABLE GLOBAL INTERRUPTS *************************/
  // Finally, after everything is initialized, we go ahead and enable interrupts.
  if (onAfterInit()) {
    sei();
  }
}

/***********************************************************************
 * END init
 *******
 * START clock init
 **********************************************************************/
#if defined(CLOCK_TUNE_INTERNAL)
  void tune_internal(void);
#endif

void __attribute__((weak)) init_clock() {
  #ifndef CLOCK_SOURCE
    #error "CLOCK_SOURCE not defined. CLOCK_SOURCE must be either 0 (internal) or 2 (external clock)"
  #endif
  #if (CLOCK_SOURCE == 0)
    #if (defined(CLOCK_TUNE_INTERNAL))
      tune_internal(); // Will be inlined as only called once. Just too long and ugly to put two implementations in middle of this.
    #else
      #if defined(_MCLK_CTRLB) && defined(F_CPU) && defined(_MAIN_CLOCK_PRESCALER)
        _PROTECTED_WRITE(CLKCTRL_MCLKCTRLB,_MCLK_CTRLB);
      #else
        #ifndef F_CPU
          #error "F_CPU not defined"
        #else
          #error "F_CPU defined as an unsupported value for untuned internal oscillator"
        #endif
      #endif
    #endif
  #elif (CLOCK_SOURCE == 2)
    _PROTECTED_WRITE(CLKCTRL_MCLKCTRLA, CLKCTRL_CLKSEL_EXTCLK_gc);
    // while (CLKCTRL.MCLKSTATUS & CLKCTRL_SOSC_bm);  // This either works, or hangs the chip - EXTS is pretty much useless here.
    // w/out CFD, easier to determine what happened if we don't just hang here.
    uint8_t count = 10;
    while (CLKCTRL.MCLKSTATUS & CLKCTRL_SOSC_bm && count--);
    if (CLKCTRL.MCLKSTATUS & CLKCTRL_EXTS_bm) {
      _PROTECTED_WRITE(CLKCTRL_MCLKCTRLB, _MCLK_CTRLB);

    }
  #else
    #error "CLOCK_SOURCE is defined, but it isn't 0 (internal) or 2 (external clock), and those are the only clock sources supported by this part."
  #endif
}

#if defined(CLOCK_TUNE_INTERNAL)
  void tune_internal() {
  #define _CLOCKSPERUS (F_CPU/1000000)
  uint8_t _osccfg; // magic name - do not change
  #if defined(USING_BOOTLOADER) && USING_BOOTLOADER == 1
      // If using Optiboot, then we do not know what value OSCFG was set to when it was bootloaded, so we have to determine it at runtime.
      uint8_t _osccfg = FUSE.OSCCFG - 1; /****** "_osccfg" IS A MAGIC NAME - DO NOT CHANGE IT ******/
  #else
    // if not we set this when the
    #if MEGATINYCORE_SERIES == 2 && (_CLOCKSPERUS > 20 || _CLOCKSPERUS== 12 || _CLOCKSPERUS == 10 || _CLOCKSPERUS == 6 || _CLOCKSPERUS == 5 || _CLOCKSPERUS == 3)
      _osccfg = 1;
    #elif MEGATINYCORE_SERIES < 2 && (_CLOCKSPERUS > 20 || _CLOCKSPERUS== 12 || _CLOCKSPERUS == 10 || _CLOCKSPERUS == 7 ||  _CLOCKSPERUS == 6 || _CLOCKSPERUS == 5 || _CLOCKSPERUS == 3)
      _osccfg = 1;
    #else
      _osccfg = 0;
    #endif
  #endif
  #include "tune_guesses.h"
    // The GUESSCAL, MAX_TUNING, TUNED_CALIBRATION_OFFSET and TUNE_PRESCALE symbols, which look like constants, aren't.
    // They're macros from tune_guesses.h and get replaced with (ternary operators and math involving osccfg), so what looks very simple here... actually isn't.
    // Evertthing hard is done in tune_guesses.h
    if (__builtin_constant_p(TUNED_CALIBRATION_OFFSET)) {
      if (TUNED_CALIBRATION_OFFSET == 255) {
        badCall("It appears that you are attempting to set a 0/1-series part to 32 MHz via tuning or otherwise set a bogus clock speed.");
      }
    }
    if (TUNED_CALIBRATION_OFFSET == 255) {

      GPIOR0 |= 0x80;
      GPIOR0 |= 0x40;
      return; // we can't do that speed at all with this part and oscillator setting! Hopefully users notice their sketch is running
      // way too slow, and will read the docs which contain further instructions for diagnosis of these sort of problems.
    } else {
      uint8_t istuned =(_SFR_MEM8((_osccfg ? 0x1306 : 0x1300) +  CLOCK_TUNE_START + HOME_TUNING_OFFSET)) != 255;
      uint8_t tunedval=_SFR_MEM8(((_osccfg ? 0x1306 : 0x1300) +  CLOCK_TUNE_START + TUNED_CALIBRATION_OFFSET));
      if (!istuned) {
        GPIOR0 |= 0x40;
        int temp = GUESSCAL;
        if (temp > MAX_TUNING) {             // uhoh, if we apply the default guess, we'd be setting it higher than it's maximum value!
          if (MAX_TUNING - temp > 5) {
            GPIOR0 |= 0x80;
            return; // How far away are we? If it's more than 5, give up - better to be obviously broken than non-obviously broken
          }
          tunedval = MAX_TUNING;
        } else if (temp < 0) {               // uhoh, if we apply the default guess, we'd be setting it to a negative value (which would wrap around, resulting in the value being too high..
          if (temp < -5) return;             // How far away are we? If it's more than 5, give up - better to be obviously broken than non-obviously broken
          tunedval = 0;
        } else {
          tunedval = temp;
        }
      } else if (tunedval == 0x80) {
        GPIOR0 |= 0x80;
        return; // this chip was tuned and it's oscillator found to be unable to reach target and/or the chip ceased to be opprate before reaching that speed
        // such that either the tuning sketch crashed or the incredilbly crude sanity checks found that arithmetic produced incorrect results.
      }
      // Udf
      _PROTECTED_WRITE(CLKCTRL_OSC20MCALIBA,tunedval);
      _NOP();
      _NOP();
    }
    _PROTECTED_WRITE(CLKCTRL_MCLKCTRLB, TUNE_PRESCALE);
  }
#endif


/********************************* ADC ****************************************/
void __attribute__((weak)) init_ADC0() {
  ADC_t* pADC;
  _fastPtr_d(pADC, &ADC0);
  #if MEGATINYCORE_SERIES < 2
  /* ADC clock 1 MHz to 1.25 MHz at frequencies supported by megaTinyCore
   * Unlike the classic AVRs, which demand 50~200 kHz, for these, the datasheet
   * spec's 50 kHz to 1.5 MHz. Slower clocks provide better response to high
   * impedance signals, since the sample and hold circuit will be connected
   * to the pin for longer However, we can use the SAMPLEN register to
   * compensate for this!
   * We target a sampling time of 12us, which is a little shorter than the
   * classic AVRs, but the sampling cap is is 5pf instead of 14
   * At clock speeds of 12, 24, and 25 MHz when we are forced to use an divider
   * that leaves us with a markedly slower ADC clock (~750 kHz), we instead use
   * a value of 7, giving us 8 ADC clocks or... around 12us. .
   * As of 2.3.0, this setting is exposed by analogReadDuration()
   * Note that on 0/1-series, the prescale settings are placed powers-of-two
   * apart. On the 2-series and Dx-series, they are MUCH closer together.
   **************************************************************************/
    //                              30 MHz / 32 = 937 kHz,  32 MHz / 32 =  1 MHz.
    #if   F_CPU   > 24000000     // 24 MHz / 16 = 1.5 MHz,  25 MHz / 32 =  780 kHz
      pADC->CTRLC  = ADC_PRESC_DIV32_gc | ADC_REFSEL_VDDREF_gc | ADC_SAMPCAP_bm;
    #elif F_CPU  >= 12000000    // 16 MHz / 16 = 1.0 MHz,  20 MHz / 16 = 1.25 MHz
      pADC->CTRLC  = ADC_PRESC_DIV16_gc | ADC_REFSEL_VDDREF_gc | ADC_SAMPCAP_bm;
    #elif F_CPU  >=  6000000    //  8 MHz /  8 = 1.0 MHz,  10 MHz /  8 = 1.25 MHz
      pADC->CTRLC  =  ADC_PRESC_DIV8_gc | ADC_REFSEL_VDDREF_gc | ADC_SAMPCAP_bm;
    #elif F_CPU  >=  3000000    //  4 MHz /  4 = 1.0 MHz,   5 MHz /  4 = 1.25 MHz
      pADC->CTRLC  =  ADC_PRESC_DIV4_gc | ADC_REFSEL_VDDREF_gc | ADC_SAMPCAP_bm;
    #else                       //  1 MHz /  2 = 500 kHz - the lowest setting
      pADC->CTRLC  =  ADC_PRESC_DIV2_gc | ADC_REFSEL_VDDREF_gc | ADC_SAMPCAP_bm;
    #endif
    #if   (F_CPU == 6000000 || F_CPU == 12000000 || F_CPU == 24000000 || F_CPU ==25000000)
      pADC->SAMPCTRL = (7); // 9 ADC clocks, 12 us
    #elif (F_CPU == 5000000 || F_CPU == 10000000 || F_CPU == 20000000)
      pADC->SAMPCTRL = (13);   // 15 ADC clock,s 12 us
    #else
      pADC->SAMPCTRL = (10); // 12 ADC clocks, 12 us
    #endif
    pADC->CTRLD    = ADC_INITDLY_DLY16_gc;
    pADC->CTRLA    = ADC_ENABLE_bm;
  #else
    /* On the 2-series maximum with internal reference is 3 MHz, so we will
     * target highest speed that doesn't exceed that and 16 ADC clocks sample
     * duration. */
    #if F_CPU     > 32000000             // 36 MHz /14 = 2.57 MHz
      pADC->CTRLB  = ADC_PRESC_DIV10_gc; // 33 MHz /14 = 2.35 MHz
    #elif F_CPU  >= 30000000             // 32 MHz /12 = 2.67 MHz
      pADC->CTRLB  = ADC_PRESC_DIV12_gc; // 30 MHz /12 = 2.50 MHz
    #elif F_CPU  >= 24000000             // 25 MHz /10 = 2.50 MHz
      pADC->CTRLB  = ADC_PRESC_DIV10_gc; // 24 MHz /10 = 2.40 MHz
    #elif F_CPU  >= 20000000
      pADC->CTRLB  = ADC_PRESC_DIV8_gc;  // 20 MHz / 8 = 2.50 MHz
    #elif F_CPU  >= 16000000
      pADC->CTRLB  = ADC_PRESC_DIV6_gc;  // 16 MHz / 6 = 2.67 MHz
    #elif F_CPU  >= 12000000
      pADC->CTRLB  = ADC_PRESC_DIV4_gc;  // 12 MHz / 4 = 3.00 MHz
    #elif F_CPU  >=  6000000            // 10 MHz / 4 = 2.50 MHz
      pADC->CTRLB  = ADC_PRESC_DIV4_gc;  //  8 MHz / 4 = 2.00 MHz
    #else                               //  5 MHz / 2 = 2.50 MHz
      pADC->CTRLB  = ADC_PRESC_DIV2_gc;  //  4 MHz / 2 = 2.00 MHz
    #endif                              //  1 MHz / 2 =  500 kHz
    pADC->CTRLE = 15; // 15.5 without PGA, 16 with PGA, corresponding to 7.75 or 8 us.
    pADC->CTRLA = ADC_ENABLE_bm | ADC_LOWLAT_bm;
    /* Default low latency mode on
     * Users can turn it off if they care about power consumption while ADC is on
     * and chip is awake, since these parts don't have the perverse ADC-left-on
     * behavior of classic AVRs.
     * ... Yes they do after all. Fudge. Fixed on Ex though.
     */
    pADC->CTRLC = TIMEBASE_1US; // defined in Arduino.h.
    pADC->PGACTRL = ADC_PGABIASSEL_3_4X_gc | ADC_ADCPGASAMPDUR_15CLK_gc;
    /* Note that we don't *enable* it automatically in init().
     * 3/4th bias is good up to 4 MHz CLK_ADC, 15 ADC Clocks to sample the PGA
     * up to 5 MHz, so within the regime of speeds that have to be compatible
     * with internal references, we are in the clear there. */
  #endif
}

// Must be called manually.
#ifdef ADC1
  __attribute__((weak)) void init_ADC1() {
    ADC_t* pADC;
    _fastPtr_d(pADC, &ADC1);
    //                              30 MHz / 32 = 937 kHz,  32 MHz / 32 =  1 MHz.
    #if   F_CPU   > 24000000     // 24 MHz / 16 = 1.5 MHz,  25 MHz / 32 =  780 kHz
      pADC->CTRLC  = ADC_PRESC_DIV32_gc | ADC_REFSEL_VDDREF_gc | ADC_SAMPCAP_bm;
    #elif F_CPU  >= 12000000    // 16 MHz / 16 = 1.0 MHz,  20 MHz / 16 = 1.25 MHz
      pADC->CTRLC  = ADC_PRESC_DIV16_gc | ADC_REFSEL_VDDREF_gc | ADC_SAMPCAP_bm;
    #elif F_CPU  >=  6000000    //  8 MHz /  8 = 1.0 MHz,  10 MHz /  8 = 1.25 MHz
      pADC->CTRLC  =  ADC_PRESC_DIV8_gc | ADC_REFSEL_VDDREF_gc | ADC_SAMPCAP_bm;
    #elif F_CPU  >=  3000000    //  4 MHz /  4 = 1.0 MHz,   5 MHz /  4 = 1.25 MHz
      pADC->CTRLC  =  ADC_PRESC_DIV4_gc | ADC_REFSEL_VDDREF_gc | ADC_SAMPCAP_bm;
    #else                       //  1 MHz /  2 = 500 kHz - the lowest setting
      pADC->CTRLC  =  ADC_PRESC_DIV2_gc | ADC_REFSEL_VDDREF_gc | ADC_SAMPCAP_bm;
    #endif
    #if   (F_CPU == 6000000 || F_CPU == 12000000 || F_CPU == 24000000 || F_CPU ==25000000)
      pADC->SAMPCTRL = (7); // 9 ADC clocks, 12 us
    #elif (F_CPU == 5000000 || F_CPU == 10000000 || F_CPU == 20000000)
      pADC->SAMPCTRL = (13);   // 15 ADC clock,s 12 us
    #else
      pADC->SAMPCTRL = (10); // 12 ADC clocks, 12 us
    #endif
    pADC->CTRLD    = ADC_INITDLY_DLY16_gc;
    pADC->CTRLA    = ADC_ENABLE_bm;
  }
#endif

void init_timers()  {
  init_TCA0();
  #if (defined(TCD0) && defined(USE_TIMERD0_PWM) && !defined(MILLIS_USE_TIMERD0))
    init_TCD0();
  #endif
}

#if (defined(TCD0) && defined(USE_TIMERD0_PWM) && !defined(MILLIS_USE_TIMERD0))
void __attribute__((weak)) init_TCD0() {
  TCD_t* pTCD;
  _fastPtr_d(pTCD, &TCD0);
  pTCD->CMPBCLR  = 509; // 510 counts, starts at 0, not 1!
  pTCD->CMPACLR  = 509;
  pTCD->CTRLC    = 0x80; // WOD outputs PWM B, WOC outputs PWM A
  //pTCD->CTRLB    = TCD_WGMODE_ONERAMP_gc; // One ramp mode. This is the default so we can save 2 words of flash
  pTCD->CTRLA    = _TIMERD0_PRESCALER; // OSC20M prescaled by 32, gives ~1.2 khz PWM at 20MHz.
}
#endif

void __attribute__((weak)) init_TCA0() {
  /*  TYPE A TIMER   */
  #if !defined(TCA_BUFFERED_3PIN)
    #if defined(PORTMUX_CTRLC)
      PORTMUX.CTRLC = TCA_PORTMUX;
    #else
      PORTMUX.TCAROUTEA = TCA_PORTMUX;
    #endif
    TCA0.SPLIT.CTRLD   = TCA_SPLIT_SPLITM_bm;
    TCA0.SPLIT.LPER    = PWM_TIMER_PERIOD;
    TCA0.SPLIT.HPER    = PWM_TIMER_PERIOD;
    TCA0.SPLIT.CTRLA   = (_TIMERA_PRESCALER | TCA_SPLIT_ENABLE_bm);
  #else
    #if defined(PORTMUX_CTRLC)
      PORTMUX.CTRLC = TCA_PORTMUX;
    #else
      PORTMUX.TCAROUTEA = TCA_PORTMUX;
    #endif
    TCA0.SINGLE.PER    = PWM_TIMER_PERIOD;
    TCA0.SINGLE.CTRLB  = TCA_SINGLE_WGMODE_SINGLESLOPE_gc;
    TCA0.SINGLE.CTRLA  = (_TIMERA_PRESCALER | TCA_SINGLE_ENABLE_bm);
  #endif

}

__attribute__((weak)) void onPreMain() {;}
__attribute__((weak)) void onBeforeInit() {;}
__attribute__((weak)) uint8_t onAfterInit() {return 1;}
