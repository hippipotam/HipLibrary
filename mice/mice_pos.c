#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#include "mice_pos.h"

int g_mice_fd = -1;
unsigned char data[3];
int left, middle, right;
int g_flags;
const char *pDevice = "/dev/input/mice";

int open_mice()
{
	g_mice_fd = open(pDevice, O_RDWR);
	if (g_mice_fd == -1) {
        	perror("Error: cannot open mouse device");
		return -1;
	} else printf("The mouse device was opened successfully.\n");
    
	// Set non-blocking
	set_nonblock_2(g_mice_fd, 1, &g_flags);
	
	return 0;
}

void close_mice()
{
	if (g_mice_fd < 0) return;
	// Restore flags
	set_nonblock_2(g_mice_fd, 0, &g_flags);
		
	close(g_mice_fd);
	g_mice_fd = -1;		
}


void get_mice_position(signed char *x, signed char *y)
{
	if (g_mice_fd < 0) return;

	int bytes = read(g_mice_fd, data, sizeof(data));	
	if (bytes > 0) {
		left = data[0] & 0x1;
		right = data[0] & 0x2;
		middle = data[0] & 0x3;
		
		*x = data[1];
		*y = data[2];
	}
}

void set_mice_position(const int x, const int y)
{
	if (g_mice_fd < 0) return;
	// May be set a relative position?
	data[0] = 0;
	data[1] = x;
	data[2] = y;
	write(g_mice_fd, data, sizeof(data));
}
