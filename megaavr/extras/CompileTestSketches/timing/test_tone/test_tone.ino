void setup() {

  pinMode(PIN_PA1, OUTPUT);
  pinMode(PIN_PA3, OUTPUT);
  Serial.begin(115200);
}
void loop() {

//#if (defined(TCB1) || !defined(MILLIS_TIMERB0)))
  //VPORTA.OUT |= 128;
  tone(PIN_PA1,1000);
  delay(1000);
  noTone(PIN_PA1);
  tone(PIN_PA1,2000);
  delay(3000);
  tone(PIN_PA3,3000);
  delay(2000);
  noTone(PIN_PA2)
  tone(PIN_PA1,4500);
  delay(3000);
  tone(PIN_PA1,7000);
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
//#else
  #warning "Tone reqires a type B timer but the only one we have is in use. Skip"
//#endif
}
