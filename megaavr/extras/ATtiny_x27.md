# ATtiny 427/827/1627/3227
![x27 Pin Mapping](ATtiny_x27.gif "Arduino Pin Mapping for ATtiny x27")

 Specifications       |    ATtiny427   |   ATtiny827   |   ATtiny1627  |  ATtiny3227   |
----------------------|----------------|---------------|---------------|---------------|
Availability          |  Since Q2 2021 | Since Q2 2021 | Since Q1 2021 |    late 2021? |
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
PWM Channels          |                        6 out of potential 8 |
Timer Type B          |                                           2 |
Timer Type D          |                                          No |
CCL logic             |                     4 LUTs, 12 of 12 inputs |
Interfaces            |                          UART (2), SPI, I2C |
Millis Timer          |             TCA0, TCB0, TCB1 (default), RTC |
RTC crystal           |                Optional  32KHz TOSC1, TOSC2 |
Clock Sources         |   Internal osc @ 16 or 20 MHz or ext. clock |
Frequency (rated)     |    Up to 5 MHz @ 1.8V, 10 @ 2.7V, 20 @ 4.5V |
Frequwncy (max*)      |   Internal runs @ ~36, chip works @ ~32 MHz |
Package               |             QFN-24, 4mm x 4mm (0.5mm pitch) |

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

The 24-pin 2-series benefit from fact that the Reset function can be moved to pin PB4, allowing PA0 to remain UPDI, while also having a proper hardware reset! Like the 24-pin 0/1-series, the VQFN-24 package is the only option, no doubt a disappointment to those hoping to assemble them at home.

As with the rest of the 2-series, the DAC, TCD, and the extra analog comparators have been removed in favor of a second UART, and the super ADC, which can accumulate 1024 samples in a single burst read (for 17 bit decimated values, and hence in theory 17-bits of resolution. For advanced ADC functionality, several new functions are provided, see ADC section of the main readme for more information.

We also get a full suite of 4 CCL logic blocks and both the CCL and event system are fully functional here, and well behaved like the megaAVR 0-series and Dx-series, not the no-interrupt version of the CCL, or the confusing event system with two kinds of chanenls. You do start to miss the TCD's extra PWM pins as you get up to 20+ GPIO pins, while still stuck with just one good PWM timer... Oh well - the point of the 2-series is the dual serial and the killer ADC.

The 42x parts, particularly the 427 will have another claim to fame by be lowest cost part with 4 CCL LUTs, the 24-pin ones having all CCL lines on actual pins (not that you usually need them all on physical pins - actually it looks like all of the 24-pin 2-series will be cheaper than anything else with 4 LUTs.

Optiboot is included and with the alternate reset option, you can have normal autoreset to enter the bootloader like a "normal" optiboot device without rendering it un-reprogrammable via UPDI (which precludes setting fuses among other things). That said, many users prefer to use one serial adapter for UPDI programming (either via SerialUPDI or some other programmer) and have a second serial adapter, which is kept open in some serial serial monitor that isn't the Arduino IDE (This is what I do, using hTerm - pretty much the only thing that the Arduino IDE serial monitor has going for it is that it automatically disconnects while uploading; otherwise it lacks almost every feature that one might want for embedded development....).

## Buy official megaTinyCore breakouts and support continued development
To be designed in the future, if people suddenly start showing interest in the 24-pin tinyAVR parts - sales of the 24-pin boards for the 0 and 1-series parts have been very poor compared to the 20-pin ones, even for assembled boards, which I find mystifying. I don't know who's using them all - clearly *someone* is because they're backordered into next year, but whoever it is, they're not using my boards.. As I have in excess of 200 unused tinyAVR x07/x17 breakout boards I'm in no hurry to design a Rev. C to support the alt reset pin and sign up to have another 100 or so produced.

[The existing boards for the 0-series and 1-series 24-pin parts work, but they don't support the alternate rewset pin for autoreset](https://www.tindie.com/products/17613/)


## Notes on Tables
`*`Maximum frequency that the the internal oscillator will reach when tuned upwards until it crashes, and the maximum frequency the part actually runs at (based on cursory, not rigorous testing - the parts that ran for a few milliseconds without crashing were considered "working") are reported. Both are at 5.0V and are typical values determined experimentally, and the parts may not function reliably at those speeds. Maximums vary between individual specimens. Don't rely on parts exceweding manufacturer specifications.


## Datasheets and Errata
See [Datasheet Listing](Datasheets.md)
