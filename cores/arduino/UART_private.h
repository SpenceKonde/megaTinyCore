/*
  UART_private.h - Hardware serial library for Wiring
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

  Modified 23 November 2006 by David A. Mellis
  Modified 28 September 2010 by Mark Sproul
  Modified 14 August 2012 by Alarus
*/

#include "wiring_private.h"

// this next line disables the entire UART.cpp, 
// this is so I can support Attiny series and any other chip without a uart
#if defined(HAVE_HWSERIAL0) || defined(HAVE_HWSERIAL1) || defined(HAVE_HWSERIAL2) || defined(HAVE_HWSERIAL3)

// Constructors ////////////////////////////////////////////////////////////////

UartClass::UartClass(
  volatile USART_t *hwserial_module,
  volatile uint8_t hwserial_rx_pin,
  volatile uint8_t hwserial_tx_pin,
  volatile uint8_t hwserial_dre_interrupt_vect_num,
  volatile uint8_t uart_mux) :
    _hwserial_module(hwserial_module),
    _hwserial_rx_pin(hwserial_rx_pin),
    _hwserial_tx_pin(hwserial_tx_pin),
    _uart_mux(uart_mux),
    _rx_buffer_head(0), _rx_buffer_tail(0),
    _tx_buffer_head(0), _tx_buffer_tail(0),
    _hwserial_dre_interrupt_vect_num(hwserial_dre_interrupt_vect_num),
    _hwserial_dre_interrupt_elevated(0),
    _prev_lvl1_interrupt_vect(0)
{
}

// Actual interrupt handlers //////////////////////////////////////////////////////////////

void UartClass::_rx_complete_irq(void)
{
  //if (bit_is_clear(*_rxdatah, USART_PERR_bp)) {
  if (!(((*_hwserial_module).RXDATAH) & USART_PERR_bm)) {
    // No Parity error, read byte and store it in the buffer if there is
    // room
    unsigned char c = (*_hwserial_module).RXDATAL;
    rx_buffer_index_t i = (unsigned int)(_rx_buffer_head + 1) % SERIAL_RX_BUFFER_SIZE;

    // if we should be storing the received character into the location
    // just before the tail (meaning that the head would advance to the
    // current location of the tail), we're about to overflow the buffer
    // and so we don't write the character or advance the head.
    if (i != _rx_buffer_tail) {
      _rx_buffer[_rx_buffer_head] = c;
      _rx_buffer_head = i;
    }
  } else {
    // Parity error, read byte but discard it
    (*_hwserial_module).RXDATAL;
  }
}

#endif // whole file
