/*
  Copyright (c) 2014 Arduino.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "RingBuffer.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

RingBuffer::RingBuffer(rb_index_type size) : size(size)
{
    _aucBuffer = (uint8_t*)malloc(size);
    memset( _aucBuffer, 0, size ) ;
    clear();
}

void RingBuffer::store_char( uint8_t c )
{
    rb_index_type i = nextIndex(_iHead);

    // if we should be storing the received character into the location
    // just before the tail (meaning that the head would advance to the
    // current location of the tail), we're about to overflow the buffer
    // and so we don't write the character or advance the head.
    if ( i != _iTail )
    {
        if (_iHead < size) {
            _aucBuffer[_iHead] = c ;
        } else {
            additionalBuffer[_iHead - size] = c;
        }
    _iHead = i ;
    }
}

void RingBuffer::clear()
{
    _iHead = 0;
    _iTail = 0;
}

int RingBuffer::read_char()
{
    if(_iTail == _iHead)
        return -1;

    uint8_t value;
    if (_iTail < size) {
        value  = _aucBuffer[_iTail];
    } else {
        value = additionalBuffer[_iTail - size];
    }
    _iTail = nextIndex(_iTail);

    return value;
}

int RingBuffer::available()
{
    int delta = _iHead - _iTail;

    if(delta < 0)
        return size + additionalSize + delta;
    else
        return delta;
}

int RingBuffer::availableForStore()
{
  int delta = _iHead - _iTail;
  if (delta >= 0)
    return size + additionalSize - 1 - delta;
  else
    return -delta - 1;
}


int RingBuffer::peek()
{
    if(_iTail == _iHead)
        return -1;

    if (_iTail < size) {
        return _aucBuffer[_iTail];
    } else {
        return additionalBuffer[_iTail - size];
    }
}

rb_index_type RingBuffer::nextIndex(rb_index_type index)
{
    return (rb_index_type)(index + 1) % (size + additionalSize);
}

bool RingBuffer::isFull()
{
    return (nextIndex(_iHead) == _iTail);
}
