/*
 *
 *  BlueZ - Bluetooth protocol stack for Linux
 *
 *  Copyright (C) 2001-2009  Marcel Holtmann <marcel@holtmann.org>
 *
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
static int at_command(int fd, char *cmd, int to)
{
	fd_set rfds;
	struct timeval timeout;
	char buf[1024];
	int sel, len, i, n;
	len = write(fd, cmd, strlen(cmd));	
	for (i = 0; i < 100; i++) {
		FD_ZERO(&rfds);
		FD_SET(fd, &rfds);
		timeout.tv_sec = 0;
		timeout.tv_usec = to;
		if ((sel = select(fd + 1, &rfds, NULL, NULL, &timeout)) > 0) {
			if (FD_ISSET(fd, &rfds)) {
				memset(buf, 0, sizeof(buf));
				len = read(fd, buf, sizeof(buf));
				for (n = 0; n < len; n++)
					printf("%c", buf[n]);
				if (strstr(buf, "\r\nOK") != NULL)
					break;
				if (strstr(buf, "\r\nERROR") != NULL)
					break;
				if (strstr(buf, "\r\nCONNECT") != NULL)
					break;
			}
		}		
	}
	return 0;
}
static int open_device(char *device)
{
	struct termios ti;
	int fd;
	fd = open(device, O_RDWR | O_NOCTTY | O_NONBLOCK);
	if (fd < 0) {
		fprintf(stderr, "Can't open serial port: %s (%d)\n",
							strerror(errno), errno);
		return -1;
	}
	tcflush(fd, TCIOFLUSH);
	/* Switch tty to RAW mode */
	cfmakeraw(&ti);
	tcsetattr(fd, TCSANOW, &ti);
	return fd;
}
static int open_socket(bdaddr_t *bdaddr, uint8_t channel)
{
	struct sockaddr_rc addr;
	int sk;
	sk = socket(PF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
	if (sk < 0) {
		fprintf(stderr, "Can't create socket: %s (%d)\n",
							strerror(errno), errno);
		return -1;
	}
	memset(&addr, 0, sizeof(addr));
	addr.rc_family = AF_BLUETOOTH;
	bacpy(&addr.rc_bdaddr, BDADDR_ANY);
	if (bind(sk, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
		fprintf(stderr, "Can't bind socket: %s (%d)\n",
							strerror(errno), errno);
		close(sk);
		return -1;
	}
	memset(&addr, 0, sizeof(addr));
	addr.rc_family = AF_BLUETOOTH;
	bacpy(&addr.rc_bdaddr, bdaddr);
	addr.rc_channel = channel;
	if (connect(sk, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
		fprintf(stderr, "Can't connect: %s (%d)\n",
							strerror(errno), errno);
		close(sk);
		return -1;
	}
	return sk;
}
static void usage(void)
{
	printf("Usage:\n\tattest <device> | <bdaddr> [channel]\n");
}
int main(int argc, char *argv[])
{
	int fd;
	bdaddr_t bdaddr;
	uint8_t channel;
	switch (argc) {
	case 2:
		str2ba(argv[1], &bdaddr);
		channel = 1;
		break;
	case 3:
		str2ba(argv[1], &bdaddr);
		channel = atoi(argv[2]);
		break;
	default:
		usage();
		exit(-1);
	}
	if (bacmp(BDADDR_ANY, &bdaddr)) {
		printf("Connecting to %s on channel %d\n", argv[1], channel);
		fd = open_socket(&bdaddr, channel);
	} else {
		printf("Opening device %s\n", argv[1]);
		fd = open_device(argv[1]);
	}
	if (fd < 0)
		exit(-2);
	
	// Send to the GSM/GPRS modem to test connection
	//printf("AT: ");
	at_command(fd, "AT\r\n", 10000);
	
	//"AT+CMGF: (0,1)" (0=PDU mode and 1=text mode), then SMS text mode is supported. 
	//If the response is "+CMGF: (0)", then SMS text mode is not supported.
	//printf("AT+CMGF=?: ");
	//at_command(fd, "AT+CMGF=?\r\n", 10000);
	
	//instruct the GSM / GPRS modem to operate in SMS text mode
	//printf("AT+CMGF=1: ");
	at_command(fd, "AT+CMGF=1\r\n", 10000);
	
	at_command(fd, "AT+CPBF=\"AlexR\"""\r\n", 10000);	
	
	//printf("AT+CMGW=\"+71234567890\": ");
	//at_command(fd, "AT+CMGW=\"+71234567890\"\r\nHello\r\n",10000); //"\"STO UNSENT\"<CR>This is an example for illustrating the syntax of the +CMGW AT command in SMS text mode.<Ctrl+z>\r\n", 10000);
	at_command(fd, "AT+CMGS=\"+71234567890\"\x0d",10000); //"Hello\x1a",10000);
	//at_command(fd, "AT+CMGW=\"Hello\"\r\n",10000);
	at_command(fd, "A simple demo of SMS text messaging.\r\n\x1a", 10000);
	
	// Send Message
	//at_command(fd, "AT+CMSS=1\r\n",10000);	
	//printf("ATZ: ");
	//at_command(fd, "ATZ\r\n", 10000);
	//printf("AT+CPBS=\"ME\": ");
	//at_command(fd, "AT+CPBS=\"ME\"\r\n", 10000);
	//printf("AT+CPBS=1,10: ");
	//at_command(fd, "AT+CPBR=1,10\r\n", 100000);	
	
	close(fd);
	return 0;
}
