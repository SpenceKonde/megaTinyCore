/* USERSIG.h USERROW library
 * Copyright (c) Spence Konde 2021
 * Based on EEPROM.h by David A. Mellis (2006) and Christopher Andrews (2015).
 *
 * This is an exact copy of the EEPROM library, only modified to point to the
 * User Signature Space (otherwise known as the USERROW), a section of 32 bytes
 * of EEPROM-like space on the tinyAVR 0/1/2 and megaAVR 0-series parts. This
 * modification involved only the names of the classes, implementing the read
 * and write methods. This does NOT support modifying the USERROW on the AVR
 * Dx-series. On those parts, it works differently; where it has byte erase
 * granularity here, on the Dx, it can only be erased by erasing the whole thing
 * and implementing the analog of this library without modification would have
 * made it too easy for someone to unintentionally exhaust the write endurance
 * of the USERROW (automatically doing an read, modify, erase, rewrite cycle
 * instead of write could amplify the number of writes (imagine, writing a new
 * value to every address. That would generate 32 erase-rewrite cycles when only
 * one was actually necessary). That will require a new approach.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PUSPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef USERSIG_h
#define USERSIG_h

#include <inttypes.h>
#include <avr/io.h>



/* USRef class.
 *
 * This object references an USERSIG cell.
 * Its purpose is to mimic a typical byte of RAM, however its storage is the USERSIG.
 * This class has an overhead of two bytes, similar to storing a pointer to an USERSIG cell.
 */

struct USRef {

  USRef(const uint8_t index)
    : index(index)                   {}

  // Access/read members.
  uint8_t operator*() const          {
    return *(volatile uint8_t *)((index & (USER_SIGNATURES_SIZE - 1)) | USER_SIGNATURES_START);
  }
  operator uint8_t() const           {
    return **this;
  }

  // Assignment/write members.
  USRef &operator=(const USRef &ref) {
    return *this = *ref;
  }
  USRef &operator=(uint8_t in)       {
    *(uint8_t *)((index & (USER_SIGNATURES_SIZE - 1)) | USER_SIGNATURES_START) = in;
    _PROTECTED_WRITE_SPM(NVMCTRL.CTRLA, NVMCTRL_CMD_PAGEERASEWRITE_gc);
    while (NVMCTRL.STATUS & NVMCTRL_EEBUSY_bm);
    return *this;
  }

  USRef &operator +=(uint8_t in)     {
    return *this = **this + in;
  }
  USRef &operator -=(uint8_t in)     {
    return *this = **this - in;
  }
  USRef &operator *=(uint8_t in)     {
    return *this = **this * in;
  }
  USRef &operator /=(uint8_t in)     {
    return *this = **this / in;
  }
  USRef &operator ^=(uint8_t in)     {
    return *this = **this ^ in;
  }
  USRef &operator %=(uint8_t in)     {
    return *this = **this % in;
  }
  USRef &operator &=(uint8_t in)     {
    return *this = **this & in;
  }
  USRef &operator |=(uint8_t in)     {
    return *this = **this | in;
  }
  USRef &operator <<=(uint8_t in)    {
    return *this = **this << in;
  }
  USRef &operator >>=(uint8_t in)    {
    return *this = **this >> in;
  }

  USRef &update(uint8_t in)          {
    return  in != *this ? *this = in : *this;
  }

  /* Prefix increment/decrement */
  USRef &operator++()                {
    return *this += 1;
  }
  USRef &operator--()                {
    return *this -= 1;
  }

  /* Postfix increment/decrement */
  uint8_t operator++ (int)           {
    uint8_t ret = **this;
    return ++(*this), ret;
  }

  uint8_t operator-- (int)           {
    uint8_t ret = **this;
    return --(*this), ret;
  }

  uint8_t index; //Index of current USERSIG cell.
};

/* USPtr class.
 *
 * This object is a bidirectional pointer to USERROW cells represented by USRef objects.
 * Just like a normal pointer type, this can be dereferenced and repositioned using
 * increment/decrement operators.
 */

struct USPtr {

  USPtr(const uint8_t index)
    : index(index)                  {}

  operator int() const              {
    return index;
  }
  USPtr &operator=(int in)          {
    return index = (in), *this;
  }

  // Iterator functionality.
  bool operator!=(const USPtr &ptr) {
    return index != ptr.index;
  }
  USRef operator*()                 {
    return index;
  }

  /* Prefix & Postfix increment/decrement */
  USPtr &operator++()               {
    return ++index, *this;
  }
  USPtr &operator--()               {
    return --index, *this;
  }
  USPtr operator++ (int)            {
    return index++;
  }
  USPtr operator-- (int)            {
    return index--;
  }

  uint8_t index; // Index of current USERROW cell.
};

/* USERSIGClass class.
 *
 *  This object represents the entire USERROW space.
 *  It wraps the functionality of USPtr and USRef into a basic interface.
 */

struct USERSIGClass {

  //Basic user access methods.
  USRef operator[](const int idx)    {
    return idx;
  }
  uint8_t read(int idx)              {
    return USRef(idx);
  }
  void write(int idx, uint8_t val)   {
    (USRef(idx)) = val;
  }
  void update(int idx, uint8_t val)  {
    USRef(idx).update(val);
  }

  // STL and C++11 iteration capability.
  USPtr begin()                      {
    return 0x00;
  }

  // Standards require this to be the item after the last valid entry. The returned pointer is invalid.
  USPtr end()                        {
    return length();
  }

  static constexpr uint8_t length()  {
    return USER_SIGNATURES_SIZE;
  }

  // Functionality to 'get' and 'put' objects to and from EEPROM.
  template< typename T > T &get(int idx, T &t) {
    USPtr e = idx;
    uint8_t *ptr = (uint8_t *) &t;
    for (int count = sizeof(T) ; count ; --count, ++e) {
      *ptr++ = *e;
    }
    return t;
  }

  template< typename T > const T &put(int idx, const T &t) {
    USPtr e = idx;
    const uint8_t *ptr = (const uint8_t *) &t;
    for (int count = sizeof(T) ; count ; --count, ++e) {
      (*e).update(*ptr++);
    }
    return t;
  }
};

static USERSIGClass USERSIG;
#endif
