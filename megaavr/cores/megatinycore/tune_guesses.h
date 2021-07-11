/* These are the gusses at cal change needed to get these speeds untuned     *
 * They are concatenated with the sigrow reference, so + or - operator works.*
 * Putting them all in one place allows better calculation of them as more   *
 * data is gathered and analyzed. 
 */

#if MEGATINYCORESERIES == 2
  #define GUESS_20_32  62
  #define GUESS_20_30  52
  #define GUESS_16_30  90
  #define GUESS_20_25  27
  #define GUESS_16_25  59
  #define GUESS_20_24  23
  #define GUESS_16_24  53
  #define GUESS_20_20   0
  #define GUESS_16_20  27
  #define GUESS_20_16 -23
  #define GUESS_16_16   0
  #define GUESS_16_12 -28
  #define MAX_TUNING  127
#else
  #define GUESS_20_30  33
  #define GUESS_20_25  17
  #define GUESS_16_25  35
  #define GUESS_20_24  13
  #define GUESS_16_24  31
  #define GUESS_20_20   0
  #define GUESS_16_20  16
  #define GUESS_20_16 -14
  #define GUESS_16_16   0
  #define GUESS_16_12 -28
  #define GUESS_16_10 -26
  #define MAX_TUNING   63
  /* These last two are ugly - sorry: Some parts need to tune up to 24 and    *
   * halve it, others can't hit 24, but can be tuned down . All 16 can hit 20 *
   * to be scaled down, but we do gather number for 10 itself. No 20 can be   *
   * tuned down to 10, just scale.                                            */
  #define GUESS_20_12_DIRECT -28
  #define GUESS_20_12 + (SIGROW_OSCCAL20M0 >= (-1*GUESS_20_12_DIRECT) ? GUESS_20_12_DIRECT : GUESS_20_24)
  #define GUESS_16_10_DIRECT -26 
  #define GUESS_16_10 + (SIGROW_OSCCAL16M0 >= (-1*GUESS_16_10_DIRECT) ? GUESS_16_10_DIRECT : GUESS_16_20)
#endif
/* Here we pick the correct one to use for the requested frequency.           *
 * First TUNE_PRESCALE is determined so you can tune 4, 5, 8 and on 2-series, *
 * 10 MHz speeds. _CLOCKSPERUS is thus based on unprescaled clock, which is   *
 * what we are calibrating. This gets checked for -1 then stored to tuneval   *
 * so the 0x80 is "unreachable) "                                             */

#if MEGATINYCORESERIES == 2
  #if F_CPU == 1000000
    #define TUNE_PRESCALE (CLKCTRL_PEN_bm | (osccfg ? CLKCTRL_PDIV_20X_gc : CLKCTRL_PDIV_16X_gc)
    #define GUESSCAL -1
  #else
    #if F_CPU < 6000000
      #define _CLOCKSPERUS (F_CPU/250000)
      #define TUNE_PRESCALE (CLKCTRL_PEN_bm | CLKCTRL_PDIV_4X_gc)
    #elif F_CPU < 12000000
      #define _CLOCKSPERUS (F_CPU/500000)
      #define TUNE_PRESCALE (CLKCTRL_PEN_bm | CLKCTRL_PDIV_2X_gc)
    #else   
      #define _CLOCKSPERUS (F_CPU/1000000)
    #endif
    #if _CLOCKSPERUS == 32
      #define GUESSCAL (osccfg ? SIGROW_OSCCAL20M0 + GUESS_20_32 : 0x80)
      #define TUNED_CALIBRATION_OFFSET (osccfg ? 5 : -1)
    #elif _CLOCKSPERUS == 30
      #define GUESSCAL (osccfg ? SIGROW_OSCCAL20M0 + GUESS_20_30 : SIGROW_OSCCAL16M0 + GUESS_16_30)
      #define TUNED_CALIBRATION_OFFSET (osccfg ? 4 : 5)
    #elif _CLOCKSPERUS == 25
      #define GUESSCAL (osccfg ? SIGROW_OSCCAL20M0 + GUESS_20_25 : SIGROW_OSCCAL16M0 + GUESS_16_25)
      #define TUNED_CALIBRATION_OFFSET (osccfg ? 3 : 4)
    #elif _CLOCKSPERUS == 24
      #define GUESSCAL (osccfg ? SIGROW_OSCCAL20M0 + GUESS_20_24 : SIGROW_OSCCAL16M0 + GUESS_16_24)
      #define TUNED_CALIBRATION_OFFSET (osccfg ? 2 : 3)
    #elif _CLOCKSPERUS == 20
      #define GUESSCAL (osccfg ?                            -1 : SIGROW_OSCCAL16M0 + GUESS_16_29)
      #define TUNED_CALIBRATION_OFFSET (osccfg ? 1 : 2)
    #elif _CLOCKSPERUS == 16
      #define GUESSCAL (osccfg ? SIGROW_OSCCAL20M0 + GUESS_20_16 :                            -1)
      #define TUNED_CALIBRATION_OFFSET (osccfg ? 0 : 1)
    #elif _CLOCKSPERUS == 12
      #define GUESSCAL (osccfg ? SIGROW_OSCCAL20M0 + GUESS_20_24 : SIGROW_OSCCAL16M0 + GUESS_16_12)
      #define TUNED_CALIBRATION_OFFSET (osccfg ? 2 : 0)
      #ifndef TUNE_PRESCALE
        #define TUNE_PRESCALE ((osccfg || (SIGROW_OSCCAL16M0 >= (-1 * GUESS_16_10_DIRECT))) ? (CLKCTRL_PEN_bm | CLKCTRL_PDIV_2X_gc) : 0)
      #else
        #error "Unsupported clock speed"
      #endif
    #else 
      #error "Unsupported clock speed"
    #endif
  #endif
#else
  #if F_CPU == 1000000
    #define TUNE_PRESCALE (CLKCTRL_PEN_bm | (osccfg ? CLKCTRL_PDIV_20X_gc : CLKCTRL_PDIV_16X_gc)
    #define GUESSCAL -1
  #else
    #if F_CPU <= 5000000
      #define _CLOCKSPERUS (F_CPU/250000)
      #define TUNE_PRESCALE (CLKCTRL_PEN_bm | CLKCTRL_PDIV_4X_gc)
    #elif F_CPU < 10000000
      #define _CLOCKSPERUS (F_CPU/500000)
      #define TUNE_PRESCALE (CLKCTRL_PEN_bm | CLKCTRL_PDIV_2X_gc)
    #else   
      #define _CLOCKSPERUS (F_CPU/1000000)
    #endif
    #if _CLOCKSPERUS == 30
      #define GUESSCAL (osccfg ? SIGROW_OSCCAL20M0 + GUESS_20_30 : 0x80)
      #define TUNED_CALIBRATION_OFFSET (osccfg ? 5 : -1)
    #elif _CLOCKSPERUS == 25
      #define GUESSCAL (osccfg ? SIGROW_OSCCAL20M0 + GUESS_20_25 : SIGROW_OSCCAL16M0 + GUESS_16_25)
      #define TUNED_CALIBRATION_OFFSET (osccfg ? 4 : 5)
    #elif _CLOCKSPERUS == 24
      #define GUESSCAL (osccfg ? SIGROW_OSCCAL20M0 + GUESS_20_24 : SIGROW_OSCCAL16M0 + GUESS_16_24)
      #define TUNED_CALIBRATION_OFFSET (osccfg ? 3 : 5)
    #elif _CLOCKSPERUS == 20 //or 5
      #define GUESSCAL (osccfg ?                            -1 : SIGROW_OSCCAL16M0 + GUESS_16_20)
      #define TUNED_CALIBRATION_OFFSET (osccfg ? 2 : 5)
    #elif _CLOCKSPERUS == 16 // or 8 or 4
      #define GUESSCAL (osccfg ? SIGROW_OSCCAL20M0 + GUESS_20_16 :                            -1)
      #define TUNED_CALIBRATION_OFFSET (osccfg ? 1 : 5)
    #elif _CLOCKSPERUS == 12
      #ifndef TUNE_PRESCALE //12 MHz
        #define GUESSCAL (osccfg ? SIGROW_OSCCAL20M0 + GUESS_20_12 : SIGROW_OSCCAL16M0 + GUESS_16_12)
        #define TUNED_CALIBRATION_OFFSET (osccfg ? 4 : 5)
        #define TUNE_PRESCALE ((osccfg && SIGROW_OSCCAL20M0 < GUESS_20_12_DIRECT) ? (CLKCTRL_PEN_bm | CLKCTRL_PDIV_2X_gc) : 0)
      #else
        #error "Unsupported clock speed"
      #endif
    #elif _CLOCKSPERUS == 10
      #define GUESSCAL (osccfg ?                          -1  : SIGROW_OSCCAL16M0 + GUESS_16_10)
      #define TUNED_CALIBRATION_OFFSET (osccfg ? 4 : 5)
      #define TUNE_PRESCALE ((osccfg || GUESSCAL == GUESS_16_20) ? (CLKCTRL_PEN_bm | CLKCTRL_PDIV_2X_gc) : 0)
    #else 
      #error "Unsupported clock speed"
    #endif
  #endif
#endif
//if no prescaling needed, remember we need to turn it off!
#ifndef TUNE_PRESCALE
  #define TUNE_PRESCALE 0
#endif
