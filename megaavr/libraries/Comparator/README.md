# Comparator
A library for interfacing with the analog comparator peripheral in the modern AVR parts - tinyAVR 0/1/2-series, megaAVR 0-series, and the latest iterations, the DA, DB, DD, and EA-series.
Developed by [MCUdude](https://github.com/MCUdude/) with some porting effort by [Spence Konde](https://github.com/SpenceKonde/). This is the readme distributed with megaTinyCore.

The tinyAVR 0-series, 2-series, and 1-series parts with less than 16k of flash have a single analog comparator, with either 1 or 2 options each for the positive and negative input pins. The "golden" 1-series parts, those with 16k or 32k flash (and other goodies) instead have three comparators, which have up to 4 options for the positive input, and 2 for the negative. The internal voltage reference can also be used as the negative side; on the 1-series and 2-series parts, that reference can be scaled by an internal 8-bit DAC, and on 0-series and 1-series parts, the reference itself can be used directly. This librarty provides a wrapper class, `Comparator` that exposes the full functionality of these peripherals without having to manually manipulate registers.

## Basics
An analog comparator, as the name implies, compares two analog voltages. If the "positive" input is higher than the "negative" one, the output is 1/HIGH, otherwise it is low. These can be configured to invert their value, and the output pins themselves can be inverted if desired (this library does not do that for you though - you want the builtin pinConfigure() for that)

Use the predefined object(s) `Comparator` or `Comparator0` and for parts that have them, `Comparator1` or `Comparator2`. (`Comparator` is #defined as `Comparator0` for code compatibility between single and multiple comparator parts. **Only the ATtiny3216, 3217, 1614, 1616, and 1617 have three comparators - They use the 1+ser columns below**. All other parts including smaller 1-series use the 0/1-ser column

## Pin Assignments
These options are radically different between the "lesser" 1-series parts - those with 2k, 4k, or 8k of flash, and the "golden" 1-series or 1+series parts, those with 16k or more. The parts where there is one positive pin that can be used by multiple parts are probably meant for use with the optional windowed mode which this library does not support. Note also that for AC0, the same pins (where supported) are used across the whole product line - though "golden" 1-series and 2-series parts have some extra options, which will likey carry over to the 3-series. Assuming they continue with tinyAVR as opposed to flushing it and replacing with low-pin-count parts in the flagship lines, like the DD-series which (if it ever comes out) will be available in a 14 or 20 pin package.

|  PIN  |  8-pin  |0/1-ser AC0| 2-ser AC0 | 1+ser AC0 | 1+ser AC1 | 1+ser AC2 |
|-------|---------|-----------|-----------|-----------|-----------|-----------|
|IN P0  | PIN_PA7 |  PIN_PA7  |  PIN_PA7  |  PIN_PA7  |  PIN_PA7  |  PIN_PA6  |
|IN P1  |   n/a   |  PIN_PB5* |  PIN_PB5* |  PIN_PB5* |  PIN_PB6* |  PIN_PB0  |
|IN P2  |   n/a   |    n/a    |  PIN_PB1  |  PIN_PB1  |  PIN_PB0  |  PIN_PB5* |
|IN P3  |   n/a   |    n/a    |  PIN_PB6* |  PIN_PB6* |  PIN_PB4* |  PIN_PB7* |
|IN N0  | PIN_PA6 |  PIN_PA6  |  PIN_PA6  |  PIN_PA6  |  PIN_PA5  |  PIN_PA7  |
|IN N1  |   n/a   |  PIN_PB4* |  PIN_PB4* |  PIN_PB4* |  PIN_PB7* |  PIN_PB6* |
|IN N2  |   n/a   |    n/a    |  PIN_PB0  |    n/a    |    n/a    |    n/a    |
|OUT    | PIN_PA3 |  PIN_PA5  |  PIN_PA5  |  PIN_PA5  |  PIN_PB3  |  PIN_PB2  |

`*` indicates that this pin is not present on all pincounts. PB4 and PB5 are only found in 20 and 24 pin parts, and PB6 and PB7 are exclusive to the 24-pin parts. These are only valid inputs for parts on which they exist

## Things to be aware of
There are several things that may be surprising about this peripheral and the wrapper this class provides.

### Regarding input pins
In accordance with the recomendations of Microchip from the datasheet, we disable the digital input for all pins used by the analog comparator through the PINnCTRL register; this is done when `init()` is called. digitalRead() will always return `LOW` on these pins. When the `stop()` method is called, these pins will remain off until manually reconfigured, unless told to restore them via `stop(true)`. You can call `stop(true)` even if the comparator is not currently enabled, but has been initialized. This may be useful if you have been using it with one set of pins.

When the PINnCTRL register is modified by the class at any point, any other configuration (input level on DB/DD, inversion, and internal pullup) will be returned to the default values. You likely don't want any of those options while using the analog comparator anyway. The pullup will throw off the reading (and is not of a tightly controlled strength, so you can't use it as part of a resistor divider), and as these pins will have analog voltages likely between the input high and low thresholds applied to them, leaving the digital input enabled will increase power consumption.

Comparator input pins should not be set as OUTPUT except in truly unusual cases. The comparator cannot be used on MVIO pins, and otherwise, voltage on any pin must not exceed the supply voltage or be lower than ground by more than half a volt, and the common mode voltage range is even narrower, at -0.2V - Vcc - and used as an output, the pin will either output 0V or Vdd, so the comparator will always have one value unless either the pin is heavilly (possibly excessively) loaded, or an inappropriate external voltage is being applied to the pins. The only plausible use case I can come up with is monitoring an output pin for a possible short circuit or excessive load. Note that this library assumes that you have not set the pins to be outputs (the comparator library, internally, only knows the PINnCTRL register for the pins - it doesn't know anything else about them, and adding that functionality would add rarely used bloat) - if you have, be sure to set them back to inputs.

### No settings are applied until init() is called
Like the other basic wrappers around modern avr peripherals (logic, ZCD, event), until the `init()` method is called, none of the requested settings have been written to the peripheral. This is correct and intended behavior, and allows modifying multiple options as close to simultaneously as possible. Nothing prevents you from calling `init()` on an enabled comparator if need be.

### The POWER and LPMODE options are not supported currently
On Dx-series parts, each analog comparator can be configured for one of three power profiles. The higher the number, the lower the power consumption and the slower the response. Currently the Comparator library always uses the highest power, fastest response option. On the tinyAVR and megaAVR parts, there is instead an LPMODE (Low Power Mode) which can be either on or off. Comparator always sets it to 0. An option to configure this may be made available in the future if there is user demand.

### Window Mode is not supported
Thus far all modern AVR parts with more than 1 comparator have had a "windowed mode" that can be selected to group 2 comparators into a single "windowed" comparator, where both comparators must use the same positive input, while the negative inputs define the upper and lower bounds of the "window", and interrupts can be generated when the state rises above, or falls below the input, or when it enters or leaves the window.

The Comparator library does not support this odd option. There are no plans to add support for this odd feature. Note also that this is entirely separate from the ADC "window comparator" mode, where a similar effect is achieved with the ADC set in free-running mode.

## Properties of the Comparator class

### input_p
This property selects the positive input of the comparator.
Accepted values:
``` c++
in_p::in0; // Use positive input pin 0 as input
in_p::in1; // Use positive input pin 1 as input (if present)
in_p::in2; // Use positive input pin 2 as input (if present)
in_p::in3; // Use positive input pin 3 as input (if present)
```

#### Usage
``` c++
Comparator.input_p = in_p::in0;  // Connect positive input pin 0 to the positive input of the comparator
```

#### Default state
`Comparator.input_p` defaults to `in_p::in0` if not specified in the user program.


### input_n
This property selects the negative input of the comparator.
Accepted values:
``` c++
in_n::in0;    // Use negative input pin 0 as input
in_n::in1;    // Use negative input pin 1 as input (if present)
in_n::in2;    // Use negative input pin 2 as input (2-series only)
in_n::vref;   // Use the voltage reference w/out DAC (0/1-series only)
in_n::dacref; // Use DACREF as input (1/2-series only)
```

#### Usage
``` c++
Comparator.input_n = in_n::vref;  // Connect voltage reference to the negative input of the comparator
```

#### Default state
`Comparator.input_n` defaults to `in_n::in0` if not specified in the user program.


### reference
On the 0-series and 1-series, this sets the voltage reference that will be used if VREF is selected as the negative input. On 1-series and 2-series, this also sets the voltage that DACREF is derived from. On 1-series parts with multiple comparators, these reference voltages can be set independently (unlike on the Dx-series). On the tinyAVR 1-series, AC0 and the DAC that can be output on PA6 share the same reference. The 0/1-series uses a rather strange set of voltages, while the 2-series uses the same voltages that most modern AVRs do

Accepted values (0/1-series):
``` c++
ref::disable;    // Do not use any reference
ref::vref_0v55;   // 0.55V internal reference
ref::vref_1v1;    // 1.1V internal reference
ref::vref_1v5;    // 1.5V internal reference
ref::vref_2v5;    // 2.5V internal reference
ref::vref_4v34;   // 4.34V internal referenc
ref::vref_2v500;  // 2.5V internal reference (compatibility)
```

Accepted values (2-series):
``` c++
ref::disable;    // Do not use any reference
ref::vref_1v024; // 1.02V internal reference
ref::vref_2v048; // 2.05V internal reference
ref::vref_2v500; // 2.5V internal reference
ref::vref_4v096; // 4.1V internal reference
ref::vref_vdd;   // VDD as reference
```

#### Usage
``` c++
Comparator.reference = ref::vref_4v096;  // Use the internal 4.096V reference for the DACREF
```

#### Default state
`Comparator.reference` defaults to `ref::disable` if not specified in the user program.

### dacref
This property configures the DACREF value - this voltage can be selected as the input for the negative side of the comparator. This is essentially an 8-bit DAC that can only be used as the AC negative input. It is calculated as:

<img src="http://latex.codecogs.com/svg.latex?V_{DACREF} = \frac{Comparator.dacref}{256} * Comparator.reference" border="0"/>

Or, in words, the the voltage from ACn.DACREF is dacref 256th's of the reference voltage.

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
hyst::disable; // No hysteresis
hyst::small;   // 10mV hysteresis (±5mV)
hyst::medium;  // 25mV hysteresis (±12.5mV)
hyst::large;   // 50mV hysteresis (±25mV)
```

#### Usage
``` c++
Comparator.hysteresis = hyst::large;  // Use 50mV hysteresis
```

#### Default state
`Comparator.hysteresis` defaults to `hyst::disable` if not specified in the user program.


### output
Property to configure the comparator output - either internal or external, inverted or not (note also that the pin itself could be inverted with the INVEN bit of PORTA.PIN7CTRL (or PORTC.PIN6CTRL if alternate output pin is used); this allows the pin to take a logical state opposite from the internal (event channel) output, thus allowing every possible combination of external and internal output polarities. When the positive input is a higher voltage than the negative one, the output will be 1 if not inverted, or 0 if inverted.

Accepted values:
``` c++
out::disable;        // No output pin, signal not inverted internally
out::disable_invert; // No output pin, signal inverted internally
out::enable;         // Enable output pin, signal not inverted internally
out::invert;         // Enable output pin, signal inverted internally
out::enable_invert;  // Identical to out::invert
```

#### Usage
``` c++
Comparator.output = out::enable; // Enable output pin (PA7), which will be HIGH when the positive input is higher than the negative one.
```

``` c++
Comparator.output = out::enable_invert; // Enable inverted output on pin (PA5)
Comparator.init();                      // Must be done before writing to the PINnCTRL
PORTA.PIN5CTRL = PORT_INVEN_bm;         // Invert PA5
// Now, PIN_PA5 will provide non-inverted output, while Comparator.read() and the event outputs provides inverted output.
```

```
| AC# | AC0 | AC1 | AC2 | AC0 on 8-pin parts |
|-----|-----|-----|-----|--------------------|
| PIN | PA5 | PB3 | PB2 |         PA3        |
```

#### Usage
``` c++
Comparator.output = out::enable; // Enable output comparator's output pin. Comparator is AC0, so this is PA5.
```

#### Default state
`Comparator.output` defaults to `out::disable` if not specified in the user program.

### output_swap
Variable for pin swapping the physical output pin to its alternative position, if available. There is only an alternate pin available to Dx/Ex-series on parts with at least 48 pins and is not available on megaAVR 0-series, or any tinyAVR parts.
Accepted values:
```c++
out::no_swap;  // Use default pin position
out::pin_swap; // Use alternative position (48 and 64-pin parts only)
```

#### Usage
```c++
Comparator.output_swap = out::no_swap; // No pin swap for output
```

#### Default state
`Comparator.output_swap` defaults to `out::no_swap` if not specified in the user program.


### output_initval
When the comparator is initialized, the pin is set to this state until the comparator has output available. To prevent a glitch during initialization, set this to what you expect it to initially output.
Accepted values:
```c++
out::init_low;  // Output pin low after initialization
out::init_high; // Output pin high after initialization
```

#### Usage
```c++
Comparator.output_initval = out::init_high;
```

#### Default state
`Comparator.output_initval` defaults to `out::init_low` if not specified in the user program.

### Which properties work where

|        Option | DA/DB |  DD |  EA | Tiny0 | Tiny1 | Tiny1+ | Tiny2 | Mega0 |
|---------------|-------|-----|-----|-------|-------|--------|-------|-------|
|     in_p::in0 |   All |Some | All |   All |   All |    All |   All |   All |
|     in_p::in1 |   All | All | All |  Some |  Some |    All |  Some |   All |
|     in_p::in2 |   All | All | All |    No |    No |    All |   All |   All |
|     in_p::in3 |   All | All | All |    No |    No |   Some |  Some |   All |
|     in_p::in4 |    No | All | All |    No |    No |     No |    No |    No |
|     in_n::in0 |   All |Some | All |   All |   All |    All |   All |   All |
|     in_n::in1 |  Some |  No | All |  Some |  Some |    All |  Some |   All |
|     in_n::in2 |   All |  No | All |    No |    No |   Some |  Some |   All |
|     in_n::in3 |    No | All | All |    No |    No |     No |   All |    No |
|    in_n::vref |    No |  No |  No |   All |   All |    All |    No |    No |
|  in_n::dacref |   All | All | All |    No |   All |    All |   All |   All |
| out::pin_swap |  Some |  No |Some |    No |    No |     No |    No |    No |
Where it is marked "Some", this will depend on pincount. Check the chart further up for more information. All means that **at least one comparator** on all pincounts has the specified input, **not that they all have it**. We do not guarantee that the library will reject attempts to specify invalid pins, and on parts with more than one comparator, it can only reject them if they are absent for ALL comparators (ex, AINN1 is always PD0 on Dx-series, and DB-series parts with 28 or 32 pins don't have that pin, so specifying `in_n::in0` will be rejected with a compile error. A tiny1614 has AINP1 for AC2 and AINP2 for AC0 and AC1. Using `in_p::in1` for AC0 or AC1 will not result in any compile error, nor will using `in_p::in2` with AC2). Use extra care on parts with multiple comparators.

## Comparator class methods

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
Comparator.stop(); // Stop comparator. Will not reenable digital input, and digitalRead() of the input pins will return 0 regardless of the voltage on the pin
```

```c++
Comparator.stop(true); // Stop comparator. Digital input on the pins that this comparator was using will be reenabled.
```

### read()
This method readss the current value of the comparator and returns it as a `bool`. It is effected by output invert configured for the analog comparator. If the output pin was inverted a second time (useful for particularly obscure use cases) via the `PINnCTRL` register, this value will not reflect it.

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
