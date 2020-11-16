/*
  Copyright (c) 2018 Arduino LLC. All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
*/

/*
   Defines for 16 bit timers used with Servo library

*/

// Adapted by Spence Konde for megaTinyCore 2019


#ifndef __SERVO_TIMERS_H__
#define __SERVO_TIMERS_H__

#ifndef MEGATINYCORE
  #error "This version of ServoTimers.h is integrated with megaTinyCore. It is not intended for use with other parts."
#endif

#if defined(TCB1) && (!defined(MILLIS_USE_TIMERB1))
  #define USE_TIMERB1
#else
  #define USE_TIMERB0
#endif

#if (!defined(USE_TIMERB1) && !defined(USE_TIMERB2) && !defined(USE_TIMERB0))||(defined(USE_TIMERB0)&&defined(MILLIS_USE_TIMERB0))
  # error "No timer available for servo, only option used for millis()"
#endif
//_timer is obviously used in Servo.cpp; and Servo.cpp works without explicitly telling the compiler this is used...
// it just generates a spurious warning, which is undesirable, all else being equal. What is really weird is that whether
//  it is marked used or not changes the size of the compiled binary by oh, a dozen or so bytes. The amount depends on the
// sketch, and not always in the same direction... but if you look at the generated assembly... there are differences
// all over the place! No observed differences in program behavior, though, so with the sketch size being a wash, err on
// the side of not getting a warning, especially a scary one like that (critically important variable considered unused).
static volatile __attribute__ ((used))  TCB_t *_timer =
#if defined(USE_TIMERB0)
  &TCB0;
#endif
#if defined(USE_TIMERB1)
  &TCB1;
#endif
#if defined(USE_TIMERB2)
  &TCB2;
#endif

typedef enum {
  timer0,
  _Nbr_16timers
} timer16_Sequence_t;


#endif  /* __SERVO_TIMERS_H__ */
