/*
 * Refer to ptc.h file for copyright, changelog, usage and license information
 */

#pragma once
#ifndef PTC_TOUCH_IO_H
#define PTC_TOUCH_IO_H

#ifdef __cplusplus
extern "C" {
#endif

#include "ptc_types.h"

#if defined (__PTC_Tiny__)

typedef struct PTC_struct {
  register8_t CTRLA;  /* Control A */
  register8_t CTRLB;  /* Control B */
  register8_t CTRLC;  /* Control C */
  register8_t CTRLD;  /* Control D */
  register8_t CTRLE;  /* Control E */
  register8_t SAMPCTRL;  /* Sample Control */
  register8_t MUXPOS;  /* Positive mux input */
  register8_t reserved_0;
  register8_t COMMAND;  /* Command */
  register8_t EVCTRL;  /* Event Control */
  register8_t INTCTRL;  /* Interrupt Control */
  register8_t INTFLAGS;  /* Interrupt Flags */
  register8_t DBGCTRL;  /* Debug Control */
  register8_t TEMP;  /* Temporary Data */
  _WORDREGISTER(reserved_1);
  _WORDREGISTER(RES);  /* ADC Accumulator Result */
  _WORDREGISTER(WINLT);  /* Window comparator low threshold */
  _WORDREGISTER(WINHT);  /* Window comparator high threshold */
  register8_t CALIB;  /* Calibration */
  register8_t reserved_2;     /* +0x17 */
  /* Any register Name below this point is based on speculation */
  register8_t CTRLP;          /* +0x18 OR with 0x03 to enable, written 0x28 for Selfcap */
  register8_t RSEL;           /* +0x19 */
  _WORDREGISTER(COMP);        /* +0x1A */
  register8_t AGAIN;          /* +0x1C */
  register8_t reserved_3;     /* +0x1D */
  register8_t SHIELD;         /* +0x1E if enabled, written 0x86, otherwise 0x00 */
  register8_t reserved_4;     /* +0x1F */
  _WORDREGISTER(RES_PTC);     /* +0x20 Some Result, written by PTC. Seems to be RES, but left-shifted, or result from PTC accumulated by ADC */
  _WORDREGISTER(PIN_OVR);     /* +0x22 all X and Y pins OR'd together at init. Pin Function Overwrite Probably*/
  _WORDREGISTER(reserved_5);  /* +0x24 */
  _WORDREGISTER(XBM);         /* +0x26 amount of writeable bits depends on chip-die family */
  _WORDREGISTER(reserved_6);  /* +0x28 */
  _WORDREGISTER(YBM);         /* +0x2A e.g. 0x3FFF (15 pins) for 1614 with only 6 PTC pins */
  _WORDREGISTER(reserved_7);;  /* +0x2C */
} PTC_t;

#define PTC     (*(PTC_t *) 0x0600) /* Analog to Digital Converter */

#elif defined (__PTC_DA__)

typedef struct PTC_struct {
  /* Any register Name below this point is based on speculation */
  register8_t CTRLA;          /* 0xC0 [0x81] Control A (RUNSTDBY, ENABLE)*/
  register8_t CTRL_SC;        /* 0xC1 [0x07] 0x01: Selfcap_EN,  */
  register8_t CTRLC;          /* 0xC2 [0x87] Control C (Prescaler | 0x80 (SampCap?))*/
  register8_t CTRLD;          /* 0xC3 [0x6F] Control D (Max: 0x6F) Is always OR'd with 0x20 or limited to 0x2F */
  register8_t SAMPCTRL;       /* 0xC4 [0x7F] SAMPCTRL (Max: 0x7F) */
  register8_t SAMPDLY;        /* 0xC5 [0x7F] 0~15 Inserted ADC clocks */
  register8_t reserved_0;     /* 0xC6 [0xFF] unused */
  register8_t CTRLB;          /* 0xC7 [0x07] SAMPNUM */
  register8_t CTRLE;          /* 0xC8 [0x07] WINCM probably */
  register8_t RSEL;           /* 0xC9 [0x77] Resistor Setting */
  register8_t CC_EN;          /* 0xCA [0x01] Enable Compensation Caps? */
  register8_t CTRL_BOOST;     /* 0xCB [0xFF] set to 0x09 if NODE_SELFCAP_SHIELD_2L */
  _DWORDREGISTER(reserved_1); /* 0xCC [0x00] unwritable */
  register8_t COMMAND;        /* 0xD0 [0x01] Command */
  register8_t EVCTRL;         /* 0xD1 [0x01] Event Control */
  register8_t INTCTRL;        /* 0xD2 [0x07] Interrupt Control */
  register8_t INTFLAGS;       /* 0xD3 [0x07] Interrupt Flags */
  register8_t reserved_2;     /* 0xD4 [0x02] unknown */
  register8_t DEBUGCTRL;      /* 0xD5 [0x01] DBGRUN */
  _WORDREGISTER(reserved_3);  /* 0xD6 [0x00] unwritable */
  _WORDREGISTER(reserved_4);  /* 0xD8 [0x00] some other result */
  _WORDREGISTER(RES);         /* 0xDA [0x00] ADC Result register */
  _WORDREGISTER(WINLT);       /* 0xDC [0xFF] Window comparator low threshold */
  _WORDREGISTER(WINHT);       /* 0xDD [0xFF] Window comparator high threshold */
  register8_t CTRL_SHIELD;    /* 0xE0 [0x00] Strobing Register, changes between Shield and Mutual ?*/
  register8_t SOME_RES;       /* 0xE1 [0x81] some unknown result value */
  _WORDREGISTER(reserved_5);  /* 0xE2 [0x00] unwritable */
  _WORDREGISTER(COMP);        /* 0xE4 [0x3FF] Compensation */
  register8_t AGAIN;          /* 0xE6 [0x1F] Analog Gain */
  register8_t reserved_6;     /* 0xE7 [0x00] unwritable */
  register8_t XBM[8];         /* 0xE8 X Channel bitmask */
  register8_t YBM[8];         /* 0xF0 Y Channel bitmask */
  register8_t XBM_4P[8];      /* 0xF8 X Channel bm when using Boost mode, unused */
} PTC_t;

#define PTC     (*(PTC_t *) 0x07C0) /* Analog to Digital Converter */

#else
  #warning "Neither __PTC_Tiny__ nor __PTC_DA__ defined"
#endif




#define RSEL_MAX RSEL_VAL_200
#define PRSC_MAX ADC_PRESC_DIV256_gc


#if F_CPU  >= 12000000        // 16 MHz / 16 = 1.0 MHz,  20 MHz / 16 = 1.25 MHz
  #define PTC_PRESC_DEFAULT   ADC_PRESC_DIV16_gc
#elif F_CPU  >=  6000000      //  8 MHz /  8 = 1.0 MHz,  10 MHz /  8 = 1.25 MHz
  #define PTC_PRESC_DEFAULT   ADC_PRESC_DIV8_gc
#elif F_CPU  >=  3000000      //  4 MHz /  4 = 1.0 MHz,   5 MHz /  4 = 1.25 MHz
  #define PTC_PRESC_DEFAULT   ADC_PRESC_DIV4_gc
#else                         //  1 MHz /  2 = 500 kHz - the lowest setting
  #define PTC_PRESC_DEFAULT   ADC_PRESC_DIV2_gc
#endif



#define PIN_TO_PTC(__pin__) (((__pin__) < NUM_TOTAL_PINS ) ? digital_pin_to_ptc_bm[__pin__] : 0x00)
const ptc_ch_bm_t digital_pin_to_ptc_bm [] = {
#if (__PTC_Pincount__ == 6)
  0x01 << 0,  //PA4
  0x01 << 1,  //PA5
  0x01 << 2,  //PA6
  0x01 << 3,  //PA7
  0x00,       //PB3
  0x00,       //PB2
  0x01 << 4,  //PB1
  0x01 << 5,  //PB0
  0x00,       //PA1
  0x00,       //PA2
  0x00,       //PA3
  0x00        //PA0
#elif  (__PTC_Pincount__ == 12)
  0x01 << 0,  // 0  PA4
  0x01 << 1,  // 1  PA5
  0x01 << 2,  // 2  PA6
  0x01 << 3,  // 3  PA7
  0x01 << 12, // 4  PB5
  0x01 << 13, // 5  PB4
  0x00,       // 6  PB3
  0x00,       // 7  PB2
  0x01 << 4,  // 8  PB1
  // Right side, bottom to top
  0x01 << 5,  // 9  PB0
  0x01 << 6,  // 10 PC0
  0x01 << 7,  // 11 PC1
  0x01 << 8,  // 12 PC2
  0x01 << 9,  // 13 PC3
  0x00,       // 14 PA1
  0x00,       // 15 PA2
  0x00,       // 16 PA3
  0x00        // 17 PA0
#elif  (__PTC_Pincount__ == 14)
  0x01 << 0,  // 0  PA4
  0x01 << 1,  // 1  PA5
  0x01 << 2,  // 2  PA6
  0x01 << 3,  // 3  PA7
  0x00,       // 4  PB7
  0x00,       // 5  PB6
  0x01 << 12, // 6  PB5
  0x01 << 13, // 7  PB4
  0x00,       // 8  PB3
  0x00,       // 9  PB2
  0x01 << 4,  // 10 PB1
  // Right side, bottom to top
  0x01 << 5,  // 11 PB0
  0x01 << 6,  // 12 PC0
  0x01 << 7,  // 13 PC1
  0x01 << 8,  // 14 PC2
  0x01 << 9,  // 15 PC3
  0x01 << 10, // 16 PC4
  0x01 << 11, // 17 PC5
  0x00,       // 18 PA1
  0x00,       // 19 PA2
  0x00,       // 20 PA3
  0x00        // 21 PA0
#elif  (__PTC_Pincount__ == 17)  // DA (ToDo)
  0x01ULL <<  0,  //  0 PA0
  0x01ULL <<  1,  //  1 PA1
  0x01ULL <<  2,  //  2 PA2/SDA
  0x01ULL <<  3,  //  3 PA3/SCL
  0x01ULL <<  4,  //  4 PA4/MOSI
  0x01ULL <<  5,  //  5 PA5/MISO
  0x01ULL <<  6,  //  6 PA6/SCK
  0x01ULL <<  7,  //  7 PA7/SS/CLKOUT
  0x00ULL,        //  8 PC0/USART1_Tx
  0x00ULL,        //  9 PC1/USART1_Rx
  0x00ULL,        // 10 PC2
  0x00ULL,        // 11 PC3
  0x01ULL << 16,  // 12 PD0/AIN0
  0x01ULL << 17,  // 13 PD1/AIN1
  0x01ULL << 18,  // 14 PD2/AIN2
  0x01ULL << 19,  // 15 PD3/AIN3
  0x01ULL << 20,  // 16 PD4/AIN4
  0x01ULL << 21,  // 17 PD5/AIN5
  0x01ULL << 22,  // 18 PD6/AIN6
  0x01ULL << 23,  // 19 PD7/AIN7/AREF
  0x01ULL << 32,  // 20 PF0/USART2_Tx/TOSC1
  0x01ULL << 33,  // 21 PF1/USART2_Rx/TOSC2
#elif  (__PTC_Pincount__ == 21)
  0x01ULL <<  0,  //  0 PA0
  0x01ULL <<  1,  //  1 PA1
  0x01ULL <<  2,  //  2 PA2/SDA
  0x01ULL <<  3,  //  3 PA3/SCL
  0x01ULL <<  4,  //  4 PA4/MOSI
  0x01ULL <<  5,  //  5 PA5/MISO
  0x01ULL <<  6,  //  6 PA6/SCK
  0x01ULL <<  7,  //  7 PA7/SS/CLKOUT
  0x00ULL,        //  8 PC0/USART1_Tx
  0x00ULL,        //  9 PC1/USART1_Rx
  0x00ULL,        // 10 PC2
  0x00ULL,        // 11 PC3
  0x01ULL << 16,  // 12 PD0/AIN0
  0x01ULL << 17,  // 13 PD1/AIN1
  0x01ULL << 18,  // 14 PD2/AIN2
  0x01ULL << 19,  // 15 PD3/AIN3
  0x01ULL << 20,  // 16 PD4/AIN4
  0x01ULL << 21,  // 17 PD5/AIN5
  0x01ULL << 22,  // 18 PD6/AIN6
  0x01ULL << 23,  // 19 PD7/AIN7/AREF
  0x01ULL << 32,  // 20 PF0/USART2_Tx/TOSC1
  0x01ULL << 33,  // 21 PF1/USART2_Rx/TOSC2
  0x01ULL << 34,  // 22 PF2/AIN12
  0x01ULL << 35,  // 23 PF3/AIN13
  0x01ULL << 36,  // 24 PF4/AIN14/TCB0 PWM
  0x01ULL << 37,  // 25 PF5/AIN15/TCB1 PWM
#elif (__PTC_Pincount__ == 31)
  0x01ULL <<  0,  //  0 PA0
  0x01ULL <<  1,  //  1 PA1
  0x01ULL <<  2,  //  2 PA2/SDA
  0x01ULL <<  3,  //  3 PA3/SCL
  0x01ULL <<  4,  //  4 PA4/MOSI
  0x01ULL <<  5,  //  5 PA5/MISO
  0x01ULL <<  6,  //  6 PA6/SCK
  0x01ULL <<  7,  //  7 PA7/SS/CLKOUT/LED_BUILTIN
  0x01ULL <<  8,  //  8 PB0/USART3_Tx
  0x01ULL <<  9,  //  9 PB1/USART3_Rx
  0x01ULL << 10,  // 10 PB2
  0x01ULL << 11,  // 11 PB3
  0x01ULL << 12,  // 12 PB4/(TCB2 PWM)
  0x01ULL << 13,  // 13 PB5
  0x00ULL,        // 14 PC0/USART1_Tx
  0x00ULL,        // 15 PC1/USART1_Rx
  0x00ULL,        // 16 PC2
  0x00ULL,        // 17 PC3
  0x00ULL,        // 18 PC4
  0x00ULL,        // 19 PC5
  0x00ULL,        // 20 PC6
  0x00ULL,        // 21 PC7
  0x01ULL << 16,  // 22 PD0/AIN0
  0x01ULL << 17,  // 23 PD1/AIN1
  0x01ULL << 18,  // 24 PD2/AIN2
  0x01ULL << 19,  // 25 PD3/AIN3
  0x01ULL << 20,  // 26 PD4/AIN4
  0x01ULL << 21,  // 27 PD5/AIN5
  0x01ULL << 22,  // 28 PD6/AIN6
  0x01ULL << 23,  // 29 PD7/AIN7/AREF
  0x01ULL << 24,  // 30 PE0/AIN8
  0x01ULL << 25,  // 31 PE1/AIN9
  0x01ULL << 26,  // 32 PE2/AIN10
  0x01ULL << 27,  // 33 PE3/AIN11
  0x01ULL << 32,  // 34 PF0/USART2_Tx/TOSC1
  0x01ULL << 33,  // 35 PF1/USART2_Rx/TOSC2
  0x01ULL << 34,  // 36 PF2/AIN12
  0x01ULL << 35,  // 37 PF3/AIN13
  0x01ULL << 36,  // 38 PF4/AIN14
  0x01ULL << 37,  // 39 PF5/AIN15
#elif  (__PTC_Pincount__ == 47)
  0x01ULL <<  0,  //  0 PA0
  0x01ULL <<  1,  //  1 PA1
  0x01ULL <<  2,  //  2 PA2/SDA
  0x01ULL <<  3,  //  3 PA3/SCL
  0x01ULL <<  4,  //  4 PA4/MOSI
  0x01ULL <<  5,  //  5 PA5/MISO
  0x01ULL <<  6,  //  6 PA6/SCK
  0x01ULL <<  7,  //  7 PA7/SS/CLKOUT/LED_BUILTIN
  0x01ULL <<  8,  //  8 PB0/USART3_Tx
  0x01ULL <<  9,  //  9 PB1/USART3_Rx
  0x01ULL << 10,  // 10 PB2
  0x01ULL << 11,  // 11 PB3
  0x01ULL << 12,  // 12 PB4/(TCB2 PWM)
  0x01ULL << 13,  // 13 PB5
  0x01ULL << 14,  // 14 PB6
  0x01ULL << 15,  // 15 PB7
  0x00ULL,        // 16 PC0/USART1_Tx
  0x00ULL,        // 17 PC1/USART1_Rx
  0x00ULL,        // 18 PC2
  0x00ULL,        // 19 PC3
  0x00ULL,        // 20 PC4
  0x00ULL,        // 21 PC5
  0x00ULL,        // 22 PC6
  0x00ULL,        // 23 PC7
  0x01ULL << 16,  // 24 PD0/AIN0
  0x01ULL << 17,  // 25 PD1/AIN1
  0x01ULL << 18,  // 26 PD2/AIN2
  0x01ULL << 19,  // 27 PD3/AIN3
  0x01ULL << 20,  // 28 PD4/AIN4
  0x01ULL << 21,  // 29 PD5/AIN5
  0x01ULL << 22,  // 30 PD6/AIN6
  0x01ULL << 23,  // 31 PD7/AIN7/AREF
  0x01ULL << 24,  // 32 PE0/AIN8
  0x01ULL << 25,  // 33 PE1/AIN9
  0x01ULL << 26,  // 34 PE2/AIN10
  0x01ULL << 27,  // 35 PE3/AIN11
  0x01ULL << 28,  // 36 PE4
  0x01ULL << 29,  // 37 PE5
  0x01ULL << 30,  // 38 PE6
  0x01ULL << 31,  // 39 PE7
  0x01ULL << 32,  // 40 PF0/USART2_Tx/TOSC1
  0x01ULL << 33,  // 41 PF1/USART2_Rx/TOSC2
  0x01ULL << 34,  // 42 PF2/AIN12
  0x01ULL << 35,  // 43 PF3/AIN13
  0x01ULL << 36,  // 44 PF4/AIN14
  0x01ULL << 37,  // 45 PF5/AIN15
  0x01ULL << 40,  // 46 PG0
  0x01ULL << 41,  // 47 PG1
  0x01ULL << 42,  // 48 PG2
  0x01ULL << 43,  // 49 PG3
  0x01ULL << 44,  // 50 PG4
  0x01ULL << 45,  // 51 PG5
  0x01ULL << 46,  // 52 PG6
  0x01ULL << 47,  // 53 PG7
#endif
};


#if defined (PORTA)
  #define PORTA_ISC(_pin_)  ((0x20 * 0) + 0x10 + _pin_)
#else
  #define PORTA_ISC(_pin_)  0x00
#endif
#if defined (PORTB)
  #define PORTB_ISC(_pin_)  ((0x20 * 1) + 0x10 + _pin_)
#else
  #define PORTB_ISC(_pin_)  0x00
#endif
#if defined (PORTC)
  #define PORTC_ISC(_pin_)  ((0x20 * 2) + 0x10 + _pin_)
#else
  #define PORTC_ISC(_pin_)  0x00
#endif
#if defined (PORTD)
  #define PORTD_ISC(_pin_)  ((0x20 * 3) + 0x10 + _pin_)
#else
  #define PORTD_ISC(_pin_)  0x00
#endif
#if defined (PORTE)
  #define PORTE_ISC(_pin_)  ((0x20 * 4) + 0x10 + _pin_)
#else
  #define PORTE_ISC(_pin_)  0x00
#endif
#if defined (PORTF)
  #define PORTF_ISC(_pin_)  ((0x20 * 5) + 0x10 + _pin_)
#else
  #define PORTF_ISC(_pin_)  0x00
#endif
#if defined (PORTG)
  #define PORTG_ISC(_pin_)  ((0x20 * 6) + 0x10 + _pin_)
#else
  #define PORTG_ISC(_pin_)  0x00
#endif


// lookup-table to quickly disable input and pull-up
const uint8_t ptc_ch_to_pin [] = {
#if (__PTC_Pincount__ <= 14)
  PORTA_ISC(4),
  PORTA_ISC(5),
  PORTA_ISC(6),
  PORTA_ISC(7),
  PORTB_ISC(1),   /* X4 / Y4 */
  PORTA_ISC(0),
#if (__PTC_Pincount__ == 12 || __PTC_Pincount__ == 14)
  PORTC_ISC(0),
  PORTC_ISC(1),
  PORTC_ISC(2),    /* X8 / Y8 */
  PORTC_ISC(3),
  PORTC_ISC(4),   // 20 pin parts: writing to this location will have no effect, but likely pre-filtered by PIN_TO_PTC anyway
  PORTC_ISC(5),   // 20 pin parts: writing to this location will have no effect, but likely pre-filtered by PIN_TO_PTC anyway
  PORTB_ISC(5),    /* X12 / Y12 */
  PORTB_ISC(6),
#endif
#else
  PORTA_ISC(0),
  PORTA_ISC(1),
  PORTA_ISC(2),
  PORTA_ISC(3),
  PORTA_ISC(4),
  PORTA_ISC(5),
  PORTA_ISC(6),
  PORTA_ISC(7),   // X7

  PORTB_ISC(0),
  PORTB_ISC(1),
  PORTB_ISC(2),
  PORTB_ISC(3),
  PORTB_ISC(4),
  PORTB_ISC(5),
  PORTB_ISC(6),
  PORTB_ISC(7),   // X15

  PORTD_ISC(0),
  PORTD_ISC(1),
  PORTD_ISC(2),
  PORTD_ISC(3),
  PORTD_ISC(4),
  PORTD_ISC(5),
  PORTD_ISC(6),
  PORTD_ISC(7),   // X23

  PORTE_ISC(0),
  PORTE_ISC(1),
  PORTE_ISC(2),
  PORTE_ISC(3),
  PORTE_ISC(4),
  PORTE_ISC(5),
  PORTE_ISC(6),
  PORTE_ISC(7),   // X31

  PORTF_ISC(0),
  PORTF_ISC(1),
  PORTF_ISC(2),
  PORTF_ISC(3),
  PORTF_ISC(4),
  PORTF_ISC(5),   // X37
  0x00,
  0x00,

  PORTG_ISC(0),
  PORTG_ISC(1),
  PORTG_ISC(2),
  PORTG_ISC(3),
  PORTG_ISC(4),
  PORTG_ISC(5),
  PORTG_ISC(6),
  PORTG_ISC(7),   // X47
#endif
};


#ifdef __cplusplus
}
#endif


#endif /* PTC_TOUCH.H_H_ */
