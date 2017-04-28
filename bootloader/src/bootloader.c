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

#define BOOTLOADER_STATUS_OK 0x3C65A95A

static uint32_t _EEPROM bootloader_status;
static void _EEPROM *bootloader_prog_start;

static union {
    uint8_t buffer[64];
    struct {
        uint32_t last_command;
        uint32_t flags;
        uint32_t crc32;
    };
} in_report;

static union {
    uint8_t buffer[64];
    struct {
        uint32_t command;
        uint32_t *address;
        uint32_t crc32;
    };
} out_report;

static const USBTransferData in_report_data = { &in_report, sizeof(in_report) };
static const USBTransferData out_report_data = { &out_report, sizeof(out_report) };

_Static_assert(sizeof(in_report) == 64, "Bootloader IN report is improperly sized. Fix this!");
_Static_assert(sizeof(out_report) == 64, "Bootloader OUT report is improperly sized. Fix this!");

static struct {
    bool out_received;
    bool in_sent;
} bootloader_fsm_status;

void bootloader_init(void)
{
    //reset_csr = RCC->CSR;

    RCC->IOPENR |= RCC_IOPENR_IOPAEN | RCC_IOPENR_IOPBEN;
    GPIOA->MODER &= ~GPIO_MODER_MODE5_1;
    GPIOB->MODER &= ~GPIO_MODER_MODE7_1;
    GPIOA->BSRR = GPIO_BSRR_BS_5;
    GPIOB->BSRR = GPIO_BSRR_BS_7;

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

void bootloader_tick(void)
{
    if (bootloader_fsm_status.out_received)
    {
        bootloader_fsm_status.out_received = false;
        in_report.last_command = out_report.command;
        usb_hid_send(&in_report_data);
    }
    if (bootloader_fsm_status.in_sent)
    {
        bootloader_fsm_status.in_sent = false;
        usb_hid_receive(&out_report_data);
    }
}

void hook_usb_hid_configured(void)
{
    //prepare for OUT report
    usb_hid_receive(&out_report_data);

    //IN repots will be issued as reports are received
}

void hook_usb_hid_in_report_sent(const USBTransferData *report)
{
    if (report->addr == in_report_data.addr)
        bootloader_fsm_status.in_sent = true;
}

void hook_usb_hid_out_report_received(const USBTransferData *report)
{
    if (report->addr == out_report_data.addr)
        bootloader_fsm_status.out_received = true;
}

