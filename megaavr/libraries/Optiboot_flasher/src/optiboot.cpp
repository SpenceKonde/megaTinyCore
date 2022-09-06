#include "optiboot.h"


/*
 * The same as do_nvmctrl but with disable/restore interrupts state
 * required to successful execution
 *
 */
void do_nvmctrl_cli(optiboot_addr_t address, uint8_t command, uint16_t data) {
  uint8_t sreg_save;
  sreg_save = SREG;    // Save old SREG value
  asm volatile("cli"); // Disable interrupts
  do_nvmctrl(address, command, data); // 16 bit address - no problems to pass directly
  SREG = sreg_save; // Restore last interrupts state
}


/**
 * @brief Checks if the microcontroller contains a bootloader that has flash
 * writing capabilities. It does so by checking if a specific number is placed
 * at the end of the bootloader section
 *
 * @return true if compatible bootloader is present
 * @return false if incompatible or no bootloader is present
 */
bool optiboot_check_writable() {
  uint8_t content = pgm_read_byte(0x1FF);
  if (content == 9) {
    return true;
  } else {
    return false;
  }
}


/**
 * @brief Erase flash page
 *
 * @param address flash page start address
 */
void optiboot_page_erase(optiboot_addr_t address) {
  // Set page by writing to address
  optiboot_page_fill(address, (uint8_t)0xFF);
  do_nvmctrl_cli(0, NVMCTRL_CMD_PAGEERASE_gc, 0); // do actual erase
}


/**
 * @brief Writes an 8-bit value to a temporary, internal buffer. Note that
 * this temporary buffer gets destroyed if you read or write to EEPROM. It's
 * therefore important that the buffer gets written to flash before doing
 * anything EEPROM related. Also note that you can write only once to one
 * location in the temporary buffer without erasing it first, which happens
 * after a flash page erase or write.
 *
 * @param address address where to write the 8-bit data
 * @param data data to write
 */
void optiboot_page_fill(optiboot_addr_t address, uint8_t data) {
  do_nvmctrl(address, NVMCTRL_CMD_COPY_gc, data);
}


/**
 * @brief Writes a word/16-bit value to a temporary, internal buffer. Note that
 * this temporary buffer gets destroyed if you read or write to EEPROM. It's
 * therefore important that the buffer gets written to flash before doing
 * anything EEPROM related. Also note that you can write only once to one
 * location in the temporary buffer without erasing it first, which happens
 * after a flash page erase or write.
 *
 * @param address address where to write the 16-bit data
 * @param data data to write
 */
void optiboot_page_fill(optiboot_addr_t address, uint16_t data) {
  optiboot_page_fill(address, (uint8_t)(data & 0xFF));
  optiboot_page_fill(++address, (uint8_t)(data >> 8));
}


/**
 * @brief Write the temporary, internal buffer to flash
 *
 */
void optiboot_page_erase_write() {
  do_nvmctrl_cli(0, NVMCTRL_CMD_PAGEERASEWRITE_gc, 0);
}


/**
 * @brief Write the temporary, internal buffer to flash
 *
 * @param address flash page start address
 */
void optiboot_page_write(optiboot_addr_t address) {
  do_nvmctrl_cli(address, NVMCTRL_CMD_PAGEWRITE_gc, 0);
}


// Higher level functions for reading and writing from flash


/**
 * @brief Read bytes from a given page and further out in memory. It will
 * continue to fill the storage_array until the stop_address is reached. Note
 * that this function will only work for data stored in near progmem, below
 * 64kiB.
 *
 * @param allocated_flash_space the allocated flash space to read from
 * @param storage_array the array to store the flash content to
 * @param page_number the flash page number to start reading from
 * @param start_address the address to start reading from, relative to the flash page number
 * @param stop_address the address where we stop reading, relative to the flash page number
 */
void optiboot_read(const uint8_t allocated_flash_space[], uint8_t storage_array[], uint16_t page_number, uint16_t start_address, uint16_t stop_address) {
  for (uint16_t j = start_address; j < stop_address; j++) {
    uint8_t read_character = allocated_flash_space[j + SPM_PAGESIZE * (page_number)];
    storage_array[j - start_address] = read_character;
  }
}


/**
 * @brief Reads an entire flash page and stores the content in storage_array.
 * Note that this function will only work for data stored in near progmem,
 * below 64kiB.
 *
 * @param allocated_flash_space the allocated flash space to read from
 * @param storage_array the array to store the flash content to
 * @param page_number the flash page number to read from
 */
void optiboot_readPage(const uint8_t allocated_flash_space[], uint8_t storage_array[], uint16_t page_number) {
  optiboot_read(allocated_flash_space, storage_array, page_number, 0, SPM_PAGESIZE);
}


/**
 * @brief Writes the content of data_to_store to a flash page.
 * Note that this function will only work for data stored in near progmem,
 * below 64kiB.
 *
 * @param allocated_flash_space the allocated flash space to read from
 * @param data_to_store an array that holds the data to store
 * @param page_number the flash page number to write to
 */
void optiboot_writePage(const uint8_t allocated_flash_space[], uint8_t data_to_store[], uint16_t page_number) {
  // Copy ram buffer to temporary flash buffer
  for (uint16_t i = 0; i < SPM_PAGESIZE; i++) {
    optiboot_page_fill((optiboot_addr_t)&allocated_flash_space[i + SPM_PAGESIZE * page_number], data_to_store[i]);
  }

  // Erase and then write page
  optiboot_page_erase_write();
}
