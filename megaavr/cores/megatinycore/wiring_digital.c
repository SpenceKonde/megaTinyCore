/*
  wiring_digital.c - digital input and output functions
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

  Modified 28 September 2010 by Mark Sproul
  Modified extensively 2018~2021 by Spence Konde for
  megaTinyCore and DxCore.
*/

#define ARDUINO_MAIN
#include "wiring_private.h"
#include "pins_arduino.h"



inline __attribute__((always_inline)) void check_valid_pin_mode(uint8_t mode) {
  if (__builtin_constant_p(mode)) {
    if (mode != INPUT && mode != OUTPUT && mode != INPUT_PULLUP) {
      badArg("The mode passed to pinMode must be INPUT, OUTPUT, or INPUT_PULLUP (these have numeric values of 0, 1, or 2); it was given a constant that was not one of these.");
    }
  }
}

void pinMode(uint8_t pin, uint8_t mode) {
  check_valid_digital_pin(pin);         /* generate compile error if a constant that is not a valid pin is used as the pin */
  check_valid_pin_mode(mode);           /* generate compile error if a constant that is not a valid pin mode is used as the mode */
  uint8_t bit_mask = digitalPinToBitMask(pin);
  if ((bit_mask == NOT_A_PIN) || (mode > INPUT_PULLUP)) {
    return;                             /* ignore invalid pins passed at runtime */
  }
  volatile uint8_t * port_base = (volatile uint8_t *) (uint16_t) ((&PORTA) + digitalPinToPort(pin));
  if (mode & 0x01) {
    // OUTPUT mode, so write DIRSET with the mask.
    *(port_base + 1) = bit_mask;
  } else { // otherwise it's input so we write DIRCLR
    *(port_base + 2) = bit_mask;
    /* By unanimous decision of users who spoke up, we shall not set the output register to emulate classic AVRs.
    if (mode == INPUT_PULLUP) {
      *(port_base + 5) = bit_mask;
    } else if (mode == 0) {
      *(port_base + 6) = bit_mask;
    }
    */
  }
  // Now an interesting question:
  // Is this faster?
  //
  //    port_base +=(uint8_t) digitalPinToBitPosition(pin)
  //     bit_mask = *port_base+0x10
  //
  // It ought to be slightly faster. After we have loaded the bit position, either we combine with 0x10 (1 clock)
  // add to port base (2 clocks, even though this will never result in a carry for any valid port that exist)
  // then ld (2 clocks).
  // Or after loading the bit position, we can add it to port_base (2 clocks)
  // then load with displacement 2 clocks. IFF the compiler puts this pointer into Y or Z, it is faster and smaller
  // by 2 bytes and one clock. And let's hope it is, because the code above would suck too if it was in X.
  // Handwritten assembly that exploited the knowledge that there will never be a carry would save 1 word and 1 clock.
  // and could probably save at least several times that in initializing the port_base pointer. But if you're using
  // pinMode you probably don't care.
  port_base += (uint8_t) digitalPinToBitPosition(pin);
  bit_mask = *(port_base + 0x10);
  if (mode & 2) {
    bit_mask |= 0x08;
  } else {
    bit_mask &= 0xF7;
  }
  *(port_base + 0x10) = bit_mask;
}

/* This turns off PWM, if enabled. It is called automatically on every digitalWrite();
 * This function can end up executing a heckovalotta code for one simple
 * Note that it only operates on the PWM source with priority - TCA > TCD > TCB/DAC
 * the order of the cases here doesn't matter - which one has priority is determined in
 * digitalPinToTimerNow() in wiring_analog.c. That's why it's recommended to make sure
 * that no pin you're about to move the PWM output of a TCA onto is currently outputting
 * PWM. It can also be used from user code (unlike on the stock core). */
void turnOffPWM(uint8_t pin)
{
  /* Actually turn off compare channel, not the timer */

  /* Get pin's timer
   * megaTinyCore only - assumes only TIMERA0, TIMERD0, or DACOUT
   * can be returned here, all have only 1 bit set, so we can use
   * __PeripheralControl as a mask to see if they have taken over
   * any timers with minimum overhead - critical on these parts
   * Since nothing that will show up here can have more than one
   * one bit set, binary and will give 0x00 if that bit is cleared
   * which is NOT_ON_TIMER.
   */
  uint8_t digital_pin_timer =  digitalPinToTimer(pin) & __PeripheralControl;
  /* end megaTinyCore-specific section */
  if (digital_pin_timer== NOT_ON_TIMER) {
    return;
  }

  uint8_t bit_mask = digitalPinToBitMask(pin);
  // TCB_t *timerB;

  switch (digital_pin_timer) {

    /* TCA0 */
    case TIMERA0:
    {
      uint8_t port=digitalPinToPort(pin);
      #if !defined(TCA_BUFFERED_3PIN)
        // uint8_t *timer_cmp_out;
        /* Bit position will give output channel */
        #ifdef __AVR_ATtinyxy2__
          if (bit_mask == 0x80) {
            bit_mask = 1;         // on the xy2, WO0 is on PA7
          }
          if (bit_mask > 0x04) {  // -> bit_pos > 2 -> output channel controlled by HCMP
            bit_mask <<= 1;       // mind the gap (between LCMP and HCMP)
          }
        #else
          if (port == PB) {        // WO0-WO2, Bitmask has one of these bits 1: 0b00hhhlll.
            if (bit_mask > 0x04) { // Is it one of the three high ones? If so
              bit_mask <<= 1;      // nudge it 1 place left swap nybbles since that's 1 clock faster than 3 rightshifts.
              _SWAP(bit_mask);     // swap nybbles since that's 1 clock faster than 3 rightshifts.
            }
          } else {
            // Otherwise, it's WO3-5. These will always be on 0b00hhh000,. Here since we ARE working with a high half timer, we need to just leftshift it once.
            bit_mask <<= 1;
          }
        #endif
        TCA0.SPLIT.CTRLB &= ~bit_mask;
      #else // 3-pin mode. Means we know it's on PORTB}
        if (bit_mask > 0x04) { // Is it one of the three high ones? If so
          bit_mask <<= 1;      // nudge it 1 place left swap nybbles since that's 1 clock faster than 3 rightshifts.
          _SWAP(bit_mask);     // swap nybbles since that's 1 clock faster than 3 rightshifts.
        }
      #endif
      break;
    }

    /* We don't need the type b timers as this core does not use them for PWM      */

    // 1-series parts have a DAC that we can use...
    #if defined(DAC0)
      case DACOUT:
        DAC0.CTRLA = 0x00;
        break;
    #endif
    /* 1-series parts also have a wacky async Type D timer, but we only use it on the 20 and 24-pin parts, as it doesn't buy us anything on the 14-pin ones...
     * In a future update, an option to use TCD0 for PWM on PA4 and PA5 on the 14-pin parts, with TCA0 initialized in SINGLE mode is possible, but this would
     * open a can of worms regarding SINGLE mode. I think we are best off telling people to call takeOverTCA0(), takeOverTCD0() and configure it themselves
     * to do what they want, because you'll never make everyone happy otherwise. My calculus would be different if they'd made 8-pin parts with more than 4k
     * of flash to fit the overhead of supporting TCA0 and TCD0 with analogWrite() comfortably - that would give a pin mapping with PWM on all pins - AND
     * it would be able to do buffering on  the TCA pins (TCD pins can always do that).*/
    #if (defined(TCD0) && defined(USE_TIMERD0_PWM))
      case TIMERD0:
      {
        // rigmarole that produces a glitch in the PWM
        #if defined(TCD_USE_WOAB)
          uint8_t fc_mask = bit_mask;// Hey that works out well!
          if (TCD0.FAULTCTRL & fc_mask) {
            uint8_t oldSREG = SREG;
            cli();
            // uint8_t TCD0_prescaler=TCD0.CTRLA&(~TCD_ENABLE_bm);
            //
            TCD0.CTRLA &= ~TCD_ENABLE_bm;
            _PROTECTED_WRITE(TCD0.FAULTCTRL, TCD0.FAULTCTRL & (~fc_mask));
            while (!(TCD0.STATUS & TCD_ENRDY_bm)); // wait until it can be re-enabled
            TCD0.CTRLA |= TCD_ENABLE_bm;           // re-enable it
            #if defined(NO_GLITCH_TIMERD0) /* This is enabled in all cases where TCD0 is used for PWM */
              // Assuming this mode is enabled, PWM can leave the pin with INVERTED mode enabled
              // So we need to make sure that's off - wouldn't that be fun to debug?
              if (bit_mask == 0x10) {
                PORTA.PIN4CTRL &= ~(PORT_INVEN_bm);
              } else {
                PORTA.PIN5CTRL &= ~(PORT_INVEN_bm);
              }
            #endif

            SREG = oldSREG;
          }
        #else
          uint8_t fc_mask = (bit_mask == 0x02 ? 0x80 : 0x40);
          if (TCD0.FAULTCTRL & fc_mask) {
            uint8_t oldSREG = SREG;
            cli();
            // uint8_t TCD0_prescaler=TCD0.CTRLA&(~TCD_ENABLE_bm);
            //
            TCD0.CTRLA &= ~TCD_ENABLE_bm;
            _PROTECTED_WRITE(TCD0.FAULTCTRL, TCD0.FAULTCTRL & (~fc_mask));
            while (!(TCD0.STATUS & TCD_ENRDY_bm)); // wait until it can be re-enabled
            TCD0.CTRLA |= TCD_ENABLE_bm;           // re-enable it
            #if defined(NO_GLITCH_TIMERD0) /* This is enabled in all cases where TCD0 is used for PWM */
              // Assuming this mode is enabled, PWM can leave the pin with INVERTED mode enabled
              // So we need to make sure that's off - wouldn't that be fun to debug?
              if (bit_mask == 0x01) {
                PORTC.PIN0CTRL &= ~(PORT_INVEN_bm);
              } else {
                PORTC.PIN1CTRL &= ~(PORT_INVEN_bm);
              }
            #endif

            SREG = oldSREG;
          }
        #endif
        break;
      }
    #endif
    // END TCD0 handling

    default:
      break;
  }
}

void digitalWrite(uint8_t pin, uint8_t val) {
  check_valid_digital_pin(pin);
  /* Get bit mask for pin */
  uint8_t bit_mask = digitalPinToBitMask(pin);
  if (bit_mask == NOT_A_PIN) {
    return;
  }

  /* Get port */
  PORT_t *port = digitalPinToPortStruct(pin);
  /* Set output to value
  This now runs even if port set INPUT in order to emulate
  the behavior of digitalWrite() on classic AVR devices, where
  you could digitalWrite() a pin while it's an input, to ensure
  that the value of the port was set correctly when it was
  changed to an output. Code in the wild relies on this behavior. */

  if (val == LOW) { /* If LOW */
    port->OUTCLR = bit_mask;
  } else if (val == CHANGE) { /* If TOGGLE
     * For the pullup setting part below
     * we need to know if it's been set high or low
     * otherwise the pullup state could get out of
     * sync with the output bit. Annoying! But we should
     * have to read it before writing OUTTGL, since that can
     * have a 1 clock delay. So read first + invert */
    val = !(port->OUT & bit_mask);
    port->OUTTGL = bit_mask;
    // Now, for the pullup setting part below
    // we need to know if it's been set high or low
    // otherwise the pullup state could get out of
    // sync with the output bit. Annoying!
    val = port->OUT & bit_mask;
    // val will now be 0 (LOW) if the toggling made it LOW
    // or bit_mask if not. And further down, we only need to
    // know if it's
  /* If HIGH OR  > TOGGLE  */
  } else {
    port->OUTSET = bit_mask;
  }

  /* Input direction */
  if (!(port->DIR & bit_mask)) {
    /* Old implementation has side effect when pin set as input -
      pull up is enabled if this function is called.
      Should we purposely implement this side effect?
    */

    /* Get bit position for getting pin ctrl reg */
    uint8_t bit_pos = digitalPinToBitPosition(pin);

    /* Calculate where pin control register is */
    volatile uint8_t *pin_ctrl_reg = getPINnCTRLregister(port, bit_pos);

    /* Save system status and disable interrupts */
    uint8_t status = SREG;
    cli();

    if (val == LOW) {
      /* Disable pullup */
      *pin_ctrl_reg &= ~PORT_PULLUPEN_bm;
    } else {
      /* Enable pull-up */
      *pin_ctrl_reg |= PORT_PULLUPEN_bm;
    }

    /* Restore system status */
    SREG = status;
  }
  /* Turn off PWM if applicable
   * If the pin supports PWM output, we need to turn it off.
   * Better to do so AFTER we have set PORTx.OUT to what we
   * want it to be when we're done. The glitch would be short
   * (though non-negligible since all these functions are, of
   * course, slow - the worst case being a TCD pin currently
   * analogWritten() 255, then digitallyWritten() to HIGH, which
   * would turn it off for the time between turnOffPWM() and
   * PORT->OUTCLR)
   * Since there's no penalty, why make a glitch we don't have to? */
  turnOffPWM(pin);
}

inline __attribute__((always_inline)) void digitalWriteFast(uint8_t pin, uint8_t val) {
  check_constant_pin(pin);
  check_valid_digital_pin(pin);
  if (pin == NOT_A_PIN) return; // sigh... I wish I didn't have to catch this... but it's all compile time known so w/e
  // Mega-0, Tiny-1 style IOPORTs
  // Assumes VPORTs exist starting at 0 for each PORT structure
  uint8_t mask = 1 << digital_pin_to_bit_position[pin];
  uint8_t port = digital_pin_to_port[pin];
  VPORT_t *vport;

  // Write pin value from VPORTx.OUT register
  vport = (VPORT_t *)(port * 4);
/*
  if (val == HIGH)
    vport->OUT |= mask;
  else if (val == LOW)
    vport->OUT &= ~mask;
  else // CHANGE
    vport->IN = mask;
*/
  if (val == LOW)
    vport->OUT &= ~mask;
  else if (val == CHANGE)
    vport->IN |= mask;
  else // HIGH
    vport->OUT |= mask;

}

int8_t digitalRead(uint8_t pin) {
  check_valid_digital_pin(pin);
  /* Get bit mask and check valid pin */
  uint8_t bit_mask = digitalPinToBitMask(pin);
  if (bit_mask == NOT_A_PIN) {
    return -1;
  }
  // Originally the Arduino core this was derived from turned off PWM on the pin
  // I cannot fathom why, insofar as the Arduino team sees Arduino as an educational
  // tool, and I can't think of a better way to learn about PWM...
  //
  // More importantkly, digitialRead() already *crawls*. Hence there is a particularly
  // high bar for stuff that would make it even slower than it already is.
  //
  // turnOffPWM(pin);

  /* Get port and check valid port */
  PORT_t *port = digitalPinToPortStruct(pin);

  /* Read pin value from PORTx.IN register */
  if (port->IN & bit_mask) {
    return HIGH;
  } else {
    return LOW;
  }
}


inline __attribute__((always_inline)) int8_t digitalReadFast(uint8_t pin) {
  check_constant_pin(pin);
  check_valid_digital_pin(pin);
  // Mega-0, Tiny-1 style IOPORTs
  // Assumes VPORTs exist starting at 0 for each PORT structure
  uint8_t mask = 1 << digital_pin_to_bit_position[pin];
  uint8_t port = digital_pin_to_port[pin];
  VPORT_t *vport;

  // Old style port logic is a small integer 0 for PORTA, 1 for PORTB etc.
  vport = (VPORT_t *)(port * 4);

  // Read pin value from VPORTx.IN register
  return !!(vport->IN & mask);
}


void openDrain(uint8_t pin, uint8_t state) {
  check_valid_digital_pin(pin);
  uint8_t bit_mask = digitalPinToBitMask(pin);
  if (bit_mask == NOT_A_PIN)  return;
  /* Get port */
  PORT_t *port = digitalPinToPortStruct(pin);
  port->OUTCLR = bit_mask;
  if (state == LOW)
    port->DIRSET = bit_mask;
  else if (state == CHANGE)
    port->DIRTGL=bit_mask;
  else // assume FLOATING
    port->DIRCLR=bit_mask;
  turnOffPWM(pin);
}

inline __attribute__((always_inline)) void openDrainFast(uint8_t pin, uint8_t val)
{
  check_constant_pin(pin);
  check_valid_digital_pin(pin);
  if (pin == NOT_A_PIN) return; // sigh... I wish I didn't have to catch this... but it's all compile time known so w/e
  // Mega-0, Tiny-1 style IOPORTs
  // Assumes VPORTs exist starting at 0 for each PORT structure
  uint8_t mask = 1 << digital_pin_to_bit_position[pin];
  uint8_t port = digital_pin_to_port[pin];
  VPORT_t *vport;
  vport = (VPORT_t *)(port * 4);
  PORT_t *portstr;
  portstr = (PORT_t *)(0x400 + (0x20 * port));

  if (val == LOW)
    vport->DIR |= mask;
  else if (val == CHANGE)
    portstr->DIRTGL = mask;
  else// FLOAT
    vport->DIR &= ~mask;
}


inline __attribute__((always_inline)) void pinModeFast(uint8_t pin, uint8_t mode) {
  check_constant_pin(pin);
  if (!__builtin_constant_p(mode)) {
    badArg("mode must be constant when used with pinModeFast");
  } else {
    if (mode != INPUT && mode != OUTPUT && mode != INPUT_PULLUP) { //} && mode != OUTPUT_PULLUP) {
      badArg("The mode passed to pinModeFast must be INPUT, OUTPUT, INPUT_PULLUP");// or OUTPUT_PULLUP");
    }
  }
  check_valid_digital_pin(pin);         // generate compile error if a constant that is not a valid pin is used as the pin
  uint8_t mask = 1 << digital_pin_to_bit_position[pin];
  uint8_t port = digital_pin_to_port[pin];
  VPORT_t *vport;
  vport = (VPORT_t *)(port * 4);
  volatile uint8_t *pin_ctrl = (volatile uint8_t *) (0x410 + digital_pin_to_port[pin] * 0x20 + digital_pin_to_bit_position[pin]);
  if (mode == OUTPUT)// || mode == OUTPUT_PULLUP)
    vport->DIR |= mask;
  else
    vport->DIR &= ~mask;
    // 1 clock to set direction
  if (mode == INPUT_PULLUP)
    *pin_ctrl |= PORT_PULLUPEN_bm;
  else //SK 3/16/23 - we were doing something really dumb here. You need to turn off the pullup when set to output too!
    *pin_ctrl &= ~PORT_PULLUPEN_bm;
    // and 5 to switch the damned pullup.
}
