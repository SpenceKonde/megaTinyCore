/* UART_check_pins.h - Validate pins_arduino from the variant.
 *
 * Written by Spence Konde in late 2021.
 *
 * This file is part of DxCore, which is free software (LGPL 2.1) based on the work
 * of countless members of the Arduino community.
 * See License.md for more information
 *
 * This is a mixture of stuff that was formerly spread out, and novel code to generate a _usart_pins variable to
 * hold required pin identifiers, mux values, and so on, as well as moving things that we bloody well know
 * out of variants. It's not like the DRE vector for hardware serial 1 is ever going to be something other than
 * USART1_DRE_vect or something, so why do we act like it could be?
 */

#pragma once

/* Recreate all those old defines so they aren't spread over all the variants when they don't vary!
 * The core does NOT use these, but other code might, so we should make sure they're here.
 * We also pick the default as the default if it's not defined.
 */

#if defined(USART0) && !defined(HWSERIAL0_MUX_DEFAULT)
  #define HWSERIAL0_MUX_DEFAULT (0)
#endif

#if defined(USART1) && !defined(HWSERIAL1_MUX_DEFAULT)
  #define HWSERIAL1_MUX_DEFAULT (0)
#endif

#if defined(USART1)
  #define NUM_HWSERIAL_PORTS 2
#elif defined(USART0)
  #define NUM_HWSERIAL_PORTS 1
#else
  #error "No USARTs present. All supported parts have at least 1 - something is wrong!"
#endif

#if defined(USART0)
  #define HAVE_HWSERIAL0                  1
  #define HWSERIAL0_DRE_VECTOR            USART0_DRE_vect
  #define HWSERIAL0_DRE_VECTOR_NUM        USART0_DRE_vect_num
  #define HWSERIAL0_RXC_VECTOR            USART0_RXC_vect
  #define HWSERIAL0                       &USART0
#endif
#if defined(USART1)
  #define HAVE_HWSERIAL1                  1
  #define HWSERIAL1_DRE_VECTOR            USART1_DRE_vect
  #define HWSERIAL1_DRE_VECTOR_NUM        USART1_DRE_vect_num
  #define HWSERIAL1_RXC_VECTOR            USART1_RXC_vect
  #define HWSERIAL1                       &USART1
#endif


#include "Arduino.h"
/* Make sure the variant doesn't define non-existent pins as belonging to serial ports. */
#if defined(PIN_HWSERIAL0_TX) && (PIN_HWSERIAL0_TX >= NUM_TOTAL_PINS) && (PIN_HWSERIAL0_TX != NOT_A_PIN)
  #error "Variant file is not self-consistent - PIN_HWSERIAL0_TX is defined as a pin which does not exist, but not NOT_A_PIN"
#endif
#if defined(PIN_HWSERIAL0_RX) && (PIN_HWSERIAL0_RX >= NUM_TOTAL_PINS) && (PIN_HWSERIAL0_RX != NOT_A_PIN)
  #error "Variant file is not self-consistent - PIN_HWSERIAL0_RX is defined as a pin which does not exist, but not NOT_A_PIN"
#endif
#if defined(PIN_HWSERIAL0_XCK) && (PIN_HWSERIAL0_XCK >= NUM_TOTAL_PINS) && (PIN_HWSERIAL0_XCK != NOT_A_PIN)
  #error "Variant file is not self-consistent - PIN_HWSERIAL0_XCK is defined as a pin which does not exist, but not NOT_A_PIN"
#endif
#if defined(PIN_HWSERIAL0_XDIR) && (PIN_HWSERIAL0_XDIR >= NUM_TOTAL_PINS) && (PIN_HWSERIAL0_XDIR != NOT_A_PIN)
  #error "Variant file is not self-consistent - PIN_HWSERIAL0_XDIR is defined as a pin which does not exist, but not NOT_A_PIN"
#endif
#if defined(PIN_HWSERIAL0_TX_PINSWAP_1) && (PIN_HWSERIAL0_TX_PINSWAP_1 >= NUM_TOTAL_PINS) && (PIN_HWSERIAL0_TX_PINSWAP_1 != NOT_A_PIN)
  #error "Variant file is not self-consistent - PIN_HWSERIAL0_TX_PINSWAP_1 is defined as a pin which does not exist, but not NOT_A_PIN"
#endif
#if defined(PIN_HWSERIAL0_RX_PINSWAP_1) && (PIN_HWSERIAL0_RX_PINSWAP_1 >= NUM_TOTAL_PINS) && (PIN_HWSERIAL0_RX_PINSWAP_1 != NOT_A_PIN)
  #error "Variant file is not self-consistent - PIN_HWSERIAL0_RX_PINSWAP_1 is defined as a pin which does not exist, but not NOT_A_PIN"
#endif
#if defined(PIN_HWSERIAL0_XCK_PINSWAP_1) && (PIN_HWSERIAL0_XCK_PINSWAP_1 >= NUM_TOTAL_PINS) && (PIN_HWSERIAL0_XCK_PINSWAP_1 != NOT_A_PIN)
  #error "Variant file is not self-consistent - PIN_HWSERIAL0_XCK_PINSWAP_1 is defined as a pin which does not exist, but not NOT_A_PIN"
#endif
#if defined(PIN_HWSERIAL0_XDIR_PINSWAP_1) && (PIN_HWSERIAL0_XDIR_PINSWAP_1 >= NUM_TOTAL_PINS) && (PIN_HWSERIAL0_XDIR_PINSWAP_1 != NOT_A_PIN)
  #error "Variant file is not self-consistent - PIN_HWSERIAL0_XDIR_PINSWAP_1 is defined as a pin which does not exist, but not NOT_A_PIN"
#endif
#if defined(PIN_HWSERIAL1_TX) && (PIN_HWSERIAL1_TX >= NUM_TOTAL_PINS) && (PIN_HWSERIAL1_TX != NOT_A_PIN)
  #error "Variant file is not self-consistent - PIN_HWSERIAL1_TX is defined as a pin which does not exist, but not NOT_A_PIN"
#endif
#if defined(PIN_HWSERIAL1_RX) && (PIN_HWSERIAL1_RX >= NUM_TOTAL_PINS) && (PIN_HWSERIAL1_RX != NOT_A_PIN)
  #error "Variant file is not self-consistent - PIN_HWSERIAL1_RX is defined as a pin which does not exist, but not NOT_A_PIN"
#endif
#if defined(PIN_HWSERIAL1_XCK) && (PIN_HWSERIAL1_XCK >= NUM_TOTAL_PINS) && (PIN_HWSERIAL1_XCK != NOT_A_PIN)
  #error "Variant file is not self-consistent - PIN_HWSERIAL1_XCK is defined as a pin which does not exist, but not NOT_A_PIN"
#endif
#if defined(PIN_HWSERIAL1_XDIR) && (PIN_HWSERIAL1_XDIR >= NUM_TOTAL_PINS) && (PIN_HWSERIAL1_XDIR != NOT_A_PIN)
  #error "Variant file is not self-consistent - PIN_HWSERIAL1_XDIR is defined as a pin which does not exist, but not NOT_A_PIN"
#endif
#if defined(PIN_HWSERIAL1_TX_PINSWAP_1) && (PIN_HWSERIAL1_TX_PINSWAP_1 >= NUM_TOTAL_PINS) && (PIN_HWSERIAL1_TX_PINSWAP_1 != NOT_A_PIN)
  #error "Variant file is not self-consistent - PIN_HWSERIAL1_TX_PINSWAP_1 is defined as a pin which does not exist, but not NOT_A_PIN"
#endif
#if defined(PIN_HWSERIAL1_RX_PINSWAP_1) && (PIN_HWSERIAL1_RX_PINSWAP_1 >= NUM_TOTAL_PINS) && (PIN_HWSERIAL1_RX_PINSWAP_1 != NOT_A_PIN)
  #error "Variant file is not self-consistent - PIN_HWSERIAL1_RX_PINSWAP_1 is defined as a pin which does not exist, but not NOT_A_PIN"
#endif
#if defined(PIN_HWSERIAL1_XCK_PINSWAP_1) && (PIN_HWSERIAL1_XCK_PINSWAP_1 >= NUM_TOTAL_PINS) && (PIN_HWSERIAL1_XCK_PINSWAP_1 != NOT_A_PIN)
  #error "Variant file is not self-consistent - PIN_HWSERIAL1_XCK_PINSWAP_1 is defined as a pin which does not exist, but not NOT_A_PIN"
#endif
#if defined(PIN_HWSERIAL1_XDIR_PINSWAP_1) && (PIN_HWSERIAL1_XDIR_PINSWAP_1 >= NUM_TOTAL_PINS) && (PIN_HWSERIAL1_XDIR_PINSWAP_1 != NOT_A_PIN)
  #error "Variant file is not self-consistent - PIN_HWSERIAL1_XDIR_PINSWAP_1 is defined as a pin which does not exist, but not NOT_A_PIN"
#endif
#if (defined(USART1) && !((PIN_HWSERIAL1_TX != NOT_A_PIN && PIN_HWSERIAL1_RX != NOT_A_PIN)))
  // if USART1 is defined, and default pins aren't NOT_A_PIN they should be the same as USART0's alternates
  #if (((PIN_HWSERIAL1_TX    != PIN_HWSERIAL0_TX_PINSWAP_1)    || (PIN_HWSERIAL1_RX   != PIN_HWSERIAL0_RX_PINSWAP_1)) || \
       ((PIN_HWSERIAL1_XCK   != PIN_HWSERIAL0_XCK_PINSWAP_1)   && (PIN_HWSERIAL1_XCK  != NOT_A_PIN)) || \
       ((PIN_HWSERIAL1_XDIR  != PIN_HWSERIAL0_XDIR_PINSWAP_1)  && (PIN_HWSERIAL1_XDIR != NOT_A_PIN)))
    #error "The variant file is incorrect. On all supported parts, the default USART1 pinset is identical to the alternate USART0 one."
  #endif
#elif (defined(USART1) && ((PIN_HWSERIAL1_TX == NOT_A_PIN || PIN_HWSERIAL1_RX == NOT_A_PIN)))
  // if USART1 is defined and the pins are NOT_A_PIN, then the variant is wrong, because there are no parts like that.
  #error "The variant file is incorrect. On all supported parts w/USART1, the default TX and RX exist, variant says they're NOT_A_PIN"
#endif
