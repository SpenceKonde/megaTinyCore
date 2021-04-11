## [Check it out, we have "discussions" now!](https://github.com/SpenceKonde/megaTinyCore/discussions)
Let's use that - it's better than gitter ever was, and it's all on one site.

## [A 2.3.0 version with full functionality is DANGEROUSLY CLOSE](https://github.com/SpenceKonde/megaTinyCore/discussions/377)

### [Wiring](Wiring.md)
### [Installation](Installation.md)

# megaTinyCore
Arduino core for the tinyAVR 0-series and 1-series chips. These parts have an improved architecture, with improved peripherals and improved execution time for certain instructions (similar to megaAVR 0-series chips like the ATmega4809 as used on Nano Every and Uno Wifi Rev. 2) in low-cost, small packages of the ATtiny line. All of these parts feature a full hardware UART, SPI and TWI interface, and the 1-series parts have a DAC for analog output as well. Moreover, these parts are *cheap* - the highest end parts, the 3216 and 3217, with 32k of flash and 2k of SRAM (same as the atmega328p used in Uno/Nano/ProMini!) run just over $1 USD, and under $.90 in quantity - less than many 8k classic AVR ATtiny parts (AVR instruction set, at a PIC price). All of these parts will run at 20MHz (at 5v) without an external crystal and the internal oscillator is accurate enough for UART communication.

These use a UPDI programming, not traditional ISP like the classic ATtiny parts did. See below for more information.

The Optiboot serial bootloader is now supported on these parts, allowing them to be programmed via a serial port. See the Optiboot section below for more information on this, and the relevant options. Installing the bootloader does require a UPDI programmer. The assembled breakout boards I sell on Tindie are available pre-bootloaded (they are bootloaded on demand) - however, at least until we get 2-series parts with the alternate reset pin option, it is suggested to stick to UPDI programming

**Arduino 1.8.13 is recommended**

This core depends on the 7.3.0-atmel3.6.1-arduino7 version of the toolchain. For this core to work **when installed manually**, you must have either the latest official Arduino AVR board package, 1.8.3 (included with Arduino 1.8.13, and available for older versions of the IDE by upgrade through board manager).  When megaTinyCore is installed through board manager, the required version of the toolchain is always installed automatically.

# UPDI programming
The UPDI programming interface is a single-wire interface for programming (and debugging - **U**niversal **P**rogramming and **D**ebugging **I**nterface) used on the tinyAVR 0/1/2-series, as well as all other modern AVR microcontrollers. In addition to purchasing a purpose-made UPDI programmer (such as the ones produced by Microchip), there are two very low-cost approaches to creating a UPDI programmer:

## With an Arduino (jtag2updi)
One can be made from a classic AVR Uno/Nano/Pro Mini; inexpensive Nano clones are the usual choice, being cheap enough that one can be wired up and then left like that - see [Making a UPDI programmer](MakeUPDIProgrammer.md); using these, you should select jtag2updi from the tools->programmer menu. Prior to the release of 2.2.0, this was the recommended method, despite the balky jtag2updi firmware and incompatibility with converting an Arduino Micro,

## From a USB-Serial adapter (pyupdi-style)
Before megaTinyCore existed, there was a tool called [pyupdi](https://github.com/mraardvark/pyupdi) - a simple python program for uploading to UPDI-equipped microcontrollers using a serial adapter modified by the addition of a single resistor. But pyupdi was not readily usable from the Arduino IDE, and so this was not an option. As of 2.2.0, megaTinyCore brings in a portable Python implementation, which opens a great many doors; Originally we were planning to adapt pyupdi, but at the urging of its author and several Microchip employees, we have instead based this functionality on [pymcuprog](https://pypi.org/project/pymcuprog/), a tool developed and maintained by Microchip which includes the same serial-port upload mechanism. **If installing manually** you must [add the python package](megaavr/tools/ManualPython.md) appropriate to your operating system in order to use this upload method (a system python installation is not sufficient, nor is one necessary).

Connections:
* Vcc, Gnd of serial adapter to Vcc, Gnd of target
* 4.7k resistor between Tx and Rx of adapter (many adapters have built-in 1k resistor in series with Tx; these should use a smaller resistor)
* Rx of adapter to UPDI pin of target. A small resistor (under 1k - like the 470 ohm one we generally recommend) in series with this is fine.

Choose "Serial Port and 4.7k" from the Tools -> Programmer menu, and select the Serial Port from the Tools -> Port menu.

**WARNING** - Prior to 2.2.8 this did not work for "burn bootloader" with Optiboot parts; success was reported, but only fuses were written, not flash.

Note that this does not give you serial monitor - you need to connect a serial adapter the normal way for that (I suggest using two, along with an external serial terminal application (since you don't have autoreset to restart the sketch when open the serial port - you can connect connect before (or during) the upload). This technique works with those $1 CH340 serial adapters from ebay, aliexpress, etc. Did you accidentally buy some that didn't have a DTR or RTS pin broken out, and so weren't very useful with the Pro Minis you hoped to use them with? Now you have something to do with them - they'll work in either of these roles.

### Serial adapter requuirements
Almost any cheaper-than-dirt serial adapter can be use d for pyupdi style programer, as long as you take care to avoid these pitfalls:
1: The FTDI FT232, (both the genuine ones, and the fakes) are known to be SLOW. It looks like they wait for more data to come to send it all at once more "efficiently"?
2. Many serial adapters have a resistor, typically between 1k and 2.2l in series with their TX line; If yours has one, just reduce the value of the resistor between Tx and Tx by about that much.
3. Some serial adapters have a dedicated LED to indicate receiving. While some fancy chips have an I/O pin that drives the RX led (the FT232 has that feature I think), a cheap adapter with an RX just put an LED and resistor on the RX line.  The load from an LED on the UPDI line will overwhelm any signal and prevent communication. Detecting teceiv

**Note:** These are the requirements for programming through the UPDI pin using the serial adapter; these are not the requirements for programming through a bootloader installed on the chip; that is covered below.

# Supported Parts (click link for pinout diagram and details)
* [ATtiny3217,1617,817,417](megaavr/extras/ATtiny_x17.md)
* [ATtiny3216,1616,816,416](megaavr/extras/ATtiny_x16.md)
* [ATtiny1614,814,414,214](megaavr/extras/ATtiny_x14.md)
* [ATtiny412,212](megaavr/extras/ATtiny_x12.md)
* [ATtiny1607,807](megaavr/extras/ATtiny_x07.md)
* [ATtiny1606,806,406](megaavr/extras/ATtiny_x06.md)
* [ATtiny1604,804,404,204](megaavr/extras/ATtiny_x04.md)
* [ATtiny402,202](megaavr/extras/ATtiny_x02.md)

## tinyAVR 2-series
The tinyAVR 2-series support is basicallly complete in 2.3.0, however, expect to find issues. Please report them when you do. Among other things, I'm unsure if bootloaders will wwork when the default pins are used for the serial port, and I know they won't work if alternate pins are.
* [ATtiny3227,1627,827,427](megaavr/extras/ATtiny_x27.md)
* [ATtiny3226,1626,826,426](megaavr/extras/ATtiny_x26.md)
* [ATtiny3224,1624,824,424](megaavr/extras/ATtiny_x24.md)

## Overall Part Comparison

## Automotive (VAO) versions
The automotive versions should also work. You must always select the 16 MHz-derived clock speeds on these parts. They do not support 20 MHz operation.

# A word on terminology ("megaAVR")
In the official Arduino board definition for their "megaAVR" hardware package, they imply that the new architecture on the megaAVR 0-series parts (which is nearly the same as used on the tinyAVR 0-series and 1-series) is called "megaavr" - that is not an official term. Microchip uses the term "megaAVR" to refer to any "ATmega" part, whether it has the old style or modern peripherals. There are no official terms to refer to all AVR parts of one family or the other. In this document, prior to 2.0.2, we used the Arduino convention. While that was nearly a year ago, I still keep finding places where I call them megaAVR. Please report them as issues if you see any.
Do note that the terms `avr` and `megaavr` are still used internally (for example, in libraries, to mark which parts a given library is compatible with, or separate different versions of a file based on what they will run on). This will continue - we have to stick with this for compatibility with what the Arduino team started with the core for the Uno WiFi Rev. 2 and Nano Every - and in any event, *some word* is needed to refer to the two groups and Microchip hasn't pr9vided one. Not sure how they managed to develop these very different parts without ever talking about then as a group.

It is unfortunate that there are not officially sanctioned terms for these two classes of AVR microcontrollers. I have beemn referring the pre-2016 AVR devices (with PORTx, DDRx, etc registers for pins) as "classic AVR" and the ones Arduino calls megaavr as "modern AVR"

# Buying tinyAVR 0-series and 1-series Breakout Boards
I sell breakout boards with regulator, UPDI header, and Serial header in my tindie shop, as well as the bare boards. Buying from my store helps support further development on the core, and is a great way to get started using these exciting new parts with Arduino. I do not yet have 2-series boards available, and to get the most from the 20 and 24-pin parts, I will need to do a board revision to use the alt-reset pin. The 14-pin parts' board revision will be largely cosmetic (all of these boards are getting a new color scheme so you can read the damned text.

### [ATtiny3217, 1607 assembled](https://www.tindie.com/products/17523/)
### [ATtiny3217/1617/817/417/1607/807 bare board](https://www.tindie.com/products/17613/)
### [ATtiny3216, 1606 assembled](https://www.tindie.com/products/17597/)
### [ATtiny3216/1616/816/416/1606/806/406 bare board](https://www.tindie.com/products/17614/)
### [ATtiny1614, 1604 assembled](https://www.tindie.com/products/17598/)
### [ATtiny1614/814/414/1604/804/404 bare board](https://www.tindie.com/products/17748/)
### [ATtiny412, 402 assembled](https://www.tindie.com/products/17685/)
### [ATtiny412/212/402/202 bare board](https://www.tindie.com/products/17749/)


## TinyAVR 2-series

## Basic specifications
Datasheet for the new tinyAVR 2-series - at least the 16k ones - http://ww1.microchip.com/downloads/en/DeviceDoc/ATtiny1624-26-27-DataSheet-DS40002234A.pdf
No 8-pin parts. While datasheet only "covers" the 16k parts, they say that there are no differences in features between parts with the same pin count (so no "golden" parts like the 16k/32k 1-series, with special features), only between parts with different pin counts. 14, 20, and 24 pin parts are all listed with 4k, 8k, 16k and 32k flash; these, respectively, come with 512, 1024, 2048, and 3072 bytes of SRAM, 4/8k parts get 128 bytes of EEPROM, the larger ones get 256. 14-pin parts come in SOIC and TSSOP, 20-pin in (wide) SOIC, SSOP, and that itty-bitty QFN like the 1616 (think we'll get the 3226 in that package too? We can hope...), and 24-pin in the same VQFN as the 3217.

TWI, SPI, USART0, AC0, are unchanged, as is NVMCTRL. (existing bootloaders will work unaltered, at least on USART0... ). Clock options unchanged. TCB0 and TCB1 got upgraded to the version in the Dx-series (ie. clock off event option, cascade, and we get separate INTCTRL bits for OVF and CAPT - nice additions, but nothing relevant to the core itself). We now get 4 CCL LUTs and 2 sequencers, instead of 2 and 1 - and they can fire interrupts like other parts with CCL (and unlike the tinyAVR 0/1-series). Second USART added as expected (that noise you hear are the ATtiny841 and 1634 sobbing in the corner). PORTMUX registers now named like the rest of the modern AVRs, though we still have the individual control over TCA0 channel positions. EVSYS now works like it does on non-tinyAVR-0/1-series parts (nice, standardization). TCD0, AC1/2, DAC0, and ADC1 are gone. And ADC0 is INSANELY FANCY and almost unrecognizable! (queue another agonized wail from the poor '841, which also had an incredibly fancy ADC with differential options)... judging by the volume of posts on different topics, I have a sense that differential ADC wasn't at the top of most of your wish-lists, but we are getting one.

And it is really *really* fancy. The ADC gets a programmable gain amplifier (1x ~ 16x), single-ended and differential mode, a new series accumulation mode (where each sample is triggered by an event) and burst accumulation mode (like our current accumulate mode) - and every sample taken is made individually available, too (and you can have it generate interrupt on each sample or only the final result. They make a big deal about using the accumulate mode to do 1024 sample oversampling - that's 4<sup>5</sup>, so decimation shouold let us take meaningful 17-bit ADC readings. Rather annoyingly, it only offers 8 and 12-bit resolution options (no 10-bit - implements 10-bit reference option by rightshifting a 12-bit reading). It, also supports ADC clock between 300khz and 6 MHz (3 MHz with internal reference; we try for approx. 2.5 MHz); to make up for the faster ADC clock, sample duration can be up to 256 ADC clocks; with minimum sample duration, over 300k samples/second is possible. It has more rigorous functionality to time the reference and input settling time too - no more throwing away the first ADC reading? (with parts in hand, I'm going to back away from this claim). This new ADC is incredibly powerful - though my initial tests indicate that the offset error is surprisingly high, and countermeasures may  be needed. The PGA can give up to 16x amplification - and it isn't only differential readings either!

megaTinyCore provides an analogRead() immplementation, and more powerful functions to use the oversampling, PGA, and so on.

Oh, and one more thing... the UPDI pin configuration has the old options - UPDI, I/O, or Reset... and a new one: UPDI + alternate RESET pin on PB4! Optiboot will finally be a viable and comfortable option! Well, on the parts that have a PB4, ie, not the 14-pin parts. Which also happen to be (if my Tindie store is any indication) the most popular package. There's always the [DD-series](http://ww1.microchip.com/downloads/en/DeviceDoc/AVR-DD-Product-Brief-DS40002215A.pdf)... That's a matter for [DxCore](https://github.com/SpenceKonde/DxCore) though...

# megaTinyCore and tinyAVR 0/1/2-series Features
These parts provide an excellent toolbox of versatile and powerful peripherals; the top-end ones are nearly on a par with classic megaAVR parts - for a tinyAVR price. One of the guiding principles of the design of megaTinyCore, as with my other cores, is to allow the supported parts to reach their full potential - or as close to that as possible within the limitations of Arduino. This (very large) section covers the features of these parts and how they are exposed by megaTinyCore, as well as features of the core itself.

## Supported Clock Speeds
* 20MHz Internal (4.5v~5.5v - typical for 5v systems)
* 16MHz Internal (4.5v~5.5v - typical for 5v systems)
* 10MHz Internal (2.7v~5.5v - typical for 3.3v systems)
* 8MHz Internal (2.7v~5.5v - typical for 3.3v systems)
* 5MHz Internal (1.8v~5.5v)
* 4MHz Internal (1.8v~5.5v)
* 1MHz Internal (1.8v~5.5v)
* 20MHz External Clock (4.5v ~ 5.5v)
* 16MHz External Clock (4.5v ~ 5.5v)
* 10MHz External Clock (2.7v ~ 5.5v)
* 8MHz  External Clock (2.7v ~ 5.5v)
* 24MHz External Clock (Overclocked)
* 28MHz External Clock (Overclocked)
* 32MHz External Clock (Overclocked - unlikely to work - 0/1-series they seem to start acting oddly at around 30 MHz at room temperature)

See [Speed Grades](megaavr/extras/SpeedGrades.md) for more information on the manufacturer's speed grades. Note that those are the voltages and clock speeds at which it is guaranteed to work, where an unexpected glitch of some description could pose a hazard to persons or property. Under favorable temperatures (ie, room temperature), and with higher tolerance for potential stability issues than a commercial customer, the speed grades can easily be beaten. *Initial testing in 2.1.x using the newly added support for external clock sources suggests that the speed grades are conservative indeed* - I was able to run the parts I tested at 32MHz at room temperature at 5V, and it's been widely observed that they seem to work fine at their full 20 MHz 4.5V ~ 5.5V speed running from 3.3V. I make no representations to their stability under those conditions. If you must run it outside the manufacturer specifications, I would suggest using the watchdog timer to reset it in the event that it hangs. A part being run outside of the manufacturer specified operating conditions should never be used for any sort of critical task or even one where a failure would be inconvenient to recover from (such as a device which was difficult to access for maintenance or replacement).

These parts do not support using an external crystal like the classic ATtiny parts do, however the internal oscillator is tightly calibrated enough that the internal clock will work for UART communication; Like the megaAVR 0-series, it includes clock corrections at 3V and 5V in the signature row, and megaTinyCore uses them for serial communication. These parts also support an external **clock** (that can come from an external oscillator - available from the usual suspects, the CLKOUT from another processor, and so on).

As usual for AVR parts, the internal oscillator's compliance covers a far wider range than the datasheet describes. In my testing on 1-series parts, it appeared that both the 16 MHz and 20 MHz oscillator can be calibrated to as low as approximately 5/8 of their nominal speed or as high as 1-5/8 of their nominal speed

### tinyAVR 2-series internal oscillator
The oscillator on the tinyAVR 2-series parts has gotten another upgrade an extra bit. The 128 settings cover the same range, just with twice the granularity (hey, if they keep it up by the next series they could have a full 256 cal settings! Imagine that! In fact, you don't have to imagine it - because it was like that nearly every AVR made until the 2016 revolution, when for some reason calibration bits were suddenly in shortage. At least the tinies are going the right direction - the Dx-series lost another bit, so the frequency compliance is minimal and granularity poor. They have autotune to turne internal oscillator from watch crystal, but it doesn't do much becasue it's limited by the granularity of the cal register...)

## Memory-mapped flash: No need to declare PROGMEM
Unlike classic AVRs, on the these parts, the flash is within the same address space as the main memory. This means `pgm_read_*_near()` is not needed to read directly from flash. Because of this, the compiler automatically puts any variable declared `const` into PROGMEM, and accesses it appropriately - you no longer need to explicitly declare them as PROGMEM. This includes quoted string literals, so the F() macro is no longer needed either (As of 2.1.0, F() once more explicitly declares things as living in PROGMEM (ie, it is slightly less efficient) in order to ensure compatibility with third party libraries).

However, do note that if you explicitly declare a variable PROGMEM, you must still use the pgm_read functions to read it, just like on classic AVRs - when a variable is declared PROGMEM on parts with memory mapped flash, the pointer is offset (address is relative to start of flash, not start of address space); this same offset is applied when using the `pgm_read_*_near()` macros. Do note that declaring things PROGMEM and accessing with `pgm_read_*_near` functions, although it works fine, is slower and wastes a small amount of flash (compared to simply declaring the variables const); the same goes for the F() macro with constant strings in 2.1.0 and later. On thwe other hand, this may be necessary for compatibility with some third party libraries (hence why we returned the F() macro to the old approach). A better approach may be forthcoming in future versions pending quantification of the benefits.

## Fast digital I/O
For timing-critical operations, we now support `digitalWriteFast()` and `digitalReadFast()`. The pin numbers passed to these functions **must** be a compile-time known constant; for best results, when using `digitalWriteFast()`, the pin value to be written should as well. Under those conditions, `digitalWriteFast()` will optimize down to a single instruction which executes in a single system clock cycle, and `digitalReadFast()` will optimize to 4 instructions (note that `if (VPORTx.IN&(1<n))` is still faster and smaller than `if (digitalReadFast(PIN_Pxn)==HIGH)` because testing a single bit within an I/O register and using that as the argument for a conditional branch can be optimized down to an SBIS/SBIC (**S**kip if **B**it in **I**/o register **S**et/**C**lear), while it requires several instructions to check that bit, and convert it to a boolean for further processing.

## Serial (UART) Support
All of the 0/1-series parts have a single hardware serial port (UART or USART); the 2-series parts have two. It works exactly like the one on official Arduino boards (except that there is no auto-reset, unless you've wired it up by fusing the UPDI pin as reset (requiring either HV-UPDI or the Optiboot bootloader to upload code), or set up an "ersatz reset pin" as described elsewhere in this document). See the pinout charts for the locations of the serial pins.

Prior to putting the part into a sleep mode, or otherwise disabling it's ability to transmit, be sure that it has finished sending the data in the buffer by calling `Serial.flush()`, otherwise the serial port will emit corrupted charachters.

### Pin Swapping
On all parts, the UART pins can be swapped to an alternate location. This is configured using the `Serial.swap()` or `Serial.pins()` methods. Both of them achieve the same thing, but differ in how you specify the set of pins to use. This must be called **before** calling `Serial.begin()`.

`Serial.swap(1) or Serial.swap(0)` will set the the mapping to the alternate (1) or default (0) pins. It will return true if this is a valid option, and false if it is not (you don't need to check this, but it may be useful during development). If an invalid option is specified, it will be set to the default one.

`Serial.pins(TX pin, RX pin)` - this will set the mapping to whichever mapping has the specified pins as TX and RX. If this is not a valid mapping option, it will return false and set the mapping to the default. This uses more flash than `Serial.swap()`; that method is preferred.

Bootloaders are available for both UART mappings; the UART bootloader option selected when you do "burn bootloader" for an Optiboot board definition is the serial port that the uploaded bootloaded will use.

**tiny 2-series warning** On the tinyAVR 2-series, there are two serial ports, `Serial` and `Serial1`. Each of can use one of two pin options (except on 14-pin parts, where the alternate pins for `Serial1` are not present). The *default* pins for `Serial1` are the same as the *alternate* pins for `Serial`.

In versions prior to 2.0.0, this was instead configured using the Tools -> UART Pins menu. If you're still using such ancient versions, please update.

### Baud Rates
To maximize the accuracy of the baud rate when using the internal oscillator, from the Tools -> Voltage for UART Baud menu, select whether the voltage is closer to 5v or 3v - the factory calibration supplies an oscillator error adjustment for the purpose of UART baud calculation for 5v and 3v, and using the right one will produce a baud rate closer to the target value. That said, testing has indicated that either setting is good enough unless you are talking to particularly finicky devices. On parts with 4k or less of flash, this setting is ignored because it adds approx 90 bytes to the size of the compiled binary, and on 2k and 4k parts, it's just not worth the space. This correction factor is never used by the bootloader.

When operating at 1MHz, as of 2.1.4 the UART can now reach 115200 baud (In version 2.1.4 through 2.2.x - Serial.flush() can hang when the baud rate is very high relative to the clock rate (ex, 115200baud and 1 MHz) due to a race condition and ther millis timer interrupt when TCA0 or TCD0 was used for millis (the interrupt for the TCBs is faster and didn't trigger it.

Maximum baud rate scales directly with the system clock speed; 2 mbaud is just barely possible at 16 MHz (this is the same maximum that was possible on classic AVRs); unlike classic AVRs, these have a "fractional baud rate generator": rather than specifying the bit period in units of 8 (U2X=1) or 16 (U2X=0) clock cycles as on classic AVRs, here it is specified in 64ths of 8 or 16 clock cycles (no, I'm not sure how they pulled that off. Seems to be established technology, but it sure looks like magic to me).  This is a boon at high baud rates - it guarantees that as long as the system is clocked high enough to generate the requested baud rate, worst case, the bit timing will be off by no more than 1/129th - less than 0.77%. At lower speeds, the error is far smaller, often negligible - for the speeds supported by megaTinyCore, the commonly used baud rates (9600, 19200, 57600 and 115200) are all within 0.1% except for 115200 @ 1 MHz (0.64%).  Of course, that is error in addition to any inaccuracy due to the internal oscillator being off-target; even if the best OSC20MCALA value is chosen by the factory cal (often but not always the case), on the 0/1-series, the increment size is 1.5%, so there's an a +/- 0.75% error - or more, because occasionally the factory calibration is 1 setting off from the ideal. On 2-series it's half that. Either way, you afe almsot guaranteed to be within 1%, with is more than enough for most serial communication. If you need it even closer (really?) you can select the voltage baud correction option from the tools menu; this will use the factory calibrated oscillator error valuesto nudge the baud rate even closer to the target.

Also as of 2.1.4, a series of methods were added for [printing numbers as hex.](./megaavr/extras/printHex.md). printHex is a development and debugging aid. Behavior may change without warning in a future release.

## SPI support
All of these parts have a single hardware SPI peripheral. It works exactly like the one on official Arduino boards using the SPI.h library. See the pinout charts for the location of these pins. Note that the 8-pin parts (412, 212, 402, 204) do not have a specific SS pin.

On all parts except the 14-pin parts, the SPI pins can be moved to an alternate location (note: On 8-pin parts, the SCK pin cannot be moved).

On 2.0.0 and later, this is configured using the SPI.swap() or SPI.pins() methods. Both of them achieve the same thing, but differ in how you specify the set of pins to use. This should be called **before** calling SPI.begin().

`SPI.swap(1) or SPI.swap(0)` will set the the mapping to the alternate (1) or default (0) pins. It will return true if this is a valid option, and false if it is not (you don't need to check this, but it may be useful during development). If an invalid option is specified, it will be set to the default one. On the tinyAVR

`SPI.pins(MOSI pin, MISO pin, SCK pin, SS pin);` - this will set the mapping to whichever mapping has the specified pins. If this is not a valid mapping option, it will return false and set the mapping to the default. This uses more flash than SPI.swap(); that method is preferred.

In versions prior to 2.0.0, this was instead configured using the Tools -> SPI Pins submenu; this is set at compile time (reburning bootloader is not required). In these versions, you could see whether the alternate pins are in use by checking if SPIREMAP is #defined - you can for example use it to check that the correct options are selected and terminate compilation so you can select the right option if that is the case.

This core disables the SS pin when running in SPI master mode. This means that the "SS" pin can be used for whatever purpose you want - unlike classic AVRs, where this could not be disabled. Earlier versions of this document incorrectly stated that this behavior was enabled in megaTinyCore; it never was, and SS was always disabled. It should be re-enabled and the SS pin configured appropriately (probably as INPUT_PULLUP) if master/slave functionality is required.

## I2C (TWI) support
All of these parts have a single hardware I2C (TWI) peripheral. It works *almost* exactly like the one on official Arduino boards using the Wire.h library, except for the additional features noted below. See the pinout charts for the location of these pins. **You must be sure to use external pullup resistors on the SDA and SCL lines if the devices you are connecting do not have those on board** (many Arduino/hobby targeted breakout boards do - typically 10k - though these may need to be enabled with a solder-bridge or jumper). The 30k-50k internal pullup resistors are not suitable for I2C pullup resistors; while they were enabled by megaTinyCore prior to version 2.1.0, this was the worst of both worlds: they often did work with the simple test case, leading the developer on their merry way thinking they were all set, only to discover that when they added another I2C device or two, or moved the device to the end of a longer cable, I2C suddenly no longer worked - it's probably better for it to fail immediately, prompting investigation and subsequent addition of appropriate pullup resistors. Note that there is nothing *preventing* one from enabling the internal pullups manually - you just should do so knowing that even if it happens to work, it's not a robust solution.

### TWI pins
On all 1-series parts with at least 14 pins, the TWI can be swapped to an alternate pair of pins. Neither 0-series nor 2-series parts have alternate pin locations for the TWI. On 2.0.0 and later, this is configured using the Wire.swap() or Wire.pins() methods. Both of them achieve the same thing, but differ in how you specify the set of pins to use. This should be called **before** Wire.begin(). This implementation of pin swapping is the same as what is used by[DxCore](https://github.com/SpenceKonde/DxCore) and [MegaCoreX](https://github.com/MCUdude/MegaCoreX).

`Wire.swap(1) or Wire.swap(0)` will set the the mapping to the alternate (1) or default (0) pins. It will return true if this is a valid option, and false if it is not (you don't need to check this, but it may be useful during development). If an invalid option is specified, it will be set to the default one.

`Wire.pins(SDA pin, SCL pin)` - this will set the mapping to whichever mapping has the specified pins as SDA and SCL. If this is not a valid mapping option, it will return false and set the mapping to the default. This uses more flash than Wire.swap(); that method is preferred.

In versions prior to 2.0.0, this was instead configured using the Tools -> I2C Pins submenu; this is set at compile time (reburning bootloader is not required). In these versions, you could see whether the alternate pins are in use by checking if TWIREMAP is #defined - you can, for example, use it to check whether correct options are selected and terminate compilation, so you can select the right option.

### General Call and Second Slave Addresses
When the version of Wire.h supplied with megaTinyCore 1.1.9 and later is used in slave mode, it is now possible to respond to the general call (0x00) address as well. This is controlled by the optional second argument to Wire.begin() (thanks https://github.com/LordJakson!) If the argument is supplied amd true, general call broadcasts will also trigger the interrupt. There is furthermore a third optional argument, which is passed unaltered to the `TWI0.SADDRMASK` register. If the low bit is 0, and bits set 1 will cause the I2C hardware to ignore that bit of the address (masked off bits will be treated as matching). If the low bit is 1, it will instead act as a second address that the device can respond to. See the datasheet for more information about this register.

### TWI buffer size
The Wire library requires a TX and RX buffer; these are both the same size, determined by how much SRAM the part has. Trying to use wire to send something too large for the buffer will result in the extra bytes just not being sent. Sure, it could have been worse,  The buffer sizes were revised in 2.3.0 to reduce the number of "oddball" parts - all tinyAVR parts supported by megaTinyCore get 32 byte buffers except the 2k parts, which literally cannot support 32b buffers. The 2k parts also don't have enough flash to do much with the Wire library - Wire is not a particularly space-efficient I2C implementation. A light-weight I2C implementation (which would have to sacrifice API compatibility) is probably necessary for meaningful use of I2C on 2k parts.

| Parts                  |   SRAM   | Buffers (2.3.0+) | Buffers (pre-2.3.0) | Notes:                                                              |
|------------------------|----------|------------------|---------------------|---------------------------------------------------------------------|
| 2k 0/1-series          |     128b |              16b |                 16b | 32b just doesn't fit Wire is basically unusable w/2k flash anyway.  |
| 4-8k 0/1,4k 2-series   | under 1k |              32b |                 16b | 32b buffer is really rough w/256b - but 16b breaks libraries        |
| other tinyAVR, AVR16DD |     1-4k |              32b |                 32b | Now all tinyAVR with >128b ram has 32b I2C TX and RX buffers        |
| AVR32DA/DB/DD parts    | Under 8k |             130b |                 64b | Official 4809 core does 128b, we add 2 more to allow for 128 byte-- |
| AVR128Dx and AVR64Dx   | 8 or 16k |             130b |                128b |  --writes to I2C memory/eeprom chips, which require 2 address bytes |

### *almost?*
There is one other rather sigmificant change, though it is transparent to almost everyone. The official Arduino megaavr core has TwoWire extending HardwareI2C, which extends Stream. We just extend Stream. HardwareI2C defines a considerable number of virtual member functions. This results in all of those methods being included in the final binary *regardless of whether they are called*. A master needs all the slave functions even if it never operates as a slave. A slave needs all the master functions, even if it never operates as a master. And the 4k parts can just forget about using Wire.h. Nobody has been able to express how this benefits the user, or why it was done other than that the 4809 is all that core was written to support and it has enough flash toget away with doing that, nor has anyone reported any problems caused by it.

## PWM support
The core provides hardware PWM via the standard `analogWrite()` function. On the 8-pin parts (412, 212, 402, 204), 4 PWM pins are available. On all other parts except 1-series parts with 20 or 24 pins, 6 PWM pins are available, all driven by Timer A. The 20 and 24 pin 1-series parts have two additional pins, driven by TCD0. The 2-series apparently traded TCD0 for a second serial port and a super-fancy ADC - those parts also all have 6 PWM pins. The Type B timers cannot be used for additional PWM pins - their output pins are the same as those available with Timer A and they are often too useful to justify using a whole TCB for  - however you can take them over if you need to generate PWM at different frequencies, though the fact that the prescaler cannot differ from the type A timer limits this use as well. See the pinout charts for a list of which pins support PWM.

**Note that TCA0 (the type A timer) on all parts is configured by the core at startup to operate in split mode, in order to support the most PWM pins possible with analogWrite(). As of the 2.2.x versions, a `takeOverTCA0()` function has been added, which can be called to instruct the core not write to TCA0-registers nor assume  any particular mode or behavior for TCA0. analogWrite() will not generate PWM except on pins driven by TCD0 on the 20/24-pin parts nor will digitalWrite turn it offIf you want to reconfigure TCA0 for other purposes, please refer to the below guide and "hard reset" the timer back to stock configuration.**
### [Taking over TCA0](megaavr/extras/TakingOverTCA0.md)

### PWM on TCD0 pins (PIN_PC0 amd PIN_PC1 - 10,11 on x16, 12,13 on x17)
The 3216, 1616, 816, 416, and the 3217, 1617 and 817 have two additional PWM pins driven by Timer D (PC0 and PC1 - pins 10 and 11 on x16, 12 and 13 on x17). Timer D is an async timer, and the outputs can't be enabled or disabled without briefly stopping the timer. This results in a brief glitch on the other PWM pin (if it is currently outputting PWM), and doing so requires slightly longer - though the duration of this glitch is under 1 us. If TCD is used as the millis timer - which is the default on any part that has a type D timer (in order to keep the timers that are more readily repurposed available - TCD0 is not an easy peripheral to work with), this will result in millis losing a very small amount of time (under 1 us) every time PWM is turned on or off on a TCD pin. Since

As of 2.2.0, analogWrite of 0 or 255 on a TCD-drivemn PWM pin does not disconnect the pin from the timer - instead it results in a constant HIGH or LOW output without disconnecting the timer(use digitalWrite() for that). This means that analogWrite(PIN_PC0 or PC1, 0) can be used to connect the timer to the pin without outputting PWM (yet) - doing this on both pins prior to setting any other duty cycles would allow one to ensure that no glitch of any sort occurs on the other TCD0 pin when the second pin is connected to it. Only digitalWrite() will disconnect the timer from the pin. When outputting a HIGH in this way, the pin is "inverted"; this means that digitalRead() on it will return 0, not 1 (if you're digitalRead'ing a pin, which you have set to output a constant HIGH, using analogWrite, and it's one of those two pins, it will read LOW - however, if you are using digitalRead() on a pin that you've set to output a constant value, you may be doing something wrong in general.

There is a benefit of having an async timer, however: megaTinyCore configures it to use the unprescaled internal oscillator as it's clock source. This means that while there is a difference in PWM frequency between 16-MHz derived and 20-MHz derived clocks, there is no change in frequency for different system clock speeds! The exception to this is when TCD0 is used as the millis/micros timing source at 1 MHz - running at full speed there resulted in spending an unreasonable fraction of runtime in the millis ISR.

On some versions of megaTinyCore prior to 2.2.0, PWM on the TCD0 pins was entirely broken.

**For general information on the available timers and how they are used PWM and other functions, consult the guide:**
This also covers the PWM frequencies that these timers will give you at various system clocks.
### [Timers and megaTinyCore](megaavr/extras/PWMandTimers.md)


## Read & Write EEPROM and USERROW
The standard EEPROM.h is available here, and as of 2.1.0, it works correctly again (and no longer tries to get clever with supporting the USERROW. Use the USERSIG.h library for that; it has the same API as EEPROM, though there may be future additions to harmonize with Dx-friendly functions for updating multiple bytes - the Dx-series parts can only erase the whole USERROW, so potentially each byte written could involve erasing and rewriting it all - the question of how to deal with that is why DxCore doesn't have a USERSIG library yet). The name "USERSIG" refers to the alternate name of the USERROW, the "User Signature" space - the name USERROW could not be used because it is defined by the io headers.

## NeoPixel (WS2812) support
The usual NeoPixel (WS2812) libraries often have problems on these parts. Like that library it supports RGB and RGBW LEDs, including all of the WS2812-compatible LEDs.. This core includes two libraries for this, both of which are tightly based on the Adafruit_NeoPixel library - one is identical except in name, while the other provides significant flash savings in exchange for minor restrictions and slightly different calling conventions. See the [tinyNeoPixel documentation](megaavr/extras/tinyNeoPixel.md) and included examples for more information.

## Tone Support
Support for tone() is provided on all parts using TCB0, unless TCB1 is present and TCB0 is set as millis source. This is like the standard tone() function. Unlike on some parts, it does not support use of the hardware output compare to generate tones. See caveats below if using TCB0 or TCB1 for millis/micros settings. On parts with 8l or less flash, tones which specify a duration longer than 65536ms and a high frequency (specifically where duration times frequency is >4.2 billion) are not supported - if specified at compiletime it will generate a compile error, and at runtime, it will produice a shorter duration tone than expected. On parts with 16k flash or more the limit is significantly higher and unlikely to be encountered.

## millis/micros Timekeeping Options
megaTinyCore provides the option to use any available timer on a part for the millis()/micros timekeeping, controlled by a Tools submenu - or it can be disabled entirely to save flash and allow use of all timer interrupts. By default, TCD0 will be used by on parts that have one - otherwise TCA0 will be used (in versions prior to 1.1.9, TCA0 was used by default on parts that could output PWM with TCD0 on pins not available for TCA0 PWM). All timers available on the parts can be used: TCA0, TCD0 (on parts that have it), TCB0, TCB1 (where present) and the RTC. Many of these - particularly the non-default options, involve tradeoffs. In brief, TCA0 is a very versatile timer that users often want to reconfigure, TCD0 loses a small amount of time when PWM is turned on or off on the two TCD0 PWM pins (10,11 on 20-pin parts, 12,13 on 24-pin parts), TCB0 conflicts with Servo and tone on parts that don't have TCB1, and when the RTC is used micros() is not available at all because the clock isn't fast enough. With these limitations in mind, the timer selection menu provides a way to move millis()/micro to the timer most appropriate for your needs.

For more information, on the hardware timers of the supported parts, and how they are used by megaTinyCore's built-in functionality, see the [Timers and megaTinyCore](megaavr/extras/PWMandTimers.md)

2.3.0 fixed a long-standing (though surprisingly low impact) "time travel" bug where the

### RTC timer for millis
If the RTC is selected as the timer for millis timekeeping, micros will not be available. Additionally, this timer is configured to run while in STANDBY sleep mode. This has two important consequences: First, it will keep time while in sleep. Secondly, every 64 seconds, the RTC overflow interrupt will fire, waking the chip - thus, if you are using the RTC for millis and putting the part into sleep, you should declare a volatile global variable that you set in the ISR that is supposed to wake the part, eg 'volatile boolean ShouldWakeUp=0;' - set it to 1 in the ISR, and when you put the ATtiny to sleep, have it check this immediately after waking, going back to sleep if it's not set, and clearing it if it is, e.g.:

```cpp
void GoToSleep() {
    do {
        sleep_cpu();
    } while (!ShouldWakeUp)
    ShouldWakeUp=0;
}
```

This functionality will be made easier to use via the megaTinySleep library in a future version of the core.

This board package also supports using an external 32.768khz crystal as the clock source for the RTC (not supported on 0-series or 8-pin parts). If this is used, make sure that the crystal is connected between the TOSC1 and TOSC2 pins (these are the same as the TX and RX pins with the default pin mapping, unfortunately), that nothing else is, that no excessively long wires or traces are connected to these pins, and that appropriate loading capacitors per crystal manufacturer datasheet are connected. Since the TOSC1 and TOSC2 pins are the same pins used for serial, you must use the alternate serial pins.

## ADC Support
These parts all have ADC channels available on most pins (11 pins on 24 and 20 pin parts, 9 on 14-pin parts and 5 on 8 pin parts - plus the one on the UPDI pin which is not normally usable), they can be read with analogRead() like on a normal AVR. While the An constants (ex, A0) are supported, and refer to the corresponding ADC channel (not the corresponding pin number), using these is deprecated - the recommended practice is to pass the digital pin number to analogRead(). Analog reference voltage can be selected as usual using analogReference(). Supported reference voltages are:
* VDD (Vcc/supply voltage - default)
* INTERNAL0V55
* INTERNAL1V1
* INTERNAL1V5
* INTERNAL2V5
* INTERNAL4V3 (alias of INTERNAL4V34)
* INTERNAL4V34
* EXTERNAL (2-series and 1614, 1616, 1617, 3216, 3217 only)

In addition to reading from pins, you can read from the following sources (pass to analogRead() in place of a pin number)
* ADC_INTREF (The internal reference - you can set it manually via VREF.CTRLA, or by calling analogReference(), followed by analogReference(VDD) - The internal reference register is not changed when switching back to VDD as reference)
* ADC_DAC0 (The value being output by DAC0, 1-series parts only)
* ADC_TEMPERATUIRE (The internal temperature sensor)

On the tinyAVR 2-series, analogRead() really does not do the new ADC justice. To truly take advantage of it, a library will be written in due time.

### ADC Sampling Speed
From 2.0.0 onwards, megaTinyCore takes advantage of the improvements in the ADC on the newer AVR parts to improve the speed of analogRead() by more than a factor of three!the ADC clock which was - on the classic AVRs - constrained to the range 50-200kHz - can be cranked up as high as 1.5MHz at full resolution! As of 2.0.0, we now use 1MHz at 16/8/4 MHz system clock, and 1.25 MHz at 20/10/5 MHz system clock. To compensate for the faster ADC clock, we set ADC0.SAMPCTRL to 14 to extend the sampling period from the default of 2 ADC clock cycles to 16 - providing the same sampling period as in previous versions, which should preserve the same accuracy when measuring high impedance signals. If you are measuring a lower impedance signal and need even faster analogRead() performance - or if you are measuring a high-impedance signal and need a longer sampling time, you can adjust that setting:

```
ADC0.SAMPCTRL=31; // maximum sampling length = 31+2 = 33 ADC clock cycles
ADC0.SAMPCTRL=0; //minimum sampling length = 0+2 = 2 ADC clock cycles
```

With the minimum sampling length, analogRead() speed would be approximately doubled from it's already-faster value.

### analogReadResolution
`analogReadResolution()` is supported from 2.0.2 onwards - valid options are 8 and 10 (this is the number of bits in the result). Like the pin-swapping functions for Serial, SPI, and I2C. Prior to 2.2.0, this returned a boolean, `true` if the argument was valid, and `false` if the argument was not valid, and when called with an invalid value, the resolution of analogRead() would be set back to the default of 10 bits. In 2.2.0, only 8 or 10 is accepted, and the value passed to it must be a constant known at compile-time - otherwise it will be a compile error (if you need to set analogReadResolution based on conditions known only at runtime, just do `if(condition) {analogReadResolution(8);} else {analogReadResolution(10);}` - the old behavior was perverse and a bad idea. The few people who used this never checked the return value, leading to unexpected behavior when porting, invalid values changed settings in a way that may not be desired and is harder to debug. An invalid value passed to this is almost certainly the result of a bug in the code; we now report it as such.

`analogReadResolution()` behavior on the 2-series parts is not yet decided - they support 8 or 12-bit conversions; owing to the historical importance of 10-bit conversions, we may provide a mechanism to tell analogRead() to take 12-bit samples, but right-shift the result by 2 places (turning it into 10 bits) for backwards compatibility.

`analogReadResolution()` cannot be called with higher resolution options than the hardware supports directly. See below, though.

### A second ADC?
On some parts, yes! The "good" 1-series parts (1614, 1616, 1617, 3216, and 3217) have a second ADC, ADC1. On the 20 and 24-pin parts, these could be used to provide analogRead() on additional pins (it can also read from DAC2). Currently, there are no plans to implement this in the core due to the large number of currently available pins. Instead, it is recommended that users who wish to "take over" an ADC to use it's more advanced functionality choose ADC1 for this purpose. See [ADC free-run and more](megaavr/extras/ADCFreerunAndMore.md) for some examples of what one might do to take advantage of it.  As of 2.0.5, megaTinyCore provides a function `init_ADC1()` which initializes ADC1 in the same way that ADC0 is (with correct prescaler for clock speed and VDD reference).

## DAC Support
The 1-series parts have an 8-bit DAC which can generate a real analog voltage (note that this provides very low current and can only be used as a voltage reference, it cannot be used to power other devices). This generates voltages between 0 and the selected VREF (which cannot be VDD, unfortunately). In 2.0.0 and later, set the DAC reference voltage via the DACReference() function - pass it one of the INTERNAL reference options listed under the ADC section above. In versions prior to 2.0.0, select the DAC VREF voltage from the Tools -> DAC Voltage Reference submenu. This voltage must be lower than Vcc to get the correct voltages. Call analogWrite() on the DAC pin to set the voltage to be output by the DAC. To turn off the DAC output, call digitalWrite() on that pin.

## Servo Support
On versions prior to 2.2.0, the Servo libraries were, to put it mildly, completely broken. See: https://github.com/SpenceKonde/megaTinyCore/issues/195 for more information. The Servo libraries should not be used in effected versions.

This core provides a version of the Servo library which will select an appropriate timer (TCB0, except on the 2-series parts and the 3216, 3217, 1617, 1616 and 1614, where there is a TCB1 available; where it is, TCB1 will be used instead). Except on parts with a TCB1, tone cannot be used at the same time as the Servo library. If millis/micros is set to use TCB1 on parts that have it, servo will use TCB0 instead, making it incompatible with tone there as well. Servo output is better the higher the clock speed - when using servos, it is recommended to run at the highest frequency permitted by the operating voltage to minimize jitter.

**Warning** If you have installed a version of the Servo library to your <sketchbook>/libraries folder (including via library manager), the IDE will use that version of the library (which is almost certainly not compatible with these parts) instead of the one supplied with megaTinyCore (which is). As a workaround, a duplicate of the Servo library is included with a different name - to use it, `#include <Servo_megaTinyCore.h>` instead of `#include <Servo.h>` - no other code changes are necessary.

## printf() support for "printable" class
Unlike the official board packages, but like many third party board packages, megaTinyCore includes the .printf() method for the printable class (used for Serial and many other libraries that have print() methods); this works like printf(), except that it outputs to the device in question; for example:
```cpp
Serial.printf("Milliseconds since start: %ld\n", millis());
```
Note that using this method will pull in just as much bloat as sprintf(), so it may be unsuitable on devices with small flash memory.

## Pin Interrupts
All pins can be used with attachInterrupt() and detachInterrupt(), on RISING, FALLING, CHANGE, or LOW. All pins can wake the chip from sleep on CHANGE or LOW. Pins marked as ASync Interrupt pins on the megaTinyCore pinout charts (pins 2 and 6 within each port can be used to wake from sleep on RISING and FALLING edge as well. Those pins are termed "fully asynchronous pins" in the datasheet

Advanced users can instead set up interrupts manually, ignoring attachInterrupt and manipulating the relevant port registers appropriately and defining the ISR with the ISR() macro - this will produce smaller code (using less flash and ram) and the ISRs will run faster as they don't have to check whether an interrupt is enabled for every pin on the port. For full information and example, see: [Pin Interrupts](megaavr/extras/PinInterrupts.md)

## EESAVE configuration option
The EESAVE fuse can be controlled via the Tools -> Save EEPROM menu. If this is set to "EEPROM retained", when the board is erased during programming, the EEPROM will not be erased. If this is set to "EEPROM not retained", uploading a new sketch will clear out the EEPROM memory. You must do Burn Bootloader to apply this setting. Prior to 2.1.0, there were problems with the EEPROM library as well which could mimic that behavior.

## SerialEvent support option
Having learned that it is deprecated in official cores, I feel no obligation to support this, it is dropped as of 2.3.0, amd we get rid of a submenu.

## Improved Export Compiled Binary
The Sketch -> Export Compiled Binary option has been significantly enhanced in megaTinyCore:

### Naming includes all Tools submenu options that effect output
The name of the exported binary includes every tools submenu selection that could impact the compiled binary, for example `Test.ino.t1614opti.20.u5V.mD0.hex` - the full part number (not just family), whether it's using optiboot (this effects start address of .hex, unlike classic AVRs where a bootloadeed board takes the same binary as non-bootloadeed one),, the clock speed (in MHz), the approximate voltage for UART baud rate calculation if used for that part, and the millis timer source (`A0`, `B0`, `B1`, `D0`, `RTC`, `RTC_XTAL` or `NONE`). Entries will be added to this as new submenus are added to the core

### Assembly Listing generation
Like ATTinyCore and DxCore, Sketch -> Export compiled binary also generates an assembly listing in the sketch folder (same naming scheme as above); this file, ending in .lst, can be opened in any text editor. It contains an assembly listing, with the C source and generated assembly intermixed, which (attempts to) show which C generated which instructions. One does not need to know assembly to understand that a whole load of cryptic instructions means more flash usage than a small number of cryptic instructions; this very simple rule helps with the most common use case for the listing: figuring out what exactly it is in the code that is responsible for it eating up all the flash. See: [Assembly Listing Basics](https://github.com/SpenceKonde/AVR-Guidance/blob/master/LowLevel/AssemblyListingBasics.md) for a very brief orientation.

### Memory Map generation (New in 2.3.0)
The memory map is a far less powerful resource than an assembly listing - but it is far easier to understand, and it is very much focused on the sizes of sections in memory (both RAM and Flash). It's a text file with a .map extension, containing a table of values, with columns delimited by the | character, and all numbers in hexadecimal (which is the only right way for memory addresses). Each symbol name is listed, along with it's address, and - if it occupies space in flash or ram, that size, as well as where they are defined. It may be best viewed in a spreadsheet, but I haven't been able to make it import into excel cleanly; a future update may add some pre-processing to help with this.

## Startup Time option
As of 2.1.0, there's now an option to set the time between reset (from any cause) and the start of code execution (previously it was always 8ms) - options are 1ms, 8ms (default), and 64ms. As of 2.2.0, all options supported by the hardware are available. Use 8ms unless you have a reason not to - that default option is generally fine... but occasionally (for example, with particularly slow rising power supplies and no BOD), the slower time may be needed.

## BOD configuration options
These parts officially support BOD trigger levels of 1.8V, 2.6V, and 4.2V, with Disabled, Active, and Sampled operation options for when the chip is in Active and Sleep modes - Disabled uses no extra power, Active uses the most, and Sampled is in the middle. As of 2.1.0, the active/sleep modes have been combined into a single menu, the nonsensical options (such as using more aggressive BOD is while sleeping than while awake) removed, and the previously unexposed options added: sampled mode is now available with two sample rates (the faster one uses ever so slightly more power, as you would expect), and "Enabled hold wake" - in that mode, BOD is disabled in sleep, enabled when not sleeping, and when waking up, code execution does not begin until the BOD is ready. See the datasheet for details on power consumption and the meaning of these options. You must do Burn Bootloader to apply this setting, even in 2.2.0 and later; it's not a "safe" setting, since if you set it to a voltage higher that the operating voltage and enabke it, the chip is "bricked" until a high enough voltage is applied.

### Additional BOD levels
Between the initial header file and preliminary datasheet release, and the more recent versions of each, several BOD settings were removed from the tinyAVR 0/1-series datasheets, and the atpack release notes described them as "unqualified" - (described as "unqualified" in the release notes accpompanying this change - I understand that this has something to do with the vetting processfor critical applications which these parts go through and do not correspond to a voltage above which the chip is guaranteed to function under certain conditions (The three official BOD levels are the voltages that the chip is guaranteed (Yup, they use that word in the datasheet!) to work within the manufacturer specified temperature range running at a system clock frequency no higher than marked, and it s indicated that this is tested in production. In contrast, the other 5 BOD levels are expected to work as one woulod expect, but Microchip does not proviode any *guarantee*, even if all other design requirements are met.nd are not guaranteed to work, and could be removed from a future silicon revision without notice (though the last possibility seems very unlikely)) were removed from the datasheet and header files. These are supported by the megaTinyCore BOD  dropdown menu, but (as of 2.0.4 - the first version that has the new headers) are marked as such in the submenu. Note that the new headers no longer provide the `*_gc` enum entries for these BOD levels..

## Auto-set safe fuses on UPDI upload - New in 2.2.0!
Whenever a UPDI programmer is used to upload code, all fuses that can be set "safely" (as in, without risk of bricking the board, or bricking the board if one does not have access to an HV programmer), and which have any built-in configuration options, will be set. Thus, except where noted, behavior will always match the selected tools menu! This means:
```
WDTCFG will not be changed - not configured by megaTinyCore
BODCFG will not be changed - not safe
OSCCFG will be set
TCD0CFG will not be changed - not configured by megaTinyCore
SYSCFG0 will not be changed - not safe
SYSCFG1 will be set
APPEND will  not be changed - not configured by megaTinyCore. There is insufficient demand to justify the development effort.
BOOTEND will be set
LOCKBIT will not be changed - not configured by megaTinyCore
```
`BODCFG` is not safe, because setting this to a higher voltage than board is running at and enabling it will "brick" the board until a higher operating voltage can be supplied. `SYSCFG0` is not safe because this is where `RSTPINCFG` lives; changing this can leave the board unprogrammable except via HV UPDI programming, and not everyone has an HV UPDI programmer. In the future if/when a programmer that guarantees HV UPDI capability which can be selected as a programmer (ie, it becomes possible to make a tools -> programmer option which will only work with HV programmers) this fuse will be set automatically when using that programmer.

As a result **in 2.2.0 and later, you no longer need to 'burn bootloader' to switch between 16-derived and 20-derived speeds!**


## Link-time Optimization (LTO) support
This core *always* uses Link Time Optimization to reduce flash usage - all versions of the compiler which support the tinyAVR 0/1/2-series parts also support LTO, so there is no need to make it optional as was done with ATTinyCore.

# How to refer to pins
The simple matter of how to refer to a pin for analogRead() and digitalRead(), particularly on non-standard hardware, has been a persistent source of confusion among Arduino users. It's my opinion that much of the blame rests with the decisions made by the Arduino team (and author of Wiring before them) regarding how pins were to be referred to; the designation of some pins as "analog pins" leads people to think that those pins cannot be used for digital operations (they are better thought of as "pins with analog input" - like how there are "pins that can output PWM"). The fact that pins have traditionally been renumbered has further muddied the water. For non-standard classic AVR parts, matters are often made even worse by multiple, incompatible "pin mappings" created by various authors over the years to make the part act "more like an Uno" or for some other purpose (ATTinyCore is a particular mess in this way, with some parts having three entirely different pin mappings).

This core uses a simple scheme for assigning the Arduino pin numbers: Pins are numbered starting from the the I/O pin closest to Vcc as pin 0 and proceeding counterclockwise, skipping the (mostly) non-usable UPDI pin. The UPDI pin is then assigned to the last pin number - as noted above, it is possible to read the voltage on the UPDI pin - we recommend this only as a last resort. On unofficial parts like these, we recommend that pins be referred to by the PIN_Pxn constants - this will maximize portability of your code and make it easier to look up information on the pins you are using in the relevant datasheets should that be necessary.

## PIN_Pxn Port Pin Numbers (recommended)
**This is the recommended way to refer to pins** Defines are also provided of form PIN_Pxn, where x is A, B, or C, and n is a number 0-7 - (Not to be confused with the PIN_An defines described below). These just resolve to the digital pin number of the pin in question - they don't go through a different code path or anything. However, they have particular utility in writing code that works across the product line with peripherals that are linked to certain pins (by Port), as most peripherals are. Several pieces of demo code in the documentation take advantage of this.  Direct port manipulation is possible as well - and in fact several powerful additional options are available for it - see [direct port manipulation](megaavr/extras/DirectPortManipulation.md).

**`PIN_Pxn` - not `Pxn`, and not `PIN_xn` - those mean different things!**

## Arduino Pin Numbers
When a single number is used to refer to a pin - in the documentation, or in your code - it is always the "Arduino pin number". These are the pin numbers shown in orange (for pins capable of analogRead()) and blue (for pins that are not) on the pinout charts. All of the other ways of referring to pins are #defined to the corresponding Arduino pin number.

## An and PIN_An constants
The core also provides An and PIN_An constants (where n is a number from 0 to 11). These refer to the ADC0 *channel* numbers. This naming system is similar to what was used on many classic AVR cores **but here, they are just #defined as the corresponding Arduino pin number**. If you need to get the analog channel number on a digital pin, use the `digitalPinToAnalogInput(pin)` macro. The An numbers are not shown on the pinout charts - just use the digital pin numbers. The mapping of analog channels to pins is shown in the the datasheet under the I/O Multiplexing Considerations chapter, and reproduced in the [advanced ADC documentation page](megaavr/extras/ADCFreerunAndMore.md). Note that channel A0 is on the UPDI/Reset pin - however, even when configured as UPDI, it can be used as an input as long as the signals it can be exposed to do not look like the UPDI enable sequence.

# Triggering a reset from software
As noted above in the discussion of the shared UPDI/reset pin, these parts support a native software reset operation; on classic AVRs, the only way was to enable the watchdog timer, and then wait for it to time out. With the "modern" AVR devices, there are two ways to trigger a reset from software: A watchdog timer reset, and the native software reset. Unlike classic AVRs, after a WDT reset, the watchdog timer is not forced on. These two methods of resetting the chip allow you to signal to the application what sort of condition triggered the reset. Additionally, while the bootloader, if used (see below) will run after a software reset, it will NOT run after a watchdog reset. This allows the application a convenient mechanism to restart itself without having to potentially wait through the bootloader attempting to communicate with whatever is connected to the serial port. The method shown below also resets the part significantly faster utilizing the "window" WDT functionality - that is, by telling the chip to reset if it receives a WDR instruction too soon - and then repeatedly sending that instruction until it resets.

```
void resetViaWDT() {
    _PROTECTED_WRITE(WDT.CTRLA,0x01); //enable the WDT, minimum timeout
    while (1) ; // spin until reset
}

void resetViaSWR() {
    _PROTECTED_WRITE(RSTCTRL.SWRR,1);
}

```

# Identifying Part and Options in sketch
It is often useful to identify what options are selected on the menus from within the sketch; this is particularly useful for verifying that you have selected the options you wrote the sketch for when opened later by yourself or someone who you shared it with. Or, you can use this, combined with `#if` macros to select the appropriate code depending on the part or options at hand.

## Part or Part Family
When writing code that may be compiled for a variety of target chips (which is particularly easy on the megaAVR parts, as the peripherals are so similar), it is often useful to be able to identify which family of parts is being used, in order to, for example, select which pin numbers to use for each purpose. Each part is identified by a #define:
`__AVR_ATtiny***__` where *** is the part number (ex: `__AVR_ATtiny3216__`).

This core provides two additional #defines for part "families" - note that in versions prior to 2.0.2, these are incorrect for many parts:
* `__AVR_ATtinyx**__` where ** is the last 2 digits of the part number (ex: `__AVR_ATtinyx16__`)
* `__AVR_ATtinyxy*__` where * is the last digit of the part number, (ex: `__AVR_ATtinyxy6__`).

This is just shorthand, for convenience - `#ifdef __AVR_ATtinyxy2__` is equivalent to `#if defined(__AVR_ATtiny212__) || defined(__AVR_ATtiny412__) || defined(__AVR_ATtiny202__) || defined(__AVR_ATtiny402__)`

Additionally, a few other useful #defines are provided for convenience:
* `MEGATINYCORE` will be defined whenever this core is in use, and should contain a string representation of the version.
* `MEGATINYCORE_MCU` will be defined as the numeric part of the part number.
* `MEGATINYCORE_SERIES` will be defined as 0, 1 or 2 for 0, 1, or 2-series parts (2.0.2 and later for 0 and 1-series - 2-series is detected as of 2.1.0, but will throw an error as the support for these parts is not yet implemented)

## Core Version
Version information for MEGATINYCORE is also provided by a few additional defines to make it easier to work with these in a sketch or compare to specific values; an example is presented below:
* MEGATINYCORE "2.0.2"
* MEGATINYCORE_MAJOR 2
* MEGATINYCORE_MINOR 0
* MEGATINYCORE_PATCH 2
* MEGATINYCORE_RELEASED 1
* MEGATINYCORE_NUM 0x02000201
While historically, these have often not been consistently updated with every release, as of 2.1.4, they are now generated directly from the platform.txt - it is hoped that this will help improve that.

## Millis Timer Selection
The option used for the millis/micros timekeeping is given by a define of the form USE_MILLIS_TIMERxx. Possible options are:
* USE_MILLIS_TIMERA0
* USE_MILLIS_TIMERB0
* USE_MILLIS_TIMERB1
* USE_MILLIS_TIMERD0
* USE_MILLIS_TIMERRTC (defined if RTC is used for millis - whether or not external crystal is used)
* USE_MILLIS_TIMERRTC_XTAL (only defined if RTC is used with external crystal)
* USE_MILLIS_TIMERNONE (defined if millis timekeeping is disabled)
* DISABLE_MILLIS (synonym for above, for backwards compatibility)
Additionally, `MILLIS_TIMER` is defined to that timer - see `Identifying Timers` below

### Checking that correct menu option is selected
If your sketch requires that the TCB0 is used as the millis timer

```
#ifndef MILLIS_USE_TIMERB0
#error "This sketch is written for use with TCB0 as the millis timing source"
#endif
```

### Core feature detection
There are a number of macros for determining what (if any) features the core supports (these are shared with megaTinyCore)
* `CORE_HAS_FASTIO` (1) - If defined as 1 or higher. indicates that digitalWriteFast() and digitalReadFast() is available. If undefined or defined as 0, these are not available. If other "fast" versions are implemented, they would be signified by this being defined as a higher number. If defined as -1, there are no digital____Fast() functions, but with constant pins, these functions are optimized aggressively for speed and flash usage (though not all the way down to 1 instruction).
* `CORE_HAS_OPENDRAIN ` (1) - If defined as 1, indicates that openDrain() and (assuming `CORE_HAS_FASTIO` is >= 1) openDrainFast() are available. If undefined or 0, these are not available.
* `CORE_HAS_PINCONFIG ` (0) - If defined as Indicates that pinConfig() is available. If not defined or defined as 0, it is not available.
* `CORE_HAS_TIMER_TAKEOVER` (1) - if defined as 1, takeOverTCxn() functions are available to let user code take full control of TCA0, TCA1 and/or TCD0.
* `CORE_HAS_TIMER_RESUME` (1)- if defined as 1, the corresponding resumeTCxn() functions, which reinitialize them and return them to their normal core-integrated functions, are available.
* `ADC_NATIVE_RESOLUTION` (12)- This is the maximum resolution, in bits, of the ADC without using oversampling.
* `ADC_NATIVE_RESOLUTION_LOW` (10) - The ADC has a resolution setting that chooses between ADC_NATIVE_RESOLUTION, and a lower resolution.
* `DIFFERENTIAL_ADC` (1) - This is defined as 1 if the part has a basic differential ADC (no gain, and V<sub>analog_in</sub> constrainted to between Gnd and V<sub>Ref</sub>), and 2 if it has a full-featured one. It does not indicate whether said differential capability is exposed by the core.
* `SUPPORT_LONG_TONES` (1)  - On some modern AVR cores, an intermediate value in the tone duration calculation can overflow (which is timed by counting times the pin is flipped) leading to a maximum duration of 4.294 million millisecond. This is worst at high frequencies, and can manifest at durations as short as 65 seconds worst case. Working around this, however, costs some flash, and some cores may make the choice to not address it (megaTinyCore only supports long tones on parts with more than 8k of flash).  If `SUPPORT_LONG_TONES` is defined as 1, as long as (duration * frequency)/500 < 4.294 billion, the duration will not be truncated. If it is defined as 0, the bug was known to the core maintainer and they chose not to fully correct it (eg, to save flash) but took the obvious step to reduce the impact, it will be truncated if (duration * frequency) exceeds 4.294 billion. If `SUPPORT_LONG_TONES` is not defined at all, the bug may be present in its original form, in which case the duration will be truncated if (duration * frequency) exceeds 2.14 billion.
* `CORE_HAS_ANALOG_ENH` (0)  - If defined as 1, analogReadEnh() (enhanced analogRead) is available. Otherwise, it is not.
* `CORE_HAS_ANALOG_DIFF` (0)  - If defined as 1, analogReadDiff() (differential enhanced analogRead) is available. Otherwise, it is not.  It has same features as enhanced, except that it takes a differential measurement.
* `MAX_OVERSAMPLED_RESOLUTION` - If either `CORE_HAS_ANALOG_ENH` or `CORE_HAS_ANALOG_DIFF` is 1, this will be defined as the maximum resolution obtainable automatically via oversampling and decimation using those functions. This will be 15 on the Dx-series parts in the near future when these functions are added.
* `ADC_MAXIMUM_GAIN` - Some parts have an amplifier, often used for differential readings. The Dx-series are not among them. If this is defined as a positive number, it is the maximum gain available. If this is defined as -1, there are one or more `OPAMP` peripherals available which could be directed towards the same purpose, though more deliberation would be needed. If it is defined as -128 (which may come out as 128 if converted to an unsigned integer), there is a gain stage on the differential ADC, but it is a classic AVR, so the available gain options depend on which pins are being measured, and there is a different procedure as detailed in the core documentation (ex, ATTinyCore 2.0.0 and later). If it is 0 or undefined, there is no built-in analog gain state for the ADC, or it is not exposed through the core.

### Identifying Timers
Each timer has a number associated with it, as shown below. This may be used by preprocessor macros (`#if` et. al.) or `if()` statenebts to check what `MILLIS_TIMER` is, or to identify which timer (if any) is associated with a pin using the `digitalPinToTimer(pin)` macro.

```
#define NOT_ON_TIMER 0x00
#define TIMERA0 0x10
#define TIMERA1 0x11 // Not present on any tinyAVR 0/1-series
#define TIMERB0 0x20
#define TIMERB1 0x21
#define TIMERB2 0x22 // Not present on any tinyAVR
#define TIMERB3 0x23 // Not present on any tinyAVR
#define TIMERB4 0x23 // Not present on any tinyAVR
#define TIMERB5 0x23 // Not present on any tinyAVR
#define TIMERD0 0x40
#define DACOUT 0x80  // Not a timer - but used like one by analogWrite()
#define TIMERRTC 0x90
#define TIMERRTC_XTAL 0x91
```

# Bootloader (optiboot) Support
A new version of Optiboot (Optiboot-x) now runs on the tinyAVR 0-series and 1-series chips.  It's under 512 bytes, and works on all parts supported by megaTinyCore, allowing for a convenient workflow with the same serial connections used for both uploading code and debugging (like a normal Arduino Pro Mini).

To use the serial bootloader, select a board definition with (optiboot) after it (note - this might be cut off due to the width of the menu; the second set of board definitions are the optiboot ones).

If the chip you will be programming has not been bootloaded, connect your UPDI programmer, and the desired options for clock rate (20/10/5MHz vs 16/8/4/1MHz is all that matters here. The fuses set the base clock to 20MHz or 16MHz, but the prescaler is set at startup by the sketch - so if you "burn bootloader" with 20MHz selected, but upload sketch compiled for 10MHz, that's fine and will work), Brown Out Detect (BOD), Serial port pins for the bootloader, and whether to leave the UPDI pin configured as such, or reconfigure it as a reset pin, and select Tools -> "Burn Bootloader"
**WARNING: After doing "Burn Bootloader", if you set the UPDI pin to act as reset or IO, the chip cannot be reprogrammed except via the serial bootloader or using an HV UPDI programmer - it is strongly suggested to first burn the bootloader with the UPDI/Reset pin left as UPDI, verify that sketches can be uploaded, and only then "Burn Bootloader" with the UPDI/Reset pin set to act as Reset**

After this, connect a serial adapter to the serial pins (as well as ground and Vcc). On the tinyAVR 0/1-series breakout boards which I sell on Tindie, a standard 6-pin "FTDI" header is provided for this that can be connected directly to many serial adapters.


If the UPDI/Reset pin option was set to UPDI or IO when bootloading, you must unplug/replug the board (or use a software reset - see note near start of readme; you can make an ersatz reset button this way) to enter the bootloader - after this the bootloader will be active for 8 seconds.

If the UPDI/Reset pin option was set to reset, you must reset the chip via the reset pin (or software reset to enter the bootloader, and the bootloader will run for 1 second before jumping to the application). This is the same as how optiboot works on classic AVR boards. The standard DTR-autoreset circuit is recommended (this is how boards I sell on Tindie with Optiboot preloaded are configured).

Serial uploads are all done at 115200 baud, regardless of port, pin mapping or part.

## "Upload using programmer" and removing Optiboot

### Removing Optiboot - 2.2.0 and later
On 2.2.0 and later, whenever code is uploaded via UPDI ("upload" from non-optiboot board definition, or "upload using programmer" from optiboot one), whether or not the device has been bootloaded (like on classic AVRs, "upload using programmer" will trash any bootloader present), `BOOTEND` will be set to match what the code was compiled for; with an optiboot board definition selected, this will result in the first 512b of flash being blank (since we erased any bootloader present), `BOOTEND=2` (512b bootloader section, as application expects), and a working sketch. With a non-optiboot board definition selected, this will result in `BOOTEND=0`, no gap at the start of the sketch, and a working sketch.

### Removing Optiboot - 2.1.5 and earlier
Like classic AVRs with hardware bootloader support (like the ATmega328p, and all other ATmega parts older than the 4809/4808), and unlike ATtiny parts without that, which use "virtual boot" to get bootloader functionality, you must do "burn bootloader" with a non-optiboot part selected in order to reprogram the part normally via UPDI.

### Background, 2.1.5 and earlier
Unlike classic AVRs with hardware bootloader support, where not doing "burn bootloader" before switching to uploading via a programmer would work until sketch size reached the very end of the flash (where the chip thinks the bootloader is), on these parts the sketch would fail in strange and bizarre ways if code is compiled with incorrect assumption about the size or presence of a bootloader (`BOOTEND` fuse) - it could be challenging to identify what the cause of the problem was when this happened - you would typically see pieces of behavior that you recognize, but not in an order that made sense. The reason for this is that the `BOOTEND` fuse sets the location of the interrupt vectors. If the application is started at 0x200, as it would for code compiled for use with Optiboot, but `BOOTEND` is set to 0, the hardware would, when an interrupt fired, jump to the blank section of flash before the application, execution would run along the blank flash until it hit the actual sketch... but it would still think it was in an interrupt, whereas in the inverse scenario, the hardware would jump to the middle of the application code instead of the vector table. Both scenarios are (speaking from experience) incredibly confusing when encountered.

In an effort to prevent these scenarios, starting in 2.2.0, whenever the part is programmed via UPDI (ie, any time except when using the bootloader to upload code), all fuses that are "safe" to set (see above) will be set, including `BOOTEND`.

## Autoreset circuit
If using the bootloader with with the UPDI pin set to reset, this is needed for uploading. It will reset the chip when serial connection is opened like on typical Arduino boards. Do not connect an autoreset circuit to the UPDI pin if it is not set to act as reset, as this will not reset the chip, and will just block UPDI programming.
* 1 Small signal diode (specifics aren't important, as long as it's approximately a standard diode)
* 1 0.1uF Ceramic Capacitor
* 1 10k Resistor
* A tinyAVR 0/1-series part with the bootloader installed on it (you can't do UPDI programming once the autoreset circuit is connected the the UPDI/Reset pin, even if that pin hasn't yet been set to act as reset, as the autoreset circuit will interfere with UPDI programming).

Connect the DTR pin of the serial adapter to one side of a 0.1uF ceramic capacitor.
Connect the other side of the capacitor to the Reset (formerly UPDI) pin.
Connect the 10k resistor between Reset and Vcc.
Connect the diode between Reset and Vcc with the band towards Vcc.

The breakout boards I sell on Tindie have a pair of pads on the back that can be bridged with solder to reversibly connect the on-board autoreset circuit to the UPDI/Reset pin. If you buy a breakout board from me which has the autoreset circuit, but which is not bootloaded, these pads will not have been connected - use a UPDI programmer to put the bootloader on it with your desired settings, then bridge the pads on the back to connect the auto-reset circuit. If you buy a breakout board from me with Optiboot preloaded, it will come set for 20/10/5MHz, BOD set to "sampled" mode with a threshold of 3.7v (for 5v boards) or 2.6v (for 3.3v boards) or disabled entirely (for no-regulator boardds), and if you ordered a board with the autoreset circuit, the UPDI pin will be set to work as reset (if you need different settings, please mention this when ordering - boards with optiboot preloaded are bootloaded only when the order is packed, so I can use whatever settings you specify).

Note that, if you have UPDI programming enabled, and desire the convenience of autoresetting to enter serial bootloader to upload sketches, or resetting the chip when entering the serial monitor, you can use the technique described near the top of the readme (under the section about there not being a dedicated reset pin) to make an ersatz reset pin that does a software reset when held low - this will enter the bootloader, so if your sketches set up an ersatz reset pin, you can use that to get into the bootloader (this isn't foolproof, because if your sketch chokes before it properly sets up the interrupt, or you choked and forgot to include the ersatz reset code in the last sketch you uploaded - but in this case you can enter bootloader by power cycling as described above). The autoreset circuit works the same way - just connect the parts as described above, only using your ersatz reset pin instead of the UPDI/Reset pin.

### A few caveats:
* The bootloader is at the beginning of memory, rather than at the end (where it was on older chips). Thus, the start of the application code must be 512b after the start of the memory - this is handled by the core, but you cannot upload a .hex file compiled with a non-optiboot board definition to an optiboot board definition and vice versa.
* **If you have set the UPDI/Reset pin to act as a reset pin, you can no longer program the part via UPDI** without using an HV programmer to reset the pin to act as UPDI.
* Currently, Optiboot_x resets the reset cause register after saving the contents in the general purpose register GPIO0. Check this register to determine the reset cause when using the bootloader.
* The new chips have more than one option mapping option for the UART (serial) pins. There is a menu option to choose this, and the one selected when you "burn bootloader" determines which version is used.
* When you "burn bootloader", the base oscillator frequency is set according to the selected clock speed, but the actual operating speed while running the sketch is set in the uploading code. If you initially set it to 16/8/4/1MHz, you may use any of those options when you upload your sketch and it will run at that speed; if you initially set it to 20/10/5MHz, you may use any of those options. If you wish to change between 16/8/4/1MHz and 20/10/5MHz, you must burn bootloader again - failure to do so will result in it running at the wrong speed, and all timing will be wrong.
* The "size" of the sketch as reported by avrdude during the upload process is 512b larger (the size of the bootloader) than the actual sketch size when a bootloader is in use (though the size reported at the end of the compile process is the correct size). The fact that the bootloader goes at the start of the flash instead of the end confuses avrdude. The size displayed by the IDE when you "verify" a sketch is correct, the value that avrdude displays is not.

# Guides
## [Power Saving techniques and Sleep](https://github.com/SpenceKonde/megaTinyCore/blob/master/megaavr/extras/PowerSave.md)
## [Direct Port Manipulation](https://github.com/SpenceKonde/megaTinyCore/blob/master/megaavr/extras/DirectPortManipulation.md)
## [Pin Interrupts](https://github.com/SpenceKonde/megaTinyCore/blob/master/megaavr/extras/PinInterrupts.md)

# List of Tools sub-menus
* Tools -> Chip - sets the specific part within a selected family to compile for and upload to.
* Tools -> Clock Speed - sets the clock speed. You must burn bootloader after changing between 16/8/4/1MHz and 20/10/5MHz to apply the changes (ie, changing from 20MHz to 10MHz does not require burning bootloader, changing from 20MHz to 16MHz does). A virgin chip will be set to use the 20MHz internal oscillator as its base clock source, so 20/10/5MHz clock speed options should work without an initial "burn bootloader" - though we still recommend it to ensure that all other fuses are set to the values you expect.
* Tools -> Retain EEPROM - determines whether to save EEPROM when uploading a new sketch. You must burn bootloader after changing this to apply the changes. This option is not available on Optiboot board definitions - programming through the bootloader does not execute a chip erase function.
* Tools -> B.O.D. Voltage - If Brown Out Detection is enabled, when Vcc falls below this voltage, the chip will be held in reset. You must burn bootloader after changing this to apply the changes. Take care that these threshold voltages are not exact - they may vary by as much as +/- 0.3v! (depending on threshold level - see electrical characteristics section of datasheet). Be sure that you do not select a BOD threshold voltage that could be triggered during programming, as this can prevent successful programming via UPDI (reported in #86).
* Tools -> UPDI/Reset pin. If set to UPDI, the pin will be left as the UPDI pin, there will be no hardware reset pin. If using Optiboot, to enter the bootloader, disconnect and reconnect power to the part or execute a software reset - either way, upload within 8 seconds. If set to Reset, the pin will be configured to act as reset, like a classic AVR, but **UPDI programming will no longer work - you must use an HV programmer if you wish to reprogram via UPDI** - if the pin is set to reset, if using Optiboot, the version of the bootloader used will jump straight to the application after a power-on reset, and will only enter the bootloader if reset by software reset or via the reset pin. The bootloader will also only wait 1 second for a sketch (ie, it behaves like optiboot does on classic AVR microcontrollers), instead of 8. Finally, if set to I/O, the pin will act as a normal I/O pin (though the pin drivers are weaker, like the reset pin on classic AVRs); if using Optiboot, the bootloader will behave in the same way as when the pin is left as UPDI. Note that if not using Optiboot, you must have an HV UPDI programmer to
* Tools -> B.O.D. Mode (active) - Determines whether to enable Brown Out Detection when the chip is active and when it's sleeping; only reasonable combinations are offered. You must burn bootloader after changing this to apply the changes.
* Tools -> Voltage Baud Correction - If you are using the internal oscillator and reaaaaally want the UART baud rates to be as close to the target as possible you can set this to the voltage closer to your operating voltage, and it will use the factory programmed internal oscillator error values. Under normal operation, this just wastes flash and is not needed. That is why it now (as of 2.3.0) defaults to Ignore.
* Tools -> millis()/micros() - If set to enable (default), millis(), micros() and pulseInLong() will be available. If set to disable, these will not be available, Serial methods which take a timeout as an argument will not have an accurate timeout (though the actual time will be proportional to the timeout supplied); delay will still work. Disabling millis() and micros() saves flash, and eliminates the millis interrupt every 1-2ms; this is especially useful on the 8-pin parts which are extremely limited in flash. Depending on the part, options to force millis/micros onto specific timers are available. A #error will be shown upon compile if a specific timer is chosen but that timer does not exist on the part in question (as the 0-series parts have fewer timers, but run from the same variant). If RTC is selected, micros() and pulseInLong() will not be available - only millis() will be.
* Tools -> UART for Bootloader - If using Optiboot bootloader, select which set of pins you want to use for serial uploads. After making a selection, you must connect a UPDI programmer and do tools -> Burn Bootloader to upload the correct bootloader for the selected option.

# Warning: Direct Register Manipulation
If you are manually manipulating registers controlling a peripheral, you should not count on the behavior of API functions that relate to the same peripheral, nor should you assume that calling said API functions will not adversely impact the rest of your application. For example, if you "take over" TCA0, you should not expect that using analogWrite() - except on the two pins on the 20/24-pin parts controlled by TCD0 - will work for generating PWM; indeed you should expect that it will break whatever you are doing with TCA0.

# Warning: I2C **requires** external pullup resistors
Earlier versions of megaTinyCore enabled the internal pullup resistors on the I2C pins. This is no longer done; they are miles away from being strong enough to meet the I2C specifications - it was decided that it is preferably for it to fail consistently without external ones than to work under simple conditions with the internal ones, yet fail under more demanding ones (more devices, longer wires, etc).

# Differences in Behavior between megaTinyCore and Official Cores
While we generally make an effort to emulate the official Arduino core, there are a few cases that were investigated, but we came to the conclusion that the compatibility would not be worth the price. The following is a (hopefully nearly complete) list of these cases.

## Serial does not manipulate interrupt priority
The official core for the (similar) megaAVR 0-series parts, which this was based on, fiddles with the interrupt priority (bet you didn't know that!) in ways that are of dubious safety towards other code. megaTinyCore does not do this (in the process saving several hundred bytes of flash). Writing to Serial when it's buffer is full, or calling `Serial.flush()` while  with interrupts disabled, or during another ISR (which you really shouldn't do anyway) will behave as it does on classic AVRs, and simply block until there is either space in the serial buffer, or the flush is completed.

## digitalRead() does not turn off PWM
On official cores, and most third party ones, the digitalRead() function turns off PWM when called on a pin. This behavior is not documented by the Arduino reference. This interferes with certain optimizations (such as fast digital IO - which is coming soon, I swear) - and moreover is logically inconsistent - a "read" operation should not change the thing it's called on. That's why it's called "read" and not "write". There does not seem to be a logically coherent reason for this - and it makes simple demonstrations of what PWM is non-trivial (imagine setting a pin to output PWM, and then looking at the output by repeatedly reading the pin). See also the above section obn PWM uaing TCD0.

## digitalWrite()/pinMode() and INPUT pins
Like the official "megaavr" core, calling digitalWrite() on a pin currently set INPUT will enable or disable the pullups as appropriate. Additionally, as of 2.2.0, megaTinyCore fixes two bugs in this "classic emulation". On a classic core, digitalWrite() on an INPUT would also write to the port output register - thus, if one subsequently called pinMode(pin,OUTPUT), the pin would immediately output that level. This behavior is not emulated in the official core, and there is a considerable amount of code in the wild which depends on it. digitalWrite() now replicates that behavior. digitalWrite() also supports "CHANGE" as an option; on the official core, this will turn the pullup on, regardless of which state the pin was previously in, instead of toggling the state of it. The state of the pullup is now set to match the value that the port output register was just set to.

Similarly, using pinMode() to set a pin to INPUT or INPUT_PULLUP will now also set the port output register.

## analogWrite() and TCD0 pins
Please see the above PWM feature description if using PWM on those pins and also doing sensitive/niche work on the impacted pins (PIN_PC0, and PIN_PC1).

## TCA0 configured in Split Mode to get 3 additional PWM pins
On official "megaavr" board package, TCA0 is configured for "Single mode" as a three-channel 16-bit timer (used to output 8-bit PWM). megaTinyCore always configures it for "Split mode" to get additional PWM outputs. See the datasheets for more information on the capabilities of TCA0. See [Taking over TCA0](megaavr/extras/TakingOverTCA0.md) for information on reconfiguring it.

## TCA0 TOP is 254, not 255
0 is a count, so at 255, there are 256 steps, and 255 of those will generate PWM output - but since Arduino defines 0 as always off and 255 as always on, there are only 254 possible values that it will use. The result of this is that (I don't remember which) either analogWrite(pin,254) results in it being LOW 2/256's of the time, or analogWrite(pin,1) results in it being HIGH 2/256's of the time. On megaTinyCore, with 255 steps, 254 of which generate PWM, the hardware is configured to match the API, and this does not occur; as it happens, 255 also (mathematically) works out such that integer math gets exact results for millis timing with both 16 MHz derived and 20 MHz derived clock speeds (relevant when TCA0 is used for millis() timing). The same thing is done for TCD0 (though to 509, giving 510 steps - analogWrite() accounts for this - so that we can get the same output frequency while keeping the fastest synchronization prescaler for fastest synchronization between TCD0 and system clock domains).

## Type B timers not used for PWM
On the official "megaavr" board package, as well as [DxCore](https://github.com/SpenceKonde/DxCore), the type B timers are used to generate 8-bit PWM (one pin per timer), there are very few circumstances where this could increase the number of usable PWM pins. These timers are just too scarce and valuable on these parts. Being minimally useful for PWM, in short supply, and highly desirable for other purposes, support for using them for PWM was removed in order to save space that would otherwise be used initializing these timers for PWM and handling them in analogWrite() et. al. If a Type B timer is used for millis(), it is configured in a radically different way than the official core does.

## digital I/O functions use old function signatures.
They return and expect uint8_t (byte) values, not enums like the official megaavr board package does. Like classic AVR cores, constants like LOW, HIGH, etc are simply #defined to appropriate values. The use of enums instead broke many common Arduino programming idioms and existing code, increased flash usage, lowered performance, and made optimization more challenging. While the enum implementation made language design purists comfortable, and provided error checking for newbies - because you couldn't pass anything  that wasn't a PinState to a digital I/O function, and would get that error checking if - as a newbie - you accidentally got careless. A compatibility layer was added to the official core - but then that got rid of what was probably the most compelling benefit, the fact that it did generate an error for new users to train them away from common Arduino practices like passing 1 or 0 to digitalWrite, `if(digitalRead(pin))` and the like. This change also had the perverse effect of making PinMode(pin,OUTPUT), an obvious typo of pinMode(pin,OUTPUT) into valid syntax (comma operator turns pin,OUTPUT into OUTPUT, and it returns a new PinMode of value OUTPUT...), instead of a syntax error. Anyway - the enums are not present here, and they never will be; this is the case with [MegaCoreX](https://github.com/MCUdude/MegaCoreX) and [DxCore](https://github.com/SpenceKonde/DxCore) as well.

# Known Compiler Bugs in 2.0.3 and Earlier
The earlier versions of the avr-gcc toolchain contained several issues relevant to these parts. Version 2.0.4 and later, when installed through Board Manager, will pull in the new toolchain package which corrects both of these issues (among other ones that nobody had noticed yet)
* Sometimes a sketch which is too big to fit will, instead of generating a message saying that, result in the error: 'relocation truncated to fit: R_AVR_13_PCREL against symbol tablejump2'.
* There is a bug in the `<avr/eeprom.h>` library's eeprom_is_ready() macro - it uses names for registers from the XMEGA family of parts (under the hood, these chips actually have an XMEGA core!). Attempting to use that macro will generate a error stating that `'NVM_STATUS' was not declared in this scope`. This has been corrected in current compiler packages; if you get this error, update the official AVR board package, upgrade to Arduino 1.8.13, or install megaTinyCore using board manager.

# Instruction Set (AVRe/AVRe+ vs AVRxt)
The classic AVR devices all use the venerable `AVRe` (ATtiny) or `AVRe+` (ATmega) instruction set (`AVRe+` differs from `AVRe` in that it has hardware multiplication and supports devices with more than 64k of flash). Modern AVR devices (with the exception of ones with minuscule flash and memory, such as the ATtiny10, which use the reduced core `AVRrc`), these parts use the latest iteration of the AVR instruction set `AVRxt`. This adds no new instructions (unlike `AVRxm` , the version used by the XMega devices), but a small number of instructions have improved execution time, and one has slower execution time. This distinction is unimportant for 99.9% of users - but if you happen to be working with hand-tuned assembly (or are using a library that does so, and are wondering why the timing is messed up), the changes are:
* PUSH is 1 cycle vs 2 on classic AVR (POP is still 2)
* CBI and SBI are 1 cycle vs 2 on classic AVR
* LDS is 3 cycles vs 2 on classic AVR :disappointed: LD and LDD are still two cycle instructions.
* RCALL and ICALL are 2 cycles vs 3 on classic AVR
* CALL is 3 cycles instead of 4 on classic AVR
* (Saving the best for last) ST and STD is 1 cycle vs 2 on classic AVR! (STS is still 2)
Note that the improvement to PUSH can make interrupts respond significantly faster (since they often have to push the contents of registers onto the stack at the beginning of the ISR), though the corresponding POP's at the end aren't any faster. The change with ST impacted tinyNeoPixel. Prior to my realizing this, the library worked on SK6812 LEDs (which happened to be what I tested with) at 16/20 MHz, but not real WS2812's. However, once I discovered this, I was able to leverage it to use a single tinyNeoPixel library instead of a different one for each port like was needed with ATTinyCore (for 8 MHz, they need to use the single cycle OUT on classic AVRs to meet timing requirements, the two cycle ST was just too slow; hence the port had to be known at compile time, or there must be one copy of the routine for each port, an extravagance that the ATtiny parts cannot afford. But with single cycle ST, that issue vanished)

## License
megaTinyCore itself is released under the [LGPL 2.1](LICENSE.md). It may be used, modified, and distributed, and it may be used as part of an application which, itself, is not open source (though any modifications to these libraries must be released under the LGPL as well). Unlike LGPLv3, if this is used in a commercial product, you are not required to provide means for user to update it.

The megaTinyCore hardware package (and by extension this repository) contains megaTinyCore as well as libraries, bootloaders, and tools. These are released under the same license, *unless specified otherwise*. For example, tinyNeoPixel and tinyNeoPixel_Static, being based on Adafruit's library, is released under GPLv3, as described in the LICENSE.md in those subfolders and within the body of the library files themselves.

This license *also* does not apply to any tools or third party programs used by, included with, or installed by megaTinyCore. Installing megaTinyCore through board manager will install and/or update other tool(s) to compile and/or upload code. These are covered by their own license terms. The pyupdi-style serial uploader in megaavr/tools is built on pymcuprog from Microchip, which is free, but *not* open source. If this is problematic for you or your organization, the megaavr/tools/pymcuprog and megaavr/tools/pydbglib directories, as well as megaavr/tools/prog.py may be deleted without adverse impact on the core other than the Serial+resistor programming option ceasing to work and instead generating a file not found error. That will remove all content from this package that is not covered by some form of open source license in accordance with their respective license files as outlined above.
