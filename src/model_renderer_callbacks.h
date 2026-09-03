#ifndef _334D0_H_
#define _334D0_H_

#include "global.h"

extern s16 D_800AF7C0[4];

void ModelRenderer_OnSlotReady(s32);
s32 ModelRenderer_IsReady(s32);
void ModelRenderer_ClearSlotModels(s32);
void ModelRenderer_UpdateSlotVertices(void);
void ModelRenderer_CacheSlotMatrices(u8);
void ModelRenderer_ApplySlotScale(u8);
void ModelRenderer_RebuildSlotMatrix(u8);
Gfx* ModelRenderer_RenderActiveSlot(Gfx*);

#endif // _334D0_H_
