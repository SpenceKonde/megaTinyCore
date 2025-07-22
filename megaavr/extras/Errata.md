# tinyAVR 0/1/2-series Errata Summary

Microcontrollers, like any other product, often have bugs which are discovered only after the product is released. Sometimes these are fixed in later "silicon revisions" - other times they are not. Frequently, silicon revisions for a given part may be released with no apparent changes to any of the issues with the part in question. These defects are documented in the "Silicon Errata" provided by the manufacturer. On the classic AVR parts, this was often a chapter near the end of the datasheet - and was generally quite short (off the top of my head, the only classic AVR that I can think of where it's frequently an issue is the ATtiny1634, where one pin can't be used as an input if the WDT is not running).

Well, for the tinyAVR 0/1-series, the list of issues is a tiny bit longer. By which I mean, the index of errata is longer than the entire errata section of most classic AVRs - likely because these were some of the first parts released with many of the new peripherals (though the megaAVR 0-series had come out slightly earlier, the 1-series had many peripherals not featured in the megaAVR 0-series, and where a peripheral was on both series, it is broken the same way over there too).

The lists of errata are ~hidden~ located in a separate document, the "Errata and datasheet clarification" sheet for the part now, instead of being in the datasheet proper like it was on classic AVRs. You know, just to make it easier for the users, so we don't have to skip over a few pages of highly relevant information on errata if we have one of the non-existent versions that doesn't have all these problems. Nope, they're *totally* not trying to hush up the bugs until you've made the buying decision and gotten too deep in the design process to switch MCUs to a competitors' one or anything, no siree - it's just to make things easier you you, the users! The specific errata relevant to a given part vary depending on the specific part and and silicon revision. The silicon revision can be read from the `SYSCFG.REVID` register (0 = A, 1 = B, etc). It can also be determined from the (undocumented) 32-byte SIB (this is how SerialUPDI reads the SIB) - the parenthecized part at the end, of the form `__._____._` where `_` represents a 1 byte character - So far the bytes in the SIB have all been either valid ASCII characters representing alphanumeric characters, spaces, `.`, `:`, and `-` - and a null terminator that broke console output when it wasn't filtered from the output sent to the console) - the first two characters, before the dot, are the rev id in hexadecimal. You can also find it without applying power by asking Microchip support what rev the part lot number corresponds to. Of course, in many cases, you don't need the chip at all to know what silicon rev it is or render the question moot. Check the latest errata sheet: For most modern AVRs, one of the following will be true
* There is only one revision released to production, so chances are that's the version you got ;-)
* There are more than one revision, but the earlier one is unknown in the wild (DB rev A4, for example, does not appear to have been sold except in tiny quantities before general availability.)
* There are more than one revision, but the list of problems associated with them is unchanged (at least for the problems you care about)
* The only parts which I know to exist in more than one revision, in significant quantities, with changes between the co are the 1-series parts that have memory sizes other than 8k.
  * The smaller ones have gotten 2 die revs. The first die rev, which came very early (practically at release) fixed a huge number of problems that no other versions of the tinyAVR 1-series had, and the second one did not correct any issues (it may have concerned "secret" errata, or it may have just been a process change). Hence unless you got very early silicon, there is effectively only one revision (the B/C revision - C fixed no publicly disclosed errata).
  * The 16k ones got the worlds most pathetic die rev - it fixed like 3 out of 25+ issues. So for most people, there is effectively only one revision.
  * The 32k 1-series are the only 1-series parts for which there are two revs in circulation which are notably different - these parts didn't have as many problems as the 16k parts to begin with, didn't do a Rev B, but got a Rev. C that fixed the RTC bug, the CCL D-latch, and the CCL LINK/OUTEN issue, and traded two bugs (USART framing error corrupting next character if register read before pin goes high, and LIN duty cycle issue were fixed (this coincided in time with the VAOs first shipping, so likely it was demanded by automotive customers), at the cost of two other USART bugs: the ISFIF bug (must turn receiver off and back on after clearing ISFIF before you can receive anything), and the (worked around by core on all parts, and widely distributed) SFDEN bug.
* No 0-series or 2-series parts have received a die rev.

```c
Serial.print("Silicon revision is: ");
Serial.println(SYSCFG.REVID);
```
Oh, and I've seen parts where SYSCFG.REVID reads as 0... *shrug* They're definitely normal chips though.

Thankfully, most of these issues will not be encountered by most Arduino users. The impact column indicates the likelihood that someone working with an impacted part through megaTinyCore and Arduino would encounter it.

* 0 - **Virtually nil** Impact is virtually nil for plausible Arduino usecases. This is typical of things that exist - likely - for the sake of the FUSA certification. The fact that the FUSA logo is still there, when there are between 6 and 24 bugs, sethe functional safety certification or failsafe industrial applications. These sorts of mechanisms frequently seem to have prioblems. I think it's something that's among the least interesting things to do rigorous testing on. Examples:
  * Flash CRC issues - We don't have a mechanism to generate the CRC internally, so we don't enable CRC. I don't think any users do either. It's one of the few things that hasn't been asked about I think.
  * The OSCLOCK "oops" ("What, you said you wanted to lock the oscillator cal! That's what we did! And *now* you tell us you meant after we write the calibration value to it?"). Yet another example of functionality that was either never tested, or was tested with such ignorance and/or laziness that they, well, i'd guess they started started it with OSCLOCK set, verified that they couldn't write to the OSCCAL register and could write to it normally "Great, it works", checking off that box, not noticing that when it was locked, the chips' oscillator was off by a mile..
* 1 - *Minimal Impact* Issue is transparently mitigated without undue performance impact and no API code will run afoul of it or behave differently. For features not exposed, it is a not encountered in common cases and can be worked around fully, or it's only encountered in exotic use cases, and it is only inconvenient.
* 2 - *Small impact*
Issue is mitigated in relevant API functions, but the workaround is distasteful (ex: the workaround has to go in an ISR), or it otherwise performs subtly differently than it is supposed to **OR** if it is not exposed by the core, and is either a small issue to exotic use cases or a trivial one fior uncommon use cases.
* 3 - *Moderate impact* If exposed by the core, we attemot to work around it, but there's only so much we can do. If it's not exposed, it has either a minor impact on most users, or large impact on exotic usecases. If an erratum either has no workaround (this is based on my understanding of the issue having read the errata and official workaround suggestion, not whether there is a workaround listed, because sometimes they say "Workaround: None" when there is a simple one, and other times, their workarounds are unhelpful). If, with the best workaround known to me, this erratum still results in something promised by the datasheet being impossible - rather than just less pleasant or performant - and the feature will plausibly see use in Arduinoland, this is the lowest rating it can get.
* 4 - *Serious Impact*
These issues are those that impact most or all users of the relevant peripheral, and for which no complete workaround will provide the functionality the datasheet described, and Microchip has confirmed by including it on errata or directly that they do consider it to be a bug, and this isn't just a really bad feature working as intended. Issues of 4 and 5 severity have a significant adverse impact on users.
* 5 - *Critical Impact*
This bug is impossible to avoid or nearly impossible to avoid if you are using the peripheral. We cannot work around it for the general case, if it can be worked around at all; any workaround is likely to have sideffects or have other limitations. If it's covered by the API, we don't handle it. These level of issue threaten to render parts unsuitable for impacted use cases. These may also be 4's with aggravating factors like poor description in the errata, and/or particularly perverse and/or surprising  behavior (I'm looking at the RTC)


## Errata Groups
Errata apply to a specific die. But the same die may be used on multiple parts, and on the tinyAVRs this is always the case. Helpfully, the datasheets and errata sheets are grouped by die design as well - now. Originally under Atmel they were trying to shoehorn the datasheets into pincount-based bins like they always did. As that's not how the dies were arranged, this resulted in confusing errata sheets. Microchip cleaned house here and switched to applying to the parts that actually shared a die. There are occasional surprises like the die used for the 417 being the 8k die; I guess the 2k/4k die only went up to 20 I/O's.
* tinyAVR 0-series
  * 202, 204, 402, 404, 406 - the small flash ones
  * 804, 806, 807 - 8k parts
  * 1604, 1606, 1607 - 16k parts
* tinyAVR 1-series
  * 212, 214, 412, 414, 416 - the small flash ones
  * 814, 816, 817, 417 - 8k parts
  * 1614, 1616, 1617 - 16k parts
  * 3216, 3217 - 32k parts (we very nearly got a 3214! They had even started including the IO headers for it. I mean, it's the same die as a 3217 in a 14-pin package... But they never shipped any silicon. No idea why - I'd like to think it was strategic decision based on expected market demand, rather than an "oh shit", but I have my suspicions... One imagines the development was done on the 24-pin part. We also know they were tight on die size, that's why there's no 3216 in QFN. The QFN24 is slightly wider than a SOIC-N... maybe the die just barely fit in the QFN24 they wanted to use, and although you could fit more die area into a SOIC-N package if the die were rectangular, it was forced to be square to fit the QFN).
* tinyAVR 2-series
  * all 4k/8k - Interesting, no longer worth it to separate the 4 and 8k parts into different dies.
  * all 16k - This is clearly what was used for development - it's at rev E!
  * all 32k

## The grand combined table, now on google sheets
[**The table is now in Google Sheets and publicly viewable - color coded too!**](https://docs.google.com/spreadsheets/d/1N-dDZ3WogEXRYcOip5kOREj_ExPQQRD1OrgMuywjXzw/edit?usp=sharing)




## tinyAVR 2-series
Unlike the other devices this core supports, the list of errata for the 2-series a real treat, wouldn't you say?
A couple of years post release, and only 8 errata, maybe 9.

There are 5 issues with universal or near universal distribution on modern AVRs, which were only fixed with the AVR-DD series in 2022. One of them is very frustrating if you have to deal with it, one is a rarely encountered nuisance which is still rated 3 because when you trip over it, the behavior is thoroughly perplexing to those unaware of the errata, and the others are worked around by the core automatically when using the relevant APIs.

And then there are three related to the new ADC, all of which involve LOWLAT mode.
We always enable LOWLAT by default, which sidesteps 2 of those issues unless users choose to turn it off. However, this means that unlike other modern AVRs, but like classic AVRs, **you must disable the ADC before entering sleep mode**.



## tinyAVR 0/1/2-series Errata Details

These errata details are taken verbatim from the Silicon Errata and Datasheet Clarification documents except as noted below - see the links in the [datasheet pages](Datasheets.md) - they have only been formatted for markdown, and gathered from the multiple errata sheets into a single document for ease of comparison and reference. Be sure to check that a given item applies to the part you are working with on the above tables.

For each issue, if warranted. we have added a **megaTinyCore note** describing impact on megaTinyCore users or other relevant observations. In a very small number of cases, we have annotated the text for clarity in the context of this combined errata document, these notes are *in italics* and the old wording in ~strikethrough~.


### Device

#### Write Operation Lost if Consecutive Writes to Specific Address Spaces I
An ST/STD/STS instruction to address > = 64 followed by an ST/STD instruction to address < 64 or SLPCTRL.CTRLA will cause loss of the last write.

**Work Around:**
To avoid loss of write operation, use one of the following workarounds depending on address space:
* Insert a NOP instruction before writing to address < 64, or use the OUT instruction instead of ST/STD
* Insert a NOP instruction before writing to SLPCTRL.CTRLA

**megaTinyCore notes:**
This effects all modern AVRs as of the time of writing. As you can see from the instruction set manual, AVRxt was closely modeled on AVRxm, with one of the major areas of difference being that AVRxt has changed the memory interface, trading the wacky combined read/write instructions for performance and probably die size. This bug was likely a consequence of that, in combination with the removal of the 20 working registers from the main data address space (that accounts for why it can strike addresses as high as 0x5F - rather than only going up to 0x3F).

Though some will think this bug to be mighty and dreadful, it is not so.

This is nowhere near as bad as it sounds in reality for the simple reason that this is a very uncommon access pattern: It requires an ST/STD directed at very low addresses (notice that STS is not impacted, probably because the write happens 1 clock later because it has to read the next instruction word to know where to write to). The compiler will *not emit code like that unless you force it to* (ie, use a compile time unknown pointer to access the I/O space - if it's known constant, it will be emitted as an OUT instruction if it's in the I/O space (at worst, this is the same speed, and it's sometimes faster; the compiler is likely weighted to treat it as faster in all cases) or an STS if it's not - the STS is favorable unless the pointer or a pointer less than 64 lower than it is already in one of the pointer register pairs and this fact is known at compile time and the optimization permitted - a bar almost never met), and you have to do that in the cycle immediately after a ST/STS to somewhere else to manifest the bug. Accessing the I/O space by pointer is weird. You generally know the address at compile time, and so the compiler will generate OUT instructions instead of ST/STD instructions. And for SLPCTRL.CTRLA, since you're never going to access the sleep controller with a pointer (you'd be insane to - it's strictly worse, and there's no incentive to use a pointer since there is only one register in the SLPCTRL, so the address is known at compile time and will be accessed using STS. That only SLPCTRL is listed, while RSTCTRL is at a lower address is interesting, but not implausible. I think most likely RSTCTRL would be impacted just the same, except that RSTFR is a "flag" register and may be handled differently, while the SWRR register must use a protected write sequence, which precludes this access pattern, by dictating that the instruction immediately preceding the key write is an OUT instruction writing to the CCP register in the high I/O space). Using a pointer to access an isolated variable or register of 1 byte in size is at least 1 clock slower but could be as much as 7 clocks slower (and the conditions required for this bug make the +4 and +7 clock options more likely, namely, likely having two pointers loaded at the same time. (the other possibilities are that an STS was followed immediately by a write to a pre-staged pointer to a low address. There are two other theoretical ways, but they're extreme contrivances involving writes to the page write buffer aimed at the last page that then either displace or postincrement to roll over the address. Writing to the page buffer and then the VPORTs like that does not maintain reasonable doubt - the only reason to do that is in order to manifest this bug.

I can think of two ways that real code could encounter these, and neither of them demonstrate good coding practices:
1. If you are trying to get clever about converting some sort of pin identifier scheme, where you associate a number with each pin wherein the three low bits are the bit position, and you end up with the port number in bits 5-7 (so you would mov the pin identifier to the low byte of the Y or Z register, ANDI with 0xE0. That's your PORTx base register, and it's easy to trick yourself into thinking that swapping the nybbles, left shifting once, and ANDI'ing with 0x1F is a fast and efficient way to access the pins via the VPORTS. It's actually not favored if you already have the pointer to the PORT struct assembled - in fact, it's not favored period, ever - unless you're doing it toget rid of the lookup tables. The key problem is that OUT and CBI/SBI only work when the IO register, and bit or working register, are compile time known - which they're not if you've got some identifier scheme that you're decoding!), and you would specifically have to set the PORTx registers (likely PORTx.PINnCTRL) before the VPORT register in order to manifest this bug. But once you have that PORTx base pointer, it's faster to so you shouldn't be doing this! Generally you don't want to use the VPORT registers like this.
2. If you are using pointers to access things that are strictly worse to access via pointers.

So if you avoid doing things that are poor programming practice, and which are slower than obvious alternatives, despite being unnatural or baroque, (traits which are commonly found in association with obscure but performant code). So just avoid poorly written, less performant code that is unnatural or more complicated than it needs to be, which you should already be doing. When you use poor programming practices to write unnatural, strange code, as long as it's maximally performant, it's hard to run into this.  . I thought I had several times while writing the above, but none of them were actually better when I counted up the clocks.

If anyone can come up with a case contrary to this, where if it weren't for this bug, you could make faster code, or where the most natural C code would compile to something that would trip over this, I would love to see it; I couldn't come up with one. It's easy to make examples of pointless code that trigger this, but difficult to write code that does so while maintaining reasonable doubt over whether the code is being written for any purpose other than to trigger this bug.

#### IDD Power-Down Current Consumption (16k 2-series, Rev. E only)
The IDD power-down leakage can exceed the targeted maximum value of 2 µA. Note that this maximum value is a target and not documented in the preliminary data sheet.

**Workaround:** None.

**megaTinyCore note:** The typical charachteristics paint a much better picture than this erratum. (of course, these are *typical* charachteristics) - The current stays under 0.5uA in power down sleep until around 80C, at which point it tacks sharply upward, with around 2uA at 125C and 1.8 volts, and twice that at 5.5V - though the voltage dependence is also exponential (less dramatically so) - it's only 2.5 uA at 4V.

Anyway, *the charachteristics graphs for all 2-series show the same power down current*. There has only been one rev of each released to production. So either the target was 2.0 uA across all voltages, but the parts only hit that on average at 1.8V, and the power consumption is the same on all of them (that is, they raised the target so it wasn't an erratum on the others), or does the graph show what it would be without the erratum? If so, how do they know?

#### On ~24-Pin~ *ATtiny1617* Automotive Devices Pin PC5 is Not Available

**Workaround:** Do not connect pin PC5 and disable input on pin (PORTC.PINTCRL5.ISC=0x4)

**megaTinyCore note:** The effected part is unlikely to be used with megaTinyCore

#### Writing the OSCLOCK Fuse in `FUSE.OSCCFG` to '1' Prevents Automatic Loading of Calibration
Writing the OSCLOCK fuse in `FUSE.OSCCFG` to '1' prevents the automatic loading of calibration values from the signature row. The device will run with an uncalibrated OSC20M oscillator.

**Workaround:** Do not use OSCLOCK for locking the oscillator calibration value. The oscillator calibration value can be locked by writing LOCK in `CLKCTRL.OSC`20MCALIBB to '1'.

**megaTinyCore note:** As the core does not set that fuse, only users making unusual changes to the fuse configuration need be concerned. And if you've been messing with the fuses, burn bootloader with the IDE to get it back to a known state.

#### The Temperature Sensor is Not Calibrated on Parts with Date Code 727, 728 and 1728 (Year 2017, Week 27/28)
The temperature sensor is not calibrated on parts with date code 727/728 (used on QFN packages) and 1728 (used on SOIC packages).
**Workaround:** If temperature sensor calibration data is required, devices with the affected date code may be returned through the Microchip RMA service. Devices with this date code are no longer shipped by Microchip.

**megaTinyCore note:** These are unlikely to be found in the wild anymore.

### AC - Analog Comparator (All exckusive to 0/1-series)
#### Coupling Through AC Pins
There is a capacitive coupling through the Analog Comparator. Toggling the selected positive AC pin may affect the selected negative input pin and vice versa.

**Workaround:** When the AC is disabled, configure AC.MUXCTRLA.MUXNEG to DAC or internal reference.

**megaTinyCore note:** It is not clear when this is relevant or how large the effect is, but it isn't present on very many parts. When it *is* present, assuming the effect is pronounced enough to be relevant, this must not be ignored, as it is relevant **when the analog comparator is not even enabled**. If you encounter this issue, please let me know - I can have the core automatically set AC.MUXCTRLA.MUXNEG, but nobody has tripped over it, and the parts include 4k and 2k parts so I'd rather not add a fix for an issue nobody is experiencing.


#### AC Interrupt Flag Not Set Unless Interrupt is Enabled
`ACn.STATUS.CMP` is not set if the `ACn.INTCTRL.CMP` is not set.

**Workaround:** Enable `ACn.INTCTRL.CMP` or use `ACn.STATUS.STATE` for polling.

**megaTinyCore note:** The included Comparator library does not poll the interrupt flag. Note that polling of those two bits they suggest is not equivalent - the first, which is what this errata concerns and which doesn't work, should latch to 1 when triggered, and stay that way until cleared, while the latter, which does work, will depend on the state *at that moment*. So a brief excursion would be missed.

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

**megaTinyCore note:** These two issues are potentially relevant now that we have a comparator library. Note that they impact only a small subset of parts. I suspect the AC capacitive coupling issue is the cause of the two false trigger issues - they came and went as a group.

### ADC - Analog-to-Digital Converter
Note that the 0/1-series ADC and the 2-series ADC are fundamentally very different. There is no erratum that is not specific to one or the other. All of the 2-series specific ones involve LOWLAT in some way.

#### ADC Stays Active in Sleep Modes for Low Latency Mode and Free Running Mode (2-series)
If the Low Latency bit (LOWLAT in ADCn.CTRLA) is '1', the ADC stays active when the device enters Power-Down
or Standby sleep modes. If the Free-Running bit (FREERUN in ADCn.CTRLF) is '1', the ADC continues to run in
Standby sleep mode even if the Run in Standby bit (RUNSTDBY in ADCn.CTRLA) is '0'. In both cases, the interrupts
will not trigger when the device enters Power-Down or Standby sleep mode.
**Work Around:** None

**megaTinyCore note:** We always set LOWLAT mode unless it is explicitly turned off. Therefore, **you must disable the ADC prior to entering sleep mode in order to get acceptable power consumption while sleeping**.

#### Low Latency Mode Must Be Set Before Changing ADC Configuration (2-series)
If using the low latency mode in the ADC, the initialization delay does not start for settings configured before the Low Latency (LOWLAT) bit in the Control A (ADCn.CTRLA) register. This may result in a conversion starting before the initialization time has ended and give a corrupt result.

**Work Around:** Enable the low latency bit (LOWLAT) in the Control A (ADCn.CTRLA) register at the start of ADC initialization before configuring any other register in the ADC.

**megaTinyCore note:** The core enables LOWLAT be default. ADCPowerOptions *attempts* to ensure that this erratum does not cause problems, but it is recommended that users not rely on our guardrail.

#### The PGA Initialization Delay Does Not Work Outside Low Latency Mode (2-series)
The initialization delay for the PGA does not start when the LOWLAT bit is ‘0’. This may cause a corrupt conversion
when the PGA is the module with the slowest initialization time. When using the internal references, this is not an
issue because of a slower initialization delay.

**Work Around:** Set the ADC in low latency mode by setting the Low Latency (LOWLAT) bit in the Control A (CTRLA) register to '1'.

**megaTinyCore note:** If using the PGA, but not the internal references, you must have LOWLAT mode enabled. This is done by the core by default and can be controlled with ADCPowerOptions().

#### SAMPDLY and ASDV Does Not Work Together With SAMPLEN (0/1-series)
Using `SAMPCTRL.SAMPLEN` at the same time as `CTRLD.SAMPDLY` or `CTRLD.ASDV` will cause an unpredictable sampling length.

**Workaround:** When setting `SAMPCTRL.SAMPLEN` greater than 0x0, the `CTRLD.SAMPDLY` and `CTRLD.ASDV` must be cleared.

**megaTinyCore note:** Not an issue unless you reconfigure the ADC manually. Be aware that SAMPLEN is configured by the core to roughly match expected behavior (based on other Arduino-compatible AVRs and official boards) when reading high impedance analog inputs. Thus, on effected parts, the ASDV and SAMPDLY features should not be used. The core never makes use of these features.

#### Pending Event Stuck When Disabling the ADC (0/1-series)
If the ADC is disabled during an event-triggered conversion, the event will not be cleared.

**Workaround:** Clear `ADC.EVCTRL.STARTEI` and wait for the conversion to complete before disabling the ADC.

**megaTinyCore note:** Only an issue if you reconfigure the ADC to use the event system as a start trigger, and then disable the ADC. Use suggested workaround in this case.

#### ADC Functionality Cannot be Ensured with CLKADC Above 1.5 MHz and a Setting of 25% Duty Cycle (0/1-series)
The ADC functionality cannot be ensured if CLKADC > 1.5 MHz with `ADCn.CALIB.DUTYCYC` set to '1'.

**Workaround:** If ADC is operated with CLKADC > 1.5 MHz, `ADCn.CALIB.DUTYCYC` must be set to '0' (50% duty cycle).

**megaTinyCore note:** megaTinyCore sets CLKADC at 1.0 - 1.25 MHz. As far as I can tell, 1.5MHz is the maximum ADC clock specified. So this "erratum" would appear to be inapplicable unless an individual is using the device in a manner contrary to the recommended operating conditions outlined in the datasheet.

#### ADC Performance Degrades with CLKADC Above 1.5 MHz and VDD < 2.7V (0/1-series)
The ADC INL performance degrades if CLKADC > 1.5 MHz and `ADCn.CALIB.DUTYCYC` set to '0' for VDD < 2.7V.

**Workaround:** None.

**megaTinyCore note:** megaTinyCore sets CLKADC at 1.0 - 1.25 MHz. As far as I can tell, 1.5MHz is the maximum ADC clock specified. So this "erratum" would appear to be inapplicable unless an individual is using the device in a manner contrary to the recommended operating conditions outlined in the datasheet.

#### ADC Interrupt Flags Cleared When Reading RESH (0/1-series)
`ADCn.INTFLAGS.RESRDY` and `ADCn.INTFLAGS.WCOMP` are cleared when reading `ADCn.RESH`.

**Workaround:** In 8-bit mode, read `ADCn.RESH` to clear the flag or clear the flag directly.

**megaTinyCore note:** megaTinyCore always reads the whole register, so this is not an issue unless you write your own routine for reading the ADC result in 8-bit mode.

#### Changing ADC Control Bits During Free-Running Mode not Working (0/1-series)
If control signals are changed during Free-Running mode, the new configuration is not properly taken into account in the next measurement. This is valid for the `ADC.CTRLB`, `ADC.CTRLC`, `ADC.SAMPCTRL` registers and the `ADC.MUXPOS`, `ADC.WINLT` and `ADC.WINHT` registers.

**Workaround:** Disable ADC Free-Running mode before updating the `ADC.CTRLB`, `ADC.CTRLC`, `ADC.SAMPCTRL`,
`ADC.MUXPOS`, `ADC.WINLT` or `ADC.WINHT` registers.

**megaTinyCore note:** If you reconfigure the ADC for free running mode and were hoping to configure it while it was live, you must be aware of this.

#### One Extra Measurement Performed After Disabling ADC Free-Running Mode (0/1-series)
The ADC may perform one additional measurement after clearing `ADCn.CTRLA.FREERUN`.

**Workaround:** Write `ADCn.CTRLA.ENABLE` to '0' to stop the Free-Running mode immediately.

**megaTinyCore note:** If you reconfigure the ADC for free running mode, you should be aware of this, the workaround is easy.

#### ADC Wake-Up with WCOMP
When waking up from STANDBY sleep mode with ADC WCOMP interrupt, the ADC is disabled for a few cycles
before the device enters ACTIVE mode. A new INITDLY is required before the next conversion.

**Workaround:** Use INITDLY before the next conversion.

**megaTinyCore note:** And how do you force it to wait for another INITDLY?

### PORTMUX - Port Multiplexer
#### Selecting Alternative Output Pin for TCA0 Waveform Output 0-2 also Changes Waveform Output 3-5 (0/1-series, small flash sizes)
Selecting alternative output pin for TCA0 in PORTMUX.CTRLC does not work as described when TCA0 operates in split mode.
* Writing PORTMUX.CTRLC bit 0 to '1' will shift the pin position for both WO0 and WO3
* Writing PORTMUX.CTRLC bit 1 to '1' will shift the pin position for both WO1 and WO4
* Writing PORTMUX.CTRLC bit 2 to '1' will shift the pin position for both WO2 and WO5
PORTMUX.CTRLC[5:3] are non-functional.

**Workaround:** None.

**megaTinyCore note:** This impacts some users if the alternate timing positions are selected on the small number of effected parts

### CCL - Configurable Custom Logic
#### The CCL Must be Disabled to Change the Configuration of a Single LUT
To reconfigure a LUT, the CCL peripheral must be disabled (write ENABLE in CCL.CTRLA to ‘0’). Writing ENABLE to ‘0’ will disable all the LUTs, and affects the LUTs not under reconfiguration.

**Workaround:** None

**megaTinyCore note:** I had assumed this annoying behavior was intended. But no, it's not. Apparently this behavior was not intended, and they apparently accidentally put a second interlock they didn't want in.  Took a long time to get fixed, and tinies with this fixed are not yet available.

#### Connecting LUTs in Linked Mode Requires OUTEN Set to '1'
Connecting the LUTs in linked mode requires `LUTnCTRLA.OUTEN` set to '1' for the LUT providing the input source.

**Workaround:** Use an event channel to link the LUTs or do not use the corresponding I/O pin for other purposes.

**megaTinyCore note:** - I have rated the severity a 3 on 1-series and 5 on 0-series, because they only have 2 event channels total that can carry async signal.

#### D-latch is Not Functional (0/1-series)
The CCL D-latch is not functional.

**Workaround:** None.

**megaTinyCore note:** This is broken on anything released before 2020. Don't try to use the D-latch sequential logic on 0/1-series parts. The D Flip-Flop can be used instead for many use cases, though it is clocked, which may make it less convenient than the latch. In any case, that's the less useful kind of latch anyway for most CCL use cases, thankfully.

### RTC - Real-Time Counter (All exckusive to 0/1-series - 2-series RTC works fine)
These errata are *extremely nasty* if you're using the RTC and are not aware of them!

#### Any Write to the `RTC.CTRLA` Register Resets the RTC and PIT Prescaler (0/1-series)
Any write to the `RTC.CTRLA` register resets the RTC and PIT prescaler.

**Workaround:** None.

**megaTinyCore note:** Neither RTC as millis source nor megaTinySleep are impacted. However, any use of the RTC other than those provided by the core has a high chance of encountering this. Additionally, the behavior that results is just wacky. We had a user who was just going nuts with this and the other RTC bug. Thia issue is particularly dangerous, as it can lead to code that implicitly requires this erratum to function.

#### Disabling the RTC Stops the PIT (0/1-series)
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

**megaTinyCore note:** Only impacts users who reconfigure TCA0 and use RESTART commands or events while the timer is counting down, a small crosssection of all users, and especially megaTinyCore users. Impacts every released part with a TCA up until the release of the AVR DD-series. What may be confusing to readers is what the problem is, because it works exactly as the datasheet specifies: *"The software can force a restart of the current waveform period by issuing a RESTART command. In this case, the counter, **direction**, and all compare outputs are set to '0'."* The issue, however, is that this behavior is considered wrong, and was documented as-it-behaves, rather than as-it-was-intended-to-behave. The correct behavior is that the restart event and restart should not change the the direction, and this implies that there may come a time when future die revisions correct this.

### TCB - Timer/Counter B
#### CCMP and CNT Registers Operate as 16-Bit Registers in 8-Bit PWM Mode
When the TCB is operating in 8-bit PWM mode (CNTMODE in TCBn.CTRLB is ‘0x7’), the low and high bytes for the CNT and CCMP registers operate as 16-bit registers for read and write. They cannot be read or written independently.

**Work Around:** Use 16-bit register access. Refer to the data sheet for further information.

**megaTinyCore note:** Only impacts users who reconfigure a Type B timer to get PWM - which isn't terribly useful on these parts anyway, especially considering competing demands for TCBs. Impacts every released part with a TCB as if July 2021. I had run into this several times and been confused by it, but somehow never figured out what was going on until a few weeks before it was added to errata sheets. Very irritating if you don't know about it

#### Minimum Event Duration Must Exceed the Selected Clock Period
Event detection will fail if TCBn receives an input event with a high/low period shorter than the period of the selected clock source (CLKSEL in `TCBn.CTRLA`). This applies to the TCB modes (CNTMODE in `TCBn.CTRLB`) Time-Out Check and Input Capture Frequency and Pulse-Width Measurement mode.

**Workaround:** Ensure that the high/low period of input events is equal to or longer than the period of the selected clock source
(CLKSEL in `TCBn.CTRLA`).

**megaTinyCore note:** Only impacts users who reconfigure a Type B timer for input capture, and hoped to measure events shorter than the system clock period. Such users should re-adjust their expectations.

#### The TCB Interrupt Flag is Cleared When Reading CCMPH
`TCBn.INTFLAGS.CAPT` is cleared when reading `TCBn.CCMPH` instead of CCMPL.

**Workaround:** Read both `TCBn.CCMPL` and `TCBn.CCMPH`.

**megaTinyCore note:** Only impacts users who reconfigure a Type B timer for input capture, but only cares about the low byte, not the high byte, and doesn't immediately recognize that when running into the bug that the interrupt flag isn't getting cleared and clear it manually (which is the most obvious cause and something to check first whenever sketch activity nearly hangs as soon as an interrupt is triggered, especially if it's only "nearly" hung). In addition to the suggested workaround of reading both bytes, you can also just manually clear the interrupt flag. Under typical conditions, whether you read the second byte or manually clear the flag will have the same run time - but manually clearing it will typically take 3 extra words of flash, while reading it will only take 1 or 2 extra words. `uint8_t val=TCBn.CCMP`will generate correct code. *though you probably want to store it in a uint16 since it's probably a 16-bit value!*

#### TCB Input Capture Frequency and Pulse-Width Measurement Mode Not Working with Prescaled (0/1-series)
Clock The TCB Input Capture Frequency and Pulse-Width Measurement mode may lock to Freeze state if CLKSEL in
`TCB.CTRLA` is set to any other value than 0x0.

**Workaround:** Only use CLKSEL equal to 0x0 when using Input Capture Frequency and Pulse-Width Measurement mode.

**megaTinyCore note:** Only impacts users who reconfigure a Type B timer for these input capture modes.

#### The TCA Restart Command Does Not Force a Restart of TCB
The TCA restart command does not force a restart of the TCB when TCB is running in SYNCUPD mode. TCB is only restarted after a TCA OVF.

**Workaround:** None.

**megaTinyCore note:** No impact unless you are trying to use this unusual feature.

### TCD - Timer/Counter D (All exckusive to 1-series)
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

#### Halt and wait for SW restart does not work in dual slope mode or when CMPASET = 0
Halting TCD and waiting for software restart (INPUTMODE in TCDn.INPUTCTRLA is ‘0x7’) does not work if compare value A is 0 (CMPASET in TCDn.CMPASET is ‘0x0’) or Dual Slope mode is used (WGMODE in TCDn.CTRLB is ‘0x3’).

**Workaround:** Configure the compare value A (CMPASET in TCDn.CMPASET) to be different from ‘0’ and do not use Dual Slope mode (WGMODE in TCDn.CTRLB is not ‘0x3’) when using this feature

**megaTinyCore note:** Irrelevant unless taking a very deep dive into TCD0.  This issue is almost universal on devices with a TCD.

### TWI - Two-Wire Interface (All exckusive to 0/1-series)
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

**Workaround:** Disable and re-enable the receiver when this condition is encountered.

**megaTinyCore note:** The core adopts this workaround on all parts. It is unclear how widely distributed this issue is, but as long as Serial is used, it should be automatically handled correctly.
