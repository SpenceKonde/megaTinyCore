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

#define NUM_DIGITAL_PINS              (22)

#if MEGATINYCORE_SERIES == 2
  /* Yes, this is actually one more than we have, but the way this is used by the code means that it actually needs to be (highest mux channel for a pin + 1) */
  #define NUM_ANALOG_INPUTS           (16)
#else
  #define NUM_ANALOG_INPUTS           (12)
#endif

#define NUM_I2C_PINS                  (2) // (SDA / SCL)
#define NUM_SPI_PINS                  (3) // (MISO / MOSI / SCK)
#define NUM_TOTAL_PINS                (22)
#define PINS_COUNT                    (22)

#define EXTERNAL_NUM_INTERRUPTS       (22)

#if (defined(TCD0) && defined(USE_TIMERD0_PWM))
  #define digitalPinHasPWM(p)         ((p) == PIN_PA4 || (p) == PIN_PA5 || (p) == PIN_PB2 || (p) == PIN_PB1 || (p) == PIN_PB0 || (p) == PIN_PC0 || (p) == PIN_PC1 || (p) == PIN_PA3)
#else
  #define digitalPinHasPWM(p)         ((p) == PIN_PA4 || (p) == PIN_PA5 || (p) == PIN_PB2 || (p) == PIN_PB1 || (p) == PIN_PB0                                     || (p) == PIN_PA3)
#endif


#if MEGATINYCORE_SERIES != 2
  #define digitalPinToAnalogInput(p)  (((p) < 4) ? ((p) + 4) : ((p) == 21 ? 0 : ((((p) > 17) && ((p) < 21)) ? ((p) - 17) : (((p) < 8) ? ((p) + 2) : (((p) < 12) ? (p):NOT_A_PIN)))))
  #if defined(ADC1) // 3217 and 1617 have a second ADC.
    #define digitalPinToAnalogInput_ADC1(p) ((p) < 6 ? (p) : ((p) < PINS_COUNT && ((p) >= PIN_PC0) ? (6 + (p) - PIN_PC0) : NOT_A_PIN))
  #endif
#else
  /* 2-series MUX table says ADC channel 0 is tied to ground, not PA0, PC0 through PC3 are a A12-15 */
  #define digitalPinToAnalogInput(p)  (((p) < 4) ? ((p) + 4) : ((((p) > 17) && ((p) < 21)) ? ((p) - 17) : (((p) < 8) ? ((p) + 2) : (((p) < 16) ? (p):NOT_A_PIN))))
#endif

#ifdef DAC0 // 1-series only
  #define DAC_PIN      (PIN_PA6)
#endif

#ifndef LED_BUILTIN
  #define LED_BUILTIN  (PIN_PA7)
#endif

/*
      ####   ###  ####  ##### #   # #   # #   #
      #   # #   # #   #   #   ## ## #   #  # #
      ####  #   # ####    #   # # # #   #   #
      #     #   # # #     #   #   # #   #  # #
      #      ###  #  #    #   #   #  ###  #   #
*/
#define SPI_MUX                       (0)
#define PIN_SPI_MOSI                  (PIN_PA1)
#define PIN_SPI_MISO                  (PIN_PA2)
#define PIN_SPI_SCK                   (PIN_PA3)
#define PIN_SPI_SS                    (PIN_PA4)
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

// Only 1-series parts have alternate pins for I2C...
#ifdef PORTMUX_TWI0_bm
  #define PIN_WIRE_SDA_PINSWAP_1      (PIN_PA1)
  #define PIN_WIRE_SCL_PINSWAP_1      (PIN_PA2)
#endif
#define PIN_WIRE_SDA                  (PIN_PB1)
#define PIN_WIRE_SCL                  (PIN_PB0)

/* Serial pin values here are not used by the core. They don't get picked up correctly in UART.h where they're needed and I can't figure out why. */
// Serial - All parts with 14 or more pins have all alt and normal pins, and all 4 of them.
#define HWSERIAL0_MUX_DEFAULT         (0)
#define PIN_HWSERIAL0_TX              (PIN_PB2)
#define PIN_HWSERIAL0_RX              (PIN_PB3)
#define PIN_HWSERIAL0_XCK             (PIN_PB1)
#define PIN_HWSERIAL0_XDIR            (PIN_PB0)
#define PIN_HWSERIAL0_TX_PINSWAP_1    (PIN_PA1)
#define PIN_HWSERIAL0_RX_PINSWAP_1    (PIN_PA2)
#define PIN_HWSERIAL0_XCK_PINSWAP_1   (PIN_PA3)
#define PIN_HWSERIAL0_XDIR_PINSWAP_1  (PIN_PA4)
#if defined(USART1)
  // Serial1 on tinyAVR 2-series only, and uses as default pins USART0's alternate pins
  #define HWSERIAL1_MUX_DEFAULT         (0)
  #define PIN_HWSERIAL1_TX              (PIN_PA1)
  #define PIN_HWSERIAL1_RX              (PIN_PA2)
  #define PIN_HWSERIAL1_XCK             (PIN_PA3)
  #define PIN_HWSERIAL1_XDIR            (PIN_PA4)
  #define PIN_HWSERIAL1_TX_PINSWAP_1    (PIN_PC2)
  #define PIN_HWSERIAL1_RX_PINSWAP_1    (PIN_PC1)
  #define PIN_HWSERIAL1_XCK_PINSWAP_1   (PIN_PC0)
  #define PIN_HWSERIAL1_XDIR_PINSWAP_1  (PIN_PC3)
#endif


#define PIN_PA4        (0)
#define PIN_PA5        (1)
#define PIN_PA6        (2)
#define PIN_PA7        (3)
#define PIN_PB7        (4)
#define PIN_PB6        (5)
#define PIN_PB5        (6)
#define PIN_PB4        (7)
#define PIN_PB3        (8)
#define PIN_PB2        (9)
#define PIN_PB1        (10)
#define PIN_PB0        (11)
#define PIN_PC0        (12)
#define PIN_PC1        (13)
#define PIN_PC2        (14)
#define PIN_PC3        (15)
#define PIN_PC4        (16)
#define PIN_PC5        (17)
#define PIN_PA0        (21)
#define PIN_PA1        (18)
#define PIN_PA2        (19)
#define PIN_PA3        (20)

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

/* Not Recommended to use the AINn constants */
#if MEGATINYCORE_SERIES != 2
  #define AIN0             NOT_A_PIN
#endif
#define AIN1               ADC_CH(1)
#define AIN2               ADC_CH(2)
#define AIN3               ADC_CH(3)
#define AIN4               ADC_CH(4)
#define AIN5               ADC_CH(5)
#define AIN6               ADC_CH(6)
#define AIN7               ADC_CH(7)
#define AIN8               ADC_CH(8)
#define AIN9               ADC_CH(9)
#define AIN10              ADC_CH(10)
#define AIN11              ADC_CH(11)
#if MEGATINYCORE_SERIES == 2
  #define AIN12              ADC_CH(12)
  #define AIN13              ADC_CH(13)
  #define AIN14              ADC_CH(14)
  #define AIN15              ADC_CH(15)
#endif

/*
            ####  ### #   #      ##  ####  ####   ##  #   #  ###
            #   #  #  ##  #     #  # #   # #   # #  #  # #  #
            ####   #  # # #     #### ####  ####  ####   #    ###
            #      #  #  ##     #  # # #   # #   #  #   #       #
            #     ### #   #     #  # #  #  #  #  #  #   #    ###
*/

#ifdef ARDUINO_MAIN

// On the Arduino board, digital pins are also used for the analog output (PWM)
// as well as analog input (ADC)
// ATtiny1617 / ARDUINO
//                              (MOSI) (UPDI)
//                              (AIN1) (AIN0)
//                                PA1   PA0   PC5   PC4   PC3   PC2
//                                18    21    17    16    15    14
//                             ______________________________________
//                            |*  24    23    22    21    20    19   |
//                            |                                      |
//        (MISO)(AIN2) PA2 19 | 1                                  18| 13  PC1 (PWM on 1-series only)
//                            |                                      |
// (EXTCLK)(SCK)(AIN3) PA3 20~| 2                                  17| 12  PC0 (PWM on 1-series only)
//                            |                                      |
//                       GND  | 3                                  16| 11~ PB0 (AIN11)(SCL)
//                            |                                      |
//                       VDD  | 4                                  15| 10~ PB1 (AIN10)(SDA)
//                            |                                      |
//      (nSS)  (AIN4) PA4  0~ | 5                                  14|  9~ PB2 (TOSC2)(TXD)
//                            |                                      |
//             (AIN5) PA5  1~ | 6                                  13|  8  PB3 (TOSC1)(RXD)
//                            |    7     8     9    10    11    12   |
//                            |______________________________________|
//                                 2     3     4     5     6     7
//                                PA6   PA7   PB7   PB6   PB5   PB4
//                              (AIN6) (AIN7)           (AIN8) (AIN9)
//                               (DAC)

/*  0/1-series. ADC1, TCB1 and AC1, AC2 present only on 16/32k 1-series. TCD0, DAC0, alternate SPI/I2C pins and AC0 inputs past 0 only available on 1-series.

  PIN#   DESC         Pin Name  Other/Sp  ADC0      ADC1      PTC       AC0       AC1       AC2       DAC0      USART0    SPI0      TWI0      TCA(PWM)  TCBn      TCD0      CCL
  0                   PA4                 AIN4      AIN0      X0/Y0                                              XDIR      SS                  WO4                WOA        LUT0-OUT
  1                   PA5       VREFA     AIN5      AIN1      X1/Y1     OUT       AINN0                                                        WO5       TCB0 WO  WOB
  2      DAC          PA6                 AIN6      AIN2      X2/Y2     AINN0     AINP1     AINP0     OUT
  3      LED          PA7                 AIN7      AIN3      X3/Y3     AINP0     AINP0     AINN0                                                                            LUT1-OUT
  4                   PB7
  5                   PB6
  6                   PB5       CLKOUT    AIN8                          AINP1               AINP2                                             *WO2
  7                   PB4                 AIN9                          AINN1     AINP3                                                       *WO1                          *LUT0-OUT
  8      RX           PB3       TOSC1                                             OUT                            RxD                          *WO0
  9      TX           PB2    TOSC2/EVOUT1                                                   OUT                  TxD                           WO2
  10     SDA          PB1                 AIN10               X4/Y4     AINP2                                    XCK                SDA        WO1
  11     SCL          PB0                 AIN11               X5/Y5               AINP2     AINP1                XDIR               SCL        WO0
  12                  PC0                           AIN6                                                                  *SCK                          *TCB0 WO  WOC
  13                  PC1                           AIN7                                                                  *MISO                                   WOD       *LUT1-OUT
  14                  PC2       EVOUT2              AIN8                                                                  *MOSI
  15                  PC3                           AIN9                                                                  *SS                 *WO3                           LUT1-IN0
  16                  PC4                           AIN10                                                                                     *WO4      *TCB1 WO             LUT1-IN1
  17                  PC5                           AIN11                                                                                     *WO5                           LUT1-IN2
  21     UPDI         PA0     RESET/UPDI  AIN0                                                                                                                               LUT0-IN0
  18     MOSI         PA1                 AIN1                                                                  *TxD       MOSI     *SDA                                     LUT0-IN1
  19     MISO         PA2       EVOUT0    AIN2                                                                  *RxD       MISO     *SCL                                     LUT0-IN2
  20     SCK          PA3       EXTCLK    AIN3                                                                  *XCK       SCK                 WO3       TCB1 WO
      * alternative pin locations


  2-series
  PIN#   DESC         Pin Name  Other/Sp  ADC0      AC0       USART0    SPI0      TWI0      TCA(PWM)  TCBn      CCL
  0                   PA4                 AIN4                XDIR      SS                   WO4                 LUT0-OUT
  1                   PA5       VREFA     AIN5      OUT                                      WO5      TCB0 WO   *LUT3-OUT
  2                   PA6                 AIN6      AINN0
  3      LED          PA7      *EVOUTA    AIN7      AINP0                                                        LUT1-OUT
  4                   PB7      *EVOUTB
  5                   PB6                           AINP3                                                       *LUT2-OUT
  6                   PB5       CLKOUT    AIN8      AINP1                                   *WO2
  7      Alt Reset    PB4      *RESET     AIN9      AINN1                                   *WO1                *LUT0-OUT
  8      RX           PB3       TOSC1                          RxD                          *WO0
  9      TX           PB2   TOSC2/EVOUTB                       TxD                           WO2                 LUT2-IN2
  10     SDA          PB1                 AIN10     AINP2      XCK                 SDA       WO1                 LUT2-IN1
  11     SCL          PB0                 AIN11     AINN2      XDIR                SCL       WO0                 LUT2-IN0
  12                  PC0                                               *SCK                          *TCB0 WO   LUT3-IN0
  13                  PC1                                               *MISO                                   *LUT1-OUT/LUT3-IN1
  14                  PC2       EVOUTC                                  *MOSI                                    LUT3-IN2
  15                  PC3                                               *SS                 *WO3                 LUT1-IN0
  16                  PC4                                                                   *WO4      *TCB1 WO   LUT1-IN1/LUT3-OUT
  17                  PC5                                                                   *WO5                 LUT1-IN2
  21     UPDI         PA0       RESET/UPDI                                                                       LUT0-IN0
  18     MOSI         PA1                 AIN1                *TxD      MOSI      *SDA                           LUT0-IN1
  19     MISO         PA2       EVOUTA    AIN2                *RxD      MISO      *SCL                           LUT0-IN2
  20     SCK          PA3       EXTCLK    AIN3                *XCK      SCK                 WO3       TCB1 WO
     * alternative pin locations
*/

const uint8_t digital_pin_to_port[] = {
  // Left side, top to bottom
  PA, // 0  PA4
  PA, // 1  PA5
  PA, // 2  PA6
  PA, // 3  PA7
  PB, // 4  PB7
  PB, // 5  PB6
  PB, // 6  PB5
  PB, // 7  PB4
  PB, // 8  PB3
  PB, // 9  PB2
  PB, // 10 PB1
  PB, // 11 PB0
  PC, // 12 PC0
  PC, // 13 PC1
  PC, // 14 PC2
  PC, // 15 PC3
  PC, // 16 PC4
  PC, // 17 PC5
  // skip PA0 until the end
  PA, // 18 PA1
  PA, // 19 PA2
  PA, // 20 PA3
  PA  // 21 PA0 UPDI/RST
};

/* Use this for accessing PINnCTRL register */
const uint8_t digital_pin_to_bit_position[] = {
  // Left side, top to bottom
  PIN4_bp, // 0  PA4
  PIN5_bp, // 1  PA5
  PIN6_bp, // 2  PA6
  PIN7_bp, // 3  PA7
  PIN7_bp, // 4  PB7
  PIN6_bp, // 5  PB6
  PIN5_bp, // 6  PB5
  PIN4_bp, // 7  PB4
  PIN3_bp, // 8  PB3
  PIN2_bp, // 9  PB2
  PIN1_bp, // 10 PB1
  PIN0_bp, // 11 PB0
  PIN0_bp, // 12 PC0
  PIN1_bp, // 13 PC1
  PIN2_bp, // 14 PC2
  PIN3_bp, // 15 PC3
  PIN4_bp, // 16 PA4
  PIN5_bp, // 17 PA5
  PIN1_bp, // 18 PA1
  PIN2_bp, // 19 PA2
  PIN3_bp, // 20 PA3
  PIN0_bp  // 21 PA0
};

/* Use this for accessing PINnCTRL register */
const uint8_t digital_pin_to_bit_mask[] = {
  PIN4_bm, // 0  PA4
  PIN5_bm, // 1  PA5
  PIN6_bm, // 2  PA6
  PIN7_bm, // 3  PA7
  PIN7_bm, // 4  PB7
  PIN6_bm, // 5  PB6
  PIN5_bm, // 6  PB5
  PIN4_bm, // 7  PB4
  PIN3_bm, // 8  PB3
  PIN2_bm, // 9  PB2
  PIN1_bm, // 10 PB1
  PIN0_bm, // 11 PB0
  PIN0_bm, // 12 PC0
  PIN1_bm, // 13 PC1
  PIN2_bm, // 14 PC2
  PIN3_bm, // 15 PC3
  PIN4_bm, // 16 PA4
  PIN5_bm, // 17 PA5
  PIN1_bm, // 18 PA1
  PIN2_bm, // 19 PA2
  PIN3_bm, // 20 PA3
  PIN0_bm  // 21 PA0
};


const uint8_t digital_pin_to_timer[] = {
  // Left side, top to bottom
  TIMERA0,         // 0  PA4 WO4 WOA
  TIMERA0,         // 1  PA5 WO5 WOB
  #if defined(DAC0)
  DACOUT,           // 2  PA6
  #else
  NOT_ON_TIMER,     // 2  PA6
  #endif
  NOT_ON_TIMER,     // 3  PA7
  NOT_ON_TIMER,     // 4  PB7
  NOT_ON_TIMER,     // 5  PB6
  NOT_ON_TIMER,     // 6  PB5 WO2 Alt
  NOT_ON_TIMER,     // 7  PB4 WO1 Alt
  NOT_ON_TIMER,     // 8  PB3 WO0 Alt
  TIMERA0,          // 9  PB2 WO2
  TIMERA0,          // 10 PB1 WO1
  // Right side, bottom to top
  TIMERA0,          // 11 PB0 WO0
  #if (defined(TCD0) && defined(USE_TIMERD0_PWM))
  TIMERD0,          // 12 PC0 WOC
  TIMERD0,          // 13 PC1 WOD
  #else
  NOT_ON_TIMER,     // 12 PC0
  NOT_ON_TIMER,     // 13 PC1
  #endif
  NOT_ON_TIMER,     // 14 PC2
  NOT_ON_TIMER,     // 15 PC3 WO3 Alt
  NOT_ON_TIMER,     // 16 PC4 WO4 Alt
  NOT_ON_TIMER,     // 17 PC5 WO5 Alt
  NOT_ON_TIMER,     // 18 PA1
  NOT_ON_TIMER,     // 19 PA2
  TIMERA0,          // 20 PA3 WO3
  NOT_ON_TIMER      // 21 PA0


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
