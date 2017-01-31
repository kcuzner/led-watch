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

static volatile uint8_t segment = 0;

int main(void)
{
    SystemCoreClockUpdate();


    buzzer_init();
    buttons_init();
    leds_init();

    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
    TIM2->DIER = TIM_DIER_UIE;
    TIM2->CR1 = TIM_CR1_CEN;
    NVIC_EnableIRQ(TIM2_IRQn);

    leds_enable();
    
    while (1)
    {
    }

    return 0;
}

void TIM2_IRQHandler()
{
    static uint8_t c = 0;
    //buzzer_trigger_beep();
        leds_set_center(0, 1, 0);
    leds_set_minute(c++, 1);
    leds_set_hour(c / 5, 1);
    leds_commit();
    if (c == 60)
    {
        c = 0;
        leds_clear();
    }
    TIM2->SR = 0;
}

