# PWM and Timers on modern AVRs and their usage in this core
This document is divided into two sections. The first one simply describes the available timers, and what they are capable of (by "simply describes" I don't claim to have made a simple description, only that the purpose is simple. and that is to describe the timers). The second section describes how they are used by this core in particular. The first section is shared by DxCore and megaTinyCore. The second contains many sections specific to one core or another.

## Background: The Timers on the tinyAVR parts
This applies to the tinyAVR 0/1/2-series, megaAVR 0-series, and AVR DA, DB and DD-series, and likely other future modern AVRs. There are very few differences between the implementations on the different families.

### TCA0 - Type A 16-bit Timer with 3/6 PWM channels
This timer is the crown jewel of the modern AVR devices, as far as timers go. It can be operated in two very different modes. The default mode on startup is "Normal" or `SINGLE` mode - it acts as a single 16-bit timer with 3 output compare channels. It can count in either direction, and can also be used as an event counter (ie, effectively "clocked" off the event), is capable of counting up or down, generating PWM in single and dual slope modes, and has 7-output prescaler. For most use cases, a TCA in SINGLE mode is on the same level as the classic avr 16-bit Timers, only with more outputs (especially for 8-bit PWM) - the newly added features aren't ones that are particularly relevant for Arduino users. In this mode, TCA0 can generate events or interrupts on compare match for each channel (independently), as well as on an overflow.

The Type A timer can be also be set to split mode to get six 8-bit PWM channels (this is how it is configured by default by megaTinyCore and DxCore - since `analogWrite()` is only 8-bit, we might as well double the number of channels we get right? In split mode the high and low bytes of the timer count `TCA0.SINGLE.CNT` register become `TCA.SPLIT.LCNT` and `TCA.SPLIT.LCNT`; likewise the period and compare registers *in SINGLE mode, these are 16-bit registers; accessing them uses the temporary register. In SPLIT mode, they are 8-bit registers!*. Unlike the TCB in PWM mode, this works correctly. The frequency of the two "halves" of the timer is always the same (prescaler is shared between the two). However, the HPER and LPER registers can be used to adjust the period independently. This means that you could change the number of counts per cycle, thus allowing for the two halves to output different frequencies of PWM - albeit at a cost in resolution. There is an example of this under the link below).

There are a few examples of using TCA0 to generate PWM at specific frequencies and duty cycles in the document on [Taking over TCA0](./TakingOverTCA0.md)

The pins that the type A timer is directed to can be mapped to a variety of different pins.
* On ATtiny parts, each Waveform Output (WO) pin can be controlled separately.
* On ATtiny parts with 14+ pins, WO0-2 can be mapped to either PB0-2 or PB3-5, and WO3-5 (the split mode only ones) can be mapped to PA3-5 or PC3-5.
* On ATtiny parts with 8 pins, they are mapped to (in order) PA3-or-PA7, PA1, PA2, and PA3. Only the first 4 are available, and WO0 must be remapped in order to use WO3.
* On everything else, each TCA is can be pointed at one group of pins from a list of options. TCA0 can (on all parts thus far released) be directed to pins 0-5 of any port, and TCA1 can be directed to PB0-5, PC4-6, PE 4-6, or PG0-5. The last two options do notwork on the AVR DA-series, but do work on the DB-series, and a later silicon die rev is likely to correct that issue on the DA.

On all parts, the TCA can be used to count events, instead of system clocks (in this mode, the prescaler is ignored). On 2-series parts, and AVR Dx-series and later, there is a second event input dedicated to things like restarting as well as allowing the timer count direction to be reversed when an event is HIGH. The event system features are not available in split mode.

#### Interrupt note
When firing interrupts from a TCA, you must ALWAYS manually clear the intflags. It is not done for you.


### TCBn - Type B 16-bit Timer
The type B timer is what I would describe as a "utility timer" - It is a very good utility timer. In this role, it can take one of no less than 7 modes.

### Periodic interrupt
This does exactly what you would expect - counts up top CCMP, and resets to 0 and generates an interrupt. This is how it's used for millis timekeeping. Even with the limited prescaling selections (1, 2, or the TCA prescaler), being able to count up to 64k makes this a lot less limiting than it would be on an 8-bit timer. It also has no less than FOUR input capture modes:

#### Input Capture on Event
Timer counts continually from 0 to 65535. This is very much like the old ICP mode on classic AVRs. Upon the specified edge (selectable) of the the event connected to the timer, it will copy the current count to CCMP and fire an interrupt. It is the responsibility of the user code to know when the last input capture occurred and subtract those values.

#### Input Capture Frequency Measurement
As above, but at the specified edge, the timer count is automatically reset (after copying count to CCMP of course), so it starts timing the next event. As the name implies, this is of great use for measuring the frequency of an input.

#### Input Capture Pulse Width Measurement
One edge restarts the timer. The other edge copies the value and fires the interrupt, thus making it perfect for timing the length of pulses.

#### Input Capture Frequecy And Pulse Width Measurement
This one is the weird one (and on some parts, it's use is subject to some silicon errata). On one edge, it starts the timer. On the other edge, it copies the count to CCMP *and continues to count* until the next edge. Only then will the interrupt fire (if enabled). Whether via polling or interrupt, when you see that this has happened, you need copy the count value someplace safely, then the CCMP value. As soon as CCMP has been read, when it next sees the "start count" edge, it will reset the timer and start counting. Thus, while great for measuring PWM of varying frequency and duty cycle where the duty cycle does not change drastically from cycle to cycle, it is not useful if you need to record every cycle (for that, you're forced to use Input Capture on Event mode, and have your interrupt very quickly switch the edge to which it is sensitive).

#### Single-shot
In addition to the input capture modes, they can be used for precise generation of a pulse with timing accuracty down to a single clock cycle. In this mode, the output pin will go high or low (selectable) in response to the specified edge of an event. The counter will count up to it's CCMP value, then reset and set the output pin low (these are the same output pins as are used for PWM). It is NOT NECESSARY to use the pin itself! You can use this instead to generate a event or interrupt after a delay has passed! Likewise, it could be triggered with an event channel connected to nothing, and manually kicked off with a software event. Without relying on TCA's prescaler, this can generate puloes of around 130ms in length!

#### Timeout Check
In this mode, one type of edge resets the counter to 0 and starts it counting, while the other edge stops the counting. In this mode the interrupt fires when the count passes CCMP - that's the "time out" that it's checking for. While this may at first sound like an odd feature, it is in fact extremely useful in combination with the custom logic and event system. The magic is that because it is both reacting to and can output an event you can use it to do things in the background. Among other things, it is exceptionally useful for working with bespoke single wire protocols.

#### 8-bit PWM mode
They can be pressed into service as a rather poor PWM timer. TCB in PWM mode is 8-bit only. When used for PWM, they can only generate 8-bit PWM, despite being a 16-bit timer, because the 16-bit `TCBn.CCMP` register is used for both the period and the compare value in the low and high bytes respectively. They always operate in single-slope mode, counting upwards, the selection of clock options is limited in the extreme: system clock, half the system clock, or the frequency depends on that of the TCA (or one of the TCA's on parts with more than one).  In other words, **the type B timers are not very good at generating PWM**. Note also that `TCBn.CCMP` is effected by silicon errata: It still acts like a 16-bit register, using the temp register for access, so you must read the low byte first, then high byte, and always write the high byte after the low one, lest it not be written or a bad value written over the low byte!

#### Extra features on 2-series and Dx
The 2-series and Dx parts add three upgrades, two useful, and the other less-so. The less useful one is a separate OVF event/interrupt source. I find this to be of dubious untility - likely the best use of it would be as a 17th bit while doing input capture.

A much more interesting option, is the clock-on-event option: The TCBs now have a second event user, which can be selected as the clock source! Combined with the CCL, this can, for example, be used to get a prescaled system clock into the TCD different from that of a TCA!

Finally, on these parts, you can combine two timers to make a single, monster, 32-bit input capture timer using the CASCADE feature. In this mode, the high timer is clocked from the OVF event of the low timer, and the CASCADE bit enabled. When the event fires, both timers will get their CCMP values saved and/or be stopped, permitting incredible precision (or just really long things to be measured). 2<sup>32</sup> / 20 MHz = 214 seconds. Prescaling the the low timer by 2 would double that. Ever wanted to time approximately 6 minute long event to a resolution of tenths of a microsecond? Er, sorry, I meant tenths of some period of time that is within a percent or so of a microsecond, unless using an external clock or something. This could be more interesting if you were, instead of time, counting events on the first timer, and 16 bits just wasn't enough.

#### Intflag summary
Sometimes the intflags automatically clear, but usually they don't. Be sureto clear them manually in the cases where it isn't done automatically!

| Mode              | CAPT Reset by | OVF reset by | OVF set under normal use |
|-------------------|---------------|--------------|--------------------------|
| Interrupts        | User code     | User code    | Only when CCMP set below CNT  |
| Tumeout Check     | User code     | User code    | Only when timeout goes on a long time  |
| Input capt. Event| Read CCMP     | User code    | Every time the counter rolls over. Rarely needed |
| Input capt. (all)| Read CCMP     | User code    | When counter rolls over. Effectively 17th bit |
| Singleshot        | User code     | User code    | No.  |
| PWM               | User code     | User code    | At end of every PWM cycle |


### TCD0 - Type D 12-bit Async Timer
The Type D timer, is a very strange timer indeed. It can run from a totally separate clock supplied on EXTCLK, or from the unprescaled internal oscillator - or, on the Dx-series, from the on-chip PLL at 2, 3, or even 4 times the speed of the external clock or internal oscillator! (the 4x multiplication option is unofficial though - it was in the very earliest headers, but was neverin the datasheet andwas removed from headers shortly thereafter. It works under favorable conditions though). It was apparently designed with a particular eye towards motor control and SMPS control applications. This makes it very nice for those sorts of use cases, but in a variety of ways ,these get in the way of using it for the sort of things that people who would be using the Arduino IDE are likely to want to do with it. First, none of the control registers can be changed while it is running; it must be briefly stopped, the register changed, and the timer restarted. In addition, the transition between stopping and starting the timer is not instant due to the synchronization process. This is fast (it looks to me to be about 2 x the synchronizer prescaler 1-8x Synchronizer-prescaler, in clock cycless. The same thing applies to reading the value of the counter - you have to request a capture by writing the SCAPTUREx bit of TCD0.CTRLE, and wait a sync-delay for it. can *also* be clocked from the unprescaled 20 MHz (or 16 MHz) internal oscillator, even if the main CPU is running more slowly. - though it also has it's own prescaler - actually, two of them - a "synchronizer" clock that can then be further prescaled for the timer itself. It supports normal PWM (what they call one-ramp mode) and dual slope mode without that much weirdness, beyond the fact that `CMPBSET` is TOP, rather than it being set by a dedicated register. But the other modes are quite clearly made for driving motors and switching power supplies. Similar to Timer1 on the ATtiny x5 and x61 series parts in the classic AVR product line,  this timer can also create programmable dead-time between cycles.

It also has a 'dither' option to allow PWM at a frequency in between frequencies possible by normal division of the clock - a 4-bit value is supplied to the TCD0.DITHER register, and this is added to a 4-bit accumulator at the end of each cycle; when this rolls over, another clock cycle is inserted in the next TCD0 cycle.

The asynchronous nature of this timer, however, comes at a great cost: It is much harder to use than the other timers. Most changes to settings require it to be disabled as noted above - and you need to wait for that operation to complete (check for the `ENABLERDY` bit in `TCD0.STATUS`). Similarly, to tell it to apply changes made to the `CMPxSET` and `CMPxCLR` registers, you must use the `TCD.CTRLE` (the "command" register) to instruct it to synchronize the registers. Similarly, to capture the current count, you need to issue a SCAPTUREx command (x is A or B - there are two capture channels) - and then wait for the corresponding bit to be set in the `TCD0.STATUS` register. In the case of turning PWM channels on and off, not only must the timer be stopped, but a timed write sequence is needed ie, `_PROTECTED_WRITE(TCD0.FAULTCTRL,value)` to write to the register that controls whether PWM is enabled; this is apparenmtly because, in the intended use-cases of motor and switching power supply control, changing this accidentally (due to a wild pointer or other software bug) could have catastrophic consequences. Writes to any register when it is not "legal" to write to it will be ignored. Thus, making use of the type D timer for even simple tasks requires careful study of the datasheet - which is itself quite terse in places where it really shouldn't be - and can be frustrating and counterintuitive.


### RTC - 16-bit Real Time Clock and Programmable Interrupt Timer
Information on the RTC and PIT will be added in a future update.

One very important thing to be aware of on the tinyAVR 0/1-series only (and not the 3216/3217, where this has been fixed) is that there is some NASTY errata here. First, you must never have the RTC running without the PIT, nor the pit without the RTC). The Microchip errata for these parts does not do justice to the extent of how strange the resulting behavior is. Second, the PIT timer will be glitch every time RTC.CTRLA is written to.

### Timer Prescaler Availability

Prescaler    | TCAn  | TCBn  | TCD0  | TCD0 sync | TD0 counterb|
------------ | ------|-------|-------|-----------|-------------|
CLK          |  YES  |  YES  |  YES  |  YES      |  YES        |
CLK2         |  YES  |  YES  |  YES* |  YES      |  NO         |
CLK/4        |  YES  |  TCA  |  YES  |  YES      |  YES        |
CLK/8        |  YES  |  TCA  |  YES  |  YES      |  NO         |
CLK/16       |  YES  |  TCA  |  YES* |  NO       |  NO         |
CLK/32       |  NO   |  NO   |  YES  |  NO       |  YES        |
CLK/64       |  YES  |  TCA  |  YES* |  NO       |  NO         |
CLK/128      |  NO   |  NO   |  YES* |  NO       |  NO         |
CLK/256      |  YES  |  TCA  |  YES* |  NO       |  NO         |
CLK/1024     |  YES  |  TCA  |  NO   |  NO       |  NO         |

* Requires using the synchronizer prescaler as well. My understanding is that this results in sync cycles taking longer.
`TCA` indicates that for this prescaler, a TCA must also use it, and then that can be prescaled, and the TCB set to use that TCA's clock.

### Resolution, Frequency and Period
When working with timers, I constantly found myself calculating periods, resolution, frequency and so on for timers at the common prescaler settings. While that is great for adhoc calculations, I felt it was worth some time to make a nice looking chart that showed those figures at a glance. The numbers shown are the resolution (when using it for timing), the frequency (at maximum range), and the period (at maximum range - ie, the most time you can measure without accounting for overflows).
### [In Google Sheets](https://docs.google.com/spreadsheets/d/10Id8DYLRtlp01KA7vvslC3cHaR4S2a1TrH7u6pHXMNY/edit?usp=sharing)

## Section Two: How the core uses these timers
**This section is a work in progress**

### PWM via analogWrite()
#### TCAn
The core reconfigures they type A timers in split mode, so each can generate up to 6 PWM channels simultaneously. The `LPER` and `HPER` registers are set to 254, giving a period of 255 cycles (it starts from 0), thus allowing 255 levels of dimming (though 0, which would be a 0% duty cycle, is not used via analogWrite, since `analogWrite(pin,0)` calls `digitalWrite(pin,LOW)` to turn off PWM on that pin). This is used instead of a PER=255 because `analogWrite(255)` in the world of Arduino is 100% on, and sets that via `digitalWrite()`, so if it counted to 255, the arduino API would provide no way to set the 255/256th duty cycle). Additionally, modifications would be needed to make `millis()`/`micros()` timekeeping work without drift at that period anyway.
The core supports generating PWM using up to 6 channels per timer, and will work with alternate PORTMUX settings as long as the the selected option isn;t one of the three-channel ones for TCA1 - those are not supported. TCA1 can be on PB0-5 or PG0-5 (and not even the latter on DA due to errata). TCA0 can go on pin 0-5 in any port (though they must all be on the same port. We default to configuring it for PD on 28/32 pin parts and PC on 48/64 pin ones).

`analogWrite()` checks the `PORTMUX.TCAROUTEA` register. In a future version the same will be done for `PORTMUX.TCDROUTEA`, but no silicon is available where that works.

#### TCD0
TCD0, by default, is configured for generating PWM (unlike TCA's, that's about all it can do usefully). TCD0 is clocked from the CLK_PER when the system is using the internal clock without prescaling. On the prescaled clocks (5 and 10 MHz) it is run it off the unprescaled oscillator (just like on the 0/1-series parts that it inherits the frequencies from), keeping the PWM frequency near the center of the target range. When an external clock is used, we run it from the internal oscillator at 8 MHz, which is right on target.

It is always used in single-ramp mode, with `CMPBCLR` (hence TOP) set to 509

The output channels are only available through megaTinyCore on 20 and 24 pin parts

#### TCBn
The type B timers are never used by megaTinyCore for PWM.

#### PWM Frequencies
The frequency of PWM output using the settings supplied by the core is shown in the table below. The "target" is 1 kHz, never less than 490 Hz or morethan 1.5 kHz. As can be seen below, there are several frequencies where this has proven an unachievable goal. The upper end of that range is the point at which - if PWMing the gate of a MOSFET - you have to start giving thought to the gate charge and switching losses, and may not be able to directly drive the gate of a modern power MOSFET and expect to get acceptable results (ie, MOSFET turns on and off completely in each cycle, there is minimal distortion of the duty cycle, and it spends most of it's "on" time with the low resistance quoted in the datasheet, instead of something much higher that would cause it to overheat and fail). Not to say that it **definitely** will work with a given MOSFET under those conditions (see [the PWM section of my MOSFET guide](https://github.com/SpenceKonde/ProductInfo/blob/master/MOSFETs/Guide.md#pwm) ), but the intent was to try to keep the frequency low enough that that use case was viable (nobody wants to be forced into using a gate driver), without compromising the ability of the timers to be useful for timekeeping.



|   CLK_PER | Prescale A |   fPWM  | Prescale D  | TOP D |  fPWM (D) |
|-----------|------------|---------|-------------|-------|-----------|
|    20 MHz |         64 | 1225 Hz | OSCHF@20 32 |   509 |   1225 Hz |
|    16 MHz |         64 |  980 Hz | OSCHF@16 32 |   509 |    980 Hz |
|    10 MHz |         64 |  613 Hz | OSCHF@20 32 |   509 |   1225 Hz |
|     8 MHz |         64 |  490 Hz | OSCHF@16 32 |   509 |    980 Hz |
|     5 MHz |         16 | 1225 Hz | OSCHF@20 32 |   509 |   1225 Hz |
|     4 MHz |         16 |  980 Hz | OSCHF@16 32 |   509 |    980 Hz |
|     1 MHz |          8 |  490 Hz | OSCHF@16 32 |   509 |    980 Hz |

This section is incomplete and will be expanded at a later date.

### Millis/Micros Timekeeping
megaTinyCore supports use of TCA0 (used by default on non-1-series parts), any TCB, or the TCD (used by default on 1-series, because it is so hard to take over and reconfigure, nobody is going to do that.)
A table is presented for each type of timer comparing the percentage of CPU time spent in the ISR, the resolution of the timekeeping functions, and the execution time of micros. Typically `micros()`  can have one of three execution times, the shortest one being overwhelmingly more common, and the differences between them are small.


#### TCAn for millis timekeeping
When TCA0 is used as the millis timekeeping source, it is set to run at the system clock prescaled by 8 when system clock is 1MHz, 16 when system clock is 4 MHz or 5 MHz, and 64 for faster clock speeds, with a period of 255 (as with PWM). This provides a `millis()`  resolution of 1-2ms, and is effecively not higher than 1ms between 16 and 30 MHz, while `micros()` resolution remains at 4 us or less. At 32 MHz or higher, to continue generating PWM output within the target range, we are forced to switch to a larger prescaler (by a factor of 4), so the resolution figures fall by a similar amoubnt, and the ISR is called that much less often.

##### TCA timekeeping resolution
|   CLK_PER | millis() | micros() | % in ISR | micros() time |
|-----------|----------|----------|----------|---------------|
|    32 MHz |  2.04 ms |   8.0 us |   0.19 % |          4 us |
|    30 MHz |  0.54 ms |   2.1 us |   0.72 % |   aprx   4 us |
|    28 MHz |  0.58 ms |   2.3 us |   0.72 % |          4 us |
|    25 MHz |  0.65 ms |   2.6 us |   0.72 % |   aprx   4 us |
|    24 MHz |  0.68 ms |   2.7 us |   0.72 % |          5 us |
|    20 MHz |  0.82 ms |   3.2 us |   0.72 % |          7 us |
|    16 MHz |  1.02 ms |   4.0 us |   0.72 % |          9 us |
|    12 MHz |  1.36 ms |   5.3 us |   0.72 % |         10 us |
|    10 MHz |  1.63 ms |   6.4 us |   0.72 % |         14 us |
|     8 MHz |  2.04 ms |   8.0 us |   0.72 % |         17 us |
|     5 MHz |  0.82 ms |   3.2 us |   2.99 % |         27 us |
|     4 MHz |  1.02 ms |   4.0 us |   2.99 % |         33 us |
|     1 MHz |  2.04 ms |   8.0 us |   5.98 % |        112 us |

In contrast to the type B timer where prescaler is held constant while the period changes, here period (in ticks) is constant but the prescaler is not. Hence each prescaler option is associated with a fixed % of time spent in the ISR (and yes, for reasons I don't understand, the generated ISR code is slightly faster for /64 prescaling compared to /256, /16, and /8 (which are equal to each other).

The micros execution time does not depend strongly on F_CPU, running from 112-145 clock cycles.

Except when the resolution is way down near the minimum, the device spends more time in the ISR on these parts. Notice that at these points that - barely - favor TCAn, the interrupt they're being compared to is firing twice as frequently!


#### TCBn for millis timekeeping
When TCB2 (or other type B timer) is used for `millis()` timekeeping, it is set to run at the system clock prescaled by 2 (1 at 1 MHz system clock) and tick over every millisecond. This makes the millis ISR very fast, and provides 1ms resolution at all speeds for millis. The `micros()` function also has 1 us or almost-1 us resolution at all clock speeds (though there are small deterministic distortions due to the performance shortcuts used for the microsecond calculations. The only reason these excellent timers are not used by default is that too many other useful things need a TCB.


|   CLK_PER | millis() | micros() | % in ISR | micros() time |
|-----------|----------|----------|----------|---------------|
|    32 MHz |     1 ms |     1 us |   0.20 % |          3 us |
|    30 MHz |     1 ms |  1.07 us |   0.22 % |               |
|    28 MHz |     1 ms |  1.14 us |   0.23 % |               |
|    25 MHz |     1 ms |  1.28 us |   0.26 % |          6 us |
|    24 MHz |     1 ms |  1.33 us |   0.27 % | *        5 us |
|    20 MHz |     1 ms |     1 us |   0.33 % | *        6 us |
|    16 MHz |     1 ms |     1 us |   0.40 % |          6 us |
|    12 MHz |     1 ms |  1.33 us |   0.54 % | *       10 us |
|    10 MHz |     1 ms |     1 us |   0.65 % | *       10 us |
|     8 MHz |     1 ms |     1 us |   0.80 % |         11 us |
|     5 MHz |     1 ms |     1 us |   1.30 % | *       23 us |
|     4 MHz |     1 ms |     1 us |   1.60 % |         21 us |
|     1 MHz |     1 ms |     1 us |   6.50 % |         78 us |

Resolution is always exactly 1ms for millis, and whereas TCAn `micros()` is limited by the resolution of the timer, here it's instead limited only by the fact that the calculations drop the least significant bits first; this results in a value that may be as low as 750, yet is being mapped to 0-999, for 1.33 us resolution in the worst cases. The timer count and the running tally of overflows could get us microseconds limited only by F_CPU/2
The percentage of time spent in the ISR varies in inverse proportion to the clock speed - the ISR simply increments a counter and clears its flags. 65 clocks from interrupt bit set to interrupted code resuming.

The time that `micros()` takes to return a value varies significatly with F_CPU. Specifically, powers of 2 are highly favorable, and almost all the calculations drop out of the 1 MHz case. micros takes between 78 and 160 clocks to run. Each factor of 2 increase in clock speed results in 5 extra clocks being added to micros in most cases (bitshifts, while faster than division, are still slow when you need multiples of them on larger types, especially in compiler-generated code)
`*` indicates that the shift & sum ersatz-division is done in hand-optimized assembly because I just couldn't stand how stupid the and stubborn the compiler was, and by the time I was done analyzing it, implementing it was trivial. This improves accuracy, particularly in the case significantly as well. In these optimized cases, as well as the powers of 2, the results are as good as possible in light of the specified resolution - where resolution is coarser than 1us, (1/resolution) of possible values for the three lowest digits (in normal base 10 number systems) is skipped and never shows up. These skipped values are distributed evenly between 1 and 999. For example, for 12/24/48, that means 250 values never show up. Of the remaining 750, 500 will show for only 1 "actual" microsecond value, and 250 for 2 consecutive "actual" microsecond values. See the table at the end of this document for the list of skipped least-significant-digit combinations. None of the optimized options will ever go back in time, even by a single microsecond (as long as the chance of backward time travel is limited to less than the time it takes micros to execute, it can't break `delay()`, cause timeouts to instantly expire or cause other catastrophic consequences) nor are more than 2 values in a row ever skipped where resolution > 1us.

### TCD0 for millis timekeeping
This will be documented in a future release.

### Tone
The `tone()` function included with DxCore uses one Type B timer. It defaults to using TCB0; do not use that for millis timekeeping if using `tone()`. Tone is not compatible with any sketch that needs to take over TCB0. If possible, use a different timer for your other needs. When used with Tone, it will use CLK_PER or CLK_PER/2 as it's clock source - the TCA clock will never be used, so it does not care if you change the TCA0 prescaler (unlike the official megaAVR core).

Tone works the same was as the normal `tone()` function on official Arduino boards. Unlike the official megaAVR board package's tone function, it can be used to generate arbitrarily low frequency tones (as low as 1 Hz). If the period between required toggling's of the pin is greater than the maximum timer period possible, it will calculate how many cycles it has to wait through between switching the pins in order to achieve the desired frequency.

It can only generate a tone on one pin at a time.

All tone generation is done via interrupts. The hardware output compare functionality is not used for generating tones because in PWM mode, the type B timers kindof suck.

### Servo Library
The Servo library included with this core uses one Type B timer. It defaults to using TCB1 if available, unless that timer is selected for Millis timekeeping. Otherwise, it will use TCB0. The Servo library is not compatible with any sketch that needs to take over these timers - if possible, use a different timer for your other needs. Servo and `tone()` can only be used together on when neither of those is used for millis timekeeping.

Regardless of which type B timer it uses, Servo configures that timer in Periodic Interrupt mode (`CNTMODE`=0) mode with CLK_PER/2 or CLK_PER as the clock source, so there is no dependence on the TCA prescaler. The timer's interrupt vector is used, and it's period is constantly adjusted as needed to generate the requested pulse lengths. In 1.1.9 and later, CLK_PER is used if the system clock is below 10MHz to generate smoother output and improve performance at low clock speeds.

The above also applies to the Servo_megaTinyCore library; it is an exact copy except for the name. If you have installed a version of Servo via Library Manager or by manually placing it in your sketchbook/libraries folder, the IDE will use that in preference to the one supplied with this core. Unfortunately, that version is not compatible with the Dx-series parts. Include Servo_megaTinyCore.h instead in this case. No changes to your code are needed other than the name of the library you include.

### Additional functions for advanced timer control
  We provide a few functions that allow the user to tell the core functions that they wish to take full control over a timer, and that the core should not try to use it for PWM. These are not part of a library because of the required integration with the core to control `analogWrite()` and digitalWrite behavior.

#### takeOverTCA0()
  After this is called, `analogWrite()` will no longer control PWM on any pins attached to timer TCA0 (though it will attempt to use other timers that the pin may be controllable with to, if any), nor will `digitalWrite()` turn it off. TCA0 will be disabled and returned to it's power on reset state. All TCBs that are used for PWM on parts with only TCA0 use that as their prescaled clock source buy default. These will not function until TCA1 is re-enabled or they are set to use a different clock source. Available only on parts with TCA1 where a different timer is used for millis timekeeping.
#### takeOverTCD0()
  After this is called, `analogWrite()` will no longer control PWM on any pins attached to timer TCD0 (though it will attempt to use other timers, if any), nor will `digitalWrite()` turn it off. There is no way to reset type D timers like Type A ones. Instead, if you are doing this at the start of your sketch, override init_TCD0. If TCD is ever supported as millis timing source, this will not be available.
#### resumeTCA0()
  This can be called after takeOverTimerTCA0(). It resets TCA0 and sets it up the way the core normally does and re-enables TCA0 PWM via analogWrite.


## Appendix I: Names of timers
Whenever a function supplied by the core returns a representation of a timer, these constants will be used

| Timer Name   | Value | Peripheral |          Used for |
|--------------|-------|------------|-------------------|
| NOT_ON_TIMER |  0x00 |            |                   |
| TIMERA0      |  0x10 |      TCA0  | millis and/or PWM |
| TIMERB0      |  0x20 |      TCB0  | millis            |
| TIMERB1      |  0x21 |      TCB1  | millis            |
| TIMERD0      |  0x40 |      TCD0  | millis     or PWM |
| TIMERRTC     |  0x90 |       RTC  | millis            |
| DACOUT       |  0x80 |      DAC0  |       analogWrite |

## Appendix II: TCB Micros Artifacts
3, 6, 12, 24, and 48 MHz, with the new optimized micros code, running from a TCB, is known to skip these (and only these) 250 values when determining the least significant thousand micros. That is, if you repeatedly calculate `micros % 1000`, none of these will show up until it has been running long enough for micros to overflow.

```text
2, 7, 10, 13, 18, 23, 28, 31,  34,  39,  42,  45,  50,  53,  56,  61,  66,  71,  74,  77,  82,  87,  92,  95,  98, 103, 108, 113, 116, 119, 124, 127, 130,
135, 138, 141, 146, 151, 156, 159, 162, 167, 170, 173, 178, 181, 184, 189, 194, 199, 202, 205, 210, 213, 216, 221, 224, 227, 232, 237, 242, 245, 248, 253,
258, 263, 266, 269, 274, 279, 284, 287, 290, 295, 298, 301, 306, 309, 312, 317, 322, 327, 330, 333, 338, 341, 344, 349, 352, 355, 360, 365, 370, 373, 376,
381, 384, 387, 392, 395, 398, 403, 408, 413, 416, 419, 424, 429, 434, 437, 440, 445, 450, 455, 458, 461, 466, 469, 472, 477, 480, 483, 488, 493, 498, 501,
504, 509, 512, 515, 520, 523, 526, 531, 536, 541, 544, 547, 552, 555, 558, 563, 566, 569, 574, 579, 584, 587, 590, 595, 600, 605, 608, 611, 616, 621, 626,
629, 632, 637, 640, 643, 648, 651, 654, 659, 664, 669, 672, 675, 680, 685, 690, 693, 696, 701, 706, 711, 714, 717, 722, 725, 728, 733, 736, 739, 744, 749,
754, 757, 760, 765, 770, 775, 778, 781, 786, 791, 796, 799, 802, 807, 810, 813, 818, 821, 824, 829, 834, 839, 842, 845, 850, 853, 856, 861, 864, 867, 872,
877, 882, 885, 888, 893, 896, 899, 904, 907, 910, 915, 920, 925, 928, 931, 936, 941, 946, 949, 952, 957, 962, 967, 970, 973, 978, 981, 984, 989, 992, 995
```

The repeats are:
```text
0, 4, 8, 12, 16, 20, 24,  27,  32,  36,  40,  44,  48,  52,  57,  60,  64,  68,  72,  76,  80,  84,  88,  91,  96, 100, 104, 107, 111, 115, 120, 123, 128,
132, 136, 140, 144, 148, 152, 155, 160, 164, 168, 172, 176, 180, 185, 188, 192, 196, 200, 204, 208, 212, 217, 220, 225, 229, 233, 236, 240, 244, 249, 252,
256, 260, 264, 268, 272, 276, 280, 283, 288, 292, 296, 300, 304, 308, 313, 316, 320, 324, 328, 332, 336, 340, 345, 348, 353, 357, 361, 364, 368, 372, 377,
380, 385, 389, 393, 397, 401, 405, 409, 412, 417, 421, 425, 428, 432, 436, 441, 444, 448, 452, 456, 460, 464, 468, 473, 476, 481, 485, 489, 492, 496, 500,
505, 508, 513, 517, 521, 525, 529, 533, 537, 540, 545, 549, 553, 557, 561, 565, 570, 573, 577, 581, 585, 589, 593, 597, 601, 604, 609, 613, 617, 620, 624,
628, 633, 636, 641, 645, 649, 653, 657, 661, 665, 668, 673, 677, 681, 684, 688, 692, 697, 700, 704, 708, 712, 716, 720, 724, 729, 732, 737, 741, 745, 748,
752, 756, 761, 764, 768, 772, 776, 780, 784, 788, 792, 795, 800, 804, 808, 812, 816, 820, 825, 828, 832, 836, 840, 844, 848, 852, 857, 860, 865, 869, 873,
876, 880, 884, 889, 892, 897, 901, 905, 909, 913, 917, 921, 924, 929, 933, 937, 940, 944, 948, 953, 956, 960, 964, 968, 972, 976, 980, 985, 988, 993, 997,
```
Similar lists can be generated for any other clock speed where resolution is coarser than 1us and a TCB is used.

The number of values will, for ideal values, be `(1000) * (1 - (1 / resolution))` or something very close to that. Non-ideal series of terms or particularly adverse clock speeds may have more than that, as well as more cases of consecutive times that get the same micros value. If the terms were chosen poorly, it is possible for a time t give micros M where M(t-1) > M(t), violating the assumptions of the rest of the core and breaking EVERYTHING. 1.3.7 is believed to be free of them for all supported clock speeds. Likewise, it is possible for there to be larger skips or discontinuities, where M(t) - M(t-1) > 2, in contrast to skips listed above (where M(t) - M(t-1) = 2 ) or consecutive times return a repeated value (ie, M(t) = M(t-1))

Timer options which have resolution of 1us (internally, it is lower) may have repeats or skips if fewer than the optimal number of terms were used (as is the case with non optimized options) or where the clock speed is particularky hard to work with, like the 28 MHz one.
