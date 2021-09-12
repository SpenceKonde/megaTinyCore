/*
  Copyright (c) 2021 MX682X

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/


#include "Arduino.h"
#include "twi.h"
#include "twi_pins.h"

/*
  Variable that holds the pointer to the static function "onSlaveIRQ"
  in the TwoWire class, if the slave functionality is used.
*/
static void (*TWI_onSlaveISR)(TWI_t *module) __attribute__((unused));



// "Private" function declaration
void NotifyUser_onRequest(struct twiData *_data);
void NotifyUser_onReceive(struct twiData *_data);

void TWI_SlaveInterruptHandler(TWI_t *module);


// Function definitions
/**
 *@brief      TWI_MasterInit Initializes TWI master operation if not already initialized
 *
 *@param      struct twiData *_data is a pointer to the structure that holds the variables
 *              of a Wire object. Following struct elements are used:
 *              _bools._masterEnabled
 *              _bools._slaveEnabled
 *              _module
 *
 *@return     void
 */
void TWI_MasterInit(struct twiData *_data) {

  #if defined(TWI_MANDS)                            // Check if the user wants to use Master AND Slave
    if (_data->_bools._masterEnabled == 1) {        // Slave is allowed to be enabled, don't re-enable the master though
      return;
    }
  #else                                             // Master OR Slave
    if (_data->_bools._masterEnabled == 1 ||        // If Master was enabled
        _data->_bools._slaveEnabled  == 1) {        // or Slave was enabled
      return;                                       // return and do nothing
    }
  #endif


  #if defined(TWI1)                                 // More then one TWI used
    if      (&TWI0 == _data->_module) {             // check which one this function is working with
      TWI0_ClearPins();
    } else if (&TWI1 == _data->_module) {
      TWI1_ClearPins();
    }
  #else                                             // Only one TWI is used
    TWI0_ClearPins();                               // Only one option is possible
  #endif

  _data->_bools._masterEnabled  = 1;
  _data->_module->MCTRLA        = TWI_ENABLE_bm;  // Master Interrupt flags stay disabled
  _data->_module->MSTATUS       = TWI_BUSSTATE_IDLE_gc;
}


/**
 *@brief      TWI_SlaveInit Initializes TWI slave operation if not already initialized
 *
 *@param      struct twiData *_data is a pointer to the structure that holds the variables
 *              of a Wire object. Following struct elements are used in this function:
 *                _bools._masterEnabled
 *                _bools._slaveEnabled
 *                _module
 *@param      uint8_t address holds the primary address that this TWI slave should listen to
 *            uint8_t receive_broadcast if true, instructs the TWI slave to react to the
 *              general TWI call 0x00
 *            uint8_t second_address holds the data for the SADDRMASK register. If the LSB is '1'
 *              the TWI handles the 7 MSB as a second address for the slave, otherwise the 7 MSB
 *              act as a bit mask, that disables the check on the corresponding SADDR bit.
 *
 *@return     void
 */
void TWI_SlaveInit(struct twiData *_data, uint8_t address, uint8_t receive_broadcast, uint8_t second_address) {

  #if defined(TWI_MANDS)                      // Check if the user wants to use Master AND Slave
    if (_data->_bools._slaveEnabled  == 1) {  // Master is allowed to be enabled, don't re-enable the slave though
      return;
    }
  #else                                       // Master or Slave
    if (_data->_bools._masterEnabled == 1 ||  // If Master was enabled
        _data->_bools._slaveEnabled  == 1) {  // or Slave was enabled
    return;                                   // return and do nothing
    }
  #endif

  #if defined(TWI1)
    if (&TWI0 == _data->_module) {
      TWI0_ClearPins();
    } else if (&TWI1 == _data->_module) {
      TWI1_ClearPins();
    }
  #else
    TWI0_ClearPins();
  #endif

  _data->_bools._slaveEnabled = 1;
  _data->_module->SADDR       = address << 1 | receive_broadcast;
  _data->_module->SADDRMASK   = second_address;
  _data->_module->SCTRLA      = TWI_DIEN_bm | TWI_APIEN_bm | TWI_PIEN_bm  | TWI_ENABLE_bm;

  /* Bus Error Detection circuitry needs Master enabled to work */
  _data->_module->MCTRLA = TWI_ENABLE_bm;
}


/**
 *@brief      TWI_Flush clears the internal state of the master and changes the bus state to idle
 *
 *@param      struct twiData *_data is a pointer to the structure that holds the variables
 *              of a Wire object. Following struct elements are used in this function:
 *                _module
 *
 *@return             void
 */
void TWI_Flush(struct twiData *_data) {
  _data->_module->MCTRLB |= TWI_FLUSH_bm;
}


/**
 *@brief      TWI_Disable disables the TWI master and slave
 *
 *@param      struct twiData *_data is a pointer to the structure that holds the variables
 *              of a Wire object. Following struct elements are used in this function:
 *                _module
 *
 *@return     void
 */
void TWI_Disable(struct twiData *_data) {
  TWI_DisableMaster(_data);
  TWI_DisableSlave(_data);
}


/**
 *@brief      TWI_DisableMaster disables the TWI master
 *
 *@param      struct twiData *_data is a pointer to the structure that holds the variables
 *              of a Wire object. Following struct elements are used in this function:
 *                _bools._masterEnabled
 *                _module
 *
 *@return     void
 */
void TWI_DisableMaster(struct twiData *_data) {
  if (true == _data->_bools._masterEnabled) {
    if (false == _data->_bools._slaveEnabled) {
      _data->_module->MCTRLA    = 0x00;  // has to stay enabled for bus error circuitry
    }
  _data->_module->MBAUD         = 0x00;
  _data->_bools._masterEnabled  = 0x00;
  }
}


/**
 *@brief      TWI_DisableSlave disables the TWI slave
 *
 *@param      struct twiData *_data is a pointer to the structure that holds the variables
 *              of a Wire object. Following struct elements are used in this function:
 *                _bools._slaveEnabled
 *                _module
 *@return     void
 */
void TWI_DisableSlave(struct twiData *_data) {
  if (true == _data->_bools._slaveEnabled) {
    if (false == _data->_bools._masterEnabled) {
      _data->_module->MCTRLA    = 0x00;      // might be enabled for bus error circuitry
    }
    _data->_module->SADDR       = 0x00;
    _data->_module->SCTRLA      = 0x00;
    _data->_module->SADDRMASK   = 0x00;
    _data->_bools._slaveEnabled = 0x00;
    #if defined(TWI_DUALCTRL)
      _data->_module->DUALCTRL  = 0x00;    // Disable pin splitting when available
    #endif
  }
}



/**
 *@brief      TWI_MasterSetBaud sets the baud register to get the desired frequency
 *
 *            After checking if the master is actually enabled, the new baud is calculated.
 *              Then it is compared to the old baud. Only if they differ, the master is disabled,
 *              the baud register updated, and the master re-enabled
 *
 *@param      struct twiData *_data is a pointer to the structure that holds the variables
 *              of a Wire object. Following struct elements are used in this function:
 *              _bools._masterEnabled
 *              _module
 *
 *@return     void
 */
void TWI_MasterSetBaud(struct twiData *_data, uint32_t frequency) {
  if (_data->_bools._masterEnabled == 1) {                // Do something only if the master is enabled.
    uint8_t newBaud = TWI_MasterCalcBaud(frequency);      // get the new Baud value
    uint8_t oldBaud = _data->_module->MBAUD;              // load the old Baud value
    if (newBaud != oldBaud) {                             // compare both, in case the code is issuing this before every transmission.
      uint8_t restore = _data->_module->MCTRLA;           // Save the old Master state
      _data->_module->MCTRLA    = 0;                         // Disable Master
      _data->_module->MBAUD     = newBaud;                    // update Baud register
      if (frequency >= 600000) {
        _data->_module->CTRLA  |=  TWI_FMPEN_bm;           // Enable FastMode+
      } else {
        _data->_module->CTRLA  &= ~TWI_FMPEN_bm;           // Disable FastMode+
      }
      _data->_module->MCTRLA    = restore;                  // restore the old register, thus enabling it again
      _data->_module->MSTATUS   = TWI_BUSSTATE_IDLE_gc;     // Force the state machine into Idle according to the data sheet
      }
   }
}


/**
 *@brief      TWI_Available returns the amount of bytes that are available to read in the master or slave buffer
 *
 *            This function is placed in this file because the slave interrupt handler needs it too.
 *            This file has no concept of the Wire object.
 *            In MANDS mode, when called from
 *            user_onRequest() or user_onReceive() it will return the number from the slave buffer
 *            due to the _toggleStreamFn flag
 *
 *@param      struct twiData *_data is a pointer to the structure that holds the variables
 *              of a Wire object. Following struct elements are used in this function:
 *                _bools._toggleStreamFn
 *                _rxHead(S)
 *                _rxTail(S)
 *
 *@return     uint8_t
 *@retval     amount of bytes available to read from the master or slave buffer
 */
uint8_t TWI_Available(struct twiData *_data) {
  uint16_t num;
  uint8_t* rxHead;
  uint8_t* rxTail;

  #if defined(TWI_MANDS)                          // Add following if master and slave are split
    if (_data->_bools._toggleStreamFn == 0x01) {
      #if defined(TWI_MERGE_BUFFERS)              // Same Buffers for tx/rx
        rxHead  = &(_data->_trHeadS);
        rxTail  = &(_data->_trTailS);
      #else                                       // Separate tx/rx Buffers
        rxHead  = &(_data->_rxHeadS);
        rxTail  = &(_data->_rxTailS);
      #endif
    } else
  #endif
  {
    #if defined(TWI_MERGE_BUFFERS)                // Same Buffers for tx/rx
      rxHead  = &(_data->_trHead);
      rxTail  = &(_data->_trTail);
    #else                                         // Separate tx/rx Buffers
      rxHead  = &(_data->_rxHead);
      rxTail  = &(_data->_rxTail);
    #endif
  }

  num = (BUFFER_LENGTH + (*rxHead) - (*rxTail));

  #if defined(BUFFER_NOT_POWER_2)
    if (num <  BUFFER_LENGTH) {
      return  num;
    } else {
      return (num - BUFFER_LENGTH);
    }
  #else
    return (num & (BUFFER_LENGTH - 1));             // a bitwise AND is more space-efficient but needs power of 2 buffer lengths
  #endif
}


/**
 *@brief      TWI_MasterWrite performs a master write operation on the TWI bus
 *
 *            As soon as the bus is in an idle state, a polled write operation is performed
 *            A STOP condition can be send at the end, or not if a REP START is wanted
 *            The user has to make sure to have a master write or read at the end with a STOP
 *
 *
 *
 *@param      struct twiData *_data is a pointer to the structure that holds the variables
 *              of a Wire object. Following struct elements are used in this function:
 *                _txBuffer[]
 *                _txHead
 *                _txTail
 *            bool send_stop enables the STOP condition at the end of a write
 *
 *@return     uint8_t
 *@retval     amount of bytes that were written. If 0, no write took place, either due
 *            to an error or because of an empty txBuffer
 */
uint8_t TWI_MasterWrite(struct twiData *_data, bool send_stop)  {
  #if defined(TWI_MERGE_BUFFERS)                              // Same Buffers for tx/rx
    uint8_t* txHead   = &(_data->_trHead);
    uint8_t* txTail   = &(_data->_trTail);
    uint8_t* txBuffer =   _data->_trBuffer;
  #else                                                       // Separate tx/rx Buffers
    uint8_t* txHead   = &(_data->_txHead);
    uint8_t* txTail   = &(_data->_txTail);
    uint8_t* txBuffer =   _data->_txBuffer;
  #endif

  if ((_data->_module->MSTATUS & TWI_BUSSTATE_gm) == TWI_BUSSTATE_UNKNOWN_gc) {
    return 0;                                                 // If the bus was not initialized, return
  }

  beginning:                                                  // Position to restart in case of an Arbitration error
  while ((_data->_module->MSTATUS & TWI_BUSSTATE_gm) == TWI_BUSSTATE_BUSY_gc);    // Wait for Bus to be free again

  uint8_t dataWritten = 0;
  uint8_t writeAddress = ADD_WRITE_BIT(_data->_slaveAddress); // Get slave address and clear the read bit
  _data->_module->MADDR = writeAddress;                       // write to the ADDR Register -> (repeated) Start condition is issued and slave address is sent

  while (!(_data->_module->MSTATUS & TWI_WIF_bm));            // Wait for the address/data transfer completion   // ...thinking about adding a timeout...
  if (_data->_module->MSTATUS & TWI_ARBLOST_bm) {             // If another Master has started writing an Address at the same time, go back and wait
    goto beginning;                                           // Until the TWI bus has changed from BUSY to IDLE
  }

  // if the slave has acknowledged the address, data can be sent
  while (true) {                                                // Do the following until a break
    uint8_t currentStatus = _data->_module->MSTATUS;            // get a local copy of the Status for easier access

    if (currentStatus & (TWI_ARBLOST_bm | TWI_BUSERR_bm)) {     // Check for Bus error (state M4 in datasheet)
      return 0;                                                 // abort operation
    }

    if (currentStatus & TWI_RXACK_bm) {                         // Address/Data was not Acknowledged (state M3 in datasheet)
      send_stop = true;                                         // make sure to send a stop bit to free the bus
      break;                                                    // break the loop and skip to the end
    }

    if (dataWritten > 0) {                                      // This if condition should run every time except in the first iteration
      (*txTail) = TWI_advancePosition(*txTail);
    }

    if ((*txHead) != (*txTail)) {                               // check if there is data to be written
      _data->_module->MDATA = txBuffer[(*txTail)];              // Writing to the register to send data
      dataWritten++;
    } else {                                                    // No data left to be written
      break;                                                    // so break the while loop
    }

    while (!(_data->_module->MSTATUS & TWI_WIF_bm));            // Wait for the address/data transfer completion
  }

  if (send_stop) {
     _data->_module->MCTRLB = TWI_MCMD_STOP_gc;                 // Send stop
  }
  return dataWritten;
}


/**
 *@brief      TWI_MasterRead performs a master read operation on the TWI bus
 *
 *            As soon as the bus is in an idle state, a polled read operation is performed
 *            A STOP condition can be send at the end, or not if a REP START is wanted
 *            The user has to make sure to have a master write or read at the end with a STOP
 *
 *
 *
 *@param      struct twiData *_data is a pointer to the structure that holds the variables
 *              of a Wire object. Following struct elements are used in this function:
 *                _rxBuffer[]
 *                _rxHead
 *                _rxTail
 *
 *            uint8_t bytesToRead is the desired amount of bytes to read. When finished, a
 *              NACK is issued.
 *            bool send_stop enables the STOP condition at the end of a write
 *
 *@return     uint8_t
 *@retval     amount of bytes that were actually read. If 0, no read took place due to a bus error
 */
uint8_t TWI_MasterRead(struct twiData *_data, uint8_t bytesToRead, bool send_stop) {
  #if defined(TWI_MERGE_BUFFERS)                                // Same Buffers for tx/rx
    uint8_t* rxHead   = &(_data->_trHead);
    uint8_t* rxTail   = &(_data->_trTail);
    uint8_t* rxBuffer =   _data->_trBuffer;
  #else                                                         // Separate tx/rx Buffers
    uint8_t* rxHead   = &(_data->_rxHead);
    // uint8_t* rxTail   = &(_data->_rxTail);
    uint8_t* rxBuffer =   _data->_rxBuffer;
  #endif


  uint8_t retVal = 0;
  uint8_t dataRead = 0;
  uint8_t currentStatus = _data->_module->MSTATUS;

  #if defined(TWI_MERGE_BUFFERS)                                // if single Buffer operation
    (*rxTail) = (*rxHead);                                      // reset buffer positions so the slave can start writing at zero.
  #endif

  if ((currentStatus & TWI_BUSSTATE_gm) == TWI_BUSSTATE_UNKNOWN_gc) { // Bus de-initialized for some reason
    return retVal;                                              // return 0
  }
  while ((currentStatus & TWI_BUSSTATE_gm) == TWI_BUSSTATE_BUSY_gc);  // Wait if another master is using the bus

  uint8_t readAddress = ADD_READ_BIT(_data->_slaveAddress);     // Get slave address and set the read bit
  _data->_module->MADDR = readAddress;                          // write to the ADDR Register -> (repeated) Start condition is issued and slave address is sent

  while (!(_data->_module->MSTATUS & TWI_RIF_bm));             // Wait for the address/data receive completion

  if (_data->_module->MSTATUS & TWI_RXACK_bm) {                 // Address was not Acknowledged (state M3 in data sheet)
    send_stop = true;                                           // Terminate the transaction, retVal is still '0'
    goto theEnd;                                                // skip the loop and go directly to the end
  }

  while (true) {
    uint8_t currentStatus = _data->_module->MSTATUS;            // get a local copy of the Status for easier access
    if (currentStatus & (TWI_ARBLOST_bm | TWI_BUSERR_bm)) {     // Check for Bus error (state M4 in data sheet)
      return 0;                                                 // abort operation
    }

    if (dataRead > (BUFFER_LENGTH-1)) {                         // Buffer overflow with this incoming Byte
      send_stop = true;                                         // make sure to end the transaction
      retVal = dataRead;                                        // prepare to return the amount of received bytes
      goto theEnd;                                              // break the loop and skip to the end
    }

    uint8_t data = _data->_module->MDATA;                       // Data is fine and we have space, so read out the data register
    rxBuffer[(*rxHead)] = data;                                 // and save it in the Buffer.

    (*rxHead) = TWI_advancePosition(*rxHead);

    dataRead++;                                                 // Byte is read
    if (dataRead < bytesToRead) {                               // expecting more bytes, so
      _data->_module->MCTRLB = TWI_MCMD_RECVTRANS_gc;           // send an ACK so the Slave so it can sends the next byte
    } else {                                                    // Otherwise,
      retVal = dataRead;                                        // prepare to return the amount of received bytes
      break;                                                    // Break the loop, continue with the NACK and, if requested, STOP
    }

    while (!(_data->_module->MSTATUS & TWI_RIF_bm));            // Wait for the address/data receive interrupt flag
  }

  theEnd:
  if (send_stop) {                                              // if requested
    _data->_module->MCTRLB = TWI_ACKACT_bm | TWI_MCMD_STOP_gc;  // send STOP (+NACK)
  }
  return retVal;                                                // return the amount of bytes read
}


/**
 *@brief      TWI_RegisterSlaveISRcallback is used to save the callback to the onSlaveIRQ() function
 *
 *            Since this file has no idea of Wire and Wire1 objects, we need to tell the address of a
 *            static function that has an idea about that objects. This is done here.
 *            Might issue a badArg when optimization is disabled or someone messed up the slave begin
 *            in the Wire.cpp file
 *
 *
 *@param      void (*function)(TWI_t *module)
 *              (*function)     - the name of the function that returns void
 *              (TWI_t *module) - and accepts the pointer to a TWI module as a parameter
 *
 *@return     void
 */
void TWI_RegisterSlaveISRcallback(void (*function)(TWI_t *module)) {
   if (__builtin_constant_p(function)) {
    if (function == NULL) {
      badArg("Null pointer passed as ISR callback");
    } else {
      TWI_onSlaveISR = function;
    }
   } else {
     if (function != NULL) {
      TWI_onSlaveISR = function;
    }
  }
}


/**
 *@brief      TWI_SlaveInterruptHandler is called from the TWI slave interrupt vectors
 *
 *            This function calls the static function in Wire.cpp that figures out the pointer
 *            to the Wire object that corresponds to the fired slave interrupt.
 *
 *
 *@param      TWI_t *module - the pointer to the TWI module
 *
 *
 *@return     void
 */
void TWI_SlaveInterruptHandler(TWI_t *module) {
    if (NULL != TWI_onSlaveISR) TWI_onSlaveISR(module);
}


/**
 *@brief      TWI_HandleSlaveIRQ checks the status register and decides the next action based on that
 *
 *            OK, so this function is a bit trickier. Apparently, the status register is not reset on
 *            every START condition so every of the 6 general states has multiple possible values. Also,
 *            the RXACK bit stays after every completed master READ (which terminates with a NACK), so
 *            I had to make sure to only check the NACK after a master READ command arrived.
 *            Following States are possible: STOP, ADDR_W, ADDR_R, DATA_W, DATA_R, DATA_NACK
 *            To keep the code better maintainable ("Do not repeat yourself" or something like this)
 *            I decided to use pointers - if MANDS, the compiler will work with the slave elements,
 *            if MORS, the compiler will work with the (shared) slave elements. The extra pointer assignment
 *            is not really translated into code, since the compiler would have used the load/store
 *            with displacement anyway due to the _data parameter being a pointer to a struct.
 *
 *
 *@param      struct twiData *_data is a pointer to the structure that holds the variables
 *              of a Wire object. Following struct elements are used in this function:
 *                _incomingAddress/_slaveAddress
 *                _txHead(S)
 *                _txTail(S)
 *                _rxHead(S)
 *                _rxTail(S)
 *                _txBuffer(S)[]
 *                _rxBuffer(S)[]
 *
 *@return     void
 */
void TWI_HandleSlaveIRQ(struct twiData *_data) {
  #if defined(TWI_MANDS)                            // Master and Slave split
    uint8_t*    address = &(_data->_incomingAddress);
    #if defined(TWI_MERGE_BUFFERS)                  // Same Buffers for tx/rx
      uint8_t* txHead   = &(_data->_trHeadS);
      uint8_t* txTail   = &(_data->_trTailS);
      uint8_t* rxHead   = &(_data->_trHeadS);
      uint8_t* rxTail   = &(_data->_trTailS);
      uint8_t* txBuffer =   _data->_trBufferS;
      uint8_t* rxBuffer =   _data->_trBufferS;
    #else                                           // Separate tx/rx Buffers
      uint8_t* txHead   = &(_data->_txHeadS);
      uint8_t* txTail   = &(_data->_txTailS);
      uint8_t* rxHead   = &(_data->_rxHeadS);
      uint8_t* rxTail   = &(_data->_rxTailS);
      uint8_t* txBuffer =   _data->_txBufferS;
      uint8_t* rxBuffer =   _data->_rxBufferS;
    #endif

  #else                                             // Slave using the master buffer
    uint8_t*    address = &(_data->_slaveAddress);
    #if defined(TWI_MERGE_BUFFERS)                  // Same Buffers for tx/rx
      uint8_t* txHead   = &(_data->_trHead);
      uint8_t* txTail   = &(_data->_trTail);
      uint8_t* rxHead   = &(_data->_trHead);
      uint8_t* rxTail   = &(_data->_trTail);
      uint8_t* txBuffer =   _data->_trBuffer;
      uint8_t* rxBuffer =   _data->_trBuffer;
    #else                                           // Separate tx/rx Buffers
      uint8_t* txHead   = &(_data->_txHead);
      uint8_t* txTail   = &(_data->_txTail);
      uint8_t* rxHead   = &(_data->_rxHead);
      uint8_t* rxTail   = &(_data->_rxTail);
      uint8_t* txBuffer =   _data->_txBuffer;
      uint8_t* rxBuffer =   _data->_rxBuffer;
    #endif
  #endif

  uint8_t slaveStatus = _data->_module->SSTATUS;
  uint8_t payload;                                  // Declaration in switch case does not work
  uint8_t nextHead;

  if (slaveStatus & (TWI_BUSERR_bm | TWI_COLL_bm))  // if Bus error/Collision was detected
  {
    payload = _data->_module->SDATA;                // Read data to remove Status flags
    (*rxTail) = (*rxHead);                          // Abort
    (*txTail) = (*txHead);                          // Abort
  } else {                                          // No Bus error/Collision was detected
    if (0 == _data->_bools._ackMatters) {
      slaveStatus = slaveStatus & ~TWI_RXACK_bm;    // remove RXACK except on master Read
    }
    switch (slaveStatus)
    {
      // STOP Interrupt
      // But no CLKHOLD
      case 0x40:    // APIF
      /* fall-through */
      case 0x42:    // APIF|DIR                     // No CLKHOLD, everything is already finished
      /* fall-through */
      case 0x60:    // APIF|CLKHOLD                 // STOP on master write / slave read
        _data->_module->SSTATUS = TWI_APIF_bm;      // Clear Flag, no further Action needed
        NotifyUser_onReceive(_data);                // Notify user program "onReceive" if necessary
        (*rxTail) = (*rxHead);                      // User should have handled all data, if not, set available rxBytes to 0
        break;
      // Address Interrupt
      case 0x61:    // APIF|CLKHOLD|AP              // ADR with master write / slave read
        (*address) = _data->_module->SDATA;
        #if defined(TWI_MERGE_BUFFERS)              // if single Buffer operation
          (*rxTail) = (*rxHead);                    // reset buffer positions so the master can start writing at zero.
        #endif
        _data->_module->SCTRLB = TWI_SCMD_RESPONSE_gc; // "Execute Acknowledge Action succeeded by reception of next byte"
        break;                                      // expecting data interrupt next (case 0xA0). Fills rxBuffer
      case 0x63:    // APIF|CLKHOLD|DIR|AP          // ADR with master read  / slave write
        (*address) = _data->_module->SDATA;         // saving address to pass to the user function
        //                                             There is no way to identify a REPSTART, so when a Master Read occurs after a master write
        NotifyUser_onReceive(_data);                // Notify user program "onReceive" if necessary
        #if !defined(TWI_MERGE_BUFFERS)             // if single Buffer operation
          (*txTail) = (*txHead);                    // reset buffer positions so the slave can start writing at zero.
        #endif
        NotifyUser_onRequest(_data);                // Notify user program "onRequest" if necessary
        _data->_module->SCTRLB = TWI_SCMD_RESPONSE_gc; // "Execute Acknowledge Action succeeded by slave data interrupt"
        break;                                      // expecting the TWI module to issue a data interrupt with DIR bit set. (case 0xA2).

      // Data Write Interrupt
      case 0xA0:    // DIF|CLKHOLD
      /* fall-through */
      case 0xA1:    // DIF|CLKHOLD|AP
        payload = _data->_module->SDATA;
        nextHead = TWI_advancePosition(*rxHead);
        if (nextHead == (*rxTail)) {                // if buffer is full
          _data->_module->SCTRLB = TWI_ACKACT_bm | TWI_SCMD_COMPTRANS_gc; // "Execute Acknowledge Action succeeded by waiting for any Start (S/Sr) condition"
          (*rxTail) = (*rxHead);                                          // Dismiss all received Data since data integrity can't be guaranteed
        } else {                                    // if buffer is not full
          rxBuffer[(*rxHead)] = payload;                  // Load data into the buffer
          (*rxHead) = nextHead;                           // Advance Head
          _data->_module->SCTRLB = TWI_SCMD_RESPONSE_gc;  // "Execute Acknowledge Action succeeded by reception of next byte"
        }
        break;

      // ACK Received
      // Data Read Interrupt
      case 0xA2:    // DIF|CLKHOLD|DIR
      /* fall-through */
      case 0xA3:    // DIF|CLKHOLD|DIR|AP
        _data->_bools._ackMatters = true;         // start checking for NACK
        if ((*txHead) != (*txTail)) {             // Data is available
          _data->_module->SDATA = txBuffer[(*txTail)];      // Writing to the register to send data
          (*txTail) = TWI_advancePosition(*txTail);         // Advance tail
          _data->_module->SCTRLB = TWI_SCMD_RESPONSE_gc;    // "Execute a byte read operation followed by Acknowledge Action"
        } else {                                            // No more data available
          _data->_module->SCTRLB = TWI_SCMD_COMPTRANS_gc;   // "Wait for any Start (S/Sr) condition"
        }
        break;

      // NACK Received
      // Data Read Interrupt
      // case 0x90:  // DIF|RXACK
      case 0xB2:    // DIF|CLKHOLD|RXACK|DIR       // data NACK on master read  / slave write
      case 0xB3:    // DIF|CLKHOLD|RXACK|DIR|AP
        _data->_bools._ackMatters = false;                // stop checking for NACK
        _data->_module->SCTRLB = TWI_SCMD_COMPTRANS_gc;   // "Wait for any Start (S/Sr) condition"
        (*txTail) = (*txHead);                            // Abort further data writes
        break;

      // Illegal State
      default:
        // Abort operation
        _data->_module->SCTRLB = TWI_ACKACT_bm | TWI_SCMD_COMPTRANS_gc;
        // while (true) {}  // while loop for debugging. Use this to see what SSTATUS is
        break;
    }
  }
}



/**
 *@brief      NotifyUser_onRequest is called from the TWI_HandleSlaveIRQ function on master READ
 *
 *            This function calls the user defined function in the sketch if it was
 *            registered. It is issued on master READ
 *
 *
 *@param      struct twiData *_data is a pointer to the structure that holds the variables
 *              of a Wire object. Following struct elements are used in this function:
 *                _bools
 *                user_onRequest()
 *
 *
 *@return     void
 */
void NotifyUser_onRequest(struct twiData *_data) {
  if (_data->user_onRequest != NULL) {
    #if defined(TWI_MANDS)
      _data->_bools._toggleStreamFn = 0x01;
      _data->user_onRequest();
      _data->_bools._toggleStreamFn = 0x00;
    #else
      _data->user_onRequest();
    #endif
  }
}


/**
 *@brief      NotifyUser_onRequest is called from the TWI_HandleSlaveIRQ function on master WRITE
 *
 *            This function calls the user defined function in the sketch if it was
 *            registered. It is issued on master WRITE. the user defined function is only called
 *            if there were bytes written. it passes the amount of written bytes to the user function
 *
 *
 *@param      struct twiData *_data is a pointer to the structure that holds the variables
 *              of a Wire object. Following struct elements are used in this function:
 *                _bools
 *                user_onReceive()
 *
 *
 *@return     void
 */
void NotifyUser_onReceive(struct twiData *_data) {
  if (_data->user_onReceive != NULL) {
    uint8_t numBytes;
    #if defined(TWI_MANDS)
      _data->_bools._toggleStreamFn = 0x01;
    #endif

    numBytes = TWI_Available(_data);
    if (numBytes > 0) {
      _data->user_onReceive(numBytes);
    }

    #if defined(TWI_MANDS)
      _data->_bools._toggleStreamFn = 0x00;
    #endif
  }
}


/**
 *@brief      TWI_advancePosition increments the given number if it smaller then the BUFFER_LENGTH value
 *
 *            When using round-robin buffers, it is important to reset at 0 when the position reaches
 *            the end of a buffer. This is done with this inline function.
 *            Because the AVR Dx chips were using a buffer of 130. I was not able to use the trick with
 *            the bitwise AND on Buffers that are a power of 2. To reduce the space usage on the smaller
 *            chips, I decided to add a #define based check to use the bitwise AND. To avoid duplicates of
 *            this part of the code, I wrote an extra function for that. It also increases readability.
 *
 *
 *@param      uint8_t pos is the number to be incremented/set to zero
 *
 *@return     uint8_t
 *@retval     the incremented number or zero.
 */
uint8_t TWI_advancePosition(uint8_t pos) {
  uint8_t nextPos = (pos + 1);
  #if defined(BUFFER_NOT_POWER_2)
    if (nextPos > (BUFFER_LENGTH - 1)) nextPos = 0;  // round-robin-ing
  #else
    nextPos &= (BUFFER_LENGTH - 1);
  #endif

  return nextPos;
}


/**
 *@brief      TWI0 Slave Interrupt vector
 */
ISR(TWI0_TWIS_vect) {
  TWI_SlaveInterruptHandler(&TWI0);
}


/**
 *@brief      TWI1 Slave Interrupt vector
 */
#if defined(TWI1)
  ISR(TWI1_TWIS_vect) {
    TWI_SlaveInterruptHandler(&TWI1);
  }
#endif
