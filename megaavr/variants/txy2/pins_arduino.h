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

#define NUM_DIGITAL_PINS            6 //
#define NUM_ANALOG_INPUTS           9
//#define NUM_RESERVED_PINS           0 // (TOSC1/2, VREF, RESET, DEBUG USART Rx/Tx)
//#define NUM_INTERNALLY_USED_PINS    0 // (2 x Chip select + 2 x UART + 4 x IO + LED_BUILTIN + 1 unused pin)
#define NUM_I2C_PINS                2 // (SDA / SCL)
#define NUM_SPI_PINS                3 // (MISO / MOSI / SCK)
#define NUM_TOTAL_FREE_PINS         (NUM_DIGITAL_PINS)
#define NUM_TOTAL_PINS              6

#define EXTERNAL_NUM_INTERRUPTS     8


#define digitalPinHasPWM(p)         ((p) != 0 && (p) != 5)

#ifdef PORTMUX_SPI0_bm
#define PIN_SPI_MISO_PINSWAP_1 	(1)
#define PIN_SPI_SCK_PINSWAP_1 		(4)
#define PIN_SPI_MOSI_PINSWAP_1 	(0)
#define PIN_SPI_SS_PINSWAP_1 		(0)
#endif
#define PIN_SPI_MISO	(3)
#define PIN_SPI_SCK		(4)
#define PIN_SPI_MOSI	(2)
#define PIN_SPI_SS		(0)

#define SPI_INTERFACES_COUNT	1

#define PIN_WIRE_SDA        (2)
#define PIN_WIRE_SCL        (3)

#ifdef PIN_WIRE_SCL_PINSWAP_1
#define SDA ((uint8_t) (PORTMUX.CTRLB&PORTMUX_TWI0_bm?PIN_WIRE_SDA_PINSWAP_1:PIN_WIRE_SDA))
#define SCL ((uint8_t) (PORTMUX.CTRLB&PORTMUX_TWI0_bm?PIN_WIRE_SCL_PINSWAP_1:PIN_WIRE_SCL))
#else
static const uint8_t SDA = PIN_WIRE_SDA;
static const uint8_t SCL = PIN_WIRE_SCL;
#endif

#ifdef PIN_SPI_SCK_PINSWAP_1
#define SS ((uint8_t) (PORTMUX.CTRLB&PORTMUX_SPI0_bm?PIN_SPI_SS_PINSWAP_1:PIN_SPI_SS))
#define MOSI ((uint8_t) (PORTMUX.CTRLB&PORTMUX_SPI0_bm?PIN_SPI_MOSI_PINSWAP_1:PIN_MOSI_SCK))
#define MISO ((uint8_t) (PORTMUX.CTRLB&PORTMUX_SPI0_bm?PIN_SPI_MISO_PINSWAP_1:PIN_MISO_SCK))
#define SCK ((uint8_t) (PORTMUX.CTRLB&PORTMUX_SPI0_bm?PIN_SPI_SCK_PINSWAP_1:PIN_SPI_SCK))
#else
static const uint8_t SS   = PIN_SPI_SS;
static const uint8_t MOSI = PIN_SPI_MOSI;
static const uint8_t MISO = PIN_SPI_MISO;
static const uint8_t SCK  = PIN_SPI_SCK;
#endif


// Mapped to HWSERIAL0 in Serial library
#define HWSERIAL0 				(&USART0)
#define HWSERIAL0_DRE_VECTOR 	(USART0_DRE_vect)
#define HWSERIAL0_DRE_VECTOR_NUM (USART0_DRE_vect_num)
#define HWSERIAL0_RXC_VECTOR 	(USART0_RXC_vect)
#define HWSERIAL0_MUX_PINSWAP_1 			0x01
#define PIN_WIRE_HWSERIAL0_RX_PINSWAP_1 	(3)
#define PIN_WIRE_HWSERIAL0_TX_PINSWAP_1 	(2)
#define HWSERIAL0_MUX 			0x00
#define PIN_WIRE_HWSERIAL0_RX 	(1)
#define PIN_WIRE_HWSERIAL0_TX 	(0)

#ifdef DAC0
#define DAC_PIN PIN_PA6
#endif
#define LED_BUILTIN PIN_PA3


#define PIN_A0   (5)
#define PIN_A1   (2)
#define PIN_A2   (3)
#define PIN_A3   (4)
#define PIN_A6	 (0)
#define PIN_A7   (1)

static const uint8_t A0 = PIN_A0;
static const uint8_t A1 = PIN_A1;
static const uint8_t A2 = PIN_A2;
static const uint8_t A3 = PIN_A3;
static const uint8_t A6 = PIN_A6;
static const uint8_t A7 = PIN_A7;

static const uint8_t PIN_PA6=0;
static const uint8_t PIN_PA7=1;
static const uint8_t PIN_PA0=5;
static const uint8_t PIN_PA1=2;
static const uint8_t PIN_PA2=3;
static const uint8_t PIN_PA3=4;

#define PINS_COUNT		(6u)

#ifdef ARDUINO_MAIN

// On the Arduino board, digital pins are also used
// for the analog output (software PWM).  Analog input
// pins are a separate set.

// ATtiny412 / ARDUINO
//                                    _____
//                            VDD   1|*    |20  GND
//           (DAC)   (AIN6) PA6  0   2|     |19  4~  PA3 (AIN3)(SCK)(EXTCLK)
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
	* alternative pin locations
*/

const uint8_t digital_pin_to_port[] = {
	PA, // 0  PA6
	PA, // 1  PA7
	PA, // 2  PA1
	PA, // 3  PA2
	// skip PA0 UPDI
	PA, // 4  PA3
	PA  // 5  PA0
};

/* Use this for accessing PINnCTRL register */
const uint8_t digital_pin_to_bit_position[] = {
	PIN6_bp, // 0  PA6
	PIN7_bp, // 1  PA7
	PIN1_bp, // 2  PA1
	PIN2_bp, // 3  PA2
	// skip PA0 UPDI
	PIN3_bp, // 4  PA3
	PIN0_bp  // 5  PA0
};

/* Use this for accessing PINnCTRL register */
const uint8_t digital_pin_to_bit_mask[] = {
	PIN6_bm, // 0  PA6
	PIN7_bm, // 1  PA7
	PIN1_bm, // 2  PA1
	PIN2_bm, // 3  PA2
	// skip PA0 UPDI
	PIN3_bm, // 4  PA3
	PIN0_bm  // 5  PA0
};

const uint8_t digital_pin_to_timer[] = {
	#if defined(DAC0)
    DACOUT,
	#else
	NOT_ON_TIMER, // 0  PA6
	#endif
	TIMERA0, // 1  PA7
	TIMERA0, // 2  PA1
	TIMERA0, // 3  PA2
	// skip PA0 UPDI
	TIMERA0, // 4  PA3
	NOT_ON_TIMER  // 5  PA0
};

/*
#define PIN_A0   (5)
#define PIN_A1   (2)
#define PIN_A2   (3)
#define PIN_A3   (4)
#define PIN_A6	 (0)
#define PIN_A7   (1)

*/
#endif

extern const uint8_t analog_pin_to_channel[];
// #define digitalPinToAnalogInput(p)  ((p < NUM_ANALOG_INPUTS) ? pgm_read_byte(analog_pin_to_channel + p) : NOT_A_PIN )
//#define digitalPinToAnalogInput(p) 		(((p) < 6 || (p) == 8 || (p) == 9 || (p) > 13) ? pgm_read_byte(analog_pin_to_channel + p) : NOT_A_PIN)
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
#define SERIAL_PORT_MONITOR			Serial
#define SERIAL_PORT_HARDWARE		Serial

#endif
