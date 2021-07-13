# ATtiny 417/817/1617/3217
![x17 Pin Mapping](ATtiny_x17.gif "Arduino Pin Mapping for ATtiny x17")

 Specifications |  ATtiny417  |  ATtiny817  |    ATtiny1617   |   ATtiny3217
------------ | ------------- | ------------- | ------------- | -------------
----------------|-------------|-------------|-----------------| -------------
Flash           |  4096 bytes |  8192 bytes |     16384 bytes | 32768 bytes
RAM             |   256 bytes |   512 bytes |      2048 bytes |  2048 bytes
EEPROM          |   128 bytes |   128 bytes |       256 bytes |   256 bytes
Bootloader | Optiboot (optional, not recommended) | Optiboot (optional, not recommended)| Optiboot (optional, not recommended) | Optiboot (optional, not recommended)
GPIO Pins       | 22/21 avail | 22/21 avail |     22/21 avail | 22/21 avail
ADC Channels    |   11 usable |   11 usable |       11 usable |   11 usable
DAC             |         Yes |         Yes |             Yes |         Yes
PWM Channels    |           8 |           8 |               8 |           8
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

The tuned options are new in 2.4.0 - see the readme amd associated page for exhaustive description of how it works and how to use it.

## Buy official megaTinyCore breakouts and support continued development
[ATtiny3217 assembled](https://www.tindie.com/products/17523/)

[ATtiny3217/1617/817/417/1607/807 bare board](https://www.tindie.com/products/17613/)

## Datasheets and Errata
See [Datasheet Listing](Datasheets.md)
