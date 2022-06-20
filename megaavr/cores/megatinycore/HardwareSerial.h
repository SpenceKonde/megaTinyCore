// Many cores use HardwareSerial.h and HardwareSerial. However, the core from which this was derived names them differently.
// This only rarely a problem - but it causes problems when libraries want to #include HardwareSerial.h and expect it to provide a class of that name, instead of UartClass.
// This should make it transparent - however not all modern AVR cores have this file

#ifndef HWSERIALNAME_HACK_H
  #define HWSERIALNAME_HACK_H
  #include <UART.h>
  #define HardwareSerial UartClass
#endif
