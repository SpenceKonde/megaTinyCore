# Pin interrupts
While the usual `attachInterrupt()` functionality is provided by megaTinyCore (and works on every pin, with every type of trigger), these will take longer to run, and use more flash, than an equivalent interrupt implemented manually (due to the need to check for all 8 pins - while a manually implemented scheme would know that only the pins configured to generate interrupts need to be checked; that takes both time and flash space). Additionally, there are common use cases (for example, reading rotary encoders, particularly more than one) where each pin being handled separately prevents convenient shortcuts from being taken. Worst of all, attaching any pin interrupt through the API causes the core to glom onto EVERY pin change interrupt. I need to fix this, or improve it, or something, but it's a very tricky problem!

It is of course even more of a problem on the Dx-series parts any call to attachInterrupt will block off all 56 interrupts, instead of just 22 at most.

See also: [InterruptVectorNames.md](InterruptVectorNames.md)

For these reasons, it is usually desirable and often necessary to manually implement a pin interrupt instead of using `attachInterrupt();`

## The big attachInterrupt() issue
If your sketch or any library it included calls attachInterrupt(), it takes control of all pin interrupt vectors! Any use of `ISR()` to manually define one of those vectors will fail to compile. Do not use attachInterrupt unless you absolutely have to. And if you do, that's all you can use. Interrupts that fire through attachInterrupt have over 40 clock cycles more overhead compared to a manually defined interrupt - just because they call an attached function - the code that chooses which attached interrupt function to call takes at least as long.

The most problematic library that uses attachInterrupt is SoftwareSerial (mostly because it is the most widely used, though it happens to also be awful in many other ways).

Note that this means that any library that needs to directly define a pin interrupt (for example, to minimize code size or meet response time requirements) is incompatible with any library that uses attachInterrupt, and vice versa. This is an unfortunate consequence of the design decisions made when the attachInterrupt API was written by Arduino. Unfortunately, time machines remain stubbornly out of reach, so we can't fix the attachInterrupt API.

megaTinycore and DxCore have attempted to do the next best thing, and starting from 2.5.0 (though it is broken in 2.5.0 through 2.5.10) implemented two new modes for attachInterrupt(). The default ("All ports") mode uses a slightly more efficient implementation, but suffers from the same essential flaw, that it blocks off every available interrupt - though it is slightly faster, wastes about 50 fewer bytes per port, and the minimum time between the same interrupt trigger being set twice without missing either of them is shorter, because the flags are cleared before calling user code instead of after. The "manual mode" uses the new implementation - with the added caveat that you must call attachPortAEnable() (replace A with the letter of the port you need to attach to) before you call attachInterrupt(). Any number of ports present on the part can be enabled, and only those ports will have the interrupt used by it. If you needed to put a SoftwareSerial port on PORTA, you could select manual attach mode, call attachPortAEnable() before you called MySoftSerial.begin(), and still also use `ISR(PORTB_PORT_vect) {...}` to define a vector.

There are two versions of the new implementation for "all ports" and "manual":
  * "Late Clear" clears the flags *after* executing all attached interrupts. It takes a few clocks longer to enter and exit the interrupt, a few bytes more flash, and it is more likely to miss two interrupts that happen in rapid succession. This is should be compatible with all code that works with normal attachInterrupt.
  * "Early Clear" clears the flags *before* executing *any* attached handlers. This saves around 10 clock cycles, and reduces the chance of missing the second of two interrupts which occur in rapid succession. However, if you need to disable the interrupt within the ISR (for example, when using a LOW LEVEL interrupt), you must clear the flag after you turn off the interrupt.

To deal with the fact that this has been a very rough road to get working, there is also an option in the submenu to fall back to the stock attachInterrupt implementation, with all of it's disadvantages.

**The methods described below are for manually implementing performant flash-efficient pin interrupts that do not use attachInterrupt(), which is fully is covered by the official Arduino reference** except that they don't talk about the response time or the flash usage compared to doing it yourself.

## Manually implementing pin interrupts
The system for interrupts on tinyAVR 0/1-series parts is different from, and vastly more powerful than that of the classic AVR parts. Unlike classic AVR parts, there are no special interrupt pins (INT0, INT1, etc.) - instead, all pins can be configured to generate an interrupt on change, rising, falling or LOW level. While all pins on a port are handled by the same ISR (like classic AVR PCINT's), the pin that triggered the interrupt is recorded in the INTFLAGS register, making it easy to determine which pin triggered the interrupt.

### Enabling the interrupt
The pin interrupt control is handled by the PORTx.PINnCTRL register.
Bits 0-2, the ISC ("Input Sense Configuration") control interrupt and input sense behavior:

* 000 `PORT_ISC_INTDISABLE_gc` = no interrupt, normal operation, the default.
* 001 `PORT_ISC_BOTHEDGES_gc` = interrupt on change
* 010 `PORT_ISC_RISING_gc` = interrupt on rising
* 011 `PORT_ISC_FALLING_gc` = interrupt on falling
* 100 `PORT_ISC_INPUT_DISABLE_gc` = digital input buffer disabled entirely, which happens to also disable the interrupt (equivalent of DIDR register on classic AVRs that have it)
* 101 `PORT_ISC_LEVEL_gc` = interrupt on LOW level - fires continuously as long as low level is held. This is quite common in the world at large; in many consumer electronics, holding a button has no effect until you release it - this is the simplest implementation that leads to this behavior (though some debounce algorithms do too)

Bit 3 controls the pullup.

Bit 3 is set when `pinMode()` is used to set the pin to INPUT_PULLUP. When manually writing the PINnCTRL registers, be sure to either use bitwise operators to preserve this bit, or set it to the correct value (the former is easier to remember, but the latter is faster)

#### The rest of PINnCTRL - in the event that you need to know it now:

Bits 4 and 5 are reserved (aka, unused).

Bit 6 is reserved on parts without MVIO, and controlls the input level threshold on parts with MVIO. See the DxCore documentation or the PORT chapter of the AVR DB or AVR DD series datasheet for more information.

Bit 7 is INVEN (INVert ENable); when set, the pin value is inverted - writing a 0 to an OUTPUT pin will drive it HIGH instead of LOW, and reading a HIGH means the pin is LOW. It does not, however, turn the pullup into a pulldown - but just about everything else is inverted, including the output of a peripheral when overriding the pin function.

### Disabling the interrupt
Clear the lowest three bits of the PINnCTRL register:

```cpp
PORTx.PINnCTRL &= ~(0b00000111);
PORTx.PINnCTRL &= ~0x07;
PORTx.PINnCTRL &= ~PORT_ISC_gm;
// The above three are equivalent.

// If you know what the value of this register should be with the interrupt off (usually 0x00 if pullup is not on, 0x08 if it is)
// you can write that directly to save a few bytes of flash, at the cost of making the code harder to read.
PORTx.PINnCTRL = 0x08; // If this pin has the pullup turned on, and we just want to turn off the interrupt.
```

### All pins can generate interrupts
However, they are not all equally capable, and there is only one ISR per port.

Unlike classic AVRs, there is no analog of the `INT0` and `INT1` pins dedicated to a single pin - so the modern parts give all pins the capabilities of the `INTn` pins, while having the shared ISR behavior of the PCINTs - with the added bonus that some pins can even differentiate between rising and falling edges while in power-down sleep mode.

**Be sure to read the section on fully and partially asynchronous pins below** before choosing a pin to use. Pins that are number 2 or 6 within a port behave slightly differently, and have enhanced capaiblilities, but also greater noise sensitivity.

### The ISR
Each port has one interrupt vector; their names are:

* PORTA_PORT_vect
* PORTB_PORT_vect
* PORTC_PORT_vect

When the interrupt condition occurs, the bit int PORTx.INTFLAGS corresponding to the interrupt will be set. At any point in time, interrupts are enabled, no interrupt is currently executing, and any bit in any PORTx.INTFLAGS bit is set, the interrupt will be triggered. If multiple pins in a port are used for interrupts corresponding to different things, you can use this to determine which pin triggered the interrupt. Note that if two pins changed almost simultaneously, or if there was a delay before entering the interrupt (maybe an interrupt was already running, or you had interrupts disabled in order to perform a time critical task like writing to WS2812 LEDs), more than one bit can be set when the ISR is called. Whenever you manually define a pin interrupt **YOU MUST CLEAR THE INTFLAG BIT(s) WITHIN THE ISR**. The interrupt will continue to be generated as long as the flag is set and it is not in an interrupt, so if you do not unset it, the ISR will run continuously after it was triggered once; to an observer, this looks likethe sketch slowing to a crawl as soon as the interrupt fires once: AVR will always execute one non-interrupt instruction after it leaves an interrupt.

To clear the bit, write a 1 to it; writing 255 to it will clear all of them. You can clear either the `PORTx.INTFLAGS` register or the `VPORTx.INTFLAGS` register. The latter is highly favorable if you can do `VPORTx.INTFLAGS |= 1<<n` where x is a constant port letter, and n is a constant bit number, and slightly favorable if you're doing `VPORTx.INTFLAGS = y` to clear multiple flags at once (hopefully a constant subset of them). However, if the port is somehow not a compiletime known constant, it holds no advantage whatsoever, and being able to work with the full on PORTx structs instead of the VPORTs is frequently convenient (most notably if you need to turn the interrupt off during the ISR).

I do not, off-hand, know what the behavior is if an INTFLAG is not cleared, but the interrupt is turned off by clearing bits in PINnCTRL; there aren't any other registers quite like the pin interrupts on these parts. In *other* cases, there is also a register to turn each interrupt source on or off, and the INTFLAG is set regardless of whether it's enabled, and the interrupt only fires if both are set. However, there is no interrupt enable bit for pin interrupts, just PINnCTRL. If you find out incidentally, that is a defect in your code. You should always clear the INTFLAGs in the interrupt.

**Note:** The intflags are cleared for you by attachInterrupt, regardless of the mode (see previous section) that you have set attachInterrupt to, so you need not clear them during an "attached" interrupt you have it set to an "early clear" mode and are disabling the interrupt within the ISR (since you don't want the disabled interrupt to fire again afterwards because the flag got set between it's being cleared and your code running). attachInterrupt takes a snapshot of the flags near the beginning, and that is what is cleared (see the [interrupt reference](Ref_Interrupts.md) for a bit more on this).

### A basic example for the x16/x06
This code demonstrates using pin interrupts - it will work on any tinyAVR 0/1/2-series, megaAVR 0-series, or DxCore part, though parts without a PB0 or PC2 will need to adjust those the lines marked with a `<---` to account for the different pin and/or port.
```cpp
unsigned long previousMillis;
byte ledState;
volatile byte interrupt1 = 0; // changed in ISR, so must be volatile or compiker will optimize it away.
volatile byte interrupt2 = 0;
volatile byte interrupt3 = 0;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(PIN_PC2, INPUT_PULLUP);  // pin 12  <--- Must change pin if PORTC is not present
  pinMode(PIN_PA2, INPUT_PULLUP);  // pin 14
  pinMode(PIN_PA1, INPUT_PULLUP);  // pin 15
  pinMode(PIN_PB0, INPUT_PULLUP);  // pin 9   <--- Must change pin if PORTB is not present
  PORTC.PIN2CTRL  = 0b00001101; //PULLUPEN = 1, ISC = 5 trigger low level     <--- Change if no PORTC
  PORTA.PIN1CTRL  = 0b00001010; //PULLUPEN = 1, ISC = 2 trigger rising
  PORTA.PIN2CTRL |= 0x02; //ISC = 2 trigger rising - uses |= so current value of PULLUPEN is preserved
  // Note: uses 4 bytes more flash than the first two examples, plus the overhead of calling pinMode().
  PORTB.PIN0CTRL  = 0b00001001; //PULLUPEN = 1, ISC = 1 trigger both          <--- Change if no PORTB
  Serial.begin(115200);
  delay(10);
  Serial.println("Startup");

}

void loop() {
  if (interrupt1){
    interrupt1 = 0;
    Serial.println("I1 fired");
  }
  if (interrupt2){
    interrupt2 = 0;
    Serial.println("I2 fired");
  }
  if (interrupt3){
    interrupt3 = 0;
    Serial.println("I3 fired");
  }
  //BlinkWithoutDelay, just so you can confirm that the sketch continues to run.
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= 1000) {
    previousMillis = currentMillis;
    if (ledState == LOW) {
      ledState = HIGH;
    } else {
      ledState = LOW;
    }
    digitalWrite(LED_BUILTIN, ledState);
  }
}

ISR(PORTA_PORT_vect) {
  byte flags = PORTA.INTFLAGS;
  PORTA.INTFLAGS = flags; //clear flags
  if (flags & 0x02) {
    interrupt1 = 1;
  }
  if (flags & 0x04) {
    interrupt2 = 1;
  }
}

ISR(PORTB_PORT_vect) {  //  <--- Change if no PORTB
  PORTB.INTFLAGS = 1; //we know only PB0 has an interrupt, so that's the only flag that could be set.
  interrupt3 = 1;
}

ISR(PORTC_PORT_vect) {  //  <--- Change if no PORTC
  _PROTECTED_WRITE(RSTCTRL.SWRR, 1); //make this pin into an ersatz reset pin.
}
```

### Synchronous and Asynchronous pins
Certain pins (pin 2 and 6 in each port) are "fully asynchronous" - These pins have several special properties:
* They can be triggered by conditions which last less than one processor cycle.
* They can wake the system from sleep on change, rising, falling or level interrupt. Other pins can only wake on change or level interrupt.
* If the pin is not driven HIGH, and you turn on the pullup and the interrupt within the same write to the PINnCTRL register, that will usually trigger the interrupt on a fully asynchronous pin. This generally undesirable behavior can happen on other pins, but it will happen any time that the pin isn't already HIGH on a fully async pin).

In the example above, note that the interrupts on pin 14 and 15 (PA1 and PA2) are configured identically. However, if the part was put to sleep, only the one on pin 15/PA2 would be able to wake the part, as they trigger on rising edge, and only PA2 is a fully asynchronous pin.

So which kind of pin to choose?
1. If you must wake the part from power down or standby sleep, and it is most convenient to specify a rising or falling edge (instead of "any edge" or a low level), you need to use a fully asynchronous pin.
2. If you expect there to be pulses shorter that 1 system clock cycle on the pin, AND you want those to trigger an interrupt, you need to use a fully asynchronous pin.
3. If you do *not* expect a valid interrupt condition on the pin to be less than 1 system clock cycle in duration, you can use any pin.
4. If you have a concern about very short (sub-clock cycle) spikes triggering the interrupt in error, you should NOT use a fully async pin.


#### What about that "dead time" in the datasheet?
The "dead time" mentioned in the datasheet should be ignored. As far as I can tell, it is almost impossible to contrive a situation where it matters; that is the minimum delay beweeen when the intflag is cleared, and when it can become set again do recall that returning from an interrupt takes 4 clocks). When I finally succeeded in making it manifest, I discovered that it is (apparently) the opposite of what the datasheet said. It's the fully async pins that have the "dead time"; I do not think it is a coincidence that the duration of the "dead time" of 3 clock cycles is the same amount of time that it takes to synchronize something to the main clock domain.
