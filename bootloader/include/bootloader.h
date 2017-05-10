/**
 * LED Wristwatch Bootloader
 *
 * Kevin Cuzner
 */

#ifndef _BOOTLOADER_H_
#define _BOOTLOADER_H_

#define FLASH_LOWER_BOUND 0x08002000
#define FLASH_UPPER_BOUND 0x0800FFFF

/**
 * Initializes the bootloader component
 */
void bootloader_init(void);

/**
 * Runs the bootloader component
 */

void bootloader_run(void);

#endif //_BOOTLOADER_H_

