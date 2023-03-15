# Identifying menu options and part identity within sketch or library
It is often useful to identify what options are selected on the menus from within the sketch - one of the most critical is the millis timer the millis timer selection. You may know that you require a certain configuration and want to prevent accidentally compiling with the wrong option and wasting time chasing that instead of a more meaningful issue. *Submenu options are not saved on a sketch or board basis, but an IDE-wide basis* so you can easily get them mixed up.

You cannot check for options that depend on the fuses at compile-time, because we don't know what they will be - sketch can be uploaded separately from setting them.

**Note:** Most of this document is the same for DxCore and megaTinyCore. This both makes my life easier, and likely yours as well, since these are most useful when writing code that works across a wide range of parts; there's no reason we need to send you looking at two versions of this file. Only the "deprecated" constants are not universal.

## Timer identification from within code
You may need to know programmatically what millis source is in use. There are several defines to help with this.
There are the legacy ones:
* `MilLIS_USE_TIMERA0`
* `MILLIS_USE_TIMERA1`
* `MILLIS_USE_TIMERB0`
* `MILLIS_USE_TIMERB1`
* `MILLIS_USE_TIMERB2`
* `MILLIS_USE_TIMERB3`
* `MILLIS_USE_TIMERB4`
* `MILLIS_USE_TIMERD0`
* `MILLIS_USE_TIMERE0` - for the unreleased AVR EB-series. This may be removed upon datasheet release if it is apparent that TCE is not suitable for millis.
* `MILLIS_USE_TIMERF0` - for the unreleased AVR EB-series. This may be removed upon datasheet release if it is apparent that TCF is not suitable for millis.
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
| `TIMERB2`       |          0x22 | Millis is generated from TCB2
| `TIMERB3`       |          0x23 | Millis is generated from TCB3
| `TIMERB4`       |          0x24 | Millis is generated from TCB4
| `TIMERD0`       |          0x40 | Millis is generated from TCD0 (megaTinyCore only)
| `TIMERRTC`      |          0x80 | Millis is generated from the RTC from internal low frequency osc. (megaTinyCore only currently)
| `TIMERRTC_XTAL` |          0x81 | Millis is generated from the RTC from external crystal oscillator. (megaTinyCore only currently)
| `TIMERRTC_XOSC` |          0x82 | Millis is generated from the RTC from external active oscillator. (megaTinyCore only currently)

To to find the millis timer:
```c
if (MILLIS_TIMER & 0x40) {
  //timer D (not available on DxCore)
} else if (MILLIS_TIMER & 0x10) {
  //TCA0
} else if (MILLIS_TIMER & 0x08){
  //TCA1
} else if (MILLIS_TIMER & 0x20) {
  //timer B, look to othr nybble to find which one
} else if (MILLIS_TIMER & 0x80) {
  //RTC
}


```
### Timer identifier interpretation (digitalPinToTimer() return values)
These are 8-bit values. They are fully enumerated and much more detail provided near the end of this document.

```text
0x10-0x15 are TCA0. The three low bits are the mux option (DxCore only - other.
0x08-0x0D is TCA1. The three low bits are the mux option (DxCore only).
0x20-0x2F is a TCB
0x30-0x3F is a TCB alt pin (not yet implemented for PWM)
0x40-0x77 is a TCD pin (high nybble is the bit within the group, low nybble is the mux. On megaTinyCore, this is always 0x40, as there is no mux for TCD.)
0x80  is the DAC output pin
TCE and TCF details pending release of more information
```

### Using to check that correct menu option is selected
If your sketch requires a certain timer NOT be used for millis, or that a certain timer nust be the timekeeping timer, you should check these #defines and be sure to either `#error` out, or at least emit a `#warning`.

```c++
#ifndef MILLIS_USE_TIMERB2
  #error "This sketch is written for use with TCB2 as the millis timing source"
#endif
```

### Testing if millis enabled
The names of these two functions are #defined as themselves if the function can be used. On DxCore, `millis` is never defined if `micros` is not, since there are no timekeeping options which suppory millis but not micros. On megaTinyCore, if the RTC is used, millis, but not micros, will be available, so `defined(micros)` is false but `defined(millis)` is true.

## Identifying part family within sketch
When writing code that may be compiled for a variety of target chips, it is often useful to detect which chip it is running on.Defines of the form `__AVR_partname__` are provided by the toolchain. However, it is often MORE useful to not have to have a giant block of #ifdefs, and have the part numbers distilled down to something more that can be checked more intuitively, since what you care about is what series.

This core provides an additional define depending on the number of pins on the part and it's family (which identifies it's peripheral selection) and the core version:
* For megaTinyCore:
  * `__AVR_ATtinyxyz__` - where z, and optionally x and y are substituted with those parts of the part number, ex: `__AVR_ATtinyxy4__` or `__AVR_ATtinyx02__`. The fully substituted form is provided by the compiler.
* For DxCore:
  * The toolchain provides `__AVR_AVRxx(D|E)(A|B|D)yy` where x is flash size in kb, and y is pincount. These are what is used to deduce flash and pincount parameters below.

The below parameter defines should be used in preference to the above.

## Part basic parameter determination
* `_AVR_FAMILY` - String - "DA", "DB", "DD, "DU", "EA", "EB" or "T0", "T1", "T2" depending on what kind of part it is.
* `_AVR_PINCOUNT` - The number of physical pins
* `_AVR_FLASH` - Flash size, in KB
* `__AVR_DA__`
* `__AVR_DB__`
* `__AVR_DD__`
* `__AVR_EA__` (unreleased, but official, will be DxCore)
* `__AVR_EB__` (unreleased, but official, will be DxCore)
* `__AVR_TINY_0__`
* `__AVR_TINY_1__`
* `__AVR_TINY_2__`
* `MEGATINYCORE_MCU` - The number after "ATtiny" in the part name.
* `MEGATINYCORE_SERIES` - the part series, 0, 1, or 2.

## Peripheral detection
Most of these need no explanation - they just tell you how many peripherals of each type you have. Those that don't need an explanation don't have one.
* `_AVR_AC_COUNT`
* `_AVR_ADC_COUNT` - 1, or 2 on the tinyAVR 1+series.
* `_AVR_DAC_COUNT`
* `_AVR_DAC_VERSION` - 0, 1, or 2. 0 is 8-bit (tinyAVR), 1 is 10-bit (Dx-series), 2 is 10-bit with OUTRANGE (Ex-series)
* `_AVR_OPAMP_COUNT`
* `_AVR_EVSYS_COUNT` - The number of total EVSYS channels.
* `_AVR_EVSYS_SYNC` - The number of synchronous EVSYS channels. 1 or 2. Only defined on tinyAVR 0/1.
* `_AVR_EVSYS_ASYNC` - the number of async EVSYS channels. 2 or 4. Only defined for the tinyAVR 0/1.
* `_AVR_EVSYS_VERSION` - 0, 1, 2, or 3 - 0 is the tinyAVR 0/1 shitshow, 1 is mega0, nearly identical to version 2, the Dx-series version, except that megaAVR calls the strobe register STROBE instead of SWEVENTx. 3 is the AVR Ex-series version, with EVGENCTRL registers for ports and RTC, and with all generator channels being created equal.
* `_AVR_LUT_COUNT`
* `_AVR_TCA_COUNT`
* `_AVR_TCB_COUNT`
* `_AVR_TCD_COUNT`
* `_AVR_TWI_COUNT`
* `_AVR_USART_COUNT`

The EVSYS defines are very useful if you are using EVSYS without event, but even if you are using event, you want to, for example, select channels to minimize the functionality you're blocking off with your configuration, unless you're using an Ex-series, where all channels are created equal.

## Identifying core version
These define the version of the core:
* For megaTinyCore:
  * `MEGATINYCORE` - megaTinyCore version number, as string. If it's "Unknown 2.5.0+" or similar, see below.
  * `MEGATINYCORE_MAJOR` - megaTinyCore major version
  * `MEGATINYCORE_MINOR` - megaTinyCore minor version
  * `MEGATINYCORE_PATCH` - megaTinyCore patch version
  * `MEGATINYCORE_RELEASED` - 1 if a released version, 0 if unreleased (ie, installed from github between releases).
  * `MEGATINYCORE_NUM` - megaTinyCore version, as unsigned long.
* For DxCore:
  * `DXCORE` - DxCore version number, as string. If it's "Unknown 1.5.0+" or similar, see below.
  * `DXCORE_MAJOR` - DxCore major version
  * `DXCORE_MINOR` - DxCore minor version
  * `DXCORE_PATCH` - DxCore patch version
  * `DXCORE_RELEASED` - 1 if a released version, 0 if unreleased (ie, installed from github between releases).
  * `DXCORE_NUM` - DxCore version, as unsigned long.
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
Identical behavior can be seen with DxCore, just with different names for the variables.

Note also that Serial.`printHexln()` can also be used; this is slightly smaller and faster, and has some additional useful features, See [Serial Reference](Ref_Serial.md)

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
* `ADC_DIFFERENTIAL = 0, 1, or 2` - This is defined as 1 if the part has a basic differential ADC (no gain, and V<sub>analog_in</sub> constrained to between Gnd and V<sub>Ref</sub>, as on the Dx-series), and 2 if it has a full-featured one. It does not indicate whether said differential capability is exposed by the core. This is 0 for 0/1-series and 2 for 2-series. Classic AVRs on ATTinyCore 2.0.0+ will define this as -1 if they have a differential ADC, as their ADC takes differential channels in a totally different way. See also `CORE_HAS_ANALOG_DIFF`
* `SUPPORT_LONG_TONES = 0 or 1` - On some modern AVR cores, an intermediate value in the tone duration calculation can overflow (which is timed by counting times the pin is flipped) leading to a maximum duration of 4.294 million millisecond. This is worst at high frequencies, and can manifest at durations as short as 65 seconds worst case. Working around this, however, costs some flash, so megaTinyCore only supports long tones on parts with more than 8k of flash. If `SUPPORT_LONG_TONES` is defined as 1, as long as (duration * frequency)/500 < 4.294 billion, the duration will not be truncated. If it is defined as 0, the bug was known to the core maintainer and they chose not to fully correct it (eg, to save flash) but took the obvious step to reduce the impact, it will be truncated if (duration * frequency) exceeds 4.294 billion. If `SUPPORT_LONG_TONES` is not defined at all, the bug may be present in its original form, in which case the duration will be truncated if (duration * frequency) exceeds 2.14 billion.
* `CORE_HAS_ANALOG_ENH = 1` - If defined as 1, `analogReadEnh()` (enhanced analogRead) is available. Otherwise, it is not; it is 1 for all parts on recent versions of megaTinyCore.
* `CORE_HAS_ANALOG_DIFF = 0 or 1` - If defined as 1, `analogReadDiff()` (differential enhanced analogRead) is available. Otherwise, it is not. 1 for 2-series, 0 for others. It has same features as enhanced, except that it takes a differential measurement. If this is -128, (128 unsigned), it is a classic AVR, not a modern one with a differential ADC, and the core's analogRead implementation accepts the constants listed in the core documentation to make an analogRead of a differential pair.

## Hardware feature detection
* `ADC_MAX_OVERSAMPLED_RESOLUTION` = 13 (tinyAVR0/1), 15 (Dx-series) or 17 (tinyAVR 2 or Ex-series) - If either `CORE_HAS_ANALOG_ENH` or `CORE_HAS_ANALOG_DIFF` is 1, this will be defined as the maximum resolution obtainable automatically via oversampling and decimation using those functions.
* `ADC_MAXIMUM_GAIN` is 0 (DA, DD, DU, tinyAVR0/1), -1 (DB), or 16 (EA, EB, tinyAVR 2) - Some parts' ADC is blessed with a Programmable Gain Amplifier (PGA) amplifier, often used for differential readings (though modern AVRs can use it for single-ended readings, if they have it - not that it's terribly useful).  The Dx-series are not among them, though the EA will have it. If this is defined as a positive number, it is the maximum gain available (16 on the EA). If this is defined as 0 that means there is no way to amplify an analog signal we might want to measure (if this is a problem you need external opamps). If this is defined as -1 (or 255 if stuffed into a uint8), there are one or more `OPAMP` peripherals available which could be directed towards the same purpose, though more deliberation and part specific work would be needed; they may be more flexible in some ways, but they are very rigid in others (for example, in that there is only one pin option for them. If it is defined as -128 (128 in a uint8) there is a gain stage on the differential ADC, but it is specified along with the pair of pins, not with separate configuration options. This also means that it's *NOT* a modern AVR! This is generally what is done on all classic AVRs with a differential ADC) so the available gain options depend on which pins are being measured, and there is a different procedure to use it, as detailed in the core documentation (ex, ATTinyCore 2.0.0 and later). If it is undefined, there is definitley no support exposed through the core, and it may not be a feature of the hardware at all.
* `_AVR_PORT_INLVL` is 1 if the INLVL bit is present. If undefined or 0, that feature is not available.

## Compatibility macros
Occasionally Microchip has not kept the names of registers or bitfields consistent between families of parts, even when the function was not changed. In some cases these have even been changed between versions of the ATpack! The places where we've papered over identical register functionality with different names are:
* The `GPIO`/`GPIOR`/`GPR` registers - they are now `GPR.GPRn`. The old names will work too, `GPIORn` is recommended for maximum compatibility.
* The `TCA_SINGLE_EVACTA` bitfield - formerly known as `TCA_SINGLE_EVACT` and it's group codes. The old names will work too, permitting code portability from tinyAVR to Dx.
* The `RTC_CLKSEL` group codes that were renamed on non-tiny parts. The old names will work too, permitting code portability from tinyAVR to Dx.
* The `CLKCTRL_SELHF_CRYSTAL_gc` option on DB-series parts which was renamed to `CLKCTRL_SELHF_XTAL_gc`. This goes both ways, as different ATpacks name them differently.
* All things `CLKCTRL_FREQSEL` related, which had the E dropped in some ATPACK versions. This goes both ways, as different ATpacks name them differently.
* All multi-bit bitfields - the `_bp` and `_bm` defines now have an underscore before the bit number. There are about a thousand impacted defines. At least they had the decency to bump the major version.


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
#define TIMERB2 0x22        // Not present on any tinyAVR, only 28+ pin Dx-series
#define TIMERB3 0x23        // Not present on any tinyAVR, only 48/64-pin Dx-series
#define TIMERB4 0x24        // Not present on any tinyAVR, only 64-pin Dx-series
#define TIMERD0 0x40        // Present only on tinyAVR 1-series and AVR Dx-series parts.
#define DACOUT 0x80         // Not a timer, obviously - but used like one by analogWrite()
#define TIMERRTC 0x90       // Usable for millis only, not PWM
#define TIMERRTC_XTAL 0x91  // Usable for millis only, not PWM
#define TIMERRTC_XOSC 0x92  // Usable for millis only, not PWM
```
