/*
  TwoWire.h - TWI/I2C library for Wiring & Arduino
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

  Modified 2021-2023 by MX682X for megaTinyCore and DxCore.
  Added Support for Simultaneous master/slave, dual mode and Wire1.
*/

#ifndef TWOWIRE_NEW_H_
#define TWOWIRE_NEW_H_

// #include <avr/io.h> -  io.h is included by Arduino.h
#include <Arduino.h>


/* The Wire library unfortunately needs TWO buffers, one for TX, and one for RX. That means, multiply these
 * values by 2 to get the actual amount of RAM they take. You can see that on the smallest ram sizes, all but
 * the minuscule buffer we provide becomes prohibitive. 32b is a magic number because it's used on the stock
 * uno/etc cores, and many libraries rely on it implicitly.
 * | System RAM  | Buffer Size | Applies to
 * |-------------|-------------|----------------------------------------------------------------|
 * |  Under 256b |       2x16b | tinyAVR 0/1-series with 2k of flash (128b RAM)                 |
 * |   256-4095b |       2x32b | All other tinyAVR 0/1/2-series, and EA/DD-series with 8 or 16k |
 * |    >= 4096b |      2x130b | Dx, EA, and megaAVR 0-series with 32k flash or more            |
 *
 * On parts that get the reduced buffer size to fit in their limited RAM, the flash is also very small,
 * and while the enhanced wire library *will* fit on 2k parts, you have very little flash left for anything else.
 * and the practicality of using it there is limited.
 */


#ifndef ADD_READ_BIT
  #define ADD_READ_BIT(address)    (address | 0x01)
#endif
#ifndef ADD_WRITE_BIT
  #define ADD_WRITE_BIT(address)   (address & ~0x01)
#endif

#ifndef DEFAULT_FREQUENCY
  #define DEFAULT_FREQUENCY 100000
#endif


#if (!defined(TWI1) && defined(TWI_USING_WIRE1))
  // If pins for Wire1 are not defined, but TWI_USING_WIRE1 was defined in the boards.txt menu, throw an error. Used for 28-pin DA/DB parts
  #error "This part only provides a single Wire interface."
#endif

/* Instead of requiring changes to the library to switch between DxCore and megaTinyCore, we can check
 * if the part supports dual mode. Goal is that the identical library can be used on both, so updates
 * in one can be propagated to the other by just copying files.
 */
#if ((defined(TWI0_DUALCTRL) && !defined(TWI_USING_WIRE1)) || (defined(TWI1_DUALCTRL) && defined(TWI_USING_WIRE1)))
  #define TWI_DUALCTRL   // This identifies if the device supports dual mode, where slave pins are different from the master pins
#endif


#if defined(__AVR_ATtiny202__) || defined(__AVR_ATtiny202__)
  #if defined(TWI_MANDS)  // 202 and 402 do not support independent master and slave.
    // #undef TWI_MANDS
    #error "Master + Slave mode is not supported on the 202 or 402."
    // If a user enables master + slave mode on a part where we know it won't we should error
    // so that they know what's wrong instead of silently disobeying
  #endif
#endif


// WIRE_HAS_END means Wire has end(), which almost all implementations do.
#ifndef WIRE_HAS_END
  #define WIRE_HAS_END 1
#endif

// These can be used to write clearer code when using the three-argument begin()
// An alternate address is specified by leftshifting and setting the low bit to 1
#define WIRE_ALT_ADDRESS(addr) ((addr << 1) | 0x01)
// While a mask is specified by leftshifting the mask, and leaving low bit 0.
#define WIRE_ADDRESS_MASK(mask) (mask << 1)

#define WIRE_SDA_HOLD_OFF 0
#define WIRE_SDA_HOLD_50  1
#define WIRE_SDA_HOLD_300 2
#define WIRE_SDA_HOLD_500 3

#define WIRE_SDA_SETUP_4  0
#define WIRE_SDA_SETUP_8  1

#define WIRE_I2C_LEVELS  0
#define WIRE_SMBUS_LEVELS  1



#if !defined(TWI_BUFFER_LENGTH)       // we need a Buffer size
  #if defined(BUFFER_LENGTH) && (BUFFER_LENGTH != 32)   // Backwards Compatibility: someone needs a non-default size
    #define TWI_BUFFER_LENGTH BUFFER_LENGTH             // Apply it.
    #warning "It seems like BUFFER_LENGTH was used to change the default Wire Buffer Size."
    #warning "The define was renamed to TWI_BUFFER_LENGTH to reduce ambiguity."
    #warning "TWI_BUFFER_LENGTH was set to BUFFER_LENGTH." // defining TWI_BUFFER_LENGTH= will remove the warnings

  #else                               // BUFFER_LENGTH was not messed with, go with our defaults
    #if (RAMSIZE < 256)
      #define TWI_BUFFER_LENGTH 16
    #elif (RAMSIZE < 4096)
      #define TWI_BUFFER_LENGTH 32
    #else
      #define TWI_BUFFER_LENGTH 130
    #endif
  #endif
#endif

// In the case someone wants to use custom 255+ buffer sizes, define TWI_16BIT_BUFFER
#if (TWI_BUFFER_LENGTH > 255) || defined(TWI_16BIT_BUFFER)
  typedef uint16_t twi_buf_index_t;
#else
  typedef uint8_t  twi_buf_index_t;
#endif


#define  TWI_TIMEOUT_ENABLE       // Enabled by default, might be disabled for debugging or other reasons
#define  TWI_ERROR_ENABLED        // Enabled by default, TWI Master Write error functionality
#define  TWI_READ_ERROR_ENABLED   // Enabled on Master Read too
//#define DISABLE_NEW_ERRORS      // Disables the new error codes and returns TWI_ERR_UNDEFINED instead.

// Errors from Arduino documentation:
#define  TWI_ERR_SUCCESS         0x00  // Default
#define  TWI_ERR_DATA_TOO_LONG   0x01  // Not used here; data too long to fit in TX buffer
#define  TWI_ERR_ACK_ADR         0x02  // Address was NACKed on Master write
#define  TWI_ERR_ACK_DAT         0x03  // Data was NACKed on Master write
#define  TWI_ERR_UNDEFINED       0x04  // Software can't tell error source
#define  TWI_ERR_TIMEOUT         0x05  // TWI Timed out on data rx/tx

// Errors that are made to help finding errors on TWI lines. Only here to give a suggestion of where to look - these may not always be reported accurately.
#if !defined(DISABLE_NEW_ERRORS)
  #define  TWI_ERR_UNINIT        0x10  // TWI was in bad state when function was called.
  #define  TWI_ERR_PULLUP        0x11  // Likely problem with pull-ups
  #define  TWI_ERR_BUS_ARB       0x12  // Bus error and/or Arbitration lost
#else
  // DISABLE_NEW_ERRORS can be used to more completely emulate the old error reporting behavior; this should rarely be needed.
  #define  TWI_ERR_UNINIT        TWI_ERR_UNDEFINED  // TWI was in bad state when method was called.
  #define  TWI_ERR_PULLUP        TWI_ERR_UNDEFINED  // Likely problem with pull-ups
  #define  TWI_ERR_BUS_ARB       TWI_ERR_UNDEFINED  // Bus error and/or Arbitration lost
#endif

#if defined(TWI_ERROR_ENABLED)
  #define TWI_ERROR_VAR    twi_error
  #define TWI_INIT_ERROR   uint8_t TWI_ERROR_VAR = TWI_ERR_SUCCESS
  #define TWI_GET_ERROR    TWI_ERROR_VAR
  #define TWI_CHK_ERROR(x) TWI_ERROR_VAR == x
  #define TWI_SET_ERROR(x) TWI_ERROR_VAR = x
#else
  #define TWI_ERROR_VAR     {}
  #define TWI_INIT_ERROR    {}
  #define TWI_GET_ERROR     {0}
  #define TWI_CHK_ERROR(x)  (true)
  #define TWI_SET_ERROR(x)  {}
#endif

#if defined(TWI_READ_ERROR_ENABLED) && defined(TWI_ERROR_ENABLED)
  #define TWIR_ERROR_VAR        twiR_error
  #define TWIR_INIT_ERROR       uint8_t TWIR_ERROR_VAR = TWI_ERR_SUCCESS
  #define TWIR_GET_ERROR        TWIR_ERROR_VAR
  #define TWIR_CHK_ERROR(x)     TWIR_ERROR_VAR == x
  #define TWIR_SET_ERROR(x)     TWIR_ERROR_VAR = x

  // #define TWI_SET_EXT_ERROR(x)  TWI_ERROR_VAR = x
#else
  #define TWIR_ERROR_VAR        {}
  #define TWIR_INIT_ERROR       {}
  #define TWIR_GET_ERROR        {0}
  #define TWIR_CHK_ERROR(x)     (true)
  #define TWIR_SET_ERROR(x)     {}

  // #define TWI_SET_EXT_ERROR(x)  {}
#endif


struct twiDataBools {         // using a struct so the compiler can use skip if bit is set/cleared
  bool _toggleStreamFn:   1;  // used to toggle between Slave and Master elements when TWI_MANDS defined
  bool _hostEnabled:      1;
  bool _clientEnabled:    1;
  bool _hostDataSent:     1;
  uint8_t _reserved:      4;
};



class TwoWire: public Stream {
  private:
    TWI_t *_module;
    uint8_t MasterCalcBaud(uint32_t frequency);

    uint8_t client_irq_mask;
    struct twiDataBools _bools;      // the structure to hold the bools for the class
    #if defined(TWI_READ_ERROR_ENABLED)
    uint8_t _errors;
    #endif

    void (*user_onRequest)(void);
    void (*user_onReceive)(int);

    uint8_t _clientAddress;
    twi_buf_index_t _bytesToReadWrite;
    twi_buf_index_t _bytesReadWritten;
    twi_buf_index_t _bytesTransmittedS;

    #if defined(TWI_MANDS)
    uint8_t _incomingAddress;
    twi_buf_index_t _bytesToReadWriteS;
    twi_buf_index_t _bytesReadWrittenS;
    #endif

    uint8_t _hostBuffer[TWI_BUFFER_LENGTH];
    #if defined(TWI_MANDS)
    uint8_t _clientBuffer[TWI_BUFFER_LENGTH];
    #endif

  public:
    explicit TwoWire(TWI_t *twi_module);
    bool pins(uint8_t sda_pin, uint8_t scl_pin);
    bool swap(uint8_t state = 1);
    bool swapModule(TWI_t *twi_module);
    void usePullups();
    uint8_t setClock(uint32_t);

    void begin(); // all attempts to make these look prettier were rejected by astyle, and it's not worth disabling linting over.
    void begin(uint8_t  address, bool receive_broadcast = 0, uint8_t second_address = 0);
    void end();
    void endMaster(void);
    void endSlave(void);

    void beginTransmission(uint8_t address);
    void beginTransmission(int     address) {
      beginTransmission((uint8_t)address);
    }

    uint8_t endTransmission(bool);
    uint8_t endTransmission(void) {
      return endTransmission(true);
    }

    twi_buf_index_t requestFrom(uint8_t address, twi_buf_index_t quantity, uint8_t sendStop = 1);

    uint8_t masterTransmit(auto *length, uint8_t *buffer, uint8_t addr, uint8_t sendStop);
    uint8_t masterReceive(auto *length, uint8_t *buffer, uint8_t addr, uint8_t sendStop);

    virtual size_t write(uint8_t);
    virtual size_t write(const uint8_t *, size_t);
    virtual int available(void);
    virtual int read(void);
    virtual int peek(void);
    void flush(void);
    uint8_t specialConfig(bool smbuslvl = 0, bool longsetup = 0, uint8_t sda_hold = 0, bool smbuslvl_dual = 0, uint8_t sda_hold_dual = 0);
    uint8_t getIncomingAddress(void);
    twi_buf_index_t getBytesRead(void);
    uint8_t slaveTransactionOpen(void);
    uint8_t checkPinLevels(void);             // Can be used to make sure after boot that SDA/SCL are high
    void    enableDualMode(bool fmp_enable);  // Moves the Slave to dedicated pins

    void selectSlaveBuffer();
    void deselectSlaveBuffer();

    void onReceive(void (*)(int));
    void onRequest(void (*)(void));

    inline size_t write(unsigned long n) {
      return      write((uint8_t)     n);
    }
    inline size_t write(long          n) {
      return      write((uint8_t)     n);
    }
    inline size_t write(unsigned int  n) {
      return      write((uint8_t)     n);
    }
    inline size_t write(int           n) {
      return      write((uint8_t)     n);
    }
    using Print::write;


    size_t readBytes(uint8_t *data, size_t quantity) {
      return readBytes((char *)data, quantity);
    }
    size_t readBytes(char *data, size_t quantity);

    static void HandleSlaveIRQ(TwoWire *wire_s);
};


#if defined(TWI0)
  extern TwoWire Wire;
#endif

#if defined (TWI1)
  #if defined (TWI_USING_WIRE1)
    extern TwoWire Wire1;
  #endif
#endif

#endif /* TWOWIRE_NEW_H_ */
