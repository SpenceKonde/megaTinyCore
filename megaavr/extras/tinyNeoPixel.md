# tinyNeoPixel - a modern AVR (AVRxt) compatible library for WS2812 "NeoPixel" and similar
**for tinyAVR 0/1/2-series, megaAVR 0-series, AVR Dx-series and future modern AVRs**

## Background
The change in architecture from the classic AVRs (which use the AVRe or AVRe+ variant of the AVR instruction set) to "modern" AVRs (which use AVRxt) causes many of the existing WS2812 libraries to not work on the new parts. This library adapts the tinyNeoPixel library to work with AVRxt version; this has slightly improved execution times, among other things, and adds support for additional clock speeds: 10 and 20 MHz because of it's use on the tinyAVR 0/1/2-series and megaAVR 0-series, and faster speeds all the way up to 48 MHz (At room temperature, at least some extended temperature range Dx-series parts can run at that speed. It's not as surprising as it seems at first glance: don't forget that the extended temp Dx parts are spec'ed for use in life safety critical applications at 125C). Unlike the version for classic AVR (AVRe/AVRe+), one of the benefits of the improved timing is that ST (indirect store) executes in 1 clock cycle, rather than 2, so there is no need for multiple conditionally included implementations that were needed before (so thy could use the single-cycle OUT instruction, whose target register must be known at compile time and which must be one of the lowest 64 register addresses). The version included with DxCore and megaTinyCore only supports the modern AVR part families, but has no processor or core specific code beyond that.

## Library name
While this library was initially created in order to both ensure compatibility with, and through the Static version, fit within the flash and memory constraints of the tinyAVR line of parts, **this library is entirely suitable for non-tiny devices**; It offers all the functionality of the Adafruit version on the library, with the addition of the Static mode. Where appropriate, core-specific matters are accounted for (not relevant for modern AVR versions). This library is code compatible with the version distributed with ATTinyCore, though the implementation of `show()` has significant differences (because those parts are AVRe, the code is more like Adafruit's original - albeit with far more frequencies supported).

Like the normal Adafruit_NeoPixel library, this supports WS2811/WS2812/SK6812 and all the nominally compatible single-wire individually addressable LEDs. Unlike that library, support for the really old first generation ones that use the 400 kHz data rate was dropped. I have not seen those for sale in ages, but they do make the library larger. It is also not compatible with the ones that use both data and clock lines, like the APA102 or the even more numerous (global dimming adds another and competition over pwm frequency for POV systems added more ways to differentiate their copies) knockoffs thereof; libraries for those do not pose special compatibility issues with AVRxt, and there is no need to reinvent, modify, or re-imagine the wheel. See the section on compatible LEDs for details on what this library supports.

### Version numbers
The version numbering may be confusing if encountered without warning. There are two active "branches" of this library.
* Version 1.5.x - (released 2022) These versions are included with ATTinyCore 2.0.0 and are the first versions to not require the tools submenu to select the port on classic AVRs. They also include corrections to a number of silent problems noticed and corrected in 2.0.4 relating to the ASM opperand constraints.
* Version 2.0.x - (released 2018-2019) These versions support the modern AVRs (those released in 2016 and later) with the improved instruction timing. The assembly that actually outputs data is not the same. Almost everything else is, and the two branches will track each other's changes.
* Version 1.0.x - This version predates either of those. It is obsolete and contains significant defects and significant limitations. It should not be used.

### Compatibility between library versions
All code written for an earlier version of the library will work when using a later version, provided that version is appropriate for the architecture. 1.5.x and 2.x.x are fully code compatible - the only difference between them is the details of the assembly routines that actually output the data, as the depend on the instruction timing, which changed between AVRe/e+ and AVRxt. Code written for newer versions is expected to work on on 1.x.x, provided it does not use any of the more recently added functions from Adafruit's version (which are only in 1.5.x and 2.x.x). Versions older than the latest one may have significant bugs, and we only provide support for the latest versions from each branch.

Code written for the Adafruit library can be converted to tinyNeoPixel by simply changing the class name and the name of the library, or to the superior Static version with further alteration of one call (the constructor) and the addition of a single line of code, provided that updateLength and updateType are not used/

### Other compatibility notes
* This supports WS2811/WS2812/SK6812 and the nominally compatible single-wire individually addressable LEDs (there are literally dozens of them)
* Support for the really old first generation ones that use the 400 kHz data rate was dropped. I have not seen those for sale in ages and doubt they are still readily available but the optimizer is not clever enough to optimize away the 400 kHz stuff when it isn't used.
* It is not compatible with any LEDs that use both data and clock lines, like the APA102 or the even more numerous (global dimming adds another variable, so the variety of clones is even larger) knockoffs thereof.
  * Libraries for those do not pose special compatibility issues with AVRxt, since the output is clocked not timing-critical, and there is no need to reinvent, modify, or re-imagine that wheel. See the section on compatible LEDs for details on what this library supports.

### Supported F_CPU speeds

| Version  |  1.0.x |  1.5.x |  2.0.x |
|----------|--------|--------|--------|
|    4 MHz |     No |     No |  $ Yes |
|    5 MHz |     No |     No |  $ Yes |
| 6.40 MHz |     No |     No |  $ Yes |
| 7.37 MHz |  ! Yes |    Yes | Likely |
|    8 MHz |  ! Yes |    Yes |    Yes |
| 9.21 MHz |  ! Yes |    Yes | Likely |
|   10 MHz |  ! Yes |    Yes |    Yes |
|11.21 MHz |    Yes |    Yes | Likely |
|   12 MHz |    Yes |    Yes |    Yes |
|14.72 MHz |    Yes |    Yes | Likely |
|   16 MHz |    Yes |    Yes |    Yes |
|18.34 MHz |    Yes |    Yes |  Maybe |
|   20 MHz |    Yes |    Yes |    Yes |
|   24 MHz |   * No |   * No |    Yes |
|   25 MHz |   * No |   * No |    Yes |
|   30 MHz |   * No |   * No |    Yes |
|   32 MHz |   * No |   * No |    Yes |
|   36 MHz |   * No |   * No | @@ Yes |
|   40 MHz |   * No |   * No |  @ Yes |
|   44 MHz |   * No |   * No | @@ Yes |
|   48 MHz |   * No |   * No |  @ Yes |

#### Notes on table

* `*` - Unsupported because Classic AVR parts do not overclock nearly as well as Modern AVRs.
* `!` - Requires ATTinyCore because the port must be specified in a tools menu.
* `@` - This speed has not been tested in 2.0.5 against a full panel of LEDs (SK6812, WS2812, WS2811). (testing on 1.5.x will be done as part of ATTC release)
* `@@` - This speed has not been tested in 2.0.5 at all. (testing on 1.5.x will be done as part of ATTC 2.0.0 release)
  * 2.0.5 has been tested against SK6812, WS2812B, WS2812, and WS2811 on all parts not marked with one of the above two markings.
* "Likely" indicates a speed on a modern AVR that we expect will work as the next closest speed will be used, and is generally close enough. However there is no longer a need to use wacky USART crystal speeds on modern AVRs due to the new Fractional Baud Rate Generator featured on the USARTs.
* "Maybe" As above, but with less confidence.
* `$` This *has* actually been tested, yes, it really works. No, I cannot imagine why you would want it. Neopixels won't run at less than around 3.5V. The power you save will be laughable compared to the power consumption of the LEDs. Even if the LEDs are off.
* At time of publication, 2.0.5 has beem tested at the marked speeds. I have not used any of the small handful of 36 or 44 MHz crystals I bought.
* As long as the clock speed is the same, there is *only one thing* that determines if the same assembly routine will work the same way on two AVRs: What version of the AVR instruction set the chips use.
  * Versions 1.x.x are suitable for **AVRe** (tinyAVR) and **AVRe+** (megaAVR - nearly identical to AVRe, plus hardware multiplication and support for larger flash sizes).
    * **AVRe** tinyAVRs are all parts named ATtiny followed be a number which does not have both 0, 1, or 2 in the 10's place and a 2, 4, 6, or 7 in the 1's place (though it may have one or the other). The number may or may not be followed by a letter.
    * **AVRe+** megaAVRs are all parts named ATmega followed by a number which does not have a 0 in the 10's place and an 8 or 9 in the 1's place. These may have one or more letters at the end or in the middle of the part number.
    * Other, older, **AVRe+** parts are also usable, these include the AT90 series, for example.
  * Version 2.x.x is suitable for **AVRxt** (tinyAVR 0/1/2-series, AVR Dx/Ex, megaAVR 0-series).
    * **AVRxt** tinyAVRs are named ATtiny followed by a number starting with the flash size in kb which is not 1k, followed by a 0, 1 or 2 indicating featureset, and ending in a 2, 4, 6, or 7 indicating the pincount (8, 14, 20 and 24).
    * **AVRxt** megaAVRs are named ATmega followed by a number starting with the flash size in kb, then a 0, and ending in either an 8 (indicating 32 pins) or 9 (indicating 48 pins).
    * AVR Dx/Ex parts, which are all **AVRxt** are named systematically: AVRsssXXpp; replace sss with the flash size in kb (ranging from 128k on top-end DA/DB to 8k on low end Ex-series), XX is the family, consisting of 2 capital letters, the first of which is either a D or an E. This indicates which set of specs it has on a large scale, while the second letter (They seem to be going in sequence, skipping any that form confusing abbreviations - ie, there's no "DC", but there might very well be an EC in the future, unless the part has a feature so prominent that it warrants a letter referencing it, like the native-USB DU-series, as yet unreleased). Finally, pp is the pincount, which thus far has ranged as high as 64 on the largest DA/DB, to as low as 14 on the smallest DD/DU/EB-series parts.
  * Looking over the instruction timing, I suspect that the 2.x.x routines would work on AVRxm (xMega) if there was an xMega Arduino core. However, no such core exists, and xMega is likely dead as a product line, and I do not expect to see an xMega Arduino core. While AVRxt boasts several significant improvements in this area, they don't matter for this purpose.
  * AVRrc (reduced core) parts are not supported by any version of this library and there are no plans to change this. These parts 1) suck, 2) have miserable instruction timing (I do not think 8 MHz could be achieved), and 3) lack the flash required anyway.
    * AVRrc includes the ATtiny 4/5/9/10, and the ATtiny102/104 (which despite the numbering, are not smaller-flash versions of the 202 and 204, but rather vastly inferior parts more similar classic AVRs, except worse). They aren't even much cheaper.
    * AVRrc is a lobotomized version of the instruction set, entirely inappropriate for controlling individually addressable LEDs or for use with Arduino. In fact, I recommend against using these parts at all.

### Licence
Unlike most of the core, tinyNeoPixel is licensed under LGPL v3 rather than 2.1. The Adafruit library was LGPL v3, and LGPL versions may not be downgraded to earlier ones. LGPL v3 prohibits "tivoization"; I am opposed to that prohibition for tightly constrained embedded systems, because their limitations could plausibly make it impossible to comply with short of open sourcing the entire codebase.

## tinyNeoPixel and tinyNeoPixel_Static
There are two versions of this library provided. `tinyNeoPixel` implements the entire API that Adafruit_NeoPixel does, including setting the length and type of LEDs (color order and whether it's RGB or RGBW) of the string at runtime. This provides maximum portability between code written for use with Adafruit_NeoPixel and tinyNeoPixel (only the constructor and library name need to be changed) - however, the memory used to store the "frame buffer" (the color of every pixel) is not included in the SRAM usage displayed when compiling a sketch because it is "dynamically allocated". This can be a large portion of the memory available on smaller parts (I have seen support inquiries from people trying to control a string of LEDs which would require several times the total memory of the part for this alone); trying to allocate a larger array than will fit in RAM does not generate any sort of error - it just fails to allocate anything at runtime, nothing gets sent to the light string, and no LEDs turn on. This dynamic memory allocation also requires compiling in malloc(), free(), and associated functions; on parts with small flash (ie, tinyAVR), this can be significant. Finally, dynamic memory allocation is arguably bad practice on small embedded systems like the AVR devices.

`tinyNeoPixel_Static` is slightly cutdown, removing the option to change the length or type of pixels at runtime (which relies on dynamic memory allocation). Instead it requires the user to manually declare the pixel array and pass it to the tinyNeoPixel constructor. **Additionally, it does not set the pinMode of the pin (the sketch must set this as OUTPUT);** - the reason for this was that it is a no-holds-barred attempt to cut the flash consumption as much as possible, Removing that last pinMode cuts out over 100 bytes, which is nothing to sneeze at on the tiniest of tinies! Note that the savings requires eliminating *all* calls to pinMode and replacing them with writes to`VPORTx.DIR` registers (if you have to set only a small number of bits in one of those, and everything is known at compile time, you're often better with several consecutive `|=` operators each setting one bit, because those end up as a single-word SBI or CBI - while `|=` for more than 1 bit requires a read/modify/write (IN, ORI, OUT, which must be done with interrupts disabled if any interrupts modify the same register. Finally, no call to begin() should be made, nor can it be made. These changes reduce sketch size and provide greater visibility on the memory usage. **Unless you need to change string length or type at runtime, it is recommended that `tinyNeoPixel_Static` be used** - it uses less flash, particularly if you don't use malloc elsewhere (I try to avoid it, and you should too - the core certainly doesn't use it except for the String class (which should also be avoided), nor, to my knowledge, do any of the included libraries).

## API extension reference
This documents only differences between the static and dynamic allocated versions, and between tinyNeoPixel and the Adafruit library. Adafruit has done a better job of documenting their functions. Other that support for more speeds and parts, and the loss of 400 kHz support, the only things changed between the Adafruit library and tinyNeoPixel are the name of the library, the name of the class, and non-user-facing asm routines hidden in `show()`. All method names are identical.

### Constructors
The constructor is the declaration that you call outside of any function, to create the global tinyNeoPixel object. The example sizes were recorded on megaTinyCore 2.4.3 on an ATtiny1624 @ 16 MHz

`tinyNeoPixel(uint16_t n, uint8_t p, neoPixelType t=NEO_GRB)` - for `tinyNeoPixel` only.
```c++
#include <tinyNeoPixel.h>
#define NUMLEDS 100
tinyNeoPixel leds = tinyNeoPixel(NUMLEDS, PIN_PA6, NEO_GRB);
void setup() {
  leds.begin();
  leds.setPixelColor(0,255,0,0); // first LED full RED
  leds.show();                   // LED turns on.
}
void loop() {/* empty loop */}
```
This example uses 1946 bytes of flash, and reports using only 34 bytes of RAM (it actually uses 334!). If you didn't have enough free memory the call to leds.begin() would fail and the LEDs would not be enabled. No facility is provided detect that begin() didn't.

`tinyNeoPixel(uint16_t n, uint8_t p, neoPixelType t=NEO_GRB, uint8_t *pxl)` - for `tinyNeoPixel_static` only - note that the class name is the same, only the library name changes. Also notice how the `pixels` array is handled: It is *declared* as an array. But it is passed without subscripting, thus passing a pointer to the start of the array. Take care to ensure that the number of leds you pass times the number of channels (3 or 4) match what you pass to the constructor. This is C++, it doesn't check array bounds. I strongly recommend
```c++
#include <tinyNeoPixel_Static.h>
#define NUMLEDS 100
byte pixels[NUMLEDS * 3];
tinyNeoPixel leds = tinyNeoPixel(NUMLEDS, PIN_PA6, NEO_GRB, pixels);
void setup() {
  /* less efficient, more obvious method */
  pinMode(PIN_PA6, OUTPUT); //pinModeFast() now exists and would shave a few more bytes off if this and any other instances of pinMode were converted to pinModeFast.
  // see below for ways to make this even smaller.
  // No begin - any class member at any address,
  leds.setPixelColor(0, 255, 0, 0); // first LED full RED
  leds.show();                   // LED turns on.
}
void loop() {/* empty loop */}
```
The equivalent example with the Static version uses only 968 bytes and reports (accurately) that it uses 325 bytes of RAM. While you can't run many leds with them, you can still cram in the basics on 2k tinyAVR 0/1-series parts (note that, with a mere 128b of ram, it's implausible that a buffer for more than 32 LEDs would fit on the 2k parts after allowing for the rest of the sketch's RAM use). So, that `pinMode(PIN_PA6, OUTPUT);` can be done better. The normal recommended way for users on on mTC/DxC is to just replace it with pinModeFast - as long as the arguments are both compile time known constants,, if the second one is OUTPUT,it takes 1 clock, while either of the

```c
  /* More efficient, but annoying method. Does not depend on any API calls */
  // VPORTA.DIR |= (1 << 6);
  /* Similarly efficient - but much more readable - mTC and DxC only at this point (though digitalWriteFast() is more common)*/
  // pinModeFast(PIN_PA6, OUTPUT);
  // Both of these faster variants depend on you knowing at compile time which pin it's going to be using. In the first version
  // you *must* know the port and bit at compile time if you want a safe, atomic single clock bit write, and you must set or
  // clear 1 and only 1 bit. If you act on more than 1 bit, the compile will fall back to a normal RMW operation, so for
  // VPORTA.OUT |= 0x03 would render as: in Ra, 0; ori Ra, 0x03; out 0, Ra; where Ra is an upper working register (r16-r31)
  // (note: VPORTA.DIR is located at address 0). Unlike the case of setting or clearing a single bit in low I/O, this is
  // not atomic, so if any interrupt writes the same register, it would have to be guarded by disabling interrupts during the
  // write. You should *never* put yourself in that situation - if you wanted to set, say, PA0 and PA1 output in one operation,
  // PORTA.DIRSET = 0x03 rendeds as ldi 0x03; sts 0x0401, 3; - that's still 6 bytes and 3 clocks (STS is a 32-bit instruction)
  // but because there's no read involved, just a ___SET/CLR/TGL register being written, it can't "lose" the write made in the
  // interrupt.
  // pinModeFast() is limited by the same compile-time-known-constant requirement for the pin, however, it allows you to use
  // the pin number, and (at compile time, during the LTO step) it looks up the port and bit of the pin number. The same goes
  // for the value, and with that info can generate the correct single instruction to set it (or, if it's an input, it's an
  // extra 5 clocks to turn the pullup on or off (this is not atomic - but for this to cause a problem, you have to use it
  // in non-interrupt context, and write to the PORTx.PINnCTRL register for the same pin within an interrupt, which is an atypical
  // use case)
  // So pinModeFast() is **actually** the recommended way to do a single pin (though if you're desperate for space, you can
  // combine "direction" and "output" changes that happen at the same time, on a single port, into one port write (though not
  // pullup. Though if you know you want the pullup on and all other option on those pins, you can turn on the pullup for n pins
  // in 2n+1 clocks and as many words with just a dead simple construction like: PORTx.PINaCTRL = 0x08; PORTy.PINbCTRL = 0x08;
  // and so on for all pins (note, linebreaks were omitted for space. We do not condone that practice on real code. )
  // can do it, since pinModeFast needs to)
  ```


`tinyNeoPixel()` -  Empty constructor is available for `tinyNeoPixel` only - for when you won't even know the type of LEDs, or how many of them, until your sketch starts running. You must set pin, type, and length later with `setPin()`, `updateLength()`, and `updateType()` before you can control any LEDs.
```c++
#include <tinyNeoPixel.h>
volatile uint16_t numleds=100;        // Declaring these volatile prevents the constants from being optimized.
volatile neoPixelType led_type;       // This is the easiest way to simulate not knowing what you're driving until runtime.
volatile uint8_t neopin = PIN_PA6;    // Maybe you read from EEPROM, maybe you have jumpers or switches, etc.
tinyNeoPixel leds = tinyNeoPixel();   // empty constructor...
void setup() {
  leds.updateLength(numleds);         // Set the length
  leds.updateType(led_type);          // Set the type
  leds.setPin(neopin);                // set the pin
  leds.begin();                       // now we've setup everything, and must call begin().
  leds.setPixelColor(0, 255, 0, 0);   // first LED full RED
  leds.show();                        // LED turns on.
}

void loop() {/* empty loop */}
```
This compiles to 2204 bytes and reports 39 bytes of RAM used as well (actually uses 339).

### API Summary
A very brief summary of the API functions present in and essentially unmodified (at least in user/programmer facing areas) follows.

#### Basic Functionality

`begin()` **tinyNeoPixel only!** This must be called after all configuration to enable data output through this library. It sets the pin `OUTPUT` and `LOW`.

`begin()` **tinyNeoPixel_static only!** This method *does not exist* on tinyNeoPixel_static, because that version of the library requires all of the information that begin would require in the constructor. Furthermore **tinyNeoPixel_static neither sets the pin direction nor makes sure it's set LOW** - on the static version, it is the responsibility of the user to not only supply the pixel buffer, but also to ensure that the pin passed to the constructor is set as an OUTPUT, and is LOW (the latter only needs to be checked if you've done something else with the pin - all pins always start up set `INPUT` and `LOW`). This permits highly space constrained contexts (which the statically allocated version was designed to work with) to save not only the aprx. 1.3k associated with malloc/free/etc, but - assuming the rest of the sketch has been written aggressively w/rt flash, and in particular, that one of these functions is called nowhere else in the application: pinMode(), digitalWrite(). If none of pinMode, digitalWrite, openDrain, digitalRead are called from the sketch (including indirectly) a small amount of additional space cam be saved.

`show()` Output the contents of the pixel buffer to the LEDs. Note - this involves vanishing into a disabled interrupt black hole for a long peroiod of time by computer standards. If CORE_SUPPORT_NUDGE_MILLIS >= 1, calls

`show(uint16_t n)` **New Feature 2.0.6+, > 4k of flash only** Output the contents of the first n pixels in the buffer to the LEDs. This doesn't add much overhead, but it does add a little bit - so to avoid causing problems for those on very low flash tinyAVRs, who have a hard enough time already we won't build this weird feature there.

`setPixelColor(uint16_t n, uint8_t r, uint8_t g, uint8_t b)` set the color of pixel `n` to the color `r,g,b` (for RGB LEDs)

`setPixelColor(uint16_t n, uint8_t r, uint8_t g, uint8_t b, uint8_t w)` set the color of pixel `n` to the color `r,g,b,w` (for RGBW LEDs)

`setPixelColor(uint16_t n, uint32_t c)` set the color of pixel `n` to color c (expressed as a uint_32 - as returned from getColor())

`getPixelColor(uint16_t n)` Returns the color of pin `n` as a uint_32

`fill(uint32_t c, uint16_t first, uint16_t count)` set `count` pixels, starting from `first` to color `c` which is a 32-bit "packed color". If `first` is unspecified, the first LED on the string is assumed. If `count` is unspecified, or if 0 is passed to it, all the LEDs from `first` to the end of the strip will be set. And if `c` is not specified, it is assumed to be 0 (off) - so `fill()` with no arguments is equivalent to `clear()`.

`setBrightness(uint8_t)` set the brightness for the whole string (0-255). Adjusting the brightness is implemented as multiplying each channel by the given brightness to get a uint16_t, and then taking only the high byte; once brightness has been set, this is done every time pixel(s) are set. Because this process is lossy, frequently adjusting the brightness will lead to quantization errors. This is a generally dubious feature/functionality, though it can substantially reduce performance of writing to pixels if used. That is not an issue if you simply write lower values to the pixels.

`clear()` clear the pixel buffer (set all colors on all LEDs to 0).

`setPin(uint8_t p)` Set the pin for output; in `tinyNeoPixel_Static`, it is your responsibility to ensure that this pin is set OUTPUT. `tinyNeoPixel` copies the Adafruit behavior, and calls pinMode() on it, with the addendant overhead (very little if you use pinMode numerous times, and hundreds of bytes if it's the only non-fast I/O you have.)

`updateLength(uint16_t n)` Set the length of the string of LEDs. Not available on tinyNeoPixel_Static.

`updateType(neoPixelType_t)` Set the color order and number of colors per pixel. Not available on tinyNeoPixel_Static.

`updateLatch(uint16_t time)` **tinyNeopixel and tinyNeoPixel_static only** On some pixels, the time needed for the colors to latch is as short as six microseconds. While you aren't going to notice 50 us delays when using 6us latching pixels, if the pattern is small and you're sending it as fast as you can, if you have 250us pixels, you might not have 250 us pauses. That would prevent the LEDs from latching properly (or if there is never a long enough pause, at all). Remember 250 us = up to 12000 system clock cycles (on the highest speed overclocked part we support, an AVR Dx-series @ 48 MHz). This function will set the number of microseconds, minimum, after one call to send() has finished before calling send again will have any effect. *This feature is not available if millis is disabled, because that breaks micros, and micros is how we time this.*

It is debatable whether the correct solution to canShow() having had a fixed value inconsistent with reality was to add this method; I think the two options for this would be to either:
1. Implement updateLatch() as described above.
2. Remove the canShow() interlock entirely.

Whether the correct choice was made depends largely on a piece of information I don't know: A histogram showing the fraction of such LEDs on the market with various latching times. I only know that "some" are 6us, "some" are 200, and "some" are 250. I do not know if there are any with latch times between those extremes, including whether 50 us latch time pixels even exist. My reasoning is as follows:
* The latch time is relevant only when a complicated pattern is not being generated. 250 us is 5000 clocks at 20 MHz and 12000 clocks at 48 MHz. At a bare minimum every LED requires three clock cycles to generate a "pattern" for - but this just gives a single color down the whole strip - is 3 clocks per LED to 'store' 3 8-bit values to RAM with `st X+` or similar, plus the loop overhead, which consists of a sbiw and a brneq which usually follows the branch - a total of 7 clocks per LED. A "push" that moves a pixel down strip by one spot requires an additional 6 above that (3 x 2 clock `ld Y+`) for a total of 13 clocks/led. Thus, for example, for 250us LEDs, if all you did was push or pull pixels, you'd need >900 pixels to exceed the wait time. Hence, we would expect many people to run into this unless either:
  * Everyone or almost everyone has some sort of delay in their code that prevents show() from being called too often, separate from canShow (since canShow would fail to prevent you from calling it during the latching pause for parts with latch delays longer than 50 us).
  * Everyone or almost everyone has much, much slower code to generate the pixel data (if you're achieving the pixel writes with setPixel, for example, that will greatly increase executions time

`uint8_t getPin()` Returns the current pin number; This will be 255, aka `NOT_A_PIN` if no pin has been set, or if an attempt was made to set a pin, but the pin number passed in was not a valid pins. Note that this function is very rarely needed, since the vast majority of cases - one way or another - supply the pin number through an integer constant. Unless you are doing something weird where the pin number is determined and set at runtime, it is more performant to to simply put the number into a preprocessor macro or const uint8_t variable, and use that.

`numPixels()` Returns the number of LEDs in the string. This is another function that just gives you back a value you passed the constructor, and which one rarely needs to use.

`getPixels()` Returns a pointer to the pixel buffer (a uint_8 array); note that this is the same pointer that you passed the constructor if using tinyNeoPixel_Static, and so calling this function will just get a pointer to a global variable that you already defined, and could have used directly. However, if using the non-static tinyNeoPixel, which dynamically allocates that buffer, if you want to write to it directly instead of through the neopixel setColor functions (for example, because you are CPU-bound and need to aggressively optimize the pixel drawing code), this is how you get a pointer to it.

`getBrightness()` Returns the current brightness setting (per setBrightness())

`sine8(uint8_t angle)` Returns the sine of the angle (angle in 256's of a circle, that is, 128 = 180 degrees), from 0 to 255. Used for some animation effects. Because this makes use of a lookup table

`uint8_t gamma8(uint8_t input_brightness)` Performs basic gamma correction for smoother color transitions, returns a gamma corrected brightness that can be passed to setPixelColor().

`uint32_t gamma32(uint_32 input_color)` As gamma8, only acts on and returns a 32-bit "packed" color (uint32_t).

`uint32_t Color(uint8_t r, uint8_t g, uint8_t b)` Return the color `r,g,b` as a "packed" color, which is a uint32_t (For RGB leds)

`uint32_t Color(uint8_t r, uint8_t g, uint8_t b, uint8_t w)` Return the color `r,g,b,w` as a uint_32 as a "packed" color, which is a uint32_t (For RGBW leds)

`uint32_t ColorHSV(uint16_t hue, uint8_t sat, uint8_t val)` Return the color described by the given Hue, Saturation and Value numbers as a uint32_t

#### Another few words on setBrightness()
setBrightness was, IMO, a terrible idea. I mean it's a great idea when you aren't thinking about the implementation and its impact on performance. It's not a terrible idea you're planning to call it less often than new values for all pixels are calculated (otherwise you get quantization error - draw a smooth rainbow pattern for example, then do setBrightness(2); setBrightness(255) to see what I mean). It also tempts users into thinking it's a good idea to setBrightness to a lower value, and then write brightnesses from 0 to 255, and think they have 255<sup>3</sup> distinct combinations. Nope, there are only brightness<sup>3</sup>

## Pixel order constants
In order to specify the order of the colors on each LED, the third argument passed to the constructor should be one of these constants; a define is provided for every possible permutation, however only a small subset of those are widespread in the wild. GRB is by FAR the most common. No, I don't know why either, but I wager there it wasn't random; the human visual system does some surprising things with light and color, and mankind has been figuring out how to make the most of those unexpected factors since we first started painting on cave walls.

### For RGB LEDs
```c++
    NEO_RGB /* Less common than you'd think */
    NEO_RBG
    NEO_GRB /* Very common */
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
You may be thinking - how is that possible? The above list covers every possible permutation! But that is only true when we restrict ourselves to the two most common sets of colors. There are two others (both believed to be based on SK6812) in circulation, both directed towards similar ends. "WWA" is White, Warm White, and Amber (all generated through a phosphor - sp it's basically neutral white, warm white, and VERY warm, white). The order, in the ones I have at least, is Cool White, Amber, Warm White. Use a different color order; then you need only make note in comments something like "GRB order gives red = Amber, green = CW, and blue = WW for WWA"

The other oddball I've seen is WW ("warm white"? "white/warm white?"). I have not actually mounted those onto a PCB to test out yet (I only bought 10); examination under UV confirms only a single kind of phosphor, but the WWA's only have two phosphors, and manage to generate cool and warm white from one of them. Perhaps the thickness of the phosphor layer or location of the die within it is the key. If anyone has played with them, I'd love to hear how they work. I expect they would either be 2-color, LEDs taking 2 bytes each, or single color ones taking a single byte of data each); until such a time as I get more information, making this library control those shall remain an exercise for the user. If there are somehow 3 emitters and it takes 3 bytes, handle as per WWA. If there are 2 emitters, call them RGBW leds and halve the number of "pixels" (I would also use the static version of the library and write directly instead of using the API functions. )

## Refresh Rate
Data is sent at 800 kbit/s, or 100k byte/s - 33,333 LEDs/s. Latching takes 50us of idle time, which is pretty small (only long enough for 5 LEDs of data). Note: Although show() guarantees that it can't be called for 50 us after it last returned. This check relies on micros() and is disabled when millis timekeeping is not used or is based on the RTC (hence no micros() is available) as well as on chips with less than 8k of flash (since micros() is painfully large to pull in). A warning is issued in these cases, and it's up to you to ensure that there's a brief pause between calls to show() if you aren't doing something else, like updating the colors of the pixels you're going to send (and if you aren't doing that, why are you sending data again?!). There do exist a few "WS2812" leds with some other chip in them that waits 200us to latch. These are rare, but if you end up using them for some reason, you may need to add a brief delay between calls to show(). On the other hand, apparently most WS2812's will latch the colors after only a few microseconds, so this issue may be largely academic.

## Supported LEDs and LED asssemblies
All individually addressable LEDs marketed as WS2811/2812/SK6812 or SK6805, as well as those claimed to be compatible with such strips are expected to work (supposedly P943S P9411 SM16703 UCS1903 TM1804 are some examples) are expected to work. I am aware of no comprehensive list. Once the WS281x's got popular, everyone started cloning them. There are well in excess of a dozen by now - most of have no english datasheet, and it is not unusual to see a timing diagram cribbed (in some cases literally an exact copy) the WS2812 datasheet. They're usually like the WS2811, ie, they get connected to external LEDs, not integrated with the LEDs themselves in a single package. They sometimes vary in thresholds of their timing (I have, for example, seen cases where a controller would only drive one of the two types, but I've never seen one that couldn't use the output of other "2812-alikes") what the maximum voltage the pins can hold off is (some are built to support multiple LEDs connected in series), and at what price they are sold. However, with the exception of the SK6812/6805 and WS281x (as well as the APA102 and it's clones), they are not widely available in the western world (at least not described accurately. I have gotten some supposed "WS2812" LEDs in a package that WorldSemi never manufactured them in ("2020" they called it, but it looks like an 0807 dual-LED package to me and touts it's longer reset pulse which relaxes timing constraints. That is clearly not a WS2812. Additionally, WS2812 and SK6812 names are used interchangibly when finished devices are sold - regardless of which one is used (usually not the WS2812). One thing that is worh noting is that people, over time, have debated whether SK6812 or WS2812 is better; one does have to admit that SK6812's are available in in RGBW (indeed, with Cool, Neutral and Warm white) and WWA versions, while the WS2812's are only RGB. These are available in nearly every plausible component package: 5050, 3535, at least two drastically different packages both called 2020 by the merchant selling them, and a 1515 (that's 1.5mm on a side). In the other direction, there are 3mm and 5mm through-hole diffused ones (though they are quite uncommon, and are *not* cheap, and there are even a few PCBs that pair a WS2811 and a trio of beefy MOSFET drivers for *MONSTER* leds, in the 10-50 watt range; the use case they're intended for remains unclear to me.

I won't try to state which controllers come in which packages: I simply don't know, because the sellers aren't honest and/or don't know which precise controller was used. The only way I could find out would be to depot them and examine the die under a microscope, and even that isn't foolproof. In any event, made by someone or another, they are available in nearly every plausible component package: 5050, 3535, at least two drastically different packages both called 2020 by the merchant selling them, 1515 (that's 1.5mm on a side), 2427 (these I am told are what are under the surface of COB led strip), 3216 (in english units, 1206 - the same size as the resistors), and more. In the other direction, there are 5mm and 8mm through-hole diffused ones (though they are uncommon, and are *not* cheap - in fact, so "not cheap" that the "christmas light" style strings still use WS2811-alikes, and there are even a number of PCBs that pair a WS2811 and a trio of beefy MOSFET drivers for *MONSTER* leds, in the 10-50 watt range available on aliexpress; the use case they're intended for remains unclear to me. They are also, even direct from china, sufficiently expensive that a string of them could run into the thousands of dollars.

### Key differences
There are a very small number of *significant* differences between the 2812-alikes, other than package and who made them:
* The 5050 package with 6 pins was used by the original WS2812,, the WS281 and WS2815,  and a small number of compatible clones. It is also used by a very large number of incompatible leds. If you see leds with a 6-pin package and were considering buying them, you need to do a bit of further investigation. There are three possibilities, and only one of them is appropriate if you wanted 2812-alikes. The pinout chart will hold the answers; if it's strip, you can probably get the key information from the markings on the strip:
  * If they have two power pins per LED, or (for strip) if there is only one data line, they are actual WS2812-not-B's. Pass, these burned out easily and had no particular advantage.
  * If they have a DI/DO and BI/BO - these are WS2813/WS2815 LEDs! These are the top of the line from World Semi. DI/DO is the normal date line, while BI/BO is a backup data line. If one LED in the strip fails for any reason, the rest of the strip after it will not fail too. The difference between the two is that WS2813's are 5v, and behave much the same as typical 2812-alikes except for the redundant data lines. WS2815's run at 12v, drawing only 1/3rd the current for the same brightness (my understanding is that they have three tiny LED dies in series). Since the voltage drop along a conductor is proportional to the current, you have to inject power less frequently. Knowing that power is defined as P = V * I, and that V = 12, and I is one third that of the WS2812, we can see that the overall power ends up as: (12/5 * 1/3) or 4/5ths that if a 2812. As noted, they're of similar brightness; the operating forward voltage of a blue LED is around 3.2V. With perfect drivers (assuming no minimum dropout - this isn't the case, but they're quite good), there is 1.8v or so of margin on the blue channel (this is the one that has the highest voltage requirement) Once the blue hits it's limit, every 0.1V below that lowers the voltage applied to the LED by 0.1s, causing color rendition failure. The 12V ones have around 2.4V of margin for blue, and once you do hit that limit, the the voltage seen by each LED will only drop by one third that much. The disadvantages of the the WS2815's are the higher cost, the lack of RGBW options, and the fact that they need a different voltage power supply than the controller. They also supposedly will wait longer before latching (250uS according to some source).
  * If they have DI/DO and CI/CO, they aren't 2812-alikes. They are APA102-alikes. They use a two-wire non-timing-critical control scheme, cost more, PWM at a higher frequency for less weird and undesirable persistence of vision effects, and use 4 bytes per LED, including a 5-bit global dimming field. The two wire control allows data to be pushed through them far faster, increasing the effective maximum frame rate by more than an order of magnitude (depending on the clone, the maximum ranges from 10 MHz to 40 MHz). These are excellent LEDs. The added finesse of global dimming makes low brightness colors look great. They are also significantly more expensive, have no RGBW or WWA versions, and as noted, have a different control scheme. This library does not support APA102 or APA102-alikes: Since the control can be achieved with the SPI port very efficiently and there are no modern-AVR-specific caveats, you can use the usual libraries recommended for them on other Arduino-compatible boards. They should work on modern AVRs too.

### Flexible PCB strip
Typically made with sticky tape on one side, and 5050 LEDs on the other, or covered in a silicon waterproofing layer for outdoor use. This stuff is incredibly common, and can often be seen in the wild decorating storefronts, and is the least expensive of these options. It looks pretty freakin' awful if you don't either use some sort of diffuser, or place the LEDs such that the LEDs are not in direct line of sight to viewers (for example, on top of bookcases, partiuclarly with a lip at the top, or to illuminate display cases (where they would be placed below the opaque edge of the display case). The points are just too bright for the tiny area of the visual field that they come from. This complaint is apparently widespread since there are a number of solutions to this. There are little diffuser caps you're supposed to put over 5050 LEDs, individually. Those don't help much. There is a highly effective method that essentially places the strip within an opaque rubber sleeve which directs the light in one direction or another as well as diffusing it, though the added expense is not negligible, the result is bulkier, and purchasing more confusing: the sleeves are available in many cross-sections, and it may not be clear which is the right choice for your use case. Other techniques include: Converting a fluorescent light like you'd put in a suspended ceiling - remove all the old guts, put LEDs on top, and reassemble. You may need to stack several layers of diffuser for acceptable results

There are a few notable variations on the LED strip:
### SK6805 COB LED strip
"COB" LEDs are those LEDs wherein the emitter is embedded in some sort of soft rubber, generally featuring large numbers of smaller LEDs (which is more efficient, and looks better). In this case, there is a row of incredibly high density (332/m) of SK6805 LEDs in a very small packages, hidden under a white rubbery diffuser that leaves the total strip barely thicker than a standard 5050-strip. While it doesn't throw off as much light because of the lower power LEDs, the tight packing of the pattern makes it appear almost continuous. Vendors claim (though I find dubious) that they only need power injected every 3m (at both ends). 1M with power into 1 end does work, though, so maybe the claim holds up. It certainly shouldn't be hard to avoid voltage droop browning if it doesn't actually hold up by limiting brightness in software. COB LED strip is incredibly beautiful - and eye-wateringly expensive - $30/m or so.
* Individually addressable 12v LED strip with external ICs and standalone RGB LEDs, usually with three LEDs per IC has each group of LEDs controlled as a group. This means less data needs to be sent for the same number of LEDs, but also limits flexibility. This is not recommended (though see Diffused String Lights below).
* Individually addressable 12v LED strip with just the LEDs is usually based on the WS2815 (this is likely what you want if going the 12v route - they put three tiny lower current LEDs in series, and drop the current they put through them). These also have an extra "bypass" data in and out, so that if one of the LEDs fails completely, the whole string doesn't go out.
* Warning: There exist 12v LED strips that are not addressable. These have no relation to this library, they are controlled using beefy mosfets, and all the LEDs on the strip must be the same color. Further discussion of these is beyond the scope of this document. They're no where near as much fun as addressable ones.

### Rigid PCBs and panels
Pre-populated PCBs full of addressable LEDs are readily available. They are commonly shaped like rings (popular for making glasses), bars, or squares. These also almost universally use 5050 package LEDs (with the exception of some panels, which use smaller packages to more effectively make a display). The refresh rate quickly becomes problematic on larger displays. While the LEDs are still not terribly comfortable to look at, the smaller number of them and their confinement to a small part of ones visual field on small panels, or distance and the larger number involved in a truly massive display on the wall of a building or something isn't unpleasant to look at in the same way.

That said, the larger LED panels use an entirely different control scheme, on account of the limited frame rate achievable as the number of LEDs increases. Some may use APA102s, but it seems more common that some proprietary control protocol is used.

### "String Lights"
Name in quotes because I don't think this is an official name; I don't know that they have one, but I have to call them something. These are the only type that is essentially always based on the WS2811: Each module has a single large RGB LED with the fogged plastic for light diffusion, connected to the '2811 driver IC (this is in spite of the existence of LEDs of the same size with 2812-alike controllers inside - I guess it's cheaper to do it this way). Each of those LED assemblies, with 6 wires from one the end opposite the LED, then has plastic surrounding it (it looks to me like they got pushed into a housing that fit tightly around the bulb and epoxy was then poured in). You should inject power at the start (or end) of each string of 50. You should use the additional power wires they provide for this; JST-SM connectors like they use can **barely** handle 3A (the maximum current from 50 LEDs), and you never want to push something right to it's limit. So should have power coming into one or both of the pairs of power wires on every section.
#### "Cursed" String Lights - mystery has been solved
I was recently made aware of (as in was wondering what the heck was wrong with my lights) a type of light string which to all outward appearances, looks almost identical to normal string lights. Side-by-side, tiny differences like the exact tint of the potting compound may be visible. However, even though the data rate coming out is correct, many LEDs will not work when down-stream from them. It is nearly impossible to confirm this behavior without a 'scope: These apparently use a different controller which also comes in a SOIC8 package... but this one outputs 3V logic levels. I didn't think it makes much sense either... until several weeks later, I was checking light strips and came upon them again - only this time the LED that was in my hand happened to have a smoother surface to the epoxy-filled case near the bottom, so I cold read the silkscreen inside. Gnd. Dat. 12v. This was twelve volt LED string! Likely the logic voltage was going through an internal regulator. If it was expecting 7 volts of headroom, they wouldn't think twice about using cheaper regulator technology, resulting in 7805-like minimum dropouts, which from 5v would yield right around 3.3v. So these are not actually cursed, and they should definitely require less frequent power injections. The only thing that gives me pause is that the brightness wasn't obviously "way too dim". So they couldn't be using the 3-die trick of the WS2813. Are they just dumping the excess voltage as heat? That would drastically increase power consumption, since the current would be the same but the voltage 2.4x higher.

### Diffused "String Lights"
These diffusers are almost always hemispheres, affixed to a round PCB, with two sets of wires coming out opposite sides (imagine taking a round PCB with LED(s) and maybe an IC on it, cutting a pingpong ball in half, and putting that over it. The flat side is usually equipped with adhesive tape for mounting against the wall. Sometimes, each module has a WS2811 and 3 LEDs. These are likely the least common of the types listed here. (I actually *did* the pingpong ball thing, with cheapo knockoff pingpong balls. That was back before such strings were easy to buy; besides I wanted APA102's for that project, and I've only seen those on panels and plain strip. It took two halves of the pingpong ball stacked to get enough diffusing for the one led in the middle. While they came out beautifully, the process was was a ton of work, and I can't recommend that to others - besides, I used the apparently extinct "APA102TW", which has a distinctly better red).

#### WARNING: If you do this anyway, **do not use real pingpong balls**
Real pingpong balls are insanely flammable - unbelivably so. for reasons I've never understood, they are still made from 50% nitrocellulose (one of the active components of smokeless gunpowder). It was widely used as a plastic before less flammable options were invented (its use in movie film ("Nitrate film") is infamous both for degrading in storage over decades rendering the originals of many famous early films unviewable, and for starting infernos in theatres back in the day). But it is still almost universally used in real pingpong balls (maybe competition rules dictate that any other material is not legal?). Don't tape them to your wall, don't put heat generating electronics in them - and really don't use them except to play pingpong or one of the "beer pong" like drinking games, if you must. The fake pingpong balls generally don't bounce right. In my younger years, not even the cheapest frat boys used fake pingpong balls in their beerpong games (I never thought to mention to someone at a college party that "Hey, you know, those pingpong balls burn like crazy"). The nitrocellulose pingpong balls can be recognized by scratching with your fingernail - real ones also contain camphor (50:50 camphor nitrocellulose i hear), which has a distinct smell. You can also always cut off a small piece, take it outdoors away dry grass or other highly flammable material, and set it alight. Don't hold it in your hand while you light it, you cannot drop it fast enough. Normal plastic will burn with a small flame, melting, maybe dripping molten plastic on the ground, take tens of seconds or more to burn, and will leave behind some unburned crud. Nitrocellulose will burn completely, with virtually no smoke and just a trace of ash, almost instantly.

### Fairy Lights
These are, as far as I know, a fairly recent innovation. These use thinner wires and MUCH smaller leds (looks like a single LED, with wires soldered directly onto it, covered in a blob of diffusing plastic. They come in strings of (typically) 50, 100, and 200. Some of the 50's can be daisy-chained (and include wires to inject power). The longer strings generally can't, and most have an integral USB power connector and controller (which generates impressively ugly patterns). It can be desoldered where it meets the LEDs, though I'd recommend cutting the wire, or desoldering it at the USB end, as their wire looks more attractive that the wire most of us have kicking around, and the LED wire itself is is a bit tricky to work with. These strings use lower current SK6805 LEDs, which allows the existence of 100 and 200 LED strings with such thin wire and with USB power (though in the case of 200-LED strings, if you start playing with them, you'll realize that the the controller is limiting the brightness in software to stay within a power budget and keep the ones at the end from going off-color; close investigation will also reveal that most strings of 200 have a joint in the middle. When repurposing these, this should be desoldered unless power can be injected from both ends. Note also that there are (or at least were) two kinds of these; in both cases, the wire looks fragile, but through totally different mechanisms. The currently dominant version has solid (not stranded) wire. On the one hand, this holds its shape very well and can easily be bent into a desired shape. On the downside, this bending action makes it more likely to break via fatigue, and the wires are also more visible. In the past, I have received a few sets made with a very different wire - incredibly flexible; I think there were other fibers woven in with the copper for improved flexibility (like "Litz wire"); it looked great, but it was eager to knot itself if you did anything suboptimal to it (like putting it onto a reel without spinning the reel on it's axis - if the reel does not rotate on it's axis during rolling and unrolling each wrap introduces a 360 degree twist!).

#### Fairy Light controllers
The fairy lights appear to use an LED that has slightly different timing from the standard ones - or at least, one that *tolerates* slightly different timing than the standard. (Some of them have a 32.768kHz crystal in them. This can be replaced with a 40 kHz crystal and that is good enough to make most LEDs work.) So if you were thinking you could chop off the controller, and wire it up to an Arduino, you can. But if you were expecting to use that controller to debug any other kind of addressable LEDs? Nope - they seem to work with only a minority of LEDs, making them of practically zero use as a debugging aid. The ones with a 32k crystal on them work if it's swapped for a 40k one.

### Notable LEDs that are NOT compatible
* Anything that is not addressable, such as standard (single color or RGB) LED strip. As far as I have seen, every listing for a WS2812 compatible device has either WS2811, WS2812, or SK6812, and usually all three, in the title of the listing on marketplace sites like EBay, AliExpress, and Amazon.
* Anything with a data AND a clock line - APA102, APA107 SK9822, HD107 and similar. These are all great LEDs - they implement 5-bit of global dimming dimming, in some cases using *actual constant current sources* instead of a superimposing a second PWM frequency! Because of that, and because they generate PWM at a higher frequency, they are suited for persistance-of-vision dependent art without looking like dashed lines, and do a far better job of rendering dim colors. Since there is both a clock and data line the transfer rate is governed by the controller and the maximum specified by the LEDs. These maximums are often high enough that in practice, the limit of the wiring will be encountered first unless great care is taken in the wiring. There exist multiple other libraries for these, since they operate via SPI (be it software or hardware SPI), no special measures are required to make those libraries compatible with AVRxt. More care and research is needed when buying these, too, since the specific LED, in some applications, actually makes a big difference, even though there is a control scheme which works for all of them (though not the HD108, which wants 8 bytes per LED instead of 4, for 16-bit color - and of course, is completely incompatible with everything else, which is likely why it doesn't seem to have set the world on fire like the WS2812 and APA102 did).

### Bad pixels
They do exist, and travel in groups. Apparently when these were relatively new there were a LOT of bad batches of them making the rounds; the root cause, apparently, was that the transparent material atop the die and LEDs was not making an airtight seal, and corrosion would rapidly destroy them at that point. On the other hand, this problem seems to be considerably less common than it was, and they are now fairly rare. At the peak of the pixel plague (as in a plague that afflicts pixels, not a plague of pixels; the latter is ongoing with no sign of slowing), the "wisdom of the internet" was that the SK6812 LEDs (which are knockoffs) were in fact more reliable than WS2812's of that era. As I said, I think WorldSemi (assuming they even still make them, and didn't throw their hands up in despair over the clones) has gotten their process sorted out.

### SK6805 vs SK6812
There exists a very similar led to the SK6812, called the SK6805. These use only 5mA per channel (and are not as bright of course). Physically, SK6805 is typically smaller (though there is some overlap in available packages) - the smallest I have seen is 1.5 x 1.5mm (yes, you read that right, you can fit a 3x3 array of them in the footprint of a standard 5050 LED), while 3.5x3.5 is very common, and 2427 is supposedly used in "COB" led strips. These are the ones used in most fairy lights.

## How much current do these LEDs actually draw?
Well on that topic... The number everyone throws around on forums is 20mA/channel for normal ones. The ones I've tested ranged from 13-16mA/channel. The SK6812's were suspiciously close to.... 12mA/channel (as if the last two digits of the part number indicate the current...) That makes assuming 60mA per LED a conservative estimate (especially if - as you should be - you are confident that your supply will simply shut down without damage if overloaded). While you system must react gracefully to that kind of load, shutting down due to overcurrent if too many LEDs are on counts as graceful if the problem is not encountered in normal operation


## Safety Notice
Large LED lighting projects can pose safety hazards when connected improperly (generally fire, though the exposed mains terminals on the backs of some power supplies commonly used for this should also be covered, along with other exposed wires). While these concerns are by no means unique to LED lighting projects, such projects often combine high current, shoddy components, inexperienced designers/programmers and operation for prolonged periods of time with little or no supervision (or in some cases without any sober supervision; it's debatable whether a mob of intoxicated partygoers is preferable to no supervision at all...) - so particular care is warranted.

Large LED light strips can draw a surprising amount of current (see note above), and a great deal of discussion has been devoted how and how often power should be injected. What I rarely see mentioned is the limit of the ubiquitous JST-SM connectors that these strips usually use. Those are rated at a MAXIMUM of 3A, when used with 22 gauge wire, and when the terminals involved are actual JST terminals (most from China are almost certainly copies, so maybe best not push it if you're not buying name brand connectors). I have seen wires supplying power to light strings get so hot (without any sign of failure of the string) that the red positive wire had darkened to the same color as the black ground wire. How could this have happened? I used pre-wired connectors with 22 AWG wire to bring in power! Examination revealed that the wire, with "22 AWG" printed right on it, was actually between 29 and 30 AWG, with the difference in outside diameter made up with thicker insulation. This is much worse than usual, but it is not unusual for "22 AWG" wire from China to turn out to be only 26 AWG.

In addition to following all the usual safety guidelines, pay particular attention to these:
* Do not overload connectors; check the maximum current rating for any type of connector you're using, and make sure that the worst case scenario remains comfortably below it. Surprisingly few people show much concern about overloading their connectors.
* Always **measure** the gauge of the wire in a batch of wire or pre-wired connectors; **intentionally mismarked wire is very common** on the Chinese market, with thicker insulation to make it look like what it is sold as. This is particularly troublesome when you were planning to crimp on connectors, as the insulation thickness no longer matches what the crimper and terminals expect.
* Always pull-test a few samples from a batch of prewired connectors (most commercial dupont line fails the pull test because they use the thinnest cheapest wire they can get (namely ribbon cable), and the terminals are either not crimped hard enough, or just not designed for such thin wire. Even some JST-SM prewired connectors do. Hopefully if you crimp your own, you're already pull testing them!
* When using pre-wired connectors, use a pin extractor to pull out a male and female pin from a given batch and compare them to a pin on the "same" connectors you already have. Proceed only with great caution and testing if you discover that the existing connectors and the ones you just got do not look identical - some terminals are made in several versions with different mating force or for different wire gauges. These both matter, but are generally designed to be compatible. On the other hand there are horror stories of incompatible male and female pins from different suppliers that would fail over time when used together (I question how that could happen unless they were being repeatedly connected and disconnected, which this individual claimed was not occurring, unless they were getting damp (even wh, or male pins that were ever so slightly too large and would damage female terminals from other manufacturers - both could result in a poor (high resistance) connection which might get hot without causing visible symptoms until it caught fire, or you noticed that the wires have been discolored from heat..
* Make sure that any power supply you use will turn itself off in the event of overcurrent, instead of changing to constant current mode. Folding back to constant current mode is great for a benchtop supply in your workshop, but in the field, all it does is waste power generating a bunch of heat wherever the fault is, until either someone notices, something burns out, or it starts a fire.
* Be certain that the system will deal gracefully with all pixels being written white. I have witnessed malfunctioning code result in this on a number of occasions. If you compile code for one speed, and it somehow ends up on a board running significantly, but not overwhelmingly slower, it will result in all the bits, 0 or 1, looking like 1's and hence all LEDs coming on white.
* Consider the use of a resettable circuit breaker (these aren't that expensive) or even a fuse if you are not satisfied with the safety of the supply you are using under overcurrent conditions *or just use a different supply!*
* Consider implementing an automatic shutoff in software the lights are left on for a certain length of time without the settings being changed (or on other criteria depending on what sensors it may have) - cases where the LEDs are at nearly maximum brightness are of greatest concern.
* Are you using a buck or boost converter anywhere? If so, there are two things to check:
  * What happens when the output is shorted? You want it to shut off, and resume functioning after power is disconnected and restored. Anything involving sparks, "pop" noises, smoke, or failure of the device in general is a no-go.
  * You want very dearly for it to use synchronous rectification to minimize the amount of waste heat it generates (and reduce overall power consumption). I often use laptop power bricks (approx 3.5A at 19-20V, in a nice consumer-proof form, which can usually be obtained for free if you don't need them to match any laptop that's still useful, and they typically safely shut off when overloaded), wuith the 19V carried by a second pair of wires ziptied along the length of the string, combined with a high efficiency buck converter at each point where power is injected. Buck converters can only pull the voltage upwards, not downwards, so they can't "fight" like some power supplies can. Under the no-load condition, whichever converter has the highest set point due to variation between units will carry most of the current - but as soon as the lights are on and the load increases, the same wire resistance that forces us to use power injectors in the first place will distribute the load more or less evenly.
  * Use buck converters that are capable of handling a significantly larger load than you ask of them. Derating components is general good practice, but it's particularly important for modules from china: It is not uncommon to see an extra amp added to the maximum current spec of the parts by dishonest vendors (as in "Oh cool a 3 A buck converter, perfect. Let's just pull the datasheet for the main IC and make sure there aren't any serious quirks that would cause problems... Ah here, looks good, looks good...  wait, why does this say '2A integrated buck converter'?")
* Beware of the potential for shorts between wires caused by stray strands of wire that break off from fatigue near a solder joint, and then work their way through the insulation through flexing and heat, eventually shorting out the string. I have had this happen to a number of strings - too much pressure from a ziptie, and the buck converter made too much waste heat.
* In a similar vein, take precautions against things coming into contact with the live power wires. 5v or 12v is not enough that you'd feel it, but it could cause cascading failures of the system, start a fire, or otherwise cause Bad Things to happen. Cover things that would be exposed wires in shrink tube, project boxes, and similar. For added security, solder mask can be painted onto conductors to keep them from getting bent and touching, and that could even be followed by UV-curing resin
* Particularly when using connectors of any sort, always think about what would happen if any one wire were to become disconnected, because connectors are some of the least reliable components in a system.


## Comparison with other WS2812 libraries
Both FastLED and Adafruit libraries have the buffered design which - while expensive in terms of RAM, you need it for arbitrary control over the colors of each pixel. Other schemes can be used for cases where the patterns are simple, at substantial memory savings (an example that supports these parts is [FAB_LED](https://github.com/sonyhome/FAB_LED)), and while those are very valuable in applicable situations, I wanted something that could take the place of a buffered, full-service LED control library. That meant that I should be modifying either FastLED or AdafruitNeoPixel (ideally keeping the API nearly identical); Opening the source code for the libraries, I found the Adafruit one to be readily understood, simple, with one purpose that it handles capably, and extensively commented, especially the assembly. Meanwhile FastLED was largely unreadable. I felt that the code suffered from "kitchen-sinkism" - when the author throws feature after feature, up to and including the kitchen sink, into a gigantic omnibus library. I had yet to locate the code that actually generated the output in the FastLED source within the length of time it took to modify the Adafruit library to achieve what I needed in the first tinyNeoPixel release.

## New Adafruit additions
If Adafruit has added new methods to their library, please report via an issue in one of my cores that ships with this library so that I can pull in the changes.

## Changelog - V2.x.x (AVRxt) version
* 2.0.5 - Correct support for several speeds around 24-32 MHz. Restructured a few of the longer delays for greater flash efficiency at very high speeds.
* 2.0.4 - Add support for speeds of 4-6 MHz.  Reviewed the assembly for correctness according to AVR GCC inline assembly documentation (or what passes for it). *every existing implementation in this library, for every speed range had a pair of incorrect constraints*. At speeds of 14 MHz or higher, there was a third incorrect constraint. All of these were inherited from the Adafruit library. Because of the quirks of the register allocation process in avr-gcc, and the fact that methods (oh, excuse me, "member functions"`*`) are exempt from link time optimization and are never inlined, these incorrect constraints could never cause problems, but that did not mean they should be left in. A future version of avr-gcc with a smarter optimizer, as well as a user chopping out a little piece to use without the rest of the library would both run the risk of issues.
* 2.0.3 - Fix issue with compile errors when micros() has been disabled (ie, if millis is disabled or set to a timing source with resolution exceeding tens of microseconds, such as the RTC). In these cases we issue a warning. See the notes above. First version for which release notes were included.


## License
Unlike the core itself (as noted within the library files), tinyNeoPixel is licensed under LGPL 3, not LGPL 2.1, since the Adafruit library was always LGPL 3.


### Nomenclature note
C++ predates the invention of names like "properties" and "methods" (as well as countless programming concepts that we take for granted on more modern languages, if you haven't noticed). Strictly speaking, they are called "member variables" and "member functions" - however when you call them those names, nobody without exposure to C++ outside Arduino will have any idea what you're talking about, nor will anyone who didn't learn C++ decades ago will have any idea what you're talking about. So we use the modern names.
