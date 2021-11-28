# ATtiny 212/412
![x12 Pin Mapping](ATtiny_x12.gif "Arduino Pin Mapping for ATtiny x12")

 Specifications |  ATtiny212|  ATtiny412
------------ | ------------- | -------------
Flash (program memory)   | 2048 bytes | 4096 bytes
Flash w/Optiboot   | 1536 bytes | 3584 bytes
RAM  | 128 bytes | 256 bytes
EEPROM | 64 bytes | 128 bytes
Bootloader | Optiboot (using 25% of available flash? Not recommended) | Optiboot (Strongly not recommended)
GPIO Pins | 6 (5 usable) | 6 (5 usable)
ADC Channels | 6 (5 usable) | 6 (5 usable)
DAC | Yes | Yes
PWM Channels | 4 | 4
Timer Type B | 1 | 1
Timer Type D | Yes | Yes
Interfaces | UART, SPI, I2C | UART, SPI, I2C

## Clock Options
These parts do not support an external HF crystal, only an external clock. No watch crystal on 8-pin parts.
 MHz | Source          | Notes
 ----|-----------------|-------
  20 | Internal        |
  16 | Internal        |
  10 | Internal        |
   8 | Internal        |
   5 | Internal        |
   4 | Internal        |
   1 | Internal        |
  20 | Internal, tuned | Works with 16 MHz oscillator
  16 | Internal, tuned | Works with 20 MHz oscillator
  12 | Internal, tuned | 
  20 | External Clock  | External clock to (PA3), short wires.
  16 | External Clock  | As above.
  10 | External Clock  | As above.
   8 | External Clock  | As above.
  24 | Internal, tuned | OVERCLOCKED, usually works
  25 | Internal, tuned | OVERCLOCKED, usually works
  30 | Internal, tuned | OVERCLOCKED, may be unstable.
  24 | External Clock  | OVERCLOCKED, usually works
  25 | External Clock  | OVERCLOCKED, usually works
  30 | External Clock  | OVERCLOCKED, may be unstable
  32 | External Clock  | OVERCLOCKED, may be unstable
When external clock is used as system clock source PA3 cannot be used for any other purpose (obviously) - all control over that pin is taken by CLKCTRL.

The tuned options are new in 2.4.0 - see the [tuned internal oscillator guide](Tuning.md) for more information before using these options.

## WARNING: On pre 2.0.0 versions of megaTinyCore, the UART defaulted to the alternate positions
This matches the megaTinyCore 412/402 Rev. - and Rev. A breakout boards below.. On bootloader . **In 2.0.0 and later, this is no longer the case!** You must call Serial.swap(1) prior to Serial.begin() to move it to the alt pins (or connect your adapter to pins 0 and 1 instead of 2 and 3). It should never have been done that way in the first place. Rev. B of the breakout boards that has the FTDI header tied to the standard serial pins is now available.

## LED_BUILTIN
The `LED_BUILTIN` macro point to pin 4 (PA3); on pre-Rev. B versions of my development board it was on pin 1 (PA7). matching the port and bit used for the higher pin count devices supported by this core, but as one of those pins is used for serial when the pins are not swapped, it was moved tp PA3.

## The issue with bootloader
* There's no dedicated reset pin.
So there is no way to do the traditional autoreset circuit to reset the chip to upload with a bootloader unless you disable UPDI (requiring HV UPDI to undo - I've got a half dozen boards that are bricked until I have time to get an hvupdi programming setup together to resurrect them). Either you manually power cycle it just prior to trying to upload, or you have some sort of ersatz-reset solution coupled to an autoreset circuit, or handle it in some other bespoke way. Regardless of the approach, short of disabling UPDI, none of them are as convenient a development cycle as we're used to. In most cases, the most convenient development configuration is to simply use UPDI programming, and leave any serial connection open while programming via UPDI using a programmer on a different port. Note that the 2-series 20 and 24 pin parts have enhancements that make a bootloader capabloe of providing a better developer experience possible.
* It takes 512b of flash - 1/4th or 1/8th of the total, but offers little, if any, advantages.

## Buy official megaTinyCore breakouts and support continued development
[ATtiny412/402 assembled](https://www.tindie.com/products/17685/)
[ATtiny412/212/402/202 bare board](https://www.tindie.com/products/17749/)

## Datasheets and Errata
See [Datasheet Listing](Datasheets.md)
