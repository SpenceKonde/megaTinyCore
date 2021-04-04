#include "Comparator.h"
#include "Arduino.h"


#if defined(AC0_AC_vect)
  #if (MEGATINYCORE_SERIES > 0) && defined(PORTB)
    AnalogComparator Comparator0(0, AC0, PORTA.PIN7CTRL, PORTB.PIN5CTRL, PORTB.PIN1CTRL, PORTB.PIN6CTRL, PORTA.PIN6CTRL, PORTB.PIN4CTRL, PORTB.PIN0CTRL);
  #else
    // 212, 412, or 0-series - only 1 pos, 1 neg pin, so save space by using definition that only allows for those.
    AnalogComparator Comparator0(0, AC0, PORTA.PIN7CTRL, PORTA.PIN6CTRL);
  #endif
#endif
// All the parts with more than one AC have at least 14 pins and hence have at least PORTA and PORTB defined and are 1-series. All PORT structs have all PINnCTRL
// registers defined - even when the pin doesn't exist.
#if defined(AC1_AC_vect)
  AnalogComparator Comparator1(1, AC1, PORTA.PIN7CTRL, PORTA.PIN6CTRL, PORTB.PIN0CTRL, PORTB.PIN4CTRL, PORTA.PIN5CTRL, PORTB.PIN6CTRL, PORTA.PIN7CTRL);
#endif
#if defined(AC2_AC_vect)
  AnalogComparator Comparator2(2, AC2, PORTA.PIN6CTRL, PORTB.PIN0CTRL, PORTB.PIN5CTRL, PORTB.PIN7CTRL, PORTA.PIN7CTRL, PORTB.PIN7CTRL, PORTA.PIN7CTRL);
#endif

// Array for storing ISR function pointers
#if defined(AC2_AC_vect)
  static volatile voidFuncPtr intFuncAC[3];
#elif defined(AC1_AC_vect)
  static volatile voidFuncPtr intFuncAC[2];
#elif defined(AC0_AC_vect)
  static volatile voidFuncPtr intFuncAC[1];
#else
  #error target does not have an analog comparator!
#endif

/* A few notes on adaptations for megaTinyCore
  1. There are two definitions for the comparator objects, one with negative and
  positive inputs 0, used for 8-pin and 0-series parts, and the other with all
  of them. 0-series parts have small flash, and 8-pin parts have small flash, so
  the small amount of flash this saves is worthwhile (you don't just get back
  the flash the constructor above eats; there's also the if/elseif chain below)
  2. Atmel and Microchip are at it again with dartboards, monkeys and feature
  lists. Though broadly similar, the devils are in the details, and there seem
  to be a considerable population of them here. Every series is rather different from the others.
  3. Inversion may be desired even when just using a as an event source, with no
  pin output.... Currently no way to do this!
  4. All this, and no wrapper around getting the current status?
  I added a value() method that returns a boolean indicating the state, just
  because it seemed weird to have wrappers around everything else but not that.
  5. Added optional (defaults to false) argument to stop(), if true, it re-enables
  the digital input buffers on the pins that were used. Doesn't take up any
  space if not used thanks to LTO.
*/


//*INDENT-OFF* formatting checker doesn't like all the indentation...
#if (MEGATINYCORE_SERIES > 0) && defined(PORTB)
AnalogComparator::AnalogComparator(
                                   const uint8_t comp_number,
                                   AC_t& ac,
                                   register8_t& in0_p,
                                   register8_t& in1_p,
                                   register8_t& in2_p,
                                   register8_t& in3_p,
                                   register8_t& in0_n,
                                   register8_t& in1_n,
                                   register8_t& in2_n
                                   )
                                   : comparator_number(comp_number),
                                     AC(ac),
                                     IN0_P(in0_p),
                                     IN1_P(in1_p),
                                     IN2_P(in2_p),
                                     IN3_P(in3_p),
                                     IN0_N(in0_n),
                                     IN1_N(in1_n),
                                     IN2_N(in2_n) { }
#else
AnalogComparator::AnalogComparator(
                                   const uint8_t comp_number,
                                   AC_t& ac,
                                   register8_t& in0_p,
                                   register8_t& in0_n
                                   )
                                   : comparator_number(comp_number),
                                     AC(ac),
                                     IN0_P(in0_p),
                                     IN0_N(in0_n) { }
#endif


void AnalogComparator::init() {
  // Set voltage reference]
  if (reference != ref::disable) {
    if (comparator_number == 0) {

      #if defined(VREF_AC0REFEN_bm)
      /* 0/1-series and 2-series name it differently
         See, on the 0-series, where there's nothing like a DAC, still called
         DAC0REF. But on 2-series, where there's a DACREF for AC0, this is
         called AC0REF. Totally reasonable bitfield naming right? */
        VREF.CTRLA  = reference; /* otherwise empty register */
        VREF.CTRLB |= VREF_AC0REFEN_bm;
        AC.DACREF = dacref;
      #else
        VREF.CTRLA  = (VREF.CTRLA & 0xF8) | reference; /* shared with ADC0 reference */
        #ifdef DAC0
          VREF.CTRLB |= VREF_DAC0REFEN_bm;
          DAC0.CTRLA |= DAC_ENABLE_bm;
          DAC0.DATA   = dacref;
        #endif
      #endif
    #if defined (AC1)
      } else if (comparator_number == 1) { // Everything with AC1 and AC2 has the DAC.
        VREF.CTRLC  = (VREF.CTRLC & 0xF8) | reference; /* shared with ADC1 reference */
        VREF.CTRLB |= VREF_DAC1REFEN_bm;
        DAC1.CTRLA |= DAC_ENABLE_bm;
        DAC1.DATA   = dacref;
    #endif
    #if defined (AC2)
      } else if (comparator_number == 2) {
        VREF.CTRLD  = reference; /* otherwise empty register */
        VREF.CTRLB |= VREF_DAC2REFEN_bm;
        DAC2.CTRLA |= DAC_ENABLE_bm;
        DAC2.DATA   = dacref;
    #endif
      }
    } else { /* it is being disabled */
    if (comparator_number == 0) {

      #if defined(VREF_AC0REFEN_bm)
      /* 0/1-series and 2-series name it differently
         See, on the 0-series, where there's nothing like a DAC, still called
         DAC0REF. But on 2-series, where there's a DACREF for AC0, this is
         called AC0REF. Totally reasonable bitfield naming right? */
        VREF.CTRLB &= ~VREF_AC0REFEN_bm;
      #else
        VREF.CTRLB &= ~VREF_DAC0REFEN_bm;
        #ifdef DAC0
          uint8_t temp=DAC0.CTRLA;
          if (!(temp & DAC_OUTEN_bm)) /* Don't disable the DAC if it's also outputting */
            DAC0.CTRLA = temp & (~DAC_ENABLE_bm);
        #endif
      #endif
    #if defined (AC1)
      } else if (comparator_number == 1) {
        VREF.CTRLB &= ~VREF_DAC1REFEN_bm;
        DAC1.CTRLA &= ~DAC_ENABLE_bm;
    #endif
    #if defined (AC2)
      } else if (comparator_number == 2) {
        VREF.CTRLB &= ~VREF_DAC2REFEN_bm;
        DAC2.CTRLA &= ~DAC_ENABLE_bm;
    #endif
      }
    }


  // Set inputs
  if (input_p == in_p::in0) {
    IN0_P = PORT_ISC_INPUT_DISABLE_gc;
  }
  #if (MEGATINYCORE_SERIES > 0) && defined(PORTB) /* shrink code size on 0-series and 8-pin parts */
    else if (input_p == in_p::in1) {
      IN1_P = PORT_ISC_INPUT_DISABLE_gc;
    } else if (input_p == in_p::in2) {
      IN2_P = PORT_ISC_INPUT_DISABLE_gc;
    } else if (input_p == in_p::in3) {
      IN3_P = PORT_ISC_INPUT_DISABLE_gc;
    }
  #endif

  if (input_n == in_n::in0) {
    IN0_N = PORT_ISC_INPUT_DISABLE_gc;
  }
  #if (MEGATINYCORE_SERIES > 0) && defined(PORTB) /* shrink code size on 0-series and 8-pin parts */
    else if (input_n == in_n::in1) {
      IN1_N = PORT_ISC_INPUT_DISABLE_gc;
    }
    #if (MEGATINYCORE_SERIES == 2)
      else if (input_n == in_n::in2) {
        IN2_N = PORT_ISC_INPUT_DISABLE_gc;
      }
    #endif
  #endif
  AC.MUXCTRLA = output_initval | (input_p << 3) | input_n | (output & 0x80);
  /* Huh! Okay then.... apparently when you enable output, it takes over the direction control too, so no need to do this!
  // Set output
  if (output != out::disable) { //either enable or invert
    // Prepare for output pin config (now only executed if output is requested on pin, though it only buys speed not code size -SK)
    #ifdef AC1
      PORT_t &output_port = (comparator_number == 0) ? PORTA : PORTB;
      uint8_t pin_number = (comparator_number == 0) ? PIN5_bm : ((comparator_number == 1) ? PIN3_bm : PIN2_bm);
    #else //Don't test which AC it is if we only have one
      PORT_t &output_port = PORTA;
      #ifndef PORTB
        uint8_t pin_number = PIN3_bm;
      #else
        uint8_t pin_number = PIN5_bm;
      #endif
    #endif
    output_port.DIRSET =  pin_number;
  }
  */

  // Set hysteresis, and output, hilwe
  AC.CTRLA = (AC.CTRLA & ~(AC_HYSMODE_gm | AC_OUTEN_bm)) | hysteresis | (output & 0x40);
}

void AnalogComparator::start(bool state) {
  if (state) {
    AC.CTRLA |=  AC_ENABLE_bm;
  } else {
    AC.CTRLA &= ~AC_ENABLE_bm;
  }
}

void AnalogComparator::stop(bool restore) {
  start(false);
  if (restore) {
    // reset inputs

    if (input_p == in_p::in0) {
      IN0_P &= ~PORT_ISC_gm;
    }
    #if (MEGATINYCORE_SERIES > 0) && defined(PORTB)
      else if (input_p == in_p::in1) {
        IN1_P &= ~PORT_ISC_gm;
      } else if (input_p == in_p::in2) {
        IN2_P &= ~PORT_ISC_gm;
      } else if (input_p == in_p::in3) {
        IN3_P &= ~PORT_ISC_gm;
      }
    #endif

    if (input_n == in_n::in0) {
      IN0_N &= ~PORT_ISC_gm;
    }
    #if (MEGATINYCORE_SERIES > 0) && defined(PORTB)
      else if (input_n == in_n::in1) {
        IN1_N &= ~PORT_ISC_gm;
      }
      #if (MEGATINYCORE_SERIES == 2)
        else if (input_n == in_n::in2) {
          IN2_N &= ~PORT_ISC_gm;
        }
      #endif
    #endif
  }
}

void AnalogComparator::attachInterrupt(void (*userFunc)(void), uint8_t mode) {
  AC_INTMODE_t intmode;
  switch (mode) {
    // Set RISING, FALLING or CHANGE interrupt trigger for the comparator output
    case RISING:
      intmode = (AC_INTMODE_t)AC_INTMODE_POSEDGE_gc;
      break;
    case FALLING:
      intmode = (AC_INTMODE_t)AC_INTMODE_NEGEDGE_gc;
      break;
    case CHANGE:
      intmode = (AC_INTMODE_t)AC_INTMODE_BOTHEDGE_gc;
      break;
    default:
      // Only RISING, FALLING and CHANGE is supported
      return;
  }

  // Store function pointer
  intFuncAC[comparator_number] = userFunc;

  // Set interrupt trigger and enable interrupt
  AC.CTRLA = (AC.CTRLA & ~AC_INTMODE_gm) | intmode ;
  AC.INTCTRL = AC_CMP_bm;
}

void AnalogComparator::detachInterrupt() {
  // Disable interrupt
  AC.INTCTRL &= ~AC_CMP_bm;
}

#ifdef AC0_AC_vect
ISR(AC0_AC_vect) {
  // Run user function
  intFuncAC[0]();

  // Clear flag
  AC0.STATUS = AC_CMP_bm;
}
#endif

#ifdef AC1_AC_vect
ISR(AC1_AC_vect) {
  // Run user function
  intFuncAC[1]();

  // Clear flag
  AC1.STATUS = AC_CMP_bm;
}
#endif

#ifdef AC2_AC_vect
ISR(AC2_AC_vect) {
  // Run user function
  intFuncAC[2]();

  // Clear flag
  AC2.STATUS = AC_CMP_bm;
}
#endif
