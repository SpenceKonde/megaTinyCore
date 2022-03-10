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
 *@param      struct twiData *_data is a pointer to the structure that holds the Wire variables
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
 *@param      struct twiData *_data is a pointer to the structure that holds the Wire variables
 *@param      uint8_t address holds the primary address that this TWI client should listen to
 *@param      uint8_t receive_broadcast if true, instructs the TWI client to react to the
 *              general TWI call 0x00
 *@param      uint8_t second_address holds the data for the SADDRMASK register. If the LSB is '1'
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
  #else                                         // Master or Slave
    if (_data->_bools._hostEnabled    == 1 ||   // If Master was enabled
        _data->_bools._clientEnabled  == 1) {   // or Slave was enabled
    return;                                     // return and do nothing
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
 *@param      struct twiData *_data is a pointer to the structure that holds the Wire variables
 *
 *@return             void
 */
void TWI_Flush(struct twiData *_data) {
  #if defined(ERRATA_TWI_FLUSH)
    // badCall("The AVR DA-series parts are impacted by an errata that leaves the TWI peripheral in a non-functioning state when using flush.");
    // restarting TWI hardware by hand. Extra size shouldn't matter on DA series
    uint8_t temp_MCTRLA     = _data->_module->MCTRLA;
    uint8_t temp_SCTRLA     = _data->_module->SCTRLA;
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
 *@param      struct twiData *_data is a pointer to the structure that holds the Wire variables
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
 *@param      struct twiData *_data is a pointer to the structure that holds the Wire variables
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
 *@param      struct twiData *_data is a pointer to the structure that holds the Wire variables
 *
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
 *            After checking if the master mode is actually enabled, the new baud is calculated.
 *              Then it is compared to the old baud. Only if they differ, the TWI is disabled,
 *              the baud register updated, and the TWI re-enabled
 *
 *@param      struct twiData *_data is a pointer to the structure that holds the Wire variables
 *
 *@return     uint8_t 1 - something went wrong; 0 - new frequency set.
 */
uint8_t TWI_MasterSetBaud(struct twiData *_data, uint32_t frequency) {
  if (__builtin_constant_p(frequency)) {
    if ((frequency < 1000) || (frequency > 15000000)) {
      badArg("Invalid frequency was passed for SCL clock!");
      return 1;
    }
  } else {
    if (frequency < 1000) {
      return 1;
    }
  }
  if (_data->_bools._hostEnabled == 1) {                  // Do something only if the host is enabled.
    uint8_t newBaud = TWI_MasterCalcBaud(frequency);      // get the new Baud value
    uint8_t oldBaud = _data->_module->MBAUD;              // load the old Baud value
    if (newBaud != oldBaud) {                             // compare both, in case the code is issuing this before every transmission.
      uint8_t restore = _data->_module->MCTRLA;           // Save the old Master state
      _data->_module->MCTRLA    = 0;                      // Disable Master
      _data->_module->MBAUD     = newBaud;                // update Baud register
      if (frequency > 400000) {
        _data->_module->CTRLA  |=  TWI_FMPEN_bm;          // Enable FastMode+
      } else {
        _data->_module->CTRLA  &= ~TWI_FMPEN_bm;          // Disable FastMode+
      }
      _data->_module->MCTRLA    = restore;                // restore the old register, thus enabling it again
      if (restore & TWI_ENABLE_bm) {                      // If the TWI was enabled,
        _data->_module->MSTATUS   = TWI_BUSSTATE_IDLE_gc;   // Force the state machine into IDLE according to the data sheet
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
uint8_t TWI_MasterCalcBaud(uint32_t frequency) {
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
 *@brief      TWI_MasterWrite performs a host write operation on the TWI bus
 *
 *            As soon as the bus is in an idle state, a polled write operation is performed
 *            A STOP condition can be send at the end, or not if a REP START is wanted
 *            The user has to make sure to have a host write or read at the end with a STOP
 *
 *
 *@param      struct twiData *_data is a pointer to the structure that holds the Wire variables
 *@param      bool send_stop enables the STOP condition at the end of a write
 *
 *@return     uint8_t
 *@retval     amount of bytes that were written. If 0, no write took place, either due
 *            to an error or because of an empty txBuffer
 */
uint8_t TWI_MasterWrite(struct twiData *_data, bool send_stop)  {
  #if defined(TWI_MERGE_BUFFERS)                              // Same Buffers for tx/rx
    uint8_t* txHead   = &(_data->_bytesToReadWrite);
    uint8_t* txBuffer =   _data->_trBuffer;
  #else                                                       // Separate tx/rx Buffers
    uint8_t* txHead   = &(_data->_bytesToWrite);
    uint8_t* txBuffer =   _data->_txBuffer;
  #endif

  TWI_t *module = _data->_module;     // Compiler treats the pointer to the TWI module as volatile and
                                      // creates bloat-y code, this fixes it
  TWI_INIT_ERROR;
  uint8_t currentSM;
  uint8_t currentStatus;
  uint8_t dataWritten = 0;
  uint16_t timeout = 0;


    if (((module->MSTATUS & TWI_BUSSTATE_gm) == TWI_BUSSTATE_UNKNOWN_gc) || // If the bus was not initialized
      ((module->MCTRLA & TWI_ENABLE_bm) == false)) {  // Or is disabled,
      return TWI_ERR_UNINIT;                          // return
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

    if   (currentStatus & (TWI_ARBLOST_bm | TWI_BUSERR_bm)) {     // Check for Bus error
        module->MSTATUS = (TWI_ARBLOST_bm | TWI_BUSERR_bm);       // reset error flags
        TWI_SET_ERROR(TWI_ERR_BUS_ARB);                           // set error flag
        break;                                                    // leave RX loop
    }

    if (currentSM == TWI_BUSSTATE_IDLE_gc) {                      // Bus has not sent START yet and is not BUSY
        module->MADDR = ADD_WRITE_BIT(_data->_clientAddress);
        timeout = 0;
    } else if (currentSM == TWI_BUSSTATE_OWNER_gc) {              // Address was sent, host is owner
      if     (currentStatus & TWI_WIF_bm) {                       // data sent
        if   (currentStatus & TWI_RXACK_bm) {                     // AND the RXACK bit is set, last byte has failed
          if (dataWritten == 0) TWI_SET_ERROR(TWI_ERR_ACK_ADR);   // if dataWritten is 0, no payload was sent, so address was NACKed
          else                  TWI_SET_ERROR(TWI_ERR_ACK_DAT);   // else payload was NACKed
          break;                                                  // leave loop
        } else {                                                  // otherwise WRITE was ACKed
          if (dataWritten < (*txHead)) {                          // check if there is data to be written
            module->MDATA = txBuffer[dataWritten];                // Writing to the register to send data
            dataWritten++;                                        // data was Written
            timeout = 0;                                          // reset timeout
          } else {                                                // else there is no data to be written
            break;                                                // TX finished, leave loop, error is still TWI_NO_ERR
          }
        }
      }
    }
  }


  if ((send_stop != 0) || (TWI_ERR_SUCCESS != TWI_GET_ERROR)) {
    module->MCTRLB = TWI_MCMD_STOP_gc;                        // Send STOP
  }
  return TWI_GET_ERROR;
}


/**
 *@brief      TWI_MasterRead performs a host read operation on the TWI bus
 *
 *            As soon as the bus is in an idle state, a polled read operation is performed
 *            A STOP condition can be send at the end, or not if a REP START is wanted
 *            The user has to make sure to have a host write or read at the end with a STOP
 *
 *
 *@param      struct twiData *_data is a pointer to the structure that holds the Wire variables
 *@param      uint8_t bytesToRead is the desired amount of bytes to read. When finished, a
 *              NACK is issued.
 *@param      bool send_stop enables the STOP condition at the end of a write
 *
 *@return     uint8_t amount of actually read bytes
 *@retval     amount of bytes that were actually read. If 0, no read took place due to a bus error
 */
uint8_t TWI_MasterRead(struct twiData *_data, uint8_t bytesToRead, bool send_stop) {
  #if defined(TWI_MERGE_BUFFERS)                                // Same Buffers for tx/rx
    uint8_t* rxHead   = &(_data->_bytesToReadWrite);
    uint8_t* rxTail   = &(_data->_bytesReadWritten);
    uint8_t* rxBuffer =   _data->_trBuffer;
  #else                                                         // Separate tx/rx Buffers
    uint8_t* rxHead   = &(_data->_bytesToRead);
    uint8_t* rxTail   = &(_data->_bytesRead);
    uint8_t* rxBuffer =   _data->_rxBuffer;
  #endif

  (*rxTail) = 0;                      // Reset counter

  TWI_t *module = _data->_module;     // Compiler treats the pointer to the TWI module as volatile and
                                      // creates bloat-y code, using a local variable fixes that

  TWIR_INIT_ERROR;             // local variable for errors
  uint8_t dataRead = 0;

  if ((module->MSTATUS & TWI_BUSSTATE_gm) != TWI_BUSSTATE_UNKNOWN_gc) {
    uint8_t currentSM;
    uint8_t currentStatus;
    uint8_t command  = 0;
    uint16_t timeout = 0;

    module->MADDR = ADD_READ_BIT(_data->_clientAddress);  // Send Address with read bit

    while (true) {
      currentStatus = module->MSTATUS;
      currentSM = currentStatus & TWI_BUSSTATE_gm;  // get the current mode of the state machine

      #if defined(TWI_TIMEOUT_ENABLE)
        if (++timeout > (F_CPU/1000)) {
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

      if (currentStatus & (TWI_ARBLOST_bm | TWI_BUSERR_bm)) {   // Check for Bus error
        module->MSTATUS = (TWI_ARBLOST_bm | TWI_BUSERR_bm);     // reset error flags
        TWIR_SET_ERROR(TWI_ERR_BUS_ARB);                        // set error flag
        break;                                                  // leave TX loop
      }

      if (command != 0) {
        if (currentSM == TWI_BUSSTATE_OWNER_gc) {
          module->MCTRLB = command;
        } else {
          break;
        }
      }

      if (currentSM == TWI_BUSSTATE_OWNER_gc) {  // Address sent, check for WIF/RIF
        if (currentStatus & TWI_RIF_bm) {         // data received
          if (dataRead < BUFFER_LENGTH) {          // Buffer still free
            rxBuffer[dataRead] = module->MDATA;      // save byte in the Buffer.
            dataRead++;                              // increment read counter
            timeout = 0;                             // reset timeout

            if (dataRead < bytesToRead) {            // expecting more bytes, so
              module->MCTRLB = TWI_MCMD_RECVTRANS_gc;  // send an ACK so the Slave so it can send the next byte
            } else {                                 // Otherwise,
              if (send_stop != 0) {
                command = TWI_ACKACT_bm | TWI_MCMD_STOP_gc;   // send STOP + NACK
              } else {
                break;
              }
            }

          } else {                                        // Buffer overflow with the incoming byte
            TWIR_SET_ERROR(TWI_ERR_BUF_OVERFLOW);         // Set Error and
            command = TWI_ACKACT_bm | TWI_MCMD_STOP_gc;   // send STOP + NACK
          }
        } else if (currentStatus & TWI_WIF_bm) {  // Address NACKed
          TWIR_SET_ERROR(TWI_ERR_RXACK);          // set error flag
          command = TWI_MCMD_STOP_gc;             // free the bus
        }
      }
    }
    (*rxHead) = dataRead;
  } else {
    TWIR_SET_ERROR(TWI_ERR_UNINIT);
  }
  #if defined(TWI_READ_ERROR_ENABLED) && defined(TWI_ERROR_ENABLED)
    _data->_errors = TWIR_GET_ERROR;                // save error flags
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
 *@param      struct twiData *_data is a pointer to the structure that holds the Wire variables
 *
 *@return     void
 */
void TWI_HandleSlaveIRQ(struct twiData *_data) {
  #if defined(TWI_MANDS)                            // Master and Slave split
    uint8_t* txHead   = &(_data->_bytesToReadWriteS);
    uint8_t* rxHead   = &(_data->_bytesToReadWriteS);

  #else                                             // Slave using the host buffer
    #if defined(TWI_MERGE_BUFFERS)                  // Same Buffers for tx/rx
      uint8_t* txHead   = &(_data->_bytesToReadWrite);
      uint8_t* rxHead   = &(_data->_bytesToReadWrite);
    #else                                           // Separate tx/rx Buffers
      uint8_t* txHead   = &(_data->_bytesToWrite);
      uint8_t* rxHead   = &(_data->_bytesToRead);
    #endif
  #endif

  uint8_t clientStatus = _data->_module->SSTATUS;

  if (clientStatus & (TWI_BUSERR_bm | TWI_COLL_bm)) {   // if Bus error/Collision was detected
    _data->_module->SDATA;                              // Read data to remove Status flags
    (*rxHead) = 0;                                      // Abort
    (*txHead) = 0;                                      // Abort
  } else {                                              // No Bus error/Collision was detected
    #if defined(TWI_MANDS)
      _data->_bools._toggleStreamFn = 0x01;             // reroute stream functions to slave elements
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
 *@param      struct twiData *_data is a pointer to the structure that holds the Wire variables
 *
 *@return     void
 */
void SlaveIRQ_AddrRead(struct twiData *_data) {
  #if defined(TWI_MANDS)                            // Master and Slave split
    uint8_t* address = &(_data->_incomingAddress);
    uint8_t* txHead  = &(_data->_bytesToReadWriteS);
    uint8_t* txTail  = &(_data->_bytesReadWrittenS);
  #else                                             // Slave using the Master buffer
    uint8_t* address = &(_data->_clientAddress);
    #if defined(TWI_MERGE_BUFFERS)                  // Same Buffers for tx/rx
      uint8_t* txHead   = &(_data->_bytesToReadWrite);
      uint8_t* txTail   = &(_data->_bytesReadWritten);
    #else                                           // Separate tx/rx Buffers
      uint8_t* txHead   = &(_data->_bytesToWrite);
      uint8_t* txTail   = &(_data->_bytesWritten);
    #endif
  #endif

  (*address) = _data->_module->SDATA;       // saving address to expose to the user sketch

                                            // There is no way to identify a REPSTART, so when a Master Read occurs after a Master write
  NotifyUser_onReceive(_data);              // Notify user program "onReceive" if necessary

  (*txHead) = 0;                            // reset buffer positions so the Slave can start writing at zero.
  (*txTail) = 0;

  NotifyUser_onRequest(_data);              // Notify user program "onRequest" if necessary
  _data->_module->SCTRLB = TWI_SCMD_RESPONSE_gc;  // "Execute Acknowledge Action succeeded by client data interrupt"
}


/**
 *@brief      SlaveIRQ_AddrWrite is a subroutine of TWI_HandleSlaveIRQ and handles the Address Write case
 *
 *@param      struct twiData *_data is a pointer to the structure that holds the Wire variables
 *
 *@return     void
 */
void SlaveIRQ_AddrWrite(struct twiData *_data) {
  #if defined(TWI_MANDS)                            // Master and Slave split
    uint8_t* address = &(_data->_incomingAddress);
    uint8_t* rxHead  = &(_data->_bytesToReadWriteS);
    uint8_t* rxTail  = &(_data->_bytesReadWrittenS);

  #else                                             // Slave using the Master buffer
    uint8_t*    address = &(_data->_clientAddress);
    #if defined(TWI_MERGE_BUFFERS)                  // Same Buffers for tx/rx
      uint8_t* rxHead   = &(_data->_bytesToReadWrite);
      uint8_t* rxTail   = &(_data->_bytesReadWritten);
    #else
      uint8_t* rxHead   = &(_data->_bytesToRead);
      uint8_t* rxTail   = &(_data->_bytesRead);
    #endif
  #endif

  (*address) = _data->_module->SDATA;
  (*rxHead) = 0;                                    // reset buffer positions so the Master can start writing at zero.
  (*rxTail) = 0;
  _data->_module->SCTRLB = TWI_SCMD_RESPONSE_gc;    // "Execute Acknowledge Action succeeded by reception of next byte"
}


/**
 *@brief      SlaveIRQ_Stop is a subroutine of TWI_HandleSlaveIRQ and handles the STOP case
 *
 *@param      struct twiData *_data is a pointer to the structure that holds the Wire variables
 *
 *@return     void
 */
void SlaveIRQ_Stop(struct twiData *_data) {
  #if defined(TWI_MANDS)                            // Master and Slave split
    uint8_t* rxHead   = &(_data->_bytesToReadWriteS);
    uint8_t* rxTail   = &(_data->_bytesReadWrittenS);
  #else                                             // Slave using the Master buffer
    #if defined(TWI_MERGE_BUFFERS)                  // Same Buffers for tx/rx
      uint8_t* rxHead   = &(_data->_bytesToReadWrite);
      uint8_t* rxTail   = &(_data->_bytesReadWritten);
    #else                                           // Separate tx/rx Buffers
      uint8_t* rxHead   = &(_data->_bytesToRead);
      uint8_t* rxTail   = &(_data->_bytesRead);
    #endif
  #endif

  _data->_module->SSTATUS = TWI_APIF_bm;      // Clear Flag, no further action needed
  NotifyUser_onReceive(_data);                // Notify user program "onReceive" if necessary
  (*rxHead) = 0;                              // User should have handled all data, if not, set available rxBytes to 0
  (*rxTail) = 0;
}

/**
 *@brief      SlaveIRQ_DataReadNack is a subroutine of TWI_HandleSlaveIRQ and handles the NACK after a byte transmission
 *
 *@param      struct twiData *_data is a pointer to the structure that holds the Wire variables
 *
 *@return     void
 */
void SlaveIRQ_DataReadNack(struct twiData *_data) {
  #if defined(TWI_MANDS)                            // Master and Slave split
    uint8_t* txHead   = &(_data->_bytesToReadWriteS);
  #else                                             // Slave using the host buffer
    #if defined(TWI_MERGE_BUFFERS)                  // Same Buffers for tx/rx
      uint8_t* txHead   = &(_data->_bytesToReadWrite);
    #else                                           // Separate tx/rx Buffers
      uint8_t* txHead   = &(_data->_bytesToWrite);
    #endif
  #endif


  _data->_bools._ackMatters = false;                // stop checking for NACK
  _data->_module->SCTRLB = TWI_SCMD_COMPTRANS_gc;   // "Wait for any Start (S/Sr) condition"
  (*txHead) = 0;                                    // Abort further data writes
}

/**
 *@brief      SlaveIRQ_DataReadAck is a subroutine of TWI_HandleSlaveIRQ and handles the ACK after a byte transmission
 *
 *@param      struct twiData *_data is a pointer to the structure that holds the Wire variables
 *
 *@return     void
 */
void SlaveIRQ_DataReadAck(struct twiData *_data) {
  #if defined(TWI_MANDS)                            // Master and Slave split
    uint8_t* txHead   = &(_data->_bytesToReadWriteS);
    uint8_t* txTail   = &(_data->_bytesReadWrittenS);
    uint8_t* txBuffer =   _data->_trBufferS;
  #else                                             // Slave using the host buffer
    #if defined(TWI_MERGE_BUFFERS)                  // Same Buffers for tx/rx
      uint8_t* txHead   = &(_data->_bytesToReadWrite);
      uint8_t* txTail   = &(_data->_bytesReadWritten);
      uint8_t* txBuffer =   _data->_trBuffer;
    #else                                           // Separate tx/rx Buffers
      uint8_t* txHead   = &(_data->_bytesToWrite);
      uint8_t* txTail   = &(_data->_bytesWritten);
      uint8_t* txBuffer =   _data->_txBuffer;
    #endif
  #endif

  _data->_bools._ackMatters = true;             // start checking for NACK
  if ((*txTail) < (*txHead)) {                  // Data is available
    _data->_module->SDATA = txBuffer[(*txTail)];    // Writing to the register to send data
    _data->_module->SCTRLB = TWI_SCMD_RESPONSE_gc;  // "Execute a byte read operation followed by Acknowledge Action"
    (*txTail)++;                                    // Increment counter for sent bytes

  } else {                                            // No more data available
    _data->_module->SCTRLB = TWI_SCMD_COMPTRANS_gc;   // "Wait for any Start (S/Sr) condition"
  }
}


/**
 *@brief      SlaveIRQ_DataWrite is a subroutine of TWI_HandleSlaveIRQ and handles the receiving of a byte
 *
 *@param      struct twiData *_data is a pointer to the structure that holds the Wire variables
 *
 *@return     void
 */
void SlaveIRQ_DataWrite(struct twiData *_data) {
  #if defined(TWI_MANDS)                            // Master and Slave split
      uint8_t* rxHead   = &(_data->_bytesToReadWriteS);
      uint8_t* rxTail   = &(_data->_bytesReadWrittenS);
      uint8_t* rxBuffer =   _data->_trBufferS;
  #else                                             // Slave using the host buffer
    #if defined(TWI_MERGE_BUFFERS)                  // Same Buffers for tx/rx
      uint8_t* rxHead   = &(_data->_bytesToReadWrite);
      uint8_t* rxTail   = &(_data->_bytesReadWritten);
      uint8_t* rxBuffer =   _data->_trBuffer;
    #else                                           // Separate tx/rx Buffers
      uint8_t* rxHead   = &(_data->_bytesToRead);
      uint8_t* rxTail   = &(_data->_bytesRead);
      uint8_t* rxBuffer =   _data->_rxBuffer;
    #endif
  #endif


  uint8_t payload = _data->_module->SDATA;

  if ((*rxHead) < BUFFER_LENGTH) {                // if buffer is not full
    rxBuffer[(*rxHead)] = payload;                  // Load data into the buffer
    (*rxHead)++;                                    // Advance Head
    _data->_module->SCTRLB = TWI_SCMD_RESPONSE_gc;  // "Execute Acknowledge Action succeeded by reception of next byte"
  } else {                                          // else buffer is full
    _data->_module->SCTRLB = TWI_ACKACT_bm | TWI_SCMD_COMPTRANS_gc;  // "Execute ACK Action succeeded by waiting for any Start (S/Sr) condition"
    (*rxHead) = 0;                                           // Dismiss all received Data since data integrity can't be guaranteed
    (*rxTail) = 0;  // Make sure available will return 0
  }
}

/**
 *@brief      NotifyUser_onRequest is called from the TWI_HandleSlaveIRQ function on host READ
 *
 *            This function calls the user defined function in the sketch if it was
 *            registered. It is issued on host READ
 *
 *@param      struct twiData *_data is a pointer to the structure that holds the Wire variables
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
 *@param      struct twiData *_data is a pointer to the structure that holds the Wire variables
 *
 *@return     void
 */
void NotifyUser_onReceive(struct twiData *_data) {
  #if defined(TWI_MANDS)                            // Master and Slave split
    uint8_t* rxHead   = &(_data->_bytesToReadWriteS);
  #else                                             // Slave using the Master buffer
    #if defined(TWI_MERGE_BUFFERS)                  // Same Buffers for tx/rx
      uint8_t* rxHead   = &(_data->_bytesToReadWrite);
    #else                                           // Separate tx/rx Buffers
      uint8_t* rxHead   = &(_data->_bytesToRead);
    #endif
  #endif
  if (_data->user_onReceive != NULL) {
    if ((*rxHead) > 0) {
      _data->user_onReceive((*rxHead));
    }
  }
}
