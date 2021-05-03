/*------------------------ Optiboot header file ---------------------------|
|                                                                          |
| June 2015 by Marek Wodzinski,  https://github.com/majekw                 |
| Modified June 2016 by MCUdude, https://github.com/MCUdude                |
| Revised March 2021 by MCUdude                                            |
| Released to public domain                                                |
|                                                                          |
| This header file gives possibility to use SPM instruction                |
| from Optiboot bootloader memory.                                         |
|                                                                          |
| There are 5 convenient functions available here:                         |
| * optiboot_page_erase - to erase a FLASH page                            |
| * optiboot_page_fill - to put words into temporary buffer                |
| * optiboot_page_write - to write contents of temporary buffer into FLASH |
| * optiboot_readPage - higher level function to read a flash page and     |
|                        store it in an array                              |
| * optiboot_writePage - higher level function to write content to         |
|                         a flash page                                     |
|                                                                          |
| For some hardcore users, you could use 'do_spm' as raw entry to          |
| bootloader spm function.                                                 |
|-------------------------------------------------------------------------*/


#ifndef OPTIBOOT_H
#define OPTIBOOT_H

#include <avr/pgmspace.h>
#include <stdint.h>

#ifndef USING_OPTIBOOT
#error "Using the optiboot.h flash writing facility requires that the board be using optiboot, but USING_OPTIBOOT is not defined."
#endif


#define NVMCTRL_CMD_COPY_gc (NVMCTRL_CMD_gm + 1)  // one beyond existing commands.
#define SPM_PAGESIZE (PROGMEM_PAGE_SIZE)

/*
 * Main 'magic' function - enter to bootloader do_spm function
 *
 * address - address to write (in bytes) but must be even number
 * command - one of __BOOT_PAGE_WRITE, __BOOT_PAGE_ERASE or __BOOT_PAGE_FILL
 * data - data to write in __BOOT_PAGE_FILL. In __BOOT_PAGE_ERASE or
 *          __BOOT_PAGE_WRITE it control if boot_rww_enable is run
 *         (0 = run, !0 = skip running boot_rww_enable)
 *
 */

// 'typedef' (in following line) and 'const' (few lines below) are a way to define external function at some arbitrary address
typedef void (*do_spm_t)(uint16_t address, uint8_t command, uint16_t data);
typedef uint16_t optiboot_addr_t;

// Mega0/xTiny/etc.   Bootloader is in low memory.
const do_spm_t do_nvmctrl = (do_spm_t)((PROGMEM_START + 2) >> 1);


void do_nvmctrl_cli(optiboot_addr_t address, uint8_t command, uint16_t data);
bool optiboot_check_writable();
void optiboot_page_erase(optiboot_addr_t address);
void optiboot_page_fill(optiboot_addr_t address, uint8_t data);
void optiboot_page_fill(optiboot_addr_t address, uint16_t data);
void optiboot_page_erase_write();
void optiboot_page_write(optiboot_addr_t address);
void optiboot_read(const uint8_t allocated_flash_space[], uint8_t storage_array[], uint16_t page_number, uint16_t start_address, uint16_t stop_address);
void optiboot_readPage(const uint8_t allocated_flash_space[], uint8_t storage_array[], uint16_t page_number);
void optiboot_writePage(const uint8_t allocated_flash_space[], uint8_t data_to_store[], uint16_t page_number);

#endif
