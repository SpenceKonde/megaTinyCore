#include <Comparator.h>
// Array for storing ISR function pointers
#if defined(AC2_AC_vect)
  static volatile voidFuncPtr intFuncAC[3];
#elif defined(AC1_AC_vect)
  static volatile voidFuncPtr intFuncAC[2];
#elif defined(AC0_AC_vect)
  static volatile voidFuncPtr intFuncAC[1];
#else
  #error "Unsupported part? This device does not have an analog comparator!"
#endif

void AnalogComparator::attachInterrupt(void (*userFunc)(void), uint8_t mode) {
  #if defined(__AVR_DD__) || !defined(DXCORE)
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
  #else
  AC_INTMODE_NORMAL_t intmode;
  switch (mode) {
    // Set RISING, FALLING or CHANGE interrupt trigger for the comparator output
    case RISING:
      intmode = (AC_INTMODE_NORMAL_t)AC_INTMODE_NORMAL_POSEDGE_gc;
      break;
    case FALLING:
      intmode = (AC_INTMODE_NORMAL_t)AC_INTMODE_NORMAL_NEGEDGE_gc;
      break;
    case CHANGE:
      intmode = (AC_INTMODE_NORMAL_t)AC_INTMODE_NORMAL_BOTHEDGE_gc;
      break;
    default:
      // Only RISING, FALLING and CHANGE is supported
      return;
  }
  #endif
  // Store function pointer
  intFuncAC[comparator_number] = userFunc;


  // Set interrupt trigger and enable interrupt
  #if !defined(DXCORE)
  AC.CTRLA = (AC.CTRLA & ~AC_INTMODE_gm) | intmode ;
  AC.INTCTRL = AC_CMP_bm;
  #else
  AC.INTCTRL = intmode | AC_CMP_bm;
  #endif
}

void AnalogComparator::detachInterrupt() {
  // Disable interrupt
  AC.INTCTRL = 0;
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
