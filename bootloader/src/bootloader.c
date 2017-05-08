/**
 * LED Wristwatch Bootloader
 *
 * Kevin Cuzner
 */

#include "bootloader.h"

#include "stm32l0xx.h"
#include "nvm.h"
#include "usb_hid.h"

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define BOOTLOADER_STATUS_OK 0x3C65A95A

static uint32_t _EEPROM bootloader_status;
static void _EEPROM *bootloader_prog_start;

typedef enum { ERR_NONE = 0, ERR_FSM = 1 << 0, ERR_COMMAND = 1 << 1, ERR_BAD_ADDR = 1 << 2, ERR_BAD_CRC32 = 1 << 3, ERR_WRITE = 1 << 4, ERR_SHORT = 1 << 5 } BootloaderError;

#define CMD_RESET 0x00000000
#define CMD_PROG  0x00000080
#define CMD_READ  0x00000040
#define CMD_EXIT  0x000000C3

static union {
    uint32_t buffer[16];
    struct {
        uint32_t last_command;
        uint32_t flags;
        uint32_t crc32_lower;
        uint32_t crc32_upper;
        uint8_t data[48];
    };
} in_report;

union {
    uint32_t buffer[16];
    struct {
        uint32_t command;
        uint32_t *address;
        uint32_t crc32_lower;
        uint32_t crc32_upper;
    };
} out_report;

static const USBTransferData in_report_data = { &in_report, sizeof(in_report) };
static const USBTransferData out_report_data = { &out_report, sizeof(out_report) };

_Static_assert(sizeof(in_report) == 64, "Bootloader IN report is improperly sized. Fix this!");
_Static_assert(sizeof(out_report) == 64, "Bootloader OUT report is improperly sized. Fix this!");

typedef enum { EV_ANY, EV_CONFIGURED, EV_HID_OUT, EV_HID_IN, EV_HID_OUT_SHORT } BootloaderEvent;
typedef enum { ST_ANY, ST_RESET, ST_STATUS, ST_LPROG, ST_UPROG, ST_LREAD, ST_UREAD } BootloaderState;

typedef BootloaderState (*BootloaderFsmFn)(BootloaderEvent);

typedef struct {
    BootloaderState state;
    BootloaderEvent ev;
    BootloaderFsmFn fn;
} BootloaderFsmEntry;

static volatile struct {
    uint32_t *address;
    uint32_t crc32_lower;
    uint32_t crc32_upper;
    BootloaderState next_state; //used when sending a status report
} bootloader_state;

/**
 * Sends a status report, starting an IN-OUT sequence
 */
static BootloaderState bootloader_send_status(BootloaderState next)
{
    bootloader_state.next_state = next;
    usb_hid_send(&in_report_data);

    return ST_STATUS;
}

/**
 * Sends a status report, queueing up a state change to ST_RESET
 */
static BootloaderState bootloader_enter_reset(void)
{
    memset(in_report.buffer, 0, sizeof(in_report));
    in_report.last_command = CMD_RESET;
    return bootloader_send_status(ST_RESET);
}

/**
 * Sends a status report, queueing up a state change to ST_LPROG
 */
static BootloaderState bootloader_enter_prog(void)
{
    bootloader_state.address = out_report.address;
    bootloader_state.crc32_lower = out_report.crc32_lower;
    bootloader_state.crc32_upper = out_report.crc32_upper;

    memset(in_report.buffer, 0, sizeof(in_report));
    in_report.last_command = CMD_PROG;

    uint32_t address = (uint32_t)bootloader_state.address;
    //determine if the address is aligned and in range
    if ((address & 0x7F) || (address < FLASH_LOWER_BOUND) || (address > FLASH_UPPER_BOUND))
    {
        in_report.flags = ERR_BAD_ADDR;
        return bootloader_send_status(ST_RESET);
    }
    else
    {
        //erase the page
        nvm_flash_erase_page(bootloader_state.address);
        return bootloader_send_status(ST_LPROG);
    }
}

/**
 * Sends a status report, queueing up a state change to ST_LREAD
 */
static BootloaderState bootloader_enter_read(void)
{
    memset(in_report.buffer, 0, sizeof(in_report));
    in_report.last_command = CMD_READ;
    //TODO: CRC32
    //TODO: This can't use send_status because it creates an IN-IN-IN sequence
    return bootloader_send_status(ST_LREAD);
}

static BootloaderState bootloader_fsm_configured(BootloaderEvent ev)
{
    usb_hid_receive(&out_report_data);
    return ST_RESET;
}

static BootloaderState bootloader_fsm_reset(BootloaderEvent ev)
{
    switch (ev)
    {
    case EV_HID_OUT:
        GPIOB->BSRR = GPIO_BSRR_BR_7;
        if (out_report.command == CMD_RESET)
        {
            return bootloader_enter_reset();
        }
        else if (out_report.command == CMD_PROG)
        {
            return bootloader_enter_prog();
        }
        else if (out_report.command == CMD_READ)
        {
            bootloader_state.address = out_report.address;
            return bootloader_enter_read();
        }
        else
        {
            memset(in_report.buffer, 0, sizeof(in_report));
            in_report.last_command = out_report.command;
            in_report.flags = ERR_COMMAND;
            return bootloader_send_status(ST_RESET);
        }
        break;
    default:
        memset(in_report.buffer, 0, sizeof(in_report));
        in_report.flags = ERR_FSM;
        return bootloader_send_status(ST_RESET);
    }
}

static BootloaderState bootloader_fsm_status(BootloaderEvent ev)
{
    GPIOB->BSRR = GPIO_BSRR_BS_7;
    usb_hid_receive(&out_report_data);
    return bootloader_state.next_state;
}

/**
 * Shared function to writing an OUT report to flash
 */
static BootloaderState bootloader_fsm_program(bool upper, BootloaderEvent ev)
{
    uint32_t i, crc32, computed_crc32;
    uint32_t *address;
    BootloaderError error_flags;

    if (ev != EV_HID_OUT)
    {
        error_flags = ERR_FSM;
        goto error;
    }

    crc32 = upper ? bootloader_state.crc32_upper : bootloader_state.crc32_lower;

    //check the CRC32 (to avoid unexpected programming events)
    CRC->CR |= CRC_CR_RESET;
    volatile uint8_t *dr = &CRC->DR;
    for (i = 0; i < 16; i++)
    {
        CRC->DR = out_report.buffer[i];
    }
    computed_crc32 = ~CRC->DR; //invert result for zlib
    if (upper)
        in_report.crc32_upper = computed_crc32;
    else
        in_report.crc32_lower = computed_crc32;
    if (crc32 != computed_crc32)
    {
        error_flags = ERR_BAD_CRC32;
        goto error;
    }

    //program the page
    address = upper ? &bootloader_state.address[16] : bootloader_state.address;
    if (!nvm_flash_write_half_page(address, out_report.buffer))
    {
        error_flags = ERR_WRITE;
        goto error;
    }

    GPIOB->BSRR = GPIO_BSRR_BR_7;
    in_report.flags = ERR_NONE;
    return bootloader_send_status(upper ? ST_RESET : ST_UPROG);

error:
    in_report.flags = error_flags;
    return bootloader_send_status(ST_RESET);
}

/**
 * State executed when the lower page to program has been received
 */
static BootloaderState bootloader_fsm_lprog(BootloaderEvent ev)
{
    return bootloader_fsm_program(false, ev);
}

/**
 * State executed when the upper page to program has been received
 */
static BootloaderState bootloader_fsm_uprog(BootloaderEvent ev)
{
    return bootloader_fsm_program(true, ev);
}

static BootloaderState bootloader_fsm_short(BootloaderEvent ev)
{
    memset(in_report.buffer, 0, sizeof(in_report));
    in_report.flags = ERR_SHORT;
    return bootloader_send_status(ST_RESET);
}

static BootloaderState bootloader_fsm_error(BootloaderEvent ev)
{
    memset(in_report.buffer, 0, sizeof(in_report));
    in_report.flags = ERR_FSM;
    return bootloader_send_status(ST_RESET);
}

static const BootloaderFsmEntry fsm[] = {
    { ST_ANY, EV_CONFIGURED, &bootloader_fsm_configured },
    { ST_RESET, EV_HID_OUT, &bootloader_fsm_reset },
    { ST_STATUS, EV_HID_IN, &bootloader_fsm_status },
    { ST_LPROG, EV_HID_OUT, &bootloader_fsm_lprog },
    { ST_UPROG, EV_HID_OUT, &bootloader_fsm_uprog },
    { ST_ANY, EV_HID_OUT_SHORT, &bootloader_fsm_short },
    { ST_ANY, EV_ANY, &bootloader_fsm_error }
};
#define FSM_SIZE sizeof(fsm)/(sizeof(BootloaderFsmEntry))

void bootloader_init(void)
{
    //reset_csr = RCC->CSR;

    RCC->IOPENR |= RCC_IOPENR_IOPAEN | RCC_IOPENR_IOPBEN;
    RCC->AHBENR |= RCC_AHBENR_CRCEN;
    GPIOA->MODER &= ~GPIO_MODER_MODE5_1;
    GPIOB->MODER &= ~GPIO_MODER_MODE7_1;
    GPIOA->BSRR = GPIO_BSRR_BS_5;
    GPIOB->BSRR = GPIO_BSRR_BS_7;

    //zlib configuration: Reverse 32-bit in, reverse out, default polynomial and init value
    //Note that the result will need to be inverted
    CRC->CR = CRC_CR_REV_IN_0 | CRC_CR_REV_IN_1 | CRC_CR_REV_OUT;

    if (bootloader_status == BOOTLOADER_STATUS_OK)
    {
        RCC->CSR |= RCC_CSR_RMVF;
        nvm_eeprom_write_w(&bootloader_status, 0);
    }
    else
    {
        nvm_eeprom_write_w(&bootloader_status, BOOTLOADER_STATUS_OK);
    }
}

static void bootloader_tick(BootloaderEvent ev)
{
    static BootloaderState state = ST_RESET;
    uint32_t i;
    for (i = 0; i < FSM_SIZE; i++)
    {
        const BootloaderFsmEntry *entry = &fsm[i];
        if (entry->state == state || entry->state == ST_ANY)
        {
            if (entry->ev == ev || entry->ev == EV_ANY)
            {
                state = entry->fn(ev);
                return;
            }
        }
    }
}

void hook_usb_hid_configured(void)
{
    bootloader_tick(EV_CONFIGURED);
}

void hook_usb_hid_in_report_sent(const USBTransferData *report)
{
    bootloader_tick(EV_HID_IN);
}

uint32_t last_length;
void hook_usb_hid_out_report_received(const USBTransferData *report)
{
    if (report->len == 64)
    {
        bootloader_tick(EV_HID_OUT);
    }
    else
    {
        bootloader_tick(EV_HID_OUT_SHORT);
    }
}

