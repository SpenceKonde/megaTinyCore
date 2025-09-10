// Many cores use HardwareSerial.h and HardwareSerial. However, the core from which this was derived names them differently.
// This only rarely a problem - but it causes problems when libraries want to #include HardwareSerial.h and expect it to provide a class of that name, instead of UartClass. Soon after this workaround, it was realized that there was an extra layer in the class hierarchy forcing multiple virtual functions and explained why serial was a space hog

/* UART.h - Hardware serial library, main header.
 * This library is free software released under LGPL 2.1.
 * See License.md for more information.
 * This file is part of DxCore and/or megaTinyCore
 *
 * Copyright (c) 2006 Nicholas Zambetti, Modified by
 * 11/23/2006 David A. Mellis, 9/20/2010 Mark Sproul,
 * 8/24/2012 Alarus, 12/3/2013 Matthijs Kooijman
 * Others (unknown) 2013-2017, 2017-2022 Spence Konde
 * and 2021-2022 MX682X
 *
 * Modified 28 September 2010 by Mark Sproul
 * Modified 14 August 2012 by Alarus
 * Modified 3 December 2013 by Matthijs Kooijman
 * Modified by SOMEONE around 2016-2017; hardware serial didn't port itself to the megaAVR 0-series.
 * Modified 2017-2021 by Spence Konde for megaTinyCore and DxCore.
 * Modified late 2021 by Spence Konde and MX682X for DxCore
 * 12/26/21: Correct bug introduced in my ASM macros. -Spence
 * 12/30/21: Clean up tests for conditional compilation
             tests for defined(USE_ASM_*) removed UART.h tests that test both defined and the value of
             USE_ASM_* macros. We check if they're defined and define them if they're not defined already
             so whenever UART.h has been loaded, those three macros are defined as either 1, or wharever
             value the user overode them with, likely 0. Also high byte of UART address always 0x08, so replace
             2-clock ldd with 1 clock ldi. - Spence
 * 03/12/23: Correct bug in TxD1' and XCK1'
 * 07/10/25: Correct bug causing strange compilation failures in ModBus and possibly other conditions.
*/

#pragma once
#include "core_devices.h"
#include <inttypes.h>
#include "api/Stream.h"
#include "pins_arduino.h"
#include "UART_constants.h"
#include "UART_check_pins.h"

// No UART_swap.h on megaTinyCore - there isn't enough to put in separate file.

/* Define constants and variables for buffering incoming serial data.  We're
 * using a ring buffer, in which head is the index of the location to which
 * to write the next incoming character and tail is the index of the
 * location from which to read.
 * NOTE: a "power of 2" buffer size is **REQUIRED** - the compiler
 * was missing optimizations, and there's no particular reason to have
 * a weird sized buffer, and several reasons not to.
 *
 * More than 256b buffers imposes a considerable performance penalty,
 * - one large enough to obliviate the entire purpose, because of the
 * need to make the access to the current index atomic. This atomic
 * block is costly - it's a macro for cli and sei
 * implemented in inline assembly, which sounds fast. But the optimizer
 * can reorder instructions *and isn't smart enough not to here* without the
 *  ": memory" clobber to create a memory barrier. This ensures that it
 * is atomic, but significantly hurts performance. (theoretical worst case
 * is 94 clocks, real-world is usually far less, but I'll only say "less"
 * The functions in question have considerable register pressure). But,
 * it unquestionably would impact USART performance at high speeds.
 *
 * * The USE_ASM_* options can be disabled by defining them as 0 either in variant pins_arduino.h
 * The buffer sizes can be overridden in by defining SERIAL_TX_BUFFER either in variant file (
 * as defines in pins_arduino.h) or boards.txt as (By passing them as extra flags).
 * Note that buffer sizes must be powers of 2 only no matter how you override it.
 * The alternative is doing division to get the modulo every time instead of a single clock bitwise and, which further lowers the
 * maximum RX speed (which, without the ASM implementation, is already lower than what the receiver is capable of).
 * The defaults below are only used if the relevant macro isn't already defined.
 * Since the USE_ASM_* = 1 option is apparently working, we do not recommend disabling it, as it will waste flash and hurt performance.
 *
 * Flash versus RAM table
 * |       |  modern tinyAVR series parts   | Other modern parts          |      |
 * | Flash | 0-series | 1-series | 2-series | mega | All Dx |  EA  |  EB  |  SD  |
 * |-------|----------|----------|----------|------|--------|------|------|------|
 * |  2048 |      128 |      128 |       -  |   -  |     -  |   -  |   -  |   -  |
 * |  4096 |      256 |      256 |      512 |   -  |     -  |   -  |   -  |   -  |
 * |  8192 |      512 |      512 |     1024 | 1024 |     -  |   -  |   -  |   -  |
 * | 16384 |     1024 |     2048 |     2048 | 2048 |   2048 | 2048 | 2048 |   -  |
 * | 32768 |       -  |     2048 |     3072 | 4096 |   4096 | 4096 | 3072 | 4096 |
 * | 49152 |       -  |       -  |       -  | 6120 |     -  |   -  |   -  |   -  |
 * | 65536 |       -  |       -  |       -  |   -  |   8192 | 6120 |   -  | 8182 |
 * |  128k |       -  |       -  |       -  |   -  |  16384 |   -  |   -  |   -  |
 * This ratio is remarkably consistent. No AVR part was ever made with less than 8:1 flash:ram,
 * nor more than 16:1, since the earliest recognizable AVRs. I am only aware of one exception. Was it some bizarro part
 * from the dark ages? Nope - it's the surprisingly popular ATmega2560!
 * The ATmega2560/2561 has only 8k RAM, a 32:1 flash to ram ratio. (to be fair, you are allowed to use external RAM
 * on those, which was a very rare feature indeed, and that is by far the most widespread part with such a feature - though if you're using the
 * XMEM interface, you've burned 19 GPIO lines right there.... The ATmega2560 is kind of the "I have a job too big for an AVR.
 * But I don't know how to program anything else!" part. That is not a compliment.
 *
 * |  RAM  | TX | RX | Amount of RAM implied | Total ram used |
 * |-------|----|----|-----------------------|----------------|
 * | < 512 | 16 | 16 | 256b (0/1 w/2k or 4k) | 32b, all 1 port|
 * | <1024 | 16 | 32 | 512b                  | 48b or 96b     |
 * | <2048 | 32 | 64 | 1024b                 | 96b or 192b    |
 * | More  | 64 | 64 | 2048b or 3072b        | 128b or 256b   |
 *
 * (the two numbers in final column are given because 0/1-serieas has 1 port, but tiny2 has 2, though if you only use one, you only
 * get one set of buffers)
 *
 * The rationale behind those numbers is simple: The ring buffer code has calculate a modulo (several hundred clock cycles, and it would have to do it in an ISR - yeah, not good.
 * But if you cheat by having a power of two size of the buffer, you can do the calculation in a low single digit number of clocks. So it's gotta be a power of 2.
 * And RX iis obviously more important than TX for the buffer size, because a TX underrun will not lose your data, just slow down transmission, while a RX overrun means lost data
 * But TX isn't unimportant, because of the importance of serial logging to Arduino development. It is very easily to, in an attempt to log enough information to debug the problem,
 * force the CPU to spend most of it's time waiting for a chance to put the next byte of log output into the buffer, which is full.
 */
/* Buffer Sizing */

// The buffer sizes can be overridden in by defining SERIAL_TX_BUFFER either in variant file (as defines in pins_arduino.h) or boards.txt (By passing them as extra flags).
// note that buffer sizes must be powers of 2 only.

#if !defined(SERIAL_TX_BUFFER_SIZE)
  #if   (INTERNAL_SRAM_SIZE  < 1024)  // 128/256b/512b RAM
    #define SERIAL_TX_BUFFER_SIZE 16
  #elif (INTERNAL_SRAM_SIZE < 2048)   // 1k RAM
    #define SERIAL_TX_BUFFER_SIZE 32
  #else
    #define SERIAL_TX_BUFFER_SIZE 64  // 2k/3k RAM
  #endif
#endif
#if !defined(SERIAL_RX_BUFFER_SIZE)   // could be overridden by boards.txt
  #if   (INTERNAL_SRAM_SIZE <  512)  // 128/256b RAM
    #define SERIAL_RX_BUFFER_SIZE 16
    // current tx buffer position = SerialClass + txtail + 37
  #elif (INTERNAL_SRAM_SIZE < 1024)  // 512b RAM
    #define SERIAL_RX_BUFFER_SIZE 32
    // current tx buffer position = SerialClass + txtail + 53
  #else
    #define SERIAL_RX_BUFFER_SIZE 64  // 1k+ RAM
    // current tx buffer position = SerialClass + txtail + 85
    // rx buffer position always = SerialClass + rxhead + 21
  #endif
#endif
/* Use INTERNAL_SRAM_SIZE instead of RAMEND - RAMSTART, which is vulnerable to
 * a fencepost error. */
#if (SERIAL_TX_BUFFER_SIZE > 256)
  typedef uint16_t tx_buffer_index_t;
#else
  typedef uint8_t  tx_buffer_index_t;
#endif
// I am not convinced > 256b is safe for the RX buffer....
#if (SERIAL_RX_BUFFER_SIZE > 256)
  typedef uint16_t rx_buffer_index_t;
#else
  typedef uint8_t  rx_buffer_index_t;
#endif
// As noted above, forcing the sizes to be a power of two saves a small
// amount of flash, and there's no compelling reason to NOT have them be
// a power of two. If this is a problem, since you're already modifying
// core, change the lines in UART.cpp where it does & (SERIAL_xX_BUFFERLSIZE-1)
// and replace them with % SERIAL_xX_BUFFER_SIZE; where xX is TX or RX.
// There are two of each, and the old ending of the line is even commented
// out at the end of the line.
#if (SERIAL_TX_BUFFER_SIZE & (SERIAL_TX_BUFFER_SIZE - 1))
  #error "ERROR: TX buffer size must be a power of two."
#endif
#if (SERIAL_RX_BUFFER_SIZE & (SERIAL_RX_BUFFER_SIZE - 1))
  #error "ERROR: RX buffer size must be a power of two."
#endif

/* Buffer sizing done */
/* DANGER DANGER DANGER */
/* The ASM implementations push the envelope, *hard*. The asm implementations themselves are only slightly better than what the compiler makes. That improvement does not justify the asm.
 * The magic is all from a couple of tricks which, if you look at them carefully, all turn out to rely on one specific trick. That trick, strictly speaking, is illegal; we are explicitly
 * told in the compiler documentation that you cannot use labels in one asm statement in another asm statement. This (as well as the more performant attachInterrupt and the Flash.h library
 * when not using optiboot, with full flash write enabled) rely on being able to do this, because we have 3 identical functions which must be present for each of the up to 6 serial ports
 * announced and released products - these fuinctions in total waste 202 bytes of flash per serial instance, with code that is identical except for a single constant./
 */
#if !defined(LTODISABLED)
#if !defined(USE_ASM_TXC)
  #define USE_ASM_TXC 2    // A bit slower than 1 in exchange for halfduplex.
//#define USE_ASM_TXC 1    // This *appears* to work? It's the easy one. saves 6b for 1 USART and 44b for each additional one
#endif

#if !defined(USE_ASM_RXC)
  #define USE_ASM_RXC 1    // This now works. Saves only 4b for 1 usart but 98 for each additional one
#endif

#if !defined(USE_ASM_DRE)
  #define USE_ASM_DRE 1    // This is the hard one...Depends on BOTH buffers, and has that other method of calling it. saves 34b for 1 USART and 68b for each additional one
#endif
#else
  #warning "LTO has been disabled! ASM TXC/RXC/DRE not available. USART falling back to the old, flash-inefficient implementation with fewer features."
  #if defined(USE_ASM_TXC)
    #undef USE_ASM_TXC
  #endif

  #if defined(USE_ASM_RXC)
    #undef USE_ASM_RXC
  #endif

  #if defined(USE_ASM_DRE)
    #undef USE_ASM_DRE
  #endif
#endif


// savings:
// 44 total for 0/1,
// 301 for 2-series, which may be nearly 9% of the total flash!
// The USE_ASM_* options can be disabled by defining them as 0 (in the same way that buffer sizes can be overridden)
// The buffer sizes can be overridden in by defining SERIAL_TX_BUFFER either in variant file (as defines in pins_arduino.h) or boards.txt (By passing them as extra flags).
// note that buffer sizes must be powers of 2 only.

#if USE_ASM_RXC == 1 && !(SERIAL_RX_BUFFER_SIZE == 256 || SERIAL_RX_BUFFER_SIZE == 128 || SERIAL_RX_BUFFER_SIZE == 64 || SERIAL_RX_BUFFER_SIZE == 32 || SERIAL_RX_BUFFER_SIZE == 16)
  #error "Assembly RX Complete (RXC) ISR is only supported when RX buffer size are 256, 128, 64, 32 or 16 bytes"
#endif

#if USE_ASM_DRE == 1 && !((SERIAL_RX_BUFFER_SIZE == 256 || SERIAL_RX_BUFFER_SIZE == 128 || SERIAL_RX_BUFFER_SIZE == 64 || SERIAL_RX_BUFFER_SIZE == 32 || SERIAL_RX_BUFFER_SIZE == 16) && \
                          (SERIAL_TX_BUFFER_SIZE == 256 || SERIAL_TX_BUFFER_SIZE == 128 || SERIAL_TX_BUFFER_SIZE == 64 || SERIAL_TX_BUFFER_SIZE == 32 || SERIAL_TX_BUFFER_SIZE == 16))
  #error "Assembly Data Register Empty (DRE) ISR is only supported when both TX and RX buffer sizes are 256, 128, 64, 32 or 16 bytes"
#endif


/* Macros to help the rare few who want sync or MSPI mode */
#define syncBegin(port, baud, config, syncopts) ({\
  if ((config & 0xC0) == 0x40)                    \
    {pinConfigure(port.getPin(2), syncopts);      \
    port.begin(baud >> 3, config);                \
  }})

#define mspiBegin(port, baud, config, invert) ({  \
  if ((config & 0xC0) == 0xC0) {                  \
    pinConfigure(port.getPin(2), invert);         \
    port.begin(baud >> 3, config);                \
  }})



// tinyAVR 0/1-series has 2 bits devoted to RS485, supporting normal (00), RS485 with XDIR driven to control
// an external line driver (01), and some other mysterious mode (10) the function of which is unclear. There is
// evidence that this poorly documented feature is also present in other hardware, and was only removed on paper.
#if !defined(USART_RS485_bm)
  #if defined(USART_RS485_0_bm)
    #define USART_RS485_bm USART_RS485_0_bm
  #else
    #define USART_RS485_bm 0x01
  #endif
#endif
#if defined(__AVR_ATtinyxy2__) // 8-pin parts use a different set of pin mappings.
const uint8_t _usart_pins[][4] = {{PIN_PA6, PIN_PA7, PIN_PA3, PIN_PA0},{PIN_PA1, PIN_PA2, NOT_A_PIN, NOT_A_PIN}};
#elif !defined(__AVR_ATtinyx26__) && !defined(__AVR_ATtinyx27__) // Everything that's not a 2-series with >= 20 pins has the standard mappings.
const uint8_t _usart_pins[][4] = {{PIN_PB2, PIN_PB3, PIN_PB1, PIN_PB0},{PIN_PA1, PIN_PA2, PIN_PA3, PIN_PA4}};
#elif defined(__AVR_ATtinyx26__) || defined(__AVR_ATtinyx27__) // 2-series with 20 or 24 pins have the alt pins for USART1.
const uint8_t _usart_pins[][4] = {
  {PIN_PB2, PIN_PB3, PIN_PB1, PIN_PB0},
  {PIN_PA1, PIN_PA2, PIN_PA3, PIN_PA4},
  {PIN_PC2, PIN_PC1, PIN_PC1, PIN_PC3}
};
#else
  #error "This can't happen - it doesn't have 8, 14, 20 or 24 pins, or it has 14 pins but no serial port - defines aren't being picked up correctly."
#endif
/*
#if defined(__AVR_ATtinyxy2__)
const uint8_t _usart_pins[][7] = {{PIN_PA6, PIN_PA7, PIN_PA0, PIN_PA3},{PIN_PA1, PIN_PA2, NOT_A_PIN, NOT_A_PIN}};
#elif !defined(__AVR_ATtinyx26__) && !defined(__AVR_ATtinyx27__) && defined(MEGATINYCORE_SERIES)
const uint8_t _usart_pins[][7] = {{4, 8, 2, 1, 0x22, 0x23, 0x20},{2, 4, 8, 16, 0x01, 0x02, 0x04}};
#elif defined(__AVR_ATtinyx26__) || defined(__AVR_ATtinyx27__)
const uint8_t _usart_pins[][7] = {
  {4, 8, 2, 1, 0x22, 0x23, 0x20},
  {2, 4, 8, 16, 0x01, 0x02, 0x04},
  {1, 2, 4, 8, 0x40, 0x41, 0x43}
};
*/
#define SERIAL_PIN_SETS 2
// WHAT THE HELL IS WRONG WITH THIS?!
// When this definition is used, row 0 is {PIN_PB3,PIN_PB2,PIN_PB1,PIN_PB0 in this file and row 2 is 0, 0, 0, 0! WTF}
// TX, RX, XCK, XDIR
/*
const uint8_t _usart_pins[][4] = {
  #if defined(HWSERIAL0_MUX)
    #if (defined(PIN_HWSERIAL0_TX) && defined(PIN_HWSERIAL0_RX) && defined(PIN_HWSERIAL0_XCK) && defined(PIN_HWSERIAL0_XIR) && PIN_HWSERIAL0_TX != NOT_A_PIN && PIN_HWSERIAL0_RX != NOT_A_PIN)
      {PIN_HWSERIAL0_TX, PIN_HWSERIAL0_RX, PIN_HWSERIAL0_XCK, PIN_HWSERIAL0_XDIR},
    #else
      {NOT_A_PIN, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN},
    #endif
  #endif
  #if defined(HWSERIAL0_MUX_PINSWAP_1)
    #if (defined(PIN_HWSERIAL0_TX_PINSWAP_1) && defined(PIN_HWSERIAL0_RX_PINSWAP_1) && defined(PIN_HWSERIAL0_XCK_PINSWAP_1) && defined(PIN_HWSERIAL0_XIR_PINSWAP_1) && PIN_HWSERIAL0_TX_PINSWAP_1 != NOT_A_PIN && PIN_HWSERIAL0_RX_PINSWAP_1 != NOT_A_PIN)
      {PIN_HWSERIAL0_TX_PINSWAP_1, PIN_HWSERIAL0_RX_PINSWAP_1, PIN_HWSERIAL0_XCK_PINSWAP_1, PIN_HWSERIAL0_XDIR_PINSWAP_1},
    #else
      {NOT_A_PIN, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN},
    #endif
  #endif
  #if defined(USART1) // On 0/1-series, with only one USART, we don't even need the third row.
    #if defined(HWSERIAL1_MUX_PINSWAP_1)
      #if (defined(PIN_HWSERIAL1_TX_PINSWAP_1) && defined(PIN_HWSERIAL1_RX_PINSWAP_1) && defined(PIN_HWSERIAL1_XCK_PINSWAP_1) && defined(PIN_HWSERIAL1_XIR_PINSWAP_1) && PIN_HWSERIAL1_TX_PINSWAP_1 != NOT_A_PIN && PIN_HWSERIAL1_RX_PINSWAP_1 != NOT_A_PIN)
        {PIN_HWSERIAL1_TX_PINSWAP_1, PIN_HWSERIAL1_RX_PINSWAP_1, PIN_HWSERIAL1_XCK_PINSWAP_1, PIN_HWSERIAL1_XDIR_PINSWAP_1},
      #else
        {NOT_A_PIN, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN},
      #endif
    #endif
  #endif
};
*/



/* DANGER DANGER DANGER */
/* CHANGING THE MEMBER VARIABLES BETWEEN HERE AND THE OTHER SCARY COMMENT WILL COMPLETELY BREAK SERIAL
 * WHEN USE_ASM_DRE and/or USE_ASM_RXC is used! */
/* DANGER DANGER DANGER */
class HardwareSerial : public Stream {
  protected:
    volatile USART_t *const _hwserial_module;
    const uint8_t _module_number;
    uint8_t _pin_set;

    volatile uint8_t _state; /* 0bvV__fphw */
    // Of course this has to be volatile! We started writing it in 2.6.x from ISRs in opaque asm.
    // Bit E ("There's an echo in here") is used if halfduplex mode is enabled, and when RXC reads the buffer,
    // it also reads state, and if this bit is set, clears the bit and exits the ISR. The opposite of a parity
    // error, which sets it's bit and exits the isr except that it doesn't care if it's already set) .
    // v = Overflow has happened at hardware level. Interrupts were disabled for too long.
    // V = ring buffer overflow has occurred.
    // _ = reserved
    // f = One or more framing errors have occurred.
    // p = One or more parity errors has occurred.
    // h = half duplex with open drain - disable RX while TX.
    // w = written (like old _written)

    volatile rx_buffer_index_t _rx_buffer_head;
    volatile rx_buffer_index_t _rx_buffer_tail;
    volatile tx_buffer_index_t _tx_buffer_head;
    volatile tx_buffer_index_t _tx_buffer_tail;

    // Don't put any members after these buffers, since only the first
    // 32 bytes of this struct can be accessed quickly using the ldd
    // instruction.
    volatile uint8_t _rx_buffer[SERIAL_RX_BUFFER_SIZE];
    volatile uint8_t _tx_buffer[SERIAL_TX_BUFFER_SIZE];
/* DANGER DANGER DANGER */
/* ANY CHANGES BETWEEN OTHER SCARY COMMENT AND THIS ONE WILL BREAK SERIAL IF THEY CHANGE RAM USED BY CLASS! */
/* DANGER DANGER DANGER */

  public:
    inline             HardwareSerial(volatile USART_t *hwserial_module, uint8_t module_number, uint8_t default_pinset);
    bool                    pins(uint8_t tx, uint8_t rx);
    bool                    swap(uint8_t mux_level = 1);
    void                   begin(uint32_t baud) {begin(baud, SERIAL_8N1);}
    void                   begin(uint32_t baud, uint16_t options);
    void                     end();
    // printHex!
    void                  printHex(const     uint8_t              b);
    void                  printHex(const      int8_t  b)              {printHex((uint8_t )   b);           }
    void                  printHex(const        char  b)              {printHex((uint8_t )   b);           }
    void                  printHex(const    uint16_t  w, bool s = 0);
    void                  printHex(const     int16_t  w, bool s = 0)  {printHex((uint16_t)w, s);           }
    void                  printHex(const    uint32_t  l, bool s = 0);
    void                  printHex(const     int32_t  l, bool s = 0)  {printHex((uint32_t)l, s);           }
    void                printHexln(const      int8_t  b)              {printHex((uint8_t )   b); println();}
    void                printHexln(const        char  b)              {printHex((uint8_t )   b); println();}
    void                printHexln(const     uint8_t  b)              {printHex(             b); println();}
    void                printHexln(const    uint16_t  w, bool s = 0)  {printHex(          w, s); println();}
    void                printHexln(const    uint32_t  l, bool s = 0)  {printHex(          l, s); println();}
    void                printHexln(const     int16_t  w, bool s = 0)  {printHex((uint16_t)w, s); println();}
    void                printHexln(const     int32_t  l, bool s = 0)  {printHex((uint32_t)l, s); println();}
    // The pointer-versions for mass printing uint8_t and uint16_t arrays.
    uint8_t *             printHex(          uint8_t* p, uint8_t len, char sep = 0            );
    uint16_t *            printHex(         uint16_t* p, uint8_t len, char sep = 0, bool s = 0);
    volatile uint8_t *    printHex(volatile  uint8_t* p, uint8_t len, char sep = 0            );
    volatile uint16_t *   printHex(volatile uint16_t* p, uint8_t len, char sep = 0, bool s = 0);
    uint8_t *           printHexln(          uint8_t* p, uint8_t len, char sep = 0            ) {
      uint8_t* ret;
      ret=printHex(p, len, sep);
      println(); return ret;
    }
    uint16_t *          printHexln(         uint16_t* p, uint8_t len, char sep = 0, bool s = 0) {
      uint16_t* ret;  ret=printHex(p, len, sep, s); println(); return ret;
    }
    volatile uint8_t *  printHexln(volatile  uint8_t* p, uint8_t len, char sep = 0            ) {
      volatile uint8_t* ret;
      ret=printHex(p, len, sep);
      println();
      return ret;
    }
    volatile uint16_t * printHexln(volatile uint16_t* p, uint8_t len, char sep = 0, bool s = 0) {
        volatile uint16_t* ret;
        ret=printHex(p, len, sep, s);
        println();
        return ret;
      }

    virtual int availableForWrite(void);
    virtual int available(void);
    virtual      int peek(void);
    virtual      int read(void);
    virtual    void flush(void);
    virtual  size_t write(uint8_t ch);
    inline   size_t write(unsigned long n)  {return write((uint8_t)n);}
    inline   size_t write(long n)           {return write((uint8_t)n);}
    inline   size_t write(unsigned int n)   {return write((uint8_t)n);}
    inline   size_t write(int n)            {return write((uint8_t)n);}
    using Print::write; // pull in write(str) and write(buf, size) from Print
    explicit operator bool() {
      return true;
    }
    uint8_t autoBaudWFB();
    void simpleSync();

    uint8_t autobaudWFB_and_wait(uint8_t n);
    uint8_t waitForSync();
    uint8_t autobaudWFB_and_request(uint8_t n = 2);
    uint8_t getStatus() {
      uint8_t ret = _statuscheck(_hwserial_module->CTRLB, _hwserial_module->STATUS, _state);
      if ((ret & 0x30) == 0x30) {
        _hwserial_module->STATUS = USART_ISFIF_bm;
        #if defined(ERRATA_ISFIF)
          uint8_t ctrlb _hwserial_module->CTRLB;
          uint8_t rxoff = ctrlb & (~USART_RXEN_bm);
          _hwserial_module->CTRLB = rxoff;
          _hwserial_module->CTRLB = ctrlb;
        #endif
      }
      _state &= 0x03; // Clear the errors we just reported.
      return ret;
    }

    uint8_t getPin(uint8_t pin); //wrapper around static _getPin

    // Interrupt handlers - Not intended to be called externally
    #if !(USE_ASM_RXC == 1 && \
         (SERIAL_RX_BUFFER_SIZE == 256 || SERIAL_RX_BUFFER_SIZE == 128 || SERIAL_RX_BUFFER_SIZE == 64 || SERIAL_RX_BUFFER_SIZE == 32 || SERIAL_RX_BUFFER_SIZE == 16))
      static void _rx_complete_irq(HardwareSerial& uartClass);
    #endif
    #if !(USE_ASM_DRE == 1 && \
         (SERIAL_RX_BUFFER_SIZE == 256 || SERIAL_RX_BUFFER_SIZE == 128 || SERIAL_RX_BUFFER_SIZE == 64 || SERIAL_RX_BUFFER_SIZE == 32 || SERIAL_RX_BUFFER_SIZE == 16) && \
         (SERIAL_TX_BUFFER_SIZE == 256 || SERIAL_TX_BUFFER_SIZE == 128 || SERIAL_TX_BUFFER_SIZE == 64 || SERIAL_TX_BUFFER_SIZE == 32 || SERIAL_TX_BUFFER_SIZE == 16))
      static void _tx_data_empty_irq(HardwareSerial& uartClass);
    #endif

  private:
    void _poll_tx_data_empty(void);
    static void _set_pins(uint8_t port_num, uint8_t mux_setting, uint8_t enmask);
    static uint8_t _pins_to_swap(uint8_t port_num, uint8_t tx_pin, uint8_t rx_pin);


    /* _statuscheck() - the static side to getStatus(). Static methods have no concept of which instance they are called from. This gives the optimizer more handholds
     * As you probably know, the optimizer's hands are pretty tightly bound when working with a normal class method, but it has a much freer hand in static methods.
     * Return value is:
     * 0HRaaFPHW
     * H = Hardware RX buffer overflowed because interrupts were disabled for too long while trying to receive data.
     * R = Ring buffer suffered an overflow. When you check Serial.available, use a while loop to read in characters until nothing is left in the ring buffer.
     * aa = Autobaud state:
     * 00 = Disabled
     * 01 = SERIAL_AUTOBAUD_ENABLED
     * 10 = SERIAL_AUTOBAUD_SYNC
     * 11 = SERIAL_AUTOBAUD_BADSYNC - This is Bad News, bro. Baaaaaad news. You tried to do autobaud, but got something that wasn't a sync, and some parts have an errata
     *      that results in the receiver disabling itself until you clear this, turn off RXEN, and turn it back on. This ugly kludge is implemented by getStatus, which calls
     *      this function. Currently only DD parts have this erratum, but I won't trust that until I've seen more recent errata that don't list it (my prediction is anything
     *      with the SFDEN bug has the ISFIF one too, which would be most or all modern AVRs)
     * F = SERIAL_FRAME_ERROR - A framing error has occurred since you last called this, indicating mismatched baud settings. See Serial Reference.
     * P = SERIAL_PARITY_ERROR - A parity error has occurred since you last called this, and the bad character did not make it to the application.
     *     When using parity mode, but mismatched baud settings, you will get a mixture of gibberish + framing error set, and
     *     parity errors.
     * H = SERIAL_HALF_DUPLEX_ENABLED - indicates that half duplex mode is enabled.
     *
     * Test for these errors like st = Serial.getStatus(); if (st & SERIAL_FRAME_ERROR) { ... }
     * For the autobaud ones with 2 bits: if (st & SERIAL_AUTOBAUD_BADSYNC) { // If it wasn't fixed by the core, you'd have to take action. But it is}
     */
    static uint8_t _statuscheck(uint8_t ctrlb, uint8_t status, uint8_t state) {
      uint8_t ret = state;
      // We have now: |HW OVF| RING OVF|_____|_______|FrameError|ParityError|HalfDuplex|Written|
      // now we fill high nybble
      if ((ctrlb & 0x06) == 0x04) {
        // Autobaud
        if (status & USART_BDF_bm) {
          ret |= SERIAL_AUTOBAUD_SYNC;
        } else if (status & USART_ISFIF_bm) {
          ret |= SERIAL_AUTOBAUD_BADSYNC;
        } else {
          ret |= SERIAL_AUTOBAUD_ENABLED;
        }
      }
      return ret;
    }
  };

#if defined(USART0)
  extern HardwareSerial Serial0;
#endif
#if defined(USART1)
  extern HardwareSerial Serial1;
#endif
