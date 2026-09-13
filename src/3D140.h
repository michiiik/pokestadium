#ifndef _3D140_H_
#define _3D140_H_

#include "global.h"

typedef struct CachedMoveData {
    /* 0x00 */ u8 moveId;
    /* 0x01 */ u8 effectId;
    /* 0x02 */ u8 power;
    /* 0x03 */ u8 type;
    /* 0x04 */ u8 accuracy;
    /* 0x05 */ u8 basePP;
} CachedMoveData; // size = 0x6

typedef struct BattleMonRuntime {
    /* 0x00 */ char unk00[0x1];
    /* 0x01 */ u8 unk_01;
    /* 0x02 */ char unk02[0x3];
    /* 0x05 */ u8 unk_05;
    /* 0x06 */ char unk06[0x5];
    /* 0x0B */ u8 speciesId;
    /* 0x0C */ u16 currentHP;
    /* 0x0E */ u16 poisonBurnPendingHP;
    /* 0x10 */ u16 leechSeedPendingHP;
    /* 0x12 */ u16 recoilPendingHP;
    /* 0x14 */ u8 partySlotIndex;
    /* 0x15 */ u8 status;
    /* 0x16 */ u8 unk_16[9];
    /* 0x1F */ u8 moveIds[4];
    /* 0x23 */ char unk23[0x1];
    /* 0x24 */ u16 dvs;
    /* 0x26 */ u8 level;
    /* 0x28 */ u16 maxHP;
    /* 0x2A */ u16 attack;
    /* 0x2C */ u16 defense;
    /* 0x2E */ u16 speed;
    /* 0x30 */ u16 special;
    /* 0x32 */ u8 currentPP[4];
    /* 0x36 */ u16 baseHP;
    /* 0x38 */ u16 origAttack;
    /* 0x3A */ u16 origDefense;
    /* 0x3C */ u16 origSpeed;
    /* 0x3E */ u16 origSpecial;
    /* 0x40 */ char unk40[0x4];
    /* 0x44 */ CachedMoveData cachedMove;
    /* 0x4A */ u8 unk_4A;
    /* 0x4B */ u8 unk_4B;
    /* 0x4C */ u8 lockedEffectFlags;
    /* 0x4D */ u8 volatileStatusFlags;
    /* 0x4E */ u8 auxStatusFlags;
    /* 0x4F */ u8 lockedEffectCounter;
    /* 0x50 */ u8 confusionCounter;
    /* 0x51 */ u8 toxicCounter;
    /* 0x52 */ u8 disabledSlotAndTurns;
    /* 0x53 */ u8 unk_53;
    /* 0x54 */ u16 effectAccumulator;
    /* 0x56 */ u8 statChangeFlag;
    /* 0x57 */ u8 disabledMoveId;
    /* 0x58 */ u8 lastMoveUsedId;
    /* 0x59 */ u8 substituteHP;
    /* 0x5A */ u8 currentMoveId;
    /* 0x5B */ u8 lastCritFlag; // Battle_ResetTransientRuntimeState: mirrors gBattleCritFlag
    /* 0x5C */ u8 statStages[6];
    /* 0x62 */ char unk62[0x2];
} BattleMonRuntime; // size = 0x64

typedef struct amConfig {
    /* 0x00 */ u32 outputRate;
    /* 0x04 */ u32 framesPerField;
    /* 0x08 */ u32 maxACMDSize;
} amConfig; // size = 0xC

extern s32 D_80077DC4;
extern f32 D_80077DC8;
extern u8 D_80077DCC;
extern f32 D_80077DD0;
extern f32 D_80077DD4;
extern f32 D_80077DD8;
extern f32 D_80077DDC;
extern f32 D_80077DE0;
extern u8 D_80077DE4;

extern BattleMonRuntime* D_800FCB18[2];

void amCreateAudioMgr(ALSynConfig*, amConfig*, u32, s32, s32);
OSTask* Audio_CreateFrameTask(OSTask*);
void __clearAudioDMA(void);
void Cry_PlayFromPokedex(s32);
void Cry_UpdatePlaybackState(void);
s32 Cry_IsPlaying(void);
s32 Audio_SetEnabled(s32);
void Audio_SetVoiceEnabled(s32);
void Audio_SetDiskReadyPolling(s32 arg0);
s32 Audio_GetDiskReadyStatus(u8* arg0);
void BitArray_SetBit(u8* array, u8 index, u8 flag);
s32 BitArray_IsBitSet(u8* array, u8 index);
void Audio_ResetChannelGroupState(u8 arg0);
void Audio_SetChannelGroupCounts(u8, u8, u8, u8);
void Audio_ConfigureChannelGroups(u8, u8, u8, u8);
void Audio_SelectModeMusic(u32 arg0, u32 arg1);
void Audio_ResetSequencePlaybackState(s32 arg0);
void func_8003EB40(s32, BattleMonRuntime*);
void func_8003F1AC(s32);
void func_8003F4C0(s32);
void Audio_QueuePendingSoundId(u32 arg0);
void func_8003F660(s32);
void func_80040A70(BattleMonRuntime* arg0, s32 arg1, s32 arg2, s32 arg3);
void func_800414B8(s32);
void Audio_StartBattleMusicSequence(void);
void Audio_StartResultMusicSequence(void);
void Audio_UpdateBattleAudioFrame(void);
void Audio_ResetBattleAudioState(void);
void Audio_RemoveLastQueuedSequenceId(void);
void Audio_QueueSequenceId(u32 arg0);
void Audio_SwapSoundCandidates(u8* arg0, u8* arg1);
u16 Audio_FilterRecentSoundId(u16 arg0);
void Audio_RecordRecentSoundId(u16 arg0);
void Audio_RemoveRecentSoundAt(u16 arg0);
u32 Audio_SelectSoundAvoidingRecent(u16* arg0);
void Audio_QueueSoundAvoidingRecent(u16* arg0);

#endif // _3D140_H_
