/*
 * gpio.h
 *
 *  Created on: feb 22, 2017
 *      Author: mike
 *
 * 		GPIO
 */

#ifndef GPIO_H_
#define GPIO_H_

#include <inttypes.h>

#define STR_BUFFER_LENGTH	80
#define MAXGPIONAME			10
#define DIR_SIZE			3

#define GPIO_PATH 	"/sys/class/gpio/"
#define EXPORT_PATH		GPIO_PATH "export"
#define UNEXPORT_PATH	GPIO_PATH "unexport"

typedef struct SGPIOPin {
	int fd;							// file descriptor
	int pin;						// pin number
	int dir;						// 0: out, 1: in
	char name[20];					// pin name
	char vpath[STR_BUFFER_LENGTH];	// path to "value" f.e. /sys/class/gpio/gpio25/value
	int err;
} GPIOPin_t;

/* Export or unexport gpio pins */
int gpio_export(uint16_t pnumber, uint8_t export);

/* Set GPIO pin direction. May be 1 (in) or 0 (out) */
int gpio_set_direction(uint16_t pnumber, uint8_t in);

/* Open GPIO pins /value (for write/read to/from) */
int gpio_open_pin(int pin, int dir);

/* Close GPIO pin */
void gpio_close_pin(int fd);

/* Write bit to GPIO */
void gpio_write_bit(const int fd, const uint8_t bit);

/* Read bit from GPIO */
uint8_t gpio_read_bit(const int fd);

#endif /* GPIO_H_ */
