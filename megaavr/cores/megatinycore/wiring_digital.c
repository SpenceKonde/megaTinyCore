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

inline __attribute__((always_inline)) void check_valid_digital_pin(pin_size_t pin) {
  if(__builtin_constant_p(pin))
    if (pin >= NUM_TOTAL_PINS && pin != NOT_A_PIN)
    // Exception made for NOT_A_PIN - code exists which relies on being able to pass this and have nothing happen.
    // While IMO very poor coding practice, these checks aren't here to prevent lazy programmers from intentionally
    // taking shortcuts we disapprove of, but to call out things that are virtually guaranteed to be a bug.
    // Passing -1/255/NOT_A_PIN to the digital I/O functions is most likely intentional.
      badArg("Digital pin is constant, but not a valid pin");
}

inline __attribute__((always_inline)) void check_valid_pin_mode(uint8_t mode) {
  if(__builtin_constant_p(mode)) {
    //if (mode == OUTPUT_PULLUP)
      //badArg("OUTPUT_PULLUP is not supported through pinMode due to overhead and impracticality vis-a-vis other pinMode features; use pinConfigure() or pinModeFast()");
    if (mode != INPUT && mode != OUTPUT && mode != INPUT_PULLUP) {
      badArg("The mode passed to pinMode must be INPUT, OUTPUT, or INPUT_PULLUP (these have numeric values of 0, 1, or 2); it was given a constant that was not one of these.");
    }
  }
}

void pinConfigure(uint8_t pin, uint16_t pinconfig) {
  check_valid_digital_pin(pin);
  uint8_t bit_mask = digitalPinToBitMask(pin);
  if (bit_mask == NOT_A_PIN) {
    return;                             /* ignore invalid pins passed at runtime */
  }
  volatile uint8_t *portbase = (volatile uint8_t*) digitalPinToPortStruct(pin);
  uint8_t bit_pos = digitalPinToBitPosition(pin);
  uint8_t setting = pinconfig & 0x03; //grab direction bits
  if (setting) {
    *(portbase + setting) = bit_mask;
  }
  pinconfig >>= 2;
  setting = pinconfig & 0x03; // as above, only for output
  if (setting) {
    *(portbase + 4 + setting) = bit_mask;
  }
  if (!(pinconfig & 0x03FFC)) return;
  pinconfig >>= 2;
  uint8_t oldSREG = SREG;
  cli();
  uint8_t pinncfg = *(portbase + 0x10 + bit_pos);
  if (pinconfig & 0x08 ) {
    pinncfg = (pinncfg & 0xF8 ) | (pinconfig & 0x07);
  }
  uint8_t temp = pinconfig & 0x30;
  if (temp) {
    if (temp == 0x30) {
      pinncfg ^= 0x08;    // toggle pullup - of dubious utility
    } else if (temp == 0x20) {
      pinncfg &= ~(0x08); // clear
    } else {
      pinncfg |= 0x08;    // set
    }
  }
  pinconfig >>= 8; // now it's just the last 4 bits.
  #ifdef MVIO
  /* only MVIO parts have this option
   *
   * Their utility in a mixed voltage system is obvious: when you run out
   * of MV pins, you can use as many GPIO pins as you want as open drain outputs, but what if
   * you need inputs? Assuming VDD > VDDIO2, any GPIO pin could be connected to that source
   * but normally you can't read them reliably. The schmitt trigger is only guaranteed to read
   * HIGH above 0.8*Vcc, so even 3.3v logic (which is widely known to be pretty reliable)
   * isn't in spec for a part running Vdd = 5.0 V!
   *
   * The solution is to set INLVL to TTL mode, which guarantees a LOW when Vin < 0.8 V and
   * a HIGH when Vin > 1.6 V.
   *
   * Whenever you're interacting with something that might use lower logic level, enable this.
   * Set or clear only - toggle not supported. I question the usefulness of the other PINnCTRL
   * toggles, but here I have very little doubt: If you want a toggle INLVL option, you're in an
   * X-Y problem and are going about something sufficiently wrong  that it is more helpful to not
   * give you that tool than help you go further into the weeds. */
  temp = pinconfig & 0x03;
  if (temp) {
    if (temp == 0x01) {
      pinncfg |= 0x40; // set
    } else {
      pinncfg &= ~(0x40);   // clear
    }
  }
  #endif
  temp = pinconfig & 0x0C;
  if (temp) {
    if (temp == 0x0C) {
      pinncfg ^= 0x80;    // toggle invert - of dubious utility, but I'll accept it.
    } else if (temp == 0x08) {
      pinncfg &= ~(0x80); // clear
    } else {
      pinncfg |= 0x80;    // set
    }
  }
  *(portbase + 0x10 + bit_pos)=pinncfg;
  SREG=oldSREG; //re-enable interrupts
}

static inline uint8_t portToPortBaseOffset(uint8_t port);
static inline uint8_t portToPortBaseOffset(uint8_t port) {
  _SWAP(port);
  return port << 1;
}
void pinMode(uint8_t pin, uint8_t mode) {
  check_valid_digital_pin(pin);         /* generate compile error if a constant that is not a valid pin is used as the pin */
  check_valid_pin_mode(mode);           /* generate compile error if a constant that is not a valid pin mode is used as the mode */
  uint8_t bit_mask = digitalPinToBitMask(pin);
  if ((bit_mask == NOT_A_PIN) || (mode > 3)) {
    return;                             /* ignore invalid pins passed at runtime */
  }
  volatile uint8_t * port_base = ((volatile uint8_t *) (uint16_t)(0x0400 | portToPortBaseOffset(digitalPinToPort(pin))));
  if (mode & 0x01) {
    // OUTPUT mode, so write DIRSET with the mask.
    *(port_base + 1) = bit_mask;
  } else {
    *(port_base + 2) = bit_mask;
    if (mode == 2) {
      *(port_base + 5) = bit_mask;
    } else if (mode == 0) {
      *(port_base + 6) = bit_mask;
    }
  }
  port_base +=(uint8_t) digitalPinToBitPosition(pin) | (uint8_t) 0x10;
  bit_mask = *port_base;
  if (mode & 2) {
    bit_mask |= 0x08;
  } else {
    bit_mask &= 0xF7;
  }
  *port_base = bit_mask;
}



void turnOffPWM(uint8_t pin) {
  /* Actually turn off compare channel, not the timer */

  /* Get pin's timer
   * megaTinyCore only - assumes only TIMERA0, TIMERD0, or DACOUT
   * can be returned here, all have only 1 bit set, so we can use
   * PeripheralControl as a mask to see if they have taken over
   * any timers with minimum overhead - critical on these parts
   * Since nothing that will show up here can have more than one
   * one bit set, binary and will give 0x00 if that bit is cleared
   * which is NOT_ON_TIMER.
   */
  uint8_t digital_pin_timer =  digitalPinToTimer(pin) & PeripheralControl;
  /* end megaTinyCore-specific section */
  if (digital_pin_timer== NOT_ON_TIMER) {
    return;
  }

  uint8_t bit_mask = digitalPinToBitMask(pin);
  //TCB_t *timerB;

  switch (digital_pin_timer) {

    /* TCA0 */
    case TIMERA0:
    {
      // uint8_t *timer_cmp_out;
      /* Bit position will give output channel */
      #ifdef __AVR_ATtinyxy2__
        if (bit_mask == 0x80) {
          bit_mask = 1;  //on the xy2, WO0 is on PA7
        }
      #endif
      if (bit_mask > 0x04) { // -> bit_pos > 2 -> output channel controlled by HCMP
        bit_mask <<= 1;      // mind the gap (between LCMP and HCMP)
      }
      // since we're turning it off, we don't need to change the CMP register
      TCA0.SPLIT.CTRLB &= ~bit_mask;
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
        uint8_t fc_mask = (bit_mask==0x02?0x80:0x40);
        if (TCD0.FAULTCTRL & fc_mask) {
          uint8_t oldSREG=SREG;
          cli();
          //uint8_t TCD0_prescaler=TCD0.CTRLA&(~TCD_ENABLE_bm);
          //
          TCD0.CTRLA &= ~TCD_ENABLE_bm;
          _PROTECTED_WRITE(TCD0.FAULTCTRL, TCD0.FAULTCTRL & (~fc_mask));
          while (!(TCD0.STATUS & TCD_ENRDY_bm)); // wait until it can be re-enabled
          TCD0.CTRLA |= TCD_ENABLE_bm;           // re-enable it
          // Assuming this mode is enabled, PWM can leave the pin with INVERTED mode enabled
          // So we need to make sure that's off - wouldn't that be fun to debug?
          #if defined(NO_GLITCH_TIMERD0) /* This is enabled in all cases where TCD0 is used for PWM */
            // We only support control of the TCD0 PWM functionality on PIN_PC0 and PIN_PC1 (on 20 and 24 pin parts )
            // so if we're here, we're acting on either PC0 or PC1.
            if (bit_mask == 0x01){
              PORTC.PIN0CTRL&=~(PORT_INVEN_bm);
            } else {
              PORTC.PIN1CTRL&=~(PORT_INVEN_bm);
            }
          #endif

          SREG=oldSREG;
        }
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


  /*
  Set output to value
  This now runs even if port set INPUT in order to emulate
  the behavior of digitalWrite() on classic AVR devices, where
  you could digitalWrite() a pin while it's an input, to ensure
  that the value of the port was set correctly when it was
  changed to an output. Code in the wild relies on this behavior.
  */

  if (val == LOW) { /* If LOW */
    port->OUTCLR = bit_mask;
  } else if (val == CHANGE) { /* If TOGGLE */
    port->OUTTGL = bit_mask;
    // Now, for the pullup setting part below
    // we need to know if it's been set high or low
    // otherwise the pullup state could get out of
    // sync with the output bit. Annoying!
    val=port->OUT & bit_mask;
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
  if (pin==NOT_A_PIN) return; // sigh... I wish I didn't have to catch this... but it's all compile time known so w/e
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
    vport->IN  |= mask;
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
  // Origionbally the Arduino core this was derived from turned off PWM on the pin
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
  port->OUTCLR=bit_mask;
  if (state == LOW)
    port->DIRSET=bit_mask;
  else if (state == CHANGE)
    port->DIRTGL=bit_mask;
  else // assume FLOAT
    port->DIRCLR=bit_mask;
  turnOffPWM(pin);
}

inline __attribute__((always_inline)) void openDrainFast(uint8_t pin, uint8_t val) {
  check_constant_pin(pin);
  check_valid_digital_pin(pin);
  if (pin==NOT_A_PIN) return; // sigh... I wish I didn't have to catch this... but it's all compile time known so w/e
  // Mega-0, Tiny-1 style IOPORTs
  // Assumes VPORTs exist starting at 0 for each PORT structure
  uint8_t mask = 1 << digital_pin_to_bit_position[pin];
  uint8_t port = digital_pin_to_port[pin];
  VPORT_t *vport;
  vport = (VPORT_t *)(port * 4);
  PORT_t *portstr;
  portstr=(PORT_t *)(0x400+(0x20*port));

  if (val == LOW)
    vport->DIR |= mask;
  else if (val==CHANGE)
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
  check_valid_digital_pin(pin);         //generate compile error if a constant that is not a valid pin is used as the pin
  uint8_t mask = 1 << digital_pin_to_bit_position[pin];
  uint8_t port = digital_pin_to_port[pin];
  VPORT_t *vport;
  vport = (VPORT_t *)(port * 4);
  volatile uint8_t *pin_ctrl = (volatile uint8_t *) (0x410 + digital_pin_to_port[pin] * 0x20 + digital_pin_to_bit_position[pin]);
  if (mode==OUTPUT)// || mode==OUTPUT_PULLUP)
    vport->DIR |= mask;
  else
    vport->DIR &= ~mask;
    // 1 clock to set direction
  if (mode == INPUT_PULLUP)
    *pin_ctrl |= PORT_PULLUPEN_bm;
  else if (mode == INPUT)
    *pin_ctrl &= ~PORT_PULLUPEN_bm;
    // and 5 to switch the damned pullup.
}
