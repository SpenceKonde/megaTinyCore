/* UART1.cpp - Hardware serial library for USART1
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

#if defined(USART1)
  #if USE_ASM_TXC == 1
    ISR(USART1_TXC_vect, ISR_NAKED) {
      __asm__ __volatile__(
            "push  r30"           "\n\t" // push the low byte of Z
            "ldi r30, 0x20"       "\n\t" // and put the low bit of this USART there - 0x20 * n
#if PROGMEM_SIZE > 8192
            "jmp _do_txc"         "\n\t"
#else
            "rjmp _do_txc"        "\n\t"
#endif // _do_txc pushes the other necessary registers and loads 0x08 into the high byte.
            :::);
    }
  #else
    ISR(USART1_TXC_vect) {
      uint8_t ctrla;
      while (USART1.STATUS & USART_RXCIF_bm) {
        ctrla      = USART1.RXDATAL;
      }
      ctrla        = USART1.CTRLA;
      ctrla       |= USART_RXCIE_bm; // turn on receive complete
      ctrla       &= ~USART_TXCIE_bm; // turn off transmit complete
      USART1.CTRLA = ctrla;
    }
  #endif
  #if !(USE_ASM_RXC == 1 && (SERIAL_RX_BUFFER_SIZE == 256 || SERIAL_RX_BUFFER_SIZE == 128 || SERIAL_RX_BUFFER_SIZE == 64 || SERIAL_RX_BUFFER_SIZE == 32 || SERIAL_RX_BUFFER_SIZE == 16))
    ISR(USART1_RXC_vect) {
      UartClass::_rx_complete_irq(Serial1);
    }
  #else
      ISR(USART1_RXC_vect, ISR_NAKED) {
        __asm__ __volatile__(
              "push      r30"     "\n\t"
              "push      r31"     "\n\t"
              :::);
        __asm__ __volatile__(
#if PROGMEM_SIZE > 8192
              "jmp   _do_rxc"     "\n\t"
#else
              "rjmp   _do_rxc"    "\n\t"
#endif
              ::"z"(&Serial1));
        __builtin_unreachable();
    }
  #endif
  #if !(USE_ASM_DRE == 1 && (SERIAL_RX_BUFFER_SIZE == 256 || SERIAL_RX_BUFFER_SIZE == 128 || SERIAL_RX_BUFFER_SIZE == 64 || SERIAL_RX_BUFFER_SIZE == 32 || SERIAL_RX_BUFFER_SIZE == 16) && \
                            (SERIAL_TX_BUFFER_SIZE == 256 || SERIAL_TX_BUFFER_SIZE == 128 || SERIAL_TX_BUFFER_SIZE == 64 || SERIAL_TX_BUFFER_SIZE == 32 || SERIAL_TX_BUFFER_SIZE == 16))
    ISR(USART1_DRE_vect) {
      UartClass::_tx_data_empty_irq(Serial1);
    }
  #else
    ISR(USART1_DRE_vect, ISR_NAKED) {
      __asm__ __volatile__(
                "push  r30"       "\n\t"
                "push  r31"       "\n\t"
                :::);
      __asm__ __volatile__(
#if PROGMEM_SIZE > 8192
                "jmp _do_dre"     "\n\t"
#else
                "rjmp _do_dre"    "\n\t"
#endif
                ::"z"(&Serial1));
      __builtin_unreachable();
    }
  #endif
  UartClass Serial1(&USART1, (uint8_t*)_usart1_pins, MUXCOUNT_USART1, HWSERIAL1_MUX_DEFAULT);
#endif  // HWSERIAL1
