#ifndef _18140_H_
#define _18140_H_

#include "global.h"

typedef struct unk_D_800ABD00 {
    /* 0x00 */ u8 unk_00;
    /* 0x02 */ s16 unk_02;
    /* 0x04 */ unk_D_86002F58_004_000_054_004* unk_04;
    /* 0x08 */ void* unk_08;
    /* 0x0C */ void* unk_0C;
} unk_D_800ABD00; // size = 0x10

typedef struct unk_func_80017540_arg1 {
    /* 0x00 */ u16 unk_00;
    /* 0x02 */ u16 unk_02;
} unk_func_80017540_arg1; // size = 0x4

s32 ModelAnim_ResolveEventIndex(s32 arg0, unk_func_80017540_arg1* arg1, s32 arg2);
s16 ModelAnim_AdvanceEventFrame(unk_D_86002F58_004_000_054* arg0, u16 arg1);
void ModelAnim_ResetEventContext(void);
void ModelAnim_BeginEventContext(unk_D_86002F58_004_000_054* arg0, u16 arg1, s32 arg2);
void ModelAnim_EndEventContext(void);
void ModelAnim_GetEventAtFrame(unk_D_86002F34_alt11_018** arg0, unk_D_86002F34_alt11_018* arg1, s32 arg2);
void ModelAnim_ClearEventTrack(unk_D_86002F58_004_000* arg0);
s16 ModelAnim_BindEventTrack(unk_D_86002F58_004_000* arg0, s16 arg1, s32 arg2);
void ModelAnim_SetEventFrame(unk_D_86002F58_004_000* arg0, s16 arg1);
s32 ModelAnim_IsEventFrame(unk_D_86002F58_004_000* arg0, s16 arg1);
s32 ModelAnim_IsEventTrackNearEnd(unk_D_86002F58_004_000* arg0);
s32 ModelAnim_IsEventTrackDone(unk_D_86002F58_004_000* arg0);

#endif // _18140_H_
