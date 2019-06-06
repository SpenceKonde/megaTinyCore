/*
   updi_io_soft.cpp

   Created: 11-08-2018 22:08:14
    Author: Cristian Balint <cristian dot balint at gmail dot com>
*/

#ifdef __AVR_ATmega16__

// Includes
#include <avr/io.h>
#include <util/delay.h>

#include "sys.h"
#include "updi_io.h"

// Defines
#ifndef F_CPU
#  define F_CPU 16000000U
#endif
#ifndef UPDI_BAUD
#  define UPDI_BAUD 225000U  // (max 225000 min approx. F_CPU/100)
#endif

// Cycle timing
#define TXDELAY (uint8_t)(((F_CPU/UPDI_BAUD) - 9) / 3)
#define RXDELAY (uint8_t)(((F_CPU/UPDI_BAUD) - 9) / 3)
#define RXHALFD (uint8_t)  (RXDELAY / 2)

// Check
#if ( (((F_CPU/UPDI_BAUD) - 9) / 3) > 254 )
# error Low baud rates are not supported - use higher UPDI_BAUD
#endif

// Functions
/* Sends regular characters through the UPDI link */
uint8_t UPDI_io::get() {

  // rx input
  DDR(UPDI_PORT)  &= ~(1 << UPDI_PIN);
  // no pullup
  PORT(UPDI_PORT) &= ~(1 << UPDI_PIN);

  uint8_t c;

  __asm volatile
  (
    " ldi  %0, 0 \n\t"           // init
    " ldi r20, 8 \n\t"           // 8 bits
    " ldi r19, 3 \n\t"           // 1 parity + 2 stop bits

    // wait for start edge
    "WaitStart: \n\t"
    " sbic %[uart_port], %[uart_pin] \n\t"
    " rjmp WaitStart \n\t"

    // skew into middle of edge
    " ldi r18, %[rxhalfd] \n\t"  // 0.5 bit cycle delay
    "HBitDelay: \n\t"
    " dec r18 \n\t"
    " brne HBitDelay \n\t"

    // 8 bits
    "RxBLoop: \n\t"
    " ldi r18, %[rxdelay] \n\t"  // 1 bit cycle delay
    "RxBDelay: \n\t"
    " dec r18 \n\t"
    " brne RxBDelay \n\t"
    " in r21, %[uart_port] \n\t" // get current bit from serial link
    " bst r21, %[uart_pin] \n\t" // use T flag
    " bld r22, 0\n\t"            // to move current data bit
    " lsr r22 \n\t"              // into carry
    " ror %0 \n\t"               // accumulate serial data bits into result
    " dec r20 \n\t"
    " brne RxBLoop \n\t"
    " nop \n\t"

    // 1 parity + 2 stop bits
    "RxSLoop: \n\t"
    " ldi r18, %[rxdelay] \n\t"  // 1 bit cycle delay
    "RxSDelay: \n\t"
    " dec r18 \n\t"
    " brne RxSDelay \n\t"
    " in r21, %[uart_port] \n\t" // get current bit from serial link
    " bst r21, %[uart_pin] \n\t" // use T flag
    " bld r22, 0\n\t"            // to move current data bit
    " lsr r22 \n\t"              // into carry
    " nop \n\t"                  // accumulate serial data bits into result
    " dec r19 \n\t"
    " brne RxSLoop \n\t"
    " nop \n\t"

    : "=r" (c)
    : [uart_port] "i" (_SFR_IO_ADDR(PIN(UPDI_PORT))),
    [uart_pin]  "i" (UPDI_PIN),
    [rxdelay]   "i" (RXDELAY),
    [rxhalfd]   "i" (RXHALFD)
    : "r0", "r18", "r19", "r20", "r21", "r22"
  );

  // re-enable pull up
  PORT(UPDI_PORT) |= (1 << UPDI_PIN);

  return c;
}

uint8_t UPDI_io::put(char c) {

  // tx enable
  DDR(UPDI_PORT) |= (1 << UPDI_PIN);

  __asm volatile
  (
    " in r0, %[uart_port] \n\t"  // port state
    " ldi r26, 2 \n\t"           // 2 bit stop
    " ldi r27, 8 \n\t"           // 8 bit parity
    " ldi r28, %[txdelay] \n\t"  // delay counter
    " ldi r30, 8 \n\t"           // 8 bit loop

    // pre delay
    // 2 bit time
    " mov r29, r28 \n\t"
    "TxDelay: \n\t"
    " nop \n\t"
    " dec r29 \n\t"
    " brne TxDelay \n\t"

    // start bit
    " mov r29, r28 \n\t"
    "TxDelayS: \n\t"
    " dec r29 \n\t"
    " brne TxDelayS \n\t"
    " bclr 6 \n\t"
    " bld r0, %[uart_pin] \n\t"
    " nop \n\t"
    " nop \n\t"
    " nop \n\t"
    " out %[uart_port], r0 \n\t"
    " nop \n\t"
    " nop \n\t"

    // 8 bits
    "TxLoop: \n\t"
    " mov r29, r28 \n\t"         // load delay counter
    "TxDelayB: \n\t"             // delay (3 cycle * delayCount) - 1
    " dec r29 \n\t"
    " brne TxDelayB \n\t"
    " bst %[ch], 0 \n\t"         // load bit in T
    " bld r0, %[uart_pin] \n\t"  // store T bit in r0
    " ror %[ch] \n\t"            // shift right into carry
    " sbci r27, 0 \n\t"          // subtract carry (accumulate parity)
    " dec r30 \n\t"              // decrement bits counter
    " out %[uart_port], r0 \n\t" // send bit out
    " brne TxLoop \n\t"          // loop for each bit
    " nop \n\t"

    // parity bit
    " mov r29, r28 \n\t"
    "TxDelayP: \n\t"
    " dec r29 \n\t"
    " brne TxDelayP \n\t"
    " bst r27, 0 \n\t"           // extract accumulated parity
    " bld r0, %[uart_pin] \n\t"
    " nop \n\t"
    " nop \n\t"
    " nop \n\t"
    " out %[uart_port], r0 \n\t" // send bit out to serial link
    " nop \n\t"
    " nop \n\t"

    // stop bits
    "StopLoop: \n\t"
    " mov r29, r28 \n\t"
    "TxDelayStop: \n\t"
    " dec r29 \n\t"
    " brne TxDelayStop \n\t"
    " bset 6 \n\t"
    " bld r0, %[uart_pin] \n\t"
    " nop \n\t"
    " nop \n\t"
    " dec r26 \n\t"
    " out %[uart_port], r0 \n\t" // send bit out to serial link
    " brne StopLoop \n\t"        // loop for each bit
    " nop \n\t"

    :
    : [uart_port] "i" (_SFR_IO_ADDR(PORT(UPDI_PORT))),
    [uart_pin]  "i" (UPDI_PIN),
    [txdelay]   "i" (TXDELAY),
    [ch]        "r" (c)
    : "r0", "r26", "r27", "r28", "r29", "r30"
  );

  // Ready for RX input
  DDR(UPDI_PORT) &= ~(1 << UPDI_PIN);

  return c;
}

static inline void send_break() {

  // tx enable
  DDR(UPDI_PORT) |= (1 << UPDI_PIN);

  //
  // 13 cycles = 24.60ms
  //

  // low 12 cycle
  PORT(UPDI_PORT) &= ~(1 << UPDI_PIN);
  _delay_us(2048 * 11);

  // high 1 cycle
  PORT(UPDI_PORT) |=  (1 << UPDI_PIN);
  _delay_us(2048);

  // RX enable
  DDR(UPDI_PORT) &= ~(1 << UPDI_PIN);

  return;
}

/* Sends special sequences through the UPDI link */
uint8_t UPDI_io::put(ctrl c) {

  switch (c) {

    case double_break:
      send_break();
      send_break();
      break;

    case single_break:
      send_break();
      break;

    case enable:

    default:
      break;
  }

  return 0;
}

void UPDI_io::init(void) {

}

#endif //__AVR_ATmega16__
