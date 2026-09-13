#ifndef _5580_H_
#define _5580_H_

#include "global.h"

typedef struct UnkStruct80001380 {
    /* 0x00 */ OSMesg mesg;
    /* 0x04 */ OSMesgQueue queue;
    /* 0x1C */ u16 state; // Sched_StartTask/RequestTaskYield/HandleRspCompletion's dispatch (0=idle,1=running,2=yielding,3=done)
    /* 0x1E */ u16 notifyFlag; // Sched_StartTask sets 1 (gfx) or 2 (type4); gates the 'DONE' message on completion
    /* 0x20 */ OSTask task;
    /* 0x60 */ char unk60[0x8];
} UnkStruct80001380; // size = 0x68

typedef struct unk_D_800A62E0 {
    /* 0x000 */ OSThread thread;
    /* 0x1B0 */ char unk1B0[0x800];
    /* 0x9B0 */ u8 stack[64];
    /* 0x9F0 */ OSMesgQueue queue;
    /* 0xA08 */ u64 time;
    /* 0xA10 */ unk_D_800AA660* clientList; // Sched_AddClient/RemoveClient/NotifyClients linked list head
    /* 0xA14 */ UnkStruct80001380* activeTask; // currently loaded/running RSP task
    /* 0xA18 */ UnkStruct80001380* audioTaskSlot; // Sched_HandleRetrace/RspEvent: M_AUDTASK-related pending slot
    /* 0xA1C */ UnkStruct80001380* rspTaskSlot; // gfx/type-4 pending slot
    /* 0xA20 */ UnkStruct80001380* nextAudioTaskSlot; // promoted into audioTaskSlot by Sched_PromotePendingTasks
    /* 0xA24 */ UnkStruct80001380* nextRspTaskSlot; // promoted into rspTaskSlot by Sched_PromotePendingTasks
    /* 0xA28 */ s32 audioDoneFlag; // cleared when audioTaskSlot's M_AUDTASK completes
    /* 0xA2C */ s32 gfxDoneFlag; // cleared when rspTaskSlot's M_GFXTASK completes (and its notifyFlag==2)
    /* 0xA30 */ s32 type4DoneFlag; // cleared when rspTaskSlot's type-4 task completes
    /* 0xA34 */ s32 frameCounter; // incremented every Sched_HandleRetrace
    /* 0xA38 */ s16 shutdownCounter; // 0=normal; Sched_HandlePreNMI sets 1, counts up to 0x1E, 0x1A triggers the SP halt spin
    /* 0xA3A */ char unkA3A[0x6];
} unk_D_800A62E0; // size = 0xA40

typedef struct UnkArray4 {
    /* 0x00 */ u8 bufferMode; // Display_SetBufferConfig's arg3
    /* 0x01 */ u8 videoModeA; // arg1; feeds Video_SetMode's arg0 once committed
    /* 0x02 */ u8 videoModeB; // arg2; feeds Video_SetMode's arg1 once committed
    /* 0x03 */ u8 bufferIndex; // arg4; compared against the committed index to detect a new frame
    /* 0x04 */ u32 displayListPtr; // arg5; DisplayTask_Submit's task data_ptr
    /* 0x08 */ u32 displayListSize; // arg6; DisplayTask_Submit's task data_size
    /* 0x0C */ u32 frameId; // arg7; compared against the committed frameId to detect a new frame
} UnkArray4; // size = 0x10

// there's some wonkyness going on like the compiler trying to 8-align filler arrays? I dont understand.
typedef struct UnkStruct80083CA0_2 {
    /* 0x000 */ OSThread thread;
    /* 0x1B0 */ OSMesg unk_1B0;
    /* 0x1B4 */ char unk1B4[0xC];
    /* 0x1C0 */ OSMesgQueue unk_1C0;
    /* 0x1D8 */ s32 nextClient; // same layout as unk_D_800AA660's nextClient
    /* 0x1DC */ s32 eventFilter; // Sched_InitClientQueue's arg1
    /* 0x1E0 */ u8 unk1E0[0x800];
    /* 0x9E0 */ UnkArray4* normalFramebuffer; // DisplayWorker_ProcessFrame's default osViSwapBuffer source
    /* 0x9E4 */ char unk9E4[0x48];
    /* 0xA2C */ s32 readyFrameCount; // Display_IsFrameReady: >0 gates readiness when taskSubmitted
    /* 0xA30 */ char unkA30[0x58];
    /* 0xA88 */ u16 unk_A88;
    /* 0xA8A */ u8 workerBusy; // set 1 before waiting on the frame-request mesg, 0 after receiving it
    /* 0xA8B */ u8 unk_A8B;
    /* 0xA8C */ u8 bufferMode; // committed copy of UnkArray4.bufferMode
    /* 0xA8D */ u8 videoModeA; // committed copy of UnkArray4.videoModeA
    /* 0xA8E */ u8 videoModeB; // committed copy of UnkArray4.videoModeB
    /* 0xA8F */ u8 bufferIndex; // committed copy of UnkArray4.bufferIndex
    /* 0xA90 */ s32 displayListPtr; // committed copy of UnkArray4.displayListPtr; !=0 gates gfx task submission
    /* 0xA94 */ s32 displayListSize;
    /* 0xA98 */ s32 frameId;
    /* 0xA9C */ u8 pendingBufferMode; // restored into bufferMode/videoModeA/B when DisplayWorker_SetBufferConfig's arg is NULL
    /* 0xA9D */ u8 pendingVideoModeA; // Video_SetMode's arg0 once applied
    /* 0xA9E */ u8 pendingVideoModeB; // Video_SetMode's arg1 once applied
    /* 0xA9F */ u8 unk_A9F;
    /* 0xAA0 */ s32 taskSubmitted; // gates Sched_WaitForTaskMessage/Display_IsFrameReady
    /* 0xAA4 */ char unkAA4[0x4];
    /* 0xAA8 */ UnkArray4* crashScreenFramebuffer; // overrides normalFramebuffer for osViSwapBuffer/crash_screen_set_draw_info when non-NULL
    /* 0xAAC */ u8 clientCount; // DisplayWorker_ProcessFrame's Sched_WaitClientQueue loop bound
    /* 0xAAD */ u8 appliedVideoModeA; // compared against pendingVideoModeA/B to detect a video-mode change
    /* 0xAAE */ u8 appliedVideoModeB;
    /* 0xAAF */ u8 appliedBufferIndex; // compared against bufferIndex to detect a new frame
    /* 0xAB0 */ char unkAB0[0x8];
    /* 0xAB8 */ s32 appliedFrameId; // compared against frameId to detect a new frame
} UnkStruct80083CA0_2; // size >= 0xABC

extern unk_D_800A62E0 D_800A62E0;

void Sched_InitClientQueue(UnkStruct80083CA0_2*, s32, s32);
s32 Sched_WaitClientQueue(UnkStruct80083CA0_2*);
s32 Sched_TryReceiveClientQueue(UnkStruct80083CA0_2*);
void Sched_Init(void);
void Sched_AddClient(unk_D_800AA660*);
void Sched_ResetTaskState(UnkStruct80001380* arg0);
void Sched_WaitForTaskMessage(UnkStruct80001380* arg0);
void Sched_RemoveClient(unk_D_800AA660* arg0);
void Sched_SubmitTask(UnkStruct80001380* arg0, s32 arg1);


#endif // _5580_H_
