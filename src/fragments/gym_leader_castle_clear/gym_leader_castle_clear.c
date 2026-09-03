#include "gym_leader_castle_clear.h"
#include "src/geo_render.h"
#include "src/model_renderer.h"
#include "src/graphics_textures.h"
#include "src/input.h"
#include "src/ui_graphics.h"
#include "src/jpeg_stream.h"
#include "src/gfx_buffer.h"
#include "src/gfx_rect.h"
#include "src/matrix.h"
#include "src/controller.h"
#include "src/memmap.h"
#include "src/memory.h"
#include "src/stage_loader.h"

s32 Glc_GeoSpriteCallback(s32, GraphNode*);
s32 Glc_GeoBannerCallback(s32, GraphNode*);

typedef struct unk_D_83101E6C {
    /* 0x00 */ s16 unk_00;
    /* 0x04 */ f32 unk_04;
    /* 0x08 */ s16 unk_08;
    /* 0x0A */ s16 unk_0A;
    /* 0x0C */ s16 unk_0C;
    /* 0x0E */ s16 unk_0E;
    /* 0x10 */ s16 unk_10;
    /* 0x12 */ s16 unk_12;
} unk_D_83101E6C; // size = 0x14

typedef struct unk_D_83102218 {
    /* 0x00 */ s16 unk_00;
    /* 0x02 */ s16 unk_02;
    /* 0x04 */ s16 unk_04;
    /* 0x08 */ f32 unk_08;
} unk_D_83102218; // size = 0xC

typedef struct unk_D_83102224 {
    /* 0x00 */ s16 unk_00;
    /* 0x02 */ s16 unk_02;
    /* 0x04 */ s16 unk_04;
} unk_D_83102224; // size = 0x8

static BinArchive* D_83101EE0;
static u8* D_83101EE4;
static s16 D_83101EE8;
static s16 D_83101EEA;
static s16 D_83101EEC;
static s16 D_83101EEE;
static s16 D_83101EF0;
static GraphNode* D_83101EF4;
static GraphNode* D_83101EF8;
static GraphNode* D_83101EFC;
static unk_D_83101F00 D_83101F00[2];
static unk_D_83101F00* D_83102210;
static unk_D_83101F00* D_83102214;
static unk_D_83102218 D_83102218;
static unk_D_83102224 D_83102224;

static Vtx D_83101BE0[] = {
    VTX(-100, 14, 0, 0, 0, 0x78, 0x32, 0xFF, 0xFF),      VTX(-100, 0, 0, 0, 448, 0xBB, 0x28, 0x8E, 0xFF),
    VTX(100, 0, 0, 6400, 448, 0xBB, 0x28, 0x8E, 0xFF),   VTX(100, 14, 0, 6400, 0, 0x78, 0x32, 0xFF, 0xFF),
    VTX(-100, 0, 0, 0, 0, 0xBB, 0x28, 0x8E, 0xFF),       VTX(-100, -14, 0, 0, 448, 0xFF, 0x1E, 0x1E, 0xFF),
    VTX(100, -14, 0, 6400, 448, 0xFF, 0x1E, 0x1E, 0xFF), VTX(100, 0, 0, 6400, 0, 0xBB, 0x28, 0x8E, 0xFF),
};

static Gfx D_83101C60[] = {
    gsSPSetGeometryMode(G_CULL_BACK),
    gsDPSetCombineLERP(SHADE, 0, TEXEL0, 0, 0, 0, 0, TEXEL0, 0, 0, 0, COMBINED, 0, 0, 0, COMBINED),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsSPVertex(D_83101BE0, 8, 0),
    gsDPLoadTextureBlock(0x0F000000, G_IM_FMT_IA, G_IM_SIZ_8b, 200, 14, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                         G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
    gsDPLoadTextureBlock(0x0F000AF0, G_IM_FMT_IA, G_IM_SIZ_8b, 200, 14, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                         G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(4, 5, 6, 0, 4, 6, 7, 0),
    gsSPEndDisplayList(),
};

static u32 D_83101D08[] = {
    0x17000000, 0x00000000, 0x00000000,    0x00000000, D_83101BE0, 0x05000000, 0x22050000,
    0x00000000, 0x08000000, Glc_GeoBannerCallback, 0x00000000, 0x06000000, 0x01000000, 0x00000000,
};

static Vtx D_83101D40[] = {
    VTX(-64, 16, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(-64, -16, 0, 0, 1024, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(64, -16, 0, 4096, 1024, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(64, 16, 0, 4096, 0, 0xFF, 0xFF, 0xFF, 0xFF),
};

static Gfx D_83101D80[] = {
    gsSPSetGeometryMode(G_CULL_BACK),
    gsDPSetCombineMode(G_CC_BLENDPEDECALA, G_CC_PASS2),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsSPVertex(D_83101D40, 4, 0),
    gsDPLoadTextureBlock(0x0F000000, G_IM_FMT_IA, G_IM_SIZ_8b, 128, 32, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                         G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
    gsSPEndDisplayList(),
};

static u32 D_83101DE8[] = {
    0x17000000, 0x00000000, 0x00000000,    0x00000000, D_83101D40, 0x05000000, 0x22050000,
    0x00000000, 0x08000000, Glc_GeoSpriteCallback, 0x00000000, 0x06000000, 0x01000000,
};

static u32 D_83101E1C[] = {
    0x0C000000,  0x05000000, 0x0B00002D, 0x00000000, 0x028001E0, 0x00000000, 0xFDBD0000,
    0x00000243,  0x05000000, 0x0D000000, 0x05000000, 0x0F000002, 0x05000000, 0x0A000000,
    &D_800AC840, 0x06000000, 0x06000000, 0x06000000, 0x06000000, 0x01000000,
};

static unk_D_83101E6C D_83101E6C[] = {
    { 0xC, 0.0f, 0, 0, 0, 0, 0, 0 },
    { 4, 2.4f, 0xFF, 0, 0, 0, 0, 0 },
    { 4, 2.0f, 0xFF, 0xFF, 0xFF, 0xC8, 0x64, 0 },
    { -1, 2.0f, 0xC8, 0xC8, 0x1E, 0, 0, 0 },
};

void Glc_UpdateSweepObject(unk_D_86002F58_004_000* arg0) {
    unk_D_83101F00* ptr = (unk_D_83101F00*)arg0;
    f32 temp_fv0;
    s32 sp1C;
    f32 var_fv0;

    switch (ptr->unk_16A) {
        case 1:
            if (ptr->unk_180 >= 0.0f) {
                ptr->unk_184 -= 0.065f;
            } else {
                ptr->unk_184 += 0.065f;
            }

            sp1C = 0;
            temp_fv0 = ptr->unk_180;
            ptr->unk_180 += ptr->unk_184;

            if (((ptr->unk_180 * temp_fv0) < 0.0f) && (abs_asm(ptr->unk_184) < 0.13f)) {
                sp1C = 1;
            }

            if (sp1C == 0) {
                ptr->unk_184 *= 0.75f;
                var_fv0 = (ptr->unk_180 * -520.0f) + 320.0f;
            } else {
                ptr->unk_16A = 2;
                ptr->unk_16C = 0;
                var_fv0 = 320.0f;
            }

            ptr->unk_000.unk_024.x = var_fv0 - 320.0f;
            D_83101EEE = (s16)var_fv0 + 0xC8;
            D_83101EF0 = 0xAC;
            break;

        case 0:
        case 2:
        case 3:
            break;
    }
}

s32 Glc_GeoBannerCallback(s32 arg0, UNUSED GraphNode* arg1) {
    switch (arg0) {
        case 0:
            break;

        case 2:
            Glc_UpdateSweepObject(D_8006F09C);
            break;

        case 5:
            gDPPipeSync(gDisplayListHead++);

            gSPSegment(gDisplayListHead++, 0x0F, Memmap_GetSegmentVaddr(D_3000000));
            gSPDisplayList(gDisplayListHead++, D_83101C60);

            GeoRender_ApplyMaterialState();
            break;
    }

    return 0;
}

void Glc_UpdateColorKeyframe(unk_D_83101F00* arg0) {
    unk_D_83101E6C* ptr1;
    unk_D_83101E6C* ptr2;
    f32 var_fv1;

    switch (arg0->unk_16A) {
        case 1:
            arg0->unk_16C--;
            ptr1 = &D_83101E6C[arg0->unk_170];
            ptr2 = &D_83101E6C[arg0->unk_170 + 1];

            arg0->unk_172 = ptr2->unk_08 + (((ptr1->unk_08 - ptr2->unk_08) * arg0->unk_16C) / arg0->unk_16E);
            arg0->unk_174 = ptr2->unk_0A + (((ptr1->unk_0A - ptr2->unk_0A) * arg0->unk_16C) / arg0->unk_16E);
            arg0->unk_176 = ptr2->unk_0C + (((ptr1->unk_0C - ptr2->unk_0C) * arg0->unk_16C) / arg0->unk_16E);

            arg0->unk_178 = ptr2->unk_0E + (((ptr1->unk_0E - ptr2->unk_0E) * arg0->unk_16C) / arg0->unk_16E);
            arg0->unk_17A = ptr2->unk_10 + (((ptr1->unk_10 - ptr2->unk_10) * arg0->unk_16C) / arg0->unk_16E);
            arg0->unk_17C = ptr2->unk_12 + (((ptr1->unk_12 - ptr2->unk_12) * arg0->unk_16C) / arg0->unk_16E);

            var_fv1 = ptr2->unk_04 + (((ptr1->unk_04 - ptr2->unk_04) * arg0->unk_16C) / arg0->unk_16E);

            if (arg0->unk_16C <= 0) {
                arg0->unk_170++;
                ptr1 = &D_83101E6C[arg0->unk_170];

                arg0->unk_16C = arg0->unk_16E = ptr1->unk_00;
                arg0->unk_172 = ptr1->unk_08;
                arg0->unk_174 = ptr1->unk_0A;
                arg0->unk_176 = ptr1->unk_0C;
                arg0->unk_178 = ptr1->unk_0E;
                arg0->unk_17A = ptr1->unk_10;
                arg0->unk_17C = ptr1->unk_12;

                var_fv1 = ptr1->unk_04;

                if (arg0->unk_16C == -1) {
                    arg0->unk_16E = 0;
                    arg0->unk_16A = 2;
                    arg0->unk_16C = arg0->unk_16E;
                }
            }

            Vec3f_SetComponentsDuplicate(&arg0->unk_000.unk_030, var_fv1, var_fv1, var_fv1);
            break;

        case 0:
        case 2:
        case 3:
            break;
    }
}

s32 Glc_GeoSpriteCallback(s32 arg0, GraphNode* arg1) {
    unk_D_83101F00* ptr = (unk_D_83101F00*)D_8006F09C;

    switch (arg0) {
        case 0:
            break;

        case 2:
            Glc_UpdateColorKeyframe(ptr);
            break;

        case 5:
            gDPPipeSync(gDisplayListHead++);

            gSPSegment(gDisplayListHead++, 0x0F, Memmap_GetSegmentVaddr(D_30015E0));
            gDPSetPrimColor(gDisplayListHead++, 0, 0, ptr->unk_172, ptr->unk_174, ptr->unk_176, 0xFF);
            gDPSetEnvColor(gDisplayListHead++, ptr->unk_178, ptr->unk_17A, ptr->unk_17C, 0xFF);
            gSPDisplayList(gDisplayListHead++, D_83101D80);

            GeoRender_ApplyMaterialState();
            break;
    }
    return 0;
}

void Glc_InitCircleWipe(void) {
    D_83102218.unk_00 = 0;
    D_83102218.unk_02 = 0;
    D_83102218.unk_08 = 7.0f;
}

void Glc_UpdateCircleWipe(void) {
    s32 tmp;

    switch (D_83102218.unk_00) {
        case 1:
            D_83102218.unk_02--;
            D_83102218.unk_08 = (D_83102218.unk_02 * 7.0f) / D_83102218.unk_04;

            tmp = D_83102218.unk_02 <= 0;
            if (tmp) {
                D_83102218.unk_00 = 2;
                D_83102218.unk_02 = D_83102218.unk_04 = 0;
                D_83102218.unk_08 = 0.0f;
            }
            break;

        case 0:
        case 2:
            break;
    }
}

void Glc_DrawCircleWipe(void) {
    s16 var_t0;
    s16 var_t1;
    s16 var_t3;
    s16 var_t4;
    s16 sp9E;
    s16 sp9C;
    s16 sp40;
    s16 sp38;
    s32 temp_ft2;

    Gfx_SetScissorRect(&gDisplayListHead, 0, 0, 0x280, 0x1E0);
    if (D_83102218.unk_00 == 0) {
        return;
    }

    if ((D_83102218.unk_08 <= 0.0f) || (D_83102218.unk_00 == 2)) {
        gSPDisplayList(gDisplayListHead++, D_8006F4C0);
        gDPSetFillColor(gDisplayListHead++, 0x00010001);
        gDPFillRectangle(gDisplayListHead++, 0, 0, 639, 479);
        return;
    }

    temp_ft2 = ROUND_MAX(1024.0f / D_83102218.unk_08);
    sp9E = ROUND_MAX(128.0f * D_83102218.unk_08);
    sp9C = ROUND_MAX(128.0f * D_83102218.unk_08);

    sp38 = (0x280 - sp9E) / 2;
    sp40 = (0x1E0 - sp9C) / 2;

    gSPDisplayList(gDisplayListHead++, D_8006F518);
    gDPSetCombineLERP(gDisplayListHead++, 1, TEXEL0, ENVIRONMENT, 0, 1, TEXEL0, ENVIRONMENT, 0, 1, TEXEL0, ENVIRONMENT,
                      0, 1, TEXEL0, ENVIRONMENT, 0);
    gDPSetEnvColor(gDisplayListHead++, 0, 0, 0, 255);
    gDPLoadTextureBlock_4b(gDisplayListHead++, D_3002C00, G_IM_FMT_I, 64, 64, 0, G_TX_MIRROR | G_TX_WRAP,
                           G_TX_MIRROR | G_TX_WRAP, 6, 6, G_TX_NOLOD, G_TX_NOLOD);

    Gfx_DrawTexturedRectClipped(sp38, sp40, sp9E, sp9C, 0, 0, temp_ft2, temp_ft2, 0);

    var_t3 = sp38;
    var_t0 = sp40;

    gSPDisplayList(gDisplayListHead++, D_8006F4C0);
    gDPSetFillColor(gDisplayListHead++, 0x00010001);

    var_t4 = sp38 + sp9E;
    var_t1 = sp40 + sp9C;

    if (sp38 < 0) {
        var_t3 = 0;
    }

    if (sp40 < 0) {
        var_t0 = 0;
    }

    if (var_t4 >= 0x281) {
        var_t4 = 0x280;
    }

    if (var_t1 >= 0x1E1) {
        var_t1 = 0x1E0;
    }

    if (var_t0 > 0) {
        gDPFillRectangle(gDisplayListHead++, 0, 0, 639, var_t0 - 1);
    }

    if (var_t1 < 0x1E0) {
        gDPFillRectangle(gDisplayListHead++, 0, var_t1, 639, 479);
    }

    if ((var_t3 > 0) && (var_t0 >= 0) && (var_t1 < 0x1E1)) {
        gDPFillRectangle(gDisplayListHead++, 0, var_t0, var_t3 - 1, var_t1 - 1);
    }

    if ((var_t4 < 0x281) && (var_t0 >= 0) && (var_t1 < 0x1E1)) {
        gDPFillRectangle(gDisplayListHead++, var_t4, var_t0, 639, var_t1 - 1);
    }
}

void Glc_StartCircleWipe(s16 arg0) {
    if (arg0 > 0) {
        D_83102218.unk_00 = 1;
        D_83102218.unk_02 = D_83102218.unk_04 = arg0;
        D_83102218.unk_08 = 7.0f;
    }
}

void Glc_InitBackgroundFade(void) {
    D_83102224.unk_00 = 0;
    D_83102224.unk_02 = 0;
    D_83102224.unk_04 = 0xC0;
}

void Glc_UpdateBackgroundFade(void) {
    switch (D_83102224.unk_00) {
        case 1:
            D_83102224.unk_02--;
            D_83102224.unk_04 = ((D_83102224.unk_02 * -0x3F) / 10) + 0xFF;
            if (D_83102224.unk_02 <= 0) {
                D_83102224.unk_00 = 2;
                D_83102224.unk_02 = 0;
                D_83102224.unk_04 = 0xFF;
            }
            break;

        case 2:
            break;
    }
}

void Glc_DrawFadedBackground(u8* arg0, s16 arg1) {
    s32 i;
    s32 j;

    gDPPipeSync(gDisplayListHead++);

    gDPSetCycleType(gDisplayListHead++, G_CYC_1CYCLE);
    gDPSetRenderMode(gDisplayListHead++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
    gDPSetTexturePersp(gDisplayListHead++, G_TP_NONE);
    gDPSetScissor(gDisplayListHead++, G_SC_NON_INTERLACE, 0, 0, 640, 480);
    gDPSetCombineLERP(gDisplayListHead++, 0, 0, 0, TEXEL0, TEXEL0, 0, ENVIRONMENT, 0, 0, 0, 0, TEXEL0, TEXEL0, 0,
                      ENVIRONMENT, 0);
    gDPSetEnvColor(gDisplayListHead++, 0, 0, 0, arg1);

    gDPPipeSync(gDisplayListHead++);

    for (i = 0; i < 0x1E0; i += 0x20) {
        for (j = 0; j < 0x280; j += 0x20) {
            gDPLoadTextureBlock(gDisplayListHead++, arg0, G_IM_FMT_RGBA, G_IM_SIZ_16b, 16, 16, 0,
                                G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK,
                                G_TX_NOLOD, G_TX_NOLOD);
            gSPTextureRectangle(gDisplayListHead++, j << 2, i << 2, (j + 0x20) << 2, (i + 0x20) << 2, G_TX_RENDERTILE,
                                0, 0, 0x0200, 0x0200);

            arg0 += 0x200;
        }
    }
}

void Glc_DrawBackground(void) {
    if (D_83102224.unk_04 < 0xFF) {
        Glc_DrawFadedBackground(D_83101EE4, D_83102224.unk_04);
    } else {
        Gfx_DrawTiledRgba16Image(D_83101EE4);
    }
}

void Glc_StartBackgroundFade(s16 arg0) {
    D_83102224.unk_00 = arg0;
    if (D_83102224.unk_00 == 1) {
        D_83102224.unk_02 = 0xA;
        D_83102224.unk_04 = 0xC0;
    }
}

void Glc_ReadInputs(void) {
    Cont_StartReadInputs();
    Cont_ReadInputs();
    Input_ResetRepeatState();
}

void Glc_InitObjectAnimation(unk_D_83101F00* arg0, s16 arg1) {
    unk_D_83101E6C* ptr;

    arg0->unk_16A = arg1;

    if (arg0->unk_16A == 1) {
        switch (arg0->unk_168) {
            case 1:
                arg0->unk_184 = 0.0f;
                arg0->unk_180 = 1.0f;
                break;

            case 2:
                arg0->unk_170 = 0;
                ptr = &D_83101E6C[arg0->unk_170];

                arg0->unk_16C = arg0->unk_16E = ptr->unk_00;
                arg0->unk_172 = ptr->unk_08;
                arg0->unk_174 = ptr->unk_0A;
                arg0->unk_176 = ptr->unk_0C;
                arg0->unk_178 = ptr->unk_0E;
                arg0->unk_17A = ptr->unk_10;
                arg0->unk_17C = ptr->unk_12;

                Vec3f_SetComponentsDuplicate(&arg0->unk_000.unk_030, ptr->unk_04, ptr->unk_04, ptr->unk_04);
                break;
        }
    }
}

void Glc_DrawScisRectangle(s16 arg0, s16 arg1, s16 arg2, s16 arg3, s16 arg4, s16 arg5, s16 arg6, s16 arg7) {
    gSPScisTextureRectangle(gDisplayListHead++, arg0 << 2, arg1 << 2, ((arg0 + arg2) - 1) << 2,
                            ((arg1 + arg3) - 1) << 2, 0, arg4, arg5, arg6, arg7);
}

void Glc_DrawTrackingIcon(void) {
    f32 v = 1.8f;
    s16 temp_fa0;
    s16 temp_ft2;

    gSPDisplayList(gDisplayListHead++, D_8006F518);

    temp_ft2 = ROUND_MAX(28 * v);
    temp_fa0 = ROUND_MAX(1024.0f / v);

    gDPLoadTextureBlock(gDisplayListHead++, D_30025E0, G_IM_FMT_RGBA, G_IM_SIZ_16b, 28, 28, 0,
                        G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                        G_TX_NOLOD);

    Glc_DrawScisRectangle(D_83101EEE, D_83101EF0, temp_ft2, temp_ft2, 0, 0, temp_fa0, temp_fa0);

    gSPDisplayList(gDisplayListHead++, D_8006F630);
}

void Glc_ClearInitState(void) {
    D_83101EE8 = 0;
    if (D_800AE540.unk_11F2 != 0) {
        D_83101EEC = 1;
    } else {
        D_83101EEC = 0;
    }
    D_83101EF0 = -0x64;
    D_83101EEE = D_83101EF0;
    Glc_InitCircleWipe();
    Glc_InitBackgroundFade();
}

void Glc_ClearDraw(void) {
    BgStage_DrawFrame();
    Glc_DrawBackground();
    Geo_RenderRootNode(D_83101EF4);
    if (D_83101EEC != 0) {
        Glc_DrawTrackingIcon();
    }
    Glc_DrawCircleWipe();
    BgStage_AdvanceFrame();
}

s32 Glc_ClearAdvanceState(void) {
    s32 sp1C = 1;

    switch (D_83101EE8) {
        case 0:
            if (StageContext_GetFadeMode() == 0) {
                D_83101EE8 = 1;
                D_83101EEA = 0;
                Glc_InitObjectAnimation(D_83102210, 1);
            }
            break;

        case 1:
            if (D_83102210->unk_16A == 2) {
                switch (D_83102214->unk_16A) {
                    case 0:
                        Glc_InitObjectAnimation(D_83102214, 1);
                        break;

                    case 2:
                        D_83101EEA += 1;
                        if (D_83101EEA == 1) {
                            Glc_StartBackgroundFade(1);
                        }

                        if (D_83101EEA >= 0x1E) {
                            D_83101EE8 = 2;
                            D_83101EEA = 0;
                            Glc_StartCircleWipe(0xF);
                            StageContext_SetClearColor(1);
                        }
                        break;
                }
            }
            break;

        case 2:
            if (D_83102218.unk_00 == 2) {
                sp1C = 0;
            }
            break;
    }

    return sp1C;
}

void Glc_ClearRunLoop(void) {
    Glc_ClearInitState();
    StageFade_StartFromOpaque(0xF);

    do {
        Glc_ReadInputs();
        Glc_UpdateCircleWipe();
        Glc_UpdateBackgroundFade();
        Glc_ClearDraw();
    } while (Glc_ClearAdvanceState() != 0);
}

void Glc_ClearInitGraphics(void) {
    unk_D_83101F00* ptr;
    MemoryBlock* temp_v0 = MainPool_AllocState(main_pool_get_available(), 0);
    s32 i;

    D_83101EF4 = process_geo_layout(temp_v0, D_83101E1C);
    D_83101EF8 = process_geo_layout(temp_v0, D_83101D08);
    D_83101EFC = process_geo_layout(temp_v0, D_83101DE8);

    MainPool_FinalizeAllocation(temp_v0);
    ModelRenderer_InitDisplayRoots();

    for (i = 0; i < 2; i++) {
        ptr = &D_83101F00[i];

        ModelRenderer_AttachDisplayObject(&ptr->unk_000);

        ptr->unk_172 = ptr->unk_174 = ptr->unk_176 = 0;
        ptr->unk_178 = ptr->unk_17A = ptr->unk_17C = 0;
        ptr->unk_168 = 0;
        ptr->unk_16A = 0;
        ptr->unk_16C = 0;
        ptr->unk_16E = 0;
        ptr->unk_170 = 0;
        ptr->unk_180 = 0.0f;
        ptr->unk_184 = 0.0f;

        switch (i) {
            case 0:
                Vec3f_SetComponentsDuplicate(&ptr->unk_000.unk_024, -520.0f, 44.0f, -579.0f);
                Vec3f_SetComponentsDuplicate(&ptr->unk_000.unk_030, 2.0f, 2.0f, 2.0f);
                Model_InitDisplayObject(&ptr->unk_000, 0, 0, D_83101EF8);
                ptr->unk_168 = 1;
                D_83102210 = ptr;
                break;

            case 1:
                Vec3f_SetComponentsDuplicate(&ptr->unk_000.unk_024, 0.0, -24.0f, -579.0f);
                Vec3f_SetComponentsDuplicate(&ptr->unk_000.unk_030, 0.0f, 0.0f, 0.0f);
                Model_InitDisplayObject(&ptr->unk_000, 0, 0, D_83101EFC);
                ptr->unk_168 = 2;
                D_83102214 = ptr;
                break;
        }
    }
}

s32 GymLeaderCastleClear_Main(UNUSED s32 arg0, UNUSED s32 arg1) {
    unk_func_80007444* sp24;

    main_pool_push_state('CLRG');

    Gfx_InitDisplayListBuffers(0x10000, 0);
    sp24 = StageContext_Allocate(1, 0, 2, 0, 2, 1);
    Font_Init(0x10, 0);

    ASSET_LOAD(D_1000000, common_menu1_ui, 0);
    ASSET_LOAD(D_3000000, gym_leader_castle_clear_gfx, 0);
    D_83101EE0 = ASSET_LOAD2(backgrounds, 1, 1);
    D_83101EE4 = BinArchive_GetFile(D_83101EE0, 0xE);

    Glc_ClearInitGraphics();
    StageContext_Activate(sp24);
    Glc_ClearRunLoop();
    StageLoader_RunFrames(2);
    StageContext_Deactivate();
    Font_Free();
    Gfx_FreeDisplayListBuffers();

    main_pool_pop_state('CLRG');

    return 0;
}
