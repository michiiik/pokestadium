#include "dynamo_game.h"
#include "src/geo_render.h"
#include "src/model_animation.h"
#include "src/model_animation_events.h"
#include "src/geo_layout.h"
#include "src/model_renderer.h"
#include "src/graphics_textures.h"
#include "src/ui_graphics.h"
#include "src/text_system.h"
#include "src/jpeg_stream.h"
#include "src/audio_sfx.h"
#include "src/audio_loop_point.h"
#include "src/audio_commands_category2.h"
#include "src/audio_category_fade.h"
#include "src/gfx_buffer.h"
#include "src/gfx_rect.h"
#include "src/matrix.h"
#include "src/fragments/widget_toolkit/widget_toolkit.h"
#include "src/fragments/minigame_shared_library/minigame_shared_library.h"
#include "src/stage_loader.h"
#include "src/controller.h"

static DisplayObject gChainLinkNode0;
static DisplayObject gChainLinkNode1;
static DisplayObject gChainLinkNode2;
static DisplayObject gChainLinkNode3;
static DisplayObject gChainLinkNode4;
static DisplayObject gChainLinkNode5;
static DisplayObject gChainLinkNode6;
static DisplayObject gChainLinkNode7;
static DisplayObject gChainLinkNode8;
static DisplayObject gChainLinkNode9;
static DisplayObject gChainLinkNode10;
static DisplayObject gChainLinkNode11;
static DisplayObject gChainLinkNode12;

static unk_D_864027C0 gHealthBarPositions[] = {
    { -60.0f, 7.5f, 0.0f, 1.0f, 1.0f, 1.0f, 0, 0xA00, 0 },
    { -20.0f, 7.5f, 0.0f, 1.0f, 1.0f, 1.0f, 0, 0x300, 0 },
    { 20.0f, 7.5f, 0.0f, 1.0f, 1.0f, 1.0f, 0, -0x300, 0 },
    { 60.0f, 7.5f, 0.0f, 1.0f, 1.0f, 1.0f, 0, -0xA00, 0 },
};

static DisplayObject* gHealthBarGraphNodePtrs[] = {
    &gChainLinkNode0, &gChainLinkNode1, &gChainLinkNode2, &gChainLinkNode3, &gChainLinkNode4, &gChainLinkNode5, &gChainLinkNode6,
    &gChainLinkNode7, &gChainLinkNode8, &gChainLinkNode9, &gChainLinkNode10, &gChainLinkNode11, &gChainLinkNode12,
};

static u8 gHealthBarUpdateCounter = 0;
static u16 gResultTimer = 0;

static AnimFrame gDynamoTutoBButtonIconAnim[] = {
    { 0x0303F980, 2 }, { 0x0303FD80, 2 }, { 0x03040180, 2 },    { 0x0303FD80, 2 }, { 0x0303F980, 2 },
    { 0x0303FD80, 2 }, { 0x03040180, 2 }, { 0x0303FD80, 2 },    { 0x0303F980, 2 }, { 0x0303FD80, 2 },
    { 0x03040180, 2 }, { 0x0303FD80, 2 }, { 0x0303F980, 0x18 }, { NULL, 0 },
};

static AnimFrame gDynamoTutoAButtonIconAnim[] = {
    { 0x0303ED80, 0x18 }, { 0x0303F180, 2 }, { 0x0303F580, 2 }, { 0x0303F180, 2 }, { 0x0303ED80, 2 },
    { 0x0303F180, 2 },    { 0x0303F580, 2 }, { 0x0303F180, 2 }, { 0x0303ED80, 2 }, { 0x0303F180, 2 },
    { 0x0303F580, 2 },    { 0x0303F180, 2 }, { 0x0303ED80, 2 }, { NULL, 0 },
};

static AnimFrame gDynamoTutoStartButtonIconAnim[] = {
    { 0x03040580, 0x12 }, { 0x03040980, 2 }, { 0x03040D80, 8 }, { 0x03040980, 2 }, { NULL, 0 },
};

static u32 gDynamoStageGeoLayoutData[] = {
    0x0C00FFFF, 0x05000000, 0x0B00001E, 0x00000000, 0x014000F0, 0x0000000F, 0x00000000, 0x00000000, 0x05000000,
    0x0D000000, 0x05000000, 0x14000000, 0x002B0012, 0xFFFFFF32, 0x16FFFFFF, 0x0F000003, 0x05000000, 0x1F00FFFF,
    0x00000000, 0x00000000, 0x00000000, 0x00640064, 0x00640000, 0x06000000, 0x05000000, 0x0A000000, &gGraphRoot0,
    0x06000000, 0x06000000, 0x06000000, 0x03000000, gFragment2_1GeoLayout, 0x06000000, 0x01000000,
};

static u8 gHealthBarAnimFlagTeam0 = 0;
static u8 gHealthBarAnimFlagTeam1 = 0;

DynamoHealthBar gDynamoChargeBarData[4];
DynamoHealthBar* gCurrentHealthBar;
static DisplayObject* gCurrentHealthBarNode;
DynamoHealthBarEffect gHealthBarEffects[4];
DynamoHealthBarEffect* gHealthBarEffectData;

void DynamoGame_ComputeAIChargeDelta(DynamoHealthBar* arg0, u8 arg1) {
    if (arg0->unk_000 == 4) {
        arg0->unk_00D = 0;
        return;
    }

    if (arg1 == 0) {
        arg0->unk_00D = MiniGame_RandomRange(3);
    } else if ((gHealthBarTimer + 1) == gHealthBarDuration) {
        arg0->unk_00D = DynamoGame_RollAIChargeBias(arg0->unk_000, 1);
    } else {
        arg0->unk_00D = DynamoGame_RollAIChargeBias(arg0->unk_000, 0);
    }
}

void DynamoGame_InitPlayerSlots(void) {
    s8 i;

    for (i = 0; i < 4; i++) {
        if (gMiniHudColorDefaults[i] == 0) {
            gDynamoChargeBarData[i].unk_000 = 4;
        } else {
            gDynamoChargeBarData[i].unk_000 = gMinigameComDifficulty;
        }
    }
}

void DynamoGame_AssignTeams(void) {
    u8 i;

    for (i = 0; i < 4; i++) {
        switch (gMenuOptionFlags[i]) {
            case 1:
                gDynamoChargeBarData[i].unk_004 = 1;
                break;

            case 2:
                gDynamoChargeBarData[i].unk_004 = 0;
                break;

            default:
                if (MiniGame_RandomRange(2) != 0) {
                    gDynamoChargeBarData[i].unk_004 = 0;
                } else {
                    gDynamoChargeBarData[i].unk_004 = 1;
                }
                break;
        }
    }
}

void DynamoGame_UpdatePlayerAnimState(void) {
    u8 i;
    s8 temp_ft2;
    u8 var_s1;
    s32 sp58;
    UNUSED s32 pad;
    u32 var_s0_2;

    if (gResultMenuVisible != 1) {
        for (i = 0; i < 4; i++) {
            gCurrentHealthBar = &gDynamoChargeBarData[i];
            sp58 = gCurrentHealthBar->unk_008;

            var_s1 = 0;
            if (gDrowzeeGameIsOver != 0) {
                if (gResultTimer < 0x57) {
                    var_s1 = 2;
                } else if (gResultTimer == 0x57) {
                    var_s1 = 1;
                    if (gCurrentHealthBar->unk_00C == 1) {
                        gCurrentHealthBar->unk_008 = 3;
                    } else {
                        gCurrentHealthBar->unk_008 = 4;
                    }
                } else if (gResultTimer == 0xB4) {
                    var_s1 = 1;
                    if (gCurrentHealthBar->unk_00C == 1) {
                        gCurrentHealthBar->unk_008 = 6;
                    } else {
                        gCurrentHealthBar->unk_008 = 5;
                    }
                } else if (ModelAnim_IsFinished(&gCurrentHealthBar->unk_014) != 0) {
                    switch (gCurrentHealthBar->unk_008) {
                        case 6:
                            var_s1 = 1;
                            if (gCurrentHealthBar->unk_004 == 0) {
                                gHealthBarAnimFlagTeam0 = 1;
                            } else if (gCurrentHealthBar->unk_004 == 1) {
                                gHealthBarAnimFlagTeam1 = 1;
                            }
                            break;

                        case 4:
                            var_s1 = 1;
                            gCurrentHealthBar->unk_008 = 5;
                            break;

                        case 0:
                        case 3:
                            var_s1 = 1;
                            gCurrentHealthBar->unk_008 = 0;
                            break;
                    }
                } else if ((gCurrentHealthBar->unk_008 == 6) &&
                           (((gHealthBarAnimFlagTeam0 != 0) && ((gCurrentHealthBar->unk_014.transformAnim.frameFixed >> 0x10) == 0x37)) ||
                            ((gHealthBarAnimFlagTeam1 != 0) && ((gCurrentHealthBar->unk_014.transformAnim.frameFixed >> 0x10) == 0x5E)))) {
                    var_s1 = 1;
                }
            } else if ((gHealthBarUpdateCounter == 0) && (gCurrentHealthBar->unk_00E >= 3) &&
                       ((gCurrentHealthBar->unk_008 == 0) || (gCurrentHealthBar->unk_008 == 3))) {
                gCurrentHealthBar->unk_008 = 1;
                var_s1 = 1;
                if ((gResultFlag_Special == 0) && (gResultMenuVisible == 0)) {
                    Audio_DispatchSoundBankCommand(0x80001, i, 0);
                }
            } else if ((gHealthBarUpdateCounter == 0) && (gCurrentHealthBar->unk_00E < 3) && (gCurrentHealthBar->unk_008 == 2)) {
                gCurrentHealthBar->unk_008 = 3;
                var_s1 = 1;
                if ((gResultFlag_Special == 0) && (gResultMenuVisible == 0)) {
                    Audio_DispatchSoundBankCommand(0x80001, i, 5);
                }
            } else {
                if ((gResultFlag_Special == 0) && (gResultMenuVisible == 0)) {
                    temp_ft2 = ROUND_MAX((gCurrentHealthBar->unk_010 * 100.0f) / gCurrentHealthBar->unk_012);

                    if (temp_ft2 >= 0x4B) {
                        var_s0_2 = 4;
                    } else if (temp_ft2 >= 0x32) {
                        var_s0_2 = 3;
                    } else if (temp_ft2 >= 0x19) {
                        var_s0_2 = 2;
                    } else {
                        var_s0_2 = 1;
                    }

                    Audio_DispatchSoundBankCommand(0x80001, i, var_s0_2);
                    Audio_DispatchSoundBankCommand(0x80002, i, var_s0_2);
                }

                if (ModelAnim_IsFinished(&gCurrentHealthBar->unk_014) != 0) {
                    switch (gCurrentHealthBar->unk_008) {
                        case 0:
                        case 3:
                            var_s1 = 1;
                            gCurrentHealthBar->unk_008 = 0;
                            break;

                        case 1:
                        case 2:
                            var_s1 = 1;
                            gCurrentHealthBar->unk_008 = 2;
                            break;
                    }
                }
            }

            if (!!var_s1) {
                if (gCurrentHealthBar->unk_008 != sp58) {
                    ModelAnim_SetAnimation(&gCurrentHealthBar->unk_014, gCurrentHealthBar->unk_008);
                    if (gCurrentHealthBar->unk_008 != 0) {
                        ModelAnim_SetEventTrack(&gCurrentHealthBar->unk_014, gCurrentHealthBar->unk_008);
                    }
                } else if (gCurrentHealthBar->unk_008 == 6) {
                    if (gCurrentHealthBar->unk_004 == 0) {
                        ModelAnim_SetFrame(&gCurrentHealthBar->unk_014, 0x1B);
                        ModelAnim_SetEventFrame(&gCurrentHealthBar->unk_014, 0x1B);
                    } else if (gCurrentHealthBar->unk_004 == 1) {
                        ModelAnim_SetFrame(&gCurrentHealthBar->unk_014, 0x4B);
                        ModelAnim_SetEventFrame(&gCurrentHealthBar->unk_014, 0x4B);
                    }
                } else if (var_s1 == 2) {
                    ModelAnim_SetFrame(&gCurrentHealthBar->unk_014, gCurrentHealthBar->unk_014.transformAnim.frameFixed >> 0x10);
                    if (gCurrentHealthBar->unk_008 != 0) {
                        ModelAnim_SetEventFrame(&gCurrentHealthBar->unk_014, gCurrentHealthBar->unk_014.transformAnim.frameFixed >> 0x10);
                    }
                } else {
                    ModelAnim_SetFrame(&gCurrentHealthBar->unk_014, 0);
                    if (gCurrentHealthBar->unk_008 != 0) {
                        ModelAnim_SetEventFrame(&gCurrentHealthBar->unk_014, 0);
                    }
                }
            }
        }
    }
}

void DynamoGame_SyncChainLinkAnimations(void) {
    u8 i;
    u32 var_a0;

    for (i = 0; i < 13; i++) {
        var_a0 = 0;

        switch (i) {
            case 0:
                var_a0 = 1;
                break;

            case 1:
            case 2:
            case 3:
            case 4:
                if (gDynamoChargeBarData[i - 1].unk_00C != 1) {
                    var_a0 = 1;
                    if (gDynamoChargeBarData[i - 1].unk_004 == 0) {
                        gHealthBarGraphNodePtrs[i]->position = gDynamoChargeBarData[i - 1].unk_014.anchors[4].position;
                    } else if (gDynamoChargeBarData[i - 1].unk_004 == 1) {
                        gHealthBarGraphNodePtrs[i]->position = gDynamoChargeBarData[i - 1].unk_014.anchors[3].position;
                    }
                }
                break;

            case 5:
            case 6:
            case 7:
            case 8:
                if (gDynamoChargeBarData[i - 5].unk_00C == 1) {
                    var_a0 = 1;
                }
                break;

            case 9:
            case 10:
            case 11:
            case 12:
                if (gDynamoChargeBarData[i - 9].unk_00C != 1) {
                    var_a0 = 1;
                }
                break;
        }

        if (var_a0 != 0) {
            ModelAnim_SetFrame(gHealthBarGraphNodePtrs[i], 0);
            ModelAnim_SetEventTrack(gHealthBarGraphNodePtrs[i], 0);
            ModelAnim_SetEventFrame(gHealthBarGraphNodePtrs[i], 0);
        }
    }
}

void DynamoGame_DetermineWinners(void) {
    u8 i;
    s16 var_s4 = 0;

    for (i = 0; i < 4; i++) {
        gCurrentHealthBar = &gDynamoChargeBarData[i];

        if ((gCurrentHealthBar->unk_010 >= gCurrentHealthBar->unk_012) && (var_s4 < gCurrentHealthBar->unk_010)) {
            var_s4 = gCurrentHealthBar->unk_010;
        }
    }

    for (i = 0; i < 4; i++) {
        gCurrentHealthBar = &gDynamoChargeBarData[i];

        if ((gCurrentHealthBar->unk_010 >= gCurrentHealthBar->unk_012) && (var_s4 == gCurrentHealthBar->unk_010)) {
            gCurrentHealthBar->unk_00C = 1;
            Widget_PauseMenuRecordWin(i);

            gDrowzeeWinnerCount++;

            if ((gResultFlag_Special == 0) && (gResultMenuVisible == 0)) {
                Audio_DispatchSoundBankCommand(0x80003, i, 0);
            }
        }
    }

    for (i = 0; i < 4; i++) {
        gCurrentHealthBar = &gDynamoChargeBarData[i];

        if (gCurrentHealthBar->unk_010 >= gCurrentHealthBar->unk_012) {
            if (gCurrentHealthBar->unk_00C == 1) {
                gCurrentHealthBar->unk_010 = gCurrentHealthBar->unk_012 + 1;
            } else {
                gCurrentHealthBar->unk_010 = gCurrentHealthBar->unk_012;
            }
        }
    }
}

void DynamoGame_UpdateCountdownAndCue(UNUSED s32 arg0) {
    Widget_CountdownUpdate();
    DynamoGame_DrawCrankIcon();
}

void DynamoGame_ReadPlayerCrankInput(void) {
    u8 i;

    if ((gResultFlag_Special != 0) || (gResultMenuVisible != 0) || (gDrowzeeGameIsOver != 0)) {
        return;
    }

    for (i = 0; i < 4; i++) {
        gCurrentHealthBar = &gDynamoChargeBarData[i];

        if (gCurrentHealthBar->unk_000 == 4) {
            if ((gHealthBarPaletteIndex == 1) || (gHealthBarPaletteIndex == 3) || ((gHealthBarPaletteIndex == 0) && (gHealthBarMode == 1))) {
                if (gControllers[i].buttonPressed & 0x8000) {
                    gCurrentHealthBar->unk_00D += 1;
                }

                if (gControllers[i].buttonPressed & 0x4000) {
                    gCurrentHealthBar->unk_00D -= 2;
                }
            } else if ((gHealthBarPaletteIndex == 2) || (gHealthBarPaletteIndex == 4) || ((gHealthBarPaletteIndex == 0) && (gHealthBarMode == 2))) {
                if (gControllers[i].buttonPressed & 0x8000) {
                    gCurrentHealthBar->unk_00D -= 2;
                }

                if (gControllers[i].buttonPressed & 0x4000) {
                    gCurrentHealthBar->unk_00D += 1;
                }
            }
        }
    }

    gHealthBarTimer++;
    if (gHealthBarTimer == 2) {
        switch (gHealthBarPaletteIndex) {
            case 3:
                gHealthBarPaletteIndex = 1;
                break;

            case 4:
                gHealthBarPaletteIndex = 2;
                break;
        }
    } else if ((gHealthBarTimer + 4) == gHealthBarDuration) {
        gHealthBarMode = gHealthBarPaletteIndex;

        switch (gHealthBarPaletteIndex) {
            case 1:
                gHealthBarPaletteIndex = 3;
                break;

            case 2:
                gHealthBarPaletteIndex = 4;
                break;
        }
    } else {
        if ((gHealthBarTimer + 2) == gHealthBarDuration) {
            gHealthBarPaletteIndex = 0;
        } else if (gHealthBarTimer >= gHealthBarDuration) {
            DynamoGame_PickNextCrankCue();
        }
    }
}

void DynamoGame_DrawTutorialScreen(s32 arg0) {
    u8 i;

    if ((arg0 == 0) || ((arg0 == 3) && (gDrowzeeTutorialPage == 1))) {
        Ui_DrawBorderedPanel(0x18, 0x16, 0x110, 0x90, 0x15);
        Ui_DrawBorderedPanel(0x18, 0xA5, 0x110, 0x36, 0x14B);

        gSPDisplayList(gDisplayListHead++, gUiEnvTintTextureDL);

        Widget_DrawSplitBanner(2, 0x19, 0xA6);
        Widget_DrawAnimIcon16B(0x62, 0xB0, gDynamoTutoBButtonIconAnim);
        Widget_DrawAnimIcon16(0x74, 0xB0, gDynamoTutoAButtonIconAnim);
        Widget_DrawAnimIcon16C(0x74, 0xC6, gDynamoTutoStartButtonIconAnim);

        gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);
    } else if ((arg0 == 1) || (arg0 == 2) || ((arg0 == 3) && (gDrowzeeTutorialPage == 0))) {
        s16 sp5C[] = {
            0x002B,
            0x0071,
            0x00B7,
            0x00FD,
        };

        gSPDisplayList(gDisplayListHead++, gUiEnvTintTextureDL);
        gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, 255);

        for (i = 0; i < 4; i++) {
            if (gDynamoChargeBarData[i].unk_000 == 4) {
                Widget_DrawPlayerIcon(i, sp5C[i], 0xC6, 0.75f);
            } else {
                Widget_DrawPlayerIcon(-1 - i, sp5C[i], 0xC6, 0.75f);
            }
        }

        gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);
    }

    if ((arg0 == 1) || (arg0 == 2) || ((arg0 == 3) && (gDrowzeeTutorialPage == 0))) {
        for (i = 0; i < 4; i++) {
            gCurrentHealthBar = &gDynamoChargeBarData[i];

            Widget_DrawProgressBar((i * 0x47) + 0x1C, 0xDB, 0x32, gCurrentHealthBar->unk_010, gCurrentHealthBar->unk_012 + 1);
        }
    }

    Font_BeginTranslucentTextRendering();
    Font_EnableTwoCycleTexturing();

    if ((arg0 == 0) || ((arg0 == 3) && (gDrowzeeTutorialPage == 1))) {
        s32 temp_s0;
        s32 sp48;
        s32 sp44;
        char* sp40 = Text_GetString(NULL, 0, gMinigameNamesBank, 0x2D);

        Font_SetActive(2, 0);
        Font_SetLineHeight(0xC);
        Gfx_SetEnvColor(0xFF, 0xFF, 0xFF, 0xFF);
        Gfx_SetPrimColor(0xF9, 0xF4, 0, 0xFF);
        Font_Printf(0xA0 - (Text_MeasureStringWidth(gCurrentTextObject, Text_GetString(NULL, 0, gMinigameNamesBank, 0x2C)) / 2), 0x20,
                      Text_GetString(NULL, 0, gMinigameNamesBank, 0x2C));
        Gfx_SetEnvColor(0xFF, 0xFF, 0xFF, 0xFF);
        Gfx_SetPrimColor(0xFF, 0xFF, 0xFF, 0xFF);
        Font_Printf(0x28, 0x34, sp40);

        temp_s0 = (Text_CountLines(sp40) * 0xC) + 0xC;
        Font_Printf(0x28, temp_s0 + 0x34, Text_GetString(NULL, 0, gMinigameNamesBank, 0x2E));

        temp_s0 += (Text_CountLines(Text_GetString(NULL, 0, gMinigameNamesBank, 0x2E)) * 0xC) + 0xC;
        Font_Printf(0x28, temp_s0 + 0x34, Text_GetString(NULL, 0, gMinigameNamesBank, 0x2F));

        Font_Printf(0x8B, 0xB2, Text_GetString(NULL, 0, gMinigameNamesBank, 0x5C));
        Font_Printf(0x8B, 0xC8, Text_GetString(NULL, 0, gMinigameNamesBank, 0x5C));
        Gfx_SetEnvColor(0xC8, 0xC8, 0xFF, 0xFF);
        Gfx_SetPrimColor(0x50, 0x50, 0xFF, 0xFF);
        Widget_MeasureLabelBox(&sp48, &sp44, sp40, Text_GetString(NULL, 0, gMinigameNamesBank, 0x5F), 0xC);
        Font_Printf(sp48 + 0x28, sp44 + 0x34, Text_GetString(NULL, 0, gMinigameNamesBank, 0x5F));
        Gfx_SetEnvColor(0xC8, 0xFA, 0xD2, 0xFF);
        Gfx_SetPrimColor(0x32, 0xE6, 0x82, 0xFF);
        Widget_MeasureLabelBox(&sp48, &sp44, sp40, Text_GetString(NULL, 0, gMinigameNamesBank, 0x60), 0xC);
        Font_Printf(sp48 + 0x28, sp44 + 0x34, Text_GetString(NULL, 0, gMinigameNamesBank, 0x60));
        Gfx_SetEnvColor(0xC8, 0xC8, 0xFF, 0xFF);
        Gfx_SetPrimColor(0x50, 0x50, 0xFF, 0xFF);
        Font_Printf(0xA2, 0xB2, Text_GetString(NULL, 0, gMinigameNamesBank, 0x30));
        Gfx_SetEnvColor(0xFF, 0xFF, 0xFF, 0xFF);
        Gfx_SetPrimColor(0xFF, 0x80, 0x80, 0xFF);
        Font_Printf(0xA2, 0xC8, Text_GetString(NULL, 0, gMinigameNamesBank, 0x14));
    }

    Font_DisableTwoCycleTexturing();
    Font_EndTexturedTextRendering();
}

void DynamoGame_DrawFrame(s32 arg0) {
    u8 i;

    BgStage_DrawFrame();
    GfxImage_FillCurrent(&gDisplayListHead, 0xA6BF);

    if (gResultMenuVisible == 0) {
        GeoRender_AdvanceFrameCounter();
    }

    Geo_RenderRootNode(gDrowzeeStageRoot);

    if (gResultConfirmPressed == 0) {
        DynamoGame_DrawTutorialScreen(arg0);
    }

    Widget_PauseMenuUpdate();
    BgStage_AdvanceFrame();

    if (gResultMenuVisible == 0) {
        gDrowzeeFrameCounter++;

        if (arg0 == 2) {
            if (gDrowzeeGameIsOver == 0) {
                gHealthBarUpdateCounter++;
                if (gHealthBarUpdateCounter >= 0xF) {
                    gHealthBarUpdateCounter = 0;

                    for (i = 0; i < 4; i++) {
                        gCurrentHealthBar = &gDynamoChargeBarData[i];

                        gCurrentHealthBar->unk_010 += gCurrentHealthBar->unk_00D;
                        if (gCurrentHealthBar->unk_010 < 0) {
                            gCurrentHealthBar->unk_010 = 0;
                        } else if (gCurrentHealthBar->unk_010 >= gCurrentHealthBar->unk_012) {
                            gDrowzeeGameIsOver = 1;
                        }
                        gCurrentHealthBar->unk_00E = gCurrentHealthBar->unk_00D;
                        DynamoGame_ComputeAIChargeDelta(gCurrentHealthBar, 1);
                    }

                    if (gDrowzeeGameIsOver != 0) {
                        DynamoGame_DetermineWinners();
                        DynamoGame_SyncChainLinkAnimations();
                    }
                }
            } else if (gResultTimer < 0xC4) {
                gResultTimer++;

                switch (gResultTimer) {
                    case 0x5B:
                    case 0x6F:
                    case 0x83:
                        if ((gDrowzeeWinnerCount != 4) && (gResultFlag_Special == 0)) {
                            Audio_DispatchSoundBankCommand(0x80004, 0, 0);
                            Audio_DispatchSoundBankCommand(0x80005, 0, 0);
                        }
                        break;

                    case 0x96:
                        if ((gDrowzeeWinnerCount != 4) && (gResultFlag_Special == 0)) {
                            Audio_DispatchSoundBankCommand(0x80006, 0, 0);
                        }
                        break;

                    case 0xC3:
                        Widget_PauseMenuTrigger(1);
                        break;
                }
            }
        }
    }
}

void DynamoGame_WaitForStart(void) {
    u8 i;
    u8 var_s1;
    s32 var_s2;

    var_s1 = 0;
    var_s2 = 1;

    StageLoader_RunFrames(5);
    StageFade_StartFromOpaque(0x10);

    while (var_s2 != 0) {
        if (var_s1 >= 0x11) {
            if (gPlayer1Controller->buttonPressed & 0x1000) {
                var_s2 = 0;
            } else if ((gMenuCursorX == 0) && (gPlayer1Controller->buttonPressed & 0x4000)) {
                Audio_PlaySoundEffectById(SFX_MENU_BACK);
                Widget_PauseMenuTrigger(2);
                gDrowzeeTutorialPage = 1;
                return;
            }
        }

        MiniGame_ReadInputs();
        DynamoGame_UpdateCountdownAndCue(0);
        DynamoGame_UpdatePlayerAnimState();
        DynamoGame_DrawFrame(0);

        if (var_s1 < 0x11) {
            var_s1++;
        }
    }

    Audio_FadeOutAll(0x3C);

    if (gResultFlag_Special == 0) {
        while (true) {
            if (gDrowzeeStartCountdown <= 0) {
                break;
            }

            for (i = 3; i > 0; i--) {
                s32 tmp = (i * 0x3C) / 2;
                if (tmp == gDrowzeeStartCountdown) {
                    if (i == 3) {
                        Widget_CountdownStart(1);
                    }
                    Audio_DispatchSoundBankCommand(0x80007, 0, 0);
                }
            }

            gDrowzeeStartCountdown--;

            MiniGame_ReadInputs();
            DynamoGame_UpdateCountdownAndCue(1);
            DynamoGame_UpdatePlayerAnimState();
            DynamoGame_DrawFrame(1);
        }
    }
}

void DynamoGame_MainLoop(void) {
    static u8 D_86502E7C = 0;

    s32 var_s0 = 1;

    if (gResultFlag_Special == 0) {
        DynamoGame_PickNextCrankCue();
        Audio_DispatchSoundBankCommand(0x80008, 0, 0);
        MiniGame_ReadInputs();
        DynamoGame_UpdateCountdownAndCue(2);
        DynamoGame_UpdatePlayerAnimState();
        DynamoGame_DrawFrame(2);

        while (var_s0 != 0) {
            if (gResultFlag_Special != 0) {
                break;
            }

            if ((D_86502E7C == 0) && (Widget_CountdownGetState() == -2)) {
                Audio_StartMusicTrack(0x1D);
                D_86502E7C = 1;
            }

            MiniGame_ReadInputs();
            DynamoGame_UpdateCountdownAndCue(2);

            if (gDrowzeeGameIsOver == 0) {
                DynamoGame_ReadPlayerCrankInput();
            } else if (gResultExitPending != 0) {
                var_s0 = 0;
            }

            DynamoGame_DrawSparkEffects(2);
            DynamoGame_UpdatePlayerAnimState();
            DynamoGame_DrawFrame(2);
        }
    }
}

void DynamoGame_ShowResultText(void) {
    u8 i;

    Audio_FadeCategories(1, 0x1E);
    Audio_FadeCategories(2, 0x1E);
    StageFade_StartFromTransparent(0x1E);

    for (i = 0; i < 30; i++) {
        MiniGame_ReadInputs();
        DynamoGame_UpdateCountdownAndCue(3);
        DynamoGame_DrawSparkEffects(3);
        DynamoGame_UpdatePlayerAnimState();
        DynamoGame_DrawFrame(3);
    }
}

void DynamoGame_InitCamera(void) {
    gDrowzeeCamera = gDrowzeeStageRoot->node.children;

    gDrowzeeCamDist = -0x200;
    gDrowzeeCamRotationY = 0;
    gDrowzeeCamDistance = 0xBE;

    gDrowzeeCamera->perspective.nearPlane = 50.0f;
    gDrowzeeCamera->perspective.farPlane = 6400.0f;
    gDrowzeeCamera->perspective.verticalFieldOfView = 40.0f;

    gDrowzeeCamera->view.targetPosition.y = 51.0f;

    Camera_ComputeEyeFromAngles(&gDrowzeeCamera->view.targetPosition, &gDrowzeeCamera->view.eyePosition, gDrowzeeCamDistance, gDrowzeeCamDist, gDrowzeeCamRotationY);
}

void DynamoGame_LoadAssets(void) {
    u8 j;
    u8 i;
    MemoryBlock* temp_v0;

    temp_v0 = MainPool_AllocState(main_pool_get_available(), 0);
    gDrowzeeStageRoot = process_geo_layout(temp_v0, gDynamoStageGeoLayoutData);
    MainPool_FinalizeAllocation(temp_v0);
    ModelRenderer_InitDisplayRoots();
    PokeIcon_OpenModelArchives();
    DynamoGame_InitPlayerSlots();
    DynamoGame_AssignTeams();

    for (i = 0; i < 2; i++) {
        if (i == 0) {
            gDrowzeeModelTemp = Model_LoadByArchiveIndex(0xC5);
        } else {
            gDrowzeeModelTemp = Model_LoadByArchiveIndex(0xC6);
        }

        for (j = 0; j < 4; j++) {
            if (i == gDynamoChargeBarData[j].unk_004) {
                gCurrentHealthBar = &gDynamoChargeBarData[j];
                gCurrentHealthBarNode = &gCurrentHealthBar->unk_014;

                ModelRenderer_AttachDisplayObject(gCurrentHealthBarNode);

                if (i == 0) {
                    Model_InitDisplayObject(gCurrentHealthBarNode, 0, 0xC5, gDrowzeeModelTemp->unk_08->unk_00[0]);
                } else {
                    Model_InitDisplayObject(gCurrentHealthBarNode, 0, 0xC6, gDrowzeeModelTemp->unk_08->unk_00[0]);
                }

                gCurrentHealthBar->unk_008 = 0;
                ModelAnim_SetAnimation(gCurrentHealthBarNode, gCurrentHealthBar->unk_008);
                ModelAnim_SetFrame(gCurrentHealthBarNode, MiniGame_RandomRange(gCurrentHealthBarNode->transformAnim.data->endFrame));

                gCurrentHealthBarNode->position.x = gHealthBarPositions[j].unk_00;
                gCurrentHealthBarNode->position.y = gHealthBarPositions[j].unk_04;

                if (i == 0) {
                    gCurrentHealthBarNode->position.y += 0.15f;
                }

                gCurrentHealthBarNode->position.z = gHealthBarPositions[j].unk_08;

                gCurrentHealthBarNode->scale.x = gHealthBarPositions[j].unk_0C;
                gCurrentHealthBarNode->scale.y = gHealthBarPositions[j].unk_10;
                gCurrentHealthBarNode->scale.z = gHealthBarPositions[j].unk_14;

                gCurrentHealthBarNode->rotation.x = gHealthBarPositions[j].unk_18;
                gCurrentHealthBarNode->rotation.y = gHealthBarPositions[j].unk_1A;
                gCurrentHealthBarNode->rotation.z = gHealthBarPositions[j].unk_1C;

                gCurrentHealthBar->unk_00C = 0;
                gCurrentHealthBar->unk_00E = 0;

                DynamoGame_ComputeAIChargeDelta(gCurrentHealthBar, 0);

                gCurrentHealthBar->unk_010 = 0;
                gCurrentHealthBar->unk_012 = 0x28;
            }
        }
    }

    {
        u16 sp5C[] = {
            0x00C9, 0x00C8, 0x00C8, 0x00C8, 0x00C8, 0x00CA, 0x00CB, 0x00CC, 0x00CD, 0x00CE, 0x00CF, 0x00D0, 0x00D1,
        };

        for (j = 0; j < 13; j++) {
            if ((j != 2) && (j != 3) && (j != 4)) {
                gDrowzeeModelTemp = Model_LoadByArchiveIndex(sp5C[j]);
            }

            gCurrentHealthBarNode = gHealthBarGraphNodePtrs[j];

            ModelRenderer_AttachDisplayObject(gCurrentHealthBarNode);
            Model_InitDisplayObject(gCurrentHealthBarNode, 0, sp5C[j], gDrowzeeModelTemp->unk_08->unk_00[0]);
            ModelAnim_SetAnimation(gCurrentHealthBarNode, 0);
            ModelAnim_SetFrame(gCurrentHealthBarNode, gCurrentHealthBarNode->transformAnim.data->endFrame);
        }
    }

    DynamoGame_InitCrankIconDisplay();
    DynamoGame_InitSparkEffects();
    DynamoGame_InitCamera();
    Widget_CountdownInit();
    Widget_PauseMenuInit();
    Widget_DrawAnimIcon16(0, 0, NULL);
    Widget_DrawAnimIcon16B(0, 0, NULL);
}

void DynamoGame_Main(UNUSED s32 arg0, UNUSED s32 arg1) {
    StageContext* sp24;

    main_pool_push_state('MINI');

    Gfx_InitDisplayListBuffers(0x18000, 0);
    sp24 = StageContext_Allocate(0, 1, 3, 1, 2, 1);
    gCurrentTextObject = Font_Init(0x16, 0);

    ASSET_LOAD(D_1000000, common_menu1_ui, 0);
    FRAGMENT_LOAD(FRAGMENT_PARTICLE_DATA_LIBRARY);

    DynamoGame_LoadAssets();
    StageContext_Activate(sp24);
    DynamoGame_WaitForStart();
    DynamoGame_MainLoop();
    DynamoGame_ShowResultText();
    StageContext_Deactivate();
    Font_Free();
    Gfx_FreeDisplayListBuffers();

    main_pool_pop_state('MINI');

    Widget_PauseMenuGetResult();
}

static s32 pad_D_86502E98 = 0;

s8 DynamoGame_RollAIChargeBias(s32 arg0, s32 arg1) {
    static u16 sAiChargeBiasWeightsNormal[4][8] = {
        { 0x0001, 0x0001, 0x0001, 0x0032, 0x0032, 0x0005, 0x0000, 0x0000 },
        { 0x0000, 0x0001, 0x0001, 0x0005, 0x0032, 0x0032, 0x0001, 0x0000 },
        { 0x0000, 0x0000, 0x0001, 0x0001, 0x0005, 0x005A, 0x001E, 0x0005 },
        { 0x0000, 0x0000, 0x0000, 0x0000, 0x0005, 0x001E, 0x005A, 0x000A },
    };

    static u16 sAiChargeBiasWeightsExtended[4][13] = {
        { 0x0003, 0x0006, 0x0014, 0x0014, 0x0014, 0x0014, 0x000A, 0x0005, 0x0003, 0x0001, 0x0000, 0x0000, 0x0000 },
        { 0x0003, 0x0006, 0x0014, 0x0014, 0x0014, 0x0014, 0x000A, 0x000A, 0x0005, 0x0003, 0x0001, 0x0000, 0x0000 },
        { 0x0003, 0x0006, 0x0014, 0x0014, 0x0014, 0x0014, 0x000A, 0x000A, 0x000A, 0x0007, 0x0005, 0x0003, 0x0001 },
        { 0x0000, 0x0003, 0x0006, 0x000A, 0x0014, 0x0014, 0x0014, 0x0014, 0x000A, 0x000A, 0x0007, 0x0005, 0x0003 },
    };

    u8 i;
    u16 var_a0;
    u16 var_a2;
    u16* sp24;
    s8 sp23;
    u8 sp22;
    u16 temp_v0;

    if (arg1 == 0) {
        sp24 = sAiChargeBiasWeightsNormal[arg0];
        sp23 = -1;
        sp22 = 8;
    } else if (arg1 == 1) {
        sp24 = sAiChargeBiasWeightsExtended[arg0];
        sp23 = -8;
        sp22 = 0xD;
    }

    var_a0 = 0;

    for (i = 0; i < sp22; i++) {
        var_a0 += sp24[i];
    }

    temp_v0 = MiniGame_RandomRange(var_a0);

    var_a2 = 0;
    for (i = 0; i < sp22; i++) {
        if ((temp_v0 >= var_a2) && (temp_v0 < (sp24[i] + var_a2))) {
            return i + sp23;
        }
        var_a2 += sp24[i];
    }

    return 0;
}
