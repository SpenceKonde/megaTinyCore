# dirty_tricks.h - a package of macros that act as a forceful shove to the optimizer
These are intended primarily for internal use.
**For experienced users with at least basic AVR assembly knowledge only**

Others are likely to find only woe and misery here. Those who know assembly, on the other hand experience woe and misery continually, and sometimes the woe and misery found here is less woeful or miserable than the usual, and these might be useful in that case.

Everyone always cares about code size, and everyone occasionally cares about performance

There are a few of us who work on the core (me, and 1, maybe two others) who know assembly pretty well, and look at the listings. The avr-gcc compiler sometimes, is absolutely AWFUL. It's not that it's just "not well optimized". It's that it does actively perverse things. It's one thing to be like "Oh, well, yeah it probably could have figured out that this was never going to carry and saved an instruction there", it's quite another to have blocks of adiw r26, n; st X, rwhatever; sbiw r26, n; back to back (yes, with sbiw's and adiw's targeting the same register executing consecutively), or clear a register immediately prior to something that replaces it's value with something else. We're talking about cases where an instruction nullifies the instruction immediately before it. The virtual instructions generated in the early passes, when they become real instructions in later ones, sometimes get rendered terribly. Other times not! But sometimes.... sometimes it's just horrible.

The ramifications of C type promotion and integer literals defaulting to 16-bit signed ints come together to cause math to be done with larger datatypes than you intend, and frequently it is nearly impossible to talk it out of this.

There is no C syntax that is consistently optimized intelligently for << 8 operations and larger shifts, and frequntly, the optimization choices made with -Os result in not *no* consideration being given to performance.

So this is a combination of simply pathological code where the n'th instruction negates the n-1-th instruction, poor renditions of the C type promotion rules that make things really inefficient, and -Os is too willing to pick incredibly slow implementations

These shortcomings impact the core, as they sometimes hamstring performance and can bloat binaries. And so dirty tricks using inline assembler were devised which give a sufficiently forceful shove to the register allocator that they will be guaranteed to be correct.

Note that some of these, like makePtr(), setLow/setHigh/clrHigh/clrLow/etc can all be achieved using unions entirely within the C framework, and the compiler usually does a much better job optimizing them.

Others, particularly the tricks to force things into the Y/Z register for displacement - those can only be forced with tricks like this. The compiler can always waste space to undo your hard work here, so these should not be used unless you're keeping an eye on the compiler output. These are designed mainly for use by the core, but they are available and documented here and reassess their use if you see it forced to add unneeded or unexpected instructions.

## **Many of these Macros are EVIL**
All are "use at own risk" and misuse may cause undefined behavior, up to and including making demons come out of your nostrils. They were all created to serve core development as their main end, not user code. You may find them useful. Or you may just make a mess; it is your responsibility to understand whether you know enough to be using these sort of macros. If you can do it with type unions, do that instead, certainly.

A word about the conventions as far as the "dirty" cases are concerned. They do not apply to the ones marked `(not dirty)`
* These are mostly lower case and short, in order to make them easier to type, unlike the usual macros-are-uppercase convention. This is not something that we would do in a real public facing API. This is an internal API that we are just documenting for the sake of completenes, and to aid the very rare few users who will benefit from them.
* Like macros, any types can be passed to these without any errors.
* Unlike most macros, only very specific types can be used without things going off the rails.
* These have no guardrails at all.

The following "pseudotypes" are used below to define the argument types in this document. These are NOT typechecked, but MUST BE OBSERVED, otherwise the behavior you see will be drastically different than you expect.
```text
uint8_t - an 8-bit value, which
variable uint8_t - An 8-bit value, which must be a local variable.
constant uint8_t - An 8-bit value, which must be a COMPILE TIME KNOWN CONSTANT
16_bit_t - a 16-bit datatype (int, unsigned int, or any pointer to anything). Most of the macros change one of these in place. Regardless of what the datatype is, it's cast to a uint16_t for as it is fed into the inline assembly. *It should be a local variable*
```

## Non-dirty-tricks kept in the same file
All global macros that get turned into asm and aren't defined in what's left of the Arduino API files.

These are described in more detail in the relevant Reference documents.

### Cyclecounting delays (not dirty)
* `_NOP()`
* `_NOP2()`
* `_NOPNOP()`
* `_NOP8()`
* `_NOP14()`

In lower case (but less compatible. Not dirty)
* `_nop()`
* `_nop2()`
* `_nop8()`
* `_nop14()`

### Swapping of Nybbles (not dirty)
* `_SWAP(n)`
* `_swap(n)`

## Dirty dirty dirty
These are the dirty tricks that we warned you about.

### High and Low math
```c
      _addLow(16_bit_t a, variable uint8_t b);
      _subLow(16_bit_t a, variable uint8_t b);
     _addHigh(16_bit_t a, variable uint8_t b);
     _subHigh(16_bit_t a, variable uint8_t b);
 _addLowConst(16_bit_t a, constant uint8_t b);
 _subLowConst(16_bit_t a, constant uint8_t b);
_addHighConst(16_bit_t a, constant uint8_t b);
_subHighConst(16_bit_t a, constant uint8_t b);
```
These add or subtract the 8-bit value b from the high or low byte of a **without** taking any measures to handle carrying, borrowing, overflow, etc.
These are best used when you have a base pointer that you need to take an offset from - likely the base pointer a is pointing to the start of a peripheral, and the offset is the location of a register within that peripheral. The peripherals are all aligned on 16, 32, 64, or 128 byte bounds and have at most that many registers. There may be times when you have 256-byte aligned pointers, too. When you want to offset them and either you need an offset of more than 63, or you know that you're already out of pointer pairs that can do displacement (and you're not working with a straight up constant. If the address can be constant-folded, and you are only using it once, this doesn't help and actually makes it worse). Typically you do this when you'd ideally like to use displacement - but you can't because you know both Y and Z are used, and the next pointer will be put into X.

If you're forced to do multiple offsets, eg
```c
*(bptr + 10) = 0x10;
*(bptr + 15) = 0x44;
*bptr = 1; // common idiom for configuring then starting an enable locked peripheral.
```
If this would use the displacement stores, then it would take 3 words and 3 clocks.

But you look at the generated assembly and see that Y and Z are already in use and getting even heavier use, so X is used instead, and the above is rendered as adiw, st, sbiw, adiw, st, sbiw, st.  (11 clocks, and 7 words).

```c
addLowConst(bptr, 10);
*bptr = 0x10;
addLowConst(bptr, 5);
*bptr = 0x44;
subLowConst(bptr, 15);
*bptr = 1;
```
This instead comes out to 6 clocks and 6 words.

### dirty tricks which can often but not always be replicated with clever use of C type unions
These pertain to setting the high or low bit to a constant value, setting it to a value already in a variable
```c
      _setHigh(16_bit_t a, variable uint8_t b);
 _setHighConst(16_bit_t a, constant uint8_t b);
      _clrHigh(16_bit_t a);
       _setLow(16_bit_t a, variable uint8_t b);
  _setLowConst(16_bit_t a, constant uint8_t b);
       _clrLow(16_bit_t a);
```

This sets the high or low byte (as requested) to a specified variable or constant. The change is made in place, and the other byte is unchanged.
Most useful when working with pointers. "set" and "setconst" set the specified byte to a non-zero value, "clr" version sets the specified byte to 0.

### dirty tricks to force a variable into a pointer
GCC fails to optimize memory accesses on AVR as well as it ought to.
When accessing memory, GCC is using STS/LDS, using 2 words per instruction and 2/3 clocks respectively.In some cases, the programmer knows in advance that the function will access multiple peripheral registers or a bigger struct. (<= +63) To convince GCC that loading the pointer in advance will be more efficient, this defines are provided. The inline assembly forces GCC. To use the instruction STD/LDD that are just one word big and need only 1/2 clocks respectively, allowing for smaller code and faster execution. It should be considered, that the address has to be loaded into the register first, thus adding 2 words and 2 clocks overhead if it isn't already there. It is not required that the second argument be a variable rather than a constant.
```c
_fastPtr_z(16_bit_t __localVar__, 16_bit_t __pointer__);
_fastPtr_y(16_bit_t __localVar__, 16_bit_t __pointer__);
_fastPtr_x(16_bit_t __localVar__, 16_bit_t __pointer__);
_fastPtr_d(16_bit_t __localVar__, 16_bit_t __pointer__); // Z or Y - the displacement capable ones
_fastPtr(16_bit_t __localVar__, 16_bit_t __pointer__); // any pointer register pair, so only direct access, access with postincrement or with predecrement will always work.
```

#### Using these
* declare local pointer variable with the same type as the original pointer
* use one of the defines, depending on how you need to access it, likely `_fastPtr_d();`
* Use the pointer normally; because it's in Y or Z it will use the access with displacement.
```c
ADC_t* pADC;
_fastPtr_d(pADC, &ADC0);
pADC->CTRLB = 0x55;
```

### Building pointers from bytes
These two macros give you a pointer (in either Z or Y, or in X, Y or Z).
```c
_makeFastPtr_d(uint8_t * newptt, const uint8_t highbyte, uint8_t lowbyte);// Z or Y - the displacement capable ones
  _makeFastPtr(uint8_t * newptt, const uint8_t highbyte, uint8_t lowbyte);// any pointer register pair. Can almost always be done with a union.
```
These take three arguments: A local variable that must be a pointer, a constant value that is to be the high byte and a 8-bit variable which is to be the low byte.
They  provide NO GUARANTEE that the compiler won't waste even more time shuffling them out of the register you forced them into, but the compiler is rarely that boneheaded. It does dumb stuff, yes, but if you've forced variables into certain registers, it's unlikely to move them unless it has to, in which case the compilers first attempt may not have been as bad as you thought.
