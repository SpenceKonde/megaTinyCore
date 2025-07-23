# Detailed Clock Reference
This section seeks to cover information about the clocking options on the tinyAVR parts at somewhat greater depth than a simple list of supported options.
Be aware that this is very different from the equivalent document for the Dx and Ex-series. The oscillators are very different.

## Overview
The modern AVRs have been released with, effectively, three different systems for the clock generation.

### On tinyAVR
The OSCCFG fuse selects whether the speed is derived from a 20 MHz or 16 MHz oscillator. At power-on, these are always set to a prescaler of /6 hence 2.66 or 3.33 MHz. The core, prior to calling setup, will reconfigure the prescaler to match the requested speed. The OSCCFG fuse is always set when a sketch is uploaded via a UPDI programmer using megaTinyCore, however it cannot be set using the bootloader. These parts can't use an external HF crystal, only an external *clock*.

They additionally have the equivalent of the classic AVR's OSCCAL calibration register, and like those parts, the compliance is many times what the datasheet claims, and you can get a very wide range of speeds from them. 0/1-series has 64 settings for each nominal clock frequency, and 2-series parts have 128. At a range of clock speeds (12-24 on the main clock on most parts), both oscillators could be tuned to output the desired clock.

I suspect that the classic AVR oscillator with the 8-bit OSCCAL register that was bifurcated in the middle is a very similar mechanism. Note that while the ranges overlap,

### On Dx-series
Fuses only control whether it starts up at 4 MHz or 32 kHz (the latter is unsupported as a clock source . Code that runs very early in the sketch (before start() but after the dirty reset check and after the clears initializers) serves to initialize the peripherals, including clkctrl to their requested settings.

The OSCCTRL.OSCHFCTRLA register's *middle* 4 bits control the frequency of the internal oscillator. This field divides neatly into two sections. 0x0, 0x1, 0x2 and 0x3 are 1, 2, 3, and 4 MHz. The next one is reserved (I'd wager it would give 4 MHz). That's followed by the next section, 8, and every subsequent multiple of 4 up to 24 are listed in the datasheet, and at least at room temperature, extending the pattern 2 more rungs is consistent, and leads to speeds of 28 and 32 MHz, generally work at room temperature. (beyond that, the last 4 are repeated).

Additionally, there is a tuning register, CLKCTRL.OSCHFTUNE. Unfortunately, it is much less useful than the tinyAVR one. The register is limited to -32 to +31, corresponding to - and + around 16%. +/-16% doesn't exactly open up new frontiers.
If an external 32.768 MHz crystal has been connected, you can, for improved temperature stability, autotune the internal oscillator from this. I had to direct a torch at the chip (from a distance) to heat it up enough to see the speed autotune....
Not the feature I'm most impressed with. A+ on concept, C on execution.

Tuning to achieve non-standard speeds is not supported on the AVR Dx-series parts under DxCore.

### On EA-series
The worst of both worlds! The OSCCFG fuse selects whether the speed is derived from a 20 MHz or 16 MHz oscillator. At power-on, these are always set to a prescaler of /6 hence 2.66 or 3.33 MHz. The core, prior to calling setup, will reconfigure the prescaler to match the requested speed. The OSCCFG fuse is always set when a sketch is uploaded via a UPDI programmer.
Yhere is a tuning register, CLKCTRL.OSCHFTUNE. Unfortunately, it is much less useful than the tinyAVR one. The register is limited to -32 to +31, corresponding to - and + around 16%. +/-16% doesn't exactly open up new frontiers.
If an external 32.768 MHz crystal has been connected, you can, for improved temperature stability, autotune the internal oscillator from this. I had to direct a torch at the chip (from a distance) to heat it up enough to see the speed autotune....
Not the feature I'm most impressed with. A+ on concept, C on execution. More tuning bits to improve granularity and compliance (at least 8) would allow autotune to lock onto a frequency that was closer to the target. The biggest enemy of autotune is the granularity.

The EB-series is almost in the same boat... except it's also got that wild PLL to play with.

Tuning to achieve non-standard speeds is not supported on the AVR Ex-series parts under DxCore.

## Supported Clock Speeds
Like classic AVRs, tinyAVRs have a "speed grades" depending on the voltage and operating conditions that they are rated for operation within. The spec is 5 MHz @ 1.8V , 10 MHz @ 2.7V (3.3V nominal) and 20 @ 4.5V (5.0V nominal) (2.7 or 4.5 for 8MHz and 16 MHz at >105C . See the Speed Grade reference for more information on this. Note that the speed grades are extremely conservative for room temperature operation, and unreal overclocking is easily achievable at room temperaturee, particularly with high temp. rated parts.

The AVR Dx-series come in I and E  spec parts.

Since the DA-series was released, Microchip has STOPPED MARKING THE SPEED GRADE ON THE CHIPS. Be sure that if you have both speed grades, you mark the chips somehow (0 and 1-series parts do still have the temperature rating marked (N = 105, F = 125).

Some of the listed speeds, while supported by the hardware are not supported by the core - typically weird, slow clocks, particularly from a crystal
For unsupported speeds, the micros and delay-us columns indicate what internal plumbing has been implemented. micros is implemented for almost all speeds, delayMicroseconds with non-compile-time-known delays for most, even some unsupported ones. delayMicroseconds() is supported and accurate at all speeds when the argument is a compile-time-known constant, as we use the avr-libc implementation.

| Clock Speed | W/in Spec (tiny, Ex) | W/in Spec (Dx) | Internal (tiny) | Internal (Dx) |  Ext. Crystal |    Ext. Clock | micros | delay-us | Notes
|-------------|----------------------|----------------|-----------------|---------------|---------------|--------|----------|-------------
|       1 MHz |  (>1.8V) !       Yes |            Yes | Yes, 16/16 = 1  | Not by DxCore | Not by DxCore |    Yes |      Yes | 1
| *     2 MHz |  (>1.8V) !       Yes |            Yes | Yes, 16/8  = 2  | Not by DxCore | Not by DxCore |    Yes |      Yes | 2
| *     3 MHz |  (>1.8V) !       Yes |            Yes | Tuned 12/4 = 3  | Not by DxCore | Not by DxCore |    Yes |       No | 2
|       4 MHz |  (>1.8V) !       Yes |            Yes |             Yes | Yes           | Not by DxCore |    Yes |      Yes | 3
| *     5 MHz |  (>1.8V) !       Yes |            Yes |   Yes, 20/4 = 5 | Not by DxCore | Not by DxCore |    Yes |      Yes |
| *     6 MHz |  (>2.7V)         Yes |            Yes | Tuned 12/4 = 3  | Not by DxCore | Not by DxCore |    Yes |       No | 2, 4
| *     7 MHz |  (>2.7V)         Yes |            Yes | Not by DxCore | Not by DxCore | Not by DxCore |    Yes |      Yes | 2, 4
|       8 MHz |  (>2.7V)         Yes |            Yes |           Yes |           Yes |           Yes |    Yes |      Yes |
|      10 MHz |  (>2.7V) !       Yes |            Yes |     Prescaled |           Yes |           Yes |    Yes |      Yes |
|      12 MHz |  (>4.5V)         Yes |            Yes |           Yes |           Yes |           Yes |    Yes |      Yes |
| *    14 MHz |  (>4.5V)         Yes |            Yes | Not by DxCore | Not by DxCore | Not by DxCore |    Yes |      Yes | 2, 4
|      16 MHz |  (>4.5V)         Yes |            Yes |           Yes |           Yes |           Yes |    Yes |      Yes |
|      20 MHz |  (>4.5V) !       Yes |            Yes |           Yes |           Yes |           Yes |    Yes |      Yes |
|      24 MHz |                   No |            Yes |           Yes |           Yes |           Yes |    Yes |      Yes |
|      25 MHz |                   No | No, easy o/c   |           Yes |           Yes |           Yes |    Yes |      Yes | 5, 6
|      28 MHz |                   No | No, easy o/c   |           Yes |           Yes |           Yes |    Yes |      Yes | 5
|      30 MHz |                   No | No, easy o/c   |           Yes |           Yes |           Yes |    Yes |      Yes | 5, 7, 9
|      32 MHz |                   No | No, easy o/c   |           Yes |           Yes |           Yes |    Yes |      Yes | 5, 9
|      36 MHz |                      | No, some work  |            No |    Most chips |    Most Chips |    Yes |      Yes | 8
|      40 MHz |                -     | No, some work  |            No |    Most chips |    Most chips |    Yes |      Yes | 8
| *    44 MHz |                -     | No, some E's OK|            No | Not by DxCore | Not by DxCore |    Yes |      Yes | 2, 8
|      48 MHz |                -     | No, some E's OK|            No |  Some E-specs |   Some E-Spec |    Yes |      Yes | 8

Notes:
The tiny and Ex-series have the same clock speed grades, including the voltage dependence.

`*` This speed is not exposed by the core because no sound argument has been presented in favor of including these odd, low frequencies. The mechanics are mostly written, so should someone convince me that these are useful, they could be made available easily. **Note that a desire to reduce power consumption is NOT a sound argument!** Refer to the electrical characteristics table. While current at frequency is only comparable at constant voltage, enough data is available that you can plot the current consumption versus frequency. Doing this, we find that the graph is roughly linear, at least in the region of interest. So we ask - when we halve the processor speed, is the current less than half or more than half? The answer is "More than", on every AVR I have checked (which is essentially every modern AVR), and every classic AVR I checked (a more limited number of examples).

1. 1 MHz is often used for low power applications, and so is supported with the internal oscillator only. It is not supported with other clock sources. Crystal uses more power, and so doesn't make sense, and external clocks seem to be power hogs. Also refer to the Idd vs Vdd characteristics graphs: If you are CPU-bound, since a first order approximation of power vs frequency is a * F_CPU + b, where b is larger than sleeping power consumption
2. Unsupported because of low demand, not technical obstacle.
3. 4 MHz from internal is always the clock source at startup. Optiboot always runs at this speed.
4. This is not natively supported by the internal oscillator but could be generated by prescaling the internal oscillator at 12 or 28 MHz. Other possible frequencies that can be created through prescaling are not shown, only integer number of MHz is included in table.
5. This is an overclock that is likely to work on all parts as long as it is run at room temperature.
7. 30 MHz is notable because it is the highest overclocked option available for tinyAVR 0/1-series running from tuned internal oscillator.
8. This is an aggressive overclock, and cannot be expected to work on all parts. Best results can be had by using E-spec (extended temperature range) parts, and higher speeds will always be achieved with an external clock.
  *To ensure a good yield, place your order on a new moon, then purify the room prior to experimentation by burning a quantity of US or Euro zone currency*
9. a 0/1-series tinyAVR can normally run no fatster than 30 MHz on internal, thought the oscillator can exceed 32, the core isn't happy at that clock speed from the internal oscillator, But with an external 32 MHz clock and a 5.1-5.2Vpower supply with good decoupling F-spec 1-series will generally run at 32 on external at room temp, but not intyernal.. In general, at all tiumes, an external clock will givbe a slightly cleaner clock signal, and thus slightly higher maximum speed before things start visibly crapping out immediately (you want to back off more than a little bit from that before you use it for anything.)

General note: Signs you've overclocked too hard: Random resets, these show as having been software resets, even if you don't use a software reset anywhere. Usually rtesults of arithmetic and logical instructions get polluted with 0's that should be 1's. The same can happen with load/store. In principle, the hardware is free to do whatever it pleases when being run outside of it's operating range. These parts seem to be rather cooperative, though.

There are multiple ways to generate some of the lower frequencies from internal oscillator (do you prescale from higher frequency, or set the oscillator to the desired one? Suspect the latter is more power efficient, but with the former you could still use the PLL while staying in spec - (in my tests the PLL worked well beyond the spec in both directions, at least at room temperature, not that you'd want to do that in production) - currently, we set the main oscillator to the desired frequency, however we may revisit this decision in the future. There might be reasons to just run the TCD off the unprescaled clock in order to.... I'm not sure what....

The DA-series does not support use of an external high frequency crystal, only external clock or the internal oscillator can be used. The internal oscillator is pretty accurate, so internal clock will work fine for UART communication (they're within a fraction of a percent at room temp) with very little voltage dependence (they have an internal regulator to generate the core voltage, which runs at a much lower voltage, and I suspect that's where the internal oscillator is located.

## Auto-tuning
Autotuning is supported in hardware from a watch crystal on AVR Dx and Ex series parts, but not on tinyAVR. This is actually only for tuning (not overclocking).

## Overclocking
The capacity of these parts to run at speeds in excess of their rated maximum boggles the mind.

My "records":
48 MHz crystal on DB, E-spec part.
40 MHz crystal on DB, DD with I-spec parts. 32int x 4 = 128 MHz PLL clocking the TCD. When I switched to 40x4=160 MHz, the output started glitching something fierce. It looked to me as if it was finally unable to synchronize the double-buffered registers without breaking things - it looked almost okay if you didn't write new duty cycles...

I have tinyAVRs that might be stable at 36 internal (tiny2), but I haven't bothered to enable that on mTC.

Running a DA at 48 MHz (or a tiny at 32) is an ambitious overclock and is totally unnecessary - so obviously this is a very exciting chance to do something for that fundamental instintive reason: "Because I can"! And you can to!  So far, I had success at room temperature with external clocks, but not external crystals, and only when using the E-spec (extended temperature range) parts (which makes sense). I am surprised how they will "just work" at 40 from a crystal though, even I-spec parts usually do However, not all parts are capable of this. Out of around a dozen parts, I've so far found 1 that doesn't work at 40. As usual with AVRs, it's the ALU that starts failing first. I have an I-spec that, at 48 external clock will run a program that does a 1 second delay between calling micros (so lots of math is being done, then prints it). Like with over-overclocked tinies, if it's not so high that they crash immediately, they start getting math wrong, which shows up as 0's being printed for micros/millis(). This is unstable, as eventually a return address will get broken, it will return to that, and everything will fail (this may now turn into a reset if you're properly handling reset flags)

These can be returned to normal operation by uploading code that doesn't try to use an excessively fast clock - since the heat generated by the CPU core is negligible even at double it's rated clock speed, the damage that a desktop computer CPU might incur when overclocked too hard is not an issue.


## megaTinyCore

## Strange possibilities for divided clocks
There are a *lot* of strange clock speeds possible through combinations of prescalers and the internal oscillator - ever wanted to run an MCU at 7 MHz? *Me neither*, but you totally can, even without a crystal... These exotic speeds are not currently supported by DxCore. The switching logic to get that clock speed from the internal oscillator is implemented for the integer MHz speeds that can be achieved in that way. Fractional speeds are not implemented at all and total timekeeping failure should be expected. I'd be lying if I said I missed the struggle to make millis and micros accurate with weirdo clock speeds back on ATTinyCore (which in turn was done to support UART crystals, because some people are very picky about baud rate accuracy. You may laugh at their posts about making sure the clock is perfect for serial - but if you're not careful, on classic AVRs, the granularity of USART baud rates was horrendous near the higher speeds!) See also the [serial reference](Ref_Serial.md)
Like classic AVRs, tinyAVRs have a "speed grades" depending on the voltage and operating conditions that they are rated for operation within. The spec is 5 MHz @ 1.8V , 10 MHz @ 2.7V (3.3V nominal) and 20 @ 4.5V (5.0V nominal) (2.7 or 4.5

| Clock Speed | Within Spec | Internal | Ext.Clk |Tuned| 0/1 |Tuned|  2  | Notes |
|-------------|-------------|----------|---------|-----|-----|-----|-----|-------|
|       1 MHz |         Yes |      Yes |      No | Yes |Presc| Yes |Presc|       |
|       2 MHz |         Yes |      Yes |      No | Yes |Presc| Yes |Presc|       |
|    2.66 MHz |         Yes |       No |      No |  No | N/A | N/A | N/A | 2     |
|       3 MHz |         Yes |       No |      No | Yes |Presc| Yes |Presc| 3     |
|    3.33 MHz |         Yes |       No |      No |  No | N/A | N/A | N/A | 2     |
|       4 MHz |         Yes |      Yes |      No | Yes |Presc| Yes |Presc|       |
|       6 MHz |         Yes |       No |      No | Yes |Presc| Yes |Presc| 3     |
|       7 MHz |         Yes |       No |      No | Yes |Presc| Yes*|Presc| 3,4,9 |
|       8 MHz |         Yes |      Yes |     Yes | Yes |Presc| Yes |Presc|       |
|      10 MHz |         Yes |      Yes |     Yes | Yes |Presc| Yes |Presc| 5     |
|      12 MHz |         Yes |       No |     Yes | Yes | Yes | Yes |Yes* | 5     |
|      14 MHz |         Yes |       No |      No | Yes | Yes | Yes|  No | 4,9    |
|      16 MHz |         Yes |      Yes |     Yes | ofc | Yes | ofc | Yes | 6     |
|      20 MHz |         Yes |      Yes |     Yes | Yes | ofc | Yes | ofc | 6     |
|      24 MHz |          No |       No |     Yes | Yes | Yes | Yes | Yes | 7     |
|      25 MHz |          No |       No |     Yes | Yes | Yes | Yes | Yes | 7     |
|      28 MHz |          No |       No |     Yes |  No |  No |  No |  No | 4     |
|      30 MHz |          No |       No |     Yes |Maybe| Yes |Maybe| Yes | 5, 8  |
|      32 MHz |          No |       No |     Yes |  No | Yes |Maybe| Yes | 5, 8  |

Notes:
1. 2 MHz could be made by prescaling, ~but there is no demand~ and was added in 2.6.0
2. Optiboot always runs at 2.66 or 3.33 MHz (it does not unset the /6 prescaler). Depending on OSCCFG, it always starts up at one of these speeds
3. This speed could be generated by prescaling a tuned oscillator, but we don't support this because there is no convincing need.
4. This speed, or the speed that a tuned oscillator would need to run at to be prescaled to it, is an awkward unpopular frequency for which there is no demand.
5. Where marked with an asterisk, this speed is at the edge of what is possible with a tuned oscillator - not all chips' oscillators will get this high. In the case of the speeds at the low end, we make up for it by prescaling double that frequency if we have to; obviously that doesn't work for the high end of the range.
6. 20 MHz is well within tuning range of the 16 MHz oscillators and vice versa. This is particularly useful if using Optiboot and hence not setting clock on upload.
7. This is a mild overclock, and it will almost always work at room temperature and 5v.
8. This is an aggressive overclock. Best results can be had by using F-spec (extended temperature range) parts; solid 5v supply is a must. Higher speeds can generally be achieved with an external clock.
9. 14 and 7 MHz, for people who like making life harder for themselves, can be achieved via tuning, but for the 2-series, `OSCCFG` must be set for 16 MHz

megaTinyCore has no plans to add support for clock speeds that do not approximate an integer (ex, 1.25 MHz, 2.5 MHz - and certainly not, the technically achievable (20/48), but useless, 516 kHz. )

The tinyAVR product line does not support use of an external high frequency crystal, only external clock or the internal oscillator can be used. The internal oscillator is pretty accurate, so internal clock will work fine for UART communication()they're within a fraction of a percent at room temp) with very little voltage dependence.

## Tuning
The compliance of the tinyAVR 0/1/2-series oscillator blows the datasheet specifications out of the water and into low earth orbit. The difference between the speed with the oscillator frequency cal at it's minimum and at it's maximum values is about the magnitude of it's nominal frequency! 0/1-series parts usually go from a little under 5/8ths of their nominal frequency with tuning at 0 up to 1-5/8ths with tuning at the maximum! 2-series parts have a slightly wider range, centered a couple of MHz higher; a typical 2-series, at a nominal frequency of 20 MHz could be tuned from as low as 12 MHz up to around 36 or so, if you were to extrapolate the speed speed (they generally don't make it to the mid 30's).

A tuning sketch must be used to experimentally determine the values to use. See the [Tuning Reference](https://github.com/SpenceKonde/megaTinyCore/blob/master/megaavr/extras/Ref_Tuning.md). Tuning is a LOT more fun here than on the DX-series!

## External clocks
External clocks are always an option. The layout considerations are pretty simple: Keep the high frequency trace as short as possible. Don't connect anything else to the high frequency trace, make sure you follow the guidelines in their datasheet on decoupling capacitors. And if using one of the rectangular SMT ones, be careful to orient it correctly when placing the part. It fits just fine rotated 180 degrees, which will connect vcc and gnd backwards, instantly destroying the oscillator and presenting a near short to the power supply.

There are a lot of reasons, however, to not use them:
1. They are power hogs. 10-25 mA is normal, 50mA is not unheard-of.
2. They are picky about voltage. There was a line of oscillators in 5032 package in production until mid 2020 which worked at 1.8-5v. They were discontinued, and there is no other such oscillator available.
  a. The 5v ones specify minimum 4.5v maximum 5.5v.
  b. 1.6-3.6v ones are available, so the lower end of the range is covered. None of the major vendors with parametric search on their catalog indicate that any which are in spec operating between 3.6v and 4.5V, so one cannot run directly from a LiPo battery with one while remaining within spec.
3. 5v units are not available in packages smaller than 7050 (7mm x 5mm) typically, 5032 is exotic for 5v oscillators, and anything smaller nonexistent.
4. They are strangely expensive, typically $1 or more from western suppliers. There is hardly any "China discount", either. Like crystals, however, the oscillators advertised on Aliexpress don't list essential specs like the voltage or part number, and sellers don't seem to know when asked.
  a. One gets the impression that external oscillators are a specialty item for precision applications, while crystals are not. They are often designed to higher accuracy and they don't depend on external components that could "pull" the frequency, and so on. In a typical application, if you don't need a precision clock source, there are other ways to get it.


## Clock troubleshooting
On a classic AVR, the result of selecting a clock source (external crystal or clock) which does not function is not subtle: You burn the bootloader to set the fuse to use that clock source, and the chip ceases to respond, including all attempts to program, other than HJV programming. Fortunately, this is a thing of the past with modern AVRs, it comes for free with the new clock handling - it starts up at a universally acceptable speed and can be easily switched at runtime. Recovery is typically as simple as uploading a new sketch with more appropriate clock settings.

External Clocks are also fairly hard to mess up, too - they're nothing like crystals, which require you to calculate the value of loading capacitors based on numbers you don't know and don't have the equipment to measure, or more likely, try to guess pick a value that will be good enough, and be ready to swap out the loading caps in event that it's not.

### Failure modes
A number of behaviors have been observed when the clock has been configured wrong, is not functioning, or that particcular specimen can't do the overclocking you are asking of it.

#### Tuned Internal Oscillator
There are two general failure modes here. First, you may discover that it is operating, but not at the expected speed. Did you run the tuning sketch, or are you relying on the core to guess? The guessing is not 100% effective. If the speed is WAY off, as in, something like 2.66 MHz or 3.33 MHz, it has either been tuned but found not to reach the requested speed. This usually happens when trying to run parts at the highest supported tuned overclock, and not all oscillators are equal. Some skew low, others skew high. The ones that are lower than average can't hit the top speeds. Alternately, it may have been tuned, and in the process the tuning sketch detected that the chip was not stable at that speed and marked it as unreachable. In either of those cases, use a slower speed, or a different chip; the one you're using can't reach the speed you want.

If, instead, the speed is close to what you asked for, but off a few percent, I would suspect that the chip was not tuned properly and repeat the tuning process.

The other failure mode is that it does seem to run at the requested speed, but you notice that sketches hang over time, or occasionally output nonsensical numbers (typically 1's turn into 0's). This means that that chip is being run faster than it's capable of under the voltage and temperature conditions. Do you have a stable 5V supply that is well-decoupled? That is essential.
You may just have to use a different chip, (keep trying and use F-spec (high temp) parts. , as in the case above.

Considerations for Tuned oscillators are covered in greater depth in the [Tuning Reference](https://github.com/SpenceKonde/megaTinyCore/blob/master/megaavr/extras/Ref_Tuning.md).

### External clocks
The normal failure mode observed is that the chip just *stops* very rarely in the initialization process - it switches to the external clock, ceases to have a clock, and doesn't do anything more. There aren't many possible causes.
Unlike crystals, you can actually put a 'scope probe onto the output of a the oscillator and see the signal. Do that. Do you see the signal?
1. If not - examine the datasheet and try to figure out why it might not be oscillating.
  a. Verify that it is getting power and ground connected to the proper pins. (on my breakout board which support either a crtystal or external clock, you need to complete a solder bridge to  connect it to Vcc, so you can also use it with a crystal)/
  b. Most have a pin that can be pulled low to turn off the output to save power. If allowed to float, they generally have internal pullups and can be left alone, but putting a 10k pullup to Vcc is an easy test to verify. Is yours shorted to ground by any chance?
  c. You didn't install it rotated 180 degrees did you? the rectangular oscillators usually have Vcc and ground opposite each other, so everything will be lined up and can be soldered down with the oscillator connected backwards. This will irrevocably destroy it within seconds. You can easily determine if this has happened - the part will be hot to the touch,
2. If you do see a signal, and it is no higher than 24 MHz (ie, there's no chance it's actually too fast for the chip), make sure you see the same signal on the pin of the chip (cold solder joints happen - I've seen parts with a pin that wasn't actually soldered in place, but which looked like it was, and when pressure was applied to the pin with the scope probe - it started working; this is much more frustrating when you're trying to debug it with the power off with a DMM, and you check every connection, yup, continuity.... but when you remove the probe and connect power, it no longer makes contact )
