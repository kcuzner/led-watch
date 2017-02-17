/**
 * Human interface device driver
 *
 * Hooks into the USB core driver
 *
 * Kevin Cuzner
 */

#ifndef _USB_HID_H_
#define _USB_HID_H_

#include "usb.h"
#include "macro_helpers.h"

/**
 * Enum for various report types
 */
typedef enum { USB_HID_IN, USB_HID_OUT, USB_HID_FEATURE } USBHIDReportType;

/**
 * Sets the data for the next IN report that will be sent
 */
void usb_hid_set_in_report(const USBTransferData *report);

/**
 * Sets the buffer location for receiving the next OUT report that may be
 * received.
 */
void usb_hid_set_out_report(const USBTransferData *buffer);

/**
 * Hook function implemented by the application which is called whenever the
 * host requests a specific report by way of control transfer. The device must
 * provide data for the requested report.
 *
 * type: Report type requested
 * reportId: ID of the report requested (zero if report ids are not used)
 * report: Transfer data populated during thsi function with a buffer for the report
 */
void usb_hook_hid_get_report(USBHIDReportType type, uint8_t reportId, USBTransferData *report);

/**
 * Hook function implemented by the application which is called whenever an OUT
 * report is received from the host.
 *
 * report: Report received, if a buffer has been set up.
 */
void usb_hook_hid_out_report(const USBTransferData *report);


/**
 * HID Descriptor Helpers
 */
#define HID_SHORT_ZERO(TAGTYPE) (TAGTYPE | 0)
#define HID_SHORT_MANY(TAGTYPE, ...) (TAGTYPE | (NUMARGS(__VA_ARGS__) & 0x3)), __VA_ARGS__
#define GET_HID_SHORT(_1, _2, _3, _4, _5, NAME, ...) NAME
#define HID_SHORT(...) GET_HID_SHORT(__VA_ARGS__, HID_SHORT_MANY, HID_SHORT_MANY, HID_SHORT_MANY, HID_SHORT_MANY, HID_SHORT_ZERO)(__VA_ARGS__)

#endif //_USB_HID_H_

