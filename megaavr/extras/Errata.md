# tinyAVR 0/1-series Errata Summary
Microcontrollers, like any other product, often have bugs which are discovered only after the product is released. Sometimes these are fixed in later "silicon revisions" - other times they are not, and sometimes subsequent revisions of the part are released without fixing any of these issues at all. These defects are documented in the "Silicon Errata" documents provided by the manufacturer. On the classic AVR parts, this was often a chapter near the end of the datasheet - and was generally quite short (off the top of my head, the only classic AVR that I can think of where it's frequently an issue is the ATtiny1634, where one pin can't be used as an input if the WDT is not running). 

The tinyAVR 0-series and 1-series parts have a particularly large number of errata - likely because these were some of the first parts released with many of the new peripherals (though the megaAVR 0-series had come out slightly earlier - in many cases with similar issues - the 1-series had many peripherals not featured in the megaAVR 0-series). The specific errata relevant to a given part vary depending on the part and silicon revision. The silicon revision can be read from the `SYSCFG.REVID` register (0 = A, 1 = B, etc). It is also marked on the chip - though determining how to decipher the markings can be challenging. 

```
Serial.print("Silicon revision is: ");
Serial.println(SYSCFG.REVID);
```

Thankfully, most of these issues will not be encountered by most Arduino users. The issue with TCD autoupdate was stumbled over during development of megaTinyCore, and worked around. The


## tinyAVR 1-series
. | ATtiny212 | . | ATtiny214 | . | ATtiny416 | . | ATtiny816 | ATtiny417 | ATtiny1614 | ATtiny3216 | . 
. | ATtiny412 | . | ATtiny414 | . | . | . | . | ATtiny817 | ATtiny1616 | ATtiny3217 | . 
. | . | . | ATtiny814 | . | . | . | . | . | ATtiny1617 | . | . 
--- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | ---
Rev. | A | B | A | B | A | B | A, B | A,B | A | A | C
Device | . | . | . | . | . | . | . | . | . | . | . 
On 24-Pin Automotive Devices Pin PC5 is Not Available | - | - | - | - | - | - | - | - | - | - | -
Writing the OSCLOCK Fuse in FUSE.OSCCFG to ‘1’ Prevents Automatic Loading of Calibration Values | - | - | - | - | - | - | - | - | X | X | X
The Temperature Sensor is Not Calibrated on Parts with Date Code 727, 728 and 1728 (Year 2017, Week 27/28) | - | X | - | X | - | X | - | - | - | - | -
AC | . | . | . | . | . | . | . | . | . | . | . 
Coupling Through AC Pins | - | - | - | - | - | - | X | X | - | - | -
AC Interrupt Flag Not Set Unless Interrupt is Enabled | X | - | X | - | X | - | X | X | X | - | -
False Triggers May Occur Under Certain Conditions | X | - | X | - | X | - | X | X | X | - | -
False Triggering When Sweeping Negative Input of the AC When the Low Power Mode is Disabled | - | - | - | - | - | - | X | X | X | - | -
ADC | . | . | . | . | . | . | . | . | . | . | . 
ADC Functionality Cannot be Ensured with CLKADC Above 1.5 MHz and a Setting of 25% Duty Cycle | X | X | X | X | X | X | X | X | X | X | X
ADC Interrupt Flags Cleared When Reading RESH | - | - | - | - | - | - | X | X | X | - | -
ADC Performance Degrades with CLKADC Above 1.5 MHz and VDD < 2.7V | X | X | X | X | X | X | X | X | X | X | X
ADC Wake-Up with WCOMP | X | - | X | - | X | - | X | X | X | - | -
Changing ADC Control Bits During Free-Running Mode not Working | X | - | X | - | X | - | X | X | X | - | -
One Extra Measurement Performed After Disabling ADC FreeRunning Mode | X | X | X | X | X | X | X | X | X | X | X
Pending Event Stuck When Disabling the ADC | - | - | - | - | - | - | X | X | X | X | -
SAMPDLY and ASDV Does Not Work Together With SAMPLEN | X | - | X | - | X | - | - | - | X | - | -
CCL | . | . | . | . | . | . | . | . | . | . | . 
Connecting LUTs in Linked Mode Requires OUTEN Set to ‘1’ | X | X | X | X | X | X | X | X | X | X | -
D-latch is Not Functional | X | X | X | X | X | X | X | X | X | X | -
PORTMUX | . | . | . | . | . | . | . | . | . | . | . 
Selecting Alternative Output Pin for TCA0 Waveform Output 0-2 also Changes Waveform Output 3-5 | - | - | X | X | X | X | - | - | - | - | -
RTC | . | . | . | . | . | . | . | . | . | . | . 
Any Write to the RTC.CTRLA Register Resets the RTC and PIT Prescaler | X | X | X | X | X | X | X | X | X | X | -
Disabling the RTC Stops the PIT | X | X | X | X | X | X | X | X | X | X | -
TCB | . | . | . | . | . | . | . | . | . | . | . 
Minimum Event Duration Must Exceed the Selected Clock Period | X | X | X | X | X | X | X | X | X | X | X
The TCB Interrupt Flag is Cleared When Reading CCMPH | X | - | X | - | X | - | X | X | X | - | -
TCB Input Capture Frequency and Pulse-Width Measurement Mode Not Working with Prescaled Clock | X | - | X | - | X | - | X | X | X | - | -
The TCA Restart Command Does Not Force a Restart of TCB | X | X | X | X | X | X | X | X | X | X | X
TCD | . | . | . | . | . | . | . | . | . | . | . 
TCD Auto-Update Not Working | - | - | - | - | - | - | X | X | X | - | -
TCD Event Output Lines May Give False Events | - | - | - | - | - | - | X | X | X | - | -
TWI | . | . | . | . | . | . | . | . | . | . | . 
TIMEOUT Bits in the TWI.MCTRLB Register are Not Accessible | X | - | X | - | X | - | X | X | X | - | -
TWI Smart Mode Gives Extra Clock Pulse | X | - | X | - | X | - | X | X | X | - | -
TWI Master Mode Wrongly Detects the Start Bit as a Stop Bit | X | - | X | - | X | - | X | X | X | - | -
The TWI Master Enable Quick Command is Not Accessible | X | - | X | - | X | - | X | X | X | - | -
USART | . | . | . | . | . | . | . | . | . | . | . 
TXD Pin Override Not Released When Disabling the Transmitter | X | X | X | X | X | X | X | X | X | X | X
Full Range Duty Cycle Not Supported When Validating LIN Sync Field | - | - | - | - | - | - | - | - | X | - | -
Frame Error on a Previous Message May Cause False Start Bit Detection | X | X | X | X | X | X | X | X | X | X | -
Open-Drain Mode Does Not Work When TXD is Configured as Output | - | - | - | - | - | - | - | - | X | X | X


## Automotive tinyAVR 1-series

. | ATtiny212/214 | ATtiny814 | ATtiny416/417 | ATtiny1614 | ATtiny3216
--- | --- | --- | --- | --- | ---
. | ATtiny412/414 | . | ATtiny816/817 | ATtiny1616 | ATtiny3217
. | . | . | . | ATtiny1617 | . 
Rev. | B | A, B | A, B | A | B
Device | . | . | . | . | . 
On 24-Pin Automotive Devices Pin PC5 is Not Available | - | - | - | X | -
Writing the OSCLOCK Fuse in FUSE.OSCCFG to ‘1’ Prevents Automatic Loading of Calibration Values | X | - | - | X | X
The Temperature Sensor is Not Calibrated on Parts with Date Code 727, 728 and 1728 (Year 2017, Week 27/28) | X | - | - | - | -
AC | . | . | . | . | . 
Coupling Through AC Pins | - | X | X | - | -
AC Interrupt Flag Not Set Unless Interrupt is Enabled | - | X | X | X | -
False Triggers May Occur Under Certain Conditions | - | X | X | X | -
False Triggering When Sweeping Negative Input of the AC When the Low Power Mode is Disabled | - | X | X | X | -
ADC | . | . | . | . | . 
ADC Functionality Cannot be Ensured with CLKADC Above 1.5 MHz and a Setting of 25% Duty Cycle | X | X | X | X | X
ADC Interrupt Flags Cleared When Reading RESH | - | X | X | X | -
ADC Performance Degrades with CLKADC Above 1.5 MHz and VDD < 2.7V | - | X | - | - | -
ADC Wake-Up with WCOMP | - | X | X | X | -
Changing ADC Control Bits During Free-Running Mode not Working | - | X | X | X | -
One Extra Measurement Performed After Disabling ADC FreeRunning Mode | X | X | X | X | X
Pending Event Stuck When Disabling the ADC | - | X | X | X | -
SAMPDLY and ASDV Does Not Work Together With SAMPLEN | - | - | - | X | -
CCL | . | . | . | . | . 
Connecting LUTs in Linked Mode Requires OUTEN Set to ‘1’ | X | X | X | X | -
D-latch is Not Functional | X | X | X | X | -
PORTMUX | . | . | . | . | . 
Selecting Alternative Output Pin for TCA0 Waveform Output 0-2 also Changes Waveform Output 3-5 | X | - | - | - | -
RTC | . | . | . | . | . 
Any Write to the RTC.CTRLA Register Resets the RTC and PIT Prescaler | X | X | X | X | -
Disabling the RTC Stops the PIT | X | X | X | X | -
TCB | . | . | . | . | . 
Minimum Event Duration Must Exceed the Selected Clock Period | X | X | X | X | X
The TCB Interrupt Flag is Cleared When Reading CCMPH | - | X | X | X | -
TCB Input Capture Frequency and Pulse-Width Measurement Mode Not Working with Prescaled Clock | - | X | X | X | -
The TCA Restart Command Does Not Force a Restart of TCB | X | X | X | X | X
TCD | . | . | . | . | . 
TCD Auto-Update Not Working | - | X | X | X | -
TCD Event Output Lines May Give False Events | - | X | X | X | -
TWI | . | . | . | . | . 
TIMEOUT Bits in the TWI.MCTRLB Register are Not Accessible | - | X | X | X | -
TWI Smart Mode Gives Extra Clock Pulse | - | X | X | X | -
TWI Master Mode Wrongly Detects the Start Bit as a Stop Bit | - | X | X | X | -
The TWI Master Enable Quick Command is Not Accessible | - | X | X | X | -
USART | . | . | . | . | . 
TXD Pin Override Not Released When Disabling the Transmitter | X | X | X | X | X
Full Range Duty Cycle Not Supported When Validating LIN Sync Field | X | - | - | X | -
Frame Error on a Previous Message May Cause False Start Bit Detection | X | X | X | X | -
Open-Drain Mode Does Not Work When TXD is Configured as Output | X | - | - | X | X

## tinyAVR 0-series Errata
. | ATtiny202 | ATtiny204 | ATtiny202/204 | ATtiny804 | ATtiny806 | ATtiny807
. | ATtiny402 | ATtiny404 | ATtiny402/404/406 | ATtiny1604 | ATtiny1606 | ATtiny1607
. | . | ATtiny406 | Automotive | Both | Both | Both
--- | --- | --- | --- | --- | --- | ---
Rev. | B | B | B | A | A | A
Device | . | . | . | . | . | . 
The Temperature Sensor is Not Calibrated on Parts with Date Code 727, 728 and 1728 (Year 2017, Week 27/28) | X | X | X | - | - | -
ADC | . | . | . | . | . | . 
ADC Functionality Cannot be Ensured with CLKADC Above 1.5 MHz and a Setting of 25% Duty Cycle | X | X | X | X | X | X
ADC Performance Degrades with CLKADC Above 1.5 MHz and VDD < 2.7V | X | X | - | X | X | X
One Extra Measurement Performed After Disabling ADC FreeRunning Mode | X | X | X | X | X | X
Pending Event Stuck When Disabling the ADC | - | - | - | X | X | X
CCL | . | . | . | . | . | . 
Connecting LUTs in Linked Mode Requires OUTEN Set to ‘1’ | X | X | X | X | X | X
D-latch is Not Functional | X | X | X | X | X | X
PORTMUX | . | . | . | . | . | . 
Selecting Alternative Output Pin for TCA0 Waveform Output 0-2 also Changes Waveform Output 3-5 | - | X | X | - | - | -
RTC | . | . | . | . | . | . 
Any Write to the RTC.CTRLA Register Resets the RTC and PIT Prescaler | X | X | X | X | X | X
Disabling the RTC Stops the PIT | X | X | X | X | X | X
TCB | . | . | . | . | . | . 
Minimum Event Duration Must Exceed the Selected Clock Period | X | X | X | X | X | X
The TCA Restart Command Does Not Force a Restart of TCB | X | X | X | X | X | X
USART | . | . | . | . | . | . 
TXD Pin Override Not Released When Disabling the Transmitter | X | X | X | X | X | X
Frame Error on a Previous Message May Cause False Start Bit Detection | X | X | X | - | - | -


## tinyAVR 0/1-series Errata Details

These errata details are taken verbatim from the Silicon Errata and Datasheet Clarification documents - see the links in the [datasheet pages](Datasheets.md) - they have only been formatted for markdown, and gathered from the multiple errata sheets into a single document for ease of comparison and reference. Be sure to check that a given item applies to the part you are working with on the above tables.


### Device
#### On 24-Pin Automotive Devices Pin PC5 is Not Available
On 24-pin automotive devices (ATtiny1617 automotive - other versions are not effected) pin PC5 is not available.
**Workaround:** Do not connect pin PC5 and disable input on pin (PORTC.PINTCRL5.ISC=0x4)

#### Writing the OSCLOCK Fuse in `FUSE.OSCCFG` to '1' Prevents Automatic Loading of Calibration
Values Writing the OSCLOCK fuse in `FUSE.OSCCFG` to '1' prevents the automatic loading of calibration values from the signature row. The device will run with an uncalibrated OSC20M oscillator.

**Workaround:** Do not use OSCLOCK for locking the oscillator calibration value. The oscillator calibration value can be locked by
writing LOCK in `CLKCTRL.OSC`20MCALIBB to '1'.

#### The Temperature Sensor is Not Calibrated on Parts with Date Code 727, 728 and 1728 (Year 2017, Week 27/28)
The temperature sensor is not calibrated on parts with date code 727/728 (used on QFN packages) and 1728 (used on SOIC packages).
**Workaround:** If temperature sensor calibration data is required, devices with the affected date code may be returned through the Microchip RMA service. Devices with this date code are no longer shipped by Microchip.

### AC - Analog Comparator
#### Coupling Through AC Pins
There is a capacitive coupling through the Analog Comparator. Toggling the selected positive AC pin may affect the selected negative input pin and vice versa.
**Workaround:** When the AC is disabled, configure AC.MUXCTRLA.MUXNEG to DAC or internal reference.

#### AC Interrupt Flag Not Set Unless Interrupt is Enabled
`ACn.STATUS.CMP` is not set if the `ACn.INTCTRL.CMP` is not set.

**Workaround:** Enable `ACn.INTCTRL.CMP` or use `ACn.STATUS.STATE` for polling.

#### False Triggers May Occur Under Certain Conditions
False triggers may occur on falling input pin:
* If the slew rate on the input signal is greater than 2 V/µs for common-mode voltage below 0.5V
* If the slew rate on the input signal is greater than 10 V/µs for common-mode voltage above 0.5V
* If the slew rate on the input signal is greater than 10 V/µs for any common-mode voltage and Low-Power mode
is enabled

**Workaround:** None.

#### False Triggering When Sweeping Negative Input of the AC When the Low-Power Mode is Disabled
A false trigger may occur if sweeping the negative input of the AC with a negative slope, and the AC has Low-Power mode disabled.

**Workaround:** Enable Low-Power mode in `AC.CTRLA.LPMODE`.

### ADC - Analog-to-Digital Converter
#### SAMPDLY and ASDV Does Not Work Together With SAMPLEN
Using `SAMPCTRL.SAMPLEN` at the same time as `CTRLD.SAMPDLY` or `CTRLD.ASDV` will cause an unpredictable sampling length.

**Workaround:** When setting `SAMPCTRL.SAMPLEN` greater than 0x0, the `CTRLD.SAMPDLY` and `CTRLD.ASDV` must be cleared.

#### Pending Event Stuck When Disabling the ADC
If the ADC is disabled during an event-triggered conversion, the event will not be cleared.

**Workaround:** Clear `ADC.EVCTRL.STARTEI` and wait for the conversion to complete before disabling the ADC.

#### ADC Functionality Cannot be Ensured with CLKADC Above 1.5 MHz and a Setting of 25% Duty Cycle
The ADC functionality cannot be ensured if CLKADC > 1.5 MHz with `ADCn.CALIB.DUTYCYC` set to '1'.

**Workaround:** If ADC is operated with CLKADC > 1.5 MHz, `ADCn.CALIB.DUTYCYC` must be set to '0' (50% duty cycle).

#### ADC Performance Degrades with CLKADC Above 1.5 MHz and VDD < 2.7V
The ADC INL performance degrades if CLKADC > 1.5 MHz and `ADCn.CALIB.DUTYCYC` set to '0' for VDD < 2.7V.

**Workaround:** None.

#### ADC Interrupt Flags Cleared When Reading RESH
`ADCn.INTFLAGS.RESRDY` and `ADCn.INTFLAGS.WCOMP` are cleared when reading `ADCn.RESH`.

**Workaround:** In 8-bit mode, read `ADCn.RESH` to clear the flag or clear the flag directly.

#### Changing ADC Control Bits During Free-Running Mode not Working
If control signals are changed during Free-Running mode, the new configuration is not properly taken into account in the next measurement. This is valid for the `ADC.CTRLB`, `ADC.CTRLC`, `ADC.SAMPCTRL` registers and the `ADC.MUXPOS`, `ADC.WINLT` and `ADC.WINHT` registers.

**Workaround:** Disable ADC Free-Running mode before updating the `ADC.CTRLB`, `ADC.CTRLC`, `ADC.SAMPCTRL`,
`ADC.MUXPOS`, `ADC.WINLT` or `ADC.WINHT` registers.

#### One Extra Measurement Performed After Disabling ADC Free-Running Mode
The ADC may perform one additional measurement after clearing `ADCn.CTRLA.FREERUN`.

**Workaround:** Write `ADCn.CTRLA.ENABLE` to '0' to stop the Free-Running mode immediately.

#### ADC Wake-Up with WCOMP
When waking up from STANDBY sleep mode with ADC WCOMP interrupt, the ADC is disabled for a few cycles
before the device enters ACTIVE mode. A new INITDLY is required before the next conversion.

**Workaround:** Use INITDLY before the next conversion.

### PORTMUX - Port Multiplexer
#### Selecting Alternative Output Pin for TCA0 Waveform Output 0-2 also Changes Waveform Output 3-5
Selecting alternative output pin for TCA0 in PORTMUX.CTRLC does not work as described when TCA0 operates in split mode.
* Writing PORTMUX.CTRLC bit 0 to '1' will shift the pin position for both WO0 and WO3
* Writing PORTMUX.CTRLC bit 1 to '1' will shift the pin position for both WO1 and WO4
* Writing PORTMUX.CTRLC bit 2 to '1' will shift the pin position for both WO2 and WO5
PORTMUX.CTRLC[5:3] are non-functional.
**Workaround:** None.

### CCL - Configurable Custom Logic
#### Connecting LUTs in Linked Mode Requires OUTEN Set to '1'
Connecting the LUTs in linked mode requires `LUTnCTRLA.OUTEN` set to '1' for the LUT providing the input source.

**Workaround:** Use an event channel to link the LUTs or do not use the corresponding I/O pin for other purposes.

#### D-latch is Not Functional
The CCL D-latch is not functional.

**Workaround:** None.

### RTC - Real-Time Counter
#### Any Write to the `RTC.CTRLA` Register Resets the RTC and PIT Prescaler
Any write to the `RTC.CTRLA` register resets the RTC and PIT prescaler.

**Workaround:** None.

#### Disabling the RTC Stops the PIT
Writing `RTC.CTRLA.RTCEN` to '0' will stop the PIT.
Writing `RTC.PITCTRLA.PITEN` to '0' will stop the RTC.

**Workaround:** Do not disable the RTC or the PIT if any of the modules are used.

### TCB - Timer/Counter B
#### Minimum Event Duration Must Exceed the Selected Clock Period
Event detection will fail if TCBn receives an input event with a high/low period shorter than the period of the selected clock source (CLKSEL in `TCBn.CTRLA`). This applies to the TCB modes (CNTMODE in `TCBn.CTRLB`) Time-Out Check and Input Capture Frequency and Pulse-Width Measurement mode.

**Workaround:** Ensure that the high/low period of input events is equal to or longer than the period of the selected clock source
(CLKSEL in `TCBn.CTRLA`).

#### The TCB Interrupt Flag is Cleared When Reading CCMPH
`TCBn.INTFLAGS.CAPT` is cleared when reading `TCBn.CCMPH` instead of CCMPL.

**Workaround:** Read both `TCBn.CCMPL` and `TCBn.CCMPH`.

#### TCB Input Capture Frequency and Pulse-Width Measurement Mode Not Working with Prescaled
Clock The TCB Input Capture Frequency and Pulse-Width Measurement mode may lock to Freeze state if CLKSEL in
`TCB.CTRLA` is set to any other value than 0x0.

**Workaround:** Only use CLKSEL equal to 0x0 when using Input Capture Frequency and Pulse-Width Measurement mode.

#### The TCA Restart Command Does Not Force a Restart of TCB
The TCA restart command does not force a restart of the TCB when TCB is running in SYNCUPD mode. TCB is only restarted after a TCA OVF.

**Workaround:** None.

### TCD - Timer/Counter D
#### TCD Event Output Lines May Give False Events
The TCD event output lines can give out false events.

**Workaround:** Use the delayed event functionality with a minimum of one cycle delay.

#### TCD Auto-Update Not Working
The TCD auto-update feature is not working.

**Workaround:** None.

### TWI - Two-Wire Interface
#### TIMEOUT Bits in the `TWI.MCTRLB` Register are Not Accessible
The TIMEOUT bits in the `TWI.MCTRLB` register are not accessible from software.

**Workaround:** When initializing TWI, BUSSTATE in `TWI.MSTATUS` should be brought into IDLE state by writing 0x1 to it.

#### TWI Master Mode Wrongly Detects the Start Bit as a Stop Bit
If TWI is enabled in Master mode followed by an immediate write to the MADDR register, the bus monitor recognizes the Start bit as a Stop bit.

**Workaround:** Wait for a minimum of two clock cycles from `TWI.MCTRLA.ENABLE` until `TWI.MADDR` is written.

#### TWI Smart Mode Gives Extra Clock Pulse
TWI Master with Smart mode enabled gives an extra clock pulse on the SCL line after sending NACK.

**Workaround:** None.

#### The TWI Master Enable Quick Command is Not Accessible
`TWI.MCTRLA.QCEN` is not accessible from software.

**Workaround:** None.

### USART - Universal Synchronous and Asynchronous Receiver and Transmitter
#### TXD Pin Override Not Released When Disabling the Transmitter
The USART will not release the TXD pin override if:
* The USART transmitter is disabled by writing the TXEN bit in `USART.CTRLB` to '0' while the USART receiver is
disabled (RXEN in `USART.CTRLB` is '0')
* Both the USART transmitter and receiver are disabled at the same time by writing the TXEN and RXEN bits in
`USART.CTRLB` to '0'

**Workaround:** There are two possible work arounds:

* Make sure the receiver is enabled (RXEN in `USART.CTRLB` is '1') while disabling the transmitter (writing TXEN
in `USART.CTRLB` to '0')
* Writing to any register in the USART after disabling the transmitter will start the USART for long enough to
release the pin override of the TXD pin

#### Full Range Duty Cycle Not Supported When Validating LIN Sync Field
For the LIN sync field, the USART is validating each bit to be within ±15% instead of the time between falling edges as described in the LIN specification. This allows a minimum duty cycle of 43.5% and a maximum duty cycle of
57.5%.

**Workaround:** None.

#### Frame Error on a Previous Message May Cause False Start Bit Detection
A false start bit detection will trigger if receiving a frame with `RXDATAH.FERR` set and reading the RXDATAL before the RxD line goes high.

**Workaround:** Wait for the RXD pin to go high before reading RXDATA, for instance, by polling the bit in `PORTn.IN` where the RXD
pin is located.

#### Open-Drain Mode Does Not Work When TXD is Configured as Output
When the USART TXD pin is configured as an output, it can drive the pin high regardless of whether the Open-Drain mode is enabled or not.

**Workaround:** Configure the TXD pin as an input by writing the corresponding bit in PORTx.DIR to '0' when using Open-Drain
mode.