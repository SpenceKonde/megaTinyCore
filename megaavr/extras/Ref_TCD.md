# `analogWrite()` and configuring TCD
There is limited support for manipulating TCD0 in specific ways, using these procedures, while not breaking `analogWrite()`. Beware - if you stray from this path, you will sink into the quicksand and you'll be lost forever...

## Background
TCD not a friendly timer. It is one of the most complex and byzantine peripherals on any modern AVR (and will likely remain so, outside of the usb interface that the DUs will have). Unlike TCA, where someone trying to do anything other than change the prescaler is almost guaranteed to cause `analogWrite()` to misbehave, for the TCD pins, we try to support a bit of a wider range because it's sufficiently complicated that few users will take over the type D timer. This is a 12-bit asynchronous timer. The async part is why it has to be so strange - there's a synchronization process which can take a few clock cycles to propagate certain changes: you need to monitor the status flags, because the synchronization will sometimes cause writes to be ignored. It also can be clocked from things faster than the system clock: The unprescaled internal oscillator (rarely relevant here, more relevant on megaTinyCore); a PLL at 2x, 3x or 4x an input frequency (which can be either system clock, or the external clock source (note: initial silicon has a bug, and that option only works for external clocks, not external crystals)); an unmultiplied external clock source; or the peripheral clock. Unlike classic AVRs where the frequency for the "timer1" peripheral from must not exceed half the system clock speed, here only the top frequency is limited, and in practice far surpasses the spec'ed upper limit at room temperature.

In addition to the synchronization, some of the intentional features make it challenging to work with, namely that it seems to be designed for applications where the slightest misconfiguration will cause catastrophic failure (ex, safety critical parts in automotive systems, big or high speed BLDC motors that can spin themselves into pieces, PSC's that could shoothrough and fail dramaticaly, etc) For this reason, much of the configuration is concerned with fault detection, and with preventing pins from being unintentionally enabled. Not only is it enable-locked, it also requires `_PROTECTED_WRITE()` to write to some key registers, which produces the timed write procedure required for reconfiguring such critical things as the clock system - and the TCD output pins. You can turn the timer off without the timed write. But you cannot turn pins on or off without it, nor set their "default state", which is what value the pins have when the fault detection events are used to turn off the output.

## Normal `analogWrite()` operation
`TCD0` provides two PWM channels: `WOA` can output on PA4 or PC0, `WOB` on PA5 or PC1. Unlike DxCore, mTC does not support configuring the TCD to output to two pins at once (though the hardware supports it, it's not very useful). The set of pins desired can be selected from the tools -> PWM pins menu.



## Permitted settings
You can do the following things and continue using `analogWrite()` and expect it to keep behaving normally. Do not expect anything else to work - if you need to do more, you need to take it over entirely.

### ~You can change frequency by multiplying the period by up to a 2, 4, 8, or 16~
If this were DxCore, where much higher frequencies can realistically end up on the TCD, where the chip can run faster and where the flash bottoms out at 16k instead of 2k, this would be a section talking about how to modify the period of the TCD to slow it down or speed it up - setting it to certain magic values will be noticed by DxCore in analogWrite, and automatically leftshift `analogWrite()` values to appropriately. megaTinyCore does not support this.


### You can change frequency, clock source, prescaler
As a general rule, given two options for how to achieve a given frequency (by combination of syncronizer and count prescalers and clock source selection), preference should be given as:
1. The count prescaler must not be 32 if doing certain bizzaro things (async events don't work in 32x count prescale on many parts due to an erratum. And because there's no /32 delay presc option (see below), it's incompatible with that too).
2. Otherwise, you want to minimize the sync prescaler, because any time PWM on apin is turned on or off, the wholeperipheral has tobe briefly shut down, because it's async and is untouchable to the CPU outside of special registers covered by the synchronizer; after disabling it, it's still not writable for another 2 prescaled synchronizer clock cycles (a status byte is pollable for this)


```c

/* Example 7: Changing prescaler - will cause brief glitch as you have to enable-cycle timer. */

/* Calculate desired value by ORing one CLKSEL, one CNTPRES, and one SYNCPRES.
 *  TCD_CLKSEL_OSCHF_gc         TCD_CNTPRES_DIV1_gc     TCD_SYNCPRES_DIV1_gc
 *  TCD_CLKSEL_CLKPER_gc        TCD_CNTPRES_DIV4_gc     TCD_SYNCPRES_DIV2_gc
 *  TCD_CLKSEL_EXTCLK_gc        TCD_CNTPRES_DIV32_gc    TCD_SYNCPRES_DIV4_gc
 *                                                      TCD_SYNCPRES_DIV8_gc
 * Sync Prescale effects everything related to TCD, including how long commands take... (so you want to keep it low, if it's not impractical to do)
 * Prescaled sync clock is prescaled to by Count prescaler to get the signal that actually clocks the counter.
 * OSCHF is internal oscillator before any prescaling.
 * PLL must be enabled or it won't do anything.
 * This will set it to count at 1/128th of the system clock (32 * 4); If TOP is 254, 128*255 = ~32640 tickss of the system clock per cycle. About 490 Hz with 16 MHz system clock:
 */
TCD0.CTRLA = 0;                         // Stop the timer.
while (!(TCD0.STATUS & TCD_ENRDY_bm));  // It needs to synchronize before you can restart it.
                                        // NOW you can write the new value you want.
TCD0.CTRLA = (TCD_CLKSEL_CLKPER_gc | TCD_CNTPRES_DIV32_gc | TCD_SYNCPRES_DIV4_gc | TCD_ENABLE_bm);

/* Altering the clock source like this will not change how analogWrite() works, just the frequency (as long as the clock you selected is present). */

/* You can use the events if you can figure out how to use them, but most don't make sense unless you've taken over the timer.
 * Simple ones like mode 4 (turn off all output while event is HIGH) can be used, and the API functions will generally bumble along unaware
 */
```
### And you can use the programmed (delayed) event
For example, to make a third passable PWM pin by using the event system and a CCL with programmed event to get a third 8-bit PWM channel. I had an example of this, and all the constraints involved in making it work, but it seems to have been lost.

This requires that CNTPRES = DLYPRES.

This limits you to count prescaler of 4 or 1, because those are the only matching options for the delay prescaler:
```text
TCD_DLYPRES_DIV1_gc <--- viable
TCD_DLYPRES_DIV2_gc
TCD_DLYPRES_DIV4_gc <--- viable
TCD_DLYPRES_DIV8_gc
```
An EVEN NUMBERED LUT (LUT 0 on tiny 1-series) is used, set up as non-sequencer-using R/S latch, using the CMPBCLR and PROGEV event channels.
```text

DLYCTRL = 0x0E; // /1 prescale
DLYCTRL = 0x2E; // /4 prescale

Write values to DLYVAL (this can be written without stopping the timer - as can DLYCTRL!) that are your desired duty cycle for the third PWM

The Logic library provides a wrapper around the CCL; the Event library provides a wrapper around the event system.

Connect TCD's PROGEV event and TCD CMPBCLR to asynchronous event channels.
Connect the CCL0 event users to those two event channels.

Configure the CCL block with:
IN0 using the CMPBCLR event channel
IN1 using the PROGEV event channel
IN2 is FEEDBACK.

Truth table is 0xE2

```

### Finally, you can configure fault events and that sort of thing (requires disabling the timer while making changes)

## Otherwise, you must take over TCD and manually configure everything
`takeOverTCD0()` will tell the core that you are assuming full responsibility for TCD0. It will ignore everything that would normally make it reconfigure this timer. This will make `analogWrite()` and `digitalWrite()` act like TCD0 doesn't exist when configuring pins.
`init_TCD0()` is called during init to configure the timer. You can override it with an empty function to prevent this. If you do this, you must call `takeOverTCD0()` before `analogWrite()` or `digitalWrite()` are called. Actually - that's silly, just override `init_TCD()` with a all to `takeOverTCD0()`.
You can do all kinds of crazy things with this timer! It can dither it's output for 16x more accurate frequencies, there are 11 different things it can do on events, you can set the default state of the compare outputs... With some persistence you can even capture the current counter value!

### What next? Study the TCD0 chapter of the datasheet
The rest is beyond the scope of the core's documentation.

### Is the TCD done growing new errata?
Uncertain. It took *years* for the errata we have to show up. Some weren't listed for over 4 years (seemingly only after the Dx-series was out, and people found the bugs there and Microchip realized they were on the 1-series as well). The TCD is a complicated timer that **really** would benefit from a... how to say it... "designer's statement of intent" regarding the wierder options. There are cases where it's clear that they intended some niche use case, but I can't say I know exactly what.
