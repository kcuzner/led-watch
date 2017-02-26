/**
 * LED Wristwatch
 *
 * Kevin Cuzner
 */

#include "power.h"

#include "stm32l0xx.h"

void __attribute__((weak)) hook_power_awake(void) { }
void __attribute__((weak)) hook_power_on_wake(void) { }
void __attribute__((weak)) hook_power_on_sleep(void) { }

void power_init(void)
{
    //Enable SYSCFG and GPIO clocks
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
    RCC->IOPENR |= RCC_IOPENR_IOPBEN;

    //Set the ~USB_PRES and CHG_STAT pins to input
    GPIOB->MODER &= ~(GPIO_MODER_MODE0 | GPIO_MODER_MODE1);

    //Enable pullups
    GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPD0 | GPIO_PUPDR_PUPD1);
    GPIOB->PUPDR |= GPIO_PUPDR_PUPD0_0 | GPIO_PUPDR_PUPD1_0;
}

void power_main(void)
{
}

PowerBatteryState power_get_battery_state(void)
{
    switch (GPIOB->IDR & (GPIO_IDR_ID0 | GPIO_IDR_ID1))
    {
    case 0x2:
        //USB present, charge stat floating
        return POWER_BATTERY_CHARGED;
    case 0x0:
        //USB present, charge stat low
        return POWER_BATTERY_CHARGING;
    default:
        //USB not present
        return POWER_BATTERY_DISCHARGING;
    }
}

void power_set_awake_time(uint16_t ticks)
{
}

