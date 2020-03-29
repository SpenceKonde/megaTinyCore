# PWM and Timer usage
This document describes how the timers are configured by the core prior to the sketch starting and/or by the built-in peripherals, and how this may impact users who wish to take full control of these peripherals.

## The Timers on megaAVR parts
This applies to the megaAVR ATtiny parts, or as Microchip calls them the "tinyAVR 0-series and 1-series" (these are all parts supported by megaTinyCore), and the ATmega4809 and it's smaller brethren, the "megaAVR 0-series". The ATmega parts are used on the Arduino Nano Every, Arduino Uno WiFi Rev. 2, and the whole family is supported as bare chips by [Hans/MCUdude](https://github.com/MCUdude)'s excellent [MegaCoreX](https://github.com/MCUdude/MegaCoreX). The ATmega 4809 is particularly notable for being the only megaAVR part available in a DIP package (excepting the 128AVRxxDA series, which will begin shipping in the second quarter of 2020 - some of those are available in a 28-pin DIP - but there is no compiler support for them on Arduino yet, and their peripherals are slightly different). Not all parts have all peripherals, and the number available will vary depending on the part - see the part-specific megaTinyCore documentation pages for a list, or the relevant datasheets for excruciating detail.

### TCA0 - Type A 16-bit Timer with 3/6 PWM channels
This timer is the crown jewel of the megavr product line, as far as timers go. It can be operated in two very different modes. The default mode on startup is "Normal" or "Single" mode - it acts as a single 16-bit timer with 3 output compare channels. It can count in either direction, and can also be used as an event counter (ie, effectively "clocked" off the event) s capable of counting up or down, generating PWM in single and dual slope modes, and has 7-output prescaler. For most use cases, this timer is on the same level as tjhe classiv avr 16-biy Timer - the newly added features aren't ones that are particularly relevant for Arduino users. Except for Split Mode. 

The Type A timer can be also be set to split mode to get analogWrite() on 6 8-bit PWM channels. In split mode the high and low bytes of the timer count TCA0.SINGLE.CNT register becomes TCA.SPLIT.LCNT and HCNT; likewise the period and compare registers. The frequency that they count at is fixed, but the periods can be adjusted independently - one could be generating 


### TCBn - Type B 16-bit Timer
The type B timer is what I would describe as a "utility timer". The have only one multipurpose interrupt vector (as opposed to TCA0 which has 3 compare and one overflow vector, and TCD 

### TCD0 - Type D 12-bit Async Timer
The Type D timer, only present on the 1-series parts, is a very strange timer indeed. It can run from a totally separate clock supplied on EXTCLK, or from the unprescaled 20MHz internal oscillator.It was apparently designed with a particular eye towards motor control and SMPS control applications. This makes it very nice for those sorts of use cases, but in a variety of ways,these get in the way of using it for the sort of things that people who would be using the Arduino IDE typical arduino-timer purposes. First, none of the control registers can be changed while it is running; it must be briefly stopped, the register changed, and the timer restarted. In addition, the transition between stopping and starting the timer is not instant due to the synchronization proces. This is fast (it looks to me to be about 2 x the synchronizer prescaler 1-8x Synchronizer-prescaler, in clock cycless. The same thing applies to reading the value of the counter - you have to request a capture by writing the SCAPTUREx bit of TCD0.CTRLE, and wait a sync-delay for it. can *also* be clocked from the unprescaled 20 MHz (or 16 MHz) internal oscillator, even if the main CPU is running more slowly. - though it also has it's own prescaler - actually, two of them - a "synchronizer" clock that can then be further prescaled for the timer itself. It supports normal PWM (what they call one-ramp mode) and dual slope mode without that much weirdness. But the other modes are quire clearly made for driving motors and switching power supplies. The asychronous nature of this timer, however, comes at a great cost. Most changes to settings require it to be disabled - and you need to wait for it to actually disable before you can m



### RTC - 16-bit Real Time Clock and Programmable Interrupt Timer

## Timer Prescaler Availability

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

## Resolution, Frequency and Period
When working with timers, I constantly found myself calculating periods, resolution, frequency and so on for timers at the common prescaler settings. While that is great for adhoc calculations, I felt it was worth some time to make a nice looking chart that showed those figures at a glance. The numbers shown are the resolution (when using it for timing), the frequency (at maximum range), and the period (at maximum range - ie, the most time you can measure without accounting for overflows).
### [In Google Sheets](https://docs.google.com/spreadsheets/d/10Id8DYLRtlp01KA7vvslC3cHaR4S2a1TrH7u6pHXMNY/edit?usp=sharing)

## Usage of Timers by megaTinyCore
This section applies only to megaTinyCore.


### PWM ( analogWrite() )
In it's stock configuration, regardless of which timer 
Prior to 1.1.9, TCA0 was always configured with the CLK/64 prescaler. As of 1.1.9, it is set to 16 at 4 or 5 MHz, and 8 at 1 MHz. in order to get more acceptable millis/micros resolution and flicker-free PWM.

When it is used for PWM, TCD0 is clocked directly from 20MHz/16MHz oscillator, in dual ramp mode.

### Millis/Micros Timekeeping
megaTinyCore allows all of the above listed timers to be selected as the clock source for timekeeping via the standard millis timekeeping functions.

#### TCA0 for millis timekeeping
This is the default option on all 0-series parts, as well as on 20 and 24-pin parts, because 

### Tone
The tone() function included with megaTinyCore uses one Type B timer. It defaults to using TCB0 unless that timer is selected for Millis timekeeping. In that case, it will use TCB1 if available. Tone is not compatible with any sketch that needs to take over these timers - if possible, use a different timer for your other needs. Servo and tone() can only be used together on parts with both TCB0 and TCB1. Regardless of which timer it uses, it will use CLK_PER or CLK_PER/2 as it's clock source - the TCA clock will never be used, so it does not care if you change the TCA0 prescaler (unlike the official megaAVR core).

Tone works the same was as the normal tone() function on official Arduino boards. Unlike the official megaAVR board package's tone function, it can be used to generate arbitrarily low frequency tones (as low as 1 Hz). If the period between required toggling's of the pin is greater than the maximum timer period possible, it will calculate how many cycles it has to wait through between switching the pins in order to achieve the desired frequency. 

It can only generate a tone on one pin at a time. 

All tone generation is done via interrupts. The hardware output compare functionality is not used for generating tones. 

### Servo Library
The Servo library included with this core uses one Type B timer. It defaults to using TCB1 if available, unless that timer is selected for Millis timekeeping. Otherwise, it will use TCB0. The Servo library is not compatible with any sketch that needs to take over these timers - if possible, use a different timer for your other needs. Servo and tone() can only be used together on parts with both TCB0 and TCB1.

Regardless of which type B timer it uses, Servo configures that timer in Periodic Interrupt mode (CNTMODE=0) mode with CLK_PER/2 or CLK_PER as the clock source, so there is no dependence on the TCA prescaler. The timer's interrupt vector is used, and it's period is constantly adjusted as needed to generate the requested pulse lengths. In 1.1.9 and later, CLK_PER is used if the system clock is below 10MHz to generate smoother output and improve performance at low clock speeds.

The above also applies to the Servo_megaTinyCore library; it is an exact copy except for the name. If you have installed a version of Servo via Library Manager or by manually placing it in your sketchbook/libraries folder, the IDE will use that in preference to the one supplied with this core. Unfortunately, that version is not compatible with the tinyAVR parts. Include Servo_megaTinyCore.h instead in this case. No changes to your code are needed other than the name of the library you include.

### megaTinySleep Library
Coming in 1.1.9

