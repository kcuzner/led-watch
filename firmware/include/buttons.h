/**
 * LED Wristwatch
 *
 * Kevin Cuzner
 */

#ifndef _BUTTONS_H_
#define _BUTTONS_H_

#include <stdint.h>

/**
 * Initializes the buttons
 */
void buttons_init(void);

/**
 * Gets the button state
 *
 * Returns the button state, one bit per button
 */
uint8_t buttons_get_state(void);

/**
 * Application hook function called when the state of the buttons changes
 *
 * state: Button state, one bit per button
 */
void hook_buttons_state_changed(uint8_t state);

#endif //_BUTTONS_H_

