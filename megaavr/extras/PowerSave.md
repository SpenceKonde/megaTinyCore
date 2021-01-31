# Optimizing for power consumption, using Sleep Mode and the Real Time Counter (RTC)
There are several solutions to reduce the power consumption on the ATtiny. Here are some suggestions to get longer life out of your battery or super capacitor:

## Reducing the clock speed
The clock speed is very important for the following two reasons:

1.  Slower clock speeds will reduce the power consumption
1.  Minimum voltage to keep the CPU running is lower with a slower clock speed

The clock speed can be set from the Arduino IDE. Go to Tools -> Clock Speed, and pick one of the 7 options.  After selecting the required clock speed, go to Tools -> Burn Bootloader. Every time you change a clock, make sure you burn the bootloader - otherwise it is not applied, and timing will be off.

These are the power characteristics for each clock:

Clock Speed | 20MHz | 16MHz | 10MHz | 8MHz | 5MHz | 4MHz | 1MHz
------------|-----------|-----------|-----------|-----------|-----------|-----------|-----------
Current | 10.2mA | 8mA | 5.5mA | 4.5mA | 3.2mA | 2mA | 0.6mA
Voltage | 4.5V-5.5V | 4.2V-5.5V | 2.7V-5.5V | 2.7V-5.5V | 1.8V-5.5V | 1.8V-5.5V | 1.8V-5.5V

We often get the request to add the 32KHz option to the clock speed menu since it only requires 13.5μA. While it is possible to run the main clock from this low power oscillator, it is not recommended. It will make certain peripherals unusable, and timers used for delay and millis require the faster oscillator to run anyway.

## Disable BOD
Make sure the Brown Out Detection is disabled in both modes (BOD Mode active and BOD Mode sleep) and burn the bootloader again.

## Reduce the voltage
There is a significant difference between running the MCU at 5V or just 2V. According to the datasheet of the ATtiny1616 the 5Mhz clock will consume 3.2mA at 5V, but only 1.2mA at 2V.

## Use Sleep Mode
The absolute minimum power consumption of the device, when all the peripherals are stopped, is 0.1μA. This can be achieved in power-down sleep mode; enabling the WDT and/or RTC (to keep track of time) but will continue where it left off after it receives an interrupt. While the CPU is not running, the device itself is still monitoring all the interrupts out there and to do so has to keep the peripherals running. Which peripherals are running depends on the sleep mode setting selected before entering sleep mode. The following 3 options are available:

1.  Idle, only the CPU is turned off. All peripherals continue to run.  This does not save very much power.
1.  Standby, the CPU is turned off as are most Peripherals. All unneeded clock sources are also turned off. Most periphrals can be set to remain active in standby - the specific implications of that may vary depending on the peripheral, and some peripherals can account for a large portion of the power consumption - especially if they keep the main oscillator running. On these parts, standby sleep mode is far more useful than it was on classic AVRs, where it was "Like power-down, only you don't save as much power" . Being able to select which peripherals you leave on, and the wealth of configuration options available for them, has made a world of difference.
1.  Power Down, the CPU is turned off and all Peripherals (except the WDT and RTC) are shut down. Only the PIT (RTC), Pin change and TWI Address Match<sup>1</sup> interrupts can wake up the device. The same mode was available on classic AVRs with similar effects.

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

### Unused pins and Sleep modes
**IMPORTANT** In order to minimize power consumption, you *must* eliminate or disable all floating pins. A floating pin is any pin which is not being driven high or low (by this or another device) nor held in a specific state (for example by pullup resistors). Because these pins transition randomly in response to ambient noise, those digital input buffers are consuming power with every trasition. To minimize power consumption, one or more of the following must apply to every pin:
* It is set as an OUTPUT
* It is set INPUT_PULLUP or the internal pullups are otherwise enabled
* It is connected to another device which is holding it HIGH or LOW
* The input buffer is disabled in the PORTx.PINnCTRL register (a wrapper function around this is planned for a future update)

If those steps are not followed, the power consumption may be orders of magnitude higher than expected in sleep modes! When left as UPDI or when used as RESET, the UPDI pin has an internal pullup enabled and no action is necessary for that pin.

## Using the Real Time Counter (RTC/PIT)
The real time counter is like the WDT in interrupt mode which on a classic AVR. On these new series the WDT can only reset the device. Interrupting at a certain time period is now done with the RTC and PIT (Programmable Interrupt Timer). These are tightly coupled peripherals. On many tinyAVR 0/1-series parts, you cannot turn off one without losing the other (see the applicable sillicon errata).
The RTC can run on the 32KHz low power oscillator and will trigger an interrupt after a certain time period. This period is based on the number of clock cycles. For example, when we have a period of 32768, this will take 1 second to count to by using the 32KHz oscillator (32.768kHz/32768= 1Hz = 1 sec.). After we reach 32768 we will trigger a Periodic Interrupt (PIT). We will be using PIT since that is the only timer interrupt we can use in Power Down mode. In standby sleep mode, the RTC interrupts are available. These are much more flexible - you get both an overflow (OVF) and a compare-match (CMP) - and you can set both the period and compare values to any 16-bit value, whereas the PIT interrupt can only be emabled at powers-of-two RTC clock cycles.

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
The RTC is in it's own "clock domain" and the microcontroller has to "synchronize" configuration changes between the registers controlling the RTC, and the RTC itself. The RTC itself is also - compared to the CPU - SLOW (32.768 kHz vs 20 MHz). While this is not the only peripheral with such synchronization concerns, they are *very* prominant here due to that speed difference. Always check RTC.STATUS before writing to RTC/PIT registers. If you test a sketch that uses these and it doesn't work, one of your first thoughts should be "Is the RTC configured correctly? Do I KNOW that it is? Maybe I'd better make sure by reading the values out of the registers after I think I've set them..." - Generally the consequence of failing to wait for synchronization is that writes to the register in question will be ignored; when the setting in question is what makes the chip wake up after you put it to sleep, and it goes to sleep upon start-up, there may not be much functionality left.

# Sleep and Serial ports
If there are any serial ports which you print output to, before going to sleep, be sure to let them finish printing everything in their transmit buffer by calling `Serial.flush()`.

# Future Development
There are plans for a library to provide improved wrappers around sleep modes, particularly regarding timekeeping. This will also provide a means to handle serial ports automatically - including waking the part upon seeing an incoming character on the serial port. The latter is more complicated than the datasheet implies due to a widespread silicon bug with SFD (Start-of-Frame Detection).

# Notes
1. "Pin change with restrictions? Like always, sure. RTC? But of course, that's what it's here for. TWI address match? Whaaat?" If your first thought is "one of these things looks out of place here" - that was mine too. One presumes that this can be implemented asynchronously at a low cost (it is, effectively, just a shift register and 7-bit binary comparator...). I don't know how difficult of an engineering task it was, or what compromises were made for it, but if you imagine building an I2C device with one of these as it's core, this is functionality has a very high payback.
