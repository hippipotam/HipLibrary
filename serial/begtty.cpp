/*
 *  begtty.c
 *
 *   Created on: Apr 25, 2014
 *       Author: begemot
 *
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>

#include "begtty.h"

BegTTY::BegTTY() :
    m_serial(-1),
    err(0)
{}

BegTTY::~BegTTY() {}

int BegTTY::OpenPort(const char *port_name, IOSpeedPair* speed, int parity)
{
    err = 0;
    m_serial = open(port_name, O_RDWR | O_NOCTTY | O_NDELAY);

    if (m_serial == -1) {
		perror("open port");
        return ERR_PORT_OPEN;
    }

    fcntl(m_serial, F_SETFL, 0);
    tcflush(m_serial, TCIOFLUSH); /* Cleaning the I/O queue. */
    int n = fcntl(m_serial, F_GETFL, 0);
    fcntl(m_serial, F_SETFL, n & ~O_NDELAY);
    if (tcgetattr(m_serial, &m_options) != 0) {
        ClosePort();
        perror("get attributes");
        return ERR_GET_ATTR;
    }

    err = SetNewOptions(speed, parity);
    if(err < 0) {
        ClosePort();
        return err;
    }

    return err;
}

void BegTTY::ClosePort()
{
    if (m_serial != -1) {
        close(m_serial);
        m_serial = -1;
    }
}

int BegTTY::SetNewOptions(IOSpeedPair* speed, int parity)
{
    printf("Configuring port...");
    struct termios new_options;

	   /* Get the current options for the port... */
     if (tcgetattr(m_serial, &new_options) != 0) {
		perror("get attributes");
        return ERR_GET_ATTR;
	}

    cfsetispeed(&new_options, speed->speed);  /* set the baud rate */
    cfsetospeed(&new_options, speed->speed);  /* set the baud rate */

    new_options.c_cflag = (new_options.c_cflag & ~CSIZE) | CS8; /* Set 8 bits */
    new_options.c_cflag |= (CLOCAL | CREAD); /* Enable the receiver and set local mode
                                                CLOCAL - do not change "owner" of port
                                                CREAD  - Enable receiver
                                            */
    new_options.c_cflag &= ~(PARENB | PARODD);
	if (parity) new_options.c_cflag |= PARENB; /* parity */
//    options.c_cflag &= ~PARODD; /* if set, the parity is even, otherwise, the parity */
    new_options.c_cflag &= ~CSTOPB; /* off - 1 stop bit, on - 2 stop bit */
    new_options.c_cflag &= ~CRTSCTS; /* deactivating RTS+CTS */

    new_options.c_iflag = IGNBRK; /* Ignore break condition */
    new_options.c_iflag &= ~(IXON|IXOFF|IXANY);

    new_options.c_lflag = 0;
    new_options.c_oflag = 0;

    new_options.c_cc[VTIME] = 1;
    new_options.c_cc[VMIN] = 60;

    /*
   * TCSANOW Make changes immediately, without waiting for the completion of data transfer operations
   * TCSADRAIN Wait until all data has been transfered
   * TCSAFLUSH Cleaning I/O buffers and make change
   */
    if (tcsetattr(m_serial, TCSANOW, &new_options) != 0) {
		perror("set attributes");
		return ERR_SET_ATTR;
   }

    int mcs = 0;
    ioctl(m_serial, TIOCMGET, &mcs);
    mcs |= TIOCM_RTS;
    ioctl(m_serial, TIOCMSET, &mcs);

    printf("[ OK ]\n");
    return 0;
}

int BegTTY::SendByte(uint8_t b)
{
#ifdef SERIAL_DEBUG
    printf("send: 0x%02X\n", b);
#endif
    return write(m_serial, &b, 1);
}

int BegTTY::SendBytes(uint8_t *buf, int32_t length)
{
#ifdef SERIAL_DEBUG
	for (int i = 0; i < length; i++)
		printf("send: 0x%02X\n", b);
#endif
	return write(m_serial, buf, length);
}

uchar BegTTY::ReceiveByte()
{
    uchar byte = 0;

    int bytes_read = read(m_serial, &byte, 1);
    if (bytes_read < 0) {
		    printf("errorneous read result.");
        return 0;
    }
#ifdef SERIAL_DEBUG
    printf("recv: 0x%02X\n", byte);
#endif

    return byte;
}

int BegTTY::ReceiveBytes(uint8_t *buf, int32_t size)
{
    memset(buf, 0, size);

    int bytes_read = read(m_serial, buf, size);

    // if (bytes_read < 0) {
    //     printf("errorneous read result...");
    //     return 0;
    // }

#ifdef SERIAL_DEBUG
    for (int i = 0; i < bytes_read; i++)
        printf("i: %d %c: 0x%02X\n", i, buf[i], buf[i]);
#endif

    return bytes_read;
}
