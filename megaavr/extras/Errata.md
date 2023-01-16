# tinyAVR 0/1/2-series Errata Summary

Microcontrollers, like any other product, often have bugs which are discovered only after the product is released. Sometimes these are fixed in later "silicon revisions" - other times they are not. Frequently, silicon revisions for a given part may be released with no apparent changes to any of the issues with the part in question. These defects are documented in the "Silicon Errata" provided by the manufacturer. On the classic AVR parts, this was often a chapter near the end of the datasheet - and was generally quite short (off the top of my head, the only classic AVR that I can think of where it's frequently an issue is the ATtiny1634, where one pin can't be used as an input if the WDT is not running).

Well, for the tinyAVR 0/1-series, the list of issues is a tiny bit longer. By which I mean, the index of errata is longer than the entire errata section of most classic AVRs - likely because these were some of the first parts released with many of the new peripherals (though the megaAVR 0-series had come out slightly earlier, the 1-series had many peripherals not featured in the megaAVR 0-series, and where a peripheral was on both series, it is broken the same way over there too).

The lists of errata are ~hidden~ located in a separate document, the "Errata and datasheet clarification" sheet for the part now, instead of being in the datasheet proper like it was on classic AVRs. You know, just to make it easier for the users, so we don't have to skip over a few pages of highly relevant information on errata if we have one of the non-existent versions that doesn't have all these problems. Nope, they're *totally* not trying to hush up the bugs until you've made the buying decision and gotten too deep in the design process to switch MCUs to a competitors' one or anything, no siree - it's just to make things easier you you, the users! The specific errata relevant to a given part vary depending on the specific part and and silicon revision. The silicon revision can be read from the `SYSCFG.REVID` register (0 = A, 1 = B, etc). It is also marked on the chip - though my understanding is that us mere mortals are not trusted with the secret of how to do so, and must have Microchip folks help with that.

Note that Rev. C of the 32k 1-series parts has been released and fixes a substantial amount of errata. It fixes some of the RTC stuff, it fixes the D-latch and the fact that you had to enable pin output to use the link output source on a downstream LUT as well as assorted other issues. Hopefully we will see these changes brought to other parts in the tinyAVR 0/1-series in the future. Two years after the preceeding sentence was written, there has been no such fix made available through a die rev on any other parts. The correlation in time of the Automotive version shipping, and this silicon revision being made, suggests that one or more major automotive customers refused to buy these parts until certain errata were corrected. One imagines a tense moment in the negotiations.

```c
Serial.print("Silicon revision is: ");
Serial.println(SYSCFG.REVID);
```

Thankfully, most of these issues will not be encountered by most Arduino users. The impact column indicates the likelihood that someone working with an effected part through megaTinyCore and Arduino would encounter it.
* 0 - Impact unlikely and non-serious if it does occur.
* 1 - Impact unlikely to cause issue or degrade functionality under ordinary conditions. Things that can only manifest under extreme corner cases are usually in this category. There are usually a number of approaches to working around these, as well,
* 2 - Impact likely when using features that are not particularly exotic, but has viable workaround and is not visible through the API provided by the core. This includes cases where the feature is inoperable but the workaround is truly trivial.
* 3 - Issue impacted functions provided by megaTinyCore or included libraries in previous versions, but worked around in current version, and will impact users of this functionality if reconfigured manually, or unavoidably impacts functionality exposed by core or included libraries, but can be worked around with little overall impact..
* 4 - Issue has a large impact on the relevant functionality in configurations other than the most common ones, without requiring an exotic corner case to trigger. In cases where the impacted features are exposed by the core or included libraries, it may not be possible to hide the issue.
* 5 - Issue has a large impact on the relevant functionality in all or the most common configurations. If that is provided by the core or library, we may be unable to hide the issue. If it involves manual configuration, that functionality will be completely and totally unusable without workarounds, and those may not hide it entirely.
* `*` - Issue is high impact but effected devices are rare
* ? - Information is so limited that the severity of the issue is difficult to assess, but likely extremely low.


## tinyAVR 2-series
Unlike the other devices this core supports, the list of errata for the 2-series a real treat, wouldn't you say?
There's one issue specific to the first-released 16k parts.

There are 5 issues with universal or near universal distribution on modern AVRs, which were only fixed with the AVR-DD series in 2022. One of them is very frustrating if you have to deal with it, one is a rarely encountered nuisance which is still rated 3 because when you trip over it, the behavior is thoroughly perplexing to those unaware of the errata, and the others are worked around by the core automatically when using the relevant APIs.

And then there are three related to the new ADC, all of which involve LOWLAT mode.
We always enable LOWLAT by default, which sidesteps 2 of those issues unless users choose to turn it off. However, this means that unlike other modern AVRs, but like classic AVRs, **you must disable the ADC before entering sleep mode**.

Erratum                                                               | Impact | 4/8k rev. B | 16k rev. E | 32k rev. A | Notes
----------------------------------------------------------------------|--------|-------------|------------|------------|-------
IDD Power-Down Current Consumption                                    |    3   |      -      |      X     |      -     | 16k parts only.
The CCL Must be Disabled to Change the Configuration of a Single LUT  |    4   |      X      |      X     |      X     | Present on all modern AVRs prior to the AVR-DD
CCMP and CNT Registers Operate as 16-Bit Registers in 8-Bit PWM Mode  |    3   |      X      |      X     |      X     | Present on all modern AVRs prior to the AVR-DD
Restart Will Reset TCA Counter Direction in NORMAL and FRQ Mode       |    1   |      X      |      X     |      X     | Present on all modern AVRs prior to the AVR-DD. works as documented. Microchip considers that incorrect.
Open-Drain Mode Does Not Work When TXD is Configured as Output        |    1   |      X      |      X     |      X     | Present on all modern AVRs prior to the AVR-DD and worked around by core automatically.
Start-of-Frame Detection Can Be Enabled in Active Mode When RXCIF Is 0|    1   |      X      |      X     |      X     | Present on many modenr AVRs. Worked around by the core automatically.
ADC Stays Active in Sleep Modes for Low Latency Mode and Free Running |    3   |      X      |      X     |      X     | Disable the ADC before entering sleep mode!!!
Low Latency Mode Must Be Set Before Changing ADC Configuration        |    1   |      X      |      X     |      X     | When reconfiguring the ADC, configure LOWLAT mode first (ADCPowerOptions() attempts to work around this.)
The PGA Initialization Delay Does Not Work Outside Low Latency Mode   |    2   |      X      |      X     |      X     | Don't turn off LOWLAT mode if using the PGA but not an internal reference.

Note that a very different errata sheet for a Rev. D of the 16k parts was distributed, with a number of issues impacting the new ADC. However, after that was published, the relevant decision makers came to their senses (whether due to customer backlash, the discovery of a more troubling issue that they never publicly disclosed in Rev. D, or saner heads prevailing in general is left to our imagination). The Rev. D "was not released to production", and those issues are all gone from the released Rev. E.


## tinyAVR 1-series
 . | . |ATtiny212 | . |ATtiny214/414 | . |ATtiny417|ATtiny1614|ATtiny3216 | .
---| --- | --- | --- | --- | --- | --- | --- | --- | ---
 . | . |ATtiny412 | . |ATtiny416 | . |ATtiny816|ATtiny1616|ATtiny3217 | .
 . | Impact | .  | . |ATtiny814 | . |ATtiny817|ATtiny1617  | . | .
Silicon Revision                                                                         | . |A|B|A|B|A,B|A|A|C
Device  | . | .  | . | .  | . | .  | . | . |.
OSCLOCK Fuse Prevents Automatic Loading of Calibration Values                            | 0 |-|-|-|-| - |X|X|X
Temperature Sensor Not Calibrated on Parts w/Date Code 727, 728 and 1728                 | 1 |-|X|-|X| - |-|-|-
**AC**  | . | .  | . | .  | . | .  | . | . |.
Coupling Through AC Pins                                                                 | 1 |-|-|-|-| X |-|-|-
AC Interrupt Flag Not Set Unless Interrupt is Enabled                                    | 1 |X|-|X|-| X |X|-|-
False Triggers May Occur Under Certain Conditions                                        | 2 |X|-|X|-| X |X|-|-
False Triggering When Sweeping Negative Input of the AC When Low Power Mode is Disabled  | 2 |-|-|-|-| X |X|-|-
**ADC**  | . | .  | . | .  | . | .  | . | . |.
ADC Functionality Cannot be Ensured with CLKADC > 1.5 MHz and a Setting of 25% Duty Cycle| 1 |X|X|X|X| X |X|X|X
ADC Interrupt Flags Cleared When Reading RESH                                            | 1 |-|-|-|-| X |X|-|-
ADC Performance Degrades with CLKADC Above 1.5 MHz and VDD < 2.7V                        | 1 |X|X|X|X| X |X|X|X
ADC Wake-Up with WCOMP                                                                   | 1 |X|-|X|-| X |X|-|-
Changing ADC Control Bits During Free-Running Mode not Working                           | 2 |X|-|X|-| X |X|-|-
One Extra Measurement Performed After Disabling ADC FreeRunning Mode                     | 2 |X|X|X|X| X |X|X|X
Pending Event Stuck When Disabling the ADC                                               | 2 |-|-|-|-| X |X|X|-
SAMPDLY and ASDV Does Not Work Together With SAMPLEN                                     | 2 |X|-|X|-| - |X|-|-
**CCL**  | . | .  | . | .  | . | .  | . | . |.
The CCL Must be Disabled to Change the Configuration of a Single LUT                     | 5 |X|X|X|X| X |X|X|X
Connecting LUTs in Linked Mode Requires OUTEN Set to ‘1’                                 | 4 |X|X|X|X| X |X|X|-
D-latch is Not Functional                                                                | 3 |X|X|X|X| X |X|X|-
**NVMCTRL**  | . | .  | . | .  | . | .  | . | . |.
In some cases, the reset values of NVMCTRLA is not 0x00. Workaround: Ingore it.          | 1 |?|?|?|?| ? |?|X|X
**PORTMUX**  | . | .  | . | .  | . | .  | . | . |.
Selecting Alternative Output Pin for TCA0 WO 0-2 also Changes WO 3-5                     | 2 |-|-|X|X| - |-|-|-
**RTC**  | . | .  | . | .  | . | .  | . | . |.
Any Write to the RTC.CTRLA Register Resets the RTC and PIT Prescaler                     | 4 |X|X|X|X| X |X|X|-
Disabling the RTC Stops the PIT                                                          | 5 |X|X|X|X| X |X|X|-
**TCA**  | . | .  | . | .  | . | .  | . | . |.
Restart Will Reset Counter Direction in NORMAL and FRQ Mode                              | 1 |X|X|X|X| X |X|X|X
**TCB**  | . | .  | . | .  | . | .  | . | . |.
CCMP and CNT Registers Operate as 16-Bit Registers in 8-Bit PWM Mode                     | 3 |X|X|X|X| X |X|X|X
Minimum Event Duration Must Exceed the Selected Clock Period                             | 1 |X|X|X|X| X |X|X|X
The TCB Interrupt Flag is Cleared When Reading CCMPH                                     | 2 |X|-|X|-| X |X|-|-
TCB Input Capture Frequency and Pulse-Width Measurement Mode w/Prescaled Clock No Work   | 1 |X|-|X|-| X |X|-|-
The TCA Restart Command Does Not Force a Restart of TCB                                  | 1 |X|X|X|X| X |X|X|X
**TCD**  | . | .  | . | .  | . | .  | . | . |.
TCD Auto-Update Not Working                                                              | 3 |-|-|-|-| X |X|-|-
TCD Event Output Lines May Give False Events                                             | 1 |-|-|-|-| X |X|-|-
Asynchronous Input Events Not Working When TCD Counter Prescaler is Used                 | 2 |X|X|X|X| X |X|X|X
Halt and wait for SW restart does not work in dual slope mode or when CMPASET = 0.       | 1 |X|X|X|X| X |X|X|X
**TWI**  | . | .  | . | .  | . | .  | . | . |.
TIMEOUT Bits in the TWI.MCTRLB Register are Not Accessible                               | 1 |X|-|X|-| X |X|-|-
TWI Smart Mode Gives Extra Clock Pulse                                                   | 2 |X|-|X|-| X |X|-|-
TWI Master Mode Wrongly Detects the Start Bit as a Stop Bit                              | 2 |X|-|X|-| X |X|-|-
The TWI Master Enable Quick Command is Not Accessible                                    | 1 |X|-|X|-| X |X|-|-
**USART**  | . | .  | . | .  | . | .  | . | . |.
TXD Pin Override Not Released When Disabling the Transmitter (core works around)         | 2 |X|X|X|X| X |X|X|X
Full Range Duty Cycle Not Supported When Validating LIN Sync Field                       | 1 |-|-|-|-| - |X|-|-
Frame Error on a Previous Message May Cause False Start Bit Detection                    | 1 |X|X|X|X| X |X|X|-
Open-Drain Mode Does Not Work When TXD is Configured as Output                           | 2 |X|X|X|X| X |X|X|X
Start-of-Frame Detection Can Unintentionally Be Enabled in Active Mode When RXCIF = 0 `*`| 2 |?|?|?|?| X |?|-|X
Receiver non-functional after inconsistent start frame field (core works around) `*`     | 2 |?|?|?|?| ? |?|-|X

`*` - Since the problems came at the same time as the fix for the "frame error on prev. message..." issue listed just above on some parts, while other parts list the start of frame and frame error errata on the same die rev, something strange is going on.
I have seen the SFDEN erratum appear and disappear from silicon errata lists, Owing to this strange mess,  The code currently assumes all parts are impacted. I currently do not trust the silicon errata lists on this mattter, as the issue has beem added and removed from the same silicon errata listings. Assume all parts have it unless proven otherwise.


## Automotive tinyAVR 1-series

 . |. | ATtiny212/214 | ATtiny814 | ATtiny416/417 | ATtiny1614 | ATtiny3216
--- | --- | --- | --- | --- | --- | ---
 . |. | ATtiny412/414 | . | ATtiny816/817 | ATtiny1616 | ATtiny3217
 . | Impact | . | . | . | ATtiny1617  | . | .
Silicon Revision                                                       | . | B |A,B|A,B| A | B
Device  | . | . | . | . | . | .
On 24-Pin Automotive Devices Pin PC5 is Not Available                  | * | - | - | - | X | -
OSCLOCK Fuse Prevents Automatic Loading of Calibration Values          | 0 | X | - | - | X | X
Temp Sensor Not Calibrated on Parts w/Date Code 727, 728 and 1728      | 1 | X | - | - | - | -
**AC**  | . | . | . | . | . | .
Coupling Through AC Pins                                               | 2 | - | X | X | - | -
AC Interrupt Flag Not Set Unless Interrupt is Enabled                  | 2 | - | X | X | X | -
False Triggers May Occur Under Certain Conditions                      | 2 | - | X | X | X | -
False Triggering When Sweeping Negative Input of the AC w/out LPMODE   | 1 | - | X | X | X | -
**ADC**  | . | . | . | . | . | .
ADC Functionality w/CLKADC > 1.5 MHz and a Setting of 25% Duty Cycle   | 1 | X | X | X | X | X
ADC Interrupt Flags Cleared When Reading RESH                          | 1 | - | X | X | X | -
ADC Performance Degrades with CLKADC Above 1.5 MHz and VDD < 2.7V      | 1 | - | X | - | - | -
ADC Wake-Up with WCOMP                                                 | . | - | X | X | X | -
Changing ADC Control Bits During Free-Running Mode not Working         | 2 | - | X | X | X | -
One Extra Measurement Performed After Disabling ADC FreeRunning Mode   | 2 | X | X | X | X | X
Pending Event Stuck When Disabling the ADC                             | 1 | - | X | X | X | -
SAMPDLY and ASDV Does Not Work Together With SAMPLEN                   | 2 | - | - | - | X | -
**CCL**  | . | . | . | . | . | .
The CCL Must be Disabled to Change the Configuration of a Single LUT   | 5 | X | X | X | X | X
Connecting LUTs in Linked Mode Requires OUTEN Set to '1'               | 4 | X | X | X | X | -
D-latch is Not Functional                                              | 3 | X | X | X | X | -
**PORTMUX**  | . | . | . | . | . | .
Selecting Alternative Output Pin for TCA0 WO 0-2 also Changes WO 3-5   | 3 | X | - | - | - | -
**RTC**  | . | . | . | . | . | .
Any Write to the RTC.CTRLA Register Resets the RTC and PIT Prescaler   | 4 | X | X | X | X | -
Disabling the RTC Stops the PIT                                        | 5 | X | X | X | X | -
**TCA**  | . | .  | . | .  | . | .  | .
Restart Will Reset Counter Direction in NORMAL and FRQ Mode            | 1 | X | X | X | X | X
**TCB**  | . | . | . | . | . | .
CCMP and CNT Registers Operate as 16-Bit Registers in 8-Bit PWM Mode   | 4 | X | X | X | X | X
Minimum Event Duration Must Exceed the Selected Clock Period           | 1 | X | X | X | X | X
The TCB Interrupt Flag is Cleared When Reading CCMPH                   | 2 | - | X | X | X | -
Input Capture Frequency and Pulse-Width Mode Not Working w/Prescaled Clk|2 | - | X | X | X | -
The TCA Restart Command Does Not Force a Restart of TCB                | 1 | X | X | X | X | X
**TCD**  | . | . | . | . | . | .
TCD Auto-Update Not Working                                            | 3 | - | X | X | X | -
TCD Event Output Lines May Give False Events                           | 1 | - | X | X | X | -
Asynchronous Input Events Not Working When TCD Counter Prescaler is Used|2 | X | X | X | X | X
**TWI**  | . | . | . | . | . | .
TIMEOUT Bits in the TWI.MCTRLB Register are Not Accessible             | 1 | - | X | X | X | -
TWI Smart Mode Gives Extra Clock Pulse                                 | 1 | - | X | X | X | -
TWI Master Mode Wrongly Detects the Start Bit as a Stop Bit            | 1 | - | X | X | X | -
The TWI Master Enable Quick Command is Not Accessible                  | 1 | - | X | X | X | -
**USART**  | . | . | . | . | . | .
TXD Pin Override Not Released When Disabling the Transmitter           | 3 | X | X | X | X | X
Full Range Duty Cycle Not Supported When Validating LIN Sync Field     | 1 | X | - | - | X | -
Frame Error on a Previous Message May Cause False Start Bit Detection  | 1 | X | X | X | X | -
Open-Drain Mode Does Not Work When TXD is Configured as Output         | 2 | X | X | X | X | X
Start-of-Frame Detection Can Be Enabled in Active Mode When RXCIF Is 0 | 2 | X?| X?| X?| X?| X?
Receiver non-functional after inconsistent start frame field           | 2 | ? | ? | ? | ? | ?

## tinyAVR 0-series Errata
 . | . |ATtiny202|ATtiny204|ATtiny202/204|ATtiny804/806/807
---  |---| --- | --- | --- | ---
 . |. |ATtiny402|ATtiny404|ATtiny402/404/406|ATtiny1604/1606/1607
 . | Impact | . |ATtiny406|Automotive|Both
Silicon Revision                                                          | . | B | B | B | A
Device | . | . | . | . | .
Temp Sensor Not Calibrated on Parts w/Date Code 727, 728 and 1728         | 1 | X | X | X | -
**ADC**  | . | . | . | . | .
ADC Functionality Cannot be Ensured with CLKADC > 1.5 MHz & 25% Duty Cycle| 1 | X | X | X | X
ADC Performance Degrades with CLKADC Above 1.5 MHz and VDD < 2.7V         | 1 | X | X | - | X
One Extra Measurement Performed After Disabling ADC FreeRunning Mode      | 1 | X | X | X | X
Pending Event Stuck When Disabling the ADC                                | 1 | - | - | - | X
**CCL**  | . | . | . | . | .
The CCL Must be Disabled to Change the Configuration of a Single LUT      | 5 | X | X | X | X
Connecting LUTs in Linked Mode Requires OUTEN Set to ‘1’                  | 4 | X | X | X | X
D-latch is Not Functional                                                 | 3 | X | X | X | X
**PORTMUX**  | . | . | . | . | .
Selecting Alternative Output Pin for TCA0 WO 0-2 also Changes WO 3-5      | 3 | - | X | X | -
**RTC**  | . | . | . | . | .
Any Write to the RTC.CTRLA Register Resets the RTC and PIT Prescaler      | 4 | X | X | X | X
Disabling the RTC Stops the PIT                                           | 5 | X | X | X | X
**TCA**  | . | .  | . | .  | . | .
Restart Will Reset Counter Direction in NORMAL and FRQ Mode               | 1 | X | X | X | X
**TCB**  | . | . | . | . | .
CCMP and CNT Registers Operate as 16-Bit Registers in 8-Bit PWM Mode      | 4 | X | X | X | X
Minimum Event Duration Must Exceed the Selected Clock Period              | 1 | X | X | X | X
The TCA Restart Command Does Not Force a Restart of TCB                   | 1 | X | X | X | X
**USART**  | . | . | . | . | .
TXD Pin Override Not Released When Disabling the Transmitter              | 3 | X | X | X | X
Frame Error on a Previous Message May Cause False Start Bit Detection     | 1 | X | X | X | -
Open-Drain Mode Does Not Work When TXD is Configured as Output            | 2 | X | X | X | X
Start-of-Frame Detection Can  Be Enabled in Active Mode When RXCIF Is ‘0’ | 2 | X?| X?| X?| X?
Receiver non-functional after inconsistent start frame field              | 2 | ? | ? | ? | ?



## tinyAVR 0/1/2-series Errata Details

These errata details are taken verbatim from the Silicon Errata and Datasheet Clarification documents except as noted below - see the links in the [datasheet pages](Datasheets.md) - they have only been formatted for markdown, and gathered from the multiple errata sheets into a single document for ease of comparison and reference. Be sure to check that a given item applies to the part you are working with on the above tables.

For each issue, we have added a **megaTinyCore note** describing impact on megaTinyCore users. In a very small number of cases, we have annotated the text for clarity in the context of this combined errata document, these notes are *in italics* and the old wording in ~strikethrough~.


### Device
#### IDD Power-Down Current Consumption (16k 2-series, Rev. E only)
The IDD power-down leakage can exceed the targeted maximum value of 2 µA. Note that this maximum value is a target and not documented in the preliminary data sheet.

**Workaround:** None.

#### On ~24-Pin~ *ATtiny1617* Automotive Devices Pin PC5 is Not Available
On ~24-pin~ *ATtiny1617* automotive devices pin PC5 is not available.

**Workaround:** Do not connect pin PC5 and disable input on pin (PORTC.PINTCRL5.ISC=0x4)

**megaTinyCore note:** The effected part is unlikely to be used with megaTinyCore

#### Writing the OSCLOCK Fuse in `FUSE.OSCCFG` to '1' Prevents Automatic Loading of Calibration
Writing the OSCLOCK fuse in `FUSE.OSCCFG` to '1' prevents the automatic loading of calibration values from the signature row. The device will run with an uncalibrated OSC20M oscillator.

**Workaround:** Do not use OSCLOCK for locking the oscillator calibration value. The oscillator calibration value can be locked by writing LOCK in `CLKCTRL.OSC`20MCALIBB to '1'.

**megaTinyCore note:** As the core does not set that fuse, only users making unusual changes to the fuse configuration need be concerned.

#### The Temperature Sensor is Not Calibrated on Parts with Date Code 727, 728 and 1728 (Year 2017, Week 27/28)
The temperature sensor is not calibrated on parts with date code 727/728 (used on QFN packages) and 1728 (used on SOIC packages).
**Workaround:** If temperature sensor calibration data is required, devices with the affected date code may be returned through the Microchip RMA service. Devices with this date code are no longer shipped by Microchip.

**megaTinyCore note:** These are unlikely to be found in the wild anymore.

### AC - Analog Comparator
#### Coupling Through AC Pins
There is a capacitive coupling through the Analog Comparator. Toggling the selected positive AC pin may affect the selected negative input pin and vice versa.

**Workaround:** When the AC is disabled, configure AC.MUXCTRLA.MUXNEG to DAC or internal reference.

**megaTinyCore note:** It is not clear when this is relevant or how large the effect is, but it isn't present on very many parts. When it *is* present, assuming the effect is pronounced enough to be relevant, this must not be ignored, as it is relevant **when the analog comparator is not even enabled**


#### AC Interrupt Flag Not Set Unless Interrupt is Enabled
`ACn.STATUS.CMP` is not set if the `ACn.INTCTRL.CMP` is not set.

**Workaround:** Enable `ACn.INTCTRL.CMP` or use `ACn.STATUS.STATE` for polling.

**megaTinyCore note:** The included Comparator library does not poll the interrupt flag.

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

**megaTinyCore note:** These two issues are potentially relevant now that we have a comparator library. Note that they impact only a small subset of parts.

### ADC - Analog-to-Digital Converter
Note that the 0/1-series ADC and the 2-series ADC are fundamentally very different. There is no erratum that is not specific to one or the other. All of the 2-series specific ones involve LOWLAT in some way.

#### ADC Stays Active in Sleep Modes for Low Latency Mode and Free Running Mode
If the Low Latency bit (LOWLAT in ADCn.CTRLA) is '1', the ADC stays active when the device enters Power-Down
or Standby sleep modes. If the Free-Running bit (FREERUN in ADCn.CTRLF) is '1', the ADC continues to run in
Standby sleep mode even if the Run in Standby bit (RUNSTDBY in ADCn.CTRLA) is '0'. In both cases, the interrupts
will not trigger when the device enters Power-Down or Standby sleep mode.
**Work Around:** None

**megaTinyCore note:** We always set LOWLAT mode unless it is explicitly turned off. Therefor, **you must disable the ADC prior to entering sleep mode in order to get acceptable power consumption while sleeping**.

#### Low Latency Mode Must Be Set Before Changing ADC Configuration
If using the low latency mode in the ADC, the initialization delay does not start for settings configured before the Low Latency (LOWLAT) bit in the Control A (ADCn.CTRLA) register. This may result in a conversion starting before the initialization time has ended and give a corrupt result.

**Work Around:** Enable the low latency bit (LOWLAT) in the Control A (ADCn.CTRLA) register at the start of ADC initialization before configuring any other register in the ADC.

**megaTinyCore note:** The core enables LOWLAT be default. ADCPowerOptions *attempts* to ensure that this erratum does not cause problems, but it is recommended that users not rely on our guardrail.

#### The PGA Initialization Delay Does Not Work Outside Low Latency Mode
The initialization delay for the PGA does not start when the LOWLAT bit is ‘0’. This may cause a corrupt conversion
when the PGA is the module with the slowest initialization time. When using the internal references, this is not an
issue because of a slower initialization delay.

**Work Around:** Set the ADC in low latency mode by setting the Low Latency (LOWLAT) bit in the Control A (CTRLA) register to '1'.

**megaTinyCore note:** If using the PGA, but not the internal references, you must have LOWLAT mode enabled. This is done by the core by default and can be controlled with ADCPowerOptions().

#### SAMPDLY and ASDV Does Not Work Together With SAMPLEN
Using `SAMPCTRL.SAMPLEN` at the same time as `CTRLD.SAMPDLY` or `CTRLD.ASDV` will cause an unpredictable sampling length.

**Workaround:** When setting `SAMPCTRL.SAMPLEN` greater than 0x0, the `CTRLD.SAMPDLY` and `CTRLD.ASDV` must be cleared.

**megaTinyCore note:** Not an issue unless you reconfigure the ADC manually. Be aware that SAMPLEN is configured by the core to roughly match expected behavior (based on other Arduino-compatible AVRs and official boards) when reading high impedance analog inputs. Thus, on effected parts, the ASDV and SAMPDLY features should not be used. The core never makes use of these features.

#### Pending Event Stuck When Disabling the ADC
If the ADC is disabled during an event-triggered conversion, the event will not be cleared.

**Workaround:** Clear `ADC.EVCTRL.STARTEI` and wait for the conversion to complete before disabling the ADC.

**megaTinyCore note:** Only an issue if you reconfigure the ADC to use the event system as a start trigger, and then disable the ADC. Use suggested workaround in this case.

#### ADC Functionality Cannot be Ensured with CLKADC Above 1.5 MHz and a Setting of 25% Duty Cycle
The ADC functionality cannot be ensured if CLKADC > 1.5 MHz with `ADCn.CALIB.DUTYCYC` set to '1'.

**Workaround:** If ADC is operated with CLKADC > 1.5 MHz, `ADCn.CALIB.DUTYCYC` must be set to '0' (50% duty cycle).

**megaTinyCore note:** megaTinyCore sets CLKADC at 1.0 - 1.25 MHz. As far as I can tell, 1.5MHz is the maximum ADC clock specified. So this "erratum" would appear to be inapplicable unless an individual is using the device in a manner contrary to the recommended operating conditions outlined in the datasheet.

#### ADC Performance Degrades with CLKADC Above 1.5 MHz and VDD < 2.7V
The ADC INL performance degrades if CLKADC > 1.5 MHz and `ADCn.CALIB.DUTYCYC` set to '0' for VDD < 2.7V.

**Workaround:** None.

**megaTinyCore note:** megaTinyCore sets CLKADC at 1.0 - 1.25 MHz. As far as I can tell, 1.5MHz is the maximum ADC clock specified. So this "erratum" would appear to be inapplicable unless an individual is using the device in a manner contrary to the recommended operating conditions outlined in the datasheet.

#### ADC Interrupt Flags Cleared When Reading RESH
`ADCn.INTFLAGS.RESRDY` and `ADCn.INTFLAGS.WCOMP` are cleared when reading `ADCn.RESH`.

**Workaround:** In 8-bit mode, read `ADCn.RESH` to clear the flag or clear the flag directly.

**megaTinyCore note:** megaTinyCore always reads the whole register, so this is not an issue unless you write your own routine for reading the ADC result in 8-bit mode.

#### Changing ADC Control Bits During Free-Running Mode not Working
If control signals are changed during Free-Running mode, the new configuration is not properly taken into account in the next measurement. This is valid for the `ADC.CTRLB`, `ADC.CTRLC`, `ADC.SAMPCTRL` registers and the `ADC.MUXPOS`, `ADC.WINLT` and `ADC.WINHT` registers.

**Workaround:** Disable ADC Free-Running mode before updating the `ADC.CTRLB`, `ADC.CTRLC`, `ADC.SAMPCTRL`,
`ADC.MUXPOS`, `ADC.WINLT` or `ADC.WINHT` registers.

**megaTinyCore note:** If you reconfigure the ADC for free running mode and were hoping to configure it while it was live, you must be aware of this.

#### One Extra Measurement Performed After Disabling ADC Free-Running Mode
The ADC may perform one additional measurement after clearing `ADCn.CTRLA.FREERUN`.

**Workaround:** Write `ADCn.CTRLA.ENABLE` to '0' to stop the Free-Running mode immediately.

**megaTinyCore note:** If you reconfigure the ADC for free running mode, you should be aware of this, the workaround is easy.

#### ADC Wake-Up with WCOMP
When waking up from STANDBY sleep mode with ADC WCOMP interrupt, the ADC is disabled for a few cycles
before the device enters ACTIVE mode. A new INITDLY is required before the next conversion.

**Workaround:** Use INITDLY before the next conversion.

**megaTinyCore note:** And how do you force it to wait for another INITDLY?

### PORTMUX - Port Multiplexer
#### Selecting Alternative Output Pin for TCA0 Waveform Output 0-2 also Changes Waveform Output 3-5
Selecting alternative output pin for TCA0 in PORTMUX.CTRLC does not work as described when TCA0 operates in split mode.
* Writing PORTMUX.CTRLC bit 0 to '1' will shift the pin position for both WO0 and WO3
* Writing PORTMUX.CTRLC bit 1 to '1' will shift the pin position for both WO1 and WO4
* Writing PORTMUX.CTRLC bit 2 to '1' will shift the pin position for both WO2 and WO5
PORTMUX.CTRLC[5:3] are non-functional.

**Workaround:** None.

**megaTinyCore note:** megaTinyCore does not currently use the remapping functions except on 8-pin pards. Only some 14-pin parts are impacted.

### CCL - Configurable Custom Logic
#### The CCL Must be Disabled to Change the Configuration of a Single LUT
To reconfigure a LUT, the CCL peripheral must be disabled (write ENABLE in CCL.CTRLA to ‘0’). Writing ENABLE to ‘0’ will disable all the LUTs, and affects the LUTs not under reconfiguration.

**Workaround:** None

**megaTinyCore note:** I had assumed this annoying behavior was intended. It is present in literally every part that has a CCL in all silicon revisions available as of July 2021

#### Connecting LUTs in Linked Mode Requires OUTEN Set to '1'
Connecting the LUTs in linked mode requires `LUTnCTRLA.OUTEN` set to '1' for the LUT providing the input source.

**Workaround:** Use an event channel to link the LUTs or do not use the corresponding I/O pin for other purposes.

**megaTinyCore note:** - I have rated the severity a 3 on 1-series and 5 on 0-series, because they only have 2 event channels total that can carry async signal.

#### D-latch is Not Functional
The CCL D-latch is not functional.

**Workaround:** None.

**megaTinyCore note:** This is broken on anything released before 2020. Don't try to use the D-latch sequential logic on 0/1-series parts. The D Flip-Flop can be used instead for many use cases, though it is clocked, which may make it less convenient than the latch. In any case, that's the less useful kind of latch anyway for most CCL use cases, thankfully.

### RTC - Real-Time Counter
These errata are *extremely nasty* if you're using the RTC and are not aware of them!

#### Any Write to the `RTC.CTRLA` Register Resets the RTC and PIT Prescaler
Any write to the `RTC.CTRLA` register resets the RTC and PIT prescaler.

**Workaround:** None.

**megaTinyCore note:** Neither RTC as millis source nor megaTinySleep are impacted. However, any use of the RTC other than those provided by the core has a high chance of encountering this. Additionally, the behavior that results is just wacky. We had a user who was just going nuts with this and the other RTC bug. Thia issue is particularly dangerous, as it can lead to code that implicitly requires this erratum to function.

#### Disabling the RTC Stops the PIT
Writing `RTC.CTRLA.RTCEN` to '0' will stop the PIT.
Writing `RTC.PITCTRLA.PITEN` to '0' will stop the RTC.

**Workaround:** Do not disable the RTC or the PIT if any of the modules are used.

**megaTinyCore note:** Neither RTC as millis source nor megaTinySleep are impacted, HOWEVER **Any manual configuration of the RTC will be unsuccessful if this issue is not taken into account** This behaves far less simply than they describe, and it can be profoundly baffling and very nasty when using the RTC. This was reported by a user on ATtiny412; it stands to reason that on impacted parts, the other one doesn't "stop" - but rather "behaves in a perverse manner thoroughly inconsistent with documentation"
This is best exemplified by the following plausible algorithm:
  1. Set RTC clock source but don't enable it.
  2. Enable the PIT with desired period.
  3. Observe anomalous behavior or absence of behavior relating to the PIT. Decide to enable to RTC as well
  4. Attempt to reconfigure the RTC, doing the usual while(RTC.STATUS) to check that it's not busy syncing before changing RTC settings.
  5. Rather than enabling the RTC, this could would simply hang. The RTC_CNTBUSY bit would be stuck '1' indicating that a count sync was in progress. As the RTC was disabled, this condition would never be cleared, so the while loop would never exit.

### TCA - Timer/Counter A
#### Restart Will Reset Counter Direction in NORMAL and FRQ Mode
When the TCA is configured to a NORMAL or FRQ mode (WGMODE in TCAn.CTRLB is ‘0x0’ or ‘0x1’), a RESTART command or Restart event will reset the count direction to default. The default is counting upwards.

**Workaround:** None.

**megaTinyCore note:** Only impacts users who reconfigure TCA0 and use RESTART commands or events while the timer is counting down, a small crosssection of all users, and especially megaTinyCore users. Impacts every released part with a TCA up until the release of the AVR DD-series. What may be confusing to readers is what the problem is, because it works exactly as the datasheet specifies: *"The software can force a restart of the current waveform period by issuing a RESTART command. In this case, the counter, **direction**, and all compare outputs are set to '0'."* The issue, however, is that this behavior is considered wrong, and was documented as-it-behaves, rather than as-it-was-intended-to-behave. The correct behavior is that the restart event and restart should not change the the direction, and this implies that there may come a time when future die revisions "correct" this.

### TCB - Timer/Counter B
#### CCMP and CNT Registers Operate as 16-Bit Registers in 8-Bit PWM Mode
When the TCB is operating in 8-bit PWM mode (CNTMODE in TCBn.CTRLB is ‘0x7’), the low and high bytes for the CNT and CCMP registers operate as 16-bit registers for read and write. They cannot be read or written independently.

**Work Around:** Use 16-bit register access. Refer to the data sheet for further information.

**megaTinyCore note:** Only impacts users who reconfigure a Type B timer to get PWM - which isn't terribly useful on these parts anyway, especially considering competing demands for TCBs. Impacts every released part with a TCB as if July 2021. I had run into this several times and been confused by it, but somehow never figured out what was going on until a few weeks before it was added to errata sheets.

#### Minimum Event Duration Must Exceed the Selected Clock Period
Event detection will fail if TCBn receives an input event with a high/low period shorter than the period of the selected clock source (CLKSEL in `TCBn.CTRLA`). This applies to the TCB modes (CNTMODE in `TCBn.CTRLB`) Time-Out Check and Input Capture Frequency and Pulse-Width Measurement mode.

**Workaround:** Ensure that the high/low period of input events is equal to or longer than the period of the selected clock source
(CLKSEL in `TCBn.CTRLA`).

**megaTinyCore note:** Only impacts users who reconfigure a Type B timer for input capture, and hoped to measure events shorter than the system clock period. Such users shoould re-adjust their expectations.

#### The TCB Interrupt Flag is Cleared When Reading CCMPH
`TCBn.INTFLAGS.CAPT` is cleared when reading `TCBn.CCMPH` instead of CCMPL.

**Workaround:** Read both `TCBn.CCMPL` and `TCBn.CCMPH`.

**megaTinyCore note:** Only impacts users who reconfigure a Type B timer for input capture, but only cares about the low byte, not the high byte, and doesn't immediately recognize that when running into the bug that the interrupt flag isn't getting cleared and clear it manually (which is the most obvious cause and something to check first whenever sketch activity nearly hangs as soon as an interrupt is triggered, especially if it's only "nearly" hung). In addition to the suggested workaround of reading both bytes, you can also just manually clear the interrupt flag. Under typical conditions, whether you read the second byte or manually clear the flag will have the same run time - but manually clearing it will typically take 3 extra words of flash, while reading it will only take 1 or 2 extra words. `uint8_t val=TCBn.CCMP`will generate correct code.

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
The type D timer is the most complex peripheral (by number of pages, as well as subjectively) on modern AVR devices. Many of these issues were not discovered until the AVR Dx-series was released, in some cases over a year after that initial relealse. TCD0 is an extremely difficult peripheral to work with in the best of times

#### TCD Event Output Lines May Give False Events
The TCD event output lines can give out false events.

**Workaround:** Use the delayed event functionality with a minimum of one cycle delay.

**megaTinyCore note:** Only an issue if using TCD0 as an event source, and fixed on some versions of these parts. Does anyone have any idea under WHAT conditions false outputs were generated?

#### TCD Auto-Update Not Working
The TCD auto-update feature is not working.

**Workaround:** None.

**megaTinyCore note:** This is accounted for in by the analogWrite() function on pins controlled by TCD0 after it was discovered the hard way. This resulted in weeks or months of delay in some features being successfully implemented in megaTinyCore.

#### Asynchronous Input Events Not Working When TCD Counter Prescaler is Used
When the TCD is configured to use asynchronous input events (CFG in TCDn.EVCTRLx is ‘0x2’) and the TCDCounter Prescaler (CNTPRES in TCDn.CTRLA) is different from ‘0x0’ events can be missed.

**Workaround:** Use the TCD Synchronization Prescaler (SYNCPRES in TCDn.CTRLA) instead of the TCD Counter Prescaler. Alternatively, use synchronous input events (CFG in TCDn.EVCTRLx is not ‘0x2’) if the input events are longer than one CLK_TCD_CNT cycle.

**megaTinyCore note:** Irrelevant unless taking a very deep dive into TCD0. This issue is almost universal on devices with a TCD.

#### Halt and wait for SW restart does not work in dual slope mode or when CMPASET = 0.
Halting TCD and waiting for software restart (INPUTMODE in TCDn.INPUTCTRLA is ‘0x7’) does not work if compare value A is 0 (CMPASET in TCDn.CMPASET is ‘0x0’) or Dual Slope mode is used (WGMODE in TCDn.CTRLB is ‘0x3’).

**Workaround:** Configure the compare value A (CMPASET in TCDn.CMPASET) to be different from ‘0’ and do not use Dual Slope mode (WGMODE in TCDn.CTRLB is not ‘0x3’) when using this feature

**megaTinyCore note:** Irrelevant unless taking a very deep dive into TCD0.  This issue is almost universal on devices with a TCD.

### TWI - Two-Wire Interface
#### TIMEOUT Bits in the `TWI.MCTRLB` Register are Not Accessible

The TIMEOUT bits in the `TWI.MCTRLB` register are not accessible from software.

**Workaround:** When initializing TWI, BUSSTATE in `TWI.MSTATUS` should be brought into IDLE state by writing 0x1 to it.

**megaTinyCore note:** Wire.h does this.

#### TWI Master Mode Wrongly Detects the Start Bit as a Stop Bit
If TWI is enabled in Master mode followed by an immediate write to the MADDR register, the bus monitor recognizes the Start bit as a Stop bit.

**Workaround:** Wait for a minimum of two clock cycles from `TWI.MCTRLA.ENABLE` until `TWI.MADDR` is written.

**megaTinyCore note:** This is not a problem with the included Wire.h implementation - it is nowhere near that fast!

#### TWI Smart Mode Gives Extra Clock Pulse
TWI Master with Smart mode enabled gives an extra clock pulse on the SCL line after sending NACK.

**Workaround:** None.

**megaTinyCore note:** This is not a problem with the included Wire.h implementation - this feature is not used.

#### The TWI Master Enable Quick Command is Not Accessible
`TWI.MCTRLA.QCEN` is not accessible from software.

**Workaround:** None.

**megaTinyCore note:** This is not a problem with the included Wire.h implementation - this feature is not used

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

**megaTinyCore note:** Despite the issue being trivial to work around, and frequently implicitly hidden (by the second clause of the workaround above), this impacts versions of megaTinyCore prior to 2.0.6 when calling Serial.end() on effected devices.

#### Full Range Duty Cycle Not Supported When Validating LIN Sync Field
For the LIN sync field, the USART is validating each bit to be within ±15% instead of the time between falling edges as described in the LIN specification. This allows a minimum duty cycle of 43.5% and a maximum duty cycle of 57.5%.

**Workaround:** None.

**megaTinyCore note:** Only a concern if reconfiguring USART for LIN, and using the sync field for autobaud, relying upon the threshold being exactly what the standard required. As LIN is generally confined to automotive applications, and since those are - by definition - safety critical, Arduino and megaTinyCore are not certified for use in such applications. Neither Arduino not megaTinyCore is designed for the level of robustness required for safety critical roles and it is reckless and irresponsible to use these tools to develop software for such roles.

#### Frame Error on a Previous Message May Cause False Start Bit Detection
A false start bit detection will trigger if receiving a frame with `RXDATAH.FERR` set and reading the RXDATAL before the RxD line goes high.

**Workaround:** Wait for the RXD pin to go high before reading RXDATA, for instance, by polling the bit in `PORTn.IN` where the RXD
pin is located.
**megaTinyCore note:** This, technically, impacts our serial implementation on effected parts. However, if you are receiving framing errors, the baud rates or port settings are wrong on one or both side - so instead of receiving one byte of garbage, you'd receive two bytes of slightly different garbage. I do not believe there are cases where this can result in data that would have otherwise been intelligible coming out as garbage short of a transient framing error in a long string of continuous data, where it never has a chance to regain it's bearings. We have not observed adverse impacts of this and as a result have implemented no mitigation measures.

#### Open-Drain Mode Does Not Work When TXD is Configured as Output
When the USART TXD pin is configured as an output, it can drive the pin high regardless of whether the Open-Drain mode is enabled or not.

**Workaround:** Configure the TXD pin as an input by writing the corresponding bit in PORTx.DIR to '0' when using Open-Drain
mode.
**megaTinyCore note:** This is handled automatically as long as open-drain mode is enabled through the options exposed by Serial.

#### Start-of-Frame Detection Can Unintentionally Be Enabled in Active Mode When RXCIF Is ‘0’
The Start-of-Frame Detector can unintentionally be enabled when the device is in Active mode and when the Receive Complete Interrupt Flag (RXCIF) in the USARTn.STATUS register is ‘0’. If the Receive Data (RXDATA) registers are read while receiving new data, RXCIF is cleared, and the Start-of-Frame Detector will be enabled and falsely detects the following falling edge as a start bit. When the Start-of-Frame Detector detects a start condition, the frame reception is restarted, resulting in corrupt received data. Note that the USART Receive Start Interrupt Flag (RXSIF) always is ‘0’ when in Active mode. No interrupt will be triggered.

**Workaround:** Disable Start-of-Frame Detection by writing ‘0’ to the Start-of-Frame Detection Enable (SFDEN) bit in the USART Control B (USARTn.CTRLB) register when the device is in Active mode. Re-enable it by writing the bit to ‘1’ before transitioning to Standby sleep mode. This work around depends on a protocol preventing a new incoming frame when re-enabling Start-of-Frame Detection. Re-enabling Start-of-Frame Detection, while a new frame is already incoming, will result in corrupted received data.

**megaTinyCore note:** This workaround is implemented by the core, for all parts. ~Impacts every available modern AVR device as of July 2021. One really wonders how this went unnoticed until 2020.~ It remains unclear precisely which parts are impacted by this issue, and hence on which parts we are simply engaging in unnecessary voodoo practices.

#### Receiver non-functional after inconsistent start frame field
After receiving an inconsistent sync frame in autobaud mode (that is, a condition that sets ISFIF), the receiver becomes non-functional.

**Workaround:** Disable and reenable the receiver when this condition is encountered.

**megaTinyCore note:** The core adopts this workaround on all parts. It is unclear how widely distributed this issue is, but as long as Serial is used, it should be automatically handled correctly.
