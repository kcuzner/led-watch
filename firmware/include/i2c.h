/**
 * LED Wristwatch
 *
 * Kevin Cuzner
 */

#ifndef _I2C_H_
#define _I2C_H_

#include <stdint.h>
#include <stdbool.h>

/**
 * Initializes the I2C peripheral
 */
void i2c_init(void);

/**
 * Writes bytes to an I2C device
 *
 * address: I2C address, bits 7:1 are the slave address, 0 is don't care.
 * reg: Register address to write to
 * buffer: Pointer to a byte buffer to write from
 * len: Length of the buffer
 */
bool i2c_write(uint8_t address, uint8_t reg, uint8_t *buffer, uint8_t len);

/**
 * Reads bytes from an I2C device
 *
 * address: I2C address, bits 7:1 are the slave address, 0 is don't care
 * reg: Register address to read from
 * buffer: Pointer to a byte buffer to read from
 * len: Length of the buffer
 */
bool i2c_read(uint8_t address, uint8_t reg, uint8_t *buffer, uint8_t len);

#endif //_I2C_H_

