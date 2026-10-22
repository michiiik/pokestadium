#ifndef _FRAGMENT5_H_
#define _FRAGMENT5_H_

#include "global.h"

extern u16 gDrowzeeFrameCounter;
extern u16 gDrowzeeStartCountdown;
extern u8 gDrowzeeGameIsOver;
extern u8 gDrowzeePauseTriggered;
extern u8 gDrowzeeTutorialPage;
extern u8 gDrowzeeWinnerCount;

extern FontContext* gCurrentTextObject;
extern ModelData* gDrowzeeModelTemp;
extern ShadowContextNode* gDrowzeeStageRoot;
extern CameraNode* gDrowzeeCamera;
extern s16 gDrowzeeCamDist;
extern s16 gDrowzeeCamRotationY;
extern s16 gDrowzeeCamDistance;

void MiniGame_ReadInputs(void);
u32 MiniGame_RandomRange(u32);


#endif // _FRAGMENT5_H_
