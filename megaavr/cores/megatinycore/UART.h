// We are returning to the conventional name and there is no longer a bloat inducing class layer between

#ifndef HWSERIALNAME_HACK_H
  #define HWSERIALNAME_HACK_H
  #include <HardwareSerial.h>
  #define UartClass HardwareSerial
#endif
