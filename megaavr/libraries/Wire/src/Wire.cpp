/*
  TwoWire.cpp - TWI/I2C library for Wiring & Arduino
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
  Modified extensively 2021-23 by MX682X for megaTinyCore and DxCore.
  Added Support for Simultaneous host/client, dual mode and Wire1.
*/
// *INDENT-OFF*   astyle wants this file to be completely unreadable with no indentation for the many preprocessor conditionals!
extern "C" {
  #include <stdlib.h>
  #include <string.h>
  #include <inttypes.h>
}


#include "Arduino.h"
#include "Wire.h"


extern "C" {    // compiler was complaining when I put twi.h into the upper C include part
  #include "twi_pins.h"
}

volatile uint8_t sleepStack = 0;

void pauseDeepSleep(uint8_t moduelAddr);
void restoreSleep(uint8_t moduelAddr);

TwoWire* twi0_wire;
TwoWire* twi1_wire;



/**
 *@brief      TwoWire creates a Wire object
 *
 *@param      TWI_t *module - the pointer to the TWI module that the Wire object is supposed to use
 *
 *@return     constructor can't return anything
 */

TwoWire::TwoWire(TWI_t *twi_module) {
  if (&TWI0 == twi_module) {
    twi0_wire = this;
  }
  #if defined(TWI1)
    else if (&TWI1 == twi_module) {
      twi1_wire = this;
    }
  #endif
  else {    // ignore NULLs
    return;
  }
  _module = twi_module;
}

/**
 *@brief      pins changes the PINMUX to correspond to the desired pins
 *
 *
 *@param      uint8_t sda_pin is the desired pin for SDA
 *@param      uint8_t scl_pin is the desired pin for SCL
 *
 *@return     bool
 *@retval     true if change was successful
 */
bool TwoWire::pins(uint8_t sda_pin, uint8_t scl_pin) {
  if (__builtin_constant_p(sda_pin) && __builtin_constant_p(scl_pin)) {
    if (!(
      #if defined(PIN_WIRE_SDA)
          (sda_pin == PIN_WIRE_SDA && scl_pin == PIN_WIRE_SCL) ||
      #endif
      #if defined(PIN_WIRE_SDA_PINSWAP_1)
          (sda_pin == PIN_WIRE_SDA_PINSWAP_1 && scl_pin == PIN_WIRE_SCL_PINSWAP_1) ||
      #endif
      #if defined(PIN_WIRE_SDA_PINSWAP_2)
          (sda_pin == PIN_WIRE_SDA_PINSWAP_2 && scl_pin == PIN_WIRE_SCL_PINSWAP_2) ||
      #endif
      #if defined(PIN_WIRE_SDA_PINSWAP_3)
          (sda_pin == PIN_WIRE_SDA_PINSWAP_3 && scl_pin == PIN_WIRE_SCL_PINSWAP_3) ||
      #endif
      #if defined(PIN_WIRE1_SDA)
          (sda_pin == PIN_WIRE1_SDA && scl_pin == PIN_WIRE1_SCL) ||
      #endif
      #if defined(PIN_WIRE1_SDA_PINSWAP_1)
          (sda_pin == PIN_WIRE1_SDA_PINSWAP_1 && scl_pin == PIN_WIRE1_SCL_PINSWAP_1) ||
      #endif
      #if defined(PIN_WIRE1_SDA_PINSWAP_2)
          (sda_pin == PIN_WIRE1_SDA_PINSWAP_2 && scl_pin == PIN_WIRE1_SCL_PINSWAP_2) ||
      #endif
        false)) { // SDA and SCL are always next to each other
      #if defined(TWI1)
        badArg("Pins passed to Wire.pins() or Wire1.pins() known at compile time to be invalid");
      #else
        badArg("Pins passed to Wire.pins() known at compile time to be invalid");
      #endif
    }
  }
  /* The above blob of code is entirely removed by the compiler - it either generates the compile error or doesn't, and nothing is evaluated at runtime
   * This was for a time in twi_pins.c - but that prevented it from ever firing, because constant folding doesn't happen if it's in a different file
   * and hence a different compilation unit. This still will only catch the case where the pins are not a valid pin combination. The construction below, I think
   * is enough to render the optimizer unable to do the sort of things it needs to in order to do this.
   * Using both Wire.pins() and Wire1.pins() within the same sketch may also be enough to defang the optimizer such that it can't check these.
   * I don't see a way around that, though... but this will at least catch the majority of cases. -SK 10/5/22.
   */
  #if defined(TWI1)
    if (&TWI0 == _module)  {
      return TWI0_Pins(sda_pin, scl_pin);
    } else if (&TWI1 == _module)  {
      return TWI1_Pins(sda_pin, scl_pin);
    } else {
      return false;
    }
  #else
    return TWI0_Pins(sda_pin, scl_pin);
  #endif
}


/**
 *@brief      swap works like pins, but is smaller
 *
 *
 *@param      uint8_t state is the desired value for the PINMUX register
 *
 *@return     bool
 *@retval     true if change was successful
 */
bool TwoWire::swap(uint8_t state) {
  if (__builtin_constant_p(state)) {
    #if defined(TWI1) // DA and DB have a second TwI, TWI1. Both have swap levels 0-2. Swap 1 and 2 of TWI1 and swap 1 of TWI0 are unavailable with under 48 pins.
      if (!(
        #if defined(PIN_WIRE1_SDA_PINSWAP_2) || defined(PIN_WIRE_SDA_PINSWAP_2)
          state == 2 ||
        #endif
        #if defined(PIN_WIRE1_SDA_PINSWAP_1) || defined(PIN_WIRE_SDA_PINSWAP_1)
          state == 1 ||
        #endif
        #if defined(PIN_WIRE_SDA) || defined(PIN_WIRE1_SDA)
          state == 0 ||
        #endif
        false)) {
          if (state > 3) {
            badArg("The requested swap level is known at compile time to be one that is not available on any part. (did you pass a bitmask instead of a number?)");
          } else if (state == 3){
            badArg("Swap level 3 is not available on DA or DB devices, only swaps 0, 1 and 2.");
          } else {
            badArg("The requested swap level is not available on this part.");
            return false;
          }
        }

    #elif defined(TWI0) // case for one TWI, TWI0
      if (!(
        #if defined(PIN_WIRE_SDA_PINSWAP_3)
          state == 3 ||
        #endif
        #if defined(PIN_WIRE_SDA_PINSWAP_2)
          state == 2 ||
        #endif
        #if defined(PIN_WIRE_SDA_PINSWAP_1)
          state == 1 ||
        #endif
        #if defined(PIN_WIRE_SDA)
          state == 0 ||
        #endif
        false)) {
          if (state > 3) {
            badArg("The requested swap level is known at compile time to be one that is not available on any part. (did you pass a bitmask instead of a number?)");
          } else {
            badArg("The requested swap level is not available on this part.");
            return false;
          }
        }

    #else
      #error "This library (Wire.h) is for modern AVRs, and all parts announced or in production have a TWI0"
    #endif
  }
  #if defined(TWI1)
    if (&TWI0 == _module) {
      return TWI0_swap(state);
    } else if (&TWI1 == _module) {
      return TWI1_swap(state);
    } else {
      return false;
    }
  #else
    return TWI0_swap(state);
  #endif
}

/**
 *@brief      usePullups enables the PULL-UP on the TWI pins.
 *
 *            This function is using the PORTMUX value, so use it after swap()!
 *
 *
 *@param      void
 *
 *@return     void
 */
void TwoWire::usePullups(void) {
  #if defined(TWI1)
    if (&TWI0 == _module) {
      TWI0_usePullups();
    } else if (&TWI1 == _module) {
      TWI1_usePullups();
    }
  #else
    TWI0_usePullups();
  #endif
}


/**
 *@brief      swapModule changes the TWI module if only one Wire object is used
 *
 *            Works only if the TWI is disabled
 *
 *
 *@param      TWI_t *module - the new pointer to a TWI module for this object
 *
 *@return     bool
 *@retval     true if change was successful
 */
bool TwoWire::swapModule(TWI_t *twi_module) {
  if (__builtin_constant_p(twi_module)) {
    if (twi_module == NULL) {
      badArg("Null Pointer passed");
      return false;
    }
  }
  #if defined(TWI1)
    #if defined(TWI_USING_WIRE1)
      badCall("swapModule() can only be used if Wire1 is not used");
    #else
      if ((_bools._hostEnabled | _bools._clientEnabled) == 0) { // make sure nothing's enabled
        if (&TWI0 == twi_module) {
          twi0_wire = this;
          _module = twi_module;
          return true;
        }
        else if (&TWI1 == twi_module) {
          twi1_wire = this;
          _module = twi_module;
          return true;
        }
      }
    #endif
  #else
    badCall("Only one TWI module available, nothing to switch with");
  #endif
  return false;
}


/**
 *@brief      begin (w/o parameters) starts initializes the host operation of the TWI
 *
 *@param      void
 *
 *@return     void
 */
void TwoWire::begin(void) {
  #if defined(TWI_MANDS)                            // Check if the user wants to use Master AND Slave
    if (_bools._hostEnabled == 1) {          // Slave is allowed to be enabled, don't re-enable the host though
      return;
    }
  #else                                             // Master OR Slave
    if ((_bools._hostEnabled | _bools._clientEnabled) == 1) {  //If either are enabled
      return;                                       // return and do nothing
    }
  #endif


  #if defined(TWI1)                                 // More then one TWI used
    if        (&TWI0 == _module) {           // check which one this function is working with
      TWI0_ClearPins();
    } else if (&TWI1 == _module) {
      TWI1_ClearPins();
    }
  #else                                             // Only one TWI is used
    TWI0_ClearPins();                               // Only one option is possible
  #endif

  _bools._hostEnabled    = 1;
  TWI_t* module = _module;
  module->MCTRLA        = TWI_ENABLE_bm;  // Master Interrupt flags stay disabled
  module->MSTATUS       = TWI_BUSSTATE_IDLE_gc;

  setClock(DEFAULT_FREQUENCY);
}


/**
 *@brief      begin (w/ parameters) changes the TWI module if only one Wire object is used
 *
 *            Works only if the TWI is disabled
 *
 *
 *@param      uint8_t address - the desired address for the client module
 *@param      bool receive_broadcast - if true, enables a response on the 0x00 call
 *@param      uint8_t second_address holds the data for the SADDRMASK register. If the LSB is '1'
 *              the TWI handles the 7 MSB as a second address for the client, otherwise the 7 MSB
 *              act as a bit mask, that disables the check on the corresponding SADDR bit.
 *
 *@return     void
 */
void TwoWire::begin(uint8_t address, bool receive_broadcast, uint8_t second_address) {
  if (__builtin_constant_p(address)) {
    if (address > 0x7F) {     // Compile-time check if address is actually 7 bit long
      badArg("TWI addresses must be supplied in 7-bit format. The read/write bit is handled by the library");
      return;
    }
  }


  #if defined(TWI_MANDS)                    // Check if the user wants to use Master AND Slave
    if (_bools._clientEnabled  == 1) {      // Master is allowed to be enabled, don't re-enable the client though
      return;
    }
  #else                                         // Master or Slave
    if ((_bools._hostEnabled | _bools._clientEnabled) == 1) {  //If either are enabled
      return;                                     // return and do nothing
    }
  #endif

  #if defined(TWI1)
    if        (&TWI0 == _module) {
      TWI0_ClearPins();
    } else if (&TWI1 == _module) {
      TWI1_ClearPins();
    }
  #else
    TWI0_ClearPins();
  #endif

  _bools._clientEnabled = 1;
  client_irq_mask = TWI_COLL_bm;
  TWI_t* module = _module;
  module->SADDR        = (address << 1) | receive_broadcast;
  module->SADDRMASK    = second_address;
  module->SCTRLA       = TWI_DIEN_bm | TWI_APIEN_bm | TWI_PIEN_bm  | TWI_ENABLE_bm;
}


/**
 *@brief      setClock sets the baud register to get the desired frequency
 *
 *            Has only an effect when used after begin(void)
 *
 *@param      uint32_t clock - the desired clock in hertz
 *
 *@return     uint8_t
 *@retval       0 if no error occurred
 *@retval       1 if a problem occurred
 */
uint8_t TwoWire::setClock(uint32_t clock) {
  TWI_t* module = _module;
  if (__builtin_constant_p(clock)) {
    if ((clock < 1000) || (clock > 15000000)) {
      badArg("Invalid frequency was passed for SCL clock!");
      return 1;
    }
  } else {
    if (clock < 1000) {
      return 1;
    }
  }
  if (_bools._hostEnabled == 1) {              // Do something only if the host is enabled.
    uint8_t newBaud = MasterCalcBaud(clock);        // get the new Baud value
    uint8_t oldBaud = module->MBAUD;                // load the old Baud value
    if (newBaud != oldBaud) {                       // compare both, in case the code is issuing this before every transmission.
      uint8_t restore = module->MCTRLA;             // Save the old Master state
      module->MCTRLA    = 0;                        // Disable Master
      module->MBAUD     = newBaud;                  // update Baud register
      if (clock > 400000) {
        module->CTRLA  |=  TWI_FMPEN_bm;            // Enable FastMode+
      } else {
        module->CTRLA  &= ~TWI_FMPEN_bm;            // Disable FastMode+
      }
      module->MCTRLA    = restore;                  // restore the old register, thus enabling it again
      if (restore & TWI_ENABLE_bm) {                // If the TWI was enabled,
        module->MSTATUS   = TWI_BUSSTATE_IDLE_gc;   // Force the state machine into IDLE according to the data sheet
      }
    }
    return 0;
  }
  return 1;
}


/**
 *@brief              TWI_MasterCalcBaud calculates the baud for the desired frequency
 *
 *@param              uint32_t frequency is the desired frequency
 *
 *@return             uint8_t value for the MBAUD register
 *@retval             the desired baud value
 */
#define TWI_BAUD(freq, t_rise) ((F_CPU / freq) / 2) - (5 + (((F_CPU / 1000000) * t_rise) / 2000))
uint8_t TwoWire::MasterCalcBaud(uint32_t frequency) {
  int16_t baud;

  #if (F_CPU == 20000000) || (F_CPU == 10000000)
    if (frequency >= 600000) {          // assuming 1.5kOhm
      baud = TWI_BAUD(frequency, 250);
    } else if (frequency >= 400000) {   // assuming 2.2kOhm
      baud = TWI_BAUD(frequency, 350);
    } else {                            // assuming 4.7kOhm
      baud = TWI_BAUD(frequency, 600);  // 300kHz will be off at 10MHz. Trade-off between size and accuracy
    }
  #else
    if (frequency >= 600000) {          // assuming 1.5kOhm
      baud = TWI_BAUD(frequency, 250);
    } else if (frequency >= 400000) {   // assuming 2.2kOhm
      baud = TWI_BAUD(frequency, 400);
    } else {                            // assuming 4.7kOhm
      baud = TWI_BAUD(frequency, 600);
    }
  #endif

  #if (F_CPU >= 20000000)
    const uint8_t baudlimit = 2;
  #elif (F_CPU == 16000000) || (F_CPU == 8000000) || (F_CPU == 4000000)
    const uint8_t baudlimit = 1;
  #else
    const uint8_t baudlimit = 0;
  #endif

  if (baud < baudlimit) {
    return baudlimit;
  } else if (baud > 255) {
    return 255;
  }

  return (uint8_t)baud;
}


/**
 *@brief      end disables the TWI host and client
 *
 *@param      void
 *
 *@return     void
 */
void TwoWire::end(void) {
  endMaster();
  endSlave();
}


/**
 *@brief      endMaster disables the TWI host
 *
 *@param      void
 *
 *@return     void
 */
void TwoWire::endMaster(void) {
  if (_bools._hostEnabled) {
    _module->MCTRLA = 0x00;
    _module->MBAUD  = 0x00;
    _bools._hostEnabled  = 0x00;
  }
}



/**
 *@brief      endSlave disables the TWI client
 *
 *@param      void
 *
 *@return     void
 */
void TwoWire::endSlave(void) {
  if (_bools._clientEnabled) {
    _module->SADDR       = 0x00;
    _module->SCTRLA      = 0x00;
    _module->SADDRMASK   = 0x00;
    _bools._clientEnabled     = 0x00;
    #if defined(TWI_DUALCTRL)
      _module->DUALCTRL  = 0x00;    // Disable pin splitting when available
    #endif
  }
}



/**
 *@brief      specialConfig allows configuring of wacky features.
 *            smbus evel: Vihmin and Vilmax are normally 0.7*Vdd (or VDDIO on MVIO pins) and 0.3*Vdd respectively.
 *            This option sets them to the SMBus 3.0 levels: In this mode, any voltage below 0.8V is guaranteed to be a LOW,
 *            and anything above 1.35 or 1.45 (see electrical characteristics in datasheet)
 *            This is of great utility for communication with lower voltage devices, especially where you don't have MVIO.
 *            Can also be set independently if dual mode used.
 *            longsetup: The setup times are normally 4 system clocks, however this can be doubled for disagreeable devices and/or adverse bus conditions
 *            Four options are available for the SDA hold times. sda_hold_dual handles the dual pins. This is used for SMBus 2.0 compatibility.
 *              WIRE_SDA_HOLD_OFF 0 - hold time off (default)
 *              WIRE_SDA_HOLD_50  1 - short hold time
 *              WIRE_SDA_HOLD_300 2 - meets SMBus 2.0 spec in typical cases
 *              WIRE_SDA_HOLD_500 3 - meets SMBus 2.0 across all corner cases
 *
 *            Returns a value between 0 and 7. 0 indicates all is well.
 *            it is otherwise bitwis combination of three error conditions (the first two of which we will stop compilation if they are constants and wrong)
 *            0x01 - smbuslevel must be 0 on parts that don't support it, you passed something else.
 *            0x02 - sda_hold_dual or smbuslevel_dual must be 0 if there is no dual mode
 *
 *@param      bool smbuslvl, bool longsetup, uint8_t sda_hold, uint8_t sda_hold_dual
 *
 *@return     uint8_t
 *            0x01 = smbus level ignored because part does not support it
 *            0x02 = sda_hold_dual ignored because part does not have dual mode.
 *            0x04 = sda_hold_dual ignored because dual mode not enabled.
 */

uint8_t TwoWire::specialConfig( __attribute__ ((unused)) bool smbuslvl, __attribute__ ((unused)) bool longsetup, __attribute__ ((unused)) uint8_t sda_hold, __attribute__ ((unused)) bool smbuslvl_dual, __attribute__ ((unused)) uint8_t sda_hold_dual) {
  uint8_t ret = 0;
  #if !defined(TWI_INPUTLVL_bm) // if there's no input level option, we want to notify the user with an error so they don't think they have a feature they don't
    if (__builtin_constant_p(smbuslvl))  { //but they could be passign a zero, which is legal. See if it's constant...
      if (smbuslvl) {                      // and non-zero, in which case error:
        badCall("the smbus level option is not present on these parts. You need a Dx for that.");
      }
    } else if (smbuslvl_dual) { //same deal for dual mode
      if (smbuslvl_dual) {
        badCall("the smbus level option is not present on these parts. You need a Dx for that.");
      }
    // the above will always fold to nothing or and error, and does not bloat binary
    // but we may not know at compile time what will be passed, so we have to have a runtime test.
    }
    else if (smbuslvl || smbuslvl_dual) {
      ret         |= 1; //
      smbuslvl     = 0; // We don't HAVE this option here, so zero out the option we pass along.
     //#if defined(TWI_DUALCTRL)
     //   smbuslvldual = 0; // no need to 0 - variable is no longer used, w/out dual ctrl, there's also not going to be smbus levels.
     //#endif
    }
  #endif
  if (__builtin_constant_p(sda_hold))  {
    if (sda_hold > 3) {
      badArg("Only 0, 1, 2 and 3 are valid SDA hold options. Suggest using the named constants.");
    }
  } else if (sda_hold > 3) {
    ret |= 0x08;
    sda_hold = 0;
  }

  #if !defined(TWI_DUALCTRL) // if no dual control, let user know with an error that they're trying to use dual mode features
    if (__builtin_constant_p(sda_hold_dual))  {
      if (sda_hold_dual) {
        badCall("Dual Mode is not supported on this part, sda_hold_dual and smbuslvl_dual must be omitted or 0");
      }
    }
    if (__builtin_constant_p(smbuslvl_dual))  {
      if (smbuslvl_dual) {
        badCall("Dual Mode is not supported on this part, smbuslvl_dual must be omitted or 0");
      }
    }
    else { //not compile time constant so have to check at runtime
      if (sda_hold_dual) { // only have to do SDA hold 0 - already caught bad input levels.
        ret |= 2; //error code.
        //sda_hold_dual= 0; not needed to zero out if no dual mode, we don't use this value anymore.
      }
    }
  #else
    if (__builtin_constant_p(sda_hold_dual))  {
      if (sda_hold_dual > 3) {
        badArg("Only 0, 1, 2 and 3 are valid SDA hold options. Suggest using the named constants.");
      }
    } else if (sda_hold_dual > 3) {
      ret |= 0x08;
      sda_hold_dual = 0;
    }
  #endif
  // Now we actually call the function in twi_pins.
  // Notice how the non-dualctrl parts also don't have smbus levels!
  #if defined(TWI1) // TWI_DUALCTRL is also defined here - everything with TWI1 has dual mode
    if (&TWI0 == _module) {
      ret |= TWI0_setConfig(smbuslvl, longsetup, sda_hold, smbuslvl_dual, sda_hold_dual);
    } else if (&TWI1 == _module) {
      ret |= TWI1_setConfig(smbuslvl, longsetup, sda_hold, smbuslvl_dual, sda_hold_dual);
    }
  #else
    #if defined(TWI_DUALCTRL) // And nothing without dual mode has smbus levels either.
      ret |= TWI0_setConfig(smbuslvl, longsetup, sda_hold, smbuslvl_dual, sda_hold_dual);
    #else
      ret |= TWI0_setConfig(longsetup, sda_hold);
    #endif
  #endif
  return ret;
}





/**
 *@brief      masterReceive sends a host READ with the specified client address
 *
 *            This function will read an arbitrary number of bytes from the TWI module
 *            and store them into the specified buffer. This allows the user to use
 *            custom sized buffers and avoids extra copy operations through read.
 *
 *@param      auto* length - pointer to a 8/16 bit variable containing the length. Will be overwritten with the amount of received bytes
 *@param      uint8_t* buffer - pointer to the memory area to be written upon.
 *@param      uint8_t addr - the address of the client (7-bit)
 *@param      uint8_t/bool sendStop - if the transaction should be terminated with a STOP condition
 *
 *@return     uint8_t
 *@retval     error code, see masterTrasnmit
 */


uint8_t TwoWire::masterReceive(auto *length, uint8_t *buffer, uint8_t addr, uint8_t sendStop) {
  TWI_t *module = _module;
  __asm__ __volatile__("\n\t" : "+z"(module));

  TWIR_INIT_ERROR;             // local variable for errors
  auto dataToRead = *length;

  uint8_t currentSM;
  uint8_t currentStatus;
  uint8_t state = 0;
  #if defined (TWI_TIMEOUT_ENABLE)
    uint16_t timeout = (F_CPU/1000);
  #endif

  while (true) {
    currentStatus = module->MSTATUS;
    currentSM = currentStatus & TWI_BUSSTATE_gm;  // get the current mode of the state machine

    if (currentSM == TWI_BUSSTATE_UNKNOWN_gc) {
      TWIR_SET_ERROR(TWI_ERR_UNINIT);
      break;
    }

    #if defined(TWI_TIMEOUT_ENABLE)
      if (--timeout == 0) {
        if      (currentSM == TWI_BUSSTATE_OWNER_gc) {
          TWIR_SET_ERROR(TWI_ERR_TIMEOUT);
        } else if (currentSM == TWI_BUSSTATE_IDLE_gc) {
          TWIR_SET_ERROR(TWI_ERR_PULLUP);
        } else {
          TWIR_SET_ERROR(TWI_ERR_UNDEFINED);
        }
        break;
      }
    #endif

    if (currentStatus & TWI_ARBLOST_bm) {   // Check for Bus error
      TWIR_SET_ERROR(TWI_ERR_BUS_ARB);      // set error flag
      break;                                // leave RX loop
    }

    if (currentSM != TWI_BUSSTATE_BUSY_gc) {
      if (state == 0x00) {
        module->MADDR = ADD_READ_BIT(addr);     // Send Address with read bit
        state |= 0x01;
        #if defined (TWI_TIMEOUT_ENABLE)
          timeout = (F_CPU/1000);               // reset timeout
        #endif
      } else {
        if (currentStatus & TWI_WIF_bm) {
          TWIR_SET_ERROR(TWI_ERR_ACK_ADR);        // set error flag
          module->MCTRLB = TWI_MCMD_STOP_gc;      // free the bus
          break;
        } else if (currentStatus & TWI_RIF_bm) {
          *buffer = module->MDATA;
          buffer++;
          #if defined (TWI_TIMEOUT_ENABLE)
            timeout = (F_CPU/1000);                // reset timeout
          #endif
          if (--dataToRead != 0) {
            module->MCTRLB = TWI_MCMD_RECVTRANS_gc;  // send an ACK so the Slave so it can send the next byte
          } else {
            if (sendStop != 0) {
              module->MCTRLB = TWI_ACKACT_bm | TWI_MCMD_STOP_gc;   // send STOP + NACK
            } else {
              module->MCTRLB = TWI_ACKACT_bm;   // Send NACK, but no STOP
            }
            break;
          }
        }
      }
    }
  }
  *length -= dataToRead;
  return TWIR_GET_ERROR;
}




/**
 *@brief      requestFrom is a wrapper for masterReceive providing the expected interface
 *
 *            When a greater quantity then the TWI_BUFFER_LENGTH is passed, the quantity gets
 *            limited to the TWI_BUFFER_LENGTH.
 *            Received Bytes must be read with read().
 *
 *@param      int/uint8_t address - the address of the client
 *@param      int/uint8_t/size_t quantity - the amount of bytes that are expected to be received
 *@param      int/bool sendStop - if the transaction should be terminated with a STOP condition
 *
 *@return     uint8_t/uint16_t
 *@retval     amount of bytes that were actually read. If 0, no read took place due to a bus error.
 */
twi_buf_index_t TwoWire::requestFrom(uint8_t  address,  twi_buf_index_t quantity,  uint8_t sendStop) {
  if (__builtin_constant_p(quantity)) {
    if (quantity > TWI_BUFFER_LENGTH) {
      badArg("requestFrom requests more bytes then there is Buffer space");
    }
  }
  if (quantity >= TWI_BUFFER_LENGTH) {
    quantity = TWI_BUFFER_LENGTH;
  }

  _clientAddress = address << 1;
  _bytesToReadWrite = quantity;  // for available/read/peek
  _bytesReadWritten = 0;

  masterReceive(&_bytesToReadWrite, _hostBuffer, _clientAddress, sendStop);  // We ignore the error that gets returned

  return _bytesToReadWrite;
}



/**
 *@brief      beginTransmission prepares the Wire object for a host WRITE.
 *
 *            This function only saves the client address in the structure, it does
 *            not perform any transmissions.
 *            a write() will fill the transmit buffer. write() has to be called after
 *            beginTransmission() was called. Any write() before beginTransmission() will be lost
 *
 *@param      uint8_t address - the address of the client
 *
 *@return     void
 */
void TwoWire::beginTransmission(uint8_t address) {
  if (__builtin_constant_p(address) > 0x7F) {     // Compile-time check if address is actually 7 bit long
    badArg("Supplied address seems to be 8 bit. Only 7-bit-addresses are supported");
    return;
  }
  if (_bools._hostEnabled) {
    // set address of targeted client
    _clientAddress = address << 1;
    _bytesToReadWrite = 0;  // fill buffer from 0
    _bytesReadWritten = 0;
  }
}


/**
 *@brief      endTransmission is the wrapper function for masterTransmit and offers the API compatibility
 *
 *            Originally, 'endTransmission' was an f(void) function. It has been modified to take
 *            one parameter indicating whether or not a STOP should be performed on the bus.
 *            Calling endTransmission(false) allows a sketch to perform a repeated start.
 *
 *            WARNING: Nothing in the library keeps track of whether the bus tenure has been
 *            properly ended with a STOP. It is very possible to leave the bus in a hung state if
 *            no call to endTransmission(true) is made. Some I2C devices will behave oddly
 *            if they do not see a STOP. Other hosts won't be able to issue their START for example.
 *
 *@param      bool sendStop - if the transaction should be terminated with a STOP condition
 *
 *
 *@return     uint8_t
 *@retval     0 for success,
 *            2 for when the address was NACK'd
 *            3 for when the data was NACK'd
 *            4 for unknown error
 *            5 for timeout on the bus
 *           16 for TWI is in a bad state
 *           17 for the pull ups likely missing
 *           18 for a bus arbitration/bus fault
 *           20 for likely a slave holding the clock low
 *          255 (-1) for TWI not initialized (begin not called) or bus somehow in "unknown" state.
 */
uint8_t TwoWire::endTransmission(bool sendStop) {
  // transmit (blocking)
  return masterTransmit(&_bytesToReadWrite, _hostBuffer, _clientAddress, sendStop);
}



/**
 *@brief      masterTransmit sends a host WRITE with the specified client address
 *
 *            This function will write an arbitrary number of bytes to the TWI module
 *            and read the data from the specified buffer. This allows the user to use
 *            custom sized buffers and avoids extra copy operations through write.
 *
 *@param      auto* length - pointer to a 8/16 bit variable containing the length. Will be overwritten with the amount of written bytes
 *@param      uint8_t* buffer - pointer to the memory area to be read from.
 *@param      uint8_t addr - the address of the client (7-bit)
 *@param      uint8_t/bool sendStop - if the transaction should be terminated with a STOP condition
 *
 *@return     uint8_t
 *@retval     errors (see endTransmission)
 */
uint8_t TwoWire::masterTransmit(auto *length, uint8_t *buffer, uint8_t addr, uint8_t sendStop) {
  TWI_t* module = _module;
  __asm__ __volatile__("\n\t" : "+z"(module));

  TWI_INIT_ERROR;
  uint8_t currentSM;
  uint8_t currentStatus;
  uint8_t stat = 0;
  auto dataToWrite = *length;
  #if defined (TWI_TIMEOUT_ENABLE)
    uint16_t timeout = (F_CPU/1000);
  #endif

  if ((module->MCTRLA & TWI_ENABLE_bm) == 0x00) {  // If the module is disabled, abort
    return TWI_ERR_UNINIT;
  }

  while (true) {
    currentStatus = module->MSTATUS;
    currentSM = currentStatus & TWI_BUSSTATE_gm;  // get the current mode of the state machine

    if (currentSM == TWI_BUSSTATE_UNKNOWN_gc) { // If the bus was not initialized
      return TWI_ERR_UNINIT;                    // abort
    }

    #if defined(TWI_TIMEOUT_ENABLE)
      if (--timeout == 0) {
        if        (currentSM == TWI_BUSSTATE_OWNER_gc) {
          TWI_SET_ERROR(TWI_ERR_TIMEOUT);
        } else if (currentSM == TWI_BUSSTATE_IDLE_gc) {
          TWI_SET_ERROR(TWI_ERR_PULLUP);
        } else {
          TWI_SET_ERROR(TWI_ERR_UNDEFINED);
        }
        break;
      }
    #endif

    if (currentStatus & TWI_ARBLOST_bm) {     // Check for Bus error
      TWI_SET_ERROR(TWI_ERR_BUS_ARB);       // set error flag
      break;                                // leave TX loop
    }

    if (currentSM != TWI_BUSSTATE_BUSY_gc) {  // Undefined was excluded, so make sure it's IDLE or OWNER
      if (stat == 0x00) {                     // At the start, we send the ADDR
        module->MADDR = ADD_WRITE_BIT(addr);      // clear bit 0
        stat |= 0x01;                             // skip this if we're done
        #if defined (TWI_TIMEOUT_ENABLE)
          timeout = (F_CPU/1000);                 // reset timeout
        #endif
      } else {
        if (currentStatus & TWI_WIF_bm) {     // ADDR was sent, check for completed write
          if (currentStatus & TWI_RXACK_bm) {   // got a NACK, see how much was written
            if (stat & 0x02) {                  // bit 1 set, data was already sent
              if (dataToWrite != 0)               // the client may send an ACK at the end. If we
                TWI_SET_ERROR(TWI_ERR_ACK_DAT);   // transferred everything, we can ignore the NACK
            } else {                              // otherwise, no data sent, ADDR NACK
              TWI_SET_ERROR(TWI_ERR_ACK_ADR);
            }
            break;
          } else {                              // No NACK on write
            if (dataToWrite != 0) {             // check if there is data to be written
              module->MDATA = *buffer;          // Writing to the register to send data
              buffer++;
              dataToWrite--;
              stat |= 0x02;                     // remember that we've sent data
              #if defined (TWI_TIMEOUT_ENABLE)
                timeout = (F_CPU/1000);                   // reset timeout
              #endif
            } else {                                      // else there is no data to be written
              break;                                      // TX finished, leave loop, error is still TWI_NO_ERR
            }
          }
        }
      } /* dataWritten == 0 */
    } /* currentSM != TWI_BUSSTATE_BUSY_gc */
  } /* while */

  *length -= dataToWrite;
  if ((sendStop != 0) || (TWI_ERR_SUCCESS != TWI_GET_ERROR)) {
    module->MCTRLB = TWI_MCMD_STOP_gc;                        // Send STOP
  }
  return TWI_GET_ERROR;
}

/**
 *@brief      write fills the transmit buffers, master or slave, depending on when it is called
 *
 *            Usually, the function fills the master transmit buffer.
 *            If called inside the specified onReceive or onRequest functions, or after
 *            selectSlaveBuffer, it fills the slave buffer.
 *
 *@param      uint8_t data - byte to put into the buffer
 *
 *
 *@return     uint8_t
 *@retval     1 if successful, 0 if the buffer is full
 */
size_t TwoWire::write(uint8_t data) {
  uint8_t* txBuffer;
  twi_buf_index_t *txHead;
  #if defined(TWI_MANDS)                   // Add following if host and client are split
    if (_bools._toggleStreamFn == 0x01) {
      txHead   = &(_bytesToReadWriteS);
      txBuffer =   _clientBuffer;
    } else
  #endif
  {
    txHead   = &(_bytesToReadWrite);
    txBuffer =   _hostBuffer;
  }

  /* Put byte in txBuffer */

  if ((*txHead) < TWI_BUFFER_LENGTH) {    // while buffer not full, write to it
    txBuffer[(*txHead)] = data;             // Load data into the buffer
    (*txHead)++;                            // advancing the head
    return 1;
  } else {
    return 0;
  }
}


/**
 *@brief      write for arrays
 *
 *            calls the write function in a for-loop
 *
 *@param      uint8_t *data - pointer to the array
 *@param      size_t quantity - amount of bytes to copy
 *
 *
 *@return     uint8_t
 *@retval     amount of bytes copied
 */
size_t TwoWire::write(const uint8_t *data, size_t quantity) {
  twi_buf_index_t i = 0;
  for (; i < quantity; i++) {
    if (TwoWire::write(*(data++)) == 0)
      break;   // break if buffer full
  }

  return i;
}



/**
 *@brief      available returns the amount of bytes that are available to read in the host or client buffer
 *
 *            Usually, the function returns the amount of bytes to read in the host buffer.
 *            If called inside the specified onReceive or onRequest functions, or after
 *            selectSlaveBuffer, it returns the amount of bytes ready to read in the client buffer
 *
 *@param      void
 *
 *@return     int
 *@retval     amount of bytes available to read from the host buffer
 */
int TwoWire::available(void) {
  #if defined(TWI_MANDS)                          // Add following if host and client are split
    if (_bools._toggleStreamFn == 0x01) {
      return (_bytesToReadWriteS - _bytesReadWrittenS);
    }
  #endif

  return (_bytesToReadWrite - _bytesReadWritten);
}



/**
 *@brief      read returns a byte from the host or client buffer and removes it from there
 *
 *            Usually, the function returns the byte from the host buffer.
 *            If called inside the specified onReceive or onRequest functions, or after
 *            selectSlaveBuffer, it returns the first byte from the slave buffer and removes it
 *            from there.
 *
 *@param      void
 *
 *@return     int
 *@retval     byte in the buffer or -1 if buffer is empty
 */
int TwoWire::read(void) {
  uint8_t *rxBuffer;
  twi_buf_index_t *rxHead, *rxTail;
  #if defined(TWI_MANDS)                         // Add following if host and client are split
    if (_bools._toggleStreamFn == 0x01) {
      rxHead   = &(_bytesToReadWriteS);
      rxTail   = &(_bytesReadWrittenS);
      rxBuffer =   _clientBuffer;
    } else
  #endif
  {
    rxHead   = &(_bytesToReadWrite);
    rxTail   = &(_bytesReadWritten);
    rxBuffer =   _hostBuffer;
  }


  if ((*rxTail) < (*rxHead)) {   // if there are bytes to read
    uint8_t c = rxBuffer[(*rxTail)];
    (*rxTail)++;
    return c;
  } else {                      // No bytes to read. At this point, rxTail moved up to
    return -1;                  // rxHead. To reset both to 0, a MasterRead or AddrWrite has to be called
  }
}


/**
 *@brief      readBytes reads a number of bytes from the buffer and removes them
 *
 *            Usually, the function reads the bytes from the host buffer.
 *            If called inside the specified onReceive or onRequest functions, or after
 *            selectSlaveBuffer, it reads the bytes from the slave buffer and removes it
 *            from there. When there are less bytes then requested, the return value will
 *            be smaller then the requested amount. It is more then the Stream variant
 *            and overwrites it.
 *
 *@param      uint8_t *data - pointer to the array
 *@param      size_t quantity - amount of bytes to copy
 *
 *@return     size_t
 *@retval     actually read bytes.
 */
size_t TwoWire::readBytes(char* data, size_t quantity) {
  twi_buf_index_t i = 0;
  for (; i < quantity; i++) {
    int16_t c = read();
    if (c < 0) break;   // break if buffer empty
    (*data++) = c;
  }
  return i;
}


/**
 *@brief      peek returns a byte from the host or client buffer but does not remove it
 *
 *            Usually, the function returns the byte from the host buffer.
 *            If called inside the specified onReceive or onRequest functions, or after
 *            selectSlaveBuffer, it returns the first byte from the slave buffer but does not
 *            remove it like read().
 *
 *@param      void
 *
 *@return     int
 *@retval     byte in the buffer or -1 if buffer is empty
 */
int TwoWire::peek(void) {
  uint8_t *rxBuffer;
  twi_buf_index_t *rxHead, *rxTail;
  #if defined(TWI_MANDS)                         // Add following if host and client are split
    if (_bools._toggleStreamFn == 0x01) {
      rxHead   = &(_bytesToReadWriteS);
      rxTail   = &(_bytesReadWrittenS);
      rxBuffer =   _clientBuffer;
    } else
  #endif
  {
    rxHead   = &(_bytesToReadWrite);
    rxTail   = &(_bytesReadWritten);
    rxBuffer =   _hostBuffer;
  }

  if ((*rxTail) < (*rxHead)) {   // if there are bytes to read
    return rxBuffer[(*rxTail)];
  } else {      // No bytes to read
    return -1;
  }
}


/**
 *@brief      flush resets the host and client buffers and restarts the TWI module
 *
 *@param      void
 *
 *@return     void
 */
void TwoWire::flush(void) {
  /* Turn off and on TWI module */
  TWI_t* module = _module;
  #if defined(ERRATA_TWI_FLUSH)
    // badCall("The AVR DA-series parts are impacted by an errata that leaves the TWI peripheral in a non-functioning state when using flush.");
    // restarting TWI hardware by hand. Extra size shouldn't matter on DA series
    uint8_t temp_MCTRLA     = module->MCTRLA;
    uint8_t temp_SCTRLA     = module->SCTRLA;
    module->MCTRLA  = 0x00;
    module->SCTRLA  = 0x00;
    module->MCTRLA  = temp_MCTRLA;
    module->MSTATUS = 0x01;  // force TWI state machine into idle state
    module->SCTRLA  = temp_SCTRLA;
  #else
    module->MCTRLB |= TWI_FLUSH_bm;
  #endif
}

/**
 *@brief      getIncomingAddress returns the last address the client has reacted to
 *
 *            When used in host only mode, it will return the clientAddress
 *            that was set by the user in beginTransmission()
 *
 *@param      void
 *
 *@return     uint8_t
 *@retval     left-shifted I2C address with the write/read bit
 */
uint8_t TwoWire::getIncomingAddress(void) {
  #if defined(TWI_MANDS)                         // Alias handler
    return _incomingAddress;
  #else
    return _clientAddress;
  #endif
}


/**
 *@brief      getBytesRead provides a facility for the slave to check how many bytes were
 *              successfully read by the master.
 *
 *            Useful for implementing a "register model" like most I2C hardware does.
 *            Calling this will reset the counter, since it is an unusual use case for
 *              that to not be the next thing you do.
 *
 *@return     uint8_t/uint16_t
 *@retval     Number of bytes read by a master from this device acting as a slave since
 *              the last time this was called.
 */

twi_buf_index_t TwoWire::getBytesRead() {
  twi_buf_index_t num = _bytesTransmittedS;
  _bytesTransmittedS = 0;
  return num;
}


/**
 *@brief      slaveTransactionOpen provides a facility for the slave to determine if a there
 *              is an ongoing transaction when called outside of one of the handlers.
 *
 *            Useful to, for example, check that no transaction is in progress before sleeping.
 *
 *@return     uint8_t
 *@retval     0 = No transaction is currently in progress.
 *            1 = A master is currently writing data to this device, and the onReceive handler
 *              will be called once this is complete.
 *            2 = A master is currently reading data from this device. slaveTransactionOpen()
 *              must be polled if it is necessary to know when this has concluded, as the
 *              onRequest handler is called at the start of a read only.
 */

uint8_t TwoWire::slaveTransactionOpen() {
  uint8_t status = _module->SSTATUS;
  if (!(status & TWI_AP_bm)) return 0;  // If AP bit is cleared, last match was a stop condition -> not in transaction.
  if (status & TWI_DIR_bm) return 2;    // DIR bit will be 1 if last address match was for read
  return 1;                             // Otherwise it was a write.
}


/**
 *@brief      enableDualMode enables the splitting of host and client pins
 *
 *            useful when you want to separate multiple TWI buses.
 *            Only available on the chips with a bigger pin count. See data sheet.
 *            To disable dualmode, please use Wire.endSlave() (in MANDS case) or Wire.end();
 *
 *@param      bool fmp_enable - set true if the TWI module has to expect a high
 *              frequency (>400kHz) on the salve pins
 *
 *@return     void
 */
void TwoWire::enableDualMode(bool fmp_enable) {
  #if defined(TWI_DUALCTRL)
    _module->DUALCTRL = ((fmp_enable << TWI_FMPEN_bp) | TWI_ENABLE_bm);
  #else
    badCall("enableDualMode was called, but device does not support it");
    (void) fmp_enable;    // Disable unused variable warning
  #endif
}


/**
 *@brief      checkPinLevels returns the pin level of the Master SDA/SCL pins
 *
 *            useful when you want to make sure the bus is ready and there is
 *              no device that might take longer to switch its pins to open-drain.
 *              In a multi-master system, a return value of 0x03 does not guarantee
 *              that the bus is IDLE.
 *
 *@param      none
 *
 *@return     uint8_t - SDA level is represented with bit 0, SCL with bit 1. Bus ready with 0x03
 */
uint8_t TwoWire::checkPinLevels(void) {
  #if defined(TWI1)
    if (&TWI0 == _module)  {
      return TWI0_checkPinLevel();
    } else if (&TWI1 == _module)  {
      return TWI1_checkPinLevel();
    } else {
      return false;
    }
  #else
    return TWI0_checkPinLevel();
  #endif
}


/**
 *@brief      selectSlaveBuffer allows the user to access the slave buffer
 *
 *            When debugging the slave mode, this can be useful to access the slave
 *            buffer with write/read/etc. outside the slave onRequest/onReceive functions.
 *
 *            It is best to call this function on disabled global or slave interrupts.
 *            deselectSlaveBuffer() has to be called later to ensure proper operation of the TWI.
 *
 *@param      void
 *
 *@return     void
 */
void TwoWire::selectSlaveBuffer(void) {
  #if defined(TWI_MANDS)
    _bools._toggleStreamFn = 0x01;
  #else
    badCall("selectSlaveBuffer() was called, but simultaneous mode is not selected");
  #endif
}


/**
 *@brief      deselectSlaveBuffer allows the user to access the master buffer(s) again
 *
 *            When debugging the slave mode, this can be useful to access the slave
 *            buffer with write/read/etc. outside the slave onRequest/onReceive functions.
 *
 *            It is best to call this function on disabled global or slave interrupts.
 *            This function has to be called after a previous call to selectSlaveBuffer().
 *
 *@param      void
 *
 *@return     void
 */
void TwoWire::deselectSlaveBuffer(void) {
  #if defined(TWI_MANDS)
    _bools._toggleStreamFn = 0x00;
  #else
    badCall("deselectSlaveBuffer() was called, but simultaneous mode is not selected");
  #endif
}





void TwoWire::HandleSlaveIRQ(TwoWire *wire_s) {
  if (wire_s == NULL) {
    return;
  }

  uint8_t *address,  *buffer;
  twi_buf_index_t *head, *tail;
  #if defined(TWI_MANDS)
    address = &(wire_s->_incomingAddress);
    head    = &(wire_s->_bytesToReadWriteS);
    tail    = &(wire_s->_bytesReadWrittenS);
    buffer  =   wire_s->_clientBuffer;
  #else
    address = &(wire_s->_clientAddress);
    head    = &(wire_s->_bytesToReadWrite);
    tail    = &(wire_s->_bytesReadWritten);
    buffer  =   wire_s->_hostBuffer;
  #endif

  #if defined(TWI_MANDS)
    wire_s->_bools._toggleStreamFn = 0x01;
  #endif

  uint8_t action = 0;
  uint8_t clientStatus = wire_s->_module->SSTATUS;


  if (clientStatus & TWI_APIF_bm) {   // Address/Stop Bit set
    if (wire_s->_bools._hostDataSent != 0) { // At this point, we have either a START, REPSTART or a STOP
      wire_s->_bools._hostDataSent = 0x00;
      if (wire_s->user_onReceive != NULL) {   // only if the last APIF was a Master Write,
        wire_s->user_onReceive((*head));      // we notify the sketch about new Data
      }
    }

    if (clientStatus & TWI_AP_bm) {     // Address bit set
      if ((*head) == 0) {                 // only if there was no data (START)
        pauseDeepSleep((uint8_t)((uint16_t)wire_s->_module));  // Only START can wake from deep sleep, change to IDLE
      }
      (*head) = 0;
      (*tail) = 0;
      (*address) = wire_s->_module->SDATA;  // read address from data register
      if (clientStatus & TWI_DIR_bm) {      // Master is reading
        if (wire_s->user_onRequest != NULL) {
          wire_s->user_onRequest();
        }
        if ((*head) == 0) {                     // If no data to transmit, send NACK
          action = TWI_ACKACT_bm | TWI_SCMD_COMPTRANS_gc;  // NACK + "Wait for any Start (S/Sr) condition"
        } else {
          action = TWI_SCMD_RESPONSE_gc;        // "Execute Acknowledge Action succeeded by reception of next byte"
        }
      } else {                          // Master is writing
        wire_s->_bools._hostDataSent = 0x01;
        action = TWI_SCMD_RESPONSE_gc;  // "Execute Acknowledge Action succeeded by slave data interrupt"
      }
    } else {                          // Stop bit set
      restoreSleep((uint8_t)((uint16_t)wire_s->_module));
      (*head) = 0;
      (*tail) = 0;
      action = TWI_SCMD_COMPTRANS_gc;  // "Wait for any Start (S/Sr) condition"
    }
  } else if (clientStatus & TWI_DIF_bm) { // Data bit set
    if (clientStatus & TWI_DIR_bm) {        // Master is reading
      if (clientStatus & wire_s->client_irq_mask) {   // If a collision was detected, or RXACK bit is set (when it matters)
        wire_s->client_irq_mask = TWI_COLL_bm;  // stop checking for NACK
        (*head) = 0;                            // Abort further data writes
        action = TWI_SCMD_COMPTRANS_gc;         // "Wait for any Start (S/Sr) condition"
      } else {                                // RXACK bit not set, no COLL
        wire_s->_bytesTransmittedS++;           // increment bytes transmitted counter (for register model)
        wire_s->client_irq_mask = TWI_COLL_bm | TWI_RXACK_bm;  // start checking for NACK
        if ((*tail) < (*head)) {                // Data is available
          wire_s->_module->SDATA = buffer[(*tail)];  // Writing to the register to send data
          (*tail)++;                              // Increment counter for sent bytes
          action = TWI_SCMD_RESPONSE_gc;          // "Execute Acknowledge Action succeeded by reception of next byte"
        } else {                                  // No more data available
          (*head) = 0;                            // Avoid triggering REPSTART handler
          action = TWI_SCMD_COMPTRANS_gc;         // "Wait for any Start (S/Sr) condition"
        }
      }
    } else {                                  // Master is writing
      uint8_t payload = wire_s->_module->SDATA;     // reading SDATA will clear the DATA IRQ flag
      if ((*head) < TWI_BUFFER_LENGTH) {            // make sure that we don't have a buffer overflow in case Master ignores NACK
        buffer[(*head)] = payload;                  // save data
        (*head)++;                                  // Advance Head
        if ((*head) == TWI_BUFFER_LENGTH) {         // if buffer is not yet full
          action = TWI_ACKACT_bm | TWI_SCMD_COMPTRANS_gc;  // "Execute ACK Action succeeded by waiting for any Start (S/Sr) condition"
        } else {                                    // else buffer would overflow with next byte
          action = TWI_SCMD_RESPONSE_gc;            // "Execute Acknowledge Action succeeded by reception of next byte"
        }
      }
    }
  }
  wire_s->_module->SCTRLB = action;  // using local variable (register) reduces the amount of loading _module
  #if defined(TWI_MANDS)
    wire_s->_bools._toggleStreamFn = 0x00;
  #endif
}



/**
 *@brief      onReceive saves the pointer to the desired function to call on host WRITE / client READ.
 *
 *            remember, the specified function is called in an ISR, so keep it short.
 *
 *@param      void (*function)(int) - a void returning function that accepts an int as parameter
 *
 *@return     void
 */
void TwoWire::onReceive(void (*function)(int)) {
  user_onReceive = function;
}


/**
 *@brief      onRequest saves the pointer to the desired function to call on host READ / client WRITE.
 *
 *            remember, the specified function is called in an ISR, so keep it short.
 *
 *@param      void (*function)(void) - a void returning function that does not accept any parameters
 *
 *@return     void
 */
void TwoWire::onRequest(void (*function)(void)) {
  user_onRequest = function;
}


/**
 *@brief      TWI0 Slave Interrupt vector
 */
ISR(TWI0_TWIS_vect) {
  TwoWire::HandleSlaveIRQ(twi0_wire);
}


/**
 *@brief      TWI1 Slave Interrupt vector
 */
#if defined(TWI1)
  ISR(TWI1_TWIS_vect) {
    TwoWire::HandleSlaveIRQ(twi1_wire);
  }
#endif


/**
 *@brief      pauseDeepSleep and restoreSleep handle the sleep guard
 *
 *            When used only by one peripheral, just saving the sleep register is plenty,
 *              But when used by more then one, special care must be taken to restore the
 *              sleep settings only at the end by using a bitmask on the upper nibble.
 *              e.g. when TWI0 - START, TWI1 - START, TWI0 - STOP, TWI1 - STOP
 *
 *@param      uint8_t module_lower_Addr - lower byte of the TWI Module address
                    (used only with two Wire objects)
 *
 *@return     void
 */
void pauseDeepSleep(uint8_t module_lower_Addr) {
  #if defined(TWI_USING_WIRE1)
    uint8_t bit_mask = 0x10;
    if (module_lower_Addr == (uint8_t)((uint16_t)&TWI1)){
      bit_mask = 0x20;
    }
    uint8_t sleepStackLoc = sleepStack;
    if (sleepStackLoc == 0) {        // Save sleep state only if stack empty
      sleepStackLoc = SLPCTRL.CTRLA;        // save sleep settings to sleepStack
      SLPCTRL.CTRLA = sleepStackLoc & 0x01; // only leave the SEN bit, if it was set
    }
    sleepStackLoc |= bit_mask;      // Remember which module is busy
    sleepStack = sleepStackLoc;
  #else
    (void) module_lower_Addr;

    if (sleepStack == 0x00) {
      uint8_t slp = SLPCTRL.CTRLA;    // save current sleep State
      sleepStack = slp;               // using local variable for less store/load
      SLPCTRL.CTRLA = slp & 0x01;     // only leave the SEN bit, if it was set
    }
  #endif
}

void restoreSleep(uint8_t module_lower_Addr) {
  #if defined(TWI_USING_WIRE1)
    uint8_t bit_mask = ~0x10;
    if (module_lower_Addr == (uint8_t)((uint16_t)&TWI1)){
      bit_mask = ~0x20;
    }
    uint8_t sleepStackLoc = sleepStack;
    sleepStackLoc &= bit_mask;
    if (sleepStackLoc > 0) {      // only do something if sleep was enabled
      if (sleepStackLoc < 0x10) {   // if upper nibble is clear
        SLPCTRL.CTRLA = sleepStackLoc;  // restore sleep
        sleepStackLoc = 0;              // reset everything
      }
    }
    sleepStack = sleepStackLoc;
  #else
    (void) module_lower_Addr;
    SLPCTRL.CTRLA = sleepStack;
    sleepStack = 0;
  #endif
}



/**
 *  Wire object constructors with the default TWI modules.
 *  If there is absolutely no way to swap the pins physically,
 *  here is the best way to do so in software.
 */
#if defined(TWI0)
  TwoWire Wire(&TWI0);
#endif

#if defined(TWI1)
  #if defined(TWI_USING_WIRE1)
    TwoWire Wire1(&TWI1);
  #endif
#endif
