/*
   stk_io.h

   Created: 18-11-2017 14:55:53
    Author: JMR_2
*/


#ifndef JICE_IO_H_
#define JICE_IO_H_

#include <stdint.h>
#include "JTAG2.h"

#warning "modify this to match your USB serial port name"
#define SERIALCOM SerialUSB

namespace JICE_io {
// Function prototypes
uint8_t put(char c);
uint8_t get(void);
void init(void);
void flush(void);
void set_baud(JTAG2::baud_rate rate);
}

#endif /* JICE_IO_H_ */
