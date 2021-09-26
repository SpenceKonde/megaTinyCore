# tinyNeoPixel_Static documentation is elsewhere
tinyNeoPixel is a tweaked version of adafruitNeoPixel, supporting more clock speeds and being modified to work with the AVRxt variant of the AVR instruction set. It is code compatible excepting the change to the class name. tinyNeoPixel_Static has been modified further to reduce flash footprint. The versions distributed with megaTinyCore and DxCore are identical in all meaningful ways. ATTinyCore supports the classic AVR ATtiny parts which use AVRe instead of AVRxt - the assembly code used to output the WS2812 signal is hence different, and it supports different clock speeds as well (the modern AVRs can run faster, especially when overclocked, so the library needs to go to a higher maximum speed, but because they have a fractional baud rate generator, there's no need for UART crystals).
  * [Summary and changes specific to tinyNeoPixel_Static](https://github.com/SpenceKonde/megaTinyCore/blob/master/megaavr/extras/tinyNeoPixel.md)
  * [Full class reference from Adafruit](https://adafruit.github.io/Adafruit_NeoPixel/html/class_adafruit___neo_pixel.html)
In the event that a function listed in that class reference is not defined in tinyNeoPixel_Static, or in the event of differences in behavior between adafruitNeoPixel and tinyNeoPixel_Static (except as noted in the first documentation link above - the Static (as in static memory allocation) has different calling conventions for the constructor doesn't support changing length of the string at runtime in the interest of reducing flash usage), that is a bug, and is likely unknown to the maintainer of megaTinyCore - please report it via github issue (preferred) or by emailing SpenceKonde@gmail.com

## Licensing
**tinyNeoPixel_Static is LGPL 3 not LGPL 2.1**
It is derived from Adafruit's adafruitNeoPixel library, which is licenced under LGPL 3.
See [LICENSE.md for the library](LICENSE.md)

The rest of this core (except where noted) is LGPL 2.1.
