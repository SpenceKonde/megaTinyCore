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
  #define GUESS_16_14 (-18)
  #define MAX_TUNING  (127)
  #define HOME_TUNING_OFFSET   (1)
#else
  #define GUESS_20_30  (33)
  #define GUESS_20_25  (17)
  #define GUESS_20_24  (13)
  #define GUESS_20_16 (-14)

  #define GUESS_16_25  (35)
  #define GUESS_16_24  (31)
  #define GUESS_16_20  (16)
  #define GUESS_16_14 (-14)
  #define GUESS_16_12 (-28)
  #define MAX_TUNING   (63)
  #define HOME_TUNING_OFFSET  (2)
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




#if (((_CLOCKSPERUS) & ((_CLOCKSPERUS - 1 )) == 0) && _CLOCKSPERUS < 16)
  //means 1, 2, 4, 8, 16 or 32
  uint8_t t = _CLOCKSPERUS;
  uint8_t u = 0;
  while( t < 16) {
    t << 1;
    u += 2;
  } if (u) {
    TUNE_PRESCALE == u;
  }
#elif _CLOCKSPERUS == 10 || _CLOCKSPERUS == 12 || _CLOCKSPERUS == 7
  TUNE_PRESCALE = 1; //enable with 2x prescale
#elif _CLOCKSPERUS == 5 || _CLOCKSPERuS == 6
  TUNE_PRESCALE = 2; //enable with 4x prescale
#endif

#if MEGATINYCORE_SERIES == 2
  if(_osccfg == 1) {
    #if   _CLOCKSPERUS == 32
      GUESSCAL = (SIGROW_OSCCAL20M0 + GUESS_20_32        );
      TUNED_CALIBRATION_OFFSET =                       (5);
    #elif _CLOCKSPERUS == 30
      GUESSCAL = (SIGROW_OSCCAL20M0 + GUESS_20_30        );
      TUNED_CALIBRATION_OFFSET =                       (4);
    #elif _CLOCKSPERUS == 25
      GUESSCAL = (SIGROW_OSCCAL20M0 + GUESS_20_25        );
      TUNED_CALIBRATION_OFFSET =                       (3);
    #elif _CLOCKSPERUS == 24
      GUESSCAL = (SIGROW_OSCCAL20M0 + GUESS_20_24        );
      TUNED_CALIBRATION_OFFSET =                       (2);
    #elif _CLOCKSPERUS == 20 // or 5 MHz
      GUESSCAL = (SIGROW_OSCCAL20M0                      );
      TUNED_CALIBRATION_OFFSET =                       (1);
    #elif _CLOCKSPERUS == 16 // or 8 or 4 MHz
      GUESSCAL = (SIGROW_OSCCAL20M0 + GUESS_20_16        );
      TUNED_CALIBRATION_OFFSET =                       (0);
    #elif _CLOCKSPERUS == 12
      GUESSCAL = (SIGROW_OSCCAL20M0 + GUESS_20_24        );
      TUNED_CALIBRATION_OFFSET =                       (2);
    #elif _CLOCKSPERUS == 10
      GUESSCAL = (SIGROW_OSCCAL20M0                      );
      TUNED_CALIBRATION_OFFSET =                       (1);
    #elif _CLOCKSPERUS == 8
      GUESSCAL = (SIGROW_OSCCAL20M0 + GUESS_20_16        );
      TUNED_CALIBRATION_OFFSET =                       (0);
    #elif _CLOCKSPERUS == 6
      GUESSCAL = (IGROW_OSCCAL20M0  + GUESS_20_24        );
      TUNED_CALIBRATION_OFFSET =                       (2);
    #elif _CLOCKSPERUS == 5
      GUESSCAL = (SIGROW_OSCCAL20M0                      );
      TUNED_CALIBRATION_OFFSET =                       (1);
    #elif _CLOCKSPERUS == 4
      GUESSCAL = (SIGROW_OSCCAL20M0 + GUESS_20_16        );
      TUNED_CALIBRATION_OFFSET =                       (0);
    #elif _CLOCKSPERUS == 2;
      GUESSCAL = (SIGROW_OSCCAL20M0 + GUESS_20_16        );
      TUNED_CALIBRATION_OFFSET =                       (0);
    #elif _CLOCKSPERUS == 1
      GUESSCAL = (SIGROW_OSCCAL20M0 + GUESS_20_16        );
      TUNED_CALIBRATION_OFFSET =                       (0);
    #endif
  } else if(osccal == 0) {
    #if _CLOCKSPERUS == 30
      GUESSCAL = (-1);
      TUNED_CALIBRATION_OFFSET =                      (-1);
    #elif _CLOCKSPERUS == 25
      GUESSCAL = (SIGROW_OSCCAL20M0 + GUESS_20_25        );
      TUNED_CALIBRATION_OFFSET =                       (5);
    #elif _CLOCKSPERUS == 24
      GUESSCAL = (SIGROW_OSCCAL20M0 + GUESS_20_24        );
      TUNED_CALIBRATION_OFFSET =                       (4);
    #elif _CLOCKSPERUS == 20 // or 5 MHz
      GUESSCAL = (SIGROW_OSCCAL20M0 + GUESS_20_20        );
      TUNED_CALIBRATION_OFFSET =                       (3);
    #elif _CLOCKSPERUS == 16 // or 8 or 4 MHz
      GUESSCAL = (SIGROW_OSCCAL20M0                      );
      TUNED_CALIBRATION_OFFSET =                       (2);
    #elif _CLOCKSPERUS == 14 // or 8 or 4 MHz
      GUESSCAL = (AIGROW_OSCCAL20M0 + GUESS_20_14        );
      TUNED_CALIBRATION_OFFSET =                       (1);
    #elif _CLOCKSPERUS == 12
      GUESSCAL = (SIGROW_OSCCAL20M0 + GUESS_20_12        );
      TUNED_CALIBRATION_OFFSET =                       (0);
    #elif _CLOCKSPERUS == 10
      GUESSCAL = (SIGROW_OSCCAL20M0                      );
      TUNED_CALIBRATION_OFFSET =                       (4);
    #elif _CLOCKSPERUS == 8
      GUESSCAL = (SIGROW_OSCCAL20M0 + GUESS_20_16        );
      TUNED_CALIBRATION_OFFSET =                       (2);
    #elif _CLOCKSPERUS == 7
      GUESSCAL = (SIGROW_OSCCAL20M0 + GUESS_20_14        );
      TUNED_CALIBRATION_OFFSET =                       (2);
    #elif _CLOCKSPERUS == 6
      GUESSCAL = (SIGROW_OSCCAL20M0 + GUESS_20_24        );
      TUNED_CALIBRATION_OFFSET =                       (2);
    #elif _CLOCKSPERUS == 5
      GUESSCAL = (SIGROW_OSCCAL20M0 + GUESS_20_16        );
      TUNED_CALIBRATION_OFFSET =                       (4);
    #elif _CLOCKSPERUS == 4
      GUESSCAL = (SIGROW_OSCCAL20M0 + GUESS_20_16        );
      TUNED_CALIBRATION_OFFSET =                       (2);
    #elif _CLOCKSPERUS == 2
      GUESSCAL = (SIGROW_OSCCAL20M0 + GUESS_20_16        );
      TUNED_CALIBRATION_OFFSET =                       (2);
    #elif _CLOCKSPERUS == 1
      GUESSCAL = (SIGROW_OSCCAL16M0 + GUESS_20_16        );
      TUNED_CALIBRATION_OFFSET =                       (2);
    #endif
  }
/* These are the gusses at cal change needed to get these speeds untuned     *
 * They are concatenated with the sigrow reference, so + or - operator works.*
 * Putting them all in one place allows better calculation of them as more   *
 * data is gathered and analyzed.
 *
 * Possible Freq's are:
 *                   Prescaled clock options     | Tuned clock options reached directly
 * 2-series, 20 MHz: 1, 2, 4, 5, 6, -  8, 10, 12 | 16,~20, 24, 25, 30, 32 -- no 14 MHz option (no room, and 14/7 MHz is a highly unwise choice of operating frequency,
 *           16 MHz: 1, 2, 4, 5, 6, 7, 8, 10, 12 | 14,~16, 20, 24, 25, 30 -- no 32 MHz option, and isn't even officially supported. but what else could I put in this slot?
 * 1-series, 20 MHz: 1, 2, 4, 5, 6, 7, 8, 10, 12 | 14, 16,~20, 24, 25, 30 -- no 32 MHz option, and not all parts can hit 30.
 *           16 MHz: 1, 2, 4, 5, 6, 7, 8, 10, -  | 12, 14,~16, 20, 24, 25 -- no 30 or 32 MHz options. Not all parts can even hit 24.10 prescaled from 20 the usual way.

 */
#else /* 0/1-series! */
  if(_osccfg == 1) {
    #if _CLOCKSPERUS > 30
      GUESSCAL =                                      (-1);
      TUNED_CALIBRATION_OFFSET =                      (-1);
    #elif _CLOCKSPERUS == 30
      GUESSCAL = (SIGROW_OSCCAL20M0 + GUESS_20_30        );
      TUNED_CALIBRATION_OFFSET =                       (5);
    #elif _CLOCKSPERUS == 25
      GUESSCAL = (SIGROW_OSCCAL20M0 + GUESS_20_25        );
      TUNED_CALIBRATION_OFFSET =                       (4);
    #elif _CLOCKSPERUS == 24
      GUESSCAL = (SIGROW_OSCCAL20M0 + GUESS_20_24        );
      TUNED_CALIBRATION_OFFSET =                       (3);
    #elif _CLOCKSPERUS == 20 // or 5 MHz
      GUESSCAL = (SIGROW_OSCCAL20M0                      );
      TUNED_CALIBRATION_OFFSET =                       (2);
    #elif _CLOCKSPERUS == 16 // or 8 or 4 MHz
      GUESSCAL = (SIGROW_OSCCAL20M0 + GUESS_20_16        );
      TUNED_CALIBRATION_OFFSET =                       (1);
    #elif _CLOCKSPERUS == 14 // or 8 or 4 MHz
      GUESSCAL = (SIGROW_OSCCAL20M0 + GUESS_20_14        );
      TUNED_CALIBRATION_OFFSET =                       (0);
    #elif _CLOCKSPERUS == 12
      GUESSCAL = (SIGROW_OSCCAL20M0 + GUESS_20_24        );
      TUNED_CALIBRATION_OFFSET =                       (3);
    #elif _CLOCKSPERUS == 10
      GUESSCAL = (SIGROW_OSCCAL20M0                      );
      TUNED_CALIBRATION_OFFSET =                       (4);
    #elif _CLOCKSPERUS == 8
      GUESSCAL = (SIGROW_OSCCAL20M0 + GUESS_20_16        );
      TUNED_CALIBRATION_OFFSET =                       (2);
    #elif _CLOCKSPERUS == 7
      GUESSCAL = (SIGROW_OSCCAL20M0 + GUESS_20_24        );
      TUNED_CALIBRATION_OFFSET =                       (2);
    #elif _CLOCKSPERUS == 6
      GUESSCAL = (SIGROW_OSCCAL20M0 + GUESS_20_24        );
      TUNED_CALIBRATION_OFFSET =                       (2);
    #elif _CLOCKSPERUS == 5
      GUESSCAL = (SIGROW_OSCCAL20M0 + GUESS_20_16        );
      TUNED_CALIBRATION_OFFSET =                       (4);
    #elif _CLOCKSPERUS == 4
      GUESSCAL = (SIGROW_OSCCAL20M0 + GUESS_20_16        );
      TUNED_CALIBRATION_OFFSET =                       (2);
    #elif _CLOCKSPERUS == 2
      GUESSCAL = (SIGROW_OSCCAL20M0 + GUESS_20_16        );
      TUNED_CALIBRATION_OFFSET =                       (2);
    #elif _CLOCKSPERUS == 1
      GUESSCAL = (SIGROW_OSCCAL20M0 + GUESS_20_16        );
      TUNED_CALIBRATION_OFFSET =                       (2);
    #endif
  } else if(_osccfg == 0) {
    #if _CLOCKSPERUS >= 30
      GUESSCAL =                                       (-1);
      TUNED_CALIBRATION_OFFSET =                       (-1);
    #elif _CLOCKSPERUS == 25
      GUESSCAL = (SIGROW_OSCCAL16M0 + GUESS_16_25        );
      TUNED_CALIBRATION_OFFSET =                       (5);
    #elif _CLOCKSPERUS == 24
      GUESSCAL = (SIGROW_OSCCAL16M0 + GUESS_16_24        );
      TUNED_CALIBRATION_OFFSET =                       (4);
    #elif _CLOCKSPERUS == 20 // or 5 MHz
      GUESSCAL = (SIGROW_OSCCAL16M0 + GUESS_16_20        );
      TUNED_CALIBRATION_OFFSET =                       (3);
    #elif _CLOCKSPERUS == 16 // or 8 or 4 MHz
      GUESSCAL = (SIGROW_OSCCAL16M0                      );
      TUNED_CALIBRATION_OFFSET =                       (2);
    #elif _CLOCKSPERUS == 14 // or 8 or 4 MHz
      GUESSCAL = (AIGROW_OSCCAL16M0 + GUESS_16_14        );
      TUNED_CALIBRATION_OFFSET =                       (1);
    #elif _CLOCKSPERUS == 12
      GUESSCAL = (SIGROW_OSCCAL16M0 + GUESS_16_12        );
      TUNED_CALIBRATION_OFFSET =                       (0);
    #elif _CLOCKSPERUS == 10
      GUESSCAL = (SIGROW_OSCCAL16M0 + GUESS_16_20        );
      TUNED_CALIBRATION_OFFSET =                       (4);
    #elif _CLOCKSPERUS == 8
      GUESSCAL = (SIGROW_OSCCAL16M0                      );
      TUNED_CALIBRATION_OFFSET =                       (2);
    #elif _CLOCKSPERUS == 7
      GUESSCAL = (SIGROW_OSCCAL16M0 + GUESS_20_14        );
      TUNED_CALIBRATION_OFFSET =                       (2);
    #elif _CLOCKSPERUS == 6
      GUESSCAL = (SIGROW_OSCCAL16M0 + GUESS_16_12        );
      TUNED_CALIBRATION_OFFSET =                       (2);
    #elif _CLOCKSPERUS == 5
      GUESSCAL = (SIGROW_OSCCAL16M0 + GUESS_16_20        );
      TUNED_CALIBRATION_OFFSET =                       (4);
    #elif _CLOCKSPERUS == 4
      GUESSCAL = (SIGROW_OSCCAL16M0                      );
      TUNED_CALIBRATION_OFFSET =                       (2);
    #elif _CLOCKSPERUS == 2
      GUESSCAL = (SIGROW_OSCCAL16M0                      );
      TUNED_CALIBRATION_OFFSET =                       (2);
    #elif _CLOCKSPERUS == 1
      GUESSCAL = (SIGROW_OSCCAL16M0                      );
      TUNED_CALIBRATION_OFFSET =                       (2);
    #endif
  }
#endif
// if no prescaling needed, remember we need to turn it off!
