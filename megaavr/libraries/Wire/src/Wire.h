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

  Modified 2021-2022 by MX682X for megaTinyCore and DxCore.
  Added Support for Simultaneous master/slave, dual mode and Wire1.
*/

#ifndef TWOWIRE_NEW_H_
#define TWOWIRE_NEW_H_

// #include <avr/io.h> -  io.h is included by Arduino.h
#include <Arduino.h>


extern "C" {
#include "twi.h"
}
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

class TwoWire: public Stream {
  private:
    twiData vars;   // We're using a struct to reduce the amount of parameters that have to be passed.
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

    twi_buffer_index_t requestFrom(uint8_t address, twi_buffer_index_t quantity, uint8_t sendStop = 1);

    virtual size_t write(uint8_t);
    virtual size_t write(const uint8_t *, size_t);
    virtual int available(void);
    virtual int read(void);
    virtual int peek(void);
    void flush(void);
    uint8_t specialConfig(bool smbuslvl = 0, bool longsetup = 0, uint8_t sda_hold = 0, bool smbuslvl_dual = 0, uint8_t sda_hold_dual = 0);
    uint8_t getIncomingAddress(void);
    twi_buffer_index_t getBytesRead(void);
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

    #if defined(TWI_READ_ERROR_ENABLED) && defined(TWI_ERROR_ENABLED)
    uint8_t returnError();
    #endif

    static void onSlaveIRQ(TWI_t *module);    // is called by the TWI interrupt routines
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
