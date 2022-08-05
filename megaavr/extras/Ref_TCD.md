# analogWrite and configuring TCD
There is limited support for manipulating TCD0 in specific ways, using these procedures, while not breaking analogWrite. Beware - if you stray from this path, you will sink into the quicksand and you'll be lost forever...

## Background
TCD not a friendly timer. It is one of the most complex and byzantine peripherals on any modern AVR (and will likely remain so, outside of the usb interface thaqt the DU's will have). Unlike TCA, where someone trying to do anything other than chanege the prescaler is almost guaranteed to cause analogWrite() to misbehaver, for the TCD pins, we try to support a bit of a wider range because it's sufficiently complicated that few users will take over the type D timer. This is a 12-bit asynchronous timer. The async part is why it has to be so strange - there's a synchronization propcess which can take a few clock cycles to propagate certqin changes: you need to monitor the status flags because they will sometimes cause writes to be ignored. It also can be clocked from things faster than the system clock: The unprescaled internal oscillator (rarely relevant here, more relevant on megaTinyCore), a PLL at 2x, 3x or 4x an input frequency (which can be either system clock, or the external clock source (note: initial silicon has a bug, and that option only works for external clocks, not external crystals)), an unmultiplied external clock source or the system clock. Unlike classic AVRs where the frequency that you clock the "timer1" peripheral from must not exceed half the clock speed, here only the top frequency is limited, and it far suprpasses the spec'ed upper limit at room temperature.

In addition to the synchronization, some of the intentional features make it challenging to work with, namely that it seems to be dewignd for pins where the slightest misconfiguration will cause catastrophic failure, so they are very concerned with fault detection ansd preventing pins from bweing accidentally enabled unintentionally. Not only is it enable-locked, it also requires `_PROTECTED_WRITE()` to write to, which produces a timed write procedure required for reconfiguring such critical things as the clock system... And the TCD output pins. You can turn the timer off without the timed write. But you cannot turn pins on or off without it, nor set their "default state", which is what value the pins have when the fault detection events are used to turn off the output.

## Normal analogWrite operation
`TCD0` provides two PWM channels: `WOA` can output on PA4 or PA6, `WOB` on PA5, PA7. Those channels can each drive either - or both - of those pins, but only at one duty cycle. Users may prefer to configure this manually - `TCD0` is capable of, among other things, generating much higher frequency PWM, as it can be clocked from the PLL at 48MHz (or more, if you don't mind exceeding the specified operating ratings - I've gotten it up to 128 MHz, allowing 8-bit pwm at 500 kHz, or a 64 MHz squarewave). It is supposed to be remappable to other sets of pins. This doesn't work in current silicon, but future silicon revs will fix this - in all cases, the core uses channel A for the 2 even numbered pins, and channel B for the two odd numbered pins (if you take it over, the two higher nubmered pins can be set to either one, we only support one option through analogWrite(). If you try to turn on third PWM pin (or both even or both odd numbered ones), the other pin controlled by that channel will also change duty cycle - you can only get two proper PWM signals out at a time. digitalWrite() the other one first, in the unlikely evenrt that you want to PWM different pins at different times in the program. (I've never wanted to do this, nor has anyone else I talked to), when I didn't also need them to output different durty signals at the same time, which is not possible.
```c++
analogWrite(PIN_PA4,128); // 50% PA4. - like usual
analogWrite(PIN_PA7,192); // 50% PA4, 75% PA7 - like usual
analogWrite(PIN_PA6, 64); // 25% PA5, 25% PA6, 75% PA7 - PA4 and PA6 are both channel A
//use digitalWrite to turn off the PWM on a pin
// or call turnOffPwm(pin);
```

## Permitted settings
You can do the following things and continue using analogWrite and expect it to keep behaving normallly. Do not expect anything else to work - if you need to do more, you need to take it over entirely.

### You can change frequency by multiplying the period by up to a 2, 4, 8, or 16

The `TCD0` prescaler is less flexible than one might want it to be, at least if you're directly driving big MOSFETs with it.. We set `TOP` and double or quadruple (even octuple, at high clocks) the nominal `TOP` of 254 to lower the frequency, trying to minimize the sync prescaler and ensure PWM is not above 2 kHz because that is too fast to directly drive most large MOSFETs See my [MOSFET guide](https://github.com/SpenceKonde/ProductInfo/blob/master/MOSFETs/Guide.md)). This is detected dynamically (Though existing dutycycles won't be adjusted; be sure you turn PWM off, or update it by calling analogWrite(). There are considerable restrictions on what the core will support. The below examples explain it). Accepted values by the core are only 254, 509, 1019, or (if running at 32 MHz or higher ) 2039. This is not immwediately synced, you need to ask for it to sync; That's good - because you may have PWM coming out of one or both pins already, and you wouldn't want the duty cycle to suddenly change. Use this as shown below - analogWritw() will write one of the twop channels for you and trigger the sync, but if you have PWM coming out of the other timer, and you don't want the duty cycle to change, you need to set that prior to calling analogWrite.

```c++
/* Example 1: Both channels either off, 100% or 0% - this is appropriate; */
TCD0.CMPBCLR = 1019;
while (!(TCD0.STATUS & TCD_CMDRDY_bm)); //make sure it's ready to accept command
TCD0.CTRLE=TCD_SYNCEOC_bm; //note that analogWrite on any TCD0 pin will do it too.

/* Example 2: one PWM channel enabled - assume we are starting from TOP = 254 */
analogWrite(PIN_PA4,128);
TCD0.CMPBCLR = 1019;

//while (!(TCD0.STATUS & TCD_CMDRDY_bm));
//TCD0.CTRLE=TCD_SYNCEOC_bm;              // suddenly, duty cycle on PA4 would go from 50% to 126% to 12.5% - or 87.5% - one of the channels.
analogWrite(PIN_PA4,128);   // Doing this to referesh the duty cycle AND trigger the update solves it

/* Example 3: one PWM channel enabled - we want the other one on, and we don't care whether the new frequency starts 1 cyc;e before the the second channel comes on: */
analogWrite(PIN_PA4,128);
TCD0.CMPBCLR = 1019;
//analogWrite(PIN_PA5,64);   // This would mess up the PWM on PA4 if we called it first -
analogWrite(PIN_PA4,128);    // but we can call it on the currently active pin to fix the duty cycle, and then apply new frequency.
analogWrite(PIN_PA5,64);     // This line woks as Works as expected now.

/* Example 4: one PWM channel enabled - but we want to write the other one and have it take effect at the *same* time that the new ferequency does */
analogWrite(PIN_PA4,128);
TCD0.CMPBCLR = 1019;
//analogWrite(PIN_PA5,64);   // This would mess up thePWM on PA4 if we called it now.
//if we want to turn them on simpultaneously with the frequency change we have to manually adjust the one that we're not analogWriting():
TCD0.CMPASET = (TCD0.CMPASET << 2) + 3; // This prevents that, so now it's safe to write to the other channel right after changing period:
analogWrite(PIN_PA5,64);     // Works as expected now.

/* The rule is if you increase period by a factor of 2^n
 *  and PWM is coming out of a WOA pin (even numbered TCD pins)
 *    you can leftshift CMPASET n bits and add (2^n-1), and then then analogWrite a pin on WOB (odd numbered) channel.
 *  Or if PWM is coming out of a WOB pin (odd numbered TCD pins)
 *    you can leftshift CMPbSET n bits and add (2^n-1), and then then analogWrite a pin on WOA (odd numbered) channel.
 * When you're decreasing the period, instead rightshift the values and do not add add or subtract anything (they integer math away).
 * This is ONLY  supported for the speeds listed - NO OTHER VALUES SHOULD BE WRITTEN TO CMBBCLR IF YOU WANT TO USE analogWrite()
 *   In all other cases, you must take full control with takeOverTCD0().
 *   This is one of very few times that the core lets you keep using the API calls aftwr reconfiguring a peripheral.
 *    There is too much overrhead to handle if we try to handle the general case, but there are good reasons for letting
 *    users shift the freequency of this one to make different tradeoffs. The implementation stays simple as long as specific values were used.
 *    and the overhead was tiny. Doing anything more complicated, the overhead shoots through the roof and that's what taking over the timer is for.
 *    but this way, you can nudge the timer frequency up and down by factors of 2 to get it in an acceptable range for what you're PWMing
 *    (ex, big FETs which often need a driver above 1 kHz or vs, say, a FET driver where you probably want to PWM as fast as you can, within limits).
 *  Notes:
 *    1. If one of the channesls is not outputting PWM from TCD0 and one is, nothing weird happens until you write the second PWM chanenl with analogWrite.
      2. If you do not change the period, nothing weird happens.
 *    3.  Outputting 0% or 100% duty cycle doesn't count, even if it' still connected to the compare channel to prevent PWM glitches d when channels are toggled.
 *        It will keep doing that, no matter what you do to the period.
 *    4. Turning on another channel will always cause a glitch if it isn't already connected to the timer. But you can do that first with analogWrite(pin,0).
 *    5. Be careful about interrupt safety if you reconfigure TCD0 in interrupts.
 *    6. Don't do ANY of this if TCD0 is used for millis!
 */
```
### You can "stage" pins at constant levels so they won't put a glitch on the timer when you first write to them
digitalWrite() or turnOffPWM() will turn them off.
Setting the compare match higher than TOP keeps it from ever changing state (setting it to 0 does not, like many timers). But the solution is simple: Get the constant output, and invert the pin if needed. Use digitalWrite() to take control back from the timer (direct port manipulation and fast writes won't work on it until then)

```c

/* Example 6: connect two channels, set duty cycle. Subsequent changes to frequency  behave as above, but there is no glitch like there otherwise would be.  */
// Assume PA6, PA7 are not outputting PWM
// We can set thsee pins to 0% duty cycle, Either way is safe to change period, because are not outputting PWM, just constant level.
analogWrite(PIN_PA4,0);
analogWrite(PIN_PA5,255); // high and low levels behave the same.
TCD0.CMPBCLR = 1019;       // increasee period.
analogWrite(PIN_PA5,64);   // This can't make anything weird happen, no other PWM coming out.
delay(5000);  //we later want to increase frequency, we can do this:
TCD0.CMPBCLR = 509;
analogWrite(PIN_PA5,64);   // Still just one active channel, everything normal.
analogWrite(PIN_PA4,90);   // Now both channels are lit up, and we need to use care when changing thr TOP:
TCD0.CMPBCLR = 254;        // halved the period, so
TCD0.CMPASET = TCD0.CMPASET >> 1; // halve the compare value of one timer,and  analogWrite the other.
analogWrite(PIN_PA5,64);   // this applies all of these settings.
delay(5000)
TCD0.CMPBCLR=1019;         // Finally, you can also do it manually:
TCD0.CMPBSET = TCD0.CMPBSET >> 1;
TCD0.CMPBSET = TCD0.CMPBSET >> 1;
while (!(TCD0.STATUS & TCD_CMDRDY_bm)); //make sure it's ready to accept command
TCD0.CTRLE=TCD_SYNCEOC_bm; //will be applied at the end of the current PWM cycle.

```
### You can change frequency, clock source, prescaler

```c

/* Example 7: Changing prescaler - will cause brief glitch as you have to enable-cycle timer. */

/* Calculate desired value by ORing one CLKSEL, one CNTPRES, and one SYNCPRES.
 *  TCD_CLKSEL_OSCHF_gc         TCD_CNTPRES_DIV1_gc     TCD_SYNCPRES_DIV1_gc
 *  TCD_CLKSEL_PLL_gc           TCD_CNTPRES_DIV4_gc     TCD_SYNCPRES_DIV2_gc
 *  TCD_CLKSEL_EXTCLK_gc        TCD_CNTPRES_DIV32_gc    TCD_SYNCPRES_DIV4_gc
 *  TCD_CLKSEL_CLKPER_gc                                TCD_SYNCPRES_DIV8_gc
 * Sync Prescale effects everything related to TCD, including how long commands take....  (so you want to keep it low, if it's not impractical to do)
 * Prescaled sync clock is preacaled to by Count prescaler to get the signal that actually clocks the counter.
 * OSCHF is internal oscillator before any prescaling.
 * PLL must be enabled or it won't do anything.
 * This will set it to count at 1/128th of the system clock (32 * 4); If TOP is 254, 128*255 = ~32640 tickss of the system clock per cycle. About 490 Hz with 16 MHz system clock.:
 */
TCD0.CTRLA = 0;                         // Stop the timer.
while (!(TCD0.STATUS & TCD_ENRDY_bm));  // It needs to synchronize before you can restart it.
                                        // NOW you can write the new value you want.
TCD0.CTRLA = (TCD_CLKSEL_CLKPER_gc | TCD_CNTPRES_DIV32_gc | TCD_SYNCPRES_DIV4_gc | TCD_ENABLE_bm);

/* Altering the clock source like this will not change how analogWrite() works, just the frequency (as long as the clock you selected is present). */

/* You can use the events if you can figurre out how to use them, but most don't make sense unless you've taken over the timer.
 * Simple ones like mode 4 (turn off all output while event is HIGH) can be used, and the API functions will generally bumble along unaware
 */
```
### And you can use the programmed (delayed) event
For example, to make a third passable PWM pin by using the event system and a CCL with programmed event to get a third 8-bit PWM channel - [see the library examples](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/libraries/Logic/examples/TCDThirdPWM)

## Otherwise, you must take over TCD and manually configure everything
`takeOverTCD0()` will tell the core that you are assuming full responsibility for TCD0. It will ignore everything tjhat would normally make it reconfigure this timeer. This will make analogWrite() and digitalWrite() act like it doesn't exist when configuringing pins.
`init_TCD0()` is called during init to configure the timer. You can override it with an empty function to prevent this. If you do this, you must call `takeOverTCD0()` before analogWrite() or digitalWrite() is used.
You can do all kinds of crazy things with this timer! It can dither it's output for 16x more accurate frequencies, there are 11 different things it can do on eventts, you can set the default state of the compare outputs.... With some persistence you can even capture the current count!

### What nest? Study the TCD0 chapter of the datasheet
The rest is beyond the scope of the core's documentation.

### Yes, 4-year old parts are growing new errata
Like I said, not many people were getting too deep on these timers. A lot of bugs in the weird features have been turning up on the Dx-series and then found on tinyAVRs, because the TCD PLL makes this into a very different and more tempting peripheral to use.
