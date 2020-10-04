/*
  EEPROM.h - EEPROM library
  Original Copyright (c) 2006 David A. Mellis.  All right reserved.
  New version by Christopher Andrews 2015.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PUSPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef USERSIG_h
#define USERSIG_h

#include <inttypes.h>
#include <avr/io.h>



/***
    UURef class.

    This object references an USERSIG cell.
    Its purpose is to mimic a typical byte of RAM, however its storage is the USERSIG.
    This class has an overhead of two bytes, similar to storing a pointer to an USERSIG cell.
***/

struct USRef {

  USRef(const uint8_t index)
    : index(index)                 {}

  //Access/read members.
  uint8_t operator*() const            {
    return *(volatile uint8_t *)(index + USER_SIGNATURES_START);
  }
  operator uint8_t() const             {
    return **this;
  }

  //Assignment/write members.
  USRef &operator=(const USRef &ref) {
    return *this = *ref;
  }
  USRef &operator=(uint8_t in)       {
    *(uint8_t *)((index & 0x1F) |  USER_SIGNATURES_START) = in;
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

  /** Prefix increment/decrement **/
  USRef &operator++()                  {
    return *this += 1;
  }
  USRef &operator--()                  {
    return *this -= 1;
  }

  /** Postfix increment/decrement **/
  uint8_t operator++ (int) {
    uint8_t ret = **this;
    return ++(*this), ret;
  }

  uint8_t operator-- (int) {
    uint8_t ret = **this;
    return --(*this), ret;
  }

  uint8_t index; //Index of current USERSIG cell.
};

/***
    USPtr class.

    This object is a bidirectional pointer to EEPROM cells represented by USRef objects.
    Just like a normal pointer type, this can be dereferenced and repositioned using
    increment/decrement operators.
***/

struct USPtr {

  USPtr(const uint8_t index)
    : index(index)                {}

  operator int() const                {
    return index;
  }
  USPtr &operator=(int in)          {
    return index = (in), *this;
  }

  //Iterator functionality.
  bool operator!=(const USPtr &ptr) {
    return index != ptr.index;
  }
  USRef operator*()                   {
    return index;
  }

  /** Prefix & Postfix increment/decrement **/
  USPtr &operator++()                 {
    return ++index, *this;
  }
  USPtr &operator--()                 {
    return --index, *this;
  }
  USPtr operator++ (int)              {
    return index++;
  }
  USPtr operator-- (int)              {
    return index--;
  }

  uint8_t index; //Index of current EEPROM cell.
};

/***
    EEPROMClass class.

    This object represents the entire EEPROM space.
    It wraps the functionality of USPtr and USRef into a basic interface.
    This class is also 100% backwards compatible with earlier Arduino core releases.
***/

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

  //STL and C++11 iteration capability.
  USPtr begin()                        {
    return 0x00;
  }
  USPtr end()                          {
    return length();  //Standards requires this to be the item after the last valid entry. The returned pointer is invalid.
  }
  static constexpr uint8_t length()   {
    return 32;
  }

  //Functionality to 'get' and 'put' objects to and from EEPROM.
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
