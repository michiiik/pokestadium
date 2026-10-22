#include "rom_device.h"

static OSMesgQueue gRomDmaQueue;
static OSPiHandle* gRomPiHandle;
static OSMesg gRomDmaMsg;

void Rom_InitDevice(s32 arg0) {
    switch (arg0) {
        case 1:
            gRomPiHandle = osDriveRomInit();
            break;

        case 2:
        default:
            gRomPiHandle = osCartRomInit();
            break;
    }

    osCreateMesgQueue(&gRomDmaQueue, &gRomDmaMsg, 1);
}

void Rom_DmaRead(u32 dev_addr, u32 dram_addr, u32 size) {
    OSIoMesg msg;
    u32 dev = dev_addr;
    u32 dram = dram_addr;
    s32 var_s0;

    while (size != 0) {
        if (size > 0x4000) {
            var_s0 = 0x4000;
        } else {
            var_s0 = size;
        }

        msg.hdr.pri = 0;
        msg.hdr.retQueue = &gRomDmaQueue;
        msg.dramAddr = dram;
        msg.devAddr = dev;
        msg.size = var_s0;

        osInvalDCache(msg.dramAddr, var_s0);
        osEPiStartDma(gRomPiHandle, &msg, 0);
        osRecvMesg(&gRomDmaQueue, NULL, 1);

        dev += var_s0;
        dram += var_s0;
        size -= var_s0;
    }
}
