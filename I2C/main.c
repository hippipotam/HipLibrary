#include <stdio.h>
#include "i2c.h"

int main(int argc, char *argv[])
{
	printf("I2C test\n");
	const char *fname = "/dev/i2c-1";
	int res = 0;
	uchar slave_i2c_addr = 0x00;
	if (!i2c_init(fname, slave_i2c_addr)) return 1;

	// ...

	i2c_close();
	printf("Done\n");
	return 0;
}
