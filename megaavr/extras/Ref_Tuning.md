# Internal oscillators and Tuning on tinyAVR devices
All of the 0/1/2-series tinyAVR devices have an internal high-frequency oscillator which is generally used as the main oscillator; it's the only option you have other than and external clock, or running from the 32kHz internal oscillator (which makes execution of any code painfully slow and is not supported by megaTinyCore - running anything other than very carefully written straight C making use of no Arduino API functions is hopelessly slow, which is why we don't support it). For brevity and familiarity to classic AVR users, we will use the familiar name, `OSCCAL` to refer to the oscillator calibration register - this is `CLKCTRL.OSC20MCALIBA` on the tinyAVR parts.

## IMPORTANT NOTE - If you tuned prior to 2.6.0, you must tune again to get correct results
It was found that many oscillators couldn't be turned down enough to reach the speeds that we were targeting, so we added 14 MHz as a clock speed tune to on configurations except a tinyAVR 2-series @ 20 MHz, where we felt it more valuable to squeeze in the magic 32 MHz option than 14 MHz. Note that this is only relevant when uploading using Optiboot - over UPDI we set the base osc speed during upload


In the table below, **bold** speeds are prescaled from higher speeds, and *italic* ones are overclocks

Numbers are given by column for the 0/1-series and 2 series at 16 and 20 base osc speed (0-series is assumed to be same as 1-series.

|  F_CPU   |   1 @ 16  |  1 @ 20   |   2 @ 16  |   2 @ 20   |
|----------|-----------|-----------|-----------|------------|
|  1 MHz   |  **Yes**  | **Tuned** |  **Yes**  | **Tuned**  |
|  2 MHz   |  **Yes**  |  **Yes**  |  **Yes**  |  **Yes**   |
|  4 MHz   |  **Yes**  | **Tuned** |  **Yes**  | **Tuned**  |
|  5 MHz   | **Tuned** |  **Yes**  | **Tuned** |  **Yes**   |
|  6 MHz   | **Tuned** | **Tuned** | **Tuned** | **Tuned**  |
|  7 MHz   | **Tuned** | **Tuned** | **Tuned** |    No      |
|  8 MHz   |  **Yes**  | **Tuned** |  **Yes**  | **Tuned**  |
| 10 MHz   | **Tuned** |  **Yes**  | **Tuned** |  **Yes**   |
| 12 MHz   |   Tuned   | **Tuned** | **Tuned** | **Tuned**  |
| 14 MHz   |   Tuned   |   Tuned   |   Tuned   |    No      |
| 16 MHz   |  Natural  |   Tuned   |  Natural  |   Tuned    |
| 20 MHz   |   Tuned   |  Natural  |   Tuned   |  Natural   |
| 24 MHz   |  *Tuned*  |  *Tuned*  |  *Tuned*  |  *Tuned*   |
| 25 MHz   |  *Maybe*  |  *Tuned*  |  *Tuned*  |  *Tuned*   |
| 30 MHz   |   *No!!*  |  *Maybe*  |  *Maybe*  | *Usually*  |
| 32 MHz   |   *No!!*  |  *No!!*   |  *No!!*   |  *Maybe*   |

## Chart legend
* **boldface**indicates prescaling used.
* *italics* indicate system clock or oscillator exceeds rated maximum,
* Yes - either the natural speed or natural osc speed can be prescaled to this. Tuning may or may not make a difference in the timing accuracy. If it does, it will be a small one.
* Tuned - Can be achieved with tuning
* ~Tuned~ Could be reached by tuning, but we don't cal to that.
* No - 14 MHz and thus 7 MHz are only supported on 2-series with base oscillator set to 16 MHz with tuning performed.
* Usually - An aggressive overclocking which the oscillator can almost always reach, but which may not be stable.
* Maybe - An aggressive overclocking which the oscillator may not be able to reach, or if it is able to reach it, may be unstable
* No!! - the oscillator cannot reach this speed on many specimens, and when it can, device is unstable.

**Note:** These values apply only to internal oscillator 32 MHz is often achievable on 0/1-series with an external clock, though good decoupling and a solid 5v supply is required. 36 MHz may work on 2=series (maybe only extended temperature range ones) with an external clock.

**7 and 14 MHz speeds are not exposed by the core** even though in some cases we record the tuned value for it  - nor are other speeds that some people might want to run at like 27, 28, or 21. I wanted to leave some of the user row open.


## **THIS IS ONLY FOR TINYAVR**
There are many good reasons not to use it on the Dx-series; the factory cal is pretty much dead on, and you can crank it up to 32 MHz without doing anything with the cal, and most importantly, you just can't change the speed as much by tuning.

## Don't use this core, particularly

## General Concepts
As usual for AVR parts, the internal oscillator's compliance covers a far wider range than the datasheet describes. In my testing on 1-series parts, it appeared that both the 16 MHz and 20 MHz oscillator can be calibrated to as low as approximately 5/8 of their nominal speed or as high as 1 5/8 of their nominal speed The oscillator on the tinyAVR 2-series parts has gotten an upgrade in the form of the calibration register getting an additional bit - it now has 7 bit granularity instead of 6! (the high bit indicates whether the center frequency is 16 or 20 MHz). The 128 settings cover a similarly sized range of speeds, though both the top and bottom ends of this range are higher. The 2-series parts also reach higher speeds before they start to fail.

Knowing this, you could make the internal oscillator run at whatever frequency between those you wanted if only you knew the right OSCCAL setting. This has a small dependence on voltage and temperature, but it is far less dramatic than the classic AVRs - but it's different on different individual specimens. While

This section is about how to "tune" a chip and automatically store those magic OSCCAL constants in the USERROW - the one section of memory which will survive burn bootloader, as well as any action other than an active attempt to delete it or a chip erase on a locked chip. Doing this will make the "tuned" options far more accurate.

## Why not an external clock
External clocks are great, with very accurate timing, better overclocking (the 1-series parts run at 32 MHz like that), etc. but it's and extra part - oh - and it needs it's own 0.01uf decoupling capacitor so 2 extra parts to place... and ordered from the cheapest chinese electronics vendors on aliexpress, prices bottom out at 50 cents or so, all told, and if you want to know what the specs are like, you're going to end paying more for the oscillator than the chip it's clocking (see, the aliexpress vendors deny having any further specs on the part nor to know what part number it is that they're selling, or even what voltage range it works at (generally you see 1.8-3.6, and 2.7-5.5 and things like that. There was a very nice oscillator that did the whole 1.8 to 5.5v range, but it was discontinues and there's no substitute), so aliexpress oscillators are a real crap shoot... (seriously? I'm imagining someone shipping parts from plastic sandwich bags of loose parts, each bag identified only by scrawled Mhz speed written on the bag with sharpie, and having bought them that way surplus, so he didn't know anything more than that about them, since the numbers either can't be looked up or aren't even present). And on their high cost or absent specs, they occupy PA3 and the traces involved need to be kept as short as possible like all high frequency lines) (Note that the tinyAVR 0/1/2-series Rev. C boards from my Tindie shop do provide pads for an external clock, with appropriately routed ultra-short trace and cuttable bridge to fully disconnect it from the through-hole for connecting header an added precaution against interference.

If you don't need *perfect* clock accuracy, but do need a significantly different (probably higher) clock speed for whatever reason ("my sketch is running slowly" is not a good reason to overclock unless you know why it is slow and expect it to be slow at that clockspeed - and all you need is more clock cycles and you'd be able to process that data fast enough. The most common cause of sketches appearing to run slowly is waiting on some piece of blocking code that depends on external events - no amount of overclocking can fix this)

## Three tuning options

### Actually Tuning the part with a reference (recommended)
To make sure you get closest to to the target speed, you really ought to tune the part with the tuning sketch.

#### Requirements
* An Arduino with a crystal for it's main system clock (Or, if you happen to have a 500 Hz signal generator compatible with 5v)
  * Alternately. most oscilloscopes have a 1 kHz output. if this is the case, uncomment `#define ONEKHZMODE` - note that while this is a more accurate signal, there is more sampling noise due to the low granularity of the rather unsophisticated way we measure the time.
* A tinyAVR 0-series, 1-series, or 2-series part. The 2-series overclock better. It must be soldered
* A UPDI programming method (jtag2updi, or just serial adapter + resistor or Schottky diode)
* Some jumpers to connect the two.

#### Step 1: Make your reference

Open the 500HzReference example (under megaTinyCore) and then select a different board - preferably one WITH A CRYSTAL (otherwise your tuning could be off by 1% or more). Currently we support a selection of classic AVRs (328p/b and even the digispark pro and MH-ET t88 digisparklike board (hey, they've got crystals of a known frequency. Why not?)). Remember that most boards and clones thereof have resonators, not crystals, so, sad to say, you get better tuning from a digispark pro or MH-ET tiny. Upload it, and what you have is blink (on LED_BUILTIN) plus an 500 kHz squarewave on pin:

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
* You can power the reference board and target from the same power source, and if possible, you should.

##### Alternate timebase
There's another option for the timebase. Most oscilloscopes have a 1 kHz squarewave output. You can connect this instead of a 500 Hz squarewave and use that as your timebase. To do this, uncomment the ONEKHZMODE define at the top of megaTinyTuner.
Connect the tinyAVR to your computer and ISP programmer, make sure it has something that doesn't set the tuning pin as output (ex, bare minimum or blink). Connect the ground terminal on the scope to Arduino ground, and, 1kHz terminal to tuning pin, and upload the tuning sketch.

#### Step 2: Prepare to load the tuner
If you aren't sure that nothing uses PA4/pin 0, (including for serial or some other interface), upload something inoffensive like bare minimum first

1. Open the megaTinyTuner example (under megaTinyCore)
2. Select the target chip, set it for 16 or 20 MHz, and disable millis.
3. Select your UPDI programming method and port. Verify that it compiles to make sure you haven't missed anything.

#### Step 3: Tune it
If a separate power supply is to be used for the timebase, its voltage must be no more than 0.5V higher than the target's operating voltage, and a HIGH output by the timebase must be of a voltage high enough to be recognized as a HIGH by the ATtiny (meaning 3.3V logic is just barely good enough when the target is running at 5V). Running the timebase from the UPDI programmer power supply by way of the target is recommended, that way you can't accidentally power the timebase when the chip isn't powered, and the timebase will always be the same voltage as the target.
1. Make the following connections:
  a. Ground of timebase to ground of target.
  b. Vcc of target to either
  c. Output pin of timebase to pin 1 (PA5) of the target.
2. Connect your UPDI programmer (or serial port for Optiboot upload). Now everything is powered (that's why you uploaded bare minimum or blink - so it and the timebase won't be fighting over that pin)
3. upload megaTinyTuner to the target. After a few seconds, the LED should blink quickly during the tuning process, and then either finish, indicated by solid-on. It is possible (in fact, quite common) for this to crash, and get rebooted by the WDT 8 seconds thereafter, after which it will similarly indicate that the part is tuned. Wait until the LED (LED_BUILTIN, the same pin the Blink uses; PA7 on most parts) turns on.

Unless you are using Optiboot (and hence cannot change fuse without reburning bootloader; this may not be worth the effort, depending on your application), you can tune both of them, and it's probably more convenient to do so at the same time; If you previously tuned at 16 MHz, select 20 MHz and repeat step 3; Otherwise, select 16 MHz and repeat.

#### Step 4: Done
Disconnect the tuner from your target and now the (tuned) options will be far more accurate.

Upload blink to the tuned target using one of the (tuned) options, and it should blink at the correct speed. Test blink with the speed you want to run at before uploading the sketch you want to run.

The tuning will persist unless you explicitly erase the USERROW (also known as a the User Signature Space). It can be manipulated with the USERSIG.h library. Even across bootloading - so if you wanted to put Optiboot onto it, you can do that, and a sketch uploaded through Optiboot with Internal 20MHz (tuned) would run at 20 MHz, and with Internal 16 MHz (tuned) would run at 16 MHz.

#### Step 5: Removing or retuning
Note that unless you select a tuned clock option, no tuning will be attempted or performed, so unless you need the full USERROW, there's no reason to remove this.

To remove: Upload the example sketch "megaTinyDeTune" under megaTinyCore examples. Open serial at 115200, and type/send D to clear the tuning constants - alternately, your own sketch could just write the last 12 bytes of the USERROW to 0xFF with USERSIG.h.

To retune: Repeat steps 2-3. Immediately after it is uploaded (must be no other reset or powercycle, and must not be running the bootloader), the tuning sketch will ALWAYS retune the chip. This allows you to fix a bad tuning, done for example far from operating conditions or with a malfunctioning reference clock.

### Self-tuning (room temperature only, no reference needed)
This is less accurate than a crystal. Connect a serial adapter to Serial, and open your favorite serial terminal (which hopefully isn't the wretched serial monitor). Use UDPI to upload SelfTuning.ino without modification, with speed set to 20 MHz Internal.

Within under a minute the oscillator will be tuned based on it's default turning value after correcting for the oscillator error (except on 2-series, where internal oscillator error isn't in the sig row and 0-series which can't compile the sketch at all). Unless you have disabled debug output, you will get a play-by-play accounting of the process.

Switch selected clock to 16 MHz and repeat the process.

This will overwrite anything you had stored in the last 12 bytes of the USERROW.

### SelfTuning from external watch crystal
Connect a serial adapter to the alternate serial pins and change the `HAS_EXT_XTAL` to 1 instead of 0.  Otherwise, proceed as above.

Once the tuning is complete, you may disconnect the crystal and it's loading caps if they are not needed. (the loading caps are of such low value that they don't *need* to be removed), and frequently end up being nasty small components. The lowest I'm willing to place is 0603, and in a couple of cases I used them for crystal loading caps to save space.

#### SelfTuning Caveats
* Using a crystal requires a 1-series or 2-series part.
* Not using a crystal will not be as accurate as an external timebase or crystal.
* Layout of the crystal pins must be done very carefully!
* Avoid being thrown out the window by malfunctioning peripherals. (see jolly story at the end of this document)

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
    guarantee that a given part's oscillator will be able to reach a certain target speed. However, in that
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

It is possible to take a reasonable guess if there is no tuning data saved on the part; this is what we do when tuned options are requested but the chip hasn't been tuned (what, you want me to read the chip before uploading to make sure it has been tuned? This would actually be a ) we will stay at the startup frequency (3.33 or 2.66 MHz) making it obvious to the user that the clock isn't set right instead of trying to set as close as we can (eg, 0x7F or 0x3F for above examples) - one of my guiding principles is that if I know I can't give the user the behavior they ask for and that nothing that is possible is good enough in current conditions, I should try to make the failure obvious and hard to miss.

See the oscillator debug sketch for more information on troubleshooting oscillator problems - it examines the state of the part and prints out what the name of the error is. The two high bits in GPIOR0 are used to pass notice of errors in tuning to the user code (the lower 6 bits are used to communicate reset cause when optiboot is in use) since there is no other guaranteed way to communicate error messages; you may well not even have a serial port set up. But if you see it running at a fraction of the expected speed, you're going to suspect the problem is related to that new tuned clock speed option you chose, right? You might even come looking at the documentation and see this explanation - that's the hope, at least.

The tuning sketch is actually fairly simple, but ugly - #ifdefs at the top choose which speeds to target as the number of microseconds they should count within eight 1ms pulses. Special tuning requirements can be met by using different numbers there (though one would still need to then compile with the correct F_CPU - meaning creating your own board file on Arduino; I gather this is easier to do with Platform.io and similar). Setup tests for a clean reset (either UPDI, or software reset, so if it doesn't have that, it can issue a software reset to ensure that it wasn't left in a bad state after being overclocked too far, If it was a UPDI upload, we store TuningDone as 255, indicating first-upload-tuning. It then waits for a FORCE pin pulled up internally - which must be held low for half a second out of the next 3. After that if it either hasn't been tuned, the FORCE pin was grounded, or it was just uploaded, it proceeds to loop, where the clock prescale is set to 4, and we start incrementing the OSCCAL, then measure 8 pulses of around 1ms each. The timing is actually done using plain old pulseIn 8 times.  Which is in fact accurate, being a tight loop of assembly. We use 1ms pulses not longer ones????? to ensure millis counts aren't lost. Alternately the 1 kHz standard output of a scope could even be used, this is twice as fast, so 16 samples are taken if it is chosen by defining ONEKHZMODE. if it decides tuning isn't needed, and after successful tuning, it will jump to a loop that acts like BLINK.

See also comments in the sketches.

### Selftuning
SelfTuning without a crystal makes the following assumptions:
* the factory calibration picked the closest room temperature option.
* If a 0/1-series, that the oscillator error value in the sigrow is accurate.
* The 32kHz oscillator is stable, but very poorly calibrated (it is extremely crude)
Assuming assumptions are correct, passable tuning can be achieved in this way.it can time the ratio of the system clock to the internal 32k oscillator. Since internal 16/20 MHz clock is known quite accurately thanks to the error terms or the 2 series just plain , we can determine the frequency of the "32khz" oscillator, and then loop over all OSCCAL values starting from 0 using the 32khz osc , calculating the main oscillator speed from the "32k" one, with a list of the 6 target values , and compare it's calculated speed to them, and if it's farther than the previous one was, it writes whichever value was closer to the target to the USERROW and then go on to the next target. We drop back to a div/4 prescaler while adjusting the clock speed, and follow the oscillator adjustments with a nop which is supposedly able to prevent bad behavior immediately after an adjustment to osccal, we then disable the prescaling and start timing the RTC event generator channel accumulating 8 samples./*

### Tidying and unstable speeds
Any OSCCAL value which is found to produce anomalous results for logical and arithmetic functions is considered to be unstable. This very often results in a crash, which is why the watchdog is used. If we survive long enough to set the prescaler back to 4, we record, The calbyte is marked 0x80. Upon startup, when all the tuning sketches see if they've already been run. They always attempt to run after a UPDI reset (ie, when you uploaded the sketch over UPDI, that ended with a UPDI reset) but otherwise they will run only after a reset if either the configurable FORCE_PIN is held low (to force recalibration, or if 0xFF is the value stored in the userrow address corresponding to the nominal oscillator speed ("Home". While checking this it will look for an unexpected 0xFF or 0x00 near the higher targets, which would imply that the microcontroller crashed before reaching the target speed; it will mark that speed and all higher one 0xC0. So when a tuned speed that the oscillator can on some parts of that model achieve is selected, `init_clock` checks the calbytes we stored in the userrow. If "home" ix 0xFF, we will guess (in terms of an offset from factory cal), and unless it gets a above 258 (which is treated as 255) or below -3 (negative numbers treated as 0). If it is out side that range, it's guess is that the speed cannot be achieves with that part and it leaves the speed at default.
The ClockDebug example can get more information on what's going on.
If there IS a value in "home", we check the calbyte for the target speed, If that & 0x80, we abort, because it has been shown to be unstable. (bit six is recorded to help humans to debug problems, and is shown by things like ClockDebug example - that is a very strong sign that you ain't never gonna get that thing running at that speed - it crashed before it even made it to the target!/

As of 2.5.13 we have finished this, and


## Historical
Prior to self-tuning actually being included, I had long theorized that it was possible.

Setting: A classroom full of programmers and anthropomorphized peripherals are attending a boring class taught by one of those professors who lost the plot a while back droning on about tuning the internal oscillator....
*Using a timebase to tune from is - of course - inconvenient. It's tantalizing, because we are just so close to not needing that. Consider that the chip starts reasonably well tuned (at room temp, they're usually within 1%, and always close enough for UART). With the internal HF oscillator's factory cal, plus - from the SIGROW - the factory cal error term (on the 2-series parts, this is often on the just one or two parts per 1024 - truly a miracle of technology). That means that we already have a known frequency reference. If we only had some independent timebase to compare that with, even a terribly inaccurate clock could be used - as long as it was stable (in terms of frequency) over the duration of a calibration cycle (which is only around 10 seconds), we could work out it's frequency from the internal clock at factory cal, and then the internal oscillator could be set to every calibration value possible and compared to that to figure out the appropriate value for the calibration to all the target speeds, just like the normal tuning sketch.... Now... uhh...*

*Excuse me, Mr. Clock in the back, would you please sit down. Everyone else, pay attention, there's a lot of potential here. As I was saying - the dream would be for there to be some second independent oscillator on the chip... especially if there were some convenient way to prescale it down to something like the timescales I showed us earlier - 1 kHz, 500 Hz... or even slower - that could totally transform the experience of tuning a modern AVR! I say - this can be a reality some day, we must just look inside ourselv--**Hey! Stop! What are you doing? Put that down! I'm serious, Clock, knock that off rig-- OW! mmmmphh*** **CRASH**tinkle.... *thud*


*My name's Clock, and - unlike a certain instructor - I value yer Time, and I specialize in keeping it Real... I've got just what you need, right here....*

The idea here is simple: Run RTC from internal 32kHz oscillator. This is inaccurate (if you have a crystal, it's not, rich kid), but pretty stable stable. Pipe it to the event system via ASYNCCH3 for 1-series or any odd channel for the 2-series (0-series parts and their owners are S. O. L. here - as usual for those low budget parts. I hope we don't have any of those here today, we dun like your kind round these parts, you hear me? The 0-series is strictly inferior and can't selftune! So if you're a 0-series, get out of here, I'm not talking to your kind. *several sadfaced 0-series parts slink out of the classroom with tears in their eyess* Because, ya see here one of the many ways ways the 0-series `EVSYS` is gimped is that, lacking async channels 2 and 3 while having the exactly same list of peripherals for the channels they do have... they don't got no PIT output channels. So they can't use the divided PIT signals as event generators. It's a little hard to imagine who'd save 7 cents tops at that kinda cost isn't it? What I never got was way there was always so many of em around, taking our precious die space!

Anyway, so let's turn on the RTC and turn on the PIT! PIT/64 should be around 512 Hz

Now grab a type B timer (not yer millis timer you fool! Who let you into this class?) Set that for frequency measurement input capture mode, clock it from 'da system clock. Now you gonna be running very close to 16 or 20 MHz immediately after system reset and init_clock(), and so the expected ccmp values should be 31250 or 39063 (16,000,000 or 20,000,000 divided by 512). Adjust that by a factor of (sigrow correction)/1024...
"I'm a 2-series they don't give us our osciullator error during factory cal anymore!
"Well OBVIOUSLY if you don't have a OSC ERR value in yer sig, you keep good enough time without it, so quit braggging." Now to find the true expected value,. Average a few captures (I'd, say, accumulate a power of 2 number of samples in an unsigned long and rightshfit the appropriate number of times - or leftshift the expected value that many times). Your sample will likely be within a percent or two of the expected, and now it's simple math to derive the frequency that the RTC is actually running at. You can now determine how many system clock cycles the TCB would count for the desired tuned speed, and just step through each cal value, time it a few times, until you find what you're looking for. And unlike that doddering fool who rambles up here every day, I've got the code you need right here! [SelfTuning example](../libraries/megaTinyCore/examples/SelfTuning/SelfTuning.ino)

Okay! Class dismissed, let's hit the beaches, I heard them AVR double-D's are there! (cheers as students leap up)

*As Mr. Clock struts towards the door a dog barks in the distance. Mr. Clock's expression turns to one of terror... He does a 180 and runs for the window himself, but before he can get there he is tackled by a huge, vicious dog*

"It looks like someone wasn't paying attentione when I was talking about the windowed watchdog timer - which is the perfect way to return to controlled operation after a wild pointer, unintentional infinite loop or..." looking at Mr. Clock, still pinnned by the watchdog " a misbehaving peripheral...... Remember, using the window lets you reset if WDR is issued to late **OR too early** And might I note that I don't believe Mr. Clock actually tested that code... Now, where was I.....

Right.....
*Using a timebase to tune from is - of course - inconvenient.* (the class starts over to a chorus of groans throughout the room)
