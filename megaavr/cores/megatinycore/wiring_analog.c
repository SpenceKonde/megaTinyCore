/*
  wiring_analog.c - analog input and output
  Part of Arduino - http://www.arduino.cc/

  Copyright (c) 2005-2006 David A. Mellis

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General
  Public License along with this library; if not, write to the
  Free Software Foundation, Inc., 59 Temple Place, Suite 330,
  Boston, MA  02111-1307  USA

  Modified 28 September 2010 by Mark Sproul
*/

#include "wiring_private.h"
#include "pins_arduino.h"
#include "Arduino.h"



inline __attribute__((always_inline)) void check_valid_digital_pin(pin_size_t pin) {
  if (__builtin_constant_p(pin)) {
    if (pin >= NUM_TOTAL_PINS && pin != NOT_A_PIN)
    // Exception made for NOT_A_PIN - code exists which relies on being able to pass this and have nothing happen.
    // While IMO very poor coding practice, these checks aren't here to prevent lazy programmers from intentionally
    // taking shortcuts we disapprove of, but to call out things that are virtually guaranteed to be a bug.
    // Passing -1/255/NOT_A_PIN to the digital I/O functions is most likely intentional.
    badArg("digital I/O function called  is constant, but not a valid pin");
  }
}

inline __attribute__((always_inline)) void check_valid_analog_pin(pin_size_t pin) {
  if (__builtin_constant_p(pin)) {
    #if MEGATINYCORE_SERIES == 2
      if (pin != ADC_TEMPERATURE && pin != ADC_GROUND && pin != ADC_DACREF0 && pin != ADC_VDDDIV10)
    #elif defined(ADC_DAC0) // 1-series
      if (pin != ADC_DAC0 && pin != ADC_INTREF && pin != ADC_TEMPERATURE && pin != ADC_GROUND)
    #else // 0-series.
      if (pin != ADC_INTREF && pin != ADC_TEMPERATURE && pin != ADC_GROUND)
    #endif
    {   // above cases cover valid internal sources.
      if (pin & 0x80) {
        #if MEGATINYCORE_SERIES == 2
          if ((pin & 0x7F) >= NUM_ANALOG_INPUTS /* || pin != 0x80 */)  // channel 0 is not connected to PA0 - but let's not block it just yet, if it's actually tied to ground, could it be helpful for offset cal?
        #else
          if ((pin & 0x7F) >= NUM_ANALOG_INPUTS)
        #endif
        {
          badArg("analogRead called with constant channel that is neither valid analog channel nor valid pin");
        }
      } else {
        pin = digitalPinToAnalogInput(pin);
        if (pin == NOT_A_PIN) {
          badArg("analogRead called with constant pin that is not a valid analog pin");
        }
      }
    }
  }
}

inline __attribute__((always_inline)) void check_valid_analog_ref(uint8_t mode) {
  if (__builtin_constant_p(mode)) {
    #if MEGATINYCORE_SERIES == 2
      if (!(mode == EXTERNAL || mode == VDD || mode== INTERNAL1V024 || mode== INTERNAL2V048 || mode== INTERNAL4V1 || mode== INTERNAL2V5))
    #elif defined(EXTERNAL)
      if (!(mode == EXTERNAL || mode == VDD || mode== INTERNAL0V55  || mode== INTERNAL1V1   || mode== INTERNAL1V5 || mode== INTERNAL2V5 || mode== INTERNAL4V34))
    #else
      if (!(mode == VDD || mode== INTERNAL0V55  || mode== INTERNAL1V1   || mode== INTERNAL1V5 || mode== INTERNAL2V5 || mode== INTERNAL4V34))
    #endif
    badArg("analogReference called with argument that is not a valid analog reference");
  }
}

inline __attribute__((always_inline)) void check_valid_enh_res(uint8_t res) {
  if (__builtin_constant_p(res)) {
    if (res < 0x80) {
      if (res < ADC_NATIVE_RESOLUTION_LOW) {
            badArg("When a resolution is passed to analogReadEnh, it must be at least the minimum native resolution (8 bits)");
      } else if (res > ADC_MAX_OVERSAMPLED_RESOLUTION) {
        badArg("The highest resolution obtainable on these parts through oversampling and decimation with a single ADC operation 13 bits on 0/1-series or 17 on 2-series");
      }
  #if MEGATINYCORE_SERIES == 2
    } else if ((res & 0x7F) > 0x0A) {
  #else
    } else if ((res & 0x7F) > 0x06) {
  #endif
      badArg("Accumulation number invalid - use one of the ADC_ACC_# constants for raw (undecimated) accumulated readings");
    }
  }
}


inline __attribute__((always_inline)) void check_valid_dac_ref(uint8_t mode) {
  if (__builtin_constant_p(mode)) {
    if (mode >= 5)
      badArg("DACReference called with argument that is not a valid DAC reference - VDD is not a valid reference tinyAVR parts!");
  }
}

inline __attribute__((always_inline)) void check_valid_duty_cycle(int16_t val) {
  if (__builtin_constant_p(val)) {
    if (val < 0 || val > 255)
      badArg("analogWrite duty cycle called with a constant not between 0 and 255");
  }
}

inline __attribute__((always_inline)) void check_valid_resolution(uint8_t res) {
  if (__builtin_constant_p(res))
  #if MEGATINYCORE_SERIES == 2
    if (res !=8 && res != 10 && res != 12)
      badArg("analogReadResolution called with invalid argument - valid options are 8, 12, or 10 (compatibility mode).");
  #else
    if (res !=8 && res != 10)
      badArg("analogReadResolution called with invalid argument - valid options are 8 or 10.");
  #endif
}


#if MEGATINYCORE_SERIES == 2
  // need a variable to store the resolution selected for analogRead - 8, 10, or 12,
  // as well as other options, such as whether to autodisable the PGA or leave it running.
  // Default resolution 10 bits, turn off PGA enabled
  static uint8_t _analog_options= 0x80 | 10;
#endif

inline bool analogReadResolution(uint8_t res) {
  check_valid_resolution(res);
  #if MEGATINYCORE_SERIES == 2
    bool temp = (res == 8 || res == 10 || res == 12);
    _analog_options = (_analog_options & 0xF0) | (temp ? res : 10); // just set that variable, setting the bit is awkward.
    return temp;
  #else
    if (res == 8) {
      ADC0.CTRLA |= ADC_RESSEL_bm;
      return true;
    }
    ADC0.CTRLA &= ~ADC_RESSEL_bm;
    return (res == 10);
  #endif
}

#ifdef DAC0
void DACReference(uint8_t mode) {
  check_valid_dac_ref(mode);
  if (mode < 5) {
    VREF.CTRLA = mode | (VREF.CTRLA & (~VREF_DAC0REFSEL_gm));
  }
}
#else
void DACReference(__attribute__ ((unused))uint8_t mode) {
  badCall("DACreference is not available - this part does not have a DAC");
}
#endif
/*----------------------------------------------------------------------------
 * Core ADC functionality implementations
 *----------------------------------------------------------------------------
 * Because the 2-series ADC is so radically different from the 0/1-series
 * (and, for that matter, just about everything else), it was most efficient
 * to separate out the core of the ADC stuff, where the viable shared code was
 * virtually nothing, and put it all in one part of the file, rather than the
 * normal practice of having the function defined in one place but with the
 * parts that are different controlled by #if's. Here, almost every line is
 * different!
 *
 * The only other time this has happened in the history of the AVR instruction
 * set, with any perpheral, is when the modern/AVRxt parts were introduced...
 * I hope we're not heading for total rewrites being required every few years.
 * I think this is just a transitional state; I couldn't see how to scale the
 * ADC like the AVR Dx-series has to something with this level of capability
 * either.
 *
 * ----------------------------------------------------------------------------
 * Functions:
 *
 * int16_t analogRead(uint8_t pin)
 *     The standard analogRead(). Single-ended, and resolution set by
 *     analogReadResolution(), default 10.
 *     large negative values are returned in the event of an error condition.

 * bool analogReadResolution(uint8_t resolution)
 *     Sets resolution for the analogRead() function. Unlike stock version,
 *     this returns true/false. If it returns false, the value passed was
 *     invalid, and resolution is set to the default, 10 bits.
 *     ONLY 8, 10 and 12 (for 2-series only) are valid. To use
 *     any other value, you must use analogReadEnh(). The way iut is done on
 *     the Zero/Due/Etc is 1) wasteful of limited resources, and 2) dishonest
 *     because when you ask for a resolution it can't provide, it will lie and
 *     pad the value with zeros.
 * bool analogSampleDuration(uint8_t dur)
 *     Sets sampling duration (SAMPLEN on 0/1 or SAMPDUR on 2-series)
 *     For high impedance voltage sources, use a longer sample length.
 *     For faster reads, use a shorter sample length. Passing 0 will
 *     approximately double analogRead() speed.
 *     Returns true if value is valid; on 0/1-series maximum is 31 (33 ADC clock
 *     sampling duration); on 2-series any 8-bit value is valid, and duration is
 *     this many plus 0.5 (without PGA) or 1 (with PGA) ADC clocks.
 *     ADC clock targeted by default at startup is 1~1.25 MHz on 0/1, and ~2 MHz on 2-series.
 * uint8_t getAnalogSampleDuration()
 *     Trivial macro (located in Arduino.h). Returns the argument that you would pass to
 *     analogSampleDuration to get the current setting back (useful before
 *     changing it to something exotic)
 * void ADCPowerOptions(uint8_t options)
 *     0 - disable PGA, turned on and off in future as above.
 *     1 - enable PGA ahead of time for the next measurement after which it will be turned off as above.
 *     2 - disable PGA, and in future turn if off immediately after use.
 *     3 - enable PGA, leave it on until explicitly turned off. This is a power hog.
 * int32_t analogReadEnh(uint8_t pin, int8_t res=ADC_NATIVE_RESOLUTION,
 *                                                          uint8_t gain=0)
 *     Enhanced analogRead(). Still single-ended, res is resolution in bits,
 *     which range from 8 to the largest value that can be obtained from using
 *     the accumlation feature and a single "start" command to oversample x4
 *     per extra bit, followed by decimation/2 per extra bit (the math is
 *     described in Microchip/Atmel app notes). This maximum is 13 bits for
 *     0/1-series parts, and 17 bits for 2-series parts.
 *     Alternately, to get the raw accumulated ADC readings, pass one of the
 *     ADC_ACC_n constants for the second argument where n is a power of 2 up
 *     to 64 (0/1-series) 128 (Dx-series) or 1024 (2-series).
 *     On 2-series (but not Dx-series parts, which don't have a PGA) the PGA
 *     can be used. I was shocked when I saw that the PGA could be used with
 *     single-ended reads too, but they explicitly state that. Valid options
 *     for gain are 0 (PGA disabled), 1 (unity gain, but can improve performance
 *     of burst reads under some circumstances, and powers of 2 up to 16)
 * int32_t analogReadDiff(uint8_t pos, uint8_t neg,
 *                      int8_t res=ADC_NATIVE_RESOLUTION, uint8_t gain=0)
 *     Uses the differential ADC functionality to take a differential ADC
 *     measurement between the specified positive and negative pins. Only pins
 *     on PORTA (tinyAVR 2-series) or on PORTD/PORTE (AVR Dx-series) can be the
 *     negative input to the ADC
 *     res works the same as for analogReadEnh.
 *     gain specifies that the programmable gain amplifier should be used.
 *     If gain = 0, the PGA will not be used. Valid options are 1, 2, 4, 8
 *     or 16 to configure the PGA to amplify by that factor.
 *
 *----------------------------------------------------------------------------
 * Errors:
 * analogReadEnh() or analogReadDiff() can return:
 *   ADC_ENH_ERROR_BAD_PIN_OR_CHANNEL (-2100000000)
 *     The selected pin has no analog input capability, does not exist,
 *     or, if it's a channel number (from ADC_CH(channel number), it
 *     does not exist or is otherwise invalid.
 *   ADC_ENH_ERROR_BUSY (-2100000001)
 *     Not yet enabled - will be an optional error in future update, maybe.
 *     The ADC is already taking a reading - either in free-running mode
 *     or through some user-implemented interrupt driven ADC routine.
 *   ADC_ENH_ERROR_INVALID_SAMPLE_LENGTH (-2100000002)
 *     Not yet enabled - will be an optional error in future update, maybe.
 *     On the 0/1-series parts, if sample length is specified, it must be
 *     below 0x1F (only 5 bits are used). That corresponds to 33 ACD clocks
 *     On the 2-series parts, this error is never returned, because all
 *     values between 0 and 255, inclusive, are valid.
 *     Currently, instead of returning an error, this will just set sample
 *     length to the maximum supported value.
 *   ADC_ENH_ERROR_RES_TOO_LOW (-2100000003)
 *     Resolution must not be lower than the lowest native resolution that
 *     the part supports (Core presents this as ADC_NATIVE_RESOLUTION_LOW)
 *     Get 8-bits and divide it if this is required; this is a rare
 *     use-case, and it is not appropriate to force the overhead on
 *     everyone else.
 *   ADC_ENH_ERROR_RES_TOO_HIGH (-2100000004)
 *    The 0/1-series can get 3 extra bits by oversampling 64x and right-
 *    shifting 3 times. The 2-series can get 5 by oversampling by a whopping
 *    1024x (taking 13-160 ms depending on options) and then right-shifting
 *    5 times. Thus, maximums are 13 and 17. Alternately, one of the
 *    ADC_ACC_n constants may be used, where n is 2, 4, 8, 16, 32, 64, or
 *    on 2-series only, 128, 256, 512, or 1024 - in this case, that many
 *    samples will be taken through burst/accumulation mode and returned
 *    without decimation.
 * analogReadDiff() can also return:
 *   ADC_ENH_ERROR_BAD_NEG_PIN (-2100000005)
 *    Returned by analogReadDiff when the requested pin for the negative
 *    input is not valid. Negative pins can only be on PORTA (for tinyAVR)
 *    or PORTD/PORTE (for Dx-series), and the selection of available
 *    channels is more limited.
 *
 * printADCRuntimeError(uint32_t error, &UartClass DebugSerial)
 *   Prints a text description of an error returnedby analogRead,
 *   analogReasdEnh(),or analogReadDiff() to the specified serial device
 *   Ex:
 *    printADCRuntimeError(-2100000003, &Serial);
 *    will peinr "ADC_ENH_ERROR_RES_TOO_LOW"
 *    Will print nothing and return false if the result wasn't an error
 *    Inefficient of flash space..
 ****************************************************************************/


#if MEGATINYCORE_SERIES == 2

  /*****************************************************
  START 2-series analogRead/analogReadXxxx functions
  *****************************************************/
  #define SINGLE_ENDED 254

  void analogReference(uint8_t mode) {
    check_valid_analog_ref(mode);
    #if defined(STRICT_ERROR_CHECKING)
      if (mode > 7) return;
    #else
      mode &= 7;
    #endif
    if (mode != 1 && mode != 3) {
      ADC0.CTRLC = TIMEBASE_1US | mode;
    }
    // Uh? Is that it? That was, ah, a tiny bit simpler.
  }

  int16_t analogRead(uint8_t pin) {
    check_valid_analog_pin(pin);
    if (pin < 0x80) {
      // If high bit set, it's a channel, otherwise it's a digital pin so we look it up..
      pin = digitalPinToAnalogInput(pin);
    } else {
      pin &= 0x3F;
    }
    #if PROGMEM_SIZE < 8096
      if (pin > 0x33) { // covers most ways a bad channel could come about
    #else
      if (pin > NUM_ANALOG_INPUTS && ((pin < 0x30) || (pin > 0x33))) {
    #endif
      return ADC_ERROR_BAD_PIN_OR_CHANNEL;
    }
    if (!ADC0.CTRLA & 0x01) return ADC_ERROR_DISABLED;

    if (ADC0.COMMAND & ADC_START_gm) return ADC_ERROR_BUSY;
    // gotta be careful here - don't want to shit ongoing conversion - unlikle classic AVRs
    // they say there is no buffering here!
    /* Select channel */
    ADC0.MUXPOS = pin; // VIA bit = 0;
    uint8_t command = (_analog_options & 0x0F) > 8 ? 0x11 : 0x01;
    /* Start conversion */
    ADC0.COMMAND = command;

    /* Wait for result ready */
    while (!(ADC0.INTFLAGS & ADC_RESRDY_bm));
    // if it's 10 bit compatibility mode, have to rightshift twice.
    if ((_analog_options & 0x0F) == 10) {
      int16_t temp = ADC0.RESULT;
      temp >>= 2;
      return temp;
    }
    return ADC0.RESULT;
  }


  inline __attribute__((always_inline)) void check_valid_negative_pin(uint8_t pin) {
    if (__builtin_constant_p(pin)) {
      if (pin < 0x80) {
        // If high bit set, it's a channel, otherwise it's a digital pin so we look it up..
        pin = digitalPinToAnalogInput(pin);
      }
      pin &= 0x3F;
      if (pin != 0x33 && pin != 0x31 && pin != 0x30 && pin > 0x07) {
        badArg("Invalid negative pin - valid options are ADC_GROUND, ADC_VDDDIV10, ADC_DACREF0, or any pin on PORTA.");
      }
    }
  }

  void ADCPowerOptions(uint8_t options) {
    // 0b SSEEPPLL
    // SS = run standby
    // 00 = no change to run standby
    // 01 = no change to run standby
    // 10 = turn off run standby
    // 11 = turn on run standby
    // EE = ENABLE
    // 00 = Do not enable or disable ADC.
    // 01 = Do not enable or disable ADC.
    // 10 = Disable the ADC.
    // 11 = Enable the ADC.
    // LL = LOWLAT
    // 00 = Do nothing.  No change to whether ADC enabled or LOWLAT bit.
    // 01 = Do nothing.  No change to whether ADC enabled or LOWLAT bit.
    // 10 = LOWLAT on. No change to whether ADC enabled.
    // 11 = LOWLAT off. No change to whether ADC enabled.
    // 00 = Do nothing,
    // 01 = Do nothing,
    // 10 = LOWLAT on.
    // 11 = LOWLAT off.
    // PP = PGA stay-on
    // 00 = No action
    // 01 = Turn off PGA, settings unchanged. It will be enabled next time is requested, but will not automatically turn off.
    // 10 = Turn on PGA, and don't turn it off automatically.
    // 11 = turn off PGA now and automatically after each use
    uint8_t temp = ADC0.CTRLA; //performance.
    if (options & 0x02) {
      ADC0.CTRLA = 0; // hopwfully workaround lowlat errata by ensuring that everything is turned off.
      // and configuring lowlat mode.
      if (options & 0x01) {
        ADC0.CTRLA |=  ADC_LOWLAT_bm;
        temp |= ADC_LOWLAT_bm;
      } else {
        ADC0.CTRLA &= ~ADC_LOWLAT_bm;
        temp &= ~ADC_LOWLAT_bm;
      }
    }
    if (options & 0x20) {
      if (options & 0x10) {
        temp |= 1; // ADC on
      } else {
        temp &= 0xFE; // ADC off
      }
    }
    if (options & 0x80) {
      if (options & 0x40) {
        temp |= 0x80; // run standby
      } else {
        temp &= 0x7F; // no run standby
      }
    }
    ADC0.CTRLA = temp; //now we apply enable and standby, and lowlat has been turned on, hopefully that's good enough for the errata.
    if (options & 0x04) { // turn off PGA.
      ADC0.PGACTRL &= ~ADC_PGAEN_bm;
      if (options & 0x08)  {
        _analog_options &= 0x7F;
      } else {
        _analog_options |= 0x80;
      }
    } else { // not instruction to turn off PGA
      if (options & 0x08) { // is it in fact an instruction to turn on the PGA and leave it on?
        ADC0.PGACTRL |= ADC_PGAEN_bm; // turn on the PGA
        _analog_options &= 0x7F;      // turn off the auto-shutoff. If they told us to turn it on explicitly, surely they don't want us to turn it off of our own accord.
      }
    }
    // What a mess!
  }

  bool analogSampleDuration(uint8_t sampdur) {
    // any uint8_t is a legal value...
    ADC0.CTRLE = sampdur;
    return true;
  }

  int32_t _analogReadEnh(uint8_t pin, uint8_t neg, uint8_t res, uint8_t gain) {
    if (!(ADC0.CTRLA & 0x01)) return ADC_ENH_ERROR_DISABLED;
    uint8_t sampnum;
    if (res > 0x80) { // raw accumulation
      sampnum = res & 0x7F;
      if (sampnum > 10) return ADC_ENH_ERROR_RES_TOO_HIGH;
    } else {
      if (res < ADC_NATIVE_RESOLUTION_LOW) return ADC_ENH_ERROR_RES_TOO_LOW;
      if (res > 17) return ADC_ENH_ERROR_RES_TOO_HIGH;
      sampnum = (res > ADC_NATIVE_RESOLUTION ? ((res - ADC_NATIVE_RESOLUTION) << 1) : 0);
    }

    if (pin < 0x80) {
      // If high bit set, it's a channel, otherwise it's a digital pin so we look it up..
      pin = digitalPinToAnalogInput(pin);
    } else {
      pin &= 0x3F;
    }
    #if PROGMEM_SIZE < 8096
      if (pin > 0x33) { // covers most ways a bad channel could come about
    #else
      if (pin > NUM_ANALOG_INPUTS && ((pin < 0x30) || (pin > 0x33))) {
    #endif
      return ADC_ENH_ERROR_BAD_PIN_OR_CHANNEL;
    }
    if (neg != SINGLE_ENDED) {
      if (neg < 0x80) {
        // If high bit set, it's a channel, otherwise it's a digital pin so we look it up..
        neg = digitalPinToAnalogInput(neg);
      } else {
        neg &= 0x3F;
      }

      #if PROGMEM_SIZE < 8096
        if (neg > 0x33)  // covers most ways a bad channel could come about
      #else
        if (neg > NUM_ANALOG_INPUTS && ((neg != 0x30) && (neg != 0x33) && (neg != 0x31)))
      #endif
      {
        return ADC_DIFF_ERROR_BAD_NEG_PIN;
      }
      neg &= 0x3F;

    }
    pin &= 0x3F;

    if (ADC0.COMMAND & ADC_START_gm) return ADC_ENH_ERROR_BUSY;
    if (gain !=0) {
      uint8_t gainbits =0;
      while (gain > 1) {
        gain >>= 1;
        gainbits+=32;
      }
      ADC0.PGACTRL = (ADC0.PGACTRL & ~ADC_GAIN_gm) | gainbits | ADC_PGAEN_bm;
    }

    ADC0.MUXPOS = (gain ? ADC_VIA_PGA_gc:0) | pin;
    if (neg != SINGLE_ENDED) {
      ADC0.MUXNEG = (gain ? ADC_VIA_PGA_gc:0) | neg;
    }
    ADC0.CTRLF = sampnum;
    uint8_t command = ((neg != SINGLE_ENDED)?0x80:0) | ((res == 8) ? ADC_MODE_SINGLE_8BIT_gc : (res > ADC_NATIVE_RESOLUTION ? ADC_MODE_BURST_gc : ADC_MODE_SINGLE_12BIT_gc)) | 1;
    ADC0.COMMAND = command;
    while (!(ADC0.INTFLAGS & ADC_RESRDY_bm));
    int32_t result = ADC0.RESULT;

    if (res < 0x80 && res > ADC_NATIVE_RESOLUTION) {
      uint8_t shift = res - ADC_NATIVE_RESOLUTION ; // - 1;
      while (shift) {
        result >>= 1;
        shift--;
      }
      // Sanity checks
      // uint8_t roundup=result&0x01;
      // result >>= 1;
      // result += roundup;
    } else if (res == 8) {
      ; // do nothing
    } else if (res < ADC_NATIVE_RESOLUTION) {
      uint8_t shift = ADC_NATIVE_RESOLUTION - res;
      result >>= shift;
    }

    // res > 0x80 (raw accumulate) or res == 8, res == 12 need no adjustment.
    if (_analog_options & 0x80) { // this bit controls autoshutoff of PGA.
      ADC0.PGACTRL &= ~ADC_PGAEN_bm;
    }
    return result;
  }

  inline __attribute__((always_inline)) int32_t analogReadEnh(uint8_t pin, uint8_t res, uint8_t gain) {
    check_valid_enh_res(res);
    check_valid_analog_pin(pin);
    if (__builtin_constant_p(gain)) {
      if (gain != 0 && gain != 1 && gain != 2 && gain != 4 && gain != 8 && gain != 16)
        badArg("The requested gain is not available on this part, accepted values are 0, 1, 2, 4, 8 and 16.");
    }
    return _analogReadEnh(pin, SINGLE_ENDED, res, gain);
  }

  inline __attribute__((always_inline)) int32_t analogReadDiff(uint8_t pos, uint8_t neg, uint8_t res, uint8_t gain) {
    check_valid_enh_res(res);
    check_valid_analog_pin(pos);
    check_valid_negative_pin(neg);
    if (__builtin_constant_p(gain)) {
      if (gain != 0 && gain != 1 && gain != 2 && gain != 4 && gain != 8 && gain != 16)
        badArg("The requested gain is not available on this part, accepted values are 0, 1, 2, 4, 8 and 16.");
    }
    return _analogReadEnh(pos, neg, res, gain);
  }

  static const int16_t adc_prescale_to_clkadc[0x11] =  {(F_CPU /  2000L),(F_CPU /  4000L),(F_CPU /  6000L),(F_CPU /  8000L),
  /* Doesn't get copied to ram because these all */     (F_CPU / 10000L),(F_CPU / 12000L),(F_CPU / 14000L),(F_CPU / 16000L),
  /* have fully memory mapped flash. Calculated  */     (F_CPU / 20000L),(F_CPU / 24000L),(F_CPU / 28000L),(F_CPU / 32000L),
  /* at compile time (we get to use division!)   */     (F_CPU / 40000L),(F_CPU / 48000L),(F_CPU / 56000L),(F_CPU / 64000L),1};


  /*
  Frequency in kHz.
  If options & 1 == 1, will set frequencies outside of safe operating range
  Otherwise, will be constrained to between 300 and 3000 (if internal reference used) or 300 and 6000 if not.
  Note: analogReference does NOT check this! So set the clock speed after reference if you want that guardrail.
  0 takes action, and -1 sets to default.
  */
  int16_t analogClockSpeed(int16_t frequency, uint8_t options) {
    if (frequency == -1) {
      frequency = 2750;
    }
    if (frequency > 0) {
      if ((options & 0x01) == 0) {
        frequency = constrain(frequency, 300, ((ADC0.CTRLC & 0x04) ? 3000 : 6000));
      }
      uint8_t prescale = 0;
      for (uint8_t i =0; i < 16; i++) {
        int16_t clkadc = adc_prescale_to_clkadc[i];
        prescale = i;
        if ((frequency >= clkadc) || /* i == 15 || */ (adc_prescale_to_clkadc[i+1] < ((options & 0x01) ? 2 : 300))) {
          ADC0.CTRLB = prescale;
          break;
        }
      }
    }
    if (frequency < 0) {
      return ADC_ERROR_INVALID_CLOCK;
    }
    return adc_prescale_to_clkadc[ADC0.CTRLB];
  }


/*---------------------------------------------------
 * END 2-series analogRead/analogReadXxxx functions
 *--------------------------------------------------*/
#else
  /*****************************************************
  START 0/1-series analogRead/analogReadXxxx functions
  *****************************************************/
    void ADCPowerOptions(uint8_t options) {
    if (__builtin_constant_p(options)) {
      if (options & 0x0F) {
        badArg("Only runstandby and enable/disable are supported - the hardware doesn't have LOWLAT nor the PGA");
      }
    }    // 0b SEE xxxx
    // SS = run standby
    // 00 = no change to run standby
    // 01 = no change to run standby
    // 10 = turn off run standby
    // 11 = turn on run standby
    // EE = ENABLE
    // 00 = Do not enable or disable ADC.
    // 01 = Do not enable or disable ADC.
    // 10 = Disable the ADC.
    // 11 = Enable the ADC.

    uint8_t temp = ADC0.CTRLA; //performance.
    if (options & 0x20) {
      if (options & 0x10) {
        temp |= 1; // ADC on
      } else {
        temp &= 0xFE; // ADC off
      }
    }
    if (options & 0x80) {
      if (options & 0x40) {
        temp |= 0x80; // run standby
      } else {
        temp &= 0x7F; // no run standby
      }
    }
    ADC0.CTRLA = temp; //now we apply enable and standby,
  }
  void analogReference(uint8_t mode) {
    check_valid_analog_ref(mode);
    switch (mode) {
      #if defined(EXTERNAL)
        case EXTERNAL:
      #endif
      case VDD:
        VREF.CTRLB &= ~VREF_ADC0REFEN_bm; // Turn off force-adc-reference-enable
        ADC0.CTRLC = (ADC0.CTRLC & ~(ADC_REFSEL_gm)) | mode | ADC_SAMPCAP_bm; // per datasheet, recommended SAMPCAP=1 at ref > 1v - we don't *KNOW* the external reference will be >1v, but it's probably more likely...
        // VREF.CTRLA does not need to be reconfigured, as the voltage references only supply their specified voltage when requested to do so by the ADC.
        break;
      case INTERNAL0V55:
        VREF.CTRLA =  VREF.CTRLA & ~(VREF_ADC0REFSEL_gm); // These bits are all 0 for 0.55v reference, so no need to do the mode << VREF_ADC0REFSEL_gp here;
        ADC0.CTRLC = (ADC0.CTRLC & ~(ADC_REFSEL_gm | ADC_SAMPCAP_bm)) | INTERNAL; // per datasheet, recommended SAMPCAP=0 at ref < 1v
        VREF.CTRLB |= VREF_ADC0REFEN_bm; // Turn off force-adc-reference-enable
        break;
      case INTERNAL1V1:
      case INTERNAL2V5:
      case INTERNAL4V34:
      case INTERNAL1V5:
        VREF.CTRLA = (VREF.CTRLA & ~(VREF_ADC0REFSEL_gm)) | (mode << VREF_ADC0REFSEL_gp);
        ADC0.CTRLC = (ADC0.CTRLC & ~(ADC_REFSEL_gm)) | INTERNAL | ADC_SAMPCAP_bm; // per datasheet, recommended SAMPCAP=1 at ref > 1v
        break;
    }
  }


  int analogRead(uint8_t pin) {
    check_valid_analog_pin(pin);

    if (pin < 0x80) {
      // If high bit set, it's a channel, otherwise it's a digital pin so we look it up..
      pin = digitalPinToAnalogInput(pin);
    }
    #if (PROGMEM_SIZE > 4096)
      // don't waste flash on smallest parts.
      if ((pin & 0x7F) > 0x1F) { // highest valid mux value for any 0 or 1-series part.
        return ADC_ERROR_BAD_PIN_OR_CHANNEL;
      }
    #endif
    if (!ADC0.CTRLA & 0x01) return ADC_ERROR_DISABLED;
    pin &= 0x1F;
    /* Reference should be already set up */
    /* Select channel */
    ADC0.MUXPOS = (pin << ADC_MUXPOS_gp);

    #if defined(STRICT_ERROR_CHECKING)
      if (ADC0.COMMAND) return ADC_ERROR_BUSY;
    #endif

    /* Start conversion */
    ADC0.COMMAND = ADC_STCONV_bm;

    /* Wait for result ready */
    while (!(ADC0.INTFLAGS & ADC_RESRDY_bm));

    /* Combine two bytes */
    return ADC0.RES;
  }


  inline __attribute__((always_inline)) void check_valid_duration(uint8_t samplen) {
    if (__builtin_constant_p(samplen)) {
      if (samplen > 31)
        badArg("Maximum SAMPLEN = 31 on these parts (33 ADC clocks)");
    }
  }

  bool analogSampleDuration(uint8_t dur) {
    check_valid_duration(dur);
    if (dur > 0x1F) {
      ADC0.SAMPCTRL = 0x1F;
      return false;
    } else {
      ADC0.SAMPCTRL = dur;
      return true;
    }
  }

  int32_t analogReadEnh(uint8_t pin, uint8_t res, uint8_t gain) {
    if (!(ADC0.CTRLA & 0x01)) return ADC_ENH_ERROR_DISABLED;
    check_valid_enh_res(res);
    check_valid_analog_pin(pin);
    if (__builtin_constant_p(gain)) {
      if (gain != 0)
        badArg("This part does not have an amplifier, gain argument must be omitted or given as 0");
    }
    uint8_t sampnum;
    if (res & 0x80) { // raw accumulation
      sampnum = res & 0x7F;
      if (sampnum > 6) return ADC_ENH_ERROR_RES_TOO_HIGH;
    } else {
      if (res < ADC_NATIVE_RESOLUTION_LOW) return ADC_ENH_ERROR_RES_TOO_LOW;
      if (res > 13) return ADC_ENH_ERROR_RES_TOO_HIGH;
      sampnum = (res > ADC_NATIVE_RESOLUTION ? ((res - ADC_NATIVE_RESOLUTION) << 1) : 0);
    }
    if (pin < 0x80) {
      // If high bit set, it's a channel, otherwise it's a digital pin so we look it up..
      pin = digitalPinToAnalogInput(pin);
    }
    #if (PROGMEM_SIZE > 4096)
      // don't waste flash on smallest parts.
      if ((pin & 0x7F) > 0x1F) { // highest valid mux value for any 0 or 1-series part.
        return ADC_ERROR_BAD_PIN_OR_CHANNEL;
      }
    #endif
    pin &= 0x1F;
    ADC0.MUXPOS = pin;
    #if defined(STRICT_ERROR_CHECKING) /* Strict error checking not yet implemented */
      if (ADC0.COMMAND) return ADC_ENH_ERROR_BUSY;
    #endif

    uint8_t _ctrla = ADC0.CTRLA;
    ADC0.CTRLA = ADC_ENABLE_bm | (res == ADC_NATIVE_RESOLUTION_LOW ? ADC_RESSEL_bm : 0);
    // if (res > 0x80) {
      ADC0.CTRLB = sampnum;
    /*} else
    if (res > ADC_NATIVE_RESOLUTION) {
      ADC0.CTRLB = 2 * (res - ADC_NATIVE_RESOLUTION);
    } else {
      ADC0.CTRLB = 0;
    }*/

    ADC0.COMMAND = ADC_STCONV_bm;
    while (!(ADC0.INTFLAGS & ADC_RESRDY_bm));
    int32_t result = ADC0.RES;
    if (res < 0x80 && res > ADC_NATIVE_RESOLUTION) {
      uint8_t shift = res - ADC_NATIVE_RESOLUTION - 1;
      while (shift) {
        result >>= 1;
        shift--;
      }
      uint8_t roundup = result & 0x01;
      result >>= 1;
      result += roundup;
    } else if (res == ADC_NATIVE_RESOLUTION - 1) { // 9 bit res?!
      result >>= 1;
    } // res > 0x80 (raw accumulate) or res == 8, res == 10 need no adjustment;
    ADC0.CTRLA = _ctrla;
    ADC0.CTRLB = 0;
    return result;
  }

  int32_t analogReadDiff(__attribute__ ((unused)) uint8_t pos, __attribute__ ((unused))uint8_t neg, __attribute__ ((unused))uint8_t res, __attribute__ ((unused))uint8_t gain) {
    badCall("This part does not have a differential ADC");
    return ADC_ENH_ERROR_NOT_DIFF_ADC;
  }
  int32_t analogReadDiff1(__attribute__ ((unused)) uint8_t pos, __attribute__ ((unused))uint8_t neg, __attribute__ ((unused))uint8_t res, __attribute__ ((unused))uint8_t gain) {
    badCall("This part does not have a differential ADC - and certainly not TWO OF THEM");
    return ADC_ENH_ERROR_NOT_DIFF_ADC;
  }
  static const int16_t adc_prescale_to_clkadc[0x09] =  {(F_CPU /  2000UL),(F_CPU /  4000UL),(F_CPU /  8000UL),(F_CPU / 16000UL),
  /* Doesn't get copied to ram because these all */     (F_CPU / 32000UL),(F_CPU / 64000UL),(F_CPU /128000UL),(F_CPU /256000UL),1};


  /*
  Frequency in kHz.
  If (options & 1) == 1, will set frequencies outside of safe operating range
  Otherwise, will be constrained to between 300 and 3000 (if internal reference used) or 300 and 6000 if not.
  Note: analogReference does NOT check this! So set the clock speed after reference if you want that guardrail.
  0 takes action, and -1 sets to default.
  */


  int16_t analogClockSpeed(int16_t frequency, uint8_t options) {
    if (frequency == -1) {
      frequency = 1450;
    }
    if (frequency > 0) {
      bool using_0v55 = !(VREF.CTRLA & VREF_ADC0REFSEL_gm || ADC0.CTRLC & ADC_REFSEL_gm);
      if ((options & 0x01) == 0) {
        frequency = constrain(frequency, (using_0v55 ? 100: 200), (using_0v55 ? 260 : 1500));
      }
      uint8_t prescale = 0;
      for (uint8_t i =0; i < 8; i++) {
        int16_t clkadc = adc_prescale_to_clkadc[i];
        prescale = i;
        if ((frequency >= clkadc) || (adc_prescale_to_clkadc[i + 1] < ((options & 0x01) ? 2 : (using_0v55 ? 100 : 200)))) {
          ADC0.CTRLC = (ADC0.CTRLC & ~ADC_PRESC_gm) | prescale;
          break;
        }
      }
    }
    if (frequency < 0) {
      return ADC_ERROR_INVALID_CLOCK;
    }
    return adc_prescale_to_clkadc[(ADC0.CTRLC & ADC_PRESC_gm)];
  }

#if defined(ADC1)
/****************************************************/
  // If we have two ADCs, while advise using only one, or controlling the second manually if advanced functionality is needed
  // if you insist, you can treat it identically by appending '1' to the names of the analog functions.
  void ADCPowerOptions1(uint8_t options) {
    if (__builtin_constant_p(options)) {
      if (options & 0x0F) {
        badArg("Only runstandby and enable/disable are supported - the hardware doesn't have LOWLAT nor the PGA");
      }
    }
    // 0b SSE xxxx
    // SS = run standby
    // 00 = no change to run standby
    // 01 = no change to run standby
    // 10 = turn off run standby
    // 11 = turn on run standby
    // EE = ENABLE
    // 00 = Do not enable or disable ADC.
    // 01 = Do not enable or disable ADC.
    // 10 = Disable the ADC.
    // 11 = Enable the ADC.

    uint8_t temp = ADC1.CTRLA; //performance.
    if (options & 0x20) {
      if (options & 0x10) {
        temp |= 1; // ADC on
      } else {
        temp &= 0xFE; // ADC off
      }
    }
    if (options & 0x80) {
      if (options & 0x40) {
        temp |= 0x80; // run standby
      } else {
        temp &= 0x7F; // no run standby
      }
    }
    ADC1.CTRLA = temp; //now we apply enable and standby
  }
  inline __attribute__((always_inline)) void check_valid_analog_pin1(pin_size_t pin) {
    if (__builtin_constant_p(pin)) {
      if (pin != ADC1_DAC1 && pin != ADC_INTREF && pin != ADC1_DAC2 && pin != ADC_GROUND) {
        // above cases cover valid internal sources.
        if (pin & 0x80) {
          if ((pin & 0x3F) >= NUM_ANALOG_INPUTS)
          {
            badArg("analogRead called with constant channel that is neither valid analog channel nor valid pin");
          }
        } else {
          pin = digitalPinToAnalogInput(pin);
          if (pin == NOT_A_PIN) {
            badArg("analogRead called with constant pin that is not a valid analog pin");
          }
        }
      }
    }
  }

  void analogReference1(uint8_t mode) {
    check_valid_analog_ref(mode);
    switch (mode) {
      #if defined(EXTERNAL)
        case EXTERNAL:
      #endif
      case VDD:
        VREF.CTRLB &= ~VREF_ADC1REFEN_bm; // Turn off force-adc-reference-enable
        ADC0.CTRLC = (ADC0.CTRLC & ~(ADC_REFSEL_gm)) | mode | ADC_SAMPCAP_bm; // per datasheet, recommended SAMPCAP=1 at ref > 1v - we don't *KNOW* the external reference will be >1v, but it's probably more likely...
        // VREF.CTRLA does not need to be reconfigured, as the voltage references only supply their specified voltage when requested to do so by the ADC.
        break;
      case INTERNAL0V55:
        VREF.CTRLC =  VREF.CTRLC & ~(VREF_ADC0REFSEL_gm); // These bits are all 0 for 0.55v reference, so no need to do the mode << VREF_ADC0REFSEL_gp here;
        ADC0.CTRLC = (ADC0.CTRLC & ~(ADC_REFSEL_gm | ADC_SAMPCAP_bm)) | INTERNAL; // per datasheet, recommended SAMPCAP=0 at ref < 1v
        VREF.CTRLB |= VREF_ADC0REFEN_bm; // Turn off force-adc-reference-enable
        break;
      case INTERNAL1V1:
      case INTERNAL2V5:
      case INTERNAL4V34:
      case INTERNAL1V5:
        VREF.CTRLC = (VREF.CTRLC & ~(VREF_ADC0REFSEL_gm)) | (mode << VREF_ADC0REFSEL_gp);
        ADC0.CTRLC = (ADC0.CTRLC & ~(ADC_REFSEL_gm)) | INTERNAL | ADC_SAMPCAP_bm; // per datasheet, recommended SAMPCAP=1 at ref > 1v
        break;
    }
  }
  // requires a bit of explanation - basically, the groupcodes are the same for ADC0 and ADC1, just different register.
  // VREF_ADC0REFSEL_gm == VREF_ADC1REFSEL_gm
  // I wouldn't say the dual ADC thing was some of their best work.

  int16_t analogClockSpeed1(int16_t frequency, uint8_t options) {
    if (frequency == -1) {
      frequency = 1450;
    }
    if (frequency > 0) {
      bool using_0v55 = !(VREF.CTRLA & VREF_ADC0REFSEL_gm || ADC1.CTRLC & ADC_REFSEL_gm);
      if ((options & 0x01) == 0) {
        frequency = constrain(frequency, (using_0v55 ? 100: 200), (using_0v55 ? 260 : 1500));
      }
      uint8_t prescale = 0;
      for (uint8_t i =0; i < 8; i++) {
        int16_t clkadc = adc_prescale_to_clkadc[i];
        prescale = i;
        if ((frequency >= clkadc) || (adc_prescale_to_clkadc[i + 1] < ((options & 0x01) ? 2 : (using_0v55 ? 100 : 200)))) {
          ADC1.CTRLC = (ADC1.CTRLC & ~ADC_PRESC_gm) | prescale;
          break;
        }
      }
    }
    if (frequency < 0) {
      return ADC_ERROR_INVALID_CLOCK;
    }
    return adc_prescale_to_clkadc[(ADC1.CTRLC & ADC_PRESC_gm)];
  }

  int16_t analogRead1(uint8_t pin) {
    check_valid_analog_pin1(pin);

    if (pin < 0x80) {
      // If high bit set, it's a channel, otherwise it's a digital pin so we look it up..
      pin = digitalPinToAnalogInput_ADC1(pin);
    }
    #if (PROGMEM_SIZE > 4096)
      // don't waste flash on smallest parts.
      if ((pin & 0x3F) > 0x1F) { // highest valid mux value for any 0 or 1-series part.
        return ADC_ERROR_BAD_PIN_OR_CHANNEL;
      }
    #endif
    if (!ADC1.CTRLA & 0x01) return ADC_ERROR_DISABLED;
    pin &= 0x1F;
    /* Reference should be already set up */
    /* Select channel */
    ADC1.MUXPOS = (pin << ADC_MUXPOS_gp);

    #if defined(STRICT_ERROR_CHECKING)
      if (ADC1.COMMAND) return ADC_ERROR_BUSY;
    #endif

    /* Start conversion */
    ADC1.COMMAND = ADC_STCONV_bm;

    /* Wait for result ready */
    while (!(ADC1.INTFLAGS & ADC_RESRDY_bm));

    /* Combine two bytes */
    return ADC1.RES;
  }


  bool analogSampleDuration1(uint8_t dur) {
    check_valid_duration(dur);
    if (dur > 0x1F) {
      ADC1.SAMPCTRL = 0x1F;
      return false;
    } else {
      ADC1.SAMPCTRL = dur;
      return true;
    }
  }

  int32_t analogReadEnh1(uint8_t pin, uint8_t res, uint8_t gain) {
    if (!(ADC1.CTRLA & 0x01)) return ADC_ENH_ERROR_DISABLED;
    check_valid_enh_res(res);
    check_valid_analog_pin(pin);
    if (__builtin_constant_p(gain)) {
      if (gain != 0)
        badArg("This part does not have an amplifier, gain argument must be omitted or given as 0");
    }
    uint8_t sampnum;
    if (res & 0x80) { // raw accumulation
      sampnum = res & 0x7F;
      if (sampnum > 6) return ADC_ENH_ERROR_RES_TOO_HIGH;
    } else {
      if (res < ADC_NATIVE_RESOLUTION_LOW) return ADC_ENH_ERROR_RES_TOO_LOW;
      if (res > 13) return ADC_ENH_ERROR_RES_TOO_HIGH;
      sampnum = (res > ADC_NATIVE_RESOLUTION ? ((res - ADC_NATIVE_RESOLUTION) << 1) : 0);
    }
    if (pin < 0x80) {
      // If high bit set, it's a channel, otherwise it's a digital pin so we look it up..
      pin = digitalPinToAnalogInput_ADC1(pin);
    }
    #if (PROGMEM_SIZE > 4096)
      // don't waste flash on smallest parts.
      if ((pin & 0x7F) > 0x1F) { // highest valid mux value for any 0 or 1-series part.
        return ADC_ERROR_BAD_PIN_OR_CHANNEL;
      }
    #endif
    pin &= 0x1F;
    ADC1.MUXPOS = pin;
    #if defined(STRICT_ERROR_CHECKING) /* Strict error checking not yet implemented */
      if (ADC1.COMMAND) return ADC_ENH_ERROR_BUSY;
    #endif

    uint8_t _ctrla = ADC1.CTRLA;
    ADC1.CTRLA = ADC_ENABLE_bm | (res == ADC_NATIVE_RESOLUTION_LOW ? ADC_RESSEL_bm : 0);
    // if (res > 0x80) {
      ADC1.CTRLB = sampnum;
    /*} else
    if (res > ADC_NATIVE_RESOLUTION) {
      ADC1.CTRLB = 2 * (res - ADC_NATIVE_RESOLUTION);
    } else {
      ADC1.CTRLB = 0;
    }*/

    ADC1.COMMAND = ADC_STCONV_bm;
    while (!(ADC1.INTFLAGS & ADC_RESRDY_bm));
    int32_t result = ADC1.RES;
    if (res < 0x80 && res > ADC_NATIVE_RESOLUTION) {
      uint8_t shift = res - ADC_NATIVE_RESOLUTION - 1;
      while (shift) {
        result >>= 1;
        shift--;
      }
      uint8_t roundup = result & 0x01;
      result >>= 1;
      result += roundup;
    } else if (res == ADC_NATIVE_RESOLUTION - 1) { // 9 bit res?!
      result >>= 1;
    } // res > 0x80 (raw accumulate) or res == 8, res == 10 need no adjustment;
    ADC1.CTRLA = _ctrla;
    ADC1.CTRLB = 0;
    return result;
  }
  #endif
  /*****************************************************
   END 0/1-series analogRead/analogReadXxxx functions
  *****************************************************/
#endif


// PWM output only works on the pins with
// hardware support.  These are defined in the variant
// pins_arduino.h file.  For the rest of the pins, we default
// to digital output.
void analogWrite(uint8_t pin, int val) {
  check_valid_digital_pin(pin);
  check_valid_duty_cycle(val);
  uint8_t bit_mask = digitalPinToBitMask(pin);
  if (bit_mask == NOT_A_PIN) {
    return;
  }
  // Set pin output because that's what Arduino does
  // Moved this way down to the end, why enable output before we have the pin doing the right thing?
  // pinMode(pin, OUTPUT);

  /* Get timer */
  /* megaTinyCore only - assumes only TIMERA0, TIMERD0, or DACOUT
   * can be returned here, all have only 1 bit set, so we can use
   * PeripheralControl as a mask to see if they have taken over
   * any timers with minimum overhead - critical on these parts
   * Since nothing that will show up here can have more than one
   * one bit set, binary and will give 0x00 if that bit is cleared
   * which is NOT_ON_TIMER.
   */
  uint8_t digital_pin_timer =  digitalPinToTimer(pin) & PeripheralControl;
  /* end megaTinyCore-specific section */

  volatile uint8_t *timer_cmp_out; // must be volatile for this to be safe.
  /* Find out Port and Pin to correctly handle port mux, and timer. */
  switch (digital_pin_timer) {
    case TIMERA0:
      if (val <= 0) { /* if zero or negative drive digital low */
        digitalWrite(pin, LOW);
      } else if (val >= 255) { /* if max or greater drive digital high */
        digitalWrite(pin, HIGH);
      } else {
        /* Calculate correct compare buffer register */
        #ifdef __AVR_ATtinyxy2__
        if (bit_mask == 0x80) {
          bit_mask = 1;  // on the xy2, WO0 is on PA7
        }
        #endif
        uint8_t offset = 0;
        if (bit_mask > 0x04) { // HCMP
          bit_mask <<= 1;      // mind the gap
          offset = 1;          // if it's an hcmp, the offset of the compare register is 1 higher.
        }
        if      (bit_mask & 0x44) offset += 4;
        else if (bit_mask & 0x22) offset += 2;
        timer_cmp_out = ((volatile uint8_t *)(&TCA0.SPLIT.LCMP0)) + (offset); //finally at the very end we get the actual pointer (since volatile variables should be treated like nuclear waste due to performance impact)
        (*timer_cmp_out) = (val); // write to it - and we're done with it.
        TCA0.SPLIT.CTRLB |= bit_mask;
      }
      break;
      // End of TCA case

  #if defined(DAC0)
    case DACOUT:
    {
      DAC0.DATA = val;
      DAC0.CTRLA = 0x41; // OUTEN=1, ENABLE=1
      break;
    }
  #endif
  #if (defined(TCD0) && defined(USE_TIMERD0_PWM))
    case TIMERD0:
      {
        //Glitches permitted: 0 or 255 will generate a glitch on the other channels and lose a tiny amount of time if used as millis timer. If you're doing that often enough though it adds up.
        #if !defined(NO_GLITCH_TIMERD0)
          if (val < 1) { /* if zero or negative drive digital low */
            digitalWrite(pin, LOW);
          } else if (val > 254) { /* if max or greater drive digital high */
            digitalWrite(pin, HIGH);
          } else {
        #else
        // Now, if NO_GLITCH_TIMERD0 is defined, val can legally be 0 or 255, which is to be interpreted as an instruction to keep the output constant LOW or HIGH.
        // 0 requires no special action - 255-0 = 255, we're counting to 254 and thus will never reach the compare matchvalue.
        // 255 on the other hand, requires us to invert the pin and set val to 0 to get the constant output. Setting the CMPxSET register to 0 produces a sub-system-clock spike
        //(maybe you don't care. but depending on the application, this could be catastrophic!)
          uint8_t set_inven = 0;
          if (val < 1) {
            val = 0;        // this will "just work", we'll set it to the maximum, it will never match, and will stay LOW
          } else if (val > 254) {
            val = 0;        // here we *also* set it to 0 so it would stay LOW
            set_inven = 1;  // but we invert the pin output with INVEN!
          }
        #endif
        // Calculation of values to write to CMPxSET
        // val is 1~254, so 255-val is 1~254.

        uint8_t oldSREG = SREG;
        cli(); // interrupts off... wouldn't due to have this mess interrupted and messed with...
        while ((TCD0.STATUS & (TCD_ENRDY_bm | TCD_CMDRDY_bm)) != (TCD_ENRDY_bm | TCD_CMDRDY_bm));
        // if previous sync/enable in progress, wait for it to finish. This is dicey for sure, because we're waiting on a peripheral
        // with interrupts off. But an interrupt could trigger one of those bits becoming unset, so we must do it this way.
        // set new values
        uint8_t fc_mask;
        if (bit_mask == 2) {  // PIN_PC1
          TCD0.CMPBSET = ((255 - val) << 1) - 1;
          fc_mask = 0x80;
        } else {        // PIN_PC0
          TCD0.CMPASET = ((255 - val) << 1) - 1;
          fc_mask = 0x40;
        }

        if (!(TCD0.FAULTCTRL & fc_mask)) {
          // if it's not active, we need to activate it... which produces a glitch in the PWM
          TCD0.CTRLA &= ~TCD_ENABLE_bm; // stop the timer
          _PROTECTED_WRITE(TCD0.FAULTCTRL, TCD0.FAULTCTRL | fc_mask);
          while (!(TCD0.STATUS & TCD_ENRDY_bm)); // wait until we can re-enable it
          TCD0.CTRLA |= TCD_ENABLE_bm; // re-enable it
        } else {
          TCD0.CTRLE = TCD_SYNCEOC_bm; // Synchronize
        }

        #if defined(NO_GLITCH_TIMERD0)
          // We only support control of the TCD0 PWM functionality on PIN_PC0 and PIN_PC1 (on 20 and 24 pin parts)
          // so if we're here, we're acting on either PC0 or PC1. And NO_GLITCH mode is enabled
          if (set_inven == 0) {
            // we are not setting invert to make the pin HIGH when not set; either was 0 (just set CMPxSET > CMPBCLR)
            // or somewhere in between.
            if (bit_mask == 1) {
              PORTC.PIN0CTRL &= ~(PORT_INVEN_bm);
            } else {
              PORTC.PIN1CTRL &= ~(PORT_INVEN_bm);
            }
          } else {
            // we *are* turning off PWM while forcing pin high - analogwrite(pin, 255) was called on TCD0 PWM pin...
            if (bit_mask == 1) {
              PORTC.PIN0CTRL |= PORT_INVEN_bm;
            } else {
              PORTC.PIN1CTRL |= PORT_INVEN_bm;
            }
          }
        #endif
        SREG = oldSREG;
      }
      break;
    #endif
    // end of TCD0 code

    /* If non timer pin, or unknown timer definition.  */
    /* do a digital write  */
    case NOT_ON_TIMER:
    default:
      if (val < 128) {
        digitalWrite(pin, LOW);
      } else {
        digitalWrite(pin, HIGH);
      }
      break;
  } // end of switch/case
  // Now that everything is said and done, we've set the pin high or low as if it's not a PWM pin, or told the timer to give it PWM if it is - this is a better timwe to finally turn on the output drivers.
  // True, it is at most 1-2 PWM timer ticks under typical settings, it's probably at least 1 tick, maybe several at 1 MHz (haven't timed analogWrite lately)
  pinMode(pin, OUTPUT);
} // end of analogWrite

void takeOverTCA0() {
  TCA0.SPLIT.CTRLA = 0;                                 // Stop TCA0
  PeripheralControl &= ~TIMERA0;                        // Mark timer as user controlled
  /* Okay, seriously? The datasheets and io headers disagree here */
  TCA0.SPLIT.CTRLESET = TCA_SPLIT_CMD_RESET_gc | 0x03;  // Reset TCA0
}

uint8_t digitalPinToTimerNow(uint8_t pin) {
  return digitalPinToTimer(pin) & PeripheralControl;
}

#if defined(TCD0)
void takeOverTCD0() {
  TCD0.CTRLA = 0;                     // Stop TCD0
  _PROTECTED_WRITE(TCD0.FAULTCTRL,0); // Turn off all outputs
  PeripheralControl &= ~TIMERD0;      // Mark timer as user controlled
}
#endif
