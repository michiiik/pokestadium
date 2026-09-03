#ifndef _FRAGMENT21_H_
#define _FRAGMENT21_H_

#include "global.h"
#include "src/fragments/minigame_actor_framework/minigame_actor_framework.h"

void SandshrewGame_PlaySoundEvent(s16 arg0, s16 arg1);
void SandshrewGame_PlaySoundEventGuarded(s16 arg0, s16 arg1);
void initSandshrew(MiniActor* sandshrew, s32 player);
void initSandshrews(void);
void miniSandshewReangle(MiniActor* sandshrew);
void miniSandshewReangleAll(void);
void miniSandshewChangeAnim(MiniActor* actor, s32 animID);
void sandshrewDecreaseVel(MiniActor* arg0, f32 arg1);
void sandshrewChangeState(MiniActor* sandshrew, s32 arg1);
void sandshrewHumanControls(MiniActor* sandshrew);
void sandshrewCompControls(MiniActor* arg0);
void SandshrewGame_DigStateMachine(MiniActor* sandshrew);
void SandshrewGame_UpdatePlayers(void);
void initSandshrewHole(MiniActor* a0, s32 arg1);
void initSandshrewHoles(void);
void SandshrewGame_UpdateHole(MiniActor* sandshrewHole, MiniActor* sandshrewPlayer);
void SandshrewGame_UpdateHoles(void);
void initSandshrewWaterGeiser(MiniActor* arg0, s32 arg1);
void initSandshrewGeisers(void);
void SandshrewGame_UpdateGeiser(MiniActor* geiser);
void SandshrewGame_UpdateGeisers(void);
void SandshrewGame_InitCamera(void);
void SandshrewGame_UpdateWinCamera(void);
void SandshrewGame_CameraControls(void);
void sandshrewMinigameInitObjects(void);
s32 SandshrewGame_CheckWinCondition(void);
void SandshrewGame_ResetDigSpeeds(void);
s32 SandshrewGame_UpdateStartCountdown(void);
void SandshrewGame_MinigameStateMachine(void);
void SandshrewGame_SyncModelTransforms(void);
void SandshrewGame_DrawTutorialText(void);
void SandshrewGame_DrawTutorialScreen(void);
void fixSandshrewMinigameHUD(void);
void SandshrewGame_DrawTutorial(void);
void SandshrewGame_DrawPlayerHUBs(s32 arg0);
void SandshrewGame_DrawFrame(s32 arg0);
void sandshrewMinigameInit(void);
void SandshrewGame_MainLoop(void);
void SandshrewGame_ShowResultText(void);
void SandshrewGame_LoadAssets(void);
s32 sandshrewMinigameLoad(s32 arg0, UNUSED s32 arg1);

#endif // _FRAGMENT21_H_
