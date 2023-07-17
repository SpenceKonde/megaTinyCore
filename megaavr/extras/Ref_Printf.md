Unlike the official board packages, but like many third party board packages, megaTinyCore includes the `printf()` method for the printable class (used for UART serial ports and most everything else with `print()` methods); this works like `sprintf()`, except that it outputs to the device in question; for example:
```cpp
Serial.printf("Milliseconds since start: %ld\n", millis());
```
Note that using this method will pull in just as much bloat as `sprintf()` and is subject to the same limitations as printf - by default, floating point values aren't printed. You can use this with all serial ports
You can choose to have a full `printf()` implementation from a Tools submenu if you want to print floating point numbers, at a cost of some additional flash.

#### **WARNING** `printf()` and Variants Thereof Have Many Pitfalls
There are a considerable number of ways to screw up with `printf()`. Some of the recent issues that have come up:
* Formatting specifiers have modifiers that they must be paired with depending on the datatype being printed, for all except one type. See the table of ones that I expect will work below (it was cribbed from [cplusplus.com/reference/cstdio/printf/](https://www.cplusplus.com/reference/cstdio/printf/), and then I chopped off all the rows that aren't applicable, which is most of them). Apparently many people are not fully aware (or at all aware) of how important this is - even when they think they know how to use printf(), and may have done so on previously (on a desktop OS, with 32-bit ints and no reason to use smaller datatypes for simple stuff).
* There are (as of 1.4.0) warnings enabled for format specifiers that don't match the the arguments, but you should not rely on them. Double check what you pass to `printf()` - `printf()` bugs are a common cause of software bugs in the real world. Be aware that *while you can use F() on the format string, there are no warnings for invalid format strings in that case*; a conservative programmer would first make the app work without F() around the format string, and only switch to F() once the format string was known working.

From cplusplus.com:
>The length sub-specifier modifies the length of the data type. This is a chart showing the types used to interpret the corresponding arguments with and without length specifier ~(if a different type is used, the proper type promotion or conversion is performed, if allowed)~:
Strikethrough mine 'cause that don't work here (and it's not my fault nor under my control - it's supplied with avrlibc, and I suspect that it's because the overhead of implementing it on an 8-bit AVR is too large). When incorrect length specifiers are given (including none when one should be used) surprising things happen. It looks to me like all the arguments get smushed together into a group of bytes. Then it reads the format string, and when it gets to a format specifier for an N byte datatype, it grabs N bytes from the argument array, formats them and prints them to whatever you're printing to, proceeding until the end of the format string. Thus, failing to match the format specifiers' length modifiers with the arguments will result in printing wrong data, for that substitution **and all subsequent ones** in that call to `printf()`.

The table below comprises the relevant lines from that table - many standard types are not a thing in Arduino.

| length | d i | u o x X | f F e E g G a A |  c  |    s   |  p   |    n     |
|--------|-----|---------|-----------------|-----|--------|------|----------|
| (none) |int16|  uint16 | float           | int |  char* |void* | int*     |
| hh     |int8 |  uint8  |                 |     |        |      | char*    |
| l      |int32|  uint32 |                 |     |        |      | int32_t* |

Notice that there is no line for 64 bit types in the table above; these are not supported (support for 64-bit types is pretty spotty, which is not surprising. Variables of that size are hard to work with on an 8-bit microcontroller with just 32 working registers). This applies to all versions of `printf()` - the capability is not supplied by avr-libc.

There are reports of memory corruption with printf, I suspect it is misunderstandign of above that is actually at hand here.

#### Selectable `printf()` Implementation
A Tools submenu lets you choose from three levels of `printf()`: full `printf()` with all features, the default one that drops float support to save 1k of flash, and the minimal one drops almost everything and for another 450 bytes flash saving (will be a big deal on the 16k and 8k parts. Less so on 128k ones). Note that selecting any non-default option here *will cause it to be included in the binary even if it's never called* - and if it's never called, it normally wouldn't be included. So an empty sketch will take more space with minimal `printf()` selected than with the default, while a sketch that uses `printf()` will take less space with minimal `printf()` vs default.
