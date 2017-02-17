/**
 * Human interface device driver
 *
 * Kevin Cuzner
 */

#include "usb_hid.h"

static uint8_t report[] = {
    0xaa
};

/**
 * Implementation of hook_usb_handle_setup_request which implements HID class
 * requests
 */
USBControlResult hook_usb_handle_setup_request(USBSetupPacket const *setup, USBTransferData *nextTransfer)
{
    switch (setup->wRequestAndType)
    {
        case USB_REQ(0x01, USB_REQ_DIR_IN | USB_REQ_TYPE_CLS | USB_REQ_RCP_IFACE):
            //Get report request
            leds_set_center(1, 0, 0);
            nextTransfer->addr = report;
            nextTransfer->len = 1;
            return USB_CTL_OK;
        case USB_REQ(0x0A, USB_REQ_DIR_OUT | USB_REQ_TYPE_CLS | USB_REQ_RCP_IFACE):
            return USB_CTL_OK;
    }
    return USB_CTL_STALL;
}

void hook_usb_set_configuration(uint16_t configuration)
{
    usb_endpoint_setup(1, 0x80, 64, USB_ENDPOINT_INTERRUPT);

    usb_endpoint_send(1, report, sizeof(report));
}

void hook_usb_endpoint_sent(uint8_t endpoint, void *buf, uint16_t len)
{
    usb_endpoint_send(1, buf, len);
    leds_set_center(0, 1, 0);
}

