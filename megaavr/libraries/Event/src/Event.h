#ifndef EVENT_H
#define EVENT_H

#include <Arduino.h>
#include "Event_tinyAVR.h"
// This makes sure that tinyAVRs are made to look more like normal parts so more code can be shared.
#include "Event_parts.h"
/* All of the extremely ugly and verbose stuff with information about the peripherals for each part family */

/* All of the verbose and repetitive files were moved into separate headers to make it easier to maintain
 * and make *this* file less of a pain in the eye to read
 *
 * Status:
 * tinyAVR 0/1 - believed working
 * tinyAVR 2 - working
 * DA, DB - working
 * DD - believed working
 * EA - needs new (but fairly easy to write) implementations of :
 * assign_* - to handle the fact that all channels are the same (HURAAAH!) and that pins and rtc generators are a bit more work to set up.
 * set_generator(uint8_t pin_number) - to handle to more complicated setup of pin generators.
 *
 */

class Event {
  public:
    Event(uint8_t channel_num, volatile uint8_t &channel_addr);
    uint8_t get_channel_number();
    static Event& get_channel(uint8_t channel_number);
    static Event& get_generator_channel(event::gen::generator_t generator);
    static Event& get_generator_channel(uint8_t generator_pin);
    uint8_t get_generator();
    void set_generator(event::gen::generator_t generator);
    static Event& assign_generator(event::gen::generator_t event_generator);
    void set_generator(uint8_t pin_number);
    static Event& assign_generator_pin(uint8_t port, uint8_t port_pin);
    static Event& assign_generator_pin(uint8_t pin_number);

    #if defined(TINY_0_OR_1_SERIES)
      void get_generator_channel(event::gens::generator_t generator) {
        get_generator_channel((event::gen::generator_t)generator);
      }
      void set_generator(event::gens::generator_t generator) {
        set_generator((event::gen::generator_t)generator);
      }
    #endif
    // *INDENT-OFF*
    #if !defined(PORT_EVGEN0SEL_gm)
      /* Where this is defined, **ALL CHANNELS ARE THE SAME** (EA-series, and likely everything after it)
       * That means that there are no genN generators!
       * though aliases of those names are #defined for those parts, no further glue is needed.
       */
      #include "Event_get_set_alias.h" // otherwise we need some unsightly glue to make get and set generator work
    #endif
    // *INDENT-ON*
    static int8_t get_user_channel_number(event::user::user_t event_user);
    static Event& get_user_channel(event::user::user_t event_user);
    void set_user(event::user::user_t event_user);
    int8_t set_user_pin(uint8_t pin_number);
    static void clear_user(event::user::user_t event_user);
    void soft_event();
    void long_soft_event(uint8_t length);
    void start(bool state = true);
    void stop();
    /* event_types: They start from 0x00 for inputs. Outputs start at 0x40 */
    static event::gen::generator_t gen_from_peripheral(TCB_t& timer, uint8_t event_type  = 0);
    static event::user::user_t    user_from_peripheral(TCB_t& timer, uint8_t user_type   = 0);
    static event::user::user_t    user_from_peripheral(USART_t& usart                       );
    static event::gen::generator_t gen_from_peripheral(TCA_t& timer, uint8_t event_type  = 0);
    static event::user::user_t    user_from_peripheral(TCA_t& timer, uint8_t user_type   = 0);
    static event::gen::generator_t gen_from_peripheral(CCL_t& logic, uint8_t event_type  = 0);
    static event::user::user_t    user_from_peripheral(CCL_t& logic, uint8_t user_type   = 0);
    static event::gen::generator_t gen_from_peripheral(AC_t&  comp                          );

  private:
    const uint8_t channel_number;      // Holds the event generator channel number
    volatile uint8_t &channel_address; // Reference to the event channel address
    uint8_t generator_type;            // Generator type the event channel is using
    static void _long_soft_event(uint8_t channel, uint8_t length);    // holds the bulky assembly routine for the long softevent.
};
/* Finally, based on thenumber of channels, we reference the external objects. */
#if defined(EVSYS_CHANNEL0)
  extern Event Event0;
#endif
#if defined(EVSYS_CHANNEL1)
  extern Event Event1;
#endif
#if defined(EVSYS_CHANNEL2)
  extern Event Event2;
#endif
#if defined(EVSYS_CHANNEL3)
  extern Event Event3;
#endif
#if defined(EVSYS_CHANNEL4)
  extern Event Event4;
#endif
#if defined(EVSYS_CHANNEL5)
  extern Event Event5;
#endif
#if defined(EVSYS_CHANNEL6)
  extern Event Event6;
#endif
#if defined(EVSYS_CHANNEL7)
  extern Event Event7;
#endif
#if defined(EVSYS_CHANNEL8)
  extern Event Event8;
#endif
#if defined(EVSYS_CHANNEL9)
  extern Event Event9;
#endif
/* Future compatibility, should we be so lucky to get an event system extended to 16 channels.*/
#if defined(EVSYS_CHANNEL10)
  extern Event Event10;
#endif
#if defined(EVSYS_CHANNEL11)
  extern Event Event11;
#endif
#if defined(EVSYS_CHANNEL12)
  extern Event Event12;
#endif
#if defined(EVSYS_CHANNEL13)
  extern Event Event13;
#endif
#if defined(EVSYS_CHANNEL14)
  extern Event Event14;
#endif
#if defined(EVSYS_CHANNEL15)
  extern Event Event15;
#endif
#endif // EVENT_H
