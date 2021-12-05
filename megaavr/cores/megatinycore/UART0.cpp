/* UART0.cpp - Hardware serial library for USART0
 * This library is free software released under LGPL 2.1.
 * See License.md for more information.
 * This file is part of megaTinyCore.
 *
 * Copyright (c) 2006 Nicholas Zambetti, Modified by
 * 11/23/2006 David A. Mellis, 9/20/2010 Mark Sproul,
 * 8/24/2012 Alarus, 12/3/2013 Matthijs Kooijman
 * Others (unknown) 2013-2017, 2017-2021 Spence Konde
 */

/* Each UartClass is defined in its own file, sine the linker pulls
 * in the entire file when any element inside is used. --gc-sections can
 * additionally cause unused symbols to be dropped, but ISRs have the
 * "used" attribute so are never dropped and they keep the
 * UartClass instance in as well. Putting each instance in its own
 * file prevents the linker from pulling in any unused instances in the
 * first place.
 */
#include "Arduino.h"
#include "UART.h"
#include "UART_private.h"

#if defined(USART0)
  #if defined(USE_ASM_TXC) && USE_ASM_TXC == 1 //&& defined(USART1) // No benefit to this if it's just one USART
    ISR(USART0_TXC_vect, ISR_NAKED) {
      __asm__ __volatile__(
            "push  r30"         "\n\t" // push the low byte of Z
            "ldi r30, 0x00"     "\n\t" // and put the low bit of this USART there - 0x20 * n
            "rjmp _do_txc"      "\n\t" // _do_txc pushes the other necessary registers and loads 0x08 into the high byte.
            :::);
    }
  #else
    ISR(USART0_TXC_vect) {
      uint8_t ctrla;
      while (USART0.STATUS & USART_RXCIF_bm) {
        ctrla      = USART0.RXDATAL;
      }
      ctrla        = USART0.CTRLA;
      ctrla       |= USART_RXCIE_bm; // turn on receive complete
      ctrla       &= ~USART_TXCIE_bm; // turn off transmit complete
      USART0.CTRLA = ctrla;
    }
  #endif

  #if !(defined(USE_ASM_RXC) && USE_ASM_RXC == 1 && (SERIAL_RX_BUFFER_SIZE == 128 || SERIAL_RX_BUFFER_SIZE == 64 || SERIAL_RX_BUFFER_SIZE == 32 || SERIAL_RX_BUFFER_SIZE == 16) /* && defined(USART1)*/)
    ISR(USART0_RXC_vect) {
      UartClass::_rx_complete_irq(Serial);
    }
  #else
    ISR(USART0_RXC_vect, ISR_NAKED) {
      __asm__ __volatile__(
            "push      r30"     "\n\t"
            "push      r31"     "\n\t"
            :::);
      __asm__ __volatile__(
            "rjmp   _do_rxc"     "\n\t"
            ::"z"(&Serial));
      __builtin_unreachable();
  }
  #endif
  #if !(defined(USE_ASM_DRE) && USE_ASM_DRE == 1 && \
       (SERIAL_RX_BUFFER_SIZE == 128 || SERIAL_RX_BUFFER_SIZE == 64 || SERIAL_RX_BUFFER_SIZE == 32 || SERIAL_RX_BUFFER_SIZE == 16) && \
       (SERIAL_TX_BUFFER_SIZE == 128 || SERIAL_TX_BUFFER_SIZE == 64 || SERIAL_TX_BUFFER_SIZE == 32 || SERIAL_TX_BUFFER_SIZE == 16))
    ISR(USART0_DRE_vect) {
      UartClass::_tx_data_empty_irq(Serial);
    }
  #else
    ISR(USART0_DRE_vect, ISR_NAKED) {
      __asm__ __volatile__(
                "push  r30"    "\n\t"
                "push  r31"    "\n\t"
                :::);
      __asm__ __volatile__(
                "rjmp _do_dre"  "\n"
                ::"z"(&Serial));
      __builtin_unreachable();
    }
  #endif

  UartClass Serial(&USART0, 0, HWSERIAL0_MUX_DEFAULT);
#endif
