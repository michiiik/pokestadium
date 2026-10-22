#include "gallery_rental_viewer.h"
#include "src/poke_icon.h"
#include "src/graphics_textures.h"
#include "src/input.h"
#include "src/pokemon_stats.h"
#include "src/text_system.h"
#include "src/jpeg_stream.h"
#include "src/gfx_buffer.h"
#include "src/controller.h"
#include "src/fragments/minigame_select/minigame_select.h"
#include "src/memory.h"
#include "src/stage_loader.h"

static s32 gSnapSelectMode;
static s32 gSnapSelectState;
static void* gSnapSelectBackground;
static RentalViewerBgData* gSnapSelectRosterData;
static s32 gRentalViewerConfirmResult;
static s32 gRentalViewerSelectedSlot;
static s32 gRentalViewerFadeCounter;

void Gallery_RentalViewerInit(s32 arg0) {
    gSnapSelectState = 0;
    gSnapSelectMode = arg0;
    gRentalViewerFadeCounter = -1;
    gRentalViewerConfirmResult = -1;
    Text_InitStringTables();
    Gallery_InitStringTables();
    Gallery_RentalViewerInitScene();
    Gallery_RentalViewerInitBoxPicker(0x68, 0x8A, gSnapSelectRosterData);
    Gallery_ResetConfirmPrompt();
}

void Gallery_RentalViewerSetFadeCounter(s32 arg0) {
    gRentalViewerFadeCounter = arg0;
}

void Gallery_RentalViewerDrawIcon(s32 arg0, s32 arg1) {
    Gfx_DrawTextureRgba16(arg0, arg1, 0x40, 0x18, gRentalViewerIconTop, 0x40, 0);
    Gfx_DrawTextureRgba16(arg0, arg1 + 0x18, 0x40, 0x18, D_4000C00, 0x40, 0);
}

void Gallery_RentalViewerDrawTitleBar(void) {
    char* sp2C = Gallery_GetUiString(4);
    Color_RGBA8 sp28 = { 0x9A, 0x37, 0x54, 0xFF };
    Color_RGBA8 sp24 = { 0xC3, 0x54, 0x84, 0xFF };

    Gallery_DrawHeaderBar(0x48, 0x28, 0x208, &sp28, &sp24);

    gSPDisplayList(gDisplayListHead++, gUiEnvTintTextureDL);

    Gallery_RentalViewerDrawIcon(0x28, 0x20);
    Font_BeginTranslucentTextRendering();
    Font_SetActive(0x10, 0);
    Gfx_SetEnvColor(0, 0, 0, 0xFF);
    Font_Printf(0x7A, 0x2D, sp2C);
    Gfx_SetEnvColor(0xFF, 0xFF, 0x77, 0xFF);
    Font_Printf(0x78, 0x2B, sp2C);
    Font_EndTexturedTextRendering();
}

void Gallery_RentalViewerDrawFrame(void) {
    BgStage_DrawFrame();
    if (gRentalViewerFadeCounter != 0) {
        Gfx_DrawTiledRgba16Image(gSnapSelectBackground);
        Gallery_RentalViewerDrawTitleBar();
    }
    Gallery_RentalViewerDrawTeamList(1);
    SnapSelect_UpdateCameraState(1);
    Gallery_DrawConfirmPrompt(1);
    BgStage_AdvanceFrame();
    if (gRentalViewerFadeCounter > 0) {
        gRentalViewerFadeCounter -= 1;
    }
}

void Gallery_RentalViewerLoadPort(s32 arg0) {
    gSnapSelectState = 2;
    Gallery_RentalViewerSetFadeCounter(0xC);

    main_pool_push_state('glpc');

    if ((gRentalViewerSelectedSlot >= 0) && (gRentalViewerSelectedSlot < 4)) {
        GbSave_LoadPort(gRentalViewerSelectedSlot);
    }

    if (arg0 != 0) {
        Gallery_RentalViewerOpenBoxPicker(gRentalViewerSelectedSlot, &gGalleryFinalSelection.unk_08);
    } else {
        Gallery_RentalViewerOpenBoxPicker(gRentalViewerSelectedSlot, NULL);
    }
}

void Gallery_RentalViewerUpdateLoadConfirm(void) {
    gRentalViewerConfirmResult = Gallery_GetConfirmPromptResult(2);

    if (gRentalViewerConfirmResult != -1) {
        if (gRentalViewerConfirmResult == -2) {
            gRentalViewerConfirmResult = 2;
        }

        if (gRentalViewerConfirmResult != 2) {
            gSnapSelectState = 4;
            StageFade_StartFromTransparent(0xF);
        } else {
            gSnapSelectState = 1;
            Gallery_RentalViewerBeginZoomIntro(gRentalViewerSelectedSlot);
            gRentalViewerConfirmResult = -1;
        }
    }
}

s32 Gallery_RentalViewerHandleInput(void) {
    static s32 gRentalViewerInputDelay = 4;

    s32 sp7C = 1;
    s32 sp78 = 0;
    s32 sp74;
    BattleMon sp20;

    switch (gSnapSelectState) {
        case 0:
            if (StageContext_GetFadeMode() == 0) {
                gRentalViewerInputDelay--;
                if (gRentalViewerInputDelay <= 0) {
                    gSnapSelectState = 1;
                    if (gSnapSelectMode == 1) {
                        gRentalViewerSelectedSlot = gGalleryFinalSelection.unk_08.unk_00;
                        Gallery_RentalViewerLoadPort(1);
                    } else {
                        if (gSnapSelectMode == 2) {
                            gRentalViewerSelectedSlot = 5;
                        } else {
                            gRentalViewerSelectedSlot = -1;
                        }
                        Gallery_RentalViewerBeginZoomIntro(gRentalViewerSelectedSlot);
                    }
                }
            }
            break;

        case 1:
            gRentalViewerSelectedSlot = Gallery_RentalViewerGetSelectedSlot();
            switch (gRentalViewerSelectedSlot) {
                case -2:
                    if (gGalleryPhotoPoolCount > 0) {
                        gSnapSelectState = 3;
                        Gallery_OpenConfirmPrompt(2, gPlayer1Controller);
                    } else {
                        sp78 = 1;
                    }
                    break;

                case 0:
                case 1:
                case 2:
                case 3:
                case 4:
                    Gallery_RentalViewerLoadPort(0);
                    break;

                case 5:
                    sp78 = 1;
                    _bcopy(&gSnapSelectRosterData->unk_7E4, &sp20, sizeof(BattleMon));
                    Gallery_CopyPhotoMonFromBattleMon(&gGallerySpecialSpecies, &sp20);
                    break;
            }

            if (sp78 != 0) {
                gSnapSelectState = 4;
                StageFade_StartFromTransparent(0xF);
            }
            break;

        case 2:
            sp74 = Gallery_RentalViewerGetLoadState();
            if (sp74 != 0) {
                main_pool_pop_state('glpc');

                gRentalViewerFadeCounter = -1;
                if (sp74 > 0) {
                    gSnapSelectState = 4;
                    StageFade_StartFromTransparent(0xF);
                } else {
                    gSnapSelectState = 1;
                    Gallery_RentalViewerBeginZoomIntro(gRentalViewerSelectedSlot);
                }
            }
            break;

        case 3:
            Gallery_RentalViewerUpdateLoadConfirm();
            break;

        case 4:
            if (StageContext_GetFadeMode() == 1) {
                sp7C = 0;
            }
            break;
    }
    return sp7C;
}

void Gallery_RentalViewerPollInput(void) {
    Cont_StartReadInputs();
    Cont_ReadInputs();
    Input_ResetRepeatState();
}

void Gallery_RentalViewerLoop(void) {
    StageFade_StartFromOpaque(7);
    do {
        Gallery_RentalViewerPollInput();
        Gallery_RentalViewerUpdate();
        Gallery_RentalViewerUpdateBoxPicker();
        Gallery_UpdateConfirmPrompt();
        Gallery_RentalViewerDrawFrame();
    } while (Gallery_RentalViewerHandleInput() != 0);
}

s32 Gallery_RentalViewerGetResult(void) {
    s32 var_v1;
    GallerySelection* temp_a0 = &gGalleryFinalSelection.unk_08;
    temp_a0 += 0;

    if (gRentalViewerConfirmResult == 0) {
        var_v1 = 2;
    } else if (gRentalViewerConfirmResult == 1) {
        var_v1 = 0;
    } else {
        switch (gRentalViewerSelectedSlot) {
            case -2:
                var_v1 = 0;
                break;

            case 5:
                var_v1 = 6;
                break;

            default:
                var_v1 = 1;
                break;
        }
    }

    temp_a0->unk_00 = gRentalViewerSelectedSlot;
    Gallery_RentalViewerSaveBoxCursor(temp_a0);
    return var_v1;
}

s32 Gallery_RentalViewerMain(s32 arg0, s32 arg1) {
    StageContext* sp24;

    main_pool_push_state('SLCT');

    Gfx_InitDisplayListBuffers(0x20000, 0);
    sp24 = StageContext_Allocate(1, 0, 2, 0, 2, 1);
    gTextObjectHandle = Font_Init(0x1C, 0);

    ASSET_LOAD(D_1000000, common_menu1_ui, 0);
    ASSET_LOAD(D_2000000, common_menu2_ui, 0);
    ASSET_LOAD(D_3000000, gallery_ui, 0);
    ASSET_LOAD(gRentalViewerIconTop, snap_select_ui, 0);

    gSnapSelectBackground = BinArchive_GetFile(ASSET_LOAD2(backgrounds, 1, 1), 0x14);
    gSnapSelectRosterData = BinArchive_GetFile(BinArchive_Open(TRAINER_DATA_ROM_START, NULL, 1, 0), 0x1A);

    PokeIcon_OpenModelArchive();
    Gallery_RentalViewerInit(arg0);
    StageContext_Activate(sp24);
    Gallery_RentalViewerLoop();
    StageContext_Deactivate();
    Font_Free();
    Gfx_FreeDisplayListBuffers();

    main_pool_pop_state('SLCT');

    return Gallery_RentalViewerGetResult();
}
