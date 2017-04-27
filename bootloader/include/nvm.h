/**
 * LED Wristwatch Bootloader
 *
 * Kevin Cuzner
 */

#ifndef _NVM_H_
#define _NVM_H_

#include <stdint.h>
#include <stdbool.h>

#define EEPROM_SECTION ".eeprom,\"aw\",%nobits//" //a bit of a hack to prevent .eeprom from being programmed
#define _EEPROM __attribute__((section (EEPROM_SECTION)))

/**
 * Writes a byte in EEPROM
 * address: Location to write to
 * data: Data to write
 */
bool nvm_eeprom_write_b(uint8_t *address, uint8_t data);

/**
 * Writes a word in EEPROM
 * address: Location to write to
 * data: Data to write
 */
bool nvm_eeprom_write_hw(uint16_t *address, uint16_t data);

/**
 * Writes a word in EEPROM
 * address: Location to write to
 * data: Data to write
 */
bool nvm_eeprom_write_w(uint32_t *address, uint32_t data);

/**
 * Erases a page (32 words) in the flash
 * address: Location to erase
 */
bool nvm_flash_erase_page(uint32_t *address);

/**
 * Writes a half-page (16 words) in the flash
 * address: Location to erase
 * data: Pointer to 16 32-bit words of data
 */
bool nvm_flash_write_half_page(uint32_t *address, uint32_t *data);

#endif //_EEPROM_H_

