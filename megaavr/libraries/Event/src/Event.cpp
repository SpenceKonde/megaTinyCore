//*INDENT-OFF* formatting checker doesn't like all the indentation...
#include "Event.h"

// Pre-defined objects
#if defined(EVSYS_CHANNEL0)
  Event Event0(0, EVSYS_CHANNEL0);
  Event Event_empty(255, EVSYS_CHANNEL0);
#endif
#if defined(EVSYS_CHANNEL1)
  Event Event1(1, EVSYS_CHANNEL1);
#endif
#if defined(EVSYS_CHANNEL2)
  Event Event2(2, EVSYS_CHANNEL2);
#endif
#if defined(EVSYS_CHANNEL3)
  Event Event3(3, EVSYS_CHANNEL3);
#endif
#if defined(EVSYS_CHANNEL4)
  Event Event4(4, EVSYS_CHANNEL4);
#endif
#if defined(EVSYS_CHANNEL5)
  Event Event5(5, EVSYS_CHANNEL5);
#endif
#if defined(EVSYS_CHANNEL6)
  Event Event6(6, EVSYS_CHANNEL6);
#endif
#if defined(EVSYS_CHANNEL7)
  Event Event7(7, EVSYS_CHANNEL7);
#endif
#if defined(EVSYS_CHANNEL8)
  Event Event8(8, EVSYS_CHANNEL8);
#endif
#if defined(EVSYS_CHANNEL9)
  Event Event9(9, EVSYS_CHANNEL9);
#endif


/**
 * @brief Construct a new Event object
 *
 * @param channel_num Event channel number
 * @param channel_addr Register address to channel generator
 */
Event::Event(uint8_t channel_num, volatile uint8_t &channel_addr)
  : channel_number(channel_num), channel_address(channel_addr) {
}


/**
 * @brief Returns the event channel number in use
 *
 * @return uint8_t Event channel number
 */
uint8_t Event::get_channel_number() {
  return channel_number;
}


/**
 * @brief Returns the object associated with the passed channel number
 *
 * @param ch_number Channel number
 * @return Event& Event object associated with the channel number.
 *         Returns the Event_empty object if invalid channel number
 */
Event& Event::get_channel(uint8_t ch_number) {
  #if defined(EVSYS_CHANNEL0)
    if (Event0.channel_number == ch_number) {
      return Event0;
    }
  #endif
  #if defined(EVSYS_CHANNEL1)
    if (Event1.channel_number == ch_number) {
      return Event1;
    }
  #endif
  #if defined(EVSYS_CHANNEL2)
    if (Event2.channel_number == ch_number) {
      return Event2;
    }
  #endif
  #if defined(EVSYS_CHANNEL3)
    if (Event3.channel_number == ch_number) {
      return Event3;
    }
  #endif
  #if defined(EVSYS_CHANNEL4)
    if (Event4.channel_number == ch_number) {
      return Event4;
    }
  #endif
  #if defined(EVSYS_CHANNEL5)
    if (Event5.channel_number == ch_number) {
      return Event5;
    }
  #endif
  #if defined(EVSYS_CHANNEL6)
    if (Event6.channel_number == ch_number) {
      return Event6;
    }
  #endif
  #if defined(EVSYS_CHANNEL7)
    if (Event7.channel_number == ch_number) {
      return Event7;
    }
  #endif
  #if defined(EVSYS_CHANNEL8)
    if (Event8.channel_number == ch_number) {
      return Event8;
    }
  #endif
  #if defined(EVSYS_CHANNEL9)
    if (Event9.channel_number == ch_number) {
      return Event9;
    }
  #endif
  else{
    return Event_empty;
  }
}

/**
 * @brief Returns the event channel object used for a particular event generator
 *
 * @param generator Event generator
 * @return Event& Event channel object used with this event generator
 */
Event& Event::get_generator_channel(gen::generator_t generator)
{
  #if defined(EVSYS_CHANNEL0)
    if (Event0.generator_type == generator) {
      return Event0;
    }
  #endif
  #if defined(EVSYS_CHANNEL1)
    if (Event1.generator_type == generator) {
      return Event1;
    }
  #endif
  #if defined(EVSYS_CHANNEL2)
    if (Event2.generator_type == generator) {
      return Event2;
    }
  #endif
  #if defined(EVSYS_CHANNEL3)
    if (Event3.generator_type == generator) {
      return Event3;
    }
  #endif
  #if defined(EVSYS_CHANNEL4)
    if (Event4.generator_type == generator) {
      return Event4;
    }
  #endif
  #if defined(EVSYS_CHANNEL5)
    if (Event5.generator_type == generator) {
      return Event5;
    }
  #endif
  #if defined(EVSYS_CHANNEL6)
    if (Event6.generator_type == generator) {
      return Event6;
    }
  #endif
  #if defined(EVSYS_CHANNEL7)
    if (Event7.generator_type == generator) {
      return Event7;
    }
  #endif
  #if defined(EVSYS_CHANNEL8)
    if (Event8.generator_type == generator) {
      return Event8;
    }
  #endif
  #if defined(EVSYS_CHANNEL9)
    if (Event9.generator_type == generator) {
      return Event9;
    }
  #endif
  #if defined(EVSYS_CHANNEL0)
    else
      return Event_empty;
  #endif
}


/**
 * @brief Returns the event channel object used for a particular event generator Arduino pin
 *
 * @param generator_pin Event generator Arduino pin number
 * @return Event& Event channel object used with this event generator
 */
Event& Event::get_generator_channel(uint8_t generator_pin)
{
  uint8_t port = digitalPinToPort(generator_pin);
  uint8_t port_pin = digitalPinToBitPosition(generator_pin);
  uint8_t gen = 0xFF;
  if (port != NOT_A_PIN && port_pin != NOT_A_PIN) {
    #if defined(PORTA_EVGENCTRL)
      volatile PORT_t* port_ptr = portToPortStruct(port);
      uint8_t temp = port_ptr->EVGENCTRL;
        if ((temp & 0x0F) == port_pin) {
          gen = 0x40 + (port << 1);
          _SWAP(port_pin);
        } else if ((temp & 0xF0) == port_pin) {
          gen = 0x40 + (port << 1) + 1;
        }
        if (gen == 0xFF)
          return Event_empty;
        else {
          #if defined(EVSYS_CHANNEL0)
            if (Event0.generator_type == gen)
              return Event0;
          #endif
          #if defined(EVSYS_CHANNEL1)
            if (Event1.generator_type == gen)
              return Event1;
          #endif
          #if defined(EVSYS_CHANNEL2)
            if (Event2.generator_type == gen)
              return Event2;
          #endif
          #if defined(EVSYS_CHANNEL3)
            if (Event3.generator_type == gen)
              return Event3;
          #endif
          #if defined(EVSYS_CHANNEL4)
            if (Event4.generator_type == gen)
              return Event4;
          #endif
          #if defined(EVSYS_CHANNEL5)
            if (Event5.generator_type == gen)
              return Event5;
          #endif
        }
    #elif !defined(MEGATINYCORE)
      gen = 0x40 | ((port & 0x01) << 3) | port_pin;
      if (port == PA || port == PB) {
        #if defined(EVSYS_CHANNEL0)
          if (Event0.generator_type == gen)
            return Event0;
        #endif
        #if defined(EVSYS_CHANNEL1)
         else if (Event1.generator_type == gen)
          return Event1;
        #endif
      } else if (port == PC || port == PD) {
        #if defined(EVSYS_CHANNEL2)
          if (Event2.generator_type == gen)
            return Event2;
        #endif
        #if defined(EVSYS_CHANNEL3)
          else if (Event3.generator_type == gen)
            return Event3;
        #endif
      } else if (port == PE || port == PF) {
        #if defined(EVSYS_CHANNEL4)
          if (Event4.generator_type == gen)
            return Event4;
        #endif
        #if defined(EVSYS_CHANNEL5)
          else if (Event5.generator_type == gen)
            return Event5;
        #endif
      }
      #if defined(PORTG)
        else if (port == PG) {
          if (Event6.generator_type == gen)
            return Event6;
          else if (Event7.generator_type == gen)
            return Event7;
        }
      #endif
    #elif MEGATINYCORE_SERIES == 2
      if (port != PC) {
        gen = port_pin | (port == PA ? 0x40 : 0x48);
        if (Event0.generator_type == gen) {
          return Event0;
        } else if (Event1.generator_type == gen) {
          return Event1;
        }
      }
      if (port != PB) {
        gen = port_pin | (port == PC ? 0x40 : 0x48);
        if (Event2.generator_type == gen) {
          return Event2;
        } else if (Event3.generator_type == gen) {
          return Event3;
        }
      }
      if (port != PA) {
        gen = port_pin | (port == PB ? 0x40 : 0x48);
        if (Event4.generator_type == gen) {
          return Event4;
        } else if (Event5.generator_type == gen) {
          return Event5;
        }
      }
    #else // Oh no, it's a 0/1-series!
      // Prefer to use the more numerous async channels.
      gen = port_pin + 0x0A;
      if (port == PA) {
        if (Event2.generator_type == gen) {
          return Event2;
        }
        gen += 3;
        if (Event0.generator_type == gen) {
          return Event0;
        }
      }
      else if (port == PB) {
        #if MEGATINYCORE_SERIES == 1 // No Event1 on 0-series
        if (Event3.generator_type == gen) {
          return Event3;
        }
        gen -= 2;
        if (Event1.generator_type == gen) {
          return Event1;
        }
        #endif
      }
      #if defined(PIN_PC0) // can't test if PORTx is defined - all are defined everywhere)
        if (port == PC) {
          #if MEGATINYCORE_SERIES == 1 // no event 4 on 0-series
            if (Event4.generator_type == gen) {
              return Event4;
            }
          #endif
          gen -= 3;
          if (Event0.generator_type == gen) {
            return Event0;
          }
        }
      #endif // PC-bearing parts end here
    #endif // end of tiny 0/1
  }
  return Event_empty;
}


/**
 * @brief Returns the generator type a particular channel has
 *
 * @return uint8_t Generator type. Returns 0 if no generator is used
 */
uint8_t Event::get_generator() {
  return generator_type;
}


/**
 * @brief Sets a generator for a particular event channel
 *
 * @param event_generator Set generator.
 *                        Use gen:: for functionality present on all event channels.
 *                        Use genN:: for functionality present on channel N.
 */
void Event::set_generator(event::gen::generator_t event_generator) {
  // Store event generator setting for use in start() and stop()
  generator_type = (uint8_t)event_generator;
}


/**
 * @brief Function that lets you use an Arduino pin as event generator.
 *        Note that you'll have to manually keep track of which event channel
 *        objects to use to make sure the passed Arduino pin is supported.
 *        A different approach is to use Event::assign_generator_pin(), which
 *        automatically picks the correct event channel for you.
 *
 * @param pin_number Arduino pin number to use as event generator
 */
void Event::set_generator(uint8_t pin_number) {
  uint8_t port = digitalPinToPort(pin_number);
  uint8_t port_pin = digitalPinToBitPosition(pin_number);

  // Store event generator setting for use in start() and stop()

  if (port != NOT_A_PIN && port_pin != NOT_A_PIN) {
    generator_type = 0x40 | (port & 0x01) << 3 | port_pin;
  } else {
    generator_type = event::gen::disable;
  }
}



/**
 * @brief Static member function that sets a port pin as event
 *        generator and returns the object it has select as event channel.
 *        It will always try to select the lowest channel number as possible.
 *
 * @param port Port to use as event generator
 * @param port_pin Pin number on port to use as event generator
 * @return Event& Event channel object used as the event channel. Returns the
 *         Event_empty object if passed Arduino pin is invalid or no event
 *         channel is available
 */
Event& Event::assign_generator_pin(uint8_t port, uint8_t port_pin) {
  if (port != NOT_A_PIN && port_pin != NOT_A_PIN) {
    #if defined(PORTA_EVGENCTRL) // EA=series
    /* Not ready yet
      uint8_t gen0 = 0x40 | (port << 0x01);
      uint8_t gen1 = gen0 + 1;
      if (Event0.generator_type == event::gen::disable || Event0.generator_type & 0x4E == gen0) {
        if (Event0.generator_type == gen0){
          if (set_ev_gen(port,portpin)){
            Event0.generator_type = gen0;
            return Event0;
          }
        } else {
          Event0.generator_type = gen1;
          set_ev_gen(port,_SWAP(portpin))
        }
      }
      if (Event1.generator_type == event::gen::disable || Event0.generator_type & 0x4E == gen0) {
        if (Event1.enerator_type == gen0){
          Event1.generator_type = gen0;
          set_ev_gen(port,portpin)
        } else {
          if (set_ev_gen(port,portpin)) {
            Event1.generator_type = gen1;
            set_ev_gen(port,_SWAP(portpin))
            return Event1;
          }
        }
        return Event1;
      }
      if (Event0.generator_type == event::gen::disable || Event0.generator_type & 0x4E == gen0) {
        if (Event0.generator_type == gen0){
          Event0.generator_type = gen0;
          set_ev_gen(port,portpin)
        } else {
          Event0.generator_type = gen1;
          set_ev_gen(port,_SWAP(portpin))
        }
          if (set_ev_gen(port,portpin)){
            return Event0;
          }
        }
        return Event2;
      }
      if (Event0.generator_type == event::gen::disable || Event0.generator_type & 0x4E == gen0) {
        if (Event0.generator_type == gen0){
          Event0.generator_type = gen0;
          set_ev_gen(port,portpin)
        } else {
          Event0.generator_type = gen1;
          set_ev_gen(port,_SWAP(portpin))
        }
          if (set_ev_gen(port,portpin)){
            return Event0;
          }
        }
      if (Event0.generator_type == event::gen::disable || Event0.generator_type & 0x4E == gen0) {
        if (Event0.generator_type == gen0){
          Event0.generator_type = gen0;
          set_ev_gen(port,portpin)
        } else {
          Event0.generator_type = gen1;
          set_ev_gen(port,_SWAP(portpin))
        }
          if (set_ev_gen(port,portpin)){
            return Event0;
          }
        }
      if (Event0.generator_type == event::gen::disable || Event0.generator_type & 0x4E == gen0) {
        if (Event0.generator_type == gen0){
          Event0.generator_type = gen0;
          set_ev_gen(port,portpin)
        } else {
          Event0.generator_type = gen1;
          set_ev_gen(port,_SWAP(portpin))
        }
          if (set_ev_gen(port,portpin)){
            return Event0;
          }
        }
        */
    #elif !defined(MEGATINYCORE) // All non-tiny work one way.
      uint8_t gen = 0x40 | (port & 0x01) << 3 | port_pin;
      if (port == PA || port == PB) {
        if (Event0.generator_type == event::gen::disable || Event0.generator_type == gen) {
          Event0.generator_type = gen;
          return Event0;
        } else if (Event1.generator_type == event::gen::disable || Event1.generator_type == gen) {
          Event1.generator_type = gen;
          return Event1;
        }
      } else if (port == PC || port == PD) {
        if (Event2.generator_type == event::gen::disable || Event2.generator_type == gen) {
          Event2.generator_type = gen;
          return Event2;
        } else if (Event3.generator_type == event::gen::disable || Event3.generator_type == gen) {
          Event3.generator_type = gen;
          return Event3;
        }
      } else if (port == PE || port == PF) {
        if (Event4.generator_type == event::gen::disable || Event4.generator_type == gen) {
          Event4.generator_type = gen;
          return Event4;
        } else if (Event5.generator_type == event::gen::disable || Event5.generator_type == gen) {
          Event5.generator_type = gen;
          return Event5;
        }
      }
      #if defined(PIN_PG0)
        else if (port == PG) {
          if (Event6.generator_type == event::gen::disable || Event6.generator_type == gen) {
            Event6.generator_type = gen;
            return Event6;
          } else if (Event7.generator_type == event::gen::disable || Event7.generator_type == gen) {
            Event7.generator_type = gen;
            return Event7;
          }
        }
      #endif
    #elif MEGATINYCORE_SERIES == 2
      if (port != PC) {
        uint8_t gen = port_pin | (port == PA ? 0x40 : 0x48);
        if (Event0.generator_type == event::gen::disable || Event0.generator_type == gen) {
          Event0.generator_type = gen;
          return Event0;
        } else if (Event1.generator_type == event::gen::disable || Event1.generator_type == gen) {
          Event1.generator_type = gen;
          return Event1;
        }
      }
      if (port != PB) {
        uint8_t gen = port_pin | (port == PC ? 0x40 : 0x48);
        if (Event2.generator_type == event::gen::disable || Event2.generator_type == gen) {
          Event2.generator_type = gen;
          return Event2;
        } else if (Event3.generator_type == event::gen::disable || Event3.generator_type == gen) {
          Event3.generator_type = gen;
          return Event3;
        }
      }
      if (port != PA) {
        uint8_t gen = port_pin | (port == PB ? 0x40 : 0x48);
        if (Event4.generator_type == event::gen::disable || Event4.generator_type == gen) {
          Event4.generator_type = gen;
          return Event4;
        } else if (Event5.generator_type == event::gen::disable || Event5.generator_type == gen) {
          Event5.generator_type = gen;
          return Event5;
        }
      }
    #else // Oh no, it's a 0/1-series!
      // Prefer to use the more numerous async channels.
      uint8_t gen = port_pin + 0x0A;
      #if defined(PIN_PB0)
        if (port == PA)
      #endif
        {
          if (Event2.generator_type == event::gen::disable || Event2.generator_type == gen) {
            Event2.generator_type = gen;
            return Event2;
          }
          gen += 3;
          if (Event0.generator_type == event::gen::disable || Event0.generator_type == gen) {
            Event0.generator_type = gen;
            return Event0;
          }
        }
      if (port == PB) {
        #if MEGATINYCORE_SERIES == 1 // No Event1 on 0-series
        if (Event3.generator_type == event::gen::disable || Event3.generator_type == gen) {
          Event3.generator_type = gen;
          return Event3;
        }
        gen -= 2;
        if (Event1.generator_type == event::gen::disable || Event1.generator_type == gen) {
          Event1.generator_type = gen;
          return Event1;
        }
        #endif
      }
      #if defined (PIN_PC0) // can't test if PORTx is defined - all are defined everywhere)
        if (port == PC) {
          #if MEGATINYCORE_SERIES == 1 // no event 4 on 0-series
            if (Event4.generator_type == event::gen::disable || Event4.generator_type == gen) {
              Event4.generator_type = gen;
              return Event4;
            }
          #endif
          gen -= 3;
          if (Event0.generator_type == event::gen::disable || Event0.generator_type == gen) {
            Event0.generator_type = gen;
            return Event0;
          }
        }
      #endif // PC-bearing parts end here
    #endif // end of tiny 0/1 assign_generator_pin()
  }
  return Event_empty;
}

/**
 * @brief Static member function that sets an Arduino pin as event
 *        generator and returns the object it has select as event channel.
 *        It will always try to select the lowest channel number as possible.
 *
 * @param pin_number Arduino pin number to use as event generator
 * @return Event& Event channel object used as the event channel. Returns the
 *         Event_empty object if passed Arduino pin is invalid or no event
 *         channel is available
 */
Event& Event::assign_generator_pin(uint8_t pin_number)
{
  uint8_t port = digitalPinToPort(pin_number);
  uint8_t port_pin = digitalPinToBitPosition(pin_number);
  return Event::assign_generator_pin(port, port_pin);
}


/**
 * @brief Static member function that takes a event::gen::generator as a parameter and
 * finds an available channel for it. Starts with the highest possible channel
 * number, 7, and iterate down to 0
 *
 * @param event_generator generator to assign to an event channel
 * @return Event& Reference to the event object the generator has been assigned to.
 * Returns the Event_empty object if no generator has been assigned to a channel
 */


Event& Event::assign_generator(event::gen::generator_t event_generator)
{
  // Check if generator is already in use
  Event& channel = Event::get_generator_channel(event_generator);
  if (channel.get_channel_number() != 255)
    return channel;

  else
  {
    #if defined(EVSYS_CHANNEL9)
      if (Event9.get_generator() == event::gen::disable) {
        Event9.set_generator(event_generator);
        return Event9;
      } else
    #endif
    #if defined(EVSYS_CHANNEL8)
      if (Event8.get_generator() == event::gen::disable) {
        Event8.set_generator(event_generator);
        return Event8;
      } else
    #endif
    #if defined(EVSYS_CHANNEL7)
      if (Event7.get_generator() == event::gen::disable) {
        Event7.set_generator(event_generator);
        return Event7;
      } else
    #endif
    #if defined(EVSYS_CHANNEL6)
      if (Event6.get_generator() == event::gen::disable) {
        Event6.set_generator(event_generator);
        return Event6;
      } else
    #endif
    #if defined(EVSYS_CHANNEL5)
      if (Event5.get_generator() == event::gen::disable) {
        Event5.set_generator(event_generator);
        return Event5;
      } else
    #endif
    #if defined(EVSYS_CHANNEL4)
      if (Event4.get_generator() == event::gen::disable) {
        Event4.set_generator(event_generator);
        return Event4;
      } else
    #endif
    #if defined(EVSYS_CHANNEL3)
      if (Event3.get_generator() == event::gen::disable) {
        Event3.set_generator(event_generator);
        return Event3;
      } else
    #endif
    #if defined(EVSYS_CHANNEL2)
      if (Event2.get_generator() == event::gen::disable) {
        Event2.set_generator(event_generator);
        return Event2;
      } else
    #endif
    #if defined(EVSYS_CHANNEL1)
      if (Event1.get_generator() == event::gen::disable) {
        Event1.set_generator(event_generator);
        return Event1;
      } else
    #endif
    #if defined(EVSYS_CHANNEL0)
      if (Event0.get_generator() == event::gen::disable) {
        Event0.set_generator(event_generator);
        return Event0;
      } else
    #endif
    return channel; // if we're on this branch, we know chan is Event_empty
  }
}


/**
 * @brief Returns the Event channel number a particular user is connected to
 *
 * @param event_user The event user to check
 * @return int8_t Event channel number.
 *         Returns -1 if not connected to any event channel
 */
int8_t Event::get_user_channel_number(event::user::user_t event_user) {
  // Figure out what user register to read from to based on the passed parameter
  volatile uint8_t *user_register = &EVSYS_USERCCLLUT0A + (volatile uint8_t&)event_user;

  // Return which channel the user is connected to
  return *user_register - 1;
}


/**
 * @brief Returns the Event channel object a particular user is connected to
 *
 * @param event_user The event user to check
 * @return Event& Event channel object. Returns the Event_empty object if the
 *         user is not connected to any event channel
 */
Event& Event::get_user_channel(event::user::user_t event_user) {
  int8_t ch_number = get_user_channel_number(event_user);

  #if defined(EVSYS_CHANNEL0)
    if (Event0.channel_number == ch_number) {
      return Event0;
    }
  #endif
  #if defined(EVSYS_CHANNEL1)
    if (Event1.channel_number == ch_number) {
      return Event1;
    }
  #endif
  #if defined(EVSYS_CHANNEL2)
    if (Event2.channel_number == ch_number) {
      return Event2;
    }
  #endif
  #if defined(EVSYS_CHANNEL3)
    if (Event3.channel_number == ch_number) {
      return Event3;
    }
  #endif
  #if defined(EVSYS_CHANNEL4)
    if (Event4.channel_number == ch_number) {
      return Event4;
    }
  #endif
  #if defined(EVSYS_CHANNEL5)
    if (Event5.channel_number == ch_number) {
      return Event5;
    }
  #endif
  #if defined(EVSYS_CHANNEL6)
    if (Event6.channel_number == ch_number) {
      return Event6;
    }
  #endif
  #if defined(EVSYS_CHANNEL7)
    if (Event7.channel_number == ch_number) {
      return Event7;
    }
  #endif
  #if defined(EVSYS_CHANNEL8)
    if (Event8.channel_number == ch_number) {
      return Event8;
    }
  #endif
  #if defined(EVSYS_CHANNEL9)
    if (Event9.channel_number == ch_number) {
      return Event9;
    }
  #endif
  else {
    return Event_empty;
  }
}


/**
 * @brief Sets a user for a particular event channel
 *
 * @param event_user The event user to connect to a particular channel
 */
void Event::set_user(event::user::user_t event_user) {
  // Figure out what user register to write to based on the passed parameter
  uint8_t event_user_mask = event_user & 0x7F;
  #if defined(TINY_0_OR_1_SERIES)
    volatile uint8_t *user_register = &EVSYS_ASYNCUSER0 + (volatile uint8_t &)event_user_mask;
  #else
    volatile uint8_t *user_register = &EVSYS_USERCCLLUT0A + (volatile uint8_t &)event_user_mask;
  #endif
    // Connect user to the channel we're working with
    *user_register = channel_number + 1;
    // Set PORTMUX pin swap for EVOUT if selected as channel generator
  #if defined(TINY_0_OR_1_SERIES)
    if (event_user >= 0x08 && event_user <= 0x0A) {
      PORTMUX.CTRLA |= 1 << (event_user - 0x08);
    }
  #else
    if (event_user & 0x80) {
      #if defined(MEGACOREX)
        PORTMUX_EVSYSROUTEA |= (1 << ((event_user & 0x7F) - 0x09));
      #elif defined(__AVR_DA__)
        PORTMUX_EVSYSROUTEA |= (1 << ((event_user & 0x7F) - 0x0E));
      #elif defined(__AVR_DB__)
        PORTMUX_EVSYSROUTEA |= (1 << ((event_user & 0x7F) - 0x0D));
      #elif defined(__AVR_DD__)
        PORTMUX_EVSYSROUTEA |= (1 << ((event_user & 0x7F) - 0x0D));
      #elif MEGATINYCORE_SERIES == 2
        PORTMUX_EVSYSROUTEA |= (1 << ((event_user & 0x7F) - 0x0D));
      #endif
    }
  #endif
}

/**
 * @brief Sets an Arduino pin as a user for a particular event channel
 *
 * @param pin_number The Arduino pin number to use as a channel user
 * @return uint8_t Event channel user enum
           (for instance event::user::evouta_pin_pa2 pin PIN_PA2 is passed)
 *         Returns -1 if invalid pin is passed
 */
int8_t Event::set_user_pin(uint8_t pin_number) {
  uint8_t port = digitalPinToPort(pin_number);
  uint8_t port_pin = digitalPinToBitPosition(pin_number);

  int8_t event_user = -1;
  if (port != NOT_A_PIN && port_pin != NOT_A_PIN) {
    #if !defined(TINY_0_OR_1_SERIES)
    /* Woah, we were missing a huge optimization opportunity here.... but only for DA/DB-series parts
       - The users are numbered in the same orderas the ports.
       - PA is #defined as 0, PB as 2, etc.
       - there are no parts for which a port exists that has a pin 2 or 7, but which does not allow that pin to be used as an event output, except for tiny 0/1, where only pin 2 is an option...
       We basically **don't have to test the port** as long as it's a valid port as we just tested. This is probably like 6-8 instructions instead of several dozen */

      #if !defined(__AVR_DD__)
        uint8_t evout_user = (int8_t) event::user::evouta_pin_pa2;
        if (port_pin == 2) { //non-0/1 pin 2 handling
          event_user = (evout_user + port);
        } //non-0/1 pin 7 handling
        else if (port_pin == 7) {
          event_user = (0x80 | (evout_user + port));
        }
      #else
        uint8_t evout_user = -1;
        if (port_pin == 7 || port_pin == 2) {
          evout_user = port_pin == 7 ? 0x89 : 0x09;
        }
        if (port >= PC) {
          port--; // PA = 0 PC = 1, PD = 2, PF = 3
          if (port == PE) {
            port--; // We decremented port once already if it was >= PC, so the last valid port, PF now is PE.
          }
        }
        if (port <= PD) {
          event_user = evout_user + port;
        }
      #endif
    #else // Ugh, it's a 0/1-series....
      if (port_pin == 2) {
        #if defined (PIN_PB2)
          #if defined(PIN_PC2)
            if (port == PC) {
              event_user = (int8_t) event::user::evoutc_pin_pc2;
            } else
          #endif
          if (port == PB) {
              event_user = (int8_t) event::user::evoutb_pin_pb2;
          } else {
            event_user = (int8_t) event::user::evoutc_pin_pc2;
          }
        #else // No PIN_PB2, meaning it could only be an 8-pin tiny, and the only evout pin there is PA2.
          event_user = (int8_t) event::user::evouta_pin_pa2;
        #endif
      } // end if (port_pin==2)
    #endif // end 0/1-series
    else {
      return -1;
    }
    set_user((event::user::user_t)((uint8_t)event_user));
  }
  return event_user;
}
    /*
    #if defined(MEGATINYCORE)
      // megaTinyCore on 0/1-series only has EVOUT on pin 2
      if (port_pin == 2) {
        #if defined(PIN_PB2)
          if (port == PA) {
            event_user = event::user::evouta_pin_pa2;
          if (port == PB)
            event_user = event::user::evoutb_pin_pb2;
           else
            event_user = event::user::evoutb_pin_pc2;
        #else // No PIN_PB2, meaning it could only be an 8-pin tiny, and the only evout pin there is PA2.
          event_user = event::user::evouta_pin_pa2;
        #endif
      }
      // 2-series gets the option for the alternate output pins... but there are only 2. faster + smaller to test directly.
      #if (MEGATINYCORE_SERIES == 2)
        #if defined(PIN_PB7)
          if (pin_number == PIN_PB7){
            event_user = event::user::evoutb_pin_pb2;
          } else
        #endif
        if (pin_number == PIN_PA7) {
          event_user = event::user::evoutb_pin_pb7
        }
      #endif
    return event_user;
    #else  // DONE WITH the tinyAVRs!
    #if defined(PIN_PA2)             // 14-pin DD doesn't have PA2 or PA7. Nor will the DU14's
      if (port == PA) {
        if (port_pin == 2)
          event_user = event::user::evouta_pin_pa2;
        else if (port_pin == 7)
          event_user = event::user::evouta_pin_pa7;
      }
    #endif
    #if defined(PIN_PB2)
      else if (port == PB) {
        if (port_pin == 2) {
          event_user = event::user::evoutb_pin_pb2;
        }
        #if defined(PIN_PB7)
          if (port_pin == 7) {
            event_user = event::user::evoutb_pin_pb7;
          }
        #endif
      }
    #endif
    #if defined PIN_PC2 // DU-series sacrificed PORTC at the altar of native USB (along with their TCD+PLL and assorted other treasures)
      else if (port == PC) {
        if (port_pin == 2) {
          event_user = event::user::evoutc_pin_pc2;
        }
        #if defined(PIN_PC7)
          else if (port_pin == 7) {
            event_user = event::user::evoutc_pin_pc7;
          }
        #endif
      }
    #endif
    else if (port == PD) { /
      #if defined(PIN_PD2) // 14 and 20 pin DD has no PD0-3, all megaAVR 0s do
        if (port_pin == 2) {
          event_user = event::user::evoutd_pin_pd2;
        } else
      #endif
      if (port_pin == 7) {
        event_user = event::user::evoutd_pin_pd7;
      }
    }
    #if defined(PIN_PE2)
      else if (port == PE) {
        if (port_pin == 2){
          event_user = event::user::evoute_pin_pe2;
        }
        #if defined(PIN_PE7)
          else if (port_pin == 7) {
            event_user = event::user::evoutc_pin_pc7;
          }
        #endif
      }
    #endif
    else if (port == PF) {
      if (port_pin == 2)
        event_user = event::user::evoutf_pin_pf2;
      }
      #if defined(PIN_PF7)
      else if (portpin == 7) {
        event_user = event::user::evoutg_pin_pg7;
      }
    #if defined(PIN_PG2)
      if (port_pin == 2)
        event_user = event::user::evoutg_pin_pg2;
      else if (port_pin == 7)
        event_user = event::user::evoutg_pin_pg7;
    #endif
    */

/**
 * @brief Clears/removed a user from a particular event channel if set
 *
 * @param event_user The event user to remove from a particular channel
 */
void Event::clear_user(event::user::user_t event_user) {
  // Figure out what user register to write to based on the passed parameter
  uint8_t event_user_mask = event_user & 0x7F;
  volatile uint8_t *user_register = &EVSYS_USERCCLLUT0A + (volatile uint8_t &)event_user_mask;

  // Disconnect from event generator
  *user_register = 0x00;

  // Clear PORTMUX pin swap for EVOUT if selected as channel user
  if (event_user & 0x80)  {
    #if defined(__AVR_ATmegax08__) || defined(__AVR_ATmegax09__)
      PORTMUX_EVSYSROUTEA &= ~(1 << ((event_user & 0x7F) - 0x09));
    #elif defined(__AVR_DA__)
      PORTMUX_EVSYSROUTEA &= ~(1 << ((event_user & 0x7F) - 0x0E));
    #elif defined(__AVR_DB__)
      PORTMUX_EVSYSROUTEA &= ~(1 << ((event_user & 0x7F) - 0x0D));
    #endif
  }
}


/**
 * @brief Creates a software event for a particular event channel
 *
 */
void Event::soft_event() {
  // Write to the bit that represent the channel in the strobe register
  #if defined(EVSYS_STROBE)
    // megaAVR 0-series calls the register EVSYS.STROBE
    EVSYS.STROBE = (1 << channel_number);
  #elif defined(TINY_0_OR_1_SERIES)
    // Channels 0 and 1 are the sync channels, which are strobbed with the syncstrobe registers. The others are the async channels,
    // strobed with the asyncstrobe. Since the sync strobe has only 2 possible thing s to strobe, bit 0 or bit 1 corresponding to byte
    // values 1 and 2, we can just add 1 to the byte value to achieve that.
    if (channel_number < 2) {
      EVSYS.SYNCSTROBE = (channel_number + 1);
    } else {
      EVSYS.ASYNCSTROBE = (1 << (channel_number - 2));
    }
  #else
    // Everything newer calls the registers EVSYS.SWEVENTA or SWEVENTB for the few devices lucky enough to have more than 8 channels.
    #if defined(EVSYS_SWEVENTB)
      if (channel_number < 8) {
        EVSYS.SWEVENTA = (1 << channel_number);
      } else {
        /* Dirty trick: No available part has more than 10 event channels.
         * So if we are strobing one of those last two, the value we are writing to the register is iwith 1 or 2 (as with sync channel
         * on tiny 0/1). So channel number is 8 or 9, subtract 7 to get 1 or 2, and write that.
         * Doesn't save much flash,but variable-shift operations are surprisignly inefficient in speed. The case for SWEVENTA involves
         * a loop, 5 clocks in duration per bit
         */
        EVSYS.SWEVENTB = channel_number - 7;
      // EVSYS.SWEVENTB = (1 << (channel_number - 8));
      }
    #else
      EVSYS.SWEVENTA = (1 << channel_number);
    #endif
  #endif
}

static void __attribute__((__unused__)) _long_soft_event(uint8_t channel, uint8_t length);    // holds the bulky assembly routine for the long softevent.

void Event::long_soft_event(uint8_t length) {
  _long_soft_event(channel_number, length);
}

static void _long_soft_event(uint8_t channel, uint8_t length) {
  uint16_t strobeaddr;
  #if defined(EVSYS_STROBE)
    strobeaddr = (uint16_t) &EVSYS_STROBE;
  #elif defined(EVSYS_SYNCSTROBE)
    if (channel > 1) {
      channel -= 2;
      strobeaddr = (uint16_t) &EVSYS_ASYNCSTROBE;
    } else {
      strobeaddr = (uint16_t) &EVSYS_SYNCSTROBE;
    }
  #elif defined(EVSYS_SWEVENTB)
    if (channel > 7) {
      channel -= 8;
      strobeaddr = (uint16_t) &EVSYS_SWEVENTB;
    } else
      strobeaddr = (uint16_t) &EVSYS_SWEVENTA;
  #elif defined(EVSYS_SWEVENTA)
    strobeaddr = (uint16_t) &EVSYS_SWEVENTA;
  #else
    #error "Don't know the strobe register!"
  #endif
  channel = (1 << channel);
  __asm__ __volatile__ (
    "in r0, 0x3F"     "\n\t" // save SREG
    "cli"             "\n\t" // interrupts off
    "cpi %1, 4"       "\n\t"
    "brcs long_soft2" "\n\t" // less than 4 -> 2
    "breq long_soft4" "\n\t" // equal to 4 -> 4
    "cpi %1, 10"      "\n\t" // compare with 8
    "brcs long_soft6" "\n\t" // less than 10 (but more than 4) -> 6
    "breq long_soft10""\n\t" // equal to 10 -> 10otherwise they get 16.
    "st Z, %0"        "\n\t" // 16.
    "st Z, %0"        "\n\t" // 15
    "st Z, %0"        "\n\t" // 14
    "st Z, %0"        "\n\t" // 13
    "st Z, %0"        "\n\t" // 12
    "st Z, %0"        "\n\t" // 11
  "long_soft10:"      "\n\t"
    "st Z, %0"        "\n\t" // 10
    "st Z, %0"        "\n\t" // 9
    "st Z, %0"        "\n\t" // 8
    "st Z, %0"        "\n\t" // 7
  "long_soft6:"       "\n\t"
    "st Z, %0"        "\n\t" // 6
    "st Z, %0"        "\n\t" // 5
  "long_soft4:"       "\n\t"
    "st Z, %0"        "\n\t" // 4
    "st Z, %0"        "\n\t" // 3
  "long_soft2:"       "\n\t"
    "st Z, %0"        "\n\t" // 2
    "st Z, %0"        "\n\t" // 1
    "out 0x3f, r0"    "\n"   // restore SREG, reenabling interrupts.
    ::"r"((uint8_t) channel),"d"((uint8_t) length),"z" ((uint16_t) strobeaddr));
}


/**
 * @brief Starts the event generator for a particular event channel
 *
 * @param state Optional parameter. Defaults to true
 */

event::gen::generator_t Event::gen_from_peripheral(__attribute__((unused))TCB_t& timer, __attribute__((unused)) uint8_t event_type) {
  // Both args marked potentially unused, because on 0 and 1-series parts the generator numbers depend on the channel and it's a giant mess.
  uint8_t gentype = -1;
  #if defined(TINY_0_OR_1_SERIES)
    badCall("gen_from_peripheral() does not support channel-specific generators. The TCBs on 0/1-series are.");
  #else
    #if !(defined(DXCORE) || defined(TINY_2_SERIES))  // Dx-series and 2-series have ovf event. Others don't.
      if (event_type != 1) {
        return (event::gen::generator_t) -1;
      } else {
        gentype = 0;
      }
    #else
      if (event_type < 2) {
        gentype = event_type;
      }
    #endif
    if (&timer == &TCB0) {
      gentype += (uint8_t) event::gen::tcb0_capt;
    } else
    #if (defined TCB1)
    if (&timer == &TCB1) {
      gentype += (uint8_t) event::gen::tcb1_capt;
    } else
    #endif
    #if (defined TCB2)
    if (&timer == &TCB2) {
      gentype += (uint8_t) event::gen::tcb2_capt;
    } else
    #endif
    #if (defined TCB3)
    if (&timer == &TCB3) {
      gentype += (uint8_t) event::gen::tcb3_capt;
    } else
    #endif
    #if (defined TCB4)
    if (&timer == &TCB4) {
      gentype += (uint8_t) event::gen::tcb4_capt;
    } else
    #endif
    {
      gentype = -1;
    }
  #endif
  return (event::gen::generator_t) gentype;
}

event::user::user_t Event::user_from_peripheral(TCB_t& timer, uint8_t user_type) {
  uint8_t user = -1;
  #if !(defined(DXCORE) || defined(TINY_2_SERIES)) // Dx-series and 2-series have event count input, others don't.
    if (user_type != 1) {
      return (event::user::user_t) -1;
    } else {
      user = 0;
    }
  #else
    if (user_type < 2) {
      user = user_type;
    }
  #endif
  if (&timer == &TCB0) {
    user += (uint8_t) event::user::tcb0_capt;
  } else
  #if (defined TCB1)
  if (&timer == &TCB1) {
    user += (uint8_t) event::user::tcb1_capt;
  } else
  #endif
  #if (defined TCB2)
  if (&timer == &TCB2) {
    user += (uint8_t) event::user::tcb2_capt;
  } else
  #endif
  #if (defined TCB3)
  if (&timer == &TCB3) {
    user += (uint8_t) event::user::tcb3_capt;
  } else
  #endif
  #if (defined TCB4)
  if (&timer == &TCB4) {
    user += (uint8_t) event::user::tcb4_capt;
  } else
  #endif
  {
    user = -1;
  }
  return (event::user::user_t) user;
}


event::gen::generator_t Event::gen_from_peripheral(__attribute__((unused)) AC_t& comp) // mark as potentially unused so we don't get warning w/error
{
  #if defined(TINY_1_16K_PLUS)
    badCall("gen_from_peripheral() does not support channel-specific generators. The AC's larger 1-series are.");
  #else
    #if defined(AC0)
      if (&comp == &AC0)
        return event::gen::ac0_out;
      else
      #if defined(AC1)
        if(&comp == &AC1)
          return event::gen::ac1_out;
        else
        #endif
      #if defined(AC2)
      if(&comp == &AC2)
        return event::gen::ac2_out;
      #endif
    #endif
  #endif
  return (event::gen::generator_t) -1;
}

event::gen::generator_t Event::gen_from_peripheral(CCL_t& logic, uint8_t logicblock) {
  uint8_t retval = -1;
  if (&logic == &CCL) {
    #if defined(TINY_0_OR_1_SERIES)
      if (logicblock < 2)
    #elif defined(CCL_LUT4CTRLA)
      if (logicblock < 6)
    #else
      if (logicblock < 4)
    #endif
    {
      retval = logicblock + (uint8_t) (event::gen::ccl0_out);
    }
  }
  return (event::gen::generator_t) retval;
}

event::user::user_t Event::user_from_peripheral(CCL_t& logic, uint8_t user_type) {
  uint8_t retval = -1;
  if (&logic == &CCL) {
    #if !defined(TINY_0_OR_1_SERIES)
      #if defined(CCL_TRUTH4)
        if (user_type < 13)
      #else
        if (user_type < 9)
      #endif
      {
        retval = user_type;
      }
    #else
      if (user_type < 5) {
        retval = user_type + 2;
      }
    #endif
  }
  return (event::user::user_t) retval;
}


event::gen::generator_t Event::gen_from_peripheral(TCA_t& timer, uint8_t event_type) {
  uint8_t retval = -1;
  if (event_type < 5) {
    #if defined(TINY_0_OR_1_SERIES)
      if (&timer == &TCA0) {
        retval = event_type +2;
      }
    #else
      if (&TCA0 == &timer) {
        retval = event_type + 0x80;
      }
      #if defined(TCA1)
        else if (&TCA1 == &timer) {
          retval = event_type += 0x88;
        }
      #endif
    #endif
  }
  return (event::gen::generator_t) retval;
}

event::user::user_t Event::user_from_peripheral(TCA_t& timer, uint8_t user_type) {
  uint8_t user = -1;
  #if !(defined(DXCORE) || defined(TINY_2_SERIES))
    if (user_type != 0) {
      return (event::user::user_t) -1;
    }
  #else
    if (user_type > 1) {
      return (event::user::user_t) -1;
    }
  #endif
  user = user_type; // 0 or 1 for event user a or b (on parts with both, or 0 for parts hat only have one.
  #if defined(TCA1)
  if (&timer == &TCA1) {
    user += 2;
  } else
  #endif
  if (&timer != &TCA0) {
    return (event::user::user_t) -1;
  }
  #if defined(__AVR_DA__)
    user += 0x1B;
  #elif defined(__AVR_DB__) || defined(__AVR_DD__)
    user += 0x1A;
  #elif defined (MEGACOREX)
    user += 0x13;
  #elif defined (TINY_2_SERIES)
    user += 0x0E;
  #else
    user += 0x10;
  #endif
  return (event::user::user_t) user;
}

event::user::user_t Event::user_from_peripheral(USART_t& usart) {
    if (&usart == &USART0) {
      return event::user::usart0_irda;
    }
  #if defined(USART1)
    else if (&usart == &USART1) {
      return event::user::usart1_irda;
    }
  #endif
  #if defined(USART2)
    else if (&usart == &USART2) {
      return event::user::usart2_irda;
    }
  #endif
  #if defined(USART3)
    else if (&usart == &USART3) {
      return event::user::usart3_irda;
    }
  #endif
  #if defined(USART4)
    else if (&usart == &USART4) {
      return event::user::usart4_irda;
    }
  #endif
  #if defined(USART5)
    else if (&usart == &USART5) {
      return event::user::usart5_irda;
    }
  #endif
  return (event::user::user_t) -1;
}


void Event::start(bool state) {
  if (state) {
    // Write event generator setting to EVSYS_CHANNELn register
    channel_address = generator_type;
  } else {
    // Disable event generator
    channel_address = event::gen::disable;
  }
}


/**
 * @brief Stops the event generator for a particular event channel
 *
 */
void Event::stop() {
  start(false);
}
