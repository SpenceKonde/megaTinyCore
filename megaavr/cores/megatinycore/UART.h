/* UART.h - Hardware serial library for Wiring
 * Copyright (c) 2006 Nicholas Zambetti.  All right reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * Modified 28 September 2010 by Mark Sproul
 * Modified 14 August 2012 by Alarus
 * Modified 3 December 2013 by Matthijs Kooijman
 * Modified by someone, sometime in 2016 to work on megaAVR 0-series
 * Modified 2017-2021 by Spence Konde for DxCore and megaTinyCore
 */

#pragma once

#include <inttypes.h>
#include "api/HardwareSerial.h"
#include "pins_arduino.h"
/* Define constants and variables for buffering incoming serial data.  We're
 * using a ring buffer (I think), in which head is the index of the location
 * to which to write the next incoming character and tail is the index of the
 * location from which to read.
 * NOTE: a "power of 2" buffer size is recommended to dramatically
 *       optimize all the modulo operations for ring buffers.
 * ... Well, it was missing some optimization, because explicitly doing the
 * optimization saved a bit more flash... I see no compelling reason to permit
 * non-power-of-2 ring buffer length. -Spence
 *
 * WARNING: When buffer sizes are increased to > 256, the buffer index
 * variables are automatically increased in size, but the extra
 * atomicity guards needed for that are not implemented. This will
 * often work, but occasionally a race condition can occur that makes
 * Serial behave erratically. See https://github.com/arduino/Arduino/issues/2405
 * Flash versus RAM table
 * |       |  modern tinyAVR series parts   | Other modern parts   |
 * | Flash | 0-series | 1-series | 2-series | mega | All Dx |  EA  |
 * |-------|----------|----------|----------|------|--------|------|
 * |  2048 |      128 |      128 |       -  |   -  |     -  |   -  |
 * |  4096 |      256 |      256 |      512 |   -  |     -  |   -  |
 * |  8192 |      512 |      512 |     1024 | 1024 |     -  | 1024 |
 * | 16384 |     1024 |     2048 |     2048 | 2048 |   2048 | 2048 |
 * | 32768 |       -  |     2048 |     3072 | 4096 |   4096 | 4096 |
 * | 49152 |       -  |       -  |       -  | 6120 |     -  |   -  |
 * | 65536 |       -  |       -  |       -  |   -  |   8192 | 6120 |
 * |  128k |       -  |       -  |       -  |   -  |  16384 |   -  |
 * This ratio is remarkably consistent. No AVR part was ever made with
 * less than 8:1 flash:ram, nor more than 16:1, since first ATmegas!
 * The sole exception? The ATmega2560/2561 has only 4k RAM.
 * (to be fair, you are allowed to use external RAM - also I think
 * a unique feature)
 */

#if !defined(SERIAL_TX_BUFFER_SIZE)   // could be overridden by boards.txt
  #if   (INTERNAL_SRAM_SIZE  < 1024)  // 256b/512b RAM
    #define SERIAL_TX_BUFFER_SIZE 16
  #elif (INTERNAL_SRAM_SIZE < 2048)   // 1k RAM
    #define SERIAL_TX_BUFFER_SIZE 32
  #else
    #define SERIAL_TX_BUFFER_SIZE 64  // 2k/3k RAM
  #endif
#endif
#if !defined(SERIAL_RX_BUFFER_SIZE)   // could be overridden by boards.txt
  #if   (INTERNAL_SRAM_SIZE <  512)  // 256b RAM
    #define SERIAL_RX_BUFFER_SIZE 16
  #elif (INTERNAL_SRAM_SIZE < 1024)  // 512b RAM
    #define SERIAL_RX_BUFFER_SIZE 32
  #else
    #define SERIAL_RX_BUFFER_SIZE 64  // 1k+ RAM
  #endif
#endif
/* Use INTERNAL_SRAM_SIZE instead of RAMEND - RAMSTART, which is vulnerable to
 * a fencepost error. */
#if (SERIAL_TX_BUFFER_SIZE > 256)
  typedef uint16_t tx_buffer_index_t;
#else
  typedef uint8_t  tx_buffer_index_t;
#endif
#if (SERIAL_RX_BUFFER_SIZE > 256)
  typedef uint16_t rx_buffer_index_t;
#else
  typedef uint8_t  rx_buffer_index_t;
#endif
// As noted above, forcing the sizes to be a power of two saves a small
// amount of flash, and there's no compelling reason to NOT have them be
// a power of two. If this is a problem, since you're already modifying
// core, change the lines in UART.cpp where it does & (SERIAL_xX_BUFFERLSIZE-1)
// and replace them with % SERIAL_xX_BUFFER_SIZE; where xX is TX or RX.
// There are two of each, and the old ending of the line is even commented
// out at the end of the line.
#if (SERIAL_TX_BUFFER_SIZE & (SERIAL_TX_BUFFER_SIZE - 1))
  #error "ERROR: TX buffer size must be a power of two."
#endif
#if (SERIAL_RX_BUFFER_SIZE & (SERIAL_RX_BUFFER_SIZE - 1))
  #error "ERROR: RX buffer size must be a power of two."
#endif

// Define config for Serial.begin(baud, config);
#undef SERIAL_5N1
#undef SERIAL_6N1
#undef SERIAL_7N1
#undef SERIAL_8N1
#undef SERIAL_5N2
#undef SERIAL_6N2
#undef SERIAL_7N2
#undef SERIAL_8N2
#undef SERIAL_5E1
#undef SERIAL_6E1
#undef SERIAL_7E1
#undef SERIAL_8E1
#undef SERIAL_5E2
#undef SERIAL_6E2
#undef SERIAL_7E2
#undef SERIAL_8E2
#undef SERIAL_5O1
#undef SERIAL_6O1
#undef SERIAL_7O1
#undef SERIAL_8O1
#undef SERIAL_5O2
#undef SERIAL_6O2
#undef SERIAL_7O2
#undef SERIAL_8O2

// Define config for Serial.begin(baud, config); Default: SERIAL_8N1
#define SERIAL_5N1 (USART_CMODE_ASYNCHRONOUS_gc | USART_CHSIZE_5BIT_gc | USART_PMODE_DISABLED_gc | USART_SBMODE_1BIT_gc)
#define SERIAL_6N1 (USART_CMODE_ASYNCHRONOUS_gc | USART_CHSIZE_6BIT_gc | USART_PMODE_DISABLED_gc | USART_SBMODE_1BIT_gc)
#define SERIAL_7N1 (USART_CMODE_ASYNCHRONOUS_gc | USART_CHSIZE_7BIT_gc | USART_PMODE_DISABLED_gc | USART_SBMODE_1BIT_gc)
#define SERIAL_8N1 (USART_CMODE_ASYNCHRONOUS_gc | USART_CHSIZE_8BIT_gc | USART_PMODE_DISABLED_gc | USART_SBMODE_1BIT_gc)

#define SERIAL_5N2 (USART_CMODE_ASYNCHRONOUS_gc | USART_CHSIZE_5BIT_gc | USART_PMODE_DISABLED_gc | USART_SBMODE_2BIT_gc)
#define SERIAL_6N2 (USART_CMODE_ASYNCHRONOUS_gc | USART_CHSIZE_6BIT_gc | USART_PMODE_DISABLED_gc | USART_SBMODE_2BIT_gc)
#define SERIAL_7N2 (USART_CMODE_ASYNCHRONOUS_gc | USART_CHSIZE_7BIT_gc | USART_PMODE_DISABLED_gc | USART_SBMODE_2BIT_gc)
#define SERIAL_8N2 (USART_CMODE_ASYNCHRONOUS_gc | USART_CHSIZE_8BIT_gc | USART_PMODE_DISABLED_gc | USART_SBMODE_2BIT_gc)

#define SERIAL_5E1 (USART_CMODE_ASYNCHRONOUS_gc | USART_CHSIZE_5BIT_gc | USART_PMODE_EVEN_gc | USART_SBMODE_1BIT_gc)
#define SERIAL_6E1 (USART_CMODE_ASYNCHRONOUS_gc | USART_CHSIZE_6BIT_gc | USART_PMODE_EVEN_gc | USART_SBMODE_1BIT_gc)
#define SERIAL_7E1 (USART_CMODE_ASYNCHRONOUS_gc | USART_CHSIZE_7BIT_gc | USART_PMODE_EVEN_gc | USART_SBMODE_1BIT_gc)
#define SERIAL_8E1 (USART_CMODE_ASYNCHRONOUS_gc | USART_CHSIZE_8BIT_gc | USART_PMODE_EVEN_gc | USART_SBMODE_1BIT_gc)

#define SERIAL_5E2 (USART_CMODE_ASYNCHRONOUS_gc | USART_CHSIZE_5BIT_gc | USART_PMODE_EVEN_gc | USART_SBMODE_2BIT_gc)
#define SERIAL_6E2 (USART_CMODE_ASYNCHRONOUS_gc | USART_CHSIZE_6BIT_gc | USART_PMODE_EVEN_gc | USART_SBMODE_2BIT_gc)
#define SERIAL_7E2 (USART_CMODE_ASYNCHRONOUS_gc | USART_CHSIZE_7BIT_gc | USART_PMODE_EVEN_gc | USART_SBMODE_2BIT_gc)
#define SERIAL_8E2 (USART_CMODE_ASYNCHRONOUS_gc | USART_CHSIZE_8BIT_gc | USART_PMODE_EVEN_gc | USART_SBMODE_2BIT_gc)

#define SERIAL_5O1 (USART_CMODE_ASYNCHRONOUS_gc | USART_CHSIZE_5BIT_gc | USART_PMODE_ODD_gc | USART_SBMODE_1BIT_gc)
#define SERIAL_6O1 (USART_CMODE_ASYNCHRONOUS_gc | USART_CHSIZE_6BIT_gc | USART_PMODE_ODD_gc | USART_SBMODE_1BIT_gc)
#define SERIAL_7O1 (USART_CMODE_ASYNCHRONOUS_gc | USART_CHSIZE_7BIT_gc | USART_PMODE_ODD_gc | USART_SBMODE_1BIT_gc)
#define SERIAL_8O1 (USART_CMODE_ASYNCHRONOUS_gc | USART_CHSIZE_8BIT_gc | USART_PMODE_ODD_gc | USART_SBMODE_1BIT_gc)

#define SERIAL_5O2 (USART_CMODE_ASYNCHRONOUS_gc | USART_CHSIZE_5BIT_gc | USART_PMODE_ODD_gc | USART_SBMODE_2BIT_gc)
#define SERIAL_6O2 (USART_CMODE_ASYNCHRONOUS_gc | USART_CHSIZE_6BIT_gc | USART_PMODE_ODD_gc | USART_SBMODE_2BIT_gc)
#define SERIAL_7O2 (USART_CMODE_ASYNCHRONOUS_gc | USART_CHSIZE_7BIT_gc | USART_PMODE_ODD_gc | USART_SBMODE_2BIT_gc)
#define SERIAL_8O2 (USART_CMODE_ASYNCHRONOUS_gc | USART_CHSIZE_8BIT_gc | USART_PMODE_ODD_gc | USART_SBMODE_2BIT_gc)

#if defined(USART_RS4850_bm) && !defined(USART_RS485_bm)
  #define USART_RS485_bm USART_RS4850_bm
#endif


#define SERIAL_PIN_SETS 2
// TX, RX, XCK, XDIR
const uint8_t _usart_pins[][4] = {
  #if defined(__ATtinyxy2__)
    {PIN_PA6, PIN_PA7, PIN_PA3, PIN_PA0},
    {PIN_PA1, PIN_PA2, NOT_A_PIN, NOT_A_PIN},
  #else
    {PIN_PB2, PIN_PB3, PIN_PB1, PIN_PB0},
    {PIN_PA1, PIN_PA2, PIN_PA3, PIN_PA4},
    #if MEGATINYCORE_SERIES == 2
      #if !defined(__ATtinyxy4__)
        {PIN_PC2, PIN_PC1, PIN_PC0, PIN_PC3},
      #else
        {NOT_A_PIN, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN}
      #endif
    #endif
  #endif
}

class UartClass : public HardwareSerial {
  protected:
    volatile USART_t *const _hwserial_module;
    const uint8_t _module_number;
    uint8_t _pin_set;

    // Has any byte been written to the UART since begin()
    bool _written;

    volatile rx_buffer_index_t _rx_buffer_head;
    volatile rx_buffer_index_t _rx_buffer_tail;
    volatile tx_buffer_index_t _tx_buffer_head;
    volatile tx_buffer_index_t _tx_buffer_tail;

    // Don't put any members after these buffers, since only the first
    // 32 bytes of this struct can be accessed quickly using the ldd
    // instruction.
    volatile unsigned char _rx_buffer[SERIAL_RX_BUFFER_SIZE];
    volatile unsigned char _tx_buffer[SERIAL_TX_BUFFER_SIZE];

  public:
    inline UartClass(volatile USART_t *hwserial_module, uint8_t hwserial_rx_pin, uint8_t hwserial_tx_pin, uint8_t hwserial_rx_pin_swap, uint8_t hwserial_tx_pin_swap, uint8_t dre_vect_num, uint8_t uart_mux, uint8_t uart_mux_swap);
    bool                    pins(uint8_t tx, uint8_t rx);
    bool                    swap(uint8_t mux_level = 1);
    void                   begin(uint32_t baud) {begin(baud, SERIAL_8N1);}
    void                   begin(uint32_t baud, uint16_t options);
    void                     end();
    void                printHex(const     uint8_t              b);
    void                printHex(const    uint16_t  w, bool s = 0);
    void                printHex(const    uint32_t  l, bool s = 0);
    void                printHex(const      int8_t  b)              {printHex((uint8_t)    b);            }
    void                printHex(const        char  b)              {printHex((uint8_t)    b);            }
    void              printHexln(const      int8_t  b)              {printHex((uint8_t)    b); println(); }
    void              printHexln(const        char  b)              {printHex((uint8_t)    b); println(); }
    void              printHexln(const     uint8_t  b)              {printHex(             b); println(); }
    void              printHexln(const    uint16_t  w, bool s = 0)  {printHex(          w, s); println(); }
    void              printHexln(const    uint32_t  l, bool s = 0)  {printHex(          l, s); println(); }
    void              printHexln(const     int16_t  w, bool s = 0)  {printHex((uint16_t)w, s); println(); }
    void              printHexln(const     int32_t  l, bool s = 0)  {printHex((uint16_t)l, s); println(); }
    uint8_t *           printHex(          uint8_t* p, uint8_t len, char sep = 0            );
    uint16_t *          printHex(         uint16_t* p, uint8_t len, char sep = 0, bool s = 0);
    volatile uint8_t *  printHex(volatile  uint8_t* p, uint8_t len, char sep = 0            );
    volatile uint16_t * printHex(volatile uint16_t* p, uint8_t len, char sep = 0, bool s = 0);

    virtual int availableForWrite(void);
    virtual int available(void);
    virtual      int peek(void);
    virtual      int read(void);
    virtual    void flush(void);
    virtual  size_t write(uint8_t ch);
    inline   size_t write(unsigned long n)  {return write((uint8_t)n);}
    inline   size_t write(long n)           {return write((uint8_t)n);}
    inline   size_t write(unsigned int n)   {return write((uint8_t)n);}
    inline   size_t write(int n)            {return write((uint8_t)n);}
    using Print::write; // pull in write(str) and write(buf, size) from Print
    explicit operator bool() {
      return true;
    }

    // Interrupt handlers - Not intended to be called externally

    // Interrupt handlers - Not intended to be called externally
    static void _rx_complete_irq(UartClass& uartClass);
    static void _tx_data_empty_irq(UartClass& uartClass);

  private:
    void _poll_tx_data_empty(void);
    static void        _set_pins(uint8_t* pinInfo, uint8_t port_num, uint8_t mux_setting, uint8_t enmask);
    static void         _mux_set(uint8_t* pinInfo, uint8_t port_num, uint8_t mux_code);
    static uint8_t _pins_to_swap(uint8_t* pinInfo, uint8_t port_num, uint8_t tx_pin, uint8_t rx_pin);
};

#if defined(USART0)
  extern UartClass Serial0;
#endif
#if defined(USART1)
  extern UartClass Serial1;
#endif
