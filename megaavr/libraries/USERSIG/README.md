# USERSIG Library V2.0 for megaTinyCore

Written by: _Christopher Andrews_

Adapted for the USERROW/USER SIGNATURE for tinyAVR 0/1/2-series and megaAVR 0-series by: _Spence Konde_

## What is the USERSIG library

The USERSIG library provides an easy to use interface to interact with the so-called "user row" or "user signature" - on tinyAVR 0/1/2-series and megaAVR 0-series parts, this works just like EEPROM - except that it is not cleared by a chip erase, regardless of whether the EESAVE bit is set in the fuses. This library is named USERSIG because USERROW is the name of a builtin way to access it included in the io headers (low level files containing register names and the like), but which provides no facility for writing or doing other things that you probably want to do with them

## A word about datatypes used
These are known by many names; `uint8_t`, `byte`, and `unsigned char` - they are all interchangible.

Addresses in the USERSIG area are given by by a `byte`, not an `int`, as all released parts have under 256 bytes of memory of this type. The library offsets it as appropriate for your part to get the memory mapped address.

## How to use it
The USERSIG library is included with megaTinyCore. To add its functionality to your sketch you'll need to reference the library header file. You do this by adding an include directive to the top of your sketch.

```Arduino
#include <USERSIG.h>

void setup(){

}

void loop(){

}

```

The library provides a global variable named `USERSIG`, you use this variable to access the library functions. The methods provided in the USERSIG class are listed below.

You can view all the examples [here](examples/).

## Library functions

### `USERSIG.read( address )` [[_example_]](examples/usersig_read/usersig_read.ino)

This function allows you to read a single byte of data from the USERSIG.
Its only parameter is an `byte` which should be set to the address you wish to read.

The function returns an `unsigned char` containing the value read.

### `USERSIG.write( address, value )` [[_example_]](examples/usersig_write/usersig_write.ino)

The `write()` method allows you to write a single byte of data to the USERSIG.
Two parameters are needed. The first is a `byte` containing the address that is to be written, and the second is the data to be written, a `byte`.

This function does not return any value.

### `USERSIG.update( address, value )` [[_example_]](examples/usersig_update/usersig_update.ino)

This function is similar to `USERSIG.write()` however this method will only write data if the cell contents pointed to by `address` is different to `value`. This method can help prevent unnecessary wear on the USERSIG cells.

This function does not return any value.

### `USERSIG.get( address, object )` [[_example_]](examples/usersig_get/usersig_get.ino)

This function will retrieve any object from the USERSIG.
Two parameters are needed to call this function. The first is a `byte` containing the address that is to be written, and the second is the object you would like to read.

This function returns a reference to the `object` passed in. It does not need to be used and is only returned for conveience.

### `USERSIG.put( address, object )` [[_example_]](examples/usersig_put/usersig_put.ino)

This function will write any object to the USERSIG.
Two parameters are needed to call this function. The first is a `byte` containing the address that is to be written, and the second is the object you would like to write.

This function uses the _update_ method to write its data, and therefore only rewrites changed cells.

This function returns a reference to the `object` passed in. It does not need to be used and is only returned for conveience.

### **Subscript operator:** `USERSIG[address]` [[_example_]](examples/usersig_crc/usersig_crc.ino)

This operator allows using the identifier `USERSIG` like an array.
USERSIG cells can be read _and_ **_written_** directly using this method.

This operator returns a reference to the USERSIG cell.

```c++
byte val;

//Read first USERSIG cell.
val = USERSIG[ 0 ];

//Write first USERSIG cell.
USERSIG[ 0 ] = val;

//Compare contents
if( val == USERSIG[ 0 ] ){
  //Do something...
}
```

### `USERSIG.length()`

This function returns a `byte` containing the number of bytes in the user signature space.

---

## Advanced features

This library uses a component based approach to provide its functionality. This means you can also use these components to design a customized approach. Two background classes are available for use: `USRef` & `USPtr`.

### `USRef` class

This object references an USERSIG cell.
Its purpose is to mimic a typical byte of RAM, however its storage is the USERSIG.
This class has an overhead of two bytes, similar to storing a pointer to an USERSIG cell.

```C++
USRef ref = USERSIG[ 10 ]; //Create a reference to 11th cell.

ref = 4; //write to USERSIG cell.

unsigned char val = ref; //Read referenced cell.
```

### `USPtr` class

This object is a bidirectional pointer to USERSIG cells represented by `USRef` objects.
Just like a normal pointer type, this type can be dereferenced and repositioned using
increment/decrement operators.

```C++
USPtr ptr = 10; //Create a pointer to 11th cell.

*ptr = 4; //dereference and write to USERSIG cell.

unsigned char val = *ptr; //dereference and read.

ptr++; //Move to next USERSIG cell.
```

#### `USERSIG.begin()`

This function returns an `USPtr` pointing to the first cell in the USERSIG.
This is useful for STL objects, custom iteration and C++11 style ranged for loops.

#### `USERSIG.end()`

This function returns an `USPtr` pointing at the location after the last USERSIG cell.
Used with `begin()` to provide custom iteration.

**Note:** The `USPtr` returned is invalid as it is out of range. In fact the hardware causes wrapping of the address (overflow) and `USERSIG.end()` actually references the first USERSIG cell.
