# tinyAVR device selection guide
There are three modern tinyAVR series, and within each family, a dozen or more parts across 3-4 different pincounts.

This document, I hope, will help you determine the best part for your needs.

## Most prominant differences
This table lists some of the most significant differences between the tinyAVR 0, 1, and 2 series parts.

1+series denotes the 5 16k or 32k parts in the 1-series. These have significantly enhanced features, without an accompanying jump in price.

| Parameter | 0-series | 1-series | 1+series | 2-series |
|-----------|----------|----------|----------|----------|
| CCL LUTs  | 2        | 2        | 2        | 4        |
| EVSYS chan| 1sync/2async | 2sync/4async | 2sync/4async | 6     |
| ADC type  | Normal   | Normal   | 2xNormal | Differential<br/> with 1/2/4/8/16x gain |
| ADC res   | 10-bit   | 10-bit   | 10-bit   | 12-bit   |
| ADC speed | 4-6x <br/> classic | 4-6x <br/> classic | 4-6x <br/> classic | 10x <br/> classic |
| VREF opts | Dumb     | Dumb     | Dumb+ext | Good     |
| DACs      | 0        | 1        | 1        | 0        |
| AC's      | 1        | 1        | 3        | 1        |
| AC DACREF | No       | Yes*     | Yes*     | Yes      |
| USARTs    | 1        | 1        | 1        | 2        |
| Alt Reset | No       | No       | No       | 20/24-pin|
| TCBs      | 1x v1.0  | 1x v1.0  | 2x v1.0  | 2x v1.1  |
| TCD0      | No       | Yes      | Yes      | No       |
| Max pract.<br/>overclock| 30 MHz | 30 MHz | 30 MHz | 32 MHz pretty easy |
| Errata    | A lot    | Tons     | A lot    | Not much |

2-series TCBs are the only ones that can do cascade mode (not very important) and count on event (if you need it, you *really* need it). The Dx and Ex-series also got the new version. The TCA is also improved on the 2-series with an extra event input.

`*` - AC0 DACREF on 1-series is DAC0, the same one that can be output to a pin. This is occasionally inconvenient.

There are two sets of VREF options, one of which is good, and one of which is dumb.
The dumb one is 0V55, 1V1, 1V5, 2V5, 4V3, VDD, and external vref supported on the 1+series. I don't know what numbnuts picked those voltages. They could have made them less-round numbers or something I guess, even less convenient... but these are intensely inconvenient voltages to have to choose from. Particularly compared to the good ones on the 2-series and every other modern AVR:

1V024. 2V048. 2V5. 4V096, VDD, external.

What's special about these? Mainly the fact that we have an ADC with a native resolution of 12 bits. That's 0-4095.
4V096 reference used for a 12-bit measurement with no gain will return a numeric voltage that is simply equal to the voltage, in millivolts. Hence you can read a pin, say you get back 2360. You want to make that into a human readable voltage? Easy, just print "2360 mV" to the console or display! Other combinations of those three power-of-2 divided by 1000 references can also be super-convenient in various circumstances, because you always end up with the numeric units of your analog reading as being a power of two (positive or negative power, if you say, oversample and decimate to get 16-bits using the 4V096. you end up with a number from 0-65535, in units of 1/16th of a millivolt. That means you can separate the fractional portion with this kind of thing:
```c
/* global scope */
const uint8_t sixteenths[]={0, 6, 13, 19, 25, 31, 38, 44, 50, 56, 63, 69, 75, 81, 88, 94};
/* Now when you take the reading */

uint8_t fract = reading & 0x0F; // 0-15
reading >>= 4; // now this has the whole millivolt part!
Serial.print(reading);
Serial.print('.');
uint8_t temp = sixteenths[fract];
if (temp < 10) {
  Serial.print('0');
}
Seriial.println(temp);
```

With a 16-byte lookup table, you've managed to drop several expensive divisions and avoid having to use the grindingly slow, flashhogging floats (if you use no floats in your sketch, you save a considerable amount of flash) Note that it was an intentional decision to only show two significant figures - you only have data enough to show with granularity of 16ths). The new references are much much more convenient!

The errata on the 2-series is not bad at all - a couple of the widespread ones we're all used to, plus you need to turn the ADC off manually before going to sleep.

The errata on the 0/1-series is a totally diffeent matter. The table of contents is too big to fit on one screen. The RTC and PIT both misbehave horribly if you enable only one of them, the CCL's D-latch doesn't work, and there's a huge variety of other bugs (most of which are worked around by the core).

Plus, the PORTMUX and EVSYS look like a beta-test version. EVSYS channels are divided into two types, the headers didn't give you names for the event users (though I think I papered over this, because it was just so fucking ugly), and the layout of generators across the channels is wierd and inconsistent. The Event library makes this a little easier to deal with, but there's no way around the fact that the event system is fiddlier and less powerful. PORTMUX works, it's just unpleasant, because they hadn't thought of naming the registers after what peripherals they controlled the route for yet, so they're CTRLA, CTRLB, CTRLC, CTRLD, instead of TCAROUTEA, USARTROUTEA etc.

On the other hand, the 1+series parts are the only AVR family to feature TWO ADCs. I suspect this was intended to be an escape hatch to be able to simulate a differential ADC - but it is much more than that. You can:
* Have the two ADCs each monitoring a different channel. I was able to get these parts to continually read two channels with an ADC, alternating between them, and between ADC reads, use a lookup table to convert the ADC reading to a value to send over SPI to a DAC... Both ADCs were free-running, and I'd poll them until the result was ready, lookup, send, then poll other adc, and when i was done optimizing, it was running at the speed of the ADCs - I was not missing a single reading from either ADC. That was a fun project. Only time I think I've ever had fun with an analog input. The overall goal was a 2-channel device to take 2 input voltages, and for each ADC reading, look up an arbitrary voltage to set the DAC to, in order to transform the voltage curve from analog input sources which had one characteristic voltage vs sensor value into the voltage curve that the other device was expecting. Client had no control over the programming on either side, and needed a thing to go in the middle to act as a translator.
* Use one ADC normally. Leave the other one sitting in free-running mode with the Window Comparator configured to generate an interrupt in the event of a voltage excursion on a specific pin - without having to take your other ADC's attention away from what it's measuring (remember that the sampling capacitor holds a small but non-zero amount of charge, which is why one usually throws away the first reading when changing channels)
* But you probably don't want to do the simulated differential ADC thing, because you could get a 2-series with a real ADC.

The 1-series also has the type D timer and all it's crazy features; there are some things that you just can't do with TCA's and TCBs.

And it has three analog comparators (what?! three?!), complete with the windowed mode like you can do on the AVR DA and DB parts (so these have two completely different ways of window comparating, and with the dual ADC, if you for some reason wanted to, you could compare three voltages against a window **entirely in the background, with no CPU involvement other than responding to the interrupt when your specified conditions occurred**)


## And the memory layout

| Parameter | 0-series | 1-series | 1+series | 2-series |
|-----------|----------|----------|----------|----------|
| 32k flash | No       | No       | 20/24 pin| Yes      |
| ram @ 32k | -        | -        | 2k       | 3k       |
| 16k flash | Yes      | No       | Yes      | Yes      |
| ram @ 16k | 1k       | -        | 2k       | 2k       |
|  8k flash | Yes      | Yes      | No       | Yes      |
| ram @ 8k  | 512b     | 512b     | -        | 1k       |
|  4k flash | Yes      | Yes      | No       | Yes      |
| ram @ 4k  | 256b     | 512b     | -        | 512b     |
|  2k flash | 8/14 pin | 8/14 pin | No       | No       |
| ram @ 2k  | 128b     | 512b     | -        | -        |


## Okay... So now, let's just go through when you would want to use which series

### 2-series
You definitely want to use the 2-series if:
* You need a really good ADC
* You need a seccond serial port
* You you want to use them as the vessel for complex CCL-based schemes. The 2-series parts are the best parts if all you really care about is the CCL. You can get 4 LUTs for cheaper than on any other part, and on top of that, the pinmapping (while kinda weird) will get you inputs on every channel except LUT0 IN0 (that's the UPDI pin) and an output from every LUT. I've used 424s on one occasion because I determined that what I needed was a TCA, a TCB, 4 CCL luts with a way to get an output pin, and the binary was compiling to around 2k (though it would grow to a hair over 3 by the end). Tiny 424 checks off every box, and SOIC-14 is the kind of package you could solder with ease even after drinking a six-pack of beer (not that I recommend soldering drunk, I just mean, they are one of the easiest surface mount packages to solder, and I consider them faster and less tedious to solder than a DIP).
* If you want autoreset and don't want to have to fart around with HV programming.
* If you like to live on the edge and for some reason need to clock a tinyAVR at 32 MHz and can't imagine why that need for speed might point to some questionable decisions elsewhere in your planning.
* If you don't need anything specific to the 1-series, the 2-series devices are all around better parts. The tiny 0/1-series kinda shipped before they ought to have been (probably because they felt they had to get some new parts out because the classic AVRs were starting to look pretty crude by the standards of the day - not like I wouldn't have made the same decision if I were in their position). Their EVSYS is particularly awful. Event.h tries it's best to paper over it, but it's kinda hard to hide.

### 1-series
Sometimes, though, the 1-series can do something, and the 2-series can't.
* If you need more than 6 PWM channels. The 1-series has a type D timer that gives you 2 more, and the TCD can do some things the other timers can't.
* If you need the functionality of a TCD.
* If two independant ADCs are more useful to you than one really fancy one.
* If you need multiple analog comparators.
* If you need a DAC output.

### 0-series
Uhhhhh.....
* If you can't get anything else.
* If you are more of a tightwad than Ebeneezer Scrooge and the suggestion of spending 2-10 cents more for a 1-series makes you launch into a tirade about how people today don't understand the value of a dollar. Or in this case, a nickel. (Some of the 0-series are actually priced **higher** than their strictly better 1-series or 1+series counterparts.
* If you are a masochist, and enjoy the pain of working with what appear to be intentionally kneecapped parts.
* Yeah, that's about it. The 0-series tinies are strictly worse than the 1-series on every conceivable metric... **and they are practically the same damned price**.

The problem with the 0-series is that they have features like a bare-bones budget part, but the low price point of the 1-series doesn't leave much room for such a part on the market, and the savings are miniscule, while they lose large amounts of functionality.

### Don't forget - there are also AVR DD-series parts invading tinyAVR territory
AVR DD-series parts come in 14. 20, 28 and 32-pin packages, with up to 64k flash/8k ram (or as little as 16k and 2k), packaged in SOIC-14, SOICW-20/28, VQFN-20/28/32 (same 3mm x 3mm that tinies got for VQFN-20 - but not available for 64k parts). SSOP-28, DIP28 and TQFP-32. Their errata list only has three issues that hardly anyone will care about. They run faster, use less power, aren't that much more expensive... oh and they've got MVIO too. Yeah they pretty much blow the doors off the tinyAVRs a generic 1-on-1 fight - though the tinies still hold onto some of their advantages (CCL pins, and the EBs only have a single UART. The only real thing the tinies can hold up as an advantage is that the 14 and 20-pin tinies do better at maximizing the availibility of peripheral functions on pins, while the DD-series instead maintains a coherant and consistent layout, at the cost of some flexibility at the low pincounts - the one thing conspicuously absent is that fancy new ADC like the 2-series has, which won't reach 14-pin parts from the "double-letter" AVRs until the EB-series, though it comes to 28, 32, and 48-pin packages sooner in the EA-series. EA-series armies will be reinforcing their positions within the tinyAVR highlands, with flash from 8k & 1k ram to 64k and 6k, as the EB-series masses for a full scale invasion with 14, 20, 28 and 32-pin parts, armed with two new waveform generating timer types, and sized at the decidedly tinyAVR-like 8k flash/1k ram to 32k flash 3k ram; they look like they're aiming for a low price point with those for sure, with those kind of memory sizes and pincounts, and only 1 UART.

## One notices a patern with their recent releases
Count how many parts were released in each family since 2020. The AVR DA and DB got 11 parts each (4 pincounts 3 memory sizes, minus the 32k/64-pin). The tinyAVR 2-series got 12 parts (3 pincounts, 4 memory sizes). DD got 12 parts (4 pincounts, 3 memory sizes) The EA-series gets 12 parts (3 pincounts 4 memory sizes) and the EB will be 12 (4 pincounts 3 memory sizes). Even the DU, whose product brief appeared only for a few hours (it was a very brief brief) listed 11 parts (4 pincounts and 3 memory sizes, and no 14 pin/64k version). It would appear that Microchip Doctrine is to release parts in squads of a dozen, or whatever the opposite of a baker's dozen is (bakers dozen is 13, 1 more than a dozen. A Microchip dozen is 11.).

My belief is that there are 6 DA/DB dies (1 per flash size (the different flash sizes initial release die rev was different for all three sizes), and the two big pincounts and 2 small pincounts share a die. The 2-series I think is 3 dies - 32k, 16k and 8/4k, while the DD is 4 (16/32k and 64k, and again the two big and two small parts share a die). EA I predict will use 1 die across all pincounts, and end up with either 3 or 4 dies depending on whether 16 and 8 are made with the same die or not. EB and DU if it happens will likely both be like the DD in this regard.

These patterns break down when you look further back to the first modern AVRs (0/1-series tinies and mega0) - but those were the chaotic days right after the 2016 revolution. They came extremely close to releasing a 3214 - even had headers for it in the ATpack, then removed them.
