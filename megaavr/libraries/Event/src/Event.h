#ifndef EVENT_H
#define EVENT_H

#include <Arduino.h>

// *INDENT-OFF* astyle hates how we formatted this.
// I mean, I do too, but I hated all the alternatives we tried even more.
// Readable code always takes priority over formatting dogma. -Spence

// Features present on all generator channels

#if MEGATINYCORE_SERIES == 2

  namespace gen {
    enum generator_t : uint8_t {
      disable         = 0x00,
      updi_synch      = 0x1,
      rtc_ovf         = 0x6,
      rtc_cmp         = 0x7,
      ccl0_out        = 0x10,
      ccl1_out        = 0x11,
      ccl2_out        = 0x12,
      ccl3_out        = 0x13,
      ac0_out         = 0x20,
      adc0_ready      = 0x24,
      adc0_sample     = 0x25,
      adc0_window     = 0x26,
      usart0_xck      = 0x60,
      usart1_xck      = 0x61,
      spi0_sck        = 0x68,
      tca0_ovf_lunf   = 0x80,
      tca0_hunf       = 0x81,
      tca0_cmp0       = 0x84,
      tca0_cmp1       = 0x85,
      tca0_cmp2       = 0x86,
      tcb0_capt       = 0xA0,
      tcb0_ovf        = 0xA1,
      tcb1_capt       = 0xA2,
      tcb1_ovf        = 0xA3,
    };
  };

  namespace gen0 {
    enum generator_t : uint8_t {
      disable     = 0x00,
      rtc_div8192 = 0x08,
      rtc_div4096 = 0x09,
      rtc_div2048 = 0x0A,
      rtc_div1024 = 0x0B,
      pin_pa0     = 0x40,
      pin_pa1     = 0x41,
      pin_pa2     = 0x42,
      pin_pa3     = 0x43,
      pin_pa4     = 0x44,
      pin_pa5     = 0x45,
      pin_pa6     = 0x46,
      pin_pa7     = 0x47,
      pin_pb0     = 0x40,
      pin_pb1     = 0x41,
      pin_pb2     = 0x42,
      pin_pb3     = 0x43,
#     if (defined(__AVR_ATtinyxy7__) || defined(__AVR_ATtinyxy6__))
      pin_pb4     = 0x44,
      pin_pb5     = 0x45,
#     endif
#     if defined(__AVR_ATtinyxy7__)
      pin_pb6     = 0x46,
      pin_pb7     = 0x47,
#     endif
    };
  };

  namespace gen1 {
    enum generator_t : uint8_t {
      disable     = 0x00,
      rtc_div512  = 0x08,
      rtc_div256  = 0x09,
      rtc_div128  = 0x0A,
      rtc_div64   = 0x0B,
      pin_pa0     = 0x40,
      pin_pa1     = 0x41,
      pin_pa2     = 0x42,
      pin_pa3     = 0x43,
      pin_pa4     = 0x44,
      pin_pa5     = 0x45,
      pin_pa6     = 0x46,
      pin_pa7     = 0x47,
      pin_pb0     = 0x40,
      pin_pb1     = 0x41,
      pin_pb2     = 0x42,
      pin_pb3     = 0x43,
#     if (defined(__AVR_ATtinyxy7__) || defined(__AVR_ATtinyxy6__))
      pin_pb4     = 0x44,
      pin_pb5     = 0x45,
#     endif
#     if defined(__AVR_ATtinyxy7__)
      pin_pb6     = 0x46,
      pin_pb7     = 0x47,
#     endif
    };
  };

  namespace gen2 {
    enum generator_t : uint8_t {
      disable     = 0x00,
      rtc_div8192 = 0x08,
      rtc_div4096 = 0x09,
      rtc_div2048 = 0x0A,
      rtc_div1024 = 0x0B,
#     if (defined(__AVR_ATtinyxy7__) || defined(__AVR_ATtinyxy6__))
      pin_pc0     = 0x40,
      pin_pc1     = 0x41,
      pin_pc2     = 0x42,
      pin_pc3     = 0x43,
#     endif
#     if defined(__AVR_ATtinyxy7__)
      pin_pc4     = 0x44,
      pin_pc5     = 0x45,
#     endif
      pin_pa0     = 0x40,
      pin_pa1     = 0x41,
      pin_pa2     = 0x42,
      pin_pa3     = 0x43,
      pin_pa4     = 0x44,
      pin_pa5     = 0x45,
      pin_pa6     = 0x46,
      pin_pa7     = 0x47,
    };
  };

  namespace gen3 {
    enum generator_t : uint8_t {
      disable     = 0x00,
      rtc_div512  = 0x08,
      rtc_div256  = 0x09,
      rtc_div128  = 0x0A,
      rtc_div64   = 0x0B,
#     if (defined(__AVR_ATtinyxy7__) || defined(__AVR_ATtinyxy6__))
      pin_pc0     = 0x40,
      pin_pc1     = 0x41,
      pin_pc2     = 0x42,
      pin_pc3     = 0x43,
#     endif
#     if defined(__AVR_ATtinyxy7__)
      pin_pc4     = 0x44,
      pin_pc5     = 0x45,
#     endif
      pin_pa0     = 0x40,
      pin_pa1     = 0x41,
      pin_pa2     = 0x42,
      pin_pa3     = 0x43,
      pin_pa4     = 0x44,
      pin_pa5     = 0x45,
      pin_pa6     = 0x46,
      pin_pa7     = 0x47,
    };
  };

  namespace gen4 {
    enum generator_t : uint8_t {
      disable     = 0x00,
      rtc_div8192 = 0x08,
      rtc_div4096 = 0x09,
      rtc_div2048 = 0x0A,
      rtc_div1024 = 0x0B,
      pin_pb0     = 0x40,
      pin_pb1     = 0x41,
      pin_pb2     = 0x42,
      pin_pb3     = 0x43,
#     if (defined(__AVR_ATtinyxy7__) || defined(__AVR_ATtinyxy6__))
      pin_pb4     = 0x44,
      pin_pb5     = 0x45,
#     endif
#     if defined(__AVR_ATtinyxy7__)
      pin_pb6     = 0x46,
      pin_pb7     = 0x47,
#     endif
#     if (defined(__AVR_ATtinyxy7__) || defined(__AVR_ATtinyxy6__))
      pin_pc0     = 0x40,
      pin_pc1     = 0x41,
      pin_pc2     = 0x42,
      pin_pc3     = 0x43,
#     endif
#     if defined(__AVR_ATtinyxy7__)
      pin_pc4     = 0x44,
      pin_pc5     = 0x45,
#     endif
    };
  };

  namespace gen5 {
    enum generator_t : uint8_t {
      disable     = 0x00,
      rtc_div512  = 0x08,
      rtc_div256  = 0x09,
      rtc_div128  = 0x0A,
      rtc_div64   = 0x0B,
      pin_pb0     = 0x40,
      pin_pb1     = 0x41,
      pin_pb2     = 0x42,
      pin_pb3     = 0x43,
#     if (defined(__AVR_ATtinyxy7__) || defined(__AVR_ATtinyxy6__))
      pin_pb4     = 0x44,
      pin_pb5     = 0x45,
#     endif
#     if defined(__AVR_ATtinyxy7__)
      pin_pb6     = 0x46,
      pin_pb7     = 0x47,
#     endif
#     if (defined(__AVR_ATtinyxy7__) || defined(__AVR_ATtinyxy6__))
      pin_pc0     = 0x40,
      pin_pc1     = 0x41,
      pin_pc2     = 0x42,
      pin_pc3     = 0x43,
#     endif
#     if defined(__AVR_ATtinyxy7__)
      pin_pc4     = 0x44,
      pin_pc5     = 0x45,
#     endif
    };
  };


  namespace user {
    enum user_t : uint8_t {
      ccl0_event_a            = 0x00,
      ccl0_event_b            = 0x01,
      ccl1_event_a            = 0x02,
      ccl1_event_b            = 0x03,
      ccl2_event_a            = 0x04,
      ccl2_event_b            = 0x05,
      ccl3_event_a            = 0x06,
      ccl3_event_b            = 0x07,
      adc0_start              = 0x08,
      evouta_pin_pa2          = 0x09,
      evouta_pin_pa7          = 0x89,
      evoutb_pin_pb2          = 0x0A,
      evoutb_pin_pb7          = 0x8A,
#     if (defined(__AVR_ATtinyxy7__) || defined(__AVR_ATtinyxy6__))
      evoutc_pin_pc2          = 0x0B,
#     endif
      usart0_irda             = 0x0C,
      usart1_irda             = 0x0D,
      tca0_cnt_a              = 0x0E,
      tca0_cnt_b              = 0x0F,
      tcb0_capt               = 0x11,
      tcb0_cnt                = 0x12,
      tcb1_capt               = 0x13,
      tcb1_cnt                = 0x14,
    };
  };

# else //0-series or 1-series

  namespace gen0 {
    enum generator_t : uint8_t {
#     if (defined(__AVR_ATtinyxy7__) || defined(__AVR_ATtinyxy6__))
      pin_pc0      = 0x07,
      pin_pc1      = 0x08,
      pin_pc2      = 0x09,
      pin_pc3      = 0x0A,
#     endif
#     if defined(__AVR_ATtinyxy7__)
      pin_pc4      = 0x0B,
      pin_pc5      = 0x0C,
#     endif
      pin_pa0      = 0x0D,
      pin_pa1      = 0x0E,
      pin_pa2      = 0x0F,
      pin_pa3      = 0x10,
      pin_pa4      = 0x11,
      pin_pa5      = 0x12,
      pin_pa6      = 0x13,
      pin_pa7      = 0x14,
#     if (PROGMEM_SIZE > 8192 && MEGATINYCORE_SERIES == 1)
      tcb1_capt    = 0x15,
#     endif
    };
  };

#  if MEGATINYCORE_SERIES == 1 && !defined(__AVR_ATtinyxy2__)
    /* Only 1-series parts have second sync channel.
       Only on parts with > 8 pins does it have any unique options */
    namespace gen1 {
      enum generator_t : uint8_t {
        pin_pb0      = 0x08,
        pin_pb1      = 0x09,
        pin_pb2      = 0x0A,
        pin_pb3      = 0x0B,
#       if (defined(__AVR_ATtinyxy7__) || defined(__AVR_ATtinyxy6__))
        pin_pb4      = 0x0C,
        pin_pb5      = 0x0D,
#       endif
#       if defined(__AVR_ATtinyxy7__)
        pin_pb6      = 0x0E,
        pin_pb7      = 0x0F,
#       endif
#       if (PROGMEM_SIZE > 8192) /* Only 16/32k 1-series, but only 1-series is here */
        tcb1_capt    = 0x10,
#       endif
      };
    };
#  endif

  namespace gen2 {
    enum generator_t : uint8_t {
      pin_pa0      = 0x0A,
      pin_pa1      = 0x0B,
      pin_pa2      = 0x0C,
      pin_pa3      = 0x0D,
      pin_pa4      = 0x0E,
      pin_pa5      = 0x0F,
      pin_pa6      = 0x10,
      pin_pa7      = 0x11,
      updi         = 0x12,
#     if (PROGMEM_SIZE > 8192 && MEGATINYCORE_SERIES == 1)
      ac1_out      = 0x13,
      ac2_out      = 0x14,
#     endif
    };
  };

#  if !defined(__AVR_ATtinyxy2__)
    /* Only on parts with > 8 pins does this have any unique options */
    namespace gen3 {
      enum generator_t : uint8_t {
        pin_pb0      = 0x0A,
        pin_pb1      = 0x0B,
        pin_pb2      = 0x0C,
        pin_pb3      = 0x0D,
#       if (defined(__AVR_ATtinyxy7__) || defined(__AVR_ATtinyxy6__))
        pin_pb4      = 0x0E,
        pin_pb5      = 0x0F,
#       endif
#       if defined(__AVR_ATtinyxy7__)
        pin_pb6      = 0x10,
        pin_pb7      = 0x11,
#       endif
#       if (PROGMEM_SIZE > 8192 && MEGATINYCORE_SERIES == 1)
        ac1_out      = 0x12,
        ac2_out      = 0x13,
#       endif
      };
    };
#  endif

#  if MEGATINYCORE_SERIES == 1
    /* Only 1-series parts have third and fourth async sync channel.
       and only parts with 20/24 pins, or the 1614 have any items on this list available */
#   if !(defined(__AVR_ATtinyxy2__) || (defined(__AVR_ATtinyxy4__) && PROGMEM_SIZE <= 8192))
    namespace gen4 {
      enum generator_t : uint8_t {
#       if (defined(__AVR_ATtinyxy7__) || defined(__AVR_ATtinyxy6__))
        pin_pc0      = 0x0A,
        pin_pc1      = 0x0B,
        pin_pc2      = 0x0C,
        pin_pc3      = 0x0D,
#       endif
#       if defined(__AVR_ATtinyxy7__)
        pin_pc4      = 0x0E,
        pin_pc5      = 0x0F,
#       endif
#       if (PROGMEM_SIZE > 8192) /* Only 16/32k 1-series, but only 1-series is here */
        ac1_out      = 0x10,
        ac2_out      = 0x11,
#       endif
      };
    };
#  endif
  /* Nope, no RTC PIT events for 0-series! */
  namespace gen5 {
    enum generator_t : uint8_t {
      rtc_div8192       = 0x0A,
      rtc_div4096       = 0x0B,
      rtc_div2048       = 0x0C,
      rtc_div1024       = 0x0D,
      rtc_div512        = 0x0E,
      rtc_div256        = 0x0F,
      rtc_div128        = 0x10,
      rtc_div64         = 0x11,
#     if (PROGMEM_SIZE > 8192) /* Only 16/32k 1-series, but only 1-series is here */
      ac1_out           = 0x12,
      ac2_out           = 0x13,
#     endif
    };
  };
#  endif // end of  1-series only part.

  namespace gens {
    enum generator_t : uint8_t {
      disable           = 0x00,
      off               = 0x00,
      tcb0_capt         = 0x01,
      tca0_ovf_lunf     = 0x02,
      tca0_hunf         = 0x03,
      tca0_cmp0         = 0x04,
      tca0_cmp1         = 0x05,
      tca0_cmp2         = 0x06,
    };
  };

  namespace gen {
    enum generator_t : uint8_t {
      disable           = 0x00,
      off               = 0x00,
      ccl_lut0          = 0x01,
      ccl_lut1          = 0x02,
      ac0_out           = 0x03,
#     if MEGATINYCORE_SERIES == 1
      tcd0_cmpbclr      = 0x04,
      tcd0_cmpaset      = 0x05,
      tcd0_cmpbset      = 0x06,
      tcd0_progev       = 0x07,
#     endif
      rtc_ovf           = 0x08,
      rtc_cmp           = 0x09,
    };
  };



  namespace user {
    enum user_t : uint8_t {
      tcb0_capt               = 0x00,
      adc0_start              = 0x01,
      ccl0_event_a            = 0x02,
      ccl1_event_a            = 0x03,
      ccl0_event_b            = 0x04,
      ccl1_event_b            = 0x05,
      tcd0_in_a               = 0x06,
      tcd0_in_b               = 0x07,
      evouta_pin_pa2          = 0x08,
      evoutb_pin_pb2          = 0x09,
      evoutc_pin_pc2          = 0x0A,
      tcb1_capt               = 0x0B,
      adc1_start              = 0x0c,
      tca0_cnt_a              = 0x10,
      usart0_irda             = 0x11,
    };
  };


#endif /* Finally, an end to all the enums! */


class Event {
  public:
    Event(uint8_t channel_num, volatile uint8_t &channel_addr);
    uint8_t get_channel_number();
    static int8_t get_user_channel(user::user_t event_user);
    void set_generator(uint8_t generator);
    uint8_t get_generator();
    void set_user(user::user_t event_user);
    static void clear_user(user::user_t event_user);
    void soft_event();
    void start(bool state = true);
    void stop();

  private:
    const uint8_t channel_number;      // Holds the event generator channel number
    volatile uint8_t &channel_address; // Reference to the event channel address
    uint8_t generator_type;            // Generator type the event channel is using
};

#if MEGATINYCORE_SERIES == 2
  #if defined(EVSYS_CHANNEL0)
    extern Event Event0;
  #endif
  #if defined(EVSYS_CHANNEL0)
    extern Event Event1;
  #endif
  #if defined(EVSYS_CHANNEL0)
    extern Event Event2;
  #endif
  #if defined(EVSYS_CHANNEL0)
    extern Event Event3;
  #endif
  #if defined(EVSYS_CHANNEL0)
    extern Event Event4;
  #endif
  #if defined(EVSYS_CHANNEL0)
    extern Event Event5;
  #endif
#else
  #if defined(EVSYS_SYNCCH0)
    extern Event Event0;
    #define EventSync0    Event0
  #endif
  #if defined(EVSYS_SYNCCH1)
    extern Event Event1;
    #define EventSync1    Event1
  #endif
  #if defined(EVSYS_ASYNCCH0)
    extern Event Event2;
    #define EventAsync0    Event2
  #endif
  #if defined(EVSYS_ASYNCCH1)
    extern Event Event3;
    #define EventAsync1    Event3
  #endif
  #if defined(EVSYS_ASYNCCH2)
    extern Event Event4;
    #define EventAsync2    Event4
  #endif
  #if defined(EVSYS_ASYNCCH3)
    extern Event Event5;
    #define EventAsync3    Event5
  #endif
#endif

#endif // EVENT_H
