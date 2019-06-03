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

#define NUM_DIGITAL_PINS            17 // 
#define NUM_ANALOG_INPUTS           11
//#define NUM_RESERVED_PINS           0 // (TOSC1/2, VREF, RESET, DEBUG USART Rx/Tx)
//#define NUM_INTERNALLY_USED_PINS    0 // (2 x Chip select + 2 x UART + 4 x IO + LED_BUILTIN + 1 unused pin)
#define NUM_I2C_PINS                2 // (SDA / SCL)
#define NUM_SPI_PINS                3 // (MISO / MOSI / SCK)
#define NUM_TOTAL_FREE_PINS         (NUM_DIGITAL_PINS)
#define NUM_TOTAL_PINS              (NUM_DIGITAL_PINS + NUM_I2C_PINS + NUM_SPI_PINS)
#define ANALOG_INPUT_OFFSET         14
#define digitalPinToAnalogInput(p)  ((p < NUM_ANALOG_INPUTS) ? (p) : (p) - ANALOG_INPUT_OFFSET)

#define EXTERNAL_NUM_INTERRUPTS     17

#define digitalPinHasPWM(p)         ((p) == 3 || (p) == 5 || (p) == 6 || (p) == 9 || (p) == 10)

#define SPI_MUX		  	(PORTMUX_SPI0_ALT1_gc)
#define PIN_SPI_MISO	(15)
#define PIN_SPI_SCK		(16)
#define PIN_SPI_MOSI	(14)
#define PIN_SPI_SS		(0)

static const uint8_t SS   = PIN_SPI_SS;
static const uint8_t MOSI = PIN_SPI_MOSI;
static const uint8_t MISO = PIN_SPI_MISO;
static const uint8_t SCK  = PIN_SPI_SCK;

#define PIN_WIRE_SDA        (8)
#define PIN_WIRE_SCL        (9)

static const uint8_t SDA = PIN_WIRE_SDA;
static const uint8_t SCL = PIN_WIRE_SCL;

#define MUX_SPI			(SPI_MUX)
#define SPI_INTERFACES_COUNT	1

#define LED_BUILTIN 6

#define PIN_A0   (10)
#define PIN_A1   (11)
#define PIN_A2   (12)
#define PIN_A3   (13)
#define PIN_A4   (14)
#define PIN_A5   (15)

static const uint8_t A0 = PIN_A0;
static const uint8_t A1 = PIN_A1;
static const uint8_t A2 = PIN_A2;
static const uint8_t A3 = PIN_A3;
static const uint8_t A4 = PIN_A4;
static const uint8_t A5 = PIN_A5;

#define PINS_COUNT		(20u)

#ifdef ARDUINO_MAIN

// On the Arduino board, digital pins are also used
// for the analog output (software PWM).  Analog input
// pins are a separate set.

// ATtiny1616 / ARDUINO
//                _____ 
//        VDD   1|*    |20  GND
//  (nSS) PA4   2|     |19  PA3 (EXTCLK)
//        PA5   3|     |18  PA2 (MISO)
//  (DAC) PA6   4|     |17  PA1 (MOSI)
//        PA7   5|     |16  PA0 (nRESET/UPDI)
//        PB5   6|     |15  PC3
//        PB4   7|     |14  PC2 
//(TOSC1) PB3   8|     |13  PC1
//(TOSC2) PB2   9|     |12  PC0
//  (SDA) PB1  10|_____|11  PB0 (SCL)
//               
//

const uint8_t PROGMEM digital_pin_to_port[] = {	
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
	PA, // 15 PA1
	PA, // 16 PA2
	PA  // 17 PA3
};

/* Use this for accessing PINnCTRL register */
const uint8_t PROGMEM digital_pin_to_bit_position[] = {
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
	PIN1_bp, // 15 PA1
	PIN2_bp, // 16 PA2
	PIN3_bp  // 17 PA3
};

/* Use this for accessing PINnCTRL register */
const uint8_t PROGMEM digital_pin_to_bit_mask[] = {
	
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
	PIN1_bm, // 15 PA1
	PIN2_bm, // 16 PA2
	PIN3_bm  // 17 PA3
};

const uint8_t PROGMEM digital_pin_to_timer[] = {
  NOT_ON_TIMER,  // 0 PC5/USART1_Rx
  NOT_ON_TIMER,  // 1 PC4/USART1_Tx
  NOT_ON_TIMER,  // 2 PA0
  TIMERB1,       // 3 PF5
  NOT_ON_TIMER,  // 4 PC6
  TIMERA0,       // 5 PB2
  TIMERB0,       // 6 PF4
  NOT_ON_TIMER,  // 7 PA1
  NOT_ON_TIMER,  // 8 PE3
  TIMERA0,       // 9 PB0
  TIMERA0,       // 10 PB1
  NOT_ON_TIMER,  // 11 PE0
  NOT_ON_TIMER,  // 12 PE1
  NOT_ON_TIMER,  // 13 PE2
  NOT_ON_TIMER,  // 14 PD0/AI0
  NOT_ON_TIMER,  // 15 PD1AI1
  NOT_ON_TIMER,  // 16 PD2/AI2
  NOT_ON_TIMER  // 17 PD3/AI3
};

const uint8_t PROGMEM analog_pin_to_channel[] = {
  3,
  2,
  1,
  0,
  12,
  13,
  5,
  4
};

#endif

extern const uint8_t analog_pin_to_channel[];
#define digitalPinToAnalogInput(p)  ((p < NUM_ANALOG_INPUTS) ? pgm_read_byte(analog_pin_to_channel + p) : NOT_A_PIN )


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
