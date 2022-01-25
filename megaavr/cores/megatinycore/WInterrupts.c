/* Completely rewritten 2021 Spence Konde;
 * This is part of DxCore/megaTinyCore. Open source softeare (LGPL 2.1) see License.md for more info.
 *
 * attachInterrupt() that uses less flash and doesn't assimilate every interrupt vector...
 *
 * The ultimate goal was not achieved, but we can manually force the effect I wanted.
 * Someone with deeper knowledge of how C/C++ decides that it will iclude the ISR from a file
 * but it does give *a way* to write code that can use an attachInterrupt library and manually define pin interrupts.
 * thisis important because attachInterrupt interrupts are miserably slow. Even in assembly, it's like 42 clocks to
 * get to the loop, 7+ per pass through it, and a little over 40 more on the way out!
 * Using lower numbered pins within a port minimizes how crappy this is. A port with 1 interrupt on bit 7 will take
 * almost 42 clocks longer to be reached as it checks lower bits than one with it's one interrupt on bit 0
 */

#include <inttypes.h>
#include <Arduino.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <stdio.h>
#include "wiring_private.h"

#if !defined(CORE_ATTACH_OLD)
/* Well, I couldn't get it to autodetect awhere you're putting constant pins on interrupts, but there are tiny
 * that you can call when using manual enabling;. Referencing them causes it to grab the associated vector, though it does have to be called to actually enable it - that's how we
 * avoid wasting resourceses - each one not  used also drops an array of 8 pointers to ISRs.
 *
 * attachPortAEnable()      attachPortBEnable()     attachPortCEnable()    attachPortDEnable()
 * attachPortEEnable()      attachPortFEnable()     attachPortGEnable()
 *
 * So you get the option wehether to enable it for each port.  Each port also doesn't get it's own bloated ISR composed almost entirely of pushes and pops for calling a function
 * it was done by doing rather evil things here but it does appear to work!
 */
 /*Only create arrays for ports that exist) */
  #if !defined(CORE_ATTACH_ALL)
    extern voidFuncPtr * intFunc_A;
    #ifdef PORTB_PINS
      extern voidFuncPtr * intFunc_B;
    #endif
    #ifdef PORTC_PINS
      extern voidFuncPtr * intFunc_C;
    #endif
  #else
    voidFuncPtr intFunc_A[8];
    #ifdef PORTB_PINS
      voidFuncPtr intFunc_B[PORTB_PINS];
    #endif
    #ifdef PORTC_PINS
      voidFuncPtr intFunc_C[PORTC_PINS];
    #endif
    volatile voidFuncPtr * intFunc[] = {
    intFunc_A,
    #ifdef PORTB_PINS
      intFunc_B,
    #endif
    #ifdef PORTC_PINS
      intFunc_C
    #endif
    };
  #endif
  #if !defined(CORE_ATTACH_ALL)
    #if defined(PORTC_PINS)
      volatile voidFuncPtr * intFunc[] = {NULL, NULL, NULL};
    #elif defined(PORTB_PINS)
      volatile voidFuncPtr * intFunc[] = {NULL, NULL};
    #else
    volatile voidFuncPtr * intFunc[] = {NULL};
    #endif
  #endif

  volatile uint8_t* portbase = (volatile uint8_t*)((uint16_t)0x400);
  void  attachInterrupt(uint8_t pin, void (*userFunc)(void), uint8_t mode) {
    uint8_t bitpos = digitalPinToBitPosition(pin);
    if (bitpos == NOT_A_PIN) {
      return;
    }
    uint8_t port = digitalPinToPort(pin);
    switch (mode) {
      case CHANGE:
        mode = PORT_ISC_BOTHEDGES_gc;
        break;
      case FALLING:
        mode = PORT_ISC_FALLING_gc;
        break;
      case RISING:
        mode = PORT_ISC_RISING_gc;
        break;
      case LOW:
        mode = PORT_ISC_LEVEL_gc;
        break;
      default:
        return;
    }
    if (intFunc[port] != NULL) { // if it is null the port is not enabled for attachInterrupt. I wasn't successful triggering a way for the
      intFunc[port][bitpos] = userFunc;
      uint8_t portoffset = ((port << 5) & 0xE0) + 0x10 + bitpos;
      // We now have the port, the mode, the bitpos and the pointer
      uint8_t settings = *(portbase + portoffset) & 0xF8;
      *(portbase + portoffset) = settings | mode;
    }
  }

  void __attribute__((naked)) __attribute__((used)) __attribute__((noreturn)) isrBody() {
    asm volatile (
     "AttachedISR:"      "\n\t" // as the scene opens, we have r16 on the stack already, portnumber x 2 in the r16
      "push  r0"         "\n\t" // so we start with a normal prologue
      "in    r0, 0x3f"   "\n\t" // The SREG
      "push  r0"         "\n\t" // on the stack
      "in    r0, 0x3b"   "\n\t" // RAMPZ
      "push  r0"         "\n\t" // on the stack.
      "push  r1"         "\n\t" // We don't need r1 but the C code we call
      "eor   r1, r1"     "\n\t" // is going to want this to be zero....
      "push  r15"        "\n\t" // push r15 (we use it - it's call-saved)
      "push  r17"        "\n\t" // and now we push all call used registers
      "push  r18"        "\n\t" // except r16 which was pused over in WInterrupts_Px
      "push  r19"        "\n\t"
      "push  r20"        "\n\t"
      "push  r21"        "\n\t"
      "push  r22"        "\n\t"
      "push  r23"        "\n\t"
      "push  r24"        "\n\t"
      "push  r25"        "\n\t"
      "push  r26"        "\n\t"
      "push  r27"        "\n\t"
      "push  r28"        "\n\t" // Not call used, but we use it.
      "push  r29"        "\n\t" // same thing.
      "push  r30"        "\n\t"
      "push  r31"        "\n\t"
      ::);
    asm volatile (  // This gets us the address of intFunc in Y pointer reg.
      "add   r26,   r16"  "\n\t" // get the address of the functions for this port (r 16 is 2x the port number)
      "adc   r27,    r1"  "\n\t" // by adding that offset to the address we had the compiler generate the ldi's for
      "ld    r28,     X+" "\n\t" // load the pointer to this port's function array...
      "ld    r29,     X"  "\n\t" // ... to the Y pointer reg.
      "add   r16,   r16"  "\n\t" // double r16, so it is 4x port number - that's the address of the start of the VPORT
      "subi  r16,   253"  "\n\t" // Now this is the address of the VPORTx.INTFLAGS
      "mov   r26,   r16"  "\n\t" // r16 to x reg low byte
      "ldi   r27,     0"  "\n\t" // clear x high byte
      "ld    r15,     X"  "\n\t" // Load flags to r15"
      "sbiw  r26,     0"  "\n\t" // this will set flag if it's zero.
      "breq  AIntEnd"     "\n\t" // port not enabled, null pointer, just clear flags end hit the exit ramp.
      "mov   r17,   r15"  "\n\t" // copy that flags to r17
    "AIntLoop:"           "\n\t"
      "lsr   r17"         "\n\t" // shift it right one place, now the LSB is in carry.
      "brcs  .+6"         "\n\t" // means we have something to do this time.
      "breq  AIntEnd"     "\n\t" // This means carry wasn't set and r17 is 0. - we're done.
      "adiw  r28,    2"   "\n\t" // otherwise it's not a the int we care about, increment Y by 2, so it will point to the next element.
      "rjmp AIntLoop"     "\n\t" // restart the loop in that case.
      "ld    r30,    Y+"  "\n\t" // load the function pointer;
      "ld    r31,    Y+"  "\n\t" // load the function pointer;
      "sbiw  r30,    0"   "\n\t" // zero-check it.
      "breq AIntLoop"     "\n\t" // restart loop if it is, don't call the null pointer
      "icall"             "\n\t" // call their function, which is allowed to shit on any upper registers other than 28, 29, 16, and 17.
      "rjmp AIntLoop"     "\n\t" // Restart loop after.
    "AIntEnd:"            "\n\t" // sooner or later r17 will be 0 and we'll branch here.
      "mov   r16,  r26"   "\n\t" // So when we do this, we end up with VPORTA.FLAGS address in r16
      "ldi   r27,    0"   "\n\t" // high byte is 0, cause we're targeting the VPORT
      "st      X,  r15"   "\n\t" // store to clear the flags....
      "pop   r31"         "\n\t" // clean up a million registers
      "pop   r30"         "\n\t"
      "pop   r29"         "\n\t"
      "pop   r28"         "\n\t"
      "pop   r27"         "\n\t"
      "pop   r26"         "\n\t"
      "pop   r25"         "\n\t"
      "pop   r24"         "\n\t"
      "pop   r23"         "\n\t"
      "pop   r22"         "\n\t"
      "pop   r21"         "\n\t"
      "pop   r20"         "\n\t"
      "pop   r19"         "\n\t"
      "pop   r18"         "\n\t"
      "pop   r17"         "\n\t" // skip 16 again - it's way down at the end, because it was pushed earlier
      "pop   r15"         "\n\t"
      "pop   r1"          "\n\t"
      "pop   r0"          "\n\t"
      "out   0x3b,  r0"   "\n\t"
      "pop   r0"          "\n\t"
      "out   0x3f,  r0"   "\n\t" // between these is where there had been stuff added to the stack that we flushed.
      "pop   r0"          "\n\t"
      "pop   r16"         "\n\t" // this was the reg we pushed back in the port-specific file.
      "reti"              "\n"  // now we should have the pointer to the return address fopr the ISR on top of the stack, so reti're
      :: "x" ((uint16_t)(&intFunc))
      );
      __builtin_unreachable();
  }

  void detachInterrupt(uint8_t pin) {
    /* Get bit position and check pin validity */
    uint8_t bitpos = digitalPinToBitPosition(pin);
    if (bitpos == NOT_A_PIN) {
      return;
    }
    uint8_t port = digitalPinToPort(pin);
    uint8_t p = (port << 5) + bitpos;
    *(((volatile uint8_t*) &PORTA_PIN0CTRL) + p) &= 0xF1; // int off....
    *((volatile uint8_t*) ((uint16_t)((port << 4)+3)))  = (1 << bitpos);// flag clear...
    intFunc[port][bitpos] = NULL; // clear pointer.
  }
/* If not enabling attach on all ports always, instead the near-identical ISR definitions are in the WInterruptsA/B/C/D/E/F/G.c files.
 * Okay, so what the f-- is going on here?
 * To avoid each interrupt vector having it's own lengthy prologue and epilog separaely, which is needed in order for a function call to be made in an ISR
 * All we do is push an upper register onto the stack so can load a value twice the PORT number there, and jump to actual function that does the work here.
 *
 * The isrBody() has two consecutive blocks of inline assembly, First, do what is basically a standard prologue, for something that calls a function (thus there is only one prologue, instead of one per port).
 * We finish the prologue but we need to push the Y pointer and r15 (call saved) for this routine. Then we slip out out of that assembly block just to grab the pointer to IntFunc array, which we need in a pointer reg;
 * The second block of inline ASM specifies that the pointer to the array of pointers to arrays of pointers to interrupt functions be passed in the X pointer register
 *
 * We couldn't have done that any sooner, we had nowhere to put it. To that we add the pre-doubled port number - pointers are 2 bytes so we need that
 * doubling.
 *
 * We can then load the pointer to this port's int functions to the call-saved Y register.
 * Sutract 0 from that pointer to check that it's not 0. A null pointer is of no use. Assuming that there's a pointer, we continue.
 * Next we need the INTFLAGs. We will get them from the VPORT, not the PORT. This is much easier.  We double r16 for address of VPORT and add 3 with subi to
 * get INTFLAGs, copy to r26, and load the 0 high byte to r27, read the flags into r15.
 *
 * Copy flags to r17 and then start the loop. We will check the intflags one at a time. We rightshift once, and check if carry is set, indicating we pushed a 1 out, if so we jump over
 * three instructions handling the case if we it was a 0: We check  then check first for the carry bit. If not, then we check if the zero flag is set, If THAT is then what's left of flags
 * is and we've handled them all, and we jump to end. and if it's not, we increment the Y pointer by 2 and return to start of the loop
 *
 * For each of the ones that we do have a flag for, we load that pointer into Z with postincrement, subtract 0 from it and look at zero flag to make sure it's not null.
 * assuming it's not, we fire icall to call the user function. Either way we then repeat the loop until out of flags.
 * which at latest will happen when we're also at end of the ports intfunc array....
 * Then, with the initial flags still in 15 and the the VPORT adderess in r16 copy that once more to a pointer register, 0 the high byte, and store the flags value we read to clear it.
 * then it's just a matter of making sure we pop everything we pushed onto the stack in the reverse order, including r16 followed by the reti to exit the interrupt..
*/

  #if defined(CORE_ATTACH_ALL)
    ISR(PORTA_PORT_vect, ISR_NAKED) {
    asm volatile(
      "push r16"      "\n\t"
      "ldi r16, 0"    "\n\t"
#if PROGMEM_SIZE > 8192
      "jmp AttachedISR" "\n\t"
#else
      "rjmp AttachedISR" "\n\t"
#endif
      ::);
    __builtin_unreachable();
    }
    #ifdef PORTB_PINS
      ISR(PORTB_PORT_vect, ISR_NAKED) {
      asm volatile(
        "push r16"      "\n\t"
        "ldi r16, 2"    "\n\t"
#if PROGMEM_SIZE > 8192
        "jmp AttachedISR" "\n\t"
#else
        "rjmp AttachedISR" "\n\t"
#endif
        ::);
      __builtin_unreachable();
    }
    #endif
    #ifdef PORTC_PINS
      ISR(PORTC_PORT_vect, ISR_NAKED) {
      asm volatile(
        "push r16"      "\n\t"
        "ldi r16, 4"    "\n\t"
#if PROGMEM_SIZE > 8192
        "jmp AttachedISR" "\n\t"
#else
        "rjmp AttachedISR" "\n\t"
#endif
        ::);
      __builtin_unreachable();
    }
    #endif
  #endif
#else /* This is the old implementation, and it's copyright boilerplate. */

  /* -*- mode: jde; c-basic-offset: 2; indent-tabs-mode: nil -*- */

  /*
  Part of the Wiring project - http://wiring.uniandes.edu.co

  Copyright (c) 2004-05 Hernando Barragan

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General
  Public License along with this library; if not, write to the
  Free Software Foundation, Inc., 59 Temple Place, Suite 330,
  Boston, MA  02111-1307  USA

  Modified 24 November 2006 by David A. Mellis
  Modified 1 August 2010 by Mark Sproul
  (Spence: and presumably someone who didn't sign to adapt it to modernAVRs,
  since nothing that this implementation would work with had been released until
  2016)
  */


  #include <inttypes.h>
  #include <avr/io.h>
  #include <avr/interrupt.h>
  #include <avr/pgmspace.h>
  #include <stdio.h>

  #include "wiring_private.h"

  static volatile voidFuncPtr intFunc[EXTERNAL_NUM_INTERRUPTS];

  void attachInterrupt(uint8_t pin, void (*userFunc)(void), uint8_t mode) {

    /* Get bit position and check pin validity */
    uint8_t bit_pos = digitalPinToBitPosition(pin);
    if (bit_pos == NOT_A_PIN) return;

    /* Get interrupt number from pin */
    uint8_t interruptNum = (digitalPinToPort(pin) * 8) + bit_pos;

    /* Check interrupt number and apply function pointer to correct array index */
    if (interruptNum < EXTERNAL_NUM_INTERRUPTS) {
      intFunc[interruptNum] = userFunc;

      // Configure the interrupt mode (trigger on low input, any change, rising
      // edge, or falling edge)

      switch (mode) {
        case CHANGE:
          mode = PORT_ISC_BOTHEDGES_gc;
          break;
        case FALLING:
          mode = PORT_ISC_FALLING_gc;
          break;
        case RISING:
          mode = PORT_ISC_RISING_gc;
          break;
        case LOW:
          mode = PORT_ISC_LEVEL_gc;
          break;
        default:
          return;
      }

      // Enable the interrupt.

      /* Get pointer to correct pin control register */
      PORT_t *port = digitalPinToPortStruct(pin);
      volatile uint8_t* pin_ctrl_reg = getPINnCTRLregister(port, bit_pos);

      /* Clear any previous setting */
      *pin_ctrl_reg &= ~(PORT_ISC_gm);

      /* Apply ISC setting */
      *pin_ctrl_reg |= mode;
    }
  }

  void detachInterrupt(uint8_t pin) {
    /* Get bit position and check pin validity */
    uint8_t bit_pos = digitalPinToBitPosition(pin);
    if (bit_pos == NOT_A_PIN) return;

    /* Get interrupt number from pin */
    uint8_t interruptNum = (digitalPinToPort(pin) * 8) + bit_pos;

    if (interruptNum < EXTERNAL_NUM_INTERRUPTS) {
      // Disable the interrupt.

      /* Get pointer to correct pin control register */
      PORT_t *port = digitalPinToPortStruct(pin);
      volatile uint8_t* pin_ctrl_reg = getPINnCTRLregister(port, bit_pos);

      /* Clear ISC setting */
      *pin_ctrl_reg &= ~(PORT_ISC_gm);

      intFunc[interruptNum] = 0;
    }
  }

  static void port_interrupt_handler(uint8_t port) {

    PORT_t *portStruct = portToPortStruct(port);
    /* Copy flags */
    uint8_t int_flags = portStruct->INTFLAGS;

    uint8_t bit_pos = PIN0_bp, bit_mask = PIN0_bm;

    /* Iterate through flags */
    while(bit_pos <= PIN7_bp) {

      /* Check if flag raised */
      if (int_flags & bit_mask) {

      /* Get interrupt */
      uint8_t interrupt_num = port*8 + bit_pos;

        /* Check if function defined */
        if (intFunc[interrupt_num] != 0) {

          /* Call function */
          intFunc[interrupt_num]();
        }
      }
      bit_pos++;
      bit_mask = (bit_mask << 1);
    }

    /* Clear flags that have been handled */
    portStruct->INTFLAGS = int_flags;
  }

  #define IMPLEMENT_ISR(vect, port) \
  ISR(vect) { \
    port_interrupt_handler(port);\
  } \


  IMPLEMENT_ISR(PORTA_PORT_vect, PA)
  #if defined(PORTB_PORT_vect)
  IMPLEMENT_ISR(PORTB_PORT_vect, PB)
  #endif
  #if defined(PORTC_PORT_vect)
  IMPLEMENT_ISR(PORTC_PORT_vect, PC)
  #endif
  #if defined(PORTD_PORT_vect)
  IMPLEMENT_ISR(PORTD_PORT_vect, PD)
  #endif
  #if defined(PORTE_PORT_vect)
  IMPLEMENT_ISR(PORTE_PORT_vect, PE)
  #endif
  #if defined(PORTF_PORT_vect)
  IMPLEMENT_ISR(PORTF_PORT_vect, PF)
  #endif
#endif
