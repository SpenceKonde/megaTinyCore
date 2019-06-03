/*
  UART.h - Hardware serial library for Wiring
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

  Modified 28 September 2010 by Mark Sproul
  Modified 14 August 2012 by Alarus
  Modified 3 December 2013 by Matthijs Kooijman
*/

#pragma once

#include <inttypes.h>
#include "api/HardwareSerial.h"
#include "pins_arduino.h"

// Define constants and variables for buffering incoming serial data.  We're
// using a ring buffer (I think), in which head is the index of the location
// to which to write the next incoming character and tail is the index of the
// location from which to read.
// NOTE: a "power of 2" buffer size is reccomended to dramatically
//       optimize all the modulo operations for ring buffers.
// WARNING: When buffer sizes are increased to > 256, the buffer index
// variables are automatically increased in size, but the extra
// atomicity guards needed for that are not implemented. This will
// often work, but occasionally a race condition can occur that makes
// Serial behave erratically. See https://github.com/arduino/Arduino/issues/2405
#if !defined(SERIAL_TX_BUFFER_SIZE)
#if ((RAMEND - RAMSTART) < 1023)
#define SERIAL_TX_BUFFER_SIZE 16
#else
#define SERIAL_TX_BUFFER_SIZE 64
#endif
#endif
#if !defined(SERIAL_RX_BUFFER_SIZE)
#if ((RAMEND - RAMSTART) < 1023)
#define SERIAL_RX_BUFFER_SIZE 16
#else
#define SERIAL_RX_BUFFER_SIZE 64
#endif
#endif
#if (SERIAL_TX_BUFFER_SIZE>256)
typedef uint16_t tx_buffer_index_t;
#else
typedef uint8_t tx_buffer_index_t;
#endif
#if  (SERIAL_RX_BUFFER_SIZE>256)
typedef uint16_t rx_buffer_index_t;
#else
typedef uint8_t rx_buffer_index_t;
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

class UartClass : public HardwareSerial
{
  protected:
    volatile USART_t * const _hwserial_module;

    volatile uint8_t const _hwserial_rx_pin;
    volatile uint8_t const _hwserial_tx_pin;

    volatile uint8_t const _uart_mux;

    // Has any byte been written to the UART since begin()
    bool _written;

    volatile rx_buffer_index_t _rx_buffer_head;
    volatile rx_buffer_index_t _rx_buffer_tail;
    volatile tx_buffer_index_t _tx_buffer_head;
    volatile tx_buffer_index_t _tx_buffer_tail;

    volatile uint8_t _hwserial_dre_interrupt_vect_num;
    volatile uint8_t _hwserial_dre_interrupt_elevated;
    volatile uint8_t _prev_lvl1_interrupt_vect;

    // Don't put any members after these buffers, since only the first
    // 32 bytes of this struct can be accessed quickly using the ldd
    // instruction.
    unsigned char _rx_buffer[SERIAL_RX_BUFFER_SIZE];
    unsigned char _tx_buffer[SERIAL_TX_BUFFER_SIZE];

  public:
    inline UartClass(volatile USART_t *hwserial_module, uint8_t hwserial_rx_pin, uint8_t hwserial_tx_pin, uint8_t dre_vect_num, uint8_t uart_mux);
    void begin(unsigned long baud) { begin(baud, SERIAL_8N1); }
    void begin(unsigned long, uint16_t);
    void end();
    virtual int available(void);
    virtual int peek(void);
    virtual int read(void);
    virtual int availableForWrite(void);
    virtual void flush(void);
    virtual size_t write(uint8_t);
    inline size_t write(unsigned long n) { return write((uint8_t)n); }
    inline size_t write(long n) { return write((uint8_t)n); }
    inline size_t write(unsigned int n) { return write((uint8_t)n); }
    inline size_t write(int n) { return write((uint8_t)n); }
    using Print::write; // pull in write(str) and write(buf, size) from Print
    operator bool() { return true; }

    // Interrupt handlers - Not intended to be called externally
    inline void _rx_complete_irq(void);
    void _tx_data_empty_irq(void);
};

#if defined(HWSERIAL0)
  extern UartClass Serial;
  #define HAVE_HWSERIAL0
#endif
#if defined(HWSERIAL1)
  extern UartClass Serial1;
  #define HAVE_HWSERIAL1
#endif
#if defined(HWSERIAL2)
  extern UartClass Serial2;
  #define HAVE_HWSERIAL2
#endif
#if defined(HWSERIAL3)
  extern UartClass Serial3;
  #define HAVE_HWSERIAL3
#endif

