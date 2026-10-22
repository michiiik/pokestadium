#include "global.h"
#include <PR/os_internal_reg.h>
#include "dp_intro.h"
#include "scheduler.h"
#include "gfx_rect.h"
#include "crash_screen.h"
#include "profiler.h"
#include "memory.h"

extern u64 F3DEX2_data_bin[]; // F3DEX2_data.bin

// .data
s32 gForceBlackScreen = 0;
char gViModeIndexTable[] = {
    0x0E, 0x10, 0x17, 0x19, 0x00, 0x02, 0x09, 0x0B,
    0x1C, 0x1E, 0x25, 0x27, 0x2A, 0x2C, 0x33, 0x35,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
};

// .rodata
const f32 gViYScaleTable[4] = {
    0.8333333135f,
    0.0f,
    0.0f,
    0.0f,
};

// .bss
UnkStruct80083CA0_2 gDisplayWorkerState;
u64 gRdpBootUcode[0x100 / sizeof(u64)];
u64 gRdpDramStack[0x408 / sizeof(u64)];
u64 gRdpYieldData[0xC08 / sizeof(u64)];
u64 gRdpOutputBuffer[0x20000 / sizeof(u64)];

// function prototypes
void Display_ClearFramebufferLine(u16 color);
void Display_SetBufferConfig(struct UnkArray4* arg0, u8 arg1, u8 arg2, u8 arg3, u8 arg4, s32 arg5, s32 arg6, s32 arg7);

void DisplayTask_Initialize(UnkStruct80001380* arg0) {
    arg0->task.t.type = M_GFXTASK;
    arg0->task.t.flags = 0;
    arg0->task.t.ucode_boot = gRdpBootUcode;
    arg0->task.t.ucode_boot_size = 0x100;
    arg0->task.t.ucode = _binary_assets_us_F3DEX2_bin_start;
    arg0->task.t.ucode_data = F3DEX2_data_bin;
    arg0->task.t.ucode_size = 0x1000;
    arg0->task.t.ucode_data_size = 0x800;
    arg0->task.t.dram_stack = (void*)ALIGN16((uintptr_t)gRdpDramStack);
    arg0->task.t.dram_stack_size = 0x400;
    arg0->task.t.yield_data_ptr = (void*)ALIGN16((uintptr_t)gRdpYieldData);
    arg0->task.t.yield_data_size = 0xC00;
    arg0->task.t.output_buff = (gRdpOutputBuffer);
    arg0->task.t.output_buff_size = (gRdpOutputBuffer + (0x20000 / sizeof(u64)));
    osCreateMesgQueue(&arg0->queue, &arg0->mesg, 1);
}

void DisplayTask_Submit(UnkStruct80001380* arg0, struct UnkArray4* arg1, s32 arg2) {
    arg0->task.t.data_ptr = (void*)(uintptr_t)arg1->unk_04;
    arg0->task.t.data_size = arg1->unk_08;
    Sched_SubmitTask(arg0, arg2);
}

void Video_SetMode(s8 arg0, s8 arg1) {
    s32 var_v0;

    if (osTvType == 2) {
        var_v0 = (arg0 * 2) + arg1 + 8;
    } else {
        var_v0 = (arg0 * 2) + arg1 + 4;
    }

    osViSetMode(&osViModeTable[(s32)gViModeIndexTable[var_v0]]);
    osViSetSpecialFeatures(0x40U);
    osViSetSpecialFeatures(2U);
    osViSetSpecialFeatures(0x10U);
    if (osTvType == 0) {
        osViSetYScale(gViYScaleTable[0]);
    }
}

void DisplayWorker_SetBufferConfig(struct UnkArray4* arg0) {
    if (arg0 == NULL) {
        gDisplayWorkerState.unk_A8C.unk_04 = 0;
        gDisplayWorkerState.unk_A8C.unk_08 = 0;
        gDisplayWorkerState.unk_A8C.unk_0C = NULL;
        gDisplayWorkerState.unk_A8C.unk_01 = gDisplayWorkerState.unk_A9C.unk_01;
        gDisplayWorkerState.unk_A8C.unk_02 = gDisplayWorkerState.unk_A9C.unk_02;
        gDisplayWorkerState.unk_A8C.unk_00 = gDisplayWorkerState.unk_A9C.unk_00;
    } else {
        gDisplayWorkerState.unk_A8C.unk_01 = arg0->unk_01;
        gDisplayWorkerState.unk_A8C.unk_02 = arg0->unk_02;
        gDisplayWorkerState.unk_A8C.unk_00 = arg0->unk_00;
        gDisplayWorkerState.unk_A8C.unk_04 = arg0->unk_04;
        gDisplayWorkerState.unk_A8C.unk_08 = arg0->unk_08;
        gDisplayWorkerState.unk_A8C.unk_0C = arg0->unk_0C;
        gDisplayWorkerState.unk_A8C.unk_03 = arg0->unk_03;
    }
}

void DisplayWorker_ProcessFrame(void) {
    s32 i;
    s32 sp20 = 0;

    if (gDisplayWorkerState.unk_A9C.unk_04 != 0) {
        Sched_WaitForTaskMessage(&gDisplayWorkerState.unk_A20);
    }

    if ((gDisplayWorkerState.unk_A8C.unk_04 != 0) && (gDisplayWorkerState.unk_AAC.unk_0C != gDisplayWorkerState.unk_A8C.unk_0C) &&
        (gDisplayWorkerState.unk_AAC.unk_03 != gDisplayWorkerState.unk_A8C.unk_03)) {
        DisplayTask_Submit(&gDisplayWorkerState.unk_A20, &gDisplayWorkerState.unk_A8C, 1);
        sp20 = 1;
    }

    for (i = 1; i < gDisplayWorkerState.unk_AAC.unk_00; i++) {
        Sched_WaitClientQueue(&gDisplayWorkerState);
    }

    while (gDisplayWorkerState.unk_1C0.validCount > 0) {
        Sched_TryReceiveClientQueue(&gDisplayWorkerState);
    }

    if (gDisplayWorkerState.unk_A9C.unk_0C != NULL) {
        osViSwapBuffer(gDisplayWorkerState.unk_A9C.unk_0C->img_p);
        osViRepeatLine(0);
        if ((gDisplayWorkerState.unk_A9C.unk_01 != gDisplayWorkerState.unk_AAC.unk_01) ||
            (gDisplayWorkerState.unk_A9C.unk_02 != gDisplayWorkerState.unk_AAC.unk_02)) {
            Video_SetMode((s8)gDisplayWorkerState.unk_A9C.unk_01, (s8)gDisplayWorkerState.unk_A9C.unk_02);
        }
        if (gForceBlackScreen != 0) {
            osViBlack(1U);
        } else {
            osViBlack(0U);
        }
        crash_screen_set_draw_info((u16*)gDisplayWorkerState.unk_A9C.unk_0C->img_p, gDisplayWorkerState.unk_A9C.unk_0C->width, 0x10);
    } else {
        osViRepeatLine(1);
        osViSwapBuffer(gDisplayWorkerState.unk_9E0->img_p);
        if ((gDisplayWorkerState.unk_A9C.unk_01 != gDisplayWorkerState.unk_AAC.unk_01) ||
            (gDisplayWorkerState.unk_A9C.unk_02 != gDisplayWorkerState.unk_AAC.unk_02)) {
            Video_SetMode((s8)gDisplayWorkerState.unk_A9C.unk_01, (s8)gDisplayWorkerState.unk_A9C.unk_02);
        }
    }

    if ((sp20 == 0) && (gDisplayWorkerState.unk_A8C.unk_04 != 0)) {
        DisplayTask_Submit(&gDisplayWorkerState.unk_A20, &gDisplayWorkerState.unk_A8C, 0);
    }

    gDisplayWorkerState.unk_AAC = gDisplayWorkerState.unk_A9C;
    gDisplayWorkerState.unk_A9C = gDisplayWorkerState.unk_A8C;
    profiler_log_thread5_time(THREAD5_END);
    Sched_WaitClientQueue(&gDisplayWorkerState);
}

void DisplayWorker_DrainEvents(void) {
    Sched_WaitClientQueue(&gDisplayWorkerState);
    while (gDisplayWorkerState.unk_1C0.validCount > 0) {
        Sched_TryReceiveClientQueue(&gDisplayWorkerState);
    }
    Sched_WaitClientQueue(&gDisplayWorkerState);
}

void DisplayWorker_ThreadMain(UNUSED void* arg) {
    __osSetFpcCsr(0x01000C01);
    Display_SetBufferConfig(&gDisplayWorkerState.unk_AAC, 0, 1, 2, 0xFF, 0, 0, 0);
    Display_SetBufferConfig(&gDisplayWorkerState.unk_A9C, 0, 1, 2, 0xFF, 0, 0, 0);
    Sched_InitClientQueue(&gDisplayWorkerState.thread, 0, 4);
    Sched_AddClient(&gDisplayWorkerState);
    DisplayTask_Initialize(&gDisplayWorkerState.unk_A20);
    while (1) {
        void* sp4C;
        if (gRspTaskManager.unk_A38 > 0) {
            DisplayWorker_DrainEvents();
            continue;
        }
        profiler_log_thread5_time(THREAD5_START);
        gDisplayWorkerState.unk_A8A = 1;
        osRecvMesg(&gDisplayWorkerState.unk_9EC, &sp4C, 1);
        gDisplayWorkerState.unk_A8A = 0;
        profiler_log_thread5_time(UNK_EVENT_1);
        profiler_log_thread5_time(UNK_EVENT_2);
        DisplayWorker_SetBufferConfig(sp4C);
        DisplayWorker_ProcessFrame();
        osSendMesg(&gDisplayWorkerState.unk_A04, (void*)'DONE', 0);
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

    osCreateThread(&gDisplayWorkerState.thread, 5, DisplayWorker_ThreadMain, NULL, &gDisplayWorkerState.unk_9E0, 0x28);
    osStartThread(&gDisplayWorkerState.thread);
    osCreateMesgQueue(&gDisplayWorkerState.unk_9EC, &gDisplayWorkerState.unk_9E4, 1);
    osCreateMesgQueue(&gDisplayWorkerState.unk_A04, &gDisplayWorkerState.unk_9E8, 1);
    gDisplayWorkerState.unk_A88 = 0;
    gDisplayWorkerState.unk_A8A = 0;
    gDisplayWorkerState.unk_9E0 = GfxImage_Allocate(0, IMAGE_SIZE_BITS_16b, 0x280, 1, MEMORY_POOL_RIGHT);
    Display_ClearFramebufferLine(1);
    Dma_WriteChunks(&gRdpBootUcode, 0xB0000B70, 0xB0000C70, 0);
}

/**
 * Write a specific color to a line on the framebuffer for a 640x480 resolution.
 */
void Display_ClearFramebufferLine(u16 color) {
    s32 width = 640;
    u16* buf = (u16*)gDisplayWorkerState.unk_9E0->img_p;

    while (width-- > 0) {
        *(buf)++ = color;
    }

    osWritebackDCache(gDisplayWorkerState.unk_9E0->img_p, 0x500);
}

u16 Display_GetFramebufferClearColor(void) {
    u16* ptr = (u16*)gDisplayWorkerState.unk_9E0->img_p;
    return *ptr;
}

s32 Display_IsCurrentFramebuffer(void) {
    s32 result = 0;

    if (osViGetCurrentFramebuffer() == gDisplayWorkerState.unk_9E0->img_p) {
        result = 1;
    }

    return result;
}

void Display_WaitForCompletion(void) {
    osRecvMesg(&gDisplayWorkerState.unk_A04, NULL, 1);
}

void Display_QueueFramebufferRequest(void* arg0) {
    osSendMesg(&gDisplayWorkerState.unk_9EC, arg0, 0);
}

void Display_WaitForFrames(s32 arg0) {
    while (arg0-- > 0) {
        Display_QueueFramebufferRequest(0);
        Display_WaitForCompletion();
    }
}

void Display_SetBufferConfig(struct UnkArray4* arg0, u8 arg1, u8 arg2, u8 arg3, u8 arg4, s32 arg5, s32 arg6, s32 arg7) {
    arg0->unk_01 = arg1;
    arg0->unk_02 = arg2;
    arg0->unk_00 = arg3;
    arg0->unk_03 = arg4;
    arg0->unk_04 = arg5;
    arg0->unk_08 = arg6;
    arg0->unk_0C = (struct FramebufferImage*)(uintptr_t)arg7;
}

s32 Display_GetWorkerStatus(void) {
    return (s32)gDisplayWorkerState.unk_AAC.unk_0C;
}

void Display_ApplyPendingVideoMode(void) {
    Video_SetMode((s8)gDisplayWorkerState.unk_AAC.unk_01, (s8)gDisplayWorkerState.unk_AAC.unk_02);
}

s32 Display_IsFrameReady(void) {
    s32 result = 1;

    if (gDisplayWorkerState.unk_A9C.unk_04 != 0) {
        result = gDisplayWorkerState.unk_A20.queue.validCount > 0;
    }
    return result;
}

void Display_EnableBlackout(void) {
    gForceBlackScreen = 1;
}

void Display_DisableBlackout(void) {
    gForceBlackScreen = 0;
}
