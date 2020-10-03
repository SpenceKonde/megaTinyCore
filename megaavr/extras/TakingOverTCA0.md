# Taking over TCA0 for your owm PWM

We have received many questions from users about how to take over TCA0 to generate 16-bit PWM or change the output frequency. There are a few non-obvious complications here, and this page aims to clear this up. First, before you begin, make sure you have megaTinyCore 1.1.6 - in prior versions, the type B timer(s) are clocked from the prescaled clock of TCA0 when they are used as a millis/micros source or for low frequency output with tone() - which meant that reconfiguring TCA0 would break that functionality as well. In 1.1.6, the only thing included with the core that is broken by reconfiguring the TCA0 prescaler is the Servo library, and that is fixed for 1.1.7.

The most common point of confusion is the fact that megaTinyCore, out of the box, configures TCA0 for use in "Split Mode" - this allows it to generate 6 8-bit PWM signals. This provides 2 additional PWM pins (3 if TCB0 is needed for other purposes) - and since analogWrite() only supports 8-bit PWM anyway, when using the Arduino API functions, there is no loss of functionality imposed by this. You must disable the timer when switching between modes, and Microchip recommends that you issue a reset command - this will reset the period, count, and compare registers to their default values. Note that in split mode, the two low bits must also be 1, or the command is ignored. The intent of those bits is unclear, as only 00 (none) and 11 (both) are listed as valid.

```
  TCA0.SPLIT.CTRLA=0; //disable TCA0 and set divider to 1
  TCA0.SPLIT.CTRLESET=TCA_SPLIT_CMD_RESET_gc|0x03; //set CMD to RESET to do a hard reset of TCA0 and enable for both halves.
  TCA0.SPLIT.CTRLD=0; //turn off split mode
```
Note that as these bits have the same function in SINGLE and SPLIT mode, it does not matter whether you reference them as TCA0.SINGLE.* or TCA0.SPLIT.*

Once this has been done, further configuration is straightforward. Failing to turn off split mode when you intend to, however, can result in strange behavior. 

### Avoid using TCA0 as the millis timer
Reconfiguring TCA0 when it is used as the millis timer source will result in loss of timekeeping functionality. When doing this, you should avoid using TCA0 as the millis source. To ensure that you don't forget to set the millis timer correctly, it is suggested to put code like the following in your sketch to halt compile if you later open the sketch and did not choose the correct millis timer source.

```
#ifdef MILLIS_USE_TIMERA0
#error "This sketch takes over TCA0 - please use a different timer for millis"
#endif
```

Better yet, you can verify that you chose the intended millis timer, rather than that you didn't pick the most unsuitable one:

```
#ifndef MILLIS_USE_TIMERB0
#error "This sketch is written for use with TCB0 as the millis timing source"
#endif
```

In these examples, it is also used to make sure one doesn't try to run them on a part where the mappings of the channels to pins are different (ie, the 8-pin parts).

##### Added complication for 8-pin parts
On the 8-pin parts, the default location for WO0 is the same as for WO3: PA3, ie, you can't get an extra channel from split mode. However, WO0 can be moved from it's default location to PA7 via the PORTMUX; megaTinyCore does this to get the extra PWM channel out of the box. This is controlled by `PORTMUX.CTRLC`. Nothing else is controlled by this register, so you can just set it to the compiler-provided constants. None of the other parts supported by megaTinyCore have PWM pins blocking each other like this.

```
PORTMUX.CTRLC = PORTMUX_TCA00_DEFAULT_gc;   // Move it back to PA3
PORTMUX.CTRLC = PORTMUX_TCA00_ALTERNATE_gc; // Move it to PA7

```

# Examples
Now for the fun part - example code! 

A note about the pin numbers - we use the PORT_Pxn notation to refer to pins; when I mention in the comments the pin number, that is an Arduino (logical) pin number, not a physical pin number (generally, this documentation does not refer to physical pin numbers except on the pinout charts). Because the mappings of peripherals to pins by the port and pin within the port is constant across the non-8-pin parts, this means the examples (except the one for 8-pin parts) will all work on all 14, 20, and 28-pin parts.


### Example 1: 16-bit PWM in single mode, dual slope with interrupt.

```
#if defined(MILLIS_USE_TIMERA0)||defined(__AVR_ATtinyxy2__)
#error "This sketch takes over TCA0, don't use for millis here.  Pin mappings on 8-pin parts are different"
#endif

unsigned int DutyCycle=0;

void setup() {
  // We will be outputting PWM on PB0
  pinMode(PIN_PB0, OUTPUT); //PB0 - TCA0 WO0, pin7 on 14-pin parts
  TCA0.SPLIT.CTRLA=0; //disable TCA0 and set divider to 1
  TCA0.SPLIT.CTRLESET=TCA_SPLIT_CMD_RESET_gc|0x03; //set CMD to RESET, and enable on both pins.
  TCA0.SPLIT.CTRLD=0; //Split mode now off, CMPn = 0, CNT = 0, PER = 255
  TCA0.SINGLE.CTRLB=(TCA_SINGLE_CMP0EN_bm|TCA_SINGLE_WGMODE_DSBOTTOM_gc); //Dual slope PWM mode OVF interrupt at BOTTOM, PWM on WO0
  TCA0.SINGLE.PER=0xFFFF; // Count all the way up to 0xFFFF
  // At 20MHz, this gives ~152Hz PWM
  TCA0.SINGLE.CMP0=DutyCycle;
  TCA0.SINGLE.INTCTRL=TCA_SINGLE_OVF_bm; //enable overflow interrupt
  TCA0.SINGLE.CTRLA=TCA_SINGLE_ENABLE_bm; //enable the timer with no prescaler
}

void loop() { // Not even going to do anything in here
}

ISR(TCA0_OVF_vect) { //on overflow, we will increment TCA0.CMP0, this will happen after every full cycle - a little over 7 minutes.
  TCA0.SINGLE.CMP0=DutyCycle++; // Because we are in Dual Slope Bottom mode, OVF fires at BOTTOM, at end, not TOP, in middle of the pulse.
  TCA0.SINGLE.INTFLAGS=TCA_SINGLE_OVF_bm; //Always remember to clear the interrupt flags, otherwise the interrupt will fire continually!
}
```


### Example 2: Variable frequency and duty cycle PWM
This generates PWM similar to above (though without the silly interrupt to change the duty cycle), but takes it a step further with two functions to set the duty cycle and frequency. 

```
#if defined(MILLIS_USE_TIMERA0)||defined(__AVR_ATtinyxy2__)
#error "This sketch takes over TCA0, don't use for millis here.  Pin mappings on 8-pin parts are different"
#endif

unsigned int Period=0xFFFF;

void setup() {
  // We will be outputting PWM on PB0
  pinMode(PIN_PB0, OUTPUT); //PB0 - TCA0 WO0, pin7 on 14-pin parts
  TCA0.SPLIT.CTRLA=0; //disable TCA0 and set divider to 1
  TCA0.SPLIT.CTRLESET=TCA_SPLIT_CMD_RESET_gc|0x03; //set CMD to RESET, and enable on both pins.
  TCA0.SPLIT.CTRLD=0; //Split mode now off, CMPn = 0, CNT = 0, PER = 255
  TCA0.SINGLE.CTRLB=(TCA_SINGLE_CMP0EN_bm|TCA_SINGLE_WGMODE_SINGLESLOPE_gc); //Single slope PWM mode, PWM on WO0
  TCA0.SINGLE.PER=Period; // Count all the way up to 0xFFFF
  // At 20MHz, this gives ~305Hz PWM
  TCA0.SINGLE.CMP0=0;
  TCA0.SINGLE.CTRLA=TCA_SINGLE_ENABLE_bm; //enable the timer with no prescaler
}

void loop() { // Not even going to do anything in here
  PWMDemo(150000);//150kHz
  PWMDemo(70000);//70kHz
  PWMDemo(15000);//15kHz
  PWMDemo(3000);//3kHz
  PWMDemo(120);//120Hz
  PWMDemo(35);//35Hz
  PWMDemo(13);//13Hz
}

void PWMDemo(unsigned long frequency){
  setFrequency(frequency);
  setDutyCycle(64); //~25%
  delay(4000);
  setDutyCycle(128); //~50%
  delay(4000);
  setDutyCycle(192); //~75%
  delay(4000);
}

void setDutyCycle(byte duty) {
  TCA0.SINGLE.CMP0=map(duty,0,255,0,Period);
}

void setFrequency(unsigned long freqInHz) {
  unsigned long tempperiod=(F_CPU/freqInHz);
  byte presc=0;
  while (tempperiod>65536 && presc<7) {
    presc++;
    tempperiod=tempperiod>>(presc>4?2:1);
  }
  Period=tempperiod;
  TCA0.SINGLE.CTRLA=(presc<<1)|TCA_SINGLE_ENABLE_bm;
  TCA0.SINGLE.PER=Period;
}
```

### Example 3: High speed 8-bit PWM on PA3 on 8-pin part
A user requested (#152) high speed PWM on PA3 on an 8-pin part. They wanted split mode disabled, and PWM frequency higher than 62KHz. This is indeed possible - though do note that the maximum frequency of PWM possible with a full 8 bits of resolution and 20MHz system clock is 78.125 kHz (20000000/256) - and the next higher frequency for which perfect 8-bit resolution is possible is half that, 39.061 kHz. Higher fequencies require lower resolution (see above example for one approach, which can also be used for intermediate frequencies) though if the frequency is constant, varying your input between 0 and the period instead of using map() is desirable, as map may not be smooth. As a further aside, if 78.125kHz is suitable, there is no need to disable split mode....

```
#if defined(MILLIS_USE_TIMERA0)||!defined(__AVR_ATtinyxy2__)
#error "This sketch is for an 8-pin part and takes over TCA0"
#endif


void setup() {
  // We will be outputting PWM on PA3 on an 8-pin part
  pinMode(PIN_PA3, OUTPUT); //PA3 - TCA0 WO0, pin 4 on 8-pin parts
  PORTMUX.CTRLC=PORTMUX_TCA00_DEFAULT_gc; //turn off PORTMUX, returning WO0 to PA3
  TCA0.SPLIT.CTRLA = 0; //disable TCA0 and set divider to 1
  TCA0.SPLIT.CTRLESET=TCA_SPLIT_CMD_RESET_gc|0x03; //set CMD to RESET, and enable on both pins.
  TCA0.SPLIT.CTRLD=0; //Split mode now off, CMPn = 0, CNT = 0, PER = 255
  TCA0.SINGLE.CTRLB = (TCA_SINGLE_CMP0EN_bm | TCA_SINGLE_WGMODE_SINGLESLOPE_gc); //Single slope PWM mode, PWM on WO0
  TCA0.SINGLE.PER = 0x00FF; // Count all the way up to 0x00FF (255) - 8-bit PWM
  // At 20MHz, this gives ~78.125kHz PWM
  TCA0.SINGLE.CMP0 = 0;
  TCA0.SINGLE.CTRLA = TCA_SINGLE_ENABLE_bm; //enable the timer with no prescaler
}

void loop() { //Lets generate some output just to prove it works
  static byte pass = 0;
  static unsigned int duty = 255;
  TCA0.SINGLE.CMP0 = duty-- ; //step down the duty cycle each iteration through loop;
  delay(100);  //so we can see the duty cycle changing over time on the scope/with an LED
  if (!duty) {
    if (pass == 0) {
      // After the first pass, lets go up to 100kHz
      pass = 1;
      duty = 199;
      TCA0.SINGLE.PER = 199;
    } else if (pass==1) {
      //and now the requested 62 kHz (actually 62.11kHz)
      pass=2;
      duty = 322;
      TCA0.SINGLE.PER = 322;
    } else { // and back to the beginning.
      pass = 0;
      duty = 255;
      TCA0.SINGLE.PER = 255;
    }
  }
}
```

### Example 4: Quick bit of fun with split mode
A quick example of how cool split mode can be - You can get two different PWM frequencies out of the same timer. Split mode only has one mode - both halves of the timer independently count down.

```
#if defined(MILLIS_USE_TIMERA0)||defined(__AVR_ATtinyxy2__)
#error "This sketch takes over TCA0, don't use for millis here.  Pin mappings on 8-pin parts are different"
#endif


void setup() {
  // We will be outputting PWM on PB0 amd PA5
  // No need to enable split mode - core has already done that for us.
  pinMode(PIN_PB0, OUTPUT); //PB0 - TCA0 WO0, pin7 on 14-pin parts
  pinMode(PIN_PA5, OUTPUT); //PA5 - TCA0 WO5, pin1 on 14-pin parts
  TCA0.SPLIT.CTRLB=TCA_SPLIT_LCMP0EN_bm|TCA_SPLIT_HCMP2EN_bm; //PWM on WO5, WO0
  TCA0.SPLIT.LPER=0xFF; // Count all the way down from 255 on WO0/WO1/WO2
  TCA0.SPLIT.HPER=200;  // Count down from only 200 on WO3/WO4/WO5
  TCA0.SPLIT.LCMP0=0x7F; //50% duty cycle
  TCA0.SPLIT.HCMP2=150;  //75% duty cycle
  TCA0.SPLIT.CTRLA=TCA_SPLIT_CLKSEL_DIV16_gc|TCA_SPLIT_ENABLE_bm; //enable the timer with prescaler of 16
}
void loop() { //nothing to do here but enjoy your PWM. 
//Prescaler of 16 and LPER and HPER values give 4.88 kHz on PB0 and 6.25kHz on PA5.   
}
```



