[![Join the chat at https://gitter.im/SpenceKonde/megaTinyCore](https://badges.gitter.im/SpenceKonde/megaTinyCore.svg)](https://gitter.im/SpenceKonde/megaTinyCore?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

# megaTinyCore
Arduino core for the new megaAVR ATtiny series chips - highly experimental (not for use in actual projects yet)


# Rough status: 
* 3216/1616/816/416 mostly okay. 1606/806/406 probably in a similar state. 
* 412/402/212/202 totally untested.  Please test and create issues when shit doesn't work! TWI may or may not work at all. 
* 1614/814/804/414/404/214/204 totally untested. Please test and create issues when shit doesn't work!
* 3217/1617/1607/817/807 totally untested. Please test and create issues when shit doesn't work!
* Lots of stuff untested, please help test if you have the hardware!
* tone() should work, at least, it compiles on all chips and there's a timer for it to use. Let us know if it doesn't generate tones.
* I think analogRead() reads the wrong pin (#45)
* Anything and everything may be changed without warning, including names of menu options
* Burn bootloader and clock speeds, I think, work. Update core and restart IDE BEFORE burning bootloader, there was a nasty bug in boards.txt (fixed with 42ea5c84639a3e758a4ede9044b4e4db99b128ef ) that would brick chips when burning bootloader!
* Upload uses jtag2updi programmer. Upload using programmer and burn bootloader use the selected programmer. 

# Known Compiler Bugs
* Sometimes a sketch which is too big to fit will, instead of generating a message saying that, result in the error: 'relocation truncated to fit: R_AVR_13_PCREL against symbol tablejump2'
