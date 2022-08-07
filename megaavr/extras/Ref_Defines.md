# Identifying menu options and part identity within sketch or library
It is often useful to identify what options are selected on the menus from within the sketch - one of the most critical is the millis timer the millis timer selection. You may know that you require a certain configuration and want to prevent accidentally compiling with the wrong option and wasting time chasing that instead of a more meaningful issue. *Submenu options are not saved on a sketch or board basis, but an IDE-wide basis* so you can easily get them mixed up.

You cannot check for options that depend on the fuses at compile-time, because we don't know what they will be - sketch can be uploaded separately from setting them.

## Timer identification from within code
You may need to know programmatically what millis source is in use. There are several defines to help with this.
There are the legacy ones:
* `MILLIS_USE_TIMERA0`
* `MILLIS_USE_TIMERB0`
* `MILLIS_USE_TIMERB1`
* `MILLIS_USE_TIMERB2`
* `MILLIS_USE_TIMERB3`
* `MILLIS_USE_TIMERB4`
* `MILLIS_USE_TIMERD0`
* `MILLIS_USE_TIMERRTC`
* `MILLIS_USE_TIMERNONE`

It is often more convenient to use these newer macros, which identify instead testing the millis time,.

### Millis timer and vector
* `MILLIS_TIMER`
* `MILLIS_VECTOR`

#### Millis timer codes
These are the values that the MILLIS_TIMER may be defined as:

| Name            | Numeric value | Meaning                             |
|-----------------|---------------|----------------------------------------------------------
| `NOT_ON_TIMER`  |          0x00 | Millis is disabled
| `NONE`          |          0x00 | Millia ia disabled
| `TIMERA0`       |          0x10 | Millis is generated from TCA0
| `TIMERA1`       |          0x08 | Millis is generated from TCA1
| `TIMERB0`       |          0x20 | Millis is generated from TCB0
| `TIMERB1`       |          0x21 | Millis is generated from TCB1
| `TIMERB2`       |          0x23 | Millis is generated from TCB2
| `TIMERB3`       |          0x23 | Millis is generated from TCB3
| `TIMERB4`       |          0x24 | Millis is generated from TCB4
| `TIMERD0`       |          0x40 | Not planned for implementation on DxCore.
| `TIMERRTC`      |          0x80 | (not yet implemented on DxCore)
| `TIMERRTC_XTAL` |          0x81 | (not yet implemented on DxCore)
| `TIMERRTC_XOSC` |          0x82 | (not yet implemented on DxCore)


However, the other bits are also used when digitalPinToTimer/Now() are used:

* For TCA's, the three lowest bits indicate the channel of the pin.
* For TCBs, if 0x30 is also set, that's the alternate pin.
* For TCD it is the form 0b0xx10yyy where xx is the WO channel and yyy is the PORTMUX setting.

To to find the millis timer:
```c
if (MILLIS_TIMER & 0x40) {
  //timer D (not available on DxCore)
} else if (MILLIS_TIMER & 0x10) {
  //TCA0
} else if (MILLIS_TIMER & 0x08){
  //TCA1
} else if (MILLIS_TIMER & 0x20) {
  //timer B, look to othr nybble to find wheich one
} else if (MILLIS_TIMER & 0x80) {
  //RTC
}


```
### Timer identifier interpretation
These are 8-bit values.
0x10-0x15 are TCA0.
0x08-0x0C is TCA1
0x20-0x2F is a TCB
0x30-0x3F is a TCB alt pin (not yet implemented)
0x40-0x77 is a TCD pin (high nybble is the bit within the group, low nybble is the mux)
0x80  is the DAC output pin


### Using to check that correct menu option is selected
If your sketch requires that the B0 is used as the millis timer, for example:

```c++
#ifndef MILLIS_USE_TIMERB2
  #error "This sketch is written for use with TCB2 as the millis timing source"
#endif
```

## Identifying part family within sketch
When writing code that may be compiled for a variety of target chips, it is often useful to detect which chip it is running on.

This core provides an additional define depending on the number of pins on the part and it's family (which identifies it's peripheral selection) and the DxCore version:
* `MEGATINYCORE_MCU` - The part number, as a number (ex, 3224 or 1607)
* `MEGATINYCORE_SERIES` - 0, 1, or 2 depending on whether it's a 0, 1, or 2-series part.
* `__AVR_ATtinyxyz__` - where z, and optionally x and y are substituted with those parts of the part number, ex: `__AVR_ATtinyxy4__` or `__AVR_ATtinyx02__`. The fully substituted form is provided by the compiler.
* `_AVR_PINCOUNT` - The number of physical pins
* `_AVR_FLASH` - Flash size, in KB - these three can be used to print the human readable part number easily.

## Peripheral detection
These do exactly what you expect.
* `_AVR_AC_COUNT`
* `_AVR_ADC_COUNT`
* `_AVR_DAC_COUNT`
* `_AVR_OPAMP_COUNT`
* `_AVR_LUT_COUNT`
* `_AVR_TCA_COUNT`
* `_AVR_TCB_COUNT`
* `_AVR_TCD_COUNT`
* `_AVR_TWI_COUNT`
* `_AVR_USART_COUNT`


## Identifying DxCore version
These define the version of the core:
* `MEGATINYCORE` - megaTinyCore version number, as string. If it's "Unknown 2.5.0+" or similar, see below.
* `MEGATINYCORE_MAJOR` - megaTinyCore major version
* `MEGATINYCORE_MINOR` - megaTinyCore minor version
* `MEGATINYCORE_PATCH` - megaTinyCore patch version
* `MEGATINYCORE_RELEASED` - 1 if a released version, 0 if unreleased (ie, installed from github between releases).
* `MEGATINYCORE_NUM` - megaTinyCore version, as unsigned long.

```c++
Serial.println(MEGATINYCORE);
Serial.print(MEGATINYCORE_MAJOR);
Serial.print(' '); // using ' ' instead of " " saves a tiny amount of flash!
Serial.print(MEGATINYCORE_MINOR);
Serial.print(' ');
Serial.print(MEGATINYCORE_PATCH);
Serial.print(' ');
Serial.println(MEGATINYCORE_RELEASED);
Serial.println(MEGATINYCORE_NUM,HEX);

```
Will produce output like:
```text
2.5.0
2 5 0 1
02050001
```
or - if your non-Arduino IDE is not able to handle the escaping and you happened to be using a 2.5.1 github build:
```text
Unknown 2.5.0+
2 5 1 0
02050100
```
The various numeric representations of version are of interest to those writing libraries or code meant to be run by others. They are meant to ensure that there is always an easy way to test against specific versions (for when a release contains a regression), as well as an "X or better" type test. Warning: In some third party development environments, the `MEGATINYCORE` define is lost. Since that's how we tell people to detect if DxCore is in use (`#if defined(MEGATINYCORE)`) we now detect that and put define it as "Unknown 1.3.7+" (where the number changes whenever I happen to notice it while editing the file for a new version). `MEGATINYCORE_NUM` is by far the easiest way to detect if a certain fix or patch is present.


## __AVR_ARCH__
This can be set to 102, 103, or 104 depending on flash size - the compiler sets this, and it works for ALL AVR parts. Anything with it set to 103 has fully mapped flash. Anything with 102 has 64k, and 104 indicates a part with 128k of flash - on megaTinyCore, it's always 103, because no parts have too much flash to map it all.
* `__AVR_ARCH__ == 103` - All parts where all of the flash is mapped in the data space. This means Dx and Ex-series parts with 32k or less of flash, tinyAVR 0/1/2-series, and megaAVR 0-series.
* `__AVR_ARCH__ == 104` - Parts with 128Kb of flash, mapped flash is split into 4 sections (AVR128DA, AVR128DB).
* `__AVR_ARCH__ == 102` - Parts with 64Kb of flash, mapped flash is split into 2 sections (AVR64DA, AVR64DB).

## Core feature detection
* `DEVICE_PORTMUX_TCA = 1` 1 = each wave output channel can be moved individually, like tinyAVRs. 2 = Whole thing moved at once
There are a number of macros for determining what (if any) features the core supports (these are shared with megaTinyCore and ATTinyCore (2.0.0+)
* `CORE_HAS_FASTIO = 2` - If defined as 1 or higher. indicates that `digitalWriteFast()` and `digitalReadFast()` is available. If undefined or defined as 0, these are not available. If 2 or higher, s it is for all parts on megaTinyCore, `pinModeFast()` is also available. If defined as -1, there are no `digital____Fast()` functions, but with constant pins, these functions are optimized aggressively for speed and flash usage (though not all the way down to 1 instruction).
* `CORE_HAS_OPENDRAIN = 1` - If defined as 1, indicates that `openDrain()` and (assuming `CORE_HAS_FASTIO` is >= 1) `openDrainFast()` are available. If undefined or 0, these are not available.
* `CORE_HAS_PINCONFIG = 1` - If defined as Indicates that `pinConfigure()` is available. If not defined or defined as 0, it is not available.
* `CORE_HAS_TIMER_TAKEOVER = 1` - if defined as 1, `takeOverTCxn()` functions are available to let user code take full control of TCA0 or TCD0.
* `CORE_HAS_TIMER_RESUME = 0`- if defined as 1, the corresponding `resumeTCxn()` functions, which reinitialize them and return them to their normal core-integrated functions, are available. Not available on megaTinyCore.
* `ADC_NATIVE_RESOLUTION = 10 or 12`- This is the maximum resolution, in bits, of the ADC without using oversampling.
* `ADC_NATIVE_RESOLUTION_LOW = 8` - The ADC has a resolution setting that chooses between ADC_NATIVE_RESOLUTION, and a lower resolution.
* `ADC_DIFFERENTIAL = 0 or 2` - This is defined as 1 if the part has a basic differential ADC (no gain, and V<sub>analog_in</sub> constrained to between Gnd and V<sub>Ref</sub>), and 2 if it has a full-featured one. It does not indicate whether said differential capability is exposed by the core. This is 0 for 0/1-series and 2 for 2-series. If it's defined as -1, it has differential capability in the way that classic AVRs do. See also `CORE_HAS_ANALOG_DIFF`
* `SUPPORT_LONG_TONES = 0 or 1` - On some modern AVR cores, an intermediate value in the tone duration calculation can overflow (which is timed by counting times the pin is flipped) leading to a maximum duration of 4.294 million millisecond. This is worst at high frequencies, and can manifest at durations as short as 65 seconds worst case. Working around this, however, costs some flash, so megaTinyCore only supports long tones on parts with more than 8k of flash. If `SUPPORT_LONG_TONES` is defined as 1, as long as (duration * frequency)/500 < 4.294 billion, the duration will not be truncated. If it is defined as 0, the bug was known to the core maintainer and they chose not to fully correct it (eg, to save flash) but took the obvious step to reduce the impact, it will be truncated if (duration * frequency) exceeds 4.294 billion. If `SUPPORT_LONG_TONES` is not defined at all, the bug may be present in its original form, in which case the duration will be truncated if (duration * frequency) exceeds 2.14 billion.
* `CORE_HAS_ANALOG_ENH = 1` - If defined as 1, `analogReadEnh()` (enhanced analogRead) is available. Otherwise, it is not; it is 1 for all parts on recent versions of megaTinyCore.
* `CORE_HAS_ANALOG_DIFF = 0 or 1` - If defined as 1, `analogReadDiff()` (differential enhanced analogRead) is available. Otherwise, it is not. 1 for 2-series, 0 for others. It has same features as enhanced, except that it takes a differential measurement. If this is -128, (128 unsigned), it is a classic AVR, not a modern one with a differential ADC, and the core's analogRead implementation accepts the constants listed in the core documentation to make an analogRead

## Hardware feature detection
* `ADC_MAX_OVERSAMPLED_RESOLUTION = 13 or 17` - If either `CORE_HAS_ANALOG_ENH` or `CORE_HAS_ANALOG_DIFF` is 1, this will be defined as the maximum resolution obtainable automatically via oversampling and decimation using those functions.
* `ADC_MAXIMUM_GAIN = 0 or 16` - The 2-series has a programmable gain amplifier on it's ADC; the others do not. If this is defined as a positive number, it is the maximum gain available (16 on the EA). If this is defined as 0 that means there is no way to amplify an analog signal we might want to measure (if this is a problem you need external op-amps). If this is defined as -1 (or 255 if stuffed into a uint8), there are one or more `OPAMP` peripherals available which could be directed towards the same purpose, though more deliberation and part specific work would be needed; they may be more flexible in some ways, but they are very rigid in others (for example, in that there is only one pin option for them. If it is defined as -128 (128 in a uint8) there is a gain stage on the differential ADC, but it is specified along with the pair of pins, not with separate configuration options. This also means that it's *NOT* a modern AVR! This is generally what is done on all classic AVRs with a differential ADC) so the available gain options depend on which pins are being measured, and there is a different procedure to use it, as detailed in the core documentation (ex, ATTinyCore 2.0.0 and later). If it is undefined, there is unheard-of no support exposed through the core, and it may not be a feature of the hardware at all.
* `PORT_ID_INLVL = 0` - Some Dx-series parts have an input level option on a per-pin basis. TinyAVRs do not.

## Compatibility macros
Occasionally Microchip has not kept the names of registers or bitfields consistent between families of parts, even when the function was not changed. In some cases these have even been changed between versions of the ATpack! The places where we've papered over identical register functionality with different names are:
* The `GPIO`/`GPIOR` registers - The old names will work too, `GPIORn` is recommended for maximum compatibility.
* The `TCA_SINGLE_EVACT` bitfield - This was changed to `TCA_SINGLE_EVACTA` on the Dx, as were it's group codes. Both names will work, permitting code portability from tinyAVR to Dx.
* The `RTC_CLKSEL` group codes were renamed on non-tiny parts. Both names will work on both cores, permitting code portability from tinyAVR to Dx.
* The `EVSYS` user registers were numbered but not named on 0/1-series. The new names work on those parts. The names with numbers only work on 0/1-series.


## Identifying Timers
Each timer has a number associated with it, as shown below. This may be used by preprocessor macros (`#if` et. al.) or `if()` statements to check what `MILLIS_TIMER` is, or to identify which timer (if any) is associated with a pin using the `digitalPinToTimer(pin)` macro. Defines are available on all parts that the core supports, whether or not the timer in question is present on the part (ie, it is safe to use them in tests/code without making sure that the part has that timer). There are two very closely related macros for determining pin timers:
* `digitalPinToTimer()` tells you what timer (if any) the pin is associated with by default. This is a constant, when the argument is constant, the optimizer will optimize it away.
* `digitalPinToTimerNow()` tells you what timer (if any) the pin is associated with currently. On megaTinyCore, this is either the result of `digitalPinToTimer()` unless that timer has been "taken over" by user code with `takeOverTCA0()` or `takeOverTCD0()`. On modern AVR cores like Dx-core which support use of `analogWrite()` even when the `PORTMUX.TCxROUTEA` register (where `x` is `A` or `D`) has been changed, this will return the timer currently associated with that pin. megaTinyCore does NOT support non-default timer pin mappings with `analogWrite()`- so if `PORTMUX.TCAROUTEA` (2-series) or `PORTMUX.CTRLC` (0/1-series) has been altered, this will not not reflect that.

```c
#define NOT_ON_TIMER 0x00   // if MILLIS_TIMER set to this, millis is disabled. If digitalPinToTimer() gives this, it is not a PWM pin
#define TIMERA0 0x10        // Present on all modern AVRs
#define TIMERA1 0x08        // Not present on any tinyAVR; only on AVR DA and DB-series parts with at least 48 pins and AVR EA-series parts
#define TIMERB0 0x20        // Present on all modern AVRs
#define TIMERB1 0x21        // Not present on any tinyAVR 0-series or tinyAVR 1-series parts with 2, 4, or 8k of flash
#define TIMERB2 0x22        // Not present on any tinyAVR
#define TIMERB3 0x23        // Not present on any tinyAVR
#define TIMERB4 0x24        // Not present on any tinyAVR
#define TIMERB5 0x25        // Not present on any tinyAVR
#define TIMERD0 0x40        // Present only on tinyAVR 1-series and AVR Dx-series parts.
#define DACOUT 0x80         // Not a timer, obviously - but used like one by analogWrite()
#define TIMERRTC 0x90       // Usable for millis only
#define TIMERRTC_XTAL 0x91  // Usable for millis only
#define TIMERRTC_XOSC 0x92  // Usable for millis only
```
