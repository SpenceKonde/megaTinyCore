#include "parts.h"
#include <util/delay.h>
#include <EEPROM.h>
// *INDENT-OFF*  this code is unreadable of you follow astyle's conventions!
#if __AVR_ARCH__ >=100
/* FOR MODERN AVRs:
 *
 *  tinyAVR must use PB0, PB1 or PB2 (PB2 is by default a serial pin though.
 *  In order for these to be good tuning sources you must have an external clock chip on them, or have a watch crystal connected to them and use RTC tuning mode.
 *  Same goes for AVR DA and megaAVR 0-series.
 *
 *  AVR DB-series and DD-series parts can use a simple MHz crystal.
 *
 *  Your tuning will never be better than the thing you're tuning it from, so don't use something
 *  running on an internal oscillator - just grab any pro mini clone which you can see the HC/49 16 mhz crystal on (the tiny ones are often much less accurate
 *  ceramic resonators that are barely better than the modern internal oscillator. )
 */
#ifndef TUNE_OUTPUT_PIN // If not defined in parts.h, need to pick default pin
  #ifndef TUNE_WITH_RTC // First the non-RTC version
    #if !defined(MEGATINYCORE)
      #if (_AVR_PINCOUNT > 14)
        // Except under tight constraints of 14-pin non-tinies, we can always count on their being a PA2.
        #define TUNE_OUTPUT_PIN PIN_PA2
      #elif defined(PIN_PC2) /*AVR DD-series has no PC0, PA2-7, and the only pins nubered 2 or lower in any port
        * are thus PA0, PA1, and PC1 and PC2. The pins on PORTA are needed for the crystal if this is to be used
        * and you actually want to tune to within an accuracy measured in  (tens of) "ppm" instead of  (several) "%" on datasheet specs.
        * So we're using either PC1 or PC2!
        */
        #define TUNE_OUTPUT_PIN PIN_PC2
     /* This must be a pin that exists, and which is numbered 0-2 in the port; we will use TCA0 to
      * generate it and set PORTMUX.TCAROUTEA appropriately.
      * This sketch uses TCA0 to generate the tuning signal, and hence you can't use it for millis
      * Well - you can, and it will still tune parts perfectly well, but only because we don't rely on
      * millis in any way shape or form to get our timing (if we did it'[d be totally fucked_/
      *
      * This sketch does not support using the CCL to redirect these orphaned output channels to usable pins,
      * and this is in fact practically trivial to do without even using the libraries.
      */
    #else /* tinyAVR */
      #define TUNE_OUTPUT_PIN PIN_PB0 /* pins B1 and B2 also work. No other pins are supported. */
    #endif
  #else
    #if (defined(MEGATINYCORE) || !defined(DD_14_PINS))
      #define TUNE_OUTPUT_PIN PIN_PA2
    #else
      #define TUNE_OUTPUT_PIN PIN_PD7
    #endif
  #endif
#else // TUNE_OUTPUT_PIN defined
  #ifndef TUNE_WITH_RTC
    #if !((((defined(PIN_PA0) && TUNE_OUTPUT_PIN == PIN_PA0) || (defined(PIN_PA1) && TUNE_OUTPUT_PIN == PIN_PA1) ||  (defined(PIN_PA2) && TUNE_OUTPUT_PIN == PIN_PA2)  || \
            (defined(PIN_PC0) && TUNE_OUTPUT_PIN == PIN_PC0) || (defined(PIN_PC1) && TUNE_OUTPUT_PIN == PIN_PC1) ||  (defined(PIN_PC2) && TUNE_OUTPUT_PIN == PIN_PC2)  || \
            (defined(PIN_PD1) && TUNE_OUTPUT_PIN == PIN_PD1) || (defined(PIN_PD2) && TUNE_OUTPUT_PIN == PIN_PD2) ||  (defined(PIN_PD0) && TUNE_OUTPUT_PIN == PIN_PD0)  || \
            (defined(PIN_PE0) && TUNE_OUTPUT_PIN == PIN_PE0) || (defined(PIN_PE1) && TUNE_OUTPUT_PIN == PIN_PE1) ||  (defined(PIN_PE2) && TUNE_OUTPUT_PIN == PIN_PE2)  || \
            (defined(PIN_PF0) && TUNE_OUTPUT_PIN == PIN_PF0) || (defined(PIN_PF1) && TUNE_OUTPUT_PIN == PIN_PF1) ||  (defined(PIN_PF2) && TUNE_OUTPUT_PIN == PIN_PF2)  || \
            (defined(PIN_PG0) && TUNE_OUTPUT_PIN == PIN_PG0) || (defined(PIN_PG1) && TUNE_OUTPUT_PIN == PIN_PG1) ||  (defined(PIN_PG2) && TUNE_OUTPUT_PIN == PIN_PG2)) || \
           ((defined(PIN_PB0) && TUNE_OUTPUT_PIN == PIN_PB0) || (defined(PIN_PB1) && TUNE_OUTPUT_PIN == PIN_PB1) ||  (defined(PIN_PB2) && TUNE_OUTPUT_PIN == PIN_PB2)))
      #error "Invalid or unusable custom output pin defined"
    #endif
  #else // TUNE_WITH_RTC
    #if (defined(MEGATINYCORE) && MEGATINYCORE_SERIES < 2)
      #if  (!((defined(PIN_PA2) && TUNE_OUTPUT_PIN == PIN_PA2) || (defined(PIN_PB2) && TUNE_OUTPUT_PIN == PIN_PB2) || (defined(PIN_PC2) && TUNE_OUTPUT_PIN == PIN_PC2)))
        #error "Cannot use RTC derved tuning signal on this pin"
      #endif
    #else
      #if (!((defined(PIN_PA2) && TUNE_OUTPUT_PIN == PIN_PA2) || (defined(PIN_PA7) && TUNE_OUTPUT_PIN == PIN_PA7) || \
             (defined(PIN_PB2) && TUNE_OUTPUT_PIN == PIN_PB2) || (defined(PIN_PB7) && TUNE_OUTPUT_PIN == PIN_PB7) || \
             (defined(PIN_PC2) && TUNE_OUTPUT_PIN == PIN_PC2) || (defined(PIN_PC7) && TUNE_OUTPUT_PIN == PIN_PC7) || \
             (defined(PIN_PD2) && TUNE_OUTPUT_PIN == PIN_PD2) || (defined(PIN_PD7) && TUNE_OUTPUT_PIN == PIN_PD7) || \
             (defined(PIN_PE2) && TUNE_OUTPUT_PIN == PIN_PE2) || (defined(PIN_PE7) && TUNE_OUTPUT_PIN == PIN_PE7) || \
             (defined(PIN_PF2) && TUNE_OUTPUT_PIN == PIN_PF2) || (defined(PIN_PF7) && TUNE_OUTPUT_PIN == PIN_PF7) || \
             (defined(PIN_PG2) && TUNE_OUTPUT_PIN == PIN_PG2) || (defined(PIN_PG7) && TUNE_OUTPUT_PIN == PIN_PG7)))
        #error "cannout use RTC derived tuning signal on this pin."
      #endif
    #endif
  #endif // end check tuning pins
#endif // end tuning pin select or check.
/* FOR CLASSIC AVRs:
 *  Uncomment which comopare channel you wish to enable (sorry not done by pins here...
 *  default will come out pin 9 on a 328p or 328pb... Hell you can even use an MHET Micronucleus t88.
 *  Anything with a 16-bit timer and a crystal or external clock
 */
#if TIMER1_TYPICAL == 87) {
  CONFIGURE_PWM();
  #define ENABLEOC1A 0
  #define ENABLEOC1B 1
#else
  #define ENABLEOC1A 1
  #define ENABLEOC1B 0
#endif



#if __AVR_ARCH__ >= 100
#if (!defined(TUNE_WITH_RTC) && defined(MILLIS_USE_TIMERA0) || (defined(TUNE_WITH_RTC) && defined(MILLIS_USE_TIMERRTC))
  void init_millis() {
    ;// do nothing -
  }
  #warning "If RTC is used to generate the tuning signal, it must not be used for millis, otherwise TCA0 must not be - millis has now been uncleanly disabled and built-in timekeeping cannot be used."
  // ; millis will never advance, delay() may never return, and micros will return wrong values. Use util/delay.h's _delay_ms() and _delay_us() instead.
#endif
void setup() {
  /* Modern AVR */
  #ifndef TUNE_WITH_RTC
    #ifndef MEGATINYCORE
      // All modern AVRs that are not tinyAVR

      byte port = digitalPinToPort(TUNE_OUTPUT_PIN);
      PORTMUX.TCAROUTEA = port;

    #endif
    takeOverTCA0();
    pinMode(TUNE_OUTPUT_PIN, OUTPUT);
    TCA0.SINGLE.CTRLB = (TCA_SINGLE_CMP1EN_bm | TCA_SINGLE_WGMODE_SINGLESLOPE_gc); // Single slope PWM mode, PWM on WO0
    #if (F_CPU > 32000000) // Some lunatic has is using a chip that's way overlcocked as timing reference!
      TCA0.SINGLE.PER   = (F_CPU/1000) - 1; //
      switch (digitalPinToBitPosition(TUNE_OUTPUT_PIN)) {
        case 0:
          TCA0.SINGLE.CMP0  = (F_CPU/2000) - 1; // 50% duty cycle,
          TCA0.SINGLE.CTRLB = (TCA_SINGLE_CMP0EN_bm | TCA_SINGLE_WGMODE_SINGLESLOPE_gc); // Single slope PWM mode, PWM on WO0
          break;
        case 1:
          TCA0.SINGLE.CMP1  = (F_CPU/2000) - 1; // 50% duty cycle,
          TCA0.SINGLE.CTRLB = (TCA_SINGLE_CMP1EN_bm | TCA_SINGLE_WGMODE_SINGLESLOPE_gc); // Single slope PWM mode, PWM on WO0
          break;
        case 2:
          TCA0.SINGLE.CMP2  = (F_CPU/2000) - 1; // 50% duty cycle,
          TCA0.SINGLE.CTRLB = (TCA_SINGLE_CMP2EN_bm | TCA_SINGLE_WGMODE_SINGLESLOPE_gc); // Single slope PWM mode, PWM on WO0
          break;
        default:
          invalidPin();
      }
      TCA0.SINGLE.CTRLA = 0x02|TCA_SINGLE_ENABLE_bm;
    #else
      TCA0.SINGLE.PER   = (F_CPU/500)  - 1; //
      switch (digitalPinToBitPosition(TUNE_OUTPUT_PIN)) {
        case 0:
          TCA0.SINGLE.CMP0  = (F_CPU/1000) - 1; // 50% duty cycle,
          TCA0.SINGLE.CTRLB = (TCA_SINGLE_CMP0EN_bm | TCA_SINGLE_WGMODE_SINGLESLOPE_gc); // Single slope PWM mode, PWM on WO0
          break;
        case 1:
          TCA0.SINGLE.CMP1  = (F_CPU/1000) - 1; // 50% duty cycle,
          TCA0.SINGLE.CTRLB = (TCA_SINGLE_CMP1EN_bm | TCA_SINGLE_WGMODE_SINGLESLOPE_gc); // Single slope PWM mode, PWM on WO0
          break;
        case 2:
          TCA0.SINGLE.CMP2  = (F_CPU/1000) - 1; // 50% duty cycle,
          TCA0.SINGLE.CTRLB = (TCA_SINGLE_CMP2EN_bm | TCA_SINGLE_WGMODE_SINGLESLOPE_gc); // Single slope PWM mode, PWM on WO0
          break;
        default:
          invalidPin();
      }
      TCA0.SINGLE.CTRLA = TCA_SINGLE_ENABLE_bm; // enable the timer with no prescaling
    #endif
    pinMode(LED_BUILTIN, OUTPUT);
  }
#else // this is for tuning another chip from a watch crystal on this chip.
  // Self-tuning with watch crystal may also be supported in the future,
  // but that woukd be it's own sketch, and is not a priority because of how
  // unpleasant they made using a crystal on these parts by virtue of their
  // sharing pins with both USART0 and one of the PWM outputs...
  // and not even one of the "bad" ones, but one of the ones that output 16-bit
  // pwm, and it's alternate pin isn't present on the most popular, 14-pin, parts!
  // Though I suspect that the alt reset - also not supported on 14-pin parts and
  // the non-QFN package option that isn't nearly the size of a brick will help
  // the popularity of the 20-pin 2-series parts.
  #error "RTC tuning is not supported at this time."
#endif
void invalidPin() {
  while(1) {
    _delay_ms(100);
    digitalWrite(LED_BUILTIN, CHANGE);
  }
}
#else
 /* In this case it's a classic AVR */
  // Target pulse length, in us.
 #define TARGET_OC1 1000
  //#define TARGET_OC2 1000
  //#define TARGET_OC3 2000
  //#define TARGET_OC4 2000
  //#define TARGET_OC5 2000
  // clocks per microsecond times microseconds times 2 gives period (since each cycle consists of a high and low pulse.)
  #define OC1PERIOD ((TARGET_OC1 * (F_CPU/1000000) * 2))
  #define OC1LENGTH ((TARGET_OC1 * (F_CPU/1000000)))
 #if (OC1PERIOD > 65535)
   #if (OC1PERIOD & 0x07 != 0)
      #error "Cannot generate accurate pulses of desired length from this timebase"
      // Mad props for the overclock if you run into this for 1ms pulses!
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
      uint8_t cs1 = 0x02;
    #else
      uint16_t Timer1TOP = (OC1PERIOD) - 1;
      uint16_t TimerOC1A = (OC1LENGTH) - 1;
      uint16_t TimerOC1B = (OC1LENGTH) - 1;
      uint8_t cs1 = 0x01;
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
    TCCR1B = ((WGM1 & 0x0C) << 1) | cs1;  // restart tme
    ICR1 = Timer1TOP;
    OCR1A = TimerOC1A;
    OCR1B = TimerOC1B;
    pinMode(LED_BUILTIN,OUTPUT);
    Serial.println(OCR1A);
  }
#endif
void loop() {
  // put your main code here, to run repeatedly:
  _delay_ms(400);
  digitalWrite(LED_BUILTIN,1);
  _delay_ms(100);
  digitalWrite(LED_BUILTIN,0);
}
