#ifndef _FRAGMENT8_H_
#define _FRAGMENT8_H_

#include "global.h"

typedef struct RattataHurdleMarker {
    /* 0x000 */ s16 activeFlag;
    /* 0x002 */ s16 markerState;
    /* 0x004 */ unk_D_86002F58_004_000 unk_004;
    /* 0x16C */ s32 unk_16C;
    /* 0x170 */ s16 unk_170;
    /* 0x172 */ s8 unk_172[6];
    /* 0x178 */ s16 flashCounter;
    /* 0x17A */ u16 unk_17A;
} RattataHurdleMarker; // size = 0x17C

typedef struct RattataRacer {
    /* 0x000 */ s16 racerIndex;
    /* 0x002 */ s16 aiDifficulty;
    /* 0x004 */ char pad4[4];
    /* 0x008 */ unk_D_86002F58_004_000 unk_008;
    /* 0x170 */ u16 inputFlags;
    /* 0x172 */ char pad172[2];
    /* 0x174 */ f32 targetSpeed;
    /* 0x178 */ f32 currentSpeed;
    /* 0x17C */ f32 minSpeedThreshold;
    /* 0x180 */ f32 topSpeed;
    /* 0x184 */ f32 minSpeed;
    /* 0x188 */ f32 accelRate;
    /* 0x18C */ f32 prevTrackPosition;
    /* 0x190 */ f32 trackPosition;
    /* 0x194 */ f32 raceProgressOffset;
    /* 0x198 */ f32 finishTime;
    /* 0x19C */ s16 runAnimState;
    /* 0x19E */ s16 racePhase;
    /* 0x1A0 */ s16 finishedFlag;
    /* 0x1A2 */ s16 unk_1A2;
    /* 0x1A4 */ f32 hurdleDistance;
    /* 0x1A8 */ f32 racerZPosition;
    /* 0x1AC */ s16 unk_1AC;
    /* 0x1AE */ s16 unk_1AE;
    /* 0x1B0 */ s16 unk_1B0;
    /* 0x1B2 */ s16 rankPlace;
    /* 0x1B4 */ s16 unk_1B4;
    /* 0x1B6 */ char pad1B6[0x12];
    /* 0x1C8 */ u16 celebratingFlag;
    /* 0x1CA */ char pad1CA[6];
} RattataRacer; // size = 0x1D0

extern u32 D_862082A0[4];

void RattataGame_InitAIDifficulty(RattataRacer* arg0);
void RattataGame_DrawRankBanners(void);
void func_862002EC(void);
void RattataGame_UpdateAIInput(void);
void RattataGame_PollInput(void);
void RattataGame_InitCamera(void);
void RattataGame_UpdateCamera(void);
void func_86200938(void);
void RattataGame_DrawTutorialScreen(s32 arg0);
void RattataGame_DrawFrame(s32 arg0);
s32 RattataGame_WaitForStart(void);
void RattataGame_UpdateStartCountdown(void);
void RattataGame_SyncRacerZPosition(RattataRacer* arg0);
void RattataGame_ResetRacerState(RattataRacer* arg0);
s32 RattataGame_CheckHurdleCollision(RattataRacer* arg0, RattataHurdleMarker* arg1);
void RattataGame_UpdateRunAnimSpeed(RattataRacer* arg0);
void RattataGame_SetRacerAnimState(RattataRacer* arg0, s32 arg1);
void RattataGame_UpdateHurdleMarkers(RattataRacer* arg0);
s16 RattataGame_UpdateRacers(void);
void RattataGame_AdvanceFinishLineApproach(void);
void RattataGame_MainLoop(void);
void RattataGame_UpdatePostRaceIdleAnim(void);
void RattataGame_PlayResultAnimation(void);
void RattataGame_WaitForAudioToFinish(void);
void RattataGame_FadeOutAndTransition(void);
void RattataGame_PlayOutroSequence(s32 arg0);
void RattataGame_WaitForContinue(void);
void RattataGame_LoadAssets(void);
void RattataGame_InitPlayerSlots(void);
void RattataGame_Main(s32 arg0, UNUSED s32 arg1);

#endif // _FRAGMENT8_H_
