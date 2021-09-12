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

  Modified 2021 by MX682X for megaTinyCore and DxCore.
  Added Support for Simultaneous master/slave, dual mode and Wire1.
*/




#ifndef TWOWIRENEW_H
#define TWOWIRENEW_H

//#include <avr/io.h>
#include <Arduino.h>


extern "C" {
#include "twi.h"
}
/* The Wire library unfortunately needs TWO buffers, one for TX, and one for RX. That means, multiply these
 * values by 2 to get the actual amount of RAM they take. You can see that on the smallest ram sizes, all but
 * the minuscule buffer we provide becomes prohibitive. A mere 16 bit buffer, times 2, is 25% of total RAM on
 * a 2k part with only 128b of RAM. Same goes for 32b on 4k 0/1-series... 32b is kind of a magic number though
 * because it's used on the stock uno/etc cores, which sort of sets an expectation, libraries rely on it and
 * so on... It ofc isn't viable with only 128b RAM, but the Wire library ah, isn't really viable with  2k of
 * flash anyway - you are basically guaranteed not to be happy with the results it would appear! At least not
 * without considerable amounts of serious optimization work on this library, which would be hard to do
 * without breaking compatibility with the Arduino API - and you all know my dedication to that! */


// WIRE_HAS_END means Wire has end()
#ifndef WIRE_HAS_END
  #define WIRE_HAS_END 1
#endif



class TwoWire: public Stream {
  private:
    twiData vars;                 //using a struct to reduce the amount of parameters that have to be passed


  public:
    TwoWire(TWI_t *twi_module);
    bool pins(uint8_t sda_pin, uint8_t scl_pin);
    bool swap(uint8_t state = 1);
    bool swapModule(TWI_t *twi_module);
    void usePullups();
    void setClock(uint32_t);

    void begin();
    // all attempts to make these look prettier were rejected by astyle, and it's not worth disabling linting over.
    void begin(uint8_t  address, bool receive_broadcast, uint8_t second_address);
    void begin(int      address, bool receive_broadcast, uint8_t second_address) {
      begin((uint8_t) address, receive_broadcast, second_address);
    }
    void begin(uint8_t  address, bool receive_broadcast) {
      begin(address, receive_broadcast, 0);
    }
    void begin(int      address, bool receive_broadcast) {
      begin((uint8_t) address, receive_broadcast, 0);
    }
    void begin(uint8_t  address) {
      begin(address, 0, 0);
    }
    void begin(int      address) {
      begin((uint8_t) address, 0, 0);
    }

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

    uint8_t requestFrom(uint8_t address, uint8_t quantity, uint8_t sendStop);
    uint8_t requestFrom(uint8_t address, uint8_t quantity);
    uint8_t requestFrom(uint8_t address, size_t  quantity, bool    sendStop);
    uint8_t requestFrom(uint8_t address, size_t  quantity);
    uint8_t requestFrom(int     address, int     quantity, int     sendStop);
    uint8_t requestFrom(int     address, int     quantity);

    virtual size_t write(uint8_t);
    virtual size_t write(const uint8_t *, size_t);
    virtual int available(void);
    virtual int read(void);
    virtual int peek(void);
    virtual void flush(void);

    uint8_t getIncomingAddress(void);
    void   enableDualMode(bool fmp_enable);      //Moves the Slave to dedicated pins

    void onReceive(void (*)(int));
    void onRequest(void (*)(void));

    inline size_t write(unsigned long n) {
      return      write((uint8_t)     n);
    }
    inline size_t write(long          n) {
      return      write((uint8_t)     n);
    }
    inline size_t write(uint          n) {
      return      write((uint8_t)     n);
    }
    inline size_t write(int           n) {
      return      write((uint8_t)     n);
    }
    using Print::write;

    void    TWI_onReceiveService(int numBytes);
    uint8_t TWI_onRequestService(void);

    static void onSlaveIRQ(TWI_t *module);    //is called by the TWI interrupt routines
};

#if defined(TWI0)
  extern TwoWire Wire;
#endif

#if defined (TWI1)
  #if defined (USING_WIRE1)
    extern TwoWire Wire1;
  #endif
#endif

#endif
