<!-- markdownlint-disable MD041 -->
# megaTinyCore - Arduino support for all tinyAVR 0/1/2-Series
## [Questions? Cases where you don't know whether the problem is in your code or the core? Things to show off?](https://github.com/SpenceKonde/megaTinyCore/discussions)
We have a Github Discussions section now. When a problem is *definitely* a defect in the core, you will reduce the time taken to fix it if you create an issue, as I prioritize issues over catching up on discussions.

### 2.6.1 is out - rejoice
Improved flash footprint of serial significantly while adding features, wire can wake slave from sleep without corrupting data, and much, much more, see [the Changelog](ChangeLog.md).

## IMPORTANT WARNINGS

### IDE 2.0.x unsupported. If you use it, use the release not an old RC: all versions prior to 2.0.0-RC9.2 known to have critical regressions
These bugs in the IDE prevent board settings from being correctly recognized. [This thread tracks known issues with 2.0 and workarounds](https://github.com/SpenceKonde/megaTinyCore/discussions/760). If you use unsupported software please reproduce all issues in 1.8.13 before reporting.

V1.8.13 is the "golden version" and the only one I recommend. All the more recent ones gained bugs, and its the last version with a substantial improvement. Be aware it does have a vulnerable version of Log4J.
Prior to megaTinyCore 2.6.0, manual installation of megaTinyCore would cause V1.8.14 of the IDE to crash due to [this bug](https://github.com/arduino/Arduino/issues/11813) when you install the core manually in your arduino folder and not when you install the core via boards manager. If you are manually edit platform.txt with
```diff
-version={versionnum.major}.{versionnum.minor}.{versionnum.patch}{versionnum.postfix}
+version=2.6.0
```
it will then work with V1.8.19.

Having determined that Arduino doesn't care about this issue (it's as if they have no concern for third party hardware - almost like they were selling hardware themselves or something).

### **Don't Buy Modern AVRs (Anything This Core Supports) on AliExpress or From eBay International Sellers**
I buy a lot of electronics stuff on AliExpress. It's a great marketplace for things that are made by Chinese companies and are mostly generic, including loads of components unavailable to individuals in the global West any other way (ex, min order is a reel or something like that). It is not a great place for the latest semiconductor product lines from major Western manufacturers, especially in the midst of a historic shortage of said chips. The modern AVR devices, when they are available through those channels at all, are **frequently reported to be fake or defective** (like ATtiny412s that think they're 416s and may not correctly execute power on reset). For that matter, **you probably don't want to buy any AVR microcontrollers on AliExpress**... Assembled boards, like Arduino Nano clones, generally work if you avoid the ones with the third party LGT8 chips and watch out for the ones with the ATmega168p instead of the '328p - but there are a lot of reports of bogus microcontrollers (I have heard of fake ATtiny85s that were actually remarked ATtiny13s, for example). There are a lot of interesting theories for where those bogus chips came from, and Microchip has remained totally silent on the issue.

Buy them from reputable distributors like Digikey or Mouser (if in a hurry, and they're on backorder - which is the case for almost everything during the Great Chip Shortage of 2022 - buy from a reputable USA/EU-based scalper, to the extent that a reputable scalper isn't an oxymoron; If you're in a pinch and need small numbers of a part, I may know a guy who can help you out (email me) but his help doesn't come cheap).

### Notice Regarding Update Issues
Well, I screwed up fairly badly in handling the board manager json. The short version of the story is that I discovered a bug in 2.4.3 that required no changes to the core, just a board manager change. So I changed the json file (big mistake). Anyone who installed after that change would get a java null pointer exception when trying to upgrade. But I didn't realize this until I had already done a release in 2.5.x, then realized I'd missed that fix, and modified the board manager json after release. Then I finally made the connection between having done that and the flood of support inquiries relating to being unable to upgrade.

If you are impacted by this, exit all instances of the IDE, delete all folders within your Arduino15 folder (reinstalling the Arduino IDE won't fix the error - arduino15 is located in some other location depending on your operating system; on windows it's C:/Users/(your username)/AppData/Local/Arduino15 - note that AppData is hidden). Deleting the folders within Arduino15 instead of the whole Arduino15 folder will allow you to keep your saved preferences file, while getting rid of any trace of the mess that my defective board manager json made of your packages.

Sorry about that. I've put in place measures to ensure I don't make such a colossal screw-up again.

## Using This Document
This document is best viewed online if you installed via board manager - [https://github.com/SpenceKonde/megaTinyCore](https://github.com/SpenceKonde/megaTinyCore)

## Getting Started
### [Wiring](Wiring.md)
### [Installation](Installation.md)
### **Arduino 1.8.13 is *strongly* recommended**

Older versions do not properly handle the programmers in the tools -> programmers menu, which degrades the UX rapidly as the number of installed cores increases. They are not suitable. The newest versions starting with 1.8.14 (including 1.8.17, 1.8.18, and 1.8.19) may generate a "panic: no major version found" error because they [fail to properly parse platform.txt](https://github.com/arduino/Arduino/issues/11813). Since 2.6.0 we have been manually modifying the platform.txt directly before release, so this is less of an issue

When megaTinyCore is installed through board manager, the required version of the toolchain is installed automatically. All 0/1/2-Series parts are supported with no extra steps.
Up until 2.2.7, we used Arduino7 version of avr-gcc (gcc 7.3.0 and avrlibc 3.6.1) with latest ATpacks as of june 2020.
Starting with 2.2.7, we began using my Azduino build of the toolchain, which has updated ATpacks for all the newly supported parts. 2.2.7 used Azduino3, 2.3.0+ used Azduino4, and starting with 2.6.0, we use Azduino5 (though it offers no benefit for us, other than saving a quarter GB of HDD space and 40mb of download bandwidth if you install both megaTinyCore and DxCore through board manager.

Manual installation is more complicated - particularly if you want support for the 2-Series; see the [Installation guide for more information](Installation.md).

## **megaTinyCore** - Background and Basics
An Arduino core for the tinyAVR 0-Series, 1-Series, and now the 2-Series. These parts have an improved architecture compared to the "classic" tinyAVR parts (which are supported by [ATTinyCore](https://github.com/SpenceKonde/ATTinyCore)), with improved peripherals and improved execution time for certain instructions (these are similar in both regards to the advanced AVR Dx-Series, as well as megaAVR 0-Series chips like the ATmega4809 as used on the official Nano Every and Uno Wifi Rev. 2 - although the Arduino team has done their best to kneecap them) in the low-cost, small packages typical of the ATtiny line. All of these parts feature at least one hardware UART, and an SPI and TWI interface (none of that USI garbage like, for example, the ATtiny85 has), a powerful event system, configurable custom logic, at least one on-chip analog comparator, a surprisingly accurate internal oscillator, and in the case of the 1-Series, an actual DAC output channel, and in the case of the 2-Series, a fancy differential ADC.

Moreover, the tinyAVR 0/1/2-Series parts are *cheap* - the highest end parts, the 3226 and 3227, with 32k of flash and 3k of SRAM (vs the 2k SRAM as the ATmega328p used in Uno/Nano/ProMini) run just over $1 USD in quantity - less than many 8k classic AVR ATtiny parts ("AVR instruction set, at a PIC price"). All of these parts are rated to run at 16 MHz or 20 MHz (at 4.5-5.5v) without an external crystal, and the internal oscillator is accurate enough for UART communication. Or I mean it would be just over $1 if the company wasn't backordered by a year on almost everything; I know I guy who got for $250 for 50pcs t3217's and thinks he should have demanded more (I believe he's still operating)

These use UPDI programming, not traditional ISP like the classic ATtiny parts did. See below for more information. Getting a UPDI programmer is simple - you can use a classic 328p-based Arduino as programmer using jtag2updi - or for better results with cheaper hardware, you can use *any USB-serial adapter* and a resistor (and preferably a diode) using the included SerialUPDI tool, or you can use AVRdude with one of the Microchip programmers (the mEDBG/nEDBG/EDBG-based programmers on their development board, Atmel-ICE or SNAP) or any UPDI programming tool that emulates one of those (which, to my knowledge, all of them do - if there is one that avrdude supports and that my core doesn't, please open an issue to let me know!).

A serial bootloader, Optiboot_x (based on the same codebase as the classical Arduino Uno bootloader, though since greatly altered) is supported on these parts (0/1-Series support is currently live, 2-Series is expected by the first week of May; adjustments for the new parts are trivial), allowing them to be programmed over a traditional serial port. See the Optiboot section below for more information on this, and the relevant options. Installing the bootloader does require a UPDI programmer. The assembled breakout boards I sell on Tindie are available pre-bootloaded (they are bootloaded on demand). **That being said** the user experience with Optiboot is a little disappointing on the 0/1-Series parts as well as the 14-pin 2-Series parts, due to their lack of a hardware reset pin that could be used with the usual autoreset circuit to automatically reset into the bootloader when the serial port is opened. You need to either disable UPDI programming entirely (requiring an HV programmer if fuse settings or bootloader need to be change after initial bootloading) or leave UPDI enabled, but start any upload within 8 seconds of applying power. The 20-pin and 24-pin 2-Series parts support an "alternate reset pin" allowing these to act more like a traditional Arduino.

## UPDI Programming
The UPDI programming interface is a single-wire interface for programming (and debugging - **U**niversal **P**rogramming and **D**ebugging **I**nterface), which is used used on the tinyAVR 0/1/2-Series, as well as all other modern AVR microcontrollers. While one can always purchase a purpose-made UPDI programmer from Microchip, this is not recommended when you will be using the Arduino IDE rather than Microchip's (godawful complicated) IDE. There are widespread reports of problems on Linux for the official Microchip programmers. There are two very low-cost alternative approaches to creating a UPDI programmer, both of which the Arduino community has more experience with than those official programmers.

### From a USB-Serial Adapter With SerialUPDI (pyupdi-style - Recommended)
Before megaTinyCore existed, there was a tool called [pyupdi](https://github.com/mraardvark/pyupdi) - a simple Python program for uploading to UPDI-equipped microcontrollers using a serial adapter modified by the addition of a single resistor. But pyupdi was not readily usable from the Arduino IDE, and so this was not an option. As of 2.2.0, megaTinyCore brings in a portable Python implementation, which opens a great many doors; Originally we were planning to adapt pyupdi, but at the urging of its author and several Microchip employees, we have instead based this functionality on [pymcuprog](https://pypi.org/project/pymcuprog/), a "more robust" tool developed and "maintained by Microchip" which includes the same serial-port upload feature, only without the performance optimizations. **If installing manually** you must [add the Python package](https://github.com/SpenceKonde/megaTinyCore/blob/master/megaavr/tools/ManualPython.md) appropriate to your operating system in order to use this upload method (a system Python installation is not sufficient, nor is one necessary).

Read the [**SerialUPDI documentation**](https://github.com/SpenceKonde/AVR-Guidance/blob/master/UPDI/jtag2updi.md) for information on the wiring.

As of 2.3.2, with the dramatic improvements in performance, and the proven reliability of the wiring scheme using a diode instead of a resistor, and in light of the flakiness of the jtag2updi firmware, this is now the recommended programming method. As of this version, programming speed has been increased by as much as a factor of 20, and now far exceeds what was possible with jtag2updi (programming via jtag2updi is roughly comparable in speed to programming via SerialUPDI on the "SLOW" speed option, 57600 baud; the normal 230400 baud version programs about three times faster than the SLOW version or jtag2updi, while the "TURBO" option (runs at 460800 baud and increases upload speed by approximately 50% over the normal one. The TURBO speed version should only be used with devices running at 4.5v or more, as we have to run the UPDI clock faster to keep up (it is also not expected to be compatible with all serial adapters - this is an intentional tradeoff for improved performance), but it allows for upload and verification of a 32kB sketch in 4 seconds.

#### Coming before year end 2022: HV programming tool
An HV programming tool to be called HyperUPDI is expected to be available (though silicon shortages may limit quantities) by year end. It is *not* intended to replace SerialUPDI.
* HV programming support for AVR-DD, AVR-EA, and tinyAVR parts, allowing the UPDI pin to be used as GPIO without precluding further programming.
* An on-board buffer will allow data to be sent in chunks of 2k or more. The result of this will be a dramatic improvement in programming speed. I expect an improvement of perhaps 5-10% on Dx-series vs SerialUPDI (as it is already very close to the theoretical maximum) - but the benefits on tinyAVR c will be considerably greater, as at TURBO speed they spent half their time in a USB latency period/
* When a normal serial console is used to access it, it will operate in passthrough mode, featuring the classic FTDI pinout.
* When in programming mode, the nominal CTS line is used to output the UPDI signal. Many boards (including those I sell) now have a solder-jumper to connect CTS to UPDI. This will allow a you to upload via UPDI and then open the serial console without changing any connections nor the use of a bootloader!
* It will utilize a new upload script (neither Prog.py nor avrdude) which leverages the python installation we bring in for SerialUPDI already.
* Because of the built in awareness of the UPDI protocol and the NVMCTRL of supported parts other features like partial erase (to supplement Flash.h on the Dx-series).
* Voltage options of 5V, 3.3V, and V<sub>target</sub> will be selectable with a slide switch, allowing programming where the target voltage is as low as 1.8V, programming devices running directly off LiPo batteries at 3.7-4.2v and so on.
* Due to the considerably more complex hardware, HyperUPDI will obviously not be a $1 device like SerialUPDI (which I expect most people will continue to use)

#### Realistically coming before year end 2022: Superior serial adapters
A single-port serial adapter with a switch that toggles between UPDI and normal mode and another switch for 5v and 3.3v, and which exposes all modem liaison pins, with an optional stackable rider board that uses the more durable JST-XH connectors to help deal with the incredibly short lifespan of a dupont cable when beingfrequently plugged in and unplugged, particularly if you don't always connect them with perfect precision. This is a feature I added primarily with myself in mind, but if there is demand, I can totally get bulk JST-XH <-> Dupont adapter cables made - but is this worthwhile? That's a good question, because the whole reason I made the rider board was that dupont connectors have a short operating life - so you'd still need to be able to replace the dupont terminals your self or buy new adapter cables from me (note: the 3-pin cable suffers this issue far more often than the 6-pin ones. One might even consider putting a JST-XH header on the target board for this reason. The rider board will also feature 2 RGB leds facing sideways (so you can see them in almost any orientation) that can be enabled to show the status of modem liaison lights (this is particularly helpful for debugging).

#### (New in 2.5.6) What's With All The Different SerialUPDI Options?
Depending on adapter model, and operating system, it has been found that different timing settings are required; however, settings needed to keep even 230400 baud from failing on Linux/Mac with most adapters impose a much larger time penalty on Windows, where the OS's serial handling is slow enough that nothing needs that delay...

The "write delay" mentioned here is to allow for the page erase-write command to finish executing; this takes a non-zero time. Depending on the adapter, USB latency and the implicit 2 or 3 byte buffer (it's like a USART, and probably implemented as one internally. The third byte that arrives has nowhere to go, because the hardware buffer is only 2 bytes deep) may be enough to allow it to work without an explicit delay. Or, it may fail partway through and report an "Error with st". The faster the adapter's latency timeout, and the faster the OS's serial handling is, the greater the chance of this being a problem. This is controlled by the `-wd` command line parameter if executing prog.py manually. As of 2.5.6 this write delay is closer to the actual time requested (in ms), previously it had a granularity of several ms, when 1 is all you needed, and as a result, the penalty it imposed was *brutal*, particularly on Windows.

Selection guide:
* 460800+ baud requires the target to be running at 4.5V+ to remain in spec (in practice, it probably doesn't need to be quite that high - but it must be a voltage high enough to be stable at 16 MHz. We set the interface clock to the maximum for all speeds above 230400 baud - while a few adapters sometimes work at 460800 without this step (which in and of itself is strange - 460800 baud is 460800 baud right?), most do not and SerialUPDI doesn't have a way of determining what the adapter is.
* CH340-based adapters have high-enough latency on most platforms, and almost always work at any speed without resorting to write delay. All options work without using the write delay.
* Almost all adapters work on Windows at 230.4k without using the write delay. A rare few do not, including some native USB microcontrollers programmed to act as serial adapters (ex: SAMD11C).
* Almost nothing except the CH340-based adapters will work at 460.8k or more without the write delay, regardless of platform.
* On Windows, many adapters (even ones that really should support it) will be unsuccessful switching to 921600 baud. I do not know why. The symptom is a pause at the start of a few seconds as it tries, followed by uploading at 115200 baud. The only one I have had success with so far is the CH340, oddly enough.
* 460800 baud on Windows with the write delay is often slower than 230400 baud without it. The same is not true on Linux/Mac, and the smaller the page size, the larger the performance hit from write delay.
* 57600 baud should be used if other options are not working, or when programming at Vcc = < 2.7V.
* 460800 baud works without the write delay on some adapters with a 10k resistor placed across the Schottky diode between TX and RX, when it doesn't work without that unless the write delay is enabled. No, I do not understand how this could be either!
* As you can see from the above, this information is largely empirical; it is not yet known how to predict the behavior.

#### Why is My FTDI Adapter Insanely Slow?
FTDI adapters (FT232, FT2232, and FT4232 etc), including the fake ones that are available on eBay/AliExpress for around $2, on Windows default to an excruciatingly long latency period of 16ms. Even with the lengths we go to in order to limit the number of latency delay periods we must wait through, this will prolong a 2.2 second upload to over 15 seconds. You must change this in order to get tolerable upload speeds:
1. Open control panel, device manager.
2. Expand Ports (COM and LPT)
3. Right click the port and choose properties
4. Click the Port Settings tab
5. Click "Advanced..." to open the advanced settings window.
6. Under the "BM Options" section, find the "Latency Timer" menu, which will likely be set to 16. Change this to 1.
7. Click OK to exit the advanced options window, and again to exit properties. You will see device manager refresh the list of hardware.
8. Uploads should be much faster now.

### With a Classic Arduino (jtag2updi)
One can be made from a classic AVR Uno/Nano/Pro Mini; inexpensive Nano clones are the usual choice, being cheap enough that one can be wired up and then left like that. We no longer provide detailed documentation for this processes; jtag2updi is deprecated. If you are still using it, you should select jtag2updi from the tools->programmer menu. This was previously our recommended option. Due to persistent jtag2updi bugs, and its reliance on the largely unmaintained 'avrdude' tool (which among other things inserts a spurious error message into all UPDI uploads made with it), this is no longer recommended.

## Compatibility Note for 32-bit Linux
Apparently Arduino isn't packaging 32-bit versions of the latest avrdude.
I defined a new tool definition which is a copy of arduino18 (the latest) except that it pulls in version 17 instead on 32-bit Linux, since that's the best that's available for that platform. The arduino17 version does not correctly support uploading with some of the Microchip programming tools.

This is currently used only for the last few releases, and should fix the avrdude not available for this platform error.

## Supported Parts (Click Link for Pinout Diagram and Details)
* tinyAVR 2-Series
  * [ATtiny3227,1627,827,427](https://github.com/SpenceKonde/megaTinyCore/blob/master/megaavr/extras/ATtiny_x27.md)
  * [ATtiny3226,1626,826,426](https://github.com/SpenceKonde/megaTinyCore/blob/master/megaavr/extras/ATtiny_x26.md)
  * [ATtiny3224,1624,824,424](https://github.com/SpenceKonde/megaTinyCore/blob/master/megaavr/extras/ATtiny_x24.md)
* tinyAVR 1-Series
  * [ATtiny3217,1617,817,417](https://github.com/SpenceKonde/megaTinyCore/blob/master/megaavr/extras/ATtiny_x17.md)
  * [ATtiny3216,1616,816,416](https://github.com/SpenceKonde/megaTinyCore/blob/master/megaavr/extras/ATtiny_x16.md)
  * [ATtiny1614,814,414,214](https://github.com/SpenceKonde/megaTinyCore/blob/master/megaavr/extras/ATtiny_x14.md)
  * [ATtiny412,212](https://github.com/SpenceKonde/megaTinyCore/blob/master/megaavr/extras/ATtiny_x12.md)
* tinyAVR 0-Series
  * [ATtiny1607,807](https://github.com/SpenceKonde/megaTinyCore/blob/master/megaavr/extras/ATtiny_x07.md)
  * [ATtiny1606,806,406](https://github.com/SpenceKonde/megaTinyCore/blob/master/megaavr/extras/ATtiny_x06.md)
  * [ATtiny1604,804,404,204](https://github.com/SpenceKonde/megaTinyCore/blob/master/megaavr/extras/ATtiny_x04.md)
  * [ATtiny402,202](https://github.com/SpenceKonde/megaTinyCore/blob/master/megaavr/extras/ATtiny_x02.md)

## Parts not supported by megaTinyCore
* Anything named like "AVR##XX##" where X is a letter and # is a number - you want my [DxCore](https://github.com/SpenceKonde/DxCore) for those
* All of the classic (pre-2016) tinyAVR parts - these are almost all supported by one of my other cores [ATTinyCore](https://github.com/SpenceKonde/ATTinyCore)
  * ATtiny 25/45/85, 24/44/84, 261/461/861, 48/88, the two small and ones (strange 43 and 4313/2313), and in 2.0.0, the 26 as well as the final-four (which show hints of experimentation in the direction of the modern AVRs), the ATtiny 441/841, 1634 and 828 plus the even stranger 26.
* Anything else [**See this document for a list of AVR part families and what arduino cores they work with**](https://github.com/SpenceKonde/AVR-Guidance/blob/master/AVRFamilies_And_Compatibility/ArduinoCores.md) - almost everything has a core that offers support, usually by myself or MCUdude.

## Overall Part Comparison
[**See this document covering all modern AVRs**](https://github.com/SpenceKonde/AVR-Guidance/blob/master/AVRFamilies_And_Compatibility/ModernAVRs.md)

## Background and specific hardware features

## A Word on Terminology ("megaAVR")
In the official Arduino board definition for their "megaavr" hardware package, they imply that the new architecture on the megaAVR 0-Series parts (which is nearly the same as used on the tinyAVR 0-Series and 1-Series) is called "megaavr" - that is not an official term. Microchip uses the term "megaAVR" to refer to any "ATmega" part, whether it has the old style or modern peripherals. There are no official terms to refer to all AVR parts of one family or the other, and a Microchip employee even denied that there was such a term internally. I'm not sure how you can manufacture two sets of parts, with the parts in each set having so much in common with each other and so little in common with the other set, with nobody coining a phrase to refer to either of them.

In this document, prior to 2.0.2, we used the Arduino convention, and despite well over a year having passed since then, I still keep finding places where I call them megaAVR. Please report this using a github issue if you see any. Do note that the terms `avr` and `megaavr` are still used internally (for example, in libraries, to mark which parts a given library is compatible with, or separate different versions of a file based on what they will run on). This will continue - we have to stick with this for compatibility with what the Arduino team started with the core for the Uno WiFi Rev. 2 and Nano Every.

In any event, *some word* is needed to refer to the two groups and Microchip hasn't provided one. In the absence of an official term, I have been referring the pre-2016 AVR devices (with PORTx, DDRx, etc registers for pins) as "**classic AVR**" and the ones Arduino calls megaavr as "**modern AVR**". There also exist some parts whose I/O modules are largely more like classic AVRs but which also have a significantly worse version of the instruction set, and typical flash sizes of 1k or less. These use the AVRrc (for reduced core) variant of AVR, whereas most classic AVRs use AVRe or AVRe+, and modern AVRs use AVRxt. The AVRrc parts are not supported by this core, and on the unfortunate occasion that I need to discuss these profoundly disappointing parts, I will refer to them as "**Reduced Core AVR**" parts.

### 2-series Basics and Comparison With 0/1-Series
[Datasheet for the new tinyAVR 2-Series](http://ww1.microchip.com/downloads/en/DeviceDoc/ATtiny1624-26-27-DataSheet-DS40002234A.pdf) - While the datasheet only "covers" the 16k parts, they clearly state that there are no differences in features between parts with the same pin count (that is, there are no "golden" parts like the 16k/32k 1-Series), only between parts with different pin counts, and only as dictated by the pincount (that is, a feature on the 24 pin part will be on the 14-pin one, unless the 14-pin one doesn't have the pins that it needs, and it's something that can't be used without pins). 14, 20, and 24 pin parts are all listed with 4k, 8k, 16k and 32k flash; these flash size options, respectively, come with 512, 1024, 2048, and 3072 bytes of SRAM (that is, the 4k and 8k parts have double the SRAM), 4/8k parts get 128 bytes of EEPROM, the larger ones get 256. 14-pin parts come in SOIC and TSSOP, 20-pin in (wide) SOIC, SSOP, and that itty-bitty QFN like the 1616 (this time they gave us the 32k part in that package too, but good luck getting one, it's backordered everywhere - I couldn't score a single one) and 24-pin in the same VQFN as the 3217.

TWI, SPI, USART0, AC0, are unchanged, as is NVMCTRL (the changes required to the bootloader were solely in relation to supporting the second USART). Clock options unchanged. TCB0 and TCB1 got upgraded to the version in the Dx-Series: clock off event option, cascade, and separate INTCTRL bits for OVF and CAPT - nice additions, but nothing relevant to the core itself), and all the parts have both TCB's. We now get 4 CCL LUTs and 2 sequencers, instead of 2 and 1 - and they can fire interrupts like other parts with CCL (and unlike the tinyAVR 0/1-Series). One of the most exciting features is that, as expected, they have a second USART (that noise you hear is the ATtiny841 and and ATtiny1634 sobbing in the corner). PORTMUX registers now named like the rest of the modern AVRs - but we didn't lose the individual control over the pins for each TCA WO channel. EVSYS now works like it does on non-tinyAVR-0/1-Series parts (which is a welcome change - the 0/1-Series was the odd-one-out, and some of the ways in which their EVSYS was different sucked). The 1-Series features of TCD0, AC1/2, DAC0, and ADC1 are *gone*. In their stead, ADC0 is much fancier and almost unrecognizable, the first new AVR released since the buyout that featured a real differential ADC. (queue another agonized wail from the poor '841, which also has an incredibly fancy ADC with great differential options, but which looks thoroughly dated next to the new ones)... judging by the volume of posts on different topics that I've seem, I have a sense that differential ADC wasn't at the top of most of your wish-lists - but it was on the top of the major chip customers' lists, and so that's what we're getting. And it was nigh time we got a proper differential ADC instead of the one on the Dx-series. And it is really *really* fancy. See below.

megaTinyCore provides an analogRead() implementation, and more powerful functions to use the oversampling and PGA (see the analog feature section below).

Oh, and one more thing... the UPDI pin configuration has the old options - UPDI, I/O, or Reset... and a new one: UPDI on PA0, with hardware RESET pin on PB4! Optiboot will finally be a viable and comfortable option at least on the parts that have a PB4, ie, not the 14-pin parts. Which also happen to be (if my Tindie store sales are any indication) the most popular kind.

### Dark Times Likely Loom Ahead for tinyAVR as a Product Line
Do you think there will be a 3 series? I do not. DD and the EA's are clearly coming after them and taking up strategic positions around tinyAVR territory. I think it's only a matter of time before the brand is eliminated like they did megaavr after the megaAVR 0-series. This is not necessarily a bad thing: All the Dx and EA series parts are very similar in pin mappings and and behavior. The tinies are less consistent. Most irritating of all, the pin numbering is whack on the tinyAVRs: It starts off in order, then PORTB is in reverse order, then PORTC back to normal? Give me a break! And this bit about the UPDI being on PA0? Ugh! On everything else it's way down at PF7. Since tradition is to use pin 0 for the first pin, and have the last number be the pin that you can't use without setting a fuse that makes the chip hard to program, that was what led to the less optimal pin numbering we got. I'd have much rathered be able to number them counterclockwise starting with A0 without breaking unwritten conventions of Arduino code.

I predict, that in 2-4 years time, there's an AVR DA, DB, DD. DU (the USB one), EA, and D/E/F-series parts down to pincounts of 8 and 64-pin parts with 128k flash and the new ADC. And nothing else branded ATtiny (another big mystery is if they're ever going to replace the ATmega2560 with a modern AVR with 100 total pins (probably 80-88 of which are I/O); That would present two issues - first, past 56 I/O pins there are no more VPORT registers left - the low I/O space is full with 28 VPORT and 4 GPIORs. How will they handle the 4 extra ports? (on the 2560, they were just second class ports that were accessed more slowly and didn't have single cycle access. [I have some musings about it and the feasibility with how few opcodes are available in appendix A here.](https://github.com/SpenceKonde/AVR-Guidance/blob/master/LowLevel/ReadingAVRHex.md). Then again if they're still selling those ancient things for over ten bucks, why would they want to change that?!

## Buying tinyAVR 1-Series and 2-Series Breakout Boards
I sell breakout boards with regulator, UPDI header, and Serial header in my tindie shop, as well as the bare boards. Buying from my store helps support further development on the core, and is a great way to get started using these exciting new parts with Arduino. Currently ATtiny1624 boards are available, but the 20 and 24-pin parts will not be sold as an assembled board until a newly revised PCB design is back from the board house to enable autoreset on the alt-reset pin. There is also a 14-pin board revision coming - thought it is largely cosmetic. The yellow solder mask has got to go, as the readability seemed to get worse in the last several batches. The new boards also standardize a 0.6" spacing between the rows of pins, instead of the current 0.7" spacing, so you will be able to, for example, put machined pin header onto them and plug them into a wide-DIP socket, or use them with our prototyping board optimized for that row spacing. Assembled 0-Series boards are being discontinued, and will not be restocked once they sell out. The same will happen for the 16k 2-Series parts once the 32k ones are available.

### [ATtiny3227, 3217, 1607 (while supplies last) assembled](https://www.tindie.com/products/17523/)
### [ATtiny3217/1617/817/417/1607/807 bare board](https://www.tindie.com/products/17613/)
### [ATtiny3226, 1626, 3216, 1606 (while supplies last) assembled](https://www.tindie.com/products/17597/)
### [ATtiny3216/1616/816/416/1606/806/406 bare board](https://www.tindie.com/products/17614/)
### [ATtiny3224, 1624, 1614, and 1604 (while supplies last) assembled](https://www.tindie.com/products/17598/)
### [ATtiny1614/814/414/1604/804/404 bare board](https://www.tindie.com/products/17748/)
### [ATtiny412, 402 assembled](https://www.tindie.com/products/17685/)
### [ATtiny412/212/402/202 bare board](https://www.tindie.com/products/17749/)


## Notable Hardware Features

### 2-Series ADC
The ADC on the 2-Series is arguably the best ADC that has shipped on an AVR to date (not counting the xMega line, which is outside the scope of this document and the concerns of the Arduino world). Certainly, outside of the xMega line there is little meaningful competition. The closest comparisons are the classic AVRs that got differential ADCs with top-notch features (the t841, mega2560 and (surprisingly) the t861 being the strongest competitors). While it isn't capable of the insane 100x and 200x gain that some parts bragged of in the classic AVR days, it was never clear to me how much of what was being amplified was simply noise (though considering my admittedly limited experience playing with differential ADCs I'm going to say "probably most of it"). This new ADC is certainly highly capable, with true differential capability (unlike the DA and DB series had), and one which rises head and shoulders above anything available on any other modern AVRs to date. The programmable gain amplifier is a new capability, and it remains to be seen what sort of feats of analog measurement people are able to get out of it; it certainly appears promising. It will be especially interesting to understand the differences between using the PGA at 1x gain, vs not using the PGA, and the benefits and disadvantages of doing so. (Microchip would be well-served by a document that discussed how to choose the right ADC configuration for a task in the general case; I have raised this concern with Microchip and the person who I spoke to indicated that it was a high priority; while the situation has been greatly improved, it still appears that the doc group was specifically instructed not to make any actual concrete recommendations for when to use the PGA at unity gain, ).

The addition of 1024-sample accumulation for the purposes of oversampling and decimation is a welcome addition, though one which also risks underestimating the magnitude and relevance of offset error. (Taking 1024 samples, (all of which have a given offset error), then decimating the sum to yield a 17-bit ADC measurement makes it easy to imagine that any error would be confined to the lowest couple of bits. But if the error was, say 5 lsb on a single measurement, when you accumulate 1024 samples and decimate, you have an offset error of 160 it is extremely easy to see that and think it's signal not noise.

It would appear that the first full sized chip featuring this ADC will be the EA-Series parts, which will be available in 28-48 pin packages with up to 64k flash, according to the available information from the product brief. Considering that each sub-generation of modern AVRs has featured tweaks to the ADC (which are mostly papered over by the Arduino API), it will be very interesting to see what surprises, if any, are in store for us with the EA-Series. The announced information leaves little room for surprises though, and the 2-Series may have been more of a trial run (like the 1-Series with the TCD and triple-AC with window comparator mode...) looking at the headers and .atdf's for the EA's, which have recently become available, it looks like they removed a couple of the mysterious "knobs" that configured parts of the ADC conversion process that I never understood, and which Microchip didn't really explain when I asked about them.

### Type D Timer - 1-Series Only
The type D timer is only used for PWM on 20/24 pin 1-Series parts. On the smaller parts, it wouldn't let us increase the total number of PWM pins. Only the WOC and WOD pins (on PC0 and PC1 respectively) don't already have TCA-driven PWM on them. As such, since analogWrite() does not support any features that would be enabled by turning off split mode (like 16-bit PWM) or enhanced by using the type D timer (like adjusting the frequency), it would just be worse, because it would require additional space to store the routine to turn on and off PWM from two types of timer, instead of one. This is not negligible on the smaller flash parts; it is on the order of 600 bytes. 150 for digitalWrite() and 450 for analogWrite() if those are ever called on a TCD PWM pin. The optimizer *should* be able to optimize away that portion of those functions in this case, as long the pins used with those functions do not include any TCD PWM pins. Note the optimizer will consider them independently, that is, digitalWrite() will include the code to turn off TCD PWM if it is used with a pin that uses TCD for PWM, whether or not you ever call analogWrite() on that pin.

### 1-Series Parts With 16k or 32k of Flash
Unlike almost every other AVR ever, there are additional "bonus" features based on the flash-size of parts within a family. The 16k and 32k versions (only) have a few extra features (which also don't appear to have been considered for pricing) - they all have 2k of ram, whether 16k or 32k, they have 3 analog comparators (including a window mode), a second - desperately needed - type B timer - and weirdest of all they have a second ADC, differing only in which pins the channels correspond to!

## Memory-mapped Flash: No Need to Declare PROGMEM
Unlike classic AVRs, on the these parts, *the flash is mapped to the same address space as the rest of the memory*. This means `pgm_read_*_near()` is not needed to read directly from flash. Because of this, the compiler automatically puts any variable declared `const` into PROGMEM, and accesses it appropriately - you no longer need to explicitly declare them as PROGMEM. This includes quoted string literals, so the F() macro is no longer needed either, though to maintain compatibility with some third party libraries, f() still declares it's argument PROGMEM.

However, do note that if you explicitly declare a variable PROGMEM, you must still use the `pgm_read` functions to read it, just like on classic AVRs. When a variable is declared PROGMEM on parts with memory mapped flash, the pointer is offset (address is relative to start of flash, not start of address space); this same offset is applied when using the `pgm_read_*_near()` macros. Do note that declaring things PROGMEM and accessing with `pgm_read_*_near` functions, although it works fine, is slower and wastes a small amount of flash (compared to simply declaring the variables const); the same goes for the F() macro with constant strings in 2.1.0 and later (for a period of time before 2.1.0, `F()` did nothing - but that caused problems for third party libraries). The authors maintained that the problem was with the core, not the library, and my choice was to accept less efficiency, or deny my users access to popular libraries). Using the `F()` macro may be necessary for compatibility with some third party libraries (the specific cases that forced the return of `F()` upon us were not of that sort - we were actually able to make the ones I knew of work with the F()-as-noop code, and they took up a few bytes less flash as a result).

### Automotive (VAO) Versions
The automotive versions should also work. You must always select the 16 MHz-derived clock speeds on these parts. They do not support 20 MHz operation.

## megaTinyCore - Features, options, and guidelines
Now on to the good part, where we get to talk about how all this is exposed by megaTinyCore. We will start with the matter of how you should refer to pins for best results, and then move on to core features, menu options, before ending with a series of links to documents with more detail on various subsystems.

## How to Refer to Pins
The simple matter of how to refer to a pin for analogRead() and digitalRead(), particularly on non-standard hardware, has been a persistent source of confusion among Arduino users. It's my opinion that much of the blame rests with the decisions made by the Arduino team (and author of Wiring before them) regarding how pins were to be referred to; the designation of some pins as "analog pins" leads people to think that those pins cannot be used for digital operations (they are better thought of as "pins with analog input" - like how there are "pins that can output PWM"). The fact that pins have traditionally been renumbered has further muddied the water. For non-standard classic AVR parts, matters are often made even worse by multiple, incompatible "pin mappings" created by various authors over the years to make the part act "more like an Uno" or for some other purpose (ATTinyCore is a particular mess in this way, with some parts having three entirely different pin mappings, in at least one case, one of the alternate mappings is a devil-inspired work of pure evil, requiring nothing short of an additional lookup table to convert analog pins to digital pins).

This core uses a simple scheme for assigning the Arduino pin numbers: Pins are numbered starting from the the I/O pin closest to Vcc as pin 0 and proceeding counterclockwise, skipping the (mostly) non-usable UPDI pin. The UPDI pin is then assigned to the last pin number (as noted above, it is possible to read the UPDI pin (both analog and digital reads work) even if it is not set as GPIO). We recommend this as a last resort: the UPDI pin always has its pullup enabled when not set as a GPIO pin, and a signal which looks too much like the UPDI enable sequence will cause undesired operation.

In order to prevent all confusion about pin identities and eliminate ambiguity, we recommend using the PIN_Pxn notation to refer to pins unless you are using a development board with different numbers or names for the pins printed on it. This will maximize portability of your code to other similar hardware and make it easier to look up information on the pins you are using in the relevant datasheets, should that be necessary.

### PIN_Pxn Port Pin Numbers (recommended)
**This is the recommended way to refer to pins** `#defines` are also provided of form `PIN_Pxn`, where `x` is A, B, or C, and `n` is a number 0-7 - (Not to be confused with the PIN_An defines described below). These just resolve to the digital pin number of the pin in question - they don't go through a different code path or anything. However, they have particular utility in writing code that works across the product line with peripherals that are linked to certain pins (by Port), as most peripherals are. Several pieces of demo code in the documentation take advantage of this. Direct port manipulation is possible as well - and in fact several powerful additional options are available for it - see [**direct port manipulation**](https://github.com/SpenceKonde/megaTinyCore/blob/master/megaavr/extras/Ref_DirectPortManipulation.md).

**`PIN_Pxn` - not `Pxn`, and not `PIN_xn` - those mean different things!**

### Arduino Pin Numbers
When a single number is used to refer to a pin - in the documentation, or in your code - it is always the "Arduino pin number". These are the pin numbers shown in orange (for pins capable of analogRead()) and blue (for pins that are not) on the pinout charts. All of the other ways of referring to pins are #defined to the corresponding Arduino pin number.

### An and PIN_An Constants
The core also provides `An` and `PIN_An` constants (where `n` is a number from 0 to 11). As with the official core, `PIN_An` is defined as the digital pin number of the pin shared with analog channel n These refer to the ADC0 *channel* numbers. This naming system is similar to what was used on many classic AVR cores **but here, they are just #defined as the corresponding Arduino pin number**. If you need to get the analog channel number on a digital pin, use the `digitalPinToAnalogInput(pin)` macro - but you only need that if you're writing an advanced ADC library.

## megaTinyCore and tinyAVR 0/1/2-Series Features
These parts (well, the 1/2-Series at least - the 0-Series is more of a budget option) provide an excellent toolbox of versatile and powerful peripherals; the top-end ones are on a par with or better than classic megaAVR parts - for a tinyAVR price. One of the guiding principles of the design of megaTinyCore, as with my other cores, is to allow the supported parts to reach their full potential - or as close to that as possible within the limitations of Arduino. This (very large) section covers the features of these parts and how they are exposed by megaTinyCore, as well as features of the core itself. This (very large) section attempts to cover each of the feature areas. Do try to find the feature you are working with if you're trying to use some chip feature and having trouble!

### Supported Clock Speeds
* 20 MHz Internal (4.5v-5.5v - typical for 5v systems)
* 16 MHz Internal (4.5v-5.5v - typical for 5v systems)
* 10 MHz Internal (2.7v-5.5v - typical for 3.3v systems)
* 8  MHz Internal (2.7v-5.5v - typical for 3.3v systems)
* 5  MHz Internal (1.8v-5.5v)
* 4  MHz Internal (1.8v-5.5v)
* 2  MHz Internal (1.8v-5.5v)
* 1  MHz Internal (1.8v-5.5v)
* 20 MHz External Clock (4.5v-5.5v)
* 16 MHz External Clock (4.5v-5.5v)
* 12 MHz External Clock (2.7v-5.5v)
* 10 MHz External Clock (2.7v-5.5v)
* 8  MHz External Clock (2.7v-5.5v)
* 6  MHz Internal (tuned)
* 5  MHz Internal (tuned)
* 4  MHz Internal (tuned)
* 2  MHz Internal (tuned)
* 1  MHz Internal (tuned)
* 7  MHz Internal (tuned, for masochists)
* 8  MHz Internal (tuned)
* 10 MHz Internal (tuned)
* 12 MHz Internal (tuned)
* 16 MHz Internal (tuned)
* 20 MHz Internal (tuned)
* 24 MHz Internal (tuned, overclocked)
* 25 MHz Internal (tuned, overclocked)
* 30 MHz Internal (tuned, overclocked) - 0/1-Series require "20MHz" OSCCFG fuse setting; 2-Series parts may or may not be able to reach 30 with "16 MHz" selected.
* 32 MHz Internal (tuned, overclocked) - 2-Series only, very optimistic overclocking, may be unstable.
* 24 MHz External clock (Overclocked)
* 25 MHz External clock (Overclocked)
* 30 MHz External clock (Overclocked aggressively)
* 32 MHz External clock (Overclocked aggressively)

**We make no claims about voltage or temperature ranges for overclocked parts - all we claim is that at least one of chips we have worked at that speed** at room temperature and 5v.

**Important - Read about [Tuning](https://github.com/SpenceKonde/megaTinyCore/blob/master/megaavr/extras/Ref_Tuning.md) before selecting any tuned option!**

More information on these clock speeds can be found in the [**Clock Reference**](https://github.com/SpenceKonde/megaTinyCore/blob/master/megaavr/extras/Ref_Clocks.md)

Voltages shown are those guaranteed to work by manufacturer specifications. Unless pushing the bounds of the operating temperature range, these parts will typically do far better (2-Series generally work at 32 MHz and 5v @ room temperature even from internal oscillator; the 0/1-Series will likewise usually work at 32 MHz with external clock provided the power supply is a stable 5.0-5.5V).

No action is required to set the `OSCCFG` fuse when the sketch is uploaded via UPDI. When uploaded through Optiboot, the fuse cannot be changed, so whatever was chosen when the bootloader was burned is what is used, and only "burn bootloader" or uploading a sketch via UPDI will change that.

All internal oscillator clock speed options use the factory default calibration unless a "tuned" option is selected, in which case the calibration is adjusted as documented in the [**Tuning Reference**](https://github.com/SpenceKonde/megaTinyCore/blob/master/megaavr/extras/Ref_Tuning.md). This can be used to get 16 MHz operation on an optiboot chip fused for 20 MHz and vice versa.

See [**Speed Grade reference**](https://github.com/SpenceKonde/megaTinyCore/blob/master/megaavr/extras/Ref_SpeedGrades.md) for more information on the manufacturer's speed grades. Note that those are the voltages and clock speeds at which it is guaranteed to work. These parts are intended to be suitable for use in applications where an unexpected glitch of some description could pose a hazard to persons or property (think cars, industrial equipment, airplanes, nuclear reactors - places where people could die if the part malfunctioned). Many hobby users will be far more relaxed about the potential for stability issues, with crashes being little more than a nuisance, and the extremes of the extended temperature range parts being far beyond what we would ever need. If your device is running (with conformal coating ofc) at the bottom of a pot of boiling water, I wouldn't expect it to overclock... *Our testing has found that the official speed grades are extremely conservative, and that is the basis of the options in the menu*.

**It has been established that the extended temperature parts overclock better** which makes sense. A part that is spec'ed to run at 20 MHz at 125C would be expected to have a better chance of running at 32 MHz at room temperature than one spec'ed only to run at 20 MHz only at 105C

## Support for Official Microchip Developmenmt Boards
As of version 2.4.0, we now provide an "Official Microchip Board" option. This doesn't do anything special other than defining `LED_BUILTIN` to be the pin that has the LED on that board, instead of A7, and defining a macro `PIN_BUTTON_BUILTIN` defined as the pin with the user button on it and making "upload" with the non-optiboot version always use the onboard programmer/debugger; tools -> programmer will be used only for "burn bootloader" and "upload using programmer". In the case of the ATtiny416 XPlained Nano, it also selects the version of the bootloader that uses the alternate pins for the serial port - it does not automatically use the alternate pins for USART0 as if you'd done Serial.swap(1) yet - functionality to support default swapping of serial pins will come in a future update, alongside some other changes in the machinery underlying the pinswap mechanism which will hopefully also reduce flash usage.

As noted above, these may not work correctly on 32-bit Linux platforms. This is beyond my control; I don't build avrdude binaries amd I am *not* taking on that task too. I have too many already.

### Why Does `blink()` Take More Flash on the XPlained Mini vs the XPlained Pro?
Both have the same ATtiny817! How can they be different?

For the same reason that blink will take more flash if you change it to use `PIN_PC0` as opposed to `PIN_PB4`: PC0, used on the XPlained Mini is a PWM pin, while PB4, used by the XPlained Pro is not. Since that is the only pin that digitalWrite() is being used on, the compiler is free to optimize away anything that isn't needed for digitalWrite() on that pin, including the functionality to turn off PWM output on a pin that supports PWM. The difference vanishes if digitalWrite() is also used on a pin that supports PWM on both devices (resulting in the higher flash use result) or if digitalWrite() is replaced with digitalWriteFast(), which will use less flash (but assumes you won't call it on a pin outputting PWM).

### Auto-set Safe Fuses on UPDI Upload
Whenever a UPDI programmer is used to upload code, all fuses that can be set "safely" (as in, without risk of bricking the board, or bricking the board if one does not have access to an HV programmer), and which have any built-in configuration options, will be set. Thus, except where noted, behavior will always match the selected tools menu. In summary, these are handled as follows:
```text
WDTCFG will not be changed - it is not configured by megaTinyCore except to reset it to the factory default when doing "burn bootloader".
BODCFG will not be changed - not safe, you could set the BOD level to 4.3 on a 3.3v system, and then it would need to get > 4.3v applied to reprogram it. If it is on the same circuit board as parts that would be damaged, this is a difficult situation to recover from.
OSCCFG will be set
TCD0CFG will not be changed - it is not configured by megaTinyCore except to reset it to the factory default when doing "burn bootloader".
SYSCFG0 will not be changed - not safe
SYSCFG1 will be set
APPEND will not be changed - it is not configured by megaTinyCore. There is insufficient demand to justify the development effort.to make use of this as DxCore does
BOOTEND will be set
LOCKBIT will not be changed - it is not configured by megaTinyCore; supporting the lockbits presents several additional complications, and commercial users with need of this facility are unlikely to be using the Arduino IDE to program production units.
```
`BODCFG` is not safe, because setting this to a higher voltage than board is running at and enabling it will "brick" the board until a higher operating voltage can be supplied; this could be particularly awkward if it is soldered to the same PCB as devices which will not tolerate those voltages.

`SYSCFG0` is not safe because this is where `RSTPINCFG` lives; changing this can leave the board unprogrammable except via HV UPDI programming, and not everyone has an HV UPDI programmer. In the future if/when a programmer that guarantees HV UPDI capability which can be selected as a programmer (ie, it becomes possible to make a tools -> programmer option which will only work with HV programmers) this fuse will be set automatically when using that programmer.

As a result **in 2.2.0 and later, you no longer need to 'burn bootloader' to switch between 16-MHz-derived and 20-MHz-derived speeds when uploading using UPDI**

### Link-time Optimization (LTO) Support
This core *always* uses Link Time Optimization to reduce flash usage - all versions of the compiler which support the tinyAVR 0/1/2-Series parts also support LTO, so there is no need to make it optional, as was done with ATTinyCore. This was a HUGE improvement in codesize when introduced, typically on the order of 5-20%!


## Exposed Hardware Features

### ADC Support
These parts all have a large number of analog inputs - DA and DB-series have up to 22 analog inputs, while the DD-series has analog input on every pin that is not used to drive the HF crystal (though the pins on PORTC are only supported when MVIO is turned off). They can be read with `analogRead()` like on a normal AVR, and we default to 10-bit resolution; you can change to the full 12-bit with `analogReadResolution()`, and use the enhanced analogRead functions to take automatically oversampled, decimated readings for higher resolution and to take differential measurements. There are 4 internal voltage references in 1.024, 2.048, 4.096 and 2.5V, plus support for external reference voltage (and Vdd of course). ADC readings are taken 3 times faster than an classic AVR, and that speed can be doubled again if what you are measuring is low impedance, or extend the sampling time by a factor greatly for reading very high impedance sources. This is detailed in the analog reference.

### DAC Support
The Dx-series parts have a 10-bit DAC which can generate a real analog voltage (note that this provides low current and can only be used as a voltage reference or control voltage, it cannot be used to power other devices). This generates voltages between 0 and the selected `VREF` (unlike the tinyAVR 1-series, this can be Vcc!). Set the DAC reference voltage via the DACR`eference()` function - pass it any of the ADC reference options listed under the ADC section above (including VDD!). Call `analogWrite()` on the DAC pin (PD6) to set the voltage to be output by the DAC (this uses it in 8-bit mode). To turn off the DAC output, call `digitalWrite()` or `turnOffPWM()` on that pin.

There may be additional options to configure the DAC on the EA-series.

See the [**ADC and DAC Reference**](https://github.com/SpenceKonde/megaTinyCore/blob/master/megaavr/extras/Ref_Analog.md) for the full details.

Using the `An` constants for analog pins is deprecated - the recommended practice is to just use the digital pin number, or better yet, use `PIN_Pxn` notation when calling `analogRead()`.

### Watchdog Timer, Software Reset
There are more options than on classic AVR for resetting, including if the code gets hung up somehow. The watchdog timer can only reset (use the RTC and PIT for timed interrupts).

See the [**Reset and Watchdog (WDT) Reference**](https://github.com/SpenceKonde/megaTinyCore/blob/master/megaavr/extras/Ref_Reset.md) and [The core-auxiliary library megaTinyCore](megaavr/libraries/megaTinyCore/README.md)

### Improved Digital I/O
This core adds a number of new features include fast digital I/O (1-14 clocks depending on what's known at compile time, and 2-28 bytes of flash (pin number must be known at compile time for the `________Fast()` functions, and for configuring all per-pin settings the hardware has with `pinConfigure()`.

See the [**Improved Digital I/O Reference**](https://github.com/SpenceKonde/megaTinyCore/blob/master/megaavr/extras/Ref_Digital.md).

### Serial (UART) Support
All of the 0/1-Series parts have a single hardware serial port (UART or USART); the 2-Series parts have two. It works exactly like the one on official Arduino boards except that there is no auto-reset, unless you've wired it up by fusing the UPDI pin as reset (requiring either HV-UPDI or the Optiboot bootloader to upload code), or set up an "ersatz reset pin" as described elsewhere in this document. See the pinout charts for the locations of the serial pins.

Prior to putting the part into a sleep mode, or otherwise disabling it's ability to transmit, be sure that it has finished sending the data in the buffer by calling `Serial.flush()`, otherwise the serial port will emit corrupted characters and/or fail to complete transmission of a message.

See the [**Serial Reference**](https://github.com/SpenceKonde/megaTinyCore/blob/master/megaavr/extras/Ref_Serial.md) for a full list of options. As of 2.5.0, almost every type of functionality that the serial hardware can do is supported, including RS485 mode, half-duplex (via LBME and ODME), and even synchronous and Master SPI mode, and 2.6.0 will add autobaud, even though it's not very useful.

### SPI Support
All of these parts have a single hardware SPI peripheral. It works like the one on official Arduino boards using the SPI.h library. See the pinout charts for the location of these pins. On 8-pin parts, the only option for the SS pin is PA0 (the UPDI/reset pin); this does not matter for the purposes of this core though, because, like the official library, this only operates as a master, and the SS pin is used only when potentially acting as a slave.

On all parts except the 14-pin parts, the SPI pins can be moved to an alternate location (note: On 8-pin parts, the SCK pin cannot be moved). This is configured using the `SPI.swap()` or `SPI.pins()` methods. Both of them achieve the same thing, but differ in how you specify the set of pins to use. This must be called **before** calling `SPI.begin()`.

`SPI.swap(1)` or `SPI.swap(0)` will set the the mapping to the alternate (`1`) or default (`0`) pins. It will return true if this is a valid option, and false if it is not (you don't need to check this, but it may be useful during development). If an invalid option is specified, it will be set to the default one.

`SPI.pins(MOSI pin, MISO pin, SCK pin, SS pin);` - this will set the mapping to whichever mapping has the specified pins. If this is not a valid mapping option, it will return false and set the mapping to the default. This uses more flash than `SPI.swap()` so that method is preferred. The `SS pin` argument is optional, as the pin is not used when acting as an SPI master, and neither this library nor the official SPI.h library support acting as a slave.

When it can be determined that arguments passed to `SPI.swap()` or `SPI.pins()` are invalid at compile time (most commonly when the argument(s) are constants, which they almost always are), the core will generate a compile error to that effect. This is meant to help prevent such detectable problems from requiring debugging time on hardware.

This core disables the SS pin, meaning the "SS" pin can be used for whatever purpose you want, and the pin is relevant only when making an SPI slave (which requires you to implement the interaction with the SPI peripheral yourself - though it's not rocket science or anything). On the classic AVRs, if SS was an input and SPI was enabled, it was acting as the SS pin, and if it went low, it would switch the device to slave mode (and SPI.h would not function until put back into master mode, which was not done automatically).

### I2C (TWI) Support
All of these parts have a single hardware I2C (TWI) peripheral. It presents an API compatible with the standard Arduino implementation, but with added support for multiple slave addresses, answering general call addresses and - most excitingly - simultaneous master and slave operation! (new in 2.5.0).

See **[Wire.h documentation](https://github.com/SpenceKonde/megaTinyCore/blob/master/megaavr/libraries/Wire/README.md)** for full description and details. The hardware I2C is one of the more complicated peripherals. Wire has had a lot of hot new enhancements recently check it out.




### PWM Support
The core provides hardware PWM via the standard `analogWrite()` function. On the 8-pin parts (412, 212, 402, 204), 4 PWM pins are available. On all other parts except 1-Series parts with 20 or 24 pins, 6 PWM pins are available, all driven by Timer A (TCA0). The 20 and 24 pin 1-Series parts have two additional pins, driven by TCD0. The 2-Series apparently traded TCD0 for a second serial port and a super-fancy ADC - those parts also all have 6 PWM pins. The Type B (TCBn) timers cannot be used for additional PWM pins - their output pins are the same as those available with Timer A and they are often too useful to justify using a whole TCB for. However, you can take them over if you need to generate PWM at different frequencies, though the fact that the prescaler cannot differ from the type A timer limits this use as well. See the pinout charts for a list of which pins support PWM.

**Note that TCA0 (the type A timer) on all parts is configured by the core at startup to operate in split mode in order to support the most PWM pins possible with `analogWrite()`. As of the 2.2.x versions, a `takeOverTCA0()` function has been added, which can be called to instruct the core not write to TCA0-registers nor assume any particular mode or behavior for TCA0. `analogWrite()` will not generate PWM except on pins driven by TCD0 on the 20/24-pin parts nor will `digitalWrite()` turn it off if you want to reconfigure TCA0 for other purposes, please refer to the below guide and "hard reset" the timer back to stock configuration.**
#### [**Taking over TCA0**](https://github.com/SpenceKonde/megaTinyCore/blob/master/megaavr/extras/TakingOverTCA0.md)

#### PWM on TCD0 Pins (PIN_PC0 and PIN_PC1 - 10,11 on x16, 12,13 on x17)
The 3216, 1616, 816, 416, and the 3217, 1617 and 817 have two additional PWM pins driven by Timer D (PC0 and PC1 - pins 10 and 11 on x16, 12 and 13 on x17). Timer D is an asynchronous (async) timer, and the outputs can't be enabled or disabled without briefly stopping the timer. This results in a brief glitch on the other PWM pin (if it is currently outputting PWM) and doing so requires slightly longer - though the duration of this glitch is under 1 us. If TCD is used as the millis timer - which is the default on any part that has a type D timer (in order to keep the timers that are more readily repurposed available - TCD0 is not an easy peripheral to work with), this will result in `millis()` losing a very small amount of time (under 1 us) every time PWM is turned on or off on a TCD pin.

As of 2.2.0, *`analogWrite()` of 0 or 255 on a TCD-driven PWM pin does not disconnect the pin from the timer* - instead it results in a constant `HIGH` or `LOW` output without disconnecting the timer (use `digitalWrite()` for that). This means that `analogWrite(PIN_PC0, 0)` or `analogWrite(PIN_PC1, 0)` can be used to connect the timer to the pin without outputting PWM (yet) - doing this on both pins prior to setting any other duty cycles would allow one to ensure that no glitch of any sort occurs on the other TCD0 pin when the second pin is connected to it. *Only `digitalWrite()` or `turnOffPWM()` will disconnect the timer from the pin*. When outputting a `HIGH` in this way, the pin is "inverted"; this means that `digitalRead()` on it will return 0, not 1 (if you're `digitalRead()`'ing a pin, which you have set to output a constant `HIGH`, using `analogWrite()`, and it's one of those two pins, it will read `LOW`. However, if you are using `digitalRead()` on a pin that you've set to output a constant value, you may be doing something wrong in general.

Because TCD is async, and can run from the unprescaled internal oscillator, that means you can lower the system clock frequency without affecting the speed of the PWM. While there is a difference in PWM frequency between 16-MHz derived and 20-MHz derived clocks, there is no change in frequency for different system clock speeds for the TCD-controlled pins (the TCA-controlled pins will vary by a factor of two) The exception to this is when TCD0 is used as the millis/micros timing source at 1 MHz - running at full speed there resulted in spending an unreasonable fraction of runtime in the `millis()` ISR (tens of percent of the time).

TCD0 is used for `millis()`/`micros()` by default on parts that have it. Be aware that this does have a small flash penalty, so you can save flash by switching to use TCA or a TCB as the timer. That will also make `micros()` return faster. There is a shortage of timers on most of these parts, and I have not seen anyone talking about or posting code that reconfigures the TCD. Meanwhile everyone seems to be reconfiguring the TCA and many libraries need a TCB. These factors have been the impetus for making TCD0 the default for `millis()`/`micros()`: it is least likely to directly interfere.

On some versions of megaTinyCore prior to 2.2.0, PWM on the TCD0 pins was entirely broken.

**For general information on the available timers and how they are used PWM and other functions, consult the guide:**
This also covers the PWM frequencies that these timers will give you at various system clocks.
[**Timers and megaTinyCore**](https://github.com/SpenceKonde/megaTinyCore/blob/master/megaavr/extras/Ref_Timers.md)

### Tone Support
Support for `tone()` is provided on all parts using TCB0, unless TCB1 is present and TCB0 is set as `millis()` source. This is like the standard `tone()` function. Unlike on some classic AVRs, it does not support use of the hardware 'output compare' to generate tones; due to the very limited PWM capabilities and restricted prescaler selection for the TCB timers, this is not practical. See caveats below if using TCB0 or TCB1 for `millis()`/`micros()` settings. On parts with 8k or less flash, tones which specify a duration longer than 65536 ms and a high frequency (specifically where duration times frequency is >4.2 billion) are not supported. If specified at compile time it will generate a compile error and at runtime it will produce a shorter duration tone than expected. This is a trade-off to reduce flash usage. It was decided that requesting a tone over a minute long qualifies as highly abnormal usage of `tone()`. Several minutes continuous at the upper limit of human hearing would be required to trigger the issue, and it would need to be even longer as the frequency got lower. At that point, rather than counting the number of times the tone pin is toggled (like `tone()` with a duration specified does), it probably makes more sense to use durationless `tone()`, and turn it off with `noTone()`. Or, for the sake of everyone within earshot, maybe don't make your device emit a high pitched tone for minutes on end at all? On parts with 16k flash or more, a slightly less flash efficient implementation is used and the limit is significantly higher and very unlikely to be encountered.

`tone()` can only play a tone on one pin at a time. In theory you can play one tone per Type B timer, simultaneously, without anything more exotic than what `tone()` does now other than adding a capability to manage the multiple pins. It is my opinion that those belong in a library, not the core. See comments in `tone.cpp` for some thoughts if you want to implement something like that - I'd be happy to give more thoughts if you have questions.

### millis()/micros() Timekeeping Options
megaTinyCore provides the option to use any available timer on a part for the `millis()`/`micros()` timekeeping, controlled by a Tools submenu. It can be disabled entirely if needed to save flash, allow use of all timer interrupts or eliminate the periodic background interrupt. By default, TCD0 will be used by on parts that have one - otherwise TCA0 will be used (in versions prior to 1.1.9, TCA0 was used by default on parts that could output PWM with TCD0 on pins not available for TCA0 PWM). All timers available on the parts can be used: TCA0, TCD0 (on parts that have it), TCB0, TCB1 (where present) and the RTC. Many of these - particularly the non-default options, involve tradeoffs. In brief, TCA0 is a very versatile timer that users often want to reconfigure, TCD0 loses a small amount of time when PWM is turned on or off on the two TCD0 PWM pins (10,11 on 20-pin parts, 12,13 on 24-pin parts), TCB0 conflicts with `Servo` and `tone()` on parts that don't have TCB1, and when the RTC is used `micros()` is not available at all because the clock isn't fast enough. With these limitations in mind, the timer selection menu provides a way to move `millis()`/`micros()` to the timer most appropriate for your needs.

For more information, on the hardware timers of the supported parts, and how they are used by megaTinyCore's built-in functionality, see the [**Timers and megaTinyCore Reference**](https://github.com/SpenceKonde/megaTinyCore/blob/master/megaavr/extras/Ref_Timers.md).

2.3.0 fixed a long-standing (though surprisingly low impact) "time travel" bug.

#### RTC Timer for `millis()`
If the RTC is selected as the timer for `millis()` timekeeping, `micros()` will not be available. Additionally, this timer will be configured to run while in STANDBY sleep mode. This has two important consequences: First, it will keep time while in sleep. Secondly, every 64 seconds, the RTC overflow interrupt will fire, waking the chip - thus, if you are using the RTC for `millis()` and putting the part into sleep, you should declare a volatile global variable that you set in the ISR that is supposed to wake the part, eg `volatile boolean ShouldWakeUp=0;`, set it to 1 in the ISR, and when you put the ATtiny to sleep, have it check this immediately after waking, going back to sleep if it's not set, and clearing it if it is set, e.g.:

```cpp
void GoToSleep() {
    do {
        sleep_cpu();
    } while (!ShouldWakeUp)
    ShouldWakeUp=0;
}
```

This functionality will be made easier to use via ModernSleep when that library is available.

This board package also supports using an external 32.768khz crystal as the clock source for the RTC (not supported on 0-Series or 8-pin parts - not our fault, the hardware doesn't support it). If this is used, make sure that the crystal is connected between the TOSC1 and TOSC2 pins (these are the same as the TX and RX pins with the default pin mapping - very convenient right?), that nothing else is, that no excessively long wires or traces are connected to these pins, and that appropriate loading capacitors per crystal manufacturer datasheet are connected (and that it's not a full moon - I found the 32k crystal to be extremely uncooperative. To reduce power usage, they try to drive the crystal as weakly as they can get away with, which in turn makes it more susceptible to interference.

#### What About External Oscillator "32768 Hz"?
Yes, you can use an external oscillator for the RTC, at least on 1 and 2 series parts.
When it's an oscillator not a crystal, it can be fed to either TOSC0 or EXTCLK; better support for this will come in the future. Note that while TOSC0 won't let you run the RTC at widlly faster speeds. EXTCLK will.

### `printf()` Support for "printable" Class
Unlike the official board packages, but like many third party board packages, megaTinyCore includes the `printf()` method for the printable class (used for UART serial ports and most everything else with `print()` methods); this works like `sprintf()`, except that it outputs to the device in question; for example:
```cpp
Serial.printf("Milliseconds since start: %ld\n", millis());
```
Note that using this method will pull in just as much bloat as `sprintf()` and is subject to the same limitations as printf - by default, floating point values aren't printed. You can use this with all serial ports
You can choose to have a full `printf()` implementation from a Tools submenu if you want to print floating point numbers, at a cost of some additional flash.

#### **WARNING** `printf()` and Variants Thereof Have Many Pitfalls
There are a considerable number of ways to screw up with `printf()`. Some of the recent issues that have come up:
* Formatting specifiers have modifiers that they must be paired with depending on the datatype being printed, for all except one type. See the table of ones that I expect will work below (it was cribbed from [cplusplus.com/reference/cstdio/printf/](https://www.cplusplus.com/reference/cstdio/printf/), and then I chopped off all the rows that aren't applicable, which is most of them). Apparently many people are not fully aware (or at all aware) of how important this is - even when they think they know how to use printf(), and may have done so on previously (on a desktop OS, with 32-bit ints and no reason to use smaller datatypes for simple stuff).
* There are (as of 1.4.0) warnings enabled for format specifiers that don't match the the arguments, but you should not rely on them. Double check what you pass to `printf()` - `printf()` bugs are a common cause of software bugs in the real world. Be aware that *while you can use F() on the format string, there are no warnings for invalid format strings in that case*; a conservative programmer would first make the app work without F() around the format string, and only switch to F() once the format string was known working.

From cplusplus.com:
>The length sub-specifier modifies the length of the data type. This is a chart showing the types used to interpret the corresponding arguments with and without length specifier ~(if a different type is used, the proper type promotion or conversion is performed, if allowed)~:
Strikethrough mine 'cause that don't work here (and it's not my fault nor under my control - it's supplied with avrlibc, and I suspect that it's because the overhead of implementing it on an 8-bit AVR is too large). When incorrect length specifiers are given (including none when one should be used) surprising things happen. It looks to me like all the arguments get smushed together into a group of bytes. Then it reads the format string, and when it gets to a format specifier for an N byte datatype, it grabs N bytes from the argument array, formats them and prints them to whatever you're printing to, proceeding until the end of the format string. Thus, failing to match the format specifiers' length modifiers with the arguments will result in printing wrong data, for that substitution **and all subsequent ones** in that call to `printf()`.

The table below comprises the relevant lines from that table - many standard types are not a thing in Arduino.

| length | d i | u o x X | f F e E g G a A |  c  |    s   |  p   |    n     |
|--------|-----|---------|-----------------|-----|--------|------|----------|
| (none) |int16|  uint16 | float           | int |  char* |void* | int*     |
| hh     |int8 |  uint8  |                 |     |        |      | char*    |
| l      |int32|  uint32 |                 |     |        |      | int32_t* |

Notice that there is no line for 64 bit types in the table above; these are not supported (support for 64-bit types is pretty spotty, which is not surprising. Variables of that size are hard to work with on an 8-bit microcontroller with just 32 working registers). This applies to all versions of `printf()` - the capability is not supplied by avr-libc.

There are reports of memory corruption with printf, I suspect it is misunderstandign of above that is actually at hand here.

#### Selectable `printf()` Implementation
A Tools submenu lets you choose from three levels of `printf()`: full `printf()` with all features, the default one that drops float support to save 1k of flash, and the minimal one drops almost everything and for another 450 bytes flash saving (will be a big deal on the 16k and 8k parts. Less so on 128k ones). Note that selecting any non-default option here *will cause it to be included in the binary even if it's never called* - and if it's never called, it normally wouldn't be included. So an empty sketch will take more space with minimal `printf()` selected than with the default, while a sketch that uses `printf()` will take less space with minimal `printf()` vs default.

### Interrupts From Pins and in General
All pins can be used with `attachInterrupt()` and `detachInterrupt()`, on `RISING`, `FALLING`, `CHANGE`, or `LOW`. All pins can wake the chip from sleep on `CHANGE` or `LOW`. Pins marked as Async Interrupt pins on the megaTinyCore pinout charts (pins 2 and 6 within each port) can be used to wake from sleep on `RISING` and `FALLING` edges as well. Those pins are termed "fully asynchronous pins" in the datasheet.

Advanced users can instead set up interrupts manually, ignoring `attachInterrupt()`, manipulating the relevant port registers appropriately and defining the ISR with the `ISR()` macro - this will produce smaller code (using less flash and RAM) and the ISRs will run faster as they don't have to check whether an interrupt is enabled for every pin on the port.

For full information and example, see [the Interrupt Reference](https://github.com/SpenceKonde/megaTinyCore/blob/master/megaavr/extras/Ref_Interrupts.md).

### Assembler Listing generation
Like my other cores, Sketch -> Export compiled binary will generate an assembly listing in the sketch folder. A memory map is also created. The formatting of the memory map leaves something to be desired, and I've written a crude script to try to improve it, see the Export reference for more information.
see [**Exported Files documentation**](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/Ref_Export.md)

### EESAVE configuration option
The EESAVE fuse can be controlled via the Tools -> Save EEPROM menu. If this is set to "EEPROM retained", when the board is erased during programming, the EEPROM will not be erased. If this is set to "EEPROM not retained", uploading a new sketch will clear out the EEPROM memory. Note that this only applies when programming via UPDI - programming through the bootloader never touches the EEPROM. Burning the bootloader is not required to apply this change on DA and DB parts, as that fuse is "safe". It IS required on DD-series parts, because its on the same fuse that controls whether the UPDI pins is acting as UPDI or I/O

See the [Export Reference](https://github.com/SpenceKonde/megaTinyCore/blob/master/megaavr/extras/Ref_Export.md).


### Brown Out Detection (BOD) Configuration Options
These parts officially support BOD trigger levels of 1.8V, 2.6V, and 4.2V, with Disabled, Active, and Sampled operation options for when the chip is in ACTIVE and SLEEP modes - Disabled uses no extra power, Active uses the most, and Sampled is in the middle. As of 2.1.0, the ACTIVE/SLEEP modes have been combined into a single menu, the nonsensical options (such as using more aggressive BOD while sleeping than while awake) were removed, and the previously unexposed options were added. Sampled mode is now available with two sample rates (the faster one uses ever so slightly more power, as you would expect) and "Enabled hold wake": in that mode, BOD is disabled in sleep, enabled when not sleeping, and when waking up, code execution does not begin until the BOD is ready. See the datasheet for details on power consumption and the meaning of these options. You must do Burn Bootloader to apply this setting, even in 2.2.0 and later. It's not a "safe" setting, since if you set it to a voltage higher that the operating voltage and enable it, the chip is "bricked" until a high enough voltage is applied.

#### Additional BOD Levels
Between the initial header file and preliminary datasheet release, and the more recent versions of each, several BOD settings were removed from the tinyAVR 0/1-Series datasheets, and the atpack release notes described them as "unqualified" - (described as "unqualified" in the release notes accompanying this change - I understand that this has something to do with the vetting process for critical applications which these parts go through and do not correspond to a voltage above which the chip is guaranteed to function under certain conditions. The three official BOD levels are the voltages that the chip is guaranteed (Yup, they use that word in the datasheet!) to work at, within the manufacturer specified temperature range and running at a system clock frequency no higher than marked. It has been implied that ongoing tests of production devices are performed and it would appear that their policy is to test against the specification in production. Nevertheless, the other 5 BOD levels are anticipated to work as one would expect, but Microchip does not provide any *guarantee* that they'll work, even if all other operating requirements are met. These "not guaranteed" voltages are still supported by the megaTinyCore BOD dropdown menu, but (as of 2.0.4 - the first version that has the new headers) are marked as "(Unofficial)" in the submenu. Note that the new headers no longer provide the `*_gc` enum entries for these BOD level. I suspect that only the three "qualified" BOD levels are tested in production, though I have used the others without problems.

| BOD level | Guaranteed speed |
|-----------|------------------|
|     1.8V  |            5 MHz |
|     2.1V  |       unofficial |
|     2.6V  |           10 MHz |
|     2.9V  |       unofficial |
|     3.3V  |       unofficial |
|     3.7V  |       unofficial |
|     4.0V  |       unofficial |
|     4.2V  |           20 MHz |


## Included Libraries
Most existing Arduino libraries work. See the [Supported Libraries List](https://github.com/SpenceKonde/megaTinyCore/blob/master/megaavr/extras/LibraryCompatibility.md) for a more complete list and discussion of what kinds of libraries might have issues. Of the few libraries that don't work, a handful happened to also be extremely popular and heavily used, such that it was felt necessary to include a compatible version with megaTinyCore. In addition to these, libraries which expose hardware that is only present on the modern AVRs, are also included. These libraries are listed below.

### megaTinyCore
This library supplies two functions to check tuning status of the chip it's running on, and now adds two software reset functions (via WDT or via software reset). It also holds the massive keywords.txt file that highlights register names and core-specific functions.

[megaTinyCore helper library docs](https://github.com/SpenceKonde/megaTinyCore/blob/master/megaavr/libraries/megaTinyCore/README.md)

### tinyNeoPixel (WS2812)
The usual NeoPixel (WS2812) libraries, including the popular FastLED as well as AdafruitNeoPixel, have problems on these parts - they depend on hand-tuned assembly, but the execution time of several key instructions has been improved. The improvements enable significant simplification of the code for driving these LEDs. This core includes a compatible version of the tinyNeoPixel library for interfacing with these ubiquitous addressable LEDs. There are two versions, both tightly based on the Adafruit_NeoPixel library. One implements a truly identical API, differing only in name (and obviously the fact that it works on tinyAVR and Dx-Series and megaAVR 0-Series parts at clock speeds from 8 MHz to 48 MHz, instead of on most classic AVRs at 8, 12, and 16 MHz). The other version makes a slight change to the constructor and drops support for changing length at runtime, in order to realize significant flash savings (around 1k). See the [tinyNeoPixel documentation](https://github.com/SpenceKonde/megaTinyCore/blob/master/megaavr/extras/tinyNeoPixel.md) and included examples for more information.

### EEPROM and USERROW
The standard EEPROM.h is available here - it works like it does on any AVR. `USERSIG.h` (from "User Signature" which the datasheet has sometimes called the `USERROW`) it has the same API as EEPROM, though there may be future additions to harmonize with Dx-friendly functions for updating multiple bytes. The Dx-Series parts can only erase the whole USERROW, so potentially each byte written could involve erasing and rewriting it all - the question of how to deal with that is why DxCore doesn't have a USERSIG library yet). The name "USERSIG" refers to the alternate name of the USERROW, the "User Signature" space - the name USERROW could not be used because it is defined by the io headers (it's the `struct` of type `USERROW_t`, made up of `USERROW.USERROW0` through `USERROW.USERROW31`. Not the most useful thing, but we never override the io header file definitions unless working around a bug.

Note: Prior to 2.1.0, we tried to get clever with supporting the `USERROW` through the EEPROM library; that not only was shortsighted (as it's logically inconsistent on anything with more than 256b of EEPROM), it also introduced some serious bugs. Use the `USERSIG.h` library for that instead.

### Servo
The usual Servo library from library manager is incompatible with these parts (minor changes could make it "work", but with glaring issues and a dependence on the configuration of TCA0). This core provides a version of the Servo library which will select an appropriate timer (TCB0 is the only option on most parts, on parts with a TCB1 (2-Series and 3216, 3217, 1617, 1616 and 1614), TCB1 will be used instead, provided it's not being used for `millis()`). Except on parts with a TCB1, Tone cannot be used at the same time as the Servo library. Servo output is better at higher clock speed; when using servos, it is recommended to run at the highest frequency permitted by the operating voltage, to minimize jitter.

**Warning** If you have installed a version of the Servo library to your <sketchbook>/libraries folder (including via library manager), the IDE will use that version of the library (which is not compatible with these parts) instead of the one supplied with megaTinyCore (which is). As a workaround, a duplicate of the Servo library is included with a different name - to use it, just `#include <Servo_megaTinyCore.h>` instead of `#include <Servo.h>` - no other code changes are necessary.

Note that the Servo libraries were only fixed in version 2.2.0 - prior to that we had a Servo library, but it didn't work due to an astonishingly large number of bugs (I swear I tested it!).

### Flash
Written by @MCUDude, this provides a more accessible (much more accessible!) wrapper around the optiboot.h library (which was written by the famous @westfw) . This supports writing to the flash of any device using Optiboot, by having the application code call routines in the bootloader to write to the flash. All modern AVRs have built-in flash protection mechanisms that permit only code executing from the bootloader section (`BOOTCODE`, in their terminology) to write to the application section (`APPCODE`). While the hardware does support a third flash section (`APPDATA`) which can be written by code running in `APPCODE` this is only usable if there is also a `BOOTCODE` section defined (otherwise the entire flash is treated as `BOOTCODE` which can never be self-programmed), and would require a separate implementation of this library to use. It would also be possible to get flash-write-from-app without use of an actual bootloader through an analog of the trick used by the DxCore Flash.h for this. Since there appears to be little demand for such a thing, that functionality is not currently implemented (they were implemented on DxCore's flash writing library because the additional effort was virtually nil, and because there was a user with a particular interest in that feature). If someone wants this, and will adapt the library, I can add the entry point to the core and give you little chunks of inline assembly that will call it. Note on terminology: on AVR Dx-Series, the fuses are called `BOOTSIZE` and `CODESIZE` whereas on 0/1-Series tinyAVRs they're called `BOOTEND` and `APPEND`. I'm not quite sure how they didn't foresee customer confusion when they called the "APPlication END" that... Regardless of the names they do the same thing, although the granularity on tinyAVRs is finer, as you would expect.

[Optiboot_flasher documentation](https://github.com/SpenceKonde/megaTinyCore/blob/master/megaavr/libraries/Optiboot_flasher/README.md)

**Warning** As noted above, there is a library for DxCore that is also named `Flash.h`. Both allow an application to write to the flash using Optiboot if present. *That is the only similarity they have*. The API, NVM hardware, method used to call the bootloader, and basically *everything about these libraries is different*. Be sure you write code for the one that matches the hardware you're using. While I (Spence Konde) wrote the DxCore one, I don't have a particularly strong opinion about which way is "right". We made them independently, but not because we each thought the other one's idea of how it should be done was wrong. They largely reflect the way the hardware interacts with its flash. For example, the one for megaTinyCore is page-oriented with its own page buffer, and these parts write in a page-oriented manner, while the DxCore library only cares about pages when erasing - and indeed, on those parts, the flash is written with word or byte granularity.

### Logic (CCL or "LUT")
All of these parts have at least a pair of Configurable Custom Logic (CCL) blocks. Each block allows you to supply an arbitrary 3-input truth table, as well as configuring other aspects of the logic block. The CCL operates *asynchronously* - meaning that things can happen faster than the clock speed, though there is optional synchronizer that will synchronize the CCL output to an event input or the system clock. The inputs can come from pins, events, or other peripherals. There's a feedback input as well, which allows a great many exciting possibilities, and a "sequencer" that can act like a latch or flip-flop using the outputs of a pair of logic blocks as its inputs. This is an incredibly powerful peripheral - especially on the 2-Series parts, which have a second pair of logic blocks, as well as the capability to trigger an interrupt when the state of one changes.

The Logic (`#include Logic.h`) library provides a simple wrapper around the CCL hardware in the tinyAVR 0/1/2-Series devices. This library is also included in DxCore and MegaCoreX, covering all AVRs with CCL hardware. Written by @MCUDude.

[Logic library documentation](https://github.com/SpenceKonde/megaTinyCore/blob/master/megaavr/libraries/Logic/README.md)

Nomenclature note: Microchip often refers to the "logic blocks" as "LUTs" - "LUT" being an abbreviation for "Lookup table", which is a central feature of a logic block (it's a very small lookup table though - 3 binary inputs gives 8 possible entries / permutations in the lookup table, which are either 1 or 0, so 1 bit each. With only 8 rows, it's a bit smaller than than most things one would refer to as a table. We use the term "logic block" in the documentation (which includes the lookup table, its input multiplexers, and its synchronizer/filter and edge detector) unless we are specifically referring to the lookup table. This also helps to avoid any confusion with other types of lookup tables.

### Comparator (2.3.0+)
These parts have either 1 or 3 (1614, 1616, 1617, 3216, and 3217) on-chip analog comparators which can be used to compare two analog voltages, and, depending on which is larger, generate an event output, drive a pin high or low, or fire an interrupt. One of the voltages can be the internal reference (0-Series) an internal reference scaled by an 8-bit DAC (everything else). This library, written by @MCUDude, provides a simple wrapper around the analog comparator(s) which makes their configuration easier and resulting code more readable (also easier on the wrists - less stuff to type in all caps) than manually configuring registers, while exposing nearly the full featureset of the analog comparators on these parts. Do note does not support the Window Comparator option for the parts with 3 comparators; There doesn't exactly seem to be a lot of demand for that one, though.

The Comparator library (`#include Comparator.h`) is also included in DxCore and MegaCoreX, covering all modern AVRs with comparator hardware. Written by @MCUDude.

[Comparator library documentation](https://github.com/SpenceKonde/megaTinyCore/blob/master/megaavr/libraries/Comparator/README.md)

## Other Libraries (Not Included)
In general you should expect the following about library compatibility:
* Anything that works on an Uno WiFi Rev. 2 or Nano Every should work or require minimal effort to convert (if you run into one that doesn't work, please let us know in either discussions or issues, so we can look into getting it working correctly. I always want to know to add to the table linked below, but particularly if it works on those boards but not here - they are very similar architectures, and any porting effort required should be minimal. The most likely explanation is that the library was tested *specifically* for the ATmega4809, rather than generally for the peripherals it used.
* The library.properties field `architectures=*` would suggest that it would work anywhere - all this means is that there are not separate folders with implementations for different architectures. It does not mean that the library does not make assumptions about architecture, test against architecture specific stuff with `#ifdef`s and so on.
* Libraries that work on other AVR-based Arduino devices will work as long as:
  * They do not directly interact with hardware registers,
  * They do not make assumptions about what pins are associated with their peripheral (ex "Okay, now my library has included Wire.h, now I'm going to `attachInterrupt()` on SDA: `attachInterrupt(A4,myInterrupt)`;) - that fails except on an '328, because it assumes that SDA is on the same pin as analog input 4 - for example);
* Libraries that make use of only Arduino API calls without assumptions about the underlying hardware are virtually guaranteed to work.

The amount of effort required to port a given library will vary widely depending on the library. Some are almost trivial, while others require deep knowledge. The fact that many libraries can be ported without much effort again underlines the need for reports from users about incompatible libraries. Almost all of the fruits are low hanging and ripe for harvest.

### [List of Known Working / Not-working Libraries](https://github.com/SpenceKonde/megaTinyCore/blob/master/megaavr/extras/LibraryCompatibility.md)
I know lots of people use libraries that aren't on that list, and I fully expect that there is a great number of libraries that work and are not listed, and I'd live to hear about them. Use the "discussions" or email me, or even submit a PR to add a line to the table. And while I'm happy to hear about working libraries I'm even more interested if you have tried some and found that they *don't* work, so I can figure out why and how much effort would be required to make them work.


## WDT and Resets
For more information on resetting from software, using the Watchdog Timer, the causes of unexpected resets and how to prevent them, and generally all things reset-related, see the [Reset Guide](https://github.com/SpenceKonde/megaTinyCore/blob/master/megaavr/extras/Ref_Reset.md).

## Identifying Part and Options in Sketch
It is often useful to identify what options are selected on the menus from within the sketch; this is particularly useful for verifying that you have selected the options you wrote the sketch for when opened later by yourself or someone who you shared it with. Or, you can use such in-sketch identification, combined with preprocessor `#if` macros, to select the appropriate code depending on the part or options at hand.

There are a great number of `#define`s provided to get information about the hardware in-use, in order to write portable and flexible code in your sketch or, especially, library code.

**Note**: You cannot distinguish an extended temperature range part from a normal one from software. Be glad you're using a tinyAVR - the full size modern AVRs have *no means to determine the temperature rating without contacting Microchip support*

## Bootloader (Optiboot) Support
A new version of Optiboot (Optiboot_x) now runs on the tinyAVR 0-Series, 1-Series and 2-Series chips. It's under 512 bytes, and works on all parts supported by this core, allowing for a convenient workflow with the same serial connections used for both uploading code and debugging (like a normal Arduino Pro Mini). Note the exception about not having autoreset unless you disable UPDI (except for the 20 and 24-pin 2-Series parts which can put reset on PB4 instead), which is a bit of a bummer.

To use the serial bootloader, select a board definition with (optiboot) after it. Note - the optiboot suffix might be visually cut off due to the width of the menu; the second / lower set of board definitions in the board menu are the optiboot ones). The 2-Series Optiboot definitions and the 0/1-Series Optiboot definitions are separate entries in the board menu.

See the [Optiboot reference](https://github.com/SpenceKonde/megaTinyCore/blob/master/megaavr/extras/Ref_Reset.md)for more information.

## Guides
These guides cover subsystems of the core in much greater detail (some of it extraneous or excessive).
### Reference Material
#### [Function Reference](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/Ref_Functions.md)
Covering top-level functions and macros that are non-standard, or are standard but poorly documented, and which aren't covered anywhere else.
#### [Analog Input (ADC) and Output (DAC)](https://github.com/SpenceKonde/megaTinyCore/blob/master/megaavr/extras/Ref_Analog.md)
The API reference for the analog-related functionality that is included in this core beyond the standard Arduino API.
#### [Digital I/O and Enhanced Options](https://github.com/SpenceKonde/megaTinyCore/blob/master/megaavr/extras/Ref_Digital.md)
The API reference for the digital I/O-related functionality that is included in this core, beyond the standard Arduino API, as well as a few digital I/O-related features that exist in the hardware which we provide no wrapper around.
#### [Interrupts](https://github.com/SpenceKonde/megaTinyCore/blob/master/megaavr/extras/Ref_Interrupts.md)
Includes a list of all interrupt vectors that can be used, how the flags are cleared (not a substitute for the datasheet - just a very quick reminder), which parts each vector exists on, and and what parts of the core, if any, make use of a vector. It also has general guidance and warnings relating to interrupts their handling, including estimates of real-world interrupt response times.
#### [Serial](https://github.com/SpenceKonde/megaTinyCore/blob/master/megaavr/extras/Ref_Serial.md)
The USARTs (Serial) have some greatly enhanced functionality compared to the stock core.
#### [SerialUPDI](https://github.com/SpenceKonde/AVR-Guidance/blob/master/UPDI/jtag2updi.md)
Serial UPDI is our recommended tool for UPDI programming.
#### [Clock Information](https://github.com/SpenceKonde/megaTinyCore/blob/master/megaavr/extras/Ref_Clocks.md)
Supported clock sources and considerations for the use thereof.
#### [Speed Grades](https://github.com/SpenceKonde/megaTinyCore/blob/master/megaavr/extras/Ref_SpeedGrades.md)
Manufacturer specs for speed at various voltages, and some discussion of BOD thresholds - this is written largely from a very conservative perspective, in contrast to most of the documentation.
#### [Callbacks and Weakly Defined Functions](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/Ref_Callbacks.md)
These are provided by the core and can be overridden with code to run in the event of certain conditions, or at certain times in the startup process.
#### [Constants for Part/Feature/Pin/Version Identification](https://github.com/SpenceKonde/megaTinyCore/blob/master/megaavr/extras/Ref_Defines.md)
The core feature `#define`s are used by megaTinyCore and other cores I maintain as well. This also documents what constant values are defined by the core for version identification, testing for features, and dealing with compatibility problems.
#### [Assembly Listing and Memory Maps](https://github.com/SpenceKonde/megaTinyCore/blob/master/megaavr/extras/Ref_Export.md)
Export compiled binary generates both assembly listings and memory maps, in addition to the hex file. The options selected are encoded in the name of the file to help prevent confusion and make it easy to compare two configurations when you are surprised by the differences between them. Also provides links to a script I wrote to reformate memory maps so you can read the damned things.
#### [Reset Control and the WDT](https://github.com/SpenceKonde/megaTinyCore/blob/master/megaavr/extras/Ref_Reset.md)
The sources of reset, and how to handle reset cause flags to ensure clean resets and proper functioning in adverse events. **Must read for production systems**
#### [Optiboot Reference](https://github.com/SpenceKonde/megaTinyCore/blob/master/megaavr/extras/Ref_Reset.md)
The installation and operation of the Optiboot bootloader (for uploading over straight serial (not SerialUPDI)) is described here. Not recommended except on the 20/24-pin 2-Series (since they have the alt reset pin) or for special use cases that demand it.
### [Timers and megaTinyCore Reference](https://github.com/SpenceKonde/megaTinyCore/blob/master/megaavr/extras/Ref_Timers.md)
This contains detailed information on how the timers are used in megaTinyCore, and some background on their capabilities.
### Other Documents
These guides are older; some are still relevant.
### [Power Saving Techniques and Sleep](https://github.com/SpenceKonde/megaTinyCore/blob/master/megaavr/extras/PowerSave.md)
### [Taking Over the PWM Timer](https://github.com/SpenceKonde/megaTinyCore/blob/master/megaavr/extras/TakingOverTCA0.md)
This has been recently updated and will likely be turned into a Ref_TCA0.
### [Advanced ADC Features - For 0/1-Series Only](https://github.com/SpenceKonde/megaTinyCore/blob/master/megaavr/extras/ADCFreerunAndMore.md)
This document describes how (on the 0 and 1 Series only) the ADC can be taken over and reconfigured, with particular attention to free running mode. The 2-Series ADC is different, and it would require changes to reflect those differences.
### Official Microchip documents that are of notable value
#### [TB3262 - AVR1000b: Getting Started with Writing C-Code for AVR MCUs](https://ww1.microchip.com/downloads/en/Appnotes/AVR1000b-Getting-Started-Writing-C-Code-for-AVR-DS90003262B.pdf)
A delightful document on bare metal programming in C.
#### [AVR Instruction Set Manual](https://ww1.microchip.com/downloads/en/DeviceDoc/AVR-InstructionSet-Manual-DS40002198.pdf)
The bible of the AVR instruction set. Like any such tome, it is a very lengthy document which contains timeless wisdom from the creator(s) in obtuse and challenging language and a confusing syntax.If you are writing assembly, you probably have this document open most of the time.
#### See also the recently written ones listed in [the analog reference](https://github.com/SpenceKonde/megaTinyCore/blob/master/megaavr/extras/Ref_Analog.md)
As promised, a bunch of additional information was released; Unfortunately it leaves some of the key questions unanswered.
## List of Tools sub-menus
* Tools -> Chip - sets the specific part within a selected family to compile for and upload to.
* Tools -> Clock - sets the clock speed. You must burn bootloader after changing between 16/8/4/1MHz and 20/10/5MHz to apply the changes (ie, changing from 20MHz to 10MHz does not require burning bootloader, changing from 20MHz to 16MHz does). A virgin chip will be set to use the 20MHz internal oscillator as its base clock source, so 20/10/5MHz clock speed options should work without an initial "burn bootloader" - though we still recommend it to ensure that all other fuses are set to the values you expect.
* Tools -> Retain EEPROM - determines whether to save EEPROM when uploading a new sketch. You must burn bootloader after changing this to apply the changes. This option is not available on Optiboot board definitions - programming through the bootloader does not execute a chip erase function.
* Tools -> B.O.D. Voltage - If Brown Out Detection is enabled, when Vcc falls below this voltage, the chip will be held in reset. You must burn bootloader after changing this to apply the changes. See the **BOD configuration options** section above for more information.
* Tools -> B.O.D. Mode when Active/Sleeping - Determines the brown-out detection mode in active and sleep modes. You must burn bootloader after changing this to apply the changes. See the **BOD configuration options** section above for more information.
* Tools -> UPDI/Reset pin (this menu is not available for 8 or 14-pin, non-optiboot parts, as an incorrect selection will brick your board if you don't have an HV UPDI programmer. If you do, the lines that could be uncommented to enable it are in the boards.txt)
  * If set to UPDI, the pin will be left as the UPDI pin, there will be no hardware reset pin. If using Optiboot, to enter the bootloader, disconnect and reconnect power to the part or execute a software reset - either way, upload within 8 seconds.
  * If set to Reset, the pin will be configured to act as reset, like a classic AVR, but **UPDI programming will no longer work - you must use an HV programmer if you wish to reprogram via UPDI** - if the pin is set to reset, if using Optiboot, the version of the bootloader used will jump straight to the application after a power-on reset, and will only enter the bootloader if reset by software reset or via the reset pin. The bootloader will also only wait 1 second for a sketch (ie, it behaves like optiboot does on classic AVR microcontrollers), instead of 8. If not using a bootloader, further programming requires an HV programmer; this option is not available on non-optiboot boards without modifying boards.txt as noted above.
  * If it is set to I/O, the pin will act as a normal I/O pin with incredibly weak pin drivers - don't expect it to be able to source or sink more than 0.5mA when set output. If using Optiboot, the bootloader will behave in the same way as when the pin is left as UPDI. To reprogram using an HV programmer, you must use the Power Cycle High Voltage (PCHV) procedure. Consult the documentation for your HV programmer for more information. This option is not available on non-optiboot boards without modifying boards.txt as noted above.
  * On the 2-Series 20 and 24-pin part, an additional options is available, and the default when using Optiboot with those parts: Alternate Reset; with this option selected the UPDI pin retains it's UPDI functionality, but the PIN_PB4 ceases to be an I/O pin, and instead acts like an external RESET pin! In light of the continued scarcity of HV UPDI programmers within the hobbyist community. This option will result in an error message if bootloading a 0/1-Series is attempted.
* Tools -> Startup Time - This is the time between reset (from any cause) and the start of code execution. We default to 8ms and recommend using that unless you have a reason not to - that default option is generally fine. In rare cases it may make sense to change, such as particularly slow rising power supplies when BOD is not enabled, such that it needs to wait longer than the usual 8ms to have a voltage high enough to function reliably at, or conversely, when the power supply is known to be fast rising (or BOD is in use) and you have need to respond almost instantly after a reset.
* ~Tools -> Voltage Baud Correction - If you are using the internal oscillator and reaaaaally want the UART baud rates to be as close to the target as possible you can set this to the voltage closer to your operating voltage, and it will use the factory programmed internal oscillator error values. Under normal operation, this just wastes flash and is not needed. That is why it now (as of 2.3.0) defaults to Ignore~. Removed from 2.5.0
* Tools -> `printf()` implementation - The default option can be swapped for a lighter weight version that omits most functionality to save a tiny amount of flash, or for a full implementation (which allows printing floats with it) at the cost of about 1k extra flash. Note that if non-default options are selected, the implementation is always included in the binary, and will take space even if not called. This applies everywhere that format strings are used, including `Serial.printf()`.
* Tools -> attachInterrupt Mode - Choose from 3 options - the new, enabled on all pins always (like the old one), Manual, or the old implementation in case of regressions in the new implementation. When in Manual mode, You must call `attachPortAEnable()` and replace `A` with the letter of the port) before attaching the interrupt. This allows `attachInterrupt()` to be used without precluding any use of a manually defined interrupt (which is always much faster to respond). Basically any time you "attach" an interrupt, the performance is much worse.
* Tools -> Wire Mode - In the past, you have only had the option of using Wire as a master, or a slave. Now the same interface can be used for both at the same time, either on the same pins, or in dual mode. To use simultaneous master or slave, or to enable a second Wire interface, the appropriate option must be selected from tools -> Wire Mode.
* Tools -> millis()/micros() - If set to enable (default), `millis()`, `micros()` and `pulseInLong()` will be available. If set to disable, these will not be available, Serial methods which take a timeout as an argument will not have an accurate timeout (though the actual time will be proportional to the timeout supplied); `delay()` will still work. Disabling `millis()` and `micros()` saves flash, and eliminates the `millis()` interrupt every 1-2 ms; this is especially useful on the 8-pin parts which are extremely limited in flash. Depending on the part, options to force `millis()`/`micros()` onto specific timers are available. A `#error` will be shown upon compile if a specific timer is chosen but that timer does not exist on the part in question (as the 0-Series parts have fewer timers, but run from the same variant). If RTC is selected, `micros()` and `pulseInLong()` will not be available - only `millis()` will be.
* Tools -> UART for Bootloader - If using Optiboot bootloader, select which set of pins you want to use for serial uploads. After making a selection, you must connect a UPDI programmer and do tools -> Burn Bootloader to upload the correct bootloader for the selected option.

## Warnings and Caveats
There are however a few cautions warranted regarding megaTinyCore - either areas where the core is different from official cores, or where the behavior is the same, but not as well known.

### Direct Register Manipulation
If you are manually manipulating registers controlling a peripheral, except as specifically noted in relevant reference pages, the stated behavior of API functions can no longer be assured. It may work like you hope, it may not, and it is not a bug if it does not, and you should not assume that calling said API functions will not adversely impact the rest of your application. For example, if you "take over" TCA0, you should not expect that using `analogWrite()` - except on the two pins on the 20/24-pin parts controlled by TCD0 - will work for generating PWM. If you reconfigure TCA0 except as noted in Ref_Timers, without calling `takeOverTCA0`, both `analogWrite()` and `digitalWrite()` on a PWM pin may disrupt your changed configuration.
## Differences in Behavior between DxCore and Official Cores
While we generally make an effort to emulate the official Arduino core, there are a few cases where the decision was made to have different behavior to avoid compromising the overall functionality; the official core is disappointing on many levels. The following is a (hopefully nearly complete) list of these cases.

### I2C **Requires** External Pullup Resistors
Earlier versions of megaTinyCore, and possibly very early versions of DxCore enabled the internal pullup resistors on the I2C pins. This is no longer done automatically - they are not strong enough to meet the I2C specifications, and it is preferable for it to fail consistently without external ones than to work under simple conditions with the internal ones, yet fail under more demanding ones (more devices, longer wires, etc). However, as a testing aid, we supply Wire.`usePullups()` to turn on the weak internal pullups. If `usePullups()` ever fixes anything, you should install external pullups straight away. Our position is that whenever external pullups are not present, I2C is not expected to work. Remember that many modules include their own on-board pullups. For more information, including on the appropriate values for pullups, see the [Wire library documentation](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/libraries/Wire/README.md)

### Serial Does Not Manipulate Interrupt Priority
The official core for the (similar) megaAVR 0-Series parts, which megaTinyCore was based on, fiddles with the interrupt priority (bet you didn't know that!) in methods that are of dubious wisdoom. megaTinyCore does not do this, saving several hundred bytes of flash in the process, and fixing at least one serious bug which could result in the microcontroller hanging if Serial was used in ways that everyone tells you not to use it, but which frequently work anyway. Writing to Serial when its buffer is full, or calling `Serial.flush()` with interrupts disabled, or during another ISR (which you *really shouldn't do*) will behave as it does on classic AVRs and simply block, manually calling the transmit handlers, until there is space in the buffer for all of the data waiting to be written or the buffer is empty (for `flush()`). On th stock megaAVR core, this could hang forever.

### SerialEvent Support is Dropped
This is deprecated on the official core and is, and always has been, a dreadful misfeature. Dropped as of 2.3.0.

### `digitalRead()` Does Not Turn Off PWM
On official cores, and most third party ones, the `digitalRead()` function turns off PWM when called on a pin. This behavior is not documented by the Arduino reference. This interferes with certain optimizations, makes `digitalRead()` take at least twice as long (likely much longer) as it needs to and generally makes little sense. Why should a "read" operation change the thing it's called on? We have a function that alters the pin it's called on: `digitalWrite()`. There does not seem to be a logically coherent reason for this and, insofar as Arduino is supposed to be an educational platform it makes simple demonstrations of what PWM is non-trivial (imagine setting a pin to output PWM, and then looking at the output by repeatedly reading the pin).

### `digitalWrite()` and `INPUT` Pins
Like the official "megaavr" core, calling `digitalWrite()` on a pin currently set `INPUT` will enable or disable the pullups as appropriate. `digitalWrite()` also supports "CHANGE" as an option; on the official core, this will turn the pullup on, regardless of which state the pin was previously in, instead of toggling the state of it. The state of the pullup is now set to match the value that the port output register was just set to.

This was done because of the huge volume of code that makes use of this behavior. We experimented with making pinMode() do the inverse for INPUT and INPUT_PULLUP, but this was removed by unanimous agreement by everyone in the discussion thread.

### `analogWrite()` and TCD0 Pins
Please see the above PWM feature description if using PWM on those pins and also using `digitalRead()` or direct port writes on the same pins (PIN_PC0, and PIN_PC1).

### TCA0 is Configured in Split Mode to Get 3 Additional PWM Pins
On the official "megaavr" board package, TCA0 is configured for "single mode" as a three-channel 16-bit timer (used to output 8-bit PWM). megaTinyCore always configures it for "Split mode" to get additional PWM outputs. See the datasheets for more information on the capabilities of TCA0. See [Taking over TCA0](https://github.com/SpenceKonde/megaTinyCore/blob/master/megaavr/extras/TakingOverTCA0.md) for information on reconfiguring it. One downside to this is that the compare channels do not support buffering, so changing the duty cycle can cause a glitch lasting up to one PWM cycle (generally under 1 ms).

### TCA0 TOP is 254, Not 255
0 is a count, so at 255, there are 256 steps, and 255 of those will generate PWM output - but since Arduino defines 0 as always off and 255 as always on, there are only 254 possible values that it will use. The result of this is that (I don't remember which) either `analogWrite(pin,254)` results in it being `LOW` 2/256's of the time, or `analogWrite(pin,1)` results in it being `HIGH` 2/256's of the time. On megaTinyCore, with 255 steps, 254 of which generate PWM, the hardware is configured to match the API, and this does not occur. As it happens, 255 also (mathematically) works out such that integer math gets exact results for `millis()` timing with both 16-MHz-derived and 20-MHz-derived clock speeds, which is relevant when TCA0 is used for `millis()` timing. The same thing is done for TCD0, though to 509, giving 510 steps. `analogWrite()` accounts for this, so that we can get the same output frequency while keeping the fastest synchronization prescaler for fastest synchronization between TCD0 and system clock domains.

### Type B Timers Not Used for PWM
On the official "megaavr" board package, as well as [DxCore](https://github.com/SpenceKonde/DxCore), the Type B timers are used to generate 8-bit PWM (one pin per timer). There are very few circumstances where this could increase the number of usable PWM pins. These timers are just too scarce and valuable on these parts. Being minimally useful for PWM, in short supply, and highly desirable for other purposes, support for using Type B timers for PWM was removed in order to save space that would otherwise be used initializing these timers for PWM and handling them in `analogWrite()` et. al. If a Type B timer is used for `millis()`, it is configured in a radically different way than the official core does it.

### Digital I/O Functions Use Old Function Signatures
They return and expect `uint8_t` (byte) values, not `enum`s like the official megaavr board package does. Like classic AVR cores, constants like `LOW`, `HIGH`, etc are simply `#define`d to appropriate values. The use of `enum`s unfortunately broke many common Arduino programming idioms and existing code (granted, these idioms were poor programming practice - they're also incredibly widespread and convenient), increased flash usage, lowered performance and made optimization more challenging. The `enum` implementation made language design purists comfortable and provided error checking for newbies, because you couldn't pass anything that wasn't a PinState to a digital I/O function and would see that error if you accidentally got careless. Nevertheless, due to all the complaints, a compatibility layer was added to the official core, so all the old tricks would work again, it was just less performant. However, that got rid of what was probably the most compelling benefit by allowing the workarounds: the fact that it did generate an error for new users to train them away from common Arduino practices like passing 1 or 0 to `digitalWrite()`, `if(digitalRead(pin))` and the like. The choice of names of the `enum`s also had the perverse effect of making `PinMode(pin,OUTPUT)` (an obvious typo of `pinMode(pin,OUTPUT)`) into valid syntax (comma operator turns `pin,OUTPUT` into `OUTPUT`, and it returns a new `PinMode` of value `OUTPUT`...) and does nothing with it, instead of a syntax error (It took me over an hour to find the erroneous capitalization. That evening, I converted the digital I/O functions to the old signatures and removed the `enum`s). Anyway - the `enum`s are not present here, and they never will be; this is the case with [MegaCoreX](https://github.com/MCUdude/MegaCoreX) and [DxCore](https://github.com/SpenceKonde/DxCore) as well.

## Very Low-level Differences Between Classic and Modern AVRs
There are two classes of significant low level architectural differences (aside from the vastly improved peripherals): the improved instruction set and the unified memory address space.

### Instruction Set (AVRe/AVRe+ vs AVRxt)
The classic AVR devices all use the venerable `AVRe` (ATtiny) or `AVRe+` (ATmega) instruction set (`AVRe+` differs from `AVRe` in that it has hardware multiplication and supports devices with more than 64k of flash). Modern AVR devices (with the exception of ones with minuscule flash and memory, such as the ATtiny10, which use the reduced core `AVRrc`) all use the latest iteration of the AVR instruction set, `AVRxt`. `AVRxt` has much in common with `AVRxm` (used in XMega parts) in terms of instruction timing - and in the few places where they differ, `AVRxt` is faster (SBIC, as well as LDD, and LD with pre-decrement, are all 1 clock slower on `AVRxm` vs `AVRxt` or `AVRe`), however `AVRxt` doesn't have the single-instruction-two-clock read-and-write instructions for memory access `LAT`, `LAC`, `LAS`, and `XCH`. The difference between subspecies of the AVR instruction set is unimportant for 99.9% of users - but if you happen to be working with hand-tuned assembly (or are using a library that does so, and are wondering why the timing is messed up), the changes are:
* Like AVRe+ and unlike AVRe (used in older tinyAVR), these do have the hardware multiplication.
* PUSH is 1 cycle vs 2 on classic AVR (POP is still 2)
* CBI and SBI are 1 cycle vs 2 on classic AVR
* LDS is 3 cycles vs 2 on classic AVR :disappointed: LD and LDD are - as always - two cycle instructions.
* RCALL and ICALL are 2 cycles vs 3 on classic AVR
* CALL is 3 cycles instead of 4 on classic AVR
* ST and STD is 1 cycle vs 2 on classic AVR (STS is still 2 - as any two word instruction must be)

As you can see, everything that involves writing to the SRAM is faster now; it would appear that any time it is writing to a location based on one of the pointer registers or the stack pointer, it's a single cycle. All the other improvements except `CBI` and `SBI` can be viewed as a consequence of that. Of course, the variants of `CALL` are faster; they have to put the return address into the stack. I can't say I've ever felt like `LAT`, `LAC`, or `LAS` would be terribly useful as they are described in the instruction set manual - those take a register and the address pointed to by the Z register, load the contents of the specified address and toggle, set or clear in that memory address the bits that were set to begin with in the register. If that worked on special function registers, it would be a very useful instruction, taking `PERIPHERAL.REGISTER |= SOME_BIT_bm;` from a 5 clock, non-atomic operation to a 2 clock atomic one! But it says they only work on SRAM... so not as much of a loss. `XCH` is more obviously useful than the others, but all 4 of them come with the need to set up the Z register... which in many cases would take long enough that it wouldn't be a notable improvement.

Note that the improvement to `PUSH` can make interrupts respond significantly faster (since they have to push the contents of registers onto the stack at the beginning of the ISR), though the corresponding `POP`s at the end aren't any faster. The change with `ST` impacted tinyNeoPixel. Prior to my realizing this, the library worked on SK6812 LEDs (which happened to be what I tested with) at 16/20 MHz, but not real WS2812's. However, once I discovered this, I was able to leverage it to use a single tinyNeoPixel library instead of a different one for each port like was needed with ATTinyCore (for 8 MHz, they need to use the single cycle `OUT` on classic AVRs to meet timing requirements, the two cycle `ST` was just too slow; hence the port had to be known at compile time, or there must be one copy of the routine for each port, an extravagance that the ATtiny parts cannot afford. But with single cycle `ST`, that issue vanished).

Oh, and one other instruction it doesn't have that (some) AVRxm parts have: The hardware `DES` encryption instruction - an instruction which is most effective at marking AVRxm as, ah, back from the time when `DES` was a big deal.

### Memory and Address Space Organization
On all modern AVRs with up to 48k of flash, both the flash and ram reside in the same address space - On tinyAVRs, the program memory starts at 0x8000, while on megaAVR 0-Series, it starts at 0x4000 to leave room for the 48k of flash that they can have, and on the Dx-Series parts with up to 32k of flash, they have the same layout as the tinyAVRs, while Dx-Series parts with 64k or 128k of flash have a 32k section of flash mapped at any given time (how to make sure variables go into this memory mapped flash has been described elsewhere in this document). There is another big and fundamental change to the layout of the address space as well: the registers are organized by peripheral. PORTA is assigned 0x400 to 0x41F. PORTB is the next 32 bytes, and so on - and the address space is far sparser - all the peripherals have multiple "reserved" registers that may or may not get functions added in the future. And each instance of a peripheral on a part that has multiple of them has the same layout. You can, say, pass a pointer to a TCB around without the functions that get it knowing which TCB they'll get, and then access the TCB registers through it. On classic AVRs the names of the registers were consistent, but their locations were all over the place, packed much more tightly, so that sort of trick isn't possible. This also means that the EEPROM (and USERROW) are part of this unified address space (on classic AVRs, reading was accomplished through special function registers, and was far more awkward).

#### The "I/O space" is Used Differently
The lowest 64 registers are special - you can read or write them with the `IN` or `OUT` instructions (hence, "I/O space") in a single clock cycle, *without* setting up a pointer to them as you would need to with `ST` or `LD`. The 32 "Low I/O registers" additionally have bit-level access instructions `CBI` and `SBI` to clear and set bits, and `SBIC`/`SBIS` to skip the next instruction if a certain bit is set or cleared. On all AVRxt parts released so far, the low I/O registers are used only for the VPORTs, up to VPORTG or the last port on the part, whichever comes first. This means `VPORTG.OUT |= 1 << n`, *where n is known at compile-time and constant*, is a 1 clock cycle atomic operation , while `VPORTG.OUT = 1 << n` (note the `=` in lieu of `|=`) takes two clock cycles. For the latter, the first cycle is to put the value to be stored into a register, and the second is to write it with an `OUT` instruction. The GPIOR0-3 registers occupying the last 4 bytes in the low I/O space (those are user-defined registers to use as you choose. We use GPIOR0 internally during startup to record reset cause, and store two types of warnings applicable to tuning). The reset flag register is always cleared very early in startup to prevent dirty resets, and when using a bootloader, so that it can honor bootloader entry conditions on next reset). No other part of this core touches those registers, and we only set GPIOR0; we never read it. So all can be used freely, as long as you remember that GPIOR0 is not empty when you enter setup, and contains the reset cause flags. Other Low I/O registers are not used by the hardware.

The 32 "high I/O registers" are used even less - they only contain the the stack pointer, `RAMPZ` on the 128k DA/DB parts, `SREG`, and `CCP` (Configuration Change Protection - where `_PROTECTED_WRITE()` does it's magic to let you write to protected registers. That's all - 5 out of 32 registers are used, the rest are "reserved". On classic AVRs, registers for assorted peripherals that the designers thought would be accessed often were put into the I/O space, so it was a disappointment that they didn't put an alias of any other registers there. I'd vote for the intflags registers to be aliased there

## License
megaTinyCore itself is released under the [LGPL 2.1](LICENSE.md). It may be used, modified, and distributed freely, and it may be used as part of an application which, itself, is not open source (though any modifications to these libraries must be released under the LGPL as well). Unlike LGPLv3, if this is used in a commercial product, you are not required to provide means for users to update it.

The DxCore hardware package (and by extension this repository) contains DxCore as well as libraries, bootloaders, and tools. These are released under the same license, *unless specified otherwise*. For example, tinyNeoPixel and tinyNeoPixel_Static, being based on Adafruit's library, are released under GPLv3, as described in the LICENSE.md in those subfolders and within the body of the library files themselves.

The pyupdi-style serial uploader in megaavr/tools is a substantially renovated version of pymcuprog from Microchip, which ~is not open source~ *has now been released under the open source MIT license!*.

Any third party tools or libraries installed on behalf of megaTinyCoreCore when installed via board manager (including but not limited to, for example, avr-gcc and avrdude) are covered by different licenses as described in their respective license files.
