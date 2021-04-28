# Logic
A library for interfacing with the CCL (Configurable Custom Logic) peripherals on modern AVR MCUs.
Developed by MCUdude for use with [MegaCoreX](https://github.com/MCUdude/MegaCoreX), adapted to tinyAVR 0/1-series parts by [Tadashi G. Takaoka](https://github.com/tgtakaoka), and to 2-series and Dx-series by [Spence Konde](https://github.com/SpenceKonde)
Correction of several issues on the tinyAVR parts and adaptation of examples by [Spence Konde](https://github.com/SpenceKonde).
All of the examples assume the use of megaTinyCore, MegaCoreX or DxCore as appropriate (code is generally directly portable between megaAVR 0-series, tinyAVR 2-series and AVR Dx-series parts, minor modifications may be required for porting to or from tinyAVR 0/1-series parts). This document is focused on the tinyAVR parts. For information on this library relating to the megaAVR or AVR Dx-series parts, see the version of this document included with MegaCoreX or DxCore.
More information about CCL can be found in the [Microchip Application Note TB3218](http://ww1.microchip.com/downloads/en/AppNotes/TB3218-Getting-Started-with-CCL-90003218A.pdf) and in the [megaAVR-0 family data sheet](http://ww1.microchip.com/downloads/en/DeviceDoc/megaAVR0-series-Family-Data-Sheet-DS40002015B.pdf) and the datasheet for the part in question.



## Logic
Class for interfacing with the built-in logic block (sometimes referred to as `LUT`s, from the "LookUp Table" - though it is curious use of language, that is what Microchip refers to them as). Use the predefined objects `Logic0`, `Logic1`, `Logic2` and `Logic3`. The logic blocks are paired, each pair sharing a single sequencer and `feedback` channel. `Logic2` and `Logic3` are only available on tinyAVR 2-series parts.

These objects expose all configuration options as properties`*`, described below, with methods`*` to apply those settings, attach interrupts (2-series only) and enable and disable the CCL in general.



### Pin availability and Quick Reference
This was simpler with 0/1-series (22 i/o pins, 10 CCL functions, none stacked onto the same pins), but with 2-series, 20 different CCL functions, crammed into just 22 GPIO pins, two of which have one blocks' input and one of the outputs for another. The ones with no inputs on lower pincount parts are more useful than one might expect; advanced use cases will often use mostly internal inputs - and the event system can always be used to get pin input in anyway - albeit at a cost of an event channel.

Logic Block |  IN0-2  | OUT | ALT OUT |
Logic0      | PA0-PA2 | PA4 | PB4     |
Logic1      | PC3-PC5 | PA7 | PC1     |
Logic2      | PB0-PB2 | PB3 | PB6     |
Logic3      | PC0-PC2 | PC4 | PA5     |

* Inputs are always in consecutive order counting by bit position (not physical order).
* All logic blocks present always have at least one output pin
* `Logic2` and `Logic3` are present only on the 2-series.

Logic Block |  8-pin      |     14-pin    |     20-pin    |     24-pin    |
Logic0 IN   | IN0 IS UPDI | IN0 IS UPDI   | IN0 IS UPDI   | IN0 IS UPDI   |
Logic0 OUT  | NO ALT OUT0 | NO ALT OUT0   | YES, BOTH     | YES, BOTH     |
Logic1 IN   | NO INPUTS   | NO INPUTS     | IN0 ONLY      | YES, ALL      |
Logic1 OUT  | NO ALT OUT1 | NO ALT OUT1   | YES, BOTH     | YES, BOTH     |
Logic2 IN   |    N/A      | YES, ALL      | YES, ALL      | YES, ALL      |
Logic2 OUT  |    N/A      | NO ALT OUT2   | NO ALT OUT2   | YES, BOTH     |
Logic3 IN   |    N/A      | NO INPUTS     | YES, ALL      | YES, ALL      |
Logic3 OUT  |    N/A      | ALT OUT3 ONLY | ALT OUT3 ONLY | YES, BOTH     |

## Properties

### enable
Property controlling whether the logic block is enabled. Like all properties, you must call LogicN.init() to apply any changes.
Accepted values:
```c++
true;  // Enable the current logic block
false; // Disable the current logic block
```

##### Usage
```c++
Logic0.enable = true; // Enable logic block 0
```

##### Default state
`Logic0.enable` defaults to `false` if not specified in the user program.


### input0..input2
Properties setting input sources for the three inputs of this logic block.

General notes:
* Timer/Counter input sources are associated with a WO (Waveform Output) channel - they are logic 1 (true) when the PWM output on that channel is `HIGH` (See the datasheet I/O multiplexed signals chart to associate WO channels with pins)
* The tinyAVR 0/1-series datasheets refer to the event channels as 0 and 1. On all subsequent parts, they are referred to as A and B. The Logic library always accepts both, though we receommend using event_a/event_b everywhere, as it is clear that that is the convention that Microchip chose to settle on.
* See the version of this file distributed with MegaCoreX or DxCore for information on the corresponding options on those parts.

#### Accepted values for tinyAVR 0/1-series parts:

``` c++
in::masked;           // Pin not in use
in::unused;           // Synonym for masked
in::disable;          // Synonym for masked
in::feedback;         // Connect output of logic block 0 or the sequencer if in use, to this input
in::link;             // Connect output of the other logic block to this input
in::event_0;          // Connect input to event a
in::event_a;          // Connect input to event a (preferred)
in::event_1;          // Connect input to event b
in::event_b;          // Connect input to event b (preferred)
in::pin;              // Connect input to CCL IN0, IN1, or IN2 - and do not change pinMode
in::input_pullup;     // Connect input to CCL IN0, IN1, or IN2 - and set input, pullup on
in::input_no_pullup;  // Connect input to CCL IN0, IN1, or IN2 - and set input, pullup off
in::input;            // Synonym for input_no_pullup
in::ac0;              // Connect input to the output of analog comparator (AC0)
in::ac;               // Synonym for ac0
in::tcb0;             // Connect input to TCB0 WO
in::tcb;              // Synonym for tcb0
in::tca0;             // Connect input to TCA0 WO0~2 for input 0~2
in::tca;              // Synonym for tca0
in::tcd;              // Connect input to TCD0 WOA, WOB, WOA for input 0, 1, 2
in::usart;            // Connect input to USART0 XCK and TXD for input 0, 1
in::spi;              // Connect input to SPI0 SCK, MOSI, MISO for input 0, 1, 2
in::ac1;              // Connect input to AC1 OUT (input 0, 1 on parts with 3 ACs only)
in::tcb1;             // Connect input to TCB1 WO (input 0, 1 on parts with 2 TCBs only)
in::ac2;              // Connect input to AC2 OUT (input 0, 1 on parts with 3 ACs only)
```

Notes specific to ATtiny 0/1-series:
* Not all inputs are available on all parts - only input sources corresponding to peripherals on that device are available. Not all options are available for input 2, as noted above.
* On CCL1 (logic1), IN0 I/O input is available only on the 20 and-24 pin parts, and IN1 and IN2 only on 24-pin parts. The event inputs can be used with pin events to take input from a different pins for up to two inputs. This is demonstrated in the five input example.
* CCL0's IN0 pin is on PA0, which is nominally the UPDI pin. This may limit the usefulness of CCL0 on the ATtiny parts (though it may work as long as the input cannot be mistaken for a UPDI activation command); configuring UPDI as GPIO prevents further programming via UPDI except via HV programming. One can always use the event system to substitute another input for IN0; This is demonstrated in the three input example.
* Only the ATtiny1614, 1616, 1617, 3216, and 3217 have TCB1, AC1, and AC2.
* Some parts in circulation are impacted by an errata: link input does not work unless pin output of the other logic block is enabled. Check the applicable errata and datasheet clarification document from Microchip.
* These were the first AVRs with CCL. They made some decisions that they changed for parts released more recently. It appears that they hadn't yet thought to use the input number to pick which instance of a peripheral to use, for one thing. These differences will impact code portability. Unlike all other parts with CCL hardware:
  * USART option will use XCK on input 0, TXD on input 1, and is not valid for input 2.
  * MISO is (supposedly) available as an input when SPI is used as the input.
  * Parts with two TCBs or three ACs give them their own channel. Normal parts have one channel for this, and the input number selects which one is used.


#### Accepted values for tinyAVR 2-series:
``` c++
in::masked;           // Pin not in use
in::unused;           // Synonym for masked
in::disable;          // Synonym for masked
in::feedback;         // Connect output of logic block 0/2, or their sequencer if in use, to this input
in::link;             // Connect output of logic block n+1 to this input
in::event_0;          // Connect input to event a
in::event_a;          // Connect input to event a (preferred)
in::event_1;          // Connect input to event b
in::event_b;          // Connect input to event b (preferred)
in::pin;              // Connect input to CCL IN0, IN1, or IN2 - and do not change pinMode
in::input_pullup;     // Connect input to CCL IN0, IN1, or IN2 - and set input, pullup on
in::input_no_pullup;  // Connect input to CCL IN0, IN1, or IN2 - and set input, pullup off
in::input;            // Synonym for input_no_pullup
in::ac;               // Connect input to the output of the internal analog comparator AC0.
in::uart;             // Connect input to UART TX. Input 0 connects to UART0 TX, input 1 to UART1 TX. Not available on input 2, and XCK not available.
in::spi;              // Connect input to SPI. Input 0 and 1 connect to MOSI, and input 2 connects to SCK
in::tca0;             // Connect input to TCA0. Input 0 connects to WO0, input 1 to WO1 and input2 to WO2
in::tca;              // Synonym for tca0
in::tcb;              // Connect input to a TCB. Input 0 connects to TCB0 W0, input 1 to TCB1 WO. Not available on input 2
```

Notes for tinyAVR 2-series:
* Inputs 0 and 1 connect to MOSI, input 2 is SCK. Master mode only. Note also that the order is different from the other ATtiny parts, but the same as Dx-series and megaAVR 0-series.
* If input on the highest-number Logic3 is set to link, it will use the output of Logic0
* If you need to link input to logic block other than the n+1 block, you can use the event system for that.
* Not all pin inputs are available on all parts; the event system can be used to substitute inputs.

##### Usage
``` c++
Logic0.input0 = in::link;         // Connect output from block 1 to input 0 of block 0
Logic0.input1 = in::input;        // Connect the input 1 from block 0 to its GPIO
Logic0.input2 = in::input_pullup; // Connect the input 2 from block 0 to its GPIO, with pullup on
```

##### Default state
`LogicN.inputN` defaults to `in::unused` if not specified in the user program.


### output
Property controlling the logic block output pin behavior. Note that the output of the logic block still can be used internally if the output pin is disabled (though on some tinyAVR 0/1-series silicon, this doesn't work, see the errata for your parts). The pin's direction and output value are overridden, so you do not need to set the pin `OUTPUT` first.
Accepted values:
```c++
out::disable; // Disable the output GPIO pin. Useful when triggering an interrupt instead.
out::enable;  // Enable the output GPIO pin
```

##### Usage
```c++
Logic0.output = out::disable; // Disable the output GPIO pin.
```

##### Default state
`LogicN.output` defaults to `out::disable` if not specified in the user program.


### output_swap
Property controlling whether to use the alternate output pin. See the pinout diagrams in the [Core this is part of](../../../README.md) for more info.
Accepted values:
```c++
out::no_swap;  // Use default pin position, pin 3 on the port
out::pin_swap; // Use alternative position, pin 6 on the port
```

##### Usage
```c++
Logic0.output_swap = out::no_swap; // No pin swap for output of block0
```

##### Default state
`LogicN.output_swap` defaults to `out::no_swap` if not specified in the user program.


### filter
Property to control whether the output is passed through a filter or synchronizer. Useful when multiple logic blocks are connected internally to prevent race conditions and glitches that could arise due to the asynchronous nature of CCL. Alternately, the delay itself may be desirable, or it can be combined with a configuration which inverts it's own output (and would otherwise oscillate asynchronously), but is clocked by some other source; this will then act to divide that clock speed by 4 (synchronizer) or
Accepted values:
```c++
filter::disable;      // No filter used, asynchronous output.
filter::synchronizer; // Connect synchronizer to output; delays output by 2 clock cycles.
filter::synch;        // Syntactic sugar for synchronizer
filter::sync;         // Syntactic sugar for synchronizer
filter::filter;       // Connect filter to output; delays output by 4 clock cycles, only passes output that is stable for >2 clock cycles.
```

##### Usage
```c++
Logic0.filter = filter::filter; // Enable filter on output of block 0
```

See also [Prescaling Clocks with CCLs](https://github.com/SpenceKonde/AVR-Guidance/blob/master/CCL_EVSYS_hacks/CCL_prescaling.md)

##### Default state
`LogicN.filter` defaults to `filter::disable` if not specified in the user program.


### clocksource
Proprty to set the clock source for the logic block; this is used for the synchronizer and filter only (otherwise, the logic blocks are asynchronous - and shockingly fast. You can rig them up so that they oscillate, and with the most direct approaches, it can reach upwards of 100 MHz!). Note that 32kHz-derived and unprescaled clock options are not available on 0-series and 1-series parts; keep this in mind if backwards portability is important. If sequential logic is used, it is clocked from the clock source used by the even-numbered logic block, if it uses a clock.
Accepted values:
```c++
clocksource::clk_per;      // Clock from the peripheral clock (ie, system clock)
clocksource::in2;          // Clock from the selected input2; it is treated as a 0 in the truth table.
clocksource::oschf;        // Clock from the **unprescaled** internal HF oscillator. Same as clk_per if system clock not prescaled. tinyAVR 2-series only
clocksource::osc32k;       // Clock from the internal 32.768 kHz oscillator - tinyAVR 2-series only
clocksource::osc1l;        // Clock from the internal 32.768 kHz oscillator prescaled by 32 - tinyAVR 2-series only
```


##### Usage
```c++
Logic2.clocksource = clocksource::oschf; // Set block 2 to use unprescaled high frequency internal oscillator.
```

##### Default state
`LogicN.clocksource` defaults to `clocksource::clk_per` if not specified in the user program.



### edgedetect
Property to control use of the edge detector. The edge detector can be used to generate a pulse when detecting a rising edge on its input. To detect a falling edge, the TRUTH table should be programmed to provide inverted output. In order to avoid unpredictable behavior, a valid filter option must be enabled (note: that's what the datasheet says; it's not clear whether you can get the unpredictable behavior, or if the edge detecter won't be connected unless a filter or synchronizer is enabled). Note that this is likely only of use when the output is being used for sequential logic or as the input to another logic block; it looks particularly useful on the odd LUT input to a J-K flip-flop sequential logic unit.

```c++
edgedetect::disable;      // No edge detection used
edgedetect::enable;       // Edge detection used
```

### sequencer
Property controlling the "sequencer" for this pair of logic blocks - these are latches or flip-flops which remember a state. There is 1 sequencer per 2 CCLs, each controls one of the two inputs to a flip flop or latch; this option is ignored for the odd-numbered logic blocks. Flip-flops are clocked from the same clock source as the even logic block, latches are asynchronous.

Accepted values:
```c++
sequencer::disable;      // No sequencer connected
sequencer::d_flip_flop;  // D flip flop sequencer connected
sequencer::jk_flip_flop; // JK flip flop sequencer connected
sequencer::d_latch;      // Gated D latch sequencer connected - note that on most tinyAVR 0/1-series parts, this doesn't work. See the Errata.
sequencer::rs_latch;     // RS latch sequencer connected
```

##### Usage
```c++
Logic0.sequencer = sequencer::disable; // Disable sequencer
```

##### Default state
`LogicN.sequencer` defaults to `sequencer::disable` if not specified in the user program.


### truth
This property contains the 8-bit truth table value.
Accepted values between 0x00 and 0xFF - this is where the input values are looked up to determine what value to output.

In2:in0 are treated as a 3-bit number (so 0-7), that bit number (starting from 0) determines the output value.

Ex: If in1 and in0 are high, and in2 is low, input is 3, (0b011 = 3). If the truth table is 0x89 - `0b10001001` - then the output will be high (0b1000*1*001). Put another way, the output it high if `truth & (1 << input)` is true.

##### Usage
```c++
Logic0.truth = 0xF0;
```

##### Default state
`LogicN.truth` defaults to `0x00` if not specified in the user program.

## Methods

### init()
Method for initializing a logic block; the settings you have previously configured will be applied and pins configured as requested at this time only.

##### Usage
```c++
Logic0.init(); // Initialize block 0
Logic1.init(); // Initialize block 1
```


### start()
Static method for starting the CCL hardware after desired blocks have been initialized using `LogicN.init()`.

##### Usage
```c++
Logic::start(); // Start CCL hardware
```

### stop()
Static method for stopping the CCL hardware, for example to reconfigure the logic blocks.

##### Usage
```c++
Logic::stop(); // Stop CCL
```

### attachInterrupt()
Method for enabling interrupts for a specific block.
Valid arguments for the third parameters are `RISING`, `FALLING` and `CHANGE`.
This method isn't available on tinyAVR 0/1-series as these parts cannot generate an interrupt from the CCL blocks.

##### Usage
```c++
Logic0.attachInterrupt(blinkLED, RISING); // Runthe blinkLED function when the putput goes high

void blinkLED()
{
  digitalWrite(myLedPin, CHANGE);
}
```


### detachInterrupt()
Method for disabling interrupts for a specific block.
This method isn't available on tinyAVR 0/1-series.

##### Usage
```c++
Logic0.detachInterrupt(); // Disable interrupts for block 0
```

## Reconfiguring
There are TWO levels of "enable protection" on the CCL hardware. According to the Silicon Errata, only one of these is intended. As always, it's anyone's guess when or if this issue will be corrected in a future silicon rev, and if so, on which parts (it would appear that Microchip only became aware of the issue after the Dx-series parts were released - although it impacts all presently available parts, it is only listed in errata updated since mid-2020). The intended enable-protection is that a given logic block cannot be reconfigured while enabled. This is handled by `init()` - you can write your new setting to a logic block, call `LogicN.init()` and it will briefly disable the logic block, make the changes, and re-enable it.

The unintended layer is that no logic block can be reconfigured without also disabling the whole CCL system. Changes can be freely made to the `Logic` classes, however, only the `init()` method will apply those changes, and you must call `Logic::stop()` before calling them, and `Logic::start()` afterwards. If/when parts become available where this is not necessary, this step may be omitted, and this library may be amended to provide a way to check.

##### Example
```c++


// logic block 0 configured, initialized, and in use
Logic1.truth=0x55;      // new truth table
Logic1.input2=tca0;     // and different input 2
Logic1.enabled=true;    // enable another LUT
Logic0.truth=0x79;      // changed truth table for LUT0

// At this point Logic1 not enabled, and logic0 is using old settings

Logic::stop();  // have to turn off Logic0 too, even though I might not want to
Logic0.init();  // apply changes to logic block 0
Logic1.init();  // apply settings to logic block 1 for the first time
Logic::start(); // re-enable


```



## Note on terminology`*`
Yes, technically, C++ doesn't have "properties" or "methods" - these are "member variables" and "member functions" in C++ parlance. They mean the same thing. I've chosen to use the more familiar, preseent day terminology.
