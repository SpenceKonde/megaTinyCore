# Logic
A library for interfacing with the CCL (Configurable Custom Logic) peripherals of the megaAVR-0 MCUs.  
Developed by MCUdude for use with [MegaCoreX](https://github.com/MCUdude/MegaCoreX), adapted to megaAVR ATtiny parts by [Tadashi G. Takaoka](https://github.com/tgtakaoka).
The megaTiny parts have either 1 (0-series) or 3 (1-series) blocks of CCL available.
More useful information about CCL can be found in the [Microchip Application Note TB3218](http://ww1.microchip.com/downloads/en/AppNotes/TB3218-Getting-Started-with-CCL-90003218A.pdf) and in the [megaAVR-0 family data sheet](http://ww1.microchip.com/downloads/en/DeviceDoc/megaAVR0-series-Family-Data-Sheet-DS40002015B.pdf).

### Warning: Examples not updated for ATtiny parts!


## Logic
Class for interfacing with the built-in logic block. use the predefined objects `Logic0`, `Logic1`, `Logic2` and `Logic3`.
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
Accepted values:
``` c++
in::masked;       // Pin not in use
in::unused;       // Pin not in use
in::disable;      // Pin not in use
in::feedback;     // Connect output of the logic block to this input
in::link;         // Connect output of logic block n+1 to this input
in::event_a;      // Connect input to event A
in::event_b;      // Connect input to event B
in::input;        // Connect input to GPIO
in::ac;           // Connect input to the output of the internal analog comparator
in::input_pullup; // Connect input to GPIO and enable the internal pullup resistor
in::uart;         // Connect input to UART TX. Input 0 connects to UART0 TX, input 1 to UART1 TX, and input 2 to UART2 TX
in::spi;          // Connect input to SPI. Input 0 and 1 connects to MOSI, and input 2 connects to SCK
in::tca0;         // Connect input to TCA0. Input 0 connects to WO0, input 1 to WO1 and input2 to WO2
in::tcb;          // Connect input to TCB. Input 0 connects to TCB0 W0, input 1 to TCB1 WO, and input 2 to TCB2 WO
```

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
filter::synchronizer; // Connect synchronizer to output
filter::filter;       // Connect filter to output
```

##### Usage
```c++
Logic0.filter = filter::filter; // Enable filter on output of block 0
```

##### Default state
`LogicN.filter` defaults to `filter::disable` if not specified in the user program.


### sequencer
Variable for connecting a sequencer to the logic block output.  
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
This method ins't available on tinyAVR series.

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
This method ins't available on tinyAVR series.

##### Usage
```c++
Logic0.detachInterrupt(); // Disable interrupts for block 0
```
