#include <stdio.h>
#include <unistd.h>
#include "mice_pos.h"
#include "../nonblock_term.h"

int main(int argc, char *argv[])
{
	signed char x, y;
	int i;
	open_mice();
	
	set_nonblock(1);
	while (getchar() != 'q') {
		x = y = 0;
		get_mice_position(&x, &y);
		printf("                 \rx=%d, y=%d\r", x, y);
		usleep(10000);
	}
	set_nonblock(0);
	close_mice();
	printf("\nDone\n");
	return 0;
}
	
