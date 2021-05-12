#include "Event.h"

// Pre-defined objects

#if MEGATINYCORE_SERIES != 2
  #if defined(EVSYS_SYNCCH0)
    Event Event0(0, EVSYS_SYNCCH0);
  #endif
  #if defined(EVSYS_SYNCCH1)
    Event Event1(1, EVSYS_SYNCCH1);
  #endif
  #if defined(EVSYS_ASYNCCH0)
    Event Event2(2, EVSYS_ASYNCCH0);
  #endif
  #if defined(EVSYS_ASYNCCH1)
    Event Event3(3, EVSYS_ASYNCCH1);
  #endif
  #if defined(EVSYS_ASYNCCH2)
    Event Event4(4, EVSYS_ASYNCCH2);
  #endif
  #if defined(EVSYS_ASYNCCH3)
    Event Event5(5, EVSYS_ASYNCCH3);
  #endif
#else
  #if defined(EVSYS_CHANNEL0)
    Event Event0(0, EVSYS_CHANNEL0);
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
 * @brief Returns the Event channel number a particular user is connected to
 *
 * @param event_user The event user to check
 * @return int8_t Event channel number. Returns -1 if not connected to any event channel
 */
int8_t Event::get_user_channel(user::user_t event_user) {
  uint8_t event_user_mask = event_user & 0x7F;
  // Figure out what user register to read from to based on the passed parameter
  #if MEGATINYCORE_SERIES == 2
  volatile uint8_t *user_register = &EVSYS_USERCCLLUT0A + (volatile uint8_t &)event_user_mask;
  #else
  volatile uint8_t *user_register = &EVSYS_ASYNCUSER0 + (volatile uint8_t &)event_user_mask;
  #endif
  // Return what channel the user is connected to
  return *user_register - 1;
}


/**
 * @brief Sets a generator for a particular event channel
 *
 * @param event_generator Set generator.
 *                        Use gen:: for functionality present on all event channels.
 *                        Use genN:: for functionality present on channel N.
 */
void Event::set_generator(uint8_t event_generator) {
  // Store event generator setting for use in start() and stop()
  generator_type = event_generator;
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
 * @brief Sets a user for a particular event channel
 *
 * @param event_user The event user to connect to a particular channel
 */
void Event::set_user(user::user_t event_user) {
  // Figure out what user register to write to based on the passed parameter
  uint8_t event_user_mask = event_user & 0x7F;
  #if MEGATINYCORE_SERIES == 2
  volatile uint8_t *user_register = &EVSYS_USERCCLLUT0A + (volatile uint8_t &)event_user_mask;
  #else
  volatile uint8_t *user_register = &EVSYS_ASYNCUSER0 + (volatile uint8_t &)event_user_mask;
  #endif
  // Connect user to the channel we're working with
  *user_register = channel_number + 1;

  // Set PORTMUX pin swap for EVOUT if selected as channel generator
  #if defined(PORTMUX_EVSYSROUTEA)
  if (event_user & 0x80) {
    PORTMUX_EVSYSROUTEA |= (1 << ((event_user & 0x7F) - 0x09));
  }
  #elif defined(PORTMUX_CTRLA)
  if ((event_user >= 8) && (event_user <= 0x0D)) {
    PORTMUX_CTRLA |= (1 << ((event_user & 0x7F) - 0x09));
  }
  #endif
}


/**
 * @brief Clears/removed a user from a particular event channel if set
 *
 * @param event_user The event user to remove from a particular channel
 */
void Event::clear_user(user::user_t event_user) {
  // Figure out what user register to write to based on the passed parameter
  uint8_t event_user_mask = event_user & 0x7F;
  #if MEGATINYCORE_SERIES == 2
  volatile uint8_t *user_register = &EVSYS_USERCCLLUT0A + (volatile uint8_t &)event_user_mask;
  #else
  volatile uint8_t *user_register = &EVSYS_ASYNCUSER0 + (volatile uint8_t &)event_user_mask;
  #endif
  // Disconnect from event generator
  *user_register = 0x00;


  #if defined(PORTMUX_EVSYSROUTEA)
  if (event_user & 0x80) {
    PORTMUX_EVSYSROUTEA &= ~(1 << ((event_user & 0x7F) - 0x09));
  }
  #elif defined(PORTMUX_CTRLA)
  if ((event_user >= 8) && (event_user <= 10)) {
    PORTMUX_CTRLA &= ~(1 << ((event_user & 0x7F) - 0x08));
  }
  #endif
}


/**
 * @brief Creates a software event for a particular event channel
 *
 */
void Event::soft_event() {
  // Write to the bit that represent the channel in the strobe register
  #if defined(EVSYS_STROBE)
  // megaAVR 0-series
  EVSYS.STROBE = (1 << channel_number);
  #elif defined (EVSYS_ASYNCSTROBE)
  // tinyAVR 0/1-series
  if (channel_number < 2) {
    // channel_number is either 0 or 1, and we want to strobe the corresponding channel, so we want to write either 1 << 0 or 1 <<1.
    // but that's the same as 0 + 1 or 1 + 1 for this very limited case, and it's either equal in size if the compiler is smart
    // or potentially much worse if it's not. I'm seeing 3 instructions from 1 << channel number if it's smart, but potentially a lot
    // more if it's stupid...
    EVSYS.SYNCSTROBE = (channel_number + 1);
  } else {
    EVSYS.ASYNCSTROBE = (1 << (channel_number - 2));
  }
  #else
  // This is a civilized part which uses the 2020 version of EVSYS
  // we expect there to be an EVSYS.SWEVENTA channel plus an
  // EVSYS.SWEVENTB it it has more than 8 event channels.
  #if defined(EVSYS_SWEVENTB)
  if (channel_number < 8) {
    EVSYS.SWEVENTA = (1 << channel_number);
  } else {
    // for the special case of only 10 channels, this is smaller:
    // channel number will be 8 or 9, so this will be 1 or 2
    // whereas the compiler would need to prepare for anything in the
    // otherwise.  No parts with more than 10 event channels exist or
    // have been announced. Whereas for the tinies, I did this for
    // size, here I do it more for speed, since these parts have
    // enough flash that the penalty is barely measurable.
    EVSYS.SWEVENTB = channel_number - 7;
    // EVSYS.SWEVENTB = (1 << (channel_number - 8));
  }
  #else
  EVSYS.SWEVENTA = (1 << channel_number);
  #endif
  #endif
}


/**
 * @brief Starts the event generator for a particular event channel
 *
 * @param state Optional parameter. Defaults to true
 */
void Event::start(bool state) {
  if (state) {
    // Write event generator setting to EVSYS_CHANNELn register
    channel_address = generator_type;
  } else {
    // Disable event generator
    channel_address = gen::disable;
  }
}


/**
 * @brief Stops the event generator for a particular event channel
 *
 */
void Event::stop() {
  start(false);
}
