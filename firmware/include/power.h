/**
 * LED Wristwatch
 *
 * Kevin Cuzner
 */

#ifndef _POWER_H_
#define _POWER_H_

#include <stdint.h>

/**
 * Power management
 *
 * The LED Wristwatch has the ability to tell when USB is plugged in and
 * the general battery charge status. It does not have the ability to
 * gauge the battery discharge state or the power load (maybe a future
 * version).
 *
 * This module controls the power status of the STM32, including
 * entering/exiting sleep mode, determining if USB is present, and
 * reporting the charge status. Other modules may hook into this one to
 * perform actions when the device is awake or to request that the
 * device stay awake longer.
 */

typedef enum { POWER_BATTERY_DISCHARGING, POWER_BATTERY_CHARGING, POWER_BATTERY_CHARGED } PowerBatteryState;

/**
 * Initializes power management
 */
void power_init(void);

/**
 * Main method for power management, never exits.
 */
void power_main(void);

/**
 * Returns the current battery state
 */
PowerBatteryState power_get_battery_state(void);

/**
 * Sets the time that the device should remain awake. This must be called
 * before a waking interrupt exits in order to prevent the device from
 * going back to sleep.
 *
 * ticks: Number of ticks to stay awake (TODO: Determine what a tick is)
 */
void power_set_awake_time(uint16_t ticks);

/**
 * Hook function implemented by the application which is called
 * repeatedly while the device is awake. The application should exit
 * this function as quickly as possible.
 */
void hook_power_awake(void);

/**
 * Hook function implemented by the application which is called when the
 * device wakes from sleep or stop mode. This hook is called during the
 * main execution loop, not during an interrupt.
 */
void hook_power_on_wake(void);

/**
 * Hook function implemented by the application which is called when the
 * device is about to enter sleep mode. Calls to power_set_awake_time
 * during this function are ignored.
 */
void hook_power_on_sleep(void);

#endif //_POWER_H_

