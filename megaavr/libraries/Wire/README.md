# Wire (TWI/I2C)
All of these parts have a single I2C/TWI module, and Wire.h provides the usual API.

## Pin Mappings
Like most peripherals, an alternatie pin mapping is available for TWI on all 1-series parts with at least 14 pins; this is configured using the Wire.swap() or Wire.pins() methods. Both of them achieve the same thing, but differ in how you specify the set of pins to use. This should be called **before** Wire.begin().

`Wire.swap(1) or Wire.swap(0)` will set the the mapping to the alternate (1) or default (0) pins. On parts which do not support an alternate pinout for TWI, `Wire.swap()` will generate a compile error if a compile-time-known value other than 0 is passed to it; on parts which do, a compile-time-known value that is neither 0 nor 1 will similarly generate an error. An invalid value that is *not* known at compile time will instead result in swap() returning false and selecting the default pins.

`Wire.pins(SDA pin, SCL pin)` - this will set the mapping to whichever mapping has the specified pins `SDA` and `SCL`. If this is not a valid mapping option, it will return false and set the mapping to the default. This uses more flash than Wire.swap(); that method is preferred. As with `Wire.swap()`, this will generate a compile error if the pins are compile-time-known constants which are not a valid SDA/SCL pair.

## SCL and SDA variables
The two pin variables, SCL and SDA are not static declarations in megaTinyCore - on parts that have an alternative mapping, they are #defined as an expression that tests the TWI portmux bit.

## Pullups
The I2C standard absolutely does require external pullups. The fact that I2C ever works with just the internal pullups is somewhat surprising - but wires get longer and/or slave devices more numerous, the bus capacitance increases and the internal pullups will no longer be reliable. By popular demand we have added a method to enable the pullups - but if this fixes I2C problems, we strongly recommend installing physical pullup resistors. After selecting the desired mapping if not using the default, call:
```
Wire.usePullups();
```

## Wire.setClock()
`Wire.setClock()` is not exact (nor is it on the official core). The clock speed always depends on network conditions - the baud generator adapts to electrical conditions on the pins and - if they are rising more slowly than they should) it will run more slowly. Master SCL clock frequencies of 50 kHz, 100 kHz (Standard Mode), 200 kHz, 400 kHz (Fast Mode), 700 kHz, and 1 MHz (Fast Mode Plus) are supported. To use speeds higher than 400 kHz, the main clock must be higher than 8 MHz, and TWI must either be disabled, or `FMPEN` must be set (`TWI0.CTRLA |= TWI_FMPEN_bm` - like the rest of TWI0.CTRLA, it must be set prior to calling begin(); they cannot be configured while the peripheral is enabled). If the peripheral is not enabled, `FMPEN` is not set, and one of the Fast Mode Plus speeds is requested, `FMPEN` will be set; if one of those speeds is requested when the peripheral is already enabled and FMPEN not set, the next fastest, 400 kHz will be used instead.

Prior to 2.3.2, Wire.setClock() did not work correctly and TWI baud rates could be wrong in either direction on any part.

## Extra Features
As courtesey of https://github.com/LordJakson, in slave mode , it is now possible to respond to the general call (0x00) address as well. This is controlled by the optional second argument to Wire.begin(). If the argument is supplied amd true, general call broadcasts will also trigger the interrupt. These parts also support optionally having a second address, or allow you to mask (ignore) some bits in the address. The optional third argument, if supplied, is passed unaltered to the TWI0.SADDRMASK register. If the low bit is 0, any bits set 1 will be ignored (masked off bits will be treated as matching). If the low bit is 1, it will instead act as a second address that the device can respond to. Hence with maximum options, the slave mode begin() call is:
```
Wire.begin(uint8_t address, bool receive_broadcast, uint8_t second_address)
```

## Errata warning
All modern AVRs, since the release of the first tinyAVR 0/1-series, through the AVR DB-series, have always had a silicon bug relating to the TWI pins. When the TWI peripheral takes control of the SCL and SDA, it correctly controls their being an INPUT or OUTPUT - but it fails to also take over the output value... That means that if the PORTx.OUT bit is 1 for either of the pins, it would be trying to drive the line high instead of low, and the I2C bus would be non-functional. As of 2.2.6, we always clear those bits in begin(); this was not done on older versions. In any event, do not `digitalWrite()` either of the pins  `HIGH` or set their `pinMode()` to `INPUT_PULLUP` after calling `Wire.begin()`. Calling them before that is pointless too, since they will be superseded by begin().
