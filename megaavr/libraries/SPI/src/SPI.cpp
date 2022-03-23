/*
   SPI Master library for Arduino Zero.
   Copyright (c) 2015 Arduino LLC

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

#include "SPI.h"
#include <Arduino.h>

#define SPI_IMODE_NONE   0
#define SPI_IMODE_EXTINT 1
#define SPI_IMODE_GLOBAL 2

const SPISettings DEFAULT_SPI_SETTINGS = SPISettings();

SPIClass::SPIClass() {
  initialized = false;

}

bool SPIClass::pins(__attribute__((unused))uint8_t pinMOSI, __attribute__((unused)) uint8_t pinMISO, __attribute__((unused)) uint8_t pinSCK, __attribute__((unused)) uint8_t pinSS) {
  #if defined(PORTMUX_CTRLB)
  #if (defined(PIN_SPI_MOSI_PINSWAP_1) && defined(PIN_SPI_MISO_PINSWAP_1) && defined(PIN_SPI_SCK_PINSWAP_1))
  if (pinMOSI == PIN_SPI_MOSI_PINSWAP_1 && pinMISO == PIN_SPI_MISO_PINSWAP_1 && pinSCK == PIN_SPI_SCK_PINSWAP_1 /* && pinSS == PIN_SPI_SS_PINSWAP_1 */) {
    _uc_mux = PORTMUX_SPI0_bm;
    return true;
  } else if (pinMOSI == PIN_SPI_MOSI && pinMISO == PIN_SPI_MISO && pinSCK == PIN_SPI_SCK /*&& pinSS == PIN_SPI_SS */) {
    _uc_mux = 0;
    return true;
  } else {
    _uc_mux = 0;
    return false;
  }
  #endif
  #elif defined(PORTMUX_SPIROUTEA)
  #if (defined(PIN_SPI_MOSI_PINSWAP_2) && defined(PIN_SPI_MISO_PINSWAP_2) && defined(PIN_SPI_SCK_PINSWAP_2))
  if (pinMOSI == PIN_SPI_MOSI_PINSWAP_2 && pinMISO == PIN_SPI_MISO_PINSWAP_2 && pinSCK == PIN_SPI_SCK_PINSWAP_2 /* && pinSS == PIN_SPI_SS_PINSWAP_2 */) {
    _uc_mux = 2;
    return true;
  } else
  #endif
  #if (defined(PIN_SPI_MOSI_PINSWAP_1) && defined(PIN_SPI_MISO_PINSWAP_1) && defined(PIN_SPI_SCK_PINSWAP_1))
    if (pinMOSI == PIN_SPI_MOSI_PINSWAP_1 && pinMISO == PIN_SPI_MISO_PINSWAP_1 && pinSCK == PIN_SPI_SCK_PINSWAP_1 /* && pinSS == PIN_SPI_SS_PINSWAP_1 */) {
      _uc_mux = 1;
      return true;
    } else if (pinMOSI == PIN_SPI_MOSI && pinMISO == PIN_SPI_MISO && pinSCK == PIN_SPI_SCK /* && pinSS == PIN_SPI_SS */) {
      _uc_mux = 0;
      return true;
    } else {
      _uc_mux = 0;
      return false;
    }
  #endif
  #endif
  return false;
}
bool SPIClass::swap(__attribute__((unused))uint8_t state) {
  #if defined(PORTMUX_CTRLB)
  #if (defined(PIN_SPI_MOSI_PINSWAP_1) && defined(PIN_SPI_MISO_PINSWAP_1) && defined(PIN_SPI_SCK_PINSWAP_1))
  if (state == 1) {
    _uc_mux = PORTMUX_SPI0_bm;
    return true;
  } else if (state == 0) {
    _uc_mux = 0;
    return true;
  } else {
    _uc_mux = 0;
    return false;
  }
  #endif
  #elif defined(PORTMUX_SPIROUTEA)
  #if (defined(PIN_SPI_MOSI_PINSWAP_2) && defined(PIN_SPI_MISO_PINSWAP_2) && defined(PIN_SPI_SCK_PINSWAP_2))
  if (state == 2) {
    _uc_mux = 2;
    return true;
  } else
  #endif
    if (state == 1) {
      _uc_mux = 1;
      return true;
    } else if (state == 0) {
      _uc_mux = 0;
      return true;
    } else {
      _uc_mux = 0;
      return false;
    }

  #endif
  return false;
}

void SPIClass::begin() {
  init();
  #if defined(PORTMUX_CTRLB)
  PORTMUX.CTRLB = _uc_mux | (PORTMUX.CTRLB & ~PORTMUX_SPI0_bm);
  #elif defined(PORTMUX_SPIROUTEA)
  PORTMUX.SPIROUTEA = _uc_mux | (PORTMUX.SPIROUTEA & ~3);
  #endif

  #if ((defined(PIN_SPI_MOSI_PINSWAP_1) && defined(PIN_SPI_MISO_PINSWAP_1) && defined(PIN_SPI_SCK_PINSWAP_1)) || (defined(PIN_SPI_MOSI_PINSWAP_2) && defined(PIN_SPI_MISO_PINSWAP_2) && defined(PIN_SPI_SCK_PINSWAP_2)))
  if (_uc_mux == 0) {
    pinMode(PIN_SPI_MOSI, OUTPUT);
    pinMode(PIN_SPI_SCK, OUTPUT);
  }
  #if (defined(PIN_SPI_MOSI_PINSWAP_1) && defined(PIN_SPI_MISO_PINSWAP_1) && defined(PIN_SPI_SCK_PINSWAP_1))
  #ifdef PORTMUX_CTRLB
  else if (_uc_mux == PORTMUX_SPI0_bm)
  #else
  else if (_uc_mux == 1)
  #endif
  {
    pinMode(PIN_SPI_MOSI_PINSWAP_1, OUTPUT);
    pinMode(PIN_SPI_SCK_PINSWAP_1, OUTPUT);
  }
  #endif
  #if (defined(PIN_SPI_MOSI_PINSWAP_2) && defined(PIN_SPI_MISO_PINSWAP_2) && defined(PIN_SPI_SCK_PINSWAP_2))
  else if (_uc_mux == 2) {
    pinMode(PIN_SPI_MOSI_PINSWAP_2, OUTPUT);
    pinMode(PIN_SPI_SCK_PINSWAP_2, OUTPUT);
  }
  #endif
  #else
  // MISO is set to input by the controller
  pinMode(PIN_SPI_MOSI, OUTPUT);
  pinMode(PIN_SPI_SCK, OUTPUT);
  #endif

  SPI0.CTRLB |= (SPI_SSD_bm);
  SPI0.CTRLA |= (SPI_ENABLE_bm | SPI_MASTER_bm);

  config(DEFAULT_SPI_SETTINGS);
}

void SPIClass::init() {
  if (initialized) {
    return;
  }
  interruptMode = SPI_IMODE_NONE;
  #ifdef CORE_ATTACH_OLD
  interruptSave = 0;
  interruptMask_lo = 0;
  interruptMask_hi = 0;
  #else
  in_transaction = 0;
  old_sreg = 0x80;
  #endif
  initialized = true;
}

void SPIClass::config(SPISettings settings) {
  SPI0.CTRLA = settings.ctrla;
  SPI0.CTRLB = settings.ctrlb;
}

void SPIClass::end() {
  SPI0.CTRLA &= ~(SPI_ENABLE_bm);
  initialized = false;
}

#ifdef CORE_ATTACH_OLD
void SPIClass::usingInterrupt(uint8_t interruptNumber) {
  if ((interruptNumber == NOT_AN_INTERRUPT)) {
    return;
  }

  if (interruptNumber >= EXTERNAL_NUM_INTERRUPTS) {
    interruptMode = SPI_IMODE_GLOBAL;
  } else {
    #if USE_MALLOC_FOR_IRQ_MAP
    if (irqMap == NULL) {
      irqMap = (uint8_t *)malloc(EXTERNAL_NUM_INTERRUPTS);
    }
    #endif

    interruptMode |= SPI_IMODE_EXTINT;
    if (interruptNumber < 32) {
      interruptMask_lo |= 1 << interruptNumber;
    } else {
      interruptMask_hi |= 1 << (interruptNumber - 32);
    }
  }
}

void SPIClass::notUsingInterrupt(uint8_t interruptNumber) {
  if ((interruptNumber == NOT_AN_INTERRUPT)) {
    return;
  }

  if (interruptMode & SPI_IMODE_GLOBAL) {
    return;  // can't go back, as there is no reference count
  }

  if (interruptNumber < 32) {
    interruptMask_lo &= ~(1 << interruptNumber);
  } else {
    interruptMask_hi &= ~(1 << (interruptNumber - 32));
  }

  if (interruptMask_lo == 0 && interruptMask_hi == 0) {
    interruptMode = SPI_IMODE_NONE;
    #if USE_MALLOC_FOR_IRQ_MAP
    free(irqMap);
    irqMap = NULL;
    #endif
  }
}

void SPIClass::detachMaskedInterrupts() {
  uint64_t temp = interruptMask_lo;
  uint8_t shift = 0;
  while (temp != 0) {
    if (temp & 1) {
      volatile uint8_t *pin_ctrl_reg = getPINnCTRLregister(portToPortStruct(shift / 8), shift % 8);
      irqMap[shift] = *pin_ctrl_reg;
      *pin_ctrl_reg &= ~(PORT_ISC_gm);
    }
    temp = temp >> 1;
    shift++;
  }
  temp = interruptMask_hi;
  shift = 32;
  while (temp != 0) {
    if (temp & 1) {
      volatile uint8_t *pin_ctrl_reg = getPINnCTRLregister(portToPortStruct(shift / 8), shift % 8);
      irqMap[shift] = *pin_ctrl_reg;
      *pin_ctrl_reg &= ~(PORT_ISC_gm);
    }
    temp = temp >> 1;
    shift++;
  }
}

void SPIClass::reattachMaskedInterrupts() {
  uint64_t temp = interruptMask_lo;
  uint8_t shift = 0;
  while (temp != 0) {
    if (temp & 1) {
      volatile uint8_t *pin_ctrl_reg = getPINnCTRLregister(portToPortStruct(shift / 8), shift % 8);
      *pin_ctrl_reg |= irqMap[shift];
    }
    temp = temp >> 1;
    shift++;
  }
  temp = interruptMask_hi;
  shift = 32;
  while (temp != 0) {
    if (temp & 1) {
      volatile uint8_t *pin_ctrl_reg = getPINnCTRLregister(portToPortStruct(shift / 8), shift % 8);
      *pin_ctrl_reg |= irqMap[shift];
    }
    temp = temp >> 1;
    shift++;
  }
}

void SPIClass::beginTransaction(SPISettings settings) {
  if (interruptMode != SPI_IMODE_NONE) {
    if (interruptMode & SPI_IMODE_GLOBAL) {
      noInterrupts();
    } else if (interruptMode & SPI_IMODE_EXTINT) {
      detachMaskedInterrupts();
    }
  }
  config(settings);
}

void SPIClass::endTransaction(void) {
  if (interruptMode != SPI_IMODE_NONE) {
    if (interruptMode & SPI_IMODE_GLOBAL) {
      interrupts();
    } else if (interruptMode & SPI_IMODE_EXTINT) {
      reattachMaskedInterrupts();
    }
  }
}
#else // End of old interrupt related stuff, start of new-attachInterrupt-compatible implementation.
void SPIClass::usingInterrupt(uint8_t interruptNumber) {
  if ((interruptNumber == NOT_AN_INTERRUPT)) {
    return;
  }
  interruptMode = SPI_IMODE_GLOBAL;
}

void SPIClass::notUsingInterrupt(uint8_t interruptNumber) {
  if ((interruptNumber == NOT_AN_INTERRUPT)) {
    return;
  }
  interruptMode = SPI_IMODE_NONE;
}

void SPIClass::beginTransaction(SPISettings settings) {
  if (interruptMode != SPI_IMODE_NONE) {
    old_sreg = SREG;
    cli(); // NoInterrupts();
  }
  in_transaction = 1;
  config(settings);
}

void SPIClass::endTransaction(void) {
  if (in_transaction) {
    in_transaction = 0;
    if (interruptMode != SPI_IMODE_NONE) {
      SREG = old_sreg;
    }
  }
}
#endif // End new attachInterrupt-compatible implementation.
void SPIClass::setBitOrder(uint8_t order) {
  if (order == LSBFIRST) {
    SPI0.CTRLA |= (SPI_DORD_bm);
  } else {
    SPI0.CTRLA &= ~(SPI_DORD_bm);
  }
}

void SPIClass::setDataMode(uint8_t mode) {
  SPI0.CTRLB = ((SPI0.CTRLB & (~SPI_MODE_gm)) | mode);
}

void SPIClass::setClockDivider(uint8_t div) {
  SPI0.CTRLA = ((SPI0.CTRLA &
                 (~(SPI_PRESC_gm | SPI_CLK2X_bm)))   // mask out values
                | div);                           // write value
}

byte SPIClass::transfer(uint8_t data) {
  /*
    The following NOP introduces a small delay that can prevent the wait
    loop from iterating when running at the maximum speed. This gives
    about 10% more speed, even if it seems counter-intuitive. At lower
    speeds it is unnoticed.
  */
  asm volatile("nop");

  SPI0.DATA = data;
  while ((SPI0.INTFLAGS & SPI_RXCIF_bm) == 0);  // wait for complete send
  return SPI0.DATA;                             // read data back
}

uint16_t SPIClass::transfer16(uint16_t data) {
  union {
    uint16_t val;
    struct {
      uint8_t lsb;
      uint8_t msb;
    };
  } t;

  t.val = data;

  if ((SPI0.CTRLA & SPI_DORD_bm) == 0) {
    t.msb = transfer(t.msb);
    t.lsb = transfer(t.lsb);
  } else {
    t.lsb = transfer(t.lsb);
    t.msb = transfer(t.msb);
  }

  return t.val;
}

void SPIClass::transfer(void *buf, size_t count) {
  uint8_t *buffer = reinterpret_cast<uint8_t *>(buf);
  for (size_t i = 0; i < count; i++) {
    *buffer = transfer(*buffer);
    buffer++;
  }
}

#if SPI_INTERFACES_COUNT > 0
  SPIClass SPI;
#endif
