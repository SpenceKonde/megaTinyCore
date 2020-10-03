#if defined(MILLIS_USE_TIMERA0)||defined(__AVR_ATtinyxy2__)
#error "This sketch takes over TCA0, don't use for millis here.  Pin mappings on 8-pin parts are different"
#endif


void setup() {
  // We will be outputting PWM on PB0 amd PA5
  pinMode(PIN_PB0, OUTPUT); //PB0 - TCA0 WO0, pin7 on 14-pin parts
  pinMode(PIN_PA5, OUTPUT); //PA5 - TCA0 WO5, pin1 on 14-pin parts
  TCA0.SPLIT.CTRLB=TCA_SPLIT_LCMP0EN_bm|TCA_SPLIT_HCMP2EN_bm; //PWM on WO5, WO0
  TCA0.SPLIT.LPER=0xFF; // Count all the way down from 255 on WO0/WO1/WO2
  TCA0.SPLIT.HPER=200;  // Count down from only 200 on WO3/WO4/WO5
  TCA0.SPLIT.LCMP0=0x7F; //50% duty cycle
  TCA0.SPLIT.HCMP2=150;  //75% duty cycle
  TCA0.SPLIT.CTRLA=TCA_SPLIT_CLKSEL_DIV16_gc|TCA_SPLIT_ENABLE_bm; //enable the timer with prescaler of 16
}
void loop() { //nothing to do here but enjoy your PWM. Prescaler of 16 and LPER and HPER values give 4.88 kHz on PB0 and 6.25kHz on PA5.
}
