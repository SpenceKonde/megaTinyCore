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
  Modified 2021-22 by MX682X for the Wire library rewriting project
*/
// *INDENT-OFF*   astyle wants this file to be completely unreadable with no indentation for the many preprocessor conditionals!
#ifndef TWI_PINS_H
#define TWI_PINS_H

#include <Arduino.h>
#include "twi_pins.h"
/*
bool TWI_checkPins(uint8_t sda_pin, uint8_t scl_pin);

bool TWI_checkPins(uint8_t sda_pin, uint8_t scl_pin) {
  if (sda_pin != (scl_pin - 1)) { // SDA and SCL are always next to each other
      return false;
  }
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
            badArg("Pins passed to Wire.pins() or Wire1.pins() known at compile time to be invalid");
          #else
            badArg("Pins passed to Wire.pins() known at compile time to be invalid");
          #endif
          return false;
        }
    }
    // For this to work:
    // 1) sda_pin = scl_pin - 1 must actually be true for all parts (it's not, because portb is numbered backwards)
    // 2) This must be part of Wire.cpp so that the constants that are passed can be constant folded
  return true;
}
*/
void TWI0_ClearPins() {
  // Set the pins low so we don't get errata'ed.
  // On parts where this erratum has been fixed, we shouldn't need to do *anything* for this!
  #if defined(PORTMUX_TWIROUTEA) // AVR Dx or Ex-series
    // Mux options for Dx/Ex-series TWI 0:
    // PA2, PA3, PC2, PC3 - Default
    // PA2, PA3, PC6, PC7 - Alt 1
    // PC2, PC3, PC6, PC7 - Alt 2
    // PA0, PA1, PC2, PC3 - Alt 3, not available on DA, DB
    #if defined(__AVR_DA__) || defined(__AVR_DB__)
      uint8_t portmux = PORTMUX.TWIROUTEA & PORTMUX_TWI0_gm; // registers are volatile, so it's bulky to recheck multiple times.
      if (portmux == PORTMUX_TWI0_ALT2_gc) {
        PORTC.OUTCLR = 0x0C;  // bits 2 and 3
      } else {
        #if defined(__AVR_DD__) || defined(__AVR_EA__)
          PORTA.OUTCLR = (portmux == PORTMUX_TWI0_ALT3_gc ? 0x03 : 0x0C);
        #else
          PORTA.OUTCLR = 0x0C;  // bits 2 and 3
        #endif
      }
      #if defined(TWI0_DUALCTRL)
        if (TWI0.DUALCTRL & TWI_ENABLE_bm) {
          #if defined(__AVR_DD__) || defined(__AVR_EA__)
            if ((portmux == PORTMUX_TWI0_DEFAULT_gc) || (portmux == PORTMUX_TWI0_ALT3_gc)) {
              PORTC.OUTCLR = 0x0C; // AVR DD only has one set of slave-only pins.
            }
            #if defined(__AVR_EA__)
              // DD-series has at most 32 pins, hence no PC6, PC7.
              // EA-series however can have 48 pins, and hence a second set of slave-only pins.
              else {
                PORTC.OUTCLR = 0xC0;
              }
            #endif
          #else
            PORTC.OUTCLR = (portmux == PORTMUX_TWI0_DEFAULT_gc ? 0x0C : 0xC0);
          #endif
        }
      #endif
    #endif
  #elif defined(PORTMUX_TWISPIROUTEA)
    // Mux options for megaAVR 0-series:
    // PA2, PA3, PC2, PC3 - Default
    // PA2, PA3, PF2, PF3 - Alt 1
    // PC2, PC3, PF2, PF3 - Alt 2
    uint8_t portmux = PORTMUX.TWISPIROUTEA & PORTMUX_TWI0_gm; // registers are volatile, so it's bulky to recheck multiple times.
    if (portmux == PORTMUX_TWI0_ALT2_gc) {
      PORTC.OUTCLR = 0x0C;  // bits 2 and 3
    } else {
      PORTA.OUTCLR = 0x0C;  // bits 2 and 3
    }
    #if defined(TWI0_DUALCTRL)
      if (TWI0.DUALCTRL & TWI_ENABLE_bm) {
        if (portmux == PORTMUX_TWI0_DEFAULT_gc) {
          PORTC.OUTCLR = 0x0C;
        } else {
          PORTF.OUTCLR = 0x0C;
        }
      }
    #endif
  #elif defined(MEGATINYCORE)  /* tinyAVR 0/1-series */
    #if (MEGATINYCORE_SERIES < 2) // Hey, this erratum isn't listed for the 2-series, so we don't have to do this!
      // 6 bytes of flash saved for 2-series parts, aww yeah!
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
        PORTB.OUTCLR = 0x03;  // else it's 0-series or 2-series. Either way, there's no remapping: it's on PB0, PB1
      #endif
    #endif
  #else
    #error "This version of Wire only supports the modern AVR (tinyAVR 0/1/2-series, megaAVR 0-series, AVR Dx-series and AVR-Ex-series parts."
  #endif
}


bool TWI0_Pins(uint8_t sda_pin, uint8_t scl_pin) {
  #if (defined(PIN_WIRE_SDA_PINSWAP_1) || defined(PIN_WIRE_SDA_PINSWAP_2) || defined(PIN_WIRE_SDA_PINSWAP_3))
// --- Attiny series ---
    #if defined(PORTMUX_CTRLB)
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
      #else // tinyAVR 0/1 without TWI multiplexer options
         return (sda_pin == PIN_WIRE_SDA);
      #endif
// --- mega0 series ---
    #elif defined(PORTMUX_TWISPIROUTEA)
        uint8_t twimux = PORTMUX.TWISPIROUTEA & ~PORTMUX_TWI0_gm;
        #if defined(PIN_WIRE_SDA_PINSWAP_2)
          if (sda_pin == PIN_WIRE_SDA_PINSWAP_2 && scl_pin == PIN_WIRE_SCL_PINSWAP_2) {
            twimux |= PORTMUX_TWI0_ALT2_gc;
            PORTMUX.TWISPIROUTEA = twimux;
            return true;
          #endif
          /* Can't happen */
        #if defined(PIN_WIRE_SDA_PINSWAP_1)
          if (sda_pin == PIN_WIRE_SDA_PINSWAP_1 && scl_pin == PIN_WIRE_SCL_PINSWAP_1) {
            // Use pin swap
            twimux |= PORTMUX_TWI0_ALT1_gc;
            PORTMUX.TWISPIROUTEA = twimux;
            return true;
          }
          /* end can'thappen */x
        #endif
        } else if (sda_pin == PIN_WIRE_SDA && scl_pin == PIN_WIRE_SCL) {
          // Use default configuration
          twimux &= ~PORTMUX_TWI0_gm;
          return true;
        } else {
          // Assume default configuration
          twimux &= ~PORTMUX_TWI0_gm;
          return false;
        }
// --- Dx series ---
    #elif defined(PORTMUX_TWIROUTEA)
      uint8_t portmux = (PORTMUX.TWIROUTEA & ~PORTMUX_TWI0_gm);
      #if      defined(PIN_WIRE_SDA_PINSWAP_3)
        if (sda_pin == PIN_WIRE_SDA_PINSWAP_3 && scl_pin == PIN_WIRE_SCL_PINSWAP_3) {
          PORTMUX.TWIROUTEA = portmux | PORTMUX_TWI0_ALT3_gc;
          return true;
        } else
      #endif
      #if      defined(PIN_WIRE_SDA_PINSWAP_2)
        #if !defined(ERRATA_TWI0_MUX2)
          if (sda_pin == PIN_WIRE_SDA_PINSWAP_2 && scl_pin == PIN_WIRE_SCL_PINSWAP_2) {
            PORTMUX.TWIROUTEA = portmux | PORTMUX_TWI0_ALT2_gc;
            return true;
        #else
          return false;
        #endif
        } else
      #endif
      #if      defined(PIN_WIRE_SDA_PINSWAP_1)
        if (sda_pin == PIN_WIRE_SDA_PINSWAP_1 && scl_pin == PIN_WIRE_SCL_PINSWAP_1) {
          PORTMUX.TWIROUTEA = portmux | PORTMUX_TWI0_ALT1_gc;
          return true;
        } else
      #endif
      #if      defined(PIN_WIRE_SDA)
        if (sda_pin == PIN_WIRE_SDA && scl_pin == PIN_WIRE_SCL) {
          // Use default configuration
          PORTMUX.TWIROUTEA = portmux;
          return true;
        } else {
          // Assume default configuration
          #if defined(__AVR_DD__)   /* DD with 14 pins has no default pins in the "default" "position! Default to alt=2 */
            PORTMUX.TWIROUTEA = portmux | PORTMUX_TWI0_ALT2_gc;  // Alt2 pins:  PC2, PC3, PC6, PC7
          #else
            PORTMUX.TWIROUTEA = portmux;
          #endif
          return false;
        }
      #endif
    #endif
  // --- No TWI Pin multiplexer ---
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
  return false;
}


bool TWI0_swap(uint8_t state) {
  #if defined(PORTMUX_CTRLB) /* tinyAVR 0/1-series */
    #if defined(PORTMUX_TWI0_bm)
      if (state == 1) {
        // Use pin swap
        PORTMUX.CTRLB |= PORTMUX_TWI0_bm;
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
    #else // tinyAVR 0-series, no alternate pins.
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
    uint8_t portmux = PORTMUX.TWIROUTEA & (~PORTMUX_TWI0_gm);
    #if defined(PIN_WIRE_SDA_PINSWAP_3)
      if (state == 3) { /* PA0, PA1, PC2, PC3 */
        // Use pin swap
        PORTMUX.TWIROUTEA = portmux | PORTMUX_TWI0_ALT3_gc;
        return true;
      } else
    #endif
    #if defined(PIN_WIRE_SDA_PINSWAP_2)
      if (state == 2) {
        // Use pin swap
        #if !defined(ERRATA_TWI0_MUX2)
          PORTMUX.TWIROUTEA = portmux | PORTMUX_TWI0_ALT2_gc;
          return true;
        #else
          return false;
        #endif
      } else
    #endif
    #if defined(PIN_WIRE_SDA_PINSWAP_1)
      if (state == 1) {
        // Use pin swap
        PORTMUX.TWIROUTEA = portmux | PORTMUX_TWI0_ALT1_gc;
        return true;
      } else
    #endif
    #if defined(PIN_WIRE_SDA) // True **unless** a DD with only 14 pins, and hence no swap levels 0 or 1.
      {
        // Use default configuration
        PORTMUX.TWIROUTEA = portmux;
        return state == 0;
      }
    #else // DD with no PA2/3 so "default" is alt2
      {
        PORTMUX.TWIROUTEA = portmux | PORTMUX_TWI0_ALT2_gc;
        return false; // on a DD14, hence without PA2, PA3, Wire.swap(0) and Wire.swap(1) are invalid, Wire.swap(2) and Wire.swap(3) are valid - but we tested for those already. Hence, if we're here,
      }
    #endif
  #elif defined(PORTMUX_TWISPIROUTEA) // megaAVR 0-series
    uint8_t portmux = PORTMUX.TWISPIROUTEA & (~PORTMUX_TWI0_gm);
    #if defined(PIN_WIRE_SDA_PINSWAP_2)
      if (state == 2) {
        // Use pin swap
        PORTMUX.TWISPIROUTEA = portmux | PORTMUX_TWI0_ALT2_gc;
        return true;
      } else
    #endif
    #if defined(PIN_WIRE_SDA_PINSWAP_1)
      if (state == 1) {
        // Use pin swap
        PORTMUX.TWISPIROUTEA = portmux | PORTMUX_TWI0_ALT1_gc;
        return true;
      } else
    #endif
    {
      // Use default configuration
      PORTMUX.TWISPIROUTEA = portmux;
      return state == 0;
    }
  #else /* tinyAVR 2-series with neither CTRLB nor TWIROUTEA because no alternate pins for TWI*/
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
  // make sure we don't get errata'ed: Clear the pins
  // and then turn on the pullups.
  #if defined(PORTMUX_TWIROUTEA) // Dx or Ex-series
    uint8_t portmux = PORTMUX.TWIROUTEA & PORTMUX_TWI0_gm;
    PORT_t *port;
    // PORTA and PORTC are present on all parts with a TWIROUTEA register
    if(portmux == PORTMUX_TWI0_ALT2_gc) {
      port = &PORTC;
    } else {
      port = &PORTA;
    }
    #if !defined(__AVR_DA__) && !defined(__AVR_DB__) //DD and EA, and presumably later parts, have an extra mux option.
      if (portmux == 3) {
        port->OUTCLR    = 0x03;  // bits 0 and 1
        port->PIN0CTRL |= PORT_PULLUPEN_bm;
        port->PIN1CTRL |= PORT_PULLUPEN_bm;
      } else
    #endif
    {
      port->OUTCLR    = 0x0C;  // bits 2 and 3
      port->PIN2CTRL |= PORT_PULLUPEN_bm;
      port->PIN3CTRL |= PORT_PULLUPEN_bm;
    }
    #if defined(TWI0_DUALCTRL)   //Also handle slave pins, if enabled
      if (TWI0.DUALCTRL & TWI_ENABLE_bm) {
        #if !(defined(__AVR_DA__) || defined(__AVR_DB__))
          if (portmux == PORTMUX_TWI0_DEFAULT_gc ||
              portmux == PORTMUX_TWI0_ALT3_gc) {
            PORTC.OUTCLR    = 0x0C;
            PORTC.PIN2CTRL |= PORT_PULLUPEN_bm;
            PORTC.PIN3CTRL |= PORT_PULLUPEN_bm;
          }
        #else
          if (portmux == PORTMUX_TWI0_DEFAULT_gc) {
            PORTC.OUTCLR    = 0x0C;  // bits 2 and 3
            PORTC.PIN2CTRL |= PORT_PULLUPEN_bm;
            PORTC.PIN3CTRL |= PORT_PULLUPEN_bm;
          } else {
            PORTC.OUTCLR    = 0xC0;  // bits 6 and 7
            PORTC.PIN6CTRL |= PORT_PULLUPEN_bm;
            PORTC.PIN7CTRL |= PORT_PULLUPEN_bm;
          }
        #endif
      }
    #endif
  #elif defined(PORTMUX_TWISPIROUTEA) // megaAVR 0-series
    uint8_t portmux = PORTMUX.TWISPIROUTEA & PORTMUX_TWI0_gm;
    PORT_t *port;
    // Note that all megaAVR 0-series parts have a PORTA, PORTC and PORTF!
    if(portmux == PORTMUX_TWI0_ALT2_gc) {
      port = &PORTC;
    } else {
      port = &PORTA;
    }
    port->OUTCLR    = 0x0C;  // bits 2 and 3
    port->PIN2CTRL |= PORT_PULLUPEN_bm;
    port->PIN3CTRL |= PORT_PULLUPEN_bm;
    #if defined(TWI0_DUALCTRL)   //Also handle slave pins, if enabled
      if (TWI0.DUALCTRL & TWI_ENABLE_bm) {
        if (portmux == PORTMUX_TWI0_DEFAULT_gc) {
          port = &PORTC;
        } else {
          port = &PORTF;
        }
        port->OUTCLR    = 0x0C;  // bits 2 and 3
        port->PIN2CTRL |= PORT_PULLUPEN_bm;
        port->PIN3CTRL |= PORT_PULLUPEN_bm;
      }
    #endif
  #elif defined(MEGATINYCORE) && MEGATINYCORE_SERIES != 2  /* tinyAVR 0/1-series */
    #if defined(PORTMUX_TWI0_bm) // 1-series with remappable TWI
      if (true == (PORTMUX.CTRLB & PORTMUX_TWI0_bm)) {
        PORTA.OUTCLR    = 0x06;
        PORTA.PIN2CTRL |= PORT_PULLUPEN_bm;
        PORTA.PIN1CTRL |= PORT_PULLUPEN_bm;
      } else {
        PORTB.OUTCLR    = 0x03;  // bits 1 and 0.
        PORTB.PIN1CTRL |= PORT_PULLUPEN_bm;
        PORTB.PIN0CTRL |= PORT_PULLUPEN_bm;
      }
    #elif defined(__AVR_ATtinyxy2__) // 8-pin 0/1-series part
      PORTA.OUTCLR    = 0x06;  // bits 2 and 1.
      PORTA.PIN2CTRL |= PORT_PULLUPEN_bm;
      PORTA.PIN1CTRL |= PORT_PULLUPEN_bm;
    #else // 0-series or 2-series part with no remapping options
      PORTB.OUTCLR    = 0x03;  // bits 1 and 0.
      PORTB.PIN1CTRL |= PORT_PULLUPEN_bm;
      PORTB.PIN0CTRL |= PORT_PULLUPEN_bm;
    #endif
  #endif
}

//Check if TWI0 Master pins have a HIGH level: Bit0 = SDA, Bit 1 = SCL
uint8_t TWI0_checkPinLevel(void) {
  #if defined(PORTMUX_TWIROUTEA)     /* Dx-series */
    uint8_t portmux = (PORTMUX.TWIROUTEA & PORTMUX_TWI0_gm);
    VPORT_t *vport;
    if (portmux == PORTMUX_TWI0_ALT2_gc) {
      vport = &VPORTC;
    } else {
      vport = &VPORTA;
    }

    #if !defined(__AVR_DA__) && !defined(__AVR_DB__) //DD and EA, and presumably later parts, have an extra mux option.
      if (portmux == 3) {
        return (vport->IN & 0x03);
      } else
    #endif
    {
      return ((vport->IN & 0x0C) >> 2);
    }
  #elif defined(PORTMUX_TWISPIROUTEA)
    uint8_t portmux = (PORTMUX.TWISPIROUTEA & PORTMUX_TWI0_gm);
    VPORT_t *vport;
    if (portmux == PORTMUX_TWI0_ALT2_gc) {
      vport = &VPORTC;
    } else {
      vport = &VPORTA;
    }
    return ((vport->IN & 0x0C) >> 2);
  #elif defined(MEGATINYCORE)  /* tinyAVR 0/1-series */
    #if defined(PORTMUX_TWI0_bm)  // Has a pin multiplexer
      if (true == (PORTMUX.CTRLB & PORTMUX_TWI0_bm)) {
        return ((VPORTA.IN & 0x06) >> 1);
      } else {
        return (VPORTB.IN & 0x03);
      }
    #elif defined(__AVR_ATtinyxy2__)  // No PORTMUX for 8-pin parts, they always use PA1/2
      return ((VPORTA.IN & 0x06) >> 1);
    #else //it uses PB0/1
      return (VPORTB.IN & 0x03);
    #endif
  #else
    #error "Only modern AVR parts are supported by this version of Wire: tinyAVR 0/1/2-series, megaAVR 0-series, AVR Dx-series or AVR Ex-series."
    return 0;
  #endif
}


uint8_t TWI0_setConfig(bool longsetup, uint8_t sda_hold) {
  uint8_t cfg = TWI0.CTRLA & 0x03;
  if (longsetup) {
    cfg |= 0x10;
  }
  cfg |= sda_hold;
  TWI0.CTRLA = cfg;
  return 0; // return success - all other errors are checked for before this is called.
}

#endif /* TWI_PINS_H */