/* Minimal demo of uaing the ADC to read temperature and operating voltage */

#define RESULTCOUNT 4
int16_t results[RESULTCOUNT];
int32_t sum;
int16_t average;

void setup() {
  // put your setup code here, to run once:
  delay(1000);
  Serial.begin(57600);
}

uint16_t readSupplyVoltage() { //returns value in millivolts to avoid floating point
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
}
void printRegisters() {
  Serial.print("ADC0.MUXPOS: ");
  showHex(ADC0.MUXPOS);
  Serial.print("  ADC0.CTRLC: ");
  showHex(ADC0.CTRLC);
  Serial.print("  VREF.CTRLA: ");
  showHex(VREF.CTRLA);
  Serial.println();
}

uint16_t readTemp() {
  //based on the datasheet, in section 30.3.2.5 Temperature Measurement
  int8_t sigrow_offset = SIGROW.TEMPSENSE1; // Read signed value from signature row
  uint8_t sigrow_gain = SIGROW.TEMPSENSE0; // Read unsigned value from signature row
  analogReference(INTERNAL1V1);
  ADC0.SAMPCTRL = 0x1F; //maximum length sampling
  ADC0.CTRLD &= ~(ADC_INITDLY_gm);
  ADC0.CTRLD |= ADC_INITDLY_DLY32_gc; //wait 32 ADC clocks before reading new reference
  uint16_t adc_reading = analogRead(ADC_TEMPERATURE); // ADC conversion result with 1.1 V internal reference
  Serial.println(adc_reading);
  analogReference(VDD);
  ADC0.SAMPCTRL = 0x0E; //14, what we now set it to automatically on startup so we can run the ADC while keeping the same sampling time
  ADC0.CTRLD &= ~(ADC_INITDLY_gm);
  ADC0.CTRLD |= ADC_INITDLY_DLY16_gc;
  uint32_t temp = adc_reading - sigrow_offset;
  Serial.println(temp);
  temp *= sigrow_gain; // Result might overflow 16 bit variable (10bit+8bit)
  Serial.println(temp);
  temp += 0x80; // Add 1/2 to get correct rounding on division below
  temp >>= 8; // Divide result to get Kelvin
  return temp;
}

void showHex(const byte b) {
  char buf [3] = { ((b >> 4) & 0x0F) | '0', (b & 0x0F) | '0', 0};
  if (buf [0] > '9') {
    buf [0] += 7;
  }
  if (buf [1] > '9') {
    buf [1] += 7;
  }
  Serial.print(buf);
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
