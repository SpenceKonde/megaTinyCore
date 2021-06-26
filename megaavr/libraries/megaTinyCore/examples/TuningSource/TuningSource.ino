#if __AVR_ARCH__ >=100
/* FOR MODERN AVRs:
 *  tinyAVR must use PB0, PB1 or PB2 (PB2 is by default a serial pin though. 
 *  In order for these to be good toning sourcesm you should have an external clock chip on them.
 *  Same goes for AVR DA and megaAVR 0-series. 
 *  
 *  AVR DB-series and DD-series parts can use a simple crystal. 
 *  
 *  Your tuning will never be better than the thing you're ruimning it from, so don't use something 
 *  with an internal oscillator!
 */
   #ifndef MEGATINYCORE
    // All modern AVRs that are not tinyAVR
    #define OUTPUT_PIN PIN_PB1 /* This must be a pin that exists, and which is numbered 0 through 2. */
    /* The pin - this uses TCA0 to generate the tuning signal, and hence you can't use it for millis
     *  and need to use it on a pin that can generate 16-bit PWM from it, not split_mode 3-5 won't work. 
     *  In theory this could be extended to more pins with the CCL, but just use a pin one that's valid
     *  to begin with. Every part like this has a PC1 that can output PWM.
     */
  #else /* tinyAVR */
    #define OUTPUT_PIN PIN_B0 /* pins B1 and B2 also work. No other pins are supported. */
  #endif

#else
/* FOR CLASSIC AVRs:
 *  Unxomment which comopare channel you wish to enable (sorry not done by pins here...
 *  default will come out pin 9 on a 328p or 328pb... Hell you can even use an MHET Micronucleus t88.
 *  Anything with a 160bit timer and a crystal or external clock (though for weird options you may have 
 *  to tell it the pins.
 *  
 *  
 */
  #define ENABLEOC1A 1
  #define ENABLEOC1B 0
#endif



#if __AVR_ARCH__ >= 100
void setup(){
  /* Modern AVR */
  #ifndef MEGATINYCORE
    // All modern AVRs that are not tinyAVR
    
    byte port = digitalPinToPort(OUTPUT_PIN);
    PORTMUX.TCAROUTEA=port;
  #endif
  
  pinMode(OUTPUT_PIN,OUTPUT);
  TCA0.SPLIT.CTRLA = 0; //disable TCA0 and set divider to 1
  TCA0.SPLIT.CTRLESET=TCA_SPLIT_CMD_RESET_gc|0x03; //set CMD to RESET, and enable on both pins.
  TCA0.SPLIT.CTRLD=0; //Split mode now off, CMPn = 0, CNT = 0, PER = 255
  TCA0.SINGLE.CTRLB = (TCA_SINGLE_CMP1EN_bm | TCA_SINGLE_WGMODE_SINGLESLOPE_gc); //Single slope PWM mode, PWM on WO0
  #if (F_CPU > 32000000) //Some lunatic has is using a chiop that's way overlcocked as timeing reference!
    TCA0.SINGLE.PER   = (F_CPU/1000) - 1; // 
    switch (digitalPinToBitPosition(OUTPUT_PIN)) {
      case 0:
        TCA0.SINGLE.CMP0  = (F_CPU/2000) - 1; // 50% duty cycle,
        TCA0.SINGLE.CTRLB = (TCA_SINGLE_CMP0EN_bm | TCA_SINGLE_WGMODE_SINGLESLOPE_gc); //Single slope PWM mode, PWM on WO0
        break;
      case 1:
        TCA0.SINGLE.CMP1  = (F_CPU/2000) - 1; // 50% duty cycle,
        TCA0.SINGLE.CTRLB = (TCA_SINGLE_CMP1EN_bm | TCA_SINGLE_WGMODE_SINGLESLOPE_gc); //Single slope PWM mode, PWM on WO0
        break;
      case 2:
        TCA0.SINGLE.CMP2  = (F_CPU/2000) - 1; // 50% duty cycle,
        TCA0.SINGLE.CTRLB = (TCA_SINGLE_CMP2EN_bm | TCA_SINGLE_WGMODE_SINGLESLOPE_gc); //Single slope PWM mode, PWM on WO0
        break;
      default:
        invalidPin();
    }
    TCA0.SINGLE.CTRLA = 0x02|TCA_SINGLE_ENABLE_bm; 
  #else
    TCA0.SINGLE.PER   = (F_CPU/500)  - 1; //
    switch (digitalPinToBitPosition(OUTPUT_PIN)) {
      case 0:
        TCA0.SINGLE.CMP0  = (F_CPU/1000) - 1; // 50% duty cycle,
        TCA0.SINGLE.CTRLB = (TCA_SINGLE_CMP0EN_bm | TCA_SINGLE_WGMODE_SINGLESLOPE_gc); //Single slope PWM mode, PWM on WO0
        break;
      case 1:
        TCA0.SINGLE.CMP1  = (F_CPU/1000) - 1; // 50% duty cycle,
        TCA0.SINGLE.CTRLB = (TCA_SINGLE_CMP1EN_bm | TCA_SINGLE_WGMODE_SINGLESLOPE_gc); //Single slope PWM mode, PWM on WO0
        break;
      case 2:
        TCA0.SINGLE.CMP2  = (F_CPU/1000) - 1; // 50% duty cycle,
        TCA0.SINGLE.CTRLB = (TCA_SINGLE_CMP2EN_bm | TCA_SINGLE_WGMODE_SINGLESLOPE_gc); //Single slope PWM mode, PWM on WO0
        break;
      default:
        invalidPin();
    }
    TCA0.SINGLE.CTRLA = TCA_SINGLE_ENABLE_bm; //enable the timer with no prescaling
  #endif
  #if MILLIS_USE_TIMERA0     
    #error "You must uae a different timer than TCA0 for millis()"
  #endif
  pinMode(LED_BUILTIN,OUTPUT);
}
void invalidPin() {
  while(1) {
    delay(100);
    digitalWrite(LED_BUILTIN,CHANGE);
  }
}
#else 
  /* In this case it's a classic AVR */
  #include "parts.h"
  
  #define ENABLEOC1A 1
  #define ENABLEOC1B 0
  
  // Target pulse length, in us. 
  #define TARGET_OC1 1000
  
  //#define TARGET_OC2 1000
  //#define TARGET_OC3 2000
  //#define TARGET_OC4 2000
  //#define TARGET_OC5 2000
  
  // clocks per microsecond times microseconds times 2 
  #define OC1PERIOD ((TARGET_OC1 * (F_CPU/1000000) * 2))
  
  #define OC1LENGTH ((TARGET_OC1 * (F_CPU/1000000))) 
  #if (OC1PERIOD > 65535) 
    #if (OC1PERIOD & 0x07 != 0)
      #error "Cannot generate accurate 1ms pulses from this timebase"
    #endif
    #define PRESCALE8
  #endif
  #define OC1LENGTH ((TARGET_OC1 * (F_CPU/1000000)))
  #define WGM1 0x0E /* = 14, TOP = ICR1 */
  
  void setup() {
    // put your setup code here, to run once:
    Serial.begin(19200);
    #if defined(PRESCALE8)
      uint16_t Timer1TOP = (OC1PERIOD >> 3) - 1;
      uint16_t TimerOC1A = (OC1LENGTH >> 3) - 1;
      uint16_t TimerOC1B = (OC1LENGTH >> 3) - 1;
      uint8_t cs1=0x02;
    #else
      uint16_t Timer1TOP = (OC1PERIOD) - 1;
      uint16_t TimerOC1A = (OC1LENGTH) - 1;
      uint16_t TimerOC1B = (OC1LENGTH) - 1;
      uint8_t cs1=0x01;
    #endif
    #if ENABLEOC1A
      pinMode(PIN_TIMER_OC1A, OUTPUT);
    #endif
    #if ENABLEOC1B
      pinMode(PIN_TIMER_OC1B, OUTPUT);
    #endif
    TCCR1B = 0; // stop time
    Serial.println(TimerOC1A);
    Serial.println(OCR1A);
    TCCR1A = (ENABLEOC1A << COM1A1)|(ENABLEOC1B << COM1B1) | (WGM1 & 0x03);
    TCCR1B = ((WGM1 & 0x0C) << 1) | cs1;  //restart tme
    ICR1 = Timer1TOP;
    OCR1A = TimerOC1A;
    OCR1B = TimerOC1B;
    pinMode(LED_BUILTIN,OUTPUT);
    Serial.println(OCR1A);
  }
#endif
  void loop() {
    // put your main code here, to run repeatedly:
    delay(1800);
    digitalWrite(LED_BUILTIN,1);
    delay(200);
    digitalWrite(LED_BUILTIN,0);
  }
  
