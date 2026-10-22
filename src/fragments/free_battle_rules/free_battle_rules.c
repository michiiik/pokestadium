#include "free_battle_rules.h"
#include "src/graphics_textures.h"
#include "src/input.h"
#include "src/ui_graphics.h"
#include "src/pokemon_stats.h"
#include "src/game_state.h"
#include "src/session.h"
#include "src/text_system.h"
#include "src/jpeg_stream.h"
#include "src/audio_sfx.h"
#include "src/DDC0.h"
#include "src/matrix.h"
#include "src/controller.h"
#include "src/memory.h"
#include "src/stage_loader.h"

typedef struct MenuSelection {
    /* 0x00 */ s16 unk_00;
    /* 0x02 */ s16 unk_02;
} MenuSelection; // size = 0x4

typedef struct RuleOptionWindow {
    /* 0x00 */ s16 unk_00;
    /* 0x02 */ s16 unk_02;
    /* 0x04 */ s16 unk_04;
    /* 0x06 */ s16 unk_06;
    /* 0x08 */ s16 unk_08;
} RuleOptionWindow; // size >= 0xA

typedef struct MenuEntry {
    /* 0x00 */ s16 unk_00;
    /* 0x02 */ s16 unk_02;
    /* 0x04 */ s16 unk_04;
    /* 0x06 */ s16 unk_06;
    /* 0x08 */ u8* unk_08;
    /* 0x0C */ u8* unk_0C;
} MenuEntry; // size = 0x10

static char** gRuleMessageBank;
static char** gOptionMessageBank;
#if defined(VERSION_US_REV2)
static s32 pad_D_84002D08_us_rev2;
#endif
static BinArchive* gRuleBackgroundArchive;
static u8* gMenuBackgroundAtlas;
static s32 pad_D_84002D10;
static s16 gControllerInputMask;
static s16 gRuleMenuState;
#if defined(VERSION_US_REV2)
static s32 pad_D_84002D18_us_rev2;
#endif
static MenuSelection gMenuSelection;
static s32 pad_D_84002D1C;
static s16 gSelectedRuleOption;
static RuleOptionWindow gRuleOptionWindow;

static s16 gWindowAnimTimer = 2;
static s16 gSelectedRuleValue = 0;
static MenuEntry gMenuEntries[11] = {
    {
        1,
        0,
        48,
        6,
        0x03008FD0,
        NULL,
    },
    {
        3,
        0,
        48,
        36,
        0x03008FD0,
        0x03009720,
    },
    {
        5,
        0,
        48,
        66,
        0x03008FD0,
        0x03009E70,
    },
    {
        9,
        0,
        48,
        96,
        0x03008FD0,
        0x0300A5C0,
    },
    {
        2,
        15,
        280,
        6,
        0x03009720,
        NULL,
    },
    {
        4,
        15,
        280,
        36,
        0x03009E70,
        NULL,
    },
    {
        8,
        15,
        280,
        66,
        0x0300A5C0,
        NULL,
    },
    {
        0,
        15,
        280,
        96,
        0x03008880,
        NULL,
    },
    {
        6,
        15,
        336,
        6,
        0x03009720,
        0x03009E70,
    },
    {
        10,
        15,
        336,
        36,
        0x03009720,
        0x0300A5C0,
    },
    {
        12,
        15,
        336,
        66,
        0x03009E70,
        0x0300A5C0,
    },
};
static s16 gRuleBattleIds[] = { 0, 3, 4, 5, 6, 0 };

void FreeBattle_DrawHeaderImage(s16 arg0, s16 arg1) {
    gSPDisplayList(gDisplayListHead++, gUiEnvTintTextureDL);

    Gfx_DrawTextureRgba16(arg0, arg1, 0x30, 0x18, D_2006C00, 0x30, 0);
    Gfx_DrawTextureRgba16(arg0, arg1 + 0x18, 0x30, 0x18, D_2007500, 0x30, 0);

    gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);
}

void FreeBattle_DrawBannerImage(s16 arg0, s16 arg1) {
    gSPDisplayList(gDisplayListHead++, gUiEnvTintTextureDL);

    Gfx_DrawTextureRgba16(arg0, arg1, 0x64, 0x14, D_200B3E0, 0x64, 0);
    Gfx_DrawTextureRgba16(arg0, arg1 + 0x14, 0x64, 0x14, D_200C380, 0x64, 0);
    Gfx_DrawTextureRgba16(arg0, arg1 + 0x28, 0x64, 0x10, D_200D320, 0x64, 0);

    gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);
}

void FreeBattle_DrawTintedBar(s16 arg0, s16 arg1, s16 arg2, Color_RGBA8* arg3, Color_RGBA8* arg4) {
    gSPDisplayList(gDisplayListHead++, gDL_XluBorder);
    gDPSetEnvColor(gDisplayListHead++, arg3->r, arg3->g, arg3->b, 255);
    gDPSetPrimColor(gDisplayListHead++, 0, 0, arg4->r, arg4->g, arg4->b, 255);

    Gfx_DrawTextureIa8(arg0, arg1, 0x10, 0x20, D_20003C0, 0x10, 0);
    Gfx_DrawTextureIa8((arg0 + arg2) - 0x10, arg1, 0x10, 0x20, D_20005C0, 0x10, 0);
    Gfx_DrawTexturedRectClipped(arg0 + 0x10, arg1, arg2 - 0x20, 0x20, 0, 0, 0, 0x400, 0);

    gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);
}

void FreeBattle_DrawTitleBar(void) {
    Color_RGBA8 sp2C;
    Color_RGBA8 sp28;

    Color_SetRGB(&sp2C, 0x64, 0x64, 0xC8);
    Color_SetRGB(&sp28, 0x28, 0x28, 0x8C);
    FreeBattle_DrawTintedBar(0x48, 0x28, 0x208, &sp2C, &sp28);
    FreeBattle_DrawHeaderImage(0x30, 0x20);
    Font_BeginTranslucentTextRendering();
    Font_SetActive(0x10, 0);
    Gfx_SetEnvColor(0, 0, 0, 0xFF);
    Font_Printf(0x6A, 0x2C, Text_GetString(NULL, 0, gRuleMessageBank, 0));
    Gfx_SetEnvColor(0xFF, 0xFF, 0x77, 0xFF);
    Font_Printf(0x68, 0x2A, Text_GetString(NULL, 0, gRuleMessageBank, 0));
    Font_EndTexturedTextRendering();
}

void FreeBattle_DrawSelectionCorners(s16 arg0, s16 arg1, s16 arg2, s16 arg3) {
    static s16 gSelectionCornerAnimTimer = 0;

    s16 sp56 = SINS(gSelectionCornerAnimTimer) * 2;
    UNUSED s32 pad[2];

    gSPDisplayList(gDisplayListHead++, gUiEnvTintTextureDL);
    gDPSetEnvColor(gDisplayListHead++, 240, 212, 104, 255);

    Gfx_DrawTextureIa8(arg0 + sp56, arg1 + sp56, 0x10, 0x10, D_2000C80, 0x10, 0);
    Gfx_DrawTextureIa8(arg0 + sp56, ((arg1 + arg3) - sp56) - 0x10, 0x10, 0x10, D_2000F80, 0x10, 0);
    Gfx_DrawTextureIa8(((arg0 + arg2) - sp56) - 0x10, arg1 + sp56, 0x10, 0x10, D_2000D80, 0x10, 0);
    Gfx_DrawTextureIa8(((arg0 + arg2) - sp56) - 0x10, ((arg1 + arg3) - sp56) - 0x10, 0x10, 0x10, D_2000E80, 0x10, 0);

    gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);

    gSelectionCornerAnimTimer += 0x2000;
}

void FreeBattle_DrawPanelFrame(s16 arg0, s16 arg1, s16 arg2, s16 arg3) {
    gSPDisplayList(gDisplayListHead++, gUiEnvTintTextureDL);

    Gfx_DrawTextureIa8(arg0 - 3, arg1 - 3, 8, 8, D_2000340, 8, 0);
    Gfx_DrawTextureIa8((arg0 + arg2) - 5, arg1 - 3, 8, 8, D_2000380, 8, 0);
    Gfx_DrawTexturedRectClipped(arg0 + 4, arg1 - 3, arg2 - 8, 8, 0, 0, 0, 0x400, 0);
    Gfx_DrawTextureIa8(arg0 - 3, (arg1 + arg3) - 5, 8, 8, D_20002C0, 8, 0);
    Gfx_DrawTexturedRectClipped(arg0 - 3, arg1 + 4, 8, arg3 - 8, 0, 0, 0x400, 0, 0);
    Gfx_DrawTextureIa8((arg0 + arg2) - 5, (arg1 + arg3) - 5, 8, 8, D_2000300, 8, 0);
    Gfx_DrawTexturedRectClipped(arg0 + 4, (arg1 + arg3) - 5, arg2 - 8, 8, 0, 0, 0, 0x400, 0);
    Gfx_DrawTexturedRectClipped((arg0 + arg2) - 5, arg1 + 4, 8, arg3 - 8, 0, 0, 0x400, 0, 0);

    gSPDisplayList(gDisplayListHead++, gMenuDL_BackgroundFill);

    Gfx_FillRectRgb(arg0 + 4, arg1 + 4, arg2 - 8, arg3 - 8, 0x3C, 0x3C, 0xA0);

    gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);
}

void FreeBattle_DrawSlicedFrame(s16 arg0, s16 arg1, s16 arg2, s16 arg3, u8 arg4, u8 arg5, u8 arg6, u8 arg7) {
    s16 temp_a2;
    s16 temp_s0_32;
    s16 var_s2;
    s16 var_s3;

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

s32 FreeBattle_IsRuleAvailable(s16 arg0) {
    MenuEntry* ptr = &gMenuEntries[arg0];
    s16 t = ptr->unk_00;
    s32 var_v1 = 0;

    if (arg0 < 4) {
        if (gRuleMenuState >= 2) {
            var_v1 = arg0 == gMenuSelection.unk_00;
        } else {
            var_v1 = ptr->unk_00 == (ptr->unk_00 & gControllerInputMask);
        }
    } else if (arg0 < 11) {
        if (gRuleMenuState >= 3) {
            var_v1 = arg0 == (gMenuSelection.unk_02 + 4);
        } else {
            s16 tmp = (~gMenuEntries[gMenuSelection.unk_00].unk_00 & gControllerInputMask);

            var_v1 = ptr->unk_00 == (ptr->unk_00 & tmp);
        }
    }
    return var_v1;
}

void FreeBattle_DrawRuleMark(s16 arg0, s16 arg1, s16 arg2) {
    if (FreeBattle_IsRuleAvailable(arg2) != 0) {
        Gfx_SetEnvColor(0xFF, 0xFF, 0xFF, 0xFF);
    } else {
        Gfx_SetEnvColor(0x64, 0x64, 0x64, 0x82);
    }
    Font_DrawCharAt(arg0, arg1, 0x26);
}

void FreeBattle_DrawRuleGrid(void) {
    s32 i;
    s16 temp_s2;
    s16 temp_s3;
    s32 var_t1;
    MenuEntry* ptr;
    s16 tmp1;
    s16 tmp2;

    FreeBattle_DrawSlicedFrame(0x64, 0x72, 0x1B0, 0x80, 0, 0, 0, 0xFF);
    FreeBattle_DrawBannerImage(0x10A, 0x96);

    gSPDisplayList(gDisplayListHead++, gUiEnvTintTextureDL);

    for (i = 0; i < 11; i++) {
        temp_s2 = gMenuEntries[i].unk_04 + 0x64;
        temp_s3 = gMenuEntries[i].unk_06 + 0x72;

        if (FreeBattle_IsRuleAvailable(i) != 0) {
            var_t1 = 0xFF;
        } else {
            var_t1 = 0x82;
        }

        gDPSetEnvColor(gDisplayListHead++, var_t1, var_t1, var_t1, var_t1);

        Gfx_DrawTextureRgba16(temp_s2, temp_s3, 0x24, 0x1A, gMenuEntries[i].unk_08, 0x24, 0);
        if (gMenuEntries[i].unk_0C != NULL) {
            Gfx_DrawTextureRgba16(temp_s2 + 0x30, temp_s3, 0x24, 0x1A, gMenuEntries[i].unk_0C, 0x24, 0);
        }
    }

    gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);

    Font_BeginTranslucentTextRendering();
    Font_SetActive(0x10, 0);

    for (i = 0; i < 11; i++) {
        if (gMenuEntries[i].unk_0C != NULL) {
            FreeBattle_DrawRuleMark(gMenuEntries[i].unk_04 + 0x87, gMenuEntries[i].unk_06 + 0x73, i);
        }
    }

    Font_SetActive(0x10, 0);

    if ((gRuleMenuState == 1) || (gRuleMenuState == 2)) {
        Gfx_SetEnvColor(0xFF, 0xFF, 0xFF, 0xFF);
    } else {
        Gfx_SetEnvColor(0x78, 0x78, 0xC8, 0xFF);
    }

    Font_Printf(0x64, 0x5A, Text_GetString(NULL, 0, gRuleMessageBank, 1));
    Font_EndTexturedTextRendering();

    if (gRuleMenuState == 1) {
        ptr = &gMenuEntries[gMenuSelection.unk_00];
        tmp1 = ptr->unk_04 + 0x64;
        tmp2 = ptr->unk_06 + 0x72;
        FreeBattle_DrawSelectionCorners(tmp1 - 4, tmp2 - 2, ((ptr->unk_0C == NULL) ? 0x24 : 0x54) + 8, 0x1E);
    } else if (gRuleMenuState == 2) {
        ptr = &gMenuEntries[gMenuSelection.unk_02];
        tmp1 = ptr[4].unk_04 + 0x64;
        tmp2 = ptr[4].unk_06 + 0x72;
        FreeBattle_DrawSelectionCorners(tmp1 - 4, tmp2 - 2, ((ptr[4].unk_0C == NULL) ? 0x24 : 0x54) + 8, 0x1E);
    }
}

void FreeBattle_UpdateRuleRowSelect(void) {
    s32 sp24 = gMenuSelection.unk_00;

    if (BTN_IS_PRESSED(gPlayer1Controller, BTN_A)) {
        Audio_PlaySoundEffectById(SFX_MENU_CONFIRM);
        gMenuSelection.unk_02 = 0;
        while (FreeBattle_IsRuleAvailable(gMenuSelection.unk_02 + 4) == 0) {
            gMenuSelection.unk_02++;
        }
        gRuleMenuState = 2;
    } else if (BTN_IS_PRESSED(gPlayer1Controller, BTN_B)) {
        Audio_PlaySoundEffectById(SFX_MENU_BACK);
        gRuleMenuState = 8;
    } else {
        do {
            if (BTN_IS_PRESSED(gPlayer1Controller, BTN_DUP)) {
                gMenuSelection.unk_00 = (gMenuSelection.unk_00 + 3) % 4;
            } else if (BTN_IS_PRESSED(gPlayer1Controller, BTN_DDOWN)) {
                gMenuSelection.unk_00 = (gMenuSelection.unk_00 + 1) % 4;
            }
        } while (FreeBattle_IsRuleAvailable(gMenuSelection.unk_00) == 0);

        if (sp24 != gMenuSelection.unk_00) {
            Audio_PlaySoundEffectById(SFX_MENU_SCROLL);
        }
    }
}

void FreeBattle_UpdateRuleOptionSelect(void) {
    s32 sp34 = gMenuSelection.unk_02;
    s32 i;
    s32 var_a0;
    s32 var_s1;
    s32 var_s4;

    if (BTN_IS_PRESSED(gPlayer1Controller, BTN_A)) {
        Audio_PlaySoundEffectById(SFX_MENU_CONFIRM);
        if (gMenuSelection.unk_02 == 3) {
            gRuleMenuState = 5;
        } else {
            gSelectedRuleOption = 0;
            gRuleMenuState = 3;
        }
        return;
    }

    var_s4 = 0;
    if (BTN_IS_PRESSED(gPlayer1Controller, BTN_B)) {
        Audio_PlaySoundEffectById(SFX_MENU_BACK);
        gMenuSelection.unk_02 = -1;
        gRuleMenuState = 1;
        return;
    }

    var_a0 = gMenuSelection.unk_02 / 4;
    var_s1 = gMenuSelection.unk_02 % 4;

    if (BTN_IS_PRESSED(gPlayer1Controller, BTN_DLEFT) && (var_a0 == 1)) {
        var_a0 = 0;
        var_s4 = 1;
    } else if (BTN_IS_PRESSED(gPlayer1Controller, BTN_DRIGHT) && (var_a0 == 0)) {
        var_a0 = 1;
        var_s4 = 1;
    } else if (BTN_IS_PRESSED(gPlayer1Controller, BTN_DUP)) {
        var_s1 = (var_s1 + 3) % 4;
        var_s4 = -1;
    } else if (BTN_IS_PRESSED(gPlayer1Controller, BTN_DDOWN)) {
        var_s1 = (var_s1 + 1) % 4;
        var_s4 = 1;
    }

    if (var_s4 != 0) {
        for (i = 0; i < 4; i++) {
            if (FreeBattle_IsRuleAvailable(var_a0 * 4 + var_s1 + 4) != 0) {
                break;
            }
            var_s1 = (var_s1 + var_s4 + 4) % 4;
        }

        if (i != 4) {
            gMenuSelection.unk_02 = var_a0 * 4 + var_s1;
        }
    }

    if (sp34 != gMenuSelection.unk_02) {
        Audio_PlaySoundEffectById(SFX_MENU_SCROLL);
    }
}

void FreeBattle_ClearRuleSelection(void) {
    gMenuSelection.unk_00 = -1;
    gMenuSelection.unk_02 = -1;
}

s32 FreeBattle_SelectionHasSubrule(void) {
    s32 var_v1 = 0;

    if ((gMenuSelection.unk_00 != -1) && (gMenuEntries[gMenuSelection.unk_00].unk_0C != 0)) {
        var_v1 = 1;
    }

    if ((gMenuSelection.unk_02 != -1) && (gMenuEntries[gMenuSelection.unk_02 + 4].unk_0C != 0)) {
        var_v1 = 1;
    }
    return var_v1;
}

void FreeBattle_DrawLevelOptionLine(s16 arg0, s16 arg1, s16 arg2, char* arg3) {
    if ((arg2 != 0) && (FreeBattle_SelectionHasSubrule() != 0)) {
        Gfx_SetEnvColor(0x3C, 0x3C, 0x3C, 0xFF);
    } else if (arg2 == gSelectedRuleOption) {
        Gfx_SetEnvColor(0xFF, 0xFF, 0, 0xFF);
    } else {
        Gfx_SetEnvColor(0xFF, 0xFF, 0xFF, 0xFF);
    }
    Font_Printf(arg0, arg1 + (arg2 * 0x18), arg3);
}

void FreeBattle_DummyFunc(void) {
}

void FreeBattle_DrawLevelSelectPanel(void) {
    UNUSED s32 pad;
    char sp2C[128];

    FreeBattle_DrawSlicedFrame(0x64, 0x10C, 0x1B0, 0x84, 0, 0, 0, 0xFF);
    Font_BeginTranslucentTextRendering();
    Font_SetActive(8, 0);
    FreeBattle_DrawLevelOptionLine(0x94, 0x114, 0, Text_GetString(NULL, 0, gRuleMessageBank, 2));
    FreeBattle_DrawLevelOptionLine(0x94, 0x114, 1, Text_GetString(NULL, 0, gRuleMessageBank, 3));
    FreeBattle_DrawLevelOptionLine(0x94, 0x114, 2, Text_GetString(NULL, 0, gRuleMessageBank, 6));
    FreeBattle_DrawLevelOptionLine(0x94, 0x114, 3, Text_GetString(NULL, 0, gRuleMessageBank, 7));
    FreeBattle_DrawLevelOptionLine(0x94, 0x114, 4, Text_GetString(NULL, 0, gRuleMessageBank, 8));
    Font_SetActive(4, 0);

    sprintf(sp2C, "%s1~100", Text_GetString(NULL, 0, gOptionMessageBank, 0x15));
    FreeBattle_DrawLevelOptionLine(0x1AE, 0x118, 0, sp2C);
    sprintf(sp2C, "%s50~55", Text_GetString(NULL, 0, gOptionMessageBank, 0x15));
    FreeBattle_DrawLevelOptionLine(0x1AE, 0x118, 1, sp2C);
    sprintf(sp2C, "%s25~30", Text_GetString(NULL, 0, gOptionMessageBank, 0x15));
    FreeBattle_DrawLevelOptionLine(0x1AE, 0x118, 2, sp2C);
    sprintf(sp2C, "%s15~20", Text_GetString(NULL, 0, gOptionMessageBank, 0x15));
    FreeBattle_DrawLevelOptionLine(0x1AE, 0x118, 3, sp2C);
    sprintf(sp2C, "%s1~100", Text_GetString(NULL, 0, gOptionMessageBank, 0x15));
    FreeBattle_DrawLevelOptionLine(0x1AE, 0x118, 4, sp2C);

    Font_SetActive(0x10, 0);

    if (gRuleMenuState == 3) {
        Gfx_SetEnvColor(0xFF, 0xFF, 0xFF, 0xFF);
    } else {
        Gfx_SetEnvColor(0x78, 0x78, 0xC8, 0xFF);
    }

    Font_Printf(0x64, 0xF4, Text_GetString(NULL, 0, gRuleMessageBank, 9));
    Font_EndTexturedTextRendering();

    if (gRuleMenuState == 3) {
        Ui_DrawAnimatedTextureMarker(0x6C, (gSelectedRuleOption * 0x18) + 0x114);
    }
}

void FreeBattle_UpdateLevelSelect(void) {
    if (BTN_IS_PRESSED(gPlayer1Controller, BTN_A)) {
        Audio_PlaySoundEffectById(0x26);
        gRuleMenuState = 7;
    } else if (BTN_IS_PRESSED(gPlayer1Controller, BTN_B)) {
        Audio_PlaySoundEffectById(SFX_MENU_BACK);
        gSelectedRuleOption = -1;
        gRuleMenuState = 2;
    } else if (FreeBattle_SelectionHasSubrule() == 0) {
        if (BTN_IS_PRESSED(gPlayer1Controller, BTN_DUP)) {
            Audio_PlaySoundEffectById(SFX_MENU_SCROLL);
            gSelectedRuleOption = (gSelectedRuleOption + 4) % 5;
        } else if (BTN_IS_PRESSED(gPlayer1Controller, BTN_DDOWN)) {
            Audio_PlaySoundEffectById(SFX_MENU_SCROLL);
            gSelectedRuleOption = (gSelectedRuleOption + 1) % 5;
        }
    }
}

void FreeBattle_ClearLevelSelection(void) {
    gSelectedRuleOption = -1;
}

void FreeBattle_DrawConfirmLine(s16 arg0, char* arg1) {
    if (arg0 == gRuleOptionWindow.unk_08) {
        Gfx_SetEnvColor(0xFF, 0xFF, 0, 0xFF);
    } else {
        Gfx_SetEnvColor(0xFF, 0xFF, 0xFF, 0xFF);
    }
    Font_Printf(gRuleOptionWindow.unk_00 + 0x34, gRuleOptionWindow.unk_02 + (arg0 * 0x1C) + 0x2B, arg1);
}

#if defined(VERSION_US_REV2)
#pragma GLOBAL_ASM("asm/us/nonmatchings/fragments/free_battle_rules/free_battle_rules/FreeBattle_DrawConfirmPanel.s")
#else
void FreeBattle_DrawConfirmPanel(void) {
    if (gRuleOptionWindow.unk_04 > 0) {
        s16 tmp = gRuleOptionWindow.unk_04 * 0x11;

        Ui_DrawBorderedPanel(gRuleOptionWindow.unk_00, gRuleOptionWindow.unk_02 + (s16)((0x88 - tmp) / 2), 0x98, tmp, 0x2121);
        if (gRuleOptionWindow.unk_04 == 8) {
            gSPDisplayList(gDisplayListHead++, gUiEnvTintTextureDL);

            Gfx_DrawTextureRgba16(gRuleOptionWindow.unk_00 + 0xC, gRuleOptionWindow.unk_02 + 0xA, 0x24, 0x1A, D_3008880, 0x24, 0);

            gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);

            Ui_DrawAnimatedTextureMarker(gRuleOptionWindow.unk_00 + 0xC, gRuleOptionWindow.unk_02 + (gRuleOptionWindow.unk_08 * 0x1C) + 0x2B);
            Font_BeginTranslucentTextRendering();
            Font_SetActive(0x10, 0);
            Font_Printf(gRuleOptionWindow.unk_00 + 0x30, gRuleOptionWindow.unk_02 + 0xB, Text_GetString(NULL, 0, gRuleMessageBank, 0xA));
            FreeBattle_DrawConfirmLine(0, Text_GetString(NULL, 0, gRuleMessageBank, 0xB));
            FreeBattle_DrawConfirmLine(1, Text_GetString(NULL, 0, gRuleMessageBank, 0xC));
            FreeBattle_DrawConfirmLine(2, Text_GetString(NULL, 0, gRuleMessageBank, 0xD));
            Font_EndTexturedTextRendering();
        }
    }
}
#endif

void FreeBattle_InitConfirmPanel(void) {
    gRuleOptionWindow.unk_00 = 0x190;
    gRuleOptionWindow.unk_02 = 0x64;
    gRuleOptionWindow.unk_04 = 0;
    gRuleOptionWindow.unk_08 = 1;
}

void FreeBattle_OpenConfirmPanel(void) {
    gWindowAnimTimer = 2;
    gRuleOptionWindow.unk_04++;
    if (gRuleOptionWindow.unk_04 == 8) {
        gRuleMenuState = 4;
    }
}

void FreeBattle_CloseConfirmPanel(void) {
    gWindowAnimTimer = 2;
    gRuleOptionWindow.unk_04--;
    if (gRuleOptionWindow.unk_04 == 0) {
        gRuleMenuState = gRuleOptionWindow.unk_06;
    }
}

void FreeBattle_UpdateConfirmDialog(void) {
    if (BTN_IS_PRESSED(gPlayer1Controller, BTN_A)) {
        gSelectedRuleValue = gRuleOptionWindow.unk_08;
        gRuleOptionWindow.unk_06 = 3;
        gSelectedRuleOption = 0;
        gRuleMenuState = 6;
        Audio_PlaySoundEffectById(SFX_MENU_CONFIRM);
    } else if (BTN_IS_PRESSED(gPlayer1Controller, BTN_B)) {
        gRuleOptionWindow.unk_06 = 2;
        gRuleMenuState = 6;
        Audio_PlaySoundEffectById(SFX_MENU_BACK);
    } else if (BTN_IS_PRESSED(gPlayer1Controller, BTN_DUP)) {
        Audio_PlaySoundEffectById(SFX_MENU_SCROLL);
        gRuleOptionWindow.unk_08 = (gRuleOptionWindow.unk_08 + 2) % 3;
    } else if (BTN_IS_PRESSED(gPlayer1Controller, BTN_DDOWN)) {
        Audio_PlaySoundEffectById(SFX_MENU_SCROLL);
        gRuleOptionWindow.unk_08 = (gRuleOptionWindow.unk_08 + 1) % 3;
    }
}

void FreeBattle_PollInputs(void) {
    Cont_StartReadInputs();
    Cont_ReadInputs();
    Input_ResetRepeatState();
}

void FreeBattle_DrawFrame(void) {
    BgStage_DrawFrame();
    if (gWindowAnimTimer > 0) {
        FreeBattle_DrawPanelFrame(0x4C, 0x54, 0x1E8, 0x150);
        gWindowAnimTimer -= 1;
    }
    FreeBattle_DrawRuleGrid();
    FreeBattle_DrawLevelSelectPanel();
    FreeBattle_DrawConfirmPanel();
    BgStage_AdvanceFrame();
}

void FreeBattle_FadeIn(void) {
    s32 i;

    if (StageContext_GetFadeMode() == 1) {
        StageFade_StartFromOpaque(8);

        for (i = 0; i < 10; i++) {
            Controller_PollInputs();
            BgStage_DrawFrame();
            Gfx_DrawTiledRgba16Image(gMenuBackgroundAtlas);
            BgStage_AdvanceFrame();
        }
    }
}

void FreeBattle_FadeOut(void) {
    Audio_StopMusic(0x10);
    StageContext_SetClearColor(0xFFFF);
    StageFade_StartFromTransparent(8);

    while (StageContext_GetFadeMode() != 1) {
        Controller_PollInputs();
        BgStage_DrawFrame();
        Gfx_DrawTiledRgba16Image(gMenuBackgroundAtlas);
        BgStage_AdvanceFrame();
    }

    StageLoader_RunFrames(2);
}

void FreeBattle_SlideInPanel(void) {
    s32 i;
    s16 tmp;

    Audio_PlaySoundEffectById(SFX_PANEL_OPEN);

    for (i = 0x10; i < 0x170; i += 0x20) {
        FreeBattle_PollInputs();
        BgStage_DrawFrame();
        Gfx_DrawTiledRgba16Image(gMenuBackgroundAtlas);
        FreeBattle_DrawTitleBar();
        tmp = i;
        tmp = (0x150 - tmp) / 2;
        FreeBattle_DrawPanelFrame(0x4C, tmp + 0x54, 0x1E8, i);
        BgStage_AdvanceFrame();
    }
}

void FreeBattle_SlideOutPanel(void) {
    s32 i;

    for (i = 9; i >= 0; i--) {
        FreeBattle_PollInputs();
        BgStage_DrawFrame();
        Gfx_DrawTiledRgba16Image(gMenuBackgroundAtlas);
        FreeBattle_DrawTitleBar();
        FreeBattle_DrawPanelFrame(0x4C, (s16)((0x16C - (s16)(0xE + i * 0x23)) / 2) + 0x54, 0x1E8, 0xE + i * 0x23);
        BgStage_AdvanceFrame();
    }

    for (i = 0; i < 2; i++) {
        BgStage_DrawFrame();
        Gfx_DrawTiledRgba16Image(gMenuBackgroundAtlas);
        BgStage_AdvanceFrame();
    }
}

#if defined(VERSION_US_REV2)
#pragma GLOBAL_ASM("asm/us/nonmatchings/fragments/free_battle_rules/free_battle_rules/FreeBattle_ApplyRuleSelection.s")
#else
void FreeBattle_ApplyRuleSelection(void) {
    char sp44[12];
    TeamRoster* temp_v0;
    s32 i;

    Session_SetMode(0, gRuleBattleIds[gSelectedRuleOption], 0);

    for (i = 0; i < 4; i++) {
        if (gMenuEntries[gMenuSelection.unk_00].unk_00 & (1 << i)) {
            sprintf(sp44, "%dP", i + 1);
            Team_AddTrainer(0, Trainer_Create(i, 0, sp44, Text_GetPlayerLabel(1)));
        }
    }

    if (gMenuSelection.unk_02 == 3) {
        gSessionContext.aiDifficulty = gSelectedRuleValue;
        temp_v0 = Trainer_Create(-1, 0, "COM", Text_GetPlayerLabel(3));
        Team_AddTrainer(1, temp_v0);
        if (gMenuSelection.unk_00 == 3) {
            temp_v0->unk_001 = 2;
        } else {
            temp_v0->unk_001 = 3;
        }
    } else {
        for (i = 0; i < 4; i++) {
            if (gMenuEntries[gMenuSelection.unk_02 + 4].unk_00 & (1 << i)) {
                sprintf(sp44, "%dP", i + 1);
                Team_AddTrainer(1, Trainer_Create(i, 0, sp44, Text_GetPlayerLabel(1)));
            }
        }
    }

    if (gSessionContext.battleSides[0].unk_01 == 2) {
        gSessionContext.battleSides[0].teamMember[0]->rosterInfo->unk_003 = 0x35;
        gSessionContext.battleSides[0].teamMember[1]->rosterInfo->unk_003 = 0x35;
    }

    if (gSessionContext.battleSides[1].unk_01 == 2) {
        gSessionContext.battleSides[1].teamMember[0]->rosterInfo->unk_003 = 0x35;
        gSessionContext.battleSides[1].teamMember[1]->rosterInfo->unk_003 = 0x35;
    }
}
#endif

s32 FreeBattle_RuleSelectLoop(void) {
    gRuleMenuState = 0;
    Audio_PlayMusicIfChanged(0x2D);
    FreeBattle_ClearRuleSelection();
    FreeBattle_ClearLevelSelection();
    FreeBattle_InitConfirmPanel();
    FreeBattle_FadeIn();
    FreeBattle_SlideInPanel();
    gRuleMenuState = 1;
    gMenuSelection.unk_00 = 0;

    while ((gRuleMenuState != 7) && (gRuleMenuState != 8)) {
        FreeBattle_PollInputs();
        switch (gRuleMenuState) {
            case 1:
                FreeBattle_UpdateRuleRowSelect();
                break;
            case 2:
                FreeBattle_UpdateRuleOptionSelect();
                break;
            case 3:
                FreeBattle_UpdateLevelSelect();
                break;
            case 4:
                FreeBattle_UpdateConfirmDialog();
                break;
            case 5:
                FreeBattle_OpenConfirmPanel();
                break;
            case 6:
                FreeBattle_CloseConfirmPanel();
                break;
        }
        FreeBattle_DrawFrame();
    }

    if (gRuleMenuState == 8) {
        FreeBattle_SlideOutPanel();
        FreeBattle_FadeOut();
        return 0;
    }

    FreeBattle_ApplyRuleSelection();
    FreeBattle_SlideOutPanel();
    return 1;
}

#if defined(VERSION_US_REV2)
#pragma GLOBAL_ASM("asm/us/nonmatchings/fragments/free_battle_rules/free_battle_rules/FreeBattle_RuleSelectMain.s")
#else
s32 FreeBattle_RuleSelectMain(s32 arg0, s32 arg1) {
    s32 sp1C;

    main_pool_push_state('RULE');

    Font_Init(0x1C, 0);
    ASSET_LOAD(D_1000000, common_menu1_ui, 0);
    ASSET_LOAD(D_2000000, common_menu2_ui, 0);
    ASSET_LOAD(D_3000000, order_select_ui, 0);
    Text_InitStringTables();
    gRuleMessageBank = Text_GetStringTable(0xF);
    gOptionMessageBank = Text_GetStringTable(6);
    gRuleBackgroundArchive = ASSET_LOAD2(backgrounds, 1, 1);
    gMenuBackgroundAtlas = BinArchive_GetFile(gRuleBackgroundArchive, 6);
    gControllerInputMask = Cont_GetControllerBits();
    StageLoader_UpdateSegments();
    sp1C = FreeBattle_RuleSelectLoop();
    StageLoader_WaitForRetrace();

    main_pool_pop_state('RULE');
    return sp1C;
}
#endif
