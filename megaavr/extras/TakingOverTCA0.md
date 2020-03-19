# Taking over TCA0 for your owm PWM

We have received many questions from users about how to take over TCA0 to generate 16-bit PWM or change the output frequency. There are a few non-obvious complications here, and this page aims to clear this up. First, before you begin, make sure you have megaTinyCore 1.1.6 - in prior versions, the type B timer(s) are clocked from the prescaled clock of TCA0 when they are used as a millis/micros source or for low frequency output with tone() - which meant that reconfiguring TCA0 would break that functionality as well. In 1.1.6, the only thing included with the core that is broken by reconfiguring the TCA0 prescaler is the Servo library. It is hoped that this can be rectified in a future release.

The most common point of confusion is the fact that megaTinyCore, out of the box, configures TCA0 for use in "Split Mode" - this allows it to generate 6 8-bit PWM signals. This provides 2 additional PWM pins (3 if TCB0 is needed for other purposes) - and since analogWrite() only supports 8-bit PWM anyway, when using the Arduino API functions, there is no loss of functionality imposed by this. Switching between single and split mode for TCA0 is one of those things on the megaAVR parts where if the correct procedure is not followed, the chip will simply ignore your writes to the register. The timer must be stopped, and should be hard-reset. 

```
  TCA0.CTRLA=0; //disable TCA0 and set divider to 1
  TCA0.CTRLESET=TCA_SINGLE_CMD_RESET_gc; //set CMD to RESET to do a hard reset of TCA0
  // All registers are now in power-on state
```
Once this has been done, further configuration is straightforward

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

### Example: 16=bit PWM in single mode

```
#ifdef MILLIS_USE_TIMERA0
#error "This sketch takes over TCA0 - please use a different timer for millis"
#endif

unsigned int DutyCycle=0;

void setup() {
  // We will be outputting PWM on PB0
  pinMode(PIN_PB0, OUTPUT); //PB0 - TCA0 WO0, pin7 on 14-pin parts
  TCA0.SPLIT.CTRLA=0; //disable TCA0 and set divider to 1
  TCA0.SPLIT.CTRLESET=TCA_SINGLE_CMD_RESET_gc; //set CMD to RESET to do a hard reset of TCA0
  TCA0.SPLIT.CTRLD=0;
  // Now, all registers are reset to their initial values
  TCA0.SINGLE.CTRLB=(TCA_SINGLE_CMP0EN_bm|TCA_SINGLE_WGMODE_SINGLESLOPE_gc); //Single slope PWM mode, PWM on WO0
  TCA0.SINGLE.PER=0xFFFF; // Count all the way up to 0xFFFF
  // At 20MHz, this gives ~305Hz PWM
  TCA0.SINGLE.CMP0=DutyCycle;
  TCA0.SINGLE.INTCTRL=TCA_SINGLE_OVF_bm; //enable overflow interrupt
  TCA0.SINGLE.CTRLA=TCA_SINGLE_ENABLE_bm; //enable the timer with no prescaler
}

void loop() { // Not even going to do anything in here
}

ISR(TCA0_OVF_vect) { //on overflow, we will increment TCA0.CMP0
  TCA0.SINGLE.CMP0=DutyCycle++; // Increment dutycycle each time the timer overflows
  TCA0.SINGLE.INTFLAGS=TCA_SINGLE_OVF_bm;
}
```


### Example: 8-bit PWM at higher frequency in single mode



