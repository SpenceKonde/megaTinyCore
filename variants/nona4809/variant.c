#include <stdbool.h>

#include "pins_arduino.h"
#include "api/Common.h"

#define FORCE_INLINE __attribute__((always_inline)) inline

void setup_timers() {

  //  TYPE A TIMER  

  // PORTMUX setting for TCA -> all outputs [0:2] point to PORTB pins [0:2]
  PORTMUX.TCAROUTEA  = PORTMUX_TCA0_PORTB_gc;

  // Setup timers for single slope PWM, but do not enable, will do in analogWrite()
  TCA0.SINGLE.CTRLB = TCA_SINGLE_WGMODE_SINGLESLOPE_gc;

  // Period setting, 16 bit register but val resolution is 8 bit
  TCA0.SINGLE.PER = PWM_TIMER_PERIOD;

  // Default duty 50%, will re-assign in analogWrite()
  TCA0.SINGLE.CMP0BUF = PWM_TIMER_COMPARE;
  TCA0.SINGLE.CMP1BUF = PWM_TIMER_COMPARE;
  TCA0.SINGLE.CMP2BUF = PWM_TIMER_COMPARE;

  // Use DIV64 prescaler (giving 250kHz clock), enable TCA timer
  TCA0.SINGLE.CTRLA = (TCA_SINGLE_CLKSEL_DIV64_gc) | (TCA_SINGLE_ENABLE_bm);

  //  TYPE B TIMERS 
  
  // Setup TCB0 routing
  #if defined(TCB0)
    PORTMUX.TCBROUTEA |= PORTMUX_TCB0_bm; // Route signal to PF4
  #endif
  
  // Setup TCB1 routing
  #if defined(TCB1)
    PORTMUX.TCBROUTEA  |= PORTMUX_TCB1_bm; // Route signal to PF5
  #endif

  // Start with TCB0
  TCB_t *timer_B = (TCB_t *)&TCB0;
  
  // Find end timer
  #if defined(TCB3)
    TCB_t *timer_B_end = (TCB_t *)&TCB3;
  #elif defined(TCB2)
    TCB_t *timer_B_end = (TCB_t *)&TCB2;
  #elif defined(TCB1)
    TCB_t *timer_B_end = (TCB_t *)&TCB1;
  #else
    TCB_t *timer_B_end = (TCB_t *)&TCB0;
  #endif

  // Timer B Setup loop for TCB[0:3]
  do
  {
    // 8 bit PWM mode, but do not enable output yet, will do in analogWrite()
    timer_B->CTRLB = (TCB_CNTMODE_PWM8_gc);

    // Assign 8-bit period
    timer_B->CCMPL = PWM_TIMER_PERIOD;

    // default duty 50%, set when output enabled
    timer_B->CCMPH = PWM_TIMER_COMPARE;

    // Use TCA clock (250kHz) and enable
    // (sync update commented out, might try to synchronize later
    timer_B->CTRLA = (TCB_CLKSEL_CLKTCA_gc)
            //|(TCB_SYNCUPD_bm)
            |(TCB_ENABLE_bm);

    // Increment pointer to next TCB instance
    timer_B++;

  // Stop when pointing to TCB3
  } while (timer_B <= timer_B_end);

  // Stuff for synchronizing PWM timers
//   // Restart TCA to sync TCBs
//   // should not be needed   
//   TCA0.SINGLE.CTRLESET = TCA_SINGLE_CMD_RESTART_gc;
//   TCA0.SINGLE.CTRLECLR = TCA_SINGLE_CMD_RESTART_gc;
//
//   timer_B = (TCB_t *)&TCB0;
//
//   // TCB are sync to TCA, remove setting 
//   for (uint8_t digitial_pin_timer = (TIMERB0 - TIMERB0);
//   digitial_pin_timer < (TIMERB3 - TIMERB0);
//   digitial_pin_timer++)
//   {
//     // disable sync with tca
//     timer_B->CTRLA &= ~ (TCB_SYNCUPD_bm);
//
//     // Add offset to register 
//     timer_B++;
//
//   }
}

FORCE_INLINE bool isDoubleBondedActive(uint8_t pin) {
  (void)pin;

  /* Check if TWI is operating on double bonded pin (Master Enable is high 
  in both Master and Slave mode for bus error detection, so this can 
  indicate an active state for Wire) */
  //if(((pin == PIN_A4) || (pin == PIN_A5)) && (TWI0.MCTRLA & TWI_ENABLE_bm)) return true;

  /* Special check for SPI_SS double bonded pin -- no action if SPI is active 
    (Using SPI Enable bit as indicator of SPI activity) */
  //if((pin == 10) && (SPI0.CTRLA & SPI_ENABLE_bm)) return true;

  return false;
}

void initVariant() {
}
