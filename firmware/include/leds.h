/**
 * LED Wristwatch
 *
 * Kevin Cuzner
 */

#include <stdint.h>

/**
 * Initializes the LEDs
 *
 * timer: Timer to use for display timing
 */
void leds_init(void);

/**
 * Starts the LED display
 */
void leds_enable(void);

/**
 * Stops the LED display
 */
void leds_disable(void);

/**
 * Clears the LEDs in the current buffer
 */
void leds_clear(void);

/**
 * Sets a minute LED to some level
 *
 * led: LED to set (0-59)
 * level: Level (0-3)
 */
void leds_set_minute(uint8_t led, uint8_t level);

/**
 * Sets an hour LED to some level
 *
 * led: LED to set (0-11)
 * level: Level (0-3)
 */
void leds_set_hour(uint8_t led, uint8_t level);

/**
 * Sets the center LED to some level
 * red: Red level (0-3)
 * green: Green level (0-3)
 * blue: Blue level (0-3) (NOTE: The blue LED has been disabled in Rev A hardware because of mistakes)
 */
void leds_set_center(uint8_t red, uint8_t green, uint8_t blue);

/**
 * Commits the LED edits and displays them
 */
void leds_commit(void);

