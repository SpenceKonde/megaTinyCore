/*
 * Copyright (C) 2025, Microchip Technology Inc. and its subsidiaries ("Microchip")
 * All rights reserved.
 *
 * This software is developed by Microchip Technology Inc. and its subsidiaries ("Microchip").
 *
 * Redistribution and use in source and binary forms, with or without modification, are
 * permitted provided that the following conditions are met:
 *
 *     1. Redistributions of source code must retain the above copyright notice, this list of
 *        conditions and the following disclaimer.
 *
 *     2. Redistributions in binary form must reproduce the above copyright notice, this list
 *        of conditions and the following disclaimer in the documentation and/or other
 *        materials provided with the distribution. Publication is not required when
 *        this file is used in an embedded application.
 *
 *     3. Microchip's name may not be used to endorse or promote products derived from this
 *        software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY MICROCHIP "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL MICROCHIP BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING BUT NOT LIMITED TO
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWSOEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _AVR_IO_H_
#  error "Include <avr/io.h> instead of this file."
#endif

#ifndef _AVR_IOXXX_H_
#  define _AVR_IOXXX_H_ "iotn806.h"
#else
#  error "Attempt to include more than one <avr/ioXXX.h> file."
#endif

#ifndef _AVR_ATTINY806_H_INCLUDED
#define _AVR_ATTINY806_H_INCLUDED

/* Ungrouped common registers */
#define CCP  _SFR_MEM8(0x0034)  /* Configuration Change Protection */
#define SREG  _SFR_MEM8(0x003F)  /* Status Register */


#define GPIOR0  _SFR_MEM8(0x001C)  /* General Purpose IO Register 0 */
#define GPIOR1  _SFR_MEM8(0x001D)  /* General Purpose IO Register 1 */
#define GPIOR2  _SFR_MEM8(0x001E)  /* General Purpose IO Register 2 */
#define GPIOR3  _SFR_MEM8(0x001F)  /* General Purpose IO Register 3 */

/* Deprecated */
#define GPIO0  _SFR_MEM8(0x001C)  /* General Purpose IO Register 0 */
#define GPIO1  _SFR_MEM8(0x001D)  /* General Purpose IO Register 1 */
#define GPIO2  _SFR_MEM8(0x001E)  /* General Purpose IO Register 2 */
#define GPIO3  _SFR_MEM8(0x001F)  /* General Purpose IO Register 3 */

/* C Language Only */
#if !defined (__ASSEMBLER__)

#include <stdint.h>

typedef volatile uint8_t register8_t;
typedef volatile uint16_t register16_t;
typedef volatile uint32_t register32_t;


#ifdef _WORDREGISTER
#undef _WORDREGISTER
#endif
#define _WORDREGISTER(regname)   \
    __extension__ union \
    { \
        register16_t regname; \
        struct \
        { \
            register8_t regname ## L; \
            register8_t regname ## H; \
        }; \
    }

#ifdef _DWORDREGISTER
#undef _DWORDREGISTER
#endif
#define _DWORDREGISTER(regname)  \
    __extension__ union \
    { \
        register32_t regname; \
        struct \
        { \
            register8_t regname ## 0; \
            register8_t regname ## 1; \
            register8_t regname ## 2; \
            register8_t regname ## 3; \
        }; \
    }


/*
==========================================================================
IO Module Structures
==========================================================================
*/


/*
--------------------------------------------------------------------------
AC - Analog Comparator
--------------------------------------------------------------------------
*/

/* Analog Comparator */
typedef struct AC_struct
{
    register8_t CTRLA;  /* Control A */
    register8_t reserved_1[1];
    register8_t MUXCTRLA;  /* Mux Control A */
    register8_t reserved_2[3];
    register8_t INTCTRL;  /* Interrupt Control */
    register8_t STATUS;  /* Status */
} AC_t;

/* Hysteresis Mode select */
typedef enum AC_HYSMODE_enum
{
    AC_HYSMODE_OFF_gc = (0x00<<1),  /* No hysteresis */
    AC_HYSMODE_10mV_gc = (0x01<<1),  /* 10mV hysteresis */
    AC_HYSMODE_25mV_gc = (0x02<<1),  /* 25mV hysteresis */
    AC_HYSMODE_50mV_gc = (0x03<<1)  /* 50mV hysteresis */
} AC_HYSMODE_t;

/* Interrupt Mode select */
typedef enum AC_INTMODE_enum
{
    AC_INTMODE_BOTHEDGE_gc = (0x00<<4),  /* Any Edge */
    AC_INTMODE_NEGEDGE_gc = (0x02<<4),  /* Negative Edge */
    AC_INTMODE_POSEDGE_gc = (0x03<<4)  /* Positive Edge */
} AC_INTMODE_t;

/* Negative Input MUX Selection */
typedef enum AC_MUXNEG_enum
{
    AC_MUXNEG_PIN0_gc = (0x00<<0),  /* Negative Pin 0 */
    AC_MUXNEG_PIN1_gc = (0x01<<0),  /* Negative Pin 1 */
    AC_MUXNEG_VREF_gc = (0x02<<0)  /* Voltage Reference */
} AC_MUXNEG_t;

/* Positive Input MUX Selection */
typedef enum AC_MUXPOS_enum
{
    AC_MUXPOS_PIN0_gc = (0x00<<3),  /* Positive Pin 0 */
    AC_MUXPOS_PIN1_gc = (0x01<<3),  /* Positive Pin 1 */
    AC_MUXPOS_PIN2_gc = (0x02<<3),  /* Positive Pin 2 */
    AC_MUXPOS_PIN3_gc = (0x03<<3)  /* Positive Pin 3 */
} AC_MUXPOS_t;

/*
--------------------------------------------------------------------------
ADC - Analog to Digital Converter
--------------------------------------------------------------------------
*/

/* Analog to Digital Converter */
typedef struct ADC_struct
{
    register8_t CTRLA;  /* Control A */
    register8_t CTRLB;  /* Control B */
    register8_t CTRLC;  /* Control C */
    register8_t CTRLD;  /* Control D */
    register8_t CTRLE;  /* Control E */
    register8_t SAMPCTRL;  /* Sample Control */
    register8_t MUXPOS;  /* Positive mux input */
    register8_t reserved_1[1];
    register8_t COMMAND;  /* Command */
    register8_t EVCTRL;  /* Event Control */
    register8_t INTCTRL;  /* Interrupt Control */
    register8_t INTFLAGS;  /* Interrupt Flags */
    register8_t DBGCTRL;  /* Debug Control */
    register8_t TEMP;  /* Temporary Data */
    register8_t reserved_2[2];
    _WORDREGISTER(RES);  /* ADC Accumulator Result */
    _WORDREGISTER(WINLT);  /* Window comparator low threshold */
    _WORDREGISTER(WINHT);  /* Window comparator high threshold */
    register8_t CALIB;  /* Calibration */
    register8_t reserved_3[1];
} ADC_t;

/* Automatic Sampling Delay Variation select */
typedef enum ADC_ASDV_enum
{
    ADC_ASDV_ASVOFF_gc = (0x00<<4),  /* The Automatic Sampling Delay Variation is disabled */
    ADC_ASDV_ASVON_gc = (0x01<<4)  /* The Automatic Sampling Delay Variation is enabled */
} ADC_ASDV_t;

/* Duty Cycle select */
typedef enum ADC_DUTYCYC_enum
{
    ADC_DUTYCYC_DUTY50_gc = (0x00<<0),  /* 50% Duty cycle */
    ADC_DUTYCYC_DUTY25_gc = (0x01<<0)  /* 25% Duty cycle */
} ADC_DUTYCYC_t;

/* Initial Delay Selection */
typedef enum ADC_INITDLY_enum
{
    ADC_INITDLY_DLY0_gc = (0x00<<5),  /* Delay 0 CLK_ADC cycles */
    ADC_INITDLY_DLY16_gc = (0x01<<5),  /* Delay 16 CLK_ADC cycles */
    ADC_INITDLY_DLY32_gc = (0x02<<5),  /* Delay 32 CLK_ADC cycles */
    ADC_INITDLY_DLY64_gc = (0x03<<5),  /* Delay 64 CLK_ADC cycles */
    ADC_INITDLY_DLY128_gc = (0x04<<5),  /* Delay 128 CLK_ADC cycles */
    ADC_INITDLY_DLY256_gc = (0x05<<5)  /* Delay 256 CLK_ADC cycles */
} ADC_INITDLY_t;

/* Analog Channel Selection Bits */
typedef enum ADC_MUXPOS_enum
{
    ADC_MUXPOS_AIN0_gc = (0x00<<0),  /* ADC input pin 0 */
    ADC_MUXPOS_AIN1_gc = (0x01<<0),  /* ADC input pin 1 */
    ADC_MUXPOS_AIN2_gc = (0x02<<0),  /* ADC input pin 2 */
    ADC_MUXPOS_AIN3_gc = (0x03<<0),  /* ADC input pin 3 */
    ADC_MUXPOS_AIN4_gc = (0x04<<0),  /* ADC input pin 4 */
    ADC_MUXPOS_AIN5_gc = (0x05<<0),  /* ADC input pin 5 */
    ADC_MUXPOS_AIN6_gc = (0x06<<0),  /* ADC input pin 6 */
    ADC_MUXPOS_AIN7_gc = (0x07<<0),  /* ADC input pin 7 */
    ADC_MUXPOS_AIN8_gc = (0x08<<0),  /* ADC input pin 8 */
    ADC_MUXPOS_AIN9_gc = (0x09<<0),  /* ADC input pin 9 */
    ADC_MUXPOS_AIN10_gc = (0x0A<<0),  /* ADC input pin 10 */
    ADC_MUXPOS_AIN11_gc = (0x0B<<0),  /* ADC input pin 11 */
    ADC_MUXPOS_INTREF_gc = (0x1D<<0),  /* Internal Ref */
    ADC_MUXPOS_TEMPSENSE_gc = (0x1E<<0),  /* Temp sensor/DAC1 */
    ADC_MUXPOS_GND_gc = (0x1F<<0)  /* GND */
} ADC_MUXPOS_t;

/* Clock Pre-scaler select */
typedef enum ADC_PRESC_enum
{
    ADC_PRESC_DIV2_gc = (0x00<<0),  /* CLK_PER divided by 2 */
    ADC_PRESC_DIV4_gc = (0x01<<0),  /* CLK_PER divided by 4 */
    ADC_PRESC_DIV8_gc = (0x02<<0),  /* CLK_PER divided by 8 */
    ADC_PRESC_DIV16_gc = (0x03<<0),  /* CLK_PER divided by 16 */
    ADC_PRESC_DIV32_gc = (0x04<<0),  /* CLK_PER divided by 32 */
    ADC_PRESC_DIV64_gc = (0x05<<0),  /* CLK_PER divided by 64 */
    ADC_PRESC_DIV128_gc = (0x06<<0),  /* CLK_PER divided by 128 */
    ADC_PRESC_DIV256_gc = (0x07<<0)  /* CLK_PER divided by 256 */
} ADC_PRESC_t;

/* Reference Selection */
typedef enum ADC_REFSEL_enum
{
    ADC_REFSEL_INTREF_gc = (0x00<<4),  /* Internal reference */
    ADC_REFSEL_VDDREF_gc = (0x01<<4),  /* VDD */
    ADC_REFSEL_VREFA_gc = (0x02<<4)  /* External reference */
} ADC_REFSEL_t;

/* ADC Resolution select */
typedef enum ADC_RESSEL_enum
{
    ADC_RESSEL_10BIT_gc = (0x00<<2),  /* 10-bit mode */
    ADC_RESSEL_8BIT_gc = (0x01<<2)  /* 8-bit mode */
} ADC_RESSEL_t;

/* Accumulation Samples select */
typedef enum ADC_SAMPNUM_enum
{
    ADC_SAMPNUM_ACC1_gc = (0x00<<0),  /* 1 ADC sample */
    ADC_SAMPNUM_ACC2_gc = (0x01<<0),  /* Accumulate 2 samples */
    ADC_SAMPNUM_ACC4_gc = (0x02<<0),  /* Accumulate 4 samples */
    ADC_SAMPNUM_ACC8_gc = (0x03<<0),  /* Accumulate 8 samples */
    ADC_SAMPNUM_ACC16_gc = (0x04<<0),  /* Accumulate 16 samples */
    ADC_SAMPNUM_ACC32_gc = (0x05<<0),  /* Accumulate 32 samples */
    ADC_SAMPNUM_ACC64_gc = (0x06<<0)  /* Accumulate 64 samples */
} ADC_SAMPNUM_t;

/* Window Comparator Mode select */
typedef enum ADC_WINCM_enum
{
    ADC_WINCM_NONE_gc = (0x00<<0),  /* No Window Comparison */
    ADC_WINCM_BELOW_gc = (0x01<<0),  /* Below Window */
    ADC_WINCM_ABOVE_gc = (0x02<<0),  /* Above Window */
    ADC_WINCM_INSIDE_gc = (0x03<<0),  /* Inside Window */
    ADC_WINCM_OUTSIDE_gc = (0x04<<0)  /* Outside Window */
} ADC_WINCM_t;

/*
--------------------------------------------------------------------------
BOD - Bod interface
--------------------------------------------------------------------------
*/

/* Bod interface */
typedef struct BOD_struct
{
    register8_t CTRLA;  /* Control A */
    register8_t CTRLB;  /* Control B */
    register8_t reserved_1[6];
    register8_t VLMCTRLA;  /* Voltage level monitor Control */
    register8_t INTCTRL;  /* Voltage level monitor interrupt Control */
    register8_t INTFLAGS;  /* Voltage level monitor interrupt Flags */
    register8_t STATUS;  /* Voltage level monitor status */
    register8_t reserved_2[4];
} BOD_t;

/* Operation in active mode select */
typedef enum BOD_ACTIVE_enum
{
    BOD_ACTIVE_DIS_gc = (0x00<<2),  /* Disabled */
    BOD_ACTIVE_ENABLED_gc = (0x01<<2),  /* Enabled */
    BOD_ACTIVE_SAMPLED_gc = (0x02<<2),  /* Sampled */
    BOD_ACTIVE_ENWAKE_gc = (0x03<<2)  /* Enabled with wake-up halted until BOD is ready */
} BOD_ACTIVE_t;

/* Bod level select */
typedef enum BOD_LVL_enum
{
    BOD_LVL_BODLEVEL0_gc = (0x00<<0),  /* 1.8 V */
    BOD_LVL_BODLEVEL2_gc = (0x02<<0),  /* 2.6 V */
    BOD_LVL_BODLEVEL7_gc = (0x07<<0)  /* 4.2 V */
} BOD_LVL_t;

/* Sample frequency select */
typedef enum BOD_SAMPFREQ_enum
{
    BOD_SAMPFREQ_1KHZ_gc = (0x00<<4),  /* 1kHz sampling frequency */
    BOD_SAMPFREQ_125HZ_gc = (0x01<<4)  /* 125Hz sampling frequency */
} BOD_SAMPFREQ_t;

/* Operation in sleep mode select */
typedef enum BOD_SLEEP_enum
{
    BOD_SLEEP_DIS_gc = (0x00<<0),  /* Disabled */
    BOD_SLEEP_ENABLED_gc = (0x01<<0),  /* Enabled */
    BOD_SLEEP_SAMPLED_gc = (0x02<<0)  /* Sampled */
} BOD_SLEEP_t;

/* Configuration select */
typedef enum BOD_VLMCFG_enum
{
    BOD_VLMCFG_BELOW_gc = (0x00<<1),  /* Interrupt when supply goes below VLM level */
    BOD_VLMCFG_ABOVE_gc = (0x01<<1),  /* Interrupt when supply goes above VLM level */
    BOD_VLMCFG_CROSS_gc = (0x02<<1)  /* Interrupt when supply crosses VLM level */
} BOD_VLMCFG_t;

/* voltage level monitor level select */
typedef enum BOD_VLMLVL_enum
{
    BOD_VLMLVL_5ABOVE_gc = (0x00<<0),  /* VLM threshold 5% above BOD level */
    BOD_VLMLVL_15ABOVE_gc = (0x01<<0),  /* VLM threshold 15% above BOD level */
    BOD_VLMLVL_25ABOVE_gc = (0x02<<0)  /* VLM threshold 25% above BOD level */
} BOD_VLMLVL_t;

/*
--------------------------------------------------------------------------
CCL - Configurable Custom Logic
--------------------------------------------------------------------------
*/

/* Configurable Custom Logic */
typedef struct CCL_struct
{
    register8_t CTRLA;  /* Control Register A */
    register8_t SEQCTRL0;  /* Sequential Control 0 */
    register8_t reserved_1[3];
    register8_t LUT0CTRLA;  /* LUT Control 0 A */
    register8_t LUT0CTRLB;  /* LUT Control 0 B */
    register8_t LUT0CTRLC;  /* LUT Control 0 C */
    register8_t TRUTH0;  /* Truth 0 */
    register8_t LUT1CTRLA;  /* LUT Control 1 A */
    register8_t LUT1CTRLB;  /* LUT Control 1 B */
    register8_t LUT1CTRLC;  /* LUT Control 1 C */
    register8_t TRUTH1;  /* Truth 1 */
    register8_t reserved_2[51];
} CCL_t;

/* Clock Source Selection */
typedef enum CCL_CLKSRC_enum
{
    CCL_CLKSRC_CLKPER_gc = (0x00<<6),  /* CLK_PER is clocking the LUT */
    CCL_CLKSRC_IN2_gc = (0x01<<6)  /* IN[2] is clocking the LUT */
} CCL_CLKSRC_t;

/* Edge Detection Enable select */
typedef enum CCL_EDGEDET_enum
{
    CCL_EDGEDET_DIS_gc = (0x00<<7),  /* Edge detector is disabled */
    CCL_EDGEDET_EN_gc = (0x01<<7)  /* Edge detector is enabled */
} CCL_EDGEDET_t;

/* Filter Selection */
typedef enum CCL_FILTSEL_enum
{
    CCL_FILTSEL_DISABLE_gc = (0x00<<4),  /* Filter disabled */
    CCL_FILTSEL_SYNCH_gc = (0x01<<4),  /* Synchronizer enabled */
    CCL_FILTSEL_FILTER_gc = (0x02<<4)  /* Filter enabled */
} CCL_FILTSEL_t;

/* LUT Input 0 Source Selection */
typedef enum CCL_INSEL0_enum
{
    CCL_INSEL0_MASK_gc = (0x00<<0),  /* Masked input */
    CCL_INSEL0_FEEDBACK_gc = (0x01<<0),  /* Feedback input source */
    CCL_INSEL0_LINK_gc = (0x02<<0),  /* Linked LUT input source */
    CCL_INSEL0_EVENT0_gc = (0x03<<0),  /* Event input source 0 */
    CCL_INSEL0_EVENT1_gc = (0x04<<0),  /* Event input source 1 */
    CCL_INSEL0_IO_gc = (0x05<<0),  /* IO pin LUTn-IN0 input source */
    CCL_INSEL0_AC0_gc = (0x06<<0),  /* AC0 OUT input source */
    CCL_INSEL0_TCB0_gc = (0x07<<0),  /* TCB0 WO input source */
    CCL_INSEL0_TCA0_gc = (0x08<<0),  /* TCA0 WO0 input source */
    CCL_INSEL0_TCD0_gc = (0x09<<0),  /* TCD0 WOA input source */
    CCL_INSEL0_USART0_gc = (0x0A<<0),  /* USART0 XCK input source */
    CCL_INSEL0_SPI0_gc = (0x0B<<0)  /* SPI0 SCK source */
} CCL_INSEL0_t;

/* LUT Input 1 Source Selection */
typedef enum CCL_INSEL1_enum
{
    CCL_INSEL1_MASK_gc = (0x00<<4),  /* Masked input */
    CCL_INSEL1_FEEDBACK_gc = (0x01<<4),  /* Feedback input source */
    CCL_INSEL1_LINK_gc = (0x02<<4),  /* Linked LUT input source */
    CCL_INSEL1_EVENT0_gc = (0x03<<4),  /* Event input source 0 */
    CCL_INSEL1_EVENT1_gc = (0x04<<4),  /* Event input source 1 */
    CCL_INSEL1_IO_gc = (0x05<<4),  /* IO pin LUTn-N1 input source */
    CCL_INSEL1_AC0_gc = (0x06<<4),  /* AC0 OUT input source */
    CCL_INSEL1_TCB0_gc = (0x07<<4),  /* TCB0 WO input source */
    CCL_INSEL1_TCA0_gc = (0x08<<4),  /* TCA0 WO1 input source */
    CCL_INSEL1_TCD0_gc = (0x09<<4),  /* TCD0 WOB input source */
    CCL_INSEL1_USART0_gc = (0x0A<<4),  /* USART0 TXD input source */
    CCL_INSEL1_SPI0_gc = (0x0B<<4)  /* SPI0 MOSI input source */
} CCL_INSEL1_t;

/* LUT Input 2 Source Selection */
typedef enum CCL_INSEL2_enum
{
    CCL_INSEL2_MASK_gc = (0x00<<0),  /* Masked input */
    CCL_INSEL2_FEEDBACK_gc = (0x01<<0),  /* Feedback input source */
    CCL_INSEL2_LINK_gc = (0x02<<0),  /* Linked LUT input source */
    CCL_INSEL2_EVENT0_gc = (0x03<<0),  /* Event input source 0 */
    CCL_INSEL2_EVENT1_gc = (0x04<<0),  /* Event input source 1 */
    CCL_INSEL2_IO_gc = (0x05<<0),  /* IO pin LUTn-IN2 input source */
    CCL_INSEL2_AC0_gc = (0x06<<0),  /* AC0 OUT input source */
    CCL_INSEL2_TCB0_gc = (0x07<<0),  /* TCB0 WO input source */
    CCL_INSEL2_TCA0_gc = (0x08<<0),  /* TCA0 WO2 input source */
    CCL_INSEL2_TCD0_gc = (0x09<<0),  /* TCD0 WOA input source */
    CCL_INSEL2_SPI0_gc = (0x0B<<0)  /* SPI0 MISO source */
} CCL_INSEL2_t;

/* Sequential Selection */
typedef enum CCL_SEQSEL_enum
{
    CCL_SEQSEL_DISABLE_gc = (0x00<<0),  /* Sequential logic disabled */
    CCL_SEQSEL_DFF_gc = (0x01<<0),  /* D FlipFlop */
    CCL_SEQSEL_JK_gc = (0x02<<0),  /* JK FlipFlop */
    CCL_SEQSEL_LATCH_gc = (0x03<<0),  /* D Latch */
    CCL_SEQSEL_RS_gc = (0x04<<0)  /* RS Latch */
} CCL_SEQSEL_t;

/*
--------------------------------------------------------------------------
CLKCTRL - Clock controller
--------------------------------------------------------------------------
*/

/* Clock controller */
typedef struct CLKCTRL_struct
{
    register8_t MCLKCTRLA;  /* MCLK Control A */
    register8_t MCLKCTRLB;  /* MCLK Control B */
    register8_t MCLKLOCK;  /* MCLK Lock */
    register8_t MCLKSTATUS;  /* MCLK Status */
    register8_t reserved_1[12];
    register8_t OSC20MCTRLA;  /* OSC20M Control A */
    register8_t OSC20MCALIBA;  /* OSC20M Calibration A */
    register8_t OSC20MCALIBB;  /* OSC20M Calibration B */
    register8_t reserved_2[5];
    register8_t OSC32KCTRLA;  /* OSC32K Control A */
    register8_t reserved_3[7];
} CLKCTRL_t;

/* Clock select */
typedef enum CLKCTRL_CLKSEL_enum
{
    CLKCTRL_CLKSEL_OSC20M_gc = (0x00<<0),  /* 16/20MHz internal oscillator */
    CLKCTRL_CLKSEL_OSCULP32K_gc = (0x01<<0),  /* 32.768kHz internal ultra low-power oscillator */
    CLKCTRL_CLKSEL_EXTCLK_gc = (0x03<<0)  /* External clock */
} CLKCTRL_CLKSEL_t;

/* Prescaler division select */
typedef enum CLKCTRL_PDIV_enum
{
    CLKCTRL_PDIV_2X_gc = (0x00<<1),  /* 2X */
    CLKCTRL_PDIV_4X_gc = (0x01<<1),  /* 4X */
    CLKCTRL_PDIV_8X_gc = (0x02<<1),  /* 8X */
    CLKCTRL_PDIV_16X_gc = (0x03<<1),  /* 16X */
    CLKCTRL_PDIV_32X_gc = (0x04<<1),  /* 32X */
    CLKCTRL_PDIV_64X_gc = (0x05<<1),  /* 64X */
    CLKCTRL_PDIV_6X_gc = (0x08<<1),  /* 6X */
    CLKCTRL_PDIV_10X_gc = (0x09<<1),  /* 10X */
    CLKCTRL_PDIV_12X_gc = (0x0A<<1),  /* 12X */
    CLKCTRL_PDIV_24X_gc = (0x0B<<1),  /* 24X */
    CLKCTRL_PDIV_48X_gc = (0x0C<<1)  /* 48X */
} CLKCTRL_PDIV_t;

/*
--------------------------------------------------------------------------
CPU - CPU
--------------------------------------------------------------------------
*/

#define CORE_VERSION  V4

/* CCP signature select */
typedef enum CCP_enum
{
    CCP_SPM_gc = (0x9D<<0),  /* SPM Instruction Protection */
    CCP_IOREG_gc = (0xD8<<0)  /* IO Register Protection */
} CCP_t;

/*
--------------------------------------------------------------------------
CPUINT - Interrupt Controller
--------------------------------------------------------------------------
*/

/* Interrupt Controller */
typedef struct CPUINT_struct
{
    register8_t CTRLA;  /* Control A */
    register8_t STATUS;  /* Status */
    register8_t LVL0PRI;  /* Interrupt Level 0 Priority */
    register8_t LVL1VEC;  /* Interrupt Level 1 Priority Vector */
} CPUINT_t;


/*
--------------------------------------------------------------------------
CRCSCAN - CRCSCAN
--------------------------------------------------------------------------
*/

/* CRCSCAN */
typedef struct CRCSCAN_struct
{
    register8_t CTRLA;  /* Control A */
    register8_t CTRLB;  /* Control B */
    register8_t STATUS;  /* Status */
    register8_t reserved_1[1];
} CRCSCAN_t;

/* CRC Flash Access Mode select */
typedef enum CRCSCAN_MODE_enum
{
    CRCSCAN_MODE_PRIORITY_gc = (0x00<<4)  /* Priority to flash */
} CRCSCAN_MODE_t;

/* CRC Source select */
typedef enum CRCSCAN_SRC_enum
{
    CRCSCAN_SRC_FLASH_gc = (0x00<<0),  /* CRC on entire flash */
    CRCSCAN_SRC_APPLICATION_gc = (0x01<<0),  /* CRC on boot and appl section of flash */
    CRCSCAN_SRC_BOOT_gc = (0x02<<0)  /* CRC on boot section of flash */
} CRCSCAN_SRC_t;

/*
--------------------------------------------------------------------------
EVSYS - Event System
--------------------------------------------------------------------------
*/

/* Event System */
typedef struct EVSYS_struct
{
    register8_t ASYNCSTROBE;  /* Asynchronous Channel Strobe */
    register8_t SYNCSTROBE;  /* Synchronous Channel Strobe */
    register8_t ASYNCCH0;  /* Asynchronous Channel 0 Generator Selection */
    register8_t ASYNCCH1;  /* Asynchronous Channel 1 Generator Selection */
    register8_t reserved_1[6];
    register8_t SYNCCH0;  /* Synchronous Channel 0 Generator Selection */
    register8_t reserved_2[7];
    register8_t ASYNCUSER0;  /* Asynchronous User Ch 0 Input Selection - TCB0 */
    register8_t ASYNCUSER1;  /* Asynchronous User Ch 1 Input Selection - ADC0 */
    register8_t ASYNCUSER2;  /* Asynchronous User Ch 2 Input Selection - CCL LUT0 Event 0 */
    register8_t ASYNCUSER3;  /* Asynchronous User Ch 3 Input Selection - CCL LUT1 Event 0 */
    register8_t ASYNCUSER4;  /* Asynchronous User Ch 4 Input Selection - CCL LUT0 Event 1 */
    register8_t ASYNCUSER5;  /* Asynchronous User Ch 5 Input Selection - CCL LUT1 Event 1 */
    register8_t ASYNCUSER6;  /* Asynchronous User Ch 6 Input Selection - TCD0 Event 0 */
    register8_t ASYNCUSER7;  /* Asynchronous User Ch 7 Input Selection - TCD0 Event 1 */
    register8_t ASYNCUSER8;  /* Asynchronous User Ch 8 Input Selection - Event Out 0 */
    register8_t ASYNCUSER9;  /* Asynchronous User Ch 9 Input Selection - Event Out 1 */
    register8_t ASYNCUSER10;  /* Asynchronous User Ch 10 Input Selection - Event Out 2 */
    register8_t ASYNCUSER11;  /* Asynchronous User Ch 11 Input Selection - TCB1 */
    register8_t ASYNCUSER12;  /* Asynchronous User Ch 12 Input Selection - ADC1 */
    register8_t reserved_3[3];
    register8_t SYNCUSER0;  /* Synchronous User Ch 0 - TCA0 */
    register8_t SYNCUSER1;  /* Synchronous User Ch 1 - USART0 */
    register8_t reserved_4[28];
} EVSYS_t;

/* Asynchronous Channel 0 Generator Selection */
typedef enum EVSYS_ASYNCCH0_enum
{
    EVSYS_ASYNCCH0_OFF_gc = (0x00<<0),  /* Off */
    EVSYS_ASYNCCH0_CCL_LUT0_gc = (0x01<<0),  /* Configurable Custom Logic LUT0 */
    EVSYS_ASYNCCH0_CCL_LUT1_gc = (0x02<<0),  /* Configurable Custom Logic LUT1 */
    EVSYS_ASYNCCH0_AC0_OUT_gc = (0x03<<0),  /* Analog Comparator 0 out */
    EVSYS_ASYNCCH0_TCD0_CMPBCLR_gc = (0x04<<0),  /* Timer/Counter D0 compare B clear */
    EVSYS_ASYNCCH0_TCD0_CMPASET_gc = (0x05<<0),  /* Timer/Counter D0 compare A set */
    EVSYS_ASYNCCH0_TCD0_CMPBSET_gc = (0x06<<0),  /* Timer/Counter D0 compare B set */
    EVSYS_ASYNCCH0_TCD0_PROGEV_gc = (0x07<<0),  /* Timer/Counter D0 program event */
    EVSYS_ASYNCCH0_RTC_OVF_gc = (0x08<<0),  /* Real Time Counter overflow */
    EVSYS_ASYNCCH0_RTC_CMP_gc = (0x09<<0),  /* Real Time Counter compare */
    EVSYS_ASYNCCH0_PORTA_PIN0_gc = (0x0A<<0),  /* Asynchronous Event from Pin PA0 */
    EVSYS_ASYNCCH0_PORTA_PIN1_gc = (0x0B<<0),  /* Asynchronous Event from Pin PA1 */
    EVSYS_ASYNCCH0_PORTA_PIN2_gc = (0x0C<<0),  /* Asynchronous Event from Pin PA2 */
    EVSYS_ASYNCCH0_PORTA_PIN3_gc = (0x0D<<0),  /* Asynchronous Event from Pin PA3 */
    EVSYS_ASYNCCH0_PORTA_PIN4_gc = (0x0E<<0),  /* Asynchronous Event from Pin PA4 */
    EVSYS_ASYNCCH0_PORTA_PIN5_gc = (0x0F<<0),  /* Asynchronous Event from Pin PA5 */
    EVSYS_ASYNCCH0_PORTA_PIN6_gc = (0x10<<0),  /* Asynchronous Event from Pin PA6 */
    EVSYS_ASYNCCH0_PORTA_PIN7_gc = (0x11<<0),  /* Asynchronous Event from Pin PA7 */
    EVSYS_ASYNCCH0_UPDI_gc = (0x12<<0),  /* Unified Program and debug interface */
    EVSYS_ASYNCCH0_AC1_OUT_gc = (0x13<<0),  /* Analog Comparator 1 out */
    EVSYS_ASYNCCH0_AC2_OUT_gc = (0x14<<0)  /* Analog Comparator 2 out */
} EVSYS_ASYNCCH0_t;

/* Asynchronous Channel 1 Generator Selection */
typedef enum EVSYS_ASYNCCH1_enum
{
    EVSYS_ASYNCCH1_OFF_gc = (0x00<<0),  /* Off */
    EVSYS_ASYNCCH1_CCL_LUT0_gc = (0x01<<0),  /* Configurable custom logic LUT0 */
    EVSYS_ASYNCCH1_CCL_LUT1_gc = (0x02<<0),  /* Configurable custom logic LUT1 */
    EVSYS_ASYNCCH1_AC0_OUT_gc = (0x03<<0),  /* Analog Comparator 0 out */
    EVSYS_ASYNCCH1_TCD0_CMPBCLR_gc = (0x04<<0),  /* Timer/Counter D0 compare B clear */
    EVSYS_ASYNCCH1_TCD0_CMPASET_gc = (0x05<<0),  /* Timer/Counter D0 compare A set */
    EVSYS_ASYNCCH1_TCD0_CMPBSET_gc = (0x06<<0),  /* Timer/Counter D0 compare B set */
    EVSYS_ASYNCCH1_TCD0_PROGEV_gc = (0x07<<0),  /* Timer/Counter D0 program event */
    EVSYS_ASYNCCH1_RTC_OVF_gc = (0x08<<0),  /* Real Time Counter overflow */
    EVSYS_ASYNCCH1_RTC_CMP_gc = (0x09<<0),  /* Real Time Counter compare */
    EVSYS_ASYNCCH1_PORTB_PIN0_gc = (0x0A<<0),  /* Asynchronous Event from Pin PB0 */
    EVSYS_ASYNCCH1_PORTB_PIN1_gc = (0x0B<<0),  /* Asynchronous Event from Pin PB1 */
    EVSYS_ASYNCCH1_PORTB_PIN2_gc = (0x0C<<0),  /* Asynchronous Event from Pin PB2 */
    EVSYS_ASYNCCH1_PORTB_PIN3_gc = (0x0D<<0),  /* Asynchronous Event from Pin PB3 */
    EVSYS_ASYNCCH1_PORTB_PIN4_gc = (0x0E<<0),  /* Asynchronous Event from Pin PB4 */
    EVSYS_ASYNCCH1_PORTB_PIN5_gc = (0x0F<<0),  /* Asynchronous Event from Pin PB5 */
    EVSYS_ASYNCCH1_PORTB_PIN6_gc = (0x10<<0),  /* Asynchronous Event from Pin PB6 */
    EVSYS_ASYNCCH1_PORTB_PIN7_gc = (0x11<<0),  /* Asynchronous Event from Pin PB7 */
    EVSYS_ASYNCCH1_AC1_OUT_gc = (0x12<<0),  /* Analog Comparator 1 out */
    EVSYS_ASYNCCH1_AC2_OUT_gc = (0x13<<0)  /* Analog Comparator 2 out */
} EVSYS_ASYNCCH1_t;

/* Asynchronous User Ch 0 Input Selection - TCB0 */
typedef enum EVSYS_ASYNCUSER0_enum
{
    EVSYS_ASYNCUSER0_OFF_gc = (0x00<<0),  /* Off */
    EVSYS_ASYNCUSER0_SYNCCH0_gc = (0x01<<0),  /* Synchronous Event Channel 0 */
    EVSYS_ASYNCUSER0_ASYNCCH0_gc = (0x03<<0),  /* Asynchronous Event Channel 0 */
    EVSYS_ASYNCUSER0_ASYNCCH1_gc = (0x04<<0)  /* Asynchronous Event Channel 1 */
} EVSYS_ASYNCUSER0_t;

/* Asynchronous User Ch 1 Input Selection - ADC0 */
typedef enum EVSYS_ASYNCUSER1_enum
{
    EVSYS_ASYNCUSER1_OFF_gc = (0x00<<0),  /* Off */
    EVSYS_ASYNCUSER1_SYNCCH0_gc = (0x01<<0),  /* Synchronous Event Channel 0 */
    EVSYS_ASYNCUSER1_ASYNCCH0_gc = (0x03<<0),  /* Asynchronous Event Channel 0 */
    EVSYS_ASYNCUSER1_ASYNCCH1_gc = (0x04<<0)  /* Asynchronous Event Channel 1 */
} EVSYS_ASYNCUSER1_t;

/* Asynchronous User Ch 10 Input Selection - Event Out 2 */
typedef enum EVSYS_ASYNCUSER10_enum
{
    EVSYS_ASYNCUSER10_OFF_gc = (0x00<<0),  /* Off */
    EVSYS_ASYNCUSER10_SYNCCH0_gc = (0x01<<0),  /* Synchronous Event Channel 0 */
    EVSYS_ASYNCUSER10_ASYNCCH0_gc = (0x03<<0),  /* Asynchronous Event Channel 0 */
    EVSYS_ASYNCUSER10_ASYNCCH1_gc = (0x04<<0)  /* Asynchronous Event Channel 1 */
} EVSYS_ASYNCUSER10_t;

/* Asynchronous User Ch 11 Input Selection - TCB1 */
typedef enum EVSYS_ASYNCUSER11_enum
{
    EVSYS_ASYNCUSER11_OFF_gc = (0x00<<0),  /* Off */
    EVSYS_ASYNCUSER11_SYNCCH0_gc = (0x01<<0),  /* Synchronous Event Channel 0 */
    EVSYS_ASYNCUSER11_ASYNCCH0_gc = (0x03<<0),  /* Asynchronous Event Channel 0 */
    EVSYS_ASYNCUSER11_ASYNCCH1_gc = (0x04<<0)  /* Asynchronous Event Channel 1 */
} EVSYS_ASYNCUSER11_t;

/* Asynchronous User Ch 12 Input Selection - ADC0 */
typedef enum EVSYS_ASYNCUSER12_enum
{
    EVSYS_ASYNCUSER12_OFF_gc = (0x00<<0),  /* Off */
    EVSYS_ASYNCUSER12_SYNCCH0_gc = (0x01<<0),  /* Synchronous Event Channel 0 */
    EVSYS_ASYNCUSER12_ASYNCCH0_gc = (0x03<<0),  /* Asynchronous Event Channel 0 */
    EVSYS_ASYNCUSER12_ASYNCCH1_gc = (0x04<<0)  /* Asynchronous Event Channel 1 */
} EVSYS_ASYNCUSER12_t;

/* Asynchronous User Ch 2 Input Selection - CCL LUT0 Event 0 */
typedef enum EVSYS_ASYNCUSER2_enum
{
    EVSYS_ASYNCUSER2_OFF_gc = (0x00<<0),  /* Off */
    EVSYS_ASYNCUSER2_SYNCCH0_gc = (0x01<<0),  /* Synchronous Event Channel 0 */
    EVSYS_ASYNCUSER2_ASYNCCH0_gc = (0x03<<0),  /* Asynchronous Event Channel 0 */
    EVSYS_ASYNCUSER2_ASYNCCH1_gc = (0x04<<0)  /* Asynchronous Event Channel 1 */
} EVSYS_ASYNCUSER2_t;

/* Asynchronous User Ch 3 Input Selection - CCL LUT1 Event 0 */
typedef enum EVSYS_ASYNCUSER3_enum
{
    EVSYS_ASYNCUSER3_OFF_gc = (0x00<<0),  /* Off */
    EVSYS_ASYNCUSER3_SYNCCH0_gc = (0x01<<0),  /* Synchronous Event Channel 0 */
    EVSYS_ASYNCUSER3_ASYNCCH0_gc = (0x03<<0),  /* Asynchronous Event Channel 0 */
    EVSYS_ASYNCUSER3_ASYNCCH1_gc = (0x04<<0)  /* Asynchronous Event Channel 1 */
} EVSYS_ASYNCUSER3_t;

/* Asynchronous User Ch 4 Input Selection - CCL LUT0 Event 1 */
typedef enum EVSYS_ASYNCUSER4_enum
{
    EVSYS_ASYNCUSER4_OFF_gc = (0x00<<0),  /* Off */
    EVSYS_ASYNCUSER4_SYNCCH0_gc = (0x01<<0),  /* Synchronous Event Channel 0 */
    EVSYS_ASYNCUSER4_ASYNCCH0_gc = (0x03<<0),  /* Asynchronous Event Channel 0 */
    EVSYS_ASYNCUSER4_ASYNCCH1_gc = (0x04<<0)  /* Asynchronous Event Channel 1 */
} EVSYS_ASYNCUSER4_t;

/* Asynchronous User Ch 5 Input Selection - CCL LUT1 Event 1 */
typedef enum EVSYS_ASYNCUSER5_enum
{
    EVSYS_ASYNCUSER5_OFF_gc = (0x00<<0),  /* Off */
    EVSYS_ASYNCUSER5_SYNCCH0_gc = (0x01<<0),  /* Synchronous Event Channel 0 */
    EVSYS_ASYNCUSER5_ASYNCCH0_gc = (0x03<<0),  /* Asynchronous Event Channel 0 */
    EVSYS_ASYNCUSER5_ASYNCCH1_gc = (0x04<<0)  /* Asynchronous Event Channel 1 */
} EVSYS_ASYNCUSER5_t;

/* Asynchronous User Ch 6 Input Selection - TCD0 Event 0 */
typedef enum EVSYS_ASYNCUSER6_enum
{
    EVSYS_ASYNCUSER6_OFF_gc = (0x00<<0),  /* Off */
    EVSYS_ASYNCUSER6_SYNCCH0_gc = (0x01<<0),  /* Synchronous Event Channel 0 */
    EVSYS_ASYNCUSER6_ASYNCCH0_gc = (0x03<<0),  /* Asynchronous Event Channel 0 */
    EVSYS_ASYNCUSER6_ASYNCCH1_gc = (0x04<<0)  /* Asynchronous Event Channel 1 */
} EVSYS_ASYNCUSER6_t;

/* Asynchronous User Ch 7 Input Selection - TCD0 Event 1 */
typedef enum EVSYS_ASYNCUSER7_enum
{
    EVSYS_ASYNCUSER7_OFF_gc = (0x00<<0),  /* Off */
    EVSYS_ASYNCUSER7_SYNCCH0_gc = (0x01<<0),  /* Synchronous Event Channel 0 */
    EVSYS_ASYNCUSER7_ASYNCCH0_gc = (0x03<<0),  /* Asynchronous Event Channel 0 */
    EVSYS_ASYNCUSER7_ASYNCCH1_gc = (0x04<<0)  /* Asynchronous Event Channel 1 */
} EVSYS_ASYNCUSER7_t;

/* Asynchronous User Ch 8 Input Selection - Event Out 0 */
typedef enum EVSYS_ASYNCUSER8_enum
{
    EVSYS_ASYNCUSER8_OFF_gc = (0x00<<0),  /* Off */
    EVSYS_ASYNCUSER8_SYNCCH0_gc = (0x01<<0),  /* Synchronous Event Channel 0 */
    EVSYS_ASYNCUSER8_ASYNCCH0_gc = (0x03<<0),  /* Asynchronous Event Channel 0 */
    EVSYS_ASYNCUSER8_ASYNCCH1_gc = (0x04<<0)  /* Asynchronous Event Channel 1 */
} EVSYS_ASYNCUSER8_t;

/* Asynchronous User Ch 9 Input Selection - Event Out 1 */
typedef enum EVSYS_ASYNCUSER9_enum
{
    EVSYS_ASYNCUSER9_OFF_gc = (0x00<<0),  /* Off */
    EVSYS_ASYNCUSER9_SYNCCH0_gc = (0x01<<0),  /* Synchronous Event Channel 0 */
    EVSYS_ASYNCUSER9_ASYNCCH0_gc = (0x03<<0),  /* Asynchronous Event Channel 0 */
    EVSYS_ASYNCUSER9_ASYNCCH1_gc = (0x04<<0)  /* Asynchronous Event Channel 1 */
} EVSYS_ASYNCUSER9_t;

/* Synchronous Channel 0 Generator Selection */
typedef enum EVSYS_SYNCCH0_enum
{
    EVSYS_SYNCCH0_OFF_gc = (0x00<<0),  /* Off */
    EVSYS_SYNCCH0_TCB0_gc = (0x01<<0),  /* Timer/Counter B0 */
    EVSYS_SYNCCH0_TCA0_OVF_LUNF_gc = (0x02<<0),  /* Timer/Counter A0 overflow */
    EVSYS_SYNCCH0_TCA0_HUNF_gc = (0x03<<0),  /* Timer/Counter A0 underflow high byte (split mode) */
    EVSYS_SYNCCH0_TCA0_CMP0_gc = (0x04<<0),  /* Timer/Counter A0 compare 0 */
    EVSYS_SYNCCH0_TCA0_CMP1_gc = (0x05<<0),  /* Timer/Counter A0 compare 1 */
    EVSYS_SYNCCH0_TCA0_CMP2_gc = (0x06<<0),  /* Timer/Counter A0 compare 2 */
    EVSYS_SYNCCH0_PORTC_PIN0_gc = (0x07<<0),  /* Synchronous Event from Pin PC0 */
    EVSYS_SYNCCH0_PORTC_PIN1_gc = (0x08<<0),  /* Synchronous Event from Pin PC1 */
    EVSYS_SYNCCH0_PORTC_PIN2_gc = (0x09<<0),  /* Synchronous Event from Pin PC2 */
    EVSYS_SYNCCH0_PORTC_PIN3_gc = (0x0A<<0),  /* Synchronous Event from Pin PC3 */
    EVSYS_SYNCCH0_PORTC_PIN4_gc = (0x0B<<0),  /* Synchronous Event from Pin PC4 */
    EVSYS_SYNCCH0_PORTC_PIN5_gc = (0x0C<<0),  /* Synchronous Event from Pin PC5 */
    EVSYS_SYNCCH0_PORTA_PIN0_gc = (0x0D<<0),  /* Synchronous Event from Pin PA0 */
    EVSYS_SYNCCH0_PORTA_PIN1_gc = (0x0E<<0),  /* Synchronous Event from Pin PA1 */
    EVSYS_SYNCCH0_PORTA_PIN2_gc = (0x0F<<0),  /* Synchronous Event from Pin PA2 */
    EVSYS_SYNCCH0_PORTA_PIN3_gc = (0x10<<0),  /* Synchronous Event from Pin PA3 */
    EVSYS_SYNCCH0_PORTA_PIN4_gc = (0x11<<0),  /* Synchronous Event from Pin PA4 */
    EVSYS_SYNCCH0_PORTA_PIN5_gc = (0x12<<0),  /* Synchronous Event from Pin PA5 */
    EVSYS_SYNCCH0_PORTA_PIN6_gc = (0x13<<0),  /* Synchronous Event from Pin PA6 */
    EVSYS_SYNCCH0_PORTA_PIN7_gc = (0x14<<0),  /* Synchronous Event from Pin PA7 */
    EVSYS_SYNCCH0_TCB1_gc = (0x15<<0)  /* Timer/Counter B1 */
} EVSYS_SYNCCH0_t;

/* Synchronous User Ch 0 - TCA0 select */
typedef enum EVSYS_SYNCUSER0_enum
{
    EVSYS_SYNCUSER0_OFF_gc = (0x00<<0),  /* Off */
    EVSYS_SYNCUSER0_SYNCCH0_gc = (0x01<<0)  /* Synchronous Event Channel 0 */
} EVSYS_SYNCUSER0_t;

/* Synchronous User Ch 1 - USART0 select */
typedef enum EVSYS_SYNCUSER1_enum
{
    EVSYS_SYNCUSER1_OFF_gc = (0x00<<0),  /* Off */
    EVSYS_SYNCUSER1_SYNCCH0_gc = (0x01<<0)  /* Synchronous Event Channel 0 */
} EVSYS_SYNCUSER1_t;

/*
--------------------------------------------------------------------------
FUSE - Fuses
--------------------------------------------------------------------------
*/

/* Fuses */
typedef struct FUSE_struct
{
    register8_t WDTCFG;  /* Watchdog Configuration */
    register8_t BODCFG;  /* BOD Configuration */
    register8_t OSCCFG;  /* Oscillator Configuration */
    register8_t reserved_1[2];
    register8_t SYSCFG0;  /* System Configuration 0 */
    register8_t SYSCFG1;  /* System Configuration 1 */
    register8_t APPEND;  /* Application Code Section End */
    register8_t BOOTEND;  /* Boot Section End */
} FUSE_t;

/* avr-libc typedef for avr/fuse.h */
typedef FUSE_t NVM_FUSES_t;

/* BOD Operation in Active Mode select */
typedef enum ACTIVE_enum
{
    ACTIVE_DIS_gc = (0x00<<2),  /* Disabled */
    ACTIVE_ENABLED_gc = (0x01<<2),  /* Enabled */
    ACTIVE_SAMPLED_gc = (0x02<<2),  /* Sampled */
    ACTIVE_ENWAKE_gc = (0x03<<2)  /* Enabled with wake-up halted until BOD is ready */
} ACTIVE_t;

/* CRC Source select */
typedef enum CRCSRC_enum
{
    CRCSRC_FLASH_gc = (0x00<<6),  /* The CRC is performed on the entire Flash (boot, application code and application data section). */
    CRCSRC_BOOT_gc = (0x01<<6),  /* The CRC is performed on the boot section of Flash */
    CRCSRC_BOOTAPP_gc = (0x02<<6),  /* The CRC is performed on the boot and application code section of Flash */
    CRCSRC_NOCRC_gc = (0x03<<6)  /* Disable CRC. */
} CRCSRC_t;

/* Frequency Select */
typedef enum FREQSEL_enum
{
    FREQSEL_16MHZ_gc = (0x01<<0),  /* 16 MHz */
    FREQSEL_20MHZ_gc = (0x02<<0)  /* 20 MHz */
} FREQSEL_t;

/* BOD Level select */
typedef enum LVL_enum
{
    LVL_BODLEVEL0_gc = (0x00<<5),  /* 1.8 V */
    LVL_BODLEVEL2_gc = (0x02<<5),  /* 2.6 V */
    LVL_BODLEVEL7_gc = (0x07<<5)  /* 4.2 V */
} LVL_t;

/* Watchdog Timeout Period select */
typedef enum PERIOD_enum
{
    PERIOD_OFF_gc = (0x00<<0),  /* Off */
    PERIOD_8CLK_gc = (0x01<<0),  /* 8 cycles (8ms) */
    PERIOD_16CLK_gc = (0x02<<0),  /* 16 cycles (16ms) */
    PERIOD_32CLK_gc = (0x03<<0),  /* 32 cycles (32ms) */
    PERIOD_64CLK_gc = (0x04<<0),  /* 64 cycles (64ms) */
    PERIOD_128CLK_gc = (0x05<<0),  /* 128 cycles (0.128s) */
    PERIOD_256CLK_gc = (0x06<<0),  /* 256 cycles (0.256s) */
    PERIOD_512CLK_gc = (0x07<<0),  /* 512 cycles (0.512s) */
    PERIOD_1KCLK_gc = (0x08<<0),  /* 1K cycles (1.0s) */
    PERIOD_2KCLK_gc = (0x09<<0),  /* 2K cycles (2.0s) */
    PERIOD_4KCLK_gc = (0x0A<<0),  /* 4K cycles (4.1s) */
    PERIOD_8KCLK_gc = (0x0B<<0)  /* 8K cycles (8.2s) */
} PERIOD_t;

/* Reset Pin Configuration select */
typedef enum RSTPINCFG_enum
{
    RSTPINCFG_GPIO_gc = (0x00<<2),  /* GPIO mode */
    RSTPINCFG_UPDI_gc = (0x01<<2),  /* UPDI mode */
    RSTPINCFG_RST_gc = (0x02<<2)  /* Reset mode */
} RSTPINCFG_t;

/* BOD Sample Frequency select */
typedef enum SAMPFREQ_enum
{
    SAMPFREQ_1KHZ_gc = (0x00<<4),  /* 1kHz sampling frequency */
    SAMPFREQ_125HZ_gc = (0x01<<4)  /* 125Hz sampling frequency */
} SAMPFREQ_t;

/* BOD Operation in Sleep Mode select */
typedef enum SLEEP_enum
{
    SLEEP_DIS_gc = (0x00<<0),  /* Disabled */
    SLEEP_ENABLED_gc = (0x01<<0),  /* Enabled */
    SLEEP_SAMPLED_gc = (0x02<<0)  /* Sampled */
} SLEEP_t;

/* Startup Time select */
typedef enum SUT_enum
{
    SUT_0MS_gc = (0x00<<0),  /* 0 ms */
    SUT_1MS_gc = (0x01<<0),  /* 1 ms */
    SUT_2MS_gc = (0x02<<0),  /* 2 ms */
    SUT_4MS_gc = (0x03<<0),  /* 4 ms */
    SUT_8MS_gc = (0x04<<0),  /* 8 ms */
    SUT_16MS_gc = (0x05<<0),  /* 16 ms */
    SUT_32MS_gc = (0x06<<0),  /* 32 ms */
    SUT_64MS_gc = (0x07<<0)  /* 64 ms */
} SUT_t;

/* Watchdog Window Timeout Period select */
typedef enum WINDOW_enum
{
    WINDOW_OFF_gc = (0x00<<4),  /* Off */
    WINDOW_8CLK_gc = (0x01<<4),  /* 8 cycles (8ms) */
    WINDOW_16CLK_gc = (0x02<<4),  /* 16 cycles (16ms) */
    WINDOW_32CLK_gc = (0x03<<4),  /* 32 cycles (32ms) */
    WINDOW_64CLK_gc = (0x04<<4),  /* 64 cycles (64ms) */
    WINDOW_128CLK_gc = (0x05<<4),  /* 128 cycles (0.128s) */
    WINDOW_256CLK_gc = (0x06<<4),  /* 256 cycles (0.256s) */
    WINDOW_512CLK_gc = (0x07<<4),  /* 512 cycles (0.512s) */
    WINDOW_1KCLK_gc = (0x08<<4),  /* 1K cycles (1.0s) */
    WINDOW_2KCLK_gc = (0x09<<4),  /* 2K cycles (2.0s) */
    WINDOW_4KCLK_gc = (0x0A<<4),  /* 4K cycles (4.1s) */
    WINDOW_8KCLK_gc = (0x0B<<4)  /* 8K cycles (8.2s) */
} WINDOW_t;


/*
--------------------------------------------------------------------------
LOCKBIT - Lockbit
--------------------------------------------------------------------------
*/

/* Lockbit */
typedef struct LOCKBIT_struct
{
    register8_t LOCKBIT;  /* Lock bits */
} LOCKBIT_t;

/* Lock Bits select */
typedef enum LB_enum
{
    LB_RWLOCK_gc = (0x3A<<0),  /* Read and write lock */
    LB_NOLOCK_gc = (0xC5<<0)  /* No locks */
} LB_t;

/*
--------------------------------------------------------------------------
NVMCTRL - Non-volatile Memory Controller
--------------------------------------------------------------------------
*/

/* Non-volatile Memory Controller */
typedef struct NVMCTRL_struct
{
    register8_t CTRLA;  /* Control A */
    register8_t CTRLB;  /* Control B */
    register8_t STATUS;  /* Status */
    register8_t INTCTRL;  /* Interrupt Control */
    register8_t INTFLAGS;  /* Interrupt Flags */
    register8_t reserved_1[1];
    _WORDREGISTER(DATA);  /* Data */
    _WORDREGISTER(ADDR);  /* Address */
    register8_t reserved_2[6];
} NVMCTRL_t;

/* Command select */
typedef enum NVMCTRL_CMD_enum
{
    NVMCTRL_CMD_NONE_gc = (0x00<<0),  /* No Command */
    NVMCTRL_CMD_PAGEWRITE_gc = (0x01<<0),  /* Write page */
    NVMCTRL_CMD_PAGEERASE_gc = (0x02<<0),  /* Erase page */
    NVMCTRL_CMD_PAGEERASEWRITE_gc = (0x03<<0),  /* Erase and write page */
    NVMCTRL_CMD_PAGEBUFCLR_gc = (0x04<<0),  /* Page buffer clear */
    NVMCTRL_CMD_CHIPERASE_gc = (0x05<<0),  /* Chip erase */
    NVMCTRL_CMD_EEERASE_gc = (0x06<<0),  /* EEPROM erase */
    NVMCTRL_CMD_FUSEWRITE_gc = (0x07<<0)  /* Write fuse (PDI only) */
} NVMCTRL_CMD_t;

/*
--------------------------------------------------------------------------
PORT - I/O Ports
--------------------------------------------------------------------------
*/

/* I/O Ports */
typedef struct PORT_struct
{
    register8_t DIR;  /* Data Direction */
    register8_t DIRSET;  /* Data Direction Set */
    register8_t DIRCLR;  /* Data Direction Clear */
    register8_t DIRTGL;  /* Data Direction Toggle */
    register8_t OUT;  /* Output Value */
    register8_t OUTSET;  /* Output Value Set */
    register8_t OUTCLR;  /* Output Value Clear */
    register8_t OUTTGL;  /* Output Value Toggle */
    register8_t IN;  /* Input Value */
    register8_t INTFLAGS;  /* Interrupt Flags */
    register8_t reserved_1[6];
    register8_t PIN0CTRL;  /* Pin 0 Control */
    register8_t PIN1CTRL;  /* Pin 1 Control */
    register8_t PIN2CTRL;  /* Pin 2 Control */
    register8_t PIN3CTRL;  /* Pin 3 Control */
    register8_t PIN4CTRL;  /* Pin 4 Control */
    register8_t PIN5CTRL;  /* Pin 5 Control */
    register8_t PIN6CTRL;  /* Pin 6 Control */
    register8_t PIN7CTRL;  /* Pin 7 Control */
    register8_t reserved_2[8];
} PORT_t;

/* Input/Sense Configuration select */
typedef enum PORT_ISC_enum
{
    PORT_ISC_INTDISABLE_gc = (0x00<<0),  /* Interrupt disabled but input buffer enabled */
    PORT_ISC_BOTHEDGES_gc = (0x01<<0),  /* Sense Both Edges */
    PORT_ISC_RISING_gc = (0x02<<0),  /* Sense Rising Edge */
    PORT_ISC_FALLING_gc = (0x03<<0),  /* Sense Falling Edge */
    PORT_ISC_INPUT_DISABLE_gc = (0x04<<0),  /* Digital Input Buffer disabled */
    PORT_ISC_LEVEL_gc = (0x05<<0)  /* Sense low Level */
} PORT_ISC_t;

/*
--------------------------------------------------------------------------
PORTMUX - Port Multiplexer
--------------------------------------------------------------------------
*/

/* Port Multiplexer */
typedef struct PORTMUX_struct
{
    register8_t CTRLA;  /* Port Multiplexer Control A */
    register8_t CTRLB;  /* Port Multiplexer Control B */
    register8_t CTRLC;  /* Port Multiplexer Control C */
    register8_t CTRLD;  /* Port Multiplexer Control D */
    register8_t reserved_1[12];
} PORTMUX_t;

/* Configurable Custom Logic LUT0 select */
typedef enum PORTMUX_LUT0_enum
{
    PORTMUX_LUT0_DEFAULT_gc = (0x00<<4),  /* Default pin */
    PORTMUX_LUT0_ALTERNATE_gc = (0x01<<4)  /* Alternate pin */
} PORTMUX_LUT0_t;

/* Configurable Custom Logic LUT1 select */
typedef enum PORTMUX_LUT1_enum
{
    PORTMUX_LUT1_DEFAULT_gc = (0x00<<5),  /* Default pin */
    PORTMUX_LUT1_ALTERNATE_gc = (0x01<<5)  /* Alternate pin */
} PORTMUX_LUT1_t;

/* Port Multiplexer SPI0 select */
typedef enum PORTMUX_SPI0_enum
{
    PORTMUX_SPI0_DEFAULT_gc = (0x00<<2),  /* Default pins */
    PORTMUX_SPI0_ALTERNATE_gc = (0x01<<2)  /* Alternate pins */
} PORTMUX_SPI0_t;

/* Port Multiplexer TCA0 Output 0 select */
typedef enum PORTMUX_TCA00_enum
{
    PORTMUX_TCA00_DEFAULT_gc = (0x00<<0),  /* Default pin */
    PORTMUX_TCA00_ALTERNATE_gc = (0x01<<0)  /* Alternate pin */
} PORTMUX_TCA00_t;

/* Port Multiplexer TCA0 Output 1 select */
typedef enum PORTMUX_TCA01_enum
{
    PORTMUX_TCA01_DEFAULT_gc = (0x00<<1),  /* Default pin */
    PORTMUX_TCA01_ALTERNATE_gc = (0x01<<1)  /* Alternate pin */
} PORTMUX_TCA01_t;

/* Port Multiplexer TCA0 Output 2 select */
typedef enum PORTMUX_TCA02_enum
{
    PORTMUX_TCA02_DEFAULT_gc = (0x00<<2),  /* Default pin */
    PORTMUX_TCA02_ALTERNATE_gc = (0x01<<2)  /* Alternate pin */
} PORTMUX_TCA02_t;

/* Port Multiplexer TCA0 Output 3 select */
typedef enum PORTMUX_TCA03_enum
{
    PORTMUX_TCA03_DEFAULT_gc = (0x00<<3),  /* Default pin */
    PORTMUX_TCA03_ALTERNATE_gc = (0x01<<3)  /* Alternate pin */
} PORTMUX_TCA03_t;

/* Port Multiplexer TCA0 Output 4 select */
typedef enum PORTMUX_TCA04_enum
{
    PORTMUX_TCA04_DEFAULT_gc = (0x00<<4),  /* Default pin */
    PORTMUX_TCA04_ALTERNATE_gc = (0x01<<4)  /* Alternate pin */
} PORTMUX_TCA04_t;

/* Port Multiplexer TCA0 Output 5 select */
typedef enum PORTMUX_TCA05_enum
{
    PORTMUX_TCA05_DEFAULT_gc = (0x00<<5),  /* Default pin */
    PORTMUX_TCA05_ALTERNATE_gc = (0x01<<5)  /* Alternate pin */
} PORTMUX_TCA05_t;

/* Port Multiplexer TCB select */
typedef enum PORTMUX_TCB0_enum
{
    PORTMUX_TCB0_DEFAULT_gc = (0x00<<0),  /* Default pin */
    PORTMUX_TCB0_ALTERNATE_gc = (0x01<<0)  /* Alternate pin */
} PORTMUX_TCB0_t;

/* Port Multiplexer USART0 select */
typedef enum PORTMUX_USART0_enum
{
    PORTMUX_USART0_DEFAULT_gc = (0x00<<0),  /* Default pins */
    PORTMUX_USART0_ALTERNATE_gc = (0x01<<0)  /* Alternate pins */
} PORTMUX_USART0_t;

/*
--------------------------------------------------------------------------
RSTCTRL - Reset controller
--------------------------------------------------------------------------
*/

/* Reset controller */
typedef struct RSTCTRL_struct
{
    register8_t RSTFR;  /* Reset Flags */
    register8_t SWRR;  /* Software Reset */
    register8_t reserved_1[2];
} RSTCTRL_t;


/*
--------------------------------------------------------------------------
RTC - Real-Time Counter
--------------------------------------------------------------------------
*/

/* Real-Time Counter */
typedef struct RTC_struct
{
    register8_t CTRLA;  /* Control A */
    register8_t STATUS;  /* Status */
    register8_t INTCTRL;  /* Interrupt Control */
    register8_t INTFLAGS;  /* Interrupt Flags */
    register8_t TEMP;  /* Temporary */
    register8_t DBGCTRL;  /* Debug control */
    register8_t reserved_1[1];
    register8_t CLKSEL;  /* Clock Select */
    _WORDREGISTER(CNT);  /* Counter */
    _WORDREGISTER(PER);  /* Period */
    _WORDREGISTER(CMP);  /* Compare */
    register8_t reserved_2[2];
    register8_t PITCTRLA;  /* PIT Control A */
    register8_t PITSTATUS;  /* PIT Status */
    register8_t PITINTCTRL;  /* PIT Interrupt Control */
    register8_t PITINTFLAGS;  /* PIT Interrupt Flags */
    register8_t reserved_3[1];
    register8_t PITDBGCTRL;  /* PIT Debug control */
    register8_t reserved_4[10];
} RTC_t;

/* Clock Select */
typedef enum RTC_CLKSEL_enum
{
    RTC_CLKSEL_INT32K_gc = (0x00<<0),  /* Internal 32kHz OSC */
    RTC_CLKSEL_INT1K_gc = (0x01<<0),  /* Internal 1kHz OSC */
    RTC_CLKSEL_EXTCLK_gc = (0x03<<0)  /* External Clock */
} RTC_CLKSEL_t;

/* Period select */
typedef enum RTC_PERIOD_enum
{
    RTC_PERIOD_OFF_gc = (0x00<<3),  /* Off */
    RTC_PERIOD_CYC4_gc = (0x01<<3),  /* RTC Clock Cycles 4 */
    RTC_PERIOD_CYC8_gc = (0x02<<3),  /* RTC Clock Cycles 8 */
    RTC_PERIOD_CYC16_gc = (0x03<<3),  /* RTC Clock Cycles 16 */
    RTC_PERIOD_CYC32_gc = (0x04<<3),  /* RTC Clock Cycles 32 */
    RTC_PERIOD_CYC64_gc = (0x05<<3),  /* RTC Clock Cycles 64 */
    RTC_PERIOD_CYC128_gc = (0x06<<3),  /* RTC Clock Cycles 128 */
    RTC_PERIOD_CYC256_gc = (0x07<<3),  /* RTC Clock Cycles 256 */
    RTC_PERIOD_CYC512_gc = (0x08<<3),  /* RTC Clock Cycles 512 */
    RTC_PERIOD_CYC1024_gc = (0x09<<3),  /* RTC Clock Cycles 1024 */
    RTC_PERIOD_CYC2048_gc = (0x0A<<3),  /* RTC Clock Cycles 2048 */
    RTC_PERIOD_CYC4096_gc = (0x0B<<3),  /* RTC Clock Cycles 4096 */
    RTC_PERIOD_CYC8192_gc = (0x0C<<3),  /* RTC Clock Cycles 8192 */
    RTC_PERIOD_CYC16384_gc = (0x0D<<3),  /* RTC Clock Cycles 16384 */
    RTC_PERIOD_CYC32768_gc = (0x0E<<3)  /* RTC Clock Cycles 32768 */
} RTC_PERIOD_t;

/* Prescaling Factor select */
typedef enum RTC_PRESCALER_enum
{
    RTC_PRESCALER_DIV1_gc = (0x00<<3),  /* RTC Clock / 1 */
    RTC_PRESCALER_DIV2_gc = (0x01<<3),  /* RTC Clock / 2 */
    RTC_PRESCALER_DIV4_gc = (0x02<<3),  /* RTC Clock / 4 */
    RTC_PRESCALER_DIV8_gc = (0x03<<3),  /* RTC Clock / 8 */
    RTC_PRESCALER_DIV16_gc = (0x04<<3),  /* RTC Clock / 16 */
    RTC_PRESCALER_DIV32_gc = (0x05<<3),  /* RTC Clock / 32 */
    RTC_PRESCALER_DIV64_gc = (0x06<<3),  /* RTC Clock / 64 */
    RTC_PRESCALER_DIV128_gc = (0x07<<3),  /* RTC Clock / 128 */
    RTC_PRESCALER_DIV256_gc = (0x08<<3),  /* RTC Clock / 256 */
    RTC_PRESCALER_DIV512_gc = (0x09<<3),  /* RTC Clock / 512 */
    RTC_PRESCALER_DIV1024_gc = (0x0A<<3),  /* RTC Clock / 1024 */
    RTC_PRESCALER_DIV2048_gc = (0x0B<<3),  /* RTC Clock / 2048 */
    RTC_PRESCALER_DIV4096_gc = (0x0C<<3),  /* RTC Clock / 4096 */
    RTC_PRESCALER_DIV8192_gc = (0x0D<<3),  /* RTC Clock / 8192 */
    RTC_PRESCALER_DIV16384_gc = (0x0E<<3),  /* RTC Clock / 16384 */
    RTC_PRESCALER_DIV32768_gc = (0x0F<<3)  /* RTC Clock / 32768 */
} RTC_PRESCALER_t;

/*
--------------------------------------------------------------------------
SIGROW - Signature row
--------------------------------------------------------------------------
*/

/* Signature row */
typedef struct SIGROW_struct
{
    register8_t DEVICEID0;  /* Device ID Byte 0 */
    register8_t DEVICEID1;  /* Device ID Byte 1 */
    register8_t DEVICEID2;  /* Device ID Byte 2 */
    register8_t SERNUM0;  /* Serial Number Byte 0 */
    register8_t SERNUM1;  /* Serial Number Byte 1 */
    register8_t SERNUM2;  /* Serial Number Byte 2 */
    register8_t SERNUM3;  /* Serial Number Byte 3 */
    register8_t SERNUM4;  /* Serial Number Byte 4 */
    register8_t SERNUM5;  /* Serial Number Byte 5 */
    register8_t SERNUM6;  /* Serial Number Byte 6 */
    register8_t SERNUM7;  /* Serial Number Byte 7 */
    register8_t SERNUM8;  /* Serial Number Byte 8 */
    register8_t SERNUM9;  /* Serial Number Byte 9 */
    register8_t reserved_1[19];
    register8_t TEMPSENSE0;  /* Temperature Sensor Calibration Byte 0 */
    register8_t TEMPSENSE1;  /* Temperature Sensor Calibration Byte 1 */
    register8_t OSC16ERR3V;  /* OSC16 error at 3V */
    register8_t OSC16ERR5V;  /* OSC16 error at 5V */
    register8_t OSC20ERR3V;  /* OSC20 error at 3V */
    register8_t OSC20ERR5V;  /* OSC20 error at 5V */
    register8_t reserved_2[26];
} SIGROW_t;


/*
--------------------------------------------------------------------------
SLPCTRL - Sleep Controller
--------------------------------------------------------------------------
*/

/* Sleep Controller */
typedef struct SLPCTRL_struct
{
    register8_t CTRLA;  /* Control */
    register8_t reserved_1[1];
} SLPCTRL_t;

/* Sleep mode select */
typedef enum SLPCTRL_SMODE_enum
{
    SLPCTRL_SMODE_IDLE_gc = (0x00<<1),  /* Idle mode */
    SLPCTRL_SMODE_STDBY_gc = (0x01<<1),  /* Standby Mode */
    SLPCTRL_SMODE_PDOWN_gc = (0x02<<1)  /* Power-down Mode */
} SLPCTRL_SMODE_t;

#define SLEEP_MODE_IDLE (0x00<<1)
#define SLEEP_MODE_STANDBY (0x01<<1)
#define SLEEP_MODE_PWR_DOWN (0x02<<1)
/*
--------------------------------------------------------------------------
SPI - Serial Peripheral Interface
--------------------------------------------------------------------------
*/

/* Serial Peripheral Interface */
typedef struct SPI_struct
{
    register8_t CTRLA;  /* Control A */
    register8_t CTRLB;  /* Control B */
    register8_t INTCTRL;  /* Interrupt Control */
    register8_t INTFLAGS;  /* Interrupt Flags */
    register8_t DATA;  /* Data */
    register8_t reserved_1[3];
} SPI_t;

/* SPI Mode select */
typedef enum SPI_MODE_enum
{
    SPI_MODE_0_gc = (0x00<<0),  /* SPI Mode 0 */
    SPI_MODE_1_gc = (0x01<<0),  /* SPI Mode 1 */
    SPI_MODE_2_gc = (0x02<<0),  /* SPI Mode 2 */
    SPI_MODE_3_gc = (0x03<<0)  /* SPI Mode 3 */
} SPI_MODE_t;

/* Prescaler select */
typedef enum SPI_PRESC_enum
{
    SPI_PRESC_DIV4_gc = (0x00<<1),  /* System Clock / 4 */
    SPI_PRESC_DIV16_gc = (0x01<<1),  /* System Clock / 16 */
    SPI_PRESC_DIV64_gc = (0x02<<1),  /* System Clock / 64 */
    SPI_PRESC_DIV128_gc = (0x03<<1)  /* System Clock / 128 */
} SPI_PRESC_t;

/*
--------------------------------------------------------------------------
SYSCFG - System Configuration Registers
--------------------------------------------------------------------------
*/

/* System Configuration Registers */
typedef struct SYSCFG_struct
{
    register8_t reserved_1[1];
    register8_t REVID;  /* Revision ID */
    register8_t EXTBRK;  /* External Break */
    register8_t reserved_2[29];
} SYSCFG_t;


/*
--------------------------------------------------------------------------
TCA - 16-bit Timer/Counter Type A
--------------------------------------------------------------------------
*/

/* 16-bit Timer/Counter Type A - Single Mode */
typedef struct TCA_SINGLE_struct
{
    register8_t CTRLA;  /* Control A */
    register8_t CTRLB;  /* Control B */
    register8_t CTRLC;  /* Control C */
    register8_t CTRLD;  /* Control D */
    register8_t CTRLECLR;  /* Control E Clear */
    register8_t CTRLESET;  /* Control E Set */
    register8_t CTRLFCLR;  /* Control F Clear */
    register8_t CTRLFSET;  /* Control F Set */
    register8_t reserved_1[1];
    register8_t EVCTRL;  /* Event Control */
    register8_t INTCTRL;  /* Interrupt Control */
    register8_t INTFLAGS;  /* Interrupt Flags */
    register8_t reserved_2[2];
    register8_t DBGCTRL;  /* Degbug Control */
    register8_t TEMP;  /* Temporary data for 16-bit Access */
    register8_t reserved_3[16];
    _WORDREGISTER(CNT);  /* Count */
    register8_t reserved_4[4];
    _WORDREGISTER(PER);  /* Period */
    _WORDREGISTER(CMP0);  /* Compare 0 */
    _WORDREGISTER(CMP1);  /* Compare 1 */
    _WORDREGISTER(CMP2);  /* Compare 2 */
    register8_t reserved_5[8];
    _WORDREGISTER(PERBUF);  /* Period Buffer */
    _WORDREGISTER(CMP0BUF);  /* Compare 0 Buffer */
    _WORDREGISTER(CMP1BUF);  /* Compare 1 Buffer */
    _WORDREGISTER(CMP2BUF);  /* Compare 2 Buffer */
    register8_t reserved_6[2];
} TCA_SINGLE_t;

/* 16-bit Timer/Counter Type A - Split Mode */
typedef struct TCA_SPLIT_struct
{
    register8_t CTRLA;  /* Control A */
    register8_t CTRLB;  /* Control B */
    register8_t CTRLC;  /* Control C */
    register8_t CTRLD;  /* Control D */
    register8_t CTRLECLR;  /* Control E Clear */
    register8_t CTRLESET;  /* Control E Set */
    register8_t reserved_1[4];
    register8_t INTCTRL;  /* Interrupt Control */
    register8_t INTFLAGS;  /* Interrupt Flags */
    register8_t reserved_2[2];
    register8_t DBGCTRL;  /* Degbug Control */
    register8_t reserved_3[17];
    register8_t LCNT;  /* Low Count */
    register8_t HCNT;  /* High Count */
    register8_t reserved_4[4];
    register8_t LPER;  /* Low Period */
    register8_t HPER;  /* High Period */
    register8_t LCMP0;  /* Low Compare */
    register8_t HCMP0;  /* High Compare */
    register8_t LCMP1;  /* Low Compare */
    register8_t HCMP1;  /* High Compare */
    register8_t LCMP2;  /* Low Compare */
    register8_t HCMP2;  /* High Compare */
    register8_t reserved_5[18];
} TCA_SPLIT_t;

/* 16-bit Timer/Counter Type A */
typedef union TCA_union
{
    TCA_SINGLE_t SINGLE;  /* Single Mode */
    TCA_SPLIT_t SPLIT;  /* Split Mode */
} TCA_t;

/* Clock Selection */
typedef enum TCA_SINGLE_CLKSEL_enum
{
    TCA_SINGLE_CLKSEL_DIV1_gc = (0x00<<1),  /* System Clock */
    TCA_SINGLE_CLKSEL_DIV2_gc = (0x01<<1),  /* System Clock / 2 */
    TCA_SINGLE_CLKSEL_DIV4_gc = (0x02<<1),  /* System Clock / 4 */
    TCA_SINGLE_CLKSEL_DIV8_gc = (0x03<<1),  /* System Clock / 8 */
    TCA_SINGLE_CLKSEL_DIV16_gc = (0x04<<1),  /* System Clock / 16 */
    TCA_SINGLE_CLKSEL_DIV64_gc = (0x05<<1),  /* System Clock / 64 */
    TCA_SINGLE_CLKSEL_DIV256_gc = (0x06<<1),  /* System Clock / 256 */
    TCA_SINGLE_CLKSEL_DIV1024_gc = (0x07<<1)  /* System Clock / 1024 */
} TCA_SINGLE_CLKSEL_t;

/* Command select */
typedef enum TCA_SINGLE_CMD_enum
{
    TCA_SINGLE_CMD_NONE_gc = (0x00<<2),  /* No Command */
    TCA_SINGLE_CMD_UPDATE_gc = (0x01<<2),  /* Force Update */
    TCA_SINGLE_CMD_RESTART_gc = (0x02<<2),  /* Force Restart */
    TCA_SINGLE_CMD_RESET_gc = (0x03<<2)  /* Force Hard Reset */
} TCA_SINGLE_CMD_t;

/* Direction select */
typedef enum TCA_SINGLE_DIR_enum
{
    TCA_SINGLE_DIR_UP_gc = (0x00<<0),  /* Count up */
    TCA_SINGLE_DIR_DOWN_gc = (0x01<<0)  /* Count down */
} TCA_SINGLE_DIR_t;

/* Event Action select */
typedef enum TCA_SINGLE_EVACT_enum
{
    TCA_SINGLE_EVACT_POSEDGE_gc = (0x00<<1),  /* Count on positive edge event */
    TCA_SINGLE_EVACT_ANYEDGE_gc = (0x01<<1),  /* Count on any edge event */
    TCA_SINGLE_EVACT_HIGHLVL_gc = (0x02<<1),  /* Count on prescaled clock while event line is 1. */
    TCA_SINGLE_EVACT_UPDOWN_gc = (0x03<<1)  /* Count on prescaled clock. Event controls count direction. Up-count when event line is 0, down-count when event line is 1. */
} TCA_SINGLE_EVACT_t;

/* Waveform generation mode select */
typedef enum TCA_SINGLE_WGMODE_enum
{
    TCA_SINGLE_WGMODE_NORMAL_gc = (0x00<<0),  /* Normal Mode */
    TCA_SINGLE_WGMODE_FRQ_gc = (0x01<<0),  /* Frequency Generation Mode */
    TCA_SINGLE_WGMODE_SINGLESLOPE_gc = (0x03<<0),  /* Single Slope PWM */
    TCA_SINGLE_WGMODE_DSTOP_gc = (0x05<<0),  /* Dual Slope PWM, overflow on TOP */
    TCA_SINGLE_WGMODE_DSBOTH_gc = (0x06<<0),  /* Dual Slope PWM, overflow on TOP and BOTTOM */
    TCA_SINGLE_WGMODE_DSBOTTOM_gc = (0x07<<0)  /* Dual Slope PWM, overflow on BOTTOM */
} TCA_SINGLE_WGMODE_t;

/* Clock Selection */
typedef enum TCA_SPLIT_CLKSEL_enum
{
    TCA_SPLIT_CLKSEL_DIV1_gc = (0x00<<1),  /* System Clock */
    TCA_SPLIT_CLKSEL_DIV2_gc = (0x01<<1),  /* System Clock / 2 */
    TCA_SPLIT_CLKSEL_DIV4_gc = (0x02<<1),  /* System Clock / 4 */
    TCA_SPLIT_CLKSEL_DIV8_gc = (0x03<<1),  /* System Clock / 8 */
    TCA_SPLIT_CLKSEL_DIV16_gc = (0x04<<1),  /* System Clock / 16 */
    TCA_SPLIT_CLKSEL_DIV64_gc = (0x05<<1),  /* System Clock / 64 */
    TCA_SPLIT_CLKSEL_DIV256_gc = (0x06<<1),  /* System Clock / 256 */
    TCA_SPLIT_CLKSEL_DIV1024_gc = (0x07<<1)  /* System Clock / 1024 */
} TCA_SPLIT_CLKSEL_t;

/* Command select */
typedef enum TCA_SPLIT_CMD_enum
{
    TCA_SPLIT_CMD_NONE_gc = (0x00<<2),  /* No Command */
    TCA_SPLIT_CMD_UPDATE_gc = (0x01<<2),  /* Force Update */
    TCA_SPLIT_CMD_RESTART_gc = (0x02<<2),  /* Force Restart */
    TCA_SPLIT_CMD_RESET_gc = (0x03<<2)  /* Force Hard Reset */
} TCA_SPLIT_CMD_t;

/*
--------------------------------------------------------------------------
TCB - 16-bit Timer Type B
--------------------------------------------------------------------------
*/

/* 16-bit Timer Type B */
typedef struct TCB_struct
{
    register8_t CTRLA;  /* Control A */
    register8_t CTRLB;  /* Control Register B */
    register8_t reserved_1[2];
    register8_t EVCTRL;  /* Event Control */
    register8_t INTCTRL;  /* Interrupt Control */
    register8_t INTFLAGS;  /* Interrupt Flags */
    register8_t STATUS;  /* Status */
    register8_t DBGCTRL;  /* Debug Control */
    register8_t TEMP;  /* Temporary Value */
    _WORDREGISTER(CNT);  /* Count */
    _WORDREGISTER(CCMP);  /* Compare or Capture */
    register8_t reserved_2[2];
} TCB_t;

/* Clock Select */
typedef enum TCB_CLKSEL_enum
{
    TCB_CLKSEL_CLKDIV1_gc = (0x00<<1),  /* CLK_PER (No Prescaling) */
    TCB_CLKSEL_CLKDIV2_gc = (0x01<<1),  /* CLK_PER/2 (From Prescaler) */
    TCB_CLKSEL_CLKTCA_gc = (0x02<<1)  /* Use Clock from TCA */
} TCB_CLKSEL_t;

/* Timer Mode select */
typedef enum TCB_CNTMODE_enum
{
    TCB_CNTMODE_INT_gc = (0x00<<0),  /* Periodic Interrupt */
    TCB_CNTMODE_TIMEOUT_gc = (0x01<<0),  /* Periodic Timeout */
    TCB_CNTMODE_CAPT_gc = (0x02<<0),  /* Input Capture Event */
    TCB_CNTMODE_FRQ_gc = (0x03<<0),  /* Input Capture Frequency measurement */
    TCB_CNTMODE_PW_gc = (0x04<<0),  /* Input Capture Pulse-Width measurement */
    TCB_CNTMODE_FRQPW_gc = (0x05<<0),  /* Input Capture Frequency and Pulse-Width measurement */
    TCB_CNTMODE_SINGLE_gc = (0x06<<0),  /* Single Shot */
    TCB_CNTMODE_PWM8_gc = (0x07<<0)  /* 8-bit PWM */
} TCB_CNTMODE_t;

/*
--------------------------------------------------------------------------
TWI - Two-Wire Interface
--------------------------------------------------------------------------
*/

/* Two-Wire Interface */
typedef struct TWI_struct
{
    register8_t CTRLA;  /* Control A */
    register8_t reserved_1[1];
    register8_t DBGCTRL;  /* Debug Control Register */
    register8_t MCTRLA;  /* Host Control A */
    register8_t MCTRLB;  /* Host Control B */
    register8_t MSTATUS;  /* Host Status */
    register8_t MBAUD;  /* Host Baud Rate Control */
    register8_t MADDR;  /* Host Address */
    register8_t MDATA;  /* Host Data */
    register8_t SCTRLA;  /* Client Control A */
    register8_t SCTRLB;  /* Client Control B */
    register8_t SSTATUS;  /* Client Status */
    register8_t SADDR;  /* Client Address */
    register8_t SDATA;  /* Client Data */
    register8_t SADDRMASK;  /* Client Address Mask */
    register8_t reserved_2[1];
} TWI_t;

/* Acknowledge Action select */
typedef enum TWI_ACKACT_enum
{
    TWI_ACKACT_ACK_gc = (0x00<<2),  /* Send ACK */
    TWI_ACKACT_NACK_gc = (0x01<<2)  /* Send NACK */
} TWI_ACKACT_t;

/* Client Address or Stop select */
typedef enum TWI_AP_enum
{
    TWI_AP_STOP_gc = (0x00<<0),  /* Stop condition generated APIF */
    TWI_AP_ADR_gc = (0x01<<0)  /* Address detection generated APIF */
} TWI_AP_t;

/* Bus State select */
typedef enum TWI_BUSSTATE_enum
{
    TWI_BUSSTATE_UNKNOWN_gc = (0x00<<0),  /* Unknown Bus State */
    TWI_BUSSTATE_IDLE_gc = (0x01<<0),  /* Bus is Idle */
    TWI_BUSSTATE_OWNER_gc = (0x02<<0),  /* This Module Controls The Bus */
    TWI_BUSSTATE_BUSY_gc = (0x03<<0)  /* The Bus is Busy */
} TWI_BUSSTATE_t;

/* Command select */
typedef enum TWI_MCMD_enum
{
    TWI_MCMD_NOACT_gc = (0x00<<0),  /* No Action */
    TWI_MCMD_REPSTART_gc = (0x01<<0),  /* Issue Repeated Start Condition */
    TWI_MCMD_RECVTRANS_gc = (0x02<<0),  /* Receive or Transmit Data, depending on DIR */
    TWI_MCMD_STOP_gc = (0x03<<0)  /* Issue Stop Condition */
} TWI_MCMD_t;

/* Command select */
typedef enum TWI_SCMD_enum
{
    TWI_SCMD_NOACT_gc = (0x00<<0),  /* No Action */
    TWI_SCMD_COMPTRANS_gc = (0x02<<0),  /* Used To Complete a Transaction */
    TWI_SCMD_RESPONSE_gc = (0x03<<0)  /* Used in Response to Address/Data Interrupt */
} TWI_SCMD_t;

/* SDA Hold Time select */
typedef enum TWI_SDAHOLD_enum
{
    TWI_SDAHOLD_OFF_gc = (0x00<<2),  /* SDA hold time off */
    TWI_SDAHOLD_50NS_gc = (0x01<<2),  /* Typical 50ns hold time */
    TWI_SDAHOLD_300NS_gc = (0x02<<2),  /* Typical 300ns hold time */
    TWI_SDAHOLD_500NS_gc = (0x03<<2)  /* Typical 500ns hold time */
} TWI_SDAHOLD_t;

/* SDA Setup Time select */
typedef enum TWI_SDASETUP_enum
{
    TWI_SDASETUP_4CYC_gc = (0x00<<4),  /* SDA setup time is 4 clock cycles */
    TWI_SDASETUP_8CYC_gc = (0x01<<4)  /* SDA setup time is 8 clock cycles */
} TWI_SDASETUP_t;

/* Inactive Bus Timeout select */
typedef enum TWI_TIMEOUT_enum
{
    TWI_TIMEOUT_DISABLED_gc = (0x00<<2),  /* Bus Timeout Disabled */
    TWI_TIMEOUT_50US_gc = (0x01<<2),  /* 50 Microseconds */
    TWI_TIMEOUT_100US_gc = (0x02<<2),  /* 100 Microseconds */
    TWI_TIMEOUT_200US_gc = (0x03<<2)  /* 200 Microseconds */
} TWI_TIMEOUT_t;

/*
--------------------------------------------------------------------------
USART - Universal Synchronous and Asynchronous Receiver and Transmitter
--------------------------------------------------------------------------
*/

/* Universal Synchronous and Asynchronous Receiver and Transmitter */
typedef struct USART_struct
{
    register8_t RXDATAL;  /* Receive Data Low Byte */
    register8_t RXDATAH;  /* Receive Data High Byte */
    register8_t TXDATAL;  /* Transmit Data Low Byte */
    register8_t TXDATAH;  /* Transmit Data High Byte */
    register8_t STATUS;  /* Status */
    register8_t CTRLA;  /* Control A */
    register8_t CTRLB;  /* Control B */
    register8_t CTRLC;  /* Control C */
    _WORDREGISTER(BAUD);  /* Baud Rate */
    register8_t reserved_1[1];
    register8_t DBGCTRL;  /* Debug Control */
    register8_t EVCTRL;  /* Event Control */
    register8_t TXPLCTRL;  /* IRCOM Transmitter Pulse Length Control */
    register8_t RXPLCTRL;  /* IRCOM Receiver Pulse Length Control */
    register8_t reserved_2[1];
} USART_t;

/* Character Size select */
typedef enum USART_CHSIZE_enum
{
    USART_CHSIZE_5BIT_gc = (0x00<<0),  /* Character size: 5 bit */
    USART_CHSIZE_6BIT_gc = (0x01<<0),  /* Character size: 6 bit */
    USART_CHSIZE_7BIT_gc = (0x02<<0),  /* Character size: 7 bit */
    USART_CHSIZE_8BIT_gc = (0x03<<0),  /* Character size: 8 bit */
    USART_CHSIZE_9BITL_gc = (0x06<<0),  /* Character size: 9 bit read low byte first */
    USART_CHSIZE_9BITH_gc = (0x07<<0)  /* Character size: 9 bit read high byte first */
} USART_CHSIZE_t;

/* Communication Mode select */
typedef enum USART_CMODE_enum
{
    USART_CMODE_ASYNCHRONOUS_gc = (0x00<<6),  /* Asynchronous Mode */
    USART_CMODE_SYNCHRONOUS_gc = (0x01<<6),  /* Synchronous Mode */
    USART_CMODE_IRCOM_gc = (0x02<<6),  /* Infrared Communication */
    USART_CMODE_MSPI_gc = (0x03<<6)  /* SPI Host Mode */
} USART_CMODE_t;

/* Parity Mode select */
typedef enum USART_PMODE_enum
{
    USART_PMODE_DISABLED_gc = (0x00<<4),  /* No Parity */
    USART_PMODE_EVEN_gc = (0x02<<4),  /* Even Parity */
    USART_PMODE_ODD_gc = (0x03<<4)  /* Odd Parity */
} USART_PMODE_t;

/* RS485 Mode internal transmitter select */
typedef enum USART_RS485_enum
{
    USART_RS485_OFF_gc = (0x00<<0),  /* RS485 Mode disabled */
    USART_RS485_EXT_gc = (0x01<<0),  /* RS485 Mode External drive */
    USART_RS485_INT_gc = (0x02<<0)  /* RS485 Mode Internal drive */
} USART_RS485_t;

/* Receiver Mode select */
typedef enum USART_RXMODE_enum
{
    USART_RXMODE_NORMAL_gc = (0x00<<1),  /* Normal mode */
    USART_RXMODE_CLK2X_gc = (0x01<<1),  /* CLK2x mode */
    USART_RXMODE_GENAUTO_gc = (0x02<<1),  /* Generic autobaud mode */
    USART_RXMODE_LINAUTO_gc = (0x03<<1)  /* LIN constrained autobaud mode */
} USART_RXMODE_t;

/* Stop Bit Mode select */
typedef enum USART_SBMODE_enum
{
    USART_SBMODE_1BIT_gc = (0x00<<3),  /* 1 stop bit */
    USART_SBMODE_2BIT_gc = (0x01<<3)  /* 2 stop bits */
} USART_SBMODE_t;

/*
--------------------------------------------------------------------------
USERROW - User Row
--------------------------------------------------------------------------
*/

/* User Row */
typedef struct USERROW_struct
{
    register8_t USERROW0;  /* User Row Byte 0 */
    register8_t USERROW1;  /* User Row Byte 1 */
    register8_t USERROW2;  /* User Row Byte 2 */
    register8_t USERROW3;  /* User Row Byte 3 */
    register8_t USERROW4;  /* User Row Byte 4 */
    register8_t USERROW5;  /* User Row Byte 5 */
    register8_t USERROW6;  /* User Row Byte 6 */
    register8_t USERROW7;  /* User Row Byte 7 */
    register8_t USERROW8;  /* User Row Byte 8 */
    register8_t USERROW9;  /* User Row Byte 9 */
    register8_t USERROW10;  /* User Row Byte 10 */
    register8_t USERROW11;  /* User Row Byte 11 */
    register8_t USERROW12;  /* User Row Byte 12 */
    register8_t USERROW13;  /* User Row Byte 13 */
    register8_t USERROW14;  /* User Row Byte 14 */
    register8_t USERROW15;  /* User Row Byte 15 */
    register8_t USERROW16;  /* User Row Byte 16 */
    register8_t USERROW17;  /* User Row Byte 17 */
    register8_t USERROW18;  /* User Row Byte 18 */
    register8_t USERROW19;  /* User Row Byte 19 */
    register8_t USERROW20;  /* User Row Byte 20 */
    register8_t USERROW21;  /* User Row Byte 21 */
    register8_t USERROW22;  /* User Row Byte 22 */
    register8_t USERROW23;  /* User Row Byte 23 */
    register8_t USERROW24;  /* User Row Byte 24 */
    register8_t USERROW25;  /* User Row Byte 25 */
    register8_t USERROW26;  /* User Row Byte 26 */
    register8_t USERROW27;  /* User Row Byte 27 */
    register8_t USERROW28;  /* User Row Byte 28 */
    register8_t USERROW29;  /* User Row Byte 29 */
    register8_t USERROW30;  /* User Row Byte 30 */
    register8_t USERROW31;  /* User Row Byte 31 */
} USERROW_t;


/*
--------------------------------------------------------------------------
VPORT - Virtual Ports
--------------------------------------------------------------------------
*/

/* Virtual Ports */
typedef struct VPORT_struct
{
    register8_t DIR;  /* Data Direction */
    register8_t OUT;  /* Output Value */
    register8_t IN;  /* Input Value */
    register8_t INTFLAGS;  /* Interrupt Flags */
} VPORT_t;


/*
--------------------------------------------------------------------------
VREF - Voltage reference
--------------------------------------------------------------------------
*/

/* Voltage reference */
typedef struct VREF_struct
{
    register8_t CTRLA;  /* Control A */
    register8_t CTRLB;  /* Control B */
    register8_t reserved_1[2];
} VREF_t;

/* ADC0 reference select */
typedef enum VREF_ADC0REFSEL_enum
{
    VREF_ADC0REFSEL_0V55_gc = (0x00<<4),  /* Voltage reference at 0.55V */
    VREF_ADC0REFSEL_1V1_gc = (0x01<<4),  /* Voltage reference at 1.1V */
    VREF_ADC0REFSEL_2V5_gc = (0x02<<4),  /* Voltage reference at 2.5V */
    VREF_ADC0REFSEL_4V34_gc = (0x03<<4),  /* Voltage reference at 4.34V */
    VREF_ADC0REFSEL_1V5_gc = (0x04<<4)  /* Voltage reference at 1.5V */
} VREF_ADC0REFSEL_t;

/* DAC0/AC0 reference select */
typedef enum VREF_DAC0REFSEL_enum
{
    VREF_DAC0REFSEL_0V55_gc = (0x00<<0),  /* Voltage reference at 0.55V */
    VREF_DAC0REFSEL_1V1_gc = (0x01<<0),  /* Voltage reference at 1.1V */
    VREF_DAC0REFSEL_2V5_gc = (0x02<<0),  /* Voltage reference at 2.5V */
    VREF_DAC0REFSEL_4V34_gc = (0x03<<0),  /* Voltage reference at 4.34V */
    VREF_DAC0REFSEL_1V5_gc = (0x04<<0)  /* Voltage reference at 1.5V */
} VREF_DAC0REFSEL_t;

/*
--------------------------------------------------------------------------
WDT - Watch-Dog Timer
--------------------------------------------------------------------------
*/

/* Watch-Dog Timer */
typedef struct WDT_struct
{
    register8_t CTRLA;  /* Control A */
    register8_t STATUS;  /* Status */
} WDT_t;

/* Period select */
typedef enum WDT_PERIOD_enum
{
    WDT_PERIOD_OFF_gc = (0x00<<0),  /* Off */
    WDT_PERIOD_8CLK_gc = (0x01<<0),  /* 8 cycles (8ms) */
    WDT_PERIOD_16CLK_gc = (0x02<<0),  /* 16 cycles (16ms) */
    WDT_PERIOD_32CLK_gc = (0x03<<0),  /* 32 cycles (32ms) */
    WDT_PERIOD_64CLK_gc = (0x04<<0),  /* 64 cycles (64ms) */
    WDT_PERIOD_128CLK_gc = (0x05<<0),  /* 128 cycles (0.128s) */
    WDT_PERIOD_256CLK_gc = (0x06<<0),  /* 256 cycles (0.256s) */
    WDT_PERIOD_512CLK_gc = (0x07<<0),  /* 512 cycles (0.512s) */
    WDT_PERIOD_1KCLK_gc = (0x08<<0),  /* 1K cycles (1.0s) */
    WDT_PERIOD_2KCLK_gc = (0x09<<0),  /* 2K cycles (2.0s) */
    WDT_PERIOD_4KCLK_gc = (0x0A<<0),  /* 4K cycles (4.1s) */
    WDT_PERIOD_8KCLK_gc = (0x0B<<0)  /* 8K cycles (8.2s) */
} WDT_PERIOD_t;

/* Window select */
typedef enum WDT_WINDOW_enum
{
    WDT_WINDOW_OFF_gc = (0x00<<4),  /* Off */
    WDT_WINDOW_8CLK_gc = (0x01<<4),  /* 8 cycles (8ms) */
    WDT_WINDOW_16CLK_gc = (0x02<<4),  /* 16 cycles (16ms) */
    WDT_WINDOW_32CLK_gc = (0x03<<4),  /* 32 cycles (32ms) */
    WDT_WINDOW_64CLK_gc = (0x04<<4),  /* 64 cycles (64ms) */
    WDT_WINDOW_128CLK_gc = (0x05<<4),  /* 128 cycles (0.128s) */
    WDT_WINDOW_256CLK_gc = (0x06<<4),  /* 256 cycles (0.256s) */
    WDT_WINDOW_512CLK_gc = (0x07<<4),  /* 512 cycles (0.512s) */
    WDT_WINDOW_1KCLK_gc = (0x08<<4),  /* 1K cycles (1.0s) */
    WDT_WINDOW_2KCLK_gc = (0x09<<4),  /* 2K cycles (2.0s) */
    WDT_WINDOW_4KCLK_gc = (0x0A<<4),  /* 4K cycles (4.1s) */
    WDT_WINDOW_8KCLK_gc = (0x0B<<4)  /* 8K cycles (8.2s) */
} WDT_WINDOW_t;
/*
==========================================================================
IO Module Instances. Mapped to memory.
==========================================================================
*/

#define VPORTA              (*(VPORT_t *) 0x0000) /* Virtual Ports */
#define VPORTB              (*(VPORT_t *) 0x0004) /* Virtual Ports */
#define VPORTC              (*(VPORT_t *) 0x0008) /* Virtual Ports */
#define RSTCTRL           (*(RSTCTRL_t *) 0x0040) /* Reset controller */
#define SLPCTRL           (*(SLPCTRL_t *) 0x0050) /* Sleep Controller */
#define CLKCTRL           (*(CLKCTRL_t *) 0x0060) /* Clock controller */
#define BOD                   (*(BOD_t *) 0x0080) /* Bod interface */
#define VREF                 (*(VREF_t *) 0x00A0) /* Voltage reference */
#define WDT                   (*(WDT_t *) 0x0100) /* Watch-Dog Timer */
#define CPUINT             (*(CPUINT_t *) 0x0110) /* Interrupt Controller */
#define CRCSCAN           (*(CRCSCAN_t *) 0x0120) /* CRCSCAN */
#define RTC                   (*(RTC_t *) 0x0140) /* Real-Time Counter */
#define EVSYS               (*(EVSYS_t *) 0x0180) /* Event System */
#define CCL                   (*(CCL_t *) 0x01C0) /* Configurable Custom Logic */
#define PORTMUX           (*(PORTMUX_t *) 0x0200) /* Port Multiplexer */
#define PORTA                (*(PORT_t *) 0x0400) /* I/O Ports */
#define PORTB                (*(PORT_t *) 0x0420) /* I/O Ports */
#define PORTC                (*(PORT_t *) 0x0440) /* I/O Ports */
#define ADC0                  (*(ADC_t *) 0x0600) /* Analog to Digital Converter */
#define AC0                    (*(AC_t *) 0x0680) /* Analog Comparator */
#define USART0              (*(USART_t *) 0x0800) /* Universal Synchronous and Asynchronous Receiver and Transmitter */
#define TWI0                  (*(TWI_t *) 0x0810) /* Two-Wire Interface */
#define SPI0                  (*(SPI_t *) 0x0820) /* Serial Peripheral Interface */
#define TCA0                  (*(TCA_t *) 0x0A00) /* 16-bit Timer/Counter Type A */
#define TCB0                  (*(TCB_t *) 0x0A40) /* 16-bit Timer Type B */
#define SYSCFG             (*(SYSCFG_t *) 0x0F00) /* System Configuration Registers */
#define NVMCTRL           (*(NVMCTRL_t *) 0x1000) /* Non-volatile Memory Controller */
#define SIGROW             (*(SIGROW_t *) 0x1100) /* Signature row */
#define FUSE                 (*(FUSE_t *) 0x1280) /* Fuses */
#define LOCK_BIT          (*(LOCKBIT_t *) 0x128A) /* Lockbit */
#define USERROW           (*(USERROW_t *) 0x1300) /* User Row */

#endif /* !defined (__ASSEMBLER__) */


/* ========== Flattened fully qualified IO register names ========== */


/* VPORT (VPORTA) - Virtual Ports */
#define VPORTA_DIR  _SFR_MEM8(0x0000)
#define VPORTA_OUT  _SFR_MEM8(0x0001)
#define VPORTA_IN  _SFR_MEM8(0x0002)
#define VPORTA_INTFLAGS  _SFR_MEM8(0x0003)


/* VPORT (VPORTB) - Virtual Ports */
#define VPORTB_DIR  _SFR_MEM8(0x0004)
#define VPORTB_OUT  _SFR_MEM8(0x0005)
#define VPORTB_IN  _SFR_MEM8(0x0006)
#define VPORTB_INTFLAGS  _SFR_MEM8(0x0007)


/* VPORT (VPORTC) - Virtual Ports */
#define VPORTC_DIR  _SFR_MEM8(0x0008)
#define VPORTC_OUT  _SFR_MEM8(0x0009)
#define VPORTC_IN  _SFR_MEM8(0x000A)
#define VPORTC_INTFLAGS  _SFR_MEM8(0x000B)


/* GPIO - General Purpose IO */
#define GPIO_GPIOR0  _SFR_MEM8(0x001C)
#define GPIO_GPIOR1  _SFR_MEM8(0x001D)
#define GPIO_GPIOR2  _SFR_MEM8(0x001E)
#define GPIO_GPIOR3  _SFR_MEM8(0x001F)


/* Deprecated */
#define GPIO_GPIOR0  _SFR_MEM8(0x001C)
#define GPIO_GPIOR1  _SFR_MEM8(0x001D)
#define GPIO_GPIOR2  _SFR_MEM8(0x001E)
#define GPIO_GPIOR3  _SFR_MEM8(0x001F)


/* CPU - CPU */
#define CPU_CCP  _SFR_MEM8(0x0034)
#define CPU_SPL  _SFR_MEM8(0x003D)
#define CPU_SPH  _SFR_MEM8(0x003E)
#define CPU_SREG  _SFR_MEM8(0x003F)


/* RSTCTRL - Reset controller */
#define RSTCTRL_RSTFR  _SFR_MEM8(0x0040)
#define RSTCTRL_SWRR  _SFR_MEM8(0x0041)


/* SLPCTRL - Sleep Controller */
#define SLPCTRL_CTRLA  _SFR_MEM8(0x0050)


/* CLKCTRL - Clock controller */
#define CLKCTRL_MCLKCTRLA  _SFR_MEM8(0x0060)
#define CLKCTRL_MCLKCTRLB  _SFR_MEM8(0x0061)
#define CLKCTRL_MCLKLOCK  _SFR_MEM8(0x0062)
#define CLKCTRL_MCLKSTATUS  _SFR_MEM8(0x0063)
#define CLKCTRL_OSC20MCTRLA  _SFR_MEM8(0x0070)
#define CLKCTRL_OSC20MCALIBA  _SFR_MEM8(0x0071)
#define CLKCTRL_OSC20MCALIBB  _SFR_MEM8(0x0072)
#define CLKCTRL_OSC32KCTRLA  _SFR_MEM8(0x0078)


/* BOD - Bod interface */
#define BOD_CTRLA  _SFR_MEM8(0x0080)
#define BOD_CTRLB  _SFR_MEM8(0x0081)
#define BOD_VLMCTRLA  _SFR_MEM8(0x0088)
#define BOD_INTCTRL  _SFR_MEM8(0x0089)
#define BOD_INTFLAGS  _SFR_MEM8(0x008A)
#define BOD_STATUS  _SFR_MEM8(0x008B)


/* VREF - Voltage reference */
#define VREF_CTRLA  _SFR_MEM8(0x00A0)
#define VREF_CTRLB  _SFR_MEM8(0x00A1)


/* WDT - Watch-Dog Timer */
#define WDT_CTRLA  _SFR_MEM8(0x0100)
#define WDT_STATUS  _SFR_MEM8(0x0101)


/* CPUINT - Interrupt Controller */
#define CPUINT_CTRLA  _SFR_MEM8(0x0110)
#define CPUINT_STATUS  _SFR_MEM8(0x0111)
#define CPUINT_LVL0PRI  _SFR_MEM8(0x0112)
#define CPUINT_LVL1VEC  _SFR_MEM8(0x0113)


/* CRCSCAN - CRCSCAN */
#define CRCSCAN_CTRLA  _SFR_MEM8(0x0120)
#define CRCSCAN_CTRLB  _SFR_MEM8(0x0121)
#define CRCSCAN_STATUS  _SFR_MEM8(0x0122)


/* RTC - Real-Time Counter */
#define RTC_CTRLA  _SFR_MEM8(0x0140)
#define RTC_STATUS  _SFR_MEM8(0x0141)
#define RTC_INTCTRL  _SFR_MEM8(0x0142)
#define RTC_INTFLAGS  _SFR_MEM8(0x0143)
#define RTC_TEMP  _SFR_MEM8(0x0144)
#define RTC_DBGCTRL  _SFR_MEM8(0x0145)
#define RTC_CLKSEL  _SFR_MEM8(0x0147)
#define RTC_CNT  _SFR_MEM16(0x0148)
#define RTC_CNTL  _SFR_MEM8(0x0148)
#define RTC_CNTH  _SFR_MEM8(0x0149)
#define RTC_PER  _SFR_MEM16(0x014A)
#define RTC_PERL  _SFR_MEM8(0x014A)
#define RTC_PERH  _SFR_MEM8(0x014B)
#define RTC_CMP  _SFR_MEM16(0x014C)
#define RTC_CMPL  _SFR_MEM8(0x014C)
#define RTC_CMPH  _SFR_MEM8(0x014D)
#define RTC_PITCTRLA  _SFR_MEM8(0x0150)
#define RTC_PITSTATUS  _SFR_MEM8(0x0151)
#define RTC_PITINTCTRL  _SFR_MEM8(0x0152)
#define RTC_PITINTFLAGS  _SFR_MEM8(0x0153)
#define RTC_PITDBGCTRL  _SFR_MEM8(0x0155)


/* EVSYS - Event System */
#define EVSYS_ASYNCSTROBE  _SFR_MEM8(0x0180)
#define EVSYS_SYNCSTROBE  _SFR_MEM8(0x0181)
#define EVSYS_ASYNCCH0  _SFR_MEM8(0x0182)
#define EVSYS_ASYNCCH1  _SFR_MEM8(0x0183)
#define EVSYS_SYNCCH0  _SFR_MEM8(0x018A)
#define EVSYS_ASYNCUSER0  _SFR_MEM8(0x0192)
#define EVSYS_ASYNCUSER1  _SFR_MEM8(0x0193)
#define EVSYS_ASYNCUSER2  _SFR_MEM8(0x0194)
#define EVSYS_ASYNCUSER3  _SFR_MEM8(0x0195)
#define EVSYS_ASYNCUSER4  _SFR_MEM8(0x0196)
#define EVSYS_ASYNCUSER5  _SFR_MEM8(0x0197)
#define EVSYS_ASYNCUSER6  _SFR_MEM8(0x0198)
#define EVSYS_ASYNCUSER7  _SFR_MEM8(0x0199)
#define EVSYS_ASYNCUSER8  _SFR_MEM8(0x019A)
#define EVSYS_ASYNCUSER9  _SFR_MEM8(0x019B)
#define EVSYS_ASYNCUSER10  _SFR_MEM8(0x019C)
#define EVSYS_ASYNCUSER11  _SFR_MEM8(0x019D)
#define EVSYS_ASYNCUSER12  _SFR_MEM8(0x019E)
#define EVSYS_SYNCUSER0  _SFR_MEM8(0x01A2)
#define EVSYS_SYNCUSER1  _SFR_MEM8(0x01A3)


/* CCL - Configurable Custom Logic */
#define CCL_CTRLA  _SFR_MEM8(0x01C0)
#define CCL_SEQCTRL0  _SFR_MEM8(0x01C1)
#define CCL_LUT0CTRLA  _SFR_MEM8(0x01C5)
#define CCL_LUT0CTRLB  _SFR_MEM8(0x01C6)
#define CCL_LUT0CTRLC  _SFR_MEM8(0x01C7)
#define CCL_TRUTH0  _SFR_MEM8(0x01C8)
#define CCL_LUT1CTRLA  _SFR_MEM8(0x01C9)
#define CCL_LUT1CTRLB  _SFR_MEM8(0x01CA)
#define CCL_LUT1CTRLC  _SFR_MEM8(0x01CB)
#define CCL_TRUTH1  _SFR_MEM8(0x01CC)


/* PORTMUX - Port Multiplexer */
#define PORTMUX_CTRLA  _SFR_MEM8(0x0200)
#define PORTMUX_CTRLB  _SFR_MEM8(0x0201)
#define PORTMUX_CTRLC  _SFR_MEM8(0x0202)
#define PORTMUX_CTRLD  _SFR_MEM8(0x0203)


/* PORT (PORTA) - I/O Ports */
#define PORTA_DIR  _SFR_MEM8(0x0400)
#define PORTA_DIRSET  _SFR_MEM8(0x0401)
#define PORTA_DIRCLR  _SFR_MEM8(0x0402)
#define PORTA_DIRTGL  _SFR_MEM8(0x0403)
#define PORTA_OUT  _SFR_MEM8(0x0404)
#define PORTA_OUTSET  _SFR_MEM8(0x0405)
#define PORTA_OUTCLR  _SFR_MEM8(0x0406)
#define PORTA_OUTTGL  _SFR_MEM8(0x0407)
#define PORTA_IN  _SFR_MEM8(0x0408)
#define PORTA_INTFLAGS  _SFR_MEM8(0x0409)
#define PORTA_PIN0CTRL  _SFR_MEM8(0x0410)
#define PORTA_PIN1CTRL  _SFR_MEM8(0x0411)
#define PORTA_PIN2CTRL  _SFR_MEM8(0x0412)
#define PORTA_PIN3CTRL  _SFR_MEM8(0x0413)
#define PORTA_PIN4CTRL  _SFR_MEM8(0x0414)
#define PORTA_PIN5CTRL  _SFR_MEM8(0x0415)
#define PORTA_PIN6CTRL  _SFR_MEM8(0x0416)
#define PORTA_PIN7CTRL  _SFR_MEM8(0x0417)


/* PORT (PORTB) - I/O Ports */
#define PORTB_DIR  _SFR_MEM8(0x0420)
#define PORTB_DIRSET  _SFR_MEM8(0x0421)
#define PORTB_DIRCLR  _SFR_MEM8(0x0422)
#define PORTB_DIRTGL  _SFR_MEM8(0x0423)
#define PORTB_OUT  _SFR_MEM8(0x0424)
#define PORTB_OUTSET  _SFR_MEM8(0x0425)
#define PORTB_OUTCLR  _SFR_MEM8(0x0426)
#define PORTB_OUTTGL  _SFR_MEM8(0x0427)
#define PORTB_IN  _SFR_MEM8(0x0428)
#define PORTB_INTFLAGS  _SFR_MEM8(0x0429)
#define PORTB_PIN0CTRL  _SFR_MEM8(0x0430)
#define PORTB_PIN1CTRL  _SFR_MEM8(0x0431)
#define PORTB_PIN2CTRL  _SFR_MEM8(0x0432)
#define PORTB_PIN3CTRL  _SFR_MEM8(0x0433)
#define PORTB_PIN4CTRL  _SFR_MEM8(0x0434)
#define PORTB_PIN5CTRL  _SFR_MEM8(0x0435)
#define PORTB_PIN6CTRL  _SFR_MEM8(0x0436)
#define PORTB_PIN7CTRL  _SFR_MEM8(0x0437)


/* PORT (PORTC) - I/O Ports */
#define PORTC_DIR  _SFR_MEM8(0x0440)
#define PORTC_DIRSET  _SFR_MEM8(0x0441)
#define PORTC_DIRCLR  _SFR_MEM8(0x0442)
#define PORTC_DIRTGL  _SFR_MEM8(0x0443)
#define PORTC_OUT  _SFR_MEM8(0x0444)
#define PORTC_OUTSET  _SFR_MEM8(0x0445)
#define PORTC_OUTCLR  _SFR_MEM8(0x0446)
#define PORTC_OUTTGL  _SFR_MEM8(0x0447)
#define PORTC_IN  _SFR_MEM8(0x0448)
#define PORTC_INTFLAGS  _SFR_MEM8(0x0449)
#define PORTC_PIN0CTRL  _SFR_MEM8(0x0450)
#define PORTC_PIN1CTRL  _SFR_MEM8(0x0451)
#define PORTC_PIN2CTRL  _SFR_MEM8(0x0452)
#define PORTC_PIN3CTRL  _SFR_MEM8(0x0453)
#define PORTC_PIN4CTRL  _SFR_MEM8(0x0454)
#define PORTC_PIN5CTRL  _SFR_MEM8(0x0455)
#define PORTC_PIN6CTRL  _SFR_MEM8(0x0456)
#define PORTC_PIN7CTRL  _SFR_MEM8(0x0457)


/* ADC (ADC0) - Analog to Digital Converter */
#define ADC0_CTRLA  _SFR_MEM8(0x0600)
#define ADC0_CTRLB  _SFR_MEM8(0x0601)
#define ADC0_CTRLC  _SFR_MEM8(0x0602)
#define ADC0_CTRLD  _SFR_MEM8(0x0603)
#define ADC0_CTRLE  _SFR_MEM8(0x0604)
#define ADC0_SAMPCTRL  _SFR_MEM8(0x0605)
#define ADC0_MUXPOS  _SFR_MEM8(0x0606)
#define ADC0_COMMAND  _SFR_MEM8(0x0608)
#define ADC0_EVCTRL  _SFR_MEM8(0x0609)
#define ADC0_INTCTRL  _SFR_MEM8(0x060A)
#define ADC0_INTFLAGS  _SFR_MEM8(0x060B)
#define ADC0_DBGCTRL  _SFR_MEM8(0x060C)
#define ADC0_TEMP  _SFR_MEM8(0x060D)
#define ADC0_RES  _SFR_MEM16(0x0610)
#define ADC0_RESL  _SFR_MEM8(0x0610)
#define ADC0_RESH  _SFR_MEM8(0x0611)
#define ADC0_WINLT  _SFR_MEM16(0x0612)
#define ADC0_WINLTL  _SFR_MEM8(0x0612)
#define ADC0_WINLTH  _SFR_MEM8(0x0613)
#define ADC0_WINHT  _SFR_MEM16(0x0614)
#define ADC0_WINHTL  _SFR_MEM8(0x0614)
#define ADC0_WINHTH  _SFR_MEM8(0x0615)
#define ADC0_CALIB  _SFR_MEM8(0x0616)


/* AC (AC0) - Analog Comparator */
#define AC0_CTRLA  _SFR_MEM8(0x0680)
#define AC0_MUXCTRLA  _SFR_MEM8(0x0682)
#define AC0_INTCTRL  _SFR_MEM8(0x0686)
#define AC0_STATUS  _SFR_MEM8(0x0687)


/* USART (USART0) - Universal Synchronous and Asynchronous Receiver and Transmitter */
#define USART0_RXDATAL  _SFR_MEM8(0x0800)
#define USART0_RXDATAH  _SFR_MEM8(0x0801)
#define USART0_TXDATAL  _SFR_MEM8(0x0802)
#define USART0_TXDATAH  _SFR_MEM8(0x0803)
#define USART0_STATUS  _SFR_MEM8(0x0804)
#define USART0_CTRLA  _SFR_MEM8(0x0805)
#define USART0_CTRLB  _SFR_MEM8(0x0806)
#define USART0_CTRLC  _SFR_MEM8(0x0807)
#define USART0_BAUD  _SFR_MEM16(0x0808)
#define USART0_BAUDL  _SFR_MEM8(0x0808)
#define USART0_BAUDH  _SFR_MEM8(0x0809)
#define USART0_DBGCTRL  _SFR_MEM8(0x080B)
#define USART0_EVCTRL  _SFR_MEM8(0x080C)
#define USART0_TXPLCTRL  _SFR_MEM8(0x080D)
#define USART0_RXPLCTRL  _SFR_MEM8(0x080E)


/* TWI (TWI0) - Two-Wire Interface */
#define TWI0_CTRLA  _SFR_MEM8(0x0810)
#define TWI0_DBGCTRL  _SFR_MEM8(0x0812)
#define TWI0_MCTRLA  _SFR_MEM8(0x0813)
#define TWI0_MCTRLB  _SFR_MEM8(0x0814)
#define TWI0_MSTATUS  _SFR_MEM8(0x0815)
#define TWI0_MBAUD  _SFR_MEM8(0x0816)
#define TWI0_MADDR  _SFR_MEM8(0x0817)
#define TWI0_MDATA  _SFR_MEM8(0x0818)
#define TWI0_SCTRLA  _SFR_MEM8(0x0819)
#define TWI0_SCTRLB  _SFR_MEM8(0x081A)
#define TWI0_SSTATUS  _SFR_MEM8(0x081B)
#define TWI0_SADDR  _SFR_MEM8(0x081C)
#define TWI0_SDATA  _SFR_MEM8(0x081D)
#define TWI0_SADDRMASK  _SFR_MEM8(0x081E)


/* SPI (SPI0) - Serial Peripheral Interface */
#define SPI0_CTRLA  _SFR_MEM8(0x0820)
#define SPI0_CTRLB  _SFR_MEM8(0x0821)
#define SPI0_INTCTRL  _SFR_MEM8(0x0822)
#define SPI0_INTFLAGS  _SFR_MEM8(0x0823)
#define SPI0_DATA  _SFR_MEM8(0x0824)


/* TCA (TCA0) - 16-bit Timer/Counter Type A - Single Mode */
#define TCA0_SINGLE_CTRLA  _SFR_MEM8(0x0A00)
#define TCA0_SINGLE_CTRLB  _SFR_MEM8(0x0A01)
#define TCA0_SINGLE_CTRLC  _SFR_MEM8(0x0A02)
#define TCA0_SINGLE_CTRLD  _SFR_MEM8(0x0A03)
#define TCA0_SINGLE_CTRLECLR  _SFR_MEM8(0x0A04)
#define TCA0_SINGLE_CTRLESET  _SFR_MEM8(0x0A05)
#define TCA0_SINGLE_CTRLFCLR  _SFR_MEM8(0x0A06)
#define TCA0_SINGLE_CTRLFSET  _SFR_MEM8(0x0A07)
#define TCA0_SINGLE_EVCTRL  _SFR_MEM8(0x0A09)
#define TCA0_SINGLE_INTCTRL  _SFR_MEM8(0x0A0A)
#define TCA0_SINGLE_INTFLAGS  _SFR_MEM8(0x0A0B)
#define TCA0_SINGLE_DBGCTRL  _SFR_MEM8(0x0A0E)
#define TCA0_SINGLE_TEMP  _SFR_MEM8(0x0A0F)
#define TCA0_SINGLE_CNT  _SFR_MEM16(0x0A20)
#define TCA0_SINGLE_CNTL  _SFR_MEM8(0x0A20)
#define TCA0_SINGLE_CNTH  _SFR_MEM8(0x0A21)
#define TCA0_SINGLE_PER  _SFR_MEM16(0x0A26)
#define TCA0_SINGLE_PERL  _SFR_MEM8(0x0A26)
#define TCA0_SINGLE_PERH  _SFR_MEM8(0x0A27)
#define TCA0_SINGLE_CMP0  _SFR_MEM16(0x0A28)
#define TCA0_SINGLE_CMP0L  _SFR_MEM8(0x0A28)
#define TCA0_SINGLE_CMP0H  _SFR_MEM8(0x0A29)
#define TCA0_SINGLE_CMP1  _SFR_MEM16(0x0A2A)
#define TCA0_SINGLE_CMP1L  _SFR_MEM8(0x0A2A)
#define TCA0_SINGLE_CMP1H  _SFR_MEM8(0x0A2B)
#define TCA0_SINGLE_CMP2  _SFR_MEM16(0x0A2C)
#define TCA0_SINGLE_CMP2L  _SFR_MEM8(0x0A2C)
#define TCA0_SINGLE_CMP2H  _SFR_MEM8(0x0A2D)
#define TCA0_SINGLE_PERBUF  _SFR_MEM16(0x0A36)
#define TCA0_SINGLE_PERBUFL  _SFR_MEM8(0x0A36)
#define TCA0_SINGLE_PERBUFH  _SFR_MEM8(0x0A37)
#define TCA0_SINGLE_CMP0BUF  _SFR_MEM16(0x0A38)
#define TCA0_SINGLE_CMP0BUFL  _SFR_MEM8(0x0A38)
#define TCA0_SINGLE_CMP0BUFH  _SFR_MEM8(0x0A39)
#define TCA0_SINGLE_CMP1BUF  _SFR_MEM16(0x0A3A)
#define TCA0_SINGLE_CMP1BUFL  _SFR_MEM8(0x0A3A)
#define TCA0_SINGLE_CMP1BUFH  _SFR_MEM8(0x0A3B)
#define TCA0_SINGLE_CMP2BUF  _SFR_MEM16(0x0A3C)
#define TCA0_SINGLE_CMP2BUFL  _SFR_MEM8(0x0A3C)
#define TCA0_SINGLE_CMP2BUFH  _SFR_MEM8(0x0A3D)


/* TCA (TCA0) - 16-bit Timer/Counter Type A - Split Mode */
#define TCA0_SPLIT_CTRLA  _SFR_MEM8(0x0A00)
#define TCA0_SPLIT_CTRLB  _SFR_MEM8(0x0A01)
#define TCA0_SPLIT_CTRLC  _SFR_MEM8(0x0A02)
#define TCA0_SPLIT_CTRLD  _SFR_MEM8(0x0A03)
#define TCA0_SPLIT_CTRLECLR  _SFR_MEM8(0x0A04)
#define TCA0_SPLIT_CTRLESET  _SFR_MEM8(0x0A05)
#define TCA0_SPLIT_INTCTRL  _SFR_MEM8(0x0A0A)
#define TCA0_SPLIT_INTFLAGS  _SFR_MEM8(0x0A0B)
#define TCA0_SPLIT_DBGCTRL  _SFR_MEM8(0x0A0E)
#define TCA0_SPLIT_LCNT  _SFR_MEM8(0x0A20)
#define TCA0_SPLIT_HCNT  _SFR_MEM8(0x0A21)
#define TCA0_SPLIT_LPER  _SFR_MEM8(0x0A26)
#define TCA0_SPLIT_HPER  _SFR_MEM8(0x0A27)
#define TCA0_SPLIT_LCMP0  _SFR_MEM8(0x0A28)
#define TCA0_SPLIT_HCMP0  _SFR_MEM8(0x0A29)
#define TCA0_SPLIT_LCMP1  _SFR_MEM8(0x0A2A)
#define TCA0_SPLIT_HCMP1  _SFR_MEM8(0x0A2B)
#define TCA0_SPLIT_LCMP2  _SFR_MEM8(0x0A2C)
#define TCA0_SPLIT_HCMP2  _SFR_MEM8(0x0A2D)


/* TCB (TCB0) - 16-bit Timer Type B */
#define TCB0_CTRLA  _SFR_MEM8(0x0A40)
#define TCB0_CTRLB  _SFR_MEM8(0x0A41)
#define TCB0_EVCTRL  _SFR_MEM8(0x0A44)
#define TCB0_INTCTRL  _SFR_MEM8(0x0A45)
#define TCB0_INTFLAGS  _SFR_MEM8(0x0A46)
#define TCB0_STATUS  _SFR_MEM8(0x0A47)
#define TCB0_DBGCTRL  _SFR_MEM8(0x0A48)
#define TCB0_TEMP  _SFR_MEM8(0x0A49)
#define TCB0_CNT  _SFR_MEM16(0x0A4A)
#define TCB0_CNTL  _SFR_MEM8(0x0A4A)
#define TCB0_CNTH  _SFR_MEM8(0x0A4B)
#define TCB0_CCMP  _SFR_MEM16(0x0A4C)
#define TCB0_CCMPL  _SFR_MEM8(0x0A4C)
#define TCB0_CCMPH  _SFR_MEM8(0x0A4D)


/* SYSCFG - System Configuration Registers */
#define SYSCFG_REVID  _SFR_MEM8(0x0F01)
#define SYSCFG_EXTBRK  _SFR_MEM8(0x0F02)


/* NVMCTRL - Non-volatile Memory Controller */
#define NVMCTRL_CTRLA  _SFR_MEM8(0x1000)
#define NVMCTRL_CTRLB  _SFR_MEM8(0x1001)
#define NVMCTRL_STATUS  _SFR_MEM8(0x1002)
#define NVMCTRL_INTCTRL  _SFR_MEM8(0x1003)
#define NVMCTRL_INTFLAGS  _SFR_MEM8(0x1004)
#define NVMCTRL_DATA  _SFR_MEM16(0x1006)
#define NVMCTRL_DATAL  _SFR_MEM8(0x1006)
#define NVMCTRL_DATAH  _SFR_MEM8(0x1007)
#define NVMCTRL_ADDR  _SFR_MEM16(0x1008)
#define NVMCTRL_ADDRL  _SFR_MEM8(0x1008)
#define NVMCTRL_ADDRH  _SFR_MEM8(0x1009)


/* SIGROW - Signature row */
#define SIGROW_DEVICEID0  _SFR_MEM8(0x1100)
#define SIGROW_DEVICEID1  _SFR_MEM8(0x1101)
#define SIGROW_DEVICEID2  _SFR_MEM8(0x1102)
#define SIGROW_SERNUM0  _SFR_MEM8(0x1103)
#define SIGROW_SERNUM1  _SFR_MEM8(0x1104)
#define SIGROW_SERNUM2  _SFR_MEM8(0x1105)
#define SIGROW_SERNUM3  _SFR_MEM8(0x1106)
#define SIGROW_SERNUM4  _SFR_MEM8(0x1107)
#define SIGROW_SERNUM5  _SFR_MEM8(0x1108)
#define SIGROW_SERNUM6  _SFR_MEM8(0x1109)
#define SIGROW_SERNUM7  _SFR_MEM8(0x110A)
#define SIGROW_SERNUM8  _SFR_MEM8(0x110B)
#define SIGROW_SERNUM9  _SFR_MEM8(0x110C)
#define SIGROW_TEMPSENSE0  _SFR_MEM8(0x1120)
#define SIGROW_TEMPSENSE1  _SFR_MEM8(0x1121)
#define SIGROW_OSC16ERR3V  _SFR_MEM8(0x1122)
#define SIGROW_OSC16ERR5V  _SFR_MEM8(0x1123)
#define SIGROW_OSC20ERR3V  _SFR_MEM8(0x1124)
#define SIGROW_OSC20ERR5V  _SFR_MEM8(0x1125)


/* FUSE - Fuses */
#define FUSE_WDTCFG  _SFR_MEM8(0x1280)
#define FUSE_BODCFG  _SFR_MEM8(0x1281)
#define FUSE_OSCCFG  _SFR_MEM8(0x1282)
#define FUSE_SYSCFG0  _SFR_MEM8(0x1285)
#define FUSE_SYSCFG1  _SFR_MEM8(0x1286)
#define FUSE_APPEND  _SFR_MEM8(0x1287)
#define FUSE_BOOTEND  _SFR_MEM8(0x1288)


/* LOCKBIT - Lockbit */
#define LOCKBIT_LOCKBIT  _SFR_MEM8(0x128A)


/* USERROW - User Row */
#define USERROW_USERROW0  _SFR_MEM8(0x1300)
#define USERROW_USERROW1  _SFR_MEM8(0x1301)
#define USERROW_USERROW2  _SFR_MEM8(0x1302)
#define USERROW_USERROW3  _SFR_MEM8(0x1303)
#define USERROW_USERROW4  _SFR_MEM8(0x1304)
#define USERROW_USERROW5  _SFR_MEM8(0x1305)
#define USERROW_USERROW6  _SFR_MEM8(0x1306)
#define USERROW_USERROW7  _SFR_MEM8(0x1307)
#define USERROW_USERROW8  _SFR_MEM8(0x1308)
#define USERROW_USERROW9  _SFR_MEM8(0x1309)
#define USERROW_USERROW10  _SFR_MEM8(0x130A)
#define USERROW_USERROW11  _SFR_MEM8(0x130B)
#define USERROW_USERROW12  _SFR_MEM8(0x130C)
#define USERROW_USERROW13  _SFR_MEM8(0x130D)
#define USERROW_USERROW14  _SFR_MEM8(0x130E)
#define USERROW_USERROW15  _SFR_MEM8(0x130F)
#define USERROW_USERROW16  _SFR_MEM8(0x1310)
#define USERROW_USERROW17  _SFR_MEM8(0x1311)
#define USERROW_USERROW18  _SFR_MEM8(0x1312)
#define USERROW_USERROW19  _SFR_MEM8(0x1313)
#define USERROW_USERROW20  _SFR_MEM8(0x1314)
#define USERROW_USERROW21  _SFR_MEM8(0x1315)
#define USERROW_USERROW22  _SFR_MEM8(0x1316)
#define USERROW_USERROW23  _SFR_MEM8(0x1317)
#define USERROW_USERROW24  _SFR_MEM8(0x1318)
#define USERROW_USERROW25  _SFR_MEM8(0x1319)
#define USERROW_USERROW26  _SFR_MEM8(0x131A)
#define USERROW_USERROW27  _SFR_MEM8(0x131B)
#define USERROW_USERROW28  _SFR_MEM8(0x131C)
#define USERROW_USERROW29  _SFR_MEM8(0x131D)
#define USERROW_USERROW30  _SFR_MEM8(0x131E)
#define USERROW_USERROW31  _SFR_MEM8(0x131F)



/*================== Bitfield Definitions ================== */

/* AC - Analog Comparator */
/* AC.CTRLA  bit masks and bit positions */
#define AC_ENABLE_bm  0x01  /* Enable bit mask. */
#define AC_ENABLE_bp  0  /* Enable bit position. */
#define AC_HYSMODE_gm  0x06  /* Hysteresis Mode group mask. */
#define AC_HYSMODE_gp  1  /* Hysteresis Mode group position. */
#define AC_HYSMODE_0_bm  (1<<1)  /* Hysteresis Mode bit 0 mask. */
#define AC_HYSMODE_0_bp  1  /* Hysteresis Mode bit 0 position. */
#define AC_HYSMODE_1_bm  (1<<2)  /* Hysteresis Mode bit 1 mask. */
#define AC_HYSMODE_1_bp  2  /* Hysteresis Mode bit 1 position. */
#define AC_INTMODE_gm  0x30  /* Interrupt Mode group mask. */
#define AC_INTMODE_gp  4  /* Interrupt Mode group position. */
#define AC_INTMODE_0_bm  (1<<4)  /* Interrupt Mode bit 0 mask. */
#define AC_INTMODE_0_bp  4  /* Interrupt Mode bit 0 position. */
#define AC_INTMODE_1_bm  (1<<5)  /* Interrupt Mode bit 1 mask. */
#define AC_INTMODE_1_bp  5  /* Interrupt Mode bit 1 position. */
#define AC_OUTEN_bm  0x40  /* Output Buffer Enable bit mask. */
#define AC_OUTEN_bp  6  /* Output Buffer Enable bit position. */
#define AC_RUNSTDBY_bm  0x80  /* Run in Standby Mode bit mask. */
#define AC_RUNSTDBY_bp  7  /* Run in Standby Mode bit position. */

/* AC.MUXCTRLA  bit masks and bit positions */
#define AC_MUXNEG_gm  0x03  /* Negative Input MUX Selection group mask. */
#define AC_MUXNEG_gp  0  /* Negative Input MUX Selection group position. */
#define AC_MUXNEG_0_bm  (1<<0)  /* Negative Input MUX Selection bit 0 mask. */
#define AC_MUXNEG_0_bp  0  /* Negative Input MUX Selection bit 0 position. */
#define AC_MUXNEG_1_bm  (1<<1)  /* Negative Input MUX Selection bit 1 mask. */
#define AC_MUXNEG_1_bp  1  /* Negative Input MUX Selection bit 1 position. */
#define AC_MUXPOS_gm  0x18  /* Positive Input MUX Selection group mask. */
#define AC_MUXPOS_gp  3  /* Positive Input MUX Selection group position. */
#define AC_MUXPOS_0_bm  (1<<3)  /* Positive Input MUX Selection bit 0 mask. */
#define AC_MUXPOS_0_bp  3  /* Positive Input MUX Selection bit 0 position. */
#define AC_MUXPOS_1_bm  (1<<4)  /* Positive Input MUX Selection bit 1 mask. */
#define AC_MUXPOS_1_bp  4  /* Positive Input MUX Selection bit 1 position. */
#define AC_INVERT_bm  0x80  /* Invert AC Output bit mask. */
#define AC_INVERT_bp  7  /* Invert AC Output bit position. */

/* AC.INTCTRL  bit masks and bit positions */
#define AC_CMP_bm  0x01  /* Analog Comparator 0 Interrupt Enable bit mask. */
#define AC_CMP_bp  0  /* Analog Comparator 0 Interrupt Enable bit position. */

/* AC.STATUS  bit masks and bit positions */
/* AC_CMP  is already defined. */
#define AC_STATE_bm  0x10  /* Analog Comparator State bit mask. */
#define AC_STATE_bp  4  /* Analog Comparator State bit position. */


/* ADC - Analog to Digital Converter */
/* ADC.CTRLA  bit masks and bit positions */
#define ADC_ENABLE_bm  0x01  /* ADC Enable bit mask. */
#define ADC_ENABLE_bp  0  /* ADC Enable bit position. */
#define ADC_FREERUN_bm  0x02  /* ADC Freerun mode bit mask. */
#define ADC_FREERUN_bp  1  /* ADC Freerun mode bit position. */
#define ADC_RESSEL_bm  0x04  /* ADC Resolution bit mask. */
#define ADC_RESSEL_bp  2  /* ADC Resolution bit position. */
#define ADC_RUNSTBY_bm  0x80  /* Run standby mode bit mask. */
#define ADC_RUNSTBY_bp  7  /* Run standby mode bit position. */

/* ADC.CTRLB  bit masks and bit positions */
#define ADC_SAMPNUM_gm  0x07  /* Accumulation Samples group mask. */
#define ADC_SAMPNUM_gp  0  /* Accumulation Samples group position. */
#define ADC_SAMPNUM_0_bm  (1<<0)  /* Accumulation Samples bit 0 mask. */
#define ADC_SAMPNUM_0_bp  0  /* Accumulation Samples bit 0 position. */
#define ADC_SAMPNUM_1_bm  (1<<1)  /* Accumulation Samples bit 1 mask. */
#define ADC_SAMPNUM_1_bp  1  /* Accumulation Samples bit 1 position. */
#define ADC_SAMPNUM_2_bm  (1<<2)  /* Accumulation Samples bit 2 mask. */
#define ADC_SAMPNUM_2_bp  2  /* Accumulation Samples bit 2 position. */

/* ADC.CTRLC  bit masks and bit positions */
#define ADC_PRESC_gm  0x07  /* Clock Pre-scaler group mask. */
#define ADC_PRESC_gp  0  /* Clock Pre-scaler group position. */
#define ADC_PRESC_0_bm  (1<<0)  /* Clock Pre-scaler bit 0 mask. */
#define ADC_PRESC_0_bp  0  /* Clock Pre-scaler bit 0 position. */
#define ADC_PRESC_1_bm  (1<<1)  /* Clock Pre-scaler bit 1 mask. */
#define ADC_PRESC_1_bp  1  /* Clock Pre-scaler bit 1 position. */
#define ADC_PRESC_2_bm  (1<<2)  /* Clock Pre-scaler bit 2 mask. */
#define ADC_PRESC_2_bp  2  /* Clock Pre-scaler bit 2 position. */
#define ADC_REFSEL_gm  0x30  /* Reference Selection group mask. */
#define ADC_REFSEL_gp  4  /* Reference Selection group position. */
#define ADC_REFSEL_0_bm  (1<<4)  /* Reference Selection bit 0 mask. */
#define ADC_REFSEL_0_bp  4  /* Reference Selection bit 0 position. */
#define ADC_REFSEL_1_bm  (1<<5)  /* Reference Selection bit 1 mask. */
#define ADC_REFSEL_1_bp  5  /* Reference Selection bit 1 position. */
#define ADC_SAMPCAP_bm  0x40  /* Sample Capacitance Selection bit mask. */
#define ADC_SAMPCAP_bp  6  /* Sample Capacitance Selection bit position. */

/* ADC.CTRLD  bit masks and bit positions */
#define ADC_SAMPDLY_gm  0x0F  /* Sampling Delay Selection group mask. */
#define ADC_SAMPDLY_gp  0  /* Sampling Delay Selection group position. */
#define ADC_SAMPDLY_0_bm  (1<<0)  /* Sampling Delay Selection bit 0 mask. */
#define ADC_SAMPDLY_0_bp  0  /* Sampling Delay Selection bit 0 position. */
#define ADC_SAMPDLY_1_bm  (1<<1)  /* Sampling Delay Selection bit 1 mask. */
#define ADC_SAMPDLY_1_bp  1  /* Sampling Delay Selection bit 1 position. */
#define ADC_SAMPDLY_2_bm  (1<<2)  /* Sampling Delay Selection bit 2 mask. */
#define ADC_SAMPDLY_2_bp  2  /* Sampling Delay Selection bit 2 position. */
#define ADC_SAMPDLY_3_bm  (1<<3)  /* Sampling Delay Selection bit 3 mask. */
#define ADC_SAMPDLY_3_bp  3  /* Sampling Delay Selection bit 3 position. */
#define ADC_ASDV_bm  0x10  /* Automatic Sampling Delay Variation bit mask. */
#define ADC_ASDV_bp  4  /* Automatic Sampling Delay Variation bit position. */
#define ADC_INITDLY_gm  0xE0  /* Initial Delay Selection group mask. */
#define ADC_INITDLY_gp  5  /* Initial Delay Selection group position. */
#define ADC_INITDLY_0_bm  (1<<5)  /* Initial Delay Selection bit 0 mask. */
#define ADC_INITDLY_0_bp  5  /* Initial Delay Selection bit 0 position. */
#define ADC_INITDLY_1_bm  (1<<6)  /* Initial Delay Selection bit 1 mask. */
#define ADC_INITDLY_1_bp  6  /* Initial Delay Selection bit 1 position. */
#define ADC_INITDLY_2_bm  (1<<7)  /* Initial Delay Selection bit 2 mask. */
#define ADC_INITDLY_2_bp  7  /* Initial Delay Selection bit 2 position. */

/* ADC.CTRLE  bit masks and bit positions */
#define ADC_WINCM_gm  0x07  /* Window Comparator Mode group mask. */
#define ADC_WINCM_gp  0  /* Window Comparator Mode group position. */
#define ADC_WINCM_0_bm  (1<<0)  /* Window Comparator Mode bit 0 mask. */
#define ADC_WINCM_0_bp  0  /* Window Comparator Mode bit 0 position. */
#define ADC_WINCM_1_bm  (1<<1)  /* Window Comparator Mode bit 1 mask. */
#define ADC_WINCM_1_bp  1  /* Window Comparator Mode bit 1 position. */
#define ADC_WINCM_2_bm  (1<<2)  /* Window Comparator Mode bit 2 mask. */
#define ADC_WINCM_2_bp  2  /* Window Comparator Mode bit 2 position. */

/* ADC.SAMPCTRL  bit masks and bit positions */
#define ADC_SAMPLEN_gm  0x1F  /* Sample length group mask. */
#define ADC_SAMPLEN_gp  0  /* Sample length group position. */
#define ADC_SAMPLEN_0_bm  (1<<0)  /* Sample length bit 0 mask. */
#define ADC_SAMPLEN_0_bp  0  /* Sample length bit 0 position. */
#define ADC_SAMPLEN_1_bm  (1<<1)  /* Sample length bit 1 mask. */
#define ADC_SAMPLEN_1_bp  1  /* Sample length bit 1 position. */
#define ADC_SAMPLEN_2_bm  (1<<2)  /* Sample length bit 2 mask. */
#define ADC_SAMPLEN_2_bp  2  /* Sample length bit 2 position. */
#define ADC_SAMPLEN_3_bm  (1<<3)  /* Sample length bit 3 mask. */
#define ADC_SAMPLEN_3_bp  3  /* Sample length bit 3 position. */
#define ADC_SAMPLEN_4_bm  (1<<4)  /* Sample length bit 4 mask. */
#define ADC_SAMPLEN_4_bp  4  /* Sample length bit 4 position. */

/* ADC.MUXPOS  bit masks and bit positions */
#define ADC_MUXPOS_gm  0x1F  /* Analog Channel Selection Bits group mask. */
#define ADC_MUXPOS_gp  0  /* Analog Channel Selection Bits group position. */
#define ADC_MUXPOS_0_bm  (1<<0)  /* Analog Channel Selection Bits bit 0 mask. */
#define ADC_MUXPOS_0_bp  0  /* Analog Channel Selection Bits bit 0 position. */
#define ADC_MUXPOS_1_bm  (1<<1)  /* Analog Channel Selection Bits bit 1 mask. */
#define ADC_MUXPOS_1_bp  1  /* Analog Channel Selection Bits bit 1 position. */
#define ADC_MUXPOS_2_bm  (1<<2)  /* Analog Channel Selection Bits bit 2 mask. */
#define ADC_MUXPOS_2_bp  2  /* Analog Channel Selection Bits bit 2 position. */
#define ADC_MUXPOS_3_bm  (1<<3)  /* Analog Channel Selection Bits bit 3 mask. */
#define ADC_MUXPOS_3_bp  3  /* Analog Channel Selection Bits bit 3 position. */
#define ADC_MUXPOS_4_bm  (1<<4)  /* Analog Channel Selection Bits bit 4 mask. */
#define ADC_MUXPOS_4_bp  4  /* Analog Channel Selection Bits bit 4 position. */

/* ADC.COMMAND  bit masks and bit positions */
#define ADC_STCONV_bm  0x01  /* Start Conversion Operation bit mask. */
#define ADC_STCONV_bp  0  /* Start Conversion Operation bit position. */

/* ADC.EVCTRL  bit masks and bit positions */
#define ADC_STARTEI_bm  0x01  /* Start Event Input Enable bit mask. */
#define ADC_STARTEI_bp  0  /* Start Event Input Enable bit position. */

/* ADC.INTCTRL  bit masks and bit positions */
#define ADC_RESRDY_bm  0x01  /* Result Ready Interrupt Enable bit mask. */
#define ADC_RESRDY_bp  0  /* Result Ready Interrupt Enable bit position. */
#define ADC_WCMP_bm  0x02  /* Window Comparator Interrupt Enable bit mask. */
#define ADC_WCMP_bp  1  /* Window Comparator Interrupt Enable bit position. */

/* ADC.INTFLAGS  bit masks and bit positions */
/* ADC_RESRDY  is already defined. */
/* ADC_WCMP  is already defined. */

/* ADC.DBGCTRL  bit masks and bit positions */
#define ADC_DBGRUN_bm  0x01  /* Debug run bit mask. */
#define ADC_DBGRUN_bp  0  /* Debug run bit position. */

/* ADC.TEMP  bit masks and bit positions */
#define ADC_TEMP_gm  0xFF  /* Temporary group mask. */
#define ADC_TEMP_gp  0  /* Temporary group position. */
#define ADC_TEMP_0_bm  (1<<0)  /* Temporary bit 0 mask. */
#define ADC_TEMP_0_bp  0  /* Temporary bit 0 position. */
#define ADC_TEMP_1_bm  (1<<1)  /* Temporary bit 1 mask. */
#define ADC_TEMP_1_bp  1  /* Temporary bit 1 position. */
#define ADC_TEMP_2_bm  (1<<2)  /* Temporary bit 2 mask. */
#define ADC_TEMP_2_bp  2  /* Temporary bit 2 position. */
#define ADC_TEMP_3_bm  (1<<3)  /* Temporary bit 3 mask. */
#define ADC_TEMP_3_bp  3  /* Temporary bit 3 position. */
#define ADC_TEMP_4_bm  (1<<4)  /* Temporary bit 4 mask. */
#define ADC_TEMP_4_bp  4  /* Temporary bit 4 position. */
#define ADC_TEMP_5_bm  (1<<5)  /* Temporary bit 5 mask. */
#define ADC_TEMP_5_bp  5  /* Temporary bit 5 position. */
#define ADC_TEMP_6_bm  (1<<6)  /* Temporary bit 6 mask. */
#define ADC_TEMP_6_bp  6  /* Temporary bit 6 position. */
#define ADC_TEMP_7_bm  (1<<7)  /* Temporary bit 7 mask. */
#define ADC_TEMP_7_bp  7  /* Temporary bit 7 position. */

/* ADC.CALIB  bit masks and bit positions */
#define ADC_DUTYCYC_bm  0x01  /* Duty Cycle bit mask. */
#define ADC_DUTYCYC_bp  0  /* Duty Cycle bit position. */


/* BOD - Bod interface */
/* BOD.CTRLA  bit masks and bit positions */
#define BOD_SLEEP_gm  0x03  /* Operation in sleep mode group mask. */
#define BOD_SLEEP_gp  0  /* Operation in sleep mode group position. */
#define BOD_SLEEP_0_bm  (1<<0)  /* Operation in sleep mode bit 0 mask. */
#define BOD_SLEEP_0_bp  0  /* Operation in sleep mode bit 0 position. */
#define BOD_SLEEP_1_bm  (1<<1)  /* Operation in sleep mode bit 1 mask. */
#define BOD_SLEEP_1_bp  1  /* Operation in sleep mode bit 1 position. */
#define BOD_ACTIVE_gm  0x0C  /* Operation in active mode group mask. */
#define BOD_ACTIVE_gp  2  /* Operation in active mode group position. */
#define BOD_ACTIVE_0_bm  (1<<2)  /* Operation in active mode bit 0 mask. */
#define BOD_ACTIVE_0_bp  2  /* Operation in active mode bit 0 position. */
#define BOD_ACTIVE_1_bm  (1<<3)  /* Operation in active mode bit 1 mask. */
#define BOD_ACTIVE_1_bp  3  /* Operation in active mode bit 1 position. */
#define BOD_SAMPFREQ_bm  0x10  /* Sample frequency bit mask. */
#define BOD_SAMPFREQ_bp  4  /* Sample frequency bit position. */

/* BOD.CTRLB  bit masks and bit positions */
#define BOD_LVL_gm  0x07  /* Bod level group mask. */
#define BOD_LVL_gp  0  /* Bod level group position. */
#define BOD_LVL_0_bm  (1<<0)  /* Bod level bit 0 mask. */
#define BOD_LVL_0_bp  0  /* Bod level bit 0 position. */
#define BOD_LVL_1_bm  (1<<1)  /* Bod level bit 1 mask. */
#define BOD_LVL_1_bp  1  /* Bod level bit 1 position. */
#define BOD_LVL_2_bm  (1<<2)  /* Bod level bit 2 mask. */
#define BOD_LVL_2_bp  2  /* Bod level bit 2 position. */

/* BOD.VLMCTRLA  bit masks and bit positions */
#define BOD_VLMLVL_gm  0x03  /* voltage level monitor level group mask. */
#define BOD_VLMLVL_gp  0  /* voltage level monitor level group position. */
#define BOD_VLMLVL_0_bm  (1<<0)  /* voltage level monitor level bit 0 mask. */
#define BOD_VLMLVL_0_bp  0  /* voltage level monitor level bit 0 position. */
#define BOD_VLMLVL_1_bm  (1<<1)  /* voltage level monitor level bit 1 mask. */
#define BOD_VLMLVL_1_bp  1  /* voltage level monitor level bit 1 position. */

/* BOD.INTCTRL  bit masks and bit positions */
#define BOD_VLMIE_bm  0x01  /* voltage level monitor interrrupt enable bit mask. */
#define BOD_VLMIE_bp  0  /* voltage level monitor interrrupt enable bit position. */
#define BOD_VLMCFG_gm  0x06  /* Configuration group mask. */
#define BOD_VLMCFG_gp  1  /* Configuration group position. */
#define BOD_VLMCFG_0_bm  (1<<1)  /* Configuration bit 0 mask. */
#define BOD_VLMCFG_0_bp  1  /* Configuration bit 0 position. */
#define BOD_VLMCFG_1_bm  (1<<2)  /* Configuration bit 1 mask. */
#define BOD_VLMCFG_1_bp  2  /* Configuration bit 1 position. */

/* BOD.INTFLAGS  bit masks and bit positions */
#define BOD_VLMIF_bm  0x01  /* Voltage level monitor interrupt flag bit mask. */
#define BOD_VLMIF_bp  0  /* Voltage level monitor interrupt flag bit position. */

/* BOD.STATUS  bit masks and bit positions */
#define BOD_VLMS_bm  0x01  /* Voltage level monitor status bit mask. */
#define BOD_VLMS_bp  0  /* Voltage level monitor status bit position. */


/* CCL - Configurable Custom Logic */
/* CCL.CTRLA  bit masks and bit positions */
#define CCL_ENABLE_bm  0x01  /* Enable bit mask. */
#define CCL_ENABLE_bp  0  /* Enable bit position. */
#define CCL_RUNSTDBY_bm  0x40  /* Run in Standby bit mask. */
#define CCL_RUNSTDBY_bp  6  /* Run in Standby bit position. */

/* CCL.SEQCTRL0  bit masks and bit positions */
#define CCL_SEQSEL_gm  0x07  /* Sequential Selection group mask. */
#define CCL_SEQSEL_gp  0  /* Sequential Selection group position. */
#define CCL_SEQSEL_0_bm  (1<<0)  /* Sequential Selection bit 0 mask. */
#define CCL_SEQSEL_0_bp  0  /* Sequential Selection bit 0 position. */
#define CCL_SEQSEL_1_bm  (1<<1)  /* Sequential Selection bit 1 mask. */
#define CCL_SEQSEL_1_bp  1  /* Sequential Selection bit 1 position. */
#define CCL_SEQSEL_2_bm  (1<<2)  /* Sequential Selection bit 2 mask. */
#define CCL_SEQSEL_2_bp  2  /* Sequential Selection bit 2 position. */

/* CCL.LUT0CTRLA  bit masks and bit positions */
/* CCL_ENABLE  is already defined. */
#define CCL_OUTEN_bm  0x08  /* Output Enable bit mask. */
#define CCL_OUTEN_bp  3  /* Output Enable bit position. */
#define CCL_FILTSEL_gm  0x30  /* Filter Selection group mask. */
#define CCL_FILTSEL_gp  4  /* Filter Selection group position. */
#define CCL_FILTSEL_0_bm  (1<<4)  /* Filter Selection bit 0 mask. */
#define CCL_FILTSEL_0_bp  4  /* Filter Selection bit 0 position. */
#define CCL_FILTSEL_1_bm  (1<<5)  /* Filter Selection bit 1 mask. */
#define CCL_FILTSEL_1_bp  5  /* Filter Selection bit 1 position. */
#define CCL_CLKSRC_bm  0x40  /* Clock Source Selection bit mask. */
#define CCL_CLKSRC_bp  6  /* Clock Source Selection bit position. */
#define CCL_EDGEDET_bm  0x80  /* Edge Detection Enable bit mask. */
#define CCL_EDGEDET_bp  7  /* Edge Detection Enable bit position. */

/* CCL.LUT0CTRLB  bit masks and bit positions */
#define CCL_INSEL0_gm  0x0F  /* LUT Input 0 Source Selection group mask. */
#define CCL_INSEL0_gp  0  /* LUT Input 0 Source Selection group position. */
#define CCL_INSEL0_0_bm  (1<<0)  /* LUT Input 0 Source Selection bit 0 mask. */
#define CCL_INSEL0_0_bp  0  /* LUT Input 0 Source Selection bit 0 position. */
#define CCL_INSEL0_1_bm  (1<<1)  /* LUT Input 0 Source Selection bit 1 mask. */
#define CCL_INSEL0_1_bp  1  /* LUT Input 0 Source Selection bit 1 position. */
#define CCL_INSEL0_2_bm  (1<<2)  /* LUT Input 0 Source Selection bit 2 mask. */
#define CCL_INSEL0_2_bp  2  /* LUT Input 0 Source Selection bit 2 position. */
#define CCL_INSEL0_3_bm  (1<<3)  /* LUT Input 0 Source Selection bit 3 mask. */
#define CCL_INSEL0_3_bp  3  /* LUT Input 0 Source Selection bit 3 position. */
#define CCL_INSEL1_gm  0xF0  /* LUT Input 1 Source Selection group mask. */
#define CCL_INSEL1_gp  4  /* LUT Input 1 Source Selection group position. */
#define CCL_INSEL1_0_bm  (1<<4)  /* LUT Input 1 Source Selection bit 0 mask. */
#define CCL_INSEL1_0_bp  4  /* LUT Input 1 Source Selection bit 0 position. */
#define CCL_INSEL1_1_bm  (1<<5)  /* LUT Input 1 Source Selection bit 1 mask. */
#define CCL_INSEL1_1_bp  5  /* LUT Input 1 Source Selection bit 1 position. */
#define CCL_INSEL1_2_bm  (1<<6)  /* LUT Input 1 Source Selection bit 2 mask. */
#define CCL_INSEL1_2_bp  6  /* LUT Input 1 Source Selection bit 2 position. */
#define CCL_INSEL1_3_bm  (1<<7)  /* LUT Input 1 Source Selection bit 3 mask. */
#define CCL_INSEL1_3_bp  7  /* LUT Input 1 Source Selection bit 3 position. */

/* CCL.LUT0CTRLC  bit masks and bit positions */
#define CCL_INSEL2_gm  0x0F  /* LUT Input 2 Source Selection group mask. */
#define CCL_INSEL2_gp  0  /* LUT Input 2 Source Selection group position. */
#define CCL_INSEL2_0_bm  (1<<0)  /* LUT Input 2 Source Selection bit 0 mask. */
#define CCL_INSEL2_0_bp  0  /* LUT Input 2 Source Selection bit 0 position. */
#define CCL_INSEL2_1_bm  (1<<1)  /* LUT Input 2 Source Selection bit 1 mask. */
#define CCL_INSEL2_1_bp  1  /* LUT Input 2 Source Selection bit 1 position. */
#define CCL_INSEL2_2_bm  (1<<2)  /* LUT Input 2 Source Selection bit 2 mask. */
#define CCL_INSEL2_2_bp  2  /* LUT Input 2 Source Selection bit 2 position. */
#define CCL_INSEL2_3_bm  (1<<3)  /* LUT Input 2 Source Selection bit 3 mask. */
#define CCL_INSEL2_3_bp  3  /* LUT Input 2 Source Selection bit 3 position. */

/* CCL.TRUTH0  bit masks and bit positions */
#define CCL_TRUTH_gm  0xFF  /* Truth Table group mask. */
#define CCL_TRUTH_gp  0  /* Truth Table group position. */
#define CCL_TRUTH_0_bm  (1<<0)  /* Truth Table bit 0 mask. */
#define CCL_TRUTH_0_bp  0  /* Truth Table bit 0 position. */
#define CCL_TRUTH_1_bm  (1<<1)  /* Truth Table bit 1 mask. */
#define CCL_TRUTH_1_bp  1  /* Truth Table bit 1 position. */
#define CCL_TRUTH_2_bm  (1<<2)  /* Truth Table bit 2 mask. */
#define CCL_TRUTH_2_bp  2  /* Truth Table bit 2 position. */
#define CCL_TRUTH_3_bm  (1<<3)  /* Truth Table bit 3 mask. */
#define CCL_TRUTH_3_bp  3  /* Truth Table bit 3 position. */
#define CCL_TRUTH_4_bm  (1<<4)  /* Truth Table bit 4 mask. */
#define CCL_TRUTH_4_bp  4  /* Truth Table bit 4 position. */
#define CCL_TRUTH_5_bm  (1<<5)  /* Truth Table bit 5 mask. */
#define CCL_TRUTH_5_bp  5  /* Truth Table bit 5 position. */
#define CCL_TRUTH_6_bm  (1<<6)  /* Truth Table bit 6 mask. */
#define CCL_TRUTH_6_bp  6  /* Truth Table bit 6 position. */
#define CCL_TRUTH_7_bm  (1<<7)  /* Truth Table bit 7 mask. */
#define CCL_TRUTH_7_bp  7  /* Truth Table bit 7 position. */

/* CCL.LUT1CTRLA  bit masks and bit positions */
/* CCL_ENABLE  is already defined. */
/* CCL_OUTEN  is already defined. */
/* CCL_FILTSEL  is already defined. */
/* CCL_CLKSRC  is already defined. */
/* CCL_EDGEDET  is already defined. */

/* CCL.LUT1CTRLB  bit masks and bit positions */
/* CCL_INSEL0  is already defined. */
/* CCL_INSEL1  is already defined. */

/* CCL.LUT1CTRLC  bit masks and bit positions */
/* CCL_INSEL2  is already defined. */

/* CCL.TRUTH1  bit masks and bit positions */
/* CCL_TRUTH  is already defined. */


/* CLKCTRL - Clock controller */
/* CLKCTRL.MCLKCTRLA  bit masks and bit positions */
#define CLKCTRL_CLKSEL_gm  0x03  /* Clock select group mask. */
#define CLKCTRL_CLKSEL_gp  0  /* Clock select group position. */
#define CLKCTRL_CLKSEL_0_bm  (1<<0)  /* Clock select bit 0 mask. */
#define CLKCTRL_CLKSEL_0_bp  0  /* Clock select bit 0 position. */
#define CLKCTRL_CLKSEL_1_bm  (1<<1)  /* Clock select bit 1 mask. */
#define CLKCTRL_CLKSEL_1_bp  1  /* Clock select bit 1 position. */
#define CLKCTRL_CLKOUT_bm  0x80  /* System clock out bit mask. */
#define CLKCTRL_CLKOUT_bp  7  /* System clock out bit position. */

/* CLKCTRL.MCLKCTRLB  bit masks and bit positions */
#define CLKCTRL_PEN_bm  0x01  /* Prescaler enable bit mask. */
#define CLKCTRL_PEN_bp  0  /* Prescaler enable bit position. */
#define CLKCTRL_PDIV_gm  0x1E  /* Prescaler division group mask. */
#define CLKCTRL_PDIV_gp  1  /* Prescaler division group position. */
#define CLKCTRL_PDIV_0_bm  (1<<1)  /* Prescaler division bit 0 mask. */
#define CLKCTRL_PDIV_0_bp  1  /* Prescaler division bit 0 position. */
#define CLKCTRL_PDIV_1_bm  (1<<2)  /* Prescaler division bit 1 mask. */
#define CLKCTRL_PDIV_1_bp  2  /* Prescaler division bit 1 position. */
#define CLKCTRL_PDIV_2_bm  (1<<3)  /* Prescaler division bit 2 mask. */
#define CLKCTRL_PDIV_2_bp  3  /* Prescaler division bit 2 position. */
#define CLKCTRL_PDIV_3_bm  (1<<4)  /* Prescaler division bit 3 mask. */
#define CLKCTRL_PDIV_3_bp  4  /* Prescaler division bit 3 position. */

/* CLKCTRL.MCLKLOCK  bit masks and bit positions */
#define CLKCTRL_LOCKEN_bm  0x01  /* Lock enable bit mask. */
#define CLKCTRL_LOCKEN_bp  0  /* Lock enable bit position. */

/* CLKCTRL.MCLKSTATUS  bit masks and bit positions */
#define CLKCTRL_SOSC_bm  0x01  /* System Oscillator changing bit mask. */
#define CLKCTRL_SOSC_bp  0  /* System Oscillator changing bit position. */
#define CLKCTRL_OSC20MS_bm  0x10  /* 20MHz oscillator status bit mask. */
#define CLKCTRL_OSC20MS_bp  4  /* 20MHz oscillator status bit position. */
#define CLKCTRL_OSC32KS_bm  0x20  /* 32KHz oscillator status bit mask. */
#define CLKCTRL_OSC32KS_bp  5  /* 32KHz oscillator status bit position. */
#define CLKCTRL_EXTS_bm  0x80  /* External Clock status bit mask. */
#define CLKCTRL_EXTS_bp  7  /* External Clock status bit position. */

/* CLKCTRL.OSC20MCTRLA  bit masks and bit positions */
#define CLKCTRL_RUNSTDBY_bm  0x02  /* Run standby bit mask. */
#define CLKCTRL_RUNSTDBY_bp  1  /* Run standby bit position. */

/* CLKCTRL.OSC20MCALIBA  bit masks and bit positions */
#define CLKCTRL_CAL20M_gm  0x3F  /* Calibration group mask. */
#define CLKCTRL_CAL20M_gp  0  /* Calibration group position. */
#define CLKCTRL_CAL20M_0_bm  (1<<0)  /* Calibration bit 0 mask. */
#define CLKCTRL_CAL20M_0_bp  0  /* Calibration bit 0 position. */
#define CLKCTRL_CAL20M_1_bm  (1<<1)  /* Calibration bit 1 mask. */
#define CLKCTRL_CAL20M_1_bp  1  /* Calibration bit 1 position. */
#define CLKCTRL_CAL20M_2_bm  (1<<2)  /* Calibration bit 2 mask. */
#define CLKCTRL_CAL20M_2_bp  2  /* Calibration bit 2 position. */
#define CLKCTRL_CAL20M_3_bm  (1<<3)  /* Calibration bit 3 mask. */
#define CLKCTRL_CAL20M_3_bp  3  /* Calibration bit 3 position. */
#define CLKCTRL_CAL20M_4_bm  (1<<4)  /* Calibration bit 4 mask. */
#define CLKCTRL_CAL20M_4_bp  4  /* Calibration bit 4 position. */
#define CLKCTRL_CAL20M_5_bm  (1<<5)  /* Calibration bit 5 mask. */
#define CLKCTRL_CAL20M_5_bp  5  /* Calibration bit 5 position. */

/* CLKCTRL.OSC20MCALIBB  bit masks and bit positions */
#define CLKCTRL_TEMPCAL20M_gm  0x0F  /* Oscillator temperature coefficient group mask. */
#define CLKCTRL_TEMPCAL20M_gp  0  /* Oscillator temperature coefficient group position. */
#define CLKCTRL_TEMPCAL20M_0_bm  (1<<0)  /* Oscillator temperature coefficient bit 0 mask. */
#define CLKCTRL_TEMPCAL20M_0_bp  0  /* Oscillator temperature coefficient bit 0 position. */
#define CLKCTRL_TEMPCAL20M_1_bm  (1<<1)  /* Oscillator temperature coefficient bit 1 mask. */
#define CLKCTRL_TEMPCAL20M_1_bp  1  /* Oscillator temperature coefficient bit 1 position. */
#define CLKCTRL_TEMPCAL20M_2_bm  (1<<2)  /* Oscillator temperature coefficient bit 2 mask. */
#define CLKCTRL_TEMPCAL20M_2_bp  2  /* Oscillator temperature coefficient bit 2 position. */
#define CLKCTRL_TEMPCAL20M_3_bm  (1<<3)  /* Oscillator temperature coefficient bit 3 mask. */
#define CLKCTRL_TEMPCAL20M_3_bp  3  /* Oscillator temperature coefficient bit 3 position. */
#define CLKCTRL_LOCK_bm  0x80  /* Lock bit mask. */
#define CLKCTRL_LOCK_bp  7  /* Lock bit position. */

/* CLKCTRL.OSC32KCTRLA  bit masks and bit positions */
/* CLKCTRL_RUNSTDBY  is already defined. */


/* CPU - CPU */
/* CPU.CCP  bit masks and bit positions */
#define CPU_CCP_gm  0xFF  /* CCP signature group mask. */
#define CPU_CCP_gp  0  /* CCP signature group position. */
#define CPU_CCP_0_bm  (1<<0)  /* CCP signature bit 0 mask. */
#define CPU_CCP_0_bp  0  /* CCP signature bit 0 position. */
#define CPU_CCP_1_bm  (1<<1)  /* CCP signature bit 1 mask. */
#define CPU_CCP_1_bp  1  /* CCP signature bit 1 position. */
#define CPU_CCP_2_bm  (1<<2)  /* CCP signature bit 2 mask. */
#define CPU_CCP_2_bp  2  /* CCP signature bit 2 position. */
#define CPU_CCP_3_bm  (1<<3)  /* CCP signature bit 3 mask. */
#define CPU_CCP_3_bp  3  /* CCP signature bit 3 position. */
#define CPU_CCP_4_bm  (1<<4)  /* CCP signature bit 4 mask. */
#define CPU_CCP_4_bp  4  /* CCP signature bit 4 position. */
#define CPU_CCP_5_bm  (1<<5)  /* CCP signature bit 5 mask. */
#define CPU_CCP_5_bp  5  /* CCP signature bit 5 position. */
#define CPU_CCP_6_bm  (1<<6)  /* CCP signature bit 6 mask. */
#define CPU_CCP_6_bp  6  /* CCP signature bit 6 position. */
#define CPU_CCP_7_bm  (1<<7)  /* CCP signature bit 7 mask. */
#define CPU_CCP_7_bp  7  /* CCP signature bit 7 position. */

/* CPU.SREG  bit masks and bit positions */
#define CPU_C_bm  0x01  /* Carry Flag bit mask. */
#define CPU_C_bp  0  /* Carry Flag bit position. */
#define CPU_Z_bm  0x02  /* Zero Flag bit mask. */
#define CPU_Z_bp  1  /* Zero Flag bit position. */
#define CPU_N_bm  0x04  /* Negative Flag bit mask. */
#define CPU_N_bp  2  /* Negative Flag bit position. */
#define CPU_V_bm  0x08  /* Two's Complement Overflow Flag bit mask. */
#define CPU_V_bp  3  /* Two's Complement Overflow Flag bit position. */
#define CPU_S_bm  0x10  /* N Exclusive Or V Flag bit mask. */
#define CPU_S_bp  4  /* N Exclusive Or V Flag bit position. */
#define CPU_H_bm  0x20  /* Half Carry Flag bit mask. */
#define CPU_H_bp  5  /* Half Carry Flag bit position. */
#define CPU_T_bm  0x40  /* Transfer Bit bit mask. */
#define CPU_T_bp  6  /* Transfer Bit bit position. */
#define CPU_I_bm  0x80  /* Global Interrupt Enable Flag bit mask. */
#define CPU_I_bp  7  /* Global Interrupt Enable Flag bit position. */


/* CPUINT - Interrupt Controller */
/* CPUINT.CTRLA  bit masks and bit positions */
#define CPUINT_LVL0RR_bm  0x01  /* Round-robin Scheduling Enable bit mask. */
#define CPUINT_LVL0RR_bp  0  /* Round-robin Scheduling Enable bit position. */
#define CPUINT_CVT_bm  0x20  /* Compact Vector Table bit mask. */
#define CPUINT_CVT_bp  5  /* Compact Vector Table bit position. */
#define CPUINT_IVSEL_bm  0x40  /* Interrupt Vector Select bit mask. */
#define CPUINT_IVSEL_bp  6  /* Interrupt Vector Select bit position. */

/* CPUINT.STATUS  bit masks and bit positions */
#define CPUINT_LVL0EX_bm  0x01  /* Level 0 Interrupt Executing bit mask. */
#define CPUINT_LVL0EX_bp  0  /* Level 0 Interrupt Executing bit position. */
#define CPUINT_LVL1EX_bm  0x02  /* Level 1 Interrupt Executing bit mask. */
#define CPUINT_LVL1EX_bp  1  /* Level 1 Interrupt Executing bit position. */
#define CPUINT_NMIEX_bm  0x80  /* Non-maskable Interrupt Executing bit mask. */
#define CPUINT_NMIEX_bp  7  /* Non-maskable Interrupt Executing bit position. */

/* CPUINT.LVL0PRI  bit masks and bit positions */
#define CPUINT_LVL0PRI_gm  0xFF  /* Interrupt Level Priority group mask. */
#define CPUINT_LVL0PRI_gp  0  /* Interrupt Level Priority group position. */
#define CPUINT_LVL0PRI_0_bm  (1<<0)  /* Interrupt Level Priority bit 0 mask. */
#define CPUINT_LVL0PRI_0_bp  0  /* Interrupt Level Priority bit 0 position. */
#define CPUINT_LVL0PRI_1_bm  (1<<1)  /* Interrupt Level Priority bit 1 mask. */
#define CPUINT_LVL0PRI_1_bp  1  /* Interrupt Level Priority bit 1 position. */
#define CPUINT_LVL0PRI_2_bm  (1<<2)  /* Interrupt Level Priority bit 2 mask. */
#define CPUINT_LVL0PRI_2_bp  2  /* Interrupt Level Priority bit 2 position. */
#define CPUINT_LVL0PRI_3_bm  (1<<3)  /* Interrupt Level Priority bit 3 mask. */
#define CPUINT_LVL0PRI_3_bp  3  /* Interrupt Level Priority bit 3 position. */
#define CPUINT_LVL0PRI_4_bm  (1<<4)  /* Interrupt Level Priority bit 4 mask. */
#define CPUINT_LVL0PRI_4_bp  4  /* Interrupt Level Priority bit 4 position. */
#define CPUINT_LVL0PRI_5_bm  (1<<5)  /* Interrupt Level Priority bit 5 mask. */
#define CPUINT_LVL0PRI_5_bp  5  /* Interrupt Level Priority bit 5 position. */
#define CPUINT_LVL0PRI_6_bm  (1<<6)  /* Interrupt Level Priority bit 6 mask. */
#define CPUINT_LVL0PRI_6_bp  6  /* Interrupt Level Priority bit 6 position. */
#define CPUINT_LVL0PRI_7_bm  (1<<7)  /* Interrupt Level Priority bit 7 mask. */
#define CPUINT_LVL0PRI_7_bp  7  /* Interrupt Level Priority bit 7 position. */

/* CPUINT.LVL1VEC  bit masks and bit positions */
#define CPUINT_LVL1VEC_gm  0xFF  /* Interrupt Vector with High Priority group mask. */
#define CPUINT_LVL1VEC_gp  0  /* Interrupt Vector with High Priority group position. */
#define CPUINT_LVL1VEC_0_bm  (1<<0)  /* Interrupt Vector with High Priority bit 0 mask. */
#define CPUINT_LVL1VEC_0_bp  0  /* Interrupt Vector with High Priority bit 0 position. */
#define CPUINT_LVL1VEC_1_bm  (1<<1)  /* Interrupt Vector with High Priority bit 1 mask. */
#define CPUINT_LVL1VEC_1_bp  1  /* Interrupt Vector with High Priority bit 1 position. */
#define CPUINT_LVL1VEC_2_bm  (1<<2)  /* Interrupt Vector with High Priority bit 2 mask. */
#define CPUINT_LVL1VEC_2_bp  2  /* Interrupt Vector with High Priority bit 2 position. */
#define CPUINT_LVL1VEC_3_bm  (1<<3)  /* Interrupt Vector with High Priority bit 3 mask. */
#define CPUINT_LVL1VEC_3_bp  3  /* Interrupt Vector with High Priority bit 3 position. */
#define CPUINT_LVL1VEC_4_bm  (1<<4)  /* Interrupt Vector with High Priority bit 4 mask. */
#define CPUINT_LVL1VEC_4_bp  4  /* Interrupt Vector with High Priority bit 4 position. */
#define CPUINT_LVL1VEC_5_bm  (1<<5)  /* Interrupt Vector with High Priority bit 5 mask. */
#define CPUINT_LVL1VEC_5_bp  5  /* Interrupt Vector with High Priority bit 5 position. */
#define CPUINT_LVL1VEC_6_bm  (1<<6)  /* Interrupt Vector with High Priority bit 6 mask. */
#define CPUINT_LVL1VEC_6_bp  6  /* Interrupt Vector with High Priority bit 6 position. */
#define CPUINT_LVL1VEC_7_bm  (1<<7)  /* Interrupt Vector with High Priority bit 7 mask. */
#define CPUINT_LVL1VEC_7_bp  7  /* Interrupt Vector with High Priority bit 7 position. */


/* CRCSCAN - CRCSCAN */
/* CRCSCAN.CTRLA  bit masks and bit positions */
#define CRCSCAN_ENABLE_bm  0x01  /* Enable CRC scan bit mask. */
#define CRCSCAN_ENABLE_bp  0  /* Enable CRC scan bit position. */
#define CRCSCAN_NMIEN_bm  0x02  /* Enable NMI Trigger bit mask. */
#define CRCSCAN_NMIEN_bp  1  /* Enable NMI Trigger bit position. */
#define CRCSCAN_RESET_bm  0x80  /* Reset CRC scan bit mask. */
#define CRCSCAN_RESET_bp  7  /* Reset CRC scan bit position. */

/* CRCSCAN.CTRLB  bit masks and bit positions */
#define CRCSCAN_SRC_gm  0x03  /* CRC Source group mask. */
#define CRCSCAN_SRC_gp  0  /* CRC Source group position. */
#define CRCSCAN_SRC_0_bm  (1<<0)  /* CRC Source bit 0 mask. */
#define CRCSCAN_SRC_0_bp  0  /* CRC Source bit 0 position. */
#define CRCSCAN_SRC_1_bm  (1<<1)  /* CRC Source bit 1 mask. */
#define CRCSCAN_SRC_1_bp  1  /* CRC Source bit 1 position. */
#define CRCSCAN_MODE_gm  0x30  /* CRC Flash Access Mode group mask. */
#define CRCSCAN_MODE_gp  4  /* CRC Flash Access Mode group position. */
#define CRCSCAN_MODE_0_bm  (1<<4)  /* CRC Flash Access Mode bit 0 mask. */
#define CRCSCAN_MODE_0_bp  4  /* CRC Flash Access Mode bit 0 position. */
#define CRCSCAN_MODE_1_bm  (1<<5)  /* CRC Flash Access Mode bit 1 mask. */
#define CRCSCAN_MODE_1_bp  5  /* CRC Flash Access Mode bit 1 position. */

/* CRCSCAN.STATUS  bit masks and bit positions */
#define CRCSCAN_BUSY_bm  0x01  /* CRC Busy bit mask. */
#define CRCSCAN_BUSY_bp  0  /* CRC Busy bit position. */
#define CRCSCAN_OK_bm  0x02  /* CRC Ok bit mask. */
#define CRCSCAN_OK_bp  1  /* CRC Ok bit position. */


/* EVSYS - Event System */
/* EVSYS.ASYNCCH0  bit masks and bit positions */
#define EVSYS_ASYNCCH0_gm  0xFF  /* Asynchronous Channel 0 Generator Selection group mask. */
#define EVSYS_ASYNCCH0_gp  0  /* Asynchronous Channel 0 Generator Selection group position. */
#define EVSYS_ASYNCCH0_0_bm  (1<<0)  /* Asynchronous Channel 0 Generator Selection bit 0 mask. */
#define EVSYS_ASYNCCH0_0_bp  0  /* Asynchronous Channel 0 Generator Selection bit 0 position. */
#define EVSYS_ASYNCCH0_1_bm  (1<<1)  /* Asynchronous Channel 0 Generator Selection bit 1 mask. */
#define EVSYS_ASYNCCH0_1_bp  1  /* Asynchronous Channel 0 Generator Selection bit 1 position. */
#define EVSYS_ASYNCCH0_2_bm  (1<<2)  /* Asynchronous Channel 0 Generator Selection bit 2 mask. */
#define EVSYS_ASYNCCH0_2_bp  2  /* Asynchronous Channel 0 Generator Selection bit 2 position. */
#define EVSYS_ASYNCCH0_3_bm  (1<<3)  /* Asynchronous Channel 0 Generator Selection bit 3 mask. */
#define EVSYS_ASYNCCH0_3_bp  3  /* Asynchronous Channel 0 Generator Selection bit 3 position. */
#define EVSYS_ASYNCCH0_4_bm  (1<<4)  /* Asynchronous Channel 0 Generator Selection bit 4 mask. */
#define EVSYS_ASYNCCH0_4_bp  4  /* Asynchronous Channel 0 Generator Selection bit 4 position. */
#define EVSYS_ASYNCCH0_5_bm  (1<<5)  /* Asynchronous Channel 0 Generator Selection bit 5 mask. */
#define EVSYS_ASYNCCH0_5_bp  5  /* Asynchronous Channel 0 Generator Selection bit 5 position. */
#define EVSYS_ASYNCCH0_6_bm  (1<<6)  /* Asynchronous Channel 0 Generator Selection bit 6 mask. */
#define EVSYS_ASYNCCH0_6_bp  6  /* Asynchronous Channel 0 Generator Selection bit 6 position. */
#define EVSYS_ASYNCCH0_7_bm  (1<<7)  /* Asynchronous Channel 0 Generator Selection bit 7 mask. */
#define EVSYS_ASYNCCH0_7_bp  7  /* Asynchronous Channel 0 Generator Selection bit 7 position. */

/* EVSYS.ASYNCCH1  bit masks and bit positions */
#define EVSYS_ASYNCCH1_gm  0xFF  /* Asynchronous Channel 1 Generator Selection group mask. */
#define EVSYS_ASYNCCH1_gp  0  /* Asynchronous Channel 1 Generator Selection group position. */
#define EVSYS_ASYNCCH1_0_bm  (1<<0)  /* Asynchronous Channel 1 Generator Selection bit 0 mask. */
#define EVSYS_ASYNCCH1_0_bp  0  /* Asynchronous Channel 1 Generator Selection bit 0 position. */
#define EVSYS_ASYNCCH1_1_bm  (1<<1)  /* Asynchronous Channel 1 Generator Selection bit 1 mask. */
#define EVSYS_ASYNCCH1_1_bp  1  /* Asynchronous Channel 1 Generator Selection bit 1 position. */
#define EVSYS_ASYNCCH1_2_bm  (1<<2)  /* Asynchronous Channel 1 Generator Selection bit 2 mask. */
#define EVSYS_ASYNCCH1_2_bp  2  /* Asynchronous Channel 1 Generator Selection bit 2 position. */
#define EVSYS_ASYNCCH1_3_bm  (1<<3)  /* Asynchronous Channel 1 Generator Selection bit 3 mask. */
#define EVSYS_ASYNCCH1_3_bp  3  /* Asynchronous Channel 1 Generator Selection bit 3 position. */
#define EVSYS_ASYNCCH1_4_bm  (1<<4)  /* Asynchronous Channel 1 Generator Selection bit 4 mask. */
#define EVSYS_ASYNCCH1_4_bp  4  /* Asynchronous Channel 1 Generator Selection bit 4 position. */
#define EVSYS_ASYNCCH1_5_bm  (1<<5)  /* Asynchronous Channel 1 Generator Selection bit 5 mask. */
#define EVSYS_ASYNCCH1_5_bp  5  /* Asynchronous Channel 1 Generator Selection bit 5 position. */
#define EVSYS_ASYNCCH1_6_bm  (1<<6)  /* Asynchronous Channel 1 Generator Selection bit 6 mask. */
#define EVSYS_ASYNCCH1_6_bp  6  /* Asynchronous Channel 1 Generator Selection bit 6 position. */
#define EVSYS_ASYNCCH1_7_bm  (1<<7)  /* Asynchronous Channel 1 Generator Selection bit 7 mask. */
#define EVSYS_ASYNCCH1_7_bp  7  /* Asynchronous Channel 1 Generator Selection bit 7 position. */

/* EVSYS.SYNCCH0  bit masks and bit positions */
#define EVSYS_SYNCCH0_gm  0xFF  /* Synchronous Channel 0 Generator Selection group mask. */
#define EVSYS_SYNCCH0_gp  0  /* Synchronous Channel 0 Generator Selection group position. */
#define EVSYS_SYNCCH0_0_bm  (1<<0)  /* Synchronous Channel 0 Generator Selection bit 0 mask. */
#define EVSYS_SYNCCH0_0_bp  0  /* Synchronous Channel 0 Generator Selection bit 0 position. */
#define EVSYS_SYNCCH0_1_bm  (1<<1)  /* Synchronous Channel 0 Generator Selection bit 1 mask. */
#define EVSYS_SYNCCH0_1_bp  1  /* Synchronous Channel 0 Generator Selection bit 1 position. */
#define EVSYS_SYNCCH0_2_bm  (1<<2)  /* Synchronous Channel 0 Generator Selection bit 2 mask. */
#define EVSYS_SYNCCH0_2_bp  2  /* Synchronous Channel 0 Generator Selection bit 2 position. */
#define EVSYS_SYNCCH0_3_bm  (1<<3)  /* Synchronous Channel 0 Generator Selection bit 3 mask. */
#define EVSYS_SYNCCH0_3_bp  3  /* Synchronous Channel 0 Generator Selection bit 3 position. */
#define EVSYS_SYNCCH0_4_bm  (1<<4)  /* Synchronous Channel 0 Generator Selection bit 4 mask. */
#define EVSYS_SYNCCH0_4_bp  4  /* Synchronous Channel 0 Generator Selection bit 4 position. */
#define EVSYS_SYNCCH0_5_bm  (1<<5)  /* Synchronous Channel 0 Generator Selection bit 5 mask. */
#define EVSYS_SYNCCH0_5_bp  5  /* Synchronous Channel 0 Generator Selection bit 5 position. */
#define EVSYS_SYNCCH0_6_bm  (1<<6)  /* Synchronous Channel 0 Generator Selection bit 6 mask. */
#define EVSYS_SYNCCH0_6_bp  6  /* Synchronous Channel 0 Generator Selection bit 6 position. */
#define EVSYS_SYNCCH0_7_bm  (1<<7)  /* Synchronous Channel 0 Generator Selection bit 7 mask. */
#define EVSYS_SYNCCH0_7_bp  7  /* Synchronous Channel 0 Generator Selection bit 7 position. */

/* EVSYS.ASYNCUSER0  bit masks and bit positions */
#define EVSYS_ASYNCUSER0_gm  0xFF  /* Asynchronous User Ch 0 Input Selection - TCB0 group mask. */
#define EVSYS_ASYNCUSER0_gp  0  /* Asynchronous User Ch 0 Input Selection - TCB0 group position. */
#define EVSYS_ASYNCUSER0_0_bm  (1<<0)  /* Asynchronous User Ch 0 Input Selection - TCB0 bit 0 mask. */
#define EVSYS_ASYNCUSER0_0_bp  0  /* Asynchronous User Ch 0 Input Selection - TCB0 bit 0 position. */
#define EVSYS_ASYNCUSER0_1_bm  (1<<1)  /* Asynchronous User Ch 0 Input Selection - TCB0 bit 1 mask. */
#define EVSYS_ASYNCUSER0_1_bp  1  /* Asynchronous User Ch 0 Input Selection - TCB0 bit 1 position. */
#define EVSYS_ASYNCUSER0_2_bm  (1<<2)  /* Asynchronous User Ch 0 Input Selection - TCB0 bit 2 mask. */
#define EVSYS_ASYNCUSER0_2_bp  2  /* Asynchronous User Ch 0 Input Selection - TCB0 bit 2 position. */
#define EVSYS_ASYNCUSER0_3_bm  (1<<3)  /* Asynchronous User Ch 0 Input Selection - TCB0 bit 3 mask. */
#define EVSYS_ASYNCUSER0_3_bp  3  /* Asynchronous User Ch 0 Input Selection - TCB0 bit 3 position. */
#define EVSYS_ASYNCUSER0_4_bm  (1<<4)  /* Asynchronous User Ch 0 Input Selection - TCB0 bit 4 mask. */
#define EVSYS_ASYNCUSER0_4_bp  4  /* Asynchronous User Ch 0 Input Selection - TCB0 bit 4 position. */
#define EVSYS_ASYNCUSER0_5_bm  (1<<5)  /* Asynchronous User Ch 0 Input Selection - TCB0 bit 5 mask. */
#define EVSYS_ASYNCUSER0_5_bp  5  /* Asynchronous User Ch 0 Input Selection - TCB0 bit 5 position. */
#define EVSYS_ASYNCUSER0_6_bm  (1<<6)  /* Asynchronous User Ch 0 Input Selection - TCB0 bit 6 mask. */
#define EVSYS_ASYNCUSER0_6_bp  6  /* Asynchronous User Ch 0 Input Selection - TCB0 bit 6 position. */
#define EVSYS_ASYNCUSER0_7_bm  (1<<7)  /* Asynchronous User Ch 0 Input Selection - TCB0 bit 7 mask. */
#define EVSYS_ASYNCUSER0_7_bp  7  /* Asynchronous User Ch 0 Input Selection - TCB0 bit 7 position. */

/* EVSYS.ASYNCUSER1  bit masks and bit positions */
#define EVSYS_ASYNCUSER1_gm  0xFF  /* Asynchronous User Ch 1 Input Selection - ADC0 group mask. */
#define EVSYS_ASYNCUSER1_gp  0  /* Asynchronous User Ch 1 Input Selection - ADC0 group position. */
#define EVSYS_ASYNCUSER1_0_bm  (1<<0)  /* Asynchronous User Ch 1 Input Selection - ADC0 bit 0 mask. */
#define EVSYS_ASYNCUSER1_0_bp  0  /* Asynchronous User Ch 1 Input Selection - ADC0 bit 0 position. */
#define EVSYS_ASYNCUSER1_1_bm  (1<<1)  /* Asynchronous User Ch 1 Input Selection - ADC0 bit 1 mask. */
#define EVSYS_ASYNCUSER1_1_bp  1  /* Asynchronous User Ch 1 Input Selection - ADC0 bit 1 position. */
#define EVSYS_ASYNCUSER1_2_bm  (1<<2)  /* Asynchronous User Ch 1 Input Selection - ADC0 bit 2 mask. */
#define EVSYS_ASYNCUSER1_2_bp  2  /* Asynchronous User Ch 1 Input Selection - ADC0 bit 2 position. */
#define EVSYS_ASYNCUSER1_3_bm  (1<<3)  /* Asynchronous User Ch 1 Input Selection - ADC0 bit 3 mask. */
#define EVSYS_ASYNCUSER1_3_bp  3  /* Asynchronous User Ch 1 Input Selection - ADC0 bit 3 position. */
#define EVSYS_ASYNCUSER1_4_bm  (1<<4)  /* Asynchronous User Ch 1 Input Selection - ADC0 bit 4 mask. */
#define EVSYS_ASYNCUSER1_4_bp  4  /* Asynchronous User Ch 1 Input Selection - ADC0 bit 4 position. */
#define EVSYS_ASYNCUSER1_5_bm  (1<<5)  /* Asynchronous User Ch 1 Input Selection - ADC0 bit 5 mask. */
#define EVSYS_ASYNCUSER1_5_bp  5  /* Asynchronous User Ch 1 Input Selection - ADC0 bit 5 position. */
#define EVSYS_ASYNCUSER1_6_bm  (1<<6)  /* Asynchronous User Ch 1 Input Selection - ADC0 bit 6 mask. */
#define EVSYS_ASYNCUSER1_6_bp  6  /* Asynchronous User Ch 1 Input Selection - ADC0 bit 6 position. */
#define EVSYS_ASYNCUSER1_7_bm  (1<<7)  /* Asynchronous User Ch 1 Input Selection - ADC0 bit 7 mask. */
#define EVSYS_ASYNCUSER1_7_bp  7  /* Asynchronous User Ch 1 Input Selection - ADC0 bit 7 position. */

/* EVSYS.ASYNCUSER2  bit masks and bit positions */
#define EVSYS_ASYNCUSER2_gm  0xFF  /* Asynchronous User Ch 2 Input Selection - CCL LUT0 Event 0 group mask. */
#define EVSYS_ASYNCUSER2_gp  0  /* Asynchronous User Ch 2 Input Selection - CCL LUT0 Event 0 group position. */
#define EVSYS_ASYNCUSER2_0_bm  (1<<0)  /* Asynchronous User Ch 2 Input Selection - CCL LUT0 Event 0 bit 0 mask. */
#define EVSYS_ASYNCUSER2_0_bp  0  /* Asynchronous User Ch 2 Input Selection - CCL LUT0 Event 0 bit 0 position. */
#define EVSYS_ASYNCUSER2_1_bm  (1<<1)  /* Asynchronous User Ch 2 Input Selection - CCL LUT0 Event 0 bit 1 mask. */
#define EVSYS_ASYNCUSER2_1_bp  1  /* Asynchronous User Ch 2 Input Selection - CCL LUT0 Event 0 bit 1 position. */
#define EVSYS_ASYNCUSER2_2_bm  (1<<2)  /* Asynchronous User Ch 2 Input Selection - CCL LUT0 Event 0 bit 2 mask. */
#define EVSYS_ASYNCUSER2_2_bp  2  /* Asynchronous User Ch 2 Input Selection - CCL LUT0 Event 0 bit 2 position. */
#define EVSYS_ASYNCUSER2_3_bm  (1<<3)  /* Asynchronous User Ch 2 Input Selection - CCL LUT0 Event 0 bit 3 mask. */
#define EVSYS_ASYNCUSER2_3_bp  3  /* Asynchronous User Ch 2 Input Selection - CCL LUT0 Event 0 bit 3 position. */
#define EVSYS_ASYNCUSER2_4_bm  (1<<4)  /* Asynchronous User Ch 2 Input Selection - CCL LUT0 Event 0 bit 4 mask. */
#define EVSYS_ASYNCUSER2_4_bp  4  /* Asynchronous User Ch 2 Input Selection - CCL LUT0 Event 0 bit 4 position. */
#define EVSYS_ASYNCUSER2_5_bm  (1<<5)  /* Asynchronous User Ch 2 Input Selection - CCL LUT0 Event 0 bit 5 mask. */
#define EVSYS_ASYNCUSER2_5_bp  5  /* Asynchronous User Ch 2 Input Selection - CCL LUT0 Event 0 bit 5 position. */
#define EVSYS_ASYNCUSER2_6_bm  (1<<6)  /* Asynchronous User Ch 2 Input Selection - CCL LUT0 Event 0 bit 6 mask. */
#define EVSYS_ASYNCUSER2_6_bp  6  /* Asynchronous User Ch 2 Input Selection - CCL LUT0 Event 0 bit 6 position. */
#define EVSYS_ASYNCUSER2_7_bm  (1<<7)  /* Asynchronous User Ch 2 Input Selection - CCL LUT0 Event 0 bit 7 mask. */
#define EVSYS_ASYNCUSER2_7_bp  7  /* Asynchronous User Ch 2 Input Selection - CCL LUT0 Event 0 bit 7 position. */

/* EVSYS.ASYNCUSER3  bit masks and bit positions */
#define EVSYS_ASYNCUSER3_gm  0xFF  /* Asynchronous User Ch 3 Input Selection - CCL LUT1 Event 0 group mask. */
#define EVSYS_ASYNCUSER3_gp  0  /* Asynchronous User Ch 3 Input Selection - CCL LUT1 Event 0 group position. */
#define EVSYS_ASYNCUSER3_0_bm  (1<<0)  /* Asynchronous User Ch 3 Input Selection - CCL LUT1 Event 0 bit 0 mask. */
#define EVSYS_ASYNCUSER3_0_bp  0  /* Asynchronous User Ch 3 Input Selection - CCL LUT1 Event 0 bit 0 position. */
#define EVSYS_ASYNCUSER3_1_bm  (1<<1)  /* Asynchronous User Ch 3 Input Selection - CCL LUT1 Event 0 bit 1 mask. */
#define EVSYS_ASYNCUSER3_1_bp  1  /* Asynchronous User Ch 3 Input Selection - CCL LUT1 Event 0 bit 1 position. */
#define EVSYS_ASYNCUSER3_2_bm  (1<<2)  /* Asynchronous User Ch 3 Input Selection - CCL LUT1 Event 0 bit 2 mask. */
#define EVSYS_ASYNCUSER3_2_bp  2  /* Asynchronous User Ch 3 Input Selection - CCL LUT1 Event 0 bit 2 position. */
#define EVSYS_ASYNCUSER3_3_bm  (1<<3)  /* Asynchronous User Ch 3 Input Selection - CCL LUT1 Event 0 bit 3 mask. */
#define EVSYS_ASYNCUSER3_3_bp  3  /* Asynchronous User Ch 3 Input Selection - CCL LUT1 Event 0 bit 3 position. */
#define EVSYS_ASYNCUSER3_4_bm  (1<<4)  /* Asynchronous User Ch 3 Input Selection - CCL LUT1 Event 0 bit 4 mask. */
#define EVSYS_ASYNCUSER3_4_bp  4  /* Asynchronous User Ch 3 Input Selection - CCL LUT1 Event 0 bit 4 position. */
#define EVSYS_ASYNCUSER3_5_bm  (1<<5)  /* Asynchronous User Ch 3 Input Selection - CCL LUT1 Event 0 bit 5 mask. */
#define EVSYS_ASYNCUSER3_5_bp  5  /* Asynchronous User Ch 3 Input Selection - CCL LUT1 Event 0 bit 5 position. */
#define EVSYS_ASYNCUSER3_6_bm  (1<<6)  /* Asynchronous User Ch 3 Input Selection - CCL LUT1 Event 0 bit 6 mask. */
#define EVSYS_ASYNCUSER3_6_bp  6  /* Asynchronous User Ch 3 Input Selection - CCL LUT1 Event 0 bit 6 position. */
#define EVSYS_ASYNCUSER3_7_bm  (1<<7)  /* Asynchronous User Ch 3 Input Selection - CCL LUT1 Event 0 bit 7 mask. */
#define EVSYS_ASYNCUSER3_7_bp  7  /* Asynchronous User Ch 3 Input Selection - CCL LUT1 Event 0 bit 7 position. */

/* EVSYS.ASYNCUSER4  bit masks and bit positions */
#define EVSYS_ASYNCUSER4_gm  0xFF  /* Asynchronous User Ch 4 Input Selection - CCL LUT0 Event 1 group mask. */
#define EVSYS_ASYNCUSER4_gp  0  /* Asynchronous User Ch 4 Input Selection - CCL LUT0 Event 1 group position. */
#define EVSYS_ASYNCUSER4_0_bm  (1<<0)  /* Asynchronous User Ch 4 Input Selection - CCL LUT0 Event 1 bit 0 mask. */
#define EVSYS_ASYNCUSER4_0_bp  0  /* Asynchronous User Ch 4 Input Selection - CCL LUT0 Event 1 bit 0 position. */
#define EVSYS_ASYNCUSER4_1_bm  (1<<1)  /* Asynchronous User Ch 4 Input Selection - CCL LUT0 Event 1 bit 1 mask. */
#define EVSYS_ASYNCUSER4_1_bp  1  /* Asynchronous User Ch 4 Input Selection - CCL LUT0 Event 1 bit 1 position. */
#define EVSYS_ASYNCUSER4_2_bm  (1<<2)  /* Asynchronous User Ch 4 Input Selection - CCL LUT0 Event 1 bit 2 mask. */
#define EVSYS_ASYNCUSER4_2_bp  2  /* Asynchronous User Ch 4 Input Selection - CCL LUT0 Event 1 bit 2 position. */
#define EVSYS_ASYNCUSER4_3_bm  (1<<3)  /* Asynchronous User Ch 4 Input Selection - CCL LUT0 Event 1 bit 3 mask. */
#define EVSYS_ASYNCUSER4_3_bp  3  /* Asynchronous User Ch 4 Input Selection - CCL LUT0 Event 1 bit 3 position. */
#define EVSYS_ASYNCUSER4_4_bm  (1<<4)  /* Asynchronous User Ch 4 Input Selection - CCL LUT0 Event 1 bit 4 mask. */
#define EVSYS_ASYNCUSER4_4_bp  4  /* Asynchronous User Ch 4 Input Selection - CCL LUT0 Event 1 bit 4 position. */
#define EVSYS_ASYNCUSER4_5_bm  (1<<5)  /* Asynchronous User Ch 4 Input Selection - CCL LUT0 Event 1 bit 5 mask. */
#define EVSYS_ASYNCUSER4_5_bp  5  /* Asynchronous User Ch 4 Input Selection - CCL LUT0 Event 1 bit 5 position. */
#define EVSYS_ASYNCUSER4_6_bm  (1<<6)  /* Asynchronous User Ch 4 Input Selection - CCL LUT0 Event 1 bit 6 mask. */
#define EVSYS_ASYNCUSER4_6_bp  6  /* Asynchronous User Ch 4 Input Selection - CCL LUT0 Event 1 bit 6 position. */
#define EVSYS_ASYNCUSER4_7_bm  (1<<7)  /* Asynchronous User Ch 4 Input Selection - CCL LUT0 Event 1 bit 7 mask. */
#define EVSYS_ASYNCUSER4_7_bp  7  /* Asynchronous User Ch 4 Input Selection - CCL LUT0 Event 1 bit 7 position. */

/* EVSYS.ASYNCUSER5  bit masks and bit positions */
#define EVSYS_ASYNCUSER5_gm  0xFF  /* Asynchronous User Ch 5 Input Selection - CCL LUT1 Event 1 group mask. */
#define EVSYS_ASYNCUSER5_gp  0  /* Asynchronous User Ch 5 Input Selection - CCL LUT1 Event 1 group position. */
#define EVSYS_ASYNCUSER5_0_bm  (1<<0)  /* Asynchronous User Ch 5 Input Selection - CCL LUT1 Event 1 bit 0 mask. */
#define EVSYS_ASYNCUSER5_0_bp  0  /* Asynchronous User Ch 5 Input Selection - CCL LUT1 Event 1 bit 0 position. */
#define EVSYS_ASYNCUSER5_1_bm  (1<<1)  /* Asynchronous User Ch 5 Input Selection - CCL LUT1 Event 1 bit 1 mask. */
#define EVSYS_ASYNCUSER5_1_bp  1  /* Asynchronous User Ch 5 Input Selection - CCL LUT1 Event 1 bit 1 position. */
#define EVSYS_ASYNCUSER5_2_bm  (1<<2)  /* Asynchronous User Ch 5 Input Selection - CCL LUT1 Event 1 bit 2 mask. */
#define EVSYS_ASYNCUSER5_2_bp  2  /* Asynchronous User Ch 5 Input Selection - CCL LUT1 Event 1 bit 2 position. */
#define EVSYS_ASYNCUSER5_3_bm  (1<<3)  /* Asynchronous User Ch 5 Input Selection - CCL LUT1 Event 1 bit 3 mask. */
#define EVSYS_ASYNCUSER5_3_bp  3  /* Asynchronous User Ch 5 Input Selection - CCL LUT1 Event 1 bit 3 position. */
#define EVSYS_ASYNCUSER5_4_bm  (1<<4)  /* Asynchronous User Ch 5 Input Selection - CCL LUT1 Event 1 bit 4 mask. */
#define EVSYS_ASYNCUSER5_4_bp  4  /* Asynchronous User Ch 5 Input Selection - CCL LUT1 Event 1 bit 4 position. */
#define EVSYS_ASYNCUSER5_5_bm  (1<<5)  /* Asynchronous User Ch 5 Input Selection - CCL LUT1 Event 1 bit 5 mask. */
#define EVSYS_ASYNCUSER5_5_bp  5  /* Asynchronous User Ch 5 Input Selection - CCL LUT1 Event 1 bit 5 position. */
#define EVSYS_ASYNCUSER5_6_bm  (1<<6)  /* Asynchronous User Ch 5 Input Selection - CCL LUT1 Event 1 bit 6 mask. */
#define EVSYS_ASYNCUSER5_6_bp  6  /* Asynchronous User Ch 5 Input Selection - CCL LUT1 Event 1 bit 6 position. */
#define EVSYS_ASYNCUSER5_7_bm  (1<<7)  /* Asynchronous User Ch 5 Input Selection - CCL LUT1 Event 1 bit 7 mask. */
#define EVSYS_ASYNCUSER5_7_bp  7  /* Asynchronous User Ch 5 Input Selection - CCL LUT1 Event 1 bit 7 position. */

/* EVSYS.ASYNCUSER6  bit masks and bit positions */
#define EVSYS_ASYNCUSER6_gm  0xFF  /* Asynchronous User Ch 6 Input Selection - TCD0 Event 0 group mask. */
#define EVSYS_ASYNCUSER6_gp  0  /* Asynchronous User Ch 6 Input Selection - TCD0 Event 0 group position. */
#define EVSYS_ASYNCUSER6_0_bm  (1<<0)  /* Asynchronous User Ch 6 Input Selection - TCD0 Event 0 bit 0 mask. */
#define EVSYS_ASYNCUSER6_0_bp  0  /* Asynchronous User Ch 6 Input Selection - TCD0 Event 0 bit 0 position. */
#define EVSYS_ASYNCUSER6_1_bm  (1<<1)  /* Asynchronous User Ch 6 Input Selection - TCD0 Event 0 bit 1 mask. */
#define EVSYS_ASYNCUSER6_1_bp  1  /* Asynchronous User Ch 6 Input Selection - TCD0 Event 0 bit 1 position. */
#define EVSYS_ASYNCUSER6_2_bm  (1<<2)  /* Asynchronous User Ch 6 Input Selection - TCD0 Event 0 bit 2 mask. */
#define EVSYS_ASYNCUSER6_2_bp  2  /* Asynchronous User Ch 6 Input Selection - TCD0 Event 0 bit 2 position. */
#define EVSYS_ASYNCUSER6_3_bm  (1<<3)  /* Asynchronous User Ch 6 Input Selection - TCD0 Event 0 bit 3 mask. */
#define EVSYS_ASYNCUSER6_3_bp  3  /* Asynchronous User Ch 6 Input Selection - TCD0 Event 0 bit 3 position. */
#define EVSYS_ASYNCUSER6_4_bm  (1<<4)  /* Asynchronous User Ch 6 Input Selection - TCD0 Event 0 bit 4 mask. */
#define EVSYS_ASYNCUSER6_4_bp  4  /* Asynchronous User Ch 6 Input Selection - TCD0 Event 0 bit 4 position. */
#define EVSYS_ASYNCUSER6_5_bm  (1<<5)  /* Asynchronous User Ch 6 Input Selection - TCD0 Event 0 bit 5 mask. */
#define EVSYS_ASYNCUSER6_5_bp  5  /* Asynchronous User Ch 6 Input Selection - TCD0 Event 0 bit 5 position. */
#define EVSYS_ASYNCUSER6_6_bm  (1<<6)  /* Asynchronous User Ch 6 Input Selection - TCD0 Event 0 bit 6 mask. */
#define EVSYS_ASYNCUSER6_6_bp  6  /* Asynchronous User Ch 6 Input Selection - TCD0 Event 0 bit 6 position. */
#define EVSYS_ASYNCUSER6_7_bm  (1<<7)  /* Asynchronous User Ch 6 Input Selection - TCD0 Event 0 bit 7 mask. */
#define EVSYS_ASYNCUSER6_7_bp  7  /* Asynchronous User Ch 6 Input Selection - TCD0 Event 0 bit 7 position. */

/* EVSYS.ASYNCUSER7  bit masks and bit positions */
#define EVSYS_ASYNCUSER7_gm  0xFF  /* Asynchronous User Ch 7 Input Selection - TCD0 Event 1 group mask. */
#define EVSYS_ASYNCUSER7_gp  0  /* Asynchronous User Ch 7 Input Selection - TCD0 Event 1 group position. */
#define EVSYS_ASYNCUSER7_0_bm  (1<<0)  /* Asynchronous User Ch 7 Input Selection - TCD0 Event 1 bit 0 mask. */
#define EVSYS_ASYNCUSER7_0_bp  0  /* Asynchronous User Ch 7 Input Selection - TCD0 Event 1 bit 0 position. */
#define EVSYS_ASYNCUSER7_1_bm  (1<<1)  /* Asynchronous User Ch 7 Input Selection - TCD0 Event 1 bit 1 mask. */
#define EVSYS_ASYNCUSER7_1_bp  1  /* Asynchronous User Ch 7 Input Selection - TCD0 Event 1 bit 1 position. */
#define EVSYS_ASYNCUSER7_2_bm  (1<<2)  /* Asynchronous User Ch 7 Input Selection - TCD0 Event 1 bit 2 mask. */
#define EVSYS_ASYNCUSER7_2_bp  2  /* Asynchronous User Ch 7 Input Selection - TCD0 Event 1 bit 2 position. */
#define EVSYS_ASYNCUSER7_3_bm  (1<<3)  /* Asynchronous User Ch 7 Input Selection - TCD0 Event 1 bit 3 mask. */
#define EVSYS_ASYNCUSER7_3_bp  3  /* Asynchronous User Ch 7 Input Selection - TCD0 Event 1 bit 3 position. */
#define EVSYS_ASYNCUSER7_4_bm  (1<<4)  /* Asynchronous User Ch 7 Input Selection - TCD0 Event 1 bit 4 mask. */
#define EVSYS_ASYNCUSER7_4_bp  4  /* Asynchronous User Ch 7 Input Selection - TCD0 Event 1 bit 4 position. */
#define EVSYS_ASYNCUSER7_5_bm  (1<<5)  /* Asynchronous User Ch 7 Input Selection - TCD0 Event 1 bit 5 mask. */
#define EVSYS_ASYNCUSER7_5_bp  5  /* Asynchronous User Ch 7 Input Selection - TCD0 Event 1 bit 5 position. */
#define EVSYS_ASYNCUSER7_6_bm  (1<<6)  /* Asynchronous User Ch 7 Input Selection - TCD0 Event 1 bit 6 mask. */
#define EVSYS_ASYNCUSER7_6_bp  6  /* Asynchronous User Ch 7 Input Selection - TCD0 Event 1 bit 6 position. */
#define EVSYS_ASYNCUSER7_7_bm  (1<<7)  /* Asynchronous User Ch 7 Input Selection - TCD0 Event 1 bit 7 mask. */
#define EVSYS_ASYNCUSER7_7_bp  7  /* Asynchronous User Ch 7 Input Selection - TCD0 Event 1 bit 7 position. */

/* EVSYS.ASYNCUSER8  bit masks and bit positions */
#define EVSYS_ASYNCUSER8_gm  0xFF  /* Asynchronous User Ch 8 Input Selection - Event Out 0 group mask. */
#define EVSYS_ASYNCUSER8_gp  0  /* Asynchronous User Ch 8 Input Selection - Event Out 0 group position. */
#define EVSYS_ASYNCUSER8_0_bm  (1<<0)  /* Asynchronous User Ch 8 Input Selection - Event Out 0 bit 0 mask. */
#define EVSYS_ASYNCUSER8_0_bp  0  /* Asynchronous User Ch 8 Input Selection - Event Out 0 bit 0 position. */
#define EVSYS_ASYNCUSER8_1_bm  (1<<1)  /* Asynchronous User Ch 8 Input Selection - Event Out 0 bit 1 mask. */
#define EVSYS_ASYNCUSER8_1_bp  1  /* Asynchronous User Ch 8 Input Selection - Event Out 0 bit 1 position. */
#define EVSYS_ASYNCUSER8_2_bm  (1<<2)  /* Asynchronous User Ch 8 Input Selection - Event Out 0 bit 2 mask. */
#define EVSYS_ASYNCUSER8_2_bp  2  /* Asynchronous User Ch 8 Input Selection - Event Out 0 bit 2 position. */
#define EVSYS_ASYNCUSER8_3_bm  (1<<3)  /* Asynchronous User Ch 8 Input Selection - Event Out 0 bit 3 mask. */
#define EVSYS_ASYNCUSER8_3_bp  3  /* Asynchronous User Ch 8 Input Selection - Event Out 0 bit 3 position. */
#define EVSYS_ASYNCUSER8_4_bm  (1<<4)  /* Asynchronous User Ch 8 Input Selection - Event Out 0 bit 4 mask. */
#define EVSYS_ASYNCUSER8_4_bp  4  /* Asynchronous User Ch 8 Input Selection - Event Out 0 bit 4 position. */
#define EVSYS_ASYNCUSER8_5_bm  (1<<5)  /* Asynchronous User Ch 8 Input Selection - Event Out 0 bit 5 mask. */
#define EVSYS_ASYNCUSER8_5_bp  5  /* Asynchronous User Ch 8 Input Selection - Event Out 0 bit 5 position. */
#define EVSYS_ASYNCUSER8_6_bm  (1<<6)  /* Asynchronous User Ch 8 Input Selection - Event Out 0 bit 6 mask. */
#define EVSYS_ASYNCUSER8_6_bp  6  /* Asynchronous User Ch 8 Input Selection - Event Out 0 bit 6 position. */
#define EVSYS_ASYNCUSER8_7_bm  (1<<7)  /* Asynchronous User Ch 8 Input Selection - Event Out 0 bit 7 mask. */
#define EVSYS_ASYNCUSER8_7_bp  7  /* Asynchronous User Ch 8 Input Selection - Event Out 0 bit 7 position. */

/* EVSYS.ASYNCUSER9  bit masks and bit positions */
#define EVSYS_ASYNCUSER9_gm  0xFF  /* Asynchronous User Ch 9 Input Selection - Event Out 1 group mask. */
#define EVSYS_ASYNCUSER9_gp  0  /* Asynchronous User Ch 9 Input Selection - Event Out 1 group position. */
#define EVSYS_ASYNCUSER9_0_bm  (1<<0)  /* Asynchronous User Ch 9 Input Selection - Event Out 1 bit 0 mask. */
#define EVSYS_ASYNCUSER9_0_bp  0  /* Asynchronous User Ch 9 Input Selection - Event Out 1 bit 0 position. */
#define EVSYS_ASYNCUSER9_1_bm  (1<<1)  /* Asynchronous User Ch 9 Input Selection - Event Out 1 bit 1 mask. */
#define EVSYS_ASYNCUSER9_1_bp  1  /* Asynchronous User Ch 9 Input Selection - Event Out 1 bit 1 position. */
#define EVSYS_ASYNCUSER9_2_bm  (1<<2)  /* Asynchronous User Ch 9 Input Selection - Event Out 1 bit 2 mask. */
#define EVSYS_ASYNCUSER9_2_bp  2  /* Asynchronous User Ch 9 Input Selection - Event Out 1 bit 2 position. */
#define EVSYS_ASYNCUSER9_3_bm  (1<<3)  /* Asynchronous User Ch 9 Input Selection - Event Out 1 bit 3 mask. */
#define EVSYS_ASYNCUSER9_3_bp  3  /* Asynchronous User Ch 9 Input Selection - Event Out 1 bit 3 position. */
#define EVSYS_ASYNCUSER9_4_bm  (1<<4)  /* Asynchronous User Ch 9 Input Selection - Event Out 1 bit 4 mask. */
#define EVSYS_ASYNCUSER9_4_bp  4  /* Asynchronous User Ch 9 Input Selection - Event Out 1 bit 4 position. */
#define EVSYS_ASYNCUSER9_5_bm  (1<<5)  /* Asynchronous User Ch 9 Input Selection - Event Out 1 bit 5 mask. */
#define EVSYS_ASYNCUSER9_5_bp  5  /* Asynchronous User Ch 9 Input Selection - Event Out 1 bit 5 position. */
#define EVSYS_ASYNCUSER9_6_bm  (1<<6)  /* Asynchronous User Ch 9 Input Selection - Event Out 1 bit 6 mask. */
#define EVSYS_ASYNCUSER9_6_bp  6  /* Asynchronous User Ch 9 Input Selection - Event Out 1 bit 6 position. */
#define EVSYS_ASYNCUSER9_7_bm  (1<<7)  /* Asynchronous User Ch 9 Input Selection - Event Out 1 bit 7 mask. */
#define EVSYS_ASYNCUSER9_7_bp  7  /* Asynchronous User Ch 9 Input Selection - Event Out 1 bit 7 position. */

/* EVSYS.ASYNCUSER10  bit masks and bit positions */
#define EVSYS_ASYNCUSER10_gm  0xFF  /* Asynchronous User Ch 10 Input Selection - Event Out 2 group mask. */
#define EVSYS_ASYNCUSER10_gp  0  /* Asynchronous User Ch 10 Input Selection - Event Out 2 group position. */
#define EVSYS_ASYNCUSER10_0_bm  (1<<0)  /* Asynchronous User Ch 10 Input Selection - Event Out 2 bit 0 mask. */
#define EVSYS_ASYNCUSER10_0_bp  0  /* Asynchronous User Ch 10 Input Selection - Event Out 2 bit 0 position. */
#define EVSYS_ASYNCUSER10_1_bm  (1<<1)  /* Asynchronous User Ch 10 Input Selection - Event Out 2 bit 1 mask. */
#define EVSYS_ASYNCUSER10_1_bp  1  /* Asynchronous User Ch 10 Input Selection - Event Out 2 bit 1 position. */
#define EVSYS_ASYNCUSER10_2_bm  (1<<2)  /* Asynchronous User Ch 10 Input Selection - Event Out 2 bit 2 mask. */
#define EVSYS_ASYNCUSER10_2_bp  2  /* Asynchronous User Ch 10 Input Selection - Event Out 2 bit 2 position. */
#define EVSYS_ASYNCUSER10_3_bm  (1<<3)  /* Asynchronous User Ch 10 Input Selection - Event Out 2 bit 3 mask. */
#define EVSYS_ASYNCUSER10_3_bp  3  /* Asynchronous User Ch 10 Input Selection - Event Out 2 bit 3 position. */
#define EVSYS_ASYNCUSER10_4_bm  (1<<4)  /* Asynchronous User Ch 10 Input Selection - Event Out 2 bit 4 mask. */
#define EVSYS_ASYNCUSER10_4_bp  4  /* Asynchronous User Ch 10 Input Selection - Event Out 2 bit 4 position. */
#define EVSYS_ASYNCUSER10_5_bm  (1<<5)  /* Asynchronous User Ch 10 Input Selection - Event Out 2 bit 5 mask. */
#define EVSYS_ASYNCUSER10_5_bp  5  /* Asynchronous User Ch 10 Input Selection - Event Out 2 bit 5 position. */
#define EVSYS_ASYNCUSER10_6_bm  (1<<6)  /* Asynchronous User Ch 10 Input Selection - Event Out 2 bit 6 mask. */
#define EVSYS_ASYNCUSER10_6_bp  6  /* Asynchronous User Ch 10 Input Selection - Event Out 2 bit 6 position. */
#define EVSYS_ASYNCUSER10_7_bm  (1<<7)  /* Asynchronous User Ch 10 Input Selection - Event Out 2 bit 7 mask. */
#define EVSYS_ASYNCUSER10_7_bp  7  /* Asynchronous User Ch 10 Input Selection - Event Out 2 bit 7 position. */

/* EVSYS.ASYNCUSER11  bit masks and bit positions */
#define EVSYS_ASYNCUSER11_gm  0xFF  /* Asynchronous User Ch 11 Input Selection - TCB1 group mask. */
#define EVSYS_ASYNCUSER11_gp  0  /* Asynchronous User Ch 11 Input Selection - TCB1 group position. */
#define EVSYS_ASYNCUSER11_0_bm  (1<<0)  /* Asynchronous User Ch 11 Input Selection - TCB1 bit 0 mask. */
#define EVSYS_ASYNCUSER11_0_bp  0  /* Asynchronous User Ch 11 Input Selection - TCB1 bit 0 position. */
#define EVSYS_ASYNCUSER11_1_bm  (1<<1)  /* Asynchronous User Ch 11 Input Selection - TCB1 bit 1 mask. */
#define EVSYS_ASYNCUSER11_1_bp  1  /* Asynchronous User Ch 11 Input Selection - TCB1 bit 1 position. */
#define EVSYS_ASYNCUSER11_2_bm  (1<<2)  /* Asynchronous User Ch 11 Input Selection - TCB1 bit 2 mask. */
#define EVSYS_ASYNCUSER11_2_bp  2  /* Asynchronous User Ch 11 Input Selection - TCB1 bit 2 position. */
#define EVSYS_ASYNCUSER11_3_bm  (1<<3)  /* Asynchronous User Ch 11 Input Selection - TCB1 bit 3 mask. */
#define EVSYS_ASYNCUSER11_3_bp  3  /* Asynchronous User Ch 11 Input Selection - TCB1 bit 3 position. */
#define EVSYS_ASYNCUSER11_4_bm  (1<<4)  /* Asynchronous User Ch 11 Input Selection - TCB1 bit 4 mask. */
#define EVSYS_ASYNCUSER11_4_bp  4  /* Asynchronous User Ch 11 Input Selection - TCB1 bit 4 position. */
#define EVSYS_ASYNCUSER11_5_bm  (1<<5)  /* Asynchronous User Ch 11 Input Selection - TCB1 bit 5 mask. */
#define EVSYS_ASYNCUSER11_5_bp  5  /* Asynchronous User Ch 11 Input Selection - TCB1 bit 5 position. */
#define EVSYS_ASYNCUSER11_6_bm  (1<<6)  /* Asynchronous User Ch 11 Input Selection - TCB1 bit 6 mask. */
#define EVSYS_ASYNCUSER11_6_bp  6  /* Asynchronous User Ch 11 Input Selection - TCB1 bit 6 position. */
#define EVSYS_ASYNCUSER11_7_bm  (1<<7)  /* Asynchronous User Ch 11 Input Selection - TCB1 bit 7 mask. */
#define EVSYS_ASYNCUSER11_7_bp  7  /* Asynchronous User Ch 11 Input Selection - TCB1 bit 7 position. */

/* EVSYS.ASYNCUSER12  bit masks and bit positions */
#define EVSYS_ASYNCUSER12_gm  0xFF  /* Asynchronous User Ch 12 Input Selection - ADC0 group mask. */
#define EVSYS_ASYNCUSER12_gp  0  /* Asynchronous User Ch 12 Input Selection - ADC0 group position. */
#define EVSYS_ASYNCUSER12_0_bm  (1<<0)  /* Asynchronous User Ch 12 Input Selection - ADC0 bit 0 mask. */
#define EVSYS_ASYNCUSER12_0_bp  0  /* Asynchronous User Ch 12 Input Selection - ADC0 bit 0 position. */
#define EVSYS_ASYNCUSER12_1_bm  (1<<1)  /* Asynchronous User Ch 12 Input Selection - ADC0 bit 1 mask. */
#define EVSYS_ASYNCUSER12_1_bp  1  /* Asynchronous User Ch 12 Input Selection - ADC0 bit 1 position. */
#define EVSYS_ASYNCUSER12_2_bm  (1<<2)  /* Asynchronous User Ch 12 Input Selection - ADC0 bit 2 mask. */
#define EVSYS_ASYNCUSER12_2_bp  2  /* Asynchronous User Ch 12 Input Selection - ADC0 bit 2 position. */
#define EVSYS_ASYNCUSER12_3_bm  (1<<3)  /* Asynchronous User Ch 12 Input Selection - ADC0 bit 3 mask. */
#define EVSYS_ASYNCUSER12_3_bp  3  /* Asynchronous User Ch 12 Input Selection - ADC0 bit 3 position. */
#define EVSYS_ASYNCUSER12_4_bm  (1<<4)  /* Asynchronous User Ch 12 Input Selection - ADC0 bit 4 mask. */
#define EVSYS_ASYNCUSER12_4_bp  4  /* Asynchronous User Ch 12 Input Selection - ADC0 bit 4 position. */
#define EVSYS_ASYNCUSER12_5_bm  (1<<5)  /* Asynchronous User Ch 12 Input Selection - ADC0 bit 5 mask. */
#define EVSYS_ASYNCUSER12_5_bp  5  /* Asynchronous User Ch 12 Input Selection - ADC0 bit 5 position. */
#define EVSYS_ASYNCUSER12_6_bm  (1<<6)  /* Asynchronous User Ch 12 Input Selection - ADC0 bit 6 mask. */
#define EVSYS_ASYNCUSER12_6_bp  6  /* Asynchronous User Ch 12 Input Selection - ADC0 bit 6 position. */
#define EVSYS_ASYNCUSER12_7_bm  (1<<7)  /* Asynchronous User Ch 12 Input Selection - ADC0 bit 7 mask. */
#define EVSYS_ASYNCUSER12_7_bp  7  /* Asynchronous User Ch 12 Input Selection - ADC0 bit 7 position. */

/* EVSYS.SYNCUSER0  bit masks and bit positions */
#define EVSYS_SYNCUSER0_gm  0xFF  /* Synchronous User Ch 0 - TCA0 group mask. */
#define EVSYS_SYNCUSER0_gp  0  /* Synchronous User Ch 0 - TCA0 group position. */
#define EVSYS_SYNCUSER0_0_bm  (1<<0)  /* Synchronous User Ch 0 - TCA0 bit 0 mask. */
#define EVSYS_SYNCUSER0_0_bp  0  /* Synchronous User Ch 0 - TCA0 bit 0 position. */
#define EVSYS_SYNCUSER0_1_bm  (1<<1)  /* Synchronous User Ch 0 - TCA0 bit 1 mask. */
#define EVSYS_SYNCUSER0_1_bp  1  /* Synchronous User Ch 0 - TCA0 bit 1 position. */
#define EVSYS_SYNCUSER0_2_bm  (1<<2)  /* Synchronous User Ch 0 - TCA0 bit 2 mask. */
#define EVSYS_SYNCUSER0_2_bp  2  /* Synchronous User Ch 0 - TCA0 bit 2 position. */
#define EVSYS_SYNCUSER0_3_bm  (1<<3)  /* Synchronous User Ch 0 - TCA0 bit 3 mask. */
#define EVSYS_SYNCUSER0_3_bp  3  /* Synchronous User Ch 0 - TCA0 bit 3 position. */
#define EVSYS_SYNCUSER0_4_bm  (1<<4)  /* Synchronous User Ch 0 - TCA0 bit 4 mask. */
#define EVSYS_SYNCUSER0_4_bp  4  /* Synchronous User Ch 0 - TCA0 bit 4 position. */
#define EVSYS_SYNCUSER0_5_bm  (1<<5)  /* Synchronous User Ch 0 - TCA0 bit 5 mask. */
#define EVSYS_SYNCUSER0_5_bp  5  /* Synchronous User Ch 0 - TCA0 bit 5 position. */
#define EVSYS_SYNCUSER0_6_bm  (1<<6)  /* Synchronous User Ch 0 - TCA0 bit 6 mask. */
#define EVSYS_SYNCUSER0_6_bp  6  /* Synchronous User Ch 0 - TCA0 bit 6 position. */
#define EVSYS_SYNCUSER0_7_bm  (1<<7)  /* Synchronous User Ch 0 - TCA0 bit 7 mask. */
#define EVSYS_SYNCUSER0_7_bp  7  /* Synchronous User Ch 0 - TCA0 bit 7 position. */

/* EVSYS.SYNCUSER1  bit masks and bit positions */
#define EVSYS_SYNCUSER1_gm  0xFF  /* Synchronous User Ch 1 - USART0 group mask. */
#define EVSYS_SYNCUSER1_gp  0  /* Synchronous User Ch 1 - USART0 group position. */
#define EVSYS_SYNCUSER1_0_bm  (1<<0)  /* Synchronous User Ch 1 - USART0 bit 0 mask. */
#define EVSYS_SYNCUSER1_0_bp  0  /* Synchronous User Ch 1 - USART0 bit 0 position. */
#define EVSYS_SYNCUSER1_1_bm  (1<<1)  /* Synchronous User Ch 1 - USART0 bit 1 mask. */
#define EVSYS_SYNCUSER1_1_bp  1  /* Synchronous User Ch 1 - USART0 bit 1 position. */
#define EVSYS_SYNCUSER1_2_bm  (1<<2)  /* Synchronous User Ch 1 - USART0 bit 2 mask. */
#define EVSYS_SYNCUSER1_2_bp  2  /* Synchronous User Ch 1 - USART0 bit 2 position. */
#define EVSYS_SYNCUSER1_3_bm  (1<<3)  /* Synchronous User Ch 1 - USART0 bit 3 mask. */
#define EVSYS_SYNCUSER1_3_bp  3  /* Synchronous User Ch 1 - USART0 bit 3 position. */
#define EVSYS_SYNCUSER1_4_bm  (1<<4)  /* Synchronous User Ch 1 - USART0 bit 4 mask. */
#define EVSYS_SYNCUSER1_4_bp  4  /* Synchronous User Ch 1 - USART0 bit 4 position. */
#define EVSYS_SYNCUSER1_5_bm  (1<<5)  /* Synchronous User Ch 1 - USART0 bit 5 mask. */
#define EVSYS_SYNCUSER1_5_bp  5  /* Synchronous User Ch 1 - USART0 bit 5 position. */
#define EVSYS_SYNCUSER1_6_bm  (1<<6)  /* Synchronous User Ch 1 - USART0 bit 6 mask. */
#define EVSYS_SYNCUSER1_6_bp  6  /* Synchronous User Ch 1 - USART0 bit 6 position. */
#define EVSYS_SYNCUSER1_7_bm  (1<<7)  /* Synchronous User Ch 1 - USART0 bit 7 mask. */
#define EVSYS_SYNCUSER1_7_bp  7  /* Synchronous User Ch 1 - USART0 bit 7 position. */


/* FUSE - Fuses */
/* FUSE.WDTCFG  bit masks and bit positions */
#define FUSE_PERIOD_gm  0x0F  /* Watchdog Timeout Period group mask. */
#define FUSE_PERIOD_gp  0  /* Watchdog Timeout Period group position. */
#define FUSE_PERIOD_0_bm  (1<<0)  /* Watchdog Timeout Period bit 0 mask. */
#define FUSE_PERIOD_0_bp  0  /* Watchdog Timeout Period bit 0 position. */
#define FUSE_PERIOD_1_bm  (1<<1)  /* Watchdog Timeout Period bit 1 mask. */
#define FUSE_PERIOD_1_bp  1  /* Watchdog Timeout Period bit 1 position. */
#define FUSE_PERIOD_2_bm  (1<<2)  /* Watchdog Timeout Period bit 2 mask. */
#define FUSE_PERIOD_2_bp  2  /* Watchdog Timeout Period bit 2 position. */
#define FUSE_PERIOD_3_bm  (1<<3)  /* Watchdog Timeout Period bit 3 mask. */
#define FUSE_PERIOD_3_bp  3  /* Watchdog Timeout Period bit 3 position. */
#define FUSE_WINDOW_gm  0xF0  /* Watchdog Window Timeout Period group mask. */
#define FUSE_WINDOW_gp  4  /* Watchdog Window Timeout Period group position. */
#define FUSE_WINDOW_0_bm  (1<<4)  /* Watchdog Window Timeout Period bit 0 mask. */
#define FUSE_WINDOW_0_bp  4  /* Watchdog Window Timeout Period bit 0 position. */
#define FUSE_WINDOW_1_bm  (1<<5)  /* Watchdog Window Timeout Period bit 1 mask. */
#define FUSE_WINDOW_1_bp  5  /* Watchdog Window Timeout Period bit 1 position. */
#define FUSE_WINDOW_2_bm  (1<<6)  /* Watchdog Window Timeout Period bit 2 mask. */
#define FUSE_WINDOW_2_bp  6  /* Watchdog Window Timeout Period bit 2 position. */
#define FUSE_WINDOW_3_bm  (1<<7)  /* Watchdog Window Timeout Period bit 3 mask. */
#define FUSE_WINDOW_3_bp  7  /* Watchdog Window Timeout Period bit 3 position. */

/* FUSE.BODCFG  bit masks and bit positions */
#define FUSE_SLEEP_gm  0x03  /* BOD Operation in Sleep Mode group mask. */
#define FUSE_SLEEP_gp  0  /* BOD Operation in Sleep Mode group position. */
#define FUSE_SLEEP_0_bm  (1<<0)  /* BOD Operation in Sleep Mode bit 0 mask. */
#define FUSE_SLEEP_0_bp  0  /* BOD Operation in Sleep Mode bit 0 position. */
#define FUSE_SLEEP_1_bm  (1<<1)  /* BOD Operation in Sleep Mode bit 1 mask. */
#define FUSE_SLEEP_1_bp  1  /* BOD Operation in Sleep Mode bit 1 position. */
#define FUSE_ACTIVE_gm  0x0C  /* BOD Operation in Active Mode group mask. */
#define FUSE_ACTIVE_gp  2  /* BOD Operation in Active Mode group position. */
#define FUSE_ACTIVE_0_bm  (1<<2)  /* BOD Operation in Active Mode bit 0 mask. */
#define FUSE_ACTIVE_0_bp  2  /* BOD Operation in Active Mode bit 0 position. */
#define FUSE_ACTIVE_1_bm  (1<<3)  /* BOD Operation in Active Mode bit 1 mask. */
#define FUSE_ACTIVE_1_bp  3  /* BOD Operation in Active Mode bit 1 position. */
#define FUSE_SAMPFREQ_bm  0x10  /* BOD Sample Frequency bit mask. */
#define FUSE_SAMPFREQ_bp  4  /* BOD Sample Frequency bit position. */
#define FUSE_LVL_gm  0xE0  /* BOD Level group mask. */
#define FUSE_LVL_gp  5  /* BOD Level group position. */
#define FUSE_LVL_0_bm  (1<<5)  /* BOD Level bit 0 mask. */
#define FUSE_LVL_0_bp  5  /* BOD Level bit 0 position. */
#define FUSE_LVL_1_bm  (1<<6)  /* BOD Level bit 1 mask. */
#define FUSE_LVL_1_bp  6  /* BOD Level bit 1 position. */
#define FUSE_LVL_2_bm  (1<<7)  /* BOD Level bit 2 mask. */
#define FUSE_LVL_2_bp  7  /* BOD Level bit 2 position. */

/* FUSE.OSCCFG  bit masks and bit positions */
#define FUSE_FREQSEL_gm  0x03  /* Frequency Select group mask. */
#define FUSE_FREQSEL_gp  0  /* Frequency Select group position. */
#define FUSE_FREQSEL_0_bm  (1<<0)  /* Frequency Select bit 0 mask. */
#define FUSE_FREQSEL_0_bp  0  /* Frequency Select bit 0 position. */
#define FUSE_FREQSEL_1_bm  (1<<1)  /* Frequency Select bit 1 mask. */
#define FUSE_FREQSEL_1_bp  1  /* Frequency Select bit 1 position. */
#define FUSE_OSCLOCK_bm  0x80  /* Oscillator Lock bit mask. */
#define FUSE_OSCLOCK_bp  7  /* Oscillator Lock bit position. */

/* FUSE.SYSCFG0  bit masks and bit positions */
#define FUSE_EESAVE_bm  0x01  /* EEPROM Save bit mask. */
#define FUSE_EESAVE_bp  0  /* EEPROM Save bit position. */
#define FUSE_RSTPINCFG_gm  0x0C  /* Reset Pin Configuration group mask. */
#define FUSE_RSTPINCFG_gp  2  /* Reset Pin Configuration group position. */
#define FUSE_RSTPINCFG_0_bm  (1<<2)  /* Reset Pin Configuration bit 0 mask. */
#define FUSE_RSTPINCFG_0_bp  2  /* Reset Pin Configuration bit 0 position. */
#define FUSE_RSTPINCFG_1_bm  (1<<3)  /* Reset Pin Configuration bit 1 mask. */
#define FUSE_RSTPINCFG_1_bp  3  /* Reset Pin Configuration bit 1 position. */
#define FUSE_CRCSRC_gm  0xC0  /* CRC Source group mask. */
#define FUSE_CRCSRC_gp  6  /* CRC Source group position. */
#define FUSE_CRCSRC_0_bm  (1<<6)  /* CRC Source bit 0 mask. */
#define FUSE_CRCSRC_0_bp  6  /* CRC Source bit 0 position. */
#define FUSE_CRCSRC_1_bm  (1<<7)  /* CRC Source bit 1 mask. */
#define FUSE_CRCSRC_1_bp  7  /* CRC Source bit 1 position. */

/* FUSE.SYSCFG1  bit masks and bit positions */
#define FUSE_SUT_gm  0x07  /* Startup Time group mask. */
#define FUSE_SUT_gp  0  /* Startup Time group position. */
#define FUSE_SUT_0_bm  (1<<0)  /* Startup Time bit 0 mask. */
#define FUSE_SUT_0_bp  0  /* Startup Time bit 0 position. */
#define FUSE_SUT_1_bm  (1<<1)  /* Startup Time bit 1 mask. */
#define FUSE_SUT_1_bp  1  /* Startup Time bit 1 position. */
#define FUSE_SUT_2_bm  (1<<2)  /* Startup Time bit 2 mask. */
#define FUSE_SUT_2_bp  2  /* Startup Time bit 2 position. */



/* LOCKBIT - Lockbit */
/* LOCKBIT.LOCKBIT  bit masks and bit positions */
#define LOCKBIT_LB_gm  0xFF  /* Lock Bits group mask. */
#define LOCKBIT_LB_gp  0  /* Lock Bits group position. */
#define LOCKBIT_LB_0_bm  (1<<0)  /* Lock Bits bit 0 mask. */
#define LOCKBIT_LB_0_bp  0  /* Lock Bits bit 0 position. */
#define LOCKBIT_LB_1_bm  (1<<1)  /* Lock Bits bit 1 mask. */
#define LOCKBIT_LB_1_bp  1  /* Lock Bits bit 1 position. */
#define LOCKBIT_LB_2_bm  (1<<2)  /* Lock Bits bit 2 mask. */
#define LOCKBIT_LB_2_bp  2  /* Lock Bits bit 2 position. */
#define LOCKBIT_LB_3_bm  (1<<3)  /* Lock Bits bit 3 mask. */
#define LOCKBIT_LB_3_bp  3  /* Lock Bits bit 3 position. */
#define LOCKBIT_LB_4_bm  (1<<4)  /* Lock Bits bit 4 mask. */
#define LOCKBIT_LB_4_bp  4  /* Lock Bits bit 4 position. */
#define LOCKBIT_LB_5_bm  (1<<5)  /* Lock Bits bit 5 mask. */
#define LOCKBIT_LB_5_bp  5  /* Lock Bits bit 5 position. */
#define LOCKBIT_LB_6_bm  (1<<6)  /* Lock Bits bit 6 mask. */
#define LOCKBIT_LB_6_bp  6  /* Lock Bits bit 6 position. */
#define LOCKBIT_LB_7_bm  (1<<7)  /* Lock Bits bit 7 mask. */
#define LOCKBIT_LB_7_bp  7  /* Lock Bits bit 7 position. */


/* NVMCTRL - Non-volatile Memory Controller */
/* NVMCTRL.CTRLA  bit masks and bit positions */
#define NVMCTRL_CMD_gm  0x07  /* Command group mask. */
#define NVMCTRL_CMD_gp  0  /* Command group position. */
#define NVMCTRL_CMD_0_bm  (1<<0)  /* Command bit 0 mask. */
#define NVMCTRL_CMD_0_bp  0  /* Command bit 0 position. */
#define NVMCTRL_CMD_1_bm  (1<<1)  /* Command bit 1 mask. */
#define NVMCTRL_CMD_1_bp  1  /* Command bit 1 position. */
#define NVMCTRL_CMD_2_bm  (1<<2)  /* Command bit 2 mask. */
#define NVMCTRL_CMD_2_bp  2  /* Command bit 2 position. */

/* NVMCTRL.CTRLB  bit masks and bit positions */
#define NVMCTRL_APCWP_bm  0x01  /* Application code write protect bit mask. */
#define NVMCTRL_APCWP_bp  0  /* Application code write protect bit position. */
#define NVMCTRL_BOOTLOCK_bm  0x02  /* Boot Lock bit mask. */
#define NVMCTRL_BOOTLOCK_bp  1  /* Boot Lock bit position. */

/* NVMCTRL.STATUS  bit masks and bit positions */
#define NVMCTRL_FBUSY_bm  0x01  /* Flash busy bit mask. */
#define NVMCTRL_FBUSY_bp  0  /* Flash busy bit position. */
#define NVMCTRL_EEBUSY_bm  0x02  /* EEPROM busy bit mask. */
#define NVMCTRL_EEBUSY_bp  1  /* EEPROM busy bit position. */
#define NVMCTRL_WRERROR_bm  0x04  /* Write error bit mask. */
#define NVMCTRL_WRERROR_bp  2  /* Write error bit position. */

/* NVMCTRL.INTCTRL  bit masks and bit positions */
#define NVMCTRL_EEREADY_bm  0x01  /* EEPROM Ready bit mask. */
#define NVMCTRL_EEREADY_bp  0  /* EEPROM Ready bit position. */

/* NVMCTRL.INTFLAGS  bit masks and bit positions */
/* NVMCTRL_EEREADY  is already defined. */


/* PORT - I/O Ports */
/* PORT.INTFLAGS  bit masks and bit positions */
#define PORT_INT_gm  0xFF  /* Pin Interrupt group mask. */
#define PORT_INT_gp  0  /* Pin Interrupt group position. */
#define PORT_INT_0_bm  (1<<0)  /* Pin Interrupt bit 0 mask. */
#define PORT_INT_0_bp  0  /* Pin Interrupt bit 0 position. */
#define PORT_INT0_bm  PORT_INT_0_bm  /* This define is deprecated and should not be used */
#define PORT_INT0_bp  PORT_INT_0_bp  /* This define is deprecated and should not be used */
#define PORT_INT_1_bm  (1<<1)  /* Pin Interrupt bit 1 mask. */
#define PORT_INT_1_bp  1  /* Pin Interrupt bit 1 position. */
#define PORT_INT1_bm  PORT_INT_1_bm  /* This define is deprecated and should not be used */
#define PORT_INT1_bp  PORT_INT_1_bp  /* This define is deprecated and should not be used */
#define PORT_INT_2_bm  (1<<2)  /* Pin Interrupt bit 2 mask. */
#define PORT_INT_2_bp  2  /* Pin Interrupt bit 2 position. */
#define PORT_INT2_bm  PORT_INT_2_bm  /* This define is deprecated and should not be used */
#define PORT_INT2_bp  PORT_INT_2_bp  /* This define is deprecated and should not be used */
#define PORT_INT_3_bm  (1<<3)  /* Pin Interrupt bit 3 mask. */
#define PORT_INT_3_bp  3  /* Pin Interrupt bit 3 position. */
#define PORT_INT3_bm  PORT_INT_3_bm  /* This define is deprecated and should not be used */
#define PORT_INT3_bp  PORT_INT_3_bp  /* This define is deprecated and should not be used */
#define PORT_INT_4_bm  (1<<4)  /* Pin Interrupt bit 4 mask. */
#define PORT_INT_4_bp  4  /* Pin Interrupt bit 4 position. */
#define PORT_INT4_bm  PORT_INT_4_bm  /* This define is deprecated and should not be used */
#define PORT_INT4_bp  PORT_INT_4_bp  /* This define is deprecated and should not be used */
#define PORT_INT_5_bm  (1<<5)  /* Pin Interrupt bit 5 mask. */
#define PORT_INT_5_bp  5  /* Pin Interrupt bit 5 position. */
#define PORT_INT5_bm  PORT_INT_5_bm  /* This define is deprecated and should not be used */
#define PORT_INT5_bp  PORT_INT_5_bp  /* This define is deprecated and should not be used */
#define PORT_INT_6_bm  (1<<6)  /* Pin Interrupt bit 6 mask. */
#define PORT_INT_6_bp  6  /* Pin Interrupt bit 6 position. */
#define PORT_INT6_bm  PORT_INT_6_bm  /* This define is deprecated and should not be used */
#define PORT_INT6_bp  PORT_INT_6_bp  /* This define is deprecated and should not be used */
#define PORT_INT_7_bm  (1<<7)  /* Pin Interrupt bit 7 mask. */
#define PORT_INT_7_bp  7  /* Pin Interrupt bit 7 position. */
#define PORT_INT7_bm  PORT_INT_7_bm  /* This define is deprecated and should not be used */
#define PORT_INT7_bp  PORT_INT_7_bp  /* This define is deprecated and should not be used */

/* PORT.PIN0CTRL  bit masks and bit positions */
#define PORT_ISC_gm  0x07  /* Input/Sense Configuration group mask. */
#define PORT_ISC_gp  0  /* Input/Sense Configuration group position. */
#define PORT_ISC_0_bm  (1<<0)  /* Input/Sense Configuration bit 0 mask. */
#define PORT_ISC_0_bp  0  /* Input/Sense Configuration bit 0 position. */
#define PORT_ISC_1_bm  (1<<1)  /* Input/Sense Configuration bit 1 mask. */
#define PORT_ISC_1_bp  1  /* Input/Sense Configuration bit 1 position. */
#define PORT_ISC_2_bm  (1<<2)  /* Input/Sense Configuration bit 2 mask. */
#define PORT_ISC_2_bp  2  /* Input/Sense Configuration bit 2 position. */
#define PORT_PULLUPEN_bm  0x08  /* Pullup enable bit mask. */
#define PORT_PULLUPEN_bp  3  /* Pullup enable bit position. */
#define PORT_INVEN_bm  0x80  /* Inverted I/O Enable bit mask. */
#define PORT_INVEN_bp  7  /* Inverted I/O Enable bit position. */

/* PORT.PIN1CTRL  bit masks and bit positions */
/* PORT_ISC  is already defined. */
/* PORT_PULLUPEN  is already defined. */
/* PORT_INVEN  is already defined. */

/* PORT.PIN2CTRL  bit masks and bit positions */
/* PORT_ISC  is already defined. */
/* PORT_PULLUPEN  is already defined. */
/* PORT_INVEN  is already defined. */

/* PORT.PIN3CTRL  bit masks and bit positions */
/* PORT_ISC  is already defined. */
/* PORT_PULLUPEN  is already defined. */
/* PORT_INVEN  is already defined. */

/* PORT.PIN4CTRL  bit masks and bit positions */
/* PORT_ISC  is already defined. */
/* PORT_PULLUPEN  is already defined. */
/* PORT_INVEN  is already defined. */

/* PORT.PIN5CTRL  bit masks and bit positions */
/* PORT_ISC  is already defined. */
/* PORT_PULLUPEN  is already defined. */
/* PORT_INVEN  is already defined. */

/* PORT.PIN6CTRL  bit masks and bit positions */
/* PORT_ISC  is already defined. */
/* PORT_PULLUPEN  is already defined. */
/* PORT_INVEN  is already defined. */

/* PORT.PIN7CTRL  bit masks and bit positions */
/* PORT_ISC  is already defined. */
/* PORT_PULLUPEN  is already defined. */
/* PORT_INVEN  is already defined. */


/* PORTMUX - Port Multiplexer */
/* PORTMUX.CTRLA  bit masks and bit positions */
#define PORTMUX_EVOUT0_bm  0x01  /* Event Output 0 bit mask. */
#define PORTMUX_EVOUT0_bp  0  /* Event Output 0 bit position. */
#define PORTMUX_EVOUT1_bm  0x02  /* Event Output 1 bit mask. */
#define PORTMUX_EVOUT1_bp  1  /* Event Output 1 bit position. */
#define PORTMUX_EVOUT2_bm  0x04  /* Event Output 2 bit mask. */
#define PORTMUX_EVOUT2_bp  2  /* Event Output 2 bit position. */
#define PORTMUX_LUT0_bm  0x10  /* Configurable Custom Logic LUT0 bit mask. */
#define PORTMUX_LUT0_bp  4  /* Configurable Custom Logic LUT0 bit position. */
#define PORTMUX_LUT1_bm  0x20  /* Configurable Custom Logic LUT1 bit mask. */
#define PORTMUX_LUT1_bp  5  /* Configurable Custom Logic LUT1 bit position. */

/* PORTMUX.CTRLB  bit masks and bit positions */
#define PORTMUX_USART0_bm  0x01  /* Port Multiplexer USART0 bit mask. */
#define PORTMUX_USART0_bp  0  /* Port Multiplexer USART0 bit position. */
#define PORTMUX_SPI0_bm  0x04  /* Port Multiplexer SPI0 bit mask. */
#define PORTMUX_SPI0_bp  2  /* Port Multiplexer SPI0 bit position. */

/* PORTMUX.CTRLC  bit masks and bit positions */
#define PORTMUX_TCA00_bm  0x01  /* Port Multiplexer TCA0 Output 0 bit mask. */
#define PORTMUX_TCA00_bp  0  /* Port Multiplexer TCA0 Output 0 bit position. */
#define PORTMUX_TCA01_bm  0x02  /* Port Multiplexer TCA0 Output 1 bit mask. */
#define PORTMUX_TCA01_bp  1  /* Port Multiplexer TCA0 Output 1 bit position. */
#define PORTMUX_TCA02_bm  0x04  /* Port Multiplexer TCA0 Output 2 bit mask. */
#define PORTMUX_TCA02_bp  2  /* Port Multiplexer TCA0 Output 2 bit position. */
#define PORTMUX_TCA03_bm  0x08  /* Port Multiplexer TCA0 Output 3 bit mask. */
#define PORTMUX_TCA03_bp  3  /* Port Multiplexer TCA0 Output 3 bit position. */
#define PORTMUX_TCA04_bm  0x10  /* Port Multiplexer TCA0 Output 4 bit mask. */
#define PORTMUX_TCA04_bp  4  /* Port Multiplexer TCA0 Output 4 bit position. */
#define PORTMUX_TCA05_bm  0x20  /* Port Multiplexer TCA0 Output 5 bit mask. */
#define PORTMUX_TCA05_bp  5  /* Port Multiplexer TCA0 Output 5 bit position. */

/* PORTMUX.CTRLD  bit masks and bit positions */
#define PORTMUX_TCB0_bm  0x01  /* Port Multiplexer TCB bit mask. */
#define PORTMUX_TCB0_bp  0  /* Port Multiplexer TCB bit position. */


/* RSTCTRL - Reset controller */
/* RSTCTRL.RSTFR  bit masks and bit positions */
#define RSTCTRL_PORF_bm  0x01  /* Power on Reset flag bit mask. */
#define RSTCTRL_PORF_bp  0  /* Power on Reset flag bit position. */
#define RSTCTRL_BORF_bm  0x02  /* Brown out detector Reset flag bit mask. */
#define RSTCTRL_BORF_bp  1  /* Brown out detector Reset flag bit position. */
#define RSTCTRL_EXTRF_bm  0x04  /* External Reset flag bit mask. */
#define RSTCTRL_EXTRF_bp  2  /* External Reset flag bit position. */
#define RSTCTRL_WDRF_bm  0x08  /* Watch dog Reset flag bit mask. */
#define RSTCTRL_WDRF_bp  3  /* Watch dog Reset flag bit position. */
#define RSTCTRL_SWRF_bm  0x10  /* Software Reset flag bit mask. */
#define RSTCTRL_SWRF_bp  4  /* Software Reset flag bit position. */
#define RSTCTRL_UPDIRF_bm  0x20  /* UPDI Reset flag bit mask. */
#define RSTCTRL_UPDIRF_bp  5  /* UPDI Reset flag bit position. */

/* RSTCTRL.SWRR  bit masks and bit positions */
#define RSTCTRL_SWRE_bm  0x01  /* Software reset enable bit mask. */
#define RSTCTRL_SWRE_bp  0  /* Software reset enable bit position. */


/* RTC - Real-Time Counter */
/* RTC.CTRLA  bit masks and bit positions */
#define RTC_RTCEN_bm  0x01  /* Enable bit mask. */
#define RTC_RTCEN_bp  0  /* Enable bit position. */
#define RTC_PRESCALER_gm  0x78  /* Prescaling Factor group mask. */
#define RTC_PRESCALER_gp  3  /* Prescaling Factor group position. */
#define RTC_PRESCALER_0_bm  (1<<3)  /* Prescaling Factor bit 0 mask. */
#define RTC_PRESCALER_0_bp  3  /* Prescaling Factor bit 0 position. */
#define RTC_PRESCALER_1_bm  (1<<4)  /* Prescaling Factor bit 1 mask. */
#define RTC_PRESCALER_1_bp  4  /* Prescaling Factor bit 1 position. */
#define RTC_PRESCALER_2_bm  (1<<5)  /* Prescaling Factor bit 2 mask. */
#define RTC_PRESCALER_2_bp  5  /* Prescaling Factor bit 2 position. */
#define RTC_PRESCALER_3_bm  (1<<6)  /* Prescaling Factor bit 3 mask. */
#define RTC_PRESCALER_3_bp  6  /* Prescaling Factor bit 3 position. */
#define RTC_RUNSTDBY_bm  0x80  /* Run In Standby bit mask. */
#define RTC_RUNSTDBY_bp  7  /* Run In Standby bit position. */

/* RTC.STATUS  bit masks and bit positions */
#define RTC_CTRLABUSY_bm  0x01  /* CTRLA Synchronization Busy Flag bit mask. */
#define RTC_CTRLABUSY_bp  0  /* CTRLA Synchronization Busy Flag bit position. */
#define RTC_CNTBUSY_bm  0x02  /* Count Synchronization Busy Flag bit mask. */
#define RTC_CNTBUSY_bp  1  /* Count Synchronization Busy Flag bit position. */
#define RTC_PERBUSY_bm  0x04  /* Period Synchronization Busy Flag bit mask. */
#define RTC_PERBUSY_bp  2  /* Period Synchronization Busy Flag bit position. */
#define RTC_CMPBUSY_bm  0x08  /* Comparator Synchronization Busy Flag bit mask. */
#define RTC_CMPBUSY_bp  3  /* Comparator Synchronization Busy Flag bit position. */

/* RTC.INTCTRL  bit masks and bit positions */
#define RTC_OVF_bm  0x01  /* Overflow Interrupt enable bit mask. */
#define RTC_OVF_bp  0  /* Overflow Interrupt enable bit position. */
#define RTC_CMP_bm  0x02  /* Compare Match Interrupt enable bit mask. */
#define RTC_CMP_bp  1  /* Compare Match Interrupt enable bit position. */

/* RTC.INTFLAGS  bit masks and bit positions */
/* RTC_OVF  is already defined. */
/* RTC_CMP  is already defined. */

/* RTC.DBGCTRL  bit masks and bit positions */
#define RTC_DBGRUN_bm  0x01  /* Run in debug bit mask. */
#define RTC_DBGRUN_bp  0  /* Run in debug bit position. */

/* RTC.CLKSEL  bit masks and bit positions */
#define RTC_CLKSEL_gm  0x03  /* Clock Select group mask. */
#define RTC_CLKSEL_gp  0  /* Clock Select group position. */
#define RTC_CLKSEL_0_bm  (1<<0)  /* Clock Select bit 0 mask. */
#define RTC_CLKSEL_0_bp  0  /* Clock Select bit 0 position. */
#define RTC_CLKSEL_1_bm  (1<<1)  /* Clock Select bit 1 mask. */
#define RTC_CLKSEL_1_bp  1  /* Clock Select bit 1 position. */

/* RTC.PITCTRLA  bit masks and bit positions */
#define RTC_PITEN_bm  0x01  /* Enable bit mask. */
#define RTC_PITEN_bp  0  /* Enable bit position. */
#define RTC_PERIOD_gm  0x78  /* Period group mask. */
#define RTC_PERIOD_gp  3  /* Period group position. */
#define RTC_PERIOD_0_bm  (1<<3)  /* Period bit 0 mask. */
#define RTC_PERIOD_0_bp  3  /* Period bit 0 position. */
#define RTC_PERIOD_1_bm  (1<<4)  /* Period bit 1 mask. */
#define RTC_PERIOD_1_bp  4  /* Period bit 1 position. */
#define RTC_PERIOD_2_bm  (1<<5)  /* Period bit 2 mask. */
#define RTC_PERIOD_2_bp  5  /* Period bit 2 position. */
#define RTC_PERIOD_3_bm  (1<<6)  /* Period bit 3 mask. */
#define RTC_PERIOD_3_bp  6  /* Period bit 3 position. */

/* RTC.PITSTATUS  bit masks and bit positions */
#define RTC_CTRLBUSY_bm  0x01  /* CTRLA Synchronization Busy Flag bit mask. */
#define RTC_CTRLBUSY_bp  0  /* CTRLA Synchronization Busy Flag bit position. */

/* RTC.PITINTCTRL  bit masks and bit positions */
#define RTC_PI_bm  0x01  /* Periodic Interrupt bit mask. */
#define RTC_PI_bp  0  /* Periodic Interrupt bit position. */

/* RTC.PITINTFLAGS  bit masks and bit positions */
/* RTC_PI  is already defined. */

/* RTC.PITDBGCTRL  bit masks and bit positions */
/* RTC_DBGRUN  is already defined. */



/* SLPCTRL - Sleep Controller */
/* SLPCTRL.CTRLA  bit masks and bit positions */
#define SLPCTRL_SEN_bm  0x01  /* Sleep enable bit mask. */
#define SLPCTRL_SEN_bp  0  /* Sleep enable bit position. */
#define SLPCTRL_SMODE_gm  0x06  /* Sleep mode group mask. */
#define SLPCTRL_SMODE_gp  1  /* Sleep mode group position. */
#define SLPCTRL_SMODE_0_bm  (1<<1)  /* Sleep mode bit 0 mask. */
#define SLPCTRL_SMODE_0_bp  1  /* Sleep mode bit 0 position. */
#define SLPCTRL_SMODE_1_bm  (1<<2)  /* Sleep mode bit 1 mask. */
#define SLPCTRL_SMODE_1_bp  2  /* Sleep mode bit 1 position. */


/* SPI - Serial Peripheral Interface */
/* SPI.CTRLA  bit masks and bit positions */
#define SPI_ENABLE_bm  0x01  /* Enable Module bit mask. */
#define SPI_ENABLE_bp  0  /* Enable Module bit position. */
#define SPI_PRESC_gm  0x06  /* Prescaler group mask. */
#define SPI_PRESC_gp  1  /* Prescaler group position. */
#define SPI_PRESC_0_bm  (1<<1)  /* Prescaler bit 0 mask. */
#define SPI_PRESC_0_bp  1  /* Prescaler bit 0 position. */
#define SPI_PRESC_1_bm  (1<<2)  /* Prescaler bit 1 mask. */
#define SPI_PRESC_1_bp  2  /* Prescaler bit 1 position. */
#define SPI_CLK2X_bm  0x10  /* Enable Double Speed bit mask. */
#define SPI_CLK2X_bp  4  /* Enable Double Speed bit position. */
#define SPI_MASTER_bm  0x20  /* Host Operation Enable bit mask. */
#define SPI_MASTER_bp  5  /* Host Operation Enable bit position. */
#define SPI_DORD_bm  0x40  /* Data Order Setting bit mask. */
#define SPI_DORD_bp  6  /* Data Order Setting bit position. */

/* SPI.CTRLB  bit masks and bit positions */
#define SPI_MODE_gm  0x03  /* SPI Mode group mask. */
#define SPI_MODE_gp  0  /* SPI Mode group position. */
#define SPI_MODE_0_bm  (1<<0)  /* SPI Mode bit 0 mask. */
#define SPI_MODE_0_bp  0  /* SPI Mode bit 0 position. */
#define SPI_MODE_1_bm  (1<<1)  /* SPI Mode bit 1 mask. */
#define SPI_MODE_1_bp  1  /* SPI Mode bit 1 position. */
#define SPI_SSD_bm  0x04  /* Client Select Disable bit mask. */
#define SPI_SSD_bp  2  /* Client Select Disable bit position. */
#define SPI_BUFWR_bm  0x40  /* Buffer Write Mode bit mask. */
#define SPI_BUFWR_bp  6  /* Buffer Write Mode bit position. */
#define SPI_BUFEN_bm  0x80  /* Buffer Mode Enable bit mask. */
#define SPI_BUFEN_bp  7  /* Buffer Mode Enable bit position. */

/* SPI.INTCTRL  bit masks and bit positions */
#define SPI_IE_bm  0x01  /* Interrupt Enable bit mask. */
#define SPI_IE_bp  0  /* Interrupt Enable bit position. */
#define SPI_SSIE_bm  0x10  /* Client Select Trigger Interrupt Enable bit mask. */
#define SPI_SSIE_bp  4  /* Client Select Trigger Interrupt Enable bit position. */
#define SPI_DREIE_bm  0x20  /* Data Register Empty Interrupt Enable bit mask. */
#define SPI_DREIE_bp  5  /* Data Register Empty Interrupt Enable bit position. */
#define SPI_TXCIE_bm  0x40  /* Transfer Complete Interrupt Enable bit mask. */
#define SPI_TXCIE_bp  6  /* Transfer Complete Interrupt Enable bit position. */
#define SPI_RXCIE_bm  0x80  /* Receive Complete Interrupt Enable bit mask. */
#define SPI_RXCIE_bp  7  /* Receive Complete Interrupt Enable bit position. */

/* SPI.INTFLAGS  bit masks and bit positions */
#define SPI_BUFOVF_bm  0x01  /* Buffer Overflow bit mask. */
#define SPI_BUFOVF_bp  0  /* Buffer Overflow bit position. */
#define SPI_SSIF_bm  0x10  /* Client Select Trigger Interrupt Flag bit mask. */
#define SPI_SSIF_bp  4  /* Client Select Trigger Interrupt Flag bit position. */
#define SPI_DREIF_bm  0x20  /* Data Register Empty Interrupt Flag bit mask. */
#define SPI_DREIF_bp  5  /* Data Register Empty Interrupt Flag bit position. */
#define SPI_TXCIF_bm  0x40  /* Transfer Complete Interrupt Flag bit mask. */
#define SPI_TXCIF_bp  6  /* Transfer Complete Interrupt Flag bit position. */
#define SPI_WRCOL_bm  0x40  /* Write Collision bit mask. */
#define SPI_WRCOL_bp  6  /* Write Collision bit position. */
#define SPI_RXCIF_bm  0x80  /* Receive Complete Interrupt Flag bit mask. */
#define SPI_RXCIF_bp  7  /* Receive Complete Interrupt Flag bit position. */
#define SPI_IF_bm  0x80  /* Interrupt Flag bit mask. */
#define SPI_IF_bp  7  /* Interrupt Flag bit position. */


/* SYSCFG - System Configuration Registers */
/* SYSCFG.EXTBRK  bit masks and bit positions */
#define SYSCFG_ENEXTBRK_bm  0x01  /* External break enable bit mask. */
#define SYSCFG_ENEXTBRK_bp  0  /* External break enable bit position. */


/* TCA - 16-bit Timer/Counter Type A */
/* TCA_SINGLE.CTRLA  bit masks and bit positions */
#define TCA_SINGLE_ENABLE_bm  0x01  /* Module Enable bit mask. */
#define TCA_SINGLE_ENABLE_bp  0  /* Module Enable bit position. */
#define TCA_SINGLE_CLKSEL_gm  0x0E  /* Clock Selection group mask. */
#define TCA_SINGLE_CLKSEL_gp  1  /* Clock Selection group position. */
#define TCA_SINGLE_CLKSEL_0_bm  (1<<1)  /* Clock Selection bit 0 mask. */
#define TCA_SINGLE_CLKSEL_0_bp  1  /* Clock Selection bit 0 position. */
#define TCA_SINGLE_CLKSEL_1_bm  (1<<2)  /* Clock Selection bit 1 mask. */
#define TCA_SINGLE_CLKSEL_1_bp  2  /* Clock Selection bit 1 position. */
#define TCA_SINGLE_CLKSEL_2_bm  (1<<3)  /* Clock Selection bit 2 mask. */
#define TCA_SINGLE_CLKSEL_2_bp  3  /* Clock Selection bit 2 position. */

/* TCA_SINGLE.CTRLB  bit masks and bit positions */
#define TCA_SINGLE_WGMODE_gm  0x07  /* Waveform generation mode group mask. */
#define TCA_SINGLE_WGMODE_gp  0  /* Waveform generation mode group position. */
#define TCA_SINGLE_WGMODE_0_bm  (1<<0)  /* Waveform generation mode bit 0 mask. */
#define TCA_SINGLE_WGMODE_0_bp  0  /* Waveform generation mode bit 0 position. */
#define TCA_SINGLE_WGMODE_1_bm  (1<<1)  /* Waveform generation mode bit 1 mask. */
#define TCA_SINGLE_WGMODE_1_bp  1  /* Waveform generation mode bit 1 position. */
#define TCA_SINGLE_WGMODE_2_bm  (1<<2)  /* Waveform generation mode bit 2 mask. */
#define TCA_SINGLE_WGMODE_2_bp  2  /* Waveform generation mode bit 2 position. */
#define TCA_SINGLE_ALUPD_bm  0x08  /* Auto Lock Update bit mask. */
#define TCA_SINGLE_ALUPD_bp  3  /* Auto Lock Update bit position. */
#define TCA_SINGLE_CMP0EN_bm  0x10  /* Compare 0 Enable bit mask. */
#define TCA_SINGLE_CMP0EN_bp  4  /* Compare 0 Enable bit position. */
#define TCA_SINGLE_CMP1EN_bm  0x20  /* Compare 1 Enable bit mask. */
#define TCA_SINGLE_CMP1EN_bp  5  /* Compare 1 Enable bit position. */
#define TCA_SINGLE_CMP2EN_bm  0x40  /* Compare 2 Enable bit mask. */
#define TCA_SINGLE_CMP2EN_bp  6  /* Compare 2 Enable bit position. */

/* TCA_SINGLE.CTRLC  bit masks and bit positions */
#define TCA_SINGLE_CMP0OV_bm  0x01  /* Compare 0 Waveform Output Value bit mask. */
#define TCA_SINGLE_CMP0OV_bp  0  /* Compare 0 Waveform Output Value bit position. */
#define TCA_SINGLE_CMP1OV_bm  0x02  /* Compare 1 Waveform Output Value bit mask. */
#define TCA_SINGLE_CMP1OV_bp  1  /* Compare 1 Waveform Output Value bit position. */
#define TCA_SINGLE_CMP2OV_bm  0x04  /* Compare 2 Waveform Output Value bit mask. */
#define TCA_SINGLE_CMP2OV_bp  2  /* Compare 2 Waveform Output Value bit position. */

/* TCA_SINGLE.CTRLD  bit masks and bit positions */
#define TCA_SINGLE_SPLITM_bm  0x01  /* Split Mode Enable bit mask. */
#define TCA_SINGLE_SPLITM_bp  0  /* Split Mode Enable bit position. */

/* TCA_SINGLE.CTRLECLR  bit masks and bit positions */
#define TCA_SINGLE_DIR_bm  0x01  /* Direction bit mask. */
#define TCA_SINGLE_DIR_bp  0  /* Direction bit position. */
#define TCA_SINGLE_LUPD_bm  0x02  /* Lock Update bit mask. */
#define TCA_SINGLE_LUPD_bp  1  /* Lock Update bit position. */
#define TCA_SINGLE_CMD_gm  0x0C  /* Command group mask. */
#define TCA_SINGLE_CMD_gp  2  /* Command group position. */
#define TCA_SINGLE_CMD_0_bm  (1<<2)  /* Command bit 0 mask. */
#define TCA_SINGLE_CMD_0_bp  2  /* Command bit 0 position. */
#define TCA_SINGLE_CMD_1_bm  (1<<3)  /* Command bit 1 mask. */
#define TCA_SINGLE_CMD_1_bp  3  /* Command bit 1 position. */

/* TCA_SINGLE.CTRLESET  bit masks and bit positions */
/* TCA_SINGLE_DIR  is already defined. */
/* TCA_SINGLE_LUPD  is already defined. */
/* TCA_SINGLE_CMD  is already defined. */

/* TCA_SINGLE.CTRLFCLR  bit masks and bit positions */
#define TCA_SINGLE_PERBV_bm  0x01  /* Period Buffer Valid bit mask. */
#define TCA_SINGLE_PERBV_bp  0  /* Period Buffer Valid bit position. */
#define TCA_SINGLE_CMP0BV_bm  0x02  /* Compare 0 Buffer Valid bit mask. */
#define TCA_SINGLE_CMP0BV_bp  1  /* Compare 0 Buffer Valid bit position. */
#define TCA_SINGLE_CMP1BV_bm  0x04  /* Compare 1 Buffer Valid bit mask. */
#define TCA_SINGLE_CMP1BV_bp  2  /* Compare 1 Buffer Valid bit position. */
#define TCA_SINGLE_CMP2BV_bm  0x08  /* Compare 2 Buffer Valid bit mask. */
#define TCA_SINGLE_CMP2BV_bp  3  /* Compare 2 Buffer Valid bit position. */

/* TCA_SINGLE.CTRLFSET  bit masks and bit positions */
/* TCA_SINGLE_PERBV  is already defined. */
/* TCA_SINGLE_CMP0BV  is already defined. */
/* TCA_SINGLE_CMP1BV  is already defined. */
/* TCA_SINGLE_CMP2BV  is already defined. */

/* TCA_SINGLE.EVCTRL  bit masks and bit positions */
#define TCA_SINGLE_CNTEI_bm  0x01  /* Count on Event Input bit mask. */
#define TCA_SINGLE_CNTEI_bp  0  /* Count on Event Input bit position. */
#define TCA_SINGLE_EVACT_gm  0x06  /* Event Action group mask. */
#define TCA_SINGLE_EVACT_gp  1  /* Event Action group position. */
#define TCA_SINGLE_EVACT_0_bm  (1<<1)  /* Event Action bit 0 mask. */
#define TCA_SINGLE_EVACT_0_bp  1  /* Event Action bit 0 position. */
#define TCA_SINGLE_EVACT_1_bm  (1<<2)  /* Event Action bit 1 mask. */
#define TCA_SINGLE_EVACT_1_bp  2  /* Event Action bit 1 position. */

/* TCA_SINGLE.INTCTRL  bit masks and bit positions */
#define TCA_SINGLE_OVF_bm  0x01  /* Overflow Interrupt bit mask. */
#define TCA_SINGLE_OVF_bp  0  /* Overflow Interrupt bit position. */
#define TCA_SINGLE_CMP0_bm  0x10  /* Compare 0 Interrupt bit mask. */
#define TCA_SINGLE_CMP0_bp  4  /* Compare 0 Interrupt bit position. */
#define TCA_SINGLE_CMP1_bm  0x20  /* Compare 1 Interrupt bit mask. */
#define TCA_SINGLE_CMP1_bp  5  /* Compare 1 Interrupt bit position. */
#define TCA_SINGLE_CMP2_bm  0x40  /* Compare 2 Interrupt bit mask. */
#define TCA_SINGLE_CMP2_bp  6  /* Compare 2 Interrupt bit position. */

/* TCA_SINGLE.INTFLAGS  bit masks and bit positions */
/* TCA_SINGLE_OVF  is already defined. */
/* TCA_SINGLE_CMP0  is already defined. */
/* TCA_SINGLE_CMP1  is already defined. */
/* TCA_SINGLE_CMP2  is already defined. */

/* TCA_SINGLE.DBGCTRL  bit masks and bit positions */
#define TCA_SINGLE_DBGRUN_bm  0x01  /* Debug Run bit mask. */
#define TCA_SINGLE_DBGRUN_bp  0  /* Debug Run bit position. */

/* TCA_SPLIT.CTRLA  bit masks and bit positions */
#define TCA_SPLIT_ENABLE_bm  0x01  /* Module Enable bit mask. */
#define TCA_SPLIT_ENABLE_bp  0  /* Module Enable bit position. */
#define TCA_SPLIT_CLKSEL_gm  0x0E  /* Clock Selection group mask. */
#define TCA_SPLIT_CLKSEL_gp  1  /* Clock Selection group position. */
#define TCA_SPLIT_CLKSEL_0_bm  (1<<1)  /* Clock Selection bit 0 mask. */
#define TCA_SPLIT_CLKSEL_0_bp  1  /* Clock Selection bit 0 position. */
#define TCA_SPLIT_CLKSEL_1_bm  (1<<2)  /* Clock Selection bit 1 mask. */
#define TCA_SPLIT_CLKSEL_1_bp  2  /* Clock Selection bit 1 position. */
#define TCA_SPLIT_CLKSEL_2_bm  (1<<3)  /* Clock Selection bit 2 mask. */
#define TCA_SPLIT_CLKSEL_2_bp  3  /* Clock Selection bit 2 position. */

/* TCA_SPLIT.CTRLB  bit masks and bit positions */
#define TCA_SPLIT_LCMP0EN_bm  0x01  /* Low Compare 0 Enable bit mask. */
#define TCA_SPLIT_LCMP0EN_bp  0  /* Low Compare 0 Enable bit position. */
#define TCA_SPLIT_LCMP1EN_bm  0x02  /* Low Compare 1 Enable bit mask. */
#define TCA_SPLIT_LCMP1EN_bp  1  /* Low Compare 1 Enable bit position. */
#define TCA_SPLIT_LCMP2EN_bm  0x04  /* Low Compare 2 Enable bit mask. */
#define TCA_SPLIT_LCMP2EN_bp  2  /* Low Compare 2 Enable bit position. */
#define TCA_SPLIT_HCMP0EN_bm  0x10  /* High Compare 0 Enable bit mask. */
#define TCA_SPLIT_HCMP0EN_bp  4  /* High Compare 0 Enable bit position. */
#define TCA_SPLIT_HCMP1EN_bm  0x20  /* High Compare 1 Enable bit mask. */
#define TCA_SPLIT_HCMP1EN_bp  5  /* High Compare 1 Enable bit position. */
#define TCA_SPLIT_HCMP2EN_bm  0x40  /* High Compare 2 Enable bit mask. */
#define TCA_SPLIT_HCMP2EN_bp  6  /* High Compare 2 Enable bit position. */

/* TCA_SPLIT.CTRLC  bit masks and bit positions */
#define TCA_SPLIT_LCMP0OV_bm  0x01  /* Low Compare 0 Output Value bit mask. */
#define TCA_SPLIT_LCMP0OV_bp  0  /* Low Compare 0 Output Value bit position. */
#define TCA_SPLIT_LCMP1OV_bm  0x02  /* Low Compare 1 Output Value bit mask. */
#define TCA_SPLIT_LCMP1OV_bp  1  /* Low Compare 1 Output Value bit position. */
#define TCA_SPLIT_LCMP2OV_bm  0x04  /* Low Compare 2 Output Value bit mask. */
#define TCA_SPLIT_LCMP2OV_bp  2  /* Low Compare 2 Output Value bit position. */
#define TCA_SPLIT_HCMP0OV_bm  0x10  /* High Compare 0 Output Value bit mask. */
#define TCA_SPLIT_HCMP0OV_bp  4  /* High Compare 0 Output Value bit position. */
#define TCA_SPLIT_HCMP1OV_bm  0x20  /* High Compare 1 Output Value bit mask. */
#define TCA_SPLIT_HCMP1OV_bp  5  /* High Compare 1 Output Value bit position. */
#define TCA_SPLIT_HCMP2OV_bm  0x40  /* High Compare 2 Output Value bit mask. */
#define TCA_SPLIT_HCMP2OV_bp  6  /* High Compare 2 Output Value bit position. */

/* TCA_SPLIT.CTRLD  bit masks and bit positions */
#define TCA_SPLIT_SPLITM_bm  0x01  /* Split Mode Enable bit mask. */
#define TCA_SPLIT_SPLITM_bp  0  /* Split Mode Enable bit position. */

/* TCA_SPLIT.CTRLECLR  bit masks and bit positions */
#define TCA_SPLIT_CMD_gm  0x0C  /* Command group mask. */
#define TCA_SPLIT_CMD_gp  2  /* Command group position. */
#define TCA_SPLIT_CMD_0_bm  (1<<2)  /* Command bit 0 mask. */
#define TCA_SPLIT_CMD_0_bp  2  /* Command bit 0 position. */
#define TCA_SPLIT_CMD_1_bm  (1<<3)  /* Command bit 1 mask. */
#define TCA_SPLIT_CMD_1_bp  3  /* Command bit 1 position. */

/* TCA_SPLIT.CTRLESET  bit masks and bit positions */
/* TCA_SPLIT_CMD  is already defined. */

/* TCA_SPLIT.INTCTRL  bit masks and bit positions */
#define TCA_SPLIT_LUNF_bm  0x01  /* Low Underflow Interrupt Enable bit mask. */
#define TCA_SPLIT_LUNF_bp  0  /* Low Underflow Interrupt Enable bit position. */
#define TCA_SPLIT_HUNF_bm  0x02  /* High Underflow Interrupt Enable bit mask. */
#define TCA_SPLIT_HUNF_bp  1  /* High Underflow Interrupt Enable bit position. */
#define TCA_SPLIT_LCMP0_bm  0x10  /* Low Compare 0 Interrupt Enable bit mask. */
#define TCA_SPLIT_LCMP0_bp  4  /* Low Compare 0 Interrupt Enable bit position. */
#define TCA_SPLIT_LCMP1_bm  0x20  /* Low Compare 1 Interrupt Enable bit mask. */
#define TCA_SPLIT_LCMP1_bp  5  /* Low Compare 1 Interrupt Enable bit position. */
#define TCA_SPLIT_LCMP2_bm  0x40  /* Low Compare 2 Interrupt Enable bit mask. */
#define TCA_SPLIT_LCMP2_bp  6  /* Low Compare 2 Interrupt Enable bit position. */

/* TCA_SPLIT.INTFLAGS  bit masks and bit positions */
/* TCA_SPLIT_LUNF  is already defined. */
/* TCA_SPLIT_HUNF  is already defined. */
/* TCA_SPLIT_LCMP0  is already defined. */
/* TCA_SPLIT_LCMP1  is already defined. */
/* TCA_SPLIT_LCMP2  is already defined. */

/* TCA_SPLIT.DBGCTRL  bit masks and bit positions */
#define TCA_SPLIT_DBGRUN_bm  0x01  /* Debug Run bit mask. */
#define TCA_SPLIT_DBGRUN_bp  0  /* Debug Run bit position. */


/* TCB - 16-bit Timer Type B */
/* TCB.CTRLA  bit masks and bit positions */
#define TCB_ENABLE_bm  0x01  /* Enable bit mask. */
#define TCB_ENABLE_bp  0  /* Enable bit position. */
#define TCB_CLKSEL_gm  0x06  /* Clock Select group mask. */
#define TCB_CLKSEL_gp  1  /* Clock Select group position. */
#define TCB_CLKSEL_0_bm  (1<<1)  /* Clock Select bit 0 mask. */
#define TCB_CLKSEL_0_bp  1  /* Clock Select bit 0 position. */
#define TCB_CLKSEL_1_bm  (1<<2)  /* Clock Select bit 1 mask. */
#define TCB_CLKSEL_1_bp  2  /* Clock Select bit 1 position. */
#define TCB_SYNCUPD_bm  0x10  /* Synchronize Update bit mask. */
#define TCB_SYNCUPD_bp  4  /* Synchronize Update bit position. */
#define TCB_RUNSTDBY_bm  0x40  /* Run Standby bit mask. */
#define TCB_RUNSTDBY_bp  6  /* Run Standby bit position. */

/* TCB.CTRLB  bit masks and bit positions */
#define TCB_CNTMODE_gm  0x07  /* Timer Mode group mask. */
#define TCB_CNTMODE_gp  0  /* Timer Mode group position. */
#define TCB_CNTMODE_0_bm  (1<<0)  /* Timer Mode bit 0 mask. */
#define TCB_CNTMODE_0_bp  0  /* Timer Mode bit 0 position. */
#define TCB_CNTMODE_1_bm  (1<<1)  /* Timer Mode bit 1 mask. */
#define TCB_CNTMODE_1_bp  1  /* Timer Mode bit 1 position. */
#define TCB_CNTMODE_2_bm  (1<<2)  /* Timer Mode bit 2 mask. */
#define TCB_CNTMODE_2_bp  2  /* Timer Mode bit 2 position. */
#define TCB_CCMPEN_bm  0x10  /* Pin Output Enable bit mask. */
#define TCB_CCMPEN_bp  4  /* Pin Output Enable bit position. */
#define TCB_CCMPINIT_bm  0x20  /* Pin Initial State bit mask. */
#define TCB_CCMPINIT_bp  5  /* Pin Initial State bit position. */
#define TCB_ASYNC_bm  0x40  /* Asynchronous Enable bit mask. */
#define TCB_ASYNC_bp  6  /* Asynchronous Enable bit position. */

/* TCB.EVCTRL  bit masks and bit positions */
#define TCB_CAPTEI_bm  0x01  /* Event Input Enable bit mask. */
#define TCB_CAPTEI_bp  0  /* Event Input Enable bit position. */
#define TCB_EDGE_bm  0x10  /* Event Edge bit mask. */
#define TCB_EDGE_bp  4  /* Event Edge bit position. */
#define TCB_FILTER_bm  0x40  /* Input Capture Noise Cancellation Filter bit mask. */
#define TCB_FILTER_bp  6  /* Input Capture Noise Cancellation Filter bit position. */

/* TCB.INTCTRL  bit masks and bit positions */
#define TCB_CAPT_bm  0x01  /* Capture or Timeout bit mask. */
#define TCB_CAPT_bp  0  /* Capture or Timeout bit position. */

/* TCB.INTFLAGS  bit masks and bit positions */
/* TCB_CAPT  is already defined. */

/* TCB.STATUS  bit masks and bit positions */
#define TCB_RUN_bm  0x01  /* Run bit mask. */
#define TCB_RUN_bp  0  /* Run bit position. */

/* TCB.DBGCTRL  bit masks and bit positions */
#define TCB_DBGRUN_bm  0x01  /* Debug Run bit mask. */
#define TCB_DBGRUN_bp  0  /* Debug Run bit position. */


/* TWI - Two-Wire Interface */
/* TWI.CTRLA  bit masks and bit positions */
#define TWI_FMPEN_bm  0x02  /* FM Plus Enable bit mask. */
#define TWI_FMPEN_bp  1  /* FM Plus Enable bit position. */
#define TWI_SDAHOLD_gm  0x0C  /* SDA Hold Time group mask. */
#define TWI_SDAHOLD_gp  2  /* SDA Hold Time group position. */
#define TWI_SDAHOLD_0_bm  (1<<2)  /* SDA Hold Time bit 0 mask. */
#define TWI_SDAHOLD_0_bp  2  /* SDA Hold Time bit 0 position. */
#define TWI_SDAHOLD_1_bm  (1<<3)  /* SDA Hold Time bit 1 mask. */
#define TWI_SDAHOLD_1_bp  3  /* SDA Hold Time bit 1 position. */
#define TWI_SDASETUP_bm  0x10  /* SDA Setup Time bit mask. */
#define TWI_SDASETUP_bp  4  /* SDA Setup Time bit position. */

/* TWI.DBGCTRL  bit masks and bit positions */
#define TWI_DBGRUN_bm  0x01  /* Debug Run bit mask. */
#define TWI_DBGRUN_bp  0  /* Debug Run bit position. */

/* TWI.MCTRLA  bit masks and bit positions */
#define TWI_ENABLE_bm  0x01  /* Enable TWI Host bit mask. */
#define TWI_ENABLE_bp  0  /* Enable TWI Host bit position. */
#define TWI_SMEN_bm  0x02  /* Smart Mode Enable bit mask. */
#define TWI_SMEN_bp  1  /* Smart Mode Enable bit position. */
#define TWI_TIMEOUT_gm  0x0C  /* Inactive Bus Timeout group mask. */
#define TWI_TIMEOUT_gp  2  /* Inactive Bus Timeout group position. */
#define TWI_TIMEOUT_0_bm  (1<<2)  /* Inactive Bus Timeout bit 0 mask. */
#define TWI_TIMEOUT_0_bp  2  /* Inactive Bus Timeout bit 0 position. */
#define TWI_TIMEOUT_1_bm  (1<<3)  /* Inactive Bus Timeout bit 1 mask. */
#define TWI_TIMEOUT_1_bp  3  /* Inactive Bus Timeout bit 1 position. */
#define TWI_QCEN_bm  0x10  /* Quick Command Enable bit mask. */
#define TWI_QCEN_bp  4  /* Quick Command Enable bit position. */
#define TWI_WIEN_bm  0x40  /* Write Interrupt Enable bit mask. */
#define TWI_WIEN_bp  6  /* Write Interrupt Enable bit position. */
#define TWI_RIEN_bm  0x80  /* Read Interrupt Enable bit mask. */
#define TWI_RIEN_bp  7  /* Read Interrupt Enable bit position. */

/* TWI.MCTRLB  bit masks and bit positions */
#define TWI_MCMD_gm  0x03  /* Command group mask. */
#define TWI_MCMD_gp  0  /* Command group position. */
#define TWI_MCMD_0_bm  (1<<0)  /* Command bit 0 mask. */
#define TWI_MCMD_0_bp  0  /* Command bit 0 position. */
#define TWI_MCMD_1_bm  (1<<1)  /* Command bit 1 mask. */
#define TWI_MCMD_1_bp  1  /* Command bit 1 position. */
#define TWI_ACKACT_bm  0x04  /* Acknowledge Action bit mask. */
#define TWI_ACKACT_bp  2  /* Acknowledge Action bit position. */
#define TWI_FLUSH_bm  0x08  /* Flush bit mask. */
#define TWI_FLUSH_bp  3  /* Flush bit position. */

/* TWI.MSTATUS  bit masks and bit positions */
#define TWI_BUSSTATE_gm  0x03  /* Bus State group mask. */
#define TWI_BUSSTATE_gp  0  /* Bus State group position. */
#define TWI_BUSSTATE_0_bm  (1<<0)  /* Bus State bit 0 mask. */
#define TWI_BUSSTATE_0_bp  0  /* Bus State bit 0 position. */
#define TWI_BUSSTATE_1_bm  (1<<1)  /* Bus State bit 1 mask. */
#define TWI_BUSSTATE_1_bp  1  /* Bus State bit 1 position. */
#define TWI_BUSERR_bm  0x04  /* Bus Error bit mask. */
#define TWI_BUSERR_bp  2  /* Bus Error bit position. */
#define TWI_ARBLOST_bm  0x08  /* Arbitration Lost bit mask. */
#define TWI_ARBLOST_bp  3  /* Arbitration Lost bit position. */
#define TWI_RXACK_bm  0x10  /* Received Acknowledge bit mask. */
#define TWI_RXACK_bp  4  /* Received Acknowledge bit position. */
#define TWI_CLKHOLD_bm  0x20  /* Clock Hold bit mask. */
#define TWI_CLKHOLD_bp  5  /* Clock Hold bit position. */
#define TWI_WIF_bm  0x40  /* Write Interrupt Flag bit mask. */
#define TWI_WIF_bp  6  /* Write Interrupt Flag bit position. */
#define TWI_RIF_bm  0x80  /* Read Interrupt Flag bit mask. */
#define TWI_RIF_bp  7  /* Read Interrupt Flag bit position. */

/* TWI.SCTRLA  bit masks and bit positions */
/* TWI_ENABLE  is already defined. */
/* TWI_SMEN  is already defined. */
#define TWI_PMEN_bm  0x04  /* Promiscuous Mode Enable bit mask. */
#define TWI_PMEN_bp  2  /* Promiscuous Mode Enable bit position. */
#define TWI_PIEN_bm  0x20  /* Stop Interrupt Enable bit mask. */
#define TWI_PIEN_bp  5  /* Stop Interrupt Enable bit position. */
#define TWI_APIEN_bm  0x40  /* Address/Stop Interrupt Enable bit mask. */
#define TWI_APIEN_bp  6  /* Address/Stop Interrupt Enable bit position. */
#define TWI_DIEN_bm  0x80  /* Data Interrupt Enable bit mask. */
#define TWI_DIEN_bp  7  /* Data Interrupt Enable bit position. */

/* TWI.SCTRLB  bit masks and bit positions */
#define TWI_SCMD_gm  0x03  /* Command group mask. */
#define TWI_SCMD_gp  0  /* Command group position. */
#define TWI_SCMD_0_bm  (1<<0)  /* Command bit 0 mask. */
#define TWI_SCMD_0_bp  0  /* Command bit 0 position. */
#define TWI_SCMD_1_bm  (1<<1)  /* Command bit 1 mask. */
#define TWI_SCMD_1_bp  1  /* Command bit 1 position. */
/* TWI_ACKACT  is already defined. */

/* TWI.SSTATUS  bit masks and bit positions */
#define TWI_AP_bm  0x01  /* Client Address or Stop bit mask. */
#define TWI_AP_bp  0  /* Client Address or Stop bit position. */
#define TWI_DIR_bm  0x02  /* Read/Write Direction bit mask. */
#define TWI_DIR_bp  1  /* Read/Write Direction bit position. */
/* TWI_BUSERR  is already defined. */
#define TWI_COLL_bm  0x08  /* Collision bit mask. */
#define TWI_COLL_bp  3  /* Collision bit position. */
/* TWI_RXACK  is already defined. */
/* TWI_CLKHOLD  is already defined. */
#define TWI_APIF_bm  0x40  /* Address/Stop Interrupt Flag bit mask. */
#define TWI_APIF_bp  6  /* Address/Stop Interrupt Flag bit position. */
#define TWI_DIF_bm  0x80  /* Data Interrupt Flag bit mask. */
#define TWI_DIF_bp  7  /* Data Interrupt Flag bit position. */

/* TWI.SADDRMASK  bit masks and bit positions */
#define TWI_ADDREN_bm  0x01  /* Address Enable bit mask. */
#define TWI_ADDREN_bp  0  /* Address Enable bit position. */
#define TWI_ADDRMASK_gm  0xFE  /* Address Mask group mask. */
#define TWI_ADDRMASK_gp  1  /* Address Mask group position. */
#define TWI_ADDRMASK_0_bm  (1<<1)  /* Address Mask bit 0 mask. */
#define TWI_ADDRMASK_0_bp  1  /* Address Mask bit 0 position. */
#define TWI_ADDRMASK_1_bm  (1<<2)  /* Address Mask bit 1 mask. */
#define TWI_ADDRMASK_1_bp  2  /* Address Mask bit 1 position. */
#define TWI_ADDRMASK_2_bm  (1<<3)  /* Address Mask bit 2 mask. */
#define TWI_ADDRMASK_2_bp  3  /* Address Mask bit 2 position. */
#define TWI_ADDRMASK_3_bm  (1<<4)  /* Address Mask bit 3 mask. */
#define TWI_ADDRMASK_3_bp  4  /* Address Mask bit 3 position. */
#define TWI_ADDRMASK_4_bm  (1<<5)  /* Address Mask bit 4 mask. */
#define TWI_ADDRMASK_4_bp  5  /* Address Mask bit 4 position. */
#define TWI_ADDRMASK_5_bm  (1<<6)  /* Address Mask bit 5 mask. */
#define TWI_ADDRMASK_5_bp  6  /* Address Mask bit 5 position. */
#define TWI_ADDRMASK_6_bm  (1<<7)  /* Address Mask bit 6 mask. */
#define TWI_ADDRMASK_6_bp  7  /* Address Mask bit 6 position. */


/* USART - Universal Synchronous and Asynchronous Receiver and Transmitter */
/* USART.RXDATAL  bit masks and bit positions */
#define USART_DATA_gm  0xFF  /* RX Data group mask. */
#define USART_DATA_gp  0  /* RX Data group position. */
#define USART_DATA_0_bm  (1<<0)  /* RX Data bit 0 mask. */
#define USART_DATA_0_bp  0  /* RX Data bit 0 position. */
#define USART_DATA_1_bm  (1<<1)  /* RX Data bit 1 mask. */
#define USART_DATA_1_bp  1  /* RX Data bit 1 position. */
#define USART_DATA_2_bm  (1<<2)  /* RX Data bit 2 mask. */
#define USART_DATA_2_bp  2  /* RX Data bit 2 position. */
#define USART_DATA_3_bm  (1<<3)  /* RX Data bit 3 mask. */
#define USART_DATA_3_bp  3  /* RX Data bit 3 position. */
#define USART_DATA_4_bm  (1<<4)  /* RX Data bit 4 mask. */
#define USART_DATA_4_bp  4  /* RX Data bit 4 position. */
#define USART_DATA_5_bm  (1<<5)  /* RX Data bit 5 mask. */
#define USART_DATA_5_bp  5  /* RX Data bit 5 position. */
#define USART_DATA_6_bm  (1<<6)  /* RX Data bit 6 mask. */
#define USART_DATA_6_bp  6  /* RX Data bit 6 position. */
#define USART_DATA_7_bm  (1<<7)  /* RX Data bit 7 mask. */
#define USART_DATA_7_bp  7  /* RX Data bit 7 position. */

/* USART.RXDATAH  bit masks and bit positions */
#define USART_DATA8_bm  0x01  /* Receiver Data Register bit mask. */
#define USART_DATA8_bp  0  /* Receiver Data Register bit position. */
#define USART_PERR_bm  0x02  /* Parity Error bit mask. */
#define USART_PERR_bp  1  /* Parity Error bit position. */
#define USART_FERR_bm  0x04  /* Frame Error bit mask. */
#define USART_FERR_bp  2  /* Frame Error bit position. */
#define USART_BUFOVF_bm  0x40  /* Buffer Overflow bit mask. */
#define USART_BUFOVF_bp  6  /* Buffer Overflow bit position. */
#define USART_RXCIF_bm  0x80  /* Receive Complete Interrupt Flag bit mask. */
#define USART_RXCIF_bp  7  /* Receive Complete Interrupt Flag bit position. */

/* USART.TXDATAL  bit masks and bit positions */
/* USART_DATA  is already defined. */

/* USART.TXDATAH  bit masks and bit positions */
/* USART_DATA8  is already defined. */

/* USART.STATUS  bit masks and bit positions */
#define USART_WFB_bm  0x01  /* Wait For Break bit mask. */
#define USART_WFB_bp  0  /* Wait For Break bit position. */
#define USART_BDF_bm  0x02  /* Break Detected Flag bit mask. */
#define USART_BDF_bp  1  /* Break Detected Flag bit position. */
#define USART_ISFIF_bm  0x08  /* Inconsistent Sync Field Interrupt Flag bit mask. */
#define USART_ISFIF_bp  3  /* Inconsistent Sync Field Interrupt Flag bit position. */
#define USART_RXSIF_bm  0x10  /* Receive Start Interrupt bit mask. */
#define USART_RXSIF_bp  4  /* Receive Start Interrupt bit position. */
#define USART_DREIF_bm  0x20  /* Data Register Empty Flag bit mask. */
#define USART_DREIF_bp  5  /* Data Register Empty Flag bit position. */
#define USART_TXCIF_bm  0x40  /* Transmit Interrupt Flag bit mask. */
#define USART_TXCIF_bp  6  /* Transmit Interrupt Flag bit position. */
/* USART_RXCIF  is already defined. */

/* USART.CTRLA  bit masks and bit positions */
#define USART_RS485_gm  0x03  /* RS485 Mode internal transmitter group mask. */
#define USART_RS485_gp  0  /* RS485 Mode internal transmitter group position. */
#define USART_RS485_0_bm  (1<<0)  /* RS485 Mode internal transmitter bit 0 mask. */
#define USART_RS485_0_bp  0  /* RS485 Mode internal transmitter bit 0 position. */
#define USART_RS485_1_bm  (1<<1)  /* RS485 Mode internal transmitter bit 1 mask. */
#define USART_RS485_1_bp  1  /* RS485 Mode internal transmitter bit 1 position. */
#define USART_ABEIE_bm  0x04  /* Auto-baud Error Interrupt Enable bit mask. */
#define USART_ABEIE_bp  2  /* Auto-baud Error Interrupt Enable bit position. */
#define USART_LBME_bm  0x08  /* Loop-back Mode Enable bit mask. */
#define USART_LBME_bp  3  /* Loop-back Mode Enable bit position. */
#define USART_RXSIE_bm  0x10  /* Receiver Start Frame Interrupt Enable bit mask. */
#define USART_RXSIE_bp  4  /* Receiver Start Frame Interrupt Enable bit position. */
#define USART_DREIE_bm  0x20  /* Data Register Empty Interrupt Enable bit mask. */
#define USART_DREIE_bp  5  /* Data Register Empty Interrupt Enable bit position. */
#define USART_TXCIE_bm  0x40  /* Transmit Complete Interrupt Enable bit mask. */
#define USART_TXCIE_bp  6  /* Transmit Complete Interrupt Enable bit position. */
#define USART_RXCIE_bm  0x80  /* Receive Complete Interrupt Enable bit mask. */
#define USART_RXCIE_bp  7  /* Receive Complete Interrupt Enable bit position. */

/* USART.CTRLB  bit masks and bit positions */
#define USART_MPCM_bm  0x01  /* Multi-processor Communication Mode bit mask. */
#define USART_MPCM_bp  0  /* Multi-processor Communication Mode bit position. */
#define USART_RXMODE_gm  0x06  /* Receiver Mode group mask. */
#define USART_RXMODE_gp  1  /* Receiver Mode group position. */
#define USART_RXMODE_0_bm  (1<<1)  /* Receiver Mode bit 0 mask. */
#define USART_RXMODE_0_bp  1  /* Receiver Mode bit 0 position. */
#define USART_RXMODE_1_bm  (1<<2)  /* Receiver Mode bit 1 mask. */
#define USART_RXMODE_1_bp  2  /* Receiver Mode bit 1 position. */
#define USART_ODME_bm  0x08  /* Open Drain Mode Enable bit mask. */
#define USART_ODME_bp  3  /* Open Drain Mode Enable bit position. */
#define USART_SFDEN_bm  0x10  /* Start Frame Detection Enable bit mask. */
#define USART_SFDEN_bp  4  /* Start Frame Detection Enable bit position. */
#define USART_TXEN_bm  0x40  /* Transmitter Enable bit mask. */
#define USART_TXEN_bp  6  /* Transmitter Enable bit position. */
#define USART_RXEN_bm  0x80  /* Reciever enable bit mask. */
#define USART_RXEN_bp  7  /* Reciever enable bit position. */

/* USART.CTRLC  bit masks and bit positions */
#define USART_UCPHA_bm  0x02  /* SPI Host Mode, Clock Phase bit mask. */
#define USART_UCPHA_bp  1  /* SPI Host Mode, Clock Phase bit position. */
#define USART_UDORD_bm  0x04  /* SPI Host Mode, Data Order bit mask. */
#define USART_UDORD_bp  2  /* SPI Host Mode, Data Order bit position. */
#define USART_CHSIZE_gm  0x07  /* Character Size group mask. */
#define USART_CHSIZE_gp  0  /* Character Size group position. */
#define USART_CHSIZE_0_bm  (1<<0)  /* Character Size bit 0 mask. */
#define USART_CHSIZE_0_bp  0  /* Character Size bit 0 position. */
#define USART_CHSIZE_1_bm  (1<<1)  /* Character Size bit 1 mask. */
#define USART_CHSIZE_1_bp  1  /* Character Size bit 1 position. */
#define USART_CHSIZE_2_bm  (1<<2)  /* Character Size bit 2 mask. */
#define USART_CHSIZE_2_bp  2  /* Character Size bit 2 position. */
#define USART_SBMODE_bm  0x08  /* Stop Bit Mode bit mask. */
#define USART_SBMODE_bp  3  /* Stop Bit Mode bit position. */
#define USART_PMODE_gm  0x30  /* Parity Mode group mask. */
#define USART_PMODE_gp  4  /* Parity Mode group position. */
#define USART_PMODE_0_bm  (1<<4)  /* Parity Mode bit 0 mask. */
#define USART_PMODE_0_bp  4  /* Parity Mode bit 0 position. */
#define USART_PMODE_1_bm  (1<<5)  /* Parity Mode bit 1 mask. */
#define USART_PMODE_1_bp  5  /* Parity Mode bit 1 position. */
#define USART_CMODE_gm  0xC0  /* Communication Mode group mask. */
#define USART_CMODE_gp  6  /* Communication Mode group position. */
#define USART_CMODE_0_bm  (1<<6)  /* Communication Mode bit 0 mask. */
#define USART_CMODE_0_bp  6  /* Communication Mode bit 0 position. */
#define USART_CMODE_1_bm  (1<<7)  /* Communication Mode bit 1 mask. */
#define USART_CMODE_1_bp  7  /* Communication Mode bit 1 position. */

/* USART.DBGCTRL  bit masks and bit positions */
#define USART_DBGRUN_bm  0x01  /* Debug Run bit mask. */
#define USART_DBGRUN_bp  0  /* Debug Run bit position. */

/* USART.EVCTRL  bit masks and bit positions */
#define USART_IREI_bm  0x01  /* IrDA Event Input Enable bit mask. */
#define USART_IREI_bp  0  /* IrDA Event Input Enable bit position. */

/* USART.TXPLCTRL  bit masks and bit positions */
#define USART_TXPL_gm  0xFF  /* Transmit pulse length group mask. */
#define USART_TXPL_gp  0  /* Transmit pulse length group position. */
#define USART_TXPL_0_bm  (1<<0)  /* Transmit pulse length bit 0 mask. */
#define USART_TXPL_0_bp  0  /* Transmit pulse length bit 0 position. */
#define USART_TXPL_1_bm  (1<<1)  /* Transmit pulse length bit 1 mask. */
#define USART_TXPL_1_bp  1  /* Transmit pulse length bit 1 position. */
#define USART_TXPL_2_bm  (1<<2)  /* Transmit pulse length bit 2 mask. */
#define USART_TXPL_2_bp  2  /* Transmit pulse length bit 2 position. */
#define USART_TXPL_3_bm  (1<<3)  /* Transmit pulse length bit 3 mask. */
#define USART_TXPL_3_bp  3  /* Transmit pulse length bit 3 position. */
#define USART_TXPL_4_bm  (1<<4)  /* Transmit pulse length bit 4 mask. */
#define USART_TXPL_4_bp  4  /* Transmit pulse length bit 4 position. */
#define USART_TXPL_5_bm  (1<<5)  /* Transmit pulse length bit 5 mask. */
#define USART_TXPL_5_bp  5  /* Transmit pulse length bit 5 position. */
#define USART_TXPL_6_bm  (1<<6)  /* Transmit pulse length bit 6 mask. */
#define USART_TXPL_6_bp  6  /* Transmit pulse length bit 6 position. */
#define USART_TXPL_7_bm  (1<<7)  /* Transmit pulse length bit 7 mask. */
#define USART_TXPL_7_bp  7  /* Transmit pulse length bit 7 position. */

/* USART.RXPLCTRL  bit masks and bit positions */
#define USART_RXPL_gm  0x7F  /* Receiver Pulse Lenght group mask. */
#define USART_RXPL_gp  0  /* Receiver Pulse Lenght group position. */
#define USART_RXPL_0_bm  (1<<0)  /* Receiver Pulse Lenght bit 0 mask. */
#define USART_RXPL_0_bp  0  /* Receiver Pulse Lenght bit 0 position. */
#define USART_RXPL_1_bm  (1<<1)  /* Receiver Pulse Lenght bit 1 mask. */
#define USART_RXPL_1_bp  1  /* Receiver Pulse Lenght bit 1 position. */
#define USART_RXPL_2_bm  (1<<2)  /* Receiver Pulse Lenght bit 2 mask. */
#define USART_RXPL_2_bp  2  /* Receiver Pulse Lenght bit 2 position. */
#define USART_RXPL_3_bm  (1<<3)  /* Receiver Pulse Lenght bit 3 mask. */
#define USART_RXPL_3_bp  3  /* Receiver Pulse Lenght bit 3 position. */
#define USART_RXPL_4_bm  (1<<4)  /* Receiver Pulse Lenght bit 4 mask. */
#define USART_RXPL_4_bp  4  /* Receiver Pulse Lenght bit 4 position. */
#define USART_RXPL_5_bm  (1<<5)  /* Receiver Pulse Lenght bit 5 mask. */
#define USART_RXPL_5_bp  5  /* Receiver Pulse Lenght bit 5 position. */
#define USART_RXPL_6_bm  (1<<6)  /* Receiver Pulse Lenght bit 6 mask. */
#define USART_RXPL_6_bp  6  /* Receiver Pulse Lenght bit 6 position. */



/* VPORT - Virtual Ports */
/* VPORT.INTFLAGS  bit masks and bit positions */
#define VPORT_INT_gm  0xFF  /* Pin Interrupt group mask. */
#define VPORT_INT_gp  0  /* Pin Interrupt group position. */
#define VPORT_INT_0_bm  (1<<0)  /* Pin Interrupt bit 0 mask. */
#define VPORT_INT_0_bp  0  /* Pin Interrupt bit 0 position. */
#define VPORT_INT_1_bm  (1<<1)  /* Pin Interrupt bit 1 mask. */
#define VPORT_INT_1_bp  1  /* Pin Interrupt bit 1 position. */
#define VPORT_INT_2_bm  (1<<2)  /* Pin Interrupt bit 2 mask. */
#define VPORT_INT_2_bp  2  /* Pin Interrupt bit 2 position. */
#define VPORT_INT_3_bm  (1<<3)  /* Pin Interrupt bit 3 mask. */
#define VPORT_INT_3_bp  3  /* Pin Interrupt bit 3 position. */
#define VPORT_INT_4_bm  (1<<4)  /* Pin Interrupt bit 4 mask. */
#define VPORT_INT_4_bp  4  /* Pin Interrupt bit 4 position. */
#define VPORT_INT_5_bm  (1<<5)  /* Pin Interrupt bit 5 mask. */
#define VPORT_INT_5_bp  5  /* Pin Interrupt bit 5 position. */
#define VPORT_INT_6_bm  (1<<6)  /* Pin Interrupt bit 6 mask. */
#define VPORT_INT_6_bp  6  /* Pin Interrupt bit 6 position. */
#define VPORT_INT_7_bm  (1<<7)  /* Pin Interrupt bit 7 mask. */
#define VPORT_INT_7_bp  7  /* Pin Interrupt bit 7 position. */


/* VREF - Voltage reference */
/* VREF.CTRLA  bit masks and bit positions */
#define VREF_DAC0REFSEL_gm  0x07  /* DAC0/AC0 reference select group mask. */
#define VREF_DAC0REFSEL_gp  0  /* DAC0/AC0 reference select group position. */
#define VREF_DAC0REFSEL_0_bm  (1<<0)  /* DAC0/AC0 reference select bit 0 mask. */
#define VREF_DAC0REFSEL_0_bp  0  /* DAC0/AC0 reference select bit 0 position. */
#define VREF_DAC0REFSEL_1_bm  (1<<1)  /* DAC0/AC0 reference select bit 1 mask. */
#define VREF_DAC0REFSEL_1_bp  1  /* DAC0/AC0 reference select bit 1 position. */
#define VREF_DAC0REFSEL_2_bm  (1<<2)  /* DAC0/AC0 reference select bit 2 mask. */
#define VREF_DAC0REFSEL_2_bp  2  /* DAC0/AC0 reference select bit 2 position. */
#define VREF_ADC0REFSEL_gm  0x70  /* ADC0 reference select group mask. */
#define VREF_ADC0REFSEL_gp  4  /* ADC0 reference select group position. */
#define VREF_ADC0REFSEL_0_bm  (1<<4)  /* ADC0 reference select bit 0 mask. */
#define VREF_ADC0REFSEL_0_bp  4  /* ADC0 reference select bit 0 position. */
#define VREF_ADC0REFSEL_1_bm  (1<<5)  /* ADC0 reference select bit 1 mask. */
#define VREF_ADC0REFSEL_1_bp  5  /* ADC0 reference select bit 1 position. */
#define VREF_ADC0REFSEL_2_bm  (1<<6)  /* ADC0 reference select bit 2 mask. */
#define VREF_ADC0REFSEL_2_bp  6  /* ADC0 reference select bit 2 position. */

/* VREF.CTRLB  bit masks and bit positions */
#define VREF_DAC0REFEN_bm  0x01  /* DAC0/AC0 reference enable bit mask. */
#define VREF_DAC0REFEN_bp  0  /* DAC0/AC0 reference enable bit position. */
#define VREF_ADC0REFEN_bm  0x02  /* ADC0 reference enable bit mask. */
#define VREF_ADC0REFEN_bp  1  /* ADC0 reference enable bit position. */


/* WDT - Watch-Dog Timer */
/* WDT.CTRLA  bit masks and bit positions */
#define WDT_PERIOD_gm  0x0F  /* Period group mask. */
#define WDT_PERIOD_gp  0  /* Period group position. */
#define WDT_PERIOD_0_bm  (1<<0)  /* Period bit 0 mask. */
#define WDT_PERIOD_0_bp  0  /* Period bit 0 position. */
#define WDT_PERIOD_1_bm  (1<<1)  /* Period bit 1 mask. */
#define WDT_PERIOD_1_bp  1  /* Period bit 1 position. */
#define WDT_PERIOD_2_bm  (1<<2)  /* Period bit 2 mask. */
#define WDT_PERIOD_2_bp  2  /* Period bit 2 position. */
#define WDT_PERIOD_3_bm  (1<<3)  /* Period bit 3 mask. */
#define WDT_PERIOD_3_bp  3  /* Period bit 3 position. */
#define WDT_WINDOW_gm  0xF0  /* Window group mask. */
#define WDT_WINDOW_gp  4  /* Window group position. */
#define WDT_WINDOW_0_bm  (1<<4)  /* Window bit 0 mask. */
#define WDT_WINDOW_0_bp  4  /* Window bit 0 position. */
#define WDT_WINDOW_1_bm  (1<<5)  /* Window bit 1 mask. */
#define WDT_WINDOW_1_bp  5  /* Window bit 1 position. */
#define WDT_WINDOW_2_bm  (1<<6)  /* Window bit 2 mask. */
#define WDT_WINDOW_2_bp  6  /* Window bit 2 position. */
#define WDT_WINDOW_3_bm  (1<<7)  /* Window bit 3 mask. */
#define WDT_WINDOW_3_bp  7  /* Window bit 3 position. */

/* WDT.STATUS  bit masks and bit positions */
#define WDT_SYNCBUSY_bm  0x01  /* Syncronization busy bit mask. */
#define WDT_SYNCBUSY_bp  0  /* Syncronization busy bit position. */
#define WDT_LOCK_bm  0x80  /* Lock enable bit mask. */
#define WDT_LOCK_bp  7  /* Lock enable bit position. */


/* ========== Generic Port Pins ========== */
#define PIN0_bm 0x01
#define PIN0_bp 0
#define PIN1_bm 0x02
#define PIN1_bp 1
#define PIN2_bm 0x04
#define PIN2_bp 2
#define PIN3_bm 0x08
#define PIN3_bp 3
#define PIN4_bm 0x10
#define PIN4_bp 4
#define PIN5_bm 0x20
#define PIN5_bp 5
#define PIN6_bm 0x40
#define PIN6_bp 6
#define PIN7_bm 0x80
#define PIN7_bp 7

/* ========== Interrupt Vector Definitions ========== */
/* Vector 0 is the reset vector */

/* CRCSCAN interrupt vectors */
#define CRCSCAN_NMI_vect_num  1
#define CRCSCAN_NMI_vect      _VECTOR(1)  /*  */

/* BOD interrupt vectors */
#define BOD_VLM_vect_num  2
#define BOD_VLM_vect      _VECTOR(2)  /*  */

/* PORTA interrupt vectors */
#define PORTA_PORT_vect_num  3
#define PORTA_PORT_vect      _VECTOR(3)  /*  */

/* PORTB interrupt vectors */
#define PORTB_PORT_vect_num  4
#define PORTB_PORT_vect      _VECTOR(4)  /*  */

/* PORTC interrupt vectors */
#define PORTC_PORT_vect_num  5
#define PORTC_PORT_vect      _VECTOR(5)  /*  */

/* RTC interrupt vectors */
#define RTC_CNT_vect_num  6
#define RTC_CNT_vect      _VECTOR(6)  /*  */
#define RTC_PIT_vect_num  7
#define RTC_PIT_vect      _VECTOR(7)  /*  */

/* TCA0 interrupt vectors */
#define TCA0_LUNF_vect_num  8
#define TCA0_LUNF_vect      _VECTOR(8)  /*  */
#define TCA0_OVF_vect_num  8
#define TCA0_OVF_vect      _VECTOR(8)  /*  */
#define TCA0_HUNF_vect_num  9
#define TCA0_HUNF_vect      _VECTOR(9)  /*  */
#define TCA0_CMP0_vect_num  10
#define TCA0_CMP0_vect      _VECTOR(10)  /*  */
#define TCA0_LCMP0_vect_num  10
#define TCA0_LCMP0_vect      _VECTOR(10)  /*  */
#define TCA0_CMP1_vect_num  11
#define TCA0_CMP1_vect      _VECTOR(11)  /*  */
#define TCA0_LCMP1_vect_num  11
#define TCA0_LCMP1_vect      _VECTOR(11)  /*  */
#define TCA0_CMP2_vect_num  12
#define TCA0_CMP2_vect      _VECTOR(12)  /*  */
#define TCA0_LCMP2_vect_num  12
#define TCA0_LCMP2_vect      _VECTOR(12)  /*  */

/* TCB0 interrupt vectors */
#define TCB0_INT_vect_num  13
#define TCB0_INT_vect      _VECTOR(13)  /*  */

/* AC0 interrupt vectors */
#define AC0_AC_vect_num  17
#define AC0_AC_vect      _VECTOR(17)  /*  */

/* ADC0 interrupt vectors */
#define ADC0_RESRDY_vect_num  20
#define ADC0_RESRDY_vect      _VECTOR(20)  /*  */
#define ADC0_WCOMP_vect_num  21
#define ADC0_WCOMP_vect      _VECTOR(21)  /*  */

/* TWI0 interrupt vectors */
#define TWI0_TWIS_vect_num  24
#define TWI0_TWIS_vect      _VECTOR(24)  /*  */
#define TWI0_TWIM_vect_num  25
#define TWI0_TWIM_vect      _VECTOR(25)  /*  */

/* SPI0 interrupt vectors */
#define SPI0_INT_vect_num  26
#define SPI0_INT_vect      _VECTOR(26)  /*  */

/* USART0 interrupt vectors */
#define USART0_RXC_vect_num  27
#define USART0_RXC_vect      _VECTOR(27)  /*  */
#define USART0_DRE_vect_num  28
#define USART0_DRE_vect      _VECTOR(28)  /*  */
#define USART0_TXC_vect_num  29
#define USART0_TXC_vect      _VECTOR(29)  /*  */

/* NVMCTRL interrupt vectors */
#define NVMCTRL_EE_vect_num  30
#define NVMCTRL_EE_vect      _VECTOR(30)  /*  */

#define _VECTOR_SIZE 2 /* Size of individual vector. */
#define _VECTORS_SIZE (31 * _VECTOR_SIZE)


/* ========== Constants ========== */

#if (defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__))
#  define DATAMEM_START     (0x0000)
#  define DATAMEM_SIZE      (40960)
#else
#  define DATAMEM_START     (0x0000U)
#  define DATAMEM_SIZE      (40960U)
#endif
#define DATAMEM_END       (DATAMEM_START + DATAMEM_SIZE - 1)

#if (defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__))
#  define IO_START     (0x0000)
#  define IO_SIZE      (4352)
#  define IO_PAGE_SIZE (0)
#else
#  define IO_START     (0x0000U)
#  define IO_SIZE      (4352U)
#  define IO_PAGE_SIZE (0U)
#endif
#define IO_END       (IO_START + IO_SIZE - 1)

#if (defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__))
#  define SIGNATURES_START     (0x1100)
#  define SIGNATURES_SIZE      (3)
#  define SIGNATURES_PAGE_SIZE (64)
#else
#  define SIGNATURES_START     (0x1100U)
#  define SIGNATURES_SIZE      (3U)
#  define SIGNATURES_PAGE_SIZE (64U)
#endif
#define SIGNATURES_END       (SIGNATURES_START + SIGNATURES_SIZE - 1)

#if (defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__))
#  define PROD_SIGNATURES_START     (0x1103)
#  define PROD_SIGNATURES_SIZE      (61)
#  define PROD_SIGNATURES_PAGE_SIZE (64)
#else
#  define PROD_SIGNATURES_START     (0x1103U)
#  define PROD_SIGNATURES_SIZE      (61U)
#  define PROD_SIGNATURES_PAGE_SIZE (64U)
#endif
#define PROD_SIGNATURES_END       (PROD_SIGNATURES_START + PROD_SIGNATURES_SIZE - 1)

#if (defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__))
#  define FUSES_START     (0x1280)
#  define FUSES_SIZE      (10)
#  define FUSES_PAGE_SIZE (32)
#else
#  define FUSES_START     (0x1280U)
#  define FUSES_SIZE      (10U)
#  define FUSES_PAGE_SIZE (32U)
#endif
#define FUSES_END       (FUSES_START + FUSES_SIZE - 1)

#if (defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__))
#  define LOCKBITS_START     (0x128A)
#  define LOCKBITS_SIZE      (1)
#  define LOCKBITS_PAGE_SIZE (32)
#else
#  define LOCKBITS_START     (0x128AU)
#  define LOCKBITS_SIZE      (1U)
#  define LOCKBITS_PAGE_SIZE (32U)
#endif
#define LOCKBITS_END       (LOCKBITS_START + LOCKBITS_SIZE - 1)

#if (defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__))
#  define USER_SIGNATURES_START     (0x1300)
#  define USER_SIGNATURES_SIZE      (32)
#  define USER_SIGNATURES_PAGE_SIZE (32)
#else
#  define USER_SIGNATURES_START     (0x1300U)
#  define USER_SIGNATURES_SIZE      (32U)
#  define USER_SIGNATURES_PAGE_SIZE (32U)
#endif
#define USER_SIGNATURES_END       (USER_SIGNATURES_START + USER_SIGNATURES_SIZE - 1)

#if (defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__))
#  define EEPROM_START     (0x1400)
#  define EEPROM_SIZE      (128)
#  define EEPROM_PAGE_SIZE (32)
#else
#  define EEPROM_START     (0x1400U)
#  define EEPROM_SIZE      (128U)
#  define EEPROM_PAGE_SIZE (32U)
#endif
#define EEPROM_END       (EEPROM_START + EEPROM_SIZE - 1)

/* Added MAPPED_EEPROM segment names for avr-libc */
#define MAPPED_EEPROM_START     (EEPROM_START)
#define MAPPED_EEPROM_SIZE      (EEPROM_SIZE)
#define MAPPED_EEPROM_PAGE_SIZE (EEPROM_PAGE_SIZE)
#define MAPPED_EEPROM_END       (MAPPED_EEPROM_START + MAPPED_EEPROM_SIZE - 1)

#if (defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__))
#  define INTERNAL_SRAM_START     (0x3E00)
#  define INTERNAL_SRAM_SIZE      (512)
#  define INTERNAL_SRAM_PAGE_SIZE (0)
#else
#  define INTERNAL_SRAM_START     (0x3E00U)
#  define INTERNAL_SRAM_SIZE      (512U)
#  define INTERNAL_SRAM_PAGE_SIZE (0U)
#endif
#define INTERNAL_SRAM_END       (INTERNAL_SRAM_START + INTERNAL_SRAM_SIZE - 1)

#if (defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__))
#  define MAPPED_PROGMEM_START     (0x8000)
#  define MAPPED_PROGMEM_SIZE      (8192)
#  define MAPPED_PROGMEM_PAGE_SIZE (64)
#else
#  define MAPPED_PROGMEM_START     (0x8000U)
#  define MAPPED_PROGMEM_SIZE      (8192U)
#  define MAPPED_PROGMEM_PAGE_SIZE (64U)
#endif
#define MAPPED_PROGMEM_END       (MAPPED_PROGMEM_START + MAPPED_PROGMEM_SIZE - 1)

#if (defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__))
#  define PROGMEM_START     (0x0000)
#  define PROGMEM_SIZE      (8192)
#  define PROGMEM_PAGE_SIZE (64)
#else
#  define PROGMEM_START     (0x0000U)
#  define PROGMEM_SIZE      (8192U)
#  define PROGMEM_PAGE_SIZE (64U)
#endif
#define PROGMEM_END       (PROGMEM_START + PROGMEM_SIZE - 1)

#define FLASHSTART   PROGMEM_START
#define FLASHEND     PROGMEM_END
#define RAMSTART     INTERNAL_SRAM_START
#define RAMSIZE      INTERNAL_SRAM_SIZE
#define RAMEND       INTERNAL_SRAM_END
#define E2END        EEPROM_END
#define E2PAGESIZE   EEPROM_PAGE_SIZE

/* ========== Fuses ========== */
#define FUSE_MEMORY_SIZE 10

/* Fuse Byte 0 (WDTCFG) */
#define FUSE_PERIOD0  (unsigned char)_BV(0)  /* Watchdog Timeout Period Bit 0 */
#define FUSE_PERIOD1  (unsigned char)_BV(1)  /* Watchdog Timeout Period Bit 1 */
#define FUSE_PERIOD2  (unsigned char)_BV(2)  /* Watchdog Timeout Period Bit 2 */
#define FUSE_PERIOD3  (unsigned char)_BV(3)  /* Watchdog Timeout Period Bit 3 */
#define FUSE_WINDOW0  (unsigned char)_BV(4)  /* Watchdog Window Timeout Period Bit 0 */
#define FUSE_WINDOW1  (unsigned char)_BV(5)  /* Watchdog Window Timeout Period Bit 1 */
#define FUSE_WINDOW2  (unsigned char)_BV(6)  /* Watchdog Window Timeout Period Bit 2 */
#define FUSE_WINDOW3  (unsigned char)_BV(7)  /* Watchdog Window Timeout Period Bit 3 */
#define FUSE0_DEFAULT  (0x0)
#define FUSE_WDTCFG_DEFAULT  (0x0)

/* Fuse Byte 1 (BODCFG) */
#define FUSE_SLEEP0  (unsigned char)_BV(0)  /* BOD Operation in Sleep Mode Bit 0 */
#define FUSE_SLEEP1  (unsigned char)_BV(1)  /* BOD Operation in Sleep Mode Bit 1 */
#define FUSE_ACTIVE0  (unsigned char)_BV(2)  /* BOD Operation in Active Mode Bit 0 */
#define FUSE_ACTIVE1  (unsigned char)_BV(3)  /* BOD Operation in Active Mode Bit 1 */
#define FUSE_SAMPFREQ  (unsigned char)_BV(4)  /* BOD Sample Frequency */
#define FUSE_LVL0  (unsigned char)_BV(5)  /* BOD Level Bit 0 */
#define FUSE_LVL1  (unsigned char)_BV(6)  /* BOD Level Bit 1 */
#define FUSE_LVL2  (unsigned char)_BV(7)  /* BOD Level Bit 2 */
#define FUSE1_DEFAULT  (0x0)
#define FUSE_BODCFG_DEFAULT  (0x0)

/* Fuse Byte 2 (OSCCFG) */
#define FUSE_FREQSEL0  (unsigned char)_BV(0)  /* Frequency Select Bit 0 */
#define FUSE_FREQSEL1  (unsigned char)_BV(1)  /* Frequency Select Bit 1 */
#define FUSE_OSCLOCK  (unsigned char)_BV(7)  /* Oscillator Lock */
#define FUSE2_DEFAULT  (0x7E)
#define FUSE_OSCCFG_DEFAULT  (0x7E)

/* Fuse Byte 3 Reserved */

/* Fuse Byte 4 Reserved */

/* Fuse Byte 5 (SYSCFG0) */
#define FUSE_EESAVE  (unsigned char)_BV(0)  /* EEPROM Save */
#define FUSE_RSTPINCFG0  (unsigned char)_BV(2)  /* Reset Pin Configuration Bit 0 */
#define FUSE_RSTPINCFG1  (unsigned char)_BV(3)  /* Reset Pin Configuration Bit 1 */
#define FUSE_CRCSRC0  (unsigned char)_BV(6)  /* CRC Source Bit 0 */
#define FUSE_CRCSRC1  (unsigned char)_BV(7)  /* CRC Source Bit 1 */
#define FUSE5_DEFAULT  (0xF6)
#define FUSE_SYSCFG0_DEFAULT  (0xF6)

/* Fuse Byte 6 (SYSCFG1) */
#define FUSE_SUT0  (unsigned char)_BV(0)  /* Startup Time Bit 0 */
#define FUSE_SUT1  (unsigned char)_BV(1)  /* Startup Time Bit 1 */
#define FUSE_SUT2  (unsigned char)_BV(2)  /* Startup Time Bit 2 */
#define FUSE6_DEFAULT  (0xFF)
#define FUSE_SYSCFG1_DEFAULT  (0xFF)

/* Fuse Byte 7 (APPEND) */
#define FUSE7_DEFAULT  (0x0)
#define FUSE_APPEND_DEFAULT  (0x0)

/* Fuse Byte 8 (BOOTEND) */
#define FUSE8_DEFAULT  (0x0)
#define FUSE_BOOTEND_DEFAULT  (0x0)

/* ========== Lock Bits ========== */
#define __LOCK_BITS_EXIST
#ifdef LOCKBITS_DEFAULT
#undef LOCKBITS_DEFAULT
#endif //LOCKBITS_DEFAULT
#define LOCKBITS_DEFAULT  (0xC5)

/* ========== Signature ========== */
#define SIGNATURE_0 0x1E
#define SIGNATURE_1 0x93
#define SIGNATURE_2 0x24

#endif /* #ifdef _AVR_ATTINY806_H_INCLUDED */

