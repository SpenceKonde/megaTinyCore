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

// the prescaler is set so that timer ticks every 64 clock cycles, and the
// the overflow handler is called every 256 ticks.


inline uint16_t clockCyclesPerMicrosecond() {
  return ((F_CPU) / 1000000L);
}

inline unsigned long clockCyclesToMicroseconds(unsigned long cycles) {
  return (cycles / clockCyclesPerMicrosecond());
}

inline unsigned long microsecondsToClockCycles(unsigned long microseconds) {
  return (microseconds * clockCyclesPerMicrosecond());
}


#ifndef MILLIS_USE_TIMERNONE


// volatile uint16_t microseconds_per_timer_overflow;
// volatile uint16_t microseconds_per_timer_tick;

#if (defined(MILLIS_USE_TIMERB0)  || defined(MILLIS_USE_TIMERB1)) // Now TCB as millis source does not need fraction
  volatile uint32_t timer_millis = 0; // That's all we need to track here

#elif !defined(MILLIS_USE_TIMERRTC) // all of this stuff is not used when the RTC is used as the timekeeping timer
  static uint16_t timer_fract = 0;
  uint16_t fract_inc;
  volatile uint32_t timer_millis = 0;
  #define FRACT_MAX (1000)
  #define FRACT_INC (millisClockCyclesToMicroseconds(TIME_TRACKING_CYCLES_PER_OVF)%1000);
  #define MILLIS_INC (millisClockCyclesToMicroseconds(TIME_TRACKING_CYCLES_PER_OVF)/1000);
  volatile uint32_t timer_overflow_count = 0;
#else
  volatile uint16_t timer_overflow_count = 0;
#endif

// overflow count is tracked for all timer options, even the RTC

#if !defined(MILLIS_USE_TIMERRTC)


// when TCD0 is used as millis source, this will be different from above, but 99 times out of 10, when a piece of code asks for clockCyclesPerMicrosecond(), they're asking about CLK_PER/CLK_MAIN/etc, not the unprescaled TCD0!
inline uint16_t millisClockCyclesPerMicrosecond() {
  #ifdef MILLIS_USE_TIMERD0
    #if (F_CPU == 20000000UL || F_CPU == 10000000UL ||F_CPU == 5000000UL)
      return (20);   // this always runs off the 20MHz oscillator
    #else
      return (16);
    #endif
  #else
    return ((F_CPU) / 1000000L);
  #endif
}


inline unsigned long millisClockCyclesToMicroseconds(unsigned long cycles) {
  return (cycles / millisClockCyclesPerMicrosecond());
}

inline unsigned long microsecondsToMillisClockCycles(unsigned long microseconds) {
  return (microseconds * millisClockCyclesPerMicrosecond());
}


#if defined(MILLIS_USE_TIMERD0)
  #ifndef TCD0
    #error "Selected millis timer, TCD0, only exists on 1-series parts"
  #endif
#elif !defined(MILLIS_USE_TIMERA0)
  static volatile TCB_t *_timer =
  #if defined(MILLIS_USE_TIMERB0)
    &TCB0;
  #elif defined(MILLIS_USE_TIMERB1)
    #ifndef TCB1
      #error "Selected millis timer, TCB1 does not exist on this part."
    #endif
    &TCB1;
  #elif defined(MILLIS_USE_TIMERB2)
    #ifndef TCB2
      #error "Selected millis timer, TCB2 does not exist on this part."
    #endif
    &TCB2;
  #elif defined(MILLIS_USE_TIMERB3)
    #ifndef TCB3
      #error "Selected millis timer, TCB3 does not exist on this part."
    #endif
    &TCB3;
  #elif defined(MILLIS_USE_TIMERB4)
    #ifndef TCB4
      #error "Selected millis timer, TCB4 does not exist on this part."
    #endif
    &TCB4;
  #else  // it's not TCB0, TCB1, TCD0, TCA0, or RTC
    #error "No millis timer selected, but not disabled - can't happen!".
  #endif
#endif

#endif // end #if !defined(MILLIS_USE_TIMERRTC)


#if defined(MILLIS_USE_TIMERA0)
  ISR(TCA0_HUNF_vect)
#elif defined(MILLIS_USE_TIMERD0)
  ISR(TCD0_OVF_vect)
#elif defined(MILLIS_USE_TIMERRTC)
  ISR(RTC_CNT_vect)
#elif defined(MILLIS_USE_TIMERB0)
  ISR(TCB0_INT_vect)
#elif defined(MILLIS_USE_TIMERB1)
  ISR(TCB1_INT_vect)
#elif defined(MILLIS_USE_TIMERB2)
  ISR(TCB2_INT_vect)
#elif defined(MILLIS_USE_TIMERB3)
  ISR(TCB3_INT_vect)
#elif defined(MILLIS_USE_TIMERB4)
  ISR(TCB4_INT_vect)
#else
  #error "No millis timer selected, but not disabled - cannot determine millis vector"
#endif
{
  // copy these to local variables so they can be stored in registers
  // (volatile variables must be read from memory on every access)

  #if (defined(MILLIS_USE_TIMERB0) || defined(MILLIS_USE_TIMERB1) || defined(MILLIS_USE_TIMERB2) || defined(MILLIS_USE_TIMERB3) || defined(MILLIS_USE_TIMERB4))
    #if (F_CPU > 2000000)
      timer_millis++; // that's all we need to do!
    #else
      timer_millis += 2;
    #endif
  #else
    #if !defined(MILLIS_USE_TIMERRTC) // TCA0 or TCD0
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
    // if RTC is used as timer, we only increment the overflow count
    // Overflow count isn't used for TCB's
    timer_overflow_count++;
  #endif
  /* Clear flag */
  #if defined(MILLIS_USE_TIMERA0)
    TCA0.SPLIT.INTFLAGS = TCA_SPLIT_HUNF_bm;
  #elif defined(MILLIS_USE_TIMERD0)
    TCD0.INTFLAGS = TCD_OVF_bm;
  #elif defined(MILLIS_USE_TIMERRTC)
    RTC.INTFLAGS = RTC_OVF_bm;
  #else // timerb
    _timer->INTFLAGS = TCB_CAPT_bm;
  #endif
}

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


unsigned long millis() {
  // return timer_overflow_count; // for debugging timekeeping issues where these variables are out of scope from the sketch
  unsigned long m;
  uint8_t oldSREG = SREG;
  cli();
  #if defined(MILLIS_USE_TIMERRTC)
    uint16_t rtccount = RTC.CNT;
    m = timer_overflow_count;
    if (RTC.INTFLAGS & RTC_OVF_bm) {
      /* There has just been an overflow that hasn't been accounted for by the interrupt. Check if the high bit of counter is set.
       * We just basically need to make sure that it didn't JUST roll over at the last couple of clocks. But this merthod is
       * implemented very efficiently (just an sbrs) so it is more efficient than other approaches. If user code is leaving
       * interrupts off nearly 30 seconds, they shouldn't be surprised. */
      if (!(rtccount & 0x8000)) m++;
    }
    SREG = oldSREG;
    m = (m << 16);
    m += rtccount;
    m = m - (m >> 5) + (m >> 7);
    /* the compiler is incorrigible - it cannot be convinced not to copy m twice, shifting one 7 times and the other 5 times
     * and wasting 35 clock cycles and several precious instruction words.
     * What you want is for it to make one copy of m, shift it right 5 places, subtract, then rightshift it 2 more.
     */
    /* Would this work?
    uint32_t temp;
    uint8_t cnt;
    __asm__ __volatile__ (
      "movw %A1, %A0     \n\t"
      "movw %C1, %C0     \n\t" //make our copy
      "ldi %2, 5"       "\n\t"
      "lsr %D1"         "\n\t"
      "ror %C1"         "\n\t"
      "ror %B1"         "\n\t"
      "ror %A1"         "\n\t"
      "dec %2"          "\n\t"
      "brne .-12"       "\n\t"
      "sub %A0, %A1"    "\n\t"
      "subc %B0, %B1"   "\n\t"
      "subc %C0, %C1"   "\n\t"
      "subc %D0, %D1"   "\n\t"
      "ldi %2, 2"       "\n\t"
      "lsr %D1"         "\n\t"
      "ror %C1"         "\n\t"
      "ror %B1"         "\n\t"
      "ror %A1"         "\n\t"
      "dec %2"          "\n\t"
      "brne .-12"       "\n\t"
      "add %A0, %A1"    "\n\t"
      "adc %B0, %B1"    "\n\t"
      "adc %C0, %C1"    "\n\t"
      "adc %D0, %D1"    "\n\t"
      : "+r" (m), "+r" (temp), "+d" (cnt)
      );
    */
  #else
    m = timer_millis;
    SREG = oldSREG;
  #endif
  return m;
}
#ifndef MILLIS_USE_TIMERRTC

  unsigned long micros() {
    unsigned long overflows, microseconds;
    #if (defined(MILLIS_USE_TIMERD0) || defined(MILLIS_USE_TIMERB0) || defined(MILLIS_USE_TIMERB1) || defined(MILLIS_USE_TIMERB2) || defined(MILLIS_USE_TIMERB3) || defined(MILLIS_USE_TIMERB4))
      uint16_t ticks;
    #else
      uint8_t ticks;
    #endif
    uint8_t flags;
    /* Save current state and disable interrupts */
    uint8_t oldSREG = SREG;
    cli(); /* INTERRUPTS OFF */
    #if defined(MILLIS_USE_TIMERA0)
      ticks = TCA0.SPLIT.HCNT;
      flags = TCA0.SPLIT.INTFLAGS;
    #elif defined(MILLIS_USE_TIMERD0)
      TCD0.CTRLE = TCD_SCAPTUREA_bm;
      while (!(TCD0.STATUS & TCD_CMDRDY_bm)); // wait for sync - should be only one iteration of this loop
      flags = TCD0.INTFLAGS;
      ticks = TCD0.CAPTUREA;
    #else
      ticks = _timer->CNT;
      flags = _timer->INTFLAGS;
    #endif // end getting ticks
    /* If the timer overflow flag is raised, and the ticks we read are low, then the timer has rolled over but
     * ISR has not fired. If we already read a high value of ticks, either we read it just before the overflow,
     * so we shouldn't increment overflows, or interrupts are disabled and micros isn't expected to work so it
     * doesn't matter.
     * Get current number of overflows and timer count */
    #if !(defined(MILLIS_USE_TIMERB0) || defined(MILLIS_USE_TIMERB1) || defined(MILLIS_USE_TIMERB2) || defined(MILLIS_USE_TIMERB3) || defined(MILLIS_USE_TIMERB4))
      overflows = timer_overflow_count;
    #else
      overflows = timer_millis;
    #endif
    /* Turn interrupts back on, assuming they were on when micros was called. */
    SREG = oldSREG; /* INTERRUPTS ON */
    #if defined(MILLIS_USE_TIMERD0)
      if ((flags & TCD_OVF_bm) && (ticks < 0x07)) {
    #elif defined(MILLIS_USE_TIMERA0)
      ticks = (TIME_TRACKING_TIMER_PERIOD) - ticks;
      if ((flags & TCA_SPLIT_HUNF_bm) && (ticks < 0x4)) {
    #else // timerb
      if ((flags & TCB_CAPT_bm) && !(ticks & 0xFF00)) {
    #endif
        #if (defined(MILLIS_USE_TIMERB0) || defined(MILLIS_USE_TIMERB1) || defined(MILLIS_USE_TIMERB2) || defined(MILLIS_USE_TIMERB3) || defined(MILLIS_USE_TIMERB4)) && !(F_CPU > 2000000UL)
          overflows +=2;
        #else
          overflows++;
        #endif
      } // end getting ticks

    #if defined(MILLIS_USE_TIMERD0)
      #if (F_CPU == 20000000UL || F_CPU == 10000000UL || F_CPU == 5000000UL)
        uint8_t ticks_l = ticks >> 1;
        ticks = ticks + ticks_l + ((ticks_l >> 2) - (ticks_l >> 4) + (ticks_l >> 7));
        // + ticks +(ticks>>1)+(ticks>>3)-(ticks>>5)+(ticks>>8))
        // speed optimization via doing math with smaller datatypes, since we know high byte is 1 or 0.
        microseconds =   overflows * (TIME_TRACKING_CYCLES_PER_OVF / 20) + ticks; // ticks value corrected above.
      #else
        microseconds = ((overflows * (TIME_TRACKING_CYCLES_PER_OVF / 16))
                          + (ticks * (TIME_TRACKING_CYCLES_PER_OVF / 16 / TIME_TRACKING_TIMER_PERIOD)));
      #endif
      #if defined(CLOCK_TUNE_INTERNAL) && !(F_CPU == 16000000UL || F_CPU ==  20000000UL || F_CPU ==  8000000UL || F_CPU ==  10000000UL || F_CPU ==  4000000UL || F_CPU ==  5000000UL)
        #warning "TCD is not supported as a millis timing source when the oscillator is tuned to a frequency other than 16 or 20 MHz. Timing results will be wrong - use TCA0 or a TCB."
      #endif
    #elif (defined(MILLIS_USE_TIMERB0) || defined(MILLIS_USE_TIMERB1) || defined(MILLIS_USE_TIMERB2) || defined(MILLIS_USE_TIMERB3) || defined(MILLIS_USE_TIMERB4))
      /* Ersatz Division for TCBs - now with inline assembly!
       *
       * It's well known that division is an operator you want to avoid like the plague on AVR.
       * Not only is it slow, the execution time isn't even easy to analyze - it depends on the
       * two opperands, particularly the divisor... so you can't just look at the generated
       * assembly and count clock cycles, you've got to either time it expoerimentally with
       * a representative set of sample data, or know how many times it will pass through the
       * loops and then count clock cycles. If the operands aren't constant (if they are, you
       * can probably manage to get it optimized away at compile time) your best hope is likely
       * simulation, assuming you know enough about the values it will end up having to divide.
       *
       * Anyway. You don't want to be doing division. But that's what you need in order to
       * appropriately scale the ick count from the prescaler-deprived TCBs. Since many users
       * reconfigure the TCA for advanced PWM, using the TCA-prescaled clock was a non-starter
       * particularly since many parts have only one TCA. But division can be approximated
       * very closely if the divisor is constant using a series of bitshifts and addition/subtraction.
       *
       * The series of shifts was determined numerically with a spreadsheet that calculated the results for
       * each value that could come from the initial round of rightshifts for any combination of
       * bitshifts and provided a number of statistics to select based on. Bacwards time travel must
       * never happenb, or if it does, it must be a shorter backward step than micros runtime - 1 us
       * otherwise delay() will break and timeouts can instantly expire when it is hit. Similarly,
       * one wants to avoid large jumps forward, and cases where more consecutive "actual" times
       * than absolutely necessary return the same value (time should flow at a constant rate).
       * Finally, the artifacts of the calculation that are unavoidable should be distributed uniformly.
       * Undershooting or overshooting 999 endpoint at the counter's maximum value is the usual
       * source of large jumps (at the overflow point) in either direction. Terms should, as much as
       * possible alternate between positive and negative to minimize artifacs.
       *
       * The most popular/important speeds are hand-implemented in assembly because the compiler
       * was doing a miserable job of it - wasting 20-30% of the execution time and it's one of the few
       * Arduino API functions that users will be surprised and dismayed to find running slowiy.
       * Not only does it run faster on "normal" boards (the 16 MHz clock eliminates the need to divide
       * DxCore offers many speeds where the math doesn't all optimize away to nothing like it does at
       * 1/2/4/8/16/32.
       *
       * Do notice that we are replacing a smaller number of terms, and it's still much faster
       * The 10's went from 5 term ersatz-division to 6, while 12's went from 5 terms to 9, yet still
       * got a lot faster. The terrible twelves are the frequency most difficult to do this with.
       * Ironically, one of the the two that are is easiest is 36, which is good enough with 3 and
       * effectively exact (That "middle 12" is closer than the other 12's get with 9!)
       * 25 also matches it. Maybe it's something 25 and 36 being perfect squares?
       *
       * The three problems were that:
       * 1. Compiler generated code stubbornly insisted doing repeated shift operation in a loop
       * with 3 cycle per iteration (the shift itself took only 2)
       * 2. Compiler could not be convinced to do things that we know will always be < 255 as
       * bytes. Sure, it wouldn't know that - it's not legal for it to do that on it's own.
       * But even when I cast everything to uint8_t, it would shift a 16-bit value around
       * unnecessarily.
       * 3. It would distribute the ticks >> 4. That is, it wouldn't shift the value of
       * ticks in place, even though it wasn't referenced after this because I was assigning
       * the result to ticks, and hence the old value was "dead"
       * Instead, it would copy it, shift the copy 3 or 4 places. Then when it needed the
       * ticks >> 2, it would make a copy of the ORIGINAL and shift that 6 places,
       * instead of copying the copy and shifting just 2 places.
       *
       * A general trend seems to be that the compiler is not smart enough to "reuse" an
       * existing value that has already been shifted such that it's closer to the target.
       * at least for multi-byte variables. This is not the worst example of it I've run into
       * but the micros() function is a little bit sensitive to the execution time.
       * Apparently people sometimes want to *do something* in response to the value it
       * returns - and they seem to want to do that in a timely manner, otherwise they'd
       * have not bothered to record a time so accurately...
       *
       * general algorithm in the assembly implementations is:
       * start with ticks in a register pair, copy to r0, r1.
       * rightshift it until we have the 0th term (closest power of 2).
       * copy it to back to original location..
       * continue rightshifting it, adding or subtracting from the original when we reach
       * the appropriate terms.
       * As soon as we've rightshifted the original enough times that we know it's < 256,
       * we switch from lsr r1 ror r0 to just lsr r0. At the next term that we want to add
       * we copy it to r1. Subsequent subtractions or additions are single-byte until we've got the last term.
       * this time, we add r1 to r0 instead of the other way around.
       * we will need to clear r1 anyway, but we do it now, since we need a known 0 to do the carry.
       * we addthat to the ticks intermediate value to get the final ticks value, and drop back into C
       * where we calculate overflows * 1000, the (now 0-999) ticks to it, and return it.
       *
       */
        // Oddball clock speeds
      #if   (F_CPU == 44000000UL) // Extreme overclocking
        ticks = ticks >> 4;
        microseconds = overflows * 1000 + (ticks - /* (ticks >> 1)  + */ (ticks >> 2) - (ticks >> 5) + /* (ticks >> 6) - */ (ticks >> 7)); // + (ticks >> 10)
      #elif (F_CPU == 36000000UL) // 50% overclock!
        ticks = ticks >> 4;
        microseconds = overflows * 1000 + (ticks - (ticks >> 3) + (ticks >> 6)); // - (ticks >> 9) + (ticks >> 10) // with 5 terms it is DEAD ON
      #elif (F_CPU == 28000000UL) // Not supported by DxCore - nobody wants it.
        ticks = ticks >> 4;
        microseconds = overflows * 1000 + (ticks + (ticks >> 2) - (ticks >> 3) + (ticks >> 5) - (ticks >> 6)); // + (ticks >> 8) - (ticks >> 9)
      #elif (F_CPU == 14000000UL) // Not supported by DxCore - nobody wants it.
        ticks = ticks >> 3;
        microseconds = overflows * 1000 + (ticks + (ticks >> 2) - (ticks >> 3) + (ticks >> 5) - (ticks >> 6)); // + (ticks >> 8) - (ticks >> 9)
      #elif (F_CPU == 30000000UL) // Easy overclock
        ticks = ticks >> 4;
        microseconds = overflows * 1000 + (ticks + (ticks >> 3) - (ticks >> 4) + (ticks >> 7) - (ticks >> 8)); // 5 terms is the optimal. Good but not as close as we get for most.
      #elif (F_CPU == 27000000UL) // You'd think this one would be a flaming bitch right?
        ticks = ticks >> 4;
        microseconds = overflows * 1000 + (ticks + (ticks >> 2) - (ticks >> 4) - (ticks >> 9)); // +0.1 average error with only 4 terms, minimal scatter... that's just not supposed to happen!
      #elif (F_CPU == 25000000UL) // Barely overclocked.
        ticks = ticks >> 4;
        microseconds = overflows * 1000 + (ticks + /* (ticks >> 1) -*/ (ticks >> 2) + /* (ticks >> 4) -*/ (ticks >> 5)); // DEAD ON with 5 terms

      /* The Terrible Twelves (or threes) - Twelve may be a great number in a lot of ways... but here, it's actually 3 in disguise.
       * NINE TERMS in the damned bitshift division expansion. And the result isn't even amazing. - it's worse than what can be done
       * with just 5 terms for dividing by 36 or 25, or a mere 3 terms with 27... where you're dividing by 9, 12.5, and 13.5 respectively,
       * or after the initial shifts, by 0.78125, 1.25 or 1.18, and comparable to the best series for division by 1.375 (44 MHz) or 0.9375 (30 MHz) which each have 7 terms,
       * though it's better than the best possible for the division by 0.875 associated with 28 MHs clocks which is also a 7 term one.
       * This is division by 0.75, which sounds like it should be the easiest out of the lot.
       *
       * This does the following:
       * ticks = ticks >> (1, 2, 3, 4, or 6 for 3 MHz, 6 MHz, 12 MHz, 24 MHz, or 48 MHz)
       * ticks = ticks + (ticks >> 1) - (ticks >> 2) + (ticks >> 3) - (ticks >> 4) + (ticks >> 5) - (ticks >> 6) + (ticks >> 7) - (ticks >> 9)
       *
       * Equivalent to :
       * ticks = ticks / (1.5, 3, 6, 12, or 24)
       *
       * Division is way too slow, but we need to convert current timer ticks, which
       * are are 0-2999, 0-5999, 0-11999, or 0-23999 into the 3 least significant digits
       * of the number of microseconds so that it can be added to overflows * 1000.
       *
       * Runtime of the assembly is 28, 30, 32, or 34 clocks
       * 3 and 6 MHz not a supported speed.
       * 57 replaced with 30 save 27 clocks @ 12 = 2 us saved
       * 67 replaced with 32 save 35 clocks @ 24 = 1.5us saved
       * 77 replaced with 34 save 43 clocks @ 48 = 1 us saved
       */
      #elif (F_CPU == 48000000UL || F_CPU == 24000000UL || F_CPU == 12000000UL || F_CPU == 6000000UL || F_CPU == 3000000UL)
        __asm__ __volatile__(
          "movw r0,%A0"   "\n\t" // we copy ticks to r0 (temp_reg) and r1 (zero_reg) so we don't need to allocate more registers.
          "lsr r1"        "\n\t" // notice how at first, each shift takes insns. Compiler wants to use an upper register, ldi number of shifts
          "ror r0"        "\n\t" // into it, then lsr, ror, dec, breq (4 insn + 5 clocks per shift, and including the ldi, it's 5 insns + 5*shiftcount clocks)
          #if (F_CPU != 3000000UL)
            "lsr r1"        "\n\t"
            "ror r0"        "\n\t"
          #endif
          #if (F_CPU == 12000000UL || F_CPU == 24000000UL || F_CPU == 48000000UL)
            "lsr r1"      "\n\t"  // sacrifice 1 word for 9 clocks on the 12 MHz configuration
            "ror r0"      "\n\t"
          #endif
          #if (F_CPU == 24000000UL || F_CPU == 48000000UL)
            "lsr r1"      "\n\t"  // sacrifice 3 words for 12 clocks on the 24 MHz configuration
            "ror r0"      "\n\t"
          #endif
          #if (F_CPU == 48000000UL)
            "lsr r1"      "\n\t"  // sacrifice 5 words for 15 clocks on the 48 MHz configuration.
            "ror r0"      "\n\t"
          #endif
          "movw %A0,r0"   "\n\t"  // This is the value we call ticks, because that's what it was in old code.
          "lsr r1"        "\n\t"  // we just copied the now shifted value back to original location.
          "ror r0"        "\n\t"  // 2 words per shift still
          "add %A0, r0"   "\n\t"  // we now have ticks >> 1, add it to original.
          "adc %B0, r1"   "\n\t"  //
          "lsr r1"        "\n\t"  //
          "ror r0"        "\n\t"  // we now have ticks >> 2. Now it's under 250, and r1 is 0
          "mov r1,r0"     "\n\t"  // so we copy the remaining value into r1.
          "lsr r1 "       "\n\t"  // now it's only 1 insn/shift!
          "sub r0,r1"     "\n\t"  // - ticks >> 3
          "lsr r1"        "\n\t"
          "add r0,r1"     "\n\t"  // + ticks >> 4
          "lsr r1"        "\n\t"
          "sub r0,r1"     "\n\t"  // - ticks >> 5
          "lsr r1"        "\n\t"
          "add r0,r1"     "\n\t"  // + ticks >> 6
          "lsr r1"        "\n\t"
          "sub r0,r1"     "\n\t"  // - ticks >> 7
          "lsr r1"        "\n\t"
          "lsr r1"        "\n\t"
          "add r0,r1"     "\n\t"  // + ticks >> 9
          "eor r1,r1"     "\n\t"  // clear out r1
          "sub %A0,r0"    "\n\t"  // Add the sum of terms that fit in a byte to what was ticks in old code.
          "sbc %B0,r1"    "\n"    // carry - see,this is why AVR needs a known zero.
          : "+r" (ticks));        // Do the rest in C. ticks is a read/write operand.
        microseconds = overflows * 1000 + ticks; // nice and clean.

      /* The Troublesome Tens - I initially fumbled this after the **now** r1 is 0 line
       * I did several dumb things - at first I thought it was my pointless moving and
       * adding. But the real problem was that on that line, I'd just deleted the
       * now unnecessary lsr r1, leaving the next as ror instead of lsr. So instead of pushing
       * that bit into the void, it came back as the new high bit, causing the device to travel
       * back in time. Unfortunately, a few hundred milliseconds isn't far back enough to
       * snag a winning ticket for todays lotto, but more than than the execution time
       * of micros is far enough back to thoroughly break delay() Even if I could just go back
       * just far enough to tell myself where the bug was, I'd take it...
       *
       * This does the following:
       * ticks = ticks >> (1, 2, 3, or 4 for 5 MHz, 10 MHz, 20 MHz, or 40 MHz)
       * ticks = ticks - (ticks >> 2) + (ticks >> 4) - (ticks >> 6) + (ticks >> 8)
       *
       * Equivalent to:
       * ticks = tick / (2.5, 5, 10, or 20)
       * Division is way too slow, but we need to convert current timer ticks, which
       * are 0-2499, 0-4999, 0-9999, or 0-19999, into the 3 least significant digits
       * of the number of microseconds so that it can be added to overflows * 1000.
       *
       * Runtime is 23,25,27, or 29 clocks, savings vs the best I could do in C
       *
       * 33 replaced with 23 save 10 clocks @ 5  = 2 us saved
       * 46 replaced with 25 save 21 clocks @ 10 = 2.5 us saved
       * 56 replaced with 27 save 29 clocks @ 20 = 1.5 us saved
       * 66 replaced with 29 save 37 clocks @ 40 = 1 us saved
       */
      #elif (F_CPU == 40000000UL || F_CPU == 20000000UL || F_CPU == 10000000UL || F_CPU == 5000000UL)
        __asm__ __volatile__(
          "movw r0,%A0"   "\n\t"  // no savings until after the initial rightshifts at 5 MHz
          "lsr r1"        "\n\t"
          "ror r0"        "\n\t"
          #if (F_CPU == 10000000UL || F_CPU == 20000000UL || F_CPU == 40000000UL)
            "lsr r1"      "\n\t"  // sacrifice 1 word for 9 clocks at 10 MHz
            "ror r0"      "\n\t"
          #endif
          #if (F_CPU == 20000000UL || F_CPU == 40000000UL)
            "lsr r1"      "\n\t"  // sacrifice 3 words for 12 clocks at 20 MHz
            "ror r0"      "\n\t"
          #endif
          #if (F_CPU == 40000000UL)
            "lsr r1"      "\n\t"  // sacrifice 5 words for 15 clocks at 40 MHz
            "ror r0"      "\n\t"
          #endif
          "movw %A0,r0"   "\n\t"  // ticks
          "lsr r1"        "\n\t"
          "ror r0"        "\n\t"
          "lsr r1"        "\n\t"
          "ror r0"        "\n\t"  //   ticks >> 2.
          "sub %A0, r0"   "\n\t"  // - ticks >> 2
          "sbc %B0, r1"   "\n\t"  // It could be 312 so we can't do what we did for the 12's
          "lsr r1"        "\n\t"
          "ror r0"        "\n\t"  // **now** r1 is 0.
          "lsr r0"        "\n\t"
          "mov r1,r0"     "\n\t"  // + ticks >> 4
          "lsr r1"        "\n\t"
          "lsr r1"        "\n\t"
          "sub r0,r1"     "\n\t"  // - ticks >> 6
          "lsr r1"        "\n\t"
          "lsr r1"        "\n\t"
          "add r0,r1"     "\n\t"  // + ticks >> 8
          "eor r1,r1"     "\n\t"  // restore zero_reg
          "add %A0,r0"    "\n\t"  // add to the shifted ticks
          "adc %B0,r1"    "\n"    // carry
          : "+r" (ticks));        // Do the rest in C. ticks is a read/write operand.
        microseconds = overflows * 1000 + ticks;
/* replaces:
      #elif (F_CPU == 48000000UL) // Extreme overclocking
        ticks = ticks >> 5;
        microseconds = overflows * 1000 + (ticks + (ticks >> 2) + (ticks >> 3) - (ticks >> 5)); // - (ticks >> 7)
      #elif (F_CPU == 24000000UL) // max rated speed
        ticks = ticks >> 4;
        microseconds = overflows * 1000 + (ticks + (ticks >> 2) + (ticks >> 3) - (ticks >> 5)); // - (ticks >> 7)
      #elif (F_CPU == 12000000UL)
        ticks = ticks >> 3;
        microseconds = overflows * 1000 + (ticks + (ticks >> 2) + (ticks >> 3) - (ticks >> 5)); // - (ticks >> 7)
      // Never was an implementation for 3 or 6, but it's obvious what the old style implementation would be,
      #elif (F_CPU == 40000000UL) // overclocked aggressively
        ticks = ticks >> 4;
        microseconds = overflows * 1000 + (ticks - (ticks >> 2) + (ticks >> 4) - (ticks >> 6)); // + (ticks >> 8)
      #elif (F_CPU == 20000000UL)
        ticks = ticks >> 3;
        microseconds = overflows * 1000 + (ticks - (ticks >> 2) + (ticks >> 4) - (ticks >> 6)); // + (ticks >> 8)
      #elif (F_CPU == 10000000UL)
        ticks = ticks >> 2;
        microseconds = overflows * 1000 + (ticks - (ticks >> 2) + (ticks >> 4) - (ticks >> 6)); // + (ticks >> 8)
      #elif (F_CPU ==  5000000UL)
        ticks = ticks >> 1;
        microseconds = overflows * 1000 + (ticks - (ticks >> 2) + (ticks >> 4) - (ticks >> 6)); // + (ticks >> 8)
*/

      // powers of 2  - and a catchall for parts without dedicated implementations. It gives wrong results, but
      // it also doesn't take forever like doing division would.
      #elif (F_CPU  == 32000000UL || F_CPU > 24000000UL)
        microseconds = overflows * 1000 + (ticks >> 4);
      #elif (F_CPU  == 16000000UL || F_CPU > 12000000UL)
        microseconds = overflows * 1000 + (ticks >> 3);
      #elif (F_CPU  ==  8000000UL || F_CPU >  6000000UL)
        microseconds = overflows * 1000 + (ticks >> 2);
      #elif (F_CPU  ==  4000000UL || F_CPU >= 3000000UL)
        microseconds = overflows * 1000 + (ticks >> 1);
      #else //(F_CPU == 1000000UL || F_CPU == 2000000UL) - here clock is running at system clock instead of half system clock.
            // and hence overflows only once per 2ms. On 2 MHz
            // also works at 2MHz, since we use CLKPER for 1MHz vs CLKPER/2 for all others.
        microseconds   = overflows * 1000 + ticks;
      #endif
      #if !((F_CPU == 48000000UL || F_CPU == 36000000UL || F_CPU == 24000000UL || F_CPU == 12000000UL || /* multiples of 12           */ \
             F_CPU == 40000000UL || F_CPU == 30000000UL || F_CPU == 20000000UL || F_CPU == 10000000UL || /* multiples of 10           */ \
             F_CPU == 32000000UL || F_CPU == 16000000UL || F_CPU ==  8000000UL || F_CPU ==  4000000UL || /* powers of 2               */ \
             F_CPU ==  2000000UL || F_CPU ==  1000000UL || F_CPU == 25000000UL || F_CPU ==  5000000UL || /* powers of 2 cont, 25, 5   */ \
             F_CPU == 44000000UL || F_CPU == 28000000UL || F_CPU == 14000000UL || F_CPU ==  3000000UL || /* oddball frequencies       */ \
             F_CPU == 27000000UL)&& /* warn fools who messed with the timers.h file too and expected that the core would sort out how */ \
            ((TIME_TRACKING_TIMER_DIVIDER == 2 && TIME_TRACKING_TICKS_PER_OVF == F_CPU/2000) || /*how to make the timer work correctly*/ \
             (TIME_TRACKING_TIMER_DIVIDER == 1 && (TIME_TRACKING_TICKS_PER_OVF == F_CPU/500 && F_CPU == 1000000) || (TIME_TRACKING_TICKS_PER_OVF == F_CPU/1000 && F_CPU == 2000000))))
                                                 /*  without them implementing it. No such luck  */
        #warning "Millis timer (TCBn) at this frequency and/or configuration unsupported, micros() will return totally bogus values."
      #endif
    #else // Done with TCB

      #if (F_CPU == 30000000UL && TIME_TRACKING_TICKS_PER_OVF == 255 && TIME_TRACKING_TIMER_DIVIDER == 64)
        microseconds = (overflows * clockCyclesToMicroseconds(TIME_TRACKING_CYCLES_PER_OVF))
            + ((ticks * 2) + ((uint16_t)(ticks >> 3)));
      #elif (F_CPU == 28000000UL && TIME_TRACKING_TICKS_PER_OVF == 255 && TIME_TRACKING_TIMER_DIVIDER == 64)
        microseconds = (overflows * clockCyclesToMicroseconds(TIME_TRACKING_CYCLES_PER_OVF))
            + ((ticks * 2) + ((uint16_t)(ticks >> 2) + (ticks >> 5)));
      #elif (F_CPU == 25000000UL && TIME_TRACKING_TICKS_PER_OVF == 255 && TIME_TRACKING_TIMER_DIVIDER == 64)
        microseconds = (overflows * clockCyclesToMicroseconds(TIME_TRACKING_CYCLES_PER_OVF))
            + (ticks * 2 + ((uint16_t)(ticks >> 1) + (ticks >> 4)));
      #elif (F_CPU == 24000000UL && TIME_TRACKING_TICKS_PER_OVF == 255 && TIME_TRACKING_TIMER_DIVIDER == 64)
        microseconds = (overflows * clockCyclesToMicroseconds(TIME_TRACKING_CYCLES_PER_OVF))
            + (ticks * 3 - ((uint16_t)(ticks >> 2) - (ticks >> 4) - (ticks >> 5)));
      #elif (F_CPU == 20000000UL && TIME_TRACKING_TICKS_PER_OVF == 255 && TIME_TRACKING_TIMER_DIVIDER == 64)
        microseconds = (overflows * clockCyclesToMicroseconds(TIME_TRACKING_CYCLES_PER_OVF))
            + (ticks * 3 + ((uint16_t)(ticks >> 2) - (ticks >> 4)));
      #elif (F_CPU == 28000000UL && TIME_TRACKING_TICKS_PER_OVF == 255 && TIME_TRACKING_TIMER_DIVIDER == 64)
        microseconds = (overflows * clockCyclesToMicroseconds(TIME_TRACKING_CYCLES_PER_OVF))
            + (ticks * 4 + ((uint16_t)(ticks >> 1) + (ticks >> 4) + (ticks >> 5)));
      #elif (F_CPU == 12000000UL && TIME_TRACKING_TICKS_PER_OVF == 255 && TIME_TRACKING_TIMER_DIVIDER == 64)
        microseconds = (overflows * clockCyclesToMicroseconds(TIME_TRACKING_CYCLES_PER_OVF))
            + (ticks * 5 + ((uint16_t)(ticks >> 2) + (ticks >> 4) + (ticks >> 5)));
      #elif (F_CPU == 10000000UL && TIME_TRACKING_TICKS_PER_OVF == 255 && TIME_TRACKING_TIMER_DIVIDER == 64)
        microseconds = (overflows * clockCyclesToMicroseconds(TIME_TRACKING_CYCLES_PER_OVF))
            + ((ticks << 3) - ((uint16_t)(ticks << 1) + (ticks >> 1) - (ticks >> 3)));
      #elif (F_CPU == 5000000UL && TIME_TRACKING_TICKS_PER_OVF == 255 && TIME_TRACKING_TIMER_DIVIDER == 16)
        microseconds = (overflows * millisClockCyclesToMicroseconds(TIME_TRACKING_CYCLES_PER_OVF))
                     + (ticks * 3 + ((uint16_t)(ticks >> 2) - (ticks >> 4)));
      #else
        #if (TIME_TRACKING_TIMER_DIVIDER%(F_CPU/1000000))
          #warning "Millis timer (TCA0) at this frequency unsupported, micros() will return bogus values."
        #endif
        microseconds = ((overflows * millisClockCyclesToMicroseconds(TIME_TRACKING_CYCLES_PER_OVF))
                      + (ticks * (millisClockCyclesToMicroseconds(TIME_TRACKING_CYCLES_PER_OVF) / TIME_TRACKING_TIMER_PERIOD)));
      #endif
    #endif // end of timer-specific part of micros calculations
    return microseconds;
  }
  #else // end of non-RTC micros code
  /* We do not have a timebase sufficiently accurate to give microsecond timing. In fact, we barely have millisecond timing available
   * The microsecond delay counts clock cycles, and so it does still work. It is planned that a future library will switch the millis
   * pause millis before sleeping and turn on the RTC, tracking the passage of time to a much coarser resolution with that, and turn
   * it back on when waking from sleep, so people can keep time while sleeping without sacrificing micros().
   * In any event, as of 2.4.3 we now provide the stub below, which we hope is more useful than being told that micros() isn't defined.
   */
    unsigned long micros() {
      badCall("microsecond timekeeping is not supported when the RTC is used as the sole timekeeping timer (though delayMicroseconds() is)");
      return -1;
    }
  #endif
#else
  /* Uses should not call millis() or micros() if the core timekeeping has been disabled. Usually, encountering this error either means
   * that they disabled millis earlier for some other sketch, and the preferences were saved with that - or that they are using a library
   * with a dependence on the timekeeping facilities. Sometimes these are meaningful, other times it is only for a feature that isn't
   * being used, or to catch a particular corner case (see: tinyNeoPixel, very end of show() for an example).
   * As of 2.4.3 we provide the stubs below, which we hope is more useful than being told that millis or micros isn't defined.
   */
  unsigned long micros() {
    badCall("micros() is not available because it has been disabled through the tools -> millis()/micros() menu");
    return -1;
  }
  unsigned long millis() {
    badCall("millis() is not available because it has been disabled through the tools -> millis()/micros() menu");
    return -1;
  }
#endif // end of non-MILLIS_USE_TIMERNONE code


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

inline __attribute__((always_inline)) void delayMicroseconds(unsigned int us) {
  // This function gets optimized away, but to what depends on whether us is constant.
  if (__builtin_constant_p(us)) {
    _delay_us(us); // Constant microseconds use the avr-libc _delay_us() which is highly accurate for all values and efficient!
  } else { // If it is not, we have to use the Arduino style implementation.
    _delayMicroseconds(us);
  }
}

/* delayMicroseconds() when delay is not a compile-time known constant.
 * Delay for the given number of microseconds. This is UGLY AS SIN and explicitly depends on function call
 * overhead for very short delays.
 * High clock speeds shouldn't return immediately for a 1us delay - we can instead only drop a fraction of a us
 * 48, 44, 40, and 32 drop 1/2 us, and 36 drops 2/3rds.
 * Note that us ceases to be in units of microseconds as soon as the function is entered; it gets turned into the loop counter.
 * Then we use a minimal number of bitshifts to calculate the number of passes through the delay loop
 * and subtract the number of loop-cycles of time we burned doing so. But need to be careful that sane values
 * don't get so much bigger that they overflow the unsigned int we're storing it in. To that end, we use
 * a longer loop at faster clock speeds.
 * In the inline assembly, when a delay of 8 clocks or longer is required, we save flash with a clever trick:
 *  "rjmp .+2" "\n\t"     // 2 cycles - jump over the return.
 *  "ret" "\n\t"          // 4 cycles - rjmped over initially...
 *  "rcall .-4" "\n\t"    // 2 cycles - ... but then called here...
 * This exploits the fact that return is a 4-clock instruction (even on AVRxt) by first hopping over a return
 * then immediately calling that return instruction - 8 clocks in 3 words. Once the ret is there, additional
 * rcall instructions can get 6 clocks in a single word, though we only get to take advantage of that once for
 * the 30 MHz case and any longer delays do better with a loop.
 */

#if   F_CPU >= 48000000L
  // 16 MHz math, 12-cycle loop, 1us burns and passes through loop twice.
  #define DELAYMICROS_TWELVE
#elif F_CPU >= 44000000L
  // 16 MHz math, 11-cycle loop, 1us burns and passes through loop twice.
  #define DELAYMICROS_ELEVEN
#elif F_CPU >= 40000000L
  // 20 MHz math, 10-cycle loop, 1us burns and passes through loop twice.
  #define DELAYMICROS_TEN
#elif F_CPU >= 36000000L
  // 12 MHz math, 12-cycle loop, 1us burns and passes through loop once.
  #define DELAYMICROS_TWELVE
#elif F_CPU >= 32000000L
  // 16 MHz math, 8-cycle loop, 1us passes through loop twice.
  #define DELAYMICROS_EIGHT
#elif F_CPU >= 30000000L
  // 12 MHz math, 10-cycle loop, 1us burns and returns.
  #define DELAYMICROS_TEN
#elif F_CPU >= 28000000L
  // 16 MHz math, 7-cycle loop, 1us burns and returns.
  #define DELAYMICROS_SEVEN
#elif F_CPU >= 27000000L
  // 12 MHz math, 9 cycle loop, 1us burns and returns
  #define DELAYMICROS_NINE
#elif F_CPU >= 24000000L
  // 12 MHz math, 8-cycle loop, 1us burns and returns.
  #define DELAYMICROS_EIGHT
#elif F_CPU >= 20000000L
  // 20 MHz math, 10-cycle loop, 1us burns and returns.
  #define DELAYMICROS_TEN
#elif F_CPU >= 16000000L
  // 16 MHz math, 4-cycle loop, 1us returns immediately.
#elif F_CPU >= 12000000L
  // 16 MHz math, 4-cycle loop, 1us returns immediately.
#elif F_CPU >= 10000000L || (F_CPU >= 5000000L && F_CPU < 8000000L)
  // 10 MHz: 5-cycle loop, 1us returns immediately
  // 5 MHz: 5-cycle loop, 1-3 us returns immediately.
  #define DELAYMICROS_FIVE
#else
  // 8 MHz: 16 MHz math, 4-cycle loop, 1-2 us returns immediately.
  // 4 MHz: 16 MHz math, 4-cycle loop, 1-4 us returns immediately.
  // 2 MHz: 16 MHz math, 4-cycle loop, 1-8 us returns immediately.
  // 1 MHz: 16 MHz math, 4-cycle loop, < 16 us returns immediately, < 25 burns and returns.
  // Anything not listed uses the fastest one that is and which is slower than F_CPU
#endif

__attribute__ ((noinline)) void _delayMicroseconds(unsigned int us) {
 /* Must be noinline because we rely on function-call overhead */
#if F_CPU == 48000000L
  // make the initial delay 24 cycles
  __asm__ __volatile__ (
    "rjmp .+2" "\n\t"     // 2 cycles - jump over next instruction.
    "ret" "\n\t"          // 4 cycles - rjmped over initially....
    "rcall .-4");         // 2 cycles - ... but then called here);
                          // wait 8 cycles with 3 words
  // the loop takes 1/4 of a microsecond (12 cycles) per iteration
  // so execute it four times for each microsecond of delay requested.
  us <<= 2; // x4 us, = 4 cycles
  // we only burned ~22 cycles above, subtraction takes another 2 - so we've lost
  // half a us and only need to drop 2 rounds through the loop!
  us -= 2; // = 2 cycles,

#elif F_CPU >= 44000000L
  // Again, we can do all this in half of 1 us, so we
  // just pass through the loop 2 times for 1 us delay.
  __asm__ __volatile__(
    "rjmp .+0" "\n\t"     // 2 cycles
    "rjmp .+0" "\n\t"     // 2 cycles
    "nop");               // 1 cycles
                          // Wait 5 cycles in 3 words.
  // the loop takes 1/4 of a microsecond (11 cycles) per iteration
  // so execute it four times for each microsecond of delay requested.
  us <<= 2; // x4 us, = 4 cycles
  // we just burned 19 (21) cycles above, remove 2
  // us is at least 8 so we can subtract 2
  us -= 2;

#elif F_CPU >= 40000000L
  // Again, we can do all this in half of 1 us, so we
  // just pass through the loop 2 times for 1 us delay.
  __asm__ __volatile__(
    "rjmp .+0" "\n\t"     // 2 cycles
    "nop");               // 1 cycles
                          // Wait 3 cycles in 2 words.
  // the loop takes 1/4 of a microsecond (10 cycles) per iteration
  // so execute it four times for each microsecond of delay requested.
  us <<= 2; // x4 us, = 4 cycles
  // we just burned 17 (19) cycles above, remove 2.
  // us is at least 8 so we can subtract 2
  us -= 2;

#elif F_CPU >= 36000000L
  // Here we get the initial delay is about 24 cycles, so we pass through
  // the loop once for 1us delay.
  __asm__ __volatile__ (
    "rjmp .+0" "\n\t"     // 2 cycles
    "rjmp .+2" "\n\t"     // 2 cycles - jump over next instruction.
    "ret" "\n\t"          // 4 cycles - rjmped over initially....
    "rcall .-4");         // 2 cycles - ... but then called here);
                          // wait 10 cycles in 4 words
  // the loop takes 1/3 of a microsecond (12 cycles) per iteration
  // so execute it three times for each microsecond of delay requested.
  us = (us << 1) + us; // x3 us, = 5 cycles
  // we just burned 23 (25) cycles above, remove 2
  us -= 2; // 2 cycles

#elif F_CPU >= 32000000L
  // here, we only take half a us at the start
  __asm__ __volatile__ ("rjmp .+0");
                          // wait 2 cycles
  // in by the end of this section.
  // the loop takes 1/4 of a microsecond (8 cycles) per iteration
  // so execute it four times for each microsecond of delay requested.
  us <<= 2; // x4 us, = 4 cycles
  // we only burned ~14 cycles above, subtraction takes another 2 - so we've lost half a us,
  // and only need to drop 2 rounds through the loop!
  us -= 2; // = 2 cycles

#elif F_CPU >= 30000000L
  // for a one-microsecond delay, burn 14 cycles and return
  __asm__ __volatile__ (
    "rjmp .+2" "\n\t"     // 2 cycles - jump over the return.
    "ret" "\n\t"          // 4 cycles - rjmped over initially...
    "rcall .-4" "\n\t"    // 2 cycles - ... but then called here...
    "rcall .-6");         // 2+4 cycles - ... and here again!
                          // Waiting 14 cycles in only 4 words
  if (us <= 1) return; // = 3 cycles, (4 when true)
  // the loop takes 1/3 of a microsecond (10 cycles) per iteration
  // so execute it three times for each microsecond of delay requested.
  us = (us << 1) + us; // x3 us, = 5 cycles
  // we just burned 28 (30) cycles above, remove 3
  us -= 3; // 2 cycles

#elif F_CPU >= 28000000L
  // for a one-microsecond delay, burn 12 cycles and return
  __asm__ __volatile__ (
    "rjmp .+0" "\n\t"     // 2 cycles
    "rjmp .+0" "\n\t"     // 2 cycles
    "rjmp .+2" "\n\t"     // 2 cycles - jump over next instruction.
    "ret" "\n\t"          // 4 cycles - rjmped over initially....
    "rcall .-4");         // 2 cycles - ... but then called here);
                          // wait 12 cycles in 5 words
  if (us <= 1) return; //  = 3 cycles, (4 when true)

  // the loop takes 1/4 of a microsecond (7 cycles) per iteration
  // so execute it four times for each microsecond of delay requested.
  us <<= 2; // x4 us, = 4 cycles=
  // we just burned 27 (29) cycles above, remove 4, (7*4=28)
  // us is at least 8 so we can subtract 5
  us -= 4; // = 2 cycles,

#elif F_CPU >= 27000000L
  // for a one-microsecond delay, burn 11 cycles and return
  __asm__ __volatile__ (  // wait 8 cycles with 3 words
    "rjmp .+2" "\n\t"     // 2 cycles - jump over next instruction.
    "ret" "\n\t"          // 4 cycles - rjmped over initially....
    "rcall .-4" "\n\t"    // 2 cycles - ... but then called here);
    "rjmp .+0" "\n\t"     // 2 cycles
    "nop");               // 1 more  == 11 total
  if (us <= 1) return;    //  = 3 cycles, (4 when true)

  // the loop takes 1/3 of a microsecond (8 cycles) per iteration
  // so execute it three times for each microsecond of delay requested.
  us = (us << 1) + us; // x3 us, = 5 cycles
  // we just burned 27 (24) cycles above, remove 3
  us -= 3; // 2 cycles


#elif F_CPU >= 24000000L
  // for a one-microsecond delay, burn 8 cycles and return
  __asm__ __volatile__ (
    "rjmp .+2" "\n\t"     // 2 cycles - jump over next instruction.
    "ret" "\n\t"          // 4 cycles - rjmped over initially....
    "rcall .-4");         // 2 cycles - ... but then called here);
                          // wait 8 cycles with 3 words
  if (us <= 1) return; //  = 3 cycles, (4 when true)

  // the loop takes 1/3 of a microsecond (8 cycles) per iteration
  // so execute it three times for each microsecond of delay requested.
  us = (us << 1) + us; // x3 us, = 5 cycles
  // we just burned 24 (22) cycles above, remove 3
  us -= 3; // 2 cycles

#elif F_CPU >= 20000000L
  // for a one-microsecond delay, burn 4 clocks and then return
  __asm__ __volatile__ (
    "rjmp .+0" "\n\t"     // 2 cycles
    "nop" );              // 1 cycle
                          // wait 3 cycles with 2 words
  if (us <= 1) return; //  = 3 cycles, (4 when true)
  // the loop takes a 1/2 of a microsecond (10 cycles) per iteration
  // so execute it twice for each microsecond of delay requested.
  us = us << 1; // x2 us, = 2 cycles
  // we just burned 21 (23) cycles above, remove 2
  // us is at least 4 so we can subtract 2.
  us -= 2; // 2 cycles

#elif F_CPU >= 16000000L
  // for a one-microsecond delay, simply return.  the overhead
  // of the function call takes 14 (16) cycles, which is 1us
  if (us <= 1) return; //  = 3 cycles, (4 when true)
  // the loop takes 1/4 of a microsecond (4 cycles) per iteration
  // so execute it four times for each microsecond of delay requested.
  us <<= 2; // x4 us, = 4 cycles
  // we just burned 19 (21) cycles above, remove 5, (5*4=20)
  // us is at least 8 so we can subtract 5
  us -= 5; // = 2 cycles

#elif F_CPU >= 12000000L
  // for a 1 microsecond delay, simply return.  the overhead
  // of the function call takes 14 (16) cycles, which is 1.5us
  if (us <= 1) return; //  = 3 cycles, (4 when true)
  // the loop takes 1/3 of a microsecond (4 cycles) per iteration
  // so execute it three times for each microsecond of delay requested.
  us = (us << 1) + us; // x3 us, = 5 cycles
  // we just burned 20 (22) cycles above, remove 5, (5*4=20)
  // us is at least 6 so we can subtract 5
  us -= 5; // 2 cycles

#elif F_CPU >= 10000000L
  // for a 1 microsecond delay, simply return.  the overhead
  // of the function call takes 14 (16) cycles, which is 1.5us
  if (us <= 2) return; //  = 3 cycles, (4 when true)
  // the loop takes 1/2 of a microsecond (5 cycles) per iteration
  // so execute it 2 times for each microsecond of delay requested.
  us = us << 1; // x2 us, = 2 cycles
  // we just burned 20 (22) cycles above, remove 4, (5*4=20)
  // us is at least 6 so we can subtract 4
  us -= 4; // 2 cycles

#elif F_CPU >= 8000000L
  // for a 1 and 2 microsecond delay, simply return.  the overhead
  // of the function call takes 14 (16) cycles, which is 2us
  if (us <= 2) return; //  = 3 cycles, (4 when true)
  // the loop takes 1/2 of a microsecond (4 cycles) per iteration
  // so execute it twice for each microsecond of delay requested.
  us <<= 1; // x2 us, = 2 cycles
  // we just burned 17 (19) cycles above, remove 5, (4*5=20)
  // us is at least 6 so we can subtract 4
  us -= 5; // = 2 cycles

#elif F_CPU >= 5000000L
  // for a 1 ~ 3 microsecond delay, simply return.  the overhead
  // of the function call takes 14 (16) cycles, which is 3us
  if (us <= 3) return; // 3 cycles, (4 when true)
  // the loop takes 1 microsecond (5 cycles) per iteration
  // so just remove 3 loops for overhead
  us -= 3; // = 2 cycles

#elif F_CPU >= 4000000L
  // for a 1 ~ 4 microsecond delay, simply return.  the overhead
  // of the function call takes 14 (16) cycles, which is 4us
  if (us <= 4) return; // 3 cycles, (4 when true)
  // the loop takes 1 microsecond (4 cycles) per iteration,
  // just remove 4 loops for overhead
  us -= 4; // = 2 cycles for the time taken up with call overhead and test above

#elif F_CPU >= 2000000L
  // for a 1 ~ 4 microsecond delay, simply return.  the overhead
  // of the function call takes 14 (16) cycles, which is 8us
  if (us <= 8) return; // 3 cycles, (4 when true)
  // the loop takes 2 microsecond (4 cycles) per iteration,
  // just remove 4 loops for overhead
  us >>= 1; // divide by 2.
  us -= 4; // = 2 cycles for the time taken up with call overhead and test above

#else // F_CPU >= 1000000
  // for the 1 MHz internal clock (default settings for common AVR microcontrollers)
  // the overhead of the function calls is 14 (16) cycles
  if (us <= 16) return; // 3 cycles, (4 when true)
  if (us <= 25) return; // 3 cycles, (4 when true), (must be at least 26 if we want to subtract 22 and rightshift twice.)
  // compensate for the time taken by the preceding and following commands (about 22 cycles)
  us -= 22; // = 2 cycles
  // the loop takes 4 microseconds (4 cycles)
  // per iteration, so execute it us/4 times
  // us is at least 4, divided by 4 gives us 1 (no zero delay bug)
  us >>= 2; // us div 4, = 4 cycles
#endif
/* Implementation of the delay loop of 4, 5, 7, 8, 10, 11, or 12 clocks. */
#if defined(DELAYMICROS_TWELVE)
  __asm__ __volatile__ (
    "1: sbiw %0, 1" "\n\t"            // 2 cycles
    "rjmp .+2"      "\n\t"            // 2 cycles - jump over next instruction.
    "ret"           "\n\t"            // 4 cycles - rjmped over initially....
    "rcall .-4"     "\n\t"            // 2 cycles - ... but then called here
    "brne 1b" : "=w" (us) : "0" (us)  // 2 cycles
  );
#elif defined(DELAYMICROS_ELEVEN)
  __asm__ __volatile__ (
    "1: sbiw %0, 1" "\n\t"            // 2 cycles
    "nop"           "\n\t"            // 1 cycle
    "rjmp .+0"      "\n\t"            // 2 cycles
    "rjmp .+0"      "\n\t"            // 2 cycles
    "rjmp .+0"      "\n\t"            // 2 cycles
    "brne 1b" : "=w" (us) : "0" (us)  // 2 cycles
  );
#elif defined(DELAYMICROS_TEN)
  __asm__ __volatile__ (
    "1: sbiw %0, 1" "\n\t"            // 2 cycles
    "rjmp .+0"      "\n\t"            // 2 cycles
    "rjmp .+0"      "\n\t"            // 2 cycles
    "rjmp .+0"      "\n\t"            // 2 cycles
    "brne 1b" : "=w" (us) : "0" (us)  // 2 cycles
  );
#elif defined(DELAYMICROS_NINE)
  __asm__ __volatile__ (
    "1: sbiw %0, 1" "\n\t"            // 2 cycles
    "rjmp .+0"      "\n\t"            // 2 cycles
    "rjmp .+0"      "\n\t"            // 2 cycles
    "nop"           "\n\t"
    "brne 1b" : "=w" (us) : "0" (us)  // 2 cycles
  );
#elif defined(DELAYMICROS_EIGHT)
  __asm__ __volatile__ (
    "1: sbiw %0, 1" "\n\t"            // 2 cycles
    "rjmp .+0"      "\n\t"            // 2 cycles
    "rjmp .+0"      "\n\t"            // 2 cycles
    "brne 1b" : "=w" (us) : "0" (us)  // 2 cycles
  );
#elif defined(DELAYMICROS_SEVEN)
  __asm__ __volatile__ (
    "1: sbiw %0, 1" "\n\t"            // 2 cycles
    "rjmp .+0"      "\n\t"            // 2 cycles
    "nop"           "\n\t"            // 1 cycle
    "brne 1b" : "=w" (us) : "0" (us)  // 2 cycles
  );
#elif defined(DELAYMICROS_FIVE)
  __asm__ __volatile__ (
    "1: sbiw %0, 1" "\n\t"            // 2 cycles
    "nop"           "\n\t"            // 1 cycle
    "brne 1b" : "=w" (us) : "0" (us)  // 2 cycles
  );
#else // the classic 4 cycle delay loop...
  __asm__ __volatile__ (
    "1: sbiw %0, 1" "\n\t"            // 2 cycles
    "brne 1b" : "=w" (us) : "0" (us)  // 2 cycles
  );
#endif
  // return = 4 cycles
}

void stop_millis()
{ // Disable the interrupt:
  #if defined(MILLIS_USE_TIMERNONE)
    badCall("stop_millis() is only valid with millis time keeping enabled.");
  #else
    #if defined(MILLIS_USE_TIMERA0)
      TCA0.SPLIT.INTCTRL &= (~TCA_SPLIT_HUNF_bm);
    #elif defined(MILLIS_USE_TIMERA1)
      TCA1.SPLIT.INTCTRL &= (~TCA_SPLIT_HUNF_bm);
    #elif defined(MILLIS_USE_TIMERD0)
      TCD0.INTCTRL &= 0xFE;
    #elif defined(MILLIS_USE_TIMERRTC)
      RTC.INTCTRL &= 0xFE;
      RTC.CTRLA &= 0xFE;
    #else
      _timer->INTCTRL &= ~TCB_CAPT_bm;
    #endif
  #endif
}



void restart_millis()
{
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
    #elif (defined(MILLIS_USE_TIMERB0) || defined(MILLIS_USE_TIMERB1) || defined(MILLIS_USE_TIMERB2) || defined(MILLIS_USE_TIMERB3) || defined(MILLIS_USE_TIMERB4)) // It's a type b timer
      _timer->CTRLB       = 0;
    #endif
    init_millis();
  #endif
}




void __attribute__((weak)) init_millis()
{
  #if defined(MILLIS_USE_TIMERNONE)
    badCall("init_millis() is only valid with millis time keeping enabled.");
  #else
    #if defined(MILLIS_USE_TIMERA0)
      TCA0.SPLIT.INTCTRL |= TCA_SPLIT_HUNF_bm;
    #elif defined(MILLIS_USE_TIMERA1)
      TCA1.SPLIT.INTCTRL |= TCA_SPLIT_HUNF_bm;
    #elif defined(MILLIS_USE_TIMERD0)
      TCD0.CMPBCLR        = TIME_TRACKING_TIMER_PERIOD; // essentially, this is TOP
      TCD0.CTRLB          = 0x00; // oneramp mode
      TCD0.CTRLC          = 0x80;
      TCD0.INTCTRL        = 0x01; // enable interrupt
      TCD0.CTRLA          = TIMERD0_PRESCALER | 0x01; // set clock source and enable!
    #elif defined(MILLIS_USE_TIMERRTC)
      while(RTC.STATUS); // if RTC is currently busy, spin until it's not.
      // to do: add support for RTC timer initialization
      RTC.PER             = 0xFFFF;
      #ifdef MILLIS_USE_TIMERRTC_XTAL
        _PROTECTED_WRITE(CLKCTRL.XOSC32KCTRLA,0x03);
        RTC.CLKSEL        = 2; // external crystal
      #else
        _PROTECTED_WRITE(CLKCTRL.OSC32KCTRLA,0x02);
        // RTC.CLKSEL=0; this is the power on value
      #endif
      RTC.INTCTRL         = 0x01; // enable overflow interrupt
      RTC.CTRLA           = (RTC_RUNSTDBY_bm|RTC_RTCEN_bm|RTC_PRESCALER_DIV32_gc);//fire it up, prescale by 32.
    #else // It's a type b timer - we have already errored out if that wasn't defined
      _timer->CCMP = TIME_TRACKING_TIMER_PERIOD;
      // Enable timer interrupt, but clear the rest of register
      _timer->INTCTRL = TCB_CAPT_bm;
      // Clear timer mode (since it will have been set as PWM by init())
      _timer->CTRLB = 0;
      // CLK_PER/1 is 0b00, . CLK_PER/2 is 0b01, so bitwise OR of valid divider with enable works
      _timer->CTRLA = TIME_TRACKING_TIMER_DIVIDER|TCB_ENABLE_bm;  // Keep this last before enabling interrupts to ensure tracking as accurate as possible
    #endif
  #endif
}

void set_millis(__attribute__((unused))uint32_t newmillis)
{
  #if defined(MILLIS_USE_TIMERNONE)
    badCall("set_millis() is only valid with millis timekeeping enabled.");
    GPIOR0 |= newmillis; // keeps the compiler from warning about unused parameter, it's a compile error if this is reachable anyway.
  #else
    #if defined(MILLIS_USE_TIMERRTC)
      // timer_overflow_count = newmillis >> 16;
      // millis = 61/64(timer_overflow_count << 16 + RTC.CNT)
      uint8_t oldSREG = SREG; // save SREG
      cli();                // interrupts off
      uint16_t temp = (newmillis % 61) << 6;
      newmillis = (newmillis / 61) << 6;
      temp = temp / 61;
      newmillis += temp;
      timer_overflow_count = newmillis >> 16;
      while(RTC.STATUS&RTC_CNTBUSY_bm); // wait if RTC busy
      RTC.CNT = newmillis & 0xFFFF;
      SREG = oldSREG; // reemable oimterripts if we killed them,
    #else
      /* farting around with micros via overflow count was ugly and buggy.
       * may implement again, better, in the future - but millis and micros
       * will get out of sync when you use set_millis
       * I think the way to do it is to make this implementation (but not big one)
       * inline, so if newmillis is constant, we can calculate the (compile-time known)
       * number of overflows using all the floating point math we want, and otherwise,
       * document that it will zero out micros.*/
      timer_millis = newmillis;
    #endif
  #endif
}

void nudge_millis(__attribute__((unused)) uint16_t nudgesize) {
  #if (MILLIS_TIMER & 0x78) /* 0x40 matches TCDs, 0x20 matches TCBs, 0x10 matches TCA0 0x08 matches TCA1, so OR them together and AND it with the timer to make sure it's not RTC, disabled, etc.  */
    uint8_t oldSREG=SREG;
    cli();
    timer_millis += nudgesize;
    SREG=oldSREG;
  #else
    #warning "Timer correction not yet supported for this timer.");
  #endif
}

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

  /******************************** CLOCK STUFF *********************************/
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
      #if (F_CPU == 20000000)
        /* No division on clock */
        _PROTECTED_WRITE(CLKCTRL_MCLKCTRLB, 0x00);
      #elif (F_CPU == 16000000)
        /* No division on clock */
        _PROTECTED_WRITE(CLKCTRL_MCLKCTRLB, 0x00);
      #elif (F_CPU == 10000000) // 20MHz prescaled by 2
        /* Clock DIV2 */
        _PROTECTED_WRITE(CLKCTRL_MCLKCTRLB, (CLKCTRL_PEN_bm | CLKCTRL_PDIV_2X_gc));
      #elif (F_CPU == 8000000) // 16MHz prescaled by 2
        /* Clock DIV2 */
        _PROTECTED_WRITE(CLKCTRL_MCLKCTRLB, (CLKCTRL_PEN_bm | CLKCTRL_PDIV_2X_gc));
      #elif (F_CPU == 5000000) // 20MHz prescaled by 4
        /* Clock DIV4 */
        _PROTECTED_WRITE(CLKCTRL_MCLKCTRLB, (CLKCTRL_PEN_bm | CLKCTRL_PDIV_4X_gc));
      #elif (F_CPU == 4000000) // 16MHz prescaled by 4
        /* Clock DIV4 */
        _PROTECTED_WRITE(CLKCTRL_MCLKCTRLB, (CLKCTRL_PEN_bm | CLKCTRL_PDIV_4X_gc));
      #elif (F_CPU == 2000000) // 16MHz prescaled by 8
        /* Clock DIV8 */
        _PROTECTED_WRITE(CLKCTRL_MCLKCTRLB, (CLKCTRL_PEN_bm | CLKCTRL_PDIV_8X_gc));
      #elif (F_CPU == 1000000) // 16MHz prescaled by 16
        /* Clock DIV16 */
        _PROTECTED_WRITE(CLKCTRL_MCLKCTRLB, (CLKCTRL_PEN_bm | CLKCTRL_PDIV_16X_gc));
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
      _PROTECTED_WRITE(CLKCTRL_MCLKCTRLB, 0x00);
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
      ADC0.CTRLC  = ADC_PRESC_DIV32_gc | ADC_REFSEL_VDDREF_gc | ADC_SAMPCAP_bm;
    #elif F_CPU  >= 12000000    // 16 MHz / 16 = 1.0 MHz,  20 MHz / 16 = 1.25 MHz
      ADC0.CTRLC  = ADC_PRESC_DIV16_gc | ADC_REFSEL_VDDREF_gc | ADC_SAMPCAP_bm;
    #elif F_CPU  >=  6000000    //  8 MHz /  8 = 1.0 MHz,  10 MHz /  8 = 1.25 MHz
      ADC0.CTRLC  =  ADC_PRESC_DIV8_gc | ADC_REFSEL_VDDREF_gc | ADC_SAMPCAP_bm;
    #elif F_CPU  >=  3000000    //  4 MHz /  4 = 1.0 MHz,   5 MHz /  4 = 1.25 MHz
      ADC0.CTRLC  =  ADC_PRESC_DIV4_gc | ADC_REFSEL_VDDREF_gc | ADC_SAMPCAP_bm;
    #else                       //  1 MHz /  2 = 500 kHz - the lowest setting
      ADC0.CTRLC  =  ADC_PRESC_DIV2_gc | ADC_REFSEL_VDDREF_gc | ADC_SAMPCAP_bm;
    #endif
    #if   (F_CPU == 6000000 || F_CPU == 12000000 || F_CPU == 24000000 || F_CPU ==25000000)
      ADC0.SAMPCTRL = (7); // 9 ADC clocks, 12 us
    #elif (F_CPU == 5000000 || F_CPU == 10000000 || F_CPU == 20000000)
      ADC0.SAMPCTRL = (13);   // 15 ADC clock,s 12 us
    #else
      ADC0.SAMPCTRL = (10); // 12 ADC clocks, 12 us
    #endif
    ADC0.CTRLD    = ADC_INITDLY_DLY16_gc;
    ADC0.CTRLA    = ADC_ENABLE_bm;
  #else
    /* On the 2-series maximum with internal reference is 3 MHz, so we will
     * target highest speed that doesn't exceed that and 16 ADC clocks sample
     * duration. */
    #if F_CPU     > 32000000            // 36 MHz /14 = 2.57 MHz
      ADC0.CTRLB  = ADC_PRESC_DIV10_gc; // 33 MHz /14 = 2.35 MHz
    #elif F_CPU  >= 30000000            // 32 MHz /12 = 2.67 MHz
      ADC0.CTRLB  = ADC_PRESC_DIV12_gc; // 30 MHz /12 = 2.50 MHz
    #elif F_CPU  >= 24000000            // 25 MHz /10 = 2.50 MHz
      ADC0.CTRLB  = ADC_PRESC_DIV10_gc; // 24 MHz /10 = 2.40 MHz
    #elif F_CPU  >= 20000000
      ADC0.CTRLB  = ADC_PRESC_DIV8_gc;  // 20 MHz / 8 = 2.50 MHz
    #elif F_CPU  >= 16000000
      ADC0.CTRLB  = ADC_PRESC_DIV6_gc;  // 16 MHz / 6 = 2.67 MHz
    #elif F_CPU  >= 12000000
      ADC0.CTRLB  = ADC_PRESC_DIV4_gc;  // 12 MHz / 4 = 3.00 MHz
    #elif F_CPU  >=  6000000            // 10 MHz / 4 = 2.50 MHz
      ADC0.CTRLB  = ADC_PRESC_DIV4_gc;  //  8 MHz / 4 = 2.00 MHz
    #else                               //  5 MHz / 2 = 2.50 MHz
      ADC0.CTRLB  = ADC_PRESC_DIV2_gc;  //  4 MHz / 2 = 2.00 MHz
    #endif                              //  1 MHz / 2 =  500 kHz
    ADC0.CTRLE = 15; // 15.5 without PGA, 16 with PGA, corresponding to 7.75 or 8 us.
    ADC0.CTRLA = ADC_ENABLE_bm | ADC_LOWLAT_bm;
    /* Default low latency mode on
     * Users can turn it off if they care about power consumption while ADC is on
     * and chip is awake, since these parts don't have the perverse ADC-left-on
     * behavior of classic AVRs. */
    ADC0.CTRLC = TIMEBASE_1US; // defined in Arduino.h.
    ADC0.PGACTRL = ADC_PGABIASSEL_3_4X_gc | ADC_ADCPGASAMPDUR_15CLK_gc;
    /* Note that we don't *enable* it automatically in init().
     * 3/4th bias is good up to 4 MHz CLK_ADC, 15 ADC Clocks to sample the PGA
     * up to 5 MHz, so within the regime of speeds that have to be compatible
     * with internal references, we are in the clear there. */
  #endif
}

// Must be called manually.
#ifdef ADC1
  __attribute__((weak)) void init_ADC1() {
    //                              30 MHz / 32 = 937 kHz,  32 MHz / 32 =  1 MHz.
    #if   F_CPU   > 24000000     // 24 MHz / 16 = 1.5 MHz,  25 MHz / 32 =  780 kHz
      ADC0.CTRLC  = ADC_PRESC_DIV32_gc | ADC_REFSEL_VDDREF_gc | ADC_SAMPCAP_bm;
    #elif F_CPU  >= 12000000    // 16 MHz / 16 = 1.0 MHz,  20 MHz / 16 = 1.25 MHz
      ADC0.CTRLC  = ADC_PRESC_DIV16_gc | ADC_REFSEL_VDDREF_gc | ADC_SAMPCAP_bm;
    #elif F_CPU  >=  6000000    //  8 MHz /  8 = 1.0 MHz,  10 MHz /  8 = 1.25 MHz
      ADC0.CTRLC  =  ADC_PRESC_DIV8_gc | ADC_REFSEL_VDDREF_gc | ADC_SAMPCAP_bm;
    #elif F_CPU  >=  3000000    //  4 MHz /  4 = 1.0 MHz,   5 MHz /  4 = 1.25 MHz
      ADC0.CTRLC  =  ADC_PRESC_DIV4_gc | ADC_REFSEL_VDDREF_gc | ADC_SAMPCAP_bm;
    #else                       //  1 MHz /  2 = 500 kHz - the lowest setting
      ADC0.CTRLC  =  ADC_PRESC_DIV2_gc | ADC_REFSEL_VDDREF_gc | ADC_SAMPCAP_bm;
    #endif
    #if   (F_CPU == 6000000 || F_CPU == 12000000 || F_CPU == 24000000 || F_CPU ==25000000)
      ADC0.SAMPCTRL = (7); // 9 ADC clocks, 12 us
    #elif (F_CPU == 5000000 || F_CPU == 10000000 || F_CPU == 20000000)
      ADC0.SAMPCTRL = (13);   // 15 ADC clock,s 12 us
    #else
      ADC0.SAMPCTRL = (10); // 12 ADC clocks, 12 us
    #endif
    ADC0.CTRLD    = ADC_INITDLY_DLY16_gc;
    ADC0.CTRLA    = ADC_ENABLE_bm;
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
  TCD0.CMPBCLR  = 509; // 510 counts, starts at 0, not 1!
  TCD0.CMPACLR  = 509;
  TCD0.CTRLC    = 0x80; // WOD outputs PWM B, WOC outputs PWM A
  TCD0.CTRLB    = TCD_WGMODE_ONERAMP_gc; // One Slope
  TCD0.CTRLA    = TIMERD0_PRESCALER; // OSC20M prescaled by 32, gives ~1.2 khz PWM at 20MHz.
}
#endif

void __attribute__((weak)) init_TCA0() {
  /*  TYPE A TIMER   */

  /* PORTMUX setting for TCA - don't need to set because using default
     UNLESS it's an 8-pin part, in which case we do.*/
  #ifdef __AVR_ATtinyxy2__
    PORTMUX.CTRLC = 1; // move WO0 output to PA7 so PA3 can be used with WO3
  #endif

  /* Enable Split Mode */
  TCA0.SPLIT.CTRLD = TCA_SPLIT_SPLITM_bm;

  // Only 1 WGM so no need to specifically set up.

  /* Period setting, 8-bit register in SPLIT mode */
  TCA0.SPLIT.LPER    = PWM_TIMER_PERIOD;
  TCA0.SPLIT.HPER    = PWM_TIMER_PERIOD;

  /* Default duty 0%, will re-assign in analogWrite() */
  // 2/1/2021: Why the heck are we bothering to set these AT ALL?! The duty cycles for *non-active* type A timer channels? Which are already initialized to 0 automatically?
  /*
    TCA0.SPLIT.LCMP0 = 0;
    TCA0.SPLIT.HCMP0 = 0;
    TCA0.SPLIT.LCMP1 = 0;
    TCA0.SPLIT.HCMP1 = 0;
    TCA0.SPLIT.LCMP2 = 0;
    TCA0.SPLIT.HCMP2 = 0;
  */

  /* Use prescale appropriate for system clock speed
   * Detect conflict between wiring.c and timers.h if we spot them, as that indicates
   * a defect in the core and would result in extremely bad behavior
   */

  #if (F_CPU > 30000000) // use 256 divider when clocked over 30 MHz
    #if defined(MILLIS_USE_TIMERA0) && (TIME_TRACKING_TIMER_DIVIDER != 256)
      #error "wiring.c and timers.h want to set millis timer TCA0 to different divider"
    #endif
    TCA0.SPLIT.CTRLA   = (TCA_SPLIT_CLKSEL_DIV256_gc) | (TCA_SPLIT_ENABLE_bm);
  #elif (F_CPU > 5000000) // use 64 divider unless it's 5 MHz or under
    #if defined(MILLIS_USE_TIMERA0) && (TIME_TRACKING_TIMER_DIVIDER != 64)
      #error "wiring.c and timers.h want to set millis timer TCA0 to different divider"
    #endif
    TCA0.SPLIT.CTRLA   =  (TCA_SPLIT_CLKSEL_DIV64_gc) | (TCA_SPLIT_ENABLE_bm);
  #elif (F_CPU > 1000000) // anything above 1 MHz
    #if defined(MILLIS_USE_TIMERA0) && (TIME_TRACKING_TIMER_DIVIDER != 16)
      #error "wiring.c and timers.h want to set millis timer TCA0 to different divider"
    #endif
    TCA0.SPLIT.CTRLA   =  (TCA_SPLIT_CLKSEL_DIV16_gc) | (TCA_SPLIT_ENABLE_bm);
  #else /* for 1 MHz and lower */
    #if defined(MILLIS_USE_TIMERA0) && (TIME_TRACKING_TIMER_DIVIDER != 8)
      #error "wiring.c and timers.h want to set millis timer TCA0 to different divider"
    #endif
    TCA0.SPLIT.CTRLA   =   (TCA_SPLIT_CLKSEL_DIV8_gc) | (TCA_SPLIT_ENABLE_bm);
  #endif
}


__attribute__((weak)) void onPreMain() {;}
__attribute__((weak)) void onBeforeInit() {;}
__attribute__((weak)) uint8_t onAfterInit() {return 1;}
