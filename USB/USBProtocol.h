#ifndef USBPROTOCOL_H
#define USBPROTOCOL_H

#include "begtty.h"
#include <CommandLine/CommandLine.h>
//#include "type_structure.h"
#include "../CleoLoader/type_structure.h"

const int HEADER_LENGTH = 6;


class USBProtocol
{
public:
    USBProtocol(CommandLine *cmd, QString name_port);
    ~USBProtocol();

    int initializeUSB();
    void CloseUSB();
    void freeMemory();

    int getStatus();
    void FPGAStartUploadFunc();
    void softReset();

    bool loadFPGAFirmware(uchar *buf, int length);




    void Test();

    template<typename T>
    bool memAllocAndFilling(int type);


    GetStatus       m_getStatus;
    FPGAFirmware    m_fpgaFirmware;
    SoftReset       m_softReset;
    FPGAStartUpload m_fpgaStartUpload;

    void sendBytes(uchar *buf, int length);


private:
    CommandLine *m_cmd;
    TTY *m_tty;
    uchar *m_package;
    QString m_port_name;

    int m_types_length[NUM_TYPES];

    template<typename T>
    void fillHeaderBuffer(uchar type);

    void sendStatus();


    void receiveBytes(uchar *buf, int length);
    uchar receiveByte();
    uchar receiveSTARTByte();
    uchar receiveSTOPByte();


};

#endif // USBPROTOCOL_H
