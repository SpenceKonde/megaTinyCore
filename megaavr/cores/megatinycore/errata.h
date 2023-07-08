#ifndef _ERRATA_H
#define _ERRATA_H

#if defined(__AVR_Tiny_0__)
  /* None of these have ever gotten a die rev that fixed bugs - the only "erratum"
  *  that they fixed involved something written during factory cal.
  */
  #if _AVR_FLASH == 16  || _AVR_FLASH == 8 /* 1604, 1606, 1607, 804, 806, 807 */

    #define ERRATA_CCL_OUTEN           (-1)
    #define ERRATA_CCL_DLATCH          (-1)
    #define ERRATA_RTC_PITANDRTC       (-1)
    #define ERRATA_CCL_PROTECTION      (-1)
    #define ERRATA_USART_ONEWIRE_PINS  (-1)
    #define ERRATA_USART_WAKE          (-1)
    #define ERRATA_ADC_FREERUN         (-1)
    #define ERRATA_TCA_RESTART         (-1)
    #define ERRATA_TCB_CCMP            (-1)
    #define ERRATA_TCB_SYNCUPD         (-1)
    #define ERRATA_TCB_PULSELEN        (-1)
  #else /* 202, 204, 402, 404, 406 */
    #define ERRATA_CCL_OUTEN           (-1)
    #define ERRATA_CCL_DLATCH          (-1)
    #define ERRATA_CCL_PROTECTION      (-1)
    #define ERRATA_ADC_FREERUN         (-1)
    #define ERRATA_RTC_PITANDRTC       (-1)
    #define ERRATA_TCA_RESTART         (-1)
    #define ERRATA_TCB_CCMP            (-1)
    #define ERRATA_TCB_SYNCUPD         (-1)
    #define ERRATA_TCB_PULSELEN        (-1)
    #define ERRATA_USART_ONEWIRE_PINS  (-1)
    #define ERRATA_USART_WAKE          (-1)
  #endif
  #define ERRATA_USART_FRAMING          (0)
  #define ERRATA_TCD_ASYNC_COUNTPSC  (-128)
  #define ERRATA_TCD_HALTANDRESTART  (-128)
  #define ERRATA_TCD_AUTOUPDATE      (-128)
  #define ERRATA_TWI_ASSORTED           (0)
  #define ERRATA_USART_ISFIF            (0)
  #define ERRATA_ADC_WCMP               (0)
  #define ERRATA_AC_FALSETRIGGER        (0)
  #define ERRATA_AC_INTFLAG             (0)
  #define ERRATA_ADC_ASDVSAMPLEN        (0)
  #define ERRATA_ADC_FLAGSRESH          (0)
  #define ERRATA_TWI_PINS               (0) /* Not in any errata? Huh? */
  #define ERRATA_TWI_FLUSH              (0) /* Flush bug hasn't showed up on any 1-series errata */
  #define ERRATA_ADC_LOWLAT          (-128) /* There's no LOWLAT here */
#elif defined(__AVR_Tiny_1__)
  #if _AVR_FLASH == 32 /* 3216 and 3217
  * Them lucky 32k parts, they get all the breaks! Not only did they start with a less buggy design overall,
  * they also have given it a die rev (to Rev. C - Rev. B was never shipped)
  */
    #define ERRATA_CCL_OUTEN            (2)
    #define ERRATA_CCL_DLATCH           (2)
    #define ERRATA_RTC_PITANDRTC        (2)
    #define ERRATA_AC_FALSETRIGGER      (0)
    #define ERRATA_AC_INTFLAG           (0)
    #define ERRATA_USART_ISFIF         (-2)
    #define ERRATA_USART_ONEWIRE_PINS  (-1)
    #define ERRATA_USART_FRAMING        (2)
    #define ERRATA_USART_WAKE          (-2)
    #define ERRATA_ADC_FREERUN          (2)
    #define ERRATA_AC_INTFLAG           (0)
    #define ERRATA_TCD_AUTOUPDATE       (0)
    #define ERRATA_ADC_ASDVSAMPLEN      (0)
    #define ERRATA_ADC_FLAGSRESH        (0)
    #define ERRATA_ADC_WCMP             (0)
    #define ERRATA_TWI_ASSORTED         (0)
    #define ERRATA_CCL_PROTECTION      (-1)
    #define ERRATA_TCA_RESTART         (-1)
    #define ERRATA_TCB_CCMP            (-1)
    #define ERRATA_TCB_SYNCUPD         (-1)
    #define ERRATA_TCB_PULSELEN        (-1)
    #define ERRATA_TCD_ASYNC_COUNTPSC  (-1)
    #define ERRATA_TCD_HALTANDRESTART  (-1)
  #elif _AVR_FLASH == 16  /* 1614, 1616, 1617
  * The first and the worst - 34 errata are listed officially.
  */
    #define ERRATA_FUSE_OSCLOCK        (-1)
    #define ERRATA_CCL_OUTEN           (-1)
    #define ERRATA_CCL_DLATCH          (-1)
    #define ERRATA_RTC_PITANDRTC       (-1)
    #define ERRATA_AC_FALSETRIGGER      (1)
    #define ERRATA_AC_INTFLAG          (-1)
    #define ERRATA_USART_ISFIF          (0)
    #define ERRATA_USART_ONEWIRE_PINS  (-1)
    #define ERRATA_USART_FRAMING       (-1)
    #define ERRATA_USART_WAKE           (0)
    #define ERRATA_TCD_AUTOUPDATE       (1)
    #define ERRATA_ADC_FREERUN         (-1)
    #define ERRATA_ADC_ASDVSAMPLEN     (-1)
    #define ERRATA_ADC_FLAGSRESH       (-1)
    #define ERRATA_ADC_WCMP            (-1)
    #define ERRATA_TWI_ASSORTED        (-1)
    #define ERRATA_CCL_PROTECTION      (-1)
    #define ERRATA_TCA_RESTART         (-1)
    #define ERRATA_TCB_CCMP            (-1)
    #define ERRATA_TCB_SYNCUPD         (-1)
    #define ERRATA_TCB_PULSELEN        (-1)
    #define ERRATA_TCD_ASYNC_COUNTPSC  (-1)
    #define ERRATA_TCD_HALTANDRESTART  (-1)
  #elif _AVR_FLASH == 8 || _AVR_PINCOUNT == 24 /* 814, 816, 817, 417
  * These are the second worst off of the 1-series.
  */
    #define ERRATA_CCL_OUTEN           (-1)
    #define ERRATA_CCL_DLATCH          (-1)
    #define ERRATA_RTC_PITANDRTC       (-1)
    #define ERRATA_AC_FALSETRIGGER     (-1)
    #define ERRATA_AC_INTFLAG          (-1)
    #define ERRATA_USART_ISFIF         (-1)
    #define ERRATA_USART_ONEWIRE_PINS  (-1)
    #define ERRATA_USART_FRAMING       (-1)
    #define ERRATA_USART_WAKE          (-1)
    #define ERRATA_ADC_FREERUN         (-1)
    #define ERRATA_AC_INTFLAG          (-1)
    #define ERRATA_TCD_AUTOUPDATE      (-1)
    #define ERRATA_ADC_ASDVSAMPLEN      (0)
    #define ERRATA_ADC_FLAGSRESH        (0)
    #define ERRATA_ADC_WCMP            (-1)
    #define ERRATA_TWI_ASSORTED        (-1)
    #define ERRATA_CCL_PROTECTION      (-1)
    #define ERRATA_TCA_RESTART         (-1)
    #define ERRATA_TCB_CCMP            (-1)
    #define ERRATA_TCB_SYNCUPD         (-1)
    #define ERRATA_TCB_PULSELEN        (-1)
    #define ERRATA_TCD_ASYNC_COUNTPSC  (-1)
    #define ERRATA_TCD_HALTANDRESTART  (-1)
  #else /* 212, 214, 412, 414, 416.
  * There's a Rev. C, but they didn't fix any bugs \o/
  * But Rev B fixed a number of them at least.
  */
    #define ERRATA_CCL_OUTEN           (-1)
    #define ERRATA_CCL_DLATCH          (-1)
    #define ERRATA_RTC_PITANDRTC       (-1)
    #define ERRATA_AC_FALSETRIGGER      (1)
    #define ERRATA_USART_ISFIF          (0)
    #define ERRATA_USART_ONEWIRE_PINS  (-1)
    #define ERRATA_USART_FRAMING       (-1)
    #define ERRATA_USART_WAKE          (-1)
    #define ERRATA_ADC_FREERUN          (1)
    #define ERRATA_AC_INTFLAG           (1)
    #define ERRATA_TCD_AUTOUPDATE       (0)
    #define ERRATA_ADC_ASDVSAMPLEN      (1)
    #define ERRATA_ADC_FLAGSRESH        (1)
    #define ERRATA_ADC_WCMP             (1)
    #define ERRATA_TWI_ASSORTED         (1)
    #define ERRATA_TCB_ICFPWM           (1)
    #define ERRATA_CCL_PROTECTION      (-1)
    #define ERRATA_TCA_RESTART         (-1)
    #define ERRATA_TCB_CCMP            (-1)
    #define ERRATA_TCB_SYNCUPD         (-1)
    #define ERRATA_TCB_PULSELEN        (-1)
    #define ERRATA_TCD_ASYNC_COUNTPSC  (-1)
    #define ERRATA_TCD_HALTANDRESTART  (-1)
  #endif
  #define ERRATA_TWI_PINS               (0) /* Not in any errata? Huh? */
  #define ERRATA_TWI_FLUSH              (0) /* Flush bug hasn't showed up on any 1-series errata */
  #define ERRATA_ADC_LOWLAT          (-128) /* There's no LOWLAT here */
#elif defined(__AVR_Tiny_2__)
  #define ERRATA_TCB_ICFPWM             (0)
  #define ERRATA_CCL_OUTEN              (0)
  #define ERRATA_CCL_DLATCH             (0)
  #define ERRATA_RTC_PITANDRTC          (0)
  #define ERRATA_AC_FALSETRIGGER        (0)
  #define ERRATA_USART_FRAMING          (0)
  #define ERRATA_USART_WAKE             (0)
  #define ERRATA_ADC_FREERUN            (0)
  #define ERRATA_AC_INTFLAG             (0)
  #define ERRATA_TCD_AUTOUPDATE         (0)
  #define ERRATA_ADC_ASDVSAMPLEN        (0)
  #define ERRATA_ADC_FLAGSRESH          (0)
  #define ERRATA_ADC_WCMP               (0)
  #define ERRATA_TWI_ASSORTED           (0)
  #define ERRATA_TCB_ICFPWM             (0)
  #define ERRATA_TCA_RESTART            (0)
  #define ERRATA_TCB_SYNCUPD            (0)
  #define ERRATA_TCB_PULSELEN           (0)
  #define ERRATA_TCD_ASYNC_COUNTPSC     (0)
  #define ERRATA_TCD_HALTANDRESTART     (0)
  #define ERRATA_TWI_PINS               (0) /* Not in any errata? Huh? */
  #define ERRATA_TWI_FLUSH              (0) /* Flush bug hasn't showed up on any 2-series errata */
  #if _AVR_FLASH == 32
    #define ERRATA_USART_ONEWIRE_PINS   (0)
    #define ERRATA_CCL_PROTECTION      (-1)
    #define ERRATA_TCA_RESTART         (-1)
    #define ERRATA_TCB_CCMP            (-1)
    #define ERRATA_USART_ISFIF         (-1)
    #define ERRATA_ADC_LOWLAT          (-1)
  #elif _AVR_FLASH == 16
    /* 16k parts also have excessive current consumption in power-down mode */
    #define ERRATA_USART_ONEWIRE_PINS  (-1)
    #define ERRATA_CCL_PROTECTION      (-1)
    #define ERRATA_TCA_RESTART         (-1)
    #define ERRATA_TCB_CCMP            (-1)
    #define ERRATA_USART_ISFIF         (-1)
    #define ERRATA_ADC_LOWLAT          (-1)
  #else /* 8k and 4k 2-series share die */
    #define ERRATA_USART_ONEWIRE_PINS   (0)
    #define ERRATA_CCL_PROTECTION      (-1)
    #define ERRATA_TCA_RESTART         (-1)
    #define ERRATA_TCB_CCMP            (-1)
    #define ERRATA_USART_ISFIF         (-1)
    #define ERRATA_ADC_LOWLAT          (-1)
  #endif
  #define ERRATA_TCD_ASYNC_COUNTPSC  (-128)
  #define ERRATA_TCD_AUTOUPDATE      (-128)
  #define ERRATA_TCD_HALTANDRESTART  (-128)
#endif

inline bool __attribute__((always_inline)) checkErrata(int8_t errata) {
  if (!(errata & 0x7F)) { // 0 or(-128)
    return 0;
  }
  #if defined(__AVR_Tiny_1__) && _AVR_FLASH == 32
    // These parts gained an 2 errata
    uint8_t revid = SYSCFG.REVID;
    if (errata == -2) {
      return revid > 2;
    }
  #else
    uint8_t revid = SYSCFG.REVID;
  #endif
  return revid < errata;
}
#endif
