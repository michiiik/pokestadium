#ifndef _46680_H_
#define _46680_H_

#include "global.h"

typedef struct unk_D_800FCED8_0000 {
    /* 0x000 */ char unk000[0x166];
    /* 0x166 */ s16 unk_166;
    /* 0x168 */ char unk168[0x22];
    /* 0x18A */ s16 unk_18A;
    /* 0x18C */ s16 unk_18C;
    /* 0x18E */ s16 unk18E;
} unk_D_800FCED8_0000; // size = 0x190

typedef struct unk_D_800FCED8_0198 {
    /* 0x000 */ char unk_0000[0x1400];
} unk_D_800FCED8_0198; // size = 0x1400

typedef struct unk_D_800FCED8_01598 {
    /* 0x00 */ char unk00[0x4];
    /* 0x04 */ u32 unk_04;
} unk_D_800FCED8_01598; // size = 0x8

typedef void (*unk_D_800FCED8_190)(s32, unk_D_800FCED8_01598*, s32, OSMesgQueue*);

typedef struct unk_D_800FCED8 {
    /* 0x0000 */ unk_D_800FCED8_0000 unk_0000; // AudioStream_OpenChannel: bzero'd each open, unk_166 set to 0x28
    /* 0x0190 */ unk_D_800FCED8_190 dmaReadFunc; // AudioStream_OpenChannel's arg2 (e.g. AudioStream_DmaReadChunk)
    /* 0x0194 */ s32 dmaBaseAddr; // AudioStream_OpenChannel's arg1; base ROM offset for DMA reads
    /* 0x0198 */ unk_D_800FCED8_0198 pcmBuffer; // decoded PCM ring buffer; func_80045FF0's decode destination, aliased via D_800FCEE0[i] = this+0x198
    /* 0x1598 */ unk_D_800FCED8_01598 unk_1598[1]; // compressed source-data ring, DMA'd via dmaReadFunc; declared size undersells - indexed by (dmaBytesRequested & 0xFFF) >> 3
    /* 0x15A0 */ u32 unk_15A0;
    /* 0x15A4 */ char unk_15A4[0xFF4];
    /* 0x2598 */ u16 chunksRemaining; // AudioStream_RefillAndDecode: decremented per decoded frame
    /* 0x259A */ u16 unk_259A; // set from unk_1598[0]'s header once per chunk; never read afterward
    /* 0x259C */ u32 chunkDwordsRemaining; // AudioStream_RefillAndDecode: decremented by 0x100 as compressed data is DMA'd
    /* 0x25A0 */ OSMesgQueue dmaMsgQueue;
    /* 0x25B8 */ OSMesg dmaMsgBuf;
    /* 0x25BC */ u32 lastChunkOffset; // set to 0x60 on first chunk; feeds dmaBytesConsumed's (>>3 & ~3) calc
    /* 0x25C0 */ u32 dmaBytesConsumed; // derived from lastChunkOffset after each decode iteration
    /* 0x25C4 */ u32 dmaBytesRequested; // AudioStream_RefillAndDecode: incremented by 0x400 per DMA read
    /* 0x25C8 */ s32 samplesConsumed; // AudioStream_ConsumeSamples's playback cursor
    /* 0x25CC */ u32 samplesDecoded; // AudioStream_RefillAndDecode's decode-produced cursor
    /* 0x25D0 */ s8 dmaPending; // AudioStream_RefillAndDecode: 0/1, a DMA read is currently in flight
    /* 0x25D1 */ s8 state; // 0=idle,1=loading,2=readyToMark,3=playing,4=aborted/done (AudioStream_MarkLoaded/Abort)
    /* 0x25D2 */ s8 decodeBudget; // AudioStream_RefillAndDecode: caps decode iterations per call
    /* 0x25D3 */ char unk25D3[0x5];
} unk_D_800FCED8; // size = 0x25D8

s32 func_80045FF0(unk_D_800FCED8*, void*);

#endif // _46680_H_
