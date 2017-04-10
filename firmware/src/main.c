/**
 * STM32L052X8 Wristwatch Firmware
 *
 * Kevin Cuzner
 */

#include "stm32l0xx.h"
#include "system_stm32l0xx.h"

#include "buzzer.h"
#include "buttons.h"
#include "leds.h"
#include "i2c.h"
#include "mma8652.h"
#include "rtc.h"
#include "usb.h"
#include "power.h"
#include "osc.h"

typedef struct __attribute__((packed))
{
    uint8_t data[8];
} WristwatchReport;

static volatile uint8_t segment = 0;

int main(void)
{
    SystemCoreClockUpdate();

    buzzer_init();
    buttons_init();
    leds_init();
    i2c_init();
    mma8652_init();
    rtc_init();
    usb_init();
    power_init();

    /*RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
    TIM2->DIER = TIM_DIER_UIE;
    TIM2->CR1 = TIM_CR1_CEN;
    NVIC_EnableIRQ(TIM2_IRQn);*/

    power_main();

    return 0;
}

void TIM2_IRQHandler()
{
    TIM2->SR = 0;
}

void hook_power_awake()
{
    rtc_refresh();
    leds_clear();
    switch (power_get_battery_state())
    {
    case POWER_BATTERY_CHARGING:
        leds_set_center(1, 0, 0);
        break;
    case POWER_BATTERY_CHARGED:
        leds_set_center(1, 1, 0);
        break;
    default:
        leds_set_center(0, 1, 0);
        break;
    }
    leds_set_minute(rtc_get_minutes(), 3);
    leds_set_minute(rtc_get_seconds(), 1);
    leds_set_hour(rtc_get_hours() % 12, 3);
    leds_commit();
}

void hook_power_on_wake()
{
    power_set_awake_time(5000);
    leds_enable();
}

void hook_power_on_sleep()
{
    leds_disable();
}

void hook_power_on_usb_connect()
{
    osc_request_hsi16();
    usb_enable();
}

void hook_power_on_usb_disconnect()
{
    usb_disable();
    osc_request_msi(5); //Anything slower than 2MHz makes for some crazy flicker
}

void hook_buttons_state_changed(uint8_t state)
{
    buzzer_trigger_beep();
}

void hook_usb_hid_out_report(const USBTransferData *transfer)
{
    //well this is unsafe...
    WristwatchReport *report = (WristwatchReport *)(transfer->addr);

    rtc_set(report->data[0], report->data[1], report->data[2], report->data[3], report->data[4], report->data[5]);
}

