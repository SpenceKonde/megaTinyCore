/*
   stk_io.cpp

   Created: 18-11-2017 15:20:29
    Author: JMR_2
*/

// Includes
#include <Arduino.h>
#include "JICE_io.h"
#include "sys.h"

namespace {
// *** Baud rate lookup table for UBRR0 register ***
// Indexed by valid values for PARAM_BAUD_RATE_VAL (defined in JTAG2.h)
uint16_t baud_tbl[8] = {baud(2400), baud(4800), baud(9600), baud(19200), baud(38400), baud(57600), baud(115200), baud(14400)};
}

// Functions
uint8_t JICE_io::put(char c) {
  SERIALCOM.write(c);
  return c;
}

uint8_t JICE_io::get(void) {
  //while (!SERIALCOM.available());
  uint8_t c = SERIALCOM.read();
  return c;
}

void JICE_io::init(void) {
  SERIALCOM.begin(115200);
}

void JICE_io::flush(void) {
  SERIALCOM.flush();
}

void JICE_io::set_baud(JTAG2::baud_rate rate) {

}
