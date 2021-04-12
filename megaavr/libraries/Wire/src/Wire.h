/*
  TwoWire.h - TWI/I2C library for Arduino & Wiring
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
*/

#ifndef TwoWire_h
#define TwoWire_h

#include <Arduino.h>
/* The Wire library unfortunately needs TWO buffers, one for TX, and one for RX. That means, multiply these
 * values by 2 to get the actual amount of RAM they take. You can see that on the smallest ram sizes, all but
 * the minuscule buffer we provide becomes prohibitive. A mere 16 bit buffer, times 2, is 25% of total RAM on
 * a 2k part with only 128b of RAM. Same goes for 32b on 4k 0/1-series... 32b is kind of a magic number though
 * because it's used on the stock uno/etc cores, which sort of sets an expectation, libraries rely on it and
 * so on... It ofc isn't viable with only 128b RAM, but the Wire library ah, isn't really viable with  2k of
 * flash anyway - you are basically guaranteed not to be happy with the results it would appear! At least not
 * without considerable amounts of serious optimization work on this library, which would be hard to do
 * without breaking compatibility with the Arduino API - and you all know my dedication to that! */

#ifndef BUFFER_LENGTH
  #if (RAMSIZE < 256)         /* Parts with 128b of RAM wince at pair of 16k buffers         */
    #define BUFFER_LENGTH 16  /* 2k tinyAVRs: 128b -  25% of available RAM                   */
  #elif (RAMSIZE < 512)       /* Parts with 256b of RAM shall allocate 24b buffers           */
    #define BUFFER_LENGTH 32  /* 4k tinyAVRs 0/1: 256b - 25% of available RAM - oof          */
  #elif (RAMSIZE < 2048)      /* parts with 512b or 1024b of RAM get 32b buffers             */
    #define BUFFER_LENGTH 32  /* 8k tinyAVRs, 16k 0-series - 6-13% of available RAM          */
  #elif (RAMSIZE < 4096)      /* 16k AVR DD-series, 16k tinyAVR 1/2-series 48b buffers       */
    #define BUFFER_LENGTH 32  /* and 32k tinyAVR   - 3-5% of available RAM                   */
  #else                       /* >=4k: Dx32/m320x (4k) m480x (6k),  Dx64 (8k) Dx128 (16k)    */
    #define BUFFER_LENGTH 130 /* 130 - 128b on all Dx with >= 4k RAM, to match official      */
  #endif                      /* 4809 core plux that couple bytes mentioned above.           */
#endif

// WIRE_HAS_END means Wire has end()
#ifndef WIRE_HAS_END
  #define WIRE_HAS_END 1
#endif
class TwoWire : public Stream {
  private:
    static uint8_t rxBuffer[];
    static uint8_t rxBufferIndex;
    static uint8_t rxBufferLength;

    static uint8_t txAddress;
    static uint8_t txBuffer[];
    static uint8_t txBufferIndex;
    static uint8_t txBufferLength;

    static uint8_t transmitting;
    static void (*user_onRequest)(void);
    static void (*user_onReceive)(int);
    static uint8_t onRequestService(void);
    static void onReceiveService(int);
  public:
    TwoWire();
    bool pins(uint8_t sda_pin, uint8_t scl_pin);
    bool swap(uint8_t state = 1);
    void usePullups();
    void begin();
    void begin(uint8_t);
    void begin(int);
    void begin(uint8_t, bool, uint8_t);
    void begin(int, bool, uint8_t);
    void begin(uint8_t, bool);
    void begin(int, bool);
    void end();
    void setClock(uint32_t);
    void beginTransmission(uint8_t);
    void beginTransmission(int);
    uint8_t endTransmission(void);
    uint8_t endTransmission(bool);
    uint8_t requestFrom(uint8_t, uint8_t);
    uint8_t requestFrom(uint8_t, uint8_t, uint8_t);
    uint8_t requestFrom(uint8_t, size_t);
    uint8_t requestFrom(uint8_t, size_t, bool);
    uint8_t requestFrom(int, int);
    uint8_t requestFrom(int, int, int);
    virtual size_t write(uint8_t);
    virtual size_t write(const uint8_t *, size_t);
    virtual int available(void);
    virtual int read(void);
    virtual int peek(void);
    virtual void flush(void);
    void onReceive(void (*)(int));
    void onRequest(void (*)(void));

    inline size_t write(unsigned long n) {
      return write((uint8_t)n);
    }
    inline size_t write(long n) {
      return write((uint8_t)n);
    }
    inline size_t write(unsigned int n) {
      return write((uint8_t)n);
    }
    inline size_t write(int n) {
      return write((uint8_t)n);
    }
    using Print::write;
};

extern TwoWire Wire;

#endif
