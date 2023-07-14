# Additional Functions in DxCore
These are in addition to what is listed in the [Arduino Reference](https://www.arduino.cc/reference/en/) as well as implementation details for API function that are Arduino-standard. Several functions are unique to the cores I maintain. The functions here are marked with (standard), (undocumented standard) (present in official cores but not documented), or (one of more of mTC, DxC, and ATC - these are the three cores I maintain, megaTinyCore, DxCore and ATTinyCore). Usually the standard API function here are offering information on what corner cases will make them malfunction. The official implementation also has corner cases where things go awry. They do not detail them in the documentation.

One example might be that you might unwittingly be implicitly depending on micros being slow. How fast or slow it is depends on clock speed, and the core. It is more performant here than the official core, especially on the most important speeds, as covered in detail in the PWM and Timer reference.

## Error reporting
One of the major challenges when writing embedded code is that there are no exceptions, so when something doesn't work correctly, you don't get an indication of what, exactly. prevented it from doing so. It's up to you to try to gather more data and analyze it to figure out where it is failing. Often, when you get to the end of it, the problem turns out to be something that one could have known ahead of time would certainly not work. While these methods are far from perfect, when we can determine at compile time that your code won't do what you are trying to make it do, will produce an error. These two "error" function calls are the mechanism used to actually create the error in almost all cases: compilation fails if any of these are referenced in the code after all constant folding and optimization.

Internally these are often used with the compiler test `__builtin_constant_p()` which returns true if the argument is a compile-time known constant value subject to constant folding:
```c
digitalWrite(100, HIGH); // This tests whether the pin is known to be invalid at compile time, and since nothing supported by this core has 100 pins, will error.
uint8_t pinnbr = 100;
digitalWrite(pinnbr, HIGH); // This will also error, because there is no chance for pinnbr to be anything other than 100.
volatile uint8_t pinnumber = 100;
digitalWrite(pinnumber, HIGH); // This will not error at compile time, because the compiler cannot optimize away the volatile variable.
```
Note that in the case of the digital I/O functions, NOT_A_PIN (255) *IS* permitted
### (DxC,mTC,ATC)  `void badArg("msg")`
This function is what we call when user code passed an argument to a function that is guaranteed to give results without meaning or requested something which is not achievable, and we know at compile time, such as `analogRead(pin without analog input option)`. The message will indicate the nature of the problem.

### (DxC,mTC,ATC) `void badCall("msg")`
This function is what we call when user code a function that doesn't make sense to call with the current configuration, regardless of what values one passes. For example, calling `stop_millis()` when millis is disabled. The message will indicate why that call is invalid.

## Digital Functions
See [Digital Reference](Ref_Digital.md)
```c
int8_t  digitalReadFast( uint8_t pin)
void    digitalWriteFast(uint8_t pin, uint8_t val)
void    openDrainFast(   uint8_t pin, uint8_t val)
void    openDrain(       uint8_t pin, uint8_t val)
void    pinConfigure(    uint8_t pin, uint16_t mode)
void    turnOffPWM(      uint8_t pin)
```
## Pin information
These are almost all preprocessor macros, not functions, but what they expand to is appropriate for the stated datatypes/
These are in many cases part of the standard API, but not all are. The concept of analog channel identifiers (eg, ADC_CH(0) = 0x80) is new in my cores, and gives an unambiguous way to represent an input by either pin or channel. Historically, this has been a mess on non-328p parts. Note that *digital I/O functions do not accept channel identifiers at this time on DxCore* or megaTinyCore - I am trying to train people away from that practice.

As a reminder, we recommend getting in the habit of using PIN_Pxn notation rather than raw numbers - it makes your code more portable between modern AVR parts, makes it easier to refer to manufacturer documentation, and promotes the habit of using those names, which eliminate all ambiguity when referring to pins, for example in forum discussions, where people on


### (standard) `uint8_t digitalPinToPort(pin)`
Returns the port that a pin belongs to - these are constants named PA, PB, PC, etc (not to be confused with PORTA, which is the port struct itself). These have numeric values of 0-6 for the existing ports, PA to PG.

### (standard) `uint8_t digitalPinToBitPosition(pin)`
Returns the bit position of a pin within a port, eg, `digitalPinToBitPosition(PIN_PA5)` will return 5.

### (standard) `uint8_t digitalPinToBitMask(pin)`
Returns the bit mask of a pin within a port, eg, `digitalPinToBitPosition(PIN_PA5)` will return (1 << 5) or 0b00100000

### (standard) `bool digitalPinHasPWM(pin)`
Returns true if the pin - at default core configuration - has PWM. Does not account for user changed to PORTMUX, but the dynamic nature of `digitalPinToTimerNow()` causes problems in some cases.

### (standard) `uint8_t digitalPinToTimer(pin)`
Returns the timer which that pin uses for PWM, if any; this is either `NOT_ON_TIMER` (0) `TIMERA0`, `TIMERD0` (1-series only) or `DACOUT` (1-series only).

### (standard) `PORT_t* portToPortStruct(port)`
### (standard) `PORT_t* digitalPinToPortStruct(pin)`
These accept, respectively, a port name (`PA`, `PB`, or `PC`), or a pin, and return a pointer to the PORTx structure associated with either the given port or the port that the pin is a member of (`PORTA`, `PORTB`, or `PORTC`). Invalid arguments will return a null pointer.

### (standard) `volatile uint8_t * getPINnCTRLregister(port, bit_pos)`
Returns a pointer to the PINnCTRL register associated with that port and bit position.

### (standard) `volatile uint8_t * portOutputRegister(P)`
### (standard) `volatile uint8_t * portInputRegister(P)`
### (standard) `volatile uint8_t * portModeRegister(P)`
These return pointers to the port output, input and direction registers (output controls whether the pin is driven high or low if driven, direction controls whether the pin is driven or is an input, and input is the read-only register containing the input value).

### (standard) `uint8_t digitalPinToAnalogInput(p)`
Returns the analog input number used internally. Only useful when fully taking over the ADC either directly or as part of a sketch.

### (mTC/DxC)`uint8_t analogChannelToDigitalPin(p)`
Returns the pin number corresponding to an analog channel identifier. This is simply the analog input number with the high bit set via the ADC_CH() macro. Think long and hard if you find yourself needing this, it is rarely important on megaTinyCore or DxCore since you can always refer to pins with the digital pin number or even better, PxN notation.

### (standard) `uint8_t analogInputToDigitalPin(p)`
Returns the digital pin number associated with an analog input number. Only useful when fully taking over the ADC.

### (DxC only) `uint8_t digitalOrAnalogPinToDigital(p)`
If `p < NUM_DIGITAL_PINS`, p is a digital pin, and is returned as is. If it is equal to `ADC_CH(n)` where n is a valid analog channel it is converted to the digital pin, and if anything else, it returns NOT_A_PIN.

### (DxC only) `uint8_t portToPinZero(port)`
Returns `PIN_Px0` - eg `portToPinZero(PA)` is PIN_PA0. This will not work if there is no "hole" provided in the pin numbering.
For example, AVR64DD14, the pins are PA0, PA1, PC1, PC2, PC3, PD4, PD5, PD6, PD7, PF6, PF7, with numbers 0, 1, 9, 10, 11, ~12 (PD0)~, 16, 17, 18, 19.  portToPinZero will work for PA (0, PIN_PA0), PC (8, PIN_PC0, even though the pin isn't there), and PD (12, PIN_PD0), but not PF.  this function is used internally in ways that don't matter for this case, and because it would otherwise involve the same pin number meaning different things, or much larger arrays (hence wasted space)

### (DxC only) `volatile uint8_t* digitalPinHasPWMTCB(p)`
Returns true if the pin has PWM available in the standard core configuration. This is a compile-time-known constant as long as the pin is, and does not account for the PORTMUX registers.
### (Standard) `uint8_t digitalPinHasPWM(p)`
Returns true if the pin has PWM available in the standard core configuration. This is a compile-time-known constant as long as the pin is, and does not account for the PORTMUX registers.

## Attach Interrupt Enable
If using the old or default (all ports) options, these functions are not available; WInterrupt will define ALL port pin interrupt vectors if `attachInterrupt()` is referenced anywhere in the sketch or included library. This is the old behavior.
If you are using the new implementation in manual mode you must call one of the following functions before attaching the interrupt (override `onPreMain()` if you need it ready for a class constructor. That port can have interrupts attached, but others cannot. That port cannot have manually written interrupts, which are typically 10-20 times faster), while the other ports can. I will also note that the implementation is hand-tuned assembly and that's still how slow it is. See the [Interrupt Reference for more information](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/Ref_Interrupts.md).
```c
void attachPortAEnable()
void attachPortBEnable()
void attachPortCEnable()
void attachPortDEnable()
void attachPortEEnable()
void attachPortFEnable()
void attachPortGEnable()
```
## Overridables
See [Callback Reference](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/Ref_Callbacks.md)
```c++
void init_reset_flags()
void onPreMain()
void onBeforeInit()
void init() // Part of ArduinoAPI
void initVariant() //part of ArduinoAPI - reserved for core and rare libraries
void init_TCA0()
void init_TCA1()
void init_TCBs()
void init_TCD0()
void init_millis()
uint8_t onAfterInit()
void onClockFailure()
void onClockTimeout()
int main(); // the Big One!
```

### `Related: uint8_t digitalPinToInterrupt(P)`
This is an obsolete macro that is only present for compatibility with old code. It has nothing to do and simply expands to the sole argument.


## Analog Functions
See [Analog Reference](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/Ref_Analog.md)
```c
int32_t analogReadEnh(       uint8_t pin,                    uint8_t res,    uint8_t gain)
int32_t analogReadDiff(      uint8_t pos,       uint8_t neg, uint8_t res,    uint8_t gain)
int16_t analogClockSpeed(    int16_t frequency,              uint8_t options             )
bool    analogReadResolution(uint8_t res                                                 )
bool    analogSampleDuration(uint8_t dur                                                 )
void    DACReference(        uint8_t mode                                                )
uint8_t getAnalogReference()
uint8_t getDACReference()
uint8_t getAnalogSampleDuration()
int8_t  getAnalogReadResolution()
```

## Timekeeping

### `millis()` and `micros()`
(standard) One difference from the stock implementation - `millis` is #defined as `millis`, and `micros` is #defined as `micros`. Macros do not expand themselves, but this allows you to test #if defined(millis) to determine if millis is available.
These functions are not available if millis has been disabled from the tools menu. They are available if timekeeping is enabled in the tools menu, but has been prevented from operating in some other way, such as by overriding the timer initialization function or calling `stop_millis()` - in that case the number they return will not increase (if the timer isn't running) or will return nonsensical values (if it is). As long as interrupts are never disabled for more than 1ms at a time, they will not throw off the times returned. These functions will lose time and behave erratically beyond that. Time will be guaranteed to be lost when interrupts are disabled for more than 2ms

### (standard, with minor changes) `delay()`
This works normally. If millis is disabled, the builtin avrlibc implementation of `_delay_ms()` is used; for constant arguments it is used directly and for variable ones it is called in a loop with 1ms delay. The catch with that is that if interrupts fire in the middle, time spent in those does not count towards the delay - unlike normal delay(). However, be warned that if millis is stopped in any way other than the tools menu, **delay will not know to use `_delay_ms()` and will likely wait forever (if the timer is stopped) or wait for unpredictable lengths of time (if it is running in some different configuration)**.

When delay is used with RTC as the millis timer, different implementations are used.

**(standard) delay() must never be called when interrupts are disabled**

### (standard, with minor changes) `delayMicroseconds()`
In order to achieve small code size and accurate timing at short delays, this function accepts an unsigned 16-bit number of microseconds, not an unsigned long like delay(). Like the official cores, delayMicroseconds() does NOT disable interrupts during the delay period - if it did, any interrupts that occurred during the delay would fire as it returned, causing your code to be delayed by them anyway. Instead, in the event that you are doing something where there are tight timing constraints, you need to have interrupts disabled before delayMicroseconds() is called, do your time-critical thing, and then re-enable it. Be aware that millis() can lose time if interrupts are disabled for more than 1ms (1000us).

*If called with a constant argument (which you should strive to always do)* it will use the highly accurate builtin `_delay_us()` from avrlibc. In that case, *the timing will always be exact, to within a clock cycle, and the section below does not apply.*

If called with an argument not known at compile time, it will use an Arduino-style loop. These are pretty damned close, even for short delays (we ensure that even when delayMicroseconds is only called in one place, it will not be inlined, since it relies on the function call overhead); The speeds for which delayMicroseconds has a variable-time delay period implemented are the same on megaTinyCore and DxCore. Not all speeds are available on both cores. The table below applies only when the delay is not known constant at compile time. The minimum is the minimum length of any call to delayMicroseconds() - at 1 MHz, calling delayMicroseconds(x) where x is known only at runtime to have a value of 1 will result in a 16 us delay - that's how long the function call and initialization overhead takes. The resolution is always 1uS unless noted otherwise; at very low speeds, the loop takes several microseconds to run; at it's shortest, it's a 4 clock cycle loop (some speeds use a slower loop - ex, 5/10/20 use a 5-clock-cycle loop, and so on - to make sure the clocks/us is evenly divisible by the length of the loop and to increase the maximum delay length)

| Clock speed | Minimum delay | Maximum delay | Notes |
|-------------|---------------|---------------|----|
|       1 MHz |         16 us |      65535 us | 4us resolution |
|       2 MHz |          8 us |      65535 us | 2us resolution |
|       4 MHz |          4 us |      65535 us |    |
|       5 MHz |          3 us |      65535 us |    |
|       8 MHz |          2 us |      32767 us |    |
|      10 MHz |          2 us |      32767 us |    |
|      12 MHz |        1.5 us |      21845 us | delayMicroseconds(1) gives 1.5us, delayMicroseconds(2) gives 2us |
|      16 MHz |          1 us |      16383 us |    |
|      20 MHz |          1 us |      32767 us |    |
|      24 MHz |          1 us |      21845 us |    |
|      27 MHz |          1 us |      21845 us |    |
|      28 MHz |          1 us |      16383 us |    |
|      30 MHz |          1 us |      21845 us |    |
|      32 MHz |          1 us |      16383 us |    |
|      40 MHz |          1 us |      16383 us | DxCore only - tinyAVRs can't run this fast |
|      44 MHz |          1 us |      16383 us | DxCore only - tinyAVRs can't run this fast |
|      48 MHz |          1 us |      16383 us | DxCore only - tinyAVRs can't run this fast |

Speeds not listed above will use an implementation for the next lowest speed and hence return wrong timing

delayMicroseconds is recommended only for delays of under 1000us - for longer ones, use micros() and a while loop: That way you can leave interrupts enabled unless you need extreme accuracy, and avoid induced millis drift.

#### Recap: delay vs delayMicroseconds()
delay():
* Uses the millis timekeeping machinery unless millis uses RTC or is disabled
  * Requires interrupts to be enabled.
  * Gives the requested delay regardless of interrupts occurring in the middle of it.
  * Works for delays of up to 4.2 billion milliseconds.
* When it can't use the millis machinery, we fall back to avrlibc's implementation.
  * Cycle counting delay, or cycle counting delay called in a loop. for non-constant delay.
  * Will be lenthened by any interrupts occurring in the middle of it.


delayMicroseconds():
* Is always a pure cycle-counting busy-wait delay.
* Interrupts will lengthen the delay
  * Disable interrupts during the timing critical section if that is problematic
* Should be used only for short delays
  * If interrupts are disabled for more than 1000us millis() timekeeping can lose time.
  * Function may not work correctly with values higher than 16383us unless the delay is a compile time known constant.
* Delays should be constants known at compile time (hence subject to constant folding) whenever possible.
* Does not have the bug where certain very short, compile-time-known delays come out shorter than they should. This bug was introduced when LTO support was added and still impacts many cores - LTO would inline the function, but the function was accounting for the call overhead in it's calculated delay.

### (undocumented standard) clockCyclesPerMicrosecond()`
Part of the standard API, but not documented. Does exactly what it says.
```c
#define clockCyclesPerMicrosecond() {(F_CPU) / 1000000L);
```

### (undocumented standard) `clockCyclesToMicroseconds(cycles)`
Part of the standard API, but not documented. Does exactly what the name implies. Note that it always rounds down, like everything in C.
```c
#define clockCyclesToMicroseconds() (cycles / clockCyclesPerMicrosecond());
```

### (undocumented standard) `microsecondsToClockCycles(uint32_t microseconds)`
Part of the standard API, but not documented. Does exactly what the name implies. Note that it always rounds down, like everything in C.
```c
#define microsecondsToClockCycles  icroseconds * clockCyclesPerMicrosecond());
```

### (standard) `_NOP()Execute a single cycle NOP (no operation) instruction which takes up 1 word of flash.

### (DxC/mTC) `_NOPNOP()` or `_NOP2()`
Execute a 2 cycle NOP (no operation) instruction (`rjmp .+0`)which takes up 1 word of flash. (Added 1.3.9)

### (DxC/mTC) `_NOP8()`
Execute an 8 cycle NOP (no operation) instruction sequence (`rjmp .+2 ret rcall .-4`) which takes up 3 words of flash. Saves 1 word vs doing via loop like below (Added 1.3.9)

### (DxC/mTC) `_NOP14()`
Execute an 14 cycle NOP (no operation) instruction sequence (`rjmp .+2 ret rcall .-4 rcall .-6`) which takes up 4 words of flash, 1 word less vs doing via loop like below. (Added 1.3.9)

Longer clock-counting delays aremore efficiently done with the 3 cycle loop (ldi, dec, brne .-4) which gives 3 clocks per iteration plus 1 for the ldi. Can then be padded with `_NOP()` or `_NOPNOP()`for numbers of cycles equal to 3 x iterations + 1 in 4 words, and 3 x iterations + 2 or 3 in 5 words

## Time Manipulation

### `void stop_millis()`
Stop the timer being used for millis, and disable the interrupt. This does not change the current count. Intended for internal use in the future timing and sleep library, so you would stop it before sleeping and start the RTC before going into standby sleep, or powerdown with the PIT.

### `void set_millis(uint32_t newmillis)`
Sets the millisecond timer to the specified number of milliseconds. Be careful if you are setting to a number lower than the current millis count if you have any timeouts ongoing, since standard best practice is to always subtract `(oldmillis - millis())` and these are unsigned. So setting it oldmillis-1 will make it look like 4.2xx billion ms have passed. and the timer will expire.

### (DxC/mTC) `void restart_millis()`
After having stopped millis either for sleep or to use timer for something else and optionally have set it to correct for passage of time, call this to restart it.

### (DxC/mTC) `void nudge_millis(uint16_t ms)`
This is not yet implemented as we assess whether it is a useful or appropriate addition, and how it fits in with set millis().
~Sets the millisecond timer forward by the specified number of milliseconds. Currently only implemented for TCB, TCA implementation will be added in a future release. This allows a clean way to advance the timer without needing to do the work of reading the current value, adding, and passing to `set_millis()`  It is intended for use before  (added becauise *I* needed it, but simple enough).
The intended use case is when you know you're disabling interrupts for a long time (milliseconds), and know exactly how long that is (ex, to update neopixels), and want to nudge the timer
forward by that much to compensate. That's what *I* wanted it for.~

### `_switchInternalToF_CPU()`
Call this if you are running from the internal clock, but it is not at F_CPU - likely when overriding `onClockTimeout()`  `onClockFailure()` is generally useless.

## PWM control
See [Timer Reference](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/Ref_Timers.md)
```text
  void takeOverTCA0()
  void takeOverTCD0()
  void resumeTCA0()
  void resumeTCA1()
  bool digitalPinHasPWMNow(uint8_t p)
  uint8_t digitalPinToTimerNow(uint8_t p)
```
