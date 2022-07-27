/* This file is ONLY included by Logic.h and should never be included by any other code under any circumstances.
 * It is loaded at the start of Logic.h so that the same code can be used even if the part is a tinyAVR 0/1-series with wacky event system.
 * Thia does a lot for tinyAVR 0/1-series, very little for 2-series, and nothing for anything else.
 */
#if !defined(LOGIC_H)
  #error "This should only be included as part of Logic.h"
#endif

// *INDENT-OFF* astyle hates how we formatted this.
// I mean, I do too, but I hated all the alternatives we tried even more.
// Readable code always takes priority over formatting dogma. -Spence

#ifndef LOGIC_EMUMS_H
#define LOGIC_PARTS_H

struct Logic::CCLBlock {
  const uint8_t number;
  const uint8_t input0_bm;
  const uint8_t input1_bm;
  const uint8_t input2_bm;
  const uint8_t output_bm;
  const uint8_t output_alt_bm;
  PORT_t &PORT_IN;
  PORT_t &PORT_OUT;
  PORT_t &PORT_ALT_OUT;
  volatile register8_t &SEQCTRL;
  volatile register8_t &LUTCTRLA;
  volatile register8_t &LUTCTRLB;
  volatile register8_t &LUTCTRLC;
  volatile register8_t &TRUTH;
};
// *INDENT-OFF* // astyle wants this to be unreadable
static const struct Logic::CCLBlock blocks[] = {
  #if defined(__AVR_ATtiny202__) || defined(__AVR_ATtiny402__) ||  \
    defined(__AVR_ATtiny204__) || defined(__AVR_ATtiny404__) ||  \
    defined(__AVR_ATtiny804__) || defined(__AVR_ATtiny1604__) || \
    defined(__AVR_ATtiny212__) || defined(__AVR_ATtiny412__) ||  \
    defined(__AVR_ATtiny214__) || defined(__AVR_ATtiny414__) ||  \
    defined(__AVR_ATtiny814__) || defined(__AVR_ATtiny1614__)
    {
      0,
      PIN0_bm, PIN1_bm, PIN2_bm, PIN6_bm, 0,
      PORTA, PORTA, PORTA,
      CCL.SEQCTRL0, CCL.LUT0CTRLA, CCL.LUT0CTRLB, CCL.LUT0CTRLC, CCL.TRUTH0,
    },
    {
      1,
      0, 0, 0, PIN7_bm, 0,
      PORTA, PORTA, PORTA,
      CCL.SEQCTRL0, CCL.LUT1CTRLA, CCL.LUT1CTRLB, CCL.LUT1CTRLC, CCL.TRUTH1,
    },
  #endif
  #if defined(__AVR_ATtiny406__)  || defined(__AVR_ATtiny806__) || \
      defined(__AVR_ATtiny1606__) ||                               \
      defined(__AVR_ATtiny416__)  || defined(__AVR_ATtiny816__) || \
      defined(__AVR_ATtiny1616__) || defined(__AVR_ATtiny3216__)
    #define PORTMUX_CCL PORTMUX.CTRLA
    #define PORTMUX_ALTOUT_bm (1 << (block.number + 4))
    {
      0,
      PIN0_bm, PIN1_bm, PIN2_bm, PIN6_bm, PIN4_bm,
      PORTA, PORTA, PORTB,
      CCL.SEQCTRL0, CCL.LUT0CTRLA, CCL.LUT0CTRLB, CCL.LUT0CTRLC, CCL.TRUTH0,
    },
    {
      1,
      PIN3_bm, 0, 0, PIN7_bm, PIN1_bm,
      PORTC, PORTA, PORTC,
      CCL.SEQCTRL0, CCL.LUT1CTRLA, CCL.LUT1CTRLB, CCL.LUT1CTRLC, CCL.TRUTH1,
    },
  #endif
  #if defined(__AVR_ATtiny807__) || defined(__AVR_ATtiny1607__) || \
      defined(__AVR_ATtiny417__) || defined(__AVR_ATtiny817__) ||  \
      defined(__AVR_ATtiny1617__) || defined(__AVR_ATtiny3217__)
    #define PORTMUX_CCL PORTMUX.CTRLA
    #define PORTMUX_ALTOUT_bm (1 << (block.number + 4))
    {
      0,
      PIN0_bm, PIN1_bm, PIN2_bm, PIN6_bm, PIN4_bm,
      PORTA, PORTA, PORTB,
      CCL.SEQCTRL0, CCL.LUT0CTRLA, CCL.LUT0CTRLB, CCL.LUT0CTRLC, CCL.TRUTH0,
    },
    {
      1,
      PIN3_bm, PIN4_bm, PIN5_bm, PIN7_bm, PIN1_bm,
      PORTC, PORTA, PORTC,
      CCL.SEQCTRL0, CCL.LUT1CTRLA, CCL.LUT1CTRLB, CCL.LUT1CTRLC, CCL.TRUTH1,
    },
  #endif
  #if defined(__AVR_ATtiny424__)  || defined(__AVR_ATtiny824__)  ||  \
      defined(__AVR_ATtiny1624__) || defined(__AVR_ATtiny3224__)
    #define PORTMUX_CCL PORTMUX.CCLROUTEA
    #define PORTMUX_ALTOUT_bm (1 << block.number)
    {
      0,
      PIN0_bm, PIN1_bm, PIN2_bm, PIN6_bm, 0,
      PORTA, PORTA, PORTA,
      CCL.SEQCTRL0, CCL.LUT0CTRLA, CCL.LUT0CTRLB, CCL.LUT0CTRLC, CCL.TRUTH0,
    },
    {
      1,
      0, 0, 0, PIN7_bm, 0,
      PORTA, PORTA, PORTA,
      CCL.SEQCTRL0, CCL.LUT1CTRLA, CCL.LUT1CTRLB, CCL.LUT1CTRLC, CCL.TRUTH1,
    },
    {
      2,
      PIN0_bm, PIN1_bm, PIN2_bm, PIN3_bm, 0,
      PORTB, PORTB, PORTA,
      CCL.SEQCTRL1, CCL.LUT2CTRLA, CCL.LUT2CTRLB, CCL.LUT2CTRLC, CCL.TRUTH2,
    },
    {
      3,
      0, 0, 0, 0, PIN5_bm,
      PORTC, PORTA, PORTA,
      CCL.SEQCTRL1, CCL.LUT3CTRLA, CCL.LUT3CTRLB, CCL.LUT3CTRLC, CCL.TRUTH3,
    },
  #endif
  #if defined(__AVR_ATtiny426__)  || defined(__AVR_ATtiny826__) || \
      defined(__AVR_ATtiny1616__) || defined(__AVR_ATtiny3216__)
    #define PORTMUX_CCL PORTMUX.CCLROUTEA
    #define PORTMUX_ALTOUT_bm (1 << block.number)
    {
      0,
      PIN0_bm, PIN1_bm, PIN2_bm, PIN6_bm, PIN4_bm,
      PORTA, PORTA, PORTB,
      CCL.SEQCTRL0, CCL.LUT0CTRLA, CCL.LUT0CTRLB, CCL.LUT0CTRLC, CCL.TRUTH0,
    },
    {
      1,
      PIN3_bm, 0, 0, PIN7_bm, PIN1_bm,
      PORTC, PORTA, PORTC,
      CCL.SEQCTRL0, CCL.LUT1CTRLA, CCL.LUT1CTRLB, CCL.LUT1CTRLC, CCL.TRUTH1,
    },
    {
      2,
      PIN0_bm, PIN1_bm, PIN2_bm, PIN3_bm, 0,
      PORTB, PORTB, PORTA,
      CCL.SEQCTRL1, CCL.LUT2CTRLA, CCL.LUT2CTRLB, CCL.LUT2CTRLC, CCL.TRUTH2,
    },
    {
      3,
      PIN0_bm, PIN1_bm, PIN2_bm, 0, PIN5_bm,
      PORTC, PORTA, PORTA,
      CCL.SEQCTRL1, CCL.LUT3CTRLA, CCL.LUT3CTRLB, CCL.LUT3CTRLC, CCL.TRUTH3,
    },
  #endif
  #if defined(__AVR_ATtiny427__) || defined(__AVR_ATtiny827__) ||  \
    defined(__AVR_ATtiny1617__)  || defined(__AVR_ATtiny3217__)
    #define PORTMUX_CCL PORTMUX.CCLROUTEA
    #define PORTMUX_ALTOUT_bm (1 << block.number)
    {
      0,
      PIN0_bm, PIN1_bm, PIN2_bm, PIN6_bm, PIN4_bm,
      PORTA, PORTA, PORTB,
      CCL.SEQCTRL0, CCL.LUT0CTRLA, CCL.LUT0CTRLB, CCL.LUT0CTRLC, CCL.TRUTH0,
    },
    {
      1,
      PIN3_bm, PIN4_bm, PIN5_bm, PIN7_bm, PIN1_bm,
      PORTC, PORTA, PORTC,
      CCL.SEQCTRL0, CCL.LUT1CTRLA, CCL.LUT1CTRLB, CCL.LUT1CTRLC, CCL.TRUTH1,
    },
    {
      2,
      PIN0_bm, PIN1_bm, PIN2_bm, PIN3_bm, PIN6_bm,
      PORTB, PORTB, PORTB,
      CCL.SEQCTRL1, CCL.LUT2CTRLA, CCL.LUT2CTRLB, CCL.LUT2CTRLC, CCL.TRUTH2,
    },
    {
      3,
      PIN0_bm, PIN1_bm, PIN2_bm, PIN4_bm, PIN5_bm,
      PORTC, PORTC, PORTA,
      CCL.SEQCTRL1, CCL.LUT3CTRLA, CCL.LUT3CTRLB, CCL.LUT3CTRLC, CCL.TRUTH3,
    },
  #endif
  #if defined(__AVR_AVR64DD20__)  || defined(__AVR_AVR64DD14__)  ||  \
      defined(__AVR_AVR32DD20__)  || defined(__AVR_AVR32DD14__)  ||  \
      defined(__AVR_AVR16DD20__)  || defined(__AVR_AVR16DD14__)
    #define PORTMUX_CCL PORTMUX.CCLROUTEA
    #define PORTMUX_ALTOUT_bm (1 << block.number)
    {
      0,
      PIN0_bm, PIN1_bm, PIN2_bm, PIN3_bm, PIN6_bm,
      PORTA, PORTA, PORTA,
      CCL.SEQCTRL0, CCL.LUT0CTRLA, CCL.LUT0CTRLB, CCL.LUT0CTRLC, CCL.TRUTH0,
    },
    {
      1,
      0, PIN1_bm, PIN2_bm, PIN3_bm, 0,
      PORTC, PORTC, PORTC,
      CCL.SEQCTRL0, CCL.LUT1CTRLA, CCL.LUT1CTRLB, CCL.LUT1CTRLC, CCL.TRUTH1,
    },
    {
      2,
      0, 0, 0, 0, PIN6_bm,
      PORTD, PORTD, PORTD,
      CCL.SEQCTRL1, CCL.LUT2CTRLA, CCL.LUT2CTRLB, CCL.LUT2CTRLC, CCL.TRUTH2,
    },
    {
      3,
      0, 0, 0, 0, 0,
      PORTA, PORTA, PORTA,
      CCL.SEQCTRL1, CCL.LUT3CTRLA, CCL.LUT3CTRLB, CCL.LUT3CTRLC, CCL.TRUTH3,
    },
  #endif
  #if defined(__AVR_ATmega808__)  || defined(__AVR_ATmega1608__) ||  \
      defined(__AVR_ATmega3208__) || defined(__AVR_ATmega4808__) ||  \
      defined(__AVR_AVR128DA32__) || defined(__AVR_AVR128DA28__) ||  \
      defined(__AVR_AVR64DA32__)  || defined(__AVR_AVR64DA28__)  ||  \
      defined(__AVR_AVR32DA32__)  || defined(__AVR_AVR32DA28__)  ||  \
      defined(__AVR_AVR128DB32__) || defined(__AVR_AVR128DB28__) ||  \
      defined(__AVR_AVR64DB32__)  || defined(__AVR_AVR64DB28__)  ||  \
      defined(__AVR_AVR32DB32__)  || defined(__AVR_AVR32DB28__)  ||  \
      defined(__AVR_AVR64DD32__)  || defined(__AVR_AVR64DD28__)  ||  \
      defined(__AVR_AVR32DD32__)  || defined(__AVR_AVR32DD28__)  ||  \
      defined(__AVR_AVR16DD32__)  || defined(__AVR_AVR16DD28__)  ||  \
      defined(__AVR_AVR64EA32__)  || defined(__AVR_AVR64EA28__)  ||  \
      defined(__AVR_AVR32EA32__)  || defined(__AVR_AVR32EA28__)  ||  \
      defined(__AVR_AVR16EA32__)  || defined(__AVR_AVR16EA28__)  ||  \
      defined(__AVR_AVR8EA32__)   || defined(__AVR_AVR8EA28__)
    #define PORTMUX_CCL PORTMUX.CCLROUTEA
    #define PORTMUX_ALTOUT_bm (1 << block.number)
    {
      0,
      PIN0_bm, PIN1_bm, PIN2_bm, PIN3_bm, PIN6_bm,
      PORTA, PORTA, PORTA,
      CCL.SEQCTRL0, CCL.LUT0CTRLA, CCL.LUT0CTRLB, CCL.LUT0CTRLC, CCL.TRUTH0,
    },
    {
      1,
      PIN0_bm, PIN1_bm, PIN2_bm, PIN3_bm, 0,
      PORTC, PORTC, PORTC,
      CCL.SEQCTRL0, CCL.LUT1CTRLA, CCL.LUT1CTRLB, CCL.LUT1CTRLC, CCL.TRUTH1,
    },
    #if defined(__AVR_AVR128DB32__) || defined(__AVR_AVR128DB28__) ||  \
        defined(__AVR_AVR64DB32__)  || defined(__AVR_AVR64DB28__)  ||  \
        defined(__AVR_AVR32DB32__)  || defined(__AVR_AVR32DB28__)  ||  \
        defined(__AVR_AVR64DD32__)  || defined(__AVR_AVR64DD28__)  ||  \
        defined(__AVR_AVR32DD32__)  || defined(__AVR_AVR32DD28__)  ||  \
        defined(__AVR_AVR16DD32__)  || defined(__AVR_AVR16DD28__)
      // These have no PD0, because it was sacrificed for VDDIO2
      {
        2,
        0, PIN1_bm, PIN2_bm, PIN3_bm, PIN6_bm,
        PORTD, PORTD, PORTD,
        CCL.SEQCTRL1, CCL.LUT2CTRLA, CCL.LUT2CTRLB, CCL.LUT2CTRLC, CCL.TRUTH2,
      },
    #else
      {
        2,
        PIN0_bm, PIN1_bm, PIN2_bm, PIN3_bm, PIN6_bm,
        PORTD, PORTD, PORTD,
        CCL.SEQCTRL1, CCL.LUT2CTRLA, CCL.LUT2CTRLB, CCL.LUT2CTRLC, CCL.TRUTH2,
      },
    #endif
    #if defined(__AVR_AVR128DA28__)|| defined(__AVR_AVR64DA28__) || \
        defined(__AVR_AVR32DA28__) || defined(__AVR_AVR64EA28__) || \
        defined(__AVR_AVR32EA28__) || defined(__AVR_AVR16EA28__) || \
        defined(__AVR_AVR8EA28__)  || defined(__AVR_AVR64DD28__) || \
        defined(__AVR_AVR32DD28__) || defined(__AVR_AVR16DD28__)
      // only has PF0, PF1, and even if RESET set GPIO, is input only
      {
        3,
        PIN0_bm, PIN1_bm, 0, 0, 0,
        PORTF, PORTF, PORTF,
        CCL.SEQCTRL1, CCL.LUT3CTRLA, CCL.LUT3CTRLB, CCL.LUT3CTRLC, CCL.TRUTH3,
      },
    #elif defined(__AVR_AVR128DA32__)|| defined(__AVR_AVR64DA32__) || \
      defined(__AVR_AVR32DA32__) || defined(__AVR_AVR64EA32__) || \
      defined(__AVR_AVR32EA32__) || defined(__AVR_AVR16EA32__) || \
      defined(__AVR_AVR8EA32__)  || defined(__AVR_AVR64DD32__) || \
      defined(__AVR_AVR32DD32__) || defined(__AVR_AVR16DD32__)
      // even if RESET set GPIO, is input only
      {
        3,
        PIN0_bm, PIN1_bm, PIN2_bm, PIN3_bm, 0,
        PORTF, PORTF, PORTF,
        CCL.SEQCTRL1, CCL.LUT3CTRLA, CCL.LUT3CTRLB, CCL.LUT3CTRLC, CCL.TRUTH3,
      },
    #else
      // megaAVR 0-series with 32-pins
      // Note that PF6 is Reset, which must be set as GPIO to use as alt output.
      {
        3,
        PIN0_bm, PIN1_bm, PIN2_bm, PIN3_bm, PIN6_bm,
        PORTF, PORTF, PORTF,
        CCL.SEQCTRL1, CCL.LUT3CTRLA, CCL.LUT3CTRLB, CCL.LUT3CTRLC, CCL.TRUTH3,
      },
    #endif
  #endif
  #if defined(__AVR_ATmega809__)  || defined(__AVR_ATmega1609__) || \
      defined(__AVR_ATmega3209__) || defined(__AVR_ATmega4809__) || \
      defined(__AVR_AVR128DA64__) || defined(__AVR_AVR128DA48__) || \
      defined(__AVR_AVR64DA64__)  || defined(__AVR_AVR64DA48__)  || \
      defined(__AVR_AVR32DA64__)  || defined(__AVR_AVR32DA48__)  || \
      defined(__AVR_AVR128DB64__) || defined(__AVR_AVR128DB48__) || \
      defined(__AVR_AVR64DB64__)  || defined(__AVR_AVR64DB48__)  || \
      defined(__AVR_AVR32DB64__)  || defined(__AVR_AVR32DB48__)  || \
      defined(__AVR_AVR64EA48__)  || defined(__AVR_AVR32EA48__)  || \
      defined(__AVR_AVR16EA48__)
    #define PORTMUX_CCL PORTMUX.CCLROUTEA
    #define PORTMUX_ALTOUT_bm (1 << block.number)
    {
      0,
      PIN0_bm, PIN1_bm, PIN2_bm, PIN3_bm, PIN6_bm,
      PORTA, PORTA, PORTA,
      CCL.SEQCTRL0, CCL.LUT0CTRLA, CCL.LUT0CTRLB, CCL.LUT0CTRLC, CCL.TRUTH0,
    },
    {
      // Note: 40-Pin version doesn't have output_alt, but the parts cannot
      // be programmatically distinguished - it's a 48-pin die in 40-pin package
      1,
      PIN0_bm, PIN1_bm, PIN2_bm, PIN3_bm, PIN6_bm,
      PORTC, PORTC, PORTC,
      CCL.SEQCTRL0, CCL.LUT1CTRLA, CCL.LUT1CTRLB, CCL.LUT1CTRLC, CCL.TRUTH1,
    },
    {
      2,
      PIN0_bm, PIN1_bm, PIN2_bm, PIN3_bm, PIN6_bm,
      PORTD, PORTD, PORTD,
      CCL.SEQCTRL1, CCL.LUT2CTRLA, CCL.LUT2CTRLB, CCL.LUT2CTRLC, CCL.TRUTH2,
    },
    {
      3,
      PIN0_bm, PIN1_bm, PIN2_bm, PIN3_bm, PIN6_bm,
      PORTF, PORTF, PORTF,
      CCL.SEQCTRL1, CCL.LUT3CTRLA, CCL.LUT3CTRLB, CCL.LUT3CTRLC, CCL.TRUTH3,
    },
  #endif
  #if defined(__AVR_AVR128DA64__) || defined(__AVR_AVR128DA48__)  || \
      defined(__AVR_AVR64DA64__)  || defined(__AVR_AVR64DA48__)   || \
      defined(__AVR_AVR32DA64__)  || defined(__AVR_AVR32DA48__)   || \
      defined(__AVR_AVR128DB64__) || defined(__AVR_AVR128DB48__)  || \
      defined(__AVR_AVR64DB64__)  || defined(__AVR_AVR64DB48__)   || \
      defined(__AVR_AVR32DB64__)  || defined(__AVR_AVR32DB48__)
  {
    4,
    PIN0_bm, PIN1_bm, PIN2_bm, PIN3_bm, PIN6_bm,
    PORTB, PORTB, PORTB,
    CCL.SEQCTRL2, CCL.LUT4CTRLA, CCL.LUT4CTRLB, CCL.LUT4CTRLC, CCL.TRUTH4,
  },
  {
    5,
    #ifdef PORTG
      PIN0_bm, PIN1_bm, PIN2_bm, PIN3_bm, PIN6_bm,
      PORTG, PORTG, PORTG,
    #else
      0,0,0,0,0,
      PORTA,PORTA,PORTA,
    #endif
    CCL.SEQCTRL2, CCL.LUT5CTRLA, CCL.LUT5CTRLB, CCL.LUT5CTRLC, CCL.TRUTH5,
  },
  #endif
};

#endif
