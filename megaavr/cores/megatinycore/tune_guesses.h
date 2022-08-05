/* These are the gusses at cal change needed to get these speeds untuned     *
 * They are concatenated with the sigrow reference, so + or - operator works.*
 * Putting them all in one place allows better calculation of them as more   *
 * data is gathered and analyzed.
 *
 * Possible target frequencies supported by megaTinyCore are:
 *
 * 2-series, 20 MHz: 16,*20, 24, 25, 30, 32
 *           16 MHz: 14,*16, 20, 24, 25, 30
 * 1-series, 20 MHz: 14, 16,*20, 24, 25, 30
 *           16 MHz: 12, 14,*16, 20, 24, 25
 * Further, prescaled clocks derived by dividing those values by 2, 4, or 8 *
 * are supported, provided that the number number of clocks per microsecond *
 * is a whole number.
 *
 * Hence the overall list of speeds achievable is
 * 2-series, 20 MHz: 1, 2, 3, 4, 5,    8, 10, 12,     16, 20, 24, 25, 30, 32 MHz
 *           16 MHz: 1, 2, 3, 4, 5, 7, 8, 10, 12, 14, 16, 20, 24, 25, 30 MHz
 * 1-series, 20 MHz: 1, 2, 3, 4, 5, 7, 8, 10, 12, 14, 16, 20, 24, 25, 30 MHz
 *           16 MHz: 1, 2, 3, 4, 5, 7, 8, 10, 12, 14, 16, 20, 24, 25 MHz
 *
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
  #define GUESS_20_14  (-1)
  #define GUESS_16_16   (0)
  #define GUESS_16_14 (-18)
  #define MAX_TUNING  (127)
  #define HOME_TUNING_OFFSET   (1)
#else
  #define GUESS_20_30  (33)
  #define GUESS_20_25  (17)
  #define GUESS_20_24  (13)
  #define GUESS_20_16 (-14)
  #define GUESS_20_14 (-19)
  #define GUESS_16_25  (35)
  #define GUESS_16_24  (31)
  #define GUESS_16_20  (16)
  #define GUESS_16_14 (-14)
  #define GUESS_16_12 (-28)
  #define MAX_TUNING   (63)
  #define HOME_TUNING_OFFSET  (2)
#endif
/* Here we pick the correct one to use for the requested frequency.                       *
 * First TUNE_PRESCALE is determined to permit tuning to very low speeds, which is used if*
 * a tuned clock is requested at a speed requiring the tuned clock to be
 *ble) "
 * Note the _ - outside of this file, we don't want anyone to know about this */


#if _CLOCKSPERUS > 12
  #define TUNE_PRESCALE (0)
#elif _CLOCKSPERUS == 10 || _CLOCKSPERUS == 12 || _CLOCKSPERUS == 7
  #define TUNE_PRESCALE (1) //enable with 2x prescale
#elif _CLOCKSPERUS == 4 || _CLOCKSPERUS == 5 || _CLOCKSPERUS == 6
  #define TUNE_PRESCALE (3) //enable with 4x prescale
#elif _CLOCKSPERUS > 1 // This is the 2 (or 3) MHz case.
  #define TUNE_PRESCALE (5)
#elif _CLOCKSPERUS == 1
  #define TUNE_PRESCALE (7)
#else
  #define TUNE_PRESCALE (0)
#endif
uint8_t GUESSCAL;
uint8_t TUNED_CALIBRATION_OFFSET;

#if MEGATINYCORE_SERIES == 2
  if (_osccfg == 1) { //Don't worry, CLOCKSPERUS is constant and gets folded!
    if (_CLOCKSPERUS == 32) {
      GUESSCAL =       (SIGROW_OSCCAL20M0 + GUESS_20_32        );
      TUNED_CALIBRATION_OFFSET =                             (5);
    } else if (_CLOCKSPERUS == 30) {
      GUESSCAL =       (SIGROW_OSCCAL20M0 + GUESS_20_30        );
      TUNED_CALIBRATION_OFFSET =                             (4);
    } else if (_CLOCKSPERUS == 25) {
      GUESSCAL =       (SIGROW_OSCCAL20M0 + GUESS_20_25        );
      TUNED_CALIBRATION_OFFSET =                             (3);
    } else if (_CLOCKSPERUS == 24) {
      GUESSCAL =       (SIGROW_OSCCAL20M0 + GUESS_20_24        );
      TUNED_CALIBRATION_OFFSET =                             (2);
    } else if (_CLOCKSPERUS == 20 ) {// or 5 MHz
      GUESSCAL =       (SIGROW_OSCCAL20M0                      );
      TUNED_CALIBRATION_OFFSET =                             (1);
    } else if (_CLOCKSPERUS == 16 ) {// or 8 or 4 MHz
      GUESSCAL =       (SIGROW_OSCCAL20M0 + GUESS_20_16        );
      TUNED_CALIBRATION_OFFSET =                             (0);
    } else if (_CLOCKSPERUS == 12) {
      GUESSCAL =       (SIGROW_OSCCAL20M0 + GUESS_20_24        );
      TUNED_CALIBRATION_OFFSET =                             (2);
    } else if (_CLOCKSPERUS == 10) {
      GUESSCAL =       (SIGROW_OSCCAL20M0                      );
      TUNED_CALIBRATION_OFFSET =                             (1);
    } else if (_CLOCKSPERUS == 8) {
      GUESSCAL =       (SIGROW_OSCCAL20M0 + GUESS_20_16        );
      TUNED_CALIBRATION_OFFSET =                             (0);
    } else if (_CLOCKSPERUS == 6) {
      GUESSCAL =       (SIGROW_OSCCAL20M0 + GUESS_20_24        );
      TUNED_CALIBRATION_OFFSET =                             (2);
    } else if (_CLOCKSPERUS == 5) {
      GUESSCAL =       (SIGROW_OSCCAL20M0                      );
      TUNED_CALIBRATION_OFFSET =                             (1);
    } else if (_CLOCKSPERUS == 4) {
      GUESSCAL =       (SIGROW_OSCCAL20M0 + GUESS_20_16        );
      TUNED_CALIBRATION_OFFSET =                             (0);
    } else if (_CLOCKSPERUS == 2) {
      GUESSCAL =       (SIGROW_OSCCAL20M0 + GUESS_20_16        );
      TUNED_CALIBRATION_OFFSET =                             (0);
    } else if (_CLOCKSPERUS == 1) {
      GUESSCAL =       (SIGROW_OSCCAL20M0 + GUESS_20_16        );
      TUNED_CALIBRATION_OFFSET =                             (0);
    } else {
      TUNED_CALIBRATION_OFFSET =                            (-1);
    }
  } else if(_osccfg == 0) {
    if (_CLOCKSPERUS == 30) {
      GUESSCAL =                                            (-1);
      TUNED_CALIBRATION_OFFSET =                            (-1);
    } else if (_CLOCKSPERUS == 25) {
      GUESSCAL =       (SIGROW_OSCCAL16M0 + GUESS_16_25        );
      TUNED_CALIBRATION_OFFSET =                             (5);
    } else if (_CLOCKSPERUS == 24) {
      GUESSCAL =       (SIGROW_OSCCAL16M0 + GUESS_16_24        );
      TUNED_CALIBRATION_OFFSET =                             (4);
    } else if (_CLOCKSPERUS == 20 ) {// or 5 MHz
      GUESSCAL =       (SIGROW_OSCCAL16M0 + GUESS_16_20        );
      TUNED_CALIBRATION_OFFSET =                             (3);
    } else if (_CLOCKSPERUS == 16 ) {// or 8 or 4 MHz
      GUESSCAL =       (SIGROW_OSCCAL16M0                      );
      TUNED_CALIBRATION_OFFSET =                             (2);
    } else if (_CLOCKSPERUS == 14 ) {// or 8 or 4 MHz
      GUESSCAL =       (SIGROW_OSCCAL16M0 + GUESS_16_14        );
      TUNED_CALIBRATION_OFFSET =                             (1);
    } else if (_CLOCKSPERUS == 12) {
      GUESSCAL =       (SIGROW_OSCCAL16M0 + GUESS_16_24        );
      TUNED_CALIBRATION_OFFSET =                             (0);
    } else if (_CLOCKSPERUS == 10) {
      GUESSCAL =       (SIGROW_OSCCAL16M0 + GUESS_16_20        );
      TUNED_CALIBRATION_OFFSET =                             (4);
    } else if (_CLOCKSPERUS == 8) {
      GUESSCAL =       (SIGROW_OSCCAL16M0 + GUESS_16_16        );
      TUNED_CALIBRATION_OFFSET =                             (2);
    } else if (_CLOCKSPERUS == 7) {
      GUESSCAL =       (SIGROW_OSCCAL16M0 + GUESS_16_14        );
      TUNED_CALIBRATION_OFFSET =                             (2);
    } else if (_CLOCKSPERUS == 6) {
      GUESSCAL =       (SIGROW_OSCCAL16M0 + GUESS_16_24        );
      TUNED_CALIBRATION_OFFSET =                             (2);
    } else if (_CLOCKSPERUS == 5) {
      GUESSCAL =       (SIGROW_OSCCAL16M0 + GUESS_16_16        );
      TUNED_CALIBRATION_OFFSET =                             (4);
    } else if (_CLOCKSPERUS == 4) {
      GUESSCAL =       (SIGROW_OSCCAL16M0 + GUESS_16_16        );
      TUNED_CALIBRATION_OFFSET =                             (2);
    } else if (_CLOCKSPERUS == 2) {
      GUESSCAL =       (SIGROW_OSCCAL16M0 + GUESS_16_16        );
      TUNED_CALIBRATION_OFFSET =                             (2);
    } else if (_CLOCKSPERUS == 1) {
      GUESSCAL =       (SIGROW_OSCCAL16M0 + GUESS_16_16        );
      TUNED_CALIBRATION_OFFSET =                             (2);
    } else {
      TUNED_CALIBRATION_OFFSET =                            (-1);
    }
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
    if (_CLOCKSPERUS > 30) {
      GUESSCAL =                                            (-1);
      TUNED_CALIBRATION_OFFSET =                            (-1);
    } else if (_CLOCKSPERUS == 30) {
      GUESSCAL =       (SIGROW_OSCCAL20M0 + GUESS_20_30        );
      TUNED_CALIBRATION_OFFSET =                             (5);
    } else if (_CLOCKSPERUS == 25) {
      GUESSCAL =       (SIGROW_OSCCAL20M0 + GUESS_20_25        );
      TUNED_CALIBRATION_OFFSET =                             (4);
    } else if (_CLOCKSPERUS == 24) {
      GUESSCAL =       (SIGROW_OSCCAL20M0 + GUESS_20_24        );
      TUNED_CALIBRATION_OFFSET =                             (3);
    } else if (_CLOCKSPERUS == 20) {
      GUESSCAL =       (SIGROW_OSCCAL20M0                      );
      TUNED_CALIBRATION_OFFSET =                             (2);
    } else if (_CLOCKSPERUS == 16)  {
      GUESSCAL =       (SIGROW_OSCCAL20M0 + GUESS_20_16        );
      TUNED_CALIBRATION_OFFSET =                             (1);
    } else if (_CLOCKSPERUS == 14) {
      GUESSCAL =       (SIGROW_OSCCAL20M0 + GUESS_20_14        );
      TUNED_CALIBRATION_OFFSET =                             (0);
    } else if (_CLOCKSPERUS == 12) {
      GUESSCAL =       (SIGROW_OSCCAL20M0 + GUESS_20_24        );
      TUNED_CALIBRATION_OFFSET =                             (3);
    } else if (_CLOCKSPERUS == 10) {
      GUESSCAL =       (SIGROW_OSCCAL20M0                      );
      TUNED_CALIBRATION_OFFSET =                             (4);
    } else if (_CLOCKSPERUS == 8) {
      GUESSCAL =       (SIGROW_OSCCAL20M0 + GUESS_20_16        );
      TUNED_CALIBRATION_OFFSET =                             (2);
    } else if (_CLOCKSPERUS == 7) {
      GUESSCAL =       (SIGROW_OSCCAL20M0 + GUESS_20_24        );
      TUNED_CALIBRATION_OFFSET =                             (2);
    } else if (_CLOCKSPERUS == 6) {
      GUESSCAL =       (SIGROW_OSCCAL20M0 + GUESS_20_24        );
      TUNED_CALIBRATION_OFFSET =                             (2);
    } else if (_CLOCKSPERUS == 5) {
      GUESSCAL =       (SIGROW_OSCCAL20M0 + GUESS_20_16        );
      TUNED_CALIBRATION_OFFSET =                             (4);
    } else if (_CLOCKSPERUS == 4) {
      GUESSCAL =       (SIGROW_OSCCAL20M0 + GUESS_20_16        );
      TUNED_CALIBRATION_OFFSET =                             (2);
    } else if (_CLOCKSPERUS == 2) {
      GUESSCAL =       (SIGROW_OSCCAL20M0 + GUESS_20_16        );
      TUNED_CALIBRATION_OFFSET =                             (2);
    } else if (_CLOCKSPERUS == 1) {
      GUESSCAL =       (SIGROW_OSCCAL20M0 + GUESS_20_16        );
      TUNED_CALIBRATION_OFFSET =                             (2);
    } else {
      GUESSCAL =                                            (-1);
      TUNED_CALIBRATION_OFFSET =                            (-1);
    }
  } else if(_osccfg == 0) {
    if (_CLOCKSPERUS >= 30) {
      GUESSCAL =                                            (-1);
      TUNED_CALIBRATION_OFFSET =                            (-1);
    } else if (_CLOCKSPERUS == 25) {
      GUESSCAL =       (SIGROW_OSCCAL16M0 + GUESS_16_25        );
      TUNED_CALIBRATION_OFFSET =                             (5);
    } else if (_CLOCKSPERUS == 24) {
      GUESSCAL =       (SIGROW_OSCCAL16M0 + GUESS_16_24        );
      TUNED_CALIBRATION_OFFSET =                             (4);
    } else if (_CLOCKSPERUS == 20 ) {// or 5 MHz
      GUESSCAL =       (SIGROW_OSCCAL16M0 + GUESS_16_20        );
      TUNED_CALIBRATION_OFFSET =                             (3);
    } else if (_CLOCKSPERUS == 16 ) {// or 8 or 4 MHz
      GUESSCAL =       (SIGROW_OSCCAL16M0                      );
      TUNED_CALIBRATION_OFFSET =                             (2);
    } else if (_CLOCKSPERUS == 14 ) {// or 8 or 4 MHz
      GUESSCAL =       (SIGROW_OSCCAL16M0 + GUESS_16_14        );
      TUNED_CALIBRATION_OFFSET =                             (1);
    } else if (_CLOCKSPERUS == 12) {
      GUESSCAL =       (SIGROW_OSCCAL16M0 + GUESS_16_12        );
      TUNED_CALIBRATION_OFFSET =                             (0);
    } else if (_CLOCKSPERUS == 10) {
      GUESSCAL =       (SIGROW_OSCCAL16M0 + GUESS_16_20        );
      TUNED_CALIBRATION_OFFSET =                             (4);
    } else if (_CLOCKSPERUS == 8) {
      GUESSCAL =       (SIGROW_OSCCAL16M0                      );
      TUNED_CALIBRATION_OFFSET =                             (2);
    } else if (_CLOCKSPERUS == 7) {
      GUESSCAL =       (SIGROW_OSCCAL16M0 + GUESS_20_14        );
      TUNED_CALIBRATION_OFFSET =                             (2);
    } else if (_CLOCKSPERUS == 6) {
      GUESSCAL =       (SIGROW_OSCCAL16M0 + GUESS_16_12        );
      TUNED_CALIBRATION_OFFSET =                             (2);
    } else if (_CLOCKSPERUS == 5) {
      GUESSCAL =       (SIGROW_OSCCAL16M0 + GUESS_16_20        );
      TUNED_CALIBRATION_OFFSET =                             (4);
    } else if (_CLOCKSPERUS == 4) {
      GUESSCAL =       (SIGROW_OSCCAL16M0                      );
      TUNED_CALIBRATION_OFFSET =                             (2);
    } else if (_CLOCKSPERUS == 2) {
      GUESSCAL =       (SIGROW_OSCCAL16M0                      );
      TUNED_CALIBRATION_OFFSET =                             (2);
    } else if (_CLOCKSPERUS == 1) {
      GUESSCAL =       (SIGROW_OSCCAL16M0                      );
      TUNED_CALIBRATION_OFFSET =                             (2);
    } else {
      GUESSCAL =                                            (-1);
      TUNED_CALIBRATION_OFFSET =                            (-1);
    }
  }
#endif
// if no prescaling needed, remember we need to turn it off!
