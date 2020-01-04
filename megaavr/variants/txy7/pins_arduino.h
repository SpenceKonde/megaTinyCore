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

#define NUM_DIGITAL_PINS            22
#define NUM_ANALOG_INPUTS           12
//#define NUM_RESERVED_PINS           0 // (TOSC1/2, VREF, RESET, DEBUG USART Rx/Tx)
//#define NUM_INTERNALLY_USED_PINS    0 // (2 x Chip select + 2 x UART + 4 x IO + LED_BUILTIN + 1 unused pin)
#define NUM_I2C_PINS                2 // (SDA / SCL)
#define NUM_SPI_PINS                3 // (MISO / MOSI / SCK)
#define NUM_TOTAL_PINS              22

#define EXTERNAL_NUM_INTERRUPTS     22
#if (defined(TCD0) && defined(USE_TIMERD0_PWM))
#define digitalPinHasPWM(p)         ((p) == 0 || (p) == 1 || (p) == 7 || (p) == 8 || (p) == 9 || (p)==12 || (p)==13 || (p) == 16 )
#else
#define digitalPinHasPWM(p)         ((p) == 0 || (p) == 1 || (p) == 11 || (p) == 10 || (p) == 9 || (p) == 20)
#endif

#ifdef SPIREMAP
#define SPI_MUX         (4)
#define PIN_SPI_MISO    (13)
#define PIN_SPI_SCK        (12)
#define PIN_SPI_MOSI    (14)
#define PIN_SPI_SS        (15)
#else
#define SPI_MUX         (0)
#define PIN_SPI_MISO    (19)
#define PIN_SPI_SCK        (20)
#define PIN_SPI_MOSI    (18)
#define PIN_SPI_SS        (0)
#endif

#define MUX_SPI            (SPI_MUX)
#define SPI_INTERFACES_COUNT    1

static const uint8_t SS   = PIN_SPI_SS;
static const uint8_t MOSI = PIN_SPI_MOSI;
static const uint8_t MISO = PIN_SPI_MISO;
static const uint8_t SCK  = PIN_SPI_SCK;

#ifdef TWIREMAP
#define TWI_MUX         (16)
#define PIN_WIRE_SDA        (18)
#define PIN_WIRE_SCL        (19)
#else
#define TWI_MUX         (0)
#define PIN_WIRE_SDA        (10)
#define PIN_WIRE_SCL        (11)
#endif

static const uint8_t SDA = PIN_WIRE_SDA;
static const uint8_t SCL = PIN_WIRE_SCL;

// Mapped to HWSERIAL0 in Serial library
#define HWSERIAL0                 (&USART0)
#define HWSERIAL0_DRE_VECTOR     (USART0_DRE_vect)
#define HWSERIAL0_DRE_VECTOR_NUM (USART0_DRE_vect_num)
#define HWSERIAL0_RXC_VECTOR     (USART0_RXC_vect)
#ifdef UARTREMAP
#define HWSERIAL0_MUX             0x01
#define PIN_WIRE_HWSERIAL0_RX     (19)
#define PIN_WIRE_HWSERIAL0_TX     (18)
#else
#define HWSERIAL0_MUX             0x00
#define PIN_WIRE_HWSERIAL0_RX     (8)
#define PIN_WIRE_HWSERIAL0_TX     (9)
#endif


#define LED_BUILTIN 3

#define PINS_COUNT        (22u)


#define PIN_A0   (21)
#define PIN_A1   (18)
#define PIN_A2   (19)
#define PIN_A3   (20)
#define PIN_A4   (0)
#define PIN_A5      (1)
#define PIN_A6     (2)
#define PIN_A7   (3)
#define PIN_A8   (6)
#define PIN_A9   (7)
#define PIN_A10  (10)
#define PIN_A11  (11)

static const uint8_t A0 = PIN_A0;
static const uint8_t A1 = PIN_A1;
static const uint8_t A2 = PIN_A2;
static const uint8_t A3 = PIN_A3;
static const uint8_t A4 = PIN_A4;
static const uint8_t A5 = PIN_A5;
static const uint8_t A6 = PIN_A6;
static const uint8_t A7 = PIN_A7;
static const uint8_t A8 = PIN_A8;
static const uint8_t A9 = PIN_A9;
static const uint8_t A10 = PIN_A10;
static const uint8_t A11 = PIN_A11;

static const uint8_t PIN_PA4=0;
static const uint8_t PIN_PA5=1;
static const uint8_t PIN_PA6=2;
static const uint8_t PIN_PA7=3;
static const uint8_t PIN_PB7=4;
static const uint8_t PIN_PB6=5;
static const uint8_t PIN_PB5=6;
static const uint8_t PIN_PB4=7;
static const uint8_t PIN_PB3=8;
static const uint8_t PIN_PB2=9;
static const uint8_t PIN_PB1=10;
static const uint8_t PIN_PB0=11;
static const uint8_t PIN_PC0=12;
static const uint8_t PIN_PC1=13;
static const uint8_t PIN_PC2=14;
static const uint8_t PIN_PC3=15;
static const uint8_t PIN_PC4=16;
static const uint8_t PIN_PC5=17;
static const uint8_t PIN_PA0=21;
static const uint8_t PIN_PA1=18;
static const uint8_t PIN_PA2=19;
static const uint8_t PIN_PA3=20;


#ifdef ARDUINO_MAIN

// On the Arduino board, digital pins are also used
// for the analog output (PWM).  Analog input
// pins are a separate set.9
// ATtiny1617 / ARDUINO
//                              (MOSI) (UPDI)
//                              (AIN1) (AIN0)
//                                PA1   PA0   PC5   PC4   PC3   PC2
//                                18    21    17    16    15    14
//                             ______________________________________
//                            |*  24    23    22    21    20    19   |
//                            |                                      |
//        (MISO)(AIN2) PA2 19 | 1                                  18| 13  PC1 (PWM only on 1-series)
//                            |                                      |
// (EXTCLK)(SCK)(AIN3) PA3 20~| 2                                  17| 12  PC0 (PWM only on 1-series)
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

/*
PIN#   DESC         Pin Name  Other/Sp  ADC0      ADC1      PTC       AC0       AC1       AC2       DAC0      USART0    SPI0      TWI0      TCA(PWM)  TCBn      TCD0      CCL
0      A0 or SS     PA4                 AIN4      AIN0      X0/Y0                                             XDIR      SS                  WO4                 WOA       LUT0-OUT
1      A1           PA5       VREFA     AIN5      AIN1      X1/Y1     OUT       AINN0                                                       WO5       TCB0 WO   WOB
2      A2 or DAC    PA6                 AIN6      AIN2      X2/Y2     AINN0     AINP1     AINP0     OUT
3      A3           PA7                 AIN7      AIN3      X3/Y3     AINP0     AINP0     AINN0                                                                           LUT1-OUT
4                   PB7
5                   PB6
6      LED          PB5       CLKOUT    AIN8                          AINP1               AINP2                                             *WO2
7                   PB4                 AIN9                          AINN1     AINP3                                                       *WO1                          *LUT0-OUT
8      RX           PB3       TOSC1                                             OUT                           RxD                           *WO0
9      TX           PB2       TOSC2 /                                                     OUT                 TxD                           WO2
                              EVOUT1
10     SDA          PB1                 AIN10               X4/Y4     AINP2                                   XCK                 SDA       WO1
11     SCL          PB0                 AIN11               X5/Y5               AINP2     AINP1               XDIR                SCL       WO0
12                  PC0                           AIN6                                                                  *SCK                          TCB0 WO   WOC
13                  PC1                           AIN7                                                                  *MISO                                   WOD       *LUT1-OUT
14                  PC2       EVOUT2              AIN8                                                                  *MOSI
15                  PC3                           AIN9                                                                  *SS                 *WO3                          LUT1-IN0
16                  PC4
17                  PC5
18     MOSI         PA1                 AIN1                                                                  *TxD      MOSI      *SDA                                    LUT0-IN1
19     MISO         PA2       EVOUT0    AIN2                                                                  *RxD      MISO      *SCL                                    LUT0-IN2
20     SCK          PA3       EXTCLK    AIN3                                                                  *XCK      SCK                 WO3       TCB1 WO
21?    UPDI         PA0       RESET/    AIN0                                                                                                                              LUT1-IN0
                              UPDI
    * alternative pin locations
*/

const uint8_t PROGMEM digital_pin_to_port[] = {
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
const uint8_t PROGMEM digital_pin_to_bit_position[] = {
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
const uint8_t PROGMEM digital_pin_to_bit_mask[] = {
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


const uint8_t PROGMEM digital_pin_to_timer[] = {
      // Left side, top to bottom
    TIMERA0,         // 0  PA4
    TIMERA0,         // 1  PA5
    #if defined(DAC0) && defined(DACVREF)
    DACOUT, // 2  PA6
    #else
    NOT_ON_TIMER, // 2  PA6
    #endif
    NOT_ON_TIMER,     // 3  PA7
    NOT_ON_TIMER,     // 4  PB7
    NOT_ON_TIMER,     // 5  PB6
    NOT_ON_TIMER,     // 6  PB5
    NOT_ON_TIMER,     // 7  PB4
    NOT_ON_TIMER,     // 8  PB3
    TIMERA0,         // 9  PB2
    TIMERA0,         // 10 PB1
    // Right side, bottom to top
    TIMERA0,         // 11 PB0
    #if (defined(TCD0) && defined(USE_TIMERD0_PWM) &&(!defined(MILLIS_USE_TIMERD0)))
    TIMERD0,         // 12 PC0
    TIMERD0,         // 13 PC1
    #else
    NOT_ON_TIMER,     // 12 PC0
    NOT_ON_TIMER,     // 13 PC1
    #endif
    NOT_ON_TIMER,     // 14 PC2
    NOT_ON_TIMER,     // 15 PC3
    NOT_ON_TIMER,     // 16 PC4
    NOT_ON_TIMER,     // 17 PC5
    NOT_ON_TIMER,     // 18 PA1
    NOT_ON_TIMER,     // 19 PA2
    TIMERA0,          // 20 PA3
    NOT_ON_TIMER     // 21 PA0


};
/*
#define PIN_A0   (21)
#define PIN_A1   (18)
#define PIN_A2   (19)
#define PIN_A3   (20)
#define PIN_A4   (0)
#define PIN_A5      (1)
#define PIN_A6     (2)
#define PIN_A7   (3)
#define PIN_A8   (6)
#define PIN_A9   (7)
#define PIN_A10  (10)
#define PIN_A11  (11)
*/
#endif

//extern const uint8_t analog_pin_to_channel[];
// #define digitalPinToAnalogInput(p)  ((p < NUM_ANALOG_INPUTS) ? pgm_read_byte(analog_pin_to_channel + p) : NOT_A_PIN )
//#define digitalPinToAnalogInput(p)         (((p) < 6 || (p) == 8 || (p) == 9 || (p) > 13) ? pgm_read_byte(analog_pin_to_channel + p) : NOT_A_PIN)
#define digitalPinToAnalogInput(p)      ((p<4)?(p+4):(p==21?0:((p>17)?(p-17):((p<8)?(p+2):(p<12?(p):NOT_A_PIN)))))


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
#define SERIAL_PORT_MONITOR            Serial
#define SERIAL_PORT_HARDWARE        Serial

#endif
