# Servo.h - A megaTinyCore compatible Servo library for users without Servo installed in Libraries
The copy of Servo from library manager is incompatible with these parts. We include a Servo_DxCore.h too in case you've installed Servo into the library folder - that one (which does not work) will be used in preference to the one we supply which does work. The workaround is to supply a second copy of the library, entirely identical except for references to this issue in comments and this paragraph.


## Basics
This implements the API described in [the Arduino documentation](https://www.arduino.cc/reference/en/libraries/servo).

* `Servo` - Class for manipulating servo motors connected to Arduino pins.

**Methods of Servo**

* `attach(uint8_t pin)`               - Attaches this instance of Servo to specified I/O pin.
* `attach(uint8_t pin, uint16_t min, uint16_t max)` - As above, but change the min and max pulse lengths(values in microseconds, default min is 544, max is 2400)
* `write(uint16_t angle)`             - Sets the servo angle in degrees.  (invalid angle that is valid as pulse in microseconds is treated as microseconds)
* `writeMicroseconds(uint16_t us)`    - Sets the servo pulse width in microseconds.
* `read()`                            - Gets the last written servo pulse width as an angle between 0 and 180.
* `readMicroseconds()`                - Gets the last written servo pulse width in microseconds. (was read_us() in first release)
* `attached()`                        - Returns true if this instance of Servo is attached to a pin.
* `detach()`                          - Stop outputting pulses to this servo and remove it's affiliation with the pin.


This library supports up to 12 servos controlled by one timer. It does not recruit additional timers. Who the hell runs over a dozen servos from one board? Please create an issue if this limit is problematic.

## Our logic for choosing the timer
* If SERVO_USE_TIMERBn is defined (where n is an integer between 0 and the number of the highest TCB), that timer will be used, or an error will be generated at compile time informing you that the timer is not present or used for millis.
  * Warning: Actually defining SERVO_USE_TIMERBn is not as easy as you would think - you can't just put that in your sketch; then sure, SERVO_USE_TIMERBx will be defined in the compilation unit derived from the .ino file and main.cpp. But the compilation unit that needs to know that is the one derived from Servo/Servo_DxCore.cpp. That one can't see any macros defined in your sketch. Since people complain about the number of tools menus, I'm disinclined to add one for it, but that's the sort of thing it takes, because you need to get the commandline parameter -DSERVO_USE_TIMERBx passed along with all the other compiler flags, so that it will be visible to all compilation units. To do that, either you need to manually compile through some third party tool (not my department), or modify boards.txt or platform.txt to add that parameter, OR instead, cheat and add the #define to the top of ServoTimers.h - just remember than you did it.
* Otherwise (which is usually the case), we try to pick a sensible timer For **Dx/Ex** this looks like.
  * If the part is a **48 or 64 pin part** with a TCB3 (these all have a second TCA as well), the default that the core chooses (see the part specific docs for your part), for TCA PWM is ports B and C for TCA1 and 0 respectively. These render TCB2 and TCB3 useless for PWM - both of the pins that these can be set to use overlap with those TCAs. That makes these timers the least useful. TCB2 is used for millis by default. So if millis hasn't been moved to TCB3, we use TCB3.
  * We do not use TCB2 if millis was moved somewhere else (as we think the chance is significant that this was done to free up TCB2 because they had some other inflexible library that only used TCB2)
  * If the part has **fewer than 32 pins**, or doesn't have a TCB3, we use TCB1 if it exists and isn't used for millis, otherwise, we use TCB0, unless it's used be millis.
  * Millis can only use one timer, and all DxCore parts have more than one TCB, so you can't possibly have both of them in use by millis, so it should always successfully find a timer, as long as nothing else needs a timer (see the section after the tinyAVRs for more on that); the case where it does not find a a TCB1 and millis is on TCB0, which is the only case where we can catch the problem and issue our own "you're out of timers" error.
* On tinyAVR the rules are the same, but the ramifications are different. There's no TCB PWM. Instead, your problem is that you have only 1 or 2 TCBs, and you're concerned about running out of timers.
  * The **tinyAVR 2-series** has 2 type B timers, and uses one by default for millis (though the TCA can be used instead if a second TCB is needed), so here too it will always find a timer.
  * The **tinyAVR 1-series** has 2 type B timers at 16k+, and 1 below that, though by default they use the TCD for millis (it's objectively a markedly worse millisecond timer than TCB, or even the TCA (both TCD and TCA can output PWM and do millis) - the real reason it's the default there is that writing code to use it is absolutely awful, difficult enough that I expect to see very little code that directly manipulates it, compared to TCA and TCB, therefore, TCD should be the default on the timer-starved tinyAVRs). That in turn leaves the TCB(s) open for libraries like Servo, and the TCA open for generating PWM at different frequencies (TCA is much more cooperative than TCD, familiar to people who used classic AVR timers, and - based on inquiries and discussions - manipulating it to change the frequency or generate high res PWM is widespread).
  * **The unfortunate 0-series**, which had around half of it's peripherals chopped off, yet has a selling price just pennies below the 1-series (wtf are you doing buying that crap? You save almost nothing and get a part that's much less capable!), if you for some reason end up stuck with these, have a TCB and a TCA and an RTC (no xtal option, and buggy, like the 1-series and mega0), millis defaults to the TCA to keep the one TCB open.
* Note that in searching for timers to use, Servo currently checks in the order TCB3 (if 48+pin), TCB1, TCB0. tone, the other builtin functionality that uses a TCB, picks the *lowest* numbered TCB that isn't used for millis. Thus, to use Servo and tone() in the same sketch, millis timekeeping must not be on TCB0 or TCB1, unless you have a TCB3, which Servo will use if it can, and then millis can use TCB0 or 1, and tone and servo will also play well together (but if you have a TCB3, you have a TCB2, so why are you not leaving millis there?).
* TCB-using libraries generally can never share the peripheral, even if they don't use it at the same time. Almost everything involving TCBs involves the TCB interrupt, so they both have to define an ISR, resulting in two ISR definitions. A program with multiple definitions of a function, much less a vector, is of course malformed and will not compile (duplicate vector).
* Note that timer scarcity is relevant, in descending order, to tinyAVR 0-series (1A, 1B, RTC), future AVR DU-series (1A, 2B *rtc* and more features to use it), AVR DD-series w/14-20 pins (TCA, *1d*, 2B, *rtc*) and tinyAVR 2-series (1A, 2B, RTC). Timers showns in *lowercase italics* are ones that their core does not offer for millis (that is, TCD or RTC on Dx-series), so you can see how tight some of those are when you want millis, and servo, and then need another timer for something else.
* The above timer priority information is most useful to people trying to get multiple timer using libraries to cooperate.


## Differences from stock library
In addition to supporting all modern AVRs and the differently named versions, there are a small number of subtle differences.
* As with the stock version, no PWM from that timer.
* Unlike the stock timer, we do not depend on a clock from a TCA.
* Like all servo libraries, we use interrupts extensively; each pin transition results in at least one TCB interrupt firing, possibly more than one.


## Note for alternative servo mechanisms
There are multiple ways to generate Servo pulses with TCA or TCD and have the last written value repeated every 20ms without CPU intervention. I do not know of any such libraries. but they are conceptually simple, and very graceful - their main disadvantage is that the number of servos they control doesn't look that great next to the TCA. The idea would be to use a TCA in 3-channel 16-bit mode, or a TCD. Relevant times are from about 500us to 2500 us. But we need TOP at 20000us.
So we `takeOverTCA0()` so we have a clean timer in single mode. What would the period need to be for TOP = 20,000 us?

Say we're clocked at 24 MHz, so 20,000 us = 480,000. That is a little larger that 65535.

Prescale the timer by 8, and you get 60000 ticks per servo cycle. So PER = 59999. Very conveniently, this also means 3 ticks = 1 us (much less awkward than if it were a non-integer), so the range would be CMP0/1/2 = 1500 to 7500. The catch of course, is that you only get 3 channels from a TCA.

You get can get 2 more from a TCD but what about the period? we only have 4096 ticks in a TCD cycle, so we would need each tick to be ~ 5us, That's 120 clk, can we manage a 128x prescale over there? 1/32 * 1/4 = 1/128. Yes! But with a duty cycle of just 1/40 to 5/40, the compare value will only range from 0 to 1/8th, so at most you have log<sub>2</sub>((2^12 / 2^3)) = log<sub>2</sub>(2^9) = 9 bits of precision - except that you don't get the whole range, because the lower 1/5th of that is also never generated, so interms of bits of information communicated, setting the 12-bit compare register, with the default range you have at most 1/10th of the code points that actually can encode a meaningful value. So log<sub>2</sub>(409.6) bits of meaningful information can be communicated in the background using a TCD, or one more bit in the two ramp configuration vs log<sub>2</sub>(2000|4000|5000|6000) bits of meaningful information via a TCA.

The TCA configs might for example look like (disclaimer: This is pure theory - I think it should be doable, but I certainly haven't done it). No idea if there's any call for that.

|  MHz |  F_CPU   | clk/ms|clk/servo|Psc|Counts |  TOP  |tick/us|min | max  | Effective res. |
|------|----------|-------|--------|----|-------|-------|-----|------|------|----------------|
|   48 | 48000000 | 48000 | 960000 | 16 | 60000 | 59999 | 3   | 1632 | 7632 | 12.55b         |
|   40 | 40000000 | 40000 | 800000 | 16 | 50000 | 49999 | 2.5 | 1360 | 6360 | 12.28b         |
|   36 | 36000000 | 36000 | 720000 | 16 | 45000 | 44999 | 2.25| 1224 | 5724 | 12.13b         |
|   32 | 32000000 | 32000 | 640000 | 16 | 40000 | 39999 | 2   | 1088 | 5088 | 11.96b         |
|   30 | 30000000 | 30000 | 600000 | 16 | 37500 | 37499 | 1.88| 1020 | 4770 | 11.87b         |
|   28 | 28000000 | 28000 | 560000 | 16 | 35000 | 34999 | 1.75| 952  | 4452 | 11.77b         |
|   24 | 24000000 | 24000 | 480000 |  8 | 60000 | 59999 | 3   | 1632 | 7632 | 12.55b         |
|   20 | 20000000 | 20000 | 400000 |  8 | 50000 | 49999 | 2.5 | 1360 | 6360 | 12.28b         |
|   16 | 16000000 | 16000 | 320000 |  8 | 40000 | 39999 | 2   | 1088 | 5088 | 11.96b         |
|   12 | 12000000 | 12000 | 240000 |  4 | 60000 | 59999 | 3   | 1632 | 7632 | 12.55b         |
|   10 | 10000000 | 10000 | 200000 |  4 | 50000 | 49999 | 2.5 | 1360 | 6360 | 12.28b         |
|    4 |  4000000 |  4000 |  80000 |  2 | 40000 | 39999 | 2   | 1088 | 5088 | 11.96b         |
|    1 |  1000000 |  1000 |  20000 |  1 | 20000 | 19999 | 1   | 544  | 2544 | 10.96b         |
|26.214| 26214000 | 26214 | 524280 |  8 | 65535 | 65534 | 3.27| 1781 | 8335 | 12.67b theoretical limit on precision for servo control on TCA |


The last entry is a magic clock speed that with the right prescaler, lands right at 65535 ticks to run through 20 ms, which is the point where this method gives the most precise output (ie, for a TCA, that's the upper limiting value. As you can see, you'd come very close to this naturally. Even at 1 MHz, usually the bane of Servo implementations this scheme should be fine.

Always use the CMPnBUF registers to write to ensure you don't get any glitched pulses.

For TCD, it might look a bit more like:
| MHz|  F_CPU   | clk/ms|clk/servo|Pscl|Counts| TOP  | tick/us | us/tick | min |  max  | Effective res. |
|----|----------|-------|--------|----|-------|------|---------|---------|-----|-------|----------------|
| 48 | 48000000 | 48000 | 960000 | 256 | 3750 | 3749 | 0.1875  |   5.333 | 102 | 477   | 8.55b          |
| 40 | 40000000 | 40000 | 800000 | 256 | 3125 | 3124 | 0.15625 |   6.4   | 85  | 397.5 | 8.28b          |
| 24 | 24000000 | 24000 | 480000 | 128 | 3750 | 3749 | 0.1875  |   5.333 | 102 | 477   | 8.55b          |
| 16 | 16000000 | 16000 | 320000 | 128 | 2500 | 2499 | 0.125   |   8     | 68  | 318   | 7.96b          |
| 10 | 10000000 | 10000 | 200000 |  64 | 3125 | 3124 | 0.15625 |   6.4   | 85  | 397.5 | 8.28b          |
|  4 |  4000000 |  4000 |  80000 |  32 | 2500 | 2499 | 0.125   |   8     | 68  | 318   | 7.96b          |
|  1 |  1000000 |  1000 |  20000 |   8 | 2500 | 2499 | 0.125   |   8     | 68  | 318   | 7.96b          |
| 24 | 24000000 | 24000 | 480000 |  64 | 7500 | 3749 | 0.375   |   2.667 | 204 | 954   | 9.55b 2-ramps  |
|  - | Arbitrary|     - |      - |   - | 4096 | 4095 | -       |   -     | 103 | 511   | 8.67b best possible w/TCD PWM single ramp.
|  - | Arbitrary|     - |      - |   - | 4096 | 4095 | -       |   -     | 207 | 1023  | 9.67b best possible w/TCD PWM (2 ramp)



Notice the difference in the min and max values - much smaller here! This matters, as you can only write (2^effective res) distinct valid values into the CMPn register, and sometimes there are  TCA is a far better timer to use for this for most purposes, because you get far more effective resolution. If you look at the TCD capabilities, best case, you've got the range of 544-2544 us containing 375 code points. That's a little thin. Some have just 250. On the other hand, TCA's worst numbers are at 1 MHz, and it still gets 2000 codepoints there, and most of the time it gets far more. How precise does it have to be before you're at the limit of the servo's input? People talk about values like 1-5 degrees precision; that suggests that 180 code points should be plenty, but I think that may be a bit of an oversimplification.


The general opinion seems to be that most of the inaccuracy is in the mechanics and gearing - there's always going to be a little play in a set of gears that don't take extreme measures against it. So, in applications where this is reasonable and practical, switching to a closed loop control scheme, where you are measuring the location of the thing the servo moves, so that if the servo doesn't put it where you wanted it, you can correct with a small nudge to the pulse length in the appropriate direction (I suspect that it would be reasonably easy to, when in debug mode, dump tons of data like this, then go through it with data analysis and come up with a much better model that predicts the position based on current and recent historical pulse lengths and the positions measured at those times, and thus lets you more accurately pick pulse lengths as you accumulate more data for a servo. If'n you do that, you should probably a) number the servos and b) in your data, also note the system clock speed (measured, not what it was set to) if using internal clock - it's usually within a %, less often a bit more, at room temp!) and it allows to get far higher accuracy. It's a lot easier to move something into a precise position if you're watching exactly where it is while doing so than if you're blindfolded. Even more so if you're in the computer's position: Blindfolded, the hand holding the object completely numbed with novocaine and the other hand tiedbehind your back, earplugs in and corks in your nostrils. Servos are used in many systems with no feedback, but you can get much more precise results with feedback.

A final note on that last line - there's a trick with TCD to get twice the TOP as implied there. The way you do this is simple: Set it to 2-ramp mode, CMPACLR = CMPBCLR = (TOP<sub>table, final line</sub>)/2). This will result in the two pulses being 180 degrees out of sync, and each cycle includes a count from 0 to CMPACLR and then 0 to CMPBCLR. So if both of those are set to TOP, then, and the compare is set for the right pulse length in both cases, you'll get alternating pulses out on the two pins, 1 pulse every 10 ms in total, and 1 for a given channel every 20. And that can happen with no CPU intervention.

At that point most of the steps to adapt the library look straightforward. The only problem being the pesky low number ofchannels you can do like this compared to normal PWM, 3 servos per TCA is all you could get with this weird method.

The point of this would be that the servoing would all be done completely without CPU intervention, leaving it to more important tasks, like outputting WS2812B signals to the lights you wrapped around the things your servos are moving while the servos are moving.
