/**
 * LED Wristwatch Bootloader
 *
 * Kevin Cuzner
 */

#include "bootloader.h"

#include "stm32l0xx.h"

#include <stdint.h>

uint32_t reset_csr;

void bootloader_init(void)
{
    reset_csr = RCC->CSR;

    RCC->CSR |= RCC_CSR_RMVF;
}

