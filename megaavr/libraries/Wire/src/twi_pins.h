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
  Modified 2021-2022 by MX682X for the Wire library rewriting project
*/

#ifndef TWI_PINS_H
#define TWI_PINS_H

#include <Arduino.h>

#include "avr/io.h"

//If a core provides badArg() and badCall(), it should define CORE_HAS_ERRORFUNS to = 1.
//otherwise we need to add them in here - this is for ease of using with MegaCoreX
#if !defined(CORE_HAS_ERRORFUNS)
  void badArg(const char*)    __attribute__((error(""))); //used when an argument is known at compile time to be an invalid value
  // but the function being called is valid to call at this time.
  void badCall(const char*)   __attribute__((error(""))); //used when it is known at compile time that a whole function cannot be called,
  // regardless of what arguments it is passed! Typically this is things like timekeeping when millis is disabled, and functions that
  // only work on some parts - things like TWI1_swap() which has only 1 option on some parts.
#endif

void    TWI0_ClearPins();
bool    TWI0_Pins(uint8_t sda_pin, uint8_t scl_pin);
bool    TWI0_swap(uint8_t state);
void    TWI0_usePullups();
uint8_t TWI0_checkPinLevel();
#if defined(TWI0_DUALCTRL)
  uint8_t TWI0_setConfig(bool smbuslvl, bool longsetup, uint8_t sda_hold, bool smbuslvl_dual, uint8_t sda_hold_dual);
#else
  uint8_t TWI0_setConfig(bool longsetup, uint8_t sda_hold);
#endif

#if defined (TWI1)
  void    TWI1_ClearPins();
  bool    TWI1_Pins(uint8_t sda_pin, uint8_t scl_pin);
  bool    TWI1_swap(uint8_t state);
  void    TWI1_usePullups();
  uint8_t TWI1_checkPinLevel();
  //  Much of this is commented out because nothing with a TWI1 doesn't have SMBus levels
  //  #if defined(TWI1_DUALCTRL)
  uint8_t TWI1_setConfig(bool smbuslvl, bool longsetup, uint8_t sda_hold, bool smbuslvl_dual, uint8_t sda_hold_dual);
  //  #else
  //    uint8_t TWI1_setConfig(bool longsetup, uint8_t sda_hold);
  //  #endif
#endif
#endif /* TWI_PINS_H */
