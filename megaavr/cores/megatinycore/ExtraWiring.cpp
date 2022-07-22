#include "Arduino.h"
#include "HardwareSerial.h"

inline uint8_t analogIsError(int16_t adcval) {
  if(adcval < 0) {
    return 1;
  }
  return 0;
}
inline uint8_t analogIsError(int32_t adcval) {
  union bytelong {
    int32_t l;
    uint8_t[4] b;
  }
  bytelong.l = adcval;
  if (bytelong.b[3] == 0x82) {
    return 1;
  }
  return 0;
}
bool printADCRuntimeError(int32_t error, HardwareSerial &__dbgser) {
  if (error < -2100000000) {
    uint8_t err = (uint8_t) (-2100000000 - error) ;
    switch (err) {
      case 0:
        __dbgser.println("pin or channel does not exist on this part"); break;
      case 1:
        __dbgser.println("ADC is already doing a conversion"); break;
      case 3:
        __dbgser.println("Requested res < 8, minimum us 8-bit"); break;
      case 4:
        __dbgser.println("Requested res is higher than a single burst can achieve."); break;
      #if defined(PGACTRL)
      case 5:
        __dbgser.println("negative pin or channel does not exist or is not a valid"); break;
      #endif
      case 6:
        __dbgser.println("The ADC has been disabled, re-enable it with ADCPowerOptions."); break;
      default: {
        __dbgser.println("Error code not recognized");
      }
      return false;
    }
  } else {
    if (error < -32760) {
      uint8_t err = (uint8_t) (-32760 - error);
      switch (err) {
      case 4:
        __dbgser.println("Invalid ADC clock selected"); break;
      case 6:
        __dbgser.println("ADC is already doing a conversion"); break;
      case 5:
        __dbgser.println("pin or channel does not exist on this part"); break;
      case 7:
        __dbgser.println("The ADC has been disabled, re-enable it with ADCPowerOptions."); break;
      default: {
        __dbgser.println("Error code not recognized");
      }
      return false;
    }
  }
}
return true;
}
