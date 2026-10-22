#ifndef _32D10_H_
#define _32D10_H_

#include "global.h"

extern u8 gPillarGlowValues[8];
extern s16 gHpWheelDigitOffsetOnes[4];
extern s16 gHpWheelDigitOffsetTens[4];
extern s16 gHpWheelDigitOffsetHundreds[4];
extern s16 gHpWheelDigitOffsetOnesSmooth[4];
extern s16 gHpWheelDigitOffsetTensSmooth[4];
extern s16 gHpWheelDigitOffsetHundredsSmooth[4];
extern s16 gTextureOffsetScale;
extern s16 gModelUpdateFlag[2];

void BattleHud_SetDigitRotationTargets(u8 arg0, s16 arg1);
void BattleHud_ApproachDigitRotations(u8 arg0, s16 arg1);
void ModelAnim_UpdateFrameChangeState(void);
s32 ModelPhase_IsIdle(void);
u8 ModelRenderer_GetActiveMode(void);
#if defined(VERSION_JP)
void ModelAnim_StartDisplayObjectAnimation(u32 arg0);
#else
s16 ModelRenderer_GetObjectType(void);
void ModelAnim_StartDisplayObjectAnimation(DisplayObject* arg0);
#endif
void ModelAnim_UpdateDisplayObjectAnimation(u32 arg0, u32 arg1);
void ModelAnim_FinalizeDisplayObjectAnimation(u32 arg0, u32 arg1);
void Gfx_SetCombineMode(Gfx*, s32, s32, s32, s32, s32, s32, s32, s32, s32, s32, s32, s32, s32, s32, s32, s32);

#endif // _32D10_H_
