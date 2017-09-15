/*
 *  begtty.h
 *
 *   Created on: Apr 25, 2014
 *       Author: begemot
 *
*/

#ifndef BEGTTY_H
#define BEGTTY_H

#include <QString>
#include <inttypes.h>
#include <termios.h>

#define LENGTH_LINE 80
#define COMM_BUFSIZE (80)
#define TTY_SEND_WAIT 0

#define ERR_PORT_OPEN   -1
#define ERR_GET_ATTR    -2
#define ERR_SET_ATTR    -3

#include "CommandLine/CommandLine.h"

class TTY
{    
public:
    TTY(CommandLine *cmd);
    ~TTY();

    int openTTY(const char *name_port);
    void closeTTY();
    void sendByte(uchar b);
    void sendBytes(uchar *buf, int length);
    uchar receiveByte();
    uchar receiveBytes(uchar *buf, int size);

private:
    int m_tty;
    CommandLine *m_cmd;
    struct termios m_options;

    int setNewOptions();

};

#endif // BEGTTY_H
