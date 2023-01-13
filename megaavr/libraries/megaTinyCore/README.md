# megaTinyCore Utility Functions

This library, initially only to supply a keywords.txt, now supplies a few special functions

## Function reference

### Tuning related
There are things here specific how megaTinyCore handless tuning and how to do it.

#### int16_t getTunedOSCCAL(uint8_t osc, uint8_t target)
The argument passed for osc must be either 16 or 20 (reflecting the case when the nominal oscillator speed is set to 16 or 20 MHz). The 'target' frequency must be one of the allowed frequencies for your part at that speed **before prescaling**.
Hence, valid values are:
12 (only available with 16 MHz 0/1-series), 14 (not on 20 MHz 2-series), 16, 20, 24, 25, 30 (not on 16 MHz 0/1-series), and 32 (2-series @ 20 only)
It will return the stored tuning value, or one of th following 4 error codes:
```text
-1: An invalid center frequency (first argument) was specified, but could not be caught at runtime (If known at compile time, we give error then)
-2: An invalid target frequency (second argument) was specified for the supplied target at runtime (If known at compile time, we give error then)
-3: This chip has not been tuned
-4: This chip was tuned, and found that it could either not reach this speed (maximum calibration value was was still too slow, or minimum too fast), or the oscillator could reach that speed, but even the rudimentary tests we perform to ensure that the chip is functioning gave crazy results indicating that the chip cannot operate at this speed from the internal oscillator
```

#### void printTuningStatus()
This will print out a description of the current tuning values stored in the USERROW - if any - to Serial; which must alreadty have been 'begun'

Both of the above functions can be called regardless of whether tuning is currently enabled, and are probably most useful when it's not, so you can figure out why it's running at the wrong clock speed with tuning enabled - but when it's enabled, serial comes out as gibberish. If you can read serial with the tuning enabled, then clearly the tuning itself works. It may not be tuned as accurately as you like,

### Reset Related
Having two ready methods to reset from software has two benefits:
1. If using a bootloader, it will run after a software reset, but not when the reset is mediated by the WDT instead. This allows application code to be told "Start the bootloader, here comes an upload" and react appropriately.
2. In all cases, it allows you to send a signal about the system state to the sketch post-restart without writing to EEPROM or USERROW, and which won't persist after power is cycled. Note that a "dirty reset" is also handled by a software reset, and so you can't tell the difference between the two.

The implementations are pulles straight from Ref_Reset, on account of a complaint from a user about there not being a wrapper around them and the difficulty of remembering how any SFR names are spelled on modern AVRs without both IO headers and datasheet open

#### void ResetViaWDT()
This will trigger a reset using the WDT, which will occur within around 10 ms, and will busy wait until then. This will never run the bootloader.

#### void ResetViaSoftware()
This will trigger a reset using the RSTCTRL, which will occur instantly. Depending on the bootloader entry conditions you selected when you burned bootloader this may enter the bootloader
