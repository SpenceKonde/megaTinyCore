# ATtiny 427/827/1627/3227
![x27 Pin Mapping](ATtiny_x27.gif "Arduino Pin Mapping for ATtiny x27")

 Specifications       |    ATtiny427   |   ATtiny827   |   ATtiny1627  |  ATtiny3227   |
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
GPIO Pins             |           22 (21 usable, 20 with alt-reset) |
ADC                   |                  12-bit, differential w/PGA |
ADC Channels          |      15, 7 of which  can be neg. diff. pin. |
DAC                   |                    Internal for AC ref only |
Comparator            |                                           1 |
PWM Channels (**)     |                        6 out of potential 8 |
Timer Type B          |                                           2 |
Timer Type D          |                                          No |
CCL logic             |                     4 LUTs, 12 of 12 inputs |
Interfaces            |                          UART (2), SPI, I2C |
Millis Timer          |             TCA0, TCB0, TCB1 (default), RTC |
RTC crystal           |                Optional  32KHz TOSC1, TOSC2 |
Clock Sources         |   Internal osc @ 16 or 20 MHz or ext. clock |
Frequency (rated)     |    Up to 5 MHz @ 1.8V, 10 @ 2.7V, 20 @ 4.5V |
Frequwncy (max*)      |   Internal runs up to 36ish, functional at 32 MHz |
Package               |             QFN-24, 4mm x 4mm (0.5mm pitch) |
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

When external clock is used as system clock source, PA3 cannot be used for any other purpose (obviously) - all control over that pin is taken by CLKCTRL.

All else being equal, your chances of reaching an aggressive overclock are better with external clock vs internal, and with the extended temperature range parts instead of normal ones.

* The overclocked options at 24/25 MHz have been found to generally work around room temperature when running at 5v. For faster speeds, initial results seem to imply that the 2-series parts are significantly more capable of handling higher operating frequencies compared to the 0/1-series, and that with solid 5v supply, 32 MHz at room temperature may be good enough for practical use, even from the internal oscillator! Whereas the previous tinyAVR parts collapsed at around 30-32 MHz running from internal oscillator, that isn't seen until the mid 30's on the 2-series parts.

The tuned options are new in 2.4.0 - see the [tuned internal oscillator guide](Ref_Tuning.md) for more information before using these options. They require running a tuning sketch on your chip.

## Overview
The 24-pin 2-series benefit from fact that the Reset function can be moved to pin PB4, allowing PA0 to remain UPDI, while also having a proper hardware reset! Like the 24-pin 0/1-series, the VQFN-24 package is the only option, no doubt a disappointment to those hoping to assemble them at home.

As with the rest of the 2-series, the DAC, TCD, and the extra analog comparators have been removed in favor of a second UART, and the super ADC, which can accumulate 1024 samples in a single burst read (for 17 bit decimated values, and hence in theory 17-bits of resolution. For advanced ADC functionality, several new functions are provided, see ADC section of the main readme for more information. You do start to miss the TCD's extra PWM pins as you get up to 20+ GPIO pins, while still stuck with just one good PWM timer... Oh well - the point of the 2-series is the dual serial and the killer ADC.

### Great for CCL
We also get a full suite of 4 CCL logic blocks and both the CCL and event system are fully functional here, like the megaAVR 0-series and Dx-series, not the no-interrupt version of the CCL and the confusing and limited event system with two kinds of channels that plague the 0/1-series. The 42x parts, particularly the 427 will have another claim to fame by be lowest cost part with 4 CCL LUTs, the 24-pin ones having all CCL lines on actual pins (not that you usually need them all on physical pins. In fact, it looks likely that all flash sizes of the 2-series will be cheaper than anything else with 4 LUTs.

## Bootloader
Optiboot is included and with the alternate reset option, you can have normal autoreset to enter the bootloader like a "normal" optiboot device without rendering it un-reprogrammable via UPDI (which would preclude setting fuses among other things). That said, many users prefer to use one serial adapter for UPDI programming (either via SerialUPDI or some other programmer) and have a second serial adapter, which is kept open in some serial serial monitor that isn't the Arduino IDE, dispensing with the bootloader altogether. (This is what I do, using hTerm - pretty much the only thing that the Arduino IDE serial monitor has going for it is that it automatically disconnects while uploading; otherwise it lacks nearly every feature that one might want for embedded development).

For these parts, provided they have 8k+ flash, I make no recommendation between Optiboot with alternate reset pin vs use with UPDI programming and no bootloader - it comes down to your preferences and development workflow. For the 4k flash parts, I question the wisdom of dedicating 1/8th of the flash for a bootloader offering little practical benefit.

## Buy official megaTinyCore breakouts and support continued development
[Assembled 3227 boards are now available, albeit with an jumper soldered onto the board to fix a bug](https://www.tindie.com/products/17523/).

[Bare boards without alt-reset support](https://www.tindie.com/products/17613/) - As noted above, the Rev. C boards came back in bad shape. I wouldn't be selling them at all, except that I assembled 10 of them with a precious 3227 chip before discovering that the power on the top and bottom halves of the boards were not connected. Rev. D of the boards will correct this issue.

## Notes on Tables
`*`Maximum frequency that the the internal oscillator will reach when tuned upwards until it crashes, and the maximum frequency the part actually runs at (based on cursory, not rigorous testing - the parts that ran for a few milliseconds without crashing or failing to perform correct arithmetic with a few unsigned longs was enough to be considered "working") are reported. Both are at 5.0V and are typical values determined experimentally, and the parts may not function reliably at those speeds under more realistic conditions. Maximums vary between individual specimens. *Don't rely on parts exceeding manufacturer specifications*.

`**` PWM channels exposed with default configuration via analogWrite(). TCA0 is in split mode, and the type B timer(s) are not used for PWM; those could be configured by user code to get two additional channels.

## Datasheets and Errata
See [Datasheet Listing](Datasheets.md)
