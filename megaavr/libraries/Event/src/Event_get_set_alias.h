/* This file is ONLY included by Event.h and should never be included by any other code under any circumstances.
 * It is loaded in the middle of the class definition, and provides aliases that let you use get_generator_channel
 * and set_generator with generators not present on all channels.
 * This file is not loaded if all channels on the part have the same generators (like on the upcoming EA-series)
 */
#if !defined(EVENT_H)
  #error "This should only be included as part of Event.h"
#endif

#ifndef EVENT_GET_SET_ALIAS_H
#define EVENT_GET_SET_ALIAS_H

#if defined(EVSYS_CHANNEL0)
  void get_generator_channel(event::gen0::generator_t generator) { get_generator_channel((event::gen::generator_t)generator); }
  void set_generator(event::gen0::generator_t generator) { set_generator((event::gen::generator_t)generator); }
#endif
#if defined(EVSYS_CHANNEL1)
  void get_generator_channel(event::gen1::generator_t generator) { get_generator_channel((event::gen::generator_t)generator); }
  void set_generator(event::gen1::generator_t generator) { set_generator((event::gen::generator_t)generator); }
#endif
#if defined(EVSYS_CHANNEL2)
  void get_generator_channel(event::gen2::generator_t generator) { get_generator_channel((event::gen::generator_t)generator); }
  void set_generator(event::gen2::generator_t generator) { set_generator((event::gen::generator_t)generator); }
#endif
#if defined(EVSYS_CHANNEL3)
  void get_generator_channel(event::gen3::generator_t generator) { get_generator_channel((event::gen::generator_t)generator); }
  void set_generator(event::gen3::generator_t generator) { set_generator((event::gen::generator_t)generator); }
#endif
#if defined(EVSYS_CHANNEL4)
  void get_generator_channel(event::gen4::generator_t generator) { get_generator_channel((event::gen::generator_t)generator); }
  void set_generator(event::gen4::generator_t generator) { set_generator((event::gen::generator_t)generator); }
#endif
#if defined(EVSYS_CHANNEL5)
  void get_generator_channel(event::gen5::generator_t generator) { get_generator_channel((event::gen::generator_t)generator); }
  void set_generator(event::gen5::generator_t generator) { set_generator((event::gen::generator_t)generator); }
#endif
#if defined(EVSYS_CHANNEL6)
  void get_generator_channel(event::gen6::generator_t generator) { get_generator_channel((event::gen::generator_t)generator); }
  void set_generator(event::gen6::generator_t generator) { set_generator((event::gen::generator_t)generator); }
#endif
#if defined(EVSYS_CHANNEL7)
  void get_generator_channel(event::gen7::generator_t generator) { get_generator_channel((event::gen::generator_t)generator); }
  void set_generator(event::gen7::generator_t generator) { set_generator((event::gen::generator_t)generator); }
#endif
#if defined(EVSYS_CHANNEL8)
  void get_generator_channel(event::gen8::generator_t generator) { get_generator_channel((event::gen::generator_t)generator); }
  void set_generator(event::gen8::generator_t generator) { set_generator((event::gen::generator_t)generator); }
#endif
#if defined(EVSYS_CHANNEL9)
  void get_generator_channel(event::gen9::generator_t generator) { get_generator_channel((event::gen::generator_t)generator); }
  void set_generator(event::gen9::generator_t generator) { set_generator((event::gen::generator_t)generator); }
#endif
// In the unlikely event that there's a part with non-identical generators and 11-16 channels
#if defined(EVSYS_CHANNEL10)
  void get_generator_channel(event::gen10::generator_t generator) { get_generator_channel((event::gen::generator_t)generator); }
  void set_generator(event::gen10::generator_t generator) { set_generator((event::gen::generator_t)generator); }
#endif
#if defined(EVSYS_CHANNEL11)
  void get_generator_channel(event::gen11::generator_t generator) { get_generator_channel((event::gen::generator_t)generator); }
  void set_generator(event::gen11::generator_t generator) { set_generator((event::gen::generator_t)generator); }
#endif
#if defined(EVSYS_CHANNEL12)
  void get_generator_channel(event::gen12::generator_t generator) { get_generator_channel((event::gen::generator_t)generator); }
  void set_generator(event::gen12::generator_t generator) { set_generator((event::gen::generator_t)generator); }
#endif
#if defined(EVSYS_CHANNEL13)
  void get_generator_channel(event::gen13::generator_t generator) { get_generator_channel((event::gen::generator_t)generator); }
  void set_generator(event::gen13::generator_t generator) { set_generator((event::gen::generator_t)generator); }
#endif
#if defined(EVSYS_CHANNEL14)
  void get_generator_channel(event::gen14::generator_t generator) { get_generator_channel((event::gen::generator_t)generator); }
  void set_generator(event::gen14::generator_t generator) { set_generator((event::gen::generator_t)generator); }
#endif
#if defined(EVSYS_CHANNEL15)
  void get_generator_channel(event::gen15::generator_t generator) { get_generator_channel((event::gen::generator_t)generator); }
  void set_generator(event::gen15::generator_t generator) { set_generator((event::gen::generator_t)generator); }
#endif
#endif //EVENT_PARTS_H
