/*
 * SPI Master library for Arduino Zero.
 * Copyright (c) 2015 Arduino LLC
 * With modification 2016-2022 Spence Konde for megaTinyCore and DxCore.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef _SPI_H_INCLUDED
#define _SPI_H_INCLUDED

#include <Arduino.h>


#ifndef USE_MALLOC_FOR_IRQ_MAP
  #define USE_MALLOC_FOR_IRQ_MAP  0
#endif

// SPI_HAS_TRANSACTION means SPI has
//   - beginTransaction()
//   - endTransaction()
//   - usingInterrupt()
//   - SPISetting(clock, bitOrder, dataMode)

#ifndef SPI_HAS_TRANSACTION
  #define SPI_HAS_TRANSACTION 1
#endif

// SPI_HAS_NOTUSINGINTERRUPT means that SPI has notUsingInterrupt() method
#ifndef SPI_HAS_NOTUSINGINTERRUPT
  #define SPI_HAS_NOTUSINGINTERRUPT 1
#endif

#ifndef SPI_MODE0
  #define SPI_MODE0           ((SPI_SSD_bm) | SPI_MODE_0_gc)
#endif
#ifndef SPI_MODE1
  #define SPI_MODE1           ((SPI_SSD_bm) | SPI_MODE_1_gc)
#endif
#ifndef SPI_MODE2
  #define SPI_MODE2           ((SPI_SSD_bm) | SPI_MODE_2_gc)
#endif
#ifndef SPI_MODE3
  #define SPI_MODE3           ((SPI_SSD_bm) | SPI_MODE_3_gc)
#endif

#ifndef SPI_MODE_MASK
  #define SPI_MODE_MASK       (SPI_MODE_gm )
#endif
#ifndef SPI_CLOCK_MASK
  #define SPI_CLOCK_MASK      (SPI_PRESC_gm)
#endif
#ifndef SPI_2XCLOCK_MASK
  #define SPI_2XCLOCK_MASK    (SPI_CLK2X_bm)
#endif

#ifndef SPI_INTERRUPT_DISABLE
  #define SPI_INTERRUPT_DISABLE     0
#endif
#ifndef SPI_INTERRUPT_ENABLE
  #define SPI_INTERRUPT_ENABLE      1
#endif


//#define EXTERNAL_NUM_INTERRUPTS   NUM_TOTAL_PINS


class SPISettings {
  public:
    SPISettings(uint32_t clock, uint8_t bitOrder, uint8_t dataMode) {
      if (__builtin_constant_p(clock)) {
        init_AlwaysInline(clock, bitOrder, dataMode);
      } else {
        init_MightInline(clock, bitOrder, dataMode);
      }
    }

    // Default speed set to 4MHz, SPI mode set to MODE 0 and Bit order set to MSB first.
    SPISettings() {
      init_AlwaysInline(4000000, MSBFIRST, SPI_MODE0);
    }

  private:
    void init_MightInline(uint32_t clock, uint8_t bitOrder, uint8_t dataMode) {
      init_AlwaysInline(clock, bitOrder, dataMode);
    }

    void init_AlwaysInline(uint32_t clock, uint8_t bitOrder, uint8_t dataMode) __attribute__((__always_inline__)) {
      // Clock settings are defined as follows. Note that this shows SPI2X
      // inverted, so the bits form increasing numbers. Also note that
      // fosc/64 appears twice.  If FOSC is 16 Mhz
      // PRESC[1:0]  ~SPI2X Freq
      //   0    0     0   fosc/2    8.00 MHz
      //   0    0     1   fosc/4    4.00 MHz
      //   0    1     0   fosc/8    2.00 MHz
      //   0    1     1   fosc/16   1.00 MHz
      //   1    0     0   fosc/32   500  kHz
      //   1    0     1   fosc/64   250  kHz
      //   1    1     0   fosc/64   250  kHz
      //   1    1     1   fosc/128  125  kHz

      // We find the fastest clock that is less than or equal to the
      // request  clock rate. The clock divider that results in clock_setting
      // is 2 ^^ (clock_div + 1). If nothing is slow enough, we'll use the
      // slowest (128 == 2 ^^ 7, so clock_div = 6).
      uint8_t clockDiv;

      // When the clock is known at compile time, use this if-then-else
      // cascade, which the compiler knows how to completely optimize
      // away. When clock is not known, use a loop instead, which generates
      // shorter code.

      /*  This is no longer the case since, F_CPU_CORRECTED is variable */
      /*  set at run time.                                             */

      uint32_t clockSetting = 0;

      clockSetting = F_CPU >> 1;
      clockDiv = 0;
      while ((clockDiv < 6) && (clock < clockSetting)) {
        clockSetting >>= 1;
        clockDiv++;
      }

      // Compensate for the duplicate fosc/64,
      // should be fosc/128 if clockdiv 6.
      if (clockDiv == 6) {
        clockDiv++;
      }

      // Invert the SPI2X bit
      clockDiv ^= 0x1;

      /* Pack into the SPISettings::ctrlb class */
      /* Set mode, disable master slave select, and disable buffering.   */
      /* dataMode is register correct, when using SPI_MODE defines only! */
      /* they have had SSD added to the modebits. That permits the SPI   */
      /* library to coeexist with code that uses the SPI as a slave,     */
      /* if and only if the SPI_MODEn named constants are used           */
      ctrlb = (dataMode) | (SPI_SSD_bm);
      //      | (0 << SPI_BUFWR_bp)
      //      | (0 << SPI_BUFEN_bp);

      /* Get Clock related values.*/
      uint8_t clockDiv_mult = (clockDiv & 0x1);
      uint8_t clockDiv_pres = (clockDiv >> 1);

      /* Pack into the SPISettings::ctrlb class     */
      /* Set Prescaler, x2, SPI to Master, and Bit Order. */

      ctrla = (clockDiv_pres << SPI_PRESC_gp)         |
              (clockDiv_mult << SPI_CLK2X_bp)         |
              (SPI_ENABLE_bm)                         |
              (SPI_MASTER_bm)                         |
              ((bitOrder == LSBFIRST) ? SPI_DORD_bm : 0);

    }
    /* member variables containing the desired SPI settings */
    uint8_t ctrla;
    uint8_t ctrlb;
    friend class SPIClass;
};

class SPIClass {
  public:
    SPIClass();

    byte transfer(uint8_t data);
    uint16_t transfer16(uint16_t data);
    void transfer(void *buf, size_t count);

    // Transaction Functions
    void usingInterrupt(uint8_t interruptNumber);
    void notUsingInterrupt(uint8_t interruptNumber);
    void beginTransaction(SPISettings settings);
    void endTransaction(void);

    bool pins(uint8_t pinMOSI, uint8_t pinMISO, uint8_t pinSCK, uint8_t pinSS = NOT_A_PIN);
    bool swap(uint8_t state = 1);
    void begin();
    void end();

    void setBitOrder(uint8_t order);
    void setDataMode(uint8_t uc_mode);
    void setClockDivider(uint8_t uc_div);

  private:
    void init();
    void config(SPISettings settings);

    // These undocumented functions should not be used.  SPI.transfer()
    // polls the hardware flag which is automatically cleared as the
    // AVR responds to SPI's interrupt.
    // Well, they're now commented out too. there is no way anything good could come of these and no indication that they were ever anything more than useless stubs.
    /*
    inline static void attachInterrupt() {
      SPI0.INTCTRL |= (SPI_IE_bm);
    }
    inline static void detachInterrupt() {
      SPI0.INTCTRL &= ~(SPI_IE_bm);
    }
    */
    #ifdef CORE_ATTACH_OLD
    void detachMaskedInterrupts();
    void reattachMaskedInterrupts();
    #endif
    SPI_t *_hwspi_module = &SPI0;
    uint8_t _uc_pinMiso;
    uint8_t _uc_pinMosi;
    uint8_t _uc_pinSCK;
    uint8_t _uc_pinSS;
    uint8_t _uc_mux;

    bool initialized;
    uint8_t interruptMode;
    #ifdef CORE_ATTACH_OLD
    char interruptSave;
    uint32_t interruptMask_lo;
    uint32_t interruptMask_hi;
    #if USE_MALLOC_FOR_IRQ_MAP
    uint8_t *irqMap = NULL;
    #else
    volatile uint8_t irqMap[EXTERNAL_NUM_INTERRUPTS];
    #endif
    #else
    uint8_t old_sreg;
    uint8_t in_transaction;
    #endif
};


#if SPI_INTERFACES_COUNT > 0
  extern SPIClass SPI;
#endif

#ifndef SPI_CLOCK_DIV2
  #define SPI_CLOCK_DIV2      (SPI_PRESC_DIV4_gc     | SPI_CLK2X_bm  )
#endif
#ifndef SPI_CLOCK_DIV4
  #define SPI_CLOCK_DIV4      (SPI_PRESC_DIV4_gc                     )
#endif
#ifndef SPI_CLOCK_DIV8
  #define SPI_CLOCK_DIV8      (SPI_PRESC_DIV16_gc    | SPI_CLK2X_bm  )
#endif
#ifndef SPI_CLOCK_DIV16
  #define SPI_CLOCK_DIV16     (SPI_PRESC_DIV16_gc                    )
#endif
#ifndef SPI_CLOCK_DIV32
  #define SPI_CLOCK_DIV32     (SPI_PRESC_DIV64_gc    | SPI_CLK2X_bm  )
#endif
#ifndef SPI_CLOCK_DIV64
  #define SPI_CLOCK_DIV64     (SPI_PRESC_DIV64_gc                    )
#endif
#ifndef SPI_CLOCK_DIV128
  #define SPI_CLOCK_DIV128    (SPI_PRESC_DIV128_gc                   )
#endif

#endif
