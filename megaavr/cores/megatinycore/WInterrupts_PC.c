#if !defined(CORE_ATTACH_OLD) && !defined(CORE_ATTACH_ALL)
  #include <inttypes.h>
  #include <avr/io.h>
  #include <avr/interrupt.h>
  #include <avr/pgmspace.h>
  #include <stdio.h>
  #include "wiring_private.h"
  #if defined(PORTC_PINS)
    extern voidFuncPtr * intFunc[];
    voidFuncPtr intFunc_C[PORTC_PINS];
    void attachPortCEnable() {
      intFunc[2] = intFunc_C;
    }
    ISR(PORTC_PORT_vect, ISR_NAKED) {
      asm volatile(
        "push r16"        "\n\t"
        "ldi r16, 4"      "\n\t"
        "jmp AttachedISR" "\n\t"
      ::);
    }
  #endif
#endif
