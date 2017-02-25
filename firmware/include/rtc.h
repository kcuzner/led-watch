/**
 * LED Wristwatch
 *
 * Kevin Cuzner
 */

#ifndef _RTC_H_
#define _RTC_H_

#include <stdint.h>
#include <stdbool.h>

/**
 * Initializes the real-time clock, including the LSE oscillator
 */
void rtc_init(void);

/**
 * Returns whether or not the RTC time has been set
 */
bool rtc_is_set(void);

/**
 * Sets the RTC
 *
 * year: Year (0-99)
 * month: Month (1-12)
 * day: Day (1-31)
 * hour: Hour (0-23)
 * minute: Minute (0-59)
 * second: Second (0-59)
 */
void rtc_set(uint8_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second);

/**
 * Refreshes the program-stored RTC calendar values from the RTC module
 *
 * This ensures the various getters are synchronized
 */
void rtc_refresh(void);

/**
 * Gets the current hours value
 */
uint8_t rtc_get_hours(void);

/**
 * Gets the current minutes value
 */
uint8_t rtc_get_minutes(void);

/**
 * Gets the current seconds value
 */
uint8_t rtc_get_seconds(void);

#endif //_RTC_H_

