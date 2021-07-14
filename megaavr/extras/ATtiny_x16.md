# ATtiny 416/816/1616/3216
![x16 Pin Mapping](ATtiny_x16.gif "Arduino Pin Mapping for ATtiny x16")

 Specifications |  ATtiny416  |  ATtiny816  |    ATtiny1616   |   ATtiny3216
----------------|-------------|-------------|-----------------| -------------
Flash           |  4096 bytes |  8192 bytes |     16384 bytes | 32768 bytes
RAM             |   256 bytes |   512 bytes |      2048 bytes |  2048 bytes
EEPROM          |   128 bytes |   128 bytes |       256 bytes |   256 bytes
Bootloader | Optiboot (optional, not recommended) | Optiboot (optional, not recommended)| Optiboot (optional, not recommended) | Optiboot (optional, not recommended)
GPIO Pins       | 18/17 avail | 18/17 avail |     18/17 avail | 18/17 avail
ADC Channels    | 11 usable   | 11 usable   |       11 usable |   11 usable
DAC             |         Yes |         Yes |             Yes |         Yes
PWM Channels *  |           8 |           8 |               8 |           8
Timer type B    |           1 |           1 |               2 |           2
Timer Type D    |         Yes |         Yes |             Yes |         Yes
Interfaces | UART, SPI, I2C | UART, SPI, I2C | UART, SPI, I2C | UART, SPI, I2C

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
  24 | External Clock (OVERCLOCKED)
  25 | External Clock (OVERCLOCKED)
  30 | External Clock (OVERCLOCKED)
  32 | External Clock (OVERCLOCKED, likely unstable)

The tuned options are new in 2.4.0 - see the readme amd associated pages for exhaustive description of how it works and how to use it.

## Buy official megaTinyCore breakouts and support continued development
[ATtiny3216 assembled](https://www.tindie.com/products/17597/)

[ATtiny3216/1616/816/416/1606/806/406 bare board](https://www.tindie.com/products/17614/)

## Notes on Tables
`*` PWM channels exposed with default configuration via analogWrite(); TCA0 is in split mode, TCD0 used for two PWM channels, and the type B timer(s) are not used for PWM.

## Datasheets and Errata
See [Datasheet Listing](Datasheets.md)
