void setup() {

  pinMode(PIN_PA7, OUTPUT);
  pinMode(PIN_PB1, OUTPUT);
  Serial.begin(115200);
}
void loop() {
  wdt_reset();
  //VPORTA.OUT |= 128;
  tone(PIN_PB1,1000);
  delay(3000);
  tone(PIN_PB1,2000);
  delay(3000);
  tone(PIN_PA3,3000);
  delay(6000);
  tone(PIN_PB1,4500);
  delay(3000);
  tone(PIN_PB1,7000);
  delay(3000);
  /*
    Serial.pritln('N`);
    noTone(PIN_PA2); //doesn't match, should do nothing.
    delay(1000);
    Serial.pritln('N`);
    noTone(PIN_PA1); //does match, should turn off
    delay(1000);
    VPORTA.OUT &= ~128;
    tone(PIN_PA1,3000,1000);
    delay(2000);
    VPORTA.OUT |= 128;
    tone(PIN_PA2,5000,1000);
    VPORTA.OUT &= ~128;
  */
}
void wdt_reset() {
  asm volatile ("wdr"::);
}
