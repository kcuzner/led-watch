/**
 * LED Wristwatch Bootloader
 *
 * Kevin Cuzner
 */

#include "nvm.h"

#include "stm32l0xx.h"

/**
 * Certain functions, such as flash write, are easier to do if the code is
 * executed from the RAM. This decoration relocates the function there and
 * prevents any inlining that might otherwise move the function to flash.
 */
#define _RAM __attribute__((section (".data#"), noinline))

/**
 * Unlocks the PECR which allows for writing EEPROM or unlocking further NVM
 * operations
 */
static void nvm_unlock_pecr(void)
{
    if (FLASH->PECR & FLASH_PECR_PELOCK)
    {
        FLASH->PEKEYR = 0x89abcdef;
        FLASH->PEKEYR = 0x02030405;
    }
}

/**
 * Unlocks the PECR and the flash
 */
static void nvm_unlock_flash(void)
{
    nvm_unlock_pecr();
    if (FLASH->PECR & FLASH_PECR_PRGLOCK)
    {
        FLASH->PRGKEYR = 0x8c9daebf;
        FLASH->PRGKEYR = 0x13141516;
    }
}

/**
 * Locks all unlocked NVM regions and registers
 */
static void nvm_lock(void)
{
    if (!(FLASH->PECR & FLASH_PECR_PELOCK))
    {
        FLASH->PECR |= FLASH_PECR_OPTLOCK | FLASH_PECR_PRGLOCK | FLASH_PECR_PELOCK;
    }
}

/**
 * RAM-located function which actually performs page erases.
 *
 * address: Page-aligned address to erase
 */
static _RAM bool nvm_flash_do_page_erase(uint32_t *address)
{
    //erase operation
    FLASH->PECR |= FLASH_PECR_ERASE | FLASH_PECR_PROG;
    *address = (uint32_t)0;
    //wait for completion
    while (FLASH->SR & FLASH_SR_BSY) { }
    if (FLASH->SR & FLASH_SR_EOP)
    {
        //completed without incident
        FLASH->SR = FLASH_SR_EOP;
        return true;
    }
    else
    {
        //there was an error
        FLASH->SR = FLASH_SR_FWWERR | FLASH_SR_PGAERR | FLASH_SR_WRPERR;
        return false;
    }
}

/**
 * RAM-located function which actually performs half-page writes on previously
 * erased pages.
 *
 * address: Half-page aligned address to write
 * data: Array to 16 32-bit words to write
 */
static _RAM bool nvm_flash_do_write_half_page(uint32_t *address, uint32_t *data)
{
    uint8_t i;

    //half-page program operation
    FLASH->PECR |= FLASH_PECR_PROG | FLASH_PECR_FPRG;
    for (i = 0; i < 16; i++)
    {
        *address = data[i]; //the actual address written is unimportant as these words will be queued
    }
    //wait for completion
    while (FLASH->SR & FLASH_SR_BSY) { }
    if (FLASH->SR & FLASH_SR_EOP)
    {
        //completed without incident
        FLASH->SR = FLASH_SR_EOP;
        return true;
    }
    else
    {
        //there was an error
        FLASH->SR = FLASH_SR_FWWERR | FLASH_SR_NOTZEROERR | FLASH_SR_PGAERR | FLASH_SR_WRPERR;
        return false;

    }
}

bool nvm_eeprom_write_b(uint8_t *address, uint8_t data)
{
    nvm_unlock_pecr();
    *address = data;
    nvm_lock();
    return true;
}

bool nvm_eeprom_write_hw(uint16_t *address, uint16_t data)
{
    nvm_unlock_pecr();
    *address = data;
    nvm_lock();
    return true;
}

bool nvm_eeprom_write_w(uint32_t *address, uint32_t data)
{
    nvm_unlock_pecr();
    *address = data;
    nvm_lock();
    return true;
}

bool nvm_flash_erase_page(uint32_t *address)
{
    bool result = false;

    if ((uint32_t)address & 0x7F)
        return false; //not page aligned

    nvm_unlock_flash();
    result = nvm_flash_do_page_erase(address);
    nvm_lock();
    return result;
}

bool nvm_flash_write_half_page(uint32_t *address, uint32_t *data)
{
    bool result = false;

    if ((uint32_t)address & 0x3F)
        return false; //not half-page aligned

    nvm_unlock_flash();
    result = nvm_flash_do_write_half_page(address, data);
    nvm_lock();
    return result;
}

