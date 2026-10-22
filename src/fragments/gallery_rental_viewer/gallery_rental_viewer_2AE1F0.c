#include "gallery_rental_viewer.h"
#include "src/geo_render.h"
#include "src/model_renderer.h"
#include "src/graphics_textures.h"
#include "src/ui_graphics.h"
#include "src/gb_data.h"
#include "src/pokemon_stats.h"
#include "src/audio_sfx.h"
#include "src/matrix.h"
#include "src/controller.h"
#include "src/fragments/rental_team_selection/rental_team_selection.h"
#include "src/math_util.h"
#include "src/memmap.h"
#include "src/memory.h"

typedef struct PlayerSlotData {
    /* 0x00 */ u8 unk_00;
    /* 0x01 */ u8 unk_01;
    /* 0x02 */ u8 unk_02;
    /* 0x03 */ u8 unk_03;
    /* 0x04 */ u16 unk_04;
    /* 0x06 */ char unk_06[1];
    /* 0x07 */ char unk07[0xD];
} PlayerSlotData; // size = 0x14

static GraphNode* gSnapSelectRoot;
static ModelNodeView* gSnapSelectNodes[6];
static ModelNodeView* gSnapSelectRentalNode;
static DisplayObject gPlayerIconNodes[2][6];
static PlayerSlotData gPlayerSlotData[4];
static u8* gPlayerIconTextures[4];
static s32 gRentalViewerState;
static s32 gSnapSelectAnimFrame;
static s32 gSelectedPlayerIndex;
static s16 gBorderHeight;
static s16 gSmallBorderHeight;

static u32 gSnapSelectLayout[] = {
    0x0C000000,  0x05000000, 0x0B00002D, 0x00000000, 0x028001E0, 0x00000000, 0xFDBD0000,
    0x00000243,  0x05000000, 0x0D000000, 0x05000000, 0x0F000002, 0x05000000, 0x0A000000,
    &gGraphRoot0, 0x06000000, 0x06000000, 0x06000000, 0x06000000, 0x01000000,
};
static Vtx gPlayerInfoPanelVerts[] = {
    VTX(-32, 64, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),      VTX(-32, 32, 0, 0, 1024, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(32, 32, 0, 2048, 1024, 0xFF, 0xFF, 0xFF, 0xFF), VTX(32, 64, 0, 2048, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(-32, 32, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),      VTX(-32, 0, 0, 0, 1024, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(32, 0, 0, 2048, 1024, 0xFF, 0xFF, 0xFF, 0xFF),  VTX(32, 32, 0, 2048, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(-16, 39, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),      VTX(-16, 15, 0, 0, 768, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(16, 15, 0, 1024, 768, 0xFF, 0xFF, 0xFF, 0xFF),  VTX(16, 39, 0, 1024, 0, 0xFF, 0xFF, 0xFF, 0xFF),
};
static Gfx gPlayerInfoPanelDL[] = {
    gsSPSetGeometryMode(G_CULL_BACK),
    gsDPSetCombineMode(G_CC_DECALRGBA, G_CC_PASS2),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsSPVertex(gPlayerInfoPanelVerts, 12, 0),
    gsDPLoadTextureBlock(0x0F000000, G_IM_FMT_RGBA, G_IM_SIZ_16b, 64, 32, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                         G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
    gsDPLoadTextureBlock(0x0F001000, G_IM_FMT_RGBA, G_IM_SIZ_16b, 64, 32, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                         G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(4, 5, 6, 0, 4, 6, 7, 0),
    gsDPLoadTextureBlock(0x0E000000, G_IM_FMT_IA, G_IM_SIZ_8b, 32, 24, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                         G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(8, 9, 10, 0, 8, 10, 11, 0),
    gsSPEndDisplayList(),
};
static u32 gPlayerInfoPanelGeo[] = {
    0x17000000, 0x00000000, 0x00000000,    0x00000000, gPlayerInfoPanelVerts, 0x05000000, 0x22040000,
    0x00000000, 0x08000000, Gallery_RentalViewerNodePostCallback, gPlayerInfoPanelDL, 0x06000000, 0x01000000, 0x00000000,
};
static Vtx gPlayerInfoPanelTintedCardVerts[] = {
    VTX(-50, 68, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),     VTX(-50, 51, 0, 0, 544, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(50, 51, 0, 3200, 544, 0xFF, 0xFF, 0xFF, 0xFF), VTX(50, 68, 0, 3200, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(-50, 51, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),     VTX(-50, 34, 0, 0, 544, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(50, 34, 0, 3200, 544, 0xFF, 0xFF, 0xFF, 0xFF), VTX(50, 51, 0, 3200, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(-50, 34, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),     VTX(-50, 17, 0, 0, 544, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(50, 17, 0, 3200, 544, 0xFF, 0xFF, 0xFF, 0xFF), VTX(50, 34, 0, 3200, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(-50, 17, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),     VTX(-50, 0, 0, 0, 544, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(50, 0, 0, 3200, 544, 0xFF, 0xFF, 0xFF, 0xFF),  VTX(50, 17, 0, 3200, 0, 0xFF, 0xFF, 0xFF, 0xFF),
};
static Gfx gPlayerInfoPanelTintedCardDL[] = {
    gsSPSetGeometryMode(G_CULL_BACK),
    gsDPSetCombineLERP(0, 0, 0, TEXEL0, TEXEL0, 0, ENVIRONMENT, 0, 0, 0, 0, COMBINED, 0, 0, 0, COMBINED),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsSPVertex(gPlayerInfoPanelTintedCardVerts, 16, 0),
    gsDPLoadTextureBlock(0x0F000000, G_IM_FMT_RGBA, G_IM_SIZ_16b, 100, 17, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                         G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
    gsDPLoadTextureBlock(0x0F000D48, G_IM_FMT_RGBA, G_IM_SIZ_16b, 100, 17, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                         G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(4, 5, 6, 0, 4, 6, 7, 0),
    gsDPLoadTextureBlock(0x0F001A90, G_IM_FMT_RGBA, G_IM_SIZ_16b, 100, 17, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                         G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(8, 9, 10, 0, 8, 10, 11, 0),
    gsDPLoadTextureBlock(0x0F0027D8, G_IM_FMT_RGBA, G_IM_SIZ_16b, 100, 17, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                         G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(12, 13, 14, 0, 12, 14, 15, 0),
    gsSPEndDisplayList(),
};
static u32 gPlayerInfoPanelTintedCardGeo[] = {
    0x17000000, 0x00000000, 0x00000000,    0x00000000, gPlayerInfoPanelTintedCardVerts, 0x05000000, 0x22050000,
    0x00000000, 0x08000000, Gallery_RentalViewerNodePostCallback, gPlayerInfoPanelTintedCardDL, 0x06000000, 0x01000000, 0x00000000,
};
static Vtx gPlayerInfoPanelCardVerts[] = {
    VTX(-50, 64, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),     VTX(-50, 48, 0, 0, 512, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(50, 48, 0, 3200, 512, 0xFF, 0xFF, 0xFF, 0xFF), VTX(50, 64, 0, 3200, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(-50, 48, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),     VTX(-50, 32, 0, 0, 512, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(50, 32, 0, 3200, 512, 0xFF, 0xFF, 0xFF, 0xFF), VTX(50, 48, 0, 3200, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(-50, 32, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),     VTX(-50, 16, 0, 0, 512, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(50, 16, 0, 3200, 512, 0xFF, 0xFF, 0xFF, 0xFF), VTX(50, 32, 0, 3200, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(-50, 16, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),     VTX(-50, 0, 0, 0, 512, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(50, 0, 0, 3200, 512, 0xFF, 0xFF, 0xFF, 0xFF),  VTX(50, 16, 0, 3200, 0, 0xFF, 0xFF, 0xFF, 0xFF),
};
static Gfx gPlayerInfoPanelCardDL[] = {
    gsSPSetGeometryMode(G_CULL_BACK),
    gsDPSetCombineMode(G_CC_DECALRGBA, G_CC_PASS2),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsSPVertex(gPlayerInfoPanelCardVerts, 16, 0),
    gsDPLoadTextureBlock(0x0F000000, G_IM_FMT_RGBA, G_IM_SIZ_16b, 100, 16, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                         G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
    gsDPLoadTextureBlock(0x0F000C80, G_IM_FMT_RGBA, G_IM_SIZ_16b, 100, 16, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                         G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(4, 5, 6, 0, 4, 6, 7, 0),
    gsDPLoadTextureBlock(0x0F001900, G_IM_FMT_RGBA, G_IM_SIZ_16b, 100, 16, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                         G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(8, 9, 10, 0, 8, 10, 11, 0),
    gsDPLoadTextureBlock(0x0F002580, G_IM_FMT_RGBA, G_IM_SIZ_16b, 100, 16, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                         G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(12, 13, 14, 0, 12, 14, 15, 0),
    gsSPEndDisplayList(),
};
static u32 gPlayerInfoPanelCardGeo[] = {
    0x17000000, 0x00000000, 0x00000000,    0x00000000, gPlayerInfoPanelCardVerts, 0x05000000, 0x22040000,
    0x00000000, 0x08000000, Gallery_RentalViewerNodePostCallback, gPlayerInfoPanelCardDL, 0x06000000, 0x01000000, 0x00000000,
};
static Vtx gPlayerInfoPanelOverlayVerts[] = {
    VTX(-36, 12, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(-36, -12, 0, 0, 768, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(36, -12, 0, 2304, 768, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(36, 12, 0, 2304, 0, 0xFF, 0xFF, 0xFF, 0xFF),
};
static Gfx gPlayerInfoPanelOverlayDL[] = {
    gsSPSetGeometryMode(G_CULL_BACK),
    gsDPSetCombineLERP(0, 0, 0, 0, TEXEL0, 0, ENVIRONMENT, 0, 0, 0, 0, COMBINED, 0, 0, 0, COMBINED),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsSPVertex(gPlayerInfoPanelOverlayVerts, 4, 0),
    gsDPLoadTextureBlock_4b(0x0F000000, G_IM_FMT_I, 80, 24, 0, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP,
                            G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
    gsSPEndDisplayList(),
};
static u32 gPlayerInfoPanelOverlayGeo[] = {
    0x17000000, 0x00000000, 0x00000000,    0x00000000, gPlayerInfoPanelOverlayVerts, 0x05000000, 0x22050000,
    0x00000000, 0x08000000, Gallery_RentalViewerNodePostCallback, gPlayerInfoPanelOverlayDL, 0x06000000, 0x01000000,
};
static RentalSlotAnimData gPlayerIconAnimData[] = {
    {
        0x01,
        0x00,
        134,
        210,
        64,
        0,
        0,
        1472,
    },
    {
        0x01,
        0x00,
        258,
        210,
        64,
        -32768,
        -32768,
        1664,
    },
    {
        0x01,
        0x00,
        382,
        210,
        64,
        0,
        0,
        1408,
    },
    {
        0x01,
        0x00,
        506,
        210,
        64,
        -32768,
        -32768,
        1600,
    },
    {
        0x01,
        0x00,
        190,
        368,
        100,
        0,
        0,
        1568,
    },
    {
        0x01,
        0x00,
        450,
        368,
        100,
        -32768,
        -32768,
        1504,
    },
};
static s16 gIconEntryX[] = { 0x46, 0x28, 0x1E, 0x1E, 0x1E, 0x1E, 0x28, 0x3C, 0x5A, 0x78, 0x6E, 0x64 };
static s16 gIconEntryY[] = { 0x14, 0x3C, 0x5F, 0x78, 0x82, 0x87, 0x82, 0x78, 0x6E, 0x69, 0x66, 0x64 };
static s16 gIconSelectX[] = { 0x46, 0x32, 0x28, 0x1E, 0x19, 0x14, 0x14, 0x14, 0x19, 0x1E, 0x28, 0x32 };
static s16 gIconSelectY[] = { 0x64, 0x6E, 0x82, 0x96, 0xA0, 0xA0, 0x78, 0x50, 0x32, 0x1E, 0xA, 5 };

s32 Gallery_RentalViewerNodePostCallback(s32 arg0, GraphNode* arg1) {
    u8* var_a0;
    u8* var_a3;

    if (arg0 == 5) {
        switch (gDefaultGraphRoot->node.callbackArg) {
            case 0:
                // clang-format off
                var_a0 = gPlayerIconTextures[0]; var_a3 = D_201D820;
                break;

            case 1:
                var_a0 = gPlayerIconTextures[1]; var_a3 = D_201DB20;
                break;

            case 2:
                var_a0 = gPlayerIconTextures[2]; var_a3 = D_201DE20;
                break;

            case 3:
                var_a0 = gPlayerIconTextures[3]; var_a3 = D_201E120;
                // clang-format on
                break;

            case 4:
                var_a0 = D_201A620;
                var_a3 = NULL;
                break;

            case 5:
                var_a0 = gRentalIconTexEmpty;
                var_a3 = NULL;
                break;

            default:
                var_a0 = D_2028420;
                var_a3 = NULL;
                break;
        }

        gDPPipeSync(gDisplayListHead++);
        gDPSetEnvColor(gDisplayListHead++, 0, 0, 0, gDefaultGraphRoot->lodFraction);
        gSPSegment(gDisplayListHead++, 0x0F, Memmap_GetSegmentVaddr(var_a0));
        gSPSegment(gDisplayListHead++, 0x0E, Memmap_GetSegmentVaddr(var_a3));
        gSPDisplayList(gDisplayListHead++, arg1->callbackArg);

        GeoRender_ApplyMaterialState();
    }
    return 0;
}

void Gallery_RentalViewerAnimateSlotScale(s32 arg0, f32 arg1) {
    s16 var_a2;
    f32 var_fa0;

    if (arg0 == gSelectedPlayerIndex) {
        var_fa0 = 0.06f;
        var_a2 = 0x3000;
    } else {
        var_fa0 = 0.02f;
        var_a2 = 0x2000;
    }

    gPlayerIconNodes[0][arg0].scale.x = ((SINS(gPlayerIconAnimData[arg0].unk_0A) * var_fa0) + 1.0f) * arg1;
    gPlayerIconNodes[0][arg0].scale.y = ((COSS(gPlayerIconAnimData[arg0].unk_0A) * var_fa0) + 1.0f) * arg1;

    gPlayerIconNodes[1][arg0].scale.x = gPlayerIconNodes[0][arg0].scale.x;
    gPlayerIconNodes[1][arg0].scale.y = gPlayerIconNodes[0][arg0].scale.y;

    gPlayerIconAnimData[arg0].unk_0A += var_a2 + ((MathUtil_Random16() & 0xF) * 0x10);
}

void Gallery_RentalViewerAnimateSlotBob(s32 arg0, f32 arg1) {
    f32 var_fv0;
    s16 var_v0;

    if (arg0 == gSelectedPlayerIndex) {
        var_fv0 = 1.5f;
        var_v0 = gPlayerIconAnimData[arg0].unk_0C * 2;
    } else {
        var_fv0 = 1.0f;
        var_v0 = gPlayerIconAnimData[arg0].unk_0C;
    }

    gPlayerIconNodes[0][arg0].rotation.z = SINS(gPlayerIconAnimData[arg0].unk_08) * (1536.0f * var_fv0);
    gPlayerIconNodes[1][arg0].position.x = gPlayerIconNodes[0][arg0].position.x =
        (gPlayerIconAnimData[arg0].unk_02 - 320.0f) - (SINS(gPlayerIconAnimData[arg0].unk_08) * (6.0f * var_fv0));

    gPlayerIconAnimData[arg0].unk_08 += var_v0;
}

void Gallery_RentalViewerRefreshPortStatus(void) {
    s32 i;
    u8 temp_s3 = Cont_GetControllerBits();
    GbSavePlayerIdentity sp48;

    for (i = 0; i < 4; i++) {
        GbSave_CopyPlayerIdentity(i, &sp48);
        Text_UntranscodeName(gPlayerSlotData[i].unk_06, &sp48.unk_02);
        gPlayerSlotData[i].unk_04 = sp48.unk_00;
        gPlayerSlotData[i].unk_01 = GbSave_GetPortGame(i);
        gPlayerSlotData[i].unk_00 = GbSave_GetSaveState(i);
        gPlayerSlotData[i].unk_02 = GbSave_SavedAtPokemonCenter(i);
        gPlayerSlotData[i].unk_03 = (temp_s3 & (1 << i)) != 0;
    }
}

void Gallery_RentalViewerUpdatePortIcons(void) {
    s32 i;

    for (i = 0; i < 4; i++) {
        if (gPlayerSlotData[i].unk_01 != 0) {
            switch (gPlayerSlotData[i].unk_01) {
                case 1:
                    gPlayerIconTextures[i] = D_201E420;
                    break;

                case 2:
                    gPlayerIconTextures[i] = D_2020420;
                    break;

                case 3:
                    gPlayerIconTextures[i] = D_2022420;
                    break;

                case 7:
                    gPlayerIconTextures[i] = D_2024420;
                    break;
            }

            if (gPlayerSlotData[i].unk_00 != 0) {
                gPlayerIconAnimData[i].unk_01 = 0x14;
            }
        } else {
            gPlayerIconTextures[i] = &D_2026420;
            if (gPlayerSlotData[i].unk_00 == 2) {
                gPlayerIconAnimData[i].unk_01 = 0x15;
            } else {
                gPlayerIconAnimData[i].unk_00 = 0;
                gPlayerIconAnimData[i].unk_01 = 1;
            }
        }
    }
}

void Gallery_RentalViewerInitScene(void) {
    MemoryBlock* temp_s1;
    s32 i;
    ModelNodeView* temp_v0_2;
    DisplayObject* ptr;

    temp_s1 = MainPool_AllocState(main_pool_get_available(), 0);
    gRentalViewerState = 0;
    gSnapSelectRoot = process_geo_layout(temp_s1, gSnapSelectLayout);
    temp_v0_2 = process_geo_layout(temp_s1, gPlayerInfoPanelGeo);
    gSnapSelectNodes[0] = temp_v0_2;
    gSnapSelectNodes[1] = temp_v0_2;
    gSnapSelectNodes[2] = temp_v0_2;
    gSnapSelectNodes[3] = temp_v0_2;
    gSnapSelectNodes[4] = process_geo_layout(temp_s1, gPlayerInfoPanelCardGeo);
    gSnapSelectNodes[5] = process_geo_layout(temp_s1, gPlayerInfoPanelTintedCardGeo);
    gSnapSelectRentalNode = process_geo_layout(temp_s1, gPlayerInfoPanelOverlayGeo);
    MainPool_FinalizeAllocation(temp_s1);
    ModelRenderer_InitDisplayRoots();

    for (i = 0, ptr = &gPlayerIconNodes[0][0]; i < 12; ptr++, i++) {
        ModelRenderer_AttachDisplayObject(ptr);
        gPlayerIconNodes[0][i].node.callbackArg = i;
    }

    Gallery_RentalViewerRefreshPortStatus();
    Gallery_RentalViewerUpdatePortIcons();
}

void Gallery_RentalViewerInitSlotModels(void) {
    s32 i;

    for (i = 0; i < 6; i++) {
        if (gPlayerIconAnimData[i].unk_00 != 0) {
            Model_InitDisplayObject(gPlayerIconNodes[0] + i, 0, 0, gSnapSelectNodes[i]);
            Model_InitDisplayObject(gPlayerIconNodes[1] + i, 0, 0, gSnapSelectRentalNode);
            Vec3f_SetComponentsDuplicate(&gPlayerIconNodes[0][i].position, gPlayerIconAnimData[i].unk_02 - 0x140, 0xF0 - gPlayerIconAnimData[i].unk_04,
                          -579.0f);
            Vec3f_SetComponentsDuplicate(&gPlayerIconNodes[1][i].position, gPlayerIconAnimData[i].unk_02 - 0x140, 0xE4 - gPlayerIconAnimData[i].unk_04,
                          -579.0f);
            gPlayerIconNodes[1][i].lodFraction = 0xB4;
        }
    }
}

s32 Gallery_RentalViewerBeginZoomIntro(s32 arg0) {
    if (gRentalViewerState == 0) {
        Gallery_RentalViewerInitSlotModels();
        if (arg0 < 0) {
            for (gSelectedPlayerIndex = 0; gSelectedPlayerIndex < 5; gSelectedPlayerIndex++) {
                if (gPlayerIconAnimData[gSelectedPlayerIndex].unk_00 != 0) {
                    break;
                }
            }
        } else {
            gSelectedPlayerIndex = arg0;
        }
        gRentalViewerState = 1;
        gSnapSelectAnimFrame = 0;
        return 1;
    }
    return 0;
}

void Gallery_RentalViewerUpdateZoomIntro(void) {
    s32 i;

    for (i = 0; i < 12; i++) {
        gPlayerIconNodes[0][i].scale.x = gIconEntryX[gSnapSelectAnimFrame] / 100.0f;
        gPlayerIconNodes[0][i].scale.y = gIconEntryY[gSnapSelectAnimFrame] / 100.0f;
    }

    gBorderHeight = ((((gSnapSelectAnimFrame * 0x32) + 0x32) / 12) + 6) & (u16)~0x1;
    gSmallBorderHeight = ((((gSnapSelectAnimFrame * 0x24) + 0x24) / 12) + 4) & (u16)~0x1;

    gSnapSelectAnimFrame++;
    if (gSnapSelectAnimFrame == 0xC) {
        gRentalViewerState = 3;
    }
}

void Gallery_RentalViewerMoveCursorUp(void) {
    s32 i;
    s32 var_a0;
    s32 var_v1;

    if (gSelectedPlayerIndex >= 4) {
        if (gSelectedPlayerIndex == 4) {
            var_v1 = 0;
            var_a0 = 1;
        } else {
            var_v1 = 3;
            var_a0 = -1;
        }

        for (i = 0; i < 4; i++) {
            if (gPlayerIconAnimData[var_v1].unk_00 != 0) {
                gSelectedPlayerIndex = var_v1;
                Audio_PlaySoundEffectById(SFX_MENU_SCROLL);
                break;
            }
            var_v1 = (var_v1 + var_a0) & 3;
        }
    }
}

void Gallery_RentalViewerMoveCursorDown(void) {
    s32 var_v0;
    s32 i;

    if (gSelectedPlayerIndex < 4) {
        var_v0 = (gSelectedPlayerIndex / 2) + 4;

        for (i = 0; i < 2; i++) {
            if (gPlayerIconAnimData[var_v0].unk_00 != 0) {
                gSelectedPlayerIndex = var_v0;
                Audio_PlaySoundEffectById(SFX_MENU_SCROLL);
                break;
            }
            var_v0 ^= 1;
        }
    }
}

void Gallery_RentalViewerMoveCursorLeft(void) {
    s32 i;

    if ((gSelectedPlayerIndex > 0) && (gSelectedPlayerIndex < 4)) {
        for (i = gSelectedPlayerIndex - 1; i >= 0; i--) {
            if (gPlayerIconAnimData[i].unk_00 != 0) {
                gSelectedPlayerIndex = i;
                Audio_PlaySoundEffectById(SFX_MENU_SCROLL);
                break;
            }
        }
    } else if (gSelectedPlayerIndex == 5) {
        gSelectedPlayerIndex = 4;
        Audio_PlaySoundEffectById(SFX_MENU_SCROLL);
    }
}

void Gallery_RentalViewerMoveCursorRight(void) {
    s32 i;

    if (gSelectedPlayerIndex < 3) {
        for (i = gSelectedPlayerIndex + 1; i < 4; i++) {
            if (gPlayerIconAnimData[i].unk_00 != 0) {
                gSelectedPlayerIndex = i;
                Audio_PlaySoundEffectById(SFX_MENU_SCROLL);
                break;
            }
        }
    } else if (gSelectedPlayerIndex == 4) {
        gSelectedPlayerIndex = 5;
        Audio_PlaySoundEffectById(SFX_MENU_SCROLL);
    }
}

void Gallery_RentalViewerUpdateSlotSelect(void) {
    s32 i;

    if (BTN_IS_PRESSED(gPlayer1Controller, BTN_A)) {
        if (gPlayerIconAnimData[gSelectedPlayerIndex].unk_01 == 0) {
            Audio_PlaySoundEffectById(0x01100012);
            gRentalViewerState = 4;
        } else {
            Audio_PlaySoundEffectById(SFX_MENU_ERROR);
            gRentalViewerState = 6;
        }
    } else if (BTN_IS_PRESSED(gPlayer1Controller, BTN_B)) {
        gRentalViewerState = 5;
        Audio_PlaySoundEffectById(SFX_MENU_BACK);
    } else if (BTN_IS_PRESSED(gPlayer1Controller, BTN_DUP)) {
        Gallery_RentalViewerMoveCursorUp();
    } else if (BTN_IS_PRESSED(gPlayer1Controller, BTN_DDOWN)) {
        Gallery_RentalViewerMoveCursorDown();
    } else if (BTN_IS_PRESSED(gPlayer1Controller, BTN_DLEFT)) {
        Gallery_RentalViewerMoveCursorLeft();
    } else if (BTN_IS_PRESSED(gPlayer1Controller, BTN_DRIGHT)) {
        Gallery_RentalViewerMoveCursorRight();
    }

    for (i = 0; i < 6; i++) {
        Gallery_RentalViewerAnimateSlotScale(i, 1.0f);
        Gallery_RentalViewerAnimateSlotBob(i, 1.0f);
    }
}

void Gallery_RentalViewerBeginConfirmZoom(void) {
    s32 i;

    gSnapSelectAnimFrame--;
    if (gSnapSelectAnimFrame == 0) {
        for (i = 0; i < 6; i++) {
            if (i != gSelectedPlayerIndex) {
                ModelRenderer_ClearDisplayObject(&gPlayerIconNodes[0][i]);
                ModelRenderer_ClearDisplayObject(&gPlayerIconNodes[1][i]);
            }
        }
        gBorderHeight = 0;
        gSmallBorderHeight = 0;
        gRentalViewerState = 2;
    } else {
        for (i = 0; i < 6; i++) {
            if (i == gSelectedPlayerIndex) {
                Gallery_RentalViewerAnimateSlotScale(i, 1.0f);
                Gallery_RentalViewerAnimateSlotBob(i, 1.0f);
            } else {
                Gallery_RentalViewerAnimateSlotScale(i, gSnapSelectAnimFrame / 12.0f);
                Gallery_RentalViewerAnimateSlotBob(i, gSnapSelectAnimFrame / 12.0f);
            }
            gBorderHeight = ((((gSnapSelectAnimFrame * 0x32) + 0x32) / 12) + 6) & 0xFFFE;
            gSmallBorderHeight = ((((gSnapSelectAnimFrame * 0x24) + 0x24) / 12) + 4) & 0xFFFE;
        }
    }
}

void Gallery_RentalViewerFinishConfirmZoom(void) {
    s32 var_a1 = gSnapSelectAnimFrame;

    if (var_a1 > 0xC) {
        var_a1 = 0xC;
    }

    if (gSnapSelectAnimFrame < 0xC) {
        gPlayerIconNodes[0][gSelectedPlayerIndex].scale.x = gIconSelectX[var_a1] / 100.0f;
        gPlayerIconNodes[0][gSelectedPlayerIndex].scale.y = gIconSelectY[var_a1] / 100.0f;

        gPlayerIconNodes[1][gSelectedPlayerIndex].scale.x = gIconSelectX[var_a1] / 100.0f;
        gPlayerIconNodes[1][gSelectedPlayerIndex].scale.y = gIconSelectY[var_a1] / 100.0f;

        gPlayerIconNodes[0][gSelectedPlayerIndex].rotation.z = 0;
    } else if (gSnapSelectAnimFrame == 0xC) {
        ModelRenderer_ClearDisplayObject(&gPlayerIconNodes[0][gSelectedPlayerIndex]);
        ModelRenderer_ClearDisplayObject(&gPlayerIconNodes[1][gSelectedPlayerIndex]);
    } else if (gSnapSelectAnimFrame == 0xE) {
        gRentalViewerState = 0;
    }
    gSnapSelectAnimFrame++;
}

void Gallery_RentalViewerCancelSelection(void) {
    s32 i;

    gSnapSelectAnimFrame--;
    if (gSnapSelectAnimFrame == 0) {
        for (i = 0; i < 6; i++) {
            ModelRenderer_ClearDisplayObject(&gPlayerIconNodes[0][i]);
            ModelRenderer_ClearDisplayObject(&gPlayerIconNodes[1][i]);
        }

        gSelectedPlayerIndex = -2;
        gBorderHeight = 0;
        gSmallBorderHeight = 0;
        gRentalViewerState = 0;
    } else {
        for (i = 0; i < 6; i++) {
            Gallery_RentalViewerAnimateSlotScale(i, gSnapSelectAnimFrame / 12.0f);
            Gallery_RentalViewerAnimateSlotBob(i, gSnapSelectAnimFrame / 12.0f);
        }

        gBorderHeight = ((((gSnapSelectAnimFrame * 0x32) + 0x32) / 12) + 6) & (u16)~1;
        gSmallBorderHeight = ((((gSnapSelectAnimFrame * 0x24) + 0x24) / 12) + 4) & (u16)~1;
    }
}

void Gallery_RentalViewerUpdateBusyConfirm(void) {
    s32 i;
    s32 var_v0;

    if (gPlayerIconAnimData[gSelectedPlayerIndex].unk_01 == 0x15) {
        var_v0 = 0;
    } else {
        var_v0 = 1;
    }

    Gallery_OpenConfirmPrompt(var_v0, gPlayer1Controller);
    if (Gallery_GetConfirmPromptResult(var_v0) != -1) {
        gRentalViewerState = 3;
    }

    for (i = 0; i < 6; i++) {
        Gallery_RentalViewerAnimateSlotScale(i, 1.0f);
        Gallery_RentalViewerAnimateSlotBob(i, 1.0f);
    }
}

void Gallery_RentalViewerUpdate(void) {
    switch (gRentalViewerState) {
        case 1:
            Gallery_RentalViewerUpdateZoomIntro();
            break;

        case 2:
            Gallery_RentalViewerFinishConfirmZoom();
            break;

        case 3:
            Gallery_RentalViewerUpdateSlotSelect();
            break;

        case 4:
            Gallery_RentalViewerBeginConfirmZoom();
            break;

        case 5:
            Gallery_RentalViewerCancelSelection();
            break;

        case 6:
            Gallery_RentalViewerUpdateBusyConfirm();
            break;
    }
}

void Gallery_RentalViewerDrawCursorMarker(void) {

    if (gRentalViewerState == 3) {
        s32 tmp = gPlayerIconAnimData[gSelectedPlayerIndex].unk_02 - (gPlayerIconAnimData[gSelectedPlayerIndex].unk_06 / 2) - 0x1E;
        s32 tmp2 = gPlayerIconAnimData[gSelectedPlayerIndex].unk_04 - 0x20;

        Ui_DrawAnimatedTextureMarker(tmp, tmp2);
    }
}

void Gallery_RentalViewerDrawSlotFrame(s32 arg0, s16 arg1, s32 arg2) {
    Color_RGBA8 sp2C;

    if (gPlayerIconAnimData[arg0].unk_00 != 0) {
        if (gPlayerIconAnimData[arg0].unk_01 == 0) {
            Color_SetRGBA(&sp2C, 0x1E, 0x1E, 0x82, 0xFF);
        } else {
            Color_SetRGBA(&sp2C, 0x64, 0x1E, 0x1E, 0xFF);
        }
    } else {
        Color_SetRGBA(&sp2C, 0x1E, 0x1E, 0x82, 0x80);
    }
    TeamSelection_DrawTexturedFrame((arg0 << 7) + 0x44, arg1 + 0x54, 0x78, arg2, sp2C.r, sp2C.g, sp2C.b, sp2C.a);
}

void Gallery_RentalViewerDrawSlotLabel(s32 arg0) {
    UNUSED s32 pad;

    if (gPlayerIconAnimData[arg0].unk_00 != 0) {
        if (gPlayerIconAnimData[arg0].unk_01 == 0) {
            Gfx_SetEnvColor(0xFF, 0xFF, 0xFF, 0xFF);
            Font_SetActive(8, 0);
            Font_Printf(((arg0 << 7) - (Font_MeasureTextExtent(8, 0, gPlayerSlotData[arg0].unk_06) / 2)) + 0x80, 0x5C,
                          gPlayerSlotData[arg0].unk_06);
            Gfx_SetEnvColor(0xFF, 0xBE, 0xFF, 0xFF);
            Font_SetActive(4, 0);
            Font_Printf(((arg0 << 7) - (Font_MeasureTextExtent(4, 0, "ID  00000") / 2)) + 0x80, 0x73, "ID  %05d",
                          gPlayerSlotData[arg0].unk_04);
        } else {
            Gfx_SetEnvColor(0xFF, 0xFF, 0xFF, 0xFF);
            Font_SetActive(8, 0);
            Font_Printf(((arg0 << 7) - (Font_MeasureTextExtent(8, 0, "???????") / 2)) + 0x80, 0x5C, "???????");
            Gfx_SetEnvColor(0xFF, 0xBE, 0xFF, 0xFF);
            Font_SetActive(4, 0);
            Font_Printf(((arg0 << 7) - (Font_MeasureTextExtent(4, 0, "ID  *****") / 2)) + 0x80, 0x73, "ID  *****");
        }
    }
}

void Gallery_RentalViewerDrawLabels(void) {
    char* temp_v0;

    Font_BeginTranslucentTextRendering();
    Gallery_RentalViewerDrawSlotLabel(0);
    Gallery_RentalViewerDrawSlotLabel(1);
    Gallery_RentalViewerDrawSlotLabel(2);
    Gallery_RentalViewerDrawSlotLabel(3);
    Gfx_SetEnvColor(0xFF, 0xFF, 0xFF, 0xFF);
    Font_SetActive(0x10, 0);
    if (gSelectedPlayerIndex != 5) {
        temp_v0 = Gallery_GetUiString(0x35);
        Font_Printf(0x140 - (Font_MeasureTextExtent(0, 0, temp_v0) / 2), 0x198, temp_v0);
    }
    Font_SetActive(8, 0);
    temp_v0 = Gallery_GetUiString(0x33);
    Font_Printf(0xC0 - (Font_MeasureTextExtent(0, 0, temp_v0) / 2), 0x102, temp_v0);
    temp_v0 = Gallery_GetUiString(0x34);
    Font_Printf(0x1C0 - (Font_MeasureTextExtent(0, 0, temp_v0) / 2), 0x102, temp_v0);
    Font_EndTexturedTextRendering();
}

void Gallery_RentalViewerDrawTeamList(s32 arg0) {
    UNUSED s32 pad;
    s16 sp3A;
    s16 temp_s0;

    if ((gRentalViewerState != 0) && (arg0 != 0)) {
        temp_s0 = (0x38 - gBorderHeight) / 2;
        sp3A = (0x28 - gSmallBorderHeight) / 2;
        Geo_RenderRootNode(gSnapSelectRoot);
        Gallery_RentalViewerDrawCursorMarker();

        if (gBorderHeight > 0) {
            Gallery_RentalViewerDrawSlotFrame(0, temp_s0, gBorderHeight);
            Gallery_RentalViewerDrawSlotFrame(1, temp_s0, gBorderHeight);
            Gallery_RentalViewerDrawSlotFrame(2, temp_s0, gBorderHeight);
            Gallery_RentalViewerDrawSlotFrame(3, temp_s0, gBorderHeight);
            TeamSelection_DrawTexturedFrame(0x44, temp_s0 + 0xF0, 0xF8, gBorderHeight, 0x78, 0x1E, 0x64, 0xFF);
            TeamSelection_DrawTexturedFrame(0x144, temp_s0 + 0xF0, 0xF8, gBorderHeight, 0x78, 0x1E, 0x64, 0xFF);
        }

        if (gSmallBorderHeight > 0) {
            TeamSelection_DrawTexturedPanel(0x38, sp3A + 0x190, 0x210, gSmallBorderHeight, 0x1E, 0x1E, 0x82, 0x96);
        }

        if (gBorderHeight == 0x38) {
            Gallery_RentalViewerDrawLabels();
        }
    }
}

s32 Gallery_RentalViewerGetSelectedSlot(void) {
    if (gRentalViewerState == 0) {
        return gSelectedPlayerIndex;
    }
    return -1;
}
