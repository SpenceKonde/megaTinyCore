/*
 * SPI Master library for Arduino Zero.
 * Copyright (c) 2015 Arduino LLC
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
#define SPI_HAS_TRANSACTION 1

// SPI_HAS_NOTUSINGINTERRUPT means that SPI has notUsingInterrupt() method
#define SPI_HAS_NOTUSINGINTERRUPT 1

#define SPI_MODE0           ( SPI_MODE_0_gc )
#define SPI_MODE1           ( SPI_MODE_1_gc )
#define SPI_MODE2           ( SPI_MODE_2_gc )
#define SPI_MODE3           ( SPI_MODE_3_gc )

#define SPI_MODE_MASK       ( SPI_MODE_gm   )
#define SPI_CLOCK_MASK      ( SPI_PRESC_gm  )
#define SPI_2XCLOCK_MASK    ( SPI_CLK2X_bm  )

#define SPI_INTERRUPT_DISABLE     0
#define SPI_INTERRUPT_ENABLE      1

#ifndef EXTERNAL_NUM_INTERRUPTS
#define EXTERNAL_NUM_INTERRUPTS   NUM_TOTAL_PINS
#endif

class SPISettings {
  public:
  SPISettings(uint32_t clock, BitOrder bitOrder, uint8_t dataMode) {
    if (__builtin_constant_p(clock)) {
      init_AlwaysInline(clock, bitOrder, dataMode);
    } else {
      init_MightInline(clock, bitOrder, dataMode);
    }
  }

  // Default speed set to 4MHz, SPI mode set to MODE 0 and Bit order set to MSB first.
  SPISettings() { init_AlwaysInline(4000000, MSBFIRST, SPI_MODE0); }

  private:
  void init_MightInline(uint32_t clock, BitOrder bitOrder, uint8_t dataMode) {
    init_AlwaysInline(clock, bitOrder, dataMode);
  }

  void init_AlwaysInline(uint32_t clock, BitOrder bitOrder, uint8_t dataMode) __attribute__((__always_inline__)) {
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
    // given clock rate. The clock divider that results in clock_setting
    // is 2 ^^ (clock_div + 1). If nothing is slow enough, we'll use the
    // slowest (128 == 2 ^^ 7, so clock_div = 6).
    uint8_t clockDiv;
    
    // When the clock is known at compiletime, use this if-then-else
    // cascade, which the compiler knows how to completely optimize
    // away. When clock is not known, use a loop instead, which generates
    // shorter code.
    
    /*  This is no longer the case since, F_CPU_CORRECTED is variable */
    /*  set at run time.                                             */

    uint32_t clockSetting = 0; 
    
    clockSetting = F_CPU_CORRECTED / 2;
    clockDiv = 0;
    while ((clockDiv < 6) && (clock < clockSetting)) {
      clockSetting /= 2;
      clockDiv++;
    }
        
    // Compensate for the duplicate fosc/64,
    // should be fosc/128 if clockdiv 6.
    if (clockDiv == 6)
    {
      clockDiv++;
    }
        
    // Invert the SPI2X bit
    clockDiv ^= 0x1;
        
    /* Pack into the SPISettings::ctrlb class */
    /* Set mode, disable master slave select, and disable buffering. */
    /* dataMode is register correct, when using SPI_MODE defines     */
    ctrlb = (dataMode)            |
            (SPI_SSD_bm)          |
            (0 << SPI_BUFWR_bp)   |
            (0 << SPI_BUFEN_bp);
                
    /* Get Clock related values.*/
    uint8_t clockDiv_mult = (clockDiv & 0x1);
    uint8_t clockDiv_pres = (clockDiv >> 1);
        
    /* Pack into the SPISettings::ctrlb class     */
    /* Set Prescaler, x2, SPI to Master, and Bit Order. */
    
    ctrla = (clockDiv_pres  << SPI_PRESC_gp)        |
            (clockDiv_mult << SPI_CLK2X_bp)         |
            (SPI_ENABLE_bm)                         |
            (SPI_MASTER_bm)                         |
            ((bitOrder == LSBFIRST) << SPI_DORD_bp);
    
  }
  /* member variables containing the desired SPI settings */
  uint8_t ctrla;
  uint8_t ctrlb;
  friend class SPIClass;
};

class SPIClass {
  public:
  SPIClass(uint8_t uc_pinMISO, uint8_t uc_pinSCK, uint8_t uc_pinMOSI, uint8_t uc_pinSS, uint8_t uc_mux);

  byte transfer(uint8_t data);
  uint16_t transfer16(uint16_t data);
  void transfer(void *buf, size_t count);

  // Transaction Functions
  void usingInterrupt(int interruptNumber);
  void notUsingInterrupt(int interruptNumber);
  void beginTransaction(SPISettings settings);
  void endTransaction(void);

  void begin();
  void end();

  void setBitOrder(BitOrder order);
  void setDataMode(uint8_t uc_mode);
  void setClockDivider(uint8_t uc_div);

  private:

  void init();
  void config(SPISettings settings);

  // These undocumented functions should not be used.  SPI.transfer()
  // polls the hardware flag which is automatically cleared as the
  // AVR responds to SPI's interrupt
  inline static void attachInterrupt() { SPI0.INTCTRL |= (SPI_IE_bm); }
  inline static void detachInterrupt() { SPI0.INTCTRL &= ~(SPI_IE_bm); }

  void detachMaskedInterrupts();
  void reattachMaskedInterrupts();

  uint8_t _uc_pinMiso;
  uint8_t _uc_pinMosi;
  uint8_t _uc_pinSCK;
  uint8_t _uc_pinSS;
  uint8_t _uc_mux;

  bool initialized;
  uint8_t interruptMode;
  char interruptSave;
  uint32_t interruptMask_lo;
  uint32_t interruptMask_hi;

  #if USE_MALLOC_FOR_IRQ_MAP
    uint8_t* irqMap = NULL;
  #else
    volatile uint8_t irqMap[EXTERNAL_NUM_INTERRUPTS];
  #endif
};


#if SPI_INTERFACES_COUNT > 0
  extern SPIClass SPI;
#endif

#define SPI_CLOCK_DIV2      ( SPI_PRESC_DIV4_gc     | SPI_CLK2X_bm  )
#define SPI_CLOCK_DIV4      ( SPI_PRESC_DIV4_gc                     )
#define SPI_CLOCK_DIV8      ( SPI_PRESC_DIV16_gc    | SPI_CLK2X_bm  )
#define SPI_CLOCK_DIV16     ( SPI_PRESC_DIV16_gc                    )
#define SPI_CLOCK_DIV32     ( SPI_PRESC_DIV64_gc    | SPI_CLK2X_bm  )
#define SPI_CLOCK_DIV64     ( SPI_PRESC_DIV64_gc                    )
#define SPI_CLOCK_DIV128    ( SPI_PRESC_DIV128_gc                   )

#endif