/*
   UPDI_hi_lvl.h

   Created: 15-02-2018 23:12:54
    Author: JMR_2
*/


#ifndef UPDI_HI_LVL_H_
#define UPDI_HI_LVL_H_

#include "UPDI_lo_lvl.h"

namespace UPDI {
// Constant Expressions
constexpr uint8_t RESET_ON = 0x59;
constexpr uint8_t RESET_OFF = 0x00;

// Function prototypes
void CPU_reset();

// returns the current CPU mode, optionally a mask can be applied to ignore "don't care" status bits
template <uint8_t mask = 0xFF>
uint8_t CPU_mode() {
  uint8_t mode = UPDI::lcds(UPDI::reg::ASI_System_Status);
  return mode & mask;
}
}

#endif /* UPDI_HI_LVL_H_ */
