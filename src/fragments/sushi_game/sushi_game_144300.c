#include "sushi_game.h"
#include "src/geo_render.h"
#include "src/geo_layout.h"
#include "src/display_object_textures.h"
#include "src/model_renderer.h"
#include "src/graphics_textures.h"
#include "src/input.h"
#include "src/ui_graphics.h"
#include "src/text_system.h"
#include "src/jpeg_stream.h"
#include "src/audio_sound_state.h"
#include "src/audio_sfx.h"
#include "src/audio_channel.h"
#include "src/audio_loop_point.h"
#include "src/audio_commands_category2.h"
#include "src/audio_category_fade.h"
#include "src/gfx_buffer.h"
#include "src/gfx_rect.h"
#include "src/controller.h"
#include "src/fragments/widget_toolkit/widget_toolkit.h"
#include "src/fragments/minigame_actor_framework/minigame_actor_framework.h"
#include "src/memory.h"
#include "src/stage_loader.h"

typedef struct SushiPanelTexture {
    /* 0x00 */ u8* texture;
    /* 0x02 */ s16 width;
    /* 0x04 */ s16 height;
} SushiPanelTexture; // size = 0x8

static u32 gSushiGoRoundGeoLayout[] = {
    0x0C00FFFF, 0x05000000, 0x0B00001E, 0x00000000, 0x014000F0, 0x0000000F, 0x00000000, 0x00000000,
    0x05000000, 0x0D000000, 0x05000000, 0x14000000, 0x002B0012, 0xFFFFFF32, 0x16FFFFFF, 0x0F000003,
    0x05000000, 0x1F00FFFF, 0x00000000, 0x00000000, 0xFFFF0000, 0x00640064, 0x00640000, 0x05000000,
    0x00000000, &D_87000000[0x6D00], 0x06000000, 0x06000000, 0x05000000, 0x0A000000, &gGraphRoot0, 0x06000000,
    0x06000000, 0x06000000, 0x03000000, gFragment2_1GeoLayout, 0x06000000, 0x01000000,
};

static Controller** gPlayer1ControllerPtr = &gPlayer1Controller;
static Controller** gPlayer2ControllerPtr = &gPlayer2Controller;
static Controller** gPlayer3ControllerPtr = &gPlayer3Controller;
static Controller** gPlayer4ControllerPtr = &gPlayer4Controller;

static s16 gPanelColumnX[] = { 0x1E, 0x1E, 0x108, 0x108 };
static s16 gPanelColumnY[] = { 0xCC, 0x10, 0x10, 0xCC };

static SushiPanelTexture gTexPlateStackBase = { 0x400C800, 32, 8 };
static SushiPanelTexture gTexPlateStackMid = { 0x400CA00, 32, 8 };
static SushiPanelTexture gTexPlateStackTop = { 0x400CC00, 32, 8 };
static SushiPanelTexture gTexPlateStackBottom = { 0x400CE00, 32, 4 };
static SushiPanelTexture gTexPlateStackSide = { 0x400CF00, 32, 8 };
static SushiPanelTexture gTexPanelCornerLower = { 0x400D100, 32, 4 };
static SushiPanelTexture gTexPanelCornerMiddle = { 0x400D200, 32, 4 };
static SushiPanelTexture* gSushiPlateStack1Textures[] = { &gTexPlateStackMid, &gTexPlateStackBase, NULL };
static SushiPanelTexture* gSushiPlateStack2Textures[] = { &gTexPlateStackBottom, &gTexPlateStackTop, &gTexPlateStackBase, NULL };
static SushiPanelTexture* gSushiPlateStack3Textures[] = { &gTexPlateStackBottom, &gTexPanelCornerLower, &gTexPlateStackSide, &gTexPlateStackBase, NULL };
static SushiPanelTexture* gSushiPlateStack4Textures[] = { &gTexPlateStackBottom, &gTexPanelCornerLower, &gTexPanelCornerMiddle, &gTexPlateStackSide, &gTexPlateStackBase, NULL };
static SushiPanelTexture* gSushiPlateStack5Textures[] = {
    &gTexPlateStackBottom, &gTexPanelCornerLower, &gTexPanelCornerMiddle, &gTexPanelCornerMiddle, &gTexPlateStackSide, &gTexPlateStackBase,
};
static SushiPanelTexture** gPanelTextureSets[] = { NULL, gSushiPlateStack1Textures, gSushiPlateStack2Textures, gSushiPlateStack3Textures, gSushiPlateStack4Textures, gSushiPlateStack5Textures };
static AnimFrame gSushiHudBottomRightIconFrames[] = {
    { 0x0303ED80, 0xE }, { 0x0303F180, 2 }, { 0x0303F580, 6 }, { 0x0303F180, 2 }, { NULL, 0 },
};
static AnimFrame gSushiHudBottomCenterIconFrames[] = {
    { 0x03040580, 0x12 }, { 0x03040980, 2 }, { 0x03040D80, 8 }, { 0x03040980, 2 }, { NULL, 0 },
};
static AnimFrame gSushiHudLeftIconFrames[] = {
    { 0x030397C0, 0xE }, { 0x03034E40, 2 }, { 0x03035A80, 2 }, { 0x03034E40, 2 }, { 0x030397C0, 2 }, { 0x0303D500, 2 },
    { 0x0303E140, 2 },   { 0x03038B80, 2 }, { 0x03037300, 2 }, { 0x03037F40, 2 }, { 0x03035A80, 2 }, { 0x0303BC80, 2 },
    { 0x0303B040, 2 },   { 0x0303A400, 2 }, { 0x030397C0, 2 }, { 0x030366C0, 2 }, { 0x03037300, 2 }, { 0x030366C0, 2 },
    { 0x030397C0, 2 },   { 0x0303A400, 2 }, { 0x0303B040, 2 }, { 0x0303A400, 2 }, { NULL, 0 },
};
static Color_RGB8 gColorWhite = { 255, 255, 255 };
static Color_RGB8 gPanelTextColors[] = {
    { 0, 0, 0xFF },
    { 0, 0xFF, 0 },
    { 0xFF, 0, 0 },
    { 0xFF, 0xFF, 0 },
};
static Color_RGB8 gSushiTargetReachedEnvGradient[] = {
    { 255, 0, 0 },
    { 0, 255, 0 },
    { 255, 255, 100 },
};
static Color_RGB8 gSushiTargetReachedPrimGradient[] = {
    { 0, 255, 0 },
    { 255, 255, 100 },
    { 255, 0, 0 },
};
static Color_RGB8 gSushiHighComboTextGradient[] = {
    { 0, 0, 255 },
    { 255, 0, 0 },
    { 255, 255, 100 },
    { 0, 0, 0 },
};
static s32 gResultSoundCategories[] = { 0x50002, 0x50002, 0x50001, 0x50002, 0, 0, 0 };

ShadowContextNode* gSushiGoRoundGraphNode;
static FontContext* gSushiGoRoundTextObject;
static u32 gSushiGoRoundFrameCounter;
static s32 gSushiGoRoundState;
s32 gSushiGoRoundTimer;
static Controller gCombinedController;
static u16 gSushiUIFlags;
static s16 gIsDemoMode;
static s16 gSushiFadeTimer;

void SushiGame_FadeAudio(s32 arg0) {
    if (gIsDemoMode == 0) {
        Audio_FadeCategories(1, arg0);
        Audio_FadeCategories(2, arg0);
    } else {
        Audio_BeginVolumeTransition(arg0);
    }
}

void SushiGame_PollInput(void) {
    Controller* temp_v0;

    Cont_StartReadInputs();
    Cont_ReadInputs();
    Input_ResetRepeatState();

    bzero(&gCombinedController, sizeof(Controller));

    temp_v0 = *gPlayer1ControllerPtr;
    gCombinedController.buttonDown |= temp_v0->buttonDown;
    gCombinedController.buttonPressed |= temp_v0->buttonPressed;
    gCombinedController.unk_0A |= temp_v0->unk_0A;

    temp_v0 = *gPlayer2ControllerPtr;
    gCombinedController.buttonDown |= temp_v0->buttonDown;
    gCombinedController.buttonPressed |= temp_v0->buttonPressed;
    gCombinedController.unk_0A |= temp_v0->unk_0A;

    temp_v0 = *gPlayer3ControllerPtr;
    gCombinedController.buttonDown |= temp_v0->buttonDown;
    gCombinedController.buttonPressed |= temp_v0->buttonPressed;
    gCombinedController.unk_0A |= temp_v0->unk_0A;

    temp_v0 = *gPlayer4ControllerPtr;
    gCombinedController.buttonDown |= temp_v0->buttonDown;
    gCombinedController.buttonPressed |= temp_v0->buttonPressed;
    gCombinedController.unk_0A |= temp_v0->unk_0A;

    if (1) {}
}

Gfx* SushiGame_DrawPlateStack(Gfx* arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4) {
    SushiPanelTexture** var_a3;
    s32 var_v0;
    s32 var_v1;
    s32 var_s0;

    var_s0 = (arg1 != 0) ? arg2 + 6 : arg2;
    var_v1 = arg3 + 24;

    while (arg4 > 0) {
        s32 sp34 = var_v1;

        if (arg4 < 5) {
            var_a3 = gPanelTextureSets[arg4];
            arg4 = 0;
        } else {
            var_a3 = &gSushiPlateStack5Textures[0];
            arg4 -= 5;
        }

        var_v0 = var_s0;
        while (*var_a3 != NULL) {
            SushiPanelTexture* ptr = *var_a3;
            sp34 -= (ptr->height * 3) / 4;

            gDPLoadTextureBlock(arg0++, ptr->texture, G_IM_FMT_RGBA, G_IM_SIZ_16b, ptr->width, ptr->height, 0,
                                G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK,
                                G_TX_NOLOD, G_TX_NOLOD);

            gSPTextureRectangle(arg0++, var_v0 << 2, sp34 << 2, (((ptr->width * 9) / 16) + var_v0) << 2,
                                (sp34 + ((ptr->height * 3) / 4)) << 2, G_TX_RENDERTILE, 0, 0, 0x071C, 0x0555);
            var_a3++;
        }

        if (arg1 == 0) {
            var_s0 += 20;
        } else {
            var_s0 -= 20;
        }
    }

    return arg0;
}

void SushiGame_DrawAllPlateStacks(void) {
    s32 i;
    Gfx* var_s4 = gDisplayListHead;
    SushiPlayerEntity* ptr;

    for (i = 0, ptr = gSushiEntities; i < 4; i++, ptr++) {
        var_s4 = SushiGame_DrawPlateStack(var_s4, i / 2, gPanelColumnX[i], gPanelColumnY[i], ptr->unk_02A);
    }

    gDisplayListHead = var_s4;
}

void SushiGame_InterpolateColorCycle(Color_RGB8* arg0, Color_RGB8* arg1, s32 arg2, s32 arg3, s32 arg4) {
    Color_RGB8* var_v0;
    Color_RGB8* var_v1;
    f32 temp_fv0;
    f32 var_ft5;
    s32 temp_a0;
    s32 tmp;

    if (arg2 >= 2) {
        tmp = gSushiGoRoundFrameCounter % arg3;
        temp_fv0 = tmp;
        temp_fv0 /= arg3;

        temp_a0 = (gSushiGoRoundFrameCounter % (arg3 * arg2)) / arg3;

        if (arg4 == 0) {
            var_v0 = &arg1[temp_a0];
            var_v1 = &arg1[(temp_a0 + 1) % arg2];
        } else {
            temp_a0 = (arg2 - temp_a0);
            temp_a0--;

            var_v0 = &arg1[temp_a0];
            temp_a0--;

            if (temp_a0 < 0) {
                temp_a0 += arg2;
            }
            var_v1 = &arg1[temp_a0];
        }

        arg0->r = var_v0->r + ((var_v1->r - var_v0->r) * temp_fv0);
        arg0->g = var_v0->g + ((var_v1->g - var_v0->g) * temp_fv0);
        arg0->b = var_v0->b + ((var_v1->b - var_v0->b) * temp_fv0);
    }
}

void SushiGame_DrawDishIcon(s32 arg0, s16 arg1, s16 arg2, f32 arg3) {
    s16 sp36;
    s16 sp34;
    s16 sp32;

    if ((arg0 > 0) && (arg0 < 9) && (arg3 != 0.0f)) {
        sp36 = ROUND_MAX(1024.0f / arg3);
        sp34 = ROUND_MAX(32.0f * arg3);
        sp32 = ROUND_MAX(32.0f * arg3);

        gDPLoadTextureBlock(gDisplayListHead++, gSushiItemDefs[arg0].unk_08, G_IM_FMT_RGBA, G_IM_SIZ_32b, 32, 32, 0,
                            G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK,
                            G_TX_NOLOD, G_TX_NOLOD);

        Gfx_DrawTexturedRectClipped(arg1, arg2, sp34, sp32, 0, 0, sp36, sp36, 0);
    }
}

void RenderSushiGoRoundUI(void) {
    s32 pad1;
    s32 pad2;
    s32 i;
    Color_RGB8 spD8[4];
    Color_RGB8 spD0[2];
    Color_RGB8 spB8[8];
    Color_RGB8 spB4 = gColorWhite;
    s32 var_v0;

    gSPDisplayList(gDisplayListHead++, gUiEnvTintTextureDL);

    if (gSushiUIFlags & 2) {
        SushiGame_DrawAllPlateStacks();
        for (i = 0; i < 4; i++) {
            if ((i == 0) || (i == 3)) {
                var_v0 = -0x18;
            } else {
                var_v0 = 0x1C;
            }

            if (!(gSushiEntities[i].unk_002 & 4)) {
                Widget_DrawPlayerIcon(i, gPanelColumnX[i], gPanelColumnY[i] + var_v0, 0.75f);
            } else {
                Widget_DrawPlayerIcon(-1 - i, gPanelColumnX[i], gPanelColumnY[i] + var_v0, 0.75f);
            }
        }
    }

    if (gIsDemoMode != 0) {
        Widget_DrawStripedBar(0x1E, 0x17);
    }

    gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);

    if (gSushiUIFlags & 1) {
        Ui_DrawBorderedPanel(0x18, 0x16, 0x110, 0x90, 0x15);
        Ui_DrawBorderedPanel(0x18, 0xA5, 0x110, 0x36, 0x14B);
        Widget_FillRect(0x17, 0x16, 2, 0x90);
        Widget_FillRect(0x127, 0x16, 2, 0x90);
    }

    Font_BeginTranslucentTextRendering();
    Font_EnableTwoCycleTexturing();

    if (gSushiUIFlags & 1) {
        s32 spA4;
        s32 spA0;
        s32 sp9C;
        s32 sp98;
        s32 sp94;

        Font_SetActive(2, 0);
        Gfx_SetEnvColor(0xFF, 0xFF, 0xFF, 0xFF);
        Gfx_SetPrimColor(0xF9, 0xF4, 0, 0xFF);
        Font_Printf(0xA0 - (Text_MeasureStringWidth(gSushiGoRoundTextObject, Text_GetString(NULL, 0, gMinigameNamesBank, 0xC)) / 2), 0x20,
                      Text_GetString(NULL, 0, gMinigameNamesBank, 0xC));
        Gfx_SetEnvColor(0xFF, 0xFF, 0xFF, 0xFF);
        Gfx_SetPrimColor(0xFF, 0xFF, 0xFF, 0xFF);
        Font_SetLineHeight(0xC);
        Font_Printf(0x28, 0x34, Text_GetString(NULL, 0, gMinigameNamesBank, 0x32));
        Font_Printf(0x28, 0x4C, Text_GetString(NULL, 0, gMinigameNamesBank, 0x33));
        Font_Printf(0x78, 0xB2, Text_GetString(NULL, 0, gMinigameNamesBank, 0x5C));
        Font_Printf(0xDA, 0xB2, Text_GetString(NULL, 0, gMinigameNamesBank, 0x5C));
        Font_Printf(0x86, 0xC8, Text_GetString(NULL, 0, gMinigameNamesBank, 0x5C));
        Gfx_SetEnvColor(0xE6, 0xD2, 0xFF, 0xFF);
        Gfx_SetPrimColor(0x82, 0x50, 0xD2, 0xFF);
        Font_Printf(0x8C, 0xB2, Text_GetString(NULL, 0, gMinigameNamesBank, 0x34));
        Gfx_SetEnvColor(0xC8, 0xC8, 0xFF, 0xFF);
        Gfx_SetPrimColor(0x50, 0x50, 0xFF, 0xFF);
        Font_Printf(0xEE, 0xB2, Text_GetString(NULL, 0, gMinigameNamesBank, 0x35));
        Gfx_SetEnvColor(0xFF, 0xFF, 0xFF, 0xFF);
        Gfx_SetPrimColor(0xFF, 0x80, 0x80, 0xFF);
        Font_Printf(0x9A, 0xC8, Text_GetString(NULL, 0, gMinigameNamesBank, 0x14));
        Widget_MeasureLabelBox(&spA4, &spA0, Text_GetString(NULL, 0, gMinigameNamesBank, 0x32), Text_GetString(NULL, 0, gMinigameNamesBank, 0x5F),
                      0xC);
        Gfx_SetEnvColor(0xC8, 0xC8, 0xFF, 0xFF);
        Gfx_SetPrimColor(0x50, 0x50, 0xFF, 0xFF);
        Font_Printf(spA4 + 0x28, spA0 + 0x34, Text_GetString(NULL, 0, gMinigameNamesBank, 0x5F));
        Font_SetActive(1, 0);
        Gfx_SetEnvColor(0xFF, 0xFF, 0xFF, 0xFF);
        Gfx_SetPrimColor(0xFF, 0xFF, 0xFF, 0xFF);

        var_v0 = 60;
        for (i = 0; i < 8; i++) {
            pad1 = i % 4;
            if (pad1) {}

            sp9C = Font_MeasureTextExtent(0, 0, Text_GetString(NULL, 0, gMinigameNamesBank, 0x36));
            sp98 = Font_MeasureTextExtent(0, 0, Text_GetString(NULL, 0, gMinigameNamesBank, i + 0x36));

            Font_Printf((((var_v0 * pad1) + sp9C) - sp98) + 0x46, ((i / 4) << 5) + 0x72,
                          Text_GetString(NULL, 0, gMinigameNamesBank, i + 0x36));
        }
    }

    switch (gSushiGoRoundState) {
        s32 var_s4;
        s32 var_s7;
        SushiPlayerEntity* ptr;

        case 4:
            break;

        case 2:
        case 3:
            if (gIsDemoMode == 0) {
                Font_SetActive(4, 0);
                for (i = 0, ptr = gSushiEntities; i < 4; i++, ptr++) {
                    if ((ptr->unk_040.unk_00 != 0) && (ptr->unk_040.unk_00 != 1)) {
                        if ((i / 2) != 0) {
                            var_s7 = -0x14;
                        } else {
                            var_s7 = 0x1E;
                        }

                        if ((i == 0) || (i == 3)) {
                            var_s4 = -0x16;
                        } else {
                            var_s4 = 0x1E;
                        }

                        if (ptr->unk_03E < 5) {
                            Gfx_SetEnvColor(0xFF, 0xFF, 0xFF, ptr->unk_040.unk_18);
                            Gfx_SetPrimColor(gPanelTextColors[i].r, gPanelTextColors[i].g, gPanelTextColors[i].b,
                                          ptr->unk_040.unk_18);
                        } else {
                            SushiGame_InterpolateColorCycle(spD8, gSushiHighComboTextGradient, 4, 4, 0);
                            Gfx_SetEnvColor(spD8[0].r, spD8[0].g, spD8[0].b, ptr->unk_040.unk_18);
                            Gfx_SetPrimColor(spD8[0].r, spD8[0].g, spD8[0].b, ptr->unk_040.unk_18);
                        }

                        Font_Printf((s16)ptr->unk_040.unk_08 + (gPanelColumnX[i] + var_s7),
                                      (gPanelColumnY[i] + var_s4) - (s16)ptr->unk_040.unk_0C, "x%d",
                                      ptr->unk_03E);
                    }
                }
            }
            break;

        case 5:
        case 6:
            Font_SetActive(4, 0);

            for (i = 0, ptr = gSushiEntities; i < 4; i++, ptr++) {
                char sp78[16];

                spD0[0] = spB4;
                spD0[1] = gPanelTextColors[i];

                SushiGame_InterpolateColorCycle(&spB8[i], spD0, 2, 0x1E, 0);
                SushiGame_InterpolateColorCycle(&spB8[i + 4], spD0, 2, 0x1E, 1);

                if ((i / 2) != 0) {
                    var_s7 = -0x32;
                } else {
                    var_s7 = 0x1C;
                }

                if ((i == 0) || (i == 3)) {
                    var_s4 = -0x16;
                } else {
                    var_s4 = 0x1E;
                }

                if ((ptr->unk_028 == 1) && (ptr->unk_018 == ptr->unk_01C) &&
                    (ptr->unk_018 > 0)) {
                    SushiGame_InterpolateColorCycle(&spD8, &gSushiTargetReachedEnvGradient, 3, 4, 0);
                    Gfx_SetEnvColor(spD8[0].r, spD8[0].g, spD8[0].b, 0xFF);
                    SushiGame_InterpolateColorCycle(&spD8, &gSushiTargetReachedPrimGradient, 3, 4, 0);
                    Gfx_SetPrimColor(spD8[0].r, spD8[0].g, spD8[0].b, 0xFF);
                } else {
                    Gfx_SetEnvColor(spB8[i].r, spB8[i].g, spB8[i].b, 0xFF);
                    Gfx_SetPrimColor(spB8[i + 4].r, spB8[i + 4].g, spB8[i + 4].b, 0xFF);
                }

                Text_SetNumberToken(6, ptr->unk_01C);
                Font_Printf(gPanelColumnX[i] + var_s7, gPanelColumnY[i] + var_s4,
                              Text_GetString(&sp78, 0x20, gMinigameNamesBank, 0x5E));
            }
            break;
    }

    Font_DisableTwoCycleTexturing();
    Font_EndTexturedTextRendering();

    if (gSushiUIFlags & 1) {
        gSPDisplayList(gDisplayListHead++, gUiEnvTintTextureDL);

        for (i = 0; i < 8; i++) {
            if ((i % 4) >= 2) {
                var_v0 = 0x3C;
            } else {
                var_v0 = 0x40;
            }

            pad1 = i % 4;
            SushiGame_DrawDishIcon(gSushiOrderIndices[i], (var_v0 * pad1) + 0x28, ((i / 4) << 5) + 0x68, 0.75f);
        }

        Widget_DrawAnimIcon28(0, 0x5A, 0xA8, gSushiHudLeftIconFrames);
        Widget_DrawAnimIcon16(0xC8, 0xB0, gSushiHudBottomRightIconFrames);
        Widget_DrawAnimIcon16C(0x74, 0xC6, gSushiHudBottomCenterIconFrames);
        Widget_DrawSplitBanner(0, 0x19, 0xA6);
    }
}

void SushiGame_DrawFrame(void) {
    BgStage_DrawFrame();
    GfxImage_ClearDepthRectangle(&gDisplayListHead, 0, 0, 0x140, 0xF0);

    if (gResultMenuVisible == 0) {
        GeoRender_AdvanceFrameCounter();
    }

    Geo_RenderRootNode(gSushiGoRoundGraphNode);
    MiniFx_DrawParticles();

    if (gResultConfirmPressed == 0) {
        RenderSushiGoRoundUI();
    }

    if (gIsDemoMode == 0) {
        Widget_PauseMenuUpdate();
    }

    if (gIsDemoMode != 0) {
        Widget_DrawControllerDisconnectBanner();
    }

    BgStage_AdvanceFrame();
}

void SushiGame_UpdateWaitForStart(void) {
    Rand_Range(0x80);

    if (StageContext_GetFadeMode() == 0) {
        if (gIsDemoMode != 0) {
            gSushiGoRoundState = 1;
            gSushiGoRoundTimer = 0x1C2;
            gSushiFadeTimer = 0xF;
        } else if (gPlayer1Controller->buttonPressed & 0x1000) {
            gSushiGoRoundState = 1;
            gSushiUIFlags &= ~1;
            gSushiUIFlags |= 2;
            gSushiGoRoundTimer = 0x708;
            gSushiFadeTimer = 0xF;
            Audio_FadeOutAll(0x3C);
            SushiGame_BroadcastPlayerPhase(0xF, 1);
        } else if ((gMenuCursorX == 0) && (gPlayer1Controller->buttonPressed & 0x4000)) {
            Audio_PlaySoundEffectById(SFX_MENU_BACK);
            gSushiGoRoundState = 7;
            gSushiFadeTimer = 0x1E;
            StageFade_StartFromTransparent(gSushiFadeTimer);
            SushiGame_FadeAudio(gSushiFadeTimer / 2);
            Widget_PauseMenuTrigger(2);
        }
    }
}

s32 SushiGame_IsRoundTimeUp(void) {
    s32 var_v1 = 0;

    if (gIsDemoMode == 0) {
        if (gSushiGoRoundTimer <= 0) {
            var_v1 = 1;
        }
    } else if (gSushiGoRoundTimer <= 0) {
        var_v1 = 1;
    }
    return var_v1;
}

s32 SushiGame_UpdateGoRoundState(void) {
    s32 sp1C;
    s32 sp18;
    SushiPlayerEntity* ptr;
    s32 i;
    s32 temp_v0;
    s32 var_a0;

    sp1C = 0;
    sp18 = 0;
    if ((gIsDemoMode != 0) && (gPlayer1Controller->buttonPressed != 0)) {
        if (StageContext_GetFadeMode() == 0) {
            sp18 = 1;
        }
    }
    if (((gResultFlag_Special != 0) || (sp18 != 0)) && (gSushiGoRoundState != 7)) {
        gSushiGoRoundState = 7;
        gSushiFadeTimer = 0x1E;
        StageFade_StartFromTransparent(gSushiFadeTimer);
        SushiGame_FadeAudio(gSushiFadeTimer / 2);
    }
    switch (gSushiGoRoundState) {
        case 0:
            SushiGame_UpdateWaitForStart();
            break;
        case 1:
            gSushiFadeTimer -= 1;
            if (gSushiFadeTimer <= 0) {
                gSushiGoRoundState = 2;
                Widget_CountdownStart(1);
            }
            break;
        case 2:
            temp_v0 = Widget_CountdownGetState();
            if ((temp_v0 >= 0) && (temp_v0 < 4)) {
                Audio_DispatchSoundBankCommand(gResultSoundCategories[temp_v0], 0, 0);
            } else if (temp_v0 == -2) {
                gSushiGoRoundState = 3;
                SushiGame_BroadcastPlayerPhase(0xF, 3);
                SushiGame_SetGamePhase(1);
            }
            break;
        case 3:
            if (gResultMenuVisible == 0) {
                gSushiGoRoundTimer--;
                if (((gSushiGoRoundTimer % 30) == 0) && (gIsDemoMode == 0)) {
                    if (((gSushiGoRoundTimer / 30) > 0) && ((gSushiGoRoundTimer / 30) < 6)) {
                        Audio_DispatchSoundBankCommand(0x50002, 0, 0);
                    }
                }
            }
            if (SushiGame_IsRoundTimeUp() != 0) {
                if (gIsDemoMode == 0) {
                    gSushiGoRoundState = 4;
                    gSushiFadeTimer = 0x1E;
                    SushiGame_BroadcastPlayerPhase(0xF, 2);
                    SushiGame_SetGamePhase(2);
                    Audio_FadeOutAll(0xA);
                    Audio_DispatchSoundBankCommand(0x50004, 0, 0);
                } else {
                    gSushiGoRoundState = 7;
                    gSushiFadeTimer = 0x1E;
                    StageFade_StartFromTransparent(gSushiFadeTimer);
                    SushiGame_FadeAudio(gSushiFadeTimer / 2);
                    Widget_PauseMenuTrigger(2);
                }
            }
            break;
        case 4:
            if (SushiGame_AreAllPlayersIdle() != 0) {
                gSushiFadeTimer--;
            }
            if (gSushiFadeTimer <= 0) {
                gSushiGoRoundState = 5;
                SushiGame_BroadcastPlayerPhase(0xF, 4);
            }
            break;
        case 5:
            var_a0 = 1;
            ptr = &gSushiEntities[0];
            for (i = 0; i < 4; i++, ptr++) {
                if (ptr->unk_018 != ptr->unk_01C) {
                    var_a0 = 0;
                }
            }
            if ((var_a0 != 0) && (gResultMenuVisible == 0)) {
                gSushiGoRoundState = 6;
                SushiGame_BroadcastPlayerPhase(0xF, 5);
                Widget_PauseMenuTrigger(1);
            }
            break;
        case 6:
            if (gResultExitPending != 0) {
                gSushiGoRoundState = 7;
                gSushiFadeTimer = 0x1E;
                StageFade_StartFromTransparent(gSushiFadeTimer);
                SushiGame_FadeAudio(gSushiFadeTimer / 2);
            }
            break;
        case 7:
            if (gSushiFadeTimer > 0) {
                gSushiFadeTimer--;
            }
            if ((gSushiFadeTimer <= 0) && (Audio_GetActivityScore() == 0)) {
                sp1C = 1;
                Audio_ResetVolumeTransition();
            }
            break;
    }
    return sp1C;
}

void SushiGame_MainLoop(void) {
    s32 var_s0;

    StageLoader_RunFrames(5);
    StageFade_StartFromOpaque(0x10);
    gSushiGoRoundFrameCounter = 0;

    var_s0 = 1;
    while (var_s0 != 0) {
        SushiGame_PollInput();
        Widget_CountdownUpdate();
        SushiGame_UpdateCamera();
        SushiGame_UpdateConveyorBelt();
        SushiGame_UpdatePlayers();
        MiniFx_UpdateParticles();
        SushiGame_DrawFrame();

        if (SushiGame_UpdateGoRoundState() != 0) {
            var_s0 = 0;
        }

        gSushiGoRoundFrameCounter++;
    }
}

void SushiGame_LoadAssets(void) {
    UNUSED s32 pad;
    MemoryBlock* sp18;

    sp18 = MainPool_AllocState(main_pool_get_available(), 0);
    gSushiGoRoundGraphNode = (GraphNode*)process_geo_layout(sp18, &gSushiGoRoundGeoLayout);

    MainPool_FinalizeAllocation(sp18);
    ModelRenderer_InitDisplayRoots();
    PokeIcon_OpenModelArchives();
    Widget_CountdownInit();
    SushiGame_InitConveyorBelt();

    if (gIsDemoMode != 0) {
        SushiGame_InitPlayerSlots(1);
    } else {
        SushiGame_InitPlayerSlots(0);
    }

    SushiGame_InitCamera();
    MiniFx_Init();
    Widget_PauseMenuInit();

    gSushiGoRoundState = 0;
    if (gIsDemoMode == 0) {
        gSushiUIFlags |= 1;
    }

    SushiGame_BroadcastPlayerPhase(0xF, 2);
}

void SushiGame_Main(s32 arg0, UNUSED s32 arg1) {
    StageContext* sp24;

    if (arg0 == 1) {
        gIsDemoMode = 1;
        Minigame_InitSharedState(9);
    } else {
        gIsDemoMode = 0;
    }

    main_pool_push_state('MINI');

    Gfx_InitDisplayListBuffers(0x18000, 0);
    sp24 = StageContext_Allocate(0, 1, 3, 1, 2, 1);
    gSushiGoRoundTextObject = Font_Init(0x17, 0);

    ASSET_LOAD(D_1000000, common_menu1_ui, 0);
    ASSET_LOAD(gRentalViewerIconTop, sushi_go_round_sprites, 0);

    FRAGMENT_LOAD(FRAGMENT_PARTICLE_DATA_LIBRARY);
    Fragment_Load((((u32)D_87000000 & 0x0FF00000) >> 0x14) - 0x10, sushi_go_round_ROM_START, order_select_ui_ROM_START);

    SushiGame_LoadAssets();
    StageContext_Activate(sp24);
    SushiGame_MainLoop();
    StageContext_Deactivate();
    Font_Free();
    Gfx_FreeDisplayListBuffers();

    main_pool_pop_state('MINI');

    Widget_PauseMenuGetResult();
}
