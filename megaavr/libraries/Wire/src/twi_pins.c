/*
  (originally found in) TwoWire.cpp - TWI/I2C library for Wiring & Arduino
  Copyright (c) 2006 Nicholas Zambetti.  All right reserved.

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

  Modified 2012 by Todd Krein (todd@krein.org) to implement repeated starts
  Modified 2017 by Chuck Todd (ctodd@cableone.net) to correct Unconfigured Slave Mode reboot
  Modified 2019-2021 by Spence Konde for megaTinyCore and DxCore.
  This version is part of megaTinyCore and DxCore; it is not expected
  to work with other hardware or cores without modifications.
  Modified 2021 by MX682X for the Wire library rewriting project
*/
// *INDENT-OFF*   astyle wants this file to be completely unreadable with no indentation for the many preprocessor conditionals!
#ifndef TWI_PINS_H
#define TWI_PINS_H

#include <Arduino.h>
#include "twi_pins.h"

bool TWI_checkPins(const uint8_t sda_pin, const uint8_t scl_pin);

bool TWI_checkPins(const uint8_t sda_pin, const uint8_t scl_pin) {
  if (__builtin_constant_p(sda_pin) && __builtin_constant_p(scl_pin)) {
    if (!(
      #if    defined(PIN_WIRE_SDA)            &&     defined(PIN_WIRE_SCL)
        ((sda_pin == PIN_WIRE_SDA)            && (scl_pin == PIN_WIRE_SCL))            ||
      #endif
      #if    defined(PIN_WIRE_SDA_PINSWAP_1)  &&     defined(PIN_WIRE_SCL_PINSWAP_1)
        ((sda_pin == PIN_WIRE_SDA_PINSWAP_1)  && (scl_pin == PIN_WIRE_SCL_PINSWAP_1))  ||
      #endif
      #if    defined(PIN_WIRE_SDA_PINSWAP_2)  &&     defined(PIN_WIRE_SCL_PINSWAP_2)
        ((sda_pin == PIN_WIRE_SDA_PINSWAP_2)  && (scl_pin == PIN_WIRE_SCL_PINSWAP_2))  ||
      #endif
      #if    defined(PIN_WIRE_SDA_PINSWAP_3)  &&     defined(PIN_WIRE_SCL_PINSWAP_3)
        ((sda_pin == PIN_WIRE_SDA_PINSWAP_3)  && (scl_pin == PIN_WIRE_SCL_PINSWAP_3))  ||
      #endif
      #if    defined(PIN_WIRE1_SDA)           &&     defined(PIN_WIRE1_SCL)
        ((sda_pin == PIN_WIRE1_SDA)           && (scl_pin == PIN_WIRE1_SCL))           ||
      #endif
      #if    defined(PIN_WIRE1_SDA_PINSWAP_1) &&     defined(PIN_WIRE1_SCL_PINSWAP_1)
        ((sda_pin == PIN_WIRE1_SDA_PINSWAP_1) && (scl_pin == PIN_WIRE1_SCL_PINSWAP_1)) ||
      #endif
      #if    defined(PIN_WIRE1_SDA_PINSWAP_2) &&     defined(PIN_WIRE1_SCL_PINSWAP_2)
        ((sda_pin == PIN_WIRE1_SDA_PINSWAP_2) && (scl_pin == PIN_WIRE1_SCL_PINSWAP_2)) ||
      #endif
        false)) {  // this false is just there to have something to OR with. Optimized away by compiler.
          #if defined(TWI1)
            badArg("Pins passed to Wire(1).pins() known at compile time to be invalid");
          #else
            badArg("Pins passed to Wire.pins() known at compile time to be invalid");
          #endif
          return false;
        } /* end of error conditionally generated when pins requested known at compile time and wrong */
    }
  return true;
}

void TWI0_ClearPins() {
  #ifdef PORTMUX_TWIROUTEA
    uint8_t muxval = PORTMUX.TWIROUTEA & PORTMUX_TWI0_gm; // registers are volatile, so this is bulky to recheck multiple times.
    if (muxval == PORTMUX_TWI0_ALT2_gc) {
      // make sure we don't get errata'ed - make sure their bits in the
      // PORTx.OUT registers are 0.
      PORTC.OUTCLR = 0x0C;  // bits 2 and 3
    } else {
      #if !defined(__AVR_DD__)
        PORTA.OUTCLR = 0x0C;  // bits 2 and 3
      #else
        PORTA.OUTCLR = (muxval == 3 ? 0x03 : 0x0C)
      #endif
    }
    #if defined(TWI_DUALCTRL)
      if (TWI0.DUALCTRL & TWI_ENABLE_bm) {
        #if !defined(__AVR_DD__)
          if (muxval == PORTMUX_TWI0_DEFAULT_gc) {
            PORTC.OUTCLR = 0x0C;  // bits 2 and 3
          } else {
            PORTC.OUTCLR = 0xC0;  // bits 6 and 7
          }
        #else
          PORTC.OUTCLR = 0x0C; // AVR DD only has one set of slave only pins.
        #endif
      }
    #endif
  #else  // megaTinyCore
    #if defined(PORTMUX_TWI0_bm)
      if ((PORTMUX.CTRLB & PORTMUX_TWI0_bm)) {
        // make sure we don't get errata'ed - make sure their bits in the
        // PORTx.OUT registers are 0.
        PORTA.OUTCLR = 0x06;  // if swapped it's on PA1, PA2
      } else {
        PORTB.OUTCLR = 0x03;  // else PB0, PB1
      }
    #elif defined(__AVR_ATtinyxy2__)
      PORTA.OUTCLR = 0x06;  // 8-pin parts always have it on PA1/2
    #else
      PORTB.OUTCLR = 0x03;  // else, zero series, no remapping, it's on PB0, PB1
  #endif
#endif
}


bool TWI0_Pins(uint8_t sda_pin, uint8_t scl_pin) {
  #if defined(PIN_WIRE_SDA)
    #if (defined(PIN_WIRE_SDA_PINSWAP_1) || defined(PIN_WIRE_SDA_PINSWAP_2) || defined(PIN_WIRE_SDA_PINSWAP_3))
      if (TWI_checkPins(sda_pin, scl_pin) == false) {
        return false;
      } /* End of test for compile time known SDA and SCL pins requested */

      #if defined(PORTMUX_CTRLB) /* tinyAVR 0/1 with TWI mux options */
        #if defined(PIN_WIRE_SDA_PINSWAP_1)
          if (sda_pin == PIN_WIRE_SDA_PINSWAP_1 && scl_pin == PIN_WIRE_SCL_PINSWAP_1) {
            // Use pin swap
            PORTMUX.CTRLB |= PORTMUX_TWI0_bm;
            return true;
          } else if (sda_pin == PIN_WIRE_SDA && scl_pin == PIN_WIRE_SCL) {
            // Use default configuration
            PORTMUX.CTRLB &= ~PORTMUX_TWI0_bm;
            return true;
          } else {
            // Assume default configuration
            PORTMUX.CTRLB &= ~PORTMUX_TWI0_bm;
            return false;
          }
        #else /* tinyAVR 0/1 without TWI mux options */
           return (sda_pin == PIN_WIRE_SDA && scl_pin == PIN_WIRE_SCL);
        #endif
      #elif defined(PORTMUX_TWIROUTEA)
      uint8_t portmuxTWI = (PORTMUX.TWIROUTEA & ~PORTMUX_TWI0_gm);
        #if      defined(PIN_WIRE_SDA_PINSWAP_3)
          if (sda_pin == PIN_WIRE_SDA_PINSWAP_3 && scl_pin == PIN_WIRE_SCL_PINSWAP_3) {
            // Use pin swap
            PORTMUX.TWIROUTEA = portmuxTWI | PORTMUX_TWI0_ALT3_gc;
            return true;
          } else
        #endif
        #if      defined(PIN_WIRE_SDA_PINSWAP_2)
          if (sda_pin == PIN_WIRE_SDA_PINSWAP_2 && scl_pin == PIN_WIRE_SCL_PINSWAP_2) {
            // Use pin swap
            PORTMUX.TWIROUTEA = portmuxTWI | PORTMUX_TWI0_ALT2_gc;
            return true;
          } else
        #endif
        #if      defined(PIN_WIRE_SDA_PINSWAP_1)
          if (sda_pin == PIN_WIRE_SDA_PINSWAP_1 && scl_pin == PIN_WIRE_SCL_PINSWAP_1) {
            // Use pin swap
            PORTMUX.TWIROUTEA = portmuxTWI | PORTMUX_TWI0_ALT1_gc;
            return true;
          } else
        #endif
        #if      defined(PIN_WIRE_SDA)
          if (sda_pin == PIN_WIRE_SDA && scl_pin == PIN_WIRE_SCL) {
            // Use default configuration
            PORTMUX.TWIROUTEA = portmuxTWI;
            return true;
          } else {
            // Assume default configuration
            #ifdef __AVR_DD__   /* DD with 14 pins has no default pins in the "default" "position! Default to alt=2 */
              PORTMUX.TWIROUTEA = portmuxTWI | PORTMUX_TWI0_ALT2_gc;  // Alt2 pins:  PC2, PC3, PC6, PC7
            #else
              PORTMUX.TWIROUTEA = portmuxTWI;
            #endif
            return false;
          }
        #endif
      #endif
    #else  // No TWI pin options - why call this?
      if (__builtin_constant_p(sda_pin) && __builtin_constant_p(scl_pin)) {
        /* constant case - error if there's no swap and the swap attempt is known at compile time */
        if (sda_pin != PIN_WIRE_SDA || scl_pin != PIN_WIRE_SCL) {
          badCall("This part does not support alternate Wire pins, if Wire.pins() is called, it must be passed the default pins");
          return false;
        } else {
          return true;
        }
      } else {  /* Non-constant case */
        return (sda_pin == PIN_WIRE_SDA && scl_pin == PIN_WIRE_SCL);
      }
    #endif
  #endif
  return false;
}


bool TWI0_swap(uint8_t state) {
  #if defined(PORTMUX_CTRLB) /* tinyAVR 0/1-series */
    #if defined(PORTMUX_TWI0_bm)
      if (state == 1) {
        // Use pin swap
        PORTMUX.CTRLB |=  PORTMUX_TWI0_bm;
        return true;
      } else if (state == 0) {
        // Use default configuration
        PORTMUX.CTRLB &= ~PORTMUX_TWI0_bm;
        return true;
      } else {
        // Assume default configuration
        PORTMUX.CTRLB &= ~PORTMUX_TWI0_bm;
        return false;
      }
    #else  // keep compiler happy
      if (__builtin_constant_p(state)) {
        if (state != 0) {
          badCall("This part does not support alternate TWI pins. If Wire.swap() is called at all, it must be passed 0 only.");
          return false;
        } else {
          return true;
        }
      } else {
        return !state;
      }
      #endif
  #elif defined(PORTMUX_TWIROUTEA) /* AVR Dx-series */
    uint8_t muxval = (PORTMUX.TWIROUTEA & ~PORTMUX_TWI0_gm);
    #if defined(PIN_WIRE_SDA_PINSWAP_3)
      if (state == 3) { /* PA0, PA1, PC2, PC3 */
        // Use pin swap
        PORTMUX.TWIROUTEA = muxval | PORTMUX_TWI0_ALT3_gc;
        return true;
      } else
    #endif
    #if defined(PIN_WIRE_SDA_PINSWAP_2)
      if (state == 2) {
        // Use pin swap
        PORTMUX.TWIROUTEA = muxval | PORTMUX_TWI0_ALT2_gc;
        return true;
      } else
    #endif
    #if defined(PIN_WIRE_SDA_PINSWAP_1)
      if (state == 1) {
        // Use pin swap
        PORTMUX.TWIROUTEA = muxval | PORTMUX_TWI0_ALT1_gc;
        return true;
      } else
    #endif
    #if defined(PIN_WIRE_SDA)
      {
        // Use default configuration
        PORTMUX.TWIROUTEA = muxval;
        return state == 0;
      }
    #else // DD with no PA2/3 so "default" is alt2
      {
        // Assume default configuration
        PORTMUX.TWIROUTEA = muxval | PORTMUX_TWI0_ALT2_gc;
        return false;
      }
    #endif
  #else /* tinyAVR 2-series with neither CTRLB nor TWIROUTEA*/
    if (__builtin_constant_p(state)) {
      if (state != 0) {
        badCall("This part does not support alternate TWI pins. If Wire.swap() is called at all, it must be passed 0 only.");
        return false;
      } else {
        return true;
      }
    } else {
      return !state;
    }
  #endif
  return false;
}


void TWI0_usePullups() {
  // make sure we don't get errata'ed - make sure their bits in the output registers are off!
  #ifdef DXCORE
    #ifndef __AVR_DD__
      PORT_t *port = portToPortStruct(PORTMUX.TWIROUTEA & 0x02);
    #else
      uint8_t temp = PORTMUX.TWIROUTEA & PORTMUX_TWI0_gm;
      PORT_t *port = portToPortStruct(temp == 2 ? PC : PA);
      if (3 == temp) {
        port->PIN0CTRL |= PORT_PULLUPEN_bm;
        port->PIN1CTRL |= PORT_PULLUPEN_bm;
        port->OUTCLR    = 0x03;  // bits 0 and 1
      } else {
    #endif
      port->PIN2CTRL |= PORT_PULLUPEN_bm;
      port->PIN3CTRL |= PORT_PULLUPEN_bm;
      port->OUTCLR    = 0x0C;  // bits 2 and 3
    #ifdef __AVR_DD__
      }
    #endif
    #if defined(TWI_DUALCTRL)
      if (true == ((TWI0.DUALCTRL) & TWI_ENABLE_bm)) {
        #ifndef (__AVR_DD__)
          if ((PORTMUX.TWIROUTEA & PORTMUX_TWI0_gm) == 0) {
            PORTC.PIN2CTRL |= PORT_PULLUPEN_bm;
            PORTC.PIN3CTRL |= PORT_PULLUPEN_bm;
            PORTC.OUTCLR    = 0x0C;  // bits 2 and 3
          } else {
            PORTC.PIN6CTRL |= PORT_PULLUPEN_bm;
            PORTC.PIN7CTRL |= PORT_PULLUPEN_bm;
            PORTC.OUTCLR    = 0xC0;  // bits 6 and 7
          }
        #else
          // AVR DD-series only ever has slave only I2C on PC2 and PC3, according to the headers.
          // Though the product brief claims it can use PF2, PF3 on 32 pin parts too, which would require
          // more mux options) It is unclear if this is a copy-paste error like the PB2 mention
          // of a TWI1 on the EA brief is assumed to be, or they really did add more options.
          PORTC.PIN2CTRL |= PORT_PULLUPEN_bm;
          PORTC.PIN3CTRL |= PORT_PULLUPEN_bm;
          PORTC.OUTCLR    = 0x0C;
        #endif
      }
    #endif
  #else  // megaTinyCore
    #if defined(PORTMUX_TWI0_bm)
      if (true == (PORTMUX.CTRLB & PORTMUX_TWI0_bm)) {
        PORTA.PIN2CTRL |= PORT_PULLUPEN_bm;
        PORTA.PIN1CTRL |= PORT_PULLUPEN_bm;
        PORTA.OUTCLR    = 0x06;
      } else {
        PORTB.PIN1CTRL |= PORT_PULLUPEN_bm;
        PORTB.PIN0CTRL |= PORT_PULLUPEN_bm;
        PORTB.OUTCLR    = 0x03;  // bits 1 and 0.
      }
    #elif defined(__AVR_ATtinyxy2__)
      PORTA.PIN2CTRL |= PORT_PULLUPEN_bm;
      PORTA.PIN1CTRL |= PORT_PULLUPEN_bm;
      PORTA.OUTCLR    = 0x06;  // bits 2 and 1.
    #else
      PORTB.PIN1CTRL |= PORT_PULLUPEN_bm;
      PORTB.PIN0CTRL |= PORT_PULLUPEN_bm;
      PORTB.OUTCLR    = 0x03;  // bits 1 and 0.
    #endif
  #endif
}
#if defined(TWI1)
void TWI1_ClearPins() {
  #if defined(PORTMUX_TWIROUTEA)
    #if defined(PIN_WIRE1_SDA_PINSWAP_2)
      if ((PORTMUX.TWIROUTEA & PORTMUX_TWI1_gm) == PORTMUX_TWI1_ALT2_gc) {
      // make sure we don't get errata'ed - make sure their bits in the
      // PORTx.OUT registers are 0.
      PORTB.OUTCLR = 0x0C;  // bits 2 and 3
      } else
    #endif
    {
      PORTF.OUTCLR = 0x0C;  // bits 2 and 3
    }
    #if defined(TWI_DUALCTRL)
      if (TWI1.DUALCTRL & TWI_ENABLE_bm) {
        if ((PORTMUX.TWIROUTEA & PORTMUX_TWI1_gm) == PORTMUX_TWI1_DEFAULT_gc) {
          PORTB.OUTCLR = 0x0C;  // bits 2 and 3
        } else {
          PORTB.OUTCLR = 0xC0;  // bits 6 and 7
        }
      }
    #endif
  #endif   // Only Dx-Series has 2 TWI
}


bool TWI1_Pins(uint8_t sda_pin, uint8_t scl_pin) {
  #if defined(PIN_WIRE1_SDA)
    #if (defined(PIN_WIRE1_SDA_PINSWAP_1) || defined(PIN_WIRE1_SDA_PINSWAP_2))
      if (TWI_checkPins(sda_pin, scl_pin) == false) {
        return false;
      }/* End of test for compile time known SDA and SCL pins requested */

      #if defined(PORTMUX_TWIROUTEA)
      uint8_t portmuxTWI1 =  PORTMUX.TWIROUTEA & ~PORTMUX_TWI1_gm;
        #if defined(PIN_WIRE1_SDA_PINSWAP_2)
          if (sda_pin == PIN_WIRE1_SDA_PINSWAP_2 && scl_pin == PIN_WIRE1_SCL_PINSWAP_2) {
            // Use pin swap
            PORTMUX.TWIROUTEA = portmuxTWI1 | PORTMUX_TWI1_ALT2_gc;
            return true;
          } else
        #endif
        #if defined(PIN_WIRE1_SDA_PINSWAP_1)
          if (sda_pin == PIN_WIRE1_SDA_PINSWAP_1 && scl_pin == PIN_WIRE1_SCL_PINSWAP_1) {
            // Use pin swap
            PORTMUX.TWIROUTEA = portmuxTWI1 | PORTMUX_TWI1_ALT1_gc;
            return true;
          } else
        #endif
        #if defined(PIN_WIRE1_SDA)
          if (sda_pin == PIN_WIRE1_SDA && scl_pin == PIN_WIRE1_SCL) {
            // Use default configuration
            PORTMUX.TWIROUTEA = portmuxTWI1;
            return true;
          } else {
            // Assume default configuration
            PORTMUX.TWIROUTEA = portmuxTWI1;
            return false;
          }
        #endif
      #endif
    #else  // No TWI pin options - why call this?
      if (__builtin_constant_p(sda_pin) && __builtin_constant_p(scl_pin)) {
        /* constant case - error if there's no swap and the swap attempt is known at compile time */
        if (sda_pin != PIN_WIRE1_SDA || scl_pin != PIN_WIRE1_SCL) {
          badCall("This part does not support alternate Wire1 pins, if Wire1.pins() is called, it must be passed the default pins");
          return false;
        } else {
          return true;
        }
      } else { /* Non-constant case */
        return (sda_pin == PIN_WIRE1_SDA && scl_pin == PIN_WIRE1_SCL);
      }
    #endif
  #endif
  return false;
}



bool TWI1_swap(uint8_t state) {
  #if defined(PORTMUX_TWIROUTEA) /* AVR Dx-series */
    #if defined(PIN_WIRE1_SDA_PINSWAP_2)
      if (state == 2) {
        // Use pin swap
        PORTMUX.TWIROUTEA = (PORTMUX.TWIROUTEA & !PORTMUX_TWI1_gm) | PORTMUX_TWI1_ALT2_gc;
        return true;
      } else
    #endif
    #if defined(PIN_WIRE1_SDA_PINSWAP_1)
      if (state == 1) {
        // Use pin swap
        PORTMUX.TWIROUTEA = (PORTMUX.TWIROUTEA & !PORTMUX_TWI1_gm) | PORTMUX_TWI1_ALT1_gc;
        return true;
      } else
    #endif
    #if defined(PIN_WIRE1_SDA)
      if (state == 0) {
        // Use default configuration
        PORTMUX.TWIROUTEA = (PORTMUX.TWIROUTEA & !PORTMUX_TWI1_gm);
        return true;
      } else {
        // Assume default configuration
        PORTMUX.TWIROUTEA = (PORTMUX.TWIROUTEA & !PORTMUX_TWI1_gm);
        return false;
      }
    #else
      {
        // Assume default configuration
        PORTMUX.TWIROUTEA = ((PORTMUX.TWIROUTEA & !PORTMUX_TWI1_gm) | PORTMUX_TWI1_ALT2_gc);
        return false;
      }
    #endif
  #endif
  return false;
}


void TWI1_usePullups() {
  #if defined(TWI1)
    uint8_t temp = PORTMUX.TWIROUTEA & PORTMUX_TWI1_gm;
    PORT_t *port = portToPortStruct(temp == 8?PB:PF);
    if (3 == temp) {
      port->OUTCLR = 0x03;  // bits 0 and 1
      port->PIN0CTRL |= PORT_PULLUPEN_bm;
      port->PIN1CTRL |= PORT_PULLUPEN_bm;
    } else {
      port->OUTCLR = 0x0C;  // bits 2 and 3
      port->PIN2CTRL |= PORT_PULLUPEN_bm;
      port->PIN3CTRL |= PORT_PULLUPEN_bm;
    }
    #if defined(TWI_DUALCTRL)
      if (TWI1.DUALCTRL & TWI_ENABLE_bm) {
        if (0 == temp) {
          PORTB.OUTCLR = 0x0C;  // bits 2 and 3
          PORTB.PIN2CTRL |= PORT_PULLUPEN_bm;
          PORTB.PIN3CTRL |= PORT_PULLUPEN_bm;
        } else {
          PORTB.OUTCLR = 0xC0;  // bits 6 and 7
          PORTB.PIN6CTRL |= PORT_PULLUPEN_bm;
          PORTB.PIN7CTRL |= PORT_PULLUPEN_bm;
        }
      }
    #endif
  #endif
}
#endif

#endif /* TWI_DRIVER_H */
