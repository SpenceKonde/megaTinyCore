# Logic
A library for interfacing with the CCL (Configurable Custom Logic) peripherals of the megaAVR-0 MCUs.
Developed by MCUdude for use with [MegaCoreX](https://github.com/MCUdude/MegaCoreX), adapted to megaAVR ATtiny parts by [Tadashi G. Takaoka](https://github.com/tgtakaoka), and to DA-series by [Spence Konde](https://github.com/SpenceKonde)
Correction of several issues on the megaAVR ATtiny parts and adaptation of examples by [Spence Konde](https://github.com/SpenceKonde).
All of the megaTiny parts have 2 blocks of CCL available. The examples included assume the use of megaTinyCore in their defines to detect the applicable part.
More useful information about CCL can be found in the [Microchip Application Note TB3218](http://ww1.microchip.com/downloads/en/AppNotes/TB3218-Getting-Started-with-CCL-90003218A.pdf) and in the [megaAVR-0 family data sheet](http://ww1.microchip.com/downloads/en/DeviceDoc/megaAVR0-series-Family-Data-Sheet-DS40002015B.pdf), or the datasheet for the ATtiny part in question.



## Logic
Class for interfacing with the built-in logic block. use the predefined objects `Logic0`, `Logic1`, `Logic2` and `Logic3`. `Logic2` and `Logic3` are only available on the ATmega parts.
Each object contains register pointers for interfacing with the right registers. Some of these variables are available to the user.


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
* Timer WO channels correspond to the specified timer's PWM Output channels. They are true when the specified timer's value is higher than the compare register for that output channel.
* Type-B timers can only generate an output under circumstances where they could drive an output pin if CCMPEN in TCBn.CTRLA is set.

Accepted values for DA-series and megaAVR 0-series parts:
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
in::ac;               // Connect input to the output of the internal analog comparator (input 0,1,2 from AC0,1,2)
in::zcd;              // Connect input to the output of the zero crossing detector (input 0,1,2 from ZCD0,1,2) - DA-series only
in::uart;             // Connect input to UART TX. Input 0 connects to UART0 TX, input 1 to UART1 TX, and input 2 to UART2 TX
in::spi;              // Connect input to SPI. Input 0 and 1 connects to MOSI, and input 2 connects to SCK
in::tca0;             // Connect input to TCA0. Input 0 connects to WO0, input 1 to WO1 and input2 to WO2
in::tca1;             // Connect input to TCA1. Input 0 connects to WO0, input 1 to WO1 and input2 to WO2 - DA-series only
in::tcb;              // Connect input to TCB. Input 0 connects to TCB0 W0, input 1 to TCB1 WO, and input 2 to TCB2 WO.
in::tcd;              // Connect input to TCD0. Input 0 connects to WOA, input 1 to WOB and input2 to WOC - DA-series only
```

Notes specific to ATmega parts
* The 48-pin and 64-pin DA-series parts have 6 LUTs. No pins for CCL5 are available in the 48-pin package.
* On 28-pin versions of the ATmega 4808, 3208, 1608, and 808, IN1 and IN2 inputs for logic3 are not available. If all input pins for all logic blocks are needed, the event system workaround shown for the ATtiny parts in the examples can be used, though note that register names and values are different. Same principle applies to the DA-series devices with fewer pins.
* According to the datasheet for SPI as input source, inputs 0 and 1 connect to MOSI. Thus, on these parts, there is no input to the logic blocks for MISO. Note also that the order is different from the ATtiny parts.
* If input on the highest-number Logic block is set to link, it will use the output of Logic0 (unless it's an early production DA-series - see the errata)
* If you need to link input to logic block other than the n+1 block, you can use the event system for that.

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
in::usart;            // Connect input to U(S)ART0 XCK and TXD for input 0, 1
in::spi;              // Connect input to SPI0 SCK, MOSI, MISO for input 0, 1, 2
in::ac1;              // Connect input to AC1 OUT (input 0, 1 only)
in::tcb1;             // Connect input to TCB1 WO (input 0, 1 only)
in::ac2;              // Connect input to AC2 OUT (input 0, 1 only)
```

Notes specific to ATtiny:
* It is not clear what TCD0 WOAn is. I suspect it is true when TCD0 count is greater than TCD0.COMPSET0 and TCD0.CMPCLR0, ie, when WOA turns on and off, which can be individually controlled; the timer continues counting until reaching TCD0.CMPCLR1 when WOB turns off (TCD0 is a very strange timer).
* Not all inputs are available on all parts - only input sources corresponding to peripherals on that device are available. Not all options are available for input 2, as noted above.
* On CCL1 (logic1), IN0 I/O input is available only on the 20 and-24 pin parts, and IN1 and IN2 only on 24-pin parts. The event inputs can be used with pin events to take input from a different pins for up to two inputs. This is demonstrated in the five input example.
* CCL0's IN0 pin is on PA0, which is nominally the UPDI pin. It can only be used as input when that pin is set as GPIO. This limits the usefulness of CCL0 on the ATtiny parts; configuring UPDI as GPIO prevents further programming via UPDI except via HV programming. Configuring this option is only supported on megaTinyCore with Optiboot bootloader (from 1.1.6 on), and prevents further modification of the fuses or bootloader without HV programming, however, as noted above, input0 can be used via the event inputs to take input from another pin. This is demonstrated in the three input examples.



##### Usage
``` c++
Logic0.input0 = in::link;         // Connect output from block 1 to input 0 of block 0
Logic0.input1 = in::input;        // Connect the input 1 from block 0 to its GPIO
Logic0.input2 = in::input_pullup; // Connect the input 2 from block 0 to its GPIO, with pullup on
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
Variable for pin swapping the physical output pin to its alternative position. See the pinout diagrams in the [Core this is part of](../../../README.md) for more info.
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
Variable to connect a filter or synchronizer to the logic block output. Useful when multiple logic blocks are connected internally to prevent race conditions and glitches that could arise due to the asynchronous nature of CCL clocking. Either filter or synchronizer is required for edge detector, below.
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
Variable set the clock source for the LUT. The inputs are checked and output updated on the rising edge of this clock. Except on the Dx-series parts, only clk_per (peripheral clock, ie, the system clock) and in2 are available. If you don't know what this means, you want clk_per. If sequential logic is used, this is also used to clock that.
Accepted values:
```c++
clocksource::clk_per;      // Clock from the peripheral clock (ie, system clock)
clocksource::in2;          // Clock from the selected input2; it is treated as a 0 in the truth table.
clocksource::oschf;        // Clock from the **unprescaled** internal HF oscillator. Same as clk_per if system clock not prescaled.
clocksource::osc32k;       // Clock from the internal 32.768 kHz oscillator
clocksource::osc1l;        // Clock from the internal 32.768 kHz oscillator prescaled by 32
```


##### Usage
```c++
Logic2.filter = filter::filter; // Set Block2 to use unprescaled high frequency internal oscillator.
```

##### Default state
`LogicN.filter` defaults to `filter::disable` if not specified in the user program.



### edgedetect
Variable for controlling use of the edge detector. The edge detector can be used to generate a pulse when detecting a rising edge on its input. To detect a falling edge, the TRUTH table should be programmed to provide inverted output. In order to avoid unpredictable behavior, a valid filter option must be enabled. Note that this is likely only of use when the output is being used for sequential logic or as the input to another logic block; it looks particularly useful on the odd LUT input to a J-K flip-flop sequential logic unit.

```c++
edgedetect::disable;      // No edge detection used
edgedetect::enable;       // Edge detection used
```

### sequencer
Variable for connecting a sequencer to the logic block output. There is 1 sequencer per 2 CCLs; this option is ignored for the odd-numbered logic blocks.
Accepted values:
```c++
sequencer::disable;      // No sequencer connected
sequencer::d_flip_flop;  // D flip flop sequencer connected
sequencer::jk_flip_flop; // JK flip flop sequencer connected
sequencer::d_latch;      // D latch sequencer connected - note that on most tinyAVR and megaAVR parts, this doesn't work. See the Errata. It should work on Dx-series.
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
Method for initializing a logic block. The logic block object to initialize is passed as an argument.

##### Usage
```c++
Logic0.init(); // Initialize block 0
Logic1.init(); // Initialize block 1
```



## start()
Method for starting the CCL hardware after all registers have been initialized using init(block_t).

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
This method isn't available on tinyAVR series, as these parts cannot generate an interrupt from the CCL blocks.

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
This method isn't available on tinyAVR series.

##### Usage
```c++
Logic0.detachInterrupt(); // Disable interrupts for block 0
```
