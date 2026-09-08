#include "3D140.h"
#include "lib/ultralib/include/PR/leo.h"
#include "src/audio_commands.h"
#include "src/libnumus/player.h"
#include "src/3D140.h"
#include "src/gb_audio.h"
#include "src/libnaudio/n_libaudio_sc.h"
#include "src/libnaudio/n_libaudio_sn_sc.h"
#include "src/audio_stream.h"
#include "src/gb_audio_render.h"
#include "src/audio_loop_point.h"

typedef union {
    struct {
        s16 type;
    } gen;
    struct {
        s16 type;
        struct AudioInfo* info;
    } done;
} AudioMsg; // size = 0x8

typedef struct AudioInfo {
    /* 0x00 */ s16* data;
    /* 0x04 */ s16 frameSamples;
    /* 0x08 */ AudioMsg msg;
} AudioInfo; // size = 0x10

#define NUM_ACMD_LISTS 2
#define NUM_OUTPUT_BUFFERS 2
#define MAX_MESGS 3
#define NUM_DMA_BUFFERS 48
#define SAMPLES 184

typedef struct AMAudioMgr {
    /* 0x000 */ Acmd* ACMDList[NUM_ACMD_LISTS];
    /* 0x008 */ AudioInfo* audioInfo[NUM_OUTPUT_BUFFERS];
    /* 0x010 */ char unk10[0x228];
    ///* 0x010 */ OSThread thread;
    ///* 0x1C0 */ OSMesgQueue audioFrameMsgQ;
    ///* 0x1D8 */ OSMesg audioFrameMsgBuf[MAX_MESGS];
    ///* 0x1E0 */ OSMesgQueue audioReplyMsgQ;
    ///* 0x1F8 */ OSMesg audioReplyMsgBuf[MAX_MESGS];
    /* 0x238 */ ALGlobals g;
} AMAudioMgr; // size = 0x288

typedef struct AMDMABuffer {
    /* 0x00 */ ALLink node;
    /* 0x08 */ u32 startAddr;
    /* 0x0C */ u32 lastFrame;
    /* 0x10 */ char* ptr;
} AMDMABuffer; // size = 0x14

typedef struct AMDMAState {
    /* 0x00 */ u8 initialized;
    /* 0x04 */ AMDMABuffer* firstUsed;
    /* 0x08 */ AMDMABuffer* firstFree;
} AMDMAState; // size = 0xC

typedef struct unk_D_800FCB48 {
    /* 0x00 */ u16 unk_00[16];
    /* 0x20 */ s32 unk_20;
    /* 0x24 */ s32 unk_24;
    /* 0x28 */ u16 unk_28[16];
    /* 0x48 */ s32 unk_48;
    /* 0x4C */ s32 unk_4C;
    /* 0x50 */ u16 unk_50[16];
    /* 0x70 */ s32 unk_70;
    /* 0x74 */ s32 unk_74;
    /* 0x78 */ u32 unk_78;
    /* 0x7C */ s32 unk_7C;
    /* 0x80 */ s32 unk_80;
} unk_D_800FCB48; // size = 0x84

typedef struct unk_D_80078548 {
    /* 0x00 */ u8 unk_00;
    /* 0x01 */ u8 unk_01;
} unk_D_80078548;

// .data
extern u8 D_80077D90[];// = { 0, 0, 0, 0 };
extern f32 D_80077D94;// = 0.8000000119f;
extern u8 D_80077D98[];// = { 0, 0, 0, 0 };
extern u8 D_80077D9C[];// = { 0, 0, 0, 0 };
extern u16 D_80077DA0;
extern u32 D_80077DA4;
extern u8 D_80077DA8;
extern s32 D_80077DAC;
extern u32 audFrameCt;
extern u32 nextDMA;
extern u32 curAcmdList;
extern AudioInfo* lastInfo;
extern s32 min_only_one;
// global
extern s32 mus_active_fade_handle;
extern f32 mus_fade_start_volume;
extern u8 mus_fade_target_volume;
extern f32 mus_fade_step_rate;
extern f32 D_80077DD4;
extern f32 D_80077DD8;
extern f32 D_80077DDC;
extern f32 D_80077DE0;
extern u8 mus_sound_effect_blocked;

extern u16 D_80077DF0[];
extern u16 D_80077E00[];
extern u16 D_80077E10[];
extern u16 D_80077E18[];
extern u16 D_80077E20[];
extern u16 D_80077E28[];
extern u16 D_80077E30[];
extern u16 D_80077E38[];
extern u16 D_80077E40[];
extern u16 D_80077E48[];
extern u16 D_80077E50[];
extern u16 D_80077E58[];
extern u16 D_80077E60[];
extern u16 D_80077E68[];
extern u16 D_80077E70[];
extern u16 D_80077E78[];
extern u16 D_80077E88[];
extern u16 D_80077E90[];
extern u16 D_80077E98;
extern u16 D_80077EA4;
extern u16 D_80077EB4;
extern u16 D_80077EC0;
extern u16 D_80077ECC;
extern u16 D_80077ED8;
extern u16 D_80077EE4;
extern u16 D_80077EF0;
extern u16 D_80077EF8;
extern u16 D_80077F04;
extern u16 D_80077F0C;
extern u16 D_80077F14;
extern u16 D_80077F1C;
extern u16 D_80077F24;
extern u16 D_80077F2C;
extern u16 D_80077F34;
extern u16 D_80077F3C;
extern u16 D_80077F44;
extern u16 D_80077F50;
extern u16 D_80077F58;
extern u16 D_80077F60;
extern u16 D_80077F6C[];
extern u16 D_80077F74[];
extern u16 D_80077F7C[];
extern u16 D_80077F88[];
extern u16 D_80077F90[];
extern u16 D_80077F98[];
extern u16 D_80077FA4[];
extern u16 D_80077FAC[];
extern u16 D_80077FB4[];
extern u16 D_80077FC0[];
extern u16 D_80077FC8[];
extern u16 D_80077FD0[];
extern u16 D_80077FD8[];
extern u16 D_80077FE4[];
extern u16 D_80077FEC[];
extern u16 D_80077FF4[];
extern u16 D_80077FFC[];
extern u16 D_80078004[];
extern u16 D_8007800C[];
extern u16 D_80078014[];
extern u8 D_8007801C;
extern u8 D_80078024;
extern u8 D_80078030;
extern u8 D_80078038;
extern u16 D_80078040[];
extern u16 D_80078048[];
extern u16 D_80078050;
extern u16 D_80078058;
extern u16 D_80078060;
extern u16 D_80078068[];
extern u8 D_80078070;
extern u16 D_80078078;
extern u16 D_80078080;
extern u16 D_80078088;
extern u16 D_80078090;
extern u16 D_80078098;
extern u16 D_800780A0;
extern u16 D_800780A8[];
extern u8 D_800780B3[]; // needs addressing
extern u8 D_800780B4[];
extern u8 D_80078180[];
extern u8 D_800781E0[];
extern u8 D_800782A4[];
extern s32 D_80078388;
extern u8 D_8007838C[4];
extern u8 D_80078390[2];
extern u8 D_80078394[4];
extern u8 D_80078398[4];
extern u8 D_8007839C[];
extern u8 D_800783A0;
extern u32 D_800783A4[4];
extern u32 D_800783B4;
extern s32 D_800783B8;
extern u8 D_800783BC;
extern u8 D_800783C0;
extern u8 D_800783C4;
extern u32 D_800783C8;
extern u32 D_800783CC;
extern u8 D_800783D0;
extern u8 D_800783D4;
extern u8 D_800783D8;
extern u8 D_800783DC;
extern u8 D_800783E0;
extern u8 D_800783E4;
extern u8 D_800783E8;
extern u8 D_800783EC;
extern s16 D_800783F0;
extern s16 D_800783F4;
extern s16 D_800783F8;
extern u8 D_800783FC;
extern u8 D_80078400;
extern u32 D_80078404;
extern s32 D_80078408;
extern u8 D_8007840C;
extern u16 D_80078410[1][9];
extern u16 D_80078446[1][9];
extern u16 D_800784EA[1][9];
extern u32 D_80078544;
extern unk_D_80078548 D_80078548[];
extern u8 D_80078554[];
extern u8 D_80078568[];

// .rodata

// const f64 D_8007C7E0 = 0.699999999999999956f;
// const f64 D_8007C7E8 = 0.599999999999999978f;
// const f64 D_8007C7F0 = 0.400000000000000022f;
// const f64 D_8007C7F8 = 0.299999999999999989f;

// const f32 D_8007CB80 = 1.100000024f;
// const f32 D_8007CB84 = 1.100000024f;
// const f32 D_8007CB88 = 1.200000048f;
// const f32 D_8007CB8C = 1.100000024f;
// const f32 D_8007CB90 = 0.009999999776f;

extern f64 D_8007C7E0;
extern f64 D_8007C7E8;
extern f64 D_8007C7F0;
extern f64 D_8007C7F8;

extern f32 D_8007CB80;
extern f32 D_8007CB84;
extern f32 D_8007CB88;
extern f32 D_8007CB8C;
extern f32 D_8007CB90;

// .bss
s32 D_800FC820;
s8 D_800FC824;
u8 D_800FC825;
s32 D_800FC828;
AMAudioMgr __am;
AMDMAState dmaState;
s32 minFrameSize;
u32 frameSize;
s32 maxFrameSize;
s32 maxRSPCmds;
s32 D_800FCAD4;
s32 D_800FCAD8;
OSMesgQueue audDMAMessageQ;
AMDMABuffer* dmaBuffs;
OSIoMesg* audDMAIOMesgBuf;
OSMesg* audDMAMessageBuf;
u32 dmaBufferLen;
OSPiHandle* D_800FCB08[4];
// s32 D_800FCB0C[3];
BattleMonRuntime* D_800FCB18[4];
s32 D_800FCB28[2];
u8 D_800FCB30[2];
u8 D_800FCB38[2][8];
unk_D_800FCB48 D_800FCB48[2];
s32 D_800FCC50[20];
s8 D_800FCCA0;
s8 D_800FCCA1;
u8 D_800FCCA2;
u32 D_800FCCA4;
s32 D_800FCCA8;
u8 D_800FCCAC;
u8 D_800FCCAD;
u8 D_800FCCAE;
u8 D_800FCCAF;
u8 D_800FCCB0;
u8 D_800FCCB1;
u8 D_800FCCB2;
u8 D_800FCCB3;
u8 D_800FCCB4;
u8 D_800FCCB5;
u8 D_800FCCB6;
u8 D_800FCCB7;
u8 D_800FCCB8[2];
u8 D_800FCCBA[2];
u8 D_800FCCBC[2];
u8 D_800FCCBE[2];
u8 D_800FCCC0[2];
u8 D_800FCCC2[2];
s8 D_800FCCC4[2];
u8 D_800FCCC6[2];
u8 D_800FCCC8[2];
u8 D_800FCCCA[2];
u8 D_800FCCCC[2];
u8 D_800FCCCE[2];
u8 D_800FCCD0[2];
u8 D_800FCCD2[2];
u8 D_800FCCD4[2];
u8 D_800FCCD6[2];
s8 D_800FCCD8[0x18];
u32 D_800FCCF0[10];
u8 D_800FCD18[0x28];

// function prototypes
void Audio_QueueSoundWithFallbackList(u16, u16*, u8*);
u32 func_80042158(u32, BattleMonRuntime*, BattleMonRuntime*);
s32 Battle_MapCategoryIdToCompactIndex(u8);
ALDMAproc __amDmaNew(AMDMAState** state);

void amCreateAudioMgr(ALSynConfig* c, amConfig* amc, u32 num_dma_buffers, s32 arg3, s32 arg4) {
    u32 i;
    f32 fsize;

    D_800FCB08[0] = osCartRomInit();
    dmaBufferLen = arg3;

    dmaState.initialized = FALSE;

    c->dmaproc = __amDmaNew;
    c->outputRate = amc->outputRate;

    i = num_dma_buffers;
    i <<= 1;
    dmaBuffs = alHeapAlloc(c->heap, 1, num_dma_buffers * sizeof(AMDMABuffer));
    audDMAIOMesgBuf = alHeapAlloc(c->heap, 1, i * sizeof(OSIoMesg));
    audDMAMessageBuf = alHeapAlloc(c->heap, 1, i * sizeof(OSMesg));

    fsize = (f32)amc->framesPerField * c->outputRate / arg4;
    frameSize = (s32)fsize;
    if (frameSize < fsize) {
        frameSize++;
    }

    frameSize = ((frameSize / SAMPLES) + 1) * SAMPLES;
    minFrameSize = frameSize - SAMPLES;
    maxFrameSize = frameSize + 0x100;

    alInit(&__am.g, c);

    dmaBuffs[0].node.prev = NULL;
    dmaBuffs[0].node.next = NULL;

    for (i = 0; i < num_dma_buffers - 1; i++) {
        alLink(&dmaBuffs[i + 1].node, &dmaBuffs[i].node);
        dmaBuffs[i].ptr = alHeapAlloc(c->heap, 1, arg3);
    }

    dmaBuffs[i].ptr = alHeapAlloc(c->heap, 1, arg3);

    for (i = 0; i < 2; i++) {
        __am.ACMDList[i] = alHeapAlloc(c->heap, 1, amc->maxACMDSize * sizeof(Acmd));
    }

    maxRSPCmds = amc->maxACMDSize;

    for (i = 0; i < 3; i++) {
        __am.audioInfo[i] = alHeapAlloc(c->heap, 1, sizeof(AudioInfo));
        __am.audioInfo[i]->msg.done.type = 0;
        __am.audioInfo[i]->msg.done.info = __am.audioInfo[i];
        __am.audioInfo[i]->data = alHeapAlloc(c->heap, 1, maxFrameSize * 4);
    }

    osCreateMesgQueue(&audDMAMessageQ, audDMAMessageBuf, num_dma_buffers << 1);

    D_800FCAD4 = c->outputRate;
    D_800FCAD8 = c->maxVVoices;
    D_800FC824 = 0;

    AudioStream_Init(c->heap, minFrameSize, maxFrameSize);
}

s32 __amDMA(s32 addr, s32 len, void* state) {
    void* foundBuffer;
    s32 delta;
    AMDMABuffer* sp34;
    OSIoMesg* ioMesg;
    AMDMABuffer* dmaPtr;
    AMDMABuffer* lastDmaPtr;
    OSPiHandle* sp2C;
    s32 buffEnd;

    if (D_800FC820 & 0x80000000) {
        return osVirtualToPhysical(addr);
    }

    sp2C = D_800FCB08[0];
    dmaPtr = dmaState.firstUsed;
    lastDmaPtr = NULL;
    while (dmaPtr) {
        buffEnd = dmaPtr->startAddr + dmaBufferLen;

        if (dmaPtr->startAddr > addr) {
            break;
        }

        if (buffEnd >= (addr + len)) {
            dmaPtr->lastFrame = audFrameCt;
            return osVirtualToPhysical(dmaPtr->ptr + addr - dmaPtr->startAddr);
        }

        lastDmaPtr = dmaPtr;
        dmaPtr = dmaPtr->node.next;
    }

    dmaPtr = dmaState.firstFree;
    if (!dmaPtr) {
        return osVirtualToPhysical(dmaState.firstUsed);
    }

    dmaState.firstFree = dmaPtr->node.next;
    alUnlink(&dmaPtr->node);

    if (lastDmaPtr != NULL) {
        alLink(&dmaPtr->node, &lastDmaPtr->node);
    } else if (dmaState.firstUsed) {
        lastDmaPtr = dmaState.firstUsed;

        dmaState.firstUsed = dmaPtr;
        dmaPtr->node.next = lastDmaPtr;
        dmaPtr->node.prev = NULL;

        lastDmaPtr->node.prev = &dmaPtr->node;
    } else {
        dmaState.firstUsed = dmaPtr;
        dmaPtr->node.next = NULL;
        dmaPtr->node.prev = NULL;
    }

    foundBuffer = dmaPtr->ptr;
    delta = addr & 1;
    addr -= delta;
    dmaPtr->startAddr = addr;
    dmaPtr->lastFrame = audFrameCt;

    ioMesg = audDMAIOMesgBuf + nextDMA++;
    ioMesg->hdr.pri = OS_MESG_PRI_HIGH;
    ioMesg->hdr.retQueue = &audDMAMessageQ;
    ioMesg->devAddr = addr;
    ioMesg->dramAddr = foundBuffer;
    ioMesg->size = dmaBufferLen;

    osInvalDCache(foundBuffer, dmaBufferLen);
    osEPiStartDma(sp2C, ioMesg, OS_READ);

    return osVirtualToPhysical(foundBuffer) + delta;
}

OSTask* Audio_CreateFrameTask(OSTask* arg0) {
    s16* audioPtr;
    Acmd* cmdp;
    s32 cmdLen;
    s32 samplesLeft;
    OSTask sp38;
    s32 pad[2];
    AudioInfo* info;
    AMAudioMgr* mgr;

    info = __am.audioInfo[audFrameCt % 3];
    audioPtr = osVirtualToPhysical(info->data);
    samplesLeft = HW_REG(AI_LEN_REG, u32) >> 2;

    if ((D_80077DA8 == 0) && (lastInfo != NULL)) {
        osAiSetNextBuffer(lastInfo->data, lastInfo->frameSamples << 2);
    }

    if ((samplesLeft >= 0x1A9) && (min_only_one != 0)) {
        info->frameSamples = minFrameSize;
        min_only_one = 0;
    } else {
        info->frameSamples = frameSize;
        min_only_one = 1;
    }

    __clearAudioDMA();

    if (D_80077DAC != 0) {
        D_800FC828 = LeoTestUnitReady(&D_800FC825);
    }

    cmdp = alAudioFrame(__am.ACMDList[curAcmdList], &cmdLen, audioPtr, info->frameSamples);
    sp38.t.data_ptr = __am.ACMDList[curAcmdList];
    sp38.t.data_size = (cmdp - __am.ACMDList[curAcmdList]) * sizeof(Acmd);

    sp38.t.type = M_AUDTASK;
    sp38.t.ucode_boot = &rspbootTextStart;
    sp38.t.ucode_boot_size = (s32)_binary_assets_us_F3DEX2_bin_start - (s32)rspbootTextStart;
    sp38.t.flags = 0;
    sp38.t.ucode = &aspMainTextStart;
    sp38.t.ucode_data = &aspMainDataStart;
    sp38.t.ucode_data_size = SP_UCODE_DATA_SIZE;
    sp38.t.dram_stack = NULL;
    sp38.t.dram_stack_size = 0;
    sp38.t.output_buff = NULL;
    sp38.t.output_buff_size = 0;
    sp38.t.yield_data_ptr = NULL;
    sp38.t.yield_data_size = 0;

    curAcmdList ^= 1;
    lastInfo = __am.audioInfo[audFrameCt % 3];
    audFrameCt++;

    if (D_80077D9C[0] != 0) {
        func_8003D32C();
    }

    if (D_80077D90[0] != 0) {
        GbMusic_UpdateChannels();
        if (lastInfo != NULL) {
            GbAudio_GenerateSamples(lastInfo->data, 0, ((audFrameCt % 3) == 0) ? 0x216 : 0x215, 0);
        }
    }

    if (D_80077D98[0] != 0) {
        func_80044EA4();
    }

    Audio_UpdateBattleAudioFrame();

    if (mus_active_fade_handle != 0) {
        mus_fade_start_volume += mus_fade_step_rate;
        if (mus_fade_step_rate > 0.0f) {
            if (mus_fade_target_volume <= mus_fade_start_volume) {
                mus_fade_start_volume = mus_fade_target_volume;
            }
        } else {
            if (mus_fade_start_volume <= mus_fade_target_volume) {
                mus_fade_start_volume = mus_fade_target_volume;
            }
        }

        Audio_QueueVoiceVolumeScale(mus_active_fade_handle, (u32)mus_fade_start_volume & 0xFF);

        if (mus_fade_start_volume == mus_fade_target_volume) {
            mus_active_fade_handle = 0;
        }
    }

    if (D_80077DD4 != 0.0f) {
        D_80077DD4 -= D_80077DD8;
        if (D_80077DD4 <= 0) {
            D_80077DD4 = 0.0f;
            Audio_QueueFadeCategoryCommand(2, 1);
        }
        Audio_QueueSetVolumeCommand(2, D_80077DD4);
    }

    if (D_80077DDC != 0.0f) {
        D_80077DDC -= D_80077DE0;
        if (D_80077DDC <= 0) {
            D_80077DDC = 0.0f;
            Audio_QueueFadeCategoryCommand(1, 1);
        }
        Audio_QueueSetVolumeCommand(1, D_80077DDC);
    }

    if (D_80077DA8 != 0) {
        sp38.t.data_size = 0;
    }

    *arg0 = sp38;

    return arg0;
}

ALDMAproc __amDmaNew(AMDMAState** state) {
    if (!dmaState.initialized) {
        dmaState.firstUsed = NULL;
        dmaState.firstFree = &dmaBuffs[0];
        dmaState.initialized = TRUE;
    }

    *state = &dmaState;

    return __amDMA;
}

#define FRAME_LAG 1

void __clearAudioDMA(void) {
    s32 i;
    void* sp40;
    AMDMABuffer* var_s0_2;
    ALLink* node;

    for (i = 0; i < nextDMA; i++) {
        osRecvMesg(&audDMAMessageQ, &sp40, 0);
    }

    var_s0_2 = dmaState.firstUsed;
    while (var_s0_2 != NULL) {
        node = var_s0_2->node.next;

        if ((var_s0_2->lastFrame + FRAME_LAG) < audFrameCt) {
            if (var_s0_2 == dmaState.firstUsed) {
                dmaState.firstUsed = var_s0_2->node.next;
            }

            alUnlink(var_s0_2);

            if (dmaState.firstFree != NULL) {
                alLink(var_s0_2, dmaState.firstFree);
            } else {
                dmaState.firstFree = var_s0_2;
                var_s0_2->node.next = NULL;
                var_s0_2->node.prev = NULL;
            }
        }

        var_s0_2 = node;
    }

    nextDMA = 0;
}


void Cry_SetEnabled(s32 arg0) {
    const f32 D_8007C7C0 = 0.8000000119f;
    if (arg0 != 0) {
        func_800498A8(0x7D00, 0x78, 0x640);
        D_80077D90[0] = 1;
        D_80077D94 = D_8007C7C0;
        return;
    }
    D_80077D90[0] = 0;
}

void Cry_PlayFromPokedex(s32 arg0) {
    if (D_80077D90[0] == 0) {
        func_800498A8(0x7D00, 0x78, 0x320);
        D_80077D90[0] = 1;
        D_80077D9C[0] = 1;
        D_80077D94 = 0.0f;
        D_80077DA0 = arg0;
        D_80077DA4 = 0;
        D_800FC824 = 0;
    }
}

#ifdef NON_MATCHING
// Matches but issues with .rodata
void func_8003D32C(void) {
    switch (D_80077D9C[0]) {
        case 1:
            Audio_StartVolumeTransition(D_80078E70, 0x50, 0x18, 3);
            GbMusic_StartTableSequence(D_80077DA0);
            D_80077D9C[0]++;
            D_80077DA4 = 0;
            break;

        case 2:
            D_80077DA4++;
            if (D_80077DA4 >= 2) {
                D_80077DA4 = 0;
                D_80077D9C[0]++;
                D_800FC824 = 1;
            }
            break;

        case 3:
            if (GbMusic_CountActiveHighChannels() == 0) {
                D_80077D9C[0]++;
                D_80077DA4 = 0;
            }
            break;

        case 4:
            if (D_80077DA4 == 0) {
                Audio_StartVolumeTransition(D_80078E70, 0x18, 0x50, 0x14);
            }

            if (D_80077DA4 >= 0x1F) {
                D_80077D9C[0]++;
            }

            D_80077DA4++;
            break;

        case 5:
            D_80077D9C[0] = 0;
            D_80077D90[0] = 0;
            D_800FC824 = 0;
            break;
    }
}
#else
#pragma GLOBAL_ASM("asm/us/nonmatchings/3D140/func_8003D32C.s")
#endif

s32 Cry_IsPlaying(void) {
    return D_80077D90[0];
}

s32 Audio_SetEnabled(s32 arg0) {
    if (arg0 == 0) {
        if (mus_free_voices_count != D_800FCAD8) {
            return 0;
        }
        D_80077DA8 = 1;
    } else {
        osAiSetFrequency(D_800FCAD4);
        lastInfo = NULL;
        dmaState.initialized = 0;
        D_80077DA8 = 0;
    }
    return 1;
}

s32 Audio_GetDiskReadyStatus(u8* arg0) {
    if (D_80077DAC == 0) {
        *arg0 = 7;
        return 8;
    }
    *arg0 = D_800FC825;
    return D_800FC828;
}

void Audio_SetDiskReadyPolling(s32 arg0) {
    if (arg0 == 1) {
        D_80077DAC = 1;
        return;
    }
    D_80077DAC = 0;
}

/**
 * Sets or clears a bit in an array.
 */
void BitArray_SetBit(u8* array, u8 index, u8 flag) {
    u8 offset = index / 8;
    u8 bitPosition = index % 8;
    u8 mask = (1 << bitPosition);
    if (flag) {
        array[offset] |= mask;   // Set the bit
    } else {
        array[offset] &= ~mask;  // Clear the bit
    }
}

/**
 * Checks if a bit is set in an array.
 */
s32 BitArray_IsBitSet(u8* array, u8 index) {
    u8 offset = index / 8;
    u8 bitPosition = index % 8;
    u8 flag = (array[offset] & (u8)(1 << bitPosition));
    if (flag) {
        return 1;
    }
    return 0;
}

void Audio_SetVoiceEnabled(s32 arg0) {
    if (arg0 != 0) {
        D_80078388 = 1;
    } else {
        D_80078388 = 0;
    }
}

void Audio_SetChannelGroupCounts(u8 arg0, u8 arg1, u8 arg2, u8 arg3) {
    D_8007838C[0] = arg0;
    D_8007838C[1] = arg1;
    D_8007838C[2] = arg2;
    D_8007838C[3] = arg3;
}

void Audio_ResetChannelGroupState(u8 arg0) {
    s32 i;

    if (arg0 < 3) {
        for (i = 0; i < 16; i++) {
            D_800FCB48[arg0].unk_00[i] = 0;
            D_800FCB48[arg0].unk_28[i] = 0;
            D_800FCB48[arg0].unk_50[i] = 0;
        }
        D_800FCB48[arg0].unk_20 = 0;
        D_800FCB48[arg0].unk_24 = 0;
        D_800FCB48[arg0].unk_48 = 0;
        D_800FCB48[arg0].unk_4C = 0;
        D_800FCB48[arg0].unk_70 = 0;
        D_800FCB48[arg0].unk_74 = 0;
        D_800FCB48[arg0].unk_78 = 0;
        D_800FCB48[arg0].unk_7C = 0;
        D_800FCB48[arg0].unk_80 = 0;
        D_800FCCBC[arg0] = 0;
        D_800FCCBE[arg0] = 0;
        D_800FCCC4[arg0] = 0;
        D_800FCCC6[arg0] = 0;
        D_800FCCC8[arg0] = 0;
        D_800FCCCA[arg0] = 0;
        D_800FCCCC[arg0] = 0;
        D_800FCCCE[arg0] = 0;
        D_800FCCD0[arg0] = 0;
        D_800FCCD2[arg0] = 0;
        D_800FCCD4[arg0] = 0;
        D_800FCCC2[arg0] = 0;
        D_800FCCC0[arg0] = 0;
    }
}

void Audio_ConfigureChannelGroups(u8 arg0, u8 arg1, u8 arg2, u8 arg3) {
    s32 i;
    s32 j;

    D_80078398[0] = arg0;
    D_80078398[1] = arg1;
    D_80078398[2] = arg2;
    D_80078398[3] = arg3;
    D_80078394[0] = 0xFF;
    D_80078394[1] = 0xFF;
    D_80078390[0] = 0;
    D_80078390[1] = 0;
    for (i = 0; i < 2; i++) {
        for (j = 0; j < 4; j++) {
            if (i + 1 == D_80078398[j]) {
                D_80078390[i] += D_8007838C[j];
            }
        }
    }
    D_800FCB18[0] = 0;
    D_800FCB18[1] = 0;
    D_800FCB28[0] = 0;
    D_800FCB28[1] = 0;
    Audio_ResetChannelGroupState(0);
    Audio_ResetChannelGroupState(1);
}

void Audio_SelectModeMusic(u32 arg0, u32 arg1) {
    s32 pad[1];
    u8 sp1B = 0;
    u16 var_a0;

    func_80041A98();
    D_80078404 = arg1;
    D_80078408 = arg0;

    if (arg1 < 0x11) {
        D_800783FC = 1;
        D_8007840C = 1;

        if ((arg0 == 0) && (arg1 != 0x10) && (arg1 != 0xF)) {
            Audio_QueueSequenceId(0x34);
        } else if ((arg0 == 0xFF) && (arg1 != 0x10) && (arg1 != 0xF)) {
            Audio_QueueSequenceId(0x47);
            arg0 = 0;
            sp1B = 1;
        }

        if (arg1 < 0xC) {
            if (arg0 < 9) {
                var_a0 = D_80078410[arg1][arg0];
                D_800783FC = 1;
                D_80078400 = 1;
            } else {
                return;
            }
        } else if (arg1 == 0xC) {
            if ((arg0 < 0x21) && (arg0 != 0)) {
                var_a0 = D_800784EA[(arg0 - 1) / 8][(arg0 - 1) % 8];
                D_800783FC = 1;
                D_80078400 = 0;
            } else {
                return;
            }
        } else if (arg1 == 0xD) {
            if (arg0 < 6) {
                var_a0 = D_80078446[arg1][arg0];
                D_800783FC = 1;
                D_80078400 = 0;
            } else {
                return;
            }
        } else {
            if (arg1 == 0xE) {
                var_a0 = 0x49;
                D_800783FC = 1;
                D_80078400 = 2;
            } else if (arg1 == 0xF) {
                var_a0 = 0;
                D_800783FC = 0;
                D_80078400 = 0;
                arg0 = 1;
            } else {
                if (arg1 == 0x10) {
                    var_a0 = 0x48;
                    D_80078400 = 1;
                    arg0 = 1;
                } else {
                    var_a0 = 0;
                    D_80078400 = 1;
                }
                D_800783FC = 1;
            }
        }

        if ((arg0 == 0) && (sp1B == 0)) {
            D_80078400 = 2;
        }

        if (var_a0 != 0) {
            Audio_QueueSequenceId(var_a0);
        }
    }
}

#ifdef NON_MATCHING
void func_8003DB84(s32 arg0) {
    s32 i;
    s32 var_t0;
    s32 var_t2;
    s32 var_t4;
    s32 var_t5;
    s32 var_v0;
    u32 var_a0;
    u32 temp_v0;

    if (arg0 != 0) {
        temp_v0 = osSetIntMask(1);
        D_800783BC = 1;
        D_800783C8 = 0;
        D_800783C0 = 0;
        D_800783C4 = 0;
        osSetIntMask(temp_v0);
        D_800783D4 = 0;
        D_800783D8 = 0;
        D_800783DC = 0;
        D_800FCCAE = 0;
        D_800FCCAF = 0;
        D_800FCCB2 = 0;
        D_800FCCB6 = 0;
        D_800783E8 = 0;
        D_800783EC = 0;
        D_800FCCA4 = 0;
        Audio_PlayCommand(0x5A, D_80078400, 0);

        if ((D_800FCB18[0] != NULL) && (D_800FCB18[1] != NULL)) {
            if (D_800FCB18[0] != NULL) {
                D_800FCB28[0] = D_800FCB18[0]->unk_0C;
            }

            if (D_800FCB18[1] != NULL) {
                D_800FCB28[1] = D_800FCB18[1]->unk_0C;
            }

            D_800FCB48[0].unk_24++;
            D_800FCB48[0].unk_00[D_800FCB48[0].unk_24 & 0xF] = D_800FCB18[0]->unk_0C;
            D_800FCB48[1].unk_24++;
            D_800FCB48[1].unk_00[D_800FCB48[1].unk_24 & 0xF] = D_800FCB18[1]->unk_0C;

            if (D_800783CC != 0) {
                var_t0 = 1;
                if (D_800FCB18[0]->unk_26 < D_800FCB18[1]->unk_26) {
                    var_t2 = 0;
                    var_t4 = 1;
                } else {
                    var_t4 = 0;
                    var_t2 = 1;
                }

                if (D_800FCB18[0]->unk_0C < D_800FCB18[1]->unk_0C) {
                    var_t0 = 0;
                    var_t5 = 1;
                } else {
                    var_t5 = 0;
                }

                if (D_800783CC < 2) {
                    if ((D_800FCB18[0]->unk_0C == 0) || (D_800FCB18[1]->unk_0C == 0)) {
                        temp_v0 = osSetIntMask(1);
                        D_800783C0 = 1;
                        osSetIntMask(temp_v0);

                        if (D_800FCB18[0]->unk_0C == 0) {
                            D_800FCCB0 = 1;
                        } else {
                            D_800FCCB0 = 2;
                        }

                        if ((D_800FCB18[0]->unk_0C == 0) && (D_800FCB18[1]->unk_0C == 0)) {
                            Audio_QueueSoundAvoidingRecent(D_80077E90);
                            return;
                        }

                        Audio_QueueSoundAvoidingRecent(D_80077E48);
                        return;
                    }

                    if ((((D_800FCB18[0]->unk_0C * 0x64u) / D_800FCB18[0]->unk_28) < 0x1E) &&
                        (((D_800FCB18[1]->unk_0C * 0x64u) / D_800FCB18[1]->unk_28) < 0x1E)) {
                        Audio_QueueSoundAvoidingRecent(D_80077E88);
                        return;
                    }

                    if (((((D_800FCB18[var_t4]->unk_26 - D_800FCB18[var_t2]->unk_26) * 0x64u) /
                          D_800FCB18[var_t2]->unk_26) >= 0x15) &&
                        (((D_800FCB18[var_t2]->unk_0C * 0x64u) / D_800FCB18[var_t2]->unk_28) < 0x28) &&
                        (((D_800FCB18[var_t4]->unk_0C * 0x64u) / D_800FCB18[var_t4]->unk_28) >= 0x51)) {
                        if ((osGetTime() % 3) == 0) {
                            var_a0 = 0x256;
                            goto block_53;
                        }
                    }

                    if ((((D_800FCB18[0]->unk_0C * 0x64u) / D_800FCB18[0]->unk_28) >= 0x5B) &&
                        (((D_800FCB18[1]->unk_0C * 0x64u) / D_800FCB18[1]->unk_28) >= 0x5B)) {
                        Audio_QueueSoundAvoidingRecent(D_80077E68);
                        return;
                    }

                    if (D_80078400 == 0) {
                        Audio_QueueSoundAvoidingRecent(D_80077E78);
                        return;
                    }

                    Audio_QueueSoundAvoidingRecent(D_80077DF0);
                    return;
                }

                if ((D_800FCB18[0]->unk_0C == 0) || (D_800FCB18[1]->unk_0C == 0)) {
                    temp_v0 = osSetIntMask(1);
                    D_800783C0 = 1;
                    osSetIntMask(temp_v0);

                    if (D_800FCB18[0]->unk_0C == 0) {
                        D_800FCCB0 = 1;
                    } else {
                        D_800FCCB0 = 2;
                    }

                    if (D_80078390[0] >= D_80078390[1]) {
                        var_v0 = D_80078390[0] - D_80078390[1];
                    } else {
                        var_v0 = D_80078390[1] - D_80078390[0];
                    }

                    if (var_v0 >= 2) {
                        Audio_QueueSoundAvoidingRecent(D_80077E70);
                        return;
                    }

                    if ((D_80078390[0] == 1) && (D_80078390[1] == 1)) {
                        var_a0 = 0x290;
                        if (D_800783E4 != 0) {
                            D_800783E4 = 0;
                        } else {
                            if ((osGetTime() % 2) != 0) {
                                var_a0 = 0x254;
                            } else {
                                var_a0 = 0x26B;
                            }
                        }
                    block_53:
                        Audio_QueueSequenceId(var_a0);
                        return;
                    }

                    if (D_80078390[D_800FCCB0 - 1] == 1) {
                        Audio_QueueSequenceId(0x26A);
                        return;
                    }

                    Audio_QueueSoundAvoidingRecent(D_80077E00);
                    return;
                }

                if (D_800FCCB0 != 0) {
                    if (((D_80078390[0] == 1) || (D_80078390[1] == 1)) && (D_80078390[0] < 4) && (D_80078390[1] < 4)) {
                        if (D_80078390[0] == 1) {
                            switch (D_80078390[1]) {
                                case 1:
                                    Audio_QueueSequenceId(0x260);
                                    Audio_QueueSequenceId(0x266);
                                    break;

                                case 2:
                                    Audio_QueueSequenceId(0x261);
                                    if (D_800FCCB0 == 1) {
                                        Audio_QueueSequenceId(0x265);
                                    } else {
                                        Audio_QueueSequenceId(0x267);
                                    }
                                    break;

                                case 3:
                                    Audio_QueueSequenceId(0x262);
                                    Audio_QueueSequenceId(0x264);
                                    break;

                                default:
                                    Audio_QueueSequenceId(0x271);
                                    break;
                            }

                            D_800FCCB0 = 0;
                            return;
                        }

                        switch (D_80078390[0]) {
                            case 2:
                                Audio_QueueSequenceId(0x25E);
                                if (D_800FCCB0 == 2) {
                                    Audio_QueueSequenceId(0x265);
                                } else {
                                    Audio_QueueSequenceId(0x267);
                                }
                                break;

                            case 3:
                                Audio_QueueSequenceId(0x25C);
                                Audio_QueueSequenceId(0x264);
                                break;

                            default:
                                Audio_QueueSequenceId(0x271);
                                break;
                        }

                        D_800FCCB0 = 0;
                        return;
                    }

                    if (D_800FCCB1 != 0) {
                        if ((osGetTime() % 2) != 0) {
                            D_800FCCB0 = 0;
                            Audio_QueueSoundAvoidingRecent(D_80077E10);
                            return;
                        }
                    }

                    if (D_800FCB18[0]->unk_16[6] != D_800FCB18[1]->unk_16[6]) {
                        if (D_800FCB18[0]->unk_16[6] != D_800FCB18[1]->unk_16[7]) {
                            if ((D_800FCB18[0]->unk_16[7] != D_800FCB18[1]->unk_16[6]) &&
                                (D_800FCB18[0]->unk_16[7] != D_800FCB18[1]->unk_16[7])) {
                                D_800FCCB0 = 0;
                                Audio_QueueSoundAvoidingRecent(D_80077E18);
                                return;
                            }
                        }
                    }

                    D_800FCCB0 = 0;
                    switch (D_80078390[0]) {
                        case 2:
                            switch (D_80078390[1]) {
                                case 2:
                                    Audio_QueueSequenceId(0x25D);
                                    Audio_QueueSequenceId(0x268);
                                    return;

                                case 3:
                                    Audio_QueueSequenceId(0x25F);
                                    Audio_QueueSequenceId(0x263);
                                    return;

                                default:
                                    Audio_QueueSequenceId(0x271);
                                    return;
                            }
                            break;

                        case 3:
                            if (D_80078390[1] == 2) {
                                Audio_QueueSequenceId(0x25B);
                                Audio_QueueSequenceId(0x263);
                                return;
                            }

                            Audio_QueueSequenceId(0x271);
                            return;

                        default:
                            if (D_80078400 == 0) {
                                Audio_QueueSoundAvoidingRecent(D_80077E78);
                                return;
                            }

                            Audio_QueueSoundAvoidingRecent(D_80077DF0);
                            return;
                    }
                } else {

                    if (D_800FCCB3 != 0) {
                        Audio_QueueSoundAvoidingRecent(D_80077E38);
                        D_800FCCB3 = 0;
                        return;
                    }

                    if (D_800FCCB4 != 0) {
                        Audio_QueueSoundAvoidingRecent(D_80077E40);
                        D_800FCCB4 = 0;
                        return;
                    }

                    if (D_800FCCB5 != 0) {
                        Audio_QueueSoundAvoidingRecent(D_80077E50);
                        D_800FCCB5 = 0;
                        return;
                    }

                    if ((D_800FCB18[0]->unk_28 * 0.5f) < (D_800FCB48[0].unk_00[(D_800FCB48[0].unk_24 - 1) & 0xF] -
                                                          D_800FCB48[0].unk_00[D_800FCB48[0].unk_24 & 0xF])) {
                        if ((D_800FCB18[1]->unk_28 * 0.5f) < (D_800FCB48[1].unk_00[(D_800FCB48[1].unk_24 - 1) & 0xF] -
                                                              D_800FCB48[1].unk_00[(D_800FCB48[1].unk_24 & 0xF)])) {
                            if ((((D_800FCB18[0]->unk_0C * 0x64u) / D_800FCB18[0]->unk_28) < 0x14u) &&
                                (((D_800FCB18[1]->unk_0C * 0x64u) / D_800FCB18[1]->unk_28) < 0x14u)) {
                                Audio_QueueSoundAvoidingRecent(D_80077E60);
                            } else {
                                Audio_QueueSoundAvoidingRecent(D_80077E58);
                            }
                            return;
                        }
                    }

                    if (D_800FCB18[0]->unk_0C >= D_800FCB18[1]->unk_0C) {
                        var_v0 = D_800FCB18[0]->unk_0C - D_800FCB18[1]->unk_0C;
                    } else {
                        var_v0 = D_800FCB18[1]->unk_0C - D_800FCB18[0]->unk_0C;
                    }

                    if ((D_800FCB18[var_t0]->unk_28 * D_8007C7E0) < var_v0) {
                        if ((((D_800FCB18[var_t5]->unk_0C * 0x64u) / D_800FCB18[var_t5]->unk_28) >= 0x47) &&
                            (D_800FCB18[0]->unk_0C != D_800FCB18[0]->unk_28) &&
                            (D_800FCB18[1]->unk_0C != D_800FCB18[1]->unk_28)) {
                            if ((D_800FCB18[var_t5]->unk_28 * D_8007C7E8) < D_800FCB18[var_t5]->unk_0C) {
                                if (D_800FCB18[var_t0]->unk_0C < (D_800FCB18[var_t0]->unk_28 * D_8007C7F0)) {
                                    Audio_QueueSoundAvoidingRecent(D_80077E20);
                                    return;
                                }
                            }
                        }
                    }

                    if (D_800FCB18[0]->unk_0C >= D_800FCB18[1]->unk_0C) {
                        var_v0 = D_800FCB18[0]->unk_0C - D_800FCB18[1]->unk_0C;
                    } else {
                        var_v0 = D_800FCB18[1]->unk_0C - D_800FCB18[0]->unk_0C;
                    }

                    if (var_v0 < 0xA) {
                        if ((D_800FCB18[0]->unk_28 * 0.5) < D_800FCB18[0]->unk_0C) {
                            if (((D_800FCB18[1]->unk_28 * 0.5) < D_800FCB18[1]->unk_0C) &&
                                (D_800FCB18[0]->unk_0C < D_800FCB18[0]->unk_28) &&
                                (D_800FCB18[1]->unk_0C < D_800FCB18[1]->unk_28)) {
                                Audio_QueueSoundAvoidingRecent(D_80077E30);
                                return;
                            }
                        }
                    }

                    if (D_800FCB18[0]->unk_0C >= D_800FCB18[1]->unk_0C) {
                        var_v0 = D_800FCB18[0]->unk_0C - D_800FCB18[1]->unk_0C;
                    } else {
                        var_v0 = D_800FCB18[1]->unk_0C - D_800FCB18[0]->unk_0C;
                    }

                    if (var_v0 < 0xA) {
                        if (D_800FCB18[0]->unk_0C < (D_800FCB18[0]->unk_28 * D_8007C7F8)) {
                            if (D_800FCB18[1]->unk_0C < (D_800FCB18[1]->unk_28 * D_8007C7F8)) {
                                Audio_QueueSoundAvoidingRecent(D_80077E28);
                                return;
                            }
                        }
                    }

                    if (D_80078400 == 0) {
                        Audio_QueueSoundAvoidingRecent(D_80077E78);
                    } else {
                        Audio_QueueSoundAvoidingRecent(D_80077DF0);
                    }
                    return;
                }
            }
        }
    } else {
        temp_v0 = osSetIntMask(1);
        D_800783BC = 0;
        osSetIntMask(temp_v0);
        Audio_FadeCommand(0x5A, 0);
        Audio_FadeCommand(0x5B, 0);

        if (D_800FCB18[0] != NULL) {
            D_800FCB28[0] = D_800FCB18[0]->unk_0C;
        }

        if (D_800FCB18[1] != NULL) {
            D_800FCB28[1] = D_800FCB18[1]->unk_0C;
        }

        D_800783CC++;

        for (i = 0; i < 2; i++) {
            D_800FCB48[i].unk_78++;
            if (D_800FCB48[i].unk_7C != 0) {
                D_800FCB48[i].unk_7C++;
            }
        }

        D_800FCCB4 = 0;
        D_800FCCB3 = 0;
        D_800FCCB5 = 0;
        D_800783E4 = 0;

        if ((D_800FCB18[0] != NULL) && (D_800FCB18[1] != NULL) && (D_8007840C != 0) && (D_800FCB18[0]->unk_0C != 0) &&
            (D_800FCB18[1]->unk_0C != 0) && (AudioStream_IsPlaying(1) == 0)) {
            if ((osGetTime() % 2) == 0) {
                Audio_QueueSoundAvoidingRecent(D_800780A8);
            }
        }
    }
}
#else
#pragma GLOBAL_ASM("asm/us/nonmatchings/3D140/func_8003DB84.s")
#endif

void func_8003EB40(s32 arg0, BattleMonRuntime* arg1) {
    u8 pad;
    u8 value;
    u8 side;
    u8 changed = 0;
    u32 i;
    u8 pad2;

    D_800FCCB1 = 0;
    D_800783F0 = 0;
    D_800783F4 = 0;
    D_800783F8 = 0;

    if (arg1 == NULL) {
        return;
    }
    if ((arg0 == 0) || ((u32)arg0 >= 5)) {
        return;
    }

    arg0--;
    side = D_80078398[arg0];
    if ((side <= 0) || (side >= 3)) {
        return;
    }
    side--;
    side &= 0xFF;

    if (D_80078394[side] != arg0) {
        changed = 1;
        D_80078394[side] = arg0;
    }

    D_800FCB18[side] = arg1;

    Audio_ResetChannelGroupState(side);

    D_800FCCC8[side ^ 1] = 0;

    D_800FCB28[side] = D_800FCB18[side]->unk_0C;
    D_800FCB48[side].unk_24++;
    D_800FCB48[side].unk_00[D_800FCB48[side].unk_24 & 0xF] = D_800FCB18[side]->unk_0C;

    value = D_800FCB18[side]->unk_0B;
    if ((value <= 0) || (value >= 0x98)) {
        return;
    }

    for (i = 0; i < D_8007839C[side]; i++) {
        if (D_800FCB38[side][i] == value) {
            goto found_value;
        }
    }

    D_800FCB38[side][D_8007839C[side]] = value;
    D_8007839C[side]++;
    if (D_8007839C[side] >= 8) {
        D_8007839C[side] = 7;
    }

    D_800FCB30[side] = 0;
    if (D_800783CC != 0) {
        Audio_QueueSequenceId(value + 0xCB);
        if (changed) {
            Audio_QueueSoundAvoidingRecent(D_80077FEC);
        }

        if (D_800FCB18[side ^ 1] == NULL) {
            return;
        }

        if (((f32)D_800FCB18[side ^ 1]->unk_26 - (f32)(D_800FCB18[side])->unk_26) > ((f32)(D_800FCB18[side])->unk_26 * 2.0f)) {
            D_800FCCB1 = 1;
            D_800FCCBA[side] = 0;
            return;
        }

        if (((f32)(D_800FCB18[side])->unk_26 * 0.3f) < ((f32)D_800FCB18[side ^ 1]->unk_26 - (f32)(D_800FCB18[side])->unk_26)) {
            D_800FCCB1 = 1;
            D_800FCCBA[side] = 0;
            return;
        }

        if (((f32)D_800FCB18[side ^ 1]->unk_26 * 0.3f) < ((f32)(D_800FCB18[side])->unk_26 - (f32)D_800FCB18[side ^ 1]->unk_26)) {
            D_800FCCB1 = 1;
            if ((D_800FCCD6[side] != 0) && (D_800FCCBA[side] != 0)) {
                if (changed == 0) {
                    Audio_QueueSoundAvoidingRecent(D_80078014);
                }
            } else if (changed == 0) {
                Audio_QueueSoundWithFallbackList(9, D_80077FF4, &D_800FCCBA[side]);
            } else {
                D_800FCCBA[side]++;
            }

            D_800FCCB8[side] = 0;
            D_800FCCD6[side] = 0;
            return;
        }

        if ((D_80078390[side] == 2) && (D_80078390[side ^ 1] == 3)) {
            if ((((u32)D_800FCB18[side ^ 1]->unk_0C * 100) / D_800FCB18[side ^ 1]->unk_28) < 0x32) {
                if (D_800783E0 == 0) {
                    if (changed == 0) {
                        Audio_QueueSequenceId(0xA);
                    }
                    D_800783E0 = 1;
                }
            }
        }
    }
    D_800FCCB8[side] = 0;
    D_800FCCBA[side] = 0;
    return;

found_value:
    if (D_800FCB30[side] == 0) {
        if (D_800783DC == 0) {
            if (D_80078390[side] < 2) {
                if ((osGetTime() % 2) == 0) {
                    Audio_QueueSequenceId(2);
                } else {
                    Audio_QueueSequenceId(1);
                }
            } else {
                Audio_QueueSequenceId(0x50);
                if (changed != 0) {
                    Audio_QueueSoundAvoidingRecent(D_80077FEC);
                }
            }
            D_800783DC = 1;
        } else {
            Audio_QueueSequenceId(0x58);
            D_800783DC = 0;
            if (changed != 0) {
                Audio_QueueSoundAvoidingRecent(D_80077FEC);
            }
        }
    } else if (changed != 0) {
        Audio_QueueSoundAvoidingRecent(D_80077FEC);
    }

    D_800FCB30[side] = 0;

    if (D_800FCB18[side ^ 1] == NULL) {
        return;
    }

    if (changed != 0) {
        return;
    }

    if ((D_800FCB18[side])->unk_15 & 0x20) {
        Audio_QueueSoundAvoidingRecent(D_80077FFC);
        return;
    }

    if ((D_800FCB18[side])->unk_15 & 7) {
        Audio_QueueSoundAvoidingRecent(D_80078004);
        return;
    }

    if ((D_800FCB18[side])->unk_15 & 8) {
        Audio_QueueSequenceId(6);
        return;
    }

    if ((D_800FCB18[side])->unk_15 & 0x10) {
        Audio_QueueSequenceId(7);
        return;
    }

    if ((((f32)(D_800FCB18[side])->unk_0C / (f32)(D_800FCB18[side])->unk_28) < 0.2f) && ((D_800FCB18[side])->unk_0C < D_800FCB18[side ^ 1]->unk_0C)) {
        Audio_QueueSoundAvoidingRecent(D_8007800C);
    }
}

void func_8003F1AC(u32 arg0) {
    u8 sp1F;

    if (arg0 == 0 || arg0 >= 5) {
        return;
    }
    arg0--;
    sp1F = D_80078398[arg0];
    if (sp1F <= 0 || sp1F >= 3) {
        return;
    }
    sp1F--;
    D_8007838C[arg0]--;
    D_80078390[sp1F]--;
    D_800FCB30[sp1F] = 0;
    if (D_800FCB18[0] == 0 || D_800FCB18[1] == 0) {
        return;
    }
    Audio_PlayCommand(0x16, D_80078400, 0);
    if (D_800783D8 != 0) {
        Audio_QueueSequenceId(0xC3);
        D_800783D8 = 0;
        D_800783E4 = 1;
        return;
    }
    D_800783D8 = 1;
    if (D_800783D4 != 0) {
        Audio_QueueSequenceId(0xB9);
        D_800783D4 = 0;
        return;
    }
    if (D_800783CC < 2) {
        if (osGetTime() % 2 == 0) {
            Audio_QueueSequenceId(0xBC);
        } else {                
            Audio_QueueSequenceId(0xC5);
        }
    } else if (D_800783D0 != 0) {
        Audio_QueueSequenceId(0xC4);
        D_800783D0 = 0;
    } else if ((D_800FCCA4 >= 0x64U) && (D_800FCCB2 == 0) && (D_800783EC == 0)) {
        if (((f32)D_800FCB18[sp1F]->unk_26 * (3.0 / 10.0)) < ((f32)D_800FCB18[sp1F ^ 1]->unk_26 - D_800FCB18[sp1F]->unk_26)) {
            Audio_QueueSoundAvoidingRecent(D_80077FC0);
        } else {
            Audio_QueueSoundAvoidingRecent(D_80077FD0);
        }
        D_800FCCD6[sp1F] = 1;
    } else if ((D_800FCB48[sp1F].unk_78 >= 8U) && (D_800FCCB2 == 0)) {
        if (((u32) D_800FCCA4 < 5U) && (D_800783E8 == 0)) {
            Audio_QueueSequenceId(0xC2);
        } else {
            Audio_QueueSoundAvoidingRecent(D_80077FC8);
        }
    } else {
        Audio_QueueSoundAvoidingRecent(D_80077FB4);
    }
}

void func_8003F4C0(u32 arg0) {
    u8 temp_v0;
    
    if (arg0 == 0 || arg0 >= 5) {
        return;
    }
    
    arg0--;
    temp_v0 = D_80078398[arg0];

    if (temp_v0 <= 0 || temp_v0 >= 3) {
        return;
    }
    
    temp_v0--;

    if (D_800FCB18[0] == 0 || D_800FCB18[1] == 0) {
        return;
    }

    if (D_800FCB18[temp_v0]->unk_0C <= 0) {
       return; 
    }
    
    D_800FCB30[temp_v0] = 1;
    D_800FCCB5 = 1;
    
    if (D_800783DC != 0) {
        Audio_QueueSequenceId(0x58);
        D_800783DC = 0;
        return;
    }
    
    D_800783DC = 1;
    
    if (D_800783CC < 2) {
        if (osGetTime() % 2 == 0) {
            Audio_QueueSequenceId(0x54);
        } else {
            Audio_QueueSequenceId(0x55);
        }
    } else if (D_800FCB48[temp_v0].unk_78 < 2) {
        Audio_QueueSoundAvoidingRecent(D_80077FE4);
    } else {
        Audio_QueueSoundAvoidingRecent(D_80077FD8);
    }
}

void Audio_QueuePendingSoundId(u32 arg0) {
    D_800FCCF0[D_80078544] = arg0;
    D_80078544++;
    if (D_80078544 >= 0xA) {
        D_80078544--;
    }
}

#ifdef NON_MATCHING
void func_8003F660(s32 arg0) {
    u32 mask;
    u32 sp40;
    s32 sp3C;
    u8 temp_s0;
    u8 sp3A;
    u8 sp39;
    u8 sp38;
    u8 sp37;
    u8 sp36;
    u8 pad1;
    u8 sp34;
    u8 pad2;
    u8 sp32;


    sp3A = 0;
    sp39 = 0;
    sp38 = 0;
    sp37 = 0;
    sp36 = 0;
    sp34 = 0;
    sp32 = 0;
    mask = osSetIntMask(1);
    sp3C = D_800FCCA2;
    D_800FCCA2 = 0;
    osSetIntMask(mask);
    D_800783D0 = 0;
    D_800FCCB2 = 0;
    if (arg0 == 0 || arg0 >= 5U) {
        return;
    }
    arg0--;
    temp_s0 = D_80078398[arg0];
    if (temp_s0 <= 0 || temp_s0 >= 3) {
        return;
    }
    temp_s0--;
    if (D_800FCB18[0] == 0 || D_800FCB18[1] == 0) {
        return;
    }
    for (sp40 = 0; sp40 < D_80078544; sp40++) {
        switch (D_800FCCF0[sp40]) {
        case 0x1D:
            sp3A = 1;
            D_800783A0 = 1;
            break;
        case 0x1E:
            sp37 = 1;
            D_800783A0 = 1;
            break;
        case 0x29:
            sp39 = 1;
            D_800783A0 = 1;
            break;
        case 0x2A:
            sp38 = 1;
            D_800783A0 = 1;
            break;
        case 0x66:
            sp36 = 1;
            break;
        case 0x6F:
            sp34 = 1;
            break;
        case 0x6B:
        case 0x6C:
            D_800FCB48[temp_s0].unk_24++;
            ((u16*)D_800FCB48[temp_s0].unk_00)[D_800FCB48[temp_s0].unk_24 & 0xF] = D_800FCB18[temp_s0]->unk_0C;
            mask = osSetIntMask(1);
            D_800FCCA2 = (u8) sp3C;
            osSetIntMask(mask);
            break;
        case 0x6E:
        case 0x70:
        case 0x9E:
        case 0xAB:
            mask = osSetIntMask(1);
            D_800FCCA2 = (u8) sp3C;
            osSetIntMask(mask);
            break;
        case 0x0:
        case 0x1:
        case 0x2:
            D_800FCCB2 = 1;
            D_800783EC = 1;
            break;
        case 0x9D:
            D_800783D0 = 1;
            D_800FCCB2 = 1;
            break;
        case 0x6D:
            Audio_QueueSequenceId(0x165);
            D_80078544 = 0;
            D_800FCCBE[temp_s0] = 0;
            Audio_PlayCommand(0x16, D_80078400, 0);
            return;
        case 0x6:
            if (D_800FCB48[temp_s0].unk_7C == 1) {
                Audio_QueueSequenceId(0x19F);
            } else if (D_800FCB48[temp_s0].unk_7C >= 4U) {
                Audio_QueueSequenceId(0x164);
            } else {
                Audio_QueueSequenceId(0x163);
            }
            D_80078544 = 0;
            D_800FCB48[temp_s0].unk_7C = 0;
            D_800FCCBC[temp_s0] = 0;
            D_800FCCB4 = 0;
            Audio_PlayCommand(0x16, D_80078400, 0);
            return;
        case 0xB:
            Audio_QueueSequenceId(0x17B);
            D_80078544 = 0;
            return;
        case 0xC:
            Audio_QueueSequenceId(0x186);
            D_800FCCC4[temp_s0] = 0;
            D_80078544 = 0;
            return;
        case 0x11:
            Audio_QueueSequenceId(0x192);
            D_800FCCD4[temp_s0] = 0;
            D_80078544 = 0;
            return;
        case 0x5:
            if ((D_80078390[0] == 1) && (D_80078390[1] == 1)) {
                Audio_QueueSoundWithFallbackList(0x17D, &D_80078040, &D_800FCCBC[temp_s0]);
            } else {
                Audio_QueueSoundWithFallbackList(0x17D, &D_8007801C, &D_800FCCBC[temp_s0]);
            }
            D_80078544 = 0;
            return;
        case 0x7:
            if ((D_80078390[0] == 1) && (D_80078390[1] == 1)) {
                Audio_QueueSoundWithFallbackList(0x17E, &D_80078038, &D_800FCCBE[temp_s0]);
            } else {
                Audio_QueueSoundWithFallbackList(0x17E, &D_80078024, &D_800FCCBE[temp_s0]);
            }
            D_80078544 = 0;
            return;
        case 0x10:
            if ((D_80078390[0] == 1) && (D_80078390[1] == 1)) {
                Audio_QueueSoundWithFallbackList(0x17F, &D_80078040, &D_800FCCC0[temp_s0]);
            } else {
                Audio_QueueSoundWithFallbackList(0x17F, &D_80078030, &D_800FCCC0[temp_s0]);
            }
            D_80078544 = 0;
            return;
        case 0x8:
            if ((D_80078390[0] == 1) && (D_80078390[1] == 1)) {
                Audio_QueueSoundWithFallbackList(0x17C, &D_80078040, &D_800FCCC2[temp_s0]);
            } else {
                Audio_QueueSoundWithFallbackList(0x17C, &D_80078048, &D_800FCCC2[temp_s0]);
            }
            D_80078544 = 0;
            return;
        case 0x9:
        case 0xA:
            if ((D_80078390[0] == 1) && (D_80078390[1] == 1)) {
                Audio_QueueSequenceId(0x180);
            } else {
                Audio_QueueSequenceId(0x17C);
            }
            D_80078544 = 0;
            return;
        case 0x13:
            Audio_QueueSoundAvoidingRecent(&D_80078050);
            D_800FCCB6 = 1;
            D_80078544 = 0;
            return;
        case 0xF:
            Audio_QueueSequenceId(0x17A);
            D_80078544 = 0;
            return;
        case 0xE:
            D_800783D0 = 1;
            if (D_800FCCC4[temp_s0] != 0) {
                Audio_QueueSoundAvoidingRecent(&D_80078060);
            } else {
                Audio_QueueSoundAvoidingRecent(&D_80078058);
                D_800FCCC4[temp_s0] = 1;
            }
            D_80078544 = 0;
            Audio_PlayCommand(0x16, D_80078400, 0);
            return;
        case 0x12:
            if (((unk_D_800FCB48*)&D_800FCCCA[temp_s0])->unk_00[0] == 0) {
                Audio_QueueSequenceId(0x18C);
            } else {
                Audio_QueueSequenceId(0x18E);
            }
            ((unk_D_800FCB48*)&D_800FCCCA[temp_s0])->unk_00[0] ^= 1;
            D_80078544 = 0;
            Audio_PlayCommand(0x16, D_80078400, 0);
            return;
        case 0x71:
        case 0x77:
            Audio_QueueSoundWithFallbackList(0x16A, &D_80078068, &D_800FCCC6[temp_s0]);
            D_80078544 = 0;
            return;
        case 0x72:
        case 0x78:
            Audio_QueueSoundWithFallbackList(0x16B, &D_80078068, &D_800FCCC6[temp_s0]);
            D_80078544 = 0;
            return;
        case 0x73:
        case 0x79:
            Audio_QueueSoundWithFallbackList(0x16C, &D_80078068, &D_800FCCC6[temp_s0]);
            D_80078544 = 0;
            return;
        case 0x74:
        case 0x7A:
            Audio_QueueSoundWithFallbackList(0x16D, &D_80078068, &D_800FCCC6[temp_s0]);
            D_80078544 = 0;
            return;
        case 0x75:
        case 0x7B:
            Audio_QueueSoundWithFallbackList(0x16E, &D_80078068, &D_800FCCC6[temp_s0]);
            D_80078544 = 0;
            return;
        case 0x76:
        case 0x7C:
            Audio_QueueSoundWithFallbackList(0x16F, &D_80078068, &D_800FCCC6[temp_s0]);
            D_80078544 = 0;
            return;
        case 0xB5:
            Audio_QueueSoundWithFallbackList(0x170, &D_80078068, &D_800FCCC6[temp_s0]);
            D_80078544 = 0;
            return;
        case 0xB7:
            Audio_QueueSoundWithFallbackList(0x171, &D_80078068, &D_800FCCC6[temp_s0]);
            D_80078544 = 0;
            return;
        case 0x9A:
            Audio_QueueSoundWithFallbackList(0x169, &D_80078068, &D_800FCCC6[temp_s0]);
            D_80078544 = 0;
            return;
        case 0x9C:
            Audio_QueueSoundWithFallbackList(0x194, &D_80078068, &D_800FCCC6[temp_s0]);
            D_80078544 = 0;
            return;
        case 0xAE:
            Audio_QueueSequenceId(0x1A0);
            D_80078544 = 0;
            return;
        case 0x7E:
        case 0x84:
            if (D_800FCCC8[temp_s0] < 3) {
                Audio_QueueSoundWithFallbackList(0xA2, &D_80078070, &D_800FCCC8[temp_s0]);
            } else {
                Audio_QueueSoundAvoidingRecent(&D_80078078);
            }
            D_80078544 = 0;
            return;
        case 0x7F:
        case 0x85:
            if (D_800FCCC8[temp_s0] < 3) {
                Audio_QueueSoundWithFallbackList(0xA3, &D_80078070, &D_800FCCC8[temp_s0]);
            } else {
                Audio_QueueSoundAvoidingRecent(&D_80078078);
            }
            D_80078544 = 0;
            return;
        case 0x80:
        case 0x86:
            if (D_800FCCC8[temp_s0] < 3) {
                Audio_QueueSoundWithFallbackList(0xA7, &D_80078070, &D_800FCCC8[temp_s0]);
            } else {
                Audio_QueueSoundAvoidingRecent(&D_80078078);
            }
            D_80078544 = 0;
            return;
        case 0x81:
        case 0x87:
            if (D_800FCCC8[temp_s0] < 3) {
                Audio_QueueSoundWithFallbackList(0xA4, &D_80078070, &D_800FCCC8[temp_s0]);
            } else {
                Audio_QueueSoundAvoidingRecent(&D_80078078);
            }
            D_80078544 = 0;
            return;
        case 0x82:
        case 0x88:
            if (D_800FCCC8[temp_s0] < 3) {
                Audio_QueueSoundWithFallbackList(0xA5, &D_80078070, &D_800FCCC8[temp_s0]);
            } else {
                Audio_QueueSoundAvoidingRecent(&D_80078078);
            }
            D_80078544 = 0;
            return;
        case 0x83:
        case 0x89:
            if (D_800FCCC8[temp_s0] < 3) {
                Audio_QueueSoundWithFallbackList(0xA6, &D_80078070, &D_800FCCC8[temp_s0]);
            } else {
                Audio_QueueSoundAvoidingRecent(&D_80078078);
            }
            D_80078544 = 0;
            return;
        case 0x19:
        case 0x1B:
            if ((D_800780B3[D_800FCCAC * 3] == 0) && ((D_800FCB18[temp_s0]->unk_16[6] == 8) || (D_800FCB18[temp_s0]->unk_16[7] == 8))) {
                Audio_QueueSoundAvoidingRecent(&D_80077F24);
            } else {
                Audio_QueueSoundAvoidingRecent(&D_80077F14);
            }
            D_80078544 = 0;
            return;
        case 0x1A:
        case 0x1C:
            if (D_800FCCD2[temp_s0 ^ 1] != 0) {
                Audio_QueueSoundAvoidingRecent(&D_80077F50);
            } else {
                if ((D_800783D4 != 0) || ((D_80078390[temp_s0 ^ 1] == 1) && ((s32)D_800FCB18[temp_s0 ^ 1]->unk_0C < 0x13))) {
                    Audio_QueueSoundAvoidingRecent(&D_80077F1C);
                    Audio_PlayCommand(0x16, D_80078400, 0);
                } else {
                    Audio_QueueSoundAvoidingRecent(&D_80077F0C);
                }
                D_800FCCD2[temp_s0 ^ 1] = 1;
            }
            D_80078544 = 0;
            return;
        case 0xA7:
            Audio_QueueSoundAvoidingRecent(&D_80077F0C);
            D_80078544 = 0;
            return;
        case 0x20:
        case 0x7D:
        case 0x8C:
        case 0x99:
        case 0x9B:
        case 0xA5:
        case 0xAD:
        case 0xB2:
        case 0xB4:
        case 0xB6:
        case 0xB8:
        case 0xB9:
        case 0xBA:
        case 0xBC:
            Audio_QueueSoundAvoidingRecent(&D_80078088);
            D_80078544 = 0;
            return;
        case 0x64:
        case 0x65:
        case 0x67:
        case 0x6A:
        case 0x8A:
        case 0x8B:
        case 0x8D:
        case 0x91:
        case 0x92:
        case 0xA0:
        case 0xA2:
        case 0xA3:
        case 0xAA:
            Audio_QueueSoundAvoidingRecent(&D_80078090);
            D_80078544 = 0;
            return;
        case 0x1F:
            Audio_QueueSequenceId(0x18D);
            D_80078544 = 0;
            return;
        case 0x93:
        case 0x94:
        case 0x95:
        case 0x96:
            Audio_QueueSequenceId(0x18F);
            D_80078544 = 0;
            return;
        case 0x97:
            Audio_QueueSoundWithFallbackList(0x18A, &D_80078098, &D_800FCCCE[temp_s0]);
            D_80078544 = 0;
            Audio_PlayCommand(0x16, D_80078400, 0);
            return;
        case 0x98:
            Audio_QueueSoundWithFallbackList(0x18B, &D_800780A0, &D_800FCCD0[temp_s0]);
            D_80078544 = 0;
            Audio_PlayCommand(0x16, D_80078400, 0);
            return;
        case 0x2B:
            Audio_QueueSequenceId(0x76);
            D_80078544 = 0;
            return;
        case 0x2C:
            Audio_QueueSoundAvoidingRecent(&D_80078080);
            D_80078544 = 0;
            return;
        case 0xBB:
            Audio_QueueSequenceId(0x189);
            D_80078544 = 0;
            Audio_PlayCommand(0x16, D_80078400, 0);
            D_800783D0 = 1;
            return;
        case 0x68:
            Audio_QueueSoundAvoidingRecent(&D_80077F34);
            D_80078544 = 0;
            return;
        case 0x69:
            Audio_QueueSoundAvoidingRecent(&D_80077F3C);
            D_80078544 = 0;
            return;
        case 0xA1:
            Audio_QueueSequenceId(0x7A);
            D_80078544 = 0;
            return;
        case 0xA6:
            Audio_QueueSoundAvoidingRecent(&D_80077F2C);
            D_80078544 = 0;
            return;
        case 0xAF:
        case 0xB0:
            Audio_QueueSequenceId(0x167);
            D_800FCB48[temp_s0].unk_24++;
            ((u16*)D_800FCB48[temp_s0].unk_00)[D_800FCB48[temp_s0].unk_24 & 0xF] = ((BattleMonRuntime*)D_800FCB18[temp_s0])->unk_0C;
            D_80078544 = 0;
            return;
        case 0xB1:
            Audio_QueueSequenceId(0x166);
            D_800FCB48[temp_s0].unk_24++;
            ((u16*)D_800FCB48[temp_s0].unk_00)[D_800FCB48[temp_s0].unk_24 & 0xF] = ((BattleMonRuntime*)D_800FCB18[temp_s0])->unk_0C;
            D_80078544 = 0;
            return;
        case 0xB3:
            Audio_QueueSequenceId(0x188);
            D_80078544 = 0;
            Audio_PlayCommand(0x16, D_80078400, 0);
            return;
        case 0xA8:
            Audio_QueueSequenceId(0x197);
            D_80078544 = 0;
            return;
        case 0xA9:
            Audio_QueueSequenceId(0x193);
            D_80078544 = 0;
            return;
        case 0xA4:
            Audio_QueueSequenceId(0x195);
            D_80078544 = 0;
            return;
        case 0xAC:
            Audio_QueueSequenceId(0x196);
            D_80078544 = 0;
            return;
        case 0x9F:
            Audio_QueueSequenceId(0x87);
            D_80078544 = 0;
            return;
        case 0x4:
            sp32 = 1;
            break;
        default: 
            break;
        }
    }
    D_80078544 = 0;
    
    if (sp34 != 0) {
        mask = osSetIntMask(1);
        D_800FCCA2 = 0;
        osSetIntMask(mask);
        Audio_QueueSequenceId(0x79);
        D_800FCCB3 = 1;
        Audio_PlayCommand(0x16, D_80078400, 0);
    } else if (sp36 != 0) {
        mask = osSetIntMask(1);
        D_800FCCA2 = 0;
        osSetIntMask(mask);
        if (D_800FCCCC[temp_s0] != 0) {
            Audio_QueueSequenceId(0xAD);
        } else {
            Audio_QueueSequenceId(0x78);
        }
        D_800FCB48[temp_s0].unk_7C = 1;
        D_800FCCCC[temp_s0] = 1;
        D_800FCCB4 = 1;
        Audio_PlayCommand(0x16, D_80078400, 0);
    } else if (sp37 != 0) {
        mask = osSetIntMask(1);
        D_800FCCA2 = 0;
        osSetIntMask(mask);
        D_800783E8 = 1;
        Audio_QueueSequenceId(0x77);
        Audio_PlayCommand(0x16, D_80078400, 0);
    } else if (sp3A != 0) {
        mask = osSetIntMask(1);
        D_800FCCA2 = 0;
        osSetIntMask(mask);
        D_800783E8 = 1;
        if (sp39 != 0) {
            if ((u32) D_800FCCA4 < 0xAU) {
                if ((D_800FCB18[temp_s0]->unk_0C > 0) && ((s32)D_800FCB18[temp_s0 ^ 1]->unk_0C >= 0xB)) {
                    Audio_QueueSoundAvoidingRecent(&D_80077EF0);
                } else {
                    Audio_QueueSoundAvoidingRecent(&D_80077F04);
                }
            } else if ((u32) D_800783CC < 2U) {
                Audio_QueueSequenceId(0x68);
                Audio_PlayCommand(0x16, D_80078400, 0);
            } else {
                if ((D_800FCB48[temp_s0 ^ 1].unk_78 < 2U) && (D_80078390[temp_s0 ^ 1] >= 2)) {
                    Audio_QueueSoundAvoidingRecent(&D_80077F58);
                } else {
                    Audio_QueueSoundAvoidingRecent(&D_80077EE4);
                }
                Audio_PlayCommand(0x16, D_80078400, 0);
            }
        } else if ((u32) D_800FCCA4 < 0xAU) {
            if (((s32)D_800FCB18[temp_s0]->unk_0C > 0) && ((s32)D_800FCB18[temp_s0 ^ 1]->unk_0C >= 0xB)) {
                Audio_QueueSoundAvoidingRecent(&D_80077EF0);
            } else {
                Audio_QueueSoundAvoidingRecent(&D_80077F04);
            }
        } else {
            Audio_QueueSoundAvoidingRecent(&D_80077EC0);
            Audio_PlayCommand(0x16, D_80078400, 0);
        }
    } else if (sp39 != 0) {
        mask = osSetIntMask(1);
        D_800FCCA2 = 0;
        osSetIntMask(mask);
        D_800783E8 = 1;
        if ((D_800FCB48[temp_s0 ^ 1].unk_78 < 2U) && (D_80078390[temp_s0 ^ 1] >= 2)) {
            Audio_QueueSoundAvoidingRecent(&D_80077F58);
        } else {
            Audio_QueueSoundAvoidingRecent(&D_80077ECC);
        }
        Audio_PlayCommand(0x16, D_80078400, 0);
    } else if (sp38 != 0) {
        mask = osSetIntMask(1);
        D_800FCCA2 = 0;
        osSetIntMask(mask);
        if ((u32) D_800FCCA4 >= 0x29U) {
            Audio_QueueSoundAvoidingRecent(&D_80077EF8);
            Audio_PlayCommand(0x16, D_80078400, 0);
        } else if ((D_800FCB18[0]->unk_0C > 0) && (D_800FCB18[1]->unk_0C > 0)) {
            Audio_QueueSoundAvoidingRecent(&D_80077ED8);
        } else {
            Audio_QueueSoundAvoidingRecent(&D_80077F04);
            Audio_PlayCommand(0x16, D_80078400, 0);
        }
    } else if ((sp32 != 0) && (D_800FCCA4 != 0)) {
        if (D_800FCCA4 >= 0x29) {
            Audio_QueueSoundAvoidingRecent(&D_80077E98);
            Audio_PlayCommand(0x16, D_80078400, 0);
        }else if (D_800FCCA4 >= 6) {
            if (( D_800FCB18[0]->unk_0C > 0) && ((s32) D_800FCB18[1]->unk_0C > 0)) {
                Audio_QueueSoundAvoidingRecent(&D_80077EA4);
            } else {
                Audio_QueueSoundAvoidingRecent(&D_80077F60);
            }
        }else if (((s32) D_800FCB18[0]->unk_0C > 0) && ((s32) D_800FCB18[1]->unk_0C > 0)) {
            Audio_QueueSoundAvoidingRecent(&D_80077EB4);
        } else {
            Audio_QueueSoundAvoidingRecent(&D_80077F04);
        }
    }
}
#else
#pragma GLOBAL_ASM("asm/us/nonmatchings/3D140/func_8003F660.s")
#endif

void func_80040A70(BattleMonRuntime* arg0, s32 arg1, s32 arg2, s32 arg3) {
    s32 i;
    s32 sp40;
    u32 temp_lo;
    u32 temp_v0;
    s32 sp34;
    u32 tmp;
    OSIntMask sp2C;

    if (arg0 == NULL) {
        return;
    }

    if (arg0 == D_800FCB18[0]) {
        sp40 = 0;
    } else if (arg0 == D_800FCB18[1]) {
        sp40 = 1;
    } else {
        return;
    }

    if ((D_800FCB18[0] == NULL) || (D_800FCB18[1] == NULL)) {
        return;
    }

    D_800FCCAC = arg1;

    switch (arg3) {
        case 1:
            D_800FCCB7 = 0;
            Audio_RemoveLastQueuedSequenceId();
            sp40 ^= 1;

            if (D_800FCB18[sp40]->unk_0C != 0) {
                for (i = 0; i < 5; i++) {
                    // FAKE
                    if (arg2) {}
                    if (arg1 == D_80078548[i].unk_00) {
                        if (D_800FCB18[sp40]->unk_0C < D_800FCB48[sp40].unk_00[D_800FCB48[sp40].unk_24 & 0xF]) {
                            tmp = (D_800FCB48[sp40].unk_00[D_800FCB48[sp40].unk_24 & 0xF] - D_800FCB18[sp40]->unk_0C);
                            D_800FCCA4 = (tmp * 0x64) / D_800FCB18[sp40]->unk_28;
                        } else {
                            D_800FCCA4 = 0;
                        }
                        D_800FCCAD = arg2;
                        return;
                    }

                    if (arg1 == D_80078548[i].unk_01) {
                        if (D_800FCB18[sp40]->unk_0C < D_800FCB48[sp40].unk_00[D_800FCB48[sp40].unk_24 & 0xF]) {
                            tmp = D_800FCB48[sp40].unk_00[D_800FCB48[sp40].unk_24 & 0xF] - D_800FCB18[sp40]->unk_0C;
                            D_800FCCA4 = (tmp * 0x64) / D_800FCB18[sp40]->unk_28;
                        } else {
                            D_800FCCA4 = 0;
                        }
                        D_800FCCAD = arg2;
                        return;
                    }
                }
            }

            temp_v0 = osSetIntMask(sp2C);

            D_800FCCA2 = 0x1E;
            if (D_800FCB18[sp40]->unk_0C < D_800FCB48[sp40].unk_00[D_800FCB48[sp40].unk_24 & 0xF]) {
                tmp = D_800FCB48[sp40].unk_00[D_800FCB48[sp40].unk_24 & 0xF] - D_800FCB18[sp40]->unk_0C;
                D_800FCCA4 = (tmp * 0x64) / D_800FCB18[sp40]->unk_28;
            } else {
                D_800FCCA4 = 0;
            }
            D_800FCCAD = arg2;

            osSetIntMask(temp_v0);

            if (D_800FCCA4 >= 0x29) {
                Audio_PlayCommand(0x16, D_80078400, 0);
            }
            break;

        case 0:
        case 2:
            Audio_RemoveLastQueuedSequenceId();
            D_800FCCC0[sp40] = 0;
            D_800FCCC2[sp40] = 0;

            if (arg1 != 0x13) {
                D_800FCCCE[sp40] = 0;
            }

            if (arg1 != 0x5B) {
                D_800FCCD0[sp40] = 0;
            }

            if (arg3 != 2) {
                D_800FCCD2[sp40] = 0;
            }

            D_800FCCAE += 1;

            for (i = 0; i < 17; i++) {
                if (arg1 == D_80078554[i]) {
                    goto loop1_end;
                }
            }

            D_800FCCC6[sp40] = 0;
        loop1_end:

            for (i = 0; i < 9; i++) {
                if (arg1 == D_80078568[i]) {
                    goto loop2_end;
                }
            }

            D_800FCCC8[sp40] = 0;
        loop2_end:

            D_800FCCA2 = 0;
            if (D_800FCCB7 == 0) {
                tmp = func_80042158(arg1, D_800FCB18[sp40], D_800FCB18[sp40 ^ 1]);

                if (tmp == -1) {
                    if (BitArray_IsBitSet(D_800FCCD8, arg1 - 1) == 0) {
                        sp34 = arg1 + 0x1A2;
                        BitArray_SetBit(D_800FCCD8, arg1 - 1, 1);
                        Audio_QueueSequenceId(sp34);
                    } else {
                        Audio_QueueSoundAvoidingRecent(D_80077F6C);
                    }
                } else if (tmp == -2) {
                    if (BitArray_IsBitSet(D_800FCCD8, arg1 - 1) == 0) {
                        sp34 = arg1 + 0x1A2;
                        BitArray_SetBit(D_800FCCD8, arg1 - 1, 1);
                        Audio_QueueSequenceId(sp34);
                    } else {
                        Audio_QueueSoundAvoidingRecent(D_80077F74);
                    }
                } else if (tmp == -3) {
                    D_800783D4 = 1;
                    if (BitArray_IsBitSet(D_800FCCD8, arg1 - 1) == 0) {
                        sp34 = arg1 + 0x1A2;
                        BitArray_SetBit(D_800FCCD8, arg1 - 1, 1);
                        Audio_QueueSequenceId(sp34);
                    } else {
                        Audio_QueueSequenceId(0x187);
                    }
                } else if (tmp == -4) {
                    if (BitArray_IsBitSet(D_800FCCD8, arg1 - 1) == 0) {
                        sp34 = arg1 + 0x1A2;
                        BitArray_SetBit(D_800FCCD8, arg1 - 1, 1);
                        Audio_QueueSequenceId(sp34);
                    } else {
                        Audio_QueueSoundAvoidingRecent(D_80077F98);
                    }
                } else if (tmp != -5) {
                    if ((D_800FCB48[sp40 ^ 1].unk_00[D_800FCB48[sp40 ^ 1].unk_24 & 0xF] < tmp) && (D_800FCCB6 == 0)) {
                        if (BitArray_IsBitSet(D_800FCCD8, arg1 - 1) == 0) {
                            sp34 = arg1 + 0x1A2;
                            BitArray_SetBit(D_800FCCD8, arg1 - 1, 1);
                            Audio_QueueSequenceId(sp34);
                        } else {
                            Audio_QueueSoundAvoidingRecent(D_80077F90);
                            D_800FCCB6 = 0;
                        }
                    } else {
                        D_800FCCB6 = 0;
                        temp_lo = (tmp * 0x64) / D_800FCB18[sp40 ^ 1]->unk_28;
                        if (temp_lo >= 0x51) {
                            if (D_800FCCAF == 0) {
                                D_800FCCAF = 1;
                                Audio_QueueSoundAvoidingRecent(D_80077F6C);
                            } else {
                                Audio_QueueSoundAvoidingRecent(D_80077F74);
                            }
                        } else if (temp_lo >= 0x33) {
                            if (D_800783CC == 1) {
                                Audio_QueueSoundAvoidingRecent(D_80077F88);
                            } else {
                                Audio_QueueSoundAvoidingRecent(D_80077F7C);
                            }
                        } else if (AudioStream_IsPlaying(1) == 0) {
                            if (D_800783CC == 1) {
                                if (D_800FCCAE == 2) {
                                    Audio_QueueSoundAvoidingRecent(D_80077FAC);
                                } else {
                                    Audio_QueueSoundAvoidingRecent(D_80077FA4);
                                }
                            } else {
                                Audio_QueueSoundAvoidingRecent(D_80077F98);
                            }
                        }
                    }
                }
            }
            break;

        case 3:
            if (D_800FCCB7 == 0) {
                D_800FCCB7 = 1;
                D_800FCCA2 = 0;
                tmp = func_80042158(arg1, D_800FCB18[sp40], D_800FCB18[sp40 ^ 1]);
                if ((tmp >> 0x10) == 0) {
                    tmp = tmp << 1;

                    if (D_800FCB48[sp40 ^ 1].unk_00[D_800FCB48[sp40 ^ 1].unk_24 & 0xF] < tmp) {
                        if (BitArray_IsBitSet(D_800FCCD8, arg1 - 1) == 0) {
                            sp34 = arg1 + 0x1A2;
                            BitArray_SetBit(D_800FCCD8, arg1 - 1, 1);
                            Audio_QueueSequenceId(sp34);
                        } else {
                            Audio_QueueSoundAvoidingRecent(D_80077F90);
                        }
                    } else {
                        temp_lo = (tmp * 0x64u) / D_800FCB18[sp40 ^ 1]->unk_28;
                        if (temp_lo >= 0x51) {
                            if (D_800FCCAF == 0) {
                                D_800FCCAF = 1;
                                Audio_QueueSoundAvoidingRecent(D_80077F6C);
                            } else {
                                Audio_QueueSoundAvoidingRecent(D_80077F74);
                            }
                        } else if (temp_lo >= 0x33) {
                            if (D_800783CC == 1) {
                                Audio_QueueSoundAvoidingRecent(D_80077F88);
                            } else {
                                Audio_QueueSoundAvoidingRecent(D_80077F7C);
                            }
                        } else if (AudioStream_IsPlaying(1) == 0) {
                            if (D_800783CC == 1) {
                                if (D_800FCCAE == 2) {
                                    Audio_QueueSoundAvoidingRecent(D_80077FAC);
                                } else {
                                    Audio_QueueSoundAvoidingRecent(D_80077FA4);
                                }
                            } else {
                                Audio_QueueSoundAvoidingRecent(D_80077F98);
                            }
                        }
                    }
                }
            }
            break;

        case 4:
            D_800FCCAE += 1;
            D_800FCCB7 = 0;
            D_800FCCA2 = 0;
            break;

        case 5:
            D_800FCCB7 = 0;
            D_800FCCA2 = 0;
            break;

        default:
            D_800FCCB7 = 0;
            D_800FCCA2 = 0;
            break;
    }
}

void func_800414B8(s32 arg0) {
    u8 temp_v0;

    if (arg0 == 0 || arg0 >= 5U) {
        return;
    }
    arg0--;
    temp_v0 = D_80078398[arg0];
    if (temp_v0 <= 0 || temp_v0 >= 3) {
        return;
    }
    temp_v0--;
    if (D_800FCCD4[temp_v0] == 0) {
        Audio_QueueSequenceId(0x190);
        D_800FCCD4[temp_v0] = 1;
    } else {
        Audio_QueueSequenceId(0x191);
    }
}

void Audio_StartBattleMusicSequence(void) {
    if (D_80078E80 == 0) {
        Audio_StartMusicTrack(0x3D);
        Audio_PlayCommand(0x16, D_80078400, 0);
        if (D_80078404 == 0xC) {
            if (!(D_80078408 & 3) && (*D_80078390 != 0)) {
                Audio_QueueSequenceId(0xCA);
                return;
            }
            Audio_QueueSequenceId(0xC8);
            return;
        }
        if (D_80078404 == 0xD) {
            if ((D_80078408 == 4) && (*D_80078390 != 0)) {
                Audio_QueueSequenceId(0xCA);
                return;
            }
            if ((D_80078408 == 5) && (*D_80078390 != 0)) {
                Audio_QueueSequenceId(0xCB);
                return;
            }
            Audio_QueueSequenceId(0xC8);
            return;
        }
        if ((D_80078408 == 8) && (*D_80078390 != 0)) {
            Audio_QueueSequenceId(0xC9);
            return;
        }
        Audio_QueueSequenceId(0xC8);
    }
}

void Audio_StartResultMusicSequence(void) {
    Audio_PlayCommand(0x16, D_80078400, 0);
    Audio_QueueSequenceId(0x1A2);
}

void Audio_UpdateBattleAudioFrame(void) {
    u8* seq;

    if ((D_800783B4 < D_800783B8) && (D_80078388 != 0) && (AudioStream_IsPlaying(1) == 0)) {
        if (D_800783A4[D_800783B4 & 3] != 0) {
            seq = D_800FC714->seqArray[D_800783A4[D_800783B4 & 3] - 1].offset;
            AudioStream_Start(1, seq, 0x7E, 0);
        }
        D_800783B4++;
    }

    if (D_800783BC != 0) {
        if ((D_800783C8 >= 0x3E9) && (D_800783C0 == 0) && (D_800783CC != 0)) {
            Audio_QueueSequenceId(0x248);
            D_800783C0 = 1;
        } else if ((D_800783C8 >= 0x7D1) && (D_800783C0 == 0) && (D_800783CC == 0)) {
            Audio_QueueSequenceId(0x248);
            D_800783C0 = 1;
            D_800783C8 = 0x3E8;
        } else if ((D_800783C8 >= 0x641) && (D_800783C0 == 1)) {
            Audio_QueueSequenceId(0x249);
            if (D_800783C4 == 0) {
                Audio_PlayCommand(0x5B, D_80078400, 0);
                D_800783C4 = 1;
            }
            D_800783C0 = 2;
        } else if ((D_800783C8 >= 0x899) && (D_800783C0 == 2)) {
            Audio_QueueSequenceId(0x24A);
            D_800783C8 = 0x3E8;
            D_800783C0 = 1;
        }

        D_800783C8++;
    }

    if (D_800FCCA2 != 0) {
        if (D_800FCCA2 == 1) {
            if (D_800FCCA4 != 0) {
                if (D_800FCCA4 >= 0x29) {
                    if (((D_800FCCAD == 0x10) || (D_800FCCAD == 0x11) || (D_800FCCAD == 0x12) || (D_800FCCAD == 0x15) ||
                         (D_800FCCAD == 0x16) || (D_800FCCAD == 0x53) || (D_800FCCAD == 0x54) ||
                         (D_800FCCAD == 0x55)) &&
                        ((D_800FCCAC == 0x34) || (D_800FCCAC == 0x35) || (D_800FCCAC == 0x7E))) {
                        Audio_QueueSoundAvoidingRecent(&D_80077F44);
                    } else {
                        Audio_QueueSoundAvoidingRecent(&D_80077E98);
                    }
                } else if (D_800FCCA4 >= 6) {
                    if ((D_800FCB18[0]->unk_0C > 0) && (D_800FCB18[1]->unk_0C > 0)) {
                        Audio_QueueSoundAvoidingRecent(&D_80077EA4);
                    } else {
                        Audio_QueueSoundAvoidingRecent(&D_80077F60);
                    }
                } else if ((D_800FCB18[0]->unk_0C > 0) && (D_800FCB18[1]->unk_0C > 0)) {
                    Audio_QueueSoundAvoidingRecent(&D_80077EB4);
                } else {
                    Audio_QueueSoundAvoidingRecent(&D_80077F04);
                }
            }
            D_800FCCA8 = D_800FCCA4;
        }
        D_800FCCA2--;
    }
}

/**
 * Everything below is byte-exact against the target except ONE instruction.
 *
 * The target materialises &D_800FCCA1 into a general register
 * (`lui $a0, %hi; addiu $a0, $a0, %lo; sb $zero, 0($a0)`) instead of the
 * `lui $at, %hi; sb $zero, %lo($at)` form IDO uses for every other scalar
 * store in this function. IDO only picks the register form when the address
 * survives CSE, i.e. when the C references that lvalue at least twice --
 * exactly why the neighbouring `D_8007839C[0]`/`D_8007839C[1]` pair keeps its
 * base in $v1 (drop either store and it reverts to the $at form).
 *
 * A read-modify-write whose read IDO folds away late (`D_800FCCA1 *= 0;`,
 * `D_800FCCA1 %= 1;`, `D_800FCCA1 = D_800FCCA1 * 0;`) reproduces the target
 * word-for-word, register-for-register -- but none of those is plausible
 * source, so the real shape of this one statement is still unknown. Things
 * that do NOT produce it: array/struct/union wrappers with a constant index,
 * casts through `s8*`, a local pointer, `static`, `register`, bitfields,
 * `&= 0` / `-= x` / `^= x` (folded too early), and one- or two-trip loops.
 * `volatile` does force a register, but IDO then allocates $t6 and schedules
 * the `addiu` after the two D_8007839C stores, which is not the target.
 */
#ifdef NON_MATCHING
void func_80041A98(void) {
    s32 i;
    s32 j;

    D_800783CC = 0;
    for (i = 0; i < 2; i++) {
        for (j = 0; j < 8; j++) {
            D_800FCB38[i][j] = 0;
        }
    }
    D_8007839C[0] = 0;
    D_8007839C[1] = 0;
    D_800FCCA1 = 0;
    D_800FCCA0 = 0;
    D_800FCCA2 = 0;
    D_800FCCA4 = 0;
    D_800FCCAC = 0;
    D_800783E0 = 0;
    D_800FCCAF = 0;
    D_800FCCB0 = 0;
    D_800FCCB2 = 0;
    D_800FCCB3 = 0;
    D_800FCCB4 = 0;
    D_800FCCB5 = 0;
    D_800FCCB7 = 0;
    D_800FCCB6 = 0;
    D_800783E4 = 0;
    D_800783E8 = 0;
    D_800783EC = 0;
    D_800783F0 = 0;
    D_800783F4 = 0;
    D_800783F8 = 0;
    for (i = 0; i < 0x15; i++) {
        D_800FCCD8[i] = 0;
    }
    for (i = 0; i < 20; i++) {
        D_800FCC50[i] = 0;
    }
    for (i = 0; i < 2; i++) {
        D_800FCCB8[i] = 0;
        D_800FCCBA[i] = 0;
        D_800FCCBC[i] = 0;
        D_800FCCBE[i] = 0;
        D_800FCCC0[i] = 0;
        D_800FCCC2[i] = 0;
        D_800FCCD4[i] = 0;
        D_800FCCD6[i] = 0;
    }
}
#else
#pragma GLOBAL_ASM("asm/us/nonmatchings/3D140/func_80041A98.s")
#endif

void Audio_QueueSequenceId(u32 arg0) {
    OSIntMask mask;

    if ((D_80078388 != 0) && (D_800783FC != 0) && (arg0 != 0)) {
        mask = osSetIntMask(OS_IM_NONE);
        D_800783A4[D_800783B8 & 3] = arg0;
        D_800783B8++;
        osSetIntMask(mask);
    }
}

void Audio_RemoveLastQueuedSequenceId(void) {
    OSIntMask mask;

    if ((D_80078388 != 0) && (D_800783FC != 0)) {
        mask = osSetIntMask(OS_IM_NONE);
        D_800783A4[(D_800783B8 - 1) & 3] = 0;
        osSetIntMask(mask);
    }
}

void Audio_SwapSoundCandidates(u8* arg0, u8* arg1) {
    u8 temp_v0 = *arg0;
    u8 temp_v1 = *arg1;

    if (temp_v1 != temp_v0) {
        *arg0 = temp_v1;
        *arg1 = temp_v0;
    }
}

u16 Audio_FilterRecentSoundId(u16 arg0) {
    s32 i;

    for (i = 0; i < 20; i++) {
        if (arg0 == D_800FCC50[i]) {
            return 0;
        }
    }

    return arg0;
}

void Audio_RecordRecentSoundId(u16 arg0) {
    u16 i;

    for (i = 0; i < 19; i++) {
        D_800FCC50[i] = D_800FCC50[i + 1];
    }

    D_800FCC50[19] = arg0;
}

void Audio_RemoveRecentSoundAt(u16 arg0) {
    u32 i;

    for (i = arg0; i > 0; i--) {
        D_800FCC50[i] = D_800FCC50[i - 1];
    }

    D_800FCC50[0] = 0;
}

u32 Audio_SelectSoundAvoidingRecent(u16* arg0) {
    u16 temp_v0 = *arg0;
    u32 i;
    u32 j;
    u16 sp5A;

    for (i = 0; i < temp_v0; i++) {
        D_800FCD18[i] = i + 1;
    }

    for (i = 0; i < temp_v0 << 1; i++) {
        Audio_SwapSoundCandidates(D_800FCD18, D_800FCD18 + (osGetTime() % (s32)temp_v0));
    }

    for (i = 0; i < temp_v0; i++) {
        sp5A = Audio_FilterRecentSoundId(arg0[D_800FCD18[i]]);
        if (sp5A != 0) {
            Audio_RecordRecentSoundId(sp5A);
            return sp5A;
        }
    }

    for (i = 0; i < 20; i++) {
        for (j = 1; j <= temp_v0; j++) {
            if (D_800FCC50[i] == arg0[j]) {
                Audio_RemoveRecentSoundAt(i);
                Audio_RecordRecentSoundId(arg0[j]);
                return arg0[j];
            }
        }
    }

    return 0;
}

void Audio_QueueSoundAvoidingRecent(u16* arg0) {
    u32 temp_v0 = Audio_SelectSoundAvoidingRecent(arg0);

    if (temp_v0 != 0) {
        Audio_QueueSequenceId(temp_v0);
    }
}

void Audio_QueueSoundWithFallbackList(u16 arg0, u16* arg1, u8* arg2) {
    u8 temp_v0;

    if (*arg2 == 0) {
        Audio_QueueSequenceId(arg0);
    } else {
        Audio_QueueSoundAvoidingRecent(arg1);
    }
    temp_v0 = *arg2;
    if (temp_v0 < 0xFF) {
        *arg2 = temp_v0 + 1;
    }
}

void func_800420F0_empty() {
    
}

#ifdef NON_MATCHING
u32 func_80042158(u32 arg0, BattleMonRuntime* arg1, BattleMonRuntime* arg2) {
    s32 side;
    u8* entry;
    u8 type;
    u8 power;
    u8 conv0;
    u8 conv1;
    f32 f0;
    f32 f2;
    f32 f12;
    f32 f14;
    f32 sp50;

    if ((arg0 == 0) || (arg0 >= 0xA6)) {
        arg0 = 1;
    }

    if (arg1 == D_800FCB18[0]) {
        side = 0;
    } else if (arg1 == D_800FCB18[1]) {
        side = 1;
    } else {
        return 0;
    }

    entry = &D_800780B4[(arg0 * 3) - 3];
    conv0 = Battle_MapCategoryIdToCompactIndex(entry[2]);

    type = entry[0];
    power = entry[1];

    switch (type) {
        case 7:
            return (u32)-1;

        case 8:
            return arg2->unk_28 / 2;

        case 0xD:
            switch (arg0) {
                case 0x31:
                    return 0x14;
                case 0x45:
                    return arg1->unk_26;
                case 0x52:
                    return 0x28;
                case 0x65:
                    return arg1->unk_26;
                case 0x95:
                    return (u32)((f32)arg1->unk_26 * 1.5f);
                default:
                    return 0;
            }

        case 0x22:
            if (entry == D_800781E0) {
                return (u32)-5;
            }
            return (u32)-4;
    }

    if (entry == D_80078180) {
        return (u32)-2;
    }

    if (arg1->unk_2A != 0) {
        f2 = arg1->unk_2A;
    } else {
        f2 = 1.0f;
    }

    if (arg2->unk_2C != 0) {
        f0 = arg2->unk_2C;
    } else {
        f0 = 1.0f;
    }

    sp50 = ((((f32)(((arg1->unk_26 * 2) / 5) + 2) * (f32)power) * f2) / f0);

    conv1 = Battle_MapCategoryIdToCompactIndex(arg2->unk_16[6]);
    {
        u8 conv2;
        u8 mode;

        conv2 = Battle_MapCategoryIdToCompactIndex(arg2->unk_16[7]);

        mode = D_800782A4[(conv1 * 0xF) + conv2];
        if (mode == 1) {
            f12 = 0.0f;
        } else if (mode == 2) {
            f12 = 0.5f;
        } else if (mode == 3) {
            f12 = 2.0f;
        } else {
            f12 = 1.0f;
        }

        mode = D_800782A4[(conv1 * 0xF) + conv0];
        if (mode == 1) {
            f2 = 0.0f;
        } else if (mode == 2) {
            f2 = 0.5f;
        } else if (mode == 3) {
            f2 = 2.0f;
        } else {
            f2 = 1.0f;
        }
    }

    if ((arg1->unk_16[6] == entry[2]) || (arg1->unk_16[7] == entry[2])) {
        f0 = 1.5f;
    } else {
        f0 = 1.0f;
    }

    {
        unk_D_800FCB48* hist;
        f32 prev;
        f32 cur;
        f32 mulA;
        f32 mulB;
        u8 flags;
        u32 ret;

        hist = &D_800FCB48[side ^ 1];
        prev = hist->unk_00[hist->unk_24 & 0xF];
        if (prev == 0.0f) {
            prev = 1.0f;
        }
        cur = arg2->unk_28;

        if (D_80078390[side] == 1) {
            mulA = D_8007CB80;
        } else {
            mulA = 1.0f;
        }

        hist = &D_800FCB48[side];
        f14 = hist->unk_00[hist->unk_24 & 0xF];
        if (f14 == 0.0f) {
            f14 = 1.0f;
        }
        f12 = arg1->unk_28;

        if (D_80078390[side ^ 1] == 1) {
            mulB = D_8007CB84;
        } else {
            mulB = 1.0f;
        }

        flags = arg2->unk_15;
        if (flags & 0x20) {
            f14 = D_8007CB88;
        } else if (flags & 7) {
            f14 = D_8007CB8C;
        } else {
            f14 = 1.0f;
        }

        ret = (u32)(((((cur / prev) * D_8007CB90) + 1.0f) * sp50 * f2 * f0) *
                    ((((f12 / hist->unk_00[D_800FCB48[side].unk_24 & 0xF]) * D_8007CB90) + 1.0f)) *
                    mulA * mulB * f14);

        if (ret >= 0x8000) {
            return 0x7FFF;
        }
        return ret;
    }
}
#else
#pragma GLOBAL_ASM("asm/us/nonmatchings/3D140/func_80042158.s")
#endif

s32 Battle_MapCategoryIdToCompactIndex(u8 arg0) {
    switch (arg0) {
    case 0:
        return 0;
    case 20:
        return 1;
    case 21:
        return 2;
    case 23:
        return 3;
    case 22:
        return 4;
    case 25:
        return 5;
    case 1:
        return 6;
    case 3:
        return 7;
    case 4:
        return 8;
    case 2:
        return 9;
    case 24:
        return 0xA;
    case 7:
        return 0xB;
    case 5:
        return 0xC;
    case 8:
        return 0xD;
    case 26:
        return 0xE;
    default:
        return 0;
    }
}

void Audio_QueueSequenceIdWrapper(u32 arg0) {
    Audio_QueueSequenceId(arg0);
}
