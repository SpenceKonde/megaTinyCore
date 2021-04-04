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

  -------- ----------- -------- --------
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
                       intermediate overflowing before the divide by 1000. removing the 2* doubled this limit.
                       But high frequencies and long durations (where frequency * duration would overflow
                       a unsigned long) still break it. I added a way to distribute the division before
                       the multiplication if the duration was long enough to start worrying about. But I
                       decided not to enable by default, it is SUPPORT_LONG_TONES - if defined and 1,
                       when duration is over 2^16, we split that line into (frequency/5) * (duration/100)

                       This version was developed for DxCore 1.3.3 and megaTinyCore 2.3.0
  *************************************************/

#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "Arduino.h"
#include "pins_arduino.h"

/* For more than one tone, change AVAILABLE_TONE_PINS and uncomment the correct
    number of timers
*/
#define AVAILABLE_TONE_PINS 1

#if defined(MILLIS_USE_TIMERB0)
  #define USE_TIMERB1
#else
  #define USE_TIMERB0
#endif

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
//  < 0 - infinitely (until stop() method called, or new play() called)

volatile long timer_toggle_count;
volatile uint8_t *timer_outtgl_reg;
static uint8_t timer_bit_mask;
uint8_t timer_cycle_per_tgl;
uint8_t timer_cycle_per_tgl_count;

// helper functions
static void disableTimer();



// frequency (in hertz) and duration (in milliseconds).
void tone(uint8_t pin, unsigned int frequency, unsigned long duration)
{
    long toggle_count = 0;
    uint32_t compare_val = 0;

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

    // Get pin related stuff
    uint8_t bit_mask = digitalPinToBitMask(pin);
    if (bit_mask == NOT_A_PIN) return;
      /* Same logic as above holds here except what it did was actively
       * disruptive. When library code takes user supplied pins and looks
       * up bitmasks and position, always check that one of them isn;t
       * NOT_A_PIN - before you get the PORT struct. Because the port
       * struct - yes it returns a null pointer, 0x0000. The start of the
       * low I/O space.... so anything you do with that could scribble
       * over (in modernAVR's case) VPORT registers! */

    // Now that we know it's a pin, we can get the register to toggle output.
    PORT_t *port = digitalPinToPortStruct(pin);
    volatile uint8_t *port_outtgl = (volatile uint8_t *)&(port->OUTTGL);

    if (_pin != pin) {
        *(port_outtgl-1) = bit_mask; // digitalWrite(pin,LOW); (set outclr for new pin)
        *(port_outtgl-6) = bit_mask; // pinMode(pin, OUTPUT); (set dirset for new pin)
    }

    // Calculate the toggle count
    if (duration > 0) {    // Duration defined
      #if defined(SUPPORT_LONG_TONES) && SUPPORT_LONG_TONES == 1
      if (duration > 65536) {
        toggle_count =  (frequency/5) * (duration / 100);
      } else {
        toggle_count =  (frequency * duration) / 500;
      }
      #else
        toggle_count = (frequency * duration) / 500;
      #endif
    } else {            // Duration not defined -- tone until noTone() call
        toggle_count = -1;
    }

    // Calculate compare value
    uint8_t divisionfactor = 1; //no prescale, toggles at half the frequency

    compare_val = ((F_CPU / frequency)>>1);
    while ((compare_val > 0x10000) && (divisionfactor<8))
    {
      compare_val = compare_val >> 1;
      divisionfactor++;
    }
    if (--compare_val > 0xFFFF) {
      //if still too high, divisionfactor reached 8 (/256), corresponding to 1Hz
      compare_val = 0xFFFF; //do the best we can.
    }


    // Timer settings -- will be type B
    // Believe it or not, we still don't need to turn off interrupts!
    // We're are however disabling the timer. There may be one final interrupt at the moment we do so
    // but that's not a problem
    if(divisionfactor==1)
    {
      _timer->CTRLA = TCB_CLKSEL_DIV1_gc;
    } else { //division factor between 2 and 8
      _timer->CTRLA = TCB_CLKSEL_DIV2_gc;
      divisionfactor--; //now between 1 and 7 //and while it is disabed, we can reconfigure in peace.
    }
    divisionfactor--; //now between 0 and 6

    // Save the results of our calculations
    timer_cycle_per_tgl = 1 << divisionfactor; //1, 2, 4, 8, 16, 32, or 64 - toggle pin once per this many cycles
    timer_cycle_per_tgl_count = timer_cycle_per_tgl;
    _timer->CCMP = compare_val; // and each cycle is this many timer ticks long
    // Timer to Periodic interrupt mode
    _timer->CTRLB = TCB_CNTMODE_INT_gc;
    _timer->CNT = 0; //not strictly necessary
    // Write compare register
    uint8_t oldSREG = SREG;
    cli();
    _pin = pin; // now we can finally safely set this!
    _timer->INTCTRL = TCB_CAPTEI_bm; //enable the interrupt
    // Store these values for this call to tone in the variables the ISR uses.
    timer_outtgl_reg = port_outtgl;
    timer_bit_mask = bit_mask;
    timer_toggle_count = toggle_count;
    // Enable timer
    _timer->CTRLA |= TCB_ENABLE_bm;
     // Enable interrupts
    SREG = oldSREG;
}

// pin which currently is being used for a tone
void noTone(uint8_t pin)
{
    if (pin == _pin) {
        timer_toggle_count = 0;
        //disableTimer();
        // Keep pin low after disabling of timer
         *(timer_outtgl_reg-6) = timer_bit_mask; // drive pin low
        _pin = NOT_A_PIN;
    }
}

// helper function for noTone()
/* Works for all timers -- the timer being disabled will go back to the
    configuration it had to output PWM for analogWrite() */
static void disableTimer()
{
    // Reinit back to producing PWM -- timer will be type B
    // Disable interrupt
    _timer->INTCTRL = 0;
    // Disable timer
    _timer->CTRLA = 0;
    _pin=NOT_A_PIN;

#if 0
    // RESTORE PWM FUNCTIONALITY:
    /* 8 bit PWM mode, but do not enable output yet, will do in analogWrite() */
    _timer->CTRLB = (TCB_CNTMODE_PWM8_gc);
    /* Assign 8-bit period */
    _timer->CCMPL = PWM_TIMER_PERIOD;
    /* default duty 50%, set when output enabled */
    _timer->CCMPH = PWM_TIMER_COMPARE;
    /* Use TCA clock (250kHz) and enable */
    /* (sync update commented out, might try to synchronize later */
    _timer->CTRLA = (TCB_CLKSEL_CLKTCA_gc) | (TCB_ENABLE_bm);
#endif
}


#if defined USE_TIMERB0
ISR(TCB0_INT_vect)
#elif defined USE_TIMERB1
ISR(TCB1_INT_vect)
#endif
{
  if(!(--timer_cycle_per_tgl_count)) //Are we ready to toggle? pre-decrement, then see if 0 or more
  {
    timer_cycle_per_tgl_count=timer_cycle_per_tgl;
    // toggle the pin
    *timer_outtgl_reg = timer_bit_mask; //toggle the pin

    if (timer_toggle_count > 0){  //if duration was specified, decrement toggle count.
        timer_toggle_count--;
    } else if (timer_toggle_count == 0) {    // If toggle count = 0, stop
      *(timer_outtgl_reg-1) = timer_bit_mask; // this gives us the corresponding OUTCLR reg
      disableTimer();
    } //otherwise timer_toggle_count wasn't supplied, go on until noTone() called
  }
  /* Clear flag */
  _timer->INTFLAGS = TCB_CAPT_bm;
}
