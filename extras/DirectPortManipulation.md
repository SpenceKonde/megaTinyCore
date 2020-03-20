# Direct Port Manipulation
When multiple i/o pins should be controlled simultaneously by the microcontroller, or when using Arduino functions, like digitalWrite and digitalRead, are too slow, port manipulation can be used to write directly to the registers.
Port registers allow for lower-level and faster manipulation of the i/o pins of the ATtiny. At least 1 port, and up to 3 ports are used in the ATtiny, where each port is indicated by a letter:
*	PORTA
*	PORTB
*	PORTC

Each port is linked with up to 8 pins, numbered 0 through 7. For example, on the ATtiny1616 the first i/o pin is located on PORTA, bit 4, noted as PA4. The PORT registers on the megaAVR parts work a little differently from classic AVR microcontrollers (like ATmega328p, ATtiny85, etc), though the concept is the same. 


DIR is the register which determines if the pin is an input or an output (like DDRx registers in classic AVR). After reset, all pins are set to input (0 in the register). In order to use the pin as an output the bit in the register must be set to 1, which can be done as follows:

    PORTA.DIR = PIN4_bm

This will set bit 4 on PORTA - if using an ATtiny1616, for example, that would correspond to Arduino pin 0. 
Multiple pins on the same port can be configured by writing each bit, or by setting multiple bitmasks (_bm is a macro for a bit mask):

    PORTA.DIR = 0b01001000;         // sets PA3 and PA6 as an output
    PORTA.DIR = PIN3_bm | PIN6_bm;  // sets PA3 and PA6 as an output

Besides setting the pins to an input (0) or output (1) in the DIR register, you can also use the DIRSET and DIRCLR registers to set (set as output) or clear (set as input) a specific pin:

    PORTA.DIRSET = PIN4_bm; // use PA4 as an output
    PORTA.DIRCLR = PIN4_bm; // use PA4 as an input

You can even toggle between an input or output by writing to the DIRTGL register.

Turning the pin on and off is done with the OUT register (this works like the PORTx register of classic AVR). Writing a 1 to the corresponding pin will set an output pin HIGH while a 0 will set it LOW:
    
    PORTA.OUT |= PIN4_bm; // write PB4 high
    PORTA.OUT &=~PIN4_bm; // write PB4 low
    
Note that the two examples above for flipping a single bit are not atomic - it's a read-modify-write operation. If an interrupt fires between the read and the write, the change that the ISR made will be reverted - if you have ISRs that are flipping pins (Servo.h does this), lines like those shown above would have to be be performed with interrupts disabled (just like classic AVR). Fortunately, the megaAVR architecture has a better solution - the OUTSET and OUTCLR registers, just like the DIRSET and DIRCLR registers described above - this is atomic:

    PORTA.OUTSET = PIN4_bm; // turn PA4 output on
    PORTA.OUTCLR = PIN4_bm; // turn PA4 output off

Or when you just want to toggle the output you can use:

    PORTA.OUTTGL = PIN4_bm; // toggle PA4 output

You can read the state of a pin by using the IN register (this is like the PINx register of classic AVR):

    bool status = PORTA.IN & PIN5_bm;

Unlike the classic AVRs, setting a pin HIGH with the OUT register while it is set as an input will not turn on the internal pullup. If you want to use the internal pullup resistor, you can set this in the PINnCTRL register as follows:

    PORTA.PIN6CTRL |= PORT_PULLUPEN_bm; // use the internal pullup resistor on PA6
    PORTA.PIN6CTRL &= ~PORT_PULLUPEN_bm; // don't use the internal pullup resistor on PA6

Note that this does mean that each pin has its own PINnCTRL register - unlike the classic AVRs where there was one register to control pullup for each port, with one bit per pin. The rest of the PINnCTRL register is used to configure pin interrupts on the pin. If not using pin interrupts with this pin, you can do: 

    PORTA.PIN6CTRL = PORT_PULLUPEN_bm; // use the internal pullup resistor on PA6
    PORTA.PIN6CTRL = 0; // don't use the internal pullup resistor on PA6
    
## Alternate names for registers

On the new megaAVR parts (as shown above), the registers are normally accessed as members of a struct (eg, PORTA is a struct, PORTA.OUT is a member of that struct). This can in some cases be problematic when porting code. The compiler libraries also provide flat names for all of these registers - replace the . with an _ - ie, PORTA.OUT is the same as PORTA_OUT.

## VPORT registers

The normal port registers are at addresses starting at 0x400. This means they are outside the range of the `sbi`, `cbi`, and `out` instructions. This can be an issue when porting code, or when writing assembler for these parts. Fortunately, to address this, each port has 4 VPORT registers - VPORTx.OUT, VPORTx.IN, VPORTx.DIR, and VPORTx.INTFLAGS - these are aliases of the corresponding PORTx registers, and can be used with `sbi`, `cbi`, and `out`. Writing 1 to a bit in VPORTx.IN will toggle the pin state if the pin is an output. 

## Equivalents to classic AVR registers
Classic AVR |  megaAVR | megaAVR VPORT
------------ | ------------- | -------------
PORTx | PORTx.OUT | VPORTx.OUT
PINx  | PORTx.IN | VPORTx.IN
DDRx  | PORTx.DIR | VPORTx.DIR

**NOTE** Unlike classic AVRs, setting the bit in PORTx.OUT while pin is set as an INPUT will *NOT* enable the pullups. Only the PORTx.PINnCTRL registers can do that. There is no VPORT register that allows changing pullup status. 
