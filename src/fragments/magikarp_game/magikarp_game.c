#include "global.h"
#include "src/memory.h"
#include "src/jpeg_stream.h"
#include "src/gfx_buffer.h"
#include "src/stage_loader.h"
#include "src/graphics_textures.h"
#include "src/audio_loop_point.h"
#include "src/fragments/widget_toolkit/widget_toolkit.h"
#include "src/fragments/minigame_shared_graphics/minigame_shared_graphics.h"
#include "src/fragments/magikarp_game/magikarp_game.h"
#include "src/geo_layout.h"
#include "src/display_object_textures.h"
#include "src/model_renderer.h"
#include "src/battle_hud.h"
#include "src/audio_sfx.h"
#include "src/controller.h"
#include "src/input.h"
#include "src/ui_graphics.h"
#include "src/gfx_rect.h"
#include "src/text_system.h"
#include "src/matrix.h"
#include "src/geo_render.h"
#include "src/model_animation.h"

typedef struct MiniHudRandomChance {
    /* 0x00 */ u32* unk_00;
    /* 0x04 */ s16 unk_04;
    /* 0x06 */ s16 unk_06;
    /* 0x08 */ s16 unk_08;
    /* 0x0A */ s16 unk_0A;
} MiniHudRandomChance; // size = 0xC

static MiniHudRandomChance gMiniHudRandomChanceTable[] = {
    { gRaceComNameMusashi, 0x0032, 0x0028, 0x0005, 0x0000 },
    { gRaceComNameSatoshi, 0x004B, 0x0037, 0x0014, 0x0000 },
    { gRaceComNameKasumi, 0x005C, 0x003C, 0x001E, 0x0000 },
    { gRaceComNameShigeru, 0x0063, 0x005A, 0x0028, 0x0000 },
};

static s16 D_86002950[][6] = {
    { 0xFFFF, 0xFFE2, 0xFFFF, 0xFFF5, 0x0000, 0x0007 }, { 0x0000, 0x0019, 0x000D, 0x000D, 0x000B, 0x000C },
    { 0x000F, 0x000F, 0x000D, 0x000E, 0x0011, 0x0011 }, { 0x000F, 0x0010, 0x0013, 0x0013, 0x0011, 0x0012 },
    { 0xA5B3, 0xA5A4, 0xA5AD, 0xA5F3, 0xA5B0, 0xA4CE }, { 0xA4CF, 0xA4CD, 0xA4EB, 0xA4C7, 0xA5DD, 0xA5F3 },
    { 0x0000, 0x0000, 0xA1A1, 0xA1A1, 0xA1A1, 0xA1A1 }, { 0xA4C7, 0xA1A1, 0xA5B8, 0xA5E3, 0xA5F3, 0xA5D7 },
    { 0xA4B7, 0xA4C6, 0xA1A1, 0xA4A6, 0xA4A8, 0xA4CE }, { 0xA5AB, 0xA5A6, 0xA5F3, 0xA5BF, 0xA1BC, 0xA4F2 },
    { 0x0000, 0x0000, 0xA4BF, 0xA4AF, 0xA4B5, 0xA4F3 }, { 0xA1A1, 0xA4BF, 0xA4BF, 0xA4A4, 0xA4BF, 0xA4D2 },
    { 0xA4C8, 0xA4AC, 0xA1A1, 0xA4AB, 0xA4C1, 0xA1A3 }, { 0x0000, 0x0000, 0xA1A1, 0xA1A1, 0xA1A1, 0xA1A1 },
    { 0xA4F2, 0xA1A1, 0xA4CA, 0xA4AC, 0xA4AF, 0xA1A1 }, { 0xA4AA, 0xA4B5, 0xA4CA, 0xA4A4, 0xA4C8, 0xA1A1 },
    { 0xA5AB, 0xA5A6, 0xA5F3, 0xA5BF, 0xA1BC, 0xA4DE }, { 0xA4C7, 0x0000, 0xA4C8, 0xA4C9, 0xA4AB, 0xA4CA },
    { 0xA4A4, 0xA4E8, 0xA1A3, 0x0000, 0xA4C1, 0xA4E3 }, { 0xA4AF, 0xA4C1, 0xA4B9, 0xA4EB, 0xA1A1, 0xA5BF },
    { 0xA5A4, 0xA5DF, 0xA5F3, 0xA5B0, 0xA4CB, 0xA4A2 }, { 0xA4EF, 0xA4BB, 0xA4C6, 0xA1A1, 0xA5C6, 0xA5F3 },
    { 0xA5DD, 0xA4E8, 0xA4AF, 0x0000, 0xA5DC, 0xA5BF }, { 0xA5F3, 0xA4F2, 0xA4AA, 0xA4B9, 0xA4CE, 0xA4AC },
    { 0xA1A1, 0xA4B8, 0xA4E7, 0xA4A6, 0xA4BA, 0xA4CB }, { 0xA5B8, 0xA5E3, 0xA5F3, 0xA5D7, 0xA4B9, 0xA4EB },
    { 0xA5B3, 0xA5C4, 0xA4C0, 0xA1A3, 0x0000, 0x0000 },
};

static f32 gKarpPlayerSpacing = 50.0f;

typedef struct MiniHudTextureRect {
    /* 0x00 */ s16 unk_00;
    /* 0x02 */ s16 unk_02;
    /* 0x04 */ s16 unk_04;
    /* 0x06 */ s16 unk_06;
    /* 0x08 */ s16 unk_08;
    /* 0x0A */ s16 unk_0A;
} MiniHudTextureRect; // size = 0xC

static MiniHudTextureRect gMiniHudTextureRects[4] = {
    { 0x0014, 0x00D7, 0xFFFF, 0xFFFF, 0x0000, 0xFFFF },
    { 0x005A, 0x00D7, 0xFFFF, 0xFFFF, 0x00FF, 0x00FF },
    { 0x00A0, 0x00D7, 0xFFFF, 0xFFFF, 0xFF00, 0x00FF },
    { 0x00E6, 0x00D7, 0xFFFF, 0xFFFF, 0xFFFF, 0x00FF },
};

static AnimFrame gMagikarpTutoAButtonIconAnim[] = {
    {
        0x0303ED80,
        0x000E,
    },
    {
        0x0303F180,
        0x0002,
    },
    {
        0x0303F580,
        0x0006,
    },
    {
        0x0303F180,
        0x0002,
    },
    {
        NULL,
        0x0000,
    },
};

static AnimFrame gMagikarpTutoStartButtonIconAnim[] = {
    {
        0x03040580,
        0x0012,
    },
    {
        0x03040980,
        0x0002,
    },
    {
        0x03040D80,
        0x0008,
    },
    {
        0x03040980,
        0x0002,
    },
    {
        NULL,
        0x0000,
    },
};

static u32 gMiniHudGeoLayout[] = {
    0x0C00FFFF, (u32)&D_5000000,  0x0B00001E,      0x00000000,      0x014000F0,      0x0000000F,
    0x00000000, 0x00000000,       (u32)&D_5000000, 0x0D000000,      (u32)&D_5000000, 0x0F000002,
    0x14000000, 0x002B0012,       0xFFFFFF32,      0x16FFFFFF,      0x0F000003,      (u32)&D_5000000,
    0x0A000000, (u32)&gGraphRoot0, (u32)&D_6000000, (u32)&D_6000000, (u32)&D_6000000, (u32)&D_3000000,
    gFragment2_1GeoLayout, (u32)&D_6000000,  (u32)&D_1000000,
};

static ModelData* gMiniHudModelData;
static ShadowContextNode* gMiniHudMainGraph;
static CameraNode* gMiniHudCamera;
static s16 gMiniHudCamRotationX;
static s16 gMiniHudCamRotationY;
static s16 gMiniHudCamDist;
static u16 gMiniHudFrameCount;
static s16 gMiniHudWinnerCount;
static s32 gMiniHudState;
static s16 gMiniHudAlpha;
static s32 gMiniHudTilemap;

typedef struct MiniHudPlayerGraph {
    /* 0x000 */ DisplayObject unk_000;
    /* 0x168 */ s16 unk_168;
    /* 0x16A */ u16 unk_16A;
    /* 0x16C */ s16 unk_16C;
} MiniHudPlayerGraph; // size = 0x170

typedef struct MagikarpPlayer {
    /* 0x000 */ s16 unk_000;
    /* 0x002 */ char pad2[2];
    /* 0x004 */ MiniHudPlayerGraph unk_004;
    /* 0x174 */ u16 unk_174;
    /* 0x176 */ u16 unk_176[2];
    /* 0x17A */ u16 unk_17A;
    /* 0x17C */ u16 unk_17C[2];
    /* 0x180 */ char pad180[2];
    /* 0x182 */ s16 unk_182;
    /* 0x184 */ s16 unk_184;
    /* 0x186 */ s16 unk_186;
    /* 0x188 */ s16 unk_188;
    /* 0x18A */ s16 unk_18A;
    /* 0x18C */ s16 unk_18C;
    /* 0x18E */ char pad18E[2];
} MagikarpPlayer; // size = 0x190

static MagikarpPlayer gMiniHudPlayerStates[4];
static MiniHudPlayerGraph gMiniHudScoreWheels[4];
static u32 gMiniHudGameTimer;
static MagikarpPlayer* D_86003B5C;
static MiniHudPlayerGraph* gCurrentPlayerGraph;
static MiniHudPlayerGraph* gCurrentPlayerWheel;
static s16 gMiniHudIntroActive;
static s16 gMiniHudIntroTimer;
static s16 gMiniHudActionFlags[4];
static s16 gMiniHudGameTimerTick;
static FontContext* gMiniHudTextObj;
static s32 gMiniHudMaxWheelValue;
static DisplayObject gMiniHudWheelNodes[4];

void MagikarpGame_UpdateAIInputBias(void) {
    s32 i;
    s32 j;

    gMiniHudPlayerStates[0].unk_17A = gPlayer1Controller->buttonPressed;
    gMiniHudPlayerStates[1].unk_17A = gPlayer2Controller->buttonPressed;
    gMiniHudPlayerStates[2].unk_17A = gPlayer3Controller->buttonPressed;
    gMiniHudPlayerStates[3].unk_17A = gPlayer4Controller->buttonPressed;

    gMiniHudPlayerStates[0].unk_174 = gPlayer1Controller->buttonDown;
    gMiniHudPlayerStates[1].unk_174 = gPlayer2Controller->buttonDown;
    gMiniHudPlayerStates[2].unk_174 = gPlayer3Controller->buttonDown;
    gMiniHudPlayerStates[3].unk_174 = gPlayer4Controller->buttonDown;

    for (i = 0; i < 4; i++) {
        D_86003B5C = &gMiniHudPlayerStates[i];

        if (D_86003B5C->unk_000 == 0) {
            s32 temp = gMiniHudState;

            if ((temp != 0) && (temp != 2) && (temp != 3) && (temp != 5)) {
                D_86003B5C->unk_174 = 0;
                D_86003B5C->unk_17A = 0;
            }
        } else if (gMiniHudState == 2) {
            switch (D_86003B5C->unk_004.unk_168) {
                case 0:
                case 12:
                case 13:
                    if (Rand_Range(0x64) < gMiniHudRandomChanceTable[gMinigameComDifficulty].unk_06) {
                        D_86003B5C->unk_17A = 0x8000;
                    }
                    break;

                case 5:
                case 7:
                    if (Rand_Range(0x64) < gMiniHudRandomChanceTable[gMinigameComDifficulty].unk_04) {
                        D_86003B5C->unk_174 = 0x8000;
                    }
                    break;

                case 8:
                    if ((D_86003B5C->unk_004.unk_000.transformAnim.frameFixed > 0x90000) &&
                        (Rand_Range(0x64) < gMiniHudRandomChanceTable[gMinigameComDifficulty].unk_08)) {
                        D_86003B5C->unk_17A = 0x8000;
                    }
                    break;

                case 9:
                    if ((D_86003B5C->unk_004.unk_000.transformAnim.frameFixed > 0xD0000) &&
                        (Rand_Range(0x64) < gMiniHudRandomChanceTable[gMinigameComDifficulty].unk_08)) {
                        D_86003B5C->unk_17A = 0x8000;
                    }
                    break;

                case 10:
                    if ((D_86003B5C->unk_004.unk_000.transformAnim.frameFixed > 0xB0000) &&
                        (Rand_Range(0x64) < gMiniHudRandomChanceTable[gMinigameComDifficulty].unk_08)) {
                        D_86003B5C->unk_17A = 0x8000;
                    }
                    break;

                case 11:
                    if ((D_86003B5C->unk_004.unk_000.transformAnim.frameFixed > 0xF0000) &&
                        (Rand_Range(0x64) < gMiniHudRandomChanceTable[gMinigameComDifficulty].unk_08)) {
                        D_86003B5C->unk_17A = 0x8000;
                    }
                    break;
            }
        }
    }

    if (gMiniHudState == 2) {
        for (j = 0; j < 4; j++) {
            D_86003B5C = &gMiniHudPlayerStates[j];

            for (i = 1; i > 0; i--) {
                D_86003B5C->unk_176[i] = D_86003B5C->unk_176[i - 1];
                D_86003B5C->unk_17C[i] = D_86003B5C->unk_17C[i - 1];
            }
            D_86003B5C->unk_176[0] = D_86003B5C->unk_174;
            D_86003B5C->unk_17C[0] = D_86003B5C->unk_17A;
        }
    } else {
        for (j = 0; j < 4; j++) {
            D_86003B5C = &gMiniHudPlayerStates[j];

            for (i = 0; i < 2; i++) {
                D_86003B5C->unk_176[i] = 0;
                D_86003B5C->unk_17C[i] = 0;
            }
        }
    }
}

s32 MagikarpGame_ReadInputs(void) {
    if (StageContext_GetFadeMode() != 0) {
        return -1;
    }

    Cont_StartReadInputs();
    Cont_ReadInputs();
    Input_ResetRepeatState();

    if (gResultMenuVisible != 1) {
        MagikarpGame_UpdateAIInputBias();
    }

    return 0;
}

void MagikarpGame_InitCamera(void) {
    gMiniHudCamera = gMiniHudMainGraph->node.children;
    MiniCam_SetActive(gMiniHudCamera);
    gMiniHudCamRotationX = 0x600;
    gMiniHudCamRotationY = 0;
    gMiniHudCamDist = 1000;

    gMiniHudCamera->perspective.nearPlane = 100.0f;
    gMiniHudCamera->perspective.farPlane = 12800.0f;
    gMiniHudCamera->perspective.verticalFieldOfView = 10.0f;

    gMiniHudCamera->view.targetPosition.x = 0.0f;
    gMiniHudCamera->view.targetPosition.y = 70.0f;
    gMiniHudCamera->view.targetPosition.z = 0.0f;

    Camera_ComputeEyeFromAngles(&gMiniHudCamera->view.targetPosition, &gMiniHudCamera->view.eyePosition, gMiniHudCamDist, gMiniHudCamRotationX, gMiniHudCamRotationY);
}

void MagikarpGame_UpdateCameraLookAt(void) {
    Camera_ComputeEyeFromAngles(&gMiniHudCamera->view.targetPosition, &gMiniHudCamera->view.eyePosition, gMiniHudCamDist, gMiniHudCamRotationX, gMiniHudCamRotationY);
}

void MagikarpGame_DrawOverlay(s32 arg0) {
    s32 i;
    UNUSED u8 sp68[] = {
        0,
        0,
        30,
    };
    UNUSED u8 sp64[] = {
        0,
        0,
        150,
    };

    switch (arg0) {
        case 0:
            break;

        default:
            gSPDisplayList(gDisplayListHead++, gUiEnvTintTextureDL);
            gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, gMiniHudAlpha);

            for (i = 0; i < 4; i++) {
                MiniHudTextureRect* tmp = &gMiniHudTextureRects[i];

                if (gMiniHudPlayerStates[i].unk_000 == 0) {
                    Widget_DrawPlayerIcon(i, tmp->unk_00, tmp->unk_02, 0.75f);
                } else {
                    Widget_DrawPlayerIcon(-1 - i, tmp->unk_00, tmp->unk_02, 0.75f);
                }
            }

            gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);
            break;
    }

    if (arg0 == 0) {
        Ui_DrawBorderedPanel(0x18, 0x16, 0x110, 0x90, 0x15);
        Ui_DrawBorderedPanel(0x18, 0xA5, 0x110, 0x36, 0x14B);
    }

    Font_BeginTranslucentTextRendering();

    switch (arg0) {
        s32 sp50;
        s32 sp4C;
        char* temp_s0_2;

        case 1:
        case 2:
        case 3:
            break;

        case 0:
            Font_EnableTwoCycleTexturing();
            Font_SetActive(2, 0);
            Font_SetLineHeight(0xC);
            Gfx_SetEnvColor(0xFF, 0xFF, 0xFF, 0xFF);
            Gfx_SetPrimColor(0xF9, 0xF4, 0, 0xFF);
            Font_Printf(0xA0 - (Text_MeasureStringWidth(gMiniHudTextObj, Text_GetString(NULL, 0, gMinigameNamesBank, 0x15)) / 2),
                        0x20, Text_GetString(NULL, 0, gMinigameNamesBank, 0x15));
            Gfx_SetEnvColor(0xFF, 0xFF, 0xFF, 0xFF);
            Gfx_SetPrimColor(0xFF, 0xFF, 0xFF, 0xFF);
            Font_Printf(0x28, 0x34, Text_GetString(NULL, 0, gMinigameNamesBank, 0x16));
            Font_Printf(0x28, 0x58, Text_GetString(NULL, 0, gMinigameNamesBank, 0x17));
            Font_Printf(0x28, 0x7C, Text_GetString(NULL, 0, gMinigameNamesBank, 0x18));
            Gfx_SetEnvColor(0xC8, 0xC8, 0xFF, 0xFF);
            Gfx_SetPrimColor(0x50, 0x50, 0xFF, 0xFF);

            temp_s0_2 = Text_GetString(NULL, 0, gMinigameNamesBank, 0x5F);

            Widget_MeasureLabelBox(&sp50, &sp4C, Text_GetString(NULL, 0, gMinigameNamesBank, 0x16), temp_s0_2, 0xC);
            if (sp50 >= 0) {
                Font_Printf(sp50 + 0x28, sp4C + 0x34, temp_s0_2);
            }

            Widget_MeasureLabelBox(&sp50, &sp4C, Text_GetString(NULL, 0, gMinigameNamesBank, 0x17), temp_s0_2, 0xC);
            if (sp50 >= 0) {
                Font_Printf(sp50 + 0x28, sp4C + 0x58, temp_s0_2);
            }

            Widget_MeasureLabelBox(&sp50, &sp4C, Text_GetString(NULL, 0, gMinigameNamesBank, 0x18), temp_s0_2, 0xC);
            if (sp50 >= 0) {
                Font_Printf(sp50 + 0x28, sp4C + 0x7C, temp_s0_2);
            }

            Gfx_SetEnvColor(0xC8, 0xC8, 0xFF, 0xFF);
            Gfx_SetPrimColor(0x50, 0x50, 0xFF, 0xFF);
            Font_Printf(0xA2, 0xB2, Text_GetString(NULL, 0, gMinigameNamesBank, 0x19));
            Gfx_SetEnvColor(0xFF, 0xFF, 0xFF, 0xFF);
            Gfx_SetPrimColor(0xFF, 0x80, 0x80, 0xFF);
            Font_Printf(0xA2, 0xC8, Text_GetString(NULL, 0, gMinigameNamesBank, 0x14));
            Gfx_SetEnvColor(0xFF, 0xFF, 0xFF, 0xFF);
            Gfx_SetPrimColor(0xFF, 0xFF, 0xFF, 0xFF);
            Font_Printf(0x8B, 0xB2, Text_GetString(NULL, 0, gMinigameNamesBank, 0x5C));
            Font_Printf(0x8B, 0xC8, Text_GetString(NULL, 0, gMinigameNamesBank, 0x5C));
            Font_DisableTwoCycleTexturing();
            break;

        case 4:
            Font_EnableTwoCycleTexturing();
            Font_SetActive(0x20, -2);
            Gfx_SetEnvColor(0xFF, 0xFF, 0xFF, 0xFF);
            Gfx_SetPrimColor(0x1E, 0x1E, 0xFF, 0xFF);
            Font_Printf(0xA0 - (Text_MeasureStringWidth(gMiniHudTextObj, Text_GetString(NULL, 0, gMinigameNamesBank, 0x5A)) / 2),
                        0x64, Text_GetString(NULL, 0, gMinigameNamesBank, 0x5A));
            Font_DisableTwoCycleTexturing();
            break;
    }

    Font_EndTexturedTextRendering();

    if (arg0 == 0) {
        gSPDisplayList(gDisplayListHead++, gUiEnvTintTextureDL);
        Widget_DrawAnimIcon16(0x74, 0xB0, gMagikarpTutoAButtonIconAnim);
        Widget_DrawAnimIcon16C(0x74, 0xC6, gMagikarpTutoStartButtonIconAnim);
        Widget_DrawSplitBanner(2, 0x19, 0xA6);
    }
}

static Gfx gScoreMarkerDL_End[] = {
    gsSPEndDisplayList(),
};

static u32 gScoreMarkerTexData[] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x000A000F, 0x00140014, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x0000000A, 0x00190021, 0x002A0033, 0x0036003B, 0x00000000, 0x00000000,
    0x00000000, 0x0000000A, 0x0019002A, 0x003B004B, 0x00550060, 0x00640067, 0x00000000, 0x00000000, 0x00000000,
    0x00140025, 0x003B0052, 0x00670076, 0x0082008B, 0x00900094, 0x00000000, 0x00000000, 0x00000014, 0x002F0046,
    0x00600079, 0x008D009F, 0x00A900B1, 0x00B600B9, 0x00000000, 0x00000000, 0x00140033, 0x0052006D, 0x0088009F,
    0x00B100C1, 0x00CA00D1, 0x00D400D6, 0x00000000, 0x00000014, 0x002F0052, 0x0071008D, 0x00A900BE, 0x00CF00D9,
    0x00E200E6, 0x00EA00EA, 0x00000000, 0x000A0025, 0x0046006D, 0x008D00AD, 0x00C400D6, 0x00E300EC, 0x00F100F4,
    0x00F600F7, 0x00000000, 0x0019003B, 0x00600088, 0x00A900C4, 0x00D900E6, 0x00F000F6, 0x00F900FB, 0x00FC00FC,
    0x0000000A, 0x002A0052, 0x0079009F, 0x00BE00D6, 0x00E600F2, 0x00F800FC, 0x00FD00FE, 0x00FE00FE, 0x00000019,
    0x003B0067, 0x008D00B1, 0x00CF00E3, 0x00F000F8, 0x00FC00FE, 0x00FE00FF, 0x00FF00FF, 0x00000021, 0x004B0076,
    0x009F00C1, 0x00D900EC, 0x00F600FC, 0x00FE00FE, 0x00FF00FF, 0x00FF00FF, 0x000A002A, 0x00550082, 0x00A900CA,
    0x00E200F1, 0x00F900FD, 0x00FE00FF, 0x00FF00FF, 0x00FF00FF, 0x000F0033, 0x0060008B, 0x00B100D1, 0x00E600F4,
    0x00FB00FE, 0x00FF00FF, 0x00FF00FF, 0x00FF00FF, 0x00140036, 0x00640090, 0x00B600D4, 0x00EA00F6, 0x00FC00FE,
    0x00FF00FF, 0x00FF00FF, 0x00FF00FF, 0x0014003B, 0x00670094, 0x00B900D6, 0x00EA00F7, 0x00FC00FE, 0x00FF00FF,
    0x00FF00FF, 0x00FF00FF,
};

static Vtx gScoreMarkerVerts[] = {
    VTX(-25, 0, -25, 0, 0, 0x00, 0x00, 0x00, 0xFF),
    VTX(-25, 0, 25, 0, 1024, 0x00, 0x00, 0x00, 0xFF),
    VTX(25, 0, 25, 1024, 1024, 0x00, 0x00, 0x00, 0xFF),
    VTX(25, 0, -25, 1024, 0, 0x00, 0x00, 0x00, 0xFF),
};

static Gfx gScoreMarkerDL[] = {
    gsDPPipeSync(),
    gsDPSetCycleType(G_CYC_2CYCLE),
    gsDPSetRenderMode(G_RM_ZB_XLU_SURF, G_RM_ZB_XLU_SURF2),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsSPClearGeometryMode(G_CULL_BACK | G_LIGHTING),
    gsSPSetGeometryMode(G_ZBUFFER),
    gsDPSetCombineMode(G_CC_MODULATEIA_PRIM, G_CC_PASS2),
    gsDPSetPrimColor(0, 0, 0, 0, 0, 200),
    gsDPSetTextureLUT(G_TT_NONE),
    gsDPLoadTextureBlock(gScoreMarkerTexData, G_IM_FMT_IA, G_IM_SIZ_16b, 16, 16, 0, G_TX_MIRROR | G_TX_WRAP,
                         G_TX_MIRROR | G_TX_WRAP, 4, 4, G_TX_NOLOD, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsSPVertex(gScoreMarkerVerts, 4, 0),
    gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsSPEndDisplayList(),
};

void func_86000C10(void) {
}

void MagikarpGame_DrawScoreMarkers(void) {
    Gfx* gfx = gDisplayListHead;
    Mtx* var_s1 = (Mtx*)Gfx_AllocDisplayList(sizeof(MtxF) * 4);
    Mtx sp100;
    Mtx spC0;
    UNUSED Mtx sp80;
    f32 temp_f20;
    s32 i;

    for (i = 0; i < 4; i++) {
        gCurrentPlayerGraph = &gMiniHudPlayerStates[i].unk_004;
        temp_f20 = (410.0f - gCurrentPlayerGraph->unk_000.anchors[0].position.y) / 510.0f;
        guTranslate(&sp100, gCurrentPlayerGraph->unk_000.anchors[0].position.x, 0.0f, 0.0f);
        guScale(&spC0, temp_f20, temp_f20, temp_f20);
        guMtxCatL(&spC0, &sp100, var_s1);

        gSPMatrix(gfx++, var_s1, G_MTX_PUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(gfx++, gScoreMarkerDL);
        gSPPopMatrix(gfx++, G_MTX_MODELVIEW);
        var_s1++;
    }

    gDisplayListHead = gfx;
}

void MagikarpGame_DrawFrame(s32 arg0) {
    BgStage_DrawFrame();
    GfxImage_ClearDepthRectangle(&gDisplayListHead, 0, 0, 0x140, 0xF0);
    Gfx_DrawTilemap(gMiniHudTilemap);

    if (gResultMenuVisible != 1) {
        GeoRender_AdvanceFrameCounter();
    }

    Widget_CountdownUpdate();
    Geo_RenderRootNode(gMiniHudMainGraph);
    gDisplayListHead = MiniCam_ApplyProjection(gDisplayListHead);
    MagikarpGame_DrawScoreMarkers();

    if (gResultConfirmPressed == 0) {
        MagikarpGame_DrawOverlay(arg0);
    }

    Widget_PauseMenuUpdate();
    BgStage_AdvanceFrame();

    gMiniHudFrameCount++;
}

void MagikarpGame_UpdateSplashRipple(void) {
    s32 i;

    for (i = 0; i < 4; i++) {
        gCurrentPlayerGraph = gCurrentPlayerWheel = &gMiniHudScoreWheels[i];

        if (gCurrentPlayerGraph->unk_000.position.y > 140.0f) {
            gCurrentPlayerGraph->unk_000.position.y -= 2.0f;
        }

        switch (gCurrentPlayerWheel->unk_16C) {
            case 0:
                if (gCurrentPlayerGraph->unk_000.rotation.y < 0x800) {
                    gCurrentPlayerGraph->unk_000.rotation.y += 0x400;
                } else {
                    gCurrentPlayerWheel->unk_16C = 1;
                }
                break;

            case 1:
                if (gCurrentPlayerGraph->unk_000.rotation.y > 0) {
                    gCurrentPlayerGraph->unk_000.rotation.y -= 0x200;
                } else {
                    gCurrentPlayerWheel->unk_16C = 2;
                }
                break;

            case 2:
            default:
                gCurrentPlayerGraph->unk_000.rotation.y = 0;
                break;
        }
    }
}

s32 MagikarpGame_WaitForStart(void) {
    s32 var_s0_2;
    s32 i;

    StageLoader_RunFrames(0xA);
    StageFade_StartFromOpaque(0x10);

    for (i = 0; i < 4; i++) {
        D_86003B5C = &gMiniHudPlayerStates[i];
        gCurrentPlayerGraph = &D_86003B5C->unk_004;
        ModelAnim_SetAnimation(&gCurrentPlayerGraph->unk_000, 0);
        gCurrentPlayerGraph->unk_000.transformAnim.speedFixed = 0x10000;
        gCurrentPlayerGraph->unk_000.transformAnim.frameFixed = (Rand_Range(0x14) << 0x10);
        gMiniHudActionFlags[i] = 0;
    }

    for (i = 0; i < 4; i++) {
        D_86003B5C = &gMiniHudPlayerStates[i];
        D_86003B5C->unk_000 = (gMiniHudColorDefaults[i] == 0) ? 0 : gMinigameComDifficulty + 1;
    }

    gMiniHudState = 0;

    var_s0_2 = true;
    while (var_s0_2 == true) {
        if (MagikarpGame_ReadInputs() == 0) {
            if (gPlayer1Controller->buttonPressed & 0x1000) {
                var_s0_2 = false;
            }

            if ((gMenuCursorX == 0) && (gPlayer1Controller->buttonPressed & 0x4000)) {
                Audio_PlaySoundEffectById(SFX_MENU_BACK);
                Widget_PauseMenuTrigger(2);
                return -1;
            }
        }

        MagikarpGame_DrawFrame(gMiniHudState);
    }

    return 0;
}

void MagikarpGame_Countdown(void) {
    s32 i;

    for (i = 0; i < 4; i++) {
        D_86003B5C = &gMiniHudPlayerStates[i];
        gCurrentPlayerGraph = &D_86003B5C->unk_004;
        ModelAnim_SetAnimation(&gCurrentPlayerGraph->unk_000, 0);
        gCurrentPlayerGraph->unk_000.transformAnim.speedFixed = 0x10000;
        gCurrentPlayerGraph->unk_000.transformAnim.frameFixed = Rand_Range(0x14) << 0x10;
        gMiniHudActionFlags[i] = 0;
    }

    gMiniHudIntroActive = 1;
    gMiniHudIntroTimer = 90;
    Widget_CountdownStart(1);
    gMiniHudState = 1;

    while (gMiniHudIntroActive == 1) {
        switch (Widget_CountdownGetState()) {
            case 1:
            case 2:
            case 3:
                MiniSound_DispatchCommand(0x20001, 0, 0);
                break;

            case 0:
                gMiniHudIntroActive = 0;
                break;
        }

        MagikarpGame_UpdateSplashRipple();
        MagikarpGame_ReadInputs();
        MagikarpGame_DrawFrame(gMiniHudState);
    }
}

void MagikarpGame_StartTrickPose(s16 arg0) {
    switch (arg0) {
        case 0:
        case 1:
            MiniSound_DispatchCommand(0x20006, D_86003B5C->unk_18A + 1, 4);
            break;

        case 2:
            MiniSound_DispatchCommand(0x20005, D_86003B5C->unk_18A + 1, 4);
            break;

        default:
            MiniSound_DispatchCommand(0x20004, D_86003B5C->unk_18A + 1, 4);
            break;
    }

    D_86003B5C->unk_004.unk_168 = 5;
    D_86003B5C->unk_004.unk_16A = arg0;

    ModelAnim_SetAnimation(&gCurrentPlayerGraph->unk_000, 8);

    gCurrentPlayerGraph->unk_000.transformAnim.frameFixed = 0;
    gCurrentPlayerGraph->unk_000.transformAnim.speedFixed = 0x10000;
}

void MagikarpGame_AdvanceTrickPoseB(void) {
    D_86003B5C->unk_004.unk_168 = 6;
    ModelAnim_SetAnimation(&gCurrentPlayerGraph->unk_000, 9);
    gCurrentPlayerGraph->unk_000.transformAnim.frameFixed = 0;
    gCurrentPlayerGraph->unk_000.transformAnim.speedFixed = 0x10000;
}

void MagikarpGame_AdvanceTrickPoseC(void) {
    D_86003B5C->unk_004.unk_168 = 7;
    ModelAnim_SetAnimation(&gCurrentPlayerGraph->unk_000, 0xA);
    gCurrentPlayerGraph->unk_000.transformAnim.frameFixed = 0;
    gCurrentPlayerGraph->unk_000.transformAnim.speedFixed = 0x10000;
}

void MagikarpGame_AdvanceTrickPoseD(void) {
    D_86003B5C->unk_004.unk_168 = 8;
    ModelAnim_SetAnimation(&gCurrentPlayerGraph->unk_000, 0xB);
    gCurrentPlayerGraph->unk_000.transformAnim.frameFixed = 0;
    gCurrentPlayerGraph->unk_000.transformAnim.speedFixed = 0x10000;
}

void MagikarpGame_AdvanceTrickPoseE(void) {
    D_86003B5C->unk_004.unk_168 = 9;
    ModelAnim_SetAnimation(&gCurrentPlayerGraph->unk_000, 0xC);
    gCurrentPlayerGraph->unk_000.transformAnim.frameFixed = 0;
    gCurrentPlayerGraph->unk_000.transformAnim.speedFixed = 0x10000;
}

void MagikarpGame_AdvanceTrickPoseF(void) {
    D_86003B5C->unk_004.unk_168 = 0xA;
    ModelAnim_SetAnimation(&gCurrentPlayerGraph->unk_000, 0xD);
    gCurrentPlayerGraph->unk_000.transformAnim.frameFixed = 0;
    gCurrentPlayerGraph->unk_000.transformAnim.speedFixed = 0x10000;
}

void MagikarpGame_AdvanceTrickPoseG(void) {
    D_86003B5C->unk_004.unk_168 = 0xB;
    ModelAnim_SetAnimation(&gCurrentPlayerGraph->unk_000, 0xE);
    gCurrentPlayerGraph->unk_000.transformAnim.frameFixed = 0;
    gCurrentPlayerGraph->unk_000.transformAnim.speedFixed = 0x10000;
}

void MagikarpGame_FinishTrickOrChain(s32 arg0) {
    s32 i;

    gMiniHudActionFlags[arg0] = 0;

    for (i = 0; i < 2; i++) {
        if (D_86003B5C->unk_17C[i] & 0x8000) {
            MagikarpGame_StartTrickPose(i);
            return;
        }
    }

    D_86003B5C->unk_004.unk_168 = 0xC;
    ModelAnim_SetAnimation(&gCurrentPlayerGraph->unk_000, 5);
    gCurrentPlayerGraph->unk_000.transformAnim.frameFixed = 0;
    gCurrentPlayerGraph->unk_000.transformAnim.speedFixed = 0x10000;
}

void MagikarpGame_LandTrick(s32 arg0) {
    gMiniHudActionFlags[arg0] = 0;
    D_86003B5C->unk_004.unk_168 = 0xD;
    ModelAnim_SetAnimation(&gCurrentPlayerGraph->unk_000, 6);
    gCurrentPlayerGraph->unk_000.transformAnim.frameFixed = 0;
    gCurrentPlayerGraph->unk_000.transformAnim.speedFixed = 0x10000;
}

void MagikarpGame_ResetPlayerState(s32 arg0) {
    gMiniHudActionFlags[arg0] = 0;
    D_86003B5C->unk_004.unk_168 = 0;
    ModelAnim_SetAnimation(&gCurrentPlayerGraph->unk_000, 0);
    gCurrentPlayerGraph->unk_000.transformAnim.frameFixed = 0;
    gCurrentPlayerGraph->unk_000.transformAnim.speedFixed = 0x10000;
}

void MagikarpGame_UpdatePlayerState(s32 arg0) {
    D_86003B5C = &gMiniHudPlayerStates[arg0];
    gCurrentPlayerWheel = &gMiniHudScoreWheels[arg0];
    gCurrentPlayerGraph = &D_86003B5C->unk_004;

    if (gResultMenuVisible != 1) {
        switch (D_86003B5C->unk_004.unk_168) {
            case 12:
                if (D_86003B5C->unk_17C[1] & 0x8000) {
                    MagikarpGame_StartTrickPose(2);
                } else if (ModelAnim_IsFinished(&gCurrentPlayerGraph->unk_000)) {
                    MagikarpGame_LandTrick(arg0);
                }
                break;

            case 13:
                if (D_86003B5C->unk_17C[1] & 0x8000) {
                    MagikarpGame_StartTrickPose(3);
                } else if (ModelAnim_IsFinished(&gCurrentPlayerGraph->unk_000)) {
                    MagikarpGame_ResetPlayerState(arg0);
                }
                break;

            case 0:
                if (D_86003B5C->unk_17C[1] & 0x8000) {
                    if (D_86003B5C->unk_18C == 1) {
                        MagikarpGame_StartTrickPose(2);
                        D_86003B5C->unk_18C = 0;
                    } else {
                        MagikarpGame_StartTrickPose(4);
                    }
                }
                break;

            case 5:
                if (ModelAnim_IsFinished(&gCurrentPlayerGraph->unk_000)) {
                    if (D_86003B5C->unk_176[1] & 0x8000) {
                        MagikarpGame_AdvanceTrickPoseC();
                    } else {
                        MagikarpGame_AdvanceTrickPoseB();
                    }
                }
                break;

            case 7:
                if (ModelAnim_IsFinished(&gCurrentPlayerGraph->unk_000)) {
                    if (D_86003B5C->unk_176[1] & 0x8000) {
                        s16 tmp = D_86003B5C->unk_004.unk_16A;

                        if (tmp < 2) {
                            MagikarpGame_AdvanceTrickPoseF();
                        } else if (tmp == 2) {
                            MagikarpGame_AdvanceTrickPoseE();
                        } else {
                            MagikarpGame_AdvanceTrickPoseG();
                        }
                    } else {
                        MagikarpGame_AdvanceTrickPoseD();
                    }
                }
                break;

            case 6:
            case 8:
            case 9:
            case 10:
            case 11:
                if (ModelAnim_IsFinished(&gCurrentPlayerGraph->unk_000)) {
                    MagikarpGame_FinishTrickOrChain(arg0);
                }
                break;

            case 1:
                if (ModelAnim_IsFinished(&gCurrentPlayerGraph->unk_000)) {
                    ModelAnim_SetAnimation(&gCurrentPlayerGraph->unk_000, 7);
                    gCurrentPlayerGraph->unk_000.transformAnim.frameFixed = 0;
                    gCurrentPlayerGraph->unk_000.transformAnim.speedFixed = 0x10000;
                }
                break;

            default:
                if (ModelAnim_IsFinished(&gCurrentPlayerGraph->unk_000)) {
                    ModelAnim_SetAnimation(&gCurrentPlayerGraph->unk_000, 0);
                    gCurrentPlayerGraph->unk_000.transformAnim.frameFixed = 0;
                    gCurrentPlayerGraph->unk_000.transformAnim.speedFixed = 0x10000;
                    gMiniHudActionFlags[arg0] = 0;
                }
                break;
        }

        if (((D_86003B5C->unk_004.unk_168 == 9) && ((gCurrentPlayerGraph->unk_000.transformAnim.frameFixed >> 0x10) == 2)) ||
            ((D_86003B5C->unk_004.unk_168 == 10) && ((gCurrentPlayerGraph->unk_000.transformAnim.frameFixed >> 0x10) == 2)) ||
            ((D_86003B5C->unk_004.unk_168 == 11) && ((gCurrentPlayerGraph->unk_000.transformAnim.frameFixed >> 0x10) == 2))) {
            if (gMiniHudActionFlags[arg0] == 0) {
                gCurrentPlayerWheel->unk_16A++;
                gMiniHudActionFlags[arg0] = 1;
                ModelAnim_SetAnimation(&gCurrentPlayerWheel->unk_000, 0);
                gCurrentPlayerWheel->unk_000.transformAnim.speedFixed = 0x10000;
                gCurrentPlayerWheel->unk_000.transformAnim.frameFixed = 0;
                MiniSound_DispatchCommand(0x20008, arg0, 4);
            }
        }

        if (gResultMenuVisible != 1) {
            BattleHud_ApproachDigitRotations(arg0, gCurrentPlayerWheel->unk_16A);
        }

        if (D_86003B5C->unk_188 > 0) {
            D_86003B5C->unk_188--;
            if (D_86003B5C->unk_188 == 0) {
                Model_SetMaterialColor(&D_86003B5C->unk_004.unk_000, 0xFF, 0xFF, 0xFF, 0);
            }
        }
    }
}

void MagikarpGame_Play(void) {
    s32 i;
    s32 temp_lo;
    s32 temp_v1;
    s32 var_s6 = 1;

    MiniSound_DispatchCommand(0x20002, 0, 0);

    for (i = 0; i < 4; i++) {
        D_86003B5C = &gMiniHudPlayerStates[i];
        gCurrentPlayerGraph = &D_86003B5C->unk_004;

        D_86003B5C->unk_004.unk_168 = 0;
        ModelAnim_SetAnimation(&gCurrentPlayerGraph->unk_000, 0);
        gCurrentPlayerGraph->unk_000.transformAnim.speedFixed = 0x10000;

        gMiniHudActionFlags[i] = 0;

        D_86003B5C->unk_18C = 1;
    }

    gMiniHudGameTimer = 0;
    gMiniHudGameTimerTick = 0;
    gMiniHudState = 2;

    while (var_s6 != 0) {
        MagikarpGame_ReadInputs();
        MagikarpGame_UpdateCameraLookAt();

        if (gMiniHudGameTimer == 0xF) {
            Audio_StartMusicTrack(10);
        }

        if (gResultMenuVisible != 1) {
            for (i = 0; i < 4; i++) {
                MagikarpGame_UpdatePlayerState(i);
            }

            MagikarpGame_UpdateSplashRipple();

            gMiniHudGameTimer++;
            if (gMiniHudGameTimer >= 0x258) {
                MiniSound_DispatchCommand(0x20009, 0, 0);
                var_s6 = 0;
            } else {
                temp_v1 = 0x258 - gMiniHudGameTimer;
                if ((temp_v1 % 30) == 0) {
                    temp_lo = temp_v1 / 30;
                    if ((temp_lo < 6) && (temp_lo > 0)) {
                        MiniSound_DispatchCommand(0x2000A, 0, 0);
                        if (1) {}
                        if (1) {}
                        if (1) {}
                    }
                }
            }
        }

        if (gResultFlag_Special != 0) {
            var_s6 = 0;
        }

        MagikarpGame_DrawFrame(gMiniHudState);
    }

    Audio_FadeOutAll(0xA);
}

s32 MagikarpGame_AllPlayersFinishedJump(void) {
    s32 var_v0 = 0;
    s32 i;

    for (i = 0; i < 4; i++) {
        D_86003B5C = &gMiniHudPlayerStates[i];
        switch (D_86003B5C->unk_004.unk_168) {
            case 5:
            case 6:
            case 7:
            case 8:
            case 9:
            case 10:
            case 11:
                break;

            default:
                var_v0++;
                break;
        }
    }

    if (var_v0 == 4) {
        return 1;
    }

    return 0;
}

void MagikarpGame_ShowWinnerSequence(void) {
    s32 i;
    s32 j;
    s32 var_s1;
    s32 var_s5;

    var_s5 = 1;
    gMiniHudState = 3;
    var_s1 = 1;

    while (var_s1 != 0) {
        MagikarpGame_ReadInputs();
        MagikarpGame_UpdateCameraLookAt();

        for (i = 0; i < 4; i++) {
            MagikarpGame_UpdatePlayerState(i);
        }

        if (MagikarpGame_AllPlayersFinishedJump() != 0) {
            var_s1 = 0;
        }

        MagikarpGame_DrawFrame(gMiniHudState);

        if (gResultFlag_Special != 0) {
            return;
        }
    }

    gMiniHudMaxWheelValue = 0;

    for (i = 0; i < 4; i++) {
        if (gMiniHudScoreWheels[i].unk_16A >= gMiniHudMaxWheelValue) {
            gMiniHudMaxWheelValue = gMiniHudScoreWheels[i].unk_16A;
        }
    }

    gMiniHudWinnerCount = 0;

    for (i = 0; i < 4; i++) {
        if ((gMiniHudMaxWheelValue == gMiniHudScoreWheels[i].unk_16A) && (gMiniHudMaxWheelValue != 0)) {
            Widget_PauseMenuRecordWin(i);
            gMiniHudWinnerCount++;
        }
    }

    for (i = 0; i < 4; i++) {
        D_86003B5C = &gMiniHudPlayerStates[i];
        gCurrentPlayerGraph = &D_86003B5C->unk_004;
        if ((gMiniHudMaxWheelValue == gMiniHudScoreWheels[i].unk_16A) && (gMiniHudMaxWheelValue > 0)) {
            D_86003B5C->unk_004.unk_168 = 1;
            ModelAnim_SetAnimation(&gCurrentPlayerGraph->unk_000, 7);
            gCurrentPlayerGraph->unk_000.transformAnim.frameFixed = 0;
        } else {
            D_86003B5C->unk_004.unk_168 = 2;
            ModelAnim_SetAnimation(&gCurrentPlayerGraph->unk_000, 0);
            gCurrentPlayerGraph->unk_000.transformAnim.frameFixed = Rand_Range(5) << 0x10;
        }
        gCurrentPlayerGraph->unk_000.transformAnim.speedFixed = 0x10000;
    }

    j = 30;
    while (j > 0) {
        MagikarpGame_ReadInputs();
        MagikarpGame_UpdateCameraLookAt();

        for (i = 0; i < 4; i++) {
            MagikarpGame_UpdatePlayerState(i);
        }

        MagikarpGame_DrawFrame(gMiniHudState);

        if (gResultFlag_Special != 0) {
            return;
        }

        if (gResultMenuVisible != 1) {
            j -= 1;
        }
    }

    Widget_PauseMenuTrigger(1);

    while (var_s5 != 0) {
        MagikarpGame_ReadInputs();
        MagikarpGame_UpdateCameraLookAt();

        for (i = 0; i < 4; i++) {
            MagikarpGame_UpdatePlayerState(i);
        }

        for (i = 0; i < 4; i++) {
            D_86003B5C = &gMiniHudPlayerStates[i];
            if (D_86003B5C->unk_17A & 0x8000) {
                var_s5 = 0;
            }
        }

        MagikarpGame_DrawFrame(gMiniHudState);

        if (gResultFlag_Special != 0) {
            return;
        }
    }
}

void MagikarpGame_FadeOutLoop(void) {
    s32 i;
    s32 j;

    Audio_FadeOutAll(30);
    StageFade_StartFromTransparent(30);

    for (i = 0; i < 30; i++) {
        MagikarpGame_ReadInputs();

        for (j = 0; j < 4; j++) {
            MagikarpGame_UpdatePlayerState(j);
        }

        MagikarpGame_DrawFrame(gMiniHudState);
    }
}

void MagikarpGame_ShowResultDelay(s32 arg0) {
    s32 i;

    gMiniHudState = 6;

    for (i = 0; i < arg0; i++) {
        MagikarpGame_ReadInputs();
        MagikarpGame_DrawFrame(gMiniHudState);
    }
}

void MagikarpGame_WaitForExit(void) {
    s32 i;

    gMiniHudState = 5;

    while (gResultExitPending == 0) {
        if ((gMenuCursorX == 1) && (gMiniHudAlpha > 0)) {
            gMiniHudAlpha -= 10;
            if (gMiniHudAlpha < 0) {
                gMiniHudAlpha = 0;
            }
        }

        for (i = 0; i < 4; i++) {
            MagikarpGame_UpdatePlayerState(i);
        }

        MagikarpGame_ReadInputs();
        MagikarpGame_DrawFrame(gMiniHudState);
    }
}

void MagikarpGame_InitActors(void) {
    s32 i;
    MemoryBlock* temp_v0;

    temp_v0 = (MemoryBlock*)MainPool_AllocState(main_pool_get_available(), 0);
    gMiniHudMainGraph = (ShadowContextNode*)process_geo_layout(temp_v0, gMiniHudGeoLayout);
    MainPool_FinalizeAllocation(temp_v0);
    ModelRenderer_InitDisplayRoots();
    PokeIcon_OpenModelArchives();
    gMiniHudAlpha = 0xFF;
    gMiniHudModelData = (ModelData*)Model_LoadByArchiveIndex(0xAD);

    for (i = 0; i < 4; i++) {
        D_86003B5C = &gMiniHudPlayerStates[i];
        gCurrentPlayerGraph = &D_86003B5C->unk_004;

        ModelRenderer_AttachDisplayObject(&gCurrentPlayerGraph->unk_000);
        Model_InitDisplayObject(&gCurrentPlayerGraph->unk_000, 0, 0xAD, gMiniHudModelData->unk_08->unk_00[0]);

        gCurrentPlayerGraph->unk_000.position.x = (i - 1.5f) * gKarpPlayerSpacing;

        gCurrentPlayerGraph->unk_000.position.y = 0.0f;
        gCurrentPlayerGraph->unk_000.position.z = 0.0f;
        gCurrentPlayerGraph->unk_000.scale.x = 0.8f;
        gCurrentPlayerGraph->unk_000.scale.y = 0.8f;
        gCurrentPlayerGraph->unk_000.scale.z = 0.8f;

        D_86003B5C->unk_182 = 0;
        D_86003B5C->unk_184 = 0;
        D_86003B5C->unk_186 = 0;
        D_86003B5C->unk_188 = 0;
        D_86003B5C->unk_18A = i;

        if (i == 0) {
            D_86003B5C->unk_000 = 0;
        } else {
            D_86003B5C->unk_000 = gMinigameComDifficulty + 1;
        }

        ModelAnim_SetAnimation(&gCurrentPlayerGraph->unk_000, 0);

        gCurrentPlayerGraph->unk_000.transformAnim.frameFixed = Rand_Range(5) << 0x10;
        gCurrentPlayerGraph->unk_000.rotation.y = -0x2800;
    }

    gMiniHudModelData = (ModelData*)Model_LoadByArchiveIndex(0xAE);

    for (i = 0; i < 4; i++) {
        gCurrentPlayerWheel = &gMiniHudScoreWheels[i];
        gCurrentPlayerGraph = gCurrentPlayerWheel;

        ModelRenderer_AttachDisplayObject(&gCurrentPlayerGraph->unk_000);
        Model_InitDisplayObject(&gCurrentPlayerGraph->unk_000, 0, 0xAE, gMiniHudModelData->unk_08->unk_00[0]);
        gCurrentPlayerGraph->unk_000.position.x = (i - 1.5f) * gKarpPlayerSpacing;
        gCurrentPlayerGraph->unk_000.position.z = 0.0f;
        gCurrentPlayerGraph->unk_000.position.y = 200.0f;
        gCurrentPlayerGraph->unk_000.rotation.y = -0x8000;
        gCurrentPlayerGraph->unk_000.scale.x = 1.4f;
        gCurrentPlayerGraph->unk_000.scale.y = 1.4f;
        gCurrentPlayerGraph->unk_000.scale.z = 1.4f;
        gCurrentPlayerWheel->unk_168 = 0;
        gCurrentPlayerWheel->unk_16A = 0;
        gCurrentPlayerWheel->unk_16C = 0;
        BattleHud_SetDigitRotationTargets(i, 0);
        gCurrentPlayerWheel->unk_000.effectSlot = i;
        ModelAnim_SetAnimation(&gCurrentPlayerGraph->unk_000, 0);
        ModelAnim_ClearTransformChannel(&gCurrentPlayerGraph->unk_000);
    }

    gMiniHudTilemap = Asset_LoadToSegment(0, (void*)fragment6_misc_ROM_START, (void*)fragment7_ROM_START, 0);
    MagikarpGame_InitCamera();
    Widget_CountdownInit();
}

void MagikarpGame_Main(UNUSED s32 arg0, UNUSED s32 arg1) {
    StageContext* sp24;

    main_pool_push_state('MINI');

    Gfx_InitDisplayListBuffers(0x20000, 0);
    sp24 = (StageContext*)StageContext_Allocate(0, 1, 3, 1, 2, 1);
    gMiniHudTextObj = Font_Init(0x36, 0);

    ASSET_LOAD(D_1000000, common_menu1_ui, 0);
    FRAGMENT_LOAD(FRAGMENT_PARTICLE_DATA_LIBRARY);

    Fragment_Load((((u32)D_8D000000 & 0x0FF00000) >> 0x14) - 0x10, _5C7A70_ROM_START, _5C7A70_ROM_END);

    MagikarpGame_InitActors();
    Widget_PauseMenuInit();
    StageContext_Activate(sp24);

    if (MagikarpGame_WaitForStart() != -1) {
        Audio_FadeOutAll(0x3C);
        MagikarpGame_ShowResultDelay(5);
        MagikarpGame_Countdown();
        MagikarpGame_Play();

        if (gResultFlag_Special == 0) {
            MagikarpGame_ShowWinnerSequence();
        }

        if (gResultFlag_Special == 0) {
            MagikarpGame_WaitForExit();
        }
    }

    MagikarpGame_FadeOutLoop();
    StageContext_Deactivate();
    Font_Free();
    Gfx_FreeDisplayListBuffers();

    main_pool_pop_state('MINI');

    Widget_PauseMenuGetResult();
}
