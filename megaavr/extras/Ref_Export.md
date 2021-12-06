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
The map is a blunt instrument - and unfortunately doesn't have as much granularity as you'd like (it will often just show that main() is taking up most of the space. Okay, great, so init(), setup() and loop() and stuff called from them are taking up most of the flash. Tell me something I don't know - the reason all your functions end up being counted in mai()) is that, because they're called in only one place, they get "inlined" to save space and improve execution time (calling a function has overhead - sometimes considerable overhead - it starts at 3-4 clocks (depending on the part) for the call, 4 for the return - but the "rules" for how different registers are treated mean that it can involve up to 36 clock cycles of shuffling registers around in the worst case, and 48 bytes of flash. Inlining is wonderful, but it makes the memory map less useful.

The other thing that makes the memory map less useful is the absolutely abysmal formatting it uses.
Despite my distaste for python and utter lack of experience with that language, I wrote a script to reformat them, because I couldn't read them either, it is [described here](https://github.com/SpenceKonde/AVR_Research/tree/main/maps_and_listings)

### Understanding the memory map
Most of this is pretty straightforward, just be reminded that
* The most significant bit in an address usually means "in the other address space" whichever one that is. Over UPDI, writing 0x001000 writes to a peripheral register. In a memory map 0x001000 is part of flash, and 0x801000 .  (above script will separate stuff in RAM and Flash)
  * Except when it's not - updi programming treats 0x000000-0x010000, as ram/register/and 0x800000 - 0xFFFFFF as dflash

## The listing
The listing file is a _gigantic_ wall of nonsense if you don't know assembly. It tries to intermix C source and assembly, but it isn't very good at it, and currently flubs it entirely for functions defined by the core when building for Optiboot boards for some reason. When it does intermix the two, it doesn't always match the source up with the assembly perfectly.

But it does include the sketch source more or less correctly, and - you can search for setup or loop to find those functions and scroll through the wall of assembly and chopped up comments. Since most of you all don't know AVR assembly, there are a few "rules" that will help use it for the specific purpose of shrinking sketches, even without knowing assembly. But mostly, you're just going to be looking at the volume of code that you can link to a given chunk of C code. Which is what you're here for, right?

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
* The instructions are always formatted as 6 spaces followed by several hexadecimal digits and a colon, 2 or 4 bytes in hex, this time with spaces between each byte. Only the last two lines in there are instructions.
  * The first digits are the address of that instruction. They are in order, and you'll notice the numbers only go up by 2 each time - the addresses are in bytes, and each instruction is a 2-byte word.
* The next 2 (or 4) bytes are the actual opcode - most instructions are 2 bytes, but a few include an address in the third and fourth byte - followed by the instruction name and arguments, in the same format they would be in if you were writing assembly.
* You'll notice that at the end of the brcc line, there's some additional information after the semicolon. This will be present every time there is any branch, jump, or call - anything that causes, or could cause, code execution to continue from some different part of the file. The first part is the address that it leads to (0xa88 here). Then, in <>'s  is the name of that function, which is sometimes a useful value, but other times is a random constant with a similar numeric value.
* After the semicolon, for instructions that are not jumps/etc, it will translate any hex values in the arguments into decimal.
* When you're just using the listing to track down the bloat, you likely have some idea where it is down to the granularity of map, and you are surprised that the function in question is as bloated as is. You can search for the function name, and then start scrolling down, following along with the comments until you find some code that you were  expecting would be small generating outsize portions of the executable, or conditionally compiled code whose condition you didn't think you met.

### Following the execution path
* You will quickly notice that assembly is non-linear - it will often jump off to somewhere distant, for no immediately obvious reason. Sometimes this is done for a good reason, other times, it's just being stupid. The compiler is at once very clever and thoroughly brain-dead, and incredibly stubborn.
  * Because of that, one can follow code forwards, though it might involve skipping around a lot. You cannot follow assembly backwards. The place where a jump or branch lands is not marked.
  * If one function is only ever called before another one but there are calls to the second function without the first, the first might be located immediately before the second, with no ret instruction, so it just keeps executing the next function when the first function is called
* Instructions starting in "br" are conditional branch instructions.
  * Instructions which cause execution to jump to a different part of the file usually have an argument of .+number or .-number, which represents the position as measured from the the *next* instruction. so .+42 means that that the next instruction it executes (if it branches) will have an address 42+2 higher than the current instruction.
  * The argument is in decimal, not hexadecimal.
  * All conditional branch statements branch based on the status register, which is usually set by the previous instruction (not always though - especially in hand-written assembly. In the example above, brcc is "branch if carry is cleared" how did carry get cleared? One line up there's a "cpi" - compare immediate. And comparison in AVR assembly, is the same thing as subtraction, except that it throws away the result and just keeps the status flags. Common conditions are breq ("branch if equal" which will trigger if the result of a math or logic operation is 0, and yes the naming there is odd), brne (if result is not 0), brcc (branch if carry cleared - means the last instruction didn't result in a borrow or carry. brcs means that it did. Everything else is much rarer. Several of the instructions are almost unheardof)
* Other instructions that jump to far different parts of the code are jmp, rjmp, and ijmp, and call, rcall, and icall - the jumps which simply "jump" to another address and continue from there, and the calls, which store the current address (onto the stack); the 'ret' instruction appears at the end of a function and returns to that address. jmp and call are 4-byte instructions; the last bytes hold the destination. Both are *word* addressed. The rjmp/rcall versions are relative, and will have a .+ or .- argument. These will also have obj-dump's translation of the address, and attempt at find a landmark to orient you. ijmp/icall use the address from the Z pointer register - it doesn't matter much when you're just trying to find where space is going, but they make the code a lot harder to follow along with.
* There is only one other kind of flow control instruction - the skipif - there are two sets of them: sbrs/sbrc, and sbis/sbic - skip if bit in [working] register is set/clear, and skip if bit in I/O space is is set/clear - the I/O space is used to access the states of pins through VPORT, but not through PORT (digitalReadFast uses VPORT but digitalRead does not - an `if (digitalRead(pin))` will be an ld or lds to load the port state, and or andi to check the bit in question, and branch with breq or brne; `if (digitalReadFast(pin))` will generally compile to sbic/sbis (there's more in the I/O space on classic AVRs - and fewer patterns to it). The working registers store the variables that the CPU has immediate access to (they're those things numbered r0-r31 that you've noticed by now), not to be confused with peripheral registers, which are what is normally meant by "register" - I try to always specify "working register", many sources do not and expect that the context will be recognized. The skip-ifs will skip the next instruction if their condition is met; it is very common to see them followed by jumps or rjmps either to make a "branch" out of them. In fact, a branch instruction that branches ahead 2 bytes (1 instruction) on some condition skipping over a jump is very common too - branch has very short range, 64 instructions in either direction, while rjmp can go 2048 in either direction.

### Working register?
* Speaking for registers... As noted above, there are 32 of them, and all variables that the CPU is "working" with need to be in them, as those are the only locations that can be acted on directly by most instructions. Most of the time the values stored in a register correspond to a variable, but the compiler is also free to store an intermediate value (ie, if you use an expression that calculates some value, and the operands are not marked volatile, and then use it again a couple of lines later, it likely won't end up recalculating the value). Data is loaded from SRAM or peripheral registers into the working registers, and values in the working registers get stored to SRAM for longer term volatile storage.
* They can be divided into upper and lower registers - the upper 16 are better, because only the upper registers can have the "immediate" instructions - things like "load immediate" and "Or with immediate" (there aren't enough possible opcodes in 16 bits to extend the immediates to all 32 registers). The 8 highest registers have more functionality as a pair, and get increasingly special. They highest 6 form the X, Y and Z registers, used for loading from and storing to the data space - st and ld will use one of these to choose where to load or store from/to.
  * R24, R25 is considered a "special upper register pair" but all it gets to do is use the ADIW/SBIW (add/subtract immediate from word - single instruction to add or subtract small integer values from a word; most frequently used on the pointer registers.
  * R26, R27 form the X register, which can load and store, and automatically predecrement and postincrement.
  * R28, R29 form the Y register, which can do everything X can, but also load and store *with displacement*... making it extremely powerful for working with objects and data structures. It is usually pointed at the current stack frame.
  * R30, R31 form the Z register, which can do everything Y can, but can also be used with the LPM instruction to read from flash, the SPM instruction to *write* flash (on some parts, including the Dx-series), and it's where the the address the ijmp/icall  jumps to or calls. (wouldn't it have been nice if they could have put some of those on a different register? Maybe the LPM/SPM, since those don't have displacement either? Or the indirect call/jump address?
  * The working register r1 is the "zero reg". It always contains zero except immediately after a multiplication operation (multiplication result goes in r1 and r0) or while executing inline assembly (which is not bound by this rule as long as it clears it before it's done). It is a very common ideom when performing math involving different types to do the math with the low byte(s) and then ensure carrying happens by performing the same operation, or the same operation only with carry between the high bytes and r1. Adding a 1-byte variable to a 4-byte one will generate an add instruction on the low byte, followed by three adds-with-carry:
  ```text
  add r18,r24
  adc r19,r1
  adc r20,r1
  adc r21,r1
  ```
