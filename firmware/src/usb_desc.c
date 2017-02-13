/**
 * STM32 USB Peripheral Driver
 *
 * Descriptor definitions
 *
 * Kevin Cuzner
 */

#include "usb_desc.h"
#include "usb.h"

#include <stddef.h>
#include <stdint.h>

//These must be half-word aligned to avoid a hard fault. I believe it is due to
//the fact that these are copied by halfword and halfword accesses seem to be
//only allowed on halfword boundaries. This could be completely wrong as it is
//a guess based on my observation.
//
//Specifically, the case was that one of these consts happened to land on
//address 0x08000155f and then the LDRH instruction in the usb sending function
//(which copies these into the PMA) would explode with a hard fault. Removing
//the cfg_descriptor (which has a odd length) would suddenly make it work since
//the addresses would then become halfword aligned again.
//
//I believe these are being compacted to 1-byte alignment by default due to the
//Os flag, but that's another guess. No hard faults occurred when the flag was
//removed.
#define FLASH_ALIGN __attribute__ ((aligned(2)))

/**
 * Device descriptor
 */
static const FLASH_ALIGN uint8_t dev_descriptor[] = {
    18, //bLength
    1, //bDescriptorType
    0x00, 0x02, //bcdUSB
    0xff, //bDeviceClass
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

/**
 * Configuration descriptor
 */
static const FLASH_ALIGN uint8_t cfg_descriptor[] = {
    9, //bLength
    2, //bDescriptorType
    9 + 9 + 7, 0x00, //wTotalLength
    1, //bNumInterfaces
    1, //bConfigurationValue,
    0, //iConfiguration
    0x80, //bmAttributes
    250, //bMaxPower
    /* INTERFACE 0 BEGIN */
    9, //bLength
    4, //bDescriptorType
    0, //bInterfaceNumber
    0, //bAlternateSetting
    1, //bNumEndpoints
    0xff, //bInterfaceClass
    0x00, //bInterfaceSubClass,
    0x00, //bInterfaceProtocol
    0, //iInterface
        /* INTERFACE 0, ENDPOINT 1 BEGIN */
        7, //bLength
        5, //bDescriptorType,
        0x81, //bEndpointAddress,
        0x02, //bmAttributes, bulk endpoint
        USB_CONTROL_ENDPOINT_SIZE, 0x00, //wMaxPacketSize,
        0 //bInterval
        /* INTERFACE 0, ENDPOINT 1 END */
    /* INTERFACE 0 END */
};

static const FLASH_ALIGN uint8_t lang_descriptor[] = {
    4, //bLength
    3, //bDescriptorType
    0x09, 0x04 //wLANGID[0]
};

static const FLASH_ALIGN uint8_t manuf_descriptor[] = {
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

static const FLASH_ALIGN uint8_t product_descriptor[] = {
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

const FLASH_ALIGN USBDescriptorEntry usb_descriptors[] = {
    { 0x0100, 0x0000, sizeof(dev_descriptor), dev_descriptor },
    { 0x0200, 0x0000, sizeof(cfg_descriptor), cfg_descriptor },
    { 0x0300, 0x0000, sizeof(lang_descriptor), lang_descriptor },
    { 0x0301, 0x0409, sizeof(manuf_descriptor), manuf_descriptor },
    { 0x0302, 0x0409, sizeof(product_descriptor), product_descriptor },
    { 0x0000, 0x0000, 0x00, NULL }
};

