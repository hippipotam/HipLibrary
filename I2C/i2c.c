
#include <stdio.h>
#include <stdlib.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#include "i2c.h"

char g_buf[10];
int com_serial;
int failcount;
int g_fd = -1;


/* Init I2Cdevice */
int i2c_init(const char *fdname, int addr)
{
	if ((g_fd = open(fdname, O_RDWR)) < 0) {
		perror("Failed to open the bus");
		com_serial = 0;
		return 0; //exit(1);
	}
	printf("Open i2c \"%s\"\n", fdname);

	if (ioctl(g_fd, I2C_SLAVE, addr) < 0) {
		perror("Failed to acquire bus access and/or talk to slave");
		com_serial = 0;
		return 0; //exit(1);
	}
	printf("Set i2c_slave addr 0x%02X\n", addr);
	return 1;
}

void i2c_close()
{
	close(g_fd);
	g_fd = -1;
}

/* Set pointer address */
void i2c_set_pointer(int address, int value)
{
	char buf[10];
	buf[0] = (uchar)address;
	buf[1] = (uchar)value;
#ifdef DEBUG
	printf("Set address 0x%02X%02X\n", buf[0], buf[1]);
#endif
	if (write(g_fd, buf, 2) != 2) {
		fprintf(stderr, "Error setting pointer\n");
		perror("Error:");
		fflush(stdout);
		com_serial = 0;
		failcount++;
	} else {
		//printf("w_0x%0*x\n", 2, buf[0]);
		//printf("w_0x%0*x\n", 2, buf[1]);
		com_serial=1;
		failcount=0;
	}
}


/* Read n bytes */
int i2c_read(int add1, int add2, int nbytes, unsigned char *buf)
{
	int n;

	i2c_set_pointer(add1, add2);
#ifdef DEBUG
	printf("Reading %d bytes...", nbytes);
#endif
	if (read(g_fd, buf, nbytes) != nbytes) {
	//if (read(fd, g_buf, nbytes) != nbytes) {
		fprintf(stderr, "\nError reading %i bytes\n", nbytes);
		perror("Error:");
		com_serial = 0;
		failcount++;
	} else {
#ifdef DEBUG
		printf("[ OK ] | ");
		for (n = 0; n < nbytes; n++)
			printf("r_0x%0*x ", 2, *buf++);
		printf("\n");
#endif
		com_serial = 1;
		failcount = 0;
	}
	return nbytes;
}

/* Write n bytes */
void i2c_write(int add1, int add2, int nbytes, unsigned char value[10])
{
	int n, i;
	uchar buf[10];
	buf[0] = (uchar)add1;
	buf[1] = (uchar)add2;

	if (nbytes >= 1) buf[2] = value[0];
	if (nbytes >= 2) buf[3] = value[1];
	if (nbytes >= 3) buf[4] = value[2];
	if (nbytes >= 4) buf[5] = value[3];
#ifdef DEBUG
	printf("Write to 0x%02x%02x | message: ", buf[0], buf[1]);
	for (i = 0; i < nbytes; i++)
		printf("%02X ", buf[i+2]);
	printf("\n");
#endif

	if (write(g_fd, buf, nbytes+2) != nbytes+2) {
		fprintf(stderr, "Error writing %i bytes\n", nbytes);
		perror("Error:");
		com_serial = 0;
		failcount++;
	} else {
#ifdef DEBUG
		printf("Write done succesfully\n");

		for (n = 0; n < (nbytes+2); n++)
			printf("w_0x%0*x\n", 2, buf[n]);
#endif
		com_serial = 1;
		failcount = 0;
	}
}
