//*INDENT-OFF* formatting checker doesn't like how this is indented. I can't imagine why.
#include "Comparator.h"
#include "Arduino.h"
/*
PIN    | DA/DB AC0 | DA/DB AC1 | DA/DB AC2 |  DD AC0  |  EA AC0  |  EA AC1  | Mega AC0 | EB AC0  | EB AC1  | DU AC0  |
-------|-----------|-----------|-----------|----------|----------|----------|----------|---------|---------|---------|
IN P0# |  PIN_PD2  |  PIN_PD2  |  PIN_PD2  | PIN_PD2* | PIN_PD2  | PIN_PD2  | PIN_PD2  | PIN_PD2 | PIN_PD2 | PIN_p   |
IN P1  |  PIN_PE0* |  PIN_PD3  |  PIN_PD4  |   n/a    | PIN_PE0* | PIN_PD3  | PIN_PD4  |   n/a   | PIN_PD3 |   n/a   |
IN P2  |  PIN_PE2* |  PIN_PD4  |  PIN_PE1* |   n/a    | PIN_PE2* | PIN_PD4  | PIN_PD6  |   n/a   | PIN_PD4 |   n/a   |
IN P3# |  PIN_PD6  |  PIN_PD6  |  PIN_PD6  | PIN_PD6  | PIN_PD6  | PIN_PD6  | PIN_PD1  | PIN_PD6 | PIN_PD6 | PIN_PD6 |
IN P4= |    n/a    |    n/a    |    n/a    | PIN_PC3! | PIN_PC3  | PIN_PC3  |   n/a    | PIN_PC3 | PIN_PC3 | PIN_PC3 |
IN P5  |    n/a    |    n/a    |    n/a    |    n/a   |   n/a    |   n/a    |   n/a    | PIN_PD4 | PIN_PA6 |   n/a   |
IN P6  |    n/a    |    n/a    |    n/a    |    n/a   |   n/a    |   n/a    |   n/a    | PIN_PD5 | PIN_PA7 |   n/a   |
IN N0  |  PIN_PD3  |  PIN_PD5  |  PIN_PD7  | PIN_PD3* | PIN_PD3  | PIN_PD5  | PIN_PD3  | PIN_PD3 | PIN_PD5 | PIN_PD5 |
IN N1# |  PIN_PD0* |  PIN_PD0* |  PIN_PD0* |   n/a    | PIN_PD0  | PIN_PD0  | PIN_PD5  | PIN_PD0 | PIN_PD0 | PIN_PD0 |
IN N2# |  PIN_PD7  |  PIN_PD7  |  PIN_PD7  | PIN_PD7  | PIN_PD7  | PIN_PD7  | PIN_PD7  | PIN_PD7 | PIN_PD7 | PIN_PD7 |
IN N3= |    n/a    |    n/a    |    n/a    | PIN_PC2! | PIN_PC2  | PIN_PC2  |   n/a    | PIN_PC2 | PIN_PC2 |   n/a   |
OUT    |  PIN_PA7  |  PIN_PA7  |  PIN_PA7  | PIN_PA7* | PIN_PA7  | PIN_PA7  | PIN_PA7  | PIN_PA7 | PIN_PA7 | PIN_PA7 |
AltOUT |  PIN_PC6* |  PIN_PC6* |  PIN_PC6* |   n/a    | PIN_PC6* | PIN_PC6* |   n/a    |   n/a   |   n/a   |   n/a   |
*/
#if (defined(ANALOG_COMP_PINS_DA_DB))
  /* P0, P1, P2, P3, N0, N1, N2 */
  #if defined(MVIO) && !defined(PORTE)
    /* if we have MVIO on a DB that has 32 or 28 pins, there's no PD0.
     * We can test whether they have that pin by looking for PORTE, which is only present on
     * parts with at least 48 pins, which will also have a PD0. Parts without MVIO always have PD0.*/
    #define PORTDPIN0 AC_NULL_REG
  #else
    #define PORTDPIN0 PORTD.PIN0CTRL
  #endif
  #if defined(AC0_AC_vect)
    #if defined(PORTE)
      AnalogComparator  Comparator0(0, AC0, PORTD.PIN2CTRL, PORTE.PIN0CTRL, PORTE.PIN2CTRL, PORTD.PIN6CTRL,                 PORTD.PIN3CTRL, PORTD.PIN0CTRL, PORTD.PIN7CTRL);
    #else
      AnalogComparator  Comparator0(0, AC0, PORTD.PIN2CTRL, AC_NULL_REG,    AC_NULL_REG,    PORTD.PIN6CTRL,                 PORTD.PIN3CTRL, PORTD.PIN0CTRL, PORTD.PIN7CTRL);
    #endif
  #endif
  #if defined(AC1_AC_vect)
    AnalogComparator    Comparator1(1, AC1, PORTD.PIN2CTRL, PORTD.PIN3CTRL, PORTD.PIN4CTRL, PORTD.PIN6CTRL,                 PORTD.PIN5CTRL, PORTD.PIN0CTRL, PORTD.PIN7CTRL);
  #endif
  #if defined(AC2_AC_vect)
    #if defined(PORTE)
      AnalogComparator  Comparator2(2, AC2, PORTD.PIN2CTRL, PORTD.PIN4CTRL, PORTE.PIN1CTRL, PORTD.PIN6CTRL,                 PORTD.PIN7CTRL, PORTD.PIN0CTRL, PORTD.PIN7CTRL);
    #else
      AnalogComparator  Comparator2(2, AC2, PORTD.PIN2CTRL, PORTD.PIN4CTRL, AC_NULL_REG,    PORTD.PIN6CTRL,/* AVR DA/DB-series has no in_p4 */                    PORTD.PIN7CTRL, PORTD.PIN0      PORTD.PIN7CTRL);
    #endif
  #endif
#elif defined(ANALOG_COMP_PINS_DD)
  /* DD:1 AC:  P0, P3, P4, N0, N2, N3 */
  /* IS_MVIO_ENABLED() may *or may not* be a compile time known constant!
   * This depends on the tools menu option selected for MVIO, and whether we can assume that the code is running on a properly configured part.
   * That is a safe assumption when we build for direct upload, since users will upload the code via UPDI and we can set the fuse then.
   * When using a bootloader, we have no such guarantee - we can't set the fuses when we upload, so unless the user tells us that they
   * really are sure it's configured right, we will test whether MVIO is enabled.
   * In any event, PD2 is only present on parts with the nearly complete PORTD. */
  #if _AVR_PINCOUNT > 20
    #define PORTDPIN2 PORTD.PIN2CTRL
  #else
    #define PORTDPIN2 AC_NULL_REG
  #endif
  #if defined(AC0_AC_vect)
    AnalogComparator    Comparator0(0, AC0, PORTDPIN2,      /* No in_p1 or in_p2          */ PORTD.PIN6CTRL, (IS_MVIO_ENABLED() ? AC_NULL_REG : PORTC.PIN3CTRL),   PORTD.PIN3CTRL,                 PORTD.PIN7CTRL, (IS_MVIO_ENABLED() ? AC_NULL_REG : PORTC.PIN2CTRL));
  #endif
#elif defined(ANALOG_COMP_PINS_EA)
  /* EA:2 ACs: P0, P1, P2, P3, P4, N0, N1, N2, N3 */
  #if defined(AC0_AC_vect)
    #if defined(PORTE)
      AnalogComparator  Comparator0(0, AC0, PORTD.PIN2CTRL, PORTE.PIN0CTRL, PORTE.PIN2CTRL, PORTD.PIN6CTRL, PORTC.PIN3CTRL,/* EA-series has no MVIO */            PORTD.PIN3CTRL, PORTD.PIN0CTRL, PORTD.PIN7CTRL, PORTC.PIN2CTRL);
    #else
      AnalogComparator  Comparator0(0, AC0, PORTD.PIN2CTRL, AC_NULL_REG,    AC_NULL_REG,    PORTD.PIN6CTRL, PORTC.PIN3CTRL,/* EA-series has no MVIO */            PORTD.PIN3CTRL, PORTD.PIN0CTRL, PORTD.PIN7CTRL, PORTC.PIN2CTRL);
    #endif
  #endif
  #if defined(AC1_AC_vect)
    AnalogComparator    Comparator1(1, AC1, PORTD.PIN2CTRL, PORTD.PIN3CTRL, PORTD.PIN4CTRL, PORTD.PIN6CTRL, PORTC.PIN3CTRL,/* EA-series has no MVIO */            PORTD.PIN5CTRL, PORTD.PIN0CTRL, PORTD.PIN7CTRL, PORTC.PIN2CTRL);
  #endif
#elif defined(ANALOG_COMP_PINS_EB)
  /* EB: 2 ACs: P0, P1, P2, P3, P4, P5, P6, N0, N1, N2, N3 */
    AnalogComparator    Comparator0(0, AC0, PORTD.PIN2CTRL, AC_NULL_REG,    AC_NULL_REG,    PORTD.PIN6CTRL, PORTC.PIN3CTRL, PORTD.PIN4CTRL,    PORTD.PIN5CTRL,    PORTD.PIN3CTRL, PORTD.PIN0CTRL, PORTD.PIN7CTRL, PORTC.PIN2CTRL);
    AnalogComparator    Comparator1(1, AC1, PORTD.PIN2CTRL, PORTD.PIN3CTRL, PORTD.PIN4CTRL, PORTD.PIN6CTRL, PORTC.PIN3CTRL, PORTA.PIN6CTRL,    PORTA.PIN7CTRL,    PORTD.PIN5CTRL, PORTD.PIN0CTRL, PORTD.PIN7CTRL, PORTC.PIN2CTRL);
#elif defined(ANALOG_COMP_PINS_MEGA)
  /* mega0:1 AC P0, P1, P2, P3, N0, N1, N2*/
  #if defined(AC0_AC_vect)
    AnalogComparator    Comparator0(0, AC0, PORTD.PIN2CTRL, PORTD.PIN4CTRL, PORTD.PIN6CTRL, PORTD.PIN1CTRL,/* megaAVR 0-series has no in_p4 */                    PORTD.PIN3CTRL, PORTD.PIN5CTRL, PORTD.PIN7CTRL);
  #endif
/* Now for the tinyAVR parts
|  PIN  |  8-pin  |0/1-series AC0|2-series AC0|1+series AC0|1+series AC1|1+series AC2|
|-------|---------|--------------|------------|------------|------------|------------|
|IN P0  | PIN_PA7 |     PIN_PA7  |   PIN_PA7  |   PIN_PA7  |   PIN_PA7  |   PIN_PA6  |
|IN P1  |   n/a   |     PIN_PB5* |   PIN_PB5* |   PIN_PB5* |   PIN_PB6* |   PIN_PB0  |
|IN P2  |   n/a   |       n/a    |   PIN_PB1  |   PIN_PB1  |   PIN_PB0  |   PIN_PB5* |
|IN P3  |   n/a   |       n/a    |   PIN_PB6* |   PIN_PB6* |   PIN_PB4* |   PIN_PB7* |
|IN N0  | PIN_PA6 |     PIN_PA6  |   PIN_PA6  |   PIN_PA6  |   PIN_PA5  |   PIN_PA7  |
|IN N1  |   n/a   |     PIN_PB4* |   PIN_PB4* |   PIN_PB4* |   PIN_PB7* |   PIN_PB6* |
|IN N2  |   n/a   |       n/a    |   PIN_PB0  |     n/a    |     n/a    |     n/a    |
|OUT    | PIN_PA3 |     PIN_PA5  |   PIN_PA5  |   PIN_PA5  |   PIN_PB3  |   PIN_PB2  |
* * indicates pins that are not present on all parts:
*   PB4, PB5 are not present on parts with less than 20 pins.
*   PB6, PB7 are only present on 24-pin parts.
* AC1, AC2 are only present on the "golden 1-series" - parts with 16k+ flash have special features.
*/
#elif defined(ANALOG_COMP_PINS_TINY_FEW)
  /* P0, N0 */
  AnalogComparator      Comparator0(0, AC0, PORTA.PIN7CTRL,                                                                 PORTA.PIN6CTRL);
#elif defined(ANALOG_COMP_PINS_TINY_SOME)
  /* P0, P1, N0, N1 */
  AnalogComparator      Comparator0(0, AC0, PORTA.PIN7CTRL, PORTB.PIN5CTRL,                                                 PORTA.PIN6CTRL, PORTB.PIN4CTRL);
#elif defined(ANALOG_COMP_PINS_TINY_GOLDEN_14)
  /* 1614: 3 AC P0, P1 (AC2), P2 (AC0, AC1), N0 */
  AnalogComparator      Comparator0(0, AC0, PORTA.PIN7CTRL, PORTB.PIN5CTRL, PORTB.PIN1CTRL,                                 PORTA.PIN6CTRL);
  AnalogComparator      Comparator1(1, AC1, PORTA.PIN7CTRL, PORTB.PIN6CTRL, PORTB.PIN0CTRL,                                 PORTA.PIN5CTRL);
  AnalogComparator      Comparator2(2, AC2, PORTA.PIN6CTRL, PORTB.PIN0CTRL, PORTB.PIN5CTRL,                                 PORTA.PIN7CTRL);
#elif defined(ANALOG_COMP_PINS_TINY_GOLDEN)
  /* other golden 1-series: 3 AC P0, P1 (AC1 on 24 only), P2, P3, N0, N1*/
  AnalogComparator      Comparator0(0, AC0, PORTA.PIN7CTRL, PORTB.PIN5CTRL, PORTB.PIN1CTRL, PORTA.PIN6CTRL,                 PORTA.PIN6CTRL, PORTB.PIN4CTRL);
  AnalogComparator      Comparator1(1, AC1, PORTA.PIN7CTRL, PORTB.PIN6CTRL, PORTB.PIN0CTRL, PORTA.PIN4CTRL,                 PORTA.PIN5CTRL, PORTB.PIN7CTRL);
  AnalogComparator      Comparator2(2, AC2, PORTA.PIN6CTRL, PORTB.PIN0CTRL, PORTB.PIN5CTRL, PORTB.PIN7CTRL,                 PORTA.PIN7CTRL, PORTB.PIN6CTRL);

#elif defined(ANALOG_COMP_PINS_TINY_TWO_14)
  /* 14-pin 2-series: 1 AC P0, P2, N0, N2*/
  AnalogComparator      Comparator0(0, AC0, PORTA.PIN7CTRL,                 PORTB.PIN1CTRL,                                 PORTA.PIN6CTRL,                 PORTB.PIN0CTRL);
#elif defined(ANALOG_COMP_PINS_TINY_TWO)
  /* Any other 2-series: 1 AC P0, P1, P2, P3, N0, N1, N2*/
  AnalogComparator      Comparator0(0, AC0, PORTA.PIN7CTRL, PORTB.PIN5CTRL, PORTB.PIN1CTRL, PORTB.PIN6CTRL,                 PORTA.PIN6CTRL, PORTB.PIN4CTRL, PORTB.PIN0CTRL);
#endif

/* A few notes on adaptations for megaTinyCore/DxCore
  1. There are two definitions for the comparator objects, depending on whether
  PORTE is defined. If it's not, we still need to feed the constructor something.
  2. The Dx-series only get one shared voltage reference!
  3. Inversion may be desired even when just using a as an event source, with no
  pin output -> disable_invert
  4. Added read() method to get current state.
  5. Added optional (defaults to false) argument to stop(), if true, it re-enables
  the digital input buffers on the pins that were used.
  6. 11/21: cleaned up a few places where we did R/M/W instead of write.
  7. Windowed mode and POWER are not supported. Windowed mode is not worth it
  POWER might be?
  8. Needs a way to disable defining the ISRs.
*/

#if defined(ANALOG_COMP_PINS_DA_DB) || defined(ANALOG_COMP_PINS_MEGA)
/* DA, DB, and megaAVR have 7 inputs per AC: P0, P1, P2, P3, N0, N1, N2 */
AnalogComparator::AnalogComparator(
                                   const uint8_t comp_number,
                                   AC_t& ac,
                                   register8_t& in0_p,
                                   register8_t& in1_p,
                                   register8_t& in2_p,
                                   register8_t& in3_p,
                                   register8_t& in0_n,
                                   register8_t& in1_n,
                                   register8_t& in2_n
                                   )
                                   : comparator_number(comp_number),
                                     AC(ac),
                                     IN0_P(in0_p),
                                     IN1_P(in1_p),
                                     IN2_P(in2_p),
                                     IN3_P(in3_p),
                                     IN0_N(in0_n),
                                     IN1_N(in1_n),
                                     IN2_N(in2_n) { }
#elif defined(ANALOG_COMP_PINS_DD) /*6 inputs: P0, P3, P4, N0, N2, N3 */
AnalogComparator::AnalogComparator(
                                   const uint8_t comp_number,
                                   AC_t& ac,
                                   register8_t& in0_p,
                                   register8_t& in3_p,
                                   register8_t& in4_p,
                                   register8_t& in0_n,
                                   register8_t& in2_n,
                                   register8_t& in3_n
                                   )
                                   : comparator_number(comp_number),
                                     AC(ac),
                                     IN0_P(in0_p),
                                     IN3_P(in3_p),
                                     IN4_P(in4_p),
                                     IN0_N(in0_n),
                                     IN2_N(in2_n),
                                     IN3_N(in3_n) { }
#elif defined(ANALOG_COMP_PINS_EA) /*9 inputs P0, P1, P2, P3, P4, N0, N1, N2, N3 */
AnalogComparator::AnalogComparator(
                                   const uint8_t comp_number,
                                   AC_t& ac,
                                   register8_t& in0_p,
                                   register8_t& in1_p,
                                   register8_t& in2_p,
                                   register8_t& in3_p,
                                   register8_t& in4_p,
                                   register8_t& in0_n,
                                   register8_t& in1_n,
                                   register8_t& in2_n,
                                   register8_t& in3_n
                                   )
                                   : comparator_number(comp_number),
                                     AC(ac),
                                     IN0_P(in0_p),
                                     IN1_P(in1_p),
                                     IN2_P(in2_p),
                                     IN3_P(in3_p),
                                     IN4_P(in4_p),
                                     IN0_N(in0_n),
                                     IN1_N(in1_n),
                                     IN2_N(in2_n),
                                     IN3_N(in3_n) { }
#elif defined(ANALOG_COMP_PINS_EB) /*9 inputs P0, P1, P2, P3, P4, N0, N1, N2, N3 */
AnalogComparator::AnalogComparator(
                                   const uint8_t comp_number,
                                   AC_t& ac,
                                   register8_t& in0_p,
                                   register8_t& in1_p,
                                   register8_t& in2_p,
                                   register8_t& in3_p,
                                   register8_t& in4_p,
                                   register8_t& in5_p,
                                   register8_t& in6_p,
                                   register8_t& in0_n,
                                   register8_t& in1_n,
                                   register8_t& in2_n,
                                   register8_t& in3_n
                                   )
                                   : comparator_number(comp_number),
                                     AC(ac),
                                     IN0_P(in0_p),
                                     IN1_P(in1_p),
                                     IN2_P(in2_p),
                                     IN3_P(in3_p),
                                     IN4_P(in4_p),
                                     IN3_P(in5_p),
                                     IN4_P(in6_p),
                                     IN0_N(in0_n),
                                     IN1_N(in1_n),
                                     IN2_N(in2_n),
                                     IN3_N(in3_n) { }
/* Start of tinyAVR */
#elif defined(ANALOG_COMP_PINS_TINY_FEW)
AnalogComparator::AnalogComparator(
                                   const uint8_t comp_number,
                                   AC_t& ac,
                                   register8_t& in0_p,
                                   register8_t& in0_n
                                   )
                                   : comparator_number(comp_number),
                                     AC(ac),
                                     IN0_P(in0_p),
                                     IN0_N(in0_n) { }
#elif defined(ANALOG_COMP_PINS_TINY_SOME)
AnalogComparator::AnalogComparator(
                                   const uint8_t comp_number,
                                   AC_t& ac,
                                   register8_t& in0_p,
                                   register8_t& in1_p,
                                   register8_t& in0_n,
                                   register8_t& in1_n
                                   )
                                   : comparator_number(comp_number),
                                     AC(ac),
                                     IN0_P(in0_p),
                                     IN1_P(in1_p),
                                     IN0_N(in0_n),
                                     IN1_N(in1_n) { }
#elif defined(ANALOG_COMP_PINS_TINY_GOLDEN_14)
AnalogComparator::AnalogComparator(
                                   const uint8_t comp_number,
                                   AC_t& ac,
                                   register8_t& in0_p,
                                   register8_t& in1_p,
                                   register8_t& in2_p,
                                   register8_t& in0_n
                                   )
                                   : comparator_number(comp_number),
                                     AC(ac),
                                     IN0_P(in0_p),
                                     IN1_P(in1_p),
                                     IN2_P(in2_p),
                                     IN0_N(in0_n) { }
#elif defined(ANALOG_COMP_PINS_TINY_TWO_14)
AnalogComparator::AnalogComparator(
                                   const uint8_t comp_number,
                                   AC_t& ac,
                                   register8_t& in0_p,
                                   register8_t& in2_p,
                                   register8_t& in0_n,
                                   register8_t& in2_n
                                   )
                                   : comparator_number(comp_number),
                                     AC(ac),
                                     IN0_P(in0_p),
                                     IN2_P(in2_p),
                                     IN0_N(in0_n),
                                     IN2_N(in2_n) { }
#elif defined(ANALOG_COMP_PINS_TINY_GOLDEN) /*6 inputs P0, P1, P2, P3, N0, N1 */
AnalogComparator::AnalogComparator(
                                   const uint8_t comp_number,
                                   AC_t& ac,
                                   register8_t& in0_p,
                                   register8_t& in1_p,
                                   register8_t& in2_p,
                                   register8_t& in3_p,
                                   register8_t& in0_n,
                                   register8_t& in1_n
                                   )
                                   : comparator_number(comp_number),
                                     AC(ac),
                                     IN0_P(in0_p),
                                     IN1_P(in1_p),
                                     IN2_P(in2_p),
                                     IN3_P(in3_p),
                                     IN0_N(in0_n),
                                     IN1_N(in1_n) { }

#elif defined(ANALOG_COMP_PINS_TINY_TWO) /*7 inputs P0, P1, P2, P3, N0, N1, N2 */
AnalogComparator::AnalogComparator(
                                   const uint8_t comp_number,
                                   AC_t& ac,
                                   register8_t& in0_p,
                                   register8_t& in1_p,
                                   register8_t& in2_p,
                                   register8_t& in3_p,
                                   register8_t& in0_n,
                                   register8_t& in1_n,
                                   register8_t& in2_n
                                   )
                                   : comparator_number(comp_number),
                                     AC(ac),
                                     IN0_P(in0_p),
                                     IN1_P(in1_p),
                                     IN2_P(in2_p),
                                     IN3_P(in3_p),
                                     IN0_N(in0_n),
                                     IN1_N(in1_n),
                                     IN2_N(in2_n) { }

#else
  #error "Unsupported part or bug in library."
#endif

void AnalogComparator::init() {
  #if defined(DXCORE)
    // Set voltage reference
    if (reference != comparator::ref::disable) {
      VREF.ACREF = VREF_ALWAYSON_bm | reference;
    } else {
      VREF.ACREF &= ~VREF_ALWAYSON_bm;
    }
    // Spence 1/21: We have 3 ACs sharing one voltage reference.
    // I adapted it to reach out and set the other objects reference property
    // to match when you call init()... not sure if this is ideal,
    // but it seems weird to have to specify it every time you turn one on otherwise you lose the others.
    if (comparator_number != 0) {
      Comparator0.reference = reference;
    }
    #if defined(AC1)
    if (comparator_number != 1) {
      Comparator1.reference = reference;
    }
    #endif
    #if defined(AC2)
    if (comparator_number != 2) {
      Comparator2.reference = reference;
    }
    #endif
    // Set DACREF
    AC.DACREF = dacref;

    // Set hysteresis and output.
    // Power setting not supported at this time.
    AC.CTRLA = (AC.CTRLA & ~(AC_HYSMODE_gm | AC_OUTEN_bm)) | hysteresis | (output & 0x40);

    AC.MUXCTRL = output_initval | (AC.MUXCTRL & ~0x3f) | (input_p << 3) | input_n | (output & 0x80);

    // Prepare for output pin swap
    #if defined(DXCORE) && (defined(HAS_48_PINS) || defined(HAS_64_PINS))
      if (output_swap == comparator::out::pin_swap) {
        PORTMUX.ACROUTEA = ~(1 << comparator_number) | output_swap;
      }
    #endif
  #elif defined(MEGATINYCORE)
    // Set voltage reference
    if (reference != comparator::ref::disable) {
      if (comparator_number == 0) {
        #if defined(VREF_AC0REFEN_bm)
        /* 0/1-series and 2-series name it differently
           See, on the 0-series, the bitfield is named DAC0REFEN, like 1-series
           Even if there is no DAC and no DACREF.
           But on 2-series, where there's a DACREF for AC0, but no proper DAC
           this is called AC0REF. Totally reasonable bitfield naming right? */
          VREF.CTRLA  = reference; /* otherwise empty register on 2-series*/
          VREF.CTRLB |= VREF_AC0REFEN_bm;
          AC.DACREF = dacref;
        #else
          VREF.CTRLA  = (VREF.CTRLA & 0xF8) | reference; /* shared with ADC0 reference */
          #ifdef DAC0
            VREF.CTRLB |= VREF_DAC0REFEN_bm;
            DAC0.CTRLA |= DAC_ENABLE_bm; /* Shared with the actual DAC */
            DAC0.DATA   = dacref;
          #endif
        #endif
      #if defined (AC1)
        } else if (comparator_number == 1) { // all tinyAVR with AC1 and AC2 has the DACREF as DACn, not ACn.DACREF.
          VREF.CTRLC  = (VREF.CTRLC & 0xF8) | reference; /* shared with ADC1 reference */
          VREF.CTRLB |= VREF_DAC1REFEN_bm;
          DAC1.CTRLA |= DAC_ENABLE_bm;
          DAC1.DATA   = dacref;
      #endif
      #if defined (AC2)
        } else if (comparator_number == 2) {
          VREF.CTRLD  = reference; /* otherwise empty register */
          VREF.CTRLB |= VREF_DAC2REFEN_bm;
          DAC2.CTRLA |= DAC_ENABLE_bm;
          DAC2.DATA   = dacref;
      #endif
      }
    } else { /* it is being disabled */
      if (comparator_number == 0) {
        #if defined(VREF_AC0REFEN_bm)
        /* 0/1-series and 2-series name it differently
           See, on the 0-series, where there's nothing like a DAC, still called
           DAC0REF. But on 2-series, where there's a DACREF for AC0, this is
           called AC0REF. Totally reasonable bitfield naming right? */
          VREF.CTRLB &= ~VREF_AC0REFEN_bm;
        #else
          VREF.CTRLB &= ~VREF_DAC0REFEN_bm;
          #ifdef DAC0
            uint8_t temp = DAC0.CTRLA;
            if (!(temp & DAC_OUTEN_bm)) /* Don't disable the DAC if it's also outputting */
              DAC0.CTRLA = temp & (~DAC_ENABLE_bm);
          #endif
        #endif
      #if defined (AC1)
        } else if (comparator_number == 1) {
          VREF.CTRLB &= ~VREF_DAC1REFEN_bm;
          DAC1.CTRLA &= ~DAC_ENABLE_bm;
      #endif
      #if defined (AC2)
        } else if (comparator_number == 2) {
          VREF.CTRLB &= ~VREF_DAC2REFEN_bm;
          DAC2.CTRLA &= ~DAC_ENABLE_bm;
      #endif
      }
    }
    AC.MUXCTRLA = (input_p << 3) | input_n | (output & 0x80);

    // Set hysteresis, and output,
    AC.CTRLA = (AC.CTRLA & ~(AC_HYSMODE_gm | AC_OUTEN_bm)) | hysteresis | (output & 0x40);
  #else /* megaAVR 0-series */
    // Set voltage reference
    if (reference != comparator::ref::disable)
    {
      VREF.CTRLA = (VREF.CTRLA & ~VREF_AC0REFSEL_AVDD_gc) | reference;
      VREF.CTRLB = VREF_AC0REFEN_bm;
    }
    else
      VREF.CTRLB &= ~VREF_AC0REFEN_bm;

    // Set DACREF
    AC.DACREF = dacref;

    // Set hysteresis and output pin state
    AC.CTRLA = (AC.CTRLA & ~AC_HYSMODE_gm) | hysteresis | (output & AC_OUTEN_bm);
  #endif
  // Now, we disable the appropriate pins...
  #if !defined(MEGATINYCORE) /* At least Dx and megaAVR can share this part */
    #if defined(ANALOG_COMP_PINS_DD)
      if        (input_p == comparator::in_p::in0) {
        IN0_P = PORT_ISC_INPUT_DISABLE_gc;
      } else if (input_p == comparator::in_p::in3) {
        IN3_P = PORT_ISC_INPUT_DISABLE_gc;
      } else if (input_p == comparator::in_p::in4) {
        IN4_P = PORT_ISC_INPUT_DISABLE_gc;
      }
      if        (input_n == comparator::in_n::in0) {
        IN0_N = PORT_ISC_INPUT_DISABLE_gc;
      } else if (input_n == comparator::in_n::in2) {
        IN2_N = PORT_ISC_INPUT_DISABLE_gc;
      } else if (input_n == comparator::in_n::in3) {
        IN3_N = PORT_ISC_INPUT_DISABLE_gc;
      }
    #elif defined(ANALOG_COMP_PINS_DA_DB) || defined(ANALOG_COMP_PINS_MEGA)
      if        (input_p == comparator::in_p::in0) {
        IN0_P = PORT_ISC_INPUT_DISABLE_gc;
      } else if (input_p == comparator::in_p::in1) {
        IN1_P = PORT_ISC_INPUT_DISABLE_gc;
      } else if (input_p == comparator::in_p::in2) {
        IN2_P = PORT_ISC_INPUT_DISABLE_gc;
      } else if (input_p == comparator::in_p::in3) {
        IN3_P = PORT_ISC_INPUT_DISABLE_gc;
      }
      if        (input_n == comparator::in_n::in0) {
        IN0_N = PORT_ISC_INPUT_DISABLE_gc;
      }
      #if !defined(ANALOG_COMP_NO_N1)
      else if (input_n == comparator::in_n::in1) {
        IN1_N = PORT_ISC_INPUT_DISABLE_gc;
      }
      #endif
      else if (input_n == comparator::in_n::in2) {
        IN2_N = PORT_ISC_INPUT_DISABLE_gc;
      }
    #elif defined(ANALOG_COMP_PINS_EA)
      if        (input_p == comparator::in_p::in0) {
        IN0_P = PORT_ISC_INPUT_DISABLE_gc;
      } else if (input_p == comparator::in_p::in1) {
        IN1_P = PORT_ISC_INPUT_DISABLE_gc;
      } else if (input_p == comparator::in_p::in2) {
        IN2_P = PORT_ISC_INPUT_DISABLE_gc;
      } else if (input_p == comparator::in_p::in3) {
        IN3_P = PORT_ISC_INPUT_DISABLE_gc;
      } else if (input_p == comparator::in_p::in4) {
        IN4_P = PORT_ISC_INPUT_DISABLE_gc;
      }
      if        (input_n == comparator::in_n::in0) {
        IN0_N = PORT_ISC_INPUT_DISABLE_gc;
      } else if (input_n == comparator::in_n::in1) {
        IN1_N = PORT_ISC_INPUT_DISABLE_gc;
      } else if (input_n == comparator::in_n::in2) {
        IN2_N = PORT_ISC_INPUT_DISABLE_gc;
      } else if (input_n == comparator::in_n::in3) {
        IN3_N = PORT_ISC_INPUT_DISABLE_gc;
      }
    #elif defined(ANALOG_COMP_PINS_EB)
      if        (input_p == comparator::in_p::in0) {
        IN0_P = PORT_ISC_INPUT_DISABLE_gc;
      } else if (input_p == comparator::in_p::in1) {
        IN1_P = PORT_ISC_INPUT_DISABLE_gc;
      } else if (input_p == comparator::in_p::in2) {
        IN2_P = PORT_ISC_INPUT_DISABLE_gc;
      } else if (input_p == comparator::in_p::in3) {
        IN3_P = PORT_ISC_INPUT_DISABLE_gc;
      } else if (input_p == comparator::in_p::in4) {
        IN4_P = PORT_ISC_INPUT_DISABLE_gc;
      } else if (input_p == comparator::in_p::in5) {
        IN5_P = PORT_ISC_INPUT_DISABLE_gc;
      } else if (input_p == comparator::in_p::in6) {
        IN6_P = PORT_ISC_INPUT_DISABLE_gc;
      }
      if        (input_n == comparator::in_n::in0) {
        IN0_N = PORT_ISC_INPUT_DISABLE_gc;
      } else if (input_n == comparator::in_n::in1) {
        IN1_N = PORT_ISC_INPUT_DISABLE_gc;
      } else if (input_n == comparator::in_n::in2) {
        IN2_N = PORT_ISC_INPUT_DISABLE_gc;
      } else if (input_n == comparator::in_n::in3) {
        IN3_N = PORT_ISC_INPUT_DISABLE_gc;
      }
    #endif
  #else /* tinyAVR */
    if          (input_p == comparator::in_p::in0) {
      IN0_P   = PORT_ISC_INPUT_DISABLE_gc;
    }
    #if   defined(ANALOG_COMP_PINS_TINY_SOME)
      else if   (input_p == comparator::in_p::in1) {
        IN1_P = PORT_ISC_INPUT_DISABLE_gc;
      }
    #elif defined(ANALOG_COMP_PINS_TINY_GOLDEN_14)
      else if   (input_p == comparator::in_p::in1) {
        IN1_P = PORT_ISC_INPUT_DISABLE_gc;
      } else if (input_p == comparator::in_p::in2) {
        IN2_P = PORT_ISC_INPUT_DISABLE_gc;
      }
    #elif defined(ANALOG_COMP_PINS_TINY_TWO_14)
      else if   (input_p == comparator::in_p::in2) {
        IN2_P = PORT_ISC_INPUT_DISABLE_gc;
      }
    #elif defined(ANALOG_COMP_PINS_TINY_GOLDEN)
      else if   (input_p == comparator::in_p::in1) {
        IN1_P = PORT_ISC_INPUT_DISABLE_gc;
      } else if (input_p == comparator::in_p::in2) {
        IN2_P = PORT_ISC_INPUT_DISABLE_gc;
      } else if (input_p == comparator::in_p::in3) {
        IN3_P = PORT_ISC_INPUT_DISABLE_gc;
      }
    #elif defined(ANALOG_COMP_PINS_TINY_GOLDEN)
      else if   (input_p == comparator::in_p::in1) {
        IN1_P = PORT_ISC_INPUT_DISABLE_gc;
      } else if (input_p == comparator::in_p::in2) {
        IN2_P = PORT_ISC_INPUT_DISABLE_gc;
      } else if (input_p == comparator::in_p::in3) {
        IN3_P = PORT_ISC_INPUT_DISABLE_gc;
      } else if (input_p == comparator::in_p::in4) {
        IN4_P = PORT_ISC_INPUT_DISABLE_gc;
      }
    #endif
    if          (input_n == comparator::in_n::in0) {
      IN0_N   = PORT_ISC_INPUT_DISABLE_gc;
    }
    #if defined(ANALOG_COMP_PINS_TINY_SOME)   || defined(ANALOG_COMP_PINS_TINY_TWO) || defined(ANALOG_COMP_PINS_TINY_GOLDEN)
      else if   (input_n == comparator::in_n::in1) {
        IN1_N = PORT_ISC_INPUT_DISABLE_gc;
      }
    #endif
    #if defined(ANALOG_COMP_PINS_TINY_TWO_14) || defined(ANALOG_COMP_PINS_TINY_TWO)
      else if   (input_n == comparator::in_n::in2) {
        IN2_N = PORT_ISC_INPUT_DISABLE_gc;
      }
    #endif
  #endif  // Phew!
}

void AnalogComparator::start(bool state) {
  if (state) {
    AC.CTRLA |= AC_ENABLE_bm;
  } else {
    AC.CTRLA &= ~AC_ENABLE_bm;
  }
}

void AnalogComparator::stop(bool restorepins) {
  start(false);
  if (restorepins) {
    #if !defined(MEGATINYCORE) /* At least Dx and megaAVR can share this part */
      #if defined(ANALOG_COMP_PINS_DD)
        if        (input_p == comparator::in_p::in0) {
          IN0_P = 0;
        } else if (input_p == comparator::in_p::in3) {
          IN3_P = 0;
        } else if (input_p == comparator::in_p::in4) {
          IN4_P = 0;
        }
        if        (input_n == comparator::in_n::in0) {
          IN0_N = 0;
        } else if (input_n == comparator::in_n::in3) {
          IN2_N = 0;
        } else if (input_n == comparator::in_n::in3) {
          IN3_N = 0;
        }
      #elif defined(ANALOG_COMP_PINS_DA_DB) || defined(ANALOG_COMP_PINS_MEGA)
        if        (input_p == comparator::in_p::in0) {
          IN0_P = 0;
        } else if (input_p == comparator::in_p::in1) {
          IN1_P = 0;
        } else if (input_p == comparator::in_p::in2) {
          IN2_P = 0;
        } else if (input_p == comparator::in_p::in3) {
          IN3_P = 0;
        }
        if        (input_n == comparator::in_n::in0) {
          IN0_N = 0;
        }
        #if !defined(ANALOG_COMP_NO_N1)
          else if (input_n == comparator::in_n::in1) {
            IN1_N = 0;
          }
        #endif
        else if (input_n == comparator::in_n::in2) {
          IN2_N = 0;
        }
      #elif defined(ANALOG_COMP_PINS_EA)
        if        (input_p == comparator::in_p::in0) {
          IN0_P = 0;
        } else if (input_p == comparator::in_p::in1) {
          IN1_P = 0;
        } else if (input_p == comparator::in_p::in2) {
          IN2_P = 0;
        } else if (input_p == comparator::in_p::in3) {
          IN3_P = 0;
        } else if (input_p == comparator::in_p::in4) {
          IN4_P = 0;
        }
        if        (input_n == comparator::in_n::in0) {
          IN0_N = 0;
        } else if (input_n == comparator::in_n::in1) {
          IN1_N = 0;
        } else if (input_n == comparator::in_n::in2) {
          IN2_N = 0;
        } else if (input_n == comparator::in_n::in3) {
          IN3_N = 0;
        }
      #elif defined(ANALOG_COMP_PINS_EB)
        if        (input_p == comparator::in_p::in0) {
          IN0_P = 0;
        } else if (input_p == comparator::in_p::in1) {
          IN1_P = 0;
        } else if (input_p == comparator::in_p::in2) {
          IN2_P = 0;
        } else if (input_p == comparator::in_p::in3) {
          IN3_P = 0;
        } else if (input_p == comparator::in_p::in4) {
          IN4_P = 0;
        } else if (input_p == comparator::in_p::in3) {
          IN5_P = 0;
        } else if (input_p == comparator::in_p::in4) {
          IN6_P = 0;
        }
        if        (input_n == comparator::in_n::in0) {
          IN0_N = 0;
        } else if (input_n == comparator::in_n::in1) {
          IN1_N = 0;
        } else if (input_n == comparator::in_n::in2) {
          IN2_N = 0;
        } else if (input_n == comparator::in_n::in3) {
          IN3_N = 0;
        }
      #endif
    #else /* tinyAVR */
      if          (input_p == comparator::in_p::in0) {
        IN0_P   = 0;
      }
      #if   defined(ANALOG_COMP_PINS_TINY_SOME)
        else if   (input_p == comparator::in_p::in1) {
          IN1_P = 0;
        }
      #elif defined(ANALOG_COMP_PINS_TINY_GOLDEN_14)
        else if   (input_p == comparator::in_p::in1) {
          IN1_P = 0;
        } else if (input_p == comparator::in_p::in2) {
          IN2_P = 0;
        }
      #elif defined(ANALOG_COMP_PINS_TINY_TWO_14)
        else if   (input_p == comparator::in_p::in2) {
          IN2_P = 0;
        }
      #elif defined(ANALOG_COMP_PINS_TINY_GOLDEN)
        else if   (input_p == comparator::in_p::in1) {
          IN1_P = 0;
        } else if (input_p == comparator::in_p::in2) {
          IN2_P = 0;
        } else if (input_p == comparator::in_p::in3) {
          IN3_P = 0;
        }
      #elif defined(ANALOG_COMP_PINS_TINY_GOLDEN)
        else if   (input_p == comparator::in_p::in1) {
          IN1_P = 0;
        } else if (input_p == comparator::in_p::in2) {
          IN2_P = 0;
        } else if (input_p == comparator::in_p::in3) {
          IN3_P = 0;
        } else if (input_p == comparator::in_p::in4) {
          IN4_P = 0;
        }
      #endif
      if          (input_n == comparator::in_n::in0) {
        IN0_N   = 0;
      }
      #if defined(ANALOG_COMP_PINS_TINY_SOME)   || defined(ANALOG_COMP_PINS_TINY_TWO) || defined(ANALOG_COMP_PINS_TINY_GOLDEN)
        else if   (input_n == comparator::in_n::in1) {
          IN1_N = 0;
        }
      #endif
      #if defined(ANALOG_COMP_PINS_TINY_TWO_14) || defined(ANALOG_COMP_PINS_TINY_TWO)
        else if   (input_n == comparator::in_n::in2) {
          IN2_N = 0;
        }
      #endif
    #endif
  }
}
