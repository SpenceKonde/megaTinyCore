# tinyNeoPixel - a modern AVR (AVRxt) compatible library for WS2812 "NeoPixel" and similar
**for tinyAVR 0/1/2-series, megaAVR 0-series, AVR Dx-series,  future AVR Ex-series**

The change in architecture from the classic AVRs (which use the AVRe or AVRe+ variant of the AVR instruction set) to "modern" AVRs (which use AVRxt) causes many of the existing WS2812 libraries to not work on the new parts. This library adapts the tinyNeoPixel library to work with AVRxt version; this has slightly improved execution times, among other things, and adds support for additional clock speeds: 10 and 20 MHz because of it's use on the tinyAVR 0/1/2-series and megaAVR 0-series, and faster speeds all the way up to 48 MHz (At room temperature, at least some extended temperaturer range Dx-series parts can run at that speed. It's not as surprising as it seems at first glance: don't forget that the extended temp Dx parts are spec'ed for use in life safety critical applications at 125C). Unlike the version for classic AVR (AVRe/AVRe+), one of the benefits of the improved timing is that ST (indirect store) executes in 1 clock cycle, rather than 2, so there is no need for multiple conditionally included implementations that were needed before (so thy could use the signle-cycle OUT instruction, whose target register must be known at compiletime and which must be one of the lowest 64 register addresses. )

Like the normal Adafruit_NeoPixel library, this supports WS2811/WS2812/SK6812 and all the nominally compatible single-wire individually addressable LEDs, but not the ones that use both data and clock, like the APA102 or the countless knockoffs thereof; libraries for those do not pose special compatibility issues with AVRxt, and there is no need to reinvent, modify, or reimagine the wheel. See the section on compatible LEDs for details on what this library supports.


## tinyNeoPixel and tinyNeoPixel_Static
There are two versions of this library provided. `tinyNeoPixel` implements the entire API that Adafruit_NeoPixel does, including setting the length and type of LEDs (color order and whether it's RGB or RGBW) of the string at runtime. This provides maximum portability between code written for use with Adafruit_NeoPixel and tinyNeoPixel (only the constructor and library name need to be changed) - however, the memory used to store the "frame buffer" (the color of every pixel) is not included in the SRAM usage displayed when compiling a sketch because it is "dynamically allocated". This can be a large portion of the memory available on smaller parts (I have seen support inquiries from people trying to control a string of LEDs which would require several times the total memory of the part for this alone); trying to allocate a larger array than will fit in RAM does not generate any sort of error - it just fails to allocate anything at runtime, nothing gets sent to the light string, and no LEDs turn on. This dynamic memory allocation also requires compiling in malloc(), free(), and associated functions; on parts with small flash (ie, tinyAVR), this can be significant. Finally, dynamic memory allocation is arguably bad practice on small embedded systems like the AVR devices.

`tinyNeoPixel_Static` is slightly cutdown, removing the option to change the length or type of pixels at runtime (which relies on dynamic memory allocation). Instead it requires the user to manually declare the pixel array and pass it to the tinyNeoPixel constructor. **Additionally, it does not set the pinMode of the pin (the sketch must set this as OUTPUT);** - the reason for this was that it is a no-holds-barred attempt to cut the flash consumption as much as possible, Removing that last pinMode cuts out over 100 bytes, which is nothing to sneeze at on the tiniest of tinies! Note that the savings requires eliminating *all* calls to pinMode and replacing them with writes to`VPORTx.DIR` registers (if you have to set only a small number of bits in one of those, and everything is known at compiletime, you're often better with several consecutive `|=` operators each setting one bit, because those end up as a single-word SBI or CBI - while `|=` for more than 1 bit requires a read/modify/write (IN, ORI, OUT, which must be done with interrupts disabled if any interrupts modify the same register. Finally, no call to begin() should be made, nor can ot be made. These changes reduce sketch size and provide greater visibility on the memory usage. **Unless you need to change string length or type at runtime, it is recommended that `tinyNeoPixel_Static` be used** - it uses less flash, particularly if you don't use malloc elsewhere (I try to avoid it - the core certainly doesn't, nor, to my knowledge, do any of the included libraries!


## Constructors
The constructor is the declaration that you call outside of any function, to create the global tinyNeoPixel object.
```c++
#include <tinyNeoPixel_Static.h>
#define NUMLEDS 100
byte pixels[NUMLEDS*3];
tinyNeoPixel leds = tinyNeoPixel(NUMLEDS, 5, NEO_GRB, pixels);
void setup() {
    pinMode(5,OUTPUT);
    leds.setPixelColor(0,255,0,0); // first LED full RED
    leds.show();                   // LED turns on.
}
```
The equivalent example with the Static version uses only 1076 bytes and reports (accurately) that it uses 330 bytes of RAM.


`tinyNeoPixel(uint16_t n, uint8_t p, neoPixelType t=NEO_GRB)` - for `tinyNeoPixel` only.
```c++
#include <tinyNeoPixel.h>
#define NUMLEDS 100
tinyNeoPixel leds = tinyNeoPixel(NUMLEDS, 5, NEO_GRB);
void setup() {
    leds.begin();
    leds.setPixelColor(0,255,0,0); // first LED full RED
    leds.show();                   // LED turns on.
}
```
This non-static tinyNeoPixel example uses 2236 bytes of flash, and reports using only 40 bytes of RAM (it actually uses 340 - and if you didn't have enough free memory the call to leds.begin() would fail, the LEDs would not be enabled, bnut the rest of the sketch would continue to run, which could be confusing to debug.

`tinyNeoPixel()` -  Empty constructor for `tinyNeoPixel` only - for when you won't even know the type of LEDs, or how many of them, until your sketch starts running. You set pin and length later with `setPin()`, `updateLength()`, and `updateType()`, which must be set before you can control any LEDs. You might have the same code running a number of lighting displays, and store the specifics in EEPROM, as shown below:
```c++
#include <tinyNeoPixel.h>
#include <EEPROM.h>
tinyNeoPixel leds = tinyNeoPixel();
void setup() {
    uint16_t numleds=EEPROM.read(0)*50; // see how many 50-LED strips we're driving.
    leds.updateLength(numleds);         //
    leds.updateType(EEPROM.read(1));    // and read what type of LEDs
    leds.setPin(5);                     // Maybe I don't need to get everything from the EEPROM
    leds.setPixelColor(0,255,0,0);      // first LED full RED
    leds.show();                        // LED turns on.
}

```
This compiles to 2256 bytes and reports 40 bytes of RAM used as well (but, it is using an additional 150 bytes times whatever number was read from the EEPROM).

## API Summary

`begin()` Enable the LEDs, on tinyNeoPixel, must be called before show() - not applicable for tinyNeoPixel_Static

`show()` Output the contents of the pixel buffer to the LEDs

`setPixelColor(uint16_t n, uint8_t r, uint8_t g, uint8_t b)` set the color of pixel `n` to the color `r,g,b` (for RGB LEDs)

`setPixelColor(uint16_t n, uint8_t r, uint8_t g, uint8_t b, uint8_t w)` set the color of pixel `n` to the color `r,g,b,w` (for RGBW LEDs)

`setPixelColor(uint16_t n, uint32_t c)` set the color of pixel `n` to color c (expressed as a uint_32 - as returned from getColor())

`getPixelColor(uint16_t n)` Returns the color of pin `n` as a uint_32

`fill(uint32_t c, uint16_t first, uint16_t count)` set `count` pixels, starting from `first` to color `c` which is a 32-bit "packed color". If `first` is unspecified, the first LED on the string is assumed. If `count` is unspecified, or if 0 is passed to it, all the LEDs from `first` to the end of the strip will be set. And if `c` is not specified, it is assumed to be 0 (off) - so `fill()` with no arguments is equivalent to `clear()`.

`setBrightness(uint8_t)` set the brightness for the whole string (0-255). Adjusting the brightness is implemented as multiplying each channel by the given brightness to get a uint16_t, and then taking only the high byte; once brightness has been set, this is done every time pixel(s) are set. Because this process is lossy, frequently adjusting the brightness will lead to quantization errors.

`clear()` clear the pixel buffer (set all colors on all LEDs to 0).

`setPin(uint8_t p)` Set the pin for output; in `tinyNeoPixel_Static`, it is your responsibility to ensure that this pin is set OUTPUT. `tinyNeoPixel` copies the Adafruit behavior, and called pinMode() on it. Be aware

`updateLength(uint16_t n)` Set the length of the string of LEDs. Not available on tinyNeoPixel_Static.

`updateType(neoPixelType_t)` Set the color order and number of colors per pixel. Not available on tinyNeoPixel_Static.

`getPixels()` Returns a pointer to the pixel buffer (a uint_8 array).

`getBrightness()` Returns the current brightness setting (per setBrightness())

`getPin()` Returns the current pin number.

`numPixels()` Returns the number of LEDs in the string

`sine8(uint8_t angle)` Returns the sine of the angle (angle in 256's of a circle, that is, 128 = 180 degrees), from 0 to 255. Used for some animation effects. Because this makes use of a lookup table

`gamma8(uint8_t input_brightness)` Performs basic gamma correction for smoother color transitions, returns a gamma corrected brightness that can be passed to setPixelColor().

`gamma32(uint_32 input_color)` As gamma8, only acts on and returns a 32-bit "packed" color (uint32_t).

`Color(uint8_t r, uint8_t g, uint8_t b)` Return the color `r,g,b` as a "packed" color, which is a uint32_t (For RGB leds)

`Color(uint8_t r, uint8_t g, uint8_t b, uint8_t w)` Return the color `r,g,b,w` as a uint_32 as a "packed" color, which is a uint32_t (For RGBW leds)

`ColorHSV(uint16_t hue, uint8_t sat, uint8_t val)` Return the color described by the given Hue, Saturation and Value numbers as a uint32_t

## Pixel order constants
In order to specify the order of the colors on each LED, the third argument passed to the constructor should be one of these constants; a define is provided for every possible permutation, however only a small subset of those are widespread in the wild. GRB is by FAR the most common.

### For RGB LEDs
```c++
    NEO_RGB
    NEO_RBG
    NEO_GRB
    NEO_GBR
    NEO_BRG
    NEO_BGR
```
### For RGBW LEDs
```c++
    NEO_WRGB
    NEO_WRBG
    NEO_WGRB
    NEO_WGBR
    NEO_WBRG
    NEO_WBGR
    NEO_RWGB
    NEO_RWBG
    NEO_RGWB
    NEO_RGBW
    NEO_RBWG
    NEO_RBGW
    NEO_GWRB
    NEO_GWBR
    NEO_GRWB
    NEO_GRBW
    NEO_GBWR
    NEO_GBRW
    NEO_BWRG
    NEO_BWGR
    NEO_BRWG
    NEO_BRGW
    NEO_BGWR
    NEO_BGRW
```
### Other color orders
You may be thinking - how is that possible? The above list covers every possible permutation! But that is only true when we restrict ourselves to the two most common sets of colors. There are two others (both belived to be based on SK6812) in circulation, both directed towards similar ends. "WWA" is White, Warm White, and Amber (all generated through a phosphor - sp it's basically neutral white, warm white, and VERY warm, white). The order, in the ones I have at least, is Cool White, Amber, Warm White. Use a different color order; then you need only make note in comments something like "GRB order gives  red = Amber, green = CW, and blue = WW for WWA"

The other oddball I've seen is WW ("warm white"? "white/warm white?"). I have not actually mounted those onto a PCB to test out yet, however examination under UV implies that there is only one color of white - a uniform warm-ish white. If anyone has played with them, I'd love to hear how they work - that is, do they have multiple warm white LEDs? Do they take fewer data bytes (potentially only one)? Or is it something else?); until such a time as I get more information, making this library control those shall remain an exercise for the user. If there are a full 3 emitters, it's straightforward to just set colors where all three channels are equal. If they take fewer than 3 bytes, that gets a bit trickier.


## Safety warning
LED lighting projects can pose safety hazards when conncted improperly (generally fire, though the exposed mains terminals on the backs of some power supplies can cause electrocution, brief contact with mains power is usually merely unpleasant) While these concerns are by no means unique to LED lighting projects, such projects often combine high current, shoddy parts, inexperienced designers/programmers and opperation for prolonged periods of time with little or no supervision - so particular care is warranted.

Large LED light strips can draw a surprising amount of current (60mA per LED and up to 144 LEDs/m with some strip = 8.6A/m), and a great deal of discussion has been devoted how and how often power should be injected. What I rarely see mentioned is the limit of the ubiquitous JST-SM connectors that these strips usually use. Those are rated at a MAXIMUM of 3A, when used with 22 gauge wire, and when the terminals involved are actual JST terminals (most from china almost certainly copies). I have seen wires supplying power to light strings get frighteningly hot, such that the red positive wire darkened to the same color as the black ground wire. Further investigation determined that the 2-pin JST-SM connectors with terminals already crimped on and inserted into the housing were of grossly inferior quality. How inferior? Well, the cross-sectional area of the copper conductor is in one batch of them (the ones that failed) is ONE SIXTH of what it is for another sample of 22AWG wire, while another prewired connector used wire with half the diameter of the wire which I think plausibly could be 22 AWG. *All of the samples had AWG 22 printed on the insulation* - an incredibly brazen level of dishonesty in my opinion, especially considering the risk of fire.
In addition to following all the usual safety guidelines, pay particular attention to these:
* Do not overload connectors; check the maximum current rating for any type of connector you're using, and make sure that the worst case scenario remains comfortably below it. Surprisingly few people show much concern about overloading their connectors.
* Always measure the gauge of the wire in a batchof wire or pre-wired connectors; intentionally mismarked wire is ubiquitous on the Chinese market, with thicker insulation to make it look like the supposed gauge. This is even worse than if they just ripped you off and could magically prevent you from realizing it, because the thicker insulation makes crimping on connectors more difficult.
* Always pull-test a few samples from a batch of prewired connectors (most commercial dupont line fails the pull test!).
* When using pre-wired connectors, use a pin extractor to pull out a male and female pin from a given batch and compare them to ones you will be using them with. If they don't match, that is a huge red flag.
* Make sure that any power supply you use will turn itself off in the event of overcurrent, instead of changing to constant current mode. That's great for a benchtop supply in your workshop, but in the field, all it does is waste power generating a bunch of heat wherever the fault is.
* Be certain that the system will deal gracefully with all pixels being written white. I have witnessed malfunctioning code result in this on a number of occasions;
* Consider the use of a resettable circuit breaker (these aren't that expensive) or even a fuse if you are not satisfied with the safety of the supply you are using when you attempt to draw excessive current.
* Consider implememting an automatic shutoff in software the lights are left on for a certain length of time without the settings being changed (or on other criteria depending on what sensors it may have) - cases where the LEDs are at nearly maximum brightness are of greatest concern.
* Are you using a buck or boost converter anywhere? If so, make sure it uses synchronous rectification to minimize the amount of waste heat it generates.

## General notes about tinyNeoPixel and addressable LEDs
This library is supplied with support for speeds up to 48 MHz which is far in excess of what the tinyAVR series can reach (but - amazingly - is within reach of the Dx-series).

### What kind of LEDs will this work with?
All individually addressable LEDs marketed as WS2811/2812/SK6812 or SK6805, as well as those claimed to be compatible with such strips are expected to work. This includes:

#### Flexible PCB strip
Typically made with sticky tape on one side, and 5050 LEDs on the other, or covered in a silicon waterproofing layer for outdoor use. This stuff is incredibly common, and can often be seen in the wild decorating storefronts, and is the least expensive of these options. It looks pretty lousy if you don't use some sort of diffuser, or have the light reaching viewers indirectly (such as locating it in a recessed fixture on the ceiling, or at the top of a glass-fronted display case that is below eye level). The points are just too bright for the tiny area of the visual field that they come from. This complaint is apparently widespread, since there are a number of solutions to this. There are little diffuser caps you're supposed to put over 5050 LEDs, individually. They don't really help. And there is a much more effective method that essentially places the strip within an opaque rubber sleve which directs the light in one direction or another as well as diffusing it; these actually work pretty well, though the added expense is not negligible, the result is bulkier, and these sleeves are available in many crosssections, and it may not be clear which is ideal for a given use case.

There area few notable variations on the LED strip:
* SK6805 COB LED strip - "COB" LEDs are those LEDs wherein the emitter is embedded in some sort of soft rubber, generally featuring large numbers of smaller LEDs (which is more efficient, and looks better). In this case, there is a row of incredibly high density (332/m) of SK6805 LEDs in 2836 packages, hidden under a white rubbery diffuser that leaves the total strip barely thicker than a standard 5050-strip. While it doesn't throw off as much light bnecause of the lower power LEDs, the tight packing of the pattern makes it appear almost continuous. Vendors claim (though I find dubious) that they only need power injected every 3m (at both ends). Incredibly beautiful, but eye-wateringly expensive for a whopping $30/m, and not less than $25/m in quantity.
* Individually addressable 12v LED strip with external IC's and standalone RGB LEDs, usually with three LEDs per IC has each group of LEDs controlled as a group. This means less data needs to be sent for the same number of LEDs, but also limits flexibility. This is not recommended (though see string lights below).
* Individually addressable 12v LED strip with just the LEDs is usually based on the WS2815 (this is likely what you want if going the 12v route - they put three tiny lower current LEDs in series, and drop the currrent they put through them). These also have an extra "bypass" data in and out, so that if one of the LEDs fails completly, |
* Warning: There exist 12v LED strips that are not addressable. These have no relation to this library, they are controlled using beefy mosfets, and all the LEDs on the strip must be the same color. Further discussion of these is beyond the scope of this document. They're no where near as much fun as addressable ones.

#### Rigid PCBs
Pre-populated PCBs full of addressable LEDs are readily available. They are commonly shaped like rings, bars, or squares. These also almost universally use 5050 package LEDs (with the exception of some panels, which use smaller packages to effectivelty make a display). While the LEDs are still not terribly comfortable to look at, the smaller number of them and their confinement to a small part of ones visual field helps.

#### Strings of LEDs with diffusers
These diffusers are  almost always hemispheres, affixed to a round PCB, with two sets of wires coming out opposite sides (imagine taking a round PCB with a LED on it, cutting a pingpong ball in half, and putting that over it. I actually *did* that back before such strings were easy to buy; besides I wanted APA102's for that project). The flat side is usually equipped with adhesive tape for mounting against the wall. Sometimes, each module has a WS2811 and 3 LEDs. These are likely the least common of the types listed here.

#### "String Lights"
These are the only type that is essentially always based on the WS2811: These feature a diffused RGB LED connected to the '2811 driver IC with wires connecting adjacent LED assemblies. Each of those LED assemblies is surrounded in molded (theoretically waterproof). You should probably inject power at the start (or end) of each string of 50. You should use the additional power wires they providel; JST-SM connectors like they use can **barely** handle 3A (the maximum current from 50 LEDs), and you never want to push something right to it's limit. So you want power coming into one of the pairs of power wires on every section.

#### Fairy Lights
These are, as far as I know, a fairly recent innovation. These use thinner wires and MUCH smaller bulbs (looks like a single LED, with wires soldered directly onto it, covered in a blob of diffusing plastic. They come in strings of (typically) 50, 100, and 200. Some of the 50's can be daisy-chained (and include wires to inject power). The longer strings generally cant, and most have an integral USB power connector and controller (which generates impressively ugly patterns). desolder it, or cut the wire after it has transitioned to stranded wire; the solid wire is covered with enamel which is hard to remove). These strings use lower current SK6805 LEDs, which allows the existence of 100 and 200 LED strings with such thin wire and with USB power (though in the case of 200-LED strings, it appears that the controller is limiting the brightness to stay within a power budget). Note also that there are (or at least were) two kinds of these; in both cases, the wire looks fragile, but through totally different mechanisms. The currently dominant version has solid (not stranded) wire. On the one hand, this holds its shape very well and can easily be bent into a desired shape. On the downside, this bending action makes it more likely to break via fatigue, and the wires are also more visible. In the past, I have received a few sets made with a very different wire - incredibly flexible; I think there were other fibers woven in with the copper for improved flexibility; it looked great, but it was eager to knot itself if you did anything suboptimal to it (like putting it onto a reel without spinning the reel on it's axis - if the reel does not rotate on it's axis during rolling and unrolling each wrap introduces a 360 degree twist!).

### Compatible LEDs: SK6812, WS2811, WS2812 - and many more
I am aware of no comprehensive list, or anythign resembling one. There are well in excess of a dozen by now; most of them have no english datasheet, and it is not unusual to see the timing diagram cribbed directly from the WS2812 datasheet. They're usually like the WS2811, ie, they are connected to external LEDs, not integrated with the LEDs themselves in a single package. They sometimes vary in the detail of how the dimming is done (some have constant current, most PWM it like the '2812 - if you care, you probably want an APA102 knockoff), what the maximum voltage the pins can hold off is (some are built to support multiple LEDs connected in series), and at white price they are sold. However, with the exception of the SK6812 and WS281x (as well as the APA102 and it's clones), they are not widely available in the western world (at least not honestly. I have gotten some supposed "WS2812" LEDs in a package that WorldSemi never manufactured them in ("2020" they called it, but it looks like an 0807 dual-LED package... and touts it's longer reset pulse which relaxes timing constraints. That is clearly not a WS2812. Additionally, WS2812 and SK6812 names are used interchangibly when finished devices are sold - regardless of which one is used (usually not the WS2812). One thing that is worh noting is that people, over time, have debated whether SK6812 or WS2812 is better; one does have to admit that SK6812's are available in in RGBW (indeed, with Cool, Neutral and Warm white) and WWA versions, while the WS2812's are only RGB. These are available in nearly every plausible component package: 5050, 3535, at least two drastically different packages both called 2020 by the merchant selling them, and a 1515 (that's 1.5mm on a side). In the other direction, there are 3mm and 5mm through-hole diffused ones, and there are even a few PCBs that pair a WS2811 (or emulator thereof) with drivers for *MONSTER* leds, in the 10-50 watt range.
#### Other notable pixel LEDs that are compatible:
* WS2813 - Like a WS2812, except that they have a bypass line so one damaged one does not take out everythign after it. These may also have a decoupling capacitor located inside the LED itself (it is visible through the transparent window if present). The datasheet indicates that one can (but does not recommend) omit the external decoupling capacitors.
* I have seen an "SK6813"  for sale on aliexpress. It appears to be pin-compatible with the WS2813.... except that the bypass input is marked "NC" (no connection) - implying that the headline feature of the WS2813 is absent, and it's really just a SK6812 with a different pinout.
* WS2815 - As with the WS2813, except here, each "LED" die is actually three emitters connected in series. Thus with one third the current, the power used by the LEDs and their brightness remains constant (as the forward voltage of an LED of 1/3rd the size driven with 1/3rd the current is the same, the current has been reduced by a factor of three, but there are three times as many LEDs, so it evens out) - while the voltage drop os . Of course, 5V * 3 = 15V, not 12V; it turns out that extra volt is only necessary so that as the voltage drops along the length of the strip, there will still be enough voltage to light up the blue LEDs. With only 1/3rd the current (hence 1/3rd the voltage drop). Thus, these are more efficient, require less frequent power injection, and are more reliable. They are, of course, one of the most expensive.

#### Notable LEDs that are NOT compatible
* Anything that is not addressable, such as standard (single color or RGB) LED strip. As far as I have seen, every listing for a WS2812 compatible device has either WS2811, WS2812, or SK6812, and usually all three, in the title of the listing on marketplace sites like EBay, AliExpress, and Amazon.
* Anything with a data AND a clock line - APA102, APA107 SK9822, HD107 and similar. These are all great LEDs - they implement 5 levels of dimming, in some cases using actual constant current sources instead of a superimposing a second PWM frequency!  Because of that, and because they generate PWM at a higher frequency they are suited for persistance-of-vision dependent art without looking like dashed lines. Since there is both a clock and data line the transfer rate is governed by the controller and the maximum specified by the LEDs. These maximums are often high enough that in practice, the limit of the wiring will be encountered first unless great care is taken in the wiring. There exist multiple other libraries for these, since they opperate via SPI (be it software or hardware SPI), no special measures are required to make those libraries compatible with AVRxt. More care and research is needed when buying these, too, since the specific LED, in some applications, actually makes a big difference, even though there is a control scheme which works for all of them (though not the HD108, which wants 8 bytes per LED instead of 4, the 16-bit color, is completely incompatible with everything else, which is likely why it doesn't seem to have set the world on fire like the WS2812 and APA102 did. )

#### Bad pixels
They do exist, and travel in groups. Apparently when these were relatively new there were a LOT of bad batches of them making the rounds; the root cause, apparently, was that the transparent material atop the die and LEDs was not making an airtight seal, and corrosion would rapidly destroy them at that point. Bad batches do exist, particularly in old stock - but on the other hand, this problem seems to be considerably less common than it was, and they are now uncommon. At the peak of the pixel plague (as in a plague that afflicts pixels, not a plague of pixels; the latter is ongoing with no sign of slowing), the "wisdom of the internet" was that the SK6812 LEDs were in fact more reliable than WS2812's of that era. As I said, I think WorldSemi (assuming they even still make them, and didn't throw their hands up in despair over the clones) has gotten their process sorted out.

#### SK6805 vs SK6812
There exists a very similar led to the SK6812, called the SK6805. There are, as far as I can tell, two key differences. Physically, SK6805 is typically smaller - the smallest I have seen is 1.5 x 1.5mm (yes, you read that right, you can fit a 3x3 array of them in the footprint of a standard 5050 LED), while 3.5x3.5 is very common, and 2836 is used in "COB" led strips.

### Library name
While this library was initially created in order to both ensure compatibility with, and through the Static version, fit within the flash and memory constraints of the tinyAVR line of parts, **this library is entirely suitable for non-tiny devices**; It offers all the functionality of the Adafruit version on the library, with the addition of the Static mode. Where appropriate, core-specific matters are accounted for (not relevant for modern AVR versions). This library is code compatible with the version distributed with ATTinyCore, though that has additional dependencies on menu options, and the implementation of `show()` has significant differences.

Dynamic allocation has no place on an 8-bit microcontroller. Since almost all of the other pixel libraries *dont* work, considering the popularity of WS2812 LEDs, I decided it would be improve the UX with the core to distribute a known working '2812 library with a standard API along with the cores.

### Comparison with other WS2812 libraries
Both FastLED and Adafruit libraries have the buffered design which - while expensive in terms of RAM, you need it for arbitrary control over the colors of each pixel. Other schemes can be used for cases where the patterns are simple, at substantal memory savings (an example that supports these parts is [FAB_LED](httpsgithub.com/sonyhome/FAB_LED)), and while those are very valuable in applicable situations, I wanted something that could take the place of a buffered, full-service LED copntrol library. That meant that I should be modifying either FastLED or AdafruitNeoPixel (ideally keeping the API nearly identical); Opening the source code for the libraries, I found the Adafruit one to be readily understood, simple, with one purpose that it handles capably, and extensively commented, especially the assembly. Meanwhile FastLED was largely unreadable. I felt that the code suffered from "kitchen-sinkism" - when the author throws feature after feature, up to and including the kitchen sink, into a gigantic omnibus library. I had yet to locate the code that actually generated the output in the FastLED source within the length of time it took to modify the Adafruit library to achieve what I needed in the first tinyNeoPixel release.

### New Adafruit additions
If Adafruit has added new methods to their library, please report via an issue in one of my cores that ships with this library so that I can pull in the changes.

### License
Unlike the core itself (as noted within the library files), tinyNeoPixel is licensed under LGPL 3, not LGPL 2.1, since the Adafruit library was always LGPL 3.
