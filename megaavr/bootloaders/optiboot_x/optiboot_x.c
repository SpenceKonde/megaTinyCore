/**********************************************************/
/* Optiboot bootloader for Arduino                        */
/*                                                        */
/* http://optiboot.googlecode.com                         */
/*                                                        */
/* Arduino-maintained version : See README.TXT            */
/* http://code.google.com/p/arduino/                      */
/*  It is the intent that changes not relevant to the     */
/*  Arduino production environment get moved from the     */
/*  optiboot project to the arduino project in "lumps."   */
/*                                                        */
/* Heavily optimised bootloader that is faster and        */
/* smaller than the Arduino standard bootloader           */
/*                                                        */
/* Enhancements:                                          */
/*   Fits in 512 bytes, saving 1.5K of code space         */
/*   Higher baud rate speeds up programming               */
/*   Written almost entirely in C                         */
/*   Customisable timeout with accurate timeconstant      */
/*                                                        */
/* What you lose:                                         */
/*   Implements a skeleton STK500 protocol which is       */
/*     missing several features including EEPROM          */
/*     programming and non-page-aligned writes            */
/*   High baud rate breaks compatibility with standard    */
/*     Arduino flash settings                             */
/*                                                        */
/* Copyright 2013-2019 by Bill Westfield.                 */
/* Copyright 2010 by Peter Knight.                        */
/*                                                        */
/* This program is free software; you can redistribute it */
/* and/or modify it under the terms of the GNU General    */
/* Public License as published by the Free Software       */
/* Foundation; either version 2 of the License, or        */
/* (at your option) any later version.                    */
/*                                                        */
/* This program is distributed in the hope that it will   */
/* be useful, but WITHOUT ANY WARRANTY; without even the  */
/* implied warranty of MERCHANTABILITY or FITNESS FOR A   */
/* PARTICULAR PURPOSE.  See the GNU General Public        */
/* License for more details.                              */
/*                                                        */
/* You should have received a copy of the GNU General     */
/* Public License along with this program; if not, write  */
/* to the Free Software Foundation, Inc.,                 */
/* 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA */
/*                                                        */
/* Licence can be viewed at                               */
/* http://www.fsf.org/licenses/gpl.txt                    */
/*                                                        */
/**********************************************************/


/**********************************************************/
/*                                                        */
/* Optional defines:                                      */
/*                                                        */
/**********************************************************/
/*                                                        */
/* BIGBOOT:                                               */
/* Build a 1k bootloader, not 512 bytes. This turns on    */
/* extra functionality.                                   */
/*                                                        */
/* BAUD_RATE:                                             */
/* Set bootloader baud rate.                              */
/*                                                        */
/* LED_START_FLASHES:                                     */
/* Number of LED flashes on bootup.                       */
/*                                                        */
/* LED_DATA_FLASH:                                        */
/* Flash LED when transferring data. For boards without   */
/* TX or RX LEDs, or for people who like blinky lights.   */
/*                                                        */
/* TIMEOUT_MS:                                            */
/* Bootloader timeout period, in milliseconds.            */
/* 500, 1000, 2000, 4000, 8000 supported.                 */
/*                                                        */
/* UART:                                                  */
/* UART number (0..n) for devices with more than          */
/* one hardware uart (megaAVR 0 and tinyAVR 2-series)     */
/*                                                        */
/**********************************************************/

/**********************************************************/
/* Version Numbers!                                       */
/*                                                        */
/* Arduino Optiboot now includes this Version number in   */
/* the source and object code.                            */
/*                                                        */
/* Version 3 was released as zip from the optiboot        */
/*  repository and was distributed with Arduino 0022.     */
/* Version 4 starts with the arduino repository commit    */
/*  that brought the arduino repository up-to-date with   */
/*  the optiboot source tree changes since v3.            */
/*    :                                                   */
/* Version 9 splits off the Mega0/Xtiny support.          */
/*  This is very different from normal AVR because of     */
/*  changed peripherals and unified address space.        */
/*                                                        */
/* It would be good if versions implemented outside the   */
/*  official repository used an out-of-seqeunce version   */
/*  number (like 104.6 if based on based on 4.5) to       */
/*  prevent collisions.  The CUSTOM_VERSION=n option      */
/*  adds n to the high version to facilitate this.        */
/*                                                        */
/**********************************************************/

/**********************************************************/
/* Edit History:                                          */
/*                                                        */
/* Sep 2020                                               */
/* 9.1 fix do_nvmctrl                                     */
/* Aug 2019                                               */
/* 9.0 Refactored for Mega0/Xtiny from optiboot.c         */
/*   :                                                    */
/* 4.1 WestfW: put version number in binary.              */
/**********************************************************/
/* *INDENT-OFF* - astyle hates optiboot                   */
/**********************************************************/

#define OPTIBOOT_MAJVER 9
#define OPTIBOOT_MINVER 1

/*
   OPTIBOOT_CUSTOMVER should be defined (by the makefile) for custom edits
   of optiboot.  That way you don't wind up with very different code that
   matches the version number of a "released" optiboot.
*/

#if !defined(OPTIBOOT_CUSTOMVER)
  #define OPTIBOOT_CUSTOMVER 0x10
#endif

unsigned const int __attribute__((section(".version"))) __attribute__((used))
optiboot_version = 256 * (OPTIBOOT_MAJVER + OPTIBOOT_CUSTOMVER) + OPTIBOOT_MINVER;

#include <inttypes.h>
#include <avr/io.h>

#if (!defined(__AVR_XMEGA__)) || (__AVR_ARCH__ != 103)
  #error CPU not supported by this version of Optiboot.
  #include <unsupported>  // include a non-existent file to stop compilation
#endif

/*
   Fuses.
This block was a holdover and was not used in any way, because almost every fuses is controlled
by a tools submenu, and hence must not be embedded in the binary.

*/
/*
FUSES = {
  .WDTCFG = 0,
  .BODCFG = FUSE_BODCFG_DEFAULT,
  .OSCCFG = FREQSEL_20MHZ_gc,
  #ifdef FUSE_TCD0CFG_DEFAULT
  .TCD0CFG = FUSE_TCD0CFG_DEFAULT,
  #endif
  #ifdef RSTPIN
  .SYSCFG0 =  CRCSRC_NOCRC_gc | RSTPINCFG_RST_gc,
  #else
  # ifdef FUSE_RSTPINCFG_gm  // group mask will be defined for triple-func pins
  .SYSCFG0 =  CRCSRC_NOCRC_gc | RSTPINCFG_UPDI_gc,
  # else
  .SYSCFG0 =  CRCSRC_NOCRC_gc,
  # endif
  #endif
  .SYSCFG1 = 0x06,
  .APPEND = 0,
  .BOOTEND = 2
};
*/
/*
   optiboot uses several "address" variables that are sometimes byte pointers,
   sometimes word pointers. sometimes 16bit quantities, and sometimes built
   up from 8bit input characters.  avr-gcc is not great at optimizing the
   assembly of larger words from bytes, but we can use the usual union to
   do this manually.  Expanding it a little, we can also get rid of casts.
*/
typedef union {
  uint8_t  *bptr;
  uint16_t *wptr;
  uint16_t word;
  uint8_t bytes[2];
} addr16_t;


/*
   pin_defs.h
   This contains most of the rather ugly defines that implement our
   ability to use UART=n and LED=D3, and some avr family bit name differences.
*/
#include "pin_defs_x.h"

/*
   stk500.h contains the constant definitions for the stk500v1 comm protocol
*/
#include "stk500.h"

#ifndef LED_START_FLASHES
  #define LED_START_FLASHES 0
#endif

/*
   The mega-0, tiny-0, and tiny-1 chips all reset to running on the
    internal oscillator, with a prescaler of 6.  The internal oscillator
    is either 20MHz or 16MHz, depending on a fuse setting - we can read
    the fuse to figure our which.
   The BRG divisor is also fractional, permitting (afaik) any reasonable
    bit rate between about 1000bps and 1Mbps.
   This makes the BRG generation a bit different than for prior processors.
*/
/* set the UART baud rate defaults */
#ifndef BAUD_RATE
  #define BAUD_RATE   115200L // Highest rate Avrdude win32 will support
#endif
#ifdef F_CPU
  #warning F_CPU is ignored for this chip (run from internal osc.)
#endif
#ifdef SINGLESPEED
  #warning SINGLESPEED ignored for this chip.
#endif
#ifdef UART
  #warning UART is ignored for this chip (use UARTTX=PortPin instead)
#endif

#define BAUD_SETTING_16 (((16000000/6)*64) / (16L*BAUD_RATE))
#define BAUD_ACTUAL_16 ((64L*(16000000/6)) / (16L*BAUD_SETTING))
#define BAUD_SETTING_20 (((20000000/6)*64) / (16L*BAUD_RATE))
#define BAUD_ACTUAL_20 ((64L*(20000000/6)) / (16L*BAUD_SETTING))

#if BAUD_SETTING_16 < 64   // divisor must be > 1.  Low bits are fraction.
  #error Unachievable baud rate (too fast) BAUD_RATE
#endif

#if BAUD_SETTING > 65635
  #error Unachievable baud rate (too slow) BAUD_RATE
#endif // baud rate slow check

/*
   Watchdog timeout translations from human readable to config vals
*/
#ifndef WDTTIME
  #define WDTPERIOD WDT_PERIOD_1KCLK_gc  // 1 second
#elif WDTTIME == 1
  #define WDTPERIOD WDT_PERIOD_1KCLK_gc  // 1 second
#elif WDTTIME == 2
  #define WDTPERIOD WDT_PERIOD_2KCLK_gc  // 2 seconds
#elif WDTTIME == 4
  #define WDTPERIOD WDT_PERIOD_4KCLK_gc  // 4 seconds
#elif WDTTIME == 8
  #define WDTPERIOD WDT_PERIOD_8KCLK_gc  // 8 seconds
#else
#endif

/*
   We can never load flash with more than 1 page at a time, so we can save
   some code space on parts with smaller pagesize by using a smaller int.
*/
#if MAPPED_PROGMEM_PAGE_SIZE > 255
  typedef uint16_t pagelen_t;
  #define GETLENGTH(len) len = getch()<<8; len |= getch()
#else
  typedef uint8_t pagelen_t;
  #define GETLENGTH(len) (void) getch() /* skip high byte */; len = getch()
#endif


/* Function Prototypes
   The main() function is in init9, which removes the interrupt vector table
   we don't need. It is also 'OS_main', which means the compiler does not
   generate any entry or exit code itself (but unlike 'naked', it doesn't
   suppress some compile-time options we want.)
*/

void pre_main(void) __attribute__((naked)) __attribute__((section(".init8")));
int main(void) __attribute__((OS_main)) __attribute__((section(".init9"))) __attribute__((used));

void __attribute__((noinline)) __attribute__((leaf)) putch(char);
uint8_t __attribute__((noinline)) __attribute__((leaf)) getch(void) ;
void __attribute__((noinline)) verifySpace();
void __attribute__((noinline)) watchdogConfig(uint8_t x);

static void getNch(uint8_t);

#if LED_START_FLASHES > 0
  static inline void flash_led(uint8_t);
#endif

#define watchdogReset()  __asm__ __volatile__ ("wdr\n")

/*
   RAMSTART should be self-explanatory.  It's bigger on parts with a
   lot of peripheral registers.
   Note that RAMSTART (for optiboot) need not be exactly at the start of RAM.
*/
#if !defined(RAMSTART)  // newer versions of gcc avr-libc define RAMSTART
  #error RAMSTART not defined.
#endif

/* everything that needs to run VERY early */
void pre_main(void) {
  // Allow convenient way of calling do_spm function - jump table,
  //   so entry to this function will always be here, independent
  //    of compilation, features, etc
  __asm__ __volatile__(
    "       rjmp    1f\n"
#ifndef APP_NOSPM
    "       rjmp    do_nvmctrl\n"
#else
    "   ret\n"   // if do_spm isn't include, return without doing anything
#endif
    "1:\n"
  );
}

/* main program starts here */
int main(void) {
  uint8_t ch;

  /*
     Making these local and in registers prevents the need for initializing
     them, and also saves space because code no longer stores to memory.
     (initializing address keeps the compiler happy, but isn't really
      necessary, and uses 4 bytes of flash.)
  */
  register addr16_t address;
  register pagelen_t  length;

  // This is the first code to run.
  //
  // Optiboot C code makes the following assumptions:
  //  No interrupts will execute
  //  SP points to RAMEND
  //  4/24/21 Spence Konde:
  //  It *also* relies on the peripherals not being configured in an adverse way.
  //  Everything is written with the assumption that we start out after a reset.
  //  So what do we do when we can see that it isn't arriving from a reset, because RSTFR is 0?
  //
  //  Previously we just made sure not to try to jump to the app in that case.
  //  Largely because in non-reset-pin configurations, after running the bootloader once,
  //  instead of continually running the bootloader until something was uploaded, it would do it
  //  once, and then hit no entry conditions and jump to (the non-existent) app, run off the
  //  end of the flash, and repeat (discovered 11/14/20)
  //
  //  Now, ~unless we are told that we should ASSUME_DIRECT_ENTRY_SAFE~, when we detect something
  //  that could be either execution running past the end of an empty flash, a direct entry
  //  attempt, or some dreadful error condition, that is, when we find no reset cause we do
  //  what the app should have and fire a software reset. Net cost 10-12 bytes, but we have 32 left
  //  after that. As of 1/1/2023, I have removed ASSUME_DIRECT_ENTRY_SAFE, on the grounds that I don't
  //  think that even a tenth of a percent of users have any idea what they would need to do to make
  //  that be true. It's like an ASSUME_SKY_IS_GREEN option - yes it is theoretically possible -
  //  for example, near the poles, at night, under a bright Aurora, the sky may indeed be green.
  //  but it is an outlandish thing to assume. ASSUME_DIRECT_ENTRY_SAFE essentially translates as
  //  ASSUME_DEVELOPER_KNOWS_EVERY_REGISTER_USED_BY_OPTIBOOT_AND_RESET_THEM_ALL - which is even
  //  more outlandish than ASSUME_SKY_IS_GREEN, because the latter would be true without specific
  //  effort from the app writer under some conditions which will occur in practice. In contrast,
  //  the core guarantees that the former will never be true unless the developer has made a great
  //  effort here.
  //  This would take a considerably larger amount of flash relative to the small 8-10 byte footprint
  //  of a software reset.
  //
  //  The clock speed must be set to the POR default (div/6) (10 bytes)
  //  If the internal oscillator calibration was changed, that must be reset to the factory cal
  //  (or something that is closer than the factory cal to the nominal osc speed). (12 bytes,
  //  if this was ever changed)
  //  If an external clock was used, we must switch back to internal (10 bytes)
  //  The UART used by the bootloader must be made safe, meaning the following members for the UART
  //  struct: CTRLB (this gets written, but we need to start with it disabled), EVCTRL, TXPLCTRL
  //  RXPLCTRL should be zero'ed out, and STATUS should be read and written back. (16 bytes)
  //  Interrupts must be disabled. (2 bytes)
  //  PINnCTRL for the two pins must be zero'ed out if you had either pin inverted (2 bytes per)
  //  The jump takes 4 bytes.
  //  That means for overhead penalty of between 6 and 34 bytes added to app binary size, which is usable
  //  for other code, you would be able to....  ... enter the bootloader less robustly, and save 10 bytes
  //  in the bootloader, where you can't use it.
  //  I do belive the phrase "strictly worse" describes this.

  __asm__ __volatile__("clr __zero_reg__");  // known-zero required by avr-libc
  ch = RSTCTRL.RSTFR;   // get reset cause
  if (ch == 0) {
    _PROTECTED_WRITE(RSTCTRL_SWRR, 0x01);
  }
  #if !defined(ENTRYCOND_REQUIRE)
    #define ENTRYCOND_REQUIRE 0x35
  #endif
  if ((ch & 0x08) || !(ch & ENTRYCOND_REQUIRE)) {
    // Start the app.
    // Dont bother trying to stuff it in r2, which requires heroic effort to fish out
    // we'll put it in GPIOR0 where it won't get stomped on.
    //__asm__ __volatile__ ("mov r2, %0\n" :: "r" (ch));
    RSTCTRL.RSTFR = ch; //clear the reset causes before jumping to app...
    GPIOR0 = ch; // but, stash the reset cause in GPIOR0 for use by app...
    watchdogConfig(WDT_PERIOD_OFF_gc);
    __asm__ __volatile__(
      "jmp 0x0200\n\t");
  }

  watchdogReset();
  //    _PROTECTED_WRITE(CLKCTRL.MCLKCTRLB, 0);  // full speed clock

  MYUART_TXPORT.DIR |= MYUART_TXPIN; // set TX pin to output
  MYUART_TXPORT.OUT |= MYUART_TXPIN;  // and "1" as per datasheet
  #if RS485 > 0
    MYUART_XDIRPORT.DIR |= MYUART_XDIRPIN; // set XDIR pin to output
  #endif
  #if defined (MYUART_PMUX_VAL)
    MYPMUX_REG = MYUART_PMUX_VAL;  // alternate pinout to use
  #endif
  if ((FUSE_OSCCFG & FUSE_FREQSEL_gm) == FREQSEL_16MHZ_gc) {
    MYUART.BAUD = BAUD_SETTING_16;
  } else {
    MYUART.BAUD = BAUD_SETTING_20;
  }
  MYUART.DBGCTRL = 1;  // run during debug
  MYUART.CTRLC = (USART_CHSIZE_gm & USART_CHSIZE_8BIT_gc);  // Async, Parity Disabled, 1 StopBit
  #if defined(RS485) && RS485 > 0
    /* Enable RS485 mode using XDIR pin */
    MYUART.CTRLA = 1;  // Interrupts: all off, RS485 mode 1
  #else
    MYUART.CTRLA = 0;  // Interrupts: all off, RS485 off
  #endif
  MYUART.CTRLB = USART_RXEN_bm | USART_TXEN_bm;

  // Set up watchdog to trigger after a bit
  //  (nominally:, 1s for autoreset, longer for manual)
  watchdogConfig(WDTPERIOD);

  #if (LED_START_FLASHES > 0) || defined(LED_DATA_FLASH) || defined(LED_START_ON)
    /* Set LED pin as output */
    LED_PORT.DIR |= LED;
  #endif

  #if LED_START_FLASHES > 0
    /* Flash onboard LED to signal entering of bootloader */
    #ifdef LED_INVERT
      flash_led(LED_START_FLASHES * 2 + 1);
    #else
      flash_led(LED_START_FLASHES * 2);
    #endif
  #else
    #if defined(LED_START_ON)
      #ifndef LED_INVERT
        /* Turn on LED to indicate starting bootloader (less code!) */
        LED_PORT.OUT |= LED;
      #endif
    #endif
  #endif

  /* Forever loop: exits by causing WDT reset */
  for (;;) {
    /* get character from UART */
    ch = getch();

    if (ch == STK_GET_PARAMETER) {
      unsigned char which = getch();
      verifySpace();
      /*
         Send optiboot version as "SW version"
         Note that the references to memory are optimized away.
      */
      if (which == STK_SW_MINOR) {
        putch(optiboot_version & 0xFF);
      } else if (which == STK_SW_MAJOR) {
        putch(optiboot_version >> 8);
      } else {
        /*
           GET PARAMETER returns a generic 0x03 reply for
           other parameters - enough to keep Avrdude happy
        */
        putch(0x03);
      }
    } else if (ch == STK_SET_DEVICE) {
      // SET DEVICE is ignored
      getNch(20);
    } else if (ch == STK_SET_DEVICE_EXT) {
      // SET DEVICE EXT is ignored
      getNch(5);
    } else if (ch == STK_LOAD_ADDRESS) {
      // LOAD ADDRESS
      address.bytes[0] = getch();
      address.bytes[1] = getch();
      // ToDo: will there be mega-0 chips with >128k of RAM?
      /*          UPDI chips apparently have byte-addressable FLASH ?
                  address.word *= 2; // Convert from word address to byte address
      */
      verifySpace();
    } else if (ch == STK_UNIVERSAL) {
      #ifndef RAMPZ
      // UNIVERSAL command is ignored
      getNch(4);
      putch(0x00);
      #endif
    }
    /* Write memory, length is big endian and is in bytes */
    else if (ch == STK_PROG_PAGE) {
      // PROGRAM PAGE - any kind of page!
      uint8_t desttype;

      GETLENGTH(length);
      desttype = getch();

      if (desttype == 'F') {
        address.word += MAPPED_PROGMEM_START;
      } else {
        address.word += MAPPED_EEPROM_START;
      }
      // TODO: user row?

      do {
        *(address.bptr++) = getch();
      } while (--length);

      // Read command terminator, start reply
      verifySpace();
      /*
         Actually Write the buffer to flash (and wait for it to finish.)
      */
      _PROTECTED_WRITE_SPM(NVMCTRL.CTRLA, NVMCTRL_CMD_PAGEERASEWRITE_gc);
      while (NVMCTRL.STATUS & (NVMCTRL_FBUSY_bm | NVMCTRL_EEBUSY_bm))
        ; // wait for flash and EEPROM not busy, just in case.
    }
    /* Read memory block mode, length is big endian.  */
    else if (ch == STK_READ_PAGE) {
      uint8_t desttype;
      GETLENGTH(length);

      desttype = getch();

      verifySpace();
      if (desttype == 'F') {
        address.word += MAPPED_PROGMEM_START;
      } else {
        address.word += MAPPED_EEPROM_START;
      }
      // TODO: user row?

      do {
        putch(*(address.bptr++));
      } while (--length);
    }

    /* Get device signature bytes  */
    else if (ch == STK_READ_SIGN) {
      // READ SIGN - return what Avrdude wants to hear
      verifySpace();
      putch(SIGROW_DEVICEID0);
      putch(SIGROW_DEVICEID1);
      putch(SIGROW_DEVICEID2);
    } else if (ch == STK_LEAVE_PROGMODE) { /* 'Q' */
      // Adaboot no-wait mod
      watchdogConfig(WDT_PERIOD_8CLK_gc);
      verifySpace();
    } else {
      // This covers the response to commands like STK_ENTER_PROGMODE
      verifySpace();
    }
    putch(STK_OK);
  }
}

void putch(char ch) {
  while (!(MYUART.STATUS & USART_DREIF_bm))
    ;
  MYUART.TXDATAL = ch;
}

uint8_t getch(void) {
  uint8_t ch, flags;
  while (!(MYUART.STATUS & USART_RXCIF_bm))
    ;
  flags = MYUART.RXDATAH;
  ch = MYUART.RXDATAL;
  if ((flags & USART_FERR_bm) == 0) {
    watchdogReset();
  }
  #ifdef LED_DATA_FLASH
    LED_PORT.IN |= LED;
  #endif

  return ch;
}

void getNch(uint8_t count) {
  do {
    getch();
  } while (--count);
  verifySpace();
}

void verifySpace() {
  if (getch() != CRC_EOP) {
    watchdogConfig(WDT_PERIOD_8CLK_gc);    // shorten WD timeout
    while (1)                             // and busy-loop so that WD causes
      ;                                 //  a reset and app start.
  }
  putch(STK_INSYNC);
}

#if LED_START_FLASHES > 0
  void flash_led(uint8_t count) {
    uint16_t delay;  // at 20MHz/6, a 16bit delay counter is enough
    while (count--) {
      LED_PORT.IN |= LED;
      // delay assuming 20Mhz OSC.  It's only to "look about right", anyway.
      for (delay = ((20E6 / 6) / 150); delay; delay--) {
        watchdogReset();
        if (MYUART.STATUS & USART_RXCIF_bm) {
          return;
        }
      }
    }
    watchdogReset(); // for breakpointing
  }
  /* An argument can be made that we should always flash at the same speed:
   * though I don't think this particularly matters. I would expect this to add
   * lds, cpse, and two rjmps to the code for a total of 10 bytes.
   * I will likely do this next time I have to recompile it all. Provided, that is,
   * we still have that much space. Should probably #ifdef it so it can be
   * turned on or off by passing a define to compiler.
   */
  /*

  void flash_led(uint8_t count) {
    uint16_t delay;  // at 20MHz/6, a 16bit delay counter is enough
    while (count--) {
      LED_PORT.IN |= LED; // Ensure same LED flash speed regardless of OSCCFG.
      if ((FUSE_OSCCFG & FUSE_FREQSEL_gm) == FREQSEL_16MHZ_gc) {
        delay = ((16E6 / 6) / 150)
      } else {
        delay = ((20E6 / 6) / 150)
      }
      for (; delay; delay--) {
        watchdogReset();
        if (MYUART.STATUS & USART_RXCIF_bm) {
          return;
        }
      }
    }
    watchdogReset(); // for breakpointing
  }

   */
#endif


/*
   Change the watchdog configuration.
    Could be a new timeout, could be off...
*/
void watchdogConfig(uint8_t x) {
  while (WDT.STATUS & WDT_SYNCBUSY_bm)
    ;  // Busy wait for sycnhronization is required!
  _PROTECTED_WRITE(WDT.CTRLA, x);
}


#ifndef APP_NOSPM
  /*
     Separate function for doing nvmctrl stuff.
     It's needed for application to do manipulate flash, since only the
      bootloader can write or erase flash, or write to the flash alias areas.
     Note that this is significantly different in the details than the
      do_spm() function provided on older AVRs.  Same "vector", though.

     How it works:
     - if the "command" is legal, write it to NVMCTRL.CTRLA
     - if the command is not legal, store data to *address
     - wait for NVM to complete

     For example, to write a flash page:
     Copy each byte with
       do_nvmctrl(flashOffset+MAPPED_PROGMEM_START, 0xFF, *inputPtr);
     Erase and write page with
       do_nvmctrl(0, NVMCTRL_CMD_PAGEERASEWRITE_gc, 0);
  */
  static void do_nvmctrl(uint16_t address, uint8_t command, uint8_t data)  __attribute__((used));
  static void do_nvmctrl(uint16_t address, uint8_t command, uint8_t data) {
    if (command <= NVMCTRL_CMD_gm) {
      _PROTECTED_WRITE_SPM(NVMCTRL.CTRLA, command);
      while (NVMCTRL.STATUS & (NVMCTRL_FBUSY_bm | NVMCTRL_EEBUSY_bm))
        ; // wait for flash and EEPROM not busy, just in case.
    } else {
      *(uint8_t *)address = data;
    }
  }
#endif


#ifdef BIGBOOT
  /*
  Optiboot is designed to fit in 512 bytes, with a minimum feature set.
  Some chips have a minimum bootloader size of 1024 bytes, and sometimes
  it is desirable to add extra features even though 512bytes is exceeded.
  In that case, the BIGBOOT can be used.
  Our extra features so far don't come close to filling 1k, so we can
  add extra "frivolous" data to the image.   In particular, we can add
  information about how Optiboot was built (which options were selected,
  what version, all in human-readable form (and extractable from the
  binary with avr-strings.)

  This can always be removed or trimmed if more actual program space
  is needed in the future.  Currently the data occupies about 160 bytes,
  */
  #define xstr(s) str(s)
  #define str(s) #s
  #define OPTFLASHSECT __attribute__((section(".fini8")))
  #define OPT2FLASH(o) OPTFLASHSECT const char f##o[] = #o "=" xstr(o)


  #ifdef LED_START_FLASHES
    OPT2FLASH(LED_START_FLASHES);
  #endif
  #ifdef LED_DATA_FLASH
    OPT2FLASH(LED_DATA_FLASH);
  #endif
  #ifdef LED_START_ON
    OPT2FLASH(LED_START_ON);
  #endif
  #ifdef LED_NAME
    OPTFLASHSECT const char f_LED[] = "LED=" LED_NAME;
  #endif

  #ifdef SUPPORT_EEPROM
    OPT2FLASH(SUPPORT_EEPROM);
  #endif

  #ifdef BAUD_RATE
    OPT2FLASH(BAUD_RATE);
  #endif
  #ifdef UARTTX
    OPTFLASHSECT const char f_uart[] = "UARTTX=" UART_NAME;
  #endif
  #ifdef RS485
    OPT2FLASH(RS485);
  #endif

  OPTFLASHSECT const char f_date[] = "Built:" __DATE__ ":" __TIME__;
  #ifdef BIGBOOT
    OPT2FLASH(BIGBOOT);
  #endif
  OPTFLASHSECT const char f_device[] = "Device=" xstr(__AVR_DEVICE_NAME__);
  #ifdef OPTIBOOT_CUSTOMVER
    #if OPTIBOOT_CUSTOMVER != 0
      OPT2FLASH(OPTIBOOT_CUSTOMVER);
    #endif
  #endif
  OPTFLASHSECT const char f_version[] = "Version=" xstr(OPTIBOOT_MAJVER) "." xstr(OPTIBOOT_MINVER);
#endif

//  Dummy application that will loop back into the bootloader if not overwritten
//  This gives the bootloader somewhere to jump, and by referencing otherwise
//  unused variables/functions in the bootloader, it prevents them from being
//  omitted by the linker, with fewer mysterious link options.
//  Is there any way to get rid of this cursed thing?! It would let us go back to
//  the output from avr-size that isn't insanely verbose!
//  I would greatly prefer mysterious link options, and avr-size showing the
//  size correctly without having to print over a dozen irrelevant values!
/*
void  __attribute__((section(".application")))
__attribute__((naked)) app();
void app() {
  *(volatile uint16_t *)(&optiboot_version);   // reference the version
  do_nvmctrl(0, NVMCTRL_CMD_PAGEBUFCLR_gc, 0); // reference this function!
  __asm__ __volatile__("jmp 0");               // This is okay here ONLY because this compiled code *is never included in the .hex file!*
  //                    ^^^^^^ NEVER EVER DO THIS IN YOUR CODE! It's done like that here because I want to make this application section (which is not emitted in
  //  the binary, **but is counted in avr-size**) to be as small as possible. And I don't like the fact that is craps up the size like that.
  //    _PROTECTED_WRITE(RSTCTRL.SWRR, 1); <---- This is how you should jump to the bootloader from the app - you don't need to do anything else to ensure safety.!
}
*/
