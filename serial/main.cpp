#include <stdio.h>
#include "begtty.h"

int main(int argc, char *argv[])
{
	BegTTY tty;
	IOSpeedPair speed;
	speed.speed = B115200;
	if (!tty.OpenPort("/dev/ttyUSB0", &speed, 0)) return -1;
	
	// ...
	
	tty.ClosePort();
	return 0;
}
