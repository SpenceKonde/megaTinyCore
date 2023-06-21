# Improved Digital I/O Functionality
This core includes a number of features to provide more control or performance when doing digital I/O. This page describes how to use them. All of the options that can be configured for a pin are exposed. The only things that aren't exposed are the slew rate limiting feature, and the multi-pin configuration facilities. The slew rate limiting is can only be configured on a per port basis; turning it on and off is so simple (see below) that it needs no wrapper. The multi-pin configuration system does not have an obvious "right way" to expose it and should be handled directly by the sketch - it is very flexible and no wrapper around it would be able to preserve it's virtues while being much of a wrapper.

## But first, about the hardware
As there is a good chance you;ve noticed, a lot of hardware is better at driving pins low than high (sinking vs sourcing). Classic AVRs had symmetric drive - they were very similar in their ability to source and sink current. The tinyAVRs were... almost symmetric. The Dx and Ex, are far from, it.

The modern AVR's aren't quite as symmetric. *this may impede techniques like the R-C filter on a pin to generate analog voltages if R is low and C large)* (as long as the current through the pin is low you don't notice). Let's compare the modern AVRs real quick here:

| Part Family      | tiny0/1 | tiny2 | Dx | Ex | tiny reset pin when used for UPDI or GPIO |
|------------------|---------|-------|----|----|-------------------------------------------|
| Abs max current  | +/-40mA |+/-40mA|+/-50|+/-40 | -                                      |
| Abs max injection| +/-15mA |+/-15mA|+/-20|+/-20 | -                                      |
|    .. @ Vdd > 4.9|+/- 1 mA |+/-1 mA|-   |  - | -                                         |
| IOH,max from graph|        | 20 mA |50mA|tbd | No graph, spec is same drop as normal I/O |
| VOH,min from graph|        | 4.3V  |3.5V|tbd | handling 30x the current. Even at 5V, 2k  |
| IOL,max from graph|        | 20 mA |100mA|tbd| output impedance.                         |
| VOH,max from graph|        | 0.43  |1.4V|tbd | -                                         |

You might be scurrying to your datasheets thinking "I swear they were symmetric on the tinys!" and if you look at the table - the worst case specs - they are. They give themselves equal room on both sides. But the graphs show a voltage drop when sourcing 20mA on a tiny2 of avg 0.7V at room temp, but a voltage drop of 0.4-0.45 on at sinking the same amount of current! And what's with 100mA on Dx? abs max is 50! Good question. Personally I appreciate the transparency on behavior in overload conditions.

What tinyAVR calls current injection, Dx/Ex-series call "Clamp current" - it's current through the protection diodes. Oddly It wasn't spec'ed on classic AVRs (but was well known as the usual cause of "blown" pins). Atmel employees are rumored to have said +/- 1mA maximum. Certainly we have made some progress on that front.


## Ballpark overhead figures
The digital I/O functions are astonishingly inefficient. This isn't my fault (not on mTC - on DxC I have definitely not helped...) - it's the Arduino API's fault
These figures are the difference between a sketch containing a single call to the function, with volatile variables used as arguments to prevent compiler from making assumptions about their values, which may substantially reduce the size of the binary otherwise.

The comparison to the fast I/O functions is grossly unfair, because the fast I/O functions have constant arguments - but this gives an idea of the scale.
Remember also that executing 1 word of code (2 bytes) takes 1 clock cycle, though not all of the code paths are traversed for all pins (indeed, we know that large portions will be skipped for all pins, because each pin can have only one PWM timer, but on the other hand, there are a few small loops in there). Gives an idea of the scale.

| Function           | t3224 | t3216 | Notes
|--------------------|-------|-------|-------------------------
| pinMode()          |  +128 |  +146 |
| digitalWrite()     |  +198 |  +340 | Calls turnOffPWM(), included in figures.
| both of above      |  +296 |  +438 | Calls turnOffPWM()
| openDrain()        |  +128 |  +262 | Calls turnOffPWM()
| turnOffPWM()       |   +72 |  +228 | 3216 has the complicated type D timer, so this is huge.
| digitalRead()      |   +76 |   +78 | Official core calls turnOffPWM (we don't)
| pinConfigure() 1   |  +104 |  +116 | Only direction/output level
| pinConfigure() 2   |  +202 |  +220 | Only pullup, input sense, and direction/outlevel
| pinConfigure() 3   |  +232 |  +250 | Unconstrained
| digitalWriteFast() |    +2 |    +2 | With constant arguments.
| pinModeFast()      |   +12 |   +12 | With constant arguments.

The takeaways from this should be that:
* digitalWrite() can easily take 35-75 or more clock cycle with turnOffPWM and then over 50 itself. So digitalWrite can take up to 7 us per call (at 20 MHz; digitalWriteFast takes 0.05 us), and it is by far the worst offender. A small consolation is that turnOffPWM does not execute the entire block of code for all parts. It checks which timer is used, and uses that block, which very roughly gives 70 clocks on a TCA pin, around 150 on a TCD pin, and considerably fewer on a pin with no PWM, though it still has to determine that the pin doesn't have pwm.
* If you know you won't have PWM coming out of a pin, pinConfigure is faster than pinMode() and digitalWrite(), especially if only setting up the more common properties.
* pinConfigure is optimized differently when called with the second argument constrained to 0x00-0x0F (setting only direction and output value) (1), with it constrained to that plus configuring pullups (2), and without restrictions (3).
* 4 bytes, times the number of the highest number digital pin, are in the form of lookup tables.
* This is why the fast digital I/O functions exist, and why there are people who habitually do `VPORTA.OUT |= 0x80;` instead of `digitalWrite(PIN_PA7,HIGH);`
* This unexpectedly poor performance of the I/O functions is also why I am not comfortable automatically switching digital I/O to "fast" type when constant arguments are given. The normal functions are just SO SLOW that such a change would be certain to break code implicitly. I have seen libraries which, for example, would not meet (nor come close to meeting) setup and hold time specifications for an external device, but for the grindingly slow pin I/O. *This breakage would likely be wholly unexpected by both library author and the user, and would furthermore be very difficult for most users to diagnose, requiring an oscilloscope, awareness of the poor performance of these API functions, and sufficient familiarity with electronics to know that this was a problem that could occur*. Hence, it would be directly contrary to the goal of making electronics accessible (which Arduino is directed towards), and the goal of maintaining a high degree of compatibility with existing Arduino libraries, which is one of the primary goals of most Arduino cores.

megaTinyCore and DxCore, like all Arduino cores, provides implementations of the usual digital I/O functions. *These implementations are not guaranteed to have identical argument and return types, though we do guarantee that same will be implicitly convertible to the types used by the reference implementation*


pinConfigure called with second argument constrained to 0x00-0x0F (setting only direction and output value) (1), with it constrained to that plus configuring pullups (2), and without restrictions (3).

About 200 bytes of that space is used for lookup tables, not instructions.

This is why the fast digital I/O functions exist.

This is why the fast digital I/O functions exist, and why there are people who habitually do `VPORTA.OUT |= 0x80;` instead of `digitalWrite(PIN_PA7, HIGH);`

It's also why I am not comfortable automatically switching digital I/O to "fast" type when constant arguments are given. The normal functions are just SO SLOW that you're sure to break code that hadn't realized they were depending on the time it took for digital write, even if just for setup or hold time for the thing it's talking to.

## openDrain()
It has always been possible to get the benefit of an open drain configuration - you set a pin's output value to 0 and toggle it between input and output. This core provides a slightly smoother (also faster) wrapper around this than using pinmode (since pinMode must also concern itself with configuring the pullup, whether it needs to be changed or not, every time - it's actually significantly slower setting things input vs output. The openDrain() function takes a pin and a value - `LOW`, `FLOATING` (or `HIGH`), or `CHANGE`. `openDrain()` always makes sure the output buffer is not set to drive the pin high; often the other end of a pin you're using in open drain mode may be connected to something running from a lower supply voltage, where setting it OUTPUT with the pin set high could damage the other device.

Use pinMode() to set the pin `INPUT_PULLUP` before you start using `openDrain()`, or use `pinConfigure()` if you need the pullup enabled too; this doesn't change it. Note that there is nothing to stop you from doing `pinMode(pin,INPUT_PULLUP); openDrain(pin, LOW);` That is non-destructive (no ratings are exceeded), and would be a good place to start for a bitbanged open drain protocol, should you ever create one - but it doesn't exactly help with power consumption if you leave it like that! If running on batteries, be sure to turn off one of the two, either by using openDrain to release it to to pullups, or digitalWrite'ing it `HIGH`, or any other method of configuring pins that gets you a pin held in a defined state where it isn't working against it's own pullup.

```c++
openDrain(PIN_PA1, LOW); // sets pin OUTPUT, LOW.
openDrain(PIN_PA1, FLOATING); // sets pin INPUT. It will either float, or (if pullup was previously enabled) be pulled up to Vdd.
```

## Fast Digital I/O
This core includes the Fast Digital I/O functions, digitalReadFast(), digitalWriteFast() and openDrainFast(). These are take up less flash than the normal version, and can execute in as little as 1 clock cycle. The catch is that the pin MUST be a known constant at compile time. For the fastest, least flash-hungry results, you should use a compile-time known constant for the pin value as well. Remember than there are three options here, not just two. If you only want two of the options, you will get smaller binaries that run faster by using the ternary operator to explicitly tell the compiler that the value is only ever going to be those two values, and then it can optimize away the third case.

An openDrainFast() is also provided. openDrainFast() also writes the pin LOW prior to setting it output or toggling it with change, in case it was set `INPUT` and `HIGH`. `CHANGE` is slightly slower and takes an extra 2 words of flash because the `VPORT` registers only work their magic when the sbi and cbi (set and clear bit index) instructions will get you what you want. For the output value, writing a 1 to the bit in `VPORT.IN` will toggle it, and that's what we do, but there is no analogous way to change the pin direction, so we have to use a full STS instruction (2 words) to write to the `PORT.DIRTGL` register after spending another word on LDI to load the bit we want to toggle into a working register.

It also includes pinModeFast(). pinModeFast() requires a constant mode argument. INPUT and INPUT_PULLUP as constant arguments are still as large and take as long to execute as digitalWriteFast with either-high-or-low-state - they turn the pullups on and off (use openDrainFast(pin,FLOATING) to execute faster and leave the pullups as is).

```c++
digitalWriteFast(PIN_PD0, val); // This one takes 10 words of flash, and executes in around half 7-9 clocks.
digitalWriteFast(PIN_PD0, val ? HIGH : LOW); // this one takes 6 words of flash and executes in 6 clocks
digitalWriteFast(PIN_PD0, HIGH); // This is fastest of all, 1 word of flash, and 1 clock cycle to execute
VPORTD.OUT |= 1 << 0; // The previous line is syntactic sugar for this. Beyond being more readable
                      // it eliminates the temptation to try to write more than one bit which instead
                      // of an atomic SBI or CBI, is compiled to a read-modify-write cycle of three
                      // instructions which will cause unexpected results should an interrupt fire in the
                      // middle and write the same register.
```

### Flash use of fast digital output functions

| function            | Any value | HIGH/LOW | Constant        |
|---------------------|-----------|----------|-----------------|
| openDrainFast()     | 14 words  | 7 words  | 2 words if LOW<br/>1 if FLOATING |
| digitalWriteFast()  | 10 words  | 6 words  | 1 words         |
| pinModeFast()       |      N/A  |     N/A  | 6 words - too easy to accidentally leave pullups on and drive pin low.  |

Execution time is 1 or sometimes 2 clocks per word that is actually executed (not all of them are in the multiple possibility options. in the case of the "any option" digitalWrite, it's 5-7
Note that the HIGH/LOW numbers include the overhead of a `(val ? HIGH : LOW)` which is required in order to get that result. That is how the numbers were generated - you can use a variable of volatile uint8_t and that will prevent the compiler from assuming anything about it's value. It is worth noting that when both pin and value are constant, this is 2-3 times faster and uses less flash than *even the call to the normal digital IO function*, much less the time it takes for the function itself (which is many times that. The worst offender is the normal digitalWrite(), because it also has to check for PWM functionality and then turn it off if enabled (and the compiler isn't allowed to skip this if you never use PWM).

1 word is 2 bytes; when openDrainFast is not setting the pin FLOATING, it needs an extra word to set the output to low level as well (just in case). This was something I debated, but decided that since it could be connected to a lower voltage part and damage caused if a HIGH was output, ensuring that that wasn't what it would do seemed justified; if CHANGE is an option, it also uses an extra 2 words because instead of a single instruction against VPORT.IN, it needs to load the pin bit mask into a register (1 word) and use STS to write it (2 words) - and it also does the clearing of the output value - hence how we end up with the penalty of 4 for the unrestricted case vs digitalWriteFast.

The size and speed of digitalReadFast is harder to calculate since it also matters what you do with the result. It is far faster than digitalRead() no matter what. The largest impact is seen if you plan to use it as a test in an if/while/etc statement; there are single-clock instructions to skip the next instruction if a specific bit in a "low I/O" register like the ones involved is set or cleared. Thus, it is most efficient to do so directly:
```c
if (digitalReadFast(PIN_PA3)) {
  Serial.println("PA3 is high")
}
```
Getting a 1 or 0 into a variable is slower and uses more flash; I can think of at least one way to do it in 3 which makes no assumption about whether it's stored in an upper or lower register, but I'm not confident in how clever the compiler is, and it's tricky to set something up where the compiler doesn't have options such that you can generalize the result.

**The fast digital I/O functions do not turn off PWM** as that is inevitably slower (far slower) than writing to pins and they would no longer be "fast" digital I/O.

## turnOffPWM(uint8_t pin) is exposed
This used to be a function only used within wiring_digital. It is now exposed to user code - as the name suggests it turns off PWM (only if analogWrite() could have put it there) for the given pin. It's performance is similar to analogWrite (nothing to get excited over), but sometimes an explicit function to turn off PWM and not change anything else is preferable: Recall that digitalWrite calls this (indeed, as shown in the above table, it's one of the main reasons digitalWrite is so damned bloated on the DX-series parts!), and that large code takes a long time to run. DigitalWrite thus does a terrible job of bitbanging. It would not be unreasonable to call this to turn off PWM, but avoid digitalWrite entirely. The pin will return to whatever state it was in before being told to generate PWM, unless digitalWriteFast() or direct writes to the port register have been used since. If the pin is set input,

## Finding current PWM timer, if any
On many cores, there is a `digitalPinToTimer(pin)` macro, mostly for internal use, which returns the timer associated with the pin. On DxCore there is the added complication that we account for the TCA portmux register, and digitalPinToTimer() only returns non-TCA timers. For the PORTMUX-aware test, call `digialPinToTimerNow(pin)` instead.

## pinConfigure()
pinConfigure is a somewhat ugly function to expose all options that can be configured on a per-pin basis. It is called as shown here; you can bitwise-or any number of the constants shown in the table below. All of those pin options will be configured as specified. Pin functions that don't have a corresponding option OR'ed into the second argument will not be changed. There are very few guard rails here: This function will happily enable pin interrupts that you don't have a handler for (but when they fire it'll crash the sketch), or waste power with pins driven low while connected to the pullup and so on.

Thanks to the work of @MCUdude 1.5.0, we can instead separate these constants with commas, like arguments. This is much, much more natural - the comma form should be preferred over the OR-ing form.

The flash efficiency of the two methods should be very similar, (though small differences do exist, depending on the number of times pinConfigure is called and with what arguments. Commas are smaller for most cases, but between 3 and 8 calls, approximately, they're slightly bigger)

```c++
pinConfigure(PIN_PA4,(PIN_DIR_INPUT | PIN_PULLUP_ON | PIN_OUT_LOW));
// Set PIN_PA4 to input, with pullup enabled and output value of LOW without changing whether the pin is inverted or triggers an interrupt.

pinConfigure(PIN_PA2,(PIN_DIR_INPUT | PIN_OUT_LOW | PIN_PULLUP_OFF | PIN_INVERT_OFF | PIN_ISC_ENABLE));
// Set PD2 input, with output register set low, pullup, invert off, and digital input enabled. Ie, the reset condition.
// Equivalent:
pinConfigure(PIN_PA2,(PIN_DIR_INPUT, PIN_OUT_LOW, PIN_PULLUP_OFF, PIN_INVERT_OFF, PIN_ISC_ENABLE));
```
The second syntax is thanks to @MCUdude and his understanding of variadic macros


| Functionality |   Enable  | Disable            | Toggle |
|---------------|-------|---------------------|--------------------|
| Direction, pinMode() | `PIN_DIR_OUTPUT`<br/>`PIN_DIR_OUT`<br/>`PIN_DIRSET` | `PIN_DIR_INPUT`<br/>`PIN_DIR_IN`<br/>`PIN_DIRCLR`       | `PIN_DIR_TOGGLE`<br/>`PIN_DIRTGL` |
| Pin output, `HIGH` or LOW | `PIN_OUT_HIGH`<br/>`PIN_OUTSET`         | `PIN_OUT_LOW`<br/>`PIN_OUTCLR`          | `PIN_OUT_TOGGLE`<br/>`PIN_OUTTGL`       |
| Internal Pullup  | `PIN_PULLUP_ON`<br/>`PIN_PULLUP`        | `PIN_PULLUP_OFF`<br/>`PIN_NOPULLUP`       | `PIN_PULLUP_TGL`       |
| Invert `HIGH` and LOW |`PIN_INVERT_ON`        | `PIN_INVERT_OFF`       | `PIN_INVERT_TGL`       |
| Digital input buffer | `PIN_INPUT_ENABLE`or<br/> `PIN_ISC_ENABLE`    | `PIN_ISC_DISABLE` or<br/>`PIN_INPUT_DISABLE`    | Not supported<br/>No plausible use case |
| Interrupt on change | `PIN_ISC_ENABLE` or<br/> `PIN_INPUT_ENABLE`       | `PIN_ISC_ENABLE` or<br/>oth     | Not applicable |
| Interrupt on Rise  | `PIN_ISC_RISE` or<br/> `PIN_INT_RISE`         | `PIN_ISC_ENABLE` or<br/>`PIN_ISC_DISABLE`     | Not applicable |
| Interrupt on Fall  | `PIN_ISC_FALL` or<br/> `PIN_INT_FALL` | `PIN_ISC_ENABLE` or<br/>`PIN_ISC_DISABLE`      | Not applicable |
| Interrupt on LOW  | `PIN_ISC_LEVEL`  or<br/> `PIN_INT_LEVEL` | `PIN_ISC_ENABLE` or<br/>`PIN_ISC_DISABLE`      | Not applicable |

For every constant with TGL or TOGGLE at the end, we provide the other spelling as well. For every binary option, in addition to the above, there is a `PIN_(option)_SET` and `PIN_(option)_CLR` for turning it on and off. The goal was to make it hard to not get the constants right.

While pinConfigure is not as fast as the fast digital I/O functions above, it's still faster than pinMode().

Again, note that unlike digitalWrite() this does not turn off PWM.

## Slew Rate Limiting (Dx-series and 2-series only)
All of the Dx-series parts have the option to limit the [slew rate](https://en.wikipedia.org/wiki/Slew_rate) for the OUTPUT pins on a on a per-port basis. This is typically done because fast-switching digital pins contain high-frequency components (in this sense, high frequency doesn't necessarily mean that it repeats, only that if it continued, it would be high frequency; this way of thinking is useful in electrical engineering), which can contribute to EMI, as well as ringing (if you're ever looked on a scope and noticed that after a transition, the voltage briefly oscillates around the final voltage - that's ringing) which may confuse downstream devices (not usually, at least in arduino land, though). Often, you will not know exactly *why* it's an issue or what goes wrong, you just see a maximum slew rate spec. If you're productizing something and it has to pass FCC`**` testing sometimes limiting the slew rate can reduce EMI.
```c
// These work but are slow and inefficient.
// Enable slew rate limiting on PORTA
PORTA.PORTCTRL |= PORT_SRL_bm;
// Disable slew rate limiting on PORTA
PORTA.PORTCTRL &= ~PORT_SRL_bm;
```
However, because there is only one bit in this register, you don't actually need to use the |= and &= operators, which do a read-modify-write operation, taking 10 bytes and 6 clocks each. Instead, you can use simple assignment. This saves 4 bytes (3 clocks) when enabling, and 6 bytes (4 clocks) on disabling. (going from `LDS, ORI/ANDI, STS` to `LDI, STS` and just `STS` if you're clearing it, since avr-gcc always keeps a "known zero" in r1). Of course, if a future part with more options set by the PORTCTRL register comes out, porting to that part would require a code change if you also wanted to use one of those new features. I suggest a reminder that it's a shortcut if you take it, like the comments below. **It is recommended that libraries not take this shortcut - because you may not still be maintaining the library, and users across the world will have downloaded it and most will not be able to figure out why your library breaks their other code**.

```c
// Enable slew rate limiting on PORTA more efficiently
PORTA.PORTCTRL = PORT_SRL_bm; // simple assignment is okay because on DA, DB, and DD-series parts, no other bits of PORTCTRL are used, saves 2 words and 3 clocks
// Disable slew rate limiting on PORTA
PORTA.PORTCTRL = 0;           // simple assignment is okay because on DA, DB, and DD-series parts, no other bits of PORTCTRL are used, saves 3 words and 4 clocks
```

These parts (like many electronics) are better at driving pins low than high. With slew rate limiting enabled, the pins take about twice as long to transition. (all values are from the typ. column - neither min nor max are specified; one gets the impression that this is not tightly controlled):

| Direction   | Normal | Slew Rate Limited |
|-------------|--------|-------------------|
| Rising      |  22 ns |             45 ns |
| Falling     |  16 ns |             30 ns |

With a 24 MHz system clock, that means "normal" would be just over half a clock cycle while rising, and just under half a clock cycle falling; with slew rate limiting, it would be just over a clock cycle rising, and 3/4ths of a clock cycle on the falling side.

## There is no INLVL or PINCONFIG on tinyAVR devices
The configurable input level option (`INLVL`) is only available on parts with MVIO (AVR DB and DD-series).

Multi-pin configuration registers, which allow setting PINnCTRL en masse are only available on AVR Dx-series.

### Basic pin information
```c++
uint8_t digitalPinToPort(uint8_t pin);
uint8_t digitalPinToBitPosition(uint8_t pin);
uint8_t digitalPinToBitMask(uint8_t pin);
uint8_t analogPinToBitPosition(uint8_t pin);
uint8_t analogPinToBitMask(uint8_t pin);
```
These take a pin number (which can be represented as a uint8_t - though it is unfortunately common practice to use 16-bit signed integers to store pin numbers in Arduino-land). If the pin passed is valid, these will return either the bit position (PIN_PA0 would return 0, PIN_PD4 will return 4 and so on), the bit mask (bit mask = 1 << (bit position), so PIN_PA0 would return 0x01, PIN_PD4 would return 0x10, etc) `*`, or the port number (PORTA = 0, PORTB = 1, etc).

## turnOffPWM(uint8_t pin) is exposed
This used to be a function only used within wiring_digital. It is now exposed to user code - as the name suggests it turns off PWM (only if analogWrite() could have put it there) for the given pin. It's performance is similar to analogWrite (nothing to get excited over), but sometimes an explicit function to turn off PWM and not change anything else is preferable: Recall that digitalWrite calls this (indeed, as shown in the above table, it's one of the main reasons digitalWrite is so damned bloated on the DX-series parts!), and that large code takes a long time to run. DigitalWrite thus does a terrible job of bitbanging. It would not be unreasonable to call this to turn off PWM, but avoid digitalWrite entirely. The pin will return to whatever state it was in before being told to generate PWM, unless digitalWriteFast() or direct writes to the port register have been used since. If the pin is set input after PWM is enabled with analogWrite (or through other means - though in such cases, the behavior when turnOffPWM() is called on any pin which nominally outputs PWM controlled by the same timer *should be treated as undefined* - it is not possible to efficiently implement such a feature on AVRs nor most other microcontrollers while maintaining behavior consistent with the API - the core would (at least on an AVR) have to iterate through every possible source of PWM, testing each in turn to see if it might have PWM output enabled on this pin. In many cases, including this core, each timer would require checking a separate bit in one or more special function registers, as well as storing a database that associates each pin with one or more combinations of bits and the corresponding bit to clear to disable PWM through that pathway. Suffice to say, that is not practical within the resource constraints of embedded systems.

## Finding current PWM timer, if any: digitalPinToTimerNow() (DxCore only)
On many cores, there is a `digitalPinToTimer(pin)` macro, mostly for internal use, which returns the timer associated with the pin. The Arduino API implicitly assumes that this is constant, and both core libraries and code in the wild often assume that it is. The digitalPinToTimer() implementation is as a macro, which frequently is optimized to a constant (provided the argument is constant, of course). The assumption that there exists not-more-than-1-to-1 timer:pin mapping was never universally valid even among AVRs (there are examples of classic AVRs that have multiple timers usable on some pins, and some even permit that to be remapped easily at runtime (the classic ATtiny841 being a notable example). All modern AVRs have at least two pins available to each timer output compare channel; and while the tinyAVRs never have more than two timers potentially usable with a given pin, the Dx-series allows one of up to 7 pins to be associated with a given timer (in groups), with some pins having as many as 4 options.

For the modern tinyAVRs, during development of megaTinyCore the design decision was made to not support PWM output via analogWrite() for any timer *type* if that *type* would add not more than 1 additional PWM pin when the type A timer was used in split mode (which has up to 6 channels) in order to make efficient use of limited flash. This leaves out the type D timer on 8-pin and 14-pin 1-series parts, and the type B timer(s) on all parts. Either of these would nearly double the flash required for PWM - however, as of recent versions you can choose between a numbert of PWM pin layounts. In any event, on megaTinyCore, `digitalPinToTimerNow()` is simply an alias of `digitalPinToTimer()` and this will remain the case, nor are there plans to add support for TCB PWM.

On the Dx-series parts, timers, pins, multiplexing options, and clock cycles are all more abundant, as are other peripherals competing for any given pin meanwhile, counterintuitively, the multiplexing scheme is actually simpler, and that inflexibility was not appropriate. As of 1.3.x, `analogWrite()` works for either type A timer **even if you change where that timer is pointed** *provided* that the appropriate `PORTMUX` register is set first. As of 1.5.x this is extended to the type D timer where hardware allows (at time of writing, only the AVR DD-series; there is a silicon bug impacting all production DA/DB-series parts. It is expected that future silicon revisions will correct this on those parts, if those are ever made available; having waited more than 2 years hoping for such fixes, and 5 years for other corrections for the tinyAVRs, I am not optimistic). On DxCore there is both `digitalPinToTimer()` and `digitalPinToTimerNow()`. The former never returns TIMERAn, but will return a constant referring to a type D timer *channel* or a type B *timer*. The latter tests the relevant `PORTMUX` register if that pin can use a type A timer; If it cannot, it then checks the standard `digitalPinToTimer()`. If that indicates that a type D timer channel can be used with the pin, it then performs a bitwise AND with the type D timer mux register, which will be true if the TCD is pointed at that pin. If that gives us a type B timer, that timer is returned as the result (notice that pins that can use either the type D timer or a type B timer will never return the type B timer, even if the type D timer is pointed elsewhere - this was a necessary compromise for flash and code complexity considerations.

**In no case will either of these macros ever return a timer which the core does not permit use of analogWrite() with.**

Furthermore, the following situations should not be expected to produce the desired result:
* Calling turnOffPWM, digitalPinToTimer, digitalPinToTimerNow or analogWrite on a pin nominally driven by a timer which the core has been instructed not to reconfigure using the takeOverTCxn() function. The core will treat the pin as an ordinary digital pin - by calling that, you assume full responsibility for all management of that timer. That function must be called if any manual configuration of a PWM timer will be or has been performed, except as specifically noted in in the timer and PWM reference or type D timer reference.
* Calling turnOffPWM, digitalPinToTimer, digitalPinToTimerNow, analogWrite or digitalWrite on a pin nominally driven by a type A timer which has been manually reconfigured, particularly if it has been configured in*single mode* for 3x16-bit PWM channels. The Arduino API does not support PWM with > 8 bits of resolution. Users who require that level of control must call takeOverTCxn(). See the timer and PWM reference.
* Calling turnOffPWM, digitalPinToTimer, digitalPinToTimerNow, analogWrite or digitalWrite on a pin nominally driven by a type B timer, but which has been configured to use an output pin other than the one shown on the DxCore pin mapping for that part - this will reconfigure PWM on the wrong pin if at all.
* Manually configuring any timer without calling takeOverTCxn, except as explicitly documented in the core documentation (be sure you are reading the correct core's documentation - usually mTC = DxC, but this is one case where that assumption is *not* valid. DxC permits more manipulation of TCD0 in particular.

## Note on number of pins and future parts
The most any announced AVR has had is 86 digital pins, the ATmega2560; In the modern AVR era, the digital pin maximum is 55, and the maximum possible without rearchitecting how pins are accessed is 56 (ie, if UPDI could be turned into I/O on DA/DB we are already there). There is no way for an AVR that allows the same bit-level access that we enjoy, on all pins, to have more than 56 pins within the AVR instructionset. Each port takes up 4 of the 32 addresses in the low I/O space for the VPORT registers, on which the single cycle bit-level access relies. Only time will tell how this is handled.
* **No VPORTH or higher** - This is clean, simple, and limiting in some ways, but probably the best route. If you needed to use VPORT access, you'll just need to use the lower 7 ports for it. Surely you don't need 57+ pins all with VPORT atomic single cycle access! This is also, in my judgement, most likely based on the fact that on the ATmega2560, the only precedent we have for running out of low I/O registers for pins, they didn't even put ports H through L into the high I/O space (heck, not only that, they did not, as far as I can see, so much as mention the pin inequality in the datasheet!).
* **VPORTs in High I/O** - There are many ways of dealing with it using a new set of differently behaving registers in the high I/O space. None of them retain the arbitrary compile-time-known-bit-write-is-one-clock of current VPORTs. At 8 registers per port, you could make a series of N operations on a bit or number of bits take N+1 clocks, and keep them atomic. This would mean a VPORT.DIR/DIRCLR/DIRSET, and VPORT.OUT/OUTSET/OUTCLR and VPORT.IN(writing to IN toggles, and there was never a single cycle DIR toggle). and VPORT.INTFLAGS. That's viable, though with the other registers in that, you could get at most only 3 more ports - which is only 24 pins: You couldn't quite do it for a 100 pin chip). Other solutions would lower capability and comprehensibility in exchange for squeezing in more pins. No matter how you do it, it doesn't really solve the problem.
* **VPORTs that can be remapped at runtime** - The stuff of library author's nightmares. You'd have some subset of the VPORTs (or maybe all of them) that could be remapped. If you put the register for that in the high I/O space it could be written quickly. But this will cause unaware old code to break by writing to the wrong pins if it didn't know to check for it, and any code that used them (ie, code that is supposed to be fast) might have to check the mapping first. This is most like what was done on the XMegas, but also serves as a shining example of the over-complexity that I think doomed those parts to poor uptake and slow death. The best of these bad approaches would probably be to have VPORTG changed into say VPORTX. Could configure it with a register in the high I/O space so it was 2 cycle overhead to write (1 for LDI port number, 1 to OUT to register). That way all code for other ports wouldn't worry, and port G is present on the fewest parts so the least code would be impacted if ported, and would be straightforward to document

`*` - Note on conventions for specifying numbers: 0x## refers to a hexadecimal number, while ## refers to a decimal digit and 0b######## refers to a value given as binary. For hexadecimal values, if the size of the datatype is unambiguosly known, we will typically represent them with an appropriate number of leading 0's - so 1 in a 1-byte datatype is written as 0x01, while 1 in a 16-bit datatype is written as 0x0001. Any time a number is not prefixed by 0x or 0b, the decimal form should be assumed. Which representation of a given value is chosen is based on the context of that value. Values that simply represent numbers are generally given in decimal. Values that are being subjected to bitwise operators, or that are bit masks, group codes, and similar, will be shown in hexadecimal.


`**` As far as the FCC testing goes, the net effect (a $5k+ barrier to entry applied only to domestic manufacturers) means that FCC might as well stand for Fail to Compete with China - Do you think any of the electronics we get from China passed FCC testing?! A good portion of them don't look to have been tested for basic functionality, let alone compliance. Since we;ve had a couple of decades of near unregulated electronic crap being imported, and there aren't computers failing, data infrastructure exploding, and airplanes crashing because of lost navigation (batteries catching fire? Well, I don't think any crashes? But definitely some emergecny landings)... maybe we don't need such strict rules (on radio emissions - and we could do wih?

All of the cases I'm aware of where interference actually caused concern were from devices intentionally designed do disrupt other devices (usually GPS jammers being used by truck drivers to cheat someone or another. whether it's the delivery driver who jams their GPS, parks next to a ravine, and chucks packages into it, scanning them as delivered, or the long haul trucker dodging the limits on driving time and so he can drive for 3 days straight, enacted after incidents where drivers struck other vehicles
