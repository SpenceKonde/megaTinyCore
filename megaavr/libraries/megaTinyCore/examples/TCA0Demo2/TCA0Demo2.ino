/* Example 2: Variable frequency and duty cycle PWM
 * https://github.com/SpenceKonde/megaTinyCore/blob/master/megaavr/extras/TakingOverTCA0.md
 *
 * This generates PWM similar to the first example (though without the silly interrupt to change the duty cycle),
 * but takes it a step further and into more practical territory with two functions to set the duty cycle and frequency.
 * Calling those instead of this PWMDemo() function is all you'd need to make use of this.
 * Somewhere I think I have the same functionality implemented for the classic AVR "Timer1" style 16-bit timers.
 */
#if defined(MILLIS_USE_TIMERA0) || defined(__AVR_ATtinyxy2__)
  #error "This sketch takes over TCA0, don't use for millis here.  Pin mappings on 8-pin parts are different"
#endif

unsigned int Period = 0xFFFF;

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

void PWMDemo(unsigned long frequency) {
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
