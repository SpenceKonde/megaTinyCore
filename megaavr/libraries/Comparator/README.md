# Comparator
A library for interfacing with the analog comparator peripheral in the AVR DA and DB series MCUs.
Developed by [MCUdude](https://github.com/MCUdude/).
The tinyAVR 0-series, 2-series, and 1-series parts with less than 16k of flash have a single analog comparator, with either 1 or 2 options each for the positive and negative input pins. 1-series parts with 16k or 32k flash instead have three comparators, which have up to 4 options for the positive input. The internal voltage reference can also be used as the negative side; on the 1-series and 2-series parts, that reference can be scaled by an internal 8-bit DAC. This librarty provides a wrapper class, `Comparator` that exposes the full functionality of these peripherals without having to manually manipulate registers.

An analog comparator, as the name implies, compares two analog voltages. If the "positive" input is higher than the "negative" one, the output is 1/HIGH, otherwise it is low. These can be configured to invert their value, and the output pins themselves can be inverted if desired (this library does not do that for you though - you want the builtin pinConfigure() for that)

Use the predefined object(s) `Comparator` or `Comparator0` and for parts that have them, `Comparator1` or `Comparator2`. (`Comparator` is #defined as `Comparator0` to permit compatibility code written for older versions of this library written for single-comparator parts). **Only the ATtiny3216, 3217, 1614, 1616, and 1617 have three comparators.** The rest of the parts have only one.

## Pin Assignments

| Pin     |  On |  AC0  |  AC1  |  AC2  |  Pincounts on   |
|---------|-----|-------|-------|-------|-----------------|
| PIN_PA5 | 012 |  OUT  | IN- 0 |   -   | OUT PA3 on 8pin |
| PIN_PA6 | 012 | IN- 0 | IN+ 1 | IN+ 0 |        -        |
| PIN_PA7 | 012 | IN+ 0 | IN+ 0 | IN- 0 |        -        |
| PIN_PB0 |   2 | IN- 2 | IN+ 2 | IN+ 1 |    14+ pin only |
| PIN_PB1 |**1**2|IN+ 2 |   -   |   -   |    14+ pin only |
| PIN_PB2 |  -  |   -   |   -   |  OUT  |    14+ pin only |
| PIN_PB3 |  -  |   -   |  OUT  |   -   |    14+ pin only |
| PIN_PB4 |  12 | IN- 1 | IN+ 3 |   -   |  20/24 pin only |
| PIN_PB5 |  12 | IN+ 1 |   -   | IN+ 2 |  20/24 pin only |
| PIN_PB6 |**1**2|IN+ 3 |   -   | IN- 1 |     24 pin only |
| PIN_PB7 |     |   -   | IN- 1 | IN+ 3 |     24 pin only |


* Not all pins exist on all parts; if a part does not have a given pin, that input is not available there.
  * On 8-pin parts, AC0 OUT is on PA3 instead of PA5, since there is no PA5.
* "On" column shows which AC0 pins are available for each series.
  * On 0-series, only positive and negative input 0 are available for AC0.
  * On 1-series with <16k flash, only AC0 and inputs 0 and 1 are available. Input 1 only available on parts with that physical pin.
  * On 1-series with 16k or 32k flash, all positive input options shown are available provided the pin exists. Negative inputs `IN- 0` and `IN- 1` are available, but `IN- 2` is not; the register only allocates 2 bits for selecting the negative side, and the 1-series has both vref and dacref options.
  * On 2-series, all AC0 inputs are available (as long as the pin exists on that part). Only one comparator, AC0, exists.

### input_p
Property for setting what input pin the positive input of the comparator should be connected to
Accepted values:
``` c++
in_p::in0; // Use positive input pin 0 as input
in_p::in1; // Use positive input pin 1 as input
in_p::in2; // Use positive input pin 2 as input
in_p::in3; // Use positive input pin 3 as input
```

#### Usage
``` c++
Comparator.input_p = in_p::in0;  // Connect positive input pin 0 to the positive input of the comparator
```

#### Default state
`Comparator.input_p` defaults to `in_p::in0` if not specified in the user program.


### input_n
Property for setting what input pin the negative input of the comparator should be connected to
Accepted values:
``` c++
in_n::in0;    // Use positive input pin 0 as input
in_n::in1;    // Use positive input pin 1 as input
in_n::in2;    // Use positive input pin 2 as input (2-series only)
in_n::vref;   // Use the voltage reference w/out DAC (0/1-series only)
in_n::dacref; // Use DACREF as input (1/2-series only)
```

#### Usage
``` c++
Comparator.input_n = in_n::vref;  // Connect voltage referemceto the negative input of the comparator
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
ref::vref_4v34;   // 4.34V internal reference
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


### dacref (not available on 0-series parts)
Property for setting the DACREF value - this voltage can be selected as the input for the negative side of the comparator. **On 1-series devices, AC0 uses the same DAC for this as is used to generate analog output on PA6!** That means that calling Comparator0.init() with dacref set will change the voltage on that pin if you have previously set it to output an analog voltage, and that analogWrite() on that pin will change the reference voltage being used by the comparator. It is advised that users not use the dacref option with AC0 if they are also using the DAC output (though it could be useful during debugging to be able to measure the voltage on that pin and know exactly what voltage the comparator is comparing to)
This is the formula for the DACREF output voltage:

<img src="http://latex.codecogs.com/svg.latex?V_{DACREF} = \frac{Comparator.dacref}{256} * Comparator.reference" border="0"/>

#### Usage
``` c++
Comparator.dacref = 127;  // the DACREF input will by a a voltage one half of the selected reference voltage.
```

#### Default state
`Comparator.dacref` defaults to `255` if not specified in the user program.


### hysteresis
Property for setting the comparator input hysteresis. Useful for eliminating "bouncing" - multiple spurious transitions as a slowly changing input with some noise (which is hard to eliminate) passes the constant voltage, or even in the steady state with equal or nearly equal voltages.
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
Property for setting the comparator output - either internal or external, inverted or not (note also that the pin itself could be inverted with the INVEN bit of PORTA.PIN7CTRL (or PORTC.PIN6CTRL if alternate output pin is used; this allows the pin to take a logical state opposite from tne internal (event channel) output, thus allowing every possible combination of external and internal output polarities.
Accepted values:
``` c++
out::disable;        // No output pin, signal not inverted internally
out::disable_invert; // No output pin, signal inverted internally
out::enable;         // Enable output pin (PA7), signal not inverted internally
out::invert;         // Enable output pin (PA7), signal inverted internally
out::enable_invert;  // Identical to out::invert
```
| AC# | AC0 | AC1 | AC2 | AC0 on 8-pin parts |
|-----|-----|-----|-----|--------------------|
| PIN | PA5 | PB3 | PB2 |         PA5        |


#### Usage
``` c++
Comparator.output = out::enable; // Enable output comparator's output pin. Comparator is AC0, so this is PA5.
```

#### Default state
`Comparator.output` defaults to `out::disable` if not specified in the user program.


### output_swap
The output_swap option is not available on tinyAVR devices; each comparator can only output to a single pin. On some parts, there are multiple output pin options (don't be too jealous - all three of their comparators to choose between the same 2 pins. Which are also the pins that the ZCD's use.)

### output_initval
Initial state the comparator output pin has when initialized.
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

## Methods of Comparator
Also known as "member functions" in C++ parlance.

### init()
Method for initializing the comparator. This copies the properties described above into the hardware registers for the comparators (until this method is called, the hardware registers are untouched). Note that if you change settings, you must call init() to apply these changes (unlike some peripherals, you do not need to disable the peripheral to do that). *In accordance with the manufacturer recommendations, init() also disables the digital input buffers on the pins used as inputs.*

#### Usage
```c++
Comparator.init(); // Initialize comparator
```

### start()
Method to actually start the analog comparator (the init() method configures it, start() turns it on) - you would normally call this after init() when setting up the comparator, or after you had called it's stop() method and now wish to restart it (if you haven't called init() in the interim, it will be started with the previous settings).
#### Usage
```c++
Comparator.start(); // Start comparator
```
### read()
Method to get the current state of the comparator's output as a boolean. This allows you to, among other things, poll the comparators instead of using them with interrupts.

#### Usage
```c++
if (Comparator.read()) {
  takeSomeAction(); //positive input higher than reference on negative input - take appropriate action to fix it
}
```


### stop()
Method for stopping the analog comparator; this clears only the enable bit, everything else is left in place. Optionally instead call `stop(true)` to also re-enable digital input on the pins used. This is only necessary if you plan to call digitalRead() on them after disabling the analog comparator (which seems an uncommon use case if they have an analog voltage connected to them!), and costs about 100 bytes of flash on a 1-series part (less on 0-series)

#### Usage
```c++
Comparator.stop(); // Stop comparator, digitalRead() on the pins it used as inputs remains disabled.

Comparator.stop(true); //Stop comparator, digitalRead() on pins it used as inputs works again.
```


### attachInterrupt()
Method for enabling analog comparator interrupt. The interrupt will trigger when the the comparator output changes.
Valid arguments for the third parameters are `RISING`, `FALLING` and `CHANGE`. Clearing of the INTFLAG is handled by the library, and does not need to be done in your interrupt code.

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
