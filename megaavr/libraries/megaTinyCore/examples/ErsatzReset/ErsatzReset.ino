// Ersatz Reset Pin Demo
// For upload via Optiboot (for example) wire DTR or RTS to the Ersatz Reset pin

#ifdef __AVR_ATtinyxy2__
  #define ERSATZ_RESET_PIN PIN_PA7
  #define RESET_MASK (1 << 7)
  #define PINCTRL PORTA.PIN7CTRL
#else
  #define ERSATZ_RESET_PIN PIN_PA4
  // Always arduino pin 0
  #define RESET_MASK (1 << 4)
  #define PINCTRL PORTA.PIN4CTRL
#endif

// We will always use pin 0 as the ersatz reset pin, EXCEPT on the 8-pin parts.
// We can't use PA4 since it doesn't exist there. PA6, PA7, PA1 and PA2 are serial pins
// so not ideal choices, and PA3 is right off the menu (for this code, but not for you)
// because we use these sketches for CI testing, and one of the configurations that we use
// for that testing is with an external clock - so we can't use the CLKI pin or the CI
// will fail, and spurious failures like that are treated as release blocking issues
// because of their tendency to hide non-spurrious errors.
// In realworld situations, on the 8-pin parts, the correct pin to use is "whatever pin you
// can spare" (if any).

// Pin Choice: There are two types of interrupt pins on the tinyAVR parts. Pins 2 and 6 on every
// port are said to be "Fully Asynchronous". This allows them to react to events shorter than a
// system clock cycle, and to wake the part from deep sleep modes on "rising" and "falling"
// edges as well as "low" level and "change" (both edges). The other pins are termed "Partially
// Asynchronous". While fully async pins have obvious advantages, they also have a non-obvious
// disadvantage: In order to respond to such brief events, they necessarily are more sensitive
// to transients; in the case of ErsatzReset (or really any interrupt that relies on a pin being
// pulled up in the normal state), if you're forced to use a fully async pin, you may find that
// you need an external, physical, pullup resistor instead of just the internal one.
// Certainly, you can't use a single write to the PINnCTRL register to both enable the interrupt
// and turn on the pullups (you shouldn't ever - *but you could sometimes do this on partially
// async pins* Not reliably mind you. )

void setup() {
  #ifndef __AVR_ATtinyxy2__
  pinMode(LED_BUILTIN, OUTPUT);
  #endif
  pinMode(ERSATZ_RESET_PIN, INPUT_PULLUP); // on 8-pin parts, this overrides the previous line! No LED blink on these; not enough pins to demo everything here...
  #if !defined(MILLIS_USE_TIMERNONE) // if we have millis, use that
  while ((!(VPORTA.IN & RESET_MASK)) && millis() < 10); // wait up to 10 milliseconds for pin to go high...
  #else // if millis is disabled, something like this achieves much the same thing, since delay() still does work with millis disabled.
  uint8_t ms = 10;
  while ((!(VPORTA.IN & RESET_MASK)) && ms) {
    delay(1);
    ms--;
  }
  #endif
  // MUCH longer than we need to wait...
  if (!(VPORTA.IN & RESET_MASK)) { // if still not high, reset...
    _PROTECTED_WRITE(RSTCTRL.SWRR, 1);
  } else {
    PINCTRL = PORT_PULLUPEN_bm | PORT_ISC_LEVEL_gc;
  }
}

void loop() {
  #ifndef __AVR_ATtinyxy2__
  digitalWrite(LED_BUILTIN, 1);
  delay(1000);
  digitalWrite(LED_BUILTIN, 0);
  delay(1000);
  #endif
}

ISR(PORTA_PORT_vect) {
  uint8_t flags = PORTA.INTFLAGS;
  PORTA.INTFLAGS = flags; // clear flags
  if (flags & RESET_MASK) {
    _PROTECTED_WRITE(RSTCTRL.SWRR, 1);
  }
}
