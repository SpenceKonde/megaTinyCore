/* These are the gusses at cal change needed to get these speeds untuned     *
 * They are concatenated with the sigrow reference, so + or - operator works.*
 * Putting them all in one place allows better calculation of them as more   *
 * data is gathered and analyzed.
 *
 * Possible Freq's are:
 * 2-series, 20 MHz: 16,20,24,25,30,32
 *           16 MHz: 12,16,20,24,25,30
 * 1-series, 20 MHz: 12,16,20,24,25,30
 *           16 MHz: 10,12,16,20,24,25

 */

#define GUESS_DEFAULT  (-1)

#if MEGATINYCORE_SERIES == 2
  #define GUESS_20_32  (62)
  #define GUESS_20_30  (52)
  #define GUESS_16_30  (90)
  #define GUESS_20_25  (27)
  #define GUESS_16_25  (59)
  #define GUESS_20_24  (23)
  #define GUESS_16_24  (53)
  #define GUESS_20_20   (0)
  #define GUESS_16_20  (27)
  #define GUESS_20_16 (-23)
  #define GUESS_16_16   (0)
  #define GUESS_16_12 (-28)
  #define MAX_TUNING  (127)
  #define HOME_TUNING_OFFSET   (1)
#else
  #define GUESS_20_30  (33)
  #define GUESS_20_25  (17)
  #define GUESS_16_25  (35)
  #define GUESS_20_24  (13)
  #define GUESS_16_24  (31)
  #define GUESS_20_20   (0)
  #define GUESS_16_20  (16)
  #define GUESS_20_16 (-14)
  #define GUESS_16_16   (0)
  #define GUESS_16_12 (-28)
  #define MAX_TUNING   (63)
  #define HOME_TUNING_OFFSET  (2)
  /* These last two are ugly - sorry: Some parts need to tune up to 24 and    *
   * halve it, others can't hit 24, but can be tuned down . All 16 can hit 20 *
   * to be scaled down, but we do gather number for 10 itself. No 20 can be   *
   * tuned down to 10, just prescale for that                                 */
  #define GUESS_20_12_DIRECT (-28)
  #define CLOCK2012_DIRECT_GUESS_UNREACHABLE     (SIGROW_OSCCAL20M0 < GUESS_20_12_DIRECT)
  #define GUESS_20_12 (CLOCK2012_DIRECT_GUESS_UNREACHABLE ? (GUESS_20_12_DIRECT : GUESS_20_24))
  #define GUESS_16_10_DIRECT (-26)
  #define CLOCK1610_DIRECT_GUESS_UNREACHABLE     (SIGROW_OSCCAL16M0 < GUESS_16_10_DIRECT)
  #define GUESS_16_10 (CLOCK1610_DIRECT_GUESS_UNREACHABLE ? (GUESS_16_20 : GUESS_16_10_DIRECT))
#endif
/* Here we pick the correct one to use for the requested frequency.                       *
 * First TUNE_PRESCALE is determined to permit tuning to very low speeds, which is used   *
 * in the rare event that 1/4/5/8/10 (2-series only) is selected with tuning.             *
 * (10 on 2-series and 12 on all parts are more complicated, and may not be set just yet  *
 * _CLOCKSPERUS is thus based on unprescaled clock, which is  what we are calibrating.    *
 * This gets checked for -1 ("clock speed appears unreachable"), and otherwise            *
 * TUNE_PRECSCALE is set to -1 if we know we can't get within
 *ble) "
 * Note the _ - outside of this file, we don't want anyone to know about this */

#if MEGATINYCORE_SERIES == 2
  #if F_CPU == 1000000
    #define TUNE_PRESCALE (CLKCTRL_PEN_bm | (osccfg ? CLKCTRL_PDIV_16X_gc /* there is no CLKCTRL_PDIV_20X_gc - but this should be a cant-happen because we set the oscillator to 16 when
    telling it to run from 1 MHz. Unless optiboot is in use, and you compile and upload for the wrong osc base frequency, because you changed clock speed selection
    and didn't reburn the bootloader *and* uploaded using optiboot with 1 MHz selected... */ : CLKCTRL_PDIV_16X_gc))
    #define GUESSCAL ((osccal ? GUESS_20_16 : 0))
  #else
    #define _CLOCKSPERUS  (F_CPU / 1000000)
    #if _CLOCKSPERUS == 32
      #define GUESSCAL (osccfg ? SIGROW_OSCCAL20M0 + GUESS_20_32 :                            0x80)
      #define TUNED_CALIBRATION_OFFSET (osccfg ? 5 : -1)
    #elif _CLOCKSPERUS == 30
      #define GUESSCAL (osccfg ? SIGROW_OSCCAL20M0 + GUESS_20_30 : SIGROW_OSCCAL16M0 + GUESS_16_30)
      #define TUNED_CALIBRATION_OFFSET (osccfg ? 4 :  5)
    #elif _CLOCKSPERUS == 25
      #define GUESSCAL (osccfg ? SIGROW_OSCCAL20M0 + GUESS_20_25 : SIGROW_OSCCAL16M0 + GUESS_16_25)
      #define TUNED_CALIBRATION_OFFSET (osccfg ? 3 :  4)
    #elif _CLOCKSPERUS == 24 /* For 12/6 MHz t00*/
      #define GUESSCAL (osccfg ? SIGROW_OSCCAL20M0 + GUESS_20_24 : SIGROW_OSCCAL16M0 + GUESS_16_24)
      #define TUNED_CALIBRATION_OFFSET (osccfg ? 2 :  3)
    #elif _CLOCKSPERUS == 20 /* For 10/5 MHz t00*/
      #define GUESSCAL (osccfg ? SIGROW_OSCCAL20M0 + 0           : SIGROW_OSCCAL16M0 + GUESS_16_29)
      #define TUNED_CALIBRATION_OFFSET (osccfg ? 1 :  2)
    #elif _CLOCKSPERUS == 16
      #define GUESSCAL (osccfg ? SIGROW_OSCCAL20M0 + GUESS_20_16 : SIGROW_OSCCAL16M0 + 0          )
      #define TUNED_CALIBRATION_OFFSET (osccfg ? 0 :  1)
    #elif _CLOCKSPERUS == 12
      #define GUESSCAL (osccfg ? SIGROW_OSCCAL20M0 + GUESS_20_24 : SIGROW_OSCCAL16M0 + GUESS_16_12)
      #define TUNED_CALIBRATION_OFFSET (osccfg ? 2 :  0)
      #ifndef TUNE_PRESCALE
        #define TUNE_PRESCALE ((osccfg ? (CLKCTRL_PEN_bm | CLKCTRL_PDIV_2X_gc) : 0))
      #endif
    #elif _CLOCKSPERUS == 10
      #define GUESSCAL ((osccfg ? (SIGROW_OSCCAL20M0 +  0) : (SIGROW_OSCCAL16M0 + GUESS_16_10))
      #define TUNED_CALIBRATION_OFFSET (osccfg ? 2 :  0)
      #ifndef TUNE_PRESCALE
        #define TUNE_PRESCALE ((osccfg ? (CLKCTRL_PEN_bm | CLKCTRL_PDIV_2X_gc) : 0))
      #endif
    #elif _CLOCKSPERUS == 8
      #define GUESSCAL (osccfg ? SIGROW_OSCCAL20M0 + GUESS_20_16 : SIGROW_OSCCAL16M0 + 0)
      #define TUNED_CALIBRATION_OFFSET (osccfg ? 2 :  0)
      #ifndef TUNE_PRESCALE
        #define TUNE_PRESCALE ((CLKCTRL_PEN_bm | CLKCTRL_PDIV_2X_gc))
      #endif
    #elif _CLOCKSPERUS == 6
      #define GUESSCAL (osccfg ? SIGROW_OSCCAL20M0 + GUESS_20_24 : SIGROW_OSCCAL16M0 + GUESS_16_12)
      #define TUNED_CALIBRATION_OFFSET (osccfg ? 2 :  0)
      #ifndef TUNE_PRESCALE
        #define TUNE_PRESCALE ((osccfg ? (CLKCTRL_PEN_bm | CLKCTRL_PDIV_4X_gc) :  (CLKCTRL_PEN_bm | CLKCTRL_PDIV_2X_gc)))
      #endif
    #elif _CLOCKSPERUS == 5
      #define GUESSCAL (osccfg ? SIGROW_OSCCAL20M0 +  0          : SIGROW_OSCCAL16M0 + GUESS_16_10)
      #define TUNED_CALIBRATION_OFFSET (osccfg ? 2 :  1)
      #ifndef TUNE_PRESCALE
        #define TUNE_PRESCALE ((osccfg ? (CLKCTRL_PEN_bm | CLKCTRL_PDIV_4X_gc) : (CLKCTRL_PEN_bm | CLKCTRL_PDIV_2X_gc)))
      #endif
    #elif _CLOCKSPERUS == 4
      #define GUESSCAL (osccfg ? SIGROW_OSCCAL20M0 + GUESS_20_16 : SIGROW_OSCCAL16M0 +  0 )
      #define TUNED_CALIBRATION_OFFSET (osccfg ? 1 :  2)
      #ifndef TUNE_PRESCALE
        #define TUNE_PRESCALE (CLKCTRL_PEN_bm | CLKCTRL_PDIV_4X_gc)
      #endif
    #elif _CLOCKSPERUS == 1
      #define GUESSCAL (osccfg ? SIGROW_OSCCAL20M0 + GUESS_20_16 : SIGROW_OSCCAL16M0 + GUESS_16_16)
      #define TUNED_CALIBRATION_OFFSET (osccfg ? 1 :  2)
      #ifndef TUNE_PRESCALE
        #define TUNE_PRESCALE (CLKCTRL_PEN_bm | CLKCTRL_PDIV_16X_gc)
      #endif
    #else
      #error "Unsupported clock speed"
    #endif
  #endif
/* These are the gusses at cal change needed to get these speeds untuned     *
 * They are concatenated with the sigrow reference, so + or - operator works.*
 * Putting them all in one place allows better calculation of them as more   *
 * data is gathered and analyzed.
 *
 * Possible Freq's are:
 * 2-series, 20 MHz: 16,20,24,25,30,32
 *           16 MHz: 12,16,20,24,25,30
 * 1-series, 20 MHz: 12,16,20,24,25,30
 *           16 MHz: 10,12,16,20,24,25

 */
#else /* 0/1-series! */
  #define _CLOCKSPERUS (F_CPU/1000000)
  #if _CLOCKSPERUS == 30
    #define GUESSCAL (osccfg ? SIGROW_OSCCAL20M0 + GUESS_20_30   :                              -1)
    #define TUNED_CALIBRATION_OFFSET    (osccfg ? 5 : -1)
  #elif _CLOCKSPERUS == 25
    #define GUESSCAL (osccfg ? SIGROW_OSCCAL20M0 + GUESS_20_25   : SIGROW_OSCCAL16M0 + GUESS_16_25)
    #define TUNED_CALIBRATION_OFFSET    (osccfg ? 4 :  5)
  #elif _CLOCKSPERUS == 24
    #define GUESSCAL (osccfg ? SIGROW_OSCCAL20M0 + GUESS_20_24   : SIGROW_OSCCAL16M0 + GUESS_16_24)
    #define TUNED_CALIBRATION_OFFSET    (osccfg ? 3 :  4)
  #elif _CLOCKSPERUS == 20 // or 5 MHz
    #define GUESSCAL (osccfg ? SIGROW_OSCCAL20M0 + 0             : SIGROW_OSCCAL16M0 + GUESS_16_20)
    #define TUNED_CALIBRATION_OFFSET    (osccfg ? 2 :  3)
  #elif _CLOCKSPERUS == 16 // or 8 or 4 MHz
    #define GUESSCAL (osccfg ? SIGROW_OSCCAL20M0 + GUESS_20_16   : SIGROW_OSCCAL16M0 + 0          )
    #define TUNED_CALIBRATION_OFFSET    (osccfg ? 1 :  2)
  #elif _CLOCKSPERUS == 12
      #define GUESSCAL (osccfg ? SIGROW_OSCCAL20M0 + GUESS_20_12 : SIGROW_OSCCAL16M0 + GUESS_16_12)
      #define TUNED_CALIBRATION_OFFSET  (osccfg ? (CLOCK2012_DIRECT_GUESS_UNREACHABLE ? 3 :0) : 4)
    #ifndef TUNE_PRESCALE  // 12 MHz
      #define TUNE_PRESCALE (osccfg || CLOCK2012_DIRECT_GUESS_UNREACHABLE ? (CLKCTRL_PEN_bm | CLKCTRL_PDIV_2X_gc) : 0)
    #endif
  #elif _CLOCKSPERUS == 10
    #define GUESSCAL (osccfg ? SIGROW_OSCCAL20M0                : SIGROW_OSCCAL16M0 + GUESS_16_10)
    #define TUNED_CALIBRATION_OFFSET (osccfg ? 4 :  5)
    #define TUNE_PRESCALE ((osccfg || CLOCK1610_DIRECT_GUESS_UNREACHABLE ) ? (CLKCTRL_PEN_bm | CLKCTRL_PDIV_4X_gc) : 0)
  #elif _CLOCKSPERUS == 8
    #define GUESSCAL (osccfg ? SIGROW_OSCCAL20M0 + GUESS_20_16  : SIGROW_OSCCAL16M0)
    #define TUNED_CALIBRATION_OFFSET (osccfg ? 2 :  0)
  #elif _CLOCKSPERUS == 6
    #define GUESSCAL (osccfg ? SIGROW_OSCCAL20M0 + GUESS_20_24  : SIGROW_OSCCAL16M0 + GUESS_16_12)
    #define TUNED_CALIBRATION_OFFSET (osccfg ? 2 :  0)
  #elif _CLOCKSPERUS == 5
    #define GUESSCAL (osccfg ? SIGROW_OSCCAL20M0 + GUESS_20_16  : SIGROW_OSCCAL16M0 + GUESS_16_10)
    #define TUNED_CALIBRATION_OFFSET (osccfg ? 4 :  5)
    #define TUNE_PRESCALE ((osccfg || CLOCK1610_DIRECT_GUESS_UNREACHABLE ) ? (CLKCTRL_PEN_bm | CLKCTRL_PDIV_4X_gc) : 0)
  #elif _CLOCKSPERUS == 4
    #define GUESSCAL (SIGROW_OSCCAL20M0 + (osccfg ? (CLOCK2012_DIRECT_GUESS_UNREACHABLE ? GUESS_20_24:GUESS_20_12 SIGROW_OSCCAL16M0 + GUESS_16_12)
    #define TUNED_CALIBRATION_OFFSET (osccfg ? 2 :  0)
  #elif _CLOCKSPERUS == 1
    #define TUNE_PRESCALE (CLKCTRL_PEN_bm | CLKCTRL_PDIV_16X_gc )
    #define GUESSCAL (osccfg ? SIGROW_OSCCAL20M0 + GUESS_20_16 : SIGROW_OSCCAL16M0)
    #define TUNED_CALIBRATION_OFFSET (2)
  #else
    #error "Unsupported clock speed"
  #endif
#endif
// if no prescaling needed, remember we need to turn it off!
#ifndef TUNE_PRESCALE
  #define TUNE_PRESCALE (0)
#endif
