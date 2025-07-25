# Changelog
This page documents (nearly) all bugfixes and enhancements that produce visible changes in behavior throughout the history of megaTinyCore. Note that this document is maintained by a human, who is - by nature - imperfect (this is also why there are so many bugs to fix); sometimes the changelog may not be updated at the same time as the changes go in, and occasionally a change is missed entirely in the changelog, though this is rare. Change descriptions may be incomplete or unclear; this is not meant to be an indepth reference. Versions that have been ~struck through~ are versions that are known to be no good and should not be used

## Planned changes not yet implemented
These items are in addition to what was listed under changes already in release.

### Planned enhancements
"Enhancements" are changes to the core which improve functionality and/or introduce new and exotic bugs. Sometimes called "Features", I prefer the term "enhancement". Calling it a feature, by my understanding of the semantics, means that it *does something new*. But many times changes are made that neither fix a bug or do something new, but rather just do something it already does faster, using less flash, or with better compile time error detection. All things that, as well as new features, would qualify as an enhancement.

#### Planned 2.7.0
* Migrate to avrdude 7.2
* Add support `_getCurrentMillisTimer()` and `getCurrentMillisMode()`, required for sleepTime.


#### Enhancements which do not have a strict timeline or are ongoing
* Finally implement that power save and sleep library I've been talking about for years.
* Add in the obnoxious interrupt features of SPI. I changed that over a year ago and this is the first complaint (about that, obviously), this needs to be implemented significantly better than the garbage stock implementation, which was a hellspawned abomination (the worst kind!)
* Under consideration: analogWriteFast(pin, duty); this will require that pin be constant, allowing the determination of the PWM compare value register to be determined at compile time, rather than runtime, and if implemented, the user *must* have already called analogWrite() on the pin in question to kick off analog output - but this function would modify the duty cycle highly efficiently.
* Port any applicable enhancements made to DxCore to megaTinyCore, should such happen be made.
* If there are *other substantial changes that need to occur* within the core, I am unaware of the complaints and hence have no plans to address them before the heat death of the universe. If you desire changes on a more rapid timeline, please create an issue so that I am aware of the presence of said problem, deficiency, or imperfection. Those form the action item list for core development activity, so if something is not listed there, **it is unlikely to be implemented/fixed/etc** simply due to my being unaware of any concern.

## Unreleased changes
Changes listed here are checked in to GitHub ("master" branch unless specifically noted; this is only done when a change involves a large amount of work and breaks the core in the interim, or where the change is considered very high risk, and needs testing by others prior to merging the changes with master - everything else goes straight into master). These changes are not yet in any "release" nor can they be installed through board manager, only downloading latest code from github will work. These changes will be included in the listed version, though planned version numbers may change without notice - critical fixes may be inserted before a planned release and the planned release bumped up a version, or versions may go from patch to minor version depending on the scale of changes

### Planned 2.6.11
* Pending: New toolchain version.
* Make wiring.c have the functions referred to in the doc.
* Fix some of the constants for timers so that all timers can still get uniform codes specifying the portmux and (for non-TCA's) which pin within the mux it is, which matters forthe other core.
* Documentation improvements.
* Corrected bug sometimes encountered when using serial under atypical cases the causes of which remain mysterious), where it would complain about `__poll_dre_done`.
* Correct bug with Comparator (#1236)
* Correct numerous issues with the microchip board defs. Clearly nobody is using them - since 2 of them didn't compile, and none of the optiboot ones would ever be able to successfully be programmed over the bootloader because selfprogramming wasn't enabled (BOOTEND = 0).
* Major Bugfix: Correct issue #1164 thanks to John Vasileff. This would cause Serial.flush to hang in one-wire mode.
* Major Bugfix: Correct buffered PWM on all parts.
* Major Bugfix: Correct PWM on non-default pins on all parts; digitalPinHasPWM.
* Fix bug with CCL clock source.
* Fix bug with Comparator
* Enhance documentation
* Removed example of exactly the sort of boneheaded and strictly-worse in a trivial part of Wire. While it was merely a bad example at the time it was written, it is now known to be a bad, dangerous example, because it featured code that demonstrated Strictly Worse access to registers, by loading a vport register to a pointer register, and using that - not only is it stupid and just a less-featureful PORT when accessed that way, avoiding pointer access to the I/O space (64 addresses, - 28 VPORTs and 4 GPIORs in low I/O and SP, RAMPZ (if applicable), CCP (the register used for `__PROTECTED_WRITE()`}), and of course the SREG. And indeed, none of those are registers that anyone should ever access using a pointer, even if it didn't wasn't safe against an incredibly scary bug - the low I/O is all about the bitwise instructions, which need both the register and bit compile time known. `VPORT_t*` is not a type you should ever declare - if you have had to declare it, *you get no benefit* from faster access to it (like you do if you use the compile-time known VPORT's amd access them without pointing. )
* Enhance error messages from wrong-part on upload.

## Released Versions

### 2.6.10 - Critical fix
* Critical bugfix: Switch to using Azduino7c, which is the same as the Azduino7b binary, but with the correct CRC attached.
* Bugfix: #1014, named constants for ADCPowerOptions() did not work on 0/1-series parts. The 4 valid options are now given named defines, and the rest are #defined as `badArg("This option is on 2-series tiny and AVR Ex-series only")`

### ~2.6.9~
**Note** This version does not install on windows. Use 2.6.10
* Bugfix: optiboot_x.c that has been in use since newyears day 2023 was never committed.
* Bugfix: Remove boot_opt.h which is not applicable to modern AVRs.
* Bugfix: Remove the useless dummy app that forced us to use avr-size -A to see the size of the bootloader separated from the app, and switch avr-size -A to normal avr-size to take advantage of this.
* Maintenance: Rebuild all bootloader files.
* Bugfix: The recently introduced entry condition functionality of Optiboot was busted.
* Maintenance: Rebuild all bootloader files again.
* Bugfix: Correct issue with missing getAnalogReadResolution function body.
* Bugfix: Ensure that the default parameters in boards.txt for 8-pin parts are defined boardwide, rather than for the default option, because people on PIO don't know when new options are added, or that all options are expected to be included. This was done for most - but not all - parts.
* Bugfix: Azduino7 toolchain for windows had excessive bloat (duplicated files - which raises some questions about the BZIP2 algorithm. (note that if arduino knew how to deal with .tar.xz (LZMA), the toolchain download would be 1/3rd the size!, and all platforms had bad copies of eeprom.h and power.h.
* Enhancement: Improve error messages when third party IDEs like PIO allow people to specify more than one millis timer, which would then fail to compile (#1002).
* Maintenance: Work around CLI regression by specifying cli 0.33.0 for the CI.
* Enhancement (contributed): Merge in support for using XDIR and RS485 (custom optiboot build only).
* Enhancement: Add a test to the start of twi_pins.h that will allows it to be carried to another modern AVR core that doesn't provide badArg and badCall. Add CORE_HAS_ERRORFUNS #define.
* Enhancement: Add `_digitalPinToCanon()` to convert pin numbers to `(port * 8) + bit_position`, will be needed to address recently reported shortcomings of the SPI library surrounding interrupts.
* Documentation: Many, many improvements to reference docs.
* Documentation: Move the errata tables to a google sheets spreadsheet. The table was beyond markdown
* Packaging: Do not delete the entire extras folder, only .png/gif/jpg/jpeg files and the subfolders, when making the board manage versions.

### 2.6.8
* CRITICAL bugfix: Fix issues introduced by pwm option menu. This prevented compilation on 1-series or Microchip boards. There were at least *4 separate issues* feeding into this.
* Enhancement: Add turnOnClockout() and turnOffClockout() to megaTinyCore.h
* Bugfix: Documentation and examples for megaTinyCore.h did not match the code. Examples did not compile and documentation described functions with the wrong names. Stuff like that.
* Bugfix: Fix analog reference documentation. Add getAnalogReference, because we advertise having it.
* Bugfix: Fix issues with generating universally valid WS2812 signals with tinyNeoPixel when running at 20 MHz-30 MHz
* Bugfix: Add UpdateLatch() method to tinyNeoPixel to adjust the latch timeout, because times as high as 250 us and as low as 6us have been encountered in the wild, and 50 us, the default, may or may not manifest in any real parts. If we're gonna have that check there, it should be meaningful, but to make it meaningful, we have to make it tunable. (even though it rarely, in practice, causes problems, because it usually takes longer than 250 us to generate a frame of pixels, and even when it doesn't we usually want a consistent frame rate and so slow it down a bit from sending the updates as fast as it possibly can). Marked bugfix because it is unarguable that a non-changable default value which is not known to match any extant hardware is a bug. Whether this is the best resolution is debatable. See tinyNeoPixel docs for more information.
* Enhancement: Improve testing coverage which would have prevented some of these bugs from making it through.

### 2.6.7
* Enhancement: finish implementing PWM pin set selection tools submenu. All reasonable pin mappings are available for 8-pin (3 combinations, 2 on 0/2-series), and 14-pin (6, 4 on 0/2-series) parts, and most plausibly useful ones on the larger parts (over a dozen on the 20-pin and 25 on the 24-pin parts), less than 20% of the absolute number of combinations. If 3-pin PWM mode is selected, it will still be 8 bit PWM - but you are guaranteed not to have a glitch when slowly raising the duty cycle because buffering is used.
* Fix issue with the change to the comparator and logic libraries so it actually lets you make your own ISR.
* Bugfix - Change clockCyclesToMicroseconds, microsecondsToClockCycles, and clockCyclesPerMicrosecond back into macros instead of inlinable functions, as some libraries depend on them being valid constexprs (which an inlinable function is not)
* Bugfix - pinModeFast will now turn off pullups if they're on when a pin is set to output. otherwise, the result could be highly problematic. For example, a situation I ran into: I had a keypad matrix, 4x4, rows set input pullup, columns set output, low. The device woke up when a button was pressed, which pulled the row down. At that point (while the button was still pressed - wake is slow on computer timescales, but very fast on human ones), I would read the pins, and then swap the pin modes, so the rows were output-low, and the columns were input pullup. One of the columns would thus be pulled low through the depressed switch, I'd read the columns, combine the two. Since this was happening in an ISR, hence should be fast, I used the pinModeFast function (and direct port reads, since I cared about a whole nybble of each port). Then I'd set the rows back to input_pullup and the columns back to output, low, and then enter power-down sleep mode. However *the pullups on the columns (output, low) were still on*. While the effect on battery life was non-catastrophic (it had a several hundred mAh rechargeable battery, and the excess load was a little under half a milliamp, so the battery life would be expected to be nearly a month. But without the driven-low, pullup-enabled pins, the current was below my ability to measure (datasheet says typical 0.2uA - so it was about 2000 times expected). This is no longer possible - pinModeFast(pin, OUTPUT) will turn off the pullups on the pin if they're on. pinModeFast is now a 6 word 7 clock function in this case, which is disappointing, but I think this would catch more people than the number of people who need e
* Remove optimization menu as it was found to be incompatible with our implementation of Serial, and I had not a clue how to fix it but could see well enough that this would have required generating horrendously bad code. At some future time, a menu might go in there, but the options have to work. ()
* Enhancement: Choose PWM mapping pins from tools menu!
* Bugfix: printHexln was spelled printHexLn in keywords.txt

#### 2.6.7 addendum - these were supposed to be released in 2.6.6
* Enhancement/bugfix: 2.6.5 was intended to add an optimization level submenu. Due to a typographical error, the menu was not shown (though no harm was done). That menu is now actually available.
* Enhancement: Add a few more macros for getting information on peripherals and updated define reference.
* **Critical Bugfix** Correct regression in Wire.h regarding master+slave mode
* Bugfix: Attached pin interrupts on parts with less than 128k of flash were wasting 2 clocks before the user code was called and 3 clocks after, as well as 4 bytes. Small potatoes compared to the overall overhead of that abomination, but there's no need to push RAMPZ if there isn't a RAMPZ register (it reads as 0 and writes to it are ignored - so there was a donothing in/push and pop/out on parts without RAMPZ. And no tinyAVR parts have a RAMPZ.
* Bugfix: pinConfigure would under some circumstances fail because of a misspelled variable
* Bugfix: gcse was incorrectly left off at one step (whereit had little if any impact on flash use) regardless of the menu setting (which coulod not be changed anyway)

### 2.6.5
* ~Enhancement: Add optimization level submenu. This allows switching to -O3 instead of -Os, and disabling or not disabling "GCSE". The latter has been found to change the size of compiled binaries by up to 5%.... in either direction, or sometimes barely change it at all, with no apparent pattern. When you get desperate for flash, you can turn it on and see if it helps your sketch, You have about a 60% chance to gain at least a tiny bit of flash, though your odds of a large improvement aren't as good.~
* Bugfix: Correct issues relating to optiboot board definitions for Microchip boards (missing board entries, missing bootloaders).
* Bugfix: Correct issue with Sampled/Sampled and Sampled (125hz)/Disabled BOD options. (#874)
* Enhancement: Port new version of pinConfigure.
* Significant Enhancement: Reimplement the whole entry condition logic for Optiboot. Increase number of potential entry conditions for 2 to 7. This is like what DxCore has, plus one which will be added there in the future.
* New Feature: Ensure that without any code changes, just use of a different batch file, exactly analogous files can be generated for the megaAVR 0-series parts, with our behavior (entry conditions and reset cause flag treatment)
  * Docs: Considerable improvements to documentation of Optiboot for developers.
* Maintenance: Harmonize versions of Wire, USERSIG, Event to pull in improvements from DxCore.
* Bugfix: Fix issue with reading with SerialUPDI (#871)
* Bugfix: Fix several USART-related issues (USART0 not working in half duplex, usart1 not working for receive, .end() less efficient than it could be and potentially wrong.)
* Performance enhancement: From @MX682X, the discovery that an appropriately timed kick to the compiler's groin would cause it to place pointers into base registers when it otherwise would not. Doing this increases efficiency dramatically in some cases, allowing the use of load and store with displacement in several places where it had previously used a truckload of LDS and/or STS instructions. This resulted in smaller, faster binaries due to more efficient initialiaation code for a number of peripherals (which impacted even users who weren't doing anything special, since the biggest beneficiaries were the ADC and timer initialization code, which happen as long as main() is not overridden). This also includes the addition of a number of macros to implement such kicks to permit this method to be readily generalized if other areas of the core are found which could benefit from it.
* Organizational enhancement: Move all of the dirty performance enhancement macros like the ones above from Arduino.h to newly added dirty_tricks.h. Change names of many macros to make them easier to type. Document these functions in [the dirty_tricks reference](megaavr/extras/Ref_dirty_tricks.md).
* Docs: General day-to-day maintenance
* Add what little support was needed for analogWrite() to work if there were tools submenu to use differerent subsets of pins to wiring_analog.c. Variant files however still require significant work, however!

#### Known issues
* Optimization level menu is not displayed.
* The version string may be incorrectly reported as 2.6.5-dev. The numeric version is correctly reported as 2.6.5.1. The workaround for the "Major version not found" bug in 1.8.14 and later Arduino IDE versions requires an easily forgotten double-entry bookkeeping step.

### 2.6.4 (2.6.3 respin due to critical compile error impacting all sketches)
* Critical Bugfix: Correct compile error encountered in all cases.
* Bugfix: Correct compile error encountered in the SimpleEvent example.
* New feature: Add support for configuring the WDT via fuses. These are set on all uploads, but ONLY for non-optiboot boards. To prevent difficulties that may be encountered when reprogramming. Because of the reliance of Optiboot on the WDT, the WDT must not be forced on when using Optiboot.
* Bugfix: Correct problems with CI changes.
* Bugfix: Correct issues relating to the fuse configuration set when uploading.

### 2.6.3 (2.6.2 respin due to json error)
* Bugfix: Correct installation error from trying to install non-functional withdrawn version of toolchain.
* Bugfix: Correct issues with compiling Comparator library.
* Bugfix: Ensure that at least one test per library is in the CI list.

### 2.6.2 (critical update)
* Critical bugfix: Burn Bootloader corrected
* Bugfix: Serial UPDI Chip erase via the Protected KEY Interface didn't happen, it was relying on the chip erase nvmctrl command, which can't be used on a locked chip, or, for that matter, one spamming software resets.
* Serial UPDI ID for AVR32DB48 corrected
* Bugfix: Undesired quote stripping bug workaround for windows cmd /C
* Enhancement: Wire updated to latest from MX682X
  * Sleep works correctly on devices acting as I2C slave.
  * Wire now supports a 256byte buffer (not recommended, but it does)
* Enhancement: Wire further modified:
  * Add a hacky option to enable the compatibility settings, most importantly the SMbus levels for talking to lower voltage parts.
  * Fix compile time error checking as is best possible within the limitations of the system (for pins, swap, and special options)
  * Wire doc enhancements
* Docs: General doc enhancements, particularly to Ref_Reset.
* Bugfix: Corrected half-duplex mode, which turned out to be a long and slow crawl, bringing work to a standstill for like a week
* Bugfix: MEGATINYCORE_NUM macro was gone.
* Bugfix: Add the missing #defines for peripheral count and reorganize core_devices to make porting easier, fix CORE_SUPPORT_LONG_TONES misspelling.
* Bugfix: Eliminate warnings from redefinition of macros.
* Enhancement: Add missing keywords for core #defines for feature ID.


### 2.6.1 (critical update)
* 2.6.0 Bugfix: Correct several USART bugs that snuck through, including an apparent regression that would prevent proper serial operation.
* 2.6.0 Bugfix: Azduino5 was no good. Same filesystem structure error as usual, moved to Azduino6.
* 2.6.0 Bugfix: The 4000 lines of workarounds for compatibility issues introduced by the toolchain update didn't make it into the megaTinyCore release.
* 2.6.0 Bugfix: The platform.txt file had been trashed by a bungled merge and tried to upload based on params that didn't exist.
* 2.6.0 Bugfix: Correct an include path that confused some alternative IDEs.
* 2.6.0 Bugfix: Correct compile error involving RS485 USART configuration.
* 2.6.0 Bugfix: Comparator library namespace error fixed. (thanks @ObviousInRetrospect)
* 2.6.0 Bugfix: Advertised 2 MHz internal option was not actually available in non-tuned configurations. Prior to 2.6.0 there was no intent to support this.
* Enhancement: millis with a type B timer is over 25% faster, and uses 30% less flash for the ISR. (thanks @MX683X)
* Bugfix: Correct a few anomalies in boards.txt.

### 2.6.0
* You can now enable and disable the ADC with analogPowerOption() and enable/disable standby mode.
* Bugfix: Fix issue with SSD bit being cleared when using beginTransaction().
* Bugfix: Fix bug in Logic with pin inputs being handled improperly.
* Bugfix: Remove multiple signatures for Wire.requestFrom to fix issues with Wire with certain libraries.
* Bugfix: Correct tuning, particularly to speeds below 16 MHz. Boards should be re-tuned if 14 MHz-derived speed is to be used
* Bugfix: Correct default option for Optiboot 2-series boards with 20 pins to be the one with alt reset.
* Bugfix: `long_soft_event` method did not work correctly.
* Bugfix: Removed attempted optimization by inlining analogReadEnh; I had wrongly assumed that it would then not inline the call to `_analogReadEnh`. Well, it wound up inlining it, with the result being exactly what I had hoped to prevent. I don't think this made it into a release.
* Bugfix: Event was not functioning correctly on tinyAVR parts.
* Bugfix: Correct bug(s) when waking from sleep mode via TWI (aka I2C/Wire) address match (Thanks @MX682X. You are one of our MVPs). TWI slaves should now *reliably* wake on address match and other wake sources from all sleep modes.
* Bugfix: 3224 optiboot maximum upload size was same as 1624. Oops.
* Bugfix: Microchip boards in optiboot mode (admittedly a rare mode) were not subtracting the size of the bootloader from the chip capacity.
* Enhancement: Lay groundwork in Event library for the new event system changes in the EA-series (There will be 2 generators per port, and 2 for RTC - but all of these will be accessible by all event channels, and a register on the peripheral controls which of the options is used for these two channels). While inapplicable to tinyAVR, Event.h and Event.cpp distributed with DxCore and megaTinyCore is identical.
* Bugfix: Fix compatibility of Comparator, Event, and Logic.
* Workaround: mysterious issues with detecting the major version that prevented manual installation ever since 1.8.14. I kept hoping that the next version would fix it, but, no such luck. It still impacted used on 1.8.19 and likely future versions as well.
* Major enhancement: Change class hierarchy for UARTs, as was done for Two_Wire (Wire.h), so that rather than pulling in api/HardwareSerial.h, and subclassing that definition of HardwareSerial (itself a subclass of Stream) as UartClass, we instead simply subclass Stream directly. UART.h will be renamed to HardwareSerial.h, HardwareSerial.h (a compatibility layer) will be renamed to UART.h and the latter adjusted to #define UartClass as HardwareSerial, and api/HardwareSerial.h will be gutted and simply #include <HardwareSerial.h> This will remove yet another piece of the disastrous "ArduinoAPI". I describe it as such because on low resource platforms like AVR; this will allow several functions currently declared virtual to lose that keyword, as the linker, even with LTO enabled, is not permitted to remove unused virtual functions. This reduces binary size when using serial significantly, despite significant new features. Credit goes to @MX624X.
* Enhancement: Split up the Event and Logic library headers for improved readability.
* Enhancement: RTC millis does not interfere with waking on RTC compare match. (PR #793)
* Enhancement: great deal of harmonization with DxCore.
* Enhancement: Improvement to stream timed read to make it work when millis is disabled, and to save 4 bytes of RAM. Note that this also requires all offsets used to access the Serial transmit and receive buffers to be reduced accordingly in the inline assembly in UART.cpp.
* New Feature: Now you can use ADC1 just like it was an ADC0, see the notes in [Analog Input (ADC) and output (DAC)](https://github.com/SpenceKonde/megaTinyCore/blob/master/megaavr/extras/Ref_Analog.md) for steps required to enable it. This does not use space if not enabled.
* New Feature: Add new functions to assist checking error codes in analogRead results.
* New Feature: Permit Logic and Comparator ISRs to be defined manually if (and only if!) the `.attachInterrupt()` method of an instance of that class is not called. You can then manually create the ISR saving about 40 bytes and 50 clock cycles, give or take.
* New Feature: megaTinyCore.h now provides functions to reset chip via WDT timeout and via software reset. If optiboot is used, the former will not run it, while the latter will.
* New Feature: Add option for serial autobaud use, record cumulative errors USART has encountered and provide method to view and clear them.
* New Feature: Support SFD to wake on USART. User is otherwise responsible to configuring this and should be aware that in order to be able to receive the characters, we turn it off after receiving a character.
* Docs: made clear om PowerSave.md that the ADC must be disabled to get low standbby current.
* Docs: Add link to my article on AVR math speed (TLDR: avoid `int64_t` `long long` like the plague)
* Docs: Fix many documentation issues, improve docs generally.
* Docs: Expand documentation significantly, and document the new serial features.
* Unenhancement: Remove getAnalogSampleDuration(), as I could not make anything compile with that in and had worked on it over a day.
* Enhancement: 2 MHz is now a supported clock speed.

### 2.5.11
* **CRITICAL BUGFIX** - I finally saw the bug in my attachInterrupt implementation. I had a operands of a load backwards. (#651)
* **CRITICAL BUGFIX** - Apply changes from DxCore to SPI library where needed for new attachInterrupt behavior.
* Make it possible to have applications that use our USERSIG library be code compatible with DxCore by providing do-nothing functions in place of dxcore's flush(), pending() functions, add erase() and make everything match the return types and report success.
* Documentation improvements in a few areas.
* Based on feedback, removed emulation of classicAVR pinMode() calls as regards the state of the PORTx.OUT register.
* tinyNeoPixel 2.0.4, with support down to lower speeds, and inline assembly that is technically correct. (Before this it was only an accident of the deterministic avr-gcc register allocation and the fact that illegally written read-only operands represented variables that fell out of scope without being further used)
* Fix output asset names for Microchip board definitions.
* Preempt compatibility problems with libraries that assume a HardwareSerial.h file.
* Improve user experience for people using updated compilers. Makes bit of a mess out of uart_constants.h though.
* Fix bugs with compatibility in names of constants associated with no change in function, this time in association with TCB clock source
* Fix issue with SerialUPDI when validating code where (size of sketch) % 512 = 1 or 2.

### 2.5.10
* **CRITICAL BUGFIX** - attachInterrupt was broken on default mode for PORTA (#625)
* Fix references for comparator library. (#639 - thanks @mechatroniks-git )
* Improve error reporting for SerialUPDI.
* 2.5.8 is removed from board manager. Users with this installed will find that it is no longer available. Those unfortunate souls still on 1.8.9 but who upgraded will require manually deleting the packages folder inside the of the Arduino15 folder to install or otherwise modify the megatinycore package. This will eliminate the error message everyone has been seeing when the board.json. There was no way to eliminate the error which did not result breakage to users on 1.8.9 and earlier with 2.5.8 installed, and the error was producing far too many big reports. Users who are still on 1.8.9 should really upgrade. 1.8.13 is the recommended version currently due to serious bugs in the latest versions.

### 2.5.9
* **CRITICAL BUGFIX** - no changes to code, just botched the json file tools-dependencies in trying to fix the avrdude version.

### 2.5.8
* Respond more gracefully when data that doesn't fit in Wire buffer is "written".
* **CRITICAL BUGFIX** - correct uploads through avrdude on non-windows platforms.
* Correct avrdude tool version to use version 17 of avrdude on 32-bit x86 linux. and version 18 everywhere else, instead of not installing on that platform because avrdude 18 isn't available for it.

### 2.5.7
* Different (better) fix for the wire library issue fixed in 2.5.6, from @MX682X (#593), we no longer use the ring buffer, simplifying the code and saving flash
* A bug in SerialUPDI which had been present since it's introduction but went unnoticed has been fixed. One of the last changes that went into Serial UPDI after almost all testing was done, was a "read chunking" option to work around an issue found in a serial adapter that was very flaky (it was a D11C programmed for serial adapter operation, with bugs). That particular buggy serial adapter however is used in the curriculum of a professor who had been driving the push to improve SerialUPDI, and getting uploads to work using that as a UPDI adapter was the entire reason for his and Quentin's involvement in this, so when we discovered that the issue was still there, -rc was added and we then confirmed that it could now upload and verify code successfully (note that the bug in the D11C serial firmware was corrected a few days later - there was a specific amount of data that when sent or received would crash it due to a bug in the USB library, which eventually was corrected) However, the standalone read functionality was not retested. Turns out it was TOTALLY BUSTED whether or not read chunks were requested. There were two issues here: First, the variable was called max_read_chunk in some places, and max_chunk_size in others. Second, when it finally filtered down to the read function, if read-chunk was specified, it only worked up to 256 bytes, because it would only ever use words if read-chunking was not requested, which imposed an unnecessary and incoherent limit, since not specifying it would use the maximum of 512b.
* Serial UPDI: Change warning level of spammiest messages. Verbose output can now be enabled from the IDE, and the volume of output it generates is sane.
* Add some comments in optiboot_x.c including prospective new version of flash_led which ensures that the led blinks at the correct speed regardless of OSCCFG fuse.
* Not a change we made, just an observation: There is no more stylechecking on code because the tool we used has vanished from the internet and it's URL doesn't even resolve now.


### 2.5.6
* **Critical "bugfix"** to work around bug in IDE that prevents parsing of certain parameters and prevented the fix implemented for optiboot uploads from working correctly. {upload.serialparams} doesn't parse. Neither does {serial_params}. It makes no sense. If it was a reserved name, both would refuse to parse, but instead only one does.
* Proliferation of SerialUPDI options has occurred to provide options for mac/linux users (in particular) because their OS's often reduce latency such that the normal 230.4k baud rate doesn't work. Unfortunately I can't hide options based on OS.
* Fix serialUPDI write delay not rely on sleep() which lacks the granularity we need.

### 2.5.5
* **Critical Bugfix** to correct issue with uploading via optiboot.
* Document my butchering of the board manager json and how to correct issues caused by it. (sorry! )
* Housekeeping in UART.h and UART.cpp
* **Critical Bugfix** to correct attachInterrupt, which would corrupt the stack when used in the default mode. (DxCore #225)
* Allow the DxCore names for RTC clock sources to be used.
* **Critical Bugfix** to correct return values from Wire.endTransaction() to match the API. (DxCore #226)
* Correct serious defect in new Wire library. The point of using 32b buffers is to match basically everything else, since having less than the assumed amount will cause failures to many libraries. However, we also stuff the slave address into the buffer, so 32 byte buffers only give us 31 bytes of data. This was detected when it broke the Adafruit OLED library. Fixing it by enlarging the buffer and sacrificing power-of-2-ness cost 54-78 bytes of flash. I was able to get back around 40% of that for parts with buffer sizes under 128b (ie, all of the megaTinyCore; DxCore doesn't care since they got 130b buffers, though they need 131b or I should have just done 128b, cause we need 130b of "data" to write a 128b page to external EEPROMs) (#593)
* Correct typo in boards.txt impacting old wrong-sig ATtiny402's. (#592)

### 2.5.4
* **CRITICAL BUGFIX**. Prior critical bugfix was unsuccessful because the flashsize test was comparing it to the wrong value. Additionally, the branch before a jmp, with it's offset specified numerically, needs to match the size of the rjmp or jmp instruction; This corrects that.

### 2.5.3
* **CRITICAL BUGFIX** Serial-using sketches would fail to compile if they were large enough.
* Make software serial suck somewhat less by performing a single bitwise-and to calculate the result of the modulo operator, instead of dividing a 2-byte signed value which we know will never be larger than twice the buffer size (of 64) and hence fits in a single unsigned byte.

### 2.5.2
* **CRITICAL BUGFIX** - burn bootloader was broken on most parts.

### 2.5.1
* Added support for serial buffer sizes of 256.
* Added test for defined(USE_ASM_TXC), USE_ASM_RXC, and USE_ASM_DRE in UART.h so that variants and board definitions can now turn this off.
* Attempting to use illegal options, like buffer sizes that aren't powers of 2, now errors out.
* **CRITICAL BUGFIX** for issue introduced in 2.5.0 which could COMPLETELY BREAK SERIAL if the sketch used >8192b of flash, with obtuse and uninformative error messages

### 2.5.0
* **New I2C/TWI implementation** (I2C and TWI refer to the same interface, the one provided by Wire.h; don't get me started about how these and the library are named).
  * Support for acting as both master and slave (on the same pins); this configuration, sometimes known as "multi-master", includes not only the simple case of multiple masters and slaves on an I2C bus, each of which is always exclusively either a master or a slave, but also the more complicated case, which has been a frequent request: For the AVR device to act as both master AND slave. It can both initiate transactions with slaves, or respond to transactions initiated by other masters.
  * New tools menu option to select whether to support only being a master *or* a slave, as we do now (default) or to support acting as both master *and* slave (new functionality).
  * Support for Dual Mode (one instance of TWI acting as master on one pair of pins and slave on another) on parts that support it (there are no current or announced tinyAVR parts with dual mode support.
  * Significantly reduced flash usage under all circumstances (even master+slave mode should use less flash than old master or slave mode - the cost of supporting master-and-slave mode is RAM for a second buffer. There is an implementation included that can use the same memory for both buffers, however, it is not currently exposed via an option due to the risk of breakage if you receive while preparing to send something.
  * Failed attempts will timeout instead of hanging indefinitely.
  * Support for slave counting the bytes read by the master, and for slave to check whether it's in the middle of a transaction (for example, before sleeping)
* Recent change to C++17 required additions to new.cpp and new.h, including sized deallocation (`delete`) and alignment-aware `new` and `delete` operators. The sized deallocation operator is called when existing code that worked before is compiled to the C++ 17 standard; since free() doesn't care about the size, implementation was straightforward. Discussion is ongoing about the aligned `new` and `delete` operators, which are also new in this version of the standards. It is likely that we will not support them, since other Arduino cores aren't even building to C++ 17 standard, so if your code needs aligned new/delete, it also won't work anywhere else in Arduino-land. While we are not shy about adding features, we do so only to support hardware features. If conditions change we will revisit this matter.
* Using millis or micros (or rather attempting to) when they are unavailable due to millis being disabled, or in the case of micros, RTC used as millis time source, give better errors.
* Clarified license (for one thing, renamed to a .md so people can read it more easily, and know that it's readable if they're on windows) for tinyNeoPixel.
* Improved docs for tinyNeoPixel. The two libraries each have a README.md linking to a greatly expanded library document.
* Document use of WDT for it's original purpose, to protect against hangs!
* Actually prevent disabling warnings - -Wall for all! You should not be compiling code with warnings disabled, the vast majority of the time, they're pointing to problems, and in the cases that aren't bugs, they're still a weak point that future bugs could come from (and that people will comment on when you post the code on forums to ask for help). I thought I'd done this a long time ago. Also pull in some warning-related flags from DxCore, including making implicit function declarations error, since the implied declarations when this happens are basically never result in working code, but it occasionally compiles and malfunctions.
* Fix timekeeping on clock speeds only supported with external clocks or tuning when a TCA or TCB is used for millis (it's still busted with the TCD)
* Correct SYSCFG0 fuse settings when burning bootloader for 2-series parts - they default the reserved bits to 1 not 0, and worse still, setting them to 0 enables a mode we probably don't want.
* Stop clearing fuse 4 by writing the default values for TCD0 on a 1-series. Now, with great difficulty, we only set that on parts that actually have the type D timer in order to keep our promise of burn bootloader restoring the chip to a fully known state. (well, except for the user row, and EEPROM if you've got it set to retain).
* Fix theoretical EEPROM.h bug inherited from avr-libc, and keep millis() from losing time when writing more than one byte at a time; update and harmonize with DxCore.
* Harmonize Comparator.h with DxCore.
* Fix 402 with bad signature support.
* Fix names of .lst and .map
* Add avrdude.conf for the 32k 2-series parts which are now becoming available.
* Fix bug with disabled millis on tinyNeoPixel libraries not working. Again.
* Correctly comment out leftover debugging prints that would be called when using `tone()` (megaTinyCore #550).
* Adjust serial buffer size 512b and 1k parts by adding an intermediate 32b serial buffer size.
  * Parts with 512b are changed - from 16->32 for RX, TX unchanged at 16 (32->48 for each port used).
  * Parts with 1k are changed - from 64 to 32 for TX, RX unchanged at 64 (128->96 for each port used).
  * Smaller and larger parts are unchanged. This mostly helps to smooth out the RAM usage curve as you change flash size - going from 256 to 512 didn't previously change the allocation, while the jump from 512b to 1k was alarmingly large. The fact that the 8k 2-series have ports each makes this more noticeable. This combined with another breakpoint led me to think that something else was broken.
* Officially deprecate jtag2updi.
* Port micros and delay-microseconds improvements from DxCore.
* Add a set of compatibility defines to make life easier for people porting non-Event-library event-using code to 0/1-series.
* SerialUPDI reference now links to it's actual location.
* Port Serial changes here from DxCore. The reduced flash use I had hoped for did not materialize, unfortunately, I'm still working on it. There's a whole bunch more functionality though!
* Improved event library with more functionality for writing libraries and portable code.
* Port new attach interrupt from DxCore.
* Port new printf option from DxCore.
* Update callbacks to match DxCore 1.4.0, most notably the init_reset_flags(). It will automatically clear reset flags if not overridden, stashing them in GPIOR0 (chosen because has lower overhead than a variable)
* Platform.txt organization and commenting. Fix issues where defines were missing from lib-discovery phase.
* Reduce flash footprint of pinMode.
* Improve methods used to block attempts to use "upload using programmer" when an optiboot board is selected. That configuration is guaranteed not to work, and we should not do things that we know 100% will not work. We would need a merged output file for this, but the IDE doesn't make those for us here. (only over on attinycore, where they're not usable and we must go out of our way to delete the damned things)
* Timers.h shared with DxCore.

### 2.4.2
* Correct bug with openDrainFast(pin,CHANGE); This was a really nasty one, albeit for a rare corner case. Using CHANGE on a pin that was not on PORTA would alter other settings for other pins seemingly at random instead due to a missing '0x' in a memory offset.
* Add a whole bunch of missing devices to SerialUPDI.
* Fix a few examples so they dont throw warnings or demonstrate the wrong way to do things.
* Instruct linker to use rcall and rjmp when possible (-mrelax). This should always have been the case, and it's absence had significant negative impact that had until now gone unnoticed.
* Fix burn bootloader with SerialUPDI (#506)
* Spelling and grammar corrections - thanks! (#508)
* Fix issue with lowByte() macro and add test sketch to exercise all macros. (#515)
* Fix issue with some macros caused by switch to C++17 standard. Typeof being an extension, it must be `__typeof__` if compiled with standard C++ 17.

### 2.4.1
Bad release. Use 2.4.2

### 2.4.0
* Finally port Event.h to tinyAVR. The event system they shipped on the 0/1-series is, to use a technical term, a shitshow. Users planning projects which depend heavily on the event system are urged to use a 2-series part unless the project also requires a type D timer, but the AVR DD-series is for some reason not an acceptable alternative.
* Better TWI Baud formula (#422 - thanks @rneurink!!) - actually works more or less how it's supposed to now, rather than using baud buckets to simplify.
* Support use of Tuned Internal oscillator to operate at 12 MHz, 16 MHz, 20 MHz, 24 MHz, 25 MHz, 30 MHz (20 MHz base only, may be unstable, particularly on 0/1-series) or even 32 MHz (2-series @ 20 MHz base only, may be unstable). For many specimens, 32 MHz is just too fast, but I suspect 24 and 25 will work with all or nearly all parts under favorable conditions. You must run the tuning sketch connected to a 500 Hz squarewave (A tuning sketch that works by tuning with a classic AVR like a 328p or anything shaped like one is included (You must use one **WITH A CRYSTAL OR EXTERNAL CLOCK** if you want accurate tuning!); Tuning values are written to the USERROW (last 12 bytes). Once tuning is completed, it is recommended to repeat for the other base oscillator (ie, 16 and 20 MHz). Yes in tuned configuration, you can run at 20 if OSCCFG fuse is pointing at 16 MHz and vice versa. If you try to use an untuned chip, we'll guess, but we could be off far enough that UART won't even work - but so far it's guesses are spookily good on the 2-series.
* Add some sketches to assist in aforementioned tuning feature, see megaTinyTuner (runs on chip being tuned) and TuningSource (runs on chip - hopefully one with a crystal - being used to tune) as well as megaTinyDeTune which erases them and documented them in Tuning.md
* New version of SerialUPDI to fix issues some experienced at speeds above SLOW - those should now be fixed at 230400 baud. All speeds are a bit slower now with the CH340 adapter most severely impacted (this is what I use, so I am reminded of how serious this issue is every time I upload anything) Turbo does NOT have this change, and it is not compatible with all serial adapters. This is expected.
* Correct tone() issues and improve efficiency of tone while catching out several corner cases that could malfunction. That file now includes a discussion in comments that should be helpful to someone who wants a way to output multiple tones at once.
* clockcycle <-> microsecond macros were unavailable with millis off. Never mind the fact that with millis disabled, that's the only timebase you have. I just sorta grouped all the timing stuff together and disabled it without realizing some were used elsewhere, that is no longer the case. (#496, also found internally)
* Correct issue with SPI.setClockDivider(). (#458)
* Correct issue with Serial1.swap(1) swapping Serial instead of Serial on the TinyAVR 2-series. (#470)
* Further corrections to Wire library behavior and documentation mirroring DxCore. No multi-master (ie, master/slave support) yet - needs too much test for this timeframe.
* Removed menu options for converting UPDI pin to something else (except alt reset pin on 2-series) on non-optiboot boards. Expert users who have the necessary exotic programming tools can reenabling it by uncommenting lines in boards.txt (to get it to apply after every programming, part of your workflow if you use one device to do the HV reset, and a normal programmer to program it subsequently - not sure how common this is - I know less common than classic AVRs). This was never supposed to be an option.
* Dead code removal and cleanup of main.cpp
* Fixes to Print api missing a few standard api functions (#485)
* Fix bug with new/new.h - it was missing a method and the dummy file to allow including it like `#include <new>` - which is as far as I'm concerned perverse and wrong, but the C standard guys seem to feel otherwise.
* Enable C++ 17.
* Add more markdown linting to the CI checks. CI now runs for most boards with meaningful options selected.
* Add a few test sketches to verify compilation works for API functions not covered by the examples, these are not included in board manager distributions as they are not educational samples.
* **Major doc improvements** which I spent far too long writing.
* Expand interrupt vector name document with general interrupt information
* **Massive improvements to part-specific docs**
* Fixed pinout charts (#442)
* Installation now covers toolchain upgrade needed for manual installation.
* PlatformIO docs moved to Extras.
* New instructions for use with Arduino-cli
* Important info links to almost all important info.
* Readme covers watchdog timer more thoroughly, including how to reset it.
* Added page from which you can view the io headers from the github documentation, because it's a pain to dig for them on your local machine, and you'll inevitably want to have them at the ready.
* Errata section updated with recently discovered bugs. Changed my assessment of the severity of a number of the bugs (for example, the RTC bugs are much higher severity now, because I have watched someone trying to figure out how the hell the RTC worked; it wasn't pretty.)
* Correct bug when compiling tinyNeoPixel when no micros function is available.. **as the compile warning says, you are responsible for giving the LEDs enough time to update** in those cases. since we cannot ensure it without millis it. Previously this worked with the RTC options, but a typo prevented it from working with millis disabled entirely.
* add `digitalPinToTimerNow()` to match DxCore; this is like digitalPinToTimer() only it will return NOT_ON_TIMER if you have used the timer takeover functions to assume full responsibility for management of a timer. Tie up some loose ends around the millis timer related defines when RTC with external source is in use.
* Add support for using a pre-compiled library (that is, one distributed with .a files in place of the interesting parts of source code, typically as part of a proprietary license)
* Correct bug in `restart_millis()` when a TCA is used for millis timekeeping.
* Add human readable macros for the options passed to ADCPowerOptions() which provides control over some power management functionality of the 2-series parts.
* Add `openDrain()`, `openDrainFast()` and `pinConfigure()` for advanced digital IO functionality.

### 2.3.2
* Expand Keywords.txt to include register names! (#386)
* Add @MCUDude's Flash.h library (not the same as the DxCore library of same name and big-picture function) (#159)
* Correct issue with compilation when using TCB0 as millis timing source on parts without TCB1 (#394)
* Correct huge bug in tinyAVR 2-series analogReadEnh which adversely impacted accuracy of readings by inadvertently setting CLK_ADC to F_CPU/2, which exceeds the spec by 33-233%.
* add analogClockSpeed(int16_t frequency = 0, uint8_t options = 0). Call with no arguments or 0, and it will just return the current CLK_ADC speed. -1 will set it to the core defaults, and a number will set it to the fastest speed that does not exceed the supplied value. Speed is expressed as kHz. Set analogReference first, as this impacts the maximum and minimum ADC clock (0.55V reference requires much slower clock). This will respect the minimum and maximum clock speeds stated in the datasheet, and if you request something outside that range, it will not set it. Unless the lowest bit in `options` is set (that is currently the only option).
* Add bootloaders for 2-series parts. Correct a weakness in optiboot's implementation of the prime directive ("The application code shall not impact operation of the bootloader"). Poorly written code which jumped to the bootloader directly (or which malfunctioned and wound up running off the end of the flash or otherwise ending up with the program counter at 0x0000 without a reset) could, if it left peripheral registers in an adverse state, interfere with the operation of Optiboot; thus it is theoretically possible that if entry on POR was not enabled, and the app did this quickly enough, and the specifics of the adverse peripheral configuration also resulted in a hang or non-entry condition, the part could only be reprogrammed via UPDI. Now, very early in the initialization process, we check reset cause and if it's 0 (no reset since we cleared it before jumping to app after previous boot), we trigger a software reset, which is always an entry condition.
* Add boards.txt entries for 2-series optiboot boards.
* MASSIVE formatting overhaul in boards.txt - big headings that can be seen in the miniature view used to scroll on many editors. Everything is in the same order for all boards so I can add and remove lines with regexes.
* Removed the board definitions for specific Microchip boards. These will be reimplemented as specific options in the other groups (eg, Curiosity1627 will show up wherever the 1627 is listed; difference will be that it will always use the onboard debugger to upload (for non-bootloader configurations) and that LED_BUILTIN will match that hardware.
* Remove 8-pin alternate serial pin bootloaders. That configuration does not require a separate bootloader - they were binary identical to the 14-24 pin alternate serial pins version.
* Revise Logic library docs, add quick pin reference table to the readme.
* Correct Wire library clock generation (#400). Enhance and correct Wire.pins() and Wire.swap() to work with new hardware and reject constant invalid arguments.
* Also create memory map when doing export compiled binary
* Massive improvement to speed of serial-updi (like, 8-20x faster, programming time under 5 sec is normal). Three speed options are available normal, a SLOW version that runs at 1/4 of normal (57600 baud), and a TURBO, at 460800 baud (double speed); TURBO should only be used when Vcc > 4.5V.
* Disable Verbose serial-updi output. It would generate output about 4 times the size of the file being uploaded, and it was so detailed that almost nobody could decipher it. It was also incredibly repetitive; so much so that zipping it compressed it over 100:1. Since that was not useful, and without a way to get something in the middle, I just don't request verbose output, regardless of what is checked in the options. The verbose output was really only useful when modifying the upload tool.
* Correct issue when the two pins controlled by TCD were written to in rapid succession (#414)


### 2.3.1
* Rerelease to address toolchain bug. No code changes.

### 2.3.0
* Port analogWrite() to the ADC on the 2-series parts.
* Implement analogReadEnh(), for both tiny 0/1-series (suitable for porting to DxCore) and 2-series (suitable for porting to future EA-series core)
* Implement analogReadDiff() for 2-series (suitable for porting to future EA-series core)
* Implement configuration functions for new ADC functionality: ADCPowerOptions() for tinyAVR 2-series, analogSampleDuration() for both tiny 0/1-series and 2-series (separate implementations)
* Implement ADC error return codes.
* Document new features. Huge number of tweaks and improvements to the documentation, including large section on the new ADC-related functions. Please read it if you are using the ADC on a 2-series part or want more out of the ADC on a 0/1-series part.
* Adjust variant pin definitions to account for the 2-series (One added USART, 4 more analog inputs on 20/24 pin parts). Correct several long-standing and unnoticed bugs. Does anyone use digitalPinHasPWM()? If so, they don't use the 24-pin 0/1-series parts....
* Port the Comparator library. (#208)
* Add capability for Comparator.stop() to restore digital input on the pins it disabled when it was started.
* Move a bunch of hardware-related macros from Arduino.h the new core_parts.h. These are things that individual users are unlikely to need to reference, (certainly not as likely as anything that's in Arduino.h - which is where people generally know to look for core-related "stuff"
* Correct large number of tone() bugs. Tone could leave the pin high. A high pitch and long duration could result in much shorter tones than intended. Frequency of 0 would leave the timer running and generating interrupts in the background for the requested duration (now it stops output if it's on the current pin, otherwise does nothing). while doing nothing. No attempt was made to handle invalid pins: it would get null pointer to port struct.... and proceed to use it without testing if it was valid, or if the bitmask was valid. Now we test for valid bitmask before requesting the port struct. Frequencies over 32768 would overflow an intermediate when called with duration and the duration would be very short. Long durations can also overflow the intermediate. The intermediate is found as `2 * frequency * duration` and both multiplications can overflow (first one is as unsigned int, second as signed long.) Since the result is divided by 1000, remove 2* and change to /500 doubled the limit and removed the problem above 32768 Hz. If SUPPORT_LONG_TONES is defined as 1, then we will take additional steps to support tones longer than a minute; otherwise, when (frequency * duration) > 2^32, it'll overflow, and duration will be way short. If not specified as 0 or 1 (no existing board definition does), this defaults to 1 for any part with more than 8k of flash, as it adds 106 bytes to compiled binary and very few people want to make use of that.
* Update pymcuprog bridge tool.
* Converge on uniform convention for specifying analog channels vs digital pins as a uint8_t (high bit = analog channel), add ADC_CH() macro. Because the An constants are just digital pin numbers, this should mean very little to most users, but keeping this the same in all the cores I maintain will improve interoperability and make my life easier.
* Add warnings for #define being used on things that aren't macros, and hence likely generating unexpected behavior. Refuse to compile with implicit definitions, as 99 times out of 10, this is a bug, and it's a waste of time to upload it.
* delayMicroseconds should now be more accurate at low clock speeds, short delays, and cases where the delay is known at compile time. Particularly noticeable is the case of things like delayMicroseconds(1) at 1 MHz, (for example) where the delay wound wind up being more like 16 us before.
* delay() on parts with 4k+ of flash will now work with any argument that fits in an unsigned long. Previously when passed a value greater than 4.295 million, an internal calculation would overflow and the delay would return much faster. I felt that the 24 bytes was a justified price to pay where one has 4k or more flash. While very long delays are generally poor programming practice, this is Arduino - if there's one assumption we can rely on, it is that poor programming practices are highly likely to be encountered in the field, so we should try to support them.
* delay() on parts with 2k of flash will retain the old implementation in the name of saving a bit of flash. 2k is not enough flash for anyone, especially on modern AVR where the I/O space is not used for peripheral registers (don't underestimate the magnitude of this effect - many register writes are twice the size that they were on classic AVR, and with more options on the peripherals, there are often more of them). Will now error if called with a compile time known value higher than 4.294 million (as in, "I want 2 hour delay so `delay(2UL*60*60*1000)`)
* Wire buffer with 256b total ram now matches classic Uno/etc and the rest of the parts supported by this core at 32 byte I2C buffers; exceeding the buffer silently fails, leaving you waiting around for a call/zoom/etc which just kinda suuucks. . The only ones that don't 32b buffers within this core now are the parts with 128b rtam and 2k of flash. Since there are two buffers, they would have 50% of their SRAM as buffer, plus another 10% or so of it being bpointers to heads and tails, just not tenable. Larger parts supported by DxCore will start getting 130b buffers at 4k of flash ( with nothing getting 64 or anything else in the middle)
* non-Arduino-IDE tools may not pass all the defines that we expect - basically, no defines that we platform.txt and boards.txt can normally guarantee can be relied upon to be there!. If Arduino.txt does not see a MEGATINYCORE define, that must have happened. In this case, detect and define a placeholder - checking for that define is how other libraries recognize that this core is in use. Also check a few other important defines for existence, rather than assuming that because my boards.txt and platform.txt will always provide them, that they will be present and one of the valid options - the intent is simply to make sure we don't sleep-walk into a wacky wromg state (imagine an X which is always defined as A, B or C via the boards.txt/platform.txt configuration, one could do `/* well it's gotta be C then right? */`. But in a situation where we don't fully control the defines passed to the compiler through the command invocation with total certainty is supposed to be supported, we need to complain if there make sure we don't blindly assume that can't-happen-per-our-boards-and-platform-definitions actually can't happen, thus above situation needs to be `#if X==A` ..... `#elif X==B`  ..... `#elif X==C` ..... `#else      #error "X undefined or invalid, X must be #defined as A, B or C" #endif`. Doing this is less work than dealing with the support requests that would result from not doing so.
* Add some defines to indicate features provided by the core.... This list will be expanded over time:
  * `CORE_HAS_FASTIO` - if defined as 1. indicates that digitalWriteFast() and digitalReadFast() is available.
  * `CORE_HAS_OPENDRAIN` - if defined as 1, indicates that openDrain() and openDrainFast() are available.
  * `CORE_HAS_PINCONFIG` - Indicates that pinConfig() is available. Currently, this is not defined here as this is not yet implemented.
  * `CORE_HAS_TIMER_TAKEOVER` - if defined as 1, takeOverTCxn() functions are available to let user code take full control of TCA0, TCA1 and/or TCD0.
  * `CORE_HAS_TIMER_RESUME` - if defined as 1, the corresponding resume TCxn() functions, which reinitialize them and return them to their normal core-integrated functions (not yet on megaTinyCore)
  * `ADC_NATIVE_RESOLUTION` - This is the maximum resolution of the ADC without using oversampling. 12 for Dx-series. 10 or 12 for tinyAVR.
  * `ADC_NATIVE_RESOLUTION_LOW` - This is the low ADC resolution setting. 10 for all Dx-series parts. Lower-res ADC resp;iutopm option. 8 bits on tinyAVR, 10 on Dx (yes, that means every possible permutation of 2 out of 8, 10, and has beemn released within a few years of each other..)
  * `DIFFERENTIAL_ADC` - This indicates that the part has a differential ADC, irrespective of whether it is exposed.
  * `SUPPORT_LONG_TONES` - This is 1 if the above mentioned long tones are supported.
  * `CORE_HAS_ANALOG_ENH` - This is 1 if the enhanced version of analogRead is available, with automatic oversampling and decimation to extend resolution to 13 or 17 bits (0/1 and 2-series respectively)
  * `CORE_HAS_ANALOG_DIFF` - This is 1 if the differential analogRead is available. It has same features as enhanced, except that it takes a differential measurement.
  * `ADC_MAX_OVERSAMPLED_RESOLUTION` - This is the maximum resolution obtainable via oversampling and decimation using those functions.
  * `ADC_MAXIMUM_GAIN` - This is 0 for all parts except: Those with a programmable gain amplifier, in which case it is the maximum gain available, and those with one or more op-amps, which return (indicating that yes there is a gain, but using it is complicated and device-specific.

* Correct critical regression impacting PWM via TCA0 WO3, WO4, and WO5. (#335)
* Correct canShow() in tinyNeoPixel to permit use when micros() is not available - skip the test and #warn the user that they must make sure that they don't call show() too frequently. WS2812-alikes need 50us pause (according to datasheet - internet says only 20us) in the data between consecutive frames to tell them that it's time to latch the data they got, and if they don't see that before you start blasting out more data, they'll think it's part of the same frame and never turn on.
* Correct bug in RTC millis timekeeping that caused reverse time travel due to improper guarding against overflow during millis() processing. Also, it's an average of 1-2 us closer to the actual number now (neglecting any contribution from oscillator inaccuracy - which is of course orders of magnitude higher) (#331)


### 2.2.8
* Correct critical bug that prevented bootloaders from being installed when using the pyupdi-style serial port and resistor. Would report success, but only fuses were set.
* Fix bug where the INVEN bit would be unset when doing digitalWrite() on pins that can do TCD PWM; now it is only unset if PWM was actually turned off (which in turn implies that if it was inverted, that was done by analogWrite(pin,255) - that's how we can generate a continuous HIGH output without disconnecting the timer (which would produce a glitch on the other pwm channel)- we set the compare value higher than TOP (which would produce continuous low) and invert the pin.
* In the process of above, found both a terribly inefficient bit of code in analogWrite and turnOffPWM, and replaced with a much faster implementation that saves some flash too! By switching to bit_mask for most calculations a variable shift which could run as many as 7 iterations at 4 clocks each was eliminated from analogWrite. A loop that could
* Correct constrain, round and similar macros.

### 2.2.7
* Clean up Servo formatting and comments and synchronize with DxCore version of library.
* We were waiting for `ENRDY` to be set before changing enable-protected registers of TCD0. That doesn't appear to be needed. We just can't re-enable it until `ENRDY` is set. It makes the glitch when turning PWM off or on... maybe 6 CLK_PER shorter? I think under default settings, the loop goes from one iteration to none....
* Do not initialize the the compare values of TCA0. 1. They are guaranteed by hardware to start up at 0, and we don't CARE what they start up as, because before the core turns any of them on, it sets them to a value. 24 bytes saved! (STS x 6)
* Add `takeOverTCA0()`, `takeOverTCD0()`. Calling these will set a flag that tells `analogWrite()` and `digitalWrite()` not to try to configure these timers, and instead to act like the pin has no PWM functionality. User assumes responsibility for the management of Waveform Output.
* Update avrdude.conf to fix support for Snap, PICKit and Curiosity programmer options.
* Fix macro definitions of `sq()` and `constrain()` which were still vulnerable to stupid stuff caused by macro leading to arguments being evaluated multiple times.
* Add `openDrain(),` `openDrainFast()` - `LOW`, `FLOAT`, and `CHANGE` are supported. LOW set pin to output, `FLOAT` (#defined as 1 now) sets it input, and `CHANGE` toggles it. It *does* turn off PWM, and it *does* set the PORTx.OUT for that pin to 0 so carelessness can't lead you to switching a pin left set to HIGH's output on (on the reasoning that you may be switching a line connected to something that could be damaged if you drove it high). It does not, however, touch the `PINnCTRL` register (so pullup is left on if you'd already set it that way). However `openDrainFast()` - the fast-I/O version of that function - does NOT touch the PWM or PORTx.OUT register.
* IDE should now highlight a few more keywords associated with megaTinyCore.
* Minor formatting improvements in ChangeLog.md.
* Update avr-gcc toolchain to azduino3.

### 2.2.6
* Fix for baud rate in twi.c from 2.2.4 actually broke it was worse than it was before. I'm hopeful that now it should again work.
* SPI library no longer requires knowing the SS pin when you call `SPI.pins()` - the SS pin has no special function with the library.

### 2.2.5
* Another board manager installation fix. Sixth time's the charm!
* Fix a few of grammar, spelling, etc mistakes.

### 2.2.4
* Some general tidying turned into significant cleanup of neopixel library.
* Fix board manager build script - it will now correct the platform.txt during release process.
* Grabbed the default baud rate calculation fix from official megaavr core's Wire library. Maybe this will fix some of the occasional I2C problems people have reported?

### 2.2.3
* Add support for MPLAB SNAP and PICkit4 programmers.

### 2.2.2
* Critical bugfix for board manager installations on linux/macos.

### 2.2.1
* Critical bugfix - somehow, the `upload` and `program` specifications for avrdude were backwards. Optiboot uploads failed because it was attempting to write fuses, and the new feature of writing safe fuses didn't work when avrdude was the programming tool.

### 2.2.0
* Add support for programming with just a USB serial adapter and 4.7k resistor like pyupdi does. (#187, #285)
* On non-Optiboot configurations, set all fuses that the core sets based on menu options, except `BODCFG`(control brown-out detection) and `SYSCFG0`(controls configuration of reset/UPDI pin, as well as whether EEPROM is retained) on all uploads. Changing the reset pin configuration could "brick" a part if the user does not have a high voltage programmer, so setting this fuse is not "safe"; similarly, setting the BOD voltage higher than the operating voltage and enabling it will prevent programming. Since the device may be soldered to devices that are not tolerant of the >4.3V required by the highest BOD setting, this would also constitute "bricking". Other fuses cannot render the device unprogrammable, so there is no reason not to program them. Judging by the content of recent issues, this is a frequent point of confusion particularly with the clock speed, despite the fact that it has worked this way on literally every Arduino core released since the dawn of Arduino! (On classic AVRs, changing any fuse could potentially "brick" the part, so this was the only safe behavior). There are no menu options to configure `WDTCFG`or `TCD0CFG`, so those aren't touched on program, only upon burn bootloader - if you manually changed those, you probably don't want an upload to undo them, whereas "burn bootloader" will return the chip to a known configuration.
* On Optiboot configurations, set all fuses except `BODCFG`and `SYSCFG0` when "upload using programmer" is used (this still removes the bootloader, just like on classic AVRs). Uploads through the bootloader will not set fuses; almost all tools submenu options require "burn bootloader" to change.
* Fix nasty bug with Optiboot entry conditions (#259)
* Add Ersatz Reset example sketch, and bootloader option.
* Correct bug with bootloader version used on 14-24 pin parts when UPDI pin is configured as GPIO.
* add `millisClockCyclesPerMicrosecond()`; what WAS `clockCyclesPerMicrosecond()` is now this - difference is that the `millisClockCyclesPerMicrosecond()` gives the number of clock cycles on the timebase for the millis timers (so, for TCD0 as millis clock, the unprescaled internal oscillator, otherwise same as `clockCyclesPerMicrosecond()`) - apparently other libraries use this for things that depend on the system clock... including my own Servo implementation! (it was tested before that was the default millis source on 3216/3217)... This is *really* nasty when it bites....
* Fix Servo library - at long last! I never realized just *how* broken it was. (#195, #241)
* What the heck? When were people going to tell me about the regression on TCD0 PWM pins?! It just didn't happen... botched refactoring of USE_TCD0_PWM AND regression to code from the bad old days before I knew how to get PWM and millis...(#249)
* Reduced the magnitude of "glitches" possible on PWM pins when turning PWM off. Corrected PWM duty cycle calculations for TCD0 (it was (2 x dutycycle)/511 instead of (2 x dutycycle)/510 - no, it's not *supposed* to be 256ths, though it is very often implemented that way (if you count to 255, you get 256ths, because the timer considered 0 to be one count).
* `analogWrite()` is now interrupt-safe for TCD0-controlled pins (this may be addressed for the other pins in a future update - however, it is "naturally" interrupt safe except when either the interrupted analogWrite or the interrupt is turning pins on or off - in contrast, TCD0 analogWrite could break in a variety of ways in almost any situation where it was interrupted)
* Also implemented NEW special behavior on the TCD0 PWM pins: analogWrite(pin,0) and analogWrite(pin,255) set pin LOW or HIGH without turning off the PWM. On TCD0, this does not require turning off the timer briefly, which, over time, would cause `millis()` to lose counts when PWM was also used based on TCD0, in addition to reducing runtime. `digitalWrite()`, however, does. (also remember that `digitalRead()`, unlike official cores, `NEVER` turns off PWM)
* Add check for compile time known invalid pins passed to Arduino digital/analog API functions, analogReference and DACReference, as well as compile time known unachievable baud rates on `Serial.begin()` for Serial (#269)
* Correct bug in `digitalPinToAnalogInput()` when called with invalid values.
* `analogReadResolution()` now requires a constant argument which must be valid - the old implementation was just awful - there are only two (will be a third on the 2-series, as we unfortunately kinda do need to be able to emulate )
* Add support for `digitalReadFast()`, `digitalWriteFast()` functions. They are only valid if the pin is compile time known constant, and optimize down to something very fast (when second argument is compile time known, a single cycle for write. Looks like 4 cycles for read?) (#205)
* Emulate `digitalWrite` behavior of classic AVRs when pin not `OUTPUT`, namely, set the `PORTx.OUT` register so that if it *is* set output, that's the state it will be in.
* Update Logic library to latest version (as with DxCore).
* When using an internal reference for the ADC, forcibly enable it. Adjust the example of reading temperature to add the (signed) sigrow correction instead of subtracting it. This appears to get better values, and it is suspected that the code in the datasheet is wrong.

### 2.1.5
* Fix critical issue with "burn bootloader" when speeds other than 5, 10, or 20 MHz was selected on devices with 8, 20, or 24 pins and 14-pin devices using Optiboot.

### 2.1.4
* Fix critical bug in alternate pins for Serial (#254)
* Improve compatibility regarding build.board changes with a compatibility layer in case other libraries are checking for the old version. (#253)
* Use build.board pattern consistent with other cores (AVR_ATtiny). (#253)
* In order to reduce flash usage, do not use UART baud correction based on voltage on 2k and 4k parts (it was always close enough for typical use cases anyway), and those parts are very tightly flash constrained.
* Remove UART baud voltage menu for 8-pin parts due to above.
* Automatically use U2X `USART` option at 2 MHz or less, and on higher speed parts when appropriate for the selected baud rate. Most visible impact of this change is that you can now use 115200 baud at 1 MHz. (#188)
* Do not apply oscillator voltage (in)correction to baud rate when using external clock.
* Reduce flash usage of UART (Serial), particularly on smaller chips, through a great many mechanisms. (#252)
* Serial.print/write/flush will no longer play with interrupt priorities, and those functions can no longer hang indefinitely if called from an elevated ISR (not that one should really be printing to serial while in an ISR), like classic AVRs - they will spin and busywait as needed.
* Fix build.extra_flags missing entries on in 2.1.x on 24-pin parts with Optiboot.
* Move version defines to platform.txt, in hopes of reducing incorrect and inconsistent version numbers in releases... (at least now the version number from {version} and the MEGATINYCORE/MEGATINYCORE_* defines will match!)
* Correct issue with enabled/sampled BOD mode with slow sampling.
* Include all SUT options in tools submenu, correct issue with 8ms SUT (it was actually setting it to 4ms).
* Include serialevent and clocksource setting in name of exported .hex and .lst files.
* Add the `Serial.printHex()` functions from DxCore; `Serial.printHex()` can be called with a 1, 2, or 4 byte integer to print out it's value in hexadecimal, with leading zero(s). For 2 and 4 byte integers, optional boolean second argument; if true, will swap the order of the two bytes (endianness); can also be called with a pointer to a uint8_t or uint16_t, a length (uint8_t, ie, max length 255), and, optionally, a separator (a single char/int8_t); this will print that many bytes as hex starting at the given pointer; if it's a pointer to a uint16_t, there's the same optional byte order boolean at the end. If you need to use that, but don't want a separator, pass 0 as the third argument.
* Merge in timing fix for TWI in slave mode (#235)

### 2.1.1, 2.1.2, 2.1.3
Urgent bugfixes for critical regressions introduced in 2.1.0.

### 2.1.0
* Improve ADC accuracy when switching references
* Fix bug with which parts we thought had the external reference, corrected docs. (#211)
* Change handling of reset cause in bootloader, see #213.
* Correct bootloader for 8-pin parts with non-swapped serial port to use PIN_PA3 for the blink LED instead of PIN_PA7 which is used when the serial port is on the alternate pins. PA7 is one of the serial pins, and hence is not available for the Optiboot triple blink if the serial port is not swapped. Swapped serial port still used PA7, as that matches a) all other megaTinyCore boards and b) the initial versions of the hardware I sell.
* Fix programmer options (#232 and others)
* Fix EEPROM library, no more USERROW - this is just the standard EEPROM library. (#200, #168)
* Add USERSIG library for writing to the USERROW (also called USER_SIGNATURE)
* Writing to flash from app now works on Optiboot parts (Thanks @WestFW) (#212, #233)
* Add options for SUT, BOD sampling frequency (#202)
* As part of above, rolled all the bod mode options into one menu; this rules out nonsensical options (ex, enabled hold wake until BOD started, when BOD is not disabled in sleep, or more stringent BOD while in sleep)
* Wire no longer tries turning on pullups; this caused problems when switching between master and slave, and the internal pullups just aren't strong enough for this! (#223)
* Moved USE_TIMERD_PWM to the variant files instead of boards.txt
* Corrected maximum sketch size for parts with less than 24 pins when using Optiboot.
* Added support for reprogrammming UPDI pin on non-Optiboot board definitions (requires HV UPDI)
* Added support for external 32.768 kHz oscillator as millis source (#238)
* Actually fix bug with names of exported binaries
* In order to maintain compatibility, F() macro now behaves just the same as on official boards. This comes at a cost of performance and size, but allows interoperability with major libraries (#216)
* Fixed bug with millis conversion with RTC (any) as clock source (#327)
* Added support for external clock sources, including ones that attempt to overclock the device.
* Added support for ignoring SerialEvent - this little used and ill-conceived feature imposes a significant cost in time on all parts whether they use it or not.
* Pulled in latest logic library (#224). Did not bring in all the examples, which were really repetitive, and added some new ones that are super cool...
* Added ModernRevSerial example under megaTinyCore "library" to get silicon rev and serial number and print it to console.

### 2.0.5
* Internal change to ADC initialization (saves a bit of flash) and init_ADC1() function for parts that have ADC1.
* Quick fix to naming of .hex and .lst files

### 2.0.4
* Switch to new and improved compiler toolchain - now get informative errors if sketch is too big on parts that use RJMP, bugfix for eeprom.h (not to be confused with EEPROM.h), various header improvements (board manager install only).
* Improve naming of exported binary and assembler listings - now includes every setting that could impact the output!
* Fix failure to export compiled binary on some linux platforms (#201)
* Massive doc improvements.
* Improve backwards compatibility of Wire.h (#203)
* Fix strange bug in EEPROM.h that was somehow missed
* Mark unqualified BOD settings that were removed from datasheet as unofficial, add warning that they may not work correctly (but suspect will be close - they were working for me before I realized they were unsupported! they were listed in io.h before!), and add Microchip's guaranteed operating speeds + BOD levels per datasheet to documentation.
* Possibly fix #189!
* Fix problem with millis not being entirely disabled when set to be disabled.
* Ever so slightly improve baud rate accuracy, reduce space taken by `Serial.begin()` by a few bytes.
* Fix compile error from Tone() on parts without a second type B timer (ie, everything not a 1614, 3216, 1616, 3217, or 1617) when TCB0 was selected as a millis source. (part of #189)
* Correct bug in `micros()` timekeeping when TCA0 is used as `millis()` timing source (introduced in 1.1.9 - after the exhaustive testing for these sorts of issues) (#189)

### 2.0.2
* Fix bug with 1MHz system clock with TCB as milis source (micros was broken)
* Remove EXTERNAL_EXPERIMENTAL - newer atpack removes ambiguity (note - we don't *use* it yet, but we didn't really need to either).
* Fix part family defines
* Add MEGATINYCORE_SERIES definition
* `analogReadResolution()` added
* Wire buffer size (broken in 2.0.0) fixed in very general way, with cases for a wide range
* fix tinyNeoPixel on real WS2812's (it previously was busted on real ones, only worked with SK6812 clones) and unsplit the libraries, because architecture improvements make that possible!

### 2.0.1
* Fix bug with some 1-series parts not compiling because of references to external reference introduced by 1.1.10
* Add an EXTERNAL_EXPERIMENTAL reference option, since the headers and datasheets disagree about what parts support this
* Fix bug in DAC introduced by 2.0.0
* Switch to 3 separate tinyNeopixel libraries instead of one, and change the examples and documentation accordingly.
* Renumber interrupt modes for consistency with MegaCoreX
* Correct `EESAVE` fuse settings for optiboot boards - it is now always off. With optiboot on the chip, it is likely that the only time chip erase is executed is when UPDI is used to erase the chip to reburn the bootloader, and users would probably expect that to wipe the EEPROM too.

### 2.0.0
* Remove all the UART/SPI/I2C pin mapping menus from tools submenus. Instead, use the newly added .swap() and .pins() methods on these objects to set the pins to be used.
* WARNING: POTENTIALLY BREAKING CHANGE: **The default pins used for Serial on 8-pin parts in previous versions are not the "default" pins per datasheet (arduino pins 0 and 1); instead, the "alternate" pins were used by default (arduino pins 2 and 3). Note that on the Rev. - and Rev. A breakouts for these parts from my Tindie store, the serial lines from the FTDI header go to the alternate pins, not the "default" ones.** (this will be corrected in Rev. B of the board). If you have sketches/hardware using this, you will either need to move connections, or add Serial.swap(1); before calling `Serial.begin()`. I realize this is inconvenient, but that previous behavior should never have been the case, and, having finally accepted the fact, it was better to cut over quickly than let more people get used to the previous behavior and then change it later.
* Improve ADC speed dramatically (it runs in about a quarter of the time it used to!) - I do not expect this to cause any issues with accuracy. The megaavr parts support much higher maximum ADC clock compared to the classic AVRs. We now set the ADC clock near to the top of it's range. In order to prevent this from hurting accuracy when reading high impedance sources, the ADC0.SAMPCTRL is set to SAMPLEN=14 instead of 0. This means samples will be taken for 16 ADC clocks instead of 2. Since the ADC clock is 8 times faster now, this should result in the same sampling time. See the ADC section for more information, including how to get even faster ADC readings from low impedance signals.
* `digitalRead()`, `pinMode()`, and `digitalWrite()` were changed back to operating on uint8's instead of the PinMode, PinStatus, etc enums like the official megaavr core does (and unlike how every other core does it). Using the enums, while it was defensible from a software architecture perspective, caused a lot of breakage of common Arduino idioms that have been in widespread use for ages, for very little benefit. This also applies to things that used BitOrder.
* `digitalRead()`, `pinMode()`, `digitalWrite()` and `analogWrite()` now take advantage of the unified memory architecture of the megaavr parts to improve performance and reduce flash usage by removing the `PROGMEM` attribute and accompanying pgm_read_byte() calls. This yields a significant improvement in performance of `analogWrite()` and `digitalRead()` in particular.
* Remove the DAC reference voltage from tools submenu. Now, use the DACReference() function - it can take any of the `INTERNAL` reference arguments that would be passed to `analogReferece()`.
* `digitalRead()` no longer turns off PWM and DAC output on the pin that is read. There was no technical need for this, and `digitalRead()` should not change the pin output state!
* `digitalRead()` now returns an int8_t instead of an int16_t - this saves a tiny amount of flash and slightly improves execution time.
* `digitalRead()` now returns -1 if called on a pin that doesn't exist, instead of 0 (LOW). This should make debugging easier, without impacting behavior when valid pin is passed to `digitalRead()`.
* Added support for manipulating the millis timer from within libraries or the sketch: init_millis(), stop_millis(), set_millis(), and restart_millis(). These are not expected to be normally used in sketches; these will be used in the upcoming megaTinySleep library which will "switch" millis timekeeping to the RTC when in sleep, and restore the millis value to whatever other timer is normally used.
* Fix a bug with the `EXTERNAL` reference option being defined for the '212 and '412 - the 8-pin parts do not have that reference option, even if they're 1-series and otherwise would

### 1.1.10
* Fix bug with Wire introduced by not testing 1.1.9 changes to Wire.
* Fix bug with EEPROM introduced by not testing 1.1.9 changes to EEPROM
* Add ability to read from temp sensor, internal reference via ADC, clean up `analogReference()`
* Add some example sketches, including reading temp and Vc

### 1.1.9
* Correct `micros()` results at 20, 10, and 5 MHz when TCA0 or TCD0 is used as millis source
* Correct `micros()` and `millis()` long term drift at 20, 10, and 5 Mhz when TCD used as millis source
* Reduce time for `micros()` to return in many situations
* Correct PWM duty cycles when TCD0 is used for PWM
* Add support for PWM on TCD0 when it is used as millis source
* Adjust TCBm period when used for millis timekeeping with 1MHz system clock to reduce time spend in the millis ISR (reduces millis resolution to 2ms)
* Lower prescaler on TCA0 to 16 when system clock is 4 Mhz or 5 MHz and 8 when running at 1 MHz (was 64)
* Increase prescaler on TCD0 to 64 when used as `millis()` source with 1 MHz system clock (it still runs from unprescaled 20/16 MHz oscillator) in order to reduce portion of time spent in the ISR. Previously it spent more than 12% of it's time in the ISR.
* `micros()` times returned are now as close to the time when `micros()` was called as possible
* Move millis interrupt to HUNF instead of LUNF when TCA0 is used for timing
* Don't prescale TCB clock in Servo at 10MHz or lower for smoother output
* Correct TRIM_DURATION in Servo library
* Add support for writing to the user row using EEPROM.h
* Add support for receiving general call messages in slave mode using the TWI peripheral through the Wire library (Thanks @LordJakson - #146 )
* Add support for second address or masking off bits of the address using the TWI0.SADDRMSK register
* Added support for the Atmel embedded debugger on the explained pro kits as programmer (thanks @rneurink #149 )
* Lots of expanded documentatio

### 1.1.8
* Fix critical bug introduced in 1.1.7 that prevented compilation with TCA0 as millis source

### 1.1.7
* Make Servo library work independently of TCA0 prescaler
* Fix bug that prevented compilation with RTC as millis source
* Improve TCBn as timekeeping source - it is now the best if you can spare a type B timer
* Fix tone not moving to TCB1 when millis using TCB0
* Improve compile errors if TCB0 used as millis and you try to use tone
* Remove dead code relating to TCB2, TCB3 - nothing here has them, why do we have that code here?

### 1.1.6
* Remove option for RTC using external crystal on 412/212/402/202 - these do not support it.
* Correct issue with gibberish if using Serial just after `Serial.begin()`
* Fix issue with disabling DAC on 14-pin parts (#141)
* Fix issue with missing option to disable DAC on 20-pin optiboot parts.
* Fix issue with `pulseIn()` giving incorrect results. (#142)
* Add support for PA0 (UPDI) as IO pin to Optiboot boards (#150)
* Fix issues with Logic library and examples (#106)
* Remove dependence of `tone()` and TCB as millis/micros source TCA0 prescaler (#144)
* Documentation improvement

### 1.1.5
* Reduce flash used by millis/micros() (#135)
* Undo change that broke #130
* Eliminate variants.c (#40)
* Fix issue with disabling millis not working correctly
* Add option to use RTC as millis timer (#133)
* Correct and update part-specific doc

### 1.1.4
* Correct issue introduced with 1.1.3 that broke compile for 20-pin parts (#134)
* Correct issue when Timer D is specified explicitly as the millis timer
* Correct issue when Timer D is used as the millis timer when clock speed is not 20/10/5MH

### 1.1.3
* Fix issue with PWM duty cycle and analogWrite(pin,255) (#123)
* Fix issue with some UPDI programmers (#125, #126)
* Reduce flash usage of all parts slightly (#127)
* Reduce flash usage of Wire library significantly (#128, #129, #130)
* Add DAC disabled option (saves flash)
* Fix issue with UART baud rate calculation (#131)
* Add menu option to control timer used for millis/micros (#124, #132)
* Default millis timer for 412,212,1614,814,414,214 changed to TCD0, as it's not used for anything else (#124)

### 1.1.2
* Fix critical issue compiling for 24-pin parts (#118)
* Fix issue with eesave menu option (#117)
* Fix issue with PWM on PA3 on 14-pin devices. (#115)
* Remove unused {upload.extra_files} parameter from platform.txt upload recipe.

### 1.1.1
* Fix issue with board manager installation (#111)
* Add support for printf to printable library (#112)
* Add menus for I2C and SPI pin mapping (#110)
* Correct bug with UART pin mapping menu and xy4/xy6/xy7 optiboot board definitions
* Correct bug with digitalPinHasPWM macro on xy2.
* Update datasheet links to point to latest dataheet version. This *should* get rid of all the preliminary datasheet links.
* Add row in entry in part-specific doc pages for spxzz bootloader for optiboot board def

### 1.1.0
* Add Logic library (#103)
* Improve pinout diagrams (#98)
* Add support for the Optiboot bootloader !!! (#52)
* Correct sketch size output to account for const variables (#95)
* Fix `EESAVE` option (which was backwards - #93)
* Fix Onboard mEDBG programmer for ATTiny416 Xplained Nano (#96)
* Add menu options for UART location on all parts (#108)

### 1.0.6
* Add missing NUM_DIGITAL_PINS macros to xy7, xy6.
* Fix PWM on Pin 1 (PA7) of xy2
* Fix Wire.h to prevent "call of overloaded 'requestFro&, unsigned int&)' is ambiguous" errors with code that works on classic avr.
* Expand documentation
* Tools -> Programmer selection now used for normal upload

### 1.0.5
* Support more PWM pins on 412/212/402/202
* Support both UART locations on 412/212/402/202

### 1.0.4
* Do not specify version of AVRDude in platform.txt (#73)
* Board manager installation improvements to prevent breaking USBTinyISP on other installed core

### 1.0.3
* Fix UART (and I suspect I2C) on 412/402 and general `PORTMUX` initialization.
* Add tinyNeoPixel and tinyNeoPixel_Static libraries, examples and documentation, add menu option to select port at 8/10MHz (saves flash)
* Pinout chart correction

### 1.0.2
* Fix `analogRead()`, which was broken on most pins
* Fix A11 on ATtiny x04 and x14 parts
* Fix 1604 (compile and upload both had separate and unrelated issues)
* Board manager installation will no longer require official megaAVR board package to be installed
* Correct analog pin macros (these don't appear to be used anywhere, but may be used by third party code)
* Add workaround for t402's with bad signature
* Improve consistency in boards.tx

### 1.0.1
* Improve TCD0-based PWM pins on parts that have them
* Implement DAC output on 1-series
* Implement Servo library
* Add a number of useful #defines
* Fix minor typos
* Move LED_BUILTIN to PA7 on all parts. The exact number of this pin will depend on the version of megaTinyCore in use
* Improve and expand documentatio

### 1.0.0
* Initial Release
