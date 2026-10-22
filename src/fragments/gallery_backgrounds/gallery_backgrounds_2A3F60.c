#include "gallery_backgrounds.h"
#include "src/graphics_textures.h"
#include "src/ui_graphics.h"
#include "src/audio_sfx.h"
#include "src/fragments/gallery_backgrounds/gallery_backgrounds.h"

static s16 gConfirmPromptId;
static Controller* gConfirmPromptController;
static s16 gConfirmPromptAnimTimer;
static s16 gConfirmPromptCursorIndex;

static s16 gConfirmPromptState = 0;
static s16 gConfirmPromptSfxTable[] = { 3, 4, 1 };

static MenuPanelInfo gConfirmPromptPanels[] = {
    {
        120,
        160,
        400,
        64,
        1,
        0,
        26889,
        NULL,
    },
    {
        120,
        160,
        400,
        64,
        1,
        0,
        26889,
        NULL,
    },
    {
        120,
        160,
        400,
        196,
        3,
        0,
        26889,
        NULL,
    },
    {
        120,
        160,
        400,
        176,
        3,
        0,
        26889,
        gConfirmPromptSfxTable,
    },
    {
        220,
        160,
        200,
        64,
        1,
        0,
        26889,
        NULL,
    },
};

static s16 gSaveOptionStringIds[] = { 0x2B, 0x2C, 0x2D, 0 };
static s16 gSaveOptionYOffsets[] = { 0, 0x22, 0x58, 0 };
static s16 gDeleteOptionStringIds[] = {
    0x2F,
    0x30,
    0x31,
    0,
};

void Gallery_DrawYesNoOptions(MenuPanelInfo* arg0, s16 arg1) {
    Font_BeginTranslucentTextRendering();
    Font_SetActive(0x10, 0);
    if (arg1 == 0) {
        Font_Printf(arg0->unk_00 + 0x18, arg0->unk_02 + 0x14, Gallery_GetCommonUiString(0x29));
    } else {
        Font_Printf(arg0->unk_00 + 0x39, arg0->unk_02 + 0x14, Gallery_GetCommonUiString(0x2A));
    }
    Font_EndTexturedTextRendering();
}

void Gallery_DrawSaveOptions(MenuPanelInfo* arg0) {
    char* temp_v0;
    s32 i;
    s32 tmp;

    Font_BeginTranslucentTextRendering();
    Font_SetActive(0x10, 0);
    temp_v0 = Gallery_GetUiString(0x2A);
    tmp = (arg0->unk_04 - Font_MeasureTextExtent(0, 0, temp_v0)) / 2;
    Font_Printf(arg0->unk_00 + tmp, arg0->unk_02 + 0x14, temp_v0);

    for (i = 0; i < arg0->unk_08; i++) {
        Font_SetActive(0x10, 0);
        temp_v0 = Gallery_GetUiString(gSaveOptionStringIds[i]);
        if (i == gConfirmPromptCursorIndex) {
            Gfx_SetEnvColor(0xFF, 0xFF, 0, 0xFF);
        } else {
            Gfx_SetEnvColor(0xFF, 0xFF, 0xFF, 0xFF);
        }

        Font_Printf(arg0->unk_00 + 0x3C, gSaveOptionYOffsets[i] + arg0->unk_02 + 0x40, temp_v0);

        if (i == 1) {
            Font_SetActive(8, 0);
            Font_Printf(arg0->unk_00 + 0x3C, gSaveOptionYOffsets[i] + arg0->unk_02 + 0x58, Gallery_GetUiString(0x36));
        }
    }

    Font_EndTexturedTextRendering();

    if (gConfirmPromptCursorIndex >= 0) {
        Ui_DrawAnimatedTextureMarker(arg0->unk_00 + 0x10, gSaveOptionYOffsets[gConfirmPromptCursorIndex] + arg0->unk_02 + 0x48);
    }
}

void Gallery_DrawDeleteOptions(MenuPanelInfo* arg0) {
    s32 i;
    s32 tmp;
    char* temp_v0;

    Font_BeginTranslucentTextRendering();
    Font_SetActive(0x10, 0);
    temp_v0 = Gallery_GetUiString(0x2E);
    tmp = (arg0->unk_04 - Font_MeasureTextExtent(0, 0, temp_v0)) / 2;
    Font_Printf(arg0->unk_00 + tmp, arg0->unk_02 + 0x14, temp_v0);

    for (i = 0; i < arg0->unk_08; i++) {
        Font_SetActive(0x10, 0);
        temp_v0 = Gallery_GetUiString(gDeleteOptionStringIds[i]);
        if (i == gConfirmPromptCursorIndex) {
            Gfx_SetEnvColor(0xFF, 0xFF, 0, 0xFF);
        } else {
            Gfx_SetEnvColor(0xFF, 0xFF, 0xFF, 0xFF);
        }
        Font_Printf(arg0->unk_00 + 0x3C, arg0->unk_02 + (i * 0x22) + 0x40, temp_v0);
    }

    Font_EndTexturedTextRendering();

    if (gConfirmPromptCursorIndex >= 0) {
        Ui_DrawAnimatedTextureMarker(arg0->unk_00 + 0x10, arg0->unk_02 + (gConfirmPromptCursorIndex * 0x22) + 0x48);
    }
}

void Gallery_DrawMessagePrompt(MenuPanelInfo* arg0) {
    UNUSED s32 pad;
    s32 sp18 = (0xC8 - Font_MeasureTextExtent(0x10, 0, Gallery_GetUiString(0x41))) / 2;

    Font_BeginTranslucentTextRendering();
    Font_SetActive(0x10, 0);
    Font_Printf(arg0->unk_00 + sp18, arg0->unk_02 + 0x14, Gallery_GetUiString(0x41));
    Font_EndTexturedTextRendering();
}

void Gallery_ResetConfirmPrompt(void) {
    gConfirmPromptState = 0;
}

void Gallery_DrawConfirmPrompt(UNUSED s32 arg0) {
    s16 var_a3;
    MenuPanelInfo* temp_s0;
    s16 tmp;

    if ((gConfirmPromptState != 0) && (gConfirmPromptAnimTimer > 0)) {
        temp_s0 = &gConfirmPromptPanels[gConfirmPromptId];
        if (gConfirmPromptAnimTimer < 4) {
            var_a3 = (temp_s0->unk_06 * gConfirmPromptAnimTimer) / 4;
            if (var_a3 < 16) {
                var_a3 = 16;
            }
            tmp = temp_s0->unk_02 + ((temp_s0->unk_06 - var_a3) / 2);

            Ui_DrawBorderedPanel(temp_s0->unk_00, tmp, temp_s0->unk_04, var_a3, temp_s0->unk_0A);
        } else {
            Ui_DrawBorderedPanel(temp_s0->unk_00, temp_s0->unk_02, temp_s0->unk_04, temp_s0->unk_06, temp_s0->unk_0A);

            switch (gConfirmPromptId) {
                case 0:
                    Gallery_DrawYesNoOptions(temp_s0, 0);
                    break;

                case 1:
                    Gallery_DrawYesNoOptions(temp_s0, 1);
                    break;

                case 2:
                    Gallery_DrawSaveOptions(temp_s0);
                    break;

                case 3:
                    Gallery_DrawDeleteOptions(temp_s0);
                    break;

                case 4:
                    Gallery_DrawMessagePrompt(temp_s0);
                    break;
            }
        }
    }
}

void Gallery_UpdateConfirmPromptInput(void) {
    s32 tmpA;
    MenuPanelInfo* temp_v0;
    s16* temp_a3;
    s32 tmp8;

    temp_v0 = &gConfirmPromptPanels[gConfirmPromptId];
    tmp8 = temp_v0->unk_08;
    temp_a3 = temp_v0->unk_0C;

    if (gConfirmPromptController != NULL) {

        if (tmp8 >= 2) {
            tmpA = gConfirmPromptCursorIndex;
            if (gConfirmPromptController->buttonPressed & 0x800) {
                gConfirmPromptCursorIndex--;
                if (gConfirmPromptCursorIndex < 0) {
                    gConfirmPromptCursorIndex = tmp8 - 1;
                }
            }

            if (gConfirmPromptController->buttonPressed & 0x400) {
                gConfirmPromptCursorIndex++;
                if (gConfirmPromptCursorIndex >= tmp8) {
                    gConfirmPromptCursorIndex = 0;
                }
            }

            if (tmpA != gConfirmPromptCursorIndex) {
                Audio_PlaySoundEffectById(SFX_MENU_SCROLL);
            }
        }

        if (gConfirmPromptController->buttonPressed & 0x8000) {
            if (temp_a3 == NULL) {
                Audio_PlaySoundEffectById(SFX_MENU_CONFIRM);
            } else {
                switch (temp_a3[gConfirmPromptCursorIndex]) {
                    case 1:
                        Audio_PlaySoundEffectById(SFX_MENU_BACK);
                        break;

                    case 2:
                        Audio_PlaySoundEffectById(SFX_MENU_CONFIRM);
                        break;

                    case 3:
                        Audio_PlaySoundEffectById(0x1D);
                        break;

                    case 4:
                        Audio_PlaySoundEffectById(0x21);
                        break;
                }
            }
            gConfirmPromptState = 3;
        } else if (gConfirmPromptController->buttonPressed & 0x4000) {
            Audio_PlaySoundEffectById(SFX_MENU_BACK);
            gConfirmPromptCursorIndex = -2;
            gConfirmPromptState = 3;
        }
    }
    gConfirmPromptAnimTimer = 4;
}

void Gallery_AdvanceConfirmPromptOpen(void) {
    gConfirmPromptAnimTimer++;
    if (gConfirmPromptAnimTimer >= 5) {
        gConfirmPromptAnimTimer = 4;
        gConfirmPromptState = 2;
    }
}

void Gallery_AdvanceConfirmPromptClose(void) {
    gConfirmPromptAnimTimer--;
    if (gConfirmPromptAnimTimer < 0) {
        gConfirmPromptAnimTimer = 0;
        gConfirmPromptState = 4;
    }
}

s32 Gallery_UpdateConfirmPrompt(void) {
    s32 sp1C = 0;

    switch (gConfirmPromptState) {
        case 2:
            Gallery_UpdateConfirmPromptInput();
            sp1C = 0;
            break;

        case 1:
            Gallery_AdvanceConfirmPromptOpen();
            sp1C = 0;
            break;

        case 3:
            Gallery_AdvanceConfirmPromptClose();
            sp1C = 1;
            break;
    }
    return sp1C;
}

s32 Gallery_OpenConfirmPrompt(s16 arg0, Controller* arg1) {
    if (gConfirmPromptState == 0) {
        Audio_PlaySoundEffectById(SFX_PANEL_OPEN);
        gConfirmPromptId = arg0;
        gConfirmPromptCursorIndex = gConfirmPromptPanels[arg0].unk_09;
        gConfirmPromptAnimTimer = 0;
        gConfirmPromptController = arg1;
        gConfirmPromptState = 1;
        return 1;
    }
    return 0;
}

s32 Gallery_RequestCloseConfirmPrompt(s16 arg0) {
    if ((gConfirmPromptState == 2) && (arg0 == gConfirmPromptId)) {
        gConfirmPromptCursorIndex = -1;
        gConfirmPromptState = 3;
        return 1;
    }
    return 0;
}

s16 Gallery_GetConfirmPromptResult(s16 arg0) {
    if ((gConfirmPromptState == 4) && (arg0 == gConfirmPromptId)) {
        gConfirmPromptState = 0;
        return gConfirmPromptCursorIndex;
    }
    return -1;
}
