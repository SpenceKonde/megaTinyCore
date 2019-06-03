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

void serialEventRun(void)
{
#if defined(HAVE_HWSERIAL0)
    if (Serial0_available && serialEvent && Serial0_available()) serialEvent();
#endif
#if defined(HAVE_HWSERIAL1)
    if (Serial1_available && serialEvent1 && Serial1_available()) serialEvent1();
#endif
#if defined(HAVE_HWSERIAL2)
    if (Serial2_available && serialEvent2 && Serial2_available()) serialEvent2();
#endif
#if defined(HAVE_HWSERIAL3)
    if (Serial3_available && serialEvent3 && Serial3_available()) serialEvent3();
#endif
}

// macro to guard critical sections when needed for large TX buffer sizes
#if (SERIAL_TX_BUFFER_SIZE > 256)
#define TX_BUFFER_ATOMIC ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
#else
#define TX_BUFFER_ATOMIC
#endif

// Actual interrupt handlers //////////////////////////////////////////////////////////////

void UartClass::_tx_data_empty_irq(void)
{
    // Check if tx buffer already empty.
    // This interrupt-handler can be called "manually" from flush();
    if (_tx_buffer_head == _tx_buffer_tail) {
        // Buffer empty, so disable "data register empty" interrupt
        (*_hwserial_module).CTRLA &= (~USART_DREIE_bm);
        return;
    }

    // There must be more data in the output
    // buffer. Send the next byte
    unsigned char c = _tx_buffer[_tx_buffer_tail];
    _tx_buffer_tail = (_tx_buffer_tail + 1) % SERIAL_TX_BUFFER_SIZE;

    // clear the TXCIF flag -- "can be cleared by writing a one to its bit
    // location". This makes sure flush() won't return until the bytes
    // actually got written
    (*_hwserial_module).STATUS = USART_TXCIF_bm;

    (*_hwserial_module).TXDATAL = c;

    while(!((*_hwserial_module).STATUS & USART_DREIF_bm));

    if (_tx_buffer_head == _tx_buffer_tail) {
        // Buffer empty, so disable "data register empty" interrupt
        (*_hwserial_module).CTRLA &= (~USART_DREIE_bm);

        //Take the DRE interrupt back no normal priority level if it has been elevated
        if(_hwserial_dre_interrupt_elevated) {
            CPUINT.LVL1VEC = _prev_lvl1_interrupt_vect;
            _hwserial_dre_interrupt_elevated = 0;
        }
    }
}

// Public Methods //////////////////////////////////////////////////////////////

void UartClass::begin(unsigned long baud, uint16_t config)
{
    // Make sure no transmissions are ongoing and USART is disabled in case begin() is called by accident
    // without first calling end()
    if(_written) {
        this->end();
    }

    // Setup port mux
    PORTMUX.USARTROUTEA |= _uart_mux;

    int32_t baud_setting = 0;

    //Make sure global interrupts are disabled during initialization
    uint8_t oldSREG = SREG;
    cli();

    baud_setting = (((8 * F_CPU_CORRECTED) / baud) + 1) / 2;
    // Disable CLK2X
    (*_hwserial_module).CTRLB &= (~USART_RXMODE_CLK2X_gc);
    (*_hwserial_module).CTRLB |= USART_RXMODE_NORMAL_gc;

    _written = false;

    //Set up the rx pin
    pinMode(_hwserial_rx_pin, INPUT_PULLUP);

    //Set up the tx pin
    digitalWrite(_hwserial_tx_pin, HIGH);
    pinMode(_hwserial_tx_pin, OUTPUT);

    int8_t sigrow_val = SIGROW.OSC16ERR5V;
    baud_setting *= (1024 + sigrow_val);
    baud_setting /= (1024 - abs(sigrow_val));

    // assign the baud_setting, a.k.a. BAUD (USART Baud Rate Register)
    (*_hwserial_module).BAUD = (int16_t) baud_setting;

    // Set USART mode of operation
    (*_hwserial_module).CTRLC = config;

    // Enable transmitter and receiver
    (*_hwserial_module).CTRLB |= (USART_RXEN_bm | USART_TXEN_bm);

    (*_hwserial_module).CTRLA |= USART_RXCIE_bm;

    // Restore SREG content
    SREG = oldSREG;
}

void UartClass::end()
{
    // wait for transmission of outgoing data
    flush();

    // Disable receiver and transmitter as well as the RX complete and
    // data register empty interrupts.
    (*_hwserial_module).CTRLB &= ~(USART_RXEN_bm | USART_TXEN_bm);
    (*_hwserial_module).CTRLA &= ~(USART_RXCIE_bm | USART_DREIE_bm);

    // clear any received data
    _rx_buffer_head = _rx_buffer_tail;

    _written = false;
}

int UartClass::available(void)
{
    return ((unsigned int)(SERIAL_RX_BUFFER_SIZE + _rx_buffer_head - _rx_buffer_tail)) % SERIAL_RX_BUFFER_SIZE;
}

int UartClass::peek(void)
{
    if (_rx_buffer_head == _rx_buffer_tail) {
        return -1;
    } else {
        return _rx_buffer[_rx_buffer_tail];
    }
}

int UartClass::read(void)
{
    // if the head isn't ahead of the tail, we don't have any characters
    if (_rx_buffer_head == _rx_buffer_tail) {
        return -1;
    } else {
        unsigned char c = _rx_buffer[_rx_buffer_tail];
        _rx_buffer_tail = (rx_buffer_index_t)(_rx_buffer_tail + 1) % SERIAL_RX_BUFFER_SIZE;
        return c;
    }
}

int UartClass::availableForWrite(void)
{
    tx_buffer_index_t head;
    tx_buffer_index_t tail;

    TX_BUFFER_ATOMIC {
        head = _tx_buffer_head;
        tail = _tx_buffer_tail;
    }
    if (head >= tail) return SERIAL_TX_BUFFER_SIZE - 1 - head + tail;
    return tail - head - 1;
}

void UartClass::flush()
{
    // If we have never written a byte, no need to flush. This special
    // case is needed since there is no way to force the TXCIF (transmit
    // complete) bit to 1 during initialization
    if (!_written) {
        return;
    }

    //Check if we are inside an ISR already (e.g. connected to a different peripheral then UART), in which case the UART ISRs will not be called.
    //Temporarily elevate the DRE interrupt to allow it to run.
    if(CPUINT.STATUS & CPUINT_LVL0EX_bm) {
        //Elevate the priority level of the Data Register Empty Interrupt vector
        //and copy whatever vector number that might be in the register already.
        _prev_lvl1_interrupt_vect = CPUINT.LVL1VEC;
        CPUINT.LVL1VEC = _hwserial_dre_interrupt_vect_num;

        _hwserial_dre_interrupt_elevated = 1;
    }

    // Spin until the data-register-empty-interrupt is disabled and TX complete interrupt flag is raised
    while ( ((*_hwserial_module).CTRLA & USART_DREIE_bm) || (!((*_hwserial_module).STATUS & USART_TXCIF_bm)) ) {

        // If interrupts are globally disabled or the and DR empty interrupt is disabled,
        // poll the "data register empty" interrupt flag to prevent deadlock
        if ( (!(SREG & CPU_I_bm)) || (!((*_hwserial_module).CTRLA & USART_DREIE_bm)) ) {
            _tx_data_empty_irq();
        }
    }
    // If we get here, nothing is queued anymore (DREIE is disabled) and
    // the hardware finished transmission (TXCIF is set).
}

size_t UartClass::write(uint8_t c)
{
    _written = true;

    // If the buffer and the data register is empty, just write the byte
    // to the data register and be done. This shortcut helps
    // significantly improve the effective data rate at high (>
    // 500kbit/s) bit rates, where interrupt overhead becomes a slowdown.
    if ( (_tx_buffer_head == _tx_buffer_tail) && ((*_hwserial_module).STATUS & USART_DREIF_bm) ) {
        (*_hwserial_module).TXDATAL = c;
        (*_hwserial_module).STATUS = USART_TXCIF_bm;

        // Make sure data register empty interrupt is disabled to avoid
        // that the interrupt handler is called in this situation
        (*_hwserial_module).CTRLA &= (~USART_DREIE_bm);

        return 1;
    }

    //Check if we are inside an ISR already (could be from by a source other than UART),
    // in which case the UART ISRs will be blocked.
    if(CPUINT.STATUS & CPUINT_LVL0EX_bm) {
        //Elevate the priority level of the Data Register Empty Interrupt vector
        //and copy whatever vector number that might be in the register already.
        _prev_lvl1_interrupt_vect = CPUINT.LVL1VEC;
        CPUINT.LVL1VEC = _hwserial_dre_interrupt_vect_num;

        _hwserial_dre_interrupt_elevated = 1;
    }

    tx_buffer_index_t i = (_tx_buffer_head + 1) % SERIAL_TX_BUFFER_SIZE;

    //If the output buffer is full, there's nothing for it other than to
    //wait for the interrupt handler to empty it a bit
    while (i == _tx_buffer_tail) {
        if ( ( !(SREG & CPU_I_bm) ) || ( !((*_hwserial_module).CTRLA & USART_DREIE_bm) ) ) {
            // Interrupts are disabled either globally or for data register empty,
            // so we'll have to poll the "data register empty" flag ourselves.
            // If it is set, pretend an interrupt has happened and call the handler
            //to free up space for us.

            _tx_data_empty_irq();
        } else {
            // nop, the interrupt handler will free up space for us
        }
    }

    _tx_buffer[_tx_buffer_head] = c;
    _tx_buffer_head = i;

    // Enable data "register empty interrupt"
    (*_hwserial_module).CTRLA |= USART_DREIE_bm;

    return 1;
}

#endif // whole file
