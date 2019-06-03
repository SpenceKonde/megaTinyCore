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

#pragma once
#include <Arduino.h>

#ifdef AVR_NANO_4809_328MODE

/*
  ARDUINO PIN  ATMEGA 328  ATMEGA 4809
  D0           PD0         PC4 (TX1)
  D1           PD1         PC5 (RX1)
  D2           PD2         PA0
  D3           PD3         PF5
  D4           PD4         PC6
  D5           PD5         PB2
  D6           PD6         PF4
  D7           PD7         PA1
  D8           PB0         PE3
  D9           PB1         PB0
  D10          PB2         PB1
  D11          PB3         PE0 (MOSI)
  D12          PB4         PE1 (MISO)
  D13          PB5         PE2 (SCK)
  A0           PC0         PD3
  A1           PC1         PD2
  A2           PC2         PD1
  A3           PC3         PD0
  A4           PC4         PA2 (SDA)
  A5           PC5         PA3 (SCL)
  A6           ADC6        PD4
  A7           ADC7        PD5
*/

class DDRBClass {
  public:
    DDRBClass(PORT_t * portb, PORT_t * porte);

    DDRBClass & operator  = (uint8_t const value);
    DDRBClass & operator &= (uint8_t const value);
    DDRBClass & operator |= (uint8_t const value);
  private: 
    PORT_t * _portb , * _porte;   
};

class PORTBClass {
  public:
    PORTBClass(PORT_t * portb, PORT_t * porte);

    PORTBClass & operator  = (uint8_t const value);
    PORTBClass & operator &= (uint8_t const value);
    PORTBClass & operator |= (uint8_t const value);
  private: 
    PORT_t * _portb , * _porte;   
};

/*****************************************************************************/

class DDRCClass {
  public:
    DDRCClass(PORT_t * porta, PORT_t * portd);

    DDRCClass & operator  = (uint8_t const value);
    DDRCClass & operator &= (uint8_t const value);
    DDRCClass & operator |= (uint8_t const value);
  private: 
    PORT_t * _porta , * _portd;   
};

class PORTCClass {
  public:
    PORTCClass(PORT_t * porta, PORT_t * portd);

    PORTCClass & operator  = (uint8_t const value);
    PORTCClass & operator &= (uint8_t const value);
    PORTCClass & operator |= (uint8_t const value);
  private: 
    PORT_t * _porta , * _portd;   
};

/*****************************************************************************/

class DDRDClass {
  public:
    DDRDClass(PORT_t * porta, PORT_t * portb, PORT_t * portc, PORT_t * portf);

    DDRDClass & operator  = (uint8_t const value);
    DDRDClass & operator &= (uint8_t const value);
    DDRDClass & operator |= (uint8_t const value);
  private: 
    PORT_t * _porta, * _portb , * _portc, * _portf;   
};

class PORTDClass {
  public:
    PORTDClass(PORT_t * porta, PORT_t * portb, PORT_t * portc, PORT_t * portf);

    PORTDClass & operator  = (uint8_t const value);
    PORTDClass & operator &= (uint8_t const value);
    PORTDClass & operator |= (uint8_t const value);
  private: 
    PORT_t * _porta, * _portb , * _portc, * _portf;   
};

/*****************************************************************************/

#ifndef HOST_BUILD
  #undef DDRB
  #undef PORTB
  #undef DDRC
  #undef PORTC
  #undef DDRD
  #undef PORTD

  extern DDRBClass  DDRB;
  extern PORTBClass PORTB;
  extern DDRCClass  DDRC;
  extern PORTCClass PORTC;
  extern DDRDClass  DDRD;
  extern PORTDClass PORTD;
#endif /* #ifndef HOST_BUILD */

#endif /* #ifdef AVR_NANO_4809_328MODE */