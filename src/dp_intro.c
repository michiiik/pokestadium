#include "global.h"
#include <PR/os_internal_reg.h>
#include "global.h"
#include "dp_intro.h"
#include "scheduler.h"
#include "gfx_rect.h"
#include "crash_screen.h"
#include "profiler.h"
#include "memory.h"

extern struct UnkStruct80083CA0_2 D_80083CA0;

extern UnkStruct80001380 D_800846C0;

extern struct UnkArray4 D_8008472C;
extern struct UnkArray4 D_8008473C;
extern struct UnkArray4 D_8008474C;

extern s32 D_80068B70;

extern u64 F3DEX2_data_bin[]; // F3DEX2_data.bin
extern u64 D_80084860[0x1];
extern u64 D_80085870[0x1];
extern u64 D_80084C68[0x1];

#define ALIGN16(val) (((val) + 0xF) & ~0xF)

extern s32 osTvType;
extern char D_80068B74[];
extern f32 D_8007AF10;

extern OSViMode D_800796E0[]; // osViModeTable

extern void DisplayWorker_ThreadMain(void*);

extern u32 D_80084680[];

extern OSMesgQueue D_8008468C;
extern OSMesgQueue D_800846A4;

extern OSMesg D_80084684;
extern OSMesg D_80084688;

extern s32 D_80084758;

// .bss?
extern u64 D_80084760[0x100 / sizeof(u64)];

// function prototypes
void Display_ClearFramebufferLine(u16 color);
void Display_SetBufferConfig(struct UnkArray4* arg0, u8 arg1, u8 arg2, u8 arg3, u8 arg4, s32 arg5, s32 arg6, s32 arg7);

void DisplayTask_Initialize(UnkStruct80001380* arg0) {
    arg0->task.t.type = M_GFXTASK;
    arg0->task.t.flags = 0;
    arg0->task.t.ucode_boot = D_80084760;
    arg0->task.t.ucode_boot_size = 0x100;
    arg0->task.t.ucode = _binary_assets_us_F3DEX2_bin_start;
    arg0->task.t.ucode_data = F3DEX2_data_bin;
    arg0->task.t.ucode_size = 0x1000;
    arg0->task.t.ucode_data_size = 0x800;
    arg0->task.t.dram_stack = (void*)ALIGN16((uintptr_t)D_80084860);
    arg0->task.t.dram_stack_size = 0x400;
    arg0->task.t.yield_data_ptr = (void*)ALIGN16((uintptr_t)D_80084C68);
    arg0->task.t.yield_data_size = 0xC00;
    arg0->task.t.output_buff = (D_80085870);
    arg0->task.t.output_buff_size = (D_80085870 + (0x20000 / sizeof(u64)));
    osCreateMesgQueue(&arg0->queue, &arg0->mesg, 1);
}

void DisplayTask_Submit(UnkStruct80001380* arg0, struct UnkArray4* arg1, s32 arg2) {
    arg0->task.t.data_ptr = (void*)(uintptr_t)arg1->displayListPtr;
    arg0->task.t.data_size = arg1->displayListSize;
    Sched_SubmitTask(arg0, arg2);
}

void Video_SetMode(s8 arg0, s8 arg1) {
    s32 var_v0;

    if (osTvType == 2) {
        var_v0 = (arg0 * 2) + arg1 + 8;
    } else {
        var_v0 = (arg0 * 2) + arg1 + 4;
    }

    osViSetMode(&D_800796E0[(s32)D_80068B74[var_v0]]);
    osViSetSpecialFeatures(0x40U);
    osViSetSpecialFeatures(2U);
    osViSetSpecialFeatures(0x10U);
    if (osTvType == 0) {
        osViSetYScale(D_8007AF10);
    }
}

void DisplayWorker_SetBufferConfig(struct UnkArray4* arg0) {
    if (arg0 == NULL) {
        D_80083CA0.displayListPtr = 0;
        D_80083CA0.displayListSize = 0;
        D_80083CA0.frameId = 0;
        D_80083CA0.videoModeA = D_80083CA0.pendingVideoModeA;
        D_80083CA0.videoModeB = D_80083CA0.pendingVideoModeB;
        D_80083CA0.bufferMode = D_80083CA0.pendingBufferMode;
    } else {
        D_80083CA0.videoModeA = arg0->videoModeA;
        D_80083CA0.videoModeB = arg0->videoModeB;
        D_80083CA0.bufferMode = arg0->bufferMode;
        D_80083CA0.displayListPtr = arg0->displayListPtr;
        D_80083CA0.displayListSize = arg0->displayListSize;
        D_80083CA0.frameId = arg0->frameId;
        D_80083CA0.bufferIndex = arg0->bufferIndex;
    }
}

void DisplayWorker_ProcessFrame(void) {
    s32 i;
    s32 sp20 = 0;

    if (D_80083CA0.taskSubmitted != 0) {
        Sched_WaitForTaskMessage(&D_800846C0);
    }

    if ((D_80083CA0.displayListPtr != 0) && (D_80083CA0.appliedFrameId != D_80083CA0.frameId) &&
        (D_80083CA0.appliedBufferIndex != D_80083CA0.bufferIndex)) {
        DisplayTask_Submit(&D_800846C0, &D_8008472C, 1);
        sp20 = 1;
    }

    for (i = 1; i < D_80083CA0.clientCount; i++) {
        Sched_WaitClientQueue(&D_80083CA0);
    }

    while (D_80083CA0.unk_1C0.validCount > 0) {
        Sched_TryReceiveClientQueue(&D_80083CA0);
    }

    if (D_80083CA0.crashScreenFramebuffer != NULL) {
        osViSwapBuffer((void*)(uintptr_t)D_80083CA0.crashScreenFramebuffer->displayListSize);
        osViRepeatLine(0);
        if ((D_80083CA0.pendingVideoModeA != D_80083CA0.appliedVideoModeA) || (D_80083CA0.pendingVideoModeB != D_80083CA0.appliedVideoModeB)) {
            Video_SetMode((s8)D_80083CA0.pendingVideoModeA, (s8)D_80083CA0.pendingVideoModeB);
        }
        if (D_80068B70 != 0) {
            osViBlack(1U);
        } else {
            osViBlack(0U);
        }
        crash_screen_set_draw_info((void*)(uintptr_t)D_80083CA0.crashScreenFramebuffer->displayListSize, *(u16*)&D_80083CA0.crashScreenFramebuffer->displayListPtr,
                                   0x10);
    } else {
        osViRepeatLine(1);
        osViSwapBuffer((void*)(uintptr_t)D_80083CA0.normalFramebuffer->displayListSize);
        if ((D_80083CA0.pendingVideoModeA != D_80083CA0.appliedVideoModeA) || (D_80083CA0.pendingVideoModeB != D_80083CA0.appliedVideoModeB)) {
            Video_SetMode((s8)D_80083CA0.pendingVideoModeA, (s8)D_80083CA0.pendingVideoModeB);
        }
    }

    if ((sp20 == 0) && (D_80083CA0.displayListPtr != 0)) {
        DisplayTask_Submit(&D_800846C0, &D_8008472C, 0);
    }

    D_8008474C = D_8008473C;
    D_8008473C = D_8008472C;
    profiler_log_thread5_time(THREAD5_END);
    Sched_WaitClientQueue(&D_80083CA0);
}

void DisplayWorker_DrainEvents(void) {
    Sched_WaitClientQueue(&D_80083CA0);
    while (D_80083CA0.unk_1C0.validCount > 0) {
        Sched_TryReceiveClientQueue(&D_80083CA0);
    }
    Sched_WaitClientQueue(&D_80083CA0);
}

void DisplayWorker_ThreadMain(UNUSED void* arg) {
    __osSetFpcCsr(0x01000C01);
    Display_SetBufferConfig(&D_8008474C, 0, 1, 2, 0xFF, 0, 0, 0);
    Display_SetBufferConfig(&D_8008473C, 0, 1, 2, 0xFF, 0, 0, 0);
    Sched_InitClientQueue(&D_80083CA0.thread, 0, 4);
    Sched_AddClient(&D_80083CA0);
    DisplayTask_Initialize(&D_800846C0);
    while (1) {
        void* sp4C;
        if (D_800A62E0.shutdownCounter > 0) {
            DisplayWorker_DrainEvents();
            continue;
        }
        profiler_log_thread5_time(THREAD5_START);
        D_80083CA0.workerBusy = 1;
        osRecvMesg(&D_8008468C, &sp4C, 1);
        D_80083CA0.workerBusy = 0;
        profiler_log_thread5_time(UNK_EVENT_1);
        profiler_log_thread5_time(UNK_EVENT_2);
        DisplayWorker_SetBufferConfig(sp4C);
        DisplayWorker_ProcessFrame();
        osSendMesg(&D_800846A4, (void*)'DONE', 0);
    }
}

void DisplayWorker_Initialize(void) {
    switch (osTvType) {
        case 1:
        case 2:
            break;

        default:
            osViBlack(1U);
            while (1) {
                ;
            }
            break;
    }

    osCreateThread(&D_80083CA0.thread, 5, DisplayWorker_ThreadMain, NULL, &D_80084680, 0x28);
    osStartThread(&D_80083CA0.thread);
    osCreateMesgQueue(&D_8008468C, &D_80084684, 1);
    osCreateMesgQueue(&D_800846A4, &D_80084688, 1);
    D_80083CA0.unk_A88 = 0;
    D_80083CA0.workerBusy = 0;
    D_80084680[0] = GfxImage_Allocate(0, IMAGE_SIZE_BITS_16b, 0x280, 1, MEMORY_POOL_RIGHT);
    Display_ClearFramebufferLine(1);
    Dma_WriteChunks(&D_80084760, 0xB0000B70, 0xB0000C70, 0);
}

/**
 * Write a specific color to a line on the framebuffer for a 640x480 resolution.
 */
void Display_ClearFramebufferLine(u16 color) {
    s32 width = 640;
    u16* buf = (void*)(uintptr_t)D_80083CA0.normalFramebuffer->displayListSize;

    while (width-- > 0) {
        *(buf)++ = color;
    }

    osWritebackDCache((void*)(uintptr_t)D_80083CA0.normalFramebuffer->displayListSize, 0x500);
}

u16 Display_GetFramebufferClearColor(void) {
    // YIKES. What is this typing?!?
    u16* ptr = (u16*)(uintptr_t)((u32*)(uintptr_t)D_80084680[0])[2];
    return *ptr;
}

s32 Display_IsCurrentFramebuffer(void) {
    s32 result = 0;

    if (osViGetCurrentFramebuffer() == ((void**)(uintptr_t)D_80084680[0])[2]) {
        result = 1;
    }

    return result;
}

void Display_WaitForCompletion(void) {
    osRecvMesg(&D_800846A4, NULL, 1);
}

void Display_QueueFramebufferRequest(void* arg0) {
    osSendMesg(&D_8008468C, arg0, 0);
}

void Display_WaitForFrames(s32 arg0) {
    while (arg0-- > 0) {
        Display_QueueFramebufferRequest(0);
        Display_WaitForCompletion();
    }
}

void Display_SetBufferConfig(struct UnkArray4* arg0, u8 arg1, u8 arg2, u8 arg3, u8 arg4, s32 arg5, s32 arg6, s32 arg7) {
    arg0->videoModeA = arg1;
    arg0->videoModeB = arg2;
    arg0->bufferMode = arg3;
    arg0->bufferIndex = arg4;
    arg0->displayListPtr = arg5;
    arg0->displayListSize = arg6;
    arg0->frameId = arg7;
}

s32 Display_GetWorkerStatus(void) {
    return D_80084758;
}

void Display_ApplyPendingVideoMode(void) {
    Video_SetMode((s8)D_80083CA0.appliedVideoModeA, (s8)D_80083CA0.appliedVideoModeB);
}

s32 Display_IsFrameReady(void) {
    s32 result = 1;

    if (D_80083CA0.taskSubmitted != 0) {
        result = D_80083CA0.readyFrameCount > 0;
    }
    return result;
}

void Display_EnableBlackout(void) {
    D_80068B70 = 1;
}

void Display_DisableBlackout(void) {
    D_80068B70 = 0;
}
