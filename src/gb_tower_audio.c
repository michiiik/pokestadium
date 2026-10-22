#include "gb_tower_audio.h"
#include <PR/os_internal_reg.h>
#include "dp_intro.h"
#include "sections.h"
#include "memory.h"
#include "dp_intro.h"
#include "jpeg_stream.h"
#include "scheduler.h"
#include "util.h"
#include "src/fragments/gb_tower_emulator/gb_tower_emulator.h"

GbEmuState* gGbEmuState;
GbEmuThreadInfo* gGbEmuThreadInfo;
static char gGbEmuThreadId;

extern u8 gGbEmuRomBank[];

void GbTower_AudioThread(UNUSED void* arg0) {
    void (*func)(void*) = Util_ConvertAddrToVirtAddr(&GbEmu_AudioVideoTick);

    __osSetFpcCsr(0x01000C01);
    Sched_InitClientQueue(gGbEmuThreadInfo, 0, 1);
    Sched_AddClient(gGbEmuThreadInfo);

    while (1) {
        Sched_WaitClientQueue(gGbEmuThreadInfo);
        if (gRspTaskManager.unk_A38 >= 0x15) {
            continue;
        }
        func(gGbEmuThreadInfo);
    }
}

void GbTower_EmuThread(UNUSED void* arg0) {
    void (*func1)(void* func) = Util_ConvertAddrToVirtAddr(&GbEmu_Init);
    void (*func2)(void* func) = Util_ConvertAddrToVirtAddr(&GbEmu_RunFrame);

    __osSetFpcCsr(0x01000C01);
    Sched_InitClientQueue(gGbEmuState, 0, 1);
    Sched_AddClient(gGbEmuState);
    func1(gGbEmuState);
    osStartThread(&gGbEmuThreadInfo->thread);

    while (1) {
        Sched_WaitClientQueue(gGbEmuState);
        if (gRspTaskManager.unk_A38 >= 0x15) {
            continue;
        }
        func2(gGbEmuState);
    }
}

void GbTower_Start(UnkInputStruct8000D738* arg0) {
    s32 temp_v0;

    main_pool_push_state('GBEM');
    gGbEmuState = main_pool_alloc(0x2210, 0);
    gGbEmuThreadInfo = main_pool_alloc(0x21E0, 0);

    FRAGMENT_LOAD(FRAGMENT_GB_TOWER_EMULATOR);
    temp_v0 = ASSET_LOAD2(fonts, 1, 1);

    gGbEmuState->font1 = BinArchive_GetFile(temp_v0, 0);
    gGbEmuState->font2 = BinArchive_GetFile(temp_v0, 1);
    gGbEmuState->unk_2204 = *arg0;
    osCreateMesgQueue(&gGbEmuState->queue2, &gGbEmuState->mesg, 1);
    osCreateThread(&gGbEmuThreadInfo->thread, 10, GbTower_AudioThread, NULL, (u32)gGbEmuThreadInfo + 0x21E0, 0x11);
    osCreateThread(&gGbEmuState->thread, 8, GbTower_EmuThread, NULL, (u32)gGbEmuState + 0x21E0, 0xF);
    gGbEmuThreadId = GbTower_SetPollMask(0);
    osStartThread(&gGbEmuState->thread);
}

OSMesg* GbTower_RecvExitMesg(void) {
    OSMesg mesg;

    osRecvMesg(&gGbEmuState->queue2, &mesg, OS_MESG_BLOCK);
    return mesg;
}

void GbTower_Stop(UnkInputStruct8000D738* arg0) {
    *arg0 = gGbEmuState->unk_2204;
    GbTower_SetPollMask(gGbEmuThreadId);
    osViBlack(1U);
    Display_ApplyPendingVideoMode();
    Sched_RemoveClient(gGbEmuState);
    Sched_RemoveClient(gGbEmuThreadInfo);
    osDestroyThread(&gGbEmuState->thread);
    osDestroyThread(&gGbEmuThreadInfo->thread);
    main_pool_pop_state('GBEM');
}
