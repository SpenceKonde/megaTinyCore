/*
  pins_arduino.h - Pin definition functions for Arduino
  Part of Arduino - http://www.arduino.cc/

  Copyright (c) 2007 David A. Mellis

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General
  Public License along with this library; if not, write to the
  Free Software Foundation, Inc., 59 Temple Place, Suite 330,
  Boston, MA  02111-1307  USA
*/

#ifndef Pins_Arduino_h
#define Pins_Arduino_h

#include <avr/pgmspace.h>
#include "timers.h"

#define NUM_DIGITAL_PINS                  (6) //
#define NUM_ANALOG_INPUTS                 (9)
#define NUM_I2C_PINS                      (2) // (SDA / SCL)
#define NUM_SPI_PINS                      (3) // (MISO / MOSI / SCK)
#define NUM_TOTAL_FREE_PINS               (NUM_DIGITAL_PINS)
#define NUM_TOTAL_PINS                    (6)
#define EXTERNAL_NUM_INTERRUPTS           (8)
#define PINS_COUNT                        (6)

#define PIN_PA6   (0)
#define PIN_PA7   (1)
#define PIN_PA1   (2)
#define PIN_PA2   (3)
#define PIN_PA3   (4)
#define PIN_PA0   (5)

#if !defined(USE_TIMERD0_PWM) && !defined(TCA_BUFFERED_3PIN)
  #define digitalPinHasPWM(p)            (((uint8_t)((p) - 1) < (uint8_t) 4 ) //PA7, PA1. PA2. PA3.
#elif !defined(USE_TIMERD0_PWM) && defined(TCA_BUFFERED_3PIN)
  #define digitalPinHasPWM(p)            (((uint8_t)((p) - 2) < (uint8_t) 3 ) //PA1. PA2. PA3.
#elif defined(USE_TIMERD0_PWM)
  #define digitalPinHasPWM(p)            (((uint8_t)(p)) < (uint8_t) 5) /* every pin except the reset pin, whichmakes since, seeing as it barely has output drivers... */
#endif

#if defined(DAC0)
  #if !defined(USE_TIMERD0_PWM)
    #define DAC_PIN                         (PIN_PA6)
  #else
    #pragma message("TCD is enabled for PWM output; DAC output via analogWrite() is disabled in favor of TCD PWM")
    /* If you got a problem with that, do it manually! The DAC is about as easy to confgiure as it gets!*/
  #endif
#endif

#ifndef LED_BUILTIN
  #define LED_BUILTIN    (PIN_PA3)
#endif

/*
      ####   ###  ####  ##### #   # #   # #   #
      #   # #   # #   #   #   ## ## #   #  # #
      ####  #   # ####    #   # # # #   #   #
      #     #   # # #     #   #   # #   #  # #
      #      ###  #  #    #   #   #  ###  #   #
*/

#ifdef PORTMUX_SPI0_bm
  #define PIN_SPI_MISO_PINSWAP_1          (PIN_PA7)
  #define PIN_SPI_SCK_PINSWAP_1           (PIN_PA3)
  #define PIN_SPI_MOSI_PINSWAP_1          (PIN_PA6)
  #define PIN_SPI_SS_PINSWAP_1            (PIN_PA6)
#endif
#define PIN_SPI_MISO                      (PIN_PA2)
#define PIN_SPI_SCK                       (PIN_PA3)
#define PIN_SPI_MOSI                      (PIN_PA1)
#define PIN_SPI_SS                        (PIN_PA6)

#define SPI_INTERFACES_COUNT              (1)

#define PIN_WIRE_SDA                      (PIN_PA1)
#define PIN_WIRE_SCL                      (PIN_PA2)

/* Serial pin values here are not used by the core. They don't get picked up correctly in UART.h where they're needed and I can't figure out why. */
// Serial (USART0)
#define HWSERIAL0_MUX_DEFAULT             (0)
#define PIN_HWSERIAL0_RX                  (PIN_PA7)
#define PIN_HWSERIAL0_TX                  (PIN_PA6)
#define PIN_HWSERIAL0_XCK                 (PIN_PA3)
#define PIN_HWSERIAL0_XDIR                (PIN_PA0)
#define PIN_HWSERIAL0_RX_PINSWAP_1        (PIN_PA2)
#define PIN_HWSERIAL0_TX_PINSWAP_1        (PIN_PA1)
// Alt pins don't include the rarely used functions on 8-pin parts.
#define PIN_HWSERIAL0_XCK_PINSWAP_1       (NOT_A_PIN)
#define PIN_HWSERIAL0_XDIR_PINSWAP_1      (NOT_A_PIN)

/*
       ##  #   #  ##  #     ###   ###      ####  ### #   #  ###
      #  # ##  # #  # #    #   # #         #   #  #  ##  # #
      #### # # # #### #    #   # #  ##     ####   #  # # #  ###
      #  # #  ## #  # #    #   # #   #     #      #  #  ##     #
      #  # #   # #  # ####  ###   ###      #     ### #   #  ###
*/

#define PIN_A0   (A0)
#define PIN_A1   (A1)
#define PIN_A2   (A2)
#define PIN_A3   (A3)
#define PIN_A6   (A6)
#define PIN_A7   (A7)

static const uint8_t A0 = PIN_PA0;
static const uint8_t A1 = PIN_PA1;
static const uint8_t A2 = PIN_PA2;
static const uint8_t A3 = PIN_PA3;
static const uint8_t A6 = PIN_PA6;
static const uint8_t A7 = PIN_PA7;


/*
            ####  ### #   #      ##  ####  ####   ##  #   #  ###
            #   #  #  ##  #     #  # #   # #   # #  #  # #  #
            ####   #  # # #     #### ####  ####  ####   #    ###
            #      #  #  ##     #  # # #   # #   #  #   #       #
            #     ### #   #     #  # #  #  #  #  #  #   #    ###
*/

#ifdef ARDUINO_MAIN

// On the Arduino board, digital pins are also used
// for the analog output (software PWM).  Analog input
// pins are a separate set.

// ATtiny412 / ARDUINO
//                                     _____
//                             VDD   1|*    |20  GND
//             (DAC) (AIN6) PA6  0   2|     |19  4~  PA3 (AIN3)(SCK)(EXTCLK)
//                   (AIN7) PA7  1   3|     |18  5   PA0 (nRESET/UPDI)
// (MOSI)(TXD*)(SDA) (AIN1) PA1  2   4|_____|17  3   PA2 (AIN2)(MISO)(RXD*)(SCL)
//
//

/*
  PIN#   DESC         Pin Name  Other/Sp  ADC0      ADC1      PTC       AC0       AC1       AC2       DAC0      USART0    SPI0      TWI0      TCA(PWM)  TCBn      TCD0      CCL
  0      A2 or DAC    PA6                 AIN6      AIN2      X2/Y2     AINN0     AINP1     AINP0     OUT       RxD                                               WOA
  1      A3           PA7                 AIN7      AIN3      X3/Y3     AINP0     AINP0     AINN0               TxD                           *WO0                WOB       LUT1-OUT
  2      MOSI         PA1                 AIN1                                                                  *TxD      MOSI      SDA       WO1                           LUT0-IN1
  3      MISO         PA2       EVOUT0    AIN2                                                                  *RxD      MISO      SCL       WO2                           LUT0-IN2
  4      SCK          PA3       EXTCLK    AIN3                                                                  *XCK      SCK                 WO3       TCB1 WO
  5      UPDI         PA0       RESET/    AIN0                                                                                                                              LUT1-IN0
                                UPDI
*/

const uint8_t digital_pin_to_port[] = {
  PA,           // 0  PA6
  PA,           // 1  PA7
  PA,           // 2  PA1
  PA,           // 3  PA2
  PA,           // 4  PA3
  PA            // 5  PA0
};

/* Use this for accessing PINnCTRL register */
const uint8_t digital_pin_to_bit_position[] = {
  PIN6_bp,      // 0  PA6
  PIN7_bp,      // 1  PA7
  PIN1_bp,      // 2  PA1
  PIN2_bp,      // 3  PA2
  PIN3_bp,      // 4  PA3
  PIN0_bp       // 5  PA0
};

/* Use this for accessing PINnCTRL register */
const uint8_t digital_pin_to_bit_mask[] = {
  PIN6_bm,      // 0  PA6
  PIN7_bm,      // 1  PA7
  PIN1_bm,      // 2  PA1
  PIN2_bm,      // 3  PA2
  PIN3_bm,      // 4  PA3
  PIN0_bm       // 5  PA0
};
//*INDENT-OFF*
const uint8_t digital_pin_to_timer[] = {
  #if defined(DAC0) && !(defined(USE_TIMERD0_PWM) && defined(TCD0))
    DACOUT,       // 0  PA6
  #elif defined(USE_TIMERD0_PWM) && defined(TCD0)
    TIMERD0,
  #else
    NOT_ON_TIMER, // 0  PA6
  #endif
  #if defined(_TCA_ALT_WO0)
    TIMERA0,    // 1  PA7
  #elif defined(USE_TIMERD0_PWM)
    TIMERD0,
  #endif
  TIMERA0,      // 2  PA1
  TIMERA0,      // 3  PA2
  #if !defined(TCA_BUFFERED_3PIN)
    TIMERA0,      // 4  PA3
  #else
    NOT_ON_TIMER, // 4  PA3
  #endif
  NOT_ON_TIMER  // 5  PA0
};
//*INDENT-ON*


#endif

#define digitalPinToAnalogInput(p)      ((p<2)?(p+6):(p<5?(p-1):(p==5?0:NOT_A_PIN)))


// These serial port names are intended to allow libraries and architecture-neutral
// sketches to automatically default to the correct port name for a particular type
// of use.  For example, a GPS module would normally connect to SERIAL_PORT_HARDWARE_OPEN,
// the first hardware serial port whose RX/TX pins are not dedicated to another use.
//
// SERIAL_PORT_MONITOR        Port which normally prints to the Arduino Serial Monitor
//
// SERIAL_PORT_USBVIRTUAL     Port which is USB virtual serial
//
// SERIAL_PORT_LINUXBRIDGE    Port which connects to a Linux system via Bridge library
//
// SERIAL_PORT_HARDWARE       Hardware serial port, physical RX & TX pins.
//
// SERIAL_PORT_HARDWARE_OPEN  Hardware serial ports which are open for use.  Their RX & TX
//                            pins are NOT connected to anything by default.
#define SERIAL_PORT_MONITOR     Serial
#define SERIAL_PORT_HARDWARE    Serial

#endif
