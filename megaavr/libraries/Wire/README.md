# Wire (TWI/I2C)
All of these parts have at least one I2C/TWI module, and Wire.h provides the usual API - and then some.

## NEW in megaTinyCore 2.6.2/DxCore 1.5.0 - sleep guard for TWI
Prior to this version, there were two bugs in the code that prevented a reliable operation of the TWI in Power-Down and Stand-By Sleep modes. Now, the library only allows IDLE sleep while a slave transaction is open, to avoid making the bus unusable. Details in paragraph "Wire & Sleep"

## NEW in megaTinyCore 2.5.0/DxCore 1.4.0 - totally rewritten Wire library thanks to @MX682X
Should be 100% backwards compatible, use less flash, and have a new menu option to enable both master AND slave instead of the usual master OR slave. This uses the same pair of pins (this is termed "multimaster mode" in the 'biz). Enabling increases binary size, and allocates another buffer for data (out of RAM), and being the less common use case, we choose to default to the more efficient implementation in light the stringent constraints on these parts. Parts with "Dual Mode" have the option to use a separate pair of pins for the slave functionality.

## Pin Mappings (tinyAVR)
| Pincount | Default  | Alt (1-series only) |
|----------|----------|---------------------|
|   8-pin  | PA1, PA2 |                     |
| 14+ pin  | PB1, PB0 | PA1, PA2            |

Given as SDA, SCL - notice that the pins within PORTB aren't numbered in the same direction as PORTA and PORTC. Some peripherals follow the numbering of the pins within the port; this is not one of those - it follows the order of the pins as they are oriented around the part. Just be glad they came to their senses

## Pin Mappings (AVR Dx-series)
| Mapping | TWI0 M/S | TWI0 Dual | TWI1 M/S | TWI1 Dual | swap |
|---------|----------|-----------|----------|-----------|------|
| Default | PA2, PA3 | PC2,  PC3 | PF2, PF3 | PB2,  PB3 |    0 |
| Alt1    | PA2, PA3 | PC6,  PC7 | PF2, PF3 | PB6,  PB7 |    1 |
| Alt2    | PC2, PC3 | PC6,  PC7 | PB2, PB3 | PB6,  PB7 |    2 |
| Alt3    | PA0, PA1 | PC2,  PC3 |    N/A   |    N/A    |    3 |

Notes:
* Only tinyAVR has the backwards port weirdness. Dx and Ex have all ports in the same order, and SDA is always 1 pin before SCL.
* Alt3 is only available on AVR DD-series, EA-series and likely future Dx and Ex parts).
* Alt1 is not available on parts which do not have PC6 and PC7 (for TWI0) or PB6, PB7 (for TWI1) because it would be identical to the default mapping.
* Alt2 is available on those parts, since its's primary pins are different - though dual mode is not available if the pins aren't present.
* If a part does not have the listed SCL and SDA pins, that mapping is not available on those parts.
* In all cases the pins are listed as SDA, SCL.

Availability of pin mappings by pincount for AVR Dx-series
| Pin Pair | Function       | 64 pin | 48 pin | 32 pin | 28 pin | 20 pin | 14 pin |
|----------|----------------|--------|--------|--------|--------|--------|--------|
| PA0, PA1 | M/S only       | No `**`| For EA | For DD | For DD | Yes    | Yes    |
| PA2, PA3 | M/S only       | Yes    | Yes    | Yes    | Yes    | Yes    | No     |
| PC2, PC3 | Either         | Yes    | Yes    | Yes    | Yes    | Yes    | Yes    |
| PC6, PC7 | Dualmode Slave | Yes    | Yes    | No     | No     | No     | No     |
| PF2, PF3 | M/S only       | Yes    | Yes    | Yes    | No     | No     | No     |
| PB2, PB3 | Either         | Yes    | Yes    | No     | No     | No     | No     |
| PB6, PB7 | Dualmode Slave | Yes    | No     | No     | No     | No     | No     |

`*` - This is a Wire1 pin option, and is only available where there are two TWI interfaces (currently, the DA and DB series).

`**` - There is no part - either currently available or announced which supports the Alt3 (PA0, PA1) mapping which is available in a 64-pin version. The DA and DB-series do not support the Alt3 mapping.

`Wire.swap(pin_set)` will set the the pin mapping to the specified pin set. See API reference below for details.

`Wire.pins(SDA pin, SCL pin)` - this will set the mapping to whichever mapping has the specified pins `SDA` and `SCL`. See API reference below for details. Wire.pins() only supports specifying mapping by pins for the master/slave pins, not the dualmode slave only pins. If you want the mode with the pins that can't do dual mode slave (PA2/3, or PF2/3) but with the alternate slave pins, you MUST use Wire.swap().

## Official specification of I2C
[From NXP, the current owner of the relevant IP](https://www.nxp.com/docs/en/user-guide/UM10204.pdf)

## Overview - I2C, what is it?
I2C (known by many names, see note at end) uses two pins, a clock (SCL) and data (SDA) for communication among two or more compatible devices. This is an open drain bus - external pullup resistors (*which you must include in your design*) keep the two lines HIGH when idle, and devices communicate by driving the pins low or releasing them. Data is clocked on the rising edge (this is a more important detail than usual, as you will see).
In each transaction, one device, the "master" or "host" initiates communication by writing a "start condition" followed by clocking out a an 1 byte address. The "slave" or "client" device with that address will send a single bit in response (the ACK bit). When using this library to make a slave device, this is configurable. Otherwise, it may be fixed, software-configurable, set by an address pin state, etc; Refer to the applicable datasheet.) The lowest bit of the address indicates whether the master is going to read or write. For a write, it will continue clocking out another byte when it gets the ACK bit, and this will repeat until either the slave refuses to ACK a byte (a NOACK) or the master is done sending. For a read, the after the address is ACK'ed, the master will continue to provide a clock, and allow the slave to control the data line. The master will ACK each byte until has read as many bytes as it is attempting to.

A start or stop condition is simply the data line being changed while the clock line is high; SDA H->L while SCL is high is a start, SDA L-> H is a stop. Otherwise, SDA is only asserted (driven low) or released when SCL is low. The "ACK" bit is generated by whichever device is receiving data, by driving SDA low after the 8th bit is received. A NACK is simply not doing so (so a device that "sends" a NACK, and a device that is not present at all, look the same to the master).

There is one more wrinkle. A slave device might be busy, and need some time to be able to receive data. I2C provides for that by permitting the slave to "stretch the clock" - On the ACK bit, a slave that needs more more time before it can handle additional incoming data (for a write) or output a response (for a read) will drive the SCL and SDA lines LOW before the master releases them (a slave that doesn't need more time can simply hold SDA low to ACK). The slave then can process the incoming message, and either continue holding SDA low to ACK or release it to NACK, and then release SCL. The master will see the rising edge, and that SDA was LOW, and recognize it as an ACK (or as a NACK if the slave released SDA), and generate the last clock pulse (to signal to a slave that ACKed to release the data line). The Wire library, when acting as slave, for example, stretches the clock at the start of a read operation while the onRequest interrupt runs.

When I2C works, it is fantastically convenient. Lots of devices can be added, there is a huge selection of available parts, and while speeds are lower than SPI, they are almost always fast enough. It is easy to use, and the addressing allows hardware to be changed with minimal inconvenience. Because it's open drain, you can interface 3.3v and 5v devices easily. The Wire library lets you make your own I2C/Wire devices from Arduino. The interface you get from Wire makes it very easy to use, you don't need to worry about "dummy bytes" like SPI, or either device being able to start a transaction (like you would if communicating over USART, and everything is great... Until it's not.

The main problem (other than the low speed, which is unavoidable on an open-drain bus) with I2C compared to other serial data buses is that because it has two wires, both of which are bidirectional, and because there is no external slave select pin (which generally acts as a failsafe), nothing prevents one malfunctioning device from taking out the whole bus. Usually either all devices on an I2C bus work, or nothing does. While SPI requires more pins, even a completely hosed SPI device will usually let go of MISO when its SS pin is raised (barring physical damage to the pin). With I2C, the most common problem is that a slave - for some reason - hangs while holding the clock and sometimes the data line as well low. At that point, and a power cycle is often needed to revive it. It can be difficult to figure out which device is malfunctioning, because when you start disconnecting things, if power to the slave is interrupted, it will release SCL and SDA, until you run your code again and whatever it is that confuses the slave happens again. It also requires *external* pullup resistors, (it *just barely works* in simple cases with the internal ones, but becomes unreliable when more devices are added).

## Pullups and logic levels
**The I2C interface is almost never within spec without external pullups.** If you aren't using breakout boards that have their own already installed, you need to add your own. Two pullups are needed, one connects SCL to Vcc, the other connects SDA to Vcc. If the parts run at different voltages, that is an added consideration - the pullups must go to the highest voltage that all parts tolerate, and that voltage must be high enough to be recognized as a HIGH by all parts. This can also require stronger pullups and lower speeds since the rise-time that matters is from the least favorable device's perspective for both minimum and maximum values of the pullups (usually the low voltage device's low input, and the higher voltage device's high threshold).

|  Wire Mode |    Frequency | Supported |  Pin drive |   Maximum Pullup Value (ohms and pF)   |  Minimum pullup value (total)   |
|------------|--------------|-----------|------------|----------------------------------------|---------------------------------|
| Standard   |   100,000 Hz |       Yes |  Under 3mA | R<sub>pu</sub>=1,180,000/C<sub>B</sub> | (V<sub>pu</sub> - (0.2*Vdd or 0.4V))  / 3 mA |
| Fast Mode  |   400,000 Hz |       Yes |  Under 3mA | R<sub>pu</sub>=  354,000/C<sub>B</sub> | (V<sub>pu</sub> - (0.2*Vdd or 0.4V))  / 3 mA |
| Fast Mode+ | 1,000,000 Hz |       Yes | Up to 20mA | R<sub>pu</sub>=  141,600/C<sub>B</sub> | (V<sub>pu</sub> - (0.2*Vdd or 0.4V))/20 mA |

If you know the bus capacitance, you can calculate the maximum value with that using these numbers from the AVR DB-series datasheet (the units have been multiplied through - if the bus capacitance is 50 pF (around 3-4 devices allowing for stray capacitance, in Standard Mode you would need pullups of less than 23.6k ohms, in Fast Mode you would need 7k pullups, and in Fast Mode Plus you would need 2.8k pullups). Unfortunately, you don't know the bus capacitance, and few of us have the equipment to measure it: It's the sum of the capacitance of the I2C pin on each device (which you know from the datasheet - usually), plus the stray capacitance from wires, PCBs, breadboard, etc. For the minimum, it's a simple application of Ohm's law and the V<sub>ILMIN</sub> and the pin drive strength - if the pins can't overcome the pullup to get an unambiguous LOW, obviously nothing will work. Note that both the thresholds and the drive strength are generally different from other I/O pins when acting as I2C. These numbers are guaranteed by the specification to work, provided the other devices in use also conform to the spec. At 5V it would be 1.5k for Standard or Fast Mode, and 200 for Fast Mode Plus.

None of that accounts for the fact that wires, particularly long ones, have non-zero inductance, and the impact of this on rise and fall times is harder to calculate. *I2C was designed to be used between ICs on a circuit board* and long wires will degrade it's performance, requiring stronger pullups and/or lower speeds than you would otherwise be able to use - note that while lowering the speed allows you to use weaker pullups or survive a higher bus capacitance, the minimum pullup value (hence strongest pullups) that can be used is fixed. Most people don't calculate the pullup values - we take an educated guess, and the window is wide enough that standard mode is rarely a problem. For small numbers of parts at standard speed, **4.7k is a good default value**, and *1.5-10k will generally be fine*. At higher frequencies, a smaller resistor might be required, see `Wire.setClock()` for the recommended values. When pulling up to a voltage lower than 5V, you typically need stronger pullups - but this is not usually a problem, since the minimum pullup value also falls.

The internal pullups, however, are typically in the area of 30-50k. That may be okay for 2 devices at standard speed. Even 3-4 devices gets dicey, and wiring could sink even the 2-device case. By default, most classic AVR cores, including the official ones, turn on the internal pullups - giving a default configuration that would work under simple conditions. But as more devices were added, the bus would fail unpredictably, and the failures are often difficult to pin down and intermittent (one would typically wind up debugging a system right on the edge of failing). We don't enable them by default. If you want to use the internal (insufficient) pullups instead of using external ones, go ahead, calling `Wire.usePullups()` after choosing the pins- but do so only with the knowledge that it only has a chance of working on small networks, and may be unreliable or unusable on larger one.  *Wire.usePullups() is intended for debuggign only! If it fixes anything, check the external pullups, because one or both are absent or not connected properly*

The ease of using multiple voltages on an open drain bus was mentioned above, but it's worth elaborating a bit here. The standard certainly doesn't guarantee that a 5V device will recognize an I2C line only pulled up to 3.3V as high (though it generally will) - but on the AVR Dx and AVR Ex parts, there's an option to let you do far better than that: "SMBUS 3.0" voltage levels. This option also, by lowering the threshold voltages, can help cope with high bus capacitance (be careful of the case where devices running at over 3.3v are present which don't have this option enabled, especially if pushing to higher clock speeds. )

## Valid Addresses
Addresses are 7 bits - 8 bits are sent, and the least significant one indicates if it's a read or write operation. This leaves 128 addresses, however, some of them are "reserved", and have a special semantic meaning in I2C and I2C-compatible protocols.
|  Address  | binary format | Purpose                                                 |
|-----------|---------------|---------------------------------------------------------|
|      0x00 |     0b0000000 | General Call, followed by single byte command           |
|      0x01 |     0b0000001 | Reserved for compatibility w/CBUS protocol (obsolete)   |
|      0x02 |     0b0000010 | Reserved for compatibility w/other bus protocols        |
| 0x04-0x07 |     0b00001xx | Master Code for HS I2C  (AVRs do not support this mode) |
| 0x08-0x6F |               | Available - this leaves 104 addresses                   |
| 0x78-0x7B |     0b11110xx | Start of a 10-bit address (Wire supports as master)     |
| 0x7C-0x7F |     0b11111xx | Reserved for future use                                 |

The 10-bit addressing mode is not supported by the hardware; it could be implemented in software for slaves, but this would require modifications to the library, and there is little reason to implement this - uptake of 10-bit addressing has been spotty at best, and most I2C devices do not support it. Since the user has control of the slave address when using this library, and since virtually everything supports 7-bit addressing as master, it is implausible that it would become necessary, and has a non-zero cost (in terms of flash) to implement even if not used. On the master side, 10-bit addressing is simple: Those two bits are the most significant bits of the 10-bit address, and the first data byte sent is the other 8 bits; this can be used if you find yourself having to talk to something that requires 10-bit addresses.

## Extra addressing options in slave mode
In slave mode, it is possible to respond to the general call (0x00) address as well as it's own address (Thanks [@LordJakson](https://github.com/LordJakson)!). This is controlled by the optional second argument to Wire.begin(); If the argument is supplied and true, it will also react to general call commands.
These parts also support optionally having a second address, or masking (ignoring) some bits in the address. The optional third argument, if supplied, is passed unaltered to the TWI0.SADDRMASK register. If the low bit is 0, any bits set 1 will be ignored (masked off bits will be treated as matching). If the low bit is 1, it will instead act as a second address that the device can respond to. See the datasheet for more details. An everyday example of a part that does that sort of address masking is the 24-series of I2C EEPROMs (any 8-pin I2C EEPROM with 24 in the part number - many vendors make indistinguishable EEPROMs): When the size of the address of any cell within the EEPROM exceeds a multiple of 8 by 3 or less, it will use that many bits of the I2C address to address the memory; those bits are otherwise set by input pins. Hence with maximum options, the slave mode begin() call is:
```c
Wire.begin(uint8_t address, bool receive_broadcast, uint8_t second_address)
```

If the slave is configured to accept more than one address, it will often be critical to know which one it is responding to. Use `getIncomingAddress()` to see which address triggered it; This value is the internal representation (e.g., it is left-shifted by 1 place, and the low bit (indicating read/write) should be ignored)
```c
uint8_t addr = Wire.getIncomingAddress() << 1; // Returns incoming address in slave mode, currently as 8-bit address (left-shifted one place).
```

## Master/Slave mode
With the release of the enhanced Wire library we support operating as master and slave on the same pins (sometimes called a multi-master configuration). This functionality is optional, controlled by a Tools submenu. Select `Tools -> TWI (I2C/Wire.h) Options -> Master and Slave` from the tools menu to enable it. This uses more flash and more RAM (for the buffers - Wire requires several buffers, which are 32b each except at the extremes of supported flash sizes. All parts, including the tinyAVR line with the exception of the ATtiny402/202 support it when the master and slave functionality is on the same pair of pins. The full-size parts (the megaAVR 0-series like the ATmega4809, and the AVR Dx-series and upcoming Ex-series) *also* have an option to move the slave functionality to another pair of pins for increased flexibility. Thus, on parts without Dual Mode or where it is not in use, the single I2C bus will be connected to the AVR that is acting in both roles, one or more devices that act as master to control it, and one or more devices that controls - in addition to any other devices that are using I2C to communicate with the master(s) or slave(s). In such complicated configuration, the limits on communication imposed by the physics are the closest at hand - more devices = higher bus capacitance = slower rise and fall times and lower maximum data rates. Take particular care with voltage levels to ensure that the pullups are connected to a voltage compatible with all connected I2C devices.

Thankfully, assuming the hardware is able to handle the job, there is no special code needed, and working sketches can often be made through simple combination of the master code (including `Wire.begin()` with no arguments) and slave code (including `Wire.begin(address)` with one or more arguments, and either `onRequest()` or `onReceive()`). If either master or slave code needs to call `end()` without disturbing the other, there is an `endSlave()` and `endMaster`, as needed.

## Initialization order
There is a right and a wrong order to call the configuration functions. This order should work:
1. Wire.swapModule(&TWI1); (AVR DA/DB for special use cases only)
2. Wire.swap(pinset) or Wire.pins(sclpin, sdapin).
3. Wire.enableDualMode(fmplus_enable); (AVR Dx and megaAVR 0-series only, if needed)
4. Wire.usePullups() *for debugging only - if this fixes it, take it out and add appropriate external pullups)*
5. Wire.begin() and/or Wire.begin(address, ...)
6. Wire.setClock(); (effects master mode only, if needed)
7. Wire.specialConfig() (optional)

See the API reference below for more information.

## API reference
This is a full listing of methods provided for the TwoWire class (the class is named TwoWire, and Wire is an object of class TwoWire). Where they exist and behave the same way as documented in the Arduino Wire API reference they are simply listed. Where they do not, it is described here.

### The TwoWire class
Wire is an object of class TwoWire. The classic AVR Wire.h, like this library, has TwoWire as a subclass of Stream.
The official megaAVR 0-series core that megaTinyCore was based on in the distant past subclassed a new "HardwareI2C" class. Unfortunately, that imposed a shocking amount of overhead with no practical benefit. Code that relies on TwoWire being a subclass of HardwareI2C is virtually non-existent, and code that would benefit from an 500 bytes or so of flash is very common. Any library you encounter that works on classic AVRs (e.g., Uno) but complains of this different inheritance is straightforward to fix, likely as simple as searching the library files for "HardwareI2C" and changing it to "Stream".

### New Tools submenu: Wire Mode
DA and DB devices have all of these options. Others only have the first two
* Master or Slave (default) - This uses the least flash and ram. At any given time Wire can be a master or a slave, but not both and you must call Wire.end(), and then the appropriate form of begin() for the mode you want to enable.
* Master And Slave - In this mode, an argumentless call to begin will start the master functionality, and a call to the form with one or more arguments will start the slave version. Both can run at the same time either using DualMode, or on the same pins (multi-master).
* Master or Slave x2 - In this mode, there is a Wire, and a Wire1 - corresponding to TWI0 and TWI1 peripherals.
* Master and Slave x2 - Combination of the two above options - Both Wire and Wire1 are provided, and *both* can be both a master and a slave at the same time, for a total of 4 pairs of I2C pins (note: having more than one I2C slave defined at once is not recommended, though this library should work. )


### Methods not present in official Arduino Wire library
This version adds several new methods to support additional functionality.
```c++
bool swap(uint8_t state = 1);
```
This will set the pin mapping to the selected pinset (see the table at top of this document). Only 1-series parts with more than 8 pins support this; on other parts, `Wire.swap()` will generate a compile error if a value known at compile time and not 0 is passed to it. On 1-series parts that do have an alternate pin mapping, a compile-time-known value that is not a 0 or 1 will similarly generate a compile error. An invalid value that is *not* known at compile time (in either case) will instead result in swap() returning false and selecting the default pins.

```c++
bool pins(uint8_t sda_pin, uint8_t scl_pin);
```
This will set the the pin mapping to the specified set of pins. If this is not a valid mapping option, it will return false and set the mapping to the default. This uses more flash than `Wire.swap()`; that method is preferred. As with `Wire.swap()`, this will generate a compile error if the pins are compile-time-known constants which are not a valid SDA/SCL pair, and pins not known at compile time will return false select the default pin mapping.

```c++
uint8_t getIncomingAddress();
```
This returns the last incoming address which most recently matched a slave's address, secondary address, or masked-address. Critical to using the general call alternate/masked address options. See the secondary and masked address examples.

```c++
twi_buffer_index_t getBytesRead();
```
When called by an I2C/TWI slave, this returns the number of bytes that have been read by a master since the last time this was called.
To replicate the sort of auto-incrementing pointer that retains state across multiple consecutive reads from the master (without an intervening write, which under the register model that this is intended for will reset the pointer;
When this is called, it resets the internal count; be sure to retain it. It can be called from within or outside of either handlers. See the register_model() and register_model_master examples.
I had always wondered why Arduino sketches that implemented slave functionality used it like Serial-with-a-clock instead of Wire/TWI/I2C. Now I know. Without a function like this, that's all they CAN do!

This method is only useful when operating as a slave. See the register model example for a demonstration of how one might use it.

`twi_buffer_index_t` is a `uint8_t` unless the buffer length has been set to more than 255 bytes, in which case it is a `uint16_t`


```c++
uint8_t slaveTransactionOpen();
```
This method, when called by an I2C slave, will return a value indicating whether the slave is busy (that is, if it has received a read command matching its address, but has either not sent any data, or has sent some data, but has not yet received a NACK after transmitting a byte to the master (which would indicate that the master is done reading from it) - in other words, if it is in the process of sending requested data to the master). If you want to enter sleep mode or change which sleep mode is selected you must make sure this returns 0. As of 2.6.2 we have failsafe measures in place (see sleep section below) to handle this eithout risk failing to let go of the bus, see the sleep section below; it is still possible to not end up in sleep mode in this case from just one call to sleep_cpu().

```c++
endMaster();
```
This is analogous to `Wire.end()`, but only effects the master functionality, as the name implies.

```c++
endSlave();
```
This is analogous to `Wire.end()`, but only effects the slave functionality, as the name implies.

```c++
usePullups();
```
Unlike the official core, we do not automatically turn on the internal pullups, specifically because it can hide problems in simple tests - but not more complicated cases. Combined with the frustrating failure modes of I2C in general (not specific to this library) this can lead to a very challenging debugging experience if/when it does manifest as most I2C devices are added or longer wires are used, possibly dependent on orientation and spatial organization. Thus, we require that you read this paragraph and recognize that it could fail unpredictably before enabling the internal pullups. This is particularly problematic since Arduino users are accustomed to not having to think much about things like wire length and capacitance of wire; this is one of only a few cases where they often become relevant.

```c++
uint8_t checkPinLevels();
```
This function returns the level of the master TWI pins, depending on the used TWI module and port multiplexer settings. Bit 0 represents SDA line and bit 1 represents SCL line. This is useful on initialisation, where you want to make sure that all devices have their pins ready in open-drain mode. A value of 0x03 indicates that both lines have a HIGH level and the bus is ready.

#### Additional New Methods not available on all parts
These new methods are available exclusively for parts with certain specialized hardware; Most full-size parts support enableDualMode (but tinyAVR does not), while only the DA and DB-series parts have the second TWI interface that swapModule requires.
```c++
swapModule(TWI_t *twi_module);
```
This function is only available if the hardware has two modules (DA or DB with 32+ pins); this allows you to swap the Wire object over to use TWI1, allowing the TWI1 pins to be used without creating both Wire and Wire1 - either because you need to use a library hardcoded to use Wire, not Wire1, or because you need to use the TWI0 pins. This must be called first, before `Wire.enableDualMode()` or `Wire.begin()`. Accepts `&TWI0` and `&TWI1` as arguments.

This method is available ONLY if both TWI0 and TWI1 are present on the device, but the tools -> Wire mode menu is not set to an option that creates Wire1. The point is to provide a facility to, without the overhead of both Wire modules, use the TWI1 pins instead of the TWI0 pins.

```c++
enableDualMode(bool fmp_enable);      // Moves the Slave to dedicated pins
```
This enables the "Dual Mode" which moves the slave functionality to a second pair of pins, such that there is a SCL/SDA pair for the master and an SCL/SDA pair for the slave. Some parameters (such as Fast Mode+ support) can be enabled separately for the slave. This must be called before `Wire.begin()` This is only available on megaAVR 0-series, and AVR Dx and Ex-series, not tinyAVR. The version of this document included with such parts will list the slave mode pin-sets. This will generate an error if referenced on a tinyAVR.


```c++
specialConfig(bool smbuslvl = 0, bool longsetup = 0, uint8_t sda_hold = 0, bool smbuslvl_dual = 0, uint8_t sda_hold_dual = 0);
```
New in 2.6.2/1.5.0
There are up to three options currently supported that tweak behavior as regarding time and voltage levels. Most users should have no need to use these, but they are sometimes needed for compatibility particularly with strange voltage levels.

Feature      | TinyAVR 0/1/2 | megaAVR 0 | Dx/Ex |
-------------|---------------|-----------|-------|
SMBus levels | No            | No        | Yes   |
Long setup   | Yes           | Yes       | Yes   |
SDA hold     | Yes, no dual  | Yes       | Yes*  |

`*` This option on the DA-series is impacted by errata on 128k parts:  two values are swapped.

**SMBus levels**
Enabling this (Dx, possibly Ex only) changes the input levels, making them drastically lower, much like the TTL input level option available on some parts. Thresholds as always are the maximum voltage that is still low enough to guarantee will read as low, and the minimum voltage guaranteed to be a high. These normally depend on Vdd - in SMBus voltage level mode, they do not. This is very useful for communicating with lower voltage devices - this is something that is most useful on either the DA (where there is no MVIO) or when the system contains three different voltage levels (say, the AVR running at 5v, using MVIO to talk to a 3.3v device, wishing to use some 1.8V I2C devices).

Vdd               | SMBus levels |  1.8V |  2.5V |  3.3V | 5.0V |
------------------|--------------|-------|-------|-------|------|
Vin Low (max)     |        0.80V | 0.54V | 0.75V | 0.99V | 1.5V |
Vin High (min)    | *      1.35V | 1.26V | 1.75V | 2.31V | 3.5V |

`*` The DA and DD (but not the DB) datasheets imply that the minimum guaranteed high is 1.45V in SMbus mode if running at less than 2.5v. Whether this is an issue with the documentation or an actual difference is unclear.


**SDA setup and hold times**
The setup time for SDA can be either 4 or 8 cycles. May be required for compatibility with some unusual devices.

The hold time can be turned off, or set to 50, 300 or 500 ns. A non-default option is required to comply with SMBus protocol.

**Dual mode options**
Both the voltage levels and the hold times can be configured for the dual mode pins independently from the master/slave pins on parts with those features.


**Constants associated with specialConfig()**.
```C++
#define WIRE_SDA_HOLD_OFF   0
#define WIRE_SDA_HOLD_50    1
#define WIRE_SDA_HOLD_300   2
#define WIRE_SDA_HOLD_500   3

#define WIRE_SDA_SETUP_4    0
#define WIRE_SDA_SETUP_8    1

#define WIRE_I2C_LEVELS     0
#define WIRE_SMBUS_LEVELS   1
```

This method should always return 0 (success). We try to error if compiletime-known invalid values are passed. During development, you should be sure to check this value to make sure that

If not:

| Return value | Meaning                                                   |
|--------------|-----------------------------------------------------------|
| 0x00         | Successful                                                |
| 0x01         | SMBus level requested on part without that feature        |
| 0x02         | Dual mode options passed. Part does not support dual mode |
| 0x04         | Dual mode options passed, dual mode present, but not enabled. Refer to startup order. |
| 0x08         | Invalid sda hold value passed (must be 0 - 3 or one of the named cosntants). The default is used instead |

Linear combinations are possible; 0x0B indicates that you asked for SMBus levels, and one or more dual mode options on a tinyAVR which supports none of those things, and you passed an invalid value for the SDA hold times, 0x05 indicates you asked for SMBus levels and one or more dual mode options without first enabling dual mode, on a megaAVR 0-series which does not support SMBus, and so on.

### Standard methods and features significant differences
```c++
begin();
```
Calling `begin()` with no arguments starts the master. It does not start slave mode.

It does NOT turn on the pullups on any pin - unlike the standard version.

```c++
begin(uint8_t  address, bool receive_broadcast = 0, uint8_t second_address = 0);
```
This starts the slave Wire functionality. *It does not start master functionality - when both are enabled, begin() and begin(address) must be called*. The second and third arguments are optional. The first argument simply specifies the slave address to listen on (like on standard `Wire.begin()`), the second argument enables receiving of general call addresses, and the third allows specification of either a second address, or a mask.

If receive_broadcast is true, the handler selected by `Wire.onReceive()` will be called when a "General Call" message is seen, containing the data or command included with it. General call commands are always writes, perhaps obviously (since more than one device attempting to respond would result in nothing but gibberish). According to the specification, a general call is followed by a 1 byte command, either 0x06 or 0x04 (0x00 is forbidden). The commands instruct the slave to use a previously programmed address, where 0x06 also instructs the slave to do a software-reset (not implemented by Wire library). If not specified, this defaults to `false`; it must be specified if the third argument is used.

If second_address is supplied, two options can be used. In both cases a 7 bit address is supplied in the largest 7 bits (that is, left-shifted once from the traditional Arduino representation); it's function is controlled by the the least significant bit - if the LSB is 1, it's a second address matched in addition to the first. Otherwise, bits that are 1's are not masked off, and are not considered. Two helper macros are provided - these are meant for the sole purpose of code readability - the macro names are self explanatory.
```c++
#define WIRE_ALT_ADDRESS(alt_address) ((alt_address << 1) | 0x01)
#define WIRE_ADDRESS_MASK(mask) (mask << 1)
// The logic provided by the hardware is equivalent to
// if (received_address) == (address) ||  (received_address) == (address)
// and
// if ((received_address | mask) == (address | mask))
```
The third argument may be omitted if not used; it will default to 0, which is written unaltered to the hardware register; it will be treated as an address mask (the low bit is 0), but because it only masks bits that are 1's - it will not mask any bits (this functionality is always enabled - we just write a 0 if it's not specified - it has no further overhead as it is handled entirely by hardware after that.

When the second or third argument was used, `Wire.getIncomingAddress()` should be called during the interrupt handler or handlers; General call will only trigger the receive handler, while a masked/secondary address can take either a read or a write.

If (and only if) the Master and Slave option is selected in the Tools -> Wire mode, the Wire interface can be enabled for both master and slave. Even when Dual Mode is used, the correct option must still be selected to enable acting as both master and slave.

```c++
setClock(uint32_t);
```
The `setClock()` method has it's usual function. `Wire.setClock()` is not exact. The hardware clock generator monitors the SCL line, and begins the next pulse only after the pin has returned to HIGH and been there for a requisite amount of time. The length of these times, t<sub>high</sub> and t<sub>low</sub> are controlled by the `TWIn.MBAUD` register, which is what setClock() changes. But the period of each cycle is composed of 4 parts: t<sub>high</sub> +t<sub>fall</sub> + t<sub>low</sub> + <sub>fall</sub>. the high and low times are controlled by this register, and tFall is influenced by whether the part is set to "FM+ mode" (this drives the pin harder). As described above, the factor limiting the speed of I2C as the speed gets faster is the rise time, which is controlled solely by the total strength of all the pullups on the bus, and the capacitance of the bus. Since the baud generator adapts to electrical conditions, which are not known to the software, this clock setting would only match with one combination of pullup strength and bus capacitance, and at the extremes, the difference in clock speed with the same baud rate set but different electrical conditions on the bus could be 50% or more. *This is preferable to the alternative approach of ignoring the electrical conditions, setting a fixed clock speed, and failing to transfer data under adverse electrical conditions*.

`Wire.setClock()`  must be called after `Wire.begin()`, and will briefly disable the TWI interface to make the necessary changes. It will handle setting the `FMPEN` bit to enable "Fast Mode Plus" for speeds in excess of what Fast Mode is capable of. Anything above 400 kHz will set the FMPEN bit to enable Fast Mode Plus (in dual mode, the slave pins must have this configured separately, since the slave needs to be warned ahead of time to expect Fast Mode Plus). Fast Mode and Standard are indistinguishable from the software's perspective.  Most parts these days support fast mode, and ones that support Fast Mode+ are common. You must be sure to run the bus at a speed compatible with all connected devices and for which the pullup is sufficient. See the discussion above for more about pullups. If not specified, the default is to approximate 100 kHz SCL clock speed. The speeds chosen by this method are more likely to be lower than you asked for than higher - this helps prevent accidentally using a higher clock speed than your other devices support. If it is problematic that the exact speed cannot be controlled tightly for I2C, the I2C bus is not the appropriate technology for your application. SPI allows the clock speed to be known with certainty at compile time (and allows much higher transfer speeds because it isn't limited by the rise-time of the open drain line)


|  Wire Mode | Maximum Frequency | Supported   |
|------------|-------------------|-------------|
| Standard   |        100,000 Hz | Yes         |
| Fast Mode  |        400,000 Hz | Yes         |
| Fast Mode+ |      1,000,000 Hz | Yes         |
| High Speed |      3,400,000 Hz | Unsupported |

Note that the resistor choice affects the SCL frequency that is generated by the AVR. This has something to do with how the TWI module is implemented in hardware. Generally, it does not impact the functionality of the bus though. The formula to calculate the frequency offers to compensate for the rise-time. To keep the library as small as possible, it was decided to measure the rise time for a couple of reasonable resistor choices, which you can find in the table below. Sticking to this values should result in accurate SCL frequencies as well as keep the rise-time inside the I2C specifications (usually).
Remember that the SCL frequency is generated by a divider, so at lower CPU frequencies, higher SCL frequencies will differ from the requested frequency more, then at higher CPU frequencies.

| expected Frequency | Recommended resistor |
|--------------------|----------------------|
|       < 400,000 Hz |             4.7k Ohm |
|       < 600,000 Hz |             2.2k Ohm |
|       >=600,000 Hz |             1.5k Ohm |

High Speed Mode is different animal altogether: FM+ more or less exhausted what could be achieved with a purely open drain bus, and of course, folks still demanded a faster bus. High Speed mode added a current source, and these devices usually separate HS from non-HS devices, using separate pins for each. No AVR device so far released supports HS I2C, and when such speeds are necessary, SPI is a better solution.

`Wire.setClock()` has been varying degrees of broken for most of the history of megaTinyCore. Users [@rneurink](https://github.com/rneurink) and [@MX682X](https://github.com/MX682X) made contributions and since 2.3.3 it has been reasonably close to correct. The old library was kind of a dumpster fire - this was far from the only problematic area of it. In a later version, the frequency accuracy was improved.

```c++
flush();
```


~A `flush()` method exists on all versions of Wire.h; indeed, `Stream` which it subclasses demands that - however very rarely is it actually implemented by anything other than Serial - (where there is a specific and very common reason use case) where one must clear the buffer in a specific way (by waiting for it to empty) before doing things like going to sleep or performing a software reset. Wire has rarely implemented this. In this case, it performs the functionality that the datasheets refer to as a "TWI_FLUSH" - this resets the internal state *of the master* - and at the Wire library level, the buffers are cleared; that command is apparently intended for error handling. The hardware keeps track of activity on the bus (as required by the protocol), but misbehaving devices can confuse the master - they might do something that the specification says a device will will not do, or generate electrical conditions that the master is unable to interpret in a useful way - pins not reaching the logic level thresholds, malformed data and which may in turn leave it confused as to whether the bus is available. It might be necessary to call in an attempt to recover from adverse events, which has historically been a challenge for the Wire library.~

There are two issues that lead to this method being a stub (do-nothing method, required to subclass stream). First is the tension with between the hardware "flush" functionality, which according to the datasheet is intended to clear up detected bus errors, with the arduino API for HardwareSerial, the only stream where flush is generally implemented, where it instead waits for outgoing transmissions to finish. Secondly, either all, or all non-tinyAVR parts appear likely to be impacted by an erratum that renders the hardware mechanism less than useful: "Flush Non-Functional", which according to the errata sheets can in practice cause the very problem it was intended to solve, and advising us to disable and re-enable TWI master instead.

So, using the hardware's flush function to clear the master's state is out. One can also argue that because of the precedent set by HardwareSerial, we should defer to it's well-defined behavior over the foggy definition of the arduino Stream API, and have it mimic that functionality. But in that case, for the I2C master, the methods to send data already block until the operation is completed, and on the slave side, the device is at the mercy of the master - we could be waiting literally an eternity (that is, it is equivalent to using Serial.flush() when acting as a slave device in synchronous mode to transmit when the XCK pin has no clock signal - this is essentially the same situation with a different interface. Waiting for data to be clocked out, without control over that clock, is probably not the ideal API to provide either; the same behavior can be achieved with `while (slaveTransactionOpen()) {...}`, but this gives an opportunity to have a timeout after which the slave can decide that the master has fallen over and (for example) disable and re-enable the interface, expecting that the next contact from the master will be a fresh start condition from a reinitialized master.

It is not hard to imagine a scenario where that would be relevant: Just consider the case of writing application code for the master, and uploading a new version while it happens to be in the middle of a transaction. You probably do not want to wait for the (reset) master to attempt (and fail at, because it would get a bus arbitration error as the slave attempted to answer at the same time the master tried to clock out the address) communication before doing anything, as you'd likely then have to reset both devices at once to upload - sometimes. If you're waiting on the completion of transactions before taking some action, you probably want to make sure you are handling the case where the master does not finish the transaction for some reason. I2C masters rarely take long periods of time between clocking in bytes when functioning correctly. At 100kHz, a byte takes under 100 us to transfer, and the master (if it's another Arduino device) is likely blocking during that time, waiting to grab another byte immediately. Even with a 130 byte buffer, this means that after a dozen ms, it should have been able to receive an entire buffer. If you were to wait 20 ms (or somewhat longer if you specifically modify the library for a massive buffer), you could conclude that the master is unlikely to ever finish. On the other hand, when the only think you intend to do is put the part to sleep, you can attempt to sleep on every pass through loop() instead as described below. On the third hand, if the system were battery powered, and that occurred, the slave would still be in a bad state, consuming IDLE sleep mode current, instead of POWER_DOWN current, which is many times lower, so maybe this would not be so great after all. In any event, a flush that worked like hardware serial would not help that case either, but rather make it drain the battery about twice as fast by not sleeping at all. This brings discussion back to the point I mentioned near the beginning: When something goes wrong with I2C, it usually goes very wrong. Unlike SPI, where you have a pin state to watch to see if the master thinks you are in a transaction, there is nothing physical that you can monitor and notice that the master vanished in the middle of a transaction.

```c++
uint8_t endTransmission(bool sendStop);
```
In 2.5.4/1.4.4 it was reported that the return value of this method did not match the API - it was returning a count of bytes written, which broke lots of existing code. This is now corrected. Values returned are a superset of the the stock version, covering three additional causes of errors. These are not expected to break existing code, since a 0 still indicates success, and it only reports some things with new codes that previously were considered "unknown errors", which should still be treated as errors by code.

| Value | Meaning                                                        | Standard |
|-------|----------------------------------------------------------------|----------|
|  0x00 | Success                                                        | Yes      |
|  0x01 | ~TX buffer overflow~ (see note)                                | Yes      |
|  0x02 | Timeout waiting for ack of address                             | Yes      |
|  0x03 | Timeout waiting for ack of data                                | Yes      |
|  0x04 | Unknown error                                                  | Yes      |
|  0x05 | Timeout                                                        | Yes      |
|  0x10 | Arbitration lost                                               | No       |
|  0x11 | Line held low or not pulled up                                 | No       |
|  0xFF | Bus in unknown state (begin() not called?)                     | No       |

In the case of a TX buffer overflow, this is not indicated by endTransmission(). endTransmission simply transmits the portion of the buffer that fits. This condition should be detected (if your code could potentially generate it), because write() earlier returned a number smaller than the number of bytes passed to it. Error code 1 is never returned by endTransmission; we do not store an extra byte of state just to report this condition that was already reported by write(), with more complete information

### Methods that have their standard behaviour
The implementation isn't identical, but the behaviour is unchanged, or is different only in an irrelevant and implicitly convertible return or argument type.
```c++
    void beginTransmission(uint8_t address);
    twi_buffer_index_t requestFrom(uint8_t address, uint8_t quantity, uint8_t sendStop); // changed from size_t return type. Argument types differ, but are implicitly convertible without issues.
    size_t write(uint8_t data);
    int available();
    int read();
    int peek();
    void end();
```
`twi_buffer_index_t` is a `uint8_t` unless the buffer length has been set to more than 255 bytes, in which case it is a `uint16_t`

### Remember when the handlers are called
```c++
    void onReceive(void (*)(int));
    void onRequest(void (*)(void));
```

#### Write Sequence
1. Master generates start condition.
2. Master clocks out the slave address with out read-bit set..
3. Slave detects and ACKs.
4. Master clocks out 1 or more data bytes as slave ACKs them.
5. Master generates a Stop Condition.
6. Slave fires `onReceive` handler passing it the number of bytes as an int16_t

#### Read Sequence
1. Master generates start condition.
2. Master clocks out the slave address with read-bit set
3. Slave detects and stretches the clock.
4. Slave fires onReceive handle.
5. In `onRequest` handler, stages all the data the master can read at this time (up to the size of the Wire Buffer) noted above.
6. Slave releases clock and ack's.
7. Master clocks in 1 byte. Slave interrupt fires *silently* after each byte to prepare the next byte and the master ACKs each one before finally NACKing when done and generating a stop condition.
8. At that point the master's `endTransaction()` call (assuming it is another Arduino) returns, and the master processes the data it received.

## Wire and Sleep on Slave devices
Between 2.5.0 and 2.6.1, the library was checking the BUSERR flag and aborted a transmission and the data if it there was an error. However, when a device was in Stand-by(SB) or Power-Down(PD) sleep mode, the BUSERR bit would be set on the STOP condition, rendering any received transmission like it never happened. Investigation of the issue concluded that the BUSERR detection circuitry relies on CLK_PER, which is generally disabled in sleep. Because of this it would not register the START condition and saw only the two STOP conditions on the bus - an invalid state. It was decided to not check for BUSERR at all, as often other errors would also cover the BUSERR cases.

Furthermore, with SB and PD sleep, the library was prone to leave the TWI bus in an undefined state, where both lines would be kept pulled low. This happened because the TWI can wake the CPU from SB/PD only on an address match, not on a data interrupt. When code attempted to put the CPU to sleep between an address match and the data interrupt, the slave will keep the SCL line low and the TWI stays unresponsive. There was an attempt to mitigate this in 2.6.1 with `slaveTransactionOpen()` but due to the lack of control over when the master might choose to initiate communications, this did not always achieve satisfactory results, and made for ugly implementations.

As of 2.6.2, the Wire library makes sure that the CPU can not enter SB/PD sleep mode between an address match interrupt and a STOP condition. This is achieved by buffering the current sleep setting and changing the sleep setting to IDLE - in this case, when the user tried to put the device to sleep  At a STOP condition, the buffered value is copied back to the register. This method was chosen, because it offers a further reduction of power consumption, as the CPU can stay in IDLE between data interrupts, as well as preventing the TWI bus of becoming unresponsive. Furthermore, the only thing the user has to do is to call `sleep_cpu()` repeatedly as long as no other jobs have to be executed. If not, the CPU will just wake up on the next data interrupt and stay awake, increasing power consumption.

Note that this is applicable only to slave devices. When acting as I2C master, you cannot accidentally put the part to sleep during a transaction unless you call sleep_cpu() from within an interrupt that fires during a transaction. In that case, you deserve what you get, which is likely the device being hung until a powercycle or reset. (Seriously folks, don't put the part to sleep within an interrupt. Not just in the context of wire. Whatever you're hoping to achieve, that is not the way to do it, and you're not going to like what it does).

## Errata warning
All modern AVRs, since the release of the first tinyAVR 0/1-series, through the AVR DB-series, have always had a silicon bug relating to the TWI pins. When the TWI peripheral takes control of the SCL and SDA, it correctly controls their being an INPUT or OUTPUT - but it fails to also take over the output value. That means that if the PORTx.OUT bit is 1 for either of the pins, it would be trying to drive the line high instead of low, the I2C bus would be non-functional **and hardware damage - while unlikely - could result**. The errata for the 2-series and DD-series implies that this issue does not impact those parts `*`. Nonetheless, we always clear those bits in begin(). Users should refrain from writing the I2C pins `HIGH` via `digitalWrite()` or any other means after calling `Wire.begin()`. Calling them before then is pointless too, since they will be superseded by begin(), and . *If you want to enable the internal pullups, use Wire.usePullups() - this should be done only as a debugging aid.*

`*` - Presence of an issue in the errata is a reliable indicator of the bug being present, but not seeing the issue listed for that part don't mean it ain't there. Users routinely report entirely novel issues, and I am aware of two bugs that have not (yet) made it into *any* errata sheet despite being known to Microchip and impacting many - in some cases *all* modern AVRs - and which had just gone unnoticed until some Arduino user tried to make it do something obscure.

## Known incompatible devices
For an unknown reason, the contactless IR-thermometer MLX90614 is not working out of the box with the modern AVRs. We suspect it is related to the new TWI module implementation interacting with a some aspect of the interface on the MLX90614 which does not follow the specifications. A workaround is to set the clock frequency to a value between 110-125kHz.

I would also suggest testing this with `Wire.specialConfig(0,1,3)` with the default speed (which normally doesn't work). If that fixes it, see if either of those non-zero arguments can be zero'ed out. If the third argument is the key one, see how far you can lower it before it fails (note that this may depend on the physical electronic properties of the bus that determine the rise and fall times). If you do this experiment, please report your results to me so that I can update this section!

At least two of the rewrites of the clock caluculation algorithm that have been submitted, independently, by two different individuals (the second of whom didn't stop there and rewrote the whole library, slashing it's flash footprint while adding functionality) were prompted initially by users trying to get this specific part to work. So - really we're all in debt to this manufacturer, whose combination of compelling hardware saddled with a badly botched I2C interface has led to so much improvement in our Wire library.

## Why are there so many names for this protocol?
Wire, TWI (Two Wire Interface), Two Wire, IIC, I2C-compatible, I2C, I<sup>2</sup>C... The reason for this is that I2C (and the explicitly formatted version of it, I<sup>2</sup>C) are trademarked by Phillips (now NXP) which has historically been very litigious, and would go after manufacturers of parts that didn't pay license fees. So devices that could communicate with and which were I2C in all but name proliferated. The last patent expired a while ago, but they still hold the trademarks, so other manufacturers persist in using their names. The actual terms described in the specification claim to cover to all devices that "can" communicate over I2C, and make exception only for FPGAs, where the person programming them also was supposed to get a license. It seems that the litigation wars have cooled somewhat now, though I still would be mighty careful if I was designing microcontrollers. In any event, Atmel always called it TWI, and that tradition was not lost when Microchip purchased them. The original name, I<sup>2</sup>C is a stylization of "IIC", for "Inter-Integrated-Circuit".

Just don't get it confused with I2S (that's a specialized protocol for real-time transmission uncompressed digital audio - for example, it is (or was) often used in CD players between the disk reading circuitry and the DAC - as well as in professional audio equipment. I2S isn't supported by the AVR line - it's not a good match for the capabilities or intended use cases of AVR devices; it's for dedicated audio stuff, not general purpose microcontrollers). or I3C (a much faster superficially similar successor meant for much faster parts with far more computational power - also not supported by AVR). Even calling it superficially similar is being rather generous, as the resemblance seems to be largely window dressing: there is a data line called SDA and a clock line called SCL, devices can be master or slave, and the slave devices have 7 bit addresses - and that's about the end of the resemblance. While the I3C standard claims backwards compatibility, it does not support clock stretching (which it calls "rarely used" - I'm not sure what gave them that idea, but the initiative is being spearheaded by Google, so it should surprise nobody that they are out of touch with reality). An AVR acting as slave could not even reach user onRequest or onReceive code even if the I3C bus was running at only 1 MHz. It runs at 12.5 MHz - in other words, these parts are between one and two orders of magnitude too slow to meet the timing constraints of I3C).
