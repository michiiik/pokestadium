#include "rental_rules.h"
#include "src/poke_icon.h"
#include "src/graphics_textures.h"
#include "src/input.h"
#include "src/ui_graphics.h"
#include "src/save_data.h"
#include "src/text_system.h"
#include "src/jpeg_stream.h"
#include "src/audio_sfx.h"
#include "src/controller.h"
#include "src/memory.h"
#include "src/stage_loader.h"

static s16 D_83003C80;
static s16 gMenuFadeTimer;
static u16 D_83003C84;
static BinArchive* gBattleAssetArchive;
static void* gBattleBackgroundAtlas;
static s16 gMenuBackgroundTimer;
static s16 gMenuFadeAlpha;
static s16 gHeaderBannerProgress;
static s32 gCurrentBattleModeTitle;
static char* gCurrentBattleModeSubtitle;
static s16* gCurrentMenuOptionIds;
static s16 gCurrentMenuOptionCount;
static s16 gCurrentMenuSelectedIndex;
static char* sStadiumModeLabel;
static char* sFreeBattleModeLabel;
static char* sEventBattleModeLabel;
static char* sGymLeaderCastleModeLabel;
static char* sVsMewtwoModeLabel;
static char* sAnythingGoesLabel;
static char* sNintendoCup97Label;
static char* sNintendoCup98Label;
static char* sNintendoCup99Label;
static char* sPetitCupLabel;
static char* sPikaCupLabel;
static char* sPrimeCupLabel;
static char* sGymLeaderCastleLabel;
static char* sVsMewtwoLabel;
RentalRulesStringTable gMenuStaticStrings;
RentalRulesStringTable gMenuOptionStrings;

char** sBattleModeLabels = NULL;
static char** gRentalSpeciesNamesBank = NULL;
static u8 gBattleModeAssetIndices[] = {
    6, 0x07, 0x08, 9, 10, 11, 12, 13, 0x0F,
};
static s16 gNintendoCupStadiumRuleIds[] = {
    0x0007, 0x0000, 0x0001, 0x0002, 0x0003, 0x0004, 5, 6, 35, 0xFFFF,
};
static s16 gNintendoCupEventBattleRuleIds[] = {
    0x0007, 0x0000, 0x0001, 29, 0x0003, 0x0004, 5, 9, 35, 0xFFFF,
};
static s16 gNintendoCupFreeBattleRuleIds[] = {
    0x0007, 0x0000, 0x0001, 0x0002, 0x0003, 0x0004, 5, 35, 0xFFFF, 0x0000,
};
static s16 gNintendoCup2StadiumRuleIds[] = {
    11, 0x0000, 0x0001, 10, 0x0004, 5, 6, 35, 0xFFFF, 0x0000,
};
static s16 gNintendoCup2EventBattleRuleIds[] = {
    11, 0x0000, 0x0001, 30, 0x0004, 5, 9, 35, 0xFFFF, 0x0000,
};
static s16 gNintendoCup2FreeBattleRuleIds[] = {
    11,     0x0000, 0x0001, 10,     0x0004, 5,  35,     0xFFFF, 14,     0x0000, 0x0001, 12,     13,
    0x0004, 5,      6,      35,     0xFFFF, 14, 0x0000, 0x0001, 0x001F, 13,     0x0004, 5,      9,
    35,     0xFFFF, 14,     0x0000, 0x0001, 12, 13,     0x0004, 5,      35,     0xFFFF, 0x0000,
};
static s16 gPetitCupStadiumRuleIds[] = {
    19, 0x0000, 0x0001, 0x000F, 0x0010, 17, 18, 0x0004, 5, 6, 35, 0xFFFF,
};
static s16 gPetitCupEventBattleRuleIds[] = {
    19, 0x0000, 0x0001, 0x0020, 0x0010, 17, 18, 0x0004, 5, 9, 35, 0xFFFF,
};
static s16 gPetitCupFreeBattleRuleIds[] = {
    19, 0x0000, 0x0001, 0x000F, 0x0010, 17, 18, 0x0004, 5, 35, 0xFFFF, 0x0000,
};
static s16 gPikaCupStadiumRuleIds[] = {
    0x0000, 0x0001, 22, 23, 0x0004, 5, 6, 35, 0xFFFF, 0x0000,
};
static s16 gPikaCupEventBattleRuleIds[] = {
    0x0000, 0x0001, 33, 23, 0x0004, 5, 9, 35, 0xFFFF, 0x0000,
};
static s16 gPikaCupFreeBattleRuleIds[] = {
    0x0000, 0x0001, 22, 23, 0x0004, 5, 35, 0xFFFF,
};
static s16 gPrimeCupStadiumRuleIds[] = {
    21, 0x0000, 0x0001, 36, 0x0004, 6, 35, 0xFFFF,
};
static s16 gPrimeCupEventBattleRuleIds[] = {
    21, 0x0000, 0x0001, 20, 0x0004, 9, 35, 0xFFFF,
};
static s16 gPrimeCupFreeBattleRuleIds[] = {
    21, 0x0000, 0x0001, 20, 0x0004, 35, 0xFFFF, 0x0000,
};
static s16 gAnythingGoesFreeBattleRuleIds[] = {
    21, 26, 20, 37, 34, 38, 35, 0xFFFF,
};
static s16 gGymLeaderCastleRuleIds[] = {
    21, 0x0000, 0x0001, 20, 25, 0x0004, 35, 0xFFFF,
};
static s16 gMenuOptions_Mode8_S8[] = {
    21, 28, 27, 20, 35, 0xFFFF,
};

char* RentalRules_GetPokemonName(s32 file_number) {
    return Text_GetString(NULL, 0, gRentalSpeciesNamesBank, file_number - 1);
}

void RentalRules_LoadBackgroundArchive(void) {
    ModeSettings var_sp20;

    if (gSessionContext.mode != 10) {
        gBattleAssetArchive = BinArchive_Open(backgrounds_ROM_START, battle_headers_ROM_START, 1, 1);
        if (gSessionContext.mode == 7) {
            Save_EnsureBankLoaded(2);
            Save_GetModeSettings(&var_sp20, gSessionContext.inRound2);
            if (var_sp20.castleClears < 8) {
                gBattleBackgroundAtlas = BinArchive_GetFile(gBattleAssetArchive, 13);
            } else {
                gBattleBackgroundAtlas = BinArchive_GetFile(gBattleAssetArchive, 0x10);
            }
        } else {
            gBattleBackgroundAtlas = BinArchive_GetFile(gBattleAssetArchive, gBattleModeAssetIndices[gSessionContext.mode]);
        }
    }
}

void RentalRules_PollInput(void) {
    Cont_StartReadInputs();
    Cont_ReadInputs();
    Input_ResetRepeatState();
}

s16* RentalRules_GetRuleIdList(SessionContext* session_context) {
    s16* var_v1 = NULL;

    switch (session_context->ruleCategory) {
        case 1:
            switch (session_context->mode) {
                case 1:
                    var_v1 = gNintendoCupStadiumRuleIds;
                    break;
                case 10:
                    var_v1 = gNintendoCupEventBattleRuleIds;
                    break;
                case 0:
                    var_v1 = gNintendoCupFreeBattleRuleIds;
                    break;
            }
            break;

        case 2:
            switch (session_context->mode) {
                case 2:
                    var_v1 = gNintendoCup2StadiumRuleIds;
                    break;
                case 10:
                    var_v1 = gNintendoCup2EventBattleRuleIds;
                    break;
                case 0:
                    var_v1 = gNintendoCup2FreeBattleRuleIds;
                    break;
            }
            break;

        case 3:
            switch (session_context->mode) {
                case 3:
                    var_v1 = gNintendoCupStadiumRuleIds;
                    break;
                case 10:
                    var_v1 = gNintendoCupEventBattleRuleIds;
                    break;
                case 0:
                    var_v1 = gNintendoCupFreeBattleRuleIds;
                    break;
            }
            break;

        case 4:
            switch (session_context->mode) {
                case 4:
                    var_v1 = gPetitCupStadiumRuleIds;
                    break;
                case 10:
                    var_v1 = gPetitCupEventBattleRuleIds;
                    break;
                case 0:
                    var_v1 = gPetitCupFreeBattleRuleIds;
                    break;
            }
            break;

        case 5:
            switch (session_context->mode) {
                case 5:
                    var_v1 = gPikaCupStadiumRuleIds;
                    break;
                case 10:
                    var_v1 = gPikaCupEventBattleRuleIds;
                    break;
                case 0:
                    var_v1 = gPikaCupFreeBattleRuleIds;
                    break;
            }
            break;

        case 6:
            switch (session_context->mode) {
                case 6:
                    var_v1 = gPrimeCupStadiumRuleIds;
                    break;
                case 10:
                    var_v1 = gPrimeCupEventBattleRuleIds;
                    break;
                case 0:
                    var_v1 = gPrimeCupFreeBattleRuleIds;
                    break;
            }
            break;

        case 0:
            switch (session_context->mode) {
                case 0:
                    var_v1 = gAnythingGoesFreeBattleRuleIds;
                    break;
            }
            break;

        case 7:
            switch (session_context->mode) {
                case 7:
                    var_v1 = gGymLeaderCastleRuleIds;
                    break;
            }
            break;

        case 8:
            switch (session_context->mode) {
                case 8:
                    var_v1 = gMenuOptions_Mode8_S8;
                    break;
            }
            break;
    }

    return var_v1;
}

char* RentalRules_GetModeLabel(s8 arg0) {
    char* ret;

    switch (arg0) {
        case 0:
            ret = sFreeBattleModeLabel;
            break;

        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
            ret = sStadiumModeLabel;
            break;

        case 7:
            ret = sGymLeaderCastleModeLabel;
            break;

        case 8:
            ret = sVsMewtwoModeLabel;
            break;

        case 10:
            ret = sEventBattleModeLabel;
            break;

        default:
            ret = NULL;
            break;
    }

    return ret;
}

char* RentalRules_GetCategoryLabel(s8 arg0) {
    char* label;

    switch (arg0) {
        case 0:
            label = sAnythingGoesLabel;
            break;

        case 1:
            label = sNintendoCup97Label;
            break;

        case 2:
            label = sNintendoCup98Label;
            break;

        case 3:
            label = sNintendoCup99Label;
            break;

        case 4:
            label = sPetitCupLabel;
            break;

        case 5:
            label = sPikaCupLabel;
            break;

        case 6:
            label = sPrimeCupLabel;
            break;

        case 7:
            label = sGymLeaderCastleLabel;
            break;

        case 8:
            label = sVsMewtwoLabel;
            break;

        default:
            label = NULL;
            break;
    }

    return label;
}

s32 RentalRules_RuleListHasEmptyEntry(s16* arg0) {
    s32 ret = 0;

    while (*arg0 != -1) {
        if (gMenuOptionStrings.unk_00[*arg0] == '\x00') {
            ret = 1;
            break;
        }
        arg0++;
    }

    return ret;
}

void RentalRules_UpdateFadeIn(void) {
    s32 var_lo;

    gMenuFadeTimer--;

    gMenuFadeAlpha = var_lo = ((10 - gMenuFadeTimer) << 10) / 10;

    if (D_83003C84 & 4) {
        gHeaderBannerProgress = var_lo;
    }

    if (gMenuFadeTimer <= 0) {
        D_83003C80 = 1;
        gMenuBackgroundTimer = 2;
    }
}

void RentalRules_UpdateMainInput(void) {
    s16 tmp = gCurrentMenuSelectedIndex;

    if (BTN_IS_PRESSED(gPlayer1Controller, BTN_DUP)) {
        gCurrentMenuSelectedIndex--;
        if (gCurrentMenuSelectedIndex < 0) {
            gCurrentMenuSelectedIndex = gCurrentMenuOptionCount - 1;
        }
    }

    if (BTN_IS_PRESSED(gPlayer1Controller, BTN_DDOWN)) {
        gCurrentMenuSelectedIndex++;
        if (gCurrentMenuSelectedIndex >= gCurrentMenuOptionCount) {
            gCurrentMenuSelectedIndex = 0;
        }
    }

    if (gCurrentMenuSelectedIndex != tmp) {
        Audio_PlaySoundEffectById(SFX_MENU_SCROLL);
    }

    if (BTN_IS_PRESSED(gPlayer1Controller, BTN_A)) {
        D_83003C80 = 2;
        gMenuFadeTimer = 0;

        if (gMenuOptionStrings.unk_00[gCurrentMenuOptionIds[gCurrentMenuSelectedIndex]] != NULL) {
            RentalRules_PopupOpen(gCurrentMenuOptionIds[gCurrentMenuSelectedIndex]);
        } else {
            RentalRules_QualifyingSpeciesWindowOpen();
            Audio_PlaySoundEffectById(SFX_PANEL_OPEN);
        }

        Audio_PlaySoundEffectById(SFX_MENU_CONFIRM);
    } else if (BTN_IS_PRESSED(gPlayer1Controller, BTN_B)) {
        D_83003C80 = 3;
        gMenuFadeTimer = 10;
        gMenuBackgroundTimer = -1;
        Audio_PlaySoundEffectById(SFX_MENU_BACK);
    }
}

void RentalRules_UpdateWaitPopupClose(void) {
    if ((RentalRules_PopupIsActive() == 0) && (RentalRules_QualifyingSpeciesWindowIsActive() == 0)) {
        D_83003C80 = 1;
    }
}

void RentalRules_UpdateFadeOut(void) {
    s32 var_lo;

    gMenuFadeTimer--;
    var_lo = (gMenuFadeTimer << 10) / 10;

    gMenuFadeAlpha = var_lo;
    if (D_83003C84 & 4) {
        gHeaderBannerProgress = var_lo;
    }

    if (gMenuFadeTimer <= 0) {
        D_83003C80 = 4;
        gMenuFadeTimer = 2;
        gMenuBackgroundTimer = -1;
    }
}

s32 RentalRules_UpdateExitDelay(void) {
    s32 var_v1 = 0;

    gMenuFadeTimer--;
    if (gMenuFadeTimer <= 0) {
        var_v1 = 1;
    }
    return var_v1;
}

void RentalRules_DrawBackgroundFallback(void) {
    gDPPipeSync(gDisplayListHead++);

    gDPSetCycleType(gDisplayListHead++, G_CYC_1CYCLE);
    gDPSetTexturePersp(gDisplayListHead++, G_TP_NONE);
    gDPSetCombineLERP(gDisplayListHead++, ENVIRONMENT, PRIMITIVE, TEXEL0, PRIMITIVE, TEXEL0, 0, ENVIRONMENT, 0,
                      ENVIRONMENT, PRIMITIVE, TEXEL0, PRIMITIVE, TEXEL0, 0, ENVIRONMENT, 0);
    gDPSetRenderMode(gDisplayListHead++, G_RM_OPA_SURF, G_RM_OPA_SURF2);
    gDPSetEnvColor(gDisplayListHead++, 140, 190, 200, 255);
    gDPSetPrimColor(gDisplayListHead++, 0, 0, 110, 160, 170, 255);
    gSPClearGeometryMode(gDisplayListHead++, G_ZBUFFER | G_LIGHTING);

    gDPLoadTextureBlock_4b(gDisplayListHead++, D_302EBC0, G_IM_FMT_I, 128, 64, 0, G_TX_NOMIRROR | G_TX_WRAP,
                           G_TX_NOMIRROR | G_TX_WRAP, 7, 6, G_TX_NOLOD, G_TX_NOLOD);

    Gfx_DrawTexturedRectClipped(0, 0, 640, 480, 0, 0, 0x400, 0x400, 0);

    gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);
}

void RentalRules_DrawHeaderBanner(void) {
    UNUSED s32 pad;
    s16 var_s1;
    s16 var_spD8;

    if (gHeaderBannerProgress == 0) {
        return;
    }

    var_s1 = (((-gHeaderBannerProgress * 0x10) + 0x4000) / 1024) + 40;
    var_spD8 = (gHeaderBannerProgress << 5) / 1024;

    gSPDisplayList(gDisplayListHead++, gDL_XluBorder);
    gDPSetEnvColor(gDisplayListHead++, 100, 100, 200, 255);
    gDPSetPrimColor(gDisplayListHead++, 0, 0, 40, 40, 140, 255);

    gDPLoadTextureBlock(gDisplayListHead++, D_20003C0, G_IM_FMT_IA, G_IM_SIZ_8b, 16, 32, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                        G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

    Gfx_DrawTexturedRectClipped(56, var_s1, 0x10, var_spD8, 0, 0, 1008, 0x8000 / var_spD8, 0);

    gDPLoadTextureBlock(gDisplayListHead++, D_20005C0, G_IM_FMT_IA, G_IM_SIZ_8b, 16, 32, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                        G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

    Gfx_DrawTexturedRectClipped(576, var_s1, 0x10, var_spD8, 0, 0, 0x400, 0x8000 / var_spD8, 0);
    Gfx_DrawTexturedRectClipped(72, var_s1, 504, var_spD8, 0, 0, 0, 0x8000 / var_spD8, 0);

    if (gHeaderBannerProgress < 0x400) {
        gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);
    } else {
        gSPDisplayList(gDisplayListHead++, gUiEnvTintTextureDL);

        Gfx_DrawTextureRgba16(48, var_s1 - 8, 48, 24, D_2006C00, 48, 0);
        Gfx_DrawTextureRgba16(48, var_s1 + 0x10, 48, 24, D_2007500, 48, 0);

        gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);

        Font_BeginTranslucentTextRendering();
        Font_SetActive(0x10, 0);

        if (gCurrentBattleModeTitle != 0) {
            char var_sp48[0x40];
            char* var_v0_21;

            Gfx_SetEnvColor(0, 0, 0, 0xFF);
            Text_SetStringToken(38, gCurrentBattleModeTitle);
            var_v0_21 = Text_GetString(var_sp48, sizeof(var_sp48), sBattleModeLabels, 0);
            Font_Printf(106, var_s1 + 4, var_v0_21);
            Gfx_SetEnvColor(0xFF, 0xFF, 0x77, 0xFF);
            Font_Printf(104, var_s1 + 2, var_v0_21);
        }

        Font_EndTexturedTextRendering();
    }
}

void RentalRules_DrawWindowFrame(s16 left, s16 bottom, s16 arg2, s16 arg3) {
    gSPDisplayList(gDisplayListHead++, gUiEnvTintTextureDL);

    Gfx_DrawTextureIa8(left - 7, bottom - 7, 8, 8, D_2000340, 8, 0);
    Gfx_DrawTextureIa8((left + arg2) - 1, bottom - 7, 8, 8, D_2000380, 8, 0);
    Gfx_DrawTexturedRectClipped(left + 1, bottom - 7, arg2 - 2, 8, 0, 0, 0, 0x400, 0);
    Gfx_DrawTextureIa8(left - 7, (bottom + arg3) - 1, 8, 8, D_20002C0, 8, 0);
    Gfx_DrawTexturedRectClipped(left - 7, bottom + 1, 8, arg3 - 2, 0, 0, 0x400, 0, 0);
    Gfx_DrawTextureIa8((left + arg2) - 1, (bottom + arg3) - 1, 8, 8, D_2000300, 8, 0);
    Gfx_DrawTexturedRectClipped(left + 1, (bottom + arg3) - 1, arg2 - 2, 8, 0, 0, 0, 0x400, 0);
    Gfx_DrawTexturedRectClipped((left + arg2) - 1, bottom + 1, 8, arg3 - 2, 0, 0, 0x400, 0, 0);

    gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);
}

void RentalRules_DrawFilledPanel(s16 x1, s16 y1, s16 width, s16 height, u8 r, u8 g, u8 b) {
    gSPDisplayList(gDisplayListHead++, gUiEnvTintTextureDL);

    gDPSetRenderMode(gDisplayListHead++, G_RM_AA_OPA_SURF, G_RM_AA_OPA_SURF2);
    gDPSetEnvColor(gDisplayListHead++, r, g, b, 255);

    Gfx_DrawTextureIa8(x1, y1, 8, 8, D_2000B40, 8, 0);
    Gfx_DrawTextureIa8((x1 + width) - 8, y1, 8, 8, D_2000B80, 8, 0);
    Gfx_DrawTexturedRectClipped(x1 + 8, y1, width - 16, 8, 0, 0, 0, 0x400, 0);
    Gfx_DrawTextureIa8(x1, (y1 + height) - 8, 8, 8, D_2000BC0, 8, 0);
    Gfx_DrawTexturedRectClipped(x1, y1 + 8, 8, height - 16, 0, 0, 0x400, 0, 0);
    Gfx_DrawTextureIa8((x1 + width) - 8, (y1 + height) - 8, 8, 8, D_2000C00, 8, 0);
    Gfx_DrawTexturedRectClipped(x1 + 8, (y1 + height) - 8, width - 16, 8, 0, 0, 0, 0x400, 0);
    Gfx_DrawTexturedRectClipped((x1 + width) - 8, y1 + 8, 8, height - 16, 0, 0, 0x400, 0, 0);

    gSPDisplayList(gDisplayListHead++, gUiTranslucentFillDL);

    Gfx_FillRectRgba(x1 + 8, y1 + 8, width - 16, height - 16, r, g, b, 0xFF);

    gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);
}

void RentalRules_DrawBulletIcon(s16 x, s16 y) {
    gDPLoadTextureTile(gDisplayListHead++, D_4000C40, G_IM_FMT_IA, G_IM_SIZ_8b, 20, 0, 0, 0, 19, 19, 0,
                       G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                       G_TX_NOLOD);

    Gfx_DrawTexturedRectClipped(x, y, 20, 20, 0, 0, 0x400, 0x400, 0);
}

void RentalRules_DrawRuleListText(s16 x, s16 y) {
    s16 tmp;
    s32 i;
    char var_sp48[0x40];

    Font_SetActive(0x10, 0);
    Gfx_SetEnvColor(0xFF, 0xFF, 0xFF, 0xFF);

    if (gCurrentBattleModeSubtitle != NULL) {
        if (gCurrentBattleModeSubtitle != sAnythingGoesLabel) {
            Text_SetStringToken(39, gCurrentBattleModeSubtitle);
            Font_Printf(x + 16, y + 4, Text_GetString(var_sp48, sizeof(var_sp48), sBattleModeLabels, 1));
        } else {
            Font_Printf(x + 16, y + 4, gCurrentBattleModeSubtitle);
        }
    }

    Font_Printf(x + 48, y + 308, Text_GetString(NULL, 0, sBattleModeLabels, 2));
    Font_SetActive(8, 0);

    for (i = 0; i < gCurrentMenuOptionCount; i++) {
        if (i != gCurrentMenuSelectedIndex) {
            Gfx_SetEnvColor(0xFF, 0xFF, 0xFF, 0xFF);
        } else {
            Gfx_SetEnvColor(0xFF, 0xFF, 0, 0xFF);
        }
        tmp = gCurrentMenuOptionIds[i];
        Font_Printf(x + 84, y + 38 + i * 24, gMenuStaticStrings.unk_00[tmp]);
    }
}

void RentalRules_DrawRuleListPanel(void) {
    static s16 D_83003B60 = 0;

    s32 i;
    s16 var_s3;
    s16 var_s2_2;

    if (gMenuFadeAlpha == 0) {
        return;
    }

    var_s3 = (((-gMenuFadeAlpha * 168) + 172032) / 1024) + 92;
    var_s2_2 = (gMenuFadeAlpha * 336) / 1024;
    RentalRules_DrawWindowFrame(56, var_s3, 528, var_s2_2);

    if (gMenuFadeAlpha >= 0x400) {
        RentalRules_DrawFilledPanel(56, var_s3, 528, 0x20, 0x3C, 0x3C, 0xA0);
        RentalRules_DrawFilledPanel(56, var_s3 + 0x20, 528, 272, 0x1E, 0x1E, 0x64);
        RentalRules_DrawFilledPanel(56, var_s3 + 304, 528, 0x20, 0x3C, 0x3C, 0xA0);
        if ((RentalRules_PopupGetProgress() == 0) && (RentalRules_GetSpeciesWindowOpeningProgress() == 0)) {
            s16 sins = SINS(D_83003B60) * 3.0f;

            Ui_DrawTextureMarker(sins + 72, var_s3 + (gCurrentMenuSelectedIndex * 24) + 42);
            D_83003B60 += 0x1000;
        }

        gSPDisplayList(gDisplayListHead++, gUiEnvTintTextureDL);

        Gfx_DrawTextureRgba16(72, var_s3 + 306, 28, 28, gRentalViewerIconTop, 28, 0);

        for (i = 0; i < gCurrentMenuOptionCount; i++) {
            RentalRules_DrawBulletIcon(116, var_s3 + 40 + i * 24);
        }

        gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);

        Font_BeginTranslucentTextRendering();
        RentalRules_DrawRuleListText(56, var_s3);
        Font_EndTexturedTextRendering();
    }
}

void RentalRules_DrawFrame(void) {
    BgStage_DrawFrame();
    if (gMenuBackgroundTimer != 0) {
        if (gSessionContext.mode == 10) {
            RentalRules_DrawBackgroundFallback();
        } else {
            Gfx_DrawTiledRgba16Image(gBattleBackgroundAtlas);
        }

        RentalRules_DrawHeaderBanner();

        if (gMenuBackgroundTimer > 0) {
            gMenuBackgroundTimer--;
        }
    }

    if ((RentalRules_PopupGetProgress() < 0x400) && (RentalRules_GetSpeciesWindowOpeningProgress() < 0x400)) {
        RentalRules_DrawRuleListPanel();
    }

    RentalRules_PopupDraw();
    RentalRules_QualifyingSpeciesWindowDraw();
    BgStage_AdvanceFrame();
}

void RentalRules_LoadCategoryStrings(void) {
    s32 i;

    for (i = 0; i < 29; i++) {
        gMenuStaticStrings.unk_00[i] = Text_GetString(NULL, 0, sBattleModeLabels, i + 4);
    }

    gMenuStaticStrings.unk_74 = gMenuStaticStrings.unk_00[2];
    gMenuStaticStrings.unk_78 = gMenuStaticStrings.unk_00[10];
    gMenuStaticStrings.unk_7C = gMenuStaticStrings.unk_00[12];
    gMenuStaticStrings.unk_80 = gMenuStaticStrings.unk_00[15];
    gMenuStaticStrings.unk_84 = gMenuStaticStrings.unk_00[22];
    gMenuStaticStrings.unk_88 = gMenuStaticStrings.unk_00[27];
    gMenuStaticStrings.unk_8C = Text_GetString(NULL, 0, sBattleModeLabels, 33);
    gMenuStaticStrings.unk_90 = gMenuStaticStrings.unk_00[20];
    gMenuStaticStrings.unk_94 = gMenuStaticStrings.unk_00[4];
    gMenuStaticStrings.unk_98 = Text_GetString(NULL, 0, sBattleModeLabels, 34);
}

void RentalRules_LoadDescriptionStrings(void) {
    s32 i;
    s32 var_s2 = 0;

    for (i = 0; i < 39; i++) {
        switch (i) {
            case 7:
            case 11:
            case 14:
            case 19:
            case 24:
                gMenuOptionStrings.unk_00[i] = NULL;
                break;

            default:
                gMenuOptionStrings.unk_00[i] = Text_GetString(NULL, 0, sBattleModeLabels, var_s2 + 35);
                var_s2++;
                break;
        }
    }
}

void RentalRules_Init(void) {
    D_83003C80 = 0;
    gMenuFadeTimer = 10;
    gMenuBackgroundTimer = -1;
    gMenuFadeAlpha = 0;
    Audio_PlaySoundEffectById(SFX_PANEL_OPEN);
    gCurrentBattleModeTitle = 0;
    gCurrentBattleModeSubtitle = NULL;
    gCurrentMenuSelectedIndex = 0;
    D_83003C84 = 0;

    gCurrentMenuOptionIds = RentalRules_GetRuleIdList(&gSessionContext);
    if (gCurrentMenuOptionIds == 0) {
        gCurrentMenuOptionCount = 0;
    } else {
        gCurrentMenuOptionCount = 0;
        while (gCurrentMenuOptionIds[gCurrentMenuOptionCount] != -1) {
            gCurrentMenuOptionCount++;
        }
    }

    RentalRules_LoadCategoryStrings();
    RentalRules_LoadDescriptionStrings();

    sStadiumModeLabel = Text_GetString(NULL, 0, sBattleModeLabels, 70);
    sFreeBattleModeLabel = Text_GetString(NULL, 0, sBattleModeLabels, 69);
    sEventBattleModeLabel = Text_GetString(NULL, 0, sBattleModeLabels, 73);
    sGymLeaderCastleModeLabel = Text_GetString(NULL, 0, sBattleModeLabels, 71);
    sVsMewtwoModeLabel = Text_GetString(NULL, 0, sBattleModeLabels, 72);
    sAnythingGoesLabel = Text_GetString(NULL, 0, sBattleModeLabels, 74);
    sNintendoCup97Label = Text_GetString(NULL, 0, sBattleModeLabels, 77);
    sNintendoCup98Label = Text_GetString(NULL, 0, sBattleModeLabels, 76);
    sNintendoCup99Label = Text_GetString(NULL, 0, sBattleModeLabels, 75);
    sPetitCupLabel = Text_GetString(NULL, 0, sBattleModeLabels, 78);
    sPikaCupLabel = Text_GetString(NULL, 0, sBattleModeLabels, 79);
    sPrimeCupLabel = Text_GetString(NULL, 0, sBattleModeLabels, 80);

    sGymLeaderCastleLabel = sGymLeaderCastleModeLabel;
    sVsMewtwoLabel = sVsMewtwoModeLabel;

    gCurrentBattleModeTitle = RentalRules_GetModeLabel(gSessionContext.mode);
    gCurrentBattleModeSubtitle = RentalRules_GetCategoryLabel(gSessionContext.ruleCategory);

    if (gSessionContext.inRound2 != 0) {
        D_83003C84 |= 1;
    }

    if (RentalRules_RuleListHasEmptyEntry(gCurrentMenuOptionIds) != 0) {
        D_83003C84 |= 2;
    }

    if (D_83003C84 & 4) {
        gHeaderBannerProgress = 0;
    } else {
        gHeaderBannerProgress = 0x400;
    }

    RentalRules_PopupReset();

    RentalRules_QualifyingSpeciesWindowInit((D_83003C84 & 2) ? 1 : 0);
}

s32 RentalRules_Update(void) {
    s32 var_sp1C = 1;

    switch (D_83003C80) {
        case 0:
            RentalRules_UpdateFadeIn();
            break;

        case 1:
            RentalRules_UpdateMainInput();
            break;

        case 2:
            RentalRules_UpdateWaitPopupClose();
            break;

        case 3:
            RentalRules_UpdateFadeOut();
            break;

        case 4:
            if (RentalRules_UpdateExitDelay() != 0) {
                var_sp1C = 0;
            }
            break;
    }
    return var_sp1C;
}

void RentalRules_Loop(void) {
    RentalRules_Init();
    do {
        RentalRules_PollInput();
        RentalRules_PopupUpdate();
        RentalRules_QualifyingSpeciesWindowUpdate();
        RentalRules_DrawFrame();
    } while (RentalRules_Update() != 0);
}

s32 RentalRules_Main(UNUSED s32 arg0, UNUSED s32 arg1) {
    main_pool_push_state('EXPL');

    Font_Init(28, 0);

    ASSET_LOAD(D_1000000, common_menu1_ui, 0);
    ASSET_LOAD(D_2000000, common_menu2_ui, 0);
    ASSET_LOAD(D_3000000, menu_select_ui, 0);
    ASSET_LOAD(gRentalViewerIconTop, rental_rules_ui, 0);

    RentalRules_LoadBackgroundArchive();
    PokeIcon_OpenModelArchive();
    Text_InitStringTables();
    sBattleModeLabels = Text_GetStringTable(0x20);
    gRentalSpeciesNamesBank = Text_GetStringTable(36);
    StageLoader_UpdateSegments();
    RentalRules_Loop();
    StageLoader_WaitForRetrace();

    main_pool_pop_state('EXPL');

    return 0;
}
