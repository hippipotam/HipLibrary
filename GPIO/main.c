/*
 * main.c
 *
 *  Created on: feb 22, 2017
 *      Author: mike
 */

#include <stdlib.h>
#include "gpio.h"
#include <unistd.h>

#define EXPORT		(uint8_t)0x01
#define UNEXPORT	(uint8_t)0x00

int main(int argc, char *argv[])
{
	int res;	
	GPIOPin_t pin_25 = { -1, 25, 0, "", 0};
	GPIOPin_t pin_26 = { -1, 26, 0, "", 0};
	GPIOPin_t pin_27 = { -1, 27, 1, "", 0};
	
	if (gpio_export(pin_25.pin, EXPORT) < 0) return 1;
	
	if (gpio_set_direction(pin_25.pin, pin_25.dir) < 0) {
		gpio_export(pin_25.pin, UNEXPORT);
		return 1;
	}
	
	pin_25.fd = gpio_open_pin(pin_25.pin, pin_25.vpath, STR_BUFFER_LENGTH, pin_25.dir);

	// ...

	gpio_close_pin(pin_25.fd);
	gpio_export(pin_25.pin, UNEXPORT);

	printf("Done\n");
	return 0;
}
