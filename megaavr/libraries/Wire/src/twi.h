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
#define USING_WIRE1       // On devices with two TWIs, this identifies if the user wants to use Wire1
#define TWI_MANDS         // This enables the simultaneous use of the Master and Slave functionality - where supported
#define TWI_MERGE_BUFFERS // Merges the tx and rx buffers - this option will break the TWI when any rx occurs between beginTransmission and endTransmission!
                          // It is not advised to use this define. Only use this when you need the RAM **really** badly
*/

#if (!defined(TWI1) && defined(USING_WIRE1))
  // If pins for Wire1 are not defined, but USING_WIRE1 was defined in the boards.txt menu, throw an error. Used for 28-pin DA/DB parts
  #error "This part does not support two Wire interfaces."
#endif

#if ((defined(TWI0_DUALCTRL) && !defined(USING_WIRE1)) || (defined(TWI1_DUALCTRL) && defined(USING_WIRE1)))
  /* Instead of requiring changes to the library to switch between DxCore and megaTinyCore, we can check
   * if the part supports dual mode. Goal is that the identical library can be used on both, so updates
   * in one can be propagated to the other by just copying files. */
  #define TWI_DUALCTRL   // This identifies if the device supports dual mode, where slave pins are different from the master pins
#endif

#if defined(ARDUINO_AVR_ATtiny202) || defined(ARDUINO_AVR_ATtiny402)
  #if defined(TWI_MANDS)  // 202 and 402 do not support independent master and slave.
    // #undef TWI_MANDS
    #error "Master + Slave mode is not supported on the 202 or 402."
    // If a user enables master + slave mode on a part where we know it won't we should error
    // so that they know what's wrong instead of silently disobeying
  #endif
#endif


#ifndef BUFFER_LENGTH
  #if (RAMSIZE < 256)          /* Parts with 128b of RAM wince at pair of 16b buffers         */
    #define BUFFER_LENGTH 16   /* 2k tinyAVRs: 128b -  25% of available RAM                   */
  #elif (RAMSIZE < 512)        /* In the past we configured this with more granularity but it */
    #define BUFFER_LENGTH 32   /* was pointed out that because of the official cores all using*/
  #elif (RAMSIZE < 2048)       /* 32 byte buffers, many libraries implicitly depend >= 32 byte*/
    #define BUFFER_LENGTH 32   /* buffers and will misbehave if the buffer used is smaller.   */
  #elif (RAMSIZE < 4096)       /* So even where it's painfully large portion of ram, we use   */
    #define BUFFER_LENGTH 32   /* that. We don't enlarge the buffer until we finally get to   */
  #else                        /* the modern AVRs with >= 4k of RAM. The 128b buffer on the   */
    #define BUFFER_LENGTH 130  /* official core thus sets a lower bound there, and we add an  */
    #define BUFFER_NOT_POWER_2 /* extra 2 bytes in order to efficiently write 128b pages to   */
  #endif                       /* the common AT24-series EEPROM chips at their largest sizes  */
#endif                         /* where the first two bytes specify the address write at      */


#define TWI_TIMEOUT_ENABLE    // Enabled by default, might be disabled for debugging or other reasons

// #define TWI_ERROR_ENABLED

// The error result may not be accurate, it just helps narrowing the problem down
#define  TWI_NO_ERR            0      // Default
#define  TWI_ERR_PULLUP        1  // Likely problem with pull-ups
#define  TWI_ERR_TIMEOUT       2  // TWI Timed out on data rx/tx
#define  TWI_ERR_BUS_ARB       3  // Bus error and/or Arbitration lost
#define  TWI_ERR_BUF_OVERFLOW  4  // Buffer overflow on master read
#define  TWI_ERR_RXACK         5  // Address or data was NACKed
#define  TWI_ERR_CLKHLD        6  // Something's holding the clock
#define  TWI_ERR_UNDEFINED     7  // Software can't tell error source

#if defined(TWI_ERROR_ENABLED)
  #define TWI_ERROR_VAR   twi_error
  #define TWI_INIT_ERROR  uint8_t TWI_ERROR_VAR = TWI_NO_ERR

  #define TWI_CHK_ERROR(x) TWI_ERROR_VAR == x
  #define TWI_SET_ERROR(x) TWI_ERROR_VAR = x
  #define TWI_SAVE_ERROR(x) x = TWI_ERROR_VAR
#else
  #define TWI_ERROR_VAR     {}
  #define TWI_INIT_ERROR    {}

  #define TWI_CHK_ERROR(x)  (true)
  #define TWI_SET_ERROR(x)  {}
  #define TWI_SAVE_ERROR(x) {}
#endif


struct twiDataBools {       // using a struct so the compiler can use skip if bit is set/cleared
  uint8_t _reserved:      4;
  bool _toggleStreamFn:   1;  // used to toggle between Slave and Master elements when TWI_MANDS defined
  bool _hostEnabled:      1;
  bool _clientEnabled:    1;
  bool _ackMatters:       1;
};

/* My original idea was to pass the whole TwoWire class as a  */
/* Pointer to this functions but this didn't work of course.  */
/* But I had the idea: since the class is basically just a    */
/* struct, why not just put the relevant variables in one     */
/* and pass that as a pointer? So now this exists and it      */
/* seems to work.                                             */

struct twiData {
  TWI_t *_module;

  struct twiDataBools _bools;      // the structure to hold the bools for the class

  #if defined(TWI_ERROR_ENABLED)
    uint8_t _errors;
  #endif

  uint8_t _clientAddress;
  #if defined(TWI_MERGE_BUFFERS)
    uint8_t _trHead;
    uint8_t _trTail;
  #else
    uint8_t _txHead;
    uint8_t _txTail;
    uint8_t _rxHead;
    uint8_t _rxTail;
  #endif

  #if defined(TWI_MANDS)
    uint8_t _incomingAddress;
    #if defined(TWI_MERGE_BUFFERS)
      uint8_t _trHeadS;
      uint8_t _trTailS;
    #else
      uint8_t _txHeadS;
      uint8_t _txTailS;
      uint8_t _rxHeadS;
      uint8_t _rxTailS;
    #endif
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
    #if defined(TWI_MERGE_BUFFERS)
      uint8_t _trBufferS[BUFFER_LENGTH];
    #else
      uint8_t _txBufferS[BUFFER_LENGTH];
      uint8_t _rxBufferS[BUFFER_LENGTH];
    #endif
  #endif
};


uint8_t  TWI_advancePosition(uint8_t pos);  // returns the next Position with Round-Robin functionality

void     TWI_MasterInit(struct        twiData *_data);
void     TWI_SlaveInit(struct      twiData *_data, uint8_t address, uint8_t receive_broadcast, uint8_t second_address);
void     TWI_Flush(struct           twiData *_data);
void     TWI_Disable(struct         twiData *_data);
void     TWI_DisableMaster(struct     twiData *_data);
void     TWI_DisableSlave(struct   twiData *_data);
void     TWI_MasterSetBaud(struct     twiData *_data, uint32_t frequency);
uint8_t  TWI_Available(struct       twiData *_data);
uint8_t  TWI_MasterWrite(struct       twiData *_data, bool send_stop);
uint8_t  TWI_MasterRead(struct        twiData *_data, uint8_t bytesToRead, bool send_stop);
void     TWI_HandleSlaveIRQ(struct twiData *_data);
uint8_t  TWI_MasterCalcBaud(uint32_t frequency);  // moved to twi_pins.h due to license incompatibilities

#endif
