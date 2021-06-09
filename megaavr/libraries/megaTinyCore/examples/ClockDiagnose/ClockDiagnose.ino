#ifndef MEGATINYCORE
#error "This sketch ONLY supports megaTinyCore and the modern tinyAVR line");

#define FALLBACK20 (19200*((1.0*F_CPU)/3333333.0))
#define FALLBACK16 (19200*((1.0*F_CPU)/2666666.7))

#include <megaTinyCore.h>
#include <util/delay.h>

void setup() {
  // pinMode (MyButtonPin,INPUT_PULLUP)
  _delay_ms(1000); //gives you a chance to get the serial monitor open.
  // while (digitalRead(MyButtonPin) == HIGH)
  // uncomment the lines above and below it and connect a button between that pin and ground to make this wait until you press the button.
  byte errortype=0;
  #if (CLOCKSOURCE == 2)
    if ((CLKCTRLK.MCLKSTATUS & CLKCTRL_EXTS_bm) && (CLKCTRL.MCLKCTRLB == 0)) {
      Serial.begin(19200);
      Serial.println("Internal oscillator not used, using external. If you can read this, the clock is working correctly. If not, the clock is not the same frequency that you selected from the menu.");
      Serial.println("If another sketch has timing related problem, either the sketch is doing something to break timekeeping, or it was uploaded with a different clock speed selected");
      Serial.flush();
      Serial.end();
      errortype |= 128;
      errortype |= 2;
    } else {
      errortype |= 1; // problem with external clock!
      if (!(CLKCTRLK.MCLKSTATUS & CLKCTRL_EXTS_bm)) {// is external clock still not up?
        errortype |= 2;
        if (CLKCTRLK.MCLKSTATUS & CLKCTRL_SOSC_bm) { // and trying?
          errortype |= 4;
        }
        errortype |= 128; //using fallback clock.

      else {
        _PROTECTED_WRITE(CLKCTRL_MCLKCTRLB, 0x00);
        }
      }
    }
  #elif (CLOCKSOURCE == 0) //internal oscillator
    if ((CLKCTRL_MCLKCTRLA & 0x03) !=  CLKCTRL_CLKSEL_OSC20M_gc) {
      //core thinks it is using iinternal, but hardware set for external, likely to do user modification of initialization routines.
      // set it back to to the configured frequency
      _PROTECTED_WRITE(CLKCTRL_MCLKCTRLB, (CLKCTRL_PEN_bm | CLKCTRL_PDIV_16X_gc));
      _PROTECTED_WRITE(CLKCTRL_MCLKCTRLA, CLKCTRL_CLKSEL_OSC20M_gc);
      while(CLKCTRL.MCLKSTATUS & CLKCTRL_SOSC_bm);

      #if (F_CPU == 20000000)
        /* No division on clock */
        _PROTECTED_WRITE(CLKCTRL_MCLKCTRLB, 0x00);
      #elif (F_CPU == 10000000 || F_CPU == 8000000) // prescaled by 2
        /* Clock DIV2 */
        _PROTECTED_WRITE(CLKCTRL_MCLKCTRLB, (CLKCTRL_PEN_bm | CLKCTRL_PDIV_2X_gc));
      #elif (F_CPU == 5000000 || (F_CPU == 4000000) // prescaled by 4
        _PROTECTED_WRITE(CLKCTRL_MCLKCTRLB, (CLKCTRL_PEN_bm | CLKCTRL_PDIV_4X_gc));
      #elif (F_CPU == 1000000) //16MHz prescaled by 16
        _PROTECTED_WRITE(CLKCTRL_MCLKCTRLB, (CLKCTRL_PEN_bm | CLKCTRL_PDIV_16X_gc));
      #else
        _PROTECTED_WRITE(CLKCTRL_MCLKCTRLB, (CLKCTRL_PEN_bm | CLKCTRL_PDIV_6X_gc));
        errortype |= 128; //we are now at fallback frequency.
      #endif
      errortype |= 8;
    }
    #if (!defined(CLOCK_TUNE_INTERNAL))
      #if F_CPU == 20000000 || F_CPU == 10000000 || F_CPU == 5000000
        if (FUSE.OSCCFG == 1) {
          errortype |= 2;
        }
      #else
        if (FUSE.OSCCFG != 1) {
          errortype |= 4;
        }
      #endif
      uint8_t errormgs=GPIOR0 & 0xC0;
      if (errormsgs == 0x80) {
        // Tuning determined this specimen cannot reach or does not run at this speed.
        errortype |= 128;
        errortype |= 16;
      } else if (errormsgs == 0xC0) {
        // Trying to achieve top speed possible on 20 MHz osc with the 16  MHz one.
        errortype |= 32;
        errortype |= 128;
      } else if (errormsgs == 0x40) {
        errortype |= 64;
      }
    #endif //if tuning enabled.
  #endif //end of clock-source dependand diagostics

  // Now that we know what, if anything, is wrong, let's set up serial so it works and tell the user.
  if (!(errortype & (128|64))) {
    if (errortype & 1) {
      Serial.begin(19200);
      Serial.println("Chip configured to use external oscillator - but there was a problem");
      Serial.println("It is running from it now, but init() waits for around 70-80 clocks for the switch which is supposed to happen in 2\r\nImplies there was no ext. clock at that point, meaning the chip started before the oscillator. In this case\r\ninit_clock() gave up, so that the sketch would run and you'd see a timing problem.\r\nYou can increase the delay before chip starts running code with the Startup Time option in tools menu, \r\nif power is slow rising, use BOD to keep it from running until oscillator's minimum voltage reached.\r\nRecommend reviewing oscillator datasheet as well.");
    } else if (errortype & 2) {
      Serial.begin(24000); //(19200*20/16 it thinks it's running faster than it is, so higher baud ask for a higher baud rate than we want/)
      Serial.println("Your part is running at an 16 MHz-derived speed, from internal w/out tuning, but selected F_CPU 20 MHz derived\r\nEither reburn bootloader with the desired speed selected, or run the tuning sketch (see docs) and use 20 MHz (tuned).\e\nwhich will give 20 MHz even if the osccfg fuse is set to 16.");
    } else if (errortype & 4) {
      Serial.begin(15360); //(19200 * 16/20 - it thinks it's running more slowly so ask for a lower speed.)
      Serial.println("Your part is running at an 20 MHz-derived speed, from internal w/out tuning, but selected F_CPU 16 MHz derived\r\nEither reburn bootloader with the desired speed selected, or run the tuning sketch (see docs) and use 16 MHz (tuned).\r\nwhich will give 16 MHz even if the osccfg fuse is set to 20.");
  }
  if (errortype & (128|64)) { //would be an else except we need to end up here if 128 not set and 16 is......
    if (errortype & 64) {
      Serial.begin(19200);
      Serial.println("Tuned internal selected, but tuning sketch has not run successfully. If you can read this, our guess was close enoguh for UART to work though!");
      Serial.flush();
      Serial.end();
    }
    _PROTECTED_WRITE(CLKCTRL_MCLKCTRLB, (CLKCTRL_PEN_bm | CLKCTRL_PDIV_6X_gc));
    _PROTECTED_WRITE(CLKCTRL_OSC20MCALIBA,(FUSE.OSCCFG == 1 ? SIGROW_OSCCAL16M0 : SIGROW_OSCCAL20M0))
    // make sure we are REALLY in fallback mode!
    Serial.begin(FUSE.OSCCFG == 1 ? FALLBACK16 : FALLBACK20);
    Serial.println("Oscillator in fallback mode");
    if (errortype & 1) {
      Serial.println("Chip configured to use external oscillator - but there was a problem":);
      if (errortype & 4) {
        Serial.println("it isn't running from it now either but still wants to - Sounds like your oscillator isn't oscillating, probably a connection problem.");
        Serial.println("Reminder: These parts do not work with a crystal. only with an external *clock* or \"oscillator\".");
      } else if (errortype & 2 ) {
        Serial.println("It's not running from external now - but it also looks like it may not have even tried, or tried and then switched back.\r\nThese parts don't have CFD. Have you modified the initialization routines?");
      }
    } else if (errortype & 2) {
      Serial.println("Using external, not internal oscillator. If you can read this, but not the previous lines, the oscillator speed does not match selected clock speed.");
    }
    if (errortype & 16) {
      Serial.println("Tuning has been run on this chip.\r\nWhen it was run, it determined that the requested can't be reached by this specimen,\n\nsee table below.\r\n Some parts ovberclock better than others; this is one of the bad ones.\r\n Try a different opart, or lower speed.");
    }
    if (errortype & 32) {
      Serial.println("The OSCCFG fuse is set to 16 MHz however a tuned speed has been requested that can only be reached with the 20 MHz oscillator.");
    }
    if (errortype & 8) {
      Serial.println("Core configured to use the internal oscillator, but a modified initialization routine configured it for an external one.\r\n This will break all core timekeeping, so don't do that.");
    }
    if (errortype & 64) {
      Serial.println("Tuned internal selected, but tuning sketch has not run successfully.If you can read this, but got a wad of gibberish before, ");
    }
  }
  printTuningStatus();
}
void loop(){
  ;
}
