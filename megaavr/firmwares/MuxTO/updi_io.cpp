/*
   updi_io.cpp

   Created: 18-11-2017 10:36:54
    Author: JMR_2
*/

// Includes
#include <Arduino.h>
#include <avr/io.h>
#include "updi_io.h"

// Functions
/* Sends regular characters through the UPDI link */
uint8_t UPDI_io::put(char c) {
  Serial2.write(c);
  Serial2.flush();
  //delayMicroseconds(10);
  long start = millis();
  while (!Serial2.available() && millis() - start < 20) {}
  char d = Serial2.read();
  if (c != d) {
    // Serial.println("echo failed! " + String(d, HEX));
  }
  return c;
}

/* Sends special sequences through the UPDI link */
uint8_t UPDI_io::put(ctrl c)
{
  Serial2.begin(300, SERIAL_8N1);

  switch (c) {
    case double_break:
      Serial2.write((uint8_t)0x00);
      Serial2.flush();
      Serial2.write((uint8_t)0x00);
      Serial2.flush();
      break;
    case single_break:
      Serial2.write((uint8_t)0x00);
      Serial2.flush();
      break;
    default:
      break;
  }
  delay(15);
  while (Serial2.available()) {
    Serial2.read();
  }
  Serial2.begin(230400, SERIAL_8E2);
  return 0;
}

uint8_t UPDI_io::get() {
  uint8_t c;
  while (!Serial2.available()) {}
  c = Serial2.read();
  //delayMicroseconds(5);
  //Serial.println("get! " + String(c, HEX));
  return c;
}

void UPDI_io::init(void)
{
  Serial2.begin(230400, SERIAL_8E2);
  //Serial.begin(115200);
}
