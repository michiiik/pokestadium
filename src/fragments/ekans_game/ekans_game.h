#ifndef _FRAGMENT18_H_
#define _FRAGMENT18_H_

#include "global.h"
#include "src/fragments/minigame_actor_framework/minigame_actor_framework.h"

s32 EkansGame_GetLaunchForceTier(MiniActor* arg0);
void miniDiglettGoldCheck(s16 arg0, s32* arg1, s32* arg2);
void EkansGame_PlaySoundEvent(s16 arg0, s16 arg1);
void EkansGame_PlaySoundEventGuarded(s16 arg0, s16 arg1);
void EkansGame_ResetEkansToSpawn(MiniActor* arg0);
void initEkans(MiniActor* ekans, s16 player);
void initAllEkans(void);
f32 EkansGame_GetPeakStickMagnitude(MiniActor* arg0);
s16 EkansGame_ClampAimRotation(MiniActor* arg0, s16 arg1);
void miniEkansHumanControls(MiniActor* ekans);
void EkansGame_ActivateCompPlayers(void);
s32 EkansGame_CompShouldRetargetGold(MiniActor* arg0);
s16 EkansGame_PickRandomActiveDiglett(void);
s16 EkansGame_ChooseTargetPreferGold(MiniActor* arg0);
s16 EkansGame_ChooseTargetAvoidOverlap(MiniActor* arg0);
s32 EkansGame_CompChooseTarget(MiniActor* arg0);
void EkansGame_CompAimAtDiglett(MiniActor* compEkans);
void miniEkansCompGetForce(MiniActor* compEkans);
s16 EkansGame_CompGetAimHoldFrames(void);
s16 EkansGame_CompGetLaunchHoldFrames(void);
void miniEkansCompControls(MiniActor* compEkans);
void EkansGame_LaunchEkans(MiniActor* ekans);
void func_86C01748(void);
void ekansFixDirToDiglett(MiniActor* arg0, s32 arg1, s32 arg2, f32 arg3, f32 arg4, f32 arg5);
void checkCollitionWithDigletts(MiniActor* ekans, s32 nPlayer);
void EkansGame_ResolveHoopContention(void);
void EkansGame_CaptureDiglett(MiniActor* ekans, s32 nPlayer);
void miniGetCloser2Diglett(MiniActor* ekans, s32 nPlayer);
void EkansGame_ApplyPullbackOffset(MiniActor* ekans);
void miniEkansAnycontrols(MiniActor* ekans);
void miniEkansPlayerStateMachine(MiniActor* ekans, s32 nPlayer);
void func_86C0250C(void);
void miniEkansPlayersUpdate(void);
void initDiglett(MiniActor* diglett, s32 nDiglett);
void initDiglettHole(MiniActor* hole, s32 arg1);
void resetActiveDigletts(void);
void func_86C027A4(void);
void func_86C027AC(void);
void func_86C027B4(void);
void resetDiglettsForRoundStart(void);
void initDigletts(void);
s16 miniEkansChooseRandDiglett(s16 arg0);
void EkansGame_RollDiglettGoldChance(MiniActor* diglett);
void miniEkansRecolorDiglet(MiniActor* diglett, s32 nActiveDiglett);
void miniDiglettStateMachine(void);
void EkansGame_UpdateDigletts(void);
void ekansMinigameInitObjects(void);
s32 EkansGame_AllEkansIdle(void);
void EkansGame_DetermineWinners(void);
s32 EkansGame_UpdateStartCountdown(void);
s32 EkansGame_UpdateMainCountdown(void);
void miniEkansMinigameStateMachine(void);
void func_86C034F8(void);
void miniEkansInitCam(void);
void EkansGame_CameraControls(void);
void EkansGame_DrawTutorialText(void);
void EkansGame_DrawTutorialScreen(void);
void miniEkansDrawCountdown(void);
void miniEkansDrawHub(void);
void EkansGame_DrawTutorial(void);
void EkansGame_DrawPlayerHUBs(s32 arg0);
void EkansGame_DrawShadows(void);
void EkansGame_DrawFrame(s32 arg0);
void initEkansMinigameAssets(void);
void miniEkansTutoScreenControls(void);
void ekansMinigameUpdate(void);
void EkansGame_LoadAssets(void);
s32 ekansMinigameLoad(s32 arg0, UNUSED s32 arg1);

#endif // _FRAGMENT18_H_
