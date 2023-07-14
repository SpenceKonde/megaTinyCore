# Improved Digital I/O Functionality
This core includes a number of features to provide more control or performance when doing digital I/O. This page describes how to use them. All of the options that can be configured for a pin are exposed. The only things that aren't exposed are the slew rate limiting feature, and the multi-pin configuration facilities. The slew rate limiting is can only be configured on a per port basis; turning it on and off is so simple (see below) that it needs no wrapper. The multi-pin configuration system does not have an obvious "right way" to expose it and should be handled directly by the sketch - it is very flexible and no wrapper around it would be able to preserve it's virtues while being much of a wrapper.

## But first, about the hardware
As there is a good chance you;ve noticed, a lot of hardware is better at driving pins low than high (sinking vs sourcing). Classic AVRs had symmetric drive - they were very similar in their ability to source and sink current. Though the modern AVRs do not spec a different maximum source and sink current, how well the chip is able to deliver that current is not equal (and likely the ultimate maximum current at which damage occurs in practice is similarly unequal). It has always been easier to pull low that to pull high

So did the modern tinyAVR parts (not exactly, but very close to it.

| Part Family      | tiny0/1 | tiny2 | Dx | Ex | tiny reset pin when used for UPDI or GPIO |
|------------------|---------|-------|----|----|-------------------------------------------|
| Abs max current  | +/-40mA |+/-40mA|+/-50|+/-40 | Unclear - not spec'ed                  |
| Abs max injection| +/-15mA |+/-15mA|+/-20|+/-20 | Unclear - not spec'ed                  |
|    .. @ Vdd > 4.9|+/- 1 mA |+/-1 mA|-   |  - | Unclear - not spec'ed                     |
| IOH,max from graph|        | 20 mA |50mA|tbd | No graph, spec is same drop as normal I/O |
| VOH,min from graph|        | 4.3V  |3.5V|tbd | handling 30x the current. Even at 5V, 2k  |
| IOL,max from graph|        | 20 mA |100mA|tbd| output impedance.                         |
| VOH,max from graph|        | 0.43  |1.4V|tbd | -                                         |

You might be scurrying to your datasheets thinking "I swear they were symmetric on the tinys!" and if you look at the table - which gives worst case values, they are. But the graphs, which give observed "typical" values, tell a different story - they show a voltage drop when sourcing 20mA on a tiny2 of around 0.7V at room temp, but a voltage drop of 0.4-0.45 on at sinking the same amount of current! And what's with the graphs of the Dx-series going to twice the maximum sink current? I noticed that too - though it's hardly novel for the graphs in the datasheet to show that that the parts typica;;y  I appreciate the transparency on behavior in overload conditions.

What tinyAVR calls current injection, Dx/Ex-series call "Clamp current" - it's current through the protection diodes. Oddly It wasn't spec'ed on classic AVRs (but was well known as the usual cause of "blown" pins). Atmel employees are rumored to have said +/- 1mA maximum. Certainly we have made some progress on that front.

It remains to be seen what the pin drive strength will be like on the Ex-series or any other future part, and until the IO pin output current is added to the characteristics graphs section of the datasheet, you don't really have much information; preliminary datasheets typically omit this sort of data, because these properties have not yet been characterized (I'm not sure how they manage that. Automating that testing, while not trivial, shouldn't be a challenge to microcontroller experts. I'm pretty sure there are a considerable number of such persons employed by Microchip.

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
| digitalRead()      |  +76 |   +60 |   +72 | Official core calls turnOffPWM (we don't)
| pinConfigure() 1   | +216 |  +104 |  +116 | Only direction/output level
| pinConfigure() 2   | +362 |  +202 |  +220 | Only pullup, input sense, and direction/outlevel
| pinConfigure() 3   | +414 |  +232 |  +252 | Unconstrained
| digitalWriteFast() |   +2 |    +2 |    +2 | With constant arguments.
| pinModeFast()      |  +12 |   +12 |   +12 | With constant arguments.
| dW-turnOffPWM()    |  +84 |  +126 |  +112 | Digital write times above less the turnOffPWM()

These figures compare the size of a while loop that continually writes to a pin number that it loads from an I/O register. This is compared with an assignment of a value to one of those registers in an otherwise empty sketch. That is to say, a sketch for a 3224 that uses pinMode, digitalWrite, openDrain, and digitalRead will use less than 300 + 136 + 60 = 496 more than a sketch with just the dummy loop (ie, `while (1) {GPIOR1 = GPIOR0};`). This comparison thus includes the digital_pin_to_* tables, with one entry per pin each. There are 4 such tables (bit_mask, bit_position , port, and timer). *Thus this table does not include call overhead, because the function, called from a single place, gets inlined*. The overhead of such a call is at least 2 bytes per place that it is called from, but that neglects additional overhead required by making callable functions. Once the processor has loaded the values of the arguments into a working register (how long this takes depends on your code), it may need to move them around to put them in the registers according to the avr-gcc ABI, and then the function may have to then move them again to get them into the register it needs them in.

The takeaways from this should be that:
* digitalWrite() can easily take 35-75 or more clock cycle with turnOffPWM and then over 50 itself. So digitalWrite can take up to 7 us per call (at 20 MHz; digitalWriteFast takes 0.05 us), and it is by far the worst offender. A small consolation is that turnOffPWM does not execute the entire block of code for all parts. It checks which timer is used, and uses that block, which very roughly gives 70 clocks on a TCA pin, around 150 on a TCD pin, and considerably fewer on a pin with no PWM, though it still has to determine that the pin doesn't have pwm.
* If you know you won't have PWM coming out of a pin, pinConfigure is faster than pinMode() and digitalWrite(), especially if only setting up the more common properties. pinConfigure 1 above has the effect of a combined pinMode and digitalWrite, and it's faster than either of them.
* With great power comes greater overhead. With only 1 timer doing PWM, the 3224 only needs 72 words to handle turning off that timer. The 3216 with 2 TCD pins has to pay around 150 words of flash in turnOffPWM, and a similar amount in analogWrite. And on Dx, it's even worse (though you only traverse a small portion of it on any given call. But we gotta handle 3 kinds of timers, some with errata to work around)
* pinConfigure is optimized differently when called with the second argument constrained to 0x00-0x0F (setting only direction and output value) (1), with it constrained to that plus configuring pullups (2), and without restrictions (3).
* Up to 4 bytes times the highest numbered digital pin are in the form of lookup tables. Which tables the compiler needs to include depends on which functions are used.
  * digital_pin_to_port[] is used by all of them.
  * digital_pin_to_bit_mask[] is used by all of them.
  * digital_pin_to_bit_position[] is used by is used by every call that needs to set the pullup, or configure interrupts on a pin;
  * digital_pin_to_timer[] is used by turnOffPWM(), anything that calls that, and analogWrite().
  * The tables are only used when the pin is not known at compile time, and it hence must be able to look up an arbitrary pin number. Fast digital I/O requires the pin number to be a compile time known constant (that's where a significant portion of the benefit is concentrated. )
* This is why the fast digital I/O functions exist, and why there are people who habitually do `VPORTA.OUT |= 0x80;` instead of `digitalWrite(PIN_PA7,HIGH);`
* This unexpectedly poor performance of the I/O functions is also why I am not comfortable automatically switching digital I/O to "fast" type when constant arguments are given. The normal functions are just SO SLOW that such a change would be certain to break code implicitly. I have seen libraries which, for example, would not meet (nor come close to meeting) setup and hold time specifications for an external device, but for the grindingly slow pin I/O. *This breakage would likely be wholly unexpected by both library author and the user, and would furthermore be very difficult for most users to diagnose, requiring an oscilloscope, awareness of the poor performance of these API functions, and sufficient familiarity with electronics to know that this was a problem that could occur*. Hence, it would be directly contrary to the goal of making electronics accessible (which Arduino is directed towards), and the goal of maintaining a high degree of compatibility with existing Arduino libraries, which is one of the primary goals of most Arduino cores.

megaTinyCore and DxCore, like all Arduino cores, provides implementations of the usual digital I/O functions. *These implementations are not guaranteed to have identical argument and return types, though we do guarantee that same will be implicitly convertible to the types used by the reference implementation*

pinConfigure called with second argument constrained to 0x00-0x0F (setting only direction and output value) (1), with it constrained to that plus configuring pullups (2), and without restrictions (3).

About 200 bytes of that space is used for lookup tables, not instructions.

This is why the fast digital I/O functions exist.

This is why the fast digital I/O functions exist, and why there are people who habitually do `VPORTA.OUT |= 0x80;` instead of `digitalWrite(PIN_PA7, HIGH);`

It's also why I am not comfortable automatically switching digital I/O to "fast" type when constant arguments are given. The normal functions are just SO SLOW that you're sure to break code that hadn't realized they were depending on the time it took for digital write, even if just for setup or hold time for the thing it's talking to.

## openDrain()
It has always been possible to get the benefit of an open drain configuration - you set a pin's output value to 0 and toggle it between input and output. This core provides a slightly smoother (also faster) wrapper around this than using pinmode (since pinMode must also concern itself with configuring the pullup, whether it needs to be changed or not, every time - it's actually significantly slower setting things input vs output. The openDrain() function takes a pin and a value - `LOW`, `FLOATING` (or `HIGH`) or `CHANGE`. `openDrain()` always makes sure the output buffer is not set to drive the pin high; often the other end of a pin you're using in open drain mode may be connected to something running from a lower supply voltage, where setting it OUTPUT with the pin set high could damage the other device.

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


| Functionality |   Enable    | Disable            | Toggle |
|---------------|-------------|---------------------|--------------------|
| Direction, pinMode()        | `PIN_DIR_OUTPUT`<br/>`PIN_DIR_OUT`<br/>`PIN_DIRSET` | `PIN_DIR_INPUT`<br/>`PIN_DIR_IN`<br/>`PIN_DIRCLR`       | `PIN_DIR_TOGGLE`<br/>`PIN_DIRTGL` |
| Pin output, `HIGH` or LOW   | `PIN_OUT_HIGH`<br/>`PIN_OUTSET`          | `PIN_OUT_LOW`<br/>`PIN_OUTCLR`                 | `PIN_OUT_TOGGLE`<br/>`PIN_OUTTGL`       |
| Internal Pullup             | `PIN_PULLUP_ON`<br/>`PIN_PULLUP`         | `PIN_PULLUP_OFF`<br/>`PIN_NOPULLUP`            | `PIN_PULLUP_TGL`       |
| Invert `HIGH` and LOW       |v`PIN_INVERT_ON`                          | `PIN_INVERT_OFF`                               | `PIN_INVERT_TGL`       |
| Use TTL levels (DB/DD only) | `PIN_INLVL_TTL`<br/>`PIN_INLVL_ON`       | `PIN_INLVL_SCHMITT`<br/>`PIN_INLVL_OFF`        | Not supported<br/>No plausible use case      |
| Digital input buffer        |`PIN_INPUT_ENABLE`or<br/>`PIN_ISC_ENABLE` | `PIN_ISC_DISABLE` or<br/>`PIN_INPUT_DISABLE`   | Not supported<br/>No plausible use case |
| Interrupt on change         |`PIN_ISC_ENABLE` or<br/>`PIN_INPUT_ENABLE`| `PIN_ISC_ENABLE`                               | Not applicable |
| Interrupt on Rise           | `PIN_ISC_RISE` or<br/> `PIN_INT_RISE`    | `PIN_ISC_ENABLE` or<br/>`PIN_ISC_DISABLE`      | Not applicable |
| Interrupt on Fall           | `PIN_ISC_FALL` or<br/> `PIN_INT_FALL`    | `PIN_ISC_ENABLE` or<br/>`PIN_ISC_DISABLE`      | Not applicable |
| Interrupt on LOW            | `PIN_ISC_LEVEL`  or<br/> `PIN_INT_LEVEL` | `PIN_ISC_ENABLE` or<br/>`PIN_ISC_DISABLE`      | Not applicable |

For every constant with TGL or TOGGLE at the end, we provide the other spelling as well. For every binary option, in addition to the above, there is a `PIN_(option)_SET` and `PIN_(option)_CLR` for turning it on and off. The goal was to make it hard to not get the constants right.

While pinConfigure is not as fast as the fast digital I/O functions above, it's still faster than pinMode().

### INLVL - input logic levels
On some parts (DB, DD, EA - likely all the new "good" parts (but not tinyAVRs. The EB-series is going to be their newer, better, less wacky t861. Considering how badly they've cut features from it, I'll be disappointed if they're not cheap. , pins can be configured to use one of two sets of input voltages: either the normal schmitt triggert input, which has thresholds as a fraction of Vdd, or the TTL mode which has fixed thresholds independent of operating voltage. On MVIO pins, using the schmitt trigger Vddio2 takes the place of VDDIO. Note that this is overridden when pins are used for the TWI - that defaults to I2C voltage levels, but supports an optional SMBus 3.0 mode.

| Voltage             |  Schmitt  |     I2C   |  TTL   | SMBus 3.0 |
|---------------------|-----------|-----------|--------|-----------|
| Low input (Max)     | 0.2 x Vdd | 0.3 x Vdd | 0.80 V |    0.80 V |
| High Input (Min)    | 0.8 x Vdd | 0.7 x Vdd | 1.60 V |    1.35 V |

These are the maximum voltage guaranteed to qualify as LOW and the minimum guaranteed to qualify as `HIGH`. Although the I2C levels are described as being different, it is worth noting that 1) if the schmitt trigger input on a recognized a 0.3 x Vdd voltage as LOW, it would surely do the same for a 0.2 x Vdd one, likewise at the high end. And typical characteristics graphs, when available, for modern AVRs with the same thresholds and I2C levels, show typical I/O pin thresholds as easily meeting the specs given for the I2C levels. The two options for I2C levels are not new for the DB either - they are widespread. What appears to be new is a less tightly specified but conceptually similar input circuit on all pins, selectable by a bit in PINnCTRL, instead of just I2C pins. It's important to keep in mind how this divides up possible input voltages (note: this is just based on room temp typical characteristics)
* With Schmitt trigger inputs, highest and lowest 1/5th of the voltage-space each go to a definite value, while the voltages between allow the hysteresis that the schmitt triggers are known for. The graphs in the DA-series datasheet (DB typical charachteristics are not yet available) show that in practice, the hysteresis occurs over a much narrower voltage range than the same graph for the 4809 (which depicted three regions of similar size).
  * Typical falling threshold ranged from 0.8 up to 2.05, and rising thresholds from 1.05 V to 2.75 V. Thus, the hysteresis region ranged in size from 0.2 to 0.7 volts, but reached a nadir just around 2-2.25 V where it could be as small as 0.15 V, since the lower threshold increases faster than the upper one at the bottom of the range.
  * When Vdd = 5.0 V - 3.3 V is not guaranteed to be `HIGH`, though the typical characteristic graphs suggest that it will be, as has always been the case on AVRs.
* With TTL input levels, behavior relative to the operating voltage varies greatly.
  * With Vdd = 1.8 V (the minimum), below about half the operating voltage, it's going to read low, while it may need to be very close to the power rail to read `HIGH`.
  * With Vdd = 5.0 V (maximum typically used intentionally) anything over about 1/3rd if the supply voltage is guaranteed to be `HIGH`, and you need to get down to 1/6th of the supply voltage for a guaranteed `LOW`.
  * The actual lower threshold is likely to be higher, and the upper threshold lower. For example, on DA-series SMBus pins, those voltages are 1.09 V and 1.24 V over most of the operating voltage range. This leave you with very little hysteresis, if that happens to be problematic.
  * If interacting with an output that for whatever reason struggles to pull the pin as close to ground as an ideal output should, or if you're counting on the transition being somewhere near the middle of the range, definitley don't use this option.

Particularly in the context of these MVIO-equipped parts, however, the TTL input level option has an obvious major advatage: It makes communication with parts that run at lower voltages easier, and reduces competition for the limited number of MVIO pins. If we allow the low part to control it push-pull (aka totam-pole and other names), and treat it as an `INPUT`, we will always read what they intend. The fact that you can switch a pin between input and output, while pulling it up externally was noted above, and the voltage it was pulled up to could be the lower operating vboltage. That can be done on any AVR. But with INLVL, this scheme is viable for a bidirectional line: the The line between devices could either be pulled up to the lowest operating voltage externally, with the DB or DD series part manipulating the pin as an open drain output (see above). One need not stop with just two devices - multiple DB/DD-series parts with `INVLV = TTL` set could communicate with multiple devices operating at the low voltage, as long as none of them ever drove it `HIGH` (though, if it was low, they would not know which device was doing that. The designer would have to make sure this approach made sense in that regard, and that everything was smart enough to not hold onto the line or anything.

## PINCONFIG and associated registers (Dx, Ex only)
The hardware has a series of registers - one shared across all ports, `PORTx.PINCONFIG` with bitfields matching the ones in the the PINnCTRL registers, and three which always read zero, `PORTx.PINCTRLUPD`,  `PORTx.PINCTRLSET` and  `PORTx.PINCTRLCLR`. These allow mass updating of the PINnCTRL registers: Every pin corresponding to a 1 on the port will have the contents of their PINnCTRL register either set to (`PINCTRLUPD`), bitwise-OR'ed with (`PINCTRLSET`) or bitwise-AND'ed with the inverse (`PORTx.PINCTRLCLR`) of the  `PINCONFIG` register. Noting keeps you from setting ISC bitfield that way, you shouldn't do that, unless you really know what you're doing,


So:
```c
PORTA.PINCONFIG = PORT_PULLUPEN_bm;
PORTA.PINCTRLSET = 0b11110000; // Turns on the pullup on PA4~7
PORTB.PINCTRLUPD = 0b10000001; // Sets PINnCTRL to only have pullup enabled on PB0 and PB7. If one of those pins was inverted or using TTL levels or had an interrupt on it, it doesn't anymore.
PORTC.PINCTRLCLR = 0b11111111; // Turns off the pullup on every pin on PORTC
PORTA.PINCONFIG = PORT_PULLUPEN_bm | PORT_INVEN_bm; //invert and turn on pullup
PORTG.PINCTRLUPD = 0b00001111; // inverts and turns on pullups on PG0~3
PORTF.PINCTRLUPD = 0b01010101; // inverts and turns on pullups on PF 0, 2, 4, and 6.

/* executes faster and looks less hideous than: */
PORTA.PIN4CTRL |=  PORT_PULLUPEN_bm; //Read-modify-write takes 6 clocks and 5 words of flash! - lds, ori, sts
PORTA.PIN5CTRL |=  PORT_PULLUPEN_bm;
PORTA.PIN6CTRL |=  PORT_PULLUPEN_bm;
PORTA.PIN7CTRL |=  PORT_PULLUPEN_bm;
PORTB.PIN0CTRL  =  PORT_PULLUPEN_bm; // Simple write 2 clocks 2 words to write, plus a single 1 for the whole group to load the value to a working register.
PORTB.PIN7CTRL  =  PORT_PULLUPEN_bm;
PORTC.PIN0CTRL &= ~PORT_PULLUPEN_bm; // 6 clocks again!
PORTC.PIN1CTRL &= ~PORT_PULLUPEN_bm;
PORTC.PIN2CTRL &= ~PORT_PULLUPEN_bm;
PORTC.PIN3CTRL &= ~PORT_PULLUPEN_bm;
PORTC.PIN4CTRL &= ~PORT_PULLUPEN_bm;
PORTC.PIN5CTRL &= ~PORT_PULLUPEN_bm;
PORTC.PIN6CTRL &= ~PORT_PULLUPEN_bm;
PORTC.PIN7CTRL &= ~PORT_PULLUPEN_bm;
PORTF.PIN4CTRL  =  PORT_PULLUPEN_bm | PORT_INVEN_bm;
PORTF.PIN5CTRL  =  PORT_PULLUPEN_bm | PORT_INVEN_bm;
PORTF.PIN6CTRL  =  PORT_PULLUPEN_bm | PORT_INVEN_bm;
PORTF.PIN7CTRL  =  PORT_PULLUPEN_bm | PORT_INVEN_bm;
PORTG.PIN0CTRL  =  PORT_PULLUPEN_bm | PORT_INVEN_bm;
PORTG.PIN1CTRL  =  PORT_PULLUPEN_bm | PORT_INVEN_bm;
PORTG.PIN2CTRL  =  PORT_PULLUPEN_bm | PORT_INVEN_bm;
PORTG.PIN3CTRL  =  PORT_PULLUPEN_bm | PORT_INVEN_bm;

/* The above contrived examples are 7 x 3 - 21 clocks and 21 words for the multipin-configuration method.
 * versus 4*6 + 2*2+1 + 8*6 + 2*8+1 = 94 clocks and 82 words of flash for the alternative.
 * The contrast is even more stark for somee realistic scenarios on high pincount devices.
 */

```
PORTA.PINCONFIG =
All bits make sense with PINCTRLUPD - some more than others - as long as you don't forget that it will replace whatever is there currently.
The lower three bits, controlling whether the pin is acting as an interrupt, will have surprising and likely unwelcome effects with  `PORTx.PINCTRLSET` and  `PORTx.PINCTRLCLR`. For PINCTRLSET they should never be 1 (any of them). For PINCTRLCLR, all should be 0 or all should be 1 ( 0b00000111 would turn off pin interrupts on the pins it's applied to without otherwise changing PINnCTRL register contents). Otherwise, the concept of flipping individual bits makes little sense for that last bitfield - the bits individually do not have a separate function, there is just the function of those three bits as a group. When changing the input sense configuration on large numbers of pins, use PINCTRLUPD instead.

This system is most useful at startup for devices running on battery to meet the datasheet requirement that the pins not be allowed to float if their digital input buffers are enabled.

```c++
// Practical example
/* Say we are using a Dx48, but only have any digital I/O on PORTA (maybe we are only using the DB48 because we need the three configurable opamps, and nothing else on other ports
 * but we need to be able to go into power down sleep mode to minimize power consumption, meaning unused pins must have pin buffer disabled */

PORTB.PINCONFIG = PORT_ISC_DISABLED_gc; //doesn't matter which port, shared between all ports.
PORTB.PINCTRLUPD = 0x3F; //disable input buffer on all pins of portB
PORTF.PINCTRLUPD = 0x3F; // disable input buffer on all pins of PORTF
PORTD.PINCTRLUPD = 0xFF; // disable input buffer on all pins of PORTD
PORTC.PINCTRLUPD = 0xFF; // disable input buffer on all pins of PORTC
PORTE.PINCTRLUPD = 0x0F; // disable input buffer on all pins of PORTE
// this is 1+2, 1+2+2, 1+2+2, 1+2 = 16 words, and executes in that many clocks. Imagine how much longer it would take to loop over 32 pins setting PINnCTRL for each (minimum words and more than that many clocks) plus the machinery for picking the port, likely hundreds of bytes total.

```

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

These parts (like many electronics) are better at driving pins low than high. With slew rate limiting enabled, the pins take about twice as long to transition. (all values are from the typ. column - neither min nor max are specified; one gets the impression that this is not tightly controlled):

| Direction   | Normal | Slew Rate Limited |
|-------------|--------|-------------------|
| Rising      |  22 ns |             45 ns |
| Falling     |  16 ns |             30 ns |

With a 24 MHz system clock, that means "normal" would be just over half a clock cycle while rising, and just under half a clock cycle falling; with slew rate limiting, it would be just over a clock cycle rising, and 3/4ths of a clock cycle on the falling side.

## There is no INLVL or PINCONFIG on tinyAVR devices
The configurable input level option (`INLVL`) is only available on parts with MVIO (AVR DB and DD-series).

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
These take a pin number (which can be represented as a uint8_t - though it is unfortunately common practice to use 16-bit signed integers to store pin numbers in Arduino-land). If the pin passed is valid, these will return either the bit position (PIN_PA0 would return 0, PIN_PD4 will return 4 and so on), the bit mask (bit mask = 1 << (bit position), so PIN_PA0 would return 0x01, PIN_PD4 would return 0x10, etc) `*`, or the port number (PORTA = 0, PORTB = 1, etc).

In the event that the pin passed to it does NOT correspond to a valid pin, the following behavior should be expected:

* If the pin does not exist, with the exception of digitalPinToPort(), these will all always return NOT_A_PIN, a numeric constant with a value of 255 (or -1; 255 and -1 are equivalent when assigned to an 8-bit type).
* digitalPinToPort has special behavior for one specific sort of situation: If the pin would have bit position 0, and that pin's number was skipped in the Arduino pin numbering, and other pins (which are neither RESET nor the UPDI pin) on that port do exist on this part (this is the case with PD0 on 28 and 32-pin DB-series and all DD-series parts, as well as PIN_PC0 for 14 and 20-pin DD-series), digitalPinToPort() will return the port number. Otherwise, digitalPinToPort() will return NOT_A_PORT if the pin is invalid. (also has a value of 255/-1 - the two are used just to make intent clearer and improve code readability).
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
On many cores, there is a `digitalPinToTimer(pin)` macro, mostly for internal use, which returns the timer associated with the pin. The Arduino API implicitly assumes that this is constant, and both core libraries and code in the wild often assume that it is. The digitalPinToTimer() implementation is as a macro, which frequently is optimized to a constant (provided the argument is constant, of course). The assumption that there exists not-more-than-1-to-1 timer:pin mapping was never universally valid even among AVRs (there are examples of classic AVRs that have multiple timers usable on some pins, and some even permit that to be remapped easily at runtime (the classic ATtiny841 being a notable example). All modern AVRs have at least two pins available to each timer output compare channel; and while the tinyAVRs never have more than two timers potentially usable with a given pin, the Dx-series allows one of up to 7 pins to be associated with a given timer (in groups), with some pins having as many as 4 options.

Furthermore, the following situations should not be expected to produce the desired result:
* Calling turnOffPWM, digitalPinToTimer, digitalPinToTimerNow or analogWrite on a pin nominally driven by a timer which the core has been instructed not to reconfigure using the takeOverTCxn() function. The core will treat the pin as an ordinary digital pin - by calling that, you assume full responsibility for all management of that timer. That function must be called if any manual configuration of a PWM timer will be or has been performed, except as specifically noted in in the timer and PWM reference or type D timer reference.
* Calling turnOffPWM, digitalPinToTimer, digitalPinToTimerNow, analogWrite or digitalWrite on a pin nominally driven by a type A timer which has been manually reconfigured, particularly if it has been configured in*single mode* for 3x16-bit PWM channels. The Arduino API does not support PWM with > 8 bits of resolution. Users who require that level of control must call takeOverTCxn(). See the timer and PWM reference.
* Calling turnOffPWM, digitalPinToTimer, digitalPinToTimerNow, analogWrite or digitalWrite on a pin nominally driven by a type B timer, but which has been configured to use an output pin other than the one shown on the DxCore pin mapping for that part - this will reconfigure PWM on the wrong pin if at all.
* Manually configuring any timer without calling takeOverTCxn, except as explicitly documented in the core documentation (be sure you are reading the correct core's documentation - usually mTC = DxC, but this is one case where that assumption is *not* valid. DxC permits more manipulation of TCD0 in particular.

## Note on number of pins and future parts
The most any announced AVR has had is 86 digital pins, the ATmega2560; In the modern AVR era, the digital pin maximum is 55, and the maximum possible without rearchitecting how pins are accessed is 56 (ie, if UPDI could be turned into I/O on DA/DB we are already there). There is no way for an AVR that allows the same bit-level access that we enjoy, on all pins, to have more than 56 pins within the AVR instruction set. 88 digital pins could fit in 11 ports, which would likely be PORTA, B, C, D, E, F, G, H, J, K, and L (They in the past have skipped PORTI, ex on 2560, probablty because I and 1 look so similar). Each port takes up 4 of the 32 addresses in the low I/O space for the VPORT registers, on which the single cycle bit-level access relies, and 4 are used for GPIORs, so we would need 16 more addresses in the low I/O space to acccomodate 4 more ports. Only time will tell how this is handled. Possibilities include:
* **Extend CBI/SBI with a new instruction variant** - Hahahaha! Right! That'll be the day. Very unlikely. However, it is *possible* in theory: Each additional register requires 16 opcodes (1 each for SBI and CBI, times 8 bits), so each port takes only 64 opcodes so 256 opcodes would be sufficient to give working VPORTs to up to 88 pins, and there are like 1500 opcodes not used by any other instructions, some of those in nice clean blocks that would work well here.
* **No VPORTH or higher** - This is clean, simple, and limiting in some ways, but probably the best route. If you needed to use VPORT access, you'll just need to use the lower 7 ports for it. Surely you don't need 57+ pins all with VPORT atomic single cycle access! This is also, in my judgement, most likely based on the fact that on the ATmega2560, the only precedent we have for running out of low I/O registers for pins, they didn't even put ports H through L into the high I/O space (heck, not only that, they did not, as far as I can see, so much as mention the pin inequality in the datasheet!).
* **VPORTs in High I/O** - There are many ways of dealing with it using a new set of differently behaving registers in the high I/O space. None of them retain the arbitrary compile-time-known-bit-write-is-one-clock of current VPORTs. At 8 registers per port, you could make a series of N operations on a bit or number of bits take N+1 clocks, and keep them atomic. This would mean a VPORT.DIR/DIRCLR/DIRSET, and VPORT.OUT/OUTSET/OUTCLR and VPORT.IN(writing to IN toggles, and there was never a single cycle DIR toggle). and VPORT.INTFLAGS. That's viable, though with the other registers in that, you could get at most only 3 more ports - which is only 24 pins: You couldn't quite do it for a 100 pin chip). Other solutions would lower capability and comprehensibility in exchange for squeezing in more pins. No matter how you do it, it doesn't really solve the problem.
* **VPORTs that can be remapped at runtime** - The stuff of library author's nightmares. You'd have some subset of the VPORTs (or maybe all of them) that could be remapped. If you put the register for that in the high I/O space it could be written quickly. But this will cause unaware old code to break by writing to the wrong pins if it didn't know to check for it, and any code that used them (ie, code that is supposed to be fast) might have to check the mapping first. This is most like what was done on the XMegas, but also serves as a shining example of the over-complexity that I think doomed those parts to poor uptake and slow death. I think this method is pointless - it is very difficult to envision a scenario where a remappable VPORT would be faster if code had to either set or test a value before accessing it.

`*` - Note on conventions for specifying numbers: 0x## refers to a hexadecimal number; this will always be written with a power of 2 number of hex digits, padded with a leading zero nybble if needed. A number written without a 0x prefix, ie, `12` refers to a decimal value. 0b######## refers to a value given as binary. For hexadecimal values, if the size of the datatype is unambiguosly known, we will typically represent them with an appropriate number of leading 0's - so 1 in a 1-byte datatype is written as 0x01, while 1 in a 16-bit datatype is written as 0x0001. Any time a number is not prefixed by 0x or 0b, the decimal form should be assumed. Which representation of a given value is chosen is based on the context of that value. Values that simply represent numbers are generally given in decimal. Values that are being subjected to bitwise operators, or that are bit masks, group codes, and similar, will be shown in hexadecimal, or when the bit math is particularly obtuse but the significance of the data is by bitfield, binary. In C, prefixing a number with 0 in an integer literal - to the surprise of many - will result in it being treated as octal (base 8), which probably made sense at the time for some wierd reason

`*` - Note on conventions for specifying numbers: 0x## refers to a hexadecimal number, while ## refers to a decimal digit and 0b######## refers to a value given as binary. For hexadecimal values, if the size of the datatype is unambiguosly known, we will typically represent them with an appropriate number of leading 0's - so 1 in a 1-byte datatype is written as 0x01, while 1 in a 16-bit datatype is written as 0x0001. Any time a number is not prefixed by 0x or 0b, the decimal form should be assumed. Which representation of a given value is chosen is based on the context of that value. Values that simply represent numbers are generally given in decimal. Values that are being subjected to bitwise operators, or that are bit masks, group codes, and similar, will be shown in hexadecimal.

All of the cases I'm aware of where interference from a device that wouldn't pass FCC testing actually caused concern were from devices intentionally designed do disrupt other devices (usually "GPS jammers" being used by truck drivers to cheat their employer or skirt safety rules - usually both). That is, they are devices that not only would fail testing, but which are explicitly illegal to an extent that they're not sold openly: Marketplace sites will take down listings for them. They are illegal devices being used to abet criminal acts which expose others risk of financial loss, injury, or death (from a car crash caused by avoiding safety regulations) whether or not a GPS jammer is used. The GPS jammers tend to have the opposite effect of what the criminals desire - rather than letting their truck go unnoticed, the GPS disruptions impact something critical, attracting attention as well as acting as a literal radio beacon that the authorities charged with maintaining the integrity of GPS can use to easily track, identify, and apprehend the user. These don't seem to be much of a problem either (except for their operator), since they aren't sold in huge volumes, and they make it easy to their operator to get busted.

I am aware of assorted cases where devices that the FCC would call "unintentional radiators" - usually poorly designed switch-mode power supplies in unshielded enclosures - were noticed by HAM Radio operators as producing unacceptable interference, but these often turned out to be located only a very short distance from the receiver, most often owned by the HAM who noticed it. That individual may well been the direct cause of the interference, - he may have built the device himself using unshielded switching power converter modules (or worse, designing his own), or removed the cover (hence shielding) from a commercial unit for some reason.
