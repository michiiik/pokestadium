#ifndef _E890_H_
#define _E890_H_

#include "global.h"

typedef struct LeoDiskDescriptor {
    /* 0x00 */ s32 unk_00;
    /* 0x04 */ u16 unk_04;
    /* 0x06 */ u16 unk_06;
    /* 0x08 */ void* unk_08;
    /* 0x0C */ s32 unk_0C;
} LeoDiskDescriptor; // size = 0x10

typedef struct LeoBootState {
    /* 0x00 */ u16 unk_00;
    /* 0x02 */ u16 unk_02;
    /* 0x04 */ u16 unk_04;
    /* 0x06 */ u16 unk_06;
} LeoBootState; // size = 0x8

extern LeoDiskDescriptor gBootIDData;

s32 Leo_ValidateBootDiskId(void);
s32 Leo_AcceptPokemonDiskDescriptor(LeoDiskDescriptor* arg0);
s32 Leo_QueueDiskIdRead(void);
s32 Leo_QueueDiskReset(void);
s32 Leo_QueueBootBlockRead(LeoDiskDescriptor* arg0);
s32 Leo_QueueDiskPayloadRead(void);
s32 Leo_HandleDiskIdCompletion(void);
s32 Leo_HandleResetCompletion(void);
s32 Leo_HandleBootBlockCompletion(LeoDiskDescriptor* arg0);
s32 Leo_HandlePayloadCompletion(void);
s32 Leo_PollDiskReady(void);
void func_8000E5B4(void);
s32 Leo_RunBootStateMachine(LeoDiskDescriptor* arg0);
s32 Leo_InitializeBootState(void);
void Leo_AbortPendingBootRequest(void);
void Leo_ResetBootStateIfReady(void);

#endif // _E890_H_
