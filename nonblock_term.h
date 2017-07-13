#pragma once

#include <unistd.h>
#include <termios.h>
#include <fcntl.h>

void set_nonblock(int state)
{
        struct termios ttystate;
        //get the terminal state
        tcgetattr(STDIN_FILENO, &ttystate);
        switch(state) {
		case 0:	//turn on canonical mode
				ttystate.c_lflag |= ECHO;
                ttystate.c_lflag |= ICANON;                
                break;
        case 1: //turn off canonical mode
                ttystate.c_lflag &= ~ICANON;               
                ttystate.c_lflag &= ~ECHO;
                ttystate.c_cc[VMIN] = 0;
                break;
        
        default: break;
        }
        //set the terminal attributes.
        tcsetattr(STDIN_FILENO, TCSANOW, &ttystate);
}

void set_nonblock_2(int fd, int state, int *flags)
{
	if (state) *flags = fcntl(fd, F_GETFL, 0);
        fcntl(fd, F_SETFL, *flags | (state ? O_NONBLOCK : 0));
}
/*
void set_nonblock_2(int state)
{
	int flags = fcntl(0, F_GETFL, 0);
	
	switch (state) {
	case 0:
		flags &= ~O_NONBLOCK;
		break;
	case 1:
		flags |= O_NONBLOCK;
		break;
	default: break;
	}
	fcntl(0, F_SETFL, flags);	
	
}
*/
