# I/O header files
These are the I/O header files from the latest ATpack available at the time of writing, the Atmel ATtiny Series Device Support v1.9.337 package, corresponding to all 0-series, 1-series and 2-series parts. Files specific to parts not supported by megaTinyCore are not included. These files are included in the documentation largely because *all post 2016 AVR devices basically require you to have both the datasheet and IO header open* when writing any sort of meaningful code (contrast with older parts, where you needed only the datasheet - the longer names for everything on the modern AVRs are far more readable, and I agree with their decision to pursue the new naming scheme, and these parts are all an incredible improvement; so I don't mean to complain.... but would it have killed them to put the actual word-for-word names for registers/bitfields/etc to copy/paste, so we don't need to keep the header file open too? If you're going to extend the length of the names of bitfields to make for more readable code, that also makes them difficult to type with perfect spelling, capitolization, and underscores in the right places, such that basically everyone will copy-paste them.)

I am redistributing these solely as a convenience to users; particularly for less experienced users in the Arduino world, it turns out that being told to look at the header file is a bit daunting ("I found a bunch of iosomething.h files, but not the one for the chip I'm using" "In the Arduino folder? Oh no that's the version that the IDE came with, the core installed a new one..." "well where is it installed to?" "Okay so its in c:/users/(yourusername)/AppData/Arduino15/packages (so on and so forth), but AppData is hidden..." "Uhhhh.... okay can't I just go download it" "Oh of course, you can download it from Microchip - download the file from this page. Then change the file extension to .zip - the download has the .atpack extension, even though it's just a zip file")

The ATpack files are all available from [http://packs.download.atmel.com/](http://packs.download.atmel.com/) They are licensed by Microchip under the Apache license.

The files in this location are NOT used by the core in any way, nor are they included in the board manager releases (the copy of the headers that are used are in `(toolchain root)/avr/include/avr`, where the root of the toolchain installation depends on your OS, how Arduino was installed, and the version of the core). The extras folder is not included in board manager releases at all. Nobody wants to have to dig up files installed via board manager, whether it's to find a header used by the compiler, a pinout chart, or the core documentation (the .md files are best read on github, anyway)

## Header file license
**These files are not part of the core and I had no hand in their development**
They have been modified only by running codespell over them in order to make the repo pass CI spellcheck.

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
* [ 817 - good for any 1-series with less than 16k of flash](iotn817.h)
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
