/* Minimal demo of uaing the ADC to read temperature and operating voltage
 * This has not yet been updated to cover temperature on the new 2-series parts
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

#define RESULTCOUNT 4
int16_t results[RESULTCOUNT];
int32_t sum;
int16_t average;

void setup() {
  // put your setup code here, to run once:
  delay(1000);
  Serial.begin(57600);
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
  Serial.print(analogReadEnh(ADC_VDDDIV10, 12)); // throwaway reading just for goood measure.
  Serial.println(" (discarded)");
  int32_t vddmeasure = analogReadEnh(ADC_VDDDIV10, 12); // Take it at 12 bits
  Serial.println(vddmeasure);
  int16_t returnval = vddmeasure >> 2; // divide by 4 to get into millivolts.
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

uint16_t readTemp() {
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
  uint32_t temp = adc_reading + sigrow_offset; // Datasheet is WRONG - the offset from the sigrow is signed; it should be added, not subtracted!
  Serial.println(temp);
  temp *= sigrow_gain; // Result might overflow 16 bit variable (10bit+8bit)
  Serial.println(temp);
  temp += 0x80; // Add 1/2 to get correct rounding on division below
  temp >>= 8; // Divide result to get Kelvin
  return temp;
  #else
  return -1;
  #endif
}
void loop() {
  int16_t reading = readSupplyVoltage();
  Serial.print("System voltage is: ");
  Serial.print(reading);
  Serial.println(" mV");
  reading = readTemp();
  Serial.print("System temperature is: ");
  Serial.print(reading);
  Serial.println(" K");

  delay(10000);
}
