# Interrupts defined manually
Use of advanced functionality of these parts frequenctly requires use of interrupts.

## What is an interrupt
As the name implies, an interrupt is something that can cause the currently running code to stop in it's tracks. The location of the instruction it was about to execute is pushed onto the stack, and it then jumps to a specific "interrupt vector" near the start of the flash. This in turn is a jump (or rjump) to the Interruipt Service Routine (ISR). This runs, and then returns to the code that was interrupted through a `RETI` instruction. Almost every peripheral can generate at least one interrupt, and most can generate several. See the datasheet for more information on what conditions they can be generated in.

## Creating an ISR
There are two ways that you may end up writing an ISR, but most of the same considerations apply. The first most Arduino users will see is an `attachInterrupt()` function or method - these take a function name as an argument. Somewhere in the core or library is the ISR itself, which checks if you've attached one, and calls it if so. This is simpler - where it's an option - though the performance suffers profoundly. While the function call itself takes only 7-8 clock cycles including the return - but the overhead it imposes if much worse. An interrupt can be called at any time. Thus, it cannot make any assumptions about working registers that are or are not available. It has to assume everything is used, and save and restore each one (by `push`ing it onto the stack at the start, and `pop`ing it back at the end; even r0 (tempreg) and r1 (the known zero). The status register ALSO needs to be saved and restored. These happen in the "prologue" and "epilogue" Any ISR (except for one declared naked) will always push r1 and r0 onto the stack, load the sreg into r0 and push that onto the stack, and clear r1 in the prologue, and undo those in the "epilogue". If you your interrupt accesses additional variables, which it probably will, working registers needed to fit them are also saved and restored (in the prologue and epilogue, not at the time they're used.  If you call a function from within the ISR, and it is either defined at runtime (ex, via attachInterrupt), or is called from multiple places, such that the compiler doesn't inline it, it is required to assume that **all** registers that a function is allowed to use without saving and restoring registers are used. That gives an overhead of something like 68 clock cycles, just because a function is called. something like attachInterrupt also has overhead where it looks at the flags and then sees which of the many interrupts which may have been attached it should call. Don't forget that this takes flash as well, and the overhead of 40-50 bytes per interrupt adds up.

The other way is directly - it's as if you are declaring a function, but instead of the name, you use the ISR() macro with the vector name as it's argument; these can have overhead as low as 21 clock cycles, split approximately evenly between before and after:

```c++
ISR(CCL_CCL_vect) {
  //try to keep this part fast.
}
```

That 21 clock overhead figure is a best-case value - it can be worse worst case possible is 68 clocks more than the same code in another context - 26 before and 42 after (It is actually quite easy to end up in that worst case - just call a function, and you're there). Well written interrupts are short and sweet and will have overhead closer to 21 than 68, while interrupts called with attachInterrupt-type methods will have an overhead of hundreds of clock cycles. The improved version of attachInterrupt in this core is somewhat less bad than the standard one, with overhead as low as around 100 clocks if the interrupt is set on bit 0 of the port (the further up in the port you go, the higher the overhead, because it checks them sequentially).

## Only one definition per vector
You cannot define the same vector as two different things. This is most often a problem with the default settings for `attachInterrupt()` - to mimic the standard API, by default, we permit attach interrupt on any pin. Even if no pin in a port is attached to, it will still always take over every port interrupt vector. There is a menu option added to 1.3.7 to select between 3 versions of `attachInterrupt()` - the new version (default), the old one (in case there is a new bug introduced by this), and manual. In manual mode you can restrict it to specific ports, such that it leaves other port vectors unused. You must call `attachPortAEnable()` (replace A with the letter of the port) before attaching the interrupt. The main point of this is that (in addition to saving flash) `attachInterrupt()` on one pin (called by a library, say) will not glom onto every single port's pin interrupt vectors so you can't manually define any. The interrupts are still just as slow (it's inherrent to calling a function by pointer from an ISR - and low-numbered pins are faster to start executing than high numbered ones. The method to enable may change - I had hoped that I could detect which pins were used, but I couldn't get the function chose which ports to enable to not count as "referencing" those ports, and hence pull in the ISR. I am not happy with it, but "can't use any pin interrupts except through `attachInterrupt()` if using a library that uses `attachInterrupt()`  is significantly worse.

## List of interrupt vector names
If there is a list of the names defined for the interrupt vectors is present somewhere in the datasheet, I was never able to find it. These are the possible names for interrupt vectors on the parts supported by megaTinyCore. Not all parts will have all interrupts listed below (interrupts associated with hardware not present on a chip won't exist there). An ISR is created with the `ISR()` macro.

**WARNING** If you misspell the name of a vector, you will get a compiler warning BUT NOT AN ERROR! Hence, you can upload the bad code... in this case the chip will freeze the instant the ISR you thought you assigned is called, as it jumps to BAD_ISR, which in turn jumps to the reset vector... but since the reti instruction is never executed, it still thinks its in an interrupt. If the recommended procedures in the [reset reference](https://github.com/SpenceKonde/megaTinyCore/blob/master/megaavr/extras/Ref_Reset.md) were followed, this will be detected through the absence of a new reset flag, triggering a software reset; Otherwise, the code will start running, with interrupts "enabled" but not actually occurring, which usually but not always results in a hang or bootloop, but could potentially do something more subtly wrong. Encountering this (and the annoying lack of a nice list anywhere outside of the io.h) was the impetus for creating this list.

| Vector Name       | 0 | 1 | 2 | Flag Cleared on | Notes                                       |
|-------------------|---|---|---|-----------------|---------------------------------------------|
| AC0_AC_vect       | X | X | X | Manually        |                                             |
| AC1_AC_vect       |   | * |   | Manually        | 1-series with 16k or 32k flash only         |
| AC2_AC_vect       |   | * |   | Manually        | 1-series with 16k or 32k flash only         |
| ADC0_ERROR_vect   |   |   | X | Manually        | Multiple flags for different errors         |
| ADC0_SAMPRDY_vect |   |   | X | Read sample reg | WCOMP uses SAMPRDY or RESRDY on 2-series    |
| ADC0_RESRDY_vect  | X | X | X | Read result reg |                                             |
| ADC0_WCOMP_vect   | X | X |   | Manually        | Window Comparator on 0/1-series             |
| ADC1_RESRDY_vect  |   | * |   | Read result reg | 1-series with 16k or 32k flash only         |
| ADC1_WCOMP_vect   |   | * |   | Manually        | 1-series with 16k or 32k flash only         |
| BOD_VLM_vect      | X | X | X | Manually(?)     |                                             |
| CCL_CCL_vect      |   |   | X | Manually        | Shared by whole CCL. Not present on 0/1     |
| CRCSCAN_NMI_vect  | X | X | X | Reset           | NMI guarantees device stopped if CRC fails  |
| NVMCTRL_EE_vect   | X | X | X | Write/Manually  | ISR must write data or disable interrupt    |
| PORTA_PORT_vect   | X | X | X | Manually        |                                             |
| PORTB_PORT_vect   | X | X | X | Manually        | Not present on 8-pin parts                  |
| PORTC_PORT_vect   | X | X | X | Manually        | Not present on 8 or 14-pin parts            |
| RTC_CNT_vect      | X | X | X | Manually        | Two possible flags, CNT and OVF             |
| RTC_PIT_vect      | X | X | X | Manually        |                                             |
| SPI0_INT_vect     | X | X | X | Depends on mode | 2 or 5 flags, some autoclear, some dont     |
| TCA0_CMP0_vect    | X | X | X | Manually        | Alias: TCA0_LCMP0_vect                      |
| TCA0_CMP1_vect    | X | X | X | Manually        | Alias: TCA0_LCMP1_vect                      |
| TCA0_CMP2_vect    | X | X | X | Manually        | Alias: TCA0_LCMP2_vect                      |
| TCA0_HUNF_vect    | X | X | X | Manually        |                                             |
| TCA0_OVF_vect     | X | X | X | Manually        | Alias: TCA0_LUNF_vect                       |
| TCB0_INT_vect     | X | X | X | Depends on mode | Two flags on 2 series only. Behavior depends on mode, see datasheet |
| TCB1_INT_vect     |   | * | X | Depends on mode | 1-series with 16k or 32k flash or 2-series  |
| TCD0_OVF_vect     |   | X |   | Manually        |                                             |
| TCD0_TRIG_vect    |   | X |   | Manually        |                                             |
| TWI0_TWIM_vect    | X | X | X | Usually Auto    | See datasheet, flag clearing is complicated |
| TWI0_TWIS_vect    | X | X | X | Usually Auto    | See datasheet, flag clearing is complicated |
| USART0_DRE_vect   | X | X | X | Write/Disable   | ISR must write data or disable interrupt    |
| USART0_RXC_vect   | X | X | X | RXCIF, on read  | Error flags, if enabled, only cleared manually |
| USART0_TXC_vect   | X | X | X | Manually        | Often used without the interrupt enabled    |

`*` - There are two classes of 1-series - the ones with 16k or more of flash, and the ones with less. These are only available on the larger ones, because they operate on a peripheral that only exists there.

## Why clearing flags is so complicated
Almost all flags *can* be manually cleared - the ones that can be cleared automatically generally do that to be helpful:
* When the purpose of the flag is to tell you that something is ready to be read, reading it clears the flag. ADC, serial interfaces, and TCB input capture do that.
* The TWI interrupts work the same way - you need to read, write, or ack/nack something to respond to the bus event; doing so clears the flag too.
* Sometimes interrupts like that can have error flags that can trigger them enabled too; those typically have to be manually cleared - by enabling them, you declare an intent to do something about them, so you're responsible for telling the hardware you did it.
* USART, and buffered SPI have DRE interrupt that can only be cleared by writing more data - otherwise you need to disable the interurpt from within the ISR. The `USARTn.TXC` (transfer/transmit complete) flags are freqently polled rather than used to fire interrupts. It's not entirley clear from the datasheet if the EEPROM ready interrupt is like that, or can be cleared manually.
* The NMI is a very special interrupt; it can be configured to be a normal interrupt *or* a Non-Maskable Interrupt. In NMI mode, the part will sit there running the interrupt instead of almost-working with damaged firmware - which could potentially create a dangerous situation if it was part of a life-saftety critical device, like the controller for an airbag, or antilock breaks in a car. In such cases, corrupted firmware might appear work fine, if the corruption only impacted code-paths related to handling the relevant emergency situations, so the vehicle and hence operator would not be aware of a problem until . No matter what the damaged firmware tries to do, it cannot disable or bypass the NMI. Only loading working firmware and resetting it will clear the NMI. This is of particular relevance in life-safety-critical applications which these parts (but NOT this software package nor Arduino in general) are certified for. Not something likely to be used in Arduino-land.

### Vectors linked to many flags
There are a few vectors with a lot of flags that can trigger them. For example, each of the PORT interrupts has 8 flags that can trigger it. One hazard with these is that if you have a large number enabled - especially if your ISR is longer than it ought to be - that interrupts could fire whille the ISR is running. You need to make sure you aren't missing those:
*note - these depict calling a function from the ISR. That's generally bad unless it will be automatically inlined because it is only referenced here, since it increases overhead due to required register saving and restoration.*

**Wrong**
```c++
ISR(PORTA_PORT_vect) {
  if (VPORTA.INTFLAGS & (1 << 0)) {
    doSomething();
  }
  if (VPORTA.INTFLAGS & (1 << 1)) {
    doSomethingElse();
  }
  VPORTA.INTFLAGS=VPORTA.INTFLAGS //WRONG - if an interrupt happened after it's conditional, it would be missed.
}
```

**Less wrong**
```c++
ISR(PORTA_PORT_vect) {
  byte flags=PORTA.INTFLAGS; //Note: slower than VPORT; use VPORTx, not PORTx for INTFLAGS
  if (flags & (1 << 0)) {
    doSomething();
  }
  if (flags & (1 << 1)) {
    doSomethingElse();
  }
  PORTA.INTFLAGS=flags; // Better... if you care whether one of those conditions happens again, though, you could still miss it.
}
```

**Correct**
```c++
// Check and clear flags at start of ISR.
ISR(PORTA_PORT_vect) {
  byte flags=VPORTA.INTFLAGS;
  PORTA.INTFLAGS=flags; // Very common approach
  if (flags & (1 << 0)) {
    doSomething();
  }
  if (flags & (1 << 1)) {
    doSomethingElse();
  }
}
```
**Also correct**
```c++
// This could be made into an else-if in order to let other interrupts fire if your ISR is slow, and is
// likely to be called often with multiple flags set - that case goes particularly well with round-robin
// interrupt scheduling - but if you're in a situation where you need this, you should be concerned about
// larger scale probems with your code - you're either generating way too many interrupts, or they are
// far too slow...

ISR(PORTA_PORT_vect) {
  if (VPORTA.INTFLAGS & (1 << 0)) {
    VPORTA.INTFLAGS |= (1 << 0);
    doSomething();
  }
  if (VPORTA.INTFLAGS & (1 << 1)) {
    VPORTA.INTFLAGS |= (1 << 1);
    doSomethingElse();
  }
}
```

Note: `if (VPORTx.INTFLAGS & (1 << n))` is a maximally efficient way to test for a bit in a `VPORTx.anything` register or one of the 4 GPIORn/GPIOn/GPR.GPRn. Those registers (like many assorted important registers on classic AVRs - and unlike any other registers besides VPORTs and GPR/GPIOR/GPIO registers (over the past 6 years, the've been known by every one of those officially) are in the "Low I/O space", and instructions for atomic bit-level access exist. (set, clear, and skip-if-set/cleared).


## If you don't need to do anything in the ISR
For example, if it's only purpose is to wake from sleep, `ISR(PERIPHERAL_INT_vect) {;}` is slower and larger than `EMPTY_INTERRUPT(PERIPHERAL_INT_vect);` The latter will produce an ISR containing only a reti instruction. The former will generate the standard prologue and epilogue, for 21-26 clock overhead, instead of just the approx. 10 clock minimum overhead from getting to the vector and returning from it.

## If two interrupts have an identical implementation
You can point two interrupt vectors at the same code, which imposes no flash penalty compared to having only one of them - however, you have ZERO information on which one triggered it in this case, and if you need to figure that out, that cost you some time, which you need to weigh against the flash cost.

```c
ISR(PERIPHERAL0_INT_vect, ISR_ALIASOF(PERIPHERAL1_INT_vect));
ISR(PERIPHERAL1_INT_vect){
  /* handle PERIPHERAL0_INT and PERIPHERAL1_INT here */
}
```

`ISR_ALIASOF()` can point a vector at an `EMPTY_INTERRUPT` which saves 2 bytes of flash compared to each one set EMPTY_INTERRUPT individually.

## Reminders
* ISRs should run FAST. Minimize the time that the code spends in the ISR. Never use polling loops unless you know that they will only need a couple of passes (an example would be TCD0 ENRDY or CMDRDY, which will never take longer than 16 clocks to clear), and avoid writing to serial. Most interrupts should just set a flag that is checked elsewhere, and do what must be done that instant (ex, read an incoming data byte from a register and store it in a buffer then set a flag or byte indicating at what point in the buffer it's at. Don't process the byte you received and figure out what it's instructing you to do - that should be done outside of the interrupt).
* `delay()` must never be used in an ISR. It depends on the timekeeping interrupt firing every millisecond. Depending on the timer used and other details, it will either last forever, or expire within 2 ms).
* If you absolutely must write something to serial, maybe for debugging - can you make it a single character? `Serial.write('*');` is much less bad than `Serial.print("Int1 fired");` (notice the single quotes - use those when you print a single character for faster execution; instead of printing a char array of length 2, the second one being the null terminator, this ).
* Read the datasheet, particularly relating to the relevant INTFLAGS register - make sure you understand when it is and is not cleared automatically, and when you need to clear it. And be sure you do so. If execution appears to slow to a crawl once the ISR fires once - you probably didn't clear the flags, resulting in the ISR executing once for every instruction outside of the ISR that is executed.
* Any global variable that an interrupt changes, if used outside the ISR, must be declared volatile - otherwise the compiler may optimize away access to it, resulting in code elsewhere not knowing that it was changed by the ISR.
* Any global variable read by the ISR and written to by code outside of the ISR which is larger than a byte must be written to with interrupts disabled - if the interrupt triggers in the middle of a write, the ISR would see a corrupted value.
* Any global variable or register subject to a read-modify-write cycle in code outside the ISR and written to within an ISR must disable interrupts while performing the read-modify-write in order to make the operation atomic.
* For the three cases immediately above, if a level 1 priority interrupt is set, the same three rules apply to variables used by a level 0 ISR and the level 1 ISR.
* The core does not use the level 1 interrupt priority option. Thus, an interrupt blocks all other interrupts, like on classic AVRs (though the global interrupt flag in the SREG is not set or cleared). Hence it has the same caveats as other things that disable interrupts (this is one of the major reasons that ISRs need to run fast):
  * An interrupt that lasts longer than 10/(usart baud rate) can cause received characters to be missed (technically, you have twice that long because it's double-buffered, minus the time overhead at the start of the interrupt that reads it. The "timer" starts from when the first byte arrives - the interrupt must fire and reach the point where the RXDATA register is read before the third byte arrives. The ISR is not something from the Museum of Efficient and Well Written ISRs, either.
  * An interrupt that lasts longer than 0.5-4ms (details depend on the timer used for millis, and at what point in the cycle it was at) will lose time as viewed through `millis()` and `micros()`
  * After 0.5-2ms (potentially as little as 512 system clocks on 1.3.6 and earlier, micros will jump backwards in time by around 0.5-2 ms. The thresholdfor this occurring and the size of the jump will depend on the system clock speed, millis timer, and what point in the cycle it is at when interrupts are disabled.


## A bit more on timing
So as described above, execution reaches the ISR within 6 system clock cycles (sometimes 5, now that rjmp is used instead of jmp when possible); Then the interrupt has to take special measures to save the state of what was interrupted, which it must restore at the end - this is automatically done by AVR-GCC at the beginning and end of an ISR (these are called the prologue and epilogue); the compiler must assume that every working register, plus the SREG, contains something that must be saved. These get saved by `push`ing them onto the stack. All of the needed registers are freed up this way before your code starts to actually execute (the more variables your ISR needs at a time, the more the prologue needs to `push` onto the stack, and the longer the delay before your code begins executing. At least on the modern AVRs, `push` is only 1 clock cycle (in the past, it was two). Of course, saving the state is only half of the job - after running the ISR that you wrote, the epilogue has to `pop` all those registers off the stack - this takes two clocks a piece. So while the datasheet loves to talk about 6 clocks to enter the interrupt routine - the bare minimum the compiler will produce is 10: 5 to get there assuming the compiler was able to use an rjmp to get there, then push r1, and r0, load SREG into one of those, push that onto the stack, and then zero out r1 (gcc needs a known zero register). So 10 clocks to enter. Then at the end pop the SREG value and store it, then pop the r0 and r1, then 4 clocks for the reti for 11 on the way out end.... plus the overhead of saving and restoring any registers your code uses. In DxCore or megaTinyCore, when a TCB is used for millis, all the ISR does is load the millisecond count, add 1 to it, and save it (that's all it does!). This requires 17 clocks in the prologue, 12 more to load the current millis tally, 4 to increment it, 8 more to save it, 3 to clear the interrupt flag, then the 4 registers we pushed need to be restored taking 8 clocks plus the 11 standard ones. That ISR winds up taking 17 + 27 + 19 = 63 clock cycles for a task that would take half that long outide of an ISR (note that a dedicated individual could implement that in assembly to save 2-3 clocks on the prologue, 4 in the body, and 4-6 in the epilogue - but that's beside the point, you can always look at generated assembly and find missed opportinities. The compiler misses more opportunities than usual in ISRs). 128k parts add 2 clocks to prologue and 3 to epilogue because they also have to save and restore RAMPZ (even if isn't changed by the ISR). This just makes it that much more important to be careful about ISRs, since execution time is amplified by the added constraints on ISRs.
One of the worst things is calling a function that won't end up being inlined or can't be optimized - like the "attachInterrupt" functions - in this case, the prologue + jump is minimum 24 clocks, and the epilogue 39, as it must assume the function uses all of the "call used" registers and save and restore them all.

### ISRs benefit the most from using the GPRs
If you're desperate for speed - or space - and if all you are doing is setting a flag, you can use one of the general purpose registers: GPR.GPR0/1/2/3 - the only place the core uses any of those is at the very beginning of execution, when the reset cause is stashed in `GPIOR0` before the reset flags are cleared and the sketch is run (that way you can see what the reset cause was - the core does not use it after that) To get the full benefit, use single-bit operations - they're in the low I/O space. So something like `GPIOR1 |= (1 << n)` where n is known at compile time, is a single clock operation which consumes no registers - it gets turned into a `sbi` - set bit index, with the register and bit being encoded by the opcode itself. The same goes for `GPIOR &= ~(1 << n)`  - these are also atomic (an interrupt couldn't interrupt them like it could a read-modify-write). There are analogous instructions that make things like `if(GPIOR1 & (1 << n))` and `if (!(GPIOR1 & (1 << n))` lightning fast. GPIOR's are only magic when manipulating a single bit, and the bit and must be known at compile time: `GPIOR1 |= 3` is a 3 clock non-atomic read-modify-write operation which needs a working register to store the intermediate value in while modifying it, which is just slightly faster than `MyGlobalByte |= 3`, which is a 6-clock non-atomic read-modify-write using 1 working register for the intermediate. `GPIOR1 |= 1; GPIOR1 |= 2;` is 2 clocks, each of which is an atomic operation which does not require a register to store any intermediate values. Note that atomicity is only a concern for code running outside the ISR, or code within a level 0 priority ISR when some other ISR is configured with level 1 priority.

## Naked ISRs
An advanced technique. This requires that either your ISR be written entirely in assembly, with your own prologue and epilogue hand optimized for this use case, or that you know for a fact that the tiny piece of C code you use doesn't change `SREG` or use any working registers.
In a naked ISR, all the compiler does for you is tell the linker to that this code is where it should point the vector. When writing a naked ISR, You are responsible for everything: prologue. epilogue, and the reti at the end. Don't forget the reti, without that you'll get crazy (and severely broken) behavior.

```c
/* OK - results in an sbi that neither changes SREG nor uses a register*/
ISR(PERIPHERAL_INT_vect, ISR_NAKED)
{
  GPIOR1 |= (1 << 0);
  reti();
}
/* OK - digitalWriteFast() where both arguments are constant maps directly to sbi*/
ISR(PERIPHERAL_INT_vect, ISR_NAKED)
{
  digitalWriteFast(PIN_PB4, HIGH);
  reti();
}
/* NO! it will need a register for MyGlobalByte*/
ISR(PERIPHERAL_INT_vect, ISR_NAKED)
{
  digitalWriteFast(PIN_PB4, MyGlobalByte);
  reti();
}
/* NO! setting multiple bit requires a register */
ISR(PERIPHERAL_INT_vect, ISR_NAKED)
{
  GPIOR1 |= 3;
  reti();
}
/* OK - the separate writes is okay, neither require a register. */
ISR(PERIPHERAL_INT_vect, ISR_NAKED)
{
  GPIOR1 |= 1;
  GPIOR1 |= 2;
  reti();
}
/* NO! This is a read-modify-write, AND the addition changes SREG. */
ISR(PERIPHERAL_INT_vect, ISR_NAKED)
{
  GPIOR1++;
  reti();
}
/* NO! You must reti(), or it will continue on and execute whatever happens to be after it in the flash (potentially anything) */
ISR(PERIPHERAL_INT_vect, ISR_NAKED)
{
  GPIOR1 |= (1 << 0);
}
/* OK - turns into sbic (Skip-next-instruction-if-Bit-in-I/O-is-Clear), sbi  */
ISR(PERIPHERAL_INT_vect, ISR_NAKED)
{
  if (GPIOR1 & (1 << 0))
    GPIOR1 |= (1 << 0);
  reti();
}
/* Anything more compllicated written in C - even when it could be safe - should be verified by reading the assembly listing.
 * The compiler does not "know" that it has to avoid using any working registers or changing the SREG.
 * ISR_NAKED is really meant to run hand-optimized assembly, rather than relying on the compiler not doing anything dumb.
 */
```
