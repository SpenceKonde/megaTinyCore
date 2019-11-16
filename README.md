[![Join the chat at https://gitter.im/SpenceKonde/megaTinyCore](https://badges.gitter.im/SpenceKonde/megaTinyCore.svg)](https://gitter.im/SpenceKonde/megaTinyCore?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)


### [Wiring](Wiring.md)
### [Installation](Installation.md)
### [Making a cheap UPDI programmer](MakeUPDIProgrammer.md)

# megaTinyCore
Arduino core for the new megaAVR ATtiny series chips. These parts represent the new megaAVR technology (as used in megaAVR chips like the ATmega4809 as used on Nano Every and Uno Wifi Rev. 2) and advanced peripherals in low-cost, small packages of the ATtiny line. All of these parts feature a full hardware UART, SPI and TWI interface, and the 1-series parts have a DAC for analog output as well. Moreover, these parts are *cheap* - the highest end parts, the 3216 and 3217, with 32k of flash and 2k of SRAM (same as the atmega328p used in Uno/Nano/ProMini!) run just over $1, and under 90 cents in quantity - less than many 8k classic AVR ATtiny parts (AVR architecture, at a PIC price). All of these parts will run at 20MHz (at 5v) without an external crystal. 

These use a UPDI programmer, not traditional ISP like the classic ATtiny parts did. One can be made from a classic AVR Uno/Nano/Pro Mini - see [Making a UPDI programmer](MakeUPDIProgrammer.md). 

The Optiboot serial bootloader is now supported (as of 1.1.0) on these parts, allowing them to be programmed via a serial port. See the Optiboot section below for more information on this, and the relevant options. Installing the bootloader does require a UPDI programmer. In the near future, I will be selling pre-bootloaded boards on Tindie.

For this core to work when installed manually, and via board manager for 1.0.1 and earlier, **you must install the official Arduino megaAVR board package** using board manager - this is required to get the compiler and correct version of avrdude installed. If you receive an error of the form "avr-g++: error: device-specs/specs-attiny402: No such file or directory", that indicates that the Arduino megaAVR board package is not installed. As of 1.0.2, this is no longer required if installed via board manager. It is still required for a manual installation. 

## Supported Parts (click link for pinout diagram and details)
* [ATtiny3217,1617,817,417](megaavr/extras/ATtiny_x17.md)
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

## There is no hardware reset pin
The reset functionality is shared with UPDI; hence, when UPDI programming is enabled, there is no hardware reset pin. Luckily, the megaAVR architecture provides the ability to do a software reset:

`_PROTECTED_WRITE(RSTCTRL.SWRR,1);`

You can set an interrupt on a pin to call this in order to create a ersatz reset pin. 

If the optiboot bootloader is used, you may optionally set the UPDI pin to act as reset; in this case it works like the normal reset pin. Note that after doing this, HV programming is needed if future UPDI programming is required. 

# Features

### Memory-mapped flash: No need to declare PROGMEM or use F() macro anymore!
Unlike classic AVRs, on the megaavr parts, the flash is within the same address space as the main memory. This means pgm_read_near functions are not needed to read them. Because of this, the compiler automatically puts any variable declared `const` into progmem, and accesses it appropriately - you no longer need to explicitly declare them PROGMEM. This includes quoted string literals, so the F() macro is no longer needed either (as of 1.0.6, the F() macro is a no-op). 

However, do note that if you explicitly declare a variable PROGMEM, you must still use the pgm_read functions to read it, just like on classic AVRs.

**WARNING** In versions of megaTinyCore 1.0.6 and earlier, the sketch size reported during compilation does not include variables declared const. PROGMEM variables, however, are reported normally. This will be fixed in the next release. 


### Serial (UART) Support
All of these parts have a single hardware serial port (UART). It works exactly like the one on official Arduino boards (except that there is no auto-reset, see note above about the lack of a reset pin). See the pinout charts for the location of the serial pins. Serial output for a few milliseconds after calling Serial.begin() seems to be off - if you are printing a message at the start of the sketch, add a small delay between Serial.begin() and your first Serial.print() call. 

On all parts, the UART pins can be swapped to an alternate location. This is controlled by the Tools -> UART Pins menu (new in 1.1.0)

Note: UART serial is broken on the x12 and x02 parts before version 1.0.3.

### SPI support
All of these parts have a single hardware SPI peripheral. It works exactly like the one on official Arduino boards using the SPI.h library. See the pinout charts for the location of these pins. Note that the 8-pin parts (412, 212, 402, 204) do not have a specific SS pin. 

On all parts except the 14-pin parts, the SPI pins can be moved to an alternate location (note: On 8-pin parts, the SCK pin cannot be moved). This can be configured using the Tools -> SPI Pins submenu; this is set at compile time (reburning bootloader is not required).

### I2C (TWI) support
All of these parts have a single hardware I2C (TWI) peripheral. It works exactly like the one on official Arduino boards using the Wire.h library. See the pinout charts for the location of these pins. 

On all parts with more than 8 pins, the TWI pins can be swapped to an alternate location. This can be configured using the Tools -> I2C Pins submenu; this is set at compile time (reburning bootloader is not required).

### PWM support
The core provides hardware PWM (analogWrite) support. On the 8-pin parts (412, 212, 402, 204), 4 PWM pins are available (1.0.5 and later - 1.0.4 and earlier only have 1). On all other parts except the x16 and x17 series, 6 PWM pins are available, driven by Timer A. The type B timers cannot be used for additional PWM pins - their output pins are the same as those available with Timer A. See the pinout charts for a list of which pins support PWM. 
The 3216,1616,816,416,3217,1617 and 817 have two additional PWM pins driven by Timer D (pins 10 and 11 on x16, 12 and 13 on x17). Timer D is an async timer, and the outputs can't be enabled or disabled without briefly stopping the timer. This results in a brief glitch on the other PWM pin (if it is currently outputting PWM), and doing so requires slightly longer (in 1.0.0, this delay is 1ms, in 1.0.1 and later, it is around 1us). This applies to digitalWrite() or analogWrite of 0 or 255 while it is currently outputting PWM, and analogWrite of 1~254 while the pin is not currently outputting PWM. This is a hardware limitation and cannot be further improved. 

### NeoPixel (WS2812) support (new in 1.0.3)
The usual NeoPixel (WS2812) libraries have problems on these parts. This core includes two libraries for this, both of which are tightly based on the Adafruit_NeoPixel library. See the [tinyNeoPixel documentation](megaavr/extras/tinyNeoPixel.md) and included examples for more information. 

### Tone Support
Support for tone() is provided on all parts using Timer B 0. This is like the standard tone() function; it does not support use of the hardware output compare to generate tones (yet). 

### ADC Support
These parts all have ADC channels available on most pins (11 pins on 24 and 20 pin parts, 9 on 14-pin parts and 5 on 8 pin parts - plus the one on the UPDI pin which is not normally usable), they can be read with analogRead() like on a normal AVR. While the An constants (ex, A0) are supported, and refer to the corresponding ADC channel (not the corresponding pin number), using these is deprecated - the recommended practice is to pass the digital pin number to analogRead(). Analog reference voltage can be selected as usual using analogReference(). Supported reference voltages are:
* VDD (Vcc/supply voltage - default)
* INTERNAL0V55
* INTERNAL1V1
* INTERNAL1V5
* INTERNAL2V5
* INTERNAL4V3
* EXTERNAL (1-series - not including 412/212 - only)

### DAC Support
The 1-series parts have an 8-bit DAC which can generate a real analog voltage (note that this provides very low current and can only be used as a voltage reference, it cannot be used to power other devices). This generates voltages between 0 and the selected VREF (which cannot be VCC, unfortunately) - select the DAC VREF voltage from the Tools -> DAC Voltage Reference submenu. This voltage must be lower than Vcc to get the correct voltages. Call analogWrite() on the DAC pin to set the voltage to be output by the DAC. To turn off the DAC output, call digitalWrite() on that pin. 

### Servo Support
This core provides a version of the Servo library which will select an appropriate timer (Timer B 0, except on the 3216, 3217, 1617,  1616 and 1614, where there is a Timer B 1 available; except on the aforementioned parts, tone cannot be used at the same time as the Servo library). Servo output is better the higher the clock speed - when using servos, it is recommended to run at the highest frequency permitted by the operating voltage to minimize jitter. 

**Warning** If you have installed a version of the Servo library to your <sketchbook>/libraries folder (including via library manager), the IDE will use that version of the library (which is not compatible with these parts) instead of the one supplied with megaTinyCore (which is). As a workaround, a duplicate of the Servo library is included with a different name - to use it, `#include <Servo_megaTinyCore.h>` instead of `#include <Servo.h>`

### printf() support for "printable" class
Unlike the official board packages, but like many third party board packages, megaTinyCore includes the .printf() method for the printable class (used for Serial and many other libraries that have print() methods); this works like printf(), except that it outputs to the device in question; for example:

> Serial.printf("Milliseconds since start: %ld\n", millis());

### Pin Interrupts 
All pins can be used with attachInterrupt() and detachInterrupt(), on RISING, FALLING, CHANGE, or LOW. All pins can wake the chip from sleep on CHANGE or LOW. Pins marked as ASync Interrupt pins on the pinout chart can be used to wake from sleep on RISING and FALLING edge as well. 

Advanced users can instead set up interrupts manually, ignoring attachInterrupt and manipulating the relevant port registers appropriately and defining the ISR with the ISR() macro - this will produce smaller code (using less flash and ram) and the ISRs will run faster as they don't have to check whether an interrupt is enabled for every pin on the port. (A document is planned to describe how to do this). For full information and example, see: [Pin Interrupts](megaavr/extras/PinInterrupts.md)

### Assembler Listing generation
Like ATTinyCore, Sketch -> Export compiled binary will generate an assembly listing in the sketch folder; this is particularly useful when attempting to reduce flash usage, as you can see how much flash is used by different functions.

### EESAVE configuration option
The EESAVE fuse can be controlled via the Tools -> Save EEPROM menu. If this is set to "EEPROM retained", when the board is erased during programming, the EEPROM will not be erased. If this is set to "EEPROM not retained", uploading a new sketch will clear out the EEPROM memory. You must do Burn Bootloader to apply this setting. 
**WARNING** In megaTinyCore 1.0.6 and earlier, this setting is backwards - setting it to retained will not retain, and vise versa. This will be corrected in the next release

### BOD configuration options
These parts support many BOD trigger levels, with Disabled, Active, and Sampled operation options for when the chip is in Active and Sleep modes - Disabled uses the least power, Active uses the most, and Sampled is in the middle. See the datasheet for details on power consumption and the meaning of these options. You must do Burn Bootloader to apply this setting. 

### Link-time Optimization (LTO) support
This core *always* uses Link Time Optimization to reduce flash usage - all versions of the compiler which support the 0-series and 1-series ATtiny parts also support LTO, so there is no need to make it optional as was done with ATtinyCore. 

### Identifying part family within sketch
When writing code that may be compiled for a variety of target chips (which is particularly easy on the megaAVR parts, as the peripherals are so similar), it is often useful to be able to identify which family of parts is being used, in order to, for example, select which pin numbers to use for each purpose. Each part is identified by a #define:
`__AVR_ATtiny***__` where *** is the part number (ex: `__AVR_ATtiny3216__`). 

This core provides two additional #defines for part "families":
* `__AVR_ATtinyx**__` where ** is the last 2 digits of the part number (ex: `__AVR_ATtinyx16`)
* `__AVR_ATtinyxy*__` where * is the last digit of the part number, (ex: `__AVR_ATtinyxy6`). 

This is just shorthand, for convenience - `#ifdef __AVR_ATtinyxy2__` is equivilent to `#if defined(__AVR_ATtiny212__) || defined(__AVR_ATtiny412__) || defined(__AVR_ATtiny202__) || defined(__AVR_ATtiny402__)`

# Bootloader (optiboot) Support
A new version of Optiboot (Optiboot-x) now runs on the Tiny0 and Tiny1 chips.  It's still under 512 bytes, so it will potentially work on any of the chips. including EEPROM support. Optiboot supports all boards supported by megaTinyCore. 

To use the serial bootloader, select a board definition with (optiboot) after it (note - this might be cut off due to the width of the menu; the second set of board definitions are the optiboot ones). Unless the chip has already been bootloaded, you must connect a UPDI programmer, select your desired options, and do Tools -> Burn Bootloader to load the bootloader onto the chip. 

After this, connect a serial adapter to the serial pins (as well as ground and Vcc). On the boards which I sell on Tindie, a standard 6-pin "FTDI" header is provided for this, and "upload" your sketch normally. 

If the UPDI/Reset pin option was set to UPDI when bootloading, you must unplug/replug the board (or use a software reset - see note near start of readme; you can make an ersatz reset button this way) to enter the bootloader - after this the bootloader will be active for 8 seconds. 

If the UPDI/Reset pin option was set to reset, you must reset the chip via the reset pin (or software reset) to enter the bootloader, and the bootloader will run for 1 second before jumping to the application. This is the same as how optiboot works on classic AVR boards. The standard DTR-autoreset circuit is recommended (this is how boards I sell on Tindie with Optiboot preloaded are configured).

Serial uploads are done at 115200 baud. 

### Autoreset circuit
Connect the DTR pin of the serial adapter to one side of a 0.1uF ceramic capacitor, the other side of the capacitor is connected to the Reset pin. Connect a 10k resistor between Reset and Vcc, and a diode between Reset and Vcc with the band towards Vcc. Note that these parts must be connected AFTER the pin is set to act as Reset instead of UPDI, as it will interfere with UPDI programming. The breakout boards I sell on Tindie have a pair of pads on the back that can be bridged with solder to reversibly connect the on-board autoreset circuit to the UPDI/Reset pin. Note that this same circuit can be used if using a software reset and pin interrupt to create an ersatz reset pin (connect to that pin, instead of reset). 

### A few caveats: 
* The bootloader is at the beginning of memory, rather than at the end (where it was on older chips.). Thus, the start of the application code must be 512b after the start of the memory - this is handled by the core, but you cannot upload a .hex file compiled with a non-optiboot board definition to an optiboot board definition and vise versa. 
* **If you have set the UPDI/Reset pin to act as a reset pin, you can no longer program the part via UPDI** without using an HV programmer to reset the pin to act as UPDI. 
* Currently, Optiboot_x resets the reset cause register after saving the contents in R2.  
* The new chips have more than one option for Uart Pins. The option selected when you "burn bootloader" determines which version is used. If using the breakout boards I sell on Tindie, the first menu option uses the pins connected to the 6-pin FTDI-style serial header. 
* When you "burn bootloader", the base oscillator frequency is set according to the selected clock speed, but the actual operating speed while running the sketch is set in the uploading code. If you initially set it to 16/8/4/1MHz, you may use any of those options when you upload your sketch and it will run at that speed; if you initially set it to 20/10/5MHz, you may use any of those options. If you wish to change between 16/8/4/1MHz and 20/10/5MHz, you must burn bootloader again - failure to do so will result in it running at the wrong speed, and all timing will be wrong. 

# Guides
### [Power Saving techniques and Sleep](https://github.com/SpenceKonde/megaTinyCore/blob/master/megaavr/extras/PowerSave.md)
### [Direct Port Manipulation](https://github.com/SpenceKonde/megaTinyCore/blob/master/megaavr/extras/DirectPortManipulation.md)

# List of Tools sub-menus
* Tools -> Chip - sets the specific part within a selected family to compile for and upload to.
* Tools -> Clock Speed - sets the clock speed. You must burn bootloader after changing between 16/8/4/1MHz and 20/10/5MHz to apply the changes (ie, changing from 20MHz to 10MHz does not require burning bootloader, changing from 20MHz to 16MHz does). A virgin chip will be set to use the 20MHz internal oscillator as it's base clock source, so 20/10/5MHz clock speed options should work without an initial "burn bootloader" - though we still recommend it to ensure that all other fuses are set to the values you expect.
* Tools -> Retain EEPROM - determines whether to save EEPROM when uploading a new sketc. You must burn bootloader after changing this to apply the changes. This option is not available on Optiboot board definitions - programming through the bootloader does not execute a chip erase function. 
* Tools -> UART pins - All of these parts have the potential to remap the UART pins. This menu option sets which pins will be used as TX and RX by default. If Optiboot is being used, the selection from this menu when you "burn bootloader" will set which set of pins will be used by Optiboot (however, once the bootloader has been uploaded, you may change this if you need the sketch to use the other set of pins - the pins chosen when the bootloader was burned will still be used for uploads, but the current selection will be used by the sketch). 
* Tools -> B.O.D. Voltage - If Brown Out Detection is enabled, when Vcc falls below this voltage, the chip will be held in reset. You must burn bootloader after changing this to apply the changes. Take care that these threshold voltages are not exact - they may vary by as much as +/- 0.3v! (depending on threshold level - see electrical characteristics section of datasheet). Be sure that you do not select a BOD threshold voltage that could be triggered during programming, as this can prevent successful programming via UPDI (reported in #86).
* Tools -> UPDI/Reset pin - This menu is only available on board definitions that use the Optiboot bootloader. If set to UPDI, the pin will be left as the UPDI pin, there will be no hardware reset pin - to enter the bootloader, disconnect and reconnect power to the part, and upload within 8 seconds. If set to Reset, the pin will be configured to act as reset, like a classic AVR, but **UPDI programming will no longer work - you must use an HV programmer if you wish to reenable UPDI** - if the pin is set to reset, the version of Optiboot uploaded will jump straight to the application after a power-on reset, and will only enter the bootloader if reset by software reset or via the reset pin. The bootloader will also only wait 1 second for a sketch (ie, it behaves like optiboot does on classic AVR microcontrollers), instead of 8. 
* Tools -> B.O.D. Mode (active) - Determines whether to enable Brown Out Detection when the chip is not sleeping. You must burn bootloader after changing this to apply the changes.
* Tools -> B.O.D. Mode (sleep) - Determines whether to enable Brown Out Detection when the chip is sleeping. You must burn bootloader after changing this to apply the changes.
* Tools -> DAC Reference Voltage - Determines the voltage reference for the DAC. This should be less than Vcc to get the right voltage. You do not need to use Burn Bootloader to apply changes to this menu.
* Tools -> tinyNeoPixel Port - If using the tinyNeoPixel library (see above), and you are running at 8 or 10 MHz, you must set this option to the port with the pin(s) you are using. Not present on the 8-pin parts, as they only have one port. If not using tinyNeoPixel library or running at 16MHz or more, this option can be ignored.
* Tools -> millis()/micros() - If set to enable (the default), millis(), micros() and pulseInLong() will be available. If set to disable, these will not be available, Serial methods which take a timeout as an argument will not have an accurate timeout (though the actual time will be proportional to the timeout supplied); delay will still work. Disabling millis() and micros() saves flash, and eliminates the millis interrupt every 1-2ms; this is especially useful on the 8-pin parts which are extremely limited in flash.

# Known Compiler Bugs
* Sometimes a sketch which is too big to fit will, instead of generating a message saying that, result in the error: 'relocation truncated to fit: R_AVR_13_PCREL against symbol tablejump2'.

# Buying ATtiny megaAVR breakout boards
I sell breakout boards with regulator, UPDI header, and Serial header in my tindie shop, as well as the bare boards. Buying from my store helps support further development on the core, and is a great way to get started using these exciting new parts with Arduino. 

### [ATtiny3217, 1607 assembled](https://www.tindie.com/products/17523/)
### [ATtiny3217/1617/817/417/1607/807 bare board](https://www.tindie.com/products/17613/)
### [ATtiny3216, 1606 assembled](https://www.tindie.com/products/17597/)
### [ATtiny3216/1616/816/416/1606/806/406 bare board](https://www.tindie.com/products/17614/)
### [ATtiny1614, 1604 assembled](https://www.tindie.com/products/17598/)
### [ATtiny1614/814/414/1604/804/404 bare board](https://www.tindie.com/products/17748/)
### [ATtiny412, 402 assembled](https://www.tindie.com/products/17685/)
### [ATtiny412/212/402/202 bare board](https://www.tindie.com/products/17749/)
