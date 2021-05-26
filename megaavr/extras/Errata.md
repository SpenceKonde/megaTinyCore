# tinyAVR 0/1-series Errata Summary
This list does not cover several newly discovered erratas. or the 2-series (at least so far, the list of 2-series errata is far shorter, which is at least a step in the right direction)

Microcontrollers, like any other product, often have bugs which are discovered only after the product is released. Sometimes these are fixed in later "silicon revisions" - other times they are not, and sometimes subsequent revisions of the part are released without fixing any of these issues at all. These defects are documented in the "Silicon Errata" documents provided by the manufacturer. On the classic AVR parts, this was often a chapter near the end of the datasheet - and was generally quite short (off the top of my head, the only classic AVR that I can think of where it's frequently an issue is the ATtiny1634, where one pin can't be used as an input if the WDT is not running).

The tinyAVR 0-series and 1-series parts have a particularly large number of errata - likely because these were some of the first parts released with many of the new peripherals (though the megaAVR 0-series had come out slightly earlier - in many cases with similar issues - the 1-series had many peripherals not featured in the megaAVR 0-series). The specific errata relevant to a given part vary depending on the part and silicon revision. The silicon revision can be read from the `SYSCFG.REVID` register (0 = A, 1 = B, etc). It is also marked on the chip - though determining how to decipher the markings can be challenging.

The list of errata from Microchip has been updated since this was written. Nothing specific to any given part. A few universal ones were added (these impact even the latest AVR DA and DB parts in many cases):
* CCMPL and CCMPH are supposed to be independent when a TCB is set to 8-bit PWM mode. They are not. You either need to write both in the correct order, or else pack both values into a 16-bit type and writeTHHAT to CCMP
* Apparently it's not intended that you have to disable the whole CCL to disable any given LUT so that you can reconfigure it. the LUTs are supposed to be protected based on their own enable bit, but it's not supposed to be required to turn off the whole CCL (which may be used for other things) to reconfigbure one LUT. *the pre-release errata sheets for AVR64DB and AVR32DB do not list it* - are they just incomplete, or is that actually fixed? That would be fabulous news if so.
* This one only hits the tinys, and if you can get your claws on a Rev C 3216 or 3217 it's fixed there (and in the Automotive version of same) - but it is NASTTY if you are using the RTC. The parts they own up to in the errata sheet is bad: Don't disable the RTC or the PIT if you need to use either one of them. stopping one will break both. And writing RTC.CTRLA resets prescaler coumt (as in the time before it ticks over again, so current tick is prolonged). What they do *not* mention is...
 * (reported by user on ATtiny412) Set RTC clock source. and enable the pit with desired period.. Besurprised that the PIT is now working correctly! buuuuut.... RTC.CNTBUSY is stuck on, and when you next to the usual while(RTC.STATUS) to check thatit's not busy with anything, you will be waiting until the cows come home.
 * ensuring that RTC.CTRLA is enabled fixes that - but the RTC and PIT can be confounding enough WITHOUT the errata.

Note that Rev. C if the 32k parts is apparently the only revision that fixes a substantial amount of errata. It fixes some of the RTC stuff, it fixes the CCL D-latch and the fact that you had to enable pin output to use the link output source on a downstream LUT. the pending event stuck issue on the ADC, and the thhing where if you kept reading the USART quickly after receiving frame errors, it would keep corrupting the next character, which might thus end up misframed and trigger the same bug.

```
Serial.print("Silicon revision is: ");
Serial.println(SYSCFG.REVID);
```

Thankfully, most of these issues will not be encountered by most Arduino users. The impact column indicates the liklihood that someone working with an effected part through megaTinyCore and Arduino would encounter it.
* 0 - Impact extremely unlikely
* 1 - Impact only when using exotic features for which megaTinyCore does not provide wrappers
* 2 - Impact likely when using normal features which may be used through libraries or register configuration.
* 3 - Issue impacted functions provided by megaTinyCore or included libraries in previous versions, but worked around in current version, and will impact users of this functionality if reconfigured manually, or unavoidably impacts functionality exposed by core or included libraries, but can be worked around.
* 4 - Issue impacts functions provided by megaTinyCore or included libraries in the current released version, but will be worked around in future versions.
* 5 - Issue impacts functions provided by megaTinyCore or included libraries and cannot be worked around.
* * - Issure is high impact but effected devices are rare

## tinyAVR 1-series
 . | . |ATtiny212 | . |ATtiny214/414 | . |ATtiny417|ATtiny1614|ATtiny3216 | .
---| --- | --- | --- | --- | --- | --- | --- | --- | ---
 . | . |ATtiny412 | . |ATtiny416 | . |ATtiny816|ATtiny1616|ATtiny3217 | .
 . | Impact | .  | . |ATtiny814 | . |ATtiny817|ATtiny1617  | . | .
Silicon Revision | . |A|B|A|B|A,B|A|A|C
Device  | . | .  | . | .  | . | .  | . | . |.
Writing the OSCLOCK Fuse in FUSE.OSCCFG to ‘1’ Prevents Automatic Loading of Calibration Values | 0 |-|-|-|-|-|X|X|X
The Temperature Sensor is Not Calibrated on Parts with Date Code 727, 728 and 1728 (Year 2017, Week 27/28) | 1 |-|X|-|X|-|-|-|-
**AC**  | . | .  | . | .  | . | .  | . | . |.
Coupling Through AC Pins | 2 |-|-|-|-|X|-|-|-
AC Interrupt Flag Not Set Unless Interrupt is Enabled  | 2 |X|-|X|-|X|X|-|-
False Triggers May Occur Under Certain Conditions  | 1 |X|-|X|-|X|X|-|-
False Triggering When Sweeping Negative Input of the AC When the Low Power Mode is Disabled  | 1 |-|-|-|-|X|X|-|-
**ADC**  | . | .  | . | .  | . | .  | . | . |.
ADC Functionality Cannot be Ensured with CLKADC Above 1.5 MHz and a Setting of 25% Duty Cycle | 1 |X|X|X|X|X|X|X|X
ADC Interrupt Flags Cleared When Reading RESH | 1 |-|-|-|-|X|X|-|-
ADC Performance Degrades with CLKADC Above 1.5 MHz and VDD < 2.7V | 1 |X|X|X|X|X|X|X|X
ADC Wake-Up with WCOMP | 1 |X|-|X|-|X|X|-|-
Changing ADC Control Bits During Free-Running Mode not Working | 2 |X|-|X|-|X|X|-|-
One Extra Measurement Performed After Disabling ADC FreeRunning Mode | 2 |X|X|X|X|X|X|X|X
Pending Event Stuck When Disabling the ADC | 2 |-|-|-|-|X|X|X|-
SAMPDLY and ASDV Does Not Work Together With SAMPLEN | 2 |X|-|X|-|-|X|-|-
**CCL**  | . | .  | . | .  | . | .  | . | . |.
Connecting LUTs in Linked Mode Requires OUTEN Set to ‘1’ | 4 |X|X|X|X|X|X|X|-
D-latch is Not Functional | 4 |X|X|X|X|X|X|X|-
**PORTMUX**  | . | .  | . | .  | . | .  | . | . |.
Selecting Alternative Output Pin for TCA0 Waveform Output 0-2 also Changes Waveform Output 3-5 | 3 |-|-|X|X|-|-|-|-
**RTC**  | . | .  | . | .  | . | .  | . | . |.
Any Write to the RTC.CTRLA Register Resets the RTC and PIT Prescaler | 2 |X|X|X|X|X|X|X|-
Disabling the RTC Stops the PIT | 2 |X|X|X|X|X|X|X|-
**TCB**  | . | .  | . | .  | . | .  | . | . |.
Minimum Event Duration Must Exceed the Selected Clock Period | 1 |X|X|X|X|X|X|X|X
The TCB Interrupt Flag is Cleared When Reading CCMPH | 2 |X|-|X|-|X|X|-|-
TCB Input Capture Frequency and Pulse-Width Measurement Mode Not Working with Prescaled Clock | 1 |X|-|X|-|X|X|-|-
The TCA Restart Command Does Not Force a Restart of TCB | 1 |X|X|X|X|X|X|X|X
**TCD**  | . | .  | . | .  | . | .  | . | . |.
TCD Auto-Update Not Working | 3 |-|-|-|-|X|X|-|-
TCD Event Output Lines May Give False Events | 1 |-|-|-|-|X|X|-|-
**TWI**  | . | .  | . | .  | . | .  | . | . |.
TIMEOUT Bits in the TWI.MCTRLB Register are Not Accessible | 1 |X|-|X|-|X|X|-|-
TWI Smart Mode Gives Extra Clock Pulse | 2 |X|-|X|-|X|X|-|-
TWI Master Mode Wrongly Detects the Start Bit as a Stop Bit | 2 |X|-|X|-|X|X|-|-
The TWI Master Enable Quick Command is Not Accessible | 1 |X|-|X|-|X|X|-|-
**USART**  | . | .  | . | .  | . | .  | . | . |.
TXD Pin Override Not Released When Disabling the Transmitter | 4 |X|X|X|X|X|X|X|X
Full Range Duty Cycle Not Supported When Validating LIN Sync Field | 1 |-|-|-|-|-|X|-|-
Frame Error on a Previous Message May Cause False Start Bit Detection | 3 |X|X|X|X|X|X|X|-
Open-Drain Mode Does Not Work When TXD is Configured as Output | 3 |-|-|-|-|-|X|X|X




## Automotive tinyAVR 1-series

 . |. | ATtiny212/214 | ATtiny814 | ATtiny416/417 | ATtiny1614 | ATtiny3216
--- | --- | --- | --- | --- | --- | ---
 . |. | ATtiny412/414 | . | ATtiny816/817 | ATtiny1616 | ATtiny3217
 . | Impact | . | . | . | ATtiny1617  | . | .
Silicon Revision | . | B | A, B | A, B | A | B
Device  | . | . | . | . | . | .
On 24-Pin Automotive Devices Pin PC5 is Not Available  | * | - | - | - | X | -
Writing the OSCLOCK Fuse in FUSE.OSCCFG to ‘1’ Prevents Automatic Loading of Calibration Values  | 0 | X | - | - | X | X
The Temperature Sensor is Not Calibrated on Parts with Date Code 727, 728 and 1728 (Year 2017, Week 27/28)  | 1 | X | - | - | - | -
**AC**  | . | . | . | . | . | .
Coupling Through AC Pins  | 2 | - | X | X | - | -
AC Interrupt Flag Not Set Unless Interrupt is Enabled  | 2 | - | X | X | X | -
False Triggers May Occur Under Certain Conditions  | 2 | - | X | X | X | -
False Triggering When Sweeping Negative Input of the AC When the Low Power Mode is Disabled  | 1 | - | X | X | X | -
**ADC**  | . | . | . | . | . | .
ADC Functionality Cannot be Ensured with CLKADC Above 1.5 MHz and a Setting of 25% Duty Cycle  | 1 | X | X | X | X | X
ADC Interrupt Flags Cleared When Reading RESH  | 1 | - | X | X | X | -
ADC Performance Degrades with CLKADC Above 1.5 MHz and VDD < 2.7V  | 1 | - | X | - | - | -
ADC Wake-Up with WCOMP  | . | - | X | X | X | -
Changing ADC Control Bits During Free-Running Mode not Working  | 2 | - | X | X | X | -
One Extra Measurement Performed After Disabling ADC FreeRunning Mode  | 2 | X | X | X | X | X
Pending Event Stuck When Disabling the ADC  | 1 | - | X | X | X | -
SAMPDLY and ASDV Does Not Work Together With SAMPLEN  | 2 | - | - | - | X | -
**CCL**  | . | . | . | . | . | .
Connecting LUTs in Linked Mode Requires OUTEN Set to '1'  | 5 | X | X | X | X | -
D-latch is Not Functional  | 3 | X | X | X | X | -
**PORTMUX**  | . | . | . | . | . | .
Selecting Alternative Output Pin for TCA0 Waveform Output 0-2 also Changes Waveform Output 3-5  | 3 | X | - | - | - | -
**RTC**  | . | . | . | . | . | .
Any Write to the RTC.CTRLA Register Resets the RTC and PIT Prescaler  | . | X | X | X | X | -
Disabling the RTC Stops the PIT  | . | X | X | X | X | -
**TCB**  | . | . | . | . | . | .
Minimum Event Duration Must Exceed the Selected Clock Period  | 3 | X | X | X | X | X
The TCB Interrupt Flag is Cleared When Reading CCMPH  | 2 | - | X | X | X | -
TCB Input Capture Frequency and Pulse-Width Measurement Mode Not Working with Prescaled Clock  | 2 | - | X | X | X | -
The TCA Restart Command Does Not Force a Restart of TCB  | 1 | X | X | X | X | X
**TCD**  | . | . | . | . | . | .
TCD Auto-Update Not Working  | 3 | - | X | X | X | -
TCD Event Output Lines May Give False Events  | 1 | - | X | X | X | -
**TWI**  | . | . | . | . | . | .
TIMEOUT Bits in the TWI.MCTRLB Register are Not Accessible  | 1 | - | X | X | X | -
TWI Smart Mode Gives Extra Clock Pulse  | 1 | - | X | X | X | -
TWI Master Mode Wrongly Detects the Start Bit as a Stop Bit  | 1 | - | X | X | X | -
The TWI Master Enable Quick Command is Not Accessible  | 1 | - | X | X | X | -
**USART**  | . | . | . | . | . | .
TXD Pin Override Not Released When Disabling the Transmitter  | 4 | X | X | X | X | X
Full Range Duty Cycle Not Supported When Validating LIN Sync Field  | 1 | X | - | - | X | -
Frame Error on a Previous Message May Cause False Start Bit Detection  | 1 | X | X | X | X | -
Open-Drain Mode Does Not Work When TXD is Configured as Output  | 2 | X | - | - | X | X

## tinyAVR 0-series Errata
 . | . |ATtiny202|ATtiny204|ATtiny202/204|ATtiny804/806/807
---  |---| --- | --- | --- | ---
 . |. |ATtiny402|ATtiny404|ATtiny402/404/406|ATtiny1604/1606/1607
 . | Impact | . |ATtiny406|Automotive|Both
Silicon Revision | . |B|B|B|A
Device | . | . | . | . | .
The Temperature Sensor is Not Calibrated on Parts with Date Code 727, 728 and 1728 (Year 2017, Week 27/28) | 1 |X|X|X|-
**ADC**  | . | . | . | . | .
ADC Functionality Cannot be Ensured with CLKADC Above 1.5 MHz and a Setting of 25% Duty Cycle | 1 |X|X|X|X
ADC Performance Degrades with CLKADC Above 1.5 MHz and VDD < 2.7V | 1 |X|X|-|X
One Extra Measurement Performed After Disabling ADC FreeRunning Mode | 1 |X|X|X|X
Pending Event Stuck When Disabling the ADC | 1 |-|-|-|X
**CCL**  | . | . | . | . | .
Connecting LUTs in Linked Mode Requires OUTEN Set to ‘1’ | 5 |X|X|X|X
D-latch is Not Functional | 3 |X|X|X|X
**PORTMUX**  | . | . | . | . | .
Selecting Alternative Output Pin for TCA0 Waveform Output 0-2 also Changes Waveform Output 3-5 | 3 |-|X|X|-
**RTC**  | . | . | . | . | .
Any Write to the RTC.CTRLA Register Resets the RTC and PIT Prescaler | 2 |X|X|X|X
Disabling the RTC Stops the PIT | 2 |X|X|X|X
**TCB**  | . | . | . | . | .
Minimum Event Duration Must Exceed the Selected Clock Period | 2 |X|X|X|X
The TCA Restart Command Does Not Force a Restart of TCB | 1 |X|X|X|X
**USART**  | . | . | . | . | .
TXD Pin Override Not Released When Disabling the Transmitter | 4 |X|X|X|X
Frame Error on a Previous Message May Cause False Start Bit Detection | 1 |X|X|X|-



## tinyAVR 0/1-series Errata Details

These errata details are taken verbatim from the Silicon Errata and Datasheet Clarification documents except as noted below - see the links in the [datasheet pages](Datasheets.md) - they have only been formatted for markdown, and gathered from the multiple errata sheets into a single document for ease of comparison and reference. Be sure to check that a given item applies to the part you are working with on the above tables.

For each issue, we have added a **megaTinyCore note** describing impact on megaTinyCore users. In a very small number of cases, we have marked annotated the text for clarity in the context of this combined errata document, these notes are *in italics* and the old wording in ~strikethrough~.


### Device
#### On ~24-Pin~ *ATtiny1617* Automotive Devices Pin PC5 is Not Available
On ~24-pin~ *ATtiny1617* automotive devices pin PC5 is not available.

**Workaround:** Do not connect pin PC5 and disable input on pin (PORTC.PINTCRL5.ISC=0x4)

**megaTinyCore note:** The effected part is unlikely to be used with megaTinyCore

#### Writing the OSCLOCK Fuse in `FUSE.OSCCFG` to '1' Prevents Automatic Loading of Calibration
Values Writing the OSCLOCK fuse in `FUSE.OSCCFG` to '1' prevents the automatic loading of calibration values from the signature row. The device will run with an uncalibrated OSC20M oscillator.

**Workaround:** Do not use OSCLOCK for locking the oscillator calibration value. The oscillator calibration value can be locked by
writing LOCK in `CLKCTRL.OSC`20MCALIBB to '1'.

**megaTinyCore note:** No concern

#### The Temperature Sensor is Not Calibrated on Parts with Date Code 727, 728 and 1728 (Year 2017, Week 27/28)
The temperature sensor is not calibrated on parts with date code 727/728 (used on QFN packages) and 1728 (used on SOIC packages).
**Workaround:** If temperature sensor calibration data is required, devices with the affected date code may be returned through the Microchip RMA service. Devices with this date code are no longer shipped by Microchip.

**megaTinyCore note:** These are unlikely to be found in the wild anymore.

### AC - Analog Comparator
#### Coupling Through AC Pins
There is a capacitive coupling through the Analog Comparator. Toggling the selected positive AC pin may affect the selected negative input pin and vice versa.

**Workaround:** When the AC is disabled, configure AC.MUXCTRLA.MUXNEG to DAC or internal reference.

**megaTinyCore note:** megaTinyCore does not provide any functionality related to the analog comparators. If your code or libraries do, and you disable them and use the input pins for other purposes, you must take account of this.

#### AC Interrupt Flag Not Set Unless Interrupt is Enabled
`ACn.STATUS.CMP` is not set if the `ACn.INTCTRL.CMP` is not set.

**Workaround:** Enable `ACn.INTCTRL.CMP` or use `ACn.STATUS.STATE` for polling.

**megaTinyCore note:** megaTinyCore does not provide any functionality related to the analog comparators. If your code or libraries do, you cannot poll the interrupt flag to see whether it has been triggered. You can poll `ACn.STATUS.CMP` (though if the comparison condition becomes true, and then false again, between the occasions on which you check this bit, you would miss it).

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

**megaTinyCore note:** megaTinyCore does not provide any functionality related to the analog comparators. If your code or libraries do, this must be accounted for)

### ADC - Analog-to-Digital Converter
#### SAMPDLY and ASDV Does Not Work Together With SAMPLEN
Using `SAMPCTRL.SAMPLEN` at the same time as `CTRLD.SAMPDLY` or `CTRLD.ASDV` will cause an unpredictable sampling length.

**Workaround:** When setting `SAMPCTRL.SAMPLEN` greater than 0x0, the `CTRLD.SAMPDLY` and `CTRLD.ASDV` must be cleared.

**megaTinyCore note:** Not an issue unless you reconfigure the ADC manually, but be aware that megaTinyCore does set SAMPLEN by default.

#### Pending Event Stuck When Disabling the ADC
If the ADC is disabled during an event-triggered conversion, the event will not be cleared.

**Workaround:** Clear `ADC.EVCTRL.STARTEI` and wait for the conversion to complete before disabling the ADC.

**megaTinyCore note:** Only an issue if you reconfigure the ADC to use the event system as a start trigger, and then disable the ADC. Use suggested workaround in this case.

#### ADC Functionality Cannot be Ensured with CLKADC Above 1.5 MHz and a Setting of 25% Duty Cycle
The ADC functionality cannot be ensured if CLKADC > 1.5 MHz with `ADCn.CALIB.DUTYCYC` set to '1'.

**Workaround:** If ADC is operated with CLKADC > 1.5 MHz, `ADCn.CALIB.DUTYCYC` must be set to '0' (50% duty cycle).

**megaTinyCore note:** megaTinyCore sets CLKADC at 1.0 - 1.25 MHz. Suggest not clocking ADC above 1.5 MHz.

#### ADC Performance Degrades with CLKADC Above 1.5 MHz and VDD < 2.7V
The ADC INL performance degrades if CLKADC > 1.5 MHz and `ADCn.CALIB.DUTYCYC` set to '0' for VDD < 2.7V.

**Workaround:** None.

**megaTinyCore note:** megaTinyCore sets CLKADC at 1.0 - 1.25 MHz.  Suggest not clocking ADC above 1.5 MHz.

#### ADC Interrupt Flags Cleared When Reading RESH
`ADCn.INTFLAGS.RESRDY` and `ADCn.INTFLAGS.WCOMP` are cleared when reading `ADCn.RESH`.

**Workaround:** In 8-bit mode, read `ADCn.RESH` to clear the flag or clear the flag directly.

**megaTinyCore note:** megaTinyCore always reads the whole register, so this is not an issue unless you write your own routine for reading the ADC result in 8-bit mode.

#### Changing ADC Control Bits During Free-Running Mode not Working
If control signals are changed during Free-Running mode, the new configuration is not properly taken into account in the next measurement. This is valid for the `ADC.CTRLB`, `ADC.CTRLC`, `ADC.SAMPCTRL` registers and the `ADC.MUXPOS`, `ADC.WINLT` and `ADC.WINHT` registers.

**Workaround:** Disable ADC Free-Running mode before updating the `ADC.CTRLB`, `ADC.CTRLC`, `ADC.SAMPCTRL`,
`ADC.MUXPOS`, `ADC.WINLT` or `ADC.WINHT` registers.

**megaTinyCore note:** If you reconfigure the ADC for free running mode, you must be aware of this.

#### One Extra Measurement Performed After Disabling ADC Free-Running Mode
The ADC may perform one additional measurement after clearing `ADCn.CTRLA.FREERUN`.

**Workaround:** Write `ADCn.CTRLA.ENABLE` to '0' to stop the Free-Running mode immediately.

**megaTinyCore note:** If you reconfigure the ADC for free running mode, you must be aware of this.

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
**megaTinyCore note:** megaTinyCore only reconfigures the output pins by default on the 8-pin parts, none of those are impacted by this.

### CCL - Configurable Custom Logic
#### Connecting LUTs in Linked Mode Requires OUTEN Set to '1'
Connecting the LUTs in linked mode requires `LUTnCTRLA.OUTEN` set to '1' for the LUT providing the input source.

**Workaround:** Use an event channel to link the LUTs or do not use the corresponding I/O pin for other purposes.

#### D-latch is Not Functional
The CCL D-latch is not functional.

**Workaround:** None.

**megaTinyCore note:** This is broken almost everywhere, don't try to use the D-latch sequential logic. The D Flip Flop can be used instead for almost all use cases (as far as I can tell, the only difference is the R signal, which is not there on the D-latch). It's not like many people are using the CCL's at all, let alone sequential logic, however nifty the functionality is.

### RTC - Real-Time Counter
#### Any Write to the `RTC.CTRLA` Register Resets the RTC and PIT Prescaler
Any write to the `RTC.CTRLA` register resets the RTC and PIT prescaler.

**Workaround:** None.

**megaTinyCore note:** Neither RTC as millis source nor megaTinySleep are impacted; users manually configuring these timers on effected parts must take account of this.

#### Disabling the RTC Stops the PIT
Writing `RTC.CTRLA.RTCEN` to '0' will stop the PIT.
Writing `RTC.PITCTRLA.PITEN` to '0' will stop the RTC.

**Workaround:** Do not disable the RTC or the PIT if any of the modules are used.

**megaTinyCore note:** Neither RTC as millis source nor megaTinySleep are impacted; users manually configuring these timers on effected parts must take account of this.

### TCB - Timer/Counter B
#### Minimum Event Duration Must Exceed the Selected Clock Period
Event detection will fail if TCBn receives an input event with a high/low period shorter than the period of the selected clock source (CLKSEL in `TCBn.CTRLA`). This applies to the TCB modes (CNTMODE in `TCBn.CTRLB`) Time-Out Check and Input Capture Frequency and Pulse-Width Measurement mode.

**Workaround:** Ensure that the high/low period of input events is equal to or longer than the period of the selected clock source
(CLKSEL in `TCBn.CTRLA`).

**megaTinyCore note:** Only impacts users who reconfigure a Type B timer for input capture.

#### The TCB Interrupt Flag is Cleared When Reading CCMPH
`TCBn.INTFLAGS.CAPT` is cleared when reading `TCBn.CCMPH` instead of CCMPL.

**Workaround:** Read both `TCBn.CCMPL` and `TCBn.CCMPH`.

**megaTinyCore note:** Only impacts users who reconfigure a Type B timer for input capture and tried to read only the low byte of CCMP; impact on someone who tried that would be severe if workaround above not used. Manually clearing the interrupt flag should also work.

#### TCB Input Capture Frequency and Pulse-Width Measurement Mode Not Working with Prescaled
Clock The TCB Input Capture Frequency and Pulse-Width Measurement mode may lock to Freeze state if CLKSEL in
`TCB.CTRLA` is set to any other value than 0x0.

**Workaround:** Only use CLKSEL equal to 0x0 when using Input Capture Frequency and Pulse-Width Measurement mode.

**megaTinyCore note:** Only impacts users who reconfigure a Type B timer for these input capture modes.

#### The TCA Restart Command Does Not Force a Restart of TCB
The TCA restart command does not force a restart of the TCB when TCB is running in SYNCUPD mode. TCB is only restarted after a TCA OVF.

**Workaround:** None.

**megaTinyCore note:** No impact unless you are trying to use this unusual feature.

### TCD - Timer/Counter D
#### TCD Event Output Lines May Give False Events
The TCD event output lines can give out false events.

**Workaround:** Use the delayed event functionality with a minimum of one cycle delay.

**megaTinyCore note:** Only an issue if using TCD0 as an event source.

#### TCD Auto-Update Not Working
The TCD auto-update feature is not working.

**Workaround:** None.

**megaTinyCore note:** This is accounted for in by the analogWrite() function on pins controlled by TCD0 after it was discovered the hard way.

### TWI - Two-Wire Interface
#### TIMEOUT Bits in the `TWI.MCTRLB` Register are Not Accessible

The TIMEOUT bits in the `TWI.MCTRLB` register are not accessible from software.

**Workaround:** When initializing TWI, BUSSTATE in `TWI.MSTATUS` should be brought into IDLE state by writing 0x1 to it.

#### TWI Master Mode Wrongly Detects the Start Bit as a Stop Bit
If TWI is enabled in Master mode followed by an immediate write to the MADDR register, the bus monitor recognizes the Start bit as a Stop bit.

**Workaround:** Wait for a minimum of two clock cycles from `TWI.MCTRLA.ENABLE` until `TWI.MADDR` is written.

**megaTinyCore note:** No concern for megaTinyCore users unless you write your own I2C implementation.

#### TWI Smart Mode Gives Extra Clock Pulse
TWI Master with Smart mode enabled gives an extra clock pulse on the SCL line after sending NACK.

**Workaround:** None.

**megaTinyCore note:** No concern for megaTinyCore users unless you write your own I2C implementation, our Wire implementation doesn't use Smart Mode.

#### The TWI Master Enable Quick Command is Not Accessible
`TWI.MCTRLA.QCEN` is not accessible from software.

**Workaround:** None.

**megaTinyCore note:** No concern for megaTinyCore users unless you write your own I2C implementation, our Wire implementation doesn't use that either.

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

**megaTinyCore note:** This impacts versions of megaTinyCore prior to 2.0.6 when calling Serial.end() on effected devices.

#### Full Range Duty Cycle Not Supported When Validating LIN Sync Field
For the LIN sync field, the USART is validating each bit to be within ±15% instead of the time between falling edges as described in the LIN specification. This allows a minimum duty cycle of 43.5% and a maximum duty cycle of 57.5%.

**Workaround:** None.

**megaTinyCore note:** Only a concern if reconfiguring USART for LIN.

#### Frame Error on a Previous Message May Cause False Start Bit Detection
A false start bit detection will trigger if receiving a frame with `RXDATAH.FERR` set and reading the RXDATAL before the RxD line goes high.

**Workaround:** Wait for the RXD pin to go high before reading RXDATA, for instance, by polling the bit in `PORTn.IN` where the RXD
pin is located.
**megaTinyCore note:** This impacts our serial implementation on effected parts - we don't even read RXDATAH or report framing errors - but if you are receiving framing errors, the baud rates are probably mismatched, so it is likely that the received data garbage anyway.

#### Open-Drain Mode Does Not Work When TXD is Configured as Output
When the USART TXD pin is configured as an output, it can drive the pin high regardless of whether the Open-Drain mode is enabled or not.

**Workaround:** Configure the TXD pin as an input by writing the corresponding bit in PORTx.DIR to '0' when using Open-Drain
mode.
**megaTinyCore note:** We don't provide a wrapper around this mode, though it could be enabled manually, in which case it would be an issue on effected parts.
