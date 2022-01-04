/* EEPROM.h - EEPROM library
 *
 * Original Copyright (c) 2006 David A. Mellis.  All right reserved.
 * New version by Christopher Andrews 2015.
 * Ported to post-2016 AVRs by Spence Konde (c) 2018-2021
 * This file is part of DxCore and megaTinyCore.
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

#ifndef EEPROM_h
#define EEPROM_h

#include <inttypes.h>
#include <avr/eeprom.h>
#include <avr/io.h>
#include <Arduino.h>

#define EEPROM_INDEX_MASK (EEPROM_SIZE - 1)

/* Parts with 256b of EEPROM or less are probably short on flash too.
 * So we should try to do what we can to minimize our usage of it.
 * To achieve this, we use a uint8_t instead of a uint16_t to store
 * the index, which results in savings of at least 2 bytes for almost
 * every occasion that one uses an index value in any way.
 */

#if EEPROM_SIZE <= 256
  #define INDEXDATATYPE uint8_t
#else
  #define INDEXDATATYPE uint16_t
#endif


/* EERef class.
 *
 * This object references an EEPROM cell.
 * Its purpose is to mimic a typical byte of RAM, however its storage is the EEPROM.
 * This class has an overhead of two bytes, similar to storing a pointer to an EEPROM cell.
 */


struct EERef {

  EERef(const INDEXDATATYPE index)
    : index(index)                 {}

  // Access/read members.
  uint8_t operator * () const            {
    return (*(uint8_t *)((uint16_t)(MAPPED_EEPROM_START + (index & EEPROM_INDEX_MASK))));
  }

  operator uint8_t() const             {
    return **this;
  }

  // Assignment/write members.
  EERef &operator = (const EERef &ref) {
    return *this = *ref;
  }

  EERef &operator = (uint8_t in)       {
    #ifdef MEGATINYCORE
    // I see no reason why eeprom_write_byte() won't corrupt EEPROM if an ISR tries to write at the wrong instant. The window is 1 clock, but not 0
    uint16_t adr = (uint16_t)MAPPED_EEPROM_START + (index & EEPROM_INDEX_MASK);
    __asm__ __volatile__(
      "ldi r30, 0x00"     "\n\t" // point the Z register at NVMCTRL.
      "ldi r31, 0x10"     "\n\t"
      "in r0, 0x3f"       "\n\t" // read the SREG into r0 to narrow the window between sbrc and cli.
      "ldd r18, Z+2"      "\n\t" // read NVMCTRL.STATUS into r18
      "andi r18, 3"       "\n\t" // if NVMCTRL is busy....
      "brne .-6"          "\n\t" // repeat until it's not.
      "cli"               "\n\t" // disable interrupts. 3 clock window during which an interrupt couldstart write since we checked
      //                            but this just means millis will lose time - nvmctrl halts CPU to finish last write
      "st X, %0"          "\n\t" // write the value we were passed
      "ldi %0, 0x9D"      "\n\t" // CCP signature loaded in it's place
      "out 0x34, %0"      "\n\t" // protection enabled
      "ldi %0, 0x03"      "\n\t" // command loaded: page erase-write.
      "st Z, %0"          "\n\t" // write the page erase-write command to nvmctrl.ctrla
      "out 0x3f, r0"      "\n"   // restore SREG
      :"+d"(in)           // take the value we are writing in any upper register as read/write,
      : "x"(adr)          // and the address (not the index) in X
      : "r30", "r31", "r18");      // clobber Z and r18. We needed an upper register for the temporary value to andi it. I wonder if this will fix the eeprom bugs too?
    return *this;
    #else
    uint8_t oldSREG = SREG;
    while (NVMCTRL.STATUS & NVMCTRL_EEBUSY_bm);
    // check at start - not end. Makes writing single bytes fasterms
    // Note that we only have interrupts disabled for the dozen or so clock cycles
    // during which we *start* the write, not for the while loop, and we save SREG
    // before the while loop. That way there is only a 1 clock window where an
    // interrupt that starts a write will cause this write to halt the CPU
    // which would cause millis() to lose time.
    // Note that writing more than 1 byte in an ISR will **always** cause millis to lose time.
    cli();

    _PROTECTED_WRITE_SPM(NVMCTRL.CTRLA, NVMCTRL_CMD_NONE_gc);
    _PROTECTED_WRITE_SPM(NVMCTRL.CTRLA, NVMCTRL_CMD_EEERWR_gc);
    *(uint8_t *)(MAPPED_EEPROM_START + (index & EEPROM_INDEX_MASK)) = in;

    SREG = oldSREG; // restore SREG and interrupts
    return *this;
    #endif
  }
  EERef &operator += (uint8_t in)     {
    return *this = **this + in;
  }
  EERef &operator -= (uint8_t in)     {
    return *this = **this - in;
  }
  EERef &operator *= (uint8_t in)     {
    return *this = **this * in;
  }
  EERef &operator /= (uint8_t in)     {
    return *this = **this / in;
  }
  EERef &operator ^= (uint8_t in)     {
    return *this = **this ^ in;
  }
  EERef &operator %= (uint8_t in)     {
    return *this = **this % in;
  }
  EERef &operator &= (uint8_t in)     {
    return *this = **this & in;
  }
  EERef &operator |= (uint8_t in)     {
    return *this = **this | in;
  }
  EERef &operator <<= (uint8_t in)    {
    return *this = **this << in;
  }
  EERef &operator >>= (uint8_t in)    {
    return *this = **this >> in;
  }

  EERef &update(uint8_t in)          {
    return  in != *this ? *this = in : *this;
  }

  /* Prefix increment/decrement */
  EERef &operator ++ ()                  {
    return *this += 1;
  }
  EERef &operator -- ()                  {
    return *this -= 1;
  }

  /* Postfix increment/decrement */
  uint8_t operator ++ (int) {
    uint8_t ret = **this;
    return ++(*this), ret;
  }

  uint8_t operator -- (int) {
    uint8_t ret = **this;
    return --(*this), ret;
  }


  INDEXDATATYPE index; // Index of current EEPROM cell.
};

/* EEPtr class.
 *
 * This object is a bidirectional pointer to EEPROM cells represented by EERef objects.
 * Just like a normal pointer type, this can be dereferenced and repositioned using
 * increment/decrement operators.
 */

struct EEPtr {
  EEPtr(const INDEXDATATYPE index)
    : index(index)                    {}

  operator int() const                {
    return index;
  }
  EEPtr &operator = (int in)            {
    return index = in, *this;
  }

  // Iterator functionality.
  bool operator != (const EEPtr &ptr)   {
    return index != ptr.index;
  }
  EERef operator * ()                   {
    return index;
  }

  /* Prefix & Postfix increment/decrement */
  EEPtr &operator ++ ()                 {
    return ++index, *this;
  }
  EEPtr &operator -- ()                 {
    return --index, *this;
  }
  EEPtr operator ++ (int)              {
    return index++;
  }
  EEPtr operator -- (int)              {
    return index--;
  }
  INDEXDATATYPE index; // Index of current EEPROM cell.
};

/* EEPROMClass class.
 *
 * This object represents the entire EEPROM space.
 * It wraps the functionality of EEPtr and EERef into a basic interface.
 * This class is also 100% backwards compatible with earlier Arduino core releases.
 */

struct EEPROMClass {

  // Basic user access methods.
  EERef operator [](const INDEXDATATYPE idx)        {
    return idx & EEPROM_END;
  }
  uint8_t read(const INDEXDATATYPE idx)              {
    return EERef(idx);
  }
  void write(INDEXDATATYPE idx, uint8_t val)   {
    (EERef(idx)) = val;
  }
  void update(INDEXDATATYPE idx, uint8_t val)  {
    EERef(idx).update(val);
  }

  // STL and C++11 iteration capability.
  EEPtr begin()                        {
    return 0x00;
  }
  EEPtr end()                          {
    return length();  // Standards requires this to be the item after the last valid entry. The returned pointer is invalid.
  }
  static constexpr int16_t length()   {
    return EEPROM_SIZE;
  }

  // Functionality to 'get' and 'put' objects to and from EEPROM.
  template< typename T > T &get(INDEXDATATYPE idx, T &t) {
    EEPtr e = idx;
    uint8_t *ptr = (uint8_t *) &t;
    for (uint8_t count = sizeof(T); count; --count, ++e) {
      *ptr++ = *e;
    }
    return t;
  }

  template< typename T > const T &put(INDEXDATATYPE idx, const T &t) {
    EEPtr e = idx;
    const uint8_t *ptr = (const uint8_t *) &t;
    for (uint8_t count = sizeof(T); count; --count, ++e) {
      (*e).update(*ptr++);
    }
    return t;
  }
};

static EEPROMClass EEPROM;
#endif
