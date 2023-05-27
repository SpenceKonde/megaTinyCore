# megaTinyCore Utility Library
This library provides a grabbag of functions mostly related to debugging problems with tuned clocks and making ADC error codes (which should rarely be encountered in practice), as well as a number of utility functions

## General API information
* Library to include is `<megaTinyCore.h>`
* This library does not make use of classes or objects - it just provides functions. I don't think a class would be at all appropriate here as this is a disparate grouping of libraries of widely different functionality.
* Not all of the examples relate to this library at all - they are instead example code for doing other core or architecture specific things. They are listed in the example index below.


## Function reference

### Reset Related
Having two ready methods to reset from software has two benefits:
1. If using a bootloader, it will run after a software reset, but not when the reset is mediated by the WDT instead. This allows application code to be told "Start the bootloader, here comes an upload" and react appropriately.
2. In all cases, it allows you to send a signal about the system state to the sketch post-restart without writing to EEPROM or USERROW, and which won't persist after power is cycled. Note that a "dirty reset" is also handled by a software reset, and so you can't tell the difference between the two.

The implementations are pulled straight from Ref_Reset, on account of a complaint from a user about there not being a wrapper around them and the difficulty of remembering how any SFR names are spelled on modern AVRs without both IO headers and datasheet open

#### `void ResetViaWDT()`
This will trigger a reset using the WDT, which will occur within around 10 ms, and will busy wait until then. This will never run the bootloader.

#### `void ResetViaSoftware()`
This will trigger a reset using the RSTCTRL, which will occur instantly. Depending on the bootloader entry conditions you selected when you burned bootloader this may enter the bootloader

### Control clock output on PB5
These parts can output their system clock on PB5 (if they have a PB5). Parts without a PB5 cannot output the system clock on an I/O pin.

#### `void turnOnClockout()`
On 20 and 24 pin parts, this will override the function of PIN_PB5 and output the system clock on it. This will generate a compile error on parts with fewer pins, as they do not have a PB5.

#### `void turnOffClockout()`
On 20 and 24 pin parts, this will return PB5 to whatever mode it was in before it was told to output the clock. This will generate a compile error on parts with fewer pins, as they do not have a PB5.

### General purpose

#### `inline uint8_t bitMaskToPosition2(uint8_t bitmask)`
#### `inline uint8_t bitMaskToPosition(uint8_t bitmask)`

These two functions are different implementations of the same thing. bitMaskToPosition2 uses hand optimized assembly. bitMaskToPosition checks if the mask is higher than 0x0F (ie, if it is 0b00010000 or more) swaps the nybbles if so. If the test is still true, then it has more than 1 bit set and is hence not a valid bitmask - return 255. We then look up the low nybble in a lookup table and return that value, plus 4 if SWAP was used.

Converting from bitmasks to bitpositions is one of the most annoying things to do on AVR. You have to do it *constantly*. Position to mask is 0-7 clocks if the compiler is smart (but it may not be, it likes setting up a loop instead) - so that's not so bad. But bitmask to bit position is at least as common of a problem, and this does not seem to have any particularly graceful solution. These functions attempt to take some of the drudgery out of this constant chore. Note that, as the concept of a bit position for a bitmask more than 1 bit wide is not well defined, we do not handle those.
* It can be achieved in 6-36 clocks without checking for things that aren't bitmasks. Adding that check adds 4 clocks for valid bitmasks and 5 for invalid ones. It is the most flash-efficient implementation at only 20-21 words under typical conditions, but with larger shifts, it is somewhat slow.
* It would require a test to make sure it wasn't over 128 (hence not valid mask) if using a lookup table, plus a whopping 128 bytes (64 instructions of space!) to use a lookup table to do it in a fast and constant time.
* The tactics used in bitMaskToPosition and bitMaskToHalfPosition are to lean on SWAP to shrink the required table to just 16 bytes, though there is some overhead to it. Testing will be needed to determine which is the smallest.
* None of these are satisfying. A common, simple operation like this should be faster and smaller.

#### `inline uint8_t bitMaskToHalfPosition(uint8_t bitmask, bool highnybble)`

This acts like the above with one key difference: it only acts on one nybble. There are a few (very few) cases where this is more useful.

### Tuning related
There are things here specific how megaTinyCore handless tuning and how to do it.

#### `int16_t getTunedOSCCAL(uint8_t osc, uint8_t target)`
The argument passed for osc must be either 16 or 20 (reflecting the case when the nominal oscillator speed is set to 16 or 20 MHz). The 'target' frequency must be one of the allowed frequencies for your part at that speed **before prescaling**.
Hence, valid values are:
12 (only available with nom. 16 MHz 0/1-series), 14 (not on 20 MHz 2-series), 16, 20, 24, 25, 30 (not on 16 MHz 0/1-series), and 32 (2-series @ 20 only)
It will return the stored tuning value, or one of th following 4 error codes:
```text
-1: An invalid center frequency (first argument) was specified, but could not be caught until runtime (If known at compile time, we give error then)
-2: An invalid target frequency (second argument) was specified for the supplied target at runtime (If known at compile time, we give error then)
-3: This chip has not been tuned
-4: This chip was tuned, and found that it could either not reach this speed (maximum calibration value was was still too slow, or minimum too fast), or the oscillator could reach that speed, but even the rudimentary math we perform in summing up the speed before returning to an in-spec speed gave us nonsensical values, indicating that the chip does not function correctly at that speed and voltage (usually manifests as bits that should be 1 coming out as 0). Note that the tuning sketch will also, upon starting after a reset, if it sees that some of the tuning has been completed, but the last few values have not been written, we conclude that the overclocked chip malfunctioning soas to trigger a dirty reset was the cause, and mark those as unachievable.
```

#### `void printTuningStatus()`
This will print out a description of the current tuning values stored in the USERROW - if any - to Serial; which must already have been 'begun'

Both of the above functions can be called regardless of whether tuning is currently enabled, and are probably most useful when it's not, so you can figure out why it's running at the wrong clock speed with tuning enabled (when tuning is enabled, but something is wrong, the clock speed is wrong and as a result, serial comes out as gibberish).

If you can read serial with the tuning enabled it's running at within a couple percent if the clock speed you specified. You may be dissatisfied with the accuracy of this tuning nonetheles. So erase the last 12 bytes of the user row, and tune it again, or use the tuning sketch and the FORCE tuning bit being certain that the voltage and temperature match the planned opperating conditions, as these are what we're calibrating for.

## Examples listing

### ClockDiagnose
Attempts to diagnose clock problems. Basic usage is covered at the top of the sketch - the idea is upload it and listen at the correct baud rate. If it can figure it out, it will try to tell you why you might not be happy with the clock behavior currently being demonstrated.

### ErsatzReset
Manual implementation of Ersatz Reset for tinyAVR 0/1-series and 14-pin 2-series.

### ErsatzReset_2
Uses the included implementation of Ersatz Reset demonstrating both ways we can reset the chip from software.

### megaTinyTuner
This is the sketch run on the device being tuned when you are tuning the part with a 1kHz reference clock. See the comments in the sketch itself for more info. Tuning rarely helps you get the clock speed more accurate, unless you are running far from room temperature (and tune at that temperature) - but it does let you get crazy speeds out of these parts. (see [the tuning reference](../../extras/Ref_Tuning.md))

### ModernRevSer
Prints out the die rev, serial number, and now the entire sigrow, for any modern AVR.

### readTempVcc
Demonstrates how you would manually read the temperature and supply voltage. Remember that we provide automatic functions to do this; those should generally be used in preference to xeroxing the wheel and implementing them yourself, now that we have these functions. These will be ported to DxCore in the next release

### SelfTuning
This sketch relies on a series of dubious assumptions, but it does do a pretty good job of tuning.
1. Assume that at factory cal, the internal oscillator is perfectly accurate (invalid, but at room temperature, particularly on 2-series, it is extremely good).
2. Assume that the 32 kHz ULP operates at a stable frequency over time-scales of seconds to minutes, but is not accurately calibrated. (that it is not very accurately calibrated is assuredly true, how stable the frequency is, that I don't know.)
3. If 1 and 2 are true, then we can determine the frequency of the 32 kHz ULP if we use it to generate a PIT event, and time that with a TCB. (this is valid, but relies on some big assumptions)
4. Now, we can then change the calibration register to get a faster or slower clock speed.
5. We can now again time the PIT event output with TCB.
6. With this information in hand, we are back at step 3, only the 32kHz ULP frequency is known, and the CPU frequency is not, and solving for the cpu frequency is trivial.

But what can be achieved by making at least 3 dubious but close to true assumptions is miraculous; this sketch simply measures the ULP frequency, and then uses that to measure the clock speed at every value the tuning register can take, writing the closest values to USERROW (see [the tuning reference](../../extras/Ref_Tuning.md)). Between timing runs, we enable the system clock prescaler to slow ourselves down so that if we find that we're starting to see instabilities, we have a greater chance of recognizing that, and informing the user. This, to my astonishment, does a good enough job of tuning that UART works, at a minimum.

### TCA0Demo, TCA0Demo2, TCA0Demo3, TCA0Demo4
A series of demonstrations on advanced use of the type A timer. See [Taking over TCA0](../../extras/TakingOverTCA0.md). Much more explanation is found in that document.

### TuningSource
This is a rather poorly written sketch that tries to detect every AVR under the sun and tell it to generate a 1 kHz clock for tuning from. Note that **the tuning source should have a crystal** - why go to all this effort if you're going to use a reference that's off by a percent or more?

It is supposed to work on classic AVRs. I think it does. I know I've used it on some, I forget which ones (I'd say an arduino pro mini or nano, but those are almost all garbage resonators instead of crystals), so it was probably one of my ATtiny841 boards or something like that. Or an AVR DB.

This whole thing could be implemented far better. PR's welcome.
