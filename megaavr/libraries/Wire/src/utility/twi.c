/******************************************************************************
  © 2018 Microchip Technology Inc. and its subsidiaries.

  Subject to your compliance with these terms, you may use Microchip software
  and any derivatives exclusively with Microchip products. It is your
  responsibility to comply with third party license terms applicable to your
  use of third party software (including open source software) that may
  accompany Microchip software.

  THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS".  NO WARRANTIES, WHETHER
  EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
  WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR
  PURPOSE. IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL,
  PUNITIVE, INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY
  KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP
  HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE
  FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
  ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
  THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.

 *****************************************************************************/

#include "twi.h"
#include "Arduino.h"

/* Master variables */
static register8_t  master_slaveAddress;                       /*!< Slave address */
static register8_t *master_writeData;                          /*!< Data to write */
static register8_t *master_readData;                           /*!< Read data */
static register8_t  master_bytesToWrite;                       /*!< Number of bytes to write */
static register8_t  master_bytesToRead;                        /*!< Number of bytes to read */
static register8_t  master_bytesWritten;                       /*!< Number of bytes written */
static register8_t  master_bytesRead;                          /*!< Number of bytes read */
static register8_t  master_sendStop;                           /*!< To send a stop at the end of the transaction or not */
static register8_t  master_trans_status;                       /*!< Status of transaction */
static register8_t  master_result;                             /*!< Result of transaction */

/* Slave variables */
static uint8_t (*TWI_onSlaveTransmit)(void) __attribute__((unused));
static void (*TWI_onSlaveReceive)(int) __attribute__((unused));
static register8_t *slave_writeData;
static register8_t *slave_readData;
static register8_t  slave_bytesToWrite;
static register8_t  slave_bytesWritten;
static register8_t  slave_bytesToRead;
static register8_t  slave_bytesRead;
static register8_t  slave_trans_status;
static register8_t  slave_result;
static register8_t  slave_callUserReceive;
static register8_t  slave_callUserRequest;

/* TWI module mode */
static volatile TWI_MODE_t twi_mode;

/*! \brief Initialize the TWI module as a master.

    TWI master initialization function.
    Enables master read and write interrupts.
    Remember to enable interrupts globally from the main application.

    \param frequency            The required baud.
*/
void TWI_MasterInit(uint32_t frequency) {
  if (twi_mode != TWI_MODE_UNKNOWN) {
    return;
  }
  #ifdef PORTMUX_TWIROUTEA
  if ((PORTMUX.TWIROUTEA & PORTMUX_TWI0_gm) == PORTMUX_TWI0_ALT2_gc) {
    // make sure we don't get errata'ed - make sure their bits in the
    // PORTx.OUT registers are 0.
    PORTC.OUTCLR = 0x0C; // bits 2 and 3
  } else {
    PORTA.OUTCLR = 0x0C; // bits 2 and 3
  }
  #else // megaTinyCore
  #if defined(PORTMUX_TWI0_bm)
  if ((PORTMUX.CTRLB & PORTMUX_TWI0_bm)) {
    // make sure we don't get errata'ed - make sure their bits in the
    // PORTx.OUT registers are 0.
    PORTA.OUTCLR = 0x06; // if swapped it's on PA1, PA2
  } else {
    PORTB.OUTCLR = 0x03; // else PB0, PB1
  }
  #elif defined(__AVR_ATtinyxy2__)
  PORTA.OUTCLR = 0x06; // 8-pin parts always have it on PA1/2
  #else
  PORTB.OUTCLR = 0x03; // else, zero series, no remapping, it's on PB0, PB1
  #endif
  #endif

  twi_mode = TWI_MODE_MASTER;

  master_bytesRead = 0;
  master_bytesWritten = 0;
  master_trans_status = TWIM_STATUS_READY;
  master_result = TWIM_RESULT_UNKNOWN;

  TWI_MasterSetBaud(frequency); // Datasheet says to set this up **before** enabling TWI.
  TWI0.MCTRLA = TWI_RIEN_bm | TWI_WIEN_bm | TWI_ENABLE_bm;
  TWI0.MSTATUS = TWI_BUSSTATE_IDLE_gc;
}

/*! \brief Initialize the TWI module as a slave.

    TWI slave initialization function.
    Enables slave address/stop and data interrupts.
    Assigns slave's own address.
    Remember to enable interrupts globally from the main application.

    \param address            The TWI Slave's own address.
*/

void TWI_SlaveInit(uint8_t address, uint8_t receive_broadcast, uint8_t second_address) {
  if (twi_mode != TWI_MODE_UNKNOWN) {
    return;
  }

  #ifdef PORTMUX_TWIROUTEA
  if ((PORTMUX.TWIROUTEA & PORTMUX_TWI0_gm) == PORTMUX_TWI0_ALT2_gc) {
    // make sure we don't get errata'ed - make sure their bits in the
    // PORTx.OUT registers are 0.
    PORTC.OUTCLR = 0x0C; // bits 2 and 3
  } else {
    PORTA.OUTCLR = 0x0C; // bits 2 and 3
  }
  #else // megaTinyCore
  #if defined(PORTMUX_TWI0_bm)
  if ((PORTMUX.CTRLB & PORTMUX_TWI0_bm)) {
    // make sure we don't get errata'ed - make sure their bits in the
    // PORTx.OUT registers are 0.
    PORTA.OUTCLR = 0x06; // if swapped it's on PA1, PA2
  } else {
    PORTB.OUTCLR = 0x03; // else PB0, PB1
  }
  #elif defined(__AVR_ATtinyxy2__)
  PORTA.OUTCLR = 0x06; // 8-pin parts always have it on PA1/2
  #else
  PORTB.OUTCLR = 0x03; // else, zero series, no remapping, it's on PB0, PB1
  #endif
  #endif

  twi_mode = TWI_MODE_SLAVE;

  slave_bytesRead = 0;
  slave_bytesWritten = 0;
  slave_trans_status = TWIS_STATUS_READY;
  slave_result = TWIS_RESULT_UNKNOWN;
  slave_callUserRequest = 0;
  slave_callUserReceive = 0;

  TWI0.SADDR = address << 1 | receive_broadcast;
  TWI0.SADDRMASK = second_address;
  TWI0.SCTRLA = TWI_DIEN_bm | TWI_APIEN_bm | TWI_PIEN_bm  | TWI_ENABLE_bm;

  /* Bus Error Detection circuitry needs Master enabled to work */
  TWI0.MCTRLA = TWI_ENABLE_bm;
}

void TWI_Flush(void) {
  TWI0.MCTRLB |= TWI_FLUSH_bm;
}

/*! \brief Disable the TWI module.

    TWI module disable function.
    Disables both master and slave modes

    \param frequency            The required baud.
*/
void TWI_Disable(void) {
  TWI0.MCTRLA = 0x00;
  TWI0.MBAUD = 0x00;
  TWI0.MSTATUS = TWI_BUSSTATE_IDLE_gc;
  TWI0.SADDR = 0x00;
  TWI0.SCTRLA = 0x00;
  TWI0.SADDRMASK = 0;
  twi_mode = TWI_MODE_UNKNOWN;
}

/*! \brief Returns the TWI bus state.

    Returns the TWI bus state (type defined in device headerfile),
    unknown, idle, owner or busy.

    \param twi The TWI_Master_t struct instance.

    \retval TWI_MASTER_BUSSTATE_UNKNOWN_gc Bus state is unknown.
    \retval TWI_MASTER_BUSSTATE_IDLE_gc    Bus state is idle.
    \retval TWI_MASTER_BUSSTATE_OWNER_gc   Bus state is owned by the master.
    \retval TWI_MASTER_BUSSTATE_BUSY_gc    Bus state is busy.
*/
TWI_BUSSTATE_t TWI_MasterState(void) {
  TWI_BUSSTATE_t twi_status;
  twi_status = (TWI_BUSSTATE_t)(TWI0.MSTATUS & TWI_BUSSTATE_gm);
  return twi_status;
}


/*! \brief Returns true if transaction is ready.

    This function returns a boolean whether the TWI Master is ready
    for a new transaction.

    \param twi The TWI_Master_t struct instance.

    \retval true  If transaction could be started.
    \retval false If transaction could not be started.
*/
uint8_t TWI_MasterReady(void) {
  uint8_t twi_status = (master_trans_status & TWIM_STATUS_READY);
  return twi_status;
}

/*! \brief Set the TWI baud rate.

    Sets the baud rate used by TWI Master.

    \param frequency            The required baud.
*/
void TWI_MasterSetBaud(uint32_t frequency) {
  /*                            THIS IS A TARGET SPEED
   * ACTUAL SPEED WILL DEPEND ON TOPOLOGY, BUS CAPACITANCE AND PULLUP STRENGTH
   *
   * uint32_t baud = (F_CPU / frequency - F_CPU / 1000 / 1000 * t_rise / 1000 - 10) / 2;
   *
   * Trise (worst case) assumed to be 1000ns @ 100kHz / 300ns @ 400kHz / 120ns @ 1MHz
   *
   * What a godawful mess that was. It didn't catch rollovers from trying to use too high of a
   * SCL frequency. The following is ugly - but it's efficient and I think it works.
   * Correlations used found by hand in Excel via numerical methods.
   * Issues openedd to complain about this halfassed method without a test case that passes in
   * megaTinyCore 2.3.1 and fails in megaTinyCore 2.3.2 (DxCore 1.3.5 and 1.3.6) will not
   * be entertained unless the reporting user has written a better implementation and tested it!
   * Why is setting the clock speed so damned hard, when it's easy for every other situationb?
   * mostly because now the master listens to the bus while it is driving it, to adapt the speed
   * of the clock to deal with the bus topology.  the high and low on the clock
   * so **it is impossible to determine** from the all relevant configuration registers, what
   * speed the TWI master will run at. That can only be determined with all peripherals
   * connected using their final cables, with test equipment connected to the I2C pins.
   *
   * So yes, these calculation methods are crude and approximate!
   */


  int16_t baud = 0;
  if (frequency < 80000) {// they want to run it slowly. Half-speed standard mode, ~50 kHz
    baud = (F_CPU / 100000) - 5;
  } else if (frequency <= 150000) {
    // they get standard mode, or the closest they can get with their hardware
    baud = (F_CPU / 200000) - 5;
  } else if (frequency <= 300000) {
    // halved speed fast mode
    baud = ((F_CPU / 1000000) * 3) - 5;
  } else if (frequency <= 600000 || ((TWI0.MCTRLA & TWI_ENABLE_bm) && !(TWI0.CTRLA & TWI_FMPEN_bm))) {
    // If frequency is more than 300 kHz (else previous case would catch it) and not more than 600 kHz
    // OR even if it is more than that, but the TWI interface is enabled and fast mode isn't, since
    // per datasheet that must be confighured prior to enabling the TWI, they get 400 kHz FM
    //
    // 400 kHz fast mode. 8 MHz is the slowest that can do it.
    #if F_CPU >= 8000000
    baud = ((F_CPU / 1000000) - 5);
    #endif
    #if F_CPU >= 12000000
  } else if (frequency < 900000) {
    #else
  } else {
    #endif
    // 3/4ths of FMPlus speed
    #if F_CPU >= 10000000
    baud = ((F_CPU / 1500000) - 5);
    #endif
    #if F_CPU >= 16000000
  } else {
    // they want to go as fast as they can...
    // Makes optimistic assumptions about the hardware and bus topology...
    baud = ((F_CPU / 2000000) - 6);
    #endif
  }
  if (baud < 1) {
    // Requested speed exceeded hardware capabilities.
    // use the fastest one that F_CPU should work with...
    #if (F_CPU >= 8000000)
    // FM full speed
    baud = ((F_CPU / 1000000) - 5);
    #elif (F_CPU >= 4000000)
    // half speed FM
    baud = ((F_CPU / 1000000) * 3) - 5;
    #else
    baud = 5;
    // 50 kHz @ 1 MHz, 100kHz @ 2 MHz.
    // should always work/
    #endif
  } else {
    // Baud is 1 or higher
    #if defined(DXCORE) && F_CPU >= 28000000
    if (baud > 255) {
      /* has to fit in 8-bit register; this is reachable for the slowest speed, and only with
       * F_CPU >= 28 MHz so we only include this on overclocked Dx-series parts (the tinyAVR
       * and megaAVR devices are generally not usable at 28 MHz, and I don't want to run risk
       * of the optimizer not realizing this is unreachable with current F_CPU and building
       * this test for the flash-constrained tiny parts, Dx is usually good up to 32, and
       * has plenty of flash so even if the so it's no big deal to waste a couple of words
       * on this */
      baud = 255;
    } else if (baud < 3) {
    #else
    if (baud < 3) {
    #endif
      baud = 3;
    }
    if (frequency > 600000 && !TWI0.MCTRLA & TWI_ENABLE_bm) {
      // Sigh, well, TWI is disabled, and we just set the baud to a valid value for FM+
      // I guess we had better make sure FMPEN is set...
      TWI0.CTRLA |= TWI_FMPEN_bm;
      // I feel like this is a bad thing to do, but on the other hand, the previous code
      // would just merrily set the BAUD register to generate FMP speeds....
      // whether FMP was enabled or not, and that certainly isn't good either.
    }
  }


  TWI0.MBAUD = (uint8_t)baud;

  // 1/16/2021:
  // Prevent an integer overflow that can result in incorrect baud rates.
  // Arduino megaAVR #90.
  // 1/23/2021:
  // Grab the rest of the fix and add it in here.
  // 4/28/2021:
  // I give up. Use numerically determinined linearizations which will be calculated at compile time. And make sure to permit a bucket slow enough that 1 MHz parts work on it.
  // If we can't make the speed bucket requested, try to use fastest one we can. 2 speeds for each mode, half and full, that is 50 kHz and 100 kHz, 200kHz, and 400kHz, and 750 kHz and 1 MHz.
  // If TWI isn't yet enabled, and user calls a function that initializes the master baud rate to an FMP speed, we will now enable it.
}

/*! \brief TWI write transaction.

    This function is TWI Master wrapper for a write-only transaction.

    \param twi          The TWI_Master_t struct instance.
    \param address      Slave address.
    \param writeData    Pointer to data to write.
    \param bytesToWrite Number of data bytes to write.

    \retval true  If transaction could be started.
    \retval false If transaction could not be started.
*/
uint8_t TWI_MasterWrite(uint8_t slave_address,
                        uint8_t *write_data,
                        uint8_t bytes_to_write,
                        uint8_t send_stop) {
  return TWI_MasterWriteRead(slave_address,
                             write_data,
                             bytes_to_write,
                             0,
                             send_stop);
}


/*! \brief TWI read transaction.

    This function is a TWI Master wrapper for read-only transaction.

    \param twi            The TWI_Master_t struct instance.
    \param address        The slave address.
    \param bytesToRead    The number of bytes to read.

    \retval true  If transaction could be started.
    \retval false If transaction could not be started.
*/
uint8_t TWI_MasterRead(uint8_t slave_address,
                       uint8_t *read_data,
                       uint8_t bytes_to_read,
                       uint8_t send_stop) {
  master_readData = read_data;

  uint8_t bytes_read = TWI_MasterWriteRead(slave_address,
                       0,
                       0,
                       bytes_to_read,
                       send_stop);
  return bytes_read;
}


/*! \brief TWI write and/or read transaction.

    This function is a TWI Master write and/or read transaction. The function
    can be used to both write and/or read bytes to/from the TWI Slave in one
    transaction.

    \param twi            The TWI_Master_t struct instance.
    \param address        The slave address.
    \param writeData      Pointer to data to write.
    \param bytesToWrite   Number of bytes to write.
    \param bytesToRead    Number of bytes to read.

    \retval true  If transaction could be started.
    \retval false If transaction could not be started.
*/
uint8_t TWI_MasterWriteRead(uint8_t slave_address,
                            uint8_t *write_data,
                            uint8_t bytes_to_write,
                            uint8_t bytes_to_read,
                            uint8_t send_stop) {
  if (twi_mode != TWI_MODE_MASTER) {
    return false;
  }

  /*Initiate transaction if bus is ready. */
  if (master_trans_status == TWIM_STATUS_READY) {

    master_trans_status = TWIM_STATUS_BUSY;
    master_result = TWIM_RESULT_UNKNOWN;

    master_writeData = write_data;

    master_bytesToWrite = bytes_to_write;
    master_bytesToRead = bytes_to_read;
    master_bytesWritten = 0;
    master_bytesRead = 0;
    master_sendStop = send_stop;
    master_slaveAddress = slave_address << 1;

  trigger_action:

    /* If write command, send the START condition + Address +
       'R/_W = 0'
    */
    if (master_bytesToWrite > 0) {
      twi_mode = TWI_MODE_MASTER_TRANSMIT;
      uint8_t writeAddress = ADD_WRITE_BIT(master_slaveAddress);
      TWI0.MADDR = writeAddress;
    }

    /* If read command, send the START condition + Address +
       'R/_W = 1'
    */
    else if (master_bytesToRead > 0) {
      twi_mode = TWI_MODE_MASTER_RECEIVE;
      uint8_t readAddress = ADD_READ_BIT(master_slaveAddress);
      TWI0.MADDR = readAddress;
    }

    else if (master_bytesToWrite == 0 && master_bytesToRead == 0) {
      twi_mode = TWI_MODE_MASTER_TRANSMIT;
      uint8_t writeAddress = ADD_WRITE_BIT(master_slaveAddress);
      TWI0.MADDR = writeAddress;
    }

    /* Arduino requires blocking function */
    while (master_result == TWIM_RESULT_UNKNOWN) {}

    // in case of arbitration lost, retry sending
    if (master_result == TWIM_RESULT_ARBITRATION_LOST) {
      goto trigger_action;
    }

    uint8_t ret = 0;
    if (master_bytesToRead > 0) {
      // return bytes really read
      ret = master_bytesRead;
    } else {
      // return 0 if success, >0 otherwise (follow classic AVR conventions)
      switch (master_result) {
        case TWIM_RESULT_OK:
          ret = 0;
          break;
        case TWIM_RESULT_BUFFER_OVERFLOW:
          ret = 1;
          break;
        case TWIM_RESULT_NACK_RECEIVED:
          ret = 3;
          break;
        default:
          ret = 4;
          break;
      }
    }

    return ret;
  } else {
    return 1;
  }
}


/*! \brief Common TWI master interrupt service routine.

    Check current status and calls the appropriate handler.

*/
void TWI_MasterInterruptHandler() {
  uint8_t currentStatus = TWI0.MSTATUS;

  /* If arbitration lost or bus error. */
  if ((currentStatus & TWI_ARBLOST_bm) ||
      (currentStatus & TWI_BUSERR_bm)) {

    TWI_MasterArbitrationLostBusErrorHandler();
  }

  /* If master write interrupt. */
  else if (currentStatus & TWI_WIF_bm) {
    TWI_MasterWriteHandler();
  }

  /* If master read interrupt. */
  else if (currentStatus & TWI_RIF_bm) {
    TWI_MasterReadHandler();
  }

  /* If unexpected state. */
  else {
    TWI_MasterTransactionFinished(TWIM_RESULT_FAIL);
  }
}

/*! \brief TWI master arbitration lost and bus error interrupt handler.

    Handles TWI responses to lost arbitration and bus error.

*/
void TWI_MasterArbitrationLostBusErrorHandler() {
  uint8_t currentStatus = TWI0.MSTATUS;

  /* If bus error. */
  if (currentStatus & TWI_BUSERR_bm) {
    master_result = TWIM_RESULT_BUS_ERROR;
  }
  /* If arbitration lost. */
  else {
    master_result = TWIM_RESULT_ARBITRATION_LOST;
  }

  /* Clear all flags, abort operation */
  TWI0.MSTATUS = currentStatus;

  /* Wait for a new operation */
  twi_mode = TWI_MODE_MASTER;
  master_trans_status = TWIM_STATUS_READY;
}


/*! \brief TWI master write interrupt handler.

    Handles TWI transactions (master write) and responses to (N)ACK.

*/
void TWI_MasterWriteHandler() {
  /* Local variables used in if tests to avoid compiler warning. */
  uint8_t bytesToWrite  = master_bytesToWrite;
  uint8_t bytesToRead   = master_bytesToRead;

  /* If NOT acknowledged (NACK) by slave cancel the transaction. */
  if (TWI0.MSTATUS & TWI_RXACK_bm) {
    if (master_sendStop) {
      TWI0.MCTRLB = TWI_MCMD_STOP_gc;
    } else {
      TWI0.MCTRLB = TWI_MCMD_REPSTART_gc;

    }
    TWI_MasterTransactionFinished(TWIM_RESULT_NACK_RECEIVED);
  }

  /* If more bytes to write, send data. */
  else if (master_bytesWritten < bytesToWrite) {
    uint8_t data = master_writeData[master_bytesWritten];
    TWI0.MDATA = data;
    master_bytesWritten++;
  }

  /* If bytes to read, send START condition + Address +
     'R/_W = 1'
  */
  else if (master_bytesRead < bytesToRead) {
    twi_mode = TWI_MODE_MASTER_RECEIVE;
    uint8_t readAddress = ADD_READ_BIT(master_slaveAddress);
    TWI0.MADDR = readAddress;
  }

  /* If transaction finished, send ACK/STOP condition if instructed and set RESULT OK. */
  else {
    if (master_sendStop) {
      TWI0.MCTRLB = TWI_MCMD_STOP_gc;
    } else {
      TWI0.MCTRLB = TWI_MCMD_REPSTART_gc;
    }
    TWI_MasterTransactionFinished(TWIM_RESULT_OK);
  }
}


/*! \brief TWI master read interrupt handler.

    This is the master read interrupt handler that takes care of
    reading bytes from the TWI slave.

    \param twi The TWI_Master_t struct instance.
*/
void TWI_MasterReadHandler() {
  /* Fetch data if bytes to be read. */
  if (master_bytesRead < master_bytesToRead) {
    uint8_t data = TWI0.MDATA;
    master_readData[master_bytesRead] = data;
    master_bytesRead++;
  }

  /* If buffer overflow, issue NACK/STOP and BUFFER_OVERFLOW condition. */
  else {
    if (master_sendStop) {
      TWI0.MCTRLB = TWI_ACKACT_bm | TWI_MCMD_STOP_gc;
    } else {
      TWI0.MCTRLB = TWI_ACKACT_bm | TWI_MCMD_REPSTART_gc;
    }

    TWI_MasterTransactionFinished(TWIM_RESULT_BUFFER_OVERFLOW);
    master_bytesToRead = 0;
    return;
  }

  /* Local variable used in if test to avoid compiler warning. */
  uint8_t bytesToRead = master_bytesToRead;

  /* If more bytes to read, issue ACK and start a byte read. */
  if (master_bytesRead < bytesToRead) {
    TWI0.MCTRLB = TWI_MCMD_RECVTRANS_gc;
  }

  /* If transaction finished, issue NACK and STOP condition if instructed. */
  else {
    if (master_sendStop) {
      TWI0.MCTRLB = TWI_ACKACT_bm | TWI_MCMD_STOP_gc;
    } else {
      TWI0.MCTRLB = TWI_ACKACT_bm | TWI_MCMD_REPSTART_gc;
    }

    TWI_MasterTransactionFinished(TWIM_RESULT_OK);
  }
}


/*! \brief TWI transaction finished handler.

    Prepares module for new transaction.

    \param result  The result of the operation.
*/
void TWI_MasterTransactionFinished(uint8_t result) {
  master_result = result;
  master_trans_status = TWIM_STATUS_READY;
  twi_mode = TWI_MODE_MASTER;
}


/*! \brief Common TWI slave interrupt service routine.

    Check current status and calls the appropriate handler.

*/
void TWI_SlaveInterruptHandler() {
  uint8_t currentStatus = TWI0.SSTATUS;

  /* If bus error */
  if (currentStatus & TWI_BUSERR_bm) {
    slave_bytesRead = 0;
    slave_bytesWritten = 0;
    slave_bytesToWrite = 0;
    TWI_SlaveTransactionFinished(TWIS_RESULT_BUS_ERROR);
  }

  /* If Address or Stop */
  else if (currentStatus & TWI_APIF_bm) {

    /* Call user onReceive function if end of Master Write/Slave Read.
       This should be hit when there is a STOP or REPSTART
    */
    if (slave_callUserReceive == 1) {
      TWI_onSlaveReceive(slave_bytesRead);
      slave_callUserReceive = 0;
    }

    /* If address match */
    if (currentStatus & TWI_AP_bm) {
      TWI_SlaveAddressMatchHandler();
    }

    /* If stop */
    else {
      TWI_SlaveStopHandler();

      /* If CLKHOLD is high, we have missed an address match
        from a fast start after stop.
        Because the flag is shared we need to handle this here.
      */
      if (TWI0.SSTATUS & TWI_CLKHOLD_bm) {

        /* CLKHOLD will be cleared by servicing the address match */
        TWI_SlaveAddressMatchHandler();
      }
    }
  }

  /* If Data Interrupt */
  else if (currentStatus & TWI_DIF_bm) {

    /* If collision flag is raised, slave transmit unsuccessful */
    if (currentStatus & TWI_COLL_bm) {
      slave_bytesRead = 0;
      slave_bytesWritten = 0;
      slave_bytesToWrite = 0;
      TWI_SlaveTransactionFinished(TWIS_RESULT_TRANSMIT_COLLISION);
    }

    /* Otherwise, normal data interrupt */
    else {
      TWI_SlaveDataHandler();
    }
  }

  /* If unexpected state */
  else {
    TWI_SlaveTransactionFinished(TWIS_RESULT_FAIL);
  }
}

/*! \brief TWI slave address interrupt handler.

    This is the slave address match handler that takes care of responding to
    being addressed by a master

*/
void TWI_SlaveAddressMatchHandler() {
  slave_trans_status = TWIS_STATUS_BUSY;
  slave_result = TWIS_RESULT_UNKNOWN;

  /* If Master Read/Slave Write */
  if (TWI0.SSTATUS & TWI_DIR_bm) {
    slave_bytesWritten = 0;
    /* Call user function  */
    slave_bytesToWrite = TWI_onSlaveTransmit();
    twi_mode = TWI_MODE_SLAVE_TRANSMIT;
  }
  /* If Master Write/Slave Read */
  else {
    slave_bytesRead = 0;
    slave_callUserReceive = 1;
    twi_mode = TWI_MODE_SLAVE_RECEIVE;
  }

  /* Send ACK, wait for data interrupt */
  TWI0.SCTRLB = TWI_SCMD_RESPONSE_gc;

  /* Data interrupt to follow... */
}

/*! \brief TWI slave stop interrupt handler.

*/
void TWI_SlaveStopHandler() {

  /* Clear APIF, don't ACK or NACK */
  TWI0.SSTATUS = TWI_APIF_bm;

  TWI_SlaveTransactionFinished(TWIS_RESULT_OK);

}

/*! \brief TWI slave data interrupt handler.

    This is the slave data handler that takes care of sending data to or
    receiving data from a master

*/
void TWI_SlaveDataHandler() {

  /* Enable stop interrupt */
  TWI0.SCTRLA |= (TWI_APIEN_bm | TWI_PIEN_bm);

  /* If Master Read/Slave Write */
  if (TWI0.SSTATUS & TWI_DIR_bm) {

    TWI_SlaveWriteHandler();
  }

  /* If Master Write/Slave Read */
  else {
    TWI_SlaveReadHandler();
  }


}

/*! \brief TWI slave data write interrupt handler.

    This is the slave data handler that takes care of sending data to a master

*/
void TWI_SlaveWriteHandler() {

  /* If NACK, slave write transaction finished */
  if ((slave_bytesWritten > 0) && (TWI0.SSTATUS & TWI_RXACK_bm)) {

    TWI0.SCTRLB = TWI_SCMD_COMPTRANS_gc;
    TWI_SlaveTransactionFinished(TWIS_RESULT_OK);
  }

  /* If ACK, master expects more data */
  else {

    if (slave_bytesWritten < slave_bytesToWrite) {
      uint8_t data = slave_writeData[slave_bytesWritten];
      TWI0.SDATA = data;
      slave_bytesWritten++;

      /* Send data, wait for data interrupt */
      TWI0.SCTRLB = TWI_SCMD_RESPONSE_gc;

    }

    /* If buffer overflow */
    else {
      TWI0.SCTRLB = TWI_SCMD_COMPTRANS_gc;
      TWI_SlaveTransactionFinished(TWIS_RESULT_BUFFER_OVERFLOW);

    }


  }
}

/*! \brief TWI slave data read interrupt handler.

    This is the slave data handler that takes care of receiving data from a master

*/
void TWI_SlaveReadHandler() {

  /* If free space in buffer */
  if (slave_bytesRead < slave_bytesToRead) {

    /* Fetch data */
    uint8_t data = TWI0.SDATA;
    slave_readData[slave_bytesRead] = data;
    slave_bytesRead++;

    /* Send ACK and wait for data interrupt */
    TWI0.SCTRLB = TWI_SCMD_RESPONSE_gc;
  }
  /* If buffer overflow, send NACK and wait for next START.
    Set result buffer overflow */
  else {
    TWI0.SCTRLB = TWI_ACKACT_bm | TWI_SCMD_COMPTRANS_gc;
    TWI_SlaveTransactionFinished(TWIS_RESULT_BUFFER_OVERFLOW);
  }
}

/*
   Function twi_attachSlaveRxEvent
   Desc     sets function called before a slave read operation
   Input    function: callback function to use
   Output   none
*/
void TWI_attachSlaveRxEvent(void (*function)(int), uint8_t *read_data, uint8_t bytes_to_read) {
  TWI_onSlaveReceive = function;
  slave_readData = read_data;
  slave_bytesToRead = bytes_to_read;
}

/*
   Function twi_attachSlaveTxEvent
   Desc     sets function called before a slave write operation
   Input    function: callback function to use
   Output   none
*/
void TWI_attachSlaveTxEvent(uint8_t (*function)(void), uint8_t *write_data) {
  TWI_onSlaveTransmit = function;
  slave_writeData = write_data;
}


/*! \brief TWI slave transaction finished handler.

    Prepares module for new transaction.

    \param result  The result of the operation.
*/
void TWI_SlaveTransactionFinished(uint8_t result) {
  TWI0.SCTRLA |= (TWI_APIEN_bm | TWI_PIEN_bm);
  twi_mode = TWI_MODE_SLAVE;
  slave_result = result;
  slave_trans_status = TWIM_STATUS_READY;
}

ISR(TWI0_TWIM_vect) {
  TWI_MasterInterruptHandler();
}

ISR(TWI0_TWIS_vect) {
  TWI_SlaveInterruptHandler();
}
