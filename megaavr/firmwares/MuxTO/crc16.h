/*
   crc16.h

   Created: 16-01-2018 23:07:05
    Author: JMR_2
*/


#ifndef CRC16_H_
#define CRC16_H_

#include <stdint.h>

namespace CRC {
uint16_t next (uint8_t newchar, uint16_t previous = 0xFFFF);		// 'previous' defaults to CRC seed value, 0xFFFF
}

#endif /* CRC16_H_ */
