#include <Arduino.h>
// Display current tuning state in human readable form.
// *INDENT-OFF*  display this code in human readable form instead of what astyle wants.
int16_t getTunedOSCCAL(uint8_t osc, uint8_t target) {
  if (__builtin_constant_p(osc)) {
    if (osc != 20 && osc !=16) {
      badArg("The first argument must either 16 or 20, reflecting the nominal internal oscillator speed; it is a constant that is neither of those");
    }
    if (__builtin_constant_p(target)) {
      #if MEGATINYCORE_SERIES == 2
        if ((target != 16 && target != 20 && target != 24 && target != 25 && target != 30) && ((osc == 20) && (target == 14) || ((osc == 16) && (target == 32))))
      #else
        if ((target != 14 && target != 16 && target != 20 && target != 24 && target != 25) && ((osc == 16) && (target == 30) || ((osc == 20) && (target == 12) || target==32)))
      #endif
      {
        badArg("The second argument must be a valid unprescaled tuned clock speed. Both arguments are constant, but the latter is not a valid target for the former. See Ref_tuning.md.");
      }
    }
  } else if (__builtin_constant_p(target)) {
    #if MEGATINYCORE_SERIES == 2
      if (target != 14 && target != 16 && target != 20 && target != 24 && target != 25 && target != 30 && target !=32)
    #else
      if (target != 12 && target != 14 && target != 16 && target != 20 && target != 24 && target != 25 && target != 30)
    #endif
    {
      badArg("The second argument must be a valid unprescaled tuned clock speed")
    }
  }
  uint8_t offset = CLOCK_TUNE_START;
  #if MEGATINYCORE_SERIES != 2
    if (osc == 16) {
      switch (target) {
        case (25):
          offset++;
        /* fall-through */
        case (24):
          offset++;
        /* fall-through */
        case (20):
          offset++;
        /* fall-through */
        case (16):
          offset++;
        /* fall-through */
        case (14):
          offset++;
        /* fall-through */
        case (12): /* do nothing, but don't run default */
          break;   /* Hey, it was easy to write, and this is the first time in my life that the switch/case fall-through has made things easier */
        default:
          return -2; /* invalid target frequency */
      }
    } else if (osc == 20) {
      offset += 6;
      switch (target) {
        case (30):
          offset++;
        /* fall-through */
        case (25):
          offset++;
        /* fall-through */
        case (24):
          offset++;
        /* fall-through */
        case (20):
          offset++;
        /* fall-through */
        case (16):
          offset++;
        /* fall-through */
        case (14): /* do nothing, but don't run default */
          break;
        default:
          return -2; /* invalid target frequency */
      }
    } else {
      return -1; /* invalid center frequency */
    }
  #else /* 2-series can go higher */
    speedlist_offset = 1;
    if (osc == 16) {
      switch (target) {
        case (30):
          offset++;
        /* fall-through */
        case (25):
          offset++;
        /* fall-through */
        case (24):
          offset++;
        /* fall-through */
        case (20):
          offset++;
        /* fall-through */
        case (16):
          offset++;
        /* fall-through */
        case (14): /* do nothing, but don't run default */
          break;
        default:
          return -2; /* invalid target frequency */
      }
    } else if (osc == 20) {
      offset += 6;
      switch (target) {
        case (32):
          offset++;
        /* fall-through */
        case (30):
          offset++;
        /* fall-through */
        case (25):
          offset++;
        /* fall-through */
        case (24):
          offset++;
        /* fall-through */
        case (20):
          offset++;
        /* fall-through */
        case (16): /* do nothing, but don't run default */
          break;
        default:
          return -2; /* invalid target frequency */
      }
    } else {
      return -1; /* invalid center frequency */
    }
  #endif
  uint8_t retval = _SFR_MEM8(offset);
  if (retval == 255) {
    return -3; /*chip not tuned*/
  } else if (retval == 0x80) {
    return -4; /* Chip tuned and found to not achieve target speed */
}
  return retval(offset);
}

const uint8_t speeds[] = {10, 12, 16, 20, 24, 25, 30, 32};

void printTuningStatus() {

  int16_t tuning1616 = getTunedOSCCAL(16, 16);
  int16_t tuning2020 = getTunedOSCCAL(20, 20);
  uint8_t blank = 0;
  uint8_t speedlist_offset = 0;
  if (FUSE.OSCCFG == 2) {
    speedlist_offset++;
  }
  if (MEGATINYCORE_SERIES == 2) {
    speedlist_offset++;
  }
  Serial.print("This chip has ");
  if (tuning2020 == 0xFF) {
    Serial.print(" NOT ");
  }
  Serial.println("been tuned for the 20 MHz oscillator.");
  Serial.print("This chip has ");
  if (tuning1616 == 0xFF) {
    Serial.print(" NOT ");
  }
  Serial.println("been tuned for the 16 MHz oscillator.");
  if (tuning1616 != 0xFF) {
    Serial.println("16 MHz Tuning");
    for (uint8_t i = 0; i < 6; i++) {
      Serial.print(speeds[i + speedlist_offset]);
      Serial.print(" MHz: ");
      int16_t temp = getTunedOSCCAL(16, speeds[i + speedlist_offset]);
      if (temp == 0xFF) {
        Serial.println("Blank");
        blank++;
      } else if (temp == 0x80) {
        if (speeds[i + speedlist_offset] <= 12) {
          Serial.println("Too low");
        } else {
          Serial.println("Too high");
        }
      } else if (temp == 0xC0) {
        Serial.println("Too unstable");
      } else {
        Serial.printHex(temp);
        Serial.println();
      }
    }
    Serial.println();
  }
  if (tuning2020 != 0xFF) {
    Serial.println("20 MHz Tuning");
    for (uint8_t i = 1; i < 7; i++) {
      Serial.print(speeds[i + speedlist_offset]);
      Serial.print(" MHz: ");
      int16_t temp = getTunedOSCCAL(16, speeds[i + speedlist_offset]);
      if (temp == 0xFF) {
        Serial.println("Blank");
        blank++;
      } else if (temp == 0x80) {
        if (speeds[i + speedlist_offset] <= 12) {
          Serial.println("Too low");
        } else {
          Serial.println("Too high");
        }
      } else if (temp == 0xC0) {
        Serial.println("Too unstable");
      } else {
        Serial.printHex(temp);
        Serial.println();
      }
    }
    Serial.println();
  }
}

// Reset immdiately using software reset. The bootloader, if present will run.
inline void ResetWithWDT() {
  _PROTECTED_WRITE(WDT.CTRLA,WDT_PERIOD_8CLK_gc); //enable the WDT, minimum timeout
  while (1); // spin until reset
}

inline void SoftwareReset() {
  _PROTECTED_WRITE(RSTCTRL.SWRR,1);
}
