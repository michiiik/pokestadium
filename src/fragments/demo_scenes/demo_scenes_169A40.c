#include "demo_scenes.h"
#include "src/geo_render.h"
#include "src/model_animation.h"
#include "src/geo_layout.h"
#include "src/display_object_textures.h"
#include "src/model_renderer.h"
#include "src/graphics_textures.h"
#include "src/input.h"
#include "src/save_data.h"
#include "src/jpeg_stream.h"
#include "src/gfx_buffer.h"
#include "src/gfx_rect.h"
#include "src/DDC0.h"
#include "src/matrix.h"
#include "src/controller.h"
#include "src/memory.h"
#include "src/stage_loader.h"

static u32 gStageGeoLayoutCommands[] = {
    0x0C00FFFF, 0x05000000, 0x0B00001E,  0x00000000, 0x014000F0, 0x00000032, 0x00000000, 0x00000118,
    0x05000000, 0x0D000000, 0x05000000,  0x14000000, 0x002B0012, 0xFFFFFF32, 0x16FFFFFF, 0x0F000003,
    0x05000000, 0x0A000000, &gGraphRoot0, 0x06000000, 0x06000000, 0x06000000, 0x06000000, 0x01000000,
};

static ModelData* gStageGeoLayout;
static GraphNode* gStageRootGraphNode;
static DisplayObject gStageRootFragmentNode;
static CameraNode* gStageCamera;
static BinArchive* gMewtwoIntroBgArchive;
static u8* gBackgroundTextureData;
static u8 gStageFadeAlpha;
static ModeSettings gSessionModeSettings;

void MewtwoIntro_DrawOverlay(void) {
}

void MewtwoIntro_InitCamera(void) {
    gStageCamera = gStageRootGraphNode->children;
    Vec3f_SetComponentsDuplicate(&gStageCamera->view.targetPosition, 0.0f, 0.0f, 0.0f);
    Vec3f_SetComponentsDuplicate(&gStageCamera->view.eyePosition, 0.0f, 0.0f, 200.0f);
    gStageCamera->perspective.nearPlane = 16.0f;
    gStageCamera->perspective.farPlane = 32768.0f;
    gStageCamera->perspective.verticalFieldOfView = 45.0f;
}

void MewtwoIntro_ReadControllerInput(void) {
    Cont_StartReadInputs();
    Cont_ReadInputs();
    Input_ResetRepeatState();
}

void MewtwoIntro_DrawBackgroundTiles(u8* arg0) {
    s32 i;
    s32 j;

    if (1) {}

    gDPPipeSync(gDisplayListHead++);

    gDPSetCycleType(gDisplayListHead++, G_CYC_1CYCLE);
    gDPSetRenderMode(gDisplayListHead++, G_RM_OPA_SURF, G_RM_OPA_SURF2);
    gDPSetTexturePersp(gDisplayListHead++, G_TP_NONE);
    gDPSetCombineMode(gDisplayListHead++, G_CC_DECALRGB, G_CC_DECALRGB);

    gDPPipeSync(gDisplayListHead++);

    for (i = 0; i < 15; i++) {
        for (j = 0; j < 20; j++, arg0 += 0x200) {
            Gfx_DrawTextureRgba16(j * 0x10, i * 0x10, 0x10, 0x10, arg0, 0x10, 0);
        }
    }

    gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);
}

void MewtwoIntro_RenderFrame(void) {
    BgStage_DrawFrame();
    MewtwoIntro_DrawBackgroundTiles(gBackgroundTextureData);
    GfxImage_ClearDepthRectangle(&gDisplayListHead, 0, 0, 0x140, 0xF0);
    GeoRender_AdvanceFrameCounter();
    Geo_RenderRootNode(gStageRootGraphNode);
    MewtwoIntro_DrawOverlay();
    BgStage_AdvanceFrame();
}

s32 MewtwoIntro_UpdateFade(void) {
    s32 var_v0;

    if ((gStageRootFragmentNode.transformAnim.frameFixed >> 0x10) >= 0x5F) {
        Model_SetMaterialColor(&gStageRootFragmentNode, 0, 0, 0, gStageFadeAlpha += 0x14);
    }

    if ((gStageRootFragmentNode.transformAnim.frameFixed >> 0x10) == 0x64) {
        StageContext_SetClearColor(1);
        StageFade_StartFromTransparent(3);
    }

    var_v0 = ModelAnim_IsAnimationDone(&gStageRootFragmentNode) != 0;
    if (var_v0) {
        return 1;
    }
}

void MewtwoIntro_RunLoop(void) {
    s32 var_s0 = 1;

    StageFade_StartFromOpaque(0xC);
    Audio_PlayMusicIfChanged(0x30);

    while (var_s0 != 0) {
        MewtwoIntro_ReadControllerInput();
        if (MewtwoIntro_UpdateFade() != 0) {
            var_s0 = 0;
        }
        MewtwoIntro_RenderFrame();
    }

    StageLoader_RunFrames(2);
}

void MewtwoIntro_InitScene(void) {
    MemoryBlock* sp1C = MainPool_AllocState(main_pool_get_available(), 0);

    gStageRootGraphNode = process_geo_layout(sp1C, &gStageGeoLayoutCommands);

    MainPool_FinalizeAllocation(sp1C);
    ModelRenderer_InitDisplayRoots();
    MewtwoIntro_InitCamera();
    ModelRenderer_AttachDisplayObject(&gStageRootFragmentNode);
    PokeIcon_OpenModelArchives();

    gStageGeoLayout = Model_LoadByArchiveIndex(0xD6);
    Model_InitDisplayObject(&gStageRootFragmentNode, 0, 0xD6, gStageGeoLayout->unk_08->unk_00[0]);
    ModelAnim_SetAnimation(&gStageRootFragmentNode, 1);
    Vec3f_SetComponentsDuplicate(&gStageRootFragmentNode.position, 0.0f, 0.0f, 0.0f);
    gStageFadeAlpha = 0;
}

s32 Stage_ShowMewtwoIntro(void) {
    StageContext* sp24;

    main_pool_push_state('MTWO');

    Gfx_InitDisplayListBuffers(0x10000, 0);
    sp24 = StageContext_Allocate(0, 1, 3, 1, 2, 1);
    Font_Init(6, 0);

    ASSET_LOAD(D_1000000, common_menu1_ui, 0);
    FRAGMENT_LOAD(FRAGMENT_PARTICLE_DATA_LIBRARY);

    Save_EnsureBankLoaded(2);
    Save_GetModeSettings(&gSessionModeSettings, gSessionContext.inRound2);

    if (gSessionContext.inRound2 == 1) {
        gMewtwoIntroBgArchive = ASSET_LOAD2(backgrounds, 1, 1);
        gBackgroundTextureData = BinArchive_GetFile(gMewtwoIntroBgArchive, 2);
    } else if (gSessionModeSettings.clearedModes & 0x100) {
        gMewtwoIntroBgArchive = ASSET_LOAD2(backgrounds, 1, 1);
        gBackgroundTextureData = BinArchive_GetFile(gMewtwoIntroBgArchive, 1);
    } else {
        gMewtwoIntroBgArchive = ASSET_LOAD2(backgrounds, 1, 1);
        gBackgroundTextureData = BinArchive_GetFile(gMewtwoIntroBgArchive, 2);
    }

    MewtwoIntro_InitScene();
    StageContext_Activate(sp24);
    MewtwoIntro_RunLoop();
    StageContext_Deactivate();
    Font_Free();
    Gfx_FreeDisplayListBuffers();

    main_pool_pop_state('MTWO');

    return 0;
}
