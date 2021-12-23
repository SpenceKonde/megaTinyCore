/* UART.cpp - Hardware serial library, main file.
 * This library is free software released under LGPL 2.1.
 * See License.md for more information.
 * This file is part of megaTinyCore.
 *
 * Copyright (c) 2006 Nicholas Zambetti, Modified by
 * 11/23/2006 David A. Mellis, 9/20/2010 Mark Sproul,
 * 8/24/2012 Alarus, 12/3/2013 Matthijs Kooijman
 * Others (unknown) 2013-2017, 2017-2021 Spence Konde
 * and 2021 MX682X
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <util/atomic.h>
//#include <avr/io.h>
#include "Arduino.h"
#include "UART.h"
#include "UART_private.h"


#if defined(HAVE_HWSERIAL0) || defined(HAVE_HWSERIAL1) || defined(HAVE_HWSERIAL2) || defined(HAVE_HWSERIAL3)
  // macro to guard critical sections when needed for large TX buffer sizes
  #if (SERIAL_TX_BUFFER_SIZE > 256)
    #define TX_BUFFER_ATOMIC ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
  #else
    #define TX_BUFFER_ATOMIC
  #endif

/*##  ###  ####
  #  #     #   #
  #   ###  ####
  #      # # #
 ### ####  #  */

/*************************************
 Transmit Complete for Half Duplex
  This is a more efficient and scalable version of the TXC ISR, the original implementation is below:



#if defined(USART1)
  ISR(USART1_TXC_vect) {
    uint8_t ctrla;
    while (USART1.STATUS & USART_RXCIF_bm) {
      ctrla = USART1.RXDATAL;
    }
    ctrla = USART1.CTRLA;
    ctrla |= USART_RXCIE_bm; // turn on receive complete
    ctrla &= ~USART_TXCIE_bm; // turn off transmit complete
    USART1.CTRLA = ctrla;
  }

  in the USARTn.cpp, there's something like this that puts the low byte of the address of the USART into r30
  after saving the register. Then it just jumps to this routine, which loads the (always the same) high byte!
  and finishes up clearing out the other register we will need and saving the SREG. The logic is very simple,
  It's just ugly. It gets worse for the other interrupts because we have to work with the class, not just the
  hardware. Crucially the only thing different betweren the USARTs here isthe addressthey're working with.
  Much of the benefit comes from being able to get the benefits of functionsin terms of flash use without the
  penalties that come with using a true function in an ISR (50-80 byte prologue + epiloge), and also
  being aware that the X register can't do displacement when planning what goes in which regiseser... which
  is not avr-gcc's strong suite, and often ends up displacing from the X with adiw/sbiw spam. savings for one
  copy of it is small. Savings for several is HUGE! Performance is better, but not much.
  Biggest advantage is for 2-series with the dual UARTs but only 4k flash, where with all 3 interrupts like this
  will 158 bytes freed up from the ISRs alone!



ISR(USART1_TXC_vect, ISR_NAKED) {
  __asm__ __volatile__(
            "push  r30"         "\n\t" // 1
            "ldi r30, 0x20"     "\n\t" // 1
            "rjmp do_txc"       "\n\t" // 1
            :::);
}


*/

#if defined(USE_ASM_TXC) && USE_ASM_TXC == 1
  void __attribute__((naked)) __attribute__((used)) __attribute__((noreturn)) _do_txc(void) {
    __asm__ __volatile__(
      "_do_txc:"                  "\n\t"  //
        "push     r31"            "\n\t"  // push other half of Z register.
        "push     r24"            "\n\t"  // push r24
        "in       r24,     0x3f"  "\n\t"  // save sreg to r24
        "push     r24"            "\n\t"  // and push that. r30 pushed and loaded by ISR already.
        "ldi      r31,     0x08"  "\n\t"  // all USARTs are 0x08n0 where n is an even hex digit.
      "_txc_flush_rx:"            "\n\t"  // start of rx flush loop.
        "ld       r24,        Z"  "\n\t"  // rx data
        "ldd      r24,   Z +  4"  "\n\t"  // status
        "sbrs     r24,        7"  "\n\t"  // if RXC bit is set,...
        "rjmp     _txc_flush_rx"  "\n\t"  // .... skip this jump to remove more from the buffer.
        "ldd      r24,   Z +  5"  "\n\t"  // read CTRLA
        "andi     r24,     0xBF"  "\n\t"  // clear TXCIE
        "ori      r24,     0x80"  "\n\t"  // set RXCIE
        "std   Z +  5,      r24"  "\n\t"  // store CTRLA
        "pop      r24"            "\n\t"  // pop r24, xcontaining old sreg.
        "out     0x3f,      r24"  "\n\t"  // restore it
        "pop      r24"            "\n\t"  // pop r24 to get it's old value back
        "pop      r31"            "\n\t"  // and r31
        "pop      r30"            "\n\t"  // Pop the register the ISR did
        "reti"                    "\n"    // return from the interrupt.
        ::);
    __builtin_unreachable();
  }
#endif
/*

  ISR(USART0_RXC_vect, ISR_NAKED) {
    __asm__ __volatile__(
          "push      r30"     "\n\t"
          "push      r31"     "\n\t"
          :::);
    __asm__ __volatile__(
          "rjmp   do_rxc"     "\n\t"
          ::"z"(&Serial));
    __builtin_unreachable();
  }

*/
#if (defined(USE_ASM_RXC) && USE_ASM_RXC == 1 && (SERIAL_RX_BUFFER_SIZE == 256 || SERIAL_RX_BUFFER_SIZE == 128 || SERIAL_RX_BUFFER_SIZE == 64 || SERIAL_RX_BUFFER_SIZE == 32 || SERIAL_RX_BUFFER_SIZE == 16) /* && defined(USART1)*/ )
  // We only ever use this on the 2-series. 1-series doesn't gain anything with this. The inlining makes the compiler FAR more efficient. RXC isn't compiled stupidly,
  // the problem is that the ABI requires it to be inefficient as hell. But it's a big deal for the smaller size 2-series parts.
  void __attribute__((naked)) __attribute__((used)) __attribute__((noreturn)) _do_rxc(void) {
    __asm__ __volatile__(
      "_do_rxc:"                      "\n\t" //
        "push       r18"              "\n\t" // r30 and r31 pushed before this.
        "in         r18,      0x3f"   "\n\t" // Save SREG
        "push       r18"              "\n\t" //
        "push       r24"              "\n\t" //
        "push       r25"              "\n\t" //
        "push       r28"              "\n\t" //
        "push       r29"              "\n\t" //
        "ldd        r28,    Z + 12"   "\n\t" // Load USART into Y pointer
        "ldd        r29,    Z + 13"   "\n\t" // We interact with the USART only this once
        "ldd        r24,    Y +  1"   "\n\t" // load high byte first
        "ld         r25,         Y"   "\n\t" // then low byte of RXdata
        "sbrc       r24,         1"   "\n\t" // if there's a parity error, then
        "rjmp  _end_rxc"              "\n\t" // do nothing more (framing errors are ok?)
        "ldd        r28,    Z + 17"   "\n\t" // load current head index
        "ldi        r24,         1"   "\n\t" // Clear r24 and initialize it with 1
        "add        r24,       r28"   "\n\t" // add current head index to it
#if   SERIAL_RX_BUFFER_SIZE == 128
        "andi       r24,      0x7F"   "\n\t" // Wrap the head around
#elif SERIAL_RX_BUFFER_SIZE == 64
        "andi       r24,      0x3F"   "\n\t" // Wrap the head around
#elif SERIAL_RX_BUFFER_SIZE == 32
        "andi       r24,      0x1F"   "\n\t" // Wrap the head around
#elif SERIAL_RX_BUFFER_SIZE == 16
        "andi       r24,      0x0F"   "\n\t" // Wrap the head around
#elif SERIAL_RX_BUFFER_SIZE != 256
  #error "Can't happen - we already checked for unsupported buffer sizes!"
#endif
// otherwise it's 256, and wraps around naturally.
        "ldd        r18,    Z + 18"   "\n\t" // load tail index
        "cp         r18,       r24"   "\n\t" // See if head is at tail. If so, buffer full,
        "breq  _end_rxc"              "\n\t" // can't do anything, just restore state and leave.
        "add        r28,       r30"   "\n\t" // r28 has what would be the next index in it.
        "mov        r29,       r31"   "\n\t" // and this is the high byte of serial instance
        "ldi        r18,         0"   "\n\t" // need a known zero to carry.
        "adc        r29,       r18"   "\n\t" // carry - Y is now pointing 21 bytes before head
        "std     Y + 21,       r25"   "\n\t" // store the new char in buffer
        "std     Z + 17,       r24"   "\n\t" // write that new head index.
      "_end_rxc:"                     "\n\t" //
        "pop        r29"              "\n\t" // Y Pointer was used for head and usart
        "pop        r28"              "\n\t" //
        "pop        r25"              "\n\t" // r25 held the received character
        "pop        r24"              "\n\t" // r24 held rxdatah, then the new head.
        "pop        r18"              "\n\t" // Restore saved SREG
        "out       0x3f,       r18"   "\n\t" // and write back
        "pop        r18"              "\n\t" // used as tail pointer and z known zero.
        "pop        r31"              "\n\t" // end with Z which the isr pushed to make room for
        "pop        r30"              "\n\t" // pointer to serial instance
        "reti"                        "\n"   // return
        ::);
    __builtin_unreachable();

  }
#elif defined(USE_ASM_RXC) && USE_ASM_RXC == 1 && defined(USART1)
  #warning "USE_ASM_RXC is defined and this has more than one serial port, but the buffer size is not supported, falling back to the classical RXC."
#else
  void UartClass::_rx_complete_irq(UartClass& uartClass) {
    // if (bit_is_clear(*_rxdatah, USART_PERR_bp)) {
    uint8_t rxDataH = uartClass._hwserial_module->RXDATAH;
    uint8_t       c = uartClass._hwserial_module->RXDATAL;  // no need to read the data twice. read it, then decide what to do
    rx_buffer_index_t rxHead = uartClass._rx_buffer_head;

    if (!(rxDataH & USART_PERR_bm)) {
      // No Parity error, read byte and store it in the buffer if there is room
      // unsigned char c = uartClass._hwserial_module->RXDATAL;
      #if SERIAL_RX_BUFFER_SIZE > 256
        rx_buffer_index_t i = (uint16_t)(rxHead + 1) % SERIAL_RX_BUFFER_SIZE;
      #else
        rx_buffer_index_t i = (uint8_t)(rxHead + 1) % SERIAL_RX_BUFFER_SIZE;
      #endif

      // if we should be storing the received character into the location
      // just before the tail (meaning that the head would advance to the
      // current location of the tail), we're about to overflow the buffer
      // and so we don't write the character or advance the head.
      if (i != uartClass._rx_buffer_tail) {
        uartClass._rx_buffer[rxHead] = c;
        uartClass._rx_buffer_head = i;
      }
    }
  }
#endif
/*

ISR(USART0_DRE_vect, ISR_NAKED) {
  __asm__ __volatile__(
            push  r30
            push  r31    "\n\t"
            :::);
  __asm__ __volatile__(
            "rjmp do_dre"   "\n\t"
            ::"z"(&Serial));
  __builtin_unreachable();

*/

#if defined(USE_ASM_DRE) && USE_ASM_DRE == 1 && \
           (SERIAL_RX_BUFFER_SIZE == 256 || SERIAL_RX_BUFFER_SIZE == 128 || SERIAL_RX_BUFFER_SIZE == 64 || SERIAL_RX_BUFFER_SIZE == 32 || SERIAL_RX_BUFFER_SIZE == 16) && \
           (SERIAL_TX_BUFFER_SIZE == 256 || SERIAL_TX_BUFFER_SIZE == 128 || SERIAL_TX_BUFFER_SIZE == 64 || SERIAL_TX_BUFFER_SIZE == 32 || SERIAL_TX_BUFFER_SIZE == 16)
  void __attribute__((naked)) __attribute__((used)) __attribute__((noreturn)) _do_dre(void) {
    __asm__ __volatile__(
    "_do_dre:"                        "\n\t"
      "push        r18"               "\n\t"
      "in          r18,     0x3F"     "\n\t"
      "push        r18"               "\n\t"
      "push        r24"               "\n\t"
      "push        r25"               "\n\t"
      "push        r26"               "\n\t"
      "push        r27"               "\n\t"
      "set"                           "\n\t"  // SEt the T flag - we use this to determine how we got here and hence whether to rjmp to end of poll or reti
    "_poll_dre:"                      "\n\t"
      "push        r28"               "\n\t"
      "push        r29"               "\n\t"
      "ldi         r18,        0"     "\n\t"
      "ldd         r28,   Z + 12"     "\n\t"  // usart in Y
      "ldd         r29,   Z + 13"     "\n\t"  // usart in Y
      "ldd         r25,   Z + 20"     "\n\t"  // tx tail in r25
      "movw        r26,      r30"     "\n\t"  // copy of serial in X
      "add         r26,      r25"     "\n\t"  // Serial + txtail  - txtail 0~63
      "adc         r27,      r18"     "\n\t"  // Carry (X = &Serial + 0~63)
#if   SERIAL_RX_BUFFER_SIZE == 256
      "subi        r26,     0xEB"     "\n\t"  //
      "sbci        r27,     0xFE"     "\n\t"  // +277
      "ld          r24,        X"     "\n\t"  // grab the character
#elif SERIAL_RX_BUFFER_SIZE == 128
      "subi        r26,     0x6B"     "\n\t"  //
      "sbci        r27,     0xFF"     "\n\t"  // +149
      "ld          r24,        X"     "\n\t"  // grab the character
#elif SERIAL_RX_BUFFER_SIZE == 64
      "subi        r26,     0xAB"     "\n\t"  //
      "sbci        r27,     0xFF"     "\n\t"  // +85
      "ld          r24,        X"     "\n\t"  // grab the character
#elif SERIAL_RX_BUFFER_SIZE == 32
      "adiw        r26,     0x35"     "\n\t"  // +53
      "ld          r24,        X"     "\n\t"  // grab the character
#elif SERIAL_RX_BUFFER_SIZE == 16
      "adiw        r26,     0x25"     "\n\t"  // +37
      "ld          r24,        X"     "\n\t"  // grab the character
#else
  #error "Can't happen - we already checked for unsupported buffer sizes!"
#endif
      "ldi         r18,     0x40"     "\n\t"
      "std       Y + 4,      r18"     "\n\t"  // clear TXC
      "std       Y + 2,      r24"     "\n\t"  // write char
      "subi        r25,     0xFF"     "\n\t"  // txtail +1
#if   SERIAL_TX_BUFFER_SIZE == 128
      "andi        r25,     0x7F"     "\n\t" // Wrap the head around
#elif SERIAL_TX_BUFFER_SIZE == 64
      "andi        r25,     0x3F"     "\n\t" // Wrap the head around
#elif SERIAL_TX_BUFFER_SIZE == 32
      "andi        r25,     0x1F"     "\n\t" // Wrap the head around
#elif SERIAL_TX_BUFFER_SIZE == 16
      "andi        r25,     0x0F"     "\n\t" // Wrap the head around
#elif SERIAL_TX_BUFFER_SIZE != 256
  #error "Can't happen - we already checked for unsupported buffer sizes!"
#endif
// otherwise it's 256, and wraps around naturally.
      "ldd         r24,   Y +  5"     "\n\t"  // get CTRLA into r24
      "ldd         r18,   Z + 19"     "\n\t"  // txhead into r18
      "cpse        r18,      r25"     "\n\t"  // if they're the same
      "rjmp  _done_dre_irq"           "\n\t"
      "andi        r24,     0xDF"     "\n\t"  // DREIE off
      "std      Y +  5,      r24"     "\n\t"  // write new ctrla
    "_done_dre_irq:"                  "\n\t"  // Beginning of the end of the DRE
      "std      Z + 20,      r25"     "\n\t"  // store new tail
      "pop         r29"               "\n\t"  // pop Y
      "pop         r28"               "\n\t"  // finish popping Y
#if PROGMEM_SIZE > 8192
      "brts        .+4"               "\n\t"  // hop over the next insn if T bit set, means entered through do_dre, rather than poll_dre
      "jmp _poll_dre_done"            "\n\t"  // >8k parts must us jmp, otherwise it will give PCREL error.
#else
      "brts        .+2"               "\n\t"  // hop over the next insn if T bit set, means entered through do_dre, rather than poll_dre
      "rjmp _poll_dre_done"           "\n\t"  // 8k parts can use RJMP
#endif
      "pop         r27"               "\n\t"  // and continue with popping registers.
      "pop         r26"               "\n\t"
      "pop         r25"               "\n\t"
      "pop         r24"               "\n\t"
      "pop         r18"               "\n\t"  // pop SREG value from stack
      "out        0x3f,     r18"      "\n\t"  // restore SREG
      "pop         r18"               "\n\t"  // pop old r18
      "pop         r31"               "\n\t"  // pop the Z that the isr pushed.
      "pop         r30"               "\n\t"
      "reti"                        "\n"   // and RETI!
      ::);
    __builtin_unreachable();
  }
#elif defined(USE_ASM_DRE) && USE_ASM_DRE == 1
  #warning "USE_ASM_DRE is defined, but the buffer sizes are not supported, falling back to the classical DRE."
#else
  void UartClass::_tx_data_empty_irq(UartClass& uartClass) {
    USART_t* usartModule = (USART_t*)uartClass._hwserial_module;  // reduces size a little bit
    tx_buffer_index_t txTail = uartClass._tx_buffer_tail;

    // Check if tx buffer already empty. when called by _poll_tx_data_empty()
    //  if (uartClass._tx_buffer_head == txTail) {
      // Buffer empty, so disable "data register empty" interrupt
      // usartModule->CTRLA &= (~USART_DREIE_bm);
      // return;
    //} // moved to poll function to make ISR smaller and faster

    // There must be more data in the output
    // buffer. Send the next byte
    uint8_t c = uartClass._tx_buffer[txTail];

    // clear the TXCIF flag -- "can be cleared by writing a one to its bit
    // location". This makes sure flush() won't return until the bytes
    // actually got written. It is critical to do this BEFORE we write the next byte
    usartModule->STATUS = USART_TXCIF_bm;
    usartModule->TXDATAL = c;

    txTail = (txTail + 1) & (SERIAL_TX_BUFFER_SIZE - 1);  //% SERIAL_TX_BUFFER_SIZE;
    uint8_t ctrla = usartModule->CTRLA;
    if (uartClass._tx_buffer_head == txTail) {
      // Buffer empty, so disable "data register empty" interrupt
      ctrla &= ~(USART_DREIE_bm);
      // if (uartClass._state & 2) { // Shouldn't need this - we turned this on in write();
      //  ctrla |= USART_TXCIE_bm;  // in half duplex, turn on TXC interrupt, which will re-enable RX int.
      //}
      usartModule->CTRLA = ctrla;
      // } // else if (uartClass._state & 2) { // Why did I think I had to do this? We turned it on when we wrote...
      // ctrla &= ~USART_TXCIE_bm;
      // usartModule->CTRLA = ctrla;
    }
    uartClass._tx_buffer_tail = txTail;
  }
#endif

// To invoke data empty "interrupt" via a call, use this method
void UartClass::_poll_tx_data_empty(void) {
  if ((!(SREG & CPU_I_bm)) ||  CPUINT.STATUS) {
    // We're here because we're waiting for space in the buffer *or* we're in flush
    // and waiting for the last byte to leave, yet we're either in an ISR, or
    // interrupts are disabled so the ISR can't fire on it's own.
    //
    // Interrupts are disabled either globally or for data register empty,
    // or we are in another ISR. (It doesn't matter *which* ISR we are in
    // whether it's another level 0, the priority one, or heaven help us
    // the NMI, if the user code says to print something or flush the buffer
    // we might as well do it. It is entirely plausible that an NMI might
    // attempt to print out some sort of record of what happened.
    //
    // so we'll have to poll the "data register empty" flag ourselves.
    // If it is set, pretend an interrupt has happened and call the handler
    // to free up space for us.
    // -Spence 10/23/20
    // Invoke interrupt handler only if conditions data register is empty
    if ((*_hwserial_module).STATUS & USART_DREIF_bm) {
      if (_tx_buffer_head != _tx_buffer_tail) {
        // Buffer empty, so disable "data register empty" interrupt
        (*_hwserial_module).CTRLA &= (~USART_DREIE_bm);

        return;
      }
      #if !(defined(USE_ASM_DRE) && USE_ASM_DRE == 1 && \
                   (SERIAL_RX_BUFFER_SIZE == 256 || SERIAL_RX_BUFFER_SIZE == 128  || SERIAL_RX_BUFFER_SIZE == 64 || SERIAL_RX_BUFFER_SIZE == 32 || SERIAL_RX_BUFFER_SIZE == 16) && \
                   (SERIAL_TX_BUFFER_SIZE == 256 || SERIAL_TX_BUFFER_SIZE == 128  || SERIAL_TX_BUFFER_SIZE == 64 || SERIAL_TX_BUFFER_SIZE == 32 || SERIAL_TX_BUFFER_SIZE == 16))
        _tx_data_empty_irq(*this);
      #else
        #ifdef USART1
        void * thisSerial = this;
        #endif
        __asm__ __volatile__(
                "clt"              "\n\t" // CLear the T flag to signal to the ISR that we got there from here.
#if PROGMEM_SIZE > 8192
                "jmp _poll_dre"    "\n\t"
#else
                "rjmp _poll_dre"    "\n\t"
#endif
                "_poll_dre_done:"    "\n"
#ifdef USART1
                ::"z"((uint16_t)thisSerial)
#else
                ::"z"(&Serial)
#endif
                : "r18","r19","r24","r25","r26","r27");
      #endif
    }
  }
  // In case interrupts are enabled, the interrupt routine will be invoked by itself
  // Note that this currently does not handle cases where the DRE interruopt becomes
  // disabled, yet you are actually attempting to send. I don't think it can happen.
}

/*###  #   # ####  #    ###  ###     #   # #### ##### #   #  ###  ####   ###
 #   # #   # #   # #     #  #        ## ## #      #   #   # #   # #   # #
 ####  #   # ####  #     #  #        # # # ###    #   ##### #   # #   #  ###
 #     #   # #   # #     #  #        #   # #      #   #   # #   # #   #     #
 #      ###  ####  #### ###  ###     #   # ####   #   #   #  ###  ####   ##*/

// Invoke this function before 'begin' to define the pins used
bool UartClass::pins(uint8_t tx, uint8_t rx) {
  uint8_t ret_val = _pins_to_swap(_module_number, tx, rx);   // return 127 when correct swap number wasn't found
  return swap(ret_val);
}

uint8_t UartClass::_pins_to_swap(uint8_t port_num, uint8_t tx_pin, uint8_t rx_pin) {
  if (tx_pin == NOT_A_PIN && rx_pin == NOT_A_PIN) {
    return  128;            // get MUX_NONE
  } else {
    const uint8_t * muxtab_ptr = _usart_pins[port_num];
    if (*muxtab_ptr == tx_pin && (*(muxtab_ptr + 1) == rx_pin)) {
      return 0;
    }
    #if !defined(__AVR_ATtinyx24__)
      if ((*(muxtab_ptr + 4)) == tx_pin && (*(muxtab_ptr + 5) == rx_pin))
    #else
      if (port_num && (*(muxtab_ptr + 4)) == tx_pin && (*(muxtab_ptr + 5) == rx_pin))
    #endif
    {
      return 1;
    }
    return NOT_A_MUX; // At this point, we have checked all group codes for this peripheral. It ain't there. Return NOT_A_MUX.
  }
}

uint8_t UartClass::getPin(uint8_t pin) {
  if (pin >3) return NOT_A_PIN;
  return (_usart_pins[_module_number + _pin_set][pin]);
}

bool UartClass::swap(uint8_t newmux) {
  #if !(MEGATINYCORE_SERIES == 2 && defined(__ATtinyxy4__))
    // it's either a 0/1-series: They have options of 0 and 1.
    // Or it's a 2-series with 20 or 24 pins. Both USARTS have option of 0 & 1.
    if (newmux < 2) {
      _pin_set = newmux;
      return true;
    }
  #else
    // means it is a 14-pin 2-series, whose second USART doesn't have an alternate location.
    if (_module_number + newmux < 2) {
      _pin_set = newmux;
      return true;
    }
  #endif
  #if MEGATINYCORE_SERIES == 2
    else if (newmux == MUX_NONE) {  // 128 codes for MUX_NONE
      _pin_set = 3;
      return true;
    }
  #endif
  else {
    _pin_set = 0;
  }
  return false;
}

void UartClass::begin(unsigned long baud, uint16_t options) {
  // Make sure no transmissions are ongoing and USART is disabled in case begin() is called by accident
  // without first calling end()
  if (_state & 1) {
    this->end();
  }

  uint8_t ctrlc = (uint8_t) options;
  if (ctrlc == 0) {                         // see if they passed anything in low byte or SERIAL_CONFIG_VALID.
    ctrlc = (uint8_t)SERIAL_8N1;            // low byte of 0 could mean they want SERIAL_5N1. Or that they thought they'd
  }
  ctrlc &= ~0x04; // Now unset that 0x04 bit if it's set, because none of the values with it set are supported. We use that to smuggle in a "this constant was specified" for 5N1
  uint8_t ctrla = (uint8_t) (options >> 8);// CTRLA will get the remains of the options high byte.
  uint16_t baud_setting = 0;                // at this point it should be able to reuse those 2 registers that it received options in!
  uint8_t   ctrlb = (~ctrla & 0xC0);        // Top two bits (TXEN RXEN), inverted so they match he sense in the registers.
  if (baud   > F_CPU / 16) {            // if this baud is too fast for non-U2X
        ctrlb   |= USART_RXMODE0_bm;        // set the U2X bit in what will become CTRLB
        baud   >>= 1;                       // And lower the baud rate by haldf
  }
  baud_setting = (((4 * F_CPU) / baud));  // And now the registers that baud was passed in are done.
  if (baud_setting < 64)                      // so set to the maximum baud rate setting.
    baud_setting= 64;       // set the U2X bit in what will become CTRLB
  //} else if (baud < (F_CPU / 16800)) {      // Baud rate is too low
  //  baud_setting = 65535;                   // minimum baud rate.'
                                            // Baud setting done now we do the other options not in CTRLC;
  if (ctrla & 0x04) {                       // is ODME option set?
    ctrlb |= USART_ODME_bm;                 // set the bit in what will become CTRLB
  }
  ctrla &= 0x2B;                            // Only LBME and RS485 (both of them); will get written to CTRLA, but we leave the event bit.
  if (ctrlb & USART_RXEN_bm) {              // if RX is to be enabled
    ctrla  |= USART_RXCIE_bm;               // we will want to enable the ISR.
  }
  uint8_t setpinmask = ctrlb & 0xC8;        // ODME in bit 3, TX and RX enabled in bit 6, 7
  if ((ctrla & USART_LBME_bm) && (setpinmask == 0xC8)) { // if it's open-drain and loopback, need to set state bit 2.
    _state                 |= 2;            // since that changes some behavior (RXC disabled while sending) // Now we should be able to ST _state.
    setpinmask             |= 0x10;         // this tells _set_pins not to disturb the configuration on the RX pin.
  }
  if (ctrla & USART_RS485_bm) {             // RS485 mode recorded here too... because we need to set
    setpinmask             |= 0x01;         // set pin output if we need to do that. Datasheet isn't clear
  }
  uint8_t oldSREG = SREG;
  cli();
  volatile USART_t* MyUSART = _hwserial_module;
  (*MyUSART).CTRLB          = 0;            // gotta disable first - some things are enable-locked.
  (*MyUSART).CTRLC          = ctrlc;        // No reason not to set first.
  (*MyUSART).BAUD           = baud_setting; // Wish I could have set it long ago
  if (ctrla & 0x20) {                       // Now we have to do a bit of work
    setpinmask             &= 0x7F;         // Remove the RX pin in this case because we get the input from elsewhere.
    (*MyUSART).EVCTRL       = 1;            // enable event input - not clear from datasheet what's needed to
    (*MyUSART).TXPLCTRL     = 0xFF;         // Disable pulse length encoding.
  } else {
    (*MyUSART).EVCTRL       = 0;            // This needs to be turned off when not in use.
  }                                         // finally strip out the SERIAL_EVENT_RX bit which is in the DREIE
  (*MyUSART).CTRLA          = ctrla & 0xDF; // position, which we never set in begin.
  (*MyUSART).CTRLB          = ctrlb;        // Set the all important CTRLB...
  _set_pins(_module_number, _pin_set, setpinmask); // set up the pin(s)
  SREG = oldSREG;                             // re-enable interrupts, and we're done.
}

void UartClass::_set_pins(uint8_t mod_nbr, uint8_t mux_set, uint8_t enmask) {
  // Set the mux register
  #if defined(PORTMUX_USARTROUTEA)
    uint8_t muxregval    = PORTMUX.USARTROUTEA;
    muxregval           &= ~(mod_nbr ? 0x0C : 0x03);
    PORTMUX.USARTROUTEA  = (muxregval) | (mux_set << (mod_nbr ? 2 : 0)); // shift muxset left if needed.

  #else
    if (mux_set) {
      PORTMUX.CTRLB       |= 0x01; // for 0/1-series this can only be zero or 1
    } else {
      PORTMUX.CTRLB       &= 0xFE;
    }
  #endif
  #if MEGATINYCORE_SERIES == 2
    if (mux_set == 3) { // not connected to pins...
      return;           // so we are done!
    }
  #endif
  const uint8_t* muxrow = &(_usart_pins[mod_nbr + mux_set][0]);
  if ((enmask & 0x40 && !(enmask & 0x08))) {
    pinMode(muxrow[0], OUTPUT); // If and only if TX is enabled and open drain isn't should the TX pin be output.
  } else if (enmask & 0x50) { // if it is enabled but is in open drain mode, or is disabled, but loopback is enabled
    // TX should be INPUT_PULLUP.
    pinMode(muxrow[0], INPUT_PULLUP);
  }
  if (enmask & 0x80 && !(enmask & 0x10)) {
    // Likewise if RX is enabled, unless loopback mode is too (in which case we caught it above, it should be pulled up
    pinMode(muxrow[1], INPUT_PULLUP);
  }
  if (enmask & 0x01) { // finally if RS485 mode is enabled, we make XDIR output, otherwise it can't drive the pin.
    pinMode(muxrow[3], OUTPUT); // make XDIR output.
  }
  /*
  uint8_t muxrow = mod_nbr + mux_set;
  if ((enmask & 0x40 && !(enmask & 0x08))) {
    pinMode(_usart_pins[muxrow][0], OUTPUT); // If any only if TX is enabled and open drain isn't should the TX pin be output.
  } else if (enmask & 0x50) { // if it is enabled but is in open drain mode, or is disabled, but loopback is enabled
    // TX should be INPUT_PULLUP.
    pinMode(_usart_pins[muxrow][0], INPUT_PULLUP);
  }
  if (enmask & 0x80 && !(enmask & 0x10)) {
    // Likewise if RX is enabled, unless loopback mode is too (in which case we caught it above, it should be pulled up
    pinMode(_usart_pins[muxrow][1], INPUT_PULLUP);
  }
  if (enmask & 0x01) { // finally if RS485 mode is enabled, we make XDIR output, otherwise it can't drive the pin.
    pinMode(_usart_pins[muxrow][3], OUTPUT); // make XDIR output.
  }
  // And it is up to the user to configure the XCK pin as required for their application if they are using that.
  */
  /*
  uint8_t muxrow = mod_nbr + mux_set;
  if (enmask & 0x40) { // tx enabled
    pinMode(_usart_pins[muxrow][0], (enmask & 0x08) ? INPUT_PULLUP : OUTPUT);
  }
  if (enmask & 0x80) {
    if (!(enmask & 0x10)) {

      pinMode(_usart_pins[muxrow][1], INPUT_PULLUP);
    } else if (!(enmask & 0x40)) { // Loopback mode set, TX disabled, and RX enabled. Wacky configuration, but I guess that means TX should be INPUT_PULLUP.
      pinMode(_usart_pins[muxrow][0], INPUT_PULLUP);
    }
  }
  if (enmask & 0x01) { // RS485 enabled
    pinMode(_usart_pins[muxrow][3], OUTPUT); // make XDIR output.
  }
  */
}

void UartClass::end() {
  // wait for transmission of outgoing data
  flush();
  // Disable receiver and transmitter as well as the RX complete and the data register empty interrupts.
  // TXCIE only used in half duplex - we can just turn the damned thing off yo!
  volatile USART_t * temp = _hwserial_module; /* compiler does a slightly better job with this. */
  temp -> CTRLB &= 0; //~(USART_RXEN_bm | USART_TXEN_bm);
  temp -> CTRLA &= 0; //~(USART_RXCIE_bm | USART_DREIE_bm | USART_TXCIE_bm);
  temp -> STATUS =  USART_TXCIF_bm; // want to make sure no chanceofthat firing in error.
  // clear any received data
  _rx_buffer_head = _rx_buffer_tail;

  // Note: Does not change output pins
  // though the datasheetsays turning the TX module sets it to input.
  _state = 0;
}
  int UartClass::available(void) {
    return ((unsigned int)(SERIAL_RX_BUFFER_SIZE + _rx_buffer_head - _rx_buffer_tail)) & (SERIAL_RX_BUFFER_SIZE - 1); //% SERIAL_RX_BUFFER_SIZE;
  }

  int UartClass::peek(void) {
    if (_rx_buffer_head == _rx_buffer_tail) {
      return -1;
    } else {
      return _rx_buffer[_rx_buffer_tail];
    }
  }

  int UartClass::read(void) {
    // if the head isn't ahead of the tail, we don't have any characters
    if (_rx_buffer_head == _rx_buffer_tail) {
      return -1;
    } else {
      unsigned char c = _rx_buffer[_rx_buffer_tail];
      _rx_buffer_tail = (rx_buffer_index_t)(_rx_buffer_tail + 1) & (SERIAL_RX_BUFFER_SIZE - 1); // % SERIAL_RX_BUFFER_SIZE;
      return c;
    }
  }

  int UartClass::availableForWrite(void) {
    tx_buffer_index_t head;
    tx_buffer_index_t tail;

    TX_BUFFER_ATOMIC {
      head = _tx_buffer_head;
      tail = _tx_buffer_tail;
    }
    if (head >= tail) {
      return SERIAL_TX_BUFFER_SIZE - 1 - head + tail;
    }
    return tail - head - 1;
  }

  void UartClass::flush() {
    // If we have never written a byte, no need to flush. This special
    // case is needed since there is no way to force the TXCIF (transmit
    // complete) bit to 1 during initialization
    if (!_state & 1) {
      return;
    }

    // Check if we are inside an ISR already (e.g. connected to a different peripheral then UART), in which case the UART ISRs will not be called.
    // Spence 10/23/20: Changed _poll_tx_data_empty() to instead call the ISR directly in this case too
    // Why elevate the interrupt if we're going to go into a busywait loop checking if the interrupt is disabled and if so, check for the bit and
    // manually call the ISR if the bit is set... *anyway*? Plus, in write(), this mode will be enabled upon a write of a single character from an ISR
    // and will stay that way until the buffer is empty, which would mean that the fairly long and slow UART TX ISR would have priority over a
    // potentially very fast interrupt that the user may have set to priority level 1. Just because a whizz-bang feature is there doesn't mean
    // it's appropriate to use for applications where it has only very small benefits, and significant risk of surprising the user and causing
    // breakage of code that would otherwise work. Finally, the previous implementation didn't check if it was called from the current lvl1 ISR
    // and in that case flush(), and write() with full buffer would just straight up hang...

    // Spin until the data-register-empty-interrupt is disabled and TX complete interrupt flag is raised
    while (((*_hwserial_module).CTRLA & USART_DREIE_bm) || (!((*_hwserial_module).STATUS & USART_TXCIF_bm))) {

      // If interrupts are globally disabled or the and DR empty interrupt is disabled,
      // poll the "data register empty" interrupt flag to prevent deadlock

      _poll_tx_data_empty();
    }
    // When we get here, nothing is queued anymore (DREIE is disabled) and
    // the hardware finished transmission (TXCIF is set).
  }


  size_t UartClass::write(uint8_t c) {
    _state |= 1; // Record that we have written to serial since it was begun.
    // If the buffer and the data register is empty, just write the byte
    // to the data register and be done. This shortcut helps
    // significantly improve the effective data rate at high (>
    // 500kbit/s) bit rates, where interrupt overhead becomes a slowdown.
    if ((_tx_buffer_head == _tx_buffer_tail) && ((*_hwserial_module).STATUS & USART_DREIF_bm)) {
      if (_state & 2) { // in half duplex mode, we turn off RXC interrupt
        uint8_t ctrla = (*_hwserial_module).CTRLA;
        ctrla &= ~USART_RXCIE_bm;
        ctrla |=  USART_TXCIE_bm;
        (*_hwserial_module).STATUS = USART_TXCIF_bm;
        (*_hwserial_module).CTRLA = ctrla;
      } else {
        (*_hwserial_module).STATUS = USART_TXCIF_bm;
      }
      // MUST clear TXCIF **before** writing new char, otherwise ill-timed interrupt can cause it to erase the flag after the new charchter has been sent!
      (*_hwserial_module).TXDATAL = c;

      /* I cannot figure out *HOW* the DRE could be enabled at this point (buffer empty and DRE flag up)
       * When the buffer was emptied, it would have turned off the DREI after it loaded the last byte.
       * Thus, the only possible way this could happen is if an interrupt also tried to write to serial,
       * *immediately* after we checked that the buffer was empty, before we made it not empty. And
       * in that case, without this line it would lose one of the characters... with that line, it could
       * stop servicing DRE until another serial write, AND lose a character. That's not better! -Spence 4/2021
       * Original comments:
       * // Make sure data register empty interrupt is disabled to avoid
       * // that the interrupt handler is called in this situation
       * (*_hwserial_module).CTRLA &= (~USART_DREIE_bm);
       */
      return 1;
    }
    tx_buffer_index_t i = (_tx_buffer_head + 1) & (SERIAL_TX_BUFFER_SIZE - 1);  // % SERIAL_TX_BUFFER_SIZE;

    // If the output buffer is full, there's nothing we can do other than to
    // wait for the interrupt handler to empty it a bit (or emulate interrupts)
    while (i == _tx_buffer_tail) {
      _poll_tx_data_empty();
    }
    _tx_buffer[_tx_buffer_head] = c;
    _tx_buffer_head = i;
    if (_state & 2) { // in half duplex mode, we turn off RXC interrupt
      uint8_t ctrla = (*_hwserial_module).CTRLA;
      ctrla &= ~USART_RXCIE_bm;
      ctrla |= USART_TXCIE_bm | USART_DREIE_bm;
      (*_hwserial_module).STATUS = USART_TXCIF_bm;
      (*_hwserial_module).CTRLA = ctrla;
    } else {
      // Enable "data register empty interrupt"
      (*_hwserial_module).CTRLA |= USART_DREIE_bm;
    }
    return 1;
  }

  void UartClass::printHex(const uint8_t b) {
    char x = (b >> 4) | '0';
    if (x > '9')
      x += 7;
    write(x);
    x = (b & 0x0F) | '0';
    if (x > '9')
      x += 7;
    write(x);
  }

  void UartClass::printHex(const uint16_t w, bool swaporder) {
    uint8_t *ptr = (uint8_t *) &w;
    if (swaporder) {
      printHex(*(ptr++));
      printHex(*(ptr));
    } else {
      printHex(*(ptr + 1));
      printHex(*(ptr));
    }
  }

  void UartClass::printHex(const uint32_t l, bool swaporder) {
    uint8_t *ptr = (uint8_t *) &l;
    if (swaporder) {
      printHex(*(ptr++));
      printHex(*(ptr++));
      printHex(*(ptr++));
      printHex(*(ptr));
    } else {
      ptr+=3;
      printHex(*(ptr--));
      printHex(*(ptr--));
      printHex(*(ptr--));
      printHex(*(ptr));
    }
  }

  uint8_t * UartClass::printHex(uint8_t* p, uint8_t len, char sep) {
    for (byte i = 0; i < len; i++) {
      if (sep && i) write(sep);
      printHex(*p++);
    }
    println();
    return p;
  }

  uint16_t * UartClass::printHex(uint16_t* p, uint8_t len, char sep, bool swaporder) {
    for (byte i = 0; i < len; i++) {
      if (sep && i) write(sep);
      printHex(*p++, swaporder);
    }
    println();
    return p;
  }
  volatile uint8_t * UartClass::printHex(volatile uint8_t* p, uint8_t len, char sep) {
    for (byte i = 0; i < len; i++) {
      if (sep && i) write(sep);
      uint8_t t = *p++;
      printHex(t);
    }
    println();
    return p;
  }
  volatile uint16_t * UartClass::printHex(volatile uint16_t* p, uint8_t len, char sep, bool swaporder) {
    for (byte i = 0; i < len; i++) {
      if (sep && i) write(sep);
      uint16_t t = *p++;
      printHex(t, swaporder);
    }
    println();
    return p;
  }

#endif
