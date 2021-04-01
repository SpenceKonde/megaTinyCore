#ifndef COMPARATOR_h
#define COMPARATOR_h

#include <Arduino.h>




namespace out {
  enum output_t : uint8_t {
    disable = 0x00,
    enable  = 0x40,
    invert  = 0x80,
  };
  enum pinswap_t : uint8_t {
    no_swap  = 0x00,
    pin_swap = 0x01,
  };
  enum initval_t : uint8_t {
    init_low  = 0x00,
    init_high = 0x40,
  };
};

namespace hyst {
  enum hysteresis_t : uint8_t {
    disable = 0x00, // No hysteresis
    small   = 0x02, // 10 mV
    medium  = 0x04, // 25 mV
    large   = 0x06, // 50 mV
  };
};

#if (MEGATINYCORE_SERIES == 0 || defined(__AVR_ATtinyxy2__))
  namespace in_p {
    enum inputP_t : uint8_t {
      in0    = 0x00,
    };
  };

  namespace in_n {
    enum inputN_t : uint8_t {
      in0    = 0x00,
      vref   = 0x02,
      #if (MEGATINYCORE_SERIES == 1) /* 212 and 412 */
        dacref = 0x03,
      #endif
    };
  };
#elif (MEGATINYCORE_SERIES == 1 && PROGMEM_SIZE < 16384)
  //small-flash 1-series, so only gets in0/in1
  namespace in_p {
    enum inputP_t : uint8_t {
      in0    = 0x00,
      in1    = 0x01,
    };
  };

  namespace in_n {
    enum inputN_t : uint8_t {
      in0    = 0x00,
      in1    = 0x01,
      vref   = 0x02,
      dacref = 0x03,
    };
  };
#else
  //2-series or golden 1-series, so has all inputs.
  namespace in_p {
    enum inputP_t : uint8_t {
      in0    = 0x00,
      in1    = 0x01,
      in2    = 0x02,
      in3    = 0x03,
    };
  };

  namespace in_n {
    enum inputN_t : uint8_t {
      in0    = 0x00,
      in1    = 0x01,
      #if MEGATINYCORE_SERIES == 2 /* only the 2-series has the third negative input pin */
        in2  = 0x02,
      #else
        vref = 0x02,
      #endif
      dacref = 0x03,
    };
  }
#endif


#if !defined(AC0_DACREF)
/* not a 2-series */

  namespace ref {
    enum reference_t : uint8_t {
      vref_0v55 = 0x00, // 1.024V
      vref_1v1   = 0x01, // 2.048V
      vref_1v5   = 0x04, // 2.5V
      vref_2v5   = 0x02,
      vref_4v3   = 0x03, // 4.096V
      disable    = 0x08,
    };
  };
#else
  namespace ref {
    enum reference_t : uint8_t {
      vref_1v024 = 0x00, // 1.024V
      vref_2v048 = 0x01, // 2.048V
      vref_2v5   = 0x02,
      vref_4v096 = 0x03, // 4.096V
      vref_vdd   = 0x07, // VDD as reference=
      disable    = 0x08,
    };
  };
#endif

class AnalogComparator {
  public:
#if (MEGATINYCORE_SERIES == 0 || defined(__AVR_ATtinyxy2__))
    AnalogComparator(const uint8_t comparator_number,
                     AC_t &ac,
                     register8_t &in0_p,
                     register8_t &in0_n);
#else
    AnalogComparator(const uint8_t comparator_number,
                     AC_t &ac,
                     register8_t &in0_p,
                     register8_t &in1_p,
                     register8_t &in2_p,
                     register8_t &in3_p,
                     register8_t &in0_n,
                     register8_t &in1_n,
                     register8_t &in2_n);
#endif
    void init();
    void start(bool state = true);
    void stop(bool restorepins = false);
    void attachInterrupt(voidFuncPtr callback, uint8_t mode);
    void detachInterrupt();
    inline bool value() {
      return !!(AC.STATUS & AC_STATE_bm);
    }

    out::output_t      output         = out::disable;
    out::pinswap_t     output_swap    = out::no_swap;
    out::initval_t     output_initval = out::init_low;
    hyst::hysteresis_t hysteresis     = hyst::disable;
    in_p::inputP_t     input_p        = in_p::in0;
    in_n::inputN_t     input_n        = in_n::in0;
    //bool               invert         = false;
    ref::reference_t   reference      = ref::disable;
    uint8_t dacref = 0xff;

  private:
    const uint8_t comparator_number;
    AC_t &AC;
#if (MEGATINYCORE_SERIES == 0 || defined(__AVR_ATtinyxy2__)) /* These are highly resource constrained; this looked like an easy way to save flash */
    register8_t &IN0_P;
    register8_t &IN0_N;
#else
    register8_t &IN0_P;
    register8_t &IN1_P;
    register8_t &IN2_P;
    register8_t &IN3_P;
    register8_t &IN0_N;
    register8_t &IN1_N;
    register8_t &IN2_N;
#endif
    bool enable = false;
};

#if defined(AC0_AC_vect)
  extern AnalogComparator Comparator0;
  #define Comparator Comparator0
#endif

#if defined(AC1_AC_vect)
  extern AnalogComparator Comparator1;
#endif

#if defined(AC2_AC_vect)
  extern AnalogComparator Comparator2;
#endif

#endif
