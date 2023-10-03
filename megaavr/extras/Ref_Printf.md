# printf() method, and "printf() in general" - considerations for correct use
There are a whole bunch of standard c functions with names that are some variation on "printf()" (fprintf, sprintf, snprintf, vprintf, etc). If you have desktop C experience, you've likely used them there, and you may have played with it on Arduino too. However, "stuff involving format strings" is also a notorious source of strange bugs in software, including no small number that were exploitable to great impact - and that's on desktop PC's, where the type weirdness doesn't become quite so visible, as you'll see below.

## We have the printf() method
Unlike the official board packages, but like many third party board packages, megaTinyCore includes the `printf()` method for the Print class. Print is the parent class of Stream, and either Print or Stream is the parent class of - uh - just about every class you see with a print() method, including HardwareSerial and TwoWire.

```cpp
Serial.printf("Milliseconds since start: %ld\n", millis());
```

Pretty straightforward if you know how to use printf() - see the below cplusplus.com link for a decent review of it. I managed to write a printf that didn't crash the sketch and printed what it was supposed to (I don't particularly care for format strings in any language. I find the syntax ugly, obtuse, and that I write a lot more bugs when I try to use them).

## The hazards of printf() and it's gang of ne'er-do-wells
There a variety of pitfalls when using printf. You *must* be sure to avoid them - they're deep pits too, some with spikes at the bottom. Security vulnerabilities caused by format-string bugs on desktop PCs and servers are one of the most common causes of security vulnerabilities. Most of us are less concerned about security, since our devices are typically not on a public facing network. When they are, hopefully they are behind a firewall.

### First - simple thing: printing to strings (sprintf/snprintf)
When you use sprintf(), there is no checking of any sort to prevent it from running off the end of the buffer, and causing undefined but almost always catastrophic behavior. snprintf() also allows you to specify the maximum size of the result that it writes to the buffer. Use snprintf() in preference to sprintf(), and make sure you don't specify too large of an n for the buffer.

### Formatting specifiers have modifiers, and you probably need to use them
Specifically, there is a length modifier that specifies the size of the data being formatted. They must match the actual data passed ***and this is not checked*** reliably, and when it is checked, it is only a warning).

The consequences of forgetting the format specifiers are severe: Not only is the output corrupted, the state of printf() internally is broken, and subsequent calls may also malfunction. It can also corrupt memory and cause undefined behavior through other means.

See the table of ones that I expect will work below (it was cribbed from [cplusplus.com/reference/cstdio/printf/](https://www.cplusplus.com/reference/cstdio/printf/), and then I chopped off all the rows that aren't applicable, which is most of them). Apparently many people are not fully aware (or at all aware) of how important this is - even when they think they know how to use printf(), and may have done so on previously (on a desktop OS, with 32-bit ints and no reason to use smaller datatypes for simple stuff).
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

There are reports of memory corruption with printf suspect it is misunderstandign of above that is actually at hand here.

### General notes about DxC/mTC and printf()'y things
No matter how you come to printing with a format string, it's gonna end up pulling in all the same bloat. Most of the `[1-2 letters]printf()` functions are little more than wrappers around various ways of supplying input to using the output from one master function.


#### There are several levels of printf support available
Which one is right for you depends on the needs of your applications and your available flash. A Tools submenu lets you choose from three levels of `printf()`. **Warning:** Enabling either of the non-default modes will force the compiler to include the other version of printf *even if it is not used*. If left at the default, and none of the printf's are used, printf is not included. Hence, you should be sure to leave this at the default if not using printf.

The options are:
Full `printf()`: This handles  the default one that drops float support to save 1k of flash, and the minimal one drops almost everything and for another 450 bytes flash saving (will be a big deal on the 16k and 8k parts. Less so on 128k ones). Note that selecting any non-default option here *will cause it to be included in the binary even if it's never called* - and if it's never called, it normally wouldn't be included. So an empty sketch will take more space with minimal `printf()` selected than with the default, while a sketch that uses `printf()` will take less space with minimal `printf()` vs default.
