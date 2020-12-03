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
    if (pin >= NUM_TOTAL_PINS) badArg("Digital pin is constant, but not a valid pin");
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

    } else { /* mode == INPUT (no pullup) */

      /* Disable pull-up */
      *pin_ctrl_reg &= ~(PORT_PULLUPEN_bm);
    }

    /* Restore state */
    SREG = status;
  }
}

// Forcing this inline keeps the callers from having to push their own stuff
// on the stack. It is a good performance win and only takes 1 more byte per
// user than calling. (It will take more bytes on the 168.)
//
// But shouldn't this be moved into pinMode? Seems silly to check and do on
// each digitalread or write.
//
// Mark Sproul:
// - Removed inline. Save 170 bytes on atmega1280
// - changed to a switch statement; added 32 bytes but much easier to read and maintain.
// - Added more #ifdefs, now compiles for atmega645
//
//static inline void turnOffPWM(uint8_t timer) __attribute__ ((always_inline));
//static inline void turnOffPWM(uint8_t timer)
static void turnOffPWM(uint8_t pin) {
  /* Actually turn off compare channel, not the timer */

  /* Get pin's timer */
  uint8_t timer = digitalPinToTimer(pin);
  if (timer == NOT_ON_TIMER) {
    return;
  }

  uint8_t bit_pos = digitalPinToBitPosition(pin);
  //TCB_t *timerB;

  switch (timer) {

    /* TCA0 */
    case TIMERA0:
    {
      /* Bit position will give output channel */
      #ifdef __AVR_ATtinyxy2__
        if (bit_pos == 7) {
          bit_pos = 0;  //on the xy2, WO0 is on PA7
        }
      #endif
      if (bit_pos > 2) {
        bit_pos++;  //there's a blank bit in the middle
      }
      /* Disable corresponding channel */
      TCA0.SPLIT.CTRLB &= ~(1 << (TCA_SPLIT_LCMP0EN_bp + bit_pos));
      break;
    }

      /* we don't need the type b timers as this core does not use them for PWM      */

    // 1-series parts have a DAC that we can use...
    #if defined(DAC0)
      case DACOUT:
        DAC0.CTRLA = 0x00;
        break;
    #endif

    // 1-series parts also have a wacky async Type D timer, but we only use it on the 20 and 24-pin parts, as it doesn't buy us anything on the 14-pin ones...
    // In a future update, an option to use TCD0 for PWM on PA4 and PA5 on the 14-pin parts, with TCA0 initialized in SINGLE mode, but this will only be done
    // if there is user demand; I suspect there is not!
    #if (defined(TCD0) && defined(USE_TIMERD0_PWM))
      case TIMERD0:
      {
        // rigmarole that produces a glitch in the PWM
        uint8_t oldSREG=SREG;
        cli();
        uint8_t TCD0_prescaler=TCD0.CTRLA&(~TCD_ENABLE_bm);
        TCD0.CTRLA = TCD0_prescaler; //stop the timer
        while (!(TCD0.STATUS & TCD_ENRDY_bm)); // wait until it's actually stopped
        _PROTECTED_WRITE(TCD0.FAULTCTRL, TCD0.FAULTCTRL | (1 << (6 + bit_pos)));
        TCD0.CTRLA = (TCD0_prescaler | TCD_ENABLE_bm); //re-enable it

        // Assuming this mode is enabled, PWM can leave the pin with INVERTED mode enabled
        // So we need to make sure that's off - wouldn't that be fun to debug?
        #if defined(NO_GLITCH_TIMERD0)
          // We only support control of the TCD0 PWM functionality on PIN_PC0 and PIN_PC1 (on 20 and 24 pin parts )
          // so if we're here, we're acting on either PC0 or PC1.
          if (bit_pos==0){
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

  /* Output direction */
  if (port->DIR & bit_mask) {

    /* Set output to value */
    if (val == LOW) { /* If LOW */
      port->OUTCLR = bit_mask;

    } else if (val == CHANGE) { /* If TOGGLE */
      port->OUTTGL = bit_mask;
      /* If HIGH OR  > TOGGLE  */
    } else {
      port->OUTSET = bit_mask;
    }

    /* Input direction */
  } else {
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
