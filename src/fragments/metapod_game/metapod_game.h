#ifndef _FRAGMENT20_H_
#define _FRAGMENT20_H_

#include "global.h"
#include "src/fragments/3/minigame_actor_framework.h"

void MetapodGame_PlaySoundEvent(s16 arg0, s16 arg1);
void MetapodGame_PlaySoundEventGuarded(s16 arg0, s16 arg1);
void miniInitMetapodPlayer(MiniActor* metapod, s32 arg1);
void miniInitMetapodPlayers(void);
void MetapodGame_BeginHarden(MiniActor* metapod);
void miniMetapodHumanControls(MiniActor* metapod);
void MetapodGame_ResetCompState(MiniActor* metapod);
s32 MetapodGame_ChooseRockDelayMode(MiniActor* arg0, s32 arg1);
void MetapodGame_ComputeRockDelay_Type0(MiniActor* metapod, s32 arg1);
void MetapodGame_ComputeRockDelay_Type1(MiniActor* arg0, s32 arg1);
void MetapodGame_ComputeRockDelay_Type2(MiniActor* metapod, s32 arg1);
void MetapodGame_CompDecideRockAvoidance(MiniActor* metapod, s32 player);
void miniMetapodCompControls(MiniActor* compMetapod, s32 nPlayer);
void miniUpdateMetapods(void);
s32 metapodRockCollisionCheck_void(void);
s32 MetapodGame_ResolveRockHit(s32 arg0);
void miniRockChecks(MiniActor* arg0, s32 nPlayer);
void miniMatapodChecks(MiniActor* metapod, s32 nPlayer);
void miniMatapodMinigameChecks(void);
void MetapodGame_SyncMetapodModelTransform(void);
void MetapodGame_SyncAllMetapodModelTransforms(void);
void miniInitRock(MiniActor* rock, s32 arg1);
void miniInitRocks(void);
void MetapodGame_ScheduleRockWave(void);
void MetapodGame_LaunchRock(MiniActor* rock);
void MetapodGame_DespawnRock(MiniActor* rock);
void miniRockStateMachine(MiniActor* rock);
void miniRockUpdatePosition(MiniActor* rock);
void miniUpdateRockTransform(MiniActor* rock);
void miniUpdateRocks(void);
void MetapodGame_SyncRockModelTransform(MiniActor* arg0);
void miniUpdateRocksPositions(void);
void MetapodGame_InitCamera(void);
void miniMetapodCameraControls(void);
void initMetapodMinigameAssets(void);
void MetapodGame_UpdateRockWaveScheduler(void);
s32 MetapodGame_CheckWinCondition(void);
s32 MetapodGame_PlayWinnerAnimations(void);
void func_86E01EB0(void);
s32 MetapodGame_UpdateStartCountdown(void);
s32 MetapodGame_AllRocksCleared(void);
void miniMetapodMinigameStateMachine(void);
void MetapodGame_UpdateParticleFx(void);
void miniDrawMetapodHealth(void);
void miniDrawMetapodHeads(void);
void MetapodGame_DrawTutorialText(void);
void MetapodGame_DrawTutorialScreen(void);
void MetapodGame_DrawTutorial(void);
void miniMetapodDrawPlayerHUBs(s32 arg0);
void MetapodGame_DrawFrame(s32 arg0);
void metapodMinigameInit(void);
void func_86E0296C(void);
void miniMetapodTutoScreenControls(void);
void MetapodGame_LockPlayerInputOnPress(void);
void MetapodGame_MainLoop(void);
void MetapodGame_ShowResultText(void);
void MetapodGame_LoadAssets(void);
s32 metapodMinigameLoad(s32 arg0, s32 arg1);

#endif // _FRAGMENT20_H_
