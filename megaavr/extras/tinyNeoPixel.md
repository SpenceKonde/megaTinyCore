# tinyNeoPixel - a modern AVR (AVRxt) compatible library for WS2812 "NeoPixel" and similar
**for tinyAVR 0/1/2-series, megaAVR 0-series, AVR DA/DB-series,  future AVR Ex-series**

The change in architecture from the classic AVRs (which use the AVRe or AVRe+ variant of the AVR instruction set) to "modern" AVRs (which use AVRxt) causes many of the existing WS2812 libraries to not work on the new parts. This library adapts the Adafruit_NeoPixel library to work with AVRxt version; this has slightly improved execution times, among other things, and adds support for additional clock speeds: 10 and 20 MHz because of it's use on the tinyAVR 0/1/2-series and megaAVR 0-series, and faster speeds all the way up to 48 MHz (I have yet to try overclocking them quite that hard, but while rated at 24 MHz, the Dx-series seems to be able to do 32 MHz off the internal oscillator (that's without touching the calibration). Unlike the version for classic AVR (AVRe/AVRe+), one of the benefits of the improved timing is that ST (indirect store) executes in 1 clock cycle, rather than 2, so there is no need for multiple versions of the library.

Like the normal Adafruit_NeoPixel library, this supports WS2811/WS2812/SK6812 and all the nominally compatible single-wire individually addressable LEDs. It does not support two-wire ones like the APA102 or the countless knockoffs thereof; libraries for those do not pose special compatibility issues with AVRxt because they are not timing critical in the same way.

## tinyNeoPixel and tinyNeoPixel_Static
There are two versions of this library provided. `tinyNeoPixel` implements the entire API that Adafruit_NeoPixel does, including setting the length and type of LEDs (color order and whether it's RGB or RGBW) of the string at runtime. This provides maximum portability between code written for use with Adafruit_NeoPixel and tinyNeoPixel (only the constructor and library name need to be changed) - however, the memory used to store the "frame buffer" (the color of every pixel) is not included in the SRAM usage displayed when compiling a sketch because it is "dynamically allocated". This can be a large portion of the memory available on smaller parts (I have seen support inquiries from people trying to control a string of LEDs which would require several times the total memory of the part for this alone); trying to allocate a larger array than will fit in RAM does not generate any sort of error - it just fails to allocate anything at runtime, nothing gets sent to the light string, and no LEDs turn on. This dynamic memory allocation also requires compiling in malloc(), free(), and associated functions; on parts with small flash (ie, tinyAVR), this can be significant. Finally, dynamic memory allocation is arguably bad practice on small embedded systems like the AVR devices.

`tinyNeoPixel_Static` is slightly cutdown, removing the option to change the length or type of pixels at runtime (which relies on dynamic memory allocation), and requires the user to manually declare the pixel array and pass it to the tinyNeoPixel constructor. Additionally, it does not set the pinMode of the pin (the sketch must set this as OUTPUT); this allows severely flash-constrained applications to save a small amount of flash by eliminating calls to pinMode and replacing them with writes to the `PORTx.DIR` or `VPORTx.DIR` registers. Finally, no call to begin() need be made - begin() is removed entirely. These changes reduce sketch size and provide greater visibility on the memory usage. Unless you need to change string length or type at runtime, it is recommended that `tinyNeoPixel_Static` be used.


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

`sine8(uint8_t angle)` Returns the sine of the angle (angle in 256's of a circle, that is, 128 = 180 degrees), from 0 to 255. Used for some animation effects, uses a lookup table kept in PROGMEM.

`gamma8(uint8_t input_brightness)` Performs basic gamma correction for smoother color transitions, returns a gamma corrected brightness that can be passed to setPixelColor().

`gamma32(uint_32 input_color)` As gamma8, only acts on and returns a 32-bit "packed" color (uint32_t).

`Color(uint8_t r, uint8_t g, uint8_t b)` Return the color `r,g,b` as a "packed" color, which is a uint32_t (For RGB leds)

`Color(uint8_t r, uint8_t g, uint8_t b, uint8_t w)` Return the color `r,g,b,w` as a uint_32 as a "packed" color, which is a uint32_t (For RGBW leds)

`ColorHSV(uint16_t hue, uint8_t sat, uint8_t val)` Return the color described by the given Hue, Saturation and Value numbers as a uint32_t

## Pixel order constants
In order to specify the order of the colors on each LED, the third argument passed to the constructor should be one of these constants; a define is provided for every possible permutation, however only a small subset of those are widespread in the wild. GRB and

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

## General notes about tinyNeoPixel
This library is supplied with support for speeds up to 48 MHz which is far in excess of what the tinyAVR series can reach (but - amazingly - is within reach of the Dx-series).

### Library name
While this library was initially created in order to both ensure compatibility with, and through the Static version, fit within the flash and memory constraints of, the tinyAVR line of parts, **this library is entirely suitable for non-tiny devices**; It offers all the functionality of the Adafruit version on the library, with the addition of the Static mode. Where appropriate, core-specific matters are accounted for (not relevant for modern AVR versions). This library is code compatible with the version distributed with ATTinyCore, though that has additional dependencies on menu options.

Dynamic allocation has no place on an 8-bit microcontroller. Since almost all of the other pixel libraries *dont* work, considering the popularity of WS2812 LEDs, I decided it would be improve the UX with the core to distribute a known working '2812 library with a standard API along with the cores.

### Why not base it on FastLED
Why did I start from the Adafruit library, and not the popular FastLED library? Because I can't make sense of that code! I got as far as determining that that library exhibits extreme kitchen-sink-ism which I detest on embedded platforms.

### New Adafruit additions
If Adafruit has added new methods to their library, please report via an issue in one of my cores that ships with this library so that I can pull in the changes.

### License
Unlike the core itself (as noted within the library files), tinyNeoPixel is licensed under LGPL 3, not LGPL 2.1, since the Adafruit library was always LGPL 3.
