#include "model_renderer_callbacks.h"
#include "src/geo_render.h"
#include "src/gallery.h"
#include "src/battle_hud.h"
#include "src/33FE0.h"
#include "src/gfx_buffer.h"
#include "src/matrix.h"

typedef struct StadiumModelSlot {
    /* 0x000 */ StadiumModel model;
    /* 0x204 */ char pad204[0x5CC];
} StadiumModelSlot; // size = 0x7D0

typedef struct unk_func_80032F94_a0 {
    /* 0x00 */ u8 pad00[0x18];
    /* 0x18 */ s32 unk_18;
    /* 0x1C */ s32 unk_1C;
} unk_func_80032F94_a0;  // size = 0x20

typedef struct unk_D_800B2EA8 {
    u8 unk_00[0xA8];
} unk_D_800B2EA8; // size = 0xA8

extern StadiumModelSlot D_800AFFC8[2][3];
extern s32 D_80076270[16];
extern Gfx D_800762B0[6];
extern Gfx D_800762C8[];
extern u32 D_800777A8[];
extern u32 D_80077950[];
extern MtxF D_800AF7C8[32];
extern unk_D_800B2EA8 D_800B2EA8[1];

// extern Gfx D_800762C8[] = {
//     gsSPSetGeometryMode(0x00FDFFFF),
//     gsDPPipeSync(),
//     gsDPSetCombineLERP(
//         TEXEL0, 0, SHADE, 0,      // Color cycle 1
//         TEXEL0, 0, SHADE, 0,      // Color cycle 2
//         0, 0, 0, SHADE,           // Alpha cycle 1
//         0, 0, 0, SHADE            // Alpha cycle 2
//     ),
//     gsDPSetAlphaCompare(G_AC_NONE),
//     gsSPEndDisplayList(),
// };

void ModelRenderer_OnSlotReady(s32 arg0) {

}

s32 ModelRenderer_IsReady(UNUSED s32 arg0) {
    return 1;
}

#ifdef NON_MATCHING
void ModelRenderer_CacheSlotMatrices(u8 arg0) {
    MtxF* temp_a0;
    MtxF* temp_v0;

    temp_v0 = GeoRender_GetRelativeMatrix(0);
    temp_a0 = &D_800AF7C8[arg0]; // arg0 * 16?

    MtxF_Copy(temp_a0, temp_v0);
    MtxF_Copy(&temp_a0[1], GeoRender_GetRelativeMatrix(-1));
}
#else
void ModelRenderer_CacheSlotMatrices(u8);
#pragma GLOBAL_ASM("asm/us/nonmatchings/334D0/ModelRenderer_CacheSlotMatrices.s")
#endif

#ifdef NON_MATCHING
void ModelRenderer_ApplySlotScale(u8 index) {
    MtxF* temp_a1;
    MtxF sp1C;

    MtxF_ExtractScale(&sp1C, &D_8006F088->unk_60.mtxf);
    temp_a1 = &D_800AF7C8[index << 0x4];
    guMtxCatF(sp1C.mf, temp_a1->mf, temp_a1->mf);
}
#else
void ModelRenderer_ApplySlotScale(u8);
#pragma GLOBAL_ASM("asm/us/nonmatchings/334D0/ModelRenderer_ApplySlotScale.s")
#endif

#ifdef NON_MATCHING
void ModelRenderer_RebuildSlotMatrix(u8 index) {
    MtxF* temp_a0;
    MtxF sp38;
    MtxF sp78;
    Vec3f spC0;
    Vec3f sp2C;
    MtxF* sp28;
    u8 temp_a2;

    temp_a2 = index;
    temp_a0 = &D_800AF7C8[index << 0x4];
    MtxF_GetScaleVector(temp_a0, &sp2C);
    guScaleF(sp38.mf, sp2C.x, sp2C.y, sp2C.z);
    guMtxXFMF(sp28->mf, 0.0f, 0.0f, 0.0f, &spC0.x, &spC0.y, &spC0.z);
    guTranslateF(sp78.mf, spC0.x, spC0.y, spC0.z);
    guMtxCatF(sp38.mf, sp78.mf, temp_a0->mf);
}
#else
void ModelRenderer_RebuildSlotMatrix(u8);
#pragma GLOBAL_ASM("asm/us/nonmatchings/334D0/ModelRenderer_RebuildSlotMatrix.s")
#endif

void ModelRenderer_ClearSlotModels(s32 arg0) {
    s32 i;
    StadiumModelSlot* slot = &D_800AFFC8[arg0];

    for (i = 0; i < 3; i++) {
        slot[i].model.unk_00 = 0;
    }
}

#ifdef NON_MATCHING
Gfx* ModelRenderer_SetupSlotColors(Gfx* gfx) {
    s16 temp_a1;
    u8 temp_v0;

    temp_v0 = D_8006F09C->materialAlpha;
    temp_a1 = D_8006F09C->modelId;
    if (temp_a1 != 0x7E) {
        if (temp_a1 != 0x92) {
            gSPDisplayList(gfx++, &D_800762C8);
            gDPSetPrimColor(gfx++, 0, temp_v0 & 0xFF, 0xFF, 0xFF, 0xFF, 0xFF);
            gDPSetEnvColor(gfx++, (0xB4 - (((s32) D_8006F084 % 8) * 0xA)), 0x20, 0, 0);
            return gfx;
        }
        gSPDisplayList(gfx++, &D_800762C8);
        gDPSetPrimColor(gfx++, 0, temp_v0 & 0xFF, 0xFF, 0xFF, 0xFF, 0xC8);
        gDPSetEnvColor(gfx++, 0xFF, 0x20, 0, 0);
        return gfx;
    }
    gSPDisplayList(gfx++, &D_800762C8);
    gDPSetPrimColor(gfx++, 0, temp_v0 & 0xFF, 0x07, 0x08, 0x02, 0x00);
    gDPSetEnvColor(gfx++, 0xFF, 0x20, 0, 0);
    return gfx;
}
#else
Gfx* ModelRenderer_SetupSlotColors(Gfx*);
#pragma GLOBAL_ASM("asm/us/nonmatchings/334D0/ModelRenderer_SetupSlotColors.s")
#endif

#ifdef NON_MATCHING
void ModelRenderer_UpdateSlotModel(void) {
    u8 index;
    s32 mode;

    mode = ModelRenderer_GetActiveMode();
    index = (u8)mode;
    if (mode < 2) {
        ModelRenderer_CacheSlotMatrices(index);
        ModelRenderer_RebuildSlotMatrix(index);
        ModelRenderer_ApplySlotScale(index);
        if (D_8006F09C->modelId == 0x92) {
            func_80033B2C(&D_800AFFC8[D_800AF7B0[index & 1]][index], &D_800B2EA8, &D_800AF7C8[index * 16], &D_80077950);
            return;
        }
        func_80033B2C(&D_800AFFC8[D_800AF7B0[index & 1]][index], &D_800B2EA8, &D_800AF7C8[index * 16], &D_800777A8);
    }
}
#else
#pragma GLOBAL_ASM("asm/us/nonmatchings/334D0/ModelRenderer_UpdateSlotModel.s")
#endif

#ifdef NON_MATCHING
void ModelRenderer_UpdateSlotVertices(void) {
    u8 index;
    s32 mode;

    mode = ModelRenderer_GetActiveMode();
    index = (u8)mode;
    if ((mode < 2) && (func_800325AC() == 0) && (D_800AF7C0[index] >= 2)) {
        ModelRenderer_CacheSlotMatrices(index);
        ModelRenderer_RebuildSlotMatrix(index);
        ModelRenderer_ApplySlotScale(index);
        func_800361C4(&D_800AFFC8[D_800AF7B0[index & 1]][index].model, &D_800AF7C8[index << 4]);
    }
}
#else
#pragma GLOBAL_ASM("asm/us/nonmatchings/334D0/ModelRenderer_UpdateSlotVertices.s")
#endif

#ifdef NON_MATCHING
Gfx* ModelRenderer_DrawStaticSegment(Gfx* gfx) {
    ModelSegment* segment;
    Mtx tempMtx;       // Temporary RSP matrix
    MtxF scaleMtx;     // Scale matrix
    MtxF translateMtx; // Translation matrix
    MtxF modelMtx;     // Matrix for model
    MtxF tempCatMtx;   // Temporary concatenated matrix
    Mtx* mtxL;
    Vec3f scale;
    Vec3f translate;

    if (D_8006F09C->modelId == 0x92) {
        segment = (ModelSegment *)&D_80077950;
    } else {
        segment = (ModelSegment *)&D_800777A8;
    }
    MtxF_ExtractScale(&modelMtx, &D_8006F088->unk_60.mtxf);
    guMtxF2L(&modelMtx.mf[0], &tempMtx);
    MtxF_Copy(&tempCatMtx, GeoRender_GetRelativeMatrix(0));
    MtxF_GetScaleVector(&tempCatMtx, &scale);
    guScaleF(&scaleMtx.mf[0], scale.x, scale.y, scale.z);
    guMtxXFMF(&tempCatMtx.mf[0], 0.0f, 0.0f, 0.0f, &translate.x, &translate.y, &translate.z);
    guTranslateF(&translateMtx.mf[0], translate.x, translate.y, translate.z);
    mtxL = Gfx_AllocDisplayList(0x40);
    guMtxCatF(&scaleMtx.mf[0], &translateMtx.mf[0], &tempCatMtx.mf[0]);
    guMtxF2L(&tempCatMtx.mf[0], mtxL);
    guMtxCatL(&tempMtx, mtxL, mtxL);
    {    Gfx *_gfx = (Gfx *)(gfx++);    _gfx->words.w0 = 0xDA380002;    _gfx->words.w1 = mtxL;    }
    gfx = ModelRenderer_SetupSlotColors(gfx);
    {    Gfx *_gfx = (Gfx *)(gfx++);    _gfx->words.w0 = 0xDB060038;    _gfx->words.w1 = (s32) segment->vertexSegment;    }
    {    Gfx *_gfx = (Gfx *)(gfx++);    _gfx->words.w0 = 0xDE000000;    _gfx->words.w1 = (s32) segment->displayList;    }
    {    Gfx *_gfx = (Gfx *)(gfx++);    _gfx->words.w0 = 0xD8380002;    _gfx->words.w1 = 0x40;    }
    {    Gfx *_gfx = (Gfx *)(gfx++);    _gfx->words.w0 = 0xDE000000;    _gfx->words.w1 = &D_800762B0;    }
    return gfx;
}
#else
Gfx* ModelRenderer_DrawStaticSegment(Gfx*);
#pragma GLOBAL_ASM("asm/us/nonmatchings/334D0/ModelRenderer_DrawStaticSegment.s")
#endif

#ifdef NON_MATCHING
Gfx* ModelRenderer_RenderActiveSlot(Gfx* gfx) {
    Vtx* vtx;
    s16 temp_v1_2;
    s16* temp_v1;
    s16* var_v1;
    s32 temp_v0;
    u8 temp_a3;
    unk_func_80032F94_a0* temp_a0;
    Gfx* gfx2;
    Gfx* gfx3;
    Gfx* gfx4;
    
    temp_v0 = ModelRenderer_GetActiveMode();
    temp_a3 = temp_v0 & 0xFF;
    if ((temp_v0 == 0xFF) || (temp_v0 == 0xFE)) {
        return ModelRenderer_DrawStaticSegment(gfx);
    }
    if (temp_v0 >= 4) {
        return gfx;
    }
    if (temp_v0 >= 2) {
        if (D_800AF7C0[temp_v0 & 1] >= 2) {
            Gfx_AllocDisplayList(0xA0);
            gfx2 = ModelRenderer_SetupSlotColors(gfx);
            temp_v1 = &D_800AF7B0[temp_v0 & 1];
            temp_a0 = (unk_func_80032F94_a0*)(&D_800AFFC8[*temp_v1][temp_v0 & 1].model + 0x138);
            {    Gfx *_gfx = (Gfx *)(gfx2++);    _gfx->words.w0 = 0xDB060038;    _gfx->words.w1 = (s32) temp_a0->unk_18;    }
            {    Gfx *_gfx = (Gfx *)(gfx2++);    _gfx->words.w0 = 0xDE000000;    _gfx->words.w1 = (s32) temp_a0->unk_1C;    }
            {    Gfx *_gfx = (Gfx *)(gfx2++);    _gfx->words.w0 = 0xDE000000;    _gfx->words.w1 = &D_800762B0;    }
            gfx = gfx2;
            *temp_v1 += 1;
        }
        return gfx;
    }
    temp_v1_2 = D_800AF7C0[temp_v0];
    switch (temp_v1_2) {                            /* irregular */
    default:
        var_v1 = &D_800AF7B0[temp_a3 & 1];
        break;
    case 1:
        ModelRenderer_UpdateSlotModel();
        var_v1 = &D_800AF7B0[temp_a3 & 1];
        break;
    case 2:
    case 3:
        vtx = Gfx_AllocDisplayList(0xA0);
        {    Gfx *_gfx = (Gfx *)(gfx++);    _gfx->words.w0 = 0xDA380002;    _gfx->words.w1 = &D_80076270;    }
        gfx3 = ModelRenderer_SetupSlotColors(gfx);
        var_v1 = &D_800AF7B0[temp_a3 & 1];
        gfx4 = Model_Draw(gfx3, &D_800AFFC8[*var_v1][temp_a3].model, vtx);
        {    Gfx *_gfx = (Gfx *)(gfx4++);    _gfx->words.w0 = 0xD8380002;    _gfx->words.w1 = 0x40;    }
        {    Gfx *_gfx = (Gfx *)(gfx4++);    _gfx->words.w0 = 0xDE000000;    _gfx->words.w1 = &D_800762B0;    }
        gfx = gfx4;
        break;
    }
    *var_v1 += 1;
    return gfx;
}
#else
Gfx* ModelRenderer_RenderActiveSlot(Gfx*);
#pragma GLOBAL_ASM("asm/us/nonmatchings/334D0/ModelRenderer_RenderActiveSlot.s")
#endif
