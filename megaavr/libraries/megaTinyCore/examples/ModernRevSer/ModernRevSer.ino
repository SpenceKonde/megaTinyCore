/* Read the silicon revision of a "modern" (post-2016, AVRxt) part, including megaAVR 0-series,
   tinyAVR 0/1/2-series, AVR Dx-series, and likely the upcoming AVR Ex-series.
   Also reads out the fuses and serial number. The fuses and serial number were part of an
   attempt to determine whether the date code was embedded into the serial number somehow.
   I suspect it is possible to determine the date code from it in some way.
   A few interesting things to note about the serial number:
      The data bytes do not appear to use the full range of values.
      The entropy is not distributed uniformly across the serial number,
          that is to say, it's a serial number, not a hash of a serial number.
          Don't try to use it as an entropy source.
      For these reasons, it is possible that additional information can be deduced from it
  On the tinyAVR series, there is *definitely* information hiding in the SIGROW, that's why we print it.
  Most interesting imho would be a list of the recorded tuning, if any, and to tune the parts (or at least selftune)
  first. That will be enough to get a good estimate of the maximum speed each specimen can run at. With a large enough
  dataset we might be able to find correlations:
  * Can we calculate tuning values without experimentally vistiting all values? This would present time savings, which could be put
  towards making sure it *really* worked at those speeds.
  * Does anything in the signature row correlate with whether the part is an extended temperature range part?
  * What are the three mystery calibrations for?
  * What are the blocks of data which are constant for each device model encoding?
  * Do Automotive parts have different values written to them?
  * Note that only tinyAVRs have visible data in the sigrow. It's all empty on Dx except for the stuff documented in the datasheet :-(

*/
#include <megaTinyCore.h>
#if (__AVR_ARCH__ < 100)
  #error "This sketch is designed for 'modern AVR' parts only (post-2016, when they revised peripherals and instruction set timing)"
#endif
void setup() {
  Serial.begin(115200);
  Serial.println();
  delay(1000);
  Serial.print("REVID: ");
  #ifdef SIGROW_SERNUM15 // AVR Dx-series
  char major = 0x40 + (SYSCFG.REVID >> 4);
  Serial.print(major);
  Serial.println(SYSCFG.REVID & 0x0F);
  #else
  Serial.println('@' + SYSCFG.REVID);
  #endif
  Serial.print("S/N: ");
  volatile uint8_t *mptr = &SIGROW_SERNUM0;
  Serial.printHex(*mptr++);
  for (byte i = 0; i < 15; i++) {
    Serial.print(':');
    Serial.printHex(*mptr++);
  }
  Serial.println();
  Serial.print("FUSES: ");
  mptr = FUSES_START;
  Serial.printHex(*mptr++);
  for (byte i = 1; i < 9; i++) {
    Serial.print(':');
    Serial.printHex(*mptr++);
  }
  Serial.println();
  Serial.println("SIGROW: ");
  mptr = SIGNATURES_START;
  Serial.printHex(*mptr++);
    for (byte i = 0; i < (SIGNATURES_SIZE + PROD_SIGNATURES_SIZE - 1); i++) {
    Serial.print(':');
    Serial.printHex(*mptr++);
  }
}
void loop() {
  delay(10000);
  ResetViaSoftware(); // they may not have autoreset, and might have a hard time catching the output the first time it goes by

}
