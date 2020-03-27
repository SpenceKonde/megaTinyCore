# PWM and Timer usage
This document describes how the timers are configured by the core prior to the sketch starting and/or by the built-in peripherals, and how this may impact users who wish to take full control of these peripherals.

## The Timers on megaAVR parts
This applies to the megaAVR ATtiny parts, or as Microchip calls them the "tinyAVR 0-series and 1-series" (these are all parts supported by megaTinyCore), and the ATmega4809 and it's smaller brethren, the "megaAVR 0-series". The ATmega parts are used on the Arduino Nano Every, Arduino Uno WiFi Rev. 2, and the whole family is supported as bare chips by [Hans/MCUdude](https://github.com/MCUdude)'s excellent [MegaCoreX](https://github.com/MCUdude/MegaCoreX). The ATmega 4809 is particularly notable for being the only megaAVR part available in a DIP package (excepting the 128AVRxxDA series, which will begin shipping in the second quarter of 2020 - some of those are available in a 28-pin DIP - but there is no compiler support for them on Arduino yet, and their peripherals are slightly different). Not all parts have all peripherals, and the number available will vary depending on the part - see the part-specific megaTinyCore documentation pages for a list, or the relevant datasheets for excruciating detail.

### TCA0 - Type A 16-bit Timer with 3/6 PWM channels

### TCBn - Type B 16-bit Timer

### TCB0 - Type D 12-bit Timer

### RTC - 16-bit Real Time Clock and Programmable Interrupt Timer

Prescaler    | TCA0   | TCBn  | TCD0   | TCD0 sync | TD0 counter|
------------ | -------|-------| -------| -------| -------| 
CLK          |  YES   |  YES  |  YES   |  YES   |  YES   |
CLK2         |  YES   |  YES  |  YES*  |  YES   |  NO    |
CLK/4        |  YES   |  TCA  |  YES   |  YES   |  YES   |
CLK/8        |  YES   |  TCA  |  YES   |  YES   |  NO    |
CLK/16       |  YES   |  TCA  |  YES*  |  NO    |  NO    |
CLK/32       |  NO    |  NO   |  YES   |  NO    |  YES   |
CLK/64       |  YES   |  TCA  |  YES*  |  NO    |  NO    |
CLK/128      |  NO    |  NO   |  YES*  |  NO    |  NO    |
CLK/256      |  YES   |  TCA  |  YES*  |  NO    |  NO    |
CLK/1024     |  YES   |  TCA  |  NO    |  NO    |  NO    |


## Usage of Timers by megaTinyCore
This section applies only to megaTinyCore.

### Millis/Micros Timekeeping

### Tone
The tone() function included with megaTinyCore uses one Type B timer. It defaults to using TCB0 unless that timer is selected for Millis timekeeping. In that case, it will use TCB1 if available. Tone is not compatible with any sketch that needs to take over these timers - if possible, use a different timer for your other needs. Servo and tone() can only be used together on parts with both TCB0 and TCB1. Regardless of which timer it uses, it will use CLK_PER or CLK_PER/2 as it's clock source - the TCA clock will never be used, so it does not care if you change the TCA0 prescaler (unlike the official megaAVR core).

Tone works the same was as the normal tone() function on official Arduino boards. Unlike the official megaAVR board package's tone function, it can be used to generate arbitrarily low frequency tones (as low as 1 Hz). If the period between required toggling's of the pin is greater than the maximum timer period possible, it will calculate how many cycles it has to wait through between switching the pins in order to achieve the desired frequency. 

It can only generate a tone on one pin at a time. 

All tone generation is done via interrupts. The hardware output compare functionality is not used for generating tones. 

### Servo Library
The Servo library included with this core uses one Type B timer. It defaults to using TCB1 if available, unless that timer is selected for Millis timekeeping. Otherwise, it will use TCB0. The Servo library is not compatible with any sketch that needs to take over these timers - if possible, use a different timer for your other needs. Servo and tone() can only be used together on parts with both TCB0 and TCB1.

Regardless of which type B timer it uses, Servo configures that timer in Periodic Interrupt mode (CNTMODE=0) mode with CLK_PER/2 or CLK_PER as the clock source, so there is no dependence on the TCA prescaler. The timer's interrupt vector is used, and it's period is constantly adjusted as needed to generate the requested pulse lengths. In 1.1.9 and later, CLK_PER is used if the system clock is below 10MHz to generate smoother output and improve performance at low clock speeds.

The above also applies to the Servo_megaTinyCore library; it is an exact copy except for the name. If you have installed a version of Servo via Library Manager or by manually placing it in your sketchbook/libraries folder, the IDE will use that in preference to the one supplied with this core. Unfortunately, that version is not compatible with the tinyAVR parts. Include Servo_megaTinyCore.h instead in this case. No changes to your code are needed other than the name of the library you include. 

### PWM ( analogWrite() )


### megaTinySleep Library
Coming in 1.1.9

