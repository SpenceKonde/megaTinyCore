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
*/

#define ARDUINO_MAIN
#include "wiring_private.h"
#include "pins_arduino.h"


inline __attribute__((always_inline)) void check_valid_digital_pin(pin_size_t pin)
{
  if(__builtin_constant_p(pin))
    if (pin >= NUM_TOTAL_PINS && pin != NOT_A_PIN)
    // Exception made for NOT_A_PIN - code exists which relies on being able to pass this and have nothing happen.
    // While IMO very poor coding practice, these checks aren't here to prevent lazy programmers from intentionally
    // taking shortcuts we disapprove of, but to call out things that are virtually guaranteed to be a bug.
    // Passing -1/255/NOT_A_PIN to the digital I/O functions is most likely intentional.
      badArg("Digital pin is constant, but not a valid pin");
}

void pinMode(uint8_t pin, uint8_t mode) {
  check_valid_digital_pin(pin);

  uint8_t bit_mask = digitalPinToBitMask(pin);

  if ((bit_mask == NOT_A_PIN) || (mode > INPUT_PULLUP)) {
    return;
  }

  PORT_t *port = digitalPinToPortStruct(pin);
  if (port == NULL) {
    return;
  }

  if (mode == OUTPUT) {

    /* Configure direction as output */
    port->DIRSET = bit_mask;

  } else { /* mode == INPUT or INPUT_PULLUP */

    uint8_t bit_pos = digitalPinToBitPosition(pin);
    /* Calculate where pin control register is */
    volatile uint8_t *pin_ctrl_reg = getPINnCTRLregister(port, bit_pos);

    /* Save state */
    uint8_t status = SREG;
    cli();

    /* Configure direction as input */
    port->DIRCLR = bit_mask;

    /* Configure pull-up resistor */
    if (mode == INPUT_PULLUP) {

      /* Enable pull-up */
      *pin_ctrl_reg |= PORT_PULLUPEN_bm;
      /* Considered this, it was in some 2.2.0 releases. This is debatable - it actually broke Wire...
       * though only because of errata effecting Wire that wasn't explicitly accounted for.
       */
      // emulate setting of the port output register on classic AVR
      port->OUTSET=bit_mask;

    } else { /* mode == INPUT (no pullup) */

      /* Disable pull-up */
      *pin_ctrl_reg &= ~(PORT_PULLUPEN_bm);
      port->OUTCLR=bit_mask;

    }

    /* Restore state */
    SREG = status;
  }
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
      //uint8_t offset=0;
      if (bit_mask > 0x40) { // HCMP
        bit_mask <<= 1;      // mind the gap
      //  offset = 1;          // used to find cmp
      }
      //Say, why were we bothering with all this? We're turning it off!
      //if      (bit_mask & 0x44) offset += 4;
      //else if (bit_mask & 0x22) offset += 2;
      //timer_cmp_out = ((uint8_t *)(&TCA0.SPLIT.LCMP0)) + (offset);
      //(*timer_cmp_out) = 0;
      TCA0.SPLIT.CTRLB &= ~bit_mask;
      break;
    }

    /* we don't need the type b timers as this core does not use them for PWM      */

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
     * it would be able to buffer. Should do an example of this!
     */
    #if (defined(TCD0) && defined(USE_TIMERD0_PWM))
      case TIMERD0:
      {
        // rigmarole that produces a glitch in the PWM
        uint8_t oldSREG=SREG;
        cli();
        //uint8_t TCD0_prescaler=TCD0.CTRLA&(~TCD_ENABLE_bm);
        TCD0.CTRLA &= ~TCD_ENABLE_bm;
        _PROTECTED_WRITE(TCD0.FAULTCTRL, TCD0.FAULTCTRL & (~(bit_mask==0x02?0x80:0x40)));
        while (!(TCD0.STATUS & TCD_ENRDY_bm)); // wait until it can be re-enabled
        TCD0.CTRLA |= TCD_ENABLE_bm;           // re-enable it

        // Assuming this mode is enabled, PWM can leave the pin with INVERTED mode enabled
        // So we need to make sure that's off - wouldn't that be fun to debug?
        #if defined(NO_GLITCH_TIMERD0)
          // We only support control of the TCD0 PWM functionality on PIN_PC0 and PIN_PC1 (on 20 and 24 pin parts )
          // so if we're here, we're acting on either PC0 or PC1.
          if (bit_mask == 0x01){
            PORTC.PIN0CTRL&=~(PORT_INVEN_bm);
          } else {
            PORTC.PIN1CTRL&=~(PORT_INVEN_bm);
          }
        #endif

        SREG=oldSREG;
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
  /* Turn off PWM if applicable */
  // If the pin supports PWM output, we need to turn it off
  // Better to do so AFTER we have set PORTx.OUT to what we want it to be when we're done
  // The glitch would be super short, of course, but why make a glitch we don't have to?
  turnOffPWM(pin);
}

inline __attribute__((always_inline)) void digitalWriteFast(uint8_t pin, uint8_t val)
{
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


inline __attribute__((always_inline)) int8_t digitalReadFast(uint8_t pin)
{
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


void openDrain(uint8_t pin, uint8_t state){
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

inline __attribute__((always_inline)) void openDrainFast(uint8_t pin, uint8_t val)
{
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
  portstr=(PORT_t *)(0x400+(20*port));

  if (val == LOW)
    vport->DIR |= mask;
  else if (val==CHANGE)
    portstr->DIRTGL = mask;
  else// FLOAT
    vport->DIR &= ~mask;
}
