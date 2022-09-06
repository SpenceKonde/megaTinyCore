/* UART_private.h - Constructor for HardwareSerial
 * This library is free software released under LGPL 2.1.
 * See License.md for more information.
 * This file is part of megaTinyCore
 *
 * Totally rewritten 2021-22 by Spence Konde and MX682X.
 * The original version was written in 2006 by Nicholas
 * Zambetti, and modified 11/23/2006 by David A. Mellis,
 * 9/20/2010 by Mark Sproul, 8/24/2012 by Alarus, and
 * other members of the Arduino community who did not sign.
 */

#include "wiring_private.h"

#if defined(USART0) || defined(USART1)
/* Significant changes in UART.cpp, UART.h, and UART_swap.h required to support more UARTs */

HardwareSerial::HardwareSerial(volatile USART_t *hwserial_module, uint8_t module_number, uint8_t mux_default) :
    _hwserial_module(hwserial_module), _module_number(module_number), _pin_set(mux_default) {
}

#endif  // whole file
