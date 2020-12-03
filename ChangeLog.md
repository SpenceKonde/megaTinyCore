# Changelog
This page documents (nearly) all bugfixes and enhancements that produce visible changes in behavior throughout the history of megaTinyCore. Note that this document is maintained by a human, who is - by nature - imperfect; sometimes the changelog may not be updated at the same time as the changes go in, and occasionally a change is missed entirely in the changelog, though this is rare.

## Changes not yet in release
Changes listed here are checked in to GitHub ("master" branch unless specifically noted; this is only done when a change involves a large amount of work and breaks the core in the interim, or where the change is considered very high risk, and needs testing by others prior to merging the changes with master). These changes are not yet in any "release" nor can they be installed through board manager, only downloading latest code from github will work. These changes will be included in the listed version, though planned version numbers may change without notice - critical fixes may be inserted before a planned release and the planned release bumped up a version, or versions may go from patch to minor version depending on the scale of changes.

### Planned for 2.2.0
* On non-Optiboot configurations, set all fuses that the core sets based on menu options, except BODCFG (control brown-out detection) and SYSCFG0 (controls configuration of reset/UPDI pin, as well as whether EEPROM is retained) on all uploads. Changing the reset pin configuration could "brick" a part if the user does not have a high voltage programmer, so setting this fuse is not "safe"; similarly, setting the BOD voltage higher than the operating voltage and enabling it will prevent programming. Since the device may be soldered to devices that are not tolerant of the >4.3V required by the highest BOD setting, this would also constitute "bricking". Other fuses cannot render the device unprogrammable, so there is no reason not to program them. Judging by the content of recent issues, this is a frequent point of confusion particularly with the clock speed, despite the fact that it has worked this way on literally every Arduino core released since the dawn of Arduino! (On classic AVRs, changing any fuse could potentially "brick" the part, so this was the only safe behavior). There are no menu options to configure WDTCFG or TCD0CFG, so those aren't touched on program, only upon burn bootloader - if you manually changed those, you probably don't want an upload to undo them, whereas "burn bootloader" will return the chip to a known configuration.
* On Optiboot configurations, set all fuses except BODCFG and SYSCFG0 when "upload using programmer" is used (this still removes the bootloader, just like on classic AVRs). Uploads through the bootloader will not set fuses; almost all tools submenu options require "burn bootloader" to change.
* Fix nasty bug with Optiboot entry conditions (#259)
* Add Ersatz Reset example sketch, and bootloader option.
* Correct bug with bootloader version used on 14-24 pin parts when UPDI pin is configured as GPIO.
* add millisClockCyclesPerMicrosecond(); what WAS clockCyclesPerMicrosecond() is now this - differemnce is that the millisClockCyclesPerMicrosecond() gives the number of clock cycles on the timebase for the millis timers (so, for TCD0 as millis clock, the unprescaled internal oscillator, otherwise same as clockCyclesPerMicrosecond()) - apparently other libraries use this for things that depend on the system clock... including my own Servo implementation! (it was tested before that was the default millis source on 3216/3217)... This is *really* nasty when it bites....
* Fix Servo library - at long last! I never realized just *how* broken it was. (#195, #241)
* What the heck? When were people going to tell me about the regression on TCD0 PWM pins?! It just didn't happen... botched refactoring of USE_TCD0_PWM AND regression to code from the bad old days before I knew how to get PWM and millis...(#249)
* Reduced the magnitude of "glitches" possible on PWM pins when turning PWM off. Corrected PWM duty cycle calculations for TCD0 (it was (2 x dutycycle)/511 instead of (2 x dutycycle)/510 - no, it's not *supposed* to be 256ths, though it is very often implemented that way (if you count to 255, you get 256ths, because the timer considered 0 to be one count).
* analogWrite() is now interrupt-safe for TCD0-controlled pins (this may be addressed for the other pins in a future update - however, it is "naturally" interrupt safe except when either the interrupted analogWrite or the interrupt is turning pins on or off - in contrast, TCD0 analogWrite could break in a variety of ways in almost any situation where it was interrupted)
* Also implemented NEW special behavior on the TCD0 PWM pins: analogWrite(pin,0) and analogWrite(pin,255) set pin LOW or HIGH without turning off the PWM. On TCD0, this does not require turning off the timer briefly, which, over time, would cause millis() to lose counts when PWM was also used based on TCD0, in addition to reducing runtime. digitalWrite(), however, does. (also remember that digitalRead(), unlike official cores, NEVER turns off PWM)
* Add check for compile time known invalid pins passed to Arduino digital/analog API functions, analogReference and DACReference, as well as compile time known unachievable baud rates on Serial.begin() for Serial (#269)
* Add support for digitalReadFast(), digitalWriteFast() functions. They are only valid if the pin is compile time known constant, and optimize down to something very fast (when second argument is compile time known, a single cycle for write. Looks like 4 cycles for read?) (#205)


## Released Versions

### 2.1.5
* Fix critical issue with "burn bootloader" when speeds other than 5, 10, or 20 MHz was selected on devices with 8, 20, or 24 pins and 14-pin devices using Optiboot.

### 2.1.4
* Fix critical bug in alternate pins for Serial (#254)
* Improve compatibility regarding build.board changes with a compatibility layer in case other libraries are checking for the old version. (#253)
* Use build.board pattern consistent with other cores (AVR_ATtiny). (#253)
* In order to reduce flash usage, do not use UART baud correction based on voltage on 2k and 4k parts (it was always close enough for typical use cases anyway), and those parts are very tightly flash constrained.
* Remove UART baud voltage menu for 8-pin parts due to above.
* Automatically use CLKX2 USART option at 2 MHz or less, and on higher speed parts when appropriate for the selected baud rate. Most visible impact of this change is that you can now use 115200 baud at 1 MHz. (#188)
* Do not apply oscillator voltage (in)correction to baud rate when using external clock.
* Reduce flash usage of UART (Serial), particularly on smaller chips, through a great many mechanisms. (#252)
* Serial.print/write/flush will no longer play with interrupt priorities, and those functions can no longer hang indefinitely if called from an elevated ISR (not that one should really be printing to serial while in an ISR), like classic AVRs - they will spin and busywait as needed.
* Fix build.extra_flags missing entries on in 2.1.x on 24-pin parts with Optiboot.
* Move version defines to platform.txt, in hopes of reducing incorrect and inconsistent version numbers in releases... (at least now the version number from {version} and the MEGATINYCORE/MEGATINYCORE_* defines will match!)
* Correct issue with enabled/sampled BOD mode with slow sampling.
* Include all SUT options in tools submenu, correct issue with 8ms SUT (it was actually setting it to 4ms).
* Include serialevent and clocksource setting in name of exported .hex and .lst files.
* Add the Serial.printHex() functions from DxCore; Serial.printHex() can be called with a 1, 2, or 4 byte integer to print out it's value in hexadecimal, with leading zero(s). For 2 and 4 byte integers, optional boolean second argument; if true, will swap the order of the two bytes (endianness); can also be called with a pointer to a uint8_t or uint16_t, a length (uint8_t, ie, max length 255), and, optionally, a separator (a single char/int8_t); this will print that many bytes as hex starting at the given pointer; if it's a pointer to a uint16_t, there's the same optional byte order boolean at the end. If you need to use that, but don't want a separator, pass 0 as the third argument.
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
* Ever so slightly improve baud rate accuracy, reduce space taken by Serial.begin() by a few bytes.
* Fix compile error from Tone() on parts without a second type B timer (ie, everything not a 1614, 3216, 1616, 3217, or 1617) when TCB0 was selected as a millis source. (part of #189)
* Correct bug in micros() timekeeping when TCA0 is used as millis() timing source (introduced in 1.1.9 - after the exhausive testing for these sorts of issues) (#189)

### 2.0.2
* Fix bug with 1MHz system clock with TCB as milis source (micros was broken)
* Remove EXTERNAL_EXPERIMENTAL - newer atpack removes ambiguity (note - we don't *use* it yet, but we didn't really need to either).
* Fix part family defines
* Add MEGATINYCORE_SERIES definition
* analogReadResolution() added
* Wire buffer size (broken in 2.0.0) fixed in very general way, with cases for a wide range
* fix tinyNeoPixel on real WS2812's (it previously was busted on real ones, only worked with SK6812 clones) and unsplit the libraries, because architecture improvements make that possible!

### 2.0.1
* Fix bug with some 1-series parts not compiling because of references to external reference introduced by 1.1.10
* Add an EXTERNAL_EXPERIMENTAL reference option, since the headers and datasheets disagree about what parts support this
* Fix bug in DAC introduced by 2.0.0
* Switch to 3 separate tinyNeopixel libraries instead of one, and change the examples and documentation accordingly.
* Renumber interrupt modes for consistency with MegaCoreX
* Correct EESAVE fuse settings for optiboot boards - it is now always off. With optiboot on the chip, it is likely that the only time chip erase is executed is when UPDI is used to erase the chip to reburn the bootloader, and users would probably expect that to wipe the EEPROM too.

### 2.0.0
* Remove all the UART/SPI/I2C pin mapping menus from tools submenus. Instead, use the newly added .swap() and .pins() methods on these objects to set the pins to be used.
* WARNING: POTENTIALLY BREAKING CHANGE: **The default pins used for Serial on 8-pin parts in previous versions are not the "default" pins per datasheet (arduino pins 0 and 1); instead, the "alternate" pins were used by default (arduino pins 2 and 3). Note that on the Rev. - and Rev. A breakouts for these parts from my Tindie store, the serial lines from the FTDI header go to the alternate pins, not the "default" ones.** (this will be corrected in Rev. B of the board). If you have sketches/hardware using this, you will either need to move connections, or add Serial.swap(1); before calling Serial.begin(). I realize this is inconvenient, but that previous behavior should never have been the case, and, having finally accepted the fact, it was better to cut over quickly than let more people get used to the previous behavior and then change it later.
* Improve ADC speed dramatically (it runs in about a quarter of the time it used to!) - I do not expect this to cause any issues with accuracy. The megaavr parts support much higher maximum ADC clock compared to the classic AVRs. We now set the ADC clock near to the top of it's range. In order to prevent this from hurting accuracy when reading high impedance sources, the ADC0.SAMPCTRL is set to SAMPLEN=14 instead of 0. This means samples will be taken for 16 ADC clocks instead of 2. Since the ADC clock is 8 times faster now, this should result in the same sampling time. See the ADC section for more information, including how to get even faster ADC readings from low impedance signals.
* digitalRead(), pinMode(), and digitalWrite() were changed back to operating on uint8's instead of the PinMode, PinStatus, etc enums like the official megaavr core does (and unlike how every other core does it). Using the enums, while it was defensible from a software architecture perspective, caused a lot of breakage of common Arduino ideoms that have been in widespread use for ages, for very little benefit. This also applies to things that used BitOrder.
* digitalRead(), pinMode(), digitalWrite() and analogWrite() now take advantage of the unified memory architecture of the megaavr parts to improve performance and reduce flash usage by removing the PROGMEM attribute and accompanying pgm_read_byte() calls. This yields a significant improvement in performance of analogWrite() and digitalRead() in particular.
* Remove the DAC reference voltage from tools submenu. Now, use the DACReference() function - it can take any of the INTERNAL reference arguments that would be passed to analogReferece().
* digitalRead() no longer turns off PWM and DAC output on the pin that is read. There was no technical need for this, and digitalRead() should not change the pin output state!
* digitalRead() now returns an int8_t instead of an int16_t - this saves a tiny amount of flash and slightly improves execution time.
* digitalRead() now returns -1 if called on a pin that doesn't exist, instead of 0 (LOW). This should make debugging easier, without impacting behavior when valid pin is passed to digitalRead().
* Added support for manipulating the millis timer from within libraries or the sketch: init_millis(), stop_millis(), set_millis(), and restart_millis(). These are not expected to be normally used in sketches; these will be used in the upcoming megaTinySleep library which will "switch" millis timekeeping to the RTC when in sleep, and restore the millis value to whatever other timer is normally used.
* Fix a bug with the EXTERNAL reference option being defined for the '212 and '412 - the 8-pin parts do not have that reference option, even if they're 1-series and otherwise would

### 1.1.10
* Fix bug with Wire introduced by not testing 1.1.9 changes to Wire.
* Fix bug with EEPROM introduced by not testing 1.1.9 changes to EEPROM
* Add ability to read from temp sensor, internal reference via ADC, clean up analogReference()
* Add some example sketches, including reading temp and Vc

### 1.1.9
* Correct micros() results at 20, 10, and 5 MHz when TCA0 or TCD0 is used as millis source
* Correct micros() and millis() long term drift at 20, 10, and 5 Mhz when TCD used as millis source
* Reduce time for micros() to return in many situations
* Correct PWM duty cycles when TCD0 is used for PWM
* Add support for PWM on TCD0 when it is used as millis source
* Adjust TCBm period when used for millis timekeeping with 1MHz system clock to reduce time spend in the millis ISR (reduces millis resolution to 2ms)
* Lower prescaler on TCA0 to 16 when system clock is 4 Mhz or 5 MHz and 8 when running at 1 MHz (was 64)
* Increase prescaler on TCD0 to 64 when used as millis() source with 1 MHz system clock (it still runs from unprescaled 20/16 MHz oscillator) in order to reduce portion of time spent in the ISR. Previously it spent more than 12% of it's time in the ISR.
* micros() times returned are now as close to the time when micros() was called as possible
* Move millis interrupt to HUNF instead of LUNF when TCA0 is used for timing
* Don't prescale TCB clock in Servo at 10MHz or lower for smoother output
* Correct TRIM_DURATION in Servo library
* Add support for writing to the user row using EEPROM.h
* Add support for receiving general call messages in slave mode using the TWI peripheral through the Wire library (Thanks @LordJakson - #146 )
* Add support for second address or masking off bits of the address using the TWI0.SADDRMSK register
* Added support for the Atmel embedded debugger on the explained pro kits as programmer (thanks @rneurink #149 )
* Lots of expanded documentatio

### 1.1.8
* Fix critical bug introduced in 1.1.7 that prevented compilation with TCA0 as millis sourc

### 1.1.7
* Make Servo library work independently of TCA0 prescaler
* Fix bug that prevented compilation with RTC as millis source
* Improve TCBn as timekeeping source - it is now the best if you can spare a type B timer
* Fix tone not moving to TCB1 when millis using TCB0
* Improve compile errors if TCB0 used as millis and you try to use tone
* Remove dead code relating to TCB2, TCB3 - nothing here has them, why do we have that code here?

### 1.1.6
* Remove option for RTC using external crystal on 412/212/402/202 - these do not support it.
* Correct issue with gibberish if using Serial just after Serial.begin()
* Fix issue with disabling DAC on 14-pin parts (#141)
* Fix issue with missing option to disable DAC on 20-pin optiboot parts.
* Fix issue with pulseIn() giving incorrect results. (#142)
* Add support for PA0 (UPDI) as IO pin to Optiboot boards (#150)
* Fix issues with Logic library and examples (#106)
* Remove dependence of tone() and TCB as millis/micros source TCA0 prescaler (#144)
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
* Fix EESAVE option (which was backwards - #93)
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
* Fix UART (and I suspect I2C) on 412/402 and general PORTMUX initialization.
* Add tinyNeoPixel and tinyNeoPixel_Static libraries, examples and documentation, add menu option to select port at 8/10MHz (saves flash)
* Pinout chart correction

### 1.0.2
* Fix analogRead(), which was broken on most pins
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
