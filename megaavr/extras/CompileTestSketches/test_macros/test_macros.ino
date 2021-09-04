void setup() {
  // put your setup code here, to run once:
  volatile long testvar = min((uint8_t) GPIOR0, (uint8_t) GPIOR1);
  testvar = max((uint8_t) GPIOR0, (uint8_t) GPIOR1);
  testvar = constrain(500 + GPIOR0, 300 + GPIOR1, 400 + GPIOR2);
  testvar = radians((uint8_t) GPIOR0);
  testvar = degrees((uint8_t) GPIOR0);
  testvar = sq((uint8_t) GPIOR0);
  testvar = round((int8_t) GPIOR0);
  GPIOR3 = testvar;
  volatile uint8_t testvar2;
  testvar2 = lowByte((uint16_t) (GPIOR0) * (uint16_t) (GPIOR1));
  testvar2 = highByte((uint16_t) (GPIOR0) * (uint16_t) (GPIOR1));
  testvar2 = bitRead((uint8_t) GPIOR0, (uint8_t) GPIOR1 & 0x07);
  bitSet(testvar2, (uint8_t) GPIOR1 & 0x07);
  bitClear(testvar2, (uint8_t) GPIOR1 & 0x07);
  bitWrite(testvar2, (uint8_t) GPIOR1 & 0x07, (uint8_t) GPIOR2 & 0x01);
  testvar2 =  bit((uint8_t) GPIOR1 & 0x07);
}

void loop() {
  // put your main code here, to run repeatedly:

}
