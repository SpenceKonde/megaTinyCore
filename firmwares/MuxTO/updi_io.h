/*
   updi_io.h

   Created: 18-11-2017 10:38:31
    Author: JMR_2
*/


#ifndef UPDI_IO_H_
#define UPDI_IO_H_

namespace UPDI_io {
// Enums
enum ctrl {single_break, double_break, enable};

// Function prototypes
uint8_t put(char) __attribute__((optimize("no-tree-loop-optimize")));
uint8_t put(ctrl);
uint8_t get() __attribute__((optimize("no-tree-loop-optimize")));
void init(void);
}

#endif /* UPDI_IO_H_ */
