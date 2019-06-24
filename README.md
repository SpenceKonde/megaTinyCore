[![Join the chat at https://gitter.im/SpenceKonde/megaTinyCore](https://badges.gitter.im/SpenceKonde/megaTinyCore.svg)](https://gitter.im/SpenceKonde/megaTinyCore?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

# megaTinyCore
Arduino core for the new megaAVR ATtiny series chips - highly experimental

# Rough status: 
* Lots of stuff untested, please help test if you have the hardware!
* tone() should work, at least, it compiles on all chips and there's a timer for it to use. Let us know if it doesn't generate tones.
* Burn bootloader and clock speeds, I think, work. Update core and restart IDE BEFORE burning bootloader if you have been using 1.0.0-dev versions prior to 6/22/2019, there was a nasty bug in boards.txt (see #49, very recently fixed) that would brick chips when burning bootloader!
* Upload uses jtag2updi programmer. Upload using programmer and burn bootloader use the selected programmer. 

# Known Compiler Bugs
* Sometimes a sketch which is too big to fit will, instead of generating a message saying that, result in the error: 'relocation truncated to fit: R_AVR_13_PCREL against symbol tablejump2'
