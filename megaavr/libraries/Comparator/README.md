# Comparator
A library for interfacing with the analog comparator peripheral in the AVR DA and DB series MCUs.
Developed by [MCUdude](https://github.com/MCUdude/).
The tinyAVR 0-series, 2-series, and 1-series parts with less than 16k of flash have a single analog comparator, with either 1 or 2 options each for the positive and negative input pins. 1-series parts with 16k or 32k flash instead have three comparators, which have up to 4 options for the positive input. The internal voltage reference can also be used as the negative side; on the 1-series and 2-series parts, that reference can be scaled by an internal 8-bit DAC.

## Comparator
Class for interfacing with the built-in comparator(s). Use the predefined objects `Comparator`/`Comparator0` and for parts that have them, `Comparator1` or `Comparator2`. (`Comparator` is #defined as `Comparator0` to permit compatibility with parts that have only a single comparator and use the same or similar library). Only the ATtiny3216, 3217, 1614, 1616, and 1617 have three comparators. The rest of the parts have only the one.


| Pin     |  AC0  |  AC1  |  AC2  | Availability    |
|---------|-------|-------|-------|-----------------|
| PIN_PA5 |  OUT  | IN- 0 |   -   |        -        |
| PIN_PA6 | IN- 0 | IN+ 1 | IN+ 0 |        -        |
| PIN_PA7 | IN+ 0 | IN+ 0 | IN- 0 |        -        |
| PIN_PB0 | IN- 2 | IN+ 2 | IN+ 1 |    14+ pin only |
| PIN_PB1 | IN+ 2 |   -   |   -   |    14+ pin only |
| PIN_PB2 |   -   |   -   |  OUT  |    14+ pin only |
| PIN_PB3 |   -   |  OUT  |   -   |    14+ pin only |
| PIN_PB4 | IN- 1 | IN+ 3 |   -   |  20/24 pin only |
| PIN_PB5 | IN+ 1 |   -   | IN+ 2 |  20/24 pin only |
| PIN_PB6 | IN+ 3 |   -   | IN- 1 |     24 pin only |
| PIN_PB7 |   -   | IN- 1 | IN+ 3 |     24 pin only |

* Not all pins exist on all parts; if a part does not have a given pin, that input is not available there.
* On 0-series, only positive and negative input 0 is available, and only AC0 exists.
* On 1-series with <16k flash, only AC0 and inputs 0 and 1 are available. Input 1 only available on parts with that physical pin.
* On 1-series with 16k or 32k flash, all positive input options shown are available provided the pin exists (there is no PB6 or PB7 except on 24-pin parts, for example, so an ATtiny3216 cannot choose positive input 3 for AC0), as are `IN- 0` and `IN- 1`. `IN- 2` is not available.
* On 2-series, all AC0 inputs are available (as long as the pin exists on that part). AC1 and AC2 do not exist.

### input_p
Property for setting what input pin the positive input of the comparator should be connected to
Accepted values:
``` c++
in_p::in0; // Use positive input pin 0 as input
in_p::in1; // Use positive input pin 1 as input
in_p::in2; // Use positive input pin 2 as input
in_p::in3; // Use positive input pin 3 as input
```

##### Usage
``` c++
Comparator.input_p = in_p::in0;  // Connect positive input pin 0 to the positive pin of the comparator
```

##### Default state
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

##### Usage
``` c++
Comparator.input_n = in_n::in0;  // Connect negative input pin 0 to the negative pin of the comparator
```

##### Default state
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

##### Usage
``` c++
Comparator.reference = ref::vref_4v096;  // Use the internal 4.096V reference for the DACREF
```

##### Default state
`Comparator.reference` defaults to `ref::disable` if not specified in the user program.


### dacref (not available on 0-series parts)
Property for setting the DACREF value - this voltage can be selected as the input for the negative side of the comparator. On 1-series devices, AC0 uses the same DAC for this as is used to generate analog output on PA6! That means that calling Comparator0.init() with dacref set will change the voltage on that pin if you have previously set it to output an analog voltage, and that analogWrite() on that pin will change the reference voltage being used by the comparator. It is advised that users not use the dacref option with AC0 if they are also using the DAC output (though it could be useful during debugging to be able to measure the voltage on that pin and know exactly what voltage the comparator is comparing too)
This is the formula for the DACREF output voltage:

<img src="http://latex.codecogs.com/svg.latex?V_{DACREF} = \frac{Comparator.dacref}{256} * Comparator.reference" border="0"/>

##### Usage
``` c++
Comparator.dacref = 127;  // Divide the reference voltage by two
```

##### Default state
`Comparator.dacref` defaults to `255` if not specified in the user program.


### hysteresis
Property for setting the comparator input hysteresis. Useful for eliminating "bouncing" - multiple spurious transitions as a slowly changing input with some noise (which is hard to eliminate) passes the constant voltage, or when the two voltages are nearly the same.
Accepted values:
``` c++
hyst::disable; // No hysteresis
hyst::small;   // 10mV hysteresis (±5mV)
hyst::medium;  // 25mV hysteresis (±12.5mV)
hyst::large;   // 50mV hysteresis (±25mV)
```

##### Usage
``` c++
Comparator.hysteresis = hyst::large;  // Use 50V hysteresis
```

##### Default state
`Comparator.hysteresis` defaults to `hyst::disable` if not specified in the user program.


### output
Property for setting the comparator output pin (PA7).
Accepted values:
``` c++
out::disable; // No output pin
out::enable;  // Enable output pin (PA7)
out::invert;  // Invert output pin (PA7)
```

##### Usage
``` c++
Comparator.output = out::enable; // Enable output pin (PA7)
```

##### Default state
`Comparator.output` defaults to `out::disable` if not specified in the user program.


### output_swap
Property for pin swapping the physical output pin to its alternative position. See the pinout diagrams in the DxCore README for detailed info.
Accepted values:
```c++
out::no_swap;  // Use default pin position
out::pin_swap; // Use alternative position
```

##### Usage
```c++
Comparator.output_swap = out::no_swap; // No pin swap for output
```

##### Default state
`Comparator.output_swap` defaults to `out::no_swap` if not specified in the user program.


### output_initval
Initial state the comparator output pin has when initialized.
Accepted values:
```c++
out::init_low;  // Output pin low after initialization
out::init_high; // Output pin high after initialization
```

##### Usage
```c++
Comparator.output_initval = out::init_high;
```

##### Default state
`Comparator.output_initval` defaults to `out::init_low` if not specified in the user program.


## init()
Method for initializing the comparator. This copies the properties described above into the hardware registers for the comparators (until this method is called, the hardware registers are untouched). Note that if you change settings, you must call init() to apply these changes (unlike some peripherals, you do not need to disable the peripheral to do that). In accordance with the manufacturer recommendations, init() also disables the digital input buffers on the pins used as inputs.

##### Usage
```c++
Comparator.init(); // Initialize comparator
```

## start()
Method for starting the analog comparator - you would normally call this after init() when seting up the comparator, or after you stopped it, and then wish to restart it (if you haven't called init() in the interim, it will be started with the previous settings).
##### Usage
```c++
Comparator.start(); // Start comparator
```


## stop()
Method for stopping the analog comparator; this clears only the enable bit, everything else is left in place. Optionally instead call `stop(true)` to also reenable digital input on the pins used. This is only necessaery if you plan to call digitalRead() on them after disabling the analog comparator (which seems an uncommon use case if they have an analog voltage connected to them!), and costs about 100 bytes of flash on a 1-series part (less on 0-series)

##### Usage
```c++
Comparator.stop(); // Stop comparator, digitalRead() on the pins it used as inputs remains disabled.

Comparator.stop(true); //Stop comparator, digitalRead() on pins it used as inputs works again.
```


## attachInterrupt()
Method for enabling analog comparator interrupt. The interrupt will trigger when the the comparator output changes.
Valid arguments for the third parameters are `RISING`, `FALLING` and `CHANGE`. Clearing of the INTFLAG is handled by the library, and does not need to be done in your interrupt code.

##### Usage
```c++
Comparator.attachInterrupt(blinkLED, RISING); // Run the blinkLED function when the comparator output goes high

void blinkLED() {
  digitalWrite(myLedPin, CHANGE);
}
```


## detachInterrupt()
Method for disabling analog comparator interrupt.

##### Usage
```c++
Comparator.detachInterrupt(); // Disable interrupt
```
