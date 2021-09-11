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
  Modified 2021 by MX682X for megaTinyCore and DxCore.
  Added Support for Simultaneous master/slave, dual mode and Wire1.
*/

extern "C" {
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
}


#include "Arduino.h"
#include "Wire.h"


extern "C" {    //compiler was complaining when I put twi.h into the upper C include part
#include "twi.h"
#include "twi_pins.h"
}

#ifndef DEFAULT_FREQUENCY
  #define DEFAULT_FREQUENCY 100000
#endif


// Initialize Class Variables //////////////////////////////////////////////////

// Constructors ////////////////////////////////////////////////////////////////
/**
 *@brief      TwoWire creates a Wire object
 *
 *@param      TWI_t *module - the pointer to the TWI module that the Wire object is supposed to use
 *
 *@return     constructor can't return anything
 */
TwoWire::TwoWire(TWI_t *twi_module) {
  vars._module = twi_module;
  vars.user_onRequest = NULL;  //Make sure to initialize this pointers
  vars.user_onReceive = NULL;  //This avoids weird jumps should something unexpected happen
}

// Public Methods //////////////////////////////////////////////////////////////
/**
 *@brief      pins changes the PINMUX to correspond to the desired pins
 *
 *
 *@param      uint8_t sda_pin is the desired pin for SDA
 *            uint8_t scl_pin is the desired pin for SCL
 *
 *@return     bool
 *@retval     true if change was successful
 */
bool TwoWire::pins(uint8_t sda_pin, uint8_t scl_pin) {
  #if defined (TWI1)
    if      (&TWI0 == vars._module)  {return TWI0_Pins(sda_pin, scl_pin);}
    else if (&TWI1 == vars._module)  {return TWI1_Pins(sda_pin, scl_pin);}
    else                             {return false;}
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
  #if defined (TWI1)
    if      (&TWI0 == vars._module) {return TWI0_swap(state);}
    else if (&TWI1 == vars._module) {return TWI1_swap(state);}
    else                            {return false;}
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
  #if defined (TWI1)
    if      (&TWI0 == vars._module) {TWI0_usePullups();}
    else if (&TWI1 == vars._module) {TWI1_usePullups();}
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
  #if defined (TWI1)
    #if defined (USING_TWI1)
      badCall("swapModule() can only be used if TWI1 is not used");
    #else
      if (vars._module->MCTRLA == 0) {   //slave and master initialisations enable MCTRLA, so just check for that
        vars._module = twi_module;
        return true;         //Success
      }
      #endif
  #else
      badCall("Only one TWI module available, nothing to switch with");
      (void)twi_module; //Remove warning unused variable
  #endif
  return false;
}


/**
 *@brief      begin (w/o parameters) starts initializes the master operation of the TWI
 *
 *@param      void
 *
 *@return     void
 */
void TwoWire::begin(void) {
  TWI_MasterInit(&vars);
  TWI_MasterSetBaud(&vars, DEFAULT_FREQUENCY);
}


/**
 *@brief      begin (w/ parameters) changes the TWI module if only one Wire object is used
 *
 *            Works only if the TWI is disabled
 *
 *
 *@param      uint8_t address - the desired address for the slave module
 *            bool receive_broadcast - if true, enables a response on the 0x00 call
 *            uint8_t second_address holds the data for the SADDRMASK register. If the LSB is '1'
 *              the TWI handles the 7 MSB as a second address for the slave, otherwise the 7 MSB
 *              act as a bit mask, that disables the check on the corresponding SADDR bit.
 *
 *@return     void
 */
void TwoWire::begin(uint8_t address, bool receive_broadcast, uint8_t second_address) {
  TWI_SlaveInit(&vars, address, receive_broadcast, second_address);
  TWI_RegisterSlaveISRcallback(onSlaveIRQ);                          //give the C part of the program a pointer to call back to.
}


/**
 *@brief      setClock sets the baud register to get the desired frequency
 *
 *            Has only an effect when used after begin(void)
 *
 *@param      uint32_t clock - the desired clock in Hertz
 *
 *@return     void
 */
void TwoWire::setClock(uint32_t clock) {
  TWI_MasterSetBaud(&vars, clock);
}


/**
 *@brief      end disables the TWI master and slave
 *
 *@param      void
 *
 *@return     void
 */
void TwoWire::end(void) {
  TWI_Disable(&vars);
}


/**
 *@brief      endMaster disables the TWI master
 *
 *@param      void
 *
 *@return     void
 */
#if defined (TWI_MANDS)
void TwoWire::endMaster(void) {
  TWI_DisableMaster(&vars);
}
#endif


/**
 *@brief      endSlave disables the TWI slave
 *
 *@param      void
 *
 *@return     void
 */
#if defined (TWI_MANDS)
void TwoWire::endSlave(void) {
  TWI_DisableSlave(&vars);
}
#endif



/**
 *@brief      requestFrom sends a master READ with the specified slave address
 *
 *            When a greater quantity then the BUFFER_LENGTH is passed, the quantity gets
 *            limited to the BUFFER_LENGTH.
 *            Received Bytes must be read with read().
 *
 *@param      int/uint8_t address - the address of the slave
 *            int/uint8_t/size_t quantity - the amount of bytes that are expected to be received
 *            int/bool sendStop - if the transaction should be terminated with a STOP condition
 *
 *@return     uint8_t
 *@retval     amount of bytes that were actually read. If 0, no read took place due to a bus error.
 */
uint8_t TwoWire::requestFrom(uint8_t address, uint8_t quantity)                   {return requestFrom((uint8_t)address, (uint8_t)quantity, (uint8_t)1);}
uint8_t TwoWire::requestFrom(uint8_t address, size_t  quantity, bool    sendStop) {return requestFrom((uint8_t)address, (uint8_t)quantity, (uint8_t)sendStop);}
uint8_t TwoWire::requestFrom(uint8_t address, size_t  quantity)                   {return requestFrom((uint8_t)address, (uint8_t)quantity, (uint8_t)1);}
uint8_t TwoWire::requestFrom(int     address, int     quantity, int     sendStop) {return requestFrom((uint8_t)address, (uint8_t)quantity, (uint8_t)sendStop);}
uint8_t TwoWire::requestFrom(int     address, int     quantity)                   {return requestFrom((uint8_t)address, (uint8_t)quantity, (uint8_t)1);}

uint8_t TwoWire::requestFrom(uint8_t address, uint8_t quantity, uint8_t sendStop) {
  if (quantity > BUFFER_LENGTH) {
    quantity = BUFFER_LENGTH;
  }

  vars._slaveAddress = address << 1;

  return TWI_MasterRead(&vars, quantity, sendStop);
}


/**
 *@brief      beginTransmission prepares the Wire object for a master WRITE.
 *
 *            This function only saves the slave address in the structure, it does
 *            not perform any transmissions.
 *            a write() will fill the transmit buffer. write() has to be called after
 *            beginTransmission() was called
 *
 *@param      uint8_t address - the address of the slave
 *
 *@return     void
 */
void TwoWire::beginTransmission(uint8_t address) {
  #if defined (TWI_MERGE_BUFFERS)                 //Same Buffers for tx/rx
    uint8_t* txHead  = &(vars._trHead);
    uint8_t* txTail  = &(vars._trTail);
  #else                                           //Separate tx/rx Buffers
    uint8_t* txHead  = &(vars._txHead);
    uint8_t* txTail  = &(vars._txTail);
  #endif
  // set address of targeted slave
  vars._slaveAddress = address << 1;
  (*txTail) = (*txHead);  //reset transmitBuffer
}


/**
 *@brief      endTransmission is the function that actually performs the (blocking) master WRITE
 *
 *            Originally, 'endTransmission' was an f(void) function. It has been modified to take
 *            one parameter indicating whether or not a STOP should be performed on the bus.
 *            Calling endTransmission(false) allows a sketch to perform a repeated start.
 *
 *            WARNING: Nothing in the library keeps track of whether the bus tenure has been
 *            properly ended with a STOP. It is very possible to leave the bus in a hung state if
 *            no call to endTransmission(true) is made. Some I2C devices will behave oddly
 *            if they do not see a STOP. Other masters won't be able to issue their START for example.
 *
 *@param      bool sendStop - if the transaction should be terminated with a STOP condition
 *
 *
 *@return     uint8_t
 *@retval     amount of bytes that were actually written. If it differs from the amount that was expected
 *              an error might have occurred.
 */
uint8_t TwoWire::endTransmission(bool sendStop) {
  // transmit (blocking)
  return TWI_MasterWrite(&vars, sendStop);
}



/**
 *@brief      write fills the transmit buffers, master or slave depending on when it is called
 *
 *            Usually, the function fills the master transmit buffer.
 *            If called inside the specified onReceive or onRequest functions, the slave buffer will be filled
 *
 *@param      uint8_t data - byte to put into the buffer
 *
 *
 *@return     uint8_t
 *@retval     1 if successful, 0 if the buffer is full
 */
size_t TwoWire::write(uint8_t data) {

  uint8_t nextHead;
  uint8_t* txHead;
  uint8_t* txTail;
  uint8_t* txBuffer;

  #if defined (TWI_MANDS)                           //Add following if master and slave are split
    if (vars._bools._toggleStreamFn == 0x01) {
      #if defined (TWI_MERGE_BUFFERS)                 //Separate tx/rx Buffers
        txHead  = &(vars._trHeadS);
        txTail  = &(vars._trTailS);
        txBuffer = vars._trBufferS;
      #else                                           //Same Buffers for tx/rx
        txHead  = &(vars._txHeadS);
        txTail  = &(vars._txTailS);
        txBuffer = vars._txBufferS;
      #endif
    }
    else
  #endif
  {
    #if defined (TWI_MERGE_BUFFERS)               //Same Buffers for tx/rx
      txHead  = &(vars._trHead);
      txTail  = &(vars._trTail);
      txBuffer = vars._trBuffer;
    #else                                         //Separate tx/rx Buffers
      txHead  = &(vars._txHead);
      txTail  = &(vars._txTail);
      txBuffer = vars._txBuffer;
    #endif
  }

  /* Put byte in txBuffer */
  nextHead = TWI_advancePosition(*txHead);

  if (nextHead == (*txTail)) return 0;          //Buffer full, stop accepting data

  txBuffer[(*txHead)] = data;             //Load data into the buffer
  (*txHead) = nextHead;                   //advancing the head

  return 1;
}


/**
 *@brief      write for arrays
 *
 *            calls the write function in a for-loop
 *
 *@param      uint8_t *data - pointer to the array
 *            size_t quantity - amount of bytes to copy
 *
 *
 *@return     uint8_t
 *@retval     amount of bytes copied
 */
size_t TwoWire::write(const uint8_t *data, size_t quantity) {

  for (size_t i = 0; i < quantity; i++) {
    write(*(data + i));
  }

  return quantity;
}



/**
 *@brief      available returns the amount of bytes that are available to read in the master or slave buffer
 *
 *            Usually, the function returns the amount of bytes to read in the master buffer.
 *            If called inside the specified onReceive or onRequest functions,
 *            it returns the amount of bytes from the slave buffer
 *
 *@param      void
 *
 *@return     int
 *@retval     amount of bytes available to read from the master buffer
 */
int TwoWire::available(void) {
  return TWI_Available(&vars);
}



/**
 *@brief      read returns a byte from the master or slave buffer and removes it from there
 *
 *            Usually, the function returns the byte from the master buffer.
 *            If called inside the specified onReceive or onRequest functions,
 *            it returns the byte from the slave buffer
 *
 *@param      void
 *
 *@return     int
 *@retval     byte in the buffer or -1 if buffer is empty
 */
int TwoWire::read(void) {

  uint8_t* rxHead;
  uint8_t* rxTail;
  uint8_t* rxBuffer;

  #if defined (TWI_MANDS)                           //Add following if master and slave are split
    if (vars._bools._toggleStreamFn == 0x01) {
      #if defined (TWI_MERGE_BUFFERS)                 //Same Buffers for tx/rx
        rxHead  = &(vars._trHeadS);
        rxTail  = &(vars._trTailS);
        rxBuffer = vars._trBufferS;
      #else                                           //Separate tx/rx Buffers
        rxHead  = &(vars._rxHeadS);
        rxTail  = &(vars._rxTailS);
        rxBuffer = vars._rxBufferS;
      #endif
    }
    else
  #endif
  {
    #if defined (TWI_MERGE_BUFFERS)               //Same Buffers for tx/rx
      rxHead  = &(vars._trHead);
      rxTail  = &(vars._trTail);
      rxBuffer = vars._trBuffer;
    #else                                         //Separate tx/rx Buffers
      rxHead  = &(vars._rxHead);
      rxTail  = &(vars._rxTail);
      rxBuffer = vars._rxBuffer;
    #endif
  }


  if ((*rxHead) == (*rxTail)) { // if the head isn't ahead of the tail, we don't have any characters
    return -1;
  }
  else {
    uint8_t c = rxBuffer[(*rxTail)];
    (*rxTail) = TWI_advancePosition(*rxTail);
    return c;
  }
}


/**
 *@brief      peek returns a byte from the master or slave buffer but does not remove it
 *
 *            Usually, the function returns the byte from the master buffer.
 *            If called inside the specified onReceive or onRequest functions,
 *            it returns the byte from the slave buffer
 *
 *@param      void
 *
 *@return     int
 *@retval     byte in the buffer or -1 if buffer is empty
 */
int TwoWire::peek(void) {

  uint8_t* rxHead;
  uint8_t* rxTail;
  uint8_t* rxBuffer;

  #if defined (TWI_MANDS)                           //Add following if master and slave are split
    if (vars._bools._toggleStreamFn == 0x01) {
      #if defined (TWI_MERGE_BUFFERS)                 //Separate tx/rx Buffers
        rxHead  = &(vars._trHeadS);
        rxTail  = &(vars._trTailS);
        rxBuffer = vars._trBufferS;
      #else                                           //Same Buffers for tx/rx
        rxHead  = &(vars._rxHeadS);
        rxTail  = &(vars._rxTailS);
        rxBuffer = vars._rxBufferS;
      #endif
    }
    else
  #endif
  {
    #if defined (TWI_MERGE_BUFFERS)               //Same Buffers for tx/rx
      rxHead  = &(vars._trHead);
      rxTail  = &(vars._trTail);
      rxBuffer = vars._trBuffer;
    #else                                         //Separate tx/rx Buffers
      rxHead  = &(vars._rxHead);
      rxTail  = &(vars._rxTail);
      rxBuffer = vars._rxBuffer;
    #endif
  }

  if ((*rxHead) == (*rxTail)) {return -1;}
  else                        {return rxBuffer[(*rxTail)];}
}


/**
 *@brief      flush resets the master and slave buffers and restarts the TWI module
 *
 *@param      void
 *
 *@return     void
 */
void TwoWire::flush(void) {
  #if defined (TWI_MERGE_BUFFERS)               //merged tx/rx Buffers
    vars._trTail = vars._trHead;
    #if defined (TWI_MANDS)
      vars._trTailS = vars._trHeadS;
    #endif
  #else
    vars._rxTail = vars._rxHead;
    vars._txTail = vars._txHead;
    #if defined (TWI_MANDS)
      vars._rxTailS = vars._rxHeadS;
      vars._txTailS = vars._txHeadS;
    #endif
  #endif

  /* Turn off and on TWI module */
  TWI_Flush(&vars);
}

/**
 *@brief      getIncomingAddress returns the last address the slave has reacted to
 *
 *            When used in master only mode, it will return the slaveAddress
 *            that was set by the user in beginTransmission()
 *
 *@param      void
 *
 *@return     uint8_t
 *@retval     left-shifted I2C address with the write/read bit
 */
uint8_t TwoWire::getIncomingAddress(void) {
  #if defined (TWI_MANDS)                         //Alias handler
    return vars._incomingAddress;
  #else
    return vars._slaveAddress;
  #endif
}


/**
 *@brief      enableDualMode enables the splitting of master and slave pins
 *
 *            useful when you want to separate multiple TWI buses.
 *            Only available on the chips with a bigger pin count. See data sheet.
 *
 *@param      bool fmp_enable - set true if the TWI module has to expect a high
 *              frequency on the salve pins
 *
 *@return     void
 */
#if defined (TWI_DUALCTRL)
void TwoWire::enableDualMode(bool fmp_enable) {
  vars._module->DUALCTRL = ((fmp_enable << TWI_FMPEN_bp) | TWI_ENABLE_bm);
}
#endif





/**
 *@brief      onSlaveIRQ is called by the interrupts and calls the interrupt handler
 *
 *            Another little hack I had to do: This function is static, thus there is no extra copy
 *            when a new Wire object, like Wire1 is initialized. When I first wrote this function
 *            I was using Wire.vars.module and Wire1.vars.module to figure out which pointer to pass,
 *            but this made the compiler create a Wire1 object in some cases, where Wire1 was never used
 *            by the user. So I rewrote this function with the though that if the module can be different,
 *            there is just one Wire object, so the code doesn't have to check if Wire is using TWI0 or TWI1
 *
 *
 *@param      TWI_t *module - the pointer to the TWI module
 *
 *@return     void
 */
void TwoWire::onSlaveIRQ(TWI_t *module){                 //This function is static and is, thus, the only one for both
                                                         //Wire interfaces. Here is decoded which interrupt was fired.

#if defined (TWI1)                                       //Two TWIs available
   #if defined (USING_WIRE1)                             //User wants to use Wire and Wire1. Need to check the interface
      if      (module == &TWI0) {
         TWI_HandleSlaveIRQ(&(Wire.vars));
      }
      else if (module == &TWI1) {
         TWI_HandleSlaveIRQ(&(Wire1.vars));
      }
   #else                                                 //User uses only Wire but can use TWI0 and TWI1
       TWI_HandleSlaveIRQ(&(Wire.vars));                 //Only one possible SlaveIRQ source/Target Class
   #endif
#else                                                    //Only TWI0 available, IRQ can only have been issued by that interface
   TWI_HandleSlaveIRQ(&(Wire.vars));                     //No need to check for it
#endif
   (void)module;
}



/**
 *@brief      onReceive saves the pointer to the desired function to call on master WRITE / slave READ.
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
 *@brief      onRequest saves the pointer to the desired function to call on master READ / slave WRITE.
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





/**
 *  Wire object constructors with the default TWI modules.
 *  If there is absolutely no way to swap the pins physically,
 *  here is the best way to do so in software.
 */
#if defined(TWI0)
  TwoWire Wire(&TWI0);
#endif

#if defined (TWI1)
#if defined (USING_WIRE1)
   TwoWire Wire1(&TWI1);
#endif
#endif