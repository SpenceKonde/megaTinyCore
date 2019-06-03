/******************************************************************************
 * © 2018 Microchip Technology Inc. and its subsidiaries.
 *
 * Subject to your compliance with these terms, you may use Microchip software
 * and any derivatives exclusively with Microchip products. It is your
 * responsibility to comply with third party license terms applicable to your
 * use of third party software (including open source software) that may
 * accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS".  NO WARRANTIES, WHETHER
 * EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY
 * IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
 * PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT,
 * SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR
 * EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED,
 * EVEN IF MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE
 * FORESEEABLE. TO THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL
 * LIABILITY ON ALL CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED
 * THE AMOUNT OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR
 * THIS SOFTWARE.
 *
 *****************************************************************************/

/*
 * UART Bootloader for tinyAVR 0- and 1-series, and megaAVR 0-series
 * Each byte received is echoed to confirm reception.
 *
 * For the code to be placed in the constructors section it is necessary
 * to disable standard startup files in Toolchain->AVR/GNU Linker->General.
 *
 * This file is targeted to UNO WiFi REV2 (ATmega4809)
 * USART0 TxD PA4
 * USART0 RxD PA5
 * LED0       PD6
 * SW1        PC1 (external pull-up)
 */
#define F_CPU_RESET (16E6/6)

#include <avr/io.h>
#include <assert.h>
#include <stdbool.h>

/* Baud rate configuration */
#define BOOT_BAUD (115200)

#define BAUD_REG_VAL (F_CPU_RESET * 64) / (BOOT_BAUD * 16)

/* Memory configuration
 * BOOTEND_FUSE * 256 must be above Bootloader Program Memory Usage,
 * this is 194 bytes at optimization level -O3, so BOOTEND_FUSE = 0x01
 */
#define BOOTEND_FUSE               (0x02)
#define BOOT_SIZE                  (BOOTEND_FUSE * 0x100)
#define MAPPED_APPLICATION_START   (MAPPED_PROGMEM_START + BOOT_SIZE)
#define MAPPED_APPLICATION_SIZE    (MAPPED_PROGMEM_SIZE - BOOT_SIZE)

/* Fuse configuration
 * BOOTEND sets the size (end) of the boot section in blocks of 256 bytes.
 * APPEND = 0x00 defines the section from BOOTEND*256 to end of Flash as application code.
 * Remaining fuses have default configuration.
 */
FUSES = {
	.OSCCFG = FREQSEL_16MHZ_gc,
	.SYSCFG0 = CRCSRC_NOCRC_gc | RSTPINCFG_UPDI_gc,
	.SYSCFG1 = SUT_64MS_gc,
	.APPEND = 0x00,
	.BOOTEND = BOOTEND_FUSE
};

/* Define application pointer type */
typedef void (*const app_t)(void);

/* Interface function prototypes */
static inline bool is_bootloader_requested(void);
static inline void init_uart(void);
static inline uint8_t uart_receive(void);
static inline void uart_send(uint8_t byte);
static inline void init_status_led(void);
static inline void toggle_status_led(void);
static inline void wait_50_ms(void);

/*
 * Main boot function
 * Put in the constructors section (.ctors) to save Flash.
 * Naked attribute used since function prologue and epilogue is unused
 */
__attribute__((naked)) __attribute__((section(".ctors"))) void boot(void)
{
  /* Initialize system for AVR GCC support, expects r1 = 0 */
  asm volatile("clr r1");

  /* 3 very fast blinks (to match optiboot behaviour) */
  init_status_led();
  for (uint8_t i = 0; i < 6; i++) {
    wait_50_ms();
    toggle_status_led();
  }

  /* Check if entering application or continuing to bootloader */
  if(!is_bootloader_requested()) {
    /* Enable Boot Section Lock */
    NVMCTRL.CTRLB = NVMCTRL_BOOTLOCK_bm;

    /* Go to application, located immediately after boot section */
    app_t app = (app_t)(BOOT_SIZE / sizeof(app_t));
    app();
  }

  /* Initialize communication interface */
  init_uart();

  /*
   * Start programming at start for application section
   * Subtract MAPPED_PROGMEM_START in condition to handle overflow on large flash sizes
   */
  uint8_t *app_ptr = (uint8_t *)MAPPED_APPLICATION_START;
  while(app_ptr - MAPPED_PROGMEM_START <= (uint8_t *)PROGMEM_END) {
    /* Receive and echo data before loading to memory */
    uint8_t rx_data = uart_receive();
    if (app_ptr == (uint8_t *)MAPPED_APPLICATION_START && rx_data == 0) {
      // skip first character if 0x00
      continue;
    }
    uart_send(rx_data);

    /* Incremental load to page buffer before writing to Flash */
    *app_ptr = rx_data;
    app_ptr++;
    if(!((uint16_t)app_ptr % MAPPED_PROGMEM_PAGE_SIZE)) {
      /* Page boundary reached, Commit page to Flash */
      _PROTECTED_WRITE_SPM(NVMCTRL.CTRLA, NVMCTRL_CMD_PAGEERASEWRITE_gc);
      while(NVMCTRL.STATUS & NVMCTRL_FBUSY_bm);

      toggle_status_led();
    }
  }

  /* Issue system reset */
  _PROTECTED_WRITE(RSTCTRL.SWRR, RSTCTRL_SWRE_bm);
}

/*
 * Boot access request function
 */
static inline bool is_bootloader_requested(void)
{
  /* Check for boot request from firmware */
  if (USERROW.USERROW31 == 0xEB) {
    /* Clear boot request*/
    USERROW.USERROW31 = 0xff;
    _PROTECTED_WRITE_SPM(NVMCTRL.CTRLA, NVMCTRL_CMD_PAGEERASEWRITE_gc);
    while(NVMCTRL.STATUS & NVMCTRL_EEBUSY_bm);

    return true;
  }
  return false;
}

/*
 * Communication interface functions
 */
static inline void init_uart(void)
{
  /* Configure UART */
  USART0.CTRLB = USART_RXEN_bm | USART_TXEN_bm;

  /* From datasheet:
   * Baud rate compensated with factory stored frequency error
   * Asynchronous communication without Auto-baud (Sync Field)
   * 20MHz Clock, 3V
   */
  int32_t baud_reg_val  = BAUD_REG_VAL;  // ideal BAUD register value
  int8_t sigrow_val = SIGROW.OSC16ERR5V;  // read signed error
  baud_reg_val *= (1024 + sigrow_val);    // sum resolution + error
  baud_reg_val += 512;                    // compensate for rounding error
  baud_reg_val /= 1024;                   // divide by resolution
  USART0.BAUD = (int16_t) baud_reg_val;   // set adjusted baud rate

  PORTMUX.USARTROUTEA |= PORTMUX_USART0_ALT1_gc;

  /* Set TxD (PA4) as output */
  VPORTA.DIR |= PIN4_bm;
}

static inline uint8_t uart_receive(void)
{
  /* Poll for data received */
  while(!(USART0.STATUS & USART_RXCIF_bm));
  return USART0.RXDATAL;
}

static inline void uart_send(uint8_t byte)
{
  /* Data will be sent when TXDATA is written */
  USART0.TXDATAL = byte;
}

static inline void init_status_led(void)
{
  /* Set LED0 (PD6) as output */
  VPORTD.DIR |= PIN6_bm;
}

static inline void toggle_status_led(void)
{
  /* Toggle LED0 (PD6) */
  VPORTD.OUT ^= PIN6_bm;
}

static inline void wait_50_ms(void)
{
  for (uint16_t i = 160000; i--; i != 0) {
    asm volatile ("nop");
  }
}