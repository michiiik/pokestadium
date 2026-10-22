#include "flash.h"
#include "PR/os_internal_flash.h"

static u32 gFlashID[4];      // __osFlashID
static OSIoMesg gFlashMsg;    // __osFlashMsg
static OSMesgQueue gFlashMessageQ; // __osFlashMessageQ
static OSPiHandle gFlashHandle;  // __osFlashHandler
static OSMesg gFlashMsgBuf[1];   // __osFlashMsgBuf
static s32 gFlashBlockSize;         // __osFlashVersion

// osFlashReInit
OSPiHandle* Flash_Reinitialize(u8 latency, u8 pulse, u8 page_size, u8 rel_duration, u32 start) {
    gFlashHandle.baseAddress = PHYS_TO_K1(start);
    gFlashHandle.type++;
    gFlashHandle.latency = latency;
    gFlashHandle.pulse = pulse;
    gFlashHandle.pageSize = page_size;
    gFlashHandle.relDuration = rel_duration;
    gFlashHandle.domain = PI_DOMAIN2;

    return &gFlashHandle;
}

// osFlashInit
OSPiHandle* Flash_Initialize(void) {
    u32 flash_type;
    u32 flash_maker;

    osCreateMesgQueue(&gFlashMessageQ, gFlashMsgBuf, ARRAY_COUNT(gFlashMsgBuf));
    if (gFlashHandle.baseAddress == PHYS_TO_K1(FLASH_START_ADDR)) {
        return &gFlashHandle;
    }

    gFlashHandle.type = DEVICE_TYPE_FLASH;
    gFlashHandle.baseAddress = PHYS_TO_K1(FLASH_START_ADDR);
    gFlashHandle.latency = FLASH_LATENCY;
    gFlashHandle.pulse = FLASH_PULSE;
    gFlashHandle.pageSize = FLASH_PAGE_SIZE;
    gFlashHandle.relDuration = FLASH_REL_DURATION;
    gFlashHandle.domain = PI_DOMAIN2;
    gFlashHandle.speed = 0;

    bzero(&gFlashHandle.transferInfo, sizeof(__OSTranxInfo));

    osEPiLinkHandle(&gFlashHandle);
    Flash_ReadId(&flash_type, &flash_maker);

    if ((flash_maker == FLASH_VERSION_MX_C) || (flash_maker == FLASH_VERSION_MX_A) ||
        (flash_maker == FLASH_VERSION_MX_PROTO_A)) {
        gFlashBlockSize = 0x40;
    } else {
        gFlashBlockSize = 0x80;
    }

    return &gFlashHandle;
}

// osFlashReadStatus
void Flash_ReadStatus(u8* flash_status) {
    u32 status;

    osEPiWriteIo(&gFlashHandle, gFlashHandle.baseAddress | FLASH_CMD_REG, FLASH_CMD_STATUS);
    osEPiReadIo(&gFlashHandle, gFlashHandle.baseAddress, &status);

    osEPiWriteIo(&gFlashHandle, gFlashHandle.baseAddress | FLASH_CMD_REG, FLASH_CMD_STATUS);
    osEPiReadIo(&gFlashHandle, gFlashHandle.baseAddress, &status);

    *flash_status = status & 0xFF;
    return;
}

// osFlashReadId
void Flash_ReadId(u32* flash_type, u32* flash_maker) {
    u8 status;

    Flash_ReadStatus(&status);
    osEPiWriteIo(&gFlashHandle, gFlashHandle.baseAddress | FLASH_CMD_REG, FLASH_CMD_ID);

    gFlashMsg.hdr.pri = OS_MESG_PRI_NORMAL;
    gFlashMsg.hdr.retQueue = &gFlashMessageQ;
    gFlashMsg.dramAddr = gFlashID;
    gFlashMsg.devAddr = 0;
    gFlashMsg.size = 2 * sizeof(u32);

    osWritebackDCache(&gFlashID, sizeof(__osFlashID));
    osEPiStartDma(&gFlashHandle, &gFlashMsg, 0);
    osRecvMesg(&gFlashMessageQ, NULL, 1);

    *flash_type = gFlashID[0];
    *flash_maker = gFlashID[1];
    return;
}

// osFlashClearStatus
void Flash_ClearStatus(void) {
    osEPiWriteIo(&gFlashHandle, gFlashHandle.baseAddress | FLASH_CMD_REG, FLASH_CMD_STATUS);
    osEPiWriteIo(&gFlashHandle, gFlashHandle.baseAddress, 0);
    return;
}

// osFlashAllErase without the timer ?
s32 Flash_EraseChip(void) {
    u32 status;

    osEPiWriteIo(&gFlashHandle, gFlashHandle.baseAddress | FLASH_CMD_REG, FLASH_CMD_CHIP_ERASE);
    osEPiWriteIo(&gFlashHandle, gFlashHandle.baseAddress | FLASH_CMD_REG, FLASH_CMD_EXECUTE_ERASE);

    do {
        osEPiReadIo(&gFlashHandle, gFlashHandle.baseAddress, &status);
    } while ((status & FLASH_STATUS_ERASE_BUSY) == FLASH_STATUS_ERASE_BUSY);

    osEPiReadIo(&gFlashHandle, gFlashHandle.baseAddress, &status);
    Flash_ClearStatus();

    if (((status & 0xFF) == 8) || ((status & 0xFF) == 0x48) || ((status & 8) == 8)) {
        return FLASH_STATUS_ERASE_OK;
    } else {
        return FLASH_STATUS_ERASE_ERROR;
    }
}

// osFlashAllErase without the timer and an arg
s32 Flash_EraseSector(s32 arg0) {
    u32 status;

    osEPiWriteIo(&gFlashHandle, gFlashHandle.baseAddress | FLASH_CMD_REG, arg0 | FLASH_CMD_SECTOR_ERASE);
    osEPiWriteIo(&gFlashHandle, gFlashHandle.baseAddress | FLASH_CMD_REG, FLASH_CMD_EXECUTE_ERASE);

    do {
        osEPiReadIo(&gFlashHandle, gFlashHandle.baseAddress, &status);
    } while ((status & FLASH_STATUS_ERASE_BUSY) == FLASH_STATUS_ERASE_BUSY);

    osEPiReadIo(&gFlashHandle, gFlashHandle.baseAddress, &status);
    Flash_ClearStatus();

    if ((((status & 0xFF) == 8) || ((status & 0xFF) == 0x48)) || ((status & 8) == 8)) {
        return FLASH_STATUS_ERASE_OK;
    } else {
        return FLASH_STATUS_ERASE_ERROR;
    }
}

// osFlashWriteBuffer
s32 Flash_StartWriteBuffer(OSIoMesg* mb, s32 priority, void* dramAddr, OSMesgQueue* mq) {
    s32 sp1C;

    osEPiWriteIo(&gFlashHandle, gFlashHandle.baseAddress | FLASH_CMD_REG, FLASH_CMD_PAGE_PROGRAM);

    mb->hdr.pri = priority;
    mb->hdr.retQueue = mq;
    mb->dramAddr = dramAddr;
    mb->devAddr = 0;
    mb->size = FLASH_BLOCK_SIZE;

    sp1C = osEPiStartDma(&gFlashHandle, mb, OS_WRITE);
    return sp1C;
}

// Similar to osFlashAllErase without the timer, an arg, and different status bits
s32 Flash_ProgramPage(s32 arg0) {
    u32 status;

    osEPiWriteIo(&gFlashHandle, gFlashHandle.baseAddress | FLASH_CMD_REG, arg0 | FLASH_CMD_PROGRAM_PAGE);

    do {
        osEPiReadIo(&gFlashHandle, gFlashHandle.baseAddress, &status);
    } while ((status & 1) == 1);

    osEPiReadIo(&gFlashHandle, gFlashHandle.baseAddress, &status);
    Flash_ClearStatus();

    if (((status & 0xFF) == 4) || ((status & 0xFF) == 0x44) || ((status & 4) == 4)) {
        return FLASH_STATUS_ERASE_OK;
    } else {
        return FLASH_STATUS_ERASE_ERROR;
    }
}

// osFlashReadArray but without handling pages individually
s32 Flash_StartReadArray(OSIoMesg* mb, s32 priority, s32 devAddr, void* dramAddr, s32 n_pages, OSMesgQueue* mq) {
    s32 sp1C;
    u32 sp18;

    osEPiWriteIo(&gFlashHandle, gFlashHandle.baseAddress | FLASH_CMD_REG, FLASH_CMD_READ_ARRAY);
    osEPiReadIo(&gFlashHandle, gFlashHandle.baseAddress, &sp18);

    mb->hdr.pri = priority;
    mb->hdr.retQueue = mq;
    mb->dramAddr = dramAddr;
    mb->devAddr = devAddr * gFlashBlockSize;
    mb->size = n_pages * FLASH_BLOCK_SIZE;

    sp1C = osEPiStartDma(&gFlashHandle, mb, OS_READ);
    return sp1C;
}
