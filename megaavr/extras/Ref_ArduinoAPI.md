# Details of API function implementation and similar.
In many cases the types of return values and input values, and the details of which argument values are accepted, is not clearly defined by the Arduino API reference. This is unfortunate at one level. On the other hand, frequently these ambiguities are handled in a particularly perverse way by the standard Arduino implementations; were they explicitly stated, we would have less of an excuse to implement them in a more coherent and efficient manner - but they're not, so we have the wiggle room to implement them right,

* Digital pin numbers are always represented as an 8-bit value that is not higher than 127. A `uint8_t` is used internally, but since only values under 128 are used, `int8_t` is equivalent. *The Arduino API does not specify a type, and standard implementation uses an int, which is less efficient*
  * Values above 127 are used internally by DxCore for analog channels, since the numeric values of analog channels overlap with those of digital pins. They should not be used by user code, and specifying them is not valid for digital functions on DxCore or megaTinyCore (this is handled differently on ATTinyCore, but that is beyond the scope of this document).
  * `PIN_Pxn` where x is a port letter, and n the number of a pin within that port is defined all pins that exist. Pin0 of ports where. *The Arduino API provides no facility for referencing pins by port and bit. This makes interpreting non-Arduino resources, such as the datasheets, more difficult*
  * `Px` where x is a port letter is defined for all ports. The numeric values start at 0 for PA and count upwards. This is returned by `digitalPinToPort()` and accepted by a number of macros. *standard but undocumented*
  * `NOT_A_PIN` is a special value returned by the "pin information macros" (out term). It takes a value of 255 (`uint8_t`) or -1 (`int8_t`). *This is standard Arduino behavior which is not documented except in the source code*
    * Always compare with `NOT_A_PIN`, never the numeric values. There is never a time when the numeric values are superior, and several ways in which they can behave in surprising ways - 255 converted to an int16 is 255, 255 converted to an int8 is -1. And if you're comparing int16's, those ain't the same.
    * `NOT_A_PORT`, `NOT_AN_INTERRUPT` and `NOT_A_MUX` exist for the same purpose. They have the same value - they exist just to make it more obvious to a reader of the code what is meant. If a new system is added which requires a placeholder for invalid values of a different sort, more of these may be added. *not all of these exist in the standard core; NOT_A_MUX is definitely not in standard cores*
* All three of the digitalI/O functions take `uint8_t` arguments, and digitalRead returns a uint8_t that is either 1 or 0. HIGH (1), LOW (0), CHANGE(2), INPUT (1), OUTPUT (0), and INPUT_PULLUP (2) are simply #defined as appropriate values. *the official classic AVR cores take `int` arguments and return `int` values. The official modern AVR core uses enums to define those values. That had a benefit in that it prevented people from using numbers. But obviously it broke TONS of code. So they put in a compatibility layer that let you pass numbers in. **thus destroying the only benefit of the system**. We have no plans to drink from that bowl of koolaid.*
  * **With the exception of HIGH and LOW, we do not guarantee that the numeric values will never change. You should always use the names in your code, not numeric values**. (INPUT_PULLUP in particular may be changed to 3 at some point, and an OUTPUT_PULLUP (2) added. By changing the value like that, pinMode could accommodate the new option with essentially no overhead; since bit 0 would indicate direction, and bit 1 would indicate the pullup state. OUTPUT_PULLUP would be useful for open-drain configurations. I have never seen code that relied upon the numeric values of pinMode arguments. *The official API does not say anything about the values of these constants*
  * **The case of HIGH and LOW is special**; there is an absolutely massive amount of code online that assumes that LOW = 0 = false and HIGH = 1 = true, and that you can do things like 'if (digitalRead(buttonPin)) ...' to test if a a pin is high. This core will never change the values of those constants.
  * Do not forget that `4 = CHANGE` - while using 0 and 1 in calls to digitalWrite is acceptable, if poor practice, passing any other number to it is not a good idea. 4 will be treated as `CHANGE` while any other number will be treated as `HIGH` - which is likely not what you expected. *the official core does not use CHANGE in this way*
* The other constants for API functions are also #defined *they are enums on the official core*

```c
#define LOW              0    /* used for digitalWrite(), digitalRead(), openDrain() and attachInterrupt() */
#define HIGH             1    /* used for digitalWrite(), digitalRead(). There is no option for HIGH level interrupt provided by the hardware */
#define FLOATING         HIGH /* Not in official API - used for openDrain, while making clear what you're doing */
#define CHANGE           4    /* used for attachInterrupt() */
#define FALLING          2    /* used for attachInterrupt() */
#define RISING           3    /* used for attachInterrupt() */
#define INPUT            0    /* used for pinMode() */
#define OUTPUT           1    /* used for pinMode() */
#define INPUT_PULLUP     2    /* used for pinMode() */
#define LSBFIRST         0    /* used for shiftIn/shiftOut */
#define MSBFIRST         1    /* used for shiftIn/shiftOut */
```
  * Note that the actual values of the input sense configuration bitfield dont match the constants used above; this is accounted for in WInterrupts.c

## Compatibility implications
Obviously, when storing values of the arguments for these functions, you need to do things differently if they're enums. But how do you do that without breaking for megaTinyCore or DxCore or classic AVR cores? There is a simple test: **To make code compatible with cores that use enums and cores that use defines, test `#if defined(HIGH)`** If it's defined, the constants are just macros, and if it's not you must be working with something that uses enums.

## Datatypes
* `byte`, `word`, and `boolean` are Arduino names for `uint8_t`, `uint16_t`, and `bool`. We provide these typedefs, but recommend against their use. You may have noticed in nearly all code and examples, I almost never use `int`, always `int16_t` - I In fact, I try to avoid naming any type by the names that do not explicitly specify it's size, to guarantee that there will be no surprises.
