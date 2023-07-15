# ADC/DAC Reference for DxCore and megaTinyCore
These parts all have a large number of analog inputs,  - plus the one on the UPDI pin which is not totally usable because the UPDI functionality). They can be read with analogRead() like on a normal AVR. While the `An` constants (ex, `A3`) are supported, and will read from the ADC channel of that number, they are in fact defined as then digital pin shared with that channel. Using the `An` constants is deprecated - the recommended practice is to just use the digital pin number, or better yet, use `PIN_Pxn` notation when calling analogRead(). Particularly with the release of 2.3.0 and tinyAVR 2-series support, a number of enhanced ADC features have been added to expose more of the power of the sophisticated ADC in these parts.

The tinyAVR 2-series is the first post-revolutionary AVR to feature a real differential ADC. *Hey! We've got differential ADC in the Dx!* No, for the last time, that is not a real differential ADC. You might as well have two single ended ADC channels tied to the same trigger and that's it. Like what was demo'ed on the 1-series tinyAVRs in fact, only on the Dx-series it's packaged more neatly and with all the auxiliary uses stomped out. A normal differential ADC channel lets you set the reference to 1v, turn the gain up to the max, and listen to the difference between the Vdd power rail and the ever so slightly higher voltage before the current sense resistor! The Dx's one is capped to V<sub>Ain</sub> <=

The ADC on the the Ex-series is even better - it is nearly the same ADC that the tinyAVR 2-series got, though it's specs have gotten a small across the board improvement, and there's a new feature that can only be used when accumulated mode is in use: Sign Chopping. Sign Chopping alternates between positive and negative inputs during an accumulated measurement as well as the sign of the value added to the total. This can greatly reduce offset error (which, particularly with gain cranked up and the reference cranked down, trying to get the most possible out of the ADC, could get surprisingly high).

**Be sure to disable to the ADC prior to putting the part to sleep if low power consumption is required!**

## Vital Statistics
| Part family | Min. Res. | Max. Res. | CLK Max | CLK used | Max Ovrsamp Res. | Extras
|-------------|-----------|-----------|---------|----------|------------------|---
| tinyAVR 0/1 |     8-bit |    10-bit | 0.2-1.5 | 1-1.25MHz|           13-bit | (1), (2), (3)
| tinyAVR 1+  |     8-bit |    10-bit | 0.2-1.5 | 1-1.25MHz|           13-bit | (1), (2), (3), (4), Whole second copy of ADC.
| AVR DA      |    10-bit |    12-bit | 0.125-2 | 1-1.4MHz |           15-bit | (5) "Differential" ADC
| AVR DB      |    10-bit |    12-bit | 0.125-2 | 1-1.4MHz |           15-bit | (5) "Differential" ADC
| AVR DD      |    10-bit |    12-bit | 0.125-2 | 1-1.4MHz |           15-bit | (5) "Differential" ADC more pins can be ADC pins.
| tinyAVR 2   |     8-bit |    12-bit |   3/6 * | 2-2.5MHz |           17-bit | (6) True differential, 1-16x PGA
| AVR EA      |     8-bit |    12-bit |   3/6 * | 2-2.5MHz |           17-bit | (6) True differential, 1-16x PGA, sign chopping.

`*` 6 MHz with external or Vdd reference, 3 MHz with internal.

Notes:
1. This part initially had aspired to function at F_ADC = 2.00 MHz, RES = 8-bits. This didn't work so well. See the errata for the marginally successful workarounds.
2. This part has a dumbass set of reference voltages (4.3, 2.5, 1.1, 1.5, 0.55). For the ultra low reference voltage, use analogSetClock() to ensure that the clock speed is within the much lower sped'ed range of 65-200 kHz. You may also want to lower sample duration since that's what it's denominated in. All other parts have 1.024/2.048/4.096/2.5
3. No external reference on 0-series and 1-series below 16k.
4. 16k (1+series) parts are blessed with a number a boons. One of them is a second ADC. I think the intent was to provide a way to approximate having a differential ADC by triggering both at once (well, one of the intents, another one was because the ADC0 is taken over by the PTC if that is in use.
5. The Dx-series differential ADC is a bad joke. First: The maximum absolute value both voltages is V<sub>ref</sub>, above that, wrong results are returned. And there's no amplification other than what you can press the opamps into service as.
  **Unsubstantiated and slanderous speculation** My theory is this: The dual ADC on the 1+ series was to get dual ADC field tested because even then they could see that the new ADC was going to be an ordeal. They figured they can get away with 1 generation of chips without explicit differential ADC, but they still needed a way to get the same effect for some major customer application
6. This is a REAL differential ADC. Max V<sub>Ain</sub> = Vdd + .1 or .2V and for useful readings their difference must be smaller than VREF. The tiny2 has some mysterious tunables, proper tuning of which is unclear. Offset error is disappointingly high, which Microchip noticed and introduced sign chopping (and removed a mysterious tunable or two) in the EA series, which should help significantly.

The differential ADC on the Dx-series is disappointing.

## Reference Voltages
Analog reference voltage can be selected as usual using analogReference(). Supported reference voltages are listed below:
In some cases the voltage determines the maximum ADC clock speed. Call analogReference() before analogClockSpeed() to ensure that the analog clock speed is appropriate when any of these apply:
* You are switching between internal and external/VDD reference on an Ex/2-series
* You are using the half volt reference on the 0/1-series, or switching from that to another reference
* You are on a Dx-series with an external reference of less than 1.8V


 | tinyAVR (0/1)                           | Voltage | Minimum Vdd | Number | Notes |
 |-----------------------------------------|---------|-------------|--------|-------|
 | `VDD` (default)                         | Vcc/Vdd |           - |     16 | . |
 | `INTERNAL0V55`                          |  0.55 V |           - |      0 | ADC clock needs to be 100kHz to 260 kHz to get accurate results |
 | `INTERNAL1v1`                           |  1.10 V |           - |      1 | . |
 | `INTERNAL2V5`                           |  2.50 V |           - |      2 | . |
 | `INTERNAL4V3`                           |  4.30 V |           - |      3 | . |
 | `INTERNAL1V5`                           |  1.50 V |           - |      4 | . |
 | `EXTERNAL`                              |       - |           - |     32 | 1+series only |

 | tinyAVR (2-series)  or Ex-series        | Voltage | Minimum Vdd | Number | Notes |
 |-----------------------------------------|---------|-------------|--------|-------|
 | `VDD` (default)                         | Vcc/Vdd |             |      0 | VDD Ref works at 6 MHz CLK ADC! |
 | `INTERNAL1V024`                         | 1.024 V |      2.5* V |      4 | 10-bit reading with 1.025 ref gives apx.   1 mv/ADC count |
 | `INTERNAL2V048`                         | 2.048 V |      2.5  V |      5 | 12-bit reading with 2.048 ref gives apx. 0.5 mv/ADC count |
 | `INTERNAL4V096`                         | 4.096 V |      4.55 V |      7 | 12-bit reading with 4.096 ref gives apx.   1 mv/ADC count |
 | `INTERNAL2V500`                         | 2.500 V |      2.7  V |      6 | . |
 | `INTERNAL4V1` (alias of INTERNAL4V096)  | 4.096 V |      4.55 V |      7 | . |
 | `EXTERNAL`                              |       - |         Vdd |      2 | External ref works at 6 MHz CLK ADC! |

 | AVR Dx/Ex-series (all)                  | Voltage | Minimum Vdd | Number | Notes |
 |-----------------------------------------|---------|-------------|--------|---------|
 | `VDD` (default)                         | Vcc/Vdd |             |      5 | . |
 | `INTERNAL1V024`                         | 1.024 V |      2.5* V |      0 | 10-bit reading with 1.025 ref gives apx.   1 mv/ADC count |
 | `INTERNAL2V048`                         | 2.048 V |      2.5  V |      1 | 12-bit reading with 2.048 ref gives apx. 0.5 mv/ADC count |
 | `INTERNAL4V096`                         | 4.096 V |      4.55 V |      2 | 12-bit reading with 4.096 ref gives apx.   1 mv/ADC count |
 | `INTERNAL2V500`                         | 2.500 V |      2.7  V |      3 | . |
 | `INTERNAL4V1` (alias of INTERNAL4V096)  | 4.096 V |      4.55 V |      2 | . |
 | `EXTERNAL`                              | >=1.0 V |         Vdd |      6 | Dx: CLK_ADC =< 500 kHz |
 | `EXTERNAL`                              | >=1.8 V |         Vdd |      6 | Dx: No CLK_ADC restriction |

 You can test like `if(getAnalogReference()==INTERNAL2V500)`, but if you try to say, print them, you just get a number. That's what is shown in the last column: contains the numerical value of the constants representing these references. Don't use those, then nobody will understand your code - including yourself in two weeks. However, if you are printing the results of `getAnalogReference()` or `getDACReference()`, these are the numbers you will see.

### Important notes regarding references

#### There is no INTERNAL
`INTERNAL` is not a supported reference, since there is no obvious reason that any of the four reference options should be used. These reference voltages are presented as they are in the datasheet, but do note that they are what the manufacturer targeted, not what the actual voltage is: The spec is +/- 4% over the temperature range of -40 to 85 C, when Vdd is at least the voltage specified above - though at typical conditions, every time I've checked, it was within 1%; The numbers they give for the voltages are specified to three decimal places (obviously that is convenient because then 1 mV = 1, 2, or 4 LSB), that should by no means be interpreted as a claim that they are *that* accurate. It is likely that the 1.024 V reference is usable below 2.5 V, but no guarantees are given. Reference voltages exceeding the supply voltage will produce meaningless results and should not be used.

#### Limits of external refeences
External reference voltage should be between 1.8v and Vdd if CLK_ADC is higher than 500kHz (by default, it is), however if you slow the ADC clock down, external references as low as 1.0V (they gave 1.024V as the lower limit. It is unclear if the slower clock speed must also be used for the internal 1.024V reference.

This is not all bad news though on the 2-series and Ex, instead of having to slow down for some reference voltages, we get higher max CLK_ADC with external references!
#### 2-series refs require slightly more headroom
From careful examination of the table above it can be seen that the minimum Vdd for a given reference voltage is slightly larger for the 2-series - You need slightly more headroom; about 0.5v, compared with around 0.4v for the 1-series. This could be a real effect (ie, increased sensitivity of the ADC to adverse operating conditions), or may simply reflect the more conservative approach that appears to have been taken to QA of 2-series parts in general (consider that 2-series parts have a total of 5 errata, with no new ones a year after release, and 4 of those impacted all modern AVRs released at the time. Compare to the 0 and 1-series each 0 and 1-series flash size has it's own grab-bag of a few dozen, most of which are on some but not all tinyAVR parts, nothing else, none of which were fixed on the last die rev for 16k 1-series, and some of which are really nasty.

#### 0/1-series-specific challenges
The reference voltages on the 1-series were awful. 0.55, 1.1, 1.5, 2.5, 4.3? None of those are convenient to work with, not compared to the new standard: 1.024, 2.048, and 4.096. So you could use
* On 0/1-series, you must be sure to slow down the ADC clock with analogClockSpeed when using the 0.55V reference, as it has a much more restricted operating clock speed range: 100 - 260 kHZ. The other references can be used at 200 - uh, well, 1500 kHz if you want accurate results according to the errata, at least on most aparts. Datasheet claim is 2 MHz but only in 8-bit res mode.

5. The External Reference option **on the 16k and 32k 1-series parts** - like most of the other features specific to just those parts - was added is a somewhat slapdash manner. They say, and I quote:
```text
When using the external reference voltage VREFA, configure ADCnREFSEL[0:2] in the corresponding VREF.CTRLn
register to the value that is closest, but above the applied reference voltage. For external references higher than
4.3V, use ADCnREFSEL[0:2] = 0x3 [4.3V].
```
And they also say that one must NOT have ADCnREFEN bit set when using an external reference, this warning was never present in the megaTinyCore docs until 1.5.12, and nobody complained. This suggests that it one of the following is true:
  a. Configuring refsel per the datasheet isn't that important after all; alternately, it may just make the reference less accurate, or otherwise throw off the accuracy enough to keep it from meeting spec, but not by enough that people who triggered it noticed.
  b. Failure to configure refsel per the datasheet causes only poor ADC accuracy, and the few individuals using an external reference either read the relevant paragraph in the datasheet and configured refsel or else thought "Damn the ADC on these parts sucks" but kept that observation to themselves.
  c. Configuring refsel per the datasheet is a *safeguard* against some undesirable outcome that could occur *if ADCnREFEN was also accidentally set* with an external reference enabled.
  d. Configuring refsel per the datasheet is a *safeguard* against some undesirable outcome that could occur *whether or not ADCnREFEN is set*, but the undesirable outcome impacts the current internal reference only.
  e. Configuring refsel per the datasheet is a safeguard against some undesirable outcome that would cause visible consequences for the running sketch, regardless of whether ADCnREFEN is set, and this has only gone unnoticed because nobody who didn't read that section of the datasheet either have not tried to use an external voltage reference, or if they have, when whatever consequences occurred, they kept that information to themselves.
  f. Everyone using external references has read that part of the datasheet, and thus nobody has encountered this condition. In this case, it could do anything from functioning normally to exploding, to raising the dead, and we wouldn't know.
  g. Nobody has tried using the external reference, period.

My guess (and it is ONLY A **GUESS**) is that b, c, or possibly d or a is the case, and that in the case of c and d, the undesired outcome takes the form of damage to the selected analog reference. Which will usually be 0.55V, since why would you change the internal reference if you had an external one connected unless you'd read that paragraph. The core starts with Vdd as the reference, but that option is located elsewhere (in the ADC itself we ask for Vdd as reference, but the VREF peripheral still thinks it's pointing at 0.55V). That reference voltage is rarely used, so even if it was easy to destroy that reference, this could plausibly have gone unnoticed by megaTinyCore users.

I think possibility g is unlikely - someone probably tried it at some point. And f is only a theoretical possibility. We all know that most people haven't made a study of the datasheet and some haven't even taken a cursory look, especially for peripherals that normally handled by Arduino API functions.

analogReference() as you know takes only a single argument, and the overhead of measuring an external reference voltage to determine what the internal reference should be set to in order to comply with that restriction would be prohibitive. If you are using the external reference on the 1-series parts, first call analogReference with a internal reference per their quoted instructions, then set it to the external reference, if you're concerned about that. I would love to know more about how important this step is. I might even attempt to see if I could demonstrate that issue. at the possible expense of an internal reference on a 1614.


## Internal Sources
In addition to reading from pins, you can read from a number of internal sources - this is done just like reading a pin, except the constant listed in the table below is used instead of the pin number:


| AVR DA           | AVR DB            | AVR DD            | AVR EA           |
|------------------|-------------------|-------------------|------------------|
| `ADC_TEMPERATURE`| `ADC_VDDDIV10`    | `ADC_VDDDIV10`    | `ADC_VDDDIV10`   |
| `ADC_GROUND` *   | `ADC_TEMPERATURE` | `ADC_TEMPERATURE` | `ADC_TEMPERATURE`|
| `ADC_DACREF0` *  | `ADC_GROUND` *    | `ADC_GROUND`      | `ADC_GROUND`     |
| `ADC_DACREF1` *  | `ADC_DACREF0` *   | `ADC_DACREF0`     | `ADC_DACREF0`    |
| `ADC_DACREF2` *  | `ADC_DACREF1` *   | `ADC_DAC0`        | `ADC_DACREF1`    |
| `ADC_DAC0`       | `ADC_DACREF2` *   | `ADC_VDDIO2DIV10` | `ADC_DAC0`       |
|                  | `ADC_DAC0` *      |                   |                  |
|                  | `ADC_VDDIO2DIV10` |                   |                  |

| tinyAVR 0/1-series                     | tinyAVR 2-series                    |
|----------------------------------------|-------------------------------------|
| `ADC_INTREF`                           | `ADC_VDDDIV10`                      |
| `ADC_TEMPERATURE`                      | `ADC_TEMPERATURE`                   |
| `ADC_DAC0` (1-series only)             | `ADC_GROUND` (offset calibration?)  |
| `ADC_GROUND` (offset calibration?)     | `ADC_DACREF0`                       |
| `ADC_DACREF0` (alias of ADC_DAC0)      | `ADC_DAC0` (alias of `ADC_DACREF0`) |
| `ADC_PTC` (not particularly useful)    | *see note*                          |
| `ADC1_DACREF1` (16/32k 1-series only)† |                                     |

The Ground internal sources are presumably meant to help correct for offset error. On Classic AVRs they made a point of talking about offset cal for differential channels. They are thus far silent on this, despite a great number of tech briefs related to the new ADC has been coming out, greatly improving the quality of the available information
DACREF0 is the the reference voltage for the analog comparator, AC0. On the 1-series, this is the same as the `DAC0` voltage (yup, the analog voltage is shared). Analog comparators AC1 and AC2 on 1-series parts with at least 16k of flash also have a DAC1 and DAC2 to generate a reference voltage for them (though it does not have an output buffer). On the 2-series, this is only used as the AC0 reference voltage; it cannot be output to a pin.

DACREF0-2 are the the reference voltages for the analog comparators. On the DA-series, there is no way to measure the supply voltage other than using DAC or DACREF source: you can neither directly measure a reference voltage like some parts, nor is there any way to get a fraction of the supply voltage like the DB and DD-series support.  Note also that on the DB series, you can't measure the outputs of the OPAMPs directly - you must output to the pin and measure that, however much the high-level descriptions sound like there is a way to route the opamp signals internally.

Unlike the AVR DA-series parts, which have neither `VDDDIV10` nor `INTREF` as an option - that meant if you wanted to know or the DB, DD, or EA-series parts. To what end are you measuring a voltage which you are in control of? To see how accurate the reference is?
At least there's a plausible use of the DAC input: recognizing that the voltage is off. On all parts there's the possibility of a load on it that's too big dragging it downwards. The DA and DB parts also have a lifetime drift issue (per errata) when the DAC is used without the output buffer.

`*` Note that the I/O headers omit several options listed in the datasheet; For Dx-series, DACREF options aren't listed in the I/O headers for MUXPOS or Ground, but we still provide the constants listed above, consistent with the datasheet.
`?` The DD-series lists a "BGTEMPSENSE" option in preliminary material, but it was removed by launch. It is unclear if this was ever implemented or if it exists in the silicon.

### Measuring VDD and Temperature
See [the example we give for very talkative implementation](../libraries/megaTinyCore/examples/ReadTempVcc/ReadTempVcc.ino) and the [megaTinyCore.h library](../libraries/megaTinycore/README.md) which provides ready-to-use function calls for these measurements.


#### Measuring VDD on 0/1-series
So the only way to measure Vdd as you can see from the tables above is to measure the internal reference, with the reference set to VDD.

But you probably want to use either the 1V1, 1V5, or 2V5 reference, not the default 0.55V one... But if you choose one of those, it switches the reference to internal! Don't worry - there's a very simple solution:
```c++
analogReference(INTERNAL2V5); // set reference to the desired voltage, and set that as the ADC reference.
analogReference(VDD); // Set the ADC reference to VDD. Voltage selected previously is still the selected, just not set as the ADC reference.
analogRead(ADC_INT_REF); // proceed to measure the analog reference.
```

On the 2-series, of course, there's no need for such silliness - you can just read ADC_VDDDIV10 with a convenient internal reference.


### Analog Resolution
The hardware supports increasing the resolution of analogRead() to the limit of the hardware's native resolution (10 or 12 bits); Additionally, we provide automatic oversampling and decimation up to the limit of what can be gathered using the accumulation feature allowing up to 13 bits of resolution (17 on 2-series); this is exposed through the `analogReadEnh()` function detailed below.

### A second ADC? *1-series only*
On some parts, yes! The "good" 1-series parts (1614, 1616, 1617, 3216, and 3217) have a second ADC, ADC1. On the 20 and 24-pin parts, these could be used to provide analogRead() on additional pins (it can also read from DAC2). Currently, there are no plans to implement this in the core due to the large number of currently available pins. Instead, it is recommended that users who wish to "take over" an ADC to use it's more advanced functionality choose ADC1 for this purpose. See [ADC free-run and more](ADCFreerunAndMore.md) for some examples of what one might do to take advantage of it (though it has not been updated for the 2-series yet. As of 2.0.5, megaTinyCore provides a function `init_ADC1()` which initializes ADC1 in the same way that ADC0 is (with correct prescaler for clock speed and VDD reference). On these parts, ADC1 can be used to read DAC1 (DACREF1) and DAC2 (DACREF2), these are channels `0x1B` and `0x1E` respectively. ~The core does not provide an equivalent to analogRead() for ADC1 at this time.~

#### Using ADC1 just like a second ADC0
This is a heinous waste of the second ADC. However it can be done - the following functions are are provided, which are identical to their normal counterparts, on devices with the second ADC:
* init_ADC1
* analogReference1
* analogClockSpeed1
* analogRead1
* analogSampleDuration1
* analogReadEnh1

If you are using this, you must call init_ADC1() before using any of them.

## ADC Sampling Speed
megaTinyCore takes advantage of the improvements in the ADC on the newer AVR parts to improve the speed of analogRead() by more than a factor of three over classic AVRs! The ADC clock which was - on the classic AVRs - constrained to the range 50-200kHz - can be cranked up as high as 1.5MHz at full resolution! On the 0/1-series, we use 1MHz at 16/8/4 MHz system clock, and 1.25 MHz at 20/10/5 MHz system clock. To compensate for the faster ADC clock, we set the sample duration to 14 to extend the sampling period from the default of 2 ADC clock cycles to 16 - providing approximately the same sampling period as on classic AVRs (which is probably a conservative decision). On the 2-series, the corresponding setting has only 0.5 or 1 ADC clock added to it, so we set it to 15 - however, it can be set all the way up to 255. The ADC clock is run at approx. 2.5 MHz on the 2-series parts.  If you are measuring a lower impedance signal and need even faster `analogRead()` performance - or if you are measuring a high-impedance signal and need a longer sampling time, you can adjust that setting with `analogSampleDuration()` detailed below.

## Analog Resolution
The hardware supports increasing the resolution of analogRead() to the limit of the hardware's native resolution (10 or 12 bits); Additionally, we provide automatic oversampling and decimation up to the limit of what can be gathered using the accumulation feature allowing up to 15 bits of resolution (17 on future Ex-series); this is exposed through the `analogReadEnh()` function detailed below.


## ADC Function Reference
This core includes the following ADC-related functions. Out of the box, analogRead() is intended to be directly compatible with the standard Arduino implementation. Additional functions are provided to use the advanced functionality of these parts and further tune the ADC to your application. Several functions are unique to the cores I maintain. The functions here not marked if they are simple implementations that duplicate the stock one. They are marked (semi-standard) if it is something that is standard on some arduino cores but not official AVR cores and/or which works differently here), or (one of more of mTC, DxC, and ATC - these are the three cores I maintain, megaTinyCore, DxCore and ATTinyCore).

### (mTC/DxC) getAnalogReference() and getDACReference()
These return the numbers listed in the reference table at the top as a `uint8_t` - you can for example test `if (getAnalogReference() == INTERNAL1V1)`

### int16_t analogRead(pin)
The standard analogRead(). Single-ended, and resolution set by analogReadResolution(), default 10 for compatibility. Negative return values indicate an error that we were not able to detect at compile time (as always, when we can detect a problem at compile time, we try to generate an error then. Return type is a 16-bit signed integer (`int` or `int16_t`).

### (semi-standard) bool analogReadResolution(resolution)
Sets resolution for the analogRead() function. Unlike stock version, this returns true/false. *If it returns false, the value passed was invalid, and resolution was set to the default, 10 bits*. Note that this can only happen when the value passed to it is determined at runtime - if you are passing a compile-time known constant which is invalid, we will issue a compile error. The only valid values are those that are supported natively by the hardware, plus 10 bit, even if not natively supported, for compatibility.
Hence, the only valid values are 10 and 12, plus 8 on the EA-series.

This is different from the Zero/Due/etc implementations, which allow you to pass anything from 1-32, and rightshift the minimum resolution or leftshift the maximum resolution as needed to get the requested range of values. Within the limited resources of an 8-bit AVR with this is a silly waste of resources, and padding a reading with zeros so it matches the format of a more precise measuring method is in questionable territory in terms of best practices. Note also that we offer oversampling & decimation with `analogReadEnh()` and `analogReadDiff()`, which can extend the resolution while keep the extra bits meaningful at the cost of slowing down the reading. `analogReadResolution()` only controls the resolution of analogRead() itself. The other functions take desired resolution as an argument, and restore the previous setting.

### (mTC/DxC) bool analogSampleDuration(duration)
Sets sampling duration (SAMPLEN), the sampling time will be 2 + SAMPLEN ADC clock cycles. Sampling time is the time the sample and hold capacitor is connected to the source before beginning conversion. For high impedance voltage sources, use a longer sample length. For faster reads, use a shorter sample length.
speed. This returns a `bool` - it is `true` if value is valid.

This value is used for all analog measurement functions.

| Part Series | Sample time<br/>(default)  | Conversion time | Total analogRead() time | Default SAMPLEN* | ADC clock sample time |
|-------------|--------------|-----------------|-------------------------|-----------------|-----------------------|
| Classic AVR |        12 us |           92 us |                  104 us | No such feature | 1.5                   |
| 0/1-series  |        12 us |       8.8-10 us |              20.8-22 us |     7, 9, or 13 | 2 + SAMPLEN           |
| Dx (10-bit) |   apx. 12 us |  approx.  10 us | Approximately  21-22 us |              14 | 2 + SAMPLEN           |
| Dx (12-bit) |   apx. 12 us |  approx.  12 us | Approximately  23-24 us |              14 | 2 + SAMPLEN           |
| 2-series    |  apx. 6.4 us |  approx. 5.6 us | Approximately     12 us |              15 | 1 + SAMPDUR           |
| EA-series   |  apx. 6.4 us |  approx. 5.6 us | Approximately     12 us |              15 | 1 + SAMPDUR           |

`*` The default samplen given above is the default that the core sets it to when the ADC is initialized. The hardware defaults to 0, giving a much shorter sampling time. Also, as shown above while it's called SAMPLEN on parts with that don't have the PGA, with the new ADC and accompanying register name reshuffle, that became SAMPDUR. SAMPLEN was the sole bitfienld in the SAMPCTRL on Dx, while SAMPDUR is the sole bitfield on the clearly named CTRLE register. I'm not sure how either of those differences made it through. If I were in the room at the time, I'd have let 'em have it, because you should never rename a register without good reason, and the renamed register should never be objectively worse than the previous one. In this case, there is no conceivable good reason, and the new register name (CTRLE I mean) is obviously worse, since it gives no indication of it's function, nor is it even located immediately after CTRLD.

On the 2-series, we are at least given some numbers: 8pF for the sample and hold cap, and 10k input resistance so a 10k source impedance would give 0.16us time constant, implying that even a 4 ADC clock sampling time is excessive, but at such clock speeds, impedance much above that would need a longer sampling period.

### (mTC/DxC) analogReadEnh(pin, res=ADC_NATIVE_RESOLUTION, gain=0)
Enhanced `analogRead()` - Perform a single-ended read on the specified pin. `res` is resolution in bits, which may range from 8 to `ADC_MAX_OVERSAMPLED_RESOLUTION`. This maximum is 13 bits for 0/1-series parts, and 17 for 2-series. If this is less than the native ADC resolution, that resolution is used, and then it is right-shifted 1, 2, or 3 times; if it is more than the native resolution, the accumulation option which will take 4<sup>n</sup> samples (where `n` is `res` native resolution) is selected. Note that maximum sample burst reads are not instantaneous, and in the most extreme cases can take milliseconds. Depending on the nature of the signal - or the realtime demands of your application - the time required for all those samples may limit the resolution that is acceptable. The accumulated result is then decimated (rightshifted n places) to yield a result with the requested resolution, which is returned. See [Atmel app note AVR121](https://ww1.microchip.com/downloads/en/appnotes/doc8003.pdf) - the specific case of the new ADC on the Ex and tinyAVR 2-series is discussed in the newer DS40002200 from Microchip, but it is a rather vapid document). Alternately, to get the raw accumulated ADC readings, pass one of the `ADC_ACC_n` constants for the second argument where `n` is a power of 2 up to 64 (0/1-series), or up to 1024 (2-series). Be aware that the lowest bits of a raw accumulated reading should not be trusted.; they're noise, not data (which is why the decimation step is needed, and why 4x the samples are required for every extra bit of resolution instead of 2x). On 2-series parts *the PGA can be used for single ended measurements*. Valid options for gain on the 2-series are 0 (PGA disabled, default), 1 (unity gain - may be appropriate under some circumstances, though I don't know what those are), or powers of 2 up to 16 (for 2x to 16x gain). On AVR Dx and tinyAVR 0/1-series parts, the gain argument should be omitted or 0; these do not have a PGA.

| Part Series | Max Acc.     | result trunc?   | Maximum acc+decimate | Max PGA gain  |
|-------------|--------------|-----------------|----------------------|---------------|
| 0/1-series  |   64 samples |  N/A            |             13 bits  | N/A - no PGA  |
| Dx-series   |  128 samples |  16 bits max    |             15 bits  | N/A - no PGA  |
| 2-series    | 1024 samples |  No             |             17 bits  | 0/1/2/4/8/16x |
| EA-series   | 1024 samples |  No             |             17 bits  | 0/1/2/4/8/16x |

**WARNING** trying to take 17-bit readings on a 2-series/EA-series will take a long time for each sample. If the sample is changing, the results will be garbage.
**WARNING** Extreme attention is required to make the least significant bits meaningful, especially when oversampling and decimating to 17 bits. This means very careful hardware design. Otherwise, you will be very accurately measuring the noise and error sources. I am praying that the EA-series will have AVDD on a separate power domain and invite us to filter it further to improve analog stability. In any event, quality external ADC reference is recommended when high accuracy is required.

```c++
  int32_t adc_reading = analogReadEnh(PIN_PD2, 13); //be sure to choose a pin with ADC support. The Dx-series and tinyAVR have ADC on very different sets of pins.
  // Measure voltage on PD2, to 13 bits of resolution.
  // AVR Dx, Ex, and tinyAVR 2-series this will be sampled 4 times using the accumulation function, and then rightshifted once
  // tinyAVR 0/1-series, this will be sampled 64 times, as we need 3 more bits, hence we need to take 2^(3*2) = 64 samples then rightshift them 3 times.,
  int32_t adc_reading2 = analogReadEnh(PIN_PD2, ADC_ACC128);
  // Take 128 samples and accumulate them. This value, k, is 19 bits wide; on the Dx-series parts, this is truncated to 16 bits - the hardware does not expose the three LSBs.
  int32_t adc_reading3 = analogReadEnh(PIN_PD2, ADC_CHOP(14));
  // oversa,[;e amd decimate to try for 14-bits of accuracy use sign chopping to redue offset error (Ex-only)
  int32_t adc_reading3 = analogReadEnh(PIN_PD2, ADC_ACC64S);
  // Similar to the raw accumulation above, this does the same thing, only with sign chopping turned on (Ex-only)
```

Negative values from ADC_ENH always indicate a runtime error; these values are easily recognized, as they are huge negative numbers

### (mTC/DxC) int32_t analogReadDiff(positive, negative, res=ADC_NATIVE_RESOLUTION, gain=0)
Differential `analogRead()` - returns a `long` (`int32_t`), not an `int` (`int16_t`). Performs a differential read using the specified pins as the positive and negative inputs. Any analog input pin can be used for the positive side, but only pins on PORTD/PORTE (AVR Dx-series), or the constants `ADC_GROUND` or `ADC_DAC0` can be used as the negative input. Information on available negative pins for the Ex-series is not yet available, but is expected to be a subset of available analog pins. The result returned is the voltage on the positive side, minus the voltage on the negative side, measured against the selected analog reference. The `res` parameter works the same way as for `analogReadEnh()`, as does the `gain` function. Gain becomes FAR more useful here than in single-ended mode as you can now take a very small difference and "magnify" it to make it easier to measure. Be careful when measuring very small values here, this is a "real" ADC not an "ideal" one, so there is a non-zero error, and through oversampling and/or gain, you can magnify that such that it looks like a signal.

On the Dx-series, the measured voltages must be less than VRef; this makes differential ADC measurements on these parts significantly less powerful than the Ex-series, where the input can extend as much as 100mV beyond the supply rails, permitting use to measure current through a high-side sense resistor against the 1.024 V reference.

```c++
  int32_t diff_first_second = analogReadDiff(PIN_PA1, PIN_PA2, 12, 0);
  // Simple differential read to with PA1 as positive and PA2 as negative. Presumable these are close in voltage.
  // (I used two pots and manually adjusted them to be very close; also could have done like 10k-100ohm-10k resistor network)
```

The 32-bit value returned should be between -65536 and 65535 at the extremes with the maximum 17-bit accumulation option, or, 32-times that if using raw accumulated values (-2.1 million to 2.1 million, approximately).

This, again, is easy to distinguish from an error code, as the error codes are close to -2.1 **b**illion.

### (mTC/DxC) analogClockSpeed(int16_t frequency = 0, uint8_t options = 0)
The accepted options for frequency are -1 (reset ADC clock to core default, 1-1.35 MHz), 0 (make no changes - just report current frequency) or a frequency, in kHz, to set the ADC clock to.

| Part series | F_ADCmin | F_ADCmax | Reference | Prescalers                  |
|-------------|----------|----------|-----------|-----------------------------|
| tinyAVR 0/1 | 100      | 250      | 0.55V     | 8 from 2 to 256             |
| tinyAVR 0/1 | 200      | 1500     | All other | 8 from 2 to 256             |
| tinyAVR 0/1 | 200      | 2000     | Not 0.55V | Must use 8-bit resolution   |
| tinyAVR 2   | 300      | 6000     | Internal  | 16 from 2 to 64             |
| tinyAVR 2   | 300      | 6000     | External  | 16 from 2 to 64             |
| AVR Dx      | 125      | 2000     | All       | 14 from 2 to 256            |
| AVR EA      | 300      | 3000     | Internal  | 16 from 2 to 64             |
| AVR EA      | 300      | 6000     | External  | 16 from 2 to 64             |

Values between 125 and 2000 are considered valid for Dx-series parts and Ex-series parts 300-3000 with internal reference, and 300-6000 with Vdd or external reference. The prescaler options are discrete, not continuous, so there are a limited number of possible settings (the fastest and slowest of which are often outside the rated operating range). The core will choose the highest frequency which is within spec, and which does not exceed the value you requested.

On any non-DX parts, you must call analogClockSpeed AFTER analogReference, since that changes the limits. On tiny 0/1, you should also be sure that if using in 8-bit mode you call analogReadResolution first, as the new prescaller is calculated when the function is called, and not at any other time. The 2-series and EA only care whether the reference is internal or Vdd/external, so you con't need to re-call this when changing between internal references. Note that changing internal references does not recalculate the speed and make sure it's legal (it has no way to report that to you if it is to adhere to the API), and the same goes for analogReadResolution.

If a 1 is passed as the second argument, the validity check will be bypassed; this allows you to operate the ADC out of spec if you really want to, which may have unpredictable results. Microchip documentation has provided little in the way of guidance on selecting this (or other ADC parameters) other than giving us the upper and lower bounds. On tinyAVR 0.1-series, the dutycycle will be dutifuly set if it's above 1500.

**Regardless of what you pass it, it will return the frequency in kHz** as a `uint16_t`.

The 0/1-series has prescalers in every power of two from 2 to 256, and at the extreme ends, typical operating frequencies will result in an ADC clock that is not in spec. Hence the second argument, Currently there is only one option: If a 1 or anything with the 1's bit set is passed, that tells the core to disregard the spec'ed minimum and maximum, and get as close to the value you asked for as possible, however accuracy may suffer and/or functionality may be impaired

In either case you don't need to know the range of prescalers - just ask for the desired frequency. We'll get as close as possible to that number without exceeding it or going outside the chip specifications, and tell you what that number we picked was. If options & 0x01 is non-zero, we will skip checking whether the frequency in question is within spec, just in case you for some reason want inaccurate results from the ADC, or the chip's clock speed has been changed (which of course breaks timekeeping too - but you may not care, or may also be compensating for that.

```c++
// ******* for both *******
Serial.println(analogClockSpeed());     // prints something between 1000 and 1500 depending on F_CPU
analogClockSpeed(300);                  // set to 300 kHz
Serial.println(analogClockSpeed());     // prints a number near 300 - the closest to 300 that was possible without going over.

// ****** For 0/1-series *******
Serial.println(analogClockSpeed(2000)); // sets prescaler such that frequency of ADC clock is as
// close to but not more than 2000 (kHz) which is the maximum supported according to the datasheet.
// Any number of 2000 or higher will get same results.
Serial.println(analogClockSpeed(20));   // A ridiculously slow ADC clock request.
// Datasheet says minimum is 125, maximum prescale(256) would give 93.75 kHz - but that's not
// within spec, and no second argument was passed; we will instead set to 128 prescaler
// for clock speed of 187.5 kHz and will return 187,
Serial.println(analogClockSpeed(20, 1)); // As above, but with the override of spec-check
// enabled, so it will set it as close to 20 as it can (93.75) and return 93.

// ****** For 2-series *******
Serial.println(analogClockSpeed(20000));   // Above manufacturer max spec, so seeks out a value that
// is no larger than that spec; 3000 if internal reference selected or 6000 otherwise.
Serial.println(analogClockSpeed(20000,1)); // Above manufacturer's spec. but we instruct
// it to ignore the spec and live dangerously. The requested speed, far in excess of the
// maximum obtainable (which is several times the maximum supported) speed, with limits bypassed,
// will lead it to choose the fastest possible ADC clock, which is only prescaled by a factor of 2,
// and return that value, likely 8000 or 10000 unless for 16 or 20 MHz. (higher if you're overclocking
// the chip in general too). One version that was released shortly after the 2-series contained a bug that would always set the
// ADC prescaler to the minimum. And I'd written about how disappointed I was in the new ADC. After fixing the bug, things improved significantly.
// I am favorably impressed by how well the poor overclocked ADC performed with an internal reference and 10 MHz ADC clock - Yes it was unacceptably poor
// but it was close enough to okay that I released it without realizing it was my bad.

// ******* For both *******
int returned_default = analogClockSpeed(-1); // reset to default value, around 1000 - 1500 kHz for Dx or 0/1-series tiny, and around 2500 for Ex or 2-series tiny.

Serial.println(returned_default);  // will print the same as the first line, assuming you hadn't changed it prior to these lines.
```
If anyone undertakes a study to determine the impact of different ADC clock frequency on accuracy, take care to adjust the sampling time to hold that constant. I would love to hear of any results; I imagine that lower clock speeds should be more accurate, but within the supported frequency range, I don't know whether these differences are worth caring about.
I've been told that application notes with some guidance on how to best configure the ADC for different jobs is coming - though it seems like what they've actually done is to just remove some of the knobs from the EA-series. In any event, we're down to one mysterious switch (marked "Sign Chopping" - and I think I've worked out what it does, and the two least mysterious of the knobs on the EA (clock speed and sampling time).

### uint8_t getAnalogReadResolution()
Returns either 8, 10 or 12, the current resolution set for analogRead. no part has more than 2 options here - between the Dx (10, 12), Ex (unknown, likely 8 and 12), and tinyAVR parts (8 and 10 or 8 and 12), all of the combinations are possible.

### uint8_t getAnalogSampleDuration()
Returns the number of ADC clocks by which the minimum sample length has been extended.

### ADC Runtime errors
When taking an analog reading, you may receive a value near -2.1 billion, or a negative value on a part without a differential ADC - these are runtime error codes.
The busy and disabled errors are the only ones that we never know at compile time. I don't think analogClockSpeed can generate runtime errors - it should always do it's best to meet your request, and if it can't, return the closest it could find.
Note that the numeric values, though not the names, of some of these were changed to make the error checking more efficient. As long as you used the named constants like you're supposed to you'll be fine. The values returned by checkAnalogError will not change in future releases, we make not guarantees about the values of the error constants themselves, though no changes are expected.

| Error name                     |     Value   | analogCheckError val | Notes
|--------------------------------|-------------|----------------------|----------------------------------------------
|ADC_ERROR_BAD_PIN_OR_CHANNEL    |      -32001 |                   -1 | The specified pin or ADC channel does not exist or does not support analog reads.
|ADC_ERROR_BUSY                  |      -32002 |                   -2 | The ADC is busy with another conversion.
|ADC_ERROR_DISABLED              |      -32007 |                   -7 | The ADC is disabled at this time. Did you disable it before going to sleep and not re-enable it?
|ADC_ENH_ERROR_BAD_PIN_OR_CHANNEL| -2100000001 |                   -1 | The specified pin or ADC channel does not exist or does not support analog reads.
|ADC_ENH_ERROR_BUSY              | -2100000002 |                   -2 | The ADC is busy with another conversion.
|ADC_ENH_ERROR_RES_TOO_LOW       | -2100000003 |                   -3 | Minimum ADC resolution is 8 bits. If you really want less, you can always rightshift it.
|ADC_ENH_ERROR_RES_TOO_HIGH      | -2100000004 |                   -4 | Maximum resolution using automatic oversampling and decimation is less than the requested resolution.
|ADC_DIFF_ERROR_BAD_NEG_PIN      | -2100000005 |                   -5 | analogReadDiff() was called with a negative input that is not valid.
|ADC_ENH_ERROR_DISABLED          | -2100000007 |                   -7 | The ADC is currently disabled. You must enable it to take measurements. Did you disable it before going to sleep and not re-enable it?
|ADC_IMPOSSIBLE_VALUE            |         N/A |                 -127 | 16-bit value > 4095, or 32-bit value that's not an error code and is outside the range of -2,097,152-4,194,303 (raw 1024-sample accumulation range.
|Potentially valid reading       |see previous |                    0 | If there is some combinations of settings that could get this value without an error condition it returns 0.

The impossible values are checked for without testing all of the bytes for greater efficiency. If you see that result one of two things was the case: the value you passed in wasn't from analog read or had been cast to a different type before you passed it, or it or was corrupted somehow (writing off end of adjacent array in memory? Overclocking too hard such that the chip was doing math wrong?).

### (mTC/DxC - New!) ADCPowerOptions(options) *2-series only prior to 2.5.12*
*For compatibility, a much more limited version is provided for 0/1-series. See below*
The PGA requires power when turned on. It is enabled by any call to `analogReadEnh()` or `analogReadDiff()` that specifies valid gain > 0; if it is not already on, this will slow down the reading. By default we turn it off afterwards. There is also a "low latency" mode that, when enabled, keeps the ADC reference and related hardware running to prevent the delay (on order of tens of microseconds) before the next analog reading is taken. We use that by default, but it can be turned off with this function.
Generate the argument for this by using one of the following constants, or bitwise-or'ing together a low latency option and a PGA option. If only one option is supplied, the other configuration will not be changed. Note that due to current errata, you **must** have LOW_LAT enabled
* `LOW_LAT_OFF`     Turn off low latency mode. *2-series only*
* `LOW_LAT_ON`      Turn on low latency mode. *2-series only*
* `PGA_OFF_ONCE`    Turn off the PGA now. Don't change settings; if not set to turn off automatically, that doesn't change. *2-series only*
* `PGA_KEEP_ON`     Enable PGA. Disable the automatic shutoff of the PGA. *2-series only*
* `PGA_AUTO_OFF`    Disable PGA now, and in future turn if off after use. *2-series only*
* `ADC_ENABLE`      Enable the ADC if it is currently disabled.     *new 2.5.12*
* `ADC_DISABLE`     Disable the ADC to save power in sleep modes.   *new 2.5.12*
* `ADC_STANDBY_ON`  Turn on ADC run standby mode                    *new 2.5.12*
* `ADC_STANDBY_OFF` Turn off ADC run standby mode                   *new 2.5.12*

Example:
```c++
ADCPowerOptions(LOW_LAT_ON  | PGA_KEEP_ON );            //  low latency on. Turn the PGA on, and do not automatically shut it off. Maximum power consumption, minimum ADC delays.
ADCPowerOptions(LOW_LAT_OFF | PGA_AUTO_OFF);            //  low latency off. Turn off the PGA and enable automatic shut off. Minimum power consumption, maximum ADC delays. **ERRATA WARNING** turning off LOWLAT can cause problems on 2=series parts! See the errata for the specific part you are using.)
ADCPowerOptions(ADC_DISABLE);                           //  turn off the ADC.
ADCPowerOptions(ADC_ENABLE);                            //  Turn the ADC back on. If LOWLAT mode was on, when you turned off the ADC it will still be on,. Same with the other options.
```

As of 2.5.12 we will always disable and re-enable the ADC if touching LOWLAT, in the hopes that this will work around the lowlat errata consistently.
**it is still recommended to call ADCPowerOptions(), if needed, before any other ADC-related functions** unless you fully understand the errata and the ramifications of your actions.
**On most 2-series parts LOWLAT mode is REQUIRED in order to use the PGA to measure anything except the DAC or temperature sensor if using VDD as a reference! See the errata; this is the only erratum with teeth that got through their internal testing. They're getting better, when not using an internal reference, or measuring the DACREF!, and disabling the ADC is REQUIRED for acceptable sleep power consumption.**

Lowlat mode is enabled by default for this reason, as well as to generally improve performance. Disabling the ADC will end the power consumption associated with it.

On 0/1-series parts, this function supports functions that are far more limited, since there are few power options.
Only the following are supported
* `ADC_ENABLE`      Enable the ADC if it is currently disabled.     *new 2.5.12*
* `ADC_DISABLE`     Disable the ADC to save power in sleep modes.   *new 2.5.12*
* `ADC_STANDBY_ON`  Turn on ADC run standby mode                    *new 2.5.12*
* `ADC_STANDBY_OFF` Turn off ADC run standby mode                   *new 2.5.12*

In all cases, if no command to turn on or off an option is passed the current setting will remain unchanged.

### Functions in megaTinyCore.h
These functions are located in megaTinyCore.h - they do not require tight core integration to work,.
#### printADCRuntimeError(uint32_t error, &UartClass DebugSerial)
Pass one of the above runtime errors and the name of a serial port to get a human-readable error message. This is wasteful of space, do don't include it in your code unless you need to. Also, *you must not cast result to a different type before calling this. The logic is determined by whether it is passed a signed 32-bit value or a signed 16-bit one*
```c++
int32_t adc_reading=AnalogReadDiff(PIN_PA1, PIN_PA2);
if (analogCheckError) { // an error occurred! Print a human readable value
  printADCRuntimeError(adc_reading, Serial);
}
```
#### analogCheckError(value)
Pass either the int16_t from analogRead or the int32_t from analogReadEnh to this to check if it's a valid value. If this returns a non-zero value, that means that you got an error, and should be printing debugging information, not trying to make use of it.
*you must not cast result to a different type before calling this - the logic is determined by whether it is passed a signed 32-bit value (from analogReadEnh or analogReadDiff) or a signed 16-bit one (from analogRead())*
```c++
// Right:
int32_t adcreading=analogReadEnh(PIN_PA1,12);
if (analogCheckError(adcreading)) {
  Serial.print("Analog value returned was an error: ");
  Serial.println(adcreading);
}
// Right:
int16_t adcreading2=analogRead(PIN_PA1);
if (analogCheckError(adcreading2)) {
  Serial.print("Analog value returned was an error: ");
  Serial.println(adcreading2);
}

// Wrong:
int32_t adcreading=analogRead(PIN_PA1); // analogRead() returned an int16_t, but if we assign it to a 32-bit variable it's going to think it came from analogReadEnh and will not correctly identify errors.
if (analogCheckError(adcreading)) {
  Serial.print("Analog value returned was an error: ");
  Serial.println(adcreading);
}
```

#### int16_t millivoltsToADCVals(int16_t mv, const int16_t ref, const uint8_t bits) *Not yet implemented*
Converts a voltage in millivolts into the approximate expected ADC counts (no accounting for error is made). See the [documentation for the DxCore Library](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/libraries/DxCore)
### ADCPowerOptions(options)
The PGA requires power when turned on. It is enabled by any call to `analogReadEnh()` or `analogReadDiff()` that specifies valid gain > 0; if it is not already on, this will slow down the reading. By default we turn it off afterwards. There is also a "low latency" mode that, when enabled, keeps the ADC reference and related hardware running to prevent the delay (on order of tens of microseconds) before the next analog reading is taken. We use that by default, but it can be turned off with this function.
Generate the argument for this by using one of the following constants, or bitwise-or'ing together a low latency option and a PGA option. If only one option is supplied, the other configuration will not be changed. Note that due to current errata, you **must** have LOW_LAT enabled
* `LOW_LAT_OFF`     Turn off low latency mode. *2-series only*
* `LOW_LAT_ON`      Turn on low latency mode. *2-series only*
* `PGA_OFF_ONCE`    Turn off the PGA now. Don't change settings; if not set to turn off automatically, that doesn't change. *2-series only*
* `PGA_KEEP_ON`     Enable PGA. Disable the automatic shutoff of the PGA. *2-series only*
* `PGA_AUTO_OFF`    Disable PGA now, and in future turn if off after use. *2-series only*
* `ADC_ENABLE`      Enable the ADC if it is currently disabled.     *new 2.5.12*
* `ADC_DISABLE`     Disable the ADC to save power in sleep modes.   *new 2.5.12*
* `ADC_STANDBY_ON`  Turn on ADC run standby mode                    *new 2.5.12*
* `ADC_STANDBY_OFF` Turn off ADC run standby mode                   *new 2.5.12*

Example:
```c++
ADCPowerOptions(LOW_LAT_ON  | PGA_KEEP_ON );            //  low latency on. Turn the PGA on, and do not automatically shut it off. Maximum power consumption, minimum ADC delays.
ADCPowerOptions(LOW_LAT_OFF | PGA_AUTO_OFF);            //  low latency off. Turn off the PGA and enable automatic shut off. Minimum power consumption, maximum ADC delays. **ERRATA WARNING** turning off LOWLAT can cause problems on 2=series parts! See the errata for the specific part you are using.)
ADCPowerOptions(ADC_DISABLE);                           //  turn off the ADC.
ADCPowerOptions(ADC_ENABLE);                            //  Turn the ADC back on. If LOWLAT mode was on, when you turned off the ADC it will still be on,. Same with the other options.
```

As of 2.5.12 (1.6.8) we will always disable and re-enable the ADC if touching LOWLAT, in the hopes that this will work around the lowlat errata consistently.

**it is still recommended to call ADCPowerOptions(), if needed, before any other ADC-related functions** unless you fully understand the errata and the ramifications of your actions.

**On most 2-series parts LOW_LAT mode is REQUIRED in order to use the PGA when not using an internal reference or measuring the DACREF!**

**Disabling the ADC is REQUIRED for acceptable sleep power consumption on the 2-series!**

**No word either way on the EA-series!**

Lowlat mode is enabled by default for this reason, as well as to generally improve performance. Disabling the ADC will end the power consumption associated with it.

On 0/1-series parts, this function supports functions that are far more limited, since there are few power options.
Only the following are supported
* `ADC_ENABLE`      Enable the ADC if it is currently disabled.     *new 2.5.12*
* `ADC_DISABLE`     Disable the ADC to save power in sleep modes.   *new 2.5.12*
* `ADC_STANDBY_ON`  Turn on ADC run standby mode                    *new 2.5.12*
* `ADC_STANDBY_OFF` Turn off ADC run standby mode                   *new 2.5.12*

In all cases, if no command to turn on or off an option is passed the current setting will remain unchanged

### DAC Support
The 1-series parts have an 8-bit DAC which can generate a real analog voltage. This generates voltages between 0 and the selected VREF (which cannot be VDD, unfortunately). Set the DAC reference voltage via the `DACReference()` function - pass it one of the `INTERNAL` reference options listed under the ADC section above. This voltage must be half a volt lower than Vcc for the voltage reference to be accurate. The DAC is exposed via the analogWrite() function: Call `analogWrite(PIN_PA6,value)` to set the voltage to be output by the DAC. To turn off the DAC output, call digitalWrite() on that pin; note that unlike most PWM pins served by TCA0, `analogWrite(PIN_PA6,0)` and `analogWrite(PIN_PA6,255)` do not act as if you called digitalWrite() on the pin; 0 or 255 written to the `DAC0.DATA` register. Thus when using a lower reference voltage, you do not have to worry about it applying the full supply voltage to the pin if you pass it 255. (which you may have connected to sensitive devices that would be harmed by such a voltage) if let the calculation return 255; that will just output 255/256ths of the reference voltage.

The 2-series and 0-series don't have DACs, though the 2-series analog comparator has a "DACREF" an 8-bit reference DAC that can only be used as one side of the AC. This should be used through the Comparator library.

## Analog *channel* identifiers
The ADC is configured in terms of channel numbers, not pin numbers. analogRead() hence converts the number of a pin with an analog channel associated with it to the number of that analog channel, so there is no need to deal with the analog channel numbers. The one exception to that is in the case of the non-pin inputs, the constants like ADC_DAC and ADC_VDDDIV10. I have a simple system to internally signal when a number isn';'t an digital pin number, but is instead an analog channel number: Simply set the high bit. I refer to these as analog channel identifiers. When the high bit is masked off, these are the value that you must set the MUX to in order to use this input source. No AVR has ever had more than 127 pins, much less that many analog channels, so this shouldn't be an issue. With 254 valid values, the current design provides room for 127 digital pins and 127 analog inputs, where the largest modern AVRs have only 56 I/O pins (it will be a technical challenge to surpass that, because they don't have any more registers for the VPORTs, and analog multiplexers that only go up to 73 (they use the second highest bit to denote non-pin inputs. )

`ADC_CH()` will take an analog channel number (ex, "0") and turn it into the analog channel identifier. But you never need to do that unless you're getting very deep into a custom ADC library) The most common example when channels are used is when reading from things that are not pins - like the internal tap on the DAC output, or the VDDDIV10 value to find the supply voltage. These may overlap with pin numbers. Also internally, channel numbers are sometimes passed between functions. They are defined for pins that exist as analog channels, with names of the form `AINn` but **you should never use the AIN values** except in truly extraordinary conditions, and even then it's probably inappropriate. However I felt like mention of them here was needed. These macros were added at different times, and that can be seen from the inconsistent naming. This is unfortunate, but it  Some macros and helper functions involved are:

```text
digitalPinToAnalogInput(pin)    - converts an digital pin to an analog channel *without* the bit that says it's a channel (designed for the very last step of analogRead preparation where we turn the pin number into the channel to set the MUX)
digitalPinToAnalogInput1(pin)   - As above.... except for ADC1
analogChannelToDigitalPin(p)    - converts an analog channel *number* to a digital pin.
analogInputToDigitalPin(p)      - converts an analog channel identifier to a digital pin number.
digitalOrAnalogPinToDigital(p)  - converts an analog channel identifier or digital pin to a digital pin number
ADC_CH(channel number)          - converts channel numbers to analog channel identifier
```

Try not to use these unless you're getting really deep into library development and direct interaction with the ADC; we provide all the constants you will need. listed above.

## A word of warning to capacitive touch sensing applications
Libraries exist that use trickery and the ADC to measure capacitance, hence detect touch/proximity. Most of these libraries (since Atmel always locked up QTouch) relied on the leftover charge in the ADC S&H cap. The magnitude of this source of error is much larger on classic AVRs. It is considered undesirable (this is why when changing ADC channels in the past, you were advised to throw out the first couple of readings!) and with each generation, this has been reduced. There are still ways to do this effectively on the 2series, but require very different approaches. Thankfully for parts that do have a PTC, great strides are being made. the tiny1-series has an active developer working on it, while Microchip itself will release a library based on qtouch for the DA.

## Microchip Documentation
Has been much expanded and I'm very happy to see this sort of document produced before the EA release. Wondering about the numbering? Well, TB stands for Technical Brief, which is what Microchip refers to the sort of documents everyone else calls an Application Note. I believe the AN lettering denotes pre-revolutionary literature (ie, Atmel naming) - but it means the same thing as a technical brief, and covers the same spread of quality. Some are like a brilliant lighthouse, illuminating the way for miles around. Others could have been written by a highschool kid paraphrasing the datasheet. is is somewhat different from a whitepaper, which is meant to be a bit more formal. Depending on
* [DS00003197A - Differential and Single-Ended ADC](https://ww1.microchip.com/downloads/en/DeviceDoc/Differential-and-Single-Ended-ADC-WhitePaper-DS00003197A.pdf)
* [TB3256 - How to Use the 12-Bit Differential ADC with PGA in Single Mode](https://www.microchip.com/content/dam/mchp/documents/MCU08/ApplicationNotes/ApplicationNotes/TB3256-How-to-Use-the-12-Bit-Differential-ADC-with-PGA-in-Single-Mode-DS90003256A.pdf)
* [AN4225 - Maximizing the Signal: Tips and Tricks to Properly Acquiring Analog Signals](https://ww1.microchip.com/downloads/aemDocuments/documents/MCU08/ApplicationNotes/ApplicationNotes/Max-Signal-Properly-TipsTrick-Acq-Analog-Signals-DS00004225.pdf)
* [TB3254 - How to Use the 12-Bit Differential ADC with PGA in Burst Accumulation Mode
](https://ww1.microchip.com/downloads/aemDocuments/documents/MCU08/ApplicationNotes/ApplicationNotes/TB3254-How-to-Use-the-12-Bit-Differential-ADC-Burst-Accumulation-Mode-DS90003254A.pdf)
* [TB3256 - How to Use the 12-Bit Differential ADC with PGA in Single Mode](https://ww1.microchip.com/downloads/en/Appnotes/TB3256-How-to-Use-the-12-Bit-Differential-ADC-with-PGA-in-Single-Mode-DS90003256A.pdf)
* [TB3257 - How to Use the 12-Bit Differential ADC in Series Accumulation Mode](https://ww1.microchip.com/downloads/en/Appnotes/TB3257-How-to-Use-the-12-Bit-Differential-ADC-in-Series-Accumulation-Mode-DS90003257A.pdf)
