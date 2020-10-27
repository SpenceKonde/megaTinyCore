# Logic
A library for interfacing with the CCL (Configurable Custom Logic) peripherals of the megaAVR-0 MCUs.
Developed by MCUdude for use with [MegaCoreX](https://github.com/MCUdude/MegaCoreX), adapted to tinyAVR 0/1-series parts by [Tadashi G. Takaoka](https://github.com/tgtakaoka), and to Dx-series and tinyAVR 2-series by [Spence Konde](https://github.com/SpenceKonde)

The examples and information below assume the use of megaTinyCore and the tinyAVR parts (as this version is the one distributed with megaTinyCore)
More useful information about CCL can be found in the [Microchip Application Note TB3218](http://ww1.microchip.com/downloads/en/AppNotes/TB3218-Getting-Started-with-CCL-90003218A.pdf) and in the datasheets for the parts in question. While the CCL blocks can be used to act as simple logic gates with the input pins used directly, because they can take inputs from other peripherals as well as the event system, the potential for the CCL goes well beyond that. The above App Note provides code for using the SPI and a CCL block to manchester encode data (that's the sort of application that the USART and SPI sources are there for).


## Logic
Class for interfacing with the built-in logic block. Use the predefined objects `Logic0`, `Logic1`, `Logic2` and `Logic3`.
`Logic2` and `Logic3` are only available on the tinyAVR 2-series, as well as megaAVR 0-series, Dx-series, and EA-series. `Logic4` and `Logic5` are available only on Dx-series parts with 48 or 64 pins.
Each object contains register pointers for interfacing with the right registers. All configurable options are exposed by this library.


### enable
Variable for enabling or disabling a logic block.
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
Variable for setting what mode input 0..2 on a logic block should have.

Notes for all parts:
* The datasheets give different names for the event channels for ATtiny and ATmega parts, both are supported by library: event 0 and event a are the same, as are event 1 and event b. These can be generated using the event system, see the relevant datasheet and examples for more information.
* Timer WO channels correspond to the specified timer's PWM Output channels. They are true when the specified pin would be driven HIGH. They do not appear to work if waveform output is not enabled, sadly.
* Type-B timers can only generate an output under circumstances where they could drive an output pin if CCMPEN in TCBn.CTRLA is set.

Accepted values for tinyAVR 2-series:
``` c++
in::masked;           // Pin not in use
in::unused;           // Pin not in use
in::disable;          // Pin not in use
in::feedback;         // Connect output of the logic block to this input
in::link;             // Connect output of logic block n+1 to this input
in::event_0;          // Connect input to event a
in::event_a;          // Connect input to event a
in::event_1;          // Connect input to event b
in::event_b;          // Connect input to event b
in::pin;              // Connect input to CCL IN0, IN1, IN2 for input 0, 1, 2, do not change pinMode
in::input_pullup;     // Connect input to CCL IN0, IN1, IN2 for input 0, 1, 2, set input, pullup on
in::input;            // Connect input to CCL IN0, IN1, IN2 for input 0, 1, 2, set input, pullup off
in::input_no_pullup;  // Connect input to CCL IN0, IN1, IN2 for input 0, 1, 2, set input, pullup off
in::ac;               // Connect input to the output of the internal analog comparator
in::uart;             // Connect input to UART TX. Input 0 connects to UART0 TX, input 1 to UART1 TX. Not valid for input 2
in::spi;              // Connect input to SPI. Input 0 and 1 connects to MOSI, and input 2 connects to SCK
in::tca0;             // Connect input to TCA0. Input 0 connects to WO0, input 1 to WO1 and input2 to WO2
in::tca;              // Synonym for tca0
in::tcb;              // Connect input to TCB. Input 0 connects to TCB0 W0, input 1 to TCB1 WO, and input 2 to TCB2 WO.
```

Notes for tinyAVR 2-series:
* Inputs 0 and 1 connect to MOSI, input 2 is SCK. Master mode only. Note also that the order is different from the other ATtiny parts, but the same as Dx-series and megaAVR 0-series.
* If input on the highest-number Logic block is set to link, it will use the output of Logic0
* If you need to link input to logic block other than the n+1 block, you can use the event system for that.
* Not all inputs are available on all parts; the event system can be used to substitute inputs.

Accepted values for tinyAVR 0/1-series parts:

``` c++
in::masked;           // Pin not in use
in::unused;           // Pin not in use
in::disable;          // Pin not in use
in::feedback;         // Connect output of the logic block to this input
in::link;             // Connect output of the other logic block to this input
in::event_0;          // Connect input to event 0
in::event_a;          // Connect input to event 0
in::event_1;          // Connect input to event 1
in::event_b;          // Connect input to event 1
in::pin;              // Connect input to CCL IN0, IN1, IN2 for input 0, 1, 2, do not change pinMode
in::input_pullup;     // Connect input to CCL IN0, IN1, IN2 for input 0, 1, 2, set input, pullup on
in::input;            // Connect input to CCL IN0, IN1, IN2 for input 0, 1, 2, set input, pullup off
in::input_no_pullup;  // Connect input to CCL IN0, IN1, IN2 for input 0, 1, 2, set input, pullup off
in::ac0;              // Connect input to AC0 OUT
in::tcb0;             // Connect input to TCB0 WO
in::tca;              // Connect input to TCA0 WO0~2 for input 0~2
in::tca0;             // Connect input to TCA0 WO0~2 for input 0~2
in::tcd;              // Connect input to TCD WOAn, WOB, WOA for input 0, 1, 2
in::usart;            // Connect input to USART0 XCK and TXD for input 0, 1
in::spi;              // Connect input to SPI0 SCK, MOSI, MISO for input 0, 1, 2
in::ac1;              // Connect input to AC1 OUT (input 0, 1 only)
in::tcb1;             // Connect input to TCB1 WO (input 0, 1 only)
in::ac2;              // Connect input to AC2 OUT (input 0, 1 only)
```

Notes specific to ATtiny 0/1-series:
* It is not clear what TCD0 WOAn is. I suspect it is true when TCD0 count is greater than TCD0.COMPSET0 and TCD0.CMPCLR0, ie, when WOA turns on and off, which can be individually controlled; the timer continues counting until reaching TCD0.CMPCLR1 when WOB turns off (TCD0 is a very strange timer).
* Not all inputs are available on all parts - only input sources corresponding to peripherals on that device are available. Not all options are available for input 2, as noted above.
* On CCL1 (logic1), IN0 I/O input is available only on the 20 and-24 pin parts, and IN1 and IN2 only on 24-pin parts. The event inputs can be used with pin events to take input from a different pins for up to two inputs. This is demonstrated in the five input example.
* CCL0's IN0 pin is on PA0, which is nominally the UPDI pin. This may limit the usefulness of CCL0 on the ATtiny parts (though it may work as long as the input cannot be mistaken for a UPDI activation command); configuring UPDI as GPIO prevents further programming via UPDI except via HV programming. One can always use the event system to substitute another input for IN0; This is demonstrated in the three input example.

##### Usage
``` c++
Logic0.input0 = in::link;  // Connect output from block 1 to input 0 of block 0
Logic0.input1 = in::input; // Connect the input 1 from block 0 to its GPIO
Logic0.input2 = in::input_pullup; // Connect the input 2 from block 0 to its GPIO and enable pullup
```

##### Default state
`LogicN.inputN` defaults to `in::unused` if not specified in the user program.


### output
Variable for changing the logic block output pin behavior. Note that the output of the logic block still can be used internally if the output pin is disabled.
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
Variable for pin swapping the physical output pin to its alternative position. See the pinout diagrams in the main MegaCoreX README for detailed info.
Accepted values:
```c++
out::no_swap;  // Use default pin position, pin 2 on the port
out::pin_swap; // Use alternative position, pin 6 on the port
```

##### Usage
```c++
Logic0.output_swap = out::no_swap; // No pin swap for output of block0
```

##### Default state
`LogicN.output_swap` defaults to `out::no_swap` if not specified in the user program.


### filter
Variable to connecting a filter or synchronizer to the logic block output. Useful when multiple logic blocks are connected internally to prevent logic race.
Accepted values:
```c++
filter::disable;      // No filter used
filter::synchronizer; // Connect synchronizer to output; delays output by 2 clock cycles.
filter::filter;       // Connect filter to output; delays output by 4 clock cycles, only passes output that is stable for >2 clock cycles.
```

##### Usage
```c++
Logic0.filter = filter::filter; // Enable filter on output of block 0
```

##### Default state
`LogicN.filter` defaults to `filter::disable` if not specified in the user program.


### clocksource
Variable set the clock source for the LUT. It is used only for the filter, synchronizer and edge detector - as the Oscillator example demonstrates, the outputs change asynchronously in less than a clock cycle. On the tinyAVR 0/1-series, only clk_per (peripheral clock, ie, the system clock) and in2 are available. If you don't know what this means, you want clk_per, the default. If sequential logic is used, the clock source for the even LUT controls that.
Accepted values:
```c++
clocksource::clk_per;      // Clock from the peripheral clock (ie, system clock)
clocksource::in2;          // Clock from the selected input2; it is treated as a 0 in the truth table.
clocksource::oschf;        // Clock from the **unprescaled** internal HF oscillator. Same as clk_per if system clock not prescaled - not available on tinyAVR 0/1-series
clocksource::osc32k;       // Clock from the internal 32.768 kHz oscillator - not available on tinyAVR 0/1-series
clocksource::osc1l;        // Clock from the internal 32.768 kHz oscillator prescaled by 32 - not available on tinyAVR 0/1-series
```

#### Usage
```c++
Logic0.clocksource = clocksource::clk_per;
```

##### Default state
`LogicN.clocksource` defaults to `clocksource::clk_per` if not specified in the user program.


### edgedetect
Variable for controlling use of the edge detector. The edge detector can be used to generate a pulse when detecting a rising edge on its input. To detect a falling edge, the TRUTH table should be programmed to provide inverted output. In order to avoid unpredictable behavior, a valid filter option must be enabled. Note that this is likely only of use when the output is being used for sequential logic or as the input to another logic block; it looks particularly useful on the odd LUT input to a J-K flip-flop sequential logic unit.

```c++
edgedetect::disable;      // No edge detection used
edgedetect::enable;       // Edge detection used
```

#### Usage
```c++
Logic0.edgedetect = edgedetect::enable;
```

##### Default state
`LogicN.edgedetect` defaults to `edgedetect::disable` if not specified in the user program.


### sequencer
Variable for connecting a sequencer to the logic block output. There is 1 sequencer per 2 CCLs; this option is ignored for the odd-numbered logic blocks. In most available hardware (see the relevant Errata), the D-latch does not work; use the D Flip-flop instead.
Accepted values:
```c++
sequencer::disable;      // No sequencer connected
sequencer::d_flip_flop;  // D flip flop sequencer connected
sequencer::jk_flip_flop; // JK flip flop sequencer connected
sequencer::d_latch;      // D latch sequencer connected
sequencer::rs_latch;     // RS latch sequencer connected
```

##### Usage
```c++
Logic0.sequencer = sequencer::disable; // Disable sequencer
```

##### Default state
`LogicN.sequencer` defaults to `sequencer::disable` if not specified in the user program.


### truth
Variable to hold the 8-bit truth table value.
Accepted values between 0x00 and 0xFF.

##### Usage
```c++
Logic0.truth = 0xF0;
```

##### Default state
`LogicN.truth` defaults to `0x00` if not specified in the user program.



## init()
Method for initializing a logic block.

##### Usage
```c++
Logic0.init(); // Initialize block 0
Logic1.init(); // Initialize block 1
```



## start()
Method for starting the CCL hardware after all registers have been initialized using LogicN.init().

##### Usage
```c++
Logic::start(); // Start CCL hardware
```



## stop()
Method for stopping the CCL hardware.

##### Usage
```c++
Logic::stop(); // Stop CCL
```



## attachInterrupt()
Method for enabling interrupts for a specific block.
Valid arguments for the third parameters are `RISING`, `FALLING` and `CHANGE`.
This method isn't available on tinyAVR 0/1-series, as these parts cannot generate an interrupt from the CCL blocks.

##### Usage
```c++
Logic0.attachInterrupt(blinkLED, RISING); // Runthe blinkLED function when the putput goes high

void blinkLED()
{
  digitalWrite(myLedPin, CHANGE);
}
```



## detachInterrupt()
Method for disabling interrupts for a specific block.
This method isn't available on tinyAVR 0/1-series.

##### Usage
```c++
Logic0.detachInterrupt(); // Disable interrupts for block 0
```

## Ideas
* Need an interrupt from CCL on tinyAVR 0/1-series? Put a pin interrupt on the output pin! Nobody says you need to connect it to anything...
* With two LUTs, one with syncronizer/filter, and the other without, but the same inputs, you have one offset by a few clocks from the other.
  * For a longer delay, take over TCD and use the PROGEV event... this delay can be quite long... even as long as a whole cycle...
  * This may be the only way to generate a truly independent PWM signal with CCL and Event system!
* CCL and the Event System are intimately connected. Don't try to think about them separately.
* Remember that peripherals can do things in response to events, in addition to generate them.
  * TCA can count events, or count only during level events, change direction based on events, restart based on events... (note: you will have to reconfigure it in normal mode instead of split mode for this)
  * Except on tinyAVR 0/1-series and megaAVR 0-series, TCB can count on event
  * Between the timer/counters, CCL, and the event system, we've got everything except a shift register without CPU involvement!
* Always be thinking of how you can use CCL and the event system to do clever things that aren't what it looks like they're meant for
* Share your clever tricks with the community - Submit them over here: https://github.com/SpenceKonde/AVR-Guidance/tree/master/CCL_EVSYS_hacks (I will be adding some of mine soon). CCL/EVSYS is an incredibly powerful tool, and we have barely scratched the surface of what it is capable of, especially on parts like the Dx-series, with up to 6 LUTs and 10 event channels.
