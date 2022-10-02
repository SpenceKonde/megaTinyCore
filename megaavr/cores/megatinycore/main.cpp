/* main.cpp - Main loop for Arduino sketches
   Part of megaTinyCore - github.com/SpenceKonde/megaTinyCore
   Copyright (c) 2018~2021 Spence Konde, (c) 2005-2013 Arduino
   Free Software - LGPL 2.1, please see LICENCE.md for details */

#include <Arduino.h>

/* Required by some libraries to compile successfully. Even though it's nonsense in Arduino. */
int atexit(void ( * /*func*/)()) { return 0; }

void initVariant() __attribute__((weak));
/* Weak empty variant initialization function. The purpose is unclear. It sounds like it was intended
 * initialize the variant, and specific variants would have their own implementation. But in practice
 * it seems to be instead used as an initialization callback that libraries can use to run code before
 * setup, like FreeRTOS - it would have been nice if the official API included such a callback. */
void initVariant() { }

void __attribute__((weak)) onPreMain();
void __attribute__((weak)) onPreMain() {
  /* Override with any user code that needs to run WAY early, in .init3 */
}
void __attribute__((weak)) onBeforeInit();
void __attribute__((weak)) onBeforeInit() {
  /* Override with any user code that needs to run before init() */
}
uint8_t __attribute__((weak)) onAfterInit();
uint8_t __attribute__((weak)) onAfterInit() {
  /* Override with any user code that needs to run before interrupts are
   * enabled but after all other core initialization.
   * return 1 to not enable interrupts) */
  return 0;
}







int main()  __attribute__((weak));
/* The main function - call initialization functions (in wiring.c) then setup, and finally loop *
 * repeatedly. If SerialEvent is enabled (which should be unusual, as it is no longer a menu    *
 * option even, that gets checked for after each call to loop). Note that _pre_main() is        *
 * called first in non-optiboot configurations (neither is needed on Optibooot configurations() *
 * an extra bit of initialization code in .init3 to fix the vectors and still happen if user    *
 * overrides main. In the past there was a USB-related function here, that is removed, as work  *
 * will be needed in any event at the core level if VUSB-based "stuff" arrives, but really I'm  *
 * just waiting for the DU-series now                                                           */
int main() {
  onBeforeInit(); // Emnpty callback called before init but after the .init stuff. First normal code executed
  init(); // Interrupts are turned on just prior to init() returning.
  initVariant();
  if (!onAfterInit()) sei();  // enable interrupts.
  setup();
  for (;;) {
    loop();
  }
}




#if (!defined(USING_OPTIBOOT))



/*********************************** CHECK RESET FLAGS ******************************************/
/* If we are not using Optiboot, we need to check the reset flagss, and reset via software for  *
 * a clean start. Unfortunately, if we clear the registers here, we'll prevent user code from   *
 * seeing them, which isn't helpful. As documented in the reset guide, we suggest overriding     *
 * this function with your own version. One example is included below and others in that guide  *
 * init_reset_Flags() should be overridden with one of the ones from the reset guide in any     *
 * production code.                                                                             *
 * If using optiboot, this will never be called, because Optiboot does the same thing.          *
 * By the time app runs, the flags will have been cleared and moved to GPIOR0* (it needs to     *
 * clear flags to honor bootloader entry conditions, so I didn't have a choice about that.      *
 * This function is called before *anything* else, so the chip is a blank slate - or it's       *
 * state is unknown. You're probably running at 4 MHz unless it was a dirty reset, in which     *
 * case it could be anything. No timekeeping is possible, period. The only exception is the     *
 * WDT reset timer with is independent of the HF oscillators and is designed to reset you out   *
 * of hangs amd bad states fthat you end up with when a bug causes the code but not the         *
 * hardware to reset,                                                                           *
 * Interrupts are disabled, or in event of dirty reset                                          *
 * LVL0EX bit will block them. In the event of a clean reset, nothing is set up.  There is no   *
 * PWM, no timekeeping of any millis/micros/delay see no time passing and and all delays,       *
 * delay_microseconds, _delay_ms() and _delay_us(), are the wrong length because they are       *
 * based on F_CPU.                                                                              *
 * If you end up here from a dirty reset, you know nothing about the configuration of the       *
 * peripherals. Check the flags, save them if you need them, and maybe turn on an LED while     *
 * waiting for the WDT to trigger. If you're debugging something really nasty, you can try to   *
 * gather data about the nature of the fault. For example, turn on an LED if\ LVL0EX is set     *
 * meaning you got here from a missing ISR. With one of those little boards with 6 LEDs on      *
 * (many are available reasonably cheaply on aliexpress et al.) end up being very useful        *
 * for this sort of thing.
 *
 * * The register in question is GPIOR0 on megaTinyCore, GPR.GPR0 on Dx-series, but both names  *
 * are aliases of each other per core_devices for compatibility
 */

  /* Minimum: Reset if we wound up here through malfunction - this relies on user clearing the  *
   * register on startup, which is rarely done in Arduino land.                                 */
  void __attribute__((weak)) init_reset_flags() ;
  void __attribute__((weak)) init_reset_flags() {
    uint8_t flags = RSTCTRL.RSTFR;
    RSTCTRL.RSTFR = flags;
    if (flags == 0) {
      _PROTECTED_WRITE(RSTCTRL.SWRR, 1);
    }
    GPIOR0 = flags;
  }

#endif

/* If using SPM from app, but not actually using Optiboot, we need to point the vector tables in the right place.
 * since the "application" is actually split across "boot" and "application" pages of flash... and it's vectors
 * are all in the section defined as "boot" section, tell the interrupt controller that, otherwise nothing'll work!
 * This could just as well be set in init() but for the fact that we support overriding main(). I don't know if
 * anyone who is doing that wants to use my flashwrite library, but it seems plausible.
 * And while we way you need to take full responsibility for setting up the part if you do, nobody is going
 * to figure this out; that's not a reasonable expectation.
 * We also at the same time make sure there's a reset flag. We can't clear it, even though that
 * needs to be done becauwe then it wouldn't be there if user needed it. But we will document the
 * need to clear it and suggest overriding init_reset_flags(), and give the examples.
 */

/* So we need to do 1 or 2 things - as long as we're not using Optiboot, we should force a      *
 * software reset if we don't see any reset flags on startup - init_reset_flags() does that,    *
 * Then if we're using SPM from app, we need to also flip the it that move s the interrupts     *
 * to the start of flash.
 */

/**************************************************************************************************
 * INITIALIZATION FUNCTIONS LOCATED ANYWHERE SPECIAL GO HERE!                                     *
 *                                                                                                *
 * They *MUST* be declared with both the ((naked)) ahd ((used)) attributes! Without the latter,   *
 * the optimizer will eliminate them. Without the former, the sketch will not start...            *
 * Wait what? Yeah, it was generating a and outputting a ret instruction, which caused the        *
 * sketch to return to nowhere under certain conditions and never reach main() at all.            *
 * I do not understand how the old vector fixer allowed the sketch to start ever... but           *
 * since it was only compiled in when flash write was enabled it could have been missed for a     *
 * long time.                                                                                     *
 **************************************************************************************************/

#if (!defined(USING_OPTIBOOT))
  void _initThreeStuff() __attribute__ ((naked)) __attribute__((used)) __attribute__ ((section (".init3")));
  // this runs, as the name implies, before the main() function is called.
  #if !defined(SPM_FROM_APP)
    // If we're not doing the SPM stuff, we need only check the flags
    void _initThreeStuff() {
      init_reset_flags();
      onPreMain();
    }
  #else
    // if we are, we also need to move the vectors. See longwinded deascription above.
    void _initThreeStuff() {
      init_reset_flags();
      onPreMain();
    }
  #endif
#else
  void _initThreeStuff() __attribute__ ((naked)) __attribute__((used)) __attribute__ ((section (".init3")));
  void _initThreeStuff() {
    onPreMain();
  }
#endif
