#ifndef _SRC_DP_INTRO_H_
#define _SRC_DP_INTRO_H_

#include "global.h"
#include "jpeg_stream.h"

void DisplayTask_Initialize(UnkStruct80001380* arg0);
void DisplayTask_Submit(UnkStruct80001380* arg0, struct UnkArray4* arg1, s32 arg2);
void Video_SetMode(s8 arg0, s8 arg1);
void DisplayWorker_SetBufferConfig(struct UnkArray4* arg0);
void DisplayWorker_ProcessFrame(void);
void DisplayWorker_DrainEvents(void);
void DisplayWorker_ThreadMain(UNUSED void* arg);
void DisplayWorker_Initialize(void);
void Display_ClearFramebufferLine(u16 color);
u16 Display_GetFramebufferClearColor(void);
s32 Display_IsCurrentFramebuffer(void);
void Display_WaitForCompletion(void);
void Display_QueueFramebufferRequest(void* arg0);
void Display_WaitForFrames(s32 arg0);
void Display_SetBufferConfig(struct UnkArray4* arg0, u8 arg1, u8 arg2, u8 arg3, u8 arg4, s32 arg5, s32 arg6, s32 arg7);
s32 Display_GetWorkerStatus(void);
void Display_ApplyPendingVideoMode(void);
s32 Display_IsFrameReady(void);
void Display_EnableBlackout(void);
void Display_DisableBlackout(void);

#endif /* _SRC_DP_INTRO_H_ */
