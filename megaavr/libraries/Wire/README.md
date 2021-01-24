# Wire (TWI/I2C)
All of these parts have a single I2C/TWI module.

## Pin Mappings
Like most peripherals, an alternatie pin mapping is available for TWI on all 1-series parts with at least 14 pins; this is configured using the Wire.swap() or Wire.pins() methods. Both of them achieve the same thing, but differ in how you specify the set of pins to use. This should be called **before** Wire.begin().

`Wire.swap(1) or Wire.swap(0)` will set the the mapping to the alternate (1) or default (0) pins. It will return true if this is a valid option, and false if it is not (you don't need to check this, but it may be useful during development). If an invalid option is specified, it will be set to the default one.

`Wire.pins(SDA pin, SCL pin)` - this will set the mapping to whichever mapping has the specified pins as SDA and SCL. If this is not a valid mapping option, it will return false and set the mapping to the default. This uses more flash than Wire.swap(); that method is preferred.

## SCL and SDA variables
The two pin variables, SCL and SDA are not static declarations in megaTinyCore - on parts that have an alternative mapping, they are #defined as an expression that tests the TWI portmux bit.

## Pullups
The I2C standard absolutely does require external pullups. The fact that I2C ever works with just the internal pullups is somewhat surprising - but wires get longer and/or slave devices more numerous, the bus capacitance increases and the internal pullups will no longer be reliable. By popular demand we have added a method to enable the pullups - but if this fixes I2C problems, we strongly recommend installing physical pullup resistors. After selecting the desired mapping if not using the default, call:
```
Wire.usePullups();
```

## Extra Features
As with megaTinyCore, courtesey of https://github.com/LordJakson, in slave mode, it is now possible to respond to the general call (0x00) address as well. This is controlled by the optional second argument to Wire.begin(). If the argument is supplied amd true, general call broadcasts will also trigger the interrupt. Not long after that PR, as I examined the TWI section of the datasheet, I noticed that there was also a register that could act as either a second address, or as a mask (SADDR). This third argument is passed unaltered to the TWI0.SADDRMASK register. If the low bit is 0, any bits set 1 will cause the I2C hardware to ignore that bit of the address (masked off bits will be treated as matching). If the low bit is 1, it will instead act as a second address that the device can respond to. Hence with maximum options, the slave mode call is this, but all options except the address are optional
```
Wire.begin(uint8_t address, bool receive_broadcast, uint8_t second_address)
```

## Errata warning
All modern AVRs, since the release of the first tinyAVR 1-series, through the AVR DB-series, they have always had a silicon bug where the TWI peripheral can take over the port direction... but not the output state. So if the PORTx.OUT bits were 1, it would be trying to drive the line high instead of low; this would not be particularly effective. As of 2.2.6, we always clear those bits when we initialize a TWI. This core, whenever begin() is called, explicitly clears the PORTx.OUT bits for the SDA and SCL lines.
