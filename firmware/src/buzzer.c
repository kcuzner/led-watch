/**
 * LED Wristwatch
 *
 * Kevin Cuzner
 */

#include "buzzer.h"

#include "stm32l0xx.h"

static volatile uint16_t counter = 0;

void buzzer_init(void)
{
    //Enable clocks
    RCC->APB2ENR |= RCC_APB2ENR_TIM22EN;
    RCC->IOPENR |= RCC_IOPENR_IOPAEN;

    //Enable TIM22 CH1 output on PA6
    //GPIOA->AFR[0] &= ~GPIO_AFRL_AFRL6;
    GPIOA->AFR[0] = 0x05000000;
    GPIOA->MODER &= ~(GPIO_MODER_MODE6);
    GPIOA->MODER |= GPIO_MODER_MODE6_1;


    //1KHz, 50%.
    //TODO: Change this to take into account new clock frequencies
    TIM22->ARR = 2100;
    TIM22->CCR1 = 1050;
    TIM22->CCMR1 = TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1;
    TIM22->DIER = TIM_DIER_UIE;

    NVIC_EnableIRQ(TIM22_IRQn);
}

void buzzer_trigger_beep(void)
{
    //set the counter to 50ms
    counter = 50;
    TIM22->CCER = TIM_CCER_CC1E;
    TIM22->CR1 = TIM_CR1_CEN;
}

void TIM22_IRQHandler()
{
    counter--;
    if (!counter)
    {
        TIM22->CCER = 0;
        TIM22->CR1 = 0;
    }
    TIM22->SR = 0;
}
