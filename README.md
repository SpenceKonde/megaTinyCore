[![Join the chat at https://gitter.im/SpenceKonde/megaTinyCore](https://badges.gitter.im/SpenceKonde/megaTinyCore.svg)](https://gitter.im/SpenceKonde/megaTinyCore?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

# megaTinyCore
Arduino core for the new megaAVR ATtiny series chips - highly experimental (not for use in actual projects yet)


# Rough status: 
* 3216/1616/816/416 mostly okay. 1606/806/406 probably in a similar state. Tone, if it works at all, only works on 3216/1616. 
* 412/402/212/202 totally untested.  Please test and create issues when shit doesn't work! Serial and TWI won't work on them yet though. 
* 1614/814/804/414/404/214/204 totally untested. Please test and create issues when shit doesn't work!
* 3217/1617/1607/817/807 totally untested. Please test and create issues when shit doesn't work!
* Lots of stuff untested, please help test if you have the hardware! 
* I think analogRead() reads the wrong pin (#45)
* Anything and everything may be changed without warning, including names of menu options
* Pretty sure burn bootloader sets OSCCFG wrong, resulting in all the timing being wrong. (#29)
* Upload uses jtag2updi programmer. Upload using programmer and burn bootloader use the selected programmer. 

# Known Compiler Bugs
* Sometimes a sketch which is too big to fit will, instead of generating a message saying that, result in the error: 'relocation truncated to fit: R_AVR_13_PCREL against symbol tablejump2'
