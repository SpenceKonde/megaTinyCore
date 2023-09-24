#ifndef __CORE_PARAMETERS_H__
#define __CORE_PARAMETERS_H__

#if !defined(MEGATINYCORE_NUM)
  #if !(defined(MEGATINYCORE_MAJOR) && defined(MEGATINYCORE_MINOR) && defined(MEGATINYCORE_PATCH) && defined(MEGATINYCORE_RELEASED))
    #warning "All of the version defines are missing, please correct your build environment; it is likely failing to define other critical values"
    // Version related defines now handled in platform.txt
  #else
    #define MEGATINYCORE_NUM ((MEGATINYCORE_MAJOR << 24) + (MEGATINYCORE_MINOR << 16) + (MEGATINYCORE_PATCH << 8) + MEGATINYCORE_RELEASED)
  #endif
#endif

#if !(defined(MEGATINYCORE) || defined(DXCORE) || defined(ATTIYNCORE))

  // This define can get black-hole'ed somehow (reported on platformio) likely the ugly syntax to pass a string define from platform.txt via a -D
  // directive passed to the compiler is getting mangled somehow, though I'm amazed it doesn't cause a  compile error. But checking for defined(MEGATINYCORE)
  // is the documented method to detect that megaTinyCore is in use, and without it things that tried to do conditional compilation based on that were not
  // recognizing it as megaTinyCore and hence would fail to compile when that conditional compilation was required to make it build.
  // From: https://github.com/adafruit/Adafruit_BusIO/issues/43

  // The whole purpose of this file is largely for for the purpose of being something that can be included anywhere to make sure we know what core we are
  // which becomes more and more important as more code is shared between the cores.

  #define MEGATINYCORE "Unknown 2.6.9+"
#else
  #if defined(DXCORE) || defined(ATTINYCORE)
    #error "This is megaTinyCore being compiled, but DXCORE and/or ATTINYCORE is defined already, something is way wrong."
  #endif
#endif


/* Check for invalid options, mostly to help point the PIO and other third party IDE users in the right direction */

#if defined(MILLIS_USE_TIMERA0) && (defined(MILLIS_USE_TIMERNONE) || \
    defined(MILLIS_USE_TIMERB0) || defined(MILLIS_USE_TIMERB1) || defined(MILLIS_USE_TIMERB2) || defined(MILLIS_USE_TIMERB3) || \
    defined(MILLIS_USE_TIMERA1) || defined(MILLIS_USE_TIMERD0) || defined(MILLIS_USE_TIMERRTC) || defined(MILLIS_USE_TIMERRTC_XTAL))
  #error "MILLIS_USE_TIMERA0 and another timer are set as the millis timer. Specify one only. If this occurred on the Arduino IDE, please report it promptly."
#endif
#if defined(MILLIS_USE_TIMERA1) && (defined(MILLIS_USE_TIMERNONE) || \
    defined(MILLIS_USE_TIMERB0) || defined(MILLIS_USE_TIMERB1) || defined(MILLIS_USE_TIMERB2) || defined(MILLIS_USE_TIMERB3) || \
    defined(MILLIS_USE_TIMERA0) || defined(MILLIS_USE_TIMERD0) || defined(MILLIS_USE_TIMERRTC) || defined(MILLIS_USE_TIMERRTC_XTAL))
  #error "MILLIS_USE_TIMERA1 and another timer are set as the millis timer. Specify one only. If this occurred on the Arduino IDE, please report it promptly."
#endif
#if defined(MILLIS_USE_TIMERB0) && (defined(MILLIS_USE_TIMERNONE) || \
    defined(MILLIS_USE_TIMERA0) || defined(MILLIS_USE_TIMERB1) || defined(MILLIS_USE_TIMERB2) || defined(MILLIS_USE_TIMERB3) || \
    defined(MILLIS_USE_TIMERA1) || defined(MILLIS_USE_TIMERD0) || defined(MILLIS_USE_TIMERRTC) || defined(MILLIS_USE_TIMERRTC_XTAL))
  #error "MILLIS_USE_TIMERB0 and another timer are set as the millis timer. Specify one only. If this occurred on the Arduino IDE, please report it promptly."
#endif
#if defined(MILLIS_USE_TIMERB1) && (defined(MILLIS_USE_TIMERNONE) || \
    defined(MILLIS_USE_TIMERB0) || defined(MILLIS_USE_TIMERA0) || defined(MILLIS_USE_TIMERB2) || defined(MILLIS_USE_TIMERB3) || \
    defined(MILLIS_USE_TIMERA1) || defined(MILLIS_USE_TIMERD0) || defined(MILLIS_USE_TIMERRTC) || defined(MILLIS_USE_TIMERRTC_XTAL))
  #error "MILLIS_USE_TIMERB1 and another timer are set as the millis timer. Specify one only. If this occurred on the Arduino IDE, please report it promptly."
#endif
#if defined(MILLIS_USE_TIMERB2) && (defined(MILLIS_USE_TIMERNONE) || \
    defined(MILLIS_USE_TIMERB0) || defined(MILLIS_USE_TIMERB1) || defined(MILLIS_USE_TIMERA0) || defined(MILLIS_USE_TIMERB3) || \
    defined(MILLIS_USE_TIMERA1) || defined(MILLIS_USE_TIMERD0) || defined(MILLIS_USE_TIMERRTC) || defined(MILLIS_USE_TIMERRTC_XTAL))
  #error "MILLIS_USE_TIMERB2 and another timer are set as the millis timer. Specify one only. If this occurred on the Arduino IDE, please report it promptly."
#endif
#if defined(MILLIS_USE_TIMERB3) && (defined(MILLIS_USE_TIMERNONE) || \
    defined(MILLIS_USE_TIMERB0) || defined(MILLIS_USE_TIMERB1) || defined(MILLIS_USE_TIMERB2) || defined(MILLIS_USE_TIMERA0) || \
    defined(MILLIS_USE_TIMERA1) || defined(MILLIS_USE_TIMERD0) || defined(MILLIS_USE_TIMERRTC) || defined(MILLIS_USE_TIMERRTC_XTAL))
  #error "MILLIS_USE_TIMERB3 and another timer are set as the millis timer. Specify one only. If this occurred on the Arduino IDE, please report it promptly."
#endif
#if defined(MILLIS_USE_TIMERD0) && (defined(MILLIS_USE_TIMERNONE) || \
    defined(MILLIS_USE_TIMERB0) || defined(MILLIS_USE_TIMERB1) || defined(MILLIS_USE_TIMERB2) || defined(MILLIS_USE_TIMERB3) || \
    defined(MILLIS_USE_TIMERA1) || defined(MILLIS_USE_TIMERA0) || defined(MILLIS_USE_TIMERRTC) || defined(MILLIS_USE_TIMERRTC_XTAL))
  #error "MILLIS_USE_TIMERB5 and another timer are set as the millis timer. Specify one only. If this occurred on the Arduino IDE, please report it promptly."
#endif
#if defined(MILLIS_USE_TIMERRTC) && (defined(MILLIS_USE_TIMERNONE) || \
    defined(MILLIS_USE_TIMERB0) || defined(MILLIS_USE_TIMERB1) || defined(MILLIS_USE_TIMERB2) || defined(MILLIS_USE_TIMERB3) || \
    defined(MILLIS_USE_TIMERA1) || defined(MILLIS_USE_TIMERD0) || defined(MILLIS_USE_TIMERA0))
  #error "MILLIS_USE_TIMERE0 and another timer are set as the millis timer. Specify one only. If this occurred on the Arduino IDE, please report it promptly."
#endif
#if defined(MILLIS_USE_TIMERRTC_XTAL) && (defined(MILLIS_USE_TIMERNONE) || \
    defined(MILLIS_USE_TIMERB0) || defined(MILLIS_USE_TIMERB1) || defined(MILLIS_USE_TIMERB2) || defined(MILLIS_USE_TIMERB3) || \
    defined(MILLIS_USE_TIMERA1) || defined(MILLIS_USE_TIMERD0) || defined(MILLIS_USE_TIMERA0))
  #error "MILLIS_USE_TIMERF0 and another timer are set as the millis timer. Specify one only. If this occurred on the Arduino IDE, please report it promptly."
#endif
#if defined(MILLIS_USE_TIMERE0) || defined(MILLIS_USE_TIMERF0)
  #error "A millis timer not supported on this core was passed. this should only be possible on third party IDEs. "
#endif
#if defined(UARTBAUD5V) || defined(UARTBAUD3V)
  #warning "The UARTBAUDxV baud rate correction was removed in 2.3.0 because it had excessive overhead and negligible benefit. The define you specified does nothing."
#endif
#endif

#endif
