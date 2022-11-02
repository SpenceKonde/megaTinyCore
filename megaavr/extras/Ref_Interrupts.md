# Interrupts defined manually
Use of advanced functionality of these parts frequenctly requires use of interrupts.

## What is an interrupt
As the name implies, an interrupt is something that can cause the currently running code to stop in it's tracks. The location of the instruction it was about to execute is pushed onto the stack, and it then jumps to a specific "interrupt vector" near the start of the flash. This in turn is a jump (or rjump) to the Interruipt Service Routine (ISR). This runs, and then returns to the code that was interrupted through a `RETI` instruction. Almost every peripheral can generate at least one interrupt, and most can generate several. See the datasheet for more information on what conditions they can be generated in.

## Creating an ISR
There are two ways that you may end up writing an ISR, but most of the same considerations apply. The first most Arduino users will see is an `attachInterrupt()` function or method - these take a function name as an argument. Somewhere in the core or library is the ISR itself, which checks if you've attached one, and calls it if so. This is simpler - where it's an option - though the performance suffers profoundly. While the function call itself takes only 7-8 clock cycles including the return - but the overhead it imposes if much worse. An interrupt can be called at any time. Thus, it cannot make any assumptions about working registers that are or are not available. It has to assume everything is used, and save and restore each one (by `push`ing it onto the stack at the start, and `pop`ing it back at the end; even r0 (tempreg) and r1 (the known zero). The status register ALSO needs to be saved and restored. These happen in the "prologue" and "epilogue" Any ISR (except for one declared naked) will always push r1 and r0 onto the stack, load the sreg into r0 and push that onto the stack, and clear r1 in the prologue, and undo those in the "epilogue". If you your interrupt accesses additional variables, which it probably will, working registers needed to fit them are also saved and restored (in the prologue and epilogue, not at the time they're used.  If you call a function from within the ISR, and it is either defined at runtime (ex, via attachInterrupt), or is called from multiple places, such that the compiler doesn't inline it, it is required to assume that **all** registers that a function is allowed to use without saving and restoring registers are used. That gives an overhead of something like 68 clock cycles, just because a function is called. something like attachInterrupt also has overhead where it looks at the flags and then sees which of the many interrupts which may have been attached it should call. Don't forget that this takes flash as well, and the overhead of 40-50 bytes per interrupt adds up.

For more information on attachInterrupt and pin interrupts in general [see the pin interrupt reference](Ref_PinInterrupts.md).

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

**WARNING** If you misspell the name of a vector, you will get a compiler warning BUT NOT AN ERROR! Hence, you can upload the bad code... in this case the chip will reset the instant the ISR you thought you assigned is called, as it jumps to BAD_ISR, which in turn jumps to the reset vector... but since the reti instruction is never executed, it still thinks its in an interrupt. Unless you've defanged the megaTiynCore dirty reset protection, this will be caught and trigger an unexpected reset. Otherwise, it will either reset uncleanly, hang, or behave unpredictably. See the [reset reference](https://github.com/SpenceKonde/megaTinyCore/blob/master/megaavr/extras/Ref_Reset.md) for more information on this potential failure mode.

| Vector Name       | 0 | 1 | 2 |Flags| Flag Cleared on | Notes                                       |
|-------------------|---|---|---|-----|-----------------|---------------------------------------------|
| AC0_AC_vect       | X | X | X |  1  | Manually        |                                             |
| AC1_AC_vect       |   | * |   |  1  | Manually        | 1-series with 16k or 32k flash only         |
| AC2_AC_vect       |   | * |   |  1  | Manually        | 1-series with 16k or 32k flash only         |
| ADC0_ERROR_vect   |   |   | X | >1  | Manually        | Multiple flags for different errors         |
| ADC0_SAMPRDY_vect |   |   | X | 1/2 | Read sample reg | WCOMP uses SAMPRDY or RESRDY on 2-series    |
| ADC0_RESRDY_vect  | X | X | X | 1/2 | Read result reg |                                             |
| ADC0_WCOMP_vect   | X | X |   |  1  | Manually        | Window Comparator on 0/1-series             |
| ADC1_RESRDY_vect  |   | * |   |  1  | Read result reg | 1-series with 16k or 32k flash only         |
| ADC1_WCOMP_vect   |   | * |   |  1  | Manually        | 1-series with 16k or 32k flash only         |
| BOD_VLM_vect      | X | X | X |  1  | Manually(?)     | The behavior is not exactly clear for flag  |
| CCL_CCL_vect      |   |   | X | 2/4 | Manually        | Shared by whole CCL. Not present on 0/1     |
| CRCSCAN_NMI_vect  | X | X | X |  1  | Reset           | NMI can ensure device stopped if CRC fails  |
| NVMCTRL_EE_vect   | X | X | X |  1  | Write/Manually  | ISR must write data or disable interrupt    |
| PORTA_PORT_vect   | X | X | X |  8  | Manually        |                                             |
| PORTB_PORT_vect   | X | X | X |  8  | Manually        | Not present on 8-pin parts                  |
| PORTC_PORT_vect   | X | X | X |  8  | Manually        | Not present on 8 or 14-pin parts            |
| RTC_CNT_vect      | X | X | X |  2  | Manually        | Two possible flags, CNT and OVF             |
| RTC_PIT_vect      | X | X | X |  1  | Manually        |                                             |
| SPI0_INT_vect     | X | X | X | 2-5 | Depends on mode | 2 in normal, 5 in buffered mode. See datasheet|
| TCA0_CMP0_vect    | X | X | X |  1  | Manually        | Alias: TCA0_LCMP0_vect if in SPLIT mode **  |
| TCA0_CMP1_vect    | X | X | X |  1  | Manually        | Alias: TCA0_LCMP1_vect if in SPLIT mode **  |
| TCA0_CMP2_vect    | X | X | X |  1  | Manually        | Alias: TCA0_LCMP2_vect if in SPLIT mode **  |
| TCA0_HUNF_vect    | X | X | X |  1  | Manually        | SPLIT mode only                             |
| TCA0_OVF_vect     | X | X | X |  1  | Manually        | Alias: TCA0_LUNF_vect if in SPLIT mode **   |
| TCB0_INT_vect     | X | X | X |  2  | Depends on mode | Two flags on 2 series only. Behavior depends on mode, see datasheet |
| TCB1_INT_vect     |   | * | X |  2  | Depends on mode | 1-series with 16k or 32k flash or 2-series, 2 flags on 2-series only  |
| TCD0_OVF_vect     |   | X |   |  1  | Manually        |                                             |
| TCD0_TRIG_vect    |   | X |   |  1  | Manually        |                                             |
| TWI0_TWIM_vect    | X | X | X | >1  | Usually Auto    | See datasheet, this is complicated and kind |
| TWI0_TWIS_vect    | X | X | X | >1  | Usually Auto    | -of obtuse. Just use Wire.h                 |
| USART0_DRE_vect   | X | X | X |  1  | Write/Disable   | ISR must write data or disable interrupt    |
| USART0_RXC_vect   | X | X | X |  1  | RXCIF, on read  | Error flags, if enabled, only cleared manually |
| USART0_TXC_vect   | X | X | X |  1  | Manually        | Often used without the interrupt enabled    |

`*` - There are two classes of 1-series - the ones with 16k or more of flash, and the ones with less. These are only available on the larger ones, because they operate on a peripheral that only exists there.
`**` - These vectors are traditionally referred to by different names when in split mode or normal mode, but the hardware can't tell the difference between the names.


## Why clearing flags is so complicated
Almost all flags *can* be manually cleared - the ones that can be cleared automatically generally do that to be helpful:
* When the purpose of the flag is to tell you that something is ready to be read, reading it clears the flag. ADC, serial interfaces, and TCB input capture do that.
* The TWI interrupts work the same way - you need to read, write, or ack/nack something to respond to the bus event; doing so clears the flag too.
* Sometimes interrupts like that can have error flags that can trigger them enabled too; those typically have to be manually cleared - by enabling them, you declare an intent to do something about them, so you're responsible for telling the hardware you did it.
* USART, and buffered SPI have DRE interrupt that can only be cleared by writing more data - otherwise you need to disable the interrupt from within the ISR. The `USARTn.TXC` (transfer/transmit complete) flags are frequently polled rather than used to fire interrupts. It's not entirely clear from the datasheet if the EEPROM ready interrupt is like that, or can be cleared manually.
* The NMI is a very special interrupt; it can be configured to be a normal interrupt *or* a Non-Maskable Interrupt. In NMI mode, the part will sit there running the interrupt instead of almost-working with damaged firmware - which could potentially create a dangerous situation if it was part of a life-safety critical device, like the controller for an airbag, or antilock breaks in a car. In such cases, corrupted firmware might appear work fine, if the corruption only impacted code-paths related to handling the relevant emergency situations, so the vehicle and hence operator would not be aware of a problem until . No matter what the damaged firmware tries to do, it cannot disable or bypass the NMI. Only loading working firmware and resetting it will clear the NMI. This is of particular relevance in life-safety-critical applications which these parts (but NOT this software package nor Arduino in general) are certified for. Not something likely to be used in Arduino-land.

## For non-port interrupts, clear the INTFLAGS after configuring everything except actually turning on the interrupt
With only a few exceptions (see the datasheet and in the case of he ACO, errata as well) interrupt flags are set regardless of whether the interrupt is enabled, so it's not unusual for an intflag to have become set either during the configuration process, or prior to it. Once you configure the peripheral, you should clear the INTFLAGS as one of the last steps, before the interrupts are enabled I usually configure everything else, clear the flags, enable the interrupts I want, and finally enable the peripheral.

Note that the flags are still set if interrupts are globally disabled. So even if you globally disable interrupts while configuring a peripheral during the configuration process, you must clear the flags prior to you reenabling interrupts.

PORT interrupts only set the flags when the PINnCTRL register is set to generate an interrupt.

## Vectors linked to many flags
There are a few vectors with a lot of flags that can trigger them. For example, each of the PORT interrupts has 8 flags that can trigger it. One hazard with these is that if you have a large number enabled - especially if your ISR is longer than it ought to be - that interrupts could fire while the ISR is running. You typically want to avoid missing any interrupts. *note - these depict calling a function from the ISR. Unless the function is only called in this one place and hence will be inlined, that is not good practice, and it involves a an extra 30 bytes and 45 clocks (approx) of overhead just because it's a function call*

**Wrong**
```c++
ISR(PERIPHERAL_INT_vect) {
  if (PERIPHERAL.INTFLAGS & (1 << 0)) {
    doSomething();
  }
  if (PERIPHERAL.INTFLAGS & (1 << 1)) {
    doSomethingElse();
  }
  PERIPHERAL.INTFLAGS=PERIPHERAL.INTFLAGS //WRONG - if an interrupt happened after it's conditional, it would be missed.
}
```

**Usually wrong - except for some pin interrupts**
The "late clear" method is far less wrong than the first. You read he flags, and base your actions on them. Then, after the interrupt processing is done, you clear **the flags you recorded and used**. This will guarantee that if you have interrupts on (say) other pins on a port different from the one that triggered the ISR, and they meet the interrupt condition while handling the first interrupt, as soon as the ISR returns, it will execute a single instruction and then restart the interrupt to handle the other pin(s). **this is the only correct way to deal with a LOW_LEVEL pin interrupt** since you need to turn off the interrupt during the ISR. On the other hand, it's possible that one handler will be much longer than the other (you try to avoid long running interrupts, but sometimes you have no choice). If one of the flags is handled fast, and the condition occurs relatively frequently you could miss one because you were handling the slow one.

```c++
ISR(PERIPHERAL_INT_vect) {
  byte flags=PERIPHERAL.INTFLAGS;
  if (flags & (1 << 0)) {
    doSomething();
  }
  if (flags & (1 << 1)) {
    doSomethingElse();
  }
  PERIPHERAL.INTFLAGS=flags;
}
```

**Usually right - but wrong for most pin interrupts**
One approach to address that is clearing the flags early. This is a good method for any interrupt except a pin interrupt that you disable in the ISR. Non-pin interrupts can be disabled by an interrupt enable bit (usually in PERIPHERAL.INTCTRL), and the INTFLAGS are set regardless of whether the pin is enabled. But pin interrupts that may retrigger very quickly (particularly a LOW LEVEL interrupt, which will immediately retrigger if the pin is still low, but also potentially switches that bounce). It is also suitable for pin interrupts where the interrupt running once more after you disable it (such as a do-nothing int for waking the chip) is not problematic.
```c++
// Check and clear flags at start of ISR.
ISR(PERIPHERAL_INT_vect) {
  byte flags=PERIPHERAL.INTFLAGS;
  PERIPHERAL.INTFLAGS=flags; // Very common approach
  if (flags & (1 << 0)) {
    doSomething();
    // Only risk with this is that if you disable the interrupt here, it may have happened again and set the INTFLAG again. In which case the interrupt would immediately run again.
  }
  if (flags & (1 << 1)) {
    doSomethingElse();
  }
}
```

**Dealing with the damned pin interrupts**
When you ARE disabling the interrupt, AND the interrupt flag will fire the ISR even when interrupt isn't enabled (ie, like a PORT pin interrupt), there are several approaches
```cpp
ISR(PORTA_PORT_vect) {
  //check flags
  byte flags = VPORTA.INTFLAGS; //here we'll use a port example, like I said, I can't think of any other cases where the disable behavior is wacky like this.
  // Let's assume flags 0 and 3 may be set.
  if (flags & (1 << 1)) {           // Check if the flag is set, if it is; since we se flags to determine what code to run, we need o check flags, otherwise we
    //                              // get to second half, and have cleared some flags that weren't recorded in flags because they happened during that timr!
    PORTA.PIN1CTRL &= ~PORT_ISC_gm; // we want to turn off hat interrupt so we do so here.
    VPORTA.INTFLAGS |= (1 << 1);    // Now clear flag knowing it won' get set again. .
  }
  if (flags & (1 << 3)) {  // Again // if nothing else, reading from that copy of flags is markedly faster.
    PORTA.PIN3CTRL &= ~PORT_ISC_gm; // we want to turn off hat interrupt so we do so here.
    VPORTA.INTFLAGS |= (1 << 3);    // Now clear flag knowing it won' get set again. .
  }
  // Now we've disabled the interrupt and clear the flags that were on, next step, use flags to decide what to do:
  if (flags & (1 << 1)) {
    // Handle flag 0 interrupt.
  }
  if (flags & (1 << 3)) {
    // Handle flag 3 interrupt.
  }
}


ISR(PORTA_PORT_vect) {
  // Imagine we have two interrupts on PORTA. Both the troublesome LOW_LEVEL type that we are using to wake from sleep, and we don't want the interrupts to fire while awake, that is, we want to disable the interrupts.
  // Howeer, one of the pins (say PA3) should also set a global flag, while the other (PA1) should just wake the part.
  PORTA.PIN3CTRL &= ~PORT_ISC_gm;   // disable both interrupts.
  PORTA.PIN1CTRL &= ~PORT_ISC_gm;   //
  if (flags & (1 << 3)) {           // Check for the flag
    GPIOR3 |= 1;                    // Set a global flag very quickly.
  }
  VPORTA.INTFLAGS = (1 << 3) | (1 << 1); // constant folding occurs here turning it into VPORTA.INTFLAGS = 0x09; Notice how VPORT intflags here is written with simple assignment.
}
```

**Always okay, but often one of above is the above is better**
The generalized case of the above, where we don't make any distinction between which ones deserve to be run right after thy're cleared, and which are longer than that should be.Clear the flags as you go is also viable - but, especially with many interrupts on a port, it can be less efficient. **WARNING** if those are actual function calls, and they can't be automatically inlined, this is by far the worst of the implementations! Avoid actual function calls in ISRs if you can, unless you know they will be inlined.
```cpp
ISR(PERIPHERAL_INT_vect) {
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

**Increased overhead - but sometimes appropriate**
```c++
// Clearing flags one at a time as they are handled, and then letting the interrupt handler return
// might be done in order to let other interrupts fire if your ISR
// is slow, and is likely to be called often with multiple flags set - that case goes particularly well with
// round-robin interrupt scheduling, or when there is another higher priority interrupt enabled that you don't want
// to be delayed while all of the interrupts for this vector are handled. If you're in a situation where you need
// this, that is a "red flag" that your handlers are too slow. or you're generating too many interrupts too fast.

ISR(PERIPHERAL_INT_vect) {
  if (VPORTA.INTFLAGS & (1 << 0)) {
    VPORTA.INTFLAGS |= (1 << 0);
    doSomething();
  }
  else if (VPORTA.INTFLAGS & (1 << 1)) {
    VPORTA.INTFLAGS |= (1 << 1);
    doSomethingElse();
  }
}
/* Equivalently - but more flexible because you can choose which cases do it more easily than you could normally */
ISR(PERIPHERAL_INT_vect) {
  if (VPORTA.INTFLAGS & (1 << 0)) {
    VPORTA.INTFLAGS |= (1 << 0);
    doSomething();
    return();
  }
  if (VPORTA.INTFLAGS & (1 << 1)) {
    VPORTA.INTFLAGS |= (1 << 1);
    doSomethingElse();
  }
}
```

Note: `if (VPORTx.INTFLAGS & (1 << n))` is a maximally efficient way to test for a bit in a `VPORTx.anything` register or one of the 4 GPIORn/GPIOn/GPR.GPRn. Those registers (like many assorted important registers on classic AVRs - and unlike any other registers besides VPORTs and GPR/GPIOR/GPIO (`*`) registers (over the past 6 years, they've been known by every one of those officially) are in the "Low I/O space", and instructions for atomic bit-level access exist. (set, clear, and skip-if-set/cleared).


## If you don't need to do anything in the ISR
For example, if it's only purpose is to wake from sleep, `ISR(PERIPHERAL_INT_vect) {;}` is slower and larger than `EMPTY_INTERRUPT(PERIPHERAL_INT_vect);` The latter will produce an ISR containing only a reti instruction. The former will generate the standard prologue and epilogue, for 21-26 clock overhead, instead of just the approx. 10 clock minimum overhead from getting to the vector and returning from it; however, this won't clear the interrupt flags, which on a modern AVR you almost always need to do. Very rarely are the INTFLAGS cleared when the handler executes, whereas on classic AVRs almost every interrupt did that

## If two interrupts have an identical implementation
You can point two interrupt vectors at the same code, which imposes no flash penalty compared to having only one of them - however, you have ZERO information on which one triggered it in this case, and if you need to figure that out, that cost you some time, which you need to weigh against the flash cost.

```c
ISR(PERIPHERAL0_INT_vect, ISR_ALIASOF(PERIPHERAL1_INT_vect));
ISR(PERIPHERAL1_INT_vect){
  /* handle PERIPHERAL0_INT and PERIPHERAL1_INT here */
}
```

`ISR_ALIASOF()` can point a vector at an `EMPTY_INTERRUPT` which saves 2 bytes of flash compared to each one set EMPTY_INTERRUPT individually.

## VPORTx.INTFLAGS is a WACKY register
The behaviour of |= and = are drastically different - but unlike other intflag registers, where there's never a correct time to use |=, because it's in the low I/O space, |= is the fastest way to clear a single bit, and behaves very differently from |= when more that one bit is specified. However, using the VPORT registers is always faster **unless** the port is not known at compile time. In that case, the VPORT will no longer act like a VPORT and will be used like a normal register. So **don't do that!** If the compiler cannot treat the address of the register as compile time known, use the PORTx.INTFLAGS, not the VPORTx.INTFLAGS. However, because inside of an interrupt is usually where you most care about speed, the VPORT.INTFLAGS register is most often used, rather than PORTx.INTFLAGS if either could be used.

* `VPORTA.INTFLAGS = 1 << 2;` this takes two clocks - 1 to load the constant '4' to a register, and one to write it to the flag register, and each of these take 1 word (2 bytes) of flash. It results in bit 2 being cleared.
* `VPORTA.INTFLAGS |= 1 << 2;` This takes only one clock, since it's a single bit set or clear on a low I/O register, and only word of flash. It results in bit 1 being cleared. This is smaller and faster than the above.
* `VPORTA.INTFLAGS = (1 << 2) | (1 << 1);` this takes two clocks - 1 to load the constant '6' to a register, and one to write it to the flag register, and each of these take 1 word (2 bytes) of flash. It results in bits 1 and 2 being cleared.
* `VPORTA.INTFLAGS |= (1 << 2) | (1 << 1);` This instead takes 3 clocks - 1 to read the register, 1 to OR it with the and 1 to write it back. It clears **ALL BITS** because a bit is either cleared already, or set, so it will be written 1 and cleared. The compiler has no idea that there's anything special about this register. Don't do this!
* `PORTA.INTFLAGS |= 1 << 2;` Unlike the VPORT intflags, writing the PORT intflags with |= even for one bit will still clear ever set bit. Don't do this!
* `PORTA.INTFLAGS = 1 << 2;` or `VPORTA.INTFLAGS |= (1 << 2) | (1 << 1);` This is the right way to set it using the PORTx.INTFLAGS, works with an bit pattern. But it takes 3 clocks and 3 words of flash instead of 1.



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
  * After 0.5-2ms (potentially as little as 512 system clocks on 1.3.6 and earlier, micros will jump backwards in time by around 0.5-2 ms. The threshold for this occurring and the size of the jump will depend on the system clock speed, millis timer, and what point in the cycle it is at when interrupts are disabled.


## A bit more on timing
So as described above, execution reaches the ISR within 6 system clock cycles (sometimes 5, now that rjmp is used instead of jmp when possible); Then the interrupt has to take special measures to save the state of what was interrupted, which it must restore at the end - this is automatically done by AVR-GCC at the beginning and end of an ISR (these are called the prologue and epilogue); the compiler must assume that every working register, plus the SREG, contains something that must be saved. These get saved by `push`ing them onto the stack. All of the needed registers are freed up this way before your code starts to actually execute (the more variables your ISR needs at a time, the more the prologue needs to `push` onto the stack, and the longer the delay before your code begins executing. At least on the modern AVRs, `push` is only 1 clock cycle (in the past, it was two). Of course, saving the state is only half of the job - after running the ISR that you wrote, the epilogue has to `pop` all those registers off the stack - this takes two clocks a piece. So while the datasheet loves to talk about 6 clocks to enter the interrupt routine - the bare minimum the compiler will produce is 10: 5 to get there assuming the compiler was able to use an rjmp to get there, then push r1, and r0, load SREG into one of those, push that onto the stack, and then zero out r1 (gcc needs a known zero register). So 10 clocks to enter. Then at the end pop the SREG value and store it, then pop the r0 and r1, then 4 clocks for the reti for 11 on the way out end.... plus the overhead of saving and restoring any registers your code uses. In DxCore or megaTinyCore, when a TCB is used for millis, all the ISR does is load the millisecond count, add 1 to it, and save it (that's all it does!). This requires 17 clocks in the prologue, 12 more to load the current millis tally, 4 to increment it, 8 more to save it, 3 to clear the interrupt flag, then the 4 registers we pushed need to be restored taking 8 clocks plus the 11 standard ones. That ISR winds up taking 17 + 27 + 19 = 63 clock cycles for a task that would take half that long outide of an ISR (note that a dedicated individual could implement that in assembly to save 2-3 clocks on the prologue, 4 in the body, and 4-6 in the epilogue - but that's beside the point, you can always look at generated assembly and find missed opportinities. The compiler misses more opportunities than usual in ISRs). 128k parts add 2 clocks to prologue and 3 to epilogue because they also have to save and restore RAMPZ (even if isn't changed by the ISR). This just makes it that much more important to be careful about ISRs, since execution time is amplified by the added constraints on ISRs.
One of the worst things is calling a function that won't end up being inlined or can't be optimized - like the "attachInterrupt" functions - in this case, the prologue + jump is minimum 24 clocks, and the epilogue 39, as it must assume the function uses all of the "call used" registers and save and restore them all.

### ISRs benefit the most from using the GPRs
If you're desperate for speed - or space - and if you need to set, clear or test a global flag, you can use one of the general purpose registers: GPIOR0/1/2/3. The only place the core uses any of those is at the very beginning of execution, when the reset cause is stashed in `GPIOR0` before the reset flags are cleared and the sketch is run (that way you can see what the reset cause was - while the 99% of users who do not need to check that flag benefit from having the reset flags cleared, because it enables the initialization code to turn a crash that might hang or result in incorrect behavior to instead simply reset cleanly  see [the reset reference](Ref_Reset.md)); the other 2 bits of GPIOR0 are used to record potential errors when a tuned internal clock is selected, again just so that user code has a way of accessing them. All of this is done before setup (ie, if you need it for this sort of thing, set it to 0 in setup).

To get the full benefit of the GPR's, you must use single-bit operations only, and be sure that the bit is known at compile time. These are lighning fast, and use no working registers at all. `GPIOR1 |= (1 << n)` where n is known at compile time, is a single clock operation which consumes no registers - it gets turned into a `sbi` - set bit index, with the register and bit being encoded by the opcode itself. The same goes for `GPIOR &= ~(1 << n)`  - these are also atomic (an interrupt couldn't interrupt them like it could a read-modify-write). There are analogous instructions that make things like `if(GPIOR1 & (1 << n))` and `if (!(GPIOR1 & (1 << n))` lightning fast. GPIOR's are only magic when manipulating a single bit, and the bit and must be known at compile time: `GPIOR1 |= 3` is a 3 clock non-atomic read-modify-write operation which needs a working register to store the intermediate value in while modifying it, which is just slightly faster than `MyGlobalByte |= 3`, which is a 6-clock non-atomic read-modify-write using 1 working register for the intermediate. `GPIOR1 |= 1; GPIOR1 |= 2;`, which achieves the same thing as GPIOR1 |= 3, is 2 clocks, each of which is an atomic operation which does not require a register to store any intermediate values. Note that atomicity is only a concern for code running outside the ISR, or code within a level 0 priority ISR when some other ISR is configured with level 1 priority. However, the fact that has no register dependence is a bigger deal in an ISR, because each working register used concurrently has to be saved at the start of the ISR, and restored at the end (total 3 clocks and 2 words). The most surprising thing is that `GPIOR1 = 3` is a 2 clock operation *which needs a working register*, that is, it would often cost 5 clock cycles, and could not be used as part of a naked ISR (next section) without assemble, however, `GPIOR1 |= 1; GPIOR1 |= 2;` can!

## Naked ISRs
One of the most advanced techniques relating to interrupts. This requires that either your ISR be written entirely in assembly, with your own prologue and epilogue hand optimized for your use case, or that you know for a fact that the tiny piece of C code you use doesn't change `SREG` or use any working registers.
In a naked ISR, all the compiler does for you is tell the linker to that this code is where the jmp or rjmp in the vector table. When writing a naked ISR, You are responsible for everything: prologue. epilogue, and the reti at the end. Don't forget the reti, without that you'll get crazy (and severely broken) behavior. So wby would you ever choose to do such a difficult method with such a large downside risk? Because it is tiny, and it is fast. An ISR which does nothing at all, (but not an EMPTY_ISR, which is the point of that macro) will, before the user-supplied ISR starts executing, at minimum, push r0 and r1 onto the stack, load SREG into r0, push that onto the stack, and then take the exclusive or of r1 with itself to clear it (since the compiler uses r1 as the "known zero" register) (4 clocks), after the end of your code, it will then pop that old SREG value back into r0, and store it to the SREG, and then pop r1 and r0 to restore their original values (7 clocks), before reti()'ing to return to wherever it was called from.

```c
/* OK - results in an sbi that neither changes SREG nor uses a register*/
ISR(PERIPHERAL_INT_vect, ISR_NAKED)
{
  GPIOR1 |= (1 << 0);
  reti();
}
/* The same interrupt without these (neither the ISR_NAKED nor the reti() ie, a normal interrupt),
 * would take an extra 11 clock cycles to run, and take up an extra 16 bytes of flash. That
 * usually isn't worth using a naked ISR. When you're on a 2k chip and the code overflows the flash
 * by less than 16 bytes, it matters a great deal. Similarly in cases where performance is of utmost
 * importance can demand this, recall that an interrupt takes around 5 to 6 clocks to get to, and 4
 * to get back from, so if simply setting a flag is all you need to do, it will halve the time it
 * spends doing the interrupt-related things. Such cases are rare indeed, but they do exist.
 * If you needed an interrupt that could run whie you were outputting data to a string of
 * neopixel type LEDs, for example, that would be possible using only a slightly modified
 * version of the tinyNeoPixel code (interrupts would need to be disabled between the first
 * and second calls to ST, but a *very* fast interrupt would not be fatal as long as it was prevented
 * from firing during a 0 bit while the data line was high)

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


`*` Those registers have, between 2015 and 2021 been called GPIORn. GPIOn, and GPIORn. At this rate, by 2030, we'll have
