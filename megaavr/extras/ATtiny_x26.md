# ATtiny 426/826/1626/3226
![x26 Pin Mapping](ATtiny_x26.gif "Arduino Pin Mapping for ATtiny x26")

 Specifications       |    ATtiny426   |   ATtiny826   |   ATtiny1626  |  ATtiny3226   |
----------------------|----------------|---------------|---------------|---------------|
Availability          |  Since Q2 2021 | Since Q2 2021 | Since Q1 2021 | Since Q4 2021 </br> but difficult to source |
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
PWM Channels **       |                        6 out of potential 8 |
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
B.O.D. voltages       | 1.8, 2.6 (for 3-3.3v systems), and 4.2 (for 5V systems) |
B.O.D. voltages       | "unqualified" 2.1, 2.9, 3.3, 3.7, 4.0       |

The "unqualified" BOD voltages are not listed in the datasheet, but were referenced in early versions of headers, just like the 0/1-series. They appear to work, I take them being described as "unqualified" to mean that they are not guaranteed by Mirochop.


## Clock Options
These parts do not support an external HF crystal, only an external clock, and/or a watch crystak for the RTC.
 MHz | Source          | Notes
 ----|-----------------|-------
  20 | Internal        | Default, typical for 5V systems
  16 | Internal        | Typical for 5V systems. Some libraries may work better at this speed.
  10 | Internal        | Typical for 3.3V systems
   8 | Internal        | Typical for 3.3V systems
   5 | Internal        | Typical for 1.8V - 2.5V systems
   4 | Internal        | Typical for 1.8V - 2.5V systems
   1 | Internal        |
  20 | Internal, tuned | 16 MHz OSC selected by fuse can be tuned up to 20 no problem.
  16 | Internal, tuned | 20 MHz selected by OSCCFG fuse can be tuned down to 16 no problem.
  12 | Internal, tuned | 16 MHz can be just barely tuned down to 12. 20 MHz gets tuned up to 24 and prescaled.
  20 | External Clock  | External clock goes to CLKI (PA3).
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

## Notes on Tables
`*`Maximum frequency that the the internal oscillator will reach when tuned upwards until it crashes, and the maximum frequency the part actually runs at (based on cursory, not rigorous testing - the parts that ran for a few milliseconds without crashing or failing to perform correct arithmetic with a few unsigned longs were considered "working") are reported. Both are at 5.0V and are typical values determined experimentally, and the parts may not function reliably at those speeds under more realistic conditions. Maximums vary between individual specimens. *Don't rely on parts exceeding manufacturer specifications*.

* PWM channel reported is the number exposed via analogWrite with default megaTinyCore configuration. TCA0 is in split mode, and the type B timer(s) are not used for PWM; those could be configured by user code to get two additional channels.
* When external clock is used as system clock source, PIN_PA3 cannot be used for any other purpose (obviously) - all control over that pin is taken by CLKCTRL. Minimize any load on this pin, including even short wires. HF stuff is very picky.
* The overclocked options at 24/25 MHz have been found to generally work around room temperature when running at 5v. For faster speeds, initial results seem to imply that the 2-series parts are significantly more capable of handling higher operating frequencies compared to the 0/1-series, and that with solid 5v supply, 32 MHz at room temperature may be good enough for practical use, even from the internal oscillator! Whereas the previous tinyAVR parts collapsed at around 30-32 MHz running from internal oscillator, that isn't seen until the mid 30's on the 2-series parts.
* All else being equal, your chances of reaching an aggressive overclock are better with external clock vs internal, and with the extended temperature range parts instead of normal ones.

The tuned options are new in 2.4.0 - see the [tuned internal oscillator guide](Ref_Tuning.md) for more information before using these options. They require running a tuning sketch on your chip.

## Overview
The 20-pin 2-series parts bring one very exciting feature, particularly for those who like using serial bootloaders instead of UPDI - the Reset function can be moved to pin PB4, allowing PA0 to remain UPDI, while also having a proper hardware reset! These are being advertised in SOIC-20 and the super tiny 3mm x 3mm QFN of the 1-series, plus SSOP. Unlike the 1-series, 32k parts will be available in QFN - It looks like they will begin shipping in march 2022, but only to people who are willing to buy a full tray of 490 pcs.

The 2-series parts sacrifice some features of the 1-series: The DAC, TCD, and the extra analog comparators have been removed in favor of a second UART, a ver fancy true differential ADC with a programmable gain amplifier, which can accumulate 1024 samples in a single burst read; decimation of the accumulated value so all of the digits are meaningful can give up to 17-bits of accuracy (provided the noise in on your circuit is low enough which it often is not. For advanced ADC functionality, several new functions are provided, see ADC section of the main readme for more information.

## External crystal
These parts support an external 32.768 kHz crystal clock source for the RTC. Conveniently, the pins for this are... PB2 and PB3, the same pins used by default for the USART. These parts expect a crystal with a very low load capacitance, on the order of 6pf. A pair of loading capacitors of a few pF each is necessary for it to oscillate, and traces between chip and crystal must be kept very short. There is rarely a need to use it. At room temperature and 3-5V, the internal oscillator is very accurate, and can be used to calculate the clock speed of the (inaccurate, but stable) 32 kHz internal RTC oscillator accurately enough to use it to tune the chip as described above, a sketch for this will be provided with a future version of this core.

## Bootloader
Optiboot is included and with the alternate reset option, you can have normal autoreset to enter the bootloader like a "normal" optiboot device without rendering it un-reprogrammable via UPDI (which would preclude setting fuses among other things). That said, many users prefer to use one serial adapter for UPDI programming (either via SerialUPDI or some other programmer) and have a second serial adapter, which is kept open in some serial serial monitor that isn't the Arduino IDE, dispensing with the bootloader altogether. (This is what I do, using hTerm - pretty much the only thing that the Arduino IDE serial monitor has going for it is that it automatically disconnects while uploading; otherwise it lacks nearly every feature that one might want for embedded development).

For these parts, provided they have 8k+ flash, I make no recommendation between Optiboot with alternate reset pin vs use with UPDI programming and no bootloader - it comes down to your preferences and development workflow. For the 4k flash parts, I question the wisdom of dedicating 1/8th of the flash for a bootloader offering little practical benefit.

## Buy official megaTinyCore breakouts and support continued development

[Assembled 3226 and 1626 boards are now available](https://www.tindie.com/products/17597/)

[ATtiny3226/1626/826/426/3216/1616/816/416/1606/806/406 bare boards are now available](https://www.tindie.com/products/17614/) Rev. C has support for autoreset on the alternate reset pin.

## Datasheets and Errata
See [Datasheet Listing](Datasheets.md)
