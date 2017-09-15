#include <QThread>
#include <QDebug>

#include "USBProtocol.h"


//#define DUMMY

USBProtocol::USBProtocol(CommandLine *cmd, QString name_port) :
    m_cmd(cmd),
    m_tty(NULL),    
    m_package(NULL),
    m_port_name(name_port)
{
    qDebug() << "[ USBProtocol ] Constructor.";
    memset(&m_getStatus, 0, sizeof(m_getStatus));
    memset(&m_fpgaFirmware, 0, sizeof(m_fpgaFirmware));
    memset(&m_softReset, 0, sizeof(m_softReset));
    memset(&m_fpgaStartUpload, 0, sizeof(m_fpgaStartUpload));

    // fill array
//    m_types_length[0] = -1;
    m_types_length[0] = sizeof(m_getStatus);
    m_types_length[1] = sizeof(m_fpgaFirmware);
    m_types_length[2] = sizeof(m_softReset);
    m_types_length[3] = sizeof(m_fpgaStartUpload);
    for(int i = 0; i < NUM_TYPES; ++i)
        qDebug() << "Types_length: " << m_types_length[i];
}

USBProtocol::~USBProtocol()
{
    qDebug() << "[ USBProtocol ] Destructor.";
    CloseUSB();
}

void USBProtocol::Test()
{
    emit m_cmd->sendOutput("USB protocol loaded.", false);
}

int USBProtocol::initializeUSB()
{
#ifdef DUMMY
    return 1;
#endif
    m_tty = new TTY(m_cmd);

    if(m_tty->openTTY(qPrintable(m_port_name)) < 0) {
        emit m_cmd->sendOutput("Error: initialize USB\n", false);
        return 0;
    }
    emit m_cmd->sendOutput(QString("Initialize %1 [ OK ]").arg(m_port_name), false);
    return 1;
}

void USBProtocol::CloseUSB()
{
    if(m_tty != NULL) {
        m_tty->closeTTY();
        delete m_tty;
        m_tty = NULL;
    }
}

void USBProtocol::freeMemory()
{
    if(m_package != NULL) {
        free(m_package);
        m_package = NULL;
    }
}

void USBProtocol::sendBytes(uchar *buf, int length)
{
    // Dummy
    qDebug() << "[ USBProtocol ] sendBytes " << length;
    emit m_cmd->sendOutput(QString("[ OK ] Send %1 bytes").arg(length), false);
    QThread::usleep(1000);
#ifndef DUMMY
    m_tty->sendBytes(buf, length);
#endif
}

uchar USBProtocol::receiveByte()
{
#ifdef DUMMY
    return 0;
#endif
    return m_tty->receiveByte();
}

uchar USBProtocol::receiveSTARTByte()
{
    return START;
}

uchar USBProtocol::receiveSTOPByte()
{
    return STOP;
}

void USBProtocol::receiveBytes(uchar *buf, int length)
{
    qDebug() << "[ USBProtocol ] receiveBytes";
#ifdef DUMMY
    buf[0] = 1;
    buf[1] = 0;
    buf[2] = 0;
    buf[3] = 0;
    buf[4] = 1;
    emit m_cmd->sendOutput(QString("[ OK ] Send %1 bytes").arg(length), false);
    return;
#endif
    m_tty->receiveBytes(buf, length);
}

template<typename T>
void USBProtocol::fillHeaderBuffer(uchar type)
{
    Q_ASSERT(m_package != NULL);
    m_package[0] = (uchar) START;
    m_package[1] = type;
    *(int *) (&m_package[2]) = sizeof(T);
    std::swap(m_package[2], m_package[5]);
    std::swap(m_package[3], m_package[4]);
}

template<typename T>
bool USBProtocol::memAllocAndFilling(int type)
{
    int length = HEADER_LENGTH + (int)sizeof(T) + 1;
    m_package = (uchar *) malloc(length);
    if(!m_package) {
        emit m_cmd->sendOutput("Error allocate memory", true);
        return false;
    }
//    memset(&m_package, 0, length);
    qDebug() << "m_package: " << sizeof(m_package);
    fillHeaderBuffer<T>(type);

    m_package[length-1] = (uchar) STOP;
    return true;
}


void USBProtocol::sendStatus()
{
    if(!memAllocAndFilling<GetStatus>(0))
        return;

    for(int i = 0; i < 8; ++i)
        qDebug() << "[" << i << "]: " << m_package[i];

#ifndef DUMMY
    m_tty->sendBytes(m_package, sizeof(m_package));
#endif
}

int USBProtocol::getStatus()
{
    qDebug() << "[ FPGALoader ] getStatus";
    sendStatus();

    while(1) {
#ifdef DUMMY
        m_package[0] = receiveSTARTByte();
#else
        emit m_cmd->sendOutput(QString("[  USBProtocol ] start recv"), false);
        m_package[0] = m_tty->receiveByte();
#endif
        emit m_cmd->sendOutput(QString("[  USBProtocol ] Receive %1").arg(m_package[0], 0, 16), false);
        qDebug() << m_package[0];
        if(m_package[0] == START) {
            emit m_cmd->sendOutput("[ OK ] Receive START...", false);
            break;
        } else emit m_cmd->sendOutput("[ FALSE ] Receive START...", false);
//        QThread::msleep(200);
    }

#ifdef DUMMY
    m_package[2] = 0;
    m_package[3] = 0;
    m_package[4] = 0;
    m_package[5] = 1;
    m_package[6] = 3;
    m_package[7] = receiveSTOPByte();
#else
    m_tty->receiveBytes(&(m_package[1]), 5);
    std::swap(m_package[2], m_package[5]);
    std::swap(m_package[3], m_package[4]);
    int length = *(int *)(&m_package[2]);
    qDebug() << length;
    m_tty->receiveBytes(&(m_package[6]), length);
    m_package[length -1] = m_tty->receiveByte();
#endif
    if(!(m_package[7] == STOP)) {
        emit m_cmd->sendOutput("Broken message", false);
        return -1;
    }
    return m_package[6];
}

void USBProtocol::FPGAStartUploadFunc()
{
    if(!memAllocAndFilling<FPGAStartUpload>(3))
        return;

    *(int *)(&m_package[6]) = (int)FEED;
    std::swap(m_package[7], m_package[8]);

    for(int i = 0; i < 9; ++i)
        qDebug() << "[" << i << "]: " << m_package[i];
#ifndef DUMMY
    m_tty->sendBytes(m_package, sizeof(m_package));
#endif
}

void USBProtocol::softReset()
{
    if(!memAllocAndFilling<SoftReset>(2))
        return;

    *(int *)(&m_package[6]) = (int)DEAD;
    std::swap(m_package[7], m_package[8]);

    for(int i = 0; i < 9; ++i)
        qDebug() << "[" << i << "]: " << m_package[i];

#ifndef DUMMY
    m_tty->sendBytes(m_package, sizeof(m_package));
#endif
}

bool USBProtocol::loadFPGAFirmware(uchar *buf, int length)
{
    qDebug() << "[ USBProtocol ] loadFPGAFirmware length: " << length;
#ifndef DUMMY
    int len = HEADER_LENGTH + (int)sizeof(FPGAFirmware) + 1;
#endif
    if(!memAllocAndFilling<FPGAFirmware>(1))
        return false;

    memcpy(&m_package[6], buf, length);

#ifndef DUMMY
    m_tty->sendBytes(buf, len);
#endif
    return true;
}




