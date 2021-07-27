# ATtiny 202/402
![x02 Pin Mapping](ATtiny_x02.gif "Arduino Pin Mapping for ATtiny x02")

 Specifications |  ATtiny202|  ATtiny402
------------ | ------------- | -------------
Flash (program memory)   | 2048 bytes | 4096 bytes
Flash w/Optiboot  | 1536 bytes | 3584 bytes
RAM  | 128 bytes | 256 bytes
EEPROM | 64 bytes | 128 bytes
Bootloader (optional) | Optiboot Optiboot (No! Don't throw away a quarter of the flash!) | Optiboot (strongly not recommended)
GPIO Pins | 6 (5 usable) | 6 (5 usable)
ADC Channels | 6 (5 usable) | 6 (5 usable)
DAC | No | No
PWM Channels | 4 | 4
Timer Type B | 1 | 1
Timer Type D | No | No
Interfaces | UART, SPI, I2C | UART, SPI, I2C

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
  20 | External Clock  |
  16 | External Clock  |
  10 | External Clock  |
   8 | External Clock  |
  24 | Internal, tuned | OVERCLOCKED, usually fine @ 5v and room temperature.
  25 | Internal, tuned | OVERCLOCKED, usually fine @ 5v and room temperature.
  30 | Internal, tuned | OVERCLOCKED, may be unstable.
  24 | External Clock  | OVERCLOCKED, usually fine @ 5v and room temperature.
  25 | External Clock  | OVERCLOCKED, usually fine @ 5v and room temperature.
  30 | External Clock  | OVERCLOCKED, may be unstable.
  32 | External Clock  | OVERCLOCKED, may be unstable.

* The overclocked options at 24/25 MHz have been found to generally work around room temperature when running at 5v. The faster ones - while they can be stable with solid 5v supply at room temperature, this is right on the edge of what these parts can do. I have specimens that will run at 5.0v but not at 4.8, for example, meaning that it would work powered by some USB ports, but not others (those range from 4.7 to 5.3v) and they are of course extremely sensitive to noise on power rails. External oscillators work more reliably than the internal one when overclocking, but they generally cost about as much as the microcontroller itself and are gross overkill (in terms of accuracy) for what most arduino folks want from them.

The tuned options are new in 2.4.0 - see the [tuned internal oscillator guide](Tuning.md) for more information before using these options.

## WARNING: On pre 2.0.0 versions of megaTinyCore, the UART defaulted to the alternate positions
This matches the megaTinyCore 412/402 Rev. - and Rev. A breakout boards below. **In 2.0.0 and later, this is no longer the case!** You must call Serial.swap(1) prior to Serial.begin() to move it to the alt pins (or connect your adapter to pins 0 and 1 instead of 2 and 3). It should never have been done that way in the first place. A Rev. B of the breakout boards that has the FTDI header tied to the standard serial pins is planned for availability in May 2020.

## Signature Issue
There exist ATtiny402's in the wild where the last byte of the signature is 0x25, instead of 0x27. Currently shipping parts from Atmel/Microchip have the correct signature, and this problem appears to be in the past, for the most part. Hoerver, if you're stuck with wrong-signature ATtiny402's, choose ATtiny402 (bad signature) from the tools -> chip menu.

## The issue with bootloader
* There's no dedicated reset pin. So there is no way to do the traditional autoreset circuit to reset the chip to upload with a bootloader unless you disable UPDI (requiring HV UPDI to undo - I've got a half dozen boards that are bricked until I have time to get an hvupdi programming setup together to ressurect them). Either you manually power cycle it just prior to trying to upload, or you have some sort of ersatz-reset solution coupled to an autoreset circuit, or handle it in some other bespoke way. Regardless of the approach, short of disabling UPDI to get a reset pin, none of them are as convenient a development cycle as we're used to. In most cases, the most convenient development configuration is to simply use UPDI programming, and leave any serial connection open while programming via UPDI using a programmer on a different port. Note that the 2-series 20 and 24 pin parts have enhancements that make a bootloader capabloe of providing a better developer experience possible.
* It takes 512b of flash - 1/4th or 1/8th of the total, but offers little, if any, advantages.

## Buy official megaTinyCore breakouts and support continued development
[ATtiny402 assembled](https://www.tindie.com/products/17685/)
[ATtiny412/212/402/202 bare board](https://www.tindie.com/products/17749/)

## Datasheets and Errata
See [Datasheet Listing](Datasheets.md)
