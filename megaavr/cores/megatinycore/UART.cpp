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

// this next line disables the entire UART.cpp,
// this is so I can support Attiny series and any other chip without a uart
#if defined(HAVE_HWSERIAL0) || defined(HAVE_HWSERIAL1) || defined(HAVE_HWSERIAL2) || defined(HAVE_HWSERIAL3)

// SerialEvent functions are weak, so when the user doesn't define them,
// the linker just sets their address to 0 (which is checked below).
// The Serialx_available is just a wrapper around Serialx.available(),
// but we can refer to it weakly so we don't pull in the entire
// UART instance if the user doesn't also refer to it.
#ifndef NOSERIALEVENT
#if defined(HAVE_HWSERIAL0)
  void serialEvent() __attribute__((weak));
  bool Serial0_available() __attribute__((weak));
#endif

#if defined(HAVE_HWSERIAL1)
  void serialEvent1() __attribute__((weak));
  bool Serial1_available() __attribute__((weak));
#endif

#if defined(HAVE_HWSERIAL2)
  void serialEvent2() __attribute__((weak));
  bool Serial2_available() __attribute__((weak));
#endif

#if defined(HAVE_HWSERIAL3)
  void serialEvent3() __attribute__((weak));
  bool Serial3_available() __attribute__((weak));
#endif

void serialEventRun(void) {
  #if defined(HAVE_HWSERIAL0)
  if (Serial0_available && serialEvent && Serial0_available()) {
    serialEvent();
  }
  #endif
  #if defined(HAVE_HWSERIAL1)
  if (Serial1_available && serialEvent1 && Serial1_available()) {
    serialEvent1();
  }
  #endif
  #if defined(HAVE_HWSERIAL2)
  if (Serial2_available && serialEvent2 && Serial2_available()) {
    serialEvent2();
  }
  #endif
  #if defined(HAVE_HWSERIAL3)
  if (Serial3_available && serialEvent3 && Serial3_available()) {
    serialEvent3();
  }
  #endif
}
#endif
// macro to guard critical sections when needed for large TX buffer sizes
#if (SERIAL_TX_BUFFER_SIZE > 256)
  #define TX_BUFFER_ATOMIC ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
#else
  #define TX_BUFFER_ATOMIC
#endif

// Actual interrupt handlers //////////////////////////////////////////////////////////////

void UartClass::_tx_data_empty_irq(void) {
  // Check if tx buffer already empty.
  if (_tx_buffer_head == _tx_buffer_tail) {
    // Buffer empty, so disable "data register empty" interrupt
    (*_hwserial_module).CTRLA &= (~USART_DREIE_bm);
    return;
  }

  // There must be more data in the output
  // buffer. Send the next byte
  unsigned char c = _tx_buffer[_tx_buffer_tail];
  _tx_buffer_tail = (_tx_buffer_tail + 1) & (SERIAL_TX_BUFFER_SIZE-1); //% SERIAL_TX_BUFFER_SIZE;

  // clear the TXCIF flag -- "can be cleared by writing a one to its bit
  // location". This makes sure flush() won't return until the bytes
  // actually got written
  (*_hwserial_module).STATUS = USART_TXCIF_bm;

  (*_hwserial_module).TXDATAL = c;

  if (_tx_buffer_head == _tx_buffer_tail) {
    // Buffer empty, so disable "data register empty" interrupt
    (*_hwserial_module).CTRLA &= (~USART_DREIE_bm);

    //Take the DRE interrupt back no normal priority level if it has been elevated
    //if (_hwserial_dre_interrupt_elevated) {
    //  CPUINT.LVL1VEC = _prev_lvl1_interrupt_vect;
    //  _hwserial_dre_interrupt_elevated = 0;
    //}
  }
}

// To invoke data empty "interrupt" via a call, use this method
void UartClass::_poll_tx_data_empty(void) {
  if ((!(SREG & CPU_I_bm)) || (!((*_hwserial_module).CTRLA & USART_DREIE_bm)) || CPUINT.STATUS) {
    // Interrupts are disabled either globally or for data register empty,
    // or we are in another ISR. (It doesn't matter *which* ISR we are in
    // whether it's another level 0, the priority one, or heaven help us
    // the NMI, if the user code says to print something or flush the buffer
    // we might as well do it. It is entirely plausible that an NMI might
    // attempt to print out some sort of record of what happened. -Spence 10/23/20
    //
    // so we'll have to poll the "data register empty" flag ourselves.
    // If it is set, pretend an interrupt has happened and call the handler
    // to free up space for us.

    // Invoke interrupt handler only if conditions data register is empty
    if ((*_hwserial_module).STATUS & USART_DREIF_bm) {
      _tx_data_empty_irq();
    }
  }
  // In case interrupts are enabled, the interrupt routine will be invoked by itself
}


// Public Methods //////////////////////////////////////////////////////////////

// Invoke this function before 'begin' to define the pins used
bool UartClass::pins(uint8_t tx, uint8_t rx) {
  for (_pin_set = 0; _pin_set < SERIAL_PIN_SETS; ++_pin_set) {
    if (tx == _hw_set[_pin_set].tx_pin && rx == _hw_set[_pin_set].rx_pin) {
      // We are good, this set of pins is supported
      return true;
    }
  }
  _pin_set = 0; // Default to standard
  return false;
}

bool UartClass::swap(uint8_t state) {
  if (state == 1) { // Use alternative pin position
    _pin_set = state;
    return true;
  } else if (state == 0) { // Use default pin position
    _pin_set = 0;
    return true;
  } else { // Invalid swap value. Use default position
    _pin_set = 0;
    return false;
  }
}

void UartClass::begin(unsigned long baud, uint16_t config) {
  if (__builtin_constant_p(baud)) {
    if (baud > (F_CPU/8)) badArg("Unachievable baud, too high - must be less than F_CPU/8");
    if (baud < (F_CPU/16800)) badArg("Unachievable baud, too low - must be more than F_CPU/16800 (16384 plus allowable error)");
  }
  // Make sure no transmissions are ongoing and USART is disabled in case begin() is called by accident
  // without first calling end()
  if (_written) {
    this->end();
  }

  struct UartPinSet *set = &_hw_set[_pin_set];

  int32_t baud_setting = 0;
  uint8_t rxmode=0;

  // Use CLK2X if appropriate.
  #if (F_CPU > 2000000)
  if(baud>=(38400*(F_CPU/1000000))) {
    rxmode = USART_RXMODE_CLK2X_gc;
    baud=baud>>1;
  }
  //else {
  //  rxmode = USART_RXMODE_NORMAL_gc;
  //}
  #else
  //if clocked at 1 or 2 MHz, always use CLK2X mode and save a bit of space, we correct for not shifting the baud value below, as it saves a bit of flash
    rxmode = USART_RXMODE_CLK2X_gc;
  #endif

  _written = false;



  //See #131 for more info on this
  #if (CLOCK_SOURCE==0 && PROGMEM_SIZE>4096 && (defined(UARTBAUD3V)||UARTBAUD5V))
  // if the flash is 2k or 4k, we really can't spare the flash for the baud rate correction...
  // it's close enough to work under normal circumstances anyway.
  #if (F_CPU==20000000UL || F_CPU==10000000UL || F_CPU==5000000UL) //this means we are on the 20MHz oscillator
  #ifdef UARTBAUD3V
  int8_t sigrow_val = SIGROW.OSC20ERR3V;
  #else
  int8_t sigrow_val = SIGROW.OSC20ERR5V;
  #endif
  #else //we are on 16MHz one
  #ifdef UARTBAUD3V
  int8_t sigrow_val = SIGROW.OSC16ERR3V;
  #else
  int8_t sigrow_val = SIGROW.OSC16ERR5V;
  #endif
  #endif
  #if (F_CPU > 2000000)
  //if we are above 2 MHz, baud was corrected above if CLK2X used.
  baud_setting = ((8 * F_CPU) / baud);
  #else
  //if clocked at 1 or 2 MHz, always use CLK2X mode and save a bit of flash...
  baud_setting = ((16 * F_CPU) / baud);
  #endif
  baud_setting *= (1024 + sigrow_val);
  baud_setting /= 2048;
  if (baud_setting>65535){
    baud_setting=65535;
  }
  #else
  #if (F_CPU > 2000000)
  //if we are above 2 MHz, baud was corrected above if CLK2X used.
  baud_setting = (((4 * F_CPU) / baud));
  #else
  //if clocked at 1 or 2 MHz, always use CLK2X mode and save a bit of flash...
  baud_setting = (((8 * F_CPU) / baud));
  #endif
  #endif

  // Make sure global interrupts are disabled during initialization
  // no reason to do this before we potentially do all that long division, right?
  uint8_t oldSREG = SREG;
  cli();
  // assign the baud_setting, a.k.a. BAUD (USART Baud Rate Register)
  (*_hwserial_module).BAUD = (uint16_t)baud_setting;

  // Set USART mode of operation
  (*_hwserial_module).CTRLC = config;

  // Enable transmitter and receiver
  (*_hwserial_module).CTRLB = ((*_hwserial_module).CTRLB&(~USART_RXMODE_gm)) | rxmode | (USART_RXEN_bm | USART_TXEN_bm);

  (*_hwserial_module).CTRLA |= USART_RXCIE_bm;


  // Let PORTMUX point to alternative UART pins as requested
  #ifdef PORTMUX_CTRLB
  PORTMUX.CTRLB = set->mux | (PORTMUX.CTRLB & ~_hw_set[1].mux);
  #else
  PORTMUX.USARTROUTEA = set->mux | (PORTMUX.USARTROUTEA & ~_hw_set[1].mux);
  #endif

  // Set pin state for swapped UART pins
  pinMode(set->rx_pin, INPUT_PULLUP);
  //digitalWrite(set->tx_pin, HIGH);
  pinMode(set->tx_pin, OUTPUT);

  // Restore SREG content
  SREG = oldSREG;
}

void UartClass::end() {
  // wait for transmission of outgoing data
  flush();

  // Disable receiver and transmitter as well as the RX complete and
  // data register empty interrupts.
  (*_hwserial_module).CTRLB &= ~(USART_RXEN_bm | USART_TXEN_bm);
  (*_hwserial_module).CTRLA &= ~(USART_RXCIE_bm | USART_DREIE_bm);

  // clear any received data
  _rx_buffer_head = _rx_buffer_tail;

  // Note: Does not change output pins
  _written = false;
}

int UartClass::available(void) {
  return ((unsigned int)(SERIAL_RX_BUFFER_SIZE + _rx_buffer_head - _rx_buffer_tail)) & (SERIAL_RX_BUFFER_SIZE-1); //% SERIAL_RX_BUFFER_SIZE;
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
    _rx_buffer_tail = (rx_buffer_index_t)(_rx_buffer_tail + 1) & (SERIAL_RX_BUFFER_SIZE-1); // % SERIAL_RX_BUFFER_SIZE;
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

  //Check if we are inside an ISR already (e.g. connected to a different peripheral then UART), in which case the UART ISRs will not be called.
  // Spence 10/23/20: Changed _poll_tx_data_empty() to instead call the ISR directly in this case too
  // Why elevate the interrupt if we're going to go into a busywait loop checking if the interrupt is disabled and if so, check for the bit and
  // manually call the ISR if the bit is set... *anyway*? Plus, in write(), this mode will be enabled upon a write of a single character from an ISR
  // and will stay that way until the buffer is empty, which would mean that the fairly long and slow UART TX ISR would have priority over a
  // potentially very fast interrupt that the user may have set to priority level 1. Just because a whizz-bang feature is there doesn't mean
  // it's appropriate to use for applications where it has only very small benefits, and significant risk of surprising the user and causing
  // breakage of code that would otherwise work. Finally, the previous implementation didn't check if it was called from the current lvl1 ISR
  // and in that case flush(), and write() with full buffer would just straight up hang...
  //
  //Temporarily elevate the DRE interrupt to allow it to run.
  //if (CPUINT.STATUS & CPUINT_LVL0EX_bm) {
    //Elevate the priority level of the Data Register Empty Interrupt vector
    //and copy whatever vector number that might be in the register already.
  //  _prev_lvl1_interrupt_vect = CPUINT.LVL1VEC;
  //  CPUINT.LVL1VEC = _hwserial_dre_interrupt_vect_num;

  //  _hwserial_dre_interrupt_elevated = 1;
  //}

  // Spin until the data-register-empty-interrupt is disabled and TX complete interrupt flag is raised
  while (((*_hwserial_module).CTRLA & USART_DREIE_bm) || (!((*_hwserial_module).STATUS & USART_TXCIF_bm))) {

    // If interrupts are globally disabled or the and DR empty interrupt is disabled,
    // poll the "data register empty" interrupt flag to prevent deadlock
    _poll_tx_data_empty();
  }
  // If we get here, nothing is queued anymore (DREIE is disabled) and
  // the hardware finished transmission (TXCIF is set).
}

void UartClass::printHex (const uint8_t b) {
  char x=(b>>4)|'0';
  if (x > '9')
    x += 7;
  write(x);
  x=(b&0x0F)|'0';
  if (x > '9')
    x += 7;
  write(x);
}
void UartClass::printHex(const uint16_t w, bool swaporder){
  uint8_t * ptr=(uint8_t*)&w;
  if (swaporder){
    printHex(*(ptr++));
    printHex(*(ptr));
  }
  else {
    printHex(*(ptr+1));
    printHex(*(ptr));
  }
}

void UartClass::printHex(const uint32_t l, bool swaporder){
  uint8_t * ptr=(uint8_t*)&l;
  if (swaporder){
    printHex(*(ptr++));
    printHex(*(ptr++));
    printHex(*(ptr++));
    printHex(*(ptr));
  }
  else {
    ptr+=3;
    printHex(*(ptr--));
    printHex(*(ptr--));
    printHex(*(ptr--));
    printHex(*(ptr));
  }
}
uint8_t * UartClass::printHex(uint8_t* p,uint8_t len, char sep) {
  for (byte i=0;i<len;i++) {
    if (sep && i) write(sep);
    printHex(*p++);
  }
  return p;
}

uint16_t * UartClass::printHex(uint16_t* p, uint8_t len, char sep, bool swaporder) {
  for (byte i=0;i<len;i++) {
    if (sep && i) write(sep);
    printHex(*p++,swaporder);
  }
  return p;
}
size_t UartClass::write(uint8_t c) {
  _written = true;

  // If the buffer and the data register is empty, just write the byte
  // to the data register and be done. This shortcut helps
  // significantly improve the effective data rate at high (>
  // 500kbit/s) bit rates, where interrupt overhead becomes a slowdown.
  if ((_tx_buffer_head == _tx_buffer_tail) && ((*_hwserial_module).STATUS & USART_DREIF_bm)) {
    (*_hwserial_module).TXDATAL = c;
    (*_hwserial_module).STATUS = USART_TXCIF_bm;

    // Make sure data register empty interrupt is disabled to avoid
    // that the interrupt handler is called in this situation
    (*_hwserial_module).CTRLA &= (~USART_DREIE_bm);

    return 1;
  }
  // See above for reasoning for disabling this functionality and moving the check
  // to _poll_tx_data_empty()
  //Check if we are inside an ISR already (could be from by a source other than UART),
  // in which case the UART ISRs will be blocked.
  //if (CPUINT.STATUS & CPUINT_LVL0EX_bm) {
    //Elevate the priority level of the Data Register Empty Interrupt vector
    //and copy whatever vector number that might be in the register already.
  //  _prev_lvl1_interrupt_vect = CPUINT.LVL1VEC;
  //  CPUINT.LVL1VEC = _hwserial_dre_interrupt_vect_num;

  //  _hwserial_dre_interrupt_elevated = 1;
  //}

  tx_buffer_index_t i = (_tx_buffer_head + 1) & (SERIAL_TX_BUFFER_SIZE-1); // % SERIAL_TX_BUFFER_SIZE;

  //If the output buffer is full, there's nothing for it other than to
  //wait for the interrupt handler to empty it a bit (or emulate interrupts)
  while (i == _tx_buffer_tail) {
    _poll_tx_data_empty();
  }

  _tx_buffer[_tx_buffer_head] = c;
  _tx_buffer_head = i;

  // Enable data "register empty interrupt"
  (*_hwserial_module).CTRLA |= USART_DREIE_bm;

  return 1;
}

#endif // whole file
