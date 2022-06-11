# Advanced ADC functionality
**This document has not been updated to cover the 2-series parts** which have an all new ADC - the features still exist, but the registers and specific behavior is different.

## Introduction to free-running mode
ADCs can be configured to operate in what is called "free running" mode. In this mode, the ADC will measure the voltage on a specified input, and - as soon as that is done - start another measurement, or "conversion". For applications that require continually monitors an analog input (particularly a single one), this can be more convenient than requesting each read individually. More importantly, it is, effectively, much faster - analogRead() kicks off a single conversion, and then "busy-waits" for the conversion to complete - with the ADC sampling in the background with free-running mode, you can do other things during this time. This is critical when the sample rate is critical - and you must perform calculations on each reading as well.

Free-running mode is also particularly useful if you need to do something if the reading is above or below certain value - the modern AVR parts have a special feature just for this application: the Window Comparator mode - in this mode, you set a high and low threshold and put the ADC into free running mode - an interrupt will fire when the result meets the specified conditions (see below for more details). On the ATtiny1614, 1616, 1617, 3216, and 3217, there is actually a *second* ADC available. On these parts, free-running mode is even more useful - you can have the second ADC continually monitoring one input in free-running mode (either to use the results directly, or via the window comparator). Note that it is *also* possible on the 1-series parts to put 2 of the analog comparators into a window comparator mode as well!

### Enabling free-running mode
```c++
  ADC0.MUXPOS=0x06; //reads from PA6/arduino pin 2, ADC0 channel 6
  ADC0.CTRLA=ADC_ENABLE_bm|ADC_FREERUN_bm; //start in freerun
  ADC0.COMMAND=ADC_STCONV_bm; //start first conversion!
```

The `MUXPOS` is the number of the analog channel that you want to read in free running mode. See the table below - you can also use digitalPinToAnalogInput(p) - obviously this only works for ADC0.

Pin     | 8-pin | 14-pin | 20-pin | 24-pin | ADC0  | ADC1
--------|-------|--------|--------|--------|-------|----
PIN_PA0 |   5   |   11   |   17   |   21   | AIN0  | -
PIN_PA1 |   2   |   8    |   14   |   18   | AIN1  | -
PIN_PA2 |   3   |   9    |   15   |   19   | AIN2  | -
PIN_PA3 |   4   |   10   |   16   |   20   | AIN3  | -
PIN_PA4 |   -   |   0    |   0    |   0    | AIN4  | AIN0
PIN_PA5 |   -   |   1    |   1    |   1    | AIN5  | AIN1
PIN_PA6 |   0   |   2    |   2    |   2    | AIN6  | AIN2
PIN_PA7 |   1   |   3    |   3    |   3    | AIN7  | AIN3
PIN_PB7 |   -   |   -    |   -    |   4    |   -   | AIN4
PIN_PB6 |   -   |   -    |   -    |   5    |   -   | AIN5
PIN_PB5 |   -   |   -    |   4    |   6    | AIN8  | -
PIN_PB4 |   -   |   -    |   5    |   7    | AIN9  | -
PIN_PB5 |   -   |   4    |   6    |   8    |   -   | -
PIN_PB4 |   -   |   5    |   7    |   9    |   -   | -
PIN_PB1 |   -   |   6    |   8    |   10   | AIN10 | -
PIN_PB0 |   -   |   7    |   9    |   11   | AIN11 | -
PIN_PC0 |   -   |   -    |   10   |   12   |   -   | AIN6
PIN_PC1 |   -   |   -    |   11   |   13   |   -   | AIN7
PIN_PC2 |   -   |   -    |   12   |   14   |   -   | AIN8
PIN_PC3 |   -   |   -    |   13   |   15   |   -   | AIN9
PIN_PC4 |   -   |   -    |   -    |   16   |   -   | AIN10
PIN_PC5 |   -   |   -    |   -    |   17   |   -   | AIN11

### Reading Results
When using the ADC in free running mode, you must write your own code to get the value. Typically, you want to grab the most recent value; one decision to make is whether you want to wait for a reading if you have already read the value since the last reading was taken, or if that doesn't matter.

Reading the value when you want to make sure you have a fresh value is straightforward:
 ```c++
uint16_t getAnalogValue() {
  while (!(ADC0.INTFLAGS & ADC_RESRDY_bm)) { // Check for the RESRDY flag (Result Ready)
    ;//spin if there's no new result
  }
  return ADC0.RES; // return the current result on channel A - this clears the RESRDY flag
}
```
If you don't, it's even simpler:
 ```c++
uint16_t getAnalogValue() {
  return ADC0.RES; // return the most recent result on channel A
}
```

Alternately, you may wish to use an ISR when the ADC conversion is complete - though be aware that with the ADC so much faster on these parts, you may find yourself dealing with more interrupts than you were expecting (unless you are using the accumulation function, see below). This makes sense when there's a clear thing that you want to do with each result - just be sure that whatever it is, it's fast (for example, if you're controlling PWM duty cycle, write the register directly, don't use analogWrite()).
 ```c++
// during setup procedures, before starting freerunning mode conversions.
ADC0.INTCTRL=ADC_RESRDY_bm; //enable the interrupt

// The ISR
ISR(ADC0_RESRDY_vect) {
  useADCValue(ADC0.RES); //reading ADC0.RES clears the RESRDY flag.
  // If for some reason you're not reading the value, though, you need to manually clear the flag by writing a 1 to it.
}
```

### Turning off free-running mode
To stop taking samples in free running mode, simply clear the `ADC_FREERUN_bm` bit in `ADCn.CTRLA`. Be aware that an "extra" reading is always taken when disabling free running mode. See [silicon errata](Errata.md) for more details.

### Changing channel, other settings in free running mode
It is recommended to stop free running mode prior to changing the channel. On some tinyAVR parts, most ADC settings cannot be changed without first turning off free running mode. See [the table of silicon errata](Errata.md)

## Automatic Accumulation
One of the most common applications of multiple consecutive ADC reads is to sum multiple readings to take an average, or for the purpose of increasing accuracy through the technique of oversampling and decimation. The modern AVR devices have an automatic accumulator feature to do this for you to make this process easier; it is controlled by `ADCn.CTRLB`. This is the only thing controlled by this register, so you can set it directly. Valid options are:
* `ADC_SAMPNUM_ACC1_gc`
* `ADC_SAMPNUM_ACC2_gc`
* `ADC_SAMPNUM_ACC4_gc`
* `ADC_SAMPNUM_ACC8_gc`
* `ADC_SAMPNUM_ACC16_gc`
* `ADC_SAMPNUM_ACC32_gc`
* `ADC_SAMPNUM_ACC64_gc`

When the conversion is complete, you can read it from the `ADCn.RES` register. This can be used either with the builtin analogRead() functions (set this register before using analogRead()), in free running mode, or manually. Because taking large numbers of samples in this way can take a while, manually starting a conversion after setting an interrupt on RESRDY may be of particular use here.

## Oversampling and decimation
If you need more accuracy - assuming the signal is changing (it won't work if you get the same result every time - you'll just get that value out) - you can use the technique of "oversampling and decimation" - for each extra bit of accuracy you want, take 4 times as many samples, and then right-shift the result one bit. For example, if we want 1e3 bits of accuracy from a 10-bit ADC:
 ```c++
// global variables:
volatile uint16_t latest_reading=0; // remember to disable interrupts while reading this so it doesn't get changed by the ISR while you're halfway through reading it.

// Setup:
ADC0.CTRLB=ADC_SAMPNUM_ACC64_gc;  //take 64 samples for 3 extra bits
ADC0.MUXPOS=0x06; //reads from PA6/arduino pin 2, ADC0 channel 6
ADC0.CTRLA=ADC_ENABLE_bm|ADC_FREERUN_bm; //start in freerun
ADC0.INTCTRL = 1 << ADC_RESRDY_bp  /* Result Ready Interrupt Enable: enabled */
               | 0 << ADC_WCMP_bp; /* Window Comparator Interrupt Enable: disabled */
ADC0.COMMAND=ADC_STCONV_bm; //start first conversion!

// These readings will take forever (1.5-1.8ms), so we will keep that global updated via an ISR:
ISR(ADC0_RESRDY_vect) {
  uint16_t raw_reading=ADC0.RES;
  latest_reading=raw_reading>>3; // 0~8191
}
```

Note that future updates of megaTinyCore may provide an automated way of configuring analogRead() to do this via an extension to analogReadResolution() - though the need for non-blocking code may render this inappropriate.

## Faster ADC conversions? Extra-high-impedance sources?
On classic AVRs, the ADC wanted to be clocked at 100-200kHz, and sampled for 1.5 ADC clocks; hence, the sampling time was 15-30 uS. On the tinyAVR 0/1-series, the ADC clock can be up to 1.5 MHz. megaTinyCore configures it for 1 MHz on 16 MHz derived clocks, and 1.25 MHz on 20 MHz derived clocks. These parts also provide the ADCn.SAMPCTRL register to allow a longer sampling time - the sampling time is 2+ADCn.SAMPCTRL. By default, megaTinyCore configures this for 14. 14+2 = 16; this is a conservative attempt to ensure that the accuracy on modern AVR parts does not suffer with higher-impedance sources, particularly ones that worked with classic AVRs. Since the faster clock permits much faster conversions anyway, this was considered an acceptable limitation for the default. Because the sampling cap is smaller on these parts, though, this is probably significantly higher than it needs to be. For low impedance sources, this can be cranked all the way back to 0 if conversion speed is of particular importance - this will reduce the conversion time from 29 ADC clocks (29us or 23us for 16 and 20 MHz derived clocks, respectively) to 13 (13us or 10.5us). By comparison, on classic AVRs, the ADC conversion time was between 65u and 130us (104us on most common clock speeds).

On the other hand, if you are reading a particularly high impedance source, you may wish to *increase* the sampling time instead. SAMPCTRL can be set as high as 31, which would give a sampling time of 33 ADC clocks (conversion time 46 ADC clocks - 46 or 37 us).

As

## "Window Comparator" mode
The tinyAVR 0/1-series ADC also supports "Window Comparator" mode. In this mode, you set two registers, ADC0.WINLT and ACD0.WINHT to the 10-bit values that are your "low threshold" and "high threshold" (no, you can't use accumulation here), and set ADC0.CTRLE to specify what conditions you want to trigger an interrupt. In this mode (presumably paired with free-running mode), the ADC0_WCMP_vect interrupt will fire when a sample is taken and it meets those conditions. Note that this could include reading the analog voltage reference as a way of monitoring Vcc when running on battery - this would be one way of implementing a low battery alarm, for example. Take care to account for the fact that if the ADC is in free-running mode, unless you stop it, change window thresholds, or turn off the interrupt, you'll keep getting interrupts every time a new reading is ready.

## ADC1 on ATtiny1614, 1616, 1617, 3216, 3217
These parts have a second ADC, just like the first - everything about it is identical except the pin assignments. As of 2.0.5, megaTinyCore provides a function `init_ADC1()` that can be called to initialize ADC1 the same way as ADC0 is initialized. This is particularly convenient if writing library code which may run on multiple clock speeds, as you don't have to duplicate the calculation of an appropriate prescaler based on F_CPU. Because it is fully independent, it is well suited to use in the background in free-running mode (possibly with the window comparator mode enabled) for long-running accumulated readings, or if you need to alternately read from two pins with differing analog voltages. Under ordinary circumstances (something where you could be using analogRead() to read pins, without any special configuration options) there is no particular reason to use it.
