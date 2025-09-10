/* This file is ONLY included by Event.h and should never be included by any other code under any circumstances.
 * It is loaded at the start of Event.h so that the same code can be used even if the part is a tinyAVR 0/1-series with wacky event system.
 * Thia does a lot for tinyAVR 0/1-series, very little for 2-series, and nothing for anything else.
 */
#if !defined(EVENT_H)
  #error "This should only be included as part of Event.h"
#endif

// *INDENT-OFF* astyle hates how we formatted this.
// I mean, I do too, but I hated all the alternatives we tried even more.
// Readable code always takes priority over formatting dogma. -Spence

#ifndef EVENT_PARTS_H
#define EVENT_PARTS_H

// Features present on all generator channels

namespace event {
#if !defined(MEGATINYCORE) && !defined(PORT_EVGEN0SEL_gm) //if neither tiny nor EA or beyond, here's the list
  namespace gen {
    enum generator_t : uint8_t {
      disable       = 0x00,
      off           = 0x00,
      updi_synch    = 0x01,
      rtc_ovf       = 0x06,
      rtc_cmp       = 0x07,
      ccl0_out      = 0x10,
      ccl1_out      = 0x11,
      ccl2_out      = 0x12,
      ccl3_out      = 0x13,
      ac0_out       = 0x20,
#if defined(AC1)
      ac1_out       = 0x21,
#endif
#if defined(AC2)
      ac2_out       = 0x22,
#endif
      adc0_ready    = 0x24,
      usart0_xck    = 0x60,
      usart1_xck    = 0x61,
      usart2_xck    = 0x62,
#if defined(USART3)
      usart3_xck    = 0x63,
#endif
      spi0_sck      = 0x68,
      tca0_ovf_lunf = 0x80,
      tca0_hunf     = 0x81,
      tca0_cmp0     = 0x84,
      tca0_cmp1     = 0x85,
      tca0_cmp2     = 0x86,
      tcb0_capt     = 0xA0,
      tcb0          = 0xA0,
      tcb1_capt     = 0xA2,
      tcb1          = 0xA2,
      tcb2_capt     = 0xA4,
      tcb2          = 0xA4,
#if defined(TCB3)
      tcb3_capt     = 0xA6,
      tcb3          = 0xA6,
#endif
#if defined(TCB4)
      tcb4_capt      = 0xA8,
      tcb4           = 0xA8,
#endif
#if defined(__AVR_DA__) || defined(__AVR_DB__)
      ccl4_out      = 0x14,
      ccl5_out      = 0x15,
      zcd0_out      = 0x30,
      zcd1_out      = 0x31,
#if defined(ZCD2)
      zcd2_out      = 0x32,
#endif
#if defined(USART4)
      usart4_xck    = 0x64,
#endif
#if defined(USART5)
      usart5_xck    = 0x65,
#endif
      spi1_sck      = 0x69,
#if defined(TCA1)
      tca1_ovf_lunf = 0x88,
      tca1_hunf     = 0x89,
      tca1_cmp0     = 0x8C,
      tca1_cmp1     = 0x8D,
      tca1_cmp2     = 0x8E,
#endif
#endif // defined(__AVR_DA__) || defined(__AVR_DB__)
#if defined(DXCORE)
      tcb0_ovf      = 0xA1,
      tcb1_ovf      = 0xA3,
#if defined(TCB2)
      tcb2_ovf      = 0xA5,
#endif
#if defined(TCB3)
      tcb3_ovf      = 0xA7,
#endif
#if defined(TCB4)
      tcb4_ovf      = 0xA9,
#endif
#endif
#if defined(TCD0)
      tcd0_cmpbclr  = 0xB0,
      tcd0_cmpaset  = 0xB1,
      tcd0_cmpbset  = 0xB2,
      tcd0_progev   = 0xB3,
#endif
#if defined(MVIO)
      mvio_ok       = 0x05,
#endif
#if defined(ZCD3)
      zcd3_out      = 0x30, // The ZCD is numbered differently, I think just because it's on different pins. Still has the same event channel
#endif
#if defined(__AVR_DB__)
      opamp0_ready  = 0x34,
      opamp1_ready  = 0x35,
#if defined (OPAMP2)
      opamp2_ready  = 0x36,
#endif
#endif // defined(__AVR_DB__)
    };
  };

  // Features unique to event generator channel 0
#if defined(EVSYS_CHANNEL0)
  namespace gen0 {
    enum generator_t : uint8_t {
      disable     = 0x00,
      off         = 0x00,
      rtc_div8192 = 0x08,
      rtc_div4096 = 0x09,
      rtc_div2048 = 0x0A,
      rtc_div1024 = 0x0B,
      pin_pa0     = 0x40,
      pin_pa1     = 0x41,
#if defined(PIN_PA2)
      pin_pa2     = 0x42,
      pin_pa3     = 0x43,
      pin_pa4     = 0x44,
      pin_pa5     = 0x45,
      pin_pa6     = 0x46,
      pin_pa7     = 0x47,
#endif
#if defined(PIN_PB0)
      pin_pb0     = 0x48,
      pin_pb1     = 0x49,
      pin_pb2     = 0x4A,
      pin_pb3     = 0x4B,
      pin_pb4     = 0x4C,
      pin_pb5     = 0x4F,
#endif
#if defined(PIN_PB6)
      pin_pb6     = 0x4E,
      pin_pb7     = 0x4F,
#endif
    };
  };
#endif

  // Features unique to event generator channel 1
#if defined(EVSYS_CHANNEL1)
  namespace gen1 {
    enum generator_t : uint8_t {
      disable     = 0x00,
      off         = 0x00,
      rtc_div512  = 0x08,
      rtc_div256  = 0x09,
      rtc_div128  = 0x0A,
      rtc_div64   = 0x0B,
      pin_pa0     = 0x40,
      pin_pa1     = 0x41,
#if defined(PIN_PA2)
      pin_pa2     = 0x42,
      pin_pa3     = 0x43,
      pin_pa4     = 0x44,
      pin_pa5     = 0x45,
      pin_pa6     = 0x46,
      pin_pa7     = 0x47,
#endif
#if defined(PIN_PB0)
      pin_pb0     = 0x48,
      pin_pb1     = 0x49,
      pin_pb2     = 0x4A,
      pin_pb3     = 0x4B,
      pin_pb4     = 0x4C,
      pin_pb5     = 0x4F,
#endif
#if defined(PIN_PB6)
      pin_pb6     = 0x4E,
      pin_pb7     = 0x4F,
#endif
    };
  };
#endif

  // Features unique to event generator channel 2
#if defined(EVSYS_CHANNEL2)
  namespace gen2 {
    enum generator_t : uint8_t {
      disable     = 0x00,
      off         = 0x00,
      rtc_div8192 = 0x08,
      rtc_div4096 = 0x09,
      rtc_div2048 = 0x0A,
      rtc_div1024 = 0x0B,
#if defined(PIN_PC0) && !defined(FAKE_PIN_PC0)
      pin_pc0     = 0x40,
#endif
      pin_pc1     = 0x41,
      pin_pc2     = 0x42,
      pin_pc3     = 0x43,
#if defined(PIN_PC4)
      pin_pc4     = 0x44,
      pin_pc5     = 0x45,
      pin_pc6     = 0x46,
      pin_pc7     = 0x47,
#endif
#if defined(PIN_PD0) && !defined(FAKE_PIN_PD0)
      pin_pd0     = 0x48,
#endif
#if defined(PIN_PD1)
      pin_pd1     = 0x49,
      pin_pd2     = 0x4A,
      pin_pd3     = 0x4B,
#endif
      pin_pd4     = 0x4C,
      pin_pd5     = 0x4F,
      pin_pd6     = 0x4E,
      pin_pd7     = 0x4F,
    };
  };
#endif

  // Features unique to event generator channel 3
#if defined(EVSYS_CHANNEL3)
  namespace gen3 {
    enum generator_t : uint8_t {
      disable     = 0x00,
      off         = 0x00,
      rtc_div512  = 0x08,
      rtc_div256  = 0x09,
      rtc_div128  = 0x0A,
      rtc_div64   = 0x0B,
#if !defined(DD_14_PINS)
      pin_pc0     = 0x40,
#endif
      pin_pc1     = 0x41,
      pin_pc2     = 0x42,
      pin_pc3     = 0x43,
#if defined(PIN_PC4)
      pin_pc4     = 0x44,
      pin_pc5     = 0x45,
      pin_pc6     = 0x46,
      pin_pc7     = 0x47,
#endif
#if defined(PIN_PD0) && !defined(FAKE_PIN_PD0) //many parts are missing this, but PIN_PD0 is used too many places internally to leave out of those parts.
      pin_pd0     = 0x48,
#endif
#if defined(PIN_PD1)
      pin_pd1     = 0x49,
      pin_pd2     = 0x4A,
      pin_pd3     = 0x4B,
#endif
      pin_pd4     = 0x4C,
      pin_pd5     = 0x4F,
      pin_pd6     = 0x4E,
      pin_pd7     = 0x4F,
    };
  };
#endif

  // Features unique to event generator channel 4
#if defined(EVSYS_CHANNEL4)
  namespace gen4 {
    enum generator_t : uint8_t {
      disable     = 0x00,
      off         = 0x00,
      rtc_div8192 = 0x08,
      rtc_div4096 = 0x09,
      rtc_div2048 = 0x0A,
      rtc_div1024 = 0x0B,
#if defined(PIN_PE0)
      pin_pe0     = 0x40,
      pin_pe1     = 0x41,
      pin_pe2     = 0x42,
      pin_pe3     = 0x43,
#endif
#if defined(PIN_PE4)
      pin_pe4     = 0x44,
      pin_pe5     = 0x45,
      pin_pe6     = 0x46,
      pin_pe7     = 0x47,
#endif
      pin_pf0     = 0x48,
      pin_pf1     = 0x49,
#if defined(PIN_PF2)
      pin_pf2     = 0x4A,
      pin_pf3     = 0x4B,
      pin_pf4     = 0x4C,
      pin_pf5     = 0x4D,
#endif
      pin_pf6     = 0x4E,
#if defined(PIN_PF7)
      pin_pf7     = 0x4F
#endif
    };
  };
#endif

  // Features unique to event generator channel 5
#if defined(EVSYS_CHANNEL5)
  namespace gen5 {
    enum generator_t : uint8_t {
      disable     = 0x00,
      off         = 0x00,
      rtc_div512  = 0x08,
      rtc_div256  = 0x09,
      rtc_div128  = 0x0A,
      rtc_div64   = 0x0B,
#if defined(PIN_PE0)
      pin_pe0     = 0x40,
      pin_pe1     = 0x41,
      pin_pe2     = 0x42,
      pin_pe3     = 0x43,
#endif
#if defined(PIN_PE4)
      pin_pe4     = 0x44,
      pin_pe5     = 0x45,
      pin_pe6     = 0x46,
      pin_pe7     = 0x47,
#endif
      pin_pf0     = 0x48,
      pin_pf1     = 0x49,
#if defined(PIN_PF2)
      pin_pf2     = 0x4A,
      pin_pf3     = 0x4B,
      pin_pf4     = 0x4C,
      pin_pf5     = 0x4D,
#endif
      pin_pf6     = 0x4E,
#if defined(PIN_PF7)
      pin_pf7     = 0x4F
#endif
    };
  };
#endif

  // Features unique to event generator channel 6
#if defined(EVSYS_CHANNEL6)
  namespace gen6 {
    enum generator_t : uint8_t {
      disable     = 0x00,
      off         = 0x00,
      rtc_div8192 = 0x08,
      rtc_div4096 = 0x09,
      rtc_div2048 = 0x0A,
      rtc_div1024 = 0x0B,
#if defined(Dx_64_PINS)
      pin_pg0     = 0x40,
      pin_pg1     = 0x41,
      pin_pg2     = 0x42,
      pin_pg3     = 0x43,
      pin_pg4     = 0x44,
      pin_pg5     = 0x45,
      pin_pg6     = 0x46,
      pin_pg7     = 0x47,
#endif
    };
  };
#endif

  // Features unique to event generator channel 7
#if defined(EVSYS_CHANNEL7)
  namespace gen7 {
    enum generator_t : uint8_t {
      disable    = 0x00,
      off        = 0x00,
      rtc_div512 = 0x08,
      rtc_div256 = 0x09,
      rtc_div128 = 0x0A,
      rtc_div64  = 0x0B,
#if defined(Dx_64_PINS)
      pin_pg0     = 0x40,
      pin_pg1     = 0x41,
      pin_pg2     = 0x42,
      pin_pg3     = 0x43,
      pin_pg4     = 0x44,
      pin_pg5     = 0x45,
      pin_pg6     = 0x46,
      pin_pg7     = 0x47,
#endif
    };
  };
#endif

  // Features unique to event generator channel 8
#if defined(EVSYS_CHANNEL8)
  namespace gen8 {
    enum generator_t : uint8_t {
      disable     = 0x00,
      off         = 0x00,
      rtc_div8192 = 0x08,
      rtc_div4096 = 0x09,
      rtc_div2048 = 0x0A,
      rtc_div1024 = 0x0B,
    };
  };
#endif

  // Features unique to event generator channel 9
#if defined(EVSYS_CHANNEL9)
  namespace gen9 {
    enum generator_t : uint8_t {
      disable    = 0x00,
      off        = 0x00,
      rtc_div512 = 0x08,
      rtc_div256 = 0x09,
      rtc_div128 = 0x0A,
      rtc_div64  = 0x0B,
    };
  };
#endif


  // Generator users
  namespace user {
    enum user_t : uint8_t {
#if defined(MEGACOREX)
      ccl0_event_a   = 0x00,
      ccl0_event_b   = 0x01,
      ccl1_event_a   = 0x02,
      ccl1_event_b   = 0x03,
      ccl2_event_a   = 0x04,
      ccl2_event_b   = 0x05,
      ccl3_event_a   = 0x06,
      ccl3_event_b   = 0x07,
      adc0_start     = 0x08,
      evouta_pin_pa2 = 0x09,
#if defined(PIN_PB2)
      evoutb_pin_pb2 = 0x0A,
#endif
      evoutc_pin_pc2 = 0x0B,
      evoutd_pin_pd2 = 0x0C,
#if defined(PIN_PE2)
      evoute_pin_pe2 = 0x0D,
#endif
      evoutf_pin_pf2 = 0x0E,
      usart0_irda    = 0x0F,
      usart1_irda    = 0x10,
      usart2_irda    = 0x11,
#if defined(USART3)
      usart3_irda    = 0x12,
#endif
      tca0           = 0x13,
      tca0_cnt_a     = 0x13,
      tcb0           = 0x14,
      tcb0_capt      = 0x14,
      tcb1           = 0x15,
      tcb1_capt      = 0x15,
      tcb2           = 0x16,
      tcb2_capt      = 0x16,
#if defined(TCB3)
      tcb3           = 0x17,
      tcb3_capt      = 0x17,
#endif
      // "Unofficial" user generators. Uses EVOUT, but swaps the output pin using PORTMUX
      evouta_pin_pa7 = 0x89,
#if defined(PIN_PC7)
      evoutc_pin_pc7 = 0x8B,
#endif
      evoutd_pin_pd7 = 0x8C,
#endif // defined(__AVR_ATmegax08__) || defined(__AVR_ATmegax09__)

#if defined(__AVR_DA__)
        ccl0_event_a   = 0x00,
        ccl0_event_b   = 0x01,
        ccl1_event_a   = 0x02,
        ccl1_event_b   = 0x03,
        ccl2_event_a   = 0x04,
        ccl2_event_b   = 0x05,
        ccl3_event_a   = 0x06,
        ccl3_event_b   = 0x07,
#if defined(CCL_TRUTH4)
        ccl4_event_a   = 0x08,
        ccl4_event_b   = 0x09,
        ccl5_event_a   = 0x0A,
        ccl5_event_b   = 0x0B,
#endif
        adc0_start     = 0x0C,
        ptc_start      = 0x0D,
        evouta_pin_pa2 = 0x0E,
#if defined(PIN_PB2)
        evoutb_pin_pb2 = 0x0F,
#endif
        evoutc_pin_pc2 = 0x10,
        evoutd_pin_pd2 = 0x11,
#if defined(PIN_PE2)
        evoute_pin_pe2 = 0x12,
#endif
#if defined(PIN_PF2)
        evoutf_pin_pf2 = 0x13,
#endif
#if defined(PIN_PG2)
        evoutg_pin_pg2 = 0x14,
#endif
        usart0_irda    = 0x15,
        usart1_irda    = 0x16,
        usart2_irda    = 0x17,
#if defined(USART3)
        usart3_irda    = 0x18,
#endif
#if defined(USART4)
        usart4_irda    = 0x19,
#endif
#if defined(USART5)
        usart5_irda    = 0x1A,
#endif
        tca0           = 0x1B,
        tca0_cnt_a     = 0x1B,
        tca0_cnt_b     = 0x1C,
        tca1           = 0x1D,
        tca1_cnt_a     = 0x1D,
        tca1_cnt_b     = 0x1E,
        tcb0_capt      = 0x1F,
        tcb0           = 0x1F,
        tcb0_cnt       = 0x20,
        tcb1_capt      = 0x21,
        tcb1           = 0x21,
        tcb1_cnt       = 0x22,
        tcb2_capt      = 0x23,
        tcb2           = 0x23,
        tcb2_cnt       = 0x24,
#if defined(TCB3)
        tcb3_capt      = 0x25,
        tcb3           = 0x25,
        tcb3_cnt       = 0x26,
#endif
#if defined(TCB4)
        tcb4_capt      = 0x27,
        tcb4           = 0x27,
        tcb4_cnt       = 0x28,
#endif
        tcd0_in_a      = 0x29,
        tcd0_in_b      = 0x2A,
        // "Unofficial" generator - high bit is a signal to also switch portmux.
        evouta_pin_pa7 = 0x8E,
#if defined(PIN_PB7)
        evoutb_pin_pb7 = 0x8F,
#endif
#if defined(PIN_PC7)
        evoutc_pin_pc7 = 0x90,
#endif
        evoutd_pin_pd7 = 0x91,
#if defined(PIN_PE7)
        evoute_pin_pe7 = 0x92,
#endif
        // evoutf_pin_pf7 = 0x93, never available on DA/DB
#if defined(PIN_PG7)
        evoutg_pin_pg7 = 0x94,
#endif
#endif // defined(__AVR_DA__)
#if defined(__AVR_DB__)
      ccl0_event_a   = 0x00,
      ccl0_event_b   = 0x01,
      ccl1_event_a   = 0x02,
      ccl1_event_b   = 0x03,
      ccl2_event_a   = 0x04,
      ccl2_event_b   = 0x05,
      ccl3_event_a   = 0x06,
      ccl3_event_b   = 0x07,
#if defined(CCL_TRUTH4)
      ccl4_event_a   = 0x08,
      ccl4_event_b   = 0x09,
      ccl5_event_a   = 0x0A,
      ccl5_event_b   = 0x0B,
#endif
      adc0_start     = 0x0C,
      evouta_pin_pa2 = 0x0D,
#if defined(PIN_PB2)
      evoutb_pin_pb2 = 0x0E,
#endif
      evoutc_pin_pc2 = 0x0F,
      evoutd_pin_pd2 = 0x10,
#if defined(PIN_PE2)
      evoute_pin_pe2 = 0x11,
#endif
#if defined(PIN_PF2)
      evoutf_pin_pf2 = 0x12,
#endif
#if defined(PIN_PG2)
      evoutg_pin_pg2 = 0x13,
    #endif
      usart0_irda    = 0x14,
      usart1_irda    = 0x15,
      usart2_irda    = 0x16,
#if defined(USART3)
      usart3_irda    = 0x17,
#endif
#if defined(USART4)
      usart4_irda    = 0x18,
#endif
#if defined(USART5)
      usart5_irda    = 0x19,
#endif
      tca0           = 0x1A,
      tca0_cnt_a     = 0x1A,
      tca0_cnt_b     = 0x1B,
      tca1           = 0x1C,
      tca1_cnt_a     = 0x1C,
      tca1_cnt_b     = 0x1D,
      tcb0_capt      = 0x1E,
      tcb0_cnt       = 0x1F,
      tcb1_capt      = 0x20,
      tcb1_cnt       = 0x21,
      tcb2_capt      = 0x22,
      tcb2_cnt       = 0x23,
#if defined(TCB3)
      tcb3_capt      = 0x24,
      tcb3_cnt       = 0x25,
#endif
#if defined(TCB4)
      tcb4_capt      = 0x26,
      tcb4_cnt       = 0x27,
#endif
      tcd0_in_a      = 0x28,
      tcd0_in_b      = 0x29,
      opamp0_enable  = 0x2A,
      opamp0_disable = 0x2B,
      opamp0_dump    = 0x2C,
      opamp0_drive   = 0x2D,
      opamp1_enable  = 0x2E,
      opamp1_disable = 0x2F,
      opamp1_dump    = 0x30,
      opamp1_drive   = 0x31,
#if defined(OPAMP2)
      opamp2_enable  = 0x32,
      opamp2_disable = 0x33,
      opamp2_dump    = 0x34,
      opamp2_drive   = 0x35,
#endif
      // "Unofficial" user generators. Uses EVOUT, but swaps the output pin using PORTMUX
      evouta_pin_pa7 = 0x8D,
#if defined(PIN_PB7)
      evoutb_pin_pb7 = 0x8E,
#endif
#if defined(PIN_PC7)
      evoutc_pin_pc7 = 0x8F,
#endif
      evoutd_pin_pd7 = 0x90,
#if defined(PIN_PE7)
      evoute_pin_pe7 = 0x91,
#endif
#if defined(PIN_PG7)
      evoutg_pin_pg7 = 0x93,
#endif
#endif // defined(__AVR_DB__)

#if defined(__AVR_DD__)
      ccl0_event_a   = 0x00,
      ccl0_event_b   = 0x01,
      ccl1_event_a   = 0x02,
      ccl1_event_b   = 0x03,
      ccl2_event_a   = 0x04,
      ccl2_event_b   = 0x05,
      ccl3_event_a   = 0x06,
      ccl3_event_b   = 0x07,
      adc0_start     = 0x08,
#if defined(PIN_PA2) // not on 14-pin ones.
      evouta_pin_pa2 = 0x09,
#endif
      evoutc_pin_pc2 = 0x0A,
#if defined(PIN_PD2) // only on 28 or 32 pin ones.
      evoutd_pin_pd2 = 0x0B,
#endif
#if defined(PIN_PF2) // only on 32-pin ones.
      evoutf_pin_pf2 = 0x0C,
#endif
      usart0_irda    = 0x0D,
      usart1_irda    = 0x0E,
      tca0           = 0x0F,
      tca0_cnt_a     = 0x0F,
      tca0_cnt_b     = 0x10,
      tcb0           = 0x11,
      tcb0_capt      = 0x11,
      tcb0_cnt       = 0x12,
      tcb1           = 0x13,
      tcb1_capt      = 0x13,
      tcb1_cnt       = 0x14,
#if defined(TCB2)
      tcb2           = 0x15,
      tcb2_capt      = 0x15,
      tcb2_cnt       = 0x16,
#endif
      tcd0_in_a      = 0x17,
      tcd0_in_b      = 0x18,
#if defined(PIN_PA7) // not on 14-pin ones.
      evouta_pin_pa7 = 0x89,
#endif
      evoutd_pin_pd7 = 0x8B,
#if defined(PIN_PF7)
      evoutf_pin_pf7 = 0x8C,
#endif
#endif
    };
  };
  // End of Dx series - start of Ex-series //
  #elif defined(PORT_EVGEN0SEL_gm) // Start EVGEN block
    // Parts with uniform event channels, eg, the future EA-series parts. We may be able to use this code for every Ex-series part, or will need only trivial changes.
    namespace gen {
      enum generator_t : uint8_t {
        disable           = 0x00,
        off               = 0x00,
        updi_synch        = 0x01,
#if defined(MVIO)
        mvio_ok         = 0x05,
#endif
        rtc_ovf           = 0x06,
        rtc_cmp           = 0x07,
        rtc_pitev0        = 0x08,
        rtc_pitev1        = 0x09,
        ccl0_out          = 0x10,
        ccl1_out          = 0x11,
        ccl2_out          = 0x12,
        ccl3_out          = 0x13,
#if defined(CCL_LUT4CTRLA) // I fully expect a 6-LUT, 64 pin Ex-series part
        ccl4_out          = 0x14,
        ccl5_out          = 0x15,
#endif
#if defined(CCL_LUT6CTRLA) // Any ATmega2560 replacement would probably have at least 8 LUTs
        ccl6_out          = 0x16,
        ccl7_out          = 0x17,
#endif
#if defined(CCL_LUT8CTRLA) // Maybe even more
        ccl8_out          = 0x18,
        ccl9_out          = 0x19,
#endif
#if defined(CCL_LUT10CTRLA)
        ccl10_out         = 0x1A,
        ccl11_out         = 0x1B,
#endif
#if defined(CCL_LUT12CTRLA)
        ccl2_out          = 0x1C,
        cc13_out          = 0x1D,
#endif
#if defined(CCL_LUT14CTRLA)
        ccl14_out         = 0x1E,
        ccl15_out         = 0x1F,
        // Hey, I can dream can't I? Can you imagine what you could do with 4 TCA's 16 event channels, 16 LUTs and a 3-phase TCD?
        // Forget having to clock your own SPI port in slave mode to get clocked data out at arbitrary baud rates....
        // you could wire the CCL up as an 8 bit shift register at that point, load it with a SWEVENT....
#endif
        ac0_out           = 0x20,
#if defined(AC1) // An Ex with only one AC would not surprise me
        ac1_out           = 0x21,
#endif
#if defined(AC2) // An Ex with three AC would not surprise me
        ac2_out           = 0x21,
#endif
#if defined(AC3) // An Ex with fourAC would not surprise me
        ac3_out           = 0x21,
#endif
        adc0_ready        = 0x24,
        adc0_sample       = 0x25,
        adc0_window       = 0x26,
#if defined(ZCD0) // An Ex with ZCD would not surprise me.
        zcd1_out          = 0x32,
#endif
#if defined(ZCD1)
        zcd1_out          = 0x32,
#endif
#if defined(ZCD2)
        zcd2_out          = 0x32,
#endif
#if defined(OPAMP0) // Shouldn't there be a later Ex-series with opamps? Would synergize great with the improved ADC
        opamp0_ready      = 0x34,
#endif
#if defined(OPAMP1)
        opamp1_ready      = 0x35,
#endif
#if defined(OPAMP2)
        opamp2_ready      = 0x36,
#endif
// Likely the rest of 0x30 would be novel analog feature-related
        porta_evgen0      = 0x40, // All current and announced modern non-tiny parts have a PORTA
        porta_evgen1      = 0x41,
#if defined(PORTB)          // PORTB is only on 48+ pin parts, and the whole thing is only 6 pins.
        portb_evgen0      = 0x42,
        portb_evgen1      = 0x43,
#endif
        portc_evgen0      = 0x44, // All current and announced modern non-tiny parts have a PORTC
        portc_evgen1      = 0x45,
        portd_evgen0      = 0x46, // All current and announced modern non-tiny parts have a PORTD
        portd_evgen1      = 0x47,
#if defined(PORTE)          // PORTE is only on 48+ pin parts.
        porte_evgen0      = 0x48,
        porte_evgen1      = 0x49,
#endif
        portf_evgen0      = 0x4A, // all non-tinies have had PF because that's where reset and UPDI are.
        portf_evgen1      = 0x4B,
#if defined(PORTG)  // One presumes there will be 64 pin Ex-series which would have a PORTG
        portg_evgen0      = 0x4C,
        portg_evgen1      = 0x4D,
#endif
#if defined(PORTH)  // At some point they will need to either surrender a non-trivial amount of market share, or offer a migration path from the m2560.
                    // Because the uniform EVSYS channels introduced with the EA leave plenty of holes, it's likely that that part would have its channels numbered similarly,
                    // with its extra ports
        porth_evgen0      = 0x4E,
        porth_evgen1      = 0x4F,
#endif
#if defined(PORTJ)  // not clear how the remaining ports would be handled, but that's probably what the 0x5_ range is earmarked for.
        portj_evgen0      = 0x50,
        portj_evgen1      = 0x51,
#endif
#if defined(PORTK)
        portk_evgen0      = 0x52,
        portk_evgen1      = 0x53,
#endif
#if defined(PORTL)
        portl_evgen0      = 0x54,
        portl_evgen1      = 0x55,
#endif
        usart0_xck        = 0x60,
#if defined(USART1)
        usart1_xck        = 0x61,
#endif
#if defined(USART2)
        usart2_xck        = 0x62,
#endif
#if defined(USART3) // I fully expect to see Ex-series parts with more serial ports and higher pincounts.
        usart3_xck        = 0x63,
#endif
#if defined(USART4)
        usart4_xck        = 0x64,
#endif
#if defined(USART5)
        usart5_xck        = 0x65,
#endif
#if defined(USART6)
        usart6_xck        = 0x66,
#endif
#if defined(USART7) // The highest number of USARTs that naturally fit this numbering coincides with the maximum plausible number on a 100 pin chip
        usart7_xck        = 0x67,
#endif
        spi0_sck          = 0x68,
#if defined(SPI1)   // I fully expect to see Ex-series parts with more SPI ports and higher pincounts.
        spi1_sck          = 0x69,
#endif
#if defined(TCA0)
        tca0_ovf_lunf     = 0x80,
        tca0_hunf         = 0x81,
        tca0_cmp0         = 0x84,
        tca0_cmp1         = 0x85,
        tca0_cmp2         = 0x86,
#endif
#if defined(TCE0)
        tce0_ovf          = 0x80,  /* Timer/Counter E0 overflow */
        tce0_cmp0         = 0x84,  /* Timer/Counter E0 compare 0 */
        tce0_cmp1         = 0x85,  /* Timer/Counter E0 compare 1 */
        tce0_cmp2         = 0x86,  /* Timer/Counter E0 compare 2 */
        tce0_cmp3         = 0x87,  /* Timer/Counter E0 compare 3 */
#endif
#if defined(TCA1)  // I fully expect to see Ex-series parts with only 1 TCA, in low pincounts.
        tca1_ovf_lunf     = 0x88,
        tca1_hunf         = 0x89,
        tca1_cmp0         = 0x8C,
        tca1_cmp1         = 0x8D,
        tca1_cmp2         = 0x8E,
#endif  // They could fit a totally new kind of peripheral in 0x9_ - or add a TCA2/TCA3, which is more likely
        tcb0              = 0xA0,
        tcb0_capt         = 0xA0,
        tcb0_ovf          = 0xA1,
        tcb1              = 0xA2,
        tcb1_capt         = 0xA2,
        tcb1_ovf          = 0xA3,
#if defined(TCB2) // I fully expect to see Ex-series parts with only 2 TCBs, in low pincounts.
        tcb2              = 0xA4,
        tcb2_capt         = 0xA4,
        tcb2_ovf          = 0xA5,
#endif
#if defined(TCB3) // I fully expect to see Ex-series parts with 4 or more TCBs, in high pincounts.
        tcb3              = 0xA6,
        tcb3_capt         = 0xA6,
        tcb3_ovf          = 0xA7,
#endif
#if defined(TCB4)
        tcb4              = 0xA8,
        tcb4_capt         = 0xA8,
        tcb4_ovf          = 0xA9,
#endif
#if defined(TCB5)
        tcb5              = 0xAA,
        tcb5_capt         = 0xAA,
        tcb5_ovf          = 0xAB,
#endif
#if defined(TCB6)
        tcb6              = 0xAC,
        tcb6_capt         = 0xAC,
        tcb6_ovf          = 0xAD,
#endif
#if defined(TCB7)
        tcb7              = 0xAE,
        tcb7_capt         = 0xAE,
        tcb7_ovf          = 0xAF,
#endif
#if defined(TCD0) // should we ever be so lucky!
        tcd0_cmpbclr      = 0xB0,
        tcd0_cmpaset      = 0xB1,
        tcd0_cmpbset      = 0xB2,
        tcd0_progev       = 0xB3,
#endif
// Saving room for TCD1?
#if defined(TCF0)
        tcf0_ovf          = 0xB8,  /* Timer/Counter F0 Overflow */
        tcf0_cmp0         = 0xB9,  /* Timer/Counter F0 compare 0 */
        tcf0_cmp1         = 0xBA  /* Timer/Counter F0 compare 1 */
#endif
    };
  };
  /* Done with plausible generators */

  #if defined(__AVR_EA__) //these are specific to the family,
    namespace user{
      enum user_t : uint8_t {
        ccl0_event_a      = 0x00,
        ccl0_event_b      = 0x01,
        ccl1_event_a      = 0x02,
        ccl1_event_b      = 0x03,
        ccl2_event_a      = 0x04,
        ccl2_event_b      = 0x05,
        ccl3_event_a      = 0x06,
        ccl3_event_b      = 0x07,
        adc0_start        = 0x08,
        evouta_pin_pa2    = 0x09,
#if defined(PIN_PB2)
        evoutb_pin_pb2    = 0x0A,
#endif
        evoutc_pin_pc2    = 0x0B,
        evoutd_pin_pd2    = 0x0C,
#if defined(PIN_PE2)
        evoute_pin_pe2    = 0x0D,
#endif
#if defined(PIN_PF2)
        evoutf_pin_pf2    = 0x0E,
#endif
        usart0_irda       = 0x0F,
#if defined(USART2)
        usart1_irda       = 0x10,
#endif
#if defined(USART2)
        usart2_irda       = 0x11,
#endif
#if defined(TCA0)
        tca0              = 0x12,
        tca0_cnt_a        = 0x12,
        tca0_cnt_b        = 0x13,
#endif
#if defined(TCA1)
        tca1              = 0x14,
        tca1_cnt_a        = 0x14,
        tca1_cnt_b        = 0x15,
#endif
#if defined(TCB0)
        tcb0              = 0x16,
        tcb0_capt         = 0x16,
        tcb0_cnt          = 0x17,
#endif
#if defined(TCB1)
        tcb1              = 0x18,
        tcb1_capt         = 0x18,
        tcb1_cnt          = 0x19,
#endif
#if defined(TCB2)
        tcb2              = 0x1A,
        tcb2_capt         = 0x1A,
        tcb2_cnt          = 0x1B,
#endif
#if defined(TCB3)
        tcb3              = 0x1C,
        tcb3_capt         = 0x1C,
        tcb3_cnt          = 0x1D,
#endif
        // "Unofficial" users. Uses EVOUT, but swaps the output pin using PORTMUX
        evouta_pin_pa7    = 0x89,
#if defined(PIN_PB7)
        evoutb_pin_pb7    = 0x8A,
#endif
#if defined(PIN_PC7)
        evoutc_pin_pc7    = 0x8B,
#endif
        evoutd_pin_pd7    = 0x8C,
#if defined(PIN_PE7)
        evoute_pin_pe7    = 0x8D,
#endif
        evoutf_pin_pf7    = 0x8E
      };
    };
  #elif defined(__AVR_EB__)
    namespace user{
      enum user_t : uint8_t {
        ccl0_event_a      = 0x00,
        ccl0_event_b      = 0x01,
        ccl1_event_a      = 0x02,
        ccl1_event_b      = 0x03,
        ccl2_event_a      = 0x04,
        ccl2_event_b      = 0x05,
        ccl3_event_a      = 0x06,
        ccl3_event_b      = 0x07,
        adc0_start        = 0x08,
#if defined(PIN_PA2)
        evouta_pin_pa2    = 0x09,
#endif
#if defined(PIN_PC2)
        evoutc_pin_pc2    = 0x0A,
#endif
#if defined(PIN_PD2)
        evoutd_pin_pd2    = 0x0B,
#endif
#if defined(PIN_PF2)
        evoutf_pin_pf2    = 0x0C,
#endif
        usart0_irda       = 0x0D,
        tcae              = 0x0E,
        tce0_cnt_a        = 0x0F,
        tce0_cnt_b        = 0x10,
#if defined(TCB0)
        tcb0              = 0x11,
        tcb0_capt         = 0x11,
        tcb0_cnt          = 0x12,
#endif
#if defined(TCB1)
        tcb1              = 0x13,
        tcb1_capt         = 0x13,
        tcb1_cnt          = 0x14,
#endif
        tcf0_cnt          = 0x15,  /* TCF0 Clock Event */
        tcf0_act          = 0x16,  /* TCF0 Action Event */
        wexa              = 0x17,  /* WEX Event A */
        wexb              = 0x18,  /* WEX Event B */
        wexc              = 0x19,  /* WEX Event C */
#if defined(PIN_PA7)
        // "Unofficial" users. Uses EVOUT, but swaps the output pin using PORTMUX
        evouta_pin_pa7    = 0x89,
#endif
        evoutd_pin_pd7    = 0x8C,
        evoutf_pin_pf7    = 0x8E
      };
    };
    #endif // defined (__AVR_EB__)
  #elif MEGATINYCORE_SERIES == 2
    #if defined(PIN_PB1)
      #define HAS_SEPARATE_GEN1
      #define HAS_SEPARATE_GEN3
      #define HAS_SEPARATE_GEN4
    #endif
    namespace gen {
      enum generator_t : uint8_t {
        disable           = 0x00,
        updi_synch        = 0x01,
        rtc_ovf           = 0x06,
        rtc_cmp           = 0x07,
        ccl0_out          = 0x10,
        ccl1_out          = 0x11,
        ccl2_out          = 0x12,
        ccl3_out          = 0x13,
        ac0_out           = 0x20,
        adc0_ready        = 0x24,
        adc0_sample       = 0x25,
        adc0_window       = 0x26,
        usart0_xck        = 0x60,
        usart1_xck        = 0x61,
        spi0_sck          = 0x68,
        tca0_ovf_lunf     = 0x80,
        tca0_hunf         = 0x81,
        tca0_cmp0         = 0x84,
        tca0_cmp1         = 0x85,
        tca0_cmp2         = 0x86,
        tcb0              = 0xA0,
        tcb0_capt         = 0xA0,
        tcb0_ovf          = 0xA1,
        tcb1              = 0xA2,
        tcb1_capt         = 0xA2,
        tcb1_ovf          = 0xA3,
      };
    };

    namespace gen0 {
      enum generator_t : uint8_t {
        disable           = 0x00,
        rtc_div8192       = 0x08,
        rtc_div4096       = 0x09,
        rtc_div2048       = 0x0A,
        rtc_div1024       = 0x0B,
        pin_pa0           = 0x40,
        pin_pa1           = 0x41,
        pin_pa2           = 0x42,
        pin_pa3           = 0x43,
        pin_pa4           = 0x44,
        pin_pa5           = 0x45,
        pin_pa6           = 0x46,
        pin_pa7           = 0x47,
#if (PIN_PB0)
          pin_pb0         = 0x48,
          pin_pb1         = 0x49,
          pin_pb2         = 0x4A,
          pin_pb3         = 0x4B,
#endif
#if defined(PIN_PB4)
        pin_pb4           = 0x4C,
        pin_pb5           = 0x4D,
#endif
#if defined(PIN_PB6)
        pin_pb6           = 0x4E,
        pin_pb7           = 0x4F,
#endif
      };
    };

    namespace gen1 {
      enum generator_t : uint8_t {
        disable           = 0x00,
        rtc_div512        = 0x08,
        rtc_div256        = 0x09,
        rtc_div128        = 0x0A,
        rtc_div64         = 0x0B,
        pin_pa0           = 0x40,
        pin_pa1           = 0x41,
        pin_pa2           = 0x42,
        pin_pa3           = 0x43,
        pin_pa4           = 0x44,
        pin_pa5           = 0x45,
        pin_pa6           = 0x46,
        pin_pa7           = 0x47,
#if (PIN_PB0)
          pin_pb0         = 0x48,
          pin_pb1         = 0x49,
          pin_pb2         = 0x4A,
          pin_pb3         = 0x4B,
#endif
#if defined(PIN_PB4)
        pin_pb4           = 0x4C,
        pin_pb5           = 0x4D,
#endif
#if defined(PIN_PB6)
        pin_pb6           = 0x4E,
        pin_pb7           = 0x4F,
#endif
      };
    };

    namespace gen2 {
      enum generator_t : uint8_t {
        disable           = 0x00,
        rtc_div8192       = 0x08,
        rtc_div4096       = 0x09,
        rtc_div2048       = 0x0A,
        rtc_div1024       = 0x0B,
#if defined(PIN_PC0)
        pin_pc0           = 0x40,
        pin_pc1           = 0x41,
        pin_pc2           = 0x42,
        pin_pc3           = 0x43,
#endif
#if defined(PIN_PC4)
        pin_pc4           = 0x44,
        pin_pc5           = 0x45,
#endif
        pin_pa0           = 0x40,
        pin_pa1           = 0x41,
        pin_pa2           = 0x42,
        pin_pa3           = 0x43,
        pin_pa4           = 0x44,
        pin_pa5           = 0x45,
        pin_pa6           = 0x46,
        pin_pa7           = 0x47,
      };
    };

    namespace gen3 {
      enum generator_t : uint8_t {
        disable           = 0x00,
        rtc_div512        = 0x08,
        rtc_div256        = 0x09,
        rtc_div128        = 0x0A,
        rtc_div64         = 0x0B,
#if defined(PIN_PC0)
        pin_pc0           = 0x40,
        pin_pc1           = 0x41,
        pin_pc2           = 0x42,
        pin_pc3           = 0x43,
#endif
#if defined(PIN_PC4)
        pin_pc4           = 0x44,
        pin_pc5           = 0x45,
#endif
        pin_pa0           = 0x40,
        pin_pa1           = 0x41,
        pin_pa2           = 0x42,
        pin_pa3           = 0x43,
        pin_pa4           = 0x44,
        pin_pa5           = 0x45,
        pin_pa6           = 0x46,
        pin_pa7           = 0x47,
      };
    };

    namespace gen4 {
      enum generator_t : uint8_t {
        disable           = 0x00,
        rtc_div8192       = 0x08,
        rtc_div4096       = 0x09,
        rtc_div2048       = 0x0A,
        rtc_div1024       = 0x0B,
#if (PIN_PB0)
          pin_pb0         = 0x40,
          pin_pb1         = 0x41,
          pin_pb2         = 0x42,
          pin_pb3         = 0x43,
#endif
#if defined(PIN_PB4)
        pin_pb4           = 0x44,
        pin_pb5           = 0x45,
#endif
#if defined(PIN_PB6)
        pin_pb6           = 0x46,
        pin_pb7           = 0x47,
#endif
#if defined(PIN_PC0)
        pin_pc0           = 0x40,
        pin_pc1           = 0x41,
        pin_pc2           = 0x42,
        pin_pc3           = 0x43,
#endif
#if defined(PIN_PC4)
        pin_pc4           = 0x44,
        pin_pc5           = 0x45,
#endif
      };
    };

    namespace gen5 {
      enum generator_t : uint8_t {
        disable           = 0x00,
        rtc_div512        = 0x08,
        rtc_div256        = 0x09,
        rtc_div128        = 0x0A,
        rtc_div64         = 0x0B,
#if (PIN_PB0)
          pin_pb0         = 0x40,
          pin_pb1         = 0x41,
          pin_pb2         = 0x42,
          pin_pb3         = 0x43,
#endif

#if defined(PIN_PB4)
        pin_pb4           = 0x44,
        pin_pb5           = 0x45,
#endif
#if defined(PIN_PB6)
        pin_pb6           = 0x46,
        pin_pb7           = 0x47,
#endif
#if defined(PIN_PC0)
        pin_pc0           = 0x40,
        pin_pc1           = 0x41,
        pin_pc2           = 0x42,
        pin_pc3           = 0x43,
#endif
#if defined(PIN_PC4)
        pin_pc4           = 0x44,
        pin_pc5           = 0x45,
#endif
      };
    };


    namespace user {
      enum user_t : uint8_t {
        ccl0_event_a      = 0x00,
        ccl0_event_b      = 0x01,
        ccl1_event_a      = 0x02,
        ccl1_event_b      = 0x03,
        ccl2_event_a      = 0x04,
        ccl2_event_b      = 0x05,
        ccl3_event_a      = 0x06,
        ccl3_event_b      = 0x07,
        adc0_start        = 0x08,
        evouta_pin_pa2    = 0x09,
        evouta_pin_pa7    = 0x89,
        evoutb_pin_pb2    = 0x0A,
#if defined(PIN_PB7)
        evoutb_pin_pb7    = 0x8A,
#endif
#if defined(PIN_PC2)
        evoutc_pin_pc2    = 0x0B,
#endif
        usart0_irda       = 0x0C,
        usart1_irda       = 0x0D,
        tca0              = 0x0E,
        tca0_cnt_a        = 0x0E,
        tca0_cnt_b        = 0x0F,
        tcb0              = 0x11,
        tcb0_capt         = 0x11,
        tcb0_cnt          = 0x12,
        tcb1              = 0x13,
        tcb1_capt         = 0x13,
        tcb1_cnt          = 0x14,
      };
    };

#else  // 0/1-series
      // Synchronous channel 0
    namespace gen0 {
      enum generator_t : uint8_t {
#if defined(PIN_PC0) // 20/24 pin parts
        pin_pc0           = 0x07,
        pin_pc1           = 0x08,
        pin_pc2           = 0x09,
        pin_pc3           = 0x0A,
#endif
#if defined(PIN_PC4) // 24-pin parts only
        pin_pc4           = 0x0B,
        pin_pc5           = 0x0C,
#endif
        pin_pa0           = 0x0D,
        pin_pa1           = 0x0E,
        pin_pa2           = 0x0F,
        pin_pa3           = 0x10,
        pin_pa4           = 0x11,
        pin_pa5           = 0x12,
        pin_pa6           = 0x13,
        pin_pa7           = 0x14,
#if defined(TINY_1_16K_PLUS)
        tcb1              = 0x15,
        tcb1_capt         = 0x15,
#endif
      };
    };
    namespace gen2 {
      enum generator_t : uint8_t {
        pin_pa0           = 0x0A,
        pin_pa1           = 0x0B,
        pin_pa2           = 0x0C,
        pin_pa3           = 0x0D,
        pin_pa4           = 0x0E,
        pin_pa5           = 0x0F,
        pin_pa6           = 0x10,
        pin_pa7           = 0x11,
        updi              = 0x12,
#if defined(TINY_1_16K_PLUS)
        ac1_out           = 0x13,
        ac2_out           = 0x14,
#endif
      };
    };

#if defined(PIN_PB0)
      /* all parts withunique options here a PORTB (ie, they're not 8-pin ones) */
      #define HAS_SEPARATE_GEN3
      namespace gen3 {
        enum generator_t : uint8_t {
#if (PIN_PB0)
          pin_pb0         = 0x0A,
          pin_pb1         = 0x0B,
          pin_pb2         = 0x0C,
          pin_pb3         = 0x0D,
#endif
#if (PIN_PB4)
          pin_pb4         = 0x0E,
          pin_pb5         = 0x0F,
#endif
#if defined(PIN_PB6)
          pin_pb6         = 0x10,
          pin_pb7         = 0x11,
#endif
#if defined(TINY_1_16K_PLUS)
          ac1_out         = 0x12,
          ac2_out         = 0x13,
#endif
        };
      };
#endif

#if MEGATINYCORE_SERIES == 1
      /* Only 1-series parts have third and fourth async sync channel.
       * and only parts with 20/24 pins, or the 1614 have any items on this list available
       * since it's all PORTC or the second and third AC.
       */
  #if defined(TINY_1_16K_PLUS) || defined(PIN_PC0)
      #define HAS_SEPARATE_GEN4
      namespace gen4 {
        enum generator_t : uint8_t {
    #if defined(PIN_PC0)
          pin_pc0        = 0x0A,
          pin_pc1        = 0x0B,
          pin_pc2        = 0x0C,
          pin_pc3        = 0x0D,
    #endif
    #if defined(PIN_PC4)
          pin_pc4        = 0x0E,
          pin_pc5        = 0x0F,
    #endif
    #if defined(TINY_1_16K_PLUS)
          ac1_out        = 0x10,
          ac2_out        = 0x11,
    #endif
        };
      };
  #endif
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
#if defined(TINY_1_16K_PLUS)
        ac1_out           = 0x12,
        ac2_out           = 0x13,
#endif
      };
    };

#if defined(PIN_PB0)
    #define HAS_SEPARATE_GEN1
      namespace gen1 {
        enum generator_t : uint8_t {
#if defined(PIN_PB0)
        pin_pb0          = 0x08,
        pin_pb1          = 0x09,
        pin_pb2          = 0x0A,
        pin_pb3          = 0x0B,
#endif
#if defined(PIN_PB4)
        pin_pb4          = 0x0C,
        pin_pb5          = 0x0D,
#endif
#if defined(PIN_PB7)
        pin_pb6          = 0x0E,
        pin_pb7          = 0x0F,
#endif
#if defined(TINY_1_16K_PLUS)
        tcb1             = 0x10,
        tcb1_capt        = 0x10,
#endif
      };
    };
#endif
#endif // end of  1-series only part.

    namespace gens {
      enum generator_t : uint8_t {
        disable          = 0x00,
        off              = 0x00,
        tcb0             = 0x01,
        tcb0_capt        = 0x01,
        tca0_ovf_lunf    = 0x02,
        tca0_hunf        = 0x03,
        tca0_cmp0        = 0x04,
        tca0_cmp1        = 0x05,
        tca0_cmp2        = 0x06,
      };
    };

    namespace gen {
      enum generator_t : uint8_t {
        disable          = 0x00,
        off              = 0x00,
        ccl_lut0         = 0x01,
        ccl0_out         = 0x01,
        ccl_lut1         = 0x02,
        ccl1_out         = 0x02,
        ac0_out          = 0x03,
#if MEGATINYCORE_SERIES == 1
        tcd0_cmpbclr     = 0x04,
        tcd0_cmpaset     = 0x05,
        tcd0_cmpbset     = 0x06,
        tcd0_progev      = 0x07,
#endif
        rtc_ovf          = 0x08,
        rtc_cmp          = 0x09,
      };
    };



    namespace user {
      enum user_t : uint8_t {
        tcb0             = 0x00,
        tcb0_capt        = 0x00,
        adc0_start       = 0x01,
        ccl0_event_a     = 0x02,
        ccl1_event_a     = 0x03,
        ccl0_event_b     = 0x04,
        ccl1_event_b     = 0x05,
#if MEGATINYCORE_SERIES == 1
        tcd0_in_a        = 0x06,
        tcd0_in_b        = 0x07,
#endif
        evouta_pin_pa2   = 0x08,
        evoutb_pin_pb2   = 0x09,
        evoutc_pin_pc2   = 0x0A,
#if defined(TINY_1_16K_PLUS)
        tcb1             = 0x0B,
        tcb1_capt        = 0x0B,
        adc1_start       = 0x0c,
#endif
        tca0             = 0x10,
        tca0_cnt_a       = 0x10,
        usart0_irda      = 0x11,
      };
    };
#endif
};



// Legacy definitions
namespace gen          {
  using namespace event::gen;
};
#if !defined(PORT_EVGEN0SEL_gm)
  #if defined(EVSYS_CHANNEL0)
  namespace gen0          {
    using namespace event::gen0;
  };
  #endif
  #if defined(EVSYS_CHANNEL1) && (!defined(MEGATINYCORE) || defined(HAS_SEPARATE_GEN1)) /* otherwise there's no gen1 namespace because there would be nothing in it */
  namespace gen1          {
    using namespace event::gen1;
  };
  #endif
  #if defined(EVSYS_CHANNEL2)
  namespace gen2          {
    using namespace event::gen2;
  };
  #endif
  #if defined(EVSYS_CHANNEL3) && (!defined(MEGATINYCORE) || defined(HAS_SEPARATE_GEN3)) /* otherwise there's no gen3 namespace because there would be nothing in it */
    namespace gen3          {
    using namespace event::gen3;
  };
  #endif
  #if defined(EVSYS_CHANNEL4) && (!defined(MEGATINYCORE) || defined(HAS_SEPARATE_GEN4)) /* otherwise there's no gen4 namespace because there would be nothing in it */
  namespace gen4          {
    using namespace event::gen4;
  };
  #endif
  #if defined(EVSYS_CHANNEL5)
  namespace gen5          {
    using namespace event::gen5;
  };
  #endif
  #if defined(EVSYS_CHANNEL6)
  namespace gen6          {
    using namespace event::gen6;
  };
  #endif
  #if defined(EVSYS_CHANNEL7)
  namespace gen7          {
    using namespace event::gen7;
  };
  #endif
  #if defined(EVSYS_CHANNEL8)
  namespace gen8          {
    using namespace event::gen8;
  };
  #endif
  #if defined(EVSYS_CHANNEL9)
  namespace gen9          {
    using namespace event::gen9;
  };
  #endif
  /* We can dream */
  #if defined(EVSYS_CHANNEL10)
  namespace gen10          {
    using namespace event::gen10;
  };
  #endif
  #if defined(EVSYS_CHANNEL11)
  namespace gen11          {
    using namespace event::gen11;
  };
  #endif
  #if defined(EVSYS_CHANNEL12)
  namespace gen12          {
    using namespace event::gen12;
  };
  #endif
  #if defined(EVSYS_CHANNEL13)
  namespace gen13          {
    using namespace event::gen13;
  };
  #endif
  #if defined(EVSYS_CHANNEL14)
  namespace gen14          {
    using namespace event::gen14;
  };
  #endif
  #if defined(EVSYS_CHANNEL15)
  namespace gen15          {
    using namespace event::gen15;
  };
  #endif
#else // on parts where all of the channels are identical, these are provided just to ease porting.
  #define gen0 gen
  #define gen1 gen
  #define gen2 gen
  #define gen3 gen
  #define gen4 gen
  #define gen5 gen
  #define gen6 gen
  #define gen7 gen
  #define gen8 gen
  #define gen9 gen
  #define gen10 gen
  #define gen11 gen
  #define gen12 gen
  #define gen13 gen
  #define gen14 gen
  #define gen15 gen
#endif
namespace user          {
  using namespace event::user;
};
#endif //EVENT_PARTS_H
