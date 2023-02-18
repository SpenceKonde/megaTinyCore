/*  (C) Spence Konde 2022 open source (LGPL2.1 see LICENSE.md)
 * wiring_extra.cpp contains a few functions that are the sort of thing you'd expect in wiring.c or similar
 * but which need to be in a cpp file because they use overloading or are just not closely related to any of
 * those files' other functions.
 *
 *************************************************************/
#include "Arduino.h"

#if defined(LTODISABLED)
  void badArg(__attribute__((unused))const char* c) {
    return;
  }
  void badCall(__attribute__((unused))const char* c) {
    return;
  }
#endif
void badCore(const char*)   __attribute__((error("")));
#if !defined(LTODISABLED)
void checkCore(uint8_t a) {
  if (!__builtin_constant_p(a)) {
    badCore("If LTO is disabled, you *MUST* inform the core by defining DISABLELTO.");
  }
}
void _checkCore() {
  checkCore(1);
}
#endif
void __pinconfigure(const uint8_t digital_pin, uint16_t pin_config) {

  uint8_t bit_mask = digitalPinToBitMask(digital_pin);
  if(bit_mask == NOT_A_PIN || !pin_config) // Return if digital pin is invalid or the other parameters or out to zero
    return;

  uint8_t bit_pos  = digitalPinToBitPosition(digital_pin);
  volatile uint8_t *portbase = (volatile uint8_t*) digitalPinToPortStruct(digital_pin);

  // Write to selected pin direction register
  uint8_t setting = pin_config & 0x03; // Mask out direction bits (DIR, DIRSET, DIRCLR, DIRTGL)
  if(setting)
    *(portbase + setting) = bit_mask;

  // Write to selected output register
  pin_config >>= 2;
  setting = pin_config & 0x03;
  if(setting)
    *(portbase + 4 + setting) = bit_mask;

  // Return if there is nothing more to configure
  if(!(pin_config & 0x3FFC))
    return;

  /* Now it gets to the fun part: The bits of PINnCTRL!
   * For this we need to disable interrupts
   * (for efficiency) we only read the register once
   * modify that value, then write it back at the end.
   */

  uint8_t oldSREG = SREG;             // Store SREG
  cli();
  pin_config >>= 2;
  uint8_t pinncfg = *(portbase + 0x10 + bit_pos);
  // Input sense configuration (ISC)
  if(pin_config & 0x08)
    pinncfg = (pinncfg & 0xF8) | (pin_config & PORT_ISC_gm);
  // Pullup resistor
  uint8_t temp = pin_config & 0x30;
  if(temp)
  {
    if(temp == 0x30)
      pinncfg ^= PORT_PULLUPEN_bm;    // Toggle pullup
    else if(temp == 0x20)
      pinncfg &= ~(PORT_PULLUPEN_bm); // Clear pullup
    else
      pinncfg |= PORT_PULLUPEN_bm;    // Set pullup
  }
  // Invert pin
  pin_config >>= 8;
    #ifdef PORT_INLVL_bm
    /* This feature is present on the AVR DB/DD-series, and headers suggest EA-series.
     * See Ref_digital.md in the online documentation for more information.
     *
     * Set or clear only - toggle not supported.
     * I question the usefulness of the other PINnCTRL toggles./
     * I do not question the value of INLVL toggle: I am almost certain it is nil.
     * If you want a toggle INLVL option, you're in an X-Y problem and are going about something
     * sufficiently wrong that giving you what you think you need does you a disservice.
     * If you disagree with this design decision please create an issue and explain the use case that would make this
     * a worthwhile feature.
     */
    temp = pin_config & 0x03;
    if (temp) {
      if (temp == 0x01) {
        pinncfg |= 0x40;              // set INLVL
      } else {
        pinncfg &= ~(0x40);           // clear INLVL
      }
    }
  #endif
  temp = pin_config & 0x0C;
  if(temp)
  {
    if(temp == 0x0C)
      pinncfg ^= PORT_INVEN_bm;       // Toggle INVEN
    else if(temp == 0x08)
      pinncfg &= ~(PORT_INVEN_bm);    // Clear INVEN
    else
      pinncfg |= PORT_INVEN_bm;       // Set INVEN
  }
  // Write to PINnCTRL register
  *(portbase + 0x10 + bit_pos) = pinncfg;

  // Restore SREG
  SREG = oldSREG;
}
/* Error Checking */
void _pinconfigure(uint8_t pin, uint16_t pin_config) {
  check_valid_digital_pin(pin);
  #if defined(HARDWIRE_INPUT_ONLY)
    if   (__builtin_constant_p(pin) && pin == HARDWIRE_INPUT_ONLY) {
      if (__builtin_constant_p(pin_config)) {
        if (pin_config & 0x01) {
          badArg("This pin cannot be set as an output because of hardware constraints on the board selected and must always be INPUT only");
        }
      } else {
        if (pin_config & 0x01) {
          pin_config = pin_config & 0xFFFC;
        }
      }
    } else {
      if (__builtin_constant_p(pin_config)) {
        if (pin_config & 0x01) {
          if (pin == HARDWIRE_INPUT_ONLY){
            pin_config = pin_config & 0xFFFC;
          }
        }
      } else {
        if (pin_config & 0x01 && pin == HARDWIRE_INPUT_ONLY) {
          pin_config = pin_config & 0xFFFC;
        }
      }
    }
  #endif
  __pinconfigure(pin,pin_config);
}
/* Allow calls with manually OR'ed constants */
void pinConfigure(uint8_t digital_pin, uint16_t pin_config) {
  _pinconfigure(digital_pin, pin_config);
}
