# Comparator
A library for interfacing with the analog comparator peripheral in the modern AVR parts - tinyAVR 0/1/2-series, megaAVR 0-series, and the latest iterations, the DA, DB, DD, and EA-series.
Developed by [MCUdude](https://github.com/MCUdude/) with some porting effort by [Spence Konde](https://github.com/SpenceKonde/). This is the readme distributed with DxCore.

The tinyAVR 0-series, 2-series, and 1-series parts with less than 16k of flash have a single analog comparator, with either 1 or 2 options each for the positive and negative input pins. The "golden" 1-series parts, those with 16k or 32k flash (and other goodies) instead have three comparators, which have up to 4 options for the positive input, and 2 for the negative. The internal voltage reference can also be used as the negative side; on the 1-series and 2-series parts, that reference can be scaled by an internal 8-bit DAC, and on 0-series and 1-series parts, the reference itself can be used directly. This library provides a wrapper class, `Comparator` that exposes the full functionality of these peripherals without having to manually manipulate registers.

## Basics
An analog comparator, as the name implies, compares two analog voltages. If the "positive" input is higher than the "negative" one, the output is 1/HIGH, otherwise it is low. These can be configured to invert their value, and the output pins themselves can be inverted if desired (this library does not do that for you though - you want the builtin pinConfigure() for that)

Use the predefined object(s) `Comparator` or `Comparator0` and for parts that have them, `Comparator1` or `Comparator2`. (`Comparator` is #defined as `Comparator0` for code compatibility between single and multiple comparator parts. **Only the ATtiny3216, 3217, 1614, 1616, and 1617 have three comparators - They use the 1+series columns below**. All other parts including smaller 1-series use the 0/1-series column

## Pin Assignments
These options are radically different between the "lesser" 1-series parts - those with 2k, 4k, or 8k of flash, and the "golden" 1-series or 1+series parts, those with 16k or more. The parts where there is one positive pin that can be used by multiple parts are probably meant for use with the optional windowed mode which this library does not support. Note also that for AC0, the same pins (where supported) are used across the whole product line - though "golden" 1-series and 2-series parts have some extra options.

|  PIN  |  8-pin  |0/1-series AC0|2-series AC0|1+series AC0|1+series AC1|1+series AC2|
|-------|---------|--------------|------------|------------|------------|------------|
|IN P0  | PIN_PA7 |     PIN_PA7  |   PIN_PA7  |   PIN_PA7  |   PIN_PA7  |   PIN_PA6  |
|IN P1  |   n/a   |     PIN_PB5* |   PIN_PB5* |   PIN_PB5* |   PIN_PB6* |   PIN_PB0  |
|IN P2  |   n/a   |       n/a    |   PIN_PB1  |   PIN_PB1  |   PIN_PB0  |   PIN_PB5* |
|IN P3  |   n/a   |       n/a    |   PIN_PB6* |   PIN_PB6* |   PIN_PB4* |   PIN_PB7* |
|IN N0  | PIN_PA6 |     PIN_PA6  |   PIN_PA6  |   PIN_PA6  |   PIN_PA5  |   PIN_PA7  |
|IN N1  |   n/a   |     PIN_PB4* |   PIN_PB4* |   PIN_PB4* |   PIN_PB7* |   PIN_PB6* |
|IN N2  |   n/a   |       n/a    |   PIN_PB0  |     n/a    |     n/a    |     n/a    |
|OUT    | PIN_PA3 |     PIN_PA5  |   PIN_PA5  |   PIN_PA5  |   PIN_PB3  |   PIN_PB2  |

`*` indicates that this pin is not present on all pincounts. PB4 and PB5 are only found in 20 and 24 pin parts, and PB6 and PB7 are exclusive to the 24-pin parts. These are only valid inputs for parts on which they exist

## Things to be aware of
There are several things that may be surprising about this peripheral and the wrapper this class provides.

### Regarding input pins
In accordance with the recommendations of Microchip from the datasheet, we disable the digital input for all pins used by the analog comparator through the PINnCTRL register; this is done when `init()` is called. digitalRead() will always return `LOW` on these pins. When the `stop()` method is called, these pins will remain off until manually reconfigured, unless told to restore them via `stop(true)`. You can call `stop(true)` even if the comparator is not currently enabled, but has been initialized. This may be useful if you have been using it with one set of pins but now want to stop using those pins and switch to different pins. (No, no I don't think this is going to be a particularly common use case. What kind of device is this with a pin that that switches between being an analog voltage that has a magic level other than the voltage at which it transitions between high and low, and would ever make sense to hook up to an analog comparator

When the PINnCTRL register is modified by the class at any point, any other configuration (input level on DB/DD, inversion, and internal pullup) will be returned to the default values. You likely don't want any of those options while using the analog comparator anyway. The pullup will throw off the reading (and is not of a tightly controlled strength, so you can't use it as part of a resistor divider), and as these pins will have analog voltages likely between the input high and low thresholds applied to them, leaving the digital input enabled will increase power consumption.

Comparator input pins should not be set as OUTPUT except in truly unusual cases. The comparator cannot be used on MVIO pins, and otherwise, voltage on any pin must not exceed the supply voltage or be lower than ground by more than half a volt, and the common mode voltage range is even narrower, at -0.2V - Vcc - and used as an output, the pin will either output 0V or Vdd, so the comparator will always have one value unless either the pin is heavily (possibly excessively) loaded, or an inappropriate external voltage is being applied to the pins. The only plausible use case I can come up with is monitoring an output pin for a possible short circuit or excessive load. Note that this library assumes that you have not set the pins to be outputs (the comparator library, internally, only knows the PINnCTRL register for the pins - it doesn't know anything else about them, and adding that functionality would add rarely used bloat) - if you have, be sure to set them back to inputs.

### No settings are applied until init() is called
Like the other basic wrappers around modern avr peripherals (logic, ZCD, event), until the `init()` method is called, none of the requested settings have been written to the peripheral. This is correct and intended behavior, and allows modifying multiple options as close to simultaneously as possible. Nothing prevents you from calling `init()` on an enabled comparator if need be.

### The POWER and LPMODE options are not supported currently
On Dx-series parts, each analog comparator can be configured for one of three power profiles. The higher the number, the lower the power consumption and the slower the response. Currently the Comparator library always uses the highest power, fastest response option. This may be changed in a future release if there is call for such a feature.

Early versions of the header specified a fourth value, but this was removed from both the datasheet and the io header before release. It may or may not secretly still be present (my guess would be that it is, just like the 4x PLL multiplier), but there was likely a reason that it was struck from the documentation (likely, that it didn't work, was inaccurate, or didn't save power). It would appear that by the time the DD was released, they'd gotten the issue sorted out, because power profile 3 is present there.

On the tinyAVR and megaAVR parts, there is instead an LPMODE (Low Power Mode) which can be either on or off. Comparator always sets it to 0. An option to configure this may be made available in the future if there is user demand.

### Window Mode is not supported
Thus far all modern AVR parts with more than 1 comparator have had a "windowed mode" that can be selected to group 2 comparators into a single "windowed" comparator, where both comparators must use the same positive input, while the negative inputs define the upper and lower bounds of the "window", and interrupts can be generated when the state rises above, or falls below the input, or when it enters or leaves the window.

The Comparator library does not support this odd option. There are no plans to add support for this feature - it's an awkward amount of synchronization for this library to provide. Note also that this is entirely separate from the ADC "window comparator" mode, where a similar effect is achieved with the ADC set in free-running mode!

## Properties of the Comparator class

### input_p
This property selects the positive input of the comparator.
Accepted values:
``` c++
comparator::in_p::in0; // Use positive input pin 0 as input
comparator::in_p::in1; // Use positive input pin 1 as input (if present)
comparator::in_p::in2; // Use positive input pin 2 as input (if present)
comparator::in_p::in3; // Use positive input pin 3 as input (if present)
comparator::in_p::in4; // Use positive input pin 4 as input (if present, DD and later omly)
comparator::in_p::in5; // Use positive input pin 5 as input (if present, EB and later only)
comparator::in_p::in6; // Use positive input pin 6 as input (if present, EB and later only)
```

#### Usage
``` c++
Comparator.input_p = comparator::in_p::in0;  // Connect positive input pin 0 to the positive input of the comparator
```

#### Default state
`Comparator.input_p` defaults to `comparator::in_p::in0` if not specified in the user program.


### input_n
This property selects the negative input of the comparator.
Accepted values:
``` c++
comparator::in_n::in0;    // Use negative input pin 0 as input
comparator::in_n::in1;    // Use negative input pin 1 as input (if present)
comparator::in_n::in2;    // Use negative input pin 2 as input (if present, 2-series and Dx/Ex only)
comparator::in_n::in3;    // Use negative input pin 3 as input (DD and later only)
comparator::in_n::vref;   // Use the voltage reference w/out DAC (0/1-series only.
comparator::in_n::dacref; // Use DACREF as input,
```
#### What is the DACREF?
On the tinyAVR 1-series, a new feature was added to the ACs: The parts had an 8-bit DAC, so they tied it to the AC's negative mux, an obvious choice. When they added the extra AC's on the 16k and 32k parts, they wanted to have the sme functionality on the other AC's (as it was very well suited to things like windowed mode, which was now posible), so they added two more DAC instances which didn't have an output buffer, and were used only for the AC's. That seemed to be a great combination, and was adopted from the mega0-series onwards built into the AC and called the DACREF. At this point, it replaced the previous VREF option entirely, and when the DAC returned as a peripheral in the Dx-series, it was independent of the ACs. In fact, it isn't even routed to the AC's; the DAC there gets direct internal connections to the ADC and the OPAMPS but nt the ACs.

#### Usage
``` c++
Comparator.input_n = comparator::in_n::vref;  // Connect voltage reference to the negative input of the comparator
```

#### Default state
`Comparator.input_n` defaults to `comparator::in_n::in0` if not specified in the user program.


### reference
On the 0-series and 1-series, this sets the voltage reference that will be used if VREF is selected as the negative input. On non-0-series, this also sets the voltage that DACREF is derived from.

On 1-series parts with multiple comparators, these reference voltages can be set independently for each comparator. On the Dx and Ex parts, parts with multiple comparators have to share just one reference voltage.. On the tinyAVR 1-series, AC0 and the DAC that can be output on PA6 share the same reference. The 0/1-series uses a rather strange set of voltages, while the 2-series uses the same voltages that most modern AVRs do.

Accepted values (0/1-series):
``` c++
comparator::ref::disable;    // Do not use any reference
comparator::ref::vref_0v55;   // 0.55V internal reference
comparator::ref::vref_1v1;    // 1.1V internal reference
comparator::ref::vref_1v5;    // 1.5V internal reference
comparator::ref::vref_2v5;    // 2.5V internal reference
comparator::ref::vref_4v34;   // 4.34V internal reference
comparator::ref::vref_2v500;  // 2.5V internal reference (compatibility)
```

Accepted values (Everything else):
``` c++
comparator::ref::disable;    // Do not use any reference
comparator::ref::vref_1v024; // 1.02V internal reference
comparator::ref::vref_2v048; // 2.05V internal reference
comparator::ref::vref_2v500; // 2.5V internal reference
comparator::ref::vref_4v096; // 4.1V internal reference
comparator::ref::vref_vdd;   // VDD as reference
```

#### Usage
``` c++
Comparator.reference = comparator::ref::vref_4v096;  // Use the internal 4.096V reference for the DACREF
```

#### Default state
`Comparator.reference` defaults to `comparator::ref::disable` if not specified in the user program.

### dacref
This property configures the DACREF value - this voltage can be selected as the input for the negative side of the comparator. This is essentially an 8-bit DAC that can only be used as the AC negative input. It is calculated as:

<img src="http://latex.codecogs.com/svg.latex?V_{DACREF} = \frac{Comparator.dacref}{256} * Comparator.reference" border="0"/>

Or, in words, the voltage from ACn.DACREF is that many 256th's of the reference voltage
*(it does not appear to be `ACn.DACREF + 1` 256'ths interestingly enough, which is what one would naively expect, and would be strictly speaking slightly better) -SK*

#### Usage
``` c++
Comparator.dacref = 128;  // Divide the reference voltage by two
```

#### Default state
`Comparator.dacref` defaults to `255` if not specified in the user program.


### hysteresis
This variable configures the comparator hysteresis (the difference between when state transitions from 0 to 1, and when it transitions from 1 to 0. Useful for eliminating undesirable stochastic or oscillatory behavior which might be seen when the two inputs are almost at the same voltage.

Accepted values:
``` c++
comparator::hyst::disable; // No hysteresis
comparator::hyst::small;   // 10mV hysteresis (±5mV)
comparator::hyst::medium;  // 25mV hysteresis (±12.5mV)
comparator::hyst::large;   // 50mV hysteresis (±25mV)
```

#### Usage
``` c++
Comparator.hysteresis = comparator::hyst::large;  // Use 50mV hysteresis
```

#### Default state
`Comparator.hysteresis` defaults to `comparator::hyst::disable` if not specified in the user program.


### output
Property to configure the comparator output - either internal or external, inverted or not (note also that the pin itself could be inverted with the INVEN bit of PORTA.PIN7CTRL (or PORTC.PIN6CTRL if alternate output pin is used); this allows the pin to take a logical state opposite from the internal (event channel) output, thus allowing every possible combination of external and internal output polarities. When the positive input is a higher voltage than the negative one, the output will be 1 if not inverted, or 0 if inverted.

Accepted values:
``` c++
comparator::out::disable;        // No output pin, signal not inverted internally
comparator::out::disable_invert; // No output pin, signal inverted internally
comparator::out::enable;         // Enable output pin, signal not inverted internally
comparator::out::invert;         // Enable output pin, signal inverted internally
comparator::out::enable_invert;  // Identical to comparator::out::invert
```

#### Usage
``` c++
Comparator.output = comparator::out::enable; // Enable output pin (PA7), which will be HIGH when the positive input is higher than the negative one.
```

``` c++
Comparator.output = comparator::out::enable_invert; // Enable inverted output on pin (PA5)
Comparator.init();                      // Must be done before writing to the PINnCTRL
PORTA.PIN5CTRL = PORT_INVEN_bm;         // Invert PA5
// Now, PIN_PA5 will provide non-inverted output, while Comparator.read() and the event outputs provides inverted output.
```

|     | AC0 | AC1 | AC2 | AC0 on 8-pin parts |
|-----|-----|-----|-----|--------------------|
| PIN | PA5 | PB3 | PB2 |         PA3        |


#### Default state
`Comparator.output` defaults to `comparator::out::disable` if not specified in the user program.


#### Usage
```c++
Comparator.output_swap = comparator::out::no_swap; // No pin swap for output
```

#### Default state
`Comparator.output_swap` defaults to `comparator::out::no_swap` if not specified in the user program.


### output_initval
When the comparator is initialized, the pin is set to this state until the comparator has output available. To prevent a glitch during initialization, set this to what you expect it to initially output.
Accepted values:
```c++
comparator::out::init_low;  // Output pin low after initialization
comparator::out::init_high; // Output pin high after initialization
```

#### Usage
```c++
Comparator.output_initval = comparator::out::init_high;
```

#### Default state
`Comparator.output_initval` defaults to `comparator::out::init_low` if not specified in the user program.

### Which options are available where

|                    Option | DA/DB   |  DD  |  EA | Tiny0 | Tiny1 | Tiny1+ | Tiny2 | Mega0 |
|---------------------------|---------|------|-----|-------|-------|--------|-------|-------|
| comparator::in_p::in0     |     All | 28p+ | All |   All |   All |    All |   All |   All |
| comparator::in_p::in1     |       * |  All |   * |  Some |  20p+ | 14/20p |  20p+ |   All |
| comparator::in_p::in2     |       * |  All |   * |    No |    No |    All |   All |   All |
| comparator::in_p::in3     |     All |  All | All |    No |    No |   20p+ |   24p |   All |
| comparator::in_p::in4     |      No |   ** | All |    No |    No |     No |    No |    No |
| comparator::in_n::in0     |     All |  All | All |   All |   All |    All |   All |   All |
| comparator::in_n::in1     |       * |   No | All |  20p+ |  20p+ | 14/20p |  20p+ |   All |
| comparator::in_n::in2     |       * |   No | All |    No |    No |   14p+ |   All |   All |
| comparator::in_n::in3     |      No |   ** | All |    No |    No | 20/24p |  20p+ |    No |
| comparator::in_n::vref    |      No |   No |  No |   All |   All |    All |    No |    No |
| comparator::in_n::dacref  |     All |  All | All |    No |   All |    All |   All |   All |
| comparator::out::pin_swap |    48p+ |   No |48p+ |    No |    No |     No |    No |    No |


`*` This is messy for the DA/DB, as they have three comparators, and 1 pin for ALL comparators is for 1 comparator are unavailable unless the part has 48+ pins

|                    Option | DA < 48p | DB < 48p | EA < 48p | All 48p+ | Prediction (by SK)    |
|---------------------------|----------|----------|----------|----------------------------------|
| comparator::in_p::in1     | AC1, AC2 | AC1, AC2 |      AC1 |          |                  48p+ |
| comparator::in_p::in2     |      AC1 |      AC1 |      AC1 |      All |                  48p+ |
| comparator::in_n::in1     |      All |       No |      All |      All | MVIO-off or 48p+ only |
| comparator::in_n::in2     | AC0, AC1 | AC0, AC1 |      All | AC0, AC1 |    Likely not for AC2 |

*The really mysterious one is negative input 2 for AC2. It is listed as being on the same physical pin, PD7. as negative input 0. This would imply that the two settings are identical and in practice AC2 never has an AINN2, as I read it. Anyone got info on any possible difference?*


`**` On the AVR DD, these are PC2 and PC3, some of the few pins found on all parts. However, they are only available for use as AC or ADC inputs if MVIO is disabled. Until the product brief of a product featuring both MVIO and the new ADC, we won't know if this is going to be an ongoing thing. My sense is it will reamain - I think the reason for it is that the ADC is powered from the AVDD domain, which is internally tied to VDD, and that sets the safe voltage on an analog input or analog comparator pin should not exceed AVDD by more than a fraction of a volt, and that would be clamped by the protection diode. If that's all it does, I'd expect it to be a "paper limitation" and that the silicon would do it just fine in MVIO mode as long as the voltages were below both VDD and VDDIO2 and otherwise just provide garbage results (GIGO). I'd have expect this in the form of an electrical characteristic, though - usually they don't like to put front and center the limitation on a peripheral. Perhapse the internal current path would have been more destructive than that, and after thorough study (perhaps helping to explain why it took 2 years from product brief to release). When the first new device with both MVIO and the good ACs gets it's product brief released, we'll know whether this will be permanent or not. My money is on "permanent". But who know when the Microchip Pre-Santa will bring us such a treat?

Where it is marked "`__p`, it is only available on the specified pincount and larger. `__p/__p` indicates that the options are available for some comparators at or above the first pincount, and all at or above the second. `*` indicates that it's a weird one, and see the note above Check the chart further up for more information. All means that **at least one comparator** on all pincounts has the specified input, **not that they all have it**. We do not guarantee that the library will reject attempts to specify invalid pins, and on parts with more than one comparator, it can only reject them if they are absent for ALL comparators (ex, AINN1 is always PD0 on Dx-series, and DB-series parts with 28 or 32 pins don't have that pin, so specifying `comparator::in_n::in0` will be rejected with a compile error. A tiny1614 has AINP1 for AC2 and AINP2 for AC0 and AC1. Using `comparator::in_p::in1` for AC0 or AC1 will not result in any compile error, nor will using `comparator::in_p::in2` with AC2). Use extra care on parts with multiple comparators.


## Comparator methods
Comparator provides a few ~methods~ member functions to provide th

### init()
The init() method applies all the settings you have specified by writing them to the peripheral registers. You must call this after specifying the desired options to write them to the peripheral registers.

#### Usage
```c++
Comparator.init(); // Initialize comparator
```

### start()
This method starts the analog comparator.
#### Usage
```c++
Comparator.start(); // Start comparator
```


### stop(bool restorePins = false)
This method stops the analog comparator. You may optionally pass a boolean value to this method. If it is `true`, digital input on the pins will be restored. Otherwise, it will not.

#### Usage
```c++
Comparator.stop(); // Stop comparator. Will not re-enable digital input, and digitalRead() of the input pins will return 0 regardless of the voltage on the pin
```

```c++
Comparator.stop(true); // Stop comparator. Digital input on the pins that this comparator was using will be re-enabled.
```

### read()
This method reads the current value of the comparator and returns it as a `bool`. It is effected by output invert configured for the analog comparator. If the output pin was inverted a second time (useful for particularly obscure use cases) via the `PINnCTRL` register, this value will not reflect it.

#### Usage
```c++

if (Comparator0.read()) {
  doSomething();
}
```


### attachInterrupt()
Method for enabling analog comparator interrupt. The interrupt will trigger when the the comparator output changes.
Valid arguments for the third parameters are `RISING`, `FALLING` and `CHANGE`. Clearing of the INTFLAG is handled by the library, and does not need to be done in your interrupt code. Note that, like any other time a function is called from an ISR, there is added overhead of about 15 clocks at the start of the ISR and twice as many at the end, at up to 56 bytes of flash are consumed for each comparator to provide the prologue and epilogue. A future version may provide the option to not define these ISRs, so that advanced users who need either the space or the speed can choose to define the ISRs manually,

#### Usage
```c++
Comparator.attachInterrupt(blinkLED, RISING); // Run the blinkLED function when the comparator output goes high

void blinkLED() {
  digitalWrite(myLedPin, CHANGE);
}
```


### detachInterrupt()
Method for disabling analog comparator interrupt.

#### Usage
```c++
Comparator.detachInterrupt(); // Disable interrupt
```

### getPeripheral()
This method simply returns a reference to the analog comparator struct which it is using. Since there is a fixed correspondence between the pre-defined objects and the peripheral struct, this is rarely necessary - in this example you could have just written to AC0.CTRLA - but what if you're writing a library where you ask the user to pass you a reference to the configured Comparator (ie, you're making a "friendly" library that's meant for people using this library, or because you're trying to support a variety of parts, and don't want to have to write 4 implementations just to configure the reference and turn on a comparator - like this library does. For such a simple peripheral, there sure have been a lot of changes since the first modern AVRs.), and within the library, you need to do something to it that requires the actual registers.

#### Usage
```c++
/* This only makes sense if you assume that this is executing in a context where
 * MyComparator was passed as a reference, so this code has no idea whether
 * it's Comparator0 or Comparator2 or Comparator6 on some future part with
 * way too many comparators */
MyComparator.init(); // initialize the comparator - let's assume it was configured a few lines before this example begins.
AC_t& AC_struct = MyComparator.getPeripheral(); // Grab the ACn struct - AC0, AC1, AC2, etc.
/* For tinyAVR and megaAVR the's LPMODE and or not-LPMODE*/
AC_struct.CTRLA |= AC_LPMODE_bm | AC_RUNSTBY_bm ; // Reduce power consumption at expense of response time and leave on in standby
/* for Dx there are 3 power profiles, this has slowest response time and lowerst power:
  AC_struct.CTRLA |= AC_PROFILE2_gc | AC_RUNSTBY_bm; // Reduce power consumption at expense of response time and leave on in standby
 * and Ex adds a third:
  AC_struct.CTRLA |= AC_PROFILE3_gc | AC_RUNSTBY_bm; // Reduce power consumption at expense of response time and leave on in standby
 * DA's initial headers listed profile 3. One imagines they found it wasn't up to internal standards, and airbrushed it out of the datasheet
 * like they did with the 4x clock multiplier. That option works (and the PLL can be run ~2.5x overclock at room temp o_o) so I'm cautiously
 * optimistic that this would work on a Dx except under adverse conditions/*
  AC_struct.CTRLA |= 0x18 | AC_RUNSTBY_bm; // CTRLA is 0bSO_PPHHE where
  S = RUNSTBY, O = OUTEN, P = POWER, H = Hysteresis, and E = Enable. POWER = 3 is marked as reserved on the Dx, but was PROFILE3 in preliminary headers.
 */
MyComparator.start();   // Enable the comparator
enterStandbySleep();  // enter standby sleep mode until the comparator interrupt fires, waking it up.
```

## *Future development*
*shouldn't LP_MODE/PROFILE and RUNSTBY be properties, and treated like everything else? Why **aren't** they? I would imagine that wanting to wake on the AC int would be one of the most common uses of that interrupt.*
*They certainly **want** to be properties and it would make the library more coherent. But it would come at a 4-8 bytes of flash (unsure if per comparator or total) and 1 or 2 bytes of ram per comparator, depending on implementation details. Probably wouldn't be popular with people on 212's, but that's a pretty small overhead considering the general level of bloat introduced by classy wrappers around peripherals like Logic, Comparator and Opamp). Maybe a new optional argument to start it in low power, low power - run standby, and run standby (correspondingly more options on DxCore of course). Because how often are you going to be changing the mode once you've turned it on? That's an odd use case (and in any case, the intuitive solution of calling start with a different argument to change it would behave as expected. By passing as the argument the value to be written to the CTRLA register it would have almost no overhead, too. -SK*
