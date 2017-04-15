/**
 * Human interface device driver
 *
 * Kevin Cuzner
 */

#include "usb_hid.h"

void __attribute__((weak)) hook_usb_hid_in_report(const USBTransferData *report) { }
void __attribute__((weak)) hook_usb_hid_out_report(const USBTransferData *report) { }

void usb_hid_set_in_report(const USBTransferData *report)
{
    usb_endpoint_send(1, report->addr, report->len);
}

void usb_hid_set_out_report(const USBTransferData *buffer)
{
    usb_endpoint_receive(2, buffer->addr, buffer->len);
}

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
            //nextTransfer->addr = report_in;
            //nextTransfer->len = sizeof(report_in);
            return USB_CTL_STALL;
//            return USB_CTL_OK;
        case USB_REQ(0x0A, USB_REQ_DIR_OUT | USB_REQ_TYPE_CLS | USB_REQ_RCP_IFACE):
            return USB_CTL_STALL;
//            return USB_CTL_OK;
    }
    return USB_CTL_STALL;
}

void hook_usb_set_configuration(uint16_t configuration)
{
    usb_endpoint_setup(1, 0x81, 64, USB_ENDPOINT_INTERRUPT);
    usb_endpoint_setup(2, 0x02, 64, USB_ENDPOINT_INTERRUPT);
}

void hook_usb_endpoint_sent(uint8_t endpoint, void *buf, uint16_t len)
{
    USBTransferData report = { buf, len };
    hook_usb_hid_in_report(&report);
}

void hook_usb_endpoint_received(uint8_t endpoint, void *buf, uint16_t len)
{
    USBTransferData report = { buf, len };
    hook_usb_hid_out_report(&report);
}

