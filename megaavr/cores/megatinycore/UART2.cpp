/* UART2.cpp - Hardware serial library for USART2
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

#if defined(USART2)
  ISR(USART2_TXC_vect) {
    // only enabled in half duplex mode - we disable RX interrupt while sending.
    // When we are done sending, we reenable the RX interrupt and disable this one.
    // Note that we do NOT clear TXC flag, which the flush() method relies on.
    uint8_t ctrla;
    while (USART2.STATUS & USART_RXCIF_bm) {
      ctrla = USART2.RXDATAL;   // We sent these, so dump them, using local var as trashcan.
    }
    ctrla = USART2.CTRLA;       // Get current CTRLA
    ctrla |= USART_RXCIE_bm;    // turn on receive complete
    ctrla &= ~USART_TXCIE_bm;   // turn off transmit complete
    USART2.CTRLA = ctrla;       // Write it back to CTRLA.
  }

  ISR(USART2_RXC_vect) {
    UartClass::_rx_complete_irq(Serial2);
  }

  ISR(USART2_DRE_vect) {
    UartClass::_tx_data_empty_irq(Serial2);
  }

  UartClass Serial2(&USART2, 2, HWSERIAL2_MUX_DEFAULT);
#endif // HAVE_HWSERIAL2
