#ifndef LOGIC_h
#define LOGIC_h

#include <Arduino.h>

namespace logic {
  namespace in {
    enum input_t : uint8_t {
      #if defined(__AVR_ATmega808__) || defined(__AVR_ATmega1608__) || \
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
      uart            = 0x08,
      usart           = 0x08,
      spi             = 0x09,
      tca0            = 0x0A,
      tca             = 0x0A,
      tcb             = 0x0C,
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
      input_pullup    = 0x15,
      input           = 0x25,
      input_no_pullup = 0x25,
      #elif defined(__AVR_AVR64DD20__)  || defined(__AVR_AVR64DD14__)  ||  \
      defined(__AVR_AVR32DD20__)  || defined(__AVR_AVR32DD14__)  ||  \
      defined(__AVR_AVR16DD20__)  || defined(__AVR_AVR16DD14__)  ||  \
      defined(__AVR_AVR64DD32__)  || defined(__AVR_AVR64DD28__)  ||  \
      defined(__AVR_AVR32DD32__)  || defined(__AVR_AVR32DD28__)  ||  \
      defined(__AVR_AVR16DD32__)  || defined(__AVR_AVR16DD28__)
      // educated guess - these have at least one of everything the rest of DA-series has,
      // and these lists have been super consistent since v1.0 (tinyAVR 0/1-series)
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

      #else //tinyAVR 0/1-series
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
};

// Legacy definitions
namespace in          { using namespace logic::in;          };
namespace out         { using namespace logic::out;         };
namespace filter      { using namespace logic::filter;      };
namespace clocksource { using namespace logic::clocksource; };
namespace edgedetect  { using namespace logic::edgedetect;  };
namespace sequencer   { using namespace logic::sequencer;   };

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
    logic::in::input_t                input0;
    logic::in::input_t                input1;
    logic::in::input_t                input2;
    logic::out::output_t              output;
    logic::out::pinswap_t             output_swap;
    logic::filter::filter_t           filter;
    logic::edgedetect::edgedet_t      edgedetect;
    uint8_t truth;
    logic::sequencer::sequencer_t     sequencer;
    logic::clocksource::clocksource_t clocksource;

    struct CCLBlock;

  private:
    const struct CCLBlock &block;

    void initInput(logic::in::input_t &input, PORT_t &port, const uint8_t pin_bm);
};

// Array for storing ISR function pointers
#if defined(CCL_CCL_vect)
  #if defined(TRUTH5)
    static volatile voidFuncPtr intFuncCCL[6];
  #else
    static volatile voidFuncPtr intFuncCCL[4];
  #endif
#endif

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
  #if defined(__AVR_ATtiny406__) || defined(__AVR_ATtiny806__) || \
  defined(__AVR_ATtiny1606__) ||                              \
  defined(__AVR_ATtiny416__) || defined(__AVR_ATtiny816__) || \
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
  #if defined(__AVR_ATtiny426__) || defined(__AVR_ATtiny826__) || \
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
  defined(__AVR_ATtiny1617__) || defined(__AVR_ATtiny3217__)
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
  #if defined(__AVR_AVR128DA64__) || defined(__AVR_AVR128DA48__) || \
  defined(__AVR_AVR64DA64__) || defined(__AVR_AVR64DA48__)   || \
  defined(__AVR_AVR32DA64__) || defined(__AVR_AVR32DA48__)   || \
  defined(__AVR_AVR128DB64__) || defined(__AVR_AVR128DB48__) || \
  defined(__AVR_AVR64DB64__) || defined(__AVR_AVR64DB48__)   || \
  defined(__AVR_AVR32DB64__) || defined(__AVR_AVR32DB48__)
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
