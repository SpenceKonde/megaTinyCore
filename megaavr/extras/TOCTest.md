# PWM and Timers on modern AVRs and their usage in this core
*  [Quick answer: Which PWM pins should I use?](Ref_Timers.md#quick-answer-which-pwm-pins-should-i-use)
  *  Why TCB2 as default millis timer?
*  Section One: Background: Timers on modern AVRs
*  Background: The Timers on the AVR Dx-series and Ex-series parts
  *  TCA0 - Type A 16-bit Timer with 3/6 PWM channels
    *  Events and CCL channels
    *  Interrupt note
    *  Future part note
  *  TCBn - Type B 16-bit Timer
    *  Periodic interrupt
    *  Input Capture on Event
    *  Input Capture Frequency Measurement
    *  Input Capture Pulse Width Measurement
    *  Input Capture Frequecy And Pulse Width Measurement
    *  Single-shot
    *  Timeout Check
    *  8-bit PWM mode
    *  Extra features on 2-series and Dx/Ex-series
    *  Intflag summary
  *  TCD0 - Type D 12-bit Async Timer
    *  For more information on what you can do to TCD0 and still use analogWrite()
  *  TCE - Lurking in the distance with WEX
  *  TCF - Another new timer
  *  RTC - 16-bit Real Time Clock and Programmable Interrupt Timer
    *  RTC/PIT errata on 0/1-series
*  Timer Prescaler Availability
*  Resolution, Frequency and Period
  *  [In Google Sheets](https://docs.google.com/spreadsheets/d/10Id8DYLRtlp01KA7vvslC3cHaR4S2a1TrH7u6pHXMNY/edit?usp=sharing)
*  Section Two: How the core uses these timers
  *  Initialization
  *  Justification for TOP = 254, not 255
  *  PWM via analogWrite()
    *  Priority
    *  Channels without pins
    *  TCAn
  *  TCD0
    *  TCD0 on DxCore
    *  TCD0 on megaTinyCore
  *  TCBn
    *  API exstens
    * # bool digitalPinHasPWMNow(uint8_t p)
    * # uint8_t digitalPinToTimerNow(uint8_t p)
  *  takeOverTCA0()
  *  takeOverTCA1()
  *  takeOverTCD0()
  *  resumeTCA0()
  *  resumeTCA1()
  *  There is no takeover or resume of TCBs
  *  PWM Frequencies
    *  Summary Table
      * DxCore
      * megaTinyCore
*  Millis/Micros Timekeeping
  *  Why this longass section matters
  *  TCAn for millis timekeeping
    *  TCA timekeeping resolution
    *  TCBn for millis timekeeping
  *  TCD0 for millis timekeeping
  *  Manipulating millis timekeeping
*  Tone
  *  Long tones which specify a duration
*  Servo Library
*  Additional functions for advanced timer control
*  Appendix I: Names of timers
  *  For DxCore
  *  For megaTinyCore
  *  Approximate algorithm for interpreting these
  *  `_GCMT` return values
*  Appendix II: TCB Micros Artifacts
