# Improved Digital I/O Functionality
This core includes a number of features to provide more control or performance when doing digital I/O. This page describes how to use them. All of the options that can be configured for a pin are exposed. The only things that aren't exposed are the slew rate limiting feature, and the multi-pin configuration facilities. The slew rate limiting is can only be configured on a per port basis; turning it on and off is so simple (see below) that it needs no wrapper. The multi-pin configuration system does not have an obvious "right way" to expose it and should be handled directly by the sketch - it is very flexible and no wrapper around it would be able to preserve it's virtues while being much of a wrapper.

## Table of Contents


## Features

* We supply the classics, which work as you know them to:
  * `pinMode(uint8_t pin, uint8_t mode)` - mode should be INPUT, OUTPUT, or INPUT_PULLUP. *All other values are invalid* and have *undefined behavior*
  * `digitalWrite(uint8_t pin, uint8_t state)`- state should be `LOW`, `HIGH`, or `CHANGE`. *All other values are invalid* and have *undefined behavior*  `LOW`, `HIGH` and `CHANGE` have numeric values of 0, 1, and 2, though using the numbers is deprecated (but means that it does work when passed booleans, which are promoted to 8-bit values of 0 or 1, are passed)
  * `digitalRead(uint8_t pin)` - returns a uint8_t, not an int. **Does not turn off PWM when used on a pin currently outputting PWM, unlike the default core.** There are limits to how far I will toe the line, and this is beyond them, how often do you want to read a pin immediately after PWMing on it?
  * `analogWrite(uint8_t pin, uint8_t duty)` - *That's not digital* "Well I can't very well talk about the ovewrhead without bringing up PWM, as you'll see. Besides, PWM is digital"
* There has always been an internal function by this name, which does exactly what it sounds like. For reasons unclear to me, it was hidden. It no lomger is. Since it dominates the execution time of any function that calls it, it can't be ignored.
  * `turnOffPWM(uint8_t pin)` - turns off PWM on the pin passed to it, if it's a PWM pin. Otherwise does nothing but burn a lot of clock cycles, especially on Dx-series. Mostly used internally, and you just digitalWrite() the pin for the same effect.
* And we have added a bunch of new API extensions...
  * `openDrain(uint8_t pin, uint8_t state)` - State = `LOW` or `FLOATING` (HIGH will work, but is not recommended). Leaves the port output value at 0 (and sets it to that if it isn't there already), and just changes the direction.
  * `digitalWriteFast(uint8_t pin, uint8_t state)` - arguments must be compile-time known foldable constants (compile errors saying as much if you try), but if they are, these are extremely fast and lightweight. And let's face it, this covers a lot of real situations.
  * `digitalReadFast(uint8_t pin)` - As above; in a construction like `if(digitalReadFast(PINNUMBER)){...}` may use only a single clock cycle on the test.
  * `pinModeFast(uint8_t pin, uint8_t mode)` - as above. Performance and overhead suffer significantly if mode not constant.
  * `openDrainFast(uint8_t pin, uint8_t state)` - as above.
  * `pinConfigure(uint8_t pin, uint16_t pin_config)` - wrapper around all things pin maniupulation related, allowing direction and output value to be set along with all the options in the PINnCFG register.
* Some policies apply to all of these functions
  * Things like `digitalWrite()` or `pinMode()` that get an argument where only a small number of options are valid, if you pass a compile time known constant that isn't valid, you'll get a compile error telling you as much (ie, (`digitalWrite(HIGH,5)` would resolve `HIGH` to 1, which is probably a valid pin, but the 5 passed as the state, is not `LOW`, `HIGH` or `CHANGE`, so that gives an error, which would hopefully lead you to correctly order the arguments.)
  * All things that take a pin number as an argument, if that argument is compile time known constant, and is invalid (eg, pin doesn't exist), you'll be told as much through a compile error
    * This is consistent with my philosophy that "When there is no way this line could ever not be a bug, and we can see that, it shouldn't compile" - such as telling a pin to enter a mode that doesn't exist, or writing to a pin that doesn't exist.
    * The exception to the non-existent pin is that `NOT_A_PIN` is allowed to fail silently, rather than giving a compile error as some libraries use that as a placeholder for "this pin function isn't used"... and then just rely on invalid pin numbers not doing anything when written to.
  * There are many ways to outfox these "badArg" errors, since they only work on foldable compile-time-known constants. But they help to quickly catch the stupid bugs - bad tab completions, constant pin assignments copy/pasted from code for other modern AVRs, etc
* We discourage referrign to pins by numbers. `PIN_PA1`, `PIN_PB3` is preferred. If they're not defined at compile time, you're compiling for the wrong part or are mistaken about the part's available pins and are trying to write pins you don't have. .
  * It's also a great help when porting between modern AVRs:
    * All tinyAVRs with a copy of a given peripheral (excepting the ADC, because they have a very very different ADC on the 2-series) have the same pins with the same functions. The 2-series only took away one pin mapping option, the ability to remap the TWI pins found on some 1-series parts.
    * All non-tiny modern AVRs with a copy of a given peripheral, if they have it's pins (as used on previous modern AVRs that were not tinyAVRs), those pins will be available for that peripheral.
      * New parts may gain additional pin mappings, but they have never lost a pin mapping.
      * The sole exception to this, to my knowledge, is the SPI and TWI mappings for the megaAVR 0-series, the immediate predecessor to the Dx-series.
        * 0-series megaAVR had one TWI and one SPI, respectively on pins 2/3 of PA or PC, with PC or PF as the dual mode pins and PA4-7, PC0-3, PE0-3.
        * Dx-series TWI0 is on: pins 2/3 of PA or PC, dual mode on PC2-3 or PC6-7. and TWI1 is on: pins 2/3 of PF or PB, dual mode on PB2-3 or PB6-7. SPI0 is on: PC0-3, PC4-7, PB4-7 and SPI1 is on: PA4-7, PE0-3, PG4-7.
        * Notice that all pins that had *a* TWI or SPI on them still do. it just may not be the same one.
        * Above lists do not include the later mappings that were added with the DD-series to make low pincounts less painful.
    * All this means that startlingly low level code can often be used with no code changes, which is how DxCore and mTC support a combined 142 (38 tinyAVR, rest Dx/Ex) chips and share most code. ATTinyCore supported under 25 parts, and is full of "one-off" implementations for parts idiosyncrasies. This dark days are OVER!

## openDrain()
It has always been possible to get the benefit of an open drain configuration - you set a pin's output value to 0 and toggle it between input and output. This core provides a slightly smoother (also faster) wrapper around this than using pinmode (since pinMode must also concern itself with configuring the pullup, whether it needs to be changed or not, every time - it's actually significantly slower setting things input vs output. The `openDrain()` function takes a pin and a value - `LOW`, `FLOATING` (or `HIGH`) or `CHANGE`. ``openDrain()`` always makes sure the output buffer is not set to drive the pin high; often the other end of a pin you're using in open drain mode may be connected to something running from a lower supply voltage, where setting it `OUTPUT` with the pin set high could damage the other device.

Use `pinMode()` to set the pin `INPUT_PULLUP` before you start using `openDrain()`, or use `pinConfigure()` if you need the pullup enabled too; this doesn't change it. Note that there is nothing to stop you from doing `pinMode(pin,INPUT_PULLUP); openDrain(pin, LOW);` That is non-destructive (no ratings are exceeded), and would be a good place to start for a bitbanged open drain protocol, should you ever create one - but it doesn't exactly help with power consumption if you leave it like that! If running on batteries, be sure to turn off one of the two, either by using openDrain to release it to to pullups, or digitalWrite'ing it `HIGH`, or any other method of configuring pins that gets you a pin held in a defined state where it isn't working against it's own pullup.

```c++
openDrain(PIN_PA1, LOW); // sets pin OUTPUT, LOW.
openDrain(PIN_PA1, FLOATING); // sets pin INPUT. It will either float, or (if pullup was previously enabled) be pulled up to Vdd.
```

## Fast Digital I/O
This core includes the Fast Digital I/O functions, `digitalReadFast()`, `digitalWriteFast()` and `openDrainFast()`. These are take up less flash than the normal version, and can execute in as little as 1 clock cycle. The catch is that the pin MUST be a known constant at compile time. For the fastest, least flash-hungry results, you should use a compile-time known constant for the pin value as well. Remember than there are three options here, not just two. If you only want two of the options, you will get smaller binaries that run faster by using the ternary operator to explicitly tell the compiler that the value is only ever going to be those two values, and then it can optimize away the third case.

An `openDrainFast()` is also provided. `openDrainFast()` also writes the pin `LOW` prior to setting it output or toggling it with change, in case it was set `INPUT` and `HIGH`. `CHANGE` is slightly slower and takes an extra 2 words of flash because the `VPORT` registers only work their magic when the sbi and cbi (set and clear bit index) instructions will get you what you want. For the output value, writing a 1 to the bit in `VPORT.IN` will toggle it, and that's what we do, but there is no analogous way to change the pin direction, so we have to use a full STS instruction (2 words) to write to the `PORT.DIRTGL` register after spending another word on LDI to load the bit we want to toggle into a working register.

It also includes `pinModeFast()`. `pinModeFast()` requires a constant mode argument. INPUT and INPUT_PULLUP as non-constant arguments are still as large and take as long to execute as digitalWriteFast with either-high-or-low-state - they turn the pullups on and off (use `openDrainFast(pin,FLOATING)` to execute faster and leave the pullups as is).

```c++
digitalWriteFast(PIN_PA3, val); // This one takes 10 words of flash, and executes in around 7-9 clocks (assuming the compiler can't deduce val).
digitalWriteFast(PIN_PA3, val ? HIGH : LOW); // this one takes 6 words of flash and and runs in 6 clocks (you've given it extra information and relaxed it's assignment: you've told it that all it needs to be capable of handling is a HIGH or LOW, not CHANGE.
                                             //
digitalWriteFast(PIN_PA3, HIGH); // This is fastest of all, 1 word of flash, and 1 clock cycle to execute.

VPORTA.OUT |= 1 << 3; // The previous line is syntactic sugar for this. Beyond being more readable
                      // it eliminates the temptation to try to write more than one bit which instead
                      // of an atomic SBI or CBI, is compiled to a read-modify-write cycle of three
                      // instructions which will cause unexpected results should an interrupt fire in the
                      // middle and write the same register.
```

### Flash use and speed of (fast) digital output functions
All of the "fast" digital I/O functions are set ((always_inline)). This results in each "call" having the size listed below. In fact, no call or subroutine is involved - always_inline tells it to replace the call with the body of the function, even when it's normal optimization (-Os) would favor putting it in a call

| function            | Any value | HIGH/LOW | Constant Val    |
|---------------------|-----------|----------|-----------------|
| openDrainFast()     | 14 words  | 7 words  | 2 words if LOW<br/>1 if FLOATING |
| digitalWriteFast()  | 10 words  | 6 words  | 1 words         |
| pinModeFast()       |      N/A  |     N/A  | 6 words         |



Execution time is 1 or sometimes 2 clocks per word that is actually executed (not all of them are in the multiple possibility options. in the case of the "any option" digitalWrite, it's 5-7
Note that the HIGH/LOW numbers include the overhead of a `(val ? HIGH : LOW)` which is required in order to get that result. That is how the numbers were generated - you can use a variable of volatile uint8_t and that will prevent the compiler from assuming anything about it's value. It is worth noting that when both pin and value are constant, this is 2-3 times faster and uses less flash than *even the call to the normal digital IO function*, much less the time it takes for the function itself (which is many times that. The worst offender is the normal `digitalWrite()`, because it also has to check for PWM functionality and then turn it off if enabled (and the compiler isn't allowed to skip this if you never use PWM).

1 word is 2 bytes; when openDrainFast is not setting the pin `FLOATING`, it needs an extra word to set the output to low level as well (just in case). This was something I debated, but decided that since it could be connected to a lower voltage part and damage caused if a `HIGH` was output, ensuring that that wasn't what it would do seemed justified; if `CHANGE` is an option, it also uses an extra 2 words because instead of a single instruction against VPORT.IN, it needs to load the pin bit mask into a register (1 word) and use STS to write it (2 words) - and it also does the clearing of the output value - hence how we end up with the penalty of 4 for the unrestricted case vs digitalWriteFast.

The size and speed of digitalReadFast is harder to calculate since it also matters what you do with the result. It is far faster than `digitalRead()` no matter what. The largest impact is seen if you plan to use it as a test in an if/while/etc statement; there are single-clock instructions to skip the next instruction if a specific bit in a "low I/O" register like the ones involved is set or cleared (in other words, there is an opcode :
```c
if (digitalReadFast(PIN_PA3)) {
  Serial.println("PA3 is high")
}
```

This is a sound way to poll for a pin changing. The loop runs in 3 cycles if it's a pin test like this, so you'd be 0-2 clock cycles behind the event. Polling's great, as long as you don't need to do more than one thing at a time. , under 0.15 us is hard to complain about
```c
while(digitalReadFast(PIN_PA3)) {
  ; //busywait for pin to go low
}
```

This is markedly less sound:
```c++
while(digitalRead(pin)) {
  ; //busywait. Instead of 0-2 cycles, it could spend a hundred clocks per read just turning off the PWM repeatedly. Now you're only starting within 5 us of the event, instead of 0.15us.
}
```

**The fast digital I/O functions do not turn off PWM** as that is inevitably slower (far slower) than writing to pins and they would no longer be "fast" digital I/O.

## turnOffPWM(uint8_t pin) is exposed
This used to be a function only used within wiring_digital. It is now exposed to user code - as the name suggests it turns off PWM (only if `analogWrite()` could have put it there) for the given pin. It's performance is similar to analogWrite (nothing to get excited over), but sometimes an explicit function to turn off PWM and not change anything else is preferable: Recall that digitalWrite calls this (indeed, as shown in the above table, it's one of the main reasons digitalWrite is so damned bloated on the DX-series parts!), and that large code takes a long time to run. DigitalWrite thus does a terrible job of bitbanging. It would not be unreasonable to call this to turn off PWM, but avoid digitalWrite entirely. The pin will return to whatever state it was in before being told to generate PWM, unless `digitalWriteFast()` or direct writes to the port register have been used since.

## Finding current PWM timer, if any
On many cores, there is a ``digitalPinToTimer(pin)`` macro, mostly for internal use, which returns the timer associated with the pin. On DxCore there is the added complication that we account for the TCA, TCD, and TCE portmux register, and `digitalPinToTimer()` only returns non-TCA timers. For the PORTMUX-aware test, call ``digialPinToTimerNow(pin)`` instead.

## pinConfigure()
pinConfigure is a somewhat ugly function to expose all options that can be configured on a per-pin basis. It is called as shown here; you can bitwise-or any number of the constants shown in the table below. All of those pin options will be configured as specified. Pin functions that don't have a corresponding option OR'ed into the second argument will not be changed. There are very few guard rails here: This function will happily enable pin interrupts that you don't have a handler for (but when they fire it'll crash the sketch), ~or waste power with pins driven low while connected to the pullup and so on.~ Supposedly they disable the pullup when driving low!

Thanks to the work of @MCUdude 1.5.0, we can instead separate these constants with commas, like arguments. This is much, much more natural - the comma form should be preferred over the OR-ing form.

The flash efficiency of the two methods should be very similar, (though small differences do exist, depending on the number of times pinConfigure is called and with what arguments. Commas are smaller for most cases, but between 3 and 8 calls, approximately, they're slightly bigger)

```c++
pinConfigure(PIN_PA4,PIN_DIR_INPUT,PIN_PULLUP_ON,PIN_OUT_LOW);
// Set PIN_PA4 to input, with pullup enabled and output value of LOW without changing whether the pin is inverted or triggers an interrupt.

pinConfigure(PIN_PA4, PIN_DIR_INPUT, PIN_OUT_LOW, PIN_PULLUP_OFF, PIN_INVERT_OFF, PIN_ISC_ENABLE);
// Set PA4 input, with output register set low, pullup turned off, invert turned off, and digital input enabled. Ie, the reset condition.

// Equivalent:
pinConfigure(PIN_PA4,(PIN_DIR_INPUT | PIN_OUT_LOW | PIN_PULLUP_OFF | PIN_INVERT_OFF | PIN_ISC_ENABLE));
```
The second syntax is thanks to @MCUdude and his understanding of variadic macros


| Functionality |   Enable    | Disable            | Toggle | Notes
|---------------|-------------|---------------------|--------------------|
| Direction, pinMode()        | `PIN_DIR_OUTPUT`<br/>`PIN_DIR_OUT`<br/>`PIN_DIRSET` | `PIN_DIR_INPUT`<br/>`PIN_DIR_IN`<br/>`PIN_DIRCLR`       | `PIN_DIR_TOGGLE`<br/>`PIN_DIRTGL` |
| Pin output, `HIGH` or `LOW` | `PIN_OUT_HIGH`<br/>`PIN_OUTSET`           | `PIN_OUT_LOW`<br/>`PIN_OUTCLR`                 | `PIN_OUT_TOGGLE`<br/>`PIN_OUTTGL`       |
| Internal Pullup             | `PIN_PULLUP_ON`<br/>`PIN_PULLUP`          | `PIN_PULLUP_OFF`<br/>`PIN_NOPULLUP`            | `PIN_PULLUP_TGL`       |
| Invert `HIGH` and `LOW`     | `PIN_INVERT_ON`                           | `PIN_INVERT_OFF`                               | `PIN_INVERT_TGL`       |
| Use TTL levels (where avail)| `PIN_INLVL_TTL`<br/>`PIN_INLVL_ON`        | `PIN_INLVL_SCHMITT`<br/>`PIN_INLVL_OFF`        | Not supported<br/>No plausible use case      |
| Enable Digital Input        | `PIN_INPUT_ENABLE`or<br/>`PIN_ISC_ENABLE` | Any of the other ISC/INT options               | Not applicable | *See note below*
| Disable Digital Input       | `PIN_ISC_DISABLE` or<br/>`PIN_INPUT_DISABLE`| Any of the other ISC/INT options             | Not applicable | *See note below*
| Interrupt on change         | `PIN_ISC_CHANGE` or<br/>`PIN_INT_CHANGE`  | Any of the other ISC/INT options               | Not applicable | *See note below*
| Interrupt on Rise           | `PIN_ISC_RISE` or<br/> `PIN_INT_RISE`     | Any of the other ISC/INT options               | Not applicable | *See note below*
| Interrupt on Fall           | `PIN_ISC_FALL` or<br/> `PIN_INT_FALL`     | Any of the other ISC/INT options               | Not applicable | *See note below*
| Interrupt on `LOW`          | `PIN_ISC_LEVEL`  or<br/> `PIN_INT_LEVEL`  | Any of the other ISC/INT options               | Not applicable | *See note below*

For every constant with TGL or TOGGLE at the end, we provide the other spelling as well. For every binary option, in addition to the above, there is a `PIN_(option)_SET` and `PIN_(option)_CLR` for turning it on and off. The goal was to make it hard to not get the constants right.

While pinConfigure is not as fast as the fast digital I/O functions above, it's still faster than `pinMode()`.

*Note that you must have defined an interrupt iof you enable and interrupt like this (which is perfectly fine). Remember that you must clear the intflags at the end of the interrupt!*


### Regarding the digital input buffer and interrupt options
Each pin can be in one of five modes: Enabled without interrupts, disabled without interrupts (saves power when connected to external voltages between Vdd and Gnd)



### INLVL - input logic levels (AVR DB, DU, EA, EB, DU only)
More recent parts (The AVR-DB, DD, DU, EA and EB) allow you to configure on a per-pin basis one of two sets of input voltage levels: either the normal schmitt triggert input, which has thresholds as a fraction of Vdd, or the TTL mode which has fixed thresholds independent of operating voltage. On MVIO pins, using the schmitt trigger Vddio2 takes the place of VDDIO. Note that this is overridden when pins are used for the TWI - that defaults to I2C voltage levels, but supports an optional SMBus 3.0 mode.

| Voltage             |  Schmitt  |     I2C   |  TTL   | SMBus 3.0 |
|---------------------|-----------|-----------|--------|-----------|
| Low input (Max)     | 0.2 x Vdd | 0.3 x Vdd | 0.80 V |    0.80 V |
| High Input (Min)    | 0.8 x Vdd | 0.7 x Vdd | 1.60 V |    1.35 V |

These are the maximum voltage guaranteed to qualify as `LOW` and the minimum guaranteed to qualify as `HIGH`. Although the I2C levels are described as being different, it is worth noting that 1) if the schmitt trigger input on a recognized a 0.3 x Vdd voltage as `LOW`, it would surely do the same for a 0.2 x Vdd one, likewise at the high end. And typical characteristics graphs, when available, for modern AVRs with the same thresholds and I2C levels, show typical I/O pin thresholds as easily meeting the specs given for the I2C levels. The two options for I2C levels are not new for the DB either - they are widespread. What appears to be new is a less tightly specified but conceptually similar input circuit on all pins, selectable by a bit in PINnCTRL, instead of just I2C pins. It's important to keep in mind how this divides up possible input voltages (note: this is just based on room temp typical characteristics)
* With Schmitt trigger inputs, highest and lowest 1/5th of the voltage-space each go to a definite value, while the voltages between allow the hysteresis that the schmitt triggers are known for. The graphs in the DA-series datasheet (DB typical charachteristics are not yet available) show that in practice, the hysteresis occurs over a much narrower voltage range than the same graph for the 4809 (which depicted three regions of similar size).
  * Typical falling threshold ranged from 0.8 up to 2.05, and rising thresholds from 1.05 V to 2.75 V. Thus, the hysteresis region ranged in size from 0.2 to 0.7 volts, but reached a nadir just around 2-2.25 V where it could be as small as 0.15 V, since the lower threshold increases faster than the upper one at the bottom of the range.
  * When Vdd = 5.0 V - 3.3 V is not guaranteed to be `HIGH`, though the typical characteristic graphs suggest that it will be, as has always been the case on AVRs.
* With TTL input levels, behavior relative to the operating voltage varies greatly.
  * With Vdd = 1.8 V (the minimum), below about half the operating voltage, it's going to read low, while it may need to be very close to the power rail to read `HIGH`.
  * With Vdd = 5.0 V (maximum typically used intentionally) anything over about 1/3rd if the supply voltage is guaranteed to be `HIGH`, and you need to get down to 1/6th of the supply voltage for a guaranteed `LOW`.
  * The actual lower threshold is likely to be higher, and the upper threshold lower. For example, on DA-series SMBus pins, those voltages are 1.09 V and 1.24 V over most of the operating voltage range. This leave you with very little hysteresis, if that happens to be problematic.
  * If interacting with an output that for whatever reason struggles to pull the pin as close to ground as an ideal output should, or if you're counting on the transition being somewhere near the middle of the range, definitely don't use this option.

Particularly in the context of these MVIO-equipped parts, however, the TTL input level option has an obvious major advantage: It makes communication with parts that run at lower voltages easier, and reduces competition for the limited number of MVIO pins. If we allow the low part to control it push-pull (aka totam-pole and other names), and treat it as an `INPUT`, we will always read what they intend. The fact that you can switch a pin between input and output, while pulling it up externally was noted above, and the voltage it was pulled up to could be the lower operating vboltage. That can be done on any AVR. But with INLVL, this scheme is viable for a bidirectional line: the The line between devices could either be pulled up to the lowest operating voltage externally, with the DB or DD series part manipulating the pin as an open drain output (see above). One need not stop with just two devices - multiple DB/DD-series parts with `INVLV = TTL` set could communicate with multiple devices operating at the low voltage, as long as none of them ever drove it `HIGH` (though, if it was low, they would not know which device was doing that. The designer would have to make sure this approach made sense in that regard, and that everything was smart enough to not hold onto the line or anything.

## PINCONFIG and associated registers (Dx, Ex only)
This section applies only to DxCore.  This doc was a bit lengthy.

This core *currently* does not make use of this feature - However in the future, sleepTime will have a function that will turn pins to their most power efficient settings en masse. On parts with PINCONFIG, that will be used.

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

These parts (like many electronics) are better at driving pins low than high, and that's not just in brute force - they rise slower than they fall. Sometimes this rapid slew rate can be an issue. With slew rate limiting enabled, the pins take about twice as long to transition. (all values are from the typ. column - neither min nor max are specified; one gets the impression that this is not tightly controlled):

| Direction   | Normal | Slew Rate Limited |
|-------------|--------|-------------------|
| Rising      |  22 ns |             45 ns |
| Falling     |  16 ns |             30 ns |

With a 24 MHz system clock, that means "normal" would be just over half a clock cycle while rising, and just under half a clock cycle falling; with slew rate limiting, it would be just over a clock cycle rising, and 3/4ths of a clock cycle on the falling side.

## There is no INLVL or PINCONFIG on tinyAVR devices
The configurable input level option (`INLVL`) is only available on the AVR Dx (except for the DA - it was added with the DBs along with MVIO for obvious reasons, and I guess the cost of including it everywhere was small enough that they figured "might as well") and Ex series (and likely any future series)

## Ballpark overhead figures
The digital I/O functions are astonishingly inefficient. This isn't my fault (not on mTC - on DxC I have definitely not helped...) - it's the Arduino API's fault
These figures are the difference between a sketch containing a single call to the function, with volatile variables used as arguments to prevent compiler from making assumptions about their values, which may substantially reduce the size of the binary otherwise.

The comparison to the fast I/O functions is grossly unfair, because the fast I/O functions have constant arguments - but this gives an idea of the scale.
Remember also that executing 1 word of code (2 bytes) takes 1 clock cycle, though not all of the code paths are traversed for all pins (indeed, we know that large portions will be skipped for all pins, because each pin can have only one PWM timer, but on the other hand, there are a few small loops in there). Gives an idea of the scale.

| Function           | DB64 | t3224 | t3216 | Notes
|--------------------|------|-------|-------|-------------------------
| pinMode()          | +306 |  +112 |  +130 | Does NOT call turnOffPWM().
| digitalWrite()     | +656 |  +216 |  +340 | Calls turnOffPWM(), included in figures.
| both of above      | +784 |  +300 |  +358 | Calls turnOffPWM()
| openDrain()        | +584 |  +136 |  +272 | calls turnOffPWM()
| turnOffPWM()       | +572 |  +100 |  +266 | Dx pays dearly for the pwm remapping. 3216 is paying for the TCD PWM.
| dW-turnOffPWM()    |  +84 |  +126 |  +112 | Digital write times above less the turnOffPWM()
| oD-turnOffPWM()    |  +12 |   +36 |    +6 | Digital write times above less the turnOffPWM()
| digitalRead()      |  +76 |   +60 |   +72 | Official core calls turnOffPWM (we don't)
| pinConfigure() 1   | +216 |  +104 |  +116 | Only direction/output level
| pinConfigure() 2   | +362 |  +202 |  +220 | Only pullup, input sense, and direction/outlevel
| pinConfigure() 3   | +414 |  +232 |  +252 | Unconstrained, DB has INLVL, tiny does not.
| digitalWriteFast() |   +2 |    +2 |    +2 | With constant arguments.
| pinModeFast()      |  +12 |   +12 |   +12 | With constant arguments.

These figures compare the size of a while loop that continually writes to a pin number that it loads from an I/O register. This is compared with an assignment of a value to one of those registers in an otherwise empty sketch. That is to say, a sketch for a 3224 that uses pinMode, digitalWrite, openDrain, and digitalRead will use less than 300 + 136 + 60 = 496 more than a sketch with just the dummy loop (ie, `while (1) {GPIOR1 = GPIOR0};`). This comparison thus includes the digital_pin_to_* tables, with one entry per pin each. There are 4 such tables (bit_mask, bit_position , port, and timer). *Thus this table does not include call overhead, because the function, called from a single place, gets inlined*. The overhead of such a call is at least 2 bytes per place that it is called from, but that neglects additional overhead required by making callable functions. Once the processor has loaded the values of the arguments into a working register (how long this takes depends on your code), it may need to move them around to put them in the registers according to the avr-gcc ABI, and then the function may have to then move them again to get them into the register it needs them in.

The takeaways from this should be that:
* `digitalWrite()` can easily take 35-75 or more clock cycle with turnOffPWM and then over 50 itself. So digitalWrite can take up to 7 us per call (at 20 MHz; digitalWriteFast takes 0.05 us), and it is by far the worst offender. A small consolation is that turnOffPWM does not execute the entire block of code for all parts. It checks which timer is used, and uses that block, which very roughly gives 70 clocks on a TCA pin, around 150 on a TCD pin, and considerably fewer on a pin with no PWM, though it still has to determine that the pin doesn't have pwm.
* If you know you don't have PWM coming out of a pin that you want to make stop, pinConfigure is faster than `pinMode()` and `digitalWrite()`, especially if only setting up the more common properties. pinConfigure 1 above has the effect of a combined pinMode and digitalWrite, and it's faster than either of them.
* With great power comes greater overhead. With only 1 timer doing PWM, the 3224 only needs 72 words to handle turning off that timer. The 3216 with 2 TCD pins has to pay around 150 words of flash in turnOffPWM, and a similar amount in analogWrite. And on Dx, it's even worse (though you only traverse a small portion of it on any given call. But we gotta handle 3 kinds of timers, some with errata to work around)
* pinConfigure is optimized differently when called with the second argument constrained to 0x00-0x0F (setting only direction and output value) (1), with it constrained to that plus configuring pullups (2), and without restrictions (3).
* Up to 4 bytes times the highest numbered digital pin are in the form of lookup tables. Which tables the compiler needs to include depends on which functions are used.
  * digital_pin_to_port[] is used by all of them.
  * digital_pin_to_bit_mask[] is used by all of them.
  * digital_pin_to_bit_position[] is used by is used by every call that needs to set the pullup, or configure interrupts on a pin;
  * digital_pin_to_timer[] is used by `turnOffPWM()`, anything that calls that, and `analogWrite()`.
  * The tables are only used when the pin is not known at compile time, and it hence must be able to look up an arbitrary pin number. Fast digital I/O requires the pin number to be a compile time known constant (that's where a significant portion of the benefit is concentrated. )
* This is why the fast digital I/O functions exist, and why there are people who habitually do `VPORTA.OUT |= 0x80;` instead of `digitalWrite(PIN_PA7,HIGH);`
* This unexpectedly poor performance of the I/O functions is also why I am not comfortable automatically switching digital I/O to "fast" type when constant arguments are given. The normal functions are just SO SLOW that such a change would be certain to break code implicitly. I have seen libraries which, for example, would not meet (nor come close to meeting) setup and hold time specifications for an external device, but for the grindingly slow pin I/O. *This breakage would likely be wholly unexpected by both library author and the user, and would furthermore be very difficult for most users to diagnose, requiring an oscilloscope, awareness of the poor performance of these API functions, and sufficient familiarity with electronics to know that this was a problem that could occur*. Hence, it would be directly contrary to the goal of making electronics accessible (which Arduino is directed towards), and the goal of maintaining a high degree of compatibility with existing Arduino libraries, which is one of the primary goals of most Arduino cores.

megaTinyCore and DxCore, like all Arduino cores, provides implementations of the usual digital I/O functions. *These implementations are not guaranteed to have identical argument and return types, though we do guarantee that same will be implicitly convertible to the types used by the reference implementation* See below for specifics

pinConfigure called with second argument constrained to 0x00-0x0F (setting only direction and output value) (1), with it constrained to that plus configuring pullups (2), and without restrictions (3).

About 200 bytes of that space is used for lookup tables, not instructions.

This is why the fast digital I/O functions exist, and why there are people who habitually do `VPORTA.OUT |= 0x80;` instead of `digitalWrite(PIN_PA7, HIGH);`

It's also why I am not comfortable automatically switching digital I/O to "fast" type when constant arguments are given. The normal functions are just SO SLOW that you're sure to break code that hadn't realized they were depending on the time it took for digital write, even if just for setup or hold time for the thing it's talking to. And yes, I absolutely have seen an example of the code where they used super short delays amongst `digitalWrites()`, and the result was totally different than what their comments described. In their case, the whole thing was a kludge, so it was compensated for by an arbitrary fudge factor somewhere (IIRC there were several of those - what was it, making a tiny85 or something act like a



## Standard and semi-standard API functions
There are a number of macros defined (as is the case with most cores) which provide a certain set of basic functions that sketches - or more importantly, library code - can use to get information on a pin number. Though these look like functions, they are preprocessor macros. This distinction only occcasionally becomes important.

The return type (since it's a macro) is undefined; what is presented below is intended to give an indication of what the type it would return, and what argument types it would expect, if it were a function rather than a macro.

### Basic pin information
```c++
uint8_t digitalPinToPort(uint8_t pin);
uint8_t digitalPinToBitPosition(uint8_t pin);
uint8_t digitalPinToBitMask(uint8_t pin);
uint8_t analogPinToBitPosition(uint8_t pin);
uint8_t analogPinToBitMask(uint8_t pin);
```
These take a pin number (we recommend representing pin numbers with a uint8_t, though it is unfortunately common practice to use 16-bit signed integers to store pin numbers in Arduino-land). If the pin passed is valid, these will return either the bit position (PIN_PA0 would return 0, PIN_PD4 will return 4 and so on), the bit mask (bit mask = 1 << (bit position), so PIN_PA0 would return 0x01, PIN_PD4 would return 0x10, etc) `*`, or the port number (PORTA = 0, PORTB = 1, etc).

> On CISC instructionsets, a right/left-shift-by-n instruction is common. We, obviously, have no such instructions; shifting a 1 byte value N bits in either direction  So if you want to do 1 << x where x is known at compile time, that's a 1 word 1 clock ldi, then a loop over the . if x is volatile or unknown at compile time it does the equivalent of while (x){ val << 1; x--}. That's like 4 or 5 clocks per bit shifted. It's just as shitty going the other direction, See, that's *WHY* we have a bitmask and a bitposition table. :-/

In the event that the pin passed to it does NOT correspond to a valid pin, the following behavior should be expected:

* If the pin does not exist, with the exception of `digitalPinToPort()`, these will all always return NOT_A_PIN, a numeric constant with a value of 255 (or -1; 255 and -1 are equivalent when assigned to an 8-bit type).
* digitalPinToPort has special behavior for one specific sort of situation: The case of the first pin (ie, Px0) within a port when that pin doesn't exist but other parts of port does. The two cases where we do this are the DB/DD with 28 and 32 pins (PD0 replaced with VDDIO2 there for MVIO), for PC0 for 14/20 pin DD's, and for (PC0 replaced with VDDIO2, cause the 14/20 pinout parts don't have the lower half of port D), Otherwise, `digitalPinToPort()` will return NOT_A_PORT if the pin is invalid. (also has a value of 255/-1 - the two are used just to make intent clearer and improve code readability).
  * This is done because there are sometimes cases, particularly for TCA-driven PWM, where you need to know what the first pin number is, so that you can count up from that to find the pins on that port that can output TCA PWM (the first 6 pins on every port can output TCA0 PWM, and the same pins on PORTB and PORTG can output TCA1 PWM, if both those pins and TCA1 are present).
  * This is only possible on DxCore because all ports are always numbered in the same order that they appear on the chip. It does NOT work on megaTinyCore (the pins aren't in order on those chips, and the TCA outputs are not as trivially ordered anyway and besides, flash is often tight enough that we can't be wasting even a small amount of flash).
  * Any pin number corresponding to a skipped number in the pinout chart, and which would not be pin 0 within that port, will return NOT_A_PORT.
  * Therefore, in order to test whether a pin exists or not, don't use `digitalPinToPort()`.

The analog pin versions will additionally return NOT_A_PIN in the event that the pin does not have analog input capability, even if it exists. Note that analog and digital pin numbers are the same on DxCore. There is no concept of a separate numbering system for analog pins.


### Things that return pointers
```c++
volatile uint8_t* getPINnCTRLregister(uint8_t port, uint8_t bit_pos);
PORT_t* digitalPinToPortStruct(uint8_t pin);
PORT_t* portToPortStruct(uint8_t port);
volatile uint8_t* portOutputRegister(uint8_t port)
volatile uint8_t* portInputRegister(uint8_t port)
volatile uint8_t* portModeRegister(uint8_t port)
```
`getPINnCTRLregister` returns a pointer to the pin's PINnCTRL register, which is used to configure things like pin interrupts, invert the pin, and control the pullup - all things that pinConfigure() above can also do. This is a *pointer* to an 8-bit register.

Two of them return a pointer to the PORT struct (having type PORT_t) for the port that this pin is part of (or for the port of that number, in the case of portToPortStruct).

Finally, the last three take a port number (from digitalPinToPort) and return a pointer to the PORTx.OUT, PORTx.IN, or PORTx.DIR register. These are pointers. In the case of OUT and DIR, you can use pointer arithmetic to reach three highly useful registers from there: Add one to get OUTSET/DIRSET, add 2 to get OUTCLR/DIRCLR, and add 3 to get OUTTGL/DIRTGL.

When working with the SET/CLR/TGL registers, remember that you must never use operators like |= or &= - you write a 1 to the bits you want to set, clear or toggle: These registers *read* as the value of the corresponding register, so `*(portOutputRegister(0) + 2) |= 1<<4;` clears the output bit of EVERY pin in PORTA that is set, not just PA4! (it reads the current value, which mirrors PORTA.OUT. It then sets bit 4 of that value, and writes the modified value to PORTA.OUTCLR, resulting in bit 4 being cleared, along with any other bit that was 1). To add insult to injury, it uses 2-3 more words of flash and 3-4 more clock cycles to do an inappropriate read-modify-write cycle than it would to do the correct thing, and simply write the value, eg (`*(portOutputRegister(0) + 2) = 1<<4;`which is equivalent to `PORTA.OUTCLR = 1<<4;`)

## turnOffPWM(uint8_t pin) is exposed
This used to be a function only used within wiring_digital. It is now exposed to user code - as the name suggests it turns off PWM (only if analogWrite() could have put it there) for the given pin. It's performance is similar to analogWrite (nothing to get excited over), but sometimes an explicit function to turn off PWM and not change anything else is preferable: Recall that digitalWrite calls this (indeed, as shown in the above table, it's one of the main reasons digitalWrite is so damned bloated on the DX-series parts!), and that large code takes a long time to run. DigitalWrite thus does a terrible job of bitbanging. It would not be unreasonable to call this to turn off PWM, but avoid digitalWrite entirely. The pin will return to whatever state it was in before being told to generate PWM, unless digitalWriteFast() or direct writes to the port register have been used since. If the pin is set input after PWM is enabled with analogWrite (or through other means - though in such cases, the behavior when turnOffPWM() is called on any pin which nominally outputs PWM controlled by the same timer *should be treated as undefined* - it is not possible to efficiently implement such a feature on AVRs nor most other microcontrollers while maintaining behavior consistent with the API - the core would (at least on an AVR) have to iterate through every possible source of PWM, testing each in turn to see if it might have PWM output enabled on this pin. In many cases, including this core, each timer would require checking a separate bit in one or more special function registers, as well as storing a database that associates each pin with one or more combinations of bits and the corresponding bit to clear to disable PWM through that pathway. Suffice to say, that is not practical within the resource constraints of embedded systems.

## Finding current PWM timer, if any: digitalPinToTimerNow() (DxCore only)
On many cores, there is a `digitalPinToTimer(pin)` macro, mostly for internal use, which returns the timer associated with the pin. The Arduino API implicitly assumes that this is constant, and both core libraries and code in the wild often assume that it is. The digitalPinToTimer() implementation is as a macro, which frequently is optimized to a constant (provided the argument is constant, of course). The assumption that there exists not-more-than-1-to-1 timer:pin mapping was never universally valid even among AVRs (there are examples of classic AVRs that have multiple timers usable on some pins, and some even permit that to be remapped easily at runtime (the classic ATtiny841 being a notable example). All modern AVRs have at least two pins available to each timer output compare channel; and while the tinyAVRs never have more than two timers potentially usable with a given pin, the Dx-series allows one of up to 7 pins to be associated with a given timer (in groups), with some pins having as many as 4 options. This is documented in those datasheets and the DxCore documentation.

## The following cases are NOT EXPECTED TO WORK
Avoid placing yourself in any of the following situations, as these **should not be expected to produce the desired result:**
* **Calling turnOffPWM(), digitalPinToTimer(), ~digitalPinToTimerNow()~ or analogWrite() on a pin nominally driven by a timer which the core has been instructed not to reconfigure using the takeOverTCxn() function**. The core will treat the pin as an ordinary digital pin - by calling the timer takeover, you assume full responsibility for all management of that timer. That function must be called if any manual configuration of a PWM timer will be or has been performed, except as specifically noted in in the timer and PWM reference or type D timer reference (we make an effort to give more leeway with semi-manual configuration of TCD (particularly on DxCore, where the timer is more powerful) because fully manual configuration is a pain in the arse).
* **Calling turnOffPWM(), digitalPinToTimer(), digitalPinToTimerNow(), analogWrite() or digitalWrite() on a pin nominally driven by a type A timer which has been manually reconfigured**, *particularly* if it has been configured in *single mode* for 3x16-bit PWM channels. The Arduino API does not support PWM with > 8 bits of resolution, so we don't take it outside of 8 bit mode, meaning we're writing halves of registers separately. . Users who require that level of control must call `takeOverTCxn()`. See the timer and PWM reference and the included TCA takeover examples.
* ~turnOffPWM(), digitalPinToTimer(), digitalPinToTimerNow() or analogWrite() or digitalWrite() on a pin nominally driven by a type B timer if the TCB portmux was used move the output off of that pin.~ We don't support PWM from TCBs, so this issue is irrelevant to mTC as we donot support wasting our precious TCB(s) on generating a single mediocre 8-bit PWM channel!
* **Manually configuring any timer without calling takeOverTCxn, except as explicitly documented in the core documentation, and then calling any function on a pin nominally (but not presently) controlled by that timer** (be sure you are reading the correct core's documentation - usually mTC = DxC, but in this case, that assumption is NOT valid - the ample resources of the Dx-series allow us a bit more room to be clever there, particularly regarding TCD0, which is considerably more powerful on the Dx-series than on tinyAVR 1-series - more because of the features around the timer than any specific differences in the timers - TCD is near identical, but on the Dx, the richer environment and new clocking options, including the PLL, making the incentive correspondingly greater for a more flexible implementation there.
* **Changing PORTMUX manually on megaTinyCore** - we do not support this. Use the PWM option menus to set this at compile-time - this greatly simplifies (read, smaller and faster) the code. The mux options on the tinyAVRs were also picked apparently while still under Atmel's "dart board" peripheral placement policy, whereas there are more symmetries to take advantage of on Dx-series and Ex-series parts, and they have oodles more flash to do that taking advantage in.
* Setting an interrupt on a pin on a Major Die Rev. "A" DA or DB-series device, then performing an `analogRead()` of the pin, due to an erratum impacting these devices: Whenever selected by the POSMUX/NEGMUX registers, while the ADC is enabled (like all cores, we do this during `init()`) get their digital input buffers disabled (yes, this meant that when I did a crude sanity check - turn on all the pullups, then read each port, and expect to see a 1 everywhere there is a pin other than the serial port you're outputting on - I found that PD0 (AIN0) was not reading anything. After the initial errata list came out, a trivial workaround was implemented (enabled by the improvements to the ADC on modern AVRs: On classic AVRs, remember how you had to take several throw-away readings and then your actual reading you used? Yeah you don't need to do that so much anymore. The effect is still there, but considerably smaller.

(*Another one of your stupid "physical laws"? Bah! Vote for me! Vote for the repeal of physical law!* "Do you think people will fall for that?" *Why not? Candidates ignore mathematics all the time, why should physics be any different?* "I thought you were running for president, not God" *I was, then I watched the longest standing democracy impeach two of their presidents in a row. Now I'm aiming higher - ever hear of a God-Emperor?* "Yes, but I was under the impression that they eschew elections..." *Once they're in power sure, but they gotta get there somehow*)


## Note about about the hardware, drive strength
As there is a good chance you;ve noticed, a lot of hardware is better at driving pins low than high (sinking vs sourcing). Classic AVRs supposedly had symmetric drive - they were very similar in their ability to source and sink current (this varied by part though - for comparison below I used the last classic AVR that came out in it's fully evolved form, the 328PB. But ever since they've been publishing graphs, it's been clear the drive strength is not symmetric.

The pretense - in the form of matching specs in the electrical characteristics table - was abandoned on the Dx-series parts, though it mysteriously returned in the Ex-series, though none have gotten graphs published yet... This summarizes the differences in digital I/O capability (not counting pincount and peripheral differences)

The EB specs 0.4V max of drop, symmetric, at 6mA and Vdd = 3.0V. But as with the tinies and classics, that doesn't mean it actually is symmetric. The Dx-series just admitted it, specing 0.6V sinking 10mA at 3.0V and 0.7V sourcing 6mA

| Part Family         | classic | tiny0/1 | tiny2  |    Dx   |    Ex   |
|---------------------|---------|---------|--------|---------|---------|
| Abs max current     | +/-40mA | +/-40mA |+/-40mA | +/-50mA | +/-40mA |
| Abs max injection   | +/-1 ma | +/-15mA |+/-15mA | +/-20mA | +/-20mA |
|    .. @ Vdd > 4.9   | +/-1 ma | +/-1 mA |+/- 1mA | (same)  | (same)  |
| IOH, max on graph   | 20 mA   | 20 mA   | 20 mA  |   50 mA | tbd**   | Sourcing Current Current shown for highest voltage shown on the highest operating voltage chart (the most favorable)
| Vdd - V@IOHnax      |  1.05V  |  0.65V  | 4.3V   |   2.0 V | tbd**   | Voltage shown corresponding to that point. Both are at the "25 C mean" line.
| IOL,max from graph  | 20 ma   | 20 mA   | 20 mA  |   100mA | tbd**   | These taken together imply that the tiny2 is about 62% better at sinking current than sourcing it (as measured by voltage drop)
| VOL,max from graph  |  0.45V  |  0.45V  | 0.43   |   1.4V  | tbd**   | while the Dx is 185% better at sinking current than sourcing it, under extreme conditions. Who the hell tests and gives graphs to 2x the abs max? Love it!
| Rpullup             | ~35k    | ~35k    | ~35k   |   ~35k  | 32k     |
| INLVL supported     |      No |      No |     No |     Yes |     Yes | Not supported on AVR DA(S).
| Fully Async Pins    | none    |    2, 6 |  2, 6  | 2,6/All |     All | From the DD onward, all pins are treated as "fully async". It means you can trigger on super short pulses (and noise), and wake on rise/fall, not just change/low. On all pins.
| PINCONTROL bulk-set | n/a     |      No |     No |     Yes |     Yes | The PINCONTROL bulk-PINnCTRL writing registers are only on Dx/Ex, where there may be a lot more unused pins.
| EVGENCTRL           | n/a     |      No |     No | DU Only |     Yes | The DU, being rather late (notice how two of the E's cut in front of it), did manage to pick up the EVGENCTRL features (user facing only if not using included Event.h library. It's an improvement, as it makes the event channels - finally - equivalent.
| Those special pins  | -       |  -      | -      | -       | -       | UPDI and reset have various levels of functionality on different parts.
| UPDI pin            | n/a     |    PA0  | PA0    | PF7*    | PF7     |
| Pin for HV pulse    | Reset   |    PA0  | PA0    | PF6*    | PF6     | This is the pin that the HV pulse for reprogramming a UPDI pin set to be non UPDI ust be sent to. If it can be used as GPIO, the pin drive is ~1/30th that of other pins.
| Pin PA0 can be GPIO | ***     | Hard    | Hard   | n/a     | n/a     | Nothing special about PA0 on Dx/Ex, other than that if there's a crystal somewhere, it's likely on those pins.
|   .. can be Reset   | n/a     | Possible| Yes    | n/a     | n/a     | as above
|   .. can be UPDI    | n/a     | Default |Default | n/a     | n/a     | as above
| Pin PB4 can be RST  | n/a     |   No    | Yes    | No      | No      | Note that 14-pin parts don't have a PB4.
| Pin PF6 can be RST  | n/a     | n/a     | n/a    | Yes     | Yes     |
|   .. can be GPIO    | n/a     | n/a     | n/a    | No      | No      | On no modern AVR with a PF6 is the pin usable as an output.
|   .. can be GPI     | n/a     | n/a     | n/a    | see     |  -->    | Yes. And this is the DEFAULT setting according to Microchip, but on these parts, it's a "harmless" fuse that gets set as specified on all updi uploads.
| Pin PF7 can be UPDI | n/a     | n/a     | n/a    | Yes     | Yes     |
| Pin PF7 can be GPIO | n/a     | n/a     | n/a    | DD/DU   | Yes     |

`*` In the DA and DB-series, although it obviously was PF7, as there was no way to enable it for general I/O prior to the DDs, they never called it that. On the DA and DB families, PF7 cannot be made GPIO, and it's only ever UPDI. Since the DD that is no longer the case, and seems to be part of their standard features now *You mean, they didn't have it ready in time for the DA/DB? Obviously they were planning it, or they'd have left output drivers on PF6.* "Or they didn't want to add unnecessary porting complexity for the same of 1 extra output on the highest pincount devices in the pipeline"

`**` - It remains to be seen what the pin drive strength will be like on the Ex-series or any other future part, and until the IO pin output current is added to the characteristics graphs section of the datasheet, you don't really have much information; preliminary datasheets typically omit this sort of data. Right now in the EA-series datasheets, we still have a specification of Minimum, Typical, and Maximum VOH and VOL of "-", "-", and "-" (though they helpfully note that the test conditions are 6mA at VDD = 3.0V; presumably yet to be conducted). Now, one really has to wonder how it takes them so long to characterize the behavior of an I/O pin. I understand how complex peripherals would be hard to test. And subtle ISA bugs like the cursed write sequence that the compiler would be perfectly within it's rights to emit but never does, nobody noticed that for like 5 years or more. But an I/O pin is pretty simple. You put a controlled load on it, and change the load while measuring the voltage on the pin. It's the sort of thing that I'm sure, nowadays, would be easy for anyone well versed in microcontrollers to automate, especially if they had to do that sort of thing, oh, every time they released a product? I'm sure Microchip has some employees who are quite knowledgeable about microcontrollers. I sure hope they do. Like, I'd expect new figures to be automatically generated by the automated testing on each new internal development rev that comes to of the fab.

`***` - On a Tiny85 or Tiny84/841, HVSP really doesn't seem too bad - sure it was way more of a pain than normal ISP, but normal ISP kinda sucks in most ways compared to UPDI. It's similar to a modern AVR with UPDI disabled, and I'd call it "Hard". But only those three part families use HVSP. Everything else takes HVPP to reprogram after reset is disabled, and HVPP requires connection of around 20 wires to the target (with 20 chances to make a mistake), most of them can't be loaded so you can just forget about doing it in system. I do not recall reading any account on forums of someone using HVPP outside of work, and those were rare. HVPP is much harder than "Hard".

### The tinies aren't symmettric drive?
You might be scurrying to your datasheets thinking "I swear they were symmetric on the tinys!" and if you look at the table - which gives worst case values, they are. But the graphs, which give observed "typical" values, tell a different story - they show a voltage drop when sourcing 20mA on a tiny2 of around 0.7V at room temp, but a voltage drop of 0.4-0.45 sinking the same amount of current. The low side drivers are stronger. The Dx-series takes it to a new level: At max opperating voltage (5.5v) they ran it up to the absolute max on the high side (recording a 2 volt drop). On the low side though, at the 50mA absolute max, they were still only seeing 0.6V. So they ran it up to twice the absolute maximum rating, which saw a 1.4V drop. Though it's hardly novel for the graphs in the datasheet to show that that the parts typically blow their rated specs out of the water, at least briefly or in favorable conditions (as in when being pushed very hard) or under normal, mild conditions; no particular AVR example comes to mind, but my preferred 1117-series regulator specs the usual dropout as a maximum. The typical characteristics graphs, in contrast, show dropout on the order of a tenth of a volt at light loads. I appreciate the transparency on behavior in overload conditions. I wish they did that more.

**IN FACT, NONE OF THE AVRS HAVE EVER ACTUALLY BEEN SYMMETRIC DRIVE!**
Not classic tinies, not classic atmegas. They met symmetric specs, the low side driver always met it with far more room to spare than the high side one. That said, the differene in drive strength is on the magntude of a power of two. (compare to some older devices that would have a 0.4mA high-side drive and 20mA low side drive (at least they could drive high, which wasn't universal then))

### Other notes on that table
What tinyAVR calls current injection, Dx/Ex-series call "Clamp current"; the latter is, in my opinion, more proper. It's current through the protection diodes, which are called "clamp diodes" because they constrain ("clamp") the voltage between supply and ground, thus preventing damage to the chip. As these are microcontrollers not power rectifiers, these diodes are very small, so the current through these diodes has to be kept low, else they fail, followed immediaely by the pin-driver they were protecting. This failure is typified by the pin always behaving as if set output, driving in the direction of the voltage excursion. Oddly It wasn't spec'ed on classic AVRs (but was well known to be low, and to be a highly effective way of blowing pins). It's said that the most experienced AVR developers can simply stare intently at a chosen pin on the old classic AVRs, imagining a voltage in excess of VDD applied to it, and burn out the pin driver just like that. In any event, Atmel employees are rumored to have said +/- 1mA maximum on classic AVRs, (in spite of an app note advising connecting a tiny85 through a high value resistor to mains to do zero cross detection. That app note describes basically doing what the ZCD on the Dx-series does, only with less help from the hardware).

Anyway, the takeaways I intended from this are:
* The I/O pins we have are a hell of a lot better than the ones classic had, and they're getting better
* The I/O pins on the Dx-series in particular are spectacularly strong, and they have greatly improved resilience to clamp current across the board.
* Pin drive is not symmetric. This has ramifications for the design constraints on an RC filter used to make analog voltages out of PWM.
* The section of the datasheet where they are talking about



## Note on number of pins and future parts
The most any announced AVR has had is 86 digital pins, the ATmega2560; In the modern AVR era, the digital pin maximum is 55, and the maximum possible without rearchitecting how pins are accessed is 56 (ie, if UPDI could be turned into I/O on DA/DB we are already there). There is no way for an AVR that allows the same bit-level access that we enjoy, on all pins, to have more than 56 pins within the AVR instruction set. 88 digital pins could fit in 11 ports, which would likely be PORTA, B, C, D, E, F, G, H, J, K, and L (They in the past have skipped PORTI, ex on 2560, probablty because I and 1 look so similar). Each port takes up 4 of the 32 addresses in the low I/O space for the VPORT registers, on which the single cycle bit-level access relies, and 4 are used for GPIORs, so we would need 16 more addresses in the low I/O space to acccomodate 4 more ports. Only time will tell how this is handled. Possibilities include:
* **Extend CBI/SBI/SBIC/SBIS with a new instruction variant** - Hahahaha! Right! That'll be the day. Very unlikely. However, it is *possible* in theory: Each additional register requires 32 opcodes (1 each for SBI and CBI, SBIC and SBIS, times 8 bits), so each port takes only 128 opcodes so 512 opcodes would be sufficient to give working VPORTs to up to 88 pins, and there are like 1500 opcodes not used by any other instructions, some of those in nice clean blocks that would work well here.
* **No VPORTH or higher** - This is clean, simple, and limiting in some ways, but probably the best route. If I had to place a bet, this would be what I bet on. Making the vports movable like they were on xmega raises the complexity a lot for relatively small returns.. If you needed to use VPORT access, you'll just need to use the lower 7 ports for it. Surely you don't need 57+ pins all with VPORT atomic single cycle access! This is also, in my judgement, most likely based on the fact that on the ATmega2560, the only precedent we have for running out of low I/O registers for pins, they didn't even put ports H through L into the high I/O space (heck, not only that, they did not, as far as I can see, so much as mention the pin inequality in the datasheet!).
* **VPORTs in High I/O** - There are many ways of dealing with it using a new set of differently behaving registers in the high I/O space. None of them retain the arbitrary compile-time-known-bit-write-is-one-clock of current VPORTs. At 8 registers per port, you could make a series of N operations on a bit or number of bits take N+1 clocks, and keep them atomic. This would mean a VPORT.DIR/DIRCLR/DIRSET, and VPORT.OUT/OUTSET/OUTCLR and VPORT.IN(writing to IN toggles, and there was never a single cycle DIR toggle). and VPORT.INTFLAGS. That's viable, though with the other registers in that, you could get at most only 3 more ports - which is only 24 pins: You couldn't quite do it for a 100 pin chip). Other solutions would lower capability and comprehensibility in exchange for squeezing in more pins. No matter how you do it, it doesn't really solve the problem.
* **VPORTs that can be remapped at runtime** - The stuff of library author's nightmares. You'd have some subset of the VPORTs (or maybe all of them) that could be remapped. If you put the register for that in the high I/O space it could be written quickly. But this will cause unaware old code to break by writing to the wrong pins if it didn't know to check for it, and any code that used them (ie, code that is supposed to be fast) might have to check the mapping first. This is most like what was done on the XMegas, but also serves as a shining example of the over-complexity that I think doomed those parts to poor uptake and slow death. I think this method is pointless - it is very difficult to envision a scenario where a remappable VPORT would be faster if code had to either set or test a value before accessing it.

`*` - Note on conventions for specifying numbers: 0x## refers to a hexadecimal number; this will always be written with a power of 2 number of hex digits, padded with a leading zero nybble if needed. A number written without a 0x prefix, ie, `12` refers to a decimal value. 0b######## refers to a value given as binary. For hexadecimal values, if the size of the datatype is unambiguosly known, we will typically represent them with an appropriate number of leading 0's - so 1 in a 1-byte datatype is written as 0x01, while 1 in a 16-bit datatype is written as 0x0001. Any time a number is not prefixed by 0x or 0b, the decimal form should be assumed. Which representation of a given value is chosen is based on the context of that value. Values that simply represent numbers are generally given in decimal. Values that are being subjected to bitwise operators, or that are bit masks, group codes, and similar, will be shown in hexadecimal, or when the bit math is particularly obtuse but the significance of the data is by bitfield, binary. In C, prefixing a number with 0 in an integer literal - to the surprise of many - will result in it being treated as *octal (base 8)*, which probably made sense at the time for some weird reason, but I can't imagine why someone would want octal.
