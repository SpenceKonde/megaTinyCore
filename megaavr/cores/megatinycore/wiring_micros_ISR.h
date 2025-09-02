/*
  wiring_micros_ISR.h - Three functions constituted the majority of wiring.c, and they're the uglest functions
  Originally part of Arduino - http://www.arduino.cc/
  Copyright (c) 2005-2006 David A. Mellis


  Copyright (c) 2025- Spence Konde
  mTC version of the "three giants", micros, the micros ISR, and delayMicroseconds.
  All are huge, all are ugly as sin, covered with inline asm, and made navigating wiring.c very difficult

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
// No include guard because it's included from only a single place. It is absolutely under no circumstances
// expected to bring anything but woe and misery (and compile errors) if included from anything else


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



#if !(defined(MILLIS_USE_TIMERNONE) || defined(MILLIS_USE_TIMERRTC))
  unsigned long micros() {
    uint32_t overflows, microseconds;
    #if (MILLIS_TYPE == MILLIS_TCB) || (MILLIS_TYPE == MILLIS_TCD)
      uint16_t ticks;
    #else /* TCA */
      uint8_t ticks;
    #endif
    uint8_t flags;
    /* Save current state and disable interrupts */
    uint8_t oldSREG = SREG;
    cli(); /* INTERRUPTS OFF */
    #if (MILLIS_TYPE == MILLIS_TCB)
      ticks = _timer->CNT;
      flags = _timer->INTFLAGS;
    #elif defined(MILLIS_USE_TIMERA0)
      ticks = TCA0.SPLIT.HCNT;
      flags = TCA0.SPLIT.INTFLAGS;
    #elif defined(MILLIS_USE_TIMERD0)
      TCD0.CTRLE = TCD_SCAPTUREA_bm;
      while (!(TCD0.STATUS & TCD_CMDRDY_bm)); // wait for sync - should be only one iteration of this loop
      flags = TCD0.INTFLAGS;
      ticks = TCD0.CAPTUREA;
    #else
      #error "Can't happen: Impossible millis source made it to wiring.c"
    #endif // end getting ticks
    /* If the timer overflow flag is raised, and the ticks we read are low, then the timer has rolled over but
     * ISR has not fired. If we already read a high value of ticks, either we read it just before the overflow,
     * so we shouldn't increment overflows, or interrupts are disabled and micros isn't expected to work so it
     * doesn't matter.
     * Get current number of overflows and timer count */
    #if (MILLIS_TYPE != MILLIS_TCB)
      overflows = timingStruct.timer_overflow_count;
    #else
      overflows = timingStruct.timer_millis;
    #endif
    /* Turn interrupts back on, assuming they were on when micros was called. */
    SREG = oldSREG; /* INTERRUPTS ON */
    #if defined(MILLIS_USE_TIMERD0)
      if ((flags & TCD_OVF_bm) && (ticks < 0x07)) {
    #elif defined(MILLIS_USE_TIMERA0)
      ticks = (TIME_TRACKING_TIMER_PERIOD) - ticks;
      if ((flags & TCA_SPLIT_HUNF_bm) && (ticks < 0x04)) {
    #else // timerb
      if ((flags & TCB_CAPT_bm) && !(ticks & 0xFF00)) {
    #endif
    #if (MILLIS_TYPE == MILLIS_TCB) && !(F_CPU > 2000000UL)
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
        //microseconds =   overflows * (TIME_TRACKING_CYCLES_PER_OVF / 20) + ticks; // ticks value corrected above. *FIVE HUNDRED AND TEN DOES NOT EVENLY DIVIDE BY 20!* What the fuck?
        microseconds =   overflows * (TIME_TRACKING_CYCLES_PER_OVF / 20) + ticks + (overflows >> 1);
      #else
        microseconds = ((overflows * (TIME_TRACKING_CYCLES_PER_OVF / 16))
                          + (ticks * (TIME_TRACKING_CYCLES_PER_OVF / 16 / TIME_TRACKING_TIMER_PERIOD))); /* ticks is multiplied to handle the higher divisor at 1 MHz it. not needed at decent clock speeds */
      #endif
      #if defined(CLOCK_TUNE_INTERNAL) && !(F_CPU == 16000000UL || F_CPU ==  20000000UL || F_CPU ==  8000000UL || F_CPU ==  10000000UL || F_CPU ==  4000000UL || F_CPU ==  5000000UL)
        #warning "TCD is not supported as a millis timing source when the oscillator is tuned to a frequency other than 16 or 20 MHz. Timing results will be wrong - use TCA0 or a TCB."
      #endif
    #elif (MILLIS_TYPE == MILLIS_TCB)
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
       * bitshifts and provided a number of statistics to select based on. Backwards time travel must
       * never happenb, or if it does, it must be a shorter backward step than micros runtime - 1 us
       * otherwise delay() will break and timeouts can instantly expire when it is hit. Similarly,
       * one wants to avoid large jumps forward, and cases where more consecutive "actual" times
       * than absolutely necessary return the same value (time should flow at a constant rate).
       * Finally, the artifacts of the calculation that are unavoidable should be distributed uniformly.
       * Undershooting or overshooting 999 endpoint at the counter's maximum value is the usual
       * source of large jumps (at the overflow point) in either direction. Terms should, as much as
       * possible alternate between positive and negative to minimize artifacts.
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
#else // MILLIS_USE_TIMERRTC is defined, so we don't have micros
  /* We do not have a timebase sufficiently accurate to give microsecond timing. In fact, we barely have millisecond timing available
   * The microsecond delay counts clock cycles, and so it does still work. It is planned that a future library will switch the millis
   * pause millis before sleeping and turn on the RTC, tracking the passage of time to a much coarser resolution with that, and turn
   * it back on when waking from sleep, so people can keep time while sleeping without sacrificing micros().
   * In any event, as of 2.4.3 we now provide the stub below, which we hope is more useful than being told that micros() isn't defined.
   */
  unsigned long micros() {
    #if defined(MILLIS_USE_TIMERRTC)
      badCall("microsecond timekeeping is not supported when the RTC is used as the sole timekeeping timer (though delayMicroseconds() is)");
    #else
      badCall("micros() is not available because it has been disabled through the tools -> millis()/micros() menu");
    #endif
    return -1;
  }
#endif // MILLIS_USE_TIMERNONE code







  // Now for the ISRs. This gets a little bit more interesting now...

#if !defined(MILLIS_USE_TIMERNONE)
  #if defined (MILLIS_USE_TIMERRTC)
    ISR(MILLIS_TIMER_VECTOR) {
      if (RTC.INTFLAGS & RTC_OVF_bm) {
        timingStruct.timer_millis += MILLIS_INC;
      }
      RTC.INTFLAGS = RTC_OVF_bm | RTC_CMP_bm; // clear flag
    }
  #else
    ISR(MILLIS_TIMER_VECTOR, ISR_NAKED) {
      // Common Interrupt header for TCB, TCA and TCD;
      // Clears the Timer Interrupt flag and pushes the CPU Registers
      // 7 words / 7 clocks
      __asm__ __volatile__(
      "push       r24"            "\n\t" // Free up one more register to load values into
      "ldi        r24, %[CLRFL]"  "\n\t" // This is the TCB interrupt clear bitmap
      "sts   %[PTCLR],      r24"  "\n\t" // write to Timer interrupt status register to clear flag. 2 clocks for sts
      "in         r24,     0x3F"  "\n\t" // Load SREG
      "push       r24"            "\n\t" // and push it on the Stack
      "push       r30"            "\n\t" // First we make room for the pointer to timingStruct by pushing the Z registers
      "push       r31"            "\n\t" //
      ::  [CLRFL] "M" (_timerS.intClear),
          [PTCLR] "m" (*_timerS.intStatusReg));

    #if (MILLIS_TYPE == MILLIS_TCB)
      __asm__ __volatile__(
      "ld         r24,        Z"  "\n\t" // Z points to LSB of timer_millis, load the LSB
      #if (F_CPU > 2000000)            // if it's 1 or 2 MHz, millis timer overflows every 2ms, intentionally sacrificing resolution for reduced time spent in ISR
      "subi       r24,     0xFF"  "\n\t" // sub immediate 0xFF is the same as to add 1. (There is no add immediate instruction, except add immediate to word)
      #else
      "subi       r24,     0xFE"  "\n\t" // sub immediate 0xFE is the same as to add 2
      #endif
      "st           Z,      r24"  "\n\t" // Store incremented value back to Z
      "ldd        r24,      Z+1"  "\n\t" // now load the next higher byte
      "sbci       r24,     0xFF"  "\n\t" // because this is sbci, it treats carry bit like subtraction, and unless we did just roll over with the last byte,
      "std        Z+1,      r24"  "\n\t" // carry will be cleared. Thus, sbci 0xFF after a subi pressed into service to add, is the same as adc r1 after an add
      "ldd        r24,      Z+2"  "\n\t" // which is what we want.
      "sbci       r24,     0xFF"  "\n\t" // This gets repeated...
      "std        Z+2,      r24"  "\n\t" //
      "ldd        r24,      Z+3"  "\n\t" //
      "sbci       r24,     0xFF"  "\n\t" //
      "std        Z+3,      r24"  "\n\t" // ... until all 4 bytes were handled, at 4 clocks and 3 words per byte -> 16 clocks
      :: "z" (&timingStruct.timer_millis)
      ); // grrr, sublime highlights this as invalid syntax because it gets confused by the ifdef's and odd syntax on inline asm


      /* ISR in C:
        ISR (TCBx_INT_vect) {       // x depends on user configuration
          #if (F_CPU > 2000000)
            timer_millis += 1;
          #else
            timer_millis += 2;
          #endif
          _timer->INTFLAGS = TCB_CAPT_bm;   // reset Interrupt flag of TCBx
        }
      */
    #else // TCA0 or TCD0, also naked
    /*
      __asm__ __volatile__(
      // ISR prologue (overall 10 words / 10 clocks (+ loading of Z)):
      "push       r25"            "\n\t" // one extra Register needed
      // timer_fract handling (8 words / 10 clocks):
      "ldd        r24,      Z+8"  "\n\t" // lo8(timingStruct.timer_fract).
      "ldd        r25,      Z+9"  "\n\t" // hi8(timingStruct.timer_fract)
      "subi       r24,%[LFRINC]"  "\n\t" // use (0xFFFF - FRACT_INC) and use the lower and higher byte to add by subtraction
      "sbci       r25,%[HFRINC]"  "\n\t" // can't use adiw since FRACT_INC might be >63
      "std        Z+8,      r24"  "\n\t" // lo8(timingStruct.timer_fract)
      "std        Z+9,      r25"  "\n\t" // hi8(timingStruct.timer_fract)
      "subi       r24,%[LFRMAX]"  "\n\t" // subtract FRACT_MAX and see if it is lower
      "sbci       r25,%[HFRMAX]"  "\n\t" //

      #if MILLIS_INC != 0                // (6 words / 4 - 5 clocks, branches were optimize to create minimal diversion)
      "brlo    higher"            "\n\t" // if FRAC_MAX was not reached,
      "ldi        r24, %[MIINC]"  "\n\t" // load "normal" MILLIS_INC (0x00-MILLIS_INC)
      "rjmp      sub4"            "\n\t" // avoid overwriting r24
      "higher:"
      #else                              // (4 words, 2 - 4 clocks)
      "brlo   sub_end"            "\n\t" // if we know at compile time that MILLIS_INC is 0,
      #endif                             // we don't have to check it at runtime, saving two insn (tst, branch)

      "std        Z+8,      r24"  "\n\t" // Overwrite the just stored value with the decremented value
      "std        Z+9,      r25"  "\n\t" // seems counter-intuitive, but it requires less registers
      "ldi        r24, %[MINCD]"  "\n\t" // load MILLIS_INC that was decreased by 1 (0xFF-MILLIS_INC)

      // timer_millis handling (12 words / 16 clocks):
      "sub4:"
      "ldd        r25,      Z+4"  "\n\t" // lo16.lo8(timingStruct.timer_millis)
      "sub        r25,      r24"  "\n\t" //
      "std        Z+4,      r25"  "\n\t" //
      "ldd        r25,      Z+5"  "\n\t" // lo16.hi8(timingStruct.timer_millis)
      "sbci       r25,     0xFF"  "\n\t" //
      "std        Z+5,      r25"  "\n\t" //
      "ldd        r25,      Z+6"  "\n\t" // hi16.lo8(timingStruct.timer_millis)
      "sbci       r25,     0xFF"  "\n\t" //
      "std        Z+6,      r25"  "\n\t" //
      "ldd        r25,      Z+7"  "\n\t" // hi16.hi8(timingStruct.timer_millis)
      "sbci       r25,     0xFF"  "\n\t" //
      "std        Z+7,      r25"  "\n\t" //
      "sub_end:"                         // only used if MILLIS_INC == 0
      // timer_overflow_count handling (12 words / 16 clocks):
      "ldd        r25,      Z+0"  "\n\t" // lo16.lo8(timingStruct.timer_overflow_count)
      "subi       r25,     0xFF"  "\n\t" //
      "std        Z+0,      r25"  "\n\t" //
      "ldd        r25,      Z+1"  "\n\t" // lo16.hi8(timingStruct.timer_overflow_count)
      "sbci       r25,     0xFF"  "\n\t" //
      "std        Z+1,      r25"  "\n\t" //
      "ldd        r25,      Z+2"  "\n\t" // hi16.lo8(timingStruct.timer_overflow_count)
      "sbci       r25,     0xFF"  "\n\t" //
      "std        Z+2,      r25"  "\n\t" //
      "ldd        r25,      Z+3"  "\n\t" // hi16.hi8(timingStruct.timer_overflow_count)
      "sbci       r25,     0xFF"  "\n\t" //
      "std        Z+3,      r25"  "\n\t" //
      // ISR epilogue (7 words / 15/16 clocks):
      "pop        r25"            "\n\t"  // new: total 72 - 74 clocks, 55 words / 53 - 75 clocks and 53 words with MILLIS_INC == 0
      :: "z" (&timingStruct),             // old: total 77 - 79 clocks total, and 58 words, vs 104-112 clocks and 84 words
        [LFRINC] "M" (((0x0000 - FRACT_INC)    & 0xFF)),
        [HFRINC] "M" (((0x0000 - FRACT_INC)>>8 & 0xFF)),
        [LFRMAX] "M" ((FRACT_MAX    & 0xFF)),
        [HFRMAX] "M" ((FRACT_MAX>>8 & 0xFF)),
        [MIINC]  "M" ((0x0000 - MILLIS_INC) & 0xFF),
        [MINCD]  "M" ((0xFFFF - MILLIS_INC) & 0xFF)
      );
  */

      __asm__ __volatile__(
      // ISR prologue (overall 10 words / 10 clocks (+ loading of Z)):
      "push       r25"            "\n\t" // second byte
      // timer_overflow_count handling (4 words / 4 + (18) + 1 = 23 clocks):
      "set"                       "\n\t" // remember to go back here
      "ldi        r24,     0xFF"  "\n\t" // first byte to be subtracted of 4, rest will be 0xFF
      "rjmp      sub4"            "\n\t" // jump down to sub/sbci
      "ovf_end:"                  "\n\t" // jump back to here afterwards
      "clt"                       "\n\t" // make sure to not jump back again

      // timer_fract handling (8 words / 10 clocks) (Z += 4):
      "ldd        r24,      Z+4"  "\n\t" // lo8(timingStruct.timer_fract).
      "ldd        r25,      Z+5"  "\n\t" // hi8(timingStruct.timer_fract)
      "subi       r24,%[LFRINC]"  "\n\t" // use (0xFFFF - FRACT_INC) and use the lower and higher byte to add by subtraction
      "sbci       r25,%[HFRINC]"  "\n\t" // can't use adiw since FRACT_INC might be >63
      "std        Z+4,      r24"  "\n\t" // lo8(timingStruct.timer_fract)
      "std        Z+5,      r25"  "\n\t" // hi8(timingStruct.timer_fract)
      "subi       r24,%[LFRMAX]"  "\n\t" // subtract FRACT_MAX and see if it is lower
      "sbci       r25,%[HFRMAX]"  "\n\t" //

      #if MILLIS_INC > 0                 // (6 words / 4 - 5 clocks, branches were optimize to create minimal diversion)
      "brlo    higher"            "\n\t" // if FRAC_MAX was not reached,
      "ldi        r24, %[MIINC]"  "\n\t" // load "normal" MILLIS_INC (0x00-MILLIS_INC)
      "rjmp      sub4"            "\n\t" // avoid overwriting r24
      "higher:"
      #else                              // (4 words, 2 - 4 clocks)
      "brlo   sub_end"            "\n\t" // if we know at compile time that MILLIS_INC is 0,
      #endif                             // we don't have to check it at runtime, saving two insn (tst, branch)

      "std        Z+4,      r24"  "\n\t" // Overwrite the just stored value with the decremented value
      "std        Z+5,      r25"  "\n\t" // seems counter-intuitive, but it requires less registers
      "ldi        r24, %[MINCD]"  "\n\t" // load MILLIS_INC that was decreased by 1 (0xFF-MILLIS_INC)

      // subtracting 4 bytes from a dword (13 words / 17 clocks)
      "sub4:"
      "ld         r25,        Z"  "\n\t" // lo16.lo8(timingStruct.timer_millis)
      "sub        r25,      r24"  "\n\t" //
      "st          Z+,      r25"  "\n\t" //
      "ld         r25,        Z"  "\n\t" // lo16.hi8
      "sbci       r25,     0xFF"  "\n\t" //
      "st          Z+,      r25"  "\n\t" //
      "ld         r25,        Z"  "\n\t" // hi16.lo8
      "sbci       r25,     0xFF"  "\n\t" //
      "st          Z+,      r25"  "\n\t" //
      "ld         r25,        Z"  "\n\t" // hi16.hi8
      "sbci       r25,     0xFF"  "\n\t" //
      "st          Z+,      r25"  "\n\t" //
      "brts   ovf_end"            "\n\t" // If T bit is set, we need to go back up
      "sub_end:"
      // ISR epilogue (7 words / 15/16 clocks):
      "pop        r25"            "\n\t"
      :: "z" (&timingStruct),
        [LFRINC] "M" (((0x0000 - FRACT_INC)    & 0xFF)),
        [HFRINC] "M" (((0x0000 - FRACT_INC)>>8 & 0xFF)),
        [LFRMAX] "M" ((FRACT_MAX    & 0xFF)),
        [HFRMAX] "M" ((FRACT_MAX>>8 & 0xFF)),
        [MIINC]  "M" ((0x0000 - MILLIS_INC) & 0xFF),
        [MINCD]  "M" ((0xFFFF - MILLIS_INC) & 0xFF)
      );
    #endif /* (defined(MILLIS_USE_TIMERB0) || defined(MILLIS_USE_TIMERB1) || defined(MILLIS_USE_TIMERB2) || defined(MILLIS_USE_TIMERB3) || defined(MILLIS_USE_TIMERB4)) */
    // Common ISR Epilogue for TCA, TCB and TCD, popping register in reverse Order
    // 6 words, 14 clocks
    __asm__ __volatile__(
      "pop        r31"            "\n\t"
      "pop        r30"            "\n\t" // 6 more clocks popping registers in reverse order.
      "pop        r24"            "\n\t" // pop r24 to get the old SREG value - 2 clock
      "out       0x3F,      r24"  "\n\t" // restore SREG - 1 clock
      "pop        r24"            "\n\t"
      "reti"                      "\n\t" // and 4 clocks for reti
      ::
      );


    }
  #endif /* defined (MILLIS_USE_TIMERRTC)*/
#endif
