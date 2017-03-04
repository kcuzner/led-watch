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


    //1KHz, low duty cycle (~2%).
    //
    //A lower duty cycle reduces the power supply droop at the expense of
    //adding harmonics to the sound. It sounds a little strange.  With a 50%
    //duty cycle, there is a noticeable flicker in the LED brightness level. It
    //disappears mostly with a 25% duty cycle and isn't noticable at all with a
    //2% duty cycle. Observing on the scope shows a several hundred mV drop with
    //50%, a couple hundred mV with 25%, and less than a hundred with 2%.

    //TODO: Change this to take into account new clock frequencies
    TIM22->ARR = 2100 * 8;
    TIM22->CCR1 = 50 * 8;
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

void __attribute__ ((interrupt ("IRQ"))) TIM22_IRQHandler()
{
    counter--;
    if (!counter)
    {
        TIM22->CCER = 0;
        TIM22->CR1 = 0;
    }
    TIM22->SR = 0;
}
