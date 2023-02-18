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
  Modified extensively 2021-22 by MX682X for megaTinyCore and DxCore.
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
  #include "twi.h"
  #include "twi_pins.h"
}


/**
 *@brief      TwoWire creates a Wire object
 *
 *@param      TWI_t *module - the pointer to the TWI module that the Wire object is supposed to use
 *
 *@return     constructor can't return anything
 */
TwoWire::TwoWire(TWI_t *twi_module) {
  vars._module = twi_module;
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
    if (&TWI0 == vars._module)  {
      return TWI0_Pins(sda_pin, scl_pin);
    } else if (&TWI1 == vars._module)  {
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
            badArg("The requested swap level is not available on any current or announced part - did you pass a bitmask instead of a number?");
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
    if (&TWI0 == vars._module) {
      return TWI0_swap(state);
    } else if (&TWI1 == vars._module) {
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
    if (&TWI0 == vars._module) {
      TWI0_usePullups();
    } else if (&TWI1 == vars._module) {
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
  #if defined(TWI1)
    #if defined(TWI_USING_WIRE1)
      badCall("swapModule() can only be used if Wire1 is not used");
    #else
      if (vars._module->MCTRLA == 0) {    // client and host initialisations enable MCTRLA, so just check for that
        vars._module = twi_module;
        return true;                      // Success
      }
    #endif
  #else
    badCall("Only one TWI module available, nothing to switch with");
  #endif
  (void)twi_module;   // Remove warning unused variable
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
  TWI_MasterInit(&vars);
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
  TWI_SlaveInit(&vars, address, receive_broadcast, second_address);
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
  return TWI_MasterSetBaud(&vars, clock);
}


/**
 *@brief      end disables the TWI host and client
 *
 *@param      void
 *
 *@return     void
 */
void TwoWire::end(void) {
  TWI_Disable(&vars);
}


/**
 *@brief      endMaster disables the TWI host
 *
 *@param      void
 *
 *@return     void
 */
#if defined(TWI_MANDS)
void TwoWire::endMaster(void) {
  TWI_DisableMaster(&vars);
}
#endif


/**
 *@brief      endSlave disables the TWI client
 *
 *@param      void
 *
 *@return     void
 */
#if defined(TWI_MANDS)
void TwoWire::endSlave(void) {
  TWI_DisableSlave(&vars);
}
#endif
/**
 *@brief      specialConfig allows configuring of wacky features.
 *            smbus evel: Vihmin and Vilmax are normally 0.7*Vdd (or VDDIO on MVIO pins) and 0.3*Vdd respectively. This option sets them to the SMBus 3.0 levels:
 *            In this mode, any voltage below 0.8V is guaranteed to be a LOW, and anything above 1.35 or 1.45 (see electrical characteristics in datasheet)
 *            This is of great utility for communication with lower voltage devices, especially where you don't have MVIO. Can also be set independently if dual mode used.
 *            loongsetup: The setup times are normally 4 system clocks, however this can be doubled for disagreeable devices and/or adverse bus conditions
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
        //sda_hold_dual= 0; not needed to zero out if no dual mode, we don't use this value anmore.
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
    if (&TWI0 == vars._module) {
      ret |= TWI0_setConfig(smbuslvl, longsetup, sda_hold, smbuslvl_dual, sda_hold_dual);
    } else if (&TWI1 == vars._module) {
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
 *@brief      requestFrom sends a host READ with the specified client address
 *
 *            When a greater quantity then the BUFFER_LENGTH is passed, the quantity gets
 *            limited to the BUFFER_LENGTH.
 *            Received Bytes must be read with read().
 *
 *@param      int/uint8_t address - the address of the client
 *@param      int/uint8_t/size_t quantity - the amount of bytes that are expected to be received
 *@param      int/bool sendStop - if the transaction should be terminated with a STOP condition
 *
 *@return     uint8_t/uint16_t
 *@retval     amount of bytes that were actually read. If 0, no read took place due to a bus error.
 */
twi_buffer_index_t TwoWire::requestFrom(uint8_t  address,  twi_buffer_index_t quantity,  uint8_t sendStop) {
  if (quantity >= BUFFER_LENGTH) {
    quantity = BUFFER_LENGTH;
  }
  vars._clientAddress = address << 1;
  return TWI_MasterRead(&vars, quantity, sendStop);
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
  twi_buffer_index_t *txHead;
  #if defined(TWI_MERGE_BUFFERS)                  // Same Buffers for tx/rx
    txHead  = &(vars._bytesToReadWrite);
  #else                                           // Separate tx/rx Buffers
    txHead  = &(vars._bytesToWrite);
  #endif
  if (__builtin_constant_p(address) > 0x7F) {     // Compile-time check if address is actually 7 bit long
    badArg("Supplied address seems to be 8 bit. Only 7-bit-addresses are supported");
    return;
  }
  // set address of targeted client
  vars._clientAddress = address << 1;
  (*txHead) = 0;  // fill buffer from 0
}


/**
 *@brief      endTransmission is the function that actually performs the (blocking) host WRITE
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
  return TWI_MasterWrite(&vars, sendStop);
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
  twi_buffer_index_t *txHead;
  #if defined(TWI_MANDS)                   // Add following if host and client are split
    if (vars._bools._toggleStreamFn == 0x01) {
      txHead   = &(vars._bytesToReadWriteS);
      txBuffer =   vars._trBufferS;
    } else
  #endif
  {
    #if defined(TWI_MERGE_BUFFERS)         // Same Buffers for tx/rx
      txHead   = &(vars._bytesToReadWrite);
      txBuffer =   vars._trBuffer;
    #else                                  // Separate tx/rx Buffers
      txHead   = &(vars._bytesToWrite);
      txBuffer =   vars._txBuffer;
    #endif
  }

  /* Put byte in txBuffer */

  if ((*txHead) < BUFFER_LENGTH) {      // while buffer not full, write to it
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
  twi_buffer_index_t i = 0;
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
  int rxHead;
  #if defined(TWI_MANDS)                          // Add following if host and client are split
    if (vars._bools._toggleStreamFn == 0x01) {
      rxHead  = vars._bytesToReadWriteS - vars._bytesReadWrittenS;
    } else
  #endif
  {
    #if defined(TWI_MERGE_BUFFERS)                // Same Buffers for tx/rx
      rxHead  = vars._bytesToReadWrite - vars._bytesReadWritten;
    #else                                         // Separate tx/rx Buffers
      rxHead  = vars._bytesToRead - vars._bytesRead;
    #endif
  }
  return rxHead;
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
  twi_buffer_index_t *rxHead, *rxTail;
  #if defined(TWI_MANDS)                         // Add following if host and client are split
    if (vars._bools._toggleStreamFn == 0x01) {
      rxHead   = &(vars._bytesToReadWriteS);
      rxTail   = &(vars._bytesReadWrittenS);
      rxBuffer =   vars._trBufferS;
    } else
  #endif
  {
    #if defined(TWI_MERGE_BUFFERS)               // Same Buffers for tx/rx
      rxHead   = &(vars._bytesToReadWrite);
      rxTail   = &(vars._bytesReadWritten);
      rxBuffer =   vars._trBuffer;
    #else                                        // Separate tx/rx Buffers
      rxHead   = &(vars._bytesToRead);
      rxTail   = &(vars._bytesRead);
      rxBuffer =   vars._rxBuffer;
    #endif
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
  twi_buffer_index_t i = 0;
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
  twi_buffer_index_t *rxHead, *rxTail;
  #if defined(TWI_MANDS)                         // Add following if host and client are split
    if (vars._bools._toggleStreamFn == 0x01) {
      rxHead   = &(vars._bytesToReadWriteS);
      rxTail   = &(vars._bytesReadWrittenS);
      rxBuffer =   vars._trBufferS;
    } else
  #endif
  {
    #if defined(TWI_MERGE_BUFFERS)               // Same Buffers for tx/rx
      rxHead   = &(vars._bytesToReadWrite);
      rxTail   = &(vars._bytesReadWritten);
      rxBuffer =   vars._trBuffer;
    #else                                        // Separate tx/rx Buffers
      rxHead   = &(vars._bytesToRead);
      rxTail   = &(vars._bytesRead);
      rxBuffer =   vars._rxBuffer;
    #endif
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
  TWI_Flush(&vars);
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
    return vars._incomingAddress;
  #else
    return vars._clientAddress;
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

twi_buffer_index_t TwoWire::getBytesRead() {
  twi_buffer_index_t num = vars._bytesTransmittedS;
  vars._bytesTransmittedS = 0;
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
  uint8_t status = vars._module->SSTATUS;
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
    vars._module->DUALCTRL = ((fmp_enable << TWI_FMPEN_bp) | TWI_ENABLE_bm);
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
    if (&TWI0 == vars._module)  {
      return TWI0_checkPinLevel();
    } else if (&TWI1 == vars._module)  {
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
    vars._bools._toggleStreamFn = 0x01;
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
    vars._bools._toggleStreamFn = 0x00;
  #else
    badCall("deselectSlaveBuffer() was called, but simultaneous mode is not selected");
  #endif
}




/**
 *@brief      onSlaveIRQ is called by the interrupts and calls the interrupt handler
 *
 *            Another little hack I had to do: This function is static, thus there is no extra copy
 *            when a new Wire object, like Wire1 is initialized. When I first wrote this function
 *            I was using Wire.vars.module and Wire1.vars.module to figure out which pointer to pass,
 *            but this made the compiler create a Wire1 object in some cases, where Wire1 was never used
 *            by the user. So I rewrote this function with the thought that if the module can be different,
 *            there is just one Wire object, so the code doesn't have to check if Wire is using TWI0 or TWI1
 *
 *
 *@param      TWI_t *module - the pointer to the TWI module
 *
 *@return     void
 */
void TwoWire::onSlaveIRQ(TWI_t *module) {          // This function is static and is, thus, the only one for both
                                                    // Wire interfaces. Here is decoded which interrupt was fired.
  #if defined(TWI1) &&  defined(TWI_USING_WIRE1)   // Two TWIs available and TWI1 is used. Need to check the module
    if (module == &TWI0) {
      TWI_HandleSlaveIRQ(&(Wire.vars));
    } else if (module == &TWI1) {
      TWI_HandleSlaveIRQ(&(Wire1.vars));
    }
  #else                                 // Otherwise, only one Wire object is being used anyway, no need to check
    (void)module;
    TWI_HandleSlaveIRQ(&(Wire.vars));
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
  vars.user_onReceive = function;
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
  vars.user_onRequest = function;
}


#if defined(TWI_READ_ERROR_ENABLED) && defined(TWI_ERROR_ENABLED)
uint8_t TwoWire::returnError() {
  return vars._errors;
}
#endif


/**
 *@brief      TWI0 Slave Interrupt vector
 */
ISR(TWI0_TWIS_vect) {
  TwoWire::onSlaveIRQ(&TWI0);
}


/**
 *@brief      TWI1 Slave Interrupt vector
 */
#if defined(TWI1)
  ISR(TWI1_TWIS_vect) {
    TwoWire::onSlaveIRQ(&TWI1);
  }
#endif


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
