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
When writing code that may be compiled for a variety of target chips, it is often useful to detect which chip it is running on.Defines of the form `__AVR_partname__` are provided by the toolchain. However, it is often MORE useful to not have to have a giant block of #ifdefs, and have the part numbers distilled down to something more that can be checked more intuitively, since what you care about is the family, number of pins, or occasionally size of the flash.

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
* `__AVR_DA__` (official, defined by avrlibc))
* `__AVR_DB__` (official, defined by avrlibc))
* `__AVR_DD__` (official, defined by avrlibc))
* `__AVR_DU__` (unreleased - official, defined by avrlibc)
* `__AVR_EA__` (official, defined by avrlibc)
* `__AVR_EB__` (unreleased - official, defined by avrlibc)
* `__AVR_TINY_0__` (unofficial - defined by the core)
* `__AVR_TINY_1__` (unofficial - defined by the core)
* `__AVR_TINY_2__` (unofficial - defined by the core)
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
The various numeric representations of version are of interest to those writing libraries or code meant to be run by others. They are meant to ensure that there is always an easy way to test against specific versions (for when a release contains a regression), as well as an "X or better" type test. Warning: In some third party development environments, the `DXCORE` define is lost. Since that's how we tell people to detect if DxCore is in use (`#if defined(DXCORE)`) we now detect that and put define it as "Unknown 1.3.7+" (where the number changes whenever I happen to notice it while editing the file for a new version). `DXCORE_NUM` is by far the easiest way to detect if a certain fix or patch is present.


## `__AVR_ARCH__`
This can be set to 102, 103, or 104 depending on flash size - the compiler sets this, and it works for ALL AVR parts. Anything with it set to 103 has fully mapped flash. Anything with 102 has 64k, and 104 indicates a part with 128k of flash - on megaTinyCore, it's always 103, because no parts have too much flash to map it all.
* `__AVR_ARCH__ == 103` - All parts where all of the flash is mapped in the data space. This means Dx and Ex-series parts with 32k or less of flash, tinyAVR 0/1/2-series, and megaAVR 0-series.
* `__AVR_ARCH__ == 104` - Parts with 128Kb of flash, mapped flash is split into 4 sections (AVR128DA, AVR128DB).
* `__AVR_ARCH__ == 102` - Parts with 64Kb of flash, mapped flash is split into 2 sections (AVR64DA, AVR64DB).

## Core feature detection
* `DEVICE_PORTMUX_TCA = 1` 1 = each wave output channel can be moved individually, like tinyAVRs. 2 = Whole thing moved at once
There are a number of macros for determining what (if any) features the core supports (these are shared with megaTinyCore and ATTinyCore (2.0.0+)
* `CORE_HAS_FASTIO = 2` - If defined as 1 or higher. indicates that `digitalWriteFast()` and `digitalReadFast()` is available. If undefined or defined as 0, these are not available. If 2 or higher, as it is for all parts on megaTinyCore, `pinModeFast()` is also available. If defined as -1, there are no `digital____Fast()` functions, but with constant pins, these functions are optimized aggressively for speed and flash usage (though not all the way down to 1 instruction). If not defined at all, the core may not be aggressively optimized.
* `CORE_HAS_OPENDRAIN = 1` - If defined as 1, indicates that `openDrain()` and (assuming `CORE_HAS_FASTIO` is >= 1) `openDrainFast()` are available. If undefined or 0, these are not available.
* `CORE_HAS_PINCONFIG = 1` - If defined as Indicates that `pinConfigure()` is available. If not defined or defined as 0, it is not available.
* `CORE_HAS_TIMER_TAKEOVER = 1` - if defined as 1 or more, `takeOverTCxn()` functions are available to let user code take full control of TCA0, TCA1 and/or TCD0. If a part, for which support has been provided by one of my cores, in a release, without a takeOverTCxn() function for a timer it has, and later gets a takeOverTCxn function for that timer, there would need to be a CORE_HAS_TIMER_TAKEOVER = 2 for versions of the core that have that new function. I hope, however, to give all new timers a takeOver function if one is appropriate with the first release of support for parts having those timers.
* `CORE_HAS_TIMER_RESUME = 0`- if defined as 1, the corresponding `resumeTCAn()` functions, which reinitialize them and return them to their normal core-integrated functions, are available. Different values in the future might be used to indicate that resume timers other than TCAn, or which might have an option to resume TCD0. Not available on megaTinyCore.
* `ADC_NATIVE_RESOLUTION = 10 or 12`- This is the maximum resolution, in bits, of the ADC without using oversampling.
* `ADC_NATIVE_RESOLUTION_LOW = 8` - The ADC has a resolution setting that chooses between ADC_NATIVE_RESOLUTION, and a lower resolution.
* `ADC_DIFFERENTIAL = 0, 1, or 2` - This is defined as 1 if the part has a basic differential ADC (no gain, and V<sub>analog_in</sub> constrained to between Gnd and V<sub>Ref</sub>, as on the Dx-series), and 2 if it has a full-featured one. It does not indicate whether said differential capability is exposed by the core. This is 0 for 0/1-series and 2 for 2-series. Classic AVRs on ATTinyCore 2.0.0+ will define this as -1 if they have a differential ADC, as their ADC takes differential channels in a totally different way, and will not define it at all if they do not. See also `CORE_HAS_ANALOG_DIFF`
* `SUPPORT_LONG_TONES = 0 or 1` - On some modern AVR cores, an intermediate value in the tone duration calculation can overflow (which is timed by counting times the pin is flipped) leading to a maximum duration of 4.294 million millisecond. This is worst at high frequencies, and can manifest at durations as short as 65 seconds worst case. Working around this, however, costs some flash, so megaTinyCore only supports long tones on parts with more than 8k of flash. If `SUPPORT_LONG_TONES` is defined as 1, as long as (duration * frequency)/500 < 4.294 billion, the duration will not be truncated. If it is defined as 0, the bug was known to the core maintainer and they chose not to fully correct it (eg, to save flash) but took the obvious step to reduce the impact, it will be truncated if (duration * frequency) exceeds 4.294 billion. If `SUPPORT_LONG_TONES` is not defined at all, the bug may be present in its original form, in which case the duration will be truncated if (duration * frequency) exceeds 2.14 billion.
* `CORE_HAS_ANALOG_ENH = 1` - If defined as 1, `analogReadEnh()` (enhanced analogRead) is available. Otherwise, it is not; it is 1 for all parts on recent versions of megaTinyCore.
* `CORE_HAS_ANALOG_DIFF = 0 or 1` - If defined as 1, `analogReadDiff()` (differential enhanced analogRead) is available. Otherwise, it is not. 1 for 2-series (which has a differential ADC) and for the Ex/Dx-series, which have at least a mediocre , 0 for others (as they do not). It has same features as enhanced, except that it takes a differential measurement. If this is -128, (128 unsigned), it is a classic AVR, not a modern one with a differential ADC, and the core's analogRead implementation accepts the constants listed in the core documentation to make an analogRead of a differential pair.
* `CORE_HAS_ERRORFUNS = 1` - Newly added define - it's become apparent that reliance on the core's badArg and badCall functions was impacting portability of library code; this can be tested for by libraries which use badArg() and badCall() so that if it's not present, the library can include a copy of the definitions. Mostly meant for internal use, though some other people *might* be using badArg() or badCall() in their libraries?

## Hardware feature detection
* `ADC_MAX_OVERSAMPLED_RESOLUTION` = 13 (tinyAVR0/1), 15 (Dx-series) or 17 (tinyAVR 2 or Ex-series) - If either `CORE_HAS_ANALOG_ENH` or `CORE_HAS_ANALOG_DIFF` is 1, this will be defined as the maximum resolution obtainable automatically via oversampling and decimation using those functions.
* `ADC_MAXIMUM_GAIN` is 0 (DA, DD, DU, tinyAVR0/1), -1 (DB), or 16 (EA, EB, tinyAVR 2) - Some parts' ADC is blessed with a Programmable Gain Amplifier (PGA) amplifier, often used for differential readings (though modern AVRs can use it for single-ended readings, if they have it - not that it's terribly useful).  The Dx-series are not among them, though the EA will have it. If this is defined as a positive number, it is the maximum gain available (16 on the EA). If this is defined as 0 that means there is no way to amplify an analog signal we might want to measure (if this is a problem you need external opamps). If this is defined as -1 (or 255 if stuffed into a uint8), there are one or more `OPAMP` peripherals available which could be directed towards the same purpose, though more deliberation and part specific work would be needed; they may be more flexible in some ways, but they are very rigid in others (for example, in that there is only one pin option for them. If it is defined as -128 (128 in a uint8) there is a gain stage on the differential ADC, but it is specified along with the pair of pins, not with separate configuration options. This also means that it's *NOT* a modern AVR! This is generally what is done on all classic AVRs with a differential ADC) so the available gain options depend on which pins are being measured, and there is a different procedure to use it, as detailed in the core documentation (ex, ATTinyCore 2.0.0 and later). If it is undefined, there is definitley no support exposed through the core, and it may not be a feature of the hardware at all.
* `_AVR_PORT_INLVL` is 1 if the INLVL bit is present. If undefined or 0, that feature is not available.

## Compatibility macros
Occasionally Microchip has not kept the names of registers or bitfields consistent between families of parts, even when the function was not changed. In some cases these have even been changed between versions of the ATpack! The places where we've papered over identical register functionality with different names are:
* The `GPIO`/`GPIOR`/`GPR` registers - on the most recent parts, these are officially `GPR.GPRn`. We provide whichever of the following are not present: GPIOn, GPIORn, and GPR_GPRn. `GPIORn` is recommended for maximum compatibility. (GPIOR compatibility note: If writing code that may be ported to classic AVRs, GPIOR3 does not exist at all, and only GPIOR0 is guaranteed to be in the low I/O space - check the register map.
* The `TCA_SINGLE_EVACTA` bitfield - formerly known as `TCA_SINGLE_EVACT` and it's group codes - the old names will work too, permitting code portability from tinyAVR to Dx.
* The `RTC_CLKSEL` group codes have different spellings of some abbreviations on tinyAVR vs Dx/Ex.  The old names will work too, permitting code portability from tinyAVR to Dx.
* The `CLKCTRL_SELHF_CRYSTAL_gc` option on DB-series parts which was renamed to `CLKCTRL_SELHF_XTAL_gc`. Both names work.
* All things `CLKCTRL_FREQSEL` related, which had the E dropped in some ATPACK versions. Both FRQSEL and the old FREQSEL spellings work. I dunno, there must be a shortage of capital E's at Microchip. Maybe it's the new EA and upcoming EB parts hogging all the E's?
* All multi-bit bitfields - the `_bp` and `_bm` defines now have an underscore before the bit number. There are about a thousand impacted defines. At least they had the decency to bump the major version.

## Errata
There are ah, a lot of errata on these parts, sad to say. See the [errata summary](./Errata.md) for details on them.
These constants are defined as either:
* `ERRATA_IRREL` (-128) - This errata does not apply to a part family because the conditions that would make it manifest can never exist there (ex: ERRATA_PORT_PD0 is irrelevant to the DA, because there are no DA parts that don't have a PD0, only smaller DBs have that issue (the nominal PD0 pin is where they put the VDDIO2 pin instead, but the input buffer for PD0 was still present, and with no pin, would always be floating, and needs to have it's input buffer turned off to avoid wasting power).
* `-1` - This applies to all specimens of this part.
* `0` - This does not apply to any parts in this family
* Any other value - This is the REVID that corrected this problem.
* use checkErrata() for best results, ex, if (checkErrata)

`checkErrata(errata name)` will return 1/true if the errata applies to this part, or 0/false if the errata does not apply or is irrelevant.

Note that checkErrata does not get compiled to a known constant - it has to read the SYSCFG.REVID - if there are circulating revisions that do and don't have the bug. But if all specimens either do or do not have the bug, they will.

When future die revs fix some of these problems, checkErrata() will no longer compile to a known constant. Thus, do not use it in #if statements. Do not use it in functions that your application relies upon the compiler constant-folding and optimizing away.


| Errata name               |   0  |   1  |   2  | Quick description
|---------------------------|------|------|------|---------------------------------------
| `ERRATA_CCL_PROTECTION`   | All  | All  | All  | The whole CCL peripheral must be disabled in order to disable any one LUT to reconfigure it.
| `ERRATA_CCL_OUTEN`        | Some | Some | No   | Some 0 and 1-series parts can't use the LINK input if the other LUT doesn't have OUTEN set.
| `ERRATA_CCL_DLATCH`       | All  | Most | No   | Except on Rev. C 3216 and 3217, the D-type Latch sequencer option is broken.
| `ERRATA_TCA_RESTART`      | All  | All  | All  | TCA restart resets direction, like DA/DB datasheet says. Both datasheet and silicon were "wrong".
| `ERRATA_TCB_CCMP`         | All  | All  | All  | In 8-bit PWM, CCMP treated as a 16-bit register not 2 8-bit ones, so both must be written, never just 1. Universal on modern AVRs.
|`ERRATA_TCD_ASYNC_COUNTPSC`|  -   | All  |  -   | Async event's are missed when TCD tries to use them if count prescaler is engaged.
| `ERRATA_TCD_AUTOUPDATE`   |  -   | Some |  -   | One of the three errata fixed in rev B of the 16k parts, out of the 34 errata present.
| `ERRATA_TCD_PORTMUX`      |  -   | All  |  -   | TCD PORTMUX unusable. Only default portmux pins work.
|`ERRATA_TCD_HALTANDRESTART`|  -   | All  |  -   | Halt and Wait for SW restart fault mode does not work in dual slope more or if CMPASET = 0
| `ERRATA_TWI_PINS`         | All  | All  | No   | The OUT register for SCL and SDA must be low, otherwise TWI will try to drive the pins high! (Wire.h makes sure this is done correctly)
| `ERRATA_TWI_FLUSH`        | All  | All  | All  | TWI_FLUSH command leaves the bus in unknown state. That is exactly what it was supposed to fix.
| `ERRATA_USART_ISFIF`      | All  | All  | All  | After an inconsistent sync field (ISFIF), you must turn off RXC and turn it back on
| ERRATA_USART_ONEWIRE_PINS | All  | All  | No   | The DIR register for TX must be set INPUT when ODME bit is set, or it can drive pin high. This is handled internally by the core, and you likely don't need to know.
| `ERRATA_USART_FRAMING`    | All  | Most | No   | Framing errors would corrupt future received bytes. The fix for this, present on Rev. C of the tiny 3216/3217, introduced the USART SFDEN wake bug below, and the ISFIF bug.
| `ERRATA_USART_WAKE`       |  No  | Few  | All  | You must clear SFDEN when waking on start of frame detection before you clear the RXCIF. This is handled internally by the core, and you likely don't need to know. Present on any parts that don't have the framing error bug.
| `ERRATA_USART_ISFIF`      |  No  | Few  | All  | Likely a consequence of the revised USART silicon's fix for "Full range duty cycle not supported when validating LIN sync" issue. That issue does not have a define, as it is related to one of the very few serial features we don't support
| `ERRATA_RTC_PITANDRTC`    | All  | Most | No   | One of the nastiest modern AVR errata. The PIT and RTC are inappropriately entangled, and neither will work correctly if the other is not enabled, and the ways in which they don't-work are perverse. Additionally, writing RTC.CTRLA resets the PIT interval timer on impacted parts. Both of these were fixed in 3216/3217 Rev C, but nowhere else.
| `ERRATA_TCB_SYNCUPD`      | All  | All  | No   | SYNCUPD mode does not restart the TCB from a TCA restart command.
| `ERRATA_TCB_PULSELEN`     | All  | All  | No   | The pulse length must be longer than the selected TCB clock period to be detected when in Time-Out Check and Input Capture Frequency and Pulse-Width Measurement.
| `ERRATA_TCB_ICFPWM`       | Some | Some | No   | Unevenly scattered across the 0/1-series. Input Capture Frequency and Pulse-Width Measurement on impacted parts only works when using CLKSEL = 0.
| `ERRATA_AC_FALSETRIGGER`  | Some | Some | No   | The analog comparators on some tiny 0/1-series parts can give false triggers, see the errata for details.
| `ERRATA_AC_INTFLAG`       | Most?| Most?| No   | ACs didn't raise the INTFLAG unless the interrupt is enabled.
| `ERRATA_FUSE_OSCLOCK`     | Most?| Most?| No   | OSCLOCK from fuses was applied before the factory cal was loaded, preventing factory cal from being loaded.
| `ERRATA_ADC_FLAGSRESH`    | Some | Some | No   | Only an issue in 8-bit mode - the interrupt flags are only cleared automatically by reading the high byte of the result.
| `ERRATA_ADC_FREERUN`      | Some | Some | No   | Certain control registers don't properly get applied if changed while freerunning mode in use, and an extra sample is taken after it is disabled.
| `ERRATA_ADC_ASDVSAMPLEN`  | Some | Some | No   | SAMPLEN and INITDLY/ADSV do not play well together
| `ERRATA_ADC_WCMP`         | Some | Some | No   | WCMP needs another INITDLY after waking the part from standby sleep mode.



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
