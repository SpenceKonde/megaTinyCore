/* UART3.cpp - Hardware serial library for USART3
 * This library is free software released under LGPL 2.1.
 * See License.md for more information.
 * This file is part of megaTinyCore.
 *
 * Copyright (c) 2006 Nicholas Zambetti, Modified by
 * 11/23/2006 David A. Mellis, 9/20/2010 Mark Sproul,
 * 8/24/2012 Alarus, 12/3/2013 Matthijs Kooijman
 * Others (unknown) 2013-2017, 2017-2021 Spence Konde
 */

#include "Arduino.h"
#include "UART.h"
#include "UART_private.h"

#if defined(USART3)
  ISR(USART3_TXC_vect) {
    // only enabled in half duplex mode - we disable RX interrupt while sending.
    // When we are done sending, we reenable the RX interrupt and disable this one.
    // Note that we do NOT clear TXC flag, which the flush() method relies on.
    uint8_t ctrla;
    while (USART3.STATUS & USART_RXCIF_bm) {
      ctrla = USART3.RXDATAL;   // We sent these, so dump them, using local var as trashcan.
    }
    ctrla = USART3.CTRLA;       // Get current CTRLA
    ctrla |= USART_RXCIE_bm;    // turn on receive complete
    ctrla &= ~USART_TXCIE_bm;   // turn off transmit complete
    USART3.CTRLA = ctrla;       // Write it back to CTRLA.
  }

  ISR(USART3_RXC_vect) {
    UartClass::_rx_complete_irq(Serial3);
  }

  ISR(USART3_DRE_vect) {
    UartClass::_tx_data_empty_irq(Serial3);
  }

  UartClass Serial3(&USART3, 3, HWSERIAL3_MUX_DEFAULT);
#endif // HAVE_HWSERIAL3
