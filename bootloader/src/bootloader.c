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
}

