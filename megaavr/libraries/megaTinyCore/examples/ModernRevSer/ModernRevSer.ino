/* Read the silicon revision of a "modern" (post-2016, AVRxt) part, including megaAVR 0-series,
 * tinyAVR 0/1/2-series, AVR Dx-series, and likely the upcoming AVR Ex-series.
 * Also reads out the fuses and serial number. The fuses and serial number were part of an
 * attempt to determine whether the date code was embedded into the serial number somehow.
 * I suspect it is possible to determine the date code from it in some way.
 * A few interesting things to note about the serial number:
 *    The data bytes do not appear to use the full range of values.
 *    The entropy is not distributed uniformly across the serial number,
 *        that is to say, it's a serial number, not a hash of a serial number.
 *        Don't try to use it as an entropy source.
 *    For these reasons, it is possible that additional information can be deduced from it
 */

#if (__AVR_ARCH__ < 100)
#error "This sketch is designed for 'modern AVR' parts only (post-2016, when they revised peripherals and instruction set timing)"
#endif
void setup() {
  Serial.begin(115200);
  Serial.println();
  delay(1000);
  Serial.print("REVID: ");
  #ifdef SIGROW_SERNUM15 //AVR Dx-series
  #define SERLEN
  char major=0x40+(SYSCFG.REVID>>4);
  Serial.print(major);
  Serial.println(SYSCFG.REVID&0x0F);
  #else
  Serial.println('@'+SYSCFG.REVID);
  #endif
  Serial.print("S/N: ");
  volatile uint8_t *mptr=&SIGROW_SERNUM0;
  showHex(*mptr++);
  for (byte i = 0;i<15;i++){
    Serial.print(':');
    showHex(*mptr++);
  }
  Serial.println();
  Serial.print("FUSES: ");
  mptr=FUSES_START;
  showHex(*mptr++);
  for (byte i = 1;i<9;i++){
    Serial.print(':');
    showHex(*mptr++);
  }
  Serial.println();


}
void showHex (const byte b) {
  char x=(b>>4)|'0';
  if (x > '9')
    x += 7;
  Serial.write(x);
  x=(b&0x0F)|'0';
  if (x > '9')
    x += 7;
  Serial.write(x);
}

void loop() {
  // put your main code here, to run repeatedly:

}
