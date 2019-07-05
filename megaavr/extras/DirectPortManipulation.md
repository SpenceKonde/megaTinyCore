# Direct Port Manipulation
When multiple i/o pins should be controlled simultaneous by the microcontroller, or when using the Arduino functions, like digitalWrite and digitalRead, are too slow, port manipulation can be used to write directly to the registers.
Port registers allow for lower-level and faster manipulation of the i/o pins of the ATtiny. At least 1 port, and up to 3 ports are used in the ATtiny, where each port is indicated by a letter:
*	PORTA
*	PORTB
*	PORTC

Each port is a register of 8 bits and these bits are linked to a pin on the microcontroller. For example, on the ATtiny1616 the first i/o pin is located on PORTA, bit 4, noted as PA4.
The macro’s in the ATtiny tool pack allow easier to understand code for the port manipulation and differ slightly from the existing ATmega. 
DIR is the register which determines if the pin is an input or an output. Standard all pins are set to input (0 in the register). In order to use the pin as an output the bit in the register must be set to 1, which can be done as follows:

    PORTA.DIR = PIN4_bm

This will set bit 4 on PORTA, which is the first i/o on the ATtiny1616
Multiple pins on the same port can be configured by writing each bit, or by setting multiple bitmasks (_bm is a macro for a bit mask):

    PORTA.DIR = 0b01001000;         // sets pin 3 and 6 as an output
    PORTA.DIR = PIN3_bm | PIN6_bm;  // sets pin 3 and 6 as an output

Besides setting the pins to an input (0) and output (1) in the DIR register, you can also use the DIRSET register to determine if a pin is an output, and DIRCLR when the pin is an input:

    PORTA.DIRSET = PIN4_bm; // use PA4 as an output
    PORTA.CLRSET = PIN4_bm; // use PA4 as an input

You can even toggle between an input and output by writing to the DIRTGL register.

Turning the pin on and off is done with the OUT register. Writing a 1 to the corresponding pin will turn the output on while a 0 will turn if off. An even better solution is using the OUTSET and OUTCLR registers, just like the DIRSET and DIRCLR registers above, as follows: 

    PORTA.OUTSET = PIN4_bm; // turn PA4 output on
    PORTA.OUTCLR = PIN4_bm; // turn PA4 output off

Or when you just want to toggle the output you can use:

    PORTA.OUTTGL = PIN4_bm; // toggle PA4 output

Reading an input pin can be done with the IN register. For example, when PA5 is configured as an input pin:

    PORTA.CLRSET = PIN5_bm; // use PA5 as an input
    
The actual status can be read with:

    bool status = PORTA.IN & PIN5_bm;

If you want to use the internal pullup resistor on this input pin you can set this in the PINnCTRL register as follows:

    PORTA.PIN5CTR = PORT_PULLUPEN_bm; // use the insertnal pullup resistor

