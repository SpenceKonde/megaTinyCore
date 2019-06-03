/*
   UPDI_cmd.h

   Created: 23-11-2017 22:46:11
    Author: JMR_2
*/


#ifndef UPDI_LO_LVL_H_
#define UPDI_LO_LVL_H_

#include <stdint.h>
#include "sys.h"
#include "updi_io.h"

namespace UPDI {
// UPDI registers
enum reg {
  Status_A, Status_B, Control_A, Control_B,
  Reg_4, Reg_5, Reg_6, ASI_Key_Status,
  ASI_Reset_Request, ASI_Control_A, ASI_System_Control_A, ASI_System_Status,
  ASI_CRC_Status, Reg_13, Reg_14, Reg_15
};


// Constant Expressions
constexpr uint8_t SYNCH = 0x55;
constexpr uint8_t ACK = 0x40;

// Activation Keys
extern uint8_t Chip_Erase[8];
extern uint8_t NVM_Prog[8];
extern uint8_t UserRow_Write[8];


// Function prototypes
void rep(uint8_t);

void stcs(reg, uint8_t);
uint8_t lcds(reg);

void read_sib(uint8_t (&)[16]);

uint8_t lds_b(uint16_t);
uint16_t lds_w(uint16_t);

void sts_b(uint16_t address, uint8_t data); /* Store data at address */
void sts_w(uint16_t, uint16_t);

uint8_t ldptr_b();
uint16_t ldptr_w();
uint8_t ld_b();
uint16_t ld_w();
uint8_t ldinc_b();
uint16_t ldinc_w();

void stptr_b(uint8_t);
void stptr_w(uint16_t);
void st_b(uint8_t);
void st_w(uint16_t);
void stinc_b(uint8_t);
void stinc_w(uint16_t);

template <class T>
inline void write_key(T (& k)[8]) __attribute__(( optimize("no-tree-loop-optimize") ));

// Function Templates
template <class T>
void write_key(T (& k)[8]) {
  UPDI_io::put(SYNCH);
  UPDI_io::put(0xE0);
  T* key_ptr = k;
  for (uint8_t i = 8; i != 0; i--) {
    UPDI_io::put(*key_ptr);
    key_ptr++;
  }
}
}

#endif /* UPDI_LO_LVL_H_ */
