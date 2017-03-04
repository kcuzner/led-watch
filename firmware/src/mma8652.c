/**
 * LED Wristwatch
 *
 * Kevin Cuzner
 */

#include "mma8652.h"

#include "stm32l0xx.h"
#include "i2c.h"

#include <stdint.h>
#include <string.h>

#define DEV_ADDR 0x3A

#define REG_STATUS 0x00
#define REG_INT_SOURCE 0x0C
#define REG_WHOAMI 0x0D
#define REG_PULSE_CFG 0x21
#define REG_PULSE_SRC 0x22
#define REG_PULSE_THSX 0x23
#define REG_PULSE_THSY 0x24
#define REG_PULSE_THSZ 0x25
#define REG_PULSE_TMLT 0x26
#define REG_PULSE_LTCY 0x27
#define REG_PULSE_WIND 0x28
#define REG_ASLP_COUNT 0x29
#define REG_CTRL_REG1 0x2A
#define REG_CTRL_REG2 0x2B
#define REG_CTRL_REG3 0x2C
#define REG_CTRL_REG4 0x2D
#define REG_CTRL_REG5 0x2E

#define REG_WHOAMI_VAL 0x4A
#define REG_PULSE_CFG_XSPEFE 0x01
#define REG_PULSE_CFG_XDPEFE 0x02
#define REG_PULSE_CFG_YSPEFE 0x04
#define REG_PULSE_CFG_YDPEFE 0x08
#define REG_PULSE_CFG_ZSPEFE 0x10
#define REG_PULSE_CFG_ZDPEFE 0x20
#define REG_PULSE_CFG_ELE    0x40
#define REG_PULSE_CFG_DPA    0x80
#define REG_PULSE_SRC_Pol_X  0x01
#define REG_PULSE_SRC_Pol_Y  0x02
#define REG_PULSE_SRC_Pol_Z  0x04
#define REG_PULSE_SRC_DPE    0x08
#define REG_PULSE_SRC_AxX    0x10
#define REG_PULSE_SRC_AxY    0x20
#define REG_PULSE_SRC_AxZ    0x40
#define REG_PULSE_SRC_EA     0x80
#define REG_PULSE_THSX_MASK  0x3F
#define REG_PULSE_THSY_MASK  0x3F
#define REG_PULSE_THSZ_MASK  0x3F
#define REG_PULSE_TMLT_MASK  0xFF
#define REG_PULSE_LTCY_MASK  0xFF
#define REG_PULSE_WIND_MASK  0xFF
#define REG_CTRL_REG1_ACTIVE     0x01
#define REG_CTRL_REG1_F_READ     0x02
#define REG_CTRL_REG1_DR0        0x08
#define REG_CTRL_REG1_DR1        0x10
#define REG_CTRL_REG1_DR2        0x20
#define REG_CTRL_REG1_ASLP_RATE0 0x40
#define REG_CTRL_REG1_ASLP_RATE1 0x80
#define REG_CTRL_REG2_MODS0  0x01
#define REG_CTRL_REG2_MODS1  0x02
#define REG_CTRL_REG2_SLPE   0x04
#define REG_CTRL_REG2_SMODS0 0x08
#define REG_CTRL_REG2_SMODS1 0x10
#define REG_CTRL_REG2_RST    0x40
#define REG_CTRL_REG2_ST     0x80
#define REG_CTRL_REG3_PP_OD       0x01
#define REG_CTRL_REG3_IPOL        0x02
#define REG_CTRL_REG3_WAKE_FF_MT  0x08
#define REG_CTRL_REG3_WAKE_PULSE  0x10
#define REG_CTRL_REG3_WAKE_LNDPRT 0x20
#define REG_CTRL_REG3_WAKE_TRANS  0x40
#define REG_CTRL_REG3_FIFO_GATE   0x80
#define REG_CTRL_REG4_INT_EN_DRDY   0x01
#define REG_CTRL_REG4_INT_EN_FF_MT  0x04
#define REG_CTRL_REG4_INT_EN_PULSE  0x08
#define REG_CTRL_REG4_INT_EN_LNDPRT 0x10
#define REG_CTRL_REG4_INT_EN_TRANS  0x20
#define REG_CTRL_REG4_INT_EN_FIFO   0x40
#define REG_CTRL_REG4_INT_EN_ASLP   0x80
#define REG_CTRL_REG5_INT_CFG_DRDY   0x01
#define REG_CTRL_REG5_INT_CFG_FF_MT  0x04
#define REG_CTRL_REG5_INT_CFG_PULSE  0x08
#define REG_CTRL_REG5_INT_CFG_LNDPRT 0x10
#define REG_CTRL_REG5_INT_CFG_TRANS  0x20
#define REG_CTRL_REG5_INT_CFG_FIFO   0x40
#define REG_CTRL_REG5_INT_CFG_ASLP   0x80

static struct {
    unsigned setup:1;
} AccelStatus;

bool mma8652_init(void)
{
    uint8_t temp;

    memset(&AccelStatus, 0, sizeof(AccelStatus));

    //Enable SYSCFG and GPIO clocks
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
    RCC->IOPENR |= RCC_IOPENR_IOPBEN;

    //determine if the device is on the bus
    if (i2c_read(DEV_ADDR, REG_WHOAMI, &temp, 1) && temp == REG_WHOAMI_VAL)
    {
        //Transition to standby mode
        if (!i2c_read(DEV_ADDR, REG_CTRL_REG1, &temp, 1))
            return false;
        temp &= ~REG_CTRL_REG1_ACTIVE;
        if (!i2c_write(DEV_ADDR, REG_CTRL_REG1, &temp, 1))
            return false;

        //Set the data rate to 800Hz when active, 12.5Hz when sleeping
        temp = REG_CTRL_REG1_ASLP_RATE0;
        if (!i2c_write(DEV_ADDR, REG_CTRL_REG1, &temp, 1))
            return false;

        //Set the z pulse threshold to 0x40
        temp = 0x40;
        if (!i2c_write(DEV_ADDR, REG_PULSE_THSZ, &temp, 1))
            return false;

        //Enable single pulse interrupt in Z direction, PULSE_SRC read clears event flag
        temp = REG_PULSE_CFG_ELE | REG_PULSE_CFG_ZSPEFE;
        if (!i2c_write(DEV_ADDR, REG_PULSE_CFG, &temp, 1))
            return false;

        //Enable PULSE wakeup, set interrupt to be active low, open drain
        temp = REG_CTRL_REG3_WAKE_PULSE | REG_CTRL_REG3_PP_OD;
        if (!i2c_write(DEV_ADDR, REG_CTRL_REG3, &temp, 1))
            return false;

        //Enable PULSE interrupt
        temp = REG_CTRL_REG4_INT_EN_PULSE;
        if (!i2c_write(DEV_ADDR, REG_CTRL_REG4, &temp, 1))
            return false;

        //Route PULSE interrupt to INT1, connected to our PB2
        temp = REG_CTRL_REG5_INT_CFG_PULSE;
        if (!i2c_write(DEV_ADDR, REG_CTRL_REG5, &temp, 1))
            return false;

        //Transition accelerometer to active mode
        temp = REG_CTRL_REG1_ACTIVE;
        if (!i2c_write(DEV_ADDR, REG_CTRL_REG1, &temp, 1))
            return false;

        //Set ~ACCEL_INT pin to input
        GPIOB->MODER &= ~(GPIO_MODER_MODE2);

        //Enable pullup
        GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPD2);
        GPIOB->PUPDR |= GPIO_PUPDR_PUPD2_0;

        //set up external interrupts from ~ACCEL_INT
        SYSCFG->EXTICR[0] &= ~(SYSCFG_EXTICR1_EXTI2);
        SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI2_PB;
        EXTI->IMR |= EXTI_IMR_IM2;
        EXTI->FTSR |= EXTI_FTSR_FT2;
        NVIC_EnableIRQ(EXTI2_3_IRQn);

        AccelStatus.setup = 1;
        return true;
    }

    return false;
}

void __attribute__((weak)) hook_mma8652_tap(void) { }

//TODO: Add a separate EXTI module with callback registration
void __attribute__((interrupt ("IRQ"))) EXTI2_3_IRQHandler(void)
{
    uint8_t temp;

    EXTI->PR &= EXTI_PR_PIF2 | EXTI_PR_PIF3;

    if (!AccelStatus.setup)
        return;

    if (!i2c_read(DEV_ADDR, REG_INT_SOURCE, &temp, 1))
        return;
    if (!i2c_read(DEV_ADDR, REG_PULSE_SRC, &temp, 1))
        return;

    hook_mma8652_tap();
}

