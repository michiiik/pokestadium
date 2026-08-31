#ifndef _F420_H_
#define _F420_H_

#include "global.h"

extern Vec3f D_8006F050;
extern Vec3s D_8006F05C;
extern Vec3f D_8006F064;
extern u16 D_8006F070[];

Color_RGBA8* Color_SetRGB(Color_RGBA8* arg0, u8 r, u8 g, u8 b);
Color_RGBA8* Color_SetRGBA(Color_RGBA8* arg0, u8 arg1, u8 arg2, u8 arg3, u8 arg4);
Vec3f* Vec3f_SetComponents(Vec3f* arg0, f32 arg1, f32 arg2, f32 arg3);
Vec3f* Vec3f_SetComponentsDuplicate(Vec3f* arg0, f32 arg1, f32 arg2, f32 arg3);
Vec3f* Vec3f_AddInPlace(Vec3f* arg0, Vec3f* arg1);
Vec3f* Vec3f_Add(Vec3f* arg0, Vec3f* arg1, Vec3f* arg2);
Vec3f* Vec3f_SubtractInPlace(Vec3f* arg0, Vec3f* arg1);
Vec3f* Vec3f_Subtract(Vec3f* arg0, Vec3f* arg1, Vec3f* arg2);
Vec3f* Vec3f_FromVec3s(Vec3f* arg0, Vec3s* arg1);
Vec3f* Vec3f_CrossProductFromPoints(Vec3f* arg0, Vec3f* arg1, Vec3f* arg2, Vec3f* arg3);
Vec3f* Vec3f_CrossProduct(Vec3f* arg0, Vec3f* arg1, Vec3f* arg2);
Vec3f* Vec3f_Normalize(Vec3f* arg0);
Vec3s* Vec3s_SetComponents(Vec3s* arg0, s16 arg1, s16 arg2, s16 arg3);
Vec3s* Vec3s_AddInPlace(Vec3s* arg0, Vec3s* arg1);
Vec3s* Vec3s_Add(Vec3s* arg0, Vec3s* arg1, Vec3s* arg2);
Vec3s* Vec3s_SubtractInPlace(Vec3s* arg0, Vec3s* arg1);
Vec3s* Vec3s_Subtract(Vec3s* arg0, Vec3s* arg1, Vec3s* arg2);
Vec3s* Vec3s_FromVec3f(Vec3s* arg0, Vec3f* arg1);
void MtxF_Copy(MtxF* dest, MtxF* src);
void MtxF_Identity(MtxF* arg0);
void MtxF_SetTranslation(MtxF* arg0, Vec3f* arg1);
void MtxF_SetLookAt(MtxF* mtx, Vec3f* from, Vec3f* to, u16 roll);
void MtxF_SetRotationTranslation(MtxF* dest, Vec3s* translate, Vec3s* rotate);
void MtxF_SetRotationTranslationF(MtxF* arg0, Vec3f* arg1, Vec3s* arg2);
void MtxF_SetRotationAndTransformTranslation(MtxF* arg0, Vec3f* arg1, Vec3s* arg2);
void MtxF_SetRotationScaleTranslation(MtxF* dest, Vec3f* b, Vec3s* c, Vec3f* arg3);
void MtxF_SetRotationAndScaledTranslation(MtxF* dest, Vec3f* b, Vec3s* c, Vec3f* arg3);
void MtxF_ApplyScaleTransform(MtxF* arg0, MtxF* arg1, MtxF* arg2, Vec3f* arg3, f32 arg4);
void MtxF_SetOrthonormalBasis(MtxF* dest, Vec3f* upDir, Vec3f* pos, s16 yaw);
void MtxF_Multiply(MtxF* arg0, MtxF* arg1, MtxF* arg2);
void MtxF_ScaleRows(MtxF* dest, MtxF* mtx, Vec3f* s);
void MtxF_GetScale(MtxF* arg0, Vec3f* arg1);
void MtxF_TransformVec3s(MtxF* arg0, Vec3s* arg1);
void MtxF_ToFixed(MtxF* dest, MtxF* src);
void MtxF_BuildTransform(MtxF* arg0, Vec3f* arg1, Vec3s* arg2, Vec3f* arg3, MtxF* arg4, MtxF* arg5);
void Vec3f_CalculateDistanceAngles(Vec3f* arg0, Vec3f* arg1, f32* arg2, s16* arg3, s16* arg4);
void Camera_ComputeEyeFromAngles(Vec3f* from, Vec3f* to, f32 dist, s16 pitch, s16 yaw);
s16 Math_StepToS(s16 current, s16 target, s16 inc);
s32 Math_StepToS32(s32 current, s32 target, s32 inc, s32 dec);
f32 Math_StepToF(f32 current, f32 target, f32 inc, f32 dec);
s16 PackedBits_ReadSigned(s16* arg0, s32 arg1, s32 arg2);
void Color_RGBToHSV(f32 arg0, f32 arg1, f32 arg2, Vec3f* arg3);
void Color_RGBA8ToHSV(Color_RGBA8_u32 arg0, Vec3f* arg1);
void Color_RGB5551ToHSV(u16 arg0, Vec3f* arg1);
f32 Color_HSVInterpolate(f32 arg0, f32 arg1, f32 arg2);
void Color_HSVToRGB(f32* arg0, f32* arg1, f32* arg2, Vec3f* arg3);
u16 Color_HSVToRGB5551(Vec3f* arg0);
Color_RGBA8_u32* Color_HSVToRGBA8(Color_RGBA8_u32* arg0, Vec3f* arg1, s32 arg2);
u16 Color_AdjustRGB5551(u16 arg0, arg1_func_80010CA8 arg1);
u32 Color_AdjustRGBA8(u32 arg0, arg1_func_80010CA8 arg1);

#endif // _F420_H_
