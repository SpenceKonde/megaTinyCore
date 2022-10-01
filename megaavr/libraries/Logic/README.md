# Logic
A library for interfacing with the CCL (Configurable Custom Logic) peripherals on modern AVR MCUs.
Developed by MCUdude for use with [MegaCoreX](https://github.com/MCUdude/MegaCoreX), adapted to tinyAVR 0/1-series parts by [Tadashi G. Takaoka](https://github.com/tgtakaoka), and to 2-series and Dx-series by [Spence Konde](https://github.com/SpenceKonde)
Correction of several issues on the tinyAVR parts and adaptation of examples by [Spence Konde](https://github.com/SpenceKonde).
All of the examples assume the use of megaTinyCore, MegaCoreX or DxCore as appropriate (code is generally directly portable between megaAVR 0-series, tinyAVR 2-series and AVR Dx-series parts, minor modifications may be required for porting to or from tinyAVR 0/1-series parts). This document is focused on the Dx-series parts. For information on this library relating to the megaAVR or tinyAVR parts, see the version of this document included with MegaCoreX or megaTinyCore.
More information about CCL can be found in the [Microchip Application Note TB3218](http://ww1.microchip.com/downloads/en/AppNotes/TB3218-Getting-Started-with-CCL-90003218A.pdf) and in the [megaAVR-0 family data sheet](http://ww1.microchip.com/downloads/en/DeviceDoc/megaAVR0-series-Family-Data-Sheet-DS40002015B.pdf) and the datasheet for the part in question.



### Pin availability and Quick Reference (Dx-series)

Logic Block |  IN0-2  | OUT | ALT OUT | Availability  | Notes:
------------|---------|-----|---------|---------------|-----------------------------
Logic0      | PA0-PA2 | PA3 |     PA6 |    All parts  |
Logic1      | PC0-PC2 | PC3 |     PC6 |    All parts  |
Logic2      | PD0-PD2 | PD3 |     PD6 |    All parts  | There is no PD0, hence no input 0 on any DD, 28 pin DB, or 32-pin DB
Logic3      | PF0-PF2 | PF3 |     --- |    All parts  | Link input broken on most DA/DB parts
Logic4      | PB0-PB2 | PB3 |     PB6 | 48+ pin DA/DB |
Logic5      | PC0-PC2 | PC3 |     PG6 | 48+ pin DA/DB |

Logic Block |  14-pin DD            |  20-pin DD          |  28 pin Dx          |  32-pin Dx          |  48-pin Dx          |  64-pin Dx          |
------------|-----------------------|---------------------|---------------------|---------------------|---------------------|---------------------|
Logic0 IN   | IN0/1 if no xtal used | No IN0/1 if HF xtal | No IN0/1 if HF xtal | No IN0/1 if HF xtal | No IN0/1 if HF xtal | No IN0/1 if HF xtal |
Logic0 OUT  | None                  | YES, Both           | YES, both           | YES, both           | YES, both           | YES, both           |
Logic1 IN   | No IN0 because no PC0 | No IN0 - no PC0     | YES, all            | YES, all            | YES, all            | YES, all            |
Logic1 OUT  | No alt output         | No alt output       | YES, both           | YES, both           | YES, both           | YES, both           |
Logic2 IN   | None                  | None                | No IN0 on DB/DD     | No IN0 on DB/DD     | YES, all            | YES, all            |
Logic3 IN   | None                  | None                | IN0, IN1 only       | YES, all            | YES, all            | YES, all            |
Logic3 OUT  | None                  | None                | None                | No alt output       | No alt output       | No alt output       |
Logic4 IN   | Not present on DD     | Not present on DD   | Not present on 28pin| Not present on 28pin| YES, all            | YES, all            |
Logic4 OUT  | Not present on DD     | Not present on DD   | Not present on 28pin| Not present on 28pin| No alt output       | YES, both           |
Logic5 IN   | Not present on DD     | Not present on DD   | Not present on 28pin| Not present on 28pin| NO (no PORTG)       | YES, all            |
Logic5 Out  | Not present on DD     | Not present on DD   | Not present on 28pin| Not present on 28pin| NO (no PORTG)       | YES, both           |

Notice how logic block 2 and 3 on 14/20 pin DD and logic block 5 on 48-pin parts have no dedicated pins. If you are not using pin input or output, use these logic blocks to conserve the more useful ones when not precluded by other considerations.




### Pin availability and Quick Reference (tinyAVR)
This was simpler with 0/1-series (22 i/o pins, 10 CCL functions, none stacked onto the same pins), but with 2-series, 20 different CCL functions, crammed into just 22 GPIO pins, two of which have one blocks' input and one of the outputs for another. The ones with no inputs on lower pincount parts are more useful than one might expect; advanced use cases will often use mostly internal inputs - and the event system can always be used to get pin input in anyway - albeit at a cost of an event channel. If you do not require pin inputs, these should be used in preference to the logic blocks that do have them all else being equal

Logic Block |  IN0-2  | OUT | 8pin | ALT OUT | Availability |
------------|---------|-----|------|---------|--------------|
Logic0      | PA0-PA2 | PA4 |  PA6 |     PB4 |    All parts |
Logic1      | PC3-PC5 | PA7 |  PA7 |     PC1 |    All parts |
Logic2      | PB0-PB2 | PB3 |  --- |     PB6 |     2-series |
Logic3      | PC0-PC2 | PC4 |  --- |     PA5 |     2-series |

* Inputs are always in consecutive order counting by bit position (not physical order).
* All logic blocks present *on tinyAVR* always have an output pin, but may not have input pins.
* `Logic2` and `Logic3` are present only on the 2-series.

Logic Block |  8-pin        | 14-pin        | 20-pin        | 24-pin        |
------------|---------------|---------------|---------------|---------------|
Logic0 IN   | IN0 is UPDI   | IN0 is UPDI   | IN0 is UPDI   | IN0 is UPDI   |
Logic0 OUT  | No alt output | No alt output | Yes, both     | Yes, both     |
Logic1 IN   | None          | None          | IN0 only      | Yes, all      |
Logic1 OUT  | No alt output | No alt output | Yes, both     | Yes, both     |
Logic2 IN   | Not present   | Yes, all      | Yes, all      | Yes, all      |
Logic2 OUT  | Not present   | No alt output | No alt output | Yes, both     |
Logic3 IN   | Not present   | None          | Yes, all      | Yes, all      |
Logic3 OUT  | Not present   | Only alt out  | Only alt out  | Yes, both     |


## Logic
`Logic` is the class the library provides for interfacing with a built-in logic block (sometimes referred to as `LUT`s, from the "LookUp Table" - though it is curious use of language, that is what Microchip refers to them as). Use the predefined objects `Logic0`, `Logic1`, `Logic2`, `Logic3`, `Logic4` and `Logic5`. The logic blocks are paired, each pair sharing a single sequencer and `feedback` channel. Additionally, each logic block is associated with a specific port, having it's input on pins 0 through 2, and it's output on pin 3 or 6 (note that these pin mappings are dramatically different on tinyAVR parts). In order: `PORTA`, `PORTC`, `PORTD`, `PORTF`, `PORTB`, and `PORTG`.

`Logic4` and `Logic5` are only available on 48 and 64 pin AVR Dx-series devices, and tinyAVR 0/1-series parts have only Logic0 and Logic1. All other existing and announced parts have 4 logic blocks. Note that unlike many peripherals, on parts that do not have any of the relevant pins, the peripheral is still present and available for use - in fact, many applications of the logic blocks use none of the input pins and often not even the output pin. Refer to the version of this file suppled with megaTinyCore for information on those parts.


### Invelid/reserved options
Note that there exist reserved and invalid values for many of the bitfields controlled by the properties of the logic block. This library does not support specifying these. No comprehensive investigation has been undertaken to exclude the existence of of hidden and/or broken features.

*A discovery of an undocumented or reserved option which was found to do something potentially useful gives the usual reward for undocumented features: an assembled breakout board featuring an impacted part (of your choice), or a quantity of bare boards of your choice from my [tindie store](https://tindie.com/stores/drazzy) of equal value.* See details at [https://github/SpenceKonde/AVR_Research/blob/master/Bounties.md](https://github.com/SpenceKonde/AVR_Research/blob/main/Bounties.md)


### Overhead
* On the 0/1-series, the overhead is approximately 546 bytes of flash and 26 bytes of RAM.
* On the 2-series, with twice as many LUTs, it is much larger: 984 bytes and 60b RAM.
* This is fairly small for 16/32k parts - it cannot be ignored on a 4k part, particularly not a 4k 2-series, but it it isn't an unreasonable amount of flash for the parts most people will be using. As of 2.6.0 if no ISR is attached or deteched, that overhead will not be pulled in, allowing a more performant manually defined interrupt to be written by the user (any "attach" type thing costs around 50 clock cycles)
* Writing a constant value to 4 registers (the minimum plausible needed to configure a LUT) for 2 or 4 LUTs requires 56 or 112 bytes, respectively.

## Logic class overview

| Property    | namespace or type           | Function                                    |
|-------------|-----------------------------|---------------------------------------------|
| enable      | bool                        | Enable or disable logic block               |
| input0      | logic::in::                 | Selects input 0                             |
| input1      | logic::in::                 | Selects input 1                             |
| input2      | logic::in::                 | Selects input 2                             |
| output      | logic::out::                | 'enable'/'disable' output pin               |
| output_swap | logic::out::                | 'no_swap/'pin_swap' use alt output pin      |
| filter      | logic::filter::             | 'filter'/'sync' or 'disable' filter         |
| edgedetect  | logic::edgedetect::         | 'enable'/'disable' edge detect mode         |
| sequencer   | logic::sequencer::          | selects the sequecer, even #'ed blocks only |
| clocksource | logic::clocksource::        | select clock source, if not async.          |
| truth       | uint8_t                     | truth table                                 |

Compatibility note: Since the version released with metaTinyCore 2.6.1 and DxCore 1.5.0, all the namespaces for the enumerated types are within the `logic::` namespace, as otherwise conflicts arose with other libraries that used namespaces of the same names.

| Methods             | Function
|---------------------|-----------------------------------------------------------------------------|
| Logic::start();     | Enables CCL with current configuration.                                     |
| Logic::stop();      | Disables CCL - must be disabled to change configuration                     |
| init();             | Write settings for this logic block to registers. CCL must be stopped first |
| attachInterrupt();  | Attach an interrupt on the CCL, supports RISING/FALLING/CHANGE              |
| detachInterrupt();  | Detach the currently attached interrupt.                                    |

The correct order for for initialization is:
1. Set the properties as required for your application.
2. Attach any interrupts if using attachInterrupt. Note that interrupts may also be defined manually - if this is done, you must avoid calling LogicN.attachInterrupt and LogicN.detachInterrupt on any logic block, and write to the appriopriate registers to enable them. Interrupts defined this way, particularly if short and simple, will execute much faster. Refer to the datasheet for more information.
2. call `LogicN.init()` to write those properties to the CCL registers.

The correct proceedure for modifying the configuration of one or more logic blocks:
1. Set the properties as required for all logic blocks being changed.
2. Call `Logic::stop()`.
3. Call `LogicN.init()` on all logic blocks that have been changed.
3. call `Logic::start()` to restart the CCL as a whole.

## Properties

### enable
Property controlling whether the logic block is enabled. Like all properties, you must call LogicN.init() to apply any changes.
Accepted values:
```c++
true;  // Enable the current logic block
false; // Disable the current logic block
```

#### Usage
```c++
Logic0.enable = true; // Enable logic block 0
```

#### Default state
`Logic0.enable` defaults to `false` if not specified in the user program.


### input0..input2
Properties setting input sources for the three inputs of this logic block.

General notes:
* Timer/Counter input sources are associated with a WO (Waveform Output) channel - they are logic 1 (true) when the PWM output on that channel is `HIGH` (See the datasheet I/O multiplexed signals chart to associate WO channels with pins). This is in contrast to the timer match *event* channels. Those are kind of lame, because they only give you a single clock cycle pulse instead (that is, EVSYS gives you pulse inputs, while CCL gives you level inputs)
* The point of `in::pin` to enable you to use a pin which is configured as an **output** too - either as a way to manually switch behavior, or when a pin is being controlled by a peripheral that is not directly accessible as an input. There are only a few cases like this that apply to tinyAVR parts (USART XDIR, TCA WO3-5 in split mode) and anything TWI come to mind)
* Logic blocks come in pairs. This matters greatly for the Feedback output, which takes the output of the even logic blocks in the pair. Hence, for each pair:
  * The evenlogic block can use it's own output, or the output of the odd logic block as inputs without issue.
  * The odd logic block can use the output of the even logic block, or the output of the even logic block in the next pair.
  * The odd logic block must be set to use an event channel and the event channel pointed at it's own output in order to use it's output as an input.
  * If the sequencer is enabled, for the purposes of pin output, and the `logic::in::feedback` option and likely event generation, it replaces the output of the even logic block
  * If the sequencer for the pair consisting of logic block 2 and 3 is used, it is not clear whether the link input from logic block 1 will get the sequencce output or the logic block output before the sequencer.

#### Accepted values for Dx-series and EA-series parts

``` c++
logic::in::masked;           // Pin not in use
logic::in::unused;           // Synonym for masked
logic::in::disable;          // Synonym for masked
logic::in::feedback;         // Connect output of even logic block in this pair or the sequencer if in use, to this input
logic::in::link;             // Connect output of next logic block to this input **Errata Warning**
logic::in::event_a;          // Connect input to event a (preferred)
logic::in::event_0;          // Connect input to event a
logic::in::event_b;          // Connect input to event b (preferred)
logic::in::event_1;          // Connect input to event b
logic::in::pin;              // Connect input to CCL IN0, IN1, or IN2 - and do not change pinMode (imagine using an OUTPUT to switch modes)
logic::in::input_pullup;     // Connect input to CCL IN0, IN1, or IN2 - and set input, pullup on
logic::in::input_no_pullup;  // Connect input to CCL IN0, IN1, or IN2 - and set input, pullup off
logic::in::input;            // Synonym for input_no_pullup
logic::in::ac;               // Connect input to AC0-2 for input 0-2. If a part does not have all 3 ACs, only inputs from ACs present on the chip can be used.
logic::in::tcb;              // Connect to TCB0-2 output for input 0-2.
logic::in::tca0;             // Connect input to TCA0 WO0~2 for input 0~2
logic::in::tca1;             // Connect input to TCA1 WO0~2 for input 0~2 - unavailable on parts without a second TCA1
logic::in::tca;              // Synonym for tca0 (for compatibility with code written for tinyAVRs and megaAVR 0-series)
logic::in::tcd;              // Connect input to TCD0 WOA, WOB, WOC (for input 0~2). Unavailable on EA-series as they don't have a TCD.
logic::in::usart;            // Connect input to TXD of USART 0~2 (for input 0~2). On parts with 2 USARTS, only works on inputs 0 and 1.
logic::in::spi;              // Connect input to SPI0 MOSI, MOSI, SCK (for input 0~2)
```
Notes specific to Dx-series:
* Notice that only SPI0 can be used. SPI input is supported in master mode only.
* Notice that only TCB0-2, USART 0-2, and TCA0/1 WO0-2 can be used in this way.
* logic:in:tcb uses outputs the level that the pin would have whether or not the pin is enabled. It only works in single shot and 8-bit pwm mode, as the others do not use the output pin.
* That the instance of the peripheral used is equal to the input number for ACs, TCBs, and USARTs, and requirement to use a certain input to get certain signals from SPI1 may become problematic when using more than one such peripheral. The same is true to a lesser extent of the WOA-C, WO0-2 <-> input number correlation.
  * This means that depending on your needs, your choice of inputs may be dictated by the
* TCD0 *WOC is always set by the core to WOA*, not WOB. Changing the relevant register manually will break analogWrite() on the corresponding pins if takeOverTCD0() has not been called. TCD only has two outputs; WOA and WOB - WOC and WOD just mirror one of the others to allow more freedom choosing pins
* **Errata warning** If input on the highest-number Logic is set to link, it will use the output of Logic0 **unless** the part is a 28 or 32 pin part, in which case, depending on the silicon rev, series, and flash size, it may not receive anything (presumable because of a mistake when scaling down the number of LUTs for the lower pincounts.)

#### Accepted values for tinyAVR 0/1-series parts

``` c++
logic::in::masked;           // Pin not in use
logic::in::unused;           // Synonym for masked
logic::in::disable;          // Synonym for masked
logic::in::feedback;         // Connect output of even logic block in this pair or the sequencer if in use, to this input
logic::in::link;             // Connect output of the next logic block to this input. Errata warning!
logic::in::event_a;          // Connect input to event a (preferred)
logic::in::event_0;          // Connect input to event a
logic::in::event_b;          // Connect input to event b (preferred)
logic::in::event_1;          // Connect input to event b
logic::in::pin;              // Connect input to CCL IN0, IN1, or IN2 - and do not change pinMode (imagine using an OUTPUT to switch modes)
logic::in::input_pullup;     // Connect input to CCL IN0, IN1, or IN2 - and set input, pullup on
logic::in::input_no_pullup;  // Connect input to CCL IN0, IN1, or IN2 - and set input, pullup off
logic::in::input;            // Synonym for input_no_pullup
logic::in::ac;               // Synonym for ac0 - Compatibility warning
logic::in::ac0;              // Connect input to the output of analog comparator (AC0) - Compatibility warning
logic::in::tcb0;             // Connect input to TCB0 WO - Compatibility warning
logic::in::tcb;              // Synonym for tcb0 - on other parts, there is only one TCB input option, and in0 is TCB0, in1 TCB1 etc.
logic::in::tca0;             // Connect input to TCA0 WO0~2 for input 0~2
logic::in::tca;              // Synonym for tca0
logic::in::tcd;              // Connect input to TCD0 WOA, WOB, WOA for input 0, 1, 2
logic::in::usart;            // Connect input to USART0 XCK and TXD for input 0, 1 - on other parts only TX is accessible this way, and input number = USART number.
logic::in::spi;              // Connect input to SPI0 SCK, MOSI, MISO for input 0, 1, 2 - Compatibility warning
logic::in::ac1;              // Connect input to AC1 OUT (input 0, 1 on parts with 3 ACs only - Compatibility warning
logic::in::ac2;              // Connect input to AC2 OUT (input 0, 1 on parts with 3 ACs only) - Compatibility warning
logic::in::tcb1;             // Connect input to TCB1 WO (input 0, 1 on parts with 2 TCBs only) - Compatibility warning
```

Notes specific to ATtiny 0/1-series:
* Not all inputs are available on all parts - only input sources corresponding to peripherals on that device are available. Not all options are available for input 2, as noted above.
* The CCL on the 0/1-series is different from every other part with a CCL. Options which are different are marked with **WEIRD** - whenever these options are used, if code will be released publicly or you may some day want to port it to other parts, it is suggested to include comments describing exactly what it's doing. See the tinyAVR 2-series for the "normal" version of these input options.
* If you need feedback input from an odd-numbered logic block, use the event system for that.
* Not all pin inputs are available on all parts (see table above). The event system can be used to bring in input from other pins.
* CCL0's IN0 pin is on PA0, which is nominally the UPDI pin. This may limit the usefulness of CCL0 on the ATtiny parts (though it may work as long as the input cannot be mistaken for a UPDI activation command); configuring UPDI as GPIO prevents further programming via UPDI except via HV programming. One can always use the event system to substitute another input for IN0; This is demonstrated in the three input example.
* Only the ATtiny1614, 1616, 1617, 3216, and 3217 have TCB1, AC1, and AC2.
* **Errata warning** Many parts in circulation are impacted by an errata (though not all - some never had it, while the 32k parts have gotten a die rev that fixes it. On effected parts, the link input does not work unless pin output of the other logic block is enabled. Check the applicable errata and datasheet clarification document from Microchip to see if your part is impacted.
* **Coompatibility warning** These were the first AVRs with CCL. They made some decisions that they realized weren't such a good idea afterall and changed for parts released more recently. Most importantly, for TCBs and ACs, as well as USARTs, the peripheral number used is the input number. SPI on these parts makes MISO available supposedly. Later parts do not. Later parts also only make USART TXD available - though you can get XCK from the event system.
* USART option will use XCK on input 0, TXD on input 1, and is not valid for input 2.
  * MISO is (supposedly) available as an input when SPI is used as the input.
  * Parts with two TCBs or three ACs give them their own channel on 0/1-series. Obviously there wouldn't be enough channels for this if it were done on a Dx with 5 TCBs, and 3 ACs. The other parts that have multiples of these (2-series and Dx) have one channel for this, and the input number selects which one is used, but only the first two can be used.
* The tinyAVR 0/1-series datasheets refer to the event channels as 0 and 1. On all subsequent parts, they are referred to as A and B. The Logic library always accepts both, though we recommend using event_a/event_b everywhere, as it is clear that that is the convention that Microchip chose to settle on.

#### Accepted values for tinyAVR 2-series
``` c++
logic::in::masked;           // Pin not in use
logic::in::unused;           // Pin not in use
logic::in::disable;          // Pin not in use
logic::in::feedback;         // Connect output of even logic block in this pair or the sequencer if in use, to this input
logic::in::link;             // Connect output of logic block n+1 to this input, or block 0 for the last block.
logic::in::event_a;          // Connect input to event a (preferred)
logic::in::event_0;          // Connect input to event a
logic::in::event_b;          // Connect input to event b (preferred)
logic::in::event_1;          // Connect input to event b
logic::in::pin;              // Connect input to CCL IN0, IN1, or IN2 - and do not change pinMode
logic::in::input_pullup;     // Connect input to CCL IN0, IN1, or IN2 - and set input, pullup on
logic::in::input_no_pullup;  // Connect input to CCL IN0, IN1, or IN2 - and set input, pullup off
logic::in::input;            // Synonym for input_no_pullup
logic::in::ac;               // Connect input to the output of the internal analog comparator AC0.
logic::in::uart;             // Connect input to UART TX. Input 0 connects to UART0 TX, input 1 to UART1 TX. Not available on input 2. XCK is not available (though it can be accessed via the event system)
logic::in::spi;              // Connect input to SPI. Input 0 and 1 connect to MOSI, and input 2 connects to SCK. MISO not available.
logic::in::tca0;             // Connect input to TCA0. Input 0 connects to WO0, input 1 to WO1 and input2 to WO2
logic::in::tca;              // Synonym for tca0
logic::in::tcb;              // Connect to TCB output (this is HIGH when the output pin would be (though output need not be enabled) in single shot and PWM modes. TCB used has same number as input number (only TCB0-2 can be used).
```

Notes for tinyAVR 2-series:
* SPI input is supported in master mode only.
* If input on the highest-number Logic3 is set to link, it will use the output of Logic0
* If you need to link input to logic block other than block N+1, or if you need feedback output from the an odd-numbered logic block, use the event system.
* Not all pin inputs are available on all parts (see table above). The event system can be used to bring in input from other pins.

#### Usage
``` c++
Logic0.input0 = logic::in::link;         // Connect output from block 1 to input 0 of block 0
Logic0.input1 = logic::in::input;        // Connect the input 1 from block 0 to its GPIO
Logic0.input2 = logic::in::input_pullup; // Connect the input 2 from block 0 to its GPIO, with pullup on
```

#### Default state
`LogicN.inputN` defaults to `logic::in::unused` if not specified in the user program.


### output
Property controlling the logic block output pin behavior. Note that the output of the logic block still can be used internally if the output pin is disabled (though on some tinyAVR 0/1-series silicon, this doesn't work, see the errata for the parts you are working with). The pin's direction and output value are overridden, so you do not need to set the pin `OUTPUT` first.
Accepted values:
```c++
logic::out::disable; // Disable the output GPIO pin. Useful when triggering an interrupt instead.
logic::out::enable;  // Enable the output GPIO pin
```

#### Usage
```c++
Logic0.output = logic::out::disable; // Disable the output GPIO pin.
```

#### Default state
`LogicN.output` defaults to `logic::out::disable` if not specified in the user program.


### output_swap
Property controlling whether to use the default or alternate output pin. No effect if output = logic::out::disable. See the pinout diagrams in the [core this is part of](../../../README.md) or the datasheet for more information.
Accepted values:
```c++
logic::out::no_swap;  // Use default pin position, pin 3 on the port
logic::out::pin_swap; // Use alternative position, pin 6 on the port
```

#### Usage
```c++
Logic0.output_swap = logic::out::no_swap; // No pin swap for output of block0
```

#### Default state
`LogicN.output_swap` defaults to `logic::out::no_swap` if not specified in the user program.


### filter
Property to control whether the output is passed through a filter or synchronizer. Useful when multiple logic blocks are connected internally to prevent race conditions and glitches that could arise due to the asynchronous nature of CCL. Alternately, the delay itself may be desirable, or it can be combined with a configuration which inverts it's own output (and would otherwise oscillate asynchronously), but is clocked by some other source; this will then act to divide that clock speed by 4 (synchronizer) or 8 (filter).
Accepted values:
```c++
logic::filter::disable;      // No filter used, asynchronous output.
logic::filter::synchronizer; // Connect synchronizer to output; delays output by 2 clock cycles.
logic::filter::synch;        // Syntactic sugar for synchronizer
logic::filter::sync;         // Syntactic sugar for synchronizer
logic::filter::filter;       // Connect filter to output; delays output by 4 clock cycles, only passes output that is stable for >2 clock cycles.
```

#### Usage
```c++
Logic0.filter = logic::filter::filter; // Enable filter on output of block 0
```

See also [Prescaling Clocks with CCLs](https://github.com/SpenceKonde/AVR-Guidance/blob/master/CCL_EVSYS_hacks/CCL_prescaling.md)

#### Default state
`LogicN.filter` defaults to `logic::filter::disable` if not specified in the user program.


### clocksource
Property to set the clock source for the logic block; this is used for the synchronizer, filter, edge detector, and sequential logic that uses a clock (otherwise, the logic blocks are asynchronous - and shockingly fast. If you rig them up so that they oscillate (ie set the input to feedback, and output the opposite of the input), it can reach upwards of 100 MHz!). Note that 32kHz-derived and unprescaled clock options are not available on 0-series and 1-series parts; keep this in mind if backwards portability is important. If sequential logic is used, it is clocked from the clock source used by the even-numbered logic block, assuming it uses any clock at all.
Accepted values:
```c++
logic::clocksource::clk_per;      // Clock from the peripheral clock (ie, system clock)
logic::clocksource::in2;          // Clock from the selected input2; it is treated as a 0 in the truth table.
logic::clocksource::oschf;        // Clock from the **unprescaled** internal HF oscillator. Same as clk_per if system clock not prescaled. tinyAVR 2-series (and Dx-series)
logic::clocksource::osc32k;       // Clock from the internal 32.768 kHz oscillator - tinyAVR 2-series only (and Dx-series)
logic::clocksource::osc1k;        // Clock from the internal 32.768 kHz oscillator prescaled by 32 - tinyAVR 2-series only (and Dx-series)
```


#### Usage
```c++
Logic2.clocksource = logic::clocksource::oschf; // Set block 2 to use unprescaled high frequency internal oscillator.
```

#### Default state
`LogicN.clocksource` defaults to `logic::clocksource::clk_per` if not specified in the user program.



### edgedetect
Property to enable the edge detector. The edge detector can be used to generate a pulse when detecting a rising edge on its input. To detect a falling edge, the TRUTH table should be programmed to provide inverted output. "In order to avoid unpredictable behavior, a valid filter option must be enabled" (note: that's what the datasheet says; it's not clear whether you can get the unpredictable behavior, or if the edge detecter won't be connected unless a filter or synchronizer is enabled). Note that the edge detector is likely only of use when the output is being used for sequential logic or as the input to another logic block; it looks particularly useful on the odd LUT input to a J-K flip-flop sequential logic unit.

```c++
logic::edgedetect::disable;      // No edge detection used
logic::edgedetect::enable;       // Edge detection used
```

### sequencer
Property controlling the "sequencer" for this pair of logic blocks - these are latches or flip-flops which will remember their state. There is 1 sequencer per 2 logic blocks; each logic block controls one of the two inputs to a flip flop or latch; *this property is ignored for the odd-numbered logic blocks*. Flip-flops are clocked from the same clock source as the even logic block. Latches are asynchronous. **The output of the sequencer will for all purposes replace the output of the even logic block**

Accepted values:
```c++
logic::sequencer::disable;      // No sequencer connected
logic::sequencer::d_flip_flop;  // D flip flop sequencer connected
logic::sequencer::jk_flip_flop; // JK flip flop sequencer connected
logic::sequencer::d_latch;      // Gated D latch sequencer connected - note that on most tinyAVR 0/1-series parts, this doesn't work. See the Errata.
logic::sequencer::rs_latch;     // RS latch sequencer connected
```

Note that it is possible to use the `feedback` input to in many cases obtain similar behavior without using two logic blocks.

#### Usage
```c++
Logic0.sequencer = logic::sequencer::disable; // Disable sequencer
```

#### Default state
`LogicN.sequencer` defaults to `logic::sequencer::disable` if not specified in the user program.


### truth
This property contains the 8-bit truth table value.
Accepted values between 0x00 and 0xFF - this is where the input values are looked up to determine what value to output.

In2:in0 are treated as a 3-bit number (so 0-7), that bit number (starting from 0) determines the output value.

Ex: If in1 and in0 are high, and in2 is low, input is 3, (0b011 = 3). If the truth table is 0x89 - `0b10001001` - then the output will be high (0b1000*1*001). Put another way, the output it high if `truth & (1 << input)` is true.

During development, it is often ~helpful~ necessary to draw out a table like:

| IN2 | IN1 | IN0 | OUTPUT |
|-----|-----|-----|--------|
|   0 |   0 |   0 |      0 |
|   0 |   0 |   1 |      0 |
|   0 |   1 |   0 |      1 |
|   0 |   1 |   1 |      0 |
|   1 |   0 |   0 |      1 |
|   1 |   0 |   1 |      0 |
|   1 |   1 |   0 |      1 |
|   1 |   1 |   1 |      1 |
Which would translate into a truth value of 0b11010100 or 0xD4.


#### Usage
```c++
Logic0.truth = 0xF0;
```

#### Default state
`LogicN.truth` defaults to `0x00` if not specified in the user program.

## Logic Methods

### init()
Method for initializing a logic block; the settings you have previously configured will be applied and pins configured as requested at this time only.

#### Usage
```c++
Logic0.init(); // Initialize block 0
Logic1.init(); // Initialize block 1
```


### start()
Static method for starting the CCL hardware after desired blocks have been initialized using `LogicN.init()`. See the section below on reconfiguring.

#### Usage
```c++
Logic::start(); // Start CCL hardware
```

### stop()
Static method for stopping the CCL hardware, for example to reconfigure the logic blocks.

#### Usage
```c++
Logic::stop(); // Stop CCL
```

### attachInterrupt()
Method for enabling interrupts for a specific block.
Valid arguments for the third parameters are `RISING`, `FALLING` and `CHANGE`.
This method isn't available on tinyAVR 0/1-series as these parts cannot generate an interrupt from the CCL blocks.
All forms of attachInterrupt, everywhere, are fundamentally evil, because they add a several microsecond overhead to the ISR simply because there is a call to a non-inlinable function;

#### Usage
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

#### Usage
```c++
Logic0.detachInterrupt(); // Disable interrupts for block 0
```

## Reconfiguring
There are TWO levels of "enable protection" on the CCL hardware. According to the Silicon Errata, only one of these is intended. As always, it's anyone's guess when or if this issue will be corrected in a future silicon rev, and if so, on which parts (it would appear that Microchip only became aware of the issue after the Dx-series parts were released - although it impacts all presently available parts, it is only listed in errata updated since mid-2020). Users are advised to proceed with use of workarounds, rather than delay work in the hopes of corrected silicon. The intended enable-protection is that a given logic block cannot be reconfigured while enabled. This is handled by `init()` - you can write your new setting to a logic block, call `LogicN.init()` and it will briefly disable the logic block, make the changes, and re-enable it.

The unintended layer is that no logic block can be reconfigured without also disabling the whole CCL system. Changes can be freely made to the `Logic` classes, however, only the `init()` method will apply those changes, and you must call `Logic::stop()` before calling `init()`, and `Logic::start()` afterwards. If/when parts become available where this is not necessary, this step may be omitted, and this library may be amended to provide a way to check.

### Example
```c++

// Imagine there's some code above this that configured and enabled Logic0.

Logic1.truth=0x55;      // new truth table
Logic1.input2=tca0;     // and different input 2
Logic1.enabled=true;    // enable another LUT
Logic0.truth=0x79;      // changed truth table for LUT0

// At this point Logic1 not enabled, and logic0 is still using the old settings

Logic::stop();  // have to turn off Logic0 too, even though I might not want to
Logic1.init();  // apply changes to logic block 1
Logic3.init();  // apply settings to logic block 3 for the first time
Logic::start(); // re-enable


```

## Think outside the box
To consider the CCL system as simply a built-in multifunction gate IC is to greatly undersell it. The true power of the CCL is in it's ability to use events directly, and to take inputs from almost everything. Even doing neat stuff like the above 0xD4 truth table on an even-numbered logic block with input 2 set to feedback to make an R/S latch without using the second logic block is only scratching the surface of what these can do! Taking that a step farther... you could then use the odd-numbered logic block with that same feedback to, say, switch between two waveforms being output by one of the PWM timers... see the [Tricks and Tips page](Tricks_and_Tips.md)

## Note on terminology
Yes, technically, C++ doesn't have "properties" or "methods" - these are "member variables" and "member functions" in C++ parlance. They mean the same thing. I've chosen to use the more familiar, preseent day terminology.
