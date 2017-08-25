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

#include <stdio.h>
#include <inttypes.h>

#define STR_BUFFER_LENGTH	80
#define MAXGPIONAME			10
#define DIR_SIZE			3

#define GPIO_PATH 	"/sys/class/gpio/"
#define EXPORT_PATH		GPIO_PATH "export"
#define UNEXPORT_PATH	GPIO_PATH "unexport"

typedef struct SGPIOPin {
	int fd;
	int pin;
	int dir;	// 0: out, 1: in
	char name[20];
	char vpath[STR_BUFFER_LENGTH];
} GPIOPin_t;

/* Export or unexport gpio pins */
int gpio_export(uint16_t pnumber, uint8_t export);

/* Set GPIO pin direction. May be "in" or "out" */
int gpio_set_direction(uint16_t pnumber, int in);

/* Open GPIO pins /value (for write/read to/from) */
int gpio_open_pin(int pin, char *value_path, int size, int dir);

/* Close GPIO pin */
void gpio_close_pin(int fd);

/* Write bit to GPIO */
void gpio_write_bit(const int fd, const uint8_t bit);

/* read bit from GPIO */
uint8_t gpio_read_bit(int fd);

#endif /* GPIO_H_ */
