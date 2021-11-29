#if !defined(CORE_ATTACH_OLD) && !defined(CORE_ATTACH_ALL)
  #include <inttypes.h>
  #include <avr/io.h>
  #include <avr/interrupt.h>
  #include <avr/pgmspace.h>
  #include <stdio.h>
  #include "wiring_private.h"
  #if defined(PORTB_PINS)
    extern voidFuncPtr * intFunc[];
    voidFuncPtr intFunc_B[PORTB_PINS];
    void attachPortBEnable() {
      intFunc[1]=intFunc_B;
    }
    ISR(PORTB_PORT_vect, ISR_NAKED) {
      asm volatile(
        "push r16"        "\n\t"
        "ldi r16, 2"      "\n\t"
        "jmp AttachedISR" "\n\t"
      ::);
    }
  #endif
#endif
