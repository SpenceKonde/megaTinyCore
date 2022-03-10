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
* When PIN3_bm is known at compile time, using `VPORTx.OUT |= PIN3_bm`is faster, and consumes less flash (likely 2 bytes less flash, 1 clock cycle faster).
* If instead of referring to PB3 with `PIN3_bm` or `(1 << 3)`, or even `0x08` (`8` is also valid, but terrible practice, and 0x08 is not much better), a variable is used, it's value may not be compile time known. The compiler is sometimes very clever when performing "constant folding". But other times, it is quite the opposite, and will miss obvious chances. Unless you're sure it will be treated as a constant and `PORTx.OUTSET` should be used - `VPORTx.OUT |= 1 << not-a-constant` takes 4 bytes more flash, 2 more clock cycles **and is not atomic** - if an interrupt fires and modifies the register at just the wrong moment, between the read and the write, the value calculated from the pre-intterrupt read will be used the change that the interrupt made will be reversed.
* If the extreme performance of the VPORT access is not needed, considering the small penalty, is is likely better practice to use `PORTx.OUTSET` in any event, if your code is for public consumption, as it's purpose is much more immediately obvious to the uninitiated. Arduino users with little experience frequently reuse and modify sketches they find on the internet, and using the option that can safely deal with non-compiletime-known values reduces the likelihood of such careless modifications producing strange bugs. That in turn benefits the experienced Arduino users who end up helping them on the forums.


## Equivalents to classic AVR registers
Classic AVR |  modern AVR |  VPORT
------------ | ------------- | -------------
PORTx | PORTx.OUT | VPORTx.OUT
PINx  | PORTx.IN | VPORTx.IN
DDRx  | PORTx.DIR | VPORTx.DIR

**NOTE** Unlike classic AVRs, setting the bit in PORTx.OUT while pin is set as an INPUT will *NOT* enable the pullups. Only the PORTx.PINnCTRL registers can do that. There is no VPORT register that allows changing pullup status. Writing to PORTx.IN does *NOT* toggle the output of the bit, but writing to VPORTx.IN *DOES*.
