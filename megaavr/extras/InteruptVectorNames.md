### List of interrupt vector names

If there is a list of the names defined for the interrupt vectors is present somewhere in the datasheet, I was never able to find it. These are the possible names for interrupt vectors on the parts supported by megaTinyCore. Not all parts will have all interrupts. An ISR is created with the ISR() macro.

**WARNING** If you misspell the name of a vector, you will get a compiler warning BUT NOT AN ERROR! Hence, you can upload the bad code... in this case the chip will freeze the instant the ISR is called, as it jumps to BAD_ISR, which in turn jumps to the reset vector... but since the interrupt flag never gets cleared, as soon as interrupts are enabled, it will do it all over again, so the chip will be hung.


* CRCSCAN_NMI_vect
* BOD_VLM_vect
* PORTA_PORT_vect
* PORTB_PORT_vect
* PORTC_PORT_vect
* RTC_CNT_vect
* RTC_PIT_vect
* TCA0_OVF_vect
* TCA0_LUNF_vect
* TCA0_HUNF_vect
* TCA0_LCMP0_vect
* TCA0_CMP0_vect
* TCA0_LCMP1_vect
* TCA0_CMP1_vect
* TCA0_CMP2_vect
* TCA0_LCMP2_vect
* TCB0_INT_vect
* TCB1_INT_vect
* TCD0_OVF_vect
* TCD0_TRIG_vect
* AC0_AC_vect
* AC1_AC_vect
* AC2_AC_vect
* ADC0_RESRDY_vect
* ADC0_WCOMP_vect
* ADC1_RESRDY_vect
* ADC1_WCOMP_vect
* TWI0_TWIS_vect
* TWI0_TWIM_vect
* SPI0_INT_vect
* USART0_RXC_vect
* USART0_DRE_vect
* USART0_TXC_vect
* NVMCTRL_EE_vect

### Example

```
ISR(PORTA_PORT_vect) {
  //ISR code goes here
}
```
### Reminders
* ISRs should run FAST. Minimize the time that the code spends in the ISR. Never use polling loops, and avoid writing to serial. Most interrupts should just set a flag that is checked elsewhere, and do what must be done that instant (eg, reading from certain hardware registers). 
* Read the datasheet, particularly relating to the relevant INTFLAGS register - some interrupt flags are not automatically cleared when the ISR runs, and if these are not cleared, it will trigger continually once it is triggered once. 
* Any global variable that an interrupt changes, if used outside the ISR, must be declared volatile - otherwise the compiler may optimize away access to it, resulting in code elsewhere not knowing that it was changed by the ISR. 
* Any global variable read by the ISR and written to by code outside of the ISR larger than a byte must be written to with interrupts disabled - if the interrupt triggers in the middle of a write, the ISR would see a corrupted value.
