#ifndef __CORE_PARAMETERS_H__
#define __CORE_PARAMETERS_H__

#if !(defined(MEGATINYCORE) || defined(DXCORE) || defined(ATTIYNCORE))

  // This define can get black-hole'ed somehow (reported on platformio) likely the ugly syntax to pass a string define from platform.txt via a -D
  // directive passed to the compiler is getting mangled somehow, though I'm amazed it doesn't cause a  compile error. But checking for defined(MEGATINYCORE)
  // is the documented method to detect that megaTinyCore is in use, and without it things that tried to do conditional compilation based on that were not
  // recognizing it as megaTinyCore and hence would fail to compile when that conditional compilation was required to make it build.
  // From: https://github.com/adafruit/Adafruit_BusIO/issues/43

  // The whole purpose of this file is largely for for the purpose of being something that can be included anywhere to make sure we know what core we are
  // which becomes more and more important as more code is shared between the cores.

  #define MEGATINYCORE "Unknown 2.6.0+"
  #if !defined(MEGATINYCORE_NUM)
    #if !(defined(MEGATINYCORE_MAJOR) && defined(MEGATINYCORE_MINOR) && defined(MEGATINYCORE_PATCH) && defined(MEGATINYCORE_RELEASED))
      #warning "All of the version defines are missing, please correct your build environment; it is likely failing to define other critical values"
      // Version related defines now handled in platform.txt
    #else
      #define MEGATINYCORE_NUM ((MEGATINYCORE_MAJOR << 24) + (MEGATINYCORE_MINOR << 16) + (MEGATINYCORE_PATCH << 8) + MEGATINYCORE_RELEASED)
    #endif
  #endif
#else
  #if defined(DXCORE) || defined(ATTINYCORE)
    #error "This is megaTinyCore being compiled, but DXCORE and/or ATTINYCORE is defined already"
  #endif
#endif
#endif
