#ifndef LOGIC_h
#define LOGIC_h

#include <Arduino.h>

namespace in {
  enum input_t : uint8_t {
    #if defined(__AVR_ATmega808__)  || defined(__AVR_ATmega1608__) || \
    defined(__AVR_ATmega3208__) || defined(__AVR_ATmega4808__) || \
    defined(__AVR_ATmega809__)  || defined(__AVR_ATmega1609__) || \
    defined(__AVR_ATmega3209__) || defined(__AVR_ATmega4809__) || \
    defined(__AVR_ATtiny3227__) || defined(__AVR_ATtiny3226__) || \
    defined(__AVR_ATtiny3224__) || defined(__AVR_ATtiny1627__) || \
    defined(__AVR_ATtiny1626__) || defined(__AVR_ATtiny1624__) || \
    defined(__AVR_ATtiny827__)  || defined(__AVR_ATtiny826__)  || \
    defined(__AVR_ATtiny824__)  || defined(__AVR_ATtiny427__)  || \
    defined(__AVR_ATtiny426__)  || defined(__AVR_ATtiny424__)
    masked          = 0x00,
    unused          = 0x00,
    disable         = 0x00,
    feedback        = 0x01,
    link            = 0x02,
    event_0         = 0x03,
    event_a         = 0x03,
    event_1         = 0x04,
    event_b         = 0x04,
    pin             = 0x05,
    ac              = 0x06,
    usart           = 0x08,
    spi             = 0x09,
    tca0            = 0x0A,
    tca             = 0x0A,
    tcb             = 0x0C,
    tcb0            = 0x0C,
    input_pullup    = 0x15,
    input           = 0x25,
    input_no_pullup = 0x25,
    #elif defined(__AVR_AVR128DA64__) || defined(__AVR_AVR128DA48__) || \
    defined(__AVR_AVR128DA32__) || defined(__AVR_AVR128DA32__) || \
    defined(__AVR_AVR64DA64__)  || defined(__AVR_AVR64DA48__)  || \
    defined(__AVR_AVR64DA32__)  || defined(__AVR_AVR64DA28__)  || \
    defined(__AVR_AVR32DA64__)  || defined(__AVR_AVR32DA48__)  || \
    defined(__AVR_AVR32DA32__)  || defined(__AVR_AVR32DA28__)  || \
    defined(__AVR_AVR128DB64__) || defined(__AVR_AVR128DB48__) || \
    defined(__AVR_AVR128DB32__) || defined(__AVR_AVR128DB32__) || \
    defined(__AVR_AVR64DB64__)  || defined(__AVR_AVR64DB48__)  || \
    defined(__AVR_AVR64DB32__)  || defined(__AVR_AVR64DB28__)  || \
    defined(__AVR_AVR32DB64__)  || defined(__AVR_AVR32DB48__)  || \
    defined(__AVR_AVR32DB32__)  || defined(__AVR_AVR32DB28__)
    masked          = 0x00,
    unused          = 0x00,
    disable         = 0x00,
    feedback        = 0x01,
    link            = 0x02,
    event_0         = 0x03,
    event_a         = 0x03,
    event_1         = 0x04,
    event_b         = 0x04,
    pin             = 0x05,
    ac              = 0x06,
    zcd             = 0x07,
    usart           = 0x08,
    spi             = 0x09,
    tca0            = 0x0A,
    tca             = 0x0A,
    tca1            = 0x0B,
    tcb             = 0x0C,
    tcd             = 0x0D,
    tcd0            = 0x0D,
    input_pullup    = 0x15,
    input           = 0x25,
    input_no_pullup = 0x25,
    #elif defined(__AVR_AVR64DD20__)  || defined(__AVR_AVR64DD14__)  ||  \
    defined(__AVR_AVR32DD20__)  || defined(__AVR_AVR32DD14__)  ||  \
    defined(__AVR_AVR16DD20__)  || defined(__AVR_AVR16DD14__)  ||  \
    defined(__AVR_AVR64DD32__)  || defined(__AVR_AVR64DD28__)  ||  \
    defined(__AVR_AVR32DD32__)  || defined(__AVR_AVR32DD28__)  ||  \
    defined(__AVR_AVR16DD32__)  || defined(__AVR_AVR16DD28__)
    // educated guess - these have at least one of everything the rest of DA-series has, and these lists have been super consistent since v1.0 (tinyAVR 0/1-series)
    masked          = 0x00,
    unused          = 0x00,
    disable         = 0x00,
    feedback        = 0x01,
    link            = 0x02,
    event_0         = 0x03,
    event_a         = 0x03,
    event_1         = 0x04,
    event_b         = 0x04,
    pin             = 0x05,
    ac              = 0x06,
    zcd             = 0x07,
    usart           = 0x08,
    spi             = 0x09,
    tca0            = 0x0A,
    tca             = 0x0A,
    tcb             = 0x0C,
    tcd             = 0x0D,
    tcd0            = 0x0D,
    input_pullup    = 0x15,
    input           = 0x25,
    input_no_pullup = 0x25,
    #elif defined(__AVR_AVR64EA28__)  || defined(__AVR_AVR32EA28__)  ||  \
    defined(__AVR_AVR16EA28__)  || defined(__AVR_AVR8EA28__)   ||  \
    defined(__AVR_AVR64EA32__)  || defined(__AVR_AVR32EA32__)  ||  \
    defined(__AVR_AVR16EA32__)  || defined(__AVR_AVR8EA32__)   ||  \
    defined(__AVR_AVR64EA48__)  || defined(__AVR_AVR32EA48__)  ||  \
    defined(__AVR_AVR16EA48__)
    // educated guess based on recent consistency and which peripherals they have
    masked          = 0x00,
    unused          = 0x00,
    disable         = 0x00,
    feedback        = 0x01,
    link            = 0x02,
    event_0         = 0x03,
    event_a         = 0x03,
    event_1         = 0x04,
    event_b         = 0x04,
    pin             = 0x05,
    ac              = 0x06,
    usart           = 0x08,
    spi             = 0x09,
    tca0            = 0x0A,
    tca             = 0x0A,
    tca1            = 0x0B,
    tcb             = 0x0C,
    input_pullup    = 0x15,
    input           = 0x25,
    input_no_pullup = 0x25,

    #else // tinyAVR 0/1-series
    masked          = 0x00,
    unused          = 0x00,
    disable         = 0x00,
    feedback        = 0x01,
    link            = 0x02,
    event_0         = 0x03,
    event_a         = 0x03,
    event_1         = 0x04,
    event_b         = 0x04,
    pin             = 0x05,
    ac              = 0x06,
    ac0             = 0x06,
    tcb0            = 0x07,
    tcb             = 0x07,
    tca             = 0x08,
    tca0            = 0x08,
    tcd             = 0x09,
    tcd0            = 0x09,
    usart           = 0x0A,
    usart0          = 0x0A,
    spi             = 0x0B,
    spi0            = 0x0B,
    ac1             = 0x0C,
    tcb1            = 0x0D,
    ac2             = 0x0E,
    input_pullup    = 0x15,
    input           = 0x25,
    input_no_pullup = 0x25,
    #endif
  };
};

// Use out:: when working with logic outputs
namespace out {
  enum output_t : uint8_t {
    disable  = 0x00,
    enable   = 0x01,
  };
  enum pinswap_t : uint8_t {
    no_swap  = 0x00,
    pin_swap = 0x01,
  };
};

// Use filter:: when working with logic output filter
namespace filter {
  enum filter_t : uint8_t {
    disable      = 0x00,
    synchronizer = 0x01,
    synch        = 0x01,
    sync         = 0x01,
    filter       = 0x02,
  };
};

// Use clocksource:: when working with logic clock source
namespace clocksource {
  enum clocksource_t : uint8_t {
    clk_per      = 0x00,
    in2          = 0x01,
    #ifdef CCL_CLKSEL_gm
    oschf        = 0x04,
    osc32k       = 0x05,
    osc1k        = 0x06,
    #endif
  };
};

// Use edgedetect:: when using edge detection with filter
namespace edgedetect {
  enum edgedet_t : uint8_t {
    disable      = 0x00,
    enable       = 0x01
  };
};

// Use sequencer:: when working with LUT sequencer
namespace sequencer {
  enum sequencer_t : uint8_t {
    disable      = 0x00,
    d_flip_flop  = 0x01,
    jk_flip_flop = 0x02,
    d_latch      = 0x03,
    sr_latch     = 0x04,
    rs_latch     = 0x04,
  };
};

class Logic {
  public:
    static void start(bool state = true);
    static void stop();

    Logic(const uint8_t block_number);
    void init();
    #if defined(CCL_CCL_vect)
    void attachInterrupt(voidFuncPtr callback, uint8_t mode);
    void detachInterrupt();
    #endif

    bool enable;
    in::input_t input0;
    in::input_t input1;
    in::input_t input2;
    out::output_t output;
    out::pinswap_t output_swap;
    filter::filter_t filter;
    edgedetect::edgedet_t edgedetect;
    uint8_t truth;
    sequencer::sequencer_t sequencer;
    clocksource::clocksource_t clocksource;

    struct CCLBlock;

  private:
    const struct CCLBlock &block;

    void initInput(in::input_t &input, PORT_t &port, const uint8_t pin_bm);
};

#if defined(CCL_TRUTH0)
  extern Logic Logic0;
#endif
#if defined(CCL_TRUTH1)
  extern Logic Logic1;
#endif
#if defined(CCL_TRUTH2)
  extern Logic Logic2;
#endif
#if defined(CCL_TRUTH3)
  extern Logic Logic3;
#endif
#if defined(CCL_TRUTH4)
  extern Logic Logic4;
#endif
#if defined(CCL_TRUTH5)
  extern Logic Logic5;
#endif

#endif
