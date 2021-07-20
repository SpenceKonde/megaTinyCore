void setup() {
  /* test variations of tone functions */
  #if defined(MILLIS_USE_TIMERB0) & !defined(TCB1)
    #warning "Skipped - tone() is unavailable when the only type B timer is used for millis instead."
  #else
    tone(PIN_PA1,1000); 
    delay(500);
    noTone(PIN_PA1);
    delay(500);
    tone(PIN_PA1,2000); 
    delay(500);
    noTone(PIN_PA2); //doesn't match, should do nothing.
    delay(1000);
    noTone(PIN_PA1); //does match, should turn off
    delay(500);
    tone(PIN_PA1,3000,1000); 
    delay(500);
  #endif
}
void loop() {
}
