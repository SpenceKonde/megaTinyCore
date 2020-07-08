 ## Supported Parts
 These pages contain pinout charts, specs, and datasheet links. Just like the pinout charts for the official boards, these are required reading for even the simplest projects and demonstrations. 
 
* [ATtiny3217,1617,817,417](ATtiny_x17.md)
* [ATtiny3216,1616,816,416](ATtiny_x16.md)
* [ATtiny1614,814,414,214](ATtiny_x14.md)
* [ATtiny412,212](ATtiny_x12.md)
* [ATtiny1607,807](ATtiny_x07.md)
* [ATtiny1606,806,406](ATtiny_x06.md)
* [ATtiny1604,804,404,204](ATtiny_x04.md)
* [ATtiny402,202](ATtiny_x02.md)

## General information
* [README](../../README.md) **(READ IT)** This is the main documentation which describes the state of the standard Arduino API functions and general caveats and key information about these parts, as well as special features of this core
* [Making UPDI programmer](../../MakeUPDIProgrammer.md) These (unless running Optiboot) require a UPDI programmer, not a classic SPI programmer like a USBAsp/etc. Fortunately the ubiquitous and dirt cheap Nano or Pro Mini can be easily adapted to this task, this document explains how.
* [Direct Port Manipulation](DirectPortManipulation.md) Like with the classic AVR parts, you can use direct port manipulation for ultra-fast read and write from pin. The names of the registers are different, and there are some additional caveats, but the sum-total effect of the changes to make the pins and their interrupts more powerful.
* [Manual Pin Interrupts](PinInterrupts.md) Pin interrupt functionality is exposed through the Arduino API via attachInterrupt() - but as ISRs are in blocks of up to 8 pins, the attachInterrupt code has to handle the general case where any number and combination of interrupts are enabled on a given pin. 
* [Sleep modes and power down](PowerSave.md) The same sorts of power savings available on classic AVRs are also available with the new tinyAVR parts - with slight differences in the setup procedures, but few substantial differences. Power-down sleep mode consumption of 0.1uA at 25C is typical assuming you can get all the peripherals turned off (at higher temperatures, power consumption is markedly higher - at 85C, the top of the range, 5uA in powerdown is typical). See the datasheet for more details. 

## Special Libraries
* [tinyNeoPixel - based tightly off of adafruitNeopixel library, optimized for minimized space, with support for all clock speeds megatTinyCore supports higher than 6MHz.](tinyNeoPixel.md)
* Logic (CCL) - the Logic library provides an easy to use wrapper around the "Configurable Custom Logic" blocks. 
* AC (analog comparator) - (library planned for future version) These parts include one or more Analog Comparator units. These can be used to continuously compare an analog value to a reference voltage, generating an interrupt or event based on which voltage is higher. A library which provides a user-friendly wrapper for this functionality is planned for a future release, based on the library in MCUDude's MegaCoreX.
