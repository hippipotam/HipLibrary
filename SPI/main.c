/*
 * main.c
 *
 *  Created on: 22 февр. 2017 г.
 *      Author: mike
 */

#include <stdlib.h>
#include "spi.h"
#include <unistd.h>

int main(int argc, char *argv[])
{
	int i;
	uint8_t byte = 0x00;
	uint8_t byte_in = 0x00;

	if (argc > 1) byte = atoi(argv[1]);

	if (SPI_init() < 0) {
		printf("SPI init error.\n");
		return 1;
	}

	usleep(1000000);

	byte_in = SPI_transfer_byte(byte);
	printf("Read byte 0x%2X\n", byte_in);

	SPI_free();

	printf("Done\n");
	return 0;
}
