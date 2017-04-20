/**
 * LED Wristwatch
 *
 * Kevin Cuzner
 */

#include "osc.h"
#include "stm32l0xx.h"

static OscChangeCallback change_callbacks[16];
static uint8_t next_change_callback = 0;

static void osc_run_callbacks(void)
{
    SystemCoreClockUpdate();
    for (uint8_t i = 0; i < next_change_callback; i++)
        change_callbacks[i]();
}

void osc_request_hsi16(void)
{
    //turn on HSI16 and switch the processor clock
    RCC->CR |= RCC_CR_HSION;
    while (!(RCC->CR & RCC_CR_HSIRDY)) { }
    RCC->CFGR ^= (RCC->CFGR ^ RCC_CFGR_SW_HSI) & RCC_CFGR_SW;
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI) { }
    //HSI is now the wakeup clock
    RCC->CFGR |= RCC_CFGR_STOPWUCK;
    //turn off MSI
    RCC->CR &= ~RCC_CR_MSION;

    osc_run_callbacks();
}

void osc_request_msi(uint8_t range)
{
    range &= 0x7;
    if (range > 6)
        range = 6;

    //Change the MSI range to the requested range
    uint32_t temp = RCC->ICSCR;
    temp &= ~RCC_ICSCR_MSIRANGE;
    temp |= (range << RCC_ICSCR_MSIRANGE_Pos);
    RCC->ICSCR = temp;

    //Turn on MSI and switch the processor clock
    RCC->CR |= RCC_CR_MSION;
    while (!(RCC->CR  & RCC_CR_MSIRDY)) { }
    RCC->CFGR ^= (RCC->CFGR ^ RCC_CFGR_SW_MSI) & RCC_CFGR_SW;
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_MSI) { }
    //MSI is now the wakeup clock
    RCC->CFGR &= ~RCC_CFGR_STOPWUCK;
    //turn off HSI
    RCC->CR &= ~RCC_CR_HSION;

    osc_run_callbacks();
}

void osc_add_callback(OscChangeCallback fn)
{
    change_callbacks[next_change_callback++] = fn;
}

