/* Minimal demo of using the ADC to read temperature and operating voltage
 *
 * Reading Vdd on the 2-series is VERY easy though, because there's a channel
 * called VDDDIV10, and the core presents it as ADC_VDDDIV10. Care to guess
 * what voltage analogRead(ADC_VDDDIV10) measures?
 *    Maybe... the supply voltage, Vdd, divided by 10?
 * So you can just switch to the internal 1.024V reference (or the others if
 * you prefer less accurate measurements) and read it. The math is even much
 * easier... with the reference voltages as multiples of 1024, the 1.024v
 * reference is in units of 0.25 mV. To get millivolts, just divide by 4.
 */

/* The 2-series temperature sensor on the other hand... well, frankly it seems to be
 * rather poorly calibrated. Batches are consistent amongst each other - but not with
 * the actual temperature. One user reported getting the very reasonable 300 or 299K value
 * for every chip he tested, as expected.
 * I tested 5 1624 and 3224 chips at ambient temperature of around 85 F (302 K), and consistently
 * got results between 313 and 318K. Running at 3.3V instead of 5V would lower the calculated
 * temperature by around 2K. It is most certainly not 105 F in my workshop, even though it is a warm day.
 *
 * So as regards temp sensing on the 2-series, YMMV
 */

#include <megaTinyCore.h>
/* This demonstrates both how the routines to measure temperature are implemented, with explanitary serial prints
 * and then calls the library functions provided that do the same thing (just faster and with less flash usage,
 * particularly on 2-series parts, where a shortcut was found in the mathematics
 */

#define RESULTCOUNT 4
int16_t results[RESULTCOUNT];
int32_t sum;
int16_t average;

void setup() {
  // put your setup code here, to run once:
  delay(1000);
  Serial.begin(57600);
  #if MEGATINYCORE_SERIES == 2
  Serial.println("On some parts, these results may be off by more than twice the spec'ed +/- 5 degrees.");
  #endif
}

uint16_t readSupplyVoltageExplained() { // returns value in millivolts to avoid floating point
  #if MEGATINYCORE_SERIES!=2
  analogReference(VDD);
  VREF.CTRLA = VREF_ADC0REFSEL_1V5_gc;
  // there is a settling time between when reference is turned on, and when it becomes valid.
  // since the reference is normally turned on only when it is requested, this virtually guarantees
  // that the first reading will be garbage; subsequent readings taken immediately after will be fine.
  // VREF.CTRLB|=VREF_ADC0REFEN_bm;
  // delay(10);
  uint16_t reading = analogRead(ADC_INTREF);
  Serial.print(reading);
  Serial.println(" (discarded)");
  reading = analogRead(ADC_INTREF);
  Serial.println(reading);
  uint32_t intermediate = 1023UL * 1500;
  reading = intermediate / reading;
  return reading;
  #else
  analogReference(INTERNAL1V024);
  Serial.print(analogReadEnh(ADC_VDDDIV10, 12)); // throwaway reading just for good measure.
  Serial.println(" (discarded)");
  int32_t vddmeasure = analogReadEnh(ADC_VDDDIV10, 12); // Take it at 12 bits
  Serial.println(vddmeasure);
  vddmeasure *= 10; // since we measured 1/10th VDD
  int16_t returnval = vddmeasure >> 2; // divide by 4 to get into millivolts.
  /* The above method of doing the math is bad, because multiplication of a 32-bit value takes
   * 81 clocks. The smart way to multiply by 10 and then divide by four is to instead take the sum
   * of the original value leftshifted one place, and the original value rightshifted one place.
   * And here most of this can be donewith a 16-bit variable not a 32 bit one
   */
  if (vddmeasure & 0x02) {
    // if last two digits were 0b11 or 0b10 we should round up
    returnval++;
  }
  return returnval;
  #endif
}
void printRegisters() {
  #if MEGATINYCORE_SERIES!=2
  Serial.print("ADC0.MUXPOS: ");
  Serial.printHex(ADC0.MUXPOS);
  Serial.print("  ADC0.CTRLC: ");
  Serial.printHex(ADC0.CTRLC);
  Serial.print("  VREF.CTRLA: ");
  Serial.printHex(VREF.CTRLA);
  Serial.println();
  #endif
}

uint16_t readTempExplained() {
  #if MEGATINYCORE_SERIES!=2
  // based on the datasheet, in section 30.3.2.5 Temperature Measurement
  int8_t sigrow_offset = SIGROW.TEMPSENSE1; // Read signed value from signature row
  uint8_t sigrow_gain = SIGROW.TEMPSENSE0; // Read unsigned value from signature row
  analogReference(INTERNAL1V1);
  ADC0.SAMPCTRL = 0x1F; // maximum length sampling
  ADC0.CTRLD &= ~(ADC_INITDLY_gm);
  ADC0.CTRLD |= ADC_INITDLY_DLY32_gc; // wait 32 ADC clocks before reading new reference
  uint16_t adc_reading = analogRead(ADC_TEMPERATURE); // ADC conversion result with 1.1 V internal reference
  Serial.println(adc_reading);
  analogReference(VDD);
  ADC0.SAMPCTRL = 0x0E; // 14, what we now set it to automatically on startup so we can run the ADC while keeping the same sampling time
  ADC0.CTRLD &= ~(ADC_INITDLY_gm);
  ADC0.CTRLD |= ADC_INITDLY_DLY16_gc;
  uint32_t temp = adc_reading - sigrow_offset;
  Serial.println(temp);
  temp *= sigrow_gain; // Result might overflow 16 bit variable (10bit+8bit)
  Serial.println(temp);
  temp += 0x80; // Add 1/2 to get correct rounding on division below
  temp >>= 8; // Divide result to get Kelvin
  return temp;
  #else
  int8_t sigrowOffset = SIGROW.TEMPSENSE1;
  uint8_t sigrowGain = SIGROW.TEMPSENSE0;
  analogSampleDuration(128); // must be >= 32µs * f_CLK_ADC per datasheet 30.3.3.7
  analogReference(INTERNAL1V024);
  uint32_t reading = analogRead(ADC_TEMPERATURE);
  reading -= sigrowOffset;
  reading *= sigrowGain;
  reading += 0x80; // Add 1/2 to get correct rounding on division below
  reading >>= 8; // Divide result to get Kelvin
  return reading;
  #endif
}



void loop() {
  int16_t reading = readSupplyVoltageExplain();
  Serial.print("System voltage is: ");
  Serial.print(reading);
  Serial.println(" mV");
  reading = readTempExplained();
  Serial.print("System temperature is: ");
  Serial.print(reading);
  Serial.println(" K");
  Serial.println("See notes in sketch on the accuracy (or rather the lack thereof) of the builtin temperature sensor");
  delay(1000);
  Serial.println("Repeating above without the explanations")
  reading = readSupplyVoltage();
  Serial.print("System voltage is: ");
  Serial.print(reading);
  Serial.println(" mV");
  reading = readTemp();
  Serial.print("System temperature is: ");
  Serial.print(reading);
  Serial.println(" K");
}
