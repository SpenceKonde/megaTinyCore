# ADC/DAC Reference for megaTinyCore
These parts all have a large number of analog inputs (11 pins on 20 and 24-pin 0/1-series, 15 on 2-series, 9 on 14-pin parts and 5 on 8-pin parts.) - plus the one on the UPDI pin which is not totally usable because the UPDI functionality). They can be read with analogRead() like on a normal AVR. While the `An` constants (ex, `A3`) are supported, and will read from the ADC channel of that number, they are in fact defined as then digital pin shared with that channel. Using the `An` constants is deprecated - the recommended practice is to just use the digital pin number, or better yet, use `PIN_Pxn` notation when calling analogRead(). Particularly with the release of 2.3.0 and tinyAVR 2-series support, a number of enhanced ADC features have been added to expose more of the power of the sophisticated ADC in these parts.

**Be sure to disable to the ADC prior to putting the part to sleep if low power consumption is required!**

## Reference Voltages
Analog reference voltage can be selected as usual using analogReference(). Supported reference voltages are listed below:

| tinyAVR 0/1-series                     | tinyAVR 2-series                        |
|----------------------------------------|-----------------------------------------|
| `VDD` (Vcc/supply voltage - default)   | `VDD` (Vcc/supply voltage - default)    |
| `INTERNAL0V55`                         | `INTERNAL1V024`                         |
| `INTERNAL1V1`                          | `INTERNAL2V048`                         |
| `INTERNAL1V5`                          | `INTERNAL4V096`                         |
| `INTERNAL2V5`                          | `INTERNAL2V500`                         |
| `INTERNAL4V3` (alias of INTERNAL4V34)  | `INTERNAL4V1` (alias of INTERNAL4V096)  |
| `INTERNAL4V34`                         | `EXTERNAL`                              |
| `EXTERNAL` (16k and 32k 1-series only) |                                         |

Note: We do not provide a reference named "INTERNAL" like some classic AVR cores do; because the available voltages vary, this would be a detriment to cross-compatibility - by generating code that would compile, but behave differently, that would introduce the potential for new bugs that would be difficult to debug. Especially since the internal reference voltage isn't the same one that classic AVRs where that usage was commonplace is.... and so these would behave wrongly no matter what was done; minor modifications to sketches are required wheever the internal references are used when porting from classic to modern AVRs.


## Internal Sources
In addition to reading from pins, you can read from a number of internal sources - this is done just like reading a pin, except the constant listed in the table below is used instead of the pin name or pin number:
| tinyAVR 0/1-series                     | tinyAVR 2-series                    |
|----------------------------------------|-------------------------------------|
| `ADC_INTREF`                           | `ADC_VDDDIV10`                      |
| `ADC_TEMPERATURE`                      | `ADC_TEMPERATURE`                   |
| `ADC_DAC0` (1-series only)             | `ADC_GROUND` (offset calibration?)  |
| `ADC_GROUND` (offset calibration?)     | `ADC_DACREF0*`                      |
| `ADC_DACREF0` (alias of ADC_DAC0)      | `ADC_DAC0` (alias of `ADC_DACREF0`) |
| `ADC_PTC` (not particularly useful)    | |

The Ground internal sources are presumable meant to help correct for offset error. On Classic AVRs they made a point of talking about offset cal for differential channels, and often all the channels could be measured

DACREF0 is the the reference voltage for the analog comparator, AC0. On the 1-series, this is the same as the `DAC0` voltage (yup, the analog voltage is shared). Analog comparators AC1 and AC2 on 1-series parts with at least 16k of flash also have a DAC1 and DAC2 to generate a reference voltage for them (though it does not have an output buffer), . On the 2-series, this is only used as the AC0 reference voltage; it cannot be output to a pin. . Unlike the AVR DA-series parts, which have neither `VDDDIV10` nor `INTREF` as an option so reading the reference voltage through `DACREF0` was the only way to determine the operating voltage, there is not a clear use case for the ability to measure this voltage.

### Analog Resolution
The hardware supports increasing the resolution of analogRead() to the limit of the hardware's native resolution (10 or 12 bits); Additionally, we provide automatic oversampling and decimation up to the limit of what can be gathered using the accumulation feature allowing up to 15 bits of resolution (17 on future Ex-series); this is exposed through the `analogReadEnh()` function detailed below.

### A second ADC? *1-series only*
On some parts, yes! The "good" 1-series parts (1614, 1616, 1617, 3216, and 3217) have a second ADC, ADC1. On the 20 and 24-pin parts, these could be used to provide analogRead() on additional pins (it can also read from DAC2). Currently, there are no plans to implement this in the core due to the large number of currently available pins. Instead, it is recommended that users who wish to "take over" an ADC to use it's more advanced functionality choose ADC1 for this purpose. See [ADC free-run and more](megaavr/extras/ADCFreerunAndMore.md) for some examples of what one might do to take advantage of it.  As of 2.0.5, megaTinyCore provides a function `init_ADC1()` which initializes ADC1 in the same way that ADC0 is (with correct prescaler for clock speed and VDD reference). On these parts, ADC1 can be used to read DAC1 (DACREF1) and DAC2 (DACREF2), these are channels `0x1B` and `0x1E` respectively. The core does not provide an equivalent to analogRead() for ADC1 at this time.

### ADC Sampling Speed
megaTinyCore takes advantage of the improvements in the ADC on the newer AVR parts to improve the speed of analogRead() by more than a factor of three over classic AVRs! The ADC clock which was - on the classic AVRs - constrained to the range 50-200kHz - can be cranked up as high as 1.5MHz at full resolution! On the 0/1-series, we use 1MHz at 16/8/4 MHz system clock, and 1.25 MHz at 20/10/5 MHz system clock. To compensate for the faster ADC clock, we set the sample duration to 14 to extend the sampling period from the default of 2 ADC clock cycles to 16 - providing approximately the same sampling period as on classic AVRs (which is probably a conservative decision). On the 2-series, the corresponding setting has only 0.5 or 1 ADC clock added to it, so we set it to 15 - however, it can be set all the way up to 255. The ADC clock is run at approx. 2.5 MHz on the 2-series parts.  If you are measuring a lower impedance signal and need even faster `analogRead()` performance - or if you are measuring a high-impedance signal and need a longer sampling time, you can adjust that setting with `analogSampleDuration()` detailed below.

## ADC Function Reference
This core includes the following ADC-related functions. Out of the box, analogRead() is intended to be directly compatible with the standard Arduino implementation. Additional functions are provided to use the advanced functionality of these parts and further tune the ADC to your application.

### getAnalogReference() and getDACReference()
These return the numbers listes in the reference table at the top as a `uint8_t`

### analogRead(pin)
The standard analogRead(). Single-ended, and resolution set by analogReadResolution(), default 10 for compatibility. Negative return values indicate an error that we were not able to detect at compile time. Return type is a 16-bit signed integer (`int` or `int16_t`).

### analogReadResolution(resolution)
Sets resolution for the analogRead() function. Unlike stock version, this returns true/false. *If it returns false, the value passed was invalid, and resolution was set to the default, 10 bits*. Note that this can only happen when the value passed to it is determined at runtime - if you are passing a compile-time known constant which is invalid, we will issue a compile error. The only valid values are those that are supported natively by the hardware, plus 10 bit, even if not natively supported, for compatibility.
Hence, the only valid values are 10 and 12. The EA-series will likely launch with the same 8bit resolution option as tinyAVR 2-series which would add 8 to that list.

This is different from the Zero/Due/etc implementations, which allow you to pass anything from 1-32, and rightshift the minimum resolution or leftshift the maximum resolution as needed to get the requested range of values. Within the limited resources of an 8-bit AVR with this is a silly waste of resources, and padding a reading with zeros so it matches the format of a more precise measuring method is in questionable territory in terms of best practices. Note also that we offer oversampling & decimation with `analogReadEnh()` and `analogReadDiff()`, which can extend the resolution while keep the extra bits meaningful at the cost of slowing down the reading. `analogReadResolution()` only controls the resolution of analogRead() itself. The other functions take desired resolution as an argument, and restore the previous setting.

### analogSampleDuration(duration)
Sets sampling duration (SAMPLEN), the sampling time will be 2 + SAMPLEN ADC clock cycles. Sampling time is the time the sample and hold capacitor is connected to the source before beginning conversion. For high impedance voltage sources, use a longer sample length. For faster reads, use a shorter sample length.
speed. This returns a `bool` - it is `true` if value is valid.

This value is used for all analog measurement functions.

| Part Series | Sample time  | Conversion time | Total analogRead() time | SAMPDUR/SAMPLEN | ADC clock sample time |
|-------------|--------------|-----------------|-------------------------|-----------------|-----------------------|
| Classic AVR |        12 us |           92 us |                  104 us | No such feature | 1.5                   |
| 0/1-series  |        12 us |       8.8-10 us |              20.8-22 us |     7, 9, or 13 | 2 + SAMPLEN           |
| Dx 10-bit   |   apx. 12 us |  approx.  10 us | Approximately  21-22 us |              14 | 2 + SAMPLEN           |
| Dx 12-bit   |   apx. 12 us |  approx.  12 us | Approximately  23-24 us |              14 | 2 + SAMPLEN           |
| 2-series    |  apx. 6.4 us |  approx. 5.6 us | Approximately     12 us |              15 | 1 + SAMPDUR           |
| EA-series   |  apx. 6.4 us |  approx. 5.6 us | Approximately     12 us |              15 | 1 + SAMPDUR           |


On the 2-series, we are at least given some numbers: 8pF for the sample and hold cap, and 10k input resistance so a 10k source impedance would give 0.16us time constant, implying that even a 4 ADC clock sampling time is excessive, but at such clock speeds, impedance much above that would need a longer sampling period. You probab

### analogReadEnh(pin, res=ADC_NATIVE_RESOLUTION, gain=0)
Enhanced `analogRead()` - Perform a single-ended read on the specified pin. `res` is resolution in bits, which may range from 8 to `ADC_MAX_OVERSAMPLED_RESOLUTION`. This maximum is 13 bits for 0/1-series parts, and 17 for 2-series. If this is less than the native ADC resolution, that resolution is used, and then it is right-shifted 1, 2, or 3 times; if it is more than the native resolution, the accumulation option which will take 4<sup>n</sup> samples (where `n` is `res` native resolution) is selected. Note that maximum sample burst reads are not instantaneous, and in the most extreme cases can take milliseconds. Depending on the nature of the signal - or the realtime demands of your application - the time required for all those samples may limit the resolution that is acceptable. The accumulated result is then decimated (rightshifted n places) to yield a result with the requested resolution, which is returned. See [Atmel app note AVR121](https://ww1.microchip.com/downloads/en/appnotes/doc8003.pdf) - the specific case of the new ADC on the Ex and tinyAVR 2-series is discussed in the newer DS40002200 from Microchip, but it is a rather vapid document). Alternately, to get the raw accumulated ADC readings, pass one of the `ADC_ACC_n` constants for the second argument where `n` is a power of 2 up to 64 (0/1-series), or up to 1024 (2-series). Be aware that the lowest bits of a raw accumulated reading should not be trusted.; they're noise, not data (which is why the decimation step is needed, and why 4x the samples are required for every extra bit of resolution instead of 2x). On 2-series parts *the PGA can be used for single ended measurements*. Valid options for gain on the 2-series are 0 (PGA disabled, default), 1 (unity gain - may be appropriate under some circumstances, though I don't know what those are), or powers of 2 up to 16 (for 2x to 16x gain). On 0/1-series parts, the gain argument should be omitted or 0; these do not have a PGA.

```c++
  int32_t adc_reading = analogReadEnh(PIN_PD2,13);
  // Measure voltage on PD2, to 13 bits of resolution. This will be sampled 4 times using the accumulation function, and then rightshifted once.
  int32_t adc_reading2 = analogReadEnh(PIN_PD2, ADC_ACC128);
  // Take 128 samples and accumulate them. This value, k, is 19 bits wide; on the Dx-series parts, this is truncated to 16 bits - the hardware does not expose the three LSBs.
```

Negative values always indicate a runtime error.

### analogReadDiff(positive, negative, res=ADC_NATIVE_RESOLUTION, gain=0) **2-series only**
Differential `analogRead()` - returns a `long` (`int32_t`), not an `int` (`int16_t`). Performs a differential read using the specified pins as the positive and negative inputs. Any analog input pin can be used for the positive side, but only inputs on PORTA, or the constants `ADC_GROUND` or `ADC_DAC0` can be used as the negative input. Information on available negative pins for the Ex-series is not yet available, but is expected to be a subset of available analog pins. The result returned is the voltage on the positive side, minus the voltage on the negative side, measured against the selected analog reference. The `res` parameter works the same way as for `analogReadEnh()`, as does the `gain` function. Gain becomes FAR more useful here than in single-ended mode as you can now take a very small difference and "magnify" it to make it easier to measure. Be careful when measuring very small values here, this is a "real" ADC not an "ideal" one, so there is a non-zero error, and through oversampling and/or gain, you can magnify that such that it looks like a signal.

```c++
  int32_t diff_first_second = analogReadDiff(PIN_PA1, PIN_PA2, 12, 0);
  // Simple differential read to with PA1 as positive and PA2 as negative. Presumable these are close in voltage.
  // (I used two pots and manually adjusted them to be very close; also could have done like 10k-100ohm-10k resistor network)
```

The 32-bit value returned should be between -65536 and 65535 at the extremes with the maximum 17-bit accumulation option, or, 32-times that if using raw accumulated values (-2.1 million to 2.1 million, approximately)

### analogClockSpeed(int16_t frequency = 0, uint8_t options = 0)
The accepted options for frequency are -1 (reset ADC clock to core default, 1-1.35 MHz), 0 (make no changes - just report current frequency) or a frequency, in kHz, to set the ADC clock to. Values between 125 and 1500 are considered valid for 0/1-series parts and for 2-series parts 300-3000 with internal reference, and 300-6000 with Vdd or external reference. The prescaler options are discrete, not continuous, so there are a limited number of possible settings (the fastest and slowest of which are often outside the rated operating range). The core will choose the highest frequency which is within spec, and which does not exceed the value you requested. If a 1 is passed as the second argument, the validity check will be bypassed; this allows you to operate the ADC out of spec if you really want to, which may have unpredictable results. Microchiop documentation has provided little in the way of guidance on selecting this (or other ADC parameters) other than giving us the upper and lower bounds.

**Regardless of what you pass it, it will return the frequency in kHz**

The 0/1-series has prescalers in every power of two from 2 to 256, and at the extreme ends, typical operating frequencies will result in an ADC clock that is not in spec.

The 2-series has a much richer selection of prescalers: Every even number up to 16, then 20, 24, 28, 32, 40, 48, 56, and 64, giving considerably more precision in this adjustment.

In either case you don't need to know the range of prescalers, just ask for a frequency and we'll get as close as possible without exceeding it, and tell you what was picked

```c++
// ******* for both *******
Serial.println(analogClockSpeed());     // prints something between 1000 and 1500 depending on F_CPU
analogClockSpeed(300);                  // set to 300 kHz
Serial.println(analogClockSpeed());     // prints a number near 300 - the closest to 300 that was possible without going over.

// ****** For Dx-series *******
Serial.println(analogClockSpeed(3000)); // sets prescaler such that frequency of ADC clock is as
// close to but not more than 2000 (kHz) which is the maximum supported according to the datasheet.
// Any number of 2000 or higher will get same results.
Serial.println(analogClockSpeed(20));   // A ridiculously slow ADC clock request.
// Datasheet says minimum is 125, maximum prescale(256) would give 93.75 kHz - but that's not
// within spec, and no second argument was passed; we will instead set to 128 prescaler
// for clock speed of 187.5 kHz and will return 187,
Serial.println(analogClockSpeed(20, 1)); // As above, but with the override of spec-check
// enabled, so it will set it as close to 20 as it can (93.75) and return 93.

// ****** For Ex-series *******
Serial.println(analogClockSpeed(20000));   // Above manufacurer max spec, so seeks out a value that
// is no larger than that spec; 3000 if internal reference selected or 6000 otherwise.
Serial.println(analogClockSpeed(20000,1)); // Above manufacturer's spec. but we instruct
// it to ignore the spec and live dangerously. The requested speed, far in excess of the
// maximum obtainable (which is several times the maximum supported) speed, with limits bypassed,
// will lead it to choose the fastest possible ADC clock, which is only prescaled by a factor of 2,
// and return that value, likely 8000 or 10000 unless for 16 or 20 MHz. (higher if you're overclocking
// the chip in general too). Expect inaccurate results of other bad behavior.

// ******* For both *******
int returned_default = analogClockSpeed(-1); // reset to default value, around 1000 - 1500 kHz for Dx and around 2500 for Ex.

Serial.println(returned_default);  // will print the same as the first line, assuming you hadn't changed it prior to these lines.
```
If anyone undertakes a study to determine the impact of different ADC clock frequency on accuracy, take care to adjust the sampling time to hold that constant. I would love to hear of any results; I imagine that lower clock speeds should be more accurate, but within the supported frequency range, I don't know whether these differences are worth caring about.
I've been told that application notes with some guidance on how to best configure the ADC for different jobs is coming. Microchip is aware that the new ADC has a bewildering number of knobs compared to classic AVRs, where there was typically only 1 degree of freedom, the reference, which is simple to pick and undeerstand, since only one prescaler setting was in spec.

### getAnalogReadResolution()
Returns either 10 or 12, the current resolution set for analogRead.

### getAnalogSampleDuration()
Returns the number of ADC clocks by which the minimum sample length has been extended.

## ADC Runtime errors
When taking an analog reading, you may receive a value near -2.1 billion - these are runtime error codes.
The busy and disabled errors are the only ones that we never know at compile time.
| Error name                     |     Value   | Notes
|--------------------------------|-------------|---------------------------------------------------------------------
|ADC_ERROR_INVALID_CLOCK         |      -32764 | Returned by analogSetClock() if, somehow, it fails to find an appropriate value. May be a cant-happen.
|ADC_ERROR_BAD_PIN_OR_CHANNEL    |      -32765 | The specified pin or ADC channel does not exist or does support analog reads.
|ADC_ERROR_BUSY                  |      -32766 | The ADC is busy with another conversion
|ADC_ERROR_DISABLED              |      -32767 | The ADC is disabled at this time.
|ADC_ENH_ERROR_BAD_PIN_OR_CHANNEL| -2100000000 | The specified pin or ADC channel does not exist or does support analog reads.
|ADC_ENH_ERROR_BUSY              | -2100000001 | The ADC is busy with another conversion.
|ADC_ENH_ERROR_RES_TOO_LOW       | -2100000003 | Minimum ADC resolution is 8 bits. If you really want less, you can always rightshit it.
|ADC_ENH_ERROR_RES_TOO_HIGH      | -2100000004 | Maximum resolution, using automatic oversampling and decimation is 15, and will be 17 on Ex-series
|ADC_DIFF_ERROR_BAD_NEG_PIN      | -2100000005 | analogReadDiff() was called with a negative input that is not valid.
|ADC_ENH_ERROR_DISABLED          | -2100000007 | The ADC is currently disabled. You must enable it to take measurements.

### ADCPowerOptions(options) *2-series only*
The PGA requires powere when turned on. It is enabled by any call to `analogReadEnh()` or `analogReadDiff()` that specifies valid gain > 0; if it is not already on, this will slow down the reading. By default we turn it off afterwards. There is also a "low latency" mode that, when enabled, keeps the ADC reference and related hardware running to prevent the delay (on order of tens of microseconds) before the next analog reading is taken. We use that by default, but it can be turned off with this function.
Generate the argument for this by using one of the following constants, or bitwise-or'ing together a low latency option and a PGA option. If only one option is supplied, the other configuration will not be changed.
* `LOW_LAT_OFF`   Turn off low latency mode.
* `LOW_LAT_ON`    Turn on low latency mode.
* `PGA_OFF_ONCE`  Turn off the PGA now. Don't change settings; if not set to turn off automatically, that doesn't change.
* `PGA_KEEP_ON`   Enable PGA. Disable the automatic shutoff of the PGA.
* `PGA_AUTO_OFF`  Disable PGA now, and in future turn if off after use.
Example:
```c
ADCPowerOptions(LOW_LAT_ON  | PGA_KEEP_ON ); //low latency on. Turn the PGA on, and do not automatically shut it off. Maximum power consumption, minimum ADC delays.
ADCPowerOptions(LOW_LAT_OFF | PGA_AUTO_OFF); //low latency off. Turn off the PGA and enable automatic shut off. Minimum power consumption, maximum ADC delays.
```

**More advanced method** This is not recommended, as it is harder to read; however under the hood the two are identical, and if setting this to something determined at runtime, this may be easier to implement. The argument is a number between 0x00 and 0x0F. The structure is 0b 0000 PPLL - P bits control the PGA power state, L bits the LOWLAT setting.
* `LL = 00` Take no action on low latency mode.
* `LL = 01` Take no action on low latency mode.
* `LL = 10` Turn off low latency mode.
* `LL = 11` Turn on low latency mode.
* `PP = 00` Don't change anything relating to the PGA.
* `PP = 01` Turn off the PGA now. Don't change settings; if not set to turn off automatically, that doesn't change.
* `PP = 10` Enable PGA. Disable the automatic shutoff of the PGA.
* `PP = 11` Disable PGA now, and in future turn if off after use

Example:
```c
ADCPowerOptions(0x0B); //0x0F = 0b 0000 1011 LL = 11, so low latency on. PP = 10, so PGA on, automatic shut off disabled. Maximum power consumption, minimum ADC delays.
ADCPowerOptions(0x0E); //0x02 = 0b 0000 1110 LL = 10, so low latency off. PP = 11, turn off the PGA and enable automatic shut off. Minimum power consumption, maximum ADC delays.
```


### DAC Support
The 1-series parts have an 8-bit DAC which can generate a real analog voltage. This generates voltages between 0 and the selected VREF (which cannot be VDD, unfortunately). Set the DAC reference voltage via the `DACReference()` function - pass it one of the `INTERNAL` reference options listed under the ADC section above. This voltage must be half a volt lower than Vcc for the voltage reference to be accurate. The DAC is exposed via the analogWrite() function: Call `analogWrite(PIN_PA6,value)` to set the voltage to be output by the DAC. To turn off the DAC output, call digitalWrite() on that pin; note that unlike most* PWM pins `analogWrite(PIN_PA6,0)` and `analogWrite(PIN_PA6,255)` do not act as if you called digitalWrite() on the pin; 0 or 255 written to the `DAC0.DATA` register; thus you do not have to worry about it applying the full supply voltage to the pin (which you may have connected to sensitive devices that would be harmed by such a voltage) if let the calculation return 255; that will just output 255/256ths of the reference voltage.


## Analog *channel* identifiers
The ADC is configured in terms of channel numbers, not pin numbers. analogRead() hence converts the number of a pin with an analog channel associated with it to the number of that analog channel, so there is no need to deal with the analog channel numbers. The one exception to that is in the case of the non-pin inputs, the constants like ADC_DAC and ADC_VDDDIV10. I have a simple system to internally signal when a number isn;t an digital pin number, but an analog channel number. These are what I cal analog channel identifiers, and they're the value that the analog mux is set to in order, with the high bit set 1 (no part has more than 128 possible values on their mux, let alone 128 valid ones; the precise numeric values will be handled on an adhoc basis if/when it becomes an issue. With 254 valid values, the current design provides room for 127 digital pins and 127 analog inputs. No AVR released has come anywhere close to that.

These numbers (they do have defines in the variants, and `ADC_CH()` will take an analog channel number (ex, "0") and turn it into the analog channel identifier. But you never need to do that unless you're getting very deep into a custom ADC library . ) The most common exaople when channels are used is when reading from things that are not pins - like the internal tap on the DAC output, or the VDDDIV10 value to find the supply voltage. These may overlap with pin numbers. Also internally, channel numbers are sometimes passed between functions. They are defined for pins that exist as analog channels, with names of rthe form `AINn` but **you should never use the AIN values** except in truly extraordinary conditions, and even then it's probably inappropriate. However I felt like mention of them here wax needed. Some macros abd helper funbctions involved are:

```text
digitalPinToAnalogInput(pin)    - converts an digital pin to an anbalog channel *without* the bit that says it's a channel (designed for the very last step of analogRead preparationm where we turn the pin number into the channel to set the MUX)
analogChannelToDigitalPin(p)    - converts an analog channel *number* to a digital pin.
analogInputToDigitalPin(p)      - converts an analog channel identifier ti a digital pin number.
digitalOrAnalogPinToDigital(p)  - converts an analog channel identifier or digital pin to a digital pin number
ADC_CH(channel number)          - converts channel numbers to analog chaannel identifier

```

Try not to use these unless you're getting really deep into library development and direct interaction with the ADC.
