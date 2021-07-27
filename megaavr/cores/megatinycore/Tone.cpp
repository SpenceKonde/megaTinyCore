/* Tone.cpp

  A Tone Generator Library

  Written by Brett Hagman

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

  -------- ----------- --------------------------------------------------------------------------------------
   0001    B Hagman    09/08/02 Initial coding
   0002    B Hagman    09/08/18 Multiple pins
   0003    B Hagman    09/08/18 Moved initialization from constructor to begin()
   0004    B Hagman    09/09/26 Fixed problems with ATmega8
   0005    B Hagman    09/11/23 Scanned prescalars for best fit on 8 bit timers
                       09/11/25 Changed pin toggle method to XOR
                       09/11/25 Fixed timer0 from being excluded
   0006    D Mellis    09/12/29 Replaced objects with functions
   0007    M Sproul    10/08/29 Changed #ifdefs from cpu to register
   0008    S Kanemoto  12/06/22 Fixed for Leonardo by @maris_HY
   0009    J Reucker   15/04/10 Issue #292 Fixed problems with ATmega8 (thanks to Pete62)
   0010    jipp        15/04/13 added additional define check #2923
   0011    E Roy       13/02/18 ported to ATmega4809
  1.3.3    S Konde      3/30/21 Fixed bug where output could be left high.
                       Fixed bug that would cause anomalies on PORTB through use of
                       a NULL pointer, which points rght at the low IO space. Just return
                       if the pin isn't a valid pin.
                       Fix anomalous silent pointless interrupts for duration of a
                       frequency=0 "tone" Now I just drive pin low and turn off timer.
                        You asked for 0 Hz? That's 0 Hz alright!
                       In all cases, we ave to avoid leaving it high because
                       that can damage speakers, so I am told..
                       Just return and turn off tone if it's the current tone pin
                       bit_mask was declared volatile; but it shouldn't.
                       _pin was not decared volatile when it should be.
                       Fix bug where high-pitch tones old overflow when calculating
                       duration (the intermediate 2 * frequency was an unsigned int,
                       even though next operation multipolied it by an unsigned long
                       - simply reoved the 2* altogether and divide by 500 at end instead
                       of 1000.
                       Long tones (where frequency * duration > 2.145 billion resulted in an
                       intermediate overflowing before the divide by 1000. removing the
                       2* doubled this limit. But high frequencies and long durations (where
                       frequency * duration would overflow a unsigned long) still break it.
                       I added a way to distribute the division before the multiplication
                       if the duration was long enough to start worrying about. But I decided
                       not to enable by on all parts - it is SUPPORT_LONG_TONES - if defined
                       and 1, when duration is over 2^16, we split that line into
                       (frequency/5) * (duration/100) This version was developed for DxCore
                        1.3.3 and megaTinyCore 2.3.0. The former always uses the bulkier
                       implementation, while the latter does only on parts with 16k+ flash.
1.3.6      S Konde      4/17/21 Fixed bug that would break compilation of all sketches if TCB0
                       was used for millis on parts without a TCB1. Now, we intentionally break
                       the compilation only if the user code actually calls tone() or noTone()
                       This version was developed for DxCore 1.3.6 and megaTinyCore 2.3.2
1.3.7      S Konde      7/21/21 reorganized code, improved comments. caught a couple of corner
                       cases which could cause odd behavior in obscure cases. Added a check
                       for ENABLE_TCB_PWM which will turn the restoration of PWM mode on and
                       off - will make it easier to move this to DxCore without breakage.
                       I think the cleanup more or less exactly got enough flash back that
                       the coverage of corner cases cost.
  **********************************************************************************************************/
/************************************************************************************************************
    S. Konde 7/21/21:
I have to wonder a few things here:
  1. We have option for available tone pins, though very little of whatever else
      would have been needed to support it is present. I guess on DxCore, that *could*
      be increased, but a great deal of plumbing for it would also be needed - multiple pins, multiple timers...
      Generally "a damned big deal" to implement, with a large number of design decisions.
      What is strange is that I can find no sign of Arduino having supported more than one simultaneous tone pin, which
      is the only case where this #define makes sense. to have.
  2. In light of the fact that tone does not let you output multiple tones on multiple pins at once, one could
      argue that noTone() should shut off the tone and ignore the argument. But the official API says ignore
      noTone(wrong_pin)
My tentative ruling is that:
  A. Any polyphonic tone functionality belongs in a library, tone should never have AVAILABLE_TONE_PINS =/= 1.
    - or possibly several libraries. One approach would be to conditionally compile the ISRs for timers based on a #define
    that would have to be used before the library is #included; that file could check for ARDUINO_MAIN, and define
    the ISRs if and only if it is (otherwise it would cause multiple definition error if included by sketch and other library).
    Another approach would have the functions to control the tone output driven by each timer each in a separate file, or have
    multiple instances of a TCB tone class, each instantiated in it's own file. The stock core has done this with the
    HardwareSerial class since the dawn of time. Thus also not creating ISRs for timers we're not 'tone()ing' with.
    The separate files would also provide an efficient means of keeping the variables that track the state of each
    timer's tons separate..
    I suspect that one could do worse than using this file, with liberal addition of the static keyword, as a basis
    for those timer-specific tone() libraries. Personally, I don't feel like I have a good idea of what features
    would be desirable for a tone library of that sort. I'm not an audio guy, and I have never connected a buzzer to
    an MCU. I test these libraries with the 'scope (hell, I wouldn't be able to tell a 1kHz tone from a hole in the
    ground without looking at it). In that hypothetical library, one design decision would have to be how to deal with
    the most important part, that being turning the noisy thing off - basically, whether to implement multiTone() and
    noMultiTone(pin), or to have things like `toneTimerTCB0.tone()/toneTimerTCB0.noTone()`. I am inclined
    to think the second approach is a more sound one. If anytone wants to write such a librarty, I'm haoppy to adcise
  B. For the default noTone() that does not involve a library, pin should he required. Arguanbly there should be  two
    versions of it, with and without a pin argument, where ther latter would shut down tone wherever it is.
    ************************************************************************************************************/

#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "Arduino.h"
#include "pins_arduino.h"

#define AVAILABLE_TONE_PINS 1

#if defined(MILLIS_USE_TIMERB0)
  #if defined(TCB1)
    #define USE_TIMERB1
  #else
    #define TONE_UNAVAILABLE
  #endif
#else
  #define USE_TIMERB0
#endif

#ifndef TONE_UNAVAILABLE
  static volatile TCB_t* _timer =
  #if defined(USE_TIMERB0)
  &TCB0;
  #endif
  #if defined(USE_TIMERB1)
  &TCB1;
  #endif

  volatile uint8_t _pin = NOT_A_PIN;
  // timerx_toggle_count:
  //  > 0 - duration specified
  //  = 0 - stopped
  //  < 0 - infinitely (until stop() method called, or new tone() called)

  volatile long timer_toggle_count;
  volatile uint8_t *timer_outtgl_reg;
  static uint8_t timer_bit_mask;
  uint8_t timer_cycle_per_tgl;
  uint8_t timer_cycle_per_tgl_count;

  // helper functions
  static void disableTimer();
#endif


// frequency (in hertz) and duration (in milliseconds).
#ifndef TONE_UNAVAILABLE
  void tone(uint8_t pin, unsigned int frequency, unsigned long duration) {
    // Believe it or not, we don't need to turn off interrupts!
    if (frequency == 0) {
      noTone(pin);
      /* turn it off (frequency 0, right? Note that this won't do anything
       * unless that was the pin outputting tone. If it wasn't nothing
       * will change. But there's no way of outputting a "0 Hz frequency"
       * and a 1 Hz frequency makes even less sense. (they did ask for 0)
       * But outputting a 1 Hz frequency on nothing (bitmask was cleared)
       * achieves nothing except generate do-nothing interrupts regularly.
       * When it is clear that provided arguments are sufficiently wrong
       * that you can't do anything useful with them, at least don't
       * do useless things instead. */
      return;
    }
    // Get pin related info
    uint8_t bit_mask = digitalPinToBitMask(pin);
    if (bit_mask == NOT_A_PIN) return;
    /* Same logic as above holds here except what it did was actively
     * disruptive. When library code takes user supplied pins and looks
     * up bitmasks and position, always check that one of them isn;t
     * NOT_A_PIN - before you get the PORT struct. Because the port
     * struct function cannot have it's result used safely. For bad
     * pins, yes it returns a null pointer, 0x0000. But not only is
     * that a valid register, it's an SFR (VPORTA.DIR)! So we don't
     * want to end up writing to that. The convention of NULL pointers
     * being invalid to work with is NOT safe to assume on embedded
     * systems. */


    long toggle_count;
    // Calculate the toggle count
    if (duration > 0) {    // Duration defined
      #if defined(SUPPORT_LONG_TONES) && SUPPORT_LONG_TONES == 1
      if (duration > 65536) {
        toggle_count = (frequency / 5) * (duration / 100);
      } else {
        toggle_count = (frequency      * duration) / 500;
      }
      #else
        toggle_count = (frequency      * duration) / 500;
      #endif
    } else {
      // Duration not specified -> infinite
      // Represented internally by toggle_count = -1
      toggle_count = -1;
    }
    // Calculate compare value
    int8_t divisionfactor = 0; //no prescale, toggles at twice the frequency

    // Timer settings -- will be type B timer or bust....
    uint32_t compare_val = ((F_CPU / frequency) >> 1);
    // We're are however disabling the timer. There may be one final interrupt at the moment we do so
    // but that's not a problem
    if (compare_val < 0x10000) { /* previously this tested for divisionfactor == 1,
      * but that relied on us having already gone through the while loop to
      * adjust it, which we haven't done yet, but we want to do this *after* the
      * timeconsuming division operation, but *before* we actually change any
      * other settings, because this is the point at which we stop the timer -
      * hence it needs to be the first to be set if we want to leave interrupts on*/
      _timer->CTRLA = TCB_CLKSEL_DIV1_gc;
    } else {
      _timer->CTRLA = TCB_CLKSEL_DIV2_gc;
      divisionfactor--;
    }
    while ((compare_val > 0x10000) && (divisionfactor < 6)) {
      // If the "else" branch above was followed, this is always true initially.
      compare_val = compare_val >> 1;
      divisionfactor++;
    }
    if (--compare_val > 0xFFFF) {
      // if still too high, divisionfactor reached 6 (each phase lasts 64 overflows), meaning the
      // user is using tone() to generate something that is not, by any stretch of the word, a
      // "tone", and they should be generating it through other means.
      compare_val = 0xFFFF; // do the best we can
    }

    // Anyway - so we know that the new pin is valid....
    if (_pin != pin) {  // ...let's see if we're using it already.
      if (_pin != NOT_A_PIN) { // If not - were we using one before?
        // If we were, we're gonna be in a world of hurt if we don't
        // turn it off before we actually start reconfiguring stuff
        // *(timer_outtgl_reg - 5) = timer_bit_mask; // pinMode(_pin, INPUT);    (write dirclr for old pin)
        // Apparently maybe it is indended for the pin to be left as an output?
        *(timer_outtgl_reg - 1) = timer_bit_mask; // digitalWrite(_pin, LOW); (write outclr for old pin)
      }
      // whether or not we _were_ using a pin, we are now, so configure the new one as an output...
      PORT_t *port             = digitalPinToPortStruct(pin);           // Since not known at compile time, use PORTs not VPORTS.
      timer_bit_mask           = bit_mask;                              // We no longer need old pin's bit_mask
      timer_outtgl_reg         = (volatile uint8_t *) &(port->OUTTGL);  // or toggle register.
      *(timer_outtgl_reg - 1)  = bit_mask;                              // digitalWrite(pin, LOW);
      *(timer_outtgl_reg - 6)  = bit_mask;                              // pinMode(pin, OUTPUT);
    }
    // Save the results of our calculations
    timer_toggle_count         = toggle_count;
    timer_cycle_per_tgl        = 1 << divisionfactor;     // 1, 2, 4, 8, 16, 32, or 64 - toggle pin once per this many cycles...
    timer_cycle_per_tgl_count  = timer_cycle_per_tgl;     // running count of remaining toggle cycles.
    _timer->CCMP               = compare_val;             // ...and each cycle is this many timer ticks long
    _timer->CTRLB              = TCB_CNTMODE_INT_gc;      // Set mode to Periodic Interrupt mode.
    _timer->CNT                = 0;                       // Not strictly necessary, but ensures there's no glitch.
    _pin                       = pin;                     // Record new pin number.
    _timer->INTCTRL            = TCB_CAPTEI_bm;           // Enable the interrupt (flag is already cleared)
    _timer->CTRLA             |= TCB_ENABLE_bm;           // Everything is ready - Enable timer!
  }
#else
  void tone(__attribute__ ((unused)) uint8_t pin, __attribute__ ((unused)) unsigned int frequency, __attribute__ ((unused)) unsigned long duration) {
    badCall("TCB0 used for millis, no other TCBs on this part; tone requires exclusive use of a type B timer, use a differemt millis timer or a tinyAVR with a second TCB (any 2-series, or 1-series with 16k+ flash)");
  }
#endif

// pin which currently is being used for a tone
#ifndef TONE_UNAVAILABLE
void noTone(uint8_t pin) {
    if (pin == _pin) {
      uint8_t old_SREG = SREG;  // Save SREG
      cli();                    // Interrupts off
      timer_toggle_count = 0;   // clear this one
      _pin = NOT_A_PIN;
      disableTimer(); // End with pin LOW, otherwise can damage some speakers.
      SREG = old_SREG;
    }
  }
#else
void noTone(__attribute__ ((unused)) uint8_t pin) {
    badCall("TCB0 used for millis, no other TCBs on this part; tone requires exclusive use of a type B timer, use a differemt millis timer or a tinyAVR with a second TCB (any 2-series, or 1-series with 16k+ flash)");
  }
#endif

// helper functiodn for ending tone.
/* Works for all timers -- the timer being disabled will go back to the
    configuration it had on startup */
static void disableTimer() {
  _timer->CTRLA     = 0; // disable timer
  _timer->INTCTRL   = 0; // disable the timer  interrupts, otherwise if something else configures it and assumes that it's in the reset configuration
  // and so doesn't write INTCTRL (because it doesn't use interrupts), a nonexistent interrupt vector would be called, ungracefully hanging the system.
  _timer->INTFLAGS  = 0xFF; // Make sure the flags are cleared (flags can be set without their interrupt being enabled, these will fire as soon as it is.
  _pin              = NOT_A_PIN; // and clear _pin.
  #if defined(ENABLE_TCB_PWM) && ENABLE_TCB_PWM == 1
    // RESTORE PWM FUNCTIONALITY, for use with cores that use the TCBs for PWM.
    // This section adds 12 words of flash and as many clock cycles, and could probably be done less expensively if the compiler was smarter about
    // using ldd/std when it offered a speed/flash advantage. But it isn't, so it doesn't. =
    _timer->CTRLB = (TCB_CNTMODE_PWM8_gc); // 8 bit PWM mode, but do not enable output yet, will do in analogWrite()
    _timer->CCMPL = PWM_TIMER_PERIOD;      // Assign 8-bit period, probably 254.
    _timer->CCMPH = PWM_TIMER_COMPARE;     // We need to set low byte in order for high byte to be written (see silicon errata).
    _timer->CTRLA = (TCB_CLKSEL_CLKTCA_gc) | (TCB_ENABLE_bm); // Use TCA clock (250kHz) and enable
  #endif
  *(timer_outtgl_reg - 1) = timer_bit_mask; // Write OUTCLR, so we are sure to end with pin LOW.
  }
#endif


#ifndef TONE_UNAVAILABLE
  #if defined USE_TIMERB0
  ISR(TCB0_INT_vect)
  #elif defined USE_TIMERB1
  ISR(TCB1_INT_vect)
  #endif
  {
    if (!(--timer_cycle_per_tgl_count)) { //Are we ready to toggle? pre-decrement, then see if we're at 0 yet.
      timer_cycle_per_tgl_count   = timer_cycle_per_tgl;  // reset countdown
      *timer_outtgl_reg           = timer_bit_mask;       // toggle the pin
      if (timer_toggle_count > 0) {  //if duration was specified, decrement toggle count.
          timer_toggle_count--;
      } else if (timer_toggle_count == 0) {       // If toggle count = 0 we are done.
        disableTimer();
      } //  otherwise timer_toggle_count wasn't supplied, go on until noTone() called
    }
    _timer->INTFLAGS = TCB_CAPT_bm; // Clear flag
  }
#endif
