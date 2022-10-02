/*
  Copyright (c) 2022 MX682X

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

// *INDENT-OFF* astyle wants this file to be completely unreadable with no indentation for the many preprocessor conditionals!

#ifndef TWI_H_
#define TWI_H_

#include "Arduino.h"



/*! For adding R/_W bit to address */
#ifndef ADD_READ_BIT
  #define ADD_READ_BIT(address)    (address | 0x01)
#endif
#ifndef ADD_WRITE_BIT
  #define ADD_WRITE_BIT(address)   (address & ~0x01)
#endif

#ifndef DEFAULT_FREQUENCY
  #define DEFAULT_FREQUENCY 100000
#endif

/* These options are or will be controlled by boards.txt menu options
#define TWI_USING_WIRE1   // On devices with two TWIs, this identifies if the user wants to use Wire1
#define TWI_MANDS         // This enables the simultaneous use of the Master and Slave functionality - where supported
#define TWI_MERGE_BUFFERS // Merges the master tx and rx buffers - this option will break the TWI when any rx occurs between beginTransmission and endTransmission!
                          // It is not advised to use this define. Only use this when you need the RAM **really** badly
*/

#if (!defined(TWI1) && defined(TWI_USING_WIRE1))
  // If pins for Wire1 are not defined, but TWI_USING_WIRE1 was defined in the boards.txt menu, throw an error. Used for 28-pin DA/DB parts
  #error "This part only provides a single Wire interface."
#endif

#if ((defined(TWI0_DUALCTRL) && !defined(TWI_USING_WIRE1)) || (defined(TWI1_DUALCTRL) && defined(TWI_USING_WIRE1)))
  /* Instead of requiring changes to the library to switch between DxCore and megaTinyCore, we can check
   * if the part supports dual mode. Goal is that the identical library can be used on both, so updates
   * in one can be propagated to the other by just copying files. */
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
/* Reasoning behind buffer lengths of 16 bytes for stuff with hardly
 * any RAM, 130 for stuff with tons and 32 for everything else.
 *
 * Parts with 128b of RAM wince at pair of 16b buffers (25%)
 *
 * In the past we configured this with more granularity but it
 * was pointed out that because of the official cores all using
 * 32 byte buffers, many libraries implicitly depend >= 32 byte
 * buffers and will misbehave if the buffer used is smaller.
 * So even where it's painfully large portion of ram, we use
 * that. Similarly, most libraries will not make use of a larger one
 * Thus, we don't enlarge the buffer until we finally get to
 * the modern AVRs with >= 4k of RAM, where it doesn't really
 * matter anymore. At that point, enlarging the buffer to 130b
 * allows for a 128b page write to the popular 24-series EEPROMs
 * Since the modern AVRs are lighter on EEPROM than classic ones
 * it is more likely that external storage would be needed.
 * SD cards are a poor choice for byte oriented data storage, in
 * addition to being astonishingly unreliable, and byte oriented
 * data storage is more useful for typical applications of AVRs
 * than file-oriented storage.
 * If the standard buffer size is not enough, the supported maximum
 * is 32.767 (due to size_t), without further modifications
 * to the library. Buffer lengths below 255 are highly recommended
 * though.
 */

#ifndef BUFFER_LENGTH
  #if (RAMSIZE < 256)
    #define BUFFER_LENGTH 16
  #elif (RAMSIZE < 4096)
    #define BUFFER_LENGTH 32
  #else
    #define BUFFER_LENGTH 130
  #endif
#endif


#if (BUFFER_LENGTH > 255)
  typedef uint16_t twi_buffer_index_t;
#else
  typedef uint8_t  twi_buffer_index_t;
#endif


#define  TWI_TIMEOUT_ENABLE       // Enabled by default, might be disabled for debugging or other reasons
#define  TWI_ERROR_ENABLED        // Enabled by default, TWI Master Write error functionality
//#define TWI_READ_ERROR_ENABLED  // Enabled on Master Read too
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
  #define  TWI_ERR_BUF_OVERFLOW  0x13  // Buffer overflow on master read
  #define  TWI_ERR_CLKHLD        0x14  // Something's holding the clock
#else
  // DISABLE_NEW_ERRORS can be used to more completely emulate the old error reporting behavior; this should rarely be needed.
  #define  TWI_ERR_UNINIT        TWI_ERR_UNDEFINED  // TWI was in bad state when method was called.
  #define  TWI_ERR_PULLUP        TWI_ERR_UNDEFINED  // Likely problem with pull-ups
  #define  TWI_ERR_BUS_ARB       TWI_ERR_UNDEFINED  // Bus error and/or Arbitration lost
  #define  TWI_ERR_BUF_OVERFLOW  TWI_ERR_UNDEFINED  // Buffer overflow on master read
  #define  TWI_ERR_CLKHLD        TWI_ERR_UNDEFINED  // Something's holding the clock
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
  #define TWIR_INIT_ERROR       uint8_t TWIR_ERROR_VAR = TWIR_ERR_SUCCESS
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


struct twiDataBools {       // using a struct so the compiler can use skip if bit is set/cleared
  bool _toggleStreamFn:   1;  // used to toggle between Slave and Master elements when TWI_MANDS defined
  bool _hostEnabled:      1;
  bool _clientEnabled:    1;
  bool _ackMatters:       1;
  uint8_t _reserved:      4;
};


struct twiData {
  TWI_t *_module;
  struct twiDataBools _bools;      // the structure to hold the bools for the class
  #if defined(TWI_READ_ERROR_ENABLED)
    uint8_t _errors;
  #endif
  twi_buffer_index_t _bytesTransmittedS;
  uint8_t _clientAddress;
  #if defined(TWI_MERGE_BUFFERS)
    twi_buffer_index_t _bytesToReadWrite;
    twi_buffer_index_t _bytesReadWritten;
  #else
    twi_buffer_index_t _bytesToWrite;
    twi_buffer_index_t _bytesToRead;
    twi_buffer_index_t _bytesRead;     // Used in slave mode exclusively
    twi_buffer_index_t _bytesWritten;  // Used in slave mode exclusively
  #endif
  #if defined(TWI_MANDS)
    uint8_t _incomingAddress;
    twi_buffer_index_t _bytesToReadWriteS;
    twi_buffer_index_t _bytesReadWrittenS;
  #endif
  void (*user_onRequest)(void);
  void (*user_onReceive)(int);
  #if defined(TWI_MERGE_BUFFERS)
    uint8_t _trBuffer[BUFFER_LENGTH];
  #else
    uint8_t _txBuffer[BUFFER_LENGTH];
    uint8_t _rxBuffer[BUFFER_LENGTH];
  #endif
  #if defined(TWI_MANDS)
    uint8_t _trBufferS[BUFFER_LENGTH];
  #endif
};


void     TWI_MasterInit(struct      twiData *_data);
void     TWI_Flush(struct           twiData *_data);
void     TWI_Disable(struct         twiData *_data);
void     TWI_DisableMaster(struct   twiData *_data);
void     TWI_DisableSlave(struct    twiData *_data);
void     TWI_HandleSlaveIRQ(struct  twiData *_data);
uint8_t  TWI_MasterWrite(struct     twiData *_data, bool send_stop);
uint8_t  TWI_MasterSetBaud(struct   twiData *_data, uint32_t frequency);
void     TWI_SlaveInit(struct       twiData *_data, uint8_t address, uint8_t receive_broadcast, uint8_t second_address);
uint8_t  TWI_MasterCalcBaud(uint32_t frequency);

twi_buffer_index_t  TWI_MasterRead(struct twiData *_data, twi_buffer_index_t bytesToRead, bool send_stop);

#endif
