#ifndef _17300_H_
#define _17300_H_

#include "global.h"

typedef struct unk_D_800ABCC0_008 {
    /* 0x00 */ u8 translationKeyCount; // ModelAnim_EvaluateTranslationChannel: ==1 means a static (non-animated) value
    /* 0x01 */ u8 rotationKeyCount;
    /* 0x02 */ u8 scaleKeyCount;
    /* 0x03 */ u8 unk_03;
    /* 0x04 */ u16 translationBaseIndex; // index into the context's translation value array
    /* 0x06 */ u16 rotationBaseIndex; // packed-bits index into the context's rotation value array
    /* 0x08 */ u16 scaleBaseIndex; // packed-bits index into the context's scale value array
} unk_D_800ABCC0_008; // size = 0xA

typedef struct unk_D_800ABCC0 {
    /* 0x00 */ u8 isActive; // ModelAnim_BeginCurveContext: 1 when curveData != NULL, else 0
    /* 0x01 */ u8 flags; // mirrors curveData->unk_00; bit2 selects 16-bit vs 12-bit packed scale format
    /* 0x02 */ s16 currentFrame;
    /* 0x04 */ unk_D_86002F58_004_000_040_004* curveData;
    /* 0x08 */ unk_D_800ABCC0_008* channels; // per-joint-axis channel descriptors, indexed by ModelAnim_EvaluateJointTransform's arg3
    /* 0x0C */ s16* translationValues; // raw keyframe values, scaled by /1000.0f
    /* 0x10 */ s16* rotationValues; // PackedBits_ReadSigned source
    /* 0x14 */ s16* scaleValues; // PackedBits_ReadSigned source
} unk_D_800ABCC0; // size = 0x18

f32 ModelAnim_EvaluateTranslationChannel(unk_D_800ABCC0* arg0, s32 arg1);
s16 ModelAnim_EvaluateRotationChannel(unk_D_800ABCC0* arg0, s32 arg1);
f32 ModelAnim_EvaluateScaleChannel(unk_D_800ABCC0* arg0, s32 arg1);
f32 ModelAnim_InterpolateKeyframe(Vec3s* arg0, s16 arg1, s16 arg2);
f32 ModelAnim_EvaluateTranslationCurve(unk_D_800ABCC0* arg0, s32 arg1);
s16 ModelAnim_EvaluateRotationCurve(unk_D_800ABCC0* arg0, s32 arg1);
f32 ModelAnim_EvaluateScaleCurve(unk_D_800ABCC0* arg0, s32 arg1);
void ModelAnim_ResetCurveContext(void);
void ModelAnim_BeginCurveContext(unk_D_86002F58_004_000_040* arg0, u16 arg1, s32 arg2);
void ModelAnim_EndCurveContext(void);
void ModelAnim_EvaluateJointTransform(Vec3f* arg0, Vec3s* arg1, Vec3f* arg2, s32 arg3);
void ModelAnim_ClearTransformChannel(unk_D_86002F58_004_000* arg0);
s32 ModelAnim_BindTransformCurve(unk_D_86002F58_004_000* arg0, s16 arg1, void* arg2, s32 arg3);
s32 ModelAnim_SetSpeed(unk_D_86002F58_004_000* arg0, s32 arg1);
void ModelAnim_SetFrame(unk_D_86002F58_004_000* arg0, s16 arg1);
s32 ModelAnim_HasCrossedFrame(unk_D_86002F58_004_000* arg0, s16 arg1);
s32 ModelAnim_IsAnimationDone(unk_D_86002F58_004_000* arg0);
s32 ModelAnim_IsFinished(unk_D_86002F58_004_000* arg0);


#endif // _17300_H_
