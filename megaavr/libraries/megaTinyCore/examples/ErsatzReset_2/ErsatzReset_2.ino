// Ersatz Reset Pin Demo 2, which is the same except that it resets using the functions provided by megaTinyCore.h.

#include <megaTinyCore.h>

#ifdef __AVR_ATtinyxy2__
  #define ERSATZ_RESET_PIN    PIN_PA7
  #define RESET_MASK          (1 << 7)
  #define PINCTRL             (PORTA.PIN7CTRL)
#else
  #define ERSATZ_RESET_PIN    PIN_PA4
  #define RESET_MASK          (1 << 4)
  #define PINCTRL             (PORTA.PIN4CTRL)
#endif

// We will always use pin 0 as the ersatz reset pin, EXCEPT on the 8-pin parts.
// We can't use PA4 since it doesn't exist there, and pin 0 is one of the serial pins
// depending on the configuration of USART0, so that would be a bad choice. Instead we
// will use PA3, arduino pin 4, the pin between UPDI and geound on the chip.

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
  if (!(VPORTA.IN & RESET_MASK)) { // if still not high, reset...
    ResetViaWDT(); // here, reset pin is still held low at startup. If we are using optiboot we already ran the bootloader.
    // Say we don't want to keep doing that. A reset via WDT will never run optiboot
  } else {
    // This works, but **only** because I had already set the same pin to enable the pullup and waited for it to go high (10 millis is ridiculously long.
    // On an unloaded pin, it's under a microsecond)
    // Otherwise, we would need to turn on the pullup, wait until the pin read high, and then write to set the pin to interrupt on low level.
    PINCTRL = PORT_PULLUPEN_bm | PORT_ISC_LEVEL_gc;
    // This happens much more visibly on fully async pins than partially async pins.
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
  byte flags = PORTA.INTFLAGS;
  PORTA.INTFLAGS = flags; // clear flags
  if (flags & RESET_MASK) {
    ResetViaSoftware();
  }
}
