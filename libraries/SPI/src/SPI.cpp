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

#include "SPI.h"
#include <Arduino.h>

#define SPI_IMODE_NONE   0
#define SPI_IMODE_EXTINT 1
#define SPI_IMODE_GLOBAL 2

const SPISettings DEFAULT_SPI_SETTINGS = SPISettings();

SPIClass::SPIClass(uint8_t uc_pinMISO, uint8_t uc_pinSCK, uint8_t uc_pinMOSI, uint8_t uc_pinSS, uint8_t uc_mux)
{
  initialized = false;

  // pins
  _uc_mux = uc_mux;
  _uc_pinMiso = uc_pinMISO;
  _uc_pinSCK = uc_pinSCK;
  _uc_pinMosi = uc_pinMOSI;
  _uc_pinSS = uc_pinSS;
}

void SPIClass::begin()
{
  init();

  PORTMUX.TWISPIROUTEA |= _uc_mux;

  pinMode(_uc_pinMosi, OUTPUT);
  pinMode(_uc_pinSCK, OUTPUT);
  // MISO is set to input by the controller

  // We don't need HW SS since salve/master mode is selected via registers
  SPI0.CTRLB |= (SPI_SSD_bm);
  SPI0.CTRLA |= (SPI_ENABLE_bm | SPI_MASTER_bm);

  config(DEFAULT_SPI_SETTINGS);
}

void SPIClass::init()
{
  if (initialized)
    return;
  interruptMode = SPI_IMODE_NONE;
  interruptSave = 0;
  interruptMask_lo = 0;
  interruptMask_hi = 0;
  initialized = true;
}

void SPIClass::config(SPISettings settings)
{
  SPI0.CTRLA = settings.ctrla;
  SPI0.CTRLB = settings.ctrlb;
}

void SPIClass::end()
{
  SPI0.CTRLA &= ~(SPI_ENABLE_bm);
  initialized = false;
}

void SPIClass::usingInterrupt(int interruptNumber)
{
  if ((interruptNumber == NOT_AN_INTERRUPT))
    return;

  if (interruptNumber >= EXTERNAL_NUM_INTERRUPTS)
    interruptMode = SPI_IMODE_GLOBAL;
  else
  {
    #if USE_MALLOC_FOR_IRQ_MAP
      if (irqMap == NULL) {
        irqMap = (uint8_t*)malloc(EXTERNAL_NUM_INTERRUPTS);
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

void SPIClass::notUsingInterrupt(int interruptNumber)
{
  if ((interruptNumber == NOT_AN_INTERRUPT))
    return;

  if (interruptMode & SPI_IMODE_GLOBAL)
    return; // can't go back, as there is no reference count

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
      volatile uint8_t* pin_ctrl_reg = getPINnCTRLregister(portToPortStruct(shift/8), shift%8);
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
      volatile uint8_t* pin_ctrl_reg = getPINnCTRLregister(portToPortStruct(shift/8), shift%8);
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
      volatile uint8_t* pin_ctrl_reg = getPINnCTRLregister(portToPortStruct(shift/8), shift%8);
      *pin_ctrl_reg |= irqMap[shift];
    }
    temp = temp >> 1;
    shift++;
  }
  temp = interruptMask_hi;
  shift = 32;
  while (temp != 0) {
    if (temp & 1) {
      volatile uint8_t* pin_ctrl_reg = getPINnCTRLregister(portToPortStruct(shift/8), shift%8);
      *pin_ctrl_reg |= irqMap[shift];
    }
    temp = temp >> 1;
    shift++;
  }
}

void SPIClass::beginTransaction(SPISettings settings)
{
  if (interruptMode != SPI_IMODE_NONE)
  {
    if (interruptMode & SPI_IMODE_GLOBAL)
    {
      noInterrupts();
    }
    else if (interruptMode & SPI_IMODE_EXTINT)
    {
      detachMaskedInterrupts();
    }
  }
  config(settings);
}

void SPIClass::endTransaction(void)
{
  if (interruptMode != SPI_IMODE_NONE)
  {
    if (interruptMode & SPI_IMODE_GLOBAL)
    {
        interrupts();
    }
    else if (interruptMode & SPI_IMODE_EXTINT)
      reattachMaskedInterrupts();
  }
}

void SPIClass::setBitOrder(BitOrder order)
{
  if (order == LSBFIRST)
    SPI0.CTRLA |=  (SPI_DORD_bm);
  else 
    SPI0.CTRLA &= ~(SPI_DORD_bm);
}

void SPIClass::setDataMode(uint8_t mode)
{
  SPI0.CTRLB = ((SPI0.CTRLB & (~SPI_MODE_gm)) | mode );
}

void SPIClass::setClockDivider(uint8_t div)
{
  SPI0.CTRLA = ((SPI0.CTRLA & 
                  ((~SPI_PRESC_gm) | (~SPI_CLK2X_bm) ))  // mask out values
                  | div);                           // write value 
}

byte SPIClass::transfer(uint8_t data)
{
  /*
  * The following NOP introduces a small delay that can prevent the wait
  * loop from iterating when running at the maximum speed. This gives
  * about 10% more speed, even if it seems counter-intuitive. At lower
  * speeds it is unnoticed.
  */
  asm volatile("nop");

  SPI0.DATA = data;
  while ((SPI0.INTFLAGS & SPI_RXCIF_bm) == 0);  // wait for complete send
  return SPI0.DATA;                             // read data back
}

uint16_t SPIClass::transfer16(uint16_t data) {
  union { uint16_t val; struct { uint8_t lsb; uint8_t msb; }; } t;

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

void SPIClass::transfer(void *buf, size_t count)
{
  uint8_t *buffer = reinterpret_cast<uint8_t *>(buf);
  for (size_t i=0; i<count; i++) {
    *buffer = transfer(*buffer);
    buffer++;
  }
}

#if SPI_INTERFACES_COUNT > 0
  SPIClass SPI (PIN_SPI_MISO,  PIN_SPI_SCK,  PIN_SPI_MOSI,  PIN_SPI_SS,  MUX_SPI);
#endif