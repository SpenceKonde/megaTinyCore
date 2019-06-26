[![Join the chat at https://gitter.im/SpenceKonde/megaTinyCore](https://badges.gitter.im/SpenceKonde/megaTinyCore.svg)](https://gitter.im/SpenceKonde/megaTinyCore?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

### [Wiring](Wiring.md)
### [Installation](Installation.md)
### [Making a cheap UPDI programmer](MakeUPDIProgrammer.md)

# Core status
* Lots of stuff untested, please help test if you have the hardware!
* Burn bootloader and clock speeds work. Update core and restart IDE BEFORE burning bootloader if you have been using 1.0.0-dev versions prior to 6/22/2019, there was a nasty bug in boards.txt (see #49, very recently fixed) that would brick chips when burning bootloader!
* PWM works on the x16 series. Belived working on other parts. 
* PWM on the Timer D pins (10, 11 on x16, 12,13 on x17) takes 1 ms to turn on or off, and during this time, the other Timer D PWM channel will be disabled. This applies to digitalWrite() or analogWrite of 0 or 255 while it is currently outputting PWM, and analogWrite of 1~254 while the pin is not currently outputting PWM. We are looking at a better solution (#51)
* analogRead works on the x16 series. Belived working on other parts. 
* tone() should work, at least, it compiles on all chips and there's a timer for it to use. Let us know if it doesn't generate tones.
* Serial works (not tested on the x12 and x02 series - these have to use the alternate pin mapping, so am less confident this will work) 
* SPI, TWI untested. 
* Upload uses jtag2updi programmer. Upload using programmer and burn bootloader use the selected programmer. 


# megaTinyCore
Arduino core for the new megaAVR ATtiny series chips - highly experimental. These parts represent the new megaAVR technology (as used in megaAVR chips like the ATmega4809 as used on Nano Every and Uno Wifi Rev. 2) and advanced peripherals in low-cost, small packages of the ATtiny line. 

These use a UPDI programmer, not traditional ISP like the classic ATtiny parts did. One can be made from a classic AVR Uno/Nano/Pro Mini - see [Making a UPDI programmer](MakeUPDIProgrammer.md)

## Supported Parts
* [ATtiny3217,1617,817](megaavr/extras/ATtiny_x17.md)
* [ATtiny3216,1616,816,416](megaavr/extras/ATtiny_x16.md)
* [ATtiny1614,814,414,214](megaavr/extras/ATtiny_x14.md)
* [ATtiny412,212](megaavr/extras/ATtiny_x12.md)
* [ATtiny1607,807](megaavr/extras/ATtiny_x07.md)
* [ATtiny1606,806,406](megaavr/extras/ATtiny_x06.md)
* [ATtiny1604,804,404,204](megaavr/extras/ATtiny_x04.md)
* [ATtiny402,202](megaavr/extras/ATtiny_x02.md)

## Supported Clock Speeds
* 20MHz Internal (4.5v~5.5v - typical for 5v systems)
* 16MHz Internal (4.5v~5.5v - typical for 5v systems)
* 10MHz Internal (2.7v~5.5v - typical for 3.3v systems)
* 8MHz Internal (2.7v~5.5v - typical for 3.3v systems)
* 5MHz Internal (1.8v~5.5v)
* 4MHz Internal (1.8v~5.5v)
* 1MHz Internal (1.8v~5.5v)

These parts do not support using an external crystal like the classic ATtiny parts do, however the internal oscillator is tightly calibrated enough that the internal clock will work for UART communication. 

# Features

### Serial (UART) Support
All of these parts have a single hardware serial port (UART). It works exactly like the one on official Arduino boards. See the pinout charts for the location of the serial pins. 

### SPI support
All of these parts have a single hardware SPI peripheral. It works exactly like the one on official Arduino boards using the SPI.h library. See the pinout charts for the location of these pins. Note that the 8-pin parts (412, 212, 402, 204) do not have a specific SS pin. 

### I2C (TWI) support
All of these parts have a single hardware I2C (TWI) peripheral. It works exactly like the one on official Arduino boards using the Wire.h library. See the pinout charts for the location of these pins.

### PWM support
The core provides hardware PWM (analogWrite) support. On the 8-pin parts (412, 212, 402, 204), only a single PWM pin is available (this is a hardware limitation - if you don't like it, direct your complaints to Atmel/Microchip). On all other parts except the x16 and x17 series, 6 PWM pins are available, driven by Timer A. On the x16 and x17 series, two additional pins are available using Timer D - however note issue #51. The type B timers cannot be used for additional PWM pins - their output pins are the same as those available with Timer A. See the pinout charts for a list of which pins support PWM. 

### Tone Support
Support for tone() is provided on all parts using Timer B 0. This is like the standard tone() function; it does not support use of the hardware output compare to generate tones (yet). 

### EESAVE configuration option
The EESAVE fuse can be controlled via the Tools -> Save EEPROM menu. If this is set to "EEPROM retained", when the board is erased during programming, the EEPROM will not be erased. If this is set to "EEPROM not retained", uploading a new sketch will clear out the EEPROM memory. You must do Burn Bootloader to apply this setting. 

### BOD configuration options
These parts support many BOD trigger levels, with Disabled, Active, and Sampled operation options for when the chip is in Active and Sleep modes - Disabled uses the least power, Active uses the most, and Sampled is in the middle. See the datasheet for details on power consumption and the meaning of these options. You must do Burn Bootloader to apply this setting. 

### DAC Support (supported as of 1.0.1)
The 1-series parts have an 8-bit DAC which can generate a real analog voltage, instead of PWM (note that this provides very low current and can only be used as a voltage reference, it cannot be used to power other devices). This generates voltages between 0 and the selected VREF (which cannot be VCC) - select the DAC VREF voltage from the Tools -> DAC Voltage Reference submenu. This voltage must be lower than Vcc to get the correct voltages. Call analogWrite() on the DAC pin to set the voltage to be output by the DAC. To turn off the DAC output, call digitalWrite() on that pin. 

### Servo Support (not yet implemented)
A future version of this core will provide a version of the Servo library which will select an appropriate timer (Timer B 0, except on the 3216, 3217, 1617 and 1616, where there is a Timer B 1 available; except on the aforementioned parts, tone cannot be used at the same time as the Servo library).

### Bootloader Support (not yet implemented)
When Optiboot is available for the ATmega4809, it will be adapted for these ATtiny parts, and the bootloader added to this core for chips with 8k or more of flash - possibly 4k chips if the bootloader ends up being small enough. 

### Assembler Listing generation
Like ATTinyCore, Sketch -> Export compiled binary will generate an assembly listing in the sketch folder; this is particularly useful when attempting to reduce flash usage, as you can see how much flash is used by different functions.

# List of Tools sub-menus
* Tools -> Chip - sets the specific part within a selected family to compile for and upload to. 
* Tools -> Clock Speed - sets the clock speed. You must burn bootloader after changing this to apply the changes. 
* Tools -> Retain EEPROM - determines whether to save EEPROM when uploading a new sketc. You must burn bootloader after changing this to apply the changes. 
* Tools -> B.O.D. Voltage - If Brown Out Detection is enabled, when Vcc falls below this voltage, the chip will be held in reset. You must burn bootloader after changing this to apply the changes. 
* Tools -> B.O.D. Mode (active) - Determines whether to enable Brown Out Detection when the chip is not sleeping. You must burn bootloader after changing this to apply the changes. 
* Tools -> B.O.D. Mode (sleep) - Determines whether to enable Brown Out Detection when the chip is sleeping. You must burn bootloader after changing this to apply the changes. 
* Tools -> DAC Reference Voltage - Determines the voltage reference for the DAC. This should be less than Vcc to get the right voltage. You do not need to use Burn Bootloader to apply changes to this menu. 

# Known Compiler Bugs
* Sometimes a sketch which is too big to fit will, instead of generating a message saying that, result in the error: 'relocation truncated to fit: R_AVR_13_PCREL against symbol tablejump2'
