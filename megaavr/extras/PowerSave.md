# Optimizing for power consumption, using Sleep Mode and the Real Time Counter (RTC)
There are several solutions to reduce the power consumption on the Dx series. Here are some suggestions to get longer life out of your battery or super capacitor:

## First, think about your power budget
Different approaches are warranted depending on what you are targeting. If you are building a remote control running on a cr2032 and you want the battery to run for a year, your power budget is an average of 25ua. In that case you cannot have a BOD running continuously. On the other hand if you are targeting 10s of ma (larger lipo battery, few days of use per charge) the reliability benefits may be worth the 17ua. There are 8760 hours per year. You should think about how much power you have based on your battery size and probably multiply current figures here by whatever time period you care about to decide how to make various tradeoffs.

## Reducing the clock speed
The clock speed is very important if (and only if) you program will be spending most of its time active (as opposed to sleeping, see below). A slower clock speeds will reduce the power consumption, approximately linearly with clock speed. It used to be that the lower the clock speed, the lower the permitted operating voltage - with the Dx-series, that is not the case: all speeds are in spec at all voltages! With proper use of sleep the slowest clock is not always the most efficient. Sometimes 5mhz will use less power than 1mhz (because more time can be spent asleep).

The clock speed can be set from the Arduino IDE. Go to Tools -> Clock Speed (when using an external clock, the selected speed must match the external clock or crystal)

We often get the request to add the 32KHz option to the clock speed menu since it only requires 13.5μA. While it is possible to run the main clock from this low power oscillator, it is not recommended. It will make certain peripherals unusable, and timers used for delay and millis require the faster oscillator to run anyway. Instead, make aggressive use of sleep.

## Disable BOD
If your power budget is tiny, make sure the Brown Out Detection is either disabled or sampled in sleep modes (and maybe active) (BOD Mode active and BOD Mode sleep) (of course, this means that as the battery dies, your project will misbehave due to brownout instead of just not working at all, so this may not be a desirable tradeoff). Sampled BOD in sleep mode may be a good tradeoff. Continuous takes 17 uA, while 32 Hz sampled BOD takes only 600 nA (per datasheet).

## Reduce the voltage
(is it better on the Dx-series? Or does it not make a difference because VCore is the same?) Various pull-ups and pull-downs will waste less power. You also might consider a switching regulator which can save you significant power. The LM3671 is quite simple and requires about the same component count and board area as an LDO. Consider running at 2.5-2.8v for more savings (but check the requirements of other circuitry).

## Check for unexpected power hogs
Most 1117 style LDOs suck (power) 5mA is not unusual.. Some simply suck (counterfeit chinese AMS1117's are known to exist, and the short circuit protection doesn't work). On the other hand, the LDL1117 regulator has a typical I<sub>q</sub> of just 250uA. If you need low power consumption always verify the quiescent current of all the parts your using. Surprises abound. If you use LEDs and habitually use a 220 or 330 ohm resistor, depending on the environment and choice of LED, a 10K be bright enough to be visible, and will save a lot of power; it may also be easier on the eye in a dark environment. Ideally, always-on LEDs should be avoided entirely. Also pay attention to the strength of your pullup resistors, especially if they'll be spending a good portion of the time driven to the opposite power rail (consider if you can rearrange things such that that isn't the case). One of the worst offenders, though, is a voltage divider that causes static draw all the time (definitely do not do this to measure the supply voltage on an AVR DB, AVR DD, or TinyAVR 2-series - these have a buildtin VDDDIV10 channel, which can by read with `analogRead(ADC_VDDDIV10)`. Be wary of modules, particularly the dirt cheap ones from China - these were generally designed without concern for power efficiency

## Disable unwanted peripherals
DO NOT use <avr/power.h> - it is for legacy parts and does nothing (and the avr-libc team has not updated it).

## When not sleeping, make sure to disable any peripherals you are not using
For example:
```c
TCA0.SPLIT.CTRLA = 0; //If you aren't using TCA0 for anything
```
By default at startup, DxCore enables all timers for use with PWM; if you're not using them you can also override the initialization functions with empty ones, see [callback reference](Ref_Callbacks.md)

## Beware of the ADC in sleep mode

The ADC can be a source of power drain.
```c++
// Before sleeping
  ADC0.CTRLA &= ~ADC_ENABLE_bm; //Very important on the tinyAVR 2-series
// upon waking if ou plan to use the ADC
  ADC0.CTRLA |= ~ADC_ENABLE_bm;
```
may reduce draw by hundreds of ua. You should also be sure to set all unused pins to either INPUT_PULLUP, OUTPUT, or disable their input (see [the Digital Pin Function referernce](Ref_Digital.md))

This can also be achieved with the `analogPowerOption()` function (see [the analog reference](Ref_Analog.md))

Other things that can waste power in standby sleep mode if not disabled include the RTC, the CCL (if set to use a clock source and run in standby mode), and any peripheral set to run in standby mode (not only do these keep the peripheral on, they keep their clock source on, which is often a larger power draw)

## Use Sleep Mode
The absolute minimum power consumption of the device, when all the peripherals are stopped, is 0.1μA. This can be achieved in power-down sleep mode; enabling the WDT and/or RTC (to keep track of time) but will continue where it left off after it receives an interrupt. While the CPU is not running, the device itself is still monitoring all the interrupts out there and to do so has to keep the peripherals running. Which peripherals are running depends on the sleep mode setting selected before entering sleep mode. The following 3 options are available:

1. Idle - only the CPU is turned off. All peripherals continue to run.  This does not save very much power.
1. Standby - on classic AVRs, this was "like power-down, only you don't save as much power". No longer - on the modern parts it is much more flexible and capable of saving as much or nearly as much power as power down mode! Obviously, the CPU is turned off. So are all peripherals unless you specifically set them to run in standby. All unneeded clock sources are also turned off - which clock sources those are depend on the settings of any peripherals set to run in standby (and unfortunately, a great many do use the main oscillator)
1. Power Down, the CPU is turned off and all Peripherals (except the WDT and RTC) are shut down. Only the PIT (RTC), Pin change and TWI Address Match interrupts can wake up the device. The same mode was available on classic AVRs with similar effects.

(wait, what? TWI address match? I knew about PCINT and RTC, but where did TWI address match come from? - Actually, if you go back and look for it, it's available in classic AVRs too. Who knows what the cost is in terms of power and die area, but it doesn't appear to be much. If you imagine making a TWI sensor built around one of these parts, that is not just nice to have, it is a 100% must have feature.

Waking from Idle sleep mode is almost instantaneous - only 6 clock cycles. Waking from  a sleep mode where the oscillator is stopped (power down, or standby if nothing requiring the oscillator is left running) takes 8 uS + 10 clock cycles.

The sleep functions are not part of the Arduino Core, but a very basic wrapper is provided by `<avr/sleep.h>`, part of avr-libc (included with the compiler toolchains). You must include this to use the sleep wrappers.

```cpp
#include <avr/sleep.h>
```

Before entering sleep mode, the desired sleep mode must be selected with set_sleep_mode():

```cpp
set_sleep_mode(SLEEP_MODE_PWR_DOWN);
```

Other options are:

```cpp
set_sleep_mode(SLEEP_MODE_IDLE);
set_sleep_mode(SLEEP_MODE_STANDBY);
```

After the sleep mode setting is selected, the sleep mode function must be enabled. Enabling sleep mode does not put the device in sleep mode, but it enables the sleep mode function:

```cpp
sleep_enable();
```

This can all be done in the setup() section of your Arduino sketch, see an example below.
Entering the actual sleep mode is done with:

```cpp
sleep_cpu();
```

This can be in your loop() section of your sketch.
Now the device will enter sleep mode and you will notice (depending on your sleep mode setting) the power consumption is less than 1μA (very difficult to measure). Waking it up with the real time counter is described next.

### Unused pins and sleep modes
**IMPORTANT** In order to minimize power consumption, you *must* eliminate or disable all floating pins. A floating pin is any pin which is not being driven high or low (by this or another device) nor held in a specific state (for example by pullup resistors). Because these pins transition randomly in response to ambient noise, those digital input buffers are consuming power with every transition. To minimize power consumption, one or more of the following must apply to every pin:
* It is set as an OUTPUT
* It is set INPUT_PULLUP or the internal pullups are otherwise enabled
* It is connected to another device which is holding it HIGH or LOW
* The input buffer is disabled in the PORTx.PINnCTRL register (a wrapper function around this is planned for a future update)

If those steps are not followed, the power consumption may be orders of magnitude higher than expected in sleep modes! When left as UPDI or when used as RESET, the UPDI pin has an internal pullup enabled and no action is necessary for that pin.

## Using the Real Time Counter (RTC/PIT)
The real time counter is like the WDT in interrupt mode on a classic AVR. On these new parts the WDT can only reset the device. Interrupting at a certain time period is now done with the RTC and PIT (Programmable Interrupt Timer). These are tightly coupled peripherals. On many tinyAVR 0/1-series parts, you cannot turn off one without losing the other (see the applicable silicon errata).
The RTC can run on the 32KHz low power oscillator and will trigger an interrupt after a certain time period. This period is based on the number of clock cycles. For example, when we have a period of 32768, this will take 1 second to count to by using the 32KHz oscillator (32.768kHz/32768= 1Hz = 1 sec.). After we reach 32768 we will trigger a Periodic Interrupt (PIT). We will be using PIT since that is the only timer interrupt we can use in Power Down mode. In standby sleep mode, the RTC interrupts are available. These are much more flexible - you get both an overflow (OVF) and a compare-match (CMP) - and you can set both the period and compare values to any 16-bit value, whereas the PIT interrupt can only be enabled at powers-of-two RTC clock cycles.

Here is an example of BLINK using the PIT interrupts and sleep instead of delay:

```cpp
#include <avr/sleep.h>

void RTC_init(void)
{
  /* Initialize RTC: */
  while (RTC.STATUS > 0)
  {
    ;                                   /* Wait for all register to be synchronized */
  }
  RTC.CLKSEL = RTC_CLKSEL_INT32K_gc;    /* 32.768kHz Internal Ultra-Low-Power Oscillator (OSCULP32K) */

  RTC.PITINTCTRL = RTC_PI_bm;           /* PIT Interrupt: enabled */

  RTC.PITCTRLA = RTC_PERIOD_CYC16384_gc /* RTC Clock Cycles 16384, resulting in 32.768kHz/16384 = 2Hz */
  | RTC_PITEN_bm;                       /* Enable PIT counter: enabled */
}

ISR(RTC_PIT_vect)
{
  RTC.PITINTFLAGS = RTC_PI_bm;          /* Clear interrupt flag by writing '1' (required) */
}

void setup() {
  RTC_init();                           /* Initialize the RTC timer */
  pinMode(7, OUTPUT);                   /* Configure pin#7 as an output */
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);  /* Set sleep mode to POWER DOWN mode */
  sleep_enable();                       /* Enable sleep mode, but not going to sleep yet */
}

void loop() {
  sleep_cpu();                          /* Sleep the device and wait for an interrupt to continue */
  digitalWrite(7, CHANGE);              /* Device woke up and toggle LED on pin#7 */
}
```
### RTC and Synchronization
The RTC is in its own "clock domain" and the microcontroller has to "synchronize" configuration changes between the registers controlling the RTC, and the RTC itself. The RTC itself is also - compared to the CPU - SLOW (32.768 kHz vs 20 MHz). While this is not the only peripheral with such synchronization concerns, they are *very* prominent here due to that speed difference. Always check RTC.STATUS before writing to RTC/PIT registers. If you test a sketch that uses these and it doesn't work, one of your first thoughts should be "Is the RTC configured correctly? Do I KNOW that it is? Maybe I'd better make sure by reading the values out of the registers after I think I've set them..." - Generally the consequence of failing to wait for synchronization is that writes to the register in question will be ignored; when the setting in question is what makes the chip wake up after you put it to sleep, and it goes to sleep upon start-up, there may not be much functionality left.

## Sleep and Serial ports
If there are any serial ports which you print output to, before going to sleep, be sure to let them finish printing everything in their transmit buffer by calling `Serial.flush()`. Also, wake on serial seems to lose data outside of sleep mode idle. And if you have serial ports connected during your power measurements, current leakage through the serial port may affect your measurements. A pair of schottky diodes + pull-ups to block current flow over the serial port data lines can solve this - BAT20JFILM is a reasonably low leakage option, 0.65ua typ at 5v.

```text
             serial power
                |
mcu tx  --<|--- z------ usb serial rx

                    mcu power
                       |
usb serial tx  --<|--- z------ mcu rx
```

## 2-Series ADC Power Consumption
When using a 2-series part, the ADC should be disabled during power down sleep mode, as it will otherwise consume 130uA continuously, almost 2 orders of magnitude more than the rest of the chip in this mode just like on classic AVRs. It may be related to the LOWLAT mode, but the LOWLAT mode needs to be enabled on some 2-series parts due to one of the few errata known for the new ADC. Turning off the ADC during sleep is currently our recommended workaround and can be doone by clearing the `ADC_ENABLE` bit of `ADC0.CTRLA` (and setting it upon wake) or using the `ADCPowerOptions()` function as detailed in [the Analog reference document](https://github.com/SpenceKonde/megaTinyCore/blob/master/megaavr/extras/Ref_Analog.md#adcpoweroptionsoptions-2-series-only-prior-to-2512)

## Future Development
There are plans for a library to provide improved wrappers around sleep modes, particularly regarding timekeeping. This will also provide a means to handle serial ports automatically - including waking the part upon seeing an incoming character on the serial port. The latter is more complicated than the datasheet implies due to a widespread silicon bug with SFD (Start-of-Frame Detection).

## Notes
1. "Pin change with restrictions? Like always, sure. RTC? But of course, that's what it's here for. TWI address match? Wait, we can wake from deepest sleep modes on that?" If your first thought is "one of these things looks out of place here" - that was mine too. One presumes that this can be implemented asynchronously at a low cost (it is, effectively, just a shift register and 7-bit binary comparator...). I don't know how difficult of an engineering task it was, or what compromises were made for it, but if you imagine building an I2C device with one of these as it's core, this is functionality has a very high payback.
