#include <stdbool.h>

#include "pins_arduino.h"
#include "api/Common.h"

#define FORCE_INLINE __attribute__((always_inline)) inline

void setup_timers() {

    /*  TYPE A TIMER   */

    /* PORTMUX setting for TCA - don't need to set because using default */
    //PORTMUX.CTRLA = PORTMUX_TCA00_DEFAULT_gc;

    /* Enable Split Mode */
    TCA0.SPLIT.CTRLD = TCA_SPLIT_SPLITM_bm;

    //Only 1 WGM so no need to specifically set up.

    /* Period setting, 8-bit register in SPLIT mode */
    TCA0.SPLIT.LPER    = PWM_TIMER_PERIOD;
    TCA0.SPLIT.HPER    = PWM_TIMER_PERIOD;

    /* Default duty 50%, will re-assign in analogWrite() */
    //TODO: replace with for loop to make this smaller;
    TCA0.SPLIT.LCMP0 = PWM_TIMER_COMPARE;
    TCA0.SPLIT.LCMP1 = PWM_TIMER_COMPARE;
    TCA0.SPLIT.LCMP2 = PWM_TIMER_COMPARE;
    TCA0.SPLIT.HCMP0 = PWM_TIMER_COMPARE;
    TCA0.SPLIT.HCMP1 = PWM_TIMER_COMPARE;
    TCA0.SPLIT.HCMP2 = PWM_TIMER_COMPARE;

    /* Use DIV64 prescaler (giving 250kHz clock), enable TCA timer */
    TCA0.SPLIT.CTRLA = (TCA_SINGLE_CLKSEL_DIV64_gc) | (TCA_SINGLE_ENABLE_bm);


    /*    TYPE B TIMERS  */

    // No megaTinyCore parts need to change the port mapping.

    /* Start with TCB0 */
    #ifdef TCB0
    TCB_t *timer_B = (TCB_t *)&TCB0;

    /* Timer B Setup loop for TCB[0:2] */
    #ifdef TCB1
    do{
    #endif
        /* 8 bit PWM mode, but do not enable output yet, will do in analogWrite() */
        timer_B->CTRLB = (TCB_CNTMODE_PWM8_gc);

        /* Assign 8-bit period */
        timer_B->CCMPL = PWM_TIMER_PERIOD;

        /* default duty 50%, set when output enabled */
        timer_B->CCMPH = PWM_TIMER_COMPARE;

        /* Use TCA clock (250kHz) and enable */
        /* (sync update commented out, might try to synchronize later */
        timer_B->CTRLA = (TCB_CLKSEL_CLKTCA_gc)
                        //|(TCB_SYNCUPD_bm)
                        |(TCB_ENABLE_bm);

        /* Increment pointer to next TCB instance */
        timer_B++;

    /* Stop when pointing to TCB3 */
    // } while (timer_B < (TCB_t *)&TCB3);

    // ATtiny only uses up to TCB1
    /* Stop when pointing to TCB1 */
    #ifdef TCB1
    } while (timer_B <= (TCB_t *)&TCB1);
    #endif
    #endif
    #ifdef TCD0
    #if defined(USE_TIMERD0_PWM) && (!(defined(MILLIS_USE_TIMERD0_A0) || defined(MILLIS_USE_TIMERD0)))
    TCD0.CMPBCLR=255; //Count to 255 (8-bit resolution)
    TCD0.CTRLC=0x80; //WOD outputs PWM B, WOC outputs PWM A
    TCD0.CTRLB=0x03; //dual slope
    TCD0.CTRLA=0x10; //OSC20M prescaled by 32, gives ~1.2 khz PWM at 20MHz.
    #endif
    #endif

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
    // NA
}
