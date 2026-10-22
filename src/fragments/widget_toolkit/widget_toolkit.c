#include "widget_toolkit.h"
#include "src/graphics_textures.h"
#include "src/text_system.h"
#include "src/controller.h"

extern u32 gPlayerIconTex_0[];
extern u32 gPlayerIconTex_1[];
extern u32 gPlayerIconTex_2[];
extern u32 gPlayerIconTex_3[];
extern u32 gPlayerIconTex_4[];
extern u32 gPlayerIconTex_5[];
extern u32 gPlayerIconTex_6[];
extern u32 gPlayerIconTex_7[];
extern u32 gResultCubeTex_8[];
extern u32 gBorderTexture_Left[];
extern u32 gBorderTexture_Middle[];
extern u32 gBorderTexture_Fill[];
extern u32 gBorderTexture_Right[];

char** gMinigameNamesBank = NULL;

static s32 gRandomCounter;
static char** gControllerWarningBank;
u16 gComHyperUnlocked;
s16 gMenuCursorX;
s16 gMenuCursorY;

s16 gMiniHudColorDefaults[4];
s16 gMinigameComDifficulty;
s16 gWinsToChampion;
s16 gPlayerWinCounts[4];
s16 gMenuSelectedOption;

static s32 pad_D_8780FA4C;

s16 gMenuOptionUsedFlags[9];
s16 gMenuOptionFlags[4];

void Widget_Init(void) {
    Text_InitStringTables();
    gMinigameNamesBank = Text_GetStringTable(0x1D);
    gControllerWarningBank = Text_GetStringTable(0x14);
}

void Widget_DrawControllerDisconnectBanner(void) {
    static s32 gScrollingMessageOffset = 0;

    s32 sp24;
    s16 temp_s0;

    if (!(Cont_GetControllerBits() & 1)) {
        sp24 = Font_MeasureTextExtent(4, 0, Text_GetString(NULL, 0, gControllerWarningBank, 0));
        temp_s0 = 0x50 - gScrollingMessageOffset;

        gDPSetScissor(gDisplayListHead++, G_SC_NON_INTERLACE, 16, 0, 304, 240);

        Font_BeginTranslucentTextRendering();
        Font_SetActive(4, 0);
        Font_Printf(temp_s0, 0xB3, Text_GetString(NULL, 0, gControllerWarningBank, 0));

        temp_s0 += sp24 + 0x1E;
        if (temp_s0 < 0x140) {
            Font_Printf(temp_s0, 0xB3, Text_GetString(NULL, 0, gControllerWarningBank, 0));
        }

        Font_EndTexturedTextRendering();

        gDPSetScissor(gDisplayListHead++, G_SC_NON_INTERLACE, 0, 0, 320, 240);

        gScrollingMessageOffset++;
        if (gScrollingMessageOffset >= (sp24 + 0x50)) {
            gScrollingMessageOffset = 0x32;
        }
    }
}

void Rand_Advance(void) {
    gRandomCounter++;
}

u32 Rand_Range(u32 arg0) {
    u32 temp_a0 = guRandom() + gRandomCounter;
    u32 var_v1;

    if (arg0 == 0) {
        var_v1 = 0;
    } else {
        var_v1 = (temp_a0 / 10) % arg0;
    }

    return var_v1;
}

void Widget_DrawProgressBar(s32 arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4) {
    UNUSED s32 pad;
    s32 spF0 = arg2 - 2;
    s32 temp_lo = ((arg4 - arg3) * spF0) / arg4;
    s32 sp1C = (arg2 - temp_lo) - 2;

    gSPDisplayList(gDisplayListHead++, gUiEnvTintTextureDL);

    gDPPipeSync(gDisplayListHead++);
    gDPSetPrimColor(gDisplayListHead++, 0, 0, 0, 0, 0, 0);
    gDPSetCombineMode(gDisplayListHead++, G_CC_PRIMITIVE, G_CC_PRIMITIVE);
    gDPSetRenderMode(gDisplayListHead++, G_RM_XLU_SURF, G_RM_XLU_SURF2);

    gSPTextureRectangle(gDisplayListHead++, (arg0 - 2) << 2, (arg1 - 2) << 2, (arg0 + arg2 + 8) << 2, (arg1 + 16) << 2,
                        G_TX_RENDERTILE, 0, 0, 0, 0);

    gDPSetCombineMode(gDisplayListHead++, G_CC_DECALRGBA, G_CC_DECALRGBA);
    gDPLoadTextureBlock(gDisplayListHead++, gBorderTexture_Left, G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 12, 0,
                        G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                        G_TX_NOLOD);

    gSPTextureRectangle(gDisplayListHead++, arg0 << 2, arg1 << 2, (arg0 + 2) << 2, (arg1 + 0xC) << 2, G_TX_RENDERTILE,
                        0, 0, 0x0400, 0x0400);

    gDPLoadTextureBlock(gDisplayListHead++, gBorderTexture_Middle, G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 12, 0,
                        G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                        G_TX_NOLOD);

    gSPTextureRectangle(gDisplayListHead++, ((arg0 + spF0) + 2) << 2, arg1 << 2, ((arg0 + spF0) + 4) << 2,
                        (arg1 + 0xC) << 2, G_TX_RENDERTILE, 0x00C0, 0, 0x0400, 0x0400);

    if (sp1C > 0) {
        gDPLoadTextureBlock(gDisplayListHead++, gBorderTexture_Right, G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 12, 0,
                            G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK,
                            G_TX_NOLOD, G_TX_NOLOD);
        gSPTextureRectangle(gDisplayListHead++, (arg0 + 2) << 2, arg1 << 2, ((arg0 + 2) + sp1C) << 2, (arg1 + 0xC) << 2,
                            G_TX_RENDERTILE, 0, 0, 0, 0x0400);
    }

    if (temp_lo > 0) {
        gDPLoadTextureBlock(gDisplayListHead++, gBorderTexture_Fill, G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 12, 0,
                            G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK,
                            G_TX_NOLOD, G_TX_NOLOD);
        gSPTextureRectangle(gDisplayListHead++, ((arg0 + 2) + sp1C) << 2, arg1 << 2,
                            (((arg0 + 2) + sp1C) + temp_lo) << 2, (arg1 + 0xC) << 2, G_TX_RENDERTILE, 0, 0, 0, 0x0400);
    }

    gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);
}

void Widget_DrawMeterBar(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    u32* spCC;
    s32 temp_lo = ((arg3 - arg2) * 0x30) / arg3;
    s32 sp14 = 0x30 - temp_lo;

    if (arg2 < ((arg3 * 0xA) / 48)) {
        spCC = gBorderTexture_Right;
    } else if (arg2 < ((arg3 * 0x1B) / 48)) {
        spCC = gBorderTexture_Right;
    } else {
        spCC = gBorderTexture_Fill;
    }

    gSPDisplayList(gDisplayListHead++, gUiEnvTintTextureDL);

    gDPLoadTextureBlock(gDisplayListHead++, gBorderTexture_Left, G_IM_FMT_RGBA, G_IM_SIZ_16b, 16, 5, 0,
                        G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                        G_TX_NOLOD);
    gSPTextureRectangle(gDisplayListHead++, arg0 << 2, arg1 << 2, (arg0 + 0x10) << 2, (arg1 + 5) << 2, G_TX_RENDERTILE,
                        0, 0, 0x0400, 0x0400);

    gDPLoadTextureBlock(gDisplayListHead++, gBorderTexture_Middle, G_IM_FMT_RGBA, G_IM_SIZ_16b, 4, 5, 0,
                        G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                        G_TX_NOLOD);
    gSPTextureRectangle(gDisplayListHead++, (arg0 + 0x40) << 2, arg1 << 2, (arg0 + 0x41) << 2, (arg1 + 5) << 2,
                        G_TX_RENDERTILE, 0, 0, 0x0400, 0x0400);

    if (sp14 > 0) {
        gDPLoadTextureBlock(gDisplayListHead++, spCC, G_IM_FMT_RGBA, G_IM_SIZ_16b, 4, 5, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                            G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
        gSPTextureRectangle(gDisplayListHead++, (arg0 + 0x10) << 2, arg1 << 2, (arg0 + sp14 + 0x10) << 2,
                            (arg1 + 5) << 2, G_TX_RENDERTILE, 0, 0, 0, 0x0400);
    }

    if (temp_lo > 0) {
        gDPLoadTextureBlock(gDisplayListHead++, D_1003E78, G_IM_FMT_RGBA, G_IM_SIZ_16b, 4, 5, 0,
                            G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK,
                            G_TX_NOLOD, G_TX_NOLOD);
        gSPTextureRectangle(gDisplayListHead++, ((arg0 + sp14) + 0x10) << 2, arg1 << 2,
                            ((arg0 + sp14) + temp_lo + 0x10) << 2, (arg1 + 5) << 2, G_TX_RENDERTILE, 0, 0, 0, 0x0400);
    }

    gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);
}

s16 gCurrentMenuIndex = -1;
u32* gPositiveTextureBank[] = { gPlayerIconTex_0, gPlayerIconTex_1, gPlayerIconTex_2, gPlayerIconTex_3 };
u32* gNegativeTextureBank[] = { gPlayerIconTex_4, gPlayerIconTex_5, gPlayerIconTex_6, gPlayerIconTex_7, gResultCubeTex_8 };

void Minigame_InitSharedState(s16 arg0) {
    s32 i;
    gComHyperUnlocked = 0;
    gCurrentMenuIndex = arg0;
    gMenuCursorX = 0;
    gMenuCursorY = 0;

    for (i = 0; i < 4; i++) {
        gMiniHudColorDefaults[i] = 1;
    }

    if (arg0 == 3) {
        gMinigameComDifficulty = 1;
    } else {
        gMinigameComDifficulty = 3;
    }

    gWinsToChampion = 3;

    for (i = 0; i < 4; i++) {
        gPlayerWinCounts[i] = 0;
    }

    i = 0;
    do {
        gMenuOptionFlags[i] = 0;
        i++;
    } while (i < 4);
    Widget_Init();
}

void Widget_FillRect(s16 arg0, s16 arg1, s16 arg2, s16 arg3) {
    gDPPipeSync(gDisplayListHead++);
    gDPSetRenderMode(gDisplayListHead++,
                     IM_RD | CVG_DST_FULL | ZMODE_OPA | CVG_X_ALPHA | FORCE_BL |
                         GBL_c1(G_BL_CLR_MEM, G_BL_0, G_BL_CLR_MEM, G_BL_1MA),
                     IM_RD | CVG_DST_FULL | ZMODE_OPA | CVG_X_ALPHA | FORCE_BL |
                         GBL_c2(G_BL_CLR_MEM, G_BL_0, G_BL_CLR_MEM, G_BL_1MA));
    gDPSetCombineMode(gDisplayListHead++, G_CC_SHADE, G_CC_SHADE);
    gDPSetCycleType(gDisplayListHead++, G_CYC_1CYCLE);
    gDPFillRectangle(gDisplayListHead++, arg0, arg1, arg0 + arg2, arg1 + arg3);
}

void Widget_MeasureLabelBox(s32* arg0, s32* arg1, char* arg2, char* arg3, s32 arg4) {
    s32 var_a1;
    s32 var_t2;
    s8 sp38[0x100];
    s8* var_a2;
    s32 sp30;
    s8* var_t0;
    s8* var_t1;
    s8* var_v0;

    *arg0 = -1;
    var_t1 = arg2;
    *arg1 = -1;
    var_t0 = sp38;
    var_t2 = 0;

    while (*var_t1 != 0) {
        var_a2 = arg3;
        var_v0 = var_t1;

        if (*var_t1 != *var_a2) {
            *var_t0++ = *var_t1;
        } else {
            var_a1 = 1;

            while (*var_a2 != 0) {
                if (*var_v0 != *var_a2) {
                    var_a1 = 0;
                    break;
                }

                var_v0++;
                var_a2++;
            }

            if (var_a1 != 0) {
                var_t2 = 1;
                break;
            }

            *var_t0++ = *var_t1;
        }
        var_t1++;
    }

    *var_t0 = 0;
    var_t0 = sp38;
    var_a2 = var_t0;
    sp30 = 0;

    while (*var_t0 != 0) {
        if (*var_t0 == 0xA) {
            sp30 += 1;
            var_a2 = var_t0;
        }
        var_t0++;
    }

    if (var_t2 != 0) {
        *arg0 = Font_MeasureTextExtent(0, 0, (char*)var_a2);
        *arg1 = arg4 * sp30;
    }
}

s32 Text_MeasureStringWidth(UNUSED FontContext* arg0, char* arg1) {
    return Font_MeasureTextExtent(0, 0, arg1);
}

void Widget_DrawPlayerIconScaled(s32 arg0, s16 arg1, s16 arg2, f32 arg3, f32 arg4) {
    static u32* gCurrentPlayerIconTex;

    f32 temp_fa0;
    f32 temp_fv1;
    s16 a;
    s16 b;
    u32* var_t3;

    if ((arg3 != 0.0f) && (arg4 != 0.0f)) {
        if (arg0 < 0) {
            arg0 = -1 - arg0;
            gCurrentPlayerIconTex = gNegativeTextureBank[arg0];
        } else {
            gCurrentPlayerIconTex = gPositiveTextureBank[arg0 % 4];
        }

        gDPLoadTextureBlock(gDisplayListHead++, gCurrentPlayerIconTex, G_IM_FMT_RGBA, G_IM_SIZ_32b, 36, 26, 0,
                            G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK,
                            G_TX_NOLOD, G_TX_NOLOD);

        a = ROUND_MAX(36.0f * arg3);
        b = ROUND_MAX(26.0f * arg4);

        Gfx_DrawTexturedRectClipped(arg1, arg2, a, b, 0, 0, 1024.0f / arg3, 1024.0f / arg4, 0);
    }
}

void Widget_DrawPlayerIcon(s32 arg0, s16 arg1, s16 arg2, f32 _scale) {
    Widget_DrawPlayerIconScaled(arg0, arg1, arg2, _scale, _scale);
}

void Widget_DrawAnimIcon16(s16 arg0, s16 arg1, AnimFrame* arg2) {
    static AnimFrame* gAnimSprite1_Current = NULL;
    static AnimFrame* gAnimSprite1_Ptr;
    static s16 gAnimSprite1_Timer;

    if ((gAnimSprite1_Current != arg2) || (arg2 == NULL)) {
        gAnimSprite1_Current = arg2;
        if (arg2 == NULL) {
            return;
        }
        gAnimSprite1_Ptr = arg2;
        gAnimSprite1_Timer = arg2->unk_04;
    }

    gAnimSprite1_Timer--;
    if (gAnimSprite1_Timer <= 0) {
        gAnimSprite1_Ptr++;
        if (gAnimSprite1_Ptr->unk_00 == NULL) {
            gAnimSprite1_Ptr = arg2;
        }
        gAnimSprite1_Timer = gAnimSprite1_Ptr->unk_04;
    }

    Gfx_DrawTextureRgba32(arg0, arg1, 0x10, 0x10, gAnimSprite1_Ptr->unk_00, 0x10, 0);
}

void Widget_DrawAnimIcon16B(s16 arg0, s16 arg1, AnimFrame* arg2) {
    static AnimFrame* gAnimSprite2_Current = NULL;
    static AnimFrame* gAnimSprite2_Ptr;
    static s16 gAnimSprite2_Timer;

    if ((gAnimSprite2_Current != arg2) || (arg2 == NULL)) {
        gAnimSprite2_Current = arg2;
        if (arg2 == NULL) {
            return;
        }
        gAnimSprite2_Ptr = arg2;
        gAnimSprite2_Timer = arg2->unk_04;
    }

    gAnimSprite2_Timer--;
    if (gAnimSprite2_Timer <= 0) {
        gAnimSprite2_Ptr++;
        if (gAnimSprite2_Ptr->unk_00 == NULL) {
            gAnimSprite2_Ptr = arg2;
        }
        gAnimSprite2_Timer = gAnimSprite2_Ptr->unk_04;
    }

    Gfx_DrawTextureRgba32(arg0, arg1, 0x10, 0x10, gAnimSprite2_Ptr->unk_00, 0x10, 0);
}

void Widget_DrawAnimIcon16C(s16 arg0, s16 arg1, AnimFrame* arg2) {
    static AnimFrame* gAnimSprite3_Current = NULL;
    static AnimFrame* gAnimSprite3_Ptr;
    static s16 gAnimSprite3_Timer;

    if ((gAnimSprite3_Current != arg2) || (arg2 == NULL)) {
        gAnimSprite3_Current = arg2;
        if (arg2 == NULL) {
            return;
        }
        gAnimSprite3_Ptr = arg2;
        gAnimSprite3_Timer = arg2->unk_04;
    }

    gAnimSprite3_Timer--;
    if (gAnimSprite3_Timer <= 0) {
        gAnimSprite3_Ptr++;
        if (gAnimSprite3_Ptr->unk_00 == NULL) {
            gAnimSprite3_Ptr = arg2;
        }
        gAnimSprite3_Timer = gAnimSprite3_Ptr->unk_04;
    }

    Gfx_DrawTextureRgba32(arg0, arg1, 0x10, 0x10, gAnimSprite3_Ptr->unk_00, 0x10, 0);
}

void Widget_DrawAnimIcon40x20(s16 arg0, s16 arg1, AnimFrame* arg2) {
    static AnimFrame* gAnimTex1_Current = NULL;
    static AnimFrame* gAnimTex1_Ptr;
    static s16 gAnimTex1_Timer;

    if ((gAnimTex1_Current != arg2) || (arg2 == NULL)) {
        gAnimTex1_Current = arg2;
        if (arg2 == NULL) {
            return;
        }
        gAnimTex1_Ptr = arg2;
        gAnimTex1_Timer = arg2->unk_04;
    }

    gAnimTex1_Timer--;
    if (gAnimTex1_Timer <= 0) {
        gAnimTex1_Ptr++;
        if (gAnimTex1_Ptr->unk_00 == NULL) {
            gAnimTex1_Ptr = arg2;
        }
        gAnimTex1_Timer = gAnimTex1_Ptr->unk_04;
    }

    gDPLoadTextureTile(gDisplayListHead++, gAnimTex1_Ptr->unk_00, G_IM_FMT_RGBA, G_IM_SIZ_32b, 40, 0, 0, 0, 39, 19, 0,
                       G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                       G_TX_NOLOD);

    Gfx_DrawTexturedRectClipped(arg0, arg1, 0x28, 0x14, 0, 0, 0x400, 0x400, 0);
}

void Widget_DrawAnimIcon40x20B(s16 arg0, s16 arg1, AnimFrame* arg2) {
    static AnimFrame* gAnimTex2_Current = NULL;
    static AnimFrame* gAnimTex2_Ptr;
    static s16 gAnimTex2_Timer;

    if ((gAnimTex2_Current != arg2) || (arg2 == NULL)) {
        gAnimTex2_Current = arg2;
        if (arg2 == NULL) {
            return;
        }
        gAnimTex2_Ptr = arg2;
        gAnimTex2_Timer = arg2->unk_04;
    }

    gAnimTex2_Timer--;
    if (gAnimTex2_Timer <= 0) {
        gAnimTex2_Ptr++;
        if (gAnimTex2_Ptr->unk_00 == NULL) {
            gAnimTex2_Ptr = arg2;
        }
        gAnimTex2_Timer = gAnimTex2_Ptr->unk_04;
    }

    gDPLoadTextureTile(gDisplayListHead++, gAnimTex2_Ptr->unk_00, G_IM_FMT_RGBA, G_IM_SIZ_32b, 40, 0, 0, 0, 39, 19, 0,
                       G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                       G_TX_NOLOD);

    Gfx_DrawTexturedRectClipped(arg0, arg1, 0x28, 0x14, 0, 0, 0x400, 0x400, 0);
}

void Widget_DrawAnimIcon24(s16 arg0, s16 arg1, AnimFrame* arg2) {
    static AnimFrame* gAnimSpriteLarge_Current = NULL;
    static AnimFrame* gAnimSpriteLarge_Ptr;
    static s16 gAnimSpriteLarge_Timer;

    if ((gAnimSpriteLarge_Current != arg2) || (arg2 == NULL)) {
        gAnimSpriteLarge_Current = arg2;
        if (arg2 == NULL) {
            return;
        }
        gAnimSpriteLarge_Ptr = arg2;
        gAnimSpriteLarge_Timer = arg2->unk_04;
    }

    gAnimSpriteLarge_Timer--;
    if (gAnimSpriteLarge_Timer <= 0) {
        gAnimSpriteLarge_Ptr++;
        if (gAnimSpriteLarge_Ptr->unk_00 == NULL) {
            gAnimSpriteLarge_Ptr = arg2;
        }
        gAnimSpriteLarge_Timer = gAnimSpriteLarge_Ptr->unk_04;
    }

    Gfx_DrawTextureRgba32(arg0, arg1, 0x18, 0x13, gAnimSpriteLarge_Ptr->unk_00, 0x18, 0);
}

void Widget_DrawAnimIcon28(s16 arg0, s16 arg1, s16 arg2, AnimFrame* arg3) {
    static AnimFrame* gAnimTexDual_Current[] = {
        NULL,
        NULL,
    };
    static AnimFrame* gAnimTexDual_Ptr[2];
    static s16 gAnimTexDual_Timer[2];

    if ((arg3 != gAnimTexDual_Current[arg0]) || (arg3 == NULL)) {
        gAnimTexDual_Current[arg0] = arg3;
        if (arg3 == NULL) {
            return;
        }
        gAnimTexDual_Ptr[arg0] = arg3;
        gAnimTexDual_Timer[arg0] = arg3->unk_04;
    }

    gAnimTexDual_Timer[arg0]--;
    if (gAnimTexDual_Timer[arg0] <= 0) {
        gAnimTexDual_Ptr[arg0]++;
        if (gAnimTexDual_Ptr[arg0]->unk_00 == NULL) {
            gAnimTexDual_Ptr[arg0] = arg3;
        }
        gAnimTexDual_Timer[arg0] = gAnimTexDual_Ptr[arg0]->unk_04;
    }

    gDPLoadTextureTile(gDisplayListHead++, gAnimTexDual_Ptr[arg0]->unk_00, G_IM_FMT_RGBA, G_IM_SIZ_32b, 28, 0, 0, 0, 27, 27,
                       0, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                       G_TX_NOLOD);

    Gfx_DrawTexturedRectClipped(arg1, arg2, 0x1C, 0x1C, 0, 0, 0x400, 0x400, 0);
}

void Widget_DrawSplitBanner(s16 arg0, s16 arg1, s16 arg2) {
    static u32 gDualTexturePanelIds[] = { 0x03049CA0, 0x0304B1C0, 0x0304C6E0, 0x0304DC00 };

    u32 sp34 = gDualTexturePanelIds[arg0];

    gDPLoadTextureTile(gDisplayListHead++, sp34, G_IM_FMT_RGBA, G_IM_SIZ_16b, 52, 0, 0, 0, 51, 25, 0,
                       G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                       G_TX_NOLOD);

    Gfx_DrawTexturedRectClipped(arg1, arg2, 0x34, 0x1A, 0, 0, 0x400, 0x400, 0);

    gDPLoadTextureTile(gDisplayListHead++, sp34 + 0xA90, G_IM_FMT_RGBA, G_IM_SIZ_16b, 52, 0, 0, 0, 51, 25, 0,
                       G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                       G_TX_NOLOD);

    Gfx_DrawTexturedRectClipped(arg1, arg2 + 0x1A, 0x34, 0x1A, 0, 0, 0x400, 0x400, 0);
}

void Widget_DrawStripedBar(s16 arg0, s16 arg1) {
    u32* ptr = D_304F120;
    s32 i;

    for (i = 0; i < 7; i++, ptr += 800) {
        gDPLoadTextureTile(gDisplayListHead++, ptr, G_IM_FMT_RGBA, G_IM_SIZ_32b, 100, 0, 0, 0, 99, 7, 0,
                           G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                           G_TX_NOLOD);

        Gfx_DrawTexturedRectClipped(arg0, arg1 + (i * 8), 0x64, 8, 0, 0, 0x400, 0x400, 0);
    }
}
