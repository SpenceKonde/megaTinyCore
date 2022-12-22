/* UART0.cpp - Hardware serial library for USART0
 * This library is free software released under LGPL 2.1.
 * See License.md for more information.
 * This file is part of megaTinyCore.
 *
 * Copyright (c) 2006 Nicholas Zambetti, Modified by
 * 11/23/2006 David A. Mellis, 9/20/2010 Mark Sproul,
 * 8/24/2012 Alarus, 12/3/2013 Matthijs Kooijman
 * unknown others 2013-2020, 2020-2022 Spence Konde
 */

/* Each HardwareSerial is defined in its own file, sine the linker pulls
 * in the entire file when any element inside is used. --gc-sections can
 * additionally cause unused symbols to be dropped, but ISRs have the
 * "used" attribute so are never dropped and they keep the
 * HardwareSerial instance in as well. Putting each instance in its own
 * file prevents the linker from pulling in any unused instances in the
 * first place.
 * There are now two versions of each ISR. All versions are stubs to call
 * the real code (so it is not duplicated). The ASM versions are more
 * efficient than calling normally can ever be because they completely
 * ignore the ABI rules for the transition from duplicated to shared
 * code and call it the most effcieient way possiblem so the almost all of
 * the flash overhead is in the shared section. They'll stop working if
 * anything changes, and it's not entirely clear whether the trick of
 * dropping out of assembly while naked to grab the address is legal,
 * though there's no reason it shouldn't be.
 * The assembly implementations over in UART.cpp depends on the structure
 * of the SerialClass. Any changes to the class member variables will
 * require changes to the asm to match.
 * This was done for 1.5.12 to correct a serious defect. amd fix a
 * to make things work better with millis off, and free up 4 bytes of sram.
 */
#include "Arduino.h"
#include "UART.h"
#include "UART_private.h"

#if defined(USART0)
  #if defined(USE_ASM_TXC) && USE_ASM_TXC == 1 //&& defined(USART1) // No benefit to this if it's just one USART
    // Note the difference between this and the other ISRs - here we don't care at all about the serial object, we just have to work with the USART
    ISR(USART0_TXC_vect, ISR_NAKED) {
      __asm__ __volatile__(
            "push  r30"           "\n\t" // push the low byte of Z - we start out 5-6 clocks behind the ball, these three instructions take 4-5 -> 9 or 11 by th time we reach _do_txc
            "ldi r30, 0x00"       "\n\t" // and put the low bit of this USART there - 0x20 * n
#if PROGMEM_SIZE > 8192
            "jmp _do_txc"         "\n\t"
#else
            "rjmp _do_txc"        "\n\t"
#endif // _do_txc pushes the other necessary registers and loads 0x08 into the high byte.
       // The reason this is possible here and not elsewhere is because TXC only needs the USART, while the others need the HardwareSerial instance.
            :::);
    }
#elif defined(USE_ASM_TXC) && USE_ASM_TXC == 2
    ISR(USART0_TXC_vect, ISR_NAKED) {
        __asm__ __volatile__(
              "push      r30"     "\n\t"
              "push      r31"     "\n\t"
              :::);
        __asm__ __volatile__(
#if PROGMEM_SIZE > 8192
              "jmp   _do_txc"     "\n\t"
#else
              "rjmp   _do_txc"    "\n\t"
#endif
              ::"z"(&Serial0));
        __builtin_unreachable();
    }

  #else
    ISR(USART0_TXC_vect) {
      // only enabled in half duplex mode - we disable RX interrupt while sending.
      // When we are done sending, we re-enable the RX interrupt and disable this one.
      // Note that we do NOT clear TXC flag, which the flush() method relies on.
      uint8_t ctrla;
      while (USART0.STATUS & USART_RXCIF_bm) {
        ctrla = USART0.RXDATAL;   // We sent these, so dump them, using local var as trashcan.
      }
      ctrla = USART0.CTRLA;       // Get current CTRLA
      ctrla |= USART_RXCIE_bm;    // turn on receive complete
      ctrla &= ~USART_TXCIE_bm;   // turn off transmit complete
      USART0.CTRLA = ctrla;       // Write it back to CTRLA.
    }
  #endif

  #if !(defined(USE_ASM_RXC) && (USE_ASM_RXC == 1 || USE_ASM_RXC == 2) && (SERIAL_RX_BUFFER_SIZE == 128 || SERIAL_RX_BUFFER_SIZE == 64 || SERIAL_RX_BUFFER_SIZE == 32 || SERIAL_RX_BUFFER_SIZE == 16) /* && defined(USART1)*/)
    ISR(USART0_RXC_vect) {
      HardwareSerial::_rx_complete_irq(Serial);
    }
  #else
      ISR(USART0_RXC_vect, ISR_NAKED) {
        __asm__ __volatile__(
              "push      r30"     "\n\t" //we start out 5-6 clocks behind the ball, then do 2 push + 2 ldi + 2-3 for jump = 11 or 13 clocks to _do_rxc (and dre is the same)
              "push      r31"     "\n\t"
              "cbi   0x1F, 0"     "\n\t"
              :::);
        __asm__ __volatile__(
#if PROGMEM_SIZE > 8192
              "jmp   _do_rxc"     "\n\t"
#else
              "rjmp   _do_rxc"    "\n\t"
#endif
              ::"z"(&Serial0));
        __builtin_unreachable();
    }
  #endif
  #if !(defined(USE_ASM_DRE) && USE_ASM_DRE == 1 && \
       (SERIAL_RX_BUFFER_SIZE == 128 || SERIAL_RX_BUFFER_SIZE == 64 || SERIAL_RX_BUFFER_SIZE == 32 || SERIAL_RX_BUFFER_SIZE == 16) && \
       (SERIAL_TX_BUFFER_SIZE == 128 || SERIAL_TX_BUFFER_SIZE == 64 || SERIAL_TX_BUFFER_SIZE == 32 || SERIAL_TX_BUFFER_SIZE == 16))
    ISR(USART0_DRE_vect) {
      HardwareSerial::_tx_data_empty_irq(Serial);
    }
  #else
    ISR(USART0_DRE_vect, ISR_NAKED) {
      __asm__ __volatile__(
                "push  r30"       "\n\t"
                "push  r31"       "\n\t"
                "cbi   0x1F, 0"   "\n\t"
                :::);
      __asm__ __volatile__(
#if PROGMEM_SIZE > 8192
                "jmp _do_dre"     "\n\t"
#else
                "rjmp _do_dre"    "\n\t"
#endif
                ::"z"(&Serial0));
      __builtin_unreachable();
    }
  #endif

  HardwareSerial Serial0(&USART0, 0, HWSERIAL0_MUX_DEFAULT); //Elsewhere we define an alias of Serial for this :-)
#endif
