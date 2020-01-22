### ATtiny 417/817/1617/3217
![x17 Pin Mapping](ATtiny_x17.gif "Arduino Pin Mapping for ATtiny x17")

 Specifications |  ATtiny417  |  ATtiny817  |    ATtiny1617   |   ATtiny3217
------------ | ------------- | ------------- | ------------- | -------------
Flash (program memory)   | 4096 bytes| 8192 bytes | 16384 bytes | 32768 bytes 
Flash w/Optiboot   | 3584 bytes| 7680 bytes | 15872 bytes | 32256 bytes 
RAM  | 256 bytes | 512 bytes | 2048 bytes | 2048 bytes 
EEPROM | 128 bytes | 128 bytes | 256 bytes | 256 bytes
Bootloader | No | No | No | No 
GPIO Pins | 22 (21 usable) | 22 (21 usable) | 22 (21 usable) | 22 (21 usable)
ADC Channels | 12 (11 usable) | 12 (11 usable) | 12 (11 usable) | 12 (11 usable)
DAC | Yes | Yes | Yes | Yes
PWM Channels | 8 | 8 | 8 | 8 
Timer Type B | 1 | 1 | 2 | 2 
Interfaces | UART, SPI, I2C | UART, SPI, I2C | UART, SPI, I2C | UART, SPI, I2C 
Clock options | Internal 20/16/10/8/5/4/1 MHz | Internal 20/16/10/8/5/4/1 MHz | Internal 20/16/10/8/5/4/1 MHz | Internal 20/16/10/8/5/4/1 MHz 

#### Millis/micros timer options
All parts default to using TCA0 for millis/micros. All options are supported, except on the 417 and 817, where TCB1 is not supported as these parts do not have a TCB1. If RTC with external crystal is used, the crystal and associated loading caps must be connected to pins 8 and 9, and those pins may not be used for other purposes - this means if Serial it to be used, the alternate pins must be selected.  


### Buy official megaTinyCore breakouts and support continued development!
[ATtiny3217 assembled](https://www.tindie.com/products/17523/)

[ATtiny3217/1617/817/417/1607/807 bare board](https://www.tindie.com/products/17613/)

### Datasheets
[ATtiny417/817](http://ww1.microchip.com/downloads/en/DeviceDoc/ATtiny417-817-DataSheet-DS40001901D.pdf)

[ATtiny1617/3217](http://ww1.microchip.com/downloads/en/DeviceDoc/ATtiny1617-3217-DataSheet-DS40001999C.pdf)
