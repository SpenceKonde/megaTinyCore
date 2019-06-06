/*
   NVM.h

   Created: 15-12-2017 10:59:53
    Author: JMR_2
*/


#ifndef NVM_H_
#define NVM_H_

#include <stdint.h>
#include "UPDI_lo_lvl.h"

namespace NVM {
// *** Base Addresses ***
enum base {
  NVM_base	= 0x1000,		/* Base address of the NVM controller */
  Sig_base	= 0x1100,		/* Base address of the signature */
  Fuse_base	= 0x1280,		/* Base address of the fuses */
  User_base	= 0x1300,		/* Base address of the User Row EEPROM */
  EEPROM_base	= 0x1400		/* Base address of the main EEPROM */
};

// *** NVM Registers (offsets from NVN_base are enum default values) ***
enum reg {
  CTRLA,
  CTRLB,
  STATUS,
  INTCTRL,
  INTFLAGS,
  Reg_5,
  DATA_lo,
  DATA_hi,
  ADDR_lo,
  ADDR_hi
};

// *** NVM Commands (write to CTRLA to execute) ***
enum cmnd {
  NOP,			/* Does nothing */
  WP,				/* Write page buffer to memory */
  ER,				/* Erase page */
  ERWP,			/* Erase and write page */
  PBC,			/* Page buffer clear */
  CHER,			/* Chip erase: erase Flash and EEPROM */
  EEER,			/* EEPROM Erase */
  WFU				/* Write fuse */
};

// *** NVM Functions ***
template <bool preserve_ptr>
void command (uint8_t cmd) {
  uint16_t temp;
  /* preserve UPDI pointer if requested */
  if (preserve_ptr) temp = UPDI::ldptr_w();
  /* Execute NVM command */
  UPDI::sts_b(NVM_base + CTRLA, cmd);
  /* restore UPDI pointer if requested */
  if (preserve_ptr) UPDI::stptr_w(temp);
}

template <bool preserve_ptr>
void wait () {
  uint16_t temp;
  /* preserve UPDI pointer if requested */
  if (preserve_ptr) temp = UPDI::ldptr_w();
  /* Wait while NVM is busy from previous operations */
  while (UPDI::lds_b(NVM::NVM_base + NVM::STATUS) & 0x03);
  /* restore UPDI pointer if requested */
  if (preserve_ptr) UPDI::stptr_w(temp);
}
}

#endif /* NVM_H_ */
