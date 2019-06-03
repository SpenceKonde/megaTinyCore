/*
  Arduino.h - Main include file for the Arduino SDK
  Copyright (c) 2005-2013 Arduino Team.  All right reserved.

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
*/

#ifndef Arduino_h
#define Arduino_h

#include "api/ArduinoAPI.h"

#include <avr/pgmspace.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#ifdef __cplusplus
extern "C"{
#endif

/* Analog reference options */

/* Change in mega4809: two places to define analog reference
 - VREF peripheral defines internal reference
 - analog peripherals define internal/Vdd/external
*/

 // internal from VREF

 /* Values shifted to avoid clashing with ADC REFSEL defines
	Will shift back in analog_reference function
  */
#define INTERNAL0V55 (VREF_ADC0REFSEL_0V55_gc >> VREF_ADC0REFSEL_gp)
#define INTERNAL1V1 (VREF_ADC0REFSEL_1V1_gc >> VREF_ADC0REFSEL_gp)
#define INTERNAL2V5 (VREF_ADC0REFSEL_2V5_gc >> VREF_ADC0REFSEL_gp)
#define INTERNAL4V3 (VREF_ADC0REFSEL_4V34_gc >> VREF_ADC0REFSEL_gp)
#define INTERNAL1V5 (VREF_ADC0REFSEL_1V5_gc >> VREF_ADC0REFSEL_gp)

#define DEFAULT     INTERNAL0V55
#define INTERNAL    ADC_REFSEL_INTREF_gc
#define VDD         ADC_REFSEL_VDDREF_gc
#define EXTERNAL    ADC_REFSEL_VREFA_gc

#define VCC_5V0 2
#define VCC_3V3 1
#define VCC_1V8 0

#define interrupts() sei()
#define noInterrupts() cli()

// avr-libc defines _NOP() since 1.6.2
#ifndef _NOP
#define _NOP() do { __asm__ volatile ("nop"); } while (0)
#endif

/* Allows performing a correction on the CPU value using the signature row 
	values indicating oscillator error provided from the device manufacturer */
#define PERFORM_SIGROW_CORRECTION_F_CPU 0

/* Variable containing corrected F_CPU value, after checks for safe operating
	frequency vs supply voltage, oscillator fuse setting and MCLK divider.
	Also includes the correction from signature row values if above #define
	PERFORM_SIGROW_CORRECTION_F_CPU = 1 */
extern uint32_t F_CPU_CORRECTED;

uint16_t clockCyclesPerMicrosecondComp(uint32_t clk);
uint16_t clockCyclesPerMicrosecond();
unsigned long clockCyclesToMicroseconds(unsigned long cycles);
unsigned long microsecondsToClockCycles(unsigned long microseconds);

// Get the bit location within the hardware port of the given virtual pin.
// This comes from the pins_*.c file for the active board configuration.

extern const uint8_t PROGMEM digital_pin_to_port[];
extern const uint8_t PROGMEM digital_pin_to_bit_mask[];
extern const uint8_t PROGMEM digital_pin_to_bit_position[];
extern const uint8_t PROGMEM digital_pin_to_timer[];

// Get the bit location within the hardware port of the given virtual pin.
// This comes from the pins_*.c file for the active board configuration.
//
// These perform slightly better as macros compared to inline functions
//
#define NOT_A_PIN 255
#define NOT_A_PORT 255
#define NOT_AN_INTERRUPT 255

#define PA 0
#define PB 1
#define PC 2
#define PD 3
#define PE 4
#define PF 5
#define NUM_TOTAL_PORTS 6

#define NOT_ON_TIMER 0
#define TIMERA0 1
#define TIMERB0 2
#define TIMERB1 3
#define TIMERB2 4
#define TIMERB3 5

void setup_timers();
bool isDoubleBondedActive(uint8_t pin);

#define digitalPinToPort(pin) ( (pin < NUM_TOTAL_PINS) ? pgm_read_byte(digital_pin_to_port + pin) : NOT_A_PIN )
#define digitalPinToBitPosition(pin) ( (pin < NUM_TOTAL_PINS) ? pgm_read_byte(digital_pin_to_bit_position + pin) : NOT_A_PIN )
#define analogPinToBitPosition(pin) ( (pin < NUM_ANALOG_INPUTS) ? pgm_read_byte(digital_pin_to_bit_position + pin + ANALOG_INPUT_OFFSET) : NOT_A_PIN )
#define digitalPinToBitMask(pin) ( (pin < NUM_TOTAL_PINS) ? pgm_read_byte(digital_pin_to_bit_mask + pin) : NOT_A_PIN )
#define analogPinToBitMask(pin) ( (pin < NUM_ANALOG_INPUTS) ? pgm_read_byte(digital_pin_to_bit_mask + pin + ANALOG_INPUT_OFFSET) : NOT_A_PIN )
#define digitalPinToTimer(pin) ( (pin < NUM_TOTAL_PINS) ? pgm_read_byte(digital_pin_to_timer + pin) : NOT_ON_TIMER )

#define portToPortStruct(port) ( (port < NUM_TOTAL_PORTS) ? ((PORT_t *)&PORTA + port) : NULL)
#define digitalPinToPortStruct(pin) ( (pin < NUM_TOTAL_PINS) ? ((PORT_t *)&PORTA + digitalPinToPort(pin)) : NULL)
#define getPINnCTRLregister(port, bit_pos) ( ((port != NULL) && (bit_pos < NOT_A_PIN)) ? ((volatile uint8_t *)&(port->PIN0CTRL) + bit_pos) : NULL )
#define digitalPinToInterrupt(P) (P)

#define portOutputRegister(P) ( (volatile uint8_t *)( &portToPortStruct(P)->OUT ) )
#define portInputRegister(P) ( (volatile uint8_t *)( &portToPortStruct(P)->IN ) )
#define portModeRegister(P) ( (volatile uint8_t *)( &portToPortStruct(P)->DIR ) )

#ifdef __cplusplus
} // extern "C"
#endif

#ifdef __cplusplus
#include "UART.h"
#include "USBCore.h"
#include "CDC.h"
#include "MSC.h"
#ifdef UNO_WIFI_REV2_328MODE
#include <UNO_compat.h>
#endif
#if defined(HAVE_HWSERIAL0) && defined(HAVE_CDCSERIAL)
#error "Targets with both UART0 and CDC serial not supported"
#endif

#endif

#include "pins_arduino.h"
#endif
