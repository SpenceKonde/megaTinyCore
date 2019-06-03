/*
   UPDI_hi_lvl.cpp

   Created: 15-02-2018 23:08:39
    Author: JMR_2
*/

#include "UPDI_hi_lvl.h"

void UPDI::CPU_reset() {
  // Request reset
  UPDI::stcs(UPDI::reg::ASI_Reset_Request, UPDI::RESET_ON);
  // Release reset (System remains in reset state until released)
  UPDI::stcs(UPDI::reg::ASI_Reset_Request, UPDI::RESET_OFF);

  // Wait for the reset process to end.
  // Either NVMPROG, UROWPROG or BOOTDONE bit will be set in the ASI_SYS_STATUS UPDI register.
  // This indicates reset is complete.
  while ( UPDI::CPU_mode<0x0E>() == 0 );
}
