void setup() {
  // put your setup code here, to run once:
}

void loop() {
  /* Test PIN_PA0 */
  #if defined(PIN_PA0) && (CLOCK_SOURCE == 0 || defined(MEGATINYCORE))
    /* Unless it's a tinyAVR part with 2k of flash, in which case we don't have the space. */
    #if (!defined(MEGATINYCORE) || PROGMEM_SIZE > 2048)
      pinMode(PIN_PA0, OUTPUT);
      pinMode(PIN_PA0, INPUT);
      pinMode(PIN_PA0, INPUT_PULLUP);
      digitalWrite(PIN_PA0, HIGH);
      digitalWrite(PIN_PA0, LOW);
      openDrain(PIN_PA0, LOW);
      openDrain(PIN_PA0, FLOATING);
      digitalRead(PIN_PA0);
      analogWrite(PIN_PA0, 0);
      analogWrite(PIN_PA0, 128);
      analogWrite(PIN_PA0, 255);
      digitalWriteFast(PIN_PA0, HIGH);
      digitalWriteFast(PIN_PA0, LOW);
      openDrainFast(PIN_PA0, LOW);
      openDrainFast(PIN_PA0, FLOATING);
      digitalReadFast(PIN_PA0);
    #endif
  #endif

  /* Test PIN_PA1 */
  #if defined(PIN_PA1) && (CLOCK_SOURCE != 1 || defined(MEGATINYCORE))
    pinMode(PIN_PA1, OUTPUT);
    pinMode(PIN_PA1, INPUT);
    pinMode(PIN_PA1, INPUT_PULLUP);
    digitalWrite(PIN_PA1, HIGH);
    digitalWrite(PIN_PA1, LOW);
    openDrain(PIN_PA1, LOW);
    openDrain(PIN_PA1, FLOATING);
    digitalRead(PIN_PA1);
    analogWrite(PIN_PA1, 0);
    analogWrite(PIN_PA1, 128);
    analogWrite(PIN_PA1, 255);
    digitalWriteFast(PIN_PA1, HIGH);
    digitalWriteFast(PIN_PA1, LOW);
    openDrainFast(PIN_PA1, LOW);
    openDrainFast(PIN_PA1, FLOATING);
    digitalReadFast(PIN_PA1);
  #endif

  /* Test PIN_PA2 */
  #ifdef PIN_PA2
    pinMode(PIN_PA2, OUTPUT);
    pinMode(PIN_PA2, INPUT);
    pinMode(PIN_PA2, INPUT_PULLUP);
    digitalWrite(PIN_PA2, HIGH);
    digitalWrite(PIN_PA2, LOW);
    openDrain(PIN_PA2, LOW);
    openDrain(PIN_PA2, FLOATING);
    digitalRead(PIN_PA2);
    analogWrite(PIN_PA2, 0);
    analogWrite(PIN_PA2, 128);
    analogWrite(PIN_PA2, 255);
    digitalWriteFast(PIN_PA2, HIGH);
    digitalWriteFast(PIN_PA2, LOW);
    openDrainFast(PIN_PA2, LOW);
    openDrainFast(PIN_PA2, FLOATING);
    digitalReadFast(PIN_PA2);
  #endif

  /* Test PIN_PA3 */

  #if defined(PIN_PA3) && !(CLOCK_SOURCE == 2 && defined(MEGATINYCORE))
    pinMode(PIN_PA3, OUTPUT);
    pinMode(PIN_PA3, INPUT);
    pinMode(PIN_PA3, INPUT_PULLUP);
    digitalWrite(PIN_PA3, HIGH);
    digitalWrite(PIN_PA3, LOW);
    openDrain(PIN_PA3, LOW);
    openDrain(PIN_PA3, FLOATING);
    digitalRead(PIN_PA3);
    analogWrite(PIN_PA3, 0);
    analogWrite(PIN_PA3, 128);
    analogWrite(PIN_PA3, 255);
    digitalWriteFast(PIN_PA3, HIGH);
    digitalWriteFast(PIN_PA3, LOW);
    openDrainFast(PIN_PA3, LOW);
    openDrainFast(PIN_PA3, FLOATING);
    digitalReadFast(PIN_PA3);
  #endif

  /* Test PIN_PA4 */
  #ifdef PIN_PA4
    pinMode(PIN_PA4, OUTPUT);
    pinMode(PIN_PA4, INPUT);
    pinMode(PIN_PA4, INPUT_PULLUP);
    digitalWrite(PIN_PA4, HIGH);
    digitalWrite(PIN_PA4, LOW);
    openDrain(PIN_PA4, LOW);
    openDrain(PIN_PA4, FLOATING);
    digitalRead(PIN_PA4);
    analogWrite(PIN_PA4, 0);
    analogWrite(PIN_PA4, 128);
    analogWrite(PIN_PA4, 255);
    digitalWriteFast(PIN_PA4, HIGH);
    digitalWriteFast(PIN_PA4, LOW);
    openDrainFast(PIN_PA4, LOW);
    openDrainFast(PIN_PA4, FLOATING);
    digitalReadFast(PIN_PA4);
  #endif

  /* Test PIN_PA5 */
  #ifdef PIN_PA5
    pinMode(PIN_PA5, OUTPUT);
    pinMode(PIN_PA5, INPUT);
    pinMode(PIN_PA5, INPUT_PULLUP);
    digitalWrite(PIN_PA5, HIGH);
    digitalWrite(PIN_PA5, LOW);
    openDrain(PIN_PA5, LOW);
    openDrain(PIN_PA5, FLOATING);
    digitalRead(PIN_PA5);
    analogWrite(PIN_PA5, 0);
    analogWrite(PIN_PA5, 128);
    analogWrite(PIN_PA5, 255);
    digitalWriteFast(PIN_PA5, HIGH);
    digitalWriteFast(PIN_PA5, LOW);
    openDrainFast(PIN_PA5, LOW);
    openDrainFast(PIN_PA5, FLOATING);
    digitalReadFast(PIN_PA5);
  #endif

  /* Test PIN_PA6 */
  #ifdef PIN_PA6
    pinMode(PIN_PA6, OUTPUT);
    pinMode(PIN_PA6, INPUT);
    pinMode(PIN_PA6, INPUT_PULLUP);
    digitalWrite(PIN_PA6, HIGH);
    digitalWrite(PIN_PA6, LOW);
    openDrain(PIN_PA6, LOW);
    openDrain(PIN_PA6, FLOATING);
    digitalRead(PIN_PA6);
    analogWrite(PIN_PA6, 0);
    analogWrite(PIN_PA6, 128);
    analogWrite(PIN_PA6, 255);
    digitalWriteFast(PIN_PA6, HIGH);
    digitalWriteFast(PIN_PA6, LOW);
    openDrainFast(PIN_PA6, LOW);
    openDrainFast(PIN_PA6, FLOATING);
    digitalReadFast(PIN_PA6);
  #endif

  /* Test PIN_PA7 */
  #ifdef PIN_PA7
    pinMode(PIN_PA7, OUTPUT);
    pinMode(PIN_PA7, INPUT);
    pinMode(PIN_PA7, INPUT_PULLUP);
    digitalWrite(PIN_PA7, HIGH);
    digitalWrite(PIN_PA7, LOW);
    openDrain(PIN_PA7, LOW);
    openDrain(PIN_PA7, FLOATING);
    digitalRead(PIN_PA7);
    analogWrite(PIN_PA7, 0);
    analogWrite(PIN_PA7, 128);
    analogWrite(PIN_PA7, 255);
    digitalWriteFast(PIN_PA7, HIGH);
    digitalWriteFast(PIN_PA7, LOW);
    openDrainFast(PIN_PA7, LOW);
    openDrainFast(PIN_PA7, FLOATING);
    digitalReadFast(PIN_PA7);
  #endif

  /* Test PIN_PB0 */
  #ifdef PIN_PB0
    pinMode(PIN_PB0, OUTPUT);
    pinMode(PIN_PB0, INPUT);
    pinMode(PIN_PB0, INPUT_PULLUP);
    digitalWrite(PIN_PB0, HIGH);
    digitalWrite(PIN_PB0, LOW);
    openDrain(PIN_PB0, LOW);
    openDrain(PIN_PB0, FLOATING);
    digitalRead(PIN_PB0);
    analogWrite(PIN_PB0, 0);
    analogWrite(PIN_PB0, 128);
    analogWrite(PIN_PB0, 255);
    digitalWriteFast(PIN_PB0, HIGH);
    digitalWriteFast(PIN_PB0, LOW);
    openDrainFast(PIN_PB0, LOW);
    openDrainFast(PIN_PB0, FLOATING);
    digitalReadFast(PIN_PB0);
  #endif

  /* Test PIN_PB1 */
  #ifdef PIN_PB1
    pinMode(PIN_PB1, OUTPUT);
    pinMode(PIN_PB1, INPUT);
    pinMode(PIN_PB1, INPUT_PULLUP);
    digitalWrite(PIN_PB1, HIGH);
    digitalWrite(PIN_PB1, LOW);
    openDrain(PIN_PB1, LOW);
    openDrain(PIN_PB1, FLOATING);
    digitalRead(PIN_PB1);
    analogWrite(PIN_PB1, 0);
    analogWrite(PIN_PB1, 128);
    analogWrite(PIN_PB1, 255);
    digitalWriteFast(PIN_PB1, HIGH);
    digitalWriteFast(PIN_PB1, LOW);
    openDrainFast(PIN_PB1, LOW);
    openDrainFast(PIN_PB1, FLOATING);
    digitalReadFast(PIN_PB1);
  #endif

  /* Test PIN_PB2 */
  #ifdef PIN_PB2
    pinMode(PIN_PB2, OUTPUT);
    pinMode(PIN_PB2, INPUT);
    pinMode(PIN_PB2, INPUT_PULLUP);
    digitalWrite(PIN_PB2, HIGH);
    digitalWrite(PIN_PB2, LOW);
    openDrain(PIN_PB2, LOW);
    openDrain(PIN_PB2, FLOATING);
    digitalRead(PIN_PB2);
    analogWrite(PIN_PB2, 0);
    analogWrite(PIN_PB2, 128);
    analogWrite(PIN_PB2, 255);
    digitalWriteFast(PIN_PB2, HIGH);
    digitalWriteFast(PIN_PB2, LOW);
    openDrainFast(PIN_PB2, LOW);
    openDrainFast(PIN_PB2, FLOATING);
    digitalReadFast(PIN_PB2);
  #endif

  /* Test PIN_PB3 */
  #ifdef PIN_PB3
    pinMode(PIN_PB3, OUTPUT);
    pinMode(PIN_PB3, INPUT);
    pinMode(PIN_PB3, INPUT_PULLUP);
    digitalWrite(PIN_PB3, HIGH);
    digitalWrite(PIN_PB3, LOW);
    openDrain(PIN_PB3, LOW);
    openDrain(PIN_PB3, FLOATING);
    digitalRead(PIN_PB3);
    analogWrite(PIN_PB3, 0);
    analogWrite(PIN_PB3, 128);
    analogWrite(PIN_PB3, 255);
    digitalWriteFast(PIN_PB3, HIGH);
    digitalWriteFast(PIN_PB3, LOW);
    openDrainFast(PIN_PB3, LOW);
    openDrainFast(PIN_PB3, FLOATING);
    digitalReadFast(PIN_PB3);
  #endif

  /* Test PIN_PB4 */
  #ifdef PIN_PB4
    pinMode(PIN_PB4, OUTPUT);
    pinMode(PIN_PB4, INPUT);
    pinMode(PIN_PB4, INPUT_PULLUP);
    digitalWrite(PIN_PB4, HIGH);
    digitalWrite(PIN_PB4, LOW);
    openDrain(PIN_PB4, LOW);
    openDrain(PIN_PB4, FLOATING);
    digitalRead(PIN_PB4);
    analogWrite(PIN_PB4, 0);
    analogWrite(PIN_PB4, 128);
    analogWrite(PIN_PB4, 255);
    digitalWriteFast(PIN_PB4, HIGH);
    digitalWriteFast(PIN_PB4, LOW);
    openDrainFast(PIN_PB4, LOW);
    openDrainFast(PIN_PB4, FLOATING);
    digitalReadFast(PIN_PB4);
  #endif

  /* Test PIN_PB5 */
  #ifdef PIN_PB5
    pinMode(PIN_PB5, OUTPUT);
    pinMode(PIN_PB5, INPUT);
    pinMode(PIN_PB5, INPUT_PULLUP);
    digitalWrite(PIN_PB5, HIGH);
    digitalWrite(PIN_PB5, LOW);
    openDrain(PIN_PB5, LOW);
    openDrain(PIN_PB5, FLOATING);
    digitalRead(PIN_PB5);
    analogWrite(PIN_PB5, 0);
    analogWrite(PIN_PB5, 128);
    analogWrite(PIN_PB5, 255);
    digitalWriteFast(PIN_PB5, HIGH);
    digitalWriteFast(PIN_PB5, LOW);
    openDrainFast(PIN_PB5, LOW);
    openDrainFast(PIN_PB5, FLOATING);
    digitalReadFast(PIN_PB5);
  #endif

  /* Test PIN_PB6 */
  #ifdef PIN_PB6
    pinMode(PIN_PB6, OUTPUT);
    pinMode(PIN_PB6, INPUT);
    pinMode(PIN_PB6, INPUT_PULLUP);
    digitalWrite(PIN_PB6, HIGH);
    digitalWrite(PIN_PB6, LOW);
    openDrain(PIN_PB6, LOW);
    openDrain(PIN_PB6, FLOATING);
    digitalRead(PIN_PB6);
    analogWrite(PIN_PB6, 0);
    analogWrite(PIN_PB6, 128);
    analogWrite(PIN_PB6, 255);
    digitalWriteFast(PIN_PB6, HIGH);
    digitalWriteFast(PIN_PB6, LOW);
    openDrainFast(PIN_PB6, LOW);
    openDrainFast(PIN_PB6, FLOATING);
    digitalReadFast(PIN_PB6);
  #endif

  /* Test PIN_PB7 */
  #ifdef PIN_PB7
    pinMode(PIN_PB7, OUTPUT);
    pinMode(PIN_PB7, INPUT);
    pinMode(PIN_PB7, INPUT_PULLUP);
    digitalWrite(PIN_PB7, HIGH);
    digitalWrite(PIN_PB7, LOW);
    openDrain(PIN_PB7, LOW);
    openDrain(PIN_PB7, FLOATING);
    digitalRead(PIN_PB7);
    analogWrite(PIN_PB7, 0);
    analogWrite(PIN_PB7, 128);
    analogWrite(PIN_PB7, 255);
    digitalWriteFast(PIN_PB7, HIGH);
    digitalWriteFast(PIN_PB7, LOW);
    openDrainFast(PIN_PB7, LOW);
    openDrainFast(PIN_PB7, FLOATING);
    digitalReadFast(PIN_PB7);
  #endif

  /* Test PIN_PC0 */
  #ifdef PIN_PC0
    pinMode(PIN_PC0, OUTPUT);
    pinMode(PIN_PC0, INPUT);
    pinMode(PIN_PC0, INPUT_PULLUP);
    digitalWrite(PIN_PC0, HIGH);
    digitalWrite(PIN_PC0, LOW);
    openDrain(PIN_PC0, LOW);
    openDrain(PIN_PC0, FLOATING);
    digitalRead(PIN_PC0);
    analogWrite(PIN_PC0, 0);
    analogWrite(PIN_PC0, 128);
    analogWrite(PIN_PC0, 255);
    digitalWriteFast(PIN_PC0, HIGH);
    digitalWriteFast(PIN_PC0, LOW);
    openDrainFast(PIN_PC0, LOW);
    openDrainFast(PIN_PC0, FLOATING);
    digitalReadFast(PIN_PC0);
  #endif

  /* Test PIN_PC1 */
  #ifdef PIN_PC1
    pinMode(PIN_PC1, OUTPUT);
    pinMode(PIN_PC1, INPUT);
    pinMode(PIN_PC1, INPUT_PULLUP);
    digitalWrite(PIN_PC1, HIGH);
    digitalWrite(PIN_PC1, LOW);
    openDrain(PIN_PC1, LOW);
    openDrain(PIN_PC1, FLOATING);
    digitalRead(PIN_PC1);
    analogWrite(PIN_PC1, 0);
    analogWrite(PIN_PC1, 128);
    analogWrite(PIN_PC1, 255);
    digitalWriteFast(PIN_PC1, HIGH);
    digitalWriteFast(PIN_PC1, LOW);
    openDrainFast(PIN_PC1, LOW);
    openDrainFast(PIN_PC1, FLOATING);
    digitalReadFast(PIN_PC1);
  #endif

  /* Test PIN_PC2 */
  #ifdef PIN_PC2
    pinMode(PIN_PC2, OUTPUT);
    pinMode(PIN_PC2, INPUT);
    pinMode(PIN_PC2, INPUT_PULLUP);
    digitalWrite(PIN_PC2, HIGH);
    digitalWrite(PIN_PC2, LOW);
    openDrain(PIN_PC2, LOW);
    openDrain(PIN_PC2, FLOATING);
    digitalRead(PIN_PC2);
    analogWrite(PIN_PC2, 0);
    analogWrite(PIN_PC2, 128);
    analogWrite(PIN_PC2, 255);
    digitalWriteFast(PIN_PC2, HIGH);
    digitalWriteFast(PIN_PC2, LOW);
    openDrainFast(PIN_PC2, LOW);
    openDrainFast(PIN_PC2, FLOATING);
    digitalReadFast(PIN_PC2);
  #endif

  /* Test PIN_PC3 */
  #ifdef PIN_PC3
    pinMode(PIN_PC3, OUTPUT);
    pinMode(PIN_PC3, INPUT);
    pinMode(PIN_PC3, INPUT_PULLUP);
    digitalWrite(PIN_PC3, HIGH);
    digitalWrite(PIN_PC3, LOW);
    openDrain(PIN_PC3, LOW);
    openDrain(PIN_PC3, FLOATING);
    digitalRead(PIN_PC3);
    analogWrite(PIN_PC3, 0);
    analogWrite(PIN_PC3, 128);
    analogWrite(PIN_PC3, 255);
    digitalWriteFast(PIN_PC3, HIGH);
    digitalWriteFast(PIN_PC3, LOW);
    openDrainFast(PIN_PC3, LOW);
    openDrainFast(PIN_PC3, FLOATING);
    digitalReadFast(PIN_PC3);
  #endif

  /* Test PIN_PC4 */
  #ifdef PIN_PC4
    pinMode(PIN_PC4, OUTPUT);
    pinMode(PIN_PC4, INPUT);
    pinMode(PIN_PC4, INPUT_PULLUP);
    digitalWrite(PIN_PC4, HIGH);
    digitalWrite(PIN_PC4, LOW);
    openDrain(PIN_PC4, LOW);
    openDrain(PIN_PC4, FLOATING);
    digitalRead(PIN_PC4);
    analogWrite(PIN_PC4, 0);
    analogWrite(PIN_PC4, 128);
    analogWrite(PIN_PC4, 255);
    digitalWriteFast(PIN_PC4, HIGH);
    digitalWriteFast(PIN_PC4, LOW);
    openDrainFast(PIN_PC4, LOW);
    openDrainFast(PIN_PC4, FLOATING);
    digitalReadFast(PIN_PC4);
  #endif

  /* Test PIN_PC5 */
  #ifdef PIN_PC5
    pinMode(PIN_PC5, OUTPUT);
    pinMode(PIN_PC5, INPUT);
    pinMode(PIN_PC5, INPUT_PULLUP);
    digitalWrite(PIN_PC5, HIGH);
    digitalWrite(PIN_PC5, LOW);
    openDrain(PIN_PC5, LOW);
    openDrain(PIN_PC5, FLOATING);
    digitalRead(PIN_PC5);
    analogWrite(PIN_PC5, 0);
    analogWrite(PIN_PC5, 128);
    analogWrite(PIN_PC5, 255);
    digitalWriteFast(PIN_PC5, HIGH);
    digitalWriteFast(PIN_PC5, LOW);
    openDrainFast(PIN_PC5, LOW);
    openDrainFast(PIN_PC5, FLOATING);
    digitalReadFast(PIN_PC5);
  #endif

  /* Test PIN_PC6 */
  #ifdef PIN_PC6
    pinMode(PIN_PC6, OUTPUT);
    pinMode(PIN_PC6, INPUT);
    pinMode(PIN_PC6, INPUT_PULLUP);
    digitalWrite(PIN_PC6, HIGH);
    digitalWrite(PIN_PC6, LOW);
    openDrain(PIN_PC6, LOW);
    openDrain(PIN_PC6, FLOATING);
    digitalRead(PIN_PC6);
    analogWrite(PIN_PC6, 0);
    analogWrite(PIN_PC6, 128);
    analogWrite(PIN_PC6, 255);
    digitalWriteFast(PIN_PC6, HIGH);
    digitalWriteFast(PIN_PC6, LOW);
    openDrainFast(PIN_PC6, LOW);
    openDrainFast(PIN_PC6, FLOATING);
    digitalReadFast(PIN_PC6);
  #endif

  /* Test PIN_PC7 */
  #ifdef PIN_PC7
    pinMode(PIN_PC7, OUTPUT);
    pinMode(PIN_PC7, INPUT);
    pinMode(PIN_PC7, INPUT_PULLUP);
    digitalWrite(PIN_PC7, HIGH);
    digitalWrite(PIN_PC7, LOW);
    openDrain(PIN_PC7, LOW);
    openDrain(PIN_PC7, FLOATING);
    digitalRead(PIN_PC7);
    analogWrite(PIN_PC7, 0);
    analogWrite(PIN_PC7, 128);
    analogWrite(PIN_PC7, 255);
    digitalWriteFast(PIN_PC7, HIGH);
    digitalWriteFast(PIN_PC7, LOW);
    openDrainFast(PIN_PC7, LOW);
    openDrainFast(PIN_PC7, FLOATING);
    digitalReadFast(PIN_PC7);
  #endif

  /* Test PIN_PD0 */
  #ifdef PIN_PD0
    pinMode(PIN_PD0, OUTPUT);
    pinMode(PIN_PD0, INPUT);
    pinMode(PIN_PD0, INPUT_PULLUP);
    digitalWrite(PIN_PD0, HIGH);
    digitalWrite(PIN_PD0, LOW);
    openDrain(PIN_PD0, LOW);
    openDrain(PIN_PD0, FLOATING);
    digitalRead(PIN_PD0);
    analogWrite(PIN_PD0, 0);
    analogWrite(PIN_PD0, 128);
    analogWrite(PIN_PD0, 255);
    digitalWriteFast(PIN_PD0, HIGH);
    digitalWriteFast(PIN_PD0, LOW);
    openDrainFast(PIN_PD0, LOW);
    openDrainFast(PIN_PD0, FLOATING);
    digitalReadFast(PIN_PD0);
  #endif

  /* Test PIN_PD1 */
  #ifdef PIN_PD1
    pinMode(PIN_PD1, OUTPUT);
    pinMode(PIN_PD1, INPUT);
    pinMode(PIN_PD1, INPUT_PULLUP);
    digitalWrite(PIN_PD1, HIGH);
    digitalWrite(PIN_PD1, LOW);
    openDrain(PIN_PD1, LOW);
    openDrain(PIN_PD1, FLOATING);
    digitalRead(PIN_PD1);
    analogWrite(PIN_PD1, 0);
    analogWrite(PIN_PD1, 128);
    analogWrite(PIN_PD1, 255);
    digitalWriteFast(PIN_PD1, HIGH);
    digitalWriteFast(PIN_PD1, LOW);
    openDrainFast(PIN_PD1, LOW);
    openDrainFast(PIN_PD1, FLOATING);
    digitalReadFast(PIN_PD1);
  #endif

  /* Test PIN_PD2 */
  #ifdef PIN_PD2
    pinMode(PIN_PD2, OUTPUT);
    pinMode(PIN_PD2, INPUT);
    pinMode(PIN_PD2, INPUT_PULLUP);
    digitalWrite(PIN_PD2, HIGH);
    digitalWrite(PIN_PD2, LOW);
    openDrain(PIN_PD2, LOW);
    openDrain(PIN_PD2, FLOATING);
    digitalRead(PIN_PD2);
    analogWrite(PIN_PD2, 0);
    analogWrite(PIN_PD2, 128);
    analogWrite(PIN_PD2, 255);
    digitalWriteFast(PIN_PD2, HIGH);
    digitalWriteFast(PIN_PD2, LOW);
    openDrainFast(PIN_PD2, LOW);
    openDrainFast(PIN_PD2, FLOATING);
    digitalReadFast(PIN_PD2);
  #endif

  /* Test PIN_PD3 */
  #ifdef PIN_PD3
    pinMode(PIN_PD3, OUTPUT);
    pinMode(PIN_PD3, INPUT);
    pinMode(PIN_PD3, INPUT_PULLUP);
    digitalWrite(PIN_PD3, HIGH);
    digitalWrite(PIN_PD3, LOW);
    openDrain(PIN_PD3, LOW);
    openDrain(PIN_PD3, FLOATING);
    digitalRead(PIN_PD3);
    analogWrite(PIN_PD3, 0);
    analogWrite(PIN_PD3, 128);
    analogWrite(PIN_PD3, 255);
    digitalWriteFast(PIN_PD3, HIGH);
    digitalWriteFast(PIN_PD3, LOW);
    openDrainFast(PIN_PD3, LOW);
    openDrainFast(PIN_PD3, FLOATING);
    digitalReadFast(PIN_PD3);
  #endif

  /* Test PIN_PD4 */
  #ifdef PIN_PD4
    pinMode(PIN_PD4, OUTPUT);
    pinMode(PIN_PD4, INPUT);
    pinMode(PIN_PD4, INPUT_PULLUP);
    digitalWrite(PIN_PD4, HIGH);
    digitalWrite(PIN_PD4, LOW);
    openDrain(PIN_PD4, LOW);
    openDrain(PIN_PD4, FLOATING);
    digitalRead(PIN_PD4);
    analogWrite(PIN_PD4, 0);
    analogWrite(PIN_PD4, 128);
    analogWrite(PIN_PD4, 255);
    digitalWriteFast(PIN_PD4, HIGH);
    digitalWriteFast(PIN_PD4, LOW);
    openDrainFast(PIN_PD4, LOW);
    openDrainFast(PIN_PD4, FLOATING);
    digitalReadFast(PIN_PD4);
  #endif

  /* Test PIN_PD5 */
  #ifdef PIN_PD5
    pinMode(PIN_PD5, OUTPUT);
    pinMode(PIN_PD5, INPUT);
    pinMode(PIN_PD5, INPUT_PULLUP);
    digitalWrite(PIN_PD5, HIGH);
    digitalWrite(PIN_PD5, LOW);
    openDrain(PIN_PD5, LOW);
    openDrain(PIN_PD5, FLOATING);
    digitalRead(PIN_PD5);
    analogWrite(PIN_PD5, 0);
    analogWrite(PIN_PD5, 128);
    analogWrite(PIN_PD5, 255);
    digitalWriteFast(PIN_PD5, HIGH);
    digitalWriteFast(PIN_PD5, LOW);
    openDrainFast(PIN_PD5, LOW);
    openDrainFast(PIN_PD5, FLOATING);
    digitalReadFast(PIN_PD5);
  #endif

  /* Test PIN_PD6 */
  #ifdef PIN_PD6
    pinMode(PIN_PD6, OUTPUT);
    pinMode(PIN_PD6, INPUT);
    pinMode(PIN_PD6, INPUT_PULLUP);
    digitalWrite(PIN_PD6, HIGH);
    digitalWrite(PIN_PD6, LOW);
    openDrain(PIN_PD6, LOW);
    openDrain(PIN_PD6, FLOATING);
    digitalRead(PIN_PD6);
    analogWrite(PIN_PD6, 0);
    analogWrite(PIN_PD6, 128);
    analogWrite(PIN_PD6, 255);
    digitalWriteFast(PIN_PD6, HIGH);
    digitalWriteFast(PIN_PD6, LOW);
    openDrainFast(PIN_PD6, LOW);
    openDrainFast(PIN_PD6, FLOATING);
    digitalReadFast(PIN_PD6);
  #endif

  /* Test PIN_PD7 */
  #ifdef PIN_PD7
    pinMode(PIN_PD7, OUTPUT);
    pinMode(PIN_PD7, INPUT);
    pinMode(PIN_PD7, INPUT_PULLUP);
    digitalWrite(PIN_PD7, HIGH);
    digitalWrite(PIN_PD7, LOW);
    openDrain(PIN_PD7, LOW);
    openDrain(PIN_PD7, FLOATING);
    digitalRead(PIN_PD7);
    analogWrite(PIN_PD7, 0);
    analogWrite(PIN_PD7, 128);
    analogWrite(PIN_PD7, 255);
    digitalWriteFast(PIN_PD7, HIGH);
    digitalWriteFast(PIN_PD7, LOW);
    openDrainFast(PIN_PD7, LOW);
    openDrainFast(PIN_PD7, FLOATING);
    digitalReadFast(PIN_PD7);
  #endif

  /* Test PIN_PE0 */
  #ifdef PIN_PE0
    pinMode(PIN_PE0, OUTPUT);
    pinMode(PIN_PE0, INPUT);
    pinMode(PIN_PE0, INPUT_PULLUP);
    digitalWrite(PIN_PE0, HIGH);
    digitalWrite(PIN_PE0, LOW);
    openDrain(PIN_PE0, LOW);
    openDrain(PIN_PE0, FLOATING);
    digitalRead(PIN_PE0);
    analogWrite(PIN_PE0, 0);
    analogWrite(PIN_PE0, 128);
    analogWrite(PIN_PE0, 255);
    digitalWriteFast(PIN_PE0, HIGH);
    digitalWriteFast(PIN_PE0, LOW);
    openDrainFast(PIN_PE0, LOW);
    openDrainFast(PIN_PE0, FLOATING);
    digitalReadFast(PIN_PE0);
  #endif

  /* Test PIN_PE1 */
  #ifdef PIN_PE1
    pinMode(PIN_PE1, OUTPUT);
    pinMode(PIN_PE1, INPUT);
    pinMode(PIN_PE1, INPUT_PULLUP);
    digitalWrite(PIN_PE1, HIGH);
    digitalWrite(PIN_PE1, LOW);
    openDrain(PIN_PE1, LOW);
    openDrain(PIN_PE1, FLOATING);
    digitalRead(PIN_PE1);
    analogWrite(PIN_PE1, 0);
    analogWrite(PIN_PE1, 128);
    analogWrite(PIN_PE1, 255);
    digitalWriteFast(PIN_PE1, HIGH);
    digitalWriteFast(PIN_PE1, LOW);
    openDrainFast(PIN_PE1, LOW);
    openDrainFast(PIN_PE1, FLOATING);
    digitalReadFast(PIN_PE1);
  #endif

  /* Test PIN_PE2 */
  #ifdef PIN_PE2
    pinMode(PIN_PE2, OUTPUT);
    pinMode(PIN_PE2, INPUT);
    pinMode(PIN_PE2, INPUT_PULLUP);
    digitalWrite(PIN_PE2, HIGH);
    digitalWrite(PIN_PE2, LOW);
    openDrain(PIN_PE2, LOW);
    openDrain(PIN_PE2, FLOATING);
    digitalRead(PIN_PE2);
    analogWrite(PIN_PE2, 0);
    analogWrite(PIN_PE2, 128);
    analogWrite(PIN_PE2, 255);
    digitalWriteFast(PIN_PE2, HIGH);
    digitalWriteFast(PIN_PE2, LOW);
    openDrainFast(PIN_PE2, LOW);
    openDrainFast(PIN_PE2, FLOATING);
    digitalReadFast(PIN_PE2);
  #endif

  /* Test PIN_PE3 */
  #ifdef PIN_PE3
    pinMode(PIN_PE3, OUTPUT);
    pinMode(PIN_PE3, INPUT);
    pinMode(PIN_PE3, INPUT_PULLUP);
    digitalWrite(PIN_PE3, HIGH);
    digitalWrite(PIN_PE3, LOW);
    openDrain(PIN_PE3, LOW);
    openDrain(PIN_PE3, FLOATING);
    digitalRead(PIN_PE3);
    analogWrite(PIN_PE3, 0);
    analogWrite(PIN_PE3, 128);
    analogWrite(PIN_PE3, 255);
    digitalWriteFast(PIN_PE3, HIGH);
    digitalWriteFast(PIN_PE3, LOW);
    openDrainFast(PIN_PE3, LOW);
    openDrainFast(PIN_PE3, FLOATING);
    digitalReadFast(PIN_PE3);
  #endif

  /* Test PIN_PE4 */
  #ifdef PIN_PE4
    pinMode(PIN_PE4, OUTPUT);
    pinMode(PIN_PE4, INPUT);
    pinMode(PIN_PE4, INPUT_PULLUP);
    digitalWrite(PIN_PE4, HIGH);
    digitalWrite(PIN_PE4, LOW);
    openDrain(PIN_PE4, LOW);
    openDrain(PIN_PE4, FLOATING);
    digitalRead(PIN_PE4);
    analogWrite(PIN_PE4, 0);
    analogWrite(PIN_PE4, 128);
    analogWrite(PIN_PE4, 255);
    digitalWriteFast(PIN_PE4, HIGH);
    digitalWriteFast(PIN_PE4, LOW);
    openDrainFast(PIN_PE4, LOW);
    openDrainFast(PIN_PE4, FLOATING);
    digitalReadFast(PIN_PE4);
  #endif

  /* Test PIN_PE5 */
  #ifdef PIN_PE5
    pinMode(PIN_PE5, OUTPUT);
    pinMode(PIN_PE5, INPUT);
    pinMode(PIN_PE5, INPUT_PULLUP);
    digitalWrite(PIN_PE5, HIGH);
    digitalWrite(PIN_PE5, LOW);
    openDrain(PIN_PE5, LOW);
    openDrain(PIN_PE5, FLOATING);
    digitalRead(PIN_PE5);
    analogWrite(PIN_PE5, 0);
    analogWrite(PIN_PE5, 128);
    analogWrite(PIN_PE5, 255);
    digitalWriteFast(PIN_PE5, HIGH);
    digitalWriteFast(PIN_PE5, LOW);
    openDrainFast(PIN_PE5, LOW);
    openDrainFast(PIN_PE5, FLOATING);
    digitalReadFast(PIN_PE5);
  #endif

  /* Test PIN_PE6 */
  #ifdef PIN_PE6
    pinMode(PIN_PE6, OUTPUT);
    pinMode(PIN_PE6, INPUT);
    pinMode(PIN_PE6, INPUT_PULLUP);
    digitalWrite(PIN_PE6, HIGH);
    digitalWrite(PIN_PE6, LOW);
    openDrain(PIN_PE6, LOW);
    openDrain(PIN_PE6, FLOATING);
    digitalRead(PIN_PE6);
    analogWrite(PIN_PE6, 0);
    analogWrite(PIN_PE6, 128);
    analogWrite(PIN_PE6, 255);
    digitalWriteFast(PIN_PE6, HIGH);
    digitalWriteFast(PIN_PE6, LOW);
    openDrainFast(PIN_PE6, LOW);
    openDrainFast(PIN_PE6, FLOATING);
    digitalReadFast(PIN_PE6);
  #endif

  /* Test PIN_PE7 */
  #ifdef PIN_PE7
    pinMode(PIN_PE7, OUTPUT);
    pinMode(PIN_PE7, INPUT);
    pinMode(PIN_PE7, INPUT_PULLUP);
    digitalWrite(PIN_PE7, HIGH);
    digitalWrite(PIN_PE7, LOW);
    openDrain(PIN_PE7, LOW);
    openDrain(PIN_PE7, FLOATING);
    digitalRead(PIN_PE7);
    analogWrite(PIN_PE7, 0);
    analogWrite(PIN_PE7, 128);
    analogWrite(PIN_PE7, 255);
    digitalWriteFast(PIN_PE7, HIGH);
    digitalWriteFast(PIN_PE7, LOW);
    openDrainFast(PIN_PE7, LOW);
    openDrainFast(PIN_PE7, FLOATING);
    digitalReadFast(PIN_PE7);
  #endif

  /* Test PIN_PF0 */
  #ifdef PIN_PF0
    pinMode(PIN_PF0, OUTPUT);
    pinMode(PIN_PF0, INPUT);
    pinMode(PIN_PF0, INPUT_PULLUP);
    digitalWrite(PIN_PF0, HIGH);
    digitalWrite(PIN_PF0, LOW);
    openDrain(PIN_PF0, LOW);
    openDrain(PIN_PF0, FLOATING);
    digitalRead(PIN_PF0);
    analogWrite(PIN_PF0, 0);
    analogWrite(PIN_PF0, 128);
    analogWrite(PIN_PF0, 255);
    digitalWriteFast(PIN_PF0, HIGH);
    digitalWriteFast(PIN_PF0, LOW);
    openDrainFast(PIN_PF0, LOW);
    openDrainFast(PIN_PF0, FLOATING);
    digitalReadFast(PIN_PF0);
  #endif

  /* Test PIN_PF1 */
  #ifdef PIN_PF1
    pinMode(PIN_PF1, OUTPUT);
    pinMode(PIN_PF1, INPUT);
    pinMode(PIN_PF1, INPUT_PULLUP);
    digitalWrite(PIN_PF1, HIGH);
    digitalWrite(PIN_PF1, LOW);
    openDrain(PIN_PF1, LOW);
    openDrain(PIN_PF1, FLOATING);
    digitalRead(PIN_PF1);
    analogWrite(PIN_PF1, 0);
    analogWrite(PIN_PF1, 128);
    analogWrite(PIN_PF1, 255);
    digitalWriteFast(PIN_PF1, HIGH);
    digitalWriteFast(PIN_PF1, LOW);
    openDrainFast(PIN_PF1, LOW);
    openDrainFast(PIN_PF1, FLOATING);
    digitalReadFast(PIN_PF1);
  #endif

  /* Test PIN_PF2 */
  #ifdef PIN_PF2
    pinMode(PIN_PF2, OUTPUT);
    pinMode(PIN_PF2, INPUT);
    pinMode(PIN_PF2, INPUT_PULLUP);
    digitalWrite(PIN_PF2, HIGH);
    digitalWrite(PIN_PF2, LOW);
    openDrain(PIN_PF2, LOW);
    openDrain(PIN_PF2, FLOATING);
    digitalRead(PIN_PF2);
    analogWrite(PIN_PF2, 0);
    analogWrite(PIN_PF2, 128);
    analogWrite(PIN_PF2, 255);
    digitalWriteFast(PIN_PF2, HIGH);
    digitalWriteFast(PIN_PF2, LOW);
    openDrainFast(PIN_PF2, LOW);
    openDrainFast(PIN_PF2, FLOATING);
    digitalReadFast(PIN_PF2);
  #endif

  /* Test PIN_PF3 */
  #ifdef PIN_PF3
    pinMode(PIN_PF3, OUTPUT);
    pinMode(PIN_PF3, INPUT);
    pinMode(PIN_PF3, INPUT_PULLUP);
    digitalWrite(PIN_PF3, HIGH);
    digitalWrite(PIN_PF3, LOW);
    openDrain(PIN_PF3, LOW);
    openDrain(PIN_PF3, FLOATING);
    digitalRead(PIN_PF3);
    analogWrite(PIN_PF3, 0);
    analogWrite(PIN_PF3, 128);
    analogWrite(PIN_PF3, 255);
    digitalWriteFast(PIN_PF3, HIGH);
    digitalWriteFast(PIN_PF3, LOW);
    openDrainFast(PIN_PF3, LOW);
    openDrainFast(PIN_PF3, FLOATING);
    digitalReadFast(PIN_PF3);
  #endif

  /* Test PIN_PF4 */
  #ifdef PIN_PF4
    pinMode(PIN_PF4, OUTPUT);
    pinMode(PIN_PF4, INPUT);
    pinMode(PIN_PF4, INPUT_PULLUP);
    digitalWrite(PIN_PF4, HIGH);
    digitalWrite(PIN_PF4, LOW);
    openDrain(PIN_PF4, LOW);
    openDrain(PIN_PF4, FLOATING);
    digitalRead(PIN_PF4);
    analogWrite(PIN_PF4, 0);
    analogWrite(PIN_PF4, 128);
    analogWrite(PIN_PF4, 255);
    digitalWriteFast(PIN_PF4, HIGH);
    digitalWriteFast(PIN_PF4, LOW);
    openDrainFast(PIN_PF4, LOW);
    openDrainFast(PIN_PF4, FLOATING);
    digitalReadFast(PIN_PF4);
  #endif

  /* Test PIN_PF5 */
  #ifdef PIN_PF5
    pinMode(PIN_PF5, OUTPUT);
    pinMode(PIN_PF5, INPUT);
    pinMode(PIN_PF5, INPUT_PULLUP);
    digitalWrite(PIN_PF5, HIGH);
    digitalWrite(PIN_PF5, LOW);
    openDrain(PIN_PF5, LOW);
    openDrain(PIN_PF5, FLOATING);
    digitalRead(PIN_PF5);
    analogWrite(PIN_PF5, 0);
    analogWrite(PIN_PF5, 128);
    analogWrite(PIN_PF5, 255);
    digitalWriteFast(PIN_PF5, HIGH);
    digitalWriteFast(PIN_PF5, LOW);
    openDrainFast(PIN_PF5, LOW);
    openDrainFast(PIN_PF5, FLOATING);
    digitalReadFast(PIN_PF5);
  #endif

  /* Test PIN_PG0 */
  #ifdef PIN_PG0
    pinMode(PIN_PG0, OUTPUT);
    pinMode(PIN_PG0, INPUT);
    pinMode(PIN_PG0, INPUT_PULLUP);
    digitalWrite(PIN_PG0, HIGH);
    digitalWrite(PIN_PG0, LOW);
    openDrain(PIN_PG0, LOW);
    openDrain(PIN_PG0, FLOATING);
    digitalRead(PIN_PG0);
    analogWrite(PIN_PG0, 0);
    analogWrite(PIN_PG0, 128);
    analogWrite(PIN_PG0, 255);
    digitalWriteFast(PIN_PG0, HIGH);
    digitalWriteFast(PIN_PG0, LOW);
    openDrainFast(PIN_PG0, LOW);
    openDrainFast(PIN_PG0, FLOATING);
    digitalReadFast(PIN_PG0);
  #endif

  /* Test PIN_PG1 */
  #ifdef PIN_PG1
    pinMode(PIN_PG1, OUTPUT);
    pinMode(PIN_PG1, INPUT);
    pinMode(PIN_PG1, INPUT_PULLUP);
    digitalWrite(PIN_PG1, HIGH);
    digitalWrite(PIN_PG1, LOW);
    openDrain(PIN_PG1, LOW);
    openDrain(PIN_PG1, FLOATING);
    digitalRead(PIN_PG1);
    analogWrite(PIN_PG1, 0);
    analogWrite(PIN_PG1, 128);
    analogWrite(PIN_PG1, 255);
    digitalWriteFast(PIN_PG1, HIGH);
    digitalWriteFast(PIN_PG1, LOW);
    openDrainFast(PIN_PG1, LOW);
    openDrainFast(PIN_PG1, FLOATING);
    digitalReadFast(PIN_PG1);
  #endif

  /* Test PIN_PG2 */
  #ifdef PIN_PG2
    pinMode(PIN_PG2, OUTPUT);
    pinMode(PIN_PG2, INPUT);
    pinMode(PIN_PG2, INPUT_PULLUP);
    digitalWrite(PIN_PG2, HIGH);
    digitalWrite(PIN_PG2, LOW);
    openDrain(PIN_PG2, LOW);
    openDrain(PIN_PG2, FLOATING);
    digitalRead(PIN_PG2);
    analogWrite(PIN_PG2, 0);
    analogWrite(PIN_PG2, 128);
    analogWrite(PIN_PG2, 255);
    digitalWriteFast(PIN_PG2, HIGH);
    digitalWriteFast(PIN_PG2, LOW);
    openDrainFast(PIN_PG2, LOW);
    openDrainFast(PIN_PG2, FLOATING);
    digitalReadFast(PIN_PG2);
  #endif

  /* Test PIN_PG3 */
  #ifdef PIN_PG3
    pinMode(PIN_PG3, OUTPUT);
    pinMode(PIN_PG3, INPUT);
    pinMode(PIN_PG3, INPUT_PULLUP);
    digitalWrite(PIN_PG3, HIGH);
    digitalWrite(PIN_PG3, LOW);
    openDrain(PIN_PG3, LOW);
    openDrain(PIN_PG3, FLOATING);
    digitalRead(PIN_PG3);
    analogWrite(PIN_PG3, 0);
    analogWrite(PIN_PG3, 128);
    analogWrite(PIN_PG3, 255);
    digitalWriteFast(PIN_PG3, HIGH);
    digitalWriteFast(PIN_PG3, LOW);
    openDrainFast(PIN_PG3, LOW);
    openDrainFast(PIN_PG3, FLOATING);
    digitalReadFast(PIN_PG3);
  #endif

  /* Test PIN_PG4 */
  #ifdef PIN_PG4
    pinMode(PIN_PG4, OUTPUT);
    pinMode(PIN_PG4, INPUT);
    pinMode(PIN_PG4, INPUT_PULLUP);
    digitalWrite(PIN_PG4, HIGH);
    digitalWrite(PIN_PG4, LOW);
    openDrain(PIN_PG4, LOW);
    openDrain(PIN_PG4, FLOATING);
    digitalRead(PIN_PG4);
    analogWrite(PIN_PG4, 0);
    analogWrite(PIN_PG4, 128);
    analogWrite(PIN_PG4, 255);
    digitalWriteFast(PIN_PG4, HIGH);
    digitalWriteFast(PIN_PG4, LOW);
    openDrainFast(PIN_PG4, LOW);
    openDrainFast(PIN_PG4, FLOATING);
    digitalReadFast(PIN_PG4);
  #endif

  /* Test PIN_PG5 */
  #ifdef PIN_PG5
    pinMode(PIN_PG5, OUTPUT);
    pinMode(PIN_PG5, INPUT);
    pinMode(PIN_PG5, INPUT_PULLUP);
    digitalWrite(PIN_PG5, HIGH);
    digitalWrite(PIN_PG5, LOW);
    openDrain(PIN_PG5, LOW);
    openDrain(PIN_PG5, FLOATING);
    digitalRead(PIN_PG5);
    analogWrite(PIN_PG5, 0);
    analogWrite(PIN_PG5, 128);
    analogWrite(PIN_PG5, 255);
    digitalWriteFast(PIN_PG5, HIGH);
    digitalWriteFast(PIN_PG5, LOW);
    openDrainFast(PIN_PG5, LOW);
    openDrainFast(PIN_PG5, FLOATING);
    digitalReadFast(PIN_PG5);
  #endif

  /* Test PIN_PG6 */
  #ifdef PIN_PG6
    pinMode(PIN_PG6, OUTPUT);
    pinMode(PIN_PG6, INPUT);
    pinMode(PIN_PG6, INPUT_PULLUP);
    digitalWrite(PIN_PG6, HIGH);
    digitalWrite(PIN_PG6, LOW);
    openDrain(PIN_PG6, LOW);
    openDrain(PIN_PG6, FLOATING);
    digitalRead(PIN_PG6);
    analogWrite(PIN_PG6, 0);
    analogWrite(PIN_PG6, 128);
    analogWrite(PIN_PG6, 255);
    digitalWriteFast(PIN_PG6, HIGH);
    digitalWriteFast(PIN_PG6, LOW);
    openDrainFast(PIN_PG6, LOW);
    openDrainFast(PIN_PG6, FLOATING);
    digitalReadFast(PIN_PG6);
  #endif

  /* Test PIN_PG7 */
  #ifdef PIN_PG7
    pinMode(PIN_PG7, OUTPUT);
    pinMode(PIN_PG7, INPUT);
    pinMode(PIN_PG7, INPUT_PULLUP);
    digitalWrite(PIN_PG7, HIGH);
    digitalWrite(PIN_PG7, LOW);
    openDrain(PIN_PG7, LOW);
    openDrain(PIN_PG7, FLOATING);
    digitalRead(PIN_PG7);
    analogWrite(PIN_PG7, 0);
    analogWrite(PIN_PG7, 128);
    analogWrite(PIN_PG7, 255);
    digitalWriteFast(PIN_PG7, HIGH);
    digitalWriteFast(PIN_PG7, LOW);
    openDrainFast(PIN_PG7, LOW);
    openDrainFast(PIN_PG7, FLOATING);
    digitalReadFast(PIN_PG7);
  #endif

  /* Test PIN_PF6 */
  #ifdef PIN_PF6
    pinMode(PIN_PF6, OUTPUT);
    pinMode(PIN_PF6, INPUT);
    pinMode(PIN_PF6, INPUT_PULLUP);
    digitalWrite(PIN_PF6, HIGH);
    digitalWrite(PIN_PF6, LOW);
    openDrain(PIN_PF6, LOW);
    openDrain(PIN_PF6, FLOATING);
    digitalRead(PIN_PF6);
    analogWrite(PIN_PF6, 0);
    analogWrite(PIN_PF6, 128);
    analogWrite(PIN_PF6, 255);
    digitalWriteFast(PIN_PF6, HIGH);
    digitalWriteFast(PIN_PF6, LOW);
    openDrainFast(PIN_PF6, LOW);
    openDrainFast(PIN_PF6, FLOATING);
    digitalReadFast(PIN_PF6);
  #endif

  /* Test PIN_PF7 */
  #ifdef PIN_PF7
    pinMode(PIN_PF7, OUTPUT);
    pinMode(PIN_PF7, INPUT);
    pinMode(PIN_PF7, INPUT_PULLUP);
    digitalWrite(PIN_PF7, HIGH);
    digitalWrite(PIN_PF7, LOW);
    openDrain(PIN_PF7, LOW);
    openDrain(PIN_PF7, FLOATING);
    digitalRead(PIN_PF7);
    analogWrite(PIN_PF7, 0);
    analogWrite(PIN_PF7, 128);
    analogWrite(PIN_PF7, 255);
    digitalWriteFast(PIN_PF7, HIGH);
    digitalWriteFast(PIN_PF7, LOW);
    openDrainFast(PIN_PF7, LOW);
    openDrainFast(PIN_PF7, FLOATING);
    digitalReadFast(PIN_PF7);
  #endif
  for (uint8_t pin = 0; pin < NUM_DIGITAL_PINS; pin++) {
    #if (defined(MEGATINYCORE) && CLOCK_SOURCE == 2)
      if (pin   != PIN_PA3)
    #else
      #if (CLOCK_SOURCE == 2)
        if (pin != PIN_PA0)
      #elif (CLOCK_SOURCE == 1)
        if (pin != PIN_PA0 && pin != PIN_PA1)
      #endif
    #endif
    {
      pinMode(pin, OUTPUT);
      pinMode(pin, INPUT);
      pinMode(pin, INPUT_PULLUP);
      digitalWrite(pin, HIGH);
      digitalWrite(pin, LOW);
      openDrain(pin, LOW);
      openDrain(pin, FLOATING);
      digitalRead(pin);
      analogWrite(pin, 0);
      analogWrite(pin, 128);
      analogWrite(pin, 255);
    }
  }
}
