#include "battle_engine.h"

static s16 gTransitionState;
static s16 gTransitionOffset;
static s16 gTransitionSpeed;

static f32 gTransitionDefaultSpeed = 30.0f;

static Vtx gFourStreamHeadStarVtx[] = {
    VTX(1, -35, -35, 0, 0, 0x88, 0x00, 0x00, 0xFF), VTX(1, -50, 0, 0, 0, 0x88, 0x00, 0x00, 0xFF),
    VTX(1, 0, 0, 0, 0, 0x88, 0x00, 0x00, 0xFF),     VTX(1, -50, 0, 0, 0, 0x1D, 0x8C, 0x00, 0xFF),
    VTX(1, -35, -35, 0, 0, 0x1D, 0xAE, 0xAE, 0xFF), VTX(200, 0, 0, 0, 0, 0x78, 0x00, 0x00, 0xFF),
    VTX(1, 0, -49, 0, 0, 0x88, 0x00, 0x00, 0xFF),   VTX(1, 0, -49, 0, 0, 0x1D, 0x00, 0x8C, 0xFF),
    VTX(1, 35, -35, 0, 0, 0x88, 0x00, 0x00, 0xFF),  VTX(1, 35, -35, 0, 0, 0x1D, 0x52, 0xAE, 0xFF),
    VTX(1, 50, 0, 0, 0, 0x88, 0x00, 0x00, 0xFF),    VTX(1, 50, 0, 0, 0, 0x1D, 0x74, 0x00, 0xFF),
    VTX(1, 35, 35, 0, 0, 0x88, 0x00, 0x00, 0xFF),   VTX(1, 35, 35, 0, 0, 0x1D, 0x52, 0x52, 0xFF),
    VTX(1, 0, 49, 0, 0, 0x88, 0x00, 0x00, 0xFF),    VTX(1, 0, 49, 0, 0, 0x1D, 0x00, 0x74, 0xFF),
    VTX(1, -35, 35, 0, 0, 0x88, 0x00, 0x00, 0xFF),  VTX(1, -35, 35, 0, 0, 0x1D, 0xAE, 0x52, 0xFF),
};
static u32 gFourStreamHeadStarTex[] = {
    0x77777777,
    0x77777777,
};
Gfx gParticleQuadDL[] = {
    gsDPPipeSync(),
    gsDPSetCycleType(G_CYC_1CYCLE),
    gsSPClearGeometryMode(G_ZBUFFER | G_SHADE | G_CULL_BOTH | G_FOG | G_LIGHTING | G_TEXTURE_GEN |
                          G_TEXTURE_GEN_LINEAR | G_LOD | G_SHADING_SMOOTH | G_CLIPPING | 0x0040F9FA),
    gsSPSetGeometryMode(G_ZBUFFER | G_SHADE | G_CULL_BACK | G_LIGHTING | G_SHADING_SMOOTH),
    gsSPTexture(0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON),
    gsDPPipeSync(),
    gsDPLoadTextureBlock_4b(gFourStreamHeadStarTex, G_IM_FMT_I, 4, 4, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, 2, 2,
                            G_TX_NOLOD, G_TX_NOLOD),
    gsDPSetCombineMode(G_CC_BLENDIA, G_CC_BLENDIA),
    gsDPSetRenderMode(G_RM_AA_ZB_OPA_SURF, G_RM_AA_ZB_OPA_SURF2),
    gsDPSetPrimColor(0xFF, 0xFF, 197, 184, 122, 255),
    gsDPSetEnvColor(197, 184, 122, 255),
    gsSPVertex(gFourStreamHeadStarVtx, 18, 0),
    gsSP2Triangles(0, 1, 2, 0, 3, 4, 5, 0),
    gsSP2Triangles(6, 0, 2, 0, 4, 7, 5, 0),
    gsSP2Triangles(8, 6, 2, 0, 7, 9, 5, 0),
    gsSP2Triangles(10, 8, 2, 0, 9, 11, 5, 0),
    gsSP2Triangles(12, 10, 2, 0, 11, 13, 5, 0),
    gsSP2Triangles(14, 12, 2, 0, 13, 15, 5, 0),
    gsSP2Triangles(16, 14, 2, 0, 15, 17, 5, 0),
    gsSP2Triangles(1, 16, 2, 0, 17, 3, 5, 0),
    gsSPEndDisplayList(),
};

static s32 pad_D_8438AA58[] = { 0, 0 };

static Gfx gTransitionEmptyDL[] = {
    gsSPEndDisplayList(),
};
static u32 gTransitionMaskTexture[] = {
    0x9FFFFFFF, 0xFFFFFFFF, 0x09FFFFFF, 0xFFFFFFFF, 0x009FFFFF, 0xFFFFFFFF, 0x0009FFFF, 0xFFFFFFFF, 0x00009FFF,
    0xFFFFFFFF, 0x000009FF, 0xFFFFFFFF, 0x0000009F, 0xFFFFFFFF, 0x00000009, 0xFFFFFFFF, 0x00000000, 0x9FFFFFFF,
    0x00000000, 0x09FFFFFF, 0x00000000, 0x009FFFFF, 0x00000000, 0x000FFFFF, 0x00000000, 0x000FFFFF, 0x00000000,
    0x009FFFFF, 0x00000000, 0x09FFFFFF, 0x00000000, 0x9FFFFFFF, 0x00000009, 0xFFFFFFFF, 0x0000009F, 0xFFFFFFFF,
    0x000009FF, 0xFFFFFFFF, 0x00009FFF, 0xFFFFFFFF, 0x0009FFFF, 0xFFFFFFFF, 0x009FFFFF, 0xFFFFFFFF, 0x09FFFFFF,
    0xFFFFFFFF, 0x9FFFFFFF, 0xFFFFFFFF, 0x9FFFFFFF, 0xFFFFFFFF, 0x09FFFFFF, 0xFFFFFFFF, 0x009FFFFF, 0xFFFFFFFF,
    0x0009FFFF, 0xFFFFFFFF, 0x00009FFF, 0xFFFFFFFF, 0x000009FF, 0xFFFFFFFF, 0x0000009F, 0xFFFFFFFF, 0x00000009,
    0xFFFFFFFF, 0x00000000, 0x9FFFFFFF, 0x00000000, 0x09FFFFFF, 0x00000000, 0x009FFFFF, 0x00000000, 0x000FFFFF,
    0x00000000, 0x000FFFFF, 0x00000000, 0x009FFFFF, 0x00000000, 0x09FFFFFF, 0x00000000, 0x9FFFFFFF, 0x00000009,
    0xFFFFFFFF, 0x0000009F, 0xFFFFFFFF, 0x000009FF, 0xFFFFFFFF, 0x00009FFF, 0xFFFFFFFF, 0x0009FFFF, 0xFFFFFFFF,
    0x009FFFFF, 0xFFFFFFFF, 0x09FFFFFF, 0xFFFFFFFF, 0x9FFFFFFF, 0xFFFFFFFF,
};
static Gfx gTransitionMaskSetupDL[] = {
    gsDPPipeSync(),
    gsDPSetCycleType(G_CYC_1CYCLE),
    gsSPClearGeometryMode(G_ZBUFFER | G_CULL_BACK | G_LIGHTING),
    gsDPSetRenderMode(G_RM_XLU_SURF, G_RM_NOOP2),
    gsDPSetCombineMode(G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM),
    gsDPSetPrimColor(0, 0, 5, 5, 5, 255),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPSetTexturePersp(G_TP_NONE),
    gsSPEndDisplayList(),
};
static Gfx gTransitionMaskCleanupDL[] = {
    gsDPSetCycleType(G_CYC_2CYCLE),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPSetTexturePersp(G_TP_PERSP),
    gsSPSetGeometryMode(G_ZBUFFER | G_CULL_BACK | G_LIGHTING),
    gsSPEndDisplayList(),
};

void BattleScene_SetStripWipeState(s32 arg0, s32 arg1) {
    gTransitionState = arg0;
    gTransitionSpeed = arg1;

    if ((gTransitionState == 0) || ((gTransitionState != 1) && (gTransitionState != 2) && (gTransitionState == 3))) {
        gTransitionOffset = 0x140;
    } else {
        gTransitionOffset = 0;
    }
}

s32 BattleScene_GetStripWipeState(void) {
    return gTransitionState;
}

void BattleScene_UpdateStripWipe(void) {
    s16 var_v0 = gTransitionOffset;

    switch (gTransitionState) {
        case 2:
            if (var_v0 < 0x140) {
                var_v0 += gTransitionSpeed;
            } else {
                gTransitionState = 0;
            }
            break;

        case 3:
            if (var_v0 > 0) {
                var_v0 -= gTransitionSpeed;
            } else {
                gTransitionState = 1;
            }
            if (var_v0 < 0) {
                var_v0 = 0;
            }
            break;
    }
    gTransitionOffset = var_v0;
}

Gfx* BattleScene_BuildStripWipeDisplayList(Gfx* arg0, s32 arg1) {
    s32 i;
    s16 left;
    s16 top;
    s32 temp_a1;
    s32 temp_v1;

    temp_a1 = arg1 & 1;
    temp_v1 = (arg1 + 1) & 1;

    gSPDisplayList(arg0++, gTransitionMaskSetupDL);
    gDPLoadTextureBlock_4b(arg0++, gTransitionMaskTexture, G_IM_FMT_I, 12, 48, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                           G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
    gDPPipeSync(arg0++);

    for (i = temp_a1; i < 5; i += 2) {
        left = gTransitionOffset * 2 - 320;
        top = i * 48;
        gSPScisTextureRectangle(arg0++, left << 2, top << 2, (left + 640) << 2, (top + 24) << 2, 0, 0, 0, 1 << 6,
                                1 << 10);

        gSPScisTextureRectangle(arg0++, left << 2, (top + 24) << 2, (left + 640) << 2, (top + 48) << 2, 0, 0, 0, 1 << 6,
                                1 << 10);
    }

    for (i = temp_v1; i < 5; i += 2) {
        left = -gTransitionOffset * 2;
        top = i * 48;
        gSPScisTextureRectangle(arg0++, left << 2, top << 2, (left + 640) << 2, (top + 24) << 2, 0, 0x500, 0, -(1 << 6),
                                1 << 10);

        gSPScisTextureRectangle(arg0++, left << 2, (top + 24) << 2, (left + 640) << 2, (top + 48) << 2, 0, 0x500, 0,
                                -(1 << 6), 1 << 10);
    }

    gSPDisplayList(arg0++, gTransitionMaskCleanupDL);

    return arg0;
}

void BattleScene_DrawStripWipe(void) {
    Gfx* gfx = gDisplayListHead;

    if (gTransitionState != 0) {
        if (gTransitionState == 2) {
            gfx = BattleScene_BuildStripWipeDisplayList(gfx, 0);
        } else {
            gfx = BattleScene_BuildStripWipeDisplayList(gfx, 1);
        }
    }
    gDisplayListHead = gfx;
}
