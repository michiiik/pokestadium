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

typedef struct HPHistoryEntry {
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
} HPHistoryEntry; // size = 0x84

typedef struct AudioMultiHitMovePair {
    /* 0x00 */ u8 unk_00;
    /* 0x01 */ u8 unk_01;
} AudioMultiHitMovePair;

// .data
// global

extern u8 D_800780B3[]; // needs addressing
extern u16 MenuSelectionSoundTable[1][9];
extern u16 D_80078446[1][9];
extern u16 D_800784EA[1][9];

// .rodata

// const f64 gAudioVolumeScale0 = 0.699999999999999956f;
// const f64 gAudioVolumeScale1 = 0.599999999999999978f;
// const f64 gAudioVolumeScale2 = 0.400000000000000022f;
// const f64 gAudioVolumeScale3 = 0.299999999999999989f;

// const f32 gAudioPitchScale0 = 1.100000024f;
// const f32 gAudioPitchScale1 = 1.100000024f;
// const f32 gAudioPitchScale2 = 1.200000048f;
// const f32 gAudioPitchScale3 = 1.100000024f;
// const f32 gAudioPitchStep = 0.009999999776f;

extern f64 gAudioVolumeScale0;
extern f64 gAudioVolumeScale1;
extern f64 gAudioVolumeScale2;
extern f64 gAudioVolumeScale3;

extern f32 gAudioPitchScale0;
extern f32 gAudioPitchScale1;
extern f32 gAudioPitchScale2;
extern f32 gAudioPitchScale3;
extern f32 gAudioPitchStep;

// .bss
s32 gAudioDMAFlags;
s8 gMusicFadeStepActive;
u8 gAudioUnitReadyCode;
s32 gAudioUnitReadyStatus;
AMAudioMgr __am;
AMDMAState dmaState;
s32 minFrameSize;
u32 frameSize;
s32 maxFrameSize;
s32 maxRSPCmds;
s32 gAudioOutputRate;
s32 gAudioMaxVoices;
OSMesgQueue audDMAMessageQ;
AMDMABuffer* dmaBuffs;
OSIoMesg* audDMAIOMesgBuf;
OSMesg* audDMAMessageBuf;
u32 dmaBufferLen;
OSPiHandle* gAudioPiHandle[4];
// s32 D_800FCB0C[3];
BattleMonRuntime* gAudioInfoPtr[4];
u32 PlayerCurrentHP[2];
u8 MoveQueueFlag[2];
u8 RecentMoveIDs[2][8];
HPHistoryEntry gHPHistory[2];
s32 gAudioRecentMessages[20];
s8 gAudioGlobalFlag0;
s8 gAudioGlobalFlag1;
u8 gAudioQueueDelayCounter;
u32 gAudioHPChangePercent;
s32 gAudioPrevHPChangePercent;
u8 gAudioCurrentMoveId;
u8 gAudioCurrentCommandId;
u8 gAudioSpecialCounter;
u8 gAudioSpecialFlag0;
u8 gAudioFaintedSidePlusOne;
u8 gAudioSfxCrossoverFlag;
u8 gAudioQueueProcessingFlag;
u8 gAudioPendingFreezeReaction;
u8 gAudioPendingSleepReaction;
u8 gAudioPendingDefeatReaction;
u8 gAudioAttackContinuesActive;
u8 gAudioMoveDamageReactionDone;
u8 gAudioEffectState0[2];
u8 gAudioEffectStateDominanceCount[2];
u8 gAudioEffectStateSleepCount[2];
u8 gAudioEffectStateFreezeCount[2];
u8 gAudioEffectStateParalysisCount[2];
u8 gAudioEffectStateCantMoveCount[2];
s8 gAudioEffectStateSelfHitPlayed[2];
u8 gAudioEffectMapJRepeatCount[2];
u8 gAudioEffectMapKEscalationCount[2];
u8 gAudioEffectToggleSfxThrash[2];
u8 gAudioEffectFellAsleepPlayed[2];
u8 gAudioEffectMapPRepeatCount[2];
u8 gAudioEffectMapQRepeatCount[2];
u8 gAudioEffectMissCrashPlayed[2];
u8 gAudioEffectStateDefeatMsgPlayed[2];
u8 gAudioEffectStateHeavyDamageKo[2];
s8 gAudioEffectTriggeredMask[0x18];
u32 gAudioCommandQueue[10];
u8 gAudioShuffleBuffer[0x28];

// function prototypes
void Audio_QueueSoundWithFallbackList(u16, u16*, u8*);
u32 Battle_EstimateMoveDamage(u32, BattleMonRuntime*, BattleMonRuntime*);
s32 Battle_MapCategoryIdToCompactIndex(u8);
ALDMAproc __amDmaNew(AMDMAState** state);

u8 gMusicEnabled[4] = {
    0x00, 0x00, 0x00, 0x00,
};
f32 gMusicVolume = 0.800000011920929f;
u8 gAudioProcessChannelsFlag[4] = {
    0x01, 0x00, 0x00, 0x00,
};
u8 gMusicFadeState[4] = {
    0x00, 0x00, 0x00, 0x00,
};
u16 gMusicFadeTargetTrack = 0x0000;
u32 gMusicFadeTimer = 0x00000000;
u8 gAudioPausedFlag = 0x00;
s32 gAudioUnitReady = 0x00000000;
u32 audFrameCt = 0x00000000;
u32 nextDMA = 0x00000000;
u32 curAcmdList = 0x00000000;
AudioInfo* lastInfo = NULL;
s32 min_only_one = 0x00000001;
s32 mus_active_fade_handle = 0x00000000;
f32 mus_fade_start_volume = 0.0f;
u8 mus_fade_target_volume = 0x00;
f32 mus_fade_step_rate = 0.0f;
f32 gMusicChannel2Volume = 0.0f;
f32 gMusicChannel2Step = 0.0f;
f32 gMusicChannel1Volume = 0.0f;
f32 gMusicChannel1Step = 0.0f;
u8 mus_sound_effect_blocked = 0x00;
u8 gAudioDmaBufferCount[8] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};
// Defined in 3D140_2.c (the second half of this translation unit pair).
void Audio_UpdateBattleAudioFrame(void);

void amCreateAudioMgr(ALSynConfig* c, amConfig* amc, u32 num_dma_buffers, s32 arg3, s32 arg4) {
    u32 i;
    f32 fsize;

    gAudioPiHandle[0] = osCartRomInit();
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

    gAudioOutputRate = c->outputRate;
    gAudioMaxVoices = c->maxVVoices;
    gMusicFadeStepActive = 0;

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

    if (gAudioDMAFlags & 0x80000000) {
        return osVirtualToPhysical(addr);
    }

    sp2C = gAudioPiHandle[0];
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

    if ((gAudioPausedFlag == 0) && (lastInfo != NULL)) {
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

    if (gAudioUnitReady != 0) {
        gAudioUnitReadyStatus = LeoTestUnitReady(&gAudioUnitReadyCode);
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

    if (gMusicFadeState[0] != 0) {
        Audio_MusicFadeUpdate();
    }

    if (gMusicEnabled[0] != 0) {
        GbMusic_UpdateChannels();
        if (lastInfo != NULL) {
            GbAudio_GenerateSamples(lastInfo->data, 0, ((audFrameCt % 3) == 0) ? 0x216 : 0x215, 0);
        }
    }

    if (gAudioProcessChannelsFlag[0] != 0) {
        Audio_ProcessChannels();
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

    if (gMusicChannel2Volume != 0.0f) {
        gMusicChannel2Volume -= gMusicChannel2Step;
        if (gMusicChannel2Volume <= 0) {
            gMusicChannel2Volume = 0.0f;
            Audio_QueueFadeCategoryCommand(2, 1);
        }
        Audio_QueueSetVolumeCommand(2, gMusicChannel2Volume);
    }

    if (gMusicChannel1Volume != 0.0f) {
        gMusicChannel1Volume -= gMusicChannel1Step;
        if (gMusicChannel1Volume <= 0) {
            gMusicChannel1Volume = 0.0f;
            Audio_QueueFadeCategoryCommand(1, 1);
        }
        Audio_QueueSetVolumeCommand(1, gMusicChannel1Volume);
    }

    if (gAudioPausedFlag != 0) {
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
        GbAudio_InitMusicPlayback(0x7D00, 0x78, 0x640);
        gMusicEnabled[0] = 1;
        gMusicVolume = D_8007C7C0;
        return;
    }
    gMusicEnabled[0] = 0;
}

void Cry_PlayFromPokedex(s32 arg0) {
    if (gMusicEnabled[0] == 0) {
        GbAudio_InitMusicPlayback(0x7D00, 0x78, 0x320);
        gMusicEnabled[0] = 1;
        gMusicFadeState[0] = 1;
        gMusicVolume = 0.0f;
        gMusicFadeTargetTrack = arg0;
        gMusicFadeTimer = 0;
        gMusicFadeStepActive = 0;
    }
}

void Audio_MusicFadeUpdate(void) {
    switch (gMusicFadeState[0]) {
        case 1:
            Audio_StartVolumeTransition(gMusicHandle, 0x50, 0x18, 3);
            GbMusic_StartTableSequence(gMusicFadeTargetTrack);
            gMusicFadeState[0]++;
            gMusicFadeTimer = 0;
            break;

        case 2:
            gMusicFadeTimer++;
            if (gMusicFadeTimer >= 2) {
                gMusicFadeTimer = 0;
                gMusicFadeState[0]++;
                gMusicFadeStepActive = 1;
            }
            break;

        case 3:
            if (GbMusic_CountActiveHighChannels() == 0) {
                gMusicFadeState[0]++;
                gMusicFadeTimer = 0;
            }
            break;

        case 4:
            if (gMusicFadeTimer == 0) {
                Audio_StartVolumeTransition(gMusicHandle, 0x18, 0x50, 0x14);
            }

            if (gMusicFadeTimer >= 0x1F) {
                gMusicFadeState[0]++;
            }

            gMusicFadeTimer++;
            break;

        case 5:
            gMusicFadeState[0] = 0;
            gMusicEnabled[0] = 0;
            gMusicFadeStepActive = 0;
            break;
    }
}

s32 Cry_IsPlaying(void) {
    return gMusicEnabled[0];
}

s32 Audio_SetEnabled(s32 arg0) {
    if (arg0 == 0) {
        if (mus_free_voices_count != gAudioMaxVoices) {
            return 0;
        }
        gAudioPausedFlag = 1;
    } else {
        osAiSetFrequency(gAudioOutputRate);
        lastInfo = NULL;
        dmaState.initialized = 0;
        gAudioPausedFlag = 0;
    }
    return 1;
}

s32 Audio_GetDiskReadyStatus(u8* arg0) {
    if (gAudioUnitReady == 0) {
        *arg0 = 7;
        return 8;
    }
    *arg0 = gAudioUnitReadyCode;
    return gAudioUnitReadyStatus;
}

void Audio_SetDiskReadyPolling(s32 arg0) {
    if (arg0 == 1) {
        gAudioUnitReady = 1;
        return;
    }
    gAudioUnitReady = 0;
}

/**
 * Sets or clears a bit in an array.
 */