#ifndef LOGIC_h
#define LOGIC_h

#include <Arduino.h>

//Use in:: when working with logic inputs
namespace in
{
  enum input_t : uint8_t
  {
    masked       = 0x00,
    unused       = 0x00,
    disable      = 0x00,
    feedback     = 0x01,
    link         = 0x02,
    event_a      = 0x03,
    event_b      = 0x04,
    input        = 0x05,
    ac           = 0x06,
    input_pullup = 0x07,
    usart        = 0x08,
    spi          = 0x09,
    tca0         = 0x0A,
    tcb          = 0x0C,
  };
};

// Use out:: when working with logic outputs
namespace out
{
  enum output_t : uint8_t
  {
    disable  = 0x00,
    enable   = 0x01,
  };
  enum pinswap_t : uint8_t
  {
    no_swap  = 0x00,
    pin_swap = 0x01,
  };
}; 

// Use filter:: when working with logic output filter
namespace filter
{
  enum filter_t : uint8_t
  {
    disable      = 0x00,
    synchronizer = 0x01,
    filter       = 0x02,
  };
};

// Use sequencer:: when working with LUT sequencer
namespace sequencer
{
  enum sequencer_t : uint8_t
  {
    disable      = 0x00,
    d_flip_flop  = 0x01,
    jk_flip_flop = 0x02,
    d_latch      = 0x03,
    sr_latch     = 0x04,
    rs_latch     = 0x04,
  };
};

class Logic
{
  public:
    static void start(bool state = true);
    static void stop();

    Logic(const uint8_t block_number);
    void init();
#if defined(CCL_CCL_vect)
    void attachInterrupt(voidFuncPtr callback, PinStatus mode);
    void detachInterrupt();
#endif

    bool enable;
    in::input_t input0;
    in::input_t input1;
    in::input_t input2;
    out::output_t output;
    out::pinswap_t output_swap;
    filter::filter_t filter;
    uint8_t truth;
    sequencer::sequencer_t sequencer;

    struct CCLBlock;

  private:
    const struct CCLBlock& block;

    void initInput(in::input_t& input, PORT_t& port, const uint8_t pin_bm);
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

#endif
