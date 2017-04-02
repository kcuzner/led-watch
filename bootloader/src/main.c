/**
 * STM32L052X8 Wristwatch Firmware
 *
 * Kevin Cuzner
 */

#include "stm32l0xx.h"
#include "system_stm32l0xx.h"

#include "usb.h"
#include "osc.h"

typedef struct __attribute__((packed))
{
    uint8_t data[8];
} WristwatchReport;

static volatile uint8_t segment = 0;

int main(void)
{
    SystemCoreClockUpdate();

    usb_init();

    osc_request_hsi16();
    usb_enable();

    while (1) { }

    return 0;
}

void TIM2_IRQHandler()
{
    TIM2->SR = 0;
}

void hook_usb_hid_out_report(const USBTransferData *transfer)
{
}

