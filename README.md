<!-- markdownlint-disable MD041 -->
### [Check it out, we have "discussions" now!](https://github.com/SpenceKonde/megaTinyCore/discussions)
Let's use that - it's better than gitter ever was, and it's all on one site.

#### [Wiring](Wiring.md)
#### [Installation](Installation.md)

# **megaTinyCore** - Supports ALL post 2016 tinyAVR w/2k+ flash
An Arduino core for the tinyAVR 0-series, 1-series, and now the 2-series. These parts have an improved architecture compared to the "classic" tinyAVR parts (which are supported by [ATTinyCore](https://github.com/SpenceKonde/ATTinyCore)), with improved peripherals and improved execution time for certain instructions (these are similar in both regards to the advanced AVR Dx-series, as well as megaAVR 0-series chips like the ATmega4809 as used on the official Nano Every and Uno Wifi Rev. 2 - although the Arduino team has done their best to kneecap them) in the low-cost, small packages typical of the ATtiny line. All of these parts feature at least one hardware UART, and an SPI and TWI interface (none of that USI garbage like, for example, the ATtiny85 has), a powerful event system, configurable custom logic, at least one on-chip analog comparator, a surprisingly accurate internal oscillator, and in the case of the 1-series, an actual DAC output channel, and in the case of the 2-series, a fancy differential ADC.

Moreover, the 0/1/2-series parts are *cheap* - the highest end parts, the 3216 and 3217, with 32k of flash and 2k of SRAM (same as the atmega328p used in Uno/Nano/ProMini) run just over $1 USD, and under $.90 in quantity - less than many 8k classic AVR ATtiny parts (AVR instruction set, at a PIC price). All of these parts are rated to run at 16 MHz or 20 MHz (at 4.5-5.5v) without an external crystal, and the internal oscillator is accurate enough for UART communication.

These use UPDI programming, not traditional ISP like the classic ATtiny parts did. See below for more information - you can use a classic 328p-based Arduino as programmer v jtag2updi, a serial adapter and a resistor (and preferably a diode) via included pymcuprog tool, one of the Microchip programmers (the mEDBG/nEDBG/EDBG-based programmers on their development board, Atmel-ICE or SNAP) or any UPDI programming tool that emulates one of those (which, to my knowledge, all of them do - if there is one that needs support added. please open an issue to let me know!)

A serial bootloader, Optiboot_x (based on the same codebase as the classical Arduino Uno bootloader) is supported on these parts (0/1-series support is currently live, 2-series is expected by the first week of May; adjustments for the new parts are trivial), allowing them to be programmed over a traditional serial port. See the Optiboot section below for more information on this, and the relevant options. Installing the bootloader does require a UPDI programmer. The assembled breakout boards I sell on Tindie are available pre-bootloaded (they are bootloaded on demand). **That being said** the user experience with Optiboot is a little disaoppointing on the 0/1-series parts as well as the 14-pin 2-series parts due to their lack of a hardware reset pin that could be used with the usual autoreset circuit to automatically reset into the bootloader when the serial port is opened - you need to either disable UPDI programming entirely (requiring an HV programmer if fuse settings or bootloader need to be change after initial bootloading) or leave UPDI enabbled, but start any upload within 8 seconds of applying power. The 20-pin and 24-pin 2-series parts support an "alternate reset pin" allowing these to act more like a traditional Arduino.

**Arduino 1.8.13 is recommended**

When megaTinyCore is installed through board manager, the required version of the toolchain is installed automatically. Manual installation - assuming you want support for the 2-series is more complicated; see the [Installation guide for more information](Installation.md).

## Using this document
This document is best viewed online if you isntalled via board manager - [https://githup.com/SpenceKonde/megaTinyCore](https://githup.com/SpenceKonde/megaTinyCore)

## UPDI programming
The UPDI programming interface is a single-wire interface for programming (and debugging - **U**niversal **P**rogramming and **D**ebugging **I**nterface) used on the tinyAVR 0/1/2-series, as well as all other modern AVR microcontrollers. While one can always purchase a purpose-made UPDI programmer from Microchip, there are two very low-cost approaches to creating a UPDI programmer, both of which the Arduino community has more experience with than those official programmers.

### From a USB-Serial adapter (pyupdi-style - recommended)
Before megaTinyCore existed, there was a tool called [pyupdi](https://github.com/mraardvark/pyupdi) - a simple python program for uploading to UPDI-equipped microcontrollers using a serial adapter modified by the addition of a single resistor. But pyupdi was not readily usable from the Arduino IDE, and so this was not an option. As of 2.2.0, megaTinyCore brings in a portable Python implementation, which opens a great many doors; Originally we were planning to adapt pyupdi, but at the urging of its author and several Microchip employees, we have instead based this functionality on [pymcuprog](https://pypi.org/project/pymcuprog/), a more robust tool developed and maintained by Microchip which includes the same serial-port upload mechanism. **If installing manually** you must [add the python package](https://github.com/SpenceKonde/megaTinyCore/blob/master/megaavr/tools/ManualPython.md) appropriate to your operating system in order to use this upload method (a system python installation is not sufficient, nor is one necessary).

As of 2.3.2, with the dramatic improvements in performance, and the proven reliability of the wiring scheme using a diode instead of a resistor, and in light of the flakiness of the jtag2updi firmware, this is now the recommended programming method. As of this version, programming speed has been increased by as much as a factor of 20, and now far exceeds what was possible with jtag2updi (programming via jtag2updi is roughly comparable in speed to programming via SerialUPDI on the "SLOW" speed option, 57600 baud; the normal 230400 baud version programs about three times faster than the SLOW version or jtag2updi, while the "TURBO" option (runs at 460800 baud and increases upload speed by approximately 50% over the normal one. The TURBO speed version should only be used with devices running at 4.5v or more, as we have to run the UPDI clock faster to keep up (it is also not expected to be compatible with all serial adapters - this is an intentional tradeoff for improved performance), but it allows for upload and verification of a 32kb sketch in 4 seconds.

The connections needed to [convert a serial adapter to a UPDI programmer are detailed here](https://github.com/SpenceKonde/AVR-Guidance/blob/master/UPDI/jtag2updi.md); see also the [docs for SerialUPDI itself](https://github.com/SpenceKonde/megaTinyCore/blob/master/megaavr/tools/README.md)

### With a classic Arduino (jtag2updi)
One can be made from a classic AVR Uno/Nano/Pro Mini; inexpensive Nano clones are the usual choice, being cheap enough that one can be wired up and then left like that - see [Making a UPDI programmer](https://github.com/SpenceKonde/AVR-Guidance/blob/master/UPDI/jtag2updi.md); using these, you should select jtag2updi from the tools->programmer menu. This was previously our recommended option; due to persistent jtag2updi bugs, and it's reliance on the largely unmaintained 'avrdude' tool (which among other things inserts a spurious error message into all UPDI uploads made with it), this is no longer recommended.

## Supported Parts (click link for pinout diagram and details)
* tinyAVR 2-series
  * [ATtiny3227,1627,827,427](https://github.com/SpenceKonde/megaTinyCore/blob/master/megaavr/extras/ATtiny_x27.md)
  * [ATtiny3226,1626,826,426](https://github.com/SpenceKonde/megaTinyCore/blob/master/megaavr/extras/ATtiny_x26.md)
  * [ATtiny3224,1624,824,424](https://github.com/SpenceKonde/megaTinyCore/blob/master/megaavr/extras/ATtiny_x24.md)
* tinyAVR 1-series
  * [ATtiny3217,1617,817,417](https://github.com/SpenceKonde/megaTinyCore/blob/master/megaavr/extras/ATtiny_x17.md)
  * [ATtiny3216,1616,816,416](https://github.com/SpenceKonde/megaTinyCore/blob/master/megaavr/extras/ATtiny_x16.md)
  * [ATtiny1614,814,414,214](https://github.com/SpenceKonde/megaTinyCore/blob/master/megaavr/extras/ATtiny_x14.md)
  * [ATtiny412,212](https://github.com/SpenceKonde/megaTinyCore/blob/master/megaavr/extras/ATtiny_x12.md)
* tinyAVR 0-series
  * [ATtiny1607,807](https://github.com/SpenceKonde/megaTinyCore/blob/master/megaavr/extras/ATtiny_x07.md)
  * [ATtiny1606,806,406](https://github.com/SpenceKonde/megaTinyCore/blob/master/megaavr/extras/ATtiny_x06.md)
  * [ATtiny1604,804,404,204](https://github.com/SpenceKonde/megaTinyCore/blob/master/megaavr/extras/ATtiny_x04.md)
  * [ATtiny402,202](https://github.com/SpenceKonde/megaTinyCore/blob/master/megaavr/extras/ATtiny_x02.md)

## Overall Part Comparison
| tinyAVR series      |     0-series   |     1-series        |      2-series       |
|---------------------|----------------|---------------------|---------------------|
| Pincounts           |  8, 14, 20, 24 |       8, 14, 20, 24 |          14, 20, 24 |
| Flash (max)         |         32768b |              32768b |    (planned) 32768b |
| RAM (max)           |          1024b |               2048b |              3072b  |
| Separate reset pin? |            NO  |                  NO |  20/24-pin optional |
| PWM pins (Arduino)  | 8pin: 4 else 6 |    8p:4, >=20p:8, 6 |                   6 |
| Type A timers       |              1 |                   1 |                   1 |
| Type B timers       |             1  |    <16k: 1 >=16k: 2 |                   2 |
| Type D timer        |            MO  |                 Yes |                  No |
| Real Time Clock     | Yes, 32k xtal  |                 Yes |                 Yes |
| USARTs    (pin options) |      1 (2) |               1 (2) |               2 (3) |
| SPI ports (pin options) |         1  | 1 (2 except 14-pin) |   1 (2 on 20/24pin) |
| TWI ports (pin options) |         1  |               1 (2) |                  1  |
| Maximum rated speed |         20 MHz |              20 MHz |              20 MHz |
| Overclocking (internal @ 5v) |   ??? |           25-30 MHz |              32 MHz |
| Overclocking (ext. clk @ 5v) |   ??? |              32 MHz |           >= 32 MHz |
| External crystal    |            NO  |                  NO |                  NO |
| Event Channels      | 1 sync 2 async |      2 sync 4 async |    6, no sync/async |
| CCL Logic Blocks    |     2 (1 pair) |          2 (1 pair) |         4 (2 pairs) |
| Analog Comparators  |  1, no DAC REF |        3, w/DAC REF |        1, w/DAC REF |
| ADC                 |         10-bit |   10-bit (>=16: 2x) |   12-bit diff w/PGA |
| Analog References   | .55V, 1.1V, 1.5V, 2.5V, 4.3V | .55V, 1.1V, 1.5V, 2.5V, 4.3V | 1.024V, 2.048V, 2.5V, 4.096V |

Overclocked speeds are typical maximum speeds that can be reached with basic functionality intact. Not all parts will reach or function at these speeds, and the operating temperature range is likely far narrower than it is at the rated speeds. I'm aware of nobody who has played with overclocking the 0-series, but as it was released at the same time as the 1-series and appears to be a 1-series with fewer features, I would expect them to be the same - though if they turned out not to be, one could rationalize a difference in either direction ("they're made with a less advanced process" if they overclock worse, or "there's more to go wrong on the 1-series")

The ADC on the 2-series is arguably the best ADC that has shipped on an AVR yet. The closest comparisons are the classic AVRs that got differential ADCs (the t841, mega2560 and t861 come to mind as having very good differential ADCs). While it isn't capable of the insane 100x and 200x gain that some parts bragged of in the classic AVR days, I'm under the impression that much of that was just being used to very precisely measure noise - and the flexibility of the new ADC miles beyond what the classicAVRs were capable of: 7 pina capavble of being negative inputs, and no restrictions on what inputs they can be paired with, gain selectable to every power of 2, and the autoaccumulation for decimation.

The type D timer is only used for PWM on 20/24 pin 1-series parts - on the smaller parts, it wouldn't let us increase the total number of PWM pins - Only the WOC and WOD pins (on PC0 and PC1 respectively) don't already have TCA-driven PWM on them. As such, since analogWrite() does not support any features that would be enabled by turning off split mode (like 16-bit PWM) or enhanced by using the type D timer (like adjusting the frequency), it would just be worse, because it would require additional space to store the routine to turn on and off PWM from two types of timer, instead of one.

The type B timers are lousy at generating PWM for a variety of reasons, and with the type B timers being so incredibly useful for other purposes, TCB-driven PWM is not supported on megaTinyCore, nor is that planned for a future version. If I thought people would stand for this, I'd drop it from DxCore too.

### Automotive (VAO) versions
The automotive versions should also work. You must always select the 16 MHz-derived clock speeds on these parts. They do not support 20 MHz operation.

## Support for official Microchip developmenmt boards
As of version 2.4.0, we now provide an "Official Microchip Board" option. This doesn't do anything special other than defining `LED_BUILTIN` to be the pin that has the LED on that board, instead of A7, and defining a macro `PIN_BUTTON_BUILTIN` defined as the pin with the user button on it and making "upload" with the non-optiboot version always use the onboard programmer/debugger, and tools -> programmer will be used only for "burn bootloader" and "upload using programmer". In the case of the ATtiny416 XPlained Nano, it also selects the version of the bootloader that uses the alternate pins for the serial port - it does not automatically use the alternate pins for USART0 as if you'd done Serial.swap(1) yet - functionality to support default swapping of serial pins will come in a future update, alongside some other changes in the machinery underlying the pinswap mechanism which will hopefully also reduce flash usage.

### Why does blink take more flash on the XPlained Mini vs the XPlained Pro? Even when both have the same ATtiny817
For the same reason that blink will take more flash if you change it to use `PIN_PC0` directly instead of letting the board definition to it for you via `LED_BUILTIN`: PC0, used on the XPlained Mini is a PWM pin, while PB4, used by the XPlained Pro is not. Since that is the only pin that digitalWrite() is being used on, the compiler is free to optimize away anything that isn't needed for digitalWrite() on that pin, including the functionality to turn off PWM output on a pin that supports PWM. The diference vanishes if digitalWrite() is also used on a pin that supports PWM on both devices (or if digitalWrite() is replaced with digitalWriteFast, which assumes that you won't call it on a pin outputting PWM).

## A word on terminology ("megaAVR")
In the official Arduino board definition for their "megaAVR" hardware package, they imply that the new architecture on the megaAVR 0-series parts (which is nearly the same as used on the tinyAVR 0-series and 1-series) is called "megaavr" - that is not an official term. Microchip uses the term "megaAVR" to refer to any "ATmega" part, whether it has the old style or modern peripherals. There are no official terms to refer to all AVR parts of one family or the other. In this document, prior to 2.0.2, we used the Arduino convention, and despite well over a year having passed since then, I still keep finding places where I call them megaAVR. Please report this using a github issue if you see any.
Do note that the terms `avr` and `megaavr` are still used internally (for example, in libraries, to mark which parts a given library is compatible with, or separate different versions of a file based on what they will run on). This will continue - we have to stick with this for compatibility with what the Arduino team started with the core for the Uno WiFi Rev. 2 and Nano Every - and in any event, *some word* is needed to refer to the two groups and Microchip hasn't provided one. Not sure how they managed to develop these very different parts without ever talking about them generally.

It is unfortunate that there are not officially sanctioned terms for these two classes of AVR microcontrollers. I have been referring the pre-2016 AVR devices (with PORTx, DDRx, etc registers for pins) as "classic AVR" and the ones Arduino calls megaavr as "modern AVR"

## Don't buy modern AVRs on AliExpress or from eBay international sellers
I buy a lot of electronics stuff on aliexpress. It's a great marketplace for things that are made by Chinese companies and are mostly generic. It is not a great place for the latest semiconductor product lines from major western manufacturers, especially in the midst of a historic shortage of said chips. The modern AVR devices people have bought on there (when they have been found at all - they're rarely offered) have been reported to be fake or defective (like ATtiny412s that think they're 416s). For that matter, you probablty don't want to buy any AVR microcontrollers on aliexpress... Assembled boards, like Arduino Nano clones, generally work if you avoid the ones with the third party LGT8 chips and watch out for the ones with the ATmega168p instead of the '328p - but there are a lot of reports of bogus microcontrollers (I have heard of fake ATtiny85s that were actually remarked ATtiny13s, for example). Buy them from reputable distributors like Digikey or Mouser (if in a hurry, and they're on backorder - which is the case for almost everything in QFN package as of July 2021 - buy from a USA/EU-based scalper).

## Buying tinyAVR 1-series and 2-series Breakout Boards
I sell breakout boards with regulator, UPDI header, and Serial header in my tindie shop, as well as the bare boards. Buying from my store helps support further development on the core, and is a great way to get started using these exciting new parts with Arduino. Currently ATtiny1624 boards are available, but the 20 and 24-pin parts will not be sold as an assembled board until a newly revised PCB design is back from the board house to enable autoreset on the alt-reset pin. There is also a 14-pin board revision coming - thought it is largely cosmetic. The yellow solder mask has got to go, as the readability seemed to get worse in the last several batches. The new boards also standardize a 0.6" spacing between the rows of pins, instead of the current 0.7" spacing, so you will be able to, for example, put machined pin header onto them and plug them into a wide-DIP socket, or use them with our prototyping board optimized for that row spacing. Assembled 0-series boards are being discontinued, and will not be restocked once they sell out. The same will happen for the 16k 2-series parts once the 32k ones are available.

### [ATtiny3217, 1607 assembled](https://www.tindie.com/products/17523/)
### [ATtiny3217/1617/817/417/1607/807 bare board](https://www.tindie.com/products/17613/)
### [ATtiny3216, 1606 assembled](https://www.tindie.com/products/17597/)
### [ATtiny3216/1616/816/416/1606/806/406 bare board](https://www.tindie.com/products/17614/)
### [ATtiny1624, 1614, and 1604 assembled](https://www.tindie.com/products/17598/)
### [ATtiny1614/814/414/1604/804/404 bare board](https://www.tindie.com/products/17748/)
### [ATtiny412, 402 assembled](https://www.tindie.com/products/17685/)
### [ATtiny412/212/402/202 bare board](https://www.tindie.com/products/17749/)

## TinyAVR 2-series

### Basics and comparison with 0/1-series
[Datasheet for the new tinyAVR 2-series](http://ww1.microchip.com/downloads/en/DeviceDoc/ATtiny1624-26-27-DataSheet-DS40002234A.pdf) - While datasheet only "covers" the 16k parts, they clearly state that there are no differences in features between parts with the same pin count (that is, there are no "golden" parts like the 16k/32k 1-series), only between parts with different pin counts. 14, 20, and 24 pin parts are all listed with 4k, 8k, 16k and 32k flash; these flash size options, respectively, come with 512, 1024, 2048, and 3072 bytes of SRAM (that is, the 4k and 8k parts have double the SRAM), 4/8k parts get 128 bytes of EEPROM, the larger ones get 256. 14-pin parts come in SOIC and TSSOP, 20-pin in (wide) SOIC, SSOP, and that itty-bitty QFN like the 1616 (think we'll get the 3226 in that package too? We can hope!), and 24-pin in the same VQFN as the 3217.

TWI, SPI, USART0, AC0, are unchanged, as is NVMCTRL (the changes required to the bootloader were solely in relation to supporting the second USART). Clock options unchanged. TCB0 and TCB1 got upgraded to the version in the Dx-series: clock off event option, cascade, and separate INTCTRL bits for OVF and CAPT - nice additions, but nothing relevant to the core itself), and all the parts have both TCB's. We now get 4 CCL LUTs and 2 sequencers, instead of 2 and 1 - and they can fire interrupts like other parts with CCL (and unlike the tinyAVR 0/1-series). One of the most exciting features is that, as expected, they have a second USART (that noise you hear is the ATtiny841 and and ATtiny1634 sobbing in the corner). PORTMUX registers now named like the rest of the modern AVRs - but we didn't lose the individual control over the pins for each TCA WO channel. EVSYS now works like it does on non-tinyAVR-0/1-series parts (which is a welcome change - the 0/1-series was the odd-one-out, and some of the ways in which their EVSYS was different sucked). The 1-series features of TCD0, AC1/2, DAC0, and ADC1 are *gone*. In their stead, ADC0 is much fancier and almost unrecognizable, the first new AVR released since the buyout that featured a real differential ADC. (queue another agonized wail from the poor '841, which also has an incredibly fancy ADC with great differential options, but which looks thoroughly dated next to the new ones/)... judging by the volume of posts on different topics that I've seem, I have a sense that differential ADC wasn't at the top of most of your wish-lists - but it was on the top of the major chip customers' lists, and so that's what we're getting one.

And it is really *really* fancy. The ADC gets a programmable gain amplifier (1x ~ 16x), single-ended and differential mode, a new series accumulation mode (where each sample is triggered by an event) and burst accumulation mode (like our current accumulate mode) - and every sample taken is made individually available, too (and you can have it generate interrupt on each sample or only the final result. They make a big deal about using the accumulate mode to do 1024 sample oversampling - that's 4<sup>5</sup>, so decimation should let us take meaningful 17-bit ADC readings. Rather annoyingly, it only offers 8 and 12-bit resolution options (no 10-bit - megaTinyCore provides a 10-bit resolution option for compatibility by rightshifting a 12-bit reading). It also supports ADC clock between 300khz and 6 MHz (3 MHz with internal reference; we try for approx. 2.5 MHz); to make up for the faster ADC clock, sample duration can be up to 256 ADC clocks; with minimum sample duration, over 300k samples/second is possible. It has more rigorous functionality to time the reference and input settling time too - no more throwing away the first ADC reading? (with parts in hand, I'm going to back away from this claim - I still get a garbage reading the first time). This new ADC is incredibly fancy - just remember that when you are using an accumulation mode, the magnitude of the noise is smaller, but offset error is just as big. Yo do need to compensate tfor thjis somehow (and rigorously too, not haowe )
megaTinyCore provides an analogRead() implementation, and more powerful functions to use the oversampling, PGA, (see the analog feature section below)

Oh, and one more thing... the UPDI pin configuration has the old options - UPDI, I/O, or Reset... and a new one: UPDI on PA0, with hardware RESET pin on PB4! Optiboot will finally be a viable and comfortable option! Well, on the parts that have a PB4, ie, not the 14-pin parts. Which also happen to be (if my Tindie store sales are any indication) the most popular version.

## megaTinyCore and tinyAVR 0/1/2-series Features
These parts (well, the 1/2-series at least - the 0-series is more of a budget option) provide an excellent toolbox of versatile and powerful peripherals; the top-end ones are on a par with or better than classic megaAVR parts - for a tinyAVR price. One of the guiding principles of the design of megaTinyCore, as with my other cores, is to allow the supported parts to reach their full potential - or as close to that as possible within the limitations of Arduino. This (very large) section covers the features of these parts and how they are exposed by megaTinyCore, as well as features of the core itself.

### Supported Clock Speeds
**Important - Read about [Tuning](megaavr/extras/Tuning.md) before selecting any tuned option!**
* 20MHz Internal (4.5v-5.5v - typical for 5v systems)
* 16MHz Internal (4.5v-5.5v - typical for 5v systems)
* 10MHz Internal (2.7v-5.5v - typical for 3.3v systems)
* 8MHz Internal (2.7v-5.5v - typical for 3.3v systems)
* 5MHz Internal (1.8v-5.5v)
* 4MHz Internal (1.8v-5.5v)
* 1MHz Internal (1.8v-5.5v)
* 20 MHz internal (tuned)
* 16 MHz internal (tuned)
* 12 MHz internal (tuned)
* 20MHz External Clock (4.5v-5.5v)
* 16MHz External Clock (4.5v-5.5v)
* 12MHz External Clock (2.7v-5.5v)
* 10MHz External Clock (2.7v-5.5v)
* 8MHz  External Clock (2.7v-5.5v)
* 24 MHz internal (tuned, overclocked)
* 25 MHz internal (tuned, overclocked)
* 30 MHz internal (tuned, overclocked) - 0/1-series require "20MHz" OSCCFG fuse setting; 2-series parts may or may not be able to reach 30 with "16 MHz" selected.
* 32 MHz internal (tuned, overclocked - 2-series only, and a very optimistic overclocking, may be unstable)
* 24MHz External Clock (Overclocked)
* 25MHz External Clock (Overclocked)
* 30MHz External Clock (Overclocked aggressively)
* 32MHz External Clock (Overclocked aggressively)


See [Speed Grades](https://github.com/SpenceKonde/megaTinyCore/blob/master/megaavr/extras/SpeedGrades.md) for more information on the manufacturer's speed grades. Note that those are the voltages and clock speeds at which it is guaranteed to work, where an unexpected glitch of some description could pose a hazard to persons or property. Under favorable temperatures (ie, room temperature), and with higher tolerance for potential stability issues than a commercial customer, the speed grades can easily be beaten. *Our testing has found that the official speed grades are extremely conservative* - I was able to run the 1-series parts I tested at 32MHz at room temperature at 5V with external oscillator (though it was sensitive to supply voltage changes), and it's been widely observed that they seem to work fine at their full 20 MHz 4.5V ~ 5.5V speed running from 3.3V. I make no claims as to their stability under conditions outside the manufacturer specifications, nor did I exhaustively test all peripherals, but there was nothing obviously broken, and adding 2 and 2 did still get a result of 4. From tuned internal oscillator, 30 MHz on 0/1 and 32 MHz on 2-series may even be stable (though much above that results in instability, and 2 + 2 sometimes is equal to 0). If you must run it outside the manufacturer specifications, I would suggest using the watchdog timer to reset it in the event that it hangs.  A part being run outside of the manufacturer specified operating conditions should never be used for any sort of critical task or even one where a failure would be inconvenient to recover from (such as a device which was difficult to access for maintenance or replacement).

These parts do not support using an external crystal like the classic ATtiny parts do, however the internal oscillator is tightly calibrated enough that the internal clock will work for UART communication without issue; Like the megaAVR 0-series, it includes clock corrections at 3V and 5V in the signature row. If you need particularly accurate UART baud rates more than you need the flash this option uses, you can enable it from the Tools submenu.

These parts also support an external **clock** (that can come from an external oscillator or - available from the usual suspects for a price nearly as high as the ATtiny itself, or from aliexpress for only prices that are slightly less exorbitant (search for "active crystal"), the CLKOUT from another processor, and so on).

### Memory-mapped flash: No need to declare PROGMEM
Unlike classic AVRs, on the these parts, *the flash is within the same address space as the rest of the memory*. This means `pgm_read_*_near()` is not needed to read directly from flash. Because of this, the compiler automatically puts any variable declared `const` into PROGMEM, and accesses it appropriately - you no longer need to explicitly declare them as PROGMEM. This includes quoted string literals, so the F() macro is no longer needed either (As of 2.1.0, F() once more explicitly declares things as living in PROGMEM (ie, it is slightly less efficient) in order to ensure compatibility with third party libraries).

However, do note that if you explicitly declare a variable PROGMEM, you must still use the pgm_read functions to read it, just like on classic AVRs - when a variable is declared PROGMEM on parts with memory mapped flash, the pointer is offset (address is relative to start of flash, not start of address space); this same offset is applied when using the `pgm_read_*_near()` macros. Do note that declaring things PROGMEM and accessing with `pgm_read_*_near` functions, although it works fine, is slower and wastes a small amount of flash (compared to simply declaring the variables const); the same goes for the F() macro with constant strings in 2.1.0 and later (for a period of time before 2.1.0, `F()` did nothing - but that caused problems for third party libraries, and the authors maintained that the problem was with the core, not the library, and my choice was to accept less efficiency, or deny my users access to popular libraries). Using the `F()` macro may be necessary for compatibility with some third party libraries (the specific cases that forced the return of `F()` upon us were not of that sort - we were actually able to make the ones I knew of work with the F()-as-noop code, and they took up a few bytes less flash as a result).

### Fast digital I/O
For timing-critical operations, we now support `digitalWriteFast()` and `digitalReadFast()`. The pin numbers passed to these functions **must** be a compile-time known constant; for best results, when using `digitalWriteFast()`, the pin value to be written should as well. Under those conditions, `digitalWriteFast()` will optimize down to a single instruction which executes in a single system clock cycle, and `digitalReadFast()` will optimize to 4 instructions (note that `if (VPORTx.IN&(1<n))` is still faster and smaller than `if (digitalReadFast(PIN_Pxn)==HIGH)` because testing a single bit within an I/O register and using that as the argument for a conditional branch can be optimized down to an SBIS/SBIC (**S**kip if **B**it in **I**/o register **S**et/**C**lear), while it requires several instructions to check that bit, and convert it to a boolean for further processing.

### Serial (UART) Support
All of the 0/1-series parts have a single hardware serial port (UART or USART); the 2-series parts have two. It works exactly like the one on official Arduino boards (except that there is no auto-reset, unless you've wired it up by fusing the UPDI pin as reset (requiring either HV-UPDI or the Optiboot bootloader to upload code), or set up an "ersatz reset pin" as described elsewhere in this document). See the pinout charts for the locations of the serial pins.

Prior to putting the part into a sleep mode, or otherwise disabling it's ability to transmit, be sure that it has finished sending the data in the buffer by calling `Serial.flush()`, otherwise the serial port will emit corrupted characters.

#### Pin Swapping
On all parts, the UART pins can be swapped to an alternate location. This is configured using the `Serial.swap()` or `Serial.pins()` methods. Both of them achieve the same thing, but differ in how you specify the set of pins to use. This must be called **before** calling `Serial.begin()`.

`Serial.swap(1) or Serial.swap(0)` will set the the mapping to the alternate (1) or default (0) pins. It will return true if this is a valid option, and false if it is not (you don't need to check this, but it may be useful during development). If an invalid option is specified, it will be set to the default one.

`Serial.pins(TX pin, RX pin)` - this will set the mapping to whichever mapping has the specified pins as TX and RX. If this is not a valid mapping option, it will return false and set the mapping to the default. This uses more flash than `Serial.swap()`; that method is preferred.

Bootloaders are available for both UART mappings; the UART bootloader option selected when you do "burn bootloader" for an Optiboot board definition is the serial port that the uploaded bootloaded will use.

**tiny 2-series warning** On the tinyAVR 2-series, there are two serial ports, `Serial` and `Serial1`. Each of can use one of two pin options (except on 14-pin parts, where the alternate pins for `Serial1` are not present). The *default* pins for `Serial1` are the same as the *alternate* pins for `Serial`.

In versions prior to 2.0.0, this was instead configured using the Tools -> UART Pins menu. If you're still using such ancient versions, please update.

#### Baud Rates
When operating at 1MHz, the UART can reach 115200 baud (2.3.0+ required for stable operation).

Maximum baud rate scales directly with the system clock speed; 2 mbaud is just barely possible at 16 MHz (this is the same maximum that was possible on classic AVRs); unlike classic AVRs, these have a "fractional baud rate generator": rather than specifying the bit period in units of 8 (U2X=1) or 16 (U2X=0) clock cycles as on classic AVRs, here it is specified in 64ths of 8 or 16 clock cycles (no, I'm not sure how they pulled that off. Seems to be established technology, but it sure looks like magic to me).  This is a boon at high baud rates - it guarantees that as long as the system is clocked high enough to generate the requested baud rate, worst case, the bit timing will be off by no more than 1/129th - less than 0.77%. At lower speeds, the error is far smaller, often negligible - for the speeds supported by megaTinyCore, the commonly used baud rates (9600, 19200, 57600 and 115200) are all within 0.1% except for 115200 @ 1 MHz (0.64%).  Of course, that is error in addition to any inaccuracy due to the internal oscillator being off-target; even if the best OSC20MCALA value is chosen by the factory cal (often but not always the case), on the 0/1-series, the increment size is 1.5%, so there's an a +/- 0.75% error - or more, because occasionally the factory calibration is 1 setting off from the ideal. On 2-series it's half that. Either way, you safe almost guaranteed to be within 1%, with is more than enough for most serial communication. If you need it even closer (really?) you can select the voltage baud correction option from the tools menu; this will use the factory calibrated oscillator error values to nudge the baud rate even closer to the target.

##### Voltage correction
To maximize the accuracy of the baud rate when using the internal oscillator, from the Tools -> Voltage for UART Baud menu, select whether the voltage is closer to 5v or 3v - the factory calibration supplies an oscillator error adjustment for the purpose of UART baud calculation for 5v and 3v, and using the right one will produce a baud rate closer to the target value. That said, testing has indicated that either setting is good enough unless you are talking to particularly finicky devices. On parts with 4k or less of flash, this setting is ignored because it adds approx 90 bytes to the size of the compiled binary, and on 2k and 4k parts, it's just not worth the space. This correction factor is never used by the bootloader - it is only necessary when you'rte interacting with something particularly picky about then baud rate which will not accept a nearly correct baud.

#### printHex
As an aid to development and debugging, we provide the printHex method. This is a method that will print a wide variety of things as hex. Passing pointers to it will usually dereference them and return the incremented pointer (so you could call it in a loop) See [the printHex documentation](https://github.com/SpenceKonde/megaTinyCore/blob/master/megaavr/extras/printHex.md) for more information. printHex is a development and debugging aid. Behavior may change without warning in a future release - the newline behavior in particular is worse than I would like it to be, but so is every way I've tried to make it better so far.

### SPI support
All of these parts have a single hardware SPI peripheral. It works exactly like the one on official Arduino boards using the SPI.h library. See the pinout charts for the location of these pins. Note that the 8-pin parts (412, 212, 402, 204) do not have a specific SS pin.

On all parts except the 14-pin parts, the SPI pins can be moved to an alternate location (note: On 8-pin parts, the SCK pin cannot be moved). This is configured using the SPI.swap() or SPI.pins() methods. Both of them achieve the same thing, but differ in how you specify the set of pins to use. This must be called **before** calling SPI.begin().

`SPI.swap(1) or SPI.swap(0)` will set the the mapping to the alternate (1) or default (0) pins. It will return true if this is a valid option, and false if it is not (you don't need to check this, but it may be useful during development). If an invalid option is specified, it will be set to the default one.

`SPI.pins(MOSI pin, MISO pin, SCK pin, SS pin);` - this will set the mapping to whichever mapping has the specified pins. If this is not a valid mapping option, it will return false and set the mapping to the default. This uses more flash than SPI.swap(); that method is preferred.

When it can be determined that arguments passed to SPI.swap() will always be invalid at compile time, that will generate a compile error to that effect - but we have no way other than returning false (which is ignored unless the returned value is checked) to let the user know that that is the reason that SPI isn't doing what they want.

In versions prior to 2.0.0, this was instead configured using the Tools -> SPI Pins submenu; this is set at compile time (reburning bootloader is not required). In these versions, you could see whether the alternate pins are in use by checking if SPIREMAP is #defined - you can for example use it to check that the correct options are selected and terminate compilation so you can select the right option if that is the case.

This core disables the SS pin when running in SPI master mode. This means that the "SS" pin can be used for whatever purpose you want - unlike classic AVRs, where this could not be disabled. Earlier versions of this document incorrectly stated that this behavior was enabled in megaTinyCore; it never was, and SS was always disabled. It should be re-enabled and the SS pin configured appropriately (probably as INPUT_PULLUP) if master/slave functionality is required.

### I2C (TWI) support
All of these parts have a single hardware I2C (TWI) peripheral. It works *almost* exactly like the one on official Arduino boards using the Wire.h library, except for the additional features noted below. See the pinout charts for the location of these pins. **You must be sure to use external pullup resistors on the SDA and SCL lines if the devices you are connecting do not have those on board** (many Arduino/hobby targeted breakout boards do - typically 10k - though these may need to be enabled with a solder-bridge or jumper). The 30k-50k internal pullup resistors are not suitable for I2C pullup resistors; while they were enabled by megaTinyCore prior to version 2.1.0, this was the worst of both worlds: they often did work with the simple test case, leading the developer on their merry way thinking they were all set, only to discover that when they added another I2C device or two, or moved the device to the end of a longer cable, I2C suddenly no longer worked - it's probably better for it to fail immediately, prompting investigation and subsequent addition of appropriate pullup resistors. Note that there is nothing *preventing* one from enabling the internal pullups manually - you just should do so knowing that even if it happens to work, it's not a robust solution.

#### TWI pins
On all 1-series parts with at least 14 pins, the TWI can be swapped to an alternate pair of pins. Neither 0-series nor 2-series parts have alternate pin locations for the TWI. On 2.0.0 and later, this is configured using the Wire.swap() or Wire.pins() methods. Both of them achieve the same thing, but differ in how you specify the set of pins to use. This should be called **before** Wire.begin(). This implementation of pin swapping is the same as what is used by[DxCore](https://github.com/SpenceKonde/DxCore) and [MegaCoreX](https://github.com/MCUdude/MegaCoreX).

`Wire.swap(1) or Wire.swap(0)` will set the the mapping to the alternate (1) or default (0) pins. It will return true if this is a valid option, and false if it is not (you don't need to check this, but it may be useful during development). If an invalid option is specified, it will be set to the default one.

`Wire.pins(SDA pin, SCL pin)` - this will set the mapping to whichever mapping has the specified pins as SDA and SCL. If this is not a valid mapping option, it will return false and set the mapping to the default. This uses more flash than Wire.swap(); that method is preferred.

In versions prior to 2.0.0, this was instead configured using the Tools -> I2C Pins submenu; this is set at compile time (reburning bootloader is not required). In these versions, you could see whether the alternate pins are in use by checking if TWIREMAP is #defined - you can, for example, use it to check whether correct options are selected and terminate compilation, so you can select the right option.

#### General Call and Second Slave Addresses
When the version of Wire.h supplied with megaTinyCore 1.1.9 and later is used in slave mode, it is now possible to respond to the general call (0x00) address as well. This is controlled by the optional second argument to Wire.begin() (thanks [@LordJakson](https://github.com/LordJakson)!) If the argument is supplied and true, general call broadcasts will also trigger the interrupt. There is furthermore a third optional argument, which is passed unaltered to the `TWI0.SADDRMASK` register. If the low bit is 0, and bits set 1 will cause the I2C hardware to ignore that bit of the address (masked off bits will be treated as matching). If the low bit is 1, it will instead act as a second address that the device can respond to. See the datasheet for more information about this register.

#### Multi-master support - acting as both slave and master
This functionality is supported by the hardware (on the same pins, not same-or-different pins like the dual mode on full size parts) is coming, hopefully in the 2.4.x timeframe! Code has been offered, but I need a stable version out the door first, amd 2.4.0 is holding up too much to delay further.

#### TWI buffer size
The Wire library requires a TX and RX buffer; these are both the same size, determined by how much SRAM the part has. Trying to use Wire to send something too large for the buffer will result in the extra bytes just not being sent. The buffer sizes were revised in 2.3.0 to reduce the number of "oddball" parts - all tinyAVR parts supported by megaTinyCore now get 32 byte buffers except the 2k parts, which literally cannot fit buffers that large in their limited SRAM. The 2k parts also don't have enough flash to do much with the Wire library, anyweay. There is little room to improve the flash footpring of the full Wire library further, so I2C use on such 2k parts will likely require alternative iplementations to fit within the flash. (we do not recommend using the 2k parts at all, due to how lousy the developer experience is under such tight constraints.

| Parts                  |   SRAM   | Buffers (2.3.0+) | Buffers (pre-2.3.0) | Notes:                                                                            |
|------------------------|----------|------------------|---------------------|-----------------------------------------------------------------------------------|
| 2k 0/1-series          |     128b |              16b |                 16b | Wire is basically unusable w/2k flash anyway, so this is largely academic         |
| 4-8k 0/1, 4k 2-series  | under 1k |              32b |                 16b | 32b buffer is really rough even w/256b - but 16b breaks libraries in confusing    |
| other tinyAVR, AVR16DD |     1-4k |              32b |                 32b | ways. Now all tinyAVR with >128b ram has 32b I2C TX and RX buffers                |
| AVR32DA/DB/DD parts    | Under 8k |             130b |                 64b | Official 4809 core does 128b, we add 2 more to allow for 128 byte page writes to  |
| AVR128Dx and AVR64Dx   | 8 or 16k |             130b |                128b | 512kbit+ (64KByte) AT24 I2C memory/eeprom chips which require 2 address bytes     |

#### *almost?*
There is one other rather significant change, though it is transparent to almost everyone. The official Arduino megaavr core has TwoWire extending HardwareI2C, which extends Stream. We just extend Stream. HardwareI2C defines a considerable number of virtual member functions. This results in all of those methods being included in the final binary *regardless of whether they are called*. A master needs all the slave functions even if it never operates as a slave. A slave needs all the master functions, even if it never operates as a master. And the 4k parts can just forget about using Wire.h. Nobody has been able to express how this benefits the user, or why it was done other than that the 4809 is all that core was written to support and it has enough flash to get away with doing that, nor has anyone reported any problems caused by it.

### PWM support
The core provides hardware PWM via the standard `analogWrite()` function. On the 8-pin parts (412, 212, 402, 204), 4 PWM pins are available. On all other parts except 1-series parts with 20 or 24 pins, 6 PWM pins are available, all driven by Timer A. The 20 and 24 pin 1-series parts have two additional pins, driven by TCD0. The 2-series apparently traded TCD0 for a second serial port and a super-fancy ADC - those parts also all have 6 PWM pins. The Type B timers cannot be used for additional PWM pins - their output pins are the same as those available with Timer A and they are often too useful to justify using a whole TCB for  - however you can take them over if you need to generate PWM at different frequencies, though the fact that the prescaler cannot differ from the type A timer limits this use as well. See the pinout charts for a list of which pins support PWM.

**Note that TCA0 (the type A timer) on all parts is configured by the core at startup to operate in split mode in order to support the most PWM pins possible with analogWrite(). As of the 2.2.x versions, a `takeOverTCA0()` function has been added, which can be called to instruct the core not write to TCA0-registers nor assume  any particular mode or behavior for TCA0. analogWrite() will not generate PWM except on pins driven by TCD0 on the 20/24-pin parts nor will digitalWrite turn it off if you want to reconfigure TCA0 for other purposes, please refer to the below guide and "hard reset" the timer back to stock configuration.**
#### [Taking over TCA0](https://github.com/SpenceKonde/megaTinyCore/blob/master/megaavr/extras/TakingOverTCA0.md)

#### PWM on TCD0 pins (PIN_PC0 and PIN_PC1 - 10,11 on x16, 12,13 on x17)
The 3216, 1616, 816, 416, and the 3217, 1617 and 817 have two additional PWM pins driven by Timer D (PC0 and PC1 - pins 10 and 11 on x16, 12 and 13 on x17). Timer D is an async timer, and the outputs can't be enabled or disabled without briefly stopping the timer. This results in a brief glitch on the other PWM pin (if it is currently outputting PWM), and doing so requires slightly longer - though the duration of this glitch is under 1 us. If TCD is used as the millis timer - which is the default on any part that has a type D timer (in order to keep the timers that are more readily repurposed available - TCD0 is not an easy peripheral to work with), this will result in millis losing a very small amount of time (under 1 us) every time PWM is turned on or off on a TCD pin. Since

As of 2.2.0, analogWrite of 0 or 255 on a TCD-driven PWM pin does not disconnect the pin from the timer - instead it results in a constant HIGH or LOW output without disconnecting the timer(use digitalWrite() for that). This means that analogWrite(PIN_PC0 or PC1, 0) can be used to connect the timer to the pin without outputting PWM (yet) - doing this on both pins prior to setting any other duty cycles would allow one to ensure that no glitch of any sort occurs on the other TCD0 pin when the second pin is connected to it. Only digitalWrite() will disconnect the timer from the pin. When outputting a HIGH in this way, the pin is "inverted"; this means that digitalRead() on it will return 0, not 1 (if you're digitalRead'ing a pin, which you have set to output a constant HIGH, using analogWrite, and it's one of those two pins, it will read LOW - however, if you are using digitalRead() on a pin that you've set to output a constant value, you may be doing something wrong in general.

There is a benefit of having an async timer, however: megaTinyCore configures it to use the unprescaled internal oscillator as it's clock source. This means that while there is a difference in PWM frequency between 16-MHz derived and 20-MHz derived clocks, there is no change in frequency for different system clock speeds! The exception to this is when TCD0 is used as the millis/micros timing source at 1 MHz - running at full speed there resulted in spending an unreasonable fraction of runtime in the millis ISR.

On some versions of megaTinyCore prior to 2.2.0, PWM on the TCD0 pins was entirely broken.

**For general information on the available timers and how they are used PWM and other functions, consult the guide:**
This also covers the PWM frequencies that these timers will give you at various system clocks.
#### [Timers and megaTinyCore](https://github.com/SpenceKonde/megaTinyCore/blob/master/megaavr/extras/PWMandTimers.md)


### Tone Support
Support for tone() is provided on all parts using TCB0, unless TCB1 is present and TCB0 is set as millis source. This is like the standard tone() function. Unlike on some classic AVRs, it does not support use of the hardware output compare to generate tones - due to the very limited PWM capabilities and restricted prescaler selection for the TCBs, this is not practical. See caveats below if using TCB0 or TCB1 for millis/micros settings. On parts with 8k or less flash, tones which specify a duration longer than 65536ms and a high frequency (specifically where duration times frequency is >4.2 billion) are not supported - if specified at compile time it will generate a compile error, and at runtime, it will produce a shorter duration tone than expected - this is a trade-off to reduce flash usage. It was decided that requesting a tone over a minute long (several minutes continuous at the upper limit of human hearing would be required to trigger the issue, and it would need to be even longer as the frequency got lower) qualifies as highly abnormal usage of tone(). At that point, rather than counting the number of times the tone pin is toggled (like tone() with a duration specified does), it probably makes more sense to use durationless tone(), and turn it off with noTone(). Or, for the sake of everyone within earshot, maybe don't make your device emit a high pitched tone for minutes on end at all? On parts with 16k flash or more a slightly less flash efficient implementation is used and the limit is significantly higher and very unlikely to be encountered.

### millis/micros Timekeeping Options
megaTinyCore provides the option to use any available timer on a part for the millis()/micros timekeeping, controlled by a Tools submenu - or it can be disabled entirely if needed to save flash, allow use of all timer interrupts, or eliminate the periodic background interrupt. By default, TCD0 will be used by on parts that have one - otherwise TCA0 will be used (in versions prior to 1.1.9, TCA0 was used by default on parts that could output PWM with TCD0 on pins not available for TCA0 PWM). All timers available on the parts can be used: TCA0, TCD0 (on parts that have it), TCB0, TCB1 (where present) and the RTC. Many of these - particularly the non-default options, involve tradeoffs. In brief, TCA0 is a very versatile timer that users often want to reconfigure, TCD0 loses a small amount of time when PWM is turned on or off on the two TCD0 PWM pins (10,11 on 20-pin parts, 12,13 on 24-pin parts), TCB0 conflicts with Servo and tone on parts that don't have TCB1, and when the RTC is used micros() is not available at all because the clock isn't fast enough. With these limitations in mind, the timer selection menu provides a way to move millis()/micro to the timer most appropriate for your needs.

For more information, on the hardware timers of the supported parts, and how they are used by megaTinyCore's built-in functionality, see the [Timers and megaTinyCore](https://github.com/SpenceKonde/megaTinyCore/blob/master/megaavr/extras/PWMandTimers.md)

2.3.0 fixed a long-standing (though surprisingly low impact) "time travel" bug where the

#### RTC timer for millis
If the RTC is selected as the timer for millis timekeeping, micros will not be available. Additionally, this timer is configured to run while in STANDBY sleep mode. This has two important consequences: First, it will keep time while in sleep. Secondly, every 64 seconds, the RTC overflow interrupt will fire, waking the chip - thus, if you are using the RTC for millis and putting the part into sleep, you should declare a volatile global variable that you set in the ISR that is supposed to wake the part, eg 'volatile boolean ShouldWakeUp=0;' - set it to 1 in the ISR, and when you put the ATtiny to sleep, have it check this immediately after waking, going back to sleep if it's not set, and clearing it if it is, e.g.:

```cpp
void GoToSleep() {
    do {
        sleep_cpu();
    } while (!ShouldWakeUp)
    ShouldWakeUp=0;
}
```

This functionality will be made easier to use via the megaTinySleep library in a future version of the core.

This board package also supports using an external 32.768khz crystal as the clock source for the RTC (not supported on 0-series or 8-pin parts). If this is used, make sure that the crystal is connected between the TOSC1 and TOSC2 pins (these are the same as the TX and RX pins with the default pin mapping, unfortunately), that nothing else is, that no excessively long wires or traces are connected to these pins, and that appropriate loading capacitors per crystal manufacturer datasheet are connected. Since the TOSC1 and TOSC2 pins are the same pins used for serial, you must use the alternate serial pins.

### ADC Support
These parts all have a large number of analog inputs (11 pins on 20 and 24-pin 0/1-series, 15 on 2-series, 9 on 14-pin parts and 5 on 8-pin parts.) - plus the one on the UPDI pin which is not totally usable because the UPDI functionality). They can be read with analogRead() like on a normal AVR. While the `An` constants (ex, `A3`) are supported, and will read from the ADC channel of that number, they are in fact defined as then digital pin shared with that channel. Using the `An` constants is deprecated - the recommended practice is to just use the digital pin number, or better yet, use `PIN_Pxn` notation when calling analogRead(). Particularly with the release of 2.3.0 and tinyAVR 2-series support, a number of enhanced ADC features have been added to expose more of the power of the sophisticated ADC in these parts.

#### Reference Voltages
Analog reference voltage can be selected as usual using analogReference(). Supported reference voltages are listed below:

| tinyAVR 0/1-series                     | tinyAVR 2-series                        |
|----------------------------------------|-----------------------------------------|
| `VDD` (Vcc/supply voltage - default)   | `VDD` (Vcc/supply voltage - default)    |
| `INTERNAL0V55`                         | `INTERNAL1V024`                         |
| `INTERNAL1V1`                          | `INTERNAL2V048`                         |
| `INTERNAL1V5`                          | `INTERNAL4V096`                         |
| `INTERNAL2V5`                          | `INTERNAL2V500`                         |
| `INTERNAL4V3` (alias of INTERNAL4V34)  | `INTERNAL4V1` (alias of INTERNAL4V096)  |
| `INTERNAL4V34`                         | `EXTERNAL`                              |
| `EXTERNAL` (16k and 32k 1-series only) |                                         |
Note: We do not provide a reference named "INTERNAL" like some classic AVR cores do; because the available voltages vary, this would be a detriment to cross-compatibility (by generating code that would compile, but behave differently, rather than generate an easily corrected error), instead of a benefit.


#### Internal Sources
In addition to reading from pins, you can read from a number of internal sources - this is done just like reading a pin, except the constant listed in the table below is used instead of the pin name or pin number:
| tinyAVR 0/1-series                     | tinyAVR 2-series                    |
|----------------------------------------|-------------------------------------|
| `ADC_INTREF`                           | `ADC_VDDDIV10`                      |
| `ADC_TEMPERATURE` (DAC1 on ADC1)       | `ADC_TEMPERATURE`                   |
| `ADC_DAC0` (1-series only)             | `ADC_GROUND` (offset calibration?)  |
| `ADC_GROUND` (offset calibration?)     | `ADC_DACREF0*`                      |
| `ADC_DACREF0` (alias of ADC_DAC0)      | `ADC_DAC0` (alias of `ADC_DACREF0`) |
| `ADC_PTC` (DAC2 on ADC1)               | |

DACREF0 is the the reference voltage for the analog comparator, AC0. On the 1-series, this is the same as the `DAC0` voltage (yup, the analog voltage is shared). Analog comparators AC1 and AC2 on 1-series parts with at least 16k of flash also have a DAC1 and DAC2 to generate a reference voltage for them (though it does not have an output buffer), . On the 2-series, this is only used as the AC0 reference voltage; it cannot be output to a pin. . Unlike the AVR DA-series parts, which have neither `VDDDIV10` nor `INTREF` as an option so reading the reference voltage through `DACREF0` was the only way to determine the operating voltage, there is not a clear use case for the ability to measure this voltage.

#### Analog Resolution
The hardware supports increasing the resolution of analogRead() to the limit of the hardware's native resolution (10 or 12 bits); Additionally, we provide automatic oversampling and decimation up to the limit of what can be gathered in one batch using the accumulation feature; this is exposed through the analogReadEnh() function detailed below.


#### A second ADC? *1-series only*
On some parts, yes! The "good" 1-series parts (1614, 1616, 1617, 3216, and 3217) have a second ADC, ADC1. On the 20 and 24-pin parts, these could be used to provide analogRead() on additional pins (it can also read from DAC2). Currently, there are no plans to implement this in the core due to the large number of currently available pins. Instead, it is recommended that users who wish to "take over" an ADC to use it's more advanced functionality choose ADC1 for this purpose. See [ADC free-run and more](megaavr/extras/ADCFreerunAndMore.md) for some examples of what one might do to take advantage of it.  As of 2.0.5, megaTinyCore provides a function `init_ADC1()` which initializes ADC1 in the same way that ADC0 is (with correct prescaler for clock speed and VDD reference).

#### ADC Sampling Speed
megaTinyCore takes advantage of the improvements in the ADC on the newer AVR parts to improve the speed of analogRead() by more than a factor of three over classic AVRs! The ADC clock which was - on the classic AVRs - constrained to the range 50-200kHz - can be cranked up as high as 1.5MHz at full resolution! On the 0/1-series, we use 1MHz at 16/8/4 MHz system clock, and 1.25 MHz at 20/10/5 MHz system clock. To compensate for the faster ADC clock, we set the sample duration to 14 to extend the sampling period from the default of 2 ADC clock cycles to 16 - providing approximately the same sampling period as on classic AVRs (which is probably a conservative decision). On the 2-series, the corresponding setting has only 0.5 or 1 ADC clock added to it, so we set it to 15 - however, it can be set all the way up to 255. The ADC clock is run at approx. 2.5 MHz on the 2-series parts.  If you are measuring a lower impedance signal and need even faster `analogRead()` performance - or if you are measuring a high-impedance signal and need a longer sampling time, you can adjust that setting with `analogSampleDuration()` detailed below.

#### Analog Functions
This core includes the following analogRead-related functions

##### analogRead(pin)
The standard analogRead(). Single-ended, and resolution set by analogReadResolution(), default 10. Negative return values indicate an error condition (though when the compiler can tell that a given call will *always* return an error, we issue a compile error.

##### analogReadResolution(resolution)
Sets resolution for the analogRead() function. Unlike stock version, this returns true/false. If it returns false, the value passed was invalid, and resolution was set to the default, 10 bits. The only valid values are those that are supported natively by the hardware, plus 10 bit even if not natively supported, for compatibility.
Hence, the only valid values are 8, 10 and - on the 2-series - 12.
This is different from the Zero/Due/etc implementations, which allow you to pass anything from 1-32, and rightshift the minimum resolution or leftshift the maximum resolution as needed to get the requested range of values. Within the limited resourced of an 8-bit AVR with potentially only 4k of flash, this is a silly waste of resources; furthermore, padding a reading with zeros so it matches the format of a more precise measuring method strikes me as generally poor practice. Note also that we offer oversampling & decimation with `analogReadEnh()` and `analogReadDiff()`, which can extend the resolution keep the extra bits meaningful. `analogReadResolution()` only controls the resolution of analogRead() itself. The other functions take desired resolution as an argument.

##### analogSampleDuration(duration)
Sets sampling duration (SAMPLEN on 0/1 or SAMPDUR on 2-series) For high impedance voltage sources, use a longer sample length. For faster reads, use a shorter sample length. Passing 0 will approximately double analogRead()
speed. This returns a `bool` - it is `true` if value is valid; on 0/1-series maximum is 31 and the duration of a sample is SAMPLEN + 2 (max. 33 ADC clock sampling duration); on 2-series any 8-bit value is valid, and duration is this many plus 0.5 (without PGA) or 1 (with PGA) ADC clocks. This value is used for all analog measurement functions.

The megaTinyCore default is 14 for 0/1-series and 15 for the 2-series - for 16 or 15.5 ADC clocks (which is half as long on the 2-series, since it supports higher ADC clock speeds. ADC clock is configured 1-1.25 MHz on 0/1, and approx. 2.5 MHz on 2-series.

Reducing it to the minimum will approximately cut the time analogRead() takes in half, but will have poor accuracy unless measuring very low impedance signals;

##### void ADCPowerOptions(options) *2-series only*
The PGA is enabled by any call to `analogReadEnh()` or `analogReadDiff()` that specifies valid gain > 0, by default we turn it off afterwards. There is also a "low latency" mode that, when enabled, keeps the ADC reference and related hardware running to prevent the delay (on order of tens of microseconds) before the next analog reading is taken). We use that by default, but it can be turned off with this function. The argument is a number between 0x00 and 0x0F. The structure is 0b 0000 PPLL - P bits control the PGA power state, L bits the LOWLAT setting.
* `LL = 00 or 01` Take no action on low latency mode.
* `LL = 10` Turn off low latency mode.
* `LL = 11` Turn on low latency mode.
* `PP = 00` Don't change anything relating to the PGA.
* `PP = 01` Turn off the PGA now. Don't change settings; if not set to turn off automatically, that doesn't change.
* `PP = 10` Enable PGA. Disable the automatic shutoff of the PGA.
* `PP = 11` Disable PGA now, and in future turn if off after use

Example:
```c
ADCPowerOptions(0x0B); //0x0F = 0b 0000 1011 LL = 11, so low latency on. PP = 10, so PGA on, automatic shut off disabled. Maximum power consumption, minimum ADC delays.
ADCPowerOptions(0x0E); //0x02 = 0b 0000 1110 LL = 10, so low latency off. PP = 11, turn off the PGA and enable automatic shut off. Minimum power consumption, maximum ADC delays.
```

##### analogReadEnh(pin, res=ADC_NATIVE_RESOLUTION, gain=0)
Enhanced `analogRead()` - returns a `long` (`int32_t`), not an `int` (`int16_t`). Perform a single-ended read on the specified pin. `res` is resolution in bits, which may range from 8 to `ADC_MAX_OVERSAMPLED_RESOLUTION`. This maximum is 13 bits for 0/1-series parts, and 17 bits for 2-series parts. If this is less than the native ADC resolution, that resolution is used, and then it is right-shifted 1, 2, or 3 times; if it is more than the native resolution, the accumulation option which will take 4<sup>n</sup> samples (where `n` is `res`-native resolution). Note that even with the faster ADC in the 2-series, and even without extending the sampling duration, a 1024 sample burst read is not instantaneous - with defaults it's around 12 ms for 1024-sample burst read, and at maximum sample duration it will take over 100ms. The accumulated result is then decimated (rightshifted n places) to yield a result with the requested resolution, which is returned. See [Atmel app note AVR121](https://ww1.microchip.com/downloads/en/appnotes/doc8003.pdf) - the specific case of the 2-series is discussed in a more recent app note from Microchip, but it is not a particularly informative document.  Alternately, to get the raw accumulated ADC readings, pass one of the `ADC_ACC_n` constants for the second argument where `n` is a power of 2 up to 64 (0/1-series), 128 (Dx-series), or 1024 (2-series). Be aware, however, that the lowest bits of a raw accumulated reading should not be trusted (which is why the decimation step is needed, and why 4x the samples are required for every extra bit of resolution instead of 2x). On 2-series parts *the PGA can be used for single ended measurements*. Valid options for gain are 0 (PGA disabled, default), 1 (unity gain - may help in some circumstances - but that is the extent of my knowledge on that matter, and powers of 2 up to 16) - on 0/1-series parts, the gain argument should be omitted or 0.
```c++
  int32_t second_pin_voltage = analogReadEnh(PIN_PA2,13);
  // measure voltage on PA2, to 13 bits of resolution, which will require either 3 or 1 bits of oversampling + decimation
  // (depending on whether it is a 0/1 or 2-series part.)
```


##### analogReadDiff(positive, negative, res=ADC_NATIVE_RESOLUTION, gain=0) *2-series only*
Differential `analogRead()` - returns a `long` (`int32_t`), not an `int` (`int16_t`). Performs a differential read using the specified pins as the positive and negative inputs. Any analog input pin can be used for the positive side, but only pins on PORTA (tinyAVR 2-series) or PORTD/PORTE (AVR Dx-series) can be used as the negative input. The result returned is the voltage on the positive side, minus the voltage on the negative side, measured against the selected analog reference. The `res` parameter works the same way as for `analogReadEnh()`, as does the `gain` function. Gain becomes FAR more useful here than in single-ended mode as you can now take a very small difference and "magnify" it to make it easier to measure.

```c++
  int32_t diff_first_second = analogReadDiff(PIN_PA1, PIN_PA2, 12, 0);
  // Simple differential read to with PA1 as positive and PA2 as negative. Presumable these are close in voltage.
  // (I used two pots and manually adjusted them to be very close; also could have done like 10k-100ohm-10k resistor network)
```

#### Offset error *2-series only*
Particularly with small reference voltages, measuring very small voltage differences, the offset error was higher than I expected (though not as bad as I feared when I wrote this initially - there was a bug analogReadEnh() that set the ADC prescaler to the minimum after every measurement). To see the magnitude of this offset error in your differential readings, take one differential measurement, then switch positive and negative and take the measurement again, and sum the two results. If there were no offset error, and assuming the voltages did not change, that would be zero. It will be important to see if methods can be used to correct for the offset. Also, remember that **oversampling does not do anything to help with offset error** - adding a bunch of measurements, all of which are high or low by the same amount will yield a sum that is proportionately just as high or low, and can increase the apparent size of the offset error  Increasing the gain, however, does help,
```c++
  analogReference(INTERNAL1V024); //minimum reference
  // Make PA1 and PA2 really close in voltage - we want to see this new ADC shine right?
  int32_t diff_first_second = analogReadDiff(PIN_PA1, PIN_PA2, 12, 0);
  int32_t diff_second_first = analogReadDiff(PIN_PA2, PIN_PA1, 12, 0);
  Serial.println(diff_first_second + diff_second_first);
  Serial.println(diff_first_second);
  Serial.println(diff_second_first);
```


#### Error Codes
The analogRead function and the enhanced version of it can return a number of different "error" codes to indicate that it was called under inappropriate conditions. These are always values very close to the minimum value of the signed integer that it returns. Although on megaTinyCore, a successful analogRead will always return a positive value (so that -1, -2, etc would work), it is done is done in this way so that the same error codes are returned on all parts, even classic AVRs (ex, ATtiny861 on ATTinyCore 2.0.0 and later) that do differential ADC reads through analogRead(). All of these error codes (for both analogRead and the enhanced versions) are #defined with the names shown below as well so you can test for them programmatically. Note that if the compiler knows at compile time that one of these errors will be generated by a given call, that will be treated as a compile error. Only error conditions that come about at runtime will trigger the error codes

##### `ADC_ERROR_BAD_PIN_OR_CHANNEL` (-32765)
##### `ADC_ENH_ERROR_BAD_PIN_OR_CHANNEL` (-2100000000)
The selected pin has no analog input capability, does not exist, or, if it's a channel number (from ADC_CH(channel number), it does not exist or is otherwise invalid.

##### `ADC_ERROR_BUSY` (-32766)
##### `ADC_ENH_ERROR_BUSY` (-2100000001)
The requested reading could not be made because the ADC is currently busy taking another measurement, for example in free running mode.

##### `ADC_ERROR_DISABLED` (-32767)
##### `ADC_ENH_ERROR_DISABLED` (-2100000007)
The requested reading could not be made because the ADC is currently disabled. Did you disable it when you put the device to sleep and never turn it back on?

##### `ADC_ENH_ERROR_RES_TOO_LOW` (-2100000003)
Resolution must not be lower than the lowest native resolution that the part supports (Core presents this as ADC_NATIVE_RESOLUTION_LOW). If you require lower resolution, that is what the `>>` operator is for.

##### `ADC_ENH_ERROR_RES_TOO_HIGH` (-2100000004)
The 0/1-series can get 3 extra bits by oversampling 64x and right- shifting 3 times. The 2-series can get 5 by oversampling by a whopping 1024x (taking 13-160 ms depending on options)
and then right-shifting 5 times. Thus, maximums are 13 and 17. If you want the raw result before decimation (being aware that the lowest bits are mostly noise), one of the `ADC_ACC_n` constants may be used, where `n` is 2, 4, 8, 16, 32, 64, or on 2-series only, 128, 256, 512, or 1024 - in this case, that many samples will be taken through burst/accumulation mode and returned without decimation.

##### `ADC_ENH_ERROR_BAD_NEG_PIN` (-2100000005)
Returned by analogReadDiff when the requested pin for the negative input is not valid as a negative input. Negative pins can only be on PORTA (for tinyAVR) or PORTD/PORTE (for Dx-series), so there are significantly fewer channels to choose from.

### DAC Support
The 1-series parts have an 8-bit DAC which can generate a real analog voltage. This generates voltages between 0 and the selected VREF (which cannot be VDD, unfortunately). Set the DAC reference voltage via the `DACReference()` function - pass it one of the `INTERNAL` reference options listed under the ADC section above. In versions prior to 2.0.0, select the DAC VREF voltage from the Tools -> DAC Voltage Reference submenu. This voltage must be half a volt lower than Vcc for the voltage reference to be accurate. The DAC is exposed via the analogWrite() function: Call `analogWrite(PIN_PA6,value)` to set the voltage to be output by the DAC. To turn off the DAC output, call digitalWrite() on that pin; note that unlike most* PWM pins `analogWrite(PIN_PA6,0)` and `analogWrite(PIN_PA6,255)` do not act as if you called digitalWrite() on the pin; 0 or 255 written to the `DAC0.DATA` register; thus you do not have to worry about it applying the full supply voltage to the pin (which you may have connected to sensitive devices that would be harmed by such a voltage) if let the calculation return 255; that will just output 255/256ths of the reference voltage.

### printf() support for "printable" class
Unlike the official board packages, but like many third party board packages, megaTinyCore includes the .printf() method for the printable class (used for Serial and many other libraries that have print() methods); this works like printf(), except that it outputs to the device in question; for example:
```cpp
Serial.printf("Milliseconds since start: %ld\n", millis());
```
Note that using this method will pull in just as much bloat as sprintf(), so it may be unsuitable on devices with small flash memory.

### Pin Interrupts
All pins can be used with attachInterrupt() and detachInterrupt(), on RISING, FALLING, CHANGE, or LOW. All pins can wake the chip from sleep on CHANGE or LOW. Pins marked as ASync Interrupt pins on the megaTinyCore pinout charts (pins 2 and 6 within each port) can be used to wake from sleep on RISING and FALLING edge as well. Those pins are termed "fully asynchronous pins" in the datasheet

Advanced users can instead set up interrupts manually, ignoring attachInterrupt and manipulating the relevant port registers appropriately and defining the ISR with the ISR() macro - this will produce smaller code (using less flash and RAM) and the ISRs will run faster as they don't have to check whether an interrupt is enabled for every pin on the port. For full information and example, see: [Pin Interrupts](https://github.com/SpenceKonde/megaTinyCore/blob/master/megaavr/extras/PinInterrupts.md)

### Improved Export Compiled Binary
The Sketch -> Export Compiled Binary option has been significantly enhanced in megaTinyCore and it now provides a record of settings when the export was performed, and assembly listings and memory map to assist in optimization and size reduction effort.

#### Naming includes all Tools submenu options that effect compiled output
The name of the exported binary includes every tools submenu selection that could impact the compiled binary, for example `Test.ino.t1614opti.20.u5V.mD0.hex` - the full part number (not just family), whether it's using optiboot (this effects start address of .hex, unlike classic AVRs where a bootloaded board takes the same binary as non-bootloaded one), the clock speed (in MHz), the approximate voltage for UART baud rate calculation if used, and the millis timer source (`A0`, `B0`, `B1`, `D0`, `RTC`, `RTC_XTAL` or `NONE`). Entries will be added to this as new submenus are added to the core.

#### Assembly Listing generation
Like ATTinyCore and DxCore, Sketch -> Export compiled binary also generates an assembly listing in the sketch folder (same naming scheme as above); this file, ending in .lst, can be opened in any text editor. It contains an assembly listing, with the C source and generated assembly intermixed, which (attempts to) show which C generated which instructions. One does not need to know assembly to understand that a whole load of cryptic instructions means more flash usage than a small number of cryptic instructions; this very simple rule helps with the most common use case for the listing: figuring out what exactly it is in the code that is responsible for it eating up all the flash. See: [Assembly Listing Basics](https://github.com/SpenceKonde/AVR-Guidance/blob/master/LowLevel/AssemblyListingBasics.md) for a very brief orientation.

#### Memory Map generation (New in 2.3.0)
The memory map is a less powerful resource than an assembly listing - but it is far easier to understand, and it is very much focused on the sizes of sections in memory (both RAM and Flash), which is a very common reason to be looking at this low-level info. It's a text file with a .map extension, containing a table of values, with columns delimited by the | character, and all numbers in hexadecimal (which is the only right way for memory addresses). Each symbol name is listed, along with it's address, and - if it occupies space in flash or ram, that size, as well as where they are defined. It may be best viewed in a spreadsheet, but I haven't been able to make it import into excel cleanly; a future update may add some pre-processing to help with this.

### BOD configuration options
These parts officially support BOD trigger levels of 1.8V, 2.6V, and 4.2V, with Disabled, Active, and Sampled operation options for when the chip is in Active and Sleep modes - Disabled uses no extra power, Active uses the most, and Sampled is in the middle. As of 2.1.0, the active/sleep modes have been combined into a single menu, the nonsensical options (such as using more aggressive BOD is while sleeping than while awake) removed, and the previously unexposed options added: sampled mode is now available with two sample rates (the faster one uses ever so slightly more power, as you would expect), and "Enabled hold wake" - in that mode, BOD is disabled in sleep, enabled when not sleeping, and when waking up, code execution does not begin until the BOD is ready. See the datasheet for details on power consumption and the meaning of these options. You must do Burn Bootloader to apply this setting, even in 2.2.0 and later; it's not a "safe" setting, since if you set it to a voltage higher that the operating voltage and enable it, the chip is "bricked" until a high enough voltage is applied.

#### Additional BOD levels
Between the initial header file and preliminary datasheet release, and the more recent versions of each, several BOD settings were removed from the tinyAVR 0/1-series datasheets, and the atpack release notes described them as "unqualified" - (described as "unqualified" in the release notes accpompanying this change - I understand that this has something to do with the vetting process for critical applications which these parts go through and do not correspond to a voltage above which the chip is guaranteed to function under certain conditions (The three official BOD levels are the voltages that the chip is guaranteed (Yup, they use that word in the datasheet!) to work within the manufacturer specified temperature range running at a system clock frequency no higher than marked, and it has been implied that ongoing tests of production devices are performed it would appear that their policy is to test against  that this is tested in production. In contrast, the other 5 BOD levels are expected to work as one would expect, but Microchip does not provide any *guarantee* that they'll work, even if all other design requirements are met. were removed from the datasheet and header files. These are supported by the megaTinyCore BOD  dropdown menu, but (as of 2.0.4 - the first version that has the new headers) are marked as such in the submenu. Note that the new headers no longer provide the `*_gc` enum entries for these BOD levels..

### Auto-set safe fuses on UPDI upload
Whenever a UPDI programmer is used to upload code, all fuses that can be set "safely" (as in, without risk of bricking the board, or bricking the board if one does not have access to an HV programmer), and which have any built-in configuration options, will be set. Thus, except where noted, behavior will always match the selected tools menu! This means:
```c
WDTCFG will not be changed - not configured by megaTinyCore
BODCFG will not be changed - not safe
OSCCFG will be set
TCD0CFG will not be changed - not configured by megaTinyCore
SYSCFG0 will not be changed - not safe
SYSCFG1 will be set
APPEND will  not be changed - not configured by megaTinyCore. There is insufficient demand to justify the development effort.to make use of this as DxCore does
BOOTEND will be set
LOCKBIT will not be changed - not configured by megaTinyCore
```
`BODCFG` is not safe, because setting this to a higher voltage than board is running at and enabling it will "brick" the board until a higher operating voltage can be supplied. `SYSCFG0` is not safe because this is where `RSTPINCFG` lives; changing this can leave the board unprogrammable except via HV UPDI programming, and not everyone has an HV UPDI programmer. In the future if/when a programmer that guarantees HV UPDI capability which can be selected as a programmer (ie, it becomes possible to make a tools -> programmer option which will only work with HV programmers) this fuse will be set automatically when using that programmer.

As a result **in 2.2.0 and later, you no longer need to 'burn bootloader' to switch between 16-derived and 20-derived speeds when uploading using UPDI**
In the future, we hope to provide a way for an application to run at 16 MHz when fuses tell it to run at 20 and vice versa. The compliance of the internal oscillator is easily sufficient for this - they normally go from around 10 MHz to 26 MHz with the 16 MHz mode selected, and 12 MHz to 30 or 32 MHz with the 20 MHz mode selected, with a few MHz variation between chips, so all one should need is a simple calibration routine that determines the OSC20MCALIBA value closest to these and writes it to the USERROW.

### Link-time Optimization (LTO) support
This core *always* uses Link Time Optimization to reduce flash usage - all versions of the compiler which support the tinyAVR 0/1/2-series parts also support LTO, so there is no need to make it optional as was done with ATTinyCore.

## Included Libraries

### tinyNeoPixel (WS2812)
The usual NeoPixel (WS2812) libraries, including the popular FastLED as well as AdafruitNeoPixel, have problems on these parts - they depend on hand-tuned assembly, but the execution time of several key instructions has been improved; the improvements enable significant simplification of the code for driving these LEDs. This core includes a compatible version of the tinyNeoPixel library for interfacing with these ubiquitous addressable LEDs. There are two versions,  both tightly based on the Adafruit_NeoPixel library - one implements a truly identical API, and differs only in name.  The other realizes significant flash savings in exchange for minor restrictions (length must be constant and known at compile time) and a slightly different syntax for the constructor. See the [tinyNeoPixel documentation](https://github.com/SpenceKonde/megaTinyCore/blob/master/megaavr/extras/tinyNeoPixel.md) and included examples for more information.

### EEPROM and USERROW
The standard EEPROM.h is available here - it works like it does on any AVR. `USERSIG.h` (from "User Signature" which the datasheet has sometimes called the `USERROW`) it has the same API as EEPROM, though there may be future additions to harmonize with Dx-friendly functions for updating multiple bytes - the Dx-series parts can only erase the whole USERROW, so potentially each byte written could involve erasing and rewriting it all - the question of how to deal with that is why DxCore doesn't have a USERSIG library yet). The name "USERSIG" refers to the alternate name of the USERROW, the "User Signature" space - the name USERROW could not be used because it is defined by the io headers (it's the `struct` of type `USERROW_t`, made up of `USERROW.USERROW0` through `USERROW.USERROW31`. Not the most useful thing, but we never override the io header file definitions unless working around a bug.

Note: Prior to 2.1.0, we tried to get clever with supporting the `USERROW` through the EEPROM library; that not only was shortsighted (as it's logically inconsistent on anything with more than 256b of EEPROM), it also introduced some serious bugs. Use the `USERSIG.h` library for that instead.

### Servo
The usual Servo library from library manager is incompatible with these parts (minor changes could make it "work", but with glaring issues and a dependence on the configuration of TCA0). This core provides a version of the Servo library which will select an appropriate timer (TCB0 is the only option on most parts, on parts with a TCB1 (2-series and 3216, 3217, 1617, 1616 and 1614), TCB1 will be used instead, provided it's not being used for millis()). Except on parts with a TCB1, tone cannot be used at the same time as the Servo library. Servo output is better at higher clock speed - when using servos, it is recommended to run at the highest frequency permitted by the operating voltage to minimize jitter.

**Warning** If you have installed a version of the Servo library to your <sketchbook>/libraries folder (including via library manager), the IDE will use that version of the library (which is not compatible with these parts) instead of the one supplied with megaTinyCore (which is). As a workaround, a duplicate of the Servo library is included with a different name - to use it, just `#include <Servo_megaTinyCore.h>` instead of `#include <Servo.h>` - no other code changes are necessary.

Note that the Servo libraries were only fixed in version 2.2.0 - prior to that we had a Servo library, but it didn't work due to an astonishingly large number of bugs (I swear I tested it!)

### Flash
Written by @MCUDude, this provides a more accessible (much more accessible!) wrapper around the optiboot.h library (which was written by the famous @westfw) . This supports writing to the flash of any device using Optiboot, by having the application code call routines in the bootloader to write to the flash (the tinyAVR 0/1/2-series parts have built-in flash protection mechanisms that permit only code executing from the bootloader section (`BOOTCODE` in their terminology) to write to the application section (`APPCODE`)). While the hardware does support a third flash section (`APPDATA`) which can be written by code running in `APPCODE` this is only usable if there is also a `BOOTCODE` section defined (otherwise the entire flash is treated as `BOOTCODE` which can never be self-programmed), and would require a separate implementation of this library to use. It would also be possible to get flash-write-from-app without use of an actual bootloader through an analog of the trick used by the DxCore Flash.h for this. Since there appears to be little demand for such a thing, that functionality is not currently implemented (they were implemented on DxCore's flash writing library because the additional effort was virtually nil, and because there was a user with a particuluar interest in that feature).

[Optiboot_flasher documentation](https://github.com/SpenceKonde/megaTinyCore/blob/master/megaavr/libraries/Optiboot_flasher/README.md)

**Warning** As noted above, there is a library for DxCore that is also named `Flash.h`. Both allow an application to write to the flash using Optiboot if present. *That is the only similarity they have*. The API, NVM hardware, method used to call the bootloader, and basically *everything about these libraries is different*. Be sure you write code for the one that matches the hardware you're using. While I (Spence Konde) wrote the DxCore one, I don't have a particularly strong opinion about which way is "right" - we made them independently, not because we each thought the other one's idea of how it should be done was wrong, and they reflect the way the hardware interacts with it's flash; the one for megaTinyCore is page-oriented, with page buffers, while the DxCore one only cares about pages when you erase just like the hardware!

### Logic
All of these parts have at least a pair of Configurable Custom Logic (CCL) blocks. Each block allows you to supply an arbitrary 3-input truth table, as well as configuring other aspects of the logic block. CCL can operate *asynchronously* - meaning that things can happen faster than the clock speed, or it can be synchronized to an event input or the system clock. The inputs can come from pins, events, or other peripherals; there's a feedback input (so you can make latches and oscillators) and a "sequencer" that can act like a latch or flip-flop using the outputs of the logic blocks as it's inputs. This is an incredibly powerful peripheral - especially on the 2-series parts, which have a second pair of logic blocks, as well as the capability to trigger an interrupt when the state of one changes.

This library provides a simple wrapper around the CCL hardware in the tinyAVR 0/1/2-series devices. This library is also included in DxCore and MegaCoreX, covering all AVRs with CCL hardware. Written by @MCUDude with contribution from others.

[Logic library documentation](https://github.com/SpenceKonde/megaTinyCore/blob/master/megaavr/libraries/Logic/README.md)

### Comparator (2.3.0+)
These parts have either 1 or 3 (1614, 1616, 1617, 3216, and 3217) on-chip analog comparators which can be used to compare two analog voltages, and, depending on which is larger, generate an event output, drive a pin high or low, or fire an interrupt. One of the voltages can be the internal reference (0-series) an internal reference scaled by an 8-bit DAC (everything else). This library, written by @MCUDude, provides a simple wrapper around the analog comparator(s) which makes their configuration easier and resulting code more readable (also easier on the wrists - less stuff to type in all caps) than manually configuring registers, while exposing nearly the full featureset of the analog comparators on these parts.  Do note does not support the Window Comparator option for the parts with 3 comparators; There doesn't exactly seem to be a lot of demand for that one, though.

This library is also included in DxCore and MegaCoreX, covering all modern AVRs with comparator hardware. Written by @MCUDude.

[Logic library documentation](https://github.com/SpenceKonde/megaTinyCore/blob/master/megaavr/libraries/Comparator/README.md)

## Supported Libraries (not included)
In general you should expect the following about library compatibility:
* Anything that works on an Uno WiFi Rev. 2 or Nano Every should work or require minimal effort to convert (if you run into one that doesn't work, please let us know in either discussions or issues, so we can look into getting it working correctly. I always want to know to add to the table linked below, but particularly if it works on those boards but not here - they are very similar architectures, and any porting effort required should be minimal. The most likely explanation is that the library tested *specifically* for the ATmega4809, rather than generally for the peripherals it used.
* The library.properties field `architectures=*` would suggest that it would work anywhere - all this means is that there are not separate folders with implementations for different architectures. It does not mean that the library does not make assumptions about architecture, test against architecture specific stuff with #ifdefs and so on.
* Libraries that work on other AVR-based arduino devices will work as long as:
  * They do not directly interact with hardware registers,
  * They do not make assumptions about what pins are associated with their peripheral (ex "Okay, now my library has included Wire.h, now I'm going to attachInterrupt() on SDA: attachInterrupt(A4,myInterrupt);) - that fails except on an '328, because it assumes that SDA is on the same pin as analog input 4 - for example);

The amount of effort required to port a given library will vary widely depending on the library. Some are almost trivial, while others require deep knowledge. The fact that many libraries can be ported without much effort again underlines the need for reports from users about incompatible libraries so we can pick this lowhanging fruit,

### [List of known working/not-working libraries](https://github.com/SpenceKonde/megaTinyCore/blob/master/megaavr/extras/LibraryCompatibility.md)
I know lots of people use libraries that aren't on that list, and I urge you all to tell me what they are - whether they do or do not work, I want to hear about it and keep enlarging the table!

## How to refer to pins
The simple matter of how to refer to a pin for analogRead() and digitalRead(), particularly on non-standard hardware, has been a persistent source of confusion among Arduino users. It's my opinion that much of the blame rests with the decisions made by the Arduino team (and author of Wiring before them) regarding how pins were to be referred to; the designation of some pins as "analog pins" leads people to think that those pins cannot be used for digital operations (they are better thought of as "pins with analog input" - like how there are "pins that can output PWM"). The fact that pins have traditionally been renumbered has further muddied the water. For non-standard classic AVR parts, matters are often made even worse by multiple, incompatible "pin mappings" created by various authors over the years to make the part act "more like an Uno" or for some other purpose (ATTinyCore is a particular mess in this way, with some parts having three entirely different pin mappings, in at least one case, one of the alternate mappings is a devil-inspired work of pure evil, requiring nothing short of an additional lookup table to convert analog pins to digital pins).

This core uses a simple scheme for assigning the Arduino pin numbers: Pins are numbered starting from the the I/O pin closest to Vcc as pin 0 and proceeding counterclockwise, skipping the (mostly) non-usable UPDI pin. The UPDI pin is then assigned to the last pin number (as noted above, it is possible to read the UPDI pin (both analog and digital reads work) even if it is not set as GPIO). We recommend this as a last resort: the UPDI pin always has it's pullup enabled when not set as a GPIO pin, and a signal which looks too much like the UPDI enable sequence will cause undesired operation.

In order to prevent all confusion about pin identities and eliminate ambiguity, we recommend using the PIN_Pxn notation to refer to [pins unless you are using a development board with different numbers or names for the pins printed on it. This will maximize portability of your code to other similar hardware and make it easier to look up information on the pins you are using in the relevant datasheets should that be necessary.

### PIN_Pxn Port Pin Numbers (recommended)
**This is the recommended way to refer to pins** Defines are also provided of form PIN_Pxn, where x is A, B, or C, and n is a number 0-7 - (Not to be confused with the PIN_An defines described below). These just resolve to the digital pin number of the pin in question - they don't go through a different code path or anything. However, they have particular utility in writing code that works across the product line with peripherals that are linked to certain pins (by Port), as most peripherals are. Several pieces of demo code in the documentation take advantage of this.  Direct port manipulation is possible as well - and in fact several powerful additional options are available for it - see [direct port manipulation](https://github.com/SpenceKonde/megaTinyCore/blob/master/megaavr/extras/DirectPortManipulation.md).

**`PIN_Pxn` - not `Pxn`, and not `PIN_xn` - those mean different things!**

### Arduino Pin Numbers
When a single number is used to refer to a pin - in the documentation, or in your code - it is always the "Arduino pin number". These are the pin numbers shown in orange (for pins capable of analogRead()) and blue (for pins that are not) on the pinout charts. All of the other ways of referring to pins are #defined to the corresponding Arduino pin number.

### An and PIN_An constants
The core also provides An and PIN_An constants (where n is a number from 0 to 11). As with the official core, PIN_An is defined as the digital pin number of the pin shared with analog channel n These refer to the ADC0 *channel* numbers. This naming system is similar to what was used on many classic AVR cores **but here, they are just #defined as the corresponding Arduino pin number**. If you need to get the analog channel number on a digital pin, use the `digitalPinToAnalogInput(pin)` macro. The An numbers are not shown on the pinout charts - just use the digital pin numbers. The mapping of analog channels to pins is shown in the the datasheet under the I/O Multiplexing Considerations chapter, and reproduced in the [advanced ADC features page](https://github.com/SpenceKonde/megaTinyCore/blob/master/megaavr/extras/ADCFreerunAndMore.md). Note that channel A0 is on the UPDI/Reset pin - however, even when configured as UPDI, it can be used as a digital input, and as an analog input on the 0 and 1-series. as long as the signals it can be exposed to do not look like the UPDI enable sequence.

## Triggering a reset from software

As noted above in the discussion of the shared UPDI/reset pin, these parts support a native software reset operation; on classic AVRs, the only way was to enable the watchdog timer, and then wait for it to time out. With the "modern" AVR devices, there are two ways to trigger a reset from software: A watchdog timer reset (as before), and the native software reset. Unlike classic AVRs, after a WDT reset, the watchdog timer is not forced on with the minimum timeout (on classic devices, this is why doing a WDT restart wirh very old bootloaders instead hung the board - the bootloader wasn't smart enough to turn it off before it was reset by it.). These two methods of resetting the chip allow you to signal to the application or bootloader what sort of condition triggered the reset.

Additionally, while the bootloader, if used (see below) will run after a software reset, it will NOT run after a watchdog reset (well - it will run, but only long enough to read the reset flag register and see that it was restarted by the WDT: That means that either the bootloader just ran, finished, and reset the device (If we didn't jump to the app in this case, we'd just sit in the bootloader doing WDT resets forever), that the application suffered from a WDT timeout due to a bug or adverse conditions (that's not the bootloader's business to get involved in) or that the application intentionally triggered a WDT reset. None of those are "entry conditions" for Optiboot - so Optiboot jumps to the app immediately).This allows the application a convenient mechanism to restart itself without having to potentially wait through the bootloader attempting to communicate with whatever is connected to the serial port.


Note: While the windowed mode would at first seem to suggest that you could reset much faster by setting it and then executing `WDR` until it resets from missing the window, you don't gain nearly as much as you'd think. First, the initial WDR needs to be syncronized - 2-3 WDT clocks, ie, 2-3 ms. Additional WDRs executed while this is happening are ignored. Only when the second WDR makes it to the watchdog timer domain will it reset the system. So the overall time to restart is 6-9ms. Instead 10-11 ms (sync delay + minimum timeout).
```c++
void resetViaWDT() {
  _PROTECTED_WRITE(WDT.CTRLA,WDT_PERIOD_8CLK_gc); //enable the WDT, minimum timeout
  while (1); // spin until reset
}

```c++
void resetViaWDTFaster() {
  _PROTECTED_WRITE(WDT.CTRLA,WDT_WINDOW_8CLK_gc | WDT_PERIOD_8CLK_gc); //enable the WDT, minimum timeout, minimum window.
  while (1) __asm__ __volatile__ ("wdr"::); // execute WDR's until reset. The loop should in total take 3 clocks (the compiler will implement it as wdr, rjmp .-4), but because of the sync to the relatively slow
}

void resetViaSWR() {
  _PROTECTED_WRITE(RSTCTRL.SWRR,1);
}
```

### Using watchdog to reset when hung
If you only worked with the watchdog timer as an Arduino user - you might not even know why it's called that, or what the original concept was, and just know it as that trick to do a software reset on classic AVRs, and as a way to generate periodic interrupts. The "purpose" of a watchdog timer is to detect when the part has become hung - either because it's wound up in an infinite loop due to a bug, or because it wound up in a bad state due to a glitch on the power supply or other adverse hardware event, has been left without a clock by an external clock source failing, went to sleep waiting for some event which doesn't end up happening (or without correctly enabling whatever is supposed to wake it up) - and issue a reset. It is often anthropomorphized as a dog, who needs to be "fed" or "pet" periodically, or else he will "bite" (commonly seen in comments - the latter generally only when intentionally triggering it, as in `while (1); //wait for the watchdog to bite`).

A typical use of this is to have the main loop (generally loop() in an Arduino sketch) reset the watchdog at the start or end of each loop, so when a function it calls ends up hung, we can



```c
// As a function
void wdt_reset() {
  __asm__ __volatile__ ("wdr"::);
}
```
Or
```c
// as a macro (which is all that wdt.h does)
#define wdt_reset() __asm__ __volatile__ ("wdr"::)
```

```
### The wrong way to reset from software
I have seen people throw around `asm volatile("jmp 0");` as a solution to a need to reset from software. Don't do that - all compiled C code makes assumptions about the state from which it is run. Jumping to 0 from a running application violates several of them unless you take care to avoid those pitfalls. Both Optiboot and the init() method of megaTinyCore make assumptions about the state of peripheral registers as well (namely, that they're in their reset configuration when it starts). Now that we finally have a right way to do software reset, use it!


## Identifying Part and Options in sketch
It is often useful to identify what options are selected on the menus from within the sketch; this is particularly useful for verifying that you have selected the options you wrote the sketch for when opened later by yourself or someone who you shared it with. Or, you can use these, combined with `#if` macros to select the appropriate code depending on the part or options at hand.

### Part or Part Family
When writing code that may be compiled for a variety of target chips (which is particularly easy on the megaAVR parts, as the peripherals are so similar), it is often useful to be able to identify which family of parts is being used, in order to, for example, select which pin numbers to use for each purpose. Each part is identified by a #define:
`__AVR_ATtiny***__` where *** is the part number (ex: `__AVR_ATtiny3216__`).

This core provides two additional #defines for part "families" - note that in versions prior to 2.0.2, these are incorrect for many parts:
* `__AVR_ATtinyx**__` where ** is the last 2 digits of the part number (ex: `__AVR_ATtinyx16__`)
* `__AVR_ATtinyxy*__` where * is the last digit of the part number, (ex: `__AVR_ATtinyxy6__`).

This is just shorthand, for convenience - `#ifdef __AVR_ATtinyxy2__` is equivalent to `#if defined(__AVR_ATtiny212__) || defined(__AVR_ATtiny412__) || defined(__AVR_ATtiny202__) || defined(__AVR_ATtiny402__)`

### Clock Source
Will either be 0 (internal oscillator) or 2 (external clock. )


### Core Version
Version information for MEGATINYCORE is also provided by a few additional defines to make it easier to work with these in a sketch or compare to specific values; an example is presented below:
* MEGATINYCORE "2.0.2"
* MEGATINYCORE_MAJOR 2
* MEGATINYCORE_MINOR 0
* MEGATINYCORE_PATCH 2
* MEGATINYCORE_RELEASED 1
* MEGATINYCORE_NUM 0x02000201
While historically, these have often not been consistently updated with every release, as of 2.1.4, they are now generated directly from the platform.txt - it is hoped that this will help improve that.

### Millis Timer Selection
The option used for the millis/micros timekeeping is given by a define of the form USE_MILLIS_TIMERxx. Possible options are:
* USE_MILLIS_TIMERA0
* USE_MILLIS_TIMERB0
* USE_MILLIS_TIMERB1
* USE_MILLIS_TIMERD0
* USE_MILLIS_TIMERRTC (defined if RTC is used for millis - whether or not external crystal is used)
* USE_MILLIS_TIMERRTC_XTAL (only defined if RTC is used with external crystal)
* USE_MILLIS_TIMERNONE (defined if millis timekeeping is disabled)
* DISABLE_MILLIS (synonym for above, for backwards compatibility)
Additionally, `MILLIS_TIMER` is defined to that timer - see `Identifying Timers` below

#### Checking that correct menu option is selected
If your sketch requires that the TCB0 is used as the millis timer

```c
#ifndef MILLIS_USE_TIMERB0
#error "This sketch is written for use with TCB0 as the millis timing source"
#endif
```

#### Core feature detection
There are a number of macros for determining what (if any) features the core supports (these are shared with DxCore) Values of these are shown in parenthesis, where two values are given, the first uis for 0/1-series, the second for 2-series, as they have different capabilities.
* `CORE_HAS_FASTIO` (1) - If defined as 1 or higher, indicates that digitalWriteFast() and digitalReadFast() is available. If undefined or defined as 0, these are not available. If other "fast" versions are implemented, they would be signified by this being defined as a higher number. If defined as -1, there are no digital____Fast() functions, but with constant pins, these functions are optimized aggressively for speed and flash usage (though not all the way down to 1 instruction).
* `CORE_HAS_OPENDRAIN` (1) - If defined as 1, indicates that openDrain() and (assuming `CORE_HAS_FASTIO` is >= 1) openDrainFast() are available. If undefined or 0, these are not available.
* `CORE_HAS_PINCONFIG` (1) - If defined as Indicates that pinConfigure() is available. If not defined or defined as 0, it is not available.
* `CORE_HAS_TIMER_TAKEOVER` (1) - if defined as 1, takeOverTCxn() functions are available to let user code take full control of TCA0, TCA1 and/or TCD0.
* `CORE_HAS_TIMER_RESUME` (1)- if defined as 1, the corresponding resumeTCxn() functions, which reinitialize them and return them to their normal core-integrated functions, are available.
* `ADC_NATIVE_RESOLUTION` (10 or 12)- This is the maximum resolution, in bits, of the ADC without using oversampling.
* `ADC_NATIVE_RESOLUTION_LOW` (8 or 10) - The ADC has a resolution setting that chooses between ADC_NATIVE_RESOLUTION, and a lower resolution.
* `ADC_DIFFERENTIAL` (0 or 2) - This is defined as 1 if the part has a basic differential ADC (no gain, and V<sub>analog_in</sub> constrained to between Gnd and V<sub>Ref</sub>), and 2 if it has a full-featured one. It does not indicate whether said differential capability is exposed by the core.
* `SUPPORT_LONG_TONES` (1)  - On some modern AVR cores, an intermediate value in the tone duration calculation can overflow (which is timed by counting times the pin is flipped) leading to a maximum duration of 4.294 million millisecond. This is worst at high frequencies, and can manifest at durations as short as 65 seconds worst case. Working around this, however, costs some flash, and some cores may make the choice to not address it (megaTinyCore only supports long tones on parts with more than 8k of flash).  If `SUPPORT_LONG_TONES` is defined as 1, as long as (duration * frequency)/500 < 4.294 billion, the duration will not be truncated. If it is defined as 0, the bug was known to the core maintainer and they chose not to fully correct it (eg, to save flash) but took the obvious step to reduce the impact, it will be truncated if (duration * frequency) exceeds 4.294 billion. If `SUPPORT_LONG_TONES` is not defined at all, the bug may be present in its original form, in which case the duration will be truncated if (duration * frequency) exceeds 2.14 billion.
* `CORE_HAS_ANALOG_ENH` (1)  - If defined as 1, analogReadEnh() (enhanced analogRead) is available. Otherwise, it is not.
* `CORE_HAS_ANALOG_DIFF` (0 or 1)  - If defined as 1, analogReadDiff() (differential enhanced analogRead) is available. Otherwise, it is not.  It has same features as enhanced, except that it takes a differential measurement.
* `ADC_MAX_OVERSAMPLED_RESOLUTION` (13 or 17) - If either `CORE_HAS_ANALOG_ENH` or `CORE_HAS_ANALOG_DIFF` is 1, this is defined as the maximum resolution obtainable automatically via oversampling and decimation using those functions.
* `ADC_MAXIMUM_GAIN` (0 or 16) - Some parts have an amplifier, often used for differential readings. The Dx-series are not among them (right now it's only the 2-series). If this is defined as a positive number, it is the maximum gain available. If this is defined as -1, there are one or more `OPAMP` peripherals available which could be directed towards the same purpose, though more deliberation would be needed. If it is defined as -128 (which may come out as 128 if converted to an unsigned integer), there is a gain stage on the differential ADC, but it is a classic AVR, so the available gain options depend on which pins are being measured, and there is a different procedure as detailed in the core documentation (ex, ATTinyCore 2.0.0 and later). If it is 0 or undefined, there is no built-in analog gain stage for the ADC, or it is not exposed through the core.

#### Identifying Timers
Each timer has a number associated with it, as shown below. This may be used by preprocessor macros (`#if` et. al.) or `if()` statements to check what `MILLIS_TIMER` is, or to identify which timer (if any) is associated with a pin using the `digitalPinToTimer(pin)` macro.

```c
#define NOT_ON_TIMER 0x00
#define TIMERA0 0x10
#define TIMERA1 0x08 // Not present on any tinyAVR
#define TIMERB0 0x20
#define TIMERB1 0x21
#define TIMERB2 0x22 // Not present on any tinyAVR
#define TIMERB3 0x23 // Not present on any tinyAVR
#define TIMERB4 0x23 // Not present on any tinyAVR
#define TIMERB5 0x23 // Not present on any tinyAVR
#define TIMERD0 0x40
#define DACOUT 0x80  // Not a timer - but used like one by analogWrite()
#define TIMERRTC 0x90
#define TIMERRTC_XTAL 0x91
```

## Bootloader (Optiboot) Support
A new version of Optiboot (Optiboot_x) now runs on the tinyAVR 0-series, 1-series and 2-series chips.  It's under 512 bytes, and works on all parts supported by this core allowing for a convenient workflow with the same serial connections used for both uploading code and debugging (like a normal Arduino Pro Mini) - except for the detail about not having autoreset unless you disable UPDI (except for the 20 and 24-pin 2-series parts which can put reset on PB4 instead), which is a bit of a bummer.

To use the serial bootloader, select a board definition with (optiboot) after it (note - this might be cut off due to the width of the menu; the second set of board definitions are the optiboot ones).

If the chip you will be programming has not been bootloaded, connect your UPDI programmer, and the desired options for clock rate (20/10/5MHz vs 16/8/4/1MHz is all that matters here. The fuses set the base clock to 20MHz or 16MHz, but the prescaler is set at startup by the sketch - so if you "burn bootloader" with 20MHz selected, but upload sketch compiled for 10MHz, that's fine and will work), Brown Out Detect (BOD), Serial port pins for the bootloader, and whether to leave the UPDI pin configured as such, or reconfigure it as a reset pin, and select Tools -> "Burn Bootloader"
**WARNING: After doing "Burn Bootloader", if you set the UPDI pin to act as reset or IO, the chip cannot be reprogrammed except via the serial bootloader or using an HV UPDI programmer - it is strongly suggested to first burn the bootloader with the UPDI/Reset pin left as UPDI, verify that sketches can be uploaded, and only then "Burn Bootloader" with the UPDI/Reset pin set to act as Reset**

After this, connect a serial adapter to the serial pins (as well as ground and Vcc). On the tinyAVR 0/1-series breakout boards which I sell on Tindie, a standard 6-pin "FTDI" header is provided for this that can be connected directly to many serial adapters.


If the UPDI/Reset pin option was set to UPDI or IO when bootloading, you must unplug/replug the board (or use a software reset - see note near start of readme; you can make an ersatz reset button this way) to enter the bootloader - after this the bootloader will be active for 8 seconds.

If the UPDI/Reset pin option was set to reset, you must reset the chip via the reset pin (or software reset to enter the bootloader, and the bootloader will run for 1 second before jumping to the application). This is the same as how optiboot works on classic AVR boards. The standard DTR-autoreset circuit is recommended. If you use the alternate reset pin option on a 2-series part, connect that pin (PB4) to the autoreset circuit, and all should work normally.

### Entry Conditions
The bootloader will wait for 1 or 8 seconds for an upload if it resets and the entry conditions are met:
* If a configuration with a reset pin was selected, the `_rst` version will be installed. It will run after a hardware (reset pin), software reset (see above), or UPDI-reset (ie, immediately after bootloading) only.
* Otherwise, it will run after any of those, as well as power on reset.
* A brownout which causes the BOD to trigger, but where the voltage on the power rails does not fall below approx. 1.25v (which would trigger the "POR rearm") is NOT an entry condition. Yes, this could be from someone attempting to power cycle it to start the bootloader and underestimating how long their capacitors would hold a charge for, which is a time when we would want to enter the bootloader - however, it could also indicate a flaky power supply or weak batteries, in which case we probably don't want to enter the bootloader.
* A software reset is always an entry condition. If you need a reset from software that doesn't run the bootloader, trigger a watchdog reset instead (covered in same section of this readme as software resets).
* A watchdog reset is never an entry condition. The WDT is how Optiboot resets when it's either timed out (actually, the thing that times out *is* the watchdog timer) or finished uploading (at which point it sets timeout to minimum and waits for reset).
* Optiboot clears the reset flag register, and stashes the reset flags in `GPIOR0` - if you need to know the cause of a reset
```c
#ifdef USING_OPTIBOOT
resetflags = GPIOR0;
GPIOR0 = 0; // omit this line if you don't need to use GPIOR0 to save 2-4 bytes of flash.
#else
resetflags = RSTCTRL.RSTFR;
RSRTCTRL.RSTFR = 0;
#endif
if (resetflags & MY_CONDITION_1) {
  // Reset was from cause X, which means something important, so do something differently
} else {
  // do the normal things.
}
```

Serial uploads are all done at 115200 baud, regardless of port, pin mapping or part.

### "Upload using programmer" and Optiboot
This upload method is NOT SUPPORTED by megaTinyCore. If you want to upload via programer, and accept that the bootloader will be lost, use the non-optiboot board definition.

### Removing Optiboot

#### Removing Optiboot - 2.2.0 and later
On 2.2.0 and later, whenever code is uploaded via UPDI ("upload" from non-optiboot board definition), whether or not the device has been bootloaded (like on classic AVRs, "upload using programmer" will trash any bootloader present), `BOOTEND` will be set to match what the code was compiled for; with an optiboot board definition selected, this will result in the first 512b of flash being blank (since we erased any bootloader present), `BOOTEND=2` (512b bootloader section, as application expects), and a working sketch. With a non-optiboot board definition selected, this will result in `BOOTEND=0`, no gap at the start of the sketch, and a working sketch.

#### Removing Optiboot - 2.1.5 and earlier
Like classic AVRs with hardware bootloader support (like the ATmega328p, and all other ATmega parts older than the 4809/4808), and unlike ATtiny parts without such support (those use "virtual boot" to get bootloader functionality) you must do "burn bootloader" with a non-optiboot part selected in order to reprogram the part normally via UPDI.

#### Background, 2.1.5 and earlier
Unlike classic AVRs with hardware bootloader support, where not doing "burn bootloader" before switching to uploading via a programmer would work until sketch size reached the very end of the flash (where the chip thinks the bootloader is), on these parts the sketch would fail in strange and bizarre ways if code is compiled with incorrect assumption about the size or presence of a bootloader (`BOOTEND` fuse) - it could be challenging to identify what the cause of the problem was when this happened - you would typically see pieces of behavior that you recognize, but not in an order that made sense. The reason for this is that the `BOOTEND` fuse sets the location of the interrupt vectors. If the application is started at 0x200, as it would for code compiled for use with Optiboot, but `BOOTEND` is set to 0, the hardware would, when an interrupt fired, jump to the blank section of flash before the application, execution would run along the blank flash until it hit the actual sketch... but it would still think it was in an interrupt, whereas in the inverse scenario, the hardware would jump to the middle of the application code instead of the vector table. Both scenarios are (speaking from experience) incredibly confusing when encountered.

In an effort to prevent these scenarios, starting in 2.2.0, whenever the part is programmed via UPDI (ie, any time except when using the bootloader to upload code), all fuses that are "safe" to set (see above) will be set, including `BOOTEND`.

### Autoreset circuit
If using the bootloader with with the UPDI pin set to reset, this is needed for uploading. It will reset the chip when serial connection is opened like on typical Arduino boards. Do not connect an autoreset circuit to the UPDI pin if it is not set to act as reset, as this will not reset the chip, and will just block UPDI programming. If you are using a 2-series part with at least 20 pins, you can use the "alternate reset" option to make PB4 act as a hardware reset (for Optiboot configurations, this is the default); in this case, follow the below instructions, except use PB4 instead of the reset pin throughout the instructions.
* 1 Small signal diode (specifics aren't important, as long as it's approximately a standard diode)
* 1 0.1uF Ceramic Capacitor
* 1 10k Resistor
* A tinyAVR 0/1-series part with the bootloader installed on it (you can't do UPDI programming once the autoreset circuit is connected the the UPDI/Reset pin, even if that pin hasn't yet been set to act as reset, as the autoreset circuit will interfere with UPDI programming).

Connect the DTR pin of the serial adapter to one side of a 0.1uF ceramic capacitor.
Connect the other side of the capacitor to the Reset (formerly UPDI) pin.
Connect the 10k resistor between Reset and Vcc.
Connect the diode between Reset and Vcc with the band towards Vcc.

The breakout boards I sell on Tindie have a pair of pads on the back that can be bridged with solder to reversibly connect the on-board autoreset circuit to the UPDI/Reset pin. If you buy a breakout board from me which has the autoreset circuit, but which is not bootloaded, these pads will not have been connected - use a UPDI programmer to put the bootloader on it with your desired settings, then bridge the pads on the back to connect the auto-reset circuit. If you buy a breakout board from me with Optiboot preloaded, it will come set for 20/10/5MHz, BOD set to "sampled" mode with a threshold of 3.7v (for 5v boards) or 2.6v (for 3.3v boards) or disabled entirely (for no-regulator boards), and if you ordered a board with the autoreset circuit, the UPDI pin will be set to work as reset (if you need different settings, please mention this when ordering - boards with optiboot preloaded are bootloaded only when the order is packed, so I can use whatever settings you specify).

Note that, if you have UPDI programming enabled, and desire the convenience of autoresetting to enter serial bootloader to upload sketches, or resetting the chip when entering the serial monitor, you can use the technique described near the top of the readme (under the section about there not being a dedicated reset pin) to make an ersatz reset pin that does a software reset when held low - this will enter the bootloader, so if your sketches set up an ersatz reset pin, you can use that to get into the bootloader (this isn't foolproof, because if your sketch chokes before it properly sets up the interrupt, or you choked and forgot to include the ersatz reset code in the last sketch you uploaded - but in this case you can enter bootloader by power cycling as described above). The autoreset circuit works the same way - just connect the parts as described above, only using your ersatz reset pin instead of the UPDI/Reset pin.

#### A few caveats
* The bootloader is at the beginning of memory, rather than at the end (where it was on older chips). Thus, the start of the application code must be 512b after the start of the memory - this is handled by the core, but you cannot upload a .hex file compiled with a non-optiboot board definition to an optiboot board definition and vice versa.
* **If you have set the UPDI/Reset pin to act as a reset pin, you can no longer program the part via UPDI** without using an HV programmer to reset the pin to act as UPDI.
* Currently, Optiboot_x resets the reset cause register after saving the contents in the general purpose register GPIO0. Check this register to determine the reset cause when using the bootloader.
* The new chips have more than one option mapping option for the UART (serial) pins. There is a menu option to choose this, and the one selected when you "burn bootloader" determines which version is used.
* When you "burn bootloader", the base oscillator frequency is set according to the selected clock speed, but the actual operating speed while running the sketch is set in the uploading code. If you initially set it to 16/8/4/1MHz, you may use any of those options when you upload your sketch and it will run at that speed; if you initially set it to 20/10/5MHz, you may use any of those options. If you wish to change between 16/8/4/1MHz and 20/10/5MHz, you must burn bootloader again - failure to do so will result in it running at the wrong speed, and all timing will be wrong.
* The "size" of the sketch as reported by avrdude during the upload process is 512b larger (the size of the bootloader) than the actual sketch size when a bootloader is in use (though the size reported at the end of the compile process is the correct size). The fact that the bootloader goes at the start of the flash instead of the end confuses avrdude. The size displayed by the IDE when you "verify" a sketch is correct, the value that avrdude displays is not.

## Guides
### [Power Saving techniques and Sleep](https://github.com/SpenceKonde/megaTinyCore/blob/master/megaavr/extras/PowerSave.md)
### [Direct Port Manipulation](https://github.com/SpenceKonde/megaTinyCore/blob/master/megaavr/extras/DirectPortManipulation.md)
### [Pin Interrupts](https://github.com/SpenceKonde/megaTinyCore/blob/master/megaavr/extras/PinInterrupts.md)
### [Taking over the PWM timer](https://github.com/SpenceKonde/megaTinyCore/blob/master/megaavr/extras/TakingOverTCA0.md)
### [Advanced ADC features - freerun, window mode, etc](https://github.com/SpenceKonde/megaTinyCore/blob/master/megaavr/extras/ADCFreerunAndMore.md)

## List of Tools sub-menus
* Tools -> Chip - sets the specific part within a selected family to compile for and upload to.
* Tools -> Clock - sets the clock speed. You must burn bootloader after changing between 16/8/4/1MHz and 20/10/5MHz to apply the changes (ie, changing from 20MHz to 10MHz does not require burning bootloader, changing from 20MHz to 16MHz does). A virgin chip will be set to use the 20MHz internal oscillator as its base clock source, so 20/10/5MHz clock speed options should work without an initial "burn bootloader" - though we still recommend it to ensure that all other fuses are set to the values you expect.
* Tools -> Retain EEPROM - determines whether to save EEPROM when uploading a new sketch. You must burn bootloader after changing this to apply the changes. This option is not available on Optiboot board definitions - programming through the bootloader does not execute a chip erase function.
* Tools -> B.O.D. Voltage - If Brown Out Detection is enabled, when Vcc falls below this voltage, the chip will be held in reset. You must burn bootloader after changing this to apply the changes. See the **BOD configuration options** section above for more information.
* Tools -> B.O.D. Mode when Active/Sleeping - Determines the brown-out detection mode in active and sleep modes. You must burn bootloader after changing this to apply the changes. See the **BOD configuration options** section above for more information.
* Tools -> UPDI/Reset pin.
  * If set to UPDI, the pin will be left as the UPDI pin, there will be no hardware reset pin. If using Optiboot, to enter the bootloader, disconnect and reconnect power to the part or execute a software reset - either way, upload within 8 seconds.
  * If set to Reset, the pin will be configured to act as reset, like a classic AVR, but **UPDI programming will no longer work - you must use an HV programmer if you wish to reprogram via UPDI** - if the pin is set to reset, if using Optiboot, the version of the bootloader used will jump straight to the application after a power-on reset, and will only enter the bootloader if reset by software reset or via the reset pin. The bootloader will also only wait 1 second for a sketch (ie, it behaves like optiboot does on classic AVR microcontrollers), instead of 8. If not using a bootloader, further programming requires an HV programmer.
  * If it is set to I/O, the pin will act as a normal I/O pin with incredibly weak pin drivers - don't expect it to be able to source or sink more than 0.5mA when set output. If using Optiboot, the bootloader will behave in the same way as when the pin is left as UPDI. To reprogram using an HV programmer, you must use the Power Cycle High Voltage (PCHV) procedure. Consult the documentation for your HV programmer for more information.
  * On the 2-series 20 and 24-pin part, an additional options is available, and the default when using Optiboot with those parts: Alternate Reset; with this option selected the UPDI pin retains it's UPDI functionality, but the PIN_PB4 ceases to be an I/O pin, and instead acts like an external RESET pin! In light of the continued scarcity of HV UPDI programmers within the hobbyist community
* Tools -> Startup Time - This is the time between reset (from any cause) and the start of code execution. We default to 8ms and recommend using that unless you have a reason not to - that default option is generally fine... In rare cases (such as particularly slow rising power supplies when BOD is not enabled, such that it needs to wait longer than the usual 8ms to have a voltage high enough to function reliably at, or conversely,  when the power supply is known to be fast rising (or BOD is in use) and you have need to respond almost instantly after a reset)
* Tools -> Voltage Baud Correction - If you are using the internal oscillator and reaaaaally want the UART baud rates to be as close to the target as possible you can set this to the voltage closer to your operating voltage, and it will use the factory programmed internal oscillator error values. Under normal operation, this just wastes flash and is not needed. That is why it now (as of 2.3.0) defaults to Ignore.
* Tools -> millis()/micros() - If set to enable (default), millis(), micros() and pulseInLong() will be available. If set to disable, these will not be available, Serial methods which take a timeout as an argument will not have an accurate timeout (though the actual time will be proportional to the timeout supplied); delay will still work. Disabling millis() and micros() saves flash, and eliminates the millis interrupt every 1-2ms; this is especially useful on the 8-pin parts which are extremely limited in flash. Depending on the part, options to force millis/micros onto specific timers are available. A #error will be shown upon compile if a specific timer is chosen but that timer does not exist on the part in question (as the 0-series parts have fewer timers, but run from the same variant). If RTC is selected, micros() and pulseInLong() will not be available - only millis() will be.
* Tools -> UART for Bootloader - If using Optiboot bootloader, select which set of pins you want to use for serial uploads. After making a selection, you must connect a UPDI programmer and do tools -> Burn Bootloader to upload the correct bootloader for the selected option.

## Warning: Direct Register Manipulation
If you are manually manipulating registers controlling a peripheral, you should not count on the behavior of API functions that relate to the same peripheral, nor should you assume that calling said API functions will not adversely impact the rest of your application. For example, if you "take over" TCA0, you should not expect that using analogWrite() - except on the two pins on the 20/24-pin parts controlled by TCD0 - will work for generating PWM; indeed you should expect that it will break whatever you are doing with TCA0.

## Warning: I2C **requires** external pullup resistors
Earlier versions of megaTinyCore enabled the internal pullup resistors on the I2C pins. This is no longer done; they are miles away from being strong enough to meet the I2C specifications - it was decided that it is preferably for it to fail consistently without external ones than to work under simple conditions with the internal ones, yet fail under more demanding ones (more devices, longer wires, etc).

## Differences in Behavior between megaTinyCore and Official Cores
While we generally make an effort to emulate the official Arduino core, there are a few cases that were investigated, but we came to the conclusion that the compatibility would not be worth the price. The following is a (hopefully nearly complete) list of these cases.

### Serial does not manipulate interrupt priority
The official core for the (similar) megaAVR 0-series parts, which this was based on, fiddles with the interrupt priority (bet you didn't know that!) in ways that are of dubious safety towards other code. megaTinyCore does not do this (in the process saving several hundred bytes of flash, and fixing at least one serious bug which could result in hangs if serial was used in ways that everyone tells you not to use it). Writing to Serial when it's buffer is full, or calling `Serial.flush()`  with interrupts disabled, or during another ISR (which you *really shouldn't do*) will behave as it does on classic AVRs, and simply block, manually calling the transmit handlers, until there is space in the buffer for all of the data waiting to be written, or the buffer is empty (for flush()).

### SerialEvent support is dropped
This is deprecated on the official core, and is and always has been a dreadful misfeature. Dropped as of 2.3.0.

### digitalRead() does not turn off PWM
On official cores, and most third party ones, the digitalRead() function turns off PWM when called on a pin. This behavior is not documented by the Arduino reference. This interferes with certain optimizations, makes digitalRead() take at least twice as long as it needs to - and generally makes little sense. Why should a "read" operation change the thing it's called on? We have a function that alters the pin it's called on: `digitalWrite()`. There does not seem to be a logically coherent reason for this - and insofar as Arduino is supposed to be an educational platform it makes simple demonstrations of what PWM is non-trivial (imagine setting a pin to output PWM, and then looking at the output by repeatedly reading the pin). See also the above section on PWM using TCD0.

### digitalWrite()/pinMode() and INPUT pins
Like the official "megaavr" core, calling digitalWrite() on a pin currently set INPUT will enable or disable the pullups as appropriate. Additionally, as of 2.2.0, megaTinyCore fixes two bugs in this "classic emulation". On a classic core, digitalWrite() on an INPUT would also write to the port output register - thus, if one subsequently called pinMode(pin,OUTPUT), the pin would immediately output that level. This behavior is not emulated in the official core, and there is a considerable amount of code in the wild which depends on it. digitalWrite() now replicates that behavior. digitalWrite() also supports "CHANGE" as an option; on the official core, this will turn the pullup on, regardless of which state the pin was previously in, instead of toggling the state of it. The state of the pullup is now set to match the value that the port output register was just set to.

Similarly, using pinMode() to set a pin to INPUT or INPUT_PULLUP will now also set the port output register. **warning** There are a few cases where this is problematic due to silicon bugs in available hardware (most notably configuring I2C pin - where one may well want the pullups on (not that they are sufficient to meet the I2C requirements), but you must not have the output register set high. The Wire.h library works around this. The USART in Open Drain mode is another case)

### analogWrite() and TCD0 pins
Please see the above PWM feature description if using PWM on those pins and also using digitalRead() or direct port writes on the same pins (PIN_PC0, and PIN_PC1).

### TCA0 configured in Split Mode to get 3 additional PWM pins
On the official "megaavr" board package, TCA0 is configured for "single mode" as a three-channel 16-bit timer (used to output 8-bit PWM). megaTinyCore always configures it for "Split mode" to get additional PWM outputs. See the datasheets for more information on the capabilities of TCA0. See [Taking over TCA0](https://github.com/SpenceKonde/megaTinyCore/blob/master/megaavr/extras/TakingOverTCA0.md) for information on reconfiguring it. One downside to this is that the compare channels do not support buffering, so changing the duty cycle can cause a glitch lasting up to PWM cycle (generally under 1 ms).

### TCA0 TOP is 254, not 255
0 is a count, so at 255, there are 256 steps, and 255 of those will generate PWM output - but since Arduino defines 0 as always off and 255 as always on, there are only 254 possible values that it will use. The result of this is that (I don't remember which) either analogWrite(pin,254) results in it being LOW 2/256's of the time, or analogWrite(pin,1) results in it being HIGH 2/256's of the time. On megaTinyCore, with 255 steps, 254 of which generate PWM, the hardware is configured to match the API, and this does not occur; as it happens, 255 also (mathematically) works out such that integer math gets exact results for millis timing with both 16 MHz derived and 20 MHz derived clock speeds (relevant when TCA0 is used for millis() timing). The same thing is done for TCD0 (though to 509, giving 510 steps - analogWrite() accounts for this - so that we can get the same output frequency while keeping the fastest synchronization prescaler for fastest synchronization between TCD0 and system clock domains).

### Type B timers not used for PWM
On the official "megaavr" board package, as well as [DxCore](https://github.com/SpenceKonde/DxCore), the type B timers are used to generate 8-bit PWM (one pin per timer), there are very few circumstances where this could increase the number of usable PWM pins. These timers are just too scarce and valuable on these parts. Being minimally useful for PWM, in short supply, and highly desirable for other purposes, support for using them for PWM was removed in order to save space that would otherwise be used initializing these timers for PWM and handling them in analogWrite() et. al. If a Type B timer is used for millis(), it is configured in a radically different way than the official core does.

### digital I/O functions use old function signatures
They return and expect uint8_t (byte) values, not enums like the official megaavr board package does. Like classic AVR cores, constants like LOW, HIGH, etc are simply #defined to appropriate values. The use of enums unfortunately broke many common Arduino programming idioms and existing code (granted, these idioms were poor programming practice - they're also incredibly widespread and convenient), increased flash usage, lowered performance, and made optimization more challenging. While the enum implementation made language design purists comfortable, and provided error checking for newbies - because you couldn't pass anything that wasn't a PinState to a digital I/O function, and would get that error checking if - as a newbie - you accidentally got careless. So, due to all the complaints, a compatibility layer was added to the official core, so all the old tricks would work again, it was just less performant. However, that got rid of what was probably the most compelling benefit, the fact that it did generate an error for new users to train them away from common Arduino practices like passing 1 or 0 to digitalWrite, `if(digitalRead(pin))` and the like. The choice of names of the enums also had the perverse effect of making PinMode(pin,OUTPUT), an obvious typo of pinMode(pin,OUTPUT) into valid syntax (comma operator turns pin,OUTPUT into OUTPUT, and it returns a new PinMode of value OUTPUT...) and does nothing with it, instead of a syntax error (It took me over an hour to find the extra capitalization. That evening, I converted the digital I/O functions to the old signatures and removed the enums). Anyway - the enums are not present here, and they never will be; this is the case with [MegaCoreX](https://github.com/MCUdude/MegaCoreX) and [DxCore](https://github.com/SpenceKonde/DxCore) as well.

## Known Compiler Bugs in 2.0.3 and Earlier
The earlier versions of the avr-gcc toolchain contained several issues relevant to these parts. Version 2.0.4 and later, when installed through Board Manager, will pull in the new toolchain package which corrects both of these issues (among other ones that nobody had noticed yet)
* Sometimes a sketch which is too big to fit will, instead of generating a message saying that, result in the error: 'relocation truncated to fit: R_AVR_13_PCREL against symbol tablejump2'.
* There is a bug in the `<avr/eeprom.h>` library's eeprom_is_ready() macro - it uses names for registers from the XMEGA family of parts (under the hood, these chips actually have an XMEGA core!). Attempting to use that macro will generate a error stating that `'NVM_STATUS' was not declared in this scope`. This has been corrected in current compiler packages; if you get this error, update the official AVR board package, upgrade to Arduino 1.8.13, or install megaTinyCore using board manager.

## Very low-level differences
There are two classes of significant low level architectural differences - the improved instruction set, and the unified memory address space.

### Instruction Set (AVRe/AVRe+ vs AVRxt)
The classic AVR devices all use the venerable `AVRe` (ATtiny) or `AVRe+` (ATmega) instruction set (`AVRe+` differs from `AVRe` in that it has hardware multiplication and supports devices with more than 64k of flash). Modern AVR devices (with the exception of ones with minuscule flash and memory, such as the ATtiny10, which use the reduced core `AVRrc`) all use  the latest iteration of the AVR instruction set, `AVRxt`. `AVRxt` has much in common with `AVRxm` (used in XMega parts) in terms of instruction timing - and in the few places where they differ, `AVRxt` is faster (SBIC, as well as LDD, and LD with predecrement, are all 1 clock slower on `AVRxm` vs `AVRxt` or `AVRe`), however `AVRxt` doesn't have the single-instruction-two-clock read-and-write instructions for memory access `LAT`, `LAC`, `LAS`, and `XCH`. The difference between subspecies of the AVR instruction set is unimportant for 99.9% of users - but if you happen to be working with hand-tuned assembly (or are using a library that does so, and are wondering why the timing is messed up), the changes are:
* Like AVRe+ and unlike AVRe (used in older tinyAVR), these do have the hardware multiplication.
* PUSH is 1 cycle vs 2 on classic AVR (POP is still 2)
* CBI and SBI are 1 cycle vs 2 on classic AVR
* LDS is 3 cycles vs 2 on classic AVR :disappointed: LD and LDD are - as always - two cycle instructions.
* RCALL and ICALL are 2 cycles vs 3 on classic AVR
* CALL is 3 cycles instead of 4 on classic AVR
* ST and STD is 1 cycle vs 2 on classic AVR (STS is still 2 - as any two word instruction must be)

As you can see, everything that involves writing to the SRAM is faster now; it would appear that any time it is writing to a location based on one of the pointer registers or the stack pointer, it's a single cycle. This has the knockon effect of making all the variants of call faster too, since they have to put the return address into the stack. It would appear that, other than the improvement to SBI/CBI, all the differences between AVRxm, AVRxt, and AVRe+ are related to how it interacts with the memory. I can't say I've ever felt like LAT, LAC, or LAS would be terribly useful as they are described in the instruction set manual - those take a register and the address pointed to by the Z register, and load the contents of the specified address, and toggle, set or clear in that memory address the bits that were set to begin with in the register. If that worked on special function registers, it would be a very useful instruction. But it says they only work on SRAM... so not as much of a loss. `XCH` is more obviously useful than the others, but all 4 of them comr with the need to set up the Z register... which in many cases would take out the most exciting use cases.

Note that the improvement to PUSH can make interrupts respond significantly faster (since they have to push the contents of registers onto the stack at the beginning of the ISR), though the corresponding POP's at the end aren't any faster. The change with ST impacted tinyNeoPixel. Prior to my realizing this, the library worked on SK6812 LEDs (which happened to be what I tested with) at 16/20 MHz, but not real WS2812's. However, once I discovered this, I was able to leverage it to use a single tinyNeoPixel library instead of a different one for each port like was needed with ATTinyCore (for 8 MHz, they need to use the single cycle OUT on classic AVRs to meet timing requirements, the two cycle ST was just too slow; hence the port had to be known at compile time, or there must be one copy of the routine for each port, an extravagance that the ATtiny parts cannot afford. But with single cycle ST, that issue vanished).

Oh, and one other instruction it doesn't have that (some) AVRxm parts have: The hardware DES encryption instruction - an instruction which is most effective at marking AVRxm as, ah, back from the time when DES was a big deal.

### Memory and address space organization
On all modern AVRs with up to 48k of flash, both the flash and ram reside in the same address space - On tinyAVRs, the program memory starts at 0x8000, while on megaAVR 0-series, it starts at 0x4000 to leave room for the 48k of flash that they can have, and on the Dx-series parts with up to 32k of flash, they have the same layout as the tinyAVRs, while Dx-series parts with 64k or 128k of flash have a 32k section of flash mapped at any given time (how to make sure variables go into this memory mapped flash has been described elsewhere in this document). There is another big and fundamental change to the layout of the address space as well: the registers are organized by peripheral. PORTA is assigned 0x400 to 0x41F. PORTB is the next 32 bytes, and so on - and the address space is far sparser - all the peripherals have multiple "reserved" registers that may or may not get functions added in the future. And each instance of a peripheral on a part that has multiple of them has the same layout. You can, say, pass a pointer to a TCB around without the functions that get it knowing which TCB they'll get, and then access the TCB registers through it. On classic AVRs the names of the registers were consistent, but their locations were all over the place, packed much more tightly, so that sort of trick isn't possible. This also means that the EEPROM (and USERROW) are part of this unified address space (on classic AVRs, reading was accomplished through special function registers, and was far more awkward. So is the full signature space. )

#### The "I/O space" is used differently
The lowest 64 registers are special - you can read or write them with the IN or OUT instructions (hence, "I/O space") in a single clock cycle, *without* setting up a pointer to them as you would need to with ST or LD. The 32 "Low I/O registers" additionally have bit-level access instructions CBI and SBI to clear and set bits, and SBIC/SBIS to skip the next instruction if a certain bit is set or cleared. On all AVRxt parts released so far, the low I/O registers are used only for the VPORTs up to VPORTG (meaning VPORTG.OUT |= 1<<n (where n is compile-time known and constant) is 1 clock cycle and atomic, while VPORTG.OUT = 1<<n is two - first to put the value to be stored into a register, and secondly to write it with an `out` instruction.  The GPIOR0-3 registers occupying the last 4 bytes in the low I/O space (those are user-defined registers to use as you choose, megaTinyCore's version of Optiboot stashes the reset cause in GPIOR0 before resetting reset cause (so it can honor bootloader entry conditiosn on next reset) and jumping to app; No other part of this core touches those registers, and we only set GPIOR0 (and only when using optiboot), never read it. So all can be used freely, as long as you remember to clear GPIOR0 if using Optiboot). On parts which do not have 7 ports, for example tinyAVR parts with only ports A, B, and C, there's a VPORTA, VPORTB and VPORTC taking up the first 12 locations in the low I/O space. And then the rest are "reserved" up until the GPIO registers.

The 32 "high I/O registers" are used even less - they only get the the stack pointer, RAMPZ on the 128k DA/DB parts, SREG, and CCP (Configuration Change Protection - where `_PROTECTED_WRITE()` does it's magic to let you write to protected registers. That's all - 5 out of 32 registers are used, the rest are "reserved". On classic AVRs, they took frequently used registers from all over the register space and put them there, since they can be read and written faster and more efficiently. It is somewhat disappointing that they didn't mirror some of the most used peripheral registers into the high I/O space. (Interrupt flags? Serial/SPI/I2C data?) This has the consequence of making some register access operations both larger and slower than the would be on classic AVRs which took advantage of the I/O space. It is not clear to me how much of an impact this ends up having on sketch size, nor why it was done. I'm sure it was discussed, and they had good reasons not to.... but it seems like kind of a waste.

## License
megaTinyCore itself is released under the [LGPL 2.1](LICENSE.md). It may be used, modified, and distributed freely, and it may be used as part of an application which, itself, is not open source (though any modifications to these libraries must be released under the LGPL as well). Unlike LGPLv3, if this is used in a commercial product, you are not required to provide means for user to update it.

The megaTinyCore hardware package (and by extension this repository) contains megaTinyCore as well as libraries, bootloaders, and tools. These are released under the same license, *unless specified otherwise*. For example, tinyNeoPixel and tinyNeoPixel_Static, being based on Adafruit's library, is released under GPLv3, as described in the LICENSE.md in those subfolders and within the body of the library files themselves.

The pyupdi-style serial uploader in megaavr/tools is built on pymcuprog from Microchip, which ~is not open source~ *has now been released under the open source MIT license!*

Any third party tools or libraries installed on behalf of megaTinyCore when installed via board manager (including but not limited to, for example, avr-gcc and avrdude) are covered by different licenses as described in their license files.
