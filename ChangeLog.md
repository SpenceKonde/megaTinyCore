1.1.6
* Remove option for RTC using external crystal on 412/212/402/202 - these do not support it.
* Correct issue with gibberish if using Serial just after Serial.begin()
* Fix issue with disabling DAC on 14-pin parts (#141)
* Fix issue with missing option to disable DAC on 20-pin optiboot parts.

1.1.5
* Reduce flash used by millis/micros() (#135)
* Undo change that broke #130
* Eliminate variants.c (#40)
* Fix issue with disabling millis not working correctly
* Add option to use RTC as millis timer (#133)
* Correct and update part-specific docs

1.1.4
* Correct issue introduced with 1.1.3 that broke compile for 20-pin parts (#134)
* Correct issue when Timer D is specified explicitly as the millis timer
* Correct issue when Timer D is used as the millis timer when clock speed is not 20/10/5MHz

1.1.3
* Fix issue with PWM duty cycle and analogWrite(pin,255) (#123)
* Fix issue with some UPDI programmers (#125, #126)
* Reduce flash usage of all parts slightly (#127)
* Reduce flash usage of Wire library significantly (#128, #129, #130)
* Add DAC disabled option (saves flash)
* Fix issue with UART baud rate calculation (#131)
* Add menu option to control timer used for millis/micros (#124, #132)
* Default millis timer for 412,212,1614,814,414,214 changed to TCD0, as it's not used for anything else (#124)

1.1.2
* Fix critical issue compiling for 24-pin parts (#118)
* Fix issue with eesave menu option (#117)
* Fix issue with PWM on PA3 on 14-pin devices. (#115)
* Remove unused {upload.extra_files} parameter from platform.txt upload recipe.

1.1.1
* Fix issue with board manager installation (#111)
* Add support for printf to printable library (#112)
* Add menus for I2C and SPI pin mapping (#110)
* Correct bug with UART pin mapping menu and xy4/xy6/xy7 optiboot board definitions
* Correct bug with digitalPinHasPWM macro on xy2.
* Update datasheet links to point to latest dataheet version. This *should* get rid of all the preliminary datasheet links.
* Add row in entry in part-specific doc pages for spxzz bootloader for optiboot board defs

1.1.0

* Add Logic library (#103)
* Improve pinout diagrams (#98)
* Add support for the Optiboot bootloader !!! (#52)
* Correct sketch size output to account for const variables (#95)
* Fix EESAVE option (which was backwards - #93)
* Fix Onboard mEDBG programmer for ATTiny416 Xplained Nano (#96)
* Add menu options for UART location on all parts (#108)

1.0.6
* Add missing NUM_DIGITAL_PINS macros to xy7, xy6.
* Fix PWM on Pin 1 (PA7) of xy2
* Fix Wire.h to prevent "call of overloaded 'requestFro&, unsigned int&)' is ambiguous" errors with code that works on classic avr.
* Expand documentation
* Tools -> Programmer selection now used for normal uploads

1.0.5
* Support more PWM pins on 412/212/402/202
* Support both UART locations on 412/212/402/202

1.0.4
* Do not specify version of AVRDude in platform.txt (#73)
* Board manager installation improvements to prevent breaking USBTinyISP on other installed cores

1.0.3 (released)
* Fix UART (and I suspect I2C) on 412/402 and general PORTMUX initialization.
* Add tinyNeoPixel and tinyNeoPixel_Static libraries, examples and documentation, add menu option to select port at 8/10MHz (saves flash)
* Pinout chart corrections

1.0.2 (released)
* Fix analogRead(), which was broken on most pins
* Fix A11 on ATtiny x04 and x14 parts
* Fix 1604 (compile and upload both had separate and unrelated issues)
* Board manager installation will no longer require official megaAVR board package to be installed
* Correct analog pin macros (these don't appear to be used anywhere, but may be used by third party code)
* Add workaround for t402's with bad signature
* Improve consistency in boards.txt


1.0.1
* Improve TCD0-based PWM pins on parts that have them
* Implement DAC output on 1-series
* Implement Servo library
* Add a number of useful #defines
* Fix minor typos
* Move LED_BUILTIN to PA7 on all parts. The exact number of this pin will depend on the version of megaTinyCore in use
* Improve and expand documentation

1.0.0
* Initial Release
