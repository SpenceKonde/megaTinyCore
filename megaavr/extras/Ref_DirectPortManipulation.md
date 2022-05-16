# Direct Port Manipulation
When multiple i/o pins should be controlled simultaneously by the microcontroller, or when using Arduino functions, like digitalWrite and digitalRead, are too slow, port manipulation can be used to write directly to the registers.
Port registers allow for lower-level and faster manipulation of the i/o pins of the ATtiny. At least 1 port, and up to 3 ports are used in the ATtiny, where each port is indicated by a letter:
* PORTA
* PORTB
* PORTC

In the ensuing text, these are referred to as PORTx, where x represents a letter corresponding to a port (A, and B, C on most tinyAVR parts). Each port is linked with up to 8 pins, numbered 0 through 7. For example, on the ATtiny1616, Arduino Pin 0 is located on PORTA, bit 4, noted as PA4. The PORT registers on these modern AVR parts work a little differently from classic AVR microcontrollers (like ATmega328p, ATtiny85, etc), though the concept is the same.

We will also use the phrase "bit mask" and "bit position" - if I want to do something PB0, that is bit position 0. For port manipulation, that is generally not what we need. Instead we may need a "bit mask". A bit mask is a byte where all bits except the one in the specified position are 0, equivalent to `(1 << bit_position)` *warning* Always put parenthesis around the shift operators!

You will very often see constant names from the io headers used. Unlike the classic AVRs, they have been far more consistent and systematic. All constants that represent a bit mask end in `_bm` is a macro for a bit mask, and have a counterpart defined which ends in `_bp`. This is the case for all single bit bitfields. Where there is a group of bits, the abbreviations are `_gm` for group mask, and `_gc` for group code. *Warning: Group codes are enumerated types, not defines, and they cannot be tested for by the preprocessor. #if defined(SOMETHING_THAT_IS_VALID_gc) is never going to be true, even if the same group code works perfectly fine in actual code!*


PORTx.DIR is the register which determines if the pin is an input or an output (like DDRx registers in classic AVR). After reset, all pins are set to input (0 in the register). In order to use the pin as an output the bit in the register must be set to 1, which can be done as follows:

    PORTA.DIR = PIN4_bm

This will set bit 4 on PORTA - if using an ATtiny1616, for example, that would correspond to Arduino pin 0.
Multiple pins on the same port can be configured by writing each bit, or by setting multiple bitmasks

    PORTA.DIR = 0b01001000;         // sets PA3 and PA6 as an output
    PORTA.DIR = PIN3_bm | PIN6_bm;  // sets PA3 and PA6 as an output

Besides setting the pins to an input (0) or output (1) in the DIR register, you can also use the DIRSET and DIRCLR registers to set (set as output) or clear (set as input) a specific pin:

    PORTA.DIRSET = PIN4_bm; // use PA4 as an output
    PORTA.DIRCLR = PIN4_bm; // use PA4 as an input

You can even toggle between an input or output by writing to the DIRTGL register.

Turning the pin on and off is done with the PORTx.OUT register (this works like the PORTx register of classic AVR). Writing a 1 to the corresponding pin will set an output pin HIGH while a 0 will set it LOW:

    PORTA.OUT |=  PIN4_bm; // write PB4 high - Don't do it like this in real life!
    PORTA.OUT &= ~PIN4_bm; // write PB4 low - Don't do it like this in real life!

Note that the two examples above for flipping a single bit are not atomic - it's a read-modify-write operation, and will take between 4 and 8 clocks and between 3 and 7 words of flash. If an interrupt fires between the read and the write, the change that the ISR made will be reverted - if you have ISRs that are flipping pins (Servo.h does this), lines like those shown above would have to be be performed with interrupts disabled (just like classic AVR). Fortunately, the modern AVR architecture provides a better solution - the OUTSET and OUTCLR registers, just like the DIRSET and DIRCLR registers described above - this is atomic - as well as being faster and smaller

    PORTA.OUTSET = PIN4_bm; // turn PA4 output on - Atomic operation taking 2-3 words, 2-3 clocks
    PORTA.OUTCLR = PIN4_bm; // turn PA4 output off

Or when you just want to toggle the output you can use:

    PORTA.OUTTGL = PIN4_bm; // toggle PA4 output

**WARNING** These registers, when read, return the value of PORTx.OUT or PORTx.DIR! Using the |= and &= operators on them will NOT do what you expect. While the SET version is mostly harmless (just 3 clock cycles slower and 4 bytes larger), `PORTA.DIRCLR |= 1<<3;` will not set PA3 to an input... it will set ALL pins in PORTx to inputs! Depending on the surrounding code and hardware involved, the resultant behavior can be incredibly difficult to debug (See #227 for an example). Don't do that!

You can read the state of a pin by using the IN register (this is like the PINx register of classic AVR):

    bool status = PORTA.IN & PIN5_bm;

Unlike the classic AVRs, setting a pin HIGH with the OUT register while it is set as an input will not turn on the internal pullup. If you want to use the internal pullup resistor, you can set this in the PINnCTRL register as follows:

    PORTA.PIN6CTRL |= PORT_PULLUPEN_bm; // use the internal pullup resistor on PA6
    PORTA.PIN6CTRL &= ~PORT_PULLUPEN_bm; // don't use the internal pullup resistor on PA6

Note that this does mean that each pin has its own PINnCTRL register - unlike the classic AVRs where there was one register to control pullup for each port, with one bit per pin. The rest of the PINnCTRL register configures the "Input Sense Configuration", otherwise known as pin interrupts, as well as providing an way to disable the pin input buffer entirely to save power, and an option to invert the pin (some other parts may have additional advanced features here). Assuming you aren't using those, these are valid.

    PORTA.PIN6CTRL = PORT_PULLUPEN_bm; // use the internal pullup resistor on PA6
    PORTA.PIN6CTRL = 0; // don't use the internal pullup resistor on PA6




## Alternate names for registers

On the modern AVR parts the registers are normally accessed as members of a struct, as shown above (eg, PORTA is a struct, PORTA.OUT is a member of that struct). This can in some cases be problematic when porting code or under certain advanced use cases. The compiler libraries also provide flat names for all of these registers - replace the . with an _ - ie, PORTA.OUT is the same as PORTA_OUT.

## VPORTx registers

The normal port registers are at addresses starting at 0x400. This means they are outside the range of the `sbi`, `cbi`, and `out` instructions. This can be an issue when porting code, or when writing assembler for these parts. Fortunately, to address this, each port has 4 VPORT registers - VPORTx.OUT, VPORTx.IN, VPORTx.DIR, and VPORTx.INTFLAGS - these are aliases of the corresponding PORTx registers, and can be used with `sbi`, `cbi`, and `out`. Writing 1 to a bit in VPORTx.IN will toggle the pin state if the pin is an output.

## |= vs =
The |= and &= assignment operators (assigning the bitwise OR or bitwise AND of the current value to a register (or variable, but that's not the issue here)) have a very notable significance in the land of AVRs, and behave dramatically differently when used on the 32 registers at address 0 through 0x1F. Used on any other variable or register, they perform a read-modify-write cycle (ie, they are not atomic, if any interrupt modifies the same registers, you need to wrap them in MAKE_ATOMIC or cli/sei (or save SREG, cli, restore SREG if the function doing it might be called with interrupts disabled), and they are slower than simple assignment.

HOWEVER, on those magic 32 registers in the "low I/O space", provided you are setting or clearing a single bit, AND that bit is compile time known, they optimize to a single instruction: sbi or cbi (set or clear bit index), which is not only atomic, but runs in a single clock cycle (unlike the 2 on classic AVRs). That makes it faster than simple assignment, which requires two instructions (potentially 3 in unusual circumstances) - an LDI (load immediate) to put the value into a CPU register, and an OUT instruction to write it out (the single cycle IN and OUT instructions only work up to address 0x3F, above that the only available options are slower. So which are these magic registers? Unlike classic AVR devices, where a near-random assortment of registers, in addition to the pin manipulation ones, were placed in the low I/O space, on modern AVR devices, it is very simple: Only the VPORT registers and the four GPIO registers (which do nothing, they're for general application use). Similarly, there's also a single instruction branch for testing a single, compile time known bit in these registers.

So, while many examples of direct port writes for classic AVRs will use |= and &= for single bit flips, when using VPORT registers (Or when using the GPIO registers, but if you're using those, you're using them exclusively because of this, and know what you're doing). **There is never a right time to write to use |= or &= with any register in PORTx except for PINnCTRL** registers. It is always slower, and it is never interrupt safe. So don't do that. Specifically, as warned about above, you should NEVER use |= or &= with the PORTx.DIRCLR, PORTx.DIRSET, PORTx.OUTSET or PORTx.OUTCLR; the SETs are simply inefficient, while the CLRs produce unexpected behavior. The same hazard that applies to the DIRCLR and OUTCLR regisers also applies to INTFLAGS - it's a register where writing 1's clears bits instead of setting them, and the compiler does not know anything about them other than that the're 'volatile' and it is required to do exactly what you say.

## VPORTx.OUT vs PORTx.OUTSET/PORTx.OUTCLR
So, knowing all this, when is it more efficient to use `VPORTx.OUT |= PIN3_bm` and when is it more efficient to use `PORTx.OUTSET=PIN3_bm`?
* When PIN3_bm is known at compile time, using `VPORTx.OUT |= PIN3_bm` is faster, and consumes less flash (likely 2 bytes less flash, 1 clock cycle faster).
* If instead of referring to PB3 with `PIN3_bm` or `(1 << 3)`, or even `0x08` (`8` is also valid, but terrible practice, and 0x08 is not much better), a variable is used, it's value may not be compile time known. The compiler is sometimes very clever when performing "constant folding". But other times, it is quite the opposite, and will miss obvious chances. Unless you're sure it will be treated as a constant and `PORTx.OUTSET` should be used - `VPORTx.OUT |= 1 << not-a-constant` takes 4 bytes more flash, 2 more clock cycles **and is not atomic** - if an interrupt fires and modifies the register at just the wrong moment, between the read and the write, the value calculated from the pre-intterrupt read will be used the change that the interrupt made will be reversed.
* If the extreme performance of the VPORT access is not needed, considering the small penalty, is is likely better practice to use `PORTx.OUTSET` in any event, if your code is for public consumption, as it's purpose is much more immediately obvious to the uninitiated. Arduino users with little experience frequently reuse and modify sketches they find on the internet, and using the option that can safely deal with non-compiletime-known values reduces the likelihood of such careless modifications producing strange bugs. That in turn benefits the experienced Arduino users who end up helping them on the forums.


## Equivalents to classic AVR registers
Classic AVR |  modern AVR |  VPORT
------------ | ------------- | -------------
PORTx | PORTx.OUT | VPORTx.OUT
PINx  | PORTx.IN | VPORTx.IN
DDRx  | PORTx.DIR | VPORTx.DIR

**NOTE** Unlike classic AVRs, setting the bit in PORTx.OUT while pin is set as an INPUT will *NOT* enable the pullups. Only the PORTx.PINnCTRL registers can do that. There is no VPORT register that allows changing pullup status. Writing to PORTx.IN does *NOT* toggle the output of the bit, but writing to VPORTx.IN *DOES*.


## Closing remarks
Issues submitted have made it clear that the above doesn't seem to make it clear enough.

Below are excerpts from my responses to such inquiries
For setting or clearing a single, fixed bit, in the PORT direction or PORT output, clearing a bit in PORT interrupt flag registers, or for toggling the PORT output, in the case where both the PORT and the bit are constant and known at compile time (values passed to class constructors or methods of class members are never compile time known as far as the optimizer is concerned), the most efficient way is with the VPORT registers:

```c
// Right:
VPORTA.DIR      |=   1 << bit position; // Set direction to output
VPORTA.DIR      &= ~(1 << bit position); // Clear direction (set to input)
VPORTA.OUT      |=   1 << bit position; // Set output value (HIGH output)
VPORTA.OUT      &= ~(1 << bit position); // Clear output value (LOW output)
VPORTA.IN       |=   1 << bit position; // Write a 1 to to the VPORT input register to toggle.
VPORTA.INTFLAGS |=   1 << bit position; // Write a 1 to to INTFLAGS to clear the flag.
// These are all an atomic, single clock cycle, single instruction operation. (CBI/SBI)

// Wrong (strictly inferior):
VPORTA.DIR      |= (1 << bit position) | (1 << other bit position);
// SBI or CBI is only generated for setting or clearing a SINGLE BIT.
// The above generates the exact same instructions as:
// uint8_t temp = VPORTA.DIR;
// temp |= (1 << bit position) | (1 << other bit position);
// VPORTA.DIR = temp;
//
// It is not atomic - if an interrupt occurs 1 clock in, the changes that it
// made will be undone, because the temp variable doesn't reflect them.
// It takes 6 bytes of flash.
// It executes in 3 clock cycles.
//
// Two consecutive writes with |= can be used, of a single bit each can be used.
// Those will each be atomic. It will take 4 bytes of flash and execute in 2 clocks.
// That is the most efficient way to set or clear two bits one of these port related registers.
// For setting or clearing more bits, that method becomes bulky, and slow, not to mention hard to read.
// See PORTx.___SET/CLR for the more flexible solution.

// Very Wrong:
uint8_t bit = findBitToSet(); // bit value is not constant
VPORTA.OUT |= (1 << bit);

// This will generate the same 3 clock code as above - only it is insidious, because it passes a quick inspection.
// **The bit to be set or cleared must be known at compile time**
// As far as the code generated, it is equivalent, and just as wrong, if instead of loading bit from a function that
// is not a constant, you tried to use a class member variable, or anything else.

// (likely) Wrongest:

*myVPORT |= (1 << bit_to_set)

// The register must be known at compile time. If the above has myVPORT #defined as (&VPORTA.OUT) for example, or
// as ((volatile uint8_t *)(0x0001)), it's fine. But if (as the naming suggests) that's in a method of a class where
// the port was passed to the constructor, or is for any other reason not compile time known,
// that will not give you SBI/CBI. In fact, for the case of the method of a class, it will result in:

// * Several load instructions (LDS) to get the address of the port register. 6 clocks (3/each)
// * an indirect load (LD) to load the value into a working register (ex, r0) 2 clocks
// * an instruction to set the bit (ORI) in the working register  1 clock
// * an indirect store (ST) to write the value in the working register to the VPORT.OUT register.  1 clock
//
// What you hoped would be a 1 clock atomic operation that takes 2 bytes of flash is instead
// likely a 10 clock, 14 byte routine that has a 3 clock window during which an interrupt could corrupt the result!
```

For the special cases of toggling the output, that is achieved by writing a 1 to the corresponding bit of the VPORT.IN register. It is important to note that PORTx.IN simply ignores writes, only VPORTx.IN has that magic.

For the case of INTFLAGS, as stated in the datasheet, they are CLEARED by writing a ONE to them. This works with bit-level writes (`|=`) or simple assignment (`=`), and the same requirement that for bit level writes, the register and bit must be known at compile time and only one flag can be cleared at a time.


The above-described manipulations where a single bit is toggled are identical to digitalWriteFast() and pinModeFast() with a constant output or mode.

The equivalent of `if (digitalReadFast())` is:
```
if (VPORTx.IN & (1 << bit))
```
or
```
if (!(VPORTx.IN & (1 << bit)))

```

And the bit must be constant and known at compile time - same as the rest of this stuff.

The special case of testing against a low I/O registers and ORing with a single bit generates an sbic/sbis instruction (" **S**kip if **B**it in **I**/O register **S**et/**C**leared") makes it possible branch based the state of a pin (or intflag) within a single clock cycle (or two or three depending on how you count).


As you can see, while blazing fast, and automatically atomic - this method of setting or clearing bits is applicable only in narrow cases.
It also **only works on the above listed registers** plus the four special `GPIOR` or `GPR` registers.

On ALL other registers, |= and &= will ALWAYS produce a read-modify-write, and the load will take 2 or 3 clock cycles, and the store 1 or 2.

A more flexible, and consistent method for controlling the port registers can be achieved with the PORTx registers, however, when you have the information to use the VPORT method, the PORT registers are slower than the VPORT registers (it takes 2 clock cycles, plus 1 or more to prepare the value to write).

These registers are:
```c
PORTx.DIR
PORTx.DIRSET
PORTx.DIRCLR
PORTx.DIRTGL
PORTx.OUT
PORTx.OUTSET
PORTx.OUTCLR
PORTx.OUTTGL
PORTx.IN
PORTx.INTFLAGS
```
The DIR, OUT, IN, and INTFLAGS registers have the same functionality as the VPORTx registers of the same names, but they are slower to access.

The `____SET` `____CLR`, `and ___TGL` registers are *special*.  When written, all bits corresponding to the bits written 1 are either set, cleared or toggled.

In effect these:
```c
PORTx.DIRSET = (1 << 5) | (1 << 3);
PORTx.DIRCLR = (1 << 5) | (1 << 3);
PORTx.DIRTGL = (1 << 5) | (1 << 3);
```
Is is a magically fast, atomic version of these:
```c
PORTx.DIR |= (1 << 5) | (1 << 3);
PORTx.DIR &= (1 << 5) | (1 << 3);
PORTx.DIR ^= (1 << 5) | (1 << 3);
```

These are the **most misunderstood registers** on the modern AVR parts, and it is extremely common for even experienced users who are comfortable working with direct port writes to use them incorrectly and get confused by the results. The problem lies in a combination of habit and a detail barely mentioned in the datasheet:

**WHEN YOU READ FROM ANY OF THE ___SET, ___CLR, or ___TGL REGISTERS, YOU GET THE VALUE IN THE CORRESPONDING REGISTER** - which means that you should **NEVER USE THOSE REGISTERS WITH &=, |=, ^=** nor do the equivalent by using simple assignment to write a value that is the bitwise combination of the register's current value and something else.

Using `___SET` in this way does nothing untoward, though it is slower and not atomic.

Not so DIRCLR - If you do `PORTx.DIRCLR |= (1 << 2) | (1 << 3)`. The compiler will then do a read modify write cycle. It will read the DIRCLR register, the current value of PORTx.DIR. Then that will bitwise OR'ed with the values you passed. It now has a temporary value in a register with each bit that is set in PORTx.DIR as well as bits 2 and 3 set. And then it will write that to PORTx.DIRCLR. This will result in EVERY BIT CURRENTLY SET BEING CLEARED!!

The behavior of the TGL registers is even stranger: if used with |= they'll turn clear all bits set (by toggling them) and toggle the requested bit. And if you foolishly use ^= with TGL, it will set the specified bit and clear all other set bits.

None of that behavior is useful. Only use simple assignment with the SET/CLR/TGL registers.

### A sidenote: INTFLAGS
While writing this, I realized that essentially, INTFLAGS is like a  `___CLR` register for a hidden register that you can't otherwise interact with.

Elsewhere on these parts, aside from the INTFLAGS registers that most peripherals have, and the reset flag register, there are very few registers like this. TCA's have CTRLESET/CTRLECLR (and CTRLFSET/CTRLFCLR) (and no corresponding base register - all access is by reading the SET or CLR registers), and the Dx-series parts have the multi-pin config registers which are similar to but distinct from this type of register.
