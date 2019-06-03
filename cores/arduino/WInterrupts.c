/* -*- mode: jde; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
  Part of the Wiring project - http://wiring.uniandes.edu.co

  Copyright (c) 2004-05 Hernando Barragan

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
  
  Modified 24 November 2006 by David A. Mellis
  Modified 1 August 2010 by Mark Sproul
*/

#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <stdio.h>

#include "wiring_private.h"

static volatile voidFuncPtr intFunc[EXTERNAL_NUM_INTERRUPTS];

void attachInterrupt(uint8_t pin, void (*userFunc)(void), PinStatus mode) {

  /* Get bit position and check pin validity */
  uint8_t bit_pos = digitalPinToBitPosition(pin);
  if(bit_pos == NOT_A_PIN) return;

  /* Get interrupt number from pin */
  uint8_t interruptNum = (digitalPinToPort(pin) * 8) + bit_pos;

  /* Check interrupt number and apply function pointer to correct array index */
  if(interruptNum < EXTERNAL_NUM_INTERRUPTS) {
    intFunc[interruptNum] = userFunc;

    // Configure the interrupt mode (trigger on low input, any change, rising
    // edge, or falling edge).  The mode constants were chosen to correspond
    // to the configuration bits in the hardware register, so we simply apply
    // the setting in the pin control register

    switch (mode) {
      case CHANGE:
        mode = PORT_ISC_BOTHEDGES_gc;
        break;
      case FALLING:
        mode = PORT_ISC_FALLING_gc;
        break;
      case RISING:
        mode = PORT_ISC_RISING_gc;
        break;
      case LOW:
        mode = PORT_ISC_LEVEL_gc;
        break;
      default:
        // AVR doesn't support level triggered interrupts
        return;
    }

    // Enable the interrupt.

    /* Get pointer to correct pin control register */
    PORT_t *port = digitalPinToPortStruct(pin);
    volatile uint8_t* pin_ctrl_reg = getPINnCTRLregister(port, bit_pos);

    /* Clear any previous setting */
    *pin_ctrl_reg &= ~(PORT_ISC_gm);

    /* Apply ISC setting */
    *pin_ctrl_reg |= mode;
  }
}

void detachInterrupt(uint8_t pin) {
  /* Get bit position and check pin validity */
  uint8_t bit_pos = digitalPinToBitPosition(pin);
  if(bit_pos == NOT_A_PIN) return;

  /* Get interrupt number from pin */
  uint8_t interruptNum = (digitalPinToPort(pin) * 8) + bit_pos;

  if(interruptNum < EXTERNAL_NUM_INTERRUPTS) {
    // Disable the interrupt.

    /* Get pointer to correct pin control register */
    PORT_t *port = digitalPinToPortStruct(pin);
    volatile uint8_t* pin_ctrl_reg = getPINnCTRLregister(port, bit_pos);

    /* Clear ISC setting */
    *pin_ctrl_reg &= ~(PORT_ISC_gm);

    intFunc[interruptNum] = 0;
  }
}

static void port_interrupt_handler(uint8_t port) {

  PORT_t *portStruct = portToPortStruct(port);
  /* Copy flags */
  uint8_t int_flags = portStruct->INTFLAGS;

  uint8_t bit_pos = PIN0_bp, bit_mask = PIN0_bm;

  /* Iterate through flags */
  while(bit_pos <= PIN7_bp){

    /* Check if flag raised */
    if(int_flags & bit_mask){

    /* Get interrupt */
    uint8_t interrupt_num = port*8 + bit_pos;

      /* Check if function defined */
      if(intFunc[interrupt_num] != 0){

        /* Call function */
        intFunc[interrupt_num]();
      }
    }
    bit_pos++;
    bit_mask = (bit_mask << 1); 
  }

  /* Clear flags that have been handled */
  portStruct->INTFLAGS = int_flags;
}

#define IMPLEMENT_ISR(vect, port) \
ISR(vect) { \
  port_interrupt_handler(port);\
} \

IMPLEMENT_ISR(PORTA_PORT_vect, PA)
IMPLEMENT_ISR(PORTB_PORT_vect, PB)
IMPLEMENT_ISR(PORTC_PORT_vect, PC)
IMPLEMENT_ISR(PORTD_PORT_vect, PD)
IMPLEMENT_ISR(PORTE_PORT_vect, PE)
IMPLEMENT_ISR(PORTF_PORT_vect, PF)
