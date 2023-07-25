#include "global.h"

s32 osContStartReadData(OSMesgQueue* mq) {
    s32 ret;

    __osSiGetAccess();
    if (__osContLastCmd != 1) {
        __osPackReadData();
        __osSiRawStartDma(OS_WRITE, &__osContPifRam);
        osRecvMesg(mq, NULL, OS_MESG_BLOCK);
    }
    ret = __osSiRawStartDma(OS_READ, &__osContPifRam);
    __osContLastCmd = CONT_CMD_READ_BUTTON;
    __osSiRelAccess();
    return ret;
}

void osContGetReadData(OSContPad* contData) {
    u8* bufptr = (u8*)(&__osContPifRam);
    __OSContReadHeader read;
    s32 i;

    for (i = 0; i < __osMaxControllers; i++, bufptr += sizeof(read), contData++) {
        read = *((__OSContReadHeader*)bufptr);
        contData->errno = (read.rxsize & 0xC0) >> 4;
        if (contData->errno == 0) {
            contData->button = read.button;
            contData->stick_x = read.joyX;
            contData->stick_y = read.joyY;
        }
    };
}

void __osPackReadData(void) {
    u8* bufptr = (u8*)(&__osContPifRam);
    __OSContReadHeader read;
    s32 i;

    for (i = 0; i < 0xF; i++) {
        __osContPifRam.ram[i] = 0;
    }
    __osContPifRam.status = 1;
    read.align = 0xFF;
    read.txsize = 1;
    read.rxsize = 4;
    read.poll = CONT_CMD_READ_BUTTON;
    read.button = 0xFFFF;
    read.joyX = 0xFF;
    read.joyY = 0xFF;
    for (i = 0; i < __osMaxControllers; i++) {
        *((__OSContReadHeader*)bufptr) = read;
        bufptr += sizeof(read);
    }
    *((u8*)bufptr) = CONT_CMD_END;
}
