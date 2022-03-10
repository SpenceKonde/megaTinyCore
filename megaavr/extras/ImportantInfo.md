# Index of Important Information
Since people frequently miss it, it is hoped that this document will help steer people to key information relevant to the core.

## What to do if you find a bug
Check the [issue list](https://github.com/SpenceKonde/megaTinyCore/issues) on Github, and if your problem isn't alreadty there, create an issue in Github. If no issue is created, no matter how big the problem is for you, even if you've discussed it with people on forums, spray painted it on the side of a building, and had it tattooed on your back, probably do not know about it. Problems like that don't get fixed. Problems reported via github issue, even piddling unimportant ones, do get fixed.

### If you're not sure if it's a bug or misunderstanding
Make sure you've read the relevant errata from the manufacturer. Some aspects of these parts are really buggy. It is evident that therPost in the [discussions section](https://github.com/SpenceKonde/megaTinyCore/discussions) and ask - I still get notifications, but i try to keep issues on the topic of actual defects with the core, since it's used as a to-do list.

## Supported Parts
These pages contain pinout charts, specs, and datasheet links. Just like the pinout charts for the official boards, these are required reading for even the simplest projects and demonstrations.

* [ATtiny3227, 1627, 827, 427](ATtiny_x27.md)
* [ATtiny3226, 1626, 826, 426](ATtiny_x26.md)
* [ATtiny3224, 1624, 824, 424](ATtiny_x24.md)
* [ATtiny3217, 1617, 817, 417](ATtiny_x17.md)
* [ATtiny3216, 1616, 816, 416](ATtiny_x16.md)
* [ATtiny1614, 814, 414, 214](ATtiny_x14.md)
* [ATtiny412, 212](ATtiny_x12.md)
* [ATtiny1607, 807](ATtiny_x07.md)
* [ATtiny1606, 806, 406](ATtiny_x06.md)
* [ATtiny1604, 804, 404, 204](ATtiny_x04.md)
* [ATtiny402, 202](ATtiny_x02.md)

## From the Manufacturer
* [The datasheets are](Datasheets.md) the official, authoritative resource that describes the expected behavior of any part. As soon as you need to step outide the Arduino API, you'll need it.
* [The I/O headers are](ioheaders/README.md) where all those register names and constants specific to these parts are defined, like the capitalized ones ending in `_bm` (bit mask) `_bp` (bit position) `_gm` (group mask) `_gp` (group position) and `_gc` (group code). I recommend copy/pasting the names from those files, rather than trying to deduce how they named a given one, and attempting to type and spell it correctly.
* [AVR Instruction Set Manual](https://ww1.microchip.com/downloads/en/DeviceDoc/AVR-InstructionSet-Manual-DS40002198.pdf) The holy scripture of the AVR instruction set. Depending on the individual writing the code and the task, it is either completely irrelevant, or absolutely essential. I keep a copy it open in my PDF viewer at all times.
* [Silicon Errata](Errata.md) are "bugs" in the hardware. These are covered in a document separate from the datasheet, so really, to understand how to do something, you read the datasheet until you think you get it. Then you go check the errata to see if anything you just read was actually wrong for the version of the parts that exists in reality, instead of what was intended. Unfortunately, the 0-series, and especially the 1-series, has a LOT of errata, much of which has not been fixed and may never be. Fortunately, matters are considerably better on the 2-series; particularly notable is an almost complete absence of new issues related to the ADC. Only the `LOWLAT` bug impacting some devices seems to be related to the new ADC, and in general, there isn't very much errata compared to any other modern AVR.

## From the toolchain folks
* [AVR Inline Assembler cookbook](https://www.nongnu.org/avr-libc/user-manual/inline_asm.html) This is not a particularly well written document, and has not been updaed in years. Unfortunately, there isn't some other updated version hiding somewhere. It dates back two decades, to the infancy of AVR and gcc version 3.3. Fortunately, the syntax of the gcc asm() statement has not changed.
* [Calling Conventions used by avr-gcc](https://gcc.gnu.org/wiki/avr-gcc) This document sometimes assumes more background knowlesdge than one might like - but as with the cookbook above, there isn't an abundance of alternatives.

## Installation information
We only officially support (and test on) the Arduino IDE. Other IDEs are often used and are well thought of, but I do not have time to learn them (recall that I run an online store and do freelance electronics design in addition to maintaining three cores); issues reported which only reproduce on alternative IDEs must include some information on what is going wrong on that platform, and what needs to be changes in the core to correct it I have no relevant knowledge of those platforms
* [Arduino IDE](../../Installation.md) is the only platform tested internally.
* [PlatformIO](PlatformIO.md) is very popular. You can thank @MCUDude for most of the work to get this core supported over there, and that guide
* [Arduino-cli](Arduino-cli.md) is an official command-line version of Arduino. This guide was submitted by @dattasaurabh82 - THANKS!

## General information
* [Change Log](../../ChangeLog.md) weith listings of all changes that I remembered to record.
* [README](../../README.md) **(READ IT)** This is the main documentation which describes the state of the standard Arduino API functions and general caveats and key information about these parts, as well as special features of this core. There are sections of that file covering most significant areas of megaTinyCore, including sections on I2C, SPI, and reading analog voltages, including the advanced functionality of the ADC in the 2-series parts, and much more.
* [Making UPDI programmer](../../MakeUPDIProgrammer.md) These (unless running Optiboot) require a UPDI programmer, not a classic SPI programmer like a USBAsp/etc. Fortunately a dirt cheap serial adapter can be used as a UPDI programmer - or a cheap nano clone can be loaded with jtag2updi - either way gets you a UPDI programmer for the cost of a cup of coffee.
* [Direct Port Manipulation](DirectPortManipulation.md) Like with the classic AVR parts, you can use direct port manipulation for ultra-fast read and write from pin. The names of the registers are different, and there are some additional caveats, but also some improvements, and the pins and their interrupts are, overall, more powerful.
* [Sleep modes and power down](PowerSave.md) The same sorts of power savings available on classic AVRs are also available with the new tinyAVR parts - with slight differences in the setup procedures, but few substantial differences. Power-down sleep mode consumption of 0.1uA at 25C is typical assuming you can get all the peripherals turned off (at higher temperatures, power consumption is markedly higher - at 85C, the top of the range, 5uA in powerdown is typical). See the datasheet for more details.
* [Names of the interrupt vectors on all parts](InterruptVectorNames.md) because for reasons I do not understand, it's a warning, not an error, to make a typo in the name of a vector; the compiled result hangs if this improperly defined interrupt is triggered. **This also contains some general information on interrupts on these parts.**
* [Taking over TCA0](TakingOverTCA0.md) and using it to generate 16-bit PWM, or 16 bit arbitrary pulses
* [Hardware timers and how this core uses them for PWM and millis](PWMandTimers.md) is documented in this document. PWM frequencies are designed to be similar to popular classic AVR devices by default.
* [Manufacturer specified "speed grade" that the 0/1-series are rated for](SpeedGrades.md) are documented here, and are extremely conservative.
* ["Tuning" the internal oscillator to different speeds](Ref_Tuning.md) is supported since 2.4.0. You can use the inclucded sketch as described here with a trusted timebase to generate calibrated values to write to USERROW, and have the core use those - but if there are no such constants stored, we'll take our best guess.
* [Manual power saving and sleep configuration](PowerSave.md) until I write the long delayed power saving library (avr/power.h doesn't do anything on these parts)

## Reference Material
* [Analog Input (ADC) and output (DAC)](https://github.com/SpenceKonde/megaTinyCore/blob/master/megaavr/extras/Ref_Analog.md) The API reference for the analog-related functionality that is included in this core beyond the standard Arduino API.
* [Digital I/O and enhanced options](https://github.com/SpenceKonde/megaTinyCore/blob/master/megaavr/extras/Ref_Digital.md) The API reference for the digital I/O-related functionality that is included in this core beyond the standard Arduino API, as well as a few digital I/O related features that exist in the hardware which we provide no wrapper around.
* [Interrupts](https://github.com/SpenceKonde/megaTinyCore/blob/master/megaavr/extras/Ref_Interrupts.md) Includes a list of all interrupt vectors that can be used, how the flags are cleared (not a substitute for the datasheet - just a very quick reminder), which parts each vector exists on, and and what parts of the core, if any, make use of a vector. It also has general guidance and warnings relating to interrupts their handling, including estimates of real-world interrupt response times.
* [Pin Interrupts](https://github.com/SpenceKonde/megaTinyCore/blob/master/megaavr/extras/Ref_PinInterrupts.md) Covers attachInterrupt() and the special considerations and complications associated with pin interrupts.
* [Serial](https://github.com/SpenceKonde/megaTinyCore/blob/master/megaavr/extras/Ref_Serial.md) The USARTs (Serial) have some additional features not seen on the official cores.
* [SerialUPDI](https://github.com/SpenceKonde/AVR-Guidance/blob/master/UPDI/jtag2updi.md) Serial UPDI is our recommended tool for UPDI programming.
* [Clock Information](https://github.com/SpenceKonde/megaTinyCore/blob/master/megaavr/extras/Ref_Clocks.md) Supported clock sources and considerations for the use thereof.
* [Speed Grades](https://github.com/SpenceKonde/megaTinyCore/blob/master/megaavr/extras/Ref_SpeedGrades.md) Manufacturer specs for speed at various voltages, and some discussion of BOD thresholds.
* [Constants for part/feature/pin/version identification](https://github.com/SpenceKonde/megaTinyCore/blob/master/megaavr/extras/Ref_Defines.md) The core feature defines are used by megaTinyCore and other cores I maintain as well. This also documents what constant values are defined by the core for version identification, testing for features, and dealing with compatibility problems.
* [Assembly Listing and Memory Maps](https://github.com/SpenceKonde/megaTinyCore/blob/master/megaavr/extras/Ref_Export.md) Export compiled binary generates both assembly listings and memory maps, in addition to the hex file. The options selected are encoded in the name of the file to help prevent confusion, and make it easy to compare two configurations when you are surprised by the differences between them.
* [Reset control and the WDT](https://github.com/SpenceKonde/megaTinyCore/blob/master/megaavr/extras/Ref_Reset.md) The sources of reset, and how to handle reset cause flags to ensure clean resets and proper functioning in adcverse events. **Must read for production systems**
* [Optiboot reference](https://github.com/SpenceKonde/megaTinyCore/blob/master/megaavr/extras/Ref_Reset.md) The installation and operation of the Optiboot bootloader (for uploading over straight serial (not SerialUPDI)) is described here. Not recommended except on the 20/24-pin 2-series (since they have the alt reset pin) or for special use cases that demand it.
* Other documents
These guides are older, some are still relevant
* [Timers and megaTinyCore](https://github.com/SpenceKonde/megaTinyCore/blob/master/megaavr/extras/PWMandTimers.md) This contains some of the information that would go into Ref_Timers like DxCore has.
* [Advanced ADC features - For 0/1-series only](https://github.com/SpenceKonde/megaTinyCore/blob/master/megaavr/extras/ADCFreerunAndMore.md) This document describes how (on the 0 and 1 series only) the timer can be taken over and reconfigured, with particular attention to free running mode. The 2-series ADC is different, and it would require changes to reflect those differences.

## Special Libraries
The libraries included with the core have README files associated with them
* [tinyNeoPixel - based closely off of adafruitNeopixel library](tinyNeoPixel.md), optimized for minimized space, with support for all clock speeds megatTinyCore supports higher than 4 MHz.
* [Logic](../libraries/Logic/README.md) (CCL) - the Logic library provides an easy to use wrapper around the "Configurable Custom Logic" blocks. The API matches DxCore and MegaCoreX versions of the library.
* [Comparator](../libraries/Comparator/README.md) interfaces with the analog comparator or comparators on these devices. The API matches DxCore and MegaCoreX versions of the library.
* [Event](../libraries/Event/README.md) for working with the Event System (EVSYS). The API matches DxCore and MegaCoreX versions of the library, which is to say that it doesn't address many of the key pain points you're likely to encounter when working with the event system.
* [SPI](https://github.com/SpenceKonde/megaTinyCore#spi-support) which works just like the normal one.
* [Wire](https://github.com/SpenceKonde/megaTinyCore#i2c-twi-support) which works like the standard one, but has a few extra features (and multimaster support coming soon!)
* [USERSIG](../libraries/USERSIG/README.md) for storing data in the User Signature Space (often called the "USERROW" but we can't call the library that because it's already defined by the io headers). Same API as EEPROM library.
* [EEPROM](../libraries/EEPROM/README.md) for storing data in the EEPROM. The API is the same as the standard one, but the docuemntation provides valuable information and warnings specific to these parts.
* Servo and Servo_megaTinyCore for controlling standard hobby servos. If you've installed the Servo library through library manager, you will need to use Servo_megaTinyCore.h in order to use the (compatible) version included with the core instead of the (incompatible) one installed in libraries folder. Both copies of the library have the same API as normal Servo.h.
* [Optiboot_flasher](../libraries/Optiboot_flasher) for calling into the Optiboot bootloader to write to the flash from application code. Like MegaCoreX's version and similar to some classic Optiboot-supported parts, and totally different from DxCore's Flash.h.
