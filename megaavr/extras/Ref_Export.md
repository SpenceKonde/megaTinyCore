# Assembly Listing Generation
When you do Sketch -> Export Compiled Binary with megaTinyCore, in addition to the usual .hex file, two additional files are generated, a .lst, and a .map, these are the assembly listing and memory map. Both of them are useful for reducing the binary size of a sketch, by showing you where the flash (or RAM) is being used.

## The filenames
The exported files will be found in the sketch folder. The file names will look something like this:
`sketch_name.ino.t3224.20c0.mB1.wO.pfF.aMan.v250.lst`
1. The name of the sketch file, including the .ino.
2. a '.' followed by a `t` and the processor part number.
3. a '.' followed by a number, the letter `c` and a 0, or 2. The first number is the clock speed in MHz, the digit after the cs is the clock source, 0 = internal clock, 1 = crystal, and 2 = external clock.
4. a '.' followed by a `m` and then either "NONE" or A, B or D. If not "NONE, this will be followed with a number. This specifies what timer was set as the millis timer when it was compiled."
5. a '.' followed by a `w` and indicates the Wire library mode: O = master OR slave, A = master AND slave
6. Finally:
* If the different printf implementation is chosen from the tools menu, .pfF or .pfM will be next, these are for the full and minimal implementations, and use different amounts of flash.
* If you have chosen a non default option for attachInterrupt, `.aOld` or `.aMan` will indicate that.
7. Ends with a '.' followed by a`v` followed by the current version of the core with no separators, then the file extension.

## The memory map
The map is a blunt instrument - and unfortunately doesn't have as much granularity as you'd like (it will often just show that main() is taking up most of the space. Okay, great, so init(), setup() and loop() and stuff called from them are taking up most of the flash. Tell me something I don't know - the reason all your functions end up being counted in main()) is that, because they're called in only one place, they get "inlined" to save space and improve execution time (calling a function has overhead - sometimes considerable overhead - it starts at 3-4 clocks (depending on the part) for the call, 4 for the return - but the "rules" for how different registers are treated mean that it can involve up to 36 clock cycles of shuffling registers around in the worst case, and 48 bytes of flash. Inlining is wonderful, but it makes the memory map less useful.

The other thing that makes the memory map less useful is the absolutely abysmal formatting it uses.
Despite my distaste for python and utter lack of experience with that language, I wrote a script to reformat them, because I couldn't read them either, it is [described here](https://github.com/SpenceKonde/AVR_Research/tree/main/maps_and_listings)

### Understanding the memory map
Most of this is pretty straightforward, just be reminded that
* The most significant bit in an address usually means "in the other address space" whichever one that is. Over UPDI, writing 0x001000 writes to a peripheral register. In a memory map 0x001000 is part of flash, and 0x801000 . (above script will separate stuff in RAM and Flash)
  * Except when it's not - updi programming treats 0x000000-0x010000, as ram/register/and 0x800000 - 0xFFFFFF as dflash

## The listing
The listing file is a *gigantic* wall of nonsense if you don't know assembly. It tries to intermix C source and assembly, which in theory should make it a lot easier to follow. It does that, sort of - but unfortunately, it t isn't very good at it. Sometimes, for reasons entirely unclear to me, it will often fail entirely to include the C code (acting like it has no access to the source) for functions that are part of the core if building for optiboot - what's strange is that the same code compiled for non-optiboot gets a correctly generated listing.

Assuming it is actually intermixing the two, it doesn't always manage to match them up perfectly - instead of showing the source immediately preceding the assembly, sometimes it will get out of sync and show the source after the assembly, or even before the previous block of assembly.... And occasionally lines that do not generate any instructions will show up in a place where they don't belong. In spite of all that, it is an incredibly useful tool, even if you don't know AVR assembly. You can search for setup or loop to find those functions and scroll through the wall of assembly and chopped up comments. Since most of you all don't know AVR assembly, there are a few "rules" that will help use it for the specific purpose of shrinking sketches or finding bloat.

Let's consider this random chunk of assembly from a listing I had open
```text
C:\Users\Spence\Documents\Arduino\hardware\DxCore\megaavr\cores\dxcore/wiring_digital.c:221
    }
  #endif
  _pinMode(pin, mode);
}
void _pinMode(uint8_t pin, uint8_t mode) {
  uint8_t bit_mask = digitalPinToBitMask(pin);
     a5a: 99 32         cpi r25, 0x29 ; 41
     a5c: a8 f4         brcc  .+42      ; 0xa88 <UartClass::begin(unsigned long, unsigned int)+0x104>
```
### How do I even start to read this?
* The instructions are always formatted as 6 spaces followed by several hexadecimal digits and a colon, 2 or 4 bytes in hex, this time with spaces between each byte. *Everything else is not an instruction, and isn't taking up space*
  * If you use a text editor that lets you "find all" with a regular expression, and which has a "minimap" view, you can highlight all the instructions in the minimap by searching something like `^ {6}[a-f0-9]+:( [a-f0-9])+.*` and doing find all, so you can quickly see any big blocks of instructions to examine more closely.
* The first digits are the address of that instruction. They are in order, and you'll notice the numbers only go up by 2 each time - the addresses are in bytes, and each instruction is a 2-byte word.
* Those next 2 (or 4) bytes are the actual opcode - most instructions are 2 bytes (1 word), but a few are two words long.
* After the opcode is the instruction name and arguments, in the same format they would be in if you were writing assembly. Instructions have 0, 1, or 2 arguments. If it has 2 arguments, they will be separated by a comma. The first instruction shown above demonstrates that.
* This is followed by a semicolon (that marks the start of a comment in assembly). Sometimes there is additional information; this may or may not be relevant or useful.
  * In the first instruction above, on the cpi line we see that one argument is in hexadecimal (indicated by the 0x), and then there's a number after the semicolon. That is simply the value of that argument converted to base ten. Sometimes this is useful, sometimes the numbers in hex are more useful.
  * The "comment" is more useful for a branch, jump, or call - anything that causes, or could cause, code execution to continue from some different part of the file. In this case, there are two parts of the comment, first, the address that it leads to (0xa88 here). Then, in <>'s is the name of that function and the number of bytes after the entrypoint of that function to which this instruction might call or jump/branch to (a "branch" jumps a very short distance - the limit is 64 words in either direction which only occurs if some test is true or false). Which when it can do it correctly, is very useful. However, it doesn't understand that AVR has two address spaces, RAM and flash, so symbols in the RAM/special function register address space can end up here instead of something useful.
* When you're just using the listing to track down the bloat, you likely have some idea where it is down to the granularity of map, and you are surprised that the function in question is as bloated as is. You can search for the function name, and then start scrolling down, following along with the comments until you find some code that you were expecting would be small generating outsize portions of the executable, or conditionally compiled code whose condition you didn't think you met.

### Following the execution path
<!-- markdownlint-disable -->
You will quickly notice that assembly is non-linear it will often jump off to somewhere distant, for no immediately obvious reason. Other than an interrupt, there are only 3 things that can cause the next instruction executed to be anything other than the following instruction.
* The argument is in decimal, not hexadecimal, the comment will contain the address of the target in hexadecimal.
* Jump - the `jmp` and much more commonly, `rjmp` instructions move execution to a different address. `rjmp` is smaller and faster, but can only jump a limited distance. `jmp` can jump to anywhere in the flash - this is a 4-byte instruction, and the two second bytes are the address it's jumping to.
  * Addresses are word addresses in the opcodes, but byte addressed in the assembly (and the comment) after them will convert it to a byte address.
  * `jmp`'s third and fourth byte are the address i's jumping to. On the small number of AVR parts with flash larger than 128k bytes (64k words, 2^16), some of the bits of the first two bytes of jump are used `*`, since this can reach the whole address space.
  * `rjmp` will have an argument of `.+number` or `.-number`, which represents the position as measured from the the *next* instruction. so `.+42` means that that the next instruction it executes (if it branches) will have an address 42+2 higher than the current instruction.
  * That means that `rjmp .-2` is an infinite loop with no test. It is normal for avr-gcc to emit one of these in every program; if `main()` were ever to return, this is where it would go (C was designed for systems where it was meaningful for a program to "exit", which is not the case on embedded systems)
  * That also means that `rjmp .+0` does nothing. This is very common in timing critical hand-written code: it's a 2-clock-cycle `nop`(no operation), and never seen in compiler generated code (if it is, it's because some assembly macro is being used)
  * Sometimes this is done to locations far more distant than necessary; if there has to be an rjmp, as far as the compiler is concerned, there's no difference between rjmping 2000 words versus 20. A human reading an assembly listing, of course, will feel differently.
* Call - like a function call, when the `call` `rcall` and `icall` are reached, execution jumps to a different part of the code, but it pushes the current address onto the stack. At the end of code reached by any sort of call instruction, you expect to find a `ret` which instructs it to return. Functions which, in C, don't have a return statement still have a `ret` in the generated assembly - assuming they are implemented with calls rather than jumps. Function arguments are passed in even numbered registers between r8 and r25 (with the second byte of a 16 bit value taking the odd byte) unless the arguments are so large that they don't fit (in which case they are passed in memory and the generated code has the means to fish the values out of memory). After a `ret` the return value comes back similarly. Additionally, the called code is permitted to change r18 through r27, r30, and r31; before a call you may find the code pushing values onto the stack - that's why: it has local variables that won't fit in the "call saved" registers and which it needs to use later. One would not expect it to be using any values that are call used after any sort of call, except for the specific register(s) holding the return value (and the compiler will not generate such code)
* Instructions starting in "br" are conditional branch instruction.
  * They have a very short range, and are always relative to the current location, using the same convention as `rjmp`.
  * All branch statements branch based on the status register, which is usually set by the previous instruction (not always though - especially in hand-written assembly. In the example above, brcc is "branch if carry is cleared". So how did carry get cleared? One line up there's a "cpi" - compare immediate. For an AVR, comparison is the same thing as subtraction - except that it throws away the result instead of storing it; It just keeps the status flags.
  * Usually there's no explicit `cp` or `cpi` before a conditional branch - arithmetic and logical operations almost always set one or more bit in the status register, so the comparison is usually already done without an explicit compare. By far the most common conditions are breq ("branch if equal" which will trigger if the result of a math or logic operation is 0, and yes the naming there does see odd, but see the previous point - comparison is ), brne (if result is not 0), brcc (branch if carry cleared - means the last instruction didn't result in a borrow or carry. brcs means that it did. Everything else is much rarer, and some are almost unheardof)
* The only instructions that jump to distant places in the code are jmp, rjmp, and ijmp, and call, rcall, and icall - the jumps which simply "jump" to another address and continue from there, and the calls, which store the current address (onto the stack); the 'ret' instruction appears at the end of a function and returns to that address. jmp and call are 4-byte instructions; the last bytes hold the destination. Both are *word* addressed. The rjmp/rcall versions are relative, and will have a .+ or .- argument. These will also have obj-dump's translation of the address, and attempt at find a landmark to orient you. ijmp/icall use the address from the Z pointer register - it doesn't matter much when you're just trying to find where space is going, but they make the code a lot harder to follow along with.
* There is only one other kind of flow control instruction - the skipif - there are two sets of them. As the name implies, under certain conditions, they will skip the next instruction.
  * `sbis`/`sbic` skip if bit in I/O space is is set/clear
  * The "I/O space" is the lowest 32 addresses in he data space (eg, memory addresses 0x0000 through 0x001F).
    * On modern AVRs it contains the VPORTs in 0x0000 through 0x1B, and the 4 GPIORs (which have no hardware defined function, other than being in the low I/O space and not used for anything else).
    * Any place where you do a test like `if (digialReadFast())` (equiv. `if (VPORTx.IN & (1 << n)`) it is *virtually guaranteed* to manifest in the generated assembly as an `sbic` or `sbis` (the compiler is free to choose whichever will let it make what it thinks is better code).
    * Classic AVRs did not have a fixed pattern to what was in the low I/O space (other than the port registers).
  * `sbrs`/`sbrc` skip if bit in [working] register is set/clear
    * These are r0-r31 - the values that the CPU currently has at it's fingertips. Or whatever CPUs have instead of fingertips.
    * Unlike the above case of testing the special function registers in the low I/O space, while `if (foo & (1 << bar))` could be compiled to a variety of different things, particularly if there is no need to leave `foo` unchanged
  * These are very frequently followed by a jump or call.
* The compiler is allowed to reorder operations as it sees fit, as long as it does not change the order of interactions with `volatile` variables (special function registers are always declared that way).
  * If it is jumping to or calling a loop, it will often land several instructions after the "start" of the loop, or equivalently, the end of the loop will jump back to before the start, which is where the counter will be incremented or decremented.
  * Sometimes, small changes will result in major changes in how the code is ordered. Other times, comparatively large changes will barely alter the output.
* There is an indirect version of jump, and of call - ijmp and icall. They jump or call the address in the Z register. Since you have the source mixed with the assembly, you usually know what it has called (and it's often a method of a class).
  * It's not uncommon to see it use ld X+ to load an address into the Y register which it then uss with ldd to load r30 and r31 and then ijmp or icall that. This is extremely disorienting without the intermixed C source.
* For everything except ijmp and icall, wherever it's jumping to can be quickly found by searching the file.
  * It is shown in hex after the `;` - in the above example, searching for ` a88:` will find only the target of the branch unless there is some particularly perverse C comment. Notice that there is a space at the beginning of that search string, so it won't match `1a88:` - if you have a regex-search capable text editor, `^ +a88:` should catch only the target of that branch.
  * You cannot follow assembly backwards. The place where a jump or branch lands *is not marked*.
    * Someone with time on their hands could write a script to annotate assembly listings, find all the places that jumps, calls, and branches land and mark the end of that line; except for `ijmp` and `icall`.
  * If one function is only ever called before another one but there are calls to the second function without the first, the first might be located immediately before the second, with no ret instruction, so it just keeps executing the next function when the first function is called
* While not often encountered in files exported by Arduino, if you ever find yourself looking at a series of instructions with no source (perhaps you don't have the source) and you see a stretch of nonsensical instructions, remember to consider the possibility that they aren't instructions at all, but rather constant data (which is normally not included in assembly listings - when the disassembler knows where the non-instructions are )
 <!-- markdownlint-restore -->

### Working register?
If you're unfamiliar with the concept of "working registers", the above doesn't entirely make sense.

Unfortunately, the term "register" is used to refer to two different types of 1-byte locations in memory. The more common usage in non-assembly context is what is sometimes called a "special function register" - the ones that generally have an ALL_CAPS name and control some peripheral. Also called "peripheral registers", these are not what is being referred to when in assembly-land - "register" in assembly context generally refers to one of the 32 "working registers" that the CPU has direct access to. The CPU can only perform calculations using values that are in a working register. Register contents are loaded from RAM or flash with `ld`, `ldd`, and `lds`, and the contents stored to RAM with `st`, `std` or `sts`. Most of the time the values stored in a register correspond to a variable, but the compiler can `**` keep an intermediate value around if it will need it.

Not all working registers are equal:
* The "immediate" instructions only work on "upper registers" - r16-r31
* The `movw` (move word) instruction only works on even numbered registers, and acts on the register, and the one after it (ie, `movw r0, r30` copies r30 and r31 to r0 and r1).
* The first two registers have special functionality with a few instructions
  * Multiplication instructions that generate a 16-bit result put it in r0 and r1.
  * On most classic AVRs, r0 and r1 have special meaning when self-programming the flash (but they don't on modern/AVRxt parts).
  * On ancient classic AVRs, the flash could only be read into r0, not an arbitrary register.
* r24, r25 is considered a "special upper register pair" but all it gets to do is use the ADIW/SBIW (add/subtract immediate from word - single instruction to add or subtract small integer values from a word; most frequently used on the pointer registers.
* r26, r27 form the X register, which can load and store, and automatically predecrement and postincrement while doing so. It cannot do load or store with displacement.
* r28, r29 form the Y register, which can do everything X can, but also load and store *with displacement*... making it extremely powerful for working with objects and data structures. It is usually pointed at the current stack frame.
* r30, r31 form the Z register, which can do everything Y can, but can also be used with the LPM instruction to read from flash, the SPM instruction to *write* flash (on some but not all parts), and it's where the the address the ijmp/icall jumps to or calls.
* r1 is the "zero reg". It always contains zero except while executing inline assembly (which is not bound by this rule as long as it clears it before it's done). It is a very common ideom when performing math involving different types to do the math with the low byte(s) and then ensure carrying happens by performing the same operation, or the same operation only with carry between the high bytes and r1. Adding a 1-byte variable to a 4-byte one will generate an add instruction on the low byte, followed by three add-with-carry-s:
```text
add r18,r24
adc r19,r1
adc r20,r1
adc r21,r1
```


`*` The instruction set specification permits 22-bit flash addresses (word addressed), and jmp can access the whole thing. Nothing like that has ever been released, and while the instruction set describes the existence of kludges analogous to RAMPZ for flash and LPM, it is not clear that such a device was ever made; if it was, it was in the xMega line, and it would have had to have had an external memory interface - no 8-bit AVR has ever existed with more than 32k of or 384k of flash, and non-xMega parts have never passed 256K of flash (ATmega2560), or 16k of ram (AVR Dx-series and ATmega1284p)

`**` "can" - just because the compiler can legally reuse an intermediate value and make more efficient code, doesn't mean that it will. Usually if it doesn't see it, trying to tweak the code to give it a hint won't change anything - You see this in particular when there is a size-speed tradeoff. The compiler is asked to optimize for size so that's what it does, essentially without regard for runtime. This is almost always the correct tradeoff, but it's not always the correct tradeoff. Try to bitshift compile a sketch that calls `micros()` for a speed (in MHz) that isn't a power of two (the math falls out for that case) and isn't three or five times a power of (they were rewritten in assembly because of this), and find the point where it has to perform the division on `ticks`. Division is slow, and for some reason, people notice how long `micros()` takes to return. It's as if they're timing it to the microsecond or something. Using actual division there would increase the time it took to return by 600-or-so clock cycles, in a function that otherwise totals under 100, so that's a non-starter. One can approximate division with a combination of rightshifts and addition and subtraction - though you need to use a lot of terms in some cases to prevent discontinuities in micros() which can break time-dependant code. But rightshifting even a 2 bit value several times takes 5 clocks per bit shift - and the compiler does not, and cannot be convinced to reuse the intermediate values, and hand written assembly was used to do just that for the most commonly used non-power-of-two speeds.
