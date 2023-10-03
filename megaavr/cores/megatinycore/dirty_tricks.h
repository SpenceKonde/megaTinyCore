/* dirty_tricks.h - Where all the ugly asm macros live.
 * Copyright Spence Konde 2023
 * Part of DxCore and megaTinyCore, which are open source
 * and released under the LGPL 2.1
 * See LICENSE.txt for full legal boilerplate if you must
 */

/* Okay... This was getting fucking absurd.
 * We (well, me and the one, maybe two other people here who speak fluent avr asm) all knew we needed unholy tricks like this.
 * We wish we didn't. But while the GCC part of avr-gcc does a pretty thorough and consistent job optimizing...
 * The AVR part is piss-poor. It's fucking awful. As I understand it, GCC from a big picture, first transforms the code into an idealized
 * version that has standins for registers, but "almost" maps to machine instructions, but is universal and this is where the bulk of the optimization happens
 * This happens in many passes to perform optimization that isn't machine specific (and hence have gotten resources lavished on them by everyone).
 * Then, it has to go from that ideal world to reality, and start naming instructions and assigning registers. This is machine specific.
 * And it is here that AVR hasn't gotten much love. The result is sometimes what might be termed pathological assembly (as in, it is indicative disease).
 * If generated assembly like SBIW r26, 20 immediately followed by ADIW r26, 23 - (or any other constant integers 63 or lower) is not a sign of disease,
 * I don't know what would be. That's when the compiler has generated code that assumes all three pointer registers can do displacement. Only two of them can,
 * because there aren't enough unique instructions that can be expressed in 16 bits to implement ldd/std on all three pointer registers (ldd/std consume
 * a huge number of opcodes. Each pointer worth of opcodes is 1/16th of the total (along with the six immediates plus rcall and rjmp.
 * That is, these 10 instructions account for 62.5% of the opcodes in the avr instructionset! There are 4096 different opcodes for each of them!)
 * and that's why while we have 3 pointer registers, we have only two that are fully functional, and which each have 2048 displaced load and 2048 displaced
 * store opcodes (32 registers * 64 q's from 0-63), making a total of 4096 displaced ld/st's, plus 128 pre/post decremented ones, 4224 in total. The X register,  the runt
 * of the litter, has only ld/st X, ld/st X+ and ld/st -X, a mere 192 opcodes (we could have totally had a 4th pointer, too, in terms of instruction opcodes at least)
 *
 * Unfortunately, both Atmel and Microchip have been asleep at the switch here, and not much has been done to make sure that AVR code that gets output isn't terrible
 * The compiler appears to be
 * - Often blind to when a pointer is being used to access offsets and hence we should be using the y and z registers with ldd and std
 * - Inept at combining bytes into larger data types.
 * - Stupid about isr prologues and epilogs (it looks inside the ISR - and sees what registers need to be saved and restored in order to not corrupt the state of the registers, but it
 * can't tell if tmp_reg, zero_reg, or SREG are changed by the body of the code so it always saves and restores those)
 * - bad at bitshifts with multibyte datatypes.
 *
 * Since like any good programmer, we have some idea of what our code should be compiling to while writing it, there come to be times when we have to look at
 * the assembly listing, and when we do we see the so-called optimizer butchering our code in unspeakable ways, wasting precious flash and excecuing slower than
 * a slug trying to skateboard up a flight of stairs.
 *
 * The offences with speed are much larger in magnitude, though we often care less about this (though occasionally we care more about speed than anyything else)
 * as compared to offenses against flash size, which on embedded systems must always be on our minds.
 *
 * To rectify the problems, assorted snippets of inline asm has been written which are an attempt to slap some sense into the compiler, and these have joined more
 * prosaic assembly macros like the NOP gang.
 *
 * The macros were a fucking pain to type (all caps) and the names were both long and inconsistent.
 *
 * With this update, we are centralizing and documenting all the dirty assembler tricks.
 *
 * I know that lowercase names and macros are sinful. Yes these are macros. Except for the classics (and we have appropriate duplicated of those), they are now lowercase.
 * I know macros are supposed to be all uppercase. But we're prefixing them with an underscore, so you still know there's spooky shit involved, it's just
 * too unpleasant to type and too ugly to read to have all these longass macro names, making a mess of all our core "speed matters" code - and besides that,
 * while macros generally don't care what types they get passed, these totally will cast the arguments to specific types like functions do. Except more permissively -
 * that's an explicit cast, which will eat things that functions won't implicitly cast), but without actually changing the type of anything :-P
 *
 * General Advice:
 * Don't use any of the wacky ones unless you know what the fuck you're doing. This is largely for internal use within the core, because we do, and we know that
 * lots of people who dont and couldn't reproduce this performance enhancement themself if they needed it use the core. So where there's a high impact
 * bit of code - either repeated a lot, and/or in a time-critical section of code that benefits from doing unholy things like this, we're going to do it to make
 * the core better.
 *
 * Many of these hinge upon bullying the compiler into assigning registers less stupidly. These do not at all prevent the register allocator from
 * smacking down the whole thing with a gigantic wad of fail if it thinks it has to (ie, when you're out of pointer registers). These should be used only with one eye
 * on the compiler listings to spot this, because at that point they are usually causing harm instead of good.
 *
 * List of dirty trick macros
 * Not dirty - cycle counting macros:
 * _NOP()
 * _NOP2()
 * _NOPNOP()
 * _NOP8()
 * _NOP14()
 *
 * And in lower case:
 * _nop()
 * _nop2()
 * _nop8()
 * _nop14()
 *
 * Not particularly dirty - AVRs have a single instruction nybble-swap:
 * _SWAP(n)
 * _swap(n)
 *
 * And now all the hideously dirty ones:
 * In all cases, pointer-like 16-bit values should be a local variable, either a pointer or uint16_t.
 *        _addLow(uint16_t a,       uint8_t b)
 *        _subLow(uint16_t a,       uint8_t b)
 *       _addHigh(uint16_t a,       uint8_t b)
 *       _subHigh(uint16_t a,       uint8_t b)
 *   _addLowConst(uint16_t a, const uint8_t b)
 *   _subLowConst(uint16_t a, const uint8_t b)
 *  _addHighConst(uint16_t a, const uint8_t b)
 *  _subHighConst(uint16_t a, const uint8_t b)
 *       _setHigh(uint16_t a,       uint8_t b)
 *  _setHighConst(uint16_t a, const uint8_t b)
 *       _clrHigh(uint16_t a)
 *        _setLow(uint16_t a,       uint8_t b)
 *   _setLowConst(uint16_t a, const uint8_t b)
 *        _clrLow(uint16_t a)
 *     _fastPtr_z(uint16_t __localVar__, uint16_t __pointer__)
 *     _fastPtr_y(uint16_t __localVar__, uint16_t __pointer__)
 *     _fastPtr_x(uint16_t __localVar__, uint16_t __pointer__)
 *     _fastPtr_d(uint16_t __localVar__, uint16_t __pointer__)
 *       _fastPtr(uint16_t __localVar__, uint16_t __pointer__)
 * _makeFastPtr_d(uint8_t * newptt, const uint8_t highbyte, uint8_t lowbyte)
 *   _makeFastPtr(uint8_t * newptt, const uint8_t highbyte, uint8_t lowbyte)
 */




#ifndef dirty_tricks_h
#define dirty_tricks_h

#ifndef _NOP
  #define _NOP()    __asm__ __volatile__ ("nop")
#endif
#ifndef _NOP2
  #define _NOP2()   __asm__ __volatile__ ("rjmp .+0")
#endif
#ifndef _NOPNOP
  #define _NOPNOP() __asm__ __volatile__ ("rjmp .+0")
#endif
#ifndef _NOP8
  #define _NOP8()   __asm__ __volatile__ ("rjmp .+2"  "\n\t"   /* 2 clk jump over next instruction */ \
                                          "ret"       "\n\t"   /* 4 clk return "wha? why here?" */    \
                                          "rcall .-4" "\n\t" ) /* 2 clk "Oh, I see. We jump over a return (2 clock) call it, and then immediately return." */
#endif

/*
Not enabled. Ugly ways to get delays at very small flash cost.
#ifndef _NOP6
  #define _NOP6()   __asm__ __volatile__ ("rcall lonereturn") // 2 bytes of flash.  2+4=6 clk only works if you've got _LONE_RETURN() somewhere. Only guaranteed to work on 8k and smaller parts.
  #define _NOP7()   __asm__ __volatile__ ("call lonereturn")  // 4 bytes of flash.  3+4=7 clk and see above, except that this will only work w/>8k flash.
  #define _LONE_RETURN() __asm__ __volatile__ ("rjmp .+2"    "\n\t"  \  // 4 bytes of flash overhead, but must exist once and only once for NOP6/7 (but not any others) . Don't trip over thr ret. Note that if you're writing inline assembly with ret elsewhere, just proceed
                                               "lonereturn:" "\n\t"  \  // it with a a label and jump to it to save 2 bytes vs this methodMust exist somwehere for
                                                 "ret"         "\n\t" )
  // It could be put into it's own function and marked with the "used" attribute. This allows elimination of the initial rjmp, at the cost of making an ugly hack even uglier.
  // Or even worse, you have other inline assembly, and you just stick the label right before the return!
  // Really, these are things you shoudnt do unless you have your back against the flash/RAM limits and a gun to your head.
  #endif
*/
#ifndef _NOP14
  #define _NOP14()  __asm__ __volatile__ ("rjmp .+2"  "\n\t"   /* same idea as above. */ \
                                          "ret"       "\n\t"   /* Except now it's no longer able to beat the loop if it has a free register. */ \
                                          "rcall .-4" "\n\t"   /* so this is unlikely to be better, ever. */ \
                                          "rcall .-6" "\n\t" )
#endif

/* Beyond this, just use a loop.
 * If you don't need submicrosecond accuracy, just use delayMicroseconds(), which uses very similar methods. See Ref_Timers
 * (eg, ldi (any upper register), n; dec r0; brne .-4)
 * and pad with rjmp or nop if needed to get target delay.
 * Simplest form takes a uint8_t and runs for 3n cycles in 3 words. Padded with `nop` or `rjmp .+0`for 3n + 1 or 3n + 2 if outside the loop, 4n or 5n if padded inside the loop
 * And so on. You will likely end up doing something like
 *
                    #define CLOCKS_PER_US   (F_CPU / 1000000);    // preprocessed away
                    #define DELAYCLOCKS     (0.8 * CLOCKS_PER_US) // say we wanted a 0.8 us delay.
                    uint8_t x = DELAYCLOCKS / 3;                  // preprocessed into a constant
                    __asm__ __volatile__ ("dec %0"      "\n\t"    // before this, an ldi is used to load x into the input operand %0
                                          "brne .-4"    "\n\t"
                      #if (DELAYCLOCKS % 3 == 2)                  // 2 clocks extra needed at end
                                          "rjmp .+0"    "\n\t"
                      #elif (DELAYCLOCKS % 3 == 1)                // 1 clock extra needed at end
                                          "nop"         "\n\t"
                      #endif
                                          : "+d"((uint8_t)(x));
 *
 * The above will take very close to 0.8us under most any conditions.  Notice how all the calculation was moved to the preprocessor.
 *
 *
 * You can extend the length of the iterations by adding nop between the dec and brne, and branching 2 bytes further. that makes it 4 clocks per iteration.
 * You can go for much longer by using 16-bits:
 *                  uint16_t x = 2000;    * overall takes 8 bytrsd
 *                  __asm__ __volatile__ ("sbiw %0,1"    "\n\t"  // Line is preceded by 2 implied LDI's to fill that upper register pair, Much higher chance of having to push and pop.
 *                                        \"brne .-4      \"\n\t\"  // SBIW takes 2 clocks. branch takes 2 clocks unless it doesn't branch, when it only takes one
 *                                        : +w"((uint16_t)(x))  // hence this takes 4N+1 clocks (4 per iteration, except for last one which is only 3, plus 2 for the pair of LDI's)
 *
 */


// The fastest way to swap nybbles
#ifndef _SWAP
  #define _SWAP(n) __asm__ __volatile__ ("swap %0"  "\n\t" :"+r"((uint8_t)(n)))
#endif
/* Lowercase versions, in keeping with the theme that will be used for these macros now, to make the code less hideous and easier to type. */
#define _swap(n) __asm__ __volatile__ ("swap %0"  "\n\t" :"+r"((uint8_t)(n)))
#define _nop()    __asm__ __volatile__ ("nop")
#define _nop2()   __asm__ __volatile__ ("rjmp .+0")
#define _nop8()   __asm__ __volatile__ ("rjmp .+2"  "\n\t"  "ret"  "\n\t"  "rcall .-4" "\n\t")
#define _nop14()  __asm__ __volatile__ ("rjmp .+2"  "\n\t"  "ret"  "\n\t"  "rcall .-4" "\n\t"  "rcall .-6" "\n\t")

/*********************/
/* Low and high Math */
/*********************/
#define       _addLow(a,b) __asm__ __volatile__ ("add  %0A, %1"  "\n\t" :"+r"((uint16_t)(a)):"r"((uint8_t) b))
#define       _subLow(a,b) __asm__ __volatile__ ("sub  %0A, %1"  "\n\t" :"+r"((uint16_t)(a)):"r"((uint8_t) b))
#define      _addHigh(a,b) __asm__ __volatile__ ("add  %0B, %1"  "\n\t" :"+r"((uint16_t)(a)):"r"((uint8_t) b))
#define      _subHigh(a,b) __asm__ __volatile__ ("sub  %0B, %1"  "\n\t" :"+r"((uint16_t)(a)):"r"((uint8_t) b))
#define  _addLowConst(a,b) __asm__ __volatile__ ("subi %0A, %1"  "\n\t" :"+r"((uint16_t)(a)):"M"((uint8_t) (0x00 - ((uint8_t) b))))
#define  _subLowConst(a,b) __asm__ __volatile__ ("subi %0A, %1"  "\n\t" :"+r"((uint16_t)(a)):"M"((uint8_t) b))
#define _addHighConst(a,b) __asm__ __volatile__ ("subi %0B, %1"  "\n\t" :"+r"((uint16_t)(a)):"M"((uint8_t) (0x00 - ((uint8_t) b))))
#define _subHighConst(a,b) __asm__ __volatile__ ("subi %0B, %1"  "\n\t" :"+r"((uint16_t)(a)):"M"((uint8_t) b))

/* _addLow(uint16_t a, uint8_t b) // note that a only needs to be something that can be cast to a 16-bit unsigned value
 * _subLow(uint16_t a, uint8_t b) // It's type is not changed, but it's value is changed, and the math is done as though it were a uint16_t
 * _addHigh(uint16_t a, uint8_t b) // Much less often useful - this is exactly analogous to above, except operating on high byte
 * _subHigh(uint16_t a, uint8_t b) // But you do occasionally need to do it.
 *
 * These perform a subtraction on the low (or high) byte. That's it. They do not make any sort of attempt to handle over/underflow.
 * It saves a clock cycle and an instruction word when you are doing math where are adding or subtracting an 8-bit value to/from a 16-bit one.
 * but you KNOW there will never be a carry or borrow, and thus, you can do the math on the low byte in isolation.
 * Why do you want to do this? Probably because the thing you're adding or subtracting a little to/from is a pointer to a memory address, like
 * the start of an instance of a peripheral and you're adding the offset to it. The peripherals are all aligned on 16, 32, 64, or 128 byte bounds
 * and have at most that many registers. There may be times when you have 256-byte aligned pointers, too. When you want to offset them and either
 * you need an offset of more than 63, or you know that you're already out of pointer pairs that can do displacement (and you're not working
 * with a straight up constant. If the address can be constant-folded, and you are only using it once, this doesn't help and actually makes it worse)
 *
 * In that case, while normally you'd do:
 *
 * uint8_t foo = *(base_pointer + 17);
 *
 * and it would perform an std z+17 or std y+17 depending on whether it's in the z or y pointer.
 * But std requires q to be 0-63 (you may need more), and the base pointer to be y or z (you may know that those two pointers have to already be
 * in use), and that the offset be constant (if it was, say, a number between 0x10 and 0x17 because you're writing to a PORTx.PINnCTRL register)
 * If x and y are unavailable but the offset is constant, then you get a 2-word 4-clock penalty as std becomes adiw, st, sbiw. If the offset isn't
 * constant, it's add adc st sub subc, still 4 clock penalty, but now 4 words too (same if it's a constant larger than 63, except with subi instead
 * of add and sub) This reduces it to 1 clock and 1 word - though you need to remember it's now pointing to a different place, and correct for that
 * next time you use it.
 * (note: If base_pointer is a compile time constant and is used only once it's better to just do the normal way too, because then it comes out as a
 * 2-word 2-clock sts, instead of 2 words to ldi the address then one more to st to it).
 *
 * Finally, be aware that you're still going to get a lot of overhead (though some is not avoidable) if you are already using all three pointers.
 */

/*********************/
/* Low and high Set  */
/*********************/
#define      _setHigh(a,b) __asm__ __volatile__ ("mov %0B,  %1"  "\n\t" :"+r"((uint16_t)(a)):"r"((uint8_t) b))
#define _setHighConst(a,b) __asm__ __volatile__ ("ldi %0B,  %1"  "\n\t" :"+d"((uint16_t)(a)):"M"((uint8_t) b))
#define        _clrHigh(a) __asm__ __volatile__ ("eor %0B, %0B"  "\n\t" :"+r"((uint16_t)(a)))
#define       _setLow(a,b) __asm__ __volatile__ ("mov %0A,  %1"  "\n\t" :"+r"((uint16_t)(a)):"r"((uint8_t) b))
#define  _setLowConst(a,b) __asm__ __volatile__ ("ldi %0A,  %1"  "\n\t" :"+d"((uint16_t)(a)):"M"((uint8_t) b))
#define         _clrLow(a) __asm__ __volatile__ ("eor %0A, %0A"  "\n\t" :"+r"((uint16_t)(a)))
/* _setHigh(uint16_t a, uint8_t b)
 * _setHighConst(uint16_t a, const uint8_t b)
 * _clrHigh(uint16_t a)
 * _setLow(uint16_t a, uint8_t b)
 * _setLowConst(uint16_t a, const uint8_t b)
 * _clrLow(uint16_t a)
 *
 * Note that a can be any 16-bit value (likely a pointer), and it will be changed in place.
 *
 * Constants must use the const version, otherwise you will pay a 1-clock 1-word penalty, and you were trying to avoid that.
 *
 * These allow you to perform surgery on a 16-bit values that already exist, where you wish to change the high or low byte
 * without altering the other byte. Amazingly, the compiler doesn't optimize this like you'd expect it to (likely because
 * of C type promotion rules).
 */

/************/
/* _fastPtr */
/************/
#define _checkType(_varA_, _varB_) if (!__builtin_types_compatible_p(__typeof__(_varA_), __typeof__(_varB_))) { badArg("Non-compatible types"); }
#define _fastPtr_z(_localVar_, _pointer_) _checkType(_localVar_, _pointer_); __asm__ __volatile__("\n\t": "=&z"(_localVar_) : "0"(_pointer_));  // r30:r31
#define _fastPtr_y(_localVar_, _pointer_) _checkType(_localVar_, _pointer_); __asm__ __volatile__("\n\t": "=&y"(_localVar_) : "0"(_pointer_));  // r28:r29
#define _fastPtr_x(_localVar_, _pointer_) _checkType(_localVar_, _pointer_); __asm__ __volatile__("\n\t": "=&x"(_localVar_) : "0"(_pointer_));  // r26:r27
#define _fastPtr_d(_localVar_, _pointer_) _checkType(_localVar_, _pointer_); __asm__ __volatile__("\n\t": "=&b"(_localVar_) : "0"(_pointer_));  // Y or Z
#define   _fastPtr(_localVar_, _pointer_) _checkType(_localVar_, _pointer_); __asm__ __volatile__("\n\t": "=&e"(_localVar_) : "0"(_pointer_));  // X,Y or Z
/* _fastPtr_z(__localVar__, __pointer__)
 * _fastPtr_y(__localVar__, __pointer__)
 * _fastPtr_x(__localVar__, __pointer__)
 * _fastPtr_d(__localVar__, __pointer__)
 *   _fastPtr(__localVar__, __pointer__)
 *
 * GCC fails to optimize memory accesses on AVR.
 * When accessing memory, GCC is using STS/LDS, using 2 words per instruction and 2/3 clocks respectively.
 * In some cases, the programmer knows in advance that the function will access multiple peripheral registers or a bigger struct. (<= +63)
 * To convince GCC that loading the pointer in advance will be more efficient, this defines are provided. The inline assembly forces GCC
 * To use the instruction STD/LDD that are just one word big and need only 1/2 clocks respectively, allowing for smaller code and faster execution.
 * It should be considered, that the address has to be loaded into the register first, thus adding 2 words and 2 clocks overhead. Not to mention the compiler
 * might use the register for something else already. As GCC simulates Displacement on X with adiw/sbiw, it is not recommended to use "E" or "X" for
 * optimizations. They were added for completion.
 *
 * How to use:
 * declare local pointer variable with the same type as the original pointer, e.g. ADC_t* pADC;
 * use one of the defines below. e.g. FORCE_LOAD_POINTER_IN_B(pADC, &ADC0)
 * and voila! all memory accesses to ADC0 are, e.g. Y+xx or Z+xx
 * it is also possible to use a function argument as __localVar__ and __pointer__ (see twi.c slave ISR)
 *
 * How do the instructions work?
 * '=' is a modifier, that signals the compiler that whatever was in the register beforehand will be overwritten. The compiler should save the value, if it will be needed afterwards
 * '&' is a modifier, that makes the compiler add a prequel to the assembly where the value (the one in brackets) is loaded in the specified register
 * 'z' is a constraint that tells the compiler what register to load a value into, in this case, the Z register.
 * '0' is a matching constraint. It tells the compiler that this input constraint is the same thing as the (number-) matching output operand. basically saying __localVar__ = __pointer__;
 * Note: The compiler is not prevented from shitting on these - it can still overrule you by moving the pointer to the register you ask and then moving it out, but if it's doing that,
 * the overhead is probably not avoidable: you filled up all the pointers so no matter what happens will probably be bad; hence why we recommend keeping one eye on the asm listing.
 * If you think you can do better in that scenario, you're probably going to have to write some assembly. At least it's AVR not something horrible like PIC!
 *
 */

/****************/
/* _makeFastPtr */
/****************/
#define _makeFastPtr_d(newptt, highbyte, lowbyte) (__asm__ __volitile("ldi %0B, %1 " "\n\t" "mov %0A, %2 " "\n\t":"+b"((uint16_t) newptr):"M" ((uint8_t) highbyte), "r" ((uint8_t ), lowbyte)
#define   _makeFastPtr(newptt, highbyte, lowbyte) (__asm__ __volitile("ldi %0B, %1 " "\n\t" "mov %0A, %2 " "\n\t":"+e"((uint16_t) newptr):"M" ((uint8_t) highbyte), "r" ((uint8_t ), lowbyte)
/* _makeFastPtr_d(uint8_t * newptt, const uint8_t highbyte, uint8_t lowbyte)
 * _makeFastPtr(uint8_t * newptt, const uint8_t highbyte, uint8_t lowbyte)
 *
 * Very similar to the above. Passed a pointer as two bytes, the high byte constant and the low byte not, this finds use in the same sort
 * of situatios as high/low math. See the I/O headers: each class of peripherals often has the same high byte for all addresses. Ports (0x0400, 0x0420, 0x0440
 * and so on. The most frequently used functions get special attention paid to this as a small gain adds up for the most commonly called functions
 * usage is typically sdmething like
 *    lowbyte = (_SWAP(usartnbr)); // passed from,elsewhere io the code, which must be free of bugs!
 *    lowbyte <<= 1;
 *    volatile uint8_t *myptr l;
 *    _makeFastPtr_d(myptr. 0x08, lowbyte)
 * Now, myptr points to the start of the specified USART, in just 2 clock cycles (4 counting the setup in the example), instead of the typical 4 (6) the compiler creates.
 * This, like most of this file, is the sort of thing you should use under the most constrained conditions only (in an ISR, or a core function that gets called right and left
 * or extreme-time-sensitive moments, or when you are using a part with less flash than you should, can't get one with larger flash (either they don't exist, or are
 * backordered past next year), and you just desperately need a tiiiiiiny bit more flash, but you've exhausted the usual methods.
 *
 * SK: I've totally been desperate enough on a 16k part that I was replacing double-quoted strings 1 character long with single quoted characters
 * in order to get rid of the space the null took up and pulling strings from progmem to RAM because that saved a word vs printing it with F() macro
 * (it was a classic AVR, no memory mapped flash). I was told 500 of them were getting programmed the next day and we needed a feature added. One that I suspected
 * weeks ago would be needed, but they'd finally realized it the evening before D-day at around 9pm with a few hundred byes of flash left. Good times!
 *
 * _makeFastPtr() should be used when you are only reading from that one address, or consecutive addresses starting there (eg, *ptr++ or *--ptr),
 *  so it can use the X, Y or Z register.
 * _makeFastPtr_d() uses only X or Y register, and should be used if you expect to be accessing constant compile time known constant offsets
 *
 */

  #endif
