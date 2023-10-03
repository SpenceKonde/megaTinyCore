#if !defined(CORE_ATTACH_OLD) && !defined(CORE_ATTACH_ALL)
  #include <inttypes.h>
  #include <avr/io.h>
  #include <avr/interrupt.h>
  #include <avr/pgmspace.h>
  #include <stdio.h>
  #include "wiring_private.h"

  #ifdef PORTA
    extern voidFuncPtr * intFunc[];
    voidFuncPtr intFunc_A[8];
    void attachPortAEnable() {
      intFunc[0] = intFunc_A;
    }
    /* It is the act of referencing attachPortAEnable() in this file that the compiler to also include the ISR.
     * (ISRs can't be assigned at runtime - the only way things like attachInterruprt look like they can is
     * by having the ISR call a function pointer (ie, icall). After saving every call used plus SREG and RAMPZ.
     * Beyond being slow, these lengthy prologues and epilogs slow take up flash. This can be like 80+ bytes
     * per vector... That's ugly, particularly on small flash parts. See the discussion in WInterrupts.c about
     * what this actually does.
     */
    ISR(PORTA_PORT_vect, ISR_NAKED) {
      asm volatile(
        "push r16"        "\n\t"
        "ldi r16, 0"      "\n\t"
#if PROGMEM_SIZE > 8192
        "jmp AttachedISR" "\n\t"
#else
        "rjmp AttachedISR" "\n\t"
#endif
      ::);
    }
  #endif
#endif
