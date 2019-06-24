[![Join the chat at https://gitter.im/SpenceKonde/megaTinyCore](https://badges.gitter.im/SpenceKonde/megaTinyCore.svg)](https://gitter.im/SpenceKonde/megaTinyCore?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

# megaTinyCore
Arduino core for the new megaAVR ATtiny series chips - highly experimental

# Rough status: 
* Lots of stuff untested, please help test if you have the hardware!
* Burn bootloader and clock speeds work. Update core and restart IDE BEFORE burning bootloader if you have been using 1.0.0-dev versions prior to 6/22/2019, there was a nasty bug in boards.txt (see #49, very recently fixed) that would brick chips when burning bootloader!
* PWM works on the x16 series. Belived working on other parts. 
* PWM on the Timer D pins (10, 11 on x16, 12,13 on x17) takes 1 ms to turn on or off, and during this time, the other Timer D PWM channel will be disabled. This applies to digitalWrite() or analogWrite of 0 or 255 while it is currently outputting PWM, and analogWrite of 1~254 while the pin is not currently outputting PWM. We are looking at a better solution (#51)
* analogRead works on the x16 series. Belived working on other parts. 
* tone() should work, at least, it compiles on all chips and there's a timer for it to use. Let us know if it doesn't generate tones.
* Serial works (not tested on the x12 and x02 series - these have to use the alternate pin mapping, so am less confident this will work) 
* SPI, TWI untested. 
* Upload uses jtag2updi programmer. Upload using programmer and burn bootloader use the selected programmer. 

# Known Compiler Bugs
* Sometimes a sketch which is too big to fit will, instead of generating a message saying that, result in the error: 'relocation truncated to fit: R_AVR_13_PCREL against symbol tablejump2'
