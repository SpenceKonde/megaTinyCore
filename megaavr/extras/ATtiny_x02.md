# ATtiny 202/402
![x02 Pin Mapping](ATtiny_x02.gif "Arduino Pin Mapping for ATtiny x02")

 Specifications |  ATtiny202|  ATtiny402
------------ | ------------- | -------------
Flash (program memory)   | 2048 bytes | 4096 bytes
Flash w/Optiboot  | 1536 bytes | 3584 bytes
RAM  | 128 bytes | 256 bytes
EEPROM | 64 bytes | 128 bytes
Bootloader | Optiboot Optiboot (No! Don't waste a quarter of the flash!) | Optiboot (optional, not recommended)
GPIO Pins | 6 (5 usable) | 6 (5 usable)
ADC Channels | 6 (5 usable) | 6 (5 usable)
DAC | No | No
PWM Channels | 4 | 4
Timer Type B | 1 | 1
Timer Type D | No | No
Interfaces | UART, SPI, I2C | UART, SPI, I2C
Clock options | Internal 20/16/10/8/5/4/1 MHz | Internal 20/16/10/8/5/4/1 MHz

## WARNING: On pre 2.0.0 versions of megaTinyCore, the UART defaulted to the alternate positions
This matches the megaTinyCore 412/402 Rev. - and Rev. A breakout boards below. **In 2.0.0 and later, this is no longer the case!** You must call Serial.swap(1) prior to Serial.begin() to move it to the alt pins (or connect your adapter to pins 0 and 1 instead of 2 and 3). It should never have been done that way in the first place. A Rev. B of the breakout boards that has the FTDI header tied to the standard serial pins is planned for availability in May 2020.

## Signature Issue
There exist ATtiny402's in the wild where the last byte of the signature is 0x25, instead of 0x27. Currently shipping parts from Atmel/Microchip have the correct signature, and this problem appears to be in the past, for the most part. Hoerver, if you're stuck with wrong-signature ATtiny402's, choose ATtiny402 (bad signature) from the tools -> chip menu.


## Buy official megaTinyCore breakouts and support continued development
[ATtiny402 assembled](https://www.tindie.com/products/17685/)
[ATtiny412/212/402/202 bare board](https://www.tindie.com/products/17749/)

## Datasheets and Errata
See [Datasheet Listing](Datasheets.md)
