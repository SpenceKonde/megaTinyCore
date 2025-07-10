# Logic
A library for interfacing with the CCL (Configurable Custom Logic) peripherals on modern AVR MCUs.
Developed by MCUdude for use with [MegaCoreX](https://github.com/MCUdude/MegaCoreX), adapted to tinyAVR 0/1-series parts by [Tadashi G. Takaoka](https://github.com/tgtakaoka), and to 2-series and Dx-series, Ex-series, and SD-series by [Spence Konde](https://github.com/SpenceKonde)
Correction of several issues on the tinyAVR parts and adaptation of examples by [Spence Konde](https://github.com/SpenceKonde).

All of the examples assume the use of megaTinyCore, MegaCoreX or DxCore as appropriate (code is generally directly portable between megaAVR 0-series, tinyAVR 2-series and AVR Dx-series parts, minor modifications may be required for porting to or from tinyAVR 0/1-series parts).

This library is easy to update for more parts as long as they're well behaved. Support for a part family does not imply that a core is available, as adding support for parts to that is more complicated.

More information about CCL can be found in the [Microchip Application Note TB3218](http://ww1.microchip.com/downloads/en/AppNotes/TB3218-Getting-Started-with-CCL-90003218A.pdf) and in the [megaAVR-0 family data sheet](http://ww1.microchip.com/downloads/en/DeviceDoc/megaAVR0-series-Family-Data-Sheet-DS40002015B.pdf) and the datasheet for the part in question.




## Pin availability and Quick Reference (Dx/Ex-series)

Logic Block |  IN0-2  | OUT | ALT OUT | Availability  | Notes:
------------|---------|-----|---------|---------------|-----------------------------
Logic0      | PA0-PA2 | PA3 |     PA6 |    All parts  |
Logic1      | PC0-PC2 | PC3 |     PC6 |    All parts  |
Logic2      | PD0-PD2 | PD3 |     PD6 |    All parts  | There is no PD0, hence no input 0 on any DD, 28 pin DB, or 32-pin DB
Logic3      | PF0-PF2 | PF3 |     --- |    All parts  | Link input broken on most DA/DB parts
Logic4      | PB0-PB2 | PB3 |     PB6 | 48+ pin DA/DB |
Logic5      | PC0-PC2 | PC3 |     PG6 | 48+ pin DA/DB |

As you can see, things are fairly straightforward, and exactly what you would expect from the pinout. Not all CCL LUTs have inputs on lower pincounts, same with outputs. No allowance is made for peculiarities of a part regarding it's pin mappings. I think Microchip came in, looked at the pinout of their last half dozen AVR releases, finding no commonality of port functionality, peripheral-to-pin mapping, or whathaveyou, swept them off the table in disgust

Logic Block |  14-pin DD            |  20-pin DD          |  28 pin DA          |  32-pin DA          |  48-pin DA/B, EA    |  64-pin DA/B        | DB, DD 28, 32 pin | DU            | EA               | EB                | SD
------------|-----------------------|---------------------|---------------------|---------------------|---------------------|---------------------|-------------------|---------------|------------------|-------------------|------------------|
Logic0 IN   | IN0/1 if no xtal used | No IN0/1 if HF xtal | All                 | All                 | All                 | All                 | All per DA,       | As DD or DA   | As DA except,    | As DU, except:    | As DD/DB         |
Logic0 OUT  | None                  | YES, Both           | YES, both           | YES, both           | YES, both           | YES, both           | except:           | except:       | always 4 LUTs    |                   | always 6 LUTs    |
Logic1 IN   | No IN0 because no PC0 | No IN0 - no PC0     | YES, all            | YES, all            | YES, all            | YES, all            |                   | None          |                  | CCL1 In avail.    |                  |
Logic1 OUT  | No alt output         | No alt output       | No alt output       | No alt output       | YES, both           | YES, both           |                   | No alt output |                  |                   |                  |
Logic2 IN   | None                  | None                | All                 | All                 | YES, all            | YES, all            | No PD0            | PD0 if > 28pin|                  |                   |                  |
Logic2 OUT  | Alt only              | Alt only            | All                 | All                 | All                 | All                 |                   |               |                  |                   |                  |
Logic3 IN   | None                  | None                | IN0, IN1 only       | YES, all            | YES, all            | YES, all            |                   |               |                  |                   |                  |
Logic3 OUT  | None                  | None                | None                | No alt output       | No alt output       | No alt output       |                   |               |                  |                   |                  |
Logic4 IN   | Not present on DD     | Not present on DD   | Not present on 28pin| Not present on 32pin| YES, all(DA/DB only)| YES, all            |                   |               |                  |                   |                  |
Logic4 OUT  | Not present on DD     | Not present on DD   | Not present on 28pin| Not present on 32pin| No alt output       | YES, both           |                   |               |                  |                   |                  |
Logic5 IN   | Not present on DD     | Not present on DD   | Not present on 28pin| Not present on 32pin| NO (no PORTG)       | YES, all            |                   |               |                  |                   |                  |
Logic5 Out  | Not present on DD     | Not present on DD   | Not present on 28pin| Not present on 32pin| NO (no PORTG)       | YES, both           |                   |               |                  |                   |                  |

Notice how logic block 2 and 3 on 14/20 pin DD and logic block 5 on 48-pin parts have no dedicated pins. If you are not using pin input or output, use these logic blocks to conserve the more useful ones when not precluded by other considerations.


## Pin availability and Quick Reference (tinyAVR)
This was simpler with 0/1-series (22 i/o pins, 10 CCL functions, none stacked onto the same pins), but with 2-series, 20 different CCL functions, crammed into just 22 GPIO pins, two of which have one blocks' input and one of the outputs for another. The ones with no inputs on lower pincount parts are more useful than one might expect; advanced use cases will often use mostly internal inputs - and the event system can always be used to get pin input in anyway - albeit at a cost of an event channel. If you do not require pin inputs, these should be used in preference to the logic blocks that do have them all else being equal

Logic Block |  IN0-2  | OUT | 8pin | ALT OUT | Availability |
------------|---------|-----|------|---------|--------------|
Logic0      | PA0-PA2 | PA4 |  PA6 |     PB4 |    All parts |
Logic1      | PC3-PC5 | PA7 |  PA7 |     PC1 |    All parts |
Logic2      | PB0-PB2 | PB3 |  --- |     PB6 |     2-series |
Logic3      | PC0-PC2 | PC4 |  --- |     PA5 |     2-series |

* Inputs are always in consecutive order counting by bit position (not physical order).
* All logic blocks present *on tinyAVR* always have an output pin, but may not have input pins. *This is not true of Dx and Ex-series parts* which bind each LUT to a port - they gain in consistency, but sacrifice coherence and effectiveness ("Why do we have 3 logic blocks with no output pins and all these pins with hardly any special functions?" versus "How did they choose the pin mapping for this thing, by throwing darts at a diagam? There's no rhyme or reason to any of it!")
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
Note that there exist reserved and invalid values for many of the bitfields controlled by the properties of the logic block, as well as reserved bits with no known or documented function. This library does not support specifying these. No comprehensive investigation has been undertaken to exclude the existence of of hidden and/or broken features, nor is there any reason to expect that such... just clear evidence of a pattern of features getting "airbrushed out" of the documentation without removal from the silicon. sometimes this is probably dictated by expedience alone. There addresses of registers

*A discovery of an undocumented or reserved option which has been found to do something potentially useful gives the usual (or it would be usual, except that there are essen) reward for undocumented features: an assembled breakout board featuring an impacted part (of your choice), or a quantity of bare boards of your choice from my [tindie store](https://tindie.com/stores/drazzy) of equal value.* See details at [https://github/SpenceKonde/AVR_Research/blob/master/Bounties.md](https://github.com/SpenceKonde/AVR_Research/blob/main/Bounties.md)


### Overhead
* On the 0/1-series, the overhead is approximately 546 bytes of flash and 26 bytes of RAM.
* On the 2-series, with twice as many LUTs, it is much larger: 984 bytes and 60b RAM.
* This is fairly small for 16/32k parts - it cannot be ignored on a 4k part, particularly not a 4k 2-series, but it it isn't an unreasonable amount of flash for the parts most people will be using. As of 2.6.0 if no ISR is attached or detached, that overhead will not be pulled in, allowing a more performant manually defined interrupt to be written by the user (any "attach" type thing costs around 50 clock cycles)
* These size concerns are not much of a problem on most Dx-series parts.
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
| clocksource | logic::clocksource::        | select clock source for applicable functions|
| truth       | uint8_t                     | truth table                                 |

Compatibility note: Since the version released with metaTinyCore 2.6.1 and DxCore 1.5.0, all the namespaces for the enumerated types are within the `logic::` namespace, as otherwise conflicts arose with other libraries that used namespaces of the same names.

| Methods             | Function
|---------------------|-----------------------------------------------------------------------------|
| Logic::start();     | Enables CCL with current configuration.                                     |
| Logic::stop();      | Disables CCL - must be disabled to change configuration                     |
| init();             | Write settings for this logic block to registers. CCL must be stopped first |
| attachInterrupt();  | Attach an interrupt on the CCL, supports RISING/FALLING/CHANGE              |
| detachInterrupt();  | Detach the currently attached interrupt.                                    |

### The correct order for for initialization is... that depends

There is an erratum that cannot be papered over:

**Intended Behavior:** When a LUT is enabled, and the CCL in general is enabled, the settings for that LUT cannot be changed and one of those must be disabled before it is reconfigured.

In this case, no special developer action is needed. Init knows that it needs to turn the LUT off if it's enabled. You may use `logic::start()` a single time in setup() and never call `logic::stop()`, and reconfigure freely.

**Errata-impacted Behavior:** When a LUT is enabled *OR* the CCL in general is enabled, the settings for that LUT cannot be  changed. Thus, to reconfigure, you set up all the new properties, then you must disable the whole CCL. Then you can call the then use the `LogicN.init()` to write the new configuration. Then you call `Logic::start()` to turn the CCL back on.


(in both cases, properties of LogicN objects are assumed to have been set)
```c++
// first time setup, with erratum:
Logic0.init();
Logic1.init();
Logic::start(); //must be after all inits


//Reconfigure Logic2:
Logic::stop(); // Logic0 and Logic1 do not run while applying changes to Logic2.
Logic2.init();
Logic::start();
```
```c++
// first time setup, without erratum:
Logic0.init();
Logic1.init();
Logic::start(); //Can be aanywhere depending on your needs.


//Reconfigure Logic2:
Logic2.init(); // Logic0 and Logic1 do run while applying changes to Logic2.
```


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
* The point of `in::pin` to enable you to use a pin which is configured as an **output** too - either as a way to manually switch behavior, or when a pin is being controlled by a peripheral that is not directly accessible as an input. There are only a few cases like this that apply to tinyAVR parts (USART XDIR, TCA WO3-5 in split mode) and anything TWI come to mind, and the last one is a real stretch. )
* Logic blocks come in pairs. This matters greatly for the Feedback output, which takes the output of the even logic blocks in the pair.

| Option      |  EVEN   |  ODD    | SEQ in use
|-------------|---------|---------|------------------
|in::link     | Out n+1 | Out n+1 | Out seq(n,n+1)
|in::feedback | Out     | Out n-1 | Out seq(n-1,n)

* The even logic block can use it's own output, or the output of the odd logic block as inputs without issue.
* The odd logic block can use the output of the even logic block, or the output of the even logic block in the next pair.
* The odd logic block must be set to use an event channel and the event channel pointed at it's own output in order to use it's output as an input.
* If the sequencer is enabled, for the purposes of pin output, and the `logic::in::feedback` option and likely event generation, it replaces the output of the even logic block
* If the sequencer for the pair consisting of logic block 2 and 3 is used, it is not clear whether the link input from logic block 1 will get the sequencce output or the logic block output before the sequencer.


#### Accepted values for tinyAVR 0/1-series parts

``` c++
logic::in::masked;           // Input not in use (for TRUTH, input treated as always 0)
logic::in::unused;           // Synonym for masked
logic::in::disable;          // Synonym for masked
logic::in::feedback;         // Receive input from output of EVEN logic block in this pair if sequencer not in use, or the sequencer if it is.
logic::in::link;             // Connect output of the next logic block to this input. Errata warning: almost all tinyAVR 0/1 parts require pin output enabled for this to work.
logic::in::event_a;          // Connect input to event a
logic::in::event_0;          // Deprecated, syn. event a
logic::in::event_b;          // Connect input to event b
logic::in::event_1;          // Deprecated, syn. event b
logic::in::pin;              // Receive input from CCL IN0, IN1, or IN2 - and do not change pinMode.
logic::in::input_pullup;     // Receive input from CCL IN0, IN1, or IN2 - and set input, pullup on.
logic::in::input_no_pullup;  // Receive input from CCL IN0, IN1, or IN2 - and set input, pullup off.
logic::in::input;            // Synonym for input_no_pullup
logic::in::ac;               // Synonym for ac0 - deprecated on 1-series tinyAVRs with more than one AC.
logic::in::ac0;              // Receive input from AC0
logic::in::tcb0;             // Receive input from TCB0 - Compatibility Warning
logic::in::tcb;              // Synonym for tcb0 - Compatibility warning
logic::in::tca0;             // Receive input from TCA0 WO0~2 for input 0~2. WO3-5 are not available.
logic::in::tca;              // Synonym for tca0 Not recommended for new code.
logic::in::tcd0;             // Receive input from TCD0 WOA, WOB, WOA for input 0, 1, 2 - 1-series only
logic::in::tcd;              // Synonym for tcd0 Not recommended for new code.
logic::in::usart;            // Receive input from USART0 XCK = IN0, TX = IN1 - On all other parts, TX of USARTn is available on INn
logic::in::spi;              // Receive input from SPI0 SCK, MOSI, MISO for input 0, 1, 2 respectively. This is very different from other parts.
logic::in::ac1;              // Receive input from AC1 OUT. Parts with 3 AC's only. AC0, AC1, and AC2 are available on all inputs unlike later parts Compatibility warning!
logic::in::ac2;              // Receive input from AC2 OUT. Parts with 3 AC's only. As above. Compatibility warning!
logic::in::tcb1;             // Receive input from TCB1 WO. TCB0 and TCB1 are available on all inputs unlike later parts - Compatibility warning!
```

Notes specific to ATtiny 0/1-series:
* Not all inputs are available on all parts - only input sources corresponding to peripherals on that device are available. Not all options are available for input 2, as noted above.
* The CCL on the 0/1-series is different from every other part with a CCL. Options which are different are marked with a warning. Whenever these options are used in code that will be released publicly or that you may some day want to port it to other parts, it is suggested to include comments describing exactly what it's doing. See the tinyAVR 2-series for the "normal" version of these input options.
* These parts have only two LUTs, thus:
  * LUT0 can use LUT0/SEQ0 output or LUT output (via feedback and link)
  * LUT1 will get LUT0/SEQ0 from both feedback and link
  * Link will not work if output is not enabled due to an erratum that is only corrected on some ATtiny3216/7 parts as of mid 2025.
* Not all pin inputs are available on all parts (see table above), however EVSYS can be used to bring in other pins if need be.
  * CCL0's IN0 pin is on PA0, which is nominally the UPDI pin. This may limit the usefulness of CCL0 on the ATtiny parts, particularly here where there is only a single pair.
* Among tinyAVR 1-series, only the ATtiny1614, 1616, 1617, 3216, and 3217 have TCB1, AC1, and AC2, all others have only AC0 and TCB0. All tiny 2-series have TCB1, and a single AC, AC0.
The two final limits add further chaos to the distributions of numeric values for users and generators - the two seem to have had separate algorithms for determining when to leave holes for absent peripherals, and when to fill them in. They *really* seem to like filling in the holes in the generator lists, leaving me wondering what it is abouut the length of the list with such a high cost...

#### Accepted values for tinyAVR 2-series
``` c++
logic::in::masked;           // Pin not in use
logic::in::unused;           // Pin not in use
logic::in::disable;          // Pin not in use
logic::in::feedback;         // Connect output of even logic block in this pair or the sequencer if in use, to this input
logic::in::link;             // Connect output of logic block n+1 to this input, or block 0 for the last block.
logic::in::event_a;          // Connect input to event a
logic::in::event_0;          // Deprecated, syn. event a
logic::in::event_b;          // Connect input to event 0
logic::in::event_1;          // Deprecated, syn. event b
logic::in::pin;              // Connect input to CCL IN0, IN1, or IN2 - and do not change pinMode
logic::in::input_pullup;     // Connect input to CCL IN0, IN1, or IN2 - and set input, pullup on
logic::in::input_no_pullup;  // Connect input to CCL IN0, IN1, or IN2 - and set input, pullup off
logic::in::input;            // Synonym for input_no_pullup
logic::in::ac;               // Connect input to the output of the internal analog comparator AC0.
logic::in::uart;             // Connect input to UART TX. Input 0 connects to UART0 TX, input 1 to UART1 TX. Not available on input 2. XCK is not available (though it can be accessed via the event system)
logic::in::spi;              // Connect input to SPI. Input 0 and 1 connect to MOSI, and input 2 connects to SCK. MISO not available.
logic::in::tca0;             // Connect input to TCA0. Input 0 connects to WO0, input 1 to WO1 and input2 to WO2. WO3-5 are not available.
logic::in::tca;              // (deprecated) Synonym for tca0; deprecated as parts with multiple TCA instances are available, and these all have their own input options.
logic::in::tcb;              // Connect to TCB output (this is HIGH when the output pin would be (though output need not be enabled) in single shot and PWM modes). Input 0 is TCB0, Input 1 TCB1, Input 2 TCB2.
```

Notes for tinyAVR 2-series:
* If input on the highest-number Logic3 is set to link, it will use the output of Logic0
* If you need to link input to logic block other than block N+1, or if you need feedback output from the an odd-numbered logic block, use the event system.
* Not all pin inputs are available on all parts (see table above). The event system can be used to bring in input from other pins.

#### Accepted values for Dx-series and Ex-series parts

``` c++
logic::in::masked;           // Pin not in use
logic::in::unused;           // Synonym for masked
logic::in::disable;          // Synonym for masked
logic::in::feedback;         // Connect output of even logic block in this pair or the sequencer if in use, to this input
logic::in::link;             // Connect output of next logic block to this input WARNING: There are errata involving this on 28 and 32 pin DA/DB parts.
logic::in::event_a;          // Connect input to event a
logic::in::event_0;          // Deprecated, syn. event a
logic::in::event_b;          // Connect input to event b
logic::in::event_1;          // Deprecated, syn. event b
logic::in::pin;              // Connect input to CCL IN0, IN1, or IN2 - and do not change pinMode. For example, you might leave it output, and toggle the pin to switch modes of your CCL contraption.
logic::in::input_pullup;     // Connect input to CCL IN0, IN1, or IN2 - and set input, pullup on.
logic::in::input_no_pullup;  // Connect input to CCL IN0, IN1, or IN2 - and set input, pullup off - set only when a push-pull output of something is connected.
logic::in::input;            // Synonym for input_no_pullup
logic::in::ac;               // Connect input to one of the analog comparators. See table below.
logic::in::tcb;              // Connect to TCB0-2 output for input 0-2.
logic::in::tca0;             // Connect input to TCA0 WO0~2 for input 0~2
logic::in::tca1;             // Connect input to TCA1 WO0~2 for input 0~2 - only available on parts with a second TCA: 48/64 pin DA/DB, and all EA-series parts.
logic::in::tca;              // Synonym for tca0 (for compatibility with code written for tinyAVRs and megaAVR 0-series). Not recommended for new code.
logic::in::tcd0;             // Connect input to TCD0 WOA, WOB, WOC (for input 0~2). Unavailable on parts which do not have a TCD.
logic::in::tcd;              // Synonym for tcd, as no part has been released with two TCDs. Presently, I question the likelihood of any device ever having a second TCD. Still, not recommended for new code.
logic::in::tce0;             // Connect to WO0 ~ 2 of TCE0. There is no way to get TCE WO3. except through an event channel.
logic::in::tcf0;             // Connect to WO0, WO1, or WO0 of TCF0. No unnumbered tcf provided as I expect a 2x TCF part at some point.
logic::in::usart;            // Connect input to TXD of USART 0~2 (for input 0~2). On parts with 2 USARTS, only works on inputs 0 and 1.
logic::in::spi;              // Connect input to SPI0 MOSI, MOSI, SCK (for input 0~2) - hence SCK goes on the one you can use as the clock, which makes sense.
```
Notes specific to Dx-series:
* Notice that only SPI0 can be used. SPI input is supported in master mode only.
* Notice that only TCB0-2, USART 0-2, and TCA0/1 WO0-2 can be used in this way.
* logic:in:tcb uses outputs the level that the pin would have whether or not the pin is enabled. It only works in single shot and 8-bit pwm mode, as the others do not use the output pin.
* That the instance of the peripheral used is equal to the input number for ACs, TCBs, and USARTs, and requirement to use a certain input to get certain signals from SPI1 may become problematic when using more than one such peripheral. The same is true to a lesser extent of the WOA-C, WO0-2 <-> input number correlation.
  * This means that depending on your needs, your choice of inputs may be dictated by the
* TCD0 *WOC is always set by the core to WOA*, not WOB. Changing the relevant register manually will break analogWrite() on the corresponding pins if takeOverTCD0() has not been called. TCD only has two outputs; WOA and WOB - WOC and WOD just mirror one of the others to allow more freedom choosing pins
* **Errata warning** If input on the highest-number Logic is set to link, it will use the output of Logic0 **unless** the part is a 28 or 32 pin part, in which case, depending on the silicon rev, series, and flash size, it may not receive anything (presumable because of a mistake when scaling down the number of LUTs for the lower pincounts.)



#### Compatibility Warnings and differences between parts
In most cases, we are able to paper over differences between parts transparently. This is one place where they haven't renamed options, but actually removed options that were previously present, such that a small number of CCL configurations that were possible on tiny1616/17 and 32k versions which will need inputs reordered, and in a few cases may be impossible, because a feature (of dubious utility) was removed. I doubt anyone was using it, but, see the tables below.


##### Analog Comparator input
The tiny 0/1, and the single-comparator parts from other series are very simple, all inputs can be connected to AC0 output.
The tiny 1+ parts (16k flash 1-series parts), which get a bushel of extra features (discussed elsewhere, but including three analog comparators) get an option for all three analog comparators on each part.
All other parts with more than one analog comparator expose AC0 output on IN0, AC1 on IN1, and AC2 on IN2 (if they have it), otjherwise either another copy of AC1, or nothing.

Heading | tiny0/1 | tiny1+  | DA/DB   | DD/DU/m0/t2| EA      | EB      | SD       |
--------|---------|---------|---------|------------|---------|---------|----------|
IN0     | ac->AC0 | ac0,1,2 | ac->AC0 | ac->AC0    | ac->AC0 | ac->AC0 | ac->AC0  |
IN1     | ac->AC0 | ac0,1,2 | ac->AC1 | ac->AC0    | ac->AC1 | ac->AC1 | ac->AC1  |
IN2     | ac->AC0 | ac0,1,2 | ac->AC2 | ac->AC0    | rsvd.   | ac->AC1 | ac->AC2  |

##### SPI, USART and TCB are not the same on the tinyAVR 0 and 1
On all parts except tinyAVR 0, 1, and 1+ (the above mentioned enhanced 1-series parts), the TCB, SPI, and USART options act the same way:
* INn gets USARTn TXD. No USARTn XCK. No UARTs higher than 2 can be used.
  * On early-modern tinyAVRs, which had one USART, IN2 is unused, IN1 gives USART0 TXD, and IN0 gives USART0 XCK (clock in synchronous mode)
* INn gets the waveform output of TCBn. No event channels for TCBs beyon the first.
  * 2-series parts do not have a TCB option for IN2, having no TCB2.
  * All other 2-timer parts see TCB1 repeated in IN2.


Heading    | tiny0/1/1+ | Everything else |
-----------|------------|-----------------|
IN0, usart | USART0 XCK | USART0 TXD      |
IN1, usart | USART0 TXD | USART1 TXD *    |
IN2, usart | rsvd.      | USART2 TXD **   |
IN0, spi   | SPI0 SCK   | SPI0 MOSI       |
IN1, spi   | SPI0 MOSI  | SPI0 MOSI       |
IN2, spi   | SPI0 MISO  | SPI0 SCK        |
IN0, tcb   | alias tcb0 | TCB0 WO         |
IN1, tcb   | alias tcb0 | TCB1 WO         |
IN2, tcb   | alias tcb0 | TCB2 WO  **     |
IN0, tcb0  | TCB0 WO    | Use tcb         |
IN1, tcb0  | TCB0 WO    | Not an option   |
IN2, tcb0  | TCB0 WO    | Not an option   |
IN0, tcb1  | TCB1 WO    | Not an option   |
IN1, tcb1  | TCB1 WO    | Use tcb         |
IN2, tcb1  | TCB1 WO    | Not an option   |



`*` Parts like the EB-series, which have only a single UART, have that UART available on all input channels.
`**` tinyAVR 2-series parts have no USART input option on IN2. On all non-tiny devices  devices without USART2, this is USART1, if present, else USART0. On non-tinyAVR parts


#### Input "value"
As shown above not all inputs are equal, particularly on tinyAVR 2-series and Dx/Ex-series, nor are all instances of the peripherals that can generate inputs equal. This effect is most prominent on devices with smaller numbers of pins and peripherals, particularly the DD-series. However, unlike with the event system it is harder to give specific guidance, because the type of inputs needed will vary so much. This challenge manifests most acutely on the 2-series tinyAVR and the DD-series parts with low pincount, which have fewer instances of peripherals (2 TCB, 1 AC, 2 USART). The larger DD's have another TCB, but are still limited to 2 USARTs and 1 AC. On all parts, input 2 is also very special - it's the only input that you can use to clock the filter/synchronizer ande sequential logic - this means that you can't get the SPI SCK input this way unless you're using it as the clock for the logic block (though that's probably the thing one would be most likely to use that for). This constraint only applies to using one of these inputs as the clock - you can used the internal oscillator, 32kHz internal

On the larger DA/DB parts, there is a similar but opposite consideration - USART0-2 and TCB0-2 are more valuable than other instances of the same peripheral - if you are using them as CCL inputs. Because of it's use as the clock input, though USART2, AC2, and TCB2 are marginally less useful.

You may find that your choice of analog comparator or other peripheral is dictated by what other peripherals you need to use. In some cases, you can use an event channel to bring that in. In other cases, you may need to use an additional logic block to grab a key channel, output it unchanged, and bring it in via a link input.


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


### clocksource
Property to set the clock source for the logic block. The CCL itself is asynchronous and combinatorial, however, several accessories do require a clock. These functions are the Synchronizer/Filter, the edge-detector, and the two flip-flop sequential logic options (but not the two latch options). The asence of

Accepted values:
```c++
/* Value:                            Description                                                 Tiny 0/1/1+    EB-series   Everything else
logic::clocksource::clk_per;      // Clock from the peripheral clock (ie, system clock)           Yes           Yes         Yes
logic::clocksource::in2;          // Clock from the selected input2. Igmored in truth table.      Yes           Yes         Yes
logic::clocksource::oschf;        // Clock from the **unprescaled** internal HF oscillator.       No            Yes         Yes
logic::clocksource::osc32k;       // Clock from the internal 32.768 kHz oscillator                No            Yes         Yes
logic::clocksource::osc1k;        // As above but prescaled by 32;                                No            Yes         Yes
logic::clocksource::pll;          // Clock from on-chip PLL                                       No            Yes         No
```


#### Usage
```c++
Logic2.clocksource = logic::clocksource::oschf; // Set block 2 to use unprescaled high frequency internal oscillator.
```

#### Default state
`LogicN.clocksource` defaults to `logic::clocksource::clk_per` if not specified in the user program.


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



### edgedetect
Property to enable the edge detector. The edge detector can be used to generate a pulse when detecting a rising edge on its input. To detect a falling edge, the TRUTH table should be programmed to provide inverted output. "In order to avoid unpredictable behavior, a valid filter option must be enabled" (note: that's what the datasheet says; it's not clear whether you can get the unpredictable behavior, or if the edge detector won't be connected unless a filter or synchronizer is enabled). Note that the edge detector is likely only of use when the output is being used for sequential logic or as the input to another logic block; it looks particularly useful on the odd LUT input to a J-K flip-flop sequential logic unit.

```c++
logic::edgedetect::disable;      // No edge detection used
logic::edgedetect::enable;       // Edge detection used
```

### sequencer
Property controlling the "sequencer" for this pair of logic blocks - these are latches or flip-flops which will remember their state. There is 1 sequencer per 2 logic blocks; each logic block controls one of the two inputs to a flip flop or latch. Under the hood, there is an extra register for each pair of LUTs that controls this - so there are 2 LUTs for every SEQ. The SEQ is always associated more with the EVEN numbered LUTs, the clock source of that LUT will also Flip-flops are clocked from the same clock source as the even logic block. Latches are asynchronous. **The output of the sequencer will for all purposes (other than the signal between the logic block and the sequencer, of course), including link, feedback, pin and event outputs, replace the output of the even logic block**

Accepted values:
```c++
logic::sequencer::disable;      // No sequencer connected
logic::sequencer::d_flip_flop;  // D flip flop sequencer connected
logic::sequencer::jk_flip_flop; // JK flip flop sequencer connected
logic::sequencer::d_latch;      // Gated D latch sequencer connected (broken on many early modern AVRs, see the errata)
logic::sequencer::rs_latch;     // RS latch sequencer connected
```

The available sequencer options, unfortunately, are capable of only slightly moore than what can be done with just an even LUT using the Feedback output, or an odd LUT with an event channel.

Particularly, if you don;t need a logic block to generate the inputs to an RS latch or D latch, you can do it easily:
IN0 and IN1 are the two inputs, while IN2 is feedback (I find this by far the easiest to work with when making truth tables. )

| 2 | 1 | 0 | Out | 2 | 1 | 0 | Out |
|---|---|---|-----|---|---|---|-----|
| 0 | 0 | 0 |  0  | 0 | 0 | 0 |  0  |
| 0 | 0 | 1 |  1  | 0 | 0 | 1 |  0  |
| 0 | 1 | 0 |  0  | 0 | 1 | 0 |  0  |
| 0 | 1 | 1 |  0  | 0 | 1 | 1 |  1  |
| 1 | 0 | 0 |  1  | 1 | 0 | 0 |  1  |
| 1 | 0 | 1 |  1  | 1 | 0 | 1 |  1  |
| 1 | 1 | 0 |  0  | 1 | 1 | 0 |  0  |
| 1 | 1 | 1 |  1  | 1 | 1 | 1 |  1  |

Hence truth tables are 0xB2 and 0xB8, and both of these leave the odd LUT available. Of course, if you need a logic block to come up with the inputs to set and reset, or if you need the clockedness of a flipflop, these are less useful.



#### D Flip-flop
The D-type fiip-flow outputs a 0 or a 1, and retains it's value unless told otherwise. The inputs consist of G and D input. As long as G is low, nothing will change, When G is high, each rising edge will latch the value on the D line to the output. As I understand, D is derived from "Data" and G from "Gate", in the sense that it opens the gate and allows new data in.

The even LUT drives the D input, and the odd LUT drives the G input.


#### JK Flip-flop
The JK-type fiip-flow outputs a 0 or a 1, and retains it's value unless told otherwise. The inputs consist of J (set) and K (clear). On the rising edge of the clock, if J is high and K is not, the output will be set to 1. If K is high and J is low, the output will be set to 0, and if both are high, it will toggle the output. In all cases, the value will be retained until the next rising edge of the lock occurs while J and K are set to values that instruct it to change. The names of the lines are as universal on flipflops are they are nonsensical - J and K? Your guess is as good as mine where they came from

#### D Latch
This is the unclocked equivalent of D flip-flop - again, there is a D and a G input. However, here, there is no clock. Asynchronously, whenever G is high, the output is set to D, and whenever G is low, the output doesn't change.

#### RS Latch
This, again, is the unclocked version of the JK flip-flop. This one more sensibly names the inputs S and R for Set and Reset. Because there is no clock, the behavior when both R and S are high is not defined (this behavior is part of deal when you use an RS latch, integrated into a CCL like this or as a discrete component). Otherwise, if the S line is high, it is set to 1, and if the R line is high, it is set to 0

| LUT  | D | G | J | K | R | S |
|------|---|---|---|---|---|---|
| EVEN | X |   | X |   | X |   |
| ODD  |   | X |   | X |   | X |

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
Method for initializing a logic block; the settings you have previously configured will be applied and pins configured as requested *only* when init() is called. See the section below on reconfiguring.

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

All forms of attachInterrupt, everywhere, are fundamentally evil, because they add a several microsecond overhead to the ISR simply because there is a call to a non-inlinable function, but the attach method is more familiar to most Arduino users.

#### Usage
```c++
Logic0.attachInterrupt(blinkLED, RISING); // Runthe blinkLED function when the output goes high

void blinkLED()
{
  digitalWrite(myLedPin, CHANGE);
}

void setup() {
  // other configuration and initialization of Logic0 needed
  Logic0.attachInterrupt(blinkLED, RISING); // Runthe blinkLED function when the putput goes high
  // and you'll need to do logic::start when you're ready for it to be enabled.
}

```


### detachInterrupt()
Method for disabling interrupts for a specific block.
This method isn't available on tinyAVR 0/1-series.

#### Usage
```c++
Logic0.detachInterrupt(); // Disable interrupts for block 0
```
### Advanced interrupts
New in 1.5.0, it is possible to use interrupts "manually" while still using the Logic library. This results in smaller, faster executing interrupts.

You must declare a function using the ISR macro, like so:
```c++
ISR(CCL_CCL_vect) {
  // It's best if interrupts run quickly and are simple (don't involve accessing scores of variables).
  // Everyday things like making a function call can dramatically slow an ISR. Ideally, you should not
  // call any function here that can't be inlined. Functions that are only called once will be inlined,
  // as will certain "always_inline" functions, which include the digitalWriteFast() and similar.
  // digitalWrite 1) won't be inlined since it's almost certainly used elsewhere, and 2) is unbelivably slow.
  // But before we get to that, you may have noticed that CCL_INT_vect doesn't have the number of the LUT in it.
  // If you've worked with manual pin interrupts you know what that means. There's one interrupt, and if there's
  // more than one CCL interrupt enabled, you have to read the flags and see which interrupt it is.
  // Note that any typo or mistake of any sort in the vector name - amazingly - will not be treated as a compile error.
  // Just a warning. When the interrupt fires the application will uncerimoniously reset.
  uint8_t flags=CCL.INTFLAGS;
  // bits 0 - 5 correspond to LUT0 - LUT5, as you could have guessed.
  // eg, 0bRR543210 - where R is a reserved bit, and the numbers are the number of the LUT corresponding to that bit.
  // You also need to clear the intflags - that isn't done automatically
  // If interrupts may fire in very rapid succession the timing of when you clear the flags could matter.
  // You should always read the flags first thing. Writing them immediately minimizes the possibility
  // of an repeated interrupt condition being missed because you hadn't cleared the flag yet, but it also increases
  // the potential for something like switch bounce to land you in the interrupt multiple times, and in extreme
  // situation, can result in the code being locked in the interrupt, because it will re-trigger immediately after
  // returning, It's probably smarter to only clear the intflags at the very end. This is done by writing a 1 to each
  // flag you are clearing, which typically means all the ones you saw were set at the start of the ISR.
  // DO NOT read the flags a second time - use the value you read earlier. That way if another logic block's interrupt
  // triggered, it would naturally be triggered as soon as you left this interrupt. The interrupt would run twice, though
  // all interrupt conditions that occurred would be handled.
  // Anyway, back to the body of the ISR
  // Assuming that your LED pin is a constant, this is the preferred way to toggle a pin:
  digitalWriteFast(myLedPin, CHANGE);
  // And now we clear the intflags...
  CCL.INTFLAGS = flags;
  // If you forget to do this, the interrupt will run continually. After each interrupt returns, a single instruction will be run before the interrupt runs again, generating the appearance of the sketch running very slowly.
}
```

Assuming you have a CCL interrupt defined, you can enable it by using the `CCL.INTCTRL0` (LUT0-LUT3) and `CCL.INTCTRL1` (LUT4, LUT5) registers.
These registers are structured like:

```text
INTCTRL0:
0b44332211
INTCTRL1 (48/64 pin DA/DB only):
0bRRRR5544

Where R indicates a reserved bit.

The two bits per LUT allow the interrupt to be triggered on
00 - neither/disable interrupt
01 - rising
10 - falling
11 - rising or falling

```

## Reconfiguring
There are TWO levels of "enable protection" on the CCL hardware. I think the intended behavior was "Protection is enabled on the LUT config when that LUT is enabled, AND the CCL is enabled" - which is what we got, except that the locked registers included the one with the bit to enable/disable that LUT, so you had toturn off the whole CCL. This dreadful behavior is  (It would appear that Microchip only became aware of the issue after the Dx-series parts were released; it's the kind of thing where you could believe that it was intended if annoying behavior, which is probably why it wasn't noticed sooner (It came at the same time as the "TCA does what the datasheet says on RESTART command" (paraphrased, ofc), another bug like that; both may indeed have been intended at the time, and only later classified as bugs, perhaps by a newly installed product manager or QA czar who thought the original intent was folly (I do suspect that there was a changing of the guard around that time. That and/or an influx of testing manpower and resources. How else do you introduce a new ADC while reducing the number of errata from the previous generation from around 20, several serious, to like 5, of which this is the only one without a workaround); I happen to agree on this count and think it's a big deal (the other feature that got this treatment I agree with as well, but why did they ever let the timers have a port direction override? . Since there is no indication that there are die revs coming out any time soon, users are advised to proceed with use of workarounds, rather than delay work in the hopes of corrected silicon.

The intended enable-protection is that a given logic block cannot be reconfigured while enabled. *This is handled by `init()` - you can write your new setting to a logic block, call `LogicN.init()` and it will briefly disable the logic block, make the changes, and re-enable it.*

The unintended layer is that no logic block can be enabled or disabled (such as to be reconfigured) without also **disabling the whole CCL system** instead of just the one LUT.

Changes can always be freely made to the `Logic` classes - changes aren't written to the hardware until you call `init()`, so that's the only thing the CCL must be disabled for. On parts which are impacted by this (All tinyAVRs, the mega 0s and the DA and DB), you must call `Logic::stop()` before calling `init()`, and `Logic::start()` afterwards. On other parts, and the above listed parts if/when they get a silicon rev, you need not call `Logic::stop()/start()` - init() handles the per-LUT disable/enable correctly.

### Testing if the enable-lock erratum is present
At time of writing, up through rev AB of the DA, no fix is available, nor is any for tinyAVR. The DD and later never had the problem, and everything before them did. The DB-series recently received the first major die rev, B0 or B1 (depending on the flash size), which fixes this and much other errata.


```c
if (checkErrata(ERRATA_CCL_PROTECTION)) { /*true if errata present */
  Logic::stop();
  Logic1.init();
  Logic::start();
} else { // No erratum, hence no problem
  Logic1.init();
}
```
### Example
```c++

// Imagine there's some code above this that configured and enabled Logic0.

Logic1.truth=0x55;      // new truth table
Logic1.input2=logic::in::tca0;     // and different input 2
Logic3.enabled=true;    // enable another LUT
Logic3.input0=logic::in::link; // Use link from LUT0
Logic3.input1=logic::in::ac;   // and the analog comparator
Logic3.input2=logic::in::pin;  // and the LUT3 IN2 pin
Logic3.truth=0x79;      // truth table for LUT3

Logic3.attachInterrupt(RISING,interruptFunction);

// Interrupt now attached - but - Logic3 not enabled, and logic1 is using old settings

// If we don't care that Logic0 will be briefly delayed, the cautious approach is fully portable.
Logic::stop();  // have to turn off Logic0 too, even though I might not want to
Logic1.init();  // apply changes to logic block 1
Logic3.init();  // apply settings to logic block 3 for the first time
Logic::start(); // re-enable
```

Or, to "do the best it can" with the hardware, but use the more compact implementations without the runtime test if it's using parts the either always or never will have the bug:
```c++

// Imagine there's some code above this that configured and enabled Logic0.
/* So the past is approximately:
 * ConfigureLogic0();
 * ConfigureLogic1();
 * Logic0.init();
 * Logic1.init();
 * Logic::start();
 * And now you want to enable Logic3 and change Logic1 settings, if possible without disturbing Logic0.
 */

void someFunction() {
  Logic1.truth=0x55;      // new truth table
  Logic1.input2=logic::in::tca0;     // and different input 2
  Logic3.enabled=true;    // enable another LUT
  Logic3.input0=logic::in::link; // Use link from LUT0
  Logic3.input1=logic::in::ac;   // and the analog comparator
  Logic3.input2=logic::in::pin;  // and the LUT3 IN2 pin
  Logic3.truth=0x79;      // truth table for LUT3

  Logic3.attachInterrupt(RISING,interruptFunction);

  // Interrupt now attached - but - Logic3 not enabled, and logic1 is using old settings

  applyCCLChanges();
  // you'll probably want to call this more than once - maybe with an argumnent that specifies which blocks to reinit,
  // depending on your application, other logic might be able to be aggregated there to save flash.

void applyCCLChanges() {
  // Adjust to suit the logic blocks you need
  // As noted at the start we wanted to avoid disturbing the Logic0 channel if possible, so we want to avoid the stop-start.
  #if defined(ERRATA_CCL_PROTECTION) && ERRATA_CCL_PROTECTION == 1
    /* Some parts have not received a die rev and hence all extant specimens exhibit this erratum, other parts came out after the bug was fixed and never had the issue.  */
    Logic::stop();  // have to turn off Logic0 too, even though I might not want to
    Logic1.init();  // apply changes to logic block 1
    Logic3.init();  // apply settings to logic block 3 for the first time
    Logic::start(); // re-enable
  #elif defined(ERRATA_CCL_PROTECTION) && ERRATA_CCL_PROTECTION == 0
    /* No version of the part has ever had this erratum, so there is no chance of encountering it */
    Logic1.init();  // apply changes to logic block 1
    Logic3.init();  // apply settings to logic block 3 for the first time
  #else
    /* Awesome! We got a die rev finally. Wait, crap, now I have to support both? */
    if checkErrata(ERRATA_CCL_PROTECTION) {
      Logic::stop();  // have to turn off Logic0 too, even though I might not want to
      Logic1.init();  // apply changes to logic block 1
      Logic3.init();  // apply settings to logic block 3 for the first time
      Logic::start(); // re-enable
    } else {
      Logic1.init();  // apply changes to logic block 1
      Logic3.init();
    }
  #endif
}
```

## Think outside the box
To consider the CCL system as simply a built-in multifunction gate IC is to greatly undersell it. The true power of the CCL is in it's ability to use events directly, and to take inputs from almost everything. Even doing neat stuff like the above mentioned "latch with no sequencer" is only scratching the surface of what these can do! Taking that a step farther... you could then use the odd-numbered logic block with that same feedback to, say, switch between two waveforms being output by one of the PWM timers, depending on what the latch is set to. See the [Tricks and Tips page](Tricks_and_Tips.md)

## About the clocked components
It is important to be absolutely clear about what is and is not clocked, because it is not always immediately clear what's being clocked and what isn't. The logic embodied in the truth table is combinatorial logic, and has no need for a clock. The clock is ONLY used for the synchronizer/filter, the edge detector, and (for even numbered LUTs) the sequencer.

The clocks also span about the broadest range of clock speeds I've seen offered for any component: From 1 kHz all the way up to above-CPU clock speeds (on the EB's potentially 4x the system clock!)

**REMEMBER NYQUIST**
*Or at least his sampling criterion! Did he do anything else important?*

You must not expect to be able to reliably or accurately recognize pulses in the signal being sampled (in this case, the *output* of 3-input truth table lookup, before all that other machinery)


## Note on terminology
Yes, technically, C++ doesn't have "properties" or "methods" - these are "member variables" and "member functions" in C++ parlance. They mean the same thing. I've chosen to use the more familiar, preseent day terminology.
