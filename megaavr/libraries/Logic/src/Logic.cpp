#include "Logic.h"


#if defined(CCL_TRUTH0)
  Logic Logic0(0);
#endif
#if defined(CCL_TRUTH1)
  Logic Logic1(1);
#endif
#if defined(CCL_TRUTH2)
  Logic Logic2(2);
#endif
#if defined(CCL_TRUTH3)
  Logic Logic3(3);
#endif
#if defined(CCL_TRUTH4)
  Logic Logic4(4);
#endif
#if defined(CCL_TRUTH5)
  Logic Logic5(5);
#endif

Logic::Logic(const uint8_t block_number)
  : enable(false),
    input0(logic::in::masked),
    input1(logic::in::masked),
    input2(logic::in::masked),
    output(logic::out::disable),
    output_swap(logic::out::no_swap),
    filter(logic::filter::disable),
    edgedetect(logic::edgedetect::disable),
    truth(0x00),
    sequencer(logic::sequencer::disable),
    clocksource(logic::clocksource::clk_per),
    block(blocks[block_number]) {
}

// static
void Logic::start(bool state) {
  CCL.CTRLA = (state ? CCL_ENABLE_bm : 0);
}


// static
void Logic::stop() {
  start(false);
}

static volatile register8_t &PINCTRL(PORT_t &port, const uint8_t pin_bm) {
  if (pin_bm == PIN0_bm) {
    return port.PIN0CTRL;
  }
  if (pin_bm == PIN1_bm) {
    return port.PIN1CTRL;
  }
  if (pin_bm == PIN2_bm) {
    return port.PIN2CTRL;
  }
  if (pin_bm == PIN3_bm) {
    return port.PIN3CTRL;
  }
  #ifdef MEGATINYCORE
  if (pin_bm == PIN4_bm) {
    return port.PIN4CTRL;
  }
  if (pin_bm == PIN5_bm) {
    return port.PIN5CTRL;
  }
  #endif
  if (pin_bm == PIN6_bm) {
    return port.PIN6CTRL;
  }
  return port.PIN7CTRL;
}

void Logic::initInput(logic::in::input_t &input, PORT_t &port, const uint8_t pin_bm) {
  if ((input & 0x30) && pin_bm) { // Input pin is either set to input or input with pullup
    port.DIRCLR = pin_bm;

    if (input == logic::in::input) {
      PINCTRL(port, pin_bm) &= ~PORT_PULLUPEN_bm;
    } else {
      PINCTRL(port, pin_bm) |= PORT_PULLUPEN_bm;
    }
  }
}

void Logic::init() {
  // Clear LUTCTRLA in case the CCL block gets reinitialized
  block.LUTCTRLA = 0x00;

  // Configure input pins
  initInput(input0, block.PORT_IN, block.input0_bm);
  initInput(input1, block.PORT_IN, block.input1_bm);
  initInput(input2, block.PORT_IN, block.input2_bm);

  // Set output pin state and output pin swap
  if (output == logic::out::enable) {
    if (output_swap == logic::out::pin_swap && block.output_alt_bm) {
      #if defined(PORTMUX_CCL)
      PORTMUX_CCL |= PORTMUX_ALTOUT_bm;
      #endif
      block.PORT_OUT.DIRSET = block.output_alt_bm;
    } else if (output_swap == logic::out::no_swap && block.output_bm) {
      #if defined(PORTMUX_CCL)
      PORTMUX_CCL &= ~PORTMUX_ALTOUT_bm;
      #endif
      block.PORT_ALT_OUT.DIRSET = block.output_bm;
    }
  }

  // Set inputs modes
  block.LUTCTRLB = ((input1 & 0x0f) << CCL_INSEL1_gp) | ((input0 & 0x0f) << CCL_INSEL0_gp);
  block.LUTCTRLC = ((input2 & 0x0f) << CCL_INSEL2_gp);

  // Set truth table
  block.TRUTH = truth;

  // Set sequencer
  if (!(block.number & 0x01)) {
    block.SEQCTRL = sequencer;
  }

  // Set logic output state and output filter
  // Register layout everything except modern tinyAVR 0/1-series.
  // LUTCTRLA: EDGEDET | OUTEN  | FILTSEL [1:0] | CLKSRC [2:0]  | Enable, e O F_F C_C_C E
  // 0/1-series tinyAVR:
  // LUTCTRLA: EDGEDET | CLKSRC | FILTSEL [1:0] | OUTEN | - | - | Enable, e C F_F O _ _ E

  block.LUTCTRLA = (output       ?  CCL_OUTEN_bm      : 0)
                 | (edgedetect   ?  CCL_EDGEDET_EN_gc : 0)
                 | (filter      <<  CCL_FILTSEL_gp       ) /* << 4 is emitted as swap, andi 0xF0, not 4x lsl */
    #ifdef CCL_CLKSRC_gp
                 | (clocksource <<  CCL_CLKSRC_gp        ) /* group position in this case is 1 */
    #else
                 | (clocksource  ?  CCL_CLKSRC_bp : 0    ) /* w/out the group defined, that means there were only two options, and that that's the only bit that should be impacted by the setting */
    #endif
                 | (enable       ?  CCL_ENABLE_bm : 0    );
}

/* Generally speaking, in code that is executed often or which isspeed sensitive, you prefer to avoid shift operations where one or both of the operands is not compile time known.
