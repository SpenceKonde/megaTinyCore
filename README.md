[![Join the chat at https://gitter.im/SpenceKonde/megaTinyCore](https://badges.gitter.im/SpenceKonde/megaTinyCore.svg)](https://gitter.im/SpenceKonde/megaTinyCore?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

# Core status
* Lots of stuff untested, please help test if you have the hardware!
* Burn bootloader and clock speeds work. Update core and restart IDE BEFORE burning bootloader if you have been using 1.0.0-dev versions prior to 6/22/2019, there was a nasty bug in boards.txt (see #49, very recently fixed) that would brick chips when burning bootloader!
* PWM works on the x16 series. Belived working on other parts. 
* PWM on the Timer D pins (10, 11 on x16, 12,13 on x17) takes 1 ms to turn on or off, and during this time, the other Timer D PWM channel will be disabled. This applies to digitalWrite() or analogWrite of 0 or 255 while it is currently outputting PWM, and analogWrite of 1~254 while the pin is not currently outputting PWM. We are looking at a better solution (#51)
* analogRead works on the x16 series. Belived working on other parts. 
* tone() should work, at least, it compiles on all chips and there's a timer for it to use. Let us know if it doesn't generate tones.
* Serial works (not tested on the x12 and x02 series - these have to use the alternate pin mapping, so am less confident this will work) 
* SPI, TWI untested. 
* Upload uses jtag2updi programmer. Upload using programmer and burn bootloader use the selected programmer. 


# megaTinyCore
Arduino core for the new megaAVR ATtiny series chips - highly experimental. These parts represent the new megaAVR technology (as used in megaAVR chips like the ATmega4809 as used on Nano Every and Uno Wifi Rev. 2) and advanced peripherals in low-cost, small packages of the ATtiny line. 

These use a UPDI programmer, not traditional ISP like the classic ATtiny parts did. One can be made from a classic AVR Uno/Nano/Pro Mini - see [Making a UPDI programmer](MakeUPDIProgrammer.md)

## Supported Parts
* [ATtiny3217,1617,817](megaavr/extras/ATtiny_x17.md)
* [ATtiny3216,1616,816,416](megaavr/extras/ATtiny_x16.md)
* [ATtiny1614,814,414,214](megaavr/extras/ATtiny_x14.md)
* [ATtiny412,212](megaavr/extras/ATtiny_x12.md)
* [ATtiny1607,807](megaavr/extras/ATtiny_x07.md)
* [ATtiny1606,806,406](megaavr/extras/ATtiny_x06.md)
* [ATtiny1604,804,404,204](megaavr/extras/ATtiny_x04.md)
* [ATtiny402,202](megaavr/extras/ATtiny_x02.md)

## Supported Clock Speeds
* 20MHz Internal (4.5v~5.5v - typical for 5v systems)
* 16MHz Internal (4.5v~5.5v - typical for 5v systems)
* 10MHz Internal (2.7v~5.5v - typical for 3.3v systems)
* 8MHz Internal (2.7v~5.5v - typical for 3.3v systems)
* 5MHz Internal (1.8v~5.5v)
* 4MHz Internal (1.8v~5.5v)
* 1MHz Internal (1.8v~5.5v)

These parts do not support using an external crystal like the classic ATtiny parts do, however the internal oscillator is tightly calibrated enough that the internal clock will work for UART communication. 

# Features

## Assembler Listing generation
Like ATTinyCore, Sketch -> Export compiled binary will generate an assembly listing in the sketch folder; this is particularly useful when attempting to reduce flash usage, as you can see how much flash is used by different functions.

# Known Compiler Bugs
* Sometimes a sketch which is too big to fit will, instead of generating a message saying that, result in the error: 'relocation truncated to fit: R_AVR_13_PCREL against symbol tablejump2'
