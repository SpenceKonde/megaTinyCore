# Reconfiguring a Type A timer (like TCA0)

We have received many questions from users about how to take over TCA0 to generate 16-bit PWM or change the output frequency. There are a few non-obvious complications here, and this document aims to clear up these confusions. First, before you begin, make sure you have a recemt version of megaTinyCore installed. This document references functions added in the 2.3.x versions; pre-2.x versions further contain significant differences that render much of this document inapplicable. What is written here largely applies to TCA1 on the AVR Dx-series parts, and is expected to apply to the AVR EA-series as well, though no definitive information on those parts beyond their mention in the AVR EA-series technical brief has been released. It it unlikely that any relevant changes will be madem but not impossible.

## For far more information on type A timers
Microchip has made available a Technical Brief describing the capabilities of the Type A timer, which is arguably the most powerful timer available on modern AVR devices (one could argue that the type D timer on the AVR DA, DB, and DD-series parts is more powerful (the same case is harder to make in the case of the tinyAVR 1-series, where there is no PLL to clock it from). I tend to disagree with that; while it is unquestionably more complex and is capable of some tasks which the TCA is not, it is applicable to significantly fewer use cases, and far more challenging to configure). That technical brief describes all applications of the type A timer, not just PWM (and also assumes it is starting from power on reset (POR) state; if you are using code from there, you can call takeOverTCA0() to both reset it to this state, and ensure that the core does not subsequently mess with it).

[Technical Brief 3217: Getting started with TCA](https://www.microchip.com/en-us/application-notes/tb3217)

## Reconfiguring TCA0 while using megaTinyCore
The most common point of confusion is the fact that megaTinyCore, out of the box, configures TCA0 for use in "Split Mode" - this allows it to generate 6 8-bit PWM signals. This provides 2 additional PWM pins (3 if TCB0 is needed for other purposes) - and since analogWrite() only supports 8-bit PWM anyway, when using the Arduino API functions, there is no loss of functionality imposed by this.
However, for users wishing to take over the timer, it has caused confusion, because it changes the behavior of the compare and period registers significantly (they are treated as 8-bit values instead of 16-bit ones), and TCA0.CTRLD which controls split mode is enable-locked. In order to help resolve this issue and make it easier to make use of the advanced functionality of the timer, we provide the `takeOverTCA0()` function. That will disable all of the core-mediated API functionality that interacts with the timer, so that digitalWrite (and analogWrite) will not reconfigure the timer, which could result in PWM on a different pin being turned off (in the vase of digitalWrite - you should not call analogWrite() on a pin that is controlled by TCA0 anyway after taking over TCA0. Simply calling `takeOverTCA0()` will perform a hard reset of the timer in addition to instructing analogWrite() and digitalWrite() not to configure that timer (digitalPinToTimerNow() will return NOT_ON_TIMER for TCA0 pins).

### Fully manual reconfiguration
You must disable the timer when switching split mode and non-split mode, and Microchip recommends that you issue a reset command. That's what we do here. This will reset timer registers to their default (not our default) values. Note that in split mode, the two low bits must be 1, or the command is ignored. Only 00 (none) and 11 (both) are listed as valid; it sounds like there had originally been a plan to allow the RESTART command to be issued independently to the two halves (and possibly a third command) but that it either was never implemented, or never worked correctly and was discovered early enough and dropped instead of adding another issue that would go unfixed for 5+ years to the errata.
```c++
  TCA0.SPLIT.CTRLA=0; //disable TCA0 and set divider to 1
  TCA0.SPLIT.CTRLESET=TCA_SPLIT_CMD_RESET_gc|0x03; //set CMD to RESET to do a hard reset of TCA0.
```
Note that as these bits have the same function in SINGLE and SPLIT mode, it does not matter whether you reference them as TCA0.SINGLE.* or TCA0.SPLIT.*

Once this has been done, further configuration is straightforward. Failing to turn off split mode when you intend to, however, will result in strange behavior. Among other things, that determines whether the 16-bit registers are treated as 16-bit registers or pairs of 8-bit registers.

## Avoid using TCA0 as the millis timer
Reconfiguring TCA0 when it is used as the millis timer source will result in loss of timekeeping functionality. When doing this, you should avoid using TCA0 as the millis source. To ensure that you don't forget to set the millis timer correctly, it is suggested to put code like the following in your sketch to halt compile if you later open the sketch and did not choose the correct millis timer source.
```c++
#ifdef MILLIS_USE_TIMERA0
  #error "This sketch takes over TCA0 - please use a different timer for millis"
#endif
```

Better yet, you can verify that you chose the intended millis timer, rather than that you didn't pick the most unsuitable one:
```c++
#ifndef MILLIS_USE_TIMERB0
  #error "This sketch is written for use with TCB0 as the millis timing source"
#endif
```

In these examples, it is also used to make sure one doesn't try to run them on a part where the mappings of the channels to pins are different (ie, the 8-pin parts).

### Added complication for 8-pin parts
On the 8-pin parts, the default location for WO0 is the same as for WO3: PA3, ie, you can't get an extra channel from split mode with the default pin assignments. However, WO0 can be moved from it's default location to PA7 via the PORTMUX; megaTinyCore does this to get the extra PWM channel. This is controlled by `PORTMUX.CTRLC`. None of the other parts supported by megaTinyCore have PWM pins on TCA0 blocking each other like this. Obviously, megaTinyCore configures this for PA7 on startup to get the extra PWM pin.

```c++
PORTMUX.CTRLC = PORTMUX_TCA00_DEFAULT_gc;   // Move it back to PA3
PORTMUX.CTRLC = PORTMUX_TCA00_ALTERNATE_gc; // Move it to PA7


```

## Examples
Now for the fun part - example code!

A note about the pin numbers - we use the PORT_Pxn notation to refer to pins; when I mention in the comments the pin number, that is an Arduino (logical) pin number, not a physical pin number (generally, this documentation does not refer to physical pin numbers except on the pinout charts). Because the mappings of peripherals to pins by the port and pin within the port is constant across the non-8-pin parts, this means the examples (except the one for 8-pin parts) will all work on all 14, 20, and 24-pin parts - and neither the mapping nor timer has changed between the 0-series and 2-series (okay, the timer's EVCTRL register was expanded on the 2-series and it can take a second event now and it can use all event generators because the weird event system of the 0/1-series was replaced with a normal one, but that's way beyond the scope of this document).


### Example 1: 16-bit PWM in single mode, dual slope with interrupt
```c++
#if defined(MILLIS_USE_TIMERA0) || defined(__AVR_ATtinyxy2__)
  #error "This sketch takes over TCA0, don't use for millis here.  Pin mappings on 8-pin parts are different"
#endif

unsigned int DutyCycle = 0;

void setup() {
  // We will be outputting PWM on PB0
  pinMode(PIN_PB0, OUTPUT); //PB0 - TCA0 WO0, pin7 on 14-pin parts
  takeOverTCA0();                           // This replaces disabling and resettng the timer, required previously.
  TCA0.SINGLE.CTRLB = (TCA_SINGLE_CMP0EN_bm | TCA_SINGLE_WGMODE_DSBOTTOM_gc); //Dual slope PWM mode OVF interrupt at BOTTOM, PWM on WO0
  TCA0.SINGLE.PER = 0xFFFF; // Count all the way up to 0xFFFF
  // At 20MHz, this gives ~152Hz PWM
  TCA0.SINGLE.CMP0 = DutyCycle;
  TCA0.SINGLE.INTCTRL = TCA_SINGLE_OVF_bm; //enable overflow interrupt
  TCA0.SINGLE.CTRLA = TCA_SINGLE_ENABLE_bm; //enable the timer with no prescaler
}

void loop() { // Not even going to do anything in here
}

ISR(TCA0_OVF_vect) { //on overflow, we will increment TCA0.CMP0, this will happen after every full cycle - a little over 7 minutes.
  TCA0.SINGLE.CMP0 = DutyCycle++; // Because we are in Dual Slope Bottom mode, OVF fires at BOTTOM, at end, not TOP, in middle of the pulse.
  TCA0.SINGLE.INTFLAGS = TCA_SINGLE_OVF_bm; //Always remember to clear the interrupt flags, otherwise the interrupt will fire continually!
}
```


### Example 2: Variable frequency and duty cycle PWM
This generates PWM similar to above (though without the silly interrupt to change the duty cycle), but takes it a step further with two functions to set the duty cycle and frequency.
```c++
#if defined(MILLIS_USE_TIMERA0)||defined(__AVR_ATtinyxy2__)
  #error "This sketch takes over TCA0, don't use for millis here.  Pin mappings on 8-pin parts are different"
#endif

unsigned int Period=0xFFFF;

void setup() {                // We will be outputting PWM on PB0
  pinMode(PIN_PB0, OUTPUT);   // PB0 - TCA0 WO0, pin7 on 14-pin parts
  takeOverTCA0();             // This replaces disabling and resettng the timer, required previously.
  TCA0.SINGLE.CTRLB = (TCA_SINGLE_CMP0EN_bm | TCA_SINGLE_WGMODE_SINGLESLOPE_gc);
  TCA0.SINGLE.PER   = Period; // Count all the way up to 0xFFFF
  //                             At 20MHz, this gives ~305Hz PWM
  TCA0.SINGLE.CMP0  = 0;
  TCA0.SINGLE.CTRLA = TCA_SINGLE_ENABLE_bm; //enable the timer with no prescaler
}

void loop() {
  PWMDemo(150000);  // 150kHz
  PWMDemo(70000);   // 70kHz
  PWMDemo(15000);   // 15kHz
  PWMDemo(3000);    // 3kHz
  PWMDemo(120);     // 120Hz
  PWMDemo(35);      // 35Hz
  PWMDemo(13);      // 13Hz
}

void PWMDemo(unsigned long frequency){
  setFrequency(frequency);
  setDutyCycle(64); // ~25%
  delay(4000);
  setDutyCycle(128);// ~50%
  delay(4000);
  setDutyCycle(192);// ~75%
  delay(4000);
}

void setDutyCycle(byte duty) {
  TCA0.SINGLE.CMP0 = map(duty, 0, 255, 0, Period);
}

void setFrequency(unsigned long freqInHz) {
  unsigned long tempperiod = (F_CPU / freqInHz);
  byte presc = 0;
  while (tempperiod > 65536 && presc < 7) {
    presc++;
    tempperiod      = tempperiod >> (presc > 4 ? 2 : 1);
  }
  Period            = tempperiod;
  TCA0.SINGLE.CTRLA = (presc << 1) | TCA_SINGLE_ENABLE_bm;
  TCA0.SINGLE.PER   = Period;
}
```

### Example 3: High speed 8-bit PWM on PA3 on 8-pin part
A user requested (#152) high speed PWM on PA3 on an 8-pin part. They wanted split mode disabled, and PWM frequency higher than 62KHz. This is indeed possible - though do note that the maximum frequency of PWM possible with a full 8 bits of resolution and 20MHz system clock is 78.125 kHz (20000000/256) - and the next higher frequency for which perfect 8-bit resolution is possible is half that, 39.061 kHz. Higher fequencies require lower resolution (see above example for one approach, which can also be used for intermediate frequencies) though if the frequency is constant, varying your input between 0 and the period instead of using map() is desirable, as map may not be smooth. As a further aside, if 78.125kHz is suitable, there is no need to disable split mode....

```c++
#if defined(MILLIS_USE_TIMERA0) || !defined(__AVR_ATtinyxy2__)
  #error "This sketch is for an 8-pin part and takes over TCA0"
#endif


void setup() {
  // We will be outputting PWM on PA3 on an 8-pin part
  pinMode(PIN_PA3, OUTPUT);                     // PA3 - TCA0 WO0, pin 4 on 8-pin parts
  PORTMUX.CTRLC     = PORTMUX_TCA00_DEFAULT_gc; // turn off PORTMUX, returning WO0 to PA3
  takeOverTCA0();                               // this replaces disabling and resettng the timer, required previously.
  TCA0.SINGLE.CTRLB = (TCA_SINGLE_CMP0EN_bm | TCA_SINGLE_WGMODE_SINGLESLOPE_gc); // Single slope PWM mode, PWM on WO0
  TCA0.SINGLE.PER   = 0x00FF;                   // Count all the way up to 0x00FF (255) - 8-bit PWM
  //                                               At 20MHz, this gives ~78.125kHz PWM
  TCA0.SINGLE.CMP0  = 0;
  TCA0.SINGLE.CTRLA = TCA_SINGLE_ENABLE_bm; // enable the timer with no prescaler
}

void loop() { // Lets generate some output just to prove it works
  static byte pass = 0;
  static unsigned int duty  = 255;
  TCA0.SINGLE.CMP0 = duty-- ; // step down the duty cycle each iteration through loop;
  delay(100);                 // so we can see the duty cycle changing over time on the scope/with an LED
  if (!duty) {
    if (pass == 0) {          // After the first pass, lets go up to 100kHz
      pass = 1;
      duty = 199;
      TCA0.SINGLE.PER = 199;
    } else if (pass == 1) {   // and now the requested 62 kHz (actually 62.11kHz)
      pass = 2;
      duty = 322;
      TCA0.SINGLE.PER = 322;
    } else {                  // and back to the beginning.
      pass = 0;
      duty = 255;
      TCA0.SINGLE.PER = 255;
    }
  }
}
```

### Example 4: Quick bit of fun with split mode
A quick example of how cool split mode can be - You can get two different PWM frequencies out of the same timer! Split mode only has one mode - both halves of the timer independently count down.
```c++
#if defined(MILLIS_USE_TIMERA0) || defined(__AVR_ATtinyxy2__)
  #error "This sketch takes over TCA0, don't use for millis here.  Pin mappings on 8-pin parts are different"
#endif


void setup() {
  // We will be outputting PWM on PB0 amd PA5
  // No need to enable split mode - core has already done that for us.
  pinMode(PIN_PB0, OUTPUT); // PB0 - TCA0 WO0, pin7 on 14-pin parts
  pinMode(PIN_PA5, OUTPUT); // PA5 - TCA0 WO5, pin1 on 14-pin parts
  TCA0.SPLIT.CTRLB  = TCA_SPLIT_LCMP0EN_bm|TCA_SPLIT_HCMP2EN_bm; // PWM on WO5, WO0
  TCA0.SPLIT.LPER   = 0xFF; // Count all the way down from 255 on WO0/WO1/WO2
  TCA0.SPLIT.HPER   = 200;  // Count down from only 200 on WO3/WO4/WO5
  TCA0.SPLIT.LCMP0  = 0x7F; // 50% duty cycle
  TCA0.SPLIT.HCMP2  = 150;  // 75% duty cycle
  TCA0.SPLIT.CTRLA  = TCA_SPLIT_CLKSEL_DIV16_gc | TCA_SPLIT_ENABLE_bm; // enable the timer with prescaler of 16
}
void loop() { //nothing to do here but enjoy your PWM.
  //Prescaler of 16 and LPER and HPER values give 4.88 kHz on PB0 and 6.25kHz on PA5.
}
```

## PORTMUX and accessing alternate pins
Unlike DxCore, where the PWM follows the pin mapping around, the combination of individually remappable pins, and the different relationship between bit positions and output channels, combined with the limited resources of these parts, made a comparable functionality too costly to implement here. On megaTinyCore, if you need to change the mapping of the pins, you must take over the timer, set the output pins to OUTPUTs, and write your own function to control the pins. The simplest approach would be something like this crude example. By losing the translation from "any pin number" to "WOn" and pushing the task of keeping the pins straight on the author, the example becomes more readable, if repetitive, and the flash usage isn't bad at all (all the functions are now super simple. The compile size grows by like 160 bytes if you force it to actually render all 6 calls to a function - as written it automatically inlines the calls to save space). As always, it's the lookups and translating logical sequences to memory addresses and bits within registers that burns flash in analogWrite().
```c++
void setup() {
  takeOverTCA0(); // take over TCA0 so digitalWrite() on alt pins won't mess up alternate pin PWM output.
  TCA0.SINGLE.CTRLD = 1;    // Enable split mode.
  TCA0.SPLIT.LPER   = 254;  // or as required by your application. If 0 and 255 turn PWM off, you want to count to 254 not 255.
  TCA0.SPLIT.HPER   = 254;
  TCA0.SPLIT.CTRLA  = TCA_SPLIT_CLKSEL_DIV64_gc | TCA_SPLIT_ENABLE_bm; // same as the core configures by default
  PORTMUX.CTRLC     = PORTMUX_TCA00_ALTERNATE_gc;  /* 0/1-series - You can OR together as many alternate TCA0 groupcodes as needed */
  //PORTMUX.TCAROUTEA = PORTMUX_TCA00_ALT1_gc; /* 2-series has different, less stupid, names. */
  pinMode(PIN_PB3, OUTPUT);
  pinMode(PIN_PB1, OUTPUT);
  pinMode(PIN_PB2, OUTPUT);
  pinMode(PIN_PA3, OUTPUT);
  pinMode(PIN_PA4, OUTPUT);
  pinMode(PIN_PA5, OUTPUT);
  pinMode(PIN_PA7, OUTPUT);
}

void loop() {
  analogWriteWO0(12);
  analogWriteWO1(34);
  analogWriteWO2(56);
  analogWriteWO3(78);
  analogWriteWO4(90);
  analogWriteWO5(200);
}
void analogWriteWO0(uint8_t duty) {
  if (duty == 0) {
    TCA0.SPLIT.CTRLB &= ~TCA_SPLIT_LCMP0EN_bm; // Turn off PWM if passed   0 duty cycle
    /* you probably also want to digitalWrite() or digitalWriteFast() the pin LOW */
  } else if (duty == 255) {
    TCA0.SPLIT.CTRLB &= ~TCA_SPLIT_LCMP0EN_bm; // Turn off PWM if passed 255 duty cycle
    /* you probably also want to digitalWrite() or digitalWriteFast() the pin HIGH */
  } else {
    TCA0.SPLIT.LCMP0  =  duty;                 // Turn set the duty cycle for WO0
    TCA0.SPLIT.CTRLB |=  TCA_SPLIT_LCMP0EN_bm; // Turn on PWM
  }
}

void analogWriteWO1(uint8_t duty) {
  if (duty == 0) {
    TCA0.SPLIT.CTRLB &= ~TCA_SPLIT_LCMP1EN_bm; // Turn off PWM if passed   0 duty cycle
    /* you probably also want to digitalWrite() or digitalWriteFast() the pin LOW */
  } else if (duty == 255) {
    TCA0.SPLIT.CTRLB &= ~TCA_SPLIT_LCMP1EN_bm; // Turn off PWM if passed 255 duty cycle
    /* you probably also want to digitalWrite() or digitalWriteFast() the pin HIGH */
  } else {
    TCA0.SPLIT.LCMP1  =  duty;                 // Turn set the duty cycle for WO1
    TCA0.SPLIT.CTRLB |=  TCA_SPLIT_LCMP1EN_bm; // Turn on PWM
  }
}

void analogWriteWO2(uint8_t duty) {
  if (duty == 0) {
    TCA0.SPLIT.CTRLB &= ~TCA_SPLIT_LCMP2EN_bm; // Turn off PWM if passed   0 duty cycle
    /* you probably also want to digitalWrite() or digitalWriteFast() the pin LOW */
  } else if (duty == 255) {
    TCA0.SPLIT.CTRLB &= ~TCA_SPLIT_LCMP2EN_bm; // Turn off PWM if passed 255 duty cycle
    /* you probably also want to digitalWrite() or digitalWriteFast() the pin HIGH */
  } else {
    TCA0.SPLIT.LCMP2  =  duty;                 // Turn set the duty cycle for WO2
    TCA0.SPLIT.CTRLB |=  TCA_SPLIT_LCMP2EN_bm; // Turn on PWM
  }
}
// For WO3 and up, they use the high bytes of the CMPn registers
void analogWriteWO3(uint8_t duty) {
  if (duty == 0) {
    TCA0.SPLIT.CTRLB &= ~TCA_SPLIT_HCMP0EN_bm; // Turn off PWM if passed   0 duty cycle
    /* you probably also want to digitalWrite() or digitalWriteFast() the pin LOW */
  } else if (duty == 255) {
    TCA0.SPLIT.CTRLB &= ~TCA_SPLIT_HCMP0EN_bm; // Turn off PWM if passed 255 duty cycle
    /* you probably also want to digitalWrite() or digitalWriteFast() the pin HIGH */
  } else {
    TCA0.SPLIT.HCMP0  =  duty;                 // Turn set the duty cycle for WO3
    TCA0.SPLIT.CTRLB |=  TCA_SPLIT_HCMP0EN_bm; // Turn on PWM
  }
}

void analogWriteWO4(uint8_t duty) {
  if (duty == 0) {
    TCA0.SPLIT.CTRLB &= ~TCA_SPLIT_HCMP1EN_bm; // Turn off PWM if passed   0 duty cycle
    /* you probably also want to digitalWrite() or digitalWriteFast() the pin LOW */
  } else if (duty == 255) {
    TCA0.SPLIT.CTRLB &= ~TCA_SPLIT_HCMP1EN_bm; // Turn off PWM if passed 255 duty cycle
    /* you probably also want to digitalWrite() or digitalWriteFast() the pin HIGH */
  } else {
    TCA0.SPLIT.HCMP1  =  duty;                 // Turn set the duty cycle for WO4
    TCA0.SPLIT.CTRLB |=  TCA_SPLIT_HCMP1EN_bm; // Turn on PWM
  }
}

void analogWriteWO5(uint8_t duty) {
  if (duty == 0) {
    TCA0.SPLIT.CTRLB &= ~TCA_SPLIT_HCMP2EN_bm; // Turn off PWM if passed   0 duty cycle
    /* you probably also want to digitalWrite() or digitalWriteFast() the pin LOW */
  } else if (duty == 255) {
    TCA0.SPLIT.CTRLB &= ~TCA_SPLIT_HCMP2EN_bm; // Turn off PWM if passed 255 duty cycle
    /* you probably also want to digitalWrite() or digitalWriteFast() the pin HIGH */
  } else {
    TCA0.SPLIT.HCMP2  =  duty;                 // Turn set the duty cycle for WO5
    TCA0.SPLIT.CTRLB |=  TCA_SPLIT_HCMP2EN_bm; // Turn on PWM
  }
}
```
