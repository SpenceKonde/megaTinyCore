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

#include "UNO_compat.h"

#ifdef UNO_WIFI_REV2_328MODE

#warning "ATMEGA328 registers emulation is enabled. You may encounter some speed issue. Please consider to disable it in the Tools menu"

PORTBClass PORTB;
PORTCClass PORTC;
PORTDClass PORTD;

DDRBClass DDRB;
DDRCClass DDRC;
DDRDClass DDRD;

#endif /* #ifdef UNO_WIFI_REV2_328MODE */