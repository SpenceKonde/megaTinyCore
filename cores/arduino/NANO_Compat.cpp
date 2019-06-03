/*
  Copyright (c) 2019 Arduino.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "NANO_Compat.h"

#ifdef AVR_NANO_4809_328MODE

#warning "ATMEGA328 registers emulation is enabled. You may encounter some speed issue. Please consider to disable it in the Tools menu"

#define PORTA_ARDUINO   (*(PORT_t *) 0x0400) /* I/O Ports */
#define PORTB_ARDUINO   (*(PORT_t *) 0x0420) /* I/O Ports */
#define PORTC_ARDUINO   (*(PORT_t *) 0x0440) /* I/O Ports */
#define PORTD_ARDUINO   (*(PORT_t *) 0x0460) /* I/O Ports */
#define PORTE_ARDUINO   (*(PORT_t *) 0x0480) /* I/O Ports */
#define PORTF_ARDUINO   (*(PORT_t *) 0x04A0) /* I/O Ports */

#ifndef HOST_BUILD
  DDRBClass  DDRB (&PORTB_ARDUINO, &PORTE_ARDUINO);
  PORTBClass PORTB(&PORTB_ARDUINO, &PORTE_ARDUINO);
  DDRCClass  DDRC (&PORTA_ARDUINO, &PORTD_ARDUINO);
  PORTCClass PORTC(&PORTA_ARDUINO, &PORTD_ARDUINO);
  DDRDClass  DDRD (&PORTA_ARDUINO, &PORTB_ARDUINO, &PORTC_ARDUINO, &PORTF_ARDUINO);
  PORTDClass PORTD(&PORTA_ARDUINO, &PORTB_ARDUINO, &PORTC_ARDUINO, &PORTF_ARDUINO);
#endif /* HOST_BUILD */

/*****************************************************************************/

inline void set_dir_register(uint8_t const value, uint8_t const nano_328_bp, PORT_t * nano_4809_port, uint8_t const nano_4809_bp) {
  if(value & (1 << nano_328_bp)) nano_4809_port->DIRSET = (1 << nano_4809_bp);
} 

inline void clr_dir_register(uint8_t const value, uint8_t const nano_328_bp, PORT_t * nano_4809_port, uint8_t const nano_4809_bp) {
  if(~value & (1 << nano_328_bp)) nano_4809_port->DIRCLR = (1 << nano_4809_bp);
} 

inline void set_or_clr_dir_register(uint8_t const value, uint8_t const nano_328_bp, PORT_t * nano_4809_port, uint8_t const nano_4809_bp) {
  (value & (1 << nano_328_bp)) ? nano_4809_port->DIRSET = (1 << nano_4809_bp) : nano_4809_port->DIRCLR = (1 << nano_4809_bp); 
}

inline void set_out_register(uint8_t const value, uint8_t const nano_328_bp, PORT_t * nano_4809_port, uint8_t const nano_4809_bp) {
  if(value & (1 << nano_328_bp)) nano_4809_port->OUTSET = (1 << nano_4809_bp);
} 

inline void clr_out_register(uint8_t const value, uint8_t const nano_328_bp, PORT_t * nano_4809_port, uint8_t const nano_4809_bp) {
  if(~value & (1 << nano_328_bp)) nano_4809_port->OUTCLR = (1 << nano_4809_bp);
} 

inline void set_or_clr_out_register(uint8_t const value, uint8_t const nano_328_bp, PORT_t * nano_4809_port, uint8_t const nano_4809_bp) {
  (value & (1 << nano_328_bp)) ? nano_4809_port->OUTSET = (1 << nano_4809_bp) : nano_4809_port->OUTCLR = (1 << nano_4809_bp); 
}

/*****************************************************************************/

DDRBClass::DDRBClass(PORT_t * portb, PORT_t * porte)
:  _portb(portb), 
   _porte(porte) { }

DDRBClass & DDRBClass::operator = (uint8_t const value) {
  set_or_clr_dir_register(value, 0, _porte, 3);
  set_or_clr_dir_register(value, 1, _portb, 0);
  set_or_clr_dir_register(value, 2, _portb, 1);
  set_or_clr_dir_register(value, 3, _porte, 0);
  set_or_clr_dir_register(value, 4, _porte, 1);
  set_or_clr_dir_register(value, 5, _porte, 2);
  return *this;
}

DDRBClass & DDRBClass::operator &= (uint8_t const value) {
  clr_dir_register(value, 0, _porte, 3);
  clr_dir_register(value, 1, _portb, 0);
  clr_dir_register(value, 2, _portb, 1);
  clr_dir_register(value, 3, _porte, 0);
  clr_dir_register(value, 4, _porte, 1);
  clr_dir_register(value, 5, _porte, 2);
  return *this;
}

DDRBClass & DDRBClass::operator |= (uint8_t const value) {
  set_dir_register(value, 0, _porte, 3);
  set_dir_register(value, 1, _portb, 0);
  set_dir_register(value, 2, _portb, 1);
  set_dir_register(value, 3, _porte, 0);
  set_dir_register(value, 4, _porte, 1);
  set_dir_register(value, 5, _porte, 2);
  return *this;
}

PORTBClass::PORTBClass(PORT_t * portb, PORT_t * porte)
:  _portb(portb), 
   _porte(porte) { }

PORTBClass & PORTBClass::operator = (uint8_t const value) {
  set_or_clr_out_register(value, 0, _porte, 3);
  set_or_clr_out_register(value, 1, _portb, 0);
  set_or_clr_out_register(value, 2, _portb, 1);
  set_or_clr_out_register(value, 3, _porte, 0);
  set_or_clr_out_register(value, 4, _porte, 1);
  set_or_clr_out_register(value, 5, _porte, 2);
  return *this;
}

PORTBClass & PORTBClass::operator &= (uint8_t const value) {
  clr_out_register(value, 0, _porte, 3);
  clr_out_register(value, 1, _portb, 0);
  clr_out_register(value, 2, _portb, 1);
  clr_out_register(value, 3, _porte, 0);
  clr_out_register(value, 4, _porte, 1);
  clr_out_register(value, 5, _porte, 2);
  return *this;
}

PORTBClass & PORTBClass::operator |= (uint8_t const value) {
  set_out_register(value, 0, _porte, 3);
  set_out_register(value, 1, _portb, 0);
  set_out_register(value, 2, _portb, 1);
  set_out_register(value, 3, _porte, 0);
  set_out_register(value, 4, _porte, 1);
  set_out_register(value, 5, _porte, 2);
  return *this;
}

/*****************************************************************************/

DDRCClass::DDRCClass(PORT_t * porta, PORT_t * portd)
: _porta(porta),
  _portd(portd) { }

DDRCClass & DDRCClass::operator = (uint8_t const value) {
  set_or_clr_dir_register(value, 0, _portd, 3);
  set_or_clr_dir_register(value, 1, _portd, 2);
  set_or_clr_dir_register(value, 2, _portd, 1);
  set_or_clr_dir_register(value, 3, _portd, 0);
  set_or_clr_dir_register(value, 4, _porta, 2);
  set_or_clr_dir_register(value, 5, _porta, 3);
  set_or_clr_dir_register(value, 6, _portd, 4);
  set_or_clr_dir_register(value, 7, _portd, 5);
  return *this;
}

DDRCClass & DDRCClass::operator &= (uint8_t const value) {
  clr_dir_register(value, 0, _portd, 3);
  clr_dir_register(value, 1, _portd, 2);
  clr_dir_register(value, 2, _portd, 1);
  clr_dir_register(value, 3, _portd, 0);
  clr_dir_register(value, 4, _porta, 2);
  clr_dir_register(value, 5, _porta, 3);
  clr_dir_register(value, 6, _portd, 4);
  clr_dir_register(value, 7, _portd, 5);
  return *this;
}

DDRCClass & DDRCClass::operator |= (uint8_t const value) {
  set_dir_register(value, 0, _portd, 3);
  set_dir_register(value, 1, _portd, 2);
  set_dir_register(value, 2, _portd, 1);
  set_dir_register(value, 3, _portd, 0);
  set_dir_register(value, 4, _porta, 2);
  set_dir_register(value, 5, _porta, 3);
  set_dir_register(value, 6, _portd, 4);
  set_dir_register(value, 7, _portd, 5);
  return *this;
}

PORTCClass::PORTCClass(PORT_t * porta, PORT_t * portd)
: _porta(porta),
  _portd(portd) { }

PORTCClass & PORTCClass::operator = (uint8_t const value) {
  set_or_clr_out_register(value, 0, _portd, 3);
  set_or_clr_out_register(value, 1, _portd, 2);
  set_or_clr_out_register(value, 2, _portd, 1);
  set_or_clr_out_register(value, 3, _portd, 0);
  set_or_clr_out_register(value, 4, _porta, 2);
  set_or_clr_out_register(value, 5, _porta, 3);
  set_or_clr_out_register(value, 6, _portd, 4);
  set_or_clr_out_register(value, 7, _portd, 5);
  return *this;
}

PORTCClass & PORTCClass::operator &= (uint8_t const value) {
  clr_out_register(value, 0, _portd, 3);
  clr_out_register(value, 1, _portd, 2);
  clr_out_register(value, 2, _portd, 1);
  clr_out_register(value, 3, _portd, 0);
  clr_out_register(value, 4, _porta, 2);
  clr_out_register(value, 5, _porta, 3);
  clr_out_register(value, 6, _portd, 4);
  clr_out_register(value, 7, _portd, 5);
  return *this;
}

PORTCClass & PORTCClass::operator |= (uint8_t const value) {
  set_out_register(value, 0, _portd, 3);
  set_out_register(value, 1, _portd, 2);
  set_out_register(value, 2, _portd, 1);
  set_out_register(value, 3, _portd, 0);
  set_out_register(value, 4, _porta, 2);
  set_out_register(value, 5, _porta, 3);
  set_out_register(value, 6, _portd, 4);
  set_out_register(value, 7, _portd, 5);
  return *this;
}

/*****************************************************************************/

DDRDClass::DDRDClass(PORT_t * porta, PORT_t * portb, PORT_t * portc, PORT_t * portf)
: _porta(porta),
  _portb(portb),
  _portc(portc),
  _portf(portf) { }

DDRDClass & DDRDClass::operator = (uint8_t const value) {
  set_or_clr_dir_register(value, 0, _portc, 4);
  set_or_clr_dir_register(value, 1, _portc, 5);
  set_or_clr_dir_register(value, 2, _porta, 0);
  set_or_clr_dir_register(value, 3, _portf, 5);
  set_or_clr_dir_register(value, 4, _portc, 6);
  set_or_clr_dir_register(value, 5, _portb, 2);
  set_or_clr_dir_register(value, 6, _portf, 4);
  set_or_clr_dir_register(value, 7, _porta, 1);
  return *this;
}

DDRDClass & DDRDClass::operator &= (uint8_t const value) {
  clr_dir_register(value, 0, _portc, 4);
  clr_dir_register(value, 1, _portc, 5);
  clr_dir_register(value, 2, _porta, 0);
  clr_dir_register(value, 3, _portf, 5);
  clr_dir_register(value, 4, _portc, 6);
  clr_dir_register(value, 5, _portb, 2);
  clr_dir_register(value, 6, _portf, 4);
  clr_dir_register(value, 7, _porta, 1);
  return *this;
}

DDRDClass & DDRDClass::operator |= (uint8_t const value) {
  set_dir_register(value, 0, _portc, 4);
  set_dir_register(value, 1, _portc, 5);
  set_dir_register(value, 2, _porta, 0);
  set_dir_register(value, 3, _portf, 5);
  set_dir_register(value, 4, _portc, 6);
  set_dir_register(value, 5, _portb, 2);
  set_dir_register(value, 6, _portf, 4);
  set_dir_register(value, 7, _porta, 1);
  return *this;
}

PORTDClass::PORTDClass(PORT_t * porta, PORT_t * portb, PORT_t * portc, PORT_t * portf)
: _porta(porta),
  _portb(portb),
  _portc(portc),
  _portf(portf) { }

PORTDClass & PORTDClass::operator = (uint8_t const value) {
  set_or_clr_out_register(value, 0, _portc, 4);
  set_or_clr_out_register(value, 1, _portc, 5);
  set_or_clr_out_register(value, 2, _porta, 0);
  set_or_clr_out_register(value, 3, _portf, 5);
  set_or_clr_out_register(value, 4, _portc, 6);
  set_or_clr_out_register(value, 5, _portb, 2);
  set_or_clr_out_register(value, 6, _portf, 4);
  set_or_clr_out_register(value, 7, _porta, 1);
  return *this;
}

PORTDClass & PORTDClass::operator &= (uint8_t const value) {
  clr_out_register(value, 0, _portc, 4);
  clr_out_register(value, 1, _portc, 5);
  clr_out_register(value, 2, _porta, 0);
  clr_out_register(value, 3, _portf, 5);
  clr_out_register(value, 4, _portc, 6);
  clr_out_register(value, 5, _portb, 2);
  clr_out_register(value, 6, _portf, 4);
  clr_out_register(value, 7, _porta, 1);
  return *this;
}

PORTDClass & PORTDClass::operator |= (uint8_t const value) {
  set_out_register(value, 0, _portc, 4);
  set_out_register(value, 1, _portc, 5);
  set_out_register(value, 2, _porta, 0);
  set_out_register(value, 3, _portf, 5);
  set_out_register(value, 4, _portc, 6);
  set_out_register(value, 5, _portb, 2);
  set_out_register(value, 6, _portf, 4);
  set_out_register(value, 7, _porta, 1);
  return *this;
}

#endif /* #ifdef AVR_NANO_4809_328MODE */
