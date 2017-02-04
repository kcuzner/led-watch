/**
 * LED Wristwatch
 *
 * Kevin Cuzner
 */

#ifndef _MMA8652_H_
#define _MMA8652_H_

#include <stdbool.h>

/**
 * Initializes the MMA8652, must be called after i2c_init otherwise it will hang
 * indefinitely
 */
bool mma8652_init(void);

/**
 * Hook called when the mma8652 is tapped in the Z direction (i.e. on the watch face)
 */
void hook_mma8652_tap(void);

#endif //_MMA8652_H_

