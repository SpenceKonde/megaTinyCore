# ATtiny 424/824/1624/3224
![x24 Pin Mapping](ATtiny_x24.gif "Arduino Pin Mapping for ATtiny x24")

 Specifications       |    ATtiny424   |   ATtiny824   |   ATtiny1627  |  ATtiny3224   |
----------------------|----------------|---------------|---------------|---------------|
Availability          |    Q2/Q3 2021  |    Q2/Q3 2021 | Since Q1 2021 |   after 4k/8k |
Flash (program memory)|     4096 bytes |    8192 bytes |   16384 bytes |   32768 bytes |
Flash w/Optiboot      |     3584 bytes |    7680 bytes |   15872 bytes |   32256 bytes |
RAM                   |      512 bytes |    1024 bytes |    2048 bytes |    3072 bytes |
EEPROM                |      128 bytes |     128 bytes |     256 bytes |     256 bytes |
Bootloader (optional) | Optiboot (not recommended) | Optiboot (not recommended) | Optiboot (not recommended) | Optiboot (not recommended) |

## Flash-independant specs

Feature               |             Specification                   |
----------------------|---------------------------------------------|
GPIO Pins             |                              12 (11 usable) |
ADC                   |                  12-bit, differential w/PGA |
ADC Channels          |       9, 7 of which  can be neg. diff. pin. |
DAC                   |                    Internal for AC ref only |
Comparator            |                                           1 |
PWM Channels **       |                                           6 |
Timer Type B          |                                           2 |
Timer Type D          |                                          No |
CCL logic             |                     4 LUTs, 12 of 12 inputs |
Interfaces            |                          UART (2), SPI, I2C |
Millis Timer          |             TCA0, TCB0, TCB1 (default), RTC |
RTC crystal           |                Optional  32KHz TOSC1, TOSC2 |
Clock Sources         |   Internal osc @ 16 or 20 MHz or ext. clock |
Frequency (rated)     |    Up to 5 MHz @ 1.8V, 10 @ 2.7V, 20 @ 4.5V |
Frequency (max * )    |   Internal runs @ ~36, chip works @ ~32 MHz |
Package               |                            SOIC-14, SSOP-14 |

## Clock Options
These parts do not support an external HF crystal, only an external clock,  and/or a watch crystak for the RTC.
 MHz | Source
  20 | Internal, no tuning
  16 | Internal, no tuning
  10 | Internal, no tuning
   8 | Internal, no tuning
   5 | Internal, no tuning
   4 | Internal, no tuning
   1 | Internal, no tuning
  20 | Internal, tuned using previously gathered settings (see docs)
  16 | Internal, tuned using previously gathered settings (see docs)
  12 | Internal, tuned using previously gathered settings (see docs)
  20 | External Clock
  16 | External Clock
  10 | External Clock
   8 | External Clock
  24 | Internal, tuned OVERCLOCKED (see docs)
  25 | Internal, tuned OVERCLOCKED (see docs)
  30 | Internal, tuned OVERCLOCKED (see docs, osc. must be in 20 MHz mode)
  32 | Internal, tuned OVERCLOCKED (see docs, osc. must be in 20 MHz mode)
  32 | Internal, tuned OVERCLOCKED, likely unstable)
  24 | External Clock OVERCLOCKED
  25 | External Clock OVERCLOCKED
  30 | Internal, tuned  OVERCLOCKED
  32 | Internal, tuned  OVERCLOCKED, likely unstable)

## Overview
The smallest pincount of the tinyAVR 2-series parts - so far there is no word on what Microchip's plans are for the 8-pin package, but everyone I've spoken to is worried about the future of AVRs in 8-pin packages.. These have been announced in the full range of flash size options - Hopefully it won't be a repeat of the 3214 which was in the ATpacks, but never actually shipped, and had all references to it removed. Notice that some 1-series features - mostly the extravagances of the "golden" 1-series parts - are gone, while the second type B timer (thankfully) is present in all sizes. The main features though are the fancy ADC, and the second USART - as well as a 32k version with 3k SRAM. The expanded RAM on the 4k and 8k parts goes a long way to making those parts more practical option.

The super ADC can accumulate 1024 samples in a single burst read; decimation of the accumulated value so all of the digits are meaningful can give up to 17-bits of accuracy. For advanced ADC functionality, several new functions are provided, see ADC section of the main readme for more information.

Optiboot is supported, but unlike the higher pincount 2-series devices, you can't have an alternate reset pin here (the pin it can be moved to doesn't exist), so these have the same reset-pain as the 1-series; this is why optiboot is marked 'not recommended' even on parts that have plenty of flash. We default to TCB1 for millis, since there is no type D timer, and because we have TCB0 for tone or TCB-dependant libraries, and TCA0 is more likely to be needed than a second TCB - [taking over TCA0](TakingOverTCA0.md) seems to be quite popular; TCB1 also has the additional benefit of having the lowestr overhead both for the interrupt millis/micros.


## Buy official megaTinyCore breakouts and support continued development
With no critical hardware differences, the same breakout boards we have been selling for the ATtiny1614 and 1614 can be used. We do have a new version in the works with improvements across the whole family, with better silkscreen (including both Pxn notation and key functions, ex. SDA, TX/RX, etc), slightly narrower 0.6" spacing between the rows of pins (for use with sockets and/or prototyping board that caters to that widely used spacing), and support for an external 3.2 x 2.5mm clock.

[ATtiny1624 assembled](https://www.tindie.com/products/17598/)

[ATtiny xy4 bare board](https://www.tindie.com/products/17748/)

## Notes on Tables
`*`Maximum frequency that the the internal oscillator will reach when tuned upwards until it crashes, and the maximum frequency the part actually runs at (based on cursory, not rigorous testing - the parts that ran for a few milliseconds without crashing were considered "working") are reported. Both are at 5.0V and are typical values determined experimentally, and the parts may not function reliably at those speeds. Maximums vary between individual specimens. Don't rely on parts exceweding manufacturer specifications.


## Datasheets and Errata
See [Datasheet Listing](Datasheets.md)
