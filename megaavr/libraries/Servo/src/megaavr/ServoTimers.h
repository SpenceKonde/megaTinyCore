/*
  Copyright (c) 2018 Arduino LLC. All right reserved.
  Copyright (c) 2021 Spence Konde

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

  This version was designed for and will be included with:
  megaTinyCore 2.3.0+
  DxCore 1.3.2+
*/

#ifndef __SERVO_TIMERS_H__
#define __SERVO_TIMERS_H__

// SERVO_USE_TIMERBn can be defined to force servo onto a specific TCB; If it is not defined, we default to TCB1
// unless it's been taken over for millis, in which case we use the TCB0. On tinyAVR parts, the same logic is used
// but there there's the possibility that only TCB0 exists AND it's being used for millis, in which case we error.

#if defined(SERVO_USE_TIMERB4)
  #if !defined(TCB4)
    #error "SERVO_USE_TIMERB4 is defined, but there is no TCB4 on selected part."
  #elif defined(MILLIS_USE_TIMERB4)
    #error "SERVO_USE_TIMERB4 is defined, but so is MILLIS_USE_TIMERB2 - TCB4 can only be used for one of these."
  #else
    #define USE_TIMERB4
  #endif
#elif defined(SERVO_USE_TIMERB3)
  #if !defined(TCB3)
    #error "SERVO_USE_TIMERB3 is defined, but there is no TCB3 on selected part."
  #elif defined(MILLIS_USE_TIMERB3)
    #error "SERVO_USE_TIMERB3 is defined, but so is MILLIS_USE_TIMERB3 - TCB3 can only be used for one of these."
  #else
    #define USE_TIMERB3
  #endif
#elif defined(SERVO_USE_TIMERB2)
  #if !defined(TCB2)
    #error "SERVO_USE_TIMERB2 is defined, but there is no TCB2 on selected part."
  #elif defined(MILLIS_USE_TIMERB2)
    #error "SERVO_USE_TIMERB3 is defined, but so is MILLIS_USE_TIMERB2 - TCB2 can only be used for one of these."
  #else
    #define USE_TIMERB2
  #endif
  // ah, finally cases that might be relevant to tinyAVR
#elif (defined(TCB1) && defined(SERVO_USE_TIMERB1))
  #if defined(MILLIS_USE_TIMERB1)
    #error "SERVO_USE_TIMERB1 is defined, but so is MILLIS_USE_TIMERB1 - TCB1 can only be used for one of these."
  #else
    #define USE_TIMERB1
  #endif
#elif (defined(TCB0) && defined(SERVO_USE_TIMERB0))
  #if defined(MILLIS_USE_TIMERB0)
    #error "SERVO_USE_TIMERB0 is defined, but so is MILLIS_USE_TIMERB0 - TCB0 can only be used for one of these."
  #else
    #define USE_TIMERB0
  #endif
  // No defines try to force a specific timer, use TCB1 if it exists unless it's being used for millis.
#elif (defined(TCB1) && !defined(MILLIS_USE_TIMERB1))
  #define USE_TIMERB1
#elif !defined(MILLIS_USE_TIMERB0)
  #define USE_TIMERB0
#else
  #error "Only one TCB, TCB0 but it is being used for millis - choose a different millis timer."
#endif


//_timer is obviously used in Servo.cpp; and Servo.cpp works without explicitly telling the compiler this is used...
// it just generates a spurious warning, which is undesirable, all else being equal. What is really weird is that whether
//  it is marked used or not changes the size of the compiled binary by oh, a dozen or so bytes. The amount depends on the
// sketch, and not always in the same direction... but if you look at the generated assembly... there are differences
// all over the place! No observed differences in program behavior, though, so with the sketch size being a wash, err on
// the side of not getting a warning, especially a scary one like that (critically important variable considered unused).
static volatile __attribute__((used))  TCB_t *_timer =
#if defined(USE_TIMERB0)
  &TCB0;
#elif defined(USE_TIMERB1)
  &TCB1;
#elif defined(USE_TIMERB2)
  &TCB2;
#elif defined(USE_TIMERB3)
  &TCB3;
#elif defined(USE_TIMERB4)
  &TCB4;
#endif

typedef enum {
  timer0,
  _Nbr_16timers
} timer16_Sequence_t;


#endif  /* __SERVO_TIMERS_H__ */
