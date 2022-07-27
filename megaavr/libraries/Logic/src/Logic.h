#ifndef LOGIC_H
#define LOGIC_H

#include <Arduino.h>

#include "LogicEnums.h"
/* The above handle differences between the hardware and are extremely
 * verbose. This made this file very difficult to use.
 */

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

#include "LogicParts.h"
// Array for storing ISR function pointers
#if defined(CCL_CCL_vect)
  #if defined(TRUTH5)
    static volatile voidFuncPtr intFuncCCL[6];
  #else
    static volatile voidFuncPtr intFuncCCL[4];
  #endif
#endif


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
