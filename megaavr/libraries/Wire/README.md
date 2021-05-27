# Wire (TWI/I2C)
All of these parts have a single I2C/TWI module, and Wire.h provides the usual API.

## Pin Mappings
Like most peripherals, an alternatie pin mapping is available for TWI on all 1-series parts with at least 14 pins; this is configured using the Wire.swap() or Wire.pins() methods. Both of them achieve the same thing, but differ in how you specify the set of pins to use. This should be called **before** Wire.begin().

The available pins will depend on the specific part.
Pins shown in the order SCL, SDA.

| Series           | Default (0) | Alternate (1) |
|------------------|-------------|---------------|
| tinyAVR 0-series | PB0, PB1    | Not Available |
| tinyAVR 1-series | PB0, PB1    | PA2, PA1      |
| tinyAVR 2-series | PB0, PB1    | Not Available |
| ATtiny212, 412   | PA2, PA1    | Not Available |

`Wire.swap(pin_set)` will set the the pin mapping to the specified set of pins. Only 1-series parts with more than 8 pins support this; on other parts,  `Wire.swap()` will generate a compile error if a value known at compile time and not 0 is passed to it. On 1-series parts that do have an alternate pin mapping, a compile-time-known value that is not a 0 or 1 will similarly generate a compile error. An invalid value that is *not* known at compile time (in either case) will instead result in swap() returning false and selecting the default pins.

`Wire.pins(SDA pin, SCL pin)` - this will set the mapping to whichever mapping has the specified pins `SDA` and `SCL`. If this is not a valid mapping option, it will return false and set the mapping to the default. This uses more flash than Wire.swap(); that method is preferred. As with `Wire.swap()`, this will generate a compile error if the pins are compile-time-known constants which are not a valid SDA/SCL pair.


## Pullups
The I2C standard absolutely does require external pullups. The fact that I2C ever works with just the internal pullups is somewhat surprising - but wires get longer and/or slave devices more numerous, the bus capacitance increases and the internal pullups will no longer be reliable. By popular demand we have added a method to enable the pullups - but if this fixes I2C problems, we strongly recommend installing physical pullup resistors. After selecting the desired mapping if not using the default, call:
```
Wire.usePullups();
```

## Wire.setClock()
`Wire.setClock()` is not exact (nor is it on the official core). The clock speed always depends on network conditions - the baud generator adapts to electrical conditions, particularly the rise time of the SCL and SDA line. `Wire.setClock()` must be called after `Wire.begin()`, and will briefly disable the TWI interface to make the necessary changes. It will handle the `FMPEN` bit to enable "Fast Mode Plus" for speeds in excess of 400 kHz.

Prior to 5/2021, `Wire.setClock()` did not work correctly and TWI baud rates could be wrong in either direction on any part. Thanks to the work of [@rneurink](https://github.com/rneurink) who saw my lousy "I give up" implementation after I botched it at least three times (all of which made it into a release) the baud calculations should now be reasonably accurate. Thanks to [@MX682X](https://github.com/MX682X), it no longer leaves the bus in a non-functional state when called after Wire.begin().

## Extra Features in Slave mode
In slave mode, it is  possible to respond to the general call (0x00) address as well (Thanks [@LordJakson](https://github.com/LordJakson)!). This is controlled by the optional second argument to Wire.begin(). If the argument is supplied amd true, general call broadcasts will also trigger the interrupt. These parts also support optionally having a second address, or masking (ignoring) some bits in the address. The optional third argument, if supplied, is passed unaltered to the TWI0.SADDRMASK register. If the low bit is 0, any bits set 1 will be ignored (masked off bits will be treated as matching). If the low bit is 1, it will instead act as a second address that the device can respond to. See the datasheet for more details. An everyday example of a part that does that sort of address masking is the 24-series of I2C EEPROMs (any 8-pin I2C EEPROM with 24 in the part number - many vendors make indistinguishable EEPROMs): When the size of the address of any cell within the EEPROM exceeds a multiple of 8 by 3 or less, it will use that many bits of the I2C address to address the memory; those bits are otherwise set by input pins. Hence with maximum options, the slave mode begin() call is:
```
Wire.begin(uint8_t address, bool receive_broadcast, uint8_t second_address)
```

## Master/Slave mode
This is supported in the hardware - however at this time, the library does not really support this. Note that the hardware only supports master and slave being on the same pair of pins, unlike dual mode on the larger chips, which allows the master and slave to be on separate pins (but it's not supported by Wire either).

## Errata warning
All modern AVRs, since the release of the first tinyAVR 0/1-series, through the AVR DB-series, have always had a silicon bug relating to the TWI pins. When the TWI peripheral takes control of the SCL and SDA, it correctly controls their being an INPUT or OUTPUT - but it fails to also take over the output value... That means that if the PORTx.OUT bit is 1 for either of the pins, it would be trying to drive the line high instead of low, and the I2C bus would be non-functional. As of 2.2.6, we always clear those bits in begin(); this was not done on older versions. In any event, do not `digitalWrite()` either of the pins  `HIGH` or set their `pinMode()` to `INPUT_PULLUP` after calling `Wire.begin()`. Calling them before that is pointless too, since they will be superseded by begin().
