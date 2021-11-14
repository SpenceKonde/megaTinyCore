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

#ifndef TWI_PINS_H
#define TWI_PINS_H

#include <Arduino.h>

#include "avr/io.h"


void   TWI0_ClearPins();
bool   TWI0_Pins(uint8_t sda_pin, uint8_t scl_pin);
bool   TWI0_swap(uint8_t state);
void   TWI0_usePullups();


#if defined (TWI1)
  void   TWI1_ClearPins();
  bool   TWI1_Pins(uint8_t sda_pin, uint8_t scl_pin);
  bool   TWI1_swap(uint8_t state);
  void   TWI1_usePullups();
#endif

#endif /* TWI_DRIVER_H */
