#ifndef _45720_H_
#define _45720_H_

#include "global.h"
#include "src/animation_double_buffer.h"

typedef struct unk_D_800FCF28 {
    /* 0x000 */ s16 unk_000[0x8A0];
} unk_D_800FCF28; // size = 0x1140

extern unk_D_800FCF28* D_800FCF28[2];
extern u32 D_800FCF30[2];

void AudioStream_Init(ALHeap*, u32, u32);
void AudioStream_Start(u32, u32, u32, u32);
void AudioStream_Stop(u32, u32);
s32 AudioStream_IsPlaying(u32);
void AudioStream_SetEnabled(s32);
void func_80044EA4(void);
void AudioStream_DmaReadChunk(u32 arg0, u32 arg1, u32 arg2, OSMesgQueue* arg3);
void AudioStream_OpenChannel(unk_D_800FCED8*, s32, s32);
void AudioStream_RefillAndDecode(unk_D_800FCED8*);
u32 AudioStream_ConsumeSamples(unk_D_800FCED8*, u32);
void AudioStream_MarkLoaded(unk_D_800FCED8*);
void AudioStream_Abort(unk_D_800FCED8*);

#endif // _45720_H_
