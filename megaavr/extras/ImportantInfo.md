# Index of Important Information
Since people frequently miss it, it is hoped that this document will help steer people to key information relevant to the core.

## Supported Parts
These pages contain pinout charts, specs, and datasheet links. Just like the pinout charts for the official boards, these are required reading for even the simplest projects and demonstrations.

* [ATtiny3227, 1627, 827, 427](ATtiny_x27.md)
* [ATtiny3226, 1626, 826, 426](ATtiny_x26.md)
* [ATtiny3224, 1624, 824, 424](ATtiny_x24.md)
* [ATtiny3217, 1617, 817, 417](ATtiny_x17.md)
* [ATtiny3216, 1616, 816, 416](ATtiny_x16.md)
* [ATtiny1614, 814, 414, 214](ATtiny_x14.md)
* [ATtiny412, 212](ATtiny_x12.md)
* [ATtiny1607, 807](ATtiny_x07.md)
* [ATtiny1606, 806, 406](ATtiny_x06.md)
* [ATtiny1604, 804, 404, 204](ATtiny_x04.md)
* [ATtiny402, 202](ATtiny_x02.md)

## General information
* [README](../../README.md) **(READ IT)** This is the main documentation which describes the state of the standard Arduino API functions and general caveats and key information about these parts, as well as special features of this core. There are sections of that file covering most significant areas of megaTinyCore, including sections on I2C, SPI, and reading analog voltages, including the advanced functionality of the ADC in the 2-series parts, and much more.
* [Making UPDI programmer](../../MakeUPDIProgrammer.md) These (unless running Optiboot) require a UPDI programmer, not a classic SPI programmer like a USBAsp/etc. Fortunately a dirt cheap serial adapter can be used as a UPDI programmer - or a cheap nano clone can be loaded with jtag2updi - either way gets you a UPDI programmer for the cost of a cup of coffee.
* [Direct Port Manipulation](DirectPortManipulation.md) Like with the classic AVR parts, you can use direct port manipulation for ultra-fast read and write from pin. The names of the registers are different, and there are some additional caveats, but the sum-total effect of the changes to make the pins and their interrupts more powerful.
* [Manual Pin Interrupts](PinInterrupts.md) Pin interrupt functionality is exposed through the Arduino API via attachInterrupt() - but as ISRs are in blocks of up to 8 pins, the attachInterrupt code has to handle the general case where any number and combination of interrupts are enabled on a given pin.
* [Sleep modes and power down](PowerSave.md) The same sorts of power savings available on classic AVRs are also available with the new tinyAVR parts - with slight differences in the setup procedures, but few substantial differences. Power-down sleep mode consumption of 0.1uA at 25C is typical assuming you can get all the peripherals turned off (at higher temperatures, power consumption is markedly higher - at 85C, the top of the range, 5uA in powerdown is typical). See the datasheet for more details.
* [Names of the interrupt vectors on all parts](InteruptVectorNames.md) because for reasons I do not understand, it's a warning, not an error, to make a typo in the name of a vector; the compiled result hangs if this improperly defined interrupt is triggered.
* [Hardware timers and how this core uses them for PWM and millis](PWMandTimers.md) is documented in this document. PWM frequencies are designed to be similar to popular classic AVR devices by default.
* [Manufacturer specified "speed grade" that the 0/1-series are rated for](SpeedGrades.md) are documented here, and are extremely conservative.
* ["Tuning" the internal oscillator to different speeds](Tuning.md) is supported. You can use the inclucded sketch as described here with a trusted timebase to generate calibrated values to write to USERROW, and have the core use those - but if there are no such constants stored, we'll take our best guess.

## Special Libraries
* [tinyNeoPixel - based closely off of adafruitNeopixel library](tinyNeoPixel.md), optimized for minimized space, with support for all clock speeds megatTinyCore supports higher than 6MHz.
* [Logic](../libraries/Logic/README.md) (CCL) - the Logic library provides an easy to use wrapper around the "Configurable Custom Logic" blocks. The API matches DxCore and MegaCoreX versions of the library.
* [Comparator](../libraries/Comparator/README.md) interfaces with the analog comparator or comparators on these devices. The API matches DxCore and MegaCoreX versions of the library.
* [Event](../libraries/Event/README.md) for working with the Event System (EVSYS). The API matches DxCore and MegaCoreX versions of the library, which is to say that it doesn't address many of the key pain points you're likely to encounter when working with the event system.
* [USERSIG](../libraries/USERSIG/README.md) for storing data in the User Signature Space (often called the "USERROW" but we can't call the library that because it's already defined by the io headers). Same API as EEPROM library.
* Servo and Servo_megaTinyCore for controlling standard hobby servos. If you've installed the Servo library through library manager, you will need to use Servo_megaTinyCore.h in order to use the (compatible) version included with the core instead of the (incompatible) one installed in libraries folder. Both copies of the library have the same API as normal Servo.h.
* [Optiboot_flasher](../libraries/Optiboot_flasher) for calling into the Optiboot bootloader to write to the flash from application code. Like MegaCoreX's version and similar to some classic Optiboot-supported parts, and totally different from DxCore's Flash.h.
