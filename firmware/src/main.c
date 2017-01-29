/**
 * STM32L052X8 Wristwatch Firmware
 *
 * Kevin Cuzner
 */

#include "stm32l0xx.h"

int main(void)
{
    //Enable clocks
    RCC->IOPENR |= RCC_IOPENR_IOPAEN | RCC_IOPENR_IOPBEN | RCC_IOPENR_IOPCEN |
        RCC_IOPENR_IOPDEN | RCC_IOPENR_IOPHEN;

    //Set all LED control pins to output
    GPIOA->MODER &= ~(GPIO_MODER_MODE0 | GPIO_MODER_MODE1 | GPIO_MODER_MODE2 |
            GPIO_MODER_MODE3 | GPIO_MODER_MODE4 | GPIO_MODER_MODE5);
    GPIOA->MODER |= GPIO_MODER_MODE0_0 | GPIO_MODER_MODE1_0 | GPIO_MODER_MODE2_0 |
        GPIO_MODER_MODE3_0 | GPIO_MODER_MODE4_0 | GPIO_MODER_MODE5_0;
    GPIOB->MODER &= ~(GPIO_MODER_MODE3 | GPIO_MODER_MODE4 | GPIO_MODER_MODE5 |
            GPIO_MODER_MODE6 | GPIO_MODER_MODE7);
    GPIOB->MODER |= GPIO_MODER_MODE3_0 | GPIO_MODER_MODE4_0 | GPIO_MODER_MODE5_0 |
       GPIO_MODER_MODE6_0 | GPIO_MODER_MODE7_0;


    GPIOB->BSRR = GPIO_BSRR_BR_7;// | GPIO_BSRR_BS_3;

    GPIOA->BSRR = GPIO_BSRR_BS_5;
    GPIOA->BSRR = GPIO_BSRR_BS_0 | GPIO_BSRR_BS_1 | GPIO_BSRR_BS_2 |
        GPIO_BSRR_BS_3 | GPIO_BSRR_BS_4 | GPIO_BSRR_BS_5;

    uint8_t segment = 0;
    while (1)
    {
        GPIOB->ODR = (segment & 0xF) << 3;

        segment++;
        segment &= 0xF;
    }

    return 0;
}

void TIM2_IRQHandler()
{
    /*static uint8_t on = 0;
    TIM2->SR = ~TIM_SR_UIF;
    if (on)
        GPIOB->BSRR = GPIO_BSRR_BS0;
    else
        GPIOB->BSRR = GPIO_BSRR_BR0;

    on ^= 0xFF;*/
}

