# I/O header files
These are the I/O header files from the latest ATpack available at the time of writing, the Atmel ATtiny Series Device Support v1.10.348 package, corresponding to all 0-series, 1-series and 2-series parts. Files specific to parts not supported by megaTinyCore are not included. These files are included in the documentation largely because *all post 2016 AVR devices basically require you to have both the datasheet and IO header open* when writing any sort of meaningful code (contrast with older parts, where you needed only the datasheet - the longer names for everything on the modern AVRs are far more readable, and I agree with their decision to pursue the new naming scheme, and these parts are all an incredible improvement; so I don't mean to complain.... but would it have killed them to put the actual word-for-word names for registers/bitfields/etc to copy/paste, so we don't need to keep the header file open too? If you're going to extend the length of the names of bitfields to make for more readable code, that also makes them difficult to type with perfect spelling, capitolization, and underscores in the right places, such that basically everyone will copy-paste them.)

I am redistributing these solely as a convenience to users; particularly for less experienced users in the Arduino world, it turns out that being told to look at the header file is a bit daunting ("I found a bunch of iosomething.h files, but not the one for the chip I'm using" "In the Arduino folder? Oh no that's the version that the IDE came with, the core installed a new one..." "well where is it installed to?" "Okay so its in c:/users/(yourusername)/AppData/Arduino15/packages (so on and so forth), but AppData is hidden..." "Uhhhh.... okay can't I just go download it" "Oh of course, you can download it from Microchip - download the file from this page. Then change the file extension to .zip - the download has the .atpack extension, even though it's just a zip file")

The ATpack files are all available from [http://packs.download.atmel.com/](http://packs.download.atmel.com/) They are licensed by Microchip under the Apache license.

The files in this location are NOT used by the core in any way, nor are they included in the board manager releases (the copy of the headers that are used are in `(toolchain root)/avr/include/avr`, where the root of the toolchain installation depends on your OS, how Arduino was installed, and the version of the core). The extras folder is not included in board manager releases at all. Nobody wants to have to dig up files installed via board manager, whether it's to find a header used by the compiler, a pinout chart, or the core documentation (the .md files are best read on github, anyway)

## Header file license
**These files are not part of the core and I had no hand in their development**

This is the license notice displayed when downloading these files. It (and only it) applies to all files in this directory with the exception of README.md itself.

```text
Copyright (c) 2020 Microchip Technology Inc.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the Licence at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
```


## Representative Headers
These examples are from the highest pincount version of each series. This much smaller handful of files is enough to find everything you'd need to copy/paste into your code.
* [1607 - good for any 0-series](iotn1607.h)
* [817 - good for any 1-series with less than 16k of flash](iotn817.h)
* [3217 - good for any 1-series with at least 16k of flash](iotn3217.h)
* [1627 - good for any 2-series](iotn1627.h)

## The header files, by flash size
* [ATtiny3216 - iotn3216.h](iotn3216.h)
* [ATtiny3217 - iotn3217.h](iotn3217.h)
* [ATtiny3224 - iotn3224.h](iotn3224.h)
* [ATtiny3226 - iotn3226.h](iotn3226.h)
* [ATtiny3227 - iotn3227.h](iotn3227.h)
* [ATtiny1606 - iotn1606.h](iotn1606.h)
* [ATtiny1607 - iotn1607.h](iotn1607.h)
* [ATtiny1614 - iotn1614.h](iotn1614.h)
* [ATtiny1616 - iotn1616.h](iotn1616.h)
* [ATtiny1617 - iotn1617.h](iotn1617.h)
* [ATtiny1624 - iotn1624.h](iotn1624.h)
* [ATtiny1626 - iotn1626.h](iotn1626.h)
* [ATtiny1627 - iotn1627.h](iotn1627.h)
* [ATtiny807  -  iotn807.h](iotn807.h)
* [ATtiny814  -  iotn814.h](iotn814.h)
* [ATtiny816  -  iotn816.h](iotn816.h)
* [ATtiny817  -  iotn817.h](iotn817.h)
* [ATtiny824  -  iotn824.h](iotn824.h)
* [ATtiny826  -  iotn826.h](iotn826.h)
* [ATtiny827  -  iotn827.h](iotn827.h)
* [ATtiny402  -  iotn402.h](iotn402.h)
* [ATtiny404  -  iotn404.h](iotn404.h)
* [ATtiny406  -  iotn406.h](iotn406.h)
* [ATtiny412  -  iotn412.h](iotn412.h)
* [ATtiny414  -  iotn414.h](iotn414.h)
* [ATtiny416  -  iotn416.h](iotn416.h)
* [ATtiny417  -  iotn417.h](iotn417.h)
* [ATtiny424  -  iotn424.h](iotn424.h)
* [ATtiny426  -  iotn426.h](iotn426.h)
* [ATtiny427  -  iotn427.h](iotn427.h)
* [ATtiny202  -  iotn202.h](iotn202.h)
* [ATtiny204  -  iotn204.h](iotn204.h)
* [ATtiny212  -  iotn212.h](iotn212.h)
* [ATtiny214  -  iotn214.h](iotn214.h)

## Header files by series

### 2-series (12-bit ADC w/PGA, 4 CCL blocks)
* [ATtiny3224 - iotn3224.h](iotn3224.h)
* [ATtiny3226 - iotn3226.h](iotn3226.h)
* [ATtiny3227 - iotn3227.h](iotn3227.h)
* [ATtiny1624 - iotn1624.h](iotn1624.h)
* [ATtiny1626 - iotn1626.h](iotn1626.h)
* [ATtiny1627 - iotn1627.h](iotn1627.h)
* [ATtiny824  -  iotn824.h](iotn824.h)
* [ATtiny826  -  iotn826.h](iotn826.h)
* [ATtiny827  -  iotn827.h](iotn827.h)
* [ATtiny424  -  iotn424.h](iotn424.h)
* [ATtiny426  -  iotn426.h](iotn426.h)
* [ATtiny427  -  iotn427.h](iotn427.h)

### Golden 1-series (dual 10-bit ADC. DAC. TCD, 3x AC)
* ATtiny3214 didn't make it into production
* [ATtiny3216 - iotn3216.h](iotn3216.h)
* [ATtiny3217 - iotn3217.h](iotn3217.h)
* [ATtiny1614 - iotn1614.h](iotn1614.h)
* [ATtiny1616 - iotn1616.h](iotn1616.h)
* [ATtiny1617 - iotn1617.h](iotn1617.h)


### Lesser 1-series (single 10-bit DAC, TCD, 1x AC)
* [ATtiny814  -  iotn814.h](iotn814.h)
* [ATtiny816  -  iotn816.h](iotn816.h)
* [ATtiny817  -  iotn817.h](iotn817.h)
* [ATtiny412  -  iotn412.h](iotn412.h)
* [ATtiny414  -  iotn414.h](iotn414.h)
* [ATtiny416  -  iotn416.h](iotn416.h)
* [ATtiny417  -  iotn417.h](iotn417.h)
* [ATtiny212  -  iotn212.h](iotn212.h)
* [ATtiny214  -  iotn214.h](iotn214.h)

### 0-series (for budget applications)
* [ATtiny1604 - iotn1604.h](iotn1604.h)
* [ATtiny1606 - iotn1606.h](iotn1606.h)
* [ATtiny1607 - iotn1607.h](iotn1607.h)
* [ATtiny807  -  iotn807.h](iotn807.h)
* [ATtiny402  -  iotn402.h](iotn402.h)
* [ATtiny404  -  iotn404.h](iotn404.h)
* [ATtiny406  -  iotn406.h](iotn406.h)
* ATtiny407 does not exist and was never even mentioned
* [ATtiny202  -  iotn202.h](iotn202.h)
* [ATtiny204  -  iotn204.h](iotn204.h)


## From the Manufactuer's Changelog:
(note: the ATpacks contain the headers, but also a ton of other stuff - "device-specs" and precompiled "libattiny_____.a" (a precompiled library of standard functions) and crtattiny.o (precompiled C runtime - roughly speaking, is a template for the linker to fill files for the standard libraries on different architectures, support files for their proprietary XC8 compiler, and support files for simulation tools - that's what they refer to when things don't make sense in the context of IO headers)
```
1.10.348 (2021-07-13) Corrected AVR-LibC device macro for ATtiny84A. Coprrected SRAM on ATtiny214 simulator model.
1.9.337 (2021-03-01)  Added support for ATtiny322x. Changed nomenclature for serial communication. Fixed mapping of RAM addresses 0x3C00-0x3FFF for ATtiny1617/1616/1614 RTL-based simulator model. Removed external 32kHz crystal option from ATtiny160x/80x/40x/20x. Added SYSCFG0.TOUTDIS for ATtiny42x/82x/162x. Added value NONE (0x00) to TCA0.SINGLE.EVCTRL EVACTB. Added CCL TRUTH bitfields for several devices. Joined CCL_SEQSEL0 and CCL_SEQSEL1 into one value-group, CCL_SEQSEL, for ATtiny42x/82x/162x.
1.8.332 (2020-10-30)  Updated conditions in .pdsc for XC8 compiler.
1.7.330 (2020-09-18)  Added support for ATtiny42x and ATtiny82x. Added missing CCL INSEL options for ATtiny161x and ATtiny321x. Corrected AC DACREF bitfield name for ATtiny162x. Added PAGE_SIZE for memories in several assembler include files.
1.6.326 (2020-08-27)  Added XC8 support files. Updated copyright notice in c-header files. Corrected ADC MUXPOS and MUXNEG values, corrected PORTMUX.TCAROUTEA and PORTMUX.TCAROUTEB bitfields, added more values to other PORTMUX bitfields and renamed the ADC DAC source to DACREF0 for ATtiny162x devices.
1.5.315 (2020-06-04)  Corrected MAPPED_ROGMEM rw-attribute for AVR8X devices. Timer/Counter caption correction. Corrected PORTMUX.EVSYSROUTEA bitfield names for ATtiny162x devices. Updated ATtiny162x simulator model.
1.4.310 (2020-04-03)  Corrected analog functionality for PB1 and PB0 in ATtiny1627/1626/1624 ATDF files.
1.4.308 (2020-03-16)  Corrected ADC modes for tiny1627/1626/1624. Corrected USART CTRLC reset value for ATtiny161x, ATtiny4xx and ATtiny2xx. Added LOCKBITS_DEFAULT for AVR8X devices.
1.4.301 (2020-01-28)  Added ATtiny1627/1626/1624. Updates and corrections to CORE_VERSION. Removed unqualified BOD level values. Remove TCD0 from Attiny1604/1606/1607
1.4.283 (2019-10-18)  Corrected TC2 instance in ATtiny441. Corrected defines for ATtiny1634. Refactored away family header files, now there is one header file per device. Corrected FUSE_MEMORY_SIZE for ATtiny4/5/9/10/20/40. Added simulator support for ATmega1609/1608/809/808. Added missing TCCR1B.PWMX bit for ATtiny261/261A/461/461A/861/861A. Updated fuse information for ATtiny-0/1 families. Added TCB1 in PORTMUX for ATtiny1614/1616/1617/3216/3217. In assembly include files list addresses with HEX and corrected interrupt vector offsets. Removed duplication of PROGMEM defines. Added support for pseudo ISR instruction in avr8-gcc. Group adjacent reserved bytes in arrays in the header files. Corrected PA1 and PA2 pad connection for ATtiny202/212/402/412.
1.3.229 (2018-09-17)  Corrected signatures for ATtiny402 and ATtiny412. Typo corrections in captions. Corrected RW status on MCLKSTATUS register. Added initval-attribute for most registers in several devices. Made headerfile size defines unsigned. Corrected flash pagesizes in assembler include files for several devices.
1.3.172 (2018-02-19)  Added TWSSRA bitfields for ATtiny20 and ATtiny40. Corrected Fuse register group size for ATtiny1607-family, ATtiny1617-family and ATtiny3217-family.
1.3.169 (2017-12-18)  Added support for ATtiny1607, ATtiny1606, ATtiny1604, ATtiny807, ATtiny806, ATtiny804, ATtiny406, ATtiny404, ATtiny402, ATtiny204 and ATtiny202.
1.3.147 (2017-10-02)  Updated AVR8X simulator models.
1.3.132 (2017-07-04)  Added CCP signature for ATtiny102 and ATtiny104. Renamed interrupt ADC_ADC to ADC for ATtiny40. Renamed interrupt ANA_COMP to ANALOG_COMP for ATtiny48 and ATtiny88. Added Hi-/Lo-byte-address defines for 16-bit registers in assembler include files for the new tiny's. Added simulator model for ATtiny3217, ATtiny3216 and ATtiny3214.
1.2.118 (2017-02-21)  Added support for ATtiny3217, ATtiny3216, ATtiny3214, ATtiny1617, ATtiny1616, ATtiny1614, ATtiny416, ATtiny414, ATtiny412, ATtiny214 and ATtiny212.
1.2.112 (2016-12-06)  Updated header files for ATtiny417, ATtiny814, ATtiny816 and ATtiny817. Simulator support for the same devices. Added support for ATtiny1617 and ATtiny1616.
1.1.102 (2016-09-29)  Added ATtiny417, ATtiny814, ATtiny816, and ATtiny817. Added GCC assembler support for ATtiny11, ATtiny12, and ATtiny15.
1.0.79 (2016-04-11) Added ATtiny102, ATtiny104, ATtiny80 and ATtiny840. Updated documentation links.
1.0.78 (2016-02-18) Added ATtiny102, ATtiny104, ATtiny80 and ATtiny840. Updated documentation links.
1.0.71 (2015-11-06) Fix simulator load issue for ATtiny85, ATtiny43U, ATtiny88, ATtiny1634, ATtiny45, ATtiny48, ATtiny40, ATtiny25, and ATtiny20.
1.0.70 (2015-10-28) Initial release.
```
