### List of interrupt vector names

If there is a list of the names defined for the interrupt vectors is present somewhere in the datasheet, I was never able to find it.  These are the possible names for interrupt vectors on the parts supported by megaTinyCore. Not all parts will have all interrupts, but where not all parts in a series have it, I've tried to mark that. An ISR is created with the ISR() macro.

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
| TCB0_INT_vect     | X | X | X | Depends on mode | Two flags on 2 series. 8 modes on all       |
| TCB1_INT_vect     |   | * | X | Depends on mode | 1-series with 16k or 32k flash only         |
| TCD0_OVF_vect     |   | X |   | Manually        |                                             |
| TCD0_TRIG_vect    |   | X |   | Manually        |                                             |
| TWI0_TWIM_vect    | X | X | X | Usually Auto    | See datasheet for list of what clears it    |
| TWI0_TWIS_vect    | X | X | X | Usually Auto    | See datasheet for list of what clears it    |
| USART0_DRE_vect   | X | X | X | Write/Disable   | ISR must write data or disable interrupt    |
| USART0_RXC_vect   | X | X | X | RXCIF, on read  | Error flags, if enabled only clear manually |
| USART0_TXC_vect   | X | X | X | Manually        |                                             |

#### Clearing flags - why so complicated?
Almost all flags *can* be manually cleared - the ones that can be cleared automatically generally do that to be helpful:
* when the purpose of the flag is to tell you that something is ready to be read, reading it clears the flag. ADC, serial interfaces, and TCB input capture do that.
* The TWI interrupts work the same way - you need to read, write, or ack/nack something to respond to the bus event; doing so clears the flag too.
* Sometimes interrupts like that can have error flags that can trigger them enabled too; those typically have to be manually cleared - by enabling them, you declare an intent to do something about them, so you're responsible for telling the hardware you did it.
* USART, and buffered SPI have DRE interrupt that can only be cleared by writing more data - otherwise you need to disable the interurpt from within the ISR. The TXC (transfer/transmit complete) flags are freqently polled rather than used to fire interrupts. It's not entirley clear from the datasheet if the EEPROM ready interrupt is like that, or can be cleared manually.
* The NMI is a very special interrupt - on tinyAVR it can only be triggered by CRC check failure; it can be configured to be a normal interrupt *or* a Non-maskable Interrupt. In NMI mode, the part will sit there running the interrupt instead of almost-working with damaged firmware, potentially creating a dangerous situation when it fails at a critical moment. No matter what the damaged firmware tries to do, it cannot disable or bypass the NMI. Onlu loading working firmware and resetting it will clear the NMI. This is of particular relevance in life-safety-critical applications which these parts (but NOT this software package nor Arduino in genberal) are certified for. Not something likely to be used in Arduino-land.

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

#### Assorted notes
* Despite the fact that no PORTB/C_PORT_vect is defined for parts without those ports, there are unused vector slots where they would go on all parts. Same goes for the TCD0 vectors, even on 0-series parts.
* The 5 vectors associated with the "good" 1-series parts take up vector slots on all parts with at least 8k of flash, but not on smaller ones; This means that hex files are not binary-compatible between 4k and 8k parts if they define interrupt vectors above the auspicious interrupt number 13. Note that binaries compiled for 8k or smaller parts, and for 16k and larger parts is never compatible if it uses any interrupts, because the vector size changes when you pass 8k. It strikes me as peciliar that the 8k parts waste that extra 10 bytes on vectors that no 8k parts use. The Optiboot_x binaries included with megaTinyCore *are* portable across the entire product line because they do not make use of interrupts.
