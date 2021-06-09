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

#if defined(TCD0)
  #define USE_TIMERD0_PWM
  #define NO_GLITCH_TIMERD0
#endif

#if MEGATINYCORE_SERIES == 2
  /* Yes, this is actually one more than we have, but the way this is used by the code means that it actually needs to be (highest mux channel for a pin +1) */
  #define NUM_ANALOG_INPUTS           (16)
#else
  #define NUM_ANALOG_INPUTS           (12)
#endif

#define NUM_I2C_PINS                  (2) // (SDA / SCL)
#define NUM_SPI_PINS                  (3) // (MISO / MOSI / SCK)
#define NUM_TOTAL_PINS                (18)
#define NUM_DIGITAL_PINS              (18)
#define PINS_COUNT                    (18)

#define EXTERNAL_NUM_INTERRUPTS       (20)

#if (defined(TCD0) && defined(USE_TIMERD0_PWM))
  #define digitalPinHasPWM(p)         ((p) == PIN_PA4 || (p) == PIN_PA5 || (p) == PIN_PB2 || (p) == PIN_PB1 || (p) == PIN_PB0 || (p) == PIN_PC0 || (p) == PIN_PC1 || (p) == PIN_PA3)
#else
  #define digitalPinHasPWM(p)         ((p) == PIN_PA4 || (p) == PIN_PA5 || (p) == PIN_PB2 || (p) == PIN_PB1 || (p) == PIN_PB0                                     || (p) == PIN_PA3)
#endif


#ifdef DAC0
  #define DAC_PIN      (PIN_PA6)
#endif

#ifndef LED_BUILTIN
  #define LED_BUILTIN  (PIN_PA7)
#endif


#if MEGATINYCORE_SERIES != 2
  #define digitalPinToAnalogInput(p)  (((p) < 6) ? ((p) + 4) : ((p) == 17 ? 0 : (((p) > 13 && (p) < 17) ? ((p) - 13) : (((p) == 8) ? 10 : ((p) == 9 ? 11 : NOT_A_PIN)))))
#else
  /* 2-series MUX table says ADC channel 0 is tied to ground, not PA0, PC0 through PC3 are a A12-15 */
  #define digitalPinToAnalogInput(p)  (((p) < 6) ? ((p) + 4) : (((p) > 13 && (p) < 17) ? ((p) - 13) : ((((p) >= 8) && ((p) < 14)) ? ((p) + 2) :  NOT_A_PIN)))
#endif

/*
      ####   ###  ####  ##### #   # #   # #   #
      #   # #   # #   #   #   ## ## #   #  # #
      ####  #   # ####    #   # # # #   #   #
      #     #   # # #     #   #   # #   #  # #
      #      ###  #  #    #   #   #  ###  #   #
*/
#define SPI_MUX                       (0)
#define PIN_SPI_SS                    (PIN_PA0)
#define PIN_SPI_MOSI                  (PIN_PA1)
#define PIN_SPI_MISO                  (PIN_PA2)
#define PIN_SPI_SCK                   (PIN_PA3)
#if defined(PORTMUX_SPI0_bm) || defined(PORTMUX_SPIROUTEA)
  #define PIN_SPI_SCK_PINSWAP_1       (PIN_PC0)
  #define PIN_SPI_MISO_PINSWAP_1      (PIN_PC1)
  #define PIN_SPI_MOSI_PINSWAP_1      (PIN_PC2)
  #define PIN_SPI_SS_PINSWAP_1        (PIN_PC3)
  #ifdef PORTMUX_SPI0_bm
    #define SPI_MUX_PINSWAP_1         (PORTMUX_SPI0_bm)
  #else
    #define SPI_MUX_PINSWAP_1         (PORTMUX_SPI0_ALT1_gc)
  #endif
#endif

#define SPI_INTERFACES_COUNT          (1)


#ifdef PORTMUX_TWI0_bm
  #define PIN_WIRE_SDA_PINSWAP_1      (PIN_PA1)
  #define PIN_WIRE_SCL_PINSWAP_1      (PIN_PA2)
#endif
#define PIN_WIRE_SDA                  (PIN_PB1)
#define PIN_WIRE_SCL                  (PIN_PB0)

// Mapped to HWSERIAL0 in Serial library
#define HWSERIAL0                     (&USART0)
#define HWSERIAL0_DRE_VECTOR          (USART0_DRE_vect)
#define HWSERIAL0_DRE_VECTOR_NUM      (USART0_DRE_vect_num)
#define HWSERIAL0_RXC_VECTOR          (USART0_RXC_vect)
#define HWSERIAL0_RXC_VECTOR_NUM      (USART0_RXC_vect_num)
#define HWSERIAL0_TXC_VECTOR          (USART0_TXC_vect)
#define HWSERIAL0_TXC_VECTOR_NUM      (USART0_TXC_vect_num)

#define HWSERIAL0_MUX                 (0x00)
#define PIN_HWSERIAL0_TX              (PIN_PB2)
#define PIN_HWSERIAL0_RX              (PIN_PB3)
#define PIN_HWSERIAL0_XCK             (PIN_PB1)
#define PIN_HWSERIAL0_XDIR            (PIN_PB0)

#define HWSERIAL0_MUX_PINSWAP_1       (0x01)
#define PIN_HWSERIAL0_TX_PINSWAP_1    (PIN_PA1)
#define PIN_HWSERIAL0_RX_PINSWAP_1    (PIN_PA2)
#define PIN_HWSERIAL0_XCK_PINSWAP_1   (PIN_PA3)
#define PIN_HWSERIAL0_XDIR_PINSWAP_1  (PIN_PA4)

#if defined(USART1)
  // Mapped to HWSERIAL1 in Serial library
  #define HWSERIAL1                     (&USART1)
  #define HWSERIAL1_DRE_VECTOR          (USART1_DRE_vect)
  #define HWSERIAL1_DRE_VECTOR_NUM      (USART1_DRE_vect_num)
  #define HWSERIAL1_RXC_VECTOR          (USART1_RXC_vect)
  #define HWSERIAL1_RXC_VECTOR_NUM      (USART1_RXC_vect_num)
  #define HWSERIAL1_TXC_VECTOR          (USART1_TXC_vect)
  #define HWSERIAL1_TXC_VECTOR_NUM      (USART1_TXC_vect_num)
  #define HWSERIAL1_MUX                 (0x00)
  #define PIN_HWSERIAL1_TX              (PIN_PA1)
  #define PIN_HWSERIAL1_RX              (PIN_PA2)
  #define PIN_HWSERIAL1_XCK             (PIN_PA3)
  #define PIN_HWSERIAL1_XDIR            (PIN_PA4)

  #define HWSERIAL1_MUX_PINSWAP_1       (0x01)
  #define PIN_HWSERIAL1_TX_PINSWAP_1    (PIN_PC2)
  #define PIN_HWSERIAL1_RX_PINSWAP_1    (PIN_PC1)
  #define PIN_HWSERIAL1_XCK_PINSWAP_1   (PIN_PC0)
  #define PIN_HWSERIAL1_XDIR_PINSWAP_1  (PIN_PC3)
#endif

/*
       ##  #   #  ##  #     ###   ###      ####  ### #   #  ###
      #  # ##  # #  # #    #   # #         #   #  #  ##  # #
      #### # # # #### #    #   # #  ##     ####   #  # # #  ###
      #  # #  ## #  # #    #   # #   #     #      #  #  ##     #
      #  # #   # #  # ####  ###   ###      #     ### #   #  ###
*/

#define PIN_PA4        (0)
#define PIN_PA5        (1)
#define PIN_PA6        (2)
#define PIN_PA7        (3)
#define PIN_PB5        (4)
#define PIN_PB4        (5)
#define PIN_PB3        (6)
#define PIN_PB2        (7)
#define PIN_PB1        (8)
#define PIN_PB0        (9)
#define PIN_PC0        (10)
#define PIN_PC1        (11)
#define PIN_PC2        (12)
#define PIN_PC3        (13)
#define PIN_PA0        (17)
#define PIN_PA1        (14)
#define PIN_PA2        (15)
#define PIN_PA3        (16)

#if MEGATINYCORE_SERIES != 2
  #define PIN_A0       (A0)
#endif
#define PIN_A1         (A1)
#define PIN_A2         (A2)
#define PIN_A3         (A3)
#define PIN_A4         (A4)
#define PIN_A5         (A5)
#define PIN_A6         (A6)
#define PIN_A7         (A7)
#define PIN_A8         (A8)
#define PIN_A9         (A9)
#define PIN_A10        (A10)
#define PIN_A11        (A11)
#if MEGATINYCORE_SERIES == 2
  #define PIN_A12      (A12)
  #define PIN_A13      (A13)
  #define PIN_A14      (A14)
  #define PIN_A15      (A15)
#endif

#if MEGATINYCORE_SERIES != 2
  static const uint8_t  A0  = PIN_PA0;
#endif
static const uint8_t    A1  = PIN_PA1;
static const uint8_t    A2  = PIN_PA2;
static const uint8_t    A3  = PIN_PA3;
static const uint8_t    A4  = PIN_PA4;
static const uint8_t    A5  = PIN_PA5;
static const uint8_t    A6  = PIN_PA6;
static const uint8_t    A7  = PIN_PA7;
static const uint8_t    A8  = PIN_PB5;
static const uint8_t    A9  = PIN_PB4;
static const uint8_t    A10 = PIN_PB1;
static const uint8_t    A11 = PIN_PB0;
#if MEGATINYCORE_SERIES == 2
  static const uint8_t  A12 = PIN_PC0;
  static const uint8_t  A13 = PIN_PC1;
  static const uint8_t  A14 = PIN_PC2;
  static const uint8_t  A15 = PIN_PC3;
#endif

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

// ATtiny1616 / ARDUINO
//                          _____
//                  VDD   1|*    |20  GND
// (nSS)  (AIN4) PA4  0~  2|     |19  16~ PA3 (AIN3)(SCK)(EXTCLK)
//        (AIN5) PA5  1~  3|     |18  15  PA2 (AIN2)(MISO)
// (DAC)  (AIN6) PA6  2   4|     |17  14  PA1 (AIN1)(MOSI)
//        (AIN7) PA7  3   5|     |16  17  PA0 (AIN0/nRESET/UPDI)
//        (AIN8) PB5  4   6|     |15  13  PC3
//        (AIN9) PB4  5   7|     |14  12  PC2
// (RXD) (TOSC1) PB3  6   8|     |13  11~ PC1 (PWM only on 1-series)
// (TXD) (TOSC2) PB2  7~  9|     |12  10~ PC0 (PWM only on 1-series)
// (SDA) (AIN10) PB1  8~ 10|_____|11   9~ PB0 (AIN11)(SCL)
//
//

/*
  PIN#   DESC         Pin Name  Other/Sp  ADC0      ADC1      PTC       AC0       AC1       AC2       DAC0      USART0    SPI0      TWI0      TCA(PWM)  TCBn      TCD0      CCL
  0      A0 or SS     PA4                 AIN4      AIN0      X0/Y0                                             XDIR      SS                  WO4                 WOA       LUT0-OUT
  1      A1           PA5       VREFA     AIN5      AIN1      X1/Y1     OUT       AINN0                                                       WO5       TCB0 WO   WOB
  2      A2 or DAC    PA6                 AIN6      AIN2      X2/Y2     AINN0     AINP1     AINP0     OUT
  3      A3           PA7                 AIN7      AIN3      X3/Y3     AINP0     AINP0     AINN0                                                                           LUT1-OUT
  4      LED          PB5       CLKOUT    AIN8                          AINP1               AINP2                                             *WO2
  5                   PB4                 AIN9                          AINN1     AINP3                                                       *WO1                          *LUT0-OUT
  6      RX           PB3       TOSC1                                             OUT                           RxD                           *WO0
  7      TX           PB2       TOSC2 /                                                     OUT                 TxD                           WO2
                              EVOUT1
  8      SDA          PB1                 AIN10               X4/Y4     AINP2                                   XCK                 SDA       WO1
  9      SCL          PB0                 AIN11               X5/Y5               AINP2     AINP1               XDIR                SCL       WO0
  10                  PC0                           AIN6                                                                  *SCK                          TCB0 WO   WOC
  11                  PC1                           AIN7                                                                  *MISO                                   WOD       *LUT1-OUT
  12                  PC2       EVOUT2              AIN8                                                                  *MOSI
  13                  PC3                           AIN9                                                                  *SS                 *WO3                          LUT1-IN0
  14     MOSI         PA1                 AIN1                                                                  *TxD      MOSI      *SDA                                    LUT0-IN1
  15     MISO         PA2       EVOUT0    AIN2                                                                  *RxD      MISO      *SCL                                    LUT0-IN2
  16     SCK          PA3       EXTCLK    AIN3                                                                  *XCK      SCK                 WO3       TCB1 WO
  17     UPDI         PA0       RESET/    AIN0                                                                                                                              LUT1-IN0
                              UPDI
    alternative pin locations
*/


const uint8_t digital_pin_to_port[] = {
  // Left side, top to bottom
  PA, // 0  PA4
  PA, // 1  PA5
  PA, // 2  PA6
  PA, // 3  PA7
  PB, // 4  PB5
  PB, // 5  PB4
  PB, // 6  PB3
  PB, // 7  PB2
  PB, // 8  PB1
  // Right side, bottom to top
  PB, // 9  PB0
  PC, // 10 PC0
  PC, // 11 PC1
  PC, // 12 PC2
  PC, // 13 PC3
  // skip PA0 until the end
  PA, // 14 PA1
  PA, // 15 PA2
  PA, // 16 PA3
  PA  // 17 PA0 UPDI/RST
};

/* Use this for accessing PINnCTRL register */
const uint8_t digital_pin_to_bit_position[] = {
  // Left side, top to bottom
  PIN4_bp, // 0  PA4
  PIN5_bp, // 1  PA5
  PIN6_bp, // 2  PA6
  PIN7_bp, // 3  PA7
  PIN5_bp, // 4  PB5
  PIN4_bp, // 5  PB4
  PIN3_bp, // 6  PB3
  PIN2_bp, // 7  PB2
  PIN1_bp, // 8  PB1
  // Right side, bottom to top
  PIN0_bp, // 9  PB0
  PIN0_bp, // 10 PC0
  PIN1_bp, // 11 PC1
  PIN2_bp, // 12 PC2
  PIN3_bp, // 13 PC3
  PIN1_bp, // 14 PA1
  PIN2_bp, // 15 PA2
  PIN3_bp, // 16 PA3
  PIN0_bp  // 17 PA0
};

/* Use this for accessing PINnCTRL register */
const uint8_t digital_pin_to_bit_mask[] = {
  // Left side, top to bottom
  PIN4_bm, // 0  PA4
  PIN5_bm, // 1  PA5
  PIN6_bm, // 2  PA6
  PIN7_bm, // 3  PA7
  PIN5_bm, // 4  PB5
  PIN4_bm, // 5  PB4
  PIN3_bm, // 6  PB3
  PIN2_bm, // 7  PB2
  PIN1_bm, // 8  PB1
  // Right side, bottom to top
  PIN0_bm, // 9  PB0
  PIN0_bm, // 10 PC0
  PIN1_bm, // 11 PC1
  PIN2_bm, // 12 PC2
  PIN3_bm, // 13 PC3
  PIN1_bm, // 14 PA1
  PIN2_bm, // 15 PA2
  PIN3_bm, // 16 PA3
  PIN0_bm  // 17 PA0
};


const uint8_t digital_pin_to_timer[] = {
  // Left side, top to bottom
  TIMERA0,    // 0  PA4
  TIMERA0,    // 1  PA5
  #if defined(DAC0)
  DACOUT, // 2  PA6
  #else
  NOT_ON_TIMER, // 2  PA6
  #endif
  NOT_ON_TIMER,   // 3  PA7
  NOT_ON_TIMER,   // 4  PB5
  NOT_ON_TIMER,   // 5  PB4
  NOT_ON_TIMER,   // 6  PB3
  TIMERA0,    // 7  PB2
  TIMERA0,    // 8  PB1
  // Right side, bottom to top
  TIMERA0,    // 9  PB0
  #if (defined(TCD0) && defined(USE_TIMERD0_PWM))
  TIMERD0,    // 10 PC0
  TIMERD0,    // 11 PC1
  #else
  NOT_ON_TIMER,   // 10 PC0
  NOT_ON_TIMER,   // 11 PC1
  #endif
  NOT_ON_TIMER,   // 12 PC2
  NOT_ON_TIMER,   // 13 PC3
  NOT_ON_TIMER,   // 14 PA1
  NOT_ON_TIMER,   // 15 PA2
  TIMERA0,      // 16 PA3
  NOT_ON_TIMER  // 17 PA0


};

#endif

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
