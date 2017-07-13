// #include "defines.h"

typedef unsigned char uchar;

/* Init I2Cdevice */
int i2c_init(const char *filename, int addr);

/* Close I2Cdevice */
void i2c_close();

/* Set pointer address */
void i2c_set_pointer(int address, int value);

/* Read n bytes */
int i2c_read(int add1, int add2, int nbytes, unsigned char *buf);
//char *i2c_read(int add1, int add2, int nbytes, int file)

/* Write n bytes */
void i2c_write(int add1, int add2, int nbytes, unsigned char value[10]);
