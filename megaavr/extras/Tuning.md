# Internal oscillators and Tuning on tinyAVR devices
All of the 0/1/2-series tinyAVR devices have an internal high-frequency oscillator which is generally used as the main oscillator; it's the only option you have other than and extermnal clock, or running from the 32kHz internal oscillator (which makes execution of any code painfully slow and is mnot supported by megaTinyCore - running anything other than very carefully written straight C making use of no Arduiino API functions is hopelessly slow, whicih is why we don't support it).

## **THIS IS ONLY FOR TINYAVR**
There are many good reasons not to use it on the Dx-series; the factory cal is pretty much dead on, and you can crank it up to 32 MHz without doing anything with the cal. I certainly have no plans to support tuning over there. I also would be disinclined towards tuning support if I could just throw on a crystal. As is shown clearly in the graphs in the latest Dx-series datasheet, you generally don't even get +/-10% from tuning, so it's not particularly exciting.

## Don't use chips with extreme tuning for safety critical tasks
I know I say this all over the place, but most of the speeds enabled by tuning are overclocks. While these chips are rated for safety critical applications, neither the Arduino libraries, nor anything related to this core are. Parts used outside their specified operating range may malfunction with unpredictable results. Almost any arbitrary result is possible, from crashes to incorrect results for arithmetic and logic, to making demons fly out of your nostrisls. That is obviously not acceptable for safety critical aoplicatoins,

## General Concepts
As usual for AVR parts, the internal oscillator's compliance covers a far wider range than the datasheet describes. In my testing on 1-series parts, it appeared that both the 16 MHz and 20 MHz oscillator can be calibrated to as low as approximately 5/8 of their nominal speed or as high as 1-5/8 of their nominal speed The oscillator on the tinyAVR 2-series parts has gotten an upgrade in the form of the calibration register getting an additional bit - it now has 7 bit granularity instead of 6! (the high bit indicates whether the center frequency is 16 or 20 MHz). The 128 settings cover a similarly sized range, of speeds, though both the top and bottom ends of this range are higher. The 2-series parts also reach higher speeds beforethey start to fail.

Knowing this, you could make the internal oscillator run at whatever frequency between those you wanted if you but know the right OSCCAL setting. This has a small depemndnce on voltage and temperature, but it os far less dramatic than the classic AVRs, and it's different on different individual specimens. I have one t841 that's a whole 0x10 different in factory cal, which is damned rare. You will generally find oscillator speed within range of

This section is about how to "tune" a chip and automatically store those magic OSCCAL constants in the USERROW - the one section of memory which will survive burn bootloader, as well as any action other than an active attemopt to delete it or a chip erase on a locked chip. Doing this will make the "tuned" options far more accuratr.

## Why not an external clock
External clocks are great, with very accurate timing, better overclocking (the 1-series parts run at 32 MHz like that), etc. but it's and extra part - oh - and it needs it's own 0.01uf decoupling capacitor so 2 extra parts to place... and ordered from the cheapest chinese electronics vendors on aliexpress, prices bottom out at 50 cents or so, all told, and if you want to know the what the specs are like , you're going to end uo paying more for the oscillatoer than the chip it's clocking. Amd on  top of all that they take up a pin (Note that the tinyAVR 0/1/2-series Rev. C boards from my Tindie shop will cover this!)

If you don't need *perfect* clock accuracy, but do need a significiantly different (probably higher) clock speed for whatever reason ("my sketch is running slowly" is not a good reason to overclock unless you know why it is slow and expect it to be slow at that clockspeed - and all you need is 50% more clock cycles and you'd be able to process that data fast enough

* Most "my sketch is slow" problems are cauised by just calling blocking functions unknowingly, not realizing could be stuck there for a while (delay(), Serial.print(), etc), cases where millis generally remains accurate) when interrupts are disabled or the ti

## Two tuning options

### Actually Tuning the part (recommended)
To make sure you get closest to to the target speed, you really ought to tune the part with the tuning sketch.

#### Requirements
* An Arduino with a crystal for it's main system clock (Or, if you happen to have a 500 Hz signal generator compatible with 5v)
  * Alternately. most oscilloscopes have a 1 kHz output. if this is the case, uncomment `#define ONEKHZMODE` - note that while this is a more accurate signal, there is more sampling noise due to the low granularity of the the rather unsophisticated way we measure the time.
* A tinyAVR 0-series, 1-series, or 2-series part. The 2-series overclock better.
* A UPDI programming method (jtag2updi, or just serial adapter + resistor or schottky diode)
* Some jumpers to connect the two.

#### Step 1: Make your reference

Open the 500HzReference example (under megaTinyCore) and then select a different board - preferably one WITH A CRYSTAL (otherwise your tuning could be off by 1% or more). Currently we support a selection of classic AVRs (328p/b and even the digispark pro and MH-ET t88 digisparklike board (hey, they've got crystals of a known frequency. Why not?). Remember that most boards and clones thereof have resonators, not crystals, so, sad to say, you get better tuning from a digispark pro or MH-ET tiny. Upload it, and what you have is blink (on LED_BUILTIN) plus an 500 kHz squarewave on pin:

|               Device              |   Output Pin   |
|-----------------------------------|----------------|
| ATmega328p-based (nano/uno/etc) ! |        9 (PB3) |
| ATmega328pb-based † !             |       9† (PB3) |
| ATtiny88 USB "MH-ET tiny"         |        9 (PB3) |
| ATmega2560/1280/2561/1281 * !     |       11 (PB5) |
| ATmega1284(p)/644(p)/etc  † !     | 8 or 13‡ (PB5) |
| Digispark Pro ‡                   |        0 (PB0) |
| TinyAVR 0/1/2 $ †                 | PB0 PB1 or PB2 |
| AVR128DA or megaAVR 0-series $    | Px0-Px2  (PC1) |
| AVR128DB or DD-series †           | Px0-Px2  (PC1) |

Notes:
!: Many boards using this processor are built with a low cost ceramic resonator (these look like about the size of a rice grain, made of shiny metal. (like a crystal, only with sloping sides, and 3 contacts on the bottom). These may be off by as much as 1-2%. Crystals are either rectangular metal devices, 7x5mm, 5x3.2mm or 3.2x2.5mm in size, or around .4-.5 inches long around 0.15" wide with rounded ends (the HC/49 package, falling out of favor due to the large amount of board real estate it takes up). Sometimes Arduino Nano clones have a crystal dedicated to the USB interface.... but still use a resonator for the main processor.

†: These are not officially sanctioned AVR devices; there is no "standard design". pin numbering may vary.

‡: Multiple pin mappings for these are in widespread use.

$: Ext. Clock very strongly recommended, especially if you're hoping for accurate tuning. On DA-series, using autotune is good enough for most purposes, though still not that good (not enough granularity in oscillator cal).

PC1 and PC2 are the only applicable pins found on all devices in the AVR Dx-series - Everything also has A0 and A1, but they are needed for the crystal (if present), and AVR DD-series has parts with only A0, A1, and C1, C2, C3, PD4-7 and PF6 (Reset), PF7 (UPDI); the sketch can only be adapted to use bits 0-2 within a port... hence why we used PORTC.

##### Hints
* As always, if you can dedicate a board to this so it's ready whenever you need it, you'll be happier.
* You can power the reference board and target from the same power source.

#### Step 2: Load the tuner
If you aren't sure that nothing uses pin0 (including for serial or some other interface), upload something inoffensive like bare minimum first

1. Open the megaTinyTune example (under megaTinyCore)
2. Select the target chip, set it for 16 or 20 MHz, and disable millis.
3. Select your UPDI programming method and port. Verify that it compiles to make sure you haven't missed anything.

#### Step 3: Tune it

1. Make the following connections:
  a. Output pin of timebase to pin 1 (PA5) of the target.
  b. Ground of timebase to ground of target.
  c. 5v of timebase to 5v of target
2. Connect your UPDI programmer (or serial port for Optiboot upload). Now everything is powered (that's why you uploaded bare minimum or blink - so it and the timebase won't be fighting over that pin)
3. upload megaTinyTuner to the target.
After a few seconds, LED will blink quickly during the tuning process. and then either finish or crash (hangs in one state or the other, and then gets rebooted by the
WDT, after which it indicates that it has been tuned with the)
Wait until the LED (LED_BUILTIN, the same pin the Blink uses; PA7 on most parts) turns on.

Unless you are using Optiboot (and hence cannot change fuse without reburning bootloader; this may not be worth the effort, depending on your application), you can tune both of them, and it's probably more convenient to do so.
If you previously tuned at 16 MHz, select 20 MHz and repeat step 3.1 - 3.3; Otherwise, select 16 MHz and repeat.

#### Step 4: Done
Disconnect the tuner from your target and now the (tuned) options will be far more accurate.

Upload blink to the tuned target using one of the (tuned) options, and it should blink at the correct speed. Test blink with the speed you want to run at before uploading the sketch you want to run.

The tuning will persist unless you explicitly erase the USERROW (also known as a the User Signature Space). It can be manipulated with the USERSIG.h library. Even across bootloading - so if you wanted to put Optiboot onto it, you can do that, and a sketch uploaded through Optiboot with Internal 20MHz (tuned) would run at 20 MHz, and with Internal 16 MHz (tuned) would run at 16 MHz.

#### Step 5: Removing or retuning
Note that unless you select a tuned clock option, no tuning will be attempted or performed, so unless you need the full USERROW, there's no reason to remove this.

To remove: Upload the example sketch "megaTinyDeTune" under megaTinyCore examples. Open serial at 115200, and type/send D to clear the tuning constants - alternately, your own sketch could just write the last 12 bytes of the USERROW to 0xFF weith USERSIG.h.

To retune: Repeat steps 2-3. Immediately after it is uploaded (must be no other reset or powercycle, and must not be running the bootloader), the tuning sketch will ALWAYS retune the chip. This allows you to fix a bad tuning, done for example far from operating conditions or with a malfunctioning reference clock.

### Without tuning the part (really easy, less accurate)

"That sounds like it would take *minutes* of work! What happens if I don't tune it, but select a (tuned) frequency anyway?"

As the #warning you get indicates - we will guess. We make no claims about the accuracy of our guesses. This **may not be accurate enough for Serial to work** depending on the individual specimen, but it may be an expedient solution in cases where the precise speed is not critical, it just needs to be near a frequency that isn't normally available. This appears to work better on 2-series parts, with untuned "tuning" values generally being close enough for USART; this may change if later lots of chips have different calibration curves.

## Quick troubleshooting
If your sketch is running far too slowly with a tuned clock selected, it is likely that the selected speed cannot be reached by that specific part: either it has been tuned (ie, calibration constants written to USERROW), but in that process, the tuning sketch determined that the oscillator could not reach the requested speed, or it has not been tuned, but when we looked at the default calibration value to add the guess adjustment to, we discovered that the guess would be outside the range of values that it can take (0-63 or 0-127 by enough that we weren't going to be close if we set it to 0 or the maximum). It is also possible that the combination of OSCCFG and part preclude operation at that frequency, no matter what (ex: 32 MHz selected on a 0/1-series part, but the oscillator is set to 16 MHz; it's rather difficult to reach this condition on recent versions of megaTinyCore, though). In all of those cases, we will not change the oscillator configuration, and instead it will run at the grindingly slow default speed of 2.66 MHz or 3.33 MHz. It is hoped that the obviously broken behavior will result in immediate recognition that the tuning is not working correctly, and no time will be wasted on other possibilities.

To tell these situations apart, since Serial is likely not usable, connect a LED to PA6 and PA7 (if PA7 if it isn't already LED_BUILTIN, such as on 8-pin parts or the Microchip development boards) and upload the following sketch:
```c
void setup() {
  VPORTA.DIR = 0xC0;          // set PA6 and PA7 outputs
  VPORTA.OUT = GPIOR0 & 0xC0;
  /* the core init() function, called before setup, uses the two high bits of GPIOR0 to report errors.
  (the bootloader uses the low 6 bits to preserve the reset cause for the application).

  PA6  ON, PA7 OFF - No tuning. If not running anywhere close to the target speed, the guess was out of range.
    The part may be able to reach that speed if you run the tuning sketch on it - however there is never any
    guarantee that a given part's oscillator will be able to reach a certain targert speed. However, in that
    case the below pattern will be seen.

  PA6 OFF,  PA7 ON - Tuned, but tuning found this speed to be unreachable (typically encountered with 30/32 MHz)
    on this specimen. The datasheet guarantees only +/- 1.5 MHz, far less than is possible in practice.

  PA6  ON,  PA7 ON - The selected speed cannot be reached by this model of chip operating from this base oscillator frequency (ex, 2-series @ 32 MHz or 1-series @ 30 with 16 MHz oscillator selected)

  Neither on       - Something strange and mysterious is going on. Possibly the tuning process generated grossly
    wrong tuning values - though this should result in something other than just running at the 2.66/3.33 MHz
    speed - either some other wrong speed, or PA7 would be on indicating that tuning found this to be unreachable.
  */
}
void loop(){}
```

The included ClockDiagnose example can be used to diagnose a variety of clock issues, including ones that do not involve tuning, however it's functionality has not been thoroughly verified experimentally. It was not designed for use when not able to refer to the sketch source code.

## Internals of oscillator tuning
As of 2.4.0, tuned internal oscillator at frequencies listed below is supported. Load the 500 Hz timebase sketch onto a working AVR microcontroller that uses a crystal or external clock for it's main clock (uhnless you want inaccurate numbers, and the tuning sketch itself to the target tinyAVR, and then connect the output of timebase to the input of tuning sketch (see comments at top of files for more info). It will cycle through all of the calibration values until it has either found a match for all of the prospective frequencies, or gone all the way to the top without finding all of them. As they are found, they will be written to the end of the USERROW - the last 6 bytes will get 20 MHz tuning values, and the 6 before those will get 16 MHz tuning values (20 or 16 refers to the center frequency). The values the tuning sketch looks for are shown below. Ensure that the part is running at the desired voltage and temperature when running the tuning for highest accuracy. If you are using the tuning features, be sure that your application code does not overwrite the calibration data after tuning.

**The "tuned" clock source options exceed Microchiop's rated maximum speed - these parts are not guaranteed to function reliably or at all** under these conditions! As a reminder, when programmed using Arduino (or any other tools that use megaTinyCore), these parts should absolutely not be used for any application where thjey could cause a safety hazard by malfunctioning, period - but especially not when overclockwe like this. Anything that the parts do beyond 20 MHz should be treated as

| Speed  | 0/1-series @ 16 | 0/1-series @ 20  |  2-series @ 16   |    2-series @ 20 | Notes  |
|--------|-----------------|------------------|----------------  |------------------|------------------|
| 10 MHz |    Yes, natural | Yes, prescale 20 | Yes, prescale 20 | Yes, prescale 20 | can't tune 20 MHz OSC down to 10, but just prescale tuned 20 MHz, 16 MHz will usually get there. None of the 2-series can achieve 10 without prescaling.
| 12 MHz |    Yes, natural |     Yes, natural |   Yes, natural   | Yes, prescale 24 | 2-series 20 can't be tuned down to 12, but we can tune it up to 24 and divide by 2.
| 16 MHz |  Yes, of course |     Yes, natural | Yes, of course   |     Yes, natural | On all parts, the 16 MHz oscillator can be turned up to 20, and other 20 down to 16.
| 20 MHz |    Yes, natural |   Yes, of course |   Yes, natural   |   Yes, of course | Use with Optiboot to not care whether the clock speed matches settings from bootloading-time -Any UPDI upload sets 16-vs-20 fuse so was never a problem except on optiboot) |
| 24 MHz |    Yes, natural |     Yes, natural |   Yes, natural   |     Yes, natural |
| 25 MHz |    Yes, natural |     Yes, natural |   Yes, natural   |     Yes, natural |
| 30 MHz |   No - can't do |   Often Unstable |     Some parts   |     Yes, natural | The 0/1 at 16 center cannot reach 30 MHz, and with 20 center barely can - if it doesn't crash before that speed. Most 2-series seem to run here at least at 5v and some can even hit it with their lower frequency oscillator.
| 32 MHz | No way in hell! |     No, can't do |  No, can't do    |  May be Unstable | Can be easily reached by the 2-series 20 MHz oscillator, which could do 36+ if only the chip could keep up.

For comparison, 0/1-series typically work at 32 @ 5v with external clock, and 2-series may be stable into the mid 30's with an external clock.

The middle 6 options can be achieved by most parts. Not all parts can make it all the way to their maximum tuning value before the chip becomes unstable; generally, they make it to 32 MHz at 5V at room temperature, but begin failing dramatically just above that; some conk out a little earlier. I would not recommend running these at more than 20 MHz where a failure would result in any inconvenience, and would not run anything where failure wuold be even a nuisance at over 25 MHz. It's worth noting that 24 MHz is a particularly useful frequency, as that is 3x the classic AVR internal oscillator speed, 1.5x what classic AVRs are most often run at from crystal, and the top rated speed for the Dx-series parts (though they can also be run a higher than rated speed. Significantly so, actually....), and code written for 24 MHz Dx-series would run at that same speed on an overclocked tinyAVR at 24 MHz. Assuming it was stable of course

*Yes, this ALSO means that the 16-MHz-center oscillator can be tuned to 20 MHz and the other way around.* No, they don't just load different calibration constants in for the two, they each have 64 or 128 values, "centered" around their respective frequencies, which overlap (and no, I checked, it's not a single list of frequency options with some offset - I checked). Not only that, sometimes the other oscillator has a calibration value that's closer to a target frequency (eg, the 16 MHz oscillator tuned up to 20 MHz gets closer to 20 than any setting on the 20 MHz oscillator does). I am sure there is a logical reason for this design. I cannot imagine what it is.

It is possible to take a reasonable guess if there is no tuning data saved on the part; this is what we do when tuned options are requested but the chip hasn't been tuned (what, you want me to read the chip before uploading to make sure it has been tuned?) we will stay at the startup frequency (3.33 or 2.66 MHz) making it obvious to the user that the clock isn't set right instead of trying to set as close as we can (eg, 0x7F or 0x3F for above examples) - one of my guiding principles is that if I know I can't give the user the behavior they ask for and that nothing that is possible is good enough in current conditions, I should try to make the failure obvious and hard to miss.

See the oscillator debug sketch for more information on troubleshooting oscillator problems - it examines the state of the part and prints out what the name of the error is. The two high bits in GPIOR0 are used to pass notice of errors in tuning to the user code (the lower 6 bits are used to communicate reset cause when optiboot is in use) since there is no other guaranteed way to communicate error messages; you may well not even have a serial port set up. But if you see it runninmg at a fraction of the expected speed, you're going to suspect the problem is related to that new tuned clock speed option you chose, right? You might even come looking at the documentation and see this explanation - that's the hope, at least.

The tuning sketch is actually fairly simple, but ugly - #ifdefs at the top choose which speeds to target as the number of microseconds they should count within eight 1ms pulses. Special tuning requirements can be met by using different numbers there (though one would still need to then compile with the correct F_CPU - meaning creating your own board file on Arduino; I gather this is easier to do with Platform.io and similar). Setup tests for a clean reset (either UPDI, or software reset, so if it doesn't have that, it can issue a software reset to ensurethat it wasn't left in a bad state after being overclocked too far, If it was a UPDI upload, we store TuningDone as 255, indicating first-upload-tuining. It then waits for a FORCE pin pulled up internally - which must be held low for half a second out of the next 3. After that if it either hasn't been tuned, the FORCE pin was grounded, or it was just uploaded, it proceeds to loop, where the clock preascale is set to 4,and we start incrementing the osccal, then measure 8 pulses of around 1ms each. The timing is actually done using plain old pulseIn 8 times.  Which is in fact accurate, being a tight loop of assembly. We use 1ms pulses not longer ones to ensure muillis counts aren't lost. Alternately the 1 kHz standard output of a scope could even be usedl this is twice as fast, so 16 samples are taken if it is chosen by defining ONEKHZMODE. if it decides tuning isn't needed, and after successful tuning, it will jump to a loop that acts like BLINK.

See also comments in the sketches.

## Alternate timebase
There's another option for the timebase. Most oscilloscopes have a 1 kHz squarewave output. You can connect this instead of a 500 Hz squarewave and use that as your timebase. To do this, uncomment the ONEKHZMODE define at the top of megaTinyTuner.
Connect the tinyAVR to your computer and ISP programmer, make sure it has something that doesn't set the tuning pin as output (ex, bare minimum or blink). Connect the ground terminal on the scope to Arduino ground, and, 1kHz terminal to tuning pin, and upload the tuning sketch.


## Future tuning sketch
*Using a timebase to tune from is - of course - inconvenient. It's tantalyzing, because we are just so close to not nee3ding that. Consider that the chip starts reasonably well tuned (at room temp, they're usually within 1%, and always close enough for UART). With the internal HF oscillator's factory cal, plus - from the SIGROW - the factory cal error term (on the 2-series parts, this is often on the just one or two parts per 1024 - truly a miracle of technology). That means that we alreadty have a knowen frequency reference. If we only had some independent timebase to compare that with, even a terribly inaccurate clock could be used - as long as it was stable (in terms of frequency) over the duration of a calibration cycle (which is only around 10 seconds), we could work out it's frequency from the internal clock at factory cal, and then the internal oscillator could be set to every calibration value possible and compared to that to figure out the appropriate value for the calibration to all the target speeds, just like the normal tuning sketch.... Now... uhh...*

*Excuse me, Mr. Clock in the back, would you please sit down. Everyone else, pay attention, there's a lot of potential here. As I was saying - the dream would be for there to be some second independent oscillator on the chip... especially if there were some convenient way to prescale it down to something like the timescales I showed us earlier - 1 kHz, 500 Hz... or even slower - that could totally transform the experience of tuning a modern AVR! I say - this can be a reality some day, we must just look inside ourselv--**Hey! Stop!  What are you doing? Put that down! I'm serious, Clock, knock that off rig-- OW! mmmmphh*** **CRASH**tinkle.... *thud*


*My name's Clock, and - unlike a certain instructor - I value yer Time, and I specialize in keeping it Real... I've got just what you need, right here....* (((to be continued)))

My idea here is simple: Run RTC from internal 32kHz oscillator. This is inaccuerate, but I believe it to be reasonably stable. Pipe it to the event system via ASYNCCH3 for 1-series or any odd channel for the 2-series (0-series parts and their owners are S. O. L. here - as usual for those low budget parts - because one of the ways in which the 0-series `EVSYS` is gimped is that, lacking async channels 2 and 3 while having the exactly same list of peripherals for the channels it does have, it can't use the divided PIT signals as event generators. What's rather hard to believe, in light of how much worse they are just to save 7 cents on the same-flash-size versions, or 15 cents on a 1607 vs a 3216) supply of the 0-series seems to be virtually nonexistent - everyplace is talking about ship times next summer, while 1-series parts, even the QFN ones that seem to be the hardest to get right now, have estimated ship dates from distributors before year-end), PIT/64 should be around 512 Hz.

Use a TCB in frequency measurement input capture mode, clocked from the system clock. You will expect that the chip will be running very close to 16 or 20 MHz immediately after system reset, and so the expected ccmp values should be 31250 or 39063 (16,000,000 or 20,000,000 divided by 512). Adjust that by a factor of (sigrow correction)/1024 to find the true expected value. Average a few captures (I'd, say, accumulate a power of 2 number of samples in an unsigned long and rightshfit the appropriate number of times - or leftshift the expected value that many times). Your sample willikely bne within a percent or two of the expected, and now it's simple math to derive the frequency that the RTC is actually running at. You can now determine how many system clock cycles the TCB would count for the desired tuned speed, and adjust the calibration value until you pass through that. An example will be added that implements the tuning process (like in megaTinyTuner now) in this way, assuming it works as well as I expect, and manage to get through it without being thrown out the window by a rogue peripheral... again...
