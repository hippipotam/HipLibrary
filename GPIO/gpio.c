/*
 * gpio.c
 *
 *  Created on: feb 22, 2017
 *      Author: mike
 */

#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include "gpio.h"

/* Export or unexport gpio pins */
int gpio_export(uint16_t pnumber, uint8_t export)
{
	int i, fd, res = 0;
	char pnum[MAXGPIONAME];
	
	snprintf(pnum, MAXGPIONAME, "%d", pnumber);
	printf("%s:\n", export ? "Export" : "Unexport"); 
	printf("    port=%d ('%s')\n", pnumber, pnum);	

	fd = open((export ? EXPORT_PATH : UNEXPORT_PATH), O_WRONLY);
	if (fd < 0) {
		perror((export ? EXPORT_PATH : UNEXPORT_PATH));
		return fd;
	}
	
	res = write(fd, pnum, strlen(pnum));
	if (res <= 0) perror("Error export write");

	close(fd);
	return res;
}

/* Set GPIO pin direction. May be 1 (in) or 0 (out) */
int gpio_set_direction(uint16_t pnumber, uint8_t in)
{
	int i, fd, res = 0;
	char dir_path[STR_BUFFER_LENGTH];
	char *dir = in ? "in" : "out";

	snprintf(dir_path, STR_BUFFER_LENGTH, "%sgpio%d/direction", GPIO_PATH, pnumber);
	
	printf("Set direction:\n");
	printf("    port=%d\n", pnumber);
	printf("    path=%s\n", dir_path);
	printf("    dir =%s\n", dir);

	fd = open(dir_path, O_WRONLY);
	if (fd < 0) {
		perror(dir_path);
		return fd;
	}

	res = write(fd, dir, strlen(dir));
	if (res <= 0) perror("Error set direction write");
	close(fd);

	return res;
}

/* Open GPIO pin /value (for write/read to/from) */
int gpio_open_pin(int pin, int dir)
{
	int i, fd;
	char vpath[STR_BUFFER_LENGTH]; // path to "value"
	
	memset(vpath, 0, sizeof(vpath));
	snprintf(vpath, sizeof(vpath), "%sgpio%d/value", GPIO_PATH, pin);
	
	fd = open(vpath, (dir ? O_WRONLY : O_RDONLY));
	if (fd < 0) perror(vpath);
	return fd;
}

/* Close GPIO pin */
void gpio_close_pin(int fd)
{
	if (fd >= 0) close(fd);
}

/* Write bit to GPIO */
void gpio_write_bit(const int fd, const uint8_t bit)
{
	int res;
	res = write(fd, &bit, 1);
	if (res < 0) perror("Write to gpio");
}

/* Read bit from GPIO */
uint8_t gpio_read_bit(const int fd)
{
	int res;
	uint8_t rdata = 0x00;

	res = read(fd, &rdata, 1);
	if (res < 0) perror("Read from gpio");

	return rdata;
}
