/**
 * STM32L052X8 Wristwatch Firmware
 *
 * Kevin Cuzner
 */

#include "stm32l0xx.h"
#include "system_stm32l0xx.h"

#include "buzzer.h"
#include "buttons.h"

static volatile uint8_t segment = 0;

int main(void)
{
    SystemCoreClockUpdate();

    //Enable clocks
    RCC->IOPENR |= RCC_IOPENR_IOPAEN | RCC_IOPENR_IOPBEN | RCC_IOPENR_IOPCEN |
        RCC_IOPENR_IOPDEN | RCC_IOPENR_IOPHEN;

    buzzer_init();
    buttons_init();

    //Set all LED control pins to output
    GPIOA->MODER &= ~(GPIO_MODER_MODE0 | GPIO_MODER_MODE1 | GPIO_MODER_MODE2 |
            GPIO_MODER_MODE3 | GPIO_MODER_MODE4 | GPIO_MODER_MODE5);
    GPIOA->MODER |= GPIO_MODER_MODE0_0 | GPIO_MODER_MODE1_0 | GPIO_MODER_MODE2_0 |
        GPIO_MODER_MODE3_0 | GPIO_MODER_MODE4_0 | GPIO_MODER_MODE5_0;
    GPIOB->MODER &= ~(GPIO_MODER_MODE3 | GPIO_MODER_MODE4 | GPIO_MODER_MODE5 |
            GPIO_MODER_MODE6 | GPIO_MODER_MODE7);
    GPIOB->MODER |= GPIO_MODER_MODE3_0 | GPIO_MODER_MODE4_0 | GPIO_MODER_MODE5_0 |
       GPIO_MODER_MODE6_0 | GPIO_MODER_MODE7_0;


    GPIOB->BSRR = GPIO_BSRR_BR_7;

    GPIOA->BSRR = GPIO_BSRR_BS_1;// | GPIO_BSRR_BS_1 | GPIO_BSRR_BS_2 |
//        GPIO_BSRR_BS_3 | GPIO_BSRR_BS_4 | GPIO_BSRR_BS_5;
    
    while (1)
    {
        GPIOB->ODR = (segment & 0xF) << 3;

        //segment++;
        //segment &= 0xF;
    }

    return 0;
}

void hook_buttons_state_changed(uint8_t state)
{
    buzzer_trigger_beep();
    GPIOA->BSRR = (((GPIOA->ODR & 0xFF) ^ state) & state) |
        ((((GPIOA->ODR & 0xFF) ^ state) & GPIOA->ODR) << 16);
}

