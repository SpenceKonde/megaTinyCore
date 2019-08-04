1.0.3
* Fix UART on 412/402 and general PORTMUX initialization. 
* Add tinyNeoPixel and tinyNeoPixel_Static libraries, add menu option to select port at 8/10MHz (saves flash)

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
