#include "Wire.h"



//Idea: TWI0 -> TWI(0x10) -> TWI1(0x20) -> TWI(0x30) -> TWI0(0x40)
//DA: 0xxB
//Tiny: 0xxA
void rxFunction(int numBytes);

uint8_t wire0_data[64];
uint8_t wire1_data[64];
uint8_t wire0_len = 0;
uint8_t wire1_len = 0;

void setup() {
  Wire.begin();  //Enable Master
  Wire.begin(0x10, false, (0x30 <<  1) | 1);   //Addr for Wire0: 0x10 - Addr for Wire1: 0x30
  Wire.onReceive(rxFunction);
}

void loop() {
  if (wire0_len > 0) {
    Wire.beginTransmission(0x20);
    Wire.write(wire0_data, wire0_len);
    Wire.endTransmission(true);
    wire0_len = 0;
  }
  if (wire1_len > 0) {
    Wire.beginTransmission(0x40);
    Wire.write(wire1_data, wire1_len);
    Wire.endTransmission(true);
    wire1_len = 0;
  }

}

void rxFunction(int numBytes) {
  uint8_t j = Wire.available();
  if (Wire.getIncomingAddress() == (0x10 << 1)) {   //Data from Wire0
    uint8_t i = 0;
    for (; i < j; i++) {
      wire0_data[i] = Wire.read();
    }
    wire0_len = i;
  } else if (Wire.getIncomingAddress() == (0x30 << 1)) {   //Data from Wire1
    uint8_t i = 0;
    for (; i < j; i++) {
      wire1_data[i] = Wire.read();
    }
    wire1_len = i;
  }
}
