/**
 * LED Wristwatch
 *
 * Kevin Cuzner
 */

#include "leds.h"

#include "stm32l0xx.h"

#include <string.h>

#define MUX_PIN_MASK (GPIO_ODR_OD3 | GPIO_ODR_OD4 | GPIO_ODR_OD5 | GPIO_ODR_OD6)
#define LED_PIN_MASK (GPIO_ODR_OD0 | GPIO_ODR_OD1 | GPIO_ODR_OD2 | GPIO_ODR_OD3 | GPIO_ODR_OD4 | GPIO_ODR_OD5)

typedef union {
    uint8_t segment;
    struct {
        unsigned zero:1;
        unsigned one:1;
        unsigned two:1;
        unsigned three:1;
        unsigned four:1;
        unsigned hour:1;
    };
} LEDSegment;

typedef struct {
    LEDSegment segments[16];
} LEDDisplay;

typedef struct {
    unsigned current_level:2;
    unsigned current_segment:4;
} LEDStatus;

static LEDDisplay edit_display;
static LEDDisplay draw_display;
static LEDStatus status;

void leds_init(void)
{
    //Enable clocks
    RCC->IOPENR |= RCC_IOPENR_IOPAEN | RCC_IOPENR_IOPBEN;
    RCC->APB2ENR |= RCC_APB2ENR_TIM21EN;

    //Set all LED control pins to output
    GPIOA->MODER &= ~(GPIO_MODER_MODE0 | GPIO_MODER_MODE1 | GPIO_MODER_MODE2 |
            GPIO_MODER_MODE3 | GPIO_MODER_MODE4 | GPIO_MODER_MODE5);
    GPIOA->MODER |= GPIO_MODER_MODE0_0 | GPIO_MODER_MODE1_0 | GPIO_MODER_MODE2_0 |
        GPIO_MODER_MODE3_0 | GPIO_MODER_MODE4_0 | GPIO_MODER_MODE5_0;
    GPIOB->MODER &= ~(GPIO_MODER_MODE3 | GPIO_MODER_MODE4 | GPIO_MODER_MODE5 |
            GPIO_MODER_MODE6 | GPIO_MODER_MODE7);
    GPIOB->MODER |= GPIO_MODER_MODE3_0 | GPIO_MODER_MODE4_0 | GPIO_MODER_MODE5_0 |
       GPIO_MODER_MODE6_0 | GPIO_MODER_MODE7_0;

    //Set the LED outputs to disable
    GPIOB->BSRR = GPIO_BSRR_BS_7;

    //Prepare the timer for interrupt
    TIM21->ARR = 5000;// 1000; //this gives us ~30Hz at 2.1MHz
    TIM21->DIER = TIM_DIER_UIE;
    NVIC_EnableIRQ(TIM21_IRQn);

    leds_clear();
    leds_commit();
}

void leds_enable(void)
{
    TIM21->CR1 = TIM_CR1_CEN;
}

void leds_disable(void)
{
    GPIOB->BSRR = GPIO_BSRR_BS_7;
    TIM21->CR1 = 0;
}

void leds_clear(void)
{
    memset(&edit_display, 0x00, sizeof(LEDDisplay));
}

void leds_set_minute(uint8_t led, uint8_t level)
{
    uint8_t segment = led / 5;
    uint8_t position = led % 5;
    if (level)
    {
        edit_display.segments[segment].segment |= 1 << position;
    }
    else
    {
        edit_display.segments[segment].segment &= ~(1 << position);
    }
}

void leds_set_hour(uint8_t led, uint8_t level)
{
    edit_display.segments[led].hour = level;
}

void leds_set_center(uint8_t red, uint8_t green, uint8_t blue)
{
    // There is some rework here. I screwed up the LED footprint on the board,
    // so it is backwards in the vertical direction. To fix this, the anode and
    // blue terminal are shorted. Only red and green are connected to the
    // 74HC154 and they are now reversed.
    edit_display.segments[13].zero = red;
    edit_display.segments[12].zero = green;
}

void leds_commit(void)
{
    memcpy(&draw_display, &edit_display, sizeof(draw_display));
}

/**
 * Perform bit magic to swap a byte
 *
 * b: Byte to swap
 */
static uint8_t reverse_byte(uint8_t b) {
   b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
   b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
   b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
   return b;
}

void TIM21_IRQHandler(void)
{
    //determine the next PORTA value
    uint8_t segmentValue = draw_display
        .segments[status.current_segment]
        .segment;

    //to ease routing, odd segments are wired backwards for the minutes
    if ((status.current_segment % 2) && status.current_segment < 12)
    {
        segmentValue = reverse_byte(segmentValue) >> 3 | (segmentValue & 0x20);
    }

    //turn off mux, set new mux value
    uint8_t muxValue = status.current_segment << 3;
    GPIOB->BSRR = (((GPIOB->ODR & MUX_PIN_MASK) ^ muxValue) & muxValue) |
        ((((GPIOB->ODR & MUX_PIN_MASK) ^ muxValue) & GPIOB->ODR) << 16) |
        GPIO_BSRR_BS_7;

    //set led value
    GPIOA->BSRR = (((GPIOA->ODR & LED_PIN_MASK) ^ segmentValue) & segmentValue) |
        ((((GPIOA->ODR & LED_PIN_MASK) ^ segmentValue) & GPIOA->ODR) << 16);

    //enable mux
    GPIOB->BSRR = GPIO_BSRR_BR_7;

    status.current_level++;
    if (!status.current_level)
        status.current_segment++;
}

