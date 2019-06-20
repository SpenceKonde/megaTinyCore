# megaTinyCore
Arduino core for the new megaAVR ATtiny series chips - highly experimental (not for use in actual projects yet)


# Rough status:
* 3216/1616/816/416 mostly okay. 1606/806/406 probably in a similar state. Tone, if it works at all, only works on 3216/1616.
* Lots of stuff untested - any testing on the -6 chips listed above that finds a problem without a corresponding issue should have an issue created for it; this would be much appreciated.
* Anything and everything may be changed without warning, including pin mapping and names of menu options
* Other chips are not ready, but may be close. Preliminary pin mapping proposal for 1614/814/414/804/404.
* We have a timer shortage on most of these chips, because one of the type b timers is used for millis, meaning it can't be used for tone (or servo, or anything else).
* Pretty sure burn bootloader sets OSCCFG wrong, resulting in all the timing being wrong. (#29)

# Known Compiler Bugs
* Sometimes a sketch which is too big to fit will, instead of generating a message saying that, result in the error: 'relocation truncated to fit: R_AVR_13_PCREL against symbol tablejump2'
