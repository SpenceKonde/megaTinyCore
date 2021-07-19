# Interrupt vectors

If there is a list of the names defined for the interrupt vectors present somewhere in the datasheet, I was never able to find it. These are the names for interrupt vectors on the parts supported by megaTinyCore. Not all parts will have all interrupts, but where not all parts in a series have it, I've tried to mark that. An ISR is created with the ISR() macro.

**WARNING** If you misspell the name of a vector, you will get a compiler warning BUT NOT AN ERROR! Hence, you can upload the bad code... in this case the chip will freeze the instant the ISR is called, as it jumps to BAD_ISR, which in turn jumps to the reset vector... but since the interrupt flag never gets cleared, as soon as interrupts are enabled, it will do it all over again, so the chip will be hung. Encountering this (and the annoying lack of a nice list anywhere outside of the io.h) was the impetus for creating this list.


| Vector Name       | 0 | 1 | 2 | Flag Cleared on | Notes                                       |
|-------------------|---|---|---|-----------------|---------------------------------------------|
| AC0_AC_vect       | X | X | X | Manually        |                                             |
| AC1_AC_vect       |   | * |   | Manually        | 1-series with 16k or 32k flash only         |
| AC2_AC_vect       |   | * |   | Manually        | 1-series with 16k or 32k flash only         |
| ADC0_ERROR_vect   |   |   | X | Manually        | Multiple flags for different errors         |
| ADC0_RESRDY_vect  | X | X | X | Read result reg |                                             |
| ADC0_SAMPRDY_vect |   |   | X | Read sample reg | WCOMP uses SAMPRDY or RESRDY on 2-series    |
| ADC0_WCOMP_vect   | X | X |   | Manually        | But gets it's own on 0/1-series             |
| ADC1_RESRDY_vect  |   | * |   | Read result reg | 1-series with 16k or 32k flash only         |
| ADC1_WCOMP_vect   |   | * |   | Manually        | 1-series with 16k or 32k flash only         |
| BOD_VLM_vect      | X | X | X | Manually(?)     |                                             |
| CCL_CCL_vect      |   |   | X | Manually        | 1 shared vector for whole CCL               |
| CRCSCAN_NMI_vect  | X | X | X | Reset           | NMI guarantees device stopped if CRC fail   |
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
| TCB0_INT_vect     | X | X | X | Depends on mode | Two flags on 2 series only. Behavior depends on mode, see datasheet |      |
| TCB1_INT_vect     |   | * | X | Depends on mode | 1-series with 16k or 32k flash or 2-series  |
| TCD0_OVF_vect     |   | X |   | Manually        |                                             |
| TCD0_TRIG_vect    |   | X |   | Manually        |                                             |
| TWI0_TWIM_vect    | X | X | X | Usually Auto    | See datasheet, flag clearing is complicated |
| TWI0_TWIS_vect    | X | X | X | Usually Auto    | See datasheet, flag clearing is complicated |
| USART0_DRE_vect   | X | X | X | Write/Disable   | ISR must write data or disable interrupt    |
| USART0_RXC_vect   | X | X | X | RXCIF, on read  | Error flags, if enabled, only cleared manually |
| USART0_TXC_vect   | X | X | X | Manually        | Often used without the interrupt enabled    |

#### Clearing flags - why it's so complicated
Almost all flags *can* be manually cleared - the ones that can be cleared automatically generally do that to be helpful. The datasheet will explicitly list under what conditions a flag is cleared.
* When the purpose of the flag is to tell you that something is ready to be read, reading it clears the flag. ADC, serial interfaces, and TCB input capture do that.
* The TWI interrupts work the same way - you need to read, write, or ack/nack something to respond to the bus event; doing so clears the flag too.
* Sometimes interrupts like that can have error flags that can trigger them enabled too; those typically have to be manually cleared - by enabling them, you declare an intent to do something about them, so you're responsible for telling the hardware you did it.
* USART, and SPI in buffered mode, have DRE interrupts whose flag can only be cleared by writing more data - otherwise you need to disable the interurpt from within the ISR. The TXC (transfer/transmit complete) flags are freqently polled rather than used to fire interrupts. It's not entirley clear from the datasheet if the EEPROM ready interrupt is like the DRE ones, or can be cleared manually.
* The NMI is a very special interrupt - on tinyAVR it can only be triggered by `CRCSCAN` failure. It can be configured to be a normal interrupt *or* a so called non-maskable interrupt. In NMI mode, the part will sit there running the interrupt handler instead of running the corrupted firmware. This is very important for industrial users, who put these parts into cars or safety-critical equipment, because the corrupted part of the firmware might be the part that only gets called in a life-safety-critical moment. For example, imagine an airbag control chip in a car - the car could just ask it if it was working but without performing a CRC check on the flash, there would be no way to tell whether it's *actually* working - only that the part of the code which responds to the inquiries from the vehicles computer was. This lets you positively ensure that such partial functionality situations can never happen due to flash corruption or programming failure. The software can of course still have (literally) fatal bugs - but if you're making airbag controllers, you've got review teams and processes in place to checm that too. No matter what the damaged firmware tries to do, it cannot disable or bypass the NMI. Only loading working firmware and resetting it will clear the NMI. *Neither this software package nor Arduino in general are certified for or should ever be used for life-safety-critical applications* so this is not something likely to be or seen in Arduino-land.

### Example
```c++
ISR(PORTA_PORT_vect) {
  //ISR code goes here
}
```
### Reminders
* ISRs should run FAST. Minimize the time that the code spends in the ISR. Never use polling loops, and avoid writing to serial. Most interrupts should just set a flag that is checked elsewhere, and do what must be done that instant (eg, reading from certain hardware registers).
* Read the datasheet, particularly relating to the relevant INTFLAGS register - some interrupt flags are not automatically cleared when the ISR runs, and if these are not cleared, it will trigger continually as soon as it is triggered once.
* Any global variable that an interrupt changes, if used outside the ISR, must be declared volatile - otherwise the compiler may optimize away access to it, resulting in code elsewhere not knowing that it was changed by the ISR.
* Any global variable read by the ISR and written to by code outside of the ISR which are larger than a byte must be written to with interrupts disabled - if the interrupt triggers in the middle of a write, the ISR would see a corrupted value.

### Empty interrupts
Often, you may find yourself wanting an "empty" interrupt, with no body; this is particularly common with interrupts that wake from sleep (though since these parts often require manually clearing interrupt flags, this is less common than on classic AVRs). The naive way to get an empty interrupt works fine, but it's inefficient, and on a 2k part, the overhead amounts to 0.9% of the total flash available! These examples below show the efficient way to do it.
```c++
ISR(SOME_INT_vect) {
  // 5-6 clocks from interrupt flag being set to code execution reaching the ISR.
  // Do nothing
} // This actually generates 20 bytes of code taking 16 cycles to execute for a total of 20 bytes and 21-22 clocks!

EMPTY_INTERRUPT(SOME_INT_vect); // this generates 2 bytes if code, just the single RETI instruction, so execution will be back to where it was before the interrupt condition within only 9-10 clocks, and saving 18 bytes of flash and a dozen clocks!

/* Almost-empty interrupt:
     For the very special case where there is only one pin on a port with the pin change interrupt enabled, you
     can use a "naked" ISR, and code which you know can execute without disrupting the state of any working
     registers. Here is an example for a pin interrupt which one might use to wake from sleep.
     You MUST use VPORTx.INTFLAGS... not PORTx.INTFLAGS. */
ISR(PORTx_PORT_vect, ISR_NAKED) {
                                    // 5-6 clocks to reach the ISR
  VPORTx.INTFLAGS |= 1 << bit_pos;  // 1 word,  1 clock. Clear the interrupt flag by writing 1 to it with sbi.
  reti();                           // 1 word,  4 clocks. Return from the interrupt.
}                   // This results in 2 words, 5 clocks. Total execution time (5 + (5 or 6) = ) 10 or 11 clocks
/*This can be extended only slightly before things get much more complicated....

  The only things you can do within a naked interrupt without writing inline assembly are:
  - VPORTx.OUT, VPORTx.DIR, VPORTx.IN, VPORTx.INTFLAGS, or GPIORn |= 1 << constant.
  - VPORTx.OUT, VPORTx.DIR, or GPIORn &= ~(1 << constant).
  - if (VPORTx.OUT, VPORTx.DIR, VPORTx.IN, VPORTx.INTFLAGS, or GPIORn & (1 << constant)) {other allowed statement(s);}.
  - if (!(VPORTx.OUT, VPORTx.DIR, VPORTx.IN, VPORTx.INTFLAGS, or GPIORn & (1 << constant))) {other allowed statement(s);}.
  - You must end it with reti();

  Departing even slightly from this list leaves you in nasal-demon territory!

  Behavior will be undefined which is overwhelmingly likely to manifest as a complete and total failure of the impacted
  sketch the instant the ISR first fires. I chose to include this only due to the extremne frequency with which one
  will find themselves needing the sort of nearly empty ISR shown above to wake from sleep, and the magnitude of the
  improvement in efficiency from such a small change. */

```
### One ISR for multiple interrupts
If you have one ISR that needs to be used for more than one vector, there's also a simple macro which imposes ZERO overhead for the second and further vectors which call the same ISR.
```c++

ISR(SOME_INT_vect) {
  // Do some stuff in the ISR
  // The code here is able to handle interrupts from either SOME_INT_vect or OTHER_INT_vect.
}

ISR_ALIAS(OTHER_INT_vect, SOME_INT_vect); // Zero overhead to point, it will simply point that vector to the same ISR as the other.
```

### Assorted notes
* Despite the fact that no PORTB/C_PORT_vect is defined for parts without those ports, there are unused vector slots where they would go on all parts. Same goes for the TCD0 vectors, even on 0-series parts.
* The 5 vectors associated with the "good" 1-series parts take up vector slots on all parts with at least 8k of flash, but not on smaller ones; This means that hex files are not binary-compatible between 4k and 8k parts if they define interrupt vectors above the auspicious interrupt number 13. Note that binaries compiled for 8k or smaller parts, and for 16k and larger parts is never compatible if it uses any interrupts, because the vector size changes when you pass 8k. It strikes me as peciliar that the 8k parts waste that extra 10 bytes on vectors that no 8k parts use. The Optiboot_x binaries included with megaTinyCore *are* portable across the entire product line because they do not make use of interrupts.
