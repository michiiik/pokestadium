#include "transfer_pak_select.h"
#include "src/geo_render.h"
#include "src/display_object_textures.h"
#include "src/poke_icon.h"
#include "src/model_renderer.h"
#include "src/graphics_textures.h"
#include "src/input.h"
#include "src/ui_graphics.h"
#include "src/gb_data.h"
#include "src/pokemon_stats.h"
#include "src/save_data.h"
#include "src/game_state.h"
#include "src/text_system.h"
#include "src/jpeg_stream.h"
#include "src/audio_sfx.h"
#include "src/audio_loop_point.h"
#include "src/gfx_buffer.h"
#include "src/matrix.h"
#include "src/controller.h"
#include "src/geo_layout.h"
#include "src/math_util.h"
#include "src/memmap.h"
#include "src/memory.h"
#include "src/stage_loader.h"

static u8* gMenuMiscAssets;
static PortStatus gEntryStatus[4];
static s16 gSelectedEntryIndex;
static PortGraphNode gEntryGraphNodes[4];
static GraphNode* gMenuGeoNode;
static GraphNode* gPortSelectRootNode;
static s16 gIsEntrySelectionActive;
static s32 gPresentationMode;
#if !defined(VERSION_JP)
static char** gEntryMessageBank;
static char gEntryIdBuffer[0x400];
#endif
static s16 gPulseAlpha;
static s16 gPrevSelectedEntryIndex;
static s32 gEntryConfirmState;

#if !defined(VERSION_JP)
static u32 gPortSelectErrMsgsJp[] = {
    0xA4B3A4CE, 0xA5ABA1BC, 0xA5C8A5EA, 0x0AA5C3A5, 0xB8A4CFA1, 0xA2A4C4A4, 0xAB0AA4A8, 0xA4DEA4BB, 0xA4F3A1A3,
    0x00000000, 0xA5ECA5DD, 0xA1BCA5C8, 0xA4ACA1A2, 0x0AA4DFA4, 0xC4A4ABA4, 0xEAA4DEA4, 0xBB0AA4F3, 0xA1A30000,
    0xA5DDA5B1, 0xA5E2A5F3, 0xA5BBA5F3, 0x0AA5BFA1, 0xBCA4C7A1, 0xA2A5ECA5, 0xDD0AA1BC, 0xA5C8A4B5, 0xA4ECA4C6,
    0xA4A40AA4, 0xDEA4BBA4, 0xF3A1A300, 0xA4BAA4AB, 0xA4F3A4AC, 0xA1A20AA4, 0xA2A4EAA4, 0xDEA4BBA4, 0xF3A1A300,
};
#else
// Port messages for TransferPak_GetPortMessageIndex results 1-4. The JP TransferPak_DrawPortBox prints these
// directly; the other builds print entries 0x0E-0x11 of string table 0x1F and keep the raw blob above.
static char gPortSelectErrMsgsJp[] = "このカートリ\nッジは、つか\nえません。";
static char D_81303C48[] = "レポートが、\nみつかりませ\nん。";
static char D_81303C68[] = "ポケモンセン\nターで、レポ\nートされてい\nません。";
static char D_81303C98[] = "ずかんが、\nありません。";
#endif

static s16 gMenuAnimTimer = 0;

static u32 gPortSelectGeoLayout[] = {
    0x0C000000,  0x05000000, 0x0B00002D, 0x00000000, 0x028001E0, 0x00000000, 0xFDBD0000, 0x00000243, 0x05000000,
    0x0D000000,  0x05000000, 0x14000000, 0x002D013B, 0xFFFFFF32, 0x16FFFFFF, 0x0F000002, 0x05000000, 0x0A000000,
    &gGraphRoot0, 0x06000000, 0x06000000, 0x06000000, 0x06000000, 0x01000000,
#if !defined(VERSION_JP)
    0x00000000,
#endif
};

#if defined(VERSION_JP)
// Prompt texts that the other builds read from string table 0x1F (entries 0x12 and 0x13) in
// TransferPak_DrawPortSelectOverlay.
static char* D_81303B94_jp = "ゲームボーイのカートリッジをえらんでください。";
static char* D_81303B98_jp = "ポケモンの カートリッジが セット\nされていません。 でんげんをきってから\nカートリッジを セットしてください。";
#endif

static Vtx gPortSelectVertices[] = {
    VTX(-32, 64, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),      VTX(-32, 32, 0, 0, 1024, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(32, 32, 0, 2048, 1024, 0xFF, 0xFF, 0xFF, 0xFF), VTX(32, 64, 0, 2048, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(-32, 32, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),      VTX(-32, 0, 0, 0, 1024, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(32, 0, 0, 2048, 1024, 0xFF, 0xFF, 0xFF, 0xFF),  VTX(32, 32, 0, 2048, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(-16, 39, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),      VTX(-16, 15, 0, 0, 768, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(16, 15, 0, 1024, 768, 0xFF, 0xFF, 0xFF, 0xFF),  VTX(16, 39, 0, 1024, 0, 0xFF, 0xFF, 0xFF, 0xFF),
};

static Gfx gPortSelectDisplayList[] = {
    gsSPSetGeometryMode(G_CULL_BACK),
    gsDPSetCombineLERP(0, 0, 0, TEXEL0, TEXEL0, 0, ENVIRONMENT, 0, 0, 0, 0, COMBINED, 0, 0, 0, COMBINED),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsSPVertex(gPortSelectVertices, 12, 0),
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

static u32 gPortSelectNodeData[] = {
    0x17000000, 0x00000000, 0x00000000,    0x00000000, gPortSelectVertices, 0x05000000, 0x22050000,
    0x00000000, 0x08000000, TransferPak_RenderPortNodeCallback, gPortSelectDisplayList, 0x06000000, 0x01000000,
};

static u32 gPortModelTextures[] = {
    D_201D820, D_201DB20, D_201DE20, D_201E120, NULL,
};

static u32 gPortGradientColors[] = {
    0x1E1E1E00,
    0x00006400,
    0x0000FF00,
};

static Color_RGB8 gGradientColor = { 0x64, 0x1E, 0x1E };
static s32 pad_D_81303F18 = 0;
static u8* gPakIconAnimFrames[] = { D_3000008, D_03000FD0, D_03001F98, D_03002F60, D_03003F28 };

s32 TransferPak_RenderPortNodeCallback(s32 arg0, GraphNode* arg1) {
    s32 idx;

    if (arg0 == 5) {
        idx = gDefaultGraphRoot->node.callbackArg;

        gDPPipeSync(gDisplayListHead++);

        if (gCurrentGameState == 0x25) {
            gDPSetScissor(gDisplayListHead++, G_SC_NON_INTERLACE, 0, 0, 639, 206);
        } else {
            gDPSetScissor(gDisplayListHead++, G_SC_NON_INTERLACE, 0, 0, 639, 202);
        }

        gDPSetEnvColor(gDisplayListHead++, 0, 0, 0, gDefaultGraphRoot->lodFraction);

        if (gEntryGraphNodes[idx].unk_01C != NULL) {
            gSPSegment(gDisplayListHead++, 0x0F, Memmap_GetSegmentVaddr(gEntryGraphNodes[idx].unk_01C));
            gSPSegment(gDisplayListHead++, 0x0E, Memmap_GetSegmentVaddr(gPortModelTextures[gEntryGraphNodes[idx].unk_002]));
            gSPDisplayList(gDisplayListHead++, arg1->callbackArg);
        }
        GeoRender_ApplyMaterialState();
    }
    return 0;
}

s32 TransferPak_GetPresentationMode(void) {
    GameOptions sp18;

    Save_EnsureBankLoaded(2);
    Save_GetOptions(&sp18);
    return sp18.gbTowerUnlockTier;
}

void TransferPak_LaunchSelectedPortAnim(void) {
    if (gSelectedEntryIndex >= 0) {
        gEntryGraphNodes[gSelectedEntryIndex].unk_000 = 1;
        gEntryGraphNodes[gSelectedEntryIndex].unk_020 = -gEntryGraphNodes[gSelectedEntryIndex].unk_028.position.x / 20.0f;
        gEntryGraphNodes[gSelectedEntryIndex].unk_024 = 36.0f;
        gIsEntrySelectionActive = 0;
    }
}

s32 TransferPak_GetPortErrorCode(s32 arg0) {
    PortStatus* ptr = &gEntryStatus[arg0];
    s32 var_v1;

    if (ptr->unk_01 != 0) {
        if (ptr->unk_00 == 0) {
            if (GbSave_HasPokedex(arg0) == 0) {
                var_v1 = 0x16;
            } else if (ptr->unk_03 != 0) {
                var_v1 = 0;
            } else {
                var_v1 = 0x13;
            }
        } else {
            var_v1 = 0x14;
        }
    } else if (ptr->unk_00 == 2) {
        var_v1 = 0x15;
    } else if (ptr->unk_04 != 0) {
        var_v1 = 1;
    } else {
        var_v1 = 2;
    }
    return var_v1;
}

s32 TransferPak_GetPortMessageIndex(s32 arg0) {
    UNUSED s32 pad;
    s32 sp18 = 0;
    s32 temp_v0 = TransferPak_GetPortErrorCode(arg0);

    if (gCurrentGameState == 0x25) {
        if (temp_v0 & 0x10) {
            switch (temp_v0) {
                case 20:
                    sp18 = 2;
                    break;

                case 19:
                    sp18 = 3;
                    break;

                case 21:
                    sp18 = 1;
                    break;

                case 22:
                    sp18 = 4;
                    break;
            }
        }
    } else if ((temp_v0 & 0x10) && (temp_v0 == 0x15)) {
        sp18 = 1;
    }
    return sp18;
}

void TransferPak_DrawPortBox(s32 arg0, s32 arg1, s32 arg2) {
    s16 tmp2;
    s16 tmp1;

    if (gEntryStatus[arg0].unk_00 == 1) {
        tmp1 = 0x80;
    } else {
        tmp1 = 0xFF;
    }

    if (TransferPak_GetPortMessageIndex(arg0) == 0) {
        TransferPak_DrawPromptPanel(arg1, arg2, 0x78, 0x38, 0x1E, 0x1E, 0x82, tmp1);
    } else {
        TransferPak_DrawPromptPanel(arg1, arg2, 0x78, 0x38, 0x64, 0x1E, 0x1E, tmp1);
    }

    if ((arg0 == gSelectedEntryIndex) && (gIsEntrySelectionActive == 1) && (TransferPak_GetPortMessageIndex(arg0) > 0)) {
        Ui_DrawGradientPanel(arg1 - 4, arg2 + 0x9C, 0x80, 0x74, &gGradientColor, &gGradientColor);
        tmp1 = arg1 + 0x28;
        tmp2 = (SINS(gMenuAnimTimer * 2) * 2) + (arg2 + 0x8C);
        TransferPak_DrawSpinningPakIcon(D_3006060, tmp1, tmp2, 0x30, 0x10, 1.0f);
    }

    Font_BeginTranslucentTextRendering();
    Gfx_SetEnvColor(0xFF, 0xFF, 0xFF, 0xFF);
#if defined(VERSION_JP)
    Font_SetActive(8, -2);
#else
    Font_SetActive(8, 0);
#endif
    Font_SetLineHeight(0x18);

    if ((arg0 == gSelectedEntryIndex) && (gIsEntrySelectionActive == 1)) {
        switch (TransferPak_GetPortMessageIndex(arg0)) {
#if defined(VERSION_JP)
            case 1:
                Font_Printf(arg1 + 5, arg2 + 0xAC, gPortSelectErrMsgsJp);
                break;

            case 2:
                Font_Printf(arg1 + 5, arg2 + 0xAC, D_81303C48);
                break;

            case 3:
                Font_Printf(arg1 + 5, arg2 + 0xAC, D_81303C68);
                break;

            case 4:
                Font_Printf(arg1 + 5, arg2 + 0xAC, D_81303C98);
                break;
#else
            case 1:
                Font_Printf(arg1 + 5, arg2 + 0xAC, Text_GetString(NULL, 0, gEntryMessageBank, 0xE));
                break;

            case 2:
                Font_Printf(arg1 + 5, arg2 + 0xAC, Text_GetString(NULL, 0, gEntryMessageBank, 0xF));
                break;

            case 3:
                Font_Printf(arg1 + 5, arg2 + 0xAC, Text_GetString(NULL, 0, gEntryMessageBank, 0x10));
                break;

            case 4:
                Font_Printf(arg1 + 5, arg2 + 0xAC, Text_GetString(NULL, 0, gEntryMessageBank, 0x11));
                break;
#endif
        }
    }

    Gfx_SetEnvColor(0xFF, 0xFF, 0xFF, 0xFF);

#if defined(VERSION_JP)
    // Left-aligned name and ID; Font_Printf formats the ID itself, so no sprintf buffer is used.
    if (gEntryStatus[arg0].unk_00 == 0) {
        Font_SetActive(8, -2);
        Font_Printf(arg1 + 0xA, arg2 + 0xA, gEntryStatus[arg0].unk_08);
        Font_SetActive(4, -3);
        Font_Printf(arg1 + 2, arg2 + 0x1E, "ID %05d", gEntryStatus[arg0].unk_06);
    } else if (gEntryStatus[arg0].unk_00 != 1) {
        Font_SetActive(8, -2);
        Font_Printf(arg1 + 0xA, arg2 + 0xA, "？？？？？");
        Font_SetActive(4, -3);
        Font_Printf(arg1 + 2, arg2 + 0x1E, "ID ＊＊＊＊＊");
    }
#else
    if (gEntryStatus[arg0].unk_00 == 0) {
        Font_SetActive(8, 0);
        Font_Printf((arg1 - (Font_MeasureTextExtent(0, 0, gEntryStatus[arg0].unk_08) / 2)) + 0x3C, arg2 + 0xA,
                      gEntryStatus[arg0].unk_08);
        Font_SetActive(4, 0);
        sprintf(gEntryIdBuffer, "ID %05d", gEntryStatus[arg0].unk_06);
        Font_Printf((arg1 - (Font_MeasureTextExtent(0, 0, gEntryIdBuffer) / 2)) + 0x3C, arg2 + 0x1E, gEntryIdBuffer);
    } else if (gEntryStatus[arg0].unk_00 != 1) {
        Font_SetActive(8, 0);
        Font_Printf((arg1 - (Font_MeasureTextExtent(0, 0, "?????") / 2)) + 0x3C, arg2 + 0xA, "?????");
        Font_SetActive(4, 0);
        Font_Printf((arg1 - (Font_MeasureTextExtent(0, 0, "ID ?????") / 2)) + 0x3C, arg2 + 0x1E, "ID ?????");
    }
#endif

    Font_EndTexturedTextRendering();
}

void TransferPak_DrawSpinningPakIcon(u8* arg0, s16 arg1, s16 arg2, s16 arg3, s16 arg4, f32 arg5) {
    UNUSED s32 pad;
    Gfx* gfx;
    u16 a1;
    u16 a2;
    u16 tmp1;
    u16 tmp2;

    gfx = gDisplayListHead;

    if (arg5 == 0.0f) {
        return;
    }

    a1 = arg1 << 2;
    a2 = arg2 << 2;

    tmp1 = a1 + (4.0f * arg3 * arg5);
    tmp2 = a2 + (4.0f * arg4 * arg5);

    gDPPipeSync(gfx++);

    gDPSetCycleType(gfx++, G_CYC_1CYCLE);
    gDPSetCombineMode(gfx++, G_CC_DECALRGBA, G_CC_DECALRGBA);
    gDPSetTextureLOD(gfx++, G_TL_TILE);
    gDPSetTextureFilter(gfx++, G_TF_BILERP);
    gDPSetTextureConvert(gfx++, G_TC_FILT);
    gDPSetTextureLUT(gfx++, G_TT_NONE);
    gDPSetTexturePersp(gfx++, G_TP_NONE);

    gDPPipeSync(gfx++);

    gDPLoadTextureBlock(gfx++, arg0, G_IM_FMT_RGBA, G_IM_SIZ_16b, arg3, arg4, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                        G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

    gSPTextureRectangle(gfx++, a1, a2, tmp1, tmp2, G_TX_RENDERTILE, 0, 0, 1024.0f / arg5, 1024.0f / arg5);

    gDisplayListHead = gfx;
}

void TransferPak_UpdatePulseAlpha(void) {
    static s16 gPulseTimer = 0;
    static u16 gPulseAlphaTable[] = { 0, 0x6400, 0, 0xFF00 };

    s16 temp_v0 = (gPulseTimer + 0xA) & 0x1FF;

    gPulseTimer = 0x100;

    if (temp_v0 < gPulseTimer) {
        gPulseAlpha = temp_v0;
        gPulseTimer = temp_v0;
    } else {
        gPulseAlpha = 0x1FF - temp_v0;
        gPulseTimer = temp_v0;
    }
}

void TransferPak_DrawPulsingGuideIcon(u8* arg0, s16 arg1, s16 arg2, s16 arg3, s16 arg4, f32 arg5, s16 arg6) {
    Gfx* gfx = gDisplayListHead;
    u16 tmpa1;
    u16 tmpa2;
    u16 tmp1;
    u16 tmp2;
    u16 v;

    if (arg5 != 0.0f) {
        gDPPipeSync(gfx++);

        tmpa1 = arg1 << 2;
        tmpa2 = arg2 << 2;
        tmp1 = tmpa1 + ((4.0f * arg3) * arg5);
        tmp2 = tmpa2 + ((4.0f * arg4) * arg5);
        v = 1024.0f / arg5;

        gDPSetCycleType(gfx++, G_CYC_1CYCLE);
        gDPSetPrimColor(gfx++, 0, 0, 255, 255, 230, gPulseAlpha & 0xFF);
        gDPSetEnvColor(gfx++, 0, 50, 255, 0);
        gDPSetRenderMode(gfx++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
        gDPSetCombineLERP(gfx++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, PRIMITIVE, 0, TEXEL0, 0, PRIMITIVE,
                          ENVIRONMENT, TEXEL0, ENVIRONMENT, PRIMITIVE, 0, TEXEL0, 0);
        gDPSetTextureLOD(gfx++, G_TL_TILE);
        gDPSetTextureFilter(gfx++, G_TF_BILERP);
        gDPSetTextureConvert(gfx++, G_TC_FILT);
        gDPSetTextureLUT(gfx++, G_TT_NONE);
        gDPSetTexturePersp(gfx++, G_TP_NONE);

        gDPPipeSync(gfx++);

        gDPLoadTextureBlock(gfx++, arg0, G_IM_FMT_I, G_IM_SIZ_8b, arg3, arg4, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                            G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

        if (arg6 > 0) {
            gSPTextureRectangle(gfx++, tmpa1, tmpa2, tmp1, tmp2, G_TX_RENDERTILE, 0, 0, v, v);
        } else {
            gSPTextureRectangle(gfx++, tmpa1, tmpa2, tmp1, tmp2, G_TX_RENDERTILE, arg3 << 5, 0, -v, v);
        }

        gDisplayListHead = gfx;
    }
}

void TransferPak_DrawPromptPanel(s16 arg0, s16 arg1, s16 arg2, s16 arg3, u8 arg4, u8 arg5, u8 arg6, u8 arg7) {
    s16 temp_a2;
    s16 temp_s0_32;
    s16 var_s2;
    s16 var_s3;

    if ((arg2 < 0x20) && (arg2 & 1)) {
        arg2++;
    }

    if ((arg3 < 0x20) && (arg3 & 1)) {
        arg3++;
    }

    if (arg2 >= 0x20) {
        var_s3 = 0x10;
    } else {
        var_s3 = arg2 / 2;
    }

    if (arg3 >= 0x20) {
        var_s2 = 0x10;
    } else {
        var_s2 = arg3 / 2;
    }

    temp_a2 = arg2 - (var_s3 * 2);
    temp_s0_32 = arg3 - (var_s2 * 2);

    gSPDisplayList(gDisplayListHead++, gUiEnvTintTextureDL);
    gDPSetEnvColor(gDisplayListHead++, arg4, arg5, arg6, arg7);

    gDPLoadTextureBlock_4b(gDisplayListHead++, D_20288E0, G_IM_FMT_I, 16, 16, 0, G_TX_NOMIRROR | G_TX_WRAP,
                           G_TX_NOMIRROR | G_TX_WRAP, 5, 5, G_TX_NOLOD, G_TX_NOLOD);

    Gfx_DrawTexturedRectClipped(arg0, arg1, var_s3, var_s2, 0, 0, 0x4000 / var_s3, 0x4000 / var_s2, 0);

    gDPLoadTextureBlock_4b(gDisplayListHead++, D_2028960, G_IM_FMT_I, 16, 16, 0, G_TX_NOMIRROR | G_TX_WRAP,
                           G_TX_NOMIRROR | G_TX_WRAP, 5, 5, G_TX_NOLOD, G_TX_NOLOD);

    Gfx_DrawTexturedRectClipped((arg0 + arg2) - var_s3, arg1, var_s3, var_s2, 0, 0, 0x4000 / var_s3, 0x4000 / var_s2, 0);

    gDPLoadTextureBlock_4b(gDisplayListHead++, D_20287E0, G_IM_FMT_I, 16, 16, 0, G_TX_NOMIRROR | G_TX_WRAP,
                           G_TX_NOMIRROR | G_TX_WRAP, 5, 5, G_TX_NOLOD, G_TX_NOLOD);

    Gfx_DrawTexturedRectClipped(arg0, (arg1 + arg3) - var_s2, var_s3, var_s2, 0, 0, 0x4000 / var_s3, 0x4000 / var_s2, 0);

    gDPLoadTextureBlock_4b(gDisplayListHead++, D_2028860, G_IM_FMT_I, 16, 16, 0, G_TX_NOMIRROR | G_TX_WRAP,
                           G_TX_NOMIRROR | G_TX_WRAP, 5, 5, G_TX_NOLOD, G_TX_NOLOD);

    Gfx_DrawTexturedRectClipped((arg0 + arg2) - var_s3, (arg1 + arg3) - var_s2, var_s3, var_s2, 0, 0, 0x4000 / var_s3,
                  0x4000 / var_s2, 0);

    gSPDisplayList(gDisplayListHead++, gUiTranslucentFillDL);

    if ((temp_a2 > 0) && (temp_s0_32 > 0)) {
        Gfx_FillRectRgba(arg0 + var_s3, arg1, temp_a2, var_s2, arg4, arg5, arg6, arg7);
        Gfx_FillRectRgba(arg0, arg1 + var_s3, arg2, temp_s0_32, arg4, arg5, arg6, arg7);
        Gfx_FillRectRgba(arg0 + var_s3, arg1 + var_s3 + temp_s0_32, temp_a2, var_s2, arg4, arg5, arg6, arg7);
    } else if (temp_a2 > 0) {
        Gfx_FillRectRgba(arg0 + var_s3, arg1, temp_a2, arg3, arg4, arg5, arg6, arg7);
    } else if (temp_s0_32 > 0) {
        Gfx_FillRectRgba(arg0, arg1 + var_s2, arg2, temp_s0_32, arg4, arg5, arg6, arg7);
    }

    gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);
}

void TransferPak_DrawPortSelectOverlay(void) {
    static s32 gBgTextureIndex = 0;

    TransferPak_UpdatePulseAlpha();

    if (gCurrentGameState == 0x25) {
        TransferPak_DrawSpinningPakIcon(gPakIconAnimFrames[gBgTextureIndex], 0x112, 0x11A, 0x30, 0x2A, 2.0f);
        gBgTextureIndex = (gBgTextureIndex + 1) % 5;
        TransferPak_DrawPulsingGuideIcon(D_30055B0, 0x98, 0xEE, 0x20, 0x23, 2.0f, 1);
        TransferPak_DrawPulsingGuideIcon(D_3004EF0, 0xC2, 0x100, 0x28, 0x2B, 2.0f, 1);
        TransferPak_DrawPulsingGuideIcon(D_30055B0, 0x1AA, 0xEE, 0x20, 0x23, 2.0f, -1);
        TransferPak_DrawPulsingGuideIcon(D_3004EF0, 0x172, 0xFC, 0x28, 0x2B, 2.0f, -1);
        TransferPak_DrawPulsingGuideIcon(D_3005A18, 0x102, 0xAC, 0x40, 0x19, 2.0f, 1);
    }

    if (gSelectedEntryIndex >= 0) {
        TransferPak_DrawPromptPanel(0x38, 0x19A, 0x210, 0x28, 0x1E, 0x1E, 0x82, 0x80);
        Font_BeginTranslucentTextRendering();
#if defined(VERSION_JP)
        Font_SetActive(0x10, -1);
        Font_Printf(0x3B, 0x1A1, D_81303B94_jp);
#else
        Font_SetActive(0x10, 0);
        Font_Printf(0x132 - (Font_MeasureTextExtent(0, 0, Text_GetString(NULL, 0, gEntryMessageBank, 0x12)) / 2), 0x1A1,
                      Text_GetString(NULL, 0, gEntryMessageBank, 0x12));
#endif
        Font_EndTexturedTextRendering();
        TransferPak_DrawPortBox(0, 0x38, 0x2E);
        TransferPak_DrawPortBox(1, 0xBC, 0x2E);
        TransferPak_DrawPortBox(2, 0x140, 0x2E);
        TransferPak_DrawPortBox(3, 0x1C4, 0x2E);
        if (gIsEntrySelectionActive == 1) {
            Ui_DrawTextureMarker((SINS(gMenuAnimTimer) * 5.0f) + ((gSelectedEntryIndex * 0x84) + 0x38), 0x7E);
        }
    } else {
        TransferPak_DrawPromptPanel(0x46, 0x146, 0x1F4, 0x78, 0x82, 0x1E, 0x1E, 0x80);
        Font_BeginTranslucentTextRendering();
#if defined(VERSION_JP)
        Font_SetActive(0x10, -2);
        Font_SetLineHeight(0x1C);
        Font_Printf(0x5A, 0x15A, D_81303B98_jp);
#else
        Font_SetActive(0x10, 0);
        Font_SetLineHeight(0x1C);
        Font_Printf(0x5A, 0x15A, Text_GetString(NULL, 0, gEntryMessageBank, 0x13));
#endif
        Font_EndTexturedTextRendering();
    }
    gMenuAnimTimer += 0xB80;
}

void TransferPak_MovePortCursor(void) {
    if (gIsEntrySelectionActive == 0) {
        return;
    }

    gPrevSelectedEntryIndex = gSelectedEntryIndex;

    if (gPlayer1Controller->buttonPressed & 0x200) {
        while (true) {
            gSelectedEntryIndex = (gSelectedEntryIndex + 3) % 4;

            if (gEntryStatus[gSelectedEntryIndex].unk_01 != 0 || TransferPak_GetPortMessageIndex(gSelectedEntryIndex) > 0) {
                break;
            }
        }
    } else if (gPlayer1Controller->buttonPressed & 0x100) {
        while (true) {
            gSelectedEntryIndex = (gSelectedEntryIndex + 1) % 4;

            if (gEntryStatus[gSelectedEntryIndex].unk_01 != 0 || TransferPak_GetPortMessageIndex(gSelectedEntryIndex) > 0) {
                break;
            }
        }
    }

    if (gSelectedEntryIndex != gPrevSelectedEntryIndex) {
        gMenuAnimTimer = 0;
        Audio_PlaySoundEffectById(SFX_PAK_CURSOR);
    }
}

void TransferPak_HandlePortSelectInput(void) {
    s32 var_v0;

    if (gIsEntrySelectionActive > 0) {
        if ((gPlayer1Controller->buttonPressed & 0x8000) && (gEntryConfirmState != 1)) {
            var_v0 = 0;
            if (gSelectedEntryIndex < 0) {
                var_v0 = 1;
            }

            if ((var_v0 == 0) && (TransferPak_GetPortMessageIndex(gSelectedEntryIndex) != 1)) {
                if (gCurrentGameState == 0x25) {
                    Audio_PlaySoundEffectById(SFX_PAK_LAB_SELECT);
                } else {
                    Audio_PlaySoundEffectById(SFX_PAK_TOWER_SELECT);
                }
                gEntryConfirmState = 1;
                TransferPak_LaunchSelectedPortAnim();
            } else {
                Audio_PlaySoundEffectById(SFX_MENU_ERROR);
            }
        } else if ((gPlayer1Controller->buttonPressed & 0x4000) && (gEntryConfirmState != 2)) {
            Audio_PlaySoundEffectById(SFX_MENU_BACK);
            gEntryConfirmState = 2;
        } else if (gSelectedEntryIndex >= 0) {
            TransferPak_MovePortCursor();
        }
    }
}

void TransferPak_PollInput(void) {
    Cont_StartReadInputs();
    Cont_ReadInputs();
    Input_ResetRepeatState();
}

void TransferPak_DrawFrame(s32 arg0) {
    BgStage_DrawFrame();
    if (gCurrentGameState == 0x25) {
        Gfx_DrawBackgroundGrid(gMenuMiscAssets);
    } else {
        Gfx_DrawMenuBackground();
    }

    if (arg0 != 0) {
        TransferPak_HandlePortSelectInput();
    }

    TransferPak_DrawPortSelectOverlay();
    Geo_RenderRootNode(gPortSelectRootNode);
    BgStage_AdvanceFrame();
}

void TransferPak_ScanAllPorts(void) {
    s32 i;
    u8 temp_s3;
    GbSavePlayerIdentity sp48;

    temp_s3 = Cont_GetControllerBits();

    for (i = 0; i < 4; i++) {
        GbSave_CopyPlayerIdentity(i, &sp48);
        Text_UntranscodeNameWrapper(gEntryStatus[i].unk_08, sp48.unk_02);
        gEntryStatus[i].unk_06 = sp48.unk_00;
        gEntryStatus[i].unk_01 = GbSave_GetPortGame(i);
        gEntryStatus[i].unk_00 = GbSave_GetSaveState(i);
        gEntryStatus[i].unk_03 = GbSave_SavedAtPokemonCenter(i);
        gEntryStatus[i].unk_04 = (temp_s3 & (1 << i)) != 0;
        gEntryStatus[i].unk_02 = GbSave_GetPartyCount(i);
    }
}

void TransferPak_AutoSelectPort(void) {
    s32 i;
    s32 sp28 = GbSave_GetPresentPortMask();
    s32 var_s1 = 0;

    if (sp28 == 0) {
        for (i = 0; i < 4; i++) {
            if (TransferPak_GetPortMessageIndex(i) == 1) {
                var_s1++;
            }
        }
    }

    if ((sp28 == 0) && (var_s1 == 0)) {
        gSelectedEntryIndex = -1;
        return;
    }

    gSelectedEntryIndex = GbSave_GetActivePort();

    if (sp28 == 0) {
        gSelectedEntryIndex = 0;
        while (TransferPak_GetPortMessageIndex(gSelectedEntryIndex) != 1) {
            gSelectedEntryIndex++;
        }
    } else if ((gSelectedEntryIndex == -1) || (gEntryStatus[gSelectedEntryIndex].unk_01 == 0)) {
        gSelectedEntryIndex = 0;
        while (gEntryStatus[gSelectedEntryIndex].unk_01 == 0) {
            gSelectedEntryIndex++;
        }
    }
}

void TransferPak_AnimatePortModels(void) {
    static s16 gPortAnimSines[] = { 0x5C0, 0x680, 0x580, 0 };

    f32 var_fv0;
    f32 var_fv1;
    s16 var_a1;
    s32 var_s1;
    PortGraphNode* var_s0;
    DisplayObject* var_v0;
    s32 i;

    for (i = 0, var_s0 = &gEntryGraphNodes[0]; i < 4; i++, var_s0++) {
        switch (var_s0->unk_000) {
            case 0:
                if (i == gSelectedEntryIndex) {
                    var_fv0 = 1.5f;
                    var_s0->unk_00A = gMenuAnimTimer + 0x4000;
                    var_a1 = 0;
                    var_fv1 = 0.06f;
                    var_s1 = 0x3000;
                } else {
                    var_fv0 = 1.0f;
                    var_a1 = gPortAnimSines[0];
                    var_fv1 = 0.02f;
                    var_s1 = 0x2000;
                }

                var_s0->unk_004 = var_s0->unk_00E - (SINS(var_s0->unk_00A) * (6.0f * var_fv0));
                var_s0->unk_010 = SINS(var_s0->unk_00A) * (1536.0f * var_fv0);
                var_s0->unk_014 = (SINS(var_s0->unk_00C) * var_fv1) + 1.0f;
                var_s0->unk_018 = (COSS(var_s0->unk_00C) * var_fv1) + 1.0f;

                var_s0->unk_00A += var_a1;
                var_s0->unk_00C += var_s1 + ((MathUtil_Random16() & 0xF) * 0x10);

                var_s0->unk_028.position.z = -579.0f;
                var_s0->unk_028.rotation.z = var_s0->unk_010;
                var_s0->unk_028.scale.x = var_s0->unk_014;
                var_s0->unk_028.scale.y = var_s0->unk_018;
                break;

            case 1:
                var_v0 = &var_s0->unk_028;

                var_v0->position.x += var_s0->unk_020;
                if (var_s0->unk_020 < 0.0f) {
                    if (var_v0->position.x < 0.0f) {
                        var_v0->position.x = 0.0f;
                    }
                } else if ((var_s0->unk_020 > 0.0f) && (var_v0->position.x > 0.0f)) {
                    var_v0->position.x = 0.0f;
                }

                var_v0->position.y += var_s0->unk_024 * 0.5f;
                var_v0->position.z = -569.0f;
                var_s0->unk_024 -= 4.0f;

                var_v0->rotation.z = 0;
                var_v0->scale.x = 1.0f;
                var_v0->scale.y = 1.0f;
                break;
        }
    }
}

s32 TransferPak_PortSelectLoop(s32 arg0) {
    s32 var_s0 = 1;
    s32 var_s1 = 0;
    s32 i;

    StageFade_StartFromOpaque(8);

    while (StageContext_GetFadeMode() != 0) {
        TransferPak_PollInput();
        TransferPak_AnimatePortModels();
        TransferPak_DrawFrame(0);
    }

    if (arg0 == 1) {
        Audio_StartMusicTrack(9);
    }

    while (var_s0 != 0) {
        TransferPak_PollInput();
        TransferPak_AnimatePortModels();
        TransferPak_DrawFrame(1);

        if (gEntryConfirmState > 0) {
            if (gPlayer1Controller->buttonPressed & 0x8000) {
                var_s0 = 0;
            } else if (gPlayer1Controller->buttonPressed & 0x4000) {
                var_s1 = -1;
                var_s0 = 0;
            }
        }
    }

    if (var_s1 != -1) {
        for (i = 0; i < 20; i++) {
            TransferPak_PollInput();
            TransferPak_AnimatePortModels();
            TransferPak_DrawFrame(1);
        }
    }

    if ((arg0 == 1) && (var_s1 == -1)) {
        Audio_FadeOutAll(0x20);
    }

    StageContext_SetClearColor(1);
    StageFade_StartFromTransparent(8);

    while (StageContext_GetFadeMode() != 1) {
        TransferPak_PollInput();
        TransferPak_AnimatePortModels();
        TransferPak_DrawFrame(0);
    }

    if ((var_s1 != -1) && (gSelectedEntryIndex >= 0)) {
        GbSave_SetActivePort(gSelectedEntryIndex);
    }

    return var_s1;
}

void TransferPak_InitPortModels(void) {
    static s16 gPortModelOrder[] = { 1, 2, 0, 3 };

    DisplayObject* ptr;
    s32 temp_s1;
    s32 i;

    gEntryConfirmState = 0;

    for (i = 0; i < 4; i++) {
        temp_s1 = gPortModelOrder[i];
        ptr = &gEntryGraphNodes[temp_s1].unk_028;

        ModelRenderer_AttachDisplayObject(ptr);
        Vec3f_SetComponentsDuplicate(&ptr->position, ((temp_s1 * 0x84) + 0x7A) - 320.0f, 64.0f, -579.0f);
        gEntryGraphNodes[temp_s1].unk_002 = temp_s1;
        gEntryGraphNodes[temp_s1].unk_000 = 0;

        switch (GbSave_GetPortGame(temp_s1)) {
            case 1:
                gEntryGraphNodes[temp_s1].unk_01C = D_201E420;
                break;

            case 2:
                gEntryGraphNodes[temp_s1].unk_01C = D_2020420;
                break;

            case 3:
                gEntryGraphNodes[temp_s1].unk_01C = D_2022420;
                break;

            case 7:
                gEntryGraphNodes[temp_s1].unk_01C = D_2024420;
                break;

            default:
                gEntryGraphNodes[temp_s1].unk_01C = NULL;
                if (TransferPak_GetPortMessageIndex(temp_s1) > 0) {
                    gEntryGraphNodes[temp_s1].unk_01C = D_3006668;
                }
                break;
        }

        Model_InitDisplayObject(ptr, 0, 0, gMenuGeoNode);
        ptr->node.callbackArg = temp_s1;
    }
}

void TransferPak_InitScene(void) {
    MemoryBlock* sp1C = MainPool_AllocState(main_pool_get_available(), 0);

    gPortSelectRootNode = process_geo_layout(sp1C, &gPortSelectGeoLayout);
    gMenuGeoNode = process_geo_layout(sp1C, &gPortSelectNodeData);
    MainPool_FinalizeAllocation(sp1C);
    ModelRenderer_InitDisplayRoots();
    TransferPak_InitPortModels();
    gIsEntrySelectionActive = 1;
}

s32 TransferPak_SelectMain(s32 arg0, UNUSED s32 arg1) {
    StageContext* sp24;
    s32 sp20;

    main_pool_push_state('GBSL');

    Gfx_InitDisplayListBuffers(0x10000, 0);
    sp24 = StageContext_Allocate(1, 0, 2, 0, 2, 1);
    Font_Init(0x1C, 0);

    ASSET_LOAD(D_1000000, common_menu1_ui, 0);
    ASSET_LOAD(D_8000000, _6EC4D0, 0);
    ASSET_LOAD(D_2000000, common_menu2_ui, 0);
    ASSET_LOAD(D_3000000, trade_select_ui, 0);
    ASSET_LOAD(D_3000000_2, gb_pak_select_ui, 0);

    FRAGMENT_LOAD(FRAGMENT_PARTICLE_DATA_LIBRARY);

    PokeIcon_OpenModelArchives();
    PokeIcon_OpenModelArchive();
    TransferPak_ScanAllPorts();
    TransferPak_AutoSelectPort();
#if !defined(VERSION_JP)
    Text_InitStringTables();
    gEntryMessageBank = Text_GetStringTable(0x1F);
#endif
    TransferPak_InitScene();
    gPresentationMode = TransferPak_GetPresentationMode();

    gMenuMiscAssets = ASSET_LOAD(0, fragment33_misc, 0);

    StageContext_Activate(sp24);
    sp20 = TransferPak_PortSelectLoop(arg0);
    StageContext_Deactivate();
    Gfx_FreeDisplayListBuffers();

    main_pool_pop_state('GBSL');

    return sp20;
}

void Gfx_DrawBackgroundGrid(u8* arg0) {
    Gfx* gfx = gDisplayListHead;
    s32 i;
    s32 j;

    gDPPipeSync(gfx++);
    gDPSetCycleType(gfx++, G_CYC_1CYCLE);
    gSPClearGeometryMode(gfx++, G_ZBUFFER);
    gSPSetGeometryMode(gfx++, 0);
    gDPSetCombineMode(gfx++, G_CC_DECALRGBA, G_CC_DECALRGBA);
    gDPSetTextureLOD(gfx++, G_TL_TILE);
    gDPSetTextureFilter(gfx++, G_TF_BILERP);
    gDPSetTextureConvert(gfx++, G_TC_FILT);
    gDPSetTextureLUT(gfx++, G_TT_NONE);
    gDPSetTexturePersp(gfx++, G_TP_NONE);

    for (i = 0; i < 15; i++) {
        for (j = 0; j < 20; j++) {
            gDPPipeSync(gfx++);

            gDPLoadTextureBlock(gfx++, arg0 + (i * 0x14 + j) * 0x200, G_IM_FMT_RGBA, G_IM_SIZ_16b, 16, 16, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                                G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
            gSPTextureRectangle(gfx++, (j * 0x20) << 2, (i * 0x20) << 2, ((j + 1) * 0x20) << 2, ((i + 1) * 0x20) << 2,
                                G_TX_RENDERTILE, 0, 0, 0x0200, 0x0200);
        }
    }

    gDisplayListHead = gfx;
}

void Gfx_DrawMenuBackground(void) {
    Gfx* gfx = gDisplayListHead;
    u8* sp30;
    s32 i;

    gDPPipeSync(gfx++);

    gDPSetCycleType(gfx++, G_CYC_1CYCLE);
    gSPClearGeometryMode(gfx++, G_ZBUFFER);
    gSPSetGeometryMode(gfx++, 0);
    gDPSetCombineMode(gfx++, G_CC_BLENDPEDECALA, G_CC_BLENDPEDECALA);
    gDPSetTextureLOD(gfx++, G_TL_TILE);
    gDPSetTextureFilter(gfx++, G_TF_BILERP);
    gDPSetTextureConvert(gfx++, G_TC_FILT);
    gDPSetTextureLUT(gfx++, G_TT_NONE);
    gDPSetTexturePersp(gfx++, G_TP_NONE);

    gDPPipeSync(gfx++);

    gDPLoadTextureBlock_4b(gfx++, D_3008670, G_IM_FMT_IA, 128, 64, 0, G_TX_NOMIRROR | G_TX_WRAP,
                           G_TX_NOMIRROR | G_TX_WRAP, 7, 7, G_TX_NOLOD, G_TX_NOLOD);

    for (i = 0; i < 8; i++) {
        gDPPipeSync(gfx++);

        gDPSetPrimColor(gfx++, 0, 0, (0xBE0 - (i * 0xBE)) / 16, (0xDC0 - (i * 0xDC)) / 16, (0x7D0 - (i * 0x7D)) / 16,
                        255);
        gDPSetEnvColor(gfx++, (0x140 - (i * 0x14)) / 16, (0x5F0 - (i * 0x5F)) / 16, (0xA0 - (i * 10)) / 16, 255);
        gSPTextureRectangle(gfx++, 0, (i * 0x40) << 2, 0x0A00, ((i + 1) * 0x40) << 2, G_TX_RENDERTILE, 0, 0, 0x0400,
                            0x0400);
    }

    switch (gPresentationMode) {
        default:
            sp30 = D_3009678;
            break;

        case 0:
            sp30 = D_3009678;
            break;

        case 1:
            sp30 = D_3012D80;
            break;

        case 2:
            sp30 = D_301C488;
            break;
    }

    gDPSetRenderMode(gfx++, AA_EN | CVG_DST_CLAMP | ZMODE_OPA | CVG_X_ALPHA | ALPHA_CVG_SEL | FORCE_BL | G_RM_PASS,
                     AA_EN | CVG_DST_CLAMP | ZMODE_OPA | CVG_X_ALPHA | ALPHA_CVG_SEL | FORCE_BL | G_RM_NOOP2);
    gDPSetCombineMode(gfx++, G_CC_DECALRGBA, G_CC_DECALRGBA);

    for (i = 0; i < 9; i++) {
        gDPPipeSync(gfx++);

        gDPLoadTextureBlock(gfx++, &sp30[i * 0x1000], G_IM_FMT_RGBA, G_IM_SIZ_16b, 128, 16, 0,
                            G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK,
                            G_TX_NOLOD, G_TX_NOLOD);
        gSPTextureRectangle(gfx++, 0x03E8, (0xCD + (i * 0x10)) << 2, 0x05E8, (0xDD + (i * 0x10)) << 2, G_TX_RENDERTILE,
                            0, 0, 0x0400, 0x0400);
    }

    gDPLoadTextureBlock(gfx++, &sp30[0x9000], G_IM_FMT_RGBA, G_IM_SIZ_16b, 128, 7, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                        G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
    gSPTextureRectangle(gfx++, 0x03E8, 0x0574, 0x05E8, 0x0590, G_TX_RENDERTILE, 0, 0, 0x0400, 0x0400);

    gDisplayListHead = gfx;
}
