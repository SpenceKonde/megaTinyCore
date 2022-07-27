#include <Arduino.h>
#include <HardwareSerial.h>
union __BW {
  int16_t si;
  int8_t sb[2];
  uint8_t b[2];
};

inline int8_t analogCheckError(int16_t adcval) {
  union __BW bw;
  bw.si = adcval;
  if (bw.b[1] == 0x82) {
    return bw.sb[0]; //returns error codes as negative numbers
  } else {
    return 0; //0 = not error
  }
}

union __BL {
  int32_t sl;
  uint8_t b[4];
  int8_t sb[4];
};

inline int8_t analogCheckError(int32_t adcval) {
  union __BL bl;
  bl.sl = adcval;
  if (bl.b[3] == 82) { // all errors start with that
    return (0 - bl.sb[0]);
  } else if ((bl.b[3] + 1) > 1 || ((bl.b[2]) + 0x20) > 0x5F) { // returning raw maximum code point accumulated 1024 times can at most yield 0x03 FF FF in single ended and minimum, 0xFFFC0000 in differential mode.
    // and if we add 20 to the third byte, it should be no larger than 0x5F, otherwise there is a bug in your code corrupting stuff, or a bug in the core
    return -127;//
  }
  return 0; //not error
}
// *INDENT-OFF*
bool printADCRuntimeError(int32_t adcval, HardwareSerial &__dbgser) {
  union __BL bl;
  bl.sl = adcval;
  if (bl.b[3] == 0x82) {
    switch (bl.b[0]) {
      case 0xFF:
        __dbgser.println(F("pin or channel does not exist on this part")); break;
      case 0xFE:
        __dbgser.println(F("ADC is already doing a conversion")); break;
      case 0xFD:
        __dbgser.println(F("Requested res < 8, minimum us 8-bit")); break;
      case 0xFC:
        __dbgser.println(F("Requested res is higher than a single burst can achieve.")); break;
#if defined(PGACTRL)
      case 0xFB:
        __dbgser.println(F("negative pin or channel does not exist or is not a valid")); break;
#endif
      case 0xF9:
        __dbgser.println(F("The ADC has been disabled, re-enable it with ADCPowerOptions.")); break;
      default: {
        __dbgser.println(F("Error code not recognized"));
      }
    }
    return false;
  } else if ((bl.b[3] + 1) > 1 || ((bl.b[2]) + 0x20) > 0x5F) {
    __dbgser.println(F("Impossible value - outside maximum range of analogReadEnh"));
    return false;
  }
  return true;
}
bool printADCRuntimeError(int16_t error, HardwareSerial &__dbgser) {
  union __BW bw;
  bw.si = error;
  if (bw.b[1] == 0x82) {
    switch (bw.b[0]) {
    case 0xFE:
      __dbgser.println("ADC is already doing a conversion"); break;
    case 0xFF:
      __dbgser.println("pin or channel does not exist on this part"); break;
    case 0xF9:
      __dbgser.println("The ADC has been disabled, re-enable it with ADCPowerOptions."); break;
    default: {
      __dbgser.println("Error code not recognized");
    }
    return false;
  } else if (bw.b[1] > 0x10) {
    __dbgser.println(F("Impossible value - outside maximum range of analogRead()"));
    return false;
  }
  return true;
}
// *INDENT-On*
