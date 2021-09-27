# Wire (TWI/I2C)
All of these parts have a single I2C/TWI module, and Wire.h provides the usual API.

## NEW in 2.4.3 - totally rewritten Wire library thanks to @MX682X
Should be 100% API compatible, use less flash, and have a new menu option to enable both master AND slave instead of the usual master OR slave. This uses the same pair of pins (this is termed "multimaster mode" in the 'biz). Enabling increases binary size, and allocates another buffer for data (out of RAM), snd being the less common use case, we choose to default to the more efficient implementation in light the stringent constraints on these parts.



## Pin Mappings
On the 1-series parts with more than 8 pins, an alternat pin mapping is available; this is configured using the Wire.swap() or Wire.pins() methods. Both of them achieve the same thing, but differ in how you specify the set of pins to use. This should be called **before** Wire.begin().

| Series           | Default (0) | Alternate (1) |
|------------------|-------------|---------------|
| tinyAVR 0-series | PB0, PB1    | Not Available |
| tinyAVR 1-series | PB0, PB1    | PA2, PA1      |
| tinyAVR 2-series | PB0, PB1    | Not Available |
| ATtiny212, 412   | PA2, PA1    | Not Available |

`Wire.swap(pin_set)` will set the the pin mapping to the specified set of pins. Only 1-series parts with more than 8 pins support this; on other parts,  `Wire.swap()` will generate a compile error if a value known at compile time and not 0 is passed to it. On 1-series parts that do have an alternate pin mapping, a compile-time-known value that is not a 0 or 1 will similarly generate a compile error. An invalid value that is *not* known at compile time (in either case) will instead result in swap() returning false and selecting the default pins.

`Wire.pins(SDA pin, SCL pin)` - this will set the mapping to whichever mapping has the specified pins `SDA` and `SCL`. If this is not a valid mapping option, it will return false and set the mapping to the default. This uses more flash than Wire.swap(); that method is preferred. As with `Wire.swap()`, this will generate a compile error if the pins are compile-time-known constants which are not a valid SDA/SCL pair.

There are no alternate TWI pin mapping options for the 0-series or 2-series devices.

## Pullups
The I2C standard absolutely does require external pullups. The fact that I2C ever works with just the internal pullups is somewhat surprising - but the protocol is designed to be resiliant to mildly adverse hardware conditions. However, as wires get longer and/or slave devices more numerous, the bus capacitance increases and the internal pullups will no longer be sufficient for proper functioning. By popular demand we have added a method to enable the pullups; we suggest using this only as a debugging aid: *if this fixes any problems, you should install external pullup resistors* (in the absence of other devices on the bus having their own pullups breakout boards often do), 4.7k is a good default value for standard speed I2C). After selecting the desired pin mapping (if not the default) call:
```c
Wire.usePullups();
```

## Wire.setClock()
`Wire.setClock()` is not exact (nor is it on the official core). The clock speed always depends on network conditions - the baud generator adapts to electrical conditions, particularly the rise time of the SCL and SDA line. `Wire.setClock()` must be called after `Wire.begin()`, and will briefly disable the TWI interface to make the necessary changes. It will handle the `FMPEN` bit to enable "Fast Mode Plus" for speeds in excess of 400 kHz.

Prior to 5/2021, `Wire.setClock()` did not work correctly and TWI baud rates could be wrong in either direction on any part. Thanks to the work of [@rneurink](https://github.com/rneurink) who saw my lousy "I give up" implementation after I botched it at least three times (all of which made it into a release) the baud calculations should now be reasonably accurate. Thanks to [@MX682X](https://github.com/MX682X), it no longer leaves the bus in a non-functional state when called after Wire.begin().

## Extra Features in Slave mode
In slave mode, it is  possible to respond to the general call (0x00) address as well (Thanks [@LordJakson](https://github.com/LordJakson)!). This is controlled by the optional second argument to Wire.begin(). If the argument is supplied amd true, general call broadcasts will also trigger the interrupt. These parts also support optionally having a second address, or masking (ignoring) some bits in the address. The optional third argument, if supplied, is passed unaltered to the TWI0.SADDRMASK register. If the low bit is 0, any bits set 1 will be ignored (masked off bits will be treated as matching). If the low bit is 1, it will instead act as a second address that the device can respond to. See the datasheet for more details. An everyday example of a part that does that sort of address masking is the 24-series of I2C EEPROMs (any 8-pin I2C EEPROM with 24 in the part number - many vendors make indistinguishable EEPROMs): When the size of the address of any cell within the EEPROM exceeds a multiple of 8 by 3 or less, it will use that many bits of the I2C address to address the memory; those bits are otherwise set by input pins. Hence with maximum options, the slave mode begin() call is:
```c
Wire.begin(uint8_t address, bool receive_broadcast, uint8_t second_address)
```

If the slave is configured to accept more than one address, it will often be critical to know which one it is responding to. This has been the case since the above features were added, but will be addressed in 2.3.4:
```c
uint8_t addr = Wire.getIncomingAddress(); // Returns incoming address in slave mode, currently as 8-bit address (leftshifted one plqace). This may bne changed to 7-bit before the release.
```

## Master/Slave mode
As of 2.4.3, we support operating as master and slave on the same pins (sometimes called a multi-master configuration). To make use of this, you must choose the appropriate option from the `Tools -> TWI (I2C/Wire.h) Options` menu. If "Msater and slave" is selected, flash and RAM use is higher, but you will be able to use both master and slave mode simultaneously. On the tinyAVR parts, there is no "dual mode", a feature of "full size" modern AVRs whereby the slave operation can be moved to a second pair of pins; thus the I2C bus will be conncted to this part plus it's master and it's slave (and potentially several of either or both). Take particular care with voltage levels to ensure that the pullups are connected to a voltage compatible with all three, and that no 5v devices on the bus have the pullups connected (they will work if the pullups are only connected to +3.3v). There is no special code needed - it simply permits you to start both master and slave modes and get working results.

## Errata warning
All modern AVRs, since the release of the first tinyAVR 0/1-series, through the AVR DB-series, have always had a silicon bug relating to the TWI pins. When the TWI peripheral takes control of the SCL and SDA, it correctly controls their being an INPUT or OUTPUT - but it fails to also take over the output value... That means that if the PORTx.OUT bit is 1 for either of the pins, it would be trying to drive the line high instead of low, and the I2C bus would be non-functional. As of 2.2.6, we always clear those bits in begin(); this was not done on older versions. In any event, do not `digitalWrite()` either of the pins  `HIGH` or set their `pinMode()` to `INPUT_PULLUP` after calling `Wire.begin()`. Calling them before that is pointless too, since they will be superseded by begin(). If you want to enabler the internal pullups, use Wire.usePullups.
