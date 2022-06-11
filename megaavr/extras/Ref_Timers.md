# PWM and Timers on modern AVRs and their usage in this core
This document is divided into two sections. The first one simply describes the available timers, and what they are capable of (by "simply describes" I don't claim to have made a simple description, only that the purpose is simple. and that is to describe the timers). The second section describes how they are used by this core in particular. The first section is shared by DxCore and megaTinyCore. The second contains many sections specific to one core or another.

## Background: The Timers on the modern AVR parts
This applies to the tinyAVR 0/1/2-series, megaAVR 0-series, and AVR DA, DB and upcoming DD and EA-series, and likely all future AVRs. There are very few differences between the implementations on the different families.

### TCA0 - Type A 16-bit Timer with 3/6 PWM channels
This timer is the crown jewel of the modern AVR devices, as far as timers go. It can be operated in two very different modes. The default mode on startup is "Normal" or `SINGLE` mode - it acts as a single 16-bit timer with 3 output compare channels. It can count in either direction, and can also be used as an event counter (ie, effectively "clocked" off the event), is capable of counting up or down, generating PWM in single and dual slope modes, and has 7-output prescaler. For most use cases, a TCA in SINGLE mode is on the same level as the classic avr 16-bit Timers, only with more outputs (especially for 8-bit PWM) - the newly added features aren't ones that are particularly relevant for Arduino users. In this mode, TCA0 can generate events or interrupts on compare match for each channel (independently), as well as on an overflow.

The Type A timer can be also be set to split mode to get six 8-bit PWM channels (this is how it is configured by default by megaTinyCore and DxCore - since `analogWrite()` is only 8-bit, we might as well double the number of channels we get right? In split mode the high and low bytes of the timer count `TCA0.SINGLE.CNT` register become `TCA.SPLIT.LCNT` and `TCA.SPLIT.LCNT`; likewise the period and compare registers *in SINGLE mode, these are 16-bit registers; accessing them uses the temporary register. In SPLIT mode, they are 8-bit registers!*. Unlike the TCB in PWM mode, this works correctly. The frequency of the two "halves" of the timer is always the same (prescaler is shared between the two). However, the HPER and LPER registers can be used to adjust the period independently. This means that you could change the number of counts per cycle, thus allowing for the two halves to output different frequencies of PWM - albeit at a cost in resolution. There is an example of this under the link below).

There are a few examples of using TCA0 to generate PWM at specific frequencies and duty cycles in the document on [Taking over TCA0](./TakingOverTCA0.md)

The pins that the type A timer is directed to can be mapped to a variety of different pins. These timer waveform output channels are numbered WO0 through WO5.
* On ATtiny parts, each Waveform Output (WO) channel can be remapped separately to one of two pins.
* On ATtiny parts with 14+ pins, WO0-2 can be mapped to either PB0-2 or PB3-5, and WO3-5 (the split mode only ones) can be mapped to PA3-5 or PC3-5.
* On ATtiny parts with 8 pins, they are mapped to (in order) PA3-or-PA7, PA1, PA2, and PA3. Only the first 4 are available, and WO0 must be remapped in order to use WO3.
* On everything else, each TCA can only be remapped as a group. TCA0 can (on all parts thus far released) be directed to pins 0-5 of any port. TCA1 can be directed to PB0-5, PG0-5, or Px4-Px6 (WO0-WO2 - no split mode channels) on some other ports; PORTC and PORTE are options on DA/DB parts, with EA-series adding PORTD and PORTA. Note that on AVR128DA64 parts (as of Q1 2022) all available hardware is impacted by errata: PORTG and PORTE are not usable, leaving only PORTB (all 6 channels) or PORTC (WO0-2).

On all parts, the TCA can be used to count events, instead of system clocks (in this mode, the prescaler is ignored). On 2-series parts, and AVR Dx-series and later, there is a second event input dedicated to things like restarting as well as allowing the timer count direction to be reversed when an event is HIGH. **The event system features are not available in split mode.**

#### Interrupt note
When firing interrupts from a TCA, you must ALWAYS manually clear the intflags. It is never done for you.

### TCBn - Type B 16-bit Timer
The type B timer is what I would describe as a "utility timer". A very, very good utility timer. In this role, it can be set to any of 7 different modes.

#### Periodic interrupt
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
In addition to the input capture modes, they can be used for precise generation of a pulse with timing accuracty down to a single clock cycle. In this mode, the output pin will go high or low (selectable) in response to the specified edge of an event. The counter will count up to it's CCMP value, then reset and set the output pin low (these are the same output pins as are used for PWM). It is NOT NECESSARY to use the pin itself - you can use this instead to generate a event or interrupt after a delay has passed. Likewise, it could be triggered with an event channel connected to nothing, and manually kicked off with a software event. Without relying on TCA's prescaler, the 16-bit nature of this timer permits surprisingly long delays! **Warning** On at least some parts (I suspect, likely all parts - I remember having strange issues working with this feature, too...) with EDGE = 1, both rising and falling edges will kick off single-shot mode, rather than only falling edges.

#### Timeout Check
In this mode, one type of edge resets the counter to 0 and starts it counting, while the other edge stops the counting. In this mode the interrupt fires when the count passes CCMP - that's the "time out" that it's checking for. While this may at first sound like an odd feature, it is in fact extremely useful in combination with the custom logic and event system. The magic is that because it is both reacting to and can output an event you can use it to do things in the background. Among other things, it is exceptionally useful for working with bespoke single wire protocols.

#### 8-bit PWM mode
The type B timer can be pressed into service as a rather poor PWM timer. A TCB in PWM mode is 8-bit only. When used for PWM, they can only generate 8-bit PWM, despite being a 16-bit timer, because the 16-bit `TCBn.CCMP` register is used for both the period and the compare value in the low and high bytes respectively. They always operate in single-slope mode, counting upwards, the selection of clock options is limited in the extreme: system clock, half the system clock, or the frequency depends on that of the TCA (or one of the TCA's on parts with more than one).  In other words, **the type B timers are not very good at generating PWM**. Note also that `TCBn.CCMP` is effected by a rather nasty silicon errata: CCMPL and CCMPH do not act as independent 8 bit registers - they still act like a 16-bit register, using the temp register for access. Hence, you must read or write the low byte first, and then the high byte, even if you only want to read or write one of the the two.

#### Extra features on 2-series and Dx
The 2-series and AVR Dx parts add three upgrades to the TCBs: one very useful, one moderately useful, and the third rarely useful. The least useful one is a separate OVF event/interrupt source. The only thing I see it being practical for is to extend the input capture to longer periods.

The very useful upgrade is the clock-on-event option: The TCBs now have a second event user, which can be selected as the clock source. Combined with clever uses of the event system and/or CCL, this can produce sophisticated functionality without requiring CPU intervention!

Finally, you can combine two timers to make a single, monster, 32-bit input capture timer using the CASCADE feature. In this mode, the high timer is clocked from the OVF event of the low timer, and the CASCADE bit enabled. When the event fires, both timers will get their CCMP values saved and/or be stopped, permitting incredible precision (or just really long things to be measured). 2<sup>32</sup> / 20 MHz = 214 seconds. Prescaling the the low timer by 2 would double that... and monitoring the OVF bit of the high timer could double it again. Ever wanted to time an approximately 12 minute long event to a resolution of tenths of a microsecond? (well, okay, tenths of some period of time that is within a percent or so of a microsecond for internal clock sources, and within some number of PPM for external clock sources). This is a painful feature to make use of on 2-series tinyAVR (or a DD-series with 14 or 20 pins) as they only have two type B timers in total.

#### Intflag summary
Sometimes the intflags automatically clear, but usually they don't. Be sure to clear them manually within your ISR if you have enabled them as an interrupt source in the cases where it isn't done automatically - otherwise the interrupt will be triggered continually once the condition occurs a single time. This causes only one non-interrupt instruction to execute between each round of the ISR, with the observed effect of the sketch speed slowing down tremendously. Millis timekeeping will either keep correct time, or not advance at all, depending on which timer interrupt is misfiring and whether it is higher or lower than the millis timer. Flags are set whether or not the interrupt is enabled, and need not be cleared unless the corresponding bit in `INTCTRL` is set.

The OVF flag only exists on 2-series parts.

| Mode              | CAPT Reset by | OVF reset by | OVF set under normal use |
|-------------------|---------------|--------------|--------------------------|
| Interrupt         | User code     | User code    | Only when CCMP set below CNT, so no CAPT interrupt occurs until next cycle  |
| Timeout Check     | User code     | User code    | Only when timeout goes on long enough for the timer to overflow |
| Input capt. Event | Read CCMP     | User code    | Every time the counter rolls over. Rarely needed |
| Input capt. (all) | Read CCMP     | User code    | When counter rolls over. Effectively 17th bit |
| Singleshot        | User code     | User code    | No.  |
| PWM               | User code     | User code    | At end of every PWM cycle |


### TCD0 - Type D 12-bit Async Timer
The Type D timer, present only on the 1-series tinyAVR, and the AVR DA, DB, and DD-series, is a very strange timer indeed. It can run from a totally separate clock supplied on EXTCLK, or from the unprescaled internal oscillator (on the Dx-series, from the on-chip PLL at 2, 3, or even 4 times the speed of the external clock or internal oscillator is also an option, though the 4x multiplication option is unofficial - it was in the very earliest headers, but was never in the datasheet and was removed from headers shortly after release. It was not however removed from the silicon!) This timer appears to be the descendant of the PSC timer on the ATmega 64M1 and Timer1 of the ATtiny861 and 85.

It can be clocked from the *unprescaled* internal oscillator, or an external clock source that is not being used as the system clock - so it can run faster than the CPU even on a tinyAVR, and it has it's own prescaler - actually, two of them - a "synchronizer" clock that can then be further prescaled for the timer itself. It supports normal PWM (what they call one-ramp mode) and dual slope mode without that much weirdness, beyond the fact that `CMPBSET` is TOP, rather than it being set by a dedicated register. But the other modes are quite clearly made for driving motors and switching power supplies. Like the older designs it is derived from there is a mechanism for generating programmable dead time between cycles.

It also has a 'dither' option to allow PWM at a frequency in between frequencies possible by normal division of the clock - a 4-bit value is supplied to the TCD0.DITHER register, and this is added to a 4-bit accumulator at the end of each cycle; when this rolls over, another clock cycle is inserted in the next TCD0 cycle. And it has a "delayed event" function to allow it to generate an event output some number of clock cycles after a compare match. Among other things, this can be used to create a third PWM channel - see the included example - using one of the CCL logic blocks. It can use event inputs to automatically do various things like jumping ahead to the next compare cycle, or pausing the output while an event is asserted.

It was designed with a particular eye towards motor control and SMPS control applications. This makes it very nice for those sorts of (typically very sophisticated) use cases, but in a variety of ways, these get in the way of using it for the sort of things that people who would be using the Arduino IDE are likely to want to do with it. First, none of the control registers can be changed while it is running; it must be briefly stopped, the register changed, and the timer restarted. In addition, the transition between stopping and starting the timer is not instantaneous due to the synchronization process. This is fast (it looks to me to be about twice the synchronization prescaler, which makes sense; so when running from the same clock as the CPU, would be 2-16 clock cycles (we set the sync prescaler to the minimum of 1)) but it is not instantaneous, and your code must wait until it is ready to be re-enabled.

Most changes to settings require it to be disabled as noted above - and you need to wait for that operation to complete (check for the `ENABLERDY` bit in `TCD0.STATUS`). Similarly, to tell it to apply changes made to the `CMPxSET` and `CMPxCLR` registers, you must use the `TCD.CTRLE` (the "command" register) to instruct it to synchronize the registers (`AUPDATE` is broken on some tinyAVR revisions, see the errata sheet).The same thing applies to reading the value of the counter (did you expect there to be a TCD0.CNT register? There isn't!) - you have to request a capture by writing the SCAPTUREx bit of TCD0.CTRLE, and wait a sync-delay (I suspect a slightly longer delay - probably 2 CPU clocks longer, since the request has to by sync'ed to the timer domain, then the value sync'ed to the CPU domain.

In the case of turning PWM channels on and off, not only must the timer be stopped, but a timed write sequence is needed ie, `_PROTECTED_WRITE(TCD0.FAULTCTRL,value)` to write to the register that controls whether PWM is enabled; this is apparently because, in the intended use-cases of motor and switching power supply control, changing this accidentally (due to a wild pointer or cosmic ray or whatever) could have catastrophic consequences (for example, causing an H-bridge used in motor control to "shoot through" (turn on both transistors at the same time, shorting the supply through the bridge) and burn out the power supply, and/or the H-bridge, or in a switching power supply, cause an inappropriate voltage to be supplied to the load, burning it out or causing inappropriate and/or destructive operation). Writes to any register when it is not "legal" to write to it will be ignored, and you will wonder why only one of the two registers you tried to change was actually changed. Thus, making use of the type D timer for even simple tasks requires careful study of TCD0 section datasheet - which despite being the longest chapter of the datasheet on any modern AVR, is still quite terse at times, leaving one guessing as to the intended behavior. Where this is the case, the behavior will often be frustrating and counterintuitive; to say that the type D timer is harder to use than the other timers is a huge understatement.

### RTC - 16-bit Real Time Clock and Programmable Interrupt Timer
Information on the RTC and PIT will be added in a future update.

#### RTC/PIT errata on 0/1-series
One very important thing to be aware of on the tinyAVR 0/1-series only (and not the 3216/3217, where this has been fixed) is that there is some NASTY errata here. First, you must never have the RTC running without the PIT, nor the pit without the RTC). The Microchip errata for impacted parts does not do justice to the extent of how strange the resulting behavior is. Second, the PIT timer will be glitch every time RTC.CTRLA is written to, because the prescaler count gets reset (since you can have up to 15 bits of prescaling, this can set the timer back up to 1 second (assuming a 32.768 kHz clock)). It is also easy to inadvertently rely on this to get a deterministic delay from the PIT, which is not supposed to be possible. The intended behavior is that if you set the PIT to generate an interrupt every X seconds, it will do that, but you don't get to control where in the cycle you start from.

### Timer Prescaler Availability

Prescaler    | TCAn  | TCBn  | TCD0  | TCD0 sync | TCD0 count | TCD0 Delay |
------------ | ------|-------|-------|-----------|------------|------------|
CLK          |  YES  |  YES  |  YES  |  YES      |  YES       | YES        |
CLK/2        |  YES  |  YES  |  YES* |  YES      |  NO        | YES        |
CLK/4        |  YES  |  TCA  |  YES  |  YES      |  YES**     | YES        |
CLK/8        |  YES  |  TCA  |  YES  |  YES      |  NO        | YES        |
CLK/16       |  YES  |  TCA  |  YES* |  NO       |  NO        | NO         |
CLK/32       |  NO   |  NO   |  YES  |  NO       |  YES**     | NO         |
CLK/64       |  YES  |  TCA  |  YES* |  NO       |  NO        | NO         |
CLK/128      |  NO   |  NO   |  YES* |  NO       |  NO        | NO         |
CLK/256      |  YES  |  TCA  |  YES* |  NO       |  NO        | NO         |
CLK/1024     |  YES  |  TCA  |  NO   |  NO       |  NO        | NO         |

`*` Requires using the synchronizer prescaler as well. My understanding is that this results in sync cycles taking longer.
`TCA` indicates that for this prescaler, a TCA must also use it, and then that can be prescaled, and the TCB set to use that TCA's clock.
`**` Enabling the count prescaler can result in missing async event inputs - either use synchronous event inputs if using event inputs, or do not use the count prescaler.

### Resolution, Frequency and Period
When working with timers, I constantly found myself calculating periods, resolution, frequency and so on for timers at the common prescaler settings. While that is great for adhoc calculations, I felt it was worth some time to make a nice looking chart that showed those figures at a glance. The numbers shown are the resolution (when using it for timing), the frequency (at maximum range), and the period (at maximum range - ie, the most time you can measure without accounting for overflows).
### [In Google Sheets](https://docs.google.com/spreadsheets/d/10Id8DYLRtlp01KA7vvslC3cHaR4S2a1TrH7u6pHXMNY/edit?usp=sharing)

## Section Two: How the core uses these timers

### PWM via analogWrite()
#### TCAn
The core reconfigures they type A timers in split mode, so each can generate up to 6 PWM channels simultaneously. The `LPER` and `HPER` registers are set to 254, giving a period of 255 cycles (it starts from 0), thus allowing 255 levels of dimming (though 0, which would be a 0% duty cycle, is not used via analogWrite, since `analogWrite(pin,0)` calls `digitalWrite(pin,LOW)` to turn off PWM on that pin). This is used instead of a PER=255 because `analogWrite(255)` in the world of Arduino is 100% on, and sets that via `digitalWrite()`, so if it counted to 255, there have to be a gap, as there would be 257 possibilities (HIGH, LOW, and 255 intermediate values) but only 256 different values could be passed to analogWrite. Additionally, modifications would be needed to make `millis()`/`micros()` timekeeping work without drift at that period anyway - 255 happens to work exceptionally well mathematically (for reasons that I do not understand - this was noticed when reimplementing the timekeeping to prevent time travel during development of megaTinyCore 1.x ).

`analogWrite()` on tinyAVR parts does NOT interpret the PORTMUX settings. The logic required is more complex than on DxCore while resources are more limited.

#### TCD0
TCD0, by default, is configured for generating PWM (unlike TCA's, that's about all it can do usefully). TCD0 is clocked from CLK_PER when the system is using the internal clock without prescaling. On the prescaled clocks (5 and 10 MHz) it is run it off the unprescaled oscillator (just like on the 0/1-series parts that it inherits the frequencies from), keeping the PWM frequency near the center of the target range.

It is always used in single-ramp mode, with `CMPBCLR` (hence TOP) set to 509 (for the same reason that TCA uses 254 as TOP; this is used to in effect add an additional divide-by-2 to the prescaler without increasing the sync prescaler or the clock frequency. )

The output channels are only available through megaTinyCore on 20 and 24 pin parts (However, see below for planned options in 2.6.x)

#### TCBn
The type B timers are never used by megaTinyCore for PWM.

#### PWM Frequencies
The frequency of PWM output using the settings supplied by the core is shown in the table below. The "target" is 1 kHz, never less than 490 Hz or morethan 1.5 kHz. As can be seen below, there are several frequencies where this has proven an unachievable goal. The upper end of that range is the point at which - if PWMing the gate of a MOSFET - you have to start giving thought to the gate charge and switching losses, and may not be able to directly drive the gate of a modern power MOSFET and expect to get acceptable results (ie, MOSFET turns on and off completely in each cycle, there is minimal distortion of the duty cycle, and it spends most of it's "on" time with the low resistance quoted in the datasheet, instead of something much higher that would cause it to overheat and fail). Not to say that it **definitely** will work with a given MOSFET under those conditions (see [the PWM section of my MOSFET guide](https://github.com/SpenceKonde/ProductInfo/blob/master/MOSFETs/Guide.md#pwm) for calculations and a shared spreadsheet that helps calculate  ), but the intent was to try to keep the frequency low enough that that use case was viable (nobody wants to be forced into using a gate driver), without compromising the ability of the timers to be useful for timekeeping.

##### TCA0

|   CLK_PER | Prescale |   fPWM  |
|-----------|----------|---------|
|    20 MHz |       64 | 1225 Hz |
|    16 MHz |       64 |  980 Hz |
|    10 MHz |       64 |  613 Hz |
|     8 MHz |       64 |  490 Hz |
|     5 MHz |       16 | 1225 Hz |
|     4 MHz |       16 |  980 Hz |
|     1 MHz |        8 |  490 Hz |

##### TCD0 (PC0, PC1 on 1-series only)

*warning* - When using the Tuned Internal oscillator clock option, the PWM frequency will scale up or down with the CPU speed, as shown below for the highest and lowest tuned frequencies available for each. However, when an external clock source is used, the internal oscillator will be left at it's default calibration (16 or 20 MHz). This will be used for TCD PWM, which we always generate from the internal oscillator (though it can be set to an external clock source,


| FREQSEL fuse  | TCD clock source | Sync Prescale | Count prescale | TOP |    fPWM |
|---------------|------------------|---------------|----------------|-----|---------|
| 0x02 (20 MHz) | OSCHF @ 32 MHz   |             2 |             32 | 509 |  980 Hz |
| 0x02 (20 MHz) | **OSCHF @ 20 MHz** |           1 |             32 | 509 | 1225 Hz |
| 0x02 (20 MHz) | OSCHF @ 12 MHz   |             1 |             32 | 509 |  735 Hz |
| 0x01 (16 MHz) | OSCHF @ 25 MHz   |             1 |             32 | 509 | 1531 Hz |
| 0x01 (16 MHz) | **OSCHF @ 16 MHz** |           1 |             32 | 509 |  980 Hz |
| 0x01 (16 MHz) | OSCHF @ 10 MHz   |             1 |             32 | 509 |  612 Hz |

This section is incomplete and will be expanded at a later date.

#### Planned new PWM options for 2.6.x versions
Starting from 2.6.0, we are planning to add an additional tools submenu, PWM pin configuration. This functionality is not yet implemented

Unlike DxCore, where the overhead of identifying the timer and channel is low and flash abounds, the calculations are longer and the resources more limited here. Thus, we cannot offer automatic PWM moving by simply setting PORTMUX like we could there, however, starting in 2.6.0, there will be a tools menu to select from several PWM layouts.

Note: This cannot be made changeable at runtime; it *must* be a tools menu option, because the number of conditionals involved would cause unacceptable code bloat.

| Option description        | TCA0 PWM        | TCD0 PWM | 14pin | 20pin | 24pin | Notes |
|---------------------------|-----------------|----------|-------|-------|-------|-------|
| Default (6xTCA)           | PB0-2,PA3-5     | None     | Yes   | No    | No    | On 14-pin parts, this is default, Both of the TCD pins already have pwm from TCA which can't be remapped, TCD is not used for PWM on those   |
| No TCD (6xTCA)            | PB0-2,PA3-5     | None     | No    | Yes   | Yes   | On 20/24-pin parts, this will save a non-negligible amount of flash, and improve performance of digitalWrite and analogWrite. No TCD PWM.  |
| Default (6xTCA, 2xTCD)    | PB0-2,PA3-5     | PC0, PC1 | No    | Yes   | Yes   | On 14-pin 1-series, Both of the TCD pins already have pwm from TCA which can't be remapped, TCD is not used for PWM on those   |
| Avoid UART (6xTCA, 2xTCD) | PB0,1,5 PA3-5   | PC0, PC1 | No    | Yes   | Yes   | On 20/24-pin parts we can avoid wasting any PWM channels on the USART pins |
| Avoid Serial (6xTCA)      | PB1-3,PA3-5     | None     | Yes   | No    | No    | Minimizes conflict between PWM and SPI/I2C/USART as much as possible on 14-pin parts. No TCD0 PWM. |
| Avoid Serial (6xTCA 2xTCD)| PB3-5,PC3-5     | PC0, PC1 | No    | No    | Yes   | Minimizes conflict between PWM and SPI/I2C/USART. TCD0 PWM on PA4, PA5. |
| Avoid UART (3xbTCA, 2xTCD)| PB0-2           | PA4, PA5 | Yes   | No    | No    | Avoid UART pins when possible. 14-pin can't fully avoid USART pins. PWM output buffering enabled. TCD0 PWM on PA4, PA5. |
| Avoid UART (3xbTCA, 2xTCD)| PB0,1,5         | PA4, PA5 | No    | Yes   | Yes   | Avoid UART pins completely. PWM output buffering enabled. TCD0 PWM on PA4, PA5. |
| Avoid UART (3xbTCA, 2xTCD)| PB0,1,5         | PC0, PC1 | No    | Yes   | Yes   | Avoid UART pins completely. PWM output buffering enabled. TCD0 PWM on PC0, PC1 (note that these are also alternate pin mapping options USART1 on 2-series). |
| Avoid UART (3xbTCA)       | PB0-2           | None     | Yes   | No    | No    | 14-pin can't fully avoid USART. PWM output buffering enabled. No TCD0 PWM - saves flash. |
| Avoid UART (3xbTCA)       | PB3-5           | None     | No    | Yes   | Yes   | 14-pin can't fully avoid USART. PWM output buffering enabled. TCD0 PWM on PA4, PA5. |
| Avoid I2C (3xbTCA, 2xTCD) | PB1-3           | PA4, PA5 | Yes   | No    | No    | 14-pin can't fully avoid I2C. You lose one of the PWM pins if you want I2C (though the 1-series - and only the 1-series - can move I2C to PA1 and PA2). PWM output buffering enabled. TCD0 PWM on PA4, PA5. |
| Avoid I2C (3xbTCA, 2xTCD) | PB3-5           | PC0, PC1 | No    | Yes   | Yes   | Avoid I2C pins. PWM output buffering enabled. TCD0 PWM on PC0, PC1. |
| Avoid I2C (3xbTCA, 2xTCD) | PB3-5           | PA4, PA5 | No    | Yes   | Yes   | Avoid I2C pins. PWM output buffering enabled. TCD0 PWM on PA4, PA5. |
| Avoid I2C (3xbTCA)        | PB1-3           | None     | Yes   | No    | No    | Avoid I2C pins. PWM output buffering enabled. No TCD0 PWM - saves flash. |
| Avoid I2C (3xbTCA)        | PB3-5           | None     | No    | Yes   | Yes   | Avoid I2C pins. PWM output buffering enabled. No TCD0 PWM - saves flash. |
| Default 8-pin (4xTCA)     | PA1,2,3,7       | None     | No    | No    | No    | 8-pin parts only, the default
| 3 pins (3xbTCA)           | PA1-3           | None     | No    | No    | No    | 8-pin parts only, trade 4th pwm pin for buffering and a bit more flash.
| 5 pins (3xbTCA, 2xTCD)    | PA1-3           | PA6, PA7 | No    | No    | No    | 8-pin 1-series parts only. 5 PWM pins. A flash hog - these parts max out at just 4k of flash. The so-called "no glitch TCD" implementation is not used in order to save flash in this case.

##### TCD PWM pins
On the 14-pin parts, when TCA is used in split mode, the only pins available for TCD PWM are PA4 and PA5 - which are already the only outputs WO4 and WO5 of TCA0 on those parts. Hence, TCD PWM is not used in that configuration as it would just waste flash without giving you more PWM pins. When TCA is not used in split mode, that opens the door to TCD-generated PWM on the 14-pin 1-series parts. Also, alternate mappings of the TCA PWM pins make TCD PWM on PA4, PA5 reasonable to use on 24-pin parts depending on what alternate functions you need from the pins not used for PWM.
If TCD PWM pins are not needed, disabling that functionality saves a bit of flash.

##### Buffering/3-pin mode? What's that?
This means the following things will be done differently in the TCA configuration:
* TCA is not run in split mode. There are only 3 output channels.
* The PERBUF CMP0BUF, CMP1BUF, and CMP2BUF can be used instead of PER, CMP0, CMP1, or CMP2, and analogWrite will do this. When the buffer registers are used, the changes are applied at the end of the current duty cycle, preventing glitches when those registers are changed. This is occasionally a problem with PWM for brightness control, where there would be a 1/255 chance analogWrite(pin,1) called when the duty cycle was previously 2, to cause the LED to be on for an entire PWM cycle. This is usually difficult to generate visible effects from, but in certain lighting situations has for reasons unclear, been problematic, resulting in a distracting flash when transitioning between very low brightness levels.
* PER and prescaler are initialized the same was as above
* However, because it is not in split mode, they can be changed to 16-bit values.
  * In 2.6.x the megaTinyCore library will be expanded with functions to support this; it is conceptually surprisingly hard to do
* Buffered mode uses slightly less flash.
  * If TCD is added as a result of changes to this menu option, that will increase the flash usage significantly.
* The number of buffered channels is abbreviated bTCA in above table.

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

Except when the resolution is way down near the minimum, the device spends more time in the ISR when using a TCA. Notice that at these points that - barely - favor TCAn, the interrupt they're being compared to is firing twice as frequently!


#### TCBn for millis timekeeping
When a TCB is used for `millis()` timekeeping, it is set to run at the system clock prescaled by 2 (except at 1 or 2 MHz system clock) and tick over every millisecond. This makes the millis ISR very fast, and provides 1ms resolution at all but the slowest speeds for millis. The `micros()` function also has 1 us or almost-1 us resolution at all clock speeds (though there are small deterministic distortions due to the performance shortcuts used for the microsecond calculations (see appendix below). The only reason these excellent timers are not used by default is that too many other useful things need a TCB.

|Note | CLK_PER | millis() | micros() | % in ISR | micros() time | Terms used             |
|-----|---------|----------|----------|----------|---------------|------------------------|
|  †  |  32 MHz |     1 ms |     1 us |   0.20 % |          3 us | 1                      |
|     |  30 MHz |     1 ms |  1.07 us |   0.22 % |        < 6 us | 5 (3,4,7,8)            |
| !   |  28 MHz |     1 ms |  1.14 us |   0.23 % |       <= 6 us | 5/7 (2,3,5,6 ~8,9~ )   |
| !   |  27 MHz |     1 ms |  1.18 us |   0.24 % |       <= 6 us | 4 (2,4,9)              |
|     |  25 MHz |     1 ms |  1.28 us |   0.26 % |       <= 6 us | 3/5 ( ~1,~ 2, ~4,~ 5)  |
|   * |  24 MHz |     1 ms |  1.33 us |   0.27 % |          5 us | 9 (0-7, 9)             |
|   * |  20 MHz |     1 ms |     1 us |   0.33 % |          6 us | 6 (2,4,6,8,10)         |
|  †  |  16 MHz |     1 ms |     1 us |   0.40 % |          6 us | 1                      |
| !   |  14 MHz |     1 ms |  1.14 us |   0.47 % | approx. 12 us | 5/7 (2,3,5,6 ~8,9~ )   |
|   * |  12 MHz |     1 ms |  1.33 us |   0.54 % |         10 us | 9 (0-7, 9)             |
|     |  10 MHz |     1 ms |     1 us |   0.65 % |         10 us | 6 (2,4,6,8,10)         |
|  †  |   8 MHz |     1 ms |     1 us |   0.80 % |         11 us | 1                      |
| !   |   7 MHz |     1 ms |  1.14 us |   0.94 % | approx. 25 us | 5/7 (2,3,5,6 ~8,9~ )   |
| ! * |   6 MHz |     1 ms |  1.33 us |   1.08 % |       > 20 us | 9 (0-7, 9)             |
|   * |   5 MHz |     1 ms |     1 us |   1.30 % |         23 us | 6 (2,4,6,8,10)         |
|  †  |   4 MHz |     1 ms |     1 us |   1.60 % |         21 us | 1                      |
| ! * |   3 MHz |     1 ms |  1.33 us |   2.16 % |       > 40 us | 9 (0-7, 9)             |
| !†  |   2 MHz |     2 ms |     1 us |   1.60 % |         39 us | 1                      |
|  †  |   1 MHz |     2 ms |     1 us |   3.25 % |         78 us | 1                      |

`!` - Theoretical, these speeds are not supported and have not been tested. % time in ISR and micros return times, where given are theoretically calculated, not experimentally verified.
`†` - Naturally ideal - no ersatz division is needed for powers of 2
`*` - indicates that the shift & sum ersatz-division is done in hand-optimized assembly because I just couldn't stand how stupid and stubborn the compiler was, and by the time I was done analyzing it, implementing it was mostly done. These use every term that would improve the accuracy of the approximation, shown in the final column. The algorithm divides the timer ticks (which is between zero and F_CPU/2000) by a power if 2 (by right shifting) to get term 0; a number of terms (generated by further rightshifting) are then added and subtracted to get the time since the last millis was counted, and that total is added to 1000x the millisecond count. The ideal number of terms varies from 1 (for powers of 2 - only term 0) to 9 (for the terrible twelves (12, 24, 48, as well as 3 and 6). In optimized cases, that's how many are used. Otherwise, some may be dropped, or different combinations used (adding 1 term, then subtracting the next, would seem equivalent to just adding the next - but because of integer math, it's not, and in fact can lead to several us of backwards timetravel, so if you'd have to do the same operation twice in a row, you get more accurate numbers if you avoid that by adding another term). In optimized cases, as well as the powers of 2, the results are as good as possible in light of the specified resolution - where resolution is coarser than 1us, (1/resolution) of possible values for the three lowest digits (in normal base 10 number systems) is skipped and never shows up. These skipped values are distributed evenly between 1 and 999. For example, for 12/24/48, that means 250 values never show up. Of the remaining 750, 500 will show for only 1 "actual" microsecond value, and 250 for 2 consecutive "actual" microsecond values. See the table at the end of this document for the list of skipped least-significant-digit combinations. None of the optimized options will never go back in time, even by a single microsecond (as long as the chance of backward time travel is limited to less than the time it takes micros to execute, it can't break `delay()`, cause timeouts to instantly expire or cause other catastrophic consequences) nor are more than 2 values in a row ever skipped. Those criteria are not guaranteed to be met for other speeds, though we do guarantee that negative time travel will never occur that causes two consecutive calls to micros to return a smaller value for the second call which is what triggers misbeahvior of timing code.

The terms used, and - where different - the number that would be ideal, is listed above

Resolution is always exactly 1ms for millis with TCB millis except for 1 and 2 MHz speeds, where the millis resolution is 2ms, though micros resolution is still 1us, and whereas TCAn `micros()` is limited by the resolution of the timer, here it's instead limited only by the fact that the calculations drop the least significant bits first; this results in a value that may be as low as 750, yet is being mapped to 0-999, for 1.33 us resolution in the worst cases. The timer count and the running tally of overflows could get us microseconds limited only by F_CPU/2.
The percentage of time spent in the ISR varies in inverse proportion to the clock speed - the ISR simply increments a counter and clears its flags, making it lightning fast. 65 clocks from interrupt bit set to interrupted code resuming.

The time that `micros()` takes to return a value varies significatly with F_CPU; where not exact, value is estimated. Specifically, powers of 2 are highly favorable, and almost all the calculations drop out of the 1 and 2 MHz cases (the are similar mathematically - at 1 MHz we don't prescale and count to 1999, at 2 we do prescale and count to 1999, but then we need only add the current timer count to 1000x the number of milliseconds. micros takes between 78 and 160 clocks to run. Everything else has to use some version of a shift-and-sum ersatz division algorithm, since we want to approximate division using `>>`, `+` and `-`. Each factor of 2 increase in clock speed results in 5 extra clocks being added to micros in most cases (bitshifts, while faster than division, are still slow when you need multiples of them on larger types, especially in compiler-generated code that doesn't reuse intermediate values.)

### TCD0 for millis timekeeping
This will be documented in a future release.

### Tone
The `tone()` function included with megaTinyCore uses one Type B timer. It defaults to using TCB0, unless the part has a TCB1 and TCB0 is used for millis. Tone is not compatible with any sketch that needs to take over TCB0. If possible, use a different timer for your other needs. When used with Tone, it will use CLK_PER or CLK_PER/2 as it's clock source - the TCA clock will never be used, so it does not care if you change the TCA0 prescaler (unlike the official "megaAVR" core).

Tone otherwise works in largely the same ways as the normal `tone()` function on official Arduino boards. Unlike the official megaAVR board package's tone function, it can be used to generate arbitrarily low frequency tones (as low as 1 Hz). If the period between required toggling's of the pin is greater than the maximum timer period possible, it will calculate how many cycles it has to wait through between switching the pins in order to achieve the desired frequency.

It can only generate a tone on one pin at a time.

All tone generation is done via interrupts. The hardware output compare functionality is not used for generating tones because in PWM mode, the type B timers aren't capable of 16-bit PWM.

### Servo Library
The Servo library included with this core uses one Type B timer. It defaults to using TCB1 if available, unless that timer is selected for Millis timekeeping. Otherwise, it will use TCB0. The Servo library is not compatible with any sketch that needs to take over these timers - if possible, use a different timer for your other needs. Servo and `tone()` can only be used together on when neither of those is used for millis timekeeping.

Regardless of which type B timer it uses, Servo configures that timer in Periodic Interrupt mode (`CNTMODE`=0) mode with CLK_PER/2 or CLK_PER as the clock source, so there is no dependence on the TCA prescaler. The timer's interrupt vector is used, and it's period is constantly adjusted as needed to generate the requested pulse lengths. In 1.1.9 and later, CLK_PER is used if the system clock is below 10MHz to generate smoother output and improve performance at low clock speeds.

The above also applies to the Servo_megaTinyCore library; it is an exact copy except for the name. If you have installed a version of Servo via Library Manager or by manually placing it in your sketchbook/libraries folder, the IDE will use that in preference to the one supplied with this core. Unfortunately, that version is not compatible with the Dx-series parts. Include Servo_megaTinyCore.h instead in this case. No changes to your code are needed other than the name of the library you include.

### Additional functions for advanced timer control
We provide a few functions that allow the user to tell the core functions that they wish to take full control over a timer, and that the core should not try to use it for PWM. These are not part of a library because of the required integration with the core to control `analogWrite()` and `digitalWrite()` behavior.

#### takeOverTCA0()
After this is called, `analogWrite()` will no longer control PWM on any pins driven by TCA0, nor will `digitalWrite()` turn it off. TCA0 will be disabled and returned to it's power on reset state.
#### takeOverTCD0()
After this is called, `analogWrite()` will no longer control PWM on any pins attached to timer TCD0 (though it will attempt to use other timers, if any), nor will `digitalWrite()` turn it off. There is no way to reset type D timers like Type A ones. Instead, if you are doing this at the start of your sketch, override init_TCD0. If TCD is ever supported as millis timing source, this will not be available.
#### resumeTCA0()
This can be called after takeOverTimerTCA0(). It resets TCA0 and sets it up the way the core normally does and re-enables TCA0 PWM via analogWrite.
#### There is no resumeTCD0()
TCD0 is much more complicated, and has no builtin "reset" command to reinitialize it.

## Appendix I: Names of timers
Whenever a function supplied by the core returns a representation of a timer, these constants will be used

| Timer Name   | Value | Peripheral |          Used for |
|--------------|-------|------------|-------------------|
| NOT_ON_TIMER |  0x00 |            |                   |
| TIMERA0      |  0x10 |      TCA0  | millis and/or PWM |
| TIMERB0      |  0x20 |      TCB0  | millis            |
| TIMERB1      |  0x21 |      TCB1  | millis            |
| TIMERD0      |  0x40 |      TCD0  | millis and/or PWM |
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

Those 250 "missing" microseconds manifest as repeats. It is difficult to intuitively understand why these values are the values they are; these were calculated using a spreadsheet to simulate the results for every timer count value:
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

The number of values would, were true division used, be `(1000) * (1 - (1 / resolution))` and with well chosen terms, will be something very close to that. Non-ideal series of terms or particularly adverse clock speeds may have more than that, as well as more cases of consecutive times that get the same micros value. If the terms used in the ersatz division are chosen poorly, it is possible that there could exist a time t such that micros()<sub>t-1</sub> > micros()<sub>t</sub>, This violates the assumptions of the core, and in egregious cases, where micros()<sub>t-x</sub> > micros()<sub>t</sub> where x is the time taken for micros to return, is termed "backwards time travel" and causes severe timekeeping malfunctions. Likewise, it is possible for there to be larger skips or discontinuities, where micros()<sub>t</sub> - micros()<sub>t-1</sub> > 2, in contrast to skips listed above (where micros()<sub>t</sub> - micros()<sub>t-1</sub> = 2 ) or where consecutive times return the same value for more than two times in a row (ie, micros()<sub>t</sub> = micros()<sub>t-1</sub> = micros()<sub>t-2</sub>), a situation called "forward time travel" as you would expect, this is a less critical problem, and as such; this is quite common in the wild on cores at odd clock speeds just prior to the millis timer overflow (ie, it might count up to 750, then skip straight to 1000). As of 2.4.0 of megaTinyCore, it is believed that there is no time travel in either direction at any supported clock speed.
