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
#include "core_parts.h"
#include "api/ArduinoAPI.h"

#include <avr/pgmspace.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#ifdef __cplusplus
extern "C" {
#endif
  // Constant checks error handler
  void badArg(const char*) __attribute__((error("")));
  void badCall(const char*) __attribute__((error("")));
  inline __attribute__((always_inline)) void check_constant_pin(pin_size_t pin)
  {
    if(!__builtin_constant_p(pin))
      badArg("Fast digital pin must be a constant");
  }

/* ADC-related stuff */
/* With 2.3.0, we do the same thing as ATTinyCore and DxCore to specify
   that something is a channel number: set the high bit 1, since it is
   incredibly unlikely that a part with more than 127 digital pins will
   ever be supported by this core */

#define ADC_CH(ch)      (0x80 | (ch))

#if MEGATINYCORE_SERIES < 2
  /* ADC constants for 0/1-series */
  #define INTERNAL0V55    (VREF_ADC0REFSEL_0V55_gc >> VREF_ADC0REFSEL_gp)
  #define INTERNAL1V1     (VREF_ADC0REFSEL_1V1_gc >> VREF_ADC0REFSEL_gp)
  #define INTERNAL2V5     (VREF_ADC0REFSEL_2V5_gc >> VREF_ADC0REFSEL_gp)
  #define INTERNAL4V3     INTERNAL4V34
  #define INTERNAL4V34    (VREF_ADC0REFSEL_4V34_gc >> VREF_ADC0REFSEL_gp)
  #define INTERNAL1V5     (VREF_ADC0REFSEL_1V5_gc >> VREF_ADC0REFSEL_gp)

  #define DEFAULT         ADC_REFSEL_VDDREF_gc
  #define INTERNAL        ADC_REFSEL_INTREF_gc
  #define VDD             ADC_REFSEL_VDDREF_gc

  #if (defined(__AVR_ATtiny1614__) || defined(__AVR_ATtiny1616__) || defined(__AVR_ATtiny1617__) || defined(__AVR_ATtiny3216__) || defined(__AVR_ATtiny3217__) )
    #define EXTERNAL      ADC_REFSEL_VREFA_gc
  #endif

  #define ADC_TEMPERATURE ADC_CH(ADC_MUXPOS_TEMPSENSE_gc)
  #define ADC_INTREF      ADC_CH(ADC_MUXPOS_INTREF_gc)
  #define ADC_GROUND      ADC_CH(ADC_MUXPOS_GND_gc)
  #ifdef DAC0
    #define ADC_DAC0      ADC_CH(ADC_MUXPOS_DAC0_gc)
    #define ADC_DACREF0   ADC_DAC0
  #endif
  // DACREF1 and DACREF2 can only be measured with ADC1. ADC1 is not exposed by megaTinyCore.
  #define ADC_DEFAULT_SAMPLE_LENGTH 14
  #define ADC_ACC2        0x81
  #define ADC_ACC4        0x82
  #define ADC_ACC8        0x83
  #define ADC_ACC16       0x84
  #define ADC_ACC32       0x85
  #define ADC_ACC64       0x86

  #define getAnalogSampleDuration()   (ADC0.SAMPCTRL)

#else
  /* ADC constants for 2-series */
  #define VDD             (0) /* ADC_REFSEL_VDD_gc    */
  #define DEFAULT         VDD /* Gee, I really wish these were named differently - both names are horrendously generic and could mean lots of different things that should be distinguished. */
  #define EXTERNAL        (2) /* ADC_REFSEL_VREFA_gc  */
  #define INTERNAL1V024   (4) /* ADC_REFSEL_1024MV_gc */
  #define INTERNAL2V048   (5) /* ADC_REFSEL_2048MV_gc */
  #define INTERNAL2V5     (6) /* ADC_REFSEL_2500MV_gc */
  #define INTERNAL4V096   (7) /* ADC_REFSEL_4096MV_gc */
  #define INTERNAL4V1     INTERNAL4V096 /* Alias */

  #define AC_REF_1V024    (VREF_AC0REFSEL_1V024_gc)
  #define AC_REF_2V048    (VREF_AC0REFSEL_2V048_gc)
  #define AC_REF_2V5      (VREF_AC0REFSEL_2V5_gc)
  #define AC_REF_4V096    (VREF_AC0REFSEL_4V096_gc)
  #define AC_REF_VDD      (VREF_AC0REFSEL_AVDD_gc)
  #define AC_REF_4V1      AC_REF_4V096/* Alias */

  #define ADC_TEMPERATURE ADC_CH(ADC_MUXPOS_TEMPSENSE_gc)
  #define ADC_GROUND      ADC_CH(ADC_MUXPOS_GND_gc)
  #define ADC_DACREF0     ADC_CH(ADC_MUXPOS_DACREF0_gc)
  #define ADC_DAC0        ADC_DACREF0 /* for compatibility, since on tinyAVR 0/1-seies, the DAC0 voltage is also AC0 DACREF if used */
  #define ADC_VDDDIV10    ADC_CH(ADC_MUXPOS_VDDDIV10_gc)

  /* >= 1us - can't use clockcycles per microsecond from timers.h because
  this needs to always round up */
  #define TIMEBASE_1US (((F_CPU + 999999UL)/1000000UL) << ADC_TIMEBASE_gp)

  #define ADC_DEFAULT_SAMPLE_LENGTH 15
  #define ADC_ACC2        0x81
  #define ADC_ACC4        0x82
  #define ADC_ACC8        0x83
  #define ADC_ACC16       0x84
  #define ADC_ACC32       0x85
  #define ADC_ACC64       0x86
  #define ADC_ACC128      0x87
  #define ADC_ACC256      0x88
  #define ADC_ACC512      0x89
  #define ADC_ACC1024     0x8A

  #define getAnalogSampleDuration()   (ADC0.CTRLE)

#endif

/* Errors in analogReadEnh and analogReadDiff are large negative numbers,
 * since it's a signed long  returned, and  a raw maximally accumulated
 * differential reading could be a huge negative number. Largest negative
 * possible is -2 147 483 648; It would be hard to tell the difference between
 * that and -2147483647, or -2147483646 and remember which is which,
 * so they start at -2100000000.
 * Errors for normal analogRead are small negative numbers because
 * analogRead should never return a negative. Neither should analogReadEnh
 * but I decided to have only 2 sets of ADC errors, not three.  */

#define ADC_ERROR_BAD_PIN_OR_CHANNEL                -32765
#define ADC_ERROR_DISABLED                          -32767
#define ADC_ERROR_BUSY                              -32766
#define ADC_ENH_ERROR_BAD_PIN_OR_CHANNEL       -2100000000
// positive channel is not (0x80 | valid_channel) nor a digital pin number
// referring to a pin with analog input.
#define ADC_ENH_ERROR_BUSY                     -2100000001
// The ADC is currently performing another conversion in the background (either
// in free-running mode or a long-running burst conversion; taking a burst
// accumulated reading and then calling a specified function when the result
// was finally ready may be supported in a future version.
#define ADC_ENH_ERROR_INVALID_SAMPLE_LENGTH    -2100000002
// SAMPLEN can be specified when calling analogReadEnh; an invalid value was
// specified. The maximum depends on the hardware.
#define ADC_ENH_ERROR_RES_TOO_LOW              -2100000003
// analogReadEnh() must not be called with a resolution lower than 8-bits.
// you can right-shift as well as the library can.
#define ADC_ENH_ERROR_RES_TOO_HIGH             -2100000004
// Only resonlutions that can be generated through accumulator oversample
// + decimation are supported, maximum is 13, 15, or 17 bits. This will
// also be returned if a larger raw accumulated result is requested.
#define ADC_DIFF_ERROR_BAD_NEG_PIN             -2100000005
// Analog pin given as negative pin is not a valid negative mux pin
#define ADC_ENH_ERROR_NOT_DIFF_ADC             -2100000006
// analogReadDiff() called from a part without a differential ADC;
// Never actually returned, because we give compile error here
#define ADC_ENH_ERROR_DISABLED                 -2100000007
// The ADC is not currently enabled.
#define ADC_ERROR_INVALID_CLOCK                     -32764
// Returned by analogClockSpeed if the value in the register is currently unknown, or if an invalid frequency is requested.




#if (!defined(TCB_CLKSEL2_bm))
  // This means it's a tinyAVR 0/1-series, or a megaAVR 0-series.
  // Their TCB_CLKSEL enums use different names for the clock settings, for reasons unclear.
  // To align with the future, we use the Dx-series names for these.
  #define TCB_CLKSEL_DIV2_gc TCB_CLKSEL_CLKDIV2_gc
  #define TCB_CLKSEL_DIV1_gc TCB_CLKSEL_CLKDIV1_gc
#endif

#define VCC_5V0 2
#define VCC_3V3 1
#define VCC_1V8 0

#define interrupts() sei()
#define noInterrupts() cli()


// NON-STANDARD API


void init_ADC0(void);
void init_ADC1(void);
void init_clock(void);
void init_millis();
void stop_millis();
void restart_millis();
void set_millis(uint32_t newmillis);
void init_timers();
void init_TCA0();
void init_TCD0();
int32_t analogReadEnh( uint8_t pin,              uint8_t res, uint8_t gain);
int32_t analogReadDiff(uint8_t pos, uint8_t neg, uint8_t res, uint8_t gain);
int16_t analogClockSpeed(int16_t frequency, uint8_t options);
void ADCPowerOptions(uint8_t options); /* 2-series only */
// Peripheral takeover
// These will remove things controlled by
// these timers from analogWrite()/turnOffPWM()
// 0x40 - TCD0, 0x10 - TCA0
void takeOverTCA0();
void takeOverTCD0();




// avr-libc defines _NOP() since 1.6.2
#ifndef _NOP
#define _NOP() do { __asm__ volatile ("nop"); } while (0)
#endif

/* Allows performing a correction on the CPU value using the signature row
  values indicating oscillator error provided from the device manufacturer */
#define PERFORM_SIGROW_CORRECTION_F_CPU 0

// We declared it here... we never defined it anywhere, and I'll be damned if I know what it is supposed to do....
// uint16_t clockCyclesPerMicrosecondComp(uint32_t clk);

uint16_t clockCyclesPerMicrosecond();
unsigned long clockCyclesToMicroseconds(unsigned long cycles);
unsigned long microsecondsToClockCycles(unsigned long microseconds);

// Copies of above for internal use, and for the really exotic use cases that want this instead of system clocks (basically never in user-land)
uint16_t millisClockCyclesPerMicrosecond();
unsigned long millisClockCyclesToMicroseconds(unsigned long cycles);
unsigned long microsecondsToMillisClockCycles(unsigned long microseconds);
__attribute__ ((noinline)) void _delayMicroseconds(unsigned int us);
// Get the bit location within the hardware port of the given virtual pin.
// This comes from the pins_*.c file for the active board configuration.

extern const uint8_t digital_pin_to_port[];
extern const uint8_t digital_pin_to_bit_mask[];
extern const uint8_t digital_pin_to_bit_position[];
extern const uint8_t digital_pin_to_timer[];

// Get the bit location within the hardware port of the given virtual pin.
// This comes from the pins_*.c file for the active board configuration.
//
// These perform slightly better as macros compared to inline functions

#define digitalPinToPort(pin)               ((pin  < NUM_TOTAL_PINS)  ? digital_pin_to_port[pin]          : NOT_A_PIN )
#define digitalPinToBitPosition(pin)        ((pin  < NUM_TOTAL_PINS)  ? digital_pin_to_bit_position[pin]  : NOT_A_PIN )
#define digitalPinToBitMask(pin)            ((pin  < NUM_TOTAL_PINS)  ? digital_pin_to_bit_mask[pin]      : NOT_A_PIN )
#define digitalPinToTimer(pin)              ((pin  < NUM_TOTAL_PINS)  ? digital_pin_to_timer[pin]         : NOT_ON_TIMER )
#define portToPortStruct(port)              ((port < NUM_TOTAL_PORTS) ? ((PORT_t *) &PORTA + port)                  : NULL)
#define digitalPinToPortStruct(pin)         ((pin  < NUM_TOTAL_PINS)  ? ((PORT_t *) &PORTA + digitalPinToPort(pin)) : NULL)
#define analogPinToBitPosition(pin)         ((digitalPinToAnalogInput(pin) != NOT_A_PIN) ? digital_pin_to_bit_position[pin] : NOT_A_PIN )
#define analogPinToBitMask(pin)             ((digitalPinToAnalogInput(pin) != NOT_A_PIN) ? digital_pin_to_bit_mask[pin]     : NOT_A_PIN )
#define getPINnCTRLregister(port, bit_pos)  (((port != NULL) && (bit_pos < NOT_A_PIN)) ? ((volatile uint8_t *)&(port->PIN0CTRL) + bit_pos) : NULL )

#define digitalPinToInterrupt(P) (P)

#define portOutputRegister(P) ((volatile uint8_t *)(&portToPortStruct(P)->OUT))
#define portInputRegister(P)  ((volatile uint8_t *)(&portToPortStruct(P)->IN ))
#define portModeRegister(P)   ((volatile uint8_t *)(&portToPortStruct(P)->DIR))


#ifdef __cplusplus
} // extern "C"
#endif

#ifdef __cplusplus
  #include "UART.h"
  int32_t analogReadEnh( uint8_t pin,              uint8_t res = ADC_NATIVE_RESOLUTION, uint8_t gain = 0);
  int32_t analogReadDiff(uint8_t pos, uint8_t neg, uint8_t res = ADC_NATIVE_RESOLUTION, uint8_t gain = 0);
  int16_t analogClockSpeed(int16_t frequency = 0, uint8_t options = 0);
#endif

// Include the variants
#include "pins_arduino.h"

// Based on those, some ugly formulae for "smart-pin" defines that follow the mux regs around:


#ifdef PIN_WIRE_SCL_PINSWAP_1
  #define SDA_NOW ((uint8_t) (PORTMUX.CTRLB & PORTMUX_TWI0_bm ? PIN_WIRE_SDA_PINSWAP_1 : PIN_WIRE_SDA))
  #define SCL_NOW ((uint8_t) (PORTMUX.CTRLB & PORTMUX_TWI0_bm ? PIN_WIRE_SCL_PINSWAP_1 : PIN_WIRE_SCL))
  static const uint8_t SDA_ALT = PIN_WIRE_SDA_PINSWAP_1;
  static const uint8_t SCL_ALT = PIN_WIRE_SCL_PINSWAP_1;
#endif
static const uint8_t SDA = PIN_WIRE_SDA;
static const uint8_t SCL = PIN_WIRE_SCL;


#ifdef PIN_SPI_SCK_PINSWAP_1
  #define SS_NOW    ((uint8_t) (PORTMUX.CTRLB & PORTMUX_SPI0_bm ? PIN_SPI_SS_PINSWAP_1    : PIN_SPI_SS))
  #define MOSI_NOW  ((uint8_t) (PORTMUX.CTRLB & PORTMUX_SPI0_bm ? PIN_SPI_MOSI_PINSWAP_1  : PIN_SPI_MOSI))
  #define MISO_NOW  ((uint8_t) (PORTMUX.CTRLB & PORTMUX_SPI0_bm ? PIN_SPI_MISO_PINSWAP_1  : PIN_SPI_MISO))
  #define SCK_NOW   ((uint8_t) (PORTMUX.CTRLB & PORTMUX_SPI0_bm ? PIN_SPI_SCK_PINSWAP_1   : PIN_SPI_SCK))
  static const uint8_t SS_ALT   = PIN_SPI_SS_PINSWAP_1;
  static const uint8_t MOSI_ALT = PIN_SPI_MOSI_PINSWAP_1;
  static const uint8_t MISO_ALT = PIN_SPI_MISO_PINSWAP_1;
  static const uint8_t SCK_ALT  = PIN_SPI_SCK_PINSWAP_1;
#endif
static const uint8_t SS   = PIN_SPI_SS;
static const uint8_t MOSI = PIN_SPI_MOSI;
static const uint8_t MISO = PIN_SPI_MISO;
static const uint8_t SCK  = PIN_SPI_SCK;



#define CORE_HAS_FASTIO 1
#define CORE_HAS_OPENDRAIN 1
// #define CORE_HAS_PINCONFIG 1

#if (MEGATINYCORE_SERIES == 2)
  // if analogReadEnh() supplied, this is defined as 1
  #define CORE_HAS_ANALOG_ENH 1
  // if analogReadDiff() supplied, this is defined as 1
  #define CORE_HAS_ANALOG_DIFF 1
#else
  #define CORE_HAS_ANALOG_ENH 1
  // if analogReadDiff() supplied, this is defined as 1
  // #define CORE_HAS_ANALOG_DIFF 1
#endif


#ifndef SUPPORT_LONG_TONES
  #if (PROGMEM_SIZE > 8192)
    #define SUPPORT_LONG_TONES 1
  #endif
#endif


#endif
