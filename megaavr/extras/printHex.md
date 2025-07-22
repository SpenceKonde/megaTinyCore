# Serial.printHex()
This core includes my printHex() methods of Serial - these provide an expedient, flash-efficient method print numbers as hexadecimal. Unlike `Serial.print(number,HEX)` these will automatically add the leading zero (if the number is under 16). When passed a 2 or 4 byte value, a third, optional argument allows reversing the byte order, allowing you swap "endian-ness" of a value. They also do this with minimal overhead, which is important on tinyAVR parts. This is purely a convenience function, but unless you call it, it doesn't take up any flash, and when debugging direct interactions with hardware registers, it's a pretty big convenience. Yes, I realize there may be better ways to do this, and certainly more graceful ones - but this one *is* small. fast, and achieves a function that one needs to do constantly, and which rendered the traditional way, is bloated (as it supports different radixes - which is a cool feature, except that probably >90% of use of printing in non-base ten is hex, and most of the remainder is base 2. Both of which are special cases.


printHex(x)   | 1366 bytes | Prints 2 charachters
print(x,2)    | 1418 bytes | Prints 1 to 8 characters
print(x,16)   | 1434 bytes | Prints 1 or 2 characters
print(x)      | 1492 bytes | Prints 1 to 3 charachters


```c++
void Serial.printHex(uint8_t b);
void Serial.printHex(int8_t b);
```
These methods simply print the value passed to it as hexadecimal. No leading `0x` or trailing separators or newlines (a future version may add a printHexln() method). These are particularly useful for printing the values of 8-bit registers, and it was for this purpose that they were added to the core. Because the leading zero is printed where appropriate, multiple bytes can be printed consecutively and the value will remain intelligible.
```c++
void Serial.printHex(uint16_t w, bool swaporder = 0);
void Serial.printHex(int16_t w, bool swaporder = 0);
void Serial.printHex(uint32_t l, bool swaporder = 0);
void Serial.printHex(int32_t l, bool swaporder = 0);
```

These methods take a 2 or 4 byte variable and do the same thing. Depending on how the data is stored, you may or may not wish to reverse the endian-ness for display (humans like to put the most significant digits first, machines prefer the opposite); these default to printing the low byte first if no second argument is provided.

`uint8_t * Serial.printHex(uint8_t* p,uint8_t len, char sep=0);`

This takes a pointer to one or (usually) more bytes. Specify the length of this array or data structure, and - if desired - a separator to place between each byte. Print arrays, consecutive bytes in the data or register space (for example, the whole block associated with a peripheral). This returns a pointer to the next byte after the end of the ones you printed.

`uint16_t * Serial.printHex(uint16_t* p, uint8_t len, char sep=0, bool swaporder=0);`

As above - only this takes a pointer to a 2-byte value, and there is an optional argument to to reverse the byte order, as above.
