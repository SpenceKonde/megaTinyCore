# Optimizing for power consumption, using Sleep Mode and the Real Time Counter (RTC)
There are several solutions to reduce the power consumption on the ATtiny. Here are some suggestions to get longer life out of your battery or super capacitor:
## Reducing the clock speed
The clock speed is very important for the following two reasons:

1.  Slower clock speeds will reduce the power consumption
1.  Minimum voltage to keep the CPU runnning is lower with a slower clock speed

The clock speed can be set from the Arduino IDE. Go to Tools -> Clock Speed, and pick on of the 7 options.  After selecting the required clock speed go to Tools -> Burn Bootloader. Every time you change a clock make sure you burn the bootloader, otherwise it is not applied, and timing will be off.  These are the power characteristics for each clock:

Clock Speed |	20MHz	| 16MHz |	10MHz |	8MHz |5MHz | 4MHz |	1MHz
------------|-------|-------|-------|-------|-------|-------|-------
Current | 10.2mA	| 10mA*	| 5.5mA	| 5mA*	| 3.2mA	| 3mA*	| 2mA*
Voltage	| 4.5V-5.5V | 4V-5.5V* | 2.7V-5.5V | 2.7V-5.5V | 1.8V-5.5V | 1.8V-5.5V | 1.8V-5.5V

*not in datasheet, estimated

We often get the request to add the 32KHz option to the clock speed menu since it only requires 13.5μA. While it is possible to run the main clock from this low power oscillator, it is not recommended. It will make certain peripherals unusable, and timers used for delay and millis do require the faster oscillator to run anyway.

## Disable BOD
Make sure the Brown Out Detection is disabled in both modes (BOD Mode active and BOD Mode sleep) and burn the bootloader again.
## Reduce the voltage
There is a significant difference between running the MCU at 5V or just 2V. According to the datasheet of the ATtiny1616 the 5Mhz clock will consume 3.2mA at 5V, but only 1.2mA at 2V. 
## Use Sleep Mode
The absolute minimum power consumption of the device, when all the peripherals are stopped, is 0.1μA. This can be accomplished by using sleep mode when the MCU is not required. When the device is in sleep mode the CPU is not executing any of the application, but will continue where it left off after it receives an interrupt. While the CPU is not running, the device itself is still monitoring all the interrupts out there and to do so has to keep the peripherals running. Which peripherals are running depends on the sleep mode setting selected before entering sleep mode. The following 3 options are available:

1.	Idle, only the CPU is turned off
1.	Standy, the CPU is turned off and most Peripherals
1.	Power Down, the CPU is turned off and all Peripherals (except the WDT and RTC), only the PIT (RTC) , Pin change and TWI Address match can wake up the device.

The sleep functions are not part of the Arduino Core, but part of the AVR-GCC compiler. Add the following line at the beginning of your code to add the sleep mode functions:

```cpp
#include <avr/sleep.h>
```

Before entering sleep mode, the correct setting most be selected with set_sleep_mode():

```cpp
set_sleep_mode(SLEEP_MODE_PWR_DOWN);
```

Other options are: 

```cpp
set_sleep_mode(SLEEP_MODE_IDLE);
set_sleep_mode(SLEEP_MODE_STANDBY);
```

After the sleep mode setting is selected the sleep mode function must be enabled. Enabling sleep mode does not put the device in sleep mode, but it enables the sleep mode function:

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

## Using the Real Time Counter (RTC)
The real time counter is like the WDT in interrupt mode which was available on the older ATtiny’s. On these new series the WDT is only used to warn the user or reset the device. Interrupting at a certain time period is now done with the RTC.
The RTC can run on the 32KHz low power oscillator and will trigger an interrupt after a certain time period. This period is based on the number of clock cycles. For example, when we have a period of 32768, this will take 1 second to count to by using the 32KHz oscillator (32.768kHz/32768= 1Hz = 1 sec.). After we reach 32768 we will trigger an Periodic Interrupt (PIT). We will be using PIT since that is the only timer interrupt we can use in Power Down mode.
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
  RTC.CLKSEL = RTC_CLKSEL_INT32K_gc;    /* 32.768kHz Internal Crystal Oscillator (XOSC32K) */
  
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
