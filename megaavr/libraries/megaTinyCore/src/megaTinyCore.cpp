#include <Arduino.h>
/* Helper tables used by bitMaskToPosition() and bitMaskToPositionNybble()
 * because this is such a common oppperation that it is sometimes worth burning 144 bytes of flash for a LUT (depends how fast you need
 * your answer vs how much flash you have to spare) */

// *INDENT-OFF* // turn off astyle
/**/                       /* 1's digit   0    1    2    3    4    5    6    7    8   9    10   11   12   13   14   15*/
const uint8_t bit_mask_to_position[] = {255,   0,   1, 255,   2, 255, 255, 255,   3, 255, 255, 255, 255, 255, 255, 255, /* 16's digit 0 */ \
/**/                                      4, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, /* 16's digit 1 */ \
/**/                                      5, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, /* 16's digit 2 */ \
/**/                                    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, /* 16's digit 3 */ \
/**/                                      6, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, /* 16's digit 4 */ \
/**/                                    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, /* 16's digit 5 */ \
/**/                                    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, /* 16's digit 6 */ \
/**/                                    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, /* 16's digit 7 */ \
/**/                                      7}; /* Numbers exceeding 128 are not bitmasks, and the function that references this tests for that */

const uint8_t bit_mask_to_position_n[] = {255,  0,  1, 255,   2, 255, 255, 255,   3, 255, 255, 255, 255, 255, 255, 255}; //



// *INDENT-OFF*  display this code in human readable form instead of what astyle wants.
uint16_t readTemp() {
  #if MEGATINYCORE_SERIES!=2 /* 0 and 1-series both have the same ADC. */
    // based on the datasheet, in section 30.3.2.5 Temperature Measurement, with added code to restore ADC config.
    int8_t sigrow_offset = SIGROW.TEMPSENSE1; // Read signed value from signature row
    uint8_t sigrow_gain = SIGROW.TEMPSENSE0; // Read unsigned value from signature row
    analogReference(INTERNAL1V1);
    uint8_t sampctrl = ADC0.SAMPCTRL;
    uint8_t ctrld = ADC0.CTRLD;
    ADC0.SAMPCTRL = 0x1F; // maximum length sampling
    ADC0.CTRLD &= ~(ADC_INITDLY_gm);
    ADC0.CTRLD |= ADC_INITDLY_DLY32_gc; // wait 32 ADC clocks before reading new reference
    uint16_t adc_reading = analogRead(ADC_TEMPERATURE); // ADC conversion result with 1.1 V internal reference
    analogReference(VDD);
    ADC0.SAMPCTRL = sampctrl;
    ADC0.CTRLD = ctrld;
    uint32_t temp = adc_reading - sigrow_offset;
    temp *= sigrow_gain; // Result might overflow 16 bit variable (10bit+8bit)
    temp += 0x80; // Add 1/2 to get correct rounding on division below
    temp >>= 8; // Divide result to get Kelvin
  return temp;
  #else /* 2-series has a very different ADC */
    int8_t  sigrowOffset  = SIGROW.TEMPSENSE1;
    uint8_t sigrowGain    = SIGROW.TEMPSENSE0;
    uint8_t analogref     = getAnalogReference();
    uint8_t sampctrl      = ADC0.CTRLE;
    analogSampleDuration(128); // must be >= 32µs * f_CLK_ADC per datasheet 30.3.3.7
    analogReference(INTERNAL1V024);
    uint32_t reading      = analogRead(ADC_TEMPERATURE);
    analogReference(analogref);
    ADC0.CTRLE            = sampctrl; //restore previous configuration.
    reading              -= sigrowOffset;
    reading              *= sigrowGain;
    reading              += 0x80; // Add 1/2 to get correct rounding on division below
    reading             >>= 8; // Divide result to get Kelvin
    return reading;
  #endif
}

uint16_t readSupplyVoltage() { // returns value in millivolts to avoid floating point
  #if MEGATINYCORE_SERIES!=2
    analogReference(VDD);
    VREF.CTRLA = VREF_ADC0REFSEL_1V5_gc;
    // there is a settling time between when reference is turned on, and when it becomes valid.
    // since the reference is normally turned on only when it is requested, this virtually guarantees
    // that the first reading will be garbage; subsequent readings taken immediately after will be fine.
    // VREF.CTRLB|=VREF_ADC0REFEN_bm;
    // delay(10);
    uint16_t reading = analogRead(ADC_INTREF); // Crumple the reading up into a ball and toss in the general direction of the trash.
    reading = analogRead(ADC_INTREF);          // Now we take the *real* reading.
    uint32_t intermediate = 1023UL * 1500;     // This would overflow a 16-bit variable.
    reading = intermediate / reading;          // Long division sucks! This single line takes about 600 clocks to execute!
    // And **this** is why the reference voltages on the 0/1-series parts suck! The comparable math on the 2-series takes maybe
    // 40 clocks, at most!
    return reading;
  #else
    int16_t returnval = 0;
    analogReference(INTERNAL1V024);
    analogReadEnh(ADC_VDDDIV10, 12); // take a reading, crumple, toss, take another reading and use that.
    int32_t vddmeasure = analogReadEnh(ADC_VDDDIV10, 12); // Take it at 12 bits
    uint16_t intermediate = (uint16_t) vddmeasure;
    //vddmeasure *= 10; // since we measured 1/10th VDD, then divide by 4 to get into millivolts NO! Don't do that! This way takes 81 clocks for the multiply,
    // then 8 more to shift it.
    if (intermediate & 0x01) {
      // if last digit was 1 we should round up
      returnval++; // Let's do that here, that way the vddmeasure variable will be dead after the line after the end of this block.
    }
    returnval += (intermediate << 1) + (intermediate >> 1); // The net effect of *=10 followed by >> 2 is multiplication by 2.5.
    // And why in the bloody hell were we doing the math in a 32 bit types anyway? I know this isn't time critical but that's no
    // excuse for being a damned moron about how we go about this.
    return returnval;
  #endif
}

/* Now we get into the ugly clock stuff */
// Display current tuning state in human readable form.
int16_t getTunedOSCCAL(uint8_t osc, uint8_t target) {
  if (__builtin_constant_p(osc)) {
    if (osc != 20 && osc !=16) {
      badArg("The first argument must either 16 or 20, reflecting the nominal internal oscillator speed; it is a constant that is neither of those");
    }
    if (__builtin_constant_p(target)) {
      #if MEGATINYCORE_SERIES == 2
        if ((target != 16 && target != 20 && target != 24 && target != 25 && target != 30) || (((osc == 20) && (target == 14)) || ((osc == 16) && (target == 32))))
      #else
        if ((target != 14 && target != 16 && target != 20 && target != 24 && target != 25) || (((osc == 16) && (target == 30)) || ((osc == 20) && (target == 12))) || target==32)
      #endif
      {
        badArg("The second argument must be a valid unprescaled tuned clock speed. Both arguments are constant, but the latter is not a valid target for the former. See Ref_tuning.md.");
      }
    }
  } else if (__builtin_constant_p(target)) {
    #if MEGATINYCORE_SERIES == 2
      if (target != 14 && target != 16 && target != 20 && target != 24 && target != 25 && target != 30 && target != 32)
    #else
      if (target != 12 && target != 14 && target != 16 && target != 20 && target != 24 && target != 25 && target != 30)
    #endif
    {
      badArg("The second argument must be a valid unprescaled tuned clock speed");
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
  return retval;
}

const uint8_t speeds[] = {10, 12, 16, 20, 24, 25, 30, 32};

void printTuningStatus() {

  int16_t tuning1616 = getTunedOSCCAL(16, 16);
  int16_t tuning2020 = getTunedOSCCAL(20, 20);
  uint8_t _speedlist_offset = 0;
  uint8_t blank = 0;
  if (FUSE.OSCCFG == 2) {
    _speedlist_offset++;
  }
  if (MEGATINYCORE_SERIES == 2) {
    _speedlist_offset++;
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
      Serial.print(speeds[i + _speedlist_offset]);
      Serial.print(" MHz: ");
      int16_t temp = getTunedOSCCAL(16, speeds[i + _speedlist_offset]);
      if (temp == 0xFF) {
        Serial.println("Blank");
        blank++;
      } else if (temp == 0x80) {
        if (speeds[i + _speedlist_offset] <= 12) {
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
      Serial.print(speeds[i + _speedlist_offset]);
      Serial.print(" MHz: ");
      int16_t temp = getTunedOSCCAL(16, speeds[i + _speedlist_offset]);
      if (temp == 0xFF) {
        Serial.println("Blank");
        blank++;
      } else if (temp == 0x80) {
        if (speeds[i + _speedlist_offset] <= 12) {
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
