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
*/

extern "C" {
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "utility/twi.h"
}

#include "Wire.h"

#ifndef DEFAULT_FREQUENCY
  #define DEFAULT_FREQUENCY 100000
#endif

// Initialize Class Variables //////////////////////////////////////////////////

uint8_t TwoWire::rxBuffer[BUFFER_LENGTH];
uint8_t TwoWire::rxBufferIndex = 0;     //head
uint8_t TwoWire::rxBufferLength = 0;    //tail

uint8_t TwoWire::txAddress = 0;
uint8_t TwoWire::txBuffer[BUFFER_LENGTH];
uint8_t TwoWire::txBufferIndex = 0;     //head
uint8_t TwoWire::txBufferLength = 0;    //tail

uint8_t TwoWire::transmitting = 0;
void (*TwoWire::user_onRequest)(void);
void (*TwoWire::user_onReceive)(int);

// Constructors ////////////////////////////////////////////////////////////////

TwoWire::TwoWire() {
}

// Public Methods //////////////////////////////////////////////////////////////
// True if pin specification actually exists
// We now compile-error if a compile time known pin mapping is requested that is invalid.
// Note that we do not currently support the dual TWI mode
// *INDENT-OFF* This is hard enough to follow indented by hand
// would be a total nightmare the way astyle wants it.
bool TwoWire::pins(uint8_t sda_pin, uint8_t scl_pin) {
  #if defined(PORTMUX_CTRLB) /* tinyAVR 0/1 with TWI mux options */
    #if defined(PIN_WIRE_SDA_PINSWAP_1) && defined(PIN_WIRE_SCL_PINSWAP_1)
      if (__builtin_constant_p(sda_pin) && __builtin_constant_p(scl_pin)) {
        if (!((sda_pin == PIN_WIRE_SDA && scl_pin == PIN_WIRE_SCL) || (sda_pin == PIN_WIRE_SDA_PINSWAP_1 && scl_pin == PIN_WIRE_SCL_PINSWAP_1)))
          badArg("Pins passed to Wire.pins() known at compile time to be invalid");
      }
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
      if (__builtin_constant_p(sda_pin) && __builtin_constant_p(scl_pin)) {
        /* constant case - error if there's no swap available and the pins they hope to use are known at compile time */
        if (sda_pin != PIN_WIRE_SDA || scl_pin != PIN_WIRE_SCL) {
          badCall("This part does not support alternate Wire pins, if Wire.pins() is called, it must be passed the default pins");
          return false;
        } else {
          return true;
        }
      } else { /* Non-constant case */
        return (sda_pin == PIN_WIRE_SDA && scl_pin == PIN_WIRE_SCL);
      }
    #endif
  #elif defined(PORTMUX_TWIROUTEA) && (defined(PIN_WIRE_SDA) || defined(PIN_WIRE_SDA_PINSWAP_2) || defined(PIN_WIRE_SDA_PINSWAP_3) )
    /* Dx-series, megaAVR only, not tinyAVR. If tinyAVR 2's supported alt TWI pins, they would probably have this too */
    if (__builtin_constant_p(sda_pin) && __builtin_constant_p(scl_pin)) {
        #if defined(PIN_WIRE_SDA)
          #if defined(PIN_WIRE_SDA_PINSWAP_2)
            #if defined(PIN_WIRE_SDA_PINSWAP_3)
              if (!((sda_pin == PIN_WIRE_SDA && scl_pin == PIN_WIRE_SCL) || (sda_pin == PIN_WIRE_SDA_PINSWAP_2 && scl_pin == PIN_WIRE_SCL_PINSWAP_2) || (sda_pin == PIN_WIRE_SDA_PINSWAP_3 && scl_pin == PIN_WIRE_SCL_PINSWAP_3))) {
            #else
              if (!((sda_pin == PIN_WIRE_SDA && scl_pin == PIN_WIRE_SCL) || (sda_pin == PIN_WIRE_SDA_PINSWAP_2 && scl_pin == PIN_WIRE_SCL_PINSWAP_2))) {
            #endif
          #else /* No pinswap 2 */
            #if defined(PIN_WIRE_SDA_PINSWAP_3)
              if (!((sda_pin == PIN_WIRE_SDA && scl_pin == PIN_WIRE_SCL) || (sda_pin == PIN_WIRE_SDA_PINSWAP_3 && scl_pin == PIN_WIRE_SCL_PINSWAP_3))) {
            #else
              if (!(sda_pin == PIN_WIRE_SDA && scl_pin == PIN_WIRE_SCL)) {
            #endif
          #endif
        #else /* No pinswap 0 */
          #if defined(PIN_WIRE_SDA_PINSWAP_2)
            #if defined(PIN_WIRE_SDA_PINSWAP_3)
              if (!((sda_pin == PIN_WIRE_SDA_PINSWAP_2 && scl_pin == PIN_WIRE_SCL_PINSWAP_2) || (sda_pin == PIN_WIRE_SDA_PINSWAP_3 && scl_pin == PIN_WIRE_SCL_PINSWAP_3))) {
            #else
              if (!((sda_pin == PIN_WIRE_SDA_PINSWAP_2 && scl_pin == PIN_WIRE_SCL_PINSWAP_2))) {
            #endif
          #else /* No pinswap 2 */
            #if defined(PIN_WIRE_SDA_PINSWAP_3)
              if (!((sda_pin == PIN_WIRE_SDA_PINSWAP_3 && scl_pin == PIN_WIRE_SCL_PINSWAP_3))) {
            #else
              #error "Can't happen"
            #endif
          #endif
        #endif
        badArg("Pins passed to Wire.pins() known at compile time to be invalid");
      } /* end of error conditionally generated when pins requested known at compile time and wrong */
    } /* End of test for compile time known SDA and SCL pins requested */
    #if defined(PIN_WIRE_SDA_PINSWAP_3)
      if (sda_pin == PIN_WIRE_SDA_PINSWAP_2 && scl_pin == PIN_WIRE_SCL_PINSWAP_2) {
        // Use pin swap
        PORTMUX.TWIROUTEA = (PORTMUX.TWIROUTEA & 0xFC) | 0x03;
        return true;
      } else
    #endif
    #if defined(PIN_WIRE_SDA_PINSWAP_2)
      if (sda_pin == PIN_WIRE_SDA_PINSWAP_2 && scl_pin == PIN_WIRE_SCL_PINSWAP_2) {
        // Use pin swap
        PORTMUX.TWIROUTEA = (PORTMUX.TWIROUTEA & 0xFC) | 0x02;
        return true;
      } else
    #endif
    #if defined(PIN_WIRE_SDA)
      if (sda_pin == PIN_WIRE_SDA && scl_pin == PIN_WIRE_SCL) {
        // Use default configuration
        PORTMUX.TWIROUTEA = (PORTMUX.TWIROUTEA & 0xFC);
        return true;
      } else {
        // Assume default configuration
        PORTMUX.TWIROUTEA = (PORTMUX.TWIROUTEA & 0xFC);
        return false;
      }
    #else /* DD with 14 pins has no default pins in the "default" "position! Default to alt=2 */
      {
        PORTMUX.TWIROUTEA = (PORTMUX.TWIROUTEA & 0xFC) | 0x02;
        return false;
      }
    #endif
  #else // No TWI pin options - why call this?
    if (__builtin_constant_p(sda_pin) && __builtin_constant_p(scl_pin)) {
      /* constant case - error if there's no swap and the swap attempt is known at compile time */
      if (sda_pin != PIN_WIRE_SDA || scl_pin != PIN_WIRE_SCL) {
        badCall("This part does not support alternate Wire pins, if Wire.pins() is called, it must be passed the default pins");
        return false;
      } else {
        return true;
      }
    } else { /* Non-constant case */
      return (sda_pin == PIN_WIRE_SDA && scl_pin == PIN_WIRE_SCL);
    }
  #endif
}

bool TwoWire::swap(uint8_t state) {
  #if defined(PORTMUX_CTRLB) /* tinyAVR 0/1-series */
    #if (defined(PIN_WIRE_SDA_PINSWAP_1))
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
    #else //keep compiler happy
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
    #if defined(PIN_WIRE_SDA_PINSWAP_3)
      if (state == 3) {
        // Use pin swap
        PORTMUX.TWIROUTEA = (PORTMUX.TWIROUTEA & 0xFC) | 0x03;
        return true;
      } else
    #endif
    #if defined(PIN_WIRE_SDA_PINSWAP_3) && defined(PIN_WIRE_SCL_PINSWAP_3)
      if (state == 2) {
        // Use pin swap
        PORTMUX.TWIROUTEA = (PORTMUX.TWIROUTEA & 0xFC) | 0x02;
        return true;
      } else
    #endif
    #if (defined(PIN_WIRE_SDA) && defined(PIN_WIRE_SCL))
      if (state == 1) {
        // Use pin swap
        PORTMUX.TWIROUTEA = (PORTMUX.TWIROUTEA & 0xFC) | 0x01;
        return true;
      } else if (state == 0) {
        // Use default configuration
        PORTMUX.TWIROUTEA = (PORTMUX.TWIROUTEA & 0xFC);
        return true;
      } else {
        // Assume default configuration
        PORTMUX.TWIROUTEA = (PORTMUX.TWIROUTEA & 0xFC);
        return false;
      }
    #else
      {
        // Assume default configuration
        PORTMUX.TWIROUTEA = (PORTMUX.TWIROUTEA & 0xFC | 0x02);
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

void TwoWire::usePullups() {
      // make sure we don't get errata'ed - make sure their bits in the output registers are off!
  #ifdef DXCORE
    // if ((PORTMUX.TWIROUTEA & PORTMUX_TWI0_gm) == 0x02) {
    // below achieves same more efficiently, since only the master/slave pins are supported by Wire.h
    // and those are only ever on PA2/PA3, or PC2/PC3 for PORTMUX.TWIROUTEA & PORTMUX_TWI0_gm == 0x02.
    // but portToPortStruct takes a port number... and PC is 2 while PA is 0. So PORTMUX.TWIROUTEA& 0x02
    // is the number that portToPortStruct would want, directly, to get that all important port struct.
    // Slightly more complicated on DD-series since they added a fourth ooption to the portmux to help
    // with the constrained pinout.
    #ifndef __AVR_DD__
      PORT_t *port = portToPortStruct(PORTMUX.TWIROUTEA & 0x02);
      #else
      uint8_t temp = PORTMUX.TWIROUTEA & PORTMUX_TWI0_gm;
      PORT_t *port = portToPortStruct(temp==2?PC:PA);
      if (temp==3) {
        port->OUTCLR = 0x03; //bits 0 and 1
        port->PIN0CTRL |= PORT_PULLUPEN_bm;
        port->PIN1CTRL |= PORT_PULLUPEN_bm;
      } else {
    #endif
      port->OUTCLR = 0x0C; //bits 2 and 3
      port->PIN2CTRL |= PORT_PULLUPEN_bm;
      port->PIN3CTRL |= PORT_PULLUPEN_bm;
    #ifdef __AVR_DD__
    }
    #endif
  #else // megaTinyCore
    #if defined(PORTMUX_TWI0_bm)
      if ((PORTMUX.CTRLB & PORTMUX_TWI0_bm)) {
        PORTA.PIN2CTRL |= PORT_PULLUPEN_bm;
        PORTA.PIN1CTRL |= PORT_PULLUPEN_bm;
        PORTA.OUTCLR = 0x06;
      } else {
        PORTB.PIN1CTRL |= PORT_PULLUPEN_bm;
        PORTB.PIN0CTRL |= PORT_PULLUPEN_bm;
        PORTB.OUTCLR = 0x03; //bits 1 and 0.
      }
    #elif defined(__AVR_ATtinyxy2__)
      PORTA.PIN2CTRL |= PORT_PULLUPEN_bm;
      PORTA.PIN1CTRL |= PORT_PULLUPEN_bm;
      PORTA.OUTCLR = 0x06; // bits 2 and 1.
    #else
      PORTB.PIN1CTRL |= PORT_PULLUPEN_bm;
      PORTB.PIN0CTRL |= PORT_PULLUPEN_bm;
      PORTB.OUTCLR = 0x03; //bits 1 and 0.
    #endif
  #endif
}

// *INDENT-ON* The rest is okay to stylecheck
void TwoWire::begin(void) {
  rxBufferIndex = 0;
  rxBufferLength = 0;

  txBufferIndex = 0;
  txBufferLength = 0;

  TWI_MasterInit(DEFAULT_FREQUENCY);
}

void TwoWire::begin(uint8_t address, bool receive_broadcast, uint8_t second_address) {
  rxBufferIndex = 0;
  rxBufferLength = 0;

  txBufferIndex = 0;
  txBufferLength = 0;

  TWI_SlaveInit(address, receive_broadcast, second_address);

  TWI_attachSlaveTxEvent(onRequestService, txBuffer); // default callback must exist
  TWI_attachSlaveRxEvent(onReceiveService, rxBuffer, BUFFER_LENGTH); // default callback must exist

}

void TwoWire::begin(int address, bool receive_broadcast, uint8_t second_address) {
  begin((uint8_t)address, receive_broadcast, second_address);
}

void TwoWire::begin(uint8_t address, bool receive_broadcast) {
  begin(address, receive_broadcast, 0);
}

void TwoWire::begin(int address, bool receive_broadcast) {
  begin((uint8_t)address, receive_broadcast, 0);
}

void TwoWire::begin(uint8_t address) {
  begin(address, 0, 0);
}

void TwoWire::begin(int address) {
  begin((uint8_t)address, 0, 0);
}
void TwoWire::end(void) {
  TWI_Disable();
}

void TwoWire::setClock(uint32_t clock) {
  TWI_MasterSetBaud(clock);
}

uint8_t TwoWire::requestFrom(uint8_t address, uint8_t quantity, uint8_t sendStop) {
  if (quantity > BUFFER_LENGTH) {
    quantity = BUFFER_LENGTH;
  }

  uint8_t bytes_read = TWI_MasterRead(address, rxBuffer, quantity, sendStop);

  /* Initialize read variables */
  rxBufferIndex = 0;
  rxBufferLength = bytes_read;

  return bytes_read;
}
uint8_t TwoWire::requestFrom(uint8_t address, uint8_t quantity) {
  return requestFrom(address, quantity, (uint8_t)1);
}
// Translate "new" style of call to requestFrom()
uint8_t TwoWire::requestFrom(uint8_t address, size_t quantity) {
  return requestFrom(address, (uint8_t)quantity, (uint8_t)1);
}
// Translate "new" style of call with three args to traditional requestFrom()
uint8_t TwoWire::requestFrom(uint8_t address, size_t quantity, bool sendStop) {
  return requestFrom(address, (uint8_t)quantity, (uint8_t)sendStop);
}

uint8_t TwoWire::requestFrom(int address, int quantity) {
  return requestFrom((uint8_t)address, (uint8_t)quantity, (uint8_t)1);
}

uint8_t TwoWire::requestFrom(int address, int quantity, int sendStop) {
  return requestFrom((uint8_t)address, (uint8_t)quantity, (uint8_t)sendStop);
}

void TwoWire::beginTransmission(uint8_t address) {
  // indicate that we are transmitting
  transmitting = 1;
  // set address of targeted slave
  txAddress = address;
  // reset tx buffer iterator vars
  txBufferIndex = 0;
  txBufferLength = 0;
}

void TwoWire::beginTransmission(int address) {
  beginTransmission((uint8_t)address);
}

//
//  Originally, 'endTransmission' was an f(void) function.
//  It has been modified to take one parameter indicating
//  whether or not a STOP should be performed on the bus.
//  Calling endTransmission(false) allows a sketch to
//  perform a repeated start.
//
//  WARNING: Nothing in the library keeps track of whether
//  the bus tenure has been properly ended with a STOP. It
//  is very possible to leave the bus in a hung state if
//  no call to endTransmission(true) is made. Some I2C
//  devices will behave oddly if they do not see a STOP.
//
uint8_t TwoWire::endTransmission(bool sendStop) {
  // transmit buffer (blocking)
  uint8_t status = TWI_MasterWrite(txAddress, txBuffer, txBufferLength, sendStop);

  // reset tx buffer iterator vars
  txBufferIndex = 0;
  txBufferLength = 0;

  // indicate that we are done transmitting
  transmitting = 0;

  return status;
}

//  This provides backwards compatibility with the original
//  definition, and expected behaviour, of endTransmission
//
uint8_t TwoWire::endTransmission(void) {
  return endTransmission(true);
}

// must be called in:
// slave tx event callback
// or after beginTransmission(address)
size_t TwoWire::write(uint8_t data) {
  /* Check if buffer is full */
  if (txBufferLength >= BUFFER_LENGTH) {
    setWriteError();
    return 0;
  }

  /* Put byte in txBuffer */
  txBuffer[txBufferIndex] = data;
  txBufferIndex++;

  /* Update buffer length */
  txBufferLength = txBufferIndex;

  return 1;
}

// must be called in:
// slave tx event callback
// or after beginTransmission(address)
size_t TwoWire::write(const uint8_t *data, size_t quantity) {

  for (size_t i = 0; i < quantity; i++) {
    write(*(data + i));
  }

  return quantity;
}


// must be called in:
// slave rx event callback
// or after requestFrom(address, numBytes)
int TwoWire::available(void) {
  return rxBufferLength - rxBufferIndex;
}

// must be called in:
// slave rx event callback
// or after requestFrom(address, numBytes)
int TwoWire::read(void) {
  int value = -1;

  // get each successive byte on each call
  if (rxBufferIndex < rxBufferLength) {
    value = rxBuffer[rxBufferIndex];
    rxBufferIndex++;
  }

  return value;
}

// must be called in:
// slave rx event callback
// or after requestFrom(address, numBytes)
int TwoWire::peek(void) {
  int value = -1;

  if (rxBufferIndex < rxBufferLength) {
    value = rxBuffer[rxBufferIndex];
  }

  return value;
}

// can be used to get out of an error state in TWI module
// e.g. when MDATA register is written before MADDR
void TwoWire::flush(void) {
  //  /* Clear buffers */
  //  for(uint8_t i = 0; i < BUFFER_LENGTH; i++){
  //    txBuffer[i] = 0;
  //    rxBuffer[i] = 0;
  //  }
  //
  //  /* Clear buffer variables */
  //  txBufferIndex = 0;
  //  txBufferLength = 0;
  //  rxBufferIndex = 0;
  //  rxBufferLength = 0;
  //
  //  /* Turn off and on TWI module */
  //  TWI_Flush();
}

// behind the scenes function that is called when data is received
void TwoWire::onReceiveService(int numBytes) {
  // don't bother if user hasn't registered a callback
  if (!user_onReceive) {
    return;
  }
  // don't bother if rx buffer is in use by a master requestFrom() op
  // i know this drops data, but it allows for slight stupidity
  // meaning, they may not have read all the master requestFrom() data yet
  if (rxBufferIndex < rxBufferLength) {
    return;
  }

  // set rx iterator vars
  rxBufferIndex = 0;
  rxBufferLength = numBytes;

  // alert user program
  user_onReceive(numBytes);
}

// behind the scenes function that is called when data is requested
uint8_t TwoWire::onRequestService(void) {
  // don't bother if user hasn't registered a callback
  if (!user_onRequest) {
    return 0;
  }

  // reset slave write buffer iterator var
  txBufferIndex = 0;
  txBufferLength = 0;

  // alert user program
  user_onRequest();

  return txBufferLength;
}

// sets function called on slave write
void TwoWire::onReceive(void (*function)(int)) {
  user_onReceive = function;
}

// sets function called on slave read
void TwoWire::onRequest(void (*function)(void)) {
  user_onRequest = function;
}

// Preinstantiate Objects //////////////////////////////////////////////////////

TwoWire Wire = TwoWire();
