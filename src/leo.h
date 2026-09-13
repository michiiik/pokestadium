#ifndef _E890_H_
#define _E890_H_

#include "global.h"

typedef struct unk_D_800AA680 {
    /* 0x00 */ s32 magic; // Leo_AcceptPokemonDiskDescriptor: must equal 'POKE'
    /* 0x04 */ u16 payloadSize; // Storage_QueueDiskTransfer's arg2 (Leo_QueueDiskPayloadRead)
    /* 0x06 */ u16 payloadTransferCount; // Storage_QueueDiskTransfer's arg3
    /* 0x08 */ void* payloadAddr; // Storage_QueueDiskTransfer's arg1
    /* 0x0C */ s32 unk_0C;
} unk_D_800AA680; // size = 0x10

typedef struct unk_D_800AA690 {
    /* 0x00 */ u16 state; // Leo_RunBootStateMachine's dispatch
    /* 0x02 */ u16 errorCode;
    /* 0x04 */ u16 lastResult; // most recent osRecvMesg completion code
    /* 0x06 */ u16 retryCount; // reset-completion retry counter, capped at 0x1F
} unk_D_800AA690; // size = 0x8

extern unk_D_800AA680 D_800AA680;

s32 Leo_ValidateBootDiskId(void);
s32 Leo_AcceptPokemonDiskDescriptor(unk_D_800AA680* arg0);
s32 Leo_QueueDiskIdRead(void);
s32 Leo_QueueDiskReset(void);
s32 Leo_QueueBootBlockRead(unk_D_800AA680* arg0);
s32 Leo_QueueDiskPayloadRead(void);
s32 Leo_HandleDiskIdCompletion(void);
s32 Leo_HandleResetCompletion(void);
s32 Leo_HandleBootBlockCompletion(unk_D_800AA680* arg0);
s32 Leo_HandlePayloadCompletion(void);
s32 Leo_PollDiskReady(void);
void func_8000E5B4(void);
s32 Leo_RunBootStateMachine(unk_D_800AA680* arg0);
s32 Leo_InitializeBootState(void);
void Leo_AbortPendingBootRequest(void);
void Leo_ResetBootStateIfReady(void);

#endif // _E890_H_
