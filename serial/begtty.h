/*
 *  begtty.h
 *
 *   Created on: Apr 25, 2014
 *       Author: begemot
 *
*/

#ifndef BEGTTY_H
#define BEGTTY_H

#include <inttypes.h>
#include <termios.h>

#define LENGTH_LINE 80
#define COMM_BUFSIZE (80)
#define TTY_SEND_WAIT 0

#define ERR_PORT_OPEN   -1
#define ERR_GET_ATTR    -2
#define ERR_SET_ATTR    -3

//#define TTY_DEBUG

typedef struct SIOSpeedPair {
	speed_t speed;
	int val;
} IOSpeedPair;

static const IOSpeedPair IOSpeedTable[] = {
	{B9600,9600},
	{B19200,19200},
	{B38400,38400},
	{B57600,57600},
	{B115200,115200},
	{B230400,230400},
	{B460800,460800},
	{B500000,500000},
	{B576000,576000},
	{B921600,921600},
	{B1000000,1000000},
	{B1152000,1152000},
	{B1500000,1500000},
	{B2000000,2000000},
};

typedef uint8_t uchar;

class BegTTY
{
		int m_serial;
    struct termios m_options;
    int err;

public:
    BegTTY();
    ~BegTTY();
		/* Open serial port */
    int OpenPort(const char *port_name, IOSpeedPair* speed, int parity);
    void ClosePort();
    int SendByte(uint8_t b);
    int SendBytes(uint8_t *buf, int32_t length);
    uchar ReceiveByte();
    int ReceiveBytes(uint8_t *buf, int32_t size);

private:
    int SetNewOptions(IOSpeedPair* speed, int parity);
};

#endif // BEGTTY_H
