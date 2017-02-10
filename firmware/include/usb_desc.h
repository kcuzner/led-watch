/**
 * STM32 USB Peripheral Driver
 *
 * Descriptor definitions
 *
 * Kevin Cuzner
 */
#ifndef _USB_DESC_H_
#define _USB_DESC_H_

#include <stdint.h>

typedef struct {
    uint16_t wValue;
    uint16_t wIndex;
    uint8_t length;
    void *addr;
} USBDescriptorEntry;

typedef struct {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint16_t wString[];
} USBStringDescriptor;

extern const USBDescriptorEntry usb_descriptors[];

#endif //_USB_DESC_H_

