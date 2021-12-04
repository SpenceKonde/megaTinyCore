# ATtiny 426/826/1626/3226
![x26 Pin Mapping](ATtiny_x26.gif "Arduino Pin Mapping for ATtiny x26")

 Specifications       |    ATtiny426   |   ATtiny826   |   ATtiny1626  |  ATtiny3226   |
----------------------|----------------|---------------|---------------|---------------|
Availability          |  Since Q2 2021 | Since Q2 2021 | Since Q1 2021 | Since Q4 2021 |
Flash (program memory)|     4096 bytes |    8192 bytes |   16384 bytes |   32768 bytes |
Flash w/Optiboot      |     3584 bytes |    7680 bytes |   15872 bytes |   32256 bytes |
RAM                   |      512 bytes |    1024 bytes |    2048 bytes |    3072 bytes |
EEPROM                |      128 bytes |     128 bytes |     256 bytes |     256 bytes |
Bootloader (optional) | Optiboot (not recommended) | Optiboot | Optiboot |    Optiboot |

## Flash-independant specs

Feature               |             Specification                   |
----------------------|---------------------------------------------|
GPIO Pins             |           18 (17 usable, 16 with alt-reset) |
ADC                   |                  12-bit, differential w/PGA |
ADC Channels          |      15, 7 of which  can be neg. diff. pin. |
DAC                   |                    Internal for AC ref only |
Comparator            |                                           1 |
PWM Channels (**)     |                        6 out of potential 8 |
Timer Type B          |                                           2 |
Timer Type D          |                                          No |
CCL logic             |                     4 LUTs, 10 of 12 inputs |
Interfaces            |                          UART (2), SPI, I2C |
Millis Timer          |             TCA0, TCB0, TCB1 (default), RTC |
RTC crystal           |                Optional  32KHz TOSC1, TOSC2 |
Clock Sources         |   Internal osc @ 16 or 20 MHz or ext. clock |
Frequency (rated)     |    Up to 5 MHz @ 1.8V, 10 @ 2.7V, 20 @ 4.5V |
Frequwncy (max*)      |  Internal runs up to 36, and works @ 32 MHz |
Package               |             SOIC-20, TSSOP-20, QFN-20 3x3mm |

## Clock Options
These parts do not support an external HF crystal, only an external clock, and/or a watch crystak for the RTC.
 MHz | Source          | Notes
 ----|-----------------|-------
  20 | Internal        |
  16 | Internal        |
  10 | Internal        |
   8 | Internal        |
   5 | Internal        |
   4 | Internal        |
   1 | Internal        |
  20 | Internal, tuned |
  16 | Internal, tuned |
  12 | Internal, tuned |
  20 | External Clock  | External clock goes to CLKI (PA3). Minimize any load on this pin, including even short wires. HF stuff is very picky.
  16 | External Clock  | As above.
  10 | External Clock  | As above.
   8 | External Clock  | As above.
  24 | Internal, tuned | OVERCLOCKED, usually fine @ 5v and room temperature.
  25 | Internal, tuned | OVERCLOCKED, usually fine @ 5v and room temperature.
  30 | Internal, tuned | OVERCLOCKED, may be unstable.
  32 | Internal, tuned | OVERCLOCKED, may be unstable.
  24 | External Clock  | OVERCLOCKED, usually fine @ 5v and room temperature. Uses CLKI/PA3 as above.
  25 | External Clock  | OVERCLOCKED, usually fine @ 5v and room temperature. Uses CLKI/PA3 as above.
  30 | External Clock  | OVERCLOCKED, may be unstable. Uses CLKI/PA3 as above.
  32 | External Clock  | OVERCLOCKED, may be unstable. Uses CLKI/PA3 as above.
When external clock is used as system clock source, it cannot be used for any other purpose (obviously) - all control over that pin is taken by CLKCTRL.

* The overclocked options at 24/25 MHz have been found to generally work around room temperature when running at 5v. For faster speeds, initial results seem to imply that the 2-series parts are significantly more capable of handling higher operating frequencies compared to the 0/1-series, and that with solid 5v supply, 32 MHz at room temperature may be good enough for practical use! Whereas the previous parts collapsed at around 30-32 MHz running from internal oscillator, that isn't seen until the mid 30's on the 2-series parts. As always, external oscillators work more reliably than the internal one when overclocking, but they generally cost about as much as the microcontroller itself and are gross overkill (in terms of accuracy) for what most arduino folks want from them.

The tuned options are new in 2.4.0 - see the [tuned internal oscillator guide](Ref_Tuning.md) for more information before using these options.

## Overview

The 20-pin 2-series parts bring one very exciting feature, particularly for those who like using serial bootloaders instead of UPDI - the Reset function can be moved to pin PB4, allowing PA0 to remain UPDI, while also having a proper hardware reset! These are being advertised in SOIC-20 and the super tiny 3mm x 3mm QFN of the 1-series, plus SSOP. Not sure SSOP will set the world on fire... It's also not certain yet if they will actually make the 32k parts available in the QFN - if so, that will be likely be the most in-demand part in the 2-series (ie, expect shortages whenever they are released)

DAC, TCD, and the extra analog comparators have been removed in favor of a second UART, and the super ADC, which can accumulate 1024 samples in a single burst read; decimation of the accumulated value so all of the digits are meaningful can give up to 17-bits of accuracy. For advanced ADC functionality, several new functions are provided, see ADC section of the main readme for more information.

## Bootloader
Optiboot is included and with the alternate reset option, you can have normal autoreset to enter the bootloader like a "normal" optiboot device without rendering it un-reprogrammable via UPDI (which would preclude setting fuses among other things). That said, many users prefer to use one serial adapter for UPDI programming (either via SerialUPDI or some other programmer) and have a second serial adapter, which is kept open in some serial serial monitor that isn't the Arduino IDE, dispensing with the bootloader altogether. (This is what I do, using hTerm - pretty much the only thing that the Arduino IDE serial monitor has going for it is that it automatically disconnects while uploading; otherwise it lacks nearly every feature that one might want for embedded development).

For these parts, provided they have 8k+ flash, I make no recommendation between Optiboot with alternate reset pin vs use with UPDI programming and no bootloader - it comes down to your preferences and development workflow. For the 4k flash parts, I question the wisdom of dedicating 1/8th of the flash for a bootloader offering little practical benefit.

## Buy official megaTinyCore breakouts and support continued development
Coming Soon!

[The existing boards for the 0-series and 1-series 20-pin parts work, but they don't support the alternate rewset pin for autoreset](https://www.tindie.com/products/17614/)


## Notes on Tables
`*`Maximum frequency that the the internal oscillator will reach when tuned upwards until it crashes, and the maximum frequency the part actually runs at (based on cursory, not rigorous testing - the parts that ran for a few milliseconds without crashing or failing to perform correct arithmetic with a few unsigned longs was enough to be considered "working") are reported. Both are at 5.0V and are typical values determined experimentally, and the parts may not function reliably at those speeds under more realistic conditions. Maximums vary between individual specimens. *Don't rely on parts exceeding manufacturer specifications*.

`**` PWM channels exposed with default configuration via analogWrite(). TCA0 is in split mode, and the type B timer(s) are not used for PWM; those could be configured by user code to get two additional channels.

## Datasheets and Errata
See [Datasheet Listing](Datasheets.md)
