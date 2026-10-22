#include "minigame_shared_library.h"
#include "src/input.h"
#include "src/controller.h"

u16 gDrowzeeFrameCounter = 0;
u16 gDrowzeeStartCountdown = 0x0069;
u8 gDrowzeeGameIsOver = 0;
u8 gDrowzeePauseTriggered = 0;
u8 gDrowzeeTutorialPage = 0;
u8 gDrowzeeWinnerCount = 0;

FontContext* gCurrentTextObject;
ModelData* gDrowzeeModelTemp;
ShadowContextNode* gDrowzeeStageRoot;
CameraNode* gDrowzeeCamera;
s16 gDrowzeeCamDist;
s16 gDrowzeeCamRotationY;
s16 gDrowzeeCamDistance;

void MiniGame_ReadInputs(void) {
    Cont_StartReadInputs();
    Cont_ReadInputs();
    Input_ResetRepeatState();
}

u32 MiniGame_RandomRange(u32 arg0) {
    if (arg0 == 0) {
        return 0;
    }
    return ((guRandom() + osGetCount()) / 10) % arg0;
}
