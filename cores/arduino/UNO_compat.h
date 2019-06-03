/*
  Copyright (c) 2018 Arduino Team.  All right reserved.
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
*/

#pragma once
#include "Arduino.h"

#ifdef UNO_WIFI_REV2_328MODE

/*
  ARDUINO PIN  ATMEGA 328  ATMEGA 4809
  0            PD0         PC5
  1            PD1         PC4
  2            PD2         PA0
  3            PD3         PF5
  4            PD4         PC6
  5            PD5         PB2
  6            PD6         PF4
  7            PD7         PA1
  8            PB0         PE3
  9            PB1         PB0
  10           PB2         PB1
  11           PB3         PE0
  12           PB4         PE1
  13           PB5         PE2
  A0           PC0         PD0
  A1           PC1         PD1
  A2           PC2         PD2
  A3           PC3         PD3
  A4           PC4         PD4
  A5           PC5         PD5
*/

#define PORTA_ARDUINO        (*(PORT_t *) 0x0400) /* I/O Ports */
#define PORTB_ARDUINO        (*(PORT_t *) 0x0420) /* I/O Ports */
#define PORTC_ARDUINO        (*(PORT_t *) 0x0440) /* I/O Ports */
#define PORTD_ARDUINO        (*(PORT_t *) 0x0460) /* I/O Ports */
#define PORTE_ARDUINO        (*(PORT_t *) 0x0480) /* I/O Ports */
#define PORTF_ARDUINO        (*(PORT_t *) 0x04A0) /* I/O Ports */

#undef PORTB
#undef PORTC
#undef PORTD

#define SET_PORT_REGISTER(inPosition, port, position) if ((1 << inPosition) & (uint8_t)value) { port.OUTSET = (1 << position);}
#define CLEAR_PORT_REGISTER(inPosition, port, position) if ((uint8_t)~(1 << inPosition) == (uint8_t)value) { port.OUTCLR = (1 << position);}
#define SET_OR_CLEAR_PORT_REGISTER(inPosition, port, position) if ((uint8_t)~(1 << inPosition) & (uint8_t)value) { port.OUTSET = (1 << position); } else {port.OUTCLR = (1 << position); }

#define SET_DIR_REGISTER(inPosition, port, position) if ((1 << inPosition) & (uint8_t)value) { port.DIRSET = (1 << position);}
#define CLEAR_DIR_REGISTER(inPosition, port, position) if ((uint8_t)~(1 << inPosition) == (uint8_t)value) { port.DIRCLR = (1 << position);}
#define SET_OR_CLEAR_DIR_REGISTER(inPosition, port, position) if ((uint8_t)~(1 << inPosition) & (uint8_t)value) { port.DIRSET = (1 << position); } else {port.DIRCLR = (1 << position); }


/** DDR Classes**/
class DDRBClass {
  public:
    DDRBClass() {}
    DDRBClass& operator=(uint8_t value) {
      SET_OR_CLEAR_DIR_REGISTER(0, PORTE_ARDUINO, 3);
      SET_OR_CLEAR_DIR_REGISTER(1, PORTB_ARDUINO, 0);
      SET_OR_CLEAR_DIR_REGISTER(2, PORTB_ARDUINO, 1);
      SET_OR_CLEAR_DIR_REGISTER(3, PORTE_ARDUINO, 0);
      SET_OR_CLEAR_DIR_REGISTER(4, PORTE_ARDUINO, 1);
      SET_OR_CLEAR_DIR_REGISTER(5, PORTE_ARDUINO, 2);
      return *this;
    }

    DDRBClass& operator&=(uint8_t value) {
      CLEAR_DIR_REGISTER(0, PORTE_ARDUINO, 3);
      CLEAR_DIR_REGISTER(1, PORTB_ARDUINO, 0);
      CLEAR_DIR_REGISTER(2, PORTB_ARDUINO, 1);
      CLEAR_DIR_REGISTER(3, PORTE_ARDUINO, 0);
      CLEAR_DIR_REGISTER(4, PORTE_ARDUINO, 1);
      CLEAR_DIR_REGISTER(5, PORTE_ARDUINO, 2);
      return *this;
    }

    DDRBClass& operator|=(uint8_t value) {
      SET_DIR_REGISTER(0, PORTE_ARDUINO, 3);
      SET_DIR_REGISTER(1, PORTB_ARDUINO, 0);
      SET_DIR_REGISTER(2, PORTB_ARDUINO, 1);
      SET_DIR_REGISTER(3, PORTE_ARDUINO, 0);
      SET_DIR_REGISTER(4, PORTE_ARDUINO, 1);
      SET_DIR_REGISTER(5, PORTE_ARDUINO, 2);
      return *this;
    }
};

class DDRCClass {
  public:
    DDRCClass() {}
    DDRCClass& operator=(uint8_t value) {
      SET_OR_CLEAR_DIR_REGISTER(0, PORTD_ARDUINO, 0);
      SET_OR_CLEAR_DIR_REGISTER(1, PORTD_ARDUINO, 1);
      SET_OR_CLEAR_DIR_REGISTER(2, PORTD_ARDUINO, 2);
      SET_OR_CLEAR_DIR_REGISTER(3, PORTD_ARDUINO, 3);
      SET_OR_CLEAR_DIR_REGISTER(4, PORTD_ARDUINO, 4);
      SET_OR_CLEAR_DIR_REGISTER(5, PORTD_ARDUINO, 5);
      return *this;
    }

    DDRCClass& operator&=(uint8_t value) {
      CLEAR_DIR_REGISTER(0, PORTD_ARDUINO, 0);
      CLEAR_DIR_REGISTER(1, PORTD_ARDUINO, 1);
      CLEAR_DIR_REGISTER(2, PORTD_ARDUINO, 2);
      CLEAR_DIR_REGISTER(3, PORTD_ARDUINO, 3);
      CLEAR_DIR_REGISTER(4, PORTD_ARDUINO, 4);
      CLEAR_DIR_REGISTER(5, PORTD_ARDUINO, 5);
      return *this;
    }

    DDRCClass& operator|=(uint8_t value) {
      SET_DIR_REGISTER(0, PORTD_ARDUINO, 0);
      SET_DIR_REGISTER(1, PORTD_ARDUINO, 1);
      SET_DIR_REGISTER(2, PORTD_ARDUINO, 2);
      SET_DIR_REGISTER(3, PORTD_ARDUINO, 3);
      SET_DIR_REGISTER(4, PORTD_ARDUINO, 4);
      SET_DIR_REGISTER(5, PORTD_ARDUINO, 5);
      return *this;
    }
};

class DDRDClass {
  public:
    DDRDClass() {}
    DDRDClass& operator=(uint8_t value) {
      SET_OR_CLEAR_DIR_REGISTER(0, PORTC_ARDUINO, 5);
      SET_OR_CLEAR_DIR_REGISTER(1, PORTC_ARDUINO, 4);
      SET_OR_CLEAR_DIR_REGISTER(2, PORTA_ARDUINO, 0);
      SET_OR_CLEAR_DIR_REGISTER(3, PORTF_ARDUINO, 5);
      SET_OR_CLEAR_DIR_REGISTER(4, PORTC_ARDUINO, 6);
      SET_OR_CLEAR_DIR_REGISTER(5, PORTB_ARDUINO, 2);
      SET_OR_CLEAR_DIR_REGISTER(6, PORTF_ARDUINO, 4);
      SET_OR_CLEAR_DIR_REGISTER(7, PORTA_ARDUINO, 1);
      return *this;
    }

    DDRDClass& operator&=(uint8_t value) {
      CLEAR_DIR_REGISTER(0, PORTC_ARDUINO, 5);
      CLEAR_DIR_REGISTER(1, PORTC_ARDUINO, 4);
      CLEAR_DIR_REGISTER(2, PORTA_ARDUINO, 0);
      CLEAR_DIR_REGISTER(3, PORTF_ARDUINO, 5);
      CLEAR_DIR_REGISTER(4, PORTC_ARDUINO, 6);
      CLEAR_DIR_REGISTER(5, PORTB_ARDUINO, 2);
      CLEAR_DIR_REGISTER(6, PORTF_ARDUINO, 4);
      CLEAR_DIR_REGISTER(7, PORTA_ARDUINO, 1);
      return *this;
    }

    DDRDClass& operator|=(uint8_t value) {
      SET_DIR_REGISTER(0, PORTC_ARDUINO, 5);
      SET_DIR_REGISTER(1, PORTC_ARDUINO, 4);
      SET_DIR_REGISTER(2, PORTA_ARDUINO, 0);
      SET_DIR_REGISTER(3, PORTF_ARDUINO, 5);
      SET_DIR_REGISTER(4, PORTC_ARDUINO, 6);
      SET_DIR_REGISTER(5, PORTB_ARDUINO, 2);
      SET_DIR_REGISTER(6, PORTF_ARDUINO, 4);
      SET_DIR_REGISTER(7, PORTA_ARDUINO, 1);
      return *this;
    }
};
/** PORT Classes**/
class PORTBClass {
  public:
    PORTBClass() {}
    PORTBClass& operator=(uint8_t value) {
      SET_OR_CLEAR_PORT_REGISTER(0, PORTE_ARDUINO, 3);
      SET_OR_CLEAR_PORT_REGISTER(1, PORTB_ARDUINO, 0);
      SET_OR_CLEAR_PORT_REGISTER(2, PORTB_ARDUINO, 1);
      SET_OR_CLEAR_PORT_REGISTER(3, PORTE_ARDUINO, 0);
      SET_OR_CLEAR_PORT_REGISTER(4, PORTE_ARDUINO, 1);
      SET_OR_CLEAR_PORT_REGISTER(5, PORTE_ARDUINO, 2);
      return *this;
    }

    PORTBClass& operator&=(uint8_t value) {
      CLEAR_PORT_REGISTER(0, PORTE_ARDUINO, 3);
      CLEAR_PORT_REGISTER(1, PORTB_ARDUINO, 0);
      CLEAR_PORT_REGISTER(2, PORTB_ARDUINO, 1);
      CLEAR_PORT_REGISTER(3, PORTE_ARDUINO, 0);
      CLEAR_PORT_REGISTER(4, PORTE_ARDUINO, 1);
      CLEAR_PORT_REGISTER(5, PORTE_ARDUINO, 2);
      return *this;
    }

    PORTBClass& operator|=(uint8_t value) {
      SET_PORT_REGISTER(0, PORTE_ARDUINO, 3);
      SET_PORT_REGISTER(1, PORTB_ARDUINO, 0);
      SET_PORT_REGISTER(2, PORTB_ARDUINO, 1);
      SET_PORT_REGISTER(3, PORTE_ARDUINO, 0);
      SET_PORT_REGISTER(4, PORTE_ARDUINO, 1);
      SET_PORT_REGISTER(5, PORTE_ARDUINO, 2);
      return *this;
    }
};

class PORTCClass {
  public:
    PORTCClass() {}
    PORTCClass& operator=(uint8_t value) {
      SET_OR_CLEAR_PORT_REGISTER(0, PORTD_ARDUINO, 0);
      SET_OR_CLEAR_PORT_REGISTER(1, PORTD_ARDUINO, 1);
      SET_OR_CLEAR_PORT_REGISTER(2, PORTD_ARDUINO, 2);
      SET_OR_CLEAR_PORT_REGISTER(3, PORTD_ARDUINO, 3);
      SET_OR_CLEAR_PORT_REGISTER(4, PORTD_ARDUINO, 4);
      SET_OR_CLEAR_PORT_REGISTER(5, PORTD_ARDUINO, 5);
      return *this;
    }

    PORTCClass& operator&=(uint8_t value) {
      CLEAR_PORT_REGISTER(0, PORTD_ARDUINO, 0);
      CLEAR_PORT_REGISTER(1, PORTD_ARDUINO, 1);
      CLEAR_PORT_REGISTER(2, PORTD_ARDUINO, 2);
      CLEAR_PORT_REGISTER(3, PORTD_ARDUINO, 3);
      CLEAR_PORT_REGISTER(4, PORTD_ARDUINO, 4);
      CLEAR_PORT_REGISTER(5, PORTD_ARDUINO, 5);
      return *this;
    }

    PORTCClass& operator|=(uint8_t value) {
      SET_PORT_REGISTER(0, PORTD_ARDUINO, 0);
      SET_PORT_REGISTER(1, PORTD_ARDUINO, 1);
      SET_PORT_REGISTER(2, PORTD_ARDUINO, 2);
      SET_PORT_REGISTER(3, PORTD_ARDUINO, 3);
      SET_PORT_REGISTER(4, PORTD_ARDUINO, 4);
      SET_PORT_REGISTER(5, PORTD_ARDUINO, 5);
      return *this;
    }
};

class PORTDClass {
  public:
    PORTDClass() {}
    PORTDClass& operator=(uint8_t value) {
      SET_OR_CLEAR_PORT_REGISTER(0, PORTC_ARDUINO, 5);
      SET_OR_CLEAR_PORT_REGISTER(1, PORTC_ARDUINO, 4);
      SET_OR_CLEAR_PORT_REGISTER(2, PORTA_ARDUINO, 0);
      SET_OR_CLEAR_PORT_REGISTER(3, PORTF_ARDUINO, 5);
      SET_OR_CLEAR_PORT_REGISTER(4, PORTC_ARDUINO, 6);
      SET_OR_CLEAR_PORT_REGISTER(5, PORTB_ARDUINO, 2);
      SET_OR_CLEAR_PORT_REGISTER(6, PORTF_ARDUINO, 4);
      SET_OR_CLEAR_PORT_REGISTER(7, PORTA_ARDUINO, 1);
      return *this;
    }

    PORTDClass& operator&=(uint8_t value) {
      CLEAR_PORT_REGISTER(0, PORTC_ARDUINO, 5);
      CLEAR_PORT_REGISTER(1, PORTC_ARDUINO, 4);
      CLEAR_PORT_REGISTER(2, PORTA_ARDUINO, 0);
      CLEAR_PORT_REGISTER(3, PORTF_ARDUINO, 5);
      CLEAR_PORT_REGISTER(4, PORTC_ARDUINO, 6);
      CLEAR_PORT_REGISTER(5, PORTB_ARDUINO, 2);
      CLEAR_PORT_REGISTER(6, PORTF_ARDUINO, 4);
      CLEAR_PORT_REGISTER(7, PORTA_ARDUINO, 1);
      return *this;
    }

    PORTDClass& operator|=(uint8_t value) {
      SET_PORT_REGISTER(0, PORTC_ARDUINO, 5);
      SET_PORT_REGISTER(1, PORTC_ARDUINO, 4);
      SET_PORT_REGISTER(2, PORTA_ARDUINO, 0);
      SET_PORT_REGISTER(3, PORTF_ARDUINO, 5);
      SET_PORT_REGISTER(4, PORTC_ARDUINO, 6);
      SET_PORT_REGISTER(5, PORTB_ARDUINO, 2);
      SET_PORT_REGISTER(6, PORTF_ARDUINO, 4);
      SET_PORT_REGISTER(7, PORTA_ARDUINO, 1);
      return *this;
    }
};

extern PORTBClass PORTB;
extern PORTCClass PORTC;
extern PORTDClass PORTD;
extern DDRBClass DDRB;
extern DDRCClass DDRC;
extern DDRDClass DDRD;

#endif /* #ifdef UNO_WIFI_REV2_328MODE */