#ifndef _FRAGMENT4_H_
#define _FRAGMENT4_H_

#include "global.h"

extern u32 gMinigameSharedStrings[];
extern u32 gRaceComNameSatoshi[];
extern u32 gRaceComNameKasumi[];
extern u32 gRaceComNameMusashi[];
extern u32 gRaceComNameShigeru[];

void MiniCam_SetActive(CameraNode*);
Gfx* MiniCam_ApplyProjection(Gfx*);
Gfx* MiniGfx_DrawTexturedQuad(Gfx*, Mtx*, f32, f32, f32, f32, void*, s32, s32);
void Gfx_DrawTilemap(s32);
s32 MiniSound_DispatchCommand(s32, s32, s32);

#endif // _FRAGMENT4_H_
