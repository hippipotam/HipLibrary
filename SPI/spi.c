/*
 * spi.c
 *
 *  Created on: feb 22, 2017
 *      Author: mike
 */

#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include "spi.h"
#include "../GPIO/gpio.h"

static GPIOPin_t spins[NUM_GPIO_PINS] = {
		{-1, GPIO_MOSI, MOSI_DIR, "GPIO_MOSI", "", 0},
		{-1, GPIO_MISO, MISO_DIR, "GPIO_MISO", "", 0},
		{-1, GPIO_SCLK, SCLK_DIR, "GPIO_SCLK", "", 0},
};

void write_MOSI(uint8_t level);
void write_SCLK(uint8_t level);
uint8_t read_MISO();

int SPI_init()
{
	int i, res = 0;
	
	for (i = 0; i < NUM_GPIO_PINS; i++) {
		spins[i].err = gpio_export(spins[i].pin, EXPORT);
		if (spins[i].err < 0) {
			res += spins[i].err;
			continue;
		}

		spins[i].err = gpio_set_direction(spins[i].pin, spins[i].dir);
		if (spins[i].err < 0) {
			gpio_export(spins[i].pin, UNEXPORT);
			res += spins[i].err;
			continue;
		}
		
		spins[i].fd = gpio_open_pin(spins[i].pin, spins[i].dir);
		if (spins[i].fd < 0) {
			gpio_export(spins[i].pin, UNEXPORT);
			res += spins[i].fd;
			continue;
		}
	}
		
	return res;
}

void SPI_free()
{
	int i;
	// TODO
	printf("Set MOSI to LOW\n");
	write_MOSI(LOW);
	
	for (i = 0; i < NUM_GPIO_PINS; i++) {
		gpio_close_pin(spins[i].fd);
		gpio_export(spins[i].pin, UNEXPORT);
	}

}

/* Export or unexport gpio pins */
/*int gpio_export(uint8_t export)
{
	int i, fd, res;

	fd = open((export ? EXPORT_PATH : UNEXPORT_PATH), O_WRONLY);
	if (fd < 0) {
		perror((export ? EXPORT_PATH : UNEXPORT_PATH));
		return fd;
	}
	for (i = 0; i < NUM_GPIO_PINS; i++) {
#ifdef DEBUG_MSG
		printf("[ DEBUG ] %s gpio%s (%s)...\n", (export ? "Export" : "Unexport"), spins[i].number, spins[i].name);
#endif
		res = write(fd, spins[i].number, spins[i].size_number);
#ifdef ERR_MSG
		if (res <= 0) perror("Error write");
#endif
	}
	close(fd);
	return res;
}

int gpio_set_direction()
{
	int i, fd, res;
	char dir_path[STR_BUFFER_LENGTH];

	for (i = 0; i < NUM_GPIO_PINS; i++) {
		snprintf(dir_path, STR_BUFFER_LENGTH, "%sgpio%s/direction", GPIO_PATH, spins[i].number);

		fd = open(dir_path, O_WRONLY);
		if (fd < 0) {
#ifdef ERR_MSG
			perror(dir_path);
#endif
			continue; //return fd;
		}
		res = write(fd, spins[i].dir, spins[i].dir_size);
#ifdef ERR_MSG
		if (res <= 0) perror("Error write");
#endif
		close(fd);
	}

	return res;
}

/// Open GPIO pins /value (for write/read to/from)
void open_gpioN_pins()
{
	int i;

	for (i = 0; i < NUM_GPIO_PINS; i++) {
		memset(spins[i].value, 0, STR_BUFFER_LENGTH);
		snprintf(spins[i].value, STR_BUFFER_LENGTH, "%sgpio%s/value", GPIO_PATH, spins[i].number);
#ifdef DEBUG_MSG
		printf("[ DEBUG ] path=\"%s\", %s direction: %s\n", spins[i].value, spins[i].name, (strcmp(spins[i].dir, "out") ? "O_RDONLY" : "O_WRONLY"));
#endif
		spins[i].fd = open(spins[i].value, (strcmp(spins[i].dir, "out") ? O_RDONLY : O_WRONLY));
		if (spins[i].fd < 0) {
#ifdef ERR_MSG
			perror(spins[i].value);
#endif
			continue;
		}
	}
}
*/

/*
void close_gpioN_pins()
{
	int i;
	for (i = 0; i < NUM_GPIO_PINS; i++) {
		close(spins[i].fd);
	}
}

void gpio_write_bit(EGPIO_pins_t pin, uint8_t bit)
{
	int res = -1;
#ifdef DEBUG_MSG
	printf("[ DEBUG ] Write bit \"%c\" to gpio%s...", bit, spins[pin].number);
	printf("(\"%s\")...", spins[pin].value);
	printf("\n");
#endif

	res = write(spins[pin].fd, &bit, 1);
#ifdef ERR_MSG
	if (res < 0) perror("Write to gpio");
#endif
}

uint8_t gpio_read_bit(EGPIO_pins_t pin)
{
	int res = -1;
	uint8_t rdata = 0x00;

	res = read(spins[pin].fd, &rdata, 1);
#ifdef ERR_MSG
	if (res < 0) perror("Read from gpio");
#endif

	return rdata;
}
*/

void write_MOSI(uint8_t level)
{
	gpio_write_bit(E_GPIO_MOSI, level);
}

void write_SCLK(uint8_t level)
{
	gpio_write_bit(E_GPIO_SCLK, level);
}

uint8_t read_MISO()
{
	return gpio_read_bit(E_GPIO_MISO);
}

uint8_t SPI_transfer_byte(uint8_t byte_out)
{
	uint8_t byte_in = 0;
	uint8_t bit;

#ifdef DEBUG_MSG
	printf("SPI_transfer_byte: \"%2X\" (\"%c\")\n", byte_out, byte_out);
#endif

	for (bit = 0x80; bit; bit >>= 1) {
		/* Shift-out a bit to the MOSI line */
		write_MOSI((byte_out & bit) ? HIGH : LOW);

		/* Delay for at least the peer's setup time */
		usleep(SPI_SCLK_LOW_TIME);

		/* Pull the clock line high */
		write_SCLK(HIGH);

#ifndef DEBUG
		/* Shift-in clock line high */
		if (read_MISO() == HIGH)
			byte_in |= bit;
#endif

		/* Delay for at least the peer's hold time */
		usleep(SPI_SCLK_HIGH_TIME);

		/* Pull the clock line low */
		write_SCLK(LOW);
	}
	return byte_in;
}
