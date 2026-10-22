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
    /* 0x7C */ u32 unk_7C;
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

// const f32 gAudioPitchScale0 = 1.100000024f;
// const f32 gAudioPitchScale1 = 1.100000024f;
// const f32 gAudioPitchScale2 = 1.200000048f;
// const f32 gAudioPitchScale3 = 1.100000024f;
// const f32 gAudioPitchStep = 0.009999999776f;


extern f32 gAudioPitchScale0;
extern f32 gAudioPitchScale1;
extern f32 gAudioPitchScale2;
extern f32 gAudioPitchScale3;
extern f32 gAudioPitchStep;

// .bss
extern s32 gAudioDMAFlags;
extern s8 gMusicFadeStepActive;
extern u8 gAudioUnitReadyCode;
extern s32 gAudioUnitReadyStatus;
extern AMAudioMgr __am;
extern AMDMAState dmaState;
extern s32 minFrameSize;
extern u32 frameSize;
extern s32 maxFrameSize;
extern s32 maxRSPCmds;
extern s32 gAudioOutputRate;
extern s32 gAudioMaxVoices;
extern OSMesgQueue audDMAMessageQ;
extern AMDMABuffer* dmaBuffs;
extern OSIoMesg* audDMAIOMesgBuf;
extern OSMesg* audDMAMessageBuf;
extern u32 dmaBufferLen;
extern OSPiHandle* gAudioPiHandle[4];
// s32 D_800FCB0C[3];
extern BattleMonRuntime* gAudioInfoPtr[4];
extern u32 PlayerCurrentHP[2];
extern u8 MoveQueueFlag[2];
extern u8 RecentMoveIDs[2][8];
extern HPHistoryEntry gHPHistory[2];
extern s32 gAudioRecentMessages[20];
extern s8 gAudioGlobalFlag0;
extern s8 gAudioGlobalFlag1;
extern u8 gAudioQueueDelayCounter;
extern u32 gAudioHPChangePercent;
extern s32 gAudioPrevHPChangePercent;
extern u8 gAudioCurrentMoveId;
extern u8 gAudioCurrentCommandId;
extern u8 gAudioSpecialCounter;
extern u8 gAudioSpecialFlag0;
extern u8 gAudioFaintedSidePlusOne;
extern u8 gAudioSfxCrossoverFlag;
extern u8 gAudioQueueProcessingFlag;
extern u8 gAudioPendingFreezeReaction;
extern u8 gAudioPendingSleepReaction;
extern u8 gAudioPendingDefeatReaction;
extern u8 gAudioAttackContinuesActive;
extern u8 gAudioMoveDamageReactionDone;
extern u8 gAudioEffectState0[2];
extern u8 gAudioEffectStateDominanceCount[2];
extern u8 gAudioEffectStateSleepCount[2];
extern u8 gAudioEffectStateFreezeCount[2];
extern u8 gAudioEffectStateParalysisCount[2];
extern u8 gAudioEffectStateCantMoveCount[2];
extern u8 gAudioEffectStateSelfHitPlayed[2];
extern u8 gAudioEffectMapJRepeatCount[2];
extern u8 gAudioEffectMapKEscalationCount[2];
extern u8 gAudioEffectToggleSfxThrash[2];
extern u8 gAudioEffectFellAsleepPlayed[2];
extern u8 gAudioEffectMapPRepeatCount[2];
extern u8 gAudioEffectMapQRepeatCount[2];
extern u8 gAudioEffectMissCrashPlayed[2];
extern u8 gAudioEffectStateDefeatMsgPlayed[2];
extern u8 gAudioEffectStateHeavyDamageKo[2];
extern s8 gAudioEffectTriggeredMask[0x18];
extern u32 gAudioCommandQueue[10];
extern u8 gAudioShuffleBuffer[0x28];

// function prototypes
void Audio_QueueSoundWithFallbackList(u16, u16*, u8*);
u32 Battle_EstimateMoveDamage(u32, BattleMonRuntime*, BattleMonRuntime*);
s32 Battle_MapCategoryIdToCompactIndex(u8);
ALDMAproc __amDmaNew(AMDMAState** state);

u16 gAnnouncerCueGenericIdle[8] = {
    0x0007, 0x024F, 0x0250, 0x0255, 0x0271, 0x0273, 0x0289, 0x028F,
};
u16 gAnnouncerCueGenericFaint[8] = {
    0x0006, 0x0269, 0x026C, 0x026D, 0x026E, 0x0287, 0x0288, 0x0000,
};
u16 gAnnouncerCuePostFaintLevelMismatch[4] = {
    0x0003, 0x0281, 0x0282, 0x0283,
};
u16 gAnnouncerCuePostFaintTypeMismatch[4] = {
    0x0003, 0x027E, 0x027F, 0x0280,
};
u16 gAnnouncerCueOneSidedDamage[4] = {
    0x0003, 0x024D, 0x0278, 0x0279,
};
u16 gAnnouncerCueEvenMatchLowHp[4] = {
    0x0003, 0x024C, 0x0274, 0x0275,
};
u16 gAnnouncerCueEvenMatch[4] = {
    0x0003, 0x024B, 0x0276, 0x0277,
};
u16 gAnnouncerCueFrozenReaction[4] = {
    0x0002, 0x027C, 0x027D, 0x0000,
};
u16 gAnnouncerCueAsleepReaction[4] = {
    0x0002, 0x027A, 0x027B, 0x0000,
};
u16 gAnnouncerCueEarlyBattleFaint[4] = {
    0x0003, 0x0252, 0x0270, 0x0272,
};
u16 gAnnouncerCueDefeatReaction[4] = {
    0x0003, 0x0284, 0x0285, 0x0286,
};
u16 gAnnouncerCueBothHeavyDamage[4] = {
    0x0003, 0x0259, 0x028B, 0x028C,
};
u16 gAnnouncerCueBothHeavyDamageLowHp[4] = {
    0x0003, 0x025A, 0x028A, 0x028C,
};
u16 gAnnouncerCueEarlyBattleBothHealthy[4] = {
    0x0003, 0x024E, 0x0258, 0x028E,
};
u16 gAnnouncerCueBigLeadFaint[4] = {
    0x0002, 0x0251, 0x028D, 0x0000,
};
u16 gAudioCueTable_GenericLow[8] = {
    0x0006, 0x024F, 0x0250, 0x0255, 0x0271, 0x0289, 0x028F, 0x0000,
};
u16 gAudioCueTable_GenericMid[4] = {
    0x0003, 0x0253, 0x026F, 0x0291,
};
u16 gAudioCueTable_GenericZero[4] = {
    0x0002, 0x0252, 0x0270, 0x0000,
};
u16 gAnnouncerCueMultiHitHeavyDamage[6] = {
    0x0005, 0x0062, 0x0067, 0x006E, 0x008D, 0x008E,
};
u16 gAnnouncerCueMultiHitMediumDamage[8] = {
    0x0006, 0x006D, 0x0072, 0x0089, 0x008A, 0x008B, 0x008C, 0x0000,
};
u16 gAnnouncerCueMultiHitLightDamage[6] = {
    0x0005, 0x006F, 0x0070, 0x009F, 0x00A0, 0x00A1,
};
u16 gAnnouncerCueCriticalHitDamage[6] = {
    0x0005, 0x0060, 0x0065, 0x0092, 0x0093, 0x0094,
};
u16 gAnnouncerCueSuperEffectiveDamage[6] = {
    0x0005, 0x0061, 0x0066, 0x0095, 0x0096, 0x0097,
};
u16 gAnnouncerCueNotVeryEffectiveDamage[6] = {
    0x0004, 0x0069, 0x0098, 0x0099, 0x009A, 0x0000,
};
u16 gAnnouncerCueCriticalSuperEffectiveDamage[6] = {
    0x0005, 0x0063, 0x0064, 0x008F, 0x0090, 0x0091,
};
u16 gAnnouncerCueCriticalHitLowDamage[4] = {
    0x0003, 0x006A, 0x009B, 0x009C,
};
u16 gAnnouncerCueNotVeryEffectiveHeavy[6] = {
    0x0004, 0x006B, 0x006C, 0x009D, 0x009E, 0x0000,
};
u16 gAnnouncerCueLightHitLowHp[4] = {
    0x0002, 0x008A, 0x008B, 0x0000,
};
u16 gAnnouncerCueAttackMissed[4] = {
    0x0003, 0x007C, 0x0086, 0x00AC,
};
u16 gAnnouncerCueNoEffect[4] = {
    0x0002, 0x007E, 0x007F, 0x0000,
};
u16 gAnnouncerCueMissNearKnockout[4] = {
    0x0002, 0x00AE, 0x00AF, 0x0000,
};
u16 gAnnouncerCueNoEffectGhost[4] = {
    0x0002, 0x0082, 0x0083, 0x0000,
};
u16 gAnnouncerCueLeechSeed[4] = {
    0x0002, 0x0072, 0x008A, 0x0000,
};
u16 gAnnouncerCueBadlyPoisoned[4] = {
    0x0002, 0x0085, 0x0084, 0x0000,
};
u16 gAnnouncerCuePoison[4] = {
    0x0002, 0x0088, 0x0071, 0x0000,
};
u16 gAnnouncerCueMultiHitHeavyDamageAlt[6] = {
    0x0005, 0x0062, 0x0067, 0x006E, 0x008D, 0x008E,
};
u16 gAnnouncerCueMissRepeated[4] = {
    0x0002, 0x0080, 0x0081, 0x0000,
};
u16 gAnnouncerCueSuperEffectiveEarly[4] = {
    0x0002, 0x00B3, 0x00B4, 0x0000,
};
u16 gAnnouncerCueMultiHitLowHp[6] = {
    0x0005, 0x0072, 0x0089, 0x008A, 0x008B, 0x008C,
};
u16 gAnnouncerCueOneHitKo[4] = {
    0x0003, 0x001D, 0x001E, 0x001F,
};
u16 gAnnouncerCueOneHitKoRepeat[4] = {
    0x0003, 0x0020, 0x0021, 0x0022,
};
u16 gAnnouncerCueHeavyDamage[6] = {
    0x0004, 0x0019, 0x001A, 0x001B, 0x001C, 0x0000,
};
u16 gAnnouncerCueHeavyDamageFirstTurn[4] = {
    0x0002, 0x001A, 0x001B, 0x0000,
};
u16 gAnnouncerCueKnockoutBlow[4] = {
    0x0003, 0x0018, 0x001B, 0x001C,
};
u16 gAnnouncerCueLightDamage[6] = {
    0x0004, 0x0014, 0x0015, 0x0016, 0x0017, 0x0000,
};
u16 gAnnouncerCueLightDamageFirstTurn[4] = {
    0x0002, 0x0010, 0x0011, 0x0000,
};
u16 gAnnouncerCueLightDamageFirstTurnAlt[4] = {
    0x0002, 0x0012, 0x0013, 0x0000,
};
u16 gAnnouncerCueRecall[6] = {
    0x0004, 0x00B7, 0x00B8, 0x00B9, 0x00BA, 0x0000,
};
u16 gAnnouncerCueRecallOutleveled[4] = {
    0x0003, 0x00BD, 0x00BE, 0x00BF,
};
u16 gAnnouncerCueRecallLateBattle[4] = {
    0x0002, 0x00C0, 0x00C1, 0x0000,
};
u16 gAnnouncerCueRecallSurvivedBigHit[4] = {
    0x0003, 0x00BB, 0x00C6, 0x00C7,
};
u16 gAnnouncerCueFainted[6] = {
    0x0004, 0x0050, 0x0051, 0x0052, 0x0053, 0x0000,
};
u16 gAnnouncerCueFaintedFirstTurn[4] = {
    0x0002, 0x0056, 0x0057, 0x0000,
};
u16 gAnnouncerCueSwitchIn[4] = {
    0x0002, 0x0059, 0x005A, 0x0000,
};
u16 gAnnouncerCueUnderdog[4] = {
    0x0002, 0x000B, 0x000C, 0x0000,
};
u16 gAnnouncerCueFrozen[4] = {
    0x0002, 0x0004, 0x0003, 0x0000,
};
u16 gAnnouncerCueAsleep[4] = {
    0x0002, 0x0005, 0x0003, 0x0000,
};
u16 gAnnouncerCueLowHp[4] = {
    0x0002, 0x0008, 0x000D, 0x0000,
};
u16 gAnnouncerCueUnderdogRepeat[4] = {
    0x0002, 0x000E, 0x000F, 0x0000,
};
u8 gAnnouncerCueSleep[8] = {
    0x00, 0x03, 0x01, 0x81, 0x01, 0x9D, 0x01, 0x9E,
};
u8 gAnnouncerCueFreeze[12] = {
    0x00, 0x04, 0x01, 0x7C, 0x01, 0x82, 0x01, 0x9D, 0x01, 0x9E, 0x00, 0x00,
};
u8 gAnnouncerCueParalysis[8] = {
    0x00, 0x02, 0x01, 0x7C, 0x01, 0x9E, 0x00, 0x00,
};
u8 gAnnouncerCueFreezeLastPokemon[8] = {
    0x00, 0x02, 0x01, 0x80, 0x01, 0x98, 0x00, 0x00,
};
u16 gAnnouncerCueBothLastPokemon[4] = {
    0x0002, 0x0180, 0x017C, 0x0000,
};
u16 gAnnouncerCueCantMove[4] = {
    0x0003, 0x0179, 0x019D, 0x019E,
};
u16 gAnnouncerCueAttackContinues[4] = {
    0x0002, 0x0177, 0x0178, 0x0000,
};
u16 gAnnouncerCueConfusionSelfHit[4] = {
    0x0002, 0x0183, 0x0184, 0x0000,
};
u16 gAnnouncerCueConfusionSelfHitRepeat[4] = {
    0x0002, 0x0184, 0x0185, 0x0000,
};
u16 gAnnouncerCueStatUp[4] = {
    0x0003, 0x0172, 0x0173, 0x0174,
};
u8 gAnnouncerCueStatDown[8] = {
    0x00, 0x03, 0x00, 0x71, 0x00, 0xA8, 0x00, 0xA9,
};
u16 gAnnouncerCueStatDownRepeat[4] = {
    0x0002, 0x0073, 0x0074, 0x0000,
};
u16 gAnnouncerCueSubstituteBroke[4] = {
    0x0002, 0x0175, 0x0176, 0x0000,
};
u16 gAnnouncerCueMoveFailed[4] = {
    0x0002, 0x007D, 0x00AA, 0x0000,
};
u16 gAnnouncerCueMoveNoEffect[4] = {
    0x0003, 0x007D, 0x00AA, 0x00AB,
};
u16 gAnnouncerCueFly[4] = {
    0x0002, 0x0199, 0x019A, 0x0000,
};
u16 gAnnouncerCueDig[4] = {
    0x0002, 0x019B, 0x019C, 0x0000,
};
u16 gAnnouncerCueBattleCommentary[6] = {
    0x0005, 0x005B, 0x005C, 0x005D, 0x005E, 0x005F,
};
u8 gAnnouncerMoveDataMoves1To68[204] = {
    0x00, 0x28, 0x00, 0x00, 0x32, 0x00, 0x1D, 0x0F, 0x00, 0x1D, 0x12, 0x00, 0x00, 0x50, 0x00, 0x10,
    0x28, 0x00, 0x04, 0x4B, 0x14, 0x05, 0x4B, 0x19, 0x06, 0x4B, 0x17, 0x00, 0x28, 0x00, 0x00, 0x37,
    0x00, 0x26, 0x01, 0x00, 0x27, 0x50, 0x00, 0x32, 0x00, 0x00, 0x00, 0x32, 0x00, 0x00, 0x28, 0x00,
    0x00, 0x23, 0x02, 0x1C, 0x00, 0x00, 0x2B, 0x46, 0x02, 0x2A, 0x0F, 0x00, 0x00, 0x50, 0x00, 0x00,
    0x23, 0x16, 0x25, 0x41, 0x00, 0x2C, 0x1E, 0x01, 0x00, 0x78, 0x00, 0x2D, 0x46, 0x01, 0x25, 0x3C,
    0x01, 0x16, 0x00, 0x00, 0x25, 0x46, 0x00, 0x00, 0x41, 0x00, 0x1D, 0x0F, 0x00, 0x26, 0x01, 0x00,
    0x00, 0x23, 0x00, 0x24, 0x55, 0x00, 0x2A, 0x0F, 0x00, 0x30, 0x5A, 0x00, 0x1B, 0x5A, 0x00, 0x30,
    0x64, 0x00, 0x13, 0x00, 0x00, 0x02, 0x0F, 0x03, 0x4D, 0x19, 0x07, 0x1D, 0x0E, 0x07, 0x13, 0x00,
    0x00, 0x1F, 0x3C, 0x00, 0x12, 0x00, 0x00, 0x1C, 0x00, 0x00, 0x20, 0x00, 0x00, 0x31, 0x00, 0x00,
    0x29, 0x01, 0x00, 0x56, 0x00, 0x00, 0x45, 0x28, 0x03, 0x04, 0x28, 0x14, 0x04, 0x5F, 0x14, 0x2E,
    0x00, 0x19, 0x00, 0x28, 0x15, 0x00, 0x78, 0x15, 0x00, 0x5F, 0x15, 0x05, 0x5F, 0x19, 0x23, 0x78,
    0x19, 0x4C, 0x41, 0x18, 0x46, 0x41, 0x15, 0x44, 0x41, 0x19, 0x50, 0x96, 0x00, 0x00, 0x23, 0x02,
    0x00, 0x50, 0x02, 0x30, 0x50, 0x01, 0x25, 0x32, 0x01, 0x00, 0x01, 0x01,
};
u8 gAnnouncerMoveDataMoves69To100[96] = {
    0x29, 0x01, 0x01, 0x00, 0x50, 0x00, 0x03, 0x14, 0x16, 0x03, 0x28, 0x16, 0x54, 0x00, 0x16, 0x0D,
    0x00, 0x00, 0x00, 0x37, 0x16, 0x27, 0x78, 0x16, 0x42, 0x00, 0x03, 0x43, 0x00, 0x16, 0x20, 0x00,
    0x16, 0x1B, 0x46, 0x16, 0x14, 0x00, 0x07, 0x29, 0x01, 0x1A, 0x2A, 0x0F, 0x14, 0x06, 0x28, 0x17,
    0x06, 0x5F, 0x17, 0x43, 0x00, 0x17, 0x06, 0x78, 0x17, 0x00, 0x32, 0x05, 0x00, 0x64, 0x04, 0x26,
    0x01, 0x04, 0x27, 0x64, 0x04, 0x42, 0x00, 0x03, 0x4C, 0x32, 0x18, 0x47, 0x5A, 0x18, 0x20, 0x00,
    0x18, 0x0A, 0x00, 0x18, 0x34, 0x00, 0x18, 0x00, 0x28, 0x00, 0x51, 0x14, 0x00, 0x1C, 0x00, 0x18,
};
u8 gAnnouncerMoveDataMoves101To165[196] = {
    0x29, 0x00, 0x08, 0x52, 0x00, 0x00, 0x3B, 0x00, 0x00, 0x0F, 0x00, 0x00, 0x38, 0x00, 0x00, 0x0B,
    0x00, 0x00, 0x0F, 0x00, 0x00, 0x16, 0x00, 0x00, 0x31, 0x00, 0x08, 0x0B, 0x00, 0x15, 0x0B, 0x00,
    0x00, 0x33, 0x00, 0x18, 0x40, 0x00, 0x18, 0x19, 0x00, 0x19, 0x41, 0x00, 0x18, 0x2F, 0x00, 0x00,
    0x1A, 0x00, 0x00, 0x53, 0x00, 0x00, 0x09, 0x00, 0x02, 0x07, 0x82, 0x00, 0x00, 0x64, 0x00, 0x24,
    0x14, 0x08, 0x21, 0x14, 0x03, 0x21, 0x41, 0x03, 0x1F, 0x41, 0x04, 0x22, 0x78, 0x14, 0x00, 0x50,
    0x15, 0x2A, 0x23, 0x15, 0x11, 0x3C, 0x00, 0x27, 0x64, 0x00, 0x1D, 0x14, 0x00, 0x46, 0x0A, 0x00,
    0x35, 0x00, 0x18, 0x16, 0x00, 0x18, 0x38, 0x00, 0x00, 0x2D, 0x55, 0x01, 0x43, 0x00, 0x00, 0x08,
    0x64, 0x18, 0x42, 0x00, 0x03, 0x1D, 0x0F, 0x00, 0x03, 0x14, 0x07, 0x20, 0x00, 0x00, 0x27, 0x8C,
    0x02, 0x39, 0x00, 0x00, 0x46, 0x14, 0x15, 0x00, 0x46, 0x00, 0x20, 0x00, 0x16, 0x16, 0x00, 0x00,
    0x29, 0x01, 0x18, 0x55, 0x00, 0x00, 0x33, 0x00, 0x03, 0x00, 0x5A, 0x15, 0x07, 0xAA, 0x00, 0x1D,
    0x12, 0x00, 0x2C, 0x32, 0x04, 0x38, 0x00, 0x18, 0x00, 0x4B, 0x05, 0x1F, 0x50, 0x00, 0x0A, 0x00,
    0x00, 0x18, 0x00, 0x00, 0x00, 0x50, 0x00, 0x28, 0x01, 0x00, 0x00, 0x46, 0x00, 0x4F, 0x00, 0x00,
    0x30, 0x32, 0x00, 0x00,
};
u8 gAnnouncerTypeEffectivenessTable[228] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x01, 0x00, 0x00,
    0x02, 0x02, 0x00, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x02, 0x00, 0x02, 0x00, 0x03,
    0x02, 0x02, 0x02, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x03, 0x00, 0x02, 0x00, 0x00, 0x03,
    0x02, 0x02, 0x00, 0x00, 0x00, 0x01, 0x03, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x02, 0x03, 0x00,
    0x02, 0x00, 0x00, 0x02, 0x03, 0x02, 0x00, 0x02, 0x03, 0x00, 0x02, 0x00, 0x02, 0x02, 0x00, 0x03,
    0x02, 0x00, 0x00, 0x03, 0x03, 0x00, 0x00, 0x03, 0x00, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x03,
    0x00, 0x02, 0x00, 0x02, 0x02, 0x02, 0x03, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00,
    0x02, 0x02, 0x00, 0x00, 0x03, 0x02, 0x02, 0x00, 0x00, 0x03, 0x00, 0x03, 0x02, 0x00, 0x00, 0x03,
    0x00, 0x01, 0x00, 0x02, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x03, 0x00, 0x03, 0x00, 0x00,
    0x00, 0x00, 0x03, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x03, 0x00, 0x00,
    0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x03, 0x00, 0x02, 0x00, 0x00, 0x02, 0x03,
    0x00, 0x00, 0x02, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x03, 0x02, 0x00, 0x02, 0x03, 0x00, 0x03,
    0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x03, 0x00, 0x00, 0x00,
};
s32 gAudioActiveFlag = 0x00000001;
u8 gTeamScores[4] = {
    0x03, 0x03, 0x03, 0x03,
};
u8 gPlayerRemainingPokemon[4] = {
    0x03, 0x03, 0x00, 0x00,
};
u8 gPlayerSideActiveIndex[4] = {
    0x00, 0x01, 0x00, 0x00,
};
u8 gPlayerSideMap[4] = {
    0x01, 0x02, 0x01, 0x02,
};
u8 gRecentMoveCount[4] = {
    0x00, 0x00, 0x00, 0x00,
};
u8 gAudioQueueNeedsUpdate = 0x00;
u32 gAudioSeqQueue[4] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};
u32 gAudioSeqTimer = 0x00000000;
s32 gAudioSeqIndex = 0x00000000;
u8 gAudioSeqActiveFlag = 0x00;
u8 gAudioSeqState = 0x00;
u8 gAudioSeqPlayedFlag = 0x00;
u32 gAudioSeqElapsed = 0x00000000;
u32 gAudioQueueMode = 0x00000000;
u8 gAudioCmd9DLatched = 0x00;
u8 gAudioEstimateMinus3Latched = 0x00;
u8 gAudioOneRemovalLatched = 0x00;
u8 gAudioQueueSfxToggle = 0x00;
u8 gAudioLowHPWarningFlag = 0x00;
u8 gAudioClimaxMusicPlayed = 0x00;
u8 gAudioRecentHeavyHitFlag = 0x00;
u8 gAudioQueueSpecialFlag = 0x00;
s16 gAudioSfxParam0 = 0x0000;
s16 gAudioSfxParam1 = 0x0000;
s16 gAudioSfxParam2 = 0x0000;
u8 gAudioSystemEnabled = 0x01;
u8 gAudioEffectId = 0x01;
u32 gCurrentStadiumPhase = 0x00000000;
s32 gStadiumPhaseSubstate = 0x00000000;
u8 gAudioStadiumSfxEnabled = 0x00;
// 0x80078410 / 0x80078446 / 0x800784EA are interior aliases (undefined_syms.ld).
u8 sMusicSeqTables[0x134] = {
    0x00, 0x35, 0x00, 0x23, 0x00, 0x24, 0x00, 0x25, 0x00, 0x26, 0x00, 0x27, 0x00, 0x28, 0x00, 0x29,
    0x00, 0x2A, 0x00, 0x35, 0x00, 0x2B, 0x00, 0x24, 0x00, 0x25, 0x00, 0x26, 0x00, 0x27, 0x00, 0x28,
    0x00, 0x29, 0x00, 0x2A, 0x00, 0x35, 0x00, 0x2C, 0x00, 0x24, 0x00, 0x25, 0x00, 0x26, 0x00, 0x27,
    0x00, 0x28, 0x00, 0x29, 0x00, 0x2A, 0x00, 0x35, 0x00, 0x2D, 0x00, 0x24, 0x00, 0x25, 0x00, 0x26,
    0x00, 0x27, 0x00, 0x28, 0x00, 0x29, 0x00, 0x2A, 0x00, 0x37, 0x00, 0x2F, 0x00, 0x24, 0x00, 0x25,
    0x00, 0x26, 0x00, 0x27, 0x00, 0x28, 0x00, 0x29, 0x00, 0x2A, 0x00, 0x36, 0x00, 0x2E, 0x00, 0x24,
    0x00, 0x25, 0x00, 0x26, 0x00, 0x27, 0x00, 0x28, 0x00, 0x29, 0x00, 0x2A, 0x00, 0x38, 0x00, 0x30,
    0x00, 0x24, 0x00, 0x25, 0x00, 0x26, 0x00, 0x27, 0x00, 0x28, 0x00, 0x29, 0x00, 0x2A, 0x00, 0x38,
    0x00, 0x31, 0x00, 0x24, 0x00, 0x25, 0x00, 0x26, 0x00, 0x27, 0x00, 0x28, 0x00, 0x29, 0x00, 0x2A,
    0x00, 0x38, 0x00, 0x32, 0x00, 0x24, 0x00, 0x25, 0x00, 0x26, 0x00, 0x27, 0x00, 0x28, 0x00, 0x29,
    0x00, 0x2A, 0x00, 0x38, 0x00, 0x33, 0x00, 0x24, 0x00, 0x25, 0x00, 0x26, 0x00, 0x27, 0x00, 0x28,
    0x00, 0x29, 0x00, 0x2A, 0x00, 0x38, 0x00, 0x30, 0x00, 0x24, 0x00, 0x25, 0x00, 0x26, 0x00, 0x27,
    0x00, 0x28, 0x00, 0x29, 0x00, 0x2A, 0x00, 0x38, 0x00, 0x30, 0x00, 0x24, 0x00, 0x25, 0x00, 0x26,
    0x00, 0x27, 0x00, 0x28, 0x00, 0x29, 0x00, 0x2A, 0x00, 0x00, 0x00, 0x39, 0x00, 0x4C, 0x00, 0x4E,
    0x00, 0x3A, 0x00, 0x4A, 0x00, 0x4D, 0x00, 0x4F, 0x00, 0x3B, 0x00, 0x00, 0x00, 0x4B, 0x00, 0x4C,
    0x00, 0x4E, 0x00, 0x3C, 0x00, 0x4A, 0x00, 0x4D, 0x00, 0x4F, 0x00, 0x3D, 0x00, 0x00, 0x00, 0x4B,
    0x00, 0x4C, 0x00, 0x4E, 0x00, 0x3E, 0x00, 0x4A, 0x00, 0x4D, 0x00, 0x4F, 0x00, 0x3F, 0x00, 0x00,
    0x00, 0x4B, 0x00, 0x4C, 0x00, 0x4E, 0x00, 0x40, 0x00, 0x4A, 0x00, 0x4D, 0x00, 0x4F, 0x00, 0x41,
    0x00, 0x00, 0x00, 0x42, 0x00, 0x43, 0x00, 0x44, 0x00, 0x45, 0x00, 0x46, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
};
u32 gCommandQueueCount = 0x00000000;
AudioMultiHitMovePair AudioCommandMap[6] = {
    { 0x03, 0x29 }, { 0x8C, 0x83 }, { 0x18, 0x1F }, { 0x9A, 0x04 }, { 0x9B, 0x2A }, { 0x00, 0x00 },
};
u8 AudioEffectTriggerIds[20] = {
    0x0E, 0x36, 0x4A, 0x60, 0x61, 0x68, 0x6A, 0x6B, 0x6E, 0x6F, 0x70, 0x71, 0x73, 0x74, 0x85, 0x97,
    0x9F, 0x00, 0x00, 0x00,
};
u8 AudioEffectSecondaryIds[24] = {
    0x1C, 0x27, 0x2B, 0x2D, 0x51, 0x67, 0x6C, 0x86, 0x94, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};


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
        gAudioActiveFlag = 1;
    } else {
        gAudioActiveFlag = 0;
    }
}

void Audio_SetChannelGroupCounts(u8 arg0, u8 arg1, u8 arg2, u8 arg3) {
    gTeamScores[0] = arg0;
    gTeamScores[1] = arg1;
    gTeamScores[2] = arg2;
    gTeamScores[3] = arg3;
}

void Audio_ResetChannelGroupState(u8 arg0) {
    s32 i;

    if (arg0 < 3) {
        for (i = 0; i < 16; i++) {
            gHPHistory[arg0].unk_00[i] = 0;
            gHPHistory[arg0].unk_28[i] = 0;
            gHPHistory[arg0].unk_50[i] = 0;
        }
        gHPHistory[arg0].unk_20 = 0;
        gHPHistory[arg0].unk_24 = 0;
        gHPHistory[arg0].unk_48 = 0;
        gHPHistory[arg0].unk_4C = 0;
        gHPHistory[arg0].unk_70 = 0;
        gHPHistory[arg0].unk_74 = 0;
        gHPHistory[arg0].unk_78 = 0;
        gHPHistory[arg0].unk_7C = 0;
        gHPHistory[arg0].unk_80 = 0;
        gAudioEffectStateSleepCount[arg0] = 0;
        gAudioEffectStateFreezeCount[arg0] = 0;
        gAudioEffectStateSelfHitPlayed[arg0] = 0;
        gAudioEffectMapJRepeatCount[arg0] = 0;
        gAudioEffectMapKEscalationCount[arg0] = 0;
        gAudioEffectToggleSfxThrash[arg0] = 0;
        gAudioEffectFellAsleepPlayed[arg0] = 0;
        gAudioEffectMapPRepeatCount[arg0] = 0;
        gAudioEffectMapQRepeatCount[arg0] = 0;
        gAudioEffectMissCrashPlayed[arg0] = 0;
        gAudioEffectStateDefeatMsgPlayed[arg0] = 0;
        gAudioEffectStateCantMoveCount[arg0] = 0;
        gAudioEffectStateParalysisCount[arg0] = 0;
    }
}

void Audio_ConfigureChannelGroups(u8 arg0, u8 arg1, u8 arg2, u8 arg3) {
    s32 i;
    s32 j;

    gPlayerSideMap[0] = arg0;
    gPlayerSideMap[1] = arg1;
    gPlayerSideMap[2] = arg2;
    gPlayerSideMap[3] = arg3;
    gPlayerSideActiveIndex[0] = 0xFF;
    gPlayerSideActiveIndex[1] = 0xFF;
    gPlayerRemainingPokemon[0] = 0;
    gPlayerRemainingPokemon[1] = 0;
    for (i = 0; i < 2; i++) {
        for (j = 0; j < 4; j++) {
            if (i + 1 == gPlayerSideMap[j]) {
                gPlayerRemainingPokemon[i] += gTeamScores[j];
            }
        }
    }
    gAudioInfoPtr[0] = 0;
    gAudioInfoPtr[1] = 0;
    PlayerCurrentHP[0] = 0;
    PlayerCurrentHP[1] = 0;
    Audio_ResetChannelGroupState(0);
    Audio_ResetChannelGroupState(1);
}

void Audio_SelectModeMusic(u32 arg0, u32 arg1) {
    s32 pad[1];
    u8 sp1B = 0;
    u16 var_a0;

    ResetAudioState();
    gCurrentStadiumPhase = arg1;
    gStadiumPhaseSubstate = arg0;

    if (arg1 < 0x11) {
        gAudioSystemEnabled = 1;
        gAudioStadiumSfxEnabled = 1;

        if ((arg0 == 0) && (arg1 != 0x10) && (arg1 != 0xF)) {
            Audio_QueueSequenceId(0x34);
        } else if ((arg0 == 0xFF) && (arg1 != 0x10) && (arg1 != 0xF)) {
            Audio_QueueSequenceId(0x47);
            arg0 = 0;
            sp1B = 1;
        }

        if (arg1 < 0xC) {
            if (arg0 < 9) {
                var_a0 = MenuSelectionSoundTable[arg1][arg0];
                gAudioSystemEnabled = 1;
                gAudioEffectId = 1;
            } else {
                return;
            }
        } else if (arg1 == 0xC) {
            if ((arg0 < 0x21) && (arg0 != 0)) {
                var_a0 = D_800784EA[(arg0 - 1) / 8][(arg0 - 1) % 8];
                gAudioSystemEnabled = 1;
                gAudioEffectId = 0;
            } else {
                return;
            }
        } else if (arg1 == 0xD) {
            if (arg0 < 6) {
                var_a0 = D_80078446[arg1][arg0];
                gAudioSystemEnabled = 1;
                gAudioEffectId = 0;
            } else {
                return;
            }
        } else {
            if (arg1 == 0xE) {
                var_a0 = 0x49;
                gAudioSystemEnabled = 1;
                gAudioEffectId = 2;
            } else if (arg1 == 0xF) {
                var_a0 = 0;
                gAudioSystemEnabled = 0;
                gAudioEffectId = 0;
                arg0 = 1;
            } else {
                if (arg1 == 0x10) {
                    var_a0 = 0x48;
                    gAudioEffectId = 1;
                    arg0 = 1;
                } else {
                    var_a0 = 0;
                    gAudioEffectId = 1;
                }
                gAudioSystemEnabled = 1;
            }
        }

        if ((arg0 == 0) && (sp1B == 0)) {
            gAudioEffectId = 2;
        }

        if (var_a0 != 0) {
            Audio_QueueSequenceId(var_a0);
        }
    }
}

void AudioFrameUpdate(s32 arg0) {
    s8 var_t0;
    s8 var_t2;
    s8 var_t4;
    s8 var_t5;
    s32 var_v0;
    s32 var_a0;
    s32 i;

    if (arg0 != 0) {
        var_v0 = osSetIntMask(1);
        gAudioSeqActiveFlag = 1;
        gAudioSeqElapsed = 0;
        gAudioSeqState = 0;
        gAudioSeqPlayedFlag = 0;
        osSetIntMask(var_v0);
        gAudioEstimateMinus3Latched = 0;
        gAudioOneRemovalLatched = 0;
        gAudioQueueSfxToggle = 0;
        gAudioSpecialCounter = 0;
        gAudioSpecialFlag0 = 0;
        gAudioQueueProcessingFlag = 0;
        gAudioAttackContinuesActive = 0;
        gAudioRecentHeavyHitFlag = 0;
        gAudioQueueSpecialFlag = 0;
        gAudioHPChangePercent = 0;
        Audio_PlayCommand(0x5A, gAudioEffectId, 0);

        if ((gAudioInfoPtr[0] == NULL) || (gAudioInfoPtr[1] == NULL)) {
            return;
        }

        if (gAudioInfoPtr[0] != NULL) {
            PlayerCurrentHP[0] = gAudioInfoPtr[0]->unk_0C;
        }

        if (gAudioInfoPtr[1] != NULL) {
            PlayerCurrentHP[1] = gAudioInfoPtr[1]->unk_0C;
        }

        gHPHistory[0].unk_24++;
        gHPHistory[0].unk_00[gHPHistory[0].unk_24 & 0xF] = gAudioInfoPtr[0]->unk_0C;
        gHPHistory[1].unk_24++;
        gHPHistory[1].unk_00[gHPHistory[1].unk_24 & 0xF] = gAudioInfoPtr[1]->unk_0C;

        if (gAudioQueueMode == 0) {
            return;
        }

        if (gAudioInfoPtr[0]->unk_26 < gAudioInfoPtr[1]->unk_26) {
            var_t2 = 0;
            var_t4 = 1;
        } else {
            var_t2 = 1;
            var_t4 = 0;
        }

        if (gAudioInfoPtr[0]->unk_0C < gAudioInfoPtr[1]->unk_0C) {
            var_t0 = 0;
            var_t5 = 1;
        } else {
            var_t0 = 1;
            var_t5 = 0;
        }

        if (gAudioQueueMode < 2) {
            if ((gAudioInfoPtr[0]->unk_0C == 0) || (gAudioInfoPtr[1]->unk_0C == 0)) {
                var_v0 = osSetIntMask(1);
                gAudioSeqState = 1;
                osSetIntMask(var_v0);

                if (gAudioInfoPtr[0]->unk_0C == 0) {
                    gAudioFaintedSidePlusOne = 1;
                } else {
                    gAudioFaintedSidePlusOne = 2;
                }

                if ((gAudioInfoPtr[0]->unk_0C == 0) && (gAudioInfoPtr[1]->unk_0C == 0)) {
                    Audio_QueueSoundAvoidingRecent(gAudioCueTable_GenericZero);
                } else {
                    Audio_QueueSoundAvoidingRecent(gAnnouncerCueEarlyBattleFaint);
                }
                return;
            }

            if (((((u32)gAudioInfoPtr[0]->unk_0C * 100) / gAudioInfoPtr[0]->unk_28) < 0x1E) &&
                ((((u32)gAudioInfoPtr[1]->unk_0C * 100) / gAudioInfoPtr[1]->unk_28) < 0x1E)) {
                Audio_QueueSoundAvoidingRecent(gAudioCueTable_GenericMid);
                return;
            }

            if (((((gAudioInfoPtr[var_t4]->unk_26 - gAudioInfoPtr[var_t2]->unk_26) * 0x64U) /
                  gAudioInfoPtr[var_t2]->unk_26) >= 0x15) &&
                ((((u32)gAudioInfoPtr[var_t2]->unk_0C * 100) / gAudioInfoPtr[var_t2]->unk_28) < 0x28) &&
                ((((u32)gAudioInfoPtr[var_t4]->unk_0C * 100) / gAudioInfoPtr[var_t4]->unk_28) >= 0x51)) {
                if ((osGetTime() % 3) == 0) {
                    var_a0 = 0x256;
                    goto block_53;
                }
            }

            if (((((u32)gAudioInfoPtr[0]->unk_0C * 100) / gAudioInfoPtr[0]->unk_28) >= 0x5B) &&
                ((((u32)gAudioInfoPtr[1]->unk_0C * 100) / gAudioInfoPtr[1]->unk_28) >= 0x5B)) {
                Audio_QueueSoundAvoidingRecent(gAnnouncerCueEarlyBattleBothHealthy);
            } else if (gAudioEffectId == 0) {
                Audio_QueueSoundAvoidingRecent(gAudioCueTable_GenericLow);
            } else {
                Audio_QueueSoundAvoidingRecent(gAnnouncerCueGenericIdle);
            }
            return;
        }

        if ((gAudioInfoPtr[0]->unk_0C == 0) || (gAudioInfoPtr[1]->unk_0C == 0)) {
            var_v0 = osSetIntMask(1);
            gAudioSeqState = 1;
            osSetIntMask(var_v0);

            if (gAudioInfoPtr[0]->unk_0C == 0) {
                gAudioFaintedSidePlusOne = 1;
            } else {
                gAudioFaintedSidePlusOne = 2;
            }

            if (gPlayerRemainingPokemon[0] >= gPlayerRemainingPokemon[1]) {
                var_v0 = gPlayerRemainingPokemon[0] - gPlayerRemainingPokemon[1];
            } else {
                var_v0 = gPlayerRemainingPokemon[1] - gPlayerRemainingPokemon[0];
            }

            if (var_v0 >= 2) {
                Audio_QueueSoundAvoidingRecent(gAnnouncerCueBigLeadFaint);
                return;
            }

            if ((gPlayerRemainingPokemon[0] == 1) && (gPlayerRemainingPokemon[1] == 1)) {
                if (gAudioClimaxMusicPlayed != 0) {
                    gAudioClimaxMusicPlayed = 0;
                    var_a0 = 0x290;
                } else if ((osGetTime() % 2) != 0) {
                    var_a0 = 0x254;
                } else {
                    var_a0 = 0x26B;
                }
block_53:
                Audio_QueueSequenceId(var_a0);
            } else if (gPlayerRemainingPokemon[gAudioFaintedSidePlusOne - 1] == 1) {
                Audio_QueueSequenceId(0x26A);
            } else {
                Audio_QueueSoundAvoidingRecent(gAnnouncerCueGenericFaint);
            }
            return;
        }

        if (gAudioFaintedSidePlusOne != 0) {
            if (((gPlayerRemainingPokemon[0] == 1) || (gPlayerRemainingPokemon[1] == 1)) && (gPlayerRemainingPokemon[0] < 4) &&
                (gPlayerRemainingPokemon[1] < 4)) {
                if (gPlayerRemainingPokemon[0] == 1) {
                    switch (gPlayerRemainingPokemon[1]) {
                        case 1:
                            Audio_QueueSequenceId(0x260);
                            Audio_QueueSequenceId(0x266);
                            break;

                        case 2:
                            Audio_QueueSequenceId(0x261);
                            if (gAudioFaintedSidePlusOne == 1) {
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

                    gAudioFaintedSidePlusOne = 0;
                    return;
                }

                switch (gPlayerRemainingPokemon[0]) {
                    case 2:
                        Audio_QueueSequenceId(0x25E);
                        if (gAudioFaintedSidePlusOne == 2) {
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

                gAudioFaintedSidePlusOne = 0;
                return;
            }

            if (gAudioSfxCrossoverFlag != 0) {
                if ((osGetTime() % 2) != 0) {
                    gAudioFaintedSidePlusOne = 0;
                    Audio_QueueSoundAvoidingRecent(gAnnouncerCuePostFaintLevelMismatch);
                    return;
                }
            }

            if (gAudioInfoPtr[0]->unk_16[6] != gAudioInfoPtr[1]->unk_16[6]) {
                if (gAudioInfoPtr[0]->unk_16[6] != gAudioInfoPtr[1]->unk_16[7]) {
                    if ((gAudioInfoPtr[0]->unk_16[7] != gAudioInfoPtr[1]->unk_16[6]) &&
                        (gAudioInfoPtr[0]->unk_16[7] != gAudioInfoPtr[1]->unk_16[7])) {
                        gAudioFaintedSidePlusOne = 0;
                        Audio_QueueSoundAvoidingRecent(gAnnouncerCuePostFaintTypeMismatch);
                        return;
                    }
                }
            }

            gAudioFaintedSidePlusOne = 0;
            switch (gPlayerRemainingPokemon[0]) {
                case 2:
                    switch (gPlayerRemainingPokemon[1]) {
                        case 2:
                            Audio_QueueSequenceId(0x25D);
                            Audio_QueueSequenceId(0x268);
                            break;

                        case 3:
                            Audio_QueueSequenceId(0x25F);
                            Audio_QueueSequenceId(0x263);
                            break;

                        default:
                            Audio_QueueSequenceId(0x271);
                            break;
                    }
                    break;

                case 3:
                    if (gPlayerRemainingPokemon[1] == 2) {
                        Audio_QueueSequenceId(0x25B);
                        Audio_QueueSequenceId(0x263);
                    } else {
                        Audio_QueueSequenceId(0x271);
                    }
                    break;

                default:
                    if (gAudioEffectId == 0) {
                        Audio_QueueSoundAvoidingRecent(gAudioCueTable_GenericLow);
                    } else {
                        Audio_QueueSoundAvoidingRecent(gAnnouncerCueGenericIdle);
                    }
                    break;
            }

            return;
        }

        if (gAudioPendingFreezeReaction != 0) {
            Audio_QueueSoundAvoidingRecent(gAnnouncerCueFrozenReaction);
            gAudioPendingFreezeReaction = 0;
            return;
        }

        if (gAudioPendingSleepReaction != 0) {
            Audio_QueueSoundAvoidingRecent(gAnnouncerCueAsleepReaction);
            gAudioPendingSleepReaction = 0;
            return;
        }

        if (gAudioPendingDefeatReaction != 0) {
            Audio_QueueSoundAvoidingRecent(gAnnouncerCueDefeatReaction);
            gAudioPendingDefeatReaction = 0;
            return;
        }

        if ((gAudioInfoPtr[0]->unk_28 * 0.5f) <
            (gHPHistory[0].unk_00[(gHPHistory[0].unk_24 - 1) & 0xF] -
             gHPHistory[0].unk_00[gHPHistory[0].unk_24 & 0xF])) {
            if ((gAudioInfoPtr[1]->unk_28 * 0.5f) <
                (gHPHistory[1].unk_00[(gHPHistory[1].unk_24 - 1) & 0xF] -
                 gHPHistory[1].unk_00[gHPHistory[1].unk_24 & 0xF])) {
                if (((((u32)gAudioInfoPtr[0]->unk_0C * 100) / gAudioInfoPtr[0]->unk_28) < 0x14U) &&
                    ((((u32)gAudioInfoPtr[1]->unk_0C * 100) / gAudioInfoPtr[1]->unk_28) < 0x14U)) {
                    Audio_QueueSoundAvoidingRecent(gAnnouncerCueBothHeavyDamageLowHp);
                } else {
                    Audio_QueueSoundAvoidingRecent(gAnnouncerCueBothHeavyDamage);
                }
                return;
            }
        }

        if (gAudioInfoPtr[0]->unk_0C >= gAudioInfoPtr[1]->unk_0C) {
            var_v0 = gAudioInfoPtr[0]->unk_0C - gAudioInfoPtr[1]->unk_0C;
        } else {
            var_v0 = gAudioInfoPtr[1]->unk_0C - gAudioInfoPtr[0]->unk_0C;
        }

        if (((gAudioInfoPtr[var_t0]->unk_28 * 0.7) < var_v0) &&
            ((((u32)gAudioInfoPtr[var_t5]->unk_0C * 100) / gAudioInfoPtr[var_t5]->unk_28) >= 0x47) &&
            (gAudioInfoPtr[0]->unk_0C != gAudioInfoPtr[0]->unk_28) &&
            (gAudioInfoPtr[1]->unk_0C != gAudioInfoPtr[1]->unk_28)) {
            if ((gAudioInfoPtr[var_t5]->unk_28 * 0.6) < gAudioInfoPtr[var_t5]->unk_0C) {
                if (gAudioInfoPtr[var_t0]->unk_0C < (gAudioInfoPtr[var_t0]->unk_28 * 0.4)) {
                    Audio_QueueSoundAvoidingRecent(gAnnouncerCueOneSidedDamage);
                    return;
                }
            }
        }

        if (gAudioInfoPtr[0]->unk_0C >= gAudioInfoPtr[1]->unk_0C) {
            var_v0 = gAudioInfoPtr[0]->unk_0C - gAudioInfoPtr[1]->unk_0C;
        } else {
            var_v0 = gAudioInfoPtr[1]->unk_0C - gAudioInfoPtr[0]->unk_0C;
        }

        if (var_v0 < 0xA) {
            if ((gAudioInfoPtr[0]->unk_28 * 0.5) < gAudioInfoPtr[0]->unk_0C) {
                if (((gAudioInfoPtr[1]->unk_28 * 0.5) < gAudioInfoPtr[1]->unk_0C) &&
                    (gAudioInfoPtr[0]->unk_0C < gAudioInfoPtr[0]->unk_28) &&
                    (gAudioInfoPtr[1]->unk_0C < gAudioInfoPtr[1]->unk_28)) {
                    Audio_QueueSoundAvoidingRecent(gAnnouncerCueEvenMatch);
                    return;
                }
            }
        }

        if (gAudioInfoPtr[0]->unk_0C >= gAudioInfoPtr[1]->unk_0C) {
            var_v0 = gAudioInfoPtr[0]->unk_0C - gAudioInfoPtr[1]->unk_0C;
        } else {
            var_v0 = gAudioInfoPtr[1]->unk_0C - gAudioInfoPtr[0]->unk_0C;
        }

        if (var_v0 < 0xA) {
            if (gAudioInfoPtr[0]->unk_0C < (gAudioInfoPtr[0]->unk_28 * 0.3)) {
                if (gAudioInfoPtr[1]->unk_0C < (gAudioInfoPtr[1]->unk_28 * 0.3)) {
                    Audio_QueueSoundAvoidingRecent(gAnnouncerCueEvenMatchLowHp);
                    return;
                }
            }
        }

        if (gAudioEffectId == 0) {
            Audio_QueueSoundAvoidingRecent(gAudioCueTable_GenericLow);
        } else {
            Audio_QueueSoundAvoidingRecent(gAnnouncerCueGenericIdle);
        }

        return;
    }

    var_v0 = osSetIntMask(1);
    gAudioSeqActiveFlag = 0;
    osSetIntMask(var_v0);
    Audio_FadeCommand(0x5A, 0);
    Audio_FadeCommand(0x5B, 0);

    if (gAudioInfoPtr[0] != NULL) {
        PlayerCurrentHP[0] = gAudioInfoPtr[0]->unk_0C;
    }

    if (gAudioInfoPtr[1] != NULL) {
        PlayerCurrentHP[1] = gAudioInfoPtr[1]->unk_0C;
    }

    gAudioQueueMode++;

    for (i = 0; i < 2; i++) {
        gHPHistory[i].unk_78++;
        if (gHPHistory[i].unk_7C != 0) {
            gHPHistory[i].unk_7C++;
        }
    }

    gAudioPendingSleepReaction = 0;
    gAudioPendingFreezeReaction = 0;
    gAudioPendingDefeatReaction = 0;
    gAudioClimaxMusicPlayed = 0;

    if ((gAudioInfoPtr[0] != NULL) && (gAudioInfoPtr[1] != NULL) && (gAudioStadiumSfxEnabled != 0) &&
        (gAudioInfoPtr[0]->unk_0C != 0) && (gAudioInfoPtr[1]->unk_0C != 0) && (AudioStream_IsPlaying(1) == 0)) {
        if ((osGetTime() % 2) == 0) {
            Audio_QueueSoundAvoidingRecent(gAnnouncerCueBattleCommentary);
        }
    }
}

void Audio_ProcessBattleInfo(s32 arg0, BattleMonRuntime* arg1) {
    u8 pad;
    u8 value;
    u8 side;
    u8 changed = 0;
    u32 i;
    u8 pad2;

    gAudioSfxCrossoverFlag = 0;
    gAudioSfxParam0 = 0;
    gAudioSfxParam1 = 0;
    gAudioSfxParam2 = 0;

    if (arg1 == NULL) {
        return;
    }
    if ((arg0 == 0) || ((u32)arg0 >= 5)) {
        return;
    }

    arg0--;
    side = gPlayerSideMap[arg0];
    if ((side <= 0) || (side >= 3)) {
        return;
    }
    side--;
    side &= 0xFF;

    if (gPlayerSideActiveIndex[side] != arg0) {
        changed = 1;
        gPlayerSideActiveIndex[side] = arg0;
    }

    gAudioInfoPtr[side] = arg1;

    Audio_ResetChannelGroupState(side);

    gAudioEffectMapKEscalationCount[side ^ 1] = 0;

    PlayerCurrentHP[side] = gAudioInfoPtr[side]->unk_0C;
    gHPHistory[side].unk_24++;
    gHPHistory[side].unk_00[gHPHistory[side].unk_24 & 0xF] = gAudioInfoPtr[side]->unk_0C;

    value = gAudioInfoPtr[side]->id;
    if ((value <= 0) || (value >= 0x98)) {
        return;
    }

    for (i = 0; i < gRecentMoveCount[side]; i++) {
        if (RecentMoveIDs[side][i] == value) {
            goto found_value;
        }
    }

    RecentMoveIDs[side][gRecentMoveCount[side]] = value;
    gRecentMoveCount[side]++;
    if (gRecentMoveCount[side] >= 8) {
        gRecentMoveCount[side] = 7;
    }

    MoveQueueFlag[side] = 0;
    if (gAudioQueueMode != 0) {
        Audio_QueueSequenceId(value + 0xCB);
        if (changed) {
            Audio_QueueSoundAvoidingRecent(gAnnouncerCueSwitchIn);
        }

        if (gAudioInfoPtr[side ^ 1] == NULL) {
            return;
        }

        if (((f32)gAudioInfoPtr[side ^ 1]->unk_26 - (f32)(gAudioInfoPtr[side])->unk_26) > ((f32)(gAudioInfoPtr[side])->unk_26 * 2.0f)) {
            gAudioSfxCrossoverFlag = 1;
            gAudioEffectStateDominanceCount[side] = 0;
            return;
        }

        if (((f32)(gAudioInfoPtr[side])->unk_26 * 0.3f) < ((f32)gAudioInfoPtr[side ^ 1]->unk_26 - (f32)(gAudioInfoPtr[side])->unk_26)) {
            gAudioSfxCrossoverFlag = 1;
            gAudioEffectStateDominanceCount[side] = 0;
            return;
        }

        if (((f32)gAudioInfoPtr[side ^ 1]->unk_26 * 0.3f) < ((f32)(gAudioInfoPtr[side])->unk_26 - (f32)gAudioInfoPtr[side ^ 1]->unk_26)) {
            gAudioSfxCrossoverFlag = 1;
            if ((gAudioEffectStateHeavyDamageKo[side] != 0) && (gAudioEffectStateDominanceCount[side] != 0)) {
                if (changed == 0) {
                    Audio_QueueSoundAvoidingRecent(gAnnouncerCueUnderdogRepeat);
                }
            } else if (changed == 0) {
                Audio_QueueSoundWithFallbackList(9, gAnnouncerCueUnderdog, &gAudioEffectStateDominanceCount[side]);
            } else {
                gAudioEffectStateDominanceCount[side]++;
            }

            gAudioEffectState0[side] = 0;
            gAudioEffectStateHeavyDamageKo[side] = 0;
            return;
        }

        if ((gPlayerRemainingPokemon[side] == 2) && (gPlayerRemainingPokemon[side ^ 1] == 3)) {
            if ((((u32)gAudioInfoPtr[side ^ 1]->unk_0C * 100) / gAudioInfoPtr[side ^ 1]->unk_28) < 0x32) {
                if (gAudioLowHPWarningFlag == 0) {
                    if (changed == 0) {
                        Audio_QueueSequenceId(0xA);
                    }
                    gAudioLowHPWarningFlag = 1;
                }
            }
        }
    }
    gAudioEffectState0[side] = 0;
    gAudioEffectStateDominanceCount[side] = 0;
    return;

found_value:
    if (MoveQueueFlag[side] == 0) {
        if (gAudioQueueSfxToggle == 0) {
            if (gPlayerRemainingPokemon[side] < 2) {
                if ((osGetTime() % 2) == 0) {
                    Audio_QueueSequenceId(2);
                } else {
                    Audio_QueueSequenceId(1);
                }
            } else {
                Audio_QueueSequenceId(0x50);
                if (changed != 0) {
                    Audio_QueueSoundAvoidingRecent(gAnnouncerCueSwitchIn);
                }
            }
            gAudioQueueSfxToggle = 1;
        } else {
            Audio_QueueSequenceId(0x58);
            gAudioQueueSfxToggle = 0;
            if (changed != 0) {
                Audio_QueueSoundAvoidingRecent(gAnnouncerCueSwitchIn);
            }
        }
    } else if (changed != 0) {
        Audio_QueueSoundAvoidingRecent(gAnnouncerCueSwitchIn);
    }

    MoveQueueFlag[side] = 0;

    if (gAudioInfoPtr[side ^ 1] == NULL) {
        return;
    }

    if (changed != 0) {
        return;
    }

    if ((gAudioInfoPtr[side])->unk_15 & 0x20) {
        Audio_QueueSoundAvoidingRecent(gAnnouncerCueFrozen);
        return;
    }

    if ((gAudioInfoPtr[side])->unk_15 & 7) {
        Audio_QueueSoundAvoidingRecent(gAnnouncerCueAsleep);
        return;
    }

    if ((gAudioInfoPtr[side])->unk_15 & 8) {
        Audio_QueueSequenceId(6);
        return;
    }

    if ((gAudioInfoPtr[side])->unk_15 & 0x10) {
        Audio_QueueSequenceId(7);
        return;
    }

    if ((((f32)(gAudioInfoPtr[side])->unk_0C / (f32)(gAudioInfoPtr[side])->unk_28) < 0.2f) && ((gAudioInfoPtr[side])->unk_0C < gAudioInfoPtr[side ^ 1]->unk_0C)) {
        Audio_QueueSoundAvoidingRecent(gAnnouncerCueLowHp);
    }
}

void RemovePlayer(u32 arg0) {
    u8 sp1F;

    if (arg0 == 0 || arg0 >= 5) {
        return;
    }
    arg0--;
    sp1F = gPlayerSideMap[arg0];
    if (sp1F <= 0 || sp1F >= 3) {
        return;
    }
    sp1F--;
    gTeamScores[arg0]--;
    gPlayerRemainingPokemon[sp1F]--;
    MoveQueueFlag[sp1F] = 0;
    if (gAudioInfoPtr[0] == 0 || gAudioInfoPtr[1] == 0) {
        return;
    }
    Audio_PlayCommand(0x16, gAudioEffectId, 0);
    if (gAudioOneRemovalLatched != 0) {
        Audio_QueueSequenceId(0xC3);
        gAudioOneRemovalLatched = 0;
        gAudioClimaxMusicPlayed = 1;
        return;
    }
    gAudioOneRemovalLatched = 1;
    if (gAudioEstimateMinus3Latched != 0) {
        Audio_QueueSequenceId(0xB9);
        gAudioEstimateMinus3Latched = 0;
        return;
    }
    if (gAudioQueueMode < 2) {
        if (osGetTime() % 2 == 0) {
            Audio_QueueSequenceId(0xBC);
        } else {                
            Audio_QueueSequenceId(0xC5);
        }
    } else if (gAudioCmd9DLatched != 0) {
        Audio_QueueSequenceId(0xC4);
        gAudioCmd9DLatched = 0;
    } else if ((gAudioHPChangePercent >= 0x64U) && (gAudioQueueProcessingFlag == 0) && (gAudioQueueSpecialFlag == 0)) {
        if (((f32)gAudioInfoPtr[sp1F]->unk_26 * (3.0 / 10.0)) < ((f32)gAudioInfoPtr[sp1F ^ 1]->unk_26 - gAudioInfoPtr[sp1F]->unk_26)) {
            Audio_QueueSoundAvoidingRecent(gAnnouncerCueRecallOutleveled);
        } else {
            Audio_QueueSoundAvoidingRecent(gAnnouncerCueRecallSurvivedBigHit);
        }
        gAudioEffectStateHeavyDamageKo[sp1F] = 1;
    } else if ((gHPHistory[sp1F].unk_78 >= 8U) && (gAudioQueueProcessingFlag == 0)) {
        if (((u32) gAudioHPChangePercent < 5U) && (gAudioRecentHeavyHitFlag == 0)) {
            Audio_QueueSequenceId(0xC2);
        } else {
            Audio_QueueSoundAvoidingRecent(gAnnouncerCueRecallLateBattle);
        }
    } else {
        Audio_QueueSoundAvoidingRecent(gAnnouncerCueRecall);
    }
}

void PlayerDefeated(u32 arg0) {
    u8 temp_v0;
    
    if (arg0 == 0 || arg0 >= 5) {
        return;
    }
    
    arg0--;
    temp_v0 = gPlayerSideMap[arg0];

    if (temp_v0 <= 0 || temp_v0 >= 3) {
        return;
    }
    
    temp_v0--;

    if (gAudioInfoPtr[0] == 0 || gAudioInfoPtr[1] == 0) {
        return;
    }

    if (gAudioInfoPtr[temp_v0]->unk_0C <= 0) {
       return; 
    }
    
    MoveQueueFlag[temp_v0] = 1;
    gAudioPendingDefeatReaction = 1;
    
    if (gAudioQueueSfxToggle != 0) {
        Audio_QueueSequenceId(0x58);
        gAudioQueueSfxToggle = 0;
        return;
    }
    
    gAudioQueueSfxToggle = 1;
    
    if (gAudioQueueMode < 2) {
        if (osGetTime() % 2 == 0) {
            Audio_QueueSequenceId(0x54);
        } else {
            Audio_QueueSequenceId(0x55);
        }
    } else if (gHPHistory[temp_v0].unk_78 < 2) {
        Audio_QueueSoundAvoidingRecent(gAnnouncerCueFaintedFirstTurn);
    } else {
        Audio_QueueSoundAvoidingRecent(gAnnouncerCueFainted);
    }
}

void Audio_QueuePendingSoundId(u32 arg0) {
    gAudioCommandQueue[gCommandQueueCount] = arg0;
    gCommandQueueCount++;
    if (gCommandQueueCount >= 0xA) {
        gCommandQueueCount--;
    }
}

void ProcessAudioCommandQueue(s32 arg0) {
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
    sp3C = gAudioQueueDelayCounter;
    gAudioQueueDelayCounter = 0;
    osSetIntMask(mask);
    gAudioCmd9DLatched = 0;
    gAudioQueueProcessingFlag = 0;
    if (arg0 == 0 || arg0 >= 5U) {
        return;
    }
    arg0--;
    temp_s0 = gPlayerSideMap[arg0];
    if (temp_s0 <= 0 || temp_s0 >= 3) {
        return;
    }
    temp_s0--;
    if (gAudioInfoPtr[0] == 0 || gAudioInfoPtr[1] == 0) {
        return;
    }
    for (sp40 = 0; sp40 < gCommandQueueCount; sp40++) {
        switch (gAudioCommandQueue[sp40]) {
        case 0x1D:
            sp3A = 1;
            gAudioQueueNeedsUpdate = 1;
            break;
        case 0x1E:
            sp37 = 1;
            gAudioQueueNeedsUpdate = 1;
            break;
        case 0x29:
            sp39 = 1;
            gAudioQueueNeedsUpdate = 1;
            break;
        case 0x2A:
            sp38 = 1;
            gAudioQueueNeedsUpdate = 1;
            break;
        case 0x66:
            sp36 = 1;
            break;
        case 0x6F:
            sp34 = 1;
            break;
        case 0x6B:
        case 0x6C:
            gHPHistory[temp_s0].unk_24++;
            ((u16*)gHPHistory[temp_s0].unk_00)[gHPHistory[temp_s0].unk_24 & 0xF] = gAudioInfoPtr[temp_s0]->unk_0C;
            mask = osSetIntMask(1);
            gAudioQueueDelayCounter = (u8) sp3C;
            osSetIntMask(mask);
            break;
        case 0x6E:
        case 0x70:
        case 0x9E:
        case 0xAB:
            mask = osSetIntMask(1);
            gAudioQueueDelayCounter = (u8) sp3C;
            osSetIntMask(mask);
            break;
        case 0x0:
        case 0x1:
        case 0x2:
            gAudioQueueProcessingFlag = 1;
            gAudioQueueSpecialFlag = 1;
            break;
        case 0x9D:
            gAudioCmd9DLatched = 1;
            gAudioQueueProcessingFlag = 1;
            break;
        case 0x6D:
            Audio_QueueSequenceId(0x165);
            gCommandQueueCount = 0;
            gAudioEffectStateFreezeCount[temp_s0] = 0;
            Audio_PlayCommand(0x16, gAudioEffectId, 0);
            return;
        case 0x6:
            if (gHPHistory[temp_s0].unk_7C == 1) {
                Audio_QueueSequenceId(0x19F);
            } else if (gHPHistory[temp_s0].unk_7C >= 4U) {
                Audio_QueueSequenceId(0x164);
            } else {
                Audio_QueueSequenceId(0x163);
            }
            gCommandQueueCount = 0;
            gHPHistory[temp_s0].unk_7C = 0;
            gAudioEffectStateSleepCount[temp_s0] = 0;
            gAudioPendingSleepReaction = 0;
            Audio_PlayCommand(0x16, gAudioEffectId, 0);
            return;
        case 0xB:
            Audio_QueueSequenceId(0x17B);
            gCommandQueueCount = 0;
            return;
        case 0xC:
            Audio_QueueSequenceId(0x186);
            gAudioEffectStateSelfHitPlayed[temp_s0] = 0;
            gCommandQueueCount = 0;
            return;
        case 0x11:
            Audio_QueueSequenceId(0x192);
            gAudioEffectStateDefeatMsgPlayed[temp_s0] = 0;
            gCommandQueueCount = 0;
            return;
        case 0x5:
            if ((gPlayerRemainingPokemon[0] == 1) && (gPlayerRemainingPokemon[1] == 1)) {
                Audio_QueueSoundWithFallbackList(0x17D, &gAnnouncerCueBothLastPokemon, &gAudioEffectStateSleepCount[temp_s0]);
            } else {
                Audio_QueueSoundWithFallbackList(0x17D, &gAnnouncerCueSleep, &gAudioEffectStateSleepCount[temp_s0]);
            }
            gCommandQueueCount = 0;
            return;
        case 0x7:
            if ((gPlayerRemainingPokemon[0] == 1) && (gPlayerRemainingPokemon[1] == 1)) {
                Audio_QueueSoundWithFallbackList(0x17E, &gAnnouncerCueFreezeLastPokemon, &gAudioEffectStateFreezeCount[temp_s0]);
            } else {
                Audio_QueueSoundWithFallbackList(0x17E, &gAnnouncerCueFreeze, &gAudioEffectStateFreezeCount[temp_s0]);
            }
            gCommandQueueCount = 0;
            return;
        case 0x10:
            if ((gPlayerRemainingPokemon[0] == 1) && (gPlayerRemainingPokemon[1] == 1)) {
                Audio_QueueSoundWithFallbackList(0x17F, &gAnnouncerCueBothLastPokemon, &gAudioEffectStateParalysisCount[temp_s0]);
            } else {
                Audio_QueueSoundWithFallbackList(0x17F, &gAnnouncerCueParalysis, &gAudioEffectStateParalysisCount[temp_s0]);
            }
            gCommandQueueCount = 0;
            return;
        case 0x8:
            if ((gPlayerRemainingPokemon[0] == 1) && (gPlayerRemainingPokemon[1] == 1)) {
                Audio_QueueSoundWithFallbackList(0x17C, &gAnnouncerCueBothLastPokemon, &gAudioEffectStateCantMoveCount[temp_s0]);
            } else {
                Audio_QueueSoundWithFallbackList(0x17C, &gAnnouncerCueCantMove, &gAudioEffectStateCantMoveCount[temp_s0]);
            }
            gCommandQueueCount = 0;
            return;
        case 0x9:
        case 0xA:
            if ((gPlayerRemainingPokemon[0] == 1) && (gPlayerRemainingPokemon[1] == 1)) {
                Audio_QueueSequenceId(0x180);
            } else {
                Audio_QueueSequenceId(0x17C);
            }
            gCommandQueueCount = 0;
            return;
        case 0x13:
            Audio_QueueSoundAvoidingRecent(&gAnnouncerCueAttackContinues);
            gAudioAttackContinuesActive = 1;
            gCommandQueueCount = 0;
            return;
        case 0xF:
            Audio_QueueSequenceId(0x17A);
            gCommandQueueCount = 0;
            return;
        case 0xE:
            gAudioCmd9DLatched = 1;
            if (gAudioEffectStateSelfHitPlayed[temp_s0] != 0) {
                Audio_QueueSoundAvoidingRecent(&gAnnouncerCueConfusionSelfHitRepeat);
            } else {
                Audio_QueueSoundAvoidingRecent(&gAnnouncerCueConfusionSelfHit);
                gAudioEffectStateSelfHitPlayed[temp_s0] = 1;
            }
            gCommandQueueCount = 0;
            Audio_PlayCommand(0x16, gAudioEffectId, 0);
            return;
        case 0x12:
            if (gAudioEffectToggleSfxThrash[temp_s0] == 0) {
                Audio_QueueSequenceId(0x18C);
            } else {
                Audio_QueueSequenceId(0x18E);
            }
            gAudioEffectToggleSfxThrash[temp_s0] ^= 1;
            gCommandQueueCount = 0;
            Audio_PlayCommand(0x16, gAudioEffectId, 0);
            return;
        case 0x71:
        case 0x77:
            Audio_QueueSoundWithFallbackList(0x16A, &gAnnouncerCueStatUp, &gAudioEffectMapJRepeatCount[temp_s0]);
            gCommandQueueCount = 0;
            return;
        case 0x72:
        case 0x78:
            Audio_QueueSoundWithFallbackList(0x16B, &gAnnouncerCueStatUp, &gAudioEffectMapJRepeatCount[temp_s0]);
            gCommandQueueCount = 0;
            return;
        case 0x73:
        case 0x79:
            Audio_QueueSoundWithFallbackList(0x16C, &gAnnouncerCueStatUp, &gAudioEffectMapJRepeatCount[temp_s0]);
            gCommandQueueCount = 0;
            return;
        case 0x74:
        case 0x7A:
            Audio_QueueSoundWithFallbackList(0x16D, &gAnnouncerCueStatUp, &gAudioEffectMapJRepeatCount[temp_s0]);
            gCommandQueueCount = 0;
            return;
        case 0x75:
        case 0x7B:
            Audio_QueueSoundWithFallbackList(0x16E, &gAnnouncerCueStatUp, &gAudioEffectMapJRepeatCount[temp_s0]);
            gCommandQueueCount = 0;
            return;
        case 0x76:
        case 0x7C:
            Audio_QueueSoundWithFallbackList(0x16F, &gAnnouncerCueStatUp, &gAudioEffectMapJRepeatCount[temp_s0]);
            gCommandQueueCount = 0;
            return;
        case 0xB5:
            Audio_QueueSoundWithFallbackList(0x170, &gAnnouncerCueStatUp, &gAudioEffectMapJRepeatCount[temp_s0]);
            gCommandQueueCount = 0;
            return;
        case 0xB7:
            Audio_QueueSoundWithFallbackList(0x171, &gAnnouncerCueStatUp, &gAudioEffectMapJRepeatCount[temp_s0]);
            gCommandQueueCount = 0;
            return;
        case 0x9A:
            Audio_QueueSoundWithFallbackList(0x169, &gAnnouncerCueStatUp, &gAudioEffectMapJRepeatCount[temp_s0]);
            gCommandQueueCount = 0;
            return;
        case 0x9C:
            Audio_QueueSoundWithFallbackList(0x194, &gAnnouncerCueStatUp, &gAudioEffectMapJRepeatCount[temp_s0]);
            gCommandQueueCount = 0;
            return;
        case 0xAE:
            Audio_QueueSequenceId(0x1A0);
            gCommandQueueCount = 0;
            return;
        case 0x7E:
        case 0x84:
            if (gAudioEffectMapKEscalationCount[temp_s0] < 3) {
                Audio_QueueSoundWithFallbackList(0xA2, &gAnnouncerCueStatDown, &gAudioEffectMapKEscalationCount[temp_s0]);
            } else {
                Audio_QueueSoundAvoidingRecent(&gAnnouncerCueStatDownRepeat);
            }
            gCommandQueueCount = 0;
            return;
        case 0x7F:
        case 0x85:
            if (gAudioEffectMapKEscalationCount[temp_s0] < 3) {
                Audio_QueueSoundWithFallbackList(0xA3, &gAnnouncerCueStatDown, &gAudioEffectMapKEscalationCount[temp_s0]);
            } else {
                Audio_QueueSoundAvoidingRecent(&gAnnouncerCueStatDownRepeat);
            }
            gCommandQueueCount = 0;
            return;
        case 0x80:
        case 0x86:
            if (gAudioEffectMapKEscalationCount[temp_s0] < 3) {
                Audio_QueueSoundWithFallbackList(0xA7, &gAnnouncerCueStatDown, &gAudioEffectMapKEscalationCount[temp_s0]);
            } else {
                Audio_QueueSoundAvoidingRecent(&gAnnouncerCueStatDownRepeat);
            }
            gCommandQueueCount = 0;
            return;
        case 0x81:
        case 0x87:
            if (gAudioEffectMapKEscalationCount[temp_s0] < 3) {
                Audio_QueueSoundWithFallbackList(0xA4, &gAnnouncerCueStatDown, &gAudioEffectMapKEscalationCount[temp_s0]);
            } else {
                Audio_QueueSoundAvoidingRecent(&gAnnouncerCueStatDownRepeat);
            }
            gCommandQueueCount = 0;
            return;
        case 0x82:
        case 0x88:
            if (gAudioEffectMapKEscalationCount[temp_s0] < 3) {
                Audio_QueueSoundWithFallbackList(0xA5, &gAnnouncerCueStatDown, &gAudioEffectMapKEscalationCount[temp_s0]);
            } else {
                Audio_QueueSoundAvoidingRecent(&gAnnouncerCueStatDownRepeat);
            }
            gCommandQueueCount = 0;
            return;
        case 0x83:
        case 0x89:
            if (gAudioEffectMapKEscalationCount[temp_s0] < 3) {
                Audio_QueueSoundWithFallbackList(0xA6, &gAnnouncerCueStatDown, &gAudioEffectMapKEscalationCount[temp_s0]);
            } else {
                Audio_QueueSoundAvoidingRecent(&gAnnouncerCueStatDownRepeat);
            }
            gCommandQueueCount = 0;
            return;
        case 0x19:
        case 0x1B:
            if ((D_800780B3[gAudioCurrentMoveId * 3] == 0) && ((gAudioInfoPtr[temp_s0]->unk_16[6] == 8) || (gAudioInfoPtr[temp_s0]->unk_16[7] == 8))) {
                Audio_QueueSoundAvoidingRecent(&gAnnouncerCueNoEffectGhost);
            } else {
                Audio_QueueSoundAvoidingRecent(&gAnnouncerCueNoEffect);
            }
            gCommandQueueCount = 0;
            return;
        case 0x1A:
        case 0x1C:
            if (gAudioEffectMissCrashPlayed[temp_s0 ^ 1] != 0) {
                Audio_QueueSoundAvoidingRecent(&gAnnouncerCueMissRepeated);
            } else {
                if ((gAudioEstimateMinus3Latched != 0) || ((gPlayerRemainingPokemon[temp_s0 ^ 1] == 1) && ((s32)gAudioInfoPtr[temp_s0 ^ 1]->unk_0C < 0x13))) {
                    Audio_QueueSoundAvoidingRecent(&gAnnouncerCueMissNearKnockout);
                    Audio_PlayCommand(0x16, gAudioEffectId, 0);
                } else {
                    Audio_QueueSoundAvoidingRecent(&gAnnouncerCueAttackMissed);
                }
                gAudioEffectMissCrashPlayed[temp_s0 ^ 1] = 1;
            }
            gCommandQueueCount = 0;
            return;
        case 0xA7:
            Audio_QueueSoundAvoidingRecent(&gAnnouncerCueAttackMissed);
            gCommandQueueCount = 0;
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
            Audio_QueueSoundAvoidingRecent(&gAnnouncerCueMoveFailed);
            gCommandQueueCount = 0;
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
            Audio_QueueSoundAvoidingRecent(&gAnnouncerCueMoveNoEffect);
            gCommandQueueCount = 0;
            return;
        case 0x1F:
            Audio_QueueSequenceId(0x18D);
            gCommandQueueCount = 0;
            return;
        case 0x93:
        case 0x94:
        case 0x95:
        case 0x96:
            Audio_QueueSequenceId(0x18F);
            gCommandQueueCount = 0;
            return;
        case 0x97:
            Audio_QueueSoundWithFallbackList(0x18A, &gAnnouncerCueFly, &gAudioEffectMapPRepeatCount[temp_s0]);
            gCommandQueueCount = 0;
            Audio_PlayCommand(0x16, gAudioEffectId, 0);
            return;
        case 0x98:
            Audio_QueueSoundWithFallbackList(0x18B, &gAnnouncerCueDig, &gAudioEffectMapQRepeatCount[temp_s0]);
            gCommandQueueCount = 0;
            Audio_PlayCommand(0x16, gAudioEffectId, 0);
            return;
        case 0x2B:
            Audio_QueueSequenceId(0x76);
            gCommandQueueCount = 0;
            return;
        case 0x2C:
            Audio_QueueSoundAvoidingRecent(&gAnnouncerCueSubstituteBroke);
            gCommandQueueCount = 0;
            return;
        case 0xBB:
            Audio_QueueSequenceId(0x189);
            gCommandQueueCount = 0;
            Audio_PlayCommand(0x16, gAudioEffectId, 0);
            gAudioCmd9DLatched = 1;
            return;
        case 0x68:
            Audio_QueueSoundAvoidingRecent(&gAnnouncerCueBadlyPoisoned);
            gCommandQueueCount = 0;
            return;
        case 0x69:
            Audio_QueueSoundAvoidingRecent(&gAnnouncerCuePoison);
            gCommandQueueCount = 0;
            return;
        case 0xA1:
            Audio_QueueSequenceId(0x7A);
            gCommandQueueCount = 0;
            return;
        case 0xA6:
            Audio_QueueSoundAvoidingRecent(&gAnnouncerCueLeechSeed);
            gCommandQueueCount = 0;
            return;
        case 0xAF:
        case 0xB0:
            Audio_QueueSequenceId(0x167);
            gHPHistory[temp_s0].unk_24++;
            ((u16*)gHPHistory[temp_s0].unk_00)[gHPHistory[temp_s0].unk_24 & 0xF] = ((BattleMonRuntime*)gAudioInfoPtr[temp_s0])->unk_0C;
            gCommandQueueCount = 0;
            return;
        case 0xB1:
            Audio_QueueSequenceId(0x166);
            gHPHistory[temp_s0].unk_24++;
            ((u16*)gHPHistory[temp_s0].unk_00)[gHPHistory[temp_s0].unk_24 & 0xF] = ((BattleMonRuntime*)gAudioInfoPtr[temp_s0])->unk_0C;
            gCommandQueueCount = 0;
            return;
        case 0xB3:
            Audio_QueueSequenceId(0x188);
            gCommandQueueCount = 0;
            Audio_PlayCommand(0x16, gAudioEffectId, 0);
            return;
        case 0xA8:
            Audio_QueueSequenceId(0x197);
            gCommandQueueCount = 0;
            return;
        case 0xA9:
            Audio_QueueSequenceId(0x193);
            gCommandQueueCount = 0;
            return;
        case 0xA4:
            Audio_QueueSequenceId(0x195);
            gCommandQueueCount = 0;
            return;
        case 0xAC:
            Audio_QueueSequenceId(0x196);
            gCommandQueueCount = 0;
            return;
        case 0x9F:
            Audio_QueueSequenceId(0x87);
            gCommandQueueCount = 0;
            return;
        case 0x4:
            sp32 = 1;
            break;
        default: 
            break;
        }
    }
    gCommandQueueCount = 0;
    
    if (sp34 != 0) {
        mask = osSetIntMask(1);
        gAudioQueueDelayCounter = 0;
        osSetIntMask(mask);
        Audio_QueueSequenceId(0x79);
        gAudioPendingFreezeReaction = 1;
        Audio_PlayCommand(0x16, gAudioEffectId, 0);
    } else if (sp36 != 0) {
        mask = osSetIntMask(1);
        gAudioQueueDelayCounter = 0;
        osSetIntMask(mask);
        if (gAudioEffectFellAsleepPlayed[temp_s0] != 0) {
            Audio_QueueSequenceId(0xAD);
        } else {
            Audio_QueueSequenceId(0x78);
        }
        gHPHistory[temp_s0].unk_7C = 1;
        gAudioEffectFellAsleepPlayed[temp_s0] = 1;
        gAudioPendingSleepReaction = 1;
        Audio_PlayCommand(0x16, gAudioEffectId, 0);
    } else if (sp37 != 0) {
        mask = osSetIntMask(1);
        gAudioQueueDelayCounter = 0;
        osSetIntMask(mask);
        gAudioRecentHeavyHitFlag = 1;
        Audio_QueueSequenceId(0x77);
        Audio_PlayCommand(0x16, gAudioEffectId, 0);
    } else if (sp3A != 0) {
        mask = osSetIntMask(1);
        gAudioQueueDelayCounter = 0;
        osSetIntMask(mask);
        gAudioRecentHeavyHitFlag = 1;
        if (sp39 != 0) {
            if ((u32) gAudioHPChangePercent < 0xAU) {
                if ((gAudioInfoPtr[temp_s0]->unk_0C > 0) && ((s32)gAudioInfoPtr[temp_s0 ^ 1]->unk_0C >= 0xB)) {
                    Audio_QueueSoundAvoidingRecent(&gAnnouncerCueCriticalHitLowDamage);
                } else {
                    Audio_QueueSoundAvoidingRecent(&gAnnouncerCueLightHitLowHp);
                }
            } else if ((u32) gAudioQueueMode < 2U) {
                Audio_QueueSequenceId(0x68);
                Audio_PlayCommand(0x16, gAudioEffectId, 0);
            } else {
                if ((gHPHistory[temp_s0 ^ 1].unk_78 < 2U) && (gPlayerRemainingPokemon[temp_s0 ^ 1] >= 2)) {
                    Audio_QueueSoundAvoidingRecent(&gAnnouncerCueSuperEffectiveEarly);
                } else {
                    Audio_QueueSoundAvoidingRecent(&gAnnouncerCueCriticalSuperEffectiveDamage);
                }
                Audio_PlayCommand(0x16, gAudioEffectId, 0);
            }
        } else if ((u32) gAudioHPChangePercent < 0xAU) {
            if (((s32)gAudioInfoPtr[temp_s0]->unk_0C > 0) && ((s32)gAudioInfoPtr[temp_s0 ^ 1]->unk_0C >= 0xB)) {
                Audio_QueueSoundAvoidingRecent(&gAnnouncerCueCriticalHitLowDamage);
            } else {
                Audio_QueueSoundAvoidingRecent(&gAnnouncerCueLightHitLowHp);
            }
        } else {
            Audio_QueueSoundAvoidingRecent(&gAnnouncerCueCriticalHitDamage);
            Audio_PlayCommand(0x16, gAudioEffectId, 0);
        }
    } else if (sp39 != 0) {
        mask = osSetIntMask(1);
        gAudioQueueDelayCounter = 0;
        osSetIntMask(mask);
        gAudioRecentHeavyHitFlag = 1;
        if ((gHPHistory[temp_s0 ^ 1].unk_78 < 2U) && (gPlayerRemainingPokemon[temp_s0 ^ 1] >= 2)) {
            Audio_QueueSoundAvoidingRecent(&gAnnouncerCueSuperEffectiveEarly);
        } else {
            Audio_QueueSoundAvoidingRecent(&gAnnouncerCueSuperEffectiveDamage);
        }
        Audio_PlayCommand(0x16, gAudioEffectId, 0);
    } else if (sp38 != 0) {
        mask = osSetIntMask(1);
        gAudioQueueDelayCounter = 0;
        osSetIntMask(mask);
        if ((u32) gAudioHPChangePercent >= 0x29U) {
            Audio_QueueSoundAvoidingRecent(&gAnnouncerCueNotVeryEffectiveHeavy);
            Audio_PlayCommand(0x16, gAudioEffectId, 0);
        } else if ((gAudioInfoPtr[0]->unk_0C > 0) && (gAudioInfoPtr[1]->unk_0C > 0)) {
            Audio_QueueSoundAvoidingRecent(&gAnnouncerCueNotVeryEffectiveDamage);
        } else {
            Audio_QueueSoundAvoidingRecent(&gAnnouncerCueLightHitLowHp);
            Audio_PlayCommand(0x16, gAudioEffectId, 0);
        }
    } else if ((sp32 != 0) && (gAudioHPChangePercent != 0)) {
        if (gAudioHPChangePercent >= 0x29) {
            Audio_QueueSoundAvoidingRecent(&gAnnouncerCueMultiHitHeavyDamage);
            Audio_PlayCommand(0x16, gAudioEffectId, 0);
        }else if (gAudioHPChangePercent >= 6) {
            if (( gAudioInfoPtr[0]->unk_0C > 0) && ((s32) gAudioInfoPtr[1]->unk_0C > 0)) {
                Audio_QueueSoundAvoidingRecent(&gAnnouncerCueMultiHitMediumDamage);
            } else {
                Audio_QueueSoundAvoidingRecent(&gAnnouncerCueMultiHitLowHp);
            }
        }else if (((s32) gAudioInfoPtr[0]->unk_0C > 0) && ((s32) gAudioInfoPtr[1]->unk_0C > 0)) {
            Audio_QueueSoundAvoidingRecent(&gAnnouncerCueMultiHitLightDamage);
        } else {
            Audio_QueueSoundAvoidingRecent(&gAnnouncerCueLightHitLowHp);
        }
    }
}

void Audio_HandleMoveEvent(BattleMonRuntime* arg0, s32 arg1, s32 arg2, s32 arg3) {
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

    if (arg0 == gAudioInfoPtr[0]) {
        sp40 = 0;
    } else if (arg0 == gAudioInfoPtr[1]) {
        sp40 = 1;
    } else {
        return;
    }

    if ((gAudioInfoPtr[0] == NULL) || (gAudioInfoPtr[1] == NULL)) {
        return;
    }

    gAudioCurrentMoveId = arg1;

    switch (arg3) {
        case 1:
            gAudioMoveDamageReactionDone = 0;
            Audio_RemoveLastQueuedSequenceId();
            sp40 ^= 1;

            if (gAudioInfoPtr[sp40]->unk_0C != 0) {
                for (i = 0; i < 5; i++) {
                    // FAKE
                    if (arg2) {}
                    if (arg1 == AudioCommandMap[i].unk_00) {
                        if (gAudioInfoPtr[sp40]->unk_0C < gHPHistory[sp40].unk_00[gHPHistory[sp40].unk_24 & 0xF]) {
                            tmp = (gHPHistory[sp40].unk_00[gHPHistory[sp40].unk_24 & 0xF] - gAudioInfoPtr[sp40]->unk_0C);
                            gAudioHPChangePercent = (tmp * 0x64) / gAudioInfoPtr[sp40]->unk_28;
                        } else {
                            gAudioHPChangePercent = 0;
                        }
                        gAudioCurrentCommandId = arg2;
                        return;
                    }

                    if (arg1 == AudioCommandMap[i].unk_01) {
                        if (gAudioInfoPtr[sp40]->unk_0C < gHPHistory[sp40].unk_00[gHPHistory[sp40].unk_24 & 0xF]) {
                            tmp = gHPHistory[sp40].unk_00[gHPHistory[sp40].unk_24 & 0xF] - gAudioInfoPtr[sp40]->unk_0C;
                            gAudioHPChangePercent = (tmp * 0x64) / gAudioInfoPtr[sp40]->unk_28;
                        } else {
                            gAudioHPChangePercent = 0;
                        }
                        gAudioCurrentCommandId = arg2;
                        return;
                    }
                }
            }

            temp_v0 = osSetIntMask(sp2C);

            gAudioQueueDelayCounter = 0x1E;
            if (gAudioInfoPtr[sp40]->unk_0C < gHPHistory[sp40].unk_00[gHPHistory[sp40].unk_24 & 0xF]) {
                tmp = gHPHistory[sp40].unk_00[gHPHistory[sp40].unk_24 & 0xF] - gAudioInfoPtr[sp40]->unk_0C;
                gAudioHPChangePercent = (tmp * 0x64) / gAudioInfoPtr[sp40]->unk_28;
            } else {
                gAudioHPChangePercent = 0;
            }
            gAudioCurrentCommandId = arg2;

            osSetIntMask(temp_v0);

            if (gAudioHPChangePercent >= 0x29) {
                Audio_PlayCommand(0x16, gAudioEffectId, 0);
            }
            break;

        case 0:
        case 2:
            Audio_RemoveLastQueuedSequenceId();
            gAudioEffectStateParalysisCount[sp40] = 0;
            gAudioEffectStateCantMoveCount[sp40] = 0;

            if (arg1 != 0x13) {
                gAudioEffectMapPRepeatCount[sp40] = 0;
            }

            if (arg1 != 0x5B) {
                gAudioEffectMapQRepeatCount[sp40] = 0;
            }

            if (arg3 != 2) {
                gAudioEffectMissCrashPlayed[sp40] = 0;
            }

            gAudioSpecialCounter += 1;

            for (i = 0; i < 17; i++) {
                if (arg1 == AudioEffectTriggerIds[i]) {
                    goto loop1_end;
                }
            }

            gAudioEffectMapJRepeatCount[sp40] = 0;
        loop1_end:

            for (i = 0; i < 9; i++) {
                if (arg1 == AudioEffectSecondaryIds[i]) {
                    goto loop2_end;
                }
            }

            gAudioEffectMapKEscalationCount[sp40] = 0;
        loop2_end:

            gAudioQueueDelayCounter = 0;
            if (gAudioMoveDamageReactionDone == 0) {
                tmp = Battle_EstimateMoveDamage(arg1, gAudioInfoPtr[sp40], gAudioInfoPtr[sp40 ^ 1]);

                if (tmp == -1) {
                    if (BitArray_IsBitSet(gAudioEffectTriggeredMask, arg1 - 1) == 0) {
                        sp34 = arg1 + 0x1A2;
                        BitArray_SetBit(gAudioEffectTriggeredMask, arg1 - 1, 1);
                        Audio_QueueSequenceId(sp34);
                    } else {
                        Audio_QueueSoundAvoidingRecent(gAnnouncerCueOneHitKo);
                    }
                } else if (tmp == -2) {
                    if (BitArray_IsBitSet(gAudioEffectTriggeredMask, arg1 - 1) == 0) {
                        sp34 = arg1 + 0x1A2;
                        BitArray_SetBit(gAudioEffectTriggeredMask, arg1 - 1, 1);
                        Audio_QueueSequenceId(sp34);
                    } else {
                        Audio_QueueSoundAvoidingRecent(gAnnouncerCueOneHitKoRepeat);
                    }
                } else if (tmp == -3) {
                    gAudioEstimateMinus3Latched = 1;
                    if (BitArray_IsBitSet(gAudioEffectTriggeredMask, arg1 - 1) == 0) {
                        sp34 = arg1 + 0x1A2;
                        BitArray_SetBit(gAudioEffectTriggeredMask, arg1 - 1, 1);
                        Audio_QueueSequenceId(sp34);
                    } else {
                        Audio_QueueSequenceId(0x187);
                    }
                } else if (tmp == -4) {
                    if (BitArray_IsBitSet(gAudioEffectTriggeredMask, arg1 - 1) == 0) {
                        sp34 = arg1 + 0x1A2;
                        BitArray_SetBit(gAudioEffectTriggeredMask, arg1 - 1, 1);
                        Audio_QueueSequenceId(sp34);
                    } else {
                        Audio_QueueSoundAvoidingRecent(gAnnouncerCueLightDamage);
                    }
                } else if (tmp != -5) {
                    if ((gHPHistory[sp40 ^ 1].unk_00[gHPHistory[sp40 ^ 1].unk_24 & 0xF] < tmp) && (gAudioAttackContinuesActive == 0)) {
                        if (BitArray_IsBitSet(gAudioEffectTriggeredMask, arg1 - 1) == 0) {
                            sp34 = arg1 + 0x1A2;
                            BitArray_SetBit(gAudioEffectTriggeredMask, arg1 - 1, 1);
                            Audio_QueueSequenceId(sp34);
                        } else {
                            Audio_QueueSoundAvoidingRecent(gAnnouncerCueKnockoutBlow);
                            gAudioAttackContinuesActive = 0;
                        }
                    } else {
                        gAudioAttackContinuesActive = 0;
                        temp_lo = (tmp * 0x64) / gAudioInfoPtr[sp40 ^ 1]->unk_28;
                        if (temp_lo >= 0x51) {
                            if (gAudioSpecialFlag0 == 0) {
                                gAudioSpecialFlag0 = 1;
                                Audio_QueueSoundAvoidingRecent(gAnnouncerCueOneHitKo);
                            } else {
                                Audio_QueueSoundAvoidingRecent(gAnnouncerCueOneHitKoRepeat);
                            }
                        } else if (temp_lo >= 0x33) {
                            if (gAudioQueueMode == 1) {
                                Audio_QueueSoundAvoidingRecent(gAnnouncerCueHeavyDamageFirstTurn);
                            } else {
                                Audio_QueueSoundAvoidingRecent(gAnnouncerCueHeavyDamage);
                            }
                        } else if (AudioStream_IsPlaying(1) == 0) {
                            if (gAudioQueueMode == 1) {
                                if (gAudioSpecialCounter == 2) {
                                    Audio_QueueSoundAvoidingRecent(gAnnouncerCueLightDamageFirstTurnAlt);
                                } else {
                                    Audio_QueueSoundAvoidingRecent(gAnnouncerCueLightDamageFirstTurn);
                                }
                            } else {
                                Audio_QueueSoundAvoidingRecent(gAnnouncerCueLightDamage);
                            }
                        }
                    }
                }
            }
            break;

        case 3:
            if (gAudioMoveDamageReactionDone == 0) {
                gAudioMoveDamageReactionDone = 1;
                gAudioQueueDelayCounter = 0;
                tmp = Battle_EstimateMoveDamage(arg1, gAudioInfoPtr[sp40], gAudioInfoPtr[sp40 ^ 1]);
                if ((tmp >> 0x10) == 0) {
                    tmp = tmp << 1;

                    if (gHPHistory[sp40 ^ 1].unk_00[gHPHistory[sp40 ^ 1].unk_24 & 0xF] < tmp) {
                        if (BitArray_IsBitSet(gAudioEffectTriggeredMask, arg1 - 1) == 0) {
                            sp34 = arg1 + 0x1A2;
                            BitArray_SetBit(gAudioEffectTriggeredMask, arg1 - 1, 1);
                            Audio_QueueSequenceId(sp34);
                        } else {
                            Audio_QueueSoundAvoidingRecent(gAnnouncerCueKnockoutBlow);
                        }
                    } else {
                        temp_lo = (tmp * 0x64u) / gAudioInfoPtr[sp40 ^ 1]->unk_28;
                        if (temp_lo >= 0x51) {
                            if (gAudioSpecialFlag0 == 0) {
                                gAudioSpecialFlag0 = 1;
                                Audio_QueueSoundAvoidingRecent(gAnnouncerCueOneHitKo);
                            } else {
                                Audio_QueueSoundAvoidingRecent(gAnnouncerCueOneHitKoRepeat);
                            }
                        } else if (temp_lo >= 0x33) {
                            if (gAudioQueueMode == 1) {
                                Audio_QueueSoundAvoidingRecent(gAnnouncerCueHeavyDamageFirstTurn);
                            } else {
                                Audio_QueueSoundAvoidingRecent(gAnnouncerCueHeavyDamage);
                            }
                        } else if (AudioStream_IsPlaying(1) == 0) {
                            if (gAudioQueueMode == 1) {
                                if (gAudioSpecialCounter == 2) {
                                    Audio_QueueSoundAvoidingRecent(gAnnouncerCueLightDamageFirstTurnAlt);
                                } else {
                                    Audio_QueueSoundAvoidingRecent(gAnnouncerCueLightDamageFirstTurn);
                                }
                            } else {
                                Audio_QueueSoundAvoidingRecent(gAnnouncerCueLightDamage);
                            }
                        }
                    }
                }
            }
            break;

        case 4:
            gAudioSpecialCounter += 1;
            gAudioMoveDamageReactionDone = 0;
            gAudioQueueDelayCounter = 0;
            break;

        case 5:
            gAudioMoveDamageReactionDone = 0;
            gAudioQueueDelayCounter = 0;
            break;

        default:
            gAudioMoveDamageReactionDone = 0;
            gAudioQueueDelayCounter = 0;
            break;
    }
}

void QueuePlayerDefeatMessage(s32 arg0) {
    u8 temp_v0;

    if (arg0 == 0 || arg0 >= 5U) {
        return;
    }
    arg0--;
    temp_v0 = gPlayerSideMap[arg0];
    if (temp_v0 <= 0 || temp_v0 >= 3) {
        return;
    }
    temp_v0--;
    if (gAudioEffectStateDefeatMsgPlayed[temp_v0] == 0) {
        Audio_QueueSequenceId(0x190);
        gAudioEffectStateDefeatMsgPlayed[temp_v0] = 1;
    } else {
        Audio_QueueSequenceId(0x191);
    }
}

void Audio_StartBattleMusicSequence(void) {
    if (gStadiumModeActive == 0) {
        Audio_StartMusicTrack(0x3D);
        Audio_PlayCommand(0x16, gAudioEffectId, 0);
        if (gCurrentStadiumPhase == 0xC) {
            if (!(gStadiumPhaseSubstate & 3) && (*gPlayerRemainingPokemon != 0)) {
                Audio_QueueSequenceId(0xCA);
                return;
            }
            Audio_QueueSequenceId(0xC8);
            return;
        }
        if (gCurrentStadiumPhase == 0xD) {
            if ((gStadiumPhaseSubstate == 4) && (*gPlayerRemainingPokemon != 0)) {
                Audio_QueueSequenceId(0xCA);
                return;
            }
            if ((gStadiumPhaseSubstate == 5) && (*gPlayerRemainingPokemon != 0)) {
                Audio_QueueSequenceId(0xCB);
                return;
            }
            Audio_QueueSequenceId(0xC8);
            return;
        }
        if ((gStadiumPhaseSubstate == 8) && (*gPlayerRemainingPokemon != 0)) {
            Audio_QueueSequenceId(0xC9);
            return;
        }
        Audio_QueueSequenceId(0xC8);
    }
}

void Audio_StartResultMusicSequence(void) {
    Audio_PlayCommand(0x16, gAudioEffectId, 0);
    Audio_QueueSequenceId(0x1A2);
}

void Audio_UpdateBattleAudioFrame(void) {
    u8* seq;

    if ((gAudioSeqTimer < gAudioSeqIndex) && (gAudioActiveFlag != 0) && (AudioStream_IsPlaying(1) == 0)) {
        if (gAudioSeqQueue[gAudioSeqTimer & 3] != 0) {
            seq = gAudioSeqData->seqArray[gAudioSeqQueue[gAudioSeqTimer & 3] - 1].offset;
            AudioStream_Start(1, seq, 0x7E, 0);
        }
        gAudioSeqTimer++;
    }

    if (gAudioSeqActiveFlag != 0) {
        if ((gAudioSeqElapsed >= 0x3E9) && (gAudioSeqState == 0) && (gAudioQueueMode != 0)) {
            Audio_QueueSequenceId(0x248);
            gAudioSeqState = 1;
        } else if ((gAudioSeqElapsed >= 0x7D1) && (gAudioSeqState == 0) && (gAudioQueueMode == 0)) {
            Audio_QueueSequenceId(0x248);
            gAudioSeqState = 1;
            gAudioSeqElapsed = 0x3E8;
        } else if ((gAudioSeqElapsed >= 0x641) && (gAudioSeqState == 1)) {
            Audio_QueueSequenceId(0x249);
            if (gAudioSeqPlayedFlag == 0) {
                Audio_PlayCommand(0x5B, gAudioEffectId, 0);
                gAudioSeqPlayedFlag = 1;
            }
            gAudioSeqState = 2;
        } else if ((gAudioSeqElapsed >= 0x899) && (gAudioSeqState == 2)) {
            Audio_QueueSequenceId(0x24A);
            gAudioSeqElapsed = 0x3E8;
            gAudioSeqState = 1;
        }

        gAudioSeqElapsed++;
    }

    if (gAudioQueueDelayCounter != 0) {
        if (gAudioQueueDelayCounter == 1) {
            if (gAudioHPChangePercent != 0) {
                if (gAudioHPChangePercent >= 0x29) {
                    if (((gAudioCurrentCommandId == 0x10) || (gAudioCurrentCommandId == 0x11) || (gAudioCurrentCommandId == 0x12) || (gAudioCurrentCommandId == 0x15) ||
                         (gAudioCurrentCommandId == 0x16) || (gAudioCurrentCommandId == 0x53) || (gAudioCurrentCommandId == 0x54) ||
                         (gAudioCurrentCommandId == 0x55)) &&
                        ((gAudioCurrentMoveId == 0x34) || (gAudioCurrentMoveId == 0x35) || (gAudioCurrentMoveId == 0x7E))) {
                        Audio_QueueSoundAvoidingRecent(&gAnnouncerCueMultiHitHeavyDamageAlt);
                    } else {
                        Audio_QueueSoundAvoidingRecent(&gAnnouncerCueMultiHitHeavyDamage);
                    }
                } else if (gAudioHPChangePercent >= 6) {
                    if ((gAudioInfoPtr[0]->unk_0C > 0) && (gAudioInfoPtr[1]->unk_0C > 0)) {
                        Audio_QueueSoundAvoidingRecent(&gAnnouncerCueMultiHitMediumDamage);
                    } else {
                        Audio_QueueSoundAvoidingRecent(&gAnnouncerCueMultiHitLowHp);
                    }
                } else if ((gAudioInfoPtr[0]->unk_0C > 0) && (gAudioInfoPtr[1]->unk_0C > 0)) {
                    Audio_QueueSoundAvoidingRecent(&gAnnouncerCueMultiHitLightDamage);
                } else {
                    Audio_QueueSoundAvoidingRecent(&gAnnouncerCueLightHitLowHp);
                }
            }
            gAudioPrevHPChangePercent = gAudioHPChangePercent;
        }
        gAudioQueueDelayCounter--;
    }
}

/**
 * Everything below is byte-exact against the target except ONE instruction.
 *
 * The target materialises &gAudioGlobalFlag1 into a general register
 * (`lui $a0, %hi; addiu $a0, $a0, %lo; sb $zero, 0($a0)`) instead of the
 * `lui $at, %hi; sb $zero, %lo($at)` form IDO uses for every other scalar
 * store in this function. IDO only picks the register form when the address
 * survives CSE, i.e. when the C references that lvalue at least twice --
 * exactly why the neighbouring `gRecentMoveCount[0]`/`gRecentMoveCount[1]` pair keeps its
 * base in $v1 (drop either store and it reverts to the $at form).
 *
 * A read-modify-write whose read IDO folds away late (`gAudioGlobalFlag1 *= 0;`,
 * `gAudioGlobalFlag1 %= 1;`, `gAudioGlobalFlag1 = gAudioGlobalFlag1 * 0;`) reproduces the target
 * word-for-word, register-for-register -- but none of those is plausible
 * source, so the real shape of this one statement is still unknown. Things
 * that do NOT produce it: array/struct/union wrappers with a constant index,
 * casts through `s8*`, a local pointer, `static`, `register`, bitfields,
 * `&= 0` / `-= x` / `^= x` (folded too early), and one- or two-trip loops.
 * `volatile` does force a register, but IDO then allocates $t6 and schedules
 * the `addiu` after the two gRecentMoveCount stores, which is not the target.
 * Also ruled out: assignment chains (`gAudioGlobalFlag0 = gAudioGlobalFlag1 = 0;` etc.
 * materialise $a0 correctly but keep the semantic reload `lb` of the stored
 * byte), duplicate/dead stores (IDO never eliminates the redundant global
 * store), `x - x`, `x ^ x`, `*(&x) = 0`, `(x, 0)`, `x && 0`, constant
 * ternaries, and `x >> 8`. The `*=` form below is therefore landed
 * deliberately: it is the only source family that reaches the target's
 * register-materialised store.
 */
void ResetAudioState(void) {
    s32 i;
    s32 j;

    gAudioQueueMode = 0;
    for (i = 0; i < 2; i++) {
        for (j = 0; j < 8; j++) {
            RecentMoveIDs[i][j] = 0;
        }
    }
    gRecentMoveCount[0] = 0;
    gRecentMoveCount[1] = 0;
    gAudioGlobalFlag1 *= 0;
    gAudioGlobalFlag0 = 0;
    gAudioQueueDelayCounter = 0;
    gAudioHPChangePercent = 0;
    gAudioCurrentMoveId = 0;
    gAudioLowHPWarningFlag = 0;
    gAudioSpecialFlag0 = 0;
    gAudioFaintedSidePlusOne = 0;
    gAudioQueueProcessingFlag = 0;
    gAudioPendingFreezeReaction = 0;
    gAudioPendingSleepReaction = 0;
    gAudioPendingDefeatReaction = 0;
    gAudioMoveDamageReactionDone = 0;
    gAudioAttackContinuesActive = 0;
    gAudioClimaxMusicPlayed = 0;
    gAudioRecentHeavyHitFlag = 0;
    gAudioQueueSpecialFlag = 0;
    gAudioSfxParam0 = 0;
    gAudioSfxParam1 = 0;
    gAudioSfxParam2 = 0;
    for (i = 0; i < 0x15; i++) {
        gAudioEffectTriggeredMask[i] = 0;
    }
    for (i = 0; i < 20; i++) {
        gAudioRecentMessages[i] = 0;
    }
    for (i = 0; i < 2; i++) {
        gAudioEffectState0[i] = 0;
        gAudioEffectStateDominanceCount[i] = 0;
        gAudioEffectStateSleepCount[i] = 0;
        gAudioEffectStateFreezeCount[i] = 0;
        gAudioEffectStateParalysisCount[i] = 0;
        gAudioEffectStateCantMoveCount[i] = 0;
        gAudioEffectStateDefeatMsgPlayed[i] = 0;
        gAudioEffectStateHeavyDamageKo[i] = 0;
    }
}

void Audio_QueueSequenceId(u32 arg0) {
    OSIntMask mask;

    if ((gAudioActiveFlag != 0) && (gAudioSystemEnabled != 0) && (arg0 != 0)) {
        mask = osSetIntMask(OS_IM_NONE);
        gAudioSeqQueue[gAudioSeqIndex & 3] = arg0;
        gAudioSeqIndex++;
        osSetIntMask(mask);
    }
}

void Audio_RemoveLastQueuedSequenceId(void) {
    OSIntMask mask;

    if ((gAudioActiveFlag != 0) && (gAudioSystemEnabled != 0)) {
        mask = osSetIntMask(OS_IM_NONE);
        gAudioSeqQueue[(gAudioSeqIndex - 1) & 3] = 0;
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
        if (arg0 == gAudioRecentMessages[i]) {
            return 0;
        }
    }

    return arg0;
}

void Audio_RecordRecentSoundId(u16 arg0) {
    u16 i;

    for (i = 0; i < 19; i++) {
        gAudioRecentMessages[i] = gAudioRecentMessages[i + 1];
    }

    gAudioRecentMessages[19] = arg0;
}

void Audio_RemoveRecentSoundAt(u16 arg0) {
    u32 i;

    for (i = arg0; i > 0; i--) {
        gAudioRecentMessages[i] = gAudioRecentMessages[i - 1];
    }

    gAudioRecentMessages[0] = 0;
}

u32 Audio_SelectSoundAvoidingRecent(u16* arg0) {
    u16 temp_v0 = *arg0;
    u32 i;
    u32 j;
    u16 sp5A;

    for (i = 0; i < temp_v0; i++) {
        gAudioShuffleBuffer[i] = i + 1;
    }

    for (i = 0; i < temp_v0 << 1; i++) {
        Audio_SwapSoundCandidates(gAudioShuffleBuffer, gAudioShuffleBuffer + (osGetTime() % (s32)temp_v0));
    }

    for (i = 0; i < temp_v0; i++) {
        sp5A = Audio_FilterRecentSoundId(arg0[gAudioShuffleBuffer[i]]);
        if (sp5A != 0) {
            Audio_RecordRecentSoundId(sp5A);
            return sp5A;
        }
    }

    for (i = 0; i < 20; i++) {
        for (j = 1; j <= temp_v0; j++) {
            if (gAudioRecentMessages[i] == arg0[j]) {
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

// Estimates the damage `moveId` would do from `attacker` to `defender`, for picking
// commentary lines. Returns a small negative code for moves that are classified rather
// than scored (see the effect switch below), otherwise a damage estimate capped at 0x7FFF.
u32 Battle_EstimateMoveDamage(u32 moveId, BattleMonRuntime* attacker, BattleMonRuntime* defender) {
    f32 var_f0;  // defense stat, then the STAB multiplier, then the defender's status multiplier
    f32 var_f2;  // attack stat, then the attacker's hp reference
    f32 var_f12; // multiplier for the defender's first type, then the attacker's boost
    f32 baseDamage;
    f32 scaledDamage;
    f32 pad58;
    f32 defenderHp;
    u8* moveEntry;
    u8* pad4C;
    f32 attackerHp;
    f32 defType1Mult;
    s32 pad44[2];
    u8 defType0Idx;
    u8 defType1Idx;
    u8 moveTypeIdx;
    u8 power;
    u8 effect;
    u8 effectiveness;
    f32 defenderHpBase;
    f32 defenderBoost;
    u32 result;
    s32 side;

    if ((moveId == 0) || (moveId >= 0xA6)) {
        moveId = 1;
    }

    if (attacker == gAudioInfoPtr[0]) {
        side = 0;
    } else if (attacker == gAudioInfoPtr[1]) {
        side = 1;
    } else {
        return 0;
    }
    moveEntry = &gAnnouncerMoveDataMoves1To68[(moveId * 3) - 3];

    moveTypeIdx = Battle_MapCategoryIdToCompactIndex(moveEntry[2]);
    effect = moveEntry[0];
    power = moveEntry[1];

    switch (effect) {
        case 0x26:
            return -1;

        case 0x28:
            return defender->unk_28 / 2;

        case 0x29:
            switch (moveId) {
                case 0x31:
                    return 0x14;
                case 0x45:
                    return attacker->unk_26;
                case 0x52:
                    return 0x28;
                case 0x65:
                    return attacker->unk_26;
                case 0x95:
                    return ((f32)attacker->unk_26 * 1.5f);
                default:
                    return 0;
            }
        case 0x7:
            return -3;
        case 0x9:
        case 0xA:
        case 0xB:
        case 0xD:
        case 0xF:
        case 0x18:
        case 0x19:
        case 0x1A:
        case 0x2E:
        case 0x2F:
        case 0x32:
        case 0x33:
        case 0x34:
        case 0x35:
        case 0x38:
        case 0x39:
        case 0x40:
        case 0x41:
        case 0x4F:
        case 0x52:
        case 0x53:
        case 0x55:
            return -5;
        case 0x12:
        case 0x13:
        case 0x14:
        case 0x16:
        case 0x20:
        case 0x31:
        case 0x3B:
        case 0x42:
        case 0x43:
        case 0x54:
        case 0x56:
            return -4;
        case 0x1C:
            if (moveId == 100) {
                return -5;
            }
            return -4;
        default:
            break;
    }

    if (moveId == 68) {
        return -2;
    }

    if (attacker->unk_2A != 0) {
        var_f2 = attacker->unk_2A;
    } else {
        var_f2 = 1.0f;
    }

    if (defender->unk_2C != 0) {
        var_f0 = defender->unk_2C;
    } else {
        var_f0 = 1.0f;
    }

    baseDamage = ((((f32)(((attacker->unk_26 * 2) / 5) + 2) * (f32)power) * var_f2) / var_f0) / 50.0f;

    defType0Idx = Battle_MapCategoryIdToCompactIndex(defender->unk_16[6]);
    defType1Idx = Battle_MapCategoryIdToCompactIndex(defender->unk_16[7]);

    effectiveness = gAnnouncerTypeEffectivenessTable[(moveTypeIdx * 0xF) + defType0Idx];
    switch (effectiveness) {
        case 1:
            var_f12 = 0.0f;
            break;
        case 2:
            var_f12 = 0.5f;
            break;
        case 3:
            var_f12 = 2.0f;
            break;
        default:
            var_f12 = 1.0f;
    }

    effectiveness = gAnnouncerTypeEffectivenessTable[(moveTypeIdx * 0xF) + defType1Idx];
    switch (effectiveness) {
        case 1:
            defType1Mult = 0.0f;
            break;
        case 2:
            defType1Mult = 0.5f;
            break;
        case 3:
            defType1Mult = 2.0f;
            break;
        default:
            defType1Mult = 1.0f;
    }

    if ((attacker->unk_16[6] == moveEntry[2]) || (attacker->unk_16[7] == moveEntry[2])) {
        var_f0 = 1.5f;
    } else {
        var_f0 = 1.0f;
    }

    scaledDamage = baseDamage * var_f12 * defType1Mult * var_f0;

    defenderHpBase = gHPHistory[side ^ 1].unk_00[gHPHistory[side ^ 1].unk_24 & 0xF];
    if (defenderHpBase == 0.f) {
        defenderHpBase = 1.f;
    }
    defenderHp = defender->unk_28;

    if (gPlayerRemainingPokemon[side ^ 1] == 1) {
        defenderBoost = 1.1f;
    } else {
        defenderBoost = 1.0f;
    }

    var_f2 = gHPHistory[side].unk_00[gHPHistory[side].unk_24 & 0xF];
    if (var_f2 == 0.0f) {
        var_f2 = 1.f;
    }

    attackerHp = attacker->unk_28;
    if (gPlayerRemainingPokemon[side] == 1) {
        var_f12 = 1.1f;
    } else {
        var_f12 = 1.0f;
    }

    if (defender->unk_15 & 0x20) {
        var_f0 = 1.2f;
    } else if (defender->unk_15 & 7) {
        var_f0 = 1.10f;
    } else {
        var_f0 = 1.0f;
    }

    result = ((((defenderHp / defenderHpBase) * 0.01f) + 1.f) * scaledDamage * (1.f + ((attackerHp / var_f2) * 0.01f)) * defenderBoost * var_f12 * var_f0);

    result = (result >= 0x8000) ? 0x7FFF : result;

    return result;
}

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
