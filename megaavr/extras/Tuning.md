# Internal oscillators and Tuning on tinyAVR devices
All of the 0/1/2-series tinyAVR devices have an internal high-frequency oscillator which is generally used as the main oscillator; it's the only option you have other than and extermnal clock, or running from the 32kHz internal oscillator (which makes execution of any code painfully slow and is mnot supported by megaTinyCore - running anything other than very carefully written straight C making use of no Arduiino API functions is hopelessly slow, whicih is why we don't support it).

## **THIS IS ONLY FOR TINYAVR**
There are many good reasons not to use it on the Dx-series; the factory cal is pretty much dead on, and you can crank it up to 32 MHz without doing anything with the cal. I certainly have no plans to support tuning over there. I also would be disinclined towards tuining support if these parts would work with a crystal.

## Don't use chips with extreme tuning for safety critical tasks
I know I say this all over the place, but most of the speeds enabled by tuning are overclocks. While these chips are rated for safety critical applications, neither the Arduino libraries, nor anything related to this core are. Parts used outside their specified operating range may malfunction with unpredictable results. Almost any arbitrary result is possible, from crashes to incorrect results for arithmatic and logic, to making demons fly out of your nostrisls. That is obviously not acceptable for safety critical aoplicatoins,

## General Concepts
As usual for AVR parts, the internal oscillator's compliance covers a far wider range than the datasheet describes. In my testing on 1-series parts, it appeared that both the 16 MHz and 20 MHz oscillator can be calibrated to as low as approximately 5/8 of their nominal speed or as high as 1-5/8 of their nominal speed The oscillator on the tinyAVR 2-series parts has gotten an upgrade in the form of the calibration register getting an additional bit - it now has 7 bit granularity instead of 6! (the high bit indicates whether the center frequency is 16 or 20 MHz). The 128 settings cover a similarly sized range, of speeds, though both the top and bottom ends of this range are higher. The 2-series parts also reach higher speeds beforethey start to fail.

Knowing this, you could make the internal oscillator run at whatever freqency you want if you but knew the right OSCCAL setting. This has a small depemndnce on voltage and temperature, but it os far less dramatic than the classic AVRs. You'd certainly be on track to match the accuracy of the factory cal if you did the calculation at the sametemperatiure and voltage it's expected to run at - even if the frequency you were targeting isn't one that is stock.

This section is about how to "tune" a chip and automatically storethose magic OSCCAL constants in the USERROW - the one section of memory which will survive burn bootloader, as well as any action other than an active attemopt to delete it or a chip erase on a locked chip. Doing this will make the "tuned" options far more accurate.


### Why not an external clock
External clocks are great and all - very accurate timing, better overclocking (the 1-series parts run reliably at 32 MHz like that), etc. but it's amn extra part - oh - and it needs it's own 0.01uf decoupling capacitor so 2 extra parts top place... and even ordered from the cheapeast dirtiest connections in china, it's hard to get them for less than 70 cents or so, all told, and if you want to know the specs, you're going to end uo paying more for the oscillatoer than the chip it's clocking. Amd on  top of all that they take up a pin (Note that the tinyAVR 0/1/2-series Rev. C boards from my Tindie shop will cover this!)


If you don't need *perfect* clock accuracy, but do need a significiantly different (probably higher) clock speed for whatever reaso ("my sketch is running slowly" is not a good reason to overclock unless you know why it is slow and expect it to be slow, hence expect that 50% more clock cycles woild make it work... if it's a mystery to you why your code is running slowly, further debugging effortshould be made prior to overclocking. Also you should first deetermine that these slow steps could mot be mitigated in some other way. Maybe you need to be just a bit faster to keep up with some other device, or you're doing one of those drive an old television hacks, or whatever). Or maybe you just want to use optiboot to load sketches (particulalrly appealing on the 20 and 24 pin 2-series parts since they have that alternate reset pin option), but don't want to re-bootload between 16 vs 20.....

Well, in that case, you are in luck, because as in 1.4.0, tuned oscillator frequencies are supported!

There are two approaches to the Internal (tuned) clock speed options -

### Actually Tuning the part (recommended)


#### Requirements
* An Arduino wih a crystal for it's main system clock (Or, if you happen to have a 500 Hz signal generator compatible with 5v)
  * Alternately. most oscilloscopes have a 1 kHz output. if this is the case, uncomment `#define ONEKHZMODE` - note that while this is a more accurate signal, there is more sampling noise due to the low granularity of the the rather unsophisticated way we measure the time.
* A tinyAVR 0-series, 1-series, or 2-series part. The 2-series overclock better.
* A UPDI programming method (jtag2updi, or just serial adapter + resistor or schottky diode)
* Some jumpers to connect the two.

#### Step 1: Make your timebase

Open the 500HzTimebase example (under megaTinyCore) and then select a different board - preferably one WITH A CRYSTAL (otherwise your tuning could be off by 1% or more). Currently we support a selection of classic AVRs that most everyone has kicking around what with the newer, better parts making thje rounds I use a nano for this (now that I don't need them to make more jtag2updi programmers). Upload it, and what you have is blink (on LED_BUILTIN) plus an 500 kHz squarewave on pin:

|               Device              |   Output Pin   |
|-----------------------------------|----------------|
| ATmega328p-based (nano/uno/etc) ! |        9 (PB3) |
| ATmega328pb-based † !             |       9† (PB3) |
| ATtiny88 USB "MH-ET tiny"         |        9 (PB3) |
| ATmega2560/1280/2561/1281 * !     |       11 (PB5) |
| ATmega1284(p)/644(p)/etc † !      | 8 or 13‡ (PB5) |
| Digispark Pro                     |        0 (PB0) |


Notes:
!: Many boards using this processor are build with a low cost ceramic resonator (these look like about the size of a rice grain, made of shiny metal. (like a crystal, only with sloping sides, and 3 contacts on the bottom). These may be off by as much as 1-2%. Crystals are either rectangular metal devices, 7x5mm, 5x3.2mm or 3.2x2.5mm in size, or around .4-.5 inches long around 0.15% wide with rounded ends (the HC/49 package, falling out of favor due to the large amount of board real estate it takes up). Sometimes Arduino Nano clones have a crystal dedicated to the USB interface.... but still use a resonator for the main processor.

†: These are not officially sanctioned AVR devices; there is no "stamdard design". pin numbering may vary.

‡: Multiple pin mappings for these are in widespread use.

(note: we presently only support 5V for this, as there isn't much call for it outside of overclocking applications)

##### Hints:
* As always, if you can dedicate a board to this so it's ready whenever you need it, you'll be happier.
* You can power the timebase board and target from the same power source.

#### Step 2: Load the tuner
If you arent sure that nothing uses pin0 (including for serial or some other interface), upload something inoffensive like bare minimum first

Open the megaTinyTune example (under megaTinyCore)
Select the target chip, set it for 16 or 20 MHz, and disable millis.
Select your UPDI programming method and port. Verify that it compiles to make sure you haven't missed anything.

#### Step 3: Tune it!

Make the following connections:
Output pin of timebase to pin 1 (PA5) of the target.
Ground of timebase to ground of target.
5v of timebase to 5v of target

Connect your UPDI programmer. Now everything is powered (that's why you uploaded bare minimum or blink - so it and the timebase won't be fighting over that pin)

And upload megaTinyTunr to the target.
Wait until the LED (LED_BUILTIN, the same pin the Blink uses; PA7 on most parts) turns on.

If you previously tuned at 16 MHz, select 20 MHz and repeat step 3.
Otherwise, select 16 MHz and repeat.

#### Step 4: Done!
Disconnect the tuner from your target and now the (tuned) options will be far more accurate.

The tuning will persist unless you explicitly erase the USERROW (also known as a the User Signature Space). It can be manipulated with the USERSIG.h library. Even across bootloading - so if you wanted to put Optiboot onto it, you can do that, and a sketch uploaded through Optiboot with Internal 20MHz (tuned) would run at 20 MHz, and with Internal 16 MHz (tuned) would run at 16 MHz.

#### Step 5: Removing or retuning

To remove: Use the example sketch "RemoveTuning" under megaTinyCore examples. or write the last 12 bytes of the USERROW to 0xFF weith USERSIG.h.

To retune: Repeat steps 2-3. Immediately after it is uploaded (must be no other reset or powercycle, and must not be running the bootloader), the tuning sketch will ALWAYS retune the chip. This allows you to fix a bad turning

### Without tuning the part (really fast)

Of course, this all begs the question of "what happens if I don't tune it, but select a (tuned) frequency anyway?"

As the #warning you get indicates - we will guess. We make no claims about the accuracy of our guesses. This **may not be accurate enough for Serial to work** depending on the individual specimen

This is not recommended practice, but it may be an expedient solution in cases where the precise speed is not critical, it just needs to be near a frequency that isn't normally available. This appears to work better on 2-series parts, but that may just be due to there being only one lot of them making the rounds, and some other facility will start cranking out ones with different OSCCAL curves as they ramp up.


#### Internal oscillator tuning
As of 2.4.0, tuned internal oscillator at frequencies listed above is supported. Load the 500 Hz timebase sketch onto a working AVR microcontroller that uses a crystal or external clock for it's main clock (uhnless you want inaccurate numbers, and the tuning sketch itself to the target tinyAVR, and then connect the output of timebase to the input of tuning sketch (see comments at top of files for more info). It will cycle through all of the calibration values until it has either found a match for all of the prospective frequencies, or gone all the way to the top without finding all of them. As they are found, they will be written to the end of the USERROW - the last 6 bytes will get 20 MHz tuning values, and the 6 before those will get 16 MHz tuning values (20 or 16 refers to the center frequency). The values the tuning sketch looks for are shown below. Ensure that the part is running at the desired voltage and temperature when running the tuning for highest accuracy. If you are using the tuning features, be sure that your application code does not overwrite the calibration data after tuning.

**The "tuned" clock source options exceed Microchiop's rated maximum speed - these parts are not guaranteed to function reliably or at all** under these conditions! As a reminder, when programmed using Arduino (or any other tools that use megaTinyCore), these parts should absolutely not be used for any application where thjey could cause a safety hazard by malfunctioning, period - but especially not when overclockwe like this. Anything that the parts do beyond 20 MHz should be treated as



| Speed  | 0/1-series @ 16 | 0/1-series @ 20  |  2-series @ 16 |    2-series @ 20 |
|--------|-----------------|------------------|----------------|------------------|
| 10 MHz |    Yes, natural | Yes, prescale 20 |             No |               No |
| 12 MHz |    Yes, natural |     Yes, natural |   Yes, natural | Yes, prescale 24 |
| 16 MHz |  Yes, of course |     Yes, natural | Yes, of course |     Yes, natural |
| 20 MHz |    Yes, natural |   Yes, of course |   Yes, natural |   Yes, of course |
| 24 MHz |    Yes, natural |     Yes, natural |   Yes, natural |     Yes, natural |
| 25 MHz |    Yes, natural |     Yes, natural |   Yes, natural |     Yes, natural |
| 30 MHz |   No - can't do |   Often Unstable |     Some parts |     Yes, natural |
| 32 MHz | No way in hell! |     No, can't do |  No, can't do  |   Often Unstable |

Here "natural" means that it's coming out of the internal oscillator at that frequency, as opposed to, say, twice the frequency and getting divided with the prescaler.


The middle 6 options can be achieved by most parts. Not all parts can make it all the way to their maximum tuning value before the chip becomes unstable; generally, they make it to 32 MHz at 5V at room temperature, but begin failing dramatically just above that; some conk out a little earlier; . I would not recommend running code at anything above 25 MHz, except to investigate the behavior of overclocked AVRs, and I would never run them above 20 MHz if a hang or crash would be inconvenient. 24 MHz is a particularly useful frequency, as that is the top rated speed for the Dx-series parts (although they will generally do 32 MHz+), and code written for 24 MHz Dx-series would run at that same speed on an overclocked tinyAVR at 24 MHz. Assuming it was stable of course, which is not guaranteed/

Yes, this ALSO means that the 16-MHz-center oscillator ca nbe tuned to 20 MHz and the other way around. No, they don't just load different calibration constants in for the two, they each have 64 or 128 values, centered around their respective frequencies, which overlap (and no, they're not the same list of values offset from each other). Not only that, sometimes the other oscillator has a calibration value that's closer to a target frequency (eg, the 16 MHz oscillator tuned up to 20 MHz gets closer to 20 than any setting on the 20 MHz oscillator does). I am sure there is a logical reason for this design. I cannot imagine what it is.

It is possible to take a reasonable guess if there is no tuning data saved on the part; this is what we do when tuned options are requested but the chip hasn't been tuned (what, you want me to read the chip before uploading to make sure it has been tuned? Dream on - if you want that, give me a PR, that's a ton of work) of what the best value for a frequency might be . If we see there is no hope (eg, we're targeting 32 (and 30 on 0/1) but oscillator is set with 16 MHz center, or it's a high frequency, but the stock cal is already high, such that we know there's no chance we'll be able to hit it) we will stay at the startup frequency (3.33 or 2.66 MHz) making it obvious to the user that the clock isn't set right instead of trying to set as close as we can (eg, 0x7F or 0x3F for above examples) - one of my guiding principles is that if I know I can't give the user the behavior they ask for and that nothing that is possible is good enough in current conditions, I should try to make the failure obvious and hard to miss.

See the oscillator debug sketch for more information on troubleshooting oscillator problems - it examines the state of the part and prints oiut that the name of the error is.  The two high bits in GPIOR0 are used to pass notice of errors in tuning to the user code (the lower 6 bits are used to communicate reset cause when optiboot is in use) since there is no other guaranteed way to communicate error messages; you may well not even have a serial port set up. But if you see it runninmg at a fraction of the expected speed, you're going to suspect the problem is related to that new tuned clock speed option you chose, right? You might even come looking at the documentation and see this explanation - that's the hope, at least.
