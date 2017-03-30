/**
 * STM32 USB Peripheral Driver
 *
 * Descriptor definitions
 *
 * Kevin Cuzner
 */

#include "usb_desc.h"
#include "usb.h"
#include "usb_hid.h"

#include <stddef.h>
#include <stdint.h>

/**
 * Device descriptor
 */
static const USB_DATA_ALIGN uint8_t dev_descriptor[] = {
    18, //bLength
    1, //bDescriptorType
    0x00, 0x02, //bcdUSB
    0x00, //bDeviceClass (defined by interfaces)
    0x00, //bDeviceSubClass
    0x00, //bDeviceProtocl
    USB_CONTROL_ENDPOINT_SIZE, //bMaxPacketSize0
    0xc0, 0x16, //idVendor
    0xdc, 0x05, //idProduct
    0x11, 0x00, //bcdDevice
    1, //iManufacturer
    2, //iProduct
    0, //iSerialNumber,
    1, //bNumConfigurations
};

static const USB_DATA_ALIGN uint8_t hid_report_descriptor[] = {
    HID_SHORT(0x04, 0x00, 0xFF), //USAGE_PAGE (Vendor Defined)
    HID_SHORT(0x08, 0x01), //USAGE (Vendor 1)
    HID_SHORT(0xa0, 0x01), //COLLECTION (Application)
    HID_SHORT(0x08, 0x01), //  USAGE (Vendor 1)
    HID_SHORT(0x14, 0x00), //  LOGICAL_MINIMUM (0)
    HID_SHORT(0x24, 0xFF, 0x00), //LOGICAL_MAXIMUM (0x00FF)
    HID_SHORT(0x74, 0x08), //  REPORT_SIZE (8)
    HID_SHORT(0x94, 0x08), //  REPORT_COUNT(8)
    HID_SHORT(0x80, 0x02), //  INPUT (Data, Var, Abs)
    HID_SHORT(0x08, 0x01), //  USAGE (Vendor 1)
    HID_SHORT(0x90, 0x02), //  OUTPUT (Data, Var, Abs)
    HID_SHORT(0xc0),       //END_COLLECTION
};

/**
 * Configuration descriptor
 */
static const USB_DATA_ALIGN uint8_t cfg_descriptor[] = {
    9, //bLength
    2, //bDescriptorType
    9 + 9 + 9 + 7 + 7, 0x00, //wTotalLength
    1, //bNumInterfaces
    1, //bConfigurationValue
    0, //iConfiguration
    0x80, //bmAttributes
    250, //bMaxPower
    /* INTERFACE 0 BEGIN */
    9, //bLength
    4, //bDescriptorType
    0, //bInterfaceNumber
    0, //bAlternateSetting
    2, //bNumEndpoints
    0x03, //bInterfaceClass (HID)
    0x00, //bInterfaceSubClass (0: no boot)
    0x00, //bInterfaceProtocol (0: none)
    0, //iInterface
        /* HID Descriptor */
        9, //bLength
        0x21, //bDescriptorType (HID)
        0x11, 0x01, //bcdHID
        0x00, //bCountryCode
        1, //bNumDescriptors
        0x22, //bDescriptorType (Report)
        sizeof(hid_report_descriptor), 0x00,
        /* INTERFACE 0, ENDPOINT 1 BEGIN */
        7, //bLength
        5, //bDescriptorType
        0x81, //bEndpointAddress (endpoint 1 IN)
        0x03, //bmAttributes, interrupt endpoint
        USB_CONTROL_ENDPOINT_SIZE, 0x00, //wMaxPacketSize,
        10, //bInterval (10 frames)
        /* INTERFACE 0, ENDPOINT 1 END */
        /* INTERFACE 0, ENDPOINT 2 BEGIN */
        7, //bLength
        5, //bDescriptorType
        0x02, //bEndpointAddress (endpoint 2 OUT)
        0x03, //bmAttributes, interrupt endpoint
        USB_CONTROL_ENDPOINT_SIZE, 0x00, //wMaxPacketSize
        10, //bInterval (10 frames)
        /* INTERFACE 0, ENDPOINT 2 END */
    /* INTERFACE 0 END */
};

static const USB_DATA_ALIGN uint8_t lang_descriptor[] = {
    4, //bLength
    3, //bDescriptorType
    0x09, 0x04 //wLANGID[0]
};

static const USB_DATA_ALIGN uint8_t manuf_descriptor[] = {
    2 + 15 * 2, //bLength
    3, //bDescriptorType
    'k', 0x00, //wString
    'e', 0x00,
    'v', 0x00,
    'i', 0x00,
    'n', 0x00,
    'c', 0x00,
    'u', 0x00,
    'z', 0x00,
    'n', 0x00,
    'e', 0x00,
    'r', 0x00,
    '.', 0x00,
    'c', 0x00,
    'o', 0x00,
    'm', 0x00
};

static const USB_DATA_ALIGN uint8_t product_descriptor[] = {
    2 + 14 * 2, //bLength
    3, //bDescriptorType
    'L', 0x00,
    'E', 0x00,
    'D', 0x00,
    ' ', 0x00,
    'W', 0x00,
    'r', 0x00,
    'i', 0x00,
    's', 0x00,
    't', 0x00,
    'w', 0x00,
    'a', 0x00,
    't', 0x00,
    'c', 0x00,
    'h', 0x00
};

const USBDescriptorEntry usb_descriptors[] = {
    { 0x0100, 0x0000, sizeof(dev_descriptor), dev_descriptor },
    { 0x0200, 0x0000, sizeof(cfg_descriptor), cfg_descriptor },
    { 0x0300, 0x0000, sizeof(lang_descriptor), lang_descriptor },
    { 0x0301, 0x0409, sizeof(manuf_descriptor), manuf_descriptor },
    { 0x0302, 0x0409, sizeof(product_descriptor), product_descriptor },
    { 0x2200, 0x0000, sizeof(hid_report_descriptor), hid_report_descriptor },
    { 0x0000, 0x0000, 0x00, NULL }
};

