/**
 * LED Wristwatch
 *
 * Kevin Cuzner
 */

#ifndef _OSC_H_
#define _OSC_H_

#include <stdint.h>

#define OSC_MAX_CALLBACKS 16

typedef void (*OscChangeCallback)(void);

/**
 * Requests the primary oscillator to change to the HSI16
 */
void osc_request_hsi16(void);

/**
 * Requests the primary oscillator to change to the MSI
 * 
 * range: Value 0-6 denoting the desired MSI frequency range
 */
void osc_request_msi(uint8_t range);

/**
 * Adds a callback function to the list called when the oscillator
 * frequency is changed
 */
void osc_add_callback(OscChangeCallback fn);

#endif //_OSC_H_

