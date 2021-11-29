/*
  UART.cpp - Hardware serial library for Wiring
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

  Created: 09.11.2017 07:29:09
  Author: M44307
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <util/atomic.h>
#include <avr/io.h>
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

void UartClass::_rx_complete_irq(UartClass& uartClass) {
  // if (bit_is_clear(*_rxdatah, USART_PERR_bp)) {
  uint8_t rxDataH = uartClass._hwserial_module->RXDATAH;
  uint8_t c = uartClass._hwserial_module->RXDATAL;  // no need to read the data twice. read it, then decide what to do
  rx_buffer_index_t rxHead = uartClass._rx_buffer_head;

  if (!(rxDataH & USART_PERR_bm)) {
    // No Parity error, read byte and store it in the buffer if there is room
    // unsigned char c = uartClass._hwserial_module->RXDATAL;
    rx_buffer_index_t i = (unsigned int)(rxHead + 1) % SERIAL_RX_BUFFER_SIZE;

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


void UartClass::_tx_data_empty_irq(UartClass& uartClass) {
  USART_t* usartModule = (USART_t*)uartClass._hwserial_module;  // reduces size a little bit
  tx_buffer_index_t txTail = uartClass._tx_buffer_tail;
/*
  // Check if tx buffer already empty. when called by _poll_tx_data_empty()
  if (uartClass._tx_buffer_head == txTail) {
    // Buffer empty, so disable "data register empty" interrupt
    usartModule->CTRLA &= (~USART_DREIE_bm);
    return;
  } //moved to poll function to make ISR smaller and faster
*/

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
    if (uartClass._state & 2) {
      ctrla |= USART_TXCIE_bm; //in half duplex, turn on TXC interrupt, which will re-enable RX int.
    }
    usartModule->CTRLA = ctrla;
  } else if (uartClass._state & 2) {
    ctrla &= ~USART_TXCIE_bm;
    usartModule->CTRLA = ctrla;
  }
  uartClass._tx_buffer_tail = txTail;
}


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
      _tx_data_empty_irq(*this);
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
  uint8_t ret_val = _pins_to_swap(_usart_pins, _mux_count, tx, rx);   // return 127 when correct swap number wasn't found
  return swap(ret_val);
}

bool UartClass::swap(uint8_t newmux) {
  #if (MEGATINYCORE_SERIES == 2) && !defined(__ATtinyxy4__))
    if (newmux == 1) {
      _pin_set = 0;
      return false;
    } else
  #else
    if (newmux < 2) {
      _pin_set = newmux;
      return true;
    } else if (newmux == MUX_NONE) {  //128 codes for MUX_NONE
      _pin_set = 3;
      return true;
    } else {
      _pin_set = 0;
    }
    return false;
  #endif
}

void UartClass::begin(unsigned long baud, uint16_t options) {
  // Make sure no transmissions are ongoing and USART is disabled in case begin() is called by accident
  // without first calling end()
  if (_state & 1) {
    this->end();
  }

  uint8_t ctrlc;
  ctrlc = (uint8_t) options;                // Pull out low byte
  if (ctrlc == 0) {                       // see if they passed anything in low byte. Ether they assuned they'd get the default
    if ((options & 0x1004) == 0) {         // or they want SERIAL_5N1. But if they used that constant, a ketbit is set - do we see that?
      ctrlc = (uint8_t SERIAL_8N1);         // If not give them the default that they exoected;
    }
  }
  uint16_t baud_setting = 0;
  uint8_t ctrla = (uint8_t) (options >> 8); // the options high byte...
  uint8_t ctrlb = (~ctrla & 0xC0);          // Top two bits (TXEN RXEN), inverted so they match he sense in the registers.
  if (baud > F_CPU / 8) {                    // requested baud too high!
    baud_setting = 64;                      // so set to the maximum baud rate setting.
    ctrlb |= USART_RXMODE0_bm;              // set the U2X bit in what will become CTRLB
  } else if (baud < (F_CPU / 16800)) {      // Baud rate is too low
    baud_setting = 65535;                   // minimum baud rate.
  } else {
    if (baud > F_CPU / 16) {                // if this baud is too fast for non-U2X
      ctrlb |= USART_RXMODE0_bm;            // set the U2X bit in what will become CTRLB
      baud >>= 1;                           // And lower the baud rate by haldf
    }                                       // Calculate the baud_setting for valid baud rate.
    baud_setting = (((4 * F_CPU) / baud));
  }
  // Baud setting done now we do the other options.
  // that aren't in CTRLC;
  if (ctrla & 0x04) {                       // is ODME option set?
    ctrlb |= USART_ODME_bm;                 // set the big in what will become CTRLB
  }
  ctrla &= 0x09;                            // Only LBME and RS485; will get written to CTRLA.
  if (ctrlb & USART_RXEN_bm) {              // if RX is to be enabled
    ctrla  |= USART_RXCIE_bm;               // we will want to enable the ISR.
  }
  uint8_t oldSREG = SREG;
  cli();
  volatile USART_t* MyUSART = _hwserial_module;
  (*MyUSART).CTRLA          = ctrla;
  (*MyUSART).CTRLB          = 0;
  (*MyUSART).CTRLC          = (uint8_t) options;
  (*MyUSART).BAUD           = baud_setting;
  uint8_t setpinmask = ctrlb & 0xC8;        // ODME in bit 3, TX and RX enabled in bit 6, 7
  // Set USART mode of operation
  if (options & SERIAL_EVENT_RX) {
    setpinmask             &= 0x7F; // Remove the RX pin in this case because we get the input from elsewhere.
    (*MyUSART).EVCTRL       = 1;    // enable event input - not clear from datasheet what's needed to
    (*MyUSART).TXPLCTRL     = 0xFF; // Disable pulse length encoding.
  } else {
    (*MyUSART).EVCTRL       = 0;
  }
  (*MyUSART).CTRLB          = ctrlb;
  if ((ctrla & USART_LBME_bm) && ((ctrlb & 0xC0) == 0xC0)) { //if it's half duplex requires special treatment if
    _state       = 2;                       // since that changes some behavior (RXC disabled while sending)
    setpinmask  |= 0x10;                    // this tells _set_pins not to disturb the configuration on the RX pin.
  }
  if (ctrla & USART_RS4850_bm) {             // RS485 mode recorded here too... because we may need to
    setpinmask  |= 0x01;                    // set pin output if we need to do that. Datasheet isn't clear
  }
  _set_pins(_usart_pins, _mux_count, _pin_set, setpinmask);
  SREG=oldSREG;
}

void UartClass::end() {
  // wait for transmission of outgoing data
  flush();
  // Disable receiver and transmitter as well as the RX complete and the data register empty interrupts.
  // TXCIE only used in half duplex
  (*_hwserial_module).CTRLB &=  ~(USART_RXEN_bm | USART_TXEN_bm);
  (*_hwserial_module).CTRLA &= ~(USART_RXCIE_bm | USART_DREIE_bm | USART_TXCIE_bm);
  (*_hwserial_module).STATUS =  USART_TXCIF_bm; // want to make sure no chanceofthat firing in error.
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
    if (!_written) {
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
    _state |= 1;

    // If the buffer and the data register is empty, just write the byte
    // to the data register and be done. This shortcut helps
    // significantly improve the effective data rate at high (>
    // 500kbit/s) bit rates, where interrupt overhead becomes a slowdown.
    if ((_tx_buffer_head == _tx_buffer_tail) && ((*_hwserial_module).STATUS & USART_DREIF_bm)) {
      if (_state & 2) { //in half duplex mode, we turn off RXC interrupt
        uint8_t ctrla = (*_hwserial_module).CTRLA;
        ctrla &= ~USART_RXCIE_bm;
        ctrla |= USART_TXCIE_bm;
        (*_hwserial_module).STATUS = USART_TXCIF_bm;
        (*_hwserial_module).CTRLA = ctrla;
      } else {
        (*_hwserial_module).STATUS = USART_TXCIF_bm;
      }
      // MUST clear TXCIF **before** writing new char, otherwise ill-timed interrupt can cause it to erase the flag after the new charchter has been sent!
      (*_hwserial_module).TXDATAL = c;


  /*   * I cannot figure out *HOW* the DRE could be enabled at this point (buffer empty and DRE flag up)
       * When the buffer was emptied, it would have turned off the DREI after it loaded the last byte.
       * Thus, the only possible way this could happen is if an interrupt also tried to write to serial,
       * *immediately* after we checked that the buffer was empty, before we made it not empty. And
       * in that case, without this line it would lose one of the characters... with that line, it could
       * stop servicing DRE until another serial write, AND lose a character. I think it's a full 10 bytes
       * 2 to read with LDS, 1 to modify, 2 to write back with STS. It's gone!    -Spence 4/2021
       * Original comments:
       // Make sure data register empty interrupt is disabled to avoid
       // that the interrupt handler is called in this situation
       (*_hwserial_module).CTRLA &= (~USART_DREIE_bm);
  */

      return 1;
    }

    tx_buffer_index_t i = (_tx_buffer_head + 1) & (SERIAL_TX_BUFFER_SIZE - 1);  // % SERIAL_TX_BUFFER_SIZE;

    // If the output buffer is full, there's nothing for it other than to
    // wait for the interrupt handler to empty it a bit (or emulate interrupts)
    while (i == _tx_buffer_tail) {
      _poll_tx_data_empty();
    }
    _tx_buffer[_tx_buffer_head] = c;
    _tx_buffer_head = i;
    if (_state & 2) { //in half duplex mode, we turn off RXC interrupt
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
