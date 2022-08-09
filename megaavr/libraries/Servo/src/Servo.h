/*
  Servo.h - Interrupt driven Servo library for Arduino using 16 bit timers- Version 2
  Copyright (c) 2009 Michael Margolis, and modified, extended, hacked up
  butchered and otherwise mangled by countless others over the years,
  most recently by Spence Konde 2019 ~ 2021 to provide reliable behavior on the
  tinyAVR 0/1/2-series, megaAVR 0-series, and AVR Dx-series.
  https://github.com/SpenceKonde

  This library is included with megaTinyCore and DxCore.

  This version was designed for and will be included with:
  megaTinyCore 2.6.0+
  DxCore 1.5.0+
*/

/*  OBLIGATORY LEGAL BOILERPLATE

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/


/*
  A servo is activated by creating an instance of the Servo class passing
  the desired pin to the attach() method.
  The servos are pulsed in the background using the value most recently
  written using the write() method.



  The methods are:

    Servo - Class for manipulating servo motors connected to Arduino pins.

    attach(pin)           - Attaches a servo motor to an i/o pin.
    attach(pin, min, max) - Attaches to a pin setting min and max values in microseconds
    default min is 544, max is 2400
    write()               - Sets the servo angle in degrees.  (invalid angle that is valid as pulse in microseconds is treated as microseconds)
    writeMicroseconds()   - Sets the servo pulse width in microseconds
    read()                - Gets the last written servo pulse width as an angle between 0 and 180.
    readMicroseconds()    - Gets the last written servo pulse width in microseconds. (was read_us() in first release)
    attached()            - Returns true if there is a servo attached.
    detach()              - Stops an attached servos from pulsing its i/o pin.


  This library supports 12 servos controlled by one timer.
  It does not recruit additional timers.

  Who the hell runs over a dozen servos from one board? Complain in the github
  issues for the core this was packaged with if this is actually a problem for you...
  I don't expect to hear from anyone. -Spence, Jan 2021
 */

#ifndef Servo_h
#define Servo_h
#include <inttypes.h>
#include <core_devices.h> // They renamed about a thousand _bm and _bp defines in latest atpacks, this file (included automatically via Arduino.h for sketches)
// ensures compatibility with both toolchain versions without the header having to include the whole Arduino.h). About 4000 lines of fixes, generatede with
// find-all, a manual read-through, and then regex replacement to add the workaround). That is, I did what they should have done.
#if (!defined(TCB_CLKSEL_2_bm))
  // This means it's a tinyAVR 0/1-series, or a megaAVR 0-series.
  // Their TCB_CLKSEL enums use different names for the clock settings, for reasons unclear.
  // To align with the future, we use the Dx-series names for these.
  #define TCB_CLKSEL_DIV2_gc TCB_CLKSEL_CLKDIV2_gc
  #define TCB_CLKSEL_DIV1_gc TCB_CLKSEL_CLKDIV1_gc
#endif
#if defined(ARDUINO_ARCH_MEGAAVR)
  #include "megaavr/ServoTimers.h"
  #if (F_CPU == 1000000)
    #warning "Running at 1MHz results in unstable servo signal."
  #endif
#else
  #error "This is an architecture specific library for ARDUINO_ARCH_MEGAAVR, but this device is not of that architecture"
#endif

// *INDENT-OFF*
#define Servo_VERSION              2     // software version of this library
#define MIN_PULSE_WIDTH          544     // the shortest pulse sent to a servo
#define MAX_PULSE_WIDTH         2400     // the longest pulse sent to a servo
#define DEFAULT_PULSE_WIDTH     1500     // default pulse width when servo is attached
#define REFRESH_INTERVAL       20000UL   // minumim time to refresh servos in microseconds - UL is super-important!
#define SERVOS_PER_TIMER          12     // the maximum number of servos controlled by one timer
#define INVALID_SERVO            255     // flag indicating an invalid servo index
// *INDENT-ON*
#define MAX_SERVOS (_Nbr_16timers  * SERVOS_PER_TIMER)


typedef struct  {       // port & bitmask used instead of pin number to realize dramatic performance boost
  uint8_t isActive;    // true if this channel is enabled, pin not pulsed if false
  uint8_t port;         // port number (A=0, B=1, and so on)
  uint8_t bitmask;      // port & bitmask used instead of pin number to realize dramatic performance boost
} ServoPin_t   ;

typedef struct {
  ServoPin_t Pin;
  volatile unsigned int ticks;
} servo_t;

class Servo {
  public:
    Servo();
    uint8_t attach(byte pin);                   // attach the given pin to the next free channel, sets pinMode, returns channel number or 0 if failure
    uint8_t attach(byte pin, int min, int max); // as above but also sets min and max values for writes.
    void detach();
    void write(unsigned int value);             // if value is < 200 its treated as an angle, otherwise as pulse width in microseconds
    void writeMicroseconds(unsigned int value); // Write pulse width in microseconds
    int read();                                 // returns current pulse width as an angle between 0 and 180 degrees
    unsigned int readMicroseconds();            // returns current pulse width in microseconds for this servo (was read_us() in first release)
    bool attached();                            // return true if this servo is attached, otherwise false
  private:
    uint8_t servoIndex;                         // index into the channel data for this servo
    int8_t min;                                 // minimum is this value times 4 added to MIN_PULSE_WIDTH
    int8_t max;                                 // maximum is this value times 4 added to MAX_PULSE_WIDTH
};

#endif
