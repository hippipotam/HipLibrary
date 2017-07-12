#pragma once

/*
 * Mouse device /dev/input/mice
 * 		relative position
 * 	data format: 
 * 		data[0]: mouse buttons
 * 			left & 0x1, right & 0x2, middle &0x3
 * 		data[1]: x relative position
 * 		data[2]: y relative position
 * 
 */

/* Open mouse device */
int open_mice();

/* Close mouse device */
void close_mice();

/* Get relative mouse position */
void get_mice_position(signed char *x, signed char *y);

/* Set relative mouse position */
void set_mice_position(const int x, const int y);

