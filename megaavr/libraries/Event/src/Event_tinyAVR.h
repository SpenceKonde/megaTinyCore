/* This file is ONLY included by Event.h and should never be included by any other code under any circumstances.
 * It is loaded at the start of Event.h so that the same code can be used even if the part is a tinyAVR 0/1-series with wacky event system.
 * Thia does a lot for tinyAVR 0/1-series, very little for 2-series, and nothing for anything else.
 */
// *INDENT-OFF*
#if !defined(EVENT_H)
  #error "This should only be included as part of Event.h"
#endif

#ifndef EVENT_TINYAVR_H
  #define EVENT_TINYAVR_H
  /* The tinyAVRs fit into three classes:
   * 0-series and 1-series up to 8k - These have the sync and async channels.
   * 1-series 16k and 32k - These have added featuees including a couple of extra generators, and the weird sync/async channels.
   * 2-series - these are almost identical to AVR Dx, with normal channels
   */

  #if defined(MEGATINYCORE)
    #if (MEGATINYCORE_SERIES != 2)
      #define TINY_0_OR_1_SERIES
    #endif

    #if MEGATINYCORE_SERIES == 1 && PROGMEM_SIZE > 8192
      /* The elite tinyAVR 1-series parts with 16k or more of flash were blessed with peripherals
       * well beyond what lesser 1-series parts got (3 AC's, a second ADC, and some other stuff
       * Microchip didn't do a good job of advertising this difference, nor do they appear to account for it in pricing.
       * The premium on 1607 over an 807 (which adds only 8k flash and 512b ram) is 3 cents in 5k+ qty, 5 cents in small quantity
       * The premium on 1617 over an 817 (which adds 8k flash, 1.5k ram, 2 comparators, external vref, plus a whole second ADC) is... 3 cents in 5k+ quantity, 2 cents in small quantity (O_o).
       * The premium on a 3217 over a 1617 (which adds only the flash and nothing else)?... 6 cents in 5k+ qty (9 cents in small quantity)
       * The premium on a 1627 over a 827  (which adds 8k flash and 1k ram) is 4 cents in 5k qty, 6 in small quantity.
       * The premium on a 3227 over a 1627 (which adds 16k flash and 1k ram) is 5 cents in 5k qty, 10 cents in small quantity.
       * If curious, the 5k price at time of writing this for th 0/1/2-series 24-pin parts were $0.84
       * These are for -MNR (0/1-series, -40 to 105C packaged on a reel) or -MUR (2-series, -40 to 85C, packed on a reel) in QFN-24 package (the only one they're made in)
       * The extended temp range premiums on the other hand (16k parts) are 1 cent, 3 cents, and 8 cents @ 5k qty. (tray packaging was compared as 2 series doesn't yet have extended temp available on reels)
       * In other words, the pricing accounts for the flash size, but not the added features on the 1-series.
       * As one would expect the premium on 125C extended temp parts (the F-spec) is larger if the normal version of the part is 85C (U-spec) than 105C (N-spec).
       * One always wonders how this splits between the "I think I people are willing to pay that much more; we're getting great yield and some of the normal chips will run at
       * F-spec speeds" and "Only XX % of our chips will meet F-spec, and more than YY% of customers would prefer them but most don't *need* them; YY > XX,
       * so we're going to choose the prices such that we convince approximately (YY - XX)/YY % of those customers to go to the normal spec, so we avoid having to sell F-spec parts
       * lower temp ranges and having a surplus of parts that won't meet F-spec." (if XX = 20, that is 20% of parts that pass tests at normal temp will pass at 125C, if there was demand for
       * 200000 F-spec parts and 500000 N-spec parts per month, They'd either have a shortfall of 100000 F-spec or an excess of 300000 N-spec per month. So this premium tells you something
       * about their yields. The fact that the premium on the AVR128DA64 is 12 cents for extended temp, while that on the DB is AVR128DB64 is 21 cents - while the premium for the DB vs the DA
       * at normal temperature spec is 10 cents, implies that either the ratio of demand for extended vs normal DBs is higher than DA's, or the yield is lower.
       */
       #define TINY_1_16K_PLUS
    #endif

    #if (defined(MEGATINYCORE) && MEGATINYCORE_SERIES == 2)
      #define TINY_2_SERIES
    #endif
  #endif
  /* Because they have two kinds of channels, we have to do some work here   *
   * to make them look the same to the rest of the library.                  */
  #if defined(MEGATINYCORE) && MEGATINYCORE_SERIES !=2
    #if defined(EVSYS_SYNCCH0)
      #define EventSync0     Event0
      #define EVSYS_CHANNEL0 EVSYS_SYNCCH0
    #endif
    #if defined(EVSYS_SYNCCH1) && MEGATINYCORE_SERIES == 1
      #define EventSync1     Event1
      #define EVSYS_CHANNEL1 EVSYS_SYNCCH1
    #endif
    #if defined(EVSYS_ASYNCCH0)
      #define EventAsync0    Event2
      #define EVSYS_CHANNEL2 EVSYS_ASYNCCH0
    #endif
    #if defined(EVSYS_ASYNCCH1) && MEGATINYCORE_SERIES == 1
      #define EventAsync1    Event3
      #define EVSYS_CHANNEL3 EVSYS_ASYNCCH1
    #endif
    #if defined(EVSYS_ASYNCCH2)
      #define EventAsync2    Event4
      #define EVSYS_CHANNEL4 EVSYS_ASYNCCH2
    #endif
    #if defined(EVSYS_ASYNCCH3) && MEGATINYCORE_SERIES == 1
      #define EventAsync3    Event5
      #define EVSYS_CHANNEL5 EVSYS_ASYNCCH3
    #endif
    /* And fix names of users */
    #if defined(EVSYS_SYNCUSER0)
      #define EVSYS_USERTCA0CNTA          EVSYS_SYNCUSER0
    #endif
    #if defined(EVSYS_SYNCUSER1)
      #define EVSYS_USERUSART0IRDA        EVSYS_SYNCUSER1
    #endif
    #if defined(EVSYS_ASYNCUSER0)
      #define EVSYS_USERTCB0CAPT          EVSYS_ASYNCUSER0
    #endif
    #if defined(EVSYS_ASYNCUSER1)
      #define EVSYS_USERADC0START         EVSYS_ASYNCUSER1
    #endif
    #if defined(EVSYS_ASYNCUSER2)
      #define EVSYS_USERCCLLUT0A          EVSYS_ASYNCUSER2
    #endif
    #if defined(EVSYS_ASYNCUSER3)
      #define EVSYS_USERCCLLUT1A          EVSYS_ASYNCUSER3
    #endif
    #if defined(EVSYS_ASYNCUSER4)
      #define EVSYS_USERCCLLUT0B          EVSYS_ASYNCUSER4
    #endif
    #if defined(EVSYS_ASYNCUSER5)
      #define EVSYS_USERCCLLUT1B          EVSYS_ASYNCUSER5
    #endif
    #if defined(EVSYS_ASYNCUSER6)
      #define EVSYS_USERTCD0INPUTA        EVSYS_ASYNCUSER6
    #endif
    #if defined(EVSYS_ASYNCUSER7)
      #define EVSYS_USERTCD0INPUTB        EVSYS_ASYNCUSER7
    #endif
    #if defined(EVSYS_ASYNCUSER8)
      #define EVSYS_USEREVSYSEVOUTA       EVSYS_ASYNCUSER8
    #endif
    #if defined(EVSYS_ASYNCUSER9)
      #define EVSYS_USEREVSYSEVOUTB       EVSYS_ASYNCUSER9
    #endif
    #if defined(EVSYS_ASYNCUSER10)
      #define EVSYS_USEREVSYSEVOUTC       EVSYS_ASYNCUSER10
    #endif
    #if defined(EVSYS_ASYNCUSER11)
      #define EVSYS_USERTCB1CAPT          EVSYS_ASYNCUSER11
    #endif
    #if defined(EVSYS_ASYNCUSER12)
      #define EVSYS_USERADC1START         EVSYS_ASYNCUSER12
    #endif
  #endif
#endif //EVENT_TINYAVR_H
