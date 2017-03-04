/**
 * LED Wristwatch
 *
 * Kevin Cuzner
 */

#include "power.h"

#include <stdbool.h>

#include "stm32l0xx.h"

#define USB_PRES_MASK GPIO_IDR_ID0
#define BAT_CHG_MASK GPIO_IDR_ID1

typedef enum { PWR_EVT_ANY, PWR_EVT_NONE, PWR_EVT_USB_CONNECT, PWR_EVT_USB_DISCONNECT } PowerEvent;
typedef enum { PWR_ST_INIT, PWR_ST_USB, PWR_ST_BATTERY, PWR_ST_SLEEP } PowerState;
typedef PowerState (*PowerStateFn)(void);
typedef struct {
    PowerState state;
    PowerEvent event;
    PowerStateFn fn;
} PowerStateEntry;

static uint32_t countdown;
static uint32_t input_state;

void __attribute__((weak)) hook_power_awake(void) { }
void __attribute__((weak)) hook_power_on_wake(void) { }
void __attribute__((weak)) hook_power_on_sleep(void) { }
void __attribute__((weak)) hook_power_on_usb_connect(void) { }
void __attribute__((weak)) hook_power_on_usb_disconnect(void) { }

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

/**
 * Desired behavior
 *
 * 1. When the USB is plugged in:
 *  - The face is always on
 *  - The HSI16 and HSI48 are activated (HSI48 logic may be moved to usb_enable/disable)
 *  - hook_power_awake is run continuously
 * 2. When the USB is unplugged
 *  2a. When the watch is inactive (face off, possibly sleeping):
 *    - MSI is slowed as much as possible, or the device is put into Stop
 *  2b. When the watch is active (face on, awake):
 *    - MSI is increased to minimum speed for running watch face without flicker
 *    - hook_power_awake is run continuously
 *    - Remain in this mode until the active countdown reaches zero
 *
 * Implementation outside this module:
 * - hook_power_awake: Contains main watch state machine
 * - hook_power_on_wake: Enables watch face
 * - hook_power_on_sleep: Disables watch face
 */

static PowerState power_fsm_init(void)
{
    PowerState nextState;

    //analyze the initial state and call any necessary hooks
    countdown = 0;
    hook_power_on_wake();
    if (GPIOB->IDR & USB_PRES_MASK)
    {
        hook_power_on_usb_connect();
        nextState = PWR_ST_USB;
    }
    else
    {
        hook_power_on_usb_disconnect();
        nextState = PWR_ST_BATTERY;
    }

    //Set up external interrupts from USB connect/disconnect
    SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI0_PB;
    EXTI->IMR |= EXTI_IMR_IM0;
    EXTI->RTSR |= EXTI_RTSR_RT0;
    EXTI->FTSR |= EXTI_FTSR_FT0;
    NVIC_EnableIRQ(EXTI0_1_IRQn);

    return nextState;
}

static PowerState power_fsm_usb_main(void)
{
    //we stay awake until an event changes that
    hook_power_awake();
    return PWR_ST_USB;
}

static PowerState power_fsm_usb_disconnect(void)
{
    countdown = 0;
    hook_power_on_usb_disconnect();
    return PWR_ST_BATTERY;
}

static PowerState power_fsm_usb_connect(void)
{
    hook_power_on_usb_connect();
    return PWR_ST_USB;
}

static PowerState power_fsm_battery_main(void)
{
    hook_power_awake();
    if (!countdown--)
    {
        return PWR_ST_SLEEP;
    }
    else
    {
        return PWR_ST_BATTERY;
    }
}

static PowerState power_fsm_sleep_main(void)
{
    hook_power_on_sleep();
    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
    __ASM volatile ("wfi");
    countdown = 0;
    hook_power_on_wake();
    return PWR_ST_BATTERY;
}

static PowerStateEntry power_fsm[] = {
    { PWR_ST_INIT, PWR_EVT_ANY, &power_fsm_init },
    { PWR_ST_USB, PWR_EVT_USB_DISCONNECT, &power_fsm_usb_disconnect },
    { PWR_ST_USB, PWR_EVT_ANY, &power_fsm_usb_main },
    { PWR_ST_BATTERY, PWR_EVT_USB_CONNECT, &power_fsm_usb_connect },
    { PWR_ST_BATTERY, PWR_EVT_ANY, &power_fsm_battery_main },
    { PWR_ST_SLEEP, PWR_EVT_ANY, &power_fsm_sleep_main }
};
#define USB_CTL_STATE_COUNT (sizeof(power_fsm)/sizeof(*power_fsm))

static void power_fsm_tick(PowerEvent event)
{
    static PowerState state = PWR_ST_INIT;

    for (uint8_t i = 0; i < USB_CTL_STATE_COUNT; i++)
    {
        PowerStateEntry *entry = &power_fsm[i];
        if (entry->state == state && (entry->event == event || entry->event == PWR_EVT_ANY))
        {
            state = entry->fn();
        }
    }
}

static PowerEvent power_get_event(uint32_t next_inputs)
{
    uint32_t changes = input_state ^ next_inputs;
    input_state = next_inputs;
    if (next_inputs & USB_PRES_MASK)
    {
        return PWR_EVT_USB_DISCONNECT;
    }
    else
    {
        return PWR_EVT_USB_CONNECT;
    }
    if (changes & next_inputs & USB_PRES_MASK) //there is a usb change and the usb bit is high in the inputs
    {
        return PWR_EVT_USB_DISCONNECT;
    }
    else if (changes & ~(next_inputs) & USB_PRES_MASK) //there is a usb change and the usb bit is low in the inputs
    {
        return PWR_EVT_USB_CONNECT;
    }
    else
    {
        return PWR_EVT_NONE;
    }
}

void power_main(void)
{
    uint32_t next_inputs;
    input_state = GPIOB->IDR & (USB_PRES_MASK | BAT_CHG_MASK);

    while (true)
    {
        next_inputs = GPIOB->IDR & (USB_PRES_MASK | BAT_CHG_MASK);
        power_fsm_tick(power_get_event(next_inputs));
    }
}

PowerBatteryState power_get_battery_state(void)
{
    switch (input_state)
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

void power_set_awake_time(uint32_t ticks)
{
    countdown = ticks;
}

void __attribute__ ((interrupt ("IRQ"))) EXTI0_1_IRQHandler()
{
    EXTI->PR &= EXTI_PR_PIF0 | EXTI_PR_PIF1;
}

