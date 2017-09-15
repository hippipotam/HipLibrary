/*
 * Structures type
 * 
*/


#ifndef TYPE_STRUCTURE_H_
#define TYPE_STRUCTURE_H_

extern "C" {
    #include <inttypes.h>
}

#define MAXSIZE_BUF	32768 // 65536 //
#define START       157
#define STOP        98
#define NUM_TYPES   4
#define FEED        16706816
#define DEAD        14593280

// Register structure
typedef struct GetStatusS {
    uint8_t board_ready:1;
    uint8_t fpga_upload_ready:1;
} GetStatus;

// Firmware to FPGA structure
typedef struct FPGAFirmwareS {
    uint32_t size;
    uint8_t buf[MAXSIZE_BUF];
    uint8_t hash;
} FPGAFirmware;

// Soft Reset board structure
typedef struct SoftResetS {
    uint32_t apply;
} SoftReset;

// FPGA Start Upload structure
typedef struct FPGAStartUploadS {
    uint32_t apply;
} FPGAStartUpload;



#endif
