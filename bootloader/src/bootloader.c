/**
 * LED Wristwatch Bootloader
 *
 * Kevin Cuzner
 */

#include "bootloader.h"

#include "stm32l0xx.h"
#include "nvm.h"

#include <stdint.h>

#define BOOTLOADER_STATUS_OK 0x3C65A95A

uint32_t _EEPROM bootloader_status;
void _EEPROM *bootloader_prog_start;

static uint32_t data[] = {
    0x01020304,
    0x05060708,
    0x090a0b0c,
    0x0d0e0f00,
    0x10203040,
    0x50607080,
    0x90a0b0c0,
    0xd0e0f0ff,
    0x11111111,
    0x22222222,
    0x33333333,
    0x44444444,
    0x55555555,
    0x66666666,
    0x77777777,
    0x88888888
};

uint32_t reset_csr;

void bootloader_init(void)
{
    reset_csr = RCC->CSR;

    RCC->IOPENR |= RCC_IOPENR_IOPAEN | RCC_IOPENR_IOPBEN;
    GPIOA->MODER &= ~GPIO_MODER_MODE5_1;
    GPIOB->MODER &= ~GPIO_MODER_MODE7_1;
    GPIOA->BSRR = GPIO_BSRR_BS_5;
    GPIOB->BSRR = GPIO_BSRR_BS_7;

    if (bootloader_status == BOOTLOADER_STATUS_OK)
    {
        RCC->CSR |= RCC_CSR_RMVF;
        nvm_eeprom_write_w(&bootloader_status, 0);
    }
    else
    {
        nvm_eeprom_write_w(&bootloader_status, BOOTLOADER_STATUS_OK);
    }

    uint32_t *address = (uint32_t*)0x08002000;

    if (nvm_flash_erase_page(address) && nvm_flash_write_half_page(address, data))
        GPIOB->BSRR = GPIO_BSRR_BR_7;
}

