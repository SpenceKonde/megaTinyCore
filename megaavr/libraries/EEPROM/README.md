## **EEPROM Library V2.1** for megaTinyCore

**Written by:** _Christopher Andrews_.

### **What is the EEPROM library.**

Th EEPROM library provides an easy to use interface to interact with the internal non-volatile storage found on tinyAVR 0/1-series any megaAVR 0-series devices. It was adapted for these devices by Spence Konde for distribution as part of megaTinyCore ( https://github.com/SpenceKonde/megaTinyCore ). The interface is fully compatible with the standard Arduino EEPROM.h library. EEPROM memory will retain it's stored data when the device is powered off. If the EESAVE bit in the SYSCFG0 fuse is set, it will retain it's values even if a UPDI programmer is used to issue a Chip Erase command, unless the part has been "locked" via the LOCKJBIT fuse, in which case the EEPROM is always cleared.

### **Additional features when used with megaAVR 0-series and tinyAVR 0/1-series devices**

Like all AVR microcontrollers, these devices have the usual complement of EEPROM memory available (See the datasheet or megaTinyCore part-specific documentation for more details). In addition to the standard EEPROM, they also have an additional "page" of memory available in what Atmel/Microchip refers to as the "user row". When using this version of EEPROM.h (Included in megaTinyCore 1.1.9 and later), the same methods described below work in the same way to read and write the user row. The library will treat any address greater than 0xFF (255) as pointing to the user row, as no devices in these product lines provide more than 256b of EEPROM memory.

The user row can be used like normal EEPROM. Unlike normal EEPROM, when the chip is locked (via the LOCKBIT fuse), it can be *written to* (but not read from) via UPDI. It is still erased when a Chip Erase instruction is used.

### **Library limitations**

As with most Arduino libraries, this library sacrifices performance and functionality in favor of ease of use. This is particularly true here - unlike "classic" AVR devices, these provide a readiliy accessible facility for programming EEPROM memory a whole page at a time (see the datasheet for additional information) - this library writes all memory one byte at a time. Providing a high performance library that made use of that would require that the library be aware of the page boundaries; after consideration, it eas decided that the flash and performance overhead of such a system would not nr justified in the resource-constrained environment of a tinyAVR device as this version of the library is intended for. 

No facility for wear leveling is provided, other than the `update()` and `put()` methods checking that the contents have not changed before writing. Under typical usage scenarios, further wear leveling methods are unnecessary - these devices are rated for 100,000 erase-write cycles, and according to the datasheet, the hardware only erases and writes bytes which have been written to the page buffer - it deos not perform a read-modify-write cycle on the whole page when any byte within the page is written or erased. If you do determine thas wear leveling is required for your application, many libraries that do this are available from various authors. Be aware that there is a bug in eeprom_is_ready() macro provided by the <avr/eeprom.h> included with most versions of avr-libc (the new toolchain package included with megaTinyCore 2.0.4 corrects this), which impacts many third party EEPROM-rel;ated libraries.  The symptom of this issue is an error referring to NVM_STATUS; to fix it, search the library code for eeprom_is_ready(). Replace `eeprom_is_ready()` with `!(NVMCTRL.STATUS & NVMCTRL_EEBUSY_bm)` and the library should work, assuming they do not make use of direct register writes that would require hand-porting to other families of AVR.

### **How to use it**
The EEPROM library is provided as part of all board packages for hardware with this sort of memory - this particular version is supplied with megaTinyCore. It will be used whenver you are compiling for

```Arduino
#include <EEPROM.h>

void setup(){

}

void loop(){

}

```

The library provides a global variable named `EEPROM`, you use this variable to access the library functions. The methods provided in the EEPROM class are listed below.

You can view all the examples [here](examples/).

### **Library functions**

#### **`EEPROM.read( address )`** [[_example_]](examples/eeprom_read/eeprom_read.ino)

This function allows you to read a single byte of data from the eeprom.
Its only parameter is an `int` which should be set to the address you wish to read.

The function returns an `unsigned char` containing the value read.

#### **`EEPROM.write( address, value )`** [[_example_]](examples/eeprom_write/eeprom_write.ino)

The `write()` method allows you to write a single byte of data to the EEPROM.
Two parameters are needed. The first is an `int` containing the address that is to be written, and the second is a the data to be written (`unsigned char`).

This function does not return any value.

#### **`EEPROM.update( address, value )`** [[_example_]](examples/eeprom_update/eeprom_update.ino)

This function is similar to `EEPROM.write()` however this method will only write data if the cell contents pointed to by `address` is different to `value`. This method can help prevent unnecessary wear on the EEPROM cells.

This function does not return any value.

#### **`EEPROM.get( address, object )`** [[_example_]](examples/eeprom_get/eeprom_get.ino)

This function will retrieve any object from the EEPROM.
Two parameters are needed to call this function. The first is an `int` containing the address that is to be written, and the second is the object you would like to read.

This function returns a reference to the `object` passed in. It does not need to be used and is only returned for conveience.

#### **`EEPROM.put( address, object )`** [[_example_]](examples/eeprom_put/eeprom_put.ino)

This function will write any object to the EEPROM.
Two parameters are needed to call this function. The first is an `int` containing the address that is to be written, and the second is the object you would like to write.

This function uses the _update_ method to write its data, and therefore only rewrites changed cells.

This function returns a reference to the `object` passed in. It does not need to be used and is only returned for conveience.

#### **Subscript operator: `EEPROM[address]`** [[_example_]](examples/eeprom_crc/eeprom_crc.ino)

This operator allows using the identifier `EEPROM` like an array.
EEPROM cells can be read _and_ **_written_** directly using this method.

This operator returns a reference to the EEPROM cell.

```c++
unsigned char val;

//Read first EEPROM cell.
val = EEPROM[ 0 ];

//Write first EEPROM cell.
EEPROM[ 0 ] = val;

//Compare contents
if( val == EEPROM[ 0 ] ){
  //Do something...
}
```

#### **`EEPROM.length()`**

This function returns an `unsigned int` containing the number of cells in the EEPROM.

---

### **Advanced features**

This library uses a component based approach to provide its functionality. This means you can also use these components to design a customized approach. Two background classes are available for use: `EERef` & `EEPtr`.

#### **`EERef` class**

This object references an EEPROM cell.
Its purpose is to mimic a typical byte of RAM, however its storage is the EEPROM.
This class has an overhead of two bytes, similar to storing a pointer to an EEPROM cell.

```C++
EERef ref = EEPROM[ 10 ]; //Create a reference to 11th cell.

ref = 4; //write to EEPROM cell.

unsigned char val = ref; //Read referenced cell.
```

#### **`EEPtr` class**

This object is a bidirectional pointer to EEPROM cells represented by `EERef` objects.
Just like a normal pointer type, this type can be dereferenced and repositioned using
increment/decrement operators.

```C++
EEPtr ptr = 10; //Create a pointer to 11th cell.

*ptr = 4; //dereference and write to EEPROM cell.

unsigned char val = *ptr; //dereference and read.

ptr++; //Move to next EEPROM cell.
```

#### **`EEPROM.begin()`**

This function returns an `EEPtr` pointing to the first cell in the EEPROM.
This is useful for STL objects, custom iteration and C++11 style ranged for loops.

#### **`EEPROM.end()`**

This function returns an `EEPtr` pointing at the location after the last EEPROM cell.
Used with `begin()` to provide custom iteration.

**Note:** The `EEPtr` returned is invalid as it is out of range. Infact the hardware causes wrapping of the address (overflow) and `EEPROM.end()` actually references the first EEPROM cell.
