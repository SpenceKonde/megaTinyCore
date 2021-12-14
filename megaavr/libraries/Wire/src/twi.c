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
// *INDENT-OFF*   astyle wants this file to be completely unreadable with no indentation for the many preprocessor conditionals!

#include "Arduino.h"
#include "twi.h"
#include "twi_pins.h"

// "Private" function declaration
void NotifyUser_onRequest(struct twiData *_data);
void NotifyUser_onReceive(struct twiData *_data);

void SlaveIRQ_AddrRead(struct twiData *_data);
void SlaveIRQ_AddrWrite(struct twiData *_data);
void SlaveIRQ_Stop(struct twiData *_data);
void SlaveIRQ_DataReadNack(struct twiData *_data);
void SlaveIRQ_DataReadAck(struct twiData *_data);
void SlaveIRQ_DataWrite(struct twiData *_data);


// Function definitions
/**
 *@brief      TWI_MasterInit Initializes TWI host operation if not already initialized
 *
 *@param      struct twiData *_data is a pointer to the structure that holds the variables
 *              of a Wire object. Following struct elements are used:
 *              _bools._hostEnabled
 *              _bools._clientEnabled
 *              _module
 *
 *@return     void
 */
void TWI_MasterInit(struct twiData *_data) {
  #if defined(TWI_MANDS)                            // Check if the user wants to use Master AND Slave
    if (_data->_bools._hostEnabled == 1) {          // Slave is allowed to be enabled, don't re-enable the host though
      return;
    }
  #else                                             // Master OR Slave
    if (_data->_bools._hostEnabled    == 1 ||       // If Master was enabled
        _data->_bools._clientEnabled  == 1) {       // or Slave was enabled
      return;                                       // return and do nothing
    }
  #endif


  #if defined(TWI1)                                 // More then one TWI used
    if        (&TWI0 == _data->_module) {           // check which one this function is working with
      TWI0_ClearPins();
    } else if (&TWI1 == _data->_module) {
      TWI1_ClearPins();
    }
  #else                                             // Only one TWI is used
    TWI0_ClearPins();                               // Only one option is possible
  #endif

  _data->_bools._hostEnabled    = 1;
  _data->_module->MCTRLA        = TWI_ENABLE_bm;  // Master Interrupt flags stay disabled
  _data->_module->MSTATUS       = TWI_BUSSTATE_IDLE_gc;

  TWI_MasterSetBaud(_data, DEFAULT_FREQUENCY);
}


/**
 *@brief      TWI_SlaveInit Initializes TWI client operation if not already initialized
 *
 *@param      struct twiData *_data is a pointer to the structure that holds the variables
 *              of a Wire object. Following struct elements are used in this function:
 *                _bools._hostEnabled
 *                _bools._clientEnabled
 *                _module
 *@param      uint8_t address holds the primary address that this TWI client should listen to
 *            uint8_t receive_broadcast if true, instructs the TWI client to react to the
 *              general TWI call 0x00
 *            uint8_t second_address holds the data for the SADDRMASK register. If the LSB is '1'
 *              the TWI handles the 7 MSB as a second address for the client, otherwise the 7 MSB
 *              act as a bit mask, that disables the check on the corresponding SADDR bit.
 *
 *@return     void
 */
void TWI_SlaveInit(struct twiData *_data, uint8_t address, uint8_t receive_broadcast, uint8_t second_address) {
  #if defined(TWI_MANDS)                      // Check if the user wants to use Master AND Slave
    if (_data->_bools._clientEnabled  == 1) {  // Master is allowed to be enabled, don't re-enable the client though
      return;
    }
  #else                                       // Master or Slave
    if (_data->_bools._hostEnabled    == 1 || // If Master was enabled
        _data->_bools._clientEnabled  == 1) { // or Slave was enabled
    return;                                   // return and do nothing
    }
  #endif

  #if defined(TWI1)
    if        (&TWI0 == _data->_module) {
      TWI0_ClearPins();
    } else if (&TWI1 == _data->_module) {
      TWI1_ClearPins();
    }
  #else
    TWI0_ClearPins();
  #endif

  _data->_bools._clientEnabled = 1;
  _data->_module->SADDR        = (address << 1) | receive_broadcast;
  _data->_module->SADDRMASK    = second_address;
  _data->_module->SCTRLA       = TWI_DIEN_bm | TWI_APIEN_bm | TWI_PIEN_bm  | TWI_ENABLE_bm;

  /* Bus Error Detection circuitry needs Master enabled to work */
  _data->_module->MCTRLA = TWI_ENABLE_bm;
}


/**
 *@brief      TWI_Flush clears the internal state of the host and changes the bus state to idle
 *
 *@param      struct twiData *_data is a pointer to the structure that holds the variables
 *              of a Wire object. Following struct elements are used in this function:
 *                _module
 *
 *@return             void
 */
void TWI_Flush(struct twiData *_data) {
  #if defined(ERRATA_TWI_FLUSH)
    // badCall("The AVR DA-series parts are impacted by an errata that leaves the TWI peripheral in a non-functioning state when using flush.");
    // restarting TWI hardware by hand. Extra size shouldn't matter on DA series
    uint8_t temp_MCTRLA      = _data->_module->MCTRLA;
    uint8_t temp_SCTRLA      = _data->_module->SCTRLA;
    _data->_module->MCTRLA  = 0x00;
    _data->_module->SCTRLA  = 0x00;
    _data->_module->MCTRLA  = temp_MCTRLA;
    _data->_module->MSTATUS = 0x01;  // force TWI state machine into idle state
    _data->_module->SCTRLA  = temp_SCTRLA;
  #else
  _data->_module->MCTRLB |= TWI_FLUSH_bm;
  #endif
}


/**
 *@brief      TWI_Disable disables the TWI host and client
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
 *@brief      TWI_DisableMaster disables the TWI host
 *
 *@param      struct twiData *_data is a pointer to the structure that holds the variables
 *              of a Wire object. Following struct elements are used in this function:
 *                _bools._hostEnabled
 *                _module
 *
 *@return     void
 */
void TWI_DisableMaster(struct twiData *_data) {
  if (true == _data->_bools._hostEnabled) {
    if (false == _data->_bools._clientEnabled) {
      _data->_module->MCTRLA    = 0x00;  // has to stay enabled for bus error circuitry
    }
  _data->_module->MBAUD         = 0x00;
  _data->_bools._hostEnabled    = 0x00;
  }
}


/**
 *@brief      TWI_DisableSlave disables the TWI client
 *
 *@param      struct twiData *_data is a pointer to the structure that holds the variables
 *              of a Wire object. Following struct elements are used in this function:
 *                _bools._clientEnabled
 *                _module
 *@return     void
 */
void TWI_DisableSlave(struct twiData *_data) {
  if (true == _data->_bools._clientEnabled) {
    if (false == _data->_bools._hostEnabled) {
      _data->_module->MCTRLA     = 0x00;      // might be enabled for bus error circuitry
    }
    _data->_module->SADDR        = 0x00;
    _data->_module->SCTRLA       = 0x00;
    _data->_module->SADDRMASK    = 0x00;
    _data->_bools._clientEnabled = 0x00;
    #if defined(TWI_DUALCTRL)
      _data->_module->DUALCTRL   = 0x00;    // Disable pin splitting when available
    #endif
  }
}



/**
 *@brief      TWI_MasterSetBaud sets the baud register to get the desired frequency
 *
 *            After checking if the host is actually enabled, the new baud is calculated.
 *              Then it is compared to the old baud. Only if they differ, the host is disabled,
 *              the baud register updated, and the host re-enabled
 *
 *@param      struct twiData *_data is a pointer to the structure that holds the variables
 *              of a Wire object. Following struct elements are used in this function:
 *              _bools._hostEnabled
 *              _module
 *
 *@return     void
 */
void TWI_MasterSetBaud(struct twiData *_data, uint32_t frequency) {
  if (_data->_bools._hostEnabled == 1) {                  // Do something only if the host is enabled.
    uint8_t newBaud = TWI_MasterCalcBaud(frequency);      // get the new Baud value
    uint8_t oldBaud = _data->_module->MBAUD;              // load the old Baud value
    if (newBaud != oldBaud) {                             // compare both, in case the code is issuing this before every transmission.
      uint8_t restore = _data->_module->MCTRLA;           // Save the old Master state
      _data->_module->MCTRLA    = 0;                      // Disable Master
      _data->_module->MBAUD     = newBaud;                // update Baud register
      if (frequency >= 600000) {
        _data->_module->CTRLA  |=  TWI_FMPEN_bm;          // Enable FastMode+
      } else {
        _data->_module->CTRLA  &= ~TWI_FMPEN_bm;          // Disable FastMode+
      }
      _data->_module->MCTRLA    = restore;                // restore the old register, thus enabling it again
      _data->_module->MSTATUS   = TWI_BUSSTATE_IDLE_gc;   // Force the state machine into Idle according to the data sheet
    }
  }
}

/**
 *@brief      TWI_Available returns the amount of bytes that are available to read in the host or client buffer
 *
 *            This function is placed in this file because the client interrupt handler needs it too.
 *            This file has no concept of the Wire object.
 *            In MANDS mode, when called from
 *            user_onRequest() or user_onReceive() it will return the number from the client buffer
 *            due to the _toggleStreamFn flag
 *
 *@param      struct twiData *_data is a pointer to the structure that holds the variables
 *              of a Wire object. Following struct elements are used in this function:
 *                _bools._toggleStreamFn
 *                _rxHead(S)
 *                _rxTail(S)
 *
 *@return     uint8_t
 *@retval     amount of bytes available to read from the host or client buffer
 */
uint8_t TWI_Available(struct twiData *_data) {
  uint16_t num;
  uint8_t* rxHead;
  uint8_t* rxTail;

  #if defined(TWI_MANDS)                          // Add following if host and client are split
    if (_data->_bools._toggleStreamFn == 0x01) {
      rxHead  = &(_data->_trHeadS);
      rxTail  = &(_data->_trTailS);
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
 *@brief      TWI_MasterWrite performs a host write operation on the TWI bus
 *
 *            As soon as the bus is in an idle state, a polled write operation is performed
 *            A STOP condition can be send at the end, or not if a REP START is wanted
 *            The user has to make sure to have a host write or read at the end with a STOP
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

  TWI_t *module = _data->_module;     // Compiler treats the pointer to the TWI module as volatile and
                                      // creates bloat-y code, this fixes it
  TWI_INIT_ERROR;
  uint8_t currentSM;
  uint8_t currentStatus;
  uint8_t dataWritten = 0;
  uint16_t timeout = 0;


  if ((module->MSTATUS & TWI_BUSSTATE_gm) == TWI_BUSSTATE_UNKNOWN_gc) {
    return 0;                                                   // If the bus was not initialized, return
  }


  while (true) {
    currentStatus = module->MSTATUS;
    currentSM = currentStatus & TWI_BUSSTATE_gm;  // get the current mode of the state machine

    #if defined(TWI_TIMEOUT_ENABLE)
      if (++timeout > (F_CPU/1000)) {
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

    if   (currentStatus & (TWI_ARBLOST_bm | TWI_BUSERR_bm)) { // Check for Bus error
        module->MSTATUS = (TWI_ARBLOST_bm | TWI_BUSERR_bm);   // reset error flags
        TWI_SET_ERROR(TWI_ERR_BUS_ARB);                       // set error flag
        break;                                                // leave RX loop
    }

    if (currentSM == TWI_BUSSTATE_IDLE_gc) {                  // Bus has not sent START yet and is not BUSY
        module->MADDR = ADD_WRITE_BIT(_data->_clientAddress);
        timeout = 0;
    } else if (currentSM == TWI_BUSSTATE_OWNER_gc) {          // Address was sent, host is owner
      if     (currentStatus & TWI_WIF_bm) {                   // data sent
        if   (currentStatus & TWI_RXACK_bm) {                 // AND the RXACK bit is set
          if (dataWritten != 0) dataWritten--;                // last Byte has failed, so decrement the counter, except if it was Address
          TWI_SET_ERROR(TWI_ERR_RXACK);                       // set error flag
          send_stop = 1;
          break;                                              // leave loop
        } else {                                              // otherwise WRITE was ACKed
          if ((*txHead) != (*txTail)) {                       // check if there is data to be written
            module->MDATA = txBuffer[(*txTail)];              // Writing to the register to send data
            (*txTail) = TWI_advancePosition(*txTail);         // advance tail
            dataWritten++;                                    // data was Written
            timeout = 0;                                      // reset timeout
          } else {                                            // else there is no data to be written
            break;                                            // TX finished, leave loop, error is still TWI_NO_ERR
          }
        }
      }
    }
  }


  if (send_stop != 0 || !(TWI_CHK_ERROR(TWI_NO_ERR))) {
    module->MCTRLB = TWI_MCMD_STOP_gc;                        // Send STOP
  }
  #if defined(TWI_ERROR_ENABLED)
    TWI_SAVE_ERROR(_data->_errors);                           // save error flags
  #endif
  return dataWritten;                                         // return amount of bytes written
}


/**
 *@brief      TWI_MasterRead performs a host read operation on the TWI bus
 *
 *            As soon as the bus is in an idle state, a polled read operation is performed
 *            A STOP condition can be send at the end, or not if a REP START is wanted
 *            The user has to make sure to have a host write or read at the end with a STOP
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

  TWI_t *module = _data->_module;     // Compiler treats the pointer to the TWI module as volatile and
                                      // creates bloat-y code, using a local variable fixes that

  if ((module->MSTATUS & TWI_BUSSTATE_gm) == TWI_BUSSTATE_UNKNOWN_gc) {
    return 0;                         // If the bus was not initialized, return
  }

  uint8_t currentSM;
  uint8_t currentStatus;
  TWI_INIT_ERROR;             // local variable for errors
  uint8_t command  = 0;
  uint8_t dataRead = 0;
  uint16_t timeout = 0;

  while (true) {
    currentStatus = module->MSTATUS;
    currentSM = currentStatus & TWI_BUSSTATE_gm;  // get the current mode of the state machine

    #if defined(TWI_TIMEOUT_ENABLE)
      if (++timeout > (F_CPU/1000)) {
        if      (currentSM == TWI_BUSSTATE_OWNER_gc) {
          TWI_SET_ERROR(TWI_ERR_TIMEOUT);
        } else if (currentSM == TWI_BUSSTATE_IDLE_gc) {
          TWI_SET_ERROR(TWI_ERR_PULLUP);
        } else {
          TWI_SET_ERROR(TWI_ERR_UNDEFINED);
        }
        break;
      }
    #endif

    if (currentStatus & (TWI_ARBLOST_bm | TWI_BUSERR_bm)) {   // Check for Bus error
      module->MSTATUS = (TWI_ARBLOST_bm | TWI_BUSERR_bm);       // reset error flags
      TWI_SET_ERROR(TWI_ERR_BUS_ARB);                           // set error flag
      break;                                                    // leave TX loop
    }

    if (command != 0) {
      if (currentSM == TWI_BUSSTATE_OWNER_gc) {
        module->MCTRLB = command;
      } else {
        break;
      }
    }

    if (currentSM == TWI_BUSSTATE_IDLE_gc) {    // Bus has not sent START yet
        module->MADDR = ADD_READ_BIT(_data->_clientAddress);
        timeout = 0;
    } else if (currentSM == TWI_BUSSTATE_OWNER_gc) {  // Address sent, check for WIF/RIF
      if (currentStatus & TWI_RIF_bm) {                    // data received
        if (dataRead > (BUFFER_LENGTH-1)) {                   // Buffer overflow with this incoming Byte
          TWI_SET_ERROR(TWI_ERR_BUF_OVERFLOW);
          command = TWI_ACKACT_bm | TWI_MCMD_STOP_gc;         // send STOP + NACK
        } else {
                                                    // Data is fine and we have space, so read out the data register
          rxBuffer[(*rxHead)] = module->MDATA;        // and save it in the Buffer.
          (*rxHead) = TWI_advancePosition(*rxHead);               // advance head
          dataRead++;                                             // Byte was read
          timeout = 0;                                            // reset timeout

          if (dataRead < bytesToRead) {                           // expecting more bytes, so
            module->MCTRLB = TWI_MCMD_RECVTRANS_gc;               // send an ACK so the Slave so it can send the next byte
          } else {                                                // Otherwise,
            if (send_stop != 0) {
              command = TWI_ACKACT_bm | TWI_MCMD_STOP_gc;       // send STOP + NACK
            } else {
              break;
            }
          }
        }
      } else if (currentStatus & TWI_WIF_bm) {  // Address NACKed
        TWI_SET_ERROR(TWI_ERR_RXACK);           // set error flag
        command = TWI_MCMD_STOP_gc;
      }
    }
  }

  #if defined(TWI_ERROR_ENABLED)
    TWI_SAVE_ERROR(_data->_errors);                             // save error flags
  #endif
  return dataRead;
}


/**
 *@brief      TWI_HandleSlaveIRQ checks the status register and decides the next action based on that
 *
 *            OK, so this function is a bit trickier. Apparently, the status register is not reset on
 *            every START condition so every of the 6 general states has multiple possible values. Also,
 *            the RXACK bit stays after every completed host READ (which terminates with a NACK), so
 *            I had to make sure to only check the NACK after a host READ command arrived.
 *            Following States are possible: STOP, ADDR_W, ADDR_R, DATA_W, DATA_R, DATA_NACK
 *            To keep the code better maintainable ("Do not repeat yourself" or something like this)
 *            I decided to use pointers - if MANDS, the compiler will work with the client elements,
 *            if MORS, the compiler will work with the (shared) client elements. The extra pointer assignment
 *            is not really translated into code, since the compiler would have used the load/store
 *            with displacement anyway due to the _data parameter being a pointer to a struct.
 *
 *
 *@param      struct twiData *_data is a pointer to the structure that holds the variables
 *              of a Wire object. Following struct elements are used in this function:
 *                _incomingAddress/_clientAddress
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
    uint8_t* txHead   = &(_data->_trHeadS);
    uint8_t* txTail   = &(_data->_trTailS);
    uint8_t* rxHead   = &(_data->_trHeadS);
    uint8_t* rxTail   = &(_data->_trTailS);

  #else                                             // Slave using the host buffer
    #if defined(TWI_MERGE_BUFFERS)                  // Same Buffers for tx/rx
      uint8_t* txHead   = &(_data->_trHead);
      uint8_t* txTail   = &(_data->_trTail);
      uint8_t* rxHead   = &(_data->_trHead);
      uint8_t* rxTail   = &(_data->_trTail);
    #else                                           // Separate tx/rx Buffers
      uint8_t* txHead   = &(_data->_txHead);
      uint8_t* txTail   = &(_data->_txTail);
      uint8_t* rxHead   = &(_data->_rxHead);
      uint8_t* rxTail   = &(_data->_rxTail);
    #endif
  #endif

  uint8_t clientStatus = _data->_module->SSTATUS;

  if (clientStatus & (TWI_BUSERR_bm | TWI_COLL_bm)) {  // if Bus error/Collision was detected
    _data->_module->SDATA;                            // Read data to remove Status flags
    (*rxTail) = (*rxHead);                          // Abort
    (*txTail) = (*txHead);                          // Abort
  } else {                                          // No Bus error/Collision was detected
    #if defined(TWI_MANDS)
      _data->_bools._toggleStreamFn = 0x01;
    #endif

    if (clientStatus & TWI_APIF_bm) {  // Address/Stop Bit set
      if (clientStatus & TWI_AP_bm) {    // Address bit set
        if (clientStatus & TWI_DIR_bm) {   // Master is reading
          SlaveIRQ_AddrRead(_data);
        } else {                          // Master is writing
          SlaveIRQ_AddrWrite(_data);
        }
      } else {                          // Stop bit set
        SlaveIRQ_Stop(_data);
      }
    } else if (clientStatus & TWI_DIF_bm) {  // Data bit set
      if (clientStatus & TWI_DIR_bm) {         // Master is reading
        if ((clientStatus & TWI_RXACK_bm) && _data->_bools._ackMatters) {  // RXACK bit is set and it matters
          SlaveIRQ_DataReadNack(_data);
        } else {                                // RXACK bit not set
          SlaveIRQ_DataReadAck(_data);
        }
      } else {                                // Master is writing
        SlaveIRQ_DataWrite(_data);
      }
    }

    #if defined(TWI_MANDS)
      _data->_bools._toggleStreamFn = 0x00;
    #endif
  }
}


/**
 *@brief      SlaveIRQ_AddrRead is a subroutine of TWI_HandleSlaveIRQ and handles the Address Read case
 *
 *
 *@param      struct twiData *_data is a pointer to the structure that holds the variables
 *              of a Wire object. Following struct elements are used in this function:
 *                _incomingAddress/_clientAddress
 *                _bools
 *                _txHead
 *                _txTail
 *
 *
 *@return     void
 */
void SlaveIRQ_AddrRead(struct twiData *_data) {
  #if defined(TWI_MANDS)                            // Master and Slave split
    uint8_t*    address = &(_data->_incomingAddress);

    uint8_t* txHead   = &(_data->_trHeadS);
    uint8_t* txTail   = &(_data->_trTailS);

  #else                                             // Slave using the host buffer
    uint8_t*    address = &(_data->_clientAddress);
    #if defined(TWI_MERGE_BUFFERS)                  // Same Buffers for tx/rx
      uint8_t* txHead   = &(_data->_trHead);
      uint8_t* txTail   = &(_data->_trTail);
    #else                                           // Separate tx/rx Buffers
      uint8_t* txHead   = &(_data->_txHead);
      uint8_t* txTail   = &(_data->_txTail);
    #endif
  #endif


  (*address) = _data->_module->SDATA;         // saving address to pass to the user function
                                              // There is no way to identify a REPSTART, so when a Master Read occurs after a host write
  NotifyUser_onReceive(_data);                // Notify user program "onReceive" if necessary
  #if !defined(TWI_MERGE_BUFFERS)             // if not single Buffer operation
    (*txTail) = (*txHead);                    // reset buffer positions so the client can start writing at zero.
  #endif
  NotifyUser_onRequest(_data);                // Notify user program "onRequest" if necessary
  _data->_module->SCTRLB = TWI_SCMD_RESPONSE_gc;  // "Execute Acknowledge Action succeeded by client data interrupt"
}

void SlaveIRQ_AddrWrite(struct twiData *_data) {
  #if defined(TWI_MANDS)                            // Master and Slave split
    uint8_t*    address = &(_data->_incomingAddress);

    uint8_t* rxHead   = &(_data->_trHeadS);
    uint8_t* rxTail   = &(_data->_trTailS);
  #else                                             // Slave using the host buffer
    uint8_t*    address = &(_data->_clientAddress);
    #if defined(TWI_MERGE_BUFFERS)                  // Same Buffers for tx/rx
      uint8_t* rxHead   = &(_data->_trHead);
      uint8_t* rxTail   = &(_data->_trTail);
    #endif
  #endif


  (*address) = _data->_module->SDATA;
  #if defined(TWI_MERGE_BUFFERS) || defined(TWI_MANDS)  // if single Buffer operation
    (*rxTail) = (*rxHead);                    // reset buffer positions so the host can start writing at zero.
  #endif
  _data->_module->SCTRLB = TWI_SCMD_RESPONSE_gc;  // "Execute Acknowledge Action succeeded by reception of next byte"
}

void SlaveIRQ_Stop(struct twiData *_data) {
  #if defined(TWI_MANDS)                            // Master and Slave split
    uint8_t* rxHead   = &(_data->_trHeadS);
    uint8_t* rxTail   = &(_data->_trTailS);
  #else                                             // Slave using the host buffer
    #if defined(TWI_MERGE_BUFFERS)                  // Same Buffers for tx/rx
      uint8_t* rxHead   = &(_data->_trHead);
      uint8_t* rxTail   = &(_data->_trTail);
    #else                                           // Separate tx/rx Buffers
      uint8_t* rxHead   = &(_data->_txHead);
      uint8_t* rxTail   = &(_data->_txTail);
    #endif
  #endif


  _data->_module->SSTATUS = TWI_APIF_bm;      // Clear Flag, no further action needed
  NotifyUser_onReceive(_data);                // Notify user program "onReceive" if necessary
  (*rxTail) = (*rxHead);                      // User should have handled all data, if not, set available rxBytes to 0
}

void SlaveIRQ_DataReadNack(struct twiData *_data) {
  #if defined(TWI_MANDS)                            // Master and Slave split
    uint8_t* txHead   = &(_data->_trHeadS);
    uint8_t* txTail   = &(_data->_trTailS);
  #else                                             // Slave using the host buffer
    #if defined(TWI_MERGE_BUFFERS)                  // Same Buffers for tx/rx
      uint8_t* txHead   = &(_data->_trHead);
      uint8_t* txTail   = &(_data->_trTail);
    #else                                           // Separate tx/rx Buffers
      uint8_t* txHead   = &(_data->_txHead);
      uint8_t* txTail   = &(_data->_txTail);
    #endif
  #endif


  _data->_bools._ackMatters = false;                        // stop checking for NACK
  _data->_module->SCTRLB = TWI_SCMD_COMPTRANS_gc;   // "Wait for any Start (S/Sr) condition"
  (*txTail) = (*txHead);                            // Abort further data writes
}

void SlaveIRQ_DataReadAck(struct twiData *_data) {
  #if defined(TWI_MANDS)                            // Master and Slave split
    uint8_t* txHead   = &(_data->_trHeadS);
    uint8_t* txTail   = &(_data->_trTailS);
    uint8_t* txBuffer =   _data->_trBufferS;
  #else                                             // Slave using the host buffer
    #if defined(TWI_MERGE_BUFFERS)                  // Same Buffers for tx/rx
      uint8_t* txHead   = &(_data->_trHead);
      uint8_t* txTail   = &(_data->_trTail);
      uint8_t* txBuffer =   _data->_trBuffer;
    #else                                           // Separate tx/rx Buffers
      uint8_t* txHead   = &(_data->_txHead);
      uint8_t* txTail   = &(_data->_txTail);
      uint8_t* txBuffer =   _data->_txBuffer;
    #endif
  #endif


  _data->_bools._ackMatters = true;         // start checking for NACK
  if ((*txHead) != (*txTail)) {             // Data is available
    _data->_slaveBytesRead++;
    _data->_module->SDATA = txBuffer[(*txTail)];      // Writing to the register to send data
    (*txTail) = TWI_advancePosition(*txTail);         // Advance tail
    _data->_module->SCTRLB = TWI_SCMD_RESPONSE_gc;    // "Execute a byte read operation followed by Acknowledge Action"

  } else {                                            // No more data available
    _data->_module->SCTRLB = TWI_SCMD_COMPTRANS_gc;   // "Wait for any Start (S/Sr) condition"
  }
}

void SlaveIRQ_DataWrite(struct twiData *_data) {
  #if defined(TWI_MANDS)                            // Master and Slave split
      uint8_t* rxHead   = &(_data->_trHeadS);
      uint8_t* rxTail   = &(_data->_trTailS);
      uint8_t* rxBuffer =   _data->_trBufferS;
  #else                                             // Slave using the host buffer
    #if defined(TWI_MERGE_BUFFERS)                  // Same Buffers for tx/rx
      uint8_t* rxHead   = &(_data->_trHead);
      uint8_t* rxTail   = &(_data->_trTail);
      uint8_t* rxBuffer =   _data->_trBuffer;
    #else                                           // Separate tx/rx Buffers
      uint8_t* rxHead   = &(_data->_rxHead);
      uint8_t* rxTail   = &(_data->_rxTail);
      uint8_t* rxBuffer =   _data->_rxBuffer;
    #endif
  #endif


  uint8_t payload = _data->_module->SDATA;
  uint8_t nextHead = TWI_advancePosition(*rxHead);

  if (nextHead == (*rxTail)) {                  // if buffer is full
    _data->_module->SCTRLB = TWI_ACKACT_bm | TWI_SCMD_COMPTRANS_gc;  // "Execute ACK Action succeeded by waiting for any Start (S/Sr) condition"
    (*rxTail) = (*rxHead);                                           // Dismiss all received Data since data integrity can't be guaranteed

  } else {                                      // if buffer is not full
    rxBuffer[(*rxHead)] = payload;                  // Load data into the buffer
    (*rxHead) = nextHead;                           // Advance Head
    _data->_module->SCTRLB = TWI_SCMD_RESPONSE_gc;  // "Execute Acknowledge Action succeeded by reception of next byte"
  }
}

/**
 *@brief      NotifyUser_onRequest is called from the TWI_HandleSlaveIRQ function on host READ
 *
 *            This function calls the user defined function in the sketch if it was
 *            registered. It is issued on host READ
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
    _data->user_onRequest();
  }
}


/**
 *@brief      NotifyUser_onRequest is called from the TWI_HandleSlaveIRQ function on host WRITE
 *
 *            This function calls the user defined function in the sketch if it was
 *            registered. It is issued on host WRITE. the user defined function is only called
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
    uint8_t numBytes = TWI_Available(_data);
    if (numBytes > 0) {
      _data->user_onReceive(numBytes);
    }
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
 *@brief              TWI_MasterCalcBaud calculates the baud for the desired frequency
 *
 *@param              uint32_t frequency is the desired frequency
 *
 *@return             uint8_t
 *@retval             the desired baud value
 */
#define TWI_BAUD(freq, t_rise) ((F_CPU / freq) / 2) - (5 + (((F_CPU / 1000000) * t_rise) / 2000))
uint8_t TWI_MasterCalcBaud(uint32_t frequency) {
  uint16_t t_rise;
  int16_t baud;

  // The nonlinearity of the frequency coupled with the processor frequency a general offset has been calculated and tested for different frequency bands
  #if F_CPU > 16000000
    if (frequency <= 100000) {
      t_rise = 1000;
      baud = TWI_BAUD(frequency, t_rise) + 6;  // Offset +6
    } else if (frequency <= 400000) {
      t_rise = 300;
      baud = TWI_BAUD(frequency, t_rise) + 1;  // Offset +1
    } else if (frequency <= 800000) {
      t_rise = 120;
      baud = TWI_BAUD(frequency, t_rise);
    } else {
      t_rise = 120;
      baud = TWI_BAUD(frequency, t_rise) - 1;  // Offset -1
    }
  #else
    if (frequency <= 100000) {
      t_rise = 1000;
      baud = TWI_BAUD(frequency, t_rise) + 8;  // Offset +8
    } else if (frequency <= 400000) {
      t_rise = 300;
      baud = TWI_BAUD(frequency, t_rise) + 1;  // Offset +1
    } else if (frequency <= 800000) {
      t_rise = 120;
      baud = TWI_BAUD(frequency, t_rise);
    } else {
      t_rise = 120;
      baud = TWI_BAUD(frequency, t_rise) - 1;  // Offset -1
    }
  #endif

  if (baud < 1) {
    baud = 1;
  } else if (baud > 255) {
    baud = 255;
  }
  return (uint8_t)baud;
}
