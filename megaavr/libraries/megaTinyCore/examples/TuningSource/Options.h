#include "Arduino.h"
#include "parts.h"

void timeron(uint8_t pin) {

#if defined(MEGATINYCORE)
  #if defined(PIN_PB0)
    #define TUNING_OUTPUT_PIN PIN_PB0
  #elif defined(PIN_PA1)
    #define TUNING_OUTPUT_PIN PIN_PA1
  #endif
 #elif defined(DXCORE)
  #if !defined(TCA0)
    #error "TCA0 is not present, the EB series is not supported";
  #if defined(PIN_PA2)
    #define TUNING_OUTPUT_PIN PIN_PA2
    PORTMUX.TCA0ROUTEA = PA;
  #elif defined(PIN_PC2)
    PORTMIX.TCA0ROUTEA = PC;
    #define TUNING_OUTPUT_PIN PIN_PC2
  #elif (CLOCK_SOURCE == 0)
    PORTMUX.TCA0ROUTEA = PA0;
    #define TUNING_OUTPUT_PIN PIN_PA0
  #else
    #error "This part in this pincount has no pins capable of outputting a tuning signal in this way."
  #endif
 #elif defined(TIMER1_TYPICAL) && TIMER1_TYPICAL == 87
   #define TUNING_OUTPUT_PIN PIN_PB1;
   if (!PIN_IS_VALID_TIMER1(pin)) {return;}
   CONFIGURE_PWM();
 #endif
 #if defined(TCA0) {
  takeOverTCA0();
  pinModeFast(TUNING_OUTPUT_PIN,OUTPUT);
  TCA0.SINGLE.PER = F_CPU/1000-1;
