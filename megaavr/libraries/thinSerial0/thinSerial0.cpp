void thinPrint(const char x) {
  while (!(USART0.STATUS & USART_UDRIF_bm));
  USART0.STATUS   = USART_TXCIF_bm; // make sure it's clear.
  USART0.TXDATAL  = x;
}
void thinPrintln() {
  thinPrint('\r');
  thinPrint('\n');
}
void thinPrint(const char* chars) {
  while (*chars) {
    thinPrint(*chars++);
  }
}
void thinPrint(const uint16_t number) {
  thinPrint((uint8_t)  (number >>  8));
  thinPrint((uint8_t)  (number >>  0));
}
void thinPrint(const int16_t number) {
  thinPrint((uint16_t) (number      ));
}

void thinPrint(const uint32_t number) {
  thinPrint((uint8_t)  (number >> 24));
  thinPrint((uint8_t)  (number >> 16));
  thinPrint((uint8_t)  (number >>  8));
  thinPrint((uint8_t)  (number >>  0));
}

void thinPrint(const int32_t number) {
  thinPrint((uint32_t) number);
}

void thinPrint(const uint8_t number) {
  char x = (number >> 4) | '0';
  if (x > '9')
    x += 7;
  thinPrint(x);
  x = (number & 0x0F) | '0';
  if (x > '9')
    x += 7;
  thinPrint(x);
}

void thinFlush() {
  while(!(USART0.STATUS & USART_TXCIF_bm));
  // wait for transfer to complete.
}
void thinBegin(uint16_t baud, uint8_t mode, uint8_t options) {
  if (!(__builtin_constant_p(baud) && __builtin_constant_p(mode) && __builtin_constant_p(options))) {
    badArg("All arguments must be constants known at compile time");
  }
  // since baud and mode are compile time known constants, all this logic is done at compile time.
  if (mode == 0) {
    openDrainFast(PIN_HWSERIAL0_TX, LOW); // dirty trick to get pinModeFast(pin,OUTPUT);
    openDrainFast(PIN_HWSERIAL0_RX, FLOATING); // combine with next line for pinModeFast(pin,INPUT_PULLUP)
    volatile uint8_t *pin_ctrl = (volatile uint8_t *) (0x410 + digital_pin_to_port[PIN_HWSERIAL0_RX] * 0x20 + digital_pin_to_bit_position[PIN_HWSERIAL0_RX]);
    *pin_ctrl = PORT_PULLUPEN_bm;
  } else if (mode == 1) {
    openDrainFast(PIN_HWSERIAL0_TX_PINSWAP_1, LOW); // dirty trick to get a pinModeFast(pin,OUTPUT);
    openDrainFast(PIN_HWSERIAL0_RX_PINSWAP_1, FLOATING); // combine with next line for pinModeFast(pin,INPUT_PULLUP)
    volatile uint8_t *pin_ctrl = (volatile uint8_t *) (0x410 + digital_pin_to_port[PIN_HWSERIAL0_RX_PINSWAP_1] * 0x20 + digital_pin_to_bit_position[PIN_HWSERIAL0_RX_PINSWAP_1]);
    *pin_ctrl = PORT_PULLUPEN_bm;
  }
  // Those should be rendered as sbi, cbi, cbi, cbi sts - 5 words. Serial has to be able to do it at runtime, making it far slower and more flash intensive.

  uint32_t baudu2x = (64*F_CPU / (8*baud))
  if (baudu2x > (131070)) {
    badArg("Baud rate too low")
  } else if (baudu2x  > 65535) {
    USART0.BAUD = (64*F_CPU / (16*baud))
  } else if (baudu2x < 64) {
    badArg("baud rate too high")
  } else {
    USART0.BAUD = (64*F_CPU / (8*baud))
  }
  // ldi ldi sts sts - 6 words
}
void thinEnd() {

}

int16_t thinRead() {
  int16_t res = 0;
  __asm__ __volatile__ (
  "thin0start:"             "\n\t" // start of loop
    "lds r0, 0x0804"        "\n\t" // USART0.STATUS 3 clocks
    "sbrc r0,7"             "\n\t" // if there's data here we can execute the rjmp below, if not 2 clocks.
    "rjmp gotch0"           "\n\t" // jump over end of the loop, we found one (normally skipped)
    "sbiw, %[res], 1"       "\n\t" // decrement the result counter, starts at 0, so decrementing it once and it won't be 0 anymore, will go around the whole 64k passes 2 clocks
    "breq thin0time"        "\n\t" // so when it rolls back to 0, we jump to end. 1 clock when not true.
    "rjmp thin0start"       "\n\t" // back to the start! 2 clocks =loop is 3 + 2 + 2 + 1 + 2 = 10 clocks, so timeout is 655360 system clocks. We don't disable interrupts, so it could be longer, ofc.
  "gotch0:"                 "\n\t" // got character
    "clr %A[res]"           "\n\t" // clear top half
    "lds %B[res], 0x0800"   "\n\t" // read TXDATAL into low half
    "rjmp .+2"              "\n\t" // skip next instruction
  "thin0time:"              "\n\t"
    "sbiw, %[res], 1"              // get here from jump if we time out (so res = 0) Decrement it once more to -1.
    :[res] "+w" (res):); // 11 words of assembly, plus 1 for the return.
  return res; // now we return what is now either -1 or the contents of TXDATA;
}
