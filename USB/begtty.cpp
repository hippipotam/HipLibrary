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

#include <QDebug>
#include <QString>
#include <QThread>

#include "begtty.h"

TTY::TTY(CommandLine *cmd) :
    m_tty(-1),
    m_cmd(cmd)
{}

TTY::~TTY() {}

int TTY::openTTY(const char *name_port)
{
    m_tty = open(name_port, O_RDWR | O_NOCTTY | O_NDELAY);

    if (m_tty == -1) {
        return ERR_PORT_OPEN;
    }

    fcntl(m_tty, F_SETFL, 0);
    tcflush(m_tty, TCIOFLUSH); /* Cleaning the I/O queue. */
    int n = fcntl(m_tty, F_GETFL, 0);
    fcntl(m_tty, F_SETFL, n & ~O_NDELAY);
    if (tcgetattr(m_tty, &m_options) != 0) {
        closeTTY();
        return ERR_GET_ATTR;
    }

    int err = setNewOptions();
    if(err < 0) {
        closeTTY();
        return err;
    }

    return 0;
}

void TTY::closeTTY()
{
    if (m_tty != -1) {
        close(m_tty);
        m_tty = -1;
    }
}

int TTY::setNewOptions()
{
    printf("Configuring port...");
    struct termios new_options;

    if (tcgetattr(m_tty, &new_options) != 0) /* Get the current options for the port... */
        return ERR_GET_ATTR;
//       errorh("tcgetattr() 3 failed\n");

    cfsetispeed(&new_options, B230400);  /* set the baud rate */
    cfsetospeed(&new_options, B230400);  /* set the baud rate */

    new_options.c_cflag = (new_options.c_cflag & ~CSIZE) | CS8; /* Set 8 bits */
    new_options.c_cflag |= (CLOCAL | CREAD); /* Enable the receiver and set local mode
                                                CLOCAL - do not change "owner" of port
                                                CREAD  - Enable receiver
                                            */
    new_options.c_cflag &= ~(PARENB | PARODD);
    new_options.c_cflag |= PARENB;  /* parity */
//    options.c_cflag &= ~PARODD; /* if set, the parity is even, otherwise, the parity */
    new_options.c_cflag &= ~CSTOPB; /* off - 1 stop bit, on - 2 stop bit */
    new_options.c_cflag &= ~CRTSCTS; /* deactivating RTS+CTS */

    new_options.c_iflag = IGNBRK; /* Ignore break condition */
    new_options.c_iflag &= ~(IXON|IXOFF|IXANY);

    new_options.c_lflag = 0;    // noncanonical mode
    new_options.c_oflag = 0;

//    new_options.c_lflag = ~ICANON;
    new_options.c_cc[VTIME] = 1;
    new_options.c_cc[VMIN] = 60;

    /*
   * TCSANOW Make changes immediately, without waiting for the completion of data transfer operations
   * TCSADRAIN Wait until all data has been transfered
   * TCSAFLUSH Cleaning I/O buffers and make change
   */
    if (tcsetattr(m_tty, TCSANOW, &new_options) != 0)
       return ERR_SET_ATTR;

    int mcs = 0;
    ioctl(m_tty, TIOCMGET, &mcs);
    mcs |= TIOCM_RTS;
    ioctl(m_tty, TIOCMSET, &mcs);

    printf("[ OK ]\n");
    return 0;
}

void TTY::sendByte(uchar b)
{
    int res = write(m_tty, &b, 1);

    if (res<1) {
        emit m_cmd->sendOutput(QString("write returned %1\nerrno = %2").arg(res).arg(errno), false);
        return;
    }
#ifdef TTY_DEBUG
    else emit m_cmd->sendOutput(QString("w: %1").arg(b, 0, 16), false);
#endif
}

uchar TTY::receiveByte()
{
    uchar byte = 0;

    int bytes_read = read(m_tty, &byte, 1);
//    emit m_cmd->sendOutput(QString("recv: %1").arg(byte, 0, 16), false);
    qDebug() << "[ TTY::receiveByte ] " << QString("%1").arg(byte, 0, 16);
    if (bytes_read < 0) {
        emit m_cmd->sendOutput(QString("errorneous read result: %1").arg(bytes_read), false);
        return 0;
    }

#ifdef TTY_DEBUG
    emit m_cmd->sendOutput(QString("r: %1").arg(byte, 0, 16), false);
#endif

    return byte;
}

void TTY::sendBytes(uchar *buf, int length)
{
    int res = write(m_tty, buf, length);

    if (res<1) {
        emit m_cmd->sendOutput(QString("write returned %1\nerrno = %2").arg(res).arg(errno), false);
        return;
    }

//    for(int i = 0; i < length; ++i) {
//        sendByte(buf[i]);
//        QThread::usleep(100);
//    }
}

uchar TTY::receiveBytes(uchar *buf, int size)
{
    memset(buf, 0, size);

    int bytes_read = read(m_tty, buf, size);

//    for(int i = 0; i < size; ++i) {
//        emit m_cmd->sendOutput(QString("recv %1: %2").arg(i).arg(buf[i], 0, 16), false);
//        qDebug() << "[ TTY::receiveByte ] " << i << ":" << QString("%1").arg(buf[i], 0, 16);
//    }
    if (bytes_read < 0) {
        emit m_cmd->sendOutput(QString("errorneous read result: %1").arg(bytes_read), false);
        return 0;
    }

#ifdef TTY_DEBUG
    for (i = 0; i < bytes_read; i++)
        printf("i: %d %c: 0x%02X\n", i, g_buf[i], g_buf[i]);
#endif

    return 1;
}









