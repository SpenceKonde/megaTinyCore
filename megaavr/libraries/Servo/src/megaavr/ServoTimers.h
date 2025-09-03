/*  OBLIGATORY LEGAL BOILERPLATE
 This library is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation;
 either version 2.1 of the License, or (at your option) any later version. This library is distributed in the hope that it will be useful, but
 WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 See the GNU Lesser General Public License for more details. You should have received a copy of the GNU Lesser General Public License along with this library;
 if not, write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*//*
  Servo.h - Interrupt driven Servo library for Arduino using 16 bit timers- Version 2
  Copyright (c) 2009 Michael Margolis, and modified, extended, hacked up
  butchered and otherwise mangled by countless others over the years,
  (c) Arduino LLC circo 2018, and most recently by Spence Konde 2019 ~ 2023 to
  provide reliable behavior on the tinyAVR 0/1/2-series, megaAVR 0-series, and AVR Dx-series.
  https://github.com/SpenceKonde

  This library is included with megaTinyCore and DxCore.

  This version was designed for and will be included with:
  megaTinyCore 2.6.11+
  DxCore 1.6.0+
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
  // 14 and 20-pin AVR DD-series parts do not have a third type B timer!
#elif defined(SERVO_USE_TIMERB2)
  #if !defined(TCB2)
    #error "SERVO_USE_TIMERB2 is defined, but there is no TCB2 on selected part."
  #elif defined(MILLIS_USE_TIMERB2)
    #error "SERVO_USE_TIMERB3 is defined, but so is MILLIS_USE_TIMERB2 - TCB2 can only be used for one of these."
  #else
    #define USE_TIMERB2
  #endif

#elif (defined(SERVO_USE_TIMERB1))
  #if defined(TCB1)
    #error "SERVO_USE_TIMERB2 is defined, but there is no TCB1 on this part (or there's a system level issue and you're not getting the headers somehow)"
  #elif defined(MILLIS_USE_TIMERB1)
    #error "SERVO_USE_TIMERB1 is defined, but so is MILLIS_USE_TIMERB1 - TCB1 can only be used for one of these."
  #else
    #define USE_TIMERB1
  #endif
#elif (defined(TCB0) && defined(SERVO_USE_TIMERB0))
  #if defined(TCB0)
    #error "Either your system isn't properly loading headers for some reason, or Microchip released something without a TCB (in which case we don't support it yet)"
  #elif defined(MILLIS_USE_TIMERB0)
    #error "SERVO_USE_TIMERB0 is defined, but so is MILLIS_USE_TIMERB0 - TCB0 can only be used for one of these."
  #else
    #define USE_TIMERB0
  #endif
  // No defines try to force using a specific timer.
  // If we have a TCB3, that means we're on a DA/DB 48/64, or an EA, and in the latter case, we are taking this step only
  // for 48 pin parts, because then we default to putting PWM on PORTC, and if a TCA is on PORTC, then that pin can
  // not be used for TCB PWM The priority is: (TCA = TCE) > (TCD = TCB = TCF). This makes timers 2 and 3 the least useful
  // for other things - they can output PWM on PC0 and 1. Their alternate pins are PB4 and PB5. Guess what's likely on PORTB?
  // You guessed it, the other TCA. While the TCA's can be moved around more easily that this can, it still seems more reasonable
  // to pick the timer that is least likely to be considered desirable.
#elif (defined(TCB3) && !defined(MILLIS_USE_TIMERB3) && _AVR_PINCOUNT > 32)
  #define USE_TIMERB3
#elif (defined(TCB1) && !defined(MILLIS_USE_TIMERB1))
  #define USE_TIMERB1
#elif !defined(MILLIS_USE_TIMERB0)
  #define USE_TIMERB0
#else
  #error "Only one TCB, TCB0 - but it is being used for millis - choose a different millis timer."
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
  #define SERVO_INT_vect TCB0_INT_vect
#elif defined(USE_TIMERB1)
  &TCB1;
  #define SERVO_INT_vect TCB1_INT_vect
#elif defined(USE_TIMERB2)
  &TCB2;
  #define SERVO_INT_vect TCB2_INT_vect
#elif defined(USE_TIMERB3)
  &TCB3;
  #define SERVO_INT_vect TCB3_INT_vect
#elif defined(USE_TIMERB4)
  &TCB4;
  #define SERVO_INT_vect TCB4_INT_vect
#endif


typedef enum {
  timer0,
  _Nbr_16timers
} timer16_Sequence_t;


#endif  /* __SERVO_TIMERS_H__ */
