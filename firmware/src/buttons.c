/**
 * LED Wristwatch
 *
 * Kevin Cuzner
 */

#include "buttons.h"

#include "stm32l0xx.h"

void buttons_init(void)
{
    //Enable SYSCFG and GPIO clocks
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
    RCC->IOPENR |= RCC_IOPENR_IOPBEN;

    //Set all button pins to input
    GPIOB->MODER &= ~(GPIO_MODER_MODE11 | GPIO_MODER_MODE12 | GPIO_MODER_MODE13 |
            GPIO_MODER_MODE14);

    //Enable pullups
    GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPD11 | GPIO_PUPDR_PUPD12 | GPIO_PUPDR_PUPD13 |
            GPIO_PUPDR_PUPD14);
    GPIOB->PUPDR |= GPIO_PUPDR_PUPD11_0 | GPIO_PUPDR_PUPD12_0 | GPIO_PUPDR_PUPD13_0 |
        GPIO_PUPDR_PUPD14_0;
    
    //set up external interrupts from those pins
    SYSCFG->EXTICR[2] = SYSCFG_EXTICR3_EXTI11_PB;
    SYSCFG->EXTICR[3] = SYSCFG_EXTICR4_EXTI12_PB | SYSCFG_EXTICR4_EXTI13_PB |
        SYSCFG_EXTICR4_EXTI14_PB;
    EXTI->IMR = EXTI_IMR_IM11 | EXTI_IMR_IM12 | EXTI_IMR_IM13 | EXTI_IMR_IM14;
    EXTI->RTSR = EXTI_RTSR_RT11 | EXTI_RTSR_RT12 | EXTI_RTSR_RT13 | EXTI_RTSR_RT14;
    EXTI->FTSR = EXTI_FTSR_FT11 | EXTI_FTSR_FT12 | EXTI_FTSR_FT13 | EXTI_FTSR_FT14;
    NVIC_EnableIRQ(EXTI4_15_IRQn);
}

void __attribute__ ((weak)) hook_buttons_state_changed(uint8_t state) { }

void EXTI4_15_IRQHandler()
{
    //TODO: Le Debounce
    hook_buttons_state_changed((GPIOB->IDR & (GPIO_IDR_ID14 | GPIO_IDR_ID13 |
                    GPIO_IDR_ID12 | GPIO_IDR_ID11)) >> GPIO_IDR_ID11_Pos);
    EXTI->PR = EXTI->PR;
}

