# Some technical notes from Microchip
Many of the technical briefs from Microchip are... maybe not the most helpful documents. Some of them aren't totally worthless however, and these are some that look like the might be more useful than average.  Obviously, the official titles of these documents are different from the titles of the links in almost every case. These are but a few of many. [Dont bother with the app note section, find the product page, and scroll down](https://www.microchip.com/)

## CCL
Microchip has in the past demonstrated a complete lack of creativity in the use of the CCL. Thankfully, they've been waking up to the power of the CCL - though sadly there's no indicaion that the functionality might be expanded any time soon. I've got over a dozen features on my wish-list for the CCL ranging from specific inputs (no way to get TCA WO3-5? No TCB 3 or 4?), to a shift register that doesn't involve taking over an SPI, to a one or more CCL++ blocks on a chip that have a 16-bit truth table and a fourth input (that's about the largest that the truth table doesn't become awkwardly large for. )
* [Their version of CCL-driven WS2812B - Not quite how I plan to implement it (their implementation requires a specific system clock, but makes more efficient use of peripherals)](https://github.com/microchip-pic-avr-examples/avr128da48-cnano-ws2812-mplab-mcc), but is in a similar vein. Except they want to do it while keeping the SPI in master mode - while half of my interest is in the potential for
* [How to generate morse code... using almost every peripheral on a 48-pin DX](https://github.com/microchip-pic-avr-examples/avr128da48-cnano-sos-training-mcc)
* ["Biphase Decoder"](https://github.com/microchip-pic-avr-examples/avr128da48-cnano-biphase-mplab-mcc/tree/master/Biphase_Decoder)
* ["Manchester Encoder"](https://github.com/microchip-pic-avr-examples/avr128da48-cnano-manchester-mplab-mcc/tree/master/Manchester_Encoder)
* Notice that these don't talk about doing dumb things like I do in some of the examples, like inverting the feedback input just to see how fast it oscillates (and certainly not piping the result to the external clock input and clocking the chip from it!).

## TCD - there ain't so much here
Let's be fair, the TCD IS the most complicated peripheral on any 8-bit AVR that isn't called an xMega (over on those parts, a peripheral like the TCD would stand out for it's simplicity and ease of use... xMega didn't seem to take off like they hoped. It's not entirely clear whether that had anything to do with the steep learning curve of all those whackjob peripherals, which achieved little gain in capability for a great deal of complexityh, or if it was some other reason. There is no shortage of other objections to the design principles at work: they couldn't be run at 5v, they cost a damned fortune, and they were basically trying to move up the compute-power/memory ladder but met fierce resistance from numerous well ARMed natives). Anyway, the TCD is the most complex peripheral measured by length of documentation, and as someone who has worked with every peripheral on these parts in some capacity, I would opine that that quantization understates it's complexity: between the synchronization-related effects and the rogues gallery of strange features, this one is a real doozy. If you can figure out how to use it though, it is an incredibly powerful timer.  Their documentation group has attempted to make sense out of the TCD for people with this largely useless docucuemnt:
* ["Getting started with the TCD"](https://ww1.microchip.com/downloads/en/Appnotes/TB3212-Getting-Started-with-TCD-DS90003212.pdf) (an accurate name - it won't get you anywhere near the finish line)
* I'm not even sure what documentation I would want to see out of them. What bothers me isn't anything in specific, it's just that there seems to be no rhyme or reason to this timer... I think more than anything a what the TCD needs is a rationalization of all it's features, how they come together and how they were envisioned to be used. Like those fault actions? What what the intended use case? Something that said "This is how you might make it control a DC-DC converter and here's how the fault inputs would be useful there, here's how you'd control a BLDC motor, and this type of feedback might be used with this sort of fault input scheme"

## OPAMPs for the DB-series
These are probably the most useful of the documents, because the OPAMPs are so novel, and many of us (us being the denizens of Ardino-land) aren't as familiar with analog stuff.
* [Opamp Calibration](https://ww1.microchip.com/downloads/en/Appnotes/GainOffsetCalib-AnalogSignalCond-OPAMP-Periph-DS00003633A.pdf)
* [Constant current driver with OPAMP](https://ww1.microchip.com/downloads/en/Appnotes/ConstCurr-Driver-AnalogSignalCond-OPAMP-Periph-DS00003632A.pdf)
* [That "use the DAC and OPAMP for MVIO" function marketing can't shut up about](https://ww1.microchip.com/downloads/en/Appnotes/Using-Int-OPAMP-Regulated-PowerSup-MVIO-DS00003636A.pdf)
* [Overcurrent protection and OPAMPS](https://ww1.microchip.com/downloads/en/Appnotes/AN3860-OverCurrProt-using-int-OPAMPs-on-AVRDB-DS00003860.pdf)
* [General OPAMP techniques](https://ww1.microchip.com/downloads/aemDocuments/documents/MCT08/ApplicationNotes/ApplicationNotes/Maximizing-the-Signal-Embedded-Operational-Amplifiers-90003110A.pdf)

## ADC tips and tricks
There are tips and tricks for running the ADC. They appear to have been written under strict orders not to provide any quantitative recommendations. The things I was most hoping for (things like "The latest parts have lika a million tunables - which ones should I pick if I want to best measure an analog signal of ____ impedance that changes at ___ Hz (or ___ V/s)? " amd "When I accumulte values on 2-series or Ex, how many of the low bits are signal, and how many are noise, and what can I do to maximize that ratio?", "Is the theoretical 17-bit accuracy from 1024x oversampling and decimation anything other than marketing hype?", "Can you make any quantitative recommendation whatsoever on selection of any of these 'tunable' options?") are not covered - *at all*. . It's a bit like reading a non-classified summary of some highly classified document.
* [General hints of dubious value](https://ww1.microchip.com/downloads/aemDocuments/documents/MCU08/ApplicationNotes/ApplicationNotes/Max-Signal-Properly-TipsTrick-Acq-Analog-Signals-DS00004225.pdf)
* [DSP on an 8-bit AVR](https://ww1.microchip.com/downloads/en/Appnotes/Digital-Signal-Proc-Perform-AVR8Core-DS00002701B.pdf)

### EA-series and 2-series only
These don't give very useful answers to the several "tunables" on the 2-series which may or may not be present on the EA-series (they seem to not be listed in the ATPACK headers, which is strange)
* [12-bit differental ADC (EA/2-series only in burst mode)](https://ww1.microchip.com/downloads/en/Appnotes/TB3254-How-to-Use-the-12-Bit-Differential-ADC-Burst-Accumulation-Mode-DS90003254A.pdf)
* [12-bit differental ADC (EA/2-series only in single mode with the PGA mode](https://ww1.microchip.com/downloads/en/Appnotes/TB3256-How-to-Use-the-12-Bit-Differential-ADC-with-PGA-in-Single-Mode-DS90003256A.pdf)
* [12-bit differental ADC (EA/2-series only in addumulation mode](https://ww1.microchip.com/downloads/en/Appnotes/TB3257-How-to-Use-the-12-Bit-Differential-ADC-in-Series-Accumulation-Mode-DS90003257A.pdf)
* [Example case of a force sensiive resistor and the 12-bit ADC](https://ww1.microchip.com/downloads/en/Appnotes/AN3408-How-to-Use-Force-Sensitive-Resistor-with-12-bit-ADC-DS00003408A.pdf)

## Other
* [A nice review of ways to use matrix keypads](https://ww1.microchip.com/downloads/en/Appnotes/00003407A.pdf)
* [Just because you can make your own buck converters doesn't mean you should - but here's some information for those who insist](https://ww1.microchip.com/downloads/en/Appnotes/Buck-ConvDesign-Feedback-Ctrl-Using-CIP-DS00003725A.pdf) Note that they use the type B timer here, not the type D one, which is supposed to be made to cater to such uses...

## When all else fails, read the documentation
When I refer to the actual documentation, I refer to three critical documents. Two of them are essential reading for anyone stepping an inch outside of the Arduino API, and is specific to the chip, and one far more general, esoteric, and enlightening (this is not the same as being useful for programming tasks)

* Be sure to read the datasheet. Then, read the relevant chapters a few more times. They don't just write those things for fun, and frankly thee datasheets aren't about to win any awards for good technical writing. They're not bad (which in and of itself is unfortunately unusual), but they're not great, amd it takes a few passes to truly grok the concepts that guided these designs (The designers took instructions from the managers, designed the product, and then told the technical writing team how it worked (and if they're anything like the engineers I know, they did a lousy job of explaining it). Product briefs on the other hand (like the clothing of the same name) cover only the most important elements: The pinout, feature list, and I/O Multiplexing and Considerations chart - which is, far and away, the part you will most often refer to while working with the modern AVRs.

* The headers are an incredible resource
The [core documentation gives a general description]
In fact, because of the great consistency between modern AVRs this has proven to leave very little to the imagination. In the case of the DD, by the time the datasheets were released, there were basically no surprises left - all the peripherals were known from the DB. The only new information in the datasheet was the fact that all the pins are fully async. In the case of the EA thus far, there are a few clear, straightforward changes in the event system, and a couple of new mappings for TCA1, and 1 new mapping for TCD0... and that's about it. We know the ADC pretty well from the 2-series. At least one of the annoying tunables that they won't tell us how to use correctly is gone, replaced with "sign chopping".

* [AVR Instruction Set Reference](https://ww1.microchip.com/downloads/en/DeviceDoc/AVR-InstructionSet-Manual-DS40002198.pdf)
If working with assembly you should make a rigorous study of the insruction set reference. One of the great strengths of AVR is the simplicity of the instruction set. You should know all the mneumonics, what arguments they take and what constraints apply - and this does not take a terribly great deal of effort, as there are not that many instructions. This is the Bible of the AVR architecture. Much like any other tome of the sort, it is packed with insight and wisdom from the creators, and describe in detail considerations you must observe, and some parts are not readily applicable to the present day`*`, and reading it is a long slog through obtuse verbiage and grammar. Unlike other more famous scriptures, you won't suffer eternal damnation for not reading this - but if you're writing assembly without it you may not be able to tell the difference. Even those who merely write C should try to make time for a cursory study of this, because this is what the compiler is turning your C into, and you can sometimes make choices that lead to the compiler being able to make better code.

### The datasheets giveth
* [tinyAVR 2-series](https://ww1.microchip.com/downloads/en/DeviceDoc/ATtiny1624-26-27-DataSheet-DS40002234B.pdf). Written for the 16k parts but applies to 32k ones as well. 
* [tinyAVR 1+Series](http://ww1.microchip.com/downloads/en/DeviceDoc/ATtiny1614-16-17-DataSheet-DS40002204A.pdf) Written for 16k parts, but the 32k parts are the same

* [tinyAVR 1-Series](https://ww1.microchip.com/downloads/en/DeviceDoc/ATtiny417-817-DataSheet-DS40001901C.pdf) Written for 817/417, but applies equally to the smaller parts with fewer pins. 

* [tinyAVR 1-series with 2 or 4k of flash] (https://ww1.microchip.com/downloads/en/DeviceDoc/ATtiny212-214-412-414-416-DataSheet-DS40002287A.pdf) 
* [tinyAVR 0-series] written for 16k parts, should apply to all. 
* [tinyAVR402/202](http://ww1.microchip.com/downloads/en/DeviceDoc/ATtiny202-402-AVR-MCU-with-Core-Independent-Peripherals_and-picoPower-40001969A.pdf)

### And the errata taketh away
* [AVR128DA Errata](https://ww1.microchip.com/downloads/en/DeviceDoc/AVR128DA-28-32-48-64-SilConErrataClarif-DS80000882D.pdf)
* [AVR64DA Errata](https://ww1.microchip.com/downloads/en/DeviceDoc/AVR64DA-28-32-48-64-SilConErrataClarif-DS80000903D.pdf)
* [AVR32DA Errata](https://ww1.microchip.com/downloads/en/DeviceDoc/AVR32DA-28-32-48-SilConErrataClarif-DS80000895D.pdf)


