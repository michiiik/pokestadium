#ifndef _45720_H_
#define _45720_H_

#include "global.h"
#include "src/gsm_decoder.h"

typedef struct AudioBuffer {
    /* 0x000 */ s16 unk_000[0x8A0];
} AudioBuffer; // size = 0x1140

extern AudioBuffer* gAudioChannelBuffer[2];
extern u32 gAudioChannelReadPos[2];

void AudioStream_Init(ALHeap*, u32, u32);
void AudioStream_Start(u32, u32, u32, u32);
void AudioStream_Stop(u32, u32);
s32 AudioStream_IsPlaying(u32);
void AudioStream_SetEnabled(s32);
void Audio_ProcessChannels(void);
void AudioStream_DmaReadChunk(u32 arg0, u32 arg1, u32 arg2, OSMesgQueue* arg3);
void AudioStream_OpenChannel(GbAudioChannelState*, s32, s32);
void AudioStream_RefillAndDecode(GbAudioChannelState*);
u32 AudioStream_ConsumeSamples(GbAudioChannelState*, u32);
void AudioStream_MarkLoaded(GbAudioChannelState*);
void AudioStream_Abort(GbAudioChannelState*);

#endif // _45720_H_
