#include "gallery_enlarge.h"
#include "src/display_object_textures.h"
#include "src/graphics_textures.h"
#include "src/input.h"
#include "src/gallery.h"
#include "src/jpeg_stream.h"
#include "src/audio_sfx.h"
#include "src/gfx_buffer.h"
#include "src/controller.h"
#include "src/fragments/gallery_backgrounds/gallery_backgrounds.h"
#include "src/memory.h"
#include "src/stage_loader.h"

static s32 gFragment46_State;
static BinArchive* gStadiumModelsArchive;
static StageContext* gFragment46_Stage;

static Camera* gFragment46_PhotoScene = NULL;

void Gallery_EnlargeInit(void) {
    UNUSED s32 pad;
    ModelLoadContext* sp28;

    gFragment46_State = 0;
    sp28 = PokeIcon_AllocFramebuffers(1);
    Gallery_ResetActiveScene();
    gFragment46_PhotoScene = Geo_CreateSceneInstance(0x280, 0x1E0, NULL, gFragment46_Stage->renderTargets[0], sp28, gStadiumModelsArchive, Gallery_GetEnlargeTarget(1));
}

void Gallery_EnlargeEmptyHandler(void) {
}

void Gallery_EnlargeDraw(void) {
    static s16 gFragment46_FrameCounter = 0;

    BgStage_DrawFrame();
    if (gFragment46_PhotoScene->unk_00 == 2) {
        Gallery_DrawPhotoThumbnail(gFragment46_PhotoScene->unk_08->img_p, 0, 0, gFragment46_PhotoScene->unk_04, gFragment46_PhotoScene->unk_06, 1, 0);
    }
    BgStage_AdvanceFrame();
    gFragment46_FrameCounter += 0x1000;
}

s32 Gallery_EnlargeCheckExitInput(void) {
    s32 sp1C = 0;

    if (Gallery_FindReadyScene(&gFragment46_PhotoScene, 1) != -1) {
        return 0;
    }

    if (BTN_IS_PRESSED(gPlayer1Controller, BTN_A | BTN_B | BTN_CDOWN)) {
        sp1C = 1;
    }

    return sp1C;
}

s32 Gallery_EnlargeUpdate(void) {
    static s32 gFragment46_FadeDelay = 2;

    s32 sp1C = 1;

    switch (gFragment46_State) {
        case 0:
            if (StageContext_GetFadeMode() == 0) {
                gFragment46_FadeDelay--;
                if (gFragment46_FadeDelay <= 0) {
                    gFragment46_State = 1;
                }
            }
            break;

        case 1:
            if (Gallery_EnlargeCheckExitInput() != 0) {
                gFragment46_State = 2;
                Audio_PlaySoundEffectById(SFX_ZOOM_OUT);
                StageFade_StartFromTransparent(1);
            }
            break;

        case 2:
            if (StageContext_GetFadeMode() == 1) {
                sp1C = 0;
            }
            break;
    }
    return sp1C;
}

void Gallery_EnlargeReadInput(void) {
    Cont_StartReadInputs();
    Cont_ReadInputs();
    Input_ResetRepeatState();
}

void Gallery_EnlargeLoop(void) {
    do {
        Gallery_ProcessSceneInstance(gFragment46_PhotoScene);
    } while (gFragment46_PhotoScene->unk_00 != 2);

    StageFade_StartFromOpaque(1);

    do {
        Gallery_EnlargeReadInput();
        Gallery_EnlargeDraw();
    } while (Gallery_EnlargeUpdate() != 0);
}

s32 Gallery_EnlargeMain(UNUSED s32 arg0, UNUSED s32 arg1) {
    StageContext* sp24;

    main_pool_push_state('ELRG');

    Gfx_InitDisplayListBuffers(0x10000, 0);

    sp24 = StageContext_Allocate(1, 0, 1, 0, 2, 1);
    gFragment46_Stage = sp24;

    Font_Init(0x18, 0);

    ASSET_LOAD(D_1000000, common_menu1_ui, 0);
    ASSET_LOAD(D_5000000, snap_mode_ui, 0);
    ASSET_LOAD(D_3000000, gallery_ui, 0);
    ASSET_LOAD(gRentalViewerIconTop, album_ui, 0);

    FRAGMENT_LOAD(FRAGMENT_PARTICLE_DATA_LIBRARY);

    PokeIcon_OpenModelArchives();

    gStadiumModelsArchive = ASSET_LOAD2(stadium_models, 1, 1);

    Gallery_EnlargeInit();
    StageContext_Activate(sp24);
    Gallery_EnlargeLoop();
    StageContext_Deactivate();
    Font_Free();
    Gfx_FreeDisplayListBuffers();

    main_pool_pop_state('ELRG');

    return 0;
}
