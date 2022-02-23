# USERSIG Library V2.0.2 for megaTinyCore

**Written by:** _Spence Konde_
**Based on EEPROM.h by:** Christopher Andrews

## What is the USERSIG library

The USERSIG library provides an easy to use interface to interact with the so-called "user row" or "user signature" - on tinyAVR 0/1/2-series and megaAVR 0-series parts, this works just like EEPROM - except that it is not cleared by a chip erase, regardless of whether the EESAVE bit is set in the fuses. This memory section is most often called the "USERROW" since that is what Microchip calls it. This library is named USERSIG (from "User Signature Space", the non-abbreviated official name and the "USER_SIGNATURE_SIZE" macros) because USERROW is the name of a builtin way to access it included in the io headers (low level files containing register names and the like), but which provides no facility for writing or doing other things that you probably want to do with them. (How did it become "User Row" from "User Signature"? Probably the same way that the read-only signature that all AVR parts have came to be called the "SIGROW" - I don't know what that way is, but it's consistent).

Addresses in the USERSIG area are given by by a `byte`, not an `int`, as all released parts have under 256 bytes of memory of this type. The library offsets it as appropriate for your part to get the memory mapped address.

## USEROW Sizes

| AVR Device                          | USERROW size | Supported | Erase granularity and mode |
|-------------------------------------|--------------|-----------|----------------------------|
| tinyAVR 0/1/2-series                |          32b |       Yes | Byte, handled automatically|
| megaAVR 0-series (8k or 16k flash)  |          32b |       Yes | Byte, handled automatically|
| megaAVR 0-series (32k or 48k flash) |          64b |       Yes | Byte, handled automatically|
| DA, DB, DD-series (all flash sizes) |          32b | On DxCore | 32b, app must handle erase |
| EA-series (all flash sizes)         |          64b |   Not yet | TBD                        |

The Dx-series uses a different version of the library, which is included with DxCore. It presents almost the same API, but not quite - this one is functionally identical to EEPROM, except that all functions that modify values have a return value of 1, and a few no-op functions that always return 0 are provided for code compatibility, and it has a way to manually erase the USERROW (also for compatibility). Those parts have hardware which cannot be made to look like EEPROM in the same way. See the library documentation for DxCore for more informnation, or the section at below for just what you need to know to make sure your code works on DxCore too..

Specifying an address beyond the size of the USEROW will wrap around to the beginning.

## Write Endurance
There is no specification given in the datasheet. You should treat the USERROW as if had a limit of 10k write/erase cycle, like flash; that has been confirmed to me in the context of the Dx-series parts by a well-placed source - however, I do not know if the same is true on tinyAVR, or if (like it's erase and write proceedures) it is instead like EEPROM.

## The last 12 bytes of the USERROW
If (and only if) you have used the included tuning sketch to calibrate the internal oscillator for other frequencies, we store that calibration data in the USERROW, at the last 12 addresses. The last 6 get the values used with the internal oscillator at nominally 20 MHz, and the next-to-last 6 get the values for when the internal oscillator is set for a 16 MHz nominal frequency: Hence you have only 20 bytes available. If you are using the "tuned" oscillator options, you should avoid overwriting the last 12 bytes - or at least the one for the frequency you are tuning for (this includes the case where the tuning sketch has never been run, and you're using the tuned internal oscillator option and relying on us to guess the correct calibration value - we'll mistake your value for the calibration value); if you do, your new value will be used as the calibration value, and at the next reset, you'll wonder why your "24 MHz" tuning has turned into 17.3 MHz or some other wacky frequency.

Those locations are only used when Tools -> Clock is set to "Internal xx MHz (tuned)" - other clock options do not require such measures.

## How to use it
The USERSIG library is included with megaTinyCore. To add its functionality to your sketch you'll need to reference the library header file. You do this by adding an include directive to the top of your sketch.

```Arduino
#include <USERSIG.h>

void setup() {

}

void loop() {

}

```

The library provides a global variable named `USERSIG`, you use this variable to access the library functions. The methods provided in the USERSIG class are listed below.

You can view all the examples [here](examples/).

## Library functions

### `USERSIG.read(address)` [[_example_]](examples/usersig_read/usersig_read.ino)

This function allows you to read a single byte of data from the USERSIG.
Its only parameter is an `byte` which should be set to the address you wish to read.

The function returns an `unsigned char` containing the value read.

### `USERSIG.write(address, value)` [[_example_]](examples/usersig_write/usersig_write.ino)

The `write()` method allows you to write a single byte of data to the USERSIG.
Two parameters are needed. The first is a `byte` containing the address that is to be written, and the second is the data to be written, a `byte`.

This function always returns an 1 as an `int8_t`, indicating that a single byte was written to the NVM. (on DxCore, it can also return a 0, indicating that the data has not yet been written, and that flush() must be called to write it).

### `USERSIG.update(address, value)` [[_example_]](examples/usersig_update/usersig_update.ino)

This function is similar to `USERSIG.write()` however this method will only write data if the cell contents pointed to by `address` is different to `value`. This method can help prevent unnecessary wear on the USERSIG cells.

This function always returns 1 as an `int8_t`, indicating that a single byte was either written to the NVM, or was verified to not need to be written to the NVM (on DxCore, it can also return a 0, indicating that the data has not yet been written, and that flush() must be called to write it).

### `USERSIG.get(address, object)` [[_example_]](examples/usersig_get/usersig_get.ino)

This function will retrieve any object from the USERSIG.
Two parameters are needed to call this function. The first is a `byte` containing the address that is to be written, and the second is the object you would like to read.

This function returns a reference to the `object` passed in. It does not need to be used and is only returned for conveience.

### `USERSIG.put(address, object)` [[_example_]](examples/usersig_put/usersig_put.ino)

This function will write any object to the USERSIG.
Two parameters are needed to call this function. The first is a `byte` containing the address that is to be written, and the second is the object you would like to write.

This function uses the _update_ method to write its data, and therefore only rewrites changed cells.

This function returns a reference to the `object` passed in. It does not need to be used and is only returned for conveience.

This should be obvious, but don't use `USERSIG.put()` to store something that won't fit in the USERROW. Addresses wrap around, and what you saved will come out looking very different.

### **Subscript operator:** `USERSIG[address]` [[_example_]](examples/usersig_crc/usersig_crc.ino)

This operator allows using the identifier `USERSIG` like an array.
USERSIG cells can be read _and_ **_written_** directly using this method.

This operator returns a reference to the USERSIG cell.

```c++
byte val;

// Read first USERSIG cell.
val = USERSIG[0];

// Write first USERSIG cell.
USERSIG[0] = val;

// Compare contents
if (val == USERSIG[0]) {
  // Do something...
}
```

### `USERSIG.length()`

This function returns a `byte` containing the number of bytes in the user signature space.


### `USERSIG.flush()`
By analogy with the stream flush() method, commits any pending writes to the NVM. This function does nothing on megaTinyCore, as the library always immediately commits all changes to the NVM because there is no reason not to (unlike DxCore).

It always returns 0 as a `int8_t`, indicating success. On DxCore it can also return negative numbers; those indicate errors.

### `USERSIG.pending()`
Returns 1 if there is data waiting to be written to the USERROW. This is never the case on megaTinyCore, but that is not the case on DxCore due to the different hardware implementation.

It always returns 0 as a `int8_t` indicating no writes are pending. On DxCore it can also return 1, indicating that there is unwritten data, and flush() must be called to write it.


### `USERSIG.erase()`
Writes 0xFF to every cell of the USERROW.

It always returns 0 as a `int8_t` indicating success. On DxCore it can also return negative numbers indicating errors.


---
## Compatibility with DxCore

DxCore provides a version of this library which accomodates it's more constrained NVM controller. Specifically, we have byte granularity with erase here. Additionally, the USERROW is EEPROM-like, not flash-like, and is thus expected to have very high write endurance. On the Dx-series, the USERROW can only be erased one page at a time. The size of the USERROW on those parts is exactly 1 page. Like both flash and EEPROM, freshly erased memory stores all 1's. To deal with this, the DxCore USERSIG library stores a copy of the USERROW in RAM. When USERROW writes occur, we check if `(new value) & (old value) == (new value)`. If so, we update the buffer in RAM, and write the single requested byte.  If not, only the buffer is updated. write() or update returns a 0, and now a write is "pending". No writes to the actual USERROW ever occur while there is a pending write, and further writes will also return 0 accordingly. Calling flush() is required (after you're done making changes) is required to save it; this will write the entire contents of the buffer to the USERROW. Afterthat is done, write() will again return 1's as long as it can be written without an erase. USERSIG.pending() will return 1 if there is a pending write, and put() will automatically call flush if and only if the object written is larger that 4 bytes, and hence is an object of some sort, rather than a simple type.

To ensure code compatibility with DxCore, there are two very simple things your code must do. This is associated with negligible overhead and there's no good reason not to do it:
1. You must always call flush(); after writing a series of values, unless the last write is performed by passing an object 5 bytes or more in size to USERSIG.put() (in which case it is unnecessary, but harmless).
2. You must not attempt to write to the USERROW from within an interrupt (the usefulness of this was felt to be too small to justify the overhead needed to deal with this - writing to NVM from within an interrupt is inadvisable in general and we recommend against it in EEPROM.h too, where it causes millis to lose time).

---

## Advanced features

This library uses a component based approach to provide its functionality. This means you can also use these components to design a customized approach. Two background classes are available for use: `USRef` & `USPtr`.

### `USRef` class

This object references an USERSIG cell.
Its purpose is to mimic a typical byte of RAM, however its storage is the USERSIG.
This class has an overhead of two bytes, similar to storing a pointer to an USERSIG cell.

```C++
USRef ref = USERSIG[10]; // Create a reference to 11th cell.

ref = 4; // write to USERSIG cell.

unsigned char val = ref; // Read referenced cell.
```

### `USPtr` class

This object is a bidirectional pointer to USERSIG cells represented by `USRef` objects.
Just like a normal pointer type, this type can be dereferenced and repositioned using
increment/decrement operators.

```C++
USPtr ptr = 10; // Create a pointer to 11th cell.

*ptr = 4; // dereference and write to USERSIG cell.

unsigned char val = *ptr; // dereference and read.

ptr++; // Move to next USERSIG cell.
```

#### `USERSIG.begin()`

This function returns an `USPtr` pointing to the first cell in the USERSIG.
This is useful for STL objects, custom iteration and C++11 style ranged for loops.

#### `USERSIG.end()`

This function returns an `USPtr` pointing at the location after the last USERSIG cell.
Used with `begin()` to provide custom iteration.

**Note:** The `USPtr` returned is invalid as it is out of range. In fact the hardware causes wrapping of the address (overflow) and `USERSIG.end()` actually references the first USERSIG cell.

## A general warning
This library and the USERROW functionality have not been as thoroughly vetted for gremlins as EEPROM. Unlike EEPROM, we don't know how many skeletons are in this ~closet~ memory section. Accordingly, before you go shipping a product that relies on this, be sure to test it under adverse scenarios if it is expected to perform reliably in them (for example, low voltage, power interruption during write, etc). Be particularly careful of things that are known to cause issues with EEPROM.


## Version History
* 2.0.2 - Make it possible to write code compatible applications for DxCore and megaTinyCore by providing stubs of the Dx-specific USERROW functions
* 2.0.1 - Correct out of bounds addreses to wrap instead of doing weirder things.
